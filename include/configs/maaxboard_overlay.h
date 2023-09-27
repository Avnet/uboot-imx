/*
 * Copyright 2022 AVNET
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __MAAXBOARD_OVERLAY_H
#define __MAAXBOARD_OVERLAY_H

#define   AVNET_UENV_FDTO_SUPPORT

#define FDT_APPLY_OVERLAY()          \
    "echo Applying DT overlay => ${dtbo_file}; " \
	"fatload mmc ${mmcdev}:${mmcpart} ${dtbo_addr} ${dtbo_dir}/${dtbo_file}; " \
	"fdt addr ${fdt_addr_r}; " \
	"fdt resize ${fdt_size}; " \
	"fdt apply ${dtbo_addr}; "

#define CHECK_APPLY_OVERLAY_VAL( name )     \
	"if env exists dtoverlay_" name " ; then " \
		"setenv dtbo_file " name "-${dtoverlay_" name "}.dtbo; " \
		FDT_APPLY_OVERLAY() \
	"fi; "

#define CHECK_APPLY_OVERLAY_EXT( name )     \
	"if env exists dtoverlay_" name " && test ${dtoverlay_" name "} = 1 -o ${dtoverlay_" name "} = yes ; then " \
		"setenv dtbo_file ext-" name ".dtbo; " \
		FDT_APPLY_OVERLAY() \
	"fi; "

#define CHECK_APPLY_OVERLAY_IDX( name )     \
	"if env exists dtoverlay_" name "; then " \
		"for i in ${dtoverlay_" name "}; do " \
			"setenv dtbo_file ext-" name "$i.dtbo; " \
			FDT_APPLY_OVERLAY() \
		" done;" \
	"fi; "

#define CHECK_APPLY_OVERLAY_DTBO( name )     \
	"if env exists dtoverlay_" name "; then " \
		"for f in ${dtoverlay_" name "}; do " \
			"setenv dtbo_file $f; " \
			FDT_APPLY_OVERLAY() \
		" done;" \
	"fi; "

#define FDT_ENTRY_DEF_SETTINGS          \
				CHECK_APPLY_OVERLAY_VAL("camera") \
				CHECK_APPLY_OVERLAY_VAL("display") \
				CHECK_APPLY_OVERLAY_VAL("usb0") \
				CHECK_APPLY_OVERLAY_EXT("gpio") \
				CHECK_APPLY_OVERLAY_EXT("wm8960") \
				CHECK_APPLY_OVERLAY_IDX("i2c") \
				CHECK_APPLY_OVERLAY_IDX("pwm") \
				CHECK_APPLY_OVERLAY_IDX("spi") \
				CHECK_APPLY_OVERLAY_IDX("uart") \
				CHECK_APPLY_OVERLAY_DTBO("extra") \


#define MMC_BOOT_WITH_FDT_OVERLAY   \
	"mmc dev ${mmcdev};"            \
	"run loadenvconf; run mmcargs;" \
	"run loadimage; run loadfdt; "  \
	FDT_ENTRY_DEF_SETTINGS          \
	"run boot_os; "                 \

#endif  /* __MAAXBOARD_OVERLAY_H */
