/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2023 Avnet
 */

#ifndef __MAAXBOARD_MINI_H
#define __MAAXBOARD_MINI_H

#include <linux/sizes.h>
#include <linux/stringify.h>
#include <asm/arch/imx-regs.h>
#include "imx_env.h"
#include "maaxboard_overlay.h"

#define UBOOT_ITB_OFFSET			0x57C00
#define FSPI_CONF_BLOCK_SIZE		0x1000
#define UBOOT_ITB_OFFSET_FSPI  \
	(UBOOT_ITB_OFFSET + FSPI_CONF_BLOCK_SIZE)
#ifdef CONFIG_FSPI_CONF_HEADER
#define CFG_SYS_UBOOT_BASE  \
	(QSPI0_AMBA_BASE + UBOOT_ITB_OFFSET_FSPI)
#else
#define CFG_SYS_UBOOT_BASE	\
	(QSPI0_AMBA_BASE + CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR * 512)
#endif

#ifdef CONFIG_SPL_BUILD
#define CONFIG_SYS_SPL_MALLOC_START	0x42200000

/* malloc f used before GD_FLG_FULL_MALLOC_INIT set */
#define CFG_MALLOC_F_ADDR		0x930000

#define CONFIG_POWER_BD71837

#endif

#define PHY_ANEG_TIMEOUT 20000


#ifdef CONFIG_NAND_BOOT
#define MFG_NAND_PARTITION "mtdparts=gpmi-nand:64m(nandboot),16m(nandfit),32m(nandkernel),16m(nanddtb),8m(nandtee),-(nandrootfs)"
#endif

#ifdef CONFIG_DISTRO_DEFAULTS
#define BOOT_TARGET_DEVICES(func) \
	func(USB, usb, 0) \
	func(MMC, mmc, 1) \
	func(MMC, mmc, 2)

#include <config_distro_bootcmd.h>
#else
#define BOOTENV
#endif

/*
 * Another approach is add the clocks for inmates into clks_init_on
 * in clk-imx8mm.c, then clk_ingore_unused could be removed.
 */
#define JH_ROOT_DTB    "imx8mm-evk-root.dtb"


/* Link Definitions */

#define CFG_SYS_INIT_RAM_ADDR        0x40000000
#define CFG_SYS_INIT_RAM_SIZE        0x200000
#define JAILHOUSE_ENV \
	"jh_clk= \0 " \
	"jh_root_dtb=" JH_ROOT_DTB "\0" \
	"jh_mmcboot=mw 0x303d0518 0xff; setenv fdtfile ${jh_root_dtb};" \
		"setenv jh_clk clk_ignore_unused mem=1212MB; " \
			   "if run loadimage; then " \
				   "run mmcboot; " \
			   "else run jh_netboot; fi; \0" \
	"jh_netboot=mw 0x303d0518 0xff; setenv fdtfile ${jh_root_dtb}; setenv jh_clk clk_ignore_unused mem=1212MB; run netboot; \0 "

#define CFG_MFG_ENV_SETTINGS \
	CFG_MFG_ENV_SETTINGS_DEFAULT \
	"initrd_addr=0x43800000\0" \
	"initrd_high=0xffffffffffffffff\0" \
	"emmc_dev=0\0"\
	"sd_dev=0\0" \

/* Initial environment variables */
#if defined(CONFIG_NAND_BOOT)
#define CFG_EXTRA_ENV_SETTINGS		\
	CFG_MFG_ENV_SETTINGS \
	"splashimage=0x50000000\0" \
	"fdt_addr_r=0x43000000\0"			\
	"fdt_addr=0x43000000\0"			\
	"fdt_high=0xffffffffffffffff\0" \
	"mtdparts=" MFG_NAND_PARTITION "\0" \
	"console=ttymxc1,115200 earlycon=ec_imx6q,0x30890000,115200\0" \
	"bootargs=console=ttymxc1,115200 earlycon=ec_imx6q,0x30890000,115200 ubi.mtd=nandrootfs "  \
		"root=ubi0:nandrootfs rootfstype=ubifs "		     \
		MFG_NAND_PARTITION \
		"\0" \
	"bootcmd=nand read ${loadaddr} 0x5000000 0x2000000;"\
		"nand read ${fdt_addr_r} 0x7000000 0x100000;"\
		"booti ${loadaddr} - ${fdt_addr_r}"

#else
#define CFG_EXTRA_ENV_SETTINGS		\
	CFG_MFG_ENV_SETTINGS \
	BOOTENV \
	JAILHOUSE_ENV \
	"prepare_mcore=setenv mcore_clk clk-imx8mm.mcore_booted;\0" \
	"kernel_addr_r=" __stringify(CONFIG_SYS_LOAD_ADDR) "\0" \
	"image=Image\0" \
	"splashimage=0x50000000\0" \
	"console=ttymxc0,115200\0" \
	"fdt_addr_r=0x43000000\0"			\
	"fdt_addr=0x43000000\0"			\
	"fdt_high=0xffffffffffffffff\0"		\
	"boot_fit=no\0" \
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
#endif

#ifdef AVNET_UENV_FDTO_SUPPORT
#undef  CONFIG_BOOTCOMMAND
#define CONFIG_BOOTCOMMAND      MMC_BOOT_WITH_FDT_OVERLAY
#endif

#define CONFIG_MMCROOT			"/dev/mmcblk0p2"  /* USDHC1 */


#define CFG_SYS_SDRAM_BASE           0x40000000
#define PHYS_SDRAM                      0x40000000
#define PHYS_SDRAM_SIZE			0x80000000 /* 2GB DDR */

#define CFG_FEC_MXC_PHYADDR          4

#define CFG_MXC_UART_BASE		UART1_BASE_ADDR
#define CFG_BAUDRATE				115200

/* USDHC */

#define CFG_SYS_FSL_USDHC_NUM	1
#define CFG_SYS_FSL_ESDHC_ADDR       0

#define CFG_SYS_NAND_BASE           0x20000000

#ifdef CONFIG_IMX_MATTER_TRUSTY
#define NS_ARCH_ARM64 1
#endif

#ifdef CONFIG_ANDROID_SUPPORT
#include "imx8mm_evk_android.h"
#endif

#endif
