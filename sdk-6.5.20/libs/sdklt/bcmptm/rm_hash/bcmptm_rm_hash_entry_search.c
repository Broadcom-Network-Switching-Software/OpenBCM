/*! \file bcmptm_rm_hash_entry_search.c
 *
 * Resource Manager for Hash Table
 *
 * This file contains the routines that implement entry search for a given entry
 * key. In hash resource manager, searching and determining if there exists a
 * matched entry for a given key is a fundamental operation upon which the basic
 * hash table operation LOOKUP, INSERT and DELETE will rely.
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
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include "bcmptm_rm_hash.h"
#include "bcmptm_rm_hash_key_extract.h"
#include "bcmptm_rm_hash_utils.h"
#include "bcmptm_rm_hash_vec_compute.h"
#include "bcmptm_rm_hash_state_mgmt.h"
#include "bcmptm_rm_hash_entry_search.h"
#include "bcmptm_rm_hash_robust_hash.h"
#include "bcmptm_rm_hash_lmem.h"
#include "bcmptm_rm_hash_pt_register.h"
#include "bcmptm_rm_hash_narrow_mode_mgmt.h"
#include "bcmptm_rm_hash_store.h"
#include "bcmptm_rm_hash_imm.h"

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

static uint32_t
rm_hash_vec_hash_store_32_bits(uint64_t vec, uint32_t bit_sel)
{
    uint32_t n = 0, idx = 0, bit_pos = 0, bit_val;

    for (idx = 0; idx < 32; idx++) {
        bit_pos = 2 * idx + bit_sel;
        bit_val = (vec & (1UL << bit_pos)) >> bit_pos;
        if (bit_val) {
            n |= 1 << idx;
        }
    }

    return n;
}

static uint32_t
rm_hash_vec_hash_store_16_bits(uint64_t vec, uint32_t bit_sel)
{
    uint32_t n = 0, idx = 0, bit_pos = 0, bit_val;

    for (idx = 0; idx < 16; idx++) {
        bit_pos = 4 * idx + bit_sel;
        bit_val = (vec & (1UL << bit_pos)) >> bit_pos;
        if (bit_val) {
            n |= 1 << idx;
        }
    }

    return n;
}

static int
rm_hash_store_attri_set(int unit,
                        bcmptm_rm_hash_dynamic_entry_attrs_t *e_attrs,
                        rm_hash_inst_srch_info_t *inst_si)
{
    bool collision = FALSE;

    SHR_FUNC_ENTER(unit);

    if (e_attrs == NULL) {
        SHR_EXIT();
    }

    inst_si->hash_store_en = e_attrs->hstore_attr.enable;
    if (inst_si->hash_store_en) {
        inst_si->hash_store_key_fields =
            e_attrs->hstore_attr.hash_store_key_fields;
        switch (e_attrs->hstore_attr.store_type) {
        case BCMPTM_RM_HASH_STORE_TYPE_16BIT_KEY_LENGTH:
            inst_si->hash_store_val = rm_hash_vec_hash_store_16_bits(
                inst_si->hash_vector->vec,
                e_attrs->hstore_attr.bit_sel_type);
            break;
        case BCMPTM_RM_HASH_STORE_TYPE_32BIT_KEY_LENGTH:
            inst_si->hash_store_val = rm_hash_vec_hash_store_32_bits(
                inst_si->hash_vector->vec,
                e_attrs->hstore_attr.bit_sel_type);
            break;
        default:
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_hash_collision_check(unit,
                                            inst_si->ltid,
                                            inst_si->hash_store_val,
                                            inst_si->hash_vector->full_key,
                                            &collision));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_hash_store_info_update(unit,
                                              inst_si->ltid,
                                              inst_si->hash_store_val));
        if (collision) {
            SHR_ERR_EXIT(SHR_E_COLL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Determine whether two entries have same key values.
 *
 * This function extracts the key fields values from the entry buffer, and
 * compare whether they are identical.
 *
 * \param [in] entry_a Entry a to be compared.
 * \param [in] entry_b Entry b to be compared.
 * \param [in] key_fields Pointer to bcmlrd_hw_field_list_t structure
 *             corresponding to a LT.
 *
 * \retval TRUE The two entries have same key values.
 * \retval FALSE The two entries have different key values.
 */
