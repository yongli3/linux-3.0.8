/* linux/arch/arm/mach-s5pv210/include/mach/gpio.h
 *
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com/
 *
 * S5PV210 - GPIO lib support
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __ASM_ARCH_GPIO_H
#define __ASM_ARCH_GPIO_H __FILE__

#define gpio_get_value	__gpio_get_value
#define gpio_set_value	__gpio_set_value
#define gpio_cansleep	__gpio_cansleep
#define gpio_to_irq	__gpio_to_irq

/* Practically, GPIO banks up to MP03 are the configurable gpio banks */

/* GPIO bank sizes */
#define S5PV210_GPIO_A0_NR	(8)
#define S5PV210_GPIO_A1_NR	(4)
#define S5PV210_GPIO_B_NR	(8)
#define S5PV210_GPIO_C0_NR	(5)
#define S5PV210_GPIO_C1_NR	(5)
#define S5PV210_GPIO_D0_NR	(4)
#define S5PV210_GPIO_D1_NR	(6)
#define S5PV210_GPIO_E0_NR	(8)
#define S5PV210_GPIO_E1_NR	(5)
#define S5PV210_GPIO_F0_NR	(8)
#define S5PV210_GPIO_F1_NR	(8)
#define S5PV210_GPIO_F2_NR	(8)
#define S5PV210_GPIO_F3_NR	(6)
#define S5PV210_GPIO_G0_NR	(7)
#define S5PV210_GPIO_G1_NR	(7)
#define S5PV210_GPIO_G2_NR	(7)
#define S5PV210_GPIO_G3_NR	(7)
#define S5PV210_GPIO_H0_NR	(8)
#define S5PV210_GPIO_H1_NR	(8)
#define S5PV210_GPIO_H2_NR	(8)
#define S5PV210_GPIO_H3_NR	(8)
#define S5PV210_GPIO_I_NR	(7)
#define S5PV210_GPIO_J0_NR	(8)
#define S5PV210_GPIO_J1_NR	(6)
#define S5PV210_GPIO_J2_NR	(8)
#define S5PV210_GPIO_J3_NR	(8)
#define S5PV210_GPIO_J4_NR	(5)

#define S5PV210_GPIO_MP01_NR	(8)
#define S5PV210_GPIO_MP02_NR	(4)
#define S5PV210_GPIO_MP03_NR	(8)
#define S5PV210_GPIO_MP04_NR	(8)
#define S5PV210_GPIO_MP05_NR	(8)
#define S5PV210_GPIO_MP06_NR	(8)
#define S5PV210_GPIO_MP07_NR	(8)

#define S5PV210_GPIO_MP10_NR	(8)
#define S5PV210_GPIO_MP11_NR	(8)
#define S5PV210_GPIO_MP12_NR	(8)
#define S5PV210_GPIO_MP13_NR	(8)
#define S5PV210_GPIO_MP14_NR	(8)
#define S5PV210_GPIO_MP15_NR	(8)
#define S5PV210_GPIO_MP16_NR	(8)
#define S5PV210_GPIO_MP17_NR	(8)
#define S5PV210_GPIO_MP18_NR	(7)

#define S5PV210_GPIO_MP20_NR	(8)
#define S5PV210_GPIO_MP21_NR	(8)
#define S5PV210_GPIO_MP22_NR	(8)
#define S5PV210_GPIO_MP23_NR	(8)
#define S5PV210_GPIO_MP24_NR	(8)
#define S5PV210_GPIO_MP25_NR	(8)
#define S5PV210_GPIO_MP26_NR	(8)
#define S5PV210_GPIO_MP27_NR	(8)
#define S5PV210_GPIO_MP28_NR	(7)

#define S5PV210_GPIO_ETC0_NR	(6)
#define S5PV210_GPIO_ETC1_NR	(8)
#define S5PV210_GPIO_ETC2_NR	(8)
#define S5PV210_GPIO_ETC4_NR	(6)


/* GPIO bank numbers */

