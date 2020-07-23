/*! \file sbr_device.c
 *
 * SBR-RM device specific functions and definitions.
 *
 * This file contains device specific functions and definitions.
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
#include <shr/shr_pb.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmptm/bcmptm_ptcache_internal.h>
#include <bcmptm/bcmptm_sbr_internal.h>
#include "sbr_device.h"
#include "sbr_internal.h"
#include "sbr_util.h"
/*******************************************************************************
 * Private variables
 */
/*! SBR BCM unit level information.  */
static sbr_dev_info_t *sbr_dev_info[BCMDRD_CONFIG_MAX_UNITS] = {0};
/*! List of SBR BCM unit driver functions.  */
static sbr_driver_t *sbr_driver[BCMDRD_CONFIG_MAX_UNITS] = {0};

typedef struct sbr_tile_lt_lkup_action_s {
    /*! LT identifier. */
    bcmltd_sid_t ltid;
    /*! LT lookup type. */
    bcmptm_sbr_lt_lookup_t lookup_type;
    /*! LT lookup add/delete/no-op/LTS-TCAM index change operation.  */
    sbr_tm_lt_lookup_oper_t ltlkup_oper;
    /*! SBR profile table operation corresponding to LT lookup operation.  */
    sbr_pt_ent_oper_t spt_eoper;
    /*! Operation status is pending when TRUE. */
    bool pending;
    /*! Match status. */
    bool matched;
    /*! SBR profile table DRD SID. */
    bcmdrd_sid_t spt_sid;
    /*! SBR profile table entry state index. */
    uint16_t spt_estate_index;
    /*! SBR physical profile table entry index. */
    uint16_t sbr_pt_entry_index;
    /*! Tile LT lookup state entry index.  */
    uint32_t tltlkup_estate_index;
    /*! Variant tile LT.LOOKUP info index for this ltid + lookup_type. */
    uint32_t tltlkup_info_index;
    /*! LT.LOOKUP LTS_TCAM_DATA_ONLY view DRD SID. */
    uint32_t sid_lts_tcam_data_only;
} sbr_tile_lt_lkup_action_t;

typedef struct sbr_tm_chg_actions_list_s {
    /*! Tile that is undergoing a tile_mode change.  */
    uint16_t tile;
    /*! Tile current mode. */
    bcmptm_sbr_tile_mode_t cur_tm;
    /*! Actions on current tile LT lookups. */
    sbr_tile_lt_lkup_action_t *current;
    /*! Number of actions in current list. */
    uint16_t current_acount;
    /*! Tile new mode. */
    bcmptm_sbr_tile_mode_t new_tm;
    /*! Actions on new tile LT lookups. */
    sbr_tile_lt_lkup_action_t *new;
    /*! Number of actions in new list. */
    uint16_t new_acount;
} sbr_tm_chg_actions_list_t;

/*******************************************************************************
 * Defines
 */
/*! Combined layer/source for BSL logging. */
#define BSL_LOG_MODULE  BSL_LS_BCMPTM_UFT
/*******************************************************************************
 * Private functions
 */
static inline int
sbr_pt_entry_idx_free_test(bcmptm_sbr_entry_state_t *e)
{
    return ((e && e->usage_mode == BCMPTM_SBR_ENTRY_FREE
             && e->ref_count == 0) ? 1 : 0);
}

static int
sbr_pt_entry_alloc(int unit,
                   uint16_t spt,
                   bcmltd_sid_t lt_id,
                   uint16_t lookup_type,
                   bool paired_alloc,
                   uint16_t entry_count,
                   uint16_t *entry_index)
{
    int e, i = 0;
    uint16_t pair_idx = SBR_UINT16_INVALID;
    bcmptm_sbr_entry_state_t *ent = NULL, *paired_ent = NULL;
    sbr_dev_info_t *di = sbr_dev_info[unit];

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(di, SHR_E_INTERNAL);
    SHR_NULL_CHECK(entry_index, SHR_E_PARAM);

    if ((paired_alloc == TRUE && entry_count != 2)
        || (paired_alloc == FALSE && entry_count != 1)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (di->spt_state[spt].free_entries_count == 0
        || (paired_alloc && di->spt_state[spt].free_entries_count < 2) ) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_RESOURCE,
                            (BSL_META_U(unit,
                                        "Not enough free entries in SPT=%s/ "
                                        "EXT=%s to alloc - rv=%s.\n"),
                             bcmdrd_pt_sid_to_name(unit,
                                                   di->spt_state[spt].drd_sid),
                             bcmdrd_pt_sid_to_name(unit,
                                                di->spt_state[spt].ext_drd_sid),
                             shr_errmsg(SHR_E_RESOURCE)));
    }

    if (paired_alloc) {
        for (e = (int)(di->spt_state[spt].entries_count - 1);
             e >= (int)di->spt_state[spt].entries_count / 2;
             e--) {
            /* Get the SPT entry state pointer. */
            ent = di->spt_estate + (di->spt_state[spt].estate_base_index + e);
            /* Calculate the corresponding paired entry index value. */
            pair_idx = (e - (di->spt_state[spt].entries_count / 2));
            /* Get the paired entry state pointer. */
            paired_ent = (di->spt_estate
                            + (di->spt_state[spt].estate_base_index
                            + pair_idx));
            /*
             * Check if the paired entries free and available for this alloc.
             */
            if (sbr_pt_entry_idx_free_test(ent) == TRUE
                && sbr_pt_entry_idx_free_test(paired_ent) == TRUE) {
                /* Update the entry state in the paired entries. */
                ent->usage_mode = BCMPTM_SBR_ENTRY_IN_USE;
                ent->ref_count++;
                ent->ltid = lt_id;
                ent->lookup_type = lookup_type;
                ent->pt_state_index = spt;
                paired_ent->usage_mode = BCMPTM_SBR_ENTRY_IN_USE;
                paired_ent->ref_count++;
                paired_ent->ltid = lt_id;
                paired_ent->lookup_type = lookup_type;
                paired_ent->pt_state_index = spt;
                /* Decrement the SBR profile table free entries count. */
                di->spt_state[spt].free_entries_count -= 2;
                /* Populate the entry index values in the out parameter. */
                *(entry_index + i++) = e;
                *(entry_index + i) = pair_idx;

                SHR_EXIT();
            }
        }
    } else {
        /*
         * For single entry alloc requests, maximize SPT entries utilization by
         * using the partially filled paired entries buckets first.
         */
        for (e = (int)(di->spt_state[spt].entries_count - 1); e >= 0; e--) {
            /* Get the SPT entry state pointer. */
            ent = di->spt_estate + (di->spt_state[spt].estate_base_index + e);
            /* Calculate the corresponding paired entry index value. */
            pair_idx = ((e + (di->spt_state[spt].entries_count / 2))
                         % di->spt_state[spt].entries_count);
            /* Get the paired entry state pointer. */
            paired_ent = (di->spt_estate
                            + (di->spt_state[spt].estate_base_index
                            + pair_idx));
            /*
             * Use the partially filled bucket i.e. entry at the paried
             * index for single entry alloc requests.
             */
            if (sbr_pt_entry_idx_free_test(ent) == FALSE
                && sbr_pt_entry_idx_free_test(paired_ent) == TRUE) {
                /* Update the entry state. */
                paired_ent->usage_mode = BCMPTM_SBR_ENTRY_IN_USE;
                paired_ent->ref_count++;
                paired_ent->ltid = lt_id;
                paired_ent->lookup_type = lookup_type;
                paired_ent->pt_state_index = spt;
                /* Decrement the SBR profile table free entries count. */
                di->spt_state[spt].free_entries_count--;
                *entry_index = pair_idx;
                SHR_EXIT();
            }
        }
        /*
         * No partially filled buckets were found so allocate the first
         * available free entry.
         */
        for (e = (int)(di->spt_state[spt].entries_count - 1); e >= 0; e--) {
            /* Get the SPT entry state pointer. */
            ent = di->spt_estate + (di->spt_state[spt].estate_base_index + e);
            if (sbr_pt_entry_idx_free_test(ent)) {
                /* Update the entry state in the paired entries. */
                ent->usage_mode = BCMPTM_SBR_ENTRY_IN_USE;
                ent->ref_count++;
                ent->ltid = lt_id;
                ent->lookup_type = lookup_type;
                ent->pt_state_index = spt;
                di->spt_state[spt].free_entries_count--;
                *entry_index = e;
                SHR_EXIT();
            }
        }
    }
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "Failed: SPT=%s/EXT=%s paired_alloc=%u for "
                          "entry_count=%u - rv=%s\n"),
               bcmdrd_pt_sid_to_name(unit, di->spt_state[spt].drd_sid),
               bcmdrd_pt_sid_to_name(unit, di->spt_state[spt].ext_drd_sid),
               paired_alloc, entry_count, shr_errmsg(SHR_E_RESOURCE)));
    SHR_IF_ERR_EXIT(SHR_E_RESOURCE);
exit:
    SHR_FUNC_EXIT();
}

static int
sbr_pt_entry_free(int unit,
                  uint16_t spt,
                  uint16_t entry_count,
                  uint16_t *entry_index)
{
    uint16_t i = 0; /* Iterator variable. */
    bcmptm_sbr_entry_state_t *ent = NULL;
    sbr_dev_info_t *di = sbr_dev_info[unit];

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(di, SHR_E_INTERNAL);
    SHR_NULL_CHECK(entry_index, SHR_E_PARAM);

    for (i = 0; i < entry_count; i++) {
        if (*(entry_index + i) >= di->spt_state[spt].entries_count) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                (BSL_META_U(unit,
                                            "SPT=%s/EXT=%s invalid "
                                            "entry_index=%u - rv=%s.\n"),
                                 bcmdrd_pt_sid_to_name
                                    (unit, di->spt_state[spt].drd_sid),
                                 bcmdrd_pt_sid_to_name
                                    (unit, di->spt_state[spt].ext_drd_sid),
                                 *(entry_index + i),
                                 shr_errmsg(SHR_E_PARAM)));
        }
        /* Get the SPT entry state pointer. */
        ent = di->spt_estate + (di->spt_state[spt].estate_base_index
                                + *(entry_index + i));
        if (ent->usage_mode != BCMPTM_SBR_ENTRY_IN_USE || ent->ref_count == 0) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_NOT_FOUND,
                                (BSL_META_U(unit,
                                            "SPT=%s/EXT=%s entry=%u with "
                                            "ref_cnt=%u is not in-use to "
                                            "free - rv=%s.\n"),
                                 bcmdrd_pt_sid_to_name
                                    (unit, di->spt_state[spt].drd_sid),
                                 bcmdrd_pt_sid_to_name
                                    (unit, di->spt_state[spt].ext_drd_sid),
                                 *(entry_index + i),
                                 ent->ref_count,
                                 shr_errmsg(SHR_E_NOT_FOUND)));
        }
        if (ent->ref_count > 0) {
            ent->ref_count--;
        }
        if (ent->ref_count == 0) {
            ent->usage_mode = BCMPTM_SBR_ENTRY_FREE;
            ent->ltid = di->invalid_ltid;
            ent->lookup_type = BCMPTM_SBR_LOOKUP_INVALID;
            ent->pt_state_index = SBR_UINT16_INVALID;
            di->spt_state[spt].free_entries_count++;
        }
        if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Done(%s): SPT=%s/EXT=%s ecnt=%u eidx=%u "
                                  "cur:ref_cnt=%u.\n"),
                       bcmptm_sbr_pt_ent_oper_str(SBR_PT_ENT_FREE),
                       bcmdrd_pt_sid_to_name(unit, di->spt_state[spt].drd_sid),
                       bcmdrd_pt_sid_to_name(unit,
                                             di->spt_state[spt].ext_drd_sid),
                       entry_count, *(entry_index + i), ent->ref_count));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static void
sbr_tm_chg_actions_list_t_init(sbr_tm_chg_actions_list_t *tm_alist) {
    if (tm_alist) {
        sal_memset(tm_alist, 0, sizeof(*tm_alist));
    }
    return;
}

static int
sbr_tltlkup_cur_state_get(int unit,
                          bcmptm_sbr_tile_state_t *ts,
                          bcmltd_sid_t ltid,
                          bcmptm_sbr_lt_lookup_t lookup_type,
                          bool *in_use,
                          uint32_t *tltlkup_estate_index)
{
    uint16_t l, l_count = 0; /* LT lookup index. */
    const bcmptm_sbr_lt_lookup_state_tile_t *lkup_state = NULL;
    const bcmptm_sbr_entry_state_t *spt_estate = NULL;
    const sbr_dev_info_t *di = sbr_dev_info[unit]; /* Device info PTR. */
    const char *lt_name = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(di, SHR_E_INTERNAL);
    SHR_NULL_CHECK(ts, SHR_E_PARAM);
    SHR_NULL_CHECK(in_use, SHR_E_PARAM);
    SHR_NULL_CHECK(tltlkup_estate_index, SHR_E_PARAM);

    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        SHR_IF_ERR_EXIT
            (bcmlrd_field_value_to_symbol(unit,
                                          TABLE_INFOt,
                                          TABLE_INFOt_TABLE_IDf,
                                          ltid,
                                          &lt_name));
    }
    /*
     * Search for LT.LOOKUP in the SPT estate array, if a match is found with
     * an in-use entry, then return the SPT entry index if valid,
     */
    l_count = (ts->lt_lookup0_count + ts->lt_lookup1_count);
    for (l = ts->lt_lookup0_base_index;
         l < (ts->lt_lookup0_base_index + l_count);
         l++) {

        /* Get the current tile LT.LOOKUP state entry ptr. */
        lkup_state = di->tile_lkup_state + l;
        SHR_NULL_CHECK(lkup_state, SHR_E_INTERNAL);

        /* Get the corresponding SPT entry state. */
        spt_estate = (di->spt_estate + lkup_state->entry_index);
        SHR_NULL_CHECK(spt_estate, SHR_E_INTERNAL);

        if (!lkup_state->in_use) {
            /* Skip the unused entries. */
            continue;
        }
        if (lkup_state->in_use && ltid == spt_estate->ltid
            && lookup_type == spt_estate->lookup_type) {
            /* Tile LT.LOOKUP index into the shared tile_lkup_state[] array. */
            *tltlkup_estate_index = l;
            *in_use = lkup_state->in_use;
            if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
                LOG_DEBUG(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Match-found: ltid=%s lkup_type=%s "
                                      "in_use=%u tltlkup_estate_index=%u.\n"),
                           lt_name, bcmptm_sbr_lt_lkup_str(lookup_type),
                           *in_use, *tltlkup_estate_index));
            }
            SHR_EXIT();
        }
    }
    *in_use = 0;
    *tltlkup_estate_index = SBR_UINT32_INVALID;
    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "NoMatch: ltid=%s lkup_type=%s in_use=%u "
                              "tltlkup_estate_index=%d rv=%s\n"),
                   lt_name, bcmptm_sbr_lt_lkup_str(lookup_type), *in_use,
                   (int)*tltlkup_estate_index, shr_errmsg(SHR_E_NOT_FOUND)));
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT();
}


