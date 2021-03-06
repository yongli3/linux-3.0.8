/* linux/drivers/media/video/s5k4ba.c
 *
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 * 		http://www.samsung.com/
 *
 * Driver for S5K4BA (UXGA camera) from Samsung Electronics
 * 1/4" 2.0Mp CMOS Image Sensor SoC with an Embedded Image Processor
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <media/v4l2-device.h>
#include <media/v4l2-subdev.h>
#include <media/v4l2-i2c-drv.h>
#include <media/s5k4ba_platform.h>

#ifdef CONFIG_VIDEO_SAMSUNG_V4L2
#include <linux/videodev2_samsung.h>
#endif
#include "s5k4ba.h"

#define S5K4BA_DRIVER_NAME	"S5K4BA"

/* Default resolution & pixelformat. plz ref s5k4ba_platform.h */
#define DEFAULT_RES		WVGA	/* Index of resoultion */
#define DEFAUT_FPS_INDEX	S5K4BA_15FPS
#define DEFAULT_FMT		V4L2_PIX_FMT_UYVY	/* YUV422 */

/*
 * Specification
 * Parallel : ITU-R. 656/601 YUV422, RGB565, RGB888 (Up to VGA), RAW10
 * Serial : MIPI CSI2 (single lane) YUV422, RGB565, RGB888 (Up to VGA), RAW10
 * Resolution : 1280 (H) x 1024 (V)
 * Image control : Brightness, Contrast, Saturation, Sharpness, Glamour
 * Effect : Mono, Negative, Sepia, Aqua, Sketch
 * FPS : 15fps @full resolution, 30fps @VGA, 24fps @720p
 * Max. pixel clock frequency : 48MHz(upto)
 * Internal PLL (6MHz to 27MHz input frequency)
 */

static int s5k4ba_init(struct v4l2_subdev *sd, u32 val);
/* Camera functional setting values configured by user concept */
struct s5k4ba_userset {
	signed int exposure_bias;	/* V4L2_CID_EXPOSURE */
	unsigned int ae_lock;
	unsigned int awb_lock;
	unsigned int auto_wb;	/* V4L2_CID_CAMERA_WHITE_BALANCE */
	unsigned int manual_wb;	/* V4L2_CID_WHITE_BALANCE_PRESET */
	unsigned int wb_temp;	/* V4L2_CID_WHITE_BALANCE_TEMPERATURE */
	unsigned int effect;	/* Color FX (AKA Color tone) */
	unsigned int contrast;	/* V4L2_CID_CONTRAST */
	unsigned int saturation;	/* V4L2_CID_SATURATION */
	unsigned int sharpness;		/* V4L2_CID_SHARPNESS */
	unsigned int glamour;
};

struct s5k4ba_state {
	struct s5k4ba_platform_data *pdata;
	struct v4l2_subdev sd;
	struct v4l2_pix_format pix;
	struct v4l2_fract timeperframe;
	struct s5k4ba_userset userset;
	int framesize_index;
	int freq;	/* MCLK in KHz */
	int is_mipi;
	int isize;
	int ver;
	int fps;
	int check_previewdata;
};

enum {
	S5K4BA_PREVIEW_VGA,
};

struct s5k4ba_enum_framesize {
	unsigned int index;
	unsigned int width;
	unsigned int height;
};

struct s5k4ba_enum_framesize s5k4ba_framesize_list[] = {
	{ S5K4BA_PREVIEW_VGA, 640, 480 }
};

static inline struct s5k4ba_state *to_state(struct v4l2_subdev *sd)
{
	return container_of(sd, struct s5k4ba_state, sd);
}

static int s5k4ba_reset(struct v4l2_subdev *sd)
{
	return s5k4ba_init(sd, 0);
}

/*
 * S5K4BA register structure : 2bytes address, 2bytes value
 * retry on write failure up-to 5 times
 */
static inline int s5k4ba_write(struct v4l2_subdev *sd, u8 addr, u8 val)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct i2c_msg msg[1];
	unsigned char reg[2];
	int err = 0;
	int retry = 0;


	if (!client->adapter)
		return -ENODEV;

