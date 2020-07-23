/*! \file rm_tcam_trans.c
 *
 * Low Level Functions for TCAM resource manager.
 * This file contains low level functions used for handling
 * transactions in resource manager
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
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include "rm_tcam.h"
#include "rm_tcam_prio_only.h"
#include "rm_tcam_prio_eid.h"
#include "rm_tcam_fp_entry_mgmt.h"
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_limits.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmptm/bcmptm_rm_tcam_fp.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <shr/shr_ha.h>
#include <bcmptm/bcmptm_rm_tcam_fp_internal.h>


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM


/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_tcam_um_trans_complete(int unit,
               bcmptm_rm_tcam_trans_info_t *trans_info_global)
{
    bcmltd_sid_t ltid;
    bcmptm_rm_tcam_lt_info_t ltid_info;
    uint8_t *active_state = NULL;
    uint8_t *backup_state = NULL;
    uint8_t pipe = 0;
    bcmptm_rm_tcam_trans_info_t *trans_info = NULL;
    bcmptm_rm_tcam_md_t *tcam_md = NULL;
    uint32_t state_size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(trans_info_global, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_md_get_first(unit, &tcam_md));

    while (tcam_md != NULL) {
        ltid = tcam_md->ltid;
        sal_memset(&ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_ptrm_info_get(unit,
                                          ltid,
                                          0,
                                          0,
                                          &ltid_info,
                                          NULL,
                                          NULL));
        if ((ltid_info.pt_type == LT_PT_TYPE_FP) ||
            (ltid_info.pt_type == LT_PT_TYPE_TCAM_PRI_AND_ENTRY_ID) ||
            (ltid_info.pt_type == LT_PT_TYPE_TCAM_PRI_ONLY)) {
            for (pipe = 0; pipe < tcam_md->num_pipes; pipe++) {
                active_state = tcam_md->active_ptrs[pipe];
                trans_info = (bcmptm_rm_tcam_trans_info_t *)active_state;
                active_state += sizeof(bcmptm_rm_tcam_trans_info_t);
                if (trans_info->trans_state == BCMPTM_RM_TCAM_STATE_IDLE) {
                    continue;
                }
                if (trans_info->trans_state == BCMPTM_RM_TCAM_STATE_UC_A) {
                    backup_state = tcam_md->backup_ptrs[pipe];
                    backup_state += sizeof(bcmptm_rm_tcam_trans_info_t);
                    state_size = (trans_info->state_size -
                                 sizeof(bcmptm_rm_tcam_trans_info_t));
                    if (trans_info_global->trans_state ==
                        BCMPTM_RM_TCAM_STATE_COPY_A_TO_B) {
                        sal_memcpy(backup_state, active_state, state_size);
                    } else {
                        sal_memcpy(active_state, backup_state, state_size);
                    }
                    /* change the transaction state to Idle */
                    trans_info->trans_state = BCMPTM_RM_TCAM_STATE_IDLE;
                    /* change the transaction state to Idle in back up too. */
                    backup_state = tcam_md->backup_ptrs[pipe];
                    trans_info =
                        (bcmptm_rm_tcam_trans_info_t *)backup_state;
                    trans_info->trans_state = BCMPTM_RM_TCAM_STATE_IDLE;
                }
            }
        } else {
            LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit, "LTID:%d has wrong "
                       "LT_PT_TYPE_XXX:%d\n"), ltid, ltid_info.pt_type));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        tcam_md = tcam_md->next;
    }
    /* update the transaction state to Idle in active DB */
    trans_info_global->trans_state = BCMPTM_RM_TCAM_STATE_IDLE;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_trans_cmd(int unit, uint64_t flags, uint32_t cseq_id,
                         bcmptm_trans_cmd_t trans_cmd) 
{
    int rv = SHR_E_NONE;
    bcmptm_rm_tcam_trans_info_t *trans_info_global = NULL;

    SHR_FUNC_ENTER(unit);

    /* Rsrc_mgr starts in IDLE state.
     * In IDLE state, ptrm_state_cseq_id is dont_care.
     * - if it gets commit()
     *          it stays in IDLE state. Ignores cseq_id.
     *
     * - if it gets abort()
     *          stays in IDLE state. Ignores cseq_id.
     *
     * - if it gets bcmptm_rm_hash_req()
     *          goes into CHANGED state and
     *          remembers cseq_id as ptrm_state_cseq_id
     *
     * - if it gets idle_check:
     *          stays in IDLE state and returns SHR_E_NONE.
     *          cseq_id should be ignored
     *
     * In CHANGED state:
     * - if it gets bcmptm_rm_hash_req()
     *          Must declare SHR_E_FAIL if cseq_id does not match ptrm_state_cseq_id
     *          Remain in CHANGED state.
     *
     * - if it gets commit()
     *          if cseq_id != ptrm_state_cseq_id, ignore commit.
     *          Else, it commits the changes to sw_state and goes to IDLE
     *
     * - if it gets abort()
     *          if cseq_id != ptrm_state_cseq_id, declare SHR_E_FAIL
     *          Else, it aborts the changes to sw_state and goes to IDLE
     *
     * - if it gets idle_check()
     *          it stays in CHANGED state and returns SHR_E_FAIL
     *          (cseq_id should be ignored)
     *
     * PTRM_state_cseq_id must be in HA space - just like rest of PTRM SW_state
 */

    switch (trans_cmd) {
        case BCMPTM_TRANS_CMD_COMMIT:
            LOG_DEBUG(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Tcam rsrc_mgr got Commit \n")));
            break;
        case BCMPTM_TRANS_CMD_ABORT:
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Tcam rsrc_mgr got Abort \n")));
            break;
        case BCMPTM_TRANS_CMD_IDLE_CHECK:
            LOG_DEBUG(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Tcam rsrc_mgr got Idle_Check \n")));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_trans_info_global_get(unit,
                                           &trans_info_global));
            if (!trans_info_global) {
                rv = SHR_E_NONE; /* RM is not initialized */
            } else if (trans_info_global->trans_state == BCMPTM_RM_TCAM_STATE_IDLE) {
                rv = SHR_E_NONE;
            } else {
                rv = SHR_E_FAIL;
            }
            break;
        case BCMPTM_TRANS_CMD_ATOMIC_TRANS_ENABLE:
            /*
             * When bcmptm_mreq_atomic_trans_enable is invoked from any CTH,
             * PTM issues this command to all resource managers.
             */
            bcmptm_rm_tcam_dynamic_trans_set(unit, TRUE);
            break;
        case BCMPTM_TRANS_CMD_ATOMIC_TRANS_DISABLE:
            /*
             * When bcmptm_mreq_atomic_trans_enable is invoked from any CTH,
             * PTM issues this command to all resource managers at the end
             * of that transaction.
             */
            bcmptm_rm_tcam_dynamic_trans_set(unit, FALSE);
            break;
        default:
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Tcam rsrc_mgr got Unknown trans_cmd \n")));
            /* rv = SHR_E_INTERNAL; */
            break;
    }
    SHR_IF_ERR_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_um_trans_cmd(int unit, uint64_t flags, uint32_t cseq_id,
                            bcmptm_trans_cmd_t trans_cmd) 
{
    int rv = SHR_E_NONE;
    bool undo_uc_state = FALSE;
    bcmptm_rm_tcam_trans_info_t *trans_info_global = NULL;

    SHR_FUNC_ENTER(unit);

    /* Rsrc_mgr starts in IDLE state.
     * In IDLE state, ptrm_state_cseq_id is dont_care.
     * - if it gets commit()
     *          it stays in IDLE state. Ignores cseq_id.
     *
     * - if it gets abort()
     *          stays in IDLE state. Ignores cseq_id.
     *
     * - if it gets bcmptm_rm_hash_req()
     *          goes into CHANGED state and
     *          remembers cseq_id as ptrm_state_cseq_id
     *
     * - if it gets idle_check:
     *          stays in IDLE state and returns SHR_E_NONE.
     *          cseq_id should be ignored
     *
     * In CHANGED state:
     * - if it gets bcmptm_rm_hash_req()
     *          Must declare SHR_E_FAIL if cseq_id does not match ptrm_state_cseq_id
     *          Remain in CHANGED state.
     *
     * - if it gets commit()
     *          if cseq_id != ptrm_state_cseq_id, ignore commit.
     *          Else, it commits the changes to sw_state and goes to IDLE
     *
     * - if it gets abort()
     *          if cseq_id != ptrm_state_cseq_id, declare SHR_E_FAIL
     *          Else, it aborts the changes to sw_state and goes to IDLE
     *
     * - if it gets idle_check()
     *          it stays in CHANGED state and returns SHR_E_FAIL
     *          (cseq_id should be ignored)
     *
     * PTRM_state_cseq_id must be in HA space - just like rest of PTRM SW_state
 */

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_trans_info_global_get(unit,
                                   &trans_info_global));

    if (!trans_info_global) {
        SHR_EXIT(); /* RM is not initialized */
    }

    switch (trans_cmd) {
        case BCMPTM_TRANS_CMD_COMMIT:
            LOG_DEBUG(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Tcam_um rsrc_mgr got Commit \n")));
            if (trans_info_global->trans_state == BCMPTM_RM_TCAM_STATE_IDLE) {
                rv = SHR_E_NONE;
            }
            else if
                (trans_info_global->trans_state == BCMPTM_RM_TCAM_STATE_UC_A ||
                 trans_info_global->trans_state ==
                                    BCMPTM_RM_TCAM_STATE_COPY_A_TO_B) {
                 /* Change the global transaction state to copy A to B */
                 trans_info_global->trans_state =
                                    BCMPTM_RM_TCAM_STATE_COPY_A_TO_B;
                 /* Call function to copy Active to Backup */
                 rv = bcmptm_rm_tcam_um_trans_complete(unit, trans_info_global);
            } else {
                /* Control should not come here */
                rv = SHR_E_INTERNAL;
            }
            break;
        case BCMPTM_TRANS_CMD_ABORT:
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Tcam_um rsrc_mgr got Abort \n")));
            if (trans_info_global->trans_state == BCMPTM_RM_TCAM_STATE_IDLE) {
                rv = SHR_E_NONE;
            } else if ((trans_info_global->trans_state ==
                                          BCMPTM_RM_TCAM_STATE_COPY_B_TO_A) ||
                       (trans_info_global->trans_state ==
                                          BCMPTM_RM_TCAM_STATE_COPY_A_TO_B)) {
                 /* Call function to copy Backup to Active or vice versa. */
                 rv = bcmptm_rm_tcam_um_trans_complete(unit, trans_info_global);
            } else if (trans_info_global->trans_state ==
                                          BCMPTM_RM_TCAM_STATE_UC_A) {
                 SHR_IF_ERR_VERBOSE_EXIT
                     (bcmptm_rm_tcam_undo_uc_state_get(unit, &undo_uc_state));
                 if (undo_uc_state == FALSE) {
                     /* Control will be here If
                      * 1. Atomic transaction is disabled either dynamically or
                      *    through initilization time configuration.
                      * 2. Single LT opeartion initiates multiple PTRM-TCAM
                      *    opeartions.
                      * 3. Any one of the PTRM-TCAM operation failed except
                      *    the first one.
                      *
                      * Hardware might be updated for successfull PTRM-TCAM
                      * operations and cannot be reverted so software state
                      * should also be not reverted if it is changed.
                      *
                      * Ideally control should never reach this condition since
                      * CTH must enable atomic transaction dynamically if single
                      * LT operation requires multiple PTM operations.
                      */
                     LOG_ERROR(BSL_LOG_MODULE,
                         (BSL_META_U(unit, "CTH using RM-TCAM didnot call "
                          "bcmptm_mreq_atomic_trans_enable when there are "
                          " multiple PTM calls for a single LT operation\n")));
                     trans_info_global->trans_state =
                                        BCMPTM_RM_TCAM_STATE_COPY_A_TO_B;
                 } else {
                     /* Change the global transaction state to copy B to A */
                     trans_info_global->trans_state =
                                        BCMPTM_RM_TCAM_STATE_COPY_B_TO_A;
                 }
                 /* Call function to copy Backup to Active */
                 rv = bcmptm_rm_tcam_um_trans_complete(unit, trans_info_global);
            } else {
                 /* Control should not come here */
                 rv = SHR_E_INTERNAL;
            }
            break;
        case BCMPTM_TRANS_CMD_IDLE_CHECK:
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Tcam_um rsrc_mgr got Idle_Check \n")));
            if (trans_info_global->trans_state == BCMPTM_RM_TCAM_STATE_IDLE) {
                rv = SHR_E_NONE;
            } else {
                rv = SHR_E_FAIL;
            }
            break;
        case BCMPTM_TRANS_CMD_ATOMIC_TRANS_ENABLE:
            bcmptm_rm_tcam_dynamic_trans_set(unit, TRUE);
            break;
        case BCMPTM_TRANS_CMD_ATOMIC_TRANS_DISABLE:
            bcmptm_rm_tcam_dynamic_trans_set(unit, FALSE);
            break;
        default:
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Tcam_um rsrc_mgr got Unknown trans_cmd \n")));
            /* rv = SHR_E_INTERNAL; */
            break;
    }
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}
