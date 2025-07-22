#ifndef __MSC_COMMON_SPL_H
#define __MSC_COMMON_SPL_H

/*
 * Copyright (C) 2023 Avnet Embedded
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

struct mxc_spl_data {
	uint32_t	dram_size[2];
};

#define	MXC_SPL_DATA_PTR \
		((struct mxc_spl_data *) \
				((CONFIG_SPL_TEXT_BASE - sizeof(struct mxc_spl_data)) & ~0xf))

#endif /* __MSC_COMMON_SPL_H */