static int
sbr_pt_entry_alloc_install(int unit,
                           bool only_check_rsrc,
                           uint16_t tile,
                           bcmltd_sid_t lt_id,
                           const bcmltd_op_arg_t *op_arg,
                           uint16_t spt,
                           sbr_tile_lt_lkup_action_t *act)
{
    uint16_t e = SBR_UINT16_INVALID;
    bool in_use = FALSE;
    uint32_t *edata_buf = NULL;
    uint32_t tltlkup_estate_index = SBR_UINT32_INVALID;
    sbr_dev_info_t *di = sbr_dev_info[unit];
    bcmptm_sbr_lt_lookup_state_tile_t *tltlkup_state = NULL;
    bcmptm_sbr_tile_state_t *ts = NULL; /* Tile state. */
    const bcmptm_sbr_entry_data_t *pdata = NULL;
    const bcmptm_sbr_tile_info_t *v_ti = NULL;
    const bcmptm_sbr_lt_lookup_info_tile_t *v_tltlkup_i = NULL;
    const char *lt_name = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(act, SHR_E_INTERNAL);
    SHR_NULL_CHECK(di, SHR_E_INTERNAL);

    ts = di->tile_state + tile;
    SHR_NULL_CHECK(ts, SHR_E_INTERNAL);

    SHR_IF_ERR_EXIT
        (sbr_tltlkup_cur_state_get(unit, ts, act->ltid, act->lookup_type,
                                   &in_use, &tltlkup_estate_index));
    if (tltlkup_estate_index != SBR_UINT32_INVALID) {
        tltlkup_state = di->tile_lkup_state + tltlkup_estate_index;
        SHR_NULL_CHECK(tltlkup_state, SHR_E_INTERNAL);
        if (tltlkup_state->in_use == TRUE
            && tltlkup_state->entry_index != SBR_UINT16_INVALID
            && tltlkup_state->spt_sid == act->spt_sid) {
            /* Update the SPT HW entry index for write in the LTS TCAM rules. */
            act->sbr_pt_entry_index = (tltlkup_state->entry_index
                                        - di->spt_state[spt].estate_base_index);
            act->spt_estate_index = tltlkup_state->entry_index;
            if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
                SHR_IF_ERR_EXIT
                    (bcmlrd_field_value_to_symbol(unit,
                                                  TABLE_INFOt,
                                                  TABLE_INFOt_TABLE_IDf,
                                                  act->ltid,
                                                  &lt_name));
                LOG_DEBUG(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "lt_id=%s tltlkup_estate_index=%u "
                                      "spt_id=%s eindex=%d rcnt=%u - already "
                                      "allocated for this Tile & LT.LOOKUP.\n"),
                           lt_name,
                           tltlkup_estate_index,
                           bcmdrd_pt_sid_to_name(unit, tltlkup_state->spt_sid),
                           act->sbr_pt_entry_index,
                           di->spt_estate[act->spt_estate_index].ref_count));
            }
            SHR_EXIT();
        }
    }
    SHR_IF_ERR_EXIT
        (sbr_pt_entry_alloc(unit, spt, act->ltid, act->lookup_type, FALSE, 1,
                            &e));
    /* Get the tile info pointer. */
    v_ti = di->var->sbr_tile_info + tile;
    SHR_NULL_CHECK(v_ti, SHR_E_INTERNAL);
    if (act->lookup_type == BCMPTM_SBR_LOOKUP0) {
        v_tltlkup_i = (v_ti->sbr_lt_lookup0_info + act->tltlkup_info_index);
    } else {
        v_tltlkup_i = (v_ti->sbr_lt_lookup1_info + act->tltlkup_info_index);
    }
    /* Get the SBR profile table data pointer from the variant data. */
    pdata = v_tltlkup_i->sbr_profile_data;
    SHR_ALLOC(edata_buf, sizeof(uint32_t) * di->spt_entry_max_wsize,
              "bcmptmSbrPtEntBuff");
    SHR_NULL_CHECK(edata_buf, SHR_E_MEMORY);
    sal_memset(edata_buf, 0, sizeof(uint32_t) * di->spt_entry_max_wsize);

    /* Copy the data word into the buffer. */
    sal_memcpy(edata_buf, pdata->dw, sizeof(uint32_t) * pdata->dw_count);
    SHR_IF_ERR_EXIT
        (bcmptm_sbr_lt_hw_write(unit, only_check_rsrc, lt_id, op_arg,
                                pdata->sid, e, edata_buf));
    /* Store the SPT HW entry index for write in the LTS TCAM rules. */
    act->sbr_pt_entry_index = e;
    /*
     * Adjust the entry offset for the SW SBR entry state index and
     * store it.
     */
    act->spt_estate_index = (di->spt_state[spt].estate_base_index + e);
    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        SHR_IF_ERR_EXIT
            (bcmlrd_field_value_to_symbol(unit, TABLE_INFOt,
                                          TABLE_INFOt_TABLE_IDf, act->ltid,
                                          &lt_name));
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Done(%s, only_check_rsrc=%u): SPT=%s "
                              "entry_index=%u tile=%u ltid=%s lkup_type=%s.\n"),
                   bcmptm_sbr_pt_ent_oper_str(SBR_PT_ENT_ALLOC_HW_INSTALL),
                   only_check_rsrc, bcmdrd_pt_sid_to_name(unit, pdata->sid),
                   act->sbr_pt_entry_index, tile, lt_name,
                   bcmptm_sbr_lt_lkup_str(act->lookup_type)));
    }
exit:
    SHR_FREE(edata_buf);
    SHR_FUNC_EXIT();
}

static int
sbr_pt_entry_free_clear(int unit,
                        bool only_check_rsrc,
                        uint16_t tile,
                        bcmltd_sid_t lt_id,
                        const bcmltd_op_arg_t *op_arg,
                        uint16_t count,
                        sbr_tile_lt_lkup_action_t *act)
{
    int pt_entry_index;
    uint32_t *edata_buf = NULL;
    bcmptm_sbr_entry_state_t *estate = NULL;
    sbr_dev_info_t *di = sbr_dev_info[unit];
    bcmptm_sbr_lt_lookup_state_tile_t *tltlkup_state = NULL;
    const char *lt_name = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(act, SHR_E_INTERNAL);
    SHR_NULL_CHECK(di, SHR_E_INTERNAL);

    estate = (di->spt_estate + act->spt_estate_index);
    SHR_NULL_CHECK(estate, SHR_E_INTERNAL);
    /* Get the LT.LOOKUP LT name. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_value_to_symbol(unit,
                                      TABLE_INFOt,
                                      TABLE_INFOt_TABLE_IDf,
                                      act->ltid,
                                      &lt_name));

    if (act->tltlkup_estate_index != SBR_UINT32_INVALID) {
        tltlkup_state = di->tile_lkup_state + act->tltlkup_estate_index;
        SHR_NULL_CHECK(tltlkup_state, SHR_E_INTERNAL);
        if (tltlkup_state->in_use == FALSE
            && tltlkup_state->entry_index == SBR_UINT16_INVALID
            && tltlkup_state->spt_sid == di->invalid_ptid) {
            /*
             * For tiles that have duplicate LT.LOOKUP entries in the Tile_Mode
             * section (due to HW support), same SBR profile table entry is
             * shared by these duplicate LT.LOOKUPs on this tile. The reference
             * count of the SBR entry is incremented once for this Tile +
             * LT.LOOKUP.
             *
             * So, during SBR entry free operation, for tiles with such
             * duplicate LT.LOOKUPs, if the SBR entry has already been freed by
             * the first instance of this LT.LOOKUP, then there is noting to do
             * for the second/duplicate LT.LOOKUP entry. As duplicate LT.LOOKUPs
             * on a tile share the same entry in tile_lkup_state[] array, check
             * the SPT SW state in this array element and exit this function
             * (E_NONE) if it meets the above if-condition.
             */
            if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
                LOG_DEBUG(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "lt_id=%s spt_id=%s eindex=%d rcnt=%u - "
                                      "already freed for this Tile & "
                                      "LT.LOOKUP.\n"),
                           lt_name,
                           bcmdrd_pt_sid_to_name(unit, act->spt_sid),
                           act->sbr_pt_entry_index,
                           estate->ref_count));
            }
            SHR_EXIT();
        }
    }
    SHR_IF_ERR_EXIT
        (sbr_pt_entry_free(unit, estate->pt_state_index, 1,
                           &act->sbr_pt_entry_index));
    if (estate->ref_count == 0) {
        /*
         * Clear the HW SPT entry if it is not longer in use by any tile
         * LT.LOOKUP.
         */
        SHR_ALLOC(edata_buf, sizeof(uint32_t) * di->spt_entry_max_wsize,
                  "bcmptmSbrPtEntBuff");
        SHR_NULL_CHECK(edata_buf, SHR_E_MEMORY);
        sal_memset(edata_buf, 0, sizeof(uint32_t) * di->spt_entry_max_wsize);

        pt_entry_index = (int)(act->sbr_pt_entry_index);
        SHR_IF_ERR_EXIT
            (bcmptm_sbr_lt_hw_write(unit, only_check_rsrc, lt_id, op_arg,
                                    act->spt_sid, pt_entry_index, edata_buf));
        if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Done(%s, only_check_rsrc=%u): SPT=%s "
                                  "sbr_pt_eidx=%u tile=%u ltid=%s "
                                  "lkup_type=%s.\n"),
                       bcmptm_sbr_pt_ent_oper_str(SBR_PT_ENT_FREE_HW_CLEAR),
                       only_check_rsrc,
                       bcmdrd_pt_sid_to_name(unit, act->spt_sid),
                       act->sbr_pt_entry_index, tile, lt_name,
                       bcmptm_sbr_lt_lkup_str(act->lookup_type)));
        }
    }
exit:
    SHR_FREE(edata_buf);
    SHR_FUNC_EXIT();
}

static int
sbr_pt_entry_ref_incr(int unit,
                      uint16_t tile,
                      bcmltd_sid_t lt_id,
                      const bcmltd_op_arg_t *op_arg,
                      sbr_tile_lt_lkup_action_t *act)
{
    bcmptm_sbr_entry_state_t *estate = NULL;
    sbr_dev_info_t *di = sbr_dev_info[unit];
    const char *lt_name = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(act, SHR_E_INTERNAL);
    SHR_NULL_CHECK(di, SHR_E_INTERNAL);

    estate = (di->spt_estate + act->spt_estate_index);
    SHR_NULL_CHECK(estate, SHR_E_INTERNAL);
    if (estate->usage_mode != BCMPTM_SBR_ENTRY_IN_USE
        || estate->ref_count == 0) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                            (BSL_META_U(unit,
                                        "SPT(=%u) entry=%u not in-use - "
                                        "rv=%s.\n"),
                             estate->pt_state_index,
                             act->spt_estate_index,
                             shr_errmsg(SHR_E_INTERNAL)));
    }
    estate->ref_count++;
    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        SHR_IF_ERR_EXIT
            (bcmlrd_field_value_to_symbol(unit,
                                          TABLE_INFOt,
                                          TABLE_INFOt_TABLE_IDf,
                                          act->ltid,
                                          &lt_name));
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Done(%s, new_ref_count=%u): SPT=%u "
                              "estate_index=%u tile=%u ltid=%s "
                              "lkup_type=%s.\n"),
                   bcmptm_sbr_pt_ent_oper_str(SBR_PT_ENT_REF_INCR),
                   estate->ref_count, estate->pt_state_index,
                   act->spt_estate_index, tile, lt_name,
                   bcmptm_sbr_lt_lkup_str(act->lookup_type)));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
sbr_ltlkup_tile_lts_tcam_update(int unit,
                                bool only_check_rsrc,
                                uint16_t tile,
                                uint32_t new_tm,
                                bcmltd_sid_t lt_id,
                                const bcmltd_op_arg_t *op_arg,
                                sbr_tile_lt_lkup_action_t *act)
{
    const sbr_dev_info_t *di = sbr_dev_info[unit];
    const bcmptm_sbr_var_drv_t *vi = NULL;
    const bcmptm_sbr_tile_info_t *v_ti = NULL;
    const bcmptm_sbr_tile_mode_info_t *v_tmi = NULL;
    const bcmptm_sbr_lt_lookup_info_tile_mode_t *v_tmltlkup_info = NULL;
    uint16_t r, l, v_tmltlkup_info_count = 0, mindex = SBR_UINT16_INVALID;
    uint16_t minbit = SBR_UINT16_INVALID, maxbit = SBR_UINT16_INVALID;
    uint16_t field_width = 0;
    uint32_t tm_index = SBR_UINT32_INVALID; /* Tile mode index. */
    uint32_t *edata_buf = NULL, spt_eindex = SBR_UINT32_INVALID;
    size_t entry_wsize = 0, entry_size = 0;
    const char *lt_name = NULL, *tile_name = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(act, SHR_E_INTERNAL);
    SHR_NULL_CHECK(op_arg, SHR_E_INTERNAL);
    SHR_NULL_CHECK(di, SHR_E_INTERNAL);

    /* Get the device variant info strcture ptr. */
    vi = di->var;
    SHR_NULL_CHECK(vi, SHR_E_INTERNAL);

    if (new_tm == di->var->tile_mode_disable) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "SBR: LTS TCAM rule update call for "
                                        "disabled tile=%u new_tm=%u - "
                                        "rv=%s.\n"),
                             tile,
                             new_tm,
                             shr_errmsg(SHR_E_PARAM)));
    }

    if (act->spt_estate_index == SBR_UINT16_INVALID) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid SBR entry index for tile=%u "
                                        "new_tm=%u - rv=%s.\n"),
                             tile,
                             new_tm,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* Get device tile info ptr. */
    v_ti = vi->sbr_tile_info + tile;
    SHR_NULL_CHECK(v_ti, SHR_E_INTERNAL);
    /* Determine the tile mode array index value for the new tile mode. */
    tm_index = new_tm - v_ti->tile_mode[0];
    /* Get the tile mode info for the given tile mode. */
    v_tmi = v_ti->tile_mode_info + tm_index;
    SHR_NULL_CHECK(v_tmi, SHR_E_INTERNAL);

    switch (act->lookup_type) {
        case BCMPTM_SBR_LOOKUP0:
            v_tmltlkup_info = v_tmi->lt_lookup0_info;
            v_tmltlkup_info_count = v_tmi->lt_lookup0_info_count;
            break;
        case BCMPTM_SBR_LOOKUP1:
            v_tmltlkup_info = v_tmi->lt_lookup1_info;
            v_tmltlkup_info_count = v_tmi->lt_lookup1_info_count;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    for (l = 0; l < v_tmltlkup_info_count; l++) {
        if (v_tmltlkup_info[l].lookup_type == act->lookup_type
            && v_tmltlkup_info[l].ltid == act->ltid
            && v_tmltlkup_info[l].sid_lts_tcam_data_only
                == act->sid_lts_tcam_data_only
            && v_tmltlkup_info[l].tile == tile) {
            mindex = l;
            break;
        }
    }

    if (l == v_tmltlkup_info_count || mindex == SBR_UINT16_INVALID) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_NOT_FOUND,
                            (BSL_META_U(unit,
                                        "LTS TCAM info not found for tile=%u "
                                        "new_tm=%u - rv=%s.\n"),
                             tile,
                             new_tm,
                             shr_errmsg(SHR_E_NOT_FOUND)));
    }

    entry_wsize = bcmdrd_pt_entry_wsize
                    (unit, v_tmltlkup_info[l].sid_lts_tcam_data_only);
    entry_size = sizeof(uint32_t) * entry_wsize;
    SHR_ALLOC(edata_buf, entry_size, "bcmptmSbrPtEntBuff");
    SHR_NULL_CHECK(edata_buf, SHR_E_MEMORY);
    sal_memset(edata_buf, 0, entry_size);

    for (r = 0; r < v_tmltlkup_info[l].rule_index_count; r++) {
        /* Clear the buffer prior to reading the HW entry. */
        sal_memset(edata_buf, 0, entry_size);
        /* Read the entry from HW. */
        SHR_IF_ERR_EXIT
            (bcmptm_sbr_lt_hw_read(unit, only_check_rsrc, lt_id, op_arg,
                                   v_tmltlkup_info[l].sid_lts_tcam_data_only,
                                   v_tmltlkup_info[l].rule_index[r],
                                   entry_size,
                                   edata_buf));
        /* Set the SBR profile table entry index value. */
        minbit = v_tmltlkup_info[l].bitp_f_sbr_profile_index->minbit;
        maxbit = v_tmltlkup_info[l].bitp_f_sbr_profile_index->maxbit;
        field_width = (maxbit - minbit) + 1;
        spt_eindex = act->sbr_pt_entry_index;
        if (spt_eindex > (uint32_t)((1 << field_width) - 1)) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_BADID,
                                (BSL_META_U(unit,
                                            "SBR profile table entry index=%u "
                                            "value exceeds the LTS_TCAM field "
                                            "width=%u - rv=%s.\n"),
                                 spt_eindex, field_width,
                                 shr_errmsg(SHR_E_BADID)));
        }
        bcmdrd_field_set(edata_buf, minbit, maxbit, &spt_eindex);

        /* Write entry to the hardware. */
        SHR_IF_ERR_EXIT
            (bcmptm_sbr_lt_hw_write(unit, only_check_rsrc, lt_id, op_arg,
                                    v_tmltlkup_info[l].sid_lts_tcam_data_only,
                                    v_tmltlkup_info[l].rule_index[r],
                                    edata_buf));
    }
    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        SHR_IF_ERR_EXIT
            (bcmlrd_field_value_to_symbol(unit,
                                          TABLE_INFOt,
                                          TABLE_INFOt_TABLE_IDf,
                                          act->ltid,
                                          &lt_name));
        /* Get tile name. */
        SHR_IF_ERR_EXIT
            (bcmlrd_field_value_to_symbol(unit, DEVICE_EM_TILE_INFOt,
                                        DEVICE_EM_TILE_INFOt_DEVICE_EM_TILE_IDf,
                                        tile, &tile_name));
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Done: LTS_TCAM rules(%u) updated t=%s ltid=%s "
                              "lkup_t=%s spt_eindex=%u min=%u max=%u.\n"),
                   v_tmltlkup_info[l].rule_index_count, tile_name, lt_name,
                   bcmptm_sbr_lt_lkup_str(act->lookup_type), spt_eindex,
                   minbit, maxbit));
    }
