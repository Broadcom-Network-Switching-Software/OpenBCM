/*! \file bcm56780_a0_tm_multicast.c
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
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmtm/chip/bcm56780_a0_tm.h>
#include <bcmtm/multicast/bcmtm_mc_group.h>
#include <bcmlrd/bcmlrd_table.h>
#include "bcm56780_a0_tm_multicast.h"
#include "bcm56780_a0_tm_mmu_flexport.h"

/*******************************************************************************
 * Local definitions
 */
#define BCMTM_MC_MAX_ENTRY_WSIZE 12

#define BSL_LOG_MODULE  BSL_LS_BCMTM_CHIP
/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Mutlicast initial copy count.
 *
 * \param [in] unit Unit number.
 * \param [in] mc_head_id Multicast head ID.
 * \param [in] repl_count Replication count. This denotes number of next hop
 * for a given multicast head ID.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56780_a0_mc_icc_set (int unit, int mc_head_id, int repl_count)
{
#define BCM56780_A0_IPMC_MAX_INTF_COUNT_PER_ENTRY 64
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf[BCMTM_MC_MAX_ENTRY_WSIZE] = {0};
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1;
    int icc_entry_ptr, icc_field_index;
    bcmdrd_fid_t icc_fields[] = {
                              ICC_VAL0f, ICC_VAL1f, ICC_VAL2f, ICC_VAL3f,
                              ICC_VAL4f, ICC_VAL5f, ICC_VAL6f, ICC_VAL7f,
                              ICC_VAL8f, ICC_VAL9f, ICC_VAL10f, ICC_VAL11f,
                              ICC_VAL12f, ICC_VAL13f, ICC_VAL14f, ICC_VAL15f,
                              ICC_VAL16f, ICC_VAL17f, ICC_VAL18f, ICC_VAL19f,
                              ICC_VAL20f, ICC_VAL21f, ICC_VAL22f, ICC_VAL23f,
                              ICC_VAL24f, ICC_VAL25f, ICC_VAL26f, ICC_VAL27f,
                              ICC_VAL28f, ICC_VAL29f, ICC_VAL30f, ICC_VAL31f,
                              ICC_VAL32f, ICC_VAL33f, ICC_VAL34f, ICC_VAL35f,
                              ICC_VAL36f, ICC_VAL37f, ICC_VAL38f, ICC_VAL39f,
                              ICC_VAL40f, ICC_VAL41f, ICC_VAL42f, ICC_VAL43f,
                              ICC_VAL44f, ICC_VAL45f, ICC_VAL46f, ICC_VAL47f,
                              ICC_VAL48f, ICC_VAL49f, ICC_VAL50f, ICC_VAL51f,
                              ICC_VAL52f, ICC_VAL53f, ICC_VAL54f, ICC_VAL55f,
                              ICC_VAL56f, ICC_VAL57f, ICC_VAL58f, ICC_VAL59f,
                              ICC_VAL60f, ICC_VAL61f, ICC_VAL62f, ICC_VAL63f,
                        };

    SHR_FUNC_ENTER(unit);

    sid = MMU_REPL_MEMBER_ICCm;
    icc_entry_ptr = mc_head_id / BCM56780_A0_IPMC_MAX_INTF_COUNT_PER_ENTRY;
    icc_field_index = mc_head_id % BCM56780_A0_IPMC_MAX_INTF_COUNT_PER_ENTRY;


    BCMTM_PT_DYN_INFO(pt_info, icc_entry_ptr, 0);
    fid = icc_fields[icc_field_index];
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, (void *)&pt_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf, (uint32_t *)&repl_count));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, (void *)&pt_info, ltmbuf));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get initial copy count for a multicast head ID.
 *
 * \param [in] unit Logical device ID.
 * \param [in] mc_head_id Multicast head ID.
 * \param [out] repl_count Number of replication associated with multicast head ID.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE error code.
 */