again:
	msg->addr = client->addr;
	msg->flags = 0;
	msg->len = 2;
	msg->buf = reg;

	reg[0] = addr & 0xff;
	reg[1] = val & 0xff;

	err = i2c_transfer(client->adapter, msg, 1);
	if (err >= 0)
		return err;	/* Returns here on success */

	/* abnormal case: retry 5 times */
	if (retry < 5) {
		dev_err(&client->dev, "%s: address: 0x%02x"
			"value: 0x%02x\n", __func__,
			reg[0], reg[1]);
		retry++;
		goto again;
	}

	return err;
}

static int s5k4ba_i2c_write(struct v4l2_subdev *sd, unsigned char i2c_data[],
				unsigned char length)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	unsigned char buf[length], i;
	struct i2c_msg msg = {client->addr, 0, length, buf};

	for (i = 0; i < length; i++)
		buf[i] = i2c_data[i];

	return i2c_transfer(client->adapter, &msg, 1) == 1 ? 0 : -EIO;
}

static int s5k4ba_write_regs(struct v4l2_subdev *sd, unsigned char regs[],
				int size)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int i, err;

	for (i = 0; i < size; i++) {
		err = s5k4ba_i2c_write(sd, &regs[i], sizeof(regs[i]));
		if (err < 0)
			v4l_info(client, "%s: register set failed\n", __func__);
	}

	return 0;
}

static const char *s5k4ba_querymenu_wb_preset[] = {
	"WB Tungsten", "WB Fluorescent", "WB sunny", "WB cloudy", NULL
};

static const char *s5k4ba_querymenu_effect_mode[] = {
	"Effect Sepia", "Effect Aqua", "Effect Monochrome",
	"Effect Negative", "Effect Sketch", NULL
};

static const char *s5k4ba_querymenu_ev_bias_mode[] = {
	"-3EV",	"-2,1/2EV", "-2EV", "-1,1/2EV",
	"-1EV", "-1/2EV", "0", "1/2EV",
	"1EV", "1,1/2EV", "2EV", "2,1/2EV",
	"3EV", NULL
};

static struct v4l2_queryctrl s5k4ba_controls[] = {
	{
		/*
		 * For now, we just support in preset type
		 * to be close to generic WB system,
		 * we define color temp range for each preset
		 */
		.id = V4L2_CID_WHITE_BALANCE_TEMPERATURE,
		.type = V4L2_CTRL_TYPE_INTEGER,
		.name = "White balance in kelvin",
		.minimum = 0,
		.maximum = 10000,
		.step = 1,
		.default_value = 0,	/* FIXME */
	},
	{
		.id = V4L2_CID_WHITE_BALANCE_PRESET,
		.type = V4L2_CTRL_TYPE_MENU,
		.name = "White balance preset",
		.minimum = 0,
		.maximum = ARRAY_SIZE(s5k4ba_querymenu_wb_preset) - 2,
		.step = 1,
		.default_value = 0,
	},
	{
		.id = V4L2_CID_CAMERA_WHITE_BALANCE,
		.type = V4L2_CTRL_TYPE_BOOLEAN,
		.name = "Auto white balance",
		.minimum = 0,
		.maximum = 1,
		.step = 1,
		.default_value = 0,
	},
	{
		.id = V4L2_CID_EXPOSURE,
		.type = V4L2_CTRL_TYPE_MENU,
		.name = "Exposure bias",
		.minimum = 0,
		.maximum = ARRAY_SIZE(s5k4ba_querymenu_ev_bias_mode) - 2,
		.step = 1,
		.default_value =
			(ARRAY_SIZE(s5k4ba_querymenu_ev_bias_mode) - 2) / 2,
			/* 0 EV */
	},
	{
		.id = V4L2_CID_CAMERA_EFFECT,
		.type = V4L2_CTRL_TYPE_MENU,
		.name = "Image Effect",
		.minimum = 0,
		.maximum = ARRAY_SIZE(s5k4ba_querymenu_effect_mode) - 2,
		.step = 1,
		.default_value = 0,
	},
	{
		.id = V4L2_CID_CAMERA_CONTRAST,
		.type = V4L2_CTRL_TYPE_INTEGER,
		.name = "Contrast",
		.minimum = 0,
		.maximum = 4,
		.step = 1,
		.default_value = 2,
	},
	{
		.id = V4L2_CID_CAMERA_SATURATION,
		.type = V4L2_CTRL_TYPE_INTEGER,
		.name = "Saturation",
		.minimum = 0,
		.maximum = 4,
		.step = 1,
		.default_value = 2,
	},
	{
		.id = V4L2_CID_CAMERA_SHARPNESS,
		.type = V4L2_CTRL_TYPE_INTEGER,
		.name = "Sharpness",
		.minimum = 0,
		.maximum = 4,
		.step = 1,
		.default_value = 2,
	},
};

