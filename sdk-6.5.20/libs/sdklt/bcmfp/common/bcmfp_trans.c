/*! \file bcmfp_trans.c
 *
 * BCMFP component transaction management APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_ha.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_ha_internal.h>
#include <bcmfp/bcmfp_trans_internal.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmfp/generated/bcmfp_ha.h>
#include <bcmptm/bcmptm_uft.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_TRANS

/*
 * Transaction state of all HA blocks created during FP
 * component initialization time.
 * This variable holds only either IDLE or UC_A states.
 * If any of the HA blokc is in UC_A the this will be set
 * to UC_A.
 * If all of the HA blocks have transaction state IDLE,
 * the this will be set to IDLE.
 */
static bcmfp_trans_state_t trans_state_fixed[BCMDRD_CONFIG_MAX_UNITS]
                                     = { BCMFP_TRANS_STATE_UC_A };
/*
 * Transaction state of all HA blocks created during FP
 * component run time.
 * This variable holds only either IDLE or UC_A states.
 * If any of the HA blokc is in UC_A the this will be set
 * to UC_A.
 * If all of the HA blocks have transaction state IDLE,
 * the this will be set to IDLE.
 */
static bcmfp_trans_state_t trans_state_dynamic[BCMDRD_CONFIG_MAX_UNITS]
                                     = { BCMFP_TRANS_STATE_UC_A };

/*!
 * Some case single FP LT operation results in multiple
 * PTM calls. In such cases atomic transaction has to be
 * enabled internally by FP module. This is because atomic
 * transaction might be disabled by default.
 * This variable is set to TRUE in bcmfp_filter_create
 * bcmfp_filter_delete APIs and set o FALSE in APIs
 * bcmfp_trans_commit and bcmfp_trans_abort.
 */
static bool bcmfp_trans_atomic_state[BCMDRD_CONFIG_MAX_UNITS];

/* Latest transaction id used in FP LT operations. */
static uint32_t bcmfp_trans_id[BCMDRD_CONFIG_MAX_UNITS];

