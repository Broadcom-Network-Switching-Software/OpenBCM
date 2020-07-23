/*! \file bcmptm_rm_hash_req_handler.c
 *
 * Resource Manager for Hash Table
 *
 * This file contains the routines that implement the basic operations of hash
 * resource manager. More specifically, it includes the routines for LOOKUP,
 * INSERT, DELETE operations.
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
#include <sal/sal_alloc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include "bcmptm_rm_hash.h"
#include "bcmptm_rm_hash_lt_ctrl.h"
#include "bcmptm_rm_hash_entry_search.h"
#include "bcmptm_rm_hash_req_handler.h"
#include "bcmptm_rm_hash_state_mgmt.h"
#include "bcmptm_rm_hash_trans_mgmt.h"
#include "bcmptm_rm_hash_utils.h"
#include "bcmptm_rm_hash_reordering.h"
#include "bcmptm_rm_hash_traverse.h"
#include "bcmptm_rm_hash_lmem.h"
#include "bcmptm_rm_hash_table_info.h"
#include "bcmptm_rm_hash_pt_register.h"
#include "bcmptm_rm_hash_hit_mgmt.h"
#include "bcmptm_rm_hash_store.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMHASH

/*******************************************************************************
 * Typedefs
 */
/*
 *! \brief Information about the free entry slot for a mapping instance.
 */
typedef struct rm_hash_inst_entry_info_s {
    /*! \brief Index of the entry. */
    uint32_t index;

    /*! \brief SLB of the entry. */
    uint32_t slb;

    /*! \brief Bank of the enty. */
    uint8_t rbank;

    /*! \brief Location information of the entry. */
    rm_hash_ent_loc_t loc;

    /*! \brief Narrow mode entry offset. */
    uint8_t nm_offset;

    /*! \brief Narrow mode. */
    rm_hash_entry_narrow_mode_t e_nm;

    /*! \brief View info of the entry. */
    const bcmptm_rm_hash_view_info_t *view_info;

    /*! \brief View info of the entry. */
    const bcmptm_rm_hash_key_format_t *key_fmt;
} rm_hash_inst_entry_info_t;

/*
 *! \brief Information about the free entry slot for a mapping.
 */
typedef struct rm_hash_map_entry_info_s {
    /*! \brief Group of the entry. */
    uint8_t map;

    /*! \brief Entry attribute. */
    rm_hash_req_ent_attr_t e_attr;

    /*! \brief Number of formats. */
    uint8_t num_insts;

    /*! \brief Entry info for each format. */
    rm_hash_inst_entry_info_t inst_ei[RM_HASH_MAX_GRP_INST_COUNT];
} rm_hash_map_entry_info_t;

/*******************************************************************************
 * Private variables
 */
/*! Per-unit hash search bucket info object */
static rm_hash_glb_srch_info_t cached_glb_srch_info[BCMDRD_CONFIG_MAX_UNITS];

/*! Per-unit hash search result info object */
static rm_hash_map_srch_result_t cached_map_srch_result[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private Functions
 */
/*!
 * \brief Fill in response information structure.
 *
 * \param [in] unit Unit number.
 * \param [in] e_count Entry count.
 * \param [in] e_idx Index of the entry.
 * \param [in] e_sid SID of the entry.
 * \param [in] ldtype_val Logical data type value.
 * \param [in] pipe Pipe number.
 * \param [out] rsp_info Pointer to bcmptm_rm_hash_rsp_t structure for recording
 *              response info for a hash operation.
 *
 * \retval NONE
 */
static void
rm_hash_rsp_info_set(int unit,
                     uint8_t e_count,
                     uint32_t *e_idx,
                     bcmdrd_sid_t *e_sid,
                     uint16_t ldtype_val,
                     uint8_t pipe,
                     bcmptm_rm_hash_rsp_t *rsp_info)
{
    uint8_t idx;

    rsp_info->rsp_entry_count = e_count;
    for (idx = 0; idx < e_count; idx++) {
        rsp_info->rsp_entry_index[idx] = e_idx[idx];
        rsp_info->rsp_entry_sid[idx] = e_sid[idx];
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "RM hash rsp info: rsp_entry_index = %d, "
                                "rsp_entry_sid = %s.\n"),
                                rsp_info->rsp_entry_index[idx],
                                bcmdrd_pt_sid_to_name(unit, e_sid[idx])));
    }
    rsp_info->rsp_dfield_format_id = ldtype_val;
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "RM hash rsp info: rsp_ldtype = %d.\n"),
                            rsp_info->rsp_dfield_format_id));
    rsp_info->rsp_entry_pipe = pipe;
}