exit:
    SHR_FREE(edata_buf);
    SHR_FUNC_EXIT();
}

static int
sbr_tltlkup_match_spt_sid_get(int unit,
                              uint16_t tile,
                              bcmptm_sbr_tile_state_t *ts,
                              bcmltd_sid_t ltid,
                              bcmptm_sbr_lt_lookup_t lookup_type,
                              uint32_t *spt_sid)
{
    uint16_t l, count; /* LT.LOOKUP index. */
    const bcmptm_sbr_var_drv_t *vi = sbr_dev_info[unit]->var; /* Var info. */
    const bcmptm_sbr_tile_info_t *v_ti = NULL;
    const bcmptm_sbr_lt_lookup_info_tile_t *v_ltlkup_base = NULL;
    const bcmptm_sbr_lt_lookup_info_tile_t *v_ltlkup_ptr = NULL;
    const char *lt_name = NULL, *tile_name = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(vi, SHR_E_INTERNAL);
    v_ti = vi->sbr_tile_info + tile;
    SHR_NULL_CHECK(v_ti, SHR_E_INTERNAL);

    if (lookup_type == BCMPTM_SBR_LOOKUP0) {
        /*
         * Get LT.LOOKUP0 base ptr and total lookup entries count from the tile
         * info structure.
         */
        v_ltlkup_base = v_ti->sbr_lt_lookup0_info;
        count = v_ti->sbr_lt_lookup0_info_count;
    } else {
        /*
         * Get LT.LOOKUP1 base ptr and total lookup entries count from the tile
         * info structure.
         */
        v_ltlkup_base = v_ti->sbr_lt_lookup1_info;
        count = v_ti->sbr_lt_lookup1_info_count;
    }
    SHR_NULL_CHECK(v_ltlkup_base, SHR_E_INTERNAL);
    /* Get LT name. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_value_to_symbol(unit, TABLE_INFOt, TABLE_INFOt_TABLE_IDf,
                                      ltid, &lt_name));
    /* Get tile name. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_value_to_symbol(unit, DEVICE_EM_TILE_INFOt,
                                      DEVICE_EM_TILE_INFOt_DEVICE_EM_TILE_IDf,
                                      tile, &tile_name));
    /*
     * Iterate over the tile LT.LOOKUPs and check for match. If the matched
     * LT.LOOKUP has valid SBR profile data, then extract the SID value and
     * return it.
     */
    for (l = 0; l < count; l++) {
        v_ltlkup_ptr = v_ltlkup_base + l;
        if (v_ltlkup_ptr->tile == tile
            && v_ltlkup_ptr->lookup_type == lookup_type
            && v_ltlkup_ptr->ltid == ltid
            && v_ltlkup_ptr->sbr_profile_data != NULL) {
            *spt_sid = v_ltlkup_ptr->sbr_profile_data[0].sid;
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "SPT_SID(=%s found) for tile=%s ltid=%s "
                                  "lkup_type=%s - rv=%s\n"),
                       bcmdrd_pt_sid_to_name(unit, *spt_sid), tile_name,
                       lt_name, bcmptm_sbr_lt_lkup_str(lookup_type),
                       shr_errmsg(SHR_E_NONE)));
            SHR_EXIT();
        }
    }
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "SPT_SID Not Found for tile=%s ltid=%s lkup_type=%s "
                          "- rv=%s\n"),
               tile_name, lt_name, bcmptm_sbr_lt_lkup_str(lookup_type),
               shr_errmsg(SHR_E_NOT_FOUND)));
    SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT();
}

static int
sbr_tmi_ltlkup_spt_state_entry_index_get(int unit,
                                         bcmptm_sbr_tile_state_t *ts,
                                         bcmltd_sid_t ltid,
                                         bcmptm_sbr_lt_lookup_t lookup_type,
                                         uint16_t *spt_estate_index,
                                         uint16_t *sbr_pt_entry_index,
                                         uint32_t *tltlkup_estate_index)
{
    uint16_t l, l_count = 0; /* LT lookup index. */
    const bcmptm_sbr_lt_lookup_state_tile_t *lkup_state = NULL;
    const bcmptm_sbr_entry_state_t *spt_estate = NULL;
    const sbr_dev_info_t *di = sbr_dev_info[unit]; /* Device info PTR. */
    const bcmptm_sbr_pt_state_t *stp_state = NULL; /* SBR PT state PTR. */
    shr_pb_t *pb = NULL; /* Print buffer. */
    const char *lt_name = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(di, SHR_E_INTERNAL);
    SHR_NULL_CHECK(ts, SHR_E_PARAM);
    SHR_NULL_CHECK(spt_estate_index, SHR_E_PARAM);
    SHR_NULL_CHECK(sbr_pt_entry_index, SHR_E_PARAM);
    SHR_NULL_CHECK(tltlkup_estate_index, SHR_E_PARAM);

    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        SHR_IF_ERR_EXIT
            (bcmlrd_field_value_to_symbol(unit, TABLE_INFOt,
                                          TABLE_INFOt_TABLE_IDf,
                                          ltid, &lt_name));
        pb = shr_pb_create();
    }
    /*
     * Search for LT.LOOKUP in the SPT estate array, if a match is found with
     * an in-use entry, then return the SPT entry index if valid,
     */
    l_count = (ts->lt_lookup0_count + ts->lt_lookup1_count);
    for (l = ts->lt_lookup0_base_index;
         l < (ts->lt_lookup0_base_index + l_count);
         l++) {

        /* Get the current tile LT.LOOKUP state entry ptr. */
        lkup_state = di->tile_lkup_state + l;
        SHR_NULL_CHECK(lkup_state, SHR_E_INTERNAL);

        if (!lkup_state->in_use) {
            /* Skip the unused entries. */
            continue;
        }

        /* Get the corresponding SPT entry state. */
        spt_estate = (di->spt_estate + lkup_state->entry_index);
        SHR_NULL_CHECK(spt_estate, SHR_E_INTERNAL);

        /* Get the SPT entry state parent SBR table state PTR. */
        stp_state = (di->spt_state + spt_estate->pt_state_index);
        SHR_NULL_CHECK(stp_state, SHR_E_INTERNAL);

        if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE) && pb) {
            shr_pb_printf(pb, "InUse-MCheck(l=%u in-use=%u): ltid=%s lkup=%s "
                              "SPT-state: eidx=%u ltid=%u lkup=%u spt=%u.\n", l,
                              lkup_state->in_use, lt_name,
                              bcmptm_sbr_lt_lkup_str(lookup_type),
                              lkup_state->entry_index, spt_estate->ltid,
                              spt_estate->lookup_type,
                              spt_estate->pt_state_index);
        }

        if (lkup_state->in_use && ltid == spt_estate->ltid
            && lookup_type == spt_estate->lookup_type) {
            if (lkup_state->entry_index != SBR_UINT16_INVALID) {
                *spt_estate_index = lkup_state->entry_index;
                *sbr_pt_entry_index = (*spt_estate_index
                                            - stp_state->estate_base_index);
            }
            /* Tile LT.LOOKUP index into the shared tile_lkup_state[] array. */
            *tltlkup_estate_index = l;

            if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE) && pb) {
                shr_pb_printf(pb,
                              "Match-found(l=%u in-use=%u): ltid=%s "
                              "lkup_type=%s SPT:estate_idx=%u pt_eidx=%u "
                              "ltlkup_eidx=%u.\n",
                              l, lkup_state->in_use, lt_name,
                              bcmptm_sbr_lt_lkup_str(lookup_type),
                              *spt_estate_index, *sbr_pt_entry_index,
                              *tltlkup_estate_index);
            }
            SHR_EXIT();
        }
    }
    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE) && pb) {
        shr_pb_printf(pb, "NoMatch: ltid=%s lkup_type=%s - rv=%s\n",
                      lt_name,
                      bcmptm_sbr_lt_lkup_str(lookup_type),
                      shr_errmsg(SHR_E_NOT_FOUND));
    }
    SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
exit:
    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE) && pb) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
        pb = NULL;
    }
    SHR_FUNC_EXIT();
}

static void
sbr_tm_change_actions_dump(int unit,
                           bool only_check_rsrc,
                           const char *tile_name,
                           const char *cur_tm,
                           const char *new_tm,
                           sbr_tm_chg_actions_list_t *tm_act,
                           shr_pb_t *pb)
{
    uint16_t i = 0;
    int rv = SHR_E_INTERNAL;
    const char *lt_name = NULL;

    if (tile_name && cur_tm && new_tm && tm_act) {
        shr_pb_printf(pb, "Tile=%s Cur_TM=%s New_TM=%s (only_check_rsrc=%-1u)\n"
                          "-------------------------------------------------\n",
                      tile_name, cur_tm, new_tm, only_check_rsrc);
        shr_pb_printf(pb, "Current TM Actions=%u\n"
                          "----------------------\n", tm_act->current_acount);
        shr_pb_printf(pb, "    ltid lkup_t lkup_op spt_op p m sid eid\n"
                          "----------------------------------------------\n");
        for (i = 0; i < tm_act->current_acount; i++) {
            rv = bcmlrd_field_value_to_symbol(unit,
                                              TABLE_INFOt,
                                              TABLE_INFOt_TABLE_IDf,
                                              tm_act->current[i].ltid,
                                              &lt_name);
            if (SHR_FAILURE(rv)) {
                lt_name = NULL;
            }
            shr_pb_printf(pb,
                          "%2u.%-40s/%-5u %-10s %-10s %-10s %-1u %-1u %-32s/"
                          "%-5u %-3d\n",
                          i, lt_name, tm_act->current[i].ltid,
                          bcmptm_sbr_lt_lkup_str
                            (tm_act->current[i].lookup_type),
                          bcmptm_sbr_tm_lt_lookup_oper_str
                            (tm_act->current[i].ltlkup_oper),
                          bcmptm_sbr_pt_ent_oper_str
                            (tm_act->current[i].spt_eoper),
                          tm_act->current[i].pending,
                          tm_act->current[i].matched,
                          bcmdrd_pt_sid_to_name
                            (unit, tm_act->current[i].spt_sid),
                          tm_act->current[i].spt_sid,
                          (int)tm_act->current[i].sbr_pt_entry_index);
        }

        shr_pb_printf(pb, "New TM Actions=%u\n"
                          "-------------------\n", tm_act->new_acount);
        shr_pb_printf(pb, "   ltid lkup_t lkup_op spt_op p m sid eid\n"
                          "---------------------------------------------\n");
        for (i = 0; i < tm_act->new_acount; i++) {
            rv = bcmlrd_field_value_to_symbol(unit,
                                              TABLE_INFOt,
                                              TABLE_INFOt_TABLE_IDf,
                                              tm_act->new[i].ltid,
                                              &lt_name);
            if (SHR_FAILURE(rv)) {
                lt_name = NULL;
            }
            shr_pb_printf(pb,
                          "%2u.%-40s/%-5u %-10s %-10s %-10s %-1u %-1u %-32s/"
                          "%-5u %-3d\n",
                          i, lt_name, tm_act->new[i].ltid,
                          bcmptm_sbr_lt_lkup_str(tm_act->new[i].lookup_type),
                          bcmptm_sbr_tm_lt_lookup_oper_str
                            (tm_act->new[i].ltlkup_oper),
                          bcmptm_sbr_pt_ent_oper_str(tm_act->new[i].spt_eoper),
                          tm_act->new[i].pending,
                          tm_act->new[i].matched,
                          bcmdrd_pt_sid_to_name(unit, tm_act->new[i].spt_sid),
                          tm_act->new[i].spt_sid,
                          (int)tm_act->new[i].sbr_pt_entry_index);
        }
    }
    return;
}

