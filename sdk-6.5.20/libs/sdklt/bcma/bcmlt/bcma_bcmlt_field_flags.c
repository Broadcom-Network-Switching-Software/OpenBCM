/*! \file bcma_bcmlt_field_flags.c
 *
 * Get field flags from BCMLT default field information.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/bcmlt/bcma_bcmlt.h>

uint32_t
bcma_bcmlt_field_flags(bcmlt_field_def_t *df)
{
    uint32_t flags = 0;

    if (df) {
        if (df->key) {
            flags |= BCMA_BCMLT_FIELD_F_KEY;
        }
        if (df->symbol) {
            flags |= BCMA_BCMLT_FIELD_F_SYMBOL;
        }
        if (df->depth > 0) {
            flags |= BCMA_BCMLT_FIELD_F_ARRAY;
        }
        if (df->width > 64) {
            flags |= BCMA_BCMLT_FIELD_F_WIDE;
        }
    }
    return flags;
}
