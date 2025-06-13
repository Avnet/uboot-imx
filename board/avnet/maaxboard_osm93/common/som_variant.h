#ifndef SOM_VARIANT_H
#define SOM_VARIANT_H

/*
 * Copyright (C) 2022 Avnet Embedded
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

struct dram_timing_info;

typedef struct dram_config {
	uint32_t				size[2];
	struct dram_timing_info *timing;
} dram_config_t;

typedef struct revision_record {
	const char *			revision_tag;
	dram_config_t			dram_cfg;
} revision_record_t;

typedef struct variant_record {
	const char *			feature_tag;
	revision_record_t 		revisions[10];
} variant_record_t;

#define REVISION_RECORD(REVISION, DRAM_SIZE0, DRAM_SIZE1, DRAM_TIMING) \
		{ \
			.revision_tag	= REVISION, \
			.dram_cfg		= { \
				.size			= { DRAM_SIZE0, DRAM_SIZE1 }, \
				.timing			= &DRAM_TIMING, \
			}, \
		}

#define FEATURE_RECORD(FEATURE, ...) \
		{ \
			.feature_tag	= FEATURE, \
			.revisions	= { \
					__VA_ARGS__ \
			}, \
		}

const revision_record_t *find_revision_record(variant_record_t *variants,
		const char *feature, const char *revision);

void print_variants(variant_record_t *variants);

#endif /* SOM_VARIANT_H */