static int
sbr_tm_change_actions_install(int unit,
                              bool only_check_rsrc,
                              bcmltd_sid_t lt_id,
                              const bcmltd_op_arg_t *op_arg,
                              sbr_tm_chg_actions_list_t *tm_act)
{
    uint16_t i;
    uint16_t pt;
    uint32_t ltlkup_base_idx = SBR_UINT32_INVALID;
    shr_pb_t *pb = NULL; /* Print buffer. */
    sbr_dev_info_t *di = sbr_dev_info[unit];
    sbr_tile_lt_lkup_action_t *act = NULL;
    bcmptm_sbr_lt_lookup_state_tile_t *tltlkup_state = NULL;
    const char *cur_tm_name = NULL, *new_tm_name=NULL, *tile_name = NULL;
    const char *lt_name = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(di, SHR_E_INTERNAL);
    SHR_NULL_CHECK(tm_act, SHR_E_PARAM);

    /*If there are valid current actions, then the current ptr must be valid. */
    if (tm_act->current_acount) {
        SHR_NULL_CHECK(tm_act->current, SHR_E_PARAM);
    }

    /* If there are valid new actions, then the new ptr must be valid. */
    if (tm_act->new_acount) {
        SHR_NULL_CHECK(tm_act->new, SHR_E_PARAM);
    }
    /* Get tile name. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_value_to_symbol(unit, DEVICE_EM_TILE_INFOt,
                                      DEVICE_EM_TILE_INFOt_DEVICE_EM_TILE_IDf,
                                      tm_act->tile, &tile_name));
    /* Get tile_mode name. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_value_to_symbol(unit,
                                      DEVICE_EM_TILE_INFOt,
                                      DEVICE_EM_TILE_INFOt_MODEf,
                                      tm_act->cur_tm,
                                      &cur_tm_name));
    SHR_IF_ERR_EXIT
        (bcmlrd_field_value_to_symbol(unit,
                                      DEVICE_EM_TILE_INFOt,
                                      DEVICE_EM_TILE_INFOt_MODEf,
                                      tm_act->new_tm,
                                      &new_tm_name));
    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        pb = shr_pb_create();
        shr_pb_printf(pb, "Actions status (p = pending, m = match):\n");
        sbr_tm_change_actions_dump(unit, only_check_rsrc, tile_name,
                                   cur_tm_name, new_tm_name,
                                   tm_act, pb);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "%s"), shr_pb_str(pb)));
        shr_pb_reset(pb);
        shr_pb_printf(pb,
                      "Processed following actions (only_check_rsrc=%-1u):\n"
                      "T=%-18s CUR_TM=%-20s NEW_TM=%-20s\n"
                      "===================================================\n",
                      only_check_rsrc, tile_name, cur_tm_name, new_tm_name);
        shr_pb_printf(pb, "cur_tm_actions\n"
                          "--------------\n");
    }
    /*
     * First perform current LT.LOOKUP actions, so that any in-use SPT
     * entries will be freed and available for allocs when the new LT.LOOKUP
     * add operations needs them.
     */
    for (i = 0; i < tm_act->current_acount; i++) {
        act = (tm_act->current + i);
        SHR_NULL_CHECK(act, SHR_E_INTERNAL);
        /* Get lt_name corresponding to this ltid. */
        SHR_IF_ERR_EXIT
            (bcmlrd_field_value_to_symbol(unit, TABLE_INFOt,
                                          TABLE_INFOt_TABLE_IDf, act->ltid,
                                          &lt_name));
        switch (act->spt_eoper) {
            case SBR_PT_ENT_FREE_HW_CLEAR:
                SHR_IF_ERR_EXIT
                    (sbr_pt_entry_free_clear(unit, only_check_rsrc,
                                             tm_act->tile, lt_id, op_arg, 1,
                                             act));
                break;
            case SBR_PT_ENT_NOOP:
                break;
            default:
                /* Invalid spt_eoper. */
                SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        switch (act->ltlkup_oper) {
            case SBR_TM_LT_LOOKUP_DELETE:
                if (act->tltlkup_estate_index
                    >= sbr_dev_info[unit]->lt_lkup_count) {
                    SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                        (BSL_META_U(unit,
                                                    "Invalid "
                                                    "tltlkup_estate_index=%u "
                                                    "- rv=%s.\n"),
                                         act->tltlkup_estate_index,
                                         shr_errmsg(SHR_E_PARAM)));
                }
                tltlkup_state = di->tile_lkup_state + act->tltlkup_estate_index;
                SHR_NULL_CHECK(tltlkup_state, SHR_E_INTERNAL);
                if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
                    LOG_DEBUG(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "Done: SBR_TM_LT_LOOKUP_DELETE - t=%s"
                                          " ltid=%s lkup_t=%s in_use=%u.\n"),
                               tile_name, lt_name,
                               bcmptm_sbr_lt_lkup_str(act->lookup_type),
                               tltlkup_state->in_use));
                }
                if (tltlkup_state->in_use) {
                    tltlkup_state->spt_sid = di->invalid_ptid;
                    tltlkup_state->entry_index = SBR_UINT16_INVALID;
                    tltlkup_state->in_use = FALSE;
                }
                act->tltlkup_estate_index = SBR_UINT32_INVALID;
                break;
            case SBR_TM_LT_LOOKUP_NOOP:
                break;
            default:
                /* Invalid ltlkup_oper for current tile_mode + LT.LOOKUP. */
                SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        act->pending = FALSE;
        if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
            shr_pb_printf(pb, "[%2u]. LT=%-40s L=%-10s lkup_oper=%-10s "
                          "tltlkup_e_i=%d tltlkup_info_i=%d SPT=%-32s "
                          "spt_eoper=%-10s estate_i=%d pt_ent_i=%d\n",
                          i, lt_name,
                          bcmptm_sbr_lt_lkup_str(act->lookup_type),
                          bcmptm_sbr_tm_lt_lookup_oper_str(act->ltlkup_oper),
                          (int)act->tltlkup_estate_index,
                          (int)act->tltlkup_info_index,
                          bcmdrd_pt_sid_to_name(unit, act->spt_sid),
                          bcmptm_sbr_pt_ent_oper_str(act->spt_eoper),
                          (int)act->spt_estate_index,
                          (int)act->sbr_pt_entry_index);
        }
    }
    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        shr_pb_printf(pb, "new_tm_actions\n"
                          "--------------\n");
    }
    for (i = 0; i < tm_act->new_acount; i++) {
        act = (tm_act->new + i);
        SHR_NULL_CHECK(act, SHR_E_INTERNAL);
        /* Get lt_name corresponding to this ltid. */
        SHR_IF_ERR_EXIT
            (bcmlrd_field_value_to_symbol(unit, TABLE_INFOt,
                                          TABLE_INFOt_TABLE_IDf, act->ltid,
                                          &lt_name));
        switch (act->spt_eoper) {
            case SBR_PT_ENT_ALLOC_HW_INSTALL:
                act->spt_estate_index = SBR_UINT16_INVALID;
                act->sbr_pt_entry_index = SBR_UINT16_INVALID;
                for (pt = 0; pt < di->spt_count; pt++) {
                    if (di->spt_state[pt].drd_sid == act->spt_sid
                        || di->spt_state[pt].ext_drd_sid == act->spt_sid) {
                        SHR_IF_ERR_EXIT
                            (sbr_pt_entry_alloc_install(unit, only_check_rsrc,
                                                        tm_act->tile, lt_id,
                                                        op_arg, pt, act));
                        break;
                    }
                }
                break;
            case SBR_PT_ENT_REF_INCR:
                SHR_IF_ERR_EXIT
                    (sbr_pt_entry_ref_incr(unit, tm_act->tile, lt_id, op_arg,
                                           act));
                break;
            case SBR_PT_ENT_NOOP:
                break;
            default:
                /* Invalid spt_eoper for new tile_mode + LT.LOOKUP. */
                SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        switch (act->ltlkup_oper) {
            case SBR_TM_LT_LOOKUP_ADD:
                SHR_IF_ERR_EXIT
                    (sbr_ltlkup_tile_lts_tcam_update(unit, only_check_rsrc,
                                                     tm_act->tile,
                                                     tm_act->new_tm, lt_id,
                                                     op_arg,
                                                     act));
                if (act->lookup_type == BCMPTM_SBR_LOOKUP0) {
                    ltlkup_base_idx
                        = di->tile_state[tm_act->tile].lt_lookup0_base_index;
                } else {
                    ltlkup_base_idx
                        = di->tile_state[tm_act->tile].lt_lookup1_base_index;
                }
                tltlkup_state = di->tile_lkup_state
                                    + (ltlkup_base_idx
                                    + act->tltlkup_info_index);
                tltlkup_state->spt_sid = act->spt_sid;
                tltlkup_state->entry_index = act->spt_estate_index;
                tltlkup_state->in_use = TRUE;
                act->tltlkup_estate_index = (ltlkup_base_idx
                                                + act->tltlkup_info_index);
                if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
                    LOG_DEBUG(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "Done: SBR_TM_LT_LOOKUP_ADD - t=%s "
                                          "ltid=%s lkup_t=%s in_use=%u.\n"),
                               tile_name, lt_name,
                               bcmptm_sbr_lt_lkup_str(act->lookup_type),
                               tltlkup_state->in_use));
                }
                break;
            case SBR_TM_LT_LOOKUP_LTS_TCAM_UPDATE:
                SHR_IF_ERR_EXIT
                    (sbr_ltlkup_tile_lts_tcam_update(unit, only_check_rsrc,
                                                     tm_act->tile,
                                                     tm_act->new_tm,
                                                     lt_id, op_arg,
                                                     act));
                if (act->lookup_type == BCMPTM_SBR_LOOKUP0) {
                    ltlkup_base_idx
                        = di->tile_state[tm_act->tile].lt_lookup0_base_index;
                } else {
                    ltlkup_base_idx
                        = di->tile_state[tm_act->tile].lt_lookup1_base_index;
                }
                tltlkup_state = di->tile_lkup_state
                                    + (ltlkup_base_idx
                                    + act->tltlkup_info_index);
                tltlkup_state->spt_sid = act->spt_sid;
                tltlkup_state->entry_index = act->spt_estate_index;
                tltlkup_state->in_use = TRUE;
                act->tltlkup_estate_index = (ltlkup_base_idx
                                                + act->tltlkup_info_index);
               if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
                    LOG_DEBUG(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "Done: "
                                          "SBR_TM_LT_LOOKUP_LTS_TCAM_UPDATE "
                                          "- t=%s ltid=%s lkup_t=%s "
                                          "in_use=%u.\n"),
                               tile_name, lt_name,
                               bcmptm_sbr_lt_lkup_str(act->lookup_type),
                               tltlkup_state->in_use));
                }
                break;
            case SBR_TM_LT_LOOKUP_NOOP:
                break;
            default:
                /* Invalid ltlkup_oper for new tile_mode + LT.LOOKUP. */
                SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        act->pending = FALSE;
        if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
            shr_pb_printf(pb, "[%2u]. LT=%-40s L=%-10s lkup_oper=%-10s "
                          "tltlkup_e_i=%d tltlkup_info_i=%d SPT=%-32s "
                          "spt_eoper=%-10s estate_i=%d pt_ent_i=%d.\n",
                          i, lt_name,
                          bcmptm_sbr_lt_lkup_str(act->lookup_type),
                          bcmptm_sbr_tm_lt_lookup_oper_str(act->ltlkup_oper),
                          (int)act->tltlkup_estate_index,
                          (int)act->tltlkup_info_index,
                          bcmdrd_pt_sid_to_name(unit, act->spt_sid),
                          bcmptm_sbr_pt_ent_oper_str(act->spt_eoper),
                          (int)act->spt_estate_index,
                          (int)act->sbr_pt_entry_index);
        }
    }
    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "%s"), shr_pb_str(pb)));
        shr_pb_reset(pb);
        shr_pb_printf(pb, "Exit: Actions status\n");
        sbr_tm_change_actions_dump(unit, only_check_rsrc, tile_name,
                                   cur_tm_name, new_tm_name,
                                   tm_act, pb);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "%s"), shr_pb_str(pb)));
    }
exit:
    if (BSL_LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        if (SHR_FUNC_ERR() && pb) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "%s"), shr_pb_str(pb)));
        }
        shr_pb_destroy(pb);
        pb = NULL;
    }
    SHR_FUNC_EXIT();
}

static int
sbr_tm_lt_lookup_match_check(int unit,
                             const bcmptm_sbr_lt_lookup_info_tile_mode_t *s,
                             const bcmptm_sbr_lt_lookup_info_tile_mode_t *list,
                             uint16_t l_count,
                             uint16_t *mindex)
{
    uint16_t i;
    const char *lt_name = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(s, SHR_E_PARAM);
    SHR_NULL_CHECK(mindex, SHR_E_PARAM);
    if (l_count == 0 || list == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT
        (bcmlrd_field_value_to_symbol(unit, TABLE_INFOt, TABLE_INFOt_TABLE_IDf,
                                      s->ltid, &lt_name));

    *mindex = SBR_UINT16_INVALID;
    for (i = 0; i < l_count; i++) {
        if (s->lookup_type == list[i].lookup_type
            && s->ltid == list[i].ltid) {
            *mindex = i;
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                             "tmltlkup-Match: ltid=%s lkup_type=%s@i=%u.\n"),
                       lt_name, bcmptm_sbr_lt_lkup_str(s->lookup_type), i));
            SHR_EXIT();
        }
    }
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "tmltlkup-NoMatch: ltid=%s lkup_type=%s.\n"),
               lt_name, bcmptm_sbr_lt_lkup_str(s->lookup_type)));
    SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT();

}
static int
sbr_lt_lookup_spt_entry_req_check(int unit,
                          const bcmptm_sbr_lt_lookup_info_tile_t *ltlkup_info,
                          uint32_t ltlkup_info_count,
                          uint16_t tile,
                          bcmltd_sid_t ltid,
                          bcmptm_sbr_lt_lookup_t lookup_type,
                          uint32_t *tltlkup_info_index,
                          bool *sbr_ent_required)
{
    uint32_t l; /* LT lookup index. */
    const bcmptm_sbr_lt_lookup_info_tile_t *v_ti = NULL; /* Var tile info. */
    const char *lt_name = NULL, *tile_name = NULL;

    SHR_FUNC_ENTER(unit);
    /* Validate input parameters. */
    SHR_NULL_CHECK(ltlkup_info, SHR_E_PARAM);
    SHR_NULL_CHECK(sbr_ent_required, SHR_E_PARAM);
    SHR_NULL_CHECK(tltlkup_info_index, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_value_to_symbol(unit, DEVICE_EM_TILE_INFOt,
                                      DEVICE_EM_TILE_INFOt_DEVICE_EM_TILE_IDf,
                                      tile, &tile_name));
    SHR_IF_ERR_EXIT
        (bcmlrd_field_value_to_symbol(unit, TABLE_INFOt, TABLE_INFOt_TABLE_IDf,
                                      ltid, &lt_name));
    for (l = 0; l < ltlkup_info_count; l++) {
        v_ti = ltlkup_info + l;
        SHR_NULL_CHECK(v_ti, SHR_E_INTERNAL);
        /* Currently not checking for "tile" number match based on spec. */
        if (v_ti->ltid == ltid && v_ti->lookup_type == lookup_type
            && v_ti->sbr_profile_data != NULL) {
            *tltlkup_info_index = l;
            *sbr_ent_required = TRUE;
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "SBR entry required: tile=%s ltid=%s "
                                  "lkup_type=%s - rv=%s\n"),
                       tile_name, lt_name,
                       bcmptm_sbr_lt_lkup_str(lookup_type),
                       shr_errmsg(SHR_E_NONE)));
            SHR_EXIT();
        }
    }
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "SBR entry not required: tile=%s ltid=%s "
                          "lkup_type=%s - rv=%s\n"),
               tile_name, lt_name,
               bcmptm_sbr_lt_lkup_str(lookup_type),
               shr_errmsg(SHR_E_NOT_FOUND)));
    SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT();
}

static int
sbr_tm_ltlkup_spt_entry_exists_check(int unit,
                                     uint32_t spt_sid,
                                     bcmltd_sid_t ltid,
                                     bcmptm_sbr_lt_lookup_t lookup_type,
                                     uint16_t *spt_estate_index,
                                     uint16_t *sbr_pt_entry_index)
{
    const sbr_dev_info_t *di = sbr_dev_info[unit]; /* Device info PTR. */
    const bcmptm_sbr_pt_state_t *spt_state = sbr_dev_info[unit]->spt_state;
    const bcmptm_sbr_entry_state_t *spt_estate = NULL;
    uint16_t pt;
    uint32_t e;
    const char *lt_name = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_value_to_symbol(unit, TABLE_INFOt, TABLE_INFOt_TABLE_IDf,
                                      ltid, &lt_name));
    for (pt = 0; pt < di->spt_count; pt++) {
        if (spt_state[pt].drd_sid == spt_sid
            || spt_state[pt].ext_drd_sid == spt_sid) {
            for (e = 0; e < spt_state[pt].entries_count; e++) {
                spt_estate = di->spt_estate +
                                (spt_state[pt].estate_base_index + e);
                SHR_NULL_CHECK(spt_estate, SHR_E_INTERNAL);
                if (spt_estate->ltid == ltid
                    && spt_estate->lookup_type == lookup_type
                    && (spt_estate->usage_mode == BCMPTM_SBR_ENTRY_IN_USE
                        || spt_estate->usage_mode
                            == BCMPTM_SBR_ENTRY_RSVD_DEFAULT)) {
                    *spt_estate_index = (spt_state[pt].estate_base_index + e);
                    *sbr_pt_entry_index = e;
                    LOG_DEBUG(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "SPT(%s) match@estate_index=%u "
                                          "pt_entry_index=%u for ltid=%s "
                                          "lkup_type=%s - rv=%s\n"),
                               bcmdrd_pt_sid_to_name(unit, spt_sid),
                               *spt_estate_index, *sbr_pt_entry_index,
                               lt_name,
                               bcmptm_sbr_lt_lkup_str(lookup_type),
                               shr_errmsg(SHR_E_NONE)));
                    SHR_EXIT();
                }
            }
        }
    }
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "SPT(%s) estate no match for ltid=%s lkup_type=%s "
                          "- rv=%s\n"),
               bcmdrd_pt_sid_to_name(unit, spt_sid), lt_name,
               bcmptm_sbr_lt_lkup_str(lookup_type),
               shr_errmsg(SHR_E_NOT_FOUND)));
    SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT();
}

