/*! \file bcmlrd_field_is_magic_field.c
 *
 * Check if the physical field is magic field.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmlrd/bcmlrd_client.h>


bool
bcmlrd_field_is_magic_field(int unit,
                            uint32_t pt_fid)
{
    bool rv = false;

    switch (pt_fid) {
    case BCMLRD_FIELD_INDEX :
    case BCMLRD_FIELD_PORT :
    case BCMLRD_FIELD_ENTRY_PRIORITY :
    case BCMLRD_FIELD_INSTANCE :
    case BCMLRD_FIELD_TABLE_SEL :
    case BCMLRD_FIELD_TRACK_INDEX :
    case BCMLRD_FIELD_PT_SUPPRESS :
    case BCMLRD_FIELD_SELECTOR :
    case BCMLRD_FIELD_LDTYPE:
        rv = TRUE;
        break;
    default:
        rv = FALSE;
        break;
    }

    return rv;
}