/*!
 * \brief Update the bucket state after an entry is deleted.
 *
 * After a new entry is deleted, the corresponding entry node which records
 * the state of the entry from the resource management perspective will be
 * deleted accordingly.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] pipe Pipe instance.
 * \param [in] inst_si Pointer to rm_hash_inst_srch_info_t structure.
 * \param [in] inst_sr Pointer to rm_hash_inst_srch_result_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_bkt_state_clear(int unit,
                        rm_hash_pt_info_t *pt_info,
                        uint8_t pipe,
                        rm_hash_inst_srch_info_t *inst_si,
                        rm_hash_inst_srch_result_t *inst_sr)
{
    rm_hash_ent_node_t *e_node = NULL;
    uint8_t rbank;
    rm_hash_nme_info_t nme_info;
    bool found = FALSE;
    uint32_t found_idx = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&nme_info, 0, sizeof(nme_info));
    rbank = inst_sr->rbank;
    if (pt_info->bank_in_narrow_mode[rbank] == TRUE) {
        nme_info.e_nm = inst_si->e_nm;
        nme_info.nm_loc = inst_sr->nme_info.nm_loc;
    }
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_node_delete(unit, pt_info, rbank, pipe,
                                        inst_sr->slb, inst_sr->e_loc.bb_bmp,
                                        inst_sr->e_loc.be_bmp,
                                        &nme_info,
                                        &e_node));
    if (inst_si->hash_store_en) {
        bcmptm_rm_hash_store_bin_search(unit,
                                        inst_si->ltid,
                                        inst_si->hash_store_val,
                                        &found,
                                        &found_idx,
                                        NULL);
        assert(found == TRUE);
        if (found) {
           SHR_IF_ERR_EXIT
               (bcmptm_rm_hash_store_entry_delete(unit,
                                                  inst_si->ltid,
                                                  found_idx));
        }
    }
    /*
     * Bucket state and entry node has been updated, corresponding
     * undo operation will be recorded in the undo list.
     */
    if (bcmptm_rm_hash_undo_list_maintained(unit) == TRUE) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_trans_undo_node_insert(unit, pt_info, inst_sr->slb,
                                                   pipe, e_node,
                                                   BCMPTM_RM_HASH_UNDO_ENT_INSERT));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear narrow mode entry.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] enm Entry narrow mode.
 * \param [in] nme_loc Narrow mode entry location.
 * \param [out] narrow_ent_buf Pointer to destination entry buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_nm_entry_clear(int unit,
                       rm_hash_pt_info_t *pt_info,
                       uint8_t rbank,
                       rm_hash_entry_narrow_mode_t e_nm,
                       uint8_t nme_loc,
                       uint32_t *narrow_ent_buf)
{
    const bcmptm_rm_hash_narrow_mode_info_t *nm_info = NULL;
    const bcmlrd_hw_rfield_info_t *field_info = NULL;
    uint16_t width, start_bit, end_bit;
    uint32_t ent[RM_HASH_MAX_NARROW_ENT_WORDS_SIZE] = {0};

    SHR_FUNC_ENTER(unit);

    nm_info = pt_info->nm_info[rbank];
    if ((nm_info == NULL) || (nm_info->nme_field_list == NULL)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    field_info = nm_info->nme_field_list[e_nm];
    width = field_info->field_width;
    start_bit = field_info->field_start_bit[nme_loc];
    end_bit = start_bit + width - 1;
    bcmdrd_field_set(narrow_ent_buf, start_bit, end_bit, ent);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set narrow mode entry.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] enm Entry narrow mode.
 * \param [in] nme_loc Narrow mode entry location.
 * \param [out] narrow_ent_buf Pointer to destination entry buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_nm_entry_set(int unit,
                     rm_hash_pt_info_t *pt_info,
                     uint8_t rbank,
                     rm_hash_entry_narrow_mode_t e_nm,
                     uint8_t nme_loc,
                     uint32_t *src_ent_buf,
                     uint32_t *dst_ent_buf)
{
    const bcmptm_rm_hash_narrow_mode_info_t *nm_info = NULL;
    const bcmlrd_hw_rfield_info_t *field_info = NULL;
    uint16_t width, start_bit, end_bit;
    uint32_t ent[RM_HASH_MAX_NARROW_ENT_WORDS_SIZE] = {0};

    SHR_FUNC_ENTER(unit);

    nm_info = pt_info->nm_info[rbank];
    if ((nm_info == NULL) || (nm_info->nme_field_list == NULL)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    field_info = nm_info->nme_field_list[e_nm];
    width = field_info->field_width;

    start_bit = field_info->field_start_bit[0];
    end_bit = start_bit + width - 1;
    bcmdrd_field_get(src_ent_buf, start_bit, end_bit, ent);

    start_bit = field_info->field_start_bit[nme_loc];
    end_bit = start_bit + width - 1;
    bcmdrd_field_set(dst_ent_buf, start_bit, end_bit, ent);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Remove an entry from hardware, PTcache and RM state.
 *
 * \param [in] unit Unit number.
 * \param [in] req_ctxt Context information of the requested operation.
 * \param [in] pglb_si Pointer to rm_hash_glb_srch_info_t structure.
 * \param [in] srch_result Entry search result.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_remove(int unit,
                   rm_hash_req_context_t *req_ctxt,
                   rm_hash_glb_srch_info_t *pglb_si,
                   rm_hash_map_srch_result_t *map_sr)
{
    uint32_t rsp_flags = BCMPTM_RSP_FLAGS_NO_FLAGS;
    bcmbd_pt_dyn_info_t dyn_info;
    const bcmptm_rm_hash_view_info_t *view_info = NULL;
    bcmdrd_sid_t sid = INVALIDm;
    rm_hash_entry_buf_t *ent_buf = NULL;
    rm_hash_inst_srch_result_t *inst_sr = NULL;
    uint32_t hit_list[RM_HASH_HIT_MAX_CNT] = {0};
    uint8_t pipe, inst;
    rm_hash_pt_info_t *pt_info = NULL;
    bcmdrd_sid_t nsid;
    int pipe_num;
    rm_hash_map_srch_info_t *map_si = NULL;
    rm_hash_inst_srch_info_t *inst_si = NULL;
    bcmbd_pt_dyn_info_t *pt_dyn_info = req_ctxt->pt_dyn_info;
    bool set_cache_vbit = FALSE;

    SHR_FUNC_ENTER(unit);

    pipe = (pt_dyn_info->tbl_inst == -1)? 0 : pt_dyn_info->tbl_inst;
    ent_buf = bcmptm_rm_hash_entry_buf_alloc();
    if (ent_buf == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_pt_info_get(unit, &pt_info));
    map_si = &pglb_si->map_srch_info[map_sr->map_idx];
    for (inst = 0; inst < map_sr->num_insts; inst++) {
        inst_si = &map_si->inst_srch_info[inst];
        inst_si->ltid = req_ctxt->req_ltid;
        inst_sr = &map_sr->inst_srch_result[inst];
        /* Update RM hash internal state. */
        if ((pt_info->bank_mode[inst_sr->rbank] == RM_HASH_BANK_MODE_MIXED) &&
            (pt_dyn_info->tbl_inst == -1)) {
            /*
             * The physical table works in mixed mode, and the intended operation
             * is for global LT.
             */
            nsid = *pt_info->single_view_sid[inst_sr->rbank];
            pipe_num = bcmdrd_pt_num_tbl_inst(unit, nsid);
            for (pipe = 0; pipe < pipe_num; pipe++) {
                SHR_IF_ERR_EXIT
                    (rm_hash_bkt_state_clear(unit, pt_info, pipe, inst_si,
                                             inst_sr));
            }
        } else if (pt_dyn_info->tbl_inst == -1) {
            SHR_IF_ERR_EXIT
                (rm_hash_bkt_state_clear(unit, pt_info, 0, inst_si, inst_sr));
        } else if (pt_dyn_info->tbl_inst >= 0) {
            pipe = pt_dyn_info->tbl_inst;
            SHR_IF_ERR_EXIT
                (rm_hash_bkt_state_clear(unit, pt_info, pipe, inst_si,
                                         inst_sr));
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        sid = inst_sr->sid;
        sal_memset(ent_buf->e_words, 0, sizeof(ent_buf->e_words));
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Remove an existing entry on mem[%s] index[%d].\n"),
                                bcmdrd_pt_sid_to_name(unit, sid),
                                inst_sr->e_idx));
        dyn_info = *req_ctxt->pt_dyn_info;
        dyn_info.index = inst_sr->e_idx;


        /*
         * Update HW and PTcache. For narrow mode entry, only portion of
         * the base entry should be cleared.
         */
        if ((inst_si->e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_HALF) ||
            (inst_si->e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_THIRD)) {
            bool cache_valid;
            bcmltd_sid_t ltid;
            uint16_t dft_id;
            uint64_t rd_req_flags = 0;
            uint32_t rd_rsp_flags = 0;
            rd_req_flags = req_ctxt->req_flags & ~BCMLT_ENT_ATTR_GET_FROM_HW;
            SHR_IF_ERR_EXIT
                (bcmptm_cmdproc_read(unit,
                                     rd_req_flags,
                                     0,
                                     sid,
                                     &dyn_info,
                                     NULL,
                                     BCMPTM_MAX_PT_ENTRY_WORDS,
                                     ent_buf->e_words,
                                     &cache_valid,
                                     &ltid,
                                     &dft_id,
                                     &rd_rsp_flags));
            if (cache_valid == FALSE) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_IF_ERR_EXIT
                (rm_hash_nm_entry_clear(unit,
                                        pt_info,
                                        inst_sr->rbank,
                                        inst_si->e_nm,
                                        inst_sr->nme_info.nm_loc,
                                        ent_buf->e_words));
            set_cache_vbit = TRUE;
        }
        SHR_IF_ERR_EXIT
            (bcmptm_cmdproc_write(unit,
                                  req_ctxt->req_flags,
                                  0,
                                  sid,
                                  &dyn_info,
                                  NULL,
                                  ent_buf->e_words,
                                  TRUE,
                                  TRUE,
                                  set_cache_vbit,
                                  BCMPTM_RM_OP_NORMAL, NULL,
                                  0,
                                  0,
                                  &rsp_flags));
        view_info = inst_sr->view_info;
        if ((view_info != NULL) && (view_info->hit_info != NULL)) {
            /* LT has HIT fields and these HIT fields should be updated as well. */
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_hit_table_update(unit,
                                                 req_ctxt,
                                                 &dyn_info,
                                                 view_info->hit_info,
                                                 hit_list,
                                                 view_info->hit_info->hit_map_count));
        }
    }

exit:
    if (ent_buf != NULL) {
        bcmptm_rm_hash_entry_buf_free(ent_buf);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get requesting entry size in uint of base entry.
 *
 * \param [in] unit Unit number.
 * \param [in] req_info Pointer to bcmptm_rm_hash_req_t structure.
 * \param [in] lt_info Pointer to bcmptm_rm_hash_lt_info_t structure.
 * \param [out] e_size Entry size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_req_ent_size_get(int unit,
                         bcmptm_rm_hash_req_t *req_info,
                         bcmptm_rm_hash_lt_info_t *lt_info,
                         uint8_t *e_size)
{
    bcmptm_rm_hash_entry_attrs_t *e_attrs = req_info->entry_attrs;
    const bcmptm_rm_hash_view_info_t *view_info = NULL;

    SHR_FUNC_ENTER(unit);

    if ((e_attrs != NULL) && (e_attrs->d_entry_attrs != NULL)) {
        *e_size = e_attrs->d_entry_attrs->entry_size;
    } else {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_ent_exact_view_get(unit,
                                               req_info->entry_attrs,
                                               lt_info,
                                               0,
                                               req_info->dfield_format_id,
                                               &view_info));
        *e_size = view_info->num_base_entries;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get least full bucket for single lt to pt mapping.
 *
 * \param [in] unit Unit number.
 * \param [in] glb_srch_info Pointer to rm_hash_glb_srch_info_t structure.
 * \param [in] tbl_inst Table instance.
 * \param [in] ent_attr Pointer to rm_hash_req_ent_attr_t structure.
 * \param [out] free_slot_info Pointer to rm_hash_free_slot_info_t structure.
 * \param [out] finst_si_list Pointer to rm_hash_inst_srch_info_t structure.
 * \param [out] fslb_list Pointer to rm_hash_ent_slb_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_single_map_free_slot_get(int unit,
                                 rm_hash_glb_srch_info_t *glb_srch_info,
                                 int tbl_inst,
                                 rm_hash_req_ent_attr_t *ent_attr,
                                 rm_hash_free_slot_info_t *free_slot_info,
                                 rm_hash_inst_srch_info_t **finst_si_list,
                                 rm_hash_ent_slb_info_t *fslb_list)
{
    uint8_t num_slb_info;
    rm_hash_map_srch_info_t *map_si = NULL;
    rm_hash_inst_srch_info_t *inst_si = NULL;
    rm_hash_ent_slb_info_t *slb_il = NULL;
    uint8_t *rbank_list = NULL, slb_idx = 0;

    SHR_FUNC_ENTER(unit);

    map_si  = &glb_srch_info->map_srch_info[0];
    inst_si = &map_si->inst_srch_info[0];
    slb_il  = inst_si->slb_info_list;
    num_slb_info = inst_si->fmt_info->num_en_rbanks;
    rbank_list = inst_si->fmt_info->en_rbank_list;

    sal_memset(free_slot_info, 0, sizeof(*free_slot_info));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_free_ent_space_search(unit,
                                              inst_si->fmt_info->pt_info,
                                              rbank_list,
                                              tbl_inst,
                                              slb_il,
                                              num_slb_info,
                                              ent_attr,
                                              free_slot_info));
    if (free_slot_info->found == TRUE) {
        finst_si_list[0] = inst_si;
        slb_idx = free_slot_info->slb_info_idx_list[0];
        fslb_list[0] = slb_il[slb_idx];
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get least full bucket for multi group lt to pt mapping.
 *
 * \param [in] unit Unit number.
 * \param [in] glb_srch_info Pointer to rm_hash_glb_srch_info_t structure.
 * \param [in] tbl_inst Table instance.
 * \param [in] ent_attr Pointer to rm_hash_req_ent_attr_t structure.
 * \param [out] free_slot_info Pointer to rm_hash_free_slot_info_t structure.
 * \param [out] finst_si_list Pointer to rm_hash_inst_srch_info_t structure.
 * \param [out] fslb_list Pointer to rm_hash_ent_slb_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_multi_map_free_slot_get(int unit,
                                rm_hash_lt_ctrl_t *lt_ctrl,
                                int tbl_inst,
                                rm_hash_glb_srch_info_t *glb_srch_info,
                                rm_hash_req_ent_attr_t *ent_attr,
                                rm_hash_free_slot_info_t *free_slot_info,
                                rm_hash_inst_srch_info_t **finst_si_list,
                                rm_hash_ent_slb_info_t *free_slb_list)
{
    uint8_t num_slb_info, map_idx, smap_idx, inst, slb_idx = 0;
    rm_hash_map_srch_info_t *map_si = NULL;
    rm_hash_inst_srch_info_t *inst_si = NULL;
    rm_hash_ent_slb_info_t slb_il[RM_HASH_MAX_RSRC_GRP_COUNT];
    rm_hash_inst_srch_info_t *inst_si_list[RM_HASH_MAX_RSRC_GRP_COUNT];
    uint8_t rbank_list[RM_HASH_MAX_RSRC_GRP_COUNT] = {0};

    SHR_FUNC_ENTER(unit);

    for (map_idx = 0; map_idx < lt_ctrl->num_maps; map_idx++) {
        /*
         * Search the group based on their size. Large size group will be
         * searched first to increase table utilization.
         * The array of srch_grp_idx[] will be updated dynamically.
         */
        smap_idx = lt_ctrl->srch_map_idx[map_idx];

        map_si = &glb_srch_info->map_srch_info[smap_idx];
        for (inst = 0; inst < map_si->num_insts; inst++) {
            inst_si = &map_si->inst_srch_info[inst];
            slb_il[slb_idx] = inst_si->slb_info_list[0];
            slb_il[slb_idx].map = smap_idx;
            slb_il[slb_idx].map |= (inst << 0x04);
            if (map_si->num_insts > 1) {
                slb_il[slb_idx].map |= RM_HASH_MULTI_INST_GRP;
            }
            rbank_list[slb_idx] = inst_si->fmt_info->en_rbank_list[0];
            inst_si_list[slb_idx] = inst_si;
            slb_idx++;
        }
    }
    if (slb_idx == 0) {
        /* There is no enabled mapping groups and banks for this LT. */
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }
    num_slb_info = slb_idx;

    sal_memset(free_slot_info, 0, sizeof(*free_slot_info));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_free_ent_space_search(unit,
                                              inst_si->fmt_info->pt_info,
                                              rbank_list,
                                              tbl_inst,
                                              slb_il,
                                              num_slb_info,
                                              ent_attr,
                                              free_slot_info));
    if (free_slot_info->found == TRUE) {
        for (inst = 0; inst < free_slot_info->num_inst; inst++) {
            slb_idx = free_slot_info->slb_info_idx_list[inst];
            finst_si_list[inst] = inst_si_list[slb_idx];
            free_slb_list[inst] = slb_il[slb_idx];
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Iterate all the hash tables and try to search a free slot.
 *
 * \param [in] unit Unit number.
 * \param [in] req_ctxt Pointer to rm_hash_req_context_t structure.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [in] glb_srch_info Pointer to rm_hash_glb_srch_info_t structure.
 * \param [out] slot_found Indicates if free slot has been found.
 * \param [out] grp_ei Pointer to buffer to hold the free slot information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_grp_free_slot_get(int unit,
                          rm_hash_req_context_t *req_ctxt,
                          rm_hash_lt_ctrl_t *lt_ctrl,
                          rm_hash_glb_srch_info_t *glb_srch_info,
                          bool *slot_found,
                          rm_hash_map_entry_info_t *map_ei)
{
    uint8_t e_size = 0, mmap, inst, grp_idx;
    rm_hash_inst_srch_info_t *finst_si[RM_HASH_MAX_GRP_INST_COUNT];
    rm_hash_inst_srch_info_t *inst_si = NULL;
    rm_hash_inst_entry_info_t *inst_ei = NULL;
    rm_hash_free_slot_info_t free_slot_info;
    rm_hash_ent_slb_info_t fslb_info_list[RM_HASH_MAX_GRP_INST_COUNT];
    rm_hash_pt_info_t *pt_info = NULL;
    bool separate = FALSE;
    bcmbd_pt_dyn_info_t *pt_dyn_info = req_ctxt->pt_dyn_info;

    SHR_FUNC_ENTER(unit);

    if (lt_ctrl->num_maps == 0) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }
    SHR_IF_ERR_EXIT
        (rm_hash_req_ent_size_get(unit, req_ctxt->req_info, &lt_ctrl->lt_info,
                                                            &e_size));

    map_ei->e_attr.e_size = e_size;
    map_ei->e_attr.e_bm = glb_srch_info->map_srch_info[0].inst_srch_info[0].e_bm;
    map_ei->e_attr.e_nm = glb_srch_info->map_srch_info[0].inst_srch_info[0].e_nm;

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_bank_has_separated_sid(unit, &separate));
    if (separate == TRUE) {
        SHR_IF_ERR_EXIT
            (rm_hash_multi_map_free_slot_get(unit,
                                             lt_ctrl,
                                             pt_dyn_info->tbl_inst,
                                             glb_srch_info,
                                             &map_ei->e_attr,
                                             &free_slot_info,
                                             finst_si,
                                             fslb_info_list));
    } else {
        SHR_IF_ERR_EXIT
            (rm_hash_single_map_free_slot_get(unit,
                                              glb_srch_info,
                                              pt_dyn_info->tbl_inst,
                                              &map_ei->e_attr,
                                              &free_slot_info,
                                              finst_si,
                                              fslb_info_list));
    }
    *slot_found = free_slot_info.found;

    if (free_slot_info.found == TRUE) {
        mmap = fslb_info_list[0].map;
        mmap = mmap & RM_HASH_MGRP_NUM_MASK;

        map_ei->map = mmap;
        map_ei->num_insts = free_slot_info.num_inst;
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_pt_info_get(unit, &pt_info));
        for (inst = 0; inst < free_slot_info.num_inst; inst++) {
            inst_si = finst_si[inst];
            inst_ei = &map_ei->inst_ei[inst];
            inst_ei->slb = fslb_info_list[inst].slb;
            inst_ei->key_fmt = inst_si->fmt_info->key_format;
            inst_ei->loc = free_slot_info.e_loc_list[inst];
            inst_ei->nm_offset = free_slot_info.nm_offset_list[inst];
            inst_ei->e_nm = inst_si->e_nm;
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_slb_to_rbank_get(unit,
                                                 pt_info,
                                                 inst_ei->slb,
                                                 &inst_ei->rbank));
            grp_idx = lt_ctrl->map_info[mmap].grp_idx[inst];
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_size_based_view_info_get(unit,
                                                         &lt_ctrl->lt_info,
                                                         grp_idx,
                                                         inst_ei->key_fmt,
                                                         e_size,
                                                         &inst_ei->view_info));
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_ent_index_get(unit,
                                              pt_info,
                                              inst_ei->slb,
                                              &inst_ei->loc,
                                              e_size,
                                              &inst_ei->rbank,
                                              &inst_ei->index));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Hash delete request handler
 *
 * \param [in] unit Unit number.
 * \param [in] req_ctxt Context information for the requested operation.
 * \param [in] rsp_entry_buf_wsize Size of response entry buffer.
 * \param [out] rsp_entry_words Pointer to a buffer for recording response entry.
 * \param [out] rsp_info Pointer to bcmptm_rm_hash_rsp_t structure.
 * \param [out] rsp_ltid Pointer to bcmltd_sid_t object.
 * \param [out] rsp_flags Pointer to response flags.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_delete(int unit,
               rm_hash_req_context_t *req_ctxt,
               size_t rsp_entry_buf_wsize,
               uint32_t *rsp_entry_words,
               bcmptm_rm_hash_rsp_t *rsp_info,
               bcmltd_sid_t *rsp_ltid,
               uint32_t *rsp_flags)
{
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    rm_hash_glb_srch_info_t *pglb_si = NULL;
    rm_hash_map_srch_result_t *pmap_sr = NULL;
    uint8_t e_size, e_idx, pipe;
    uint32_t rsp_entry_index[BCMPTM_RM_HASH_RSP_MAX_INST_CNT];
    bcmdrd_sid_t rsp_entry_sid[BCMPTM_RM_HASH_RSP_MAX_INST_CNT];
    bcmbd_pt_dyn_info_t *pt_dyn_info = req_ctxt->pt_dyn_info;

    SHR_FUNC_ENTER(unit);

    pipe = (pt_dyn_info->tbl_inst == -1)? 0 : pt_dyn_info->tbl_inst;
    pglb_si = bcmptm_rm_hash_glb_srch_info_alloc();
    pmap_sr = bcmptm_rm_hash_map_srch_result_alloc();
    if ((pglb_si == NULL) || (pmap_sr == NULL)) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(pglb_si, 0, sizeof(*pglb_si));
    sal_memset(pmap_sr, 0, sizeof(*pmap_sr));

    /* Get lt contrl structure for this LTID. */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_lt_ctrl_get(unit, req_ctxt->req_ltid, &lt_ctrl));
    SHR_NULL_CHECK(lt_ctrl, SHR_E_PARAM);

    /* Search all the groups to determine if the entry exists. */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_search(unit,
                                   req_ctxt,
                                   lt_ctrl,
                                   pglb_si,
                                   pmap_sr));

    if (pmap_sr->e_exist == FALSE) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT
        (rm_hash_ent_remove(unit, req_ctxt, pglb_si, pmap_sr));
    for (e_idx = 0; e_idx < pmap_sr->num_insts; e_idx++) {
        rsp_entry_index[e_idx] = pmap_sr->inst_srch_result[e_idx].e_idx;
        rsp_entry_sid[e_idx]   = pmap_sr->inst_srch_result[e_idx].sid;
    }
    rm_hash_rsp_info_set(unit,
                         pmap_sr->num_insts,
                         rsp_entry_index,
                         rsp_entry_sid,
                         pmap_sr->inst_srch_result[0].ldtype_val,
                         pipe,
                         rsp_info);

    e_size = bcmdrd_pt_entry_wsize(unit, pmap_sr->inst_srch_result[0].sid);
    if (rsp_entry_buf_wsize < e_size) {
         SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (((lt_ctrl->key_field_exchange == TRUE) ||
         (lt_ctrl->data_field_exchange == TRUE)) &&
         (pmap_sr->map_idx > 0)) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_key_fields_exchange(unit,
                                                req_ctxt->req_info->entry_attrs,
                                                &lt_ctrl->lt_info,
                                                pmap_sr->map_idx,
                                                0,
                                                pmap_sr->inst_srch_result[0].e_words,
                                                rsp_entry_words));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_data_fields_exchange(unit,
                                                 &lt_ctrl->lt_info,
                                                 pmap_sr->inst_srch_result[0].ldtype_val,
                                                 pmap_sr->map_idx,
                                                 0,
                                                 pmap_sr->inst_srch_result[0].e_words,
                                                 rsp_entry_words));
    } else {
        sal_memcpy(rsp_entry_words, pmap_sr->inst_srch_result[0].e_words, e_size * 4);
    }

    *rsp_ltid = req_ctxt->req_ltid;
    *rsp_flags = BCMPTM_RSP_FLAGS_NO_FLAGS;
    SHR_EXIT();

exit:
    if (pglb_si != NULL) {
        bcmptm_rm_hash_glb_srch_info_free(pglb_si);
    }
    if (pmap_sr != NULL) {
        bcmptm_rm_hash_map_srch_result_free(pmap_sr);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Replace an entry with same entry size.
 *
 * \param [in] unit Unit number.
 * \param [in] req_ctxt Context information for the requested operation.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [in] grp_sr Pointer to rm_hash_map_srch_result_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_replace(int unit,
                    rm_hash_req_context_t *req_ctxt,
                    rm_hash_lt_ctrl_t *lt_ctrl,
                    rm_hash_glb_srch_info_t *pglb_si,
                    rm_hash_map_srch_result_t *map_sr)
{
    bcmptm_rm_hash_lt_info_t *lt_info = NULL;
    bcmbd_pt_dyn_info_t dyn_info = {-1, -1};
    uint32_t wr_rsp_flags = BCMPTM_RSP_FLAGS_NO_FLAGS;
    uint16_t data_type = 0;
    const bcmptm_rm_hash_key_format_t *key_fmt = NULL;
    const bcmptm_rm_hash_view_info_t *view_info = NULL;
    int rv = SHR_E_NONE;
    uint32_t *pentry = NULL;
    bcmptm_rm_hash_req_t *req_info = req_ctxt->req_info;
    uint8_t map_idx, inst, grp_idx;
    rm_hash_entry_buf_t *ent_buf = NULL;
    rm_hash_inst_srch_result_t *inst_sr = NULL;
    uint32_t hit_list[RM_HASH_HIT_MAX_CNT];
    uint8_t hit_cnt = 0;
    bool field_exchange = FALSE;
    rm_hash_map_srch_info_t *map_si = NULL;
    rm_hash_inst_srch_info_t *inst_si = NULL;
    rm_hash_pt_info_t *pt_info;
    uint32_t nm_ent[BCMPTM_MAX_PT_FIELD_WORDS] = {0};
    bcmltd_sid_t req_ltid = req_ctxt->req_ltid;

    SHR_FUNC_ENTER(unit);

    map_idx = map_sr->map_idx;
    field_exchange = ((lt_ctrl->key_field_exchange == TRUE) ||
                      (lt_ctrl->data_field_exchange == TRUE)) && (map_idx > 0);
    if (field_exchange == TRUE) {
        ent_buf = bcmptm_rm_hash_entry_buf_alloc();
        if (ent_buf == NULL) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
    }

    lt_info = &lt_ctrl->lt_info;
    pentry = req_info->entry_words;

    map_si = &pglb_si->map_srch_info[map_idx];
    for (inst = 0; inst < map_sr->num_insts; inst++) {
        grp_idx = lt_ctrl->map_info[map_idx].grp_idx[inst];
        if (field_exchange == TRUE) {
            sal_memset(ent_buf->e_words, 0, sizeof(ent_buf->e_words));
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_key_fields_exchange(unit,
                                                    req_info->entry_attrs,
                                                    lt_info,
                                                    0,
                                                    grp_idx,
                                                    req_info->entry_words,
                                                    ent_buf->e_words));
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_data_fields_exchange(unit,
                                                     lt_info,
                                                     req_info->dfield_format_id,
                                                     0,
                                                     grp_idx,
                                                     req_info->entry_words,
                                                     ent_buf->e_words));
            pentry = ent_buf->e_words;
        }
        inst_si = &map_si->inst_srch_info[inst];
        inst_sr = &map_sr->inst_srch_result[inst];
        key_fmt = inst_sr->key_fmt;
        view_info = inst_sr->view_info;
        /* The key type value needs to be filled into entry buffer */
        if (key_fmt->key_type_val != NULL) {
            bcmptm_rm_hash_key_type_fill(unit,
                                         *key_fmt->key_type_val,
                                         view_info,
                                         pentry);
        }
        rv = bcmptm_rm_hash_pdtype_get(unit,
                                       lt_info,
                                       grp_idx,
                                       req_info->dfield_format_id,
                                       &data_type);
        if (rv == SHR_E_NONE) {
            bcmptm_rm_hash_data_type_fill(unit,
                                          data_type,
                                          view_info,
                                          pentry);
        }
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "There exists a matched entry with same size for "
                                "ltid[%d] on mem[%s] index[%d]. "
                                "The existing matched entry will be replaced.\n"),
                                req_ctxt->req_ltid,
                                bcmdrd_pt_sid_to_name(unit,
                                                      inst_sr->sid),
                                                      inst_sr->e_idx));
        if (view_info->hit_info != NULL) {
            sal_memset(hit_list, 0, sizeof(hit_list));
            hit_cnt = 0;
            bcmptm_rm_hash_entry_hit_bit_retrieve(unit,
                                                  pentry,
                                                  view_info->hit_info,
                                                  hit_list,
                                                  &hit_cnt);
        }
        /* Use the values provided by caller, except index */
        dyn_info = *req_ctxt->pt_dyn_info;
        dyn_info.index = inst_sr->e_idx;
        /*
         * For narrow mode entry, only portion of the base entry
         * needs to be updated.
         */
        if ((inst_si->e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_HALF) ||
            (inst_si->e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_THIRD)) {
            bool cache_valid;
            bcmltd_sid_t rsp_ltid;
            uint16_t dft_id;
            uint64_t rd_req_flags = 0;
            uint32_t rd_rsp_flags = 0;
            rd_req_flags = req_ctxt->req_flags & ~BCMLT_ENT_ATTR_GET_FROM_HW;
            SHR_IF_ERR_EXIT
                (bcmptm_cmdproc_read(unit,
                                     rd_req_flags,
                                     0,
                                     inst_sr->sid,
                                     &dyn_info,
                                     NULL,
                                     BCMPTM_MAX_PT_ENTRY_WORDS,
                                     nm_ent,
                                     &cache_valid,
                                     &rsp_ltid,
                                     &dft_id,
                                     &rd_rsp_flags));
            if (cache_valid == FALSE) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_pt_info_get(unit, &pt_info));
            SHR_IF_ERR_EXIT
                (rm_hash_nm_entry_set(unit,
                                      pt_info,
                                      inst_sr->rbank,
                                      inst_si->e_nm,
                                      inst_sr->nme_info.nm_loc,
                                      pentry,
                                      nm_ent));
            pentry = nm_ent;
            req_ltid = 0;
        }
        SHR_IF_ERR_EXIT
            (bcmptm_cmdproc_write(unit,
                                  req_ctxt->req_flags,
                                  0,
                                  inst_sr->sid,
                                  &dyn_info,
                                  NULL, /* misc_info */
                                  pentry,
                                  TRUE,
                                  TRUE,
                                  TRUE,
                                  BCMPTM_RM_OP_NORMAL, NULL,
                                  req_ltid,
                                  req_info->dfield_format_id,
                                  &wr_rsp_flags));
        if (view_info->hit_info != NULL) {
            /* LT has HIT fields and these HIT fields should be updated as well. */
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_hit_table_update(unit,
                                                 req_ctxt,
                                                 &dyn_info,
                                                 view_info->hit_info,
                                                 hit_list,
                                                 hit_cnt));
        }
    }

