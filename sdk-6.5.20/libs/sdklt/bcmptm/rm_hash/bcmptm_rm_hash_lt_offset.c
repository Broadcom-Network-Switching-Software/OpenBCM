/*! \file bcmptm_rm_hash_lt_offset.c
 *
 * Miscellaneous.
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
#include <bcmlrd/bcmlrd_table.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>


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

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_hash_ltid_identify(int unit, bool warm)
{
    bcmltd_sid_t ltid;
    const lt_mreq_info_t *lrd_info = NULL;
    uint32_t lt_state_offset;
    int tmp_rv;
    bcmptm_sub_phase_t phase;
    SHR_FUNC_ENTER(unit);

    /* Phase 1
     * Identify LTIDs that are managed by rm_hash
     */
    phase = BCMPTM_SUB_PHASE_1;
    SHR_IF_ERR_EXIT(
        bcmptm_rm_hash_comp_config(unit, warm, phase));

    /* There may be 100 tables with LTIDs 0-99 in multi-unit system.
     * Unit may support only 20 tables but with LTIDs ranging from 80 - 99
     * BCMLTD_TABLE_COUNT will be 100 in this case and bcmlrd_table_count_get()
     * will be 20 for this unit. */

    for (ltid = 0; ltid < BCMLTD_TABLE_COUNT; ltid++) {
        lrd_info = NULL;
        tmp_rv = SHR_E_NONE;
        tmp_rv = bcmlrd_lt_mreq_info_get(unit, ltid, &lrd_info);
        if (tmp_rv == SHR_E_NONE) {
            SHR_NULL_CHECK(lrd_info, SHR_E_FAIL);
            if ((lrd_info->pt_type == LT_PT_TYPE_HASH) ||
                (lrd_info->pt_type == LT_PT_TYPE_FP_EM) ||
                (lrd_info->pt_type == LT_PT_TYPE_HASH_MPLS_DECAP)) {
                SHR_IF_ERR_EXIT(
                    bcmptm_rm_hash_lt_reg(unit, ltid, phase,
                                          &lt_state_offset));
            }
        } else if (tmp_rv != SHR_E_UNAVAIL) {
            /* Error case */
            SHR_IF_ERR_EXIT(tmp_rv);
        } /* else {
             tmp_rv = SHR_E_UNAVAIL which means there is no rsrc_mgr_data_struct
             => it is not managed by PTRM
        } */
    } /* foreach ltid */

    /* Phase 2
     * Can store (ltid, corresponding lt_state_offset)
     */
    phase = BCMPTM_SUB_PHASE_2;
    SHR_IF_ERR_EXIT(
        bcmptm_rm_hash_comp_config(unit, warm, phase));
    for (ltid = 0; ltid < BCMLTD_TABLE_COUNT; ltid++) {
        lrd_info = NULL;
        tmp_rv = SHR_E_NONE;
        tmp_rv = bcmlrd_lt_mreq_info_get(unit, ltid, &lrd_info);
        if (tmp_rv == SHR_E_NONE) {
            SHR_NULL_CHECK(lrd_info, SHR_E_FAIL);
            if ((lrd_info->pt_type == LT_PT_TYPE_HASH) ||
                (lrd_info->pt_type == LT_PT_TYPE_FP_EM) ||
                (lrd_info->pt_type == LT_PT_TYPE_HASH_MPLS_DECAP)) {
                SHR_IF_ERR_EXIT(
                    bcmptm_rm_hash_lt_reg(unit, ltid, phase, &lt_state_offset));
            }
        } else if (tmp_rv != SHR_E_UNAVAIL) {
            /* Error case */
            SHR_IF_ERR_EXIT(tmp_rv);
        } /* else {
             tmp_rv = SHR_E_UNAVAIL which is ok
             Not having rsrc_mgr_data_struct => it is not managed by PTRM
        } */
    } /* foreach ltid */

    /* Phase 3
     * rsrc_mgr can do any remainder tasks
     */
    phase = BCMPTM_SUB_PHASE_3;
    SHR_IF_ERR_EXIT(
        bcmptm_rm_hash_comp_config(unit, warm, phase));
    for (ltid = 0; ltid < BCMLTD_TABLE_COUNT; ltid++) {
        lrd_info = NULL;
        tmp_rv = SHR_E_NONE;
        tmp_rv = bcmlrd_lt_mreq_info_get(unit, ltid, &lrd_info);
        if (tmp_rv == SHR_E_NONE) {
            SHR_NULL_CHECK(lrd_info, SHR_E_FAIL);
            if ((lrd_info->pt_type == LT_PT_TYPE_HASH) ||
                (lrd_info->pt_type == LT_PT_TYPE_FP_EM) ||
                (lrd_info->pt_type == LT_PT_TYPE_HASH_MPLS_DECAP)) {
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_lt_reg(unit, ltid, phase, &lt_state_offset));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}