const char * const *s5k4ba_ctrl_get_menu(u32 id)
{
	switch (id) {
	case V4L2_CID_WHITE_BALANCE_PRESET:
		return s5k4ba_querymenu_wb_preset;

	case V4L2_CID_CAMERA_EFFECT:
		return s5k4ba_querymenu_effect_mode;

	case V4L2_CID_EXPOSURE:
		return s5k4ba_querymenu_ev_bias_mode;

	default:
		return v4l2_ctrl_get_menu(id);
	}
}

static inline struct v4l2_queryctrl const *s5k4ba_find_qctrl(int id)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(s5k4ba_controls); i++)
		if (s5k4ba_controls[i].id == id)
			return &s5k4ba_controls[i];

	return NULL;
}

static int s5k4ba_queryctrl(struct v4l2_subdev *sd, struct v4l2_queryctrl *qc)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(s5k4ba_controls); i++) {
		if (s5k4ba_controls[i].id == qc->id) {
			memcpy(qc, &s5k4ba_controls[i],
				sizeof(struct v4l2_queryctrl));
			return 0;
		}
	}

	return -EINVAL;
}

static int s5k4ba_querymenu(struct v4l2_subdev *sd, struct v4l2_querymenu *qm)
{
	struct v4l2_queryctrl qctrl;

	qctrl.id = qm->id;
	s5k4ba_queryctrl(sd, &qctrl);

	return v4l2_ctrl_query_menu(qm, &qctrl, s5k4ba_ctrl_get_menu(qm->id));
}

static int s5k4ba_g_fmt(struct v4l2_subdev *sd, struct v4l2_format *fmt)
{
	int err = 0;

	return err;
}

static int s5k4ba_s_fmt(struct v4l2_subdev *sd, struct v4l2_format *fmt)
{
	int err = 0;

	return err;
}

static int s5k4ba_enum_framesizes(struct v4l2_subdev *sd,
					struct v4l2_frmsizeenum *fsize)
{
	struct s5k4ba_state *state = to_state(sd);
	int num_entries = sizeof(s5k4ba_framesize_list) /
				sizeof(struct s5k4ba_enum_framesize);
	struct s5k4ba_enum_framesize *elem;
	int index = 0;
	int i = 0;

	/* The camera interface should read this value, this is the resolution
	 * at which the sensor would provide framedata to the camera i/f
	 *
	 * In case of image capture,
	 * this returns the default camera resolution (WVGA)
	 */
	fsize->type = V4L2_FRMSIZE_TYPE_DISCRETE;

	index = state->framesize_index;

	for (i = 0; i < num_entries; i++) {
		elem = &s5k4ba_framesize_list[i];
		if (elem->index == index) {
			fsize->discrete.width =
			    s5k4ba_framesize_list[index].width;
			fsize->discrete.height =
			    s5k4ba_framesize_list[index].height;
			return 0;
		}
	}

	return -EINVAL;
}

static int s5k4ba_enum_frameintervals(struct v4l2_subdev *sd,
					struct v4l2_frmivalenum *fival)
{
	int err = 0;

	return err;
}

static int s5k4ba_enum_fmt(struct v4l2_subdev *sd, struct v4l2_fmtdesc *fmtdesc)
{
	int err = 0;

	return err;
}

static int s5k4ba_try_fmt(struct v4l2_subdev *sd, struct v4l2_format *fmt)
{
	int err = 0;

	return err;
}

static int s5k4ba_g_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *param)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int err = 0;

	dev_dbg(&client->dev, "%s\n", __func__);

	return err;
}

static int s5k4ba_s_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *param)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int err = 0;

	dev_dbg(&client->dev, "%s: numerator %d, denominator: %d\n",
		__func__, param->parm.capture.timeperframe.numerator,
		param->parm.capture.timeperframe.denominator);

	return err;
}

