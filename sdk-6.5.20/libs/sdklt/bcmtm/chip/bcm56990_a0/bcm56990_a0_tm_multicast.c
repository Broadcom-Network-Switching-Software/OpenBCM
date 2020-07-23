/*! \file bcm56990_a0_tm_multicast.c
 *
 * TM BST multicast chip specific functionality implementation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <shr/shr_debug.h>
#include <sal/sal_sleep.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/chip/bcm56990_a0_enum.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmtm/chip/bcm56990_a0_tm.h>
#include <bcmtm/multicast/bcmtm_mc_group.h>
#include <bcmlrd/bcmlrd_table.h>
#include "bcm56990_a0_tm_multicast_internal.h"
#include "bcm56990_a0_tm_mmu_flexport.h"

/*******************************************************************************
 * Local definitions
 */
#define BCMTM_MC_MAX_ENTRY_WSIZE 18

#define BSL_LOG_MODULE  BSL_LS_BCMTM_CHIP

/*******************************************************************************
 * Private functions
 */
static int
bcm56990_a0_mc_icc_set (int unit, int mc_head_id, int repl_count)
{
    bcmdrd_sid_t sid;
    uint32_t ltmbuf[BCMTM_MC_MAX_ENTRY_WSIZE] = {0};
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmltd_sid_t ltid = -1;

    SHR_FUNC_ENTER(unit);

    sid = MMU_REPL_HEAD_TBLm;

    BCMTM_PT_DYN_INFO(pt_dyn_info, mc_head_id, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, (void *)&pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, MEMBER_ICCf, ltmbuf, (uint32_t *)&repl_count));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, (void *)&pt_dyn_info, ltmbuf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_mc_icc_get (int unit, int mc_head_id, int *repl_count)
{
    bcmdrd_sid_t sid;
    uint32_t ltmbuf[BCMTM_MC_MAX_ENTRY_WSIZE] = {0}, fval;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmltd_sid_t ltid = -1;

    SHR_FUNC_ENTER(unit);

    sid = MMU_REPL_HEAD_TBLm;

    BCMTM_PT_DYN_INFO(pt_dyn_info, mc_head_id, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, (void *)&pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, MEMBER_ICCf, ltmbuf, &fval));
    *repl_count = fval;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable/Disable dynamic replication.
 * This function enable dynamic modification to the replication list.
 *
 * \param [in] unit Logical device id.
 * \param [in] mc_group_id Multicast group ID.
 * \param [in] enable Enable/disable dynamic updation of list.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56990_a0_mc_dynamic_repl_set(int unit,
                                uint32_t mc_group_id,
                                uint32_t enable)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf = 0;
    bcmlrd_sid_t ltid = -1;
    bcmtm_pt_info_t pt_dyn_info = {0};

    SHR_FUNC_ENTER(unit);

    sid = MMU_RQE_DYNAMIC_PROGRAMMING_CONTROLr;
    fid = MC_IDXf;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);

    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &mc_group_id));
    fid = ENABLEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &enable));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_mc_repl_group_entry_set (int unit, uint32_t mc_group_id,
                                     bcmtm_mc_repl_group_info_t *mc_group_info)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf[BCMTM_MC_MAX_ENTRY_WSIZE] = {0};
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmltd_sid_t ltid = -1;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mc_group_info, SHR_E_PARAM);
    sid = MMU_REPL_GROUP_INFO_TBLm;

    BCMTM_PT_DYN_INFO(pt_dyn_info, mc_group_id, 0);
    fid = MEMBER_BMPf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf,
                         (uint32_t *)&mc_group_info->port_agg_bmp));

    fid = BASE_PTRf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf,
                         (uint32_t *)&mc_group_info->base_ptr));

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, (void *)&pt_dyn_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_mc_repl_group_entry_get (int unit, uint32_t mc_group_id,
                                     bcmtm_mc_repl_group_info_t *mc_group_info)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf[BCMTM_MC_MAX_ENTRY_WSIZE] = {0};
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmltd_sid_t ltid = -1;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mc_group_info, SHR_E_PARAM);
    sid = MMU_REPL_GROUP_INFO_TBLm;

    BCMTM_PT_DYN_INFO(pt_dyn_info, mc_group_id, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, (void *)&pt_dyn_info, ltmbuf));

    fid = MEMBER_BMPf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, ltmbuf,
                         (uint32_t *)&mc_group_info->port_agg_bmp));

    fid = BASE_PTRf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, ltmbuf,
                         (uint32_t *)&mc_group_info->base_ptr));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_mc_repl_head_entry_set (int unit, uint32_t mc_head_id,
                                    uint32_t start_ptr)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf[BCMTM_MC_MAX_ENTRY_WSIZE] = {0};
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmltd_sid_t ltid = -1;

    SHR_FUNC_ENTER(unit);

    sid = MMU_REPL_HEAD_TBLm;

    BCMTM_PT_DYN_INFO(pt_dyn_info, mc_head_id, 0);
    fid = HEAD_PTRf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf, (uint32_t *) &start_ptr));

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, (void *)&pt_dyn_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_mc_repl_head_entry_get (int unit, uint32_t mc_head_id,
                                    uint32_t *start_ptr)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf[BCMTM_MC_MAX_ENTRY_WSIZE] = {0};
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmltd_sid_t ltid = -1;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(start_ptr, SHR_E_PARAM);
    sid = MMU_REPL_HEAD_TBLm;

    BCMTM_PT_DYN_INFO(pt_dyn_info, mc_head_id, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, (void *)&pt_dyn_info, ltmbuf));

    fid = HEAD_PTRf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, ltmbuf, start_ptr));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Copy multicast replication head table.
 *
 * \param [in] unit Logical device id.
 * \param [in] mc_group_info Multicast group.
 * \param [in] repl_head_info Replication head.
 * \param [in] start_ptr Replication head table start pointer from where
 * replication head table is to copy.
 * \param [in] base_ptr New replication head base pointer for the copied list.
 * \param [in] repl_count Number of next hop in the list for the updated list.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56990_a0_mc_repl_head_copy(int unit,
                              bcmtm_mc_repl_group_info_t *mc_group_info,
                              bcmtm_mc_repl_head_info_t *repl_head_info,
                              uint32_t start_ptr,
                              int *base_ptr,
                              int repl_count)
{
#define TH4_MC_RESERVED_HEAD_ENTRY 3824
    int num_entry, icc_count = 0;
    uint32_t head_id = 0;
    uint32_t dup_head_id = TH4_MC_RESERVED_HEAD_ENTRY;
    uint32_t list_ptr = 0;

    SHR_FUNC_ENTER(unit);

    *base_ptr = dup_head_id;

    head_id = (uint32_t)mc_group_info->base_ptr;

    for (num_entry = 0; num_entry < mc_group_info->num_entries; num_entry++) {
        list_ptr = 0;
        if (head_id == (uint32_t)repl_head_info->index) {
            list_ptr = start_ptr;
            /*
             * icc count for unknown replication list taken care in calling
             * function.
             */
            icc_count = repl_count;
        } else {
            SHR_IF_ERR_EXIT
                (bcm56990_a0_mc_repl_head_entry_get(unit, head_id, &list_ptr));
            SHR_IF_ERR_EXIT
                (bcm56990_a0_mc_icc_get(unit, head_id, &icc_count));
        }
        SHR_IF_ERR_EXIT
            (bcm56990_a0_mc_repl_head_entry_set(unit, dup_head_id, list_ptr));
        SHR_IF_ERR_EXIT
            (bcm56990_a0_mc_icc_set(unit, dup_head_id, icc_count));
        head_id++;
        dup_head_id++;
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Dynamic replication list update.
 * This function implements the sequence needed for the dynamic updation
 * of the replication list.
 *
 * \param [in] unit Logical device id.
 * \param [in] repl_head_info Replication head
 * \param [in] start_ptr New replication list.
 * \param [in] repl_count Replication count. This denotes icc count for the
 * replication list.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56990_a0_mc_repl_list_update(int unit,
                                bcmtm_mc_repl_head_info_t *repl_head_info,
                                uint32_t start_ptr,
                                int repl_count)
{
    bool psim, asim, emul;
    bcmtm_mc_repl_group_info_t mc_group_info;
    int base_ptr, mc_group_id;

    SHR_FUNC_ENTER(unit);

    psim = bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM);
    asim = bcmdrd_feature_enabled(unit, BCMDRD_FT_ACTIVE_SIM);
    emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);

    sal_memset(&mc_group_info, 0, sizeof(bcmtm_mc_repl_group_info_t));
    mc_group_id = repl_head_info->mc_group_id;

    /*
     * 2. Setup new replication head that duplicates the old
     * REPL_HEAD/MEMBER_ICC entries.
     */
    SHR_IF_ERR_EXIT
        (bcm56990_a0_mc_repl_group_entry_get(unit, mc_group_id, &mc_group_info));
    SHR_IF_ERR_EXIT
        (bcm56990_a0_mc_repl_head_copy(unit, &mc_group_info,
                                       repl_head_info, start_ptr,
                                       &base_ptr, repl_count));

    /*
     * 3. Setup REPL_GROUP entry 0 which duplicates the old entry except the
     * base pointer points newly setup REPL_HEAD.
     */
    mc_group_info.base_ptr = base_ptr;
    SHR_IF_ERR_EXIT
        (bcm56990_a0_mc_repl_group_entry_set(unit, 0, &mc_group_info));

    /*
     * 4. Set MMU_RQE_DYNAMIC_PROGRAMMING_CONTROL.MC_IDX to mc_group_id to be
     * changed with the new replication list.
     */
    SHR_IF_ERR_EXIT
        (bcm56990_a0_mc_dynamic_repl_set(unit, mc_group_id, 1));
    /*
     * 5. Wait for 2ms and then issue RQE_FLUSH.
     */
    if (psim || asim) {
        SHR_EXIT();
    }
    sal_usleep(2000 + (emul ? 1 : 0) * EMULATION_FACTOR);
    SHR_IF_ERR_EXIT
        (bcm56990_a0_tm_rqe_port_flush(unit));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Dynamic replication list update cleanup.
 * This function implements the sequence for clean up once dynamic replication
 * list is updated.
 *
 * \param [in] unit Logical device id.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56990_a0_mc_repl_list_update_cleanup(int unit)
{
    int idx = 0;
    bool psim, asim, emul;
    bcmtm_mc_repl_group_info_t mc_group_info;
    int base_ptr = 0;

    SHR_FUNC_ENTER(unit);
    psim = bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM);
    asim = bcmdrd_feature_enabled(unit, BCMDRD_FT_ACTIVE_SIM);
    emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);

    sal_memset(&mc_group_info, 0, sizeof(bcmtm_mc_repl_group_info_t));

    /*
     * 7. Reset MMU_RQE_DYNAMIC_PROGRAMMING_CONTROL.MC_IDX.
     */
    SHR_IF_ERR_EXIT
        (bcm56990_a0_mc_dynamic_repl_set(unit, 0, 0));

    /*
     * 8.1 Wait for 2 msec and then issue MMU_RQE flush.
     * 8.2 Free up group 0 allocated entries.
     */
    if (!psim && !asim) {
        sal_usleep(2000 + (emul ? 1 : 0) * EMULATION_FACTOR);
        SHR_IF_ERR_EXIT
            (bcm56990_a0_tm_rqe_port_flush(unit));
    }
    /* Clear head list */
    SHR_IF_ERR_EXIT
        (bcm56990_a0_mc_repl_group_entry_get(unit, 0, &mc_group_info));
    base_ptr = mc_group_info.base_ptr;
    for (idx = 0; idx < mc_group_info.num_entries; idx++) {
        SHR_IF_ERR_EXIT
            (bcm56990_a0_mc_repl_head_entry_set(unit, base_ptr, 0));
        SHR_IF_ERR_EXIT
            (bcm56990_a0_mc_icc_set(unit, base_ptr, 0));
        base_ptr++;
    }

    /* Reset REPL_GROUP entry 0 */
    sal_memset(&mc_group_info, 0, sizeof(bcmtm_mc_repl_group_info_t));
    SHR_IF_ERR_EXIT
        (bcm56990_a0_mc_repl_group_entry_set(unit, 0, &mc_group_info));