static int
bcm56780_a0_mc_icc_get (int unit, int mc_head_id, int *repl_count)
{
#define BCM56780_A0_IPMC_MAX_INTF_COUNT_PER_ENTRY 64
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf[BCMTM_MC_MAX_ENTRY_WSIZE] = {0}, fval;
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1;
    int icc_entry_ptr, icc_field_index;
    bcmdrd_fid_t icc_fields[] = {
        ICC_VAL0f, ICC_VAL1f, ICC_VAL2f, ICC_VAL3f,
        ICC_VAL4f, ICC_VAL5f, ICC_VAL6f, ICC_VAL7f,
        ICC_VAL8f, ICC_VAL9f, ICC_VAL10f, ICC_VAL11f,
        ICC_VAL12f, ICC_VAL13f, ICC_VAL14f, ICC_VAL15f,
        ICC_VAL16f, ICC_VAL17f, ICC_VAL18f, ICC_VAL19f,
        ICC_VAL20f, ICC_VAL21f, ICC_VAL22f, ICC_VAL23f,
        ICC_VAL24f, ICC_VAL25f, ICC_VAL26f, ICC_VAL27f,
        ICC_VAL28f, ICC_VAL29f, ICC_VAL30f, ICC_VAL31f,
        ICC_VAL32f, ICC_VAL33f, ICC_VAL34f, ICC_VAL35f,
        ICC_VAL36f, ICC_VAL37f, ICC_VAL38f, ICC_VAL39f,
        ICC_VAL40f, ICC_VAL41f, ICC_VAL42f, ICC_VAL43f,
        ICC_VAL44f, ICC_VAL45f, ICC_VAL46f, ICC_VAL47f,
        ICC_VAL48f, ICC_VAL49f, ICC_VAL50f, ICC_VAL51f,
        ICC_VAL52f, ICC_VAL53f, ICC_VAL54f, ICC_VAL55f,
        ICC_VAL56f, ICC_VAL57f, ICC_VAL58f, ICC_VAL59f,
        ICC_VAL60f, ICC_VAL61f, ICC_VAL62f, ICC_VAL63f,
    };

    SHR_FUNC_ENTER(unit);

    sid = MMU_REPL_MEMBER_ICCm;
    icc_entry_ptr = mc_head_id / BCM56780_A0_IPMC_MAX_INTF_COUNT_PER_ENTRY;
    icc_field_index = mc_head_id % BCM56780_A0_IPMC_MAX_INTF_COUNT_PER_ENTRY;


    BCMTM_PT_DYN_INFO(pt_info, icc_entry_ptr, 0);
    fid = icc_fields[icc_field_index];
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, ltmbuf, &fval));
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
bcm56780_a0_mc_dynamic_repl_set(int unit,
                                uint32_t mc_group_id,
                                uint32_t enable)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf = 0;
    bcmlrd_sid_t ltid = -1;
    bcmtm_pt_info_t pt_info = {0};

    SHR_FUNC_ENTER(unit);

    sid = MMU_RQE_DYNAMIC_PROGRAMMING_CONTROLr;
    fid = MC_IDXf;
    BCMTM_PT_DYN_INFO(pt_info, 0, 0);

    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &mc_group_id));
    fid = ENABLEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &enable));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get replication list.
 * This gives the start pointer to the replication list for a given
 * multicast head ID.
 *
 * \param [in] unit Logical device id.
 * \param [in] mc_head_id Multicast head ID.
 * \param [out] start_ptr Pointer to the first replication list entry in
 * replication ID linked list.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56780_a0_mc_repl_head_entry_get (int unit,
                                    uint32_t mc_head_id,
                                    uint32_t *start_ptr)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf[BCMTM_MC_MAX_ENTRY_WSIZE] = {0};
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1;
    uint32_t fval = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(start_ptr, SHR_E_PARAM);
    sid = MMU_REPL_HEAD_TBLm;

    BCMTM_PT_DYN_INFO(pt_info, mc_head_id, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, (void *)&pt_info, ltmbuf));

    fid = HEAD_PTRf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, ltmbuf, &fval));
    *start_ptr = fval;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set replication list entry to the multicast head ID.
 *
 * \param [in] unit Logical device id.
 * \param [in] mc_head_id Multicast head ID.
 * \param [in] start_ptr Pointer to the first replication list entry in
 * replication ID linked list.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56780_a0_mc_repl_head_entry_set (int unit,
                                    uint32_t mc_head_id,
                                    uint32_t start_ptr)
{
    bcmdrd_sid_t sid = MMU_REPL_HEAD_TBLm;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf[BCMTM_MC_MAX_ENTRY_WSIZE] = {0};
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1;

    SHR_FUNC_ENTER(unit);

    BCMTM_PT_DYN_INFO(pt_info, mc_head_id, 0);
    fid = HEAD_PTRf;

    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf, &start_ptr));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
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
bcm56780_a0_mc_repl_head_copy(int unit,
                              bcmtm_mc_repl_group_info_t *mc_group_info,
                              bcmtm_mc_repl_head_info_t *repl_head_info,
                              uint32_t start_ptr,
                              int *base_ptr,
                              int repl_count)
{
#define TD4_X9_MC_RESERVED_HEAD_ENTRY 147296
    int num_entry, icc_count;
    uint32_t head_id = 0;
    uint32_t dup_head_id = TD4_X9_MC_RESERVED_HEAD_ENTRY;
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
                (bcm56780_a0_mc_repl_head_entry_get(unit, head_id, &list_ptr));
            SHR_IF_ERR_EXIT
                (bcm56780_a0_mc_icc_get(unit, head_id, &icc_count));
        }
        SHR_IF_ERR_EXIT
            (bcm56780_a0_mc_repl_head_entry_set(unit, dup_head_id, list_ptr));
        SHR_IF_ERR_EXIT
            (bcm56780_a0_mc_icc_set(unit, dup_head_id, icc_count));
        head_id++;
        dup_head_id++;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set multicast replication group entry.
 * This configures MMU_REPL_GROUP_INFO_TBLm for a given multicast index.
 *
 * \param [in] unit Logical device id.
 * \param [in] mc_group_id Multicast group ID.
 * \param [in] mc_group_info Mutlicast group configurations.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56780_a0_mc_repl_group_entry_set (int unit, uint32_t mc_group_id,
                                     bcmtm_mc_repl_group_info_t *mc_group_info)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bool psim, asim;
    uint32_t ltmbuf[BCMTM_MC_MAX_ENTRY_WSIZE] = {0};
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mc_group_info, SHR_E_PARAM);
    sid = MMU_REPL_GROUP_INFO_TBLm;

    psim = bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM);
    asim = bcmdrd_feature_enabled(unit, BCMDRD_FT_ACTIVE_SIM);

    BCMTM_PT_DYN_INFO(pt_info, mc_group_id, 0);
    fid = MEMBER_BMPf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf,
                         (uint32_t *)&mc_group_info->port_agg_bmp));

    fid = BASE_PTRf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf,
                         (uint32_t *)&mc_group_info->base_ptr));

    SHR_BITCOUNT_RANGE(mc_group_info->port_agg_bmp, mc_group_info->num_entries,
                        0, MAX_PORT_AGG_BMP);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, (void *)&pt_info, ltmbuf));

    if (mc_group_info->rqe_flush) {
        if (psim || asim) {
            SHR_EXIT();
        }
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_rqe_port_flush(unit));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get multicast replication group entry.
 * This gets the configured MMU_REPL_GROUP_INFO_TBLm for a given multicast index.
 *
 * \param [in] unit Logical device id.
 * \param [in] mc_group_id Multicast group ID.
 * \param [out] mc_group_info Mutlicast group configurations.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56780_a0_mc_repl_group_entry_get (int unit,
                                     uint32_t mc_group_id,
                                     bcmtm_mc_repl_group_info_t *mc_group_info)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf[BCMTM_MC_MAX_ENTRY_WSIZE] = {0};
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mc_group_info, SHR_E_PARAM);
    sid = MMU_REPL_GROUP_INFO_TBLm;

    BCMTM_PT_DYN_INFO(pt_info, mc_group_id, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, (void *)&pt_info, ltmbuf));

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
bcm56780_a0_mc_repl_list_update(int unit,
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
        (bcm56780_a0_mc_repl_group_entry_get(unit, mc_group_id, &mc_group_info));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_mc_repl_head_copy(unit, &mc_group_info,
                                       repl_head_info, start_ptr,
                                       &base_ptr, repl_count));

    /*
     * 3. Setup REPL_GROUP entry 0 which duplicates the old entry except the
     * base pointer points newly setup REPL_HEAD.
     */
    mc_group_info.base_ptr = base_ptr;
    SHR_IF_ERR_EXIT
        (bcm56780_a0_mc_repl_group_entry_set(unit, 0, &mc_group_info));

    /*
     * 4. Set MMU_RQE_DYNAMIC_PROGRAMMING_CONTROL.MC_IDX to mc_group_id to be
     * changed with the new replication list.
     */
    SHR_IF_ERR_EXIT
        (bcm56780_a0_mc_dynamic_repl_set(unit, mc_group_id, 1));
    /*
     * 5. Wait for 2ms and then issue RQE_FLUSH.
     */
    if (psim || asim) {
        SHR_EXIT();
    }
    sal_usleep(2000 + (emul ? 1 : 0) * EMULATION_FACTOR);
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_rqe_port_flush(unit));
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
bcm56780_a0_mc_repl_list_update_cleanup(int unit)
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
        (bcm56780_a0_mc_dynamic_repl_set(unit, 0, 0));

    /*
     * 8.1 Wait for 2 msec and then issue MMU_RQE flush.
     * 8.2 Free up group 0 allocated entries.
     */
    if (!psim && !asim) {
        sal_usleep(2000 + (emul ? 1 : 0) * EMULATION_FACTOR);
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_rqe_port_flush(unit));
    }
    /* Clear head list */
    SHR_IF_ERR_EXIT
        (bcm56780_a0_mc_repl_group_entry_get(unit, 0, &mc_group_info));
    base_ptr = mc_group_info.base_ptr;
    for (idx = 0; idx < mc_group_info.num_entries; idx++) {
        SHR_IF_ERR_EXIT
            (bcm56780_a0_mc_repl_head_entry_set(unit, base_ptr, 0));
        SHR_IF_ERR_EXIT
            (bcm56780_a0_mc_icc_set(unit, base_ptr, 0));
        base_ptr++;
    }

    /* Reset REPL_GROUP entry 0 */
    sal_memset(&mc_group_info, 0, sizeof(bcmtm_mc_repl_group_info_t));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_mc_repl_group_entry_set(unit, 0, &mc_group_info));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set multicast replication list.
 * Programs both the ITMs for the replication list.
 *
 * \param [in] unit Logical device id.
 * \param [in] mc_list_entry Multicast replication list.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56780_a0_mc_repl_list_entry_set (int unit,
                                    bcmtm_mc_repl_list_entry_t *mc_list_entry)
{
    bcmdrd_sid_t sid = MMU_REPL_LIST_TBLm;
    bcmdrd_fid_t fid;
    bcmdrd_fid_t fid_list[] = {
                    NEXT_HOP_INDEX_0f, NEXT_HOP_INDEX_1f, NEXT_HOP_INDEX_2f,
                    NEXT_HOP_INDEX_3f, NEXT_HOP_INDEX_4f, NEXT_HOP_INDEX_5f,
                    };
    uint32_t ltmbuf[BCMTM_MC_MAX_ENTRY_WSIZE] = {0};
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1;
    int i;
    uint32_t fval, remaining_reps;
    bcmtm_mc_dev_info_t *mc_dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_mc_dev_info_get(unit, &mc_dev));
    SHR_NULL_CHECK(mc_list_entry, SHR_E_PARAM);
    if (mc_list_entry->entry_ptr == 0) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    BCMTM_PT_DYN_INFO(pt_info, mc_list_entry->entry_ptr, 0);
    if (mc_list_entry->sparse_mode) {
        for (i = 0; i < mc_dev->nhops_per_repl_index; i++) {
            if (mc_list_entry->mode_bitmap & (1U << i)) {
                fid = fid_list[i];
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid, fid,
                                     ltmbuf, &mc_list_entry->nhop[i]));
            }
        }
        fid = MODEf;
        fval = 1;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

        fid = MODE_1_BITMAPf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, fid, ltmbuf,
                             &mc_list_entry->mode_bitmap));
    } else {
        fid = MODEf;
        fval = 0;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
        fid = LSB_VLAN_BMf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, fid, ltmbuf, &(mc_list_entry->nhop[0])));
        fid = MSB_VLANf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, fid, ltmbuf, &mc_list_entry->msb));
    }

    fid = RMNG_REPSf;
    if (mc_list_entry->remaining_reps > 31) {
        remaining_reps = 0;
    } else {
        remaining_reps = mc_list_entry->remaining_reps;
    }
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf, &(remaining_reps)));

    fid = NEXTPTRf;
    fval = mc_list_entry->next_ptr;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get multicast replication list.
 *
 * \param [in] unit Logical device id.
 * \param [out] mc_list_entry Multicast replication list.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56780_a0_mc_repl_list_entry_get (int unit,
                                    bcmtm_mc_repl_list_entry_t *mc_list_entry)
{
    bcmdrd_sid_t sid = MMU_REPL_LIST_TBLm;
    bcmdrd_fid_t fid;
    bcmdrd_fid_t fid_list[] = {
                    NEXT_HOP_INDEX_0f, NEXT_HOP_INDEX_1f, NEXT_HOP_INDEX_2f,
                    NEXT_HOP_INDEX_3f, NEXT_HOP_INDEX_4f, NEXT_HOP_INDEX_5f,
                    };
    uint32_t ltmbuf[BCMTM_MC_MAX_ENTRY_WSIZE] = {0};
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1;
    int start_ptr, i;
    uint32_t fval;
    bcmtm_mc_dev_info_t *mc_dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_mc_dev_info_get(unit, &mc_dev));
    SHR_NULL_CHECK(mc_list_entry, SHR_E_PARAM);
    if (mc_list_entry->entry_ptr == 0) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    start_ptr = mc_list_entry->entry_ptr;
    BCMTM_PT_DYN_INFO(pt_info, start_ptr, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, (void *)&pt_info,
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
        for (i = 0; i < mc_dev->nhops_per_repl_index; i++) {
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

/*!
 * Check for if the replication list is in use or free.
 *
 * \param [in] unit Logical devide ID.
 * \param [in] repl_entry_ptr Replication list.
 * \param [out] in_use Status of the replication list.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56780_a0_mc_repl_list_in_use_get (int unit,
                                     int repl_entry_ptr,
                                     bool *in_use)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf[BCMTM_MC_MAX_ENTRY_WSIZE] = {0};
    bcmtm_pt_info_t pt_info = {0};
    int entry_done, i, entry_ptr;
    bcmltd_sid_t ltid = -1;

    SHR_FUNC_ENTER(unit);
    sid = MMU_REPL_STATE_TBLm;

    *in_use = 0;
    for (i = 0; i < 9; i++) {
        BCMTM_PT_DYN_INFO(pt_info, i, 0);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, sid, ltid,
                                   (void *)&pt_info, ltmbuf));
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
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief IP multicast replication config set.
 *
 * \param [in] unit Logical device id.
 * \param [in] mc_group_id Multicast group ID.
 * \param [in] ipmc_info IPMC config.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56780_a0_mc_ipmc_set (int unit,
                         int mc_group_id,
                         bcmtm_mc_ipmc_info_t *ipmc_info)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf[BCMTM_MC_MAX_ENTRY_WSIZE] = {0};
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1;
    int fval;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ipmc_info, SHR_E_PARAM);
    sid = IPOST_REPLICATION_L3_BITMAP_PROFILE_LOWERm;

    BCMTM_PT_DYN_INFO(pt_info, mc_group_id, 0);
    fid = L2_BITMAPf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf,
                         (uint32_t *)ipmc_info->l2_bitmap));
    fid = IGNORE_L2_BITMAPf;
    fval = ipmc_info->l2_mc_disable;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf,
                         (uint32_t *)&fval));

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, (void *)&pt_info, ltmbuf));

    sid = IPOST_REPLICATION_L3_BITMAP_PROFILE_UPPERm;

    BCMTM_PT_DYN_INFO(pt_info, mc_group_id, 0);
    fid = L3_BITMAPf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf,
                         (uint32_t *)ipmc_info->l3_bitmap));

    fid = DO_NOT_CUT_THROUGHf;
    fval = ipmc_info->do_not_cut_through;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf,
                         (uint32_t *)&fval));


    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, (void *)&pt_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief IP multicast replication config get.
 *
 * \param [in] unit Logical device id.
 * \param [in] mc_group_id Multicast group ID.
 * \param [out] ipmc_info IPMC config.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56780_a0_mc_ipmc_get (int unit,
                         int mc_group_id,
                         bcmtm_mc_ipmc_info_t *ipmc_info)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf[BCMTM_MC_MAX_ENTRY_WSIZE] = {0};
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1;
    uint32_t fval;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ipmc_info, SHR_E_PARAM);

    sid = IPOST_REPLICATION_L3_BITMAP_PROFILE_LOWERm;
    BCMTM_PT_DYN_INFO(pt_info, mc_group_id, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, (void *)&pt_info, ltmbuf));
    fid = L2_BITMAPf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, ltmbuf,
                         (uint32_t *)ipmc_info->l2_bitmap));
    fid = IGNORE_L2_BITMAPf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, ltmbuf,
                         (uint32_t *)&fval));

    ipmc_info->l2_mc_disable = fval;


    sid = IPOST_REPLICATION_L3_BITMAP_PROFILE_UPPERm;

    BCMTM_PT_DYN_INFO(pt_info, mc_group_id, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, (void *)&pt_info, ltmbuf));
    fid = DO_NOT_CUT_THROUGHf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, ltmbuf,
                         (uint32_t *)&fval));
    ipmc_info->do_not_cut_through = fval;

    fid = L3_BITMAPf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, ltmbuf,
                         (uint32_t *)ipmc_info->l3_bitmap));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the number of replication head table count.
 * \param [in] unit Logical device id.
 * \param [out] num_index Number of entries.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM num_index is null.
 */
static int
bcm56780_a0_mc_repl_head_index_count_get(int unit, int *num_index)
{
    int max, min;
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(num_index, SHR_E_PARAM);
    max = bcmdrd_pt_index_max(unit, MMU_REPL_HEAD_TBLm);
    min = bcmdrd_pt_index_min(unit, MMU_REPL_HEAD_TBLm);
    *num_index = max - min + 1;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the maximum number of next hops.
 * \param [in] unit Logical device id.
 * \param [out] num_index Number of next hops.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM num_index is null.
 */
static int
bcm56780_a0_mc_nhop_index_count_get(int unit, int *num_index)
{
#define MAX_NUM_NHOP_INDEX 32768
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(num_index, SHR_E_PARAM);
    *num_index = MAX_NUM_NHOP_INDEX;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the number of next hops per replication index in sparse mode.
 * \param [in] unit Logical device id.
 * \param [out] num_index Number of next hops.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM num_index is null.
 */
static int
bcm56780_a0_mc_nhops_per_repl_index_sparse_mode_get(int unit, int *num_index)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(num_index, SHR_E_PARAM);
    *num_index = TD4_X9_NHOPS_PER_REPL_INDEX_SPARSE_MODE;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the maximum number of packet replications.
 * \param [in] unit Logical device id.
 * \param [out] num_index Maximum number of replication for a multicast group ID.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM num_index is null.
 */
static int
bcm56780_a0_mc_max_packet_replication_get(int unit, int *num_index)
{
    SHR_FUNC_ENTER(unit);
    *num_index = TD4_X9_MAX_MC_PKT_REPL;
    SHR_FUNC_EXIT();
}

/*! \brief Get the multicast replication list free count.
 *
 * \param [in] unit Logical unit number.
 * \param [out] free Number of free replication list.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM num_index is null.
 */
static int
bcm56780_a0_mc_repl_list_free_count(int unit, uint64_t *free_count)
{
    int in_use = 0;
    bcmtm_mc_dev_info_t *mc_dev;
    int tc = TD4_X9_MAX_REPL_LIST_COUNT;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_mc_dev_info_get(unit, &mc_dev));

    SHR_BITCOUNT_RANGE(mc_dev->used_list_bmp, in_use, 0, tc);
    *free_count = TD4_X9_MAX_REPL_LIST_COUNT - in_use;
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcm56780_a0_tm_mc_drv_get(int unit, void *mc_drv)
{
    bcmtm_mc_drv_t bcm56780_a0_mc_drv = {
        .mc_repl_group_entry_set = bcm56780_a0_mc_repl_group_entry_set,
        .mc_repl_group_entry_get = bcm56780_a0_mc_repl_group_entry_get,
        .mc_repl_head_entry_set = bcm56780_a0_mc_repl_head_entry_set,
        .mc_repl_head_entry_get = bcm56780_a0_mc_repl_head_entry_get,
        .mc_repl_list_entry_set = bcm56780_a0_mc_repl_list_entry_set,
        .mc_repl_list_entry_get = bcm56780_a0_mc_repl_list_entry_get,
        .mc_repl_list_in_use_get = bcm56780_a0_mc_repl_list_in_use_get,
        .mc_repl_icc_set = bcm56780_a0_mc_icc_set,
        .mc_ipmc_set = bcm56780_a0_mc_ipmc_set,
        .mc_ipmc_get = bcm56780_a0_mc_ipmc_get,
        .mc_repl_head_index_count_get = bcm56780_a0_mc_repl_head_index_count_get,
        .mc_nhop_index_count_get = bcm56780_a0_mc_nhop_index_count_get,
        .mc_nhops_per_repl_index_sparse_mode_get =
                            bcm56780_a0_mc_nhops_per_repl_index_sparse_mode_get,
        .mc_max_packet_replication_get =
                            bcm56780_a0_mc_max_packet_replication_get,
        .mc_repl_list_update = bcm56780_a0_mc_repl_list_update,
        .mc_repl_list_update_cleanup = bcm56780_a0_mc_repl_list_update_cleanup,
        .mc_repl_list_free_count = bcm56780_a0_mc_repl_list_free_count,
    };
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mc_drv, SHR_E_INTERNAL);

    *((bcmtm_mc_drv_t *)mc_drv) = bcm56780_a0_mc_drv;
exit:
    SHR_FUNC_EXIT();
}