static int
bcmfp_trans_commit(int unit, bcmfp_stage_id_t stage_id)
{
    int rv;
    bool idle_flag = TRUE;
    void *ha_mem = NULL;
    void *backup_ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint8_t backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;
    bcmfp_ha_blk_hdr_t *backup_blk_hdr = NULL;

    SHR_FUNC_ENTER(unit);

    /* Copy the active HA block to back up HA block
     * corresponding to the given stage.
     */
    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_blk_hdr_get(unit, blk_id, &blk_hdr));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_backup_blk_id_get(unit, blk_id, &backup_blk_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_blk_hdr_get(unit, backup_blk_id, &backup_blk_hdr));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_get(unit, backup_blk_id, &backup_ha_mem));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    blk_hdr->trans_state = BCMFP_TRANS_STATE_COPY_A_TO_B;
    /* No need to set the trans_state of active block to IDLE as
     * trans_state of backup block is always IDLE.
     */
    sal_memcpy(backup_ha_mem, ha_mem, blk_hdr->blk_size);
    backup_blk_hdr->signature = BCMFP_HA_STRUCT_SIGN + backup_blk_id;
    backup_blk_hdr->backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
    /* If all the fixed HA blocks are in IDLE state then set the global
     * transaction state to IDLE.
     */
    for (blk_id = BCMFP_HA_BLK_ID_FIXED_MIN;
        blk_id < BCMFP_HA_BLK_ID_FIXED_MAX;
        blk_id++) {
        rv = bcmfp_ha_blk_hdr_get(unit, blk_id, &blk_hdr);
        if (rv == SHR_E_NOT_FOUND) {
            continue;
        }
        SHR_IF_ERR_EXIT(rv);
        if (blk_hdr->trans_state != BCMFP_TRANS_STATE_IDLE) {
            idle_flag = FALSE;
            break;
        }
    }
    if (idle_flag == TRUE) {
        trans_state_fixed[unit] = BCMFP_TRANS_STATE_IDLE;
    }

    /* If global transaction state of dynamic HA blocks is IDLE,
     * then nothing pending.
     */
    if (trans_state_dynamic[unit] == BCMFP_TRANS_STATE_IDLE) {
        SHR_EXIT();
    }

    /* Commit all the non IDLE dynamic HA blocks. If more than one fixed
     * HA block is modified in the transaction,then all the dynamic blocks
     * are committed when committing the first fixed HA block.
     */
    for (blk_id = BCMFP_HA_BLK_ID_DYNAMIC_MIN;
        blk_id <= BCMFP_HA_BLK_ID_DYNAMIC_MAX;
        blk_id++) {
        rv = bcmfp_ha_blk_hdr_get(unit, blk_id, &blk_hdr);
        if (rv == SHR_E_NOT_FOUND) {
            continue;
        }
        SHR_IF_ERR_EXIT(rv);
        if (blk_hdr->trans_state == BCMFP_TRANS_STATE_IDLE) {
            continue;
        }
        backup_blk_id = blk_hdr->backup_blk_id;
        if (backup_blk_id == BCMFP_HA_BLK_ID_INVALID) {
            continue;
        }
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_ha_blk_hdr_get(unit, backup_blk_id,
                                    &backup_blk_hdr));
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_ha_mem_get(unit, backup_blk_id,
                                 &backup_ha_mem));
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
        blk_hdr->trans_state = BCMFP_TRANS_STATE_COPY_A_TO_B;
        sal_memcpy(backup_ha_mem, ha_mem, blk_hdr->blk_size);
        backup_blk_hdr->signature =  BCMFP_HA_STRUCT_SIGN + backup_blk_id;
        backup_blk_hdr->trans_state = BCMFP_TRANS_STATE_IDLE;
        blk_hdr->trans_state = BCMFP_TRANS_STATE_IDLE;
        backup_blk_hdr->backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
    }
    trans_state_dynamic[unit] = BCMFP_TRANS_STATE_IDLE;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_trans_abort(int unit, bcmfp_stage_id_t stage_id)
{
    int rv;
    bool idle_flag = TRUE;
    void *ha_mem = NULL;
    void *backup_ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint8_t backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;
    bcmfp_ha_blk_hdr_t *backup_blk_hdr = NULL;

    SHR_FUNC_ENTER(unit);

    /* Copy the back up HA block to active HA block
     * corresponding to the given stage.
     */
    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_blk_hdr_get(unit, blk_id, &blk_hdr));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_backup_blk_id_get(unit, blk_id, &backup_blk_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_blk_hdr_get(unit, backup_blk_id, &backup_blk_hdr));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_get(unit, backup_blk_id, &backup_ha_mem));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    blk_hdr->trans_state = BCMFP_TRANS_STATE_COPY_B_TO_A;
    /* No need to set the trans_state of active block to IDLE as
     * trans_state of backup block is always IDLE.
     */
    sal_memcpy(ha_mem, backup_ha_mem, blk_hdr->blk_size);
    blk_hdr->signature = BCMFP_HA_STRUCT_SIGN + blk_id;
    blk_hdr->backup_blk_id = backup_blk_id;
    /* If all the fixed HA blocks are in IDLE state then set the global
     * transaction state to IDLE.
     */
    for (blk_id = BCMFP_HA_BLK_ID_FIXED_MIN;
        blk_id < BCMFP_HA_BLK_ID_FIXED_MAX;
        blk_id++) {
        rv = bcmfp_ha_blk_hdr_get(unit, blk_id, &blk_hdr);
        if (rv == SHR_E_NOT_FOUND) {
            continue;
        }
        SHR_IF_ERR_EXIT(rv);
        if (blk_hdr->trans_state != BCMFP_TRANS_STATE_IDLE) {
            idle_flag = FALSE;
            break;
        }
    }
    if (idle_flag == TRUE) {
        trans_state_fixed[unit] = BCMFP_TRANS_STATE_IDLE;
    }

    /* If global transaction state of dynamic HA blocks is IDLE,
     * then nothing pending.
     */
    if (trans_state_dynamic[unit] == BCMFP_TRANS_STATE_IDLE) {
        SHR_EXIT();
    }

    /* Abort all the dynamic HA blocks. If more than one fixed HA block
     * is modified in the transaction,then all the dynamic blocks are
     * aborted when aborting the first fixed HA block.
     */
    for (blk_id = BCMFP_HA_BLK_ID_DYNAMIC_MIN;
        blk_id <= BCMFP_HA_BLK_ID_DYNAMIC_MAX;
        blk_id++) {
        rv = bcmfp_ha_blk_hdr_get(unit, blk_id, &blk_hdr);
        if (rv == SHR_E_NOT_FOUND) {
            continue;
        }
        SHR_IF_ERR_EXIT(rv);
        if (blk_hdr->trans_state == BCMFP_TRANS_STATE_IDLE) {
            continue;
        }

        backup_blk_id = blk_hdr->backup_blk_id;
        if (backup_blk_id == BCMFP_HA_BLK_ID_INVALID) {
            continue;
        }
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_ha_blk_hdr_get(unit, backup_blk_id,
                                    &backup_blk_hdr));
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_ha_mem_get(unit, backup_blk_id,
                                 &backup_ha_mem));
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
        blk_hdr->trans_state = BCMFP_TRANS_STATE_COPY_B_TO_A;
        sal_memcpy(ha_mem, backup_ha_mem, blk_hdr->blk_size);
        blk_hdr->signature = BCMFP_HA_STRUCT_SIGN + blk_id;
        backup_blk_hdr->trans_state = BCMFP_TRANS_STATE_IDLE;
        blk_hdr->trans_state = BCMFP_TRANS_STATE_IDLE;
        blk_hdr->backup_blk_id = backup_blk_id;
    }
    trans_state_dynamic[unit] = BCMFP_TRANS_STATE_IDLE;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_trans_idle_check(int unit, bcmfp_stage_id_t stage_id)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;

    SHR_FUNC_ENTER(unit);

    if (trans_state_fixed[unit] == BCMFP_TRANS_STATE_IDLE &&
        trans_state_dynamic[unit] == BCMFP_TRANS_STATE_IDLE) {
        SHR_EXIT();
    }

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_blk_hdr_get(unit, blk_id, &blk_hdr));
    if (blk_hdr->trans_state == BCMFP_TRANS_STATE_UC_A ||
        trans_state_dynamic[unit] == BCMFP_TRANS_STATE_UC_A) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_trans_atomic_state_set(int unit,
                             uint32_t trans_id,
                             bool atomic_trans_state)
{
    SHR_FUNC_ENTER(unit);

    if (atomic_trans_state == TRUE) {
        if (bcmfp_trans_atomic_state[unit] != TRUE ||
            bcmfp_trans_id[unit] != trans_id) {
            bcmfp_trans_atomic_state[unit] = TRUE;
            bcmfp_trans_id[unit] = trans_id;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_mreq_atomic_trans_enable(unit));
        }
    } else {
        bcmfp_trans_atomic_state[unit] = FALSE;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_trans_cmd(int unit,
                bcmfp_stage_id_t stage_id,
                bcmfp_trans_cmd_t trans_cmd)
{
    SHR_FUNC_ENTER(unit);

    switch (trans_cmd) {
        case BCMFP_TRANS_CMD_IDLE_CHECK:
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmfp_trans_idle_check(unit, stage_id));
            break;
        case BCMFP_TRANS_CMD_COMMIT:
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmfp_trans_commit(unit, stage_id));
            break;
        case BCMFP_TRANS_CMD_ABORT:
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmfp_trans_abort(unit, stage_id));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_trans_state_set(int unit, uint8_t blk_id,
                      bcmfp_trans_state_t trans_state)
{
    void *ha_mem = NULL;
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    blk_hdr = (bcmfp_ha_blk_hdr_t *)ha_mem;
    blk_hdr->trans_state = trans_state;

    if (BCMFP_HA_BLK_ID_IS_FIXED(blk_id) &&
        (trans_state == BCMFP_TRANS_STATE_UC_A)) {
        trans_state_fixed[unit] = trans_state;
    }
    if (BCMFP_HA_BLK_ID_IS_DYNAMIC(blk_id) &&
        (trans_state == BCMFP_TRANS_STATE_UC_A)) {
        trans_state_dynamic[unit] = trans_state;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ing_trans_commit(int unit,
                       bcmltd_sid_t sid,
                       uint32_t trans_id,
                       void *context)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Disable the dynamic atomic transaction state.*/
    (void)bcmfp_trans_atomic_state_set(unit, trans_id, FALSE);

    rv = bcmfp_trans_cmd(unit, BCMFP_STAGE_ID_INGRESS,
                         BCMFP_TRANS_CMD_IDLE_CHECK);
    if (rv == SHR_E_FAIL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_trans_cmd(unit,
                             BCMFP_STAGE_ID_INGRESS,
                             BCMFP_TRANS_CMD_COMMIT));

        bcmevm_publish_event_notify(unit,
                                    "FP_ING_COMMIT",
                                    trans_id);
    } else {
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

void
bcmfp_ing_trans_abort(int unit,
                      bcmltd_sid_t sid,
                      uint32_t trans_id,
                      void *context)
{
    shr_error_t rv = SHR_E_NONE;

    /* Disable the dynamic atomic transaction state.*/
    (void)bcmfp_trans_atomic_state_set(unit, trans_id, FALSE);

    rv = bcmfp_trans_cmd(unit, BCMFP_STAGE_ID_INGRESS,
                         BCMFP_TRANS_CMD_IDLE_CHECK);
    if (rv == SHR_E_FAIL) {
        (void)bcmfp_trans_cmd(unit,
                              BCMFP_STAGE_ID_INGRESS,
                              BCMFP_TRANS_CMD_ABORT);
    }

    bcmevm_publish_event_notify(unit,
                                "FP_ING_ABORT",
                                trans_id);
    return;
}

int
bcmfp_egr_trans_commit(int unit,
                       bcmltd_sid_t sid,
                       uint32_t trans_id,
                       void *context)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Disable the dynamic atomic transaction state.*/
    (void)bcmfp_trans_atomic_state_set(unit, trans_id, FALSE);

    rv = bcmfp_trans_cmd(unit, BCMFP_STAGE_ID_EGRESS,
                         BCMFP_TRANS_CMD_IDLE_CHECK);
    if (rv == SHR_E_FAIL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_trans_cmd(unit,
                             BCMFP_STAGE_ID_EGRESS,
                             BCMFP_TRANS_CMD_COMMIT));
        bcmevm_publish_event_notify(unit,
                                    "FP_EGR_COMMIT",
                                    trans_id);
    } else {
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

void
bcmfp_egr_trans_abort(int unit,
                      bcmltd_sid_t sid,
                      uint32_t trans_id,
                      void *context)
{
    shr_error_t rv = SHR_E_NONE;

    /* Disable the dynamic atomic transaction state.*/
    (void)bcmfp_trans_atomic_state_set(unit, trans_id, FALSE);

    rv = bcmfp_trans_cmd(unit, BCMFP_STAGE_ID_EGRESS,
                         BCMFP_TRANS_CMD_IDLE_CHECK);
    if (rv == SHR_E_FAIL) {
        (void)bcmfp_trans_cmd(unit,
                              BCMFP_STAGE_ID_EGRESS,
                              BCMFP_TRANS_CMD_ABORT);
    }

    bcmevm_publish_event_notify(unit,
                                "FP_EGR_ABORT",
                                trans_id);
    return;
}

int
bcmfp_vlan_trans_commit(int unit,
                        bcmltd_sid_t sid,
                        uint32_t trans_id,
                        void *context)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Disable the dynamic atomic transaction state.*/
    (void)bcmfp_trans_atomic_state_set(unit, trans_id, FALSE);

    rv = bcmfp_trans_cmd(unit, BCMFP_STAGE_ID_LOOKUP,
                         BCMFP_TRANS_CMD_IDLE_CHECK);
    if (rv == SHR_E_FAIL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_trans_cmd(unit,
                             BCMFP_STAGE_ID_LOOKUP,
                             BCMFP_TRANS_CMD_COMMIT));
    } else {
        SHR_IF_ERR_EXIT(rv);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_trans_cmd(unit,
                        BCMFP_STAGE_ID_LOOKUP,
                        BCMFP_TRANS_CMD_COMMIT));
    /* commit the sbr entries allocated by the UFT manager to HA space */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_sbr_tile_mode_commit(unit));

