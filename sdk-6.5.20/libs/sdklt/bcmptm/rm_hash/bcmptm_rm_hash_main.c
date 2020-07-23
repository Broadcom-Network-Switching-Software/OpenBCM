/*! \file bcmptm_rm_hash_main.c
 *
 * Resource Manager for Hash Table
 *
 * This file contains the resource manager for physical hash tables
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
#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include "bcmptm_rm_hash.h"
#include "bcmptm_rm_hash_lt_ctrl.h"
#include "bcmptm_rm_hash_pt_register.h"
#include "bcmptm_rm_hash_state_mgmt.h"
#include "bcmptm_rm_hash_trans_mgmt.h"
#include "bcmptm_rm_hash_reordering.h"
#include "bcmptm_rm_hash_robust_hash.h"
#include "bcmptm_rm_hash_lmem.h"
#include "bcmptm_rm_hash_store.h"

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
 * Public Functions
 */


/*
 * bcmptm_rm_hash_init
 *
 * Init resource manager for hash table
 */
int
bcmptm_rm_hash_init(int unit, bool warm)
{
    int rv;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmptm_rm_hash_lmem_init(unit));
    SHR_IF_ERR_EXIT(bcmptm_rm_hash_lt_rbust_info_init(unit));
    rv = bcmptm_rm_hash_pt_info_vec_init(unit, warm);
    
    if (rv == SHR_E_UNAVAIL) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(rv);
    SHR_IF_ERR_EXIT(bcmptm_rm_hash_lt_hash_vec_attr_update(unit));
    SHR_IF_ERR_EXIT(bcmptm_rm_hash_lt_bank_enable_init(unit));
    SHR_IF_ERR_EXIT(bcmptm_rm_hash_lt_rbust_hash_config(unit, warm));
    SHR_IF_ERR_EXIT(bcmptm_rm_hash_lt_bkt_mode_init(unit));
    SHR_IF_ERR_EXIT(bcmptm_rm_hash_blk_ctrl_alloc(unit, warm));
    SHR_IF_ERR_EXIT(bcmptm_rm_hash_trans_state_elems_alloc(unit, warm));
    SHR_IF_ERR_EXIT(bcmptm_rm_hash_trans_state_elems_config(unit, warm));
    SHR_IF_ERR_EXIT(bcmptm_rm_hash_lt_ctrl_move_depth_init(unit));
    SHR_IF_ERR_EXIT(bcmptm_rm_hash_lt_ctrl_pipe_mode_init(unit));
    SHR_IF_ERR_EXIT(bcmptm_rm_hash_event_handle_init(unit));
    SHR_IF_ERR_EXIT(bcmptm_rm_hash_dev_lt_ctrl_dump(unit));
    SHR_IF_ERR_EXIT(bcmptm_rm_hash_pt_reg_info_dump(unit));
    SHR_IF_ERR_EXIT(bcmptm_rm_hash_ent_move_info_init(unit));
    SHR_IF_ERR_EXIT(bcmptm_rm_hash_store_init(unit, warm));
exit:
    SHR_FUNC_EXIT();
}

/*
 * bcmptm_rm_hash_cleanup
 *
 * Clean up resource manager for hash table
 */
int
bcmptm_rm_hash_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    bcmptm_rm_hash_store_cleanup(unit);
    SHR_IF_ERR_EXIT(bcmptm_rm_hash_lt_ctrl_cleanup(unit));
    SHR_IF_ERR_EXIT(bcmptm_rm_hash_ent_move_info_cleanup(unit));
    SHR_IF_ERR_EXIT(bcmptm_rm_hash_trans_state_elems_cleanup(unit));
    bcmptm_rm_hash_rbust_info_cleanup(unit);
    bcmptm_rm_hash_lmem_cleanup(unit);
    bcmptm_rm_hash_pt_reg_info_cleanup(unit);
    bcmptm_rm_hash_blk_ctrl_cleanup(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_comp_config(int unit, bool warm, bcmptm_sub_phase_t phase)
{
    SHR_FUNC_ENTER(unit);

    switch (phase) {
    case BCMPTM_SUB_PHASE_1:
        break;
    case BCMPTM_SUB_PHASE_2:
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_lt_ctrl_init(unit));
        break;
    case BCMPTM_SUB_PHASE_3:
        break;
    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_lt_reg(int unit,
                      bcmltd_sid_t ltid,
                      bcmptm_sub_phase_t phase,
                      uint32_t *lt_state_offset)
{
    SHR_FUNC_ENTER(unit);

    switch (phase) {
    case BCMPTM_SUB_PHASE_1:
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_pt_info_pre_config(unit));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_pt_reg_info_add(unit, ltid));
        break;
    case BCMPTM_SUB_PHASE_2:
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_lt_ctrl_add(unit, ltid));
        break;
    case BCMPTM_SUB_PHASE_3:
        break;
    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}