static int s5k4ba_g_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct s5k4ba_state *state = to_state(sd);
	struct s5k4ba_userset userset = state->userset;
	int err = 0;

	switch (ctrl->id) {
	case V4L2_CID_CAMERA_WHITE_BALANCE:
		ctrl->value = userset.auto_wb;
		break;
	case V4L2_CID_WHITE_BALANCE_PRESET:
		ctrl->value = userset.manual_wb;
		break;
	case V4L2_CID_CAMERA_EFFECT:
		ctrl->value = userset.effect;
		break;
	case V4L2_CID_CAMERA_CONTRAST:
		ctrl->value = userset.contrast;
		break;
	case V4L2_CID_CAMERA_SATURATION:
		ctrl->value = userset.saturation;
		break;
	case V4L2_CID_CAMERA_SHARPNESS:
		ctrl->value = userset.saturation;
		break;
	case V4L2_CID_CAM_JPEG_MAIN_SIZE:
	case V4L2_CID_CAM_JPEG_MAIN_OFFSET:
	case V4L2_CID_CAM_JPEG_THUMB_SIZE:
	case V4L2_CID_CAM_JPEG_THUMB_OFFSET:
	case V4L2_CID_CAM_JPEG_POSTVIEW_OFFSET:
	case V4L2_CID_CAM_JPEG_MEMSIZE:
	case V4L2_CID_CAM_JPEG_QUALITY:
	case V4L2_CID_CAMERA_AUTO_FOCUS_RESULT_FIRST:
	case V4L2_CID_CAM_DATE_INFO_YEAR:
	case V4L2_CID_CAM_DATE_INFO_MONTH:
	case V4L2_CID_CAM_DATE_INFO_DATE:
	case V4L2_CID_CAM_SENSOR_VER:
	case V4L2_CID_CAM_FW_MINOR_VER:
	case V4L2_CID_CAM_FW_MAJOR_VER:
	case V4L2_CID_CAM_PRM_MINOR_VER:
	case V4L2_CID_CAM_PRM_MAJOR_VER:
	case V4L2_CID_ESD_INT:
	case V4L2_CID_CAMERA_GET_ISO:
	case V4L2_CID_CAMERA_GET_SHT_TIME:
	case V4L2_CID_CAMERA_OBJ_TRACKING_STATUS:
	case V4L2_CID_CAMERA_SMART_AUTO_STATUS:
		ctrl->value = 0;
		break;
	case V4L2_CID_EXPOSURE:
		ctrl->value = userset.exposure_bias;
		break;
	default:
		dev_err(&client->dev, "%s: no such ctrl\n", __func__);
		/* err = -EINVAL; */
		break;
	}

	return err;
}