static int
sbr_tile_enable(int unit,
                uint16_t tile,
                bcmptm_sbr_tile_mode_t new_tm,
                bool only_check_rsrc,
                bcmptm_sbr_tile_state_t *ts,
                bcmltd_sid_t lt_id,
                const bcmltd_op_arg_t *op_arg)
{
    uint16_t i, l; /* LT lookup index. */
    const sbr_dev_info_t *di = sbr_dev_info[unit]; /* Device info PTR. */
    const bcmptm_sbr_var_drv_t *vi = sbr_dev_info[unit]->var;
    const bcmptm_sbr_tile_info_t *v_ti = NULL;
    const bcmptm_sbr_tile_mode_info_t *v_tmi_new; /* Tile mode info. */
    int rv = SHR_E_INTERNAL; /* Return value. */
    sbr_tm_chg_actions_list_t tm_act;
    bool sbr_ent_required = FALSE;

    SHR_FUNC_ENTER(unit);

    /* Initialize tile mode actions structure. */
    sbr_tm_chg_actions_list_t_init(&tm_act);
    /* Validate input parameters. */
    SHR_NULL_CHECK(di, SHR_E_PARAM);
    SHR_NULL_CHECK(ts, SHR_E_PARAM);
    SHR_NULL_CHECK(vi, SHR_E_PARAM);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
    /* Get the tile info ptr for this tile. */
    v_ti = vi->sbr_tile_info + tile;
    SHR_NULL_CHECK(v_ti, SHR_E_PARAM);

    /* Check if this tile is already in the requested new tile_mode. */
    if (ts->tile_mode == new_tm) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Tile=%u already in tm=%u - rv=%s.\n"),
                             tile,
                             new_tm,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* Get the tile current tile_mode info. */
    v_tmi_new = v_ti->tile_mode_info + (new_tm - v_ti->tile_mode[0]);
    SHR_NULL_CHECK(v_tmi_new, SHR_E_INTERNAL);
    tm_act.new_acount = v_tmi_new->lt_lookup0_info_count
                                + v_tmi_new->lt_lookup1_info_count;
    if (tm_act.new_acount == 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Empty LT.LOOKUP list (count=%u) on tile=%u "
                                "for new tile_mode=%u.\n"),
                     tm_act.new_acount, tile, new_tm));
        ts->tile_mode = new_tm;
        SHR_EXIT();
    }
    SHR_ALLOC(tm_act.new,
              sizeof(*(tm_act.new)) * tm_act.new_acount,
              "bcmptmSbrTmNewAct");
    SHR_NULL_CHECK(tm_act.new, SHR_E_MEMORY);
    sal_memset(tm_act.new, 0, sizeof(*(tm_act.new)) * tm_act.new_acount);
    /* Save the TM change params in the TM action struct. */
    tm_act.tile = tile;
    tm_act.new_tm = new_tm;
    tm_act.cur_tm = ts->tile_mode;

    /*
     * As this tile is being enabled from disabled state, all new LT.LOOKUPs
     * must be added, corresponding SPT entries must be allocated (if
     * required and does not exists already) or increment SBR entry ref count
     * (if required and already exists because tiles share the same SBR profile
     * table and LT.LOOKUP).
     */
    for (l = 0; l < tm_act.new_acount; l++) {
        /* Initialize the values. */
        sbr_ent_required = FALSE;
        tm_act.new[l].tltlkup_info_index = SBR_UINT32_INVALID;

        if (l < v_tmi_new->lt_lookup0_info_count) {
            tm_act.new[l].ltid = v_tmi_new->lt_lookup0_info[l].ltid;
            tm_act.new[l].lookup_type =
                                    v_tmi_new->lt_lookup0_info[l].lookup_type;
            tm_act.new[l].sid_lts_tcam_data_only =
                        v_tmi_new->lt_lookup0_info[l].sid_lts_tcam_data_only;
            /* sbr_profile_data ptr must be not-null when sbr_ent_required=1 */
            rv = sbr_lt_lookup_spt_entry_req_check(unit,
                                            v_ti->sbr_lt_lookup0_info,
                                            v_ti->sbr_lt_lookup0_info_count,
                                            tile,
                                            tm_act.new[l].ltid,
                                            tm_act.new[l].lookup_type,
                                            &tm_act.new[l].tltlkup_info_index,
                                            &sbr_ent_required);
        } else {
            i = l - v_tmi_new->lt_lookup0_info_count;
            tm_act.new[l].ltid = v_tmi_new->lt_lookup1_info[i].ltid;
            tm_act.new[l].lookup_type =
                                    v_tmi_new->lt_lookup1_info[i].lookup_type;
            tm_act.new[l].sid_lts_tcam_data_only =
                        v_tmi_new->lt_lookup1_info[i].sid_lts_tcam_data_only;
            rv = sbr_lt_lookup_spt_entry_req_check(unit,
                                            v_ti->sbr_lt_lookup1_info,
                                            v_ti->sbr_lt_lookup1_info_count,
                                            tile,
                                            tm_act.new[l].ltid,
                                            tm_act.new[l].lookup_type,
                                            &tm_act.new[l].tltlkup_info_index,
                                            &sbr_ent_required);
        }
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);

        if (sbr_ent_required) {
            /*
             * Check if this new LT.LOOKUP on this tile is already pointing to
             * a valid SBR entry in the SPT entry state table. If it is,
             * then return E_EXISTS error, as a new LT.LOOKUP on a tile that
             * requires an SBR entry must not be pointing any SBR entry.
             * (check if this LT.LOOKUP is handled correctly in the tile
             * LT.LOOKUP delete sequence).
             */
            tm_act.new[l].spt_estate_index = SBR_UINT16_INVALID;
            tm_act.new[l].sbr_pt_entry_index = SBR_UINT16_INVALID;
            tm_act.new[l].tltlkup_estate_index = SBR_UINT32_INVALID;
            rv = sbr_tmi_ltlkup_spt_state_entry_index_get(unit, ts,
                                        tm_act.new[l].ltid,
                                        tm_act.new[l].lookup_type,
                                        &tm_act.new[l].spt_estate_index,
                                        &tm_act.new[l].sbr_pt_entry_index,
                                        &tm_act.new[l].tltlkup_estate_index);
            if (rv != SHR_E_NOT_FOUND) {
                SHR_IF_ERR_MSG_EXIT(SHR_E_EXISTS,
                                    (BSL_META_U(unit,
                                                "tile_mode=%u LT.LOOKUP "
                                                "(ltid=%u lkup_type=%u) on "
                                                "tile=%u already has a valid "
                                                "SBR entry@index=%u "
                                                "tltlkup_estate_index=%u "
                                                "spt_sid=%u - rv=%s.\n"),
                                     new_tm,
                                     tm_act.new[l].ltid,
                                     tm_act.new[l].lookup_type,
                                     tile,
                                     tm_act.new[l].spt_estate_index,
                                     tm_act.new[l].tltlkup_estate_index,
                                     tm_act.new[l].spt_sid,
                                     shr_errmsg(SHR_E_EXISTS)));
            }
            /*
             * To check if another tile is sharing the SBR profile table with
             * this tile and already has an entry allocated for this LT.LOOKUP
             * type, retrieve the SBR SID associated with this LT.LOOKUP from
             * v_tltlkup_info.sbr_profile_data[] array.
             */
            tm_act.new[l].spt_sid = di->invalid_ptid;
            rv = sbr_tltlkup_match_spt_sid_get(unit, tile, ts,
                                               tm_act.new[l].ltid,
                                               tm_act.new[l].lookup_type,
                                               &tm_act.new[l].spt_sid);
            SHR_IF_ERR_MSG_EXIT(rv,
                                (BSL_META_U(unit,
                                            "LT.LOOKUP requires SPT entry, but "
                                            "could not find the SBR SID in "
                                            "tile lt_lookup_info_tile_t[] for "
                                            "tile=%u ltid=%u lookup_type=%u -"
                                            "rv=%s.\n"),
                                 tile,
                                 tm_act.new[l].ltid,
                                 tm_act.new[l].lookup_type,
                                 shr_errmsg(rv)));

            /*
             * Check if another tile shares SBR table with this tile and
             * has an SBR entry for the same LT.LOOKUP, if it exists, only
             * reference count for this SBR entry needs to be incremented
             * and the LTS TCAM rule for this LT.LOOKUP on the new tile must
             * point to this existing entry in HW.
             */
            rv = sbr_tm_ltlkup_spt_entry_exists_check(unit,
                                          tm_act.new[l].spt_sid,
                                          tm_act.new[l].ltid,
                                          tm_act.new[l].lookup_type,
                                          &tm_act.new[l].spt_estate_index,
                                          &tm_act.new[l].sbr_pt_entry_index);
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
            if (rv == SHR_E_NONE) {
                /*
                 * SBR entry already exists for this LT.LOOKUP, increment
                 * the reference count and update the LTS TCAM rules if
                 * required.
                 */
                tm_act.new[l].spt_eoper = SBR_PT_ENT_REF_INCR;
                tm_act.new[l].ltlkup_oper = SBR_TM_LT_LOOKUP_LTS_TCAM_UPDATE;
            } else {
                tm_act.new[l].spt_eoper = SBR_PT_ENT_ALLOC_HW_INSTALL;
                tm_act.new[l].ltlkup_oper = SBR_TM_LT_LOOKUP_ADD;
                /* New SBR entry index to be allocated. */
                tm_act.new[l].spt_estate_index = SBR_UINT16_INVALID;
                tm_act.new[l].tltlkup_estate_index = SBR_UINT32_INVALID;
            }
            tm_act.new[l].pending = TRUE;
        } else {
            /*
             * No action required for this LT.LOOKUP as it is not pointing
             * to any SBR entry based on var sbr_lt_lookup0/1_info
             * sbr_profile_data.
             */
            tm_act.new[l].spt_eoper = SBR_PT_ENT_NOOP;
            tm_act.new[l].ltlkup_oper = SBR_TM_LT_LOOKUP_NOOP;
            tm_act.new[l].spt_estate_index = SBR_UINT16_INVALID;
            tm_act.new[l].tltlkup_estate_index = SBR_UINT32_INVALID;
            tm_act.new[l].pending = FALSE;
        }
    }
    rv = sbr_tm_change_actions_install(unit, only_check_rsrc, lt_id, op_arg,
                                       &tm_act);
    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_MSG_EXIT(rv,
                            (BSL_META_U(unit,
                                        "Tile(=%u) mode(=%u) change error - "
                                        "rv=%s.\n"),
                             tile,
                             new_tm,
                             shr_errmsg(rv)));
    }
    ts->tile_mode = new_tm;
exit:
    SHR_FREE(tm_act.new);
    SHR_FUNC_EXIT();
}

static int
sbr_tile_disable(int unit,
                 uint16_t tile,
                 bcmptm_sbr_tile_mode_t tm_disable,
                 bool only_check_rsrc,
                 bcmptm_sbr_tile_state_t *ts,
                 bcmltd_sid_t lt_id,
                 const bcmltd_op_arg_t *op_arg)
{
    uint16_t i, l; /* LT lookup index. */
    uint32_t v_tltlkup_info_count = 0;
    const sbr_dev_info_t *di = sbr_dev_info[unit]; /* Device info PTR. */
    const bcmptm_sbr_var_drv_t *vi = NULL;
    const bcmptm_sbr_tile_info_t *v_ti = NULL;
    const bcmptm_sbr_tile_mode_info_t *v_tmi_cur; /* Tile mode info. */
    const bcmptm_sbr_lt_lookup_info_tile_t *v_tltlkup_info = NULL;
    int rv = SHR_E_INTERNAL; /* Return value. */
    sbr_tm_chg_actions_list_t tm_act; /* Tile mode change actions. */
    bool sbr_ent_required = FALSE;

    SHR_FUNC_ENTER(unit);

    /* Initialize tile mode actions structure. */
    sbr_tm_chg_actions_list_t_init(&tm_act);
    /* Validate input parameters. */
    SHR_NULL_CHECK(ts, SHR_E_PARAM);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
    SHR_NULL_CHECK(di, SHR_E_PARAM);

    vi = sbr_dev_info[unit]->var;
    SHR_NULL_CHECK(vi, SHR_E_PARAM);

    v_ti = vi->sbr_tile_info + tile;
    SHR_NULL_CHECK(v_ti, SHR_E_PARAM);

    /* Check if this tile is already in disabled mode. */
    if (ts->tile_mode == tm_disable) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Tile=%u already DISABLED - rv=%s.\n"),
                             tile,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* Get the tile current tile_mode info. */
    v_tmi_cur = v_ti->tile_mode_info + (ts->tile_mode - v_ti->tile_mode[0]);
    SHR_NULL_CHECK(v_tmi_cur, SHR_E_INTERNAL);
    tm_act.current_acount = v_tmi_cur->lt_lookup0_info_count
                                + v_tmi_cur->lt_lookup1_info_count;
    if (tm_act.current_acount == 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Empty LT.LOOKUP list (count=%u) on tile=%u "
                                "for current tile_mode=%u.\n"),
                     tm_act.current_acount, tile, ts->tile_mode));
        ts->tile_mode = tm_disable;
        SHR_EXIT();
    }
    SHR_ALLOC(tm_act.current,
              sizeof(*(tm_act.current)) * tm_act.current_acount,
              "bcmptmSbrTmCurAct");
    SHR_NULL_CHECK(tm_act.current, SHR_E_MEMORY);
    sal_memset(tm_act.current, 0,
               sizeof(*(tm_act.current)) * tm_act.current_acount);
    /* Save the TM change params in the TM action struct. */
    tm_act.tile = tile;
    tm_act.cur_tm = ts->tile_mode;
    tm_act.new_tm = tm_disable;

    for (l = 0; l < tm_act.current_acount; l++) {
        tm_act.current[l].sid_lts_tcam_data_only = di->invalid_ptid;
        if (l < v_tmi_cur->lt_lookup0_info_count) {
            tm_act.current[l].ltid = v_tmi_cur->lt_lookup0_info[l].ltid;
            tm_act.current[l].lookup_type
                                = v_tmi_cur->lt_lookup0_info[l].lookup_type;
            v_tltlkup_info = v_ti->sbr_lt_lookup0_info;
            v_tltlkup_info_count = v_ti->sbr_lt_lookup0_info_count;
        } else {
            i = l - v_tmi_cur->lt_lookup0_info_count;
            tm_act.current[l].ltid = v_tmi_cur->lt_lookup1_info[i].ltid;
            tm_act.current[l].lookup_type
                                = v_tmi_cur->lt_lookup1_info[i].lookup_type;
            v_tltlkup_info = v_ti->sbr_lt_lookup1_info;
            v_tltlkup_info_count = v_ti->sbr_lt_lookup1_info_count;
        }
        /*
         * As this tile is being disabled, in-use LT.LOOKUPs must be deleted,
         * find if this LT.LOOKUP is using an SBR entry.
         */
        sbr_ent_required = FALSE;
        tm_act.current[l].tltlkup_info_index = SBR_UINT32_INVALID;

        rv = sbr_lt_lookup_spt_entry_req_check(unit, v_tltlkup_info,
                                        v_tltlkup_info_count,
                                        tile,
                                        tm_act.current[l].ltid,
                                        tm_act.current[l].lookup_type,
                                        &tm_act.current[l].tltlkup_info_index,
                                        &sbr_ent_required);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
        if (sbr_ent_required) {
            /* Get the in-use SBR entry index for this LT.LOOKUP. */
            tm_act.current[l].spt_estate_index = SBR_UINT16_INVALID;
            tm_act.current[l].sbr_pt_entry_index = SBR_UINT16_INVALID;
            tm_act.current[l].tltlkup_estate_index = SBR_UINT32_INVALID;
            rv = sbr_tmi_ltlkup_spt_state_entry_index_get(unit,
                                    ts,
                                    tm_act.current[l].ltid,
                                    tm_act.current[l].lookup_type,
                                    &tm_act.current[l].spt_estate_index,
                                    &tm_act.current[l].sbr_pt_entry_index,
                                    &tm_act.current[l].tltlkup_estate_index);
            SHR_IF_ERR_MSG_EXIT(rv,
                                (BSL_META_U(unit,
                                            "SPT estate index not found for "
                                            "tile=%u cur_tm=%u new_tm=%u "
                                            "l=%u:ltid=%u lkup_type=%u\n"),
                                 tile, ts->tile_mode, tm_disable, l,
                                 tm_act.new[l].ltid,
                                 tm_act.new[l].lookup_type));
            /* Get the SBR profile table SID. */
            tm_act.current[l].spt_sid = di->invalid_ptid;
            rv = sbr_tltlkup_match_spt_sid_get(unit, tile, ts,
                                               tm_act.current[l].ltid,
                                               tm_act.current[l].lookup_type,
                                               &tm_act.current[l].spt_sid);
            SHR_IF_ERR_MSG_EXIT(rv,
                                (BSL_META_U(unit,
                                        "SPT_SID required but not found for "
                                        "tile=%u ltid=%u lookup_type=%u -"
                                        "rv=%s.\n"),
                                 tile,
                                 tm_act.current[l].ltid,
                                 tm_act.current[l].lookup_type,
                                 shr_errmsg(rv)));
            tm_act.current[l].ltlkup_oper = SBR_TM_LT_LOOKUP_DELETE;
            tm_act.current[l].spt_eoper = SBR_PT_ENT_FREE_HW_CLEAR;
            tm_act.current[l].pending = TRUE;
        } else {
            tm_act.current[l].ltlkup_oper = SBR_TM_LT_LOOKUP_NOOP;
            tm_act.current[l].spt_eoper = SBR_PT_ENT_NOOP;
            tm_act.current[l].pending = FALSE;
        }
    }
    rv = sbr_tm_change_actions_install(unit, only_check_rsrc, lt_id, op_arg,
                                       &tm_act);
    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_MSG_EXIT(rv,
                            (BSL_META_U(unit,
                                        "Tile(=%u) mode(=%u) change error - "
                                        "rv=%s.\n"),
                             tile,
                             tm_disable,
                             shr_errmsg(rv)));
    }
    ts->tile_mode = tm_disable;
