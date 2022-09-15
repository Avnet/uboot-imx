/*
 * Copyright 2022 AVNET
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __AVNET_FDT_OVERLAY_H
#define __AVNET_FDT_OVERLAY_H

#define   AVNET_UENV_FDTO_SUPPORT

#define GET_OVERLAY_APPLY(dtbo_name)             \
	"echo Applying DT overlay: " dtbo_name " ;"  \
	"fatload mmc ${mmcdev}:${mmcpart} ${dtbo_addr} ${overlaydir}/" dtbo_name "; " \
	"fdt addr ${fdt_addr}; " \
	"fdt apply ${dtbo_addr}; "

#define   FDT_ENTRY_CHECK_RUN_ENV(string, name)     \
        "if env exists " string " && "              \
        "   test ${" string "} = 1 -o ${" string "} = yes ; then " \
            GET_OVERLAY_APPLY(name)           \
        "fi; "

#define   FDT_ENTRY_CHECK_RUN_CAMERA(name)                   \
        "if env exists " name " ; then "                     \
            " if test ${" name "} = ov5640 ; then "          \
			GET_OVERLAY_APPLY("maaxboard-8ulp-ov5640.dtbo")  \
            " elif test ${" name "} = as0260 ; then "        \
            GET_OVERLAY_APPLY("maaxboard-8ulp-as0260.dtbo")  \
            " else;"                                         \
            "       echo  no found displayer; "              \
            " fi; "                                          \
        "fi; "


/*
 * enable_overlay_xxx is set in uEnv.txt, then load the corresponding dtbo file
 */
#define FDT_ENTRY_DEF_SETTINGS          \
				FDT_ENTRY_CHECK_RUN_ENV("enable_overlay_gpio", "maaxboard-8ulp-ext-gpio.dtbo") \
				FDT_ENTRY_CHECK_RUN_ENV("enable_overlay_i2c", "maaxboard-8ulp-ext-i2c.dtbo")   \
				FDT_ENTRY_CHECK_RUN_ENV("enable_overlay_spi", "maaxboard-8ulp-ext-spi.dtbo")   \
				FDT_ENTRY_CHECK_RUN_ENV("enable_overlay_uart", "maaxboard-8ulp-ext-uart.dtbo") \
				FDT_ENTRY_CHECK_RUN_ENV("enable_overlay_disp", "maaxboard-8ulp-mipi.dtbo")     \
				FDT_ENTRY_CHECK_RUN_ENV("enable_overlay_cam", "maaxboard-8ulp-cam.dtbo")       \


#define MMC_RUN_FDT_OVERLAY       \
	"fdt addr ${fdt_addr}; "      \
	"fdt resize 0x10000; "        \
	FDT_ENTRY_DEF_SETTINGS        \
	"if env exists fdt_extra_overlays && test -n $fdt_extra_overlays ; then " \
		"for dtbo_file in ${fdt_extra_overlays};  do "  \
			GET_OVERLAY_APPLY( "${dtbo_file}" )         \
		"done; "                  \
	"fi;"


#define DEFAULT_MMC_UENV_ARGS \
	MMC_FDT_OVERLAY_SETTING \
    "bootenvfile=uEnv.txt\0" \
    "importbootenv=echo Importing environment from mmc${mmcdev} ...; " \
        "env import -t ${loadaddr} ${filesize}\0" \
    "loadbootenv=fatload mmc ${mmcdev} ${loadaddr} ${bootenvfile}\0" \
    "envboot=mmc dev ${mmcdev}; " \
        "if mmc rescan; then " \
            "echo SD/MMC found on device ${mmcdev};" \
            "if run loadbootenv; then " \
                "echo Loaded env from ${bootenvfile};" \
                "run importbootenv;" \
            "fi;" \
            "if test -n $uenvcmd; then " \
                "echo Running uenvcmd ...;" \
                "run uenvcmd;" \
            "fi;" \
        "fi;\0"


#define MMC_FDT_OVERLAY_SETTING                  \
	"dtbo_addr=0x83010000\0"                     \
	"overlaydir=overlays\0"                      \
	"loaddtbo=" MMC_RUN_FDT_OVERLAY "\0"         \


 #define MMC_BOOT_WITH_FDT_OVERLAY      \
	"mmc dev ${mmcdev};"                \
	"run envboot; "                     \
	"if run loadimage; then "           \
		"run mmcargs; "                 \
		"run loadfdt; "                 \
		"run loaddtbo; "                \
		"run boot_os; "                 \
	"else  "                            \
		"run netboot; "                 \
	"fi; "                              \


#endif  /* __AVNET_FDT_OVERLAY_H */
