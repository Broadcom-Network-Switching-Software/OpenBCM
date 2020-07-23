/** \file algo/ipq/ipq_alloc_mngr.c
 *
 * Implementation for advanced IPQ resource manager.
 *
 * Manage queue allocation.
 * Split queues bitmap to regions, and have the ability to allocate a
 * queue from a specific region.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RESMNGR

/**
* Include files:
* {
*/

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_ipq_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_ipq_alloc_mngr_access.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_types.h>
#include <bcm_int/dnx/algo/ipq/ipq_alloc_mngr.h>
#include <bcm_int/dnx/cosq/ingress/ipq.h>

/** 
 * } 
 */

/**
* Functions:
* {
*/

/**
 * See .h file.
 */
void
dnx_algo_ipq_tc_map_profile_print_cb(
    int unit,
    const void *data)
{
    int tc;
    dnx_ipq_tc_map_t *tc_mapping = (dnx_ipq_tc_map_t *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    for (tc = 0; tc < DNX_COSQ_NOF_TC; tc++)
    {
        SW_STATE_PRETTY_PRINT_ADD_ARR(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "DP -> COSQ", tc_mapping->offset[tc],
                                      NULL, NULL, DNX_COSQ_NOF_DP);
    }
    SW_STATE_PRETTY_PRINT_FINISH();
}

/**
 * }
 */
