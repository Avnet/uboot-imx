/*
 * Copyright 2021 AVNET
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __EM_FDT_OVERLAY_H
#define __EM_FDT_OVERLAY_H

#define GET_OVERLAY_APPLY(dtbo_name)          \
	"echo Applying DT overlay: " dtbo_name " ;"  \
	"fatload mmc ${mmcdev}:${mmcpart} ${loadaddr_dtb} ${overlaydir}/" dtbo_name "; " \
	"fdt apply ${loadaddr_dtb}; "

#define   FDT_ENTRY_CHECK_RUN_ENV(string, name)     \
        "if env exists " string " && "              \
        "   test ${" string "} = 1 -o ${" string "} = yes ; then " \
            GET_OVERLAY_APPLY(name)           \
        "fi; "

#define   FDT_ENTRY_CHECK_RUN_VAL(x, f)       \
        "if env exists " #x " &&  "           \
        "   test ${" #x "} = 1 || test ${" #x "} = yes ; then " \
            GET_OVERLAY_APPLY( #f )           \
        "fi; "

#if defined(CONFIG_TARGET_MAAXBOARD)
#define   FDT_ENTRY_CHECK_RUN_DISP(name)              \
        "if env exists " name " ; then "              \
            " if test ${" name "} = hdmi ; then "     \
			GET_OVERLAY_APPLY("maaxboard-hdmi.dtbo")  \
            " elif test ${" name "} = mipi ; then "   \
            GET_OVERLAY_APPLY("maaxboard-mipi.dtbo")  \
            " elif test ${" name "} = dual ; then "   \
            GET_OVERLAY_APPLY("maaxboard-dual-display.dtbo")  \
            " else;"                                  \
            "       echo  no found displayer; "       \
            " fi; "                                   \
        "fi; "
#endif

/* Default overlays for different board,
 * enable_overlay_xxx is set in uEnv.txt, then load the corresponding dtbo file
 */
#if defined(CONFIG_TARGET_MAAXBOARD)
#define FDT_ENTRY_DEF_SETTINGS          \
				FDT_ENTRY_CHECK_RUN_DISP("enable_overlay_disp")  \
				FDT_ENTRY_CHECK_RUN_ENV("enable_overlay_gpio", "maaxboard-ext-gpio.dtbo") \
				FDT_ENTRY_CHECK_RUN_ENV("enable_overlay_i2c", "maaxboard-ext-i2c.dtbo") \
				FDT_ENTRY_CHECK_RUN_ENV("enable_overlay_pwm", "maaxboard-ext-pwm.dtbo") \
				FDT_ENTRY_CHECK_RUN_ENV("enable_overlay_spi", "maaxboard-ext-spi.dtbo") \
				FDT_ENTRY_CHECK_RUN_ENV("enable_overlay_sai2", "maaxboard-ext-sai2.dtbo") \
				FDT_ENTRY_CHECK_RUN_ENV("enable_overlay_uart2", "maaxboard-ext-uart2.dtbo") \
				FDT_ENTRY_CHECK_RUN_ENV("enable_overlay_camera", "maaxboard-ov5640.dtbo") \
				FDT_ENTRY_CHECK_RUN_ENV("enable_overlay_usbgadget", "maaxboard-usb0-device.dtbo")

#elif defined(CONFIG_TARGET_MAAXBOARD_MINI)
#define FDT_ENTRY_DEF_SETTINGS          \
				FDT_ENTRY_CHECK_RUN_VAL(enable_overlay_mipi, maaxboard-mini-mipi.dtbo) \
				FDT_ENTRY_CHECK_RUN_VAL(enable_overlay_gpio, maaxboard-mini-ext-gpio.dtbo) \
				FDT_ENTRY_CHECK_RUN_VAL(enable_overlay_i2c, maaxboard-mini-ext-i2c.dtbo) \
				FDT_ENTRY_CHECK_RUN_VAL(enable_overlay_pwm, maaxboard-mini-ext-pwm.dtbo) \
				FDT_ENTRY_CHECK_RUN_VAL(enable_overlay_spi, maaxboard-mini-ext-spi.dtbo) \
				FDT_ENTRY_CHECK_RUN_VAL(enable_overlay_sai3, maaxboard-mini-ext-sai3.dtbo) \
				FDT_ENTRY_CHECK_RUN_VAL(enable_overlay_uart2, maaxboard-mini-ext-uart2.dtbo) \
				FDT_ENTRY_CHECK_RUN_VAL(enable_overlay_camera, maaxboard-mini-ov5640.dtbo) \
				FDT_ENTRY_CHECK_RUN_VAL(enable_overlay_usbgadget, maaxboard-mini-usb1-device.dtbo)

