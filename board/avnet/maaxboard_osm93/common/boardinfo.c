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

#include <linux/types.h>
#include <linux/errno.h>
#include <asm/mach-imx/mxc_i2c.h>
#include <i2c.h>
#include <command.h>
#include <asm/arch/sys_proto.h>
#include "string.h"
#include "boardinfo.h"

static board_info_t board_info;
static uint32_t dirty = 0;

static bool bi_check_magic(const board_info_t *bi)
{
	if (bi == NULL) goto error;

	if (bi->head.magic[0] != 'm' || bi->head.magic[1] != 's' || bi->head.magic[2] != 'c' )
		goto error;

	return true;

error:
	BI_DEBUG("Magic check failed. \n");
	return false;
}

static int bi_calc_checksum(const board_info_t *bi, uint16_t *chksum)
{
	int i;
	const unsigned char *ptr;

	if (bi == NULL) return -EINVAL;

	if (bi->head.version == BI_VER_1_0 || bi->head.version == BI_VER_1_1) {
		const bi_v1_0_t *body = &BI_GET_BODY(bi, 1, 0);
		ptr = (const unsigned char*)body;
		*chksum = 0;
		for (i = 0; i < sizeof(*body); i++)
			*chksum += ptr[i];
	}
	else {
		return -EINVAL;
	}

	return 0;
}

static bool bi_check_checksum(const board_info_t *bi)
{
	uint16_t chksum;
	int ret;

	if (bi == NULL) goto error;

	ret = bi_calc_checksum(bi, &chksum);
	if (ret)
		goto error;

	if (chksum != bi->head.body_chksum)
		goto error;

	return true;
error:
	BI_DEBUG("Checksum check failed. \n");
	return false;
}

static bool bi_ckeck(const board_info_t *bi)
{
	return bi_check_magic(bi) && bi_check_checksum(bi);
}

void bi_print(const board_info_t *bi)
{
	if (bi == NULL) return;

	printf("------------------------------\n");
	printf("company .......... %s\n", bi_get_company(bi));
	printf("form factor ...... %s\n", bi_get_form_factor(bi));
	printf("platform ......... %s\n", bi_get_platform(bi));
	printf("processor ........ %s\n", bi_get_processor(bi));
	printf("feature .......... %s\n", bi_get_feature(bi));
	printf("serial ........... %s\n", bi_get_serial(bi));
	printf("revision (MES) ... %s\n", bi_get_revision(bi));
#ifdef BI_HAS_BSP_SPECIFIC
	if (bi_has_bsp_specific(bi))
		printf("bsp............... 0x%02x\n", bi_get_bsp_specific(bi));
#endif
	printf("boot count ....... %d\n", bi_get_boot_count(bi));
	printf("------------------------------\n");
}

__weak int read_boardinfo(int offset, uint8_t *buffer, int size)
{
	puts("Boardinfo read method not implemented! \n");
	return -ENODATA;
}

board_info_t *bi_alloc(void)
{
	memset(&board_info, 0, sizeof(board_info));
	return &board_info;
}

board_info_t *bi_read(void)
{
	int ret;

	memset(&board_info, 0, sizeof(board_info));

	ret = read_boardinfo(0, (uint8_t*)&board_info, sizeof(board_info));
	if (ret)
		goto error;

	if (!bi_ckeck(&board_info)) {
		puts("ERROR: check of Boarddata failed\n");
		ret = -EINVAL;
		goto error;
	}

	return &board_info;

error:
	memset(&board_info, 0, sizeof(board_info));
	return NULL;
}

__weak int write_boardinfo(int offset, uint8_t *buffer, int size)
{
	puts("Boardinfo write method not implemented! \n");
	return -ENODATA;
}

