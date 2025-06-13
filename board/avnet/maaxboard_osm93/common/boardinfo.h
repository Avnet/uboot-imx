#ifndef __MSC_BOARDINFO_H__
#define __MSC_BOARDINFO_H__

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

#define BI_VER_MAJ	1
#define BI_VER_MIN	0

#define BI_COMPANY_LEN		3
#define BI_FEATURE_LEN		8
#define BI_SERIAL_LEN		11
#define BI_REVISION_LEN		2

#define BI_COMPANY_BIT		(1<<0)
#define BI_FEATURE_BIT		(1<<1)
#define BI_SERIAL_BIT		(1<<2)
#define BI_REVISION_BIT		(1<<3)
#define BI_BSP_SPECIFIC_BIT	(1<<4)

typedef struct bi_head {
	uint8_t		magic[4];
	uint8_t		version;
	uint8_t		v_min;
	uint16_t	body_len;
	uint16_t	body_off;
	uint16_t	body_chksum;
	uint32_t	__reserved[2];
} bi_head_t;

typedef struct bi_v1_0 {
	uint32_t	__feature_bits;
	char		company	[BI_COMPANY_LEN + 1];
	char		feature[BI_FEATURE_LEN + 1];
	char		serial_number[BI_SERIAL_LEN + 1];
	char		revision[BI_REVISION_LEN + 1];
	uint32_t	__reserved1;
	uint16_t	__reserved2;
} bi_v1_0_t;

typedef struct bi_v1_1 {
	uint32_t	__feature_bits;
	char		company	[BI_COMPANY_LEN + 1];
	char		feature[BI_FEATURE_LEN + 1];
	char		serial_number[BI_SERIAL_LEN + 1];
	char		revision[BI_REVISION_LEN + 1];
	uint8_t		bsp_specific; /* optionally enabled via BI_HAS_BSP_SPECIFIC, content is up to the BSP */
	uint32_t	__reserved1;
	uint8_t		__reserved2;
} bi_v1_1_t;

typedef struct board_info {
	bi_head_t		head;
	union {
		bi_v1_0_t	v1_0;
		bi_v1_1_t	v1_1; /* requires #define BI_USE_VERSION_1_1 to be used in BSP */
	} body;
	uint32_t		boot_count;
} board_info_t;

#define BI_STR "Boardinfo"

#define BI_CALC_VER(MAJ, MIN) \
	((MAJ)<<4 | (MIN))

#define BI_VER_1_0	BI_CALC_VER	(1, 0)
#define BI_VER_1_1	BI_CALC_VER	(1, 1)

#ifdef BI_USE_VERSION_1_1
# define BI_VERSION BI_VER_1_1
#else
# define BI_VERSION BI_VER_1_0
#endif

#define BI_HAS_FEATURE(BI, F) \
	((BI)->body.v1_0.__feature_bits & BI_##F##_BIT)

#define BI_ENABLE_FEATURE(BI, F) \
	do { \
		(BI)->body.v1_0.__feature_bits |= BI_##F##_BIT; \
	} \
	while(0);

#define BI_GET_BODY(BI, MAJ, MIN) \
	((BI)->body.v##MAJ##_##MIN)

#define BI_PRINT(format, ...) \
	do { \
		printf("%s: ", BI_STR); \
		printf(format, ## __VA_ARGS__); \
	} \
	while(0);

#if defined(DEBUG)
  #define BI_DEBUG(format, ...) \
	do { \
		printf("%s: ", BI_STR); \
		printf(format, ## __VA_ARGS__); \
	} \
	while(0);
#else /* defined(DEBUG) */
  #define BI_DEBUG(format, ...)
#endif /* defined(DEBUG) */

board_info_t *bi_alloc(void);
board_info_t *bi_read(void);
int read_boardinfo(int offset, uint8_t *buffer, int size);
int write_boardinfo(int offset, uint8_t *buffer, int size);

const char* bi_get_company(const board_info_t *bi);
const char* bi_get_form_factor(const board_info_t *bi);
const char* bi_get_platform(const board_info_t *bi);
const char* bi_get_processor(const board_info_t *bi);
const char* bi_get_feature(const board_info_t *bi);
const char* bi_get_serial(const board_info_t *bi);
const char* bi_get_revision(const board_info_t *bi);
const char* bi_not_available_string(void);

bool bi_has_bsp_specific(const board_info_t *bi);
u8 bi_get_bsp_specific(const board_info_t *bi);

uint32_t bi_get_boot_count(const board_info_t *bi);

int bi_inc_boot_count(board_info_t *bi);

void bi_print(const board_info_t *bi);

int bi_set_feature(board_info_t *bi, const char *string);
int bi_set_revision(board_info_t *bi, const char *string);
int bi_set_bsp_specific(board_info_t *bi, u8 bsp_specific);

int bi_save(board_info_t *bi);

#endif /* __MSC_BOARDINFO_H__ */