static int s5k4ba_s_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct s5k4ba_state *state = to_state(sd);
	int err = 0;
	int value = ctrl->value;

	switch (ctrl->id) {

	case V4L2_CID_CAMERA_FLASH_MODE:
	case V4L2_CID_CAMERA_BRIGHTNESS:
		break;
	case V4L2_CID_CAMERA_WHITE_BALANCE:
		dev_dbg(&client->dev, "%s: V4L2_CID_CAMERA_WHITE_BALANCE\n",
			__func__);

		if (value <= WHITE_BALANCE_AUTO) {
			err = s5k4ba_write_regs(sd,
			(unsigned char *) s5k4ba_regs_awb_enable[value],
				sizeof(s5k4ba_regs_awb_enable[value]));
		} else {
			err = s5k4ba_write_regs(sd,
			(unsigned char *) s5k4ba_regs_wb_preset[value-2],
				sizeof(s5k4ba_regs_wb_preset[value-2]));
		}
		break;
	case V4L2_CID_CAMERA_EFFECT:
		dev_dbg(&client->dev, "%s: V4L2_CID_CAMERA_EFFECT\n", __func__);
		err = s5k4ba_write_regs(sd,
		(unsigned char *) s5k4ba_regs_color_effect[value-1],
			sizeof(s5k4ba_regs_color_effect[value-1]));
		break;
	case V4L2_CID_CAMERA_ISO:
	case V4L2_CID_CAMERA_METERING:
		break;
	case V4L2_CID_CAMERA_CONTRAST:
		dev_dbg(&client->dev, "%s: V4L2_CID_CAMERA_CONTRAST\n", __func__);
		err = s5k4ba_write_regs(sd,
		(unsigned char *) s5k4ba_regs_contrast_bias[value],
			sizeof(s5k4ba_regs_contrast_bias[value]));
		break;
	case V4L2_CID_CAMERA_SATURATION:
		dev_dbg(&client->dev, "%s: V4L2_CID_CAMERA_SATURATION\n", __func__);
		err = s5k4ba_write_regs(sd,
		(unsigned char *) s5k4ba_regs_saturation_bias[value],
			sizeof(s5k4ba_regs_saturation_bias[value]));
		break;
	case V4L2_CID_CAMERA_SHARPNESS:
		dev_dbg(&client->dev, "%s: V4L2_CID_CAMERA_SHARPNESS\n", __func__);
		err = s5k4ba_write_regs(sd,
		(unsigned char *) s5k4ba_regs_sharpness_bias[value],
			sizeof(s5k4ba_regs_sharpness_bias[value]));
		break;
	case V4L2_CID_CAMERA_WDR:
	case V4L2_CID_CAMERA_FACE_DETECTION:
	case V4L2_CID_CAMERA_FOCUS_MODE:
	case V4L2_CID_CAM_JPEG_QUALITY:
	case V4L2_CID_CAMERA_SCENE_MODE:
	case V4L2_CID_CAMERA_GPS_LATITUDE:
	case V4L2_CID_CAMERA_GPS_LONGITUDE:
	case V4L2_CID_CAMERA_GPS_TIMESTAMP:
	case V4L2_CID_CAMERA_GPS_ALTITUDE:
	case V4L2_CID_CAMERA_OBJECT_POSITION_X:
	case V4L2_CID_CAMERA_OBJECT_POSITION_Y:
	case V4L2_CID_CAMERA_SET_AUTO_FOCUS:
	case V4L2_CID_CAMERA_FRAME_RATE:
		break;
	case V4L2_CID_CAM_PREVIEW_ONOFF:
		if (state->check_previewdata == 0)
			err = 0;
		else
			err = -EIO;
		break;
	case V4L2_CID_CAMERA_CHECK_DATALINE:
	case V4L2_CID_CAMERA_CHECK_DATALINE_STOP:
		break;
	case V4L2_CID_CAMERA_RESET:
		dev_dbg(&client->dev, "%s: V4L2_CID_CAMERA_RESET\n", __func__);
		err = s5k4ba_reset(sd);
		break;
	case V4L2_CID_EXPOSURE:
		dev_dbg(&client->dev, "%s: V4L2_CID_EXPOSURE\n", __func__);
		err = s5k4ba_write_regs(sd,
		(unsigned char *) s5k4ba_regs_ev_bias[value],
			sizeof(s5k4ba_regs_ev_bias[value]));
		break;
	default:
		dev_err(&client->dev, "%s: no such control\n", __func__);
		/* err = -EINVAL; */
		break;
	}

	if (err < 0)
		dev_dbg(&client->dev, "%s: vidioc_s_ctrl failed\n", __func__);

	return err;
}

static int s5k4ba_init(struct v4l2_subdev *sd, u32 val)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct s5k4ba_state *state = to_state(sd);
	int err = -EINVAL, i;

	v4l_info(client, "%s: camera initialization start\n", __func__);

	for (i = 0; i < S5K4BA_INIT_REGS; i++) {
		err = s5k4ba_i2c_write(sd, s5k4ba_init_reg[i],
					sizeof(s5k4ba_init_reg[i]));
		if (err < 0)
			v4l_info(client, "%s: register set failed\n", __func__);
	}

	if (err < 0) {
		/* This is preview fail */
		state->check_previewdata = 100;
		v4l_err(client,
			"%s: camera initialization failed. err(%d)\n",
			__func__, state->check_previewdata);
		return -EIO;
	}

	/* This is preview success */
	state->check_previewdata = 0;
	return 0;
}

/*
 * s_config subdev ops
 * With camera device, we need to re-initialize every single opening time
 * therefor, it is not necessary to be initialized on probe time.
 * except for version checking.
 * NOTE: version checking is optional
 */
