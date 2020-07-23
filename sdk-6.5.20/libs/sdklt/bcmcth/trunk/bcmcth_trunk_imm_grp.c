/*! \file bcmcth_trunk_imm_grp.c
 *
 * Purpose: IMM handler implementation for TRUNK LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmcth/bcmcth_trunk_types.h>
#include <bcmcth/bcmcth_trunk_util.h>
#include <bcmcth/bcmcth_trunk_db.h>
#include <bcmcth/bcmcth_trunk_drv.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmissu/issu_api.h>

/************************************************************************
 * Private variables
 */
/*! TRUNK bookkeeping HA structure. */
static bcmcth_trunk_imm_grp_bk_t *grp_bk[BCMDRD_CONFIG_MAX_UNITS];

/*! TRUNK bookkeeping temporary structure. */
static bcmcth_trunk_imm_grp_bk_t *grp_bk_temp[BCMDRD_CONFIG_MAX_UNITS];

/*! TRUNK group SW bitmap changed in one commit. */
SHR_BITDCL *bcmcth_trunk_grp_bitmap_diff[BCMDRD_CONFIG_MAX_UNITS];

/*! Pointer to trunk group state. */
#define GRP_HA(unit)                 grp_bk[unit]

/*! Pointer to trunk group temporary state. */
#define GRP_TEMP(unit)               grp_bk_temp[unit]

/*! Pointer to trunk group temporary state. */
#define GRP_DIFF(unit)               bcmcth_trunk_grp_bitmap_diff[unit]

#define BCMCTH_TRUNK_GRP_MAX_MEMBERS(_u_) \
        (GRP_TEMP(_u_)->max_member_cnt)

#define BCMCTH_TRUNK_GRP_MAX(_u_) \
        (GRP_TEMP(_u_)->max_group_cnt)

#define BCMCTH_TRUNK_GRP_INFO(_u_) \
    (GRP_TEMP(_u_)->grp_ptr->group_array)

#define BCMCTH_TRUNK_GRP(_u_, _idx_) \
    (&(GRP_TEMP(_u_)->grp_ptr->group_array[_idx_]))

#define GRP_EXIST(_u_, _idx_) \
    (BCMCTH_TRUNK_GRP(_u_, _idx_)->inserted)

#define BCMCTH_TRUNK_GRP_INSERTED_SET(_u_, _idx_, _val_)  \
    (((BCMCTH_TRUNK_GRP(_u_, _idx_))->inserted) = _val_)

#define GRP_UC_MEMBER_CNT(_u_, _idx_)  \
    ((BCMCTH_TRUNK_GRP(_u_, _idx_))->uc_cnt)

#define BCMCTH_TRUNK_GRP_UC_MEMBER_CNT_SET(_u_, _idx_, _val_)  \
    (((BCMCTH_TRUNK_GRP(_u_, _idx_))->uc_cnt) = _val_)

#define GRP_NONUC_MEMBER_CNT(_u_, _idx_)  \
    ((BCMCTH_TRUNK_GRP(_u_, _idx_))->nonuc_cnt)

#define BCMCTH_TRUNK_GRP_NONUC_MEMBER_CNT_SET(_u_, _idx_, _val_)  \
    (((BCMCTH_TRUNK_GRP(_u_, _idx_))->nonuc_cnt) = _val_)

#define GRP_MAX_MEMBERS(_u_, _idx_)  \
    ((BCMCTH_TRUNK_GRP(_u_, _idx_))->uc_max_members)

#define BCMCTH_TRUNK_GRP_MAX_MEMBERS_SET(_u_, _idx_, _val_)  \
    (((BCMCTH_TRUNK_GRP(_u_, _idx_))->uc_max_members) = _val_)

#define GRP_BASE_PTR(_u_, _idx_)  \
    ((BCMCTH_TRUNK_GRP(_u_, _idx_))->base_ptr)

#define BCMCTH_TRUNK_GRP_NONUC_MEMBER_SYSTEM_PORT(_u_, _idx_) \
    (&((BCMCTH_TRUNK_GRP(_u_, _idx_))->nonuc_system_port))

#define BCMCTH_TRUNK_GRP_UC_MEMBER_SYSTEM_PORT(_u_, _idx_) \
    (&((BCMCTH_TRUNK_GRP(_u_, _idx_))->uc_system_port))

#define BCMCTH_TRUNK_GRP_UC_EGRESS_DISABLE(_u_, _idx_) \
    (&((BCMCTH_TRUNK_GRP(_u_, _idx_))->uc_egr_block))

#define BCMCTH_TRUNK_GRP_NONUC_EGRESS_DISABLE(_u_, _idx_) \
    (&((BCMCTH_TRUNK_GRP(_u_, _idx_))->nonuc_egr_block))

#define BCMCTH_TRUNK_GRP_L2_EIF(_u_, _idx_) \
    (&((BCMCTH_TRUNK_GRP(_u_, _idx_))->l2_eif))

/* BSL Module TBD */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TRUNK

/*!
 * \brief IMM input fields parse routine.
 *
 * Parse TRUNK IMM entry fields and save the data to entry.
 *
 * \param [in]  unit Unit number.
 * \param [in]  key field list.
 * \param [in]  data field list.
 * \param [out] entry TRUNK LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c in to \c LT entry.
 */
