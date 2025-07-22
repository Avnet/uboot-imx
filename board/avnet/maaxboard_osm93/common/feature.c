/** @file      feature.c

    @copyright Copyright (C) 2020 by MSC Technologies GmbH
    SPDX-License Identifier: GPL-2.0-or-later

    @author    Markus Pietrek

    @details   See "Product Nomenclature for MSC COM's" document
*/

#include <common.h>
#include <string.h>
#include <hexdump.h>

#include "feature.h"

static bool decode_dram(const char *feature_key, struct feature_key_decoded *fkd)
{
    char dram = feature_key[0];
    if (dram == 'N') {
        /* noop, dram.size_mb is 0 which is the initialized/default value */
    } else {
        if (dram == '8')
            fkd->dram.size_mb = 256;
        else if (dram == '9')
            fkd->dram.size_mb = 512;
        else if (dram < '0' || dram > '7') {
            printf("can't decode DRAM\n");
            return false;
        } else {
            dram = dram - '0';
            fkd->dram.size_mb = 1024;

            while (dram > 0) {
                fkd->dram.size_mb *= 2;
                --dram;
            }
        }
    }

    return true;
}

static bool decode_display(const char *feature_key, struct feature_key_decoded *fkd)
{
    int d;
    char display = feature_key[4];
    if (display == 'N')
        return true; /* fkt.display is already initialized to false */

    d = hex_to_bin(display);
    if (d < 0) {
        printf("can't decode display\n");
        return false;
    }
    if (d & 1)
        fkd->display.edp = true;
    if (d & 2)
        fkd->display.lvds = true;
    if (d & 4)
        fkd->display.hdmi = true;
    if (d & 8)
        fkd->display.dsi = true;

    return true;
}

static bool decode_periph(const char *feature_key, struct feature_key_decoded *fkd)
{
    char periph = feature_key[5];
    (void) periph; /* to hide unused warnings */

#ifdef CONFIG_TARGET_MSC_SM2S_IMX8
    /* nothing to do yet */
#endif

    return true;
}

bool board_decode_feature(const char *feature_key, struct feature_key_decoded *fkd)
{
    if (strlen(feature_key) != 8)
        goto err_format;

    memset(fkd, 0, sizeof(*fkd));

    if (!decode_dram(feature_key, fkd))
        goto err_format;

    if (!decode_display(feature_key, fkd))
        goto err_format;

    if (!decode_periph(feature_key, fkd))
        goto err_format;

    fkd->valid = true;

    return true;

err_format:
    printf("feature key has wrong format\n");
    return false;
}