int bi_save(board_info_t *bi)
{
	bi_head_t *head;
	uint16_t chksum = 0;

	if (bi == NULL) return -ENODATA;

	head = &bi->head;
	head->magic[0] = 'm';
	head->magic[1] = 's';
	head->magic[2] = 'c';
	head->version  = BI_VERSION;
	bi_calc_checksum(bi, &chksum);
	head->body_chksum = chksum;
	head->body_off = sizeof(bi_head_t);
	head->body_len = sizeof(bi_v1_0_t); /* v1_1 has the same size as v1_0 */
	if (BI_VERSION == BI_VER_1_0) {
		 BI_GET_BODY(bi, 1, 0).__reserved1 = 0;
		 BI_GET_BODY(bi, 1, 0).__reserved2 = 0;
	} else if (BI_VERSION == BI_VER_1_1) {
		 BI_GET_BODY(bi, 1, 1).__reserved1 = 0;
		 BI_GET_BODY(bi, 1, 1).__reserved2 = 0;
	}

	return write_boardinfo(0, (uint8_t*)bi, sizeof(*bi));
}

static int bi_set_company(board_info_t *bi, const char *str)
{
#if defined(CONFIG_BOARDINFO_PROTECTION)
	if (BI_HAS_FEATURE(bi, COMPANY))
	{
		BOARDINFO_PRINT("not allowed, company already set. \n");
		return -EINVAL;
	}
#endif /* defined(CONFIG_BOARDINFO_PROTECTION) */

	if (strlen(str) > BI_COMPANY_LEN)
	{
		BI_PRINT("string too long for company (max %d characters). \n",
				BI_COMPANY_LEN);
		return -EINVAL;
	}

	memset(BI_GET_BODY(bi, 1 , 0).company, 0,
			sizeof(BI_GET_BODY(bi, 1 , 0).company));
	strncpy(BI_GET_BODY(bi, 1 , 0).company, str, BI_COMPANY_LEN);

	BI_ENABLE_FEATURE(bi, COMPANY);
	dirty |= BI_COMPANY_BIT;

	return 0;
}

const char* bi_get_company(const board_info_t *bi)
{
	if (BI_HAS_FEATURE(bi, COMPANY))
		return BI_GET_BODY(bi, 1, 0).company;
	return bi_not_available_string();
}

__weak const char* bi_get_form_factor(const board_info_t *bi)
{
	return bi_not_available_string();
}

__weak const char* bi_get_platform(const board_info_t *bi)
{
	return bi_not_available_string();
}

__weak const char* bi_get_processor(const board_info_t *bi)
{
	return bi_not_available_string();
}

int bi_set_feature(board_info_t *bi, const char *string)
{
#if defined(CONFIG_BOARDINFO_PROTECTION)
	if (BI_HAS_FEATURE(bi, FEATURE))
	{
		BI_PRINT("not allowed, feature key already set. \n");
		return -EINVAL;
	}
#endif /* defined(CONFIG_BOARDINFO_PROTECTION) */

	if (strlen(string) > BI_FEATURE_LEN)
	{
		BI_PRINT("string too long for feature key (max %d characters). \n",
				BI_FEATURE_LEN);
		return -EINVAL;
	}

	memset(BI_GET_BODY(bi, 1 , 0).feature, 0,
			sizeof(BI_GET_BODY(bi, 1 , 0).feature));
	strncpy(BI_GET_BODY(bi, 1 , 0).feature, string, BI_FEATURE_LEN);

	BI_ENABLE_FEATURE(bi, FEATURE);
	dirty |= BI_FEATURE_BIT;

	return 0;
}

const char* bi_get_feature(const board_info_t *bi)
{
	if (BI_HAS_FEATURE(bi, FEATURE))
		return BI_GET_BODY(bi, 1, 0).feature;
	return bi_not_available_string();
}

static int bi_set_serial(board_info_t *bi, const char *string)
{
#if defined(CONFIG_BOARDINFO_PROTECTION)
	if (BI_HAS_FEATURE(bi, SERIAL))
	{
		BOARDINFO_PRINT("not allowed, serial number already set. \n");
		return -EINVAL;
	}
#endif /* defined(CONFIG_BOARDINFO_PROTECTION) */

	if (strlen(string) > BI_SERIAL_LEN)
	{
		BI_PRINT("string too long for serial number (max %d characters). \n",
				BI_SERIAL_LEN);
		return -EINVAL;
	}

	memset(BI_GET_BODY(bi, 1 , 0).serial_number, 0,
			sizeof(BI_GET_BODY(bi, 1 , 0).serial_number));
	strncpy(BI_GET_BODY(bi, 1 , 0).serial_number, string, BI_SERIAL_LEN);

	BI_ENABLE_FEATURE(bi, SERIAL);
	dirty |= BI_SERIAL_BIT;

	return 0;
}