/* CONFIG_S3C_GPIO_SPACE allows the user to select extra
 * space for debugging purposes so that any accidental
 * change from one gpio bank to another can be caught.
*/

#define S5PV210_GPIO_NEXT(__gpio) \
	((__gpio##_START) + (__gpio##_NR) + CONFIG_S3C_GPIO_SPACE + 1)

enum s5p_gpio_number {
	S5PV210_GPIO_A0_START	= 0,
	S5PV210_GPIO_A1_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_A0),
	S5PV210_GPIO_B_START 	= S5PV210_GPIO_NEXT(S5PV210_GPIO_A1),
	S5PV210_GPIO_C0_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_B),
	S5PV210_GPIO_C1_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_C0),
	S5PV210_GPIO_D0_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_C1),
	S5PV210_GPIO_D1_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_D0),
	S5PV210_GPIO_E0_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_D1),
	S5PV210_GPIO_E1_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_E0),
	S5PV210_GPIO_F0_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_E1),
	S5PV210_GPIO_F1_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_F0),
	S5PV210_GPIO_F2_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_F1),
	S5PV210_GPIO_F3_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_F2),
	S5PV210_GPIO_G0_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_F3),
	S5PV210_GPIO_G1_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_G0),
	S5PV210_GPIO_G2_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_G1),
	S5PV210_GPIO_G3_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_G2),
	S5PV210_GPIO_H0_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_G3),
	S5PV210_GPIO_H1_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_H0),
	S5PV210_GPIO_H2_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_H1),
	S5PV210_GPIO_H3_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_H2),
	S5PV210_GPIO_I_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_H3),
	S5PV210_GPIO_J0_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_I),
	S5PV210_GPIO_J1_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_J0),
	S5PV210_GPIO_J2_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_J1),
	S5PV210_GPIO_J3_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_J2),
	S5PV210_GPIO_J4_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_J3),
	S5PV210_GPIO_MP01_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_J4),
	S5PV210_GPIO_MP02_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_MP01),
	S5PV210_GPIO_MP03_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_MP02),
	S5PV210_GPIO_MP04_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_MP03),
	S5PV210_GPIO_MP05_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_MP04),
	S5PV210_GPIO_MP06_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_MP05),
	S5PV210_GPIO_MP07_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_MP06),
	S5PV210_GPIO_MP10_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_MP07),
	S5PV210_GPIO_MP11_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_MP10),
	S5PV210_GPIO_MP12_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_MP11),
	S5PV210_GPIO_MP13_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_MP12),
	S5PV210_GPIO_MP14_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_MP13),
	S5PV210_GPIO_MP15_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_MP14),
	S5PV210_GPIO_MP16_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_MP15),
	S5PV210_GPIO_MP17_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_MP16),
	S5PV210_GPIO_MP18_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_MP17),
	S5PV210_GPIO_MP20_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_MP18),
	S5PV210_GPIO_MP21_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_MP20),
	S5PV210_GPIO_MP22_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_MP21),
	S5PV210_GPIO_MP23_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_MP22),
	S5PV210_GPIO_MP24_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_MP23),
	S5PV210_GPIO_MP25_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_MP24),
	S5PV210_GPIO_MP26_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_MP25),
	S5PV210_GPIO_MP27_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_MP26),
	S5PV210_GPIO_MP28_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_MP27),
	S5PV210_GPIO_ETC0_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_MP28),
	S5PV210_GPIO_ETC1_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_ETC0),
	S5PV210_GPIO_ETC2_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_ETC1),
	S5PV210_GPIO_ETC4_START	= S5PV210_GPIO_NEXT(S5PV210_GPIO_ETC2),
};

