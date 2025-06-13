/** @file       fdt_env.c

    @copyright  Copyright (C) 2020 by MSC Technologies GmbH

    SPDX-License-Identifier: GPL-2.0-or-later

    @author     Markus Pietrek

    @details
*/

#include <common.h>
#include <vsprintf.h>
#include <env.h>
#include "fdt_env.h"

void fdt_file_set_env(const char *var_name, const char *module, const char *dtb_suffix, const char *dtb_suffix2)
{
	char def[256];
	snprintf(def, sizeof(def), "%s-%s%s%s.dtb", module, dtb_suffix,
		dtb_suffix2 ? "-" : "",
		dtb_suffix2 ? dtb_suffix2 : "");
	env_set(var_name, def);
}

/* fdt_file_set_default_value("baseboard", "msc-sm2s-imx8", "baseboard", "ep1") sets
   the environment variable fdt_file_baseboard to msc-sm2s-imx8-baseboard-ep1.dtb
   unless it is already defined.
   dtb_suffix2 is optional and can be NULL.
   Total variable content must not be longer than 256 bytes.
*/

void fdt_file_set_default_value(const char *var_name, const char *module,
	const char *dtb_suffix, const char *dtb_suffix2)
{
	const char *fdt_file;

	fdt_file = env_get(var_name);
	if (!fdt_file || !strcmp(fdt_file, "auto"))
		fdt_file_set_env(var_name, module, dtb_suffix, dtb_suffix2);
}

#if defined(CONFIG_VIDEO) || defined(CONFIG_DM_VIDEO)
extern bool video_off; /* drivers/video/video_link.c */

/*
 return <0 on error,
 return 0 when no display is enabled
 return 1 when a display is enabled
*/
int fdt_board_set_display(void *fdt, const struct fdt_board_display_ops *ops, const void *context, const char *module, bool disabled)
{
	const char *dm;
	const char *display;
	const char *active_display;

	if (!disabled)
		display = env_get("display");
	else
		display = NULL;

	if (!display || !strcmp(display, "none")) {
		/* Disable display */
		/* We don't do "setenv video_off yes" as the internal dependency
		   to the variable 'display' would not be intuitive.
		   We would have to disable 'video_off' whenever the display variable
		   is set. */
		video_off = true;

		active_display = NULL;
	} else {
		if (strcmp(display, "auto")) {
			/* with auto use the default one in the device tree */
			int err = ops->set_display(fdt, context, display);
			if (err < 0)
				return err;
		}

		active_display = ops->get_display(fdt, context);
	}

	dm = env_get("fdt_file_display_mode");
	if (dm) {
		if (!strcmp(dm, "auto")) {
			/* set the kernel's device tree file for the display */
			if (!display)
				display = active_display;

			if (active_display)
				fdt_file_set_env(FDT_FILE_ENV_VAR(display), module, "display", active_display);
			else
				env_set(FDT_FILE_ENV_VAR(display), "undefined");
		} else if (!strcmp(dm, "fix")) {
			/* nothing to do, don't modify FDT_FILE_ENV_VAR(display) */
		} else {
			printf("WARNING: fdt_file_display_mode='%s' is not supported\n", dm);
		}
	}
	if (active_display) {
		printf("Display: %s\n", active_display);
		return 1;
	} else {
		return 0;
	}
}
#endif
