/** @file      feature.h

    @copyright Copyright (C) 2020 by MSC Technologies GmbH
    SPDX-License Identifier: GPL-2.0-or-later

    @author    Markus Pietrek

    @details   See "Product Nomenclature for MSC COM's" document
*/

#ifndef __MSC_FEATURE_H
#define __MSC_FEATURE_H

struct feature_key_decoded {
	bool valid; /* no decoded entries had errors */

	struct {
		int size_mb; /* DRAM size in MiB */
	} dram;

	struct { /* supported displays */
		bool edp;
		bool lvds;
		bool hdmi;
		bool dsi;
	} display;

	struct { /* periph is specific to the product */
#ifdef CONFIG_TARGET_MSC_SM2S_IMX8
	/* nothing to do yet */
#endif
	} periph;
};

bool board_decode_feature(const char *feature_key, struct feature_key_decoded *fkd);

#endif // __MSC_FEATURE_H