static bool
rm_hash_key_is_identical(const uint32_t *entry_a,
                         const uint32_t *entry_b,
                         const bcmlrd_hw_field_list_t *key_fields)
{
    uint8_t idx;
    int sbit, ebit;
    uint32_t field_a[BCMPTM_MAX_PT_FIELD_WORDS];
    uint32_t field_b[BCMPTM_MAX_PT_FIELD_WORDS];

    for (idx = 0; idx < key_fields->num_fields; idx++) {
        uint16_t num_words, word;
        num_words = (key_fields->field_width[idx] + 31) / 32;

        field_a[num_words - 1] = 0;
        field_b[num_words - 1] = 0;
        sbit = key_fields->field_start_bit[idx];
        ebit = key_fields->field_start_bit[idx] + key_fields->field_width[idx] - 1;
        bcmdrd_field_get(entry_a, sbit, ebit, field_a);
        bcmdrd_field_get(entry_b, sbit, ebit, field_b);

        for (word = 0; word < num_words; word++) {
            if (field_a[word] != field_b[word]) {
                return FALSE;
            }
        }
    }
    return TRUE;
}

/*!
 * \brief Get the hardware logical bucket for an entry
 *
 * This function computes hardware logical bucket for an entry
 *
 * \param [in] unit Unit number.
 * \param [in] e_words Pointer to entry buffer.
 * \param [in] key_format Pointer to bcmptm_rm_hash_key_format_t structure.
 * \param [in] e_attrs Pointer to bcmptm_rm_hash_dynamic_entry_attrs_t structure.
 * \param [in] rhash_ctrl Pointer to rm_hash_rhash_ctrl_t structure.
 * \param [out] inst_si Pointer to rm_hash_inst_srch_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_hlb_get(int unit,
                uint32_t *e_words,
                const bcmptm_rm_hash_key_format_t *key_format,
                bcmptm_rm_hash_dynamic_entry_attrs_t *e_attrs,
                rm_hash_rhash_ctrl rhash_ctrl,
                rm_hash_inst_srch_info_t *inst_si)
{
    uint8_t key_a[BCMPTM_MAX_PT_ENTRY_WORDS * 4] = {0};
    uint8_t key_b[BCMPTM_MAX_PT_ENTRY_WORDS * 4];
    uint8_t *pkey_a = key_a, *pkey_b = key_a;
    int key_size = 0, raw_key_length = 0;
    uint16_t key_len = key_format->hash_vector_attr->key_length;
    const bcmlrd_hw_field_list_t *key_fields = key_format->hw_key_fields;
    bool enabled = FALSE;
    int remap_idx, act_idx;

    SHR_FUNC_ENTER(unit);

    if (inst_si->hash_vector->valid == FALSE) {
        /*
         * Extract the key portion from the entry. The entry should have been
         * prepared according to the format defined by key_fields. And the correct
         * key type value has been filled.
 */
        if (e_attrs != NULL) {
            key_fields = e_attrs->hw_key_fields;
        }
        bcmptm_rm_hash_key_extract(unit, e_words, key_fields, key_len,
                                   rhash_ctrl, key_a, &raw_key_length,
                                   &key_size);
        /* Store the full key when hash store is enabled. */
        if (e_attrs != NULL && e_attrs->hstore_attr.enable) {
            sal_memcpy(inst_si->hash_vector->full_key,
                       key_a, RM_HASH_MAX_KEY_WORDS * 4);
        }
        /*
         * Only the physical hash table supports robust hash and this feature
         * has been enabled, the key_b extraction and transform is required.
         */
        if (rhash_ctrl != NULL) {
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_rbust_enable_get(unit, rhash_ctrl, &enabled));
            if (enabled == TRUE) {
                /* key_b transform is required. */
                sal_memcpy(key_b, key_a, key_size / 8);
                bcmptm_rm_hash_robust_hash_table_index_get(unit,
                                                           rhash_ctrl,
                                                           key_a,
                                                           raw_key_length,
                                                           &remap_idx,
                                                           &act_idx);
                bcmptm_rm_hash_robust_hash_get(unit, rhash_ctrl, 0, key_a,
                                               raw_key_length, remap_idx, act_idx);
                bcmptm_rm_hash_robust_hash_get(unit, rhash_ctrl, 1, key_b,
                                               raw_key_length, remap_idx, act_idx);
                pkey_b = key_b;
            }
        }
    }

    /* Calculate hardware logical buckets on all the specified banks. */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_vector_compute(unit,
                                       e_words,
                                       pkey_a,
                                       pkey_b,
                                       key_size,
                                       key_format->hash_vector_attr,
                                       inst_si->e_bm,
                                       inst_si));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get Software logical buckets for an entry.
 *
 * \param [in] unit Unit number.
 * \param [in] e_words Entry buffer.
 * \param [in] key_format Pointer to bcmptm_rm_hash_key_format_t structure.
 * \param [in] e_attrs Pointer to bcmptm_rm_hash_dynamic_entry_attrs_t structure.
 * \param [out] inst_si Pointer to rm_hash_inst_srch_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_slb_info_get(int unit,
                     uint32_t *e_words,
                     const bcmptm_rm_hash_key_format_t *key_format,
                     bcmptm_rm_hash_dynamic_entry_attrs_t *e_attrs,
                     rm_hash_inst_srch_info_t *inst_si)
{
    rm_hash_fmt_info_t *fmt_info = inst_si->fmt_info;
    rm_hash_pt_info_t *pt_info;
    uint8_t rbank;

    SHR_FUNC_ENTER(unit);

    pt_info = fmt_info->pt_info;
    rbank = fmt_info->en_rbank_list[0];

    if (e_attrs == NULL) {
        /* Get bucket mode of the entry on this table. */
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_ent_bkt_mode_get(unit,
                                             key_format,
                                             pt_info,
                                             rbank,
                                             &inst_si->e_bm,
                                             &inst_si->e_nm));
    } else {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_bkt_mode_mapping(unit,
                                             e_attrs->bucket_mode,
                                             &inst_si->e_bm));
    }
    /* Compute hardware logical bucket list. */
    SHR_IF_ERR_EXIT
        (rm_hash_hlb_get(unit,
                         e_words,
                         key_format,
                         e_attrs,
                         pt_info->rhash_ctrl[rbank],
                         inst_si));
    /*
     * For this ltid, hardware logical bucket list have been computed.
     * Compute software logical bucket and corresponding valid base
     * bucket bitmap within them.
     */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_hlb_to_slb_get(unit, pt_info, inst_si));

    /* Set hash store attribute if nesessay. */
    SHR_IF_ERR_EXIT
        (rm_hash_store_attri_set(unit, e_attrs, inst_si));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Search an entry within a bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] req_flags Flags from PTM.
 * \param [in] pt_dyn_info Pointer to pt_dyn_info structure.
 * \param [in] e_words Entry buffer.
 * \param [in] key_fmt Pointer to bcmptm_rm_hash_key_format_t structure.
 * \param [in] key_fields Pointer to bcmlrd_hw_field_list_t structure.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] pipe Pipe number.
 * \param [in] slb Software logical bucket.
 * \param [in] inst_si Pointer to rm_hash_inst_srch_info_t structure.
 * \param [in] e_sig Signature of the entry node.
 * \param [out] inst_sr Pointer to rm_hash_inst_srch_result_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_key_fmt_ent_search_in_bkt(int unit,
                                  uint64_t req_flags,
                                  bcmbd_pt_dyn_info_t *pt_dyn_info,
                                  uint32_t *e_words,
                                  const bcmptm_rm_hash_key_format_t *key_fmt,
                                  const bcmlrd_hw_field_list_t *key_fields,
                                  rm_hash_pt_info_t *pt_info,
                                  uint8_t rbank,
                                  uint8_t pipe,
                                  uint32_t slb,
                                  rm_hash_inst_srch_info_t *inst_si,
                                  uint32_t e_sig,
                                  rm_hash_inst_srch_result_t *inst_sr)
{
    rm_hash_ent_node_t *e_node = NULL;
    rm_hash_slb_state_t *b_state = NULL;
    uint8_t idx, num_valid_ent = 0, size = 0;
    uint32_t e_idx;
    bcmdrd_sid_t sid;
    bool match = FALSE;
    uint32_t nm_ent[BCMPTM_MAX_PT_FIELD_WORDS];
    uint32_t *e_buf = inst_sr->e_words;
    bool bank_in_nm = FALSE;
    bool ent_found = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_slb_state_get(unit, pt_info, rbank, pipe, slb,
                                      &b_state));
    if ((b_state == NULL) || (b_state->ve_cnt == 0)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    bank_in_nm = pt_info->bank_in_narrow_mode[rbank];
    if (bank_in_nm == TRUE) {
        e_buf = nm_ent;
    }

    bcmptm_rm_hash_base_ent_node_get(unit, pt_info, rbank, pipe,
                                     b_state->e_node_offset,
                                     &e_node);
    for (idx = 0; idx < pt_info->slb_num_entries; idx++, e_node++) {
        bcmbd_pt_dyn_info_t dyn_info = *pt_dyn_info;
        bool cache_valid = FALSE;
        bcmltd_sid_t ltid = 0;
        uint16_t dft_id = 0;
        uint64_t rd_req_flags = 0;
        uint32_t rd_rsp_flags = 0;
        if (num_valid_ent > b_state->ve_cnt) {
            break;
        }
        if ((e_node->flag & RM_HASH_NODE_IN_USE) == 0) {
            continue;
        }
        num_valid_ent++;
        size = bcmptm_rm_hash_ent_size_get(e_node->e_loc.be_bmp);
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_ent_index_get(unit, pt_info, slb,
                                          &e_node->e_loc,
                                          size, &rbank,
                                          &e_idx));
        dyn_info.index = e_idx;
        sid = e_node->sid;

        if ((inst_si->hash_store_en) &&
            (e_node->flag & RM_HASH_NODE_HASH_STORE) &&
            (e_node->sig == inst_si->hash_store_val)) {
            /*
             * When hash store is enabled, entries with differnt
             * signatures might have same hash_store_values.
             */
            rd_req_flags = req_flags & ~BCMLT_ENT_ATTR_GET_FROM_HW;
            SHR_IF_ERR_EXIT
                (bcmptm_cmdproc_read(unit,
                                     rd_req_flags,
                                     0,
                                     sid,
                                     &dyn_info,
                                     NULL,
                                     BCMPTM_MAX_PT_ENTRY_WORDS,
                                     inst_sr->e_words,
                                     &cache_valid,
                                     &ltid,
                                     &dft_id,
                                     &rd_rsp_flags));
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_store_full_key_same
                    (unit, ltid,
                     inst_si->hash_store_val,
                     inst_si->hash_vector->full_key,
                     &ent_found));
        } else if (e_node->sig == e_sig) {
            rd_req_flags = req_flags & ~BCMLT_ENT_ATTR_GET_FROM_HW;
            SHR_IF_ERR_EXIT
                (bcmptm_cmdproc_read(unit,
                                     rd_req_flags,
                                     0,
                                     sid,
                                     &dyn_info,
                                     NULL,
                                     BCMPTM_MAX_PT_ENTRY_WORDS,
                                     e_buf,
                                     &cache_valid,
                                     &ltid,
                                     &dft_id,
                                     &rd_rsp_flags));
            if (cache_valid == FALSE) {
                continue;
            }
            if (bank_in_nm == TRUE) {
                uint8_t nm_loc = 0;

                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_nm_ent_node_match(unit,
                                                      e_node,
                                                      inst_si->e_nm,
                                                      &match,
                                                      &nm_loc));
                /*
                 * The entry being searched is a narrow mode entry, and its
                 * narrow mode matches with the entry requested.
                 */
                if (match == TRUE) {
                    SHR_IF_ERR_EXIT
                        (bcmptm_rm_hash_nm_entry_extract(unit,
                                                         pt_info,
                                                         rbank,
                                                         inst_si->e_nm,
                                                         e_node->nm_loc,
                                                         e_buf,
                                                         inst_sr->e_words));
                }
            }
            /*
             * Now the entry in inst_sr->e_words can be compared with the entry
             * requested by LTM/PTM, irrespective of it's narrow mode or not.
             */
            ent_found = rm_hash_key_is_identical(e_words,
                                                 inst_sr->e_words,
                                                 key_fields);
        } else {
            continue;
        }
        if (ent_found == TRUE) {
            inst_sr->e_exist = TRUE;
            inst_sr->key_fmt = key_fmt;
            inst_sr->sid = sid;
            inst_sr->e_idx = e_idx;
            inst_sr->rbank = rbank;
            inst_sr->slb = slb;
            inst_sr->e_loc = e_node->e_loc;
            inst_sr->nme_info.e_nm = inst_si->e_nm;
            inst_sr->nme_info.nm_loc = e_node->nm_loc;
            inst_sr->ldtype_val = dft_id;
            if (req_flags & BCMLT_ENT_ATTR_GET_FROM_HW) {
                SHR_IF_ERR_EXIT
                    (bcmptm_cmdproc_read(unit,
                                         req_flags,
                                         0,
                                         sid,
                                         &dyn_info,
                                         NULL,
                                         BCMPTM_MAX_PT_ENTRY_WORDS,
                                         e_buf,
                                         &cache_valid,
                                         &ltid,
                                         &dft_id,
                                         &rd_rsp_flags));
                if (match == TRUE) {
                    SHR_IF_ERR_EXIT
                        (bcmptm_rm_hash_nm_entry_extract(unit,
                                                         pt_info,
                                                         rbank,
                                                         inst_si->e_nm,
                                                         e_node->nm_loc,
                                                         e_buf,
                                                         inst_sr->e_words));
                }
            }
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Search an entry in the physical hash table.
 *
 * \param [in] unit Unit number.
 * \param [in] req_flags Flags from PTM.
 * \param [in] pt_dyn_info Pointer to bcmbd_pt_dyn_info_t structure.
 * \param [in] e_words Entry buffer.
 * \param [in] key_fmt Pointer to bcmptm_rm_hash_key_format_t structure.
 * \param [in] e_attrs Pointer to bcmptm_rm_hash_dynamic_entry_attrs_t structure.
 * \param [in] inst_si Pointer to rm_hash_inst_srch_info_t structure.
 * \param [out] inst_sr Pointer to rm_hash_inst_srch_result_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_key_fmt_ent_search(int unit,
                           uint64_t req_flags,
                           bcmbd_pt_dyn_info_t *pt_dyn_info,
                           uint32_t *e_words,
                           const bcmptm_rm_hash_key_format_t *key_fmt,
                           bcmptm_rm_hash_dynamic_entry_attrs_t *e_attrs,
                           rm_hash_inst_srch_info_t *inst_si,
                           rm_hash_inst_srch_result_t *inst_sr)
{
    uint8_t bank_idx, rbank, *prbank = NULL;
    uint8_t num_en_rbank;
    uint32_t e_sig, slb;
    uint8_t pipe;
    const bcmlrd_hw_field_list_t *key_fields = NULL;
    rm_hash_pt_info_t *pt_info = inst_si->fmt_info->pt_info;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if (e_attrs == NULL) {
        key_fields = key_fmt->hw_key_fields;
    } else {
        key_fields = e_attrs->hw_key_fields;
    }
    /*
     * Generate entry signature according to the specified key field format.
     * This is for accelerating entry searching.
     */
    bcmptm_rm_hash_ent_sig_generate(unit, e_words, key_fields, &e_sig);
    num_en_rbank = inst_si->fmt_info->num_en_rbanks;
    prbank = inst_si->fmt_info->en_rbank_list;
    pipe = (pt_dyn_info->tbl_inst == -1)? 0 : pt_dyn_info->tbl_inst;

    for (bank_idx = 0; bank_idx < num_en_rbank; bank_idx++) {
        rbank = prbank[bank_idx];
        slb = inst_si->slb_info_list[bank_idx].slb;
        rv = rm_hash_key_fmt_ent_search_in_bkt(unit, req_flags, pt_dyn_info,
                                               e_words, key_fmt, key_fields,
                                               pt_info, rbank, pipe, slb,
                                               inst_si, e_sig, inst_sr);
        if (rv == SHR_E_NONE) {
            SHR_EXIT();
        } else if (rv == SHR_E_NOT_FOUND) {
            continue;
        } else {
            SHR_ERR_EXIT(rv);
        }
    }
    inst_sr->e_exist = FALSE;

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_hash_ent_search(int unit,
                          rm_hash_req_context_t *req_ctxt,
                          rm_hash_lt_ctrl_t *lt_ctrl,
                          rm_hash_glb_srch_info_t *glb_si,
                          rm_hash_map_srch_result_t *map_sr)
{
    bcmptm_rm_hash_req_t *req_info = req_ctxt->req_info;
    uint32_t *ent = NULL;
    uint8_t map_idx, inst, num_insts, grp_idx, fmt_idx;
    bcmptm_rm_hash_lt_info_t *lt_info = &lt_ctrl->lt_info;
    rm_hash_fmt_info_t *fmt_info = NULL;
    const bcmptm_rm_hash_key_format_t *key_format = NULL;
    const bcmptm_rm_hash_view_info_t *view_info = NULL;
    rm_hash_inst_srch_info_t *inst_si = NULL;
    rm_hash_inst_srch_result_t *inst_sr = NULL;
    rm_hash_entry_buf_t *ent_buf = NULL;
    bool all_insts_exist = FALSE;
    bcmptm_rm_hash_entry_attrs_t *e_attrs = req_info->entry_attrs;
    bcmptm_rm_hash_dynamic_entry_attrs_t *d_attrs = NULL;

    SHR_FUNC_ENTER(unit);

    map_sr->e_exist = FALSE;
    ent = req_info->entry_words;
    glb_si->num_vecs = 0;

    for (map_idx = 0; map_idx < lt_ctrl->num_maps; map_idx++) {
        if (req_info->entry_attrs != NULL) {
            /*
             * Entry_attrs has been provided to specify
             * the key format to be operated.
             */
            num_insts = 1;
            /* It is assumed that there will not be multiple groups in a map. */
            grp_idx = lt_ctrl->map_info[map_idx].grp_idx[0];
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_fmt_info_get(unit,
                                             req_info->entry_attrs,
                                             lt_ctrl,
                                             grp_idx,
                                             &fmt_info));
            glb_si->map_srch_info[map_idx].num_insts = 1;
            inst_si = &glb_si->map_srch_info[map_idx].inst_srch_info[0];
            inst_si->fmt_info = fmt_info;
        } else {
            num_insts = lt_ctrl->map_info[map_idx].num_insts;
            glb_si->map_srch_info[map_idx].num_insts = num_insts;
            for (inst = 0; inst < num_insts; inst++) {
                inst_si = &glb_si->map_srch_info[map_idx].inst_srch_info[inst];
                fmt_idx = lt_ctrl->map_info[map_idx].fmt_idx[inst];
                fmt_info = &lt_ctrl->fmt_info[fmt_idx];
                inst_si->fmt_info = fmt_info;
            }
        }

        for (inst = 0; inst < num_insts; inst++) {
            bcmdrd_sid_t remap_tab;
            uint8_t vec_idx = 0;
            inst_si = &glb_si->map_srch_info[map_idx].inst_srch_info[inst];
            key_format = inst_si->fmt_info->key_format;
            SHR_NULL_CHECK(key_format, SHR_E_PARAM);

            if (fmt_info->num_en_rbanks == 0) {
                continue;
            }
            grp_idx = lt_ctrl->map_info[map_idx].grp_idx[inst];
            /*
             * Group index and key format has strict corresponding
             * relationship.
             */
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_req_ent_view_info_get(unit,
                                                      req_info,
                                                      lt_info,
                                                      grp_idx,
                                                      key_format,
                                                      &view_info));
            /* Perform potential key field exchange operation. */
            if ((lt_ctrl->key_field_exchange == TRUE) && (grp_idx > 0)) {
                if (ent_buf == NULL) {
                    ent_buf = bcmptm_rm_hash_entry_buf_alloc();
                    if (ent_buf == NULL) {
                        SHR_ERR_EXIT(SHR_E_MEMORY);
                    }
                }
                sal_memset(ent_buf->e_words, 0, sizeof(ent_buf->e_words));
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_key_fields_exchange(unit,
                                                        req_info->entry_attrs,
                                                        lt_info,
                                                        0,
                                                        grp_idx,
                                                        req_info->entry_words,
                                                        ent_buf->e_words));
                ent = ent_buf->e_words;
            }
            /*
             * Fill KEY TYPE value into entry buffer. Each search bank may have
             * separated key type value.
             */
            if ((key_format->key_type_val != NULL) && (view_info != NULL)) {
                bcmptm_rm_hash_key_type_fill(unit,
                                             key_format->key_type_val[0],
                                             view_info,
                                             ent);
            } else if ((e_attrs != NULL) && (e_attrs->d_entry_attrs != NULL)) {
                d_attrs = e_attrs->d_entry_attrs;
            } else {
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
            }

            if (key_format->rh_remap_a_sid != NULL) {
                remap_tab = key_format->rh_remap_a_sid[0];
                for (vec_idx = 0; vec_idx < glb_si->num_vecs; vec_idx++) {
                    if (glb_si->vec_list[vec_idx].robust_remap_sid == remap_tab) {
                        /* The hash vector has been computed. */
                        inst_si->hash_vector = &glb_si->vec_list[vec_idx];
                        break;
                    }
                }
                if (vec_idx == glb_si->num_vecs) {
                    /* Not computed yet. */
                    glb_si->vec_list[vec_idx].robust_remap_sid = remap_tab;
                    glb_si->vec_list[vec_idx].valid = FALSE;
                    inst_si->hash_vector = &glb_si->vec_list[vec_idx];
                    glb_si->num_vecs++;
                }
            } else {
                glb_si->vec_list[0].valid = FALSE;
                inst_si->hash_vector = &glb_si->vec_list[0];
            }

            inst_si->ltid = lt_ctrl->ltid;
            SHR_IF_ERR_EXIT
                (rm_hash_slb_info_get(unit, ent, key_format, d_attrs, inst_si));
            inst_sr = &map_sr->inst_srch_result[inst];
            SHR_IF_ERR_EXIT
                (rm_hash_key_fmt_ent_search(unit,
                                            req_ctxt->req_flags,
                                            req_ctxt->pt_dyn_info,
                                            ent,
                                            key_format,
                                            d_attrs,
                                            inst_si,
                                            inst_sr));
            if (inst_sr->e_exist == TRUE) {
                /*
                 * If the entry exists, the entry in the result is in the format
                 * of a particular group, and it may not match with the format
                 * LTM expects.
                 */
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_ent_view_info_get(unit,
                                                      lt_info,
                                                      grp_idx,
                                                      inst_sr->sid,
                                                      &view_info));
                inst_sr->view_info = view_info;
            }
            if (inst == 0) {
                all_insts_exist = inst_sr->e_exist;
            } else {
                all_insts_exist = all_insts_exist && inst_sr->e_exist;
            }
        }

        if (all_insts_exist == TRUE) {
            map_sr->e_exist = TRUE;
            map_sr->map_idx = map_idx;
            map_sr->num_insts = num_insts;
            SHR_EXIT();
        }
    }

exit:
    if (ent_buf != NULL) {
        bcmptm_rm_hash_entry_buf_free(ent_buf);
    }
    SHR_FUNC_EXIT();
}

