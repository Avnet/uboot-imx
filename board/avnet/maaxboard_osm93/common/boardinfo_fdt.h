#ifndef __MSC_BOARDINFO_FDT_H__
#define __MSC_BOARDINFO_FDT_H__

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

#include "boardinfo.h"

/* firmware_version: comma separated list of individual firmware components or NULL if not necessary

   <comp_version>[,<comp_version>[,<comp_version>...]]
   comp_version=<name>=<val>
   name=<string_val>
   val=<string_val>
   string_val=string without spaces and commas
   e.g.
   SCFW=4a1654e0,SECO_FW=376e3c15,ATF=b0a00f2
*/
int bi_fixup_fdt(const board_info_t *bi, void *fdt, const char *firmware_version, const char *extra_hw_version);

#endif /* __MSC_BOARDINFO_FDT_H__ */