static int s5k4ba_s_config(struct v4l2_subdev *sd, int irq, void *platform_data)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct s5k4ba_state *state = to_state(sd);
	struct s5k4ba_platform_data *pdata;

	dev_info(&client->dev, "fetching platform data\n");

	pdata = client->dev.platform_data;

	if (!pdata) {
		dev_err(&client->dev, "%s: no platform data\n", __func__);
		return -ENODEV;
	}

	/*
	 * Assign default format and resolution
	 * Use configured default information in platform data
	 * or without them, use default information in driver
	 */
	if (!(pdata->default_width && pdata->default_height)) {
		/* TODO: assign driver default resolution */
	} else {
		state->pix.width = pdata->default_width;
		state->pix.height = pdata->default_height;
	}

	if (!pdata->pixelformat)
		state->pix.pixelformat = DEFAULT_FMT;
	else
		state->pix.pixelformat = pdata->pixelformat;

	if (!pdata->freq)
		state->freq = 24000000;	/* 24MHz default */
	else
		state->freq = pdata->freq;

	if (!pdata->is_mipi) {
		state->is_mipi = 0;
		dev_info(&client->dev, "parallel mode\n");
	} else
		state->is_mipi = pdata->is_mipi;

	return 0;
}

static const struct v4l2_subdev_core_ops s5k4ba_core_ops = {
	.init = s5k4ba_init,	/* initializing API */
	.s_config = s5k4ba_s_config,	/* Fetch platform data */
	.queryctrl = s5k4ba_queryctrl,
	.querymenu = s5k4ba_querymenu,
	.g_ctrl = s5k4ba_g_ctrl,
	.s_ctrl = s5k4ba_s_ctrl,
};

static const struct v4l2_subdev_video_ops s5k4ba_video_ops = {
	.g_fmt = s5k4ba_g_fmt,
	.s_fmt = s5k4ba_s_fmt,
	.enum_framesizes = s5k4ba_enum_framesizes,
	.enum_frameintervals = s5k4ba_enum_frameintervals,
	.enum_fmt = s5k4ba_enum_fmt,
	.try_fmt = s5k4ba_try_fmt,
	.g_parm = s5k4ba_g_parm,
	.s_parm = s5k4ba_s_parm,
};

static const struct v4l2_subdev_ops s5k4ba_ops = {
	.core = &s5k4ba_core_ops,
	.video = &s5k4ba_video_ops,
};

/*
 * s5k4ba_probe
 * Fetching platform data is being done with s_config subdev call.
 * In probe routine, we just register subdev device
 */
static int s5k4ba_probe(struct i2c_client *client,
			 const struct i2c_device_id *id)
{
	struct s5k4ba_state *state;
	struct v4l2_subdev *sd;

	state = kzalloc(sizeof(struct s5k4ba_state), GFP_KERNEL);
	if (state == NULL)
		return -ENOMEM;

	sd = &state->sd;
	strcpy(sd->name, S5K4BA_DRIVER_NAME);

	/* Registering subdev */
	v4l2_i2c_subdev_init(sd, client, &s5k4ba_ops);

	dev_info(&client->dev, "s5k4ba has been probed\n");
	return 0;
}


static int s5k4ba_remove(struct i2c_client *client)
{
	struct v4l2_subdev *sd = i2c_get_clientdata(client);

	v4l2_device_unregister_subdev(sd);
	kfree(to_state(sd));
	return 0;
}

static const struct i2c_device_id s5k4ba_id[] = {
	{ S5K4BA_DRIVER_NAME, 0 },
	{ },
};
MODULE_DEVICE_TABLE(i2c, s5k4ba_id);

static struct v4l2_i2c_driver_data v4l2_i2c_data = {
	.name = S5K4BA_DRIVER_NAME,
	.probe = s5k4ba_probe,
	.remove = __devexit_p(s5k4ba_remove),
	.id_table = s5k4ba_id,
};

MODULE_DESCRIPTION("Samsung Electronics S5K4BA UXGA camera driver");
MODULE_AUTHOR("Jinsung Yang <jsgood.yang@samsung.com>");
MODULE_LICENSE("GPL");