exit:
    SHR_FREE(tm_act.current);
    SHR_FUNC_EXIT();
}

static int
sbr_tile_mode_change(int unit,
                     uint16_t tile,
                     bcmptm_sbr_tile_mode_t new_tm,
                     bool only_check_rsrc,
                     bcmptm_sbr_tile_state_t *ts,
                     bcmltd_sid_t lt_id,
                     const bcmltd_op_arg_t *op_arg)
{
    int rv = SHR_E_INTERNAL;
    uint16_t l, i, new_ltlkup_mindex; /* LT lookup index. */
    const sbr_dev_info_t *di = sbr_dev_info[unit]; /* Device info PTR. */
    const bcmptm_sbr_var_drv_t *vi = sbr_dev_info[unit]->var;
    const bcmptm_sbr_tile_info_t *v_ti = NULL;
    const bcmptm_sbr_tile_mode_info_t *v_tmi_cur; /* Tile mode info. */
    const bcmptm_sbr_tile_mode_info_t *v_tmi_new; /* Tile mode info. */
    sbr_tm_chg_actions_list_t tm_act; /* Tile mode change actions. */
    bool sbr_ent_required;
    const char *lt_name = NULL;

    SHR_FUNC_ENTER(unit);
    /* Initialize tile mode actions structure. */
    sbr_tm_chg_actions_list_t_init(&tm_act);
    /* Validate input parameters. */
    SHR_NULL_CHECK(di, SHR_E_PARAM);
    SHR_NULL_CHECK(ts, SHR_E_PARAM);
    SHR_NULL_CHECK(vi, SHR_E_PARAM);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);

    /* Get the tile info pointer for this tile. */
    v_ti = vi->sbr_tile_info + tile;
    SHR_NULL_CHECK(v_ti, SHR_E_PARAM);
    /*
     * tile_mode_info entries in var_info are from TILE_MODE_MAP section
     * and per-tile supported LT.LOOKUP0/1 entries are listed only once in this
     * section. So, it is guaranteed that a LT.LOOKUP0/1 in v_tmi_cur[] will
     * have only one match in v_tmi_new[], if an LT.LOOKUP is supported both
     * in the current and the new tile modes.
     */
    v_tmi_new = v_ti->tile_mode_info + (new_tm - v_ti->tile_mode[0]);
    SHR_NULL_CHECK(v_tmi_new, SHR_E_INTERNAL);
    v_tmi_cur = v_ti->tile_mode_info + (ts->tile_mode - v_ti->tile_mode[0]);
    SHR_NULL_CHECK(v_tmi_cur, SHR_E_INTERNAL);

    tm_act.current_acount = v_tmi_cur->lt_lookup0_info_count
                                + v_tmi_cur->lt_lookup1_info_count;
    if (tm_act.current_acount > 0) {
        SHR_ALLOC(tm_act.current,
                  sizeof(*(tm_act.current)) * tm_act.current_acount,
                  "bcmptmSbrTmCurAct");
        SHR_NULL_CHECK(tm_act.current, SHR_E_MEMORY);
        sal_memset(tm_act.current, 0,
                   sizeof(*(tm_act.current)) * tm_act.current_acount);
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Empty LT.LOOKUP list (count=%u) on tile=%u "
                                "for current tile_mode=%u.\n"),
                     tm_act.current_acount, tile, ts->tile_mode));
    }

    tm_act.new_acount = v_tmi_new->lt_lookup0_info_count
                            + v_tmi_new->lt_lookup1_info_count;
    if (tm_act.new_acount > 0) {
        SHR_ALLOC(tm_act.new,
                  sizeof(*(tm_act.new)) * tm_act.new_acount,
                  "bcmptmSbrTmNewAct");
        SHR_NULL_CHECK(tm_act.new, SHR_E_MEMORY);
        sal_memset(tm_act.new, 0, sizeof(*(tm_act.new)) * tm_act.new_acount);
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Empty LT.LOOKUP list (count=%u) on tile=%u "
                                "for new tile_mode=%u.\n"),
                     tm_act.new_acount, tile, new_tm));
    }

    /* Save the TM change params in the TM action struct. */
    tm_act.tile = tile;
    tm_act.cur_tm = ts->tile_mode;
    tm_act.new_tm = new_tm;

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "Determine actions(cnt=%u) for cur tile_mode(%u).\n"),
               tm_act.current_acount, tm_act.cur_tm));
    /*
     * Find the list of current LT LOOKUPs that need to be deleted or require
     * LTS_TCAM rule SBR entry index update in the new tile mode.
     */
    for (l = 0; l < tm_act.current_acount; l++) {
        tm_act.current[l].sid_lts_tcam_data_only = di->invalid_ptid;
        if (l < v_tmi_cur->lt_lookup0_info_count) {
            tm_act.current[l].ltid = v_tmi_cur->lt_lookup0_info[l].ltid;
            tm_act.current[l].lookup_type
                            = v_tmi_cur->lt_lookup0_info[l].lookup_type;
            /* Search current LT.LOOKUP0 entry in the new LT.LOOKUP0 list. */
            rv = sbr_tm_lt_lookup_match_check(unit,
                                              (v_tmi_cur->lt_lookup0_info + l),
                                              v_tmi_new->lt_lookup0_info,
                                              v_tmi_new->lt_lookup0_info_count,
                                              &new_ltlkup_mindex);
        } else {
            /* Calculate index value for accessing v_tmi_cur[] element. */
            i = (l - v_tmi_cur->lt_lookup0_info_count);
            tm_act.current[l].ltid = v_tmi_cur->lt_lookup1_info[i].ltid;
            tm_act.current[l].lookup_type
                            = v_tmi_cur->lt_lookup1_info[i].lookup_type;

            /* Search current LT.LOOKUP1 entry in the new LT.LOOKUP1 list. */
            rv = sbr_tm_lt_lookup_match_check(unit,
                                              (v_tmi_cur->lt_lookup1_info + i),
                                              v_tmi_new->lt_lookup1_info,
                                              v_tmi_new->lt_lookup1_info_count,
                                              &new_ltlkup_mindex);
        }
        if (rv != SHR_E_NOT_FOUND) {
            SHR_IF_ERR_EXIT
                (bcmlrd_field_value_to_symbol(unit,
                                              TABLE_INFOt,
                                              TABLE_INFOt_TABLE_IDf,
                                              tm_act.current[l].ltid,
                                              &lt_name));
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "l=%u ltid=%s lookup_type=%s rv=%s.\n"),
                       l, lt_name,
                       bcmptm_sbr_lt_lkup_str(tm_act.current[l].lookup_type),
                       shr_errmsg(rv)));
        }
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
        /*
         * When rv == SHR_E_NONE, then LT.LOOK exists in the current and also
         * new tile modes.
         */
        if (rv == SHR_E_NONE) {
            tm_act.current[l].ltlkup_oper = SBR_TM_LT_LOOKUP_NOOP;
            tm_act.current[l].spt_eoper = SBR_PT_ENT_NOOP;
            tm_act.current[l].matched = TRUE;
            new_ltlkup_mindex = (l < v_tmi_cur->lt_lookup0_info_count)
                                    ? new_ltlkup_mindex
                                        : (v_tmi_new->lt_lookup0_info_count
                                           + new_ltlkup_mindex);
            tm_act.new[new_ltlkup_mindex].matched = TRUE;
            tm_act.new[new_ltlkup_mindex].spt_estate_index = SBR_UINT16_INVALID;
        } else {
            /*
             * No match (SHR_NOT_FOUND), so delete this LT.LOOKUP and free it's
             * SBR profile table entry if it is in-use.
             */
            sbr_ent_required = FALSE;
            tm_act.current[l].tltlkup_info_index = SBR_UINT32_INVALID;
            if (tm_act.current[l].lookup_type == BCMPTM_SBR_LOOKUP0) {
                rv = sbr_lt_lookup_spt_entry_req_check(unit,
                                        v_ti->sbr_lt_lookup0_info,
                                        v_ti->sbr_lt_lookup0_info_count,
                                        tile,
                                        tm_act.current[l].ltid,
                                        tm_act.current[l].lookup_type,
                                        &tm_act.current[l].tltlkup_info_index,
                                        &sbr_ent_required);
            } else {
                rv = sbr_lt_lookup_spt_entry_req_check(unit,
                                        v_ti->sbr_lt_lookup1_info,
                                        v_ti->sbr_lt_lookup1_info_count,
                                        tile,
                                        tm_act.current[l].ltid,
                                        tm_act.current[l].lookup_type,
                                        &tm_act.current[l].tltlkup_info_index,
                                        &sbr_ent_required);
            }
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
            if (sbr_ent_required) {
                tm_act.current[l].spt_estate_index = SBR_UINT16_INVALID;
                tm_act.current[l].sbr_pt_entry_index = SBR_UINT16_INVALID;
                rv = sbr_tmi_ltlkup_spt_state_entry_index_get(unit, ts,
                                    tm_act.current[l].ltid,
                                    tm_act.current[l].lookup_type,
                                    &tm_act.current[l].spt_estate_index,
                                    &tm_act.current[l].sbr_pt_entry_index,
                                    &tm_act.current[l].tltlkup_estate_index);
                SHR_IF_ERR_MSG_EXIT(rv,
                                    (BSL_META_U(unit,
                                                "SPT estate index not found "
                                                "for tile=%u cur_tm=%u "
                                                "new_tm=%u l=%u:ltid=%u "
                                                "lkup_type=%u\n"),
                                     tile, ts->tile_mode, new_tm, l,
                                     tm_act.new[l].ltid,
                                     tm_act.new[l].lookup_type));
                /* Get the SBR profile table SID. */
                tm_act.current[l].spt_sid = di->invalid_ptid;
                rv = sbr_tltlkup_match_spt_sid_get(unit, tile, ts,
                                               tm_act.current[l].ltid,
                                               tm_act.current[l].lookup_type,
                                               &tm_act.current[l].spt_sid);
                SHR_IF_ERR_MSG_EXIT(rv,
                                    (BSL_META_U(unit,
                                            "SPT_SID required but not found "
                                            "for tile=%u ltid=%u "
                                            "lookup_type=%u - rv=%s.\n"),
                                     tile,
                                     tm_act.current[l].ltid,
                                     tm_act.current[l].lookup_type,
                                     shr_errmsg(rv)));
                tm_act.current[l].spt_eoper = SBR_PT_ENT_FREE_HW_CLEAR;
                tm_act.current[l].ltlkup_oper = SBR_TM_LT_LOOKUP_DELETE;
                tm_act.current[l].pending = TRUE;
            } else {
                tm_act.current[l].ltlkup_oper = SBR_TM_LT_LOOKUP_NOOP;
                tm_act.current[l].spt_eoper = SBR_PT_ENT_NOOP;
                tm_act.current[l].pending = FALSE;
            }
        }
    }
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "Determine actions(cnt=%u) for new tile_mode(%u).\n"),
               tm_act.new_acount, new_tm));
    for (l = 0; l < tm_act.new_acount; l++) {
        /* Initialize the values. */
        sbr_ent_required = FALSE;
        tm_act.new[l].tltlkup_info_index = SBR_UINT32_INVALID;
        if (l < v_tmi_new->lt_lookup0_info_count) {
            tm_act.new[l].ltid = v_tmi_new->lt_lookup0_info[l].ltid;
            tm_act.new[l].lookup_type =
                            v_tmi_new->lt_lookup0_info[l].lookup_type;
            tm_act.new[l].sid_lts_tcam_data_only =
                        v_tmi_new->lt_lookup0_info[l].sid_lts_tcam_data_only;
            /* sbr_profile_data ptr must be not-null when sbr_ent_required=1 */
            rv = sbr_lt_lookup_spt_entry_req_check(unit,
                                            v_ti->sbr_lt_lookup0_info,
                                            v_ti->sbr_lt_lookup0_info_count,
                                            tile,
                                            tm_act.new[l].ltid,
                                            tm_act.new[l].lookup_type,
                                            &tm_act.new[l].tltlkup_info_index,
                                            &sbr_ent_required);
        } else {
            i = l - v_tmi_new->lt_lookup0_info_count;
            tm_act.new[l].ltid = v_tmi_new->lt_lookup1_info[i].ltid;
            tm_act.new[l].lookup_type =
                            v_tmi_new->lt_lookup1_info[i].lookup_type;
            tm_act.new[l].sid_lts_tcam_data_only =
                        v_tmi_new->lt_lookup1_info[i].sid_lts_tcam_data_only;
            rv = sbr_lt_lookup_spt_entry_req_check(unit,
                                            v_ti->sbr_lt_lookup1_info,
                                            v_ti->sbr_lt_lookup1_info_count,
                                            tile,
                                            tm_act.new[l].ltid,
                                            tm_act.new[l].lookup_type,
                                            &tm_act.new[l].tltlkup_info_index,
                                            &sbr_ent_required);
        }
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);

        if (sbr_ent_required) {
            if (tm_act.new[l].matched == TRUE) {
                /*
                 * For LT.LOOKUPs that match in the old and new tile_mode and
                 * need a SBR entry, an entry must already exists in the SPT
                 * entry state array, get the SPT entry index for updating the
                 * LTS TCAM rules for the new tile mode.
                 */
                tm_act.new[l].spt_estate_index = SBR_UINT16_INVALID;
                tm_act.new[l].sbr_pt_entry_index = SBR_UINT16_INVALID;
                tm_act.new[l].tltlkup_estate_index = SBR_UINT32_INVALID;
                rv = sbr_tmi_ltlkup_spt_state_entry_index_get(unit, ts,
                                        tm_act.new[l].ltid,
                                        tm_act.new[l].lookup_type,
                                        &tm_act.new[l].spt_estate_index,
                                        &tm_act.new[l].sbr_pt_entry_index,
                                        &tm_act.new[l].tltlkup_estate_index);
                SHR_IF_ERR_MSG_EXIT(rv,
                                    (BSL_META_U(unit,
                                                "SPT estate index not found "
                                                "for tile=%u cur_tm=%u "
                                                "new_tm=%u l=%u:ltid=%u "
                                                "lkup_type=%u\n"),
                                     tile, ts->tile_mode, new_tm, l,
                                     tm_act.new[l].ltid,
                                     tm_act.new[l].lookup_type));
                tm_act.new[l].spt_eoper = SBR_PT_ENT_NOOP;
                tm_act.new[l].ltlkup_oper = SBR_TM_LT_LOOKUP_LTS_TCAM_UPDATE;
                tm_act.new[l].pending = TRUE;
            } else {
                /*
                 * No match so this LT.LOOKUP is new and requires an SBR entry.
                 * Check the SPT entry state table and findout if this new
                 * LT.LOOKUP is already pointing to an SBR entry, if it does,
                 * return error.
                 */
                tm_act.new[l].spt_estate_index = SBR_UINT16_INVALID;
                tm_act.new[l].sbr_pt_entry_index = SBR_UINT16_INVALID;
                tm_act.new[l].tltlkup_estate_index = SBR_UINT32_INVALID;
                rv = sbr_tmi_ltlkup_spt_state_entry_index_get(unit, ts,
                                        tm_act.new[l].ltid,
                                        tm_act.new[l].lookup_type,
                                        &tm_act.new[l].spt_estate_index,
                                        &tm_act.new[l].sbr_pt_entry_index,
                                        &tm_act.new[l].tltlkup_estate_index);
                if (rv != SHR_E_NOT_FOUND) {
                    SHR_IF_ERR_MSG_EXIT(SHR_E_EXISTS,
                                        (BSL_META_U(unit,
                                                    "tile_mode=%u LT.LOOKUP "
                                                    "(ltid=%u lkup_type=%u) on "
                                                    "tile=%u already has a "
                                                    "valid SBR entry@index=%u "
                                                    "tltlkup_estate_index=%u "
                                                    "spt_sid=%u - rv=%s.\n"),
                                         new_tm,
                                         tm_act.new[l].ltid,
                                         tm_act.new[l].lookup_type,
                                         tile,
                                         tm_act.new[l].spt_estate_index,
                                         tm_act.new[l].tltlkup_estate_index,
                                         tm_act.new[l].spt_sid,
                                         shr_errmsg(SHR_E_EXISTS)));
                }
                /*
                 * Check whether another tile is sharing the SBR profile table
                 * with this tile and already has an entry allocated for this
                 * LT.LOOKUP type. First get the SBR SID associated with this
                 * LT.LOOKUP from v_tltlkup_info.sbr_profile_data[] array.
                 */
                tm_act.new[l].spt_sid = di->invalid_ptid;
                rv = sbr_tltlkup_match_spt_sid_get(unit, tile, ts,
                                                   tm_act.new[l].ltid,
                                                   tm_act.new[l].lookup_type,
                                                   &tm_act.new[l].spt_sid);
                SHR_IF_ERR_MSG_EXIT(rv,
                                    (BSL_META_U(unit,
                                                "LT.LOOKUP requires SPT entry, "
                                                "but could not find the SBR "
                                                "SID in tile "
                                                "lt_lookup_info_tile_t[] for "
                                                "tile=%u ltid=%u "
                                                "lookup_type=%u - rv=%s.\n"),
                                     tile,
                                     tm_act.new[l].ltid,
                                     tm_act.new[l].lookup_type,
                                     shr_errmsg(rv)));
                rv = sbr_tm_ltlkup_spt_entry_exists_check(unit,
                                          tm_act.new[l].spt_sid,
                                          tm_act.new[l].ltid,
                                          tm_act.new[l].lookup_type,
                                          &tm_act.new[l].spt_estate_index,
                                          &tm_act.new[l].sbr_pt_entry_index);
                SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
                if (rv == SHR_E_NONE) {
                    /*
                     * SBR entry already exists for this LT.LOOKUP, increment
                     * the reference count and update the LTS TCAM rules if
                     * required.
                     */
                    tm_act.new[l].spt_eoper = SBR_PT_ENT_REF_INCR;
                    tm_act.new[l].ltlkup_oper
                                = SBR_TM_LT_LOOKUP_LTS_TCAM_UPDATE;
                } else {
                    /* New SBR entry must be allocated. */
                    tm_act.new[l].spt_eoper = SBR_PT_ENT_ALLOC_HW_INSTALL;
                    tm_act.new[l].ltlkup_oper = SBR_TM_LT_LOOKUP_ADD;
                    tm_act.new[l].spt_estate_index = SBR_UINT16_INVALID;
                    tm_act.new[l].tltlkup_estate_index = SBR_UINT32_INVALID;
                }
                tm_act.new[l].pending = TRUE;
            }
        } else {
            /*
             * No further action required for this LT.LOOKUP as it does not
             * require an SBR entry.
             */
            tm_act.new[l].spt_eoper = SBR_PT_ENT_NOOP;
            tm_act.new[l].ltlkup_oper = SBR_TM_LT_LOOKUP_NOOP;
            tm_act.new[l].spt_estate_index = SBR_UINT16_INVALID;
            tm_act.new[l].tltlkup_estate_index = SBR_UINT32_INVALID;
            tm_act.new[l].pending = FALSE;
        }
    }

    rv = sbr_tm_change_actions_install(unit, only_check_rsrc, lt_id, op_arg,
                                       &tm_act);
    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_MSG_EXIT(rv,
                            (BSL_META_U(unit,
                                        "Tile(=%u) mode(=%u) change error - "
                                        "rv=%s.\n"),
                             tile,
                             new_tm,
                             shr_errmsg(rv)));
    }
    ts->tile_mode = new_tm;
