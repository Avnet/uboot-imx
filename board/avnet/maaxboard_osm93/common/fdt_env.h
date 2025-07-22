/** @file       fdt_env.h

    @copyright  Copyright (C) 2020 by MSC Technologies GmbH

    SPDX-License-Identifier: GPL-2.0-or-later

    @author     Markus Pietrek

    @details
*/

#ifndef __MSC_FDT_ENV_H__
#define __MSC_FDT_ENV_H__

#define FDT_FILE_ENV_VAR(x) "fdt_file_" # x

void fdt_file_set_env(const char *var_name, const char *module,
    const char *dtb_suffix, const char *dtb_suffix2);
void fdt_file_set_default_value(const char *var_name, const char *module,
    const char *dtb_suffix, const char *dtb_suffix2);


struct fdt_board_display_ops {
    int (*set_display)(void *fdt, const void *context, const char *display);
    const char * (*get_display)(void *fdt, const void *context);
};

int fdt_board_set_display(void *fdt, const struct fdt_board_display_ops *ops, const void *context, const char *module, bool disabled);

#endif