exit:
    SHR_FUNC_EXIT();
}


static int
bcm56990_a0_mc_repl_list_entry_set (int unit,
                                    bcmtm_mc_repl_list_entry_t *mc_list_entry)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bcmdrd_fid_t fid_list[] = {
                    NEXT_HOP_INDEX_0f,
                    NEXT_HOP_INDEX_1f,
                    NEXT_HOP_INDEX_2f,
                    NEXT_HOP_INDEX_3f
                    };
    uint32_t ltmbuf[BCMTM_MC_MAX_ENTRY_WSIZE] = {0};
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmltd_sid_t ltid = -1;
    int remaining_reps, i, fval;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mc_list_entry, SHR_E_PARAM);
    if (mc_list_entry->entry_ptr == 0) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    sid = MMU_REPL_LIST_TBLm;

    BCMTM_PT_DYN_INFO(pt_dyn_info, mc_list_entry->entry_ptr, 0);
    if (mc_list_entry->sparse_mode) {
        for (i = 0; i < 4; i++) {
            if (mc_list_entry->mode_bitmap & (1U << i)) {
                fid = fid_list[i];
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid, fid, ltmbuf,
                                     (uint32_t *)&mc_list_entry->nhop[i]));
            }
        }
        fid = MODEf;
        fval = 1;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, fid, ltmbuf, (uint32_t *)&fval));

        fid = MODE_1_BITMAPf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, fid, ltmbuf,
                             (uint32_t *)&mc_list_entry->mode_bitmap));
    } else {
        fid = MODEf;
        fval = 0;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, fid, ltmbuf, (uint32_t *)&fval));

        fid = LSB_VLAN_BMf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, fid, ltmbuf,
                             (uint32_t *)&mc_list_entry->nhop));
        fid = MSB_VLANf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, fid, ltmbuf,
                             (uint32_t *)&mc_list_entry->msb));
    }

    fid = RMNG_REPSf;
    if (mc_list_entry->remaining_reps > 31) {
        remaining_reps = 0;
    } else {
        remaining_reps = mc_list_entry->remaining_reps;
    }
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf, (uint32_t *)&remaining_reps));

    fid = NEXTPTRf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf,
                         (uint32_t *)&mc_list_entry->next_ptr));

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, (void *)&pt_dyn_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_mc_repl_list_entry_get (int unit,
                                    bcmtm_mc_repl_list_entry_t *mc_list_entry)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bcmdrd_fid_t fid_list[] = {
                    NEXT_HOP_INDEX_0f,
                    NEXT_HOP_INDEX_1f,
                    NEXT_HOP_INDEX_2f,
                    NEXT_HOP_INDEX_3f
                    };
    uint32_t ltmbuf[BCMTM_MC_MAX_ENTRY_WSIZE] = {0};
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmltd_sid_t ltid = -1;
    int start_ptr, i;
    uint32_t fval;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mc_list_entry, SHR_E_PARAM);
    if (mc_list_entry->entry_ptr == 0) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    sid = MMU_REPL_LIST_TBLm;

    start_ptr = mc_list_entry->entry_ptr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, start_ptr, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, (void *)&pt_dyn_info,
                               ltmbuf));
    fid = MODEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, ltmbuf, &fval));
    mc_list_entry->sparse_mode = fval;
    if (mc_list_entry->sparse_mode) {
        fid = MODE_1_BITMAPf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_get(unit, sid, fid, ltmbuf,
                            (uint32_t *)&mc_list_entry->mode_bitmap));
        for (i = 0; i < 4; i++) {
            mc_list_entry->nhop[i] = 0;
            if (mc_list_entry->mode_bitmap & (1U << i)) {
                fid = fid_list[i];
                SHR_IF_ERR_EXIT
                    (bcmtm_field_get(unit, sid, fid, ltmbuf,
                                     (uint32_t *)&mc_list_entry->nhop[i]));
            }
        }

    } else {
        fid = MSB_VLANf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_get(unit, sid, fid, ltmbuf,
                             (uint32_t *)&mc_list_entry->msb));

        fid = LSB_VLAN_BMf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_get(unit, sid, fid, ltmbuf,
                             (uint32_t *)&mc_list_entry->nhop));
    }

    mc_list_entry->remaining_reps = -1;

    fid = NEXTPTRf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, ltmbuf,
                         (uint32_t *)&mc_list_entry->next_ptr));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_mc_repl_list_in_use_get (int unit, int repl_entry_ptr,
                                     bool *in_use)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf[BCMTM_MC_MAX_ENTRY_WSIZE] = {0};
    bcmtm_pt_info_t pt_dyn_info = {0};
    int entry_done, itm, i, entry_ptr;
    bcmltd_sid_t ltid = -1;

    SHR_FUNC_ENTER(unit);
    sid = MMU_REPL_STATE_TBLm;

    *in_use = 0;
    for (itm = 0; itm < 2; itm++) {
        for (i = 0; i < 9; i++) {
            BCMTM_PT_DYN_INFO(pt_dyn_info, i, itm);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_read(unit, sid, ltid,
                            (void *)&pt_dyn_info, ltmbuf));
            fid = ENTRY_DONEf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_get(unit, sid, fid, ltmbuf, (uint32_t *)&entry_done));
            if (entry_done) {
                continue;
            }
            fid = LIST_PTRf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_get(unit, sid, fid, ltmbuf, (uint32_t *)&entry_ptr));
            if (entry_ptr == repl_entry_ptr) {
                *in_use = 1;
                SHR_EXIT();
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_mc_ipmc_set (int unit, int mc_group_id,
                         bcmtm_mc_ipmc_info_t *ipmc_info)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf[BCMTM_MC_MAX_ENTRY_WSIZE] = {0};
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmltd_sid_t ltid = -1;
    int fval;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ipmc_info, SHR_E_PARAM);
    sid = L3_IPMCm;

    BCMTM_PT_DYN_INFO(pt_dyn_info, mc_group_id, 0);
    fid = L2_BITMAPf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf,
                         (uint32_t *)ipmc_info->l2_bitmap));
    fid = L3_BITMAPf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf,
                         (uint32_t *)ipmc_info->l3_bitmap));

    fid = DO_NOT_CUT_THROUGHf;
    fval = ipmc_info->do_not_cut_through;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf,
                         (uint32_t *)&fval));

    fid = VALIDf;
    fval = ipmc_info->valid;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf,
                         (uint32_t *)&fval));

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, (void *)&pt_dyn_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_mc_ipmc_get (int unit, int mc_group_id,
                         bcmtm_mc_ipmc_info_t *ipmc_info)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf[BCMTM_MC_MAX_ENTRY_WSIZE] = {0};
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmltd_sid_t ltid = -1;
    uint32_t fval;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ipmc_info, SHR_E_PARAM);
    sid = L3_IPMCm;

    BCMTM_PT_DYN_INFO(pt_dyn_info, mc_group_id, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, (void *)&pt_dyn_info, ltmbuf));

    fid = VALIDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, ltmbuf,
                         (uint32_t *)&fval));
    ipmc_info->valid = fval;

    fid = DO_NOT_CUT_THROUGHf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, ltmbuf,
                         (uint32_t *)&fval));
    ipmc_info->do_not_cut_through = fval;

    fid = L3_BITMAPf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, ltmbuf,
                         (uint32_t *)ipmc_info->l3_bitmap));

    fid = L2_BITMAPf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, ltmbuf,
                         (uint32_t *)ipmc_info->l2_bitmap));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_mc_repl_head_index_count_get(int unit, int *num_index)
{
    int max, min;
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(num_index, SHR_E_PARAM);
    max = bcmdrd_pt_index_max(unit, MMU_REPL_HEAD_TBLm);
    min = bcmdrd_pt_index_min(unit, MMU_REPL_HEAD_TBLm);
    *num_index = max - min;
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_mc_nhop_index_count_get(int unit, int *num_index)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(num_index, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
           (bcmlrd_table_max_index_get(unit, L3_MC_NHOPt,
                               (uint32_t *)num_index));
    *num_index = *num_index + 1;
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_mc_nhops_per_repl_index_sparse_mode_get(int unit, int *num_index)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(num_index, SHR_E_PARAM);
    *num_index = TH4_NHOPS_PER_REPL_INDEX_SPARSE_MODE;
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_mc_max_packet_replication_get(int unit, int *num_index)
{
    SHR_FUNC_ENTER(unit);
    *num_index = TH4_MAX_MC_PKT_REPL;
    SHR_FUNC_EXIT();
}

/*! \brief Get the multicast replication list free count.
 *
 * \param [in] unit Logical unit number.
 * \param [out] free Number of free replication list.
 */
static int
bcm56990_a0_mc_repl_list_free_count(int unit, uint64_t *free_count)
{
    int in_use = 0;
    bcmtm_mc_dev_info_t *mc_dev;
    int tc = TH4_MAX_REPL_LIST_COUNT;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_mc_dev_info_get(unit, &mc_dev));

    SHR_BITCOUNT_RANGE(mc_dev->used_list_bmp, in_use, 0, tc);
    *free_count = TH4_MAX_REPL_LIST_COUNT - in_use;
exit:
    SHR_FUNC_EXIT();
}
/*******************************************************************************
 * Public functions
 */
int
bcm56990_a0_tm_multicast_init (int unit, int warm)
{
    uint32_t ltmbuf[BCMTM_MC_MAX_ENTRY_WSIZE] = {0};
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bcmltd_sid_t ltid = -1;
    uint32_t size  = 0;

    SHR_FUNC_ENTER(unit);

    if (!warm) {
        size = bcmdrd_pt_index_max(unit,L2MCm) + 1;
        sid = MC_CONTROL_5r;
        BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
        fid = SHARED_TABLE_L2MC_SIZEf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, fid, ltmbuf,
                             &size));
        size = bcmdrd_pt_index_max(unit,L3_IPMCm) + 1;
        fid = SHARED_TABLE_IPMC_SIZEf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, fid, ltmbuf,
                             &size));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, ltid,
                                    (void *)&pt_dyn_info, ltmbuf));
    }
