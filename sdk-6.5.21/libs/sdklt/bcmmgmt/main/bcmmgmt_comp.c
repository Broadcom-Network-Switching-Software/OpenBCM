/*! \file bcmmgmt_comp.c
 *
 * System manager component name functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <shr/shr_types.h>

#include <bcmmgmt/bcmmgmt.h>
#include <bcmmgmt/bcmmgmt_comp.h>

#define BSL_LOG_MODULE BSL_LS_BCMMGMT_INIT

/*******************************************************************************
 * Local definitions
 */

static shr_enum_map_t comp_enum_map[] = {
#define BCMMGMT_COMPLIST_ENTRY(_name, _id) \
    { #_name, BCMMGMT_##_name##_COMP_ID },
#include <bcmmgmt/bcmmgmt_complist.h>
    { NULL, BCMMGMT_MAX_COMP_ID }
};

/*******************************************************************************
 * Public functions
 */

uint32_t
bcmmgmt_comp_id_from_name(const char *name)
{
    shr_enum_map_t *cem = &comp_enum_map[0];

    while (cem->name) {
        if (sal_strcasecmp(name, cem->name) == 0) {
            return cem->val;
        }
        cem++;
    }
    return BCMMGMT_MAX_COMP_ID;
}

const char *
bcmmgmt_comp_name_from_id(uint32_t comp_id)
{
    shr_enum_map_t *cem = &comp_enum_map[0];

    while (cem->name) {
        if ((int)comp_id == cem->val) {
            return cem->name;
        }
        cem++;
    }
    return NULL;
}
