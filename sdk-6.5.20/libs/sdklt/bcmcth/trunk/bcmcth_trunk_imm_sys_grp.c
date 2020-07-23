/*! \file bcmcth_trunk_imm_sys_grp.c
 *
 * Purpose:     handler implementation for TRUNK_SYSTEM LT.
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

/*******************************************************************************
 * Private variables
 */
/*! TRUNK_SYSTEM bookkeeping HA structure. */
static bcmcth_trunk_sys_grp_bk_t *sys_grp_bk[BCMDRD_CONFIG_MAX_UNITS];

/*! TRUNK_SYSTEM bookkeeping temporary structure. */
static bcmcth_trunk_sys_grp_bk_t * sys_grp_bk_temp[BCMDRD_CONFIG_MAX_UNITS];

/*! TRUNK_SYSTEM group SW bitmap changed in one commit. */
SHR_BITDCL *bcmcth_trunk_sys_grp_bitmap_diff[BCMDRD_CONFIG_MAX_UNITS];

/*! Pointer to trunk group state. */
#define SYS_GRP_HA(unit)                 sys_grp_bk[unit]

/*! Pointer to trunk group temporary state. */
#define SYS_GRP_TEMP(unit)               sys_grp_bk_temp[unit]

/*! Pointer to trunk group temporary state. */
#define SYS_GRP_DIFF(unit)               bcmcth_trunk_sys_grp_bitmap_diff[unit]



#define BCMCTH_TRUNK_SYS_GRP_INFO(_u_) \
    (SYS_GRP_TEMP(_u_)->grp_ptr->sys_grp_array)

#define BCMCTH_TRUNK_SYS_GRP(_u_, _idx_) \
    (&(SYS_GRP_TEMP(_u_)->grp_ptr->sys_grp_array[_idx_]))

#define SYS_GRP_EXIST(_u_, _idx_)  \
    ((BCMCTH_TRUNK_SYS_GRP(_u_, _idx_))->inserted)

#define BCMCTH_TRUNK_SYS_GRP_INSERTED_SET(_u_, _idx_, _val_)  \
    (((BCMCTH_TRUNK_SYS_GRP(_u_, _idx_))->inserted) = _val_)

#define SYS_GRP_MEMBER_CNT(_u_, _idx_)  \
    ((BCMCTH_TRUNK_SYS_GRP(_u_, _idx_))->cnt)

#define BCMCTH_TRUNK_SYS_GRP_MEMBER_CNT_SET(_u_, _idx_, _val_)  \
    (((BCMCTH_TRUNK_SYS_GRP(_u_, _idx_))->cnt) = _val_)

#define SYS_GRP_MAX_MEMBERS(_u_, _idx_)  \
    ((BCMCTH_TRUNK_SYS_GRP(_u_, _idx_))->max_members)

#define BCMCTH_TRUNK_SYS_GRP_MAX_MEMBERS_SET(_u_, _idx_, _val_)  \
    (((BCMCTH_TRUNK_SYS_GRP(_u_, _idx_))->max_members) = _val_)

#define SYS_GRP_BASE_PTR(_u_, _idx_)  \
    ((BCMCTH_TRUNK_SYS_GRP(_u_, _idx_))->base_ptr)

/* BSL Module TBD */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TRUNK
/*******************************************************************************
 * Private functions
 */
/*!
 * \brief IMM input fields parse routine.
 *
 * Parse TRUNK_SYSTEM IMM entry fields and save the data to entry.
 *
 * \param [in]  unit Unit number.
 * \param [in]  key field list.
 * \param [in]  data field list.
 * \param [out] entry TRUNK_SYSTEM LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c in to \c LT entry.
 */
