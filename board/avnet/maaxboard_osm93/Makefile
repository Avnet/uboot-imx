#
# Copyright 2025 Avnet
#
# SPDX-License-Identifier:      GPL-2.0+
#

obj-y += maaxboard_osm93.o
obj-y += ../../freescale/common/mmc.o

ifndef CONFIG_SPL_BUILD
       obj-$(CONFIG_USB_TCPC) += ../../freescale/common/tcpc.o
endif

ifdef CONFIG_SPL_BUILD
obj-y += spl.o
ifdef CONFIG_IMX9_DRAM_INLINE_ECC
obj-$(CONFIG_IMX93_EVK_LPDDR4X) += lpddr4x_timing_ecc.o lpddr4x_timing_1866mts_ecc.o lpddr4_2GiB_micron_timing.o
obj-$(CONFIG_TARGET_IMX91P_11X11_EVK) += lpddr4_timing_2400mts_ecc.o
else
obj-$(CONFIG_IMX93_EVK_LPDDR4X) += lpddr4x_timing.o lpddr4x_timing_1866mts.o lpddr4_2GiB_micron_timing.o
obj-$(CONFIG_TARGET_IMX91P_11X11_EVK) += lpddr4_timing_2400mts.o
endif
endif
