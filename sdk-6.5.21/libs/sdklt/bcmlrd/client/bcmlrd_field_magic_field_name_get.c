/*! \file bcmlrd_field_magic_field_name_get.c
 *
 * Get the magic field name.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmlrd/bcmlrd_client.h>


const char *
bcmlrd_field_magic_field_name_get(int unit,
                                  uint32_t pt_fid)
{
    const char *name = NULL;

    switch (pt_fid) {
    case BCMLRD_FIELD_INDEX:
        name = "__INDEX";
        break;
    case BCMLRD_FIELD_PORT:
        name = "__PORT";
        break;
    case BCMLRD_FIELD_ENTRY_PRIORITY:
        name = "__ENTRY_PRIORITY";
        break;
    case BCMLRD_FIELD_INSTANCE:
        name = "__INSTANCE";
        break;
    case BCMLRD_FIELD_TABLE_SEL:
        name = "__TABLE_SEL";
        break;
    case BCMLRD_FIELD_TRACK_INDEX:
        name = "__TRACK_INDEX";
        break;
    case BCMLRD_FIELD_PT_SUPPRESS:
        name = "__PT_SUPPRESS";
        break;
    case BCMLRD_FIELD_SELECTOR:
        name = "__SELECTOR";
        break;
    case BCMLRD_FIELD_LDTYPE:
        name = "__LDTYPE";
        break;
    default:
        name = NULL;
        break;
    }

    return name;
}
