/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2022 NXP
 */

#ifndef __IMX93_EVK_H
#define __IMX93_EVK_H

#include <linux/sizes.h>
#include <linux/stringify.h>
#include <asm/arch/imx-regs.h>
#include "imx_env.h"
#include "maaxboard_overlay.h"

#define CFG_SYS_UBOOT_BASE	\
	(QSPI0_AMBA_BASE + CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR * 512)

#ifdef CONFIG_AHAB_BOOT
#define AHAB_ENV "sec_boot=yes\0"
#else
#define AHAB_ENV "sec_boot=no\0"
#endif

#ifdef CONFIG_DISTRO_DEFAULTS
#define BOOT_TARGET_DEVICES(func) \
	func(MMC, mmc, 0) \
	func(MMC, mmc, 1) \
	func(USB, usb, 0)

#include <config_distro_bootcmd.h>
#else
#define BOOTENV
#endif

#define JH_ROOT_DTB    "maaxboard-osm93.dtb"

#define JAILHOUSE_ENV \
	"jh_root_dtb=" JH_ROOT_DTB "\0" \
	"jh_mmcboot=setenv fdtfile ${jh_root_dtb}; " \
		    "setenv jh_clk clk_ignore_unused mem=1248MB kvm-arm.mode=nvhe; " \
		    "if run loadimage; then run mmcboot;" \
		    "else run jh_netboot; fi; \0" \
	"jh_netboot=setenv fdtfile ${jh_root_dtb}; " \
		    "setenv jh_clk clk_ignore_unused mem=1248MB kvm-arm.mode=nvhe; run netboot; \0 "

#define CFG_MFG_ENV_SETTINGS \
	CFG_MFG_ENV_SETTINGS_DEFAULT \
	"initrd_addr=0x83800000\0" \
	"initrd_high=0xffffffffffffffff\0" \
	"emmc_dev=0\0"\
	"sd_dev=1\0" \

/* Initial environment variables */
#define CFG_EXTRA_ENV_SETTINGS		\
	JAILHOUSE_ENV \
	CFG_MFG_ENV_SETTINGS \
	BOOTENV \
	AHAB_ENV \
	"scriptaddr=0x83500000\0" \
	"kernel_addr_r=" __stringify(CONFIG_SYS_LOAD_ADDR) "\0" \
	"image=Image\0" \
	"splashimage=0x90000000\0" \
	"console=ttyLP0,115200 earlycon\0" \
	"fdt_addr_r=0x83000000\0"			\
	"fdt_addr=0x83000000\0"			\
	"fdt_high=0xffffffffffffffff\0"		\
	"cntr_addr=0x98000000\0"			\
	"cntr_file=os_cntr_signed.bin\0" \
	"boot_fit=no\0" \
	"fdtfile=" CONFIG_DEFAULT_FDT_FILE "\0" \
	"bootm_size=0x10000000\0" \
	"mmcdev=" __stringify(CONFIG_SYS_MMC_ENV_DEV)"\0" \
	"mmcpart=1\0" \
	"mmcroot=/dev/mmcblk1p2 rootwait rw\0" \
	"mmcautodetect=yes\0" \
	"mmcargs=setenv bootargs ${jh_clk} console=${console} root=${mmcroot}\0 " \
	"dtbo_addr=0x83010000\0"  \
	"dtbo_dir=overlays\0"   \
	"fdt_size=0x10000\0" \
	"bootenvfile=uEnv.txt\0"  \
	"loadenvconf=fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${bootenvfile};env import -t ${loadaddr} ${filesize}\0" \
	"loadimage=fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${image}\0" \
	"loadfdt=fatload mmc ${mmcdev}:${mmcpart} ${fdt_addr_r} ${fdtfile}\0" \
	"boot_os=booti ${loadaddr} - ${fdt_addr_r};\0" \
	"mmcboot=run mmcargs; run loadimage; run loadfdt; run boot_os\0"

#ifdef AVNET_UENV_FDTO_SUPPORT
#undef  CONFIG_BOOTCOMMAND
#define CONFIG_BOOTCOMMAND      MMC_BOOT_WITH_FDT_OVERLAY
#endif

/* Link Definitions */

#define CFG_SYS_INIT_RAM_ADDR        0x80000000
#define CFG_SYS_INIT_RAM_SIZE        0x200000

#define CFG_SYS_SDRAM_BASE           0x80000000
#define PHYS_SDRAM                      0x80000000
#define PHYS_SDRAM_SIZE			0x80000000 /* 2GB DDR */

#define CFG_SYS_FSL_USDHC_NUM	2

/* Using ULP WDOG for reset */
#define WDOG_BASE_ADDR          WDG3_BASE_ADDR

#if defined(CONFIG_CMD_NET)
#define PHY_ANEG_TIMEOUT 20000
#endif

#ifdef CONFIG_IMX_MATTER_TRUSTY
#define NS_ARCH_ARM64 1
#endif

#ifdef CONFIG_ANDROID_SUPPORT
#include "imx93_evk_android.h"
#endif

#endif
