/*
 * Copyright (C) 2023 Avnet Embedded, MSC Technologies GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <common.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/libfdt.h>
#include <fdt_support.h>
#include "boardinfo_fdt.h"
#include <version.h>
#include <version_string.h>

static int bi_setprop(void *fdt, int nodeoffset, const char *prop, const char *val)
{
	int err;

	err = fdt_setprop(fdt, nodeoffset, prop, val, strlen(val) + 1);
	if (err < 0) {
		printf("WARNING: could not set '%s' %s.\n", prop, fdt_strerror(err));
		return err;
	}

	return 0;
}

#define FDT_ADD_BI_PROPERTY_STR(X, VAL) \
	do { \
		err = bi_setprop(fdt, nodeoffset, #X, VAL); \
		if (err < 0) { \
			return err; \
		} \
	} while(0)

#define FDT_ADD_BI_PROPERTY(X) FDT_ADD_BI_PROPERTY_STR(X, bi_get_##X(bi))

int bi_fixup_fdt(const board_info_t *bi, void *fdt, const char *firmware_version, const char *extra_hw_version)
{
	int nodeoffset;
	int err;

	if (bi == NULL) {
		/* This is not an error. It may happen when the board has not been individualized in factory. */
		printf("Warning: no boardinfo for devicetree fixup available\n");
		return 0;
	}

	err = fdt_check_header(fdt);
	if (err < 0) {
		printf("%s: %s\n", __FUNCTION__, fdt_strerror(err));
		return err;
	}

	/* find or create "/boardinfo" node. */
	nodeoffset = fdt_find_or_add_subnode(fdt, 0, "boardinfo");
	if (nodeoffset < 0)
		return nodeoffset;

	FDT_ADD_BI_PROPERTY(company);
	FDT_ADD_BI_PROPERTY(form_factor);
	FDT_ADD_BI_PROPERTY(platform);
	FDT_ADD_BI_PROPERTY(processor);
	FDT_ADD_BI_PROPERTY(feature);
	FDT_ADD_BI_PROPERTY(serial);
	FDT_ADD_BI_PROPERTY(revision);

	{
		char buf[16];
		sprintf(buf, "%i", bi_get_boot_count(bi));
		FDT_ADD_BI_PROPERTY_STR(boot_count, buf);

		FDT_ADD_BI_PROPERTY_STR(bootloader_version, version_string);
	}

#ifdef BI_HAS_BSP_SPECIFIC
	if (bi_has_bsp_specific(bi)) {
		char buf[8];
		sprintf(buf, "0x%02x", bi_get_bsp_specific(bi));
		FDT_ADD_BI_PROPERTY_STR(bsp_specific, buf);
	}
#endif

	if (firmware_version)
		FDT_ADD_BI_PROPERTY_STR(firmware_version, firmware_version);

	if (extra_hw_version && *extra_hw_version)
		FDT_ADD_BI_PROPERTY_STR(extra_hw_version, extra_hw_version);

	return 0;

}
