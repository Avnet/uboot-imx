/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2023 Avnet
 */

#ifndef __MAAXBOARD_8ULP_H
#define __MAAXBOARD_8ULP_H

#include <linux/sizes.h>
#include <asm/arch/imx-regs.h>
#include "imx_env.h"
#include "maaxboard_overlay.h"

#define CONFIG_SYS_BOOTM_LEN		(SZ_64M)
#define CONFIG_SPL_MAX_SIZE		(148 * 1024)
#define CONFIG_SYS_MONITOR_LEN		(512 * 1024)
#define CONFIG_SYS_UBOOT_BASE	(QSPI0_AMBA_BASE + CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR * 512)

#ifdef CONFIG_SPL_BUILD
#define CONFIG_SPL_STACK		0x22050000
#define CONFIG_SPL_BSS_START_ADDR	0x22048000
#define CONFIG_SPL_BSS_MAX_SIZE		0x2000	/* 8 KB */
#define CONFIG_SYS_SPL_MALLOC_START	0x22040000
#define CONFIG_SYS_SPL_MALLOC_SIZE	0x8000	/* 32 KB */

#define CONFIG_MALLOC_F_ADDR		0x22040000

#define CONFIG_SPL_ABORT_ON_RAW_IMAGE /* For RAW image gives a error info not panic */

#endif

#define COUNTER_FREQUENCY       1000000 /* 1MHz */

#define CONFIG_SERIAL_TAG

/* ENET Config */
#if defined(CONFIG_FEC_MXC)
#define CONFIG_ETHPRIME                 "FEC"
#define PHY_ANEG_TIMEOUT		20000

#define CONFIG_FEC_XCV_TYPE		RMII
#define CONFIG_FEC_MXC_PHYADDR		1

#define IMX_FEC_BASE			0x29950000
#endif

#ifdef CONFIG_AHAB_BOOT
#define AHAB_ENV "sec_boot=yes\0"
#else
#define AHAB_ENV "sec_boot=no\0"
#endif

#ifdef CONFIG_DISTRO_DEFAULTS
#define BOOT_TARGET_DEVICES(func) \
	func(USB, usb, 0) \
	func(MMC, mmc, 0)

#include <config_distro_bootcmd.h>
#else
#define BOOTENV
#endif

#define JAILHOUSE_ENV \
	"jh_clk= \0 " \
	"jh_mmcboot=setenv jh_clk clk_ignore_unused mem=896MB; run loadimage; run mmcboot\0 " \
	"jh_netboot=setenv jh_clk clk_ignore_unused mem=896MB; run netboot\0 "

#define CONFIG_MFG_ENV_SETTINGS \
	CONFIG_MFG_ENV_SETTINGS_DEFAULT \
	"initrd_addr=0x83800000\0" \
	"initrd_high=0xffffffffffffffff\0" \
	"emmc_dev=0\0"\
	"sd_dev=2\0"

/* Initial environment variables */
#define CONFIG_EXTRA_ENV_SETTINGS		\
	CONFIG_MFG_ENV_SETTINGS \
	BOOTENV \
	JAILHOUSE_ENV \
	AHAB_ENV \
	"scriptaddr=0x83500000\0" \
	"kernel_addr_r=" __stringify(CONFIG_SYS_LOAD_ADDR) "\0" \
	"image=Image\0" \
	"splashimage=0x90000000\0" \
	"console=ttyLP1,115200 earlycon\0" \
	"fdtoverlay_addr_r=0x83040000\0"			\
	"fdt_addr_r=0x83000000\0"			\
	"fdt_addr=0x83000000\0"			\
	"fdt_high=0xffffffffffffffff\0"		\
	"cntr_addr=0x98000000\0"			\
	"cntr_file=os_cntr_signed.bin\0" \
	"boot_fit=no\0" \
	"fdtfile=" CONFIG_DEFAULT_FDT_FILE "\0" \
	"bootm_size=0x10000000\0" \
	"mmcdev="__stringify(CONFIG_SYS_MMC_ENV_DEV)"\0" \
	"mmcpart=1\0" \
	"mmcroot=" CONFIG_MMCROOT " rootwait rw\0" \
	"mmcautodetect=yes\0" \
	"mmcargs=setenv bootargs ${jh_clk} console=${console} root=${mmcroot}\0 " \
	"dtbo_addr=0x83010000\0"  \
	"dtbo_dir=overlays\0"   \
	"fdt_size=0x10000\0" \
	"bootenvfile=uEnv.txt\0"  \
	"loadenvconf=fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${bootenvfile};env import -t ${loadaddr} ${filesize}\0" \
	"loadimage=fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${image}\0" \
	"loadfdt=fatload mmc ${mmcdev}:${mmcpart} ${fdt_addr_r} ${fdtfile}\0" \
	"loadcntr=fatload mmc ${mmcdev}:${mmcpart} ${cntr_addr} ${cntr_file}\0" \
	"auth_os=auth_cntr ${cntr_addr}\0" \
	"boot_os=booti ${loadaddr} - ${fdt_addr_r};\0" \
	"mmcboot=usb start; run mmcargs; run loadimage; run loadfdt; run boot_os\0"


#ifdef AVNET_UENV_FDTO_SUPPORT
#undef  CONFIG_BOOTCOMMAND
#define CONFIG_BOOTCOMMAND      MMC_BOOT_WITH_FDT_OVERLAY
#endif

/* Link Definitions */

#define CONFIG_SYS_INIT_RAM_ADDR	0x80000000
#define CONFIG_SYS_INIT_RAM_SIZE	0x80000
#define CONFIG_SYS_INIT_SP_OFFSET	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR		(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

#define CONFIG_MMCROOT			"/dev/mmcblk0p2"

#define CONFIG_SYS_SDRAM_BASE		0x80000000
#define PHYS_SDRAM			0x80000000
#define PHYS_SDRAM_SIZE			0x80000000 /* 2GB DDR */

/* Monitor Command Prompt */
#define CONFIG_SYS_CBSIZE		2048
#define CONFIG_SYS_MAXARGS		64
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)

/* Using ULP WDOG for reset */
#define WDOG_BASE_ADDR			WDG3_RBASE
/* USB Configs */
#define CONFIG_USB_MAX_CONTROLLER_COUNT 2

#ifdef CONFIG_ANDROID_SUPPORT
#include "imx8ulp_evk_android.h"
#endif

#endif
