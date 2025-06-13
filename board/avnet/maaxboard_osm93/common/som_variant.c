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

#include <common.h>
#include <errno.h>
#include "som_variant.h"

const revision_record_t *find_revision_record(variant_record_t *variants,
		const char *feature, const char *revision)
{
	const variant_record_t *ptr_v;
	int idx;

	for(ptr_v = variants; ptr_v->feature_tag; ptr_v++)
	{
		if (strlen(ptr_v->feature_tag) != strlen(feature)
				|| strcmp(ptr_v->feature_tag, feature))
			continue;

		for (idx=0; idx<ARRAY_SIZE(ptr_v->revisions); idx++)
		{
			const revision_record_t *ptr_r = &ptr_v->revisions[idx];

			if (ptr_r->revision_tag)
				if (strlen(ptr_r->revision_tag) == strlen(revision)
						&& strcmp(ptr_r->revision_tag, revision) == 0)
					return ptr_r;
		}
	}

	printf("Error: variant (feature='%s', revision='%s') not supported!\n",
			feature, revision);

	return NULL;
}

void print_variants(variant_record_t *variants)
{
	const variant_record_t *ptr_v;
	int idx;

	printf("\nSupported variants are:\n\n");
	printf ("%8s | %s \n", "feature", "revision(s)");
	printf ("=============================\n");
	for(ptr_v = variants; ptr_v->feature_tag; ptr_v++)
	{
		printf ("%8s |", ptr_v->feature_tag);

		for (idx=0; idx<ARRAY_SIZE(ptr_v->revisions); idx++)
		{
			const revision_record_t *ptr_r = &ptr_v->revisions[idx];

			if (ptr_r->revision_tag)
				printf (" %2s ", ptr_r->revision_tag);
		}

		printf ("\n");
	}
}
