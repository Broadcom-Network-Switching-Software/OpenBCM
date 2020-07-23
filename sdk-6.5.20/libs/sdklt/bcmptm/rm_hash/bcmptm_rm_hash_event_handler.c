/*! \file bcmptm_rm_hash_event_handler.c
 *
 * Physical table register function for hash resource manager
 *
 * This file contains physical table register function for physical hash tables.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmevm/bcmevm_api.h>
#include "bcmptm_rm_hash.h"
#include "bcmptm_rm_hash_pt_register.h"
#include "bcmptm_rm_hash_lt_ctrl.h"
/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMHASH

/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */


/*******************************************************************************
 * Private Functions
 */
/*!
 * \brief Hash resource group change event handler.
 *
 * \param [in] unit Unit number.
 * \param [in] event Resource group change event.
 * \param [in] ev_data Resource group change event data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static void
rm_hash_grp_change_event_handler(int unit, const char *event, uint64_t ev_data)
{
    bool separate = FALSE;

    (void)bcmptm_rm_hash_bank_has_separated_sid(unit, &separate);
    /* Updated hash vector configuration. */
    if (separate == FALSE) {
        bcmptm_rm_hash_sid_based_event_handler(unit);
    } else {
        bcmptm_rm_hash_ltid_based_event_handler(unit);
    }
    bcmptm_rm_hash_lt_bank_enable_init(unit);
    bcmptm_rm_hash_lt_bkt_mode_init(unit);
}

/*!
 * \brief Hash resource LT reordering depth change event handler.
 *
 * \param [in] unit Unit number.
 * \param [in] event Hash reordering depth change event.
 * \param [in] ev_data Hash reordering depth change event data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static void
rm_hash_lt_move_depth_change_handler(int unit, const char *event, uint64_t ev_data)
{
    (void)bcmptm_rm_hash_move_depth_change(unit);
}

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_hash_event_handle_init(int unit)
{

    SHR_FUNC_ENTER(unit);
    /*
     * Register handler to handle bank count, bank offset and
     * robust configuration change.
     */
    SHR_IF_ERR_EXIT(
        bcmevm_register_published_event(unit,
                                        BCMPTM_EVENT_UFT_GROUP_CHANGE_COMMIT,
                                        rm_hash_grp_change_event_handler));
    SHR_IF_ERR_EXIT(
        bcmevm_register_published_event(unit,
                                        BCMPTM_EVENT_UFT_BANK_CHANGE_COMMIT,
                                        rm_hash_grp_change_event_handler));
    /* Register handler to handle LT hash reordering depth change. */
    SHR_IF_ERR_EXIT(
        bcmevm_register_published_event(unit,
                                        BCMPTM_EVENT_UFT_LT_MOVEDEPTH_COMMIT,
                                        rm_hash_lt_move_depth_change_handler));

exit:
    SHR_FUNC_EXIT();
}