static int
bcmcth_trunk_imm_sys_grp_entry_parse(int unit,
                                     const bcmltd_field_t *key,
                                     const bcmltd_field_t *data,
                                     bcmcth_trunk_sys_grp_param_t *entry)
{
    uint32_t fid;
    uint32_t arr_idx;
    uint64_t key_val, fval;
    const bcmltd_field_t *in = NULL;

    SHR_FUNC_ENTER(unit);

    /* Read the trunk ID */
    fid = TRUNK_SYSTEMt_TRUNK_SYSTEM_IDf;
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_trunk_imm_field_get(unit,
                                               key,
                                               fid,
                                               &key_val));

    entry->id = (uint32_t) key_val;
    LOG_VERBOSE(BSL_LOG_MODULE,
     (BSL_META_U(unit, "TRUNK_SYSTEMt_TRUNK_SYSTEM_IDf = %d.\n"), entry->id));

    /* For each field ID access the IMM table data fields */
    for (in = data; (in != NULL); in = in->next) {
        fid = in->id;
        fval = in->data;
        arr_idx = in->idx;
        switch (fid) {
            case TRUNK_SYSTEMt_LB_MODEf:
                entry->lb_mode = (uint8_t) fval;
                entry->lb_mode_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                  "TRUNK_SYSTEMt_LB_MODEf = %d.\n"),
                 (uint8_t) fval));
                break;
            case TRUNK_SYSTEMt_MAX_MEMBERSf:
               entry->max_members = (uint16_t)fval;
                entry->max_members_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                  "TRUNK_SYSTEMt_UC_MAX_MEMBERSf = %d.\n"),
                 (uint16_t) fval));
                break;
            case TRUNK_SYSTEMt_MEMBER_CNTf:
                entry->cnt = (uint16_t)fval;
                entry->cnt_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                  "TRUNK_SYSTEMt_UC_MEMBER_CNTf = %d.\n"),
                  (uint16_t) fval));
                break;
            case TRUNK_SYSTEMt_MEMBER_PORT_IDf:
                if ((arr_idx) >= BCMCTH_TRUNK_SYSTEM_MAX_MEMBERS) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                    "TRUNK_SYSTEMt_UC_MEMBER_PORT_IDf[%d] index invalid.\n"),
                            arr_idx));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                entry->port[arr_idx] = (uint8_t) fval;
                entry->port_valid[arr_idx] = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                     "TRUNK_SYSTEMt_UC_MEMBER_PORT_IDf[%d] = %d.\n"),
                                arr_idx, (uint8_t) fval));
                break;
            case TRUNK_SYSTEMt_RESOLUTION_DISABLEf:
                entry->disable = (bool) fval;
                entry->disable_valid = TRUE;
                LOG_VERBOSE(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                  "TRUNK_SYSTEMt_RESOLUTION_DISABLEf= %d.\n"),
                  (bool) fval));
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
 * \brief TRUNK_SYSTEM transaction validate.
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
bcmcth_trunk_imm_sys_grp_validate(int unit,
                                  bcmltd_sid_t sid,
                                  bcmimm_entry_event_t event_reason,
                                  const bcmltd_field_t *key,
                                  const bcmltd_field_t *data,
                                  void *context)
{
    bcmcth_trunk_sys_grp_param_t trunk_entry;
    uint32_t trunk_id = 0;
    uint32_t max_group_cnt;
    int cnt;
    int max_members;
    uint32_t num;
    uint64_t def_val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(key, SHR_E_PARAM);

    if (sid != TRUNK_SYSTEMt) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    if (!SYS_GRP_TEMP(unit)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    max_group_cnt = SYS_GRP_TEMP(unit)->max_group_cnt;

    sal_memset(&trunk_entry, 0, sizeof(trunk_entry));

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_trunk_imm_sys_grp_entry_parse(unit,
                                             key,
                                             data,
                                             &trunk_entry));

   trunk_id = trunk_entry.id;
    if (trunk_id >= max_group_cnt) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "TRUNK_SYSTEM_ID=%d invalid.\n"), trunk_id));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
            if (SYS_GRP_EXIST(unit, trunk_id)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                     "TRUNK_SYSTEM_ID=%d exist.\n"), trunk_id));
                SHR_ERR_EXIT(SHR_E_EXISTS);
            }
            if (!trunk_entry.max_members_valid) {
                SHR_IF_ERR_VERBOSE_EXIT
                   (bcmlrd_field_default_get(unit,
                                             TRUNK_SYSTEMt,
                                             TRUNK_SYSTEMt_MAX_MEMBERSf,
                                             1,
                                             &def_val,
                                             &num));
                trunk_entry.max_members = (uint16_t)def_val;
                trunk_entry.max_members_valid = TRUE;
            }
            if (trunk_entry.cnt > trunk_entry.max_members) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            break;
        case BCMIMM_ENTRY_LOOKUP:
            if (!SYS_GRP_EXIST(unit, trunk_id)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                     "TRUNK_SYSTEM_ID=%d not found.\n"), trunk_id));
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
            }
            break;
        case BCMIMM_ENTRY_DELETE:
            if (!SYS_GRP_EXIST(unit, trunk_id)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                     "TRUNK_SYSTEM_ID=%d not found.\n"), trunk_id));
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
            }
            break;
        case BCMIMM_ENTRY_UPDATE:
            if (!SYS_GRP_EXIST(unit, trunk_id)) {
               LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit,
                    "TRUNK_SYSTEM_ID=%d not found.\n"), trunk_id));
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
            }
            if (trunk_entry.cnt_valid) {
                cnt = trunk_entry.cnt;
            } else {
                cnt = SYS_GRP_MEMBER_CNT(unit, trunk_id);;
            }

            if (trunk_entry.max_members_valid) {
                max_members = trunk_entry.max_members;
            } else {
                max_members =SYS_GRP_MAX_MEMBERS(unit, trunk_id);
            }
            if (cnt > max_members) {
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
bcmcth_sys_trunk_find_trunk_id(int unit,
                            bcmcth_trunk_sys_grp_bk_t *bk,
                            uint32_t base_index,
                            uint32_t *sys_trunk_id)
{
    bcmcth_trunk_imm_sys_group_t *grp;
    int group_cnt;
    int group_id;

    SHR_FUNC_ENTER(unit);

    *sys_trunk_id = -1;
    grp = bk->grp_ptr->sys_grp_array;
    group_cnt = bk->max_group_cnt;

    for (group_id = 0; group_id < group_cnt; group_id++) {
        if ((grp->base_ptr == base_index) &&
            (grp->inserted == TRUE)) {
            *sys_trunk_id = group_id;
            SHR_EXIT();
        }
        grp++;
    }

    SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);

exit:
   SHR_FUNC_EXIT();
}