#elif defined(CONFIG_TARGET_MAAXBOARD_NANO)
#define FDT_ENTRY_DEF_SETTINGS          \
				FDT_ENTRY_CHECK_RUN_VAL(enable_overlay_mipi, maaxboard-nano-mipi.dtbo) \
				FDT_ENTRY_CHECK_RUN_ENV("enable_overlay_gpio", "maaxboard-nano-ext-gpio.dtbo") \
				FDT_ENTRY_CHECK_RUN_ENV("enable_overlay_i2c", "maaxboard-nano-ext-i2c.dtbo") \
				FDT_ENTRY_CHECK_RUN_ENV("enable_overlay_pwm", "maaxboard-nano-ext-pwm.dtbo") \
				FDT_ENTRY_CHECK_RUN_ENV("enable_overlay_spi", "maaxboard-nano-ext-spi.dtbo") \
				FDT_ENTRY_CHECK_RUN_ENV("enable_overlay_uart4", "maaxboard-nano-ext-uart4.dtbo") \
				FDT_ENTRY_CHECK_RUN_ENV("enable_overlay_camera", "maaxboard-nano-ov5640.dtbo")

#else
#define FDT_ENTRY_DEF_SETTINGS         ""
#endif


#define MMC_RUN_FDT_OVERLAY       \
	"fdt addr ${fdt_addr}; "      \
	"fdt resize 0x20000; "        \
	FDT_ENTRY_DEF_SETTINGS        \
	"if env exists fdt_extra_overlays && test -n $fdt_extra_overlays ; then " \
		"for dtbo_file in ${fdt_extra_overlays};  do "  \
			GET_OVERLAY_APPLY( "${dtbo_file}" )         \
		"done; "                  \
	"fi;"


#define MMC_FDT_OVERLAY_SETTING              \
	"loadaddr_dtb=0x43200000\0"              \
	"overlaydir=overlays\0"                  \
	"mmcbootdto=echo Booting from mmc ...; " \
		"run mmcargs; "                      \
		"if test -n ${boot_fit} && test ${boot_fit} = yes || test ${boot_fit} = try; then " \
			"bootm ${loadaddr}; "            \
		"else "                              \
			"if run loadfdt; then "          \
				MMC_RUN_FDT_OVERLAY          \
				"booti ${loadaddr} - ${fdt_addr}; "  \
			"else "                          \
				"echo WARN: Cannot load the DT; "    \
			"fi; "                           \
		"fi;\0"


 #define MMC_BOOT_WITH_FDT_OVERLAY                 \
	   "mmc dev ${mmcdev}; if mmc rescan; then "   \
		   "if run loadbootscript; then "          \
			   "run bootscript; "                  \
		   "else "                                 \
			   "if run loadimage; then "           \
				   "run mmcbootdto; "              \
			   "else run netboot; "                \
			   "fi; "                              \
		   "fi; "                                  \
	   "fi;"

#define   EM_UENV_FDTO_SUPPORT

/*
For example, place the macro definition like this in your board's config file:
#define CONFIG_EXTRA_ENV_SETTINGS    \
		DEFAULT_MMC_MX8_ARGS     \
		MMC_FDT_OVERLAY_SETTING  \
		"script=boot.scr\0"      \
		"image=Image\0"          \
		"console=ttymxc0,115200 earlycon=ec_imx6q,0x30860000,115200\0" \

#ifdef EM_UENV_FDTO_SUPPORT
#undef CONFIG_BOOTCOMMAND
#define CONFIG_BOOTCOMMAND      \
		"run distro_bootcmd;"   \
		MMC_BOOT_UENV           \
		MMC_BOOT_WITH_FDT_OVERLAY
#endif
*/

#endif  /* __EM_FDT_OVERLAY_H */