const char* bi_get_serial(const board_info_t *bi)
{
	if (BI_HAS_FEATURE(bi, SERIAL))
		return BI_GET_BODY(bi, 1, 0).serial_number;
	return bi_not_available_string();
}

int bi_set_revision(board_info_t *bi, const char *string)
{
#if defined(CONFIG_BOARDINFO_PROTECTION)
	if (BI_HAS_FEATURE(bi, REVISION))
	{
		BOARDINFO_PRINT("not allowed, serial number already set. \n");
		return -EINVAL;
	}
#endif /* defined(CONFIG_BOARDINFO_PROTECTION) */

	if (strlen(string) > BI_REVISION_LEN)
	{
		BI_PRINT("string too long for revision number (max %d characters). \n",
				BI_REVISION_LEN);
		return -EINVAL;
	}

	memset(BI_GET_BODY(bi, 1 , 0).revision, 0,
			sizeof(BI_GET_BODY(bi, 1 , 0).revision));
	strncpy(BI_GET_BODY(bi, 1 , 0).revision, string, BI_REVISION_LEN);

	BI_ENABLE_FEATURE(bi, REVISION);
	dirty |= BI_REVISION_BIT;

	return 0;
}

const char* bi_get_revision(const board_info_t *bi)
{
	if (BI_HAS_FEATURE(bi, REVISION))
		return BI_GET_BODY(bi, 1, 0).revision;
	return bi_not_available_string();
}

#ifdef BI_HAS_BSP_SPECIFIC
bool bi_has_bsp_specific(const board_info_t *bi)
{
	return !!BI_HAS_FEATURE(bi, BSP_SPECIFIC);
}

u8 bi_get_bsp_specific(const board_info_t *bi)
{
	return BI_GET_BODY(bi, 1 , 1).bsp_specific;
}

int bi_set_bsp_specific(board_info_t *bi, u8 bsp_specific)
{
	BI_GET_BODY(bi, 1 , 1).bsp_specific = bsp_specific;
	BI_ENABLE_FEATURE(bi, BSP_SPECIFIC);
	dirty |= BI_BSP_SPECIFIC_BIT;

	return 0;
}
#endif

int bi_inc_boot_count(board_info_t *bi)
{
	bi->boot_count += 1;

	return write_boardinfo(offsetof(board_info_t, boot_count),
			(uint8_t *)&bi->boot_count, sizeof(bi->boot_count));
}

uint32_t bi_get_boot_count(const board_info_t *bi)
{
	return bi->boot_count;
}

static bool bi_is_complete(const board_info_t *bi)
{
	if (bi == NULL) return false;
	return BI_HAS_FEATURE(bi, COMPANY)
			&& BI_HAS_FEATURE(bi, FEATURE)
#ifdef BI_HAS_BSP_SPECIFIC
			&& BI_HAS_FEATURE(bi, BSP_SPECIFIC)
#endif
			&& BI_HAS_FEATURE(bi, SERIAL);
}

const char* bi_not_available_string(void)
{
	return "N/A";
}

#if defined(CONFIG_CMD_BOARDINFO)

static int do_boardinfo_show(struct cmd_tbl *cmdtp, int flag, int argc,
		char * const argv[])
{
	bi_print(&board_info);
	return CMD_RET_SUCCESS;
}

static int do_boardinfo_company(struct cmd_tbl *cmdtp, int flag, int argc,
		char * const argv[])
{
	if (bi_set_company(&board_info, str_tolower(argv[1])))
		return CMD_RET_FAILURE;

	BI_PRINT("OK \n");
	return CMD_RET_SUCCESS;
}

static int do_boardinfo_feature(struct cmd_tbl *cmdtp, int flag, int argc,
		char * const argv[])
{
	if (bi_set_feature(&board_info, str_toupper(argv[1])))
		return CMD_RET_FAILURE;

	BI_PRINT("OK \n");
	return CMD_RET_SUCCESS;
}

