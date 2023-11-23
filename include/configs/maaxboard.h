/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2023 Avnet
 */

#ifndef __MAAXBOARD_H
#define __MAAXBOARD_H

#include <linux/sizes.h>
#include <linux/stringify.h>
#include <asm/arch/imx-regs.h>
#include "imx_env.h"
#include "maaxboard_overlay.h"

#ifdef CONFIG_SPL_BUILD
/*#define CONFIG_ENABLE_DDR_TRAINING_DEBUG*/

/* malloc f used before GD_FLG_FULL_MALLOC_INIT set */
#define CFG_MALLOC_F_ADDR		0x182000
/* For RAW image gives a error info not panic */
#define CONFIG_SPL_ABORT_ON_RAW_IMAGE

#undef CONFIG_DM_MMC

#define CONFIG_POWER_I2C
#define CONFIG_POWER_BD71837

#define CFG_POWER_PFUZE100_I2C_ADDR 0x08
#endif

/* ENET Config */
/* ENET1 */
#if defined(CONFIG_FEC_MXC)
#define CFG_FEC_MXC_PHYADDR          4
#define PHY_ANEG_TIMEOUT 20000
#endif

#ifdef CONFIG_DISTRO_DEFAULTS
#define BOOT_TARGET_DEVICES(func) \
	func(MMC, mmc, 0) \
    func(USB, usb, 0)

#include <config_distro_bootcmd.h>
#else
#define BOOTENV
#endif

/*
 * Another approach is add the clocks for inmates into clks_init_on
 * in clk-imx8mq.c, then clk_ingore_unused could be removed.
 */
#define JAILHOUSE_ENV \
	"jh_clk= \0 " \
	"jh_mmcboot=setenv fdtfile maaxboard.dtb; " \
		"setenv jh_clk clk_ignore_unused mem=1872M; " \
			   "if run loadimage; then " \
				   "run mmcboot; " \
			   "else run jh_netboot; fi; \0" \
	"jh_netboot=setenv fdtfile maaxboard.dtb; setenv jh_clk clk_ignore_unused mem=1872MB; run netboot; \0 "

#define SR_IR_V2_COMMAND \
	"nodes=/soc@0/caam-sm@100000 /soc@0/bus@30000000/caam_secvio /soc@0/bus@30000000/caam-snvs@30370000 /soc@0/bus@32c00000/hdmi@32c00000 /soc@0/bus@32c00000/display-controller@32e00000 /soc@0/vpu@38300000 /soc@0/vpu_v4l2 /gpu3d@38000000 /audio-codec-bt-sco /audio-codec /sound-bt-sco /sound-wm8524 /sound-spdif /sound-hdmi-arc /binman \0" \
	"sr_ir_v2_cmd=cp.b ${fdtcontroladdr} ${fdt_addr_r} 0x10000;"\
	"fdt addr ${fdt_addr_r};"\
	"fdt set /soc@0/usb@38100000 compatible snps,dwc3;" \
	"fdt set /soc@0/usb@38200000 compatible snps,dwc3;" \
	"for i in ${nodes}; do fdt rm ${i}; done \0"

#define CFG_MFG_ENV_SETTINGS \
	CFG_MFG_ENV_SETTINGS_DEFAULT \
	"initrd_addr=0x43800000\0" \
	"initrd_high=0xffffffffffffffff\0" \
	"emmc_dev=0\0"\
	"sd_dev=1\0" \

/* Initial environment variables */
#define CFG_EXTRA_ENV_SETTINGS		\
	CFG_MFG_ENV_SETTINGS \
	BOOTENV \
	SR_IR_V2_COMMAND \
	JAILHOUSE_ENV \
    "prepare_mcore=setenv mcore_clk clk-imx8mq.mcore_booted;\0" \
	"kernel_addr_r=" __stringify(CONFIG_SYS_LOAD_ADDR) "\0" \
    "image=Image\0" \
    "splashimage=0x50000000\0" \
    "console=ttymxc0,115200\0" \
    "fdt_addr_r=0x43000000\0"           \
    "fdt_addr=0x43000000\0"         \
    "fdt_high=0xffffffffffffffff\0"     \
    "boot_fdt=try\0" \
    "fdtfile=" CONFIG_DEFAULT_FDT_FILE "\0" \
    "bootm_size=0x10000000\0" \
    "mmcdev="__stringify(CONFIG_SYS_MMC_ENV_DEV)"\0" \
    "mmcpart=1\0" \
    "mmcroot=" CONFIG_MMCROOT " rootwait rw\0" \
    "mmcautodetect=yes\0" \
    "mmcargs=setenv bootargs ${jh_clk} ${mcore_clk} console=${console} root=${mmcroot}\0 " \
    "dtbo_addr=0x43010000\0"  \
    "dtbo_dir=overlays\0"   \
    "fdt_size=0x10000\0" \
    "bootenvfile=uEnv.txt\0"  \
    "loadenvconf=fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${bootenvfile};env import -t ${loadaddr} ${filesize}\0" \
    "loadimage=fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${image}\0" \
    "loadfdt=fatload mmc ${mmcdev}:${mmcpart} ${fdt_addr_r} ${fdtfile}\0" \
    "boot_os=booti ${loadaddr} - ${fdt_addr_r};\0" \
    "mmcboot=run mmcargs; run loadimage; run loadfdt; run boot_os\0"

/* Link Definitions */

#define CFG_SYS_INIT_RAM_ADDR        0x40000000
#define CFG_SYS_INIT_RAM_SIZE        0x80000


#define CFG_SYS_SDRAM_BASE           0x40000000
#define PHYS_SDRAM                      0x40000000
#define PHYS_SDRAM_SIZE			0x80000000 /* 3GB DDR */

#define CFG_MXC_UART_BASE		UART_BASE_ADDR(1)

#define CONFIG_MMCROOT          "/dev/mmcblk1p2"  /* USDHC2 */

#define CFG_SYS_FSL_USDHC_NUM	2
#define CFG_SYS_FSL_ESDHC_ADDR       0

#ifdef AVNET_UENV_FDTO_SUPPORT
#undef  CONFIG_BOOTCOMMAND
#define CONFIG_BOOTCOMMAND      MMC_BOOT_WITH_FDT_OVERLAY
#endif

#ifdef CONFIG_ANDROID_SUPPORT
#include "imx8mq_evk_android.h"
#endif

#endif