exit:
    SHR_FUNC_EXIT();
}

void
bcmfp_vlan_trans_abort(int unit,
                       bcmltd_sid_t sid,
                       uint32_t trans_id,
                       void *context)
{
    shr_error_t rv = SHR_E_NONE;

    /* Disable the dynamic atomic transaction state.*/
    (void)bcmfp_trans_atomic_state_set(unit, trans_id, FALSE);

    rv = bcmfp_trans_cmd(unit, BCMFP_STAGE_ID_LOOKUP,
                         BCMFP_TRANS_CMD_IDLE_CHECK);
    if (rv == SHR_E_FAIL) {
        (void)bcmfp_trans_cmd(unit,
                              BCMFP_STAGE_ID_LOOKUP,
                              BCMFP_TRANS_CMD_ABORT);
    }
    /* abort the sbr entries allocated by the UFT manager to HA space */
    rv = bcmptm_uft_sbr_tile_mode_abort(unit);
    return;
}

int
bcmfp_em_trans_commit(int unit,
                      bcmltd_sid_t sid,
                      uint32_t trans_id,
                      void *context)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Disable the dynamic atomic transaction state.*/
    (void)bcmfp_trans_atomic_state_set(unit, trans_id, FALSE);

    rv = bcmfp_trans_cmd(unit, BCMFP_STAGE_ID_EXACT_MATCH,
                         BCMFP_TRANS_CMD_IDLE_CHECK);
    if (rv == SHR_E_FAIL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_trans_cmd(unit,
                             BCMFP_STAGE_ID_EXACT_MATCH,
                             BCMFP_TRANS_CMD_COMMIT));
    } else {
        SHR_IF_ERR_EXIT(rv);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_trans_cmd(unit,
                        BCMFP_STAGE_ID_EXACT_MATCH,
                        BCMFP_TRANS_CMD_COMMIT));
    /* commit the sbr entries allocated by the UFT manager to HA space */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_sbr_tile_mode_commit(unit));
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT();
}