static int do_boardinfo_serial(struct cmd_tbl *cmdtp, int flag, int argc,
		char * const argv[])
{
	if (bi_set_serial(&board_info, argv[1]))
		return CMD_RET_FAILURE;

	BI_PRINT("OK \n");
	return CMD_RET_SUCCESS;
}

static int do_boardinfo_revision(struct cmd_tbl *cmdtp, int flag, int argc,
		char * const argv[])
{
	if (bi_set_revision(&board_info, argv[1]))
		return CMD_RET_FAILURE;

	BI_PRINT("OK \n");
	return CMD_RET_SUCCESS;
}

#ifdef BI_HAS_BSP_SPECIFIC
static int do_boardinfo_bsp(struct cmd_tbl *cmdtp, int flag, int argc,
		char * const argv[])
{
	u8 bsp = simple_strtoul(argv[1], NULL, 16);

	if (bi_set_bsp_specific(&board_info, bsp))
		return CMD_RET_FAILURE;

	BI_PRINT("OK \n");
	return CMD_RET_SUCCESS;
}
#endif

static int do_boardinfo_save(struct cmd_tbl *cmdtp, int flag, int argc,
		char * const argv[])
{
	if (dirty) {
		int ret = bi_save(&board_info);
		if (ret == 0)
			dirty = 0;

		BI_PRINT("save %s. \n", (ret == 0) ? "done" : "failed");
	}
	else
		BI_PRINT("nothing to save, ignore. \n");

	return CMD_RET_SUCCESS;
}

static int do_boardinfo_complete(struct cmd_tbl *cmdtp, int flag, int argc,
		char * const argv[])
{
	if (bi_is_complete(&board_info) == false) {
		BI_PRINT("NOK, not complete. \n");
		return CMD_RET_FAILURE;
	}

	if (dirty) {
		BI_PRINT("NOK, dirty (not saved yet). \n");
		return CMD_RET_FAILURE;
	}

	BI_PRINT("OK, complete. \n");
	return CMD_RET_SUCCESS;
}

static struct cmd_tbl cmd_boardinfo_sub[] = {
	U_BOOT_CMD_MKENT(show, 2, 0, do_boardinfo_show, "", ""),
	U_BOOT_CMD_MKENT(company, 3, 0, do_boardinfo_company, "", ""),
	U_BOOT_CMD_MKENT(feature, 3, 0, do_boardinfo_feature, "", ""),
	U_BOOT_CMD_MKENT(serial, 3, 0, do_boardinfo_serial, "", ""),
	U_BOOT_CMD_MKENT(revision, 3, 0, do_boardinfo_revision, "", ""),
#ifdef BI_HAS_BSP_SPECIFIC
	U_BOOT_CMD_MKENT(bsp, 3, 0, do_boardinfo_bsp, "", ""),
#endif
	U_BOOT_CMD_MKENT(save, 2, 0, do_boardinfo_save, "", ""),
	U_BOOT_CMD_MKENT(complete, 2, 0, do_boardinfo_complete, "", ""),
};

static int do_boardinfo(struct cmd_tbl *cmdtp, int flag, int argc,
		char * const argv[])
{
	struct cmd_tbl *c;

	argc--; argv++;

	c = find_cmd_tbl(argv[0], cmd_boardinfo_sub, ARRAY_SIZE(cmd_boardinfo_sub));

	if (c)
		return c->cmd(cmdtp, flag, argc, argv);
	else
		return CMD_RET_USAGE;
}

U_BOOT_CMD(
	boardinfo, 3, 1, do_boardinfo,
	"Miscellaneous boardinfo commands",
	"show                - read and dump boardinfo \n"
	"boardinfo company <string>    - set company string (eg. msc).\n"
	"boardinfo feature <string>    - set feature key.\n"
	"boardinfo serial <string>     - set serial number.\n"
	"boardinfo revision <string>   - set revision (MES).\n"
#ifdef BI_HAS_BSP_SPECIFIC
	"boardinfo bsp <hex>           - set BSP specific flags.\n"
#endif
	"boardinfo save                - save boardinfo to EEPROM \n"
	"boardinfo complete            - check boardinfo completeness and integrity\n"
);

#endif /* defined(CONFIG_CMD_BOARDINFO) */