exit:
    SHR_FREE(tm_act.current);
    SHR_FREE(tm_act.new);
    SHR_FUNC_EXIT();
}
/*******************************************************************************
 * Public Functions
 */
sbr_dev_info_t *
bcmptm_sbr_device_info_get(int unit)
{
    return (sbr_dev_info[unit]);
}

sbr_driver_t *
bcmptm_sbr_driver_get(int unit)
{
    return (sbr_driver[unit]);
}

static int
bcmptm_sbr_device_info_init(int unit,
                            const bcmptm_sbr_var_drv_t *var_info,
                            sbr_dev_info_t *dev)
{
    SHR_FUNC_ENTER(unit);
    if (sbr_driver[unit]->device_info_init == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    SHR_IF_ERR_EXIT
        ((sbr_driver[unit]->device_info_init)(unit, var_info, dev));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_sbr_device_init(int unit, bool warm)
{
    const bcmptm_sbr_var_drv_t *var = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_ALLOC(sbr_dev_info[unit], sizeof(sbr_dev_info_t), "bcmptmSbrDevInfo");
    SHR_NULL_CHECK(sbr_dev_info[unit], SHR_E_MEMORY);
    sal_memset(sbr_dev_info[unit], 0, sizeof(sbr_dev_info_t));

    SHR_ALLOC(sbr_driver[unit], sizeof(sbr_driver_t), "bcmptmSbrDrv");
    SHR_NULL_CHECK(sbr_driver[unit], SHR_E_MEMORY);
    sal_memset(sbr_driver[unit], 0, sizeof(sbr_driver_t));

    /* Register the SBR driver functions for this BCM unit. */
    rv = bcmptm_sbr_driver_register(unit, sbr_driver[unit]);
    if (rv == SHR_E_UNAVAIL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    SHR_IF_ERR_EXIT(rv);

    /* Get the unit variant information. */
    rv = bcmptm_sbr_var_info_get(unit, &var);
    if (rv == SHR_E_UNAVAIL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    SHR_IF_ERR_EXIT(rv);

    /*
     * Initialize the device variant specific information, like the number
     * of SBR profile tables and their sizes, number of tiles managed by
     * SBR, their sizes and the LT lookups supported by these tiles.
     */
    SHR_IF_ERR_EXIT
        (bcmptm_sbr_device_info_init(unit, var, sbr_dev_info[unit]));

    if (!warm) {
        SHR_IF_ERR_EXIT
            ((sbr_driver[unit]->default_profiles_init)(unit, var,
                                                       sbr_dev_info[unit]));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_sbr_device_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (sbr_driver[unit]->device_deinit != NULL) {
        SHR_IF_ERR_EXIT
            ((sbr_driver[unit]->device_deinit)(unit));
    }
    if (sbr_dev_info[unit]) {
        SHR_FREE(sbr_dev_info[unit]);
    }
    SHR_FREE(sbr_driver[unit]);
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_sbr_tile_mode_set(int unit,
                         bcmptm_sbr_tile_t tile,
                         bcmptm_sbr_tile_mode_t tile_mode,
                         bool only_check_rsrc,
                         bcmltd_sid_t lt_id,
                         const bcmltd_op_arg_t *op_arg)
{
    const sbr_dev_info_t *di = NULL;         /* Device info. */
    const bcmptm_sbr_var_drv_t *vi = NULL;   /* Device variant info. */
    const bcmptm_sbr_tile_mode_info_t *v_tmi = NULL; /* Tile mode info. */
    const bcmptm_sbr_tile_info_t *v_ti = NULL; /* Tile info. */
    bcmptm_sbr_tile_state_t *ts = NULL; /* Tile state. */
    const char *tile_name = NULL, *cur_tm_name = NULL, *new_tm_name = NULL;

    SHR_FUNC_ENTER(unit);

    /* Validate input parameters. */
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
    /* Get the device info ptr for this unit in the local variable. */
    di = sbr_dev_info[unit];
    SHR_NULL_CHECK(di, SHR_E_INTERNAL);

    /* Get device variant info. */
    vi = di->var;
    SHR_NULL_CHECK(vi, SHR_E_INTERNAL);

    /* The tile number must be within the tiles count managed by SBR-RM. */
    if (tile > vi->tile_count) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid input tile=%u - rv=%s.\n"),
                             tile,
                             shr_errmsg(SHR_E_PARAM)));
    }
    /* Get the tile info pointer for the input tile. */
    v_ti = vi->sbr_tile_info + tile;
    SHR_NULL_CHECK(v_ti, SHR_E_PARAM);

    /*
     * Validate the input tile_mode param value, verify that  it is valid for
     * the given tile.
     */
    if (tile_mode != vi->tile_mode_disable
        && (tile_mode < v_ti->tile_mode[0]
            || tile_mode > v_ti->tile_mode[v_ti->tile_mode_count - 1])) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid input tile_mode=%u for "
                                        "tile=%u - rv=%s.\n"),
                             tile_mode,
                             tile,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* Get the tile mode info for the new tile_mode for the given tile. */
    v_tmi = vi->sbr_tile_info[tile].tile_mode_info
                    + (tile_mode - v_ti->tile_mode[0]);
    SHR_NULL_CHECK(v_tmi, SHR_E_INTERNAL);

    /* Get the current tile state. */
    ts = di->tile_state + tile;
    SHR_NULL_CHECK(ts, SHR_E_INTERNAL);

    /* Get tile name. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_value_to_symbol(unit, DEVICE_EM_TILE_INFOt,
                                      DEVICE_EM_TILE_INFOt_DEVICE_EM_TILE_IDf,
                                      tile, &tile_name));
    /* Get tile_mode name. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_value_to_symbol(unit,
                                      DEVICE_EM_TILE_INFOt,
                                      DEVICE_EM_TILE_INFOt_MODEf,
                                      ts->tile_mode,
                                      &cur_tm_name));
    SHR_IF_ERR_EXIT
        (bcmlrd_field_value_to_symbol(unit,
                                      DEVICE_EM_TILE_INFOt,
                                      DEVICE_EM_TILE_INFOt_MODEf,
                                      tile_mode,
                                      &new_tm_name));
    if (ts->tile_mode != vi->tile_mode_disable
        && (ts->tile_mode < v_ti->tile_mode[0] || ts->tile_mode
            > v_ti->tile_mode[vi->sbr_tile_info[tile].tile_mode_count - 1])) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_INTERNAL,
                            (BSL_META_U(unit,
                                        "Invalid current tile_mode=%s"
                                        " detected for tile=%s - rv=%s\n"),
                             cur_tm_name, tile_name,
                             shr_errmsg(SHR_E_INTERNAL)));
    }

    /* Check if the old and new tile_mode settings are same. */
    if (ts->tile_mode == tile_mode) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "tile=%s: Current tile_modem=%s and new "
                                "tile_mode=%s are the same!.\n"),
                     tile_name, cur_tm_name, new_tm_name));
        /*
         * If they are same, return success (E_NONE) as there is nothing to do,
         * exit this function.
         */
        SHR_EXIT();
    }

    /* New tile_mode=DISABLED and old tile_mode=in-use case. */
    if (ts->tile_mode != vi->tile_mode_disable
        && tile_mode == vi->tile_mode_disable) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "sbr_tile_disable(only_check_rsrc=%u): tile=%s "
                                "new:tile_mode=%s.\n"),
                     only_check_rsrc, tile_name, new_tm_name));
        /*
         * As this active tile is now being disabled, the ref_count of the
         * SBR entries associated with the LT.LOOKUPs of this tile must be
         * decremented. The SPT entry must be freed if the new "ref_count == 0".
         */
        SHR_IF_ERR_EXIT
            (sbr_tile_disable(unit, tile, tile_mode, only_check_rsrc, ts, lt_id,
                              op_arg));
    } else if (ts->tile_mode == vi->tile_mode_disable
               && tile_mode != vi->tile_mode_disable) {
        /* New tile_mode=in-use and old tile_mode=DISABLED case. */
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "sbr_tile_enable(only_check_rsrc=%u): tile=%s "
                                "cur:tile_mode=%s new:tile_mode=%s.\n"),
                     only_check_rsrc, tile_name, cur_tm_name, new_tm_name));
        /* All the LT.LOOKUPs are new for this tile and must be added. */
        SHR_IF_ERR_EXIT
            (sbr_tile_enable(unit, tile, tile_mode, only_check_rsrc, ts, lt_id,
                             op_arg));
    } else {
        /* Active tile mode changed case. */
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "sbr_tile_mode_change(only_check_rsrc=%u): "
                                "tile=%s cur:tile_mode=%s new:tile_mode=%s.\n"),
                     only_check_rsrc, tile_name, cur_tm_name, new_tm_name));
        SHR_IF_ERR_EXIT
            (sbr_tile_mode_change(unit, tile, tile_mode, only_check_rsrc, ts,
                                  lt_id, op_arg));
    }