/* S5PV210 GPIO number definitions */
#define S5PV210_GPA0(_nr)	(S5PV210_GPIO_A0_START + (_nr))
#define S5PV210_GPA1(_nr)	(S5PV210_GPIO_A1_START + (_nr))
#define S5PV210_GPB(_nr)	(S5PV210_GPIO_B_START + (_nr))
#define S5PV210_GPC0(_nr)	(S5PV210_GPIO_C0_START + (_nr))
#define S5PV210_GPC1(_nr)	(S5PV210_GPIO_C1_START + (_nr))
#define S5PV210_GPD0(_nr)	(S5PV210_GPIO_D0_START + (_nr))
#define S5PV210_GPD1(_nr)	(S5PV210_GPIO_D1_START + (_nr))
#define S5PV210_GPE0(_nr)	(S5PV210_GPIO_E0_START + (_nr))
#define S5PV210_GPE1(_nr)	(S5PV210_GPIO_E1_START + (_nr))
#define S5PV210_GPF0(_nr)	(S5PV210_GPIO_F0_START + (_nr))
#define S5PV210_GPF1(_nr)	(S5PV210_GPIO_F1_START + (_nr))
#define S5PV210_GPF2(_nr)	(S5PV210_GPIO_F2_START + (_nr))
#define S5PV210_GPF3(_nr)	(S5PV210_GPIO_F3_START + (_nr))
#define S5PV210_GPG0(_nr)	(S5PV210_GPIO_G0_START + (_nr))
#define S5PV210_GPG1(_nr)	(S5PV210_GPIO_G1_START + (_nr))
#define S5PV210_GPG2(_nr)	(S5PV210_GPIO_G2_START + (_nr))
#define S5PV210_GPG3(_nr)	(S5PV210_GPIO_G3_START + (_nr))
#define S5PV210_GPH0(_nr)	(S5PV210_GPIO_H0_START + (_nr))
#define S5PV210_GPH1(_nr)	(S5PV210_GPIO_H1_START + (_nr))
#define S5PV210_GPH2(_nr)	(S5PV210_GPIO_H2_START + (_nr))
#define S5PV210_GPH3(_nr)	(S5PV210_GPIO_H3_START + (_nr))
#define S5PV210_GPI(_nr)	(S5PV210_GPIO_I_START + (_nr))
#define S5PV210_GPJ0(_nr)	(S5PV210_GPIO_J0_START + (_nr))
#define S5PV210_GPJ1(_nr)	(S5PV210_GPIO_J1_START + (_nr))
#define S5PV210_GPJ2(_nr)	(S5PV210_GPIO_J2_START + (_nr))
#define S5PV210_GPJ3(_nr)	(S5PV210_GPIO_J3_START + (_nr))
#define S5PV210_GPJ4(_nr)	(S5PV210_GPIO_J4_START + (_nr))
#define S5PV210_MP01(_nr)	(S5PV210_GPIO_MP01_START + (_nr))
#define S5PV210_MP02(_nr)	(S5PV210_GPIO_MP02_START + (_nr))
#define S5PV210_MP03(_nr)	(S5PV210_GPIO_MP03_START + (_nr))
#define S5PV210_MP04(_nr)	(S5PV210_GPIO_MP04_START + (_nr))
#define S5PV210_MP05(_nr)	(S5PV210_GPIO_MP05_START + (_nr))
#define S5PV210_MP06(_nr)	(S5PV210_GPIO_MP06_START + (_nr))
#define S5PV210_MP07(_nr)	(S5PV210_GPIO_MP07_START + (_nr))
#define S5PV210_MP10(_nr)	(S5PV210_GPIO_MP10_START + (_nr))
#define S5PV210_MP11(_nr)	(S5PV210_GPIO_MP11_START + (_nr))
#define S5PV210_MP12(_nr)	(S5PV210_GPIO_MP12_START + (_nr))
#define S5PV210_MP13(_nr)	(S5PV210_GPIO_MP13_START + (_nr))
#define S5PV210_MP14(_nr)	(S5PV210_GPIO_MP14_START + (_nr))
#define S5PV210_MP15(_nr)	(S5PV210_GPIO_MP15_START + (_nr))
#define S5PV210_MP16(_nr)	(S5PV210_GPIO_MP16_START + (_nr))
#define S5PV210_MP17(_nr)	(S5PV210_GPIO_MP17_START + (_nr))
#define S5PV210_MP18(_nr)	(S5PV210_GPIO_MP18_START + (_nr))
#define S5PV210_MP20(_nr)	(S5PV210_GPIO_MP20_START + (_nr))
#define S5PV210_MP21(_nr)	(S5PV210_GPIO_MP21_START + (_nr))
#define S5PV210_MP22(_nr)	(S5PV210_GPIO_MP22_START + (_nr))
#define S5PV210_MP23(_nr)	(S5PV210_GPIO_MP23_START + (_nr))
#define S5PV210_MP24(_nr)	(S5PV210_GPIO_MP24_START + (_nr))
#define S5PV210_MP25(_nr)	(S5PV210_GPIO_MP25_START + (_nr))
#define S5PV210_MP26(_nr)	(S5PV210_GPIO_MP26_START + (_nr))
#define S5PV210_MP27(_nr)	(S5PV210_GPIO_MP27_START + (_nr))
#define S5PV210_MP28(_nr)	(S5PV210_GPIO_MP28_START + (_nr))
#define S5PV210_ETC0(_nr)	(S5PV210_GPIO_ETC0_START + (_nr))
#define S5PV210_ETC1(_nr)	(S5PV210_GPIO_ETC1_START + (_nr))
#define S5PV210_ETC2(_nr)	(S5PV210_GPIO_ETC2_START + (_nr))
#define S5PV210_ETC4(_nr)	(S5PV210_GPIO_ETC4_START + (_nr))