exit:
    if (ent_buf != NULL) {
        bcmptm_rm_hash_entry_buf_free(ent_buf);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Construct SLB list to be recorded in RM state.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [in] glb_srch_info Pointer to rm_hash_glb_srch_info_t structure.
 * \param [out] slb_info_list Buffer to hold SLB list.
 * \param [out] num_slb_info Buffer to hold SLB number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_slb_list_get(int unit,
                         rm_hash_lt_ctrl_t *lt_ctrl,
                         rm_hash_glb_srch_info_t *glb_srch_info,
                         rm_hash_ent_slb_info_t *slb_info_list,
                         uint8_t *num_slb_info)
{
    uint8_t map_idx, inst, slb_idx = 0;
    rm_hash_map_srch_info_t *map_si = NULL;
    rm_hash_inst_srch_info_t *inst_si = NULL;

    SHR_FUNC_ENTER(unit);

    /* Only record valid SLB for currently enabled maps. */
    for (map_idx = 0; map_idx < lt_ctrl->num_maps; map_idx++) {
        map_si = &glb_srch_info->map_srch_info[map_idx];
        for (inst = 0; inst < map_si->num_insts; inst++) {
            inst_si = &map_si->inst_srch_info[inst];
            slb_info_list[slb_idx] = inst_si->slb_info_list[0];
            slb_info_list[slb_idx].map = map_idx;
            if (map_si->num_insts > 1) {
                slb_info_list[slb_idx].map |= RM_HASH_MULTI_INST_GRP;
            }
            slb_idx++;
        }
    }
    *num_slb_info = slb_idx;

    SHR_FUNC_EXIT();
}

/*!
 * \brief Report an entry move event during hash entry install.
 *
 * \param [in] unit Unit number.
 * \param [in] tbl_inst Table pipe instance.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [in] map_ei Pointer to rm_hash_map_entry_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_install_report(int unit,
                           int tbl_inst,
                           rm_hash_lt_ctrl_t *lt_ctrl,
                           rm_hash_map_entry_info_t *map_ei)
{
    uint8_t idx;
    uint32_t dst_ctr_index[RM_HASH_LOOKUP_MAX_CNT];
    uint32_t src_ctr_index[RM_HASH_LOOKUP_MAX_CNT];
    rm_hash_inst_entry_info_t *inst_ei = &map_ei->inst_ei[0];

    rm_hash_lt_flex_ctr_info_t *fc_info;

    SHR_FUNC_ENTER(unit);

    fc_info = &lt_ctrl->flex_ctr_info;
    if ((fc_info->num_lookups == 0) || (fc_info->entry_index_cb == NULL) ||
        (fc_info->report_for_insert == FALSE)) {
        SHR_EXIT();
    }

    /* Construct destination counter index. */
    for (idx = 0; idx < fc_info->num_lookups; idx++) {
        if (map_ei->num_insts > 1) {
            inst_ei = &map_ei->inst_ei[idx];
        }
        if (fc_info->lookup_entry_move_reporting[idx] == TRUE) {
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_flex_ctr_offset_get(unit,
                                                    lt_ctrl,
                                                    inst_ei->view_info->sid[0],
                                                    tbl_inst,
                                                    inst_ei->index,
                                                    map_ei->e_attr.e_size,
                                                    &dst_ctr_index[idx]));
        } else {
            dst_ctr_index[idx] = BCMPTM_INVALID_HIT_INDEX;
        }
    }
    /*
     * Construct source counter index. For new entry insert, the source
     * will be always set as BCMPTM_INVALID_HIT_INDEX.
     */
    for (idx = 0; idx < fc_info->num_lookups; idx++) {
        src_ctr_index[idx] = BCMPTM_INVALID_HIT_INDEX;
    }
    bcmptm_rm_hash_move_event_notify(unit, lt_ctrl->ltid, tbl_inst,
                                     fc_info->num_lookups,
                                     fc_info->context,
                                     dst_ctr_index, src_ctr_index);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Prepare entry via fill key type and data type.
 *
 * \param [in] unit Unit number.
 * \param [in] req_info Pointer to bcmptm_rm_hash_req_t structure..
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [in] key_fmt Pointer to bcmptm_rm_hash_key_format_t structure.
 * \param [in] view_info Pointer to bcmptm_rm_hash_view_info_t structure.
 * \param [in] grp_idx Group index.
 * \param [out] pentry Pointer to entry buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static void
