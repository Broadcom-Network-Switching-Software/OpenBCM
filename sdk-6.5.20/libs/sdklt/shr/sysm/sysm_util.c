/*! \file sysm_util.c
 *
 * System manager utitlity functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <shr/shr_sysm.h>

#define BSL_LOG_MODULE BSL_LS_SHR_SYSM

/*******************************************************************************
 * Local definitions
 */

static const char *(*sysm_comp_name)(uint32_t comp_id);

/*******************************************************************************
 * Public functions
 */

void
shr_sysm_comp_name_init(const char *(*comp_name)(uint32_t comp_id))
{
    sysm_comp_name = comp_name;
}

const char *
shr_sysm_comp_name(uint32_t comp_id)
{
    const char *name;

    if (sysm_comp_name == NULL || (name = sysm_comp_name(comp_id)) == NULL) {
        return "-";
    }
    return name;
}
