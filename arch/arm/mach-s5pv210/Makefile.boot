   zreladdr-y	:= 0x20008000
params_phys-y	:= 0x20000100

zreladdr-$(CONFIG_MACH_SMDKC110) := 0x30008000
params_phys-$(CONFIG_MACH_SMDKC110)      := 0x30000100

zreladdr-$(CONFIG_MACH_SMDKV210)	:= 0x30008000
params_phys-$(CONFIG_MACH_SMDKV210)	:= 0x20000100