exit:
    SHR_FUNC_EXIT();
}

int bcm56990_a0_tm_mc_drv_get(int unit, void *mc_drv)
{
    bcmtm_mc_drv_t bcm56990_a0_mc_drv = {
        .mc_repl_group_entry_set = bcm56990_a0_mc_repl_group_entry_set,
        .mc_repl_group_entry_get = bcm56990_a0_mc_repl_group_entry_get,
        .mc_repl_head_entry_set = bcm56990_a0_mc_repl_head_entry_set,
        .mc_repl_head_entry_get = bcm56990_a0_mc_repl_head_entry_get,
        .mc_repl_list_entry_set = bcm56990_a0_mc_repl_list_entry_set,
        .mc_repl_list_entry_get = bcm56990_a0_mc_repl_list_entry_get,
        .mc_repl_list_in_use_get = bcm56990_a0_mc_repl_list_in_use_get,
        .mc_repl_icc_set = bcm56990_a0_mc_icc_set,
        .mc_ipmc_set = bcm56990_a0_mc_ipmc_set,
        .mc_ipmc_get = bcm56990_a0_mc_ipmc_get,
        .mc_repl_head_index_count_get = bcm56990_a0_mc_repl_head_index_count_get,
        .mc_nhop_index_count_get = bcm56990_a0_mc_nhop_index_count_get,
        .mc_nhops_per_repl_index_sparse_mode_get =
            bcm56990_a0_mc_nhops_per_repl_index_sparse_mode_get,
        .mc_max_packet_replication_get = bcm56990_a0_mc_max_packet_replication_get,
        .mc_repl_list_update = bcm56990_a0_mc_repl_list_update,
        .mc_repl_list_update_cleanup = bcm56990_a0_mc_repl_list_update_cleanup,
        .mc_repl_list_free_count = bcm56990_a0_mc_repl_list_free_count,
    };
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mc_drv, SHR_E_INTERNAL);

    *((bcmtm_mc_drv_t *)mc_drv) = bcm56990_a0_mc_drv;
exit:
    SHR_FUNC_EXIT();
}