/* Define EXT INT GPIO */
#define S5P_EXT_INT0(x)		S5PV210_GPH0(x)
#define S5P_EXT_INT1(x)		S5PV210_GPH1(x)
#define S5P_EXT_INT2(x)		S5PV210_GPH2(x)
#define S5P_EXT_INT3(x)		S5PV210_GPH3(x)

/* the end of the S5PV210 specific gpios */
#define S5PV210_GPIO_END	(S5PV210_ETC4(S5PV210_GPIO_ETC4_NR) + 1)
#define S3C_GPIO_END		S5PV210_GPIO_END

/* define the number of gpios we need to the one after the GPJ4() range */
#define ARCH_NR_GPIOS		(S5PV210_ETC4(S5PV210_GPIO_ETC4_NR) +	\
				 CONFIG_SAMSUNG_GPIO_EXTRA + 1)

#include <asm-generic/gpio.h>

#include <plat/gpio-cfg.h>

extern int s3c_gpio_slp_cfgpin(unsigned int pin, unsigned int to);
extern s3c_gpio_pull_t s3c_gpio_get_slp_cfgpin(unsigned int pin);

#define S3C_GPIO_SLP_OUT0       ((__force s3c_gpio_pull_t)0x00)
#define S3C_GPIO_SLP_OUT1       ((__force s3c_gpio_pull_t)0x01)
#define S3C_GPIO_SLP_INPUT      ((__force s3c_gpio_pull_t)0x02)
#define S3C_GPIO_SLP_PREV       ((__force s3c_gpio_pull_t)0x03)

extern int s3c_gpio_set_drvstrength(unsigned int pin, unsigned int config);
extern int s3c_gpio_set_slewrate(unsigned int pin, unsigned int config);

#define S3C_GPIO_DRVSTR_1X      (0)
#define S3C_GPIO_DRVSTR_2X      (1)
#define S3C_GPIO_DRVSTR_3X      (2)
#define S3C_GPIO_DRVSTR_4X      (3)

#define S3C_GPIO_SLEWRATE_FAST  (0)
#define S3C_GPIO_SLEWRATE_SLOW  (1)

extern int s3c_gpio_slp_setpull_updown(unsigned int pin, s3c_gpio_pull_t pull);
extern int s5pv210_gpiolib_init(void);

#endif /* __ASM_ARCH_GPIO_H */