void
bcmfp_em_trans_abort(int unit,
                     bcmltd_sid_t sid,
                     uint32_t trans_id,
                     void *context)
{
    shr_error_t rv = SHR_E_NONE;

    /* Disable the dynamic atomic transaction state.*/
    (void)bcmfp_trans_atomic_state_set(unit, trans_id, FALSE);

    rv = bcmfp_trans_cmd(unit, BCMFP_STAGE_ID_EXACT_MATCH,
                         BCMFP_TRANS_CMD_IDLE_CHECK);
    if (rv == SHR_E_FAIL) {
        (void)bcmfp_trans_cmd(unit,
                              BCMFP_STAGE_ID_EXACT_MATCH,
                              BCMFP_TRANS_CMD_ABORT);
    }
    /* abort the sbr entries allocated by the UFT manager to HA space */
    rv = bcmptm_uft_sbr_tile_mode_abort(unit);
    return;
}

int
bcmfp_em_ft_trans_commit(int unit,
                         bcmltd_sid_t sid,
                         uint32_t trans_id,
                         void *context)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Disable the dynamic atomic transaction state.*/
    (void)bcmfp_trans_atomic_state_set(unit, trans_id, FALSE);

    rv = bcmfp_trans_cmd(unit, BCMFP_STAGE_ID_FLOW_TRACKER,
                         BCMFP_TRANS_CMD_IDLE_CHECK);
    if (rv == SHR_E_FAIL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_trans_cmd(unit,
                             BCMFP_STAGE_ID_FLOW_TRACKER,
                             BCMFP_TRANS_CMD_COMMIT));
    } else {
        SHR_IF_ERR_EXIT(rv);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_trans_cmd(unit,
                        BCMFP_STAGE_ID_FLOW_TRACKER,
                        BCMFP_TRANS_CMD_COMMIT));
    /* commit the sbr entries allocated by the UFT manager to HA space */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_sbr_tile_mode_commit(unit));
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT();
}

void
bcmfp_em_ft_trans_abort(int unit,
                        bcmltd_sid_t sid,
                        uint32_t trans_id,
                        void *context)
{
    shr_error_t rv = SHR_E_NONE;

    /* Disable the dynamic atomic transaction state.*/
    (void)bcmfp_trans_atomic_state_set(unit, trans_id, FALSE);

    rv = bcmfp_trans_cmd(unit, BCMFP_STAGE_ID_FLOW_TRACKER,
                         BCMFP_TRANS_CMD_IDLE_CHECK);
    if (rv == SHR_E_FAIL) {
        (void)bcmfp_trans_cmd(unit,
                              BCMFP_STAGE_ID_FLOW_TRACKER,
                              BCMFP_TRANS_CMD_ABORT);
    }
    /* abort the sbr entries allocated by the UFT manager to HA space */
    rv = bcmptm_uft_sbr_tile_mode_abort(unit);
    return;
}