rm_hash_entry_prep(int unit,
                   bcmptm_rm_hash_req_t *req_info,
                   rm_hash_lt_ctrl_t *lt_ctrl,
                   const bcmptm_rm_hash_key_format_t *key_fmt,
                   const bcmptm_rm_hash_view_info_t *view_info,
                   uint8_t grp_idx,
                   uint32_t *pentry)
{
    uint16_t data_type = 0;
    int rv = SHR_E_NONE;

    /* The key type value needs to be filled into entry buffer. */
    if (key_fmt->key_type_val != NULL) {
        bcmptm_rm_hash_key_type_fill(unit,
                                     key_fmt->key_type_val[0],
                                     view_info,
                                     pentry);
    }

    rv = bcmptm_rm_hash_pdtype_get(unit,
                                   &lt_ctrl->lt_info,
                                   grp_idx,
                                   req_info->dfield_format_id,
                                   &data_type);
    if (rv == SHR_E_NONE) {
        /*
         * There is physical data type value needs to be filled into
         * entry buffer.
         */
        bcmptm_rm_hash_data_type_fill(unit,
                                      data_type,
                                      view_info,
                                      pentry);
    }
}

/*!
 * \brief Get the free entry node index for a map.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] map_ei Pointer to rm_hash_map_entry_info_t structure.
 * \param [in] pipe Pipe number.
 * \param [out] e_node_idx_list Pointer to entry node index buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_node_idx_list_get(int unit,
                              rm_hash_pt_info_t *pt_info,
                              rm_hash_map_entry_info_t *map_ei,
                              uint8_t pipe,
                              uint8_t *e_node_idx_list)
{
    uint8_t idx;
    rm_hash_inst_entry_info_t *ei = NULL;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < map_ei->num_insts; idx++) {
        ei = &map_ei->inst_ei[idx];
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_free_ent_node_idx_get(unit,
                                                  pt_info,
                                                  ei->rbank,
                                                  pipe,
                                                  ei->slb,
                                                  &e_node_idx_list[idx]));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update the bucket state after an entry is inserted.
 *
 * After a new entry is inserted, a corresponding entry node which records
 * the state of the entry from the resource management perspective will be
 * allocated, populated and placed at the appropriate location.
 *
 * \param [in] unit Unit number.
 * \param [in] req_ltid Logical table id.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] e_loc Pointer to rm_hash_ent_loc_t structure.
 * \param [in] slb_info Pointer to rm_hash_ent_slb_info_t structure.
 * \param [in] num_slb_info Number of slb information.
 * \param [in] rbank_list List of banks.
 * \param [in] pipe Pipe instance.
 * \param [in] e_node_idx_list Entry node list.
 * \param [in] num_rbanks Number of banks.
 * \param [in] instance Instance number.
 * \param [in] slb SLB of the entry.
 * \param [in] sid Physical table symbol id.
 * \param [in] e_sig Signature of the entry node.
 * \param [in] inst_si Pointer to rm_hash_inst_srch_info_t structure.
 * \param [in] flags Indicating if LT is global.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_bkt_state_set(int unit,
                      bcmltd_sid_t req_ltid,
                      rm_hash_pt_info_t *pt_info,
                      rm_hash_inst_entry_info_t *inst_ei,
                      rm_hash_ent_slb_info_t *slb_info,
                      uint8_t num_slb_info,
                      uint8_t pipe,
                      uint8_t *e_node_idx_list,
                      rm_hash_map_entry_info_t *map_ei,
                      uint8_t instance,
                      uint32_t e_sig,
                      rm_hash_inst_srch_info_t *inst_si,
                      uint32_t flags)
{
    rm_hash_ent_node_t *e_node = NULL;
    rm_hash_nme_info_t nm_info;
    bool found = FALSE;
    uint32_t ins_idx = 0;

    SHR_FUNC_ENTER(unit);

    nm_info.e_nm = map_ei->e_attr.e_nm;
    nm_info.nm_loc = inst_ei->nm_offset;
    nm_info.ltid = req_ltid;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "New entry node inserting...\n")));
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "physical mem:[%s], "
                            "base bucket bitmap:[%x], "
                            "base entry bitmap:[%x], "
                            "logical entry bitmap:[%x] \n"),
                            bcmdrd_pt_sid_to_name(unit, inst_ei->view_info->sid[0]),
                            inst_ei->loc.bb_bmp,
                            inst_ei->loc.be_bmp,
                            inst_ei->loc.le_bmp));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_node_allocate(unit, pt_info, inst_ei->rbank,
                                          pipe, inst_ei->slb,
                                          &e_node_idx_list[instance],
                                          &e_node));
    if (inst_si->hash_store_en) {
        e_node->flag |= RM_HASH_NODE_HASH_STORE;
    }
    bcmptm_rm_hash_ent_node_set(unit, pt_info, &inst_ei->loc,
                                slb_info, num_slb_info,
                                inst_ei->rbank, pipe,
                                e_node_idx_list, map_ei->num_insts,
                                inst_ei->view_info->sid[0],
                                e_sig,
                                map_ei->e_attr.e_bm,
                                e_node);
    e_node->flag |= flags;
    if (nm_info.e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_HALF) {
        e_node->flag |= RM_HASH_NODE_NARROWMODEHALF;
        e_node->nm_loc = nm_info.nm_loc;
    } else if (nm_info.e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_THIRD) {
        e_node->flag |= RM_HASH_NODE_NARROWMODETHIRD;
        e_node->nm_loc = nm_info.nm_loc;
    }

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_node_insert(unit, pt_info, inst_ei->rbank, pipe,
                                        inst_ei->slb, &nm_info, e_node));

    if (inst_si->hash_store_en) {
        bcmptm_rm_hash_store_bin_search(unit,
                                        inst_si->ltid,
                                        inst_si->hash_store_val,
                                        &found,
                                        NULL,
                                        &ins_idx);
        assert(found == FALSE);
        if (!found) {
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_store_entry_insert
                    (unit,
                     inst_si->ltid,
                     inst_si->hash_store_val,
                     inst_si->hash_vector->full_key,
                     ins_idx));
        }
    }

    /*
     * Bucket state and entry node has been updated, corresponding
     * undo operation will be recorded in the undo list.
     */
    if (bcmptm_rm_hash_undo_list_maintained(unit) == TRUE) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_trans_undo_node_insert(unit,
                                                   pt_info,
                                                   inst_ei->slb,
                                                   pipe,
                                                   e_node,
                                                   BCMPTM_RM_HASH_UNDO_ENT_DELETE));
    }

