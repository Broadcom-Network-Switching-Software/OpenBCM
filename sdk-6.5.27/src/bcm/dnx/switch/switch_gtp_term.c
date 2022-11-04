/**
 * \file switch_gtp_term.c
 *
 * handle gtp termination set of APIs
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SWITCH

/*
 * Include files.
 * {
 */
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm/switch.h>
#include <include/bcm_int/dnx/stk/stk_sys.h>
#include <bcm_int/dnx/switch/switch_gtp_term.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_switch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_gtp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_flow.h>

/*
 * }
 */

/**
 *  See switch.h
 */
int
bcm_dnx_switch_gtp_terminator_add(
    int unit,
    bcm_switch_gtp_terminator_t * info)
{
    return BCM_E_UNAVAIL;
}

/**
 *  See switch.h
 */
int
bcm_dnx_switch_gtp_terminator_get(
    int unit,
    bcm_switch_gtp_terminator_t * info)
{
    return BCM_E_UNAVAIL;
}

/**
 *  See switch.h
 */
int
bcm_dnx_switch_gtp_terminator_delete(
    int unit,
    bcm_switch_gtp_terminator_t * info)
{
    return BCM_E_UNAVAIL;
}

/**
 *  See switch.h
 */
int
bcm_dnx_switch_gtp_terminator_traverse(
    int unit,
    bcm_switch_gtp_terminator_additional_info_t * additional_info,
    bcm_switch_gtp_terminator_traverse_cb cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}