static int
bcmcth_trunk_imm_proc_grp_entry_parse(int unit,
                                      const bcmltd_field_t *key,
                                      const bcmltd_field_t *data,
                                      bcmcth_trunk_group_param_t *entry)
{
    uint32_t fid;
    uint32_t arr_idx;
    uint64_t key_val, fval;
    const bcmltd_field_t *in = NULL;

    SHR_FUNC_ENTER(unit);

    /* Read the trunk ID */
    fid = TRUNKt_TRUNK_IDf;
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_trunk_imm_field_get(
                                               unit,
                                               key,
                                               fid,
                                               &key_val));

    entry->id = (uint32_t) key_val;
    LOG_VERBOSE(BSL_LOG_MODULE,
     (BSL_META_U(unit, "TRUNKt_TRUNK_IDf = %d.\n"), entry->id));

    /* For each field ID access the IMM table data fields */
    for (in = data; (in != NULL); in = in->next) {
        fid = in->id;
        fval = in->data;
        arr_idx = in->idx;
        switch (fid) {
            case TRUNKt_LB_MODEf:
                entry->lb_mode = (uint8_t)fval;
                entry->lb_mode_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNKt_LB_MODEf = %d.\n"),
                    (uint8_t) fval));
                break;
            case TRUNKt_UC_MAX_MEMBERSf:
                entry->uc_max_members = (uint16_t) fval;
                entry->uc_max_members_valid = TRUE;
                if ((entry->uc_max_members > BCMCTH_TRUNK_MAX_MEMBERS) ||
                    (entry->uc_max_members < BCMCTH_TRUNK_MIN_MEMBERS)) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }

                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNKt_UC_MAX_MEMBERSf = %d.\n"),
                    (uint16_t) fval));
                break;
            case TRUNKt_UC_MEMBER_CNTf:
                entry->uc_cnt = (uint16_t) fval;
                entry->uc_cnt_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNKt_UC_MEMBER_CNTf = %d.\n"),
                    (uint16_t) fval));
                break;
            case TRUNKt_UC_MEMBER_PORT_SYSTEMf:
                if ((arr_idx) >= BCMCTH_TRUNK_MAX_MEMBERS) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                    "TRUNKt_UC_MEMBER_PORT_SYSTEMf[%d] index invalid.\n"),
                                    arr_idx));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                entry->uc_system_port[arr_idx] = (uint16_t) fval;
                entry->uc_system_port_valid[arr_idx] = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                        "TRUNKt_UC_MEMBER_PORT_SYSTEMf[%d] = %d.\n"),
                                arr_idx, (uint16_t) fval));
                break;
            case TRUNKt_NONUC_MEMBER_CNTf:
                entry->nonuc_cnt = (uint16_t) fval;
                entry->nonuc_cnt_valid = (uint16_t) TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                          "TRUNKt_NONUC_MEMBER_CNTf = %d.\n"),
                    (uint16_t) fval));
                break;
            case TRUNKt_NONUC_MEMBER_PORT_SYSTEMf:
                if ((arr_idx) >= BCMCTH_TRUNK_MAX_MEMBERS) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                           "TRUNKt_NONUC_MEMBER_PORT[%d] idx invalid.\n"),
                                    arr_idx));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                entry->nonuc_system_port[arr_idx] = (uint16_t)  fval;
                entry->nonuc_system_port_valid[arr_idx] = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                            "TRUNKt_NONUC_MEMBER_PORTf[%d] = %d.\n"),
                                arr_idx, (uint16_t) fval));
                break;
            case TRUNKt_MEMBER_L2_EIFf:
                if ((arr_idx) >= BCMCTH_TRUNK_MAX_MEMBERS) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                             "TRUNKt_MEMBER_L2_OIF[%d] idx invalid.\n"),
                                    arr_idx));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                entry->l2_oif[arr_idx] = (uint32_t)  fval;
                entry->l2_oif_valid[arr_idx] = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                              "TRUNKt_MEMBER_L2_OIFf[%d] = %d.\n"),
                                arr_idx, (uint32_t) fval));
                break;
            case TRUNKt_NONUC_EGR_BLOCKf:
                 if ((arr_idx) >= BCMCTH_TRUNK_MAX_MEMBERS) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                             "TRUNKt_NONUC_EGR_BLOCKf[%d] idx invalid.\n"),
                                    arr_idx));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                entry->nonuc_egr_block[arr_idx] = (uint32_t)  fval;
                entry->nonuc_egr_block_valid[arr_idx] = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                              "TRUNKt_NONUC_EGR_BLOCKf[%d] = %d.\n"),
                                arr_idx, (uint32_t) fval));
                break;

            case TRUNKt_UC_EGR_BLOCKf:
                if ((arr_idx) >= BCMCTH_TRUNK_MAX_MEMBERS) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                             "TRUNKt_UC_EGR_BLOCKf[%d] idx invalid.\n"),
                                    arr_idx));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                entry->uc_egr_block[arr_idx] = (uint32_t)  fval;
                entry->uc_egr_block_valid[arr_idx] = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                              "TRUNKt_UC_EGR_BLOCKf[%d] = %d.\n"),
                                arr_idx, (uint32_t) fval));
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
                break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TRUNK transaction validate.
 *
 * \param [in] unit Unit number.
 * \param [in] sid LTD Symbol ID.
 * \param [in] event_reason Reason code.
 * \param [in] key List of the key fields.
 * \param [in] data List of the data fields.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
bcmcth_trunk_imm_proc_grp_validate(int unit,
                                   bcmltd_sid_t sid,
                                   bcmimm_entry_event_t event_reason,
                                   const bcmltd_field_t *key,
                                   const bcmltd_field_t *data,
                                   void *context)
{
    bcmcth_trunk_group_param_t trunk_entry;
    uint32_t trunk_id = 0;
    int uc_cnt;
    int max_members;
    uint32_t max_group_cnt;
    uint32_t num;
    uint64_t def_val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(key, SHR_E_PARAM);

    if (sid != TRUNKt) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    sal_memset(&trunk_entry, 0, sizeof(trunk_entry));

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_trunk_imm_proc_grp_entry_parse(unit,
                                              key,
                                              data,
                                              &trunk_entry));

    max_group_cnt = GRP_TEMP(unit)->max_group_cnt;
    trunk_id = trunk_entry.id;

    if (trunk_id >= max_group_cnt) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "TRUNK_ID=%d invalid.\n"), trunk_id));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }


    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
            if (GRP_EXIST(unit, trunk_id)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_ID=%d exist.\n"), trunk_id));
                SHR_ERR_EXIT(SHR_E_EXISTS);
            }
            if (!trunk_entry.uc_max_members_valid) {
                SHR_IF_ERR_VERBOSE_EXIT
               (bcmlrd_field_default_get(unit,
                                         TRUNKt,
                                         TRUNKt_UC_MAX_MEMBERSf,
                                         1,
                                         &def_val,
                                         &num));
                trunk_entry.uc_max_members = (uint16_t)def_val;
                trunk_entry.uc_max_members_valid = TRUE;
            }
            if (trunk_entry.uc_cnt > trunk_entry.uc_max_members) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit,
                   "TRUNK_ID=%d UC_MEMBER_CNT= %d is greater than" \
                   " UC_MAX_MEMEBERS = %d."),
                   trunk_id, trunk_entry.uc_cnt, trunk_entry.uc_max_members));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            break;
        case BCMIMM_ENTRY_LOOKUP:
            if (!GRP_EXIST(unit, trunk_id)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_ID=%d not found.\n"), trunk_id));
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
            }
            break;
        case BCMIMM_ENTRY_DELETE:
            if (!GRP_EXIST(unit, trunk_id)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "TRUNK_ID=%d not found.\n"), trunk_id));
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
            }
            break;
        case BCMIMM_ENTRY_UPDATE:
            if (!GRP_EXIST(unit, trunk_id)) {
               LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit, "TRUNK_ID=%d not found.\n"), trunk_id));
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
            }

            if (trunk_entry.uc_cnt_valid) {
                uc_cnt = trunk_entry.uc_cnt;
            } else {
                uc_cnt = GRP_UC_MEMBER_CNT(unit, trunk_id);
            }

            if (trunk_entry.uc_max_members_valid) {
                max_members = trunk_entry.uc_max_members;
            } else {
                max_members = GRP_MAX_MEMBERS(unit, trunk_id);
            }
            if (uc_cnt > max_members) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit,
                   "TRUNK_ID=%d UC_MEMBER_CNT= %d is greater than" \
                   " UC_MAX_MEMEBERS = %d."),
                   trunk_id, uc_cnt, max_members));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
            break;
   }

exit:
   SHR_FUNC_EXIT();
}