exit:
    SHR_FUNC_EXIT();
}

static void
rm_hash_key_hash_store_fill(bcmptm_rm_hash_dynamic_entry_attrs_t *attr,
                            uint32_t hash_store_val,
                            uint32_t *e_words)
{
    int sbit, ebit;
    uint32_t key_val = hash_store_val;
    const bcmlrd_hw_field_list_t *key_info =
        attr->hstore_attr.hash_store_key_fields;

    sbit = key_info->field_start_bit[0];
    ebit = sbit + key_info->field_width[0] - 1;
    bcmdrd_field_set(e_words, sbit, ebit, &key_val);
}

/*!
 * \brief Install a entry into hash table.
 *
 * \param [in] unit Unit number.
 * \param [in] req_ctxt Context information for the requested operation.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [in] glb_srch_info Pointer to rm_hash_glb_srch_info_t structure.
 * \param [in] map_ei Pointer to rm_hash_map_entry_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_install(int unit,
                    rm_hash_req_context_t *req_ctxt,
                    rm_hash_lt_ctrl_t *lt_ctrl,
                    rm_hash_glb_srch_info_t *glb_srch_info,
                    rm_hash_map_entry_info_t *map_ei)
{
    rm_hash_entry_buf_t *ent_buf = NULL;
    uint32_t *pentry = NULL;
    const bcmptm_rm_hash_key_format_t *key_fmt = NULL;
    int pipe_num;
    uint8_t num_slb_info = 0, inst, num_en_rbanks;
    uint32_t e_sig[RM_HASH_MAX_GRP_INST_COUNT] = {0, 0};
    bcmbd_pt_dyn_info_t dyn_info = {-1, -1};
    uint32_t wr_rsp_flags = BCMPTM_RSP_FLAGS_NO_FLAGS;
    rm_hash_ent_slb_info_t slb_info_list[RM_HASH_MAX_RSRC_GRP_COUNT];
    rm_hash_ent_slb_info_t *slb_info = NULL;
    bcmptm_rm_hash_req_t *req_info = req_ctxt->req_info;
    rm_hash_inst_entry_info_t *inst_ei = NULL;
    rm_hash_inst_srch_info_t *inst_si = NULL;
    uint8_t e_node_idx_list[RM_HASH_MAX_GRP_INST_COUNT];
    uint32_t hit_list[RM_HASH_HIT_MAX_CNT];
    uint8_t hit_cnt = 0;
    bcmptm_rm_hash_entry_attrs_t *e_attrs = req_info->entry_attrs;
    bool field_exchange = FALSE, separate = FALSE;
    uint8_t pipe;
    rm_hash_pt_info_t *pt_info = NULL;
    bcmdrd_sid_t sid;
    bcmbd_pt_dyn_info_t *pt_dyn_info = req_ctxt->pt_dyn_info;
    uint32_t nm_ent[BCMPTM_MAX_PT_FIELD_WORDS] = {0};
    bcmltd_sid_t req_ltid = req_ctxt->req_ltid;

    SHR_FUNC_ENTER(unit);

    pipe = (pt_dyn_info->tbl_inst == -1)? 0 : pt_dyn_info->tbl_inst;

    field_exchange = ((lt_ctrl->key_field_exchange == TRUE) ||
                     (lt_ctrl->data_field_exchange == TRUE)) && (map_ei->map > 0);
    if (field_exchange == TRUE) {
        ent_buf = bcmptm_rm_hash_entry_buf_alloc();
        if (ent_buf == NULL) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
    }
    pentry = req_info->entry_words;
    /* When hash store is enabled, update pentry. */
    if ((e_attrs != NULL) && (e_attrs->d_entry_attrs != NULL)) {
        if (e_attrs->d_entry_attrs->hstore_attr.enable) {
            pentry = e_attrs->d_entry_attrs->hstore_attr.hash_store_ent_words;
        }
    }

    if (field_exchange == TRUE) {
        sal_memset(ent_buf->e_words, 0, sizeof(ent_buf->e_words));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_key_fields_exchange(unit,
                                                req_info->entry_attrs,
                                                &lt_ctrl->lt_info,
                                                0,
                                                map_ei->map,
                                                req_info->entry_words,
                                                ent_buf->e_words));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_data_fields_exchange(unit,
                                                 &lt_ctrl->lt_info,
                                                 req_info->dfield_format_id,
                                                 0,
                                                 map_ei->map,
                                                 req_info->entry_words,
                                                 ent_buf->e_words));
        pentry = ent_buf->e_words;
    }

    for (inst = 0; inst < map_ei->num_insts; inst++) {
        inst_ei = &map_ei->inst_ei[inst];
        key_fmt = inst_ei->key_fmt;

        rm_hash_entry_prep(unit, req_info, lt_ctrl, key_fmt, inst_ei->view_info,
                           lt_ctrl->map_info[map_ei->map].grp_idx[inst],
                           pentry);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "New entry install for "
                                "ltid:[%d] on mem:[%s] index:[%d]."),
                                req_ctxt->req_ltid,
                                bcmdrd_pt_sid_to_name(unit, inst_ei->view_info->sid[0]),
                                inst_ei->index));
        /* Update hash store key to the entry. */
        inst_si = &glb_srch_info->map_srch_info[map_ei->map].inst_srch_info[inst];
        if ((inst_si->hash_store_en) &&
            (e_attrs != NULL) &&
            (e_attrs->d_entry_attrs != NULL)) {
            rm_hash_key_hash_store_fill(e_attrs->d_entry_attrs,
                                        inst_si->hash_store_val,
                                        pentry);
        }
        if (key_fmt->hw_key_fields != NULL) {
            bcmptm_rm_hash_ent_sig_generate(unit,
                                            pentry,
                                            key_fmt->hw_key_fields,
                                            &e_sig[inst]);
        } else if ((e_attrs != NULL) && (e_attrs->d_entry_attrs != NULL)) {
            if (!inst_si->hash_store_en) {
                bcmptm_rm_hash_ent_sig_generate(unit,
                    pentry,
                    e_attrs->d_entry_attrs->hw_key_fields,
                    &e_sig[inst]);
            } else {
                e_sig[inst] = inst_si->hash_store_val;
            }
        }
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_bank_has_separated_sid(unit, &separate));
        if (separate == TRUE) {
            SHR_IF_ERR_EXIT
                (rm_hash_ent_slb_list_get(unit,
                                          lt_ctrl,
                                          glb_srch_info,
                                          slb_info_list,
                                          &num_slb_info));
            slb_info = slb_info_list;
        } else {
            inst_si = &glb_srch_info->map_srch_info[map_ei->map].inst_srch_info[inst];
            slb_info = inst_si->slb_info_list;
            num_en_rbanks = inst_si->fmt_info->num_en_rbanks;
            num_slb_info = num_en_rbanks;
        }
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_pt_info_get(unit, &pt_info));
        if ((pt_info->bank_mode[inst_ei->rbank] == RM_HASH_BANK_MODE_MIXED) &&
            (pt_dyn_info->tbl_inst == -1)) {
            /*
             * The physical table works in mixed mode, and the intended operation
             * is for global LT.
             */
            sid = *pt_info->single_view_sid[inst_ei->rbank];
            pipe_num = bcmdrd_pt_num_tbl_inst(unit, sid);
            for (pipe = 0; pipe < pipe_num; pipe++) {
                SHR_IF_ERR_EXIT
                    (rm_hash_ent_node_idx_list_get(unit, pt_info, map_ei,
                                                   pipe, e_node_idx_list));
                SHR_IF_ERR_EXIT
                    (rm_hash_bkt_state_set(unit,
                                           req_ctxt->req_ltid,
                                           pt_info,
                                           inst_ei,
                                           slb_info,
                                           num_slb_info,
                                           pipe,
                                           e_node_idx_list,
                                           map_ei,
                                           inst,
                                           e_sig[inst],
                                           inst_si,
                                           RM_HASH_NODE_GLOBAL));
            }
        } else if (pt_dyn_info->tbl_inst == -1) {
            if (inst == 0) {
                SHR_IF_ERR_EXIT
                    (rm_hash_ent_node_idx_list_get(unit, pt_info, map_ei,
                                                   0, e_node_idx_list));
            }
            SHR_IF_ERR_EXIT
                (rm_hash_bkt_state_set(unit,
                                       req_ctxt->req_ltid,
                                       pt_info,
                                       inst_ei,
                                       slb_info,
                                       num_slb_info,
                                       0,
                                       e_node_idx_list,
                                       map_ei,
                                       inst,
                                       e_sig[inst],
                                       inst_si,
                                       RM_HASH_NODE_GLOBAL));
        } else if (pt_dyn_info->tbl_inst >= 0) {
            pipe = pt_dyn_info->tbl_inst;
            if (inst == 0) {
                SHR_IF_ERR_EXIT
                    (rm_hash_ent_node_idx_list_get(unit, pt_info, map_ei,
                                                   pipe, e_node_idx_list));
            }
            SHR_IF_ERR_EXIT
                (rm_hash_bkt_state_set(unit,
                                       req_ctxt->req_ltid,
                                       pt_info,
                                       inst_ei,
                                       slb_info,
                                       num_slb_info,
                                       pipe,
                                       e_node_idx_list,
                                       map_ei,
                                       inst,
                                       e_sig[inst],
                                       inst_si,
                                       RM_HASH_NODE_PERPIPE));
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (inst_ei->view_info->hit_info != NULL) {
            sal_memset(hit_list, 0, sizeof(hit_list));
            hit_cnt = 0;
            bcmptm_rm_hash_entry_hit_bit_retrieve(unit,
                                                  pentry,
                                                  inst_ei->view_info->hit_info,
                                                  hit_list,
                                                  &hit_cnt);
        }
        /* Use the values provided by caller, except index */
        dyn_info = *req_ctxt->pt_dyn_info;
        dyn_info.index = inst_ei->index;
        /*
         * For narrow mode entry, only portion of the base entry
         * needs to be updated.
         */
        if ((inst_ei->e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_HALF) ||
            (inst_ei->e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_THIRD)) {
            bool cache_valid;
            bcmltd_sid_t rsp_ltid;
            uint16_t dft_id;
            uint64_t rd_req_flags = 0;
            uint32_t rd_rsp_flags = 0;
            rd_req_flags = req_ctxt->req_flags & ~BCMLT_ENT_ATTR_GET_FROM_HW;
            SHR_IF_ERR_EXIT
                (bcmptm_cmdproc_read(unit,
                                     rd_req_flags,
                                     0,
                                     inst_ei->view_info->sid[0],
                                     &dyn_info,
                                     NULL,
                                     BCMPTM_MAX_PT_ENTRY_WORDS,
                                     nm_ent,
                                     &cache_valid,
                                     &rsp_ltid,
                                     &dft_id,
                                     &rd_rsp_flags));
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_pt_info_get(unit, &pt_info));
            SHR_IF_ERR_EXIT
                (rm_hash_nm_entry_set(unit,
                                      pt_info,
                                      inst_ei->rbank,
                                      inst_ei->e_nm,
                                      inst_ei->nm_offset,
                                      pentry,
                                      nm_ent));
            pentry = nm_ent;
            req_ltid = 0;
        }
        SHR_IF_ERR_EXIT
            (bcmptm_cmdproc_write(unit,
                                  req_ctxt->req_flags,
                                  0,
                                  inst_ei->view_info->sid[0],
                                  &dyn_info,
                                  NULL,
                                  pentry,
                                  TRUE,
                                  TRUE,
                                  TRUE,
                                  BCMPTM_RM_OP_NORMAL, NULL,
                                  req_ltid,
                                  req_info->dfield_format_id,
                                  &wr_rsp_flags));
        if (inst_ei->view_info->hit_info != NULL) {
            /* LT has HIT fields and these HIT fields should be updated as well. */
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_hit_table_update(unit,
                                                 req_ctxt,
                                                 &dyn_info,
                                                 inst_ei->view_info->hit_info,
                                                 hit_list,
                                                 hit_cnt));
        }
        if ((inst_ei->view_info->lp_sid != NULL) &&
            (map_ei->e_attr.e_nm != BCMPTM_RM_HASH_ENTRY_NARROW_MODE_HALF) &&
            (map_ei->e_attr.e_nm != BCMPTM_RM_HASH_ENTRY_NARROW_MODE_THIRD)) {
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_lp_ent_update(unit,
                                              req_ctxt->pt_dyn_info,
                                              *inst_ei->key_fmt->single_view_sid,
                                              *inst_ei->key_fmt->key_attr_sid,
                                              *inst_ei->view_info->lp_sid,
                                              map_ei->e_attr.e_bm,
                                              inst_ei->view_info->num_base_entries,
                                              pt_info->base_bkt_cnt[inst_ei->rbank],
                                              inst_ei->index));
        }
    }
    SHR_IF_ERR_EXIT
        (rm_hash_ent_install_report(unit,
                                    pt_dyn_info->tbl_inst,
                                    lt_ctrl,
                                    map_ei));

