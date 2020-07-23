/*! \file bcmltm_stats.c
 *
 * Logical Table Manager
 *
 * This module contains routines for the modification, retrieval, and
 * manipulation of Logical Table statistics.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmltm/bcmltm_stats_internal.h>
#include <bsl/bsl.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_STATS

static bcmltm_stats_mgmt_t lt_stats_mgmt[BCMDRD_CONFIG_MAX_UNITS];

/*
 * Accessor macros
 */
#define LT_STATS_MGMT(_u)         lt_stats_mgmt[(_u)]
#define LT_STATS_SIZE(_u)         (LT_STATS_MGMT(_u).stats_array_size)

#define LT_STATS_GET_CB(_u)       (LT_STATS_MGMT(_u).stats_lt_get_cb)

/*******************************************************************************
 * Private functions
 */

/*******************************************************************************
 * Public functions
 */

void
bcmltm_stats_increment(uint32_t *lt_stats,
                       uint32_t stat_field)
{
    if (lt_stats == NULL) {
        /* Can not update missing stats array */
        return;
    }

    if (stat_field < BCMLRD_FIELD_STATS_NUM) {
        lt_stats[stat_field]++;
    }

    return;
}

void
bcmltm_stats_lt_get_register(int unit,
                             bcmltm_stats_lt_get_f stats_lt_get_cb)
{
    LT_STATS_GET_CB(unit) = stats_lt_get_cb;
}

int
bcmltm_stats_lt_get(int unit,
                    uint32_t ltid,
                    uint32_t **lt_stats_p)
{
    if (LT_STATS_GET_CB(unit) == NULL) {
        if (lt_stats_p != NULL) {
            *lt_stats_p = NULL;
        }
        return SHR_E_UNAVAIL;
    }

    return LT_STATS_GET_CB(unit)(unit, ltid, lt_stats_p);
}