static int
bcmcth_sys_trunk_itbm_defragment(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 bcmltd_sid_t lt_id,
                                 uint32_t trunk_id,
                                 uint32_t new_mem_max,
                                 bcmcth_trunk_sys_grp_bk_t *bk_record,
                                 shr_itbm_list_hdl_t itbm_hdl)
{
    shr_itbm_blk_t *itbm_blk = NULL;
    uint32_t itbm_blk_count;
    shr_itbm_defrag_blk_mseq_t *m_seq;
    bcmcth_trunk_grp_defrag_t m_src, m_dest;
    int rv;
    uint32_t i, j;
    bcmcth_trunk_imm_sys_group_t *grp = NULL;
    uint32_t dfg_fblk_count = 0;
    shr_itbm_blk_t *dfg_fblk_arr = NULL;
    shr_itbm_blk_t dfg_lfblk;
    int old_base = -1;
    int new_base = -1;
    uint32_t last_idx = 0;
    bool defrag_start = FALSE;

    SHR_FUNC_ENTER(unit);
    grp = SYS_GRP_HA(unit)->grp_ptr->sys_grp_array + trunk_id;
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
                if (old_base != (int) itbm_blk[i].first_elem) {
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
        rv = bcmcth_sys_trunk_find_trunk_id(unit, bk_record, m_src.group_base, &m_src.trunk_id);

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

            rv = bcmcth_trunk_drv_sys_grp_move(unit,
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
        SHR_BITSET(SYS_GRP_DIFF(unit), m_src.trunk_id);
    }

    SHR_IF_ERR_EXIT(shr_itbm_list_defrag_end(itbm_hdl, true));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_sys_trunk_itbm_base_allocate(int unit,
                                    const bcmltd_op_arg_t *op_arg,
                                    bcmltd_sid_t lt_id,
                                    uint32_t trunk_id,
                                    bcmcth_trunk_sys_grp_bk_t *bk_record,
                                    uint32_t new_mem_max,
                                    uint32_t *new_base)
{
    int rv;
    uint32_t bucket = SHR_ITBM_INVALID;
    uint32_t itbm_new_base;
    shr_itbm_list_hdl_t itbm_hdl;
    bcmcth_trunk_imm_sys_group_t *grp = NULL;
    uint32_t old_max = 0;
    int old_base = -1;

    SHR_FUNC_ENTER(unit);

    grp = SYS_GRP_HA(unit)->grp_ptr->sys_grp_array + trunk_id;
    old_base = grp->base_ptr;
    old_max = grp->max_members;

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
        SHR_IF_ERR_EXIT(
            bcmcth_sys_trunk_itbm_defragment(unit,
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
bcmcth_sys_trunk_itbm_base_update(int unit,
                                  const bcmltd_op_arg_t *op_arg,
                                  bcmltd_sid_t lt_id,
                                  bcmcth_trunk_sys_grp_bk_t *bk,
                                  bcmcth_trunk_sys_grp_param_t *param,
                                  uint32_t *new_base)
{
    int rv;
    shr_itbm_list_hdl_t itbm_hdl;
    bcmcth_trunk_imm_sys_group_t *grp;
    uint32_t old_base, old_max, new_max;

    SHR_FUNC_ENTER(unit);

    /* Locate info structure based on trunk_id */
    grp = bk->grp_ptr->sys_grp_array + param->id;
    old_base = grp->base_ptr;
    old_max  = grp->max_members;
    *new_base = old_base;

    itbm_hdl = bk->itbm_hdl;


    if (param->max_members_valid) {
        new_max  = param->max_members;
    } else {
        new_max = grp->max_members;
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
        rv = bcmcth_sys_trunk_itbm_base_allocate(unit,
                                                 op_arg,
                                                 lt_id,
                                                 param->id,
                                                 bk,
                                                 new_max,
                                                 new_base);
        if (rv == SHR_E_NONE) {
            /* Get new base. Free old one if not resize. */
            if (old_base != *new_base) {
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
bcmcth_sys_trunk_itbm_delete_block(int unit,
                                   bcmcth_trunk_sys_grp_bk_t *bk,
                                   bcmcth_trunk_sys_grp_param_t *param)
{
    bcmcth_trunk_imm_sys_group_t *grp;

    SHR_FUNC_ENTER(unit);

    /* Locate info structure based on trunk_id */
    grp = bk->grp_ptr->sys_grp_array + param->id;

    /* Delete current ITBM block */
    SHR_IF_ERR_EXIT(shr_itbm_list_block_free(bk->itbm_hdl,
                                             grp->max_members,
                                             SHR_ITBM_INVALID,
                                             grp->base_ptr));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_trunk_imm_sys_grp_update(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                bcmltd_sid_t sid,
                                const bcmltd_field_t *key,
                                const bcmltd_field_t *data,
                                bcmimm_entry_event_t event_reason)
{
    bcmcth_trunk_sys_grp_param_t trunk_entry;
    int trunk_id = 0;
    uint32_t num;
    uint64_t def_val;
    uint32_t new_base = -1;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(key, SHR_E_PARAM);

    if (sid != TRUNK_SYSTEMt) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    sal_memset(&trunk_entry, 0, sizeof(trunk_entry));

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_trunk_imm_sys_grp_entry_parse(unit,
                                             key,
                                             data,
                                             &trunk_entry));

    trunk_id = trunk_entry.id;
    if (event_reason == BCMIMM_ENTRY_UPDATE) {

        if (!trunk_entry.cnt_valid) {
            trunk_entry.cnt = SYS_GRP_MEMBER_CNT(unit, trunk_id);
            trunk_entry.cnt_valid = TRUE;
        }

        if (!trunk_entry.max_members_valid) {
            trunk_entry.max_members = SYS_GRP_MAX_MEMBERS(unit, trunk_id);
            trunk_entry.max_members_valid = TRUE;
        }

        if (SYS_GRP_TEMP(unit)->itbm_support) {
            SHR_IF_ERR_EXIT(
                bcmcth_sys_trunk_itbm_base_update(unit,
                                                  op_arg,
                                                  TRUNK_SYSTEMt,
                                                  SYS_GRP_TEMP(unit),
                                                  &trunk_entry,
                                                  &new_base));
        }

        SHR_IF_ERR_VERBOSE_EXIT(
            bcmcth_trunk_drv_sys_grp_update(unit,
                                            op_arg,
                                            TRUNK_SYSTEMt,
                                            SYS_GRP_TEMP(unit),
                                            &trunk_entry,
                                            new_base));

    } else if (event_reason == BCMIMM_ENTRY_INSERT) {
        if (!trunk_entry.max_members_valid) {
            SHR_IF_ERR_VERBOSE_EXIT
               (bcmlrd_field_default_get(unit,
                                         TRUNK_SYSTEMt,
                                         TRUNK_SYSTEMt_MAX_MEMBERSf,
                                         1,
                                         &def_val,
                                         &num));
            trunk_entry.max_members = (uint16_t)def_val;
            trunk_entry.max_members_valid = TRUE;
        }

        if (SYS_GRP_TEMP(unit)->itbm_support) {
            SHR_IF_ERR_EXIT
                (bcmcth_sys_trunk_itbm_base_allocate(unit,
                                                     op_arg,
                                                     TRUNK_SYSTEMt,
                                                     trunk_id,
                                                     SYS_GRP_TEMP(unit),
                                                     trunk_entry.max_members,
                                                     &new_base));
        }

        SHR_IF_ERR_VERBOSE_EXIT(
            bcmcth_trunk_drv_sys_grp_add(unit,
                                         op_arg,
                                         TRUNK_SYSTEMt,
                                         SYS_GRP_TEMP(unit),
                                         &trunk_entry,
                                         new_base));
    } else {
       SHR_ERR_EXIT(SHR_E_INTERNAL);
    }


    BCMCTH_TRUNK_SYS_GRP_INSERTED_SET(unit, trunk_id, TRUE);

    if (trunk_entry.cnt_valid) {
       (BCMCTH_TRUNK_SYS_GRP_MEMBER_CNT_SET(unit,
                                            trunk_id,
                                            trunk_entry.cnt));
    }

    if (trunk_entry.max_members_valid) {
       (BCMCTH_TRUNK_SYS_GRP_MAX_MEMBERS_SET(unit,
                                             trunk_id,
                                             trunk_entry.max_members));
    }

    /* Set in use group difference flag for system trunk id. */
    SHR_BITSET(SYS_GRP_DIFF(unit), trunk_id);

exit:

    SHR_FUNC_EXIT();

}

/*!
 * \brief TRUNK_SYSTEM imm commit call back
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
bcmcth_trunk_imm_sys_grp_commit(int unit,
                                bcmltd_sid_t sid,
                                uint32_t trans_id,
                                void *context)
{
    uint32_t idx;
    uint32_t max_group_cnt = 0;
    uint32_t size = 0;

    SHR_FUNC_ENTER(unit);

    if (!SYS_GRP_HA(unit)) {
        SHR_EXIT();
    }

    if (!SYS_GRP_TEMP(unit)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    max_group_cnt = SYS_GRP_TEMP(unit)->max_group_cnt;

    size = SHR_BITALLOCSIZE(max_group_cnt);

    for (idx = 0; idx < max_group_cnt; idx++) {
        if (SHR_BITGET(SYS_GRP_DIFF(unit), idx)) {
            SYS_GRP_HA(unit)->grp_ptr->sys_grp_array[idx] =
            SYS_GRP_TEMP(unit)->grp_ptr->sys_grp_array[idx];
        }
    }
    sal_memcpy(SYS_GRP_HA(unit)->mbmp_ptr->mbmp,
               SYS_GRP_TEMP(unit)->mbmp_ptr->mbmp,
               SHR_BITALLOCSIZE(SYS_GRP_TEMP(unit)->max_member_cnt));

    sal_memset(SYS_GRP_DIFF(unit), 0, size);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TRUNK_SYSTEM transaction abort
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
bcmcth_trunk_imm_sys_grp_abort(int unit,
                               bcmltd_sid_t sid,
                               uint32_t trans_id,
                               void *context)
{
    uint32_t idx;
    uint32_t  max_group_cnt = 0;
    uint32_t size = 0;
    int rv = SHR_E_NONE;

    if (!SYS_GRP_HA(unit) || !SYS_GRP_TEMP(unit)) {
        return;
    }

    max_group_cnt = SYS_GRP_TEMP(unit)->max_group_cnt;
    size = SHR_BITALLOCSIZE(max_group_cnt);

    /* Iterate each trunk_id and release working copy resource. */
    for (idx = 0; idx < max_group_cnt; idx++) {
        if (SHR_BITGET(SYS_GRP_DIFF(unit), idx)) {
            if (SYS_GRP_TEMP(unit)->itbm_support == TRUE) {
                if (SYS_GRP_TEMP(unit)->grp_ptr->sys_grp_array[idx].inserted == TRUE) {
                    rv = shr_itbm_list_block_free(
                             SYS_GRP_TEMP(unit)->itbm_hdl,
                             SYS_GRP_TEMP(unit)->grp_ptr->sys_grp_array[idx].max_members,
                             SHR_ITBM_INVALID,
                             SYS_GRP_TEMP(unit)->grp_ptr->sys_grp_array[idx].base_ptr);
                    if (rv != SHR_E_NONE) {
                        LOG_ERROR(BSL_LOG_MODULE,
                                  (BSL_META_U(unit,
                                              "System trunk ITBM release resource error.")));
                        return;
                    }
                }
            }
        }
    }

    /* Iterate each trunk_id and restore from master copy resource. */
    for (idx = 0; idx < max_group_cnt; idx++) {
        if (SHR_BITGET(SYS_GRP_DIFF(unit), idx)) {
            SYS_GRP_TEMP(unit)->grp_ptr->sys_grp_array[idx] =
            SYS_GRP_HA(unit)->grp_ptr->sys_grp_array[idx];

            if (SYS_GRP_TEMP(unit)->itbm_support == TRUE) {
                if (SYS_GRP_HA(unit)->grp_ptr->sys_grp_array[idx].inserted == TRUE) {
                    rv = shr_itbm_list_block_alloc_id(
                             SYS_GRP_HA(unit)->itbm_hdl,
                             SYS_GRP_HA(unit)->grp_ptr->sys_grp_array[idx].max_members,
                             SHR_ITBM_INVALID,
                             SYS_GRP_HA(unit)->grp_ptr->sys_grp_array[idx].base_ptr);
                    if (rv != SHR_E_NONE) {
                        LOG_ERROR(BSL_LOG_MODULE,
                                  (BSL_META_U(unit,
                                              "System trunk ITBM rebuild resource error.")));
                        return;
                    }
                }
            }
        }
    }

    sal_memcpy(SYS_GRP_TEMP(unit)->mbmp_ptr->mbmp,
               SYS_GRP_HA(unit)->mbmp_ptr->mbmp,
               SHR_BITALLOCSIZE(SYS_GRP_TEMP(unit)->max_member_cnt));
    sal_memset(SYS_GRP_DIFF(unit), 0, size);

    return;
}

/*!
 * \brief TRUNK_SYSTEM LT entry delete
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] sid LTD Symbol ID.
 * \param [in] key List of the key fields.
 * \param [in] data List of the data fields.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
bcmcth_trunk_imm_sys_grp_delete(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                bcmltd_sid_t sid,
                                const bcmltd_field_t *key,
                                const bcmltd_field_t *data)
{
    bcmcth_trunk_sys_grp_param_t trunk_entry;
    int trunk_id = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(key, SHR_E_PARAM);

    if (sid != TRUNK_SYSTEMt) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    sal_memset(&trunk_entry, 0, sizeof(trunk_entry));

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_trunk_imm_sys_grp_entry_parse(unit,
                                             key,
                                             data,
                                             &trunk_entry));

    if (SYS_GRP_TEMP(unit)->itbm_support) {
        SHR_IF_ERR_EXIT(
            bcmcth_sys_trunk_itbm_delete_block(unit,
                                               SYS_GRP_TEMP(unit),
                                               &trunk_entry));
    }

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_trunk_drv_sys_grp_del(unit,
                                     op_arg,
                                     TRUNK_SYSTEMt,
                                     SYS_GRP_TEMP(unit),
                                     &trunk_entry));

    trunk_id = trunk_entry.id;
       sal_memset(BCMCTH_TRUNK_SYS_GRP(unit, trunk_id), 0,
              sizeof(bcmcth_trunk_imm_sys_group_t));
    BCMCTH_TRUNK_SYS_GRP_INSERTED_SET(unit, trunk_id, FALSE);
    /* Set in use group difference flag for system trunk id. */
    SHR_BITSET(SYS_GRP_DIFF(unit), trunk_id);
exit:
    SHR_FUNC_EXIT();
}
/*!
 * \brief TRUNK_SYSTEM table change callback function for staging.
 *
 * Handle TRUNK_SYSTEM IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id LT Transaction Id
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
bcmcth_trunk_imm_sys_grp_stage(int unit,
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
            SHR_IF_ERR_VERBOSE_EXIT(bcmcth_trunk_imm_sys_grp_update(
                                                            unit,
                                                            op_arg,
                                                            sid,
                                                            key,
                                                            data,
                                                            event_reason));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT(bcmcth_trunk_imm_sys_grp_delete(
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
 * \brief TRUNK_SYSTEM In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to TRUNK_SYSTEM logical table entry commit stages.
 */
static bcmimm_lt_cb_t bcmcth_trunk_imm_sys_grp_callback = {

  /*! Validate function. */
  .validate = bcmcth_trunk_imm_sys_grp_validate,

  /*! Staging function. */
  .op_stage    = bcmcth_trunk_imm_sys_grp_stage,

  /*! Commit function. */
  .commit   = bcmcth_trunk_imm_sys_grp_commit,

  /*! Abort function. */
  .abort    = bcmcth_trunk_imm_sys_grp_abort
};

/*!
 * \brief TRUNK_SYSTEM LT imm registration
 *
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to register the imm callbacks.
 */
static int
bcmcth_trunk_imm_sys_grp_register(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmimm_lt_event_reg(unit,
                            TRUNK_SYSTEMt,
                            &bcmcth_trunk_imm_sys_grp_callback,
                            NULL));

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcmcth_trunk_imm_sys_grp_init(int unit, bool warm)
{
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    uint32_t alloc_size = 0;
    uint32_t max_group_cnt = 0;
    uint32_t ref_sign_1 = 0x68880000;
    uint32_t ref_sign_2 = 0x68880001;
    char     *ptr = NULL;
    bcmcth_trunk_imm_sys_group_ha_blk_t *grp_ptr = NULL;
    bcmcth_trunk_imm_sys_group_mbmp_ha_blk_t *mbmp_ptr = NULL;
    uint32_t size_of_grp_array= 0, size_of_grp = 0;
    uint32_t size_of_grp_mbmp= 0, size_of_mbmp = 0;
    uint32_t idx;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if (!SYS_GRP_HA(unit)) {
        alloc_size = sizeof(bcmcth_trunk_sys_grp_bk_t);
        SHR_ALLOC(SYS_GRP_HA(unit), alloc_size, "bcmcthTrunkSysGrpBk");
        SHR_NULL_CHECK(SYS_GRP_HA(unit), SHR_E_MEMORY);
        sal_memset(SYS_GRP_HA(unit), 0, alloc_size);

        SHR_ALLOC(SYS_GRP_TEMP(unit), alloc_size, "bcmcthTrunkSysGrpBkTemp");
        SHR_NULL_CHECK(SYS_GRP_TEMP(unit), SHR_E_MEMORY);
        sal_memset(SYS_GRP_TEMP(unit), 0, alloc_size);

        SHR_IF_ERR_VERBOSE_EXIT(
           bcmcth_trunk_drv_sys_grp_init(unit, SYS_GRP_TEMP(unit)));
        SYS_GRP_HA(unit)->max_member_cnt = SYS_GRP_TEMP(unit)->max_member_cnt;
        SYS_GRP_HA(unit)->max_group_cnt = SYS_GRP_TEMP(unit)->max_group_cnt;

        /* Incorporate ITBM info to master copy */
        SYS_GRP_HA(unit)->itbm_support = SYS_GRP_TEMP(unit)->itbm_support;
        SYS_GRP_HA(unit)->itbm_hdl = SYS_GRP_TEMP(unit)->itbm_hdl;

        max_group_cnt = SYS_GRP_TEMP(unit)->max_group_cnt;

        size_of_grp_array = max_group_cnt * sizeof(bcmcth_trunk_imm_sys_group_t);

        size_of_grp_mbmp = SHR_BITALLOCSIZE(
                           SYS_GRP_TEMP(unit)->max_member_cnt);
        size_of_grp = sizeof(bcmcth_trunk_imm_sys_group_ha_blk_t) +
                      size_of_grp_array;
        size_of_mbmp = sizeof(bcmcth_trunk_imm_sys_group_mbmp_ha_blk_t) +
                      size_of_grp_mbmp;
        /* HA allocation for system group. */
        ha_req_size = size_of_grp;
        ha_alloc_size = ha_req_size;
        ptr = shr_ha_mem_alloc(unit,
                               BCMMGMT_TRUNK_COMP_ID,
                               BCMCTH_TRUNK_SYSTEM_GRP_SUB_COMP_ID,
                               "bcmcthTrunkSystemGrpHaInfo",
                               &ha_alloc_size);
        SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ha_req_size) ?
                        SHR_E_MEMORY : SHR_E_NONE);
        SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
        grp_ptr = (bcmcth_trunk_imm_sys_group_ha_blk_t *)ptr;
        SHR_NULL_CHECK(grp_ptr, SHR_E_MEMORY);

        /* HA allocation for system group member bitmap. */
        ha_req_size = size_of_mbmp;
        ha_alloc_size = ha_req_size;
        ptr = shr_ha_mem_alloc(unit,
                               BCMMGMT_TRUNK_COMP_ID,
                               BCMCTH_TRUNK_SYSTEM_MEMBER_BMP_SUB_COMP_ID,
                               "bcmcthTrunkSystemMbmpHaInfo",
                               &ha_alloc_size);
        SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ha_req_size) ?
                        SHR_E_MEMORY : SHR_E_NONE);
        SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
        mbmp_ptr = (bcmcth_trunk_imm_sys_group_mbmp_ha_blk_t *) ptr;
        SHR_NULL_CHECK(mbmp_ptr, SHR_E_MEMORY);

        SYS_GRP_HA(unit)->grp_ptr = grp_ptr;
        SYS_GRP_HA(unit)->mbmp_ptr = mbmp_ptr;
        if (!warm) {
            sal_memset(SYS_GRP_HA(unit)->grp_ptr, 0, size_of_grp);
            SYS_GRP_HA(unit)->grp_ptr->signature = ref_sign_1;
            SYS_GRP_HA(unit)->grp_ptr->array_size = max_group_cnt;
            sal_memset(SYS_GRP_HA(unit)->grp_ptr->sys_grp_array, 0, size_of_grp_array);

            bcmissu_struct_info_report(unit, BCMMGMT_TRUNK_COMP_ID,
                                        BCMCTH_TRUNK_SYSTEM_GRP_SUB_COMP_ID, 0,
                                        size_of_grp, 1,
                                        BCMCTH_TRUNK_IMM_SYS_GROUP_HA_BLK_T_ID);

            bcmissu_struct_info_report(unit, BCMMGMT_TRUNK_COMP_ID,
                                        BCMCTH_TRUNK_SYSTEM_MEMBER_BMP_SUB_COMP_ID, 0,
                                        size_of_mbmp, 1,
                                        BCMCTH_TRUNK_IMM_SYS_GROUP_MBMP_HA_BLK_T_ID);

            sal_memset(SYS_GRP_HA(unit)->mbmp_ptr, 0, size_of_mbmp);
            SYS_GRP_HA(unit)->mbmp_ptr->array_size = max_group_cnt;
            SYS_GRP_HA(unit)->mbmp_ptr->signature = ref_sign_2;
            sal_memset(SYS_GRP_HA(unit)->mbmp_ptr->mbmp, 0, size_of_grp_mbmp);

            /*
             * Reserve entry 0 of system trunk member, as invalid
             * system trunk groups have their TRUNK_SYSTEM_GROUP table entry's
             * BASE_PTR and TG_SIZE fields set to 0.
             * The default value of entry 0 should be null,
             * so it is not set here.
             */
            SHR_BITSET(SYS_GRP_HA(unit)->mbmp_ptr->mbmp, 0);

        } else {
            SHR_IF_ERR_MSG_EXIT
            ((grp_ptr->signature != ref_sign_1
              ? SHR_E_FAIL : SHR_E_NONE),
              (BSL_META_U(unit,
              "\n\t WB system trunk group info: "
              "Signature mismatch for group info"
              "exp=0x%-8x act=0x%-8x\n"),
               ref_sign_1,
               grp_ptr->signature));
            SHR_IF_ERR_MSG_EXIT
            ((mbmp_ptr->signature != ref_sign_2
              ? SHR_E_FAIL : SHR_E_NONE),
              (BSL_META_U(unit,
               "\n\t WB system trunk group mbmp info: "
               "Signature mismatch for group mbmp info"
               "/exp=0x%-8x act=0x%-8x\n"),
               ref_sign_2,
               mbmp_ptr->signature));

        }
        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                "\n\t WB system trunk group info: "
                "Signature=0x%-8x HA addr=0x%p\n"),
                 grp_ptr->signature,
                 (void*)grp_ptr));

        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                "\n\t WB system trunk group mbmp info: "
                "Signature=0x%-8x HA addr=0x%p\n"),
                 mbmp_ptr->signature,
                 (void*)mbmp_ptr));

        SHR_ALLOC(SYS_GRP_TEMP(unit)->grp_ptr,
                  size_of_grp,
                  "bcmcthTrunkImmSysGrpHaBlkTemp");
        SHR_ALLOC(SYS_GRP_TEMP(unit)->mbmp_ptr,
                  size_of_mbmp,
                  "bcmcthTrunkImmSysGrpMbmpHaBlkTemp");
        SHR_NULL_CHECK(SYS_GRP_TEMP(unit)->grp_ptr, SHR_E_MEMORY);
        SHR_NULL_CHECK(SYS_GRP_TEMP(unit)->grp_ptr->sys_grp_array, SHR_E_MEMORY);
        if (!warm) {
            sal_memset(SYS_GRP_TEMP(unit)->grp_ptr,
                       0,
                       size_of_grp);
            SYS_GRP_TEMP(unit)->grp_ptr->signature =
                           ref_sign_1;
            SYS_GRP_TEMP(unit)->grp_ptr->array_size =
                            max_group_cnt;
        } else {
            sal_memcpy(SYS_GRP_TEMP(unit)->grp_ptr,
                       SYS_GRP_HA(unit)->grp_ptr,
                       size_of_grp);
        }

        SHR_NULL_CHECK(SYS_GRP_TEMP(unit)->mbmp_ptr, SHR_E_MEMORY);
        if (!warm) {
            sal_memset(SYS_GRP_TEMP(unit)->mbmp_ptr, 0, size_of_mbmp);
            SYS_GRP_TEMP(unit)->mbmp_ptr->signature =
                       ref_sign_2;
            SYS_GRP_TEMP(unit)->mbmp_ptr->array_size =
                       max_group_cnt;
            SHR_BITSET(SYS_GRP_TEMP(unit)->mbmp_ptr->mbmp, 0);
        } else {
            sal_memcpy(SYS_GRP_TEMP(unit)->mbmp_ptr,
                                SYS_GRP_HA(unit)->mbmp_ptr,
                                size_of_mbmp);

            if (TRUE == SYS_GRP_TEMP(unit)->itbm_support) {
                /* Rebuild itbm database for warmboot. */
                for (idx = 0; idx < SYS_GRP_TEMP(unit)->max_group_cnt; idx++) {
                    if (SYS_GRP_TEMP(unit)->grp_ptr->sys_grp_array[idx].inserted == TRUE) {
                        /* Rebuild ITBM resource database. */
                        rv = shr_itbm_list_block_alloc_id(
                                 SYS_GRP_TEMP(unit)->itbm_hdl,
                                 SYS_GRP_TEMP(unit)->grp_ptr->sys_grp_array[idx].max_members,
                                 SHR_ITBM_INVALID,
                                 SYS_GRP_TEMP(unit)->grp_ptr->sys_grp_array[idx].base_ptr);
                        if (rv != SHR_E_NONE) {
                            LOG_ERROR(BSL_LOG_MODULE,
                                      (BSL_META_U(unit,
                                                  "System trunk warmboot ITBM error.")));
                            SHR_ERR_EXIT(rv);
                        }
                    }
                }
            }

        }
        /* TRUNK_SYSTEM bitmap diff in one commit.*/
        alloc_size = SHR_BITALLOCSIZE(max_group_cnt);
        SHR_ALLOC(SYS_GRP_DIFF(unit), alloc_size, "bcmcthTrunkSysGrpBmpDiff");
        SHR_NULL_CHECK(SYS_GRP_DIFF(unit), SHR_E_MEMORY);
        sal_memset(SYS_GRP_DIFF(unit), 0, alloc_size);
    }

    /* IMM registration. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_trunk_imm_sys_grp_register(unit));

exit:
    if (SHR_FUNC_ERR()) {
        (void) bcmcth_trunk_imm_sys_grp_cleanup(unit, TRUE);
    }
    SHR_FUNC_EXIT();
}

int
bcmcth_trunk_imm_sys_grp_cleanup(int unit, bool ha_free)
{
    uint32_t idx;

    SHR_FUNC_ENTER(unit);
    if (SYS_GRP_HA(unit) != NULL) {
        if (SYS_GRP_HA(unit)->grp_ptr != NULL && SYS_GRP_HA(unit)->mbmp_ptr) {
            if (ha_free) {
                bcmissu_struct_info_clear(
                                      unit,
                                      BCMCTH_TRUNK_IMM_SYS_GROUP_HA_BLK_T_ID,
                                      BCMMGMT_TRUNK_COMP_ID,
                                      BCMCTH_TRUNK_SYSTEM_GRP_SUB_COMP_ID);
                bcmissu_struct_info_clear(
                                      unit,
                                      BCMCTH_TRUNK_IMM_SYS_GROUP_MBMP_HA_BLK_T_ID,
                                      BCMMGMT_TRUNK_COMP_ID,
                                      BCMCTH_TRUNK_SYSTEM_MEMBER_BMP_SUB_COMP_ID);
                shr_ha_mem_free(unit, SYS_GRP_HA(unit)->grp_ptr);
                shr_ha_mem_free(unit, SYS_GRP_HA(unit)->mbmp_ptr);
            }
            SHR_FREE(SYS_GRP_HA(unit));
        }
    }

    if (SYS_GRP_TEMP(unit) != NULL) {
        /* Iterate each trunk_id and release itbm resource. */
        for (idx = 0; idx < SYS_GRP_TEMP(unit)->max_group_cnt; idx++) {
            if (SYS_GRP_TEMP(unit)->itbm_support == TRUE) {
                if (SYS_GRP_TEMP(unit)->grp_ptr->sys_grp_array[idx].inserted == TRUE) {
                    shr_itbm_list_block_free(
                        SYS_GRP_TEMP(unit)->itbm_hdl,
                        SYS_GRP_TEMP(unit)->grp_ptr->sys_grp_array[idx].max_members,
                        SHR_ITBM_INVALID,
                        SYS_GRP_TEMP(unit)->grp_ptr->sys_grp_array[idx].base_ptr);
                }
            }
        }

        /* Destroy ITBM handle */
        shr_itbm_list_destroy(SYS_GRP_TEMP(unit)->itbm_hdl);

        SHR_FREE(SYS_GRP_TEMP(unit)->grp_ptr);
        SHR_FREE(SYS_GRP_TEMP(unit)->mbmp_ptr);
        SYS_GRP_TEMP(unit)->grp_ptr = NULL;
        SYS_GRP_TEMP(unit)->mbmp_ptr = NULL;
    }
    SHR_FREE(SYS_GRP_TEMP(unit));
    SHR_FREE(SYS_GRP_DIFF(unit));
    SHR_FUNC_EXIT();

}