exit:
    if (ent_buf != NULL) {
        bcmptm_rm_hash_entry_buf_free(ent_buf);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Determine if entry can be replaced directly.
 *
 * \param [in] unit Unit number.
 * \param [in] req_info Pointer to bcmptm_rm_hash_req_t structure.
 * \param [in] lt_ctrl Pointer to rm_hash_map_srch_result_t structure.
 * \param [in] map_sr Pointer to rm_hash_map_srch_result_t structure.
 * \param [out] replaceable Buffer to hold the result.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_entry_replaceable(int unit,
                          bcmptm_rm_hash_req_t *req_info,
                          rm_hash_lt_ctrl_t *lt_ctrl,
                          rm_hash_map_srch_result_t *map_sr,
                          bool *replaceable)
{
    const bcmptm_rm_hash_view_info_t *view_info = NULL;
    uint8_t e_size;
    rm_hash_inst_srch_result_t *inst_sr = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (rm_hash_req_ent_size_get(unit, req_info, &lt_ctrl->lt_info, &e_size));
    /*
     * View info for different instances is the same.
     * Just need to check the first instance.
     */
    inst_sr = &map_sr->inst_srch_result[0];
    view_info = inst_sr->view_info;
    if (e_size == view_info->num_base_entries) {
        *replaceable = TRUE;
        SHR_EXIT();
    } else {
        *replaceable = FALSE;
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Try to replace an entry.
 *
 * \param [in] unit Unit number.
 * \param [in] req_ctxt Pointer to rm_hash_req_context_t structure.
 * \param [in] lt_ctrl Pointer to rm_hash_map_srch_result_t structure.
 * \param [in] pmap_sr Pointer to rm_hash_map_srch_result_t structure.
 * \param [in] pipe Pipe number.
 * \param [out] rsp_info Pointer to bcmptm_rm_hash_rsp_t structure.
 * \param [out] rsp_ltid Pointer to bcmltd_sid_t object.
 * \param [out] rsp_flags Pointer to response flags.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_replace(int unit,
                rm_hash_req_context_t *req_ctxt,
                rm_hash_lt_ctrl_t *lt_ctrl,
                rm_hash_glb_srch_info_t *pglb_si,
                rm_hash_map_srch_result_t *pmap_sr,
                uint8_t pipe,
                bcmptm_rm_hash_rsp_t *rsp_info,
                bcmltd_sid_t *rsp_ltid,
                uint32_t *rsp_flags)
{
    bool replaceable = FALSE;
    uint8_t e_idx;
    bcmptm_rm_hash_req_t *req_info = req_ctxt->req_info;
    uint32_t rsp_entry_index[BCMPTM_RM_HASH_RSP_MAX_INST_CNT];
    bcmdrd_sid_t rsp_entry_sid[BCMPTM_RM_HASH_RSP_MAX_INST_CNT];

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (rm_hash_entry_replaceable(unit,
                                   req_info,
                                   lt_ctrl,
                                   pmap_sr,
                                   &replaceable));
    if (replaceable == TRUE) {
        SHR_IF_ERR_EXIT
            (rm_hash_ent_replace(unit, req_ctxt, lt_ctrl, pglb_si, pmap_sr));
        for (e_idx = 0; e_idx < pmap_sr->num_insts; e_idx++) {
            rsp_entry_index[e_idx] = pmap_sr->inst_srch_result[e_idx].e_idx;
            rsp_entry_sid[e_idx] = pmap_sr->inst_srch_result[e_idx].sid;
        }
        rm_hash_rsp_info_set(unit,
                             pmap_sr->num_insts,
                             rsp_entry_index,
                             rsp_entry_sid,
                             req_info->dfield_format_id,
                             pipe,
                             rsp_info);
        *rsp_ltid = req_ctxt->req_ltid;
        *rsp_flags = BCMPTM_RSP_FLAGS_REPLACED;
        SHR_EXIT();
    }
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Try to insert an entry without entry move.
 *
 * \param [in] unit Unit number.
 * \param [in] req_ctxt Pointer to rm_hash_req_context_t structure.
 * \param [in] lt_ctrl Pointer to rm_hash_map_srch_result_t structure.
 * \param [in] pglb_si Pointer to rm_hash_glb_srch_info_t structure.
 * \param [in] pmap_sr Pointer to rm_hash_map_srch_result_t structure.
 * \param [in] pipe Pipe number.
 * \param [out] rsp_info Pointer to bcmptm_rm_hash_rsp_t structure.
 * \param [out] rsp_ltid Pointer to bcmltd_sid_t object.
 * \param [out] rsp_flags Pointer to response flags.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_insert_without_ent_move(int unit,
                                rm_hash_req_context_t *req_ctxt,
                                rm_hash_lt_ctrl_t *lt_ctrl,
                                rm_hash_glb_srch_info_t *pglb_si,
                                rm_hash_map_srch_result_t *pmap_sr,
                                uint8_t pipe,
                                bcmptm_rm_hash_rsp_t *rsp_info,
                                bcmltd_sid_t *rsp_ltid,
                                uint32_t *rsp_flags)
{
    bool free_slot_found = FALSE;
    rm_hash_map_entry_info_t map_ei;
    uint64_t flags = 0;
    uint8_t e_idx;
    bcmptm_rm_hash_req_t *req_info = req_ctxt->req_info;
    uint32_t rsp_entry_index[BCMPTM_RM_HASH_RSP_MAX_INST_CNT];
    bcmdrd_sid_t rsp_entry_sid[BCMPTM_RM_HASH_RSP_MAX_INST_CNT];

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (rm_hash_grp_free_slot_get(unit,
                                   req_ctxt,
                                   lt_ctrl,
                                   pglb_si,
                                   &free_slot_found,
                                   &map_ei));
    if (free_slot_found == TRUE) {
        SHR_IF_ERR_EXIT
            (rm_hash_ent_install(unit, req_ctxt, lt_ctrl, pglb_si, &map_ei));
        if (pmap_sr->e_exist == TRUE) {
            SHR_IF_ERR_EXIT
                (rm_hash_ent_remove(unit, req_ctxt, pglb_si, pmap_sr));
            flags = BCMPTM_RSP_FLAGS_REPLACED;
        }
        for (e_idx = 0; e_idx < map_ei.num_insts; e_idx++) {
            rsp_entry_index[e_idx] = map_ei.inst_ei[e_idx].index;
            rsp_entry_sid[e_idx] = map_ei.inst_ei[e_idx].view_info->sid[0];
        }
        rm_hash_rsp_info_set(unit,
                             map_ei.num_insts,
                             rsp_entry_index,
                             rsp_entry_sid,
                             req_info->dfield_format_id,
                             pipe,
                             rsp_info);
        *rsp_ltid = req_ctxt->req_ltid;
        *rsp_flags = flags;
        SHR_EXIT();
    }
    SHR_ERR_EXIT(SHR_E_RESOURCE);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Try to insert an entry via entry move.
 *
 * \param [in] unit Unit number.
 * \param [in] req_ctxt Pointer to rm_hash_req_context_t structure.
 * \param [in] lt_ctrl Pointer to rm_hash_map_srch_result_t structure.
 * \param [in] pglb_si Pointer to rm_hash_glb_srch_info_t structure.
 * \param [in] pmap_sr Pointer to rm_hash_map_srch_result_t structure.
 * \param [in] pipe Pipe number.
 * \param [out] rsp_info Pointer to bcmptm_rm_hash_rsp_t structure.
 * \param [out] rsp_ltid Pointer to bcmltd_sid_t object.
 * \param [out] rsp_flags Pointer to response flags.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_insert_with_multi_map_move(int unit,
                                   rm_hash_req_context_t *req_ctxt,
                                   rm_hash_lt_ctrl_t *lt_ctrl,
                                   rm_hash_glb_srch_info_t *pglb_si,
                                   rm_hash_map_srch_result_t *pmap_sr,
                                   uint8_t pipe,
                                   bcmptm_rm_hash_rsp_t *rsp_info,
                                   bcmltd_sid_t *rsp_ltid,
                                   uint32_t *rsp_flags)
{
    uint8_t map_idx, idx, grp_idx, fmt_idx, e_idx;
    rm_hash_ent_slb_info_t e_slb_il[RM_HASH_MAX_GRP_INST_COUNT];
    rm_hash_ent_loc_t e_loc_list[RM_HASH_MAX_GRP_INST_COUNT];
    uint8_t rbank_list[RM_HASH_MAX_GRP_INST_COUNT], rbank, e_size;
    rm_hash_map_srch_info_t *map_si = NULL;
    rm_hash_pt_info_t *pt_info;
    uint32_t slb;
    bcmbd_pt_dyn_info_t *pt_dyn_info = req_ctxt->pt_dyn_info;
    int rv = SHR_E_NONE;
    rm_hash_map_entry_info_t map_ei;
    rm_hash_fmt_info_t *fmt_info = NULL;
    const bcmptm_rm_hash_key_format_t *key_format = NULL;
    uint64_t flags = 0;
    uint32_t rsp_entry_index[BCMPTM_RM_HASH_RSP_MAX_INST_CNT];
    bcmdrd_sid_t rsp_entry_sid[BCMPTM_RM_HASH_RSP_MAX_INST_CNT];

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_pt_info_get(unit, &pt_info));
    SHR_IF_ERR_EXIT
        (rm_hash_req_ent_size_get(unit, req_ctxt->req_info, &lt_ctrl->lt_info,
                                  &e_size));

    map_ei.e_attr.e_size = e_size;
    map_ei.e_attr.e_bm = pglb_si->map_srch_info[0].inst_srch_info[0].e_bm;
    map_ei.e_attr.e_nm = pglb_si->map_srch_info[0].inst_srch_info[0].e_nm;
    for (map_idx = 0; map_idx < lt_ctrl->num_maps; map_idx++) {
        map_si = &pglb_si->map_srch_info[map_idx];
        for (idx = 0; idx < map_si->num_insts; idx++) {
            e_slb_il[idx] = map_si->inst_srch_info[idx].slb_info_list[0];
            e_slb_il[idx].map = map_idx;
            if (map_si->num_insts > 1) {
                e_slb_il[idx].map |= RM_HASH_MULTI_INST_GRP;
            }
        }
        slb = map_si->inst_srch_info[0].slb_info_list[0].slb;
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_slb_to_rbank_get(unit, pt_info, slb, &rbank));
        if ((pt_dyn_info->tbl_inst == -1) &&
            (pt_info->bank_mode[rbank] == RM_HASH_BANK_MODE_MIXED)) {
            continue;
        }
        sal_memset(e_loc_list, 0, sizeof(e_loc_list));
        sal_memset(rbank_list, 0, sizeof(rbank_list));
        rv = bcmptm_rm_hash_ent_move(unit,
                                     req_ctxt,
                                     pt_info,
                                     lt_ctrl->move_depth,
                                     map_si->num_insts,
                                     e_slb_il,
                                     map_ei.e_attr.e_size,
                                     map_ei.e_attr.e_bm,
                                     e_loc_list,
                                     rbank_list);
        if (rv == SHR_E_NONE) {
            uint8_t index;
            /* Some free slot has been found via moving existing entries. */
            map_ei.map = map_idx;
            map_ei.num_insts = map_si->num_insts;
            for (index = 0; index < map_si->num_insts; index++) {
                map_ei.inst_ei[index].slb = e_slb_il[index].slb;
                map_ei.inst_ei[index].loc = e_loc_list[index];
                map_ei.inst_ei[index].rbank = rbank_list[index];
                grp_idx = lt_ctrl->map_info[map_idx].grp_idx[index];
                fmt_idx = lt_ctrl->map_info[map_idx].fmt_idx[index];
                fmt_info = &lt_ctrl->fmt_info[fmt_idx];
                key_format = fmt_info->key_format;
                SHR_NULL_CHECK(key_format, SHR_E_PARAM);
                if (key_format == NULL) {
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
                map_ei.inst_ei[index].key_fmt = key_format;
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_size_based_view_info_get(unit,
                                                             &lt_ctrl->lt_info,
                                                             grp_idx,
                                                             key_format,
                                                             map_ei.e_attr.e_size,
                                                             &map_ei.inst_ei[index].view_info));
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_ent_index_get(unit,
                                                  pt_info,
                                                  map_ei.inst_ei[index].slb,
                                                  &map_ei.inst_ei[index].loc,
                                                  map_ei.e_attr.e_size,
                                                  &map_ei.inst_ei[index].rbank,
                                                  &map_ei.inst_ei[index].index));
            }
            break;
        }
    }
    if (map_idx == lt_ctrl->num_maps) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    SHR_IF_ERR_EXIT
        (rm_hash_ent_install(unit, req_ctxt, lt_ctrl, pglb_si, &map_ei));
    if (pmap_sr->e_exist == TRUE) {
        SHR_IF_ERR_EXIT
            (rm_hash_ent_remove(unit, req_ctxt, pglb_si, pmap_sr));
        flags = BCMPTM_RSP_FLAGS_REPLACED;
    }
    for (e_idx = 0; e_idx < map_ei.num_insts; e_idx++) {
        rsp_entry_index[e_idx] = map_ei.inst_ei[e_idx].index;
        rsp_entry_sid[e_idx] = map_ei.inst_ei[e_idx].view_info->sid[0];
    }
    rm_hash_rsp_info_set(unit,
                         map_ei.num_insts,
                         rsp_entry_index,
                         rsp_entry_sid,
                         req_ctxt->req_info->dfield_format_id,
                         pipe,
                         rsp_info);
    *rsp_ltid = req_ctxt->req_ltid;
    *rsp_flags = flags;
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Try to insert an entry via entry move.
 *
 * \param [in] unit Unit number.
 * \param [in] req_ctxt Pointer to rm_hash_req_context_t structure.
 * \param [in] lt_ctrl Pointer to rm_hash_map_srch_result_t structure.
 * \param [in] pglb_si Pointer to rm_hash_glb_srch_info_t structure.
 * \param [in] pmap_sr Pointer to rm_hash_map_srch_result_t structure.
 * \param [in] pipe Pipe number.
 * \param [out] rsp_info Pointer to bcmptm_rm_hash_rsp_t structure.
 * \param [out] rsp_ltid Pointer to bcmltd_sid_t object.
 * \param [out] rsp_flags Pointer to response flags.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_insert_with_single_map_move(int unit,
                                    rm_hash_req_context_t *req_ctxt,
                                    rm_hash_lt_ctrl_t *lt_ctrl,
                                    rm_hash_glb_srch_info_t *pglb_si,
                                    rm_hash_map_srch_result_t *pmap_sr,
                                    uint8_t pipe,
                                    bcmptm_rm_hash_rsp_t *rsp_info,
                                    bcmltd_sid_t *rsp_ltid,
                                    uint32_t *rsp_flags)
{
    rm_hash_fmt_info_t *fmt_info = NULL;
    bcmptm_rm_hash_req_t *req_info = req_ctxt->req_info;
    rm_hash_ent_slb_info_t *slb_info = NULL;
    uint8_t num_slb_info, slb_idx, e_idx, e_size, grp_idx;
    int rv = SHR_E_NONE;
    rm_hash_pt_info_t *pt_info;
    rm_hash_map_entry_info_t map_ei;
    const bcmptm_rm_hash_key_format_t *key_format = NULL;
    uint32_t rsp_entry_index[BCMPTM_RM_HASH_RSP_MAX_INST_CNT];
    bcmdrd_sid_t rsp_entry_sid[BCMPTM_RM_HASH_RSP_MAX_INST_CNT];
    uint64_t flags = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_pt_info_get(unit, &pt_info));
    SHR_IF_ERR_EXIT
        (rm_hash_req_ent_size_get(unit, req_ctxt->req_info, &lt_ctrl->lt_info,
                                  &e_size));

    map_ei.e_attr.e_size = e_size;
    map_ei.e_attr.e_bm = pglb_si->map_srch_info[0].inst_srch_info[0].e_bm;
    map_ei.e_attr.e_nm = pglb_si->map_srch_info[0].inst_srch_info[0].e_nm;
    slb_info = pglb_si->map_srch_info[0].inst_srch_info[0].slb_info_list;
    /* Find the fmt info based on the key format provided by the LTM. */
    grp_idx = lt_ctrl->map_info[0].grp_idx[0];
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_fmt_info_get(unit,
                                     req_ctxt->req_info->entry_attrs,
                                     lt_ctrl,
                                     grp_idx,
                                     &fmt_info));
    num_slb_info = fmt_info->num_en_rbanks;
    for (slb_idx = 0; slb_idx < num_slb_info; slb_idx++) {
        rv = bcmptm_rm_hash_ent_move(unit,
                                     req_ctxt,
                                     pt_info,
                                     lt_ctrl->move_depth,
                                     1,
                                     &slb_info[slb_idx],
                                     map_ei.e_attr.e_size,
                                     map_ei.e_attr.e_bm,
                                     &map_ei.inst_ei[0].loc,
                                     &map_ei.inst_ei[0].rbank);
        if (rv == SHR_E_NONE) {
            /* Some free slot has been found via moving existing entries. */
            break;
        }
    }

    if (slb_idx == num_slb_info) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }
    key_format = fmt_info->key_format;
    map_ei.map = 0;
    map_ei.num_insts = 1;
    map_ei.inst_ei[0].slb = slb_info[slb_idx].slb;
    map_ei.inst_ei[0].key_fmt = key_format;
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_exact_view_get(unit,
                                           req_info->entry_attrs,
                                           &lt_ctrl->lt_info,
                                           grp_idx,
                                           req_info->dfield_format_id,
                                           &map_ei.inst_ei[0].view_info));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_index_get(unit,
                                      pt_info,
                                      map_ei.inst_ei[0].slb,
                                      &map_ei.inst_ei[0].loc,
                                      map_ei.e_attr.e_size,
                                      NULL,
                                      &map_ei.inst_ei[0].index));
    SHR_IF_ERR_EXIT
        (rm_hash_ent_install(unit,
                             req_ctxt,
                             lt_ctrl,
                             pglb_si,
                             &map_ei));
    if (pmap_sr->e_exist == TRUE) {
        SHR_IF_ERR_EXIT
            (rm_hash_ent_remove(unit, req_ctxt, pglb_si, pmap_sr));
        flags = BCMPTM_RSP_FLAGS_REPLACED;
    }
    for (e_idx = 0; e_idx < map_ei.num_insts; e_idx++) {
        rsp_entry_index[e_idx] = map_ei.inst_ei[e_idx].index;
        rsp_entry_sid[e_idx] = map_ei.inst_ei[e_idx].view_info->sid[0];
    }
    rm_hash_rsp_info_set(unit,
                         map_ei.num_insts,
                         rsp_entry_index,
                         rsp_entry_sid,
                         req_info->dfield_format_id,
                         pipe,
                         rsp_info);
    *rsp_ltid = req_ctxt->req_ltid;
    *rsp_flags = flags;
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Hash insert request handler
 *
 * \param [in] unit Unit number.
 * \param [in] req_ctxt Context information for the requested operation.
 * \param [in] rsp_entry_buf_wsize Size of response entry buffer.
 * \param [out] rsp_entry_words Pointer to a buffer for recording response entry.
 * \param [out] rsp_info Pointer to bcmptm_rm_hash_rsp_t structure.
 * \param [out] rsp_ltid Pointer to bcmltd_sid_t object.
 * \param [out] rsp_flags Pointer to response flags.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_insert(int unit,
               rm_hash_req_context_t *req_ctxt,
               size_t rsp_entry_buf_wsize,
               uint32_t *rsp_entry_words,
               bcmptm_rm_hash_rsp_t *rsp_info,
               bcmltd_sid_t *rsp_ltid,
               uint32_t *rsp_flags)
{
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    bcmptm_rm_hash_req_t *req_info = req_ctxt->req_info;
    rm_hash_glb_srch_info_t *pglb_si = NULL;
    rm_hash_map_srch_result_t *pmap_sr = NULL;
    bool separate = FALSE;
    uint8_t pipe;
    int rv;
    bcmbd_pt_dyn_info_t *pt_dyn_info = req_ctxt->pt_dyn_info;

    SHR_FUNC_ENTER(unit);

    pipe = (pt_dyn_info->tbl_inst == -1)? 0 : pt_dyn_info->tbl_inst;
    /* Get lt contrl structure for this LTID. */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_lt_ctrl_get(unit, req_ctxt->req_ltid, &lt_ctrl));
    SHR_NULL_CHECK(lt_ctrl, SHR_E_PARAM);

    if (req_info->same_key == TRUE) {
        /*
         * An entry lookup operation has been performed before this insert
         * operation, and the entry physical, logical bucket list and search
         * result has been saved, no matter if there exists matched entry.
         * In this case, the search result will be used directly to avoid
         * hash vector re-computing in the following insert operation.
         */
        pglb_si = &cached_glb_srch_info[unit];
        pmap_sr = &cached_map_srch_result[unit];
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit, "No LOOKUP preceding entry INSERT!!\n")));
        pglb_si = bcmptm_rm_hash_glb_srch_info_alloc();
        pmap_sr = bcmptm_rm_hash_map_srch_result_alloc();
        if ((pglb_si == NULL) || (pmap_sr == NULL)) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        sal_memset(pglb_si, 0, sizeof(*pglb_si));
        sal_memset(pmap_sr, 0, sizeof(*pmap_sr));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_ent_search(unit, req_ctxt, lt_ctrl, pglb_si,
                                       pmap_sr));
    }

    if (pmap_sr->e_exist == TRUE) {
        rv = rm_hash_replace(unit, req_ctxt, lt_ctrl, pglb_si, pmap_sr, pipe,
                             rsp_info, rsp_ltid, rsp_flags);
        if (rv == SHR_E_NONE) {
            SHR_EXIT();
        } else if (rv != SHR_E_NOT_FOUND) {
            SHR_IF_ERR_EXIT(rv);
        }
    }
    rv = rm_hash_insert_without_ent_move(unit, req_ctxt, lt_ctrl, pglb_si,
                                         pmap_sr, pipe, rsp_info,
                                         rsp_ltid, rsp_flags);
    if (rv == SHR_E_NONE) {
        SHR_EXIT();
    } else if (rv != SHR_E_RESOURCE) {
        SHR_IF_ERR_EXIT(rv);
    }

    if ((rv == SHR_E_RESOURCE) &&
        ((pglb_si->map_srch_info[0].inst_srch_info[0].e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_HALF) ||
         (pglb_si->map_srch_info[0].inst_srch_info[0].e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_THIRD))) {
        SHR_IF_ERR_EXIT(rv);
    }
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_bank_has_separated_sid(unit, &separate));
    if (separate == TRUE) {
        rv = rm_hash_insert_with_multi_map_move(unit, req_ctxt, lt_ctrl, pglb_si,
                                                pmap_sr, pipe, rsp_info, rsp_ltid,
                                                rsp_flags);
    } else {
        rv = rm_hash_insert_with_single_map_move(unit, req_ctxt, lt_ctrl, pglb_si,
                                                 pmap_sr, pipe, rsp_info, rsp_ltid,
                                                 rsp_flags);
    }
    if (rv == SHR_E_NONE) {
        SHR_EXIT();
    } else {
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    if (req_info->same_key == FALSE) {
        if (pglb_si != NULL) {
            bcmptm_rm_hash_glb_srch_info_free(pglb_si);
        }
        if (pmap_sr != NULL) {
            bcmptm_rm_hash_map_srch_result_free(pmap_sr);
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the requested context information.
 *
 * \param [in] req_flags Flags from PTM.
 * \param [in] req_ltid Requested ltid.
 * \param [in] pt_dyn_info Dynamic information.
 * \param [in] req_info Pointer to bcmptm_rm_hash_req_t structure.
 * \param [out] req_ctxt Requested context information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static void
rm_hash_req_ctxt_init(uint64_t req_flags,
                      bcmltd_sid_t req_ltid,
                      bcmbd_pt_dyn_info_t *pt_dyn_info,
                      bcmptm_rm_hash_req_t *req_info,
                      rm_hash_req_context_t *req_ctxt)
{
    req_ctxt->req_flags = req_flags;
    req_ctxt->pt_dyn_info = pt_dyn_info;
    req_ctxt->req_ltid = req_ltid;
    req_ctxt->req_info = req_info;
}

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_hash_lookup(int unit,
                      uint64_t req_flags,
                      bcmltd_sid_t req_ltid,
                      bcmbd_pt_dyn_info_t *pt_dyn_info,
                      bcmptm_rm_hash_req_t *req_info,
                      size_t rsp_entry_buf_wsize,
                      uint32_t *rsp_entry_words,
                      bcmptm_rm_hash_rsp_t *rsp_info,
                      bcmltd_sid_t *rsp_ltid,
                      uint32_t *rsp_flags,
                      uint8_t *map_index,
                      rm_hash_nme_info_t *nme_info)
{
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    rm_hash_lt_flex_ctr_info_t *fc_info;
    rm_hash_glb_srch_info_t *glb_srch_info = &cached_glb_srch_info[unit];
    rm_hash_map_srch_result_t *map_sr = &cached_map_srch_result[unit];
    rm_hash_inst_srch_result_t *inst_sr = NULL;
    uint8_t e_size, inst, e_idx, pipe;
    rm_hash_req_context_t req_ctxt;
    uint32_t ctr_offset = 0, rsp_entry_index[BCMPTM_RM_HASH_RSP_MAX_INST_CNT];
    bcmdrd_sid_t rsp_entry_sid[BCMPTM_RM_HASH_RSP_MAX_INST_CNT];

    SHR_FUNC_ENTER(unit);

    rm_hash_req_ctxt_init(req_flags,
                          req_ltid,
                          pt_dyn_info,
                          req_info,
                          &req_ctxt);
    /* Get lt contrl structure for this LTID. */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_lt_ctrl_get(unit, req_ctxt.req_ltid, &lt_ctrl));
    SHR_NULL_CHECK(lt_ctrl, SHR_E_PARAM);
    fc_info = &lt_ctrl->flex_ctr_info;

    pipe = (pt_dyn_info->tbl_inst == -1)? 0 : pt_dyn_info->tbl_inst;
    /* Search all the groups to determine if the entry exists. */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_search(unit,
                                   &req_ctxt,
                                   lt_ctrl,
                                   glb_srch_info,
                                   map_sr));

    if (map_sr->e_exist == FALSE) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (req_flags & BCMLT_ENT_ATTR_GET_FROM_HW) {
        for (inst = 0; inst < map_sr->num_insts; inst++) {
            inst_sr = &map_sr->inst_srch_result[inst];
            if (inst_sr->view_info->hit_info != NULL) {
                bcmbd_pt_dyn_info_t dyn_info;
                dyn_info = *req_ctxt.pt_dyn_info;
                dyn_info.index = inst_sr->e_idx;
                /*
                 * Get hit bit information for those hash tables which have
                 * separated physical hit tables.
                 */
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_hit_bit_get(unit,
                                                &req_ctxt,
                                                &dyn_info,
                                                inst_sr->view_info->hit_info,
                                                inst_sr->e_words));
            }
        }
    }
    for (e_idx = 0; e_idx < map_sr->num_insts; e_idx++) {
        inst_sr = &map_sr->inst_srch_result[e_idx];
        rsp_entry_index[e_idx] = inst_sr->e_idx;
        rsp_entry_sid[e_idx] = inst_sr->sid;

        if (fc_info->entry_index_cb != NULL) {
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_flex_ctr_offset_get(unit,
                                                    lt_ctrl,
                                                    inst_sr->sid,
                                                    pt_dyn_info->tbl_inst,
                                                    inst_sr->e_idx,
                                                    inst_sr->view_info->num_base_entries,
                                                    &ctr_offset));
            rsp_info->rsp_entry_ctr_offset[e_idx] = ctr_offset;
        }
    }
    rm_hash_rsp_info_set(unit,
                         map_sr->num_insts,
                         rsp_entry_index,
                         rsp_entry_sid,
                         map_sr->inst_srch_result[0].ldtype_val,
                         pipe,
                         rsp_info);

    e_size = bcmdrd_pt_entry_wsize(unit, map_sr->inst_srch_result[0].sid);
    if (rsp_entry_buf_wsize < e_size) {
         SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (fc_info->entry_index_cb != NULL) {
        bcmptm_rm_hash_ent_hit_index_info_set(unit,
                                              lt_ctrl,
                                              pt_dyn_info->tbl_inst,
                                              map_sr);
    }
    if (((lt_ctrl->key_field_exchange == TRUE) ||
         (lt_ctrl->data_field_exchange == TRUE)) && (map_sr->map_idx > 0)) {
        /* Perform key and data field exchange to the format LTM expects. */
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_key_fields_exchange(unit,
                                                req_info->entry_attrs,
                                                &lt_ctrl->lt_info,
                                                map_sr->map_idx,
                                                0,
                                                map_sr->inst_srch_result[0].e_words,
                                                rsp_entry_words));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_data_fields_exchange(unit,
                                                 &lt_ctrl->lt_info,
                                                 req_info->dfield_format_id,
                                                 map_sr->map_idx,
                                                 0,
                                                 map_sr->inst_srch_result[0].e_words,
                                                 rsp_entry_words));
    } else {
        sal_memcpy(rsp_entry_words, map_sr->inst_srch_result[0].e_words, e_size * 4);
    }

    *rsp_ltid = req_ltid;
    *rsp_flags = BCMPTM_RSP_FLAGS_NO_FLAGS;
    *map_index = map_sr->map_idx;
    *nme_info = map_sr->inst_srch_result[0].nme_info;
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

