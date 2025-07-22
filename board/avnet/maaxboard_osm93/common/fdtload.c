/** @file       fdtload.c

    @copyright  Copyright (C) 2020 by MSC Technologies GmbH

    SPDX-License-Identifier: GPL-2.0-or-later

    @author     Markus Pietrek

    @details    Merges a device tree .dtb with none or more device tree overlay .dtbo files.
	            The load address of the .dtb is defined in the variable fdt_addr.
				The load address of the .dto is defined in the variable fdt_addr_ov.
				Files are loaded via the command loadfdt_raw. The address will be set as tmp_fdt_addr and the file as tmp_fdt_file
*/

#include <common.h>
#include <command.h>
#include <fdt_support.h>
#include <mapmem.h>

static const char * const tmp_fdt_addr_env = "tmp_fdt_addr";
static const char * const tmp_fdt_file_env = "tmp_fdt_file";
static const char * const cmd_loadfdt = "run loadfdt_raw";

static int load_fdt(const char *file, ulong where_to)
{
	char buf[32];
	int rc;

	sprintf(buf, "0x%lx", where_to);

	rc = env_set(tmp_fdt_addr_env, buf);
	if (rc)
		goto out_0;

	rc = env_set(tmp_fdt_file_env, file);
	if (rc)
		goto out_1;

	rc = run_command(cmd_loadfdt, 0);
	if (rc)
		printf("Loading %s to 0x%lx failed\n", file, where_to);

	env_set(tmp_fdt_addr_env, NULL);
out_1:
	env_set(tmp_fdt_file_env, NULL);
out_0:

	return rc
		? CMD_RET_FAILURE
		: CMD_RET_SUCCESS;
}

static int do_fdtload(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	const char *var;
	ulong fdt_addr;
	ulong fdt_addr_ov = 0; /* assignment to get rid of a wrong compiler warning that fdt_addr_ov may be uninitialized */
	ulong addr;
	int i;

	if (argc < 2)
		return CMD_RET_USAGE;

	if (env_get(tmp_fdt_addr_env) ||
		env_get(tmp_fdt_file_env)) {
		printf("Variables '%s' and '%s' mustn't be set as they are used internally\n",
			tmp_fdt_addr_env, tmp_fdt_file_env);
		return CMD_RET_FAILURE;
	}

	var = env_get("fdt_addr");
	if (var == NULL) {
		printf("fdt_addr not set\n");
		return CMD_RET_FAILURE;
	}
	fdt_addr = simple_strtoul(var, NULL, 16);

	if (argc > 2) {
		var = env_get("fdt_addr_ov");
		if (var == NULL) {
			printf("fdt_addr_ov not set\n");
			return CMD_RET_USAGE;
		}
		fdt_addr_ov = simple_strtoul(var, NULL, 16);
	}

	addr = fdt_addr;
	for (i = 1; i < argc; ++i) {
		int rc;

		if (!strcmp(argv[i], "undefined"))
			/* file is not provided */
			continue;

		rc = load_fdt(argv[i], addr);
		if (rc)
			return rc;

		if (i == 1) {
			set_working_fdt_addr(addr);

			/* from now one we are processing device tree overlays */
			addr = fdt_addr_ov;
		} else {
			/* merge dtb overlay, see "fdt apply" in cmd/fdt.c */
			struct fdt_header *blob;
			ulong filesize;

			filesize = env_get_hex("filesize", 0);
			if (!filesize) {
				printf("Empty device tree\n");
				return CMD_RET_FAILURE;
			}

			blob = map_sysmem(addr, 0);

			fdt_shrink_to_minimum(working_fdt, filesize);

			/* apply method prints messages on error */
			if (fdt_overlay_apply_verbose(working_fdt, blob))
				return CMD_RET_FAILURE;
		}
	}

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	fdtload,	CONFIG_SYS_MAXARGS,	2,	do_fdtload,
	"loads devicetree's and merges them into one",
	"dtb [dtbo..]\n"
	"\t- loads at least one device tree file via ${loadfdt_raw}.\n"
	"\t  If device tree overlays (.dtb or .dtbo) are provided, they are merged into the first tree.\n"
	"\t  This uses the variables fdt_addr and fdt_addr_ov."
);