static int
bcmcth_trunk_find_trunk_id(int unit,
                            bcmcth_trunk_imm_grp_bk_t *bk,
                            uint32_t base_index,
                            uint32_t *trunk_id)
{
    bcmcth_trunk_imm_group_t *grp;
    int group_cnt;
    int group_id;

    SHR_FUNC_ENTER(unit);

    *trunk_id = -1;
    grp = (bcmcth_trunk_imm_group_t *)bk->grp_ptr->group_array;
    group_cnt = bk->max_group_cnt;

    for (group_id = 0; group_id < group_cnt; group_id++) {
        if ((grp->base_ptr == base_index) &&
            (grp->inserted == TRUE)) {
            *trunk_id = group_id;
            SHR_EXIT();
        }
        grp++;
    }

    SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);

exit:
   SHR_FUNC_EXIT();
}

static int
bcmcth_trunk_itbm_defragment(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmltd_sid_t lt_id,
                             uint32_t trunk_id,
                             uint32_t new_mem_max,
                             bcmcth_trunk_imm_grp_bk_t *bk_record,
                             shr_itbm_list_hdl_t itbm_hdl)
{
    shr_itbm_blk_t *itbm_blk = NULL;
    uint32_t itbm_blk_count;
    shr_itbm_defrag_blk_mseq_t *m_seq;
    bcmcth_trunk_grp_defrag_t m_src, m_dest;
    int rv;
    uint32_t i, j;
    bcmcth_trunk_imm_group_t *grp = NULL;
    uint32_t dfg_fblk_count = 0;
    shr_itbm_blk_t *dfg_fblk_arr = NULL;
    shr_itbm_blk_t dfg_lfblk;
    int old_base = -1;
    int new_base = -1;
    uint32_t last_idx = 0;
    bool defrag_start = FALSE;

    SHR_FUNC_ENTER(unit);


    /* Lopate info structure based on trunk_id */
    grp = GRP_HA(unit)->grp_ptr->group_array + trunk_id;
    old_base = grp->base_ptr;

    /*
     * Initialize group member start index to the table Min index value as we have
     * to start the compression operation from the first (Min) index of the
     * table.
     */
    shr_itbm_blk_t_init(&dfg_lfblk);
    SHR_IF_ERR_EXIT
        (shr_itbm_list_defrag_start(itbm_hdl,
                                    &itbm_blk_count,
                                    &itbm_blk,
                                    &m_seq,
                                    &dfg_fblk_count,
                                    &dfg_fblk_arr,
                                    &dfg_lfblk));

    if (dfg_fblk_count) {
        /*
         * Free block size available.
         * 1. Check for the largest lock if availble for the
         * requested resources.
         * 2. Check if the free block sie meets the criteria for
         * resize.
         */
        if (dfg_lfblk.ecount  >= new_mem_max) {
            defrag_start = TRUE;
        } else {
            for (i = 0; i < itbm_blk_count; i++) {
                if (old_base != (int)itbm_blk[i].first_elem) {
                    continue;
                }
                last_idx = (int) (m_seq[i].mcount - 1);
                new_base = m_seq[i].first_elem[last_idx];
                for (j = 0; j < dfg_fblk_count; j++) {
                    if (dfg_fblk_arr[j].first_elem ==
                       new_base + itbm_blk[i].ecount) {
                        if ((dfg_fblk_arr[j].ecount +
                             itbm_blk[i].ecount) >= new_mem_max) {
                            defrag_start = TRUE;
                        }
                        break;
                    }
                }
            }
        }
    }

    if (defrag_start == FALSE) {
        LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit,
        "TRUNK GROUP_ID=%d cannot be inserted due to lack of resources."),
        trunk_id));
        SHR_IF_ERR_EXIT(shr_itbm_list_defrag_end(itbm_hdl, false));
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    for (i = 0; i < itbm_blk_count; ++i) {
        /* itbm_blk gives source information */
        m_src.group_base = itbm_blk[i].first_elem;
        m_src.group_size = itbm_blk[i].ecount;

        /* Needs to retrive group ID based on source index */
        rv = bcmcth_trunk_find_trunk_id(
                              unit,
                              bk_record,
                              m_src.group_base,
                              &m_src.trunk_id);

        if (rv == SHR_E_NOT_FOUND) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Trunk ITBM trunk id not found")));

            SHR_ERR_EXIT(rv);
        }

        for (j = 0; j < (m_seq[i].mcount); ++j) {
            /* m_seq gives move to - destination information */
            m_dest.group_base = m_seq[i].first_elem[j];
            m_dest.group_size = m_seq[i].ecount;

            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Trunk ITBM defragment oper - blk[%d] seq[%d] - tid: %4d, "
                                    "src_base: %4d, src_size: %d, dest_base: %4d, dest_size: %4d \n"),
                                    i, j, m_src.trunk_id,
                                    m_src.group_base,
                                    m_src.group_size,
                                    m_dest.group_base,
                                    m_dest.group_size));

            rv = bcmcth_trunk_drv_imm_grp_move(unit,
                                               op_arg,
                                               lt_id,
                                               bk_record,
                                               &m_src,
                                               &m_dest);

            if (rv != SHR_E_NONE) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Trunk ITBM defragmentation HW move error: %d "), rv));
                SHR_ERR_EXIT(rv);
            }

            m_src.group_base = m_dest.group_base;
            m_src.group_size = m_dest.group_size;
        }

        /* Mark that this trunk group is moved */
        SHR_BITSET(GRP_DIFF(unit), m_src.trunk_id);
    }

    SHR_IF_ERR_EXIT(shr_itbm_list_defrag_end(itbm_hdl, true));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_trunk_itbm_base_allocate(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                bcmltd_sid_t lt_id,
                                uint32_t trunk_id,
                                bcmcth_trunk_imm_grp_bk_t *bk_record,
                                uint32_t new_mem_max,
                                uint32_t *new_base)
{
    int rv;
    uint32_t bucket = SHR_ITBM_INVALID;
    uint32_t itbm_new_base;
    shr_itbm_list_hdl_t itbm_hdl;
    bcmcth_trunk_imm_group_t *grp = NULL;
    uint32_t old_max = 0;
    int old_base = -1;

    SHR_FUNC_ENTER(unit);

    grp = GRP_HA(unit)->grp_ptr->group_array + trunk_id;
    old_base = grp->base_ptr;
    old_max = grp->uc_max_members;
    itbm_hdl = bk_record->itbm_hdl;

    /* Try to allocate new blocks */
    rv = shr_itbm_list_block_alloc(itbm_hdl,
                                   new_mem_max,
                                   &bucket,
                                   &itbm_new_base);
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Trunk ITBM base allocate rv=%d new_base %d.\n"),
                            rv, itbm_new_base));

    /* Need defragmentation. */
    if ((SHR_E_FULL == rv) || (SHR_E_RESOURCE == rv)) {

        /* Defragment the member table and retry for index allocation. */
        SHR_IF_ERR_EXIT(bcmcth_trunk_itbm_defragment(unit,
                                                     op_arg,
                                                     lt_id,
                                                     trunk_id,
                                                     new_mem_max,
                                                     bk_record,
                                                     itbm_hdl));

        rv = shr_itbm_list_block_resize(itbm_hdl,
                                        old_max,
                                        bucket,
                                        old_base,
                                        new_mem_max);
        if (rv == SHR_E_NONE) {
            *new_base = old_base;
            SHR_ERR_EXIT(rv);
        } else {
            /* Try to allocate again after defragmentation */
            rv = shr_itbm_list_block_alloc(
                                       itbm_hdl,
                                       new_mem_max,
                                       &bucket,
                                       &itbm_new_base);
        }
    }

    if (rv == SHR_E_NONE) {
        *new_base = itbm_new_base;
    } else {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

exit:

    SHR_FUNC_EXIT();
}