int bcmptm_rm_hash_req(int unit,
                       uint64_t req_flags,
                       uint32_t cseq_id,
                       bcmltd_sid_t req_ltid,
                       bcmbd_pt_dyn_info_t *pt_dyn_info,
                       bcmptm_op_type_t req_op,
                       bcmptm_rm_hash_req_t *req_info,
                       size_t rsp_entry_buf_wsize,

                       uint32_t *rsp_entry_words,
                       bcmptm_rm_hash_rsp_t *rsp_info,
                       bcmltd_sid_t *rsp_ltid,
                       uint32_t *rsp_flags)
{
    int tmp_rv;
    rm_hash_req_context_t req_ctxt;
    bcmptm_table_info_t *table_info = NULL;
    uint32_t table_size = 0;
    uint8_t map_index = 0;
    rm_hash_nme_info_t nme_info;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    SHR_NULL_CHECK(req_info, SHR_E_PARAM);
    SHR_NULL_CHECK(req_info->entry_words, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_entry_words, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);
    rm_hash_req_ctxt_init(req_flags,
                              req_ltid,
                              pt_dyn_info,
                              req_info,
                              &req_ctxt);
    switch (req_op) {
    case BCMPTM_OP_LOOKUP:
        tmp_rv = bcmptm_rm_hash_lookup(unit,
                                       req_flags,
                                       req_ltid,
                                       pt_dyn_info,
                                       req_info,
                                       rsp_entry_buf_wsize,
                                       rsp_entry_words,
                                       rsp_info,
                                       rsp_ltid,
                                       rsp_flags,
                                       &map_index,
                                       &nme_info);
        if (tmp_rv == SHR_E_NOT_FOUND) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        } else {
            SHR_IF_ERR_EXIT(tmp_rv);
        }
        break;
    case BCMPTM_OP_INSERT:
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_trans_ent_req_cmd(unit, cseq_id));
        tmp_rv = rm_hash_insert(unit,
                                &req_ctxt,
                                rsp_entry_buf_wsize,
                                rsp_entry_words,
                                rsp_info,
                                rsp_ltid,
                                rsp_flags);
        if (tmp_rv == SHR_E_RESOURCE || tmp_rv == SHR_E_MEMORY) {
            SHR_ERR_EXIT(tmp_rv);
        } else {
            SHR_IF_ERR_EXIT(tmp_rv);
        }
        break;
    case BCMPTM_OP_DELETE:
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_trans_ent_req_cmd(unit, cseq_id));
        tmp_rv = rm_hash_delete(unit,
                                &req_ctxt,
                                rsp_entry_buf_wsize,
                                rsp_entry_words,
                                rsp_info,
                                rsp_ltid,
                                rsp_flags);
        if (tmp_rv == SHR_E_NOT_FOUND) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        } else {
            SHR_IF_ERR_EXIT(tmp_rv);
        }
        break;
    case BCMPTM_OP_GET_FIRST:
        tmp_rv = bcmptm_rm_hash_travs_cmd_proc(unit,
                                               req_flags,
                                               cseq_id,
                                               req_ltid,
                                               pt_dyn_info,
                                               BCMPTM_OP_GET_FIRST,
                                               req_info,
                                               rsp_entry_buf_wsize,
                                               rsp_entry_words,
                                               rsp_info,
                                               rsp_ltid,
                                               rsp_flags);
        if (tmp_rv == SHR_E_NOT_FOUND) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        } else {
            SHR_IF_ERR_EXIT(tmp_rv);
        }
        break;
    case BCMPTM_OP_GET_NEXT:
        tmp_rv = bcmptm_rm_hash_travs_cmd_proc(unit,
                                               req_flags,
                                               cseq_id,
                                               req_ltid,
                                               pt_dyn_info,
                                               BCMPTM_OP_GET_NEXT,
                                               req_info,
                                               rsp_entry_buf_wsize,
                                               rsp_entry_words,
                                               rsp_info,
                                               rsp_ltid,
                                               rsp_flags);
        if ((tmp_rv == SHR_E_NOT_FOUND) || (tmp_rv == SHR_E_UNAVAIL)) {
            SHR_ERR_EXIT(tmp_rv);
        } else {
            SHR_IF_ERR_EXIT(tmp_rv);
        }
        break;
    case BCMPTM_OP_GET_TABLE_INFO:
        SHR_NULL_CHECK(rsp_info->rsp_misc, SHR_E_PARAM);
        table_info = (bcmptm_table_info_t *)rsp_info->rsp_misc;
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_table_info_get(unit, req_ltid, &table_size));
        if ((table_info->entry_limit == 0) ||
            (table_size < table_info->entry_limit)) {
            table_info->entry_limit = table_size;
        }
        break;
    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}
