/*! \file bcmlrd_field_is_internal_field.c
 *
 * Check if the physical field is internal field.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_client.h>


bool
bcmlrd_field_is_internal_field(int unit,
                               uint32_t pt_fid)
{
    bool rv = false;

    if (pt_fid >= BCMLTD_INTERNAL_FIELD_BASE) {
        rv = TRUE;
    } else {
        rv = FALSE;
    }

    return rv;
}