static int
bcmcth_trunk_itbm_base_update(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmltd_sid_t lt_id,
                              bcmcth_trunk_imm_grp_bk_t *bk,
                              bcmcth_trunk_group_param_t *param,
                              uint32_t *new_base)
{
    int rv;
    shr_itbm_list_hdl_t itbm_hdl;
    bcmcth_trunk_imm_group_t *grp;
    uint32_t old_base, old_max, new_max;

    SHR_FUNC_ENTER(unit);

    /* Lopate info structure based on trunk_id */
    grp = bk->grp_ptr->group_array + param->id;
    old_base = grp->base_ptr;
    old_max  = grp->uc_max_members;
    *new_base = old_base;

    itbm_hdl = bk->itbm_hdl;


    if (param->uc_max_members_valid) {
        new_max  = param->uc_max_members;
    } else {
        new_max = grp->uc_max_members;
    }

    /* If member max is the same, return */
    if (new_max == old_max) {
        SHR_EXIT();
    }

    /* Run resize first which will handle two cases:
     * 1. Same base with shrinked member count - pass for sure
     * 2. Same base with Slightly increased member count, pass based on gap size
     *    between blocks.
     */
    rv = shr_itbm_list_block_resize(itbm_hdl,
                                    old_max,
                                    SHR_ITBM_INVALID,
                                    old_base,
                                    new_max);

    if (rv != SHR_E_NONE) {
        /* Now we have to look for a new base and do defragmentation if needed */
        rv = bcmcth_trunk_itbm_base_allocate(unit,
                                             op_arg,
                                             lt_id,
                                             param->id,
                                             bk,
                                             new_max,
                                             new_base);
        if (rv == SHR_E_NONE) {
            /*
             * Check for rebase and do not free resources
             * incase.
             */
            if (*new_base != old_base) {
                /* Get new base. Free old one. */
                rv = shr_itbm_list_block_free(itbm_hdl,
                                              old_max,
                                              SHR_ITBM_INVALID,
                                              old_base);
                if (rv != SHR_E_NONE) {
                    LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Trunk ITBM list block free failure")));

                    SHR_ERR_EXIT(rv);
                }
            }
        } else {
            SHR_ERR_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_trunk_itbm_delete_block(int unit,
                               bcmcth_trunk_imm_grp_bk_t *bk,
                               bcmcth_trunk_group_param_t *param)
{
    bcmcth_trunk_imm_group_t *grp;

    SHR_FUNC_ENTER(unit);

    /* Locate info structure based on trunk_id */
    grp = bk->grp_ptr->group_array + param->id;

    /* Delete current ITBM block */
    SHR_IF_ERR_EXIT(shr_itbm_list_block_free(bk->itbm_hdl,
                                             grp->uc_max_members,
                                             SHR_ITBM_INVALID,
                                             grp->base_ptr));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_trunk_imm_proc_grp_update(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 bcmltd_sid_t sid,
                                 const bcmltd_field_t *key,
                                 const bcmltd_field_t *data,
                                 bcmimm_entry_event_t event_reason)
{
    bcmcth_trunk_group_param_t *trunk_entry = NULL;
    int trunk_id = 0;
    int i = 0;
    uint32_t *val32 = NULL;
    uint16_t *val = NULL;
    uint32_t num;
    uint64_t def_val;
    uint32_t new_base = -1;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(key, SHR_E_PARAM);

    if (sid != TRUNKt) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    trunk_entry = sal_alloc(sizeof(bcmcth_trunk_group_param_t), "bcmcthTrunkCfg");
    SHR_NULL_CHECK(trunk_entry, SHR_E_MEMORY);
    sal_memset(trunk_entry, 0, sizeof(bcmcth_trunk_group_param_t));

    SHR_IF_ERR_EXIT(
        bcmcth_trunk_imm_proc_grp_entry_parse(unit,
                                              key,
                                              data,
                                              trunk_entry));

    trunk_id = trunk_entry->id;
    if (event_reason == BCMIMM_ENTRY_UPDATE) {
        if (!trunk_entry->uc_cnt_valid) {
            trunk_entry->uc_cnt = GRP_UC_MEMBER_CNT(unit, trunk_id);
            trunk_entry->uc_cnt_valid = TRUE;
        }

        if (!trunk_entry->nonuc_cnt_valid) {
            trunk_entry->nonuc_cnt = GRP_NONUC_MEMBER_CNT(unit, trunk_id);
            trunk_entry->nonuc_cnt_valid = TRUE;
        }

        for (i = 0; i < trunk_entry->nonuc_cnt; i++) {
            if (!trunk_entry->nonuc_system_port_valid[i]) {
                val = (uint16_t *) BCMCTH_TRUNK_GRP_NONUC_MEMBER_SYSTEM_PORT(
                                   unit, trunk_id);
                trunk_entry->nonuc_system_port[i] = *(val + i);
                if (trunk_entry->nonuc_system_port[i]) {
                    /* Set valid only for non zero values. */
                    trunk_entry->nonuc_system_port_valid[i] = TRUE;
                }
            }
        }

        for (i = 0; i < trunk_entry->uc_cnt; i++) {
             if (!trunk_entry->uc_system_port_valid[i]) {
                val = (uint16_t *) BCMCTH_TRUNK_GRP_UC_MEMBER_SYSTEM_PORT(
                                   unit, trunk_id);
                trunk_entry->uc_system_port[i] = *(val + i);
                if (trunk_entry->uc_system_port[i]) {
                    /* Set valid only for non zero values. */
                    trunk_entry->uc_system_port_valid[i] = TRUE;
                }
            }
        }

          for (i = 0; i < trunk_entry->uc_cnt; i++) {
             if (!trunk_entry->l2_oif_valid[i]) {
                val = (uint16_t *) BCMCTH_TRUNK_GRP_L2_EIF(
                                   unit, trunk_id);
                trunk_entry->l2_oif[i] = *(val + i);
                if (trunk_entry->l2_oif[i]) {
                    /* Set valid only for non zero values. */
                    trunk_entry->l2_oif_valid[i] = TRUE;
                }
            }
        }

        for (i = 0; i < trunk_entry->uc_cnt; i++) {
             if (!trunk_entry->uc_egr_block_valid[i]) {
                val32 = (uint32_t *) BCMCTH_TRUNK_GRP_UC_EGRESS_DISABLE(
                                   unit, trunk_id);
                trunk_entry->uc_egr_block[i] =
                SHR_BITGET(val32, i);
                if (trunk_entry->uc_egr_block[i]) {
                    /* Set valid only for non zero values. */
                    trunk_entry->uc_egr_block_valid[i] = TRUE;
                }
            }
        }
        for (i = 0; i < trunk_entry->nonuc_cnt; i++) {
             if (!trunk_entry->nonuc_egr_block_valid[i]) {
                val32 = (uint32_t *) BCMCTH_TRUNK_GRP_NONUC_EGRESS_DISABLE(
                                   unit, trunk_id);
                trunk_entry->nonuc_egr_block[i] =
                SHR_BITGET(val32, i);
                if (trunk_entry->nonuc_egr_block[i]) {
                    /* Set valid only for non zero values. */
                    trunk_entry->nonuc_egr_block_valid[i] = TRUE;
                }
            }
        }

        if (!trunk_entry->uc_max_members_valid) {
            trunk_entry->uc_max_members =  GRP_MAX_MEMBERS(unit, trunk_id);
            trunk_entry->uc_max_members_valid = TRUE;
        }

        if (GRP_TEMP(unit)->itbm_support) {
            SHR_IF_ERR_EXIT(
                bcmcth_trunk_itbm_base_update(unit,
                                              op_arg,
                                              TRUNKt,
                                              GRP_TEMP(unit),
                                              trunk_entry,
                                              &new_base));
        }

        SHR_IF_ERR_EXIT(
            bcmcth_trunk_drv_imm_grp_update(unit,
                                            op_arg,
                                            TRUNKt,
                                            GRP_TEMP(unit),
                                            trunk_entry,
                                            new_base));

    } else if (event_reason == BCMIMM_ENTRY_INSERT) {
        if (!trunk_entry->uc_max_members_valid) {
            SHR_IF_ERR_VERBOSE_EXIT
               (bcmlrd_field_default_get(unit,
                                         TRUNKt,
                                         TRUNKt_UC_MAX_MEMBERSf,
                                         1,
                                         &def_val,
                                         &num));
            trunk_entry->uc_max_members = (uint16_t)def_val;
            trunk_entry->uc_max_members_valid = TRUE;
        }

        if (GRP_TEMP(unit)->itbm_support) {
            SHR_IF_ERR_EXIT
                (bcmcth_trunk_itbm_base_allocate(unit,
                                                 op_arg,
                                                 TRUNKt,
                                                 trunk_id,
                                                 GRP_TEMP(unit),
                                                 trunk_entry->uc_max_members,
                                                 &new_base));
        }

        SHR_IF_ERR_EXIT(
            bcmcth_trunk_drv_imm_grp_add(unit,
                                         op_arg,
                                         TRUNKt,
                                         GRP_TEMP(unit),
                                         trunk_entry,
                                         new_base));
    } else {
       SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    BCMCTH_TRUNK_GRP_INSERTED_SET(unit, trunk_id, TRUE );
    if (trunk_entry->uc_cnt_valid) {
       BCMCTH_TRUNK_GRP_UC_MEMBER_CNT_SET(unit,
                                          trunk_id,
                                          trunk_entry->uc_cnt);
    }

    if (trunk_entry->uc_max_members_valid ) {
        (BCMCTH_TRUNK_GRP_MAX_MEMBERS_SET(unit,
                                          trunk_id,
                                          trunk_entry->uc_max_members));
    }

    if (trunk_entry->nonuc_cnt_valid) {
        (BCMCTH_TRUNK_GRP_NONUC_MEMBER_CNT_SET(unit,
                                               trunk_id,
                                               trunk_entry->nonuc_cnt));
    }
    for (i = 0; i < trunk_entry->nonuc_cnt; i++) {
        if (trunk_entry->nonuc_system_port_valid[i]) {
            val = (uint16_t *) BCMCTH_TRUNK_GRP_NONUC_MEMBER_SYSTEM_PORT(
                               unit, trunk_id);
            *(val + i) = trunk_entry->nonuc_system_port[i];
        }
    }

    for (i = 0; i < trunk_entry->uc_cnt; i++) {
        if (trunk_entry->uc_system_port_valid[i]) {
            val = (uint16_t *) BCMCTH_TRUNK_GRP_UC_MEMBER_SYSTEM_PORT(
                               unit, trunk_id);
            *(val + i) = trunk_entry->uc_system_port[i];
        }
    }
    for (i = 0; i < trunk_entry->uc_cnt; i++) {
        if (trunk_entry->uc_egr_block_valid[i]) {
            val32 = (uint32_t *) BCMCTH_TRUNK_GRP_UC_EGRESS_DISABLE(
                               unit, trunk_id);
            if (trunk_entry->uc_egr_block[i]) {
                SHR_BITSET(val32, i);
            } else {
                SHR_BITCLR(val32, i);
            }
        }
    }
    for (i = 0; i < trunk_entry->uc_cnt; i++) {
        if (trunk_entry->l2_oif_valid[i]) {
            val = (uint16_t *) BCMCTH_TRUNK_GRP_L2_EIF(
                               unit, trunk_id);
            *(val + i) = trunk_entry->l2_oif[i];
        }
    }
    for (i = 0; i < trunk_entry->nonuc_cnt; i++) {
        if (trunk_entry->nonuc_egr_block_valid[i]) {
            val32 = (uint32_t *) BCMCTH_TRUNK_GRP_NONUC_EGRESS_DISABLE(
                               unit, trunk_id);
            if (trunk_entry->nonuc_egr_block[i]) {
                SHR_BITSET(val32, i);
            } else {
                SHR_BITCLR(val32, i);
            }
        }
    }
    /* Set in use group difference flag for trunk_id. */
    SHR_BITSET(GRP_DIFF(unit), trunk_id);
exit:

    if (trunk_entry) {
        sal_free(trunk_entry);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief TRUNK imm commit call back
 *
 * Discard the backup state as the current
 * transaction is getting committed.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
bcmcth_trunk_imm_proc_grp_commit(int unit,
                                 bcmltd_sid_t sid,
                                 uint32_t trans_id,
                                 void *context)
{
    uint32_t idx;
    uint32_t size = SHR_BITALLOCSIZE(BCMCTH_TRUNK_GRP_MAX(unit));

    SHR_FUNC_ENTER(unit);

    if (!GRP_HA(unit)) {
        SHR_EXIT();
    }

    for (idx = 0; idx < BCMCTH_TRUNK_GRP_MAX(unit); idx++) {
        if (SHR_BITGET(GRP_DIFF(unit), idx)) {

            GRP_HA(unit)->grp_ptr->group_array[idx] =
            GRP_TEMP(unit)->grp_ptr->group_array[idx];
        }
    }
    sal_memcpy(GRP_HA(unit)->mbmp_ptr->mbmp,
               GRP_TEMP(unit)->mbmp_ptr->mbmp,
               SHR_BITALLOCSIZE(GRP_TEMP(unit)->max_member_cnt));

    sal_memset(GRP_DIFF(unit), 0, size);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TRUNK transaction abort
 *
 * Rollback the DLB Trunk entry from backup state
 * as current transaction is getting aborted.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval None.
 */
static void
bcmcth_trunk_imm_proc_grp_abort(int unit,
                                bcmltd_sid_t sid,
                                uint32_t trans_id,
                                void *context)
{
    uint32_t idx;
    uint32_t size = SHR_BITALLOCSIZE(BCMCTH_TRUNK_GRP_MAX(unit));
    shr_itbm_list_hdl_t itbm_hdl;
    uint32_t base_index;
    uint32_t grp_cnt;
    int rv;


    if (!GRP_HA(unit)) {
        return;
    }

    /* Iterate each trunk_id and release working copy resource. */
    for (idx = 0; idx < BCMCTH_TRUNK_GRP_MAX(unit); idx++) {
        if (SHR_BITGET(GRP_DIFF(unit), idx)) {
            if (GRP_TEMP(unit)->itbm_support == TRUE) {
                if (GRP_TEMP(unit)->grp_ptr->group_array[idx].inserted == TRUE) {
                    itbm_hdl = GRP_TEMP(unit)->itbm_hdl;
                    base_index =  GRP_TEMP(unit)->grp_ptr->group_array[idx].base_ptr;
                    grp_cnt = GRP_TEMP(unit)->grp_ptr->group_array[idx].uc_max_members;
                    rv = shr_itbm_list_block_free(itbm_hdl,
                                                  grp_cnt,
                                                  SHR_ITBM_INVALID,
                                                  base_index);
                    if (rv != SHR_E_NONE) {
                        LOG_ERROR(BSL_LOG_MODULE,
                                  (BSL_META_U(unit, "Trunk ITBM release resource database error.")));
                        return;
                    }
                }
            }
        }
    }

    /* Iterate each trunk_id and restore from master copy resource. */
    for (idx = 0; idx < BCMCTH_TRUNK_GRP_MAX(unit); idx++) {
        if (SHR_BITGET(GRP_DIFF(unit), idx)) {
            GRP_TEMP(unit)->grp_ptr->group_array[idx] =
            GRP_HA(unit)->grp_ptr->group_array[idx];

            if (GRP_TEMP(unit)->itbm_support == TRUE) {
                if (GRP_HA(unit)->grp_ptr->group_array[idx].inserted == TRUE) {
                    itbm_hdl = GRP_HA(unit)->itbm_hdl;
                    base_index =  GRP_HA(unit)->grp_ptr->group_array[idx].base_ptr;
                    grp_cnt = GRP_HA(unit)->grp_ptr->group_array[idx].uc_max_members;
                    rv = shr_itbm_list_block_alloc_id(itbm_hdl,
                                                      grp_cnt,
                                                      SHR_ITBM_INVALID,
                                                      base_index);
                    if (rv != SHR_E_NONE) {
                        LOG_ERROR(BSL_LOG_MODULE,
                                  (BSL_META_U(unit, "Trunk ITBM rebuild resource database error.")));
                        return;
                    }
                }
            }
        }
    }

    sal_memcpy(GRP_TEMP(unit)->mbmp_ptr->mbmp, GRP_HA(unit)->mbmp_ptr->mbmp,
               SHR_BITALLOCSIZE(GRP_TEMP(unit)->max_member_cnt));
    sal_memset(GRP_DIFF(unit), 0, size);

    return;
}

/*!
 * \brief TRUNK LT entry delete
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Operation arguments.
 * \param [in] sid LTD Symbol ID.
 * \param [in] key List of the key fields.
 * \param [in] data List of the data fields.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
bcmcth_trunk_imm_proc_grp_delete(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 bcmltd_sid_t sid,
                                 const bcmltd_field_t *key,
                                 const bcmltd_field_t *data)
{
    bcmcth_trunk_group_param_t trunk_entry;
    int trunk_id = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(key, SHR_E_PARAM);

    if (sid != TRUNKt) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    sal_memset(&trunk_entry, 0, sizeof(trunk_entry));

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_trunk_imm_proc_grp_entry_parse(unit,
                                              key,
                                              data,
                                              &trunk_entry));

    if (GRP_TEMP(unit)->itbm_support) {
        SHR_IF_ERR_EXIT(
            bcmcth_trunk_itbm_delete_block(unit,
                                           GRP_TEMP(unit),
                                           &trunk_entry));
    }

   SHR_IF_ERR_EXIT(
        bcmcth_trunk_drv_imm_grp_del(unit,
                                     op_arg,
                                     TRUNKt,
                                     GRP_TEMP(unit),
                                     &trunk_entry));

   trunk_id = trunk_entry.id;
   sal_memset(BCMCTH_TRUNK_GRP(unit, trunk_id), 0,
              sizeof(bcmcth_trunk_imm_group_t));
   BCMCTH_TRUNK_GRP_INSERTED_SET(unit, trunk_id, FALSE);
   /* Set in use group difference flag for trunk_id. */
   SHR_BITSET(GRP_DIFF(unit), trunk_id);
exit:
   SHR_FUNC_EXIT();

}
/*!
 * \brief TRUNK table change callback function for staging.
 *
 * Handle TRUNK IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] op_arg Operation arguments.
 * \param [in] event_reason Call back reason.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 * \param [out] output_fields This is a linked list of fields,
 * can be used by the component to add fields into the entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
bcmcth_trunk_imm_proc_grp_stage(int unit,
                                bcmltd_sid_t sid,
                                const bcmltd_op_arg_t *op_arg,
                                bcmimm_entry_event_t event_reason,
                                const bcmltd_field_t *key,
                                const bcmltd_field_t *data,
                                void *context,
                                bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }
    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT(bcmcth_trunk_imm_proc_grp_update(
                                                unit,
                                                op_arg,
                                                sid,
                                                key,
                                                data,
                                                event_reason));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT(bcmcth_trunk_imm_proc_grp_delete(
                                                unit,
                                                op_arg,
                                                sid,
                                                key,
                                                data));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}
/*!
 * \brief TRUNK In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to TRUNK logical table entry commit stages.
 */
static bcmimm_lt_cb_t bcmcth_trunk_imm_grp_callback = {

  /*! Validate function. */
  .validate = bcmcth_trunk_imm_proc_grp_validate,

  /*! Staging function. */
  .op_stage    = bcmcth_trunk_imm_proc_grp_stage,

  /*! Commit function. */
  .commit   = bcmcth_trunk_imm_proc_grp_commit,

  /*! Abort function. */
  .abort    = bcmcth_trunk_imm_proc_grp_abort
};

/*!
 * \brief TRUNK LT imm registration
 *
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to register the imm callbacks.
 */
static int
bcmcth_trunk_imm_grp_register(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmimm_lt_event_reg(unit,
                            TRUNKt,
                            &bcmcth_trunk_imm_grp_callback,
                            NULL));

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */
int
bcmcth_trunk_imm_grp_init(int unit, bool warm)
{
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    uint32_t alloc_size = 0;
    bcmcth_trunk_imm_group_ha_blk_t *grp_ptr = NULL;
    bcmcth_trunk_imm_group_mbmp_ha_blk_t *mbmp_ptr = NULL;
    char     *ptr = NULL;
    uint32_t ref_sign_1 = 0x58880000;
    uint32_t ref_sign_2 = 0x58880001;
    uint32_t size_of_grp_array= 0, size_of_grp = 0;
    uint32_t size_of_grp_mbmp= 0, size_of_mbmp = 0;
    uint32_t idx;
    shr_itbm_list_hdl_t itbm_hdl;
    uint32_t base_index;
    uint32_t grp_cnt;
    int rv;

    SHR_FUNC_ENTER(unit);

    if (!GRP_HA(unit)) {
        alloc_size = sizeof(bcmcth_trunk_imm_grp_bk_t);
        SHR_ALLOC(GRP_HA(unit), alloc_size, "bcmcthTrunkImmGrpBk");
        SHR_NULL_CHECK(GRP_HA(unit), SHR_E_MEMORY);
        sal_memset(GRP_HA(unit), 0, alloc_size);

        SHR_ALLOC(GRP_TEMP(unit), alloc_size, "bcmcthTrunkImmGrpBkTemp");
        SHR_NULL_CHECK(GRP_TEMP(unit), SHR_E_MEMORY);
        sal_memset(GRP_TEMP(unit), 0, alloc_size);

        SHR_IF_ERR_EXIT(bcmcth_trunk_drv_imm_grp_init(unit, GRP_TEMP(unit)));

        GRP_HA(unit)->max_member_cnt = GRP_TEMP(unit)->max_member_cnt;
        GRP_HA(unit)->max_group_cnt = GRP_TEMP(unit)->max_group_cnt;

        /* itbm_hdl is inited from chip init */
        GRP_HA(unit)->itbm_support = GRP_TEMP(unit)->itbm_support;
        GRP_HA(unit)->itbm_hdl = GRP_TEMP(unit)->itbm_hdl;

        size_of_grp_array = (BCMCTH_TRUNK_GRP_MAX(unit) *
                      sizeof(bcmcth_trunk_imm_group_t));

        size_of_grp_mbmp = SHR_BITALLOCSIZE(
                         GRP_TEMP(unit)->max_member_cnt);
        size_of_grp = sizeof(bcmcth_trunk_imm_group_ha_blk_t) +
                      size_of_grp_array;
        size_of_mbmp = sizeof(bcmcth_trunk_imm_group_mbmp_ha_blk_t) +
                      size_of_grp_mbmp;

        /* HA allocation for group. */
        ha_req_size = size_of_grp;

        ha_alloc_size = ha_req_size;

        ptr = shr_ha_mem_alloc(unit,
                               BCMMGMT_TRUNK_COMP_ID,
                               BCMCTH_TRUNK_GRP_SUB_COMP_ID,
                               "bcmcthTrunkGrpHaInfo",
                               &ha_alloc_size);

        SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ha_req_size) ?
                        SHR_E_MEMORY : SHR_E_NONE);
        SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
        grp_ptr = (bcmcth_trunk_imm_group_ha_blk_t *)ptr;
        SHR_NULL_CHECK(grp_ptr, SHR_E_MEMORY);


        /* HA allocation for group member bitmap. */
        ha_req_size = size_of_mbmp;

        ha_alloc_size = ha_req_size;

        ptr = shr_ha_mem_alloc(unit,
                               BCMMGMT_TRUNK_COMP_ID,
                               BCMCTH_TRUNK_MEMBER_BMP_SUB_COMP_ID,
                               "bcmcthTrunkGrpMbmpHaInfo",
                               &ha_alloc_size);

        SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ha_req_size) ?
                        SHR_E_MEMORY : SHR_E_NONE);
        SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
        mbmp_ptr = (bcmcth_trunk_imm_group_mbmp_ha_blk_t *) ptr;
        SHR_NULL_CHECK(mbmp_ptr, SHR_E_MEMORY);

        GRP_HA(unit)->grp_ptr = grp_ptr;
        GRP_HA(unit)->mbmp_ptr = mbmp_ptr;
        if (!warm) {
            sal_memset(GRP_HA(unit)->grp_ptr, 0, size_of_grp);
            GRP_HA(unit)->grp_ptr->signature = ref_sign_1;
            GRP_HA(unit)->grp_ptr->array_size = BCMCTH_TRUNK_GRP_MAX(unit);
            sal_memset(GRP_HA(unit)->grp_ptr->group_array, 0, size_of_grp_array);

            bcmissu_struct_info_report(unit,
                                       BCMMGMT_TRUNK_COMP_ID,
                                       BCMCTH_TRUNK_GRP_SUB_COMP_ID,
                                       0,
                                       size_of_grp, 1,
                                       BCMCTH_TRUNK_IMM_GROUP_HA_BLK_T_ID);

            sal_memset(GRP_HA(unit)->mbmp_ptr, 0, size_of_mbmp);
            GRP_HA(unit)->mbmp_ptr->signature = ref_sign_2;
            GRP_HA(unit)->mbmp_ptr->array_size = BCMCTH_TRUNK_GRP_MAX(unit);
            sal_memset(GRP_HA(unit)->mbmp_ptr->mbmp, 0, size_of_grp_mbmp);

            bcmissu_struct_info_report(unit,
                                       BCMMGMT_TRUNK_COMP_ID,
                                       BCMCTH_TRUNK_MEMBER_BMP_SUB_COMP_ID,
                                       0,
                                       size_of_mbmp, 1,
                                       BCMCTH_TRUNK_IMM_GROUP_MBMP_HA_BLK_T_ID);

            /*
             * Reserve entry 0 of TRUNK_MEMBERm, as invalid Higig
             * trunk groups have their TRUNK_GROUP table entry's
             * BASE_PTR and TG_SIZE fields set to 0.
             * The default value of entry 0 should be null,
             * so it is not set here.
             */
            SHR_BITSET(GRP_HA(unit)->mbmp_ptr->mbmp, 0);
        } else {
            SHR_IF_ERR_MSG_EXIT
            ((grp_ptr->signature != ref_sign_1
              ? SHR_E_FAIL : SHR_E_NONE),
              (BSL_META_U(unit,
              "\n\t WB trunk group info: Signature mismatch for group info"
              "exp=0x%-8x act=0x%-8x\n"),
               ref_sign_1,
               grp_ptr->signature));
            SHR_IF_ERR_MSG_EXIT
            ((mbmp_ptr->signature != ref_sign_2
              ? SHR_E_FAIL : SHR_E_NONE),
              (BSL_META_U(unit,
               "\n\t WB trunk group mbmp info: Signature mismatch for group mbmp info"
               "/exp=0x%-8x act=0x%-8x\n"),
               ref_sign_2,
               mbmp_ptr->signature));
        }

        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                "\n\t WB trunk group info: Signature=0x%-8x HA addr=0x%p\n"),
                 grp_ptr->signature,
                 (void*)grp_ptr));

        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                "\n\t WB trunk group mbmp info: Signature=0x%-8x HA addr=0x%p\n"),
                 mbmp_ptr->signature,
                 (void*)mbmp_ptr));

        /* Temporary structure for group. */
        SHR_ALLOC(GRP_TEMP(unit)->grp_ptr,
                  size_of_grp,
                  "bcmcthTrunkImmGrpHaBlkTemp");
        /* Temporary structure for member bitmap. */
        SHR_ALLOC(GRP_TEMP(unit)->mbmp_ptr,
                  size_of_mbmp,
                  "bcmcthTrunkImmGrpMbmpHaBlkTemp");
        SHR_NULL_CHECK(GRP_TEMP(unit)->grp_ptr, SHR_E_MEMORY);
        SHR_NULL_CHECK(GRP_TEMP(unit)->grp_ptr->group_array, SHR_E_MEMORY);
        if (!warm) {
            sal_memset(GRP_TEMP(unit)->grp_ptr,
                       0,
                       size_of_grp);
            GRP_TEMP(unit)->grp_ptr->signature = ref_sign_1;
            GRP_TEMP(unit)->grp_ptr->array_size = BCMCTH_TRUNK_GRP_MAX(unit);
        } else {
            sal_memcpy(GRP_TEMP(unit)->grp_ptr,
                       GRP_HA(unit)->grp_ptr,
                       size_of_grp);

            if (TRUE == GRP_HA(unit)->itbm_support) {
                /* Rebuild itbm database for warmboot. */
                for (idx = 0; idx < BCMCTH_TRUNK_GRP_MAX(unit); idx++) {
                    if (GRP_HA(unit)->grp_ptr->group_array[idx].inserted == TRUE) {
                        /* Rebuild ITBM resource database based on master copy */
                        itbm_hdl = GRP_HA(unit)->itbm_hdl;
                        base_index =  GRP_HA(unit)->grp_ptr->group_array[idx].base_ptr;
                        grp_cnt = GRP_HA(unit)->grp_ptr->group_array[idx].uc_max_members;
                        rv = shr_itbm_list_block_alloc_id(itbm_hdl,
                                                          grp_cnt,
                                                          SHR_ITBM_INVALID,
                                                          base_index);
                        if (rv != SHR_E_NONE) {
                            LOG_ERROR(BSL_LOG_MODULE,
                                      (BSL_META_U(unit, "Trunk ITBM warmboot rebuild resource database error.")));
                            SHR_ERR_EXIT(rv);
                        }
                    }
                }
            }

        }

        SHR_NULL_CHECK(GRP_TEMP(unit)->mbmp_ptr, SHR_E_MEMORY);
        SHR_NULL_CHECK(GRP_TEMP(unit)->mbmp_ptr->mbmp, SHR_E_MEMORY);
        if (!warm) {
            sal_memset(GRP_TEMP(unit)->mbmp_ptr, 0, size_of_mbmp);
            GRP_TEMP(unit)->mbmp_ptr->signature = ref_sign_2;
            GRP_TEMP(unit)->mbmp_ptr->array_size = BCMCTH_TRUNK_GRP_MAX(unit);
            SHR_BITSET(GRP_TEMP(unit)->mbmp_ptr->mbmp, 0);
        } else {
            sal_memcpy(GRP_TEMP(unit)->mbmp_ptr,
                                GRP_HA(unit)->mbmp_ptr,
                                size_of_mbmp);
        }

        /* TRUNK bitmap diff in one commit.*/
        alloc_size = SHR_BITALLOCSIZE(BCMCTH_TRUNK_GRP_MAX(unit));
        SHR_ALLOC(GRP_DIFF(unit),
                  alloc_size,
                  "bcmcthTrunkImmGrpBmpDiff");
        SHR_NULL_CHECK(GRP_DIFF(unit), SHR_E_MEMORY);
        sal_memset(GRP_DIFF(unit), 0, alloc_size);
    }

    /* IMM registration. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_trunk_imm_grp_register(unit));

exit:
    if (SHR_FUNC_ERR()) {
        (void) bcmcth_trunk_imm_grp_cleanup(unit, TRUE);
    }
    SHR_FUNC_EXIT();
}

int
bcmcth_trunk_imm_grp_cleanup(int unit, bool ha_free)
{
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    if (GRP_HA(unit) != NULL) {
        if (GRP_HA(unit)->grp_ptr != NULL && GRP_HA(unit)->mbmp_ptr) {
            if (ha_free) {
                bcmissu_struct_info_clear(unit,
                                      BCMCTH_TRUNK_IMM_GROUP_HA_BLK_T_ID,
                                      BCMMGMT_TRUNK_COMP_ID,
                                      BCMCTH_TRUNK_GRP_SUB_COMP_ID);

                bcmissu_struct_info_clear(unit,
                                      BCMCTH_TRUNK_IMM_GROUP_MBMP_HA_BLK_T_ID,
                                      BCMMGMT_TRUNK_COMP_ID,
                                      BCMCTH_TRUNK_MEMBER_BMP_SUB_COMP_ID);
                shr_ha_mem_free(unit, GRP_HA(unit)->grp_ptr);
                shr_ha_mem_free(unit, GRP_HA(unit)->mbmp_ptr);
            }
            SHR_FREE(GRP_HA(unit));
        }
    }

    if (GRP_TEMP(unit) != NULL) {

        /* Iterate each trunk_id and release itbm resource. */
        for (idx = 0; idx < BCMCTH_TRUNK_GRP_MAX(unit); idx++) {
            if (GRP_TEMP(unit)->itbm_support == TRUE) {
                if (GRP_TEMP(unit)->grp_ptr->group_array[idx].inserted == TRUE) {
                    shr_itbm_list_block_free(GRP_TEMP(unit)->itbm_hdl,
                                             GRP_TEMP(unit)->grp_ptr->group_array[idx].uc_max_members,
                                             SHR_ITBM_INVALID,
                                             GRP_TEMP(unit)->grp_ptr->group_array[idx].base_ptr);
                }
            }
        }

        /* Destroy ITBM handle */
        shr_itbm_list_destroy(GRP_TEMP(unit)->itbm_hdl);

        SHR_FREE(GRP_TEMP(unit)->grp_ptr);
        SHR_FREE(GRP_TEMP(unit)->mbmp_ptr);
        GRP_TEMP(unit)->grp_ptr = NULL;
        GRP_TEMP(unit)->mbmp_ptr = NULL;
    }
    SHR_FREE(GRP_TEMP(unit));
    SHR_FREE(GRP_DIFF(unit));

    SHR_FUNC_EXIT();
}



