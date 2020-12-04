/*! \file bcmfp_strings.c
 *
 *  APIs to print meaningful names to different
 *  enumerations in BCMFP. These APIs can be used
 *  in LOG messages.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_strings_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

char *
bcmfp_stage_string(bcmfp_stage_id_t stage_id) {

    static char *stage_strings[] = BCMFP_STAGE_STRINGS;

    if (stage_id < BCMFP_STAGE_ID_COUNT && stage_id >= 0) {
       return stage_strings[stage_id];
    }
    return "STAGE_UNKNOWN";
}

char *
bcmfp_group_mode_string(bcmfp_group_mode_t group_mode) {

    static char *group_mode_strings[] = BCMFP_GROUP_MODE_STRINGS;

    if (group_mode < BCMFP_GROUP_MODE_COUNT && group_mode >= 0) {
        /* coverity[overrun-local] */
       return group_mode_strings[group_mode];
    }
    return "GROUP_MODE_UNKNOWN";
}
