#
# Copyright 2025 Avnet
#
# SPDX-License-Identifier:      GPL-2.0+
#

obj-y                    += ../../freescale/common/mmc.o
obj-y                    += boardinfo.o


obj-y                    += ./common/string.o
obj-y                    += ./common/mac_tools.o
obj-y                    += ./common/feature.o
obj-y                    += ./common/boardinfo.o
obj-y                    += ./common/boardinfo_fdt.o

obj-$(CONFIG_IMX8M)      += ./common/mx8m_common.o
obj-$(CONFIG_IMX9)       += ./common/mx9_common.o ./common/mx9_fuse.o
obj-y                    += ./common/som_variant.o


ifdef CONFIG_SPL_BUILD
obj-y                                  += spl.o
obj-y                                  += early_init.o
obj-y                                  += lpddr4_512MiB_nanya_timing.o
obj-y                                  += lpddr4_1GiB_nanya_timing.o
obj-y                                  += lpddr4_2GiB_micron_timing.o
obj-$(CONFIG_IMX93_EVK_LPDDR4X)        += lpddr4x_timing.o lpddr4x_timing_1866mts.o
obj-$(CONFIG_TARGET_IMX91P_11X11_EVK)  += lpddr4_timing_2400mts.o
else
obj-y                                  += maaxboard_osm93.o
obj-$(CONFIG_USB_TCPC)                 += ../../freescale/common/tcpc.o
obj-y                                  += ./common/fdtload.o
obj-y                                  += ./common/fdt_env.o
endif