exit:
    if (only_check_rsrc && !SHR_FUNC_ERR()) {
        SHR_IF_ERR_EXIT
            (bcmptm_sbr_tile_mode_abort(unit));
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_sbr_entry_state_get(int unit,
                           bcmdrd_sid_t spt_sid,
                           uint16_t entry_index,
                           bcmptm_sbr_entry_state_t **entry_state,
                           bcmptm_sbr_pt_state_t **spt_state)
{
    uint16_t pt = SBR_UINT16_INVALID;
    uint32_t spt_estate_index = SBR_UINT32_INVALID;
    sbr_dev_info_t *di = sbr_dev_info[unit];
    const char *lt_name = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(di, SHR_E_INTERNAL);
    SHR_NULL_CHECK(di->spt_state, SHR_E_INTERNAL);
    SHR_NULL_CHECK(di->spt_estate, SHR_E_INTERNAL);
    /* Validate input parameters. */
    SHR_NULL_CHECK(entry_state, SHR_E_PARAM);
    if (bcmdrd_pt_is_valid(unit, spt_sid) == FALSE) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    /*
     * Traverse the device SBR tables and find the table with matching SID
     * value, get the entry state pointer and return it in the function out
     * parameter.
     */
    for (pt = 0; pt < di->spt_count; pt++) {
        if (di->spt_state[pt].drd_sid != spt_sid
            && di->spt_state[pt].ext_drd_sid != spt_sid) {
            continue;
        }
        if (entry_index >= di->spt_state[pt].entries_count) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                (BSL_META_U(unit,
                                            "spt_sid=%u invalid entry_index=%u "
                                            "- rv=%s.\n"),
                                 spt_sid,
                                 entry_index,
                                 shr_errmsg(SHR_E_PARAM)));
        }
        spt_estate_index = (di->spt_state[pt].estate_base_index + entry_index);
        *entry_state = di->spt_estate + spt_estate_index;
        *spt_state = (di->spt_state + pt);

        if ((*entry_state)->ltid != BCMLTD_SID_INVALID
             && ((*entry_state)->ltid != di->invalid_ltid)) {
            SHR_IF_ERR_EXIT
                (bcmlrd_field_value_to_symbol(unit, TABLE_INFOt,
                                              TABLE_INFOt_TABLE_IDf,
                                              (*entry_state)->ltid,
                                              &lt_name));
        }
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "In:spt_sid=%s entry_index=%u Out: SPT=%s/EXT=%s "
                              "entry_state:[usage_mode=%s "
                              "lt_id=%s/%d lookup_type=%s]\n"),
                   bcmdrd_pt_sid_to_name(unit, spt_sid), entry_index,
                   bcmdrd_pt_sid_to_name(unit, (*spt_state)->drd_sid),
                   bcmdrd_pt_sid_to_name(unit, (*spt_state)->ext_drd_sid),
                   bcmptm_sbr_ent_usage_str((*entry_state)->usage_mode),
                   lt_name ? lt_name : "null",
                   (int)(*entry_state)->ltid,
                   bcmptm_sbr_lt_lkup_str((*entry_state)->lookup_type)));
        SHR_EXIT();
    }
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "spt_sid=%s entry_index=%u - rv=%s.\n"),
               bcmdrd_pt_sid_to_name(unit, spt_sid), entry_index,
               shr_errmsg(SHR_E_UNAVAIL)));
    SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_sbr_entry_index_get(int unit,
                           bcmdrd_sid_t spt_sid,
                           bcmltd_sid_t lt_id,
                           bcmptm_sbr_lt_lookup_t lookup_type,
                           uint16_t *entry_index)
{
    uint16_t e, pt = SBR_UINT16_INVALID;
    uint16_t estate_base_index = SBR_UINT16_INVALID;
    sbr_dev_info_t *di = sbr_dev_info[unit];
    bcmptm_sbr_entry_state_t *estate = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(di, SHR_E_INTERNAL);
    SHR_NULL_CHECK(di->spt_state, SHR_E_INTERNAL);
    SHR_NULL_CHECK(di->spt_estate, SHR_E_INTERNAL);
    SHR_NULL_CHECK(entry_index, SHR_E_PARAM);
    /* Validate input parameters. */
    if ((bcmdrd_pt_is_valid(unit, spt_sid) == FALSE)
        || lookup_type > BCMPTM_SBR_LOOKUP_RESERVED) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_NULL_CHECK(bcmlrd_table_get(lt_id), SHR_E_PARAM);

    for (pt = 0; pt < di->spt_count; pt++) {
        if (di->spt_state[pt].drd_sid != spt_sid
            && di->spt_state[pt].ext_drd_sid != spt_sid) {
            continue;
        }
        estate_base_index = di->spt_state[pt].estate_base_index;
        for (e = 0; e < di->spt_state[pt].entries_count; e++) {
            estate = di->spt_estate + (estate_base_index + e);
            if (estate->ltid == lt_id
                && estate->lookup_type == lookup_type) {
                *entry_index = e;
                SHR_EXIT();
            }
        }
    }
    SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_sbr_lookup_en_mask_set(int unit,
                              bcmptm_sbr_tile_t tile,
                              bcmptm_sbr_tile_mode_t tile_mode,
                              bcmltd_sid_t lookup_lt_id,
                              bcmptm_sbr_lt_lookup_t lookup_type,
                              bcmltd_sid_t lt_id,
                              const bcmltd_op_arg_t *op_arg,
                              uint16_t value_lookup_en_valuef,
                              uint16_t value_lookup_en_maskf)
{
    uint16_t r = 0, l = 0, v_tmltlkup_info_count = 0;
    uint16_t minbit = SBR_UINT16_INVALID, maxbit = SBR_UINT16_INVALID;
    uint32_t *km_edata_buf = NULL, *xy_edata_buf = NULL, fld_value = 0;
    size_t entry_wsize = 0, entry_size = 0;
    bool is_tcam = FALSE;
    const sbr_dev_info_t *di = NULL;         /* Device info. */
    const bcmptm_sbr_var_drv_t *vi = NULL;   /* Device variant info. */
    const bcmptm_sbr_tile_mode_info_t *v_tmi = NULL; /* Tile mode info. */
    const bcmptm_sbr_tile_info_t *v_ti = NULL; /* Tile info. */
    const bcmptm_sbr_lt_lookup_info_tile_mode_t *v_tmltlkup_info = NULL;
    const bcmptm_sbr_lookup_en_info_t *v_tmltlkup_en_info = NULL;
    const char *lt_name = NULL;
    bcmptm_sbr_tile_state_t *ts = NULL; /* Tile state. */

    SHR_FUNC_ENTER(unit);

    /* Get the device info ptr for this unit in the local variable. */
    di = sbr_dev_info[unit];
    SHR_NULL_CHECK(di, SHR_E_INTERNAL);

    /* Get device variant info. */
    vi = di->var;
    SHR_NULL_CHECK(vi, SHR_E_UNAVAIL);

    /* The tile number must be within the tiles count managed by SBR-RM. */
    if (tile > vi->tile_count) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid input tile=%u - rv=%s.\n"),
                             tile,
                             shr_errmsg(SHR_E_PARAM)));
    }
    /* Get the tile info pointer for the input tile. */
    v_ti = vi->sbr_tile_info + tile;
    SHR_NULL_CHECK(v_ti, SHR_E_UNAVAIL);

    /* Get the current tile state. */
    ts = di->tile_state + tile;
    SHR_NULL_CHECK(ts, SHR_E_INTERNAL);

    /*
     * Validate the input tile_mode param value, verify that it is valid for
     * the given tile.
     */
    if (tile_mode != vi->tile_mode_disable
        && (tile_mode < v_ti->tile_mode[0]
            || tile_mode > v_ti->tile_mode[v_ti->tile_mode_count - 1])) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                            (BSL_META_U(unit,
                                        "Invalid input tile_mode=%u for "
                                        "tile=%u - rv=%s.\n"),
                             tile_mode,
                             tile,
                             shr_errmsg(SHR_E_PARAM)));
    }

    /* Get the variant tile mode info for the input tile_mode. */
    v_tmi = vi->sbr_tile_info[tile].tile_mode_info + (tile_mode
                                                       - v_ti->tile_mode[0]);
    SHR_NULL_CHECK(v_tmi, SHR_E_UNAVAIL);

    switch (lookup_type) {
        case BCMPTM_SBR_LOOKUP0:
            if (v_tmi->lt_lookup0_info_count == 0) {
                SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
            }
            v_tmltlkup_info = v_tmi->lt_lookup0_info;
            v_tmltlkup_info_count = v_tmi->lt_lookup0_info_count;
            break;
        case BCMPTM_SBR_LOOKUP1:
            if (v_tmi->lt_lookup1_info_count == 0) {
                SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
            }
            v_tmltlkup_info = v_tmi->lt_lookup1_info;
            v_tmltlkup_info_count = v_tmi->lt_lookup1_info_count;
            break;
        default:
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    for (l = 0; l < v_tmltlkup_info_count; l++) {
        if (v_tmltlkup_info[l].lookup_type == lookup_type
            && v_tmltlkup_info[l].ltid == lookup_lt_id
            && v_tmltlkup_info[l].tile == tile) {
            break;
        }
    }

    if (l == v_tmltlkup_info_count) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_NOT_FOUND,
                            (BSL_META_U(unit,
                                        "LTS TCAM info not found for tile=%u "
                                        "tile_mode=%u - rv=%s.\n"),
                             tile,
                             tile_mode,
                             shr_errmsg(SHR_E_NOT_FOUND)));
    }

    /*
     * Get Tile LT.LOOKUP enable info ptr that contains the field bit
     * positions.
     */
    v_tmltlkup_en_info = v_tmltlkup_info[l].lookup_en_info;
    SHR_NULL_CHECK(v_tmltlkup_en_info, SHR_E_UNAVAIL);

    entry_wsize = bcmdrd_pt_entry_wsize
                    (unit, v_tmltlkup_info[l].sid_lts_tcam_data_only);
    entry_size = sizeof(uint32_t) * entry_wsize;
    SHR_ALLOC(km_edata_buf, entry_size, "bcmptmSbrPtKmEntBuff");
    SHR_NULL_CHECK(km_edata_buf, SHR_E_MEMORY);
    sal_memset(km_edata_buf, 0, entry_size);

    for (r = 0; r < v_tmltlkup_info[l].rule_index_count; r++) {
        /* Clear the buffer prior to reading the HW entry. */
        sal_memset(km_edata_buf, 0, entry_size);
        /* Read the entry from HW. */
        SHR_IF_ERR_EXIT
            (bcmptm_sbr_lt_hw_read(unit, 0, lt_id, op_arg,
                                   v_tmltlkup_info[l].sid_lts_tcam_data_only,
                                   v_tmltlkup_info[l].rule_index[r],
                                   entry_size,
                                   km_edata_buf));

        /* Get LOOKUP_EN_MASKf bits position in the entry. */
        minbit = v_tmltlkup_en_info->bitp_f_lookup_en_mask->minbit;
        maxbit = v_tmltlkup_en_info->bitp_f_lookup_en_mask->maxbit;
        /* Set LOOKUP_EN_MASKf value in the entry. */
        fld_value = value_lookup_en_maskf;
        bcmdrd_field_set(km_edata_buf, minbit, maxbit, &fld_value);

        /* Get LOOKUP_EN_VALUEf bits position in the entry. */
        minbit = v_tmltlkup_en_info->bitp_f_lookup_en_value->minbit;
        maxbit = v_tmltlkup_en_info->bitp_f_lookup_en_value->maxbit;
        /* Set LOOKUP_EN_VALUEf value in the entry. */
        fld_value = value_lookup_en_valuef;
        bcmdrd_field_set(km_edata_buf, minbit, maxbit, &fld_value);

        if (bcmdrd_pt_attr_is_cam(unit,
                                  v_tmltlkup_info[l].sid_lts_tcam_data_only)) {
            is_tcam = TRUE;
            SHR_ALLOC(xy_edata_buf, entry_size, "bcmptmSbrPtXyEntBuff");
            SHR_NULL_CHECK(xy_edata_buf, SHR_E_MEMORY);
            sal_memset(xy_edata_buf, 0, entry_size);
            /* KM -> TCAM format conversion */
            SHR_IF_ERR_EXIT
                (bcmptm_pt_tcam_km_to_xy(unit,
                                    v_tmltlkup_info[l].sid_lts_tcam_data_only,
                                    1,   /* 1 for !SLAM */
                                    km_edata_buf,   /* km_entry */
                                    xy_edata_buf)); /* xy_entry */
            /* Write entry to LTS_TCAM table. */
            SHR_IF_ERR_EXIT
                (bcmptm_sbr_lt_hw_write(unit, 0, lt_id, op_arg,
                                    v_tmltlkup_info[l].sid_lts_tcam_data_only,
                                    v_tmltlkup_info[l].rule_index[r],
                                    xy_edata_buf));
        } else {
            /* Write entry to LTS_TCAM table. */
            SHR_IF_ERR_EXIT
                (bcmptm_sbr_lt_hw_write(unit, 0, lt_id, op_arg,
                                    v_tmltlkup_info[l].sid_lts_tcam_data_only,
                                    v_tmltlkup_info[l].rule_index[r],
                                    km_edata_buf));
        }
    }
    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        SHR_IF_ERR_EXIT
            (bcmlrd_field_value_to_symbol(unit,
                                          TABLE_INFOt,
                                          TABLE_INFOt_TABLE_IDf,
                                          lookup_lt_id,
                                          &lt_name));
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Done(is_tcam=%u): LTS_TCAM rule/s (cnt=%u) "
                              "updated for t=%u ltid=%s lkup_t=%s with "
                              "en:value=0x%x mask=0x%x.\n"),
                   is_tcam, v_tmltlkup_info[l].rule_index_count, tile, lt_name,
                   bcmptm_sbr_lt_lkup_str(lookup_type),
                   value_lookup_en_valuef, value_lookup_en_maskf));
    }

exit:
    SHR_FREE(km_edata_buf);
    SHR_FREE(xy_edata_buf);
    SHR_FUNC_EXIT();
}

int
bcmptm_sbr_entry_alloc(int unit,
                       bcmdrd_sid_t spt_sid,
                       bcmltd_sid_t lt_id,
                       uint16_t lookup_type,
                       bool paired_lookups,
                       uint16_t entry_count,
                       uint16_t *entry_index)
{
    const char *lt_name = NULL;
    sbr_dev_info_t *di = sbr_dev_info[unit];
    uint16_t spt = SBR_UINT16_INVALID;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(di, SHR_E_INTERNAL);

    /* Validate input parameters. */
    SHR_NULL_CHECK(entry_index, SHR_E_PARAM);
    if ((bcmdrd_pt_is_valid(unit, spt_sid) == FALSE)
        || lookup_type > BCMPTM_SBR_LOOKUP_RESERVED) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_NULL_CHECK(bcmlrd_table_get(lt_id), SHR_E_PARAM);

    for (spt = 0; spt < di->spt_count; spt++) {
        if (di->spt_state[spt].drd_sid == spt_sid
            || di->spt_state[spt].ext_drd_sid == spt_sid) {
            SHR_IF_ERR_EXIT
                (sbr_pt_entry_alloc(unit, spt, lt_id, lookup_type,
                                    paired_lookups, entry_count,
                                    entry_index));
            SHR_EXIT();
        }
    }
    SHR_IF_ERR_EXIT
        (bcmlrd_field_value_to_symbol(unit, TABLE_INFOt, TABLE_INFOt_TABLE_IDf,
                                      lt_id, &lt_name));
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "spt_sid=%s lt_id=%s lookup_type=%s paired=%u - "
                          "rv=%s.\n"),
               bcmdrd_pt_sid_to_name(unit, spt_sid), lt_name,
               bcmptm_sbr_lt_lkup_str(lookup_type), paired_lookups,
               shr_errmsg(SHR_E_UNAVAIL)));
    SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_sbr_entry_free(int unit,
                      bcmdrd_sid_t spt_sid,
                      uint16_t entry_count,
                      uint16_t *entry_index)
{
    sbr_dev_info_t *di = sbr_dev_info[unit];
    uint16_t spt = SBR_UINT16_INVALID;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(di, SHR_E_INTERNAL);

    /* Validate input parameters. */
    SHR_NULL_CHECK(entry_index, SHR_E_PARAM);
    if (bcmdrd_pt_is_valid(unit, spt_sid) == FALSE) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    for (spt = 0; spt < di->spt_count; spt++) {
        if (di->spt_state[spt].drd_sid == spt_sid
            || di->spt_state[spt].ext_drd_sid == spt_sid) {
            SHR_IF_ERR_EXIT
                (sbr_pt_entry_free(unit, spt, entry_count, entry_index));
            SHR_EXIT();
        }
    }
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "spt_sid=%s - rv=%s.\n"),
               bcmdrd_pt_sid_to_name(unit, spt_sid),
               shr_errmsg(SHR_E_UNAVAIL)));
    SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_sbr_tile_lt_lookup_find(int unit,
                               bcmptm_sbr_tile_t tile,
                               bcmltd_sid_t lookup_lt_id,
                               bcmptm_sbr_lt_lookup_t lookup_type)
{
    const sbr_dev_info_t *di = NULL;         /* Device info. */
    const bcmptm_sbr_var_drv_t *vi = NULL;   /* Device variant info. */
    const bcmptm_sbr_tile_info_t *v_ti = NULL; /* Tile info. */
    const bcmptm_sbr_lt_lookup_info_tile_t *v_tltlkup_i = NULL;
    const char *tile_name = NULL, *lt_name = NULL;
    uint32_t v_tltlkup_cnt = 0, i = 0; /* LT.LOOKUP array elements count. */
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /* Get the device info ptr for this unit in the local variable. */
    di = sbr_dev_info[unit];
    SHR_NULL_CHECK(di, SHR_E_INTERNAL);

    /* Get device variant info. */
    vi = di->var;
    SHR_NULL_CHECK(vi, SHR_E_INTERNAL);

    /* The tile number must be within the tiles count managed by SBR-RM. */
    if (tile > vi->tile_count) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Invalid tile=%u - rv=%s.\n"),
                     tile, shr_errmsg(SHR_E_PARAM)));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    /* Get the tile info pointer for the input tile. */
    v_ti = vi->sbr_tile_info + tile;
    SHR_NULL_CHECK(v_ti, SHR_E_INTERNAL);

    switch (lookup_type) {
        case BCMPTM_SBR_LOOKUP0:
            v_tltlkup_i = v_ti->sbr_lt_lookup0_info;
            v_tltlkup_cnt = v_ti->sbr_lt_lookup0_info_count;
            break;
        case BCMPTM_SBR_LOOKUP1:
            v_tltlkup_i = v_ti->sbr_lt_lookup1_info;
            v_tltlkup_cnt = v_ti->sbr_lt_lookup1_info_count;
            break;
        default:
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Invalid lookup_type=%u - rv=%s.\n"),
                         lookup_type, shr_errmsg(SHR_E_PARAM)));
            SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if ((v_tltlkup_cnt > 0 && v_tltlkup_i == NULL)
        || (v_tltlkup_i != NULL && v_tltlkup_cnt == 0)) {
        /* Possible only if there is an error in SBR tooling output data. */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    /* Get tile name. */
    rv = bcmlrd_field_value_to_symbol(unit, DEVICE_EM_TILE_INFOt,
                                      DEVICE_EM_TILE_INFOt_DEVICE_EM_TILE_IDf,
                                      tile, &tile_name);
    if (SHR_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Invalid tile=%u - rv=%s.\n"),
                         tile, shr_errmsg(SHR_E_PARAM)));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    /* Get LT name. */
    rv = bcmlrd_field_value_to_symbol(unit, TABLE_INFOt, TABLE_INFOt_TABLE_IDf,                                       lookup_lt_id, &lt_name);
    if (SHR_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Invalid lookup_lt_id=%u - rv=%s.\n"),
                         lookup_lt_id, shr_errmsg(SHR_E_PARAM)));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Search for input LT_ID in device variant tile LT.LOOKUP info array. */
    for (i = 0; i < v_tltlkup_cnt; i++) {
        if (lookup_lt_id == v_tltlkup_i[i].ltid) {
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Found: tile=%s lt_id=%s lookup_type=%s.\n"),
                       tile_name, lt_name, bcmptm_sbr_lt_lkup_str(lookup_type))
                      );
            /* Match found, tile can host the input lt_id + lookup_type. */
            SHR_EXIT();
        }
    }
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "tile=%s lt_id=%s lookup_type=%s - rv=%s.\n"),
               tile_name, lt_name, bcmptm_sbr_lt_lkup_str(lookup_type),
               shr_errmsg(SHR_E_NOT_FOUND)));
    SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT();
}
