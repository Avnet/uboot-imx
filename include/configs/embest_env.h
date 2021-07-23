/*
 * Copyright 2018-2019 EMBEST
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __EM_SBC_UENV_H
#define __EM_SBC_UENV_H

#ifndef CONFIG_CMD_IMPORTENV
#define CONFIG_CMD_IMPORTENV
#endif

#define DEFAULT_MMC_UENV_ARGS \
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

#define MMC_BOOT_UENV  "run envboot;"

#define DEFAULT_MMC_MX6_ARGS  DEFAULT_MMC_UENV_ARGS
#define DEFAULT_MMC_MX7_ARGS  DEFAULT_MMC_UENV_ARGS
#define DEFAULT_MMC_MX8_ARGS  DEFAULT_MMC_UENV_ARGS

/*
For example, place the macro definition like this in your board's config file:
#define CONFIG_EXTRA_ENV_SETTINGS \
	DEFAULT_MMC_MX8_ARGS \
	"script=boot.scr\0" \
	"image=Image\0" \
	"console=ttymxc0,115200 earlycon=ec_imx6q,0x30860000,115200\0" \

#define CONFIG_BOOTCOMMAND \
    MMC_BOOT_UENV \
    "mmc dev ${mmcdev}; if mmc rescan; then " \

*/

#endif  /* __EM_SBC_UENV_H */
