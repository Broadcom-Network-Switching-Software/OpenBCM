/*! \file bcm56880_a0_ptm_sbr.c
 *
 * Chip specific functions for PTM UFT
 *
 * This file contains the chip specific functions for PTM UFT
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
#include <shr/shr_pb_local.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmdrd/chip/bcm56880_a0_enum.h>
#include <bcmdrd/chip/bcm56880_a0_defs.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_device_constants.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_sbr_internal.h>
#include "./bcmptm_common.h"
#include "bcm56880_a0_ptm_sbr.h"
/*******************************************************************************
 * Defines
 */
/*! Combined layer/source for BSL logging. */
#define BSL_LOG_MODULE  BSL_LS_BCMPTM_UFT
#define MAX_TMP_SPT_COUNT 1024

/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */
static
bcmptm_sbr_pt_state_t *spt_state[BCMDRD_CONFIG_MAX_UNITS];
static
bcmptm_sbr_entry_state_t *spt_estate[BCMDRD_CONFIG_MAX_UNITS];
static
bcmptm_sbr_lt_lookup_state_tile_t *tile_lkup_state[BCMDRD_CONFIG_MAX_UNITS];
static
bcmptm_sbr_tile_state_t *tile_state[BCMDRD_CONFIG_MAX_UNITS];
static
bcmdrd_sid_t *tmp_spt_list[BCMDRD_CONFIG_MAX_UNITS];
static
uint16_t tmp_spt_count[BCMDRD_CONFIG_MAX_UNITS];
static
bcmdrd_sid_t *final_spt_list[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Private Functions
 */
static bool
sbr_ext_to_main_list_sid_match(const bcmptm_sbr_ext_to_main_t *spt_ext,
                               bcmdrd_sid_t sid,
                               bool match_main,
                               uint16_t *mindex)
{
    uint16_t pt = 0;

    if (spt_ext && spt_ext->mem_ext && spt_ext->mem && mindex) {
        *mindex = SBR_UINT16_INVALID;
        for (pt = 0; pt < spt_ext->count; pt++) {
            if (match_main == FALSE && spt_ext->mem_ext[pt] == sid) {
                *mindex = pt;
                break;
            }
            if (match_main == TRUE && spt_ext->mem[pt] == sid) {
                *mindex = pt;
                break;
            }
        }
        return (*mindex != SBR_UINT16_INVALID ? TRUE : FALSE);
    }
    return (FALSE);
}

static int
sbr_tiles_init(int unit,
               const bcmptm_sbr_var_drv_t *vi,
               sbr_dev_info_t *dev)
{
    uint16_t t; /* Tile index. */
    const bcmptm_sbr_tile_info_t *v_ti = NULL;
    bcmptm_sbr_tile_state_t *ts = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(vi, SHR_E_INIT);
    SHR_NULL_CHECK(dev, SHR_E_INIT);

    ts = dev->tile_state;
    SHR_NULL_CHECK(ts, SHR_E_INIT);

    for (t = 0; t < vi->tile_count; t++) {
        v_ti = vi->sbr_tile_info + t;
        if (v_ti->initial_tile_mode == vi->tile_mode_disable) {
            ts[t].tile_mode = vi->tile_mode_disable;
        } else {
            ts[t].tile_mode = v_ti->tile_mode[0];
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
sbr_tile_lt_lookup_search(int unit,
                          sbr_dev_info_t *dev,
                          const bcmptm_sbr_lt_lookup_info_tile_t *v_lkup_info,
                          uint32_t v_lkup_info_count,
                          const bcmptm_sbr_entry_state_t *estate,
                          bool *found,
                          uint32_t *lt_lkup_index,
                          uint32_t *spt_drd_sid)
{
    uint32_t l;    /* LT.LOOKUP index. */
    uint16_t pdata_idx; /* Profile table entry data index. */
    const bcmptm_sbr_lt_lookup_info_tile_t *v_lt_lkup = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(v_lkup_info, SHR_E_PARAM);
    SHR_NULL_CHECK(estate, SHR_E_PARAM);
    SHR_NULL_CHECK(found, SHR_E_PARAM);

    for (l = 0; l < v_lkup_info_count; l++) {
        v_lt_lkup = v_lkup_info + l;
        if (v_lt_lkup->ltid != estate->ltid
            || v_lt_lkup->lookup_type != estate->lookup_type) {
            continue;
        }
        if (v_lt_lkup->lt_lookup_is_default == FALSE) {
            continue;
        }
        for (pdata_idx = 0;
             pdata_idx < v_lt_lkup->sbr_profile_data_count;
             pdata_idx++) {
            if (dev->spt_state[estate->pt_state_index].drd_sid
                == v_lt_lkup->sbr_profile_data[pdata_idx].sid) {
                LOG_DEBUG(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Match: v_lkup_idx(l)=%u SID=%u "
                                      "spt_pdata_index=%u "
                                      "spt_estate@spt_idx=%u var:lkup_type=%u "
                                      "ltid=%u is_default=%u\n"),
                           l,
                           v_lt_lkup->sbr_profile_data[pdata_idx].sid,
                           pdata_idx,
                           estate->pt_state_index,
                           v_lt_lkup->lookup_type,
                           v_lt_lkup->ltid,
                           v_lt_lkup->lt_lookup_is_default));
                *found = TRUE;
                *lt_lkup_index = l;
                *spt_drd_sid = v_lt_lkup->sbr_profile_data[pdata_idx].sid;
                break;
            }
        }
        if (*found) {
            break;
        }
    }
    if (l == v_lkup_info_count) {
        *found = FALSE;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
sbr_tiles_lt_lookup_match_check(int unit,
                                const bcmptm_sbr_var_drv_t *vi,
                                sbr_dev_info_t *dev,
                                const bcmptm_sbr_entry_state_t *estate,
                                uint16_t *tile,
                                uint32_t *lt_lkup_index,
                                uint32_t *spt_drd_sid)
{
    uint16_t t; /* Tile index. */
    bool match_found = FALSE;
    const bcmptm_sbr_tile_info_t *v_ti = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(estate, SHR_E_INIT);
    for (t = 0; t < vi->sbr_tile_info_count; t++) {
        v_ti = vi->sbr_tile_info + t;
        SHR_NULL_CHECK(v_ti, SHR_E_INIT);
        if (estate->lookup_type == BCMPTM_SBR_LOOKUP0
            && v_ti->sbr_lt_lookup0_info_count > 0) {
            /* Search in tile LT.LOOKUP0 list. */
            SHR_IF_ERR_EXIT
                (sbr_tile_lt_lookup_search(unit,
                                           dev,
                                           v_ti->sbr_lt_lookup0_info,
                                           v_ti->sbr_lt_lookup0_info_count,
                                           estate,
                                           &match_found,
                                           lt_lkup_index,
                                           spt_drd_sid));
        }
        if (estate->lookup_type == BCMPTM_SBR_LOOKUP1
            && v_ti->sbr_lt_lookup1_info_count > 0) {
            /* Search in tile LT.LOOKUP1 list. */
            SHR_IF_ERR_EXIT
                (sbr_tile_lt_lookup_search(unit,
                                           dev,
                                           v_ti->sbr_lt_lookup1_info,
                                           v_ti->sbr_lt_lookup1_info_count,
                                           estate,
                                           &match_found,
                                           lt_lkup_index,
                                           spt_drd_sid));
        }
        if (match_found) {
            *tile = t;
            SHR_EXIT();
        }
        match_found = FALSE;
    }
    SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT();
}

static int
sbr_pt_estate_init(int unit,
                   const bcmptm_sbr_var_drv_t *vi,
                   sbr_dev_info_t *dev)
{
    int rv = SHR_E_INTERNAL; /* Return value. */
    uint16_t pt, map, e, tile; /* Index variables. */
    uint32_t spt_drd_sid; /* SBR profile table DRD SID. */
    uint16_t spt_map_count = vi->sbr_entry_profile_map_count;
    uint32_t lt_lookup_index; /* LOOKUP0/1 match index. */
    bcmptm_sbr_entry_state_t *estate = NULL; /* SPT entry state pointer. */
    bcmptm_sbr_tile_state_t *ts = NULL; /* Tile state. */
    bcmptm_sbr_lt_lookup_state_tile_t *tlkup_state = NULL; /* Lkup state. */
    const bcmptm_sbr_entry_profile_map_t *spt_map = NULL;

    SHR_FUNC_ENTER(unit);

    spt_map = vi->sbr_entry_profile_map;
    for (map = 0; map < spt_map_count; map++) {
        for (pt = 0; pt < dev->spt_count; pt++) {
            if (spt_map[map].sid != spt_state[unit][pt].drd_sid
                && spt_map[map].sid != spt_state[unit][pt].ext_drd_sid) {
                continue;
            }
            e = (spt_state[unit][pt].estate_base_index
                    + spt_map[map].entry_index);
            estate = (spt_estate[unit] + e);
            if (estate->usage_mode != BCMPTM_SBR_ENTRY_FREE) {
                SHR_IF_ERR_MSG_EXIT(SHR_E_INIT,
                                    (BSL_META_U(unit,
                                                "Entry@idx=%u in SBRm(SID)=%u "
                                                "already saved as in-use.\n"),
                                     spt_map[map].entry_index,
                                     spt_map[map].sid));
            }
            estate->usage_mode = spt_map[map].usage;
            estate->ltid = spt_map[map].ltid;
            estate->lookup_type = spt_map[map].lookup_type;
            if (estate->usage_mode == BCMPTM_SBR_ENTRY_IN_USE) {
                estate->ref_count = 0;
            } else {
                /*
                 * Entry reserved by NPL, so increment the ref count and
                 * decrement the free count.
                 */
                estate->ref_count++;
                spt_state[unit][pt].free_entries_count--;
            }
            estate->pt_state_index = pt;
            break;
        }
    }
    for (e = 0; e < dev->spt_entries_count; e++) {
        if (spt_estate[unit][e].usage_mode == BCMPTM_SBR_ENTRY_IN_USE) {
            tile = SBR_UINT16_INVALID;
            spt_drd_sid = dev->invalid_ptid;
            lt_lookup_index = SBR_UINT32_INVALID;
            rv = sbr_tiles_lt_lookup_match_check(unit,
                                                 vi,
                                                 dev,
                                                 (spt_estate[unit] + e),
                                                 &tile,
                                                 &lt_lookup_index,
                                                 &spt_drd_sid);
            if (rv == SHR_E_NOT_FOUND) {
                pt = spt_estate[unit][e].pt_state_index;
                SHR_IF_ERR_MSG_EXIT(SHR_E_INIT,
                                    (BSL_META_U(unit,
                                                "SPT=%-3u: In-use "
                                                "entry@idx=%-4u with ltid=%u "
                                                "+ lkup=%u has no_tile "
                                                "lt.lookup_type match.\n"),
                                     pt,
                                     (e
                                      - spt_state[unit][pt].estate_base_index),
                                     spt_estate[unit][e].ltid,
                                     spt_estate[unit][e].lookup_type));

            }
            if (tile == SBR_UINT16_INVALID
                || lt_lookup_index == SBR_UINT32_INVALID
                || spt_drd_sid == dev->invalid_ptid) {
                SHR_IF_ERR_MSG_EXIT(SHR_E_INIT,
                                    (BSL_META_U(unit,
                                                "Req. tile state param not "
                                                "updated: tile=%u "
                                                "lt_lookup_index=%u "
                                                "spt_drd_sid=%u.\n"),
                                     e,
                                     lt_lookup_index,
                                     spt_drd_sid));

            }
            spt_estate[unit][e].ref_count++;
            /* Decrement free entries count for this SPT. */
            pt = spt_estate[unit][e].pt_state_index;
            spt_state[unit][pt].free_entries_count--;

            ts = dev->tile_state + tile;
            SHR_NULL_CHECK(ts, SHR_E_INTERNAL);
            switch (spt_estate[unit][e].lookup_type) {
                case BCMPTM_SBR_LOOKUP0:
                    if (ts->lt_lookup0_base_index != SBR_UINT32_INVALID) {
                        tlkup_state = dev->tile_lkup_state
                                        + (ts->lt_lookup0_base_index
                                        + lt_lookup_index);
                    }
                    break;
                case BCMPTM_SBR_LOOKUP1:
                    if (ts->lt_lookup1_base_index != SBR_UINT32_INVALID) {
                        tlkup_state = dev->tile_lkup_state
                                        + (ts->lt_lookup1_base_index
                                        + lt_lookup_index);
                    }
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_NULL_CHECK(tlkup_state, SHR_E_INTERNAL);

            /* Mark this LT.LOOKUP state on this tile as in-use. */
            tlkup_state->in_use = TRUE;
            /* Store the SBR profile table entry state index. */
            tlkup_state->entry_index = e;
            tlkup_state->spt_sid = spt_drd_sid;
            ts = NULL;
            tlkup_state = NULL;
        } else if (spt_estate[unit][e].usage_mode == BCMPTM_SBR_ENTRY_FREE) {
            /* Initialize ltid as invalid for free entries in profile table. */
            spt_estate[unit][e].ltid = dev->invalid_ltid;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_sbr_default_profiles_init(int unit,
                                      const bcmptm_sbr_var_drv_t *vi,
                                      sbr_dev_info_t *dev)
{
    SHR_FUNC_ENTER(unit);

    /* Validate input parameters. */
    SHR_NULL_CHECK(dev, SHR_E_INIT);
    SHR_NULL_CHECK(dev->spt_state, SHR_E_INIT);
    SHR_NULL_CHECK(dev->spt_estate, SHR_E_INIT);
    SHR_NULL_CHECK(dev->tile_state, SHR_E_INIT);
    SHR_NULL_CHECK(dev->tile_lkup_state, SHR_E_INIT);
    SHR_NULL_CHECK(vi, SHR_E_INIT);
    SHR_NULL_CHECK(vi->sbr_tile_info, SHR_E_INIT);
    SHR_NULL_CHECK(vi->sbr_entry_profile_map, SHR_E_INIT);
    SHR_NULL_CHECK(vi->sbr_profile_table, SHR_E_INIT);

    SHR_IF_ERR_EXIT
        (sbr_tiles_init(unit, vi, dev));

    SHR_IF_ERR_EXIT
        (sbr_pt_estate_init(unit, vi, dev));
exit:
    SHR_FUNC_EXIT();
}

/* Add sid for phy sbr_profile table in tmp_spt_list[] array */
static int
new_spt_sid_add(int unit,
                const bcmptm_sbr_var_drv_t *vi,
                bcmdrd_sid_t new_spt_sid)
{
    uint16_t pt = SBR_UINT16_INVALID;
    bool found_in_list;
    SHR_FUNC_ENTER(unit);
    if (new_spt_sid != INVALIDm) {
        /* search in vi list */
        
        found_in_list = FALSE;
        for (pt = 0; pt < vi->sbr_profile_table_count; pt++) {
            if (new_spt_sid == vi->sbr_profile_table[pt]) {
                /* sid_in_new_ds is already in variant_info sbr_pt list */
                found_in_list = TRUE;
                break;
            }
        }
        if (!found_in_list) {
            /* search in tmp_spt list */
            
            for (pt = 0; pt < tmp_spt_count[unit]; pt++) {
                if (new_spt_sid == *(tmp_spt_list[unit] + pt)) {
                    /* sid_in_new_ds is already in tmp_spt list */
                    found_in_list = TRUE;
                    break;
                }
            }
        }
        if (!found_in_list) {
            uint16_t index = tmp_spt_count[unit];
            if (index >= MAX_TMP_SPT_COUNT) {
                SHR_IF_ERR_MSG_EXIT(SHR_E_INIT,
                                    (BSL_META_U(unit,
                                                "Cannot manage more than %0u "
                                                "new SBR_PROFILE_TABLEm\n"),
                                     MAX_TMP_SPT_COUNT));
            }
            *(tmp_spt_list[unit] + index) = new_spt_sid;
            tmp_spt_count[unit]++;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int basic_comp(const void *x, const void *y)
{
    return (*(int *)x - *(int *)y);
}

static int
final_spt_list_create(int unit, const bcmptm_sbr_var_drv_t *vi)
{
    size_t alloc_sz; /* Malloc size. */
    uint16_t i, spm_lt_info_count = 0, tc = 0;
    const bcmptm_sbr_lt_str_pt_info_t *spm_lt_info_base = NULL;
    const bcmptm_sbr_lt_str_pt_info_t *spm_lt_info = NULL;
    bcmdrd_sid_t new_spt_sid = INVALIDm;
    const bcmlrd_hmf_t *hmf_info;
    SHR_FUNC_ENTER(unit);

    /* Alloc space in Heap (non_ha_mem) to store new sid list */
    alloc_sz = sizeof(bcmdrd_sid_t) * MAX_TMP_SPT_COUNT;
    SHR_ALLOC(tmp_spt_list[unit], alloc_sz, "bcmptmSbrSptListTmp");
    SHR_NULL_CHECK(tmp_spt_list[unit], SHR_E_MEMORY);
    sal_memset(tmp_spt_list[unit], 0, alloc_sz);

    /* Create temporary sid list of sbr_pt that are not already present in
     * vi->sbr_profile_table list */
    if (vi->sbr_list) {
        spm_lt_info_base = vi->sbr_list->lt_str_pt_info;
        spm_lt_info_count = vi->sbr_list->lt_str_pt_info_count;
        if (spm_lt_info_count != 0) {
            SHR_NULL_CHECK(spm_lt_info_base, SHR_E_INTERNAL);
        }

    }

    for (i = 0; i < spm_lt_info_count; i++) {
        uint32_t h;
        spm_lt_info = &(spm_lt_info_base[i]);
        for (h = 0; h < spm_lt_info->hw_entry_info_count; h++) {
            const bcmptm_sbr_hw_entry_info_t *hwe_info;
            uint32_t v;

            hwe_info = spm_lt_info->hw_entry_info + h;
            hmf_info = hwe_info->l2p_map_info.map;
            if (hmf_info == NULL) {
                continue;
            }
            for (v = 0; v < hmf_info->view_count; v++) {
                const bcmlrd_hmf_view_t *view = hmf_info->view + v;
                uint32_t ins;
                for (ins = 0; ins < view->instance_count; ins++) {
                    const bcmlrd_hmf_instance_t *instance;
                    uint32_t mc;

                    instance = view->instance[ins];
                    if (instance == NULL) {
                        continue;
                    }
                    for (mc = 0; mc < instance->memory_count; mc++) {
                        const bcmlrd_hmf_memory_t *memory =
                            instance->memory[mc];
                        if (memory == NULL) {
                            continue;
                        }
                        new_spt_sid = memory->ptsid;
                        SHR_IF_ERR_EXIT
                            (new_spt_sid_add(unit, vi, new_spt_sid));
                        tc++;
                    } /* mc */
                } /* ins */
            } /* v */
        } /* h */
    } /* i */

    /* Alloc space in Heap (non_ha_mem) to store final_sid list */
    alloc_sz = sizeof(bcmdrd_sid_t) *
               (vi->sbr_profile_table_count + tmp_spt_count[unit]);
    SHR_ALLOC(final_spt_list[unit], alloc_sz, "bcmptmSbrSptList");
    SHR_NULL_CHECK(final_spt_list[unit], SHR_E_MEMORY);
    sal_memset(final_spt_list[unit], 0, alloc_sz);

    /* Copy all spt from vi list to final_spt list. */
    sal_memcpy(final_spt_list[unit],
               vi->sbr_profile_table,
               vi->sbr_profile_table_count * sizeof(bcmdrd_sid_t));

    /* Append tmp_spt list to final_spt list. */
    sal_memcpy(final_spt_list[unit] + vi->sbr_profile_table_count,
               tmp_spt_list[unit],
               tmp_spt_count[unit] * sizeof(bcmdrd_sid_t));

    /* Sort final_spt list.
     * Total number of entries in final_spt list =
     *      (vi->sbr_profile_table_count + tmp_spt_count[unit])
     */
    sal_qsort(final_spt_list[unit],
              (vi->sbr_profile_table_count + tmp_spt_count[unit]),
              sizeof(bcmdrd_sid_t),
              basic_comp);

exit:
    SHR_FREE(tmp_spt_list[unit]);
    tmp_spt_list[unit] = NULL;
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_sbr_device_info_init(int unit,
                                 const bcmptm_sbr_var_drv_t *vi,
                                 sbr_dev_info_t *dev)
{
    uint16_t pt, t, l;  /* Profile table and tile index. */
    uint16_t spt_idx = SBR_UINT16_INVALID;
    uint16_t mindex = SBR_UINT16_INVALID; /* Match index. */
    uint32_t wsize = 0;  /* Entry size in words. */
    size_t alloc_sz; /* Malloc size. */
    const bcmptm_sbr_tile_info_t *v_ti = NULL; /* Device tile info. */
    shr_pb_t *pb = NULL; /* Print buffer. */

    SHR_FUNC_ENTER(unit);
    /* Validate input parameters. */
    SHR_NULL_CHECK(dev, SHR_E_INIT);
    SHR_NULL_CHECK(vi, SHR_E_INIT);
    SHR_NULL_CHECK(vi->sbr_tile_info, SHR_E_INIT);
    SHR_NULL_CHECK(vi->sbr_entry_profile_map, SHR_E_INIT);
    SHR_NULL_CHECK(vi->sbr_profile_table, SHR_E_INIT);

    if (vi->sbr_tile_info_count == 0
        || vi->sbr_entry_profile_map_count == 0
        || vi->sbr_profile_table_count == 0
        || vi->tile_count == 0
        || (vi->tile_count != vi->sbr_tile_info_count)) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_INIT,
                            (BSL_META_U(unit,
                                        "Invalid var info: tile_info_count=%u "
                                        "|| spt_map_count=%u || spt_count=%u "
                                        "|| tile_count=%u\n"),
                             vi->sbr_tile_info_count,
                             vi->sbr_entry_profile_map_count,
                             vi->sbr_profile_table_count,
                             vi->tile_count));
    }

    dev->invalid_ltid = BCMLTD_INVALID_LT;
    dev->invalid_ptid = BCM56880_A0_ENUM_COUNT;

    /* Create final sorted list of sbr_profile_tables */
    SHR_IF_ERR_EXIT
        (final_spt_list_create(unit, vi));

    /* Store the values in unit device structure. */
    dev->var = vi;
    dev->tile_count = vi->tile_count;
    dev->spt_count = 0;
    /*
     * Determine the total no. of SBR profile tables for which SBR-RM has to
     * maintain the SW state.
     */
    for (pt = 0; pt < (vi->sbr_profile_table_count + tmp_spt_count[unit]);
         pt++) {
        /*
         * Check if current drd_sid element in final_spt_list[] matches
         * one of the elements in the mem_ext[] array.
         */
        if (sbr_ext_to_main_list_sid_match(vi->sbr_ext_to_main,
                                           final_spt_list[unit][pt], FALSE,
                                           &mindex)) {
            /*
             * This SBR profile table is an extension table, so skip it as
             * its entries state will be managed via the main SBR profile
             * table spt_state.
             */
            continue;
        }
        /*
         * Not a match in sbr_ext_to_main list, so this profile table must be
         * managed separately, increment the device profile table count.
         */
        dev->spt_count++;
    }
    if (dev->spt_count == 0) {
        SHR_ERR_MSG_EXIT(SHR_E_INIT,
                         (BSL_META_U(unit,
                                     "No valid SBR profile tables found on "
                                     "this device: spt_count=%u - rv=%s."),
                          dev->spt_count, shr_errmsg(SHR_E_INIT)));
    }
    /* Allocate SPT state array memory. */
    alloc_sz = sizeof(*spt_state[0]) * dev->spt_count;
    SHR_ALLOC(spt_state[unit], alloc_sz, "bcmptmSbrPtStateArr");
    SHR_NULL_CHECK(spt_state[unit], SHR_E_MEMORY);
    sal_memset(spt_state[unit], 0, alloc_sz);
    dev->spt_state_blk_sz = alloc_sz;

    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        pb = shr_pb_create();
        shr_pb_printf(pb, "spt_state(count=%u) summary:\n"
                          "==============================\n", dev->spt_count);
    }
    /* Initialize the main SBR profile tables SW state. */
    for (spt_idx = 0, pt = 0;
         spt_idx < (vi->sbr_profile_table_count + tmp_spt_count[unit])
         && pt < dev->spt_count;
         spt_idx++) {
        if (sbr_ext_to_main_list_sid_match(vi->sbr_ext_to_main,
                                           final_spt_list[unit][spt_idx],
                                           FALSE, &mindex)) {
            /* Skip mem_ext[] SBR profile tables. */
            continue;
        }
        /*
         * Not a match in sbr_ext_to_main list, so manage this profile
         * table as independent main table.
         */
        spt_state[unit][pt].drd_sid = final_spt_list[unit][spt_idx];
        spt_state[unit][pt].ext_drd_sid = dev->invalid_ptid;
        spt_state[unit][pt].entries_count = bcmptm_pt_index_count(unit,
                                                spt_state[unit][pt].drd_sid);
        spt_state[unit][pt].free_entries_count
                                        = spt_state[unit][pt].entries_count;
        spt_state[unit][pt].estate_base_index = dev->spt_entries_count;
        dev->spt_entries_count += spt_state[unit][pt].entries_count;
        wsize = bcmdrd_pt_entry_wsize(unit, spt_state[unit][pt].drd_sid);
        if (wsize > dev->spt_entry_max_wsize) {
            dev->spt_entry_max_wsize = wsize;
        }
        if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE) && pb) {
            shr_pb_printf(pb, "pt=%-4u: SID=%u/EXT_SID=%d base_index=%u "
                              "ecnt=%u tot_lecnt=%u\n",
                          pt,
                          spt_state[unit][pt].drd_sid,
                          (int)spt_state[unit][pt].ext_drd_sid,
                          spt_state[unit][pt].estate_base_index,
                          spt_state[unit][pt].entries_count,
                          dev->spt_entries_count);
        }
        /* Increment spt_state array index value. */
        pt++;
    }
    if (pt != dev->spt_count) {
        SHR_ERR_MSG_EXIT(SHR_E_INIT,
                         (BSL_META_U(unit,
                                     "Expected to initialize SPT=%u count"
                                     "but initialized only %u - rv=%s."),
                          dev->spt_count, pt, shr_errmsg(SHR_E_INIT)));
    }
    /*
     * Search the sbr_ext_to_main[] array using the main drd_sid value
     * from spt_state[] array that was initialized in the previous for-loop.
     * For the main SBR profile tables that have a match, retrieve their
     * corresponding mem_ext[] drd_sid value using the mindex returned by
     * this function and initialize it in spt_state[].ext_drd_sid.
     */
    for (pt = 0; pt < dev->spt_count; pt++) {
        if (sbr_ext_to_main_list_sid_match(vi->sbr_ext_to_main,
                                           spt_state[unit][pt].drd_sid,
                                           TRUE, &mindex)) {
            /*
             * Update the ext SBR profile table drd_sid value for this main
             * SBR profile table.
             */
            spt_state[unit][pt].ext_drd_sid
                                = vi->sbr_ext_to_main->mem_ext[mindex];
            if ((wsize = bcmdrd_pt_entry_wsize(unit,
                                               spt_state[unit][pt].ext_drd_sid))
                > dev->spt_entry_max_wsize) {
                dev->spt_entry_max_wsize = wsize;
            }
        }
    }
    /* SBR table entries memory. */
    alloc_sz = (sizeof(*spt_estate[0]) * dev->spt_entries_count);
    SHR_ALLOC(spt_estate[unit], alloc_sz, "bcmptmSbrPtEntStateArr");
    SHR_NULL_CHECK(spt_estate[unit], SHR_E_MEMORY);
    sal_memset(spt_estate[unit], 0, alloc_sz);
    dev->spt_estate_blk_sz = alloc_sz;

    /* Tile state memory. */
    alloc_sz = sizeof(*tile_state[0]) * dev->tile_count;
    SHR_ALLOC(tile_state[unit], alloc_sz, "bcmptmSbrTileStateArr");
    SHR_NULL_CHECK(tile_state[unit], SHR_E_MEMORY);
    sal_memset(tile_state[unit], 0, alloc_sz);
    dev->tile_state_blk_sz = alloc_sz;

    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE) && pb) {
        shr_pb_printf(pb, "tile_state summary:\n"
                          "==================\n");
    }
    for (t = 0; t < vi->sbr_tile_info_count; t++) {
        v_ti = vi->sbr_tile_info + t;
        SHR_NULL_CHECK(v_ti, SHR_E_INIT);

        tile_state[unit][t].lt_lookup0_count = v_ti->sbr_lt_lookup0_info_count;
        if (tile_state[unit][t].lt_lookup0_count) {
            tile_state[unit][t].lt_lookup0_base_index = dev->lt_lkup_count;
        } else {
            tile_state[unit][t].lt_lookup0_base_index = SBR_UINT32_INVALID;
        }
        dev->lt_lkup_count += tile_state[unit][t].lt_lookup0_count;

        tile_state[unit][t].lt_lookup1_count = v_ti->sbr_lt_lookup1_info_count;
        if (tile_state[unit][t].lt_lookup1_count) {
            tile_state[unit][t].lt_lookup1_base_index = dev->lt_lkup_count;
        } else {
            tile_state[unit][t].lt_lookup1_base_index = SBR_UINT32_INVALID;
        }
        dev->lt_lkup_count += tile_state[unit][t].lt_lookup1_count;
        if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE) && pb) {
            shr_pb_printf(pb,
                          "t=%-2u: lkup0[base=%-4d cnt=%-4u ] lkup1[base=%-4d "
                          "cnt=%-4u] dev_total:lt_lkup_count=%-4u\n",
                          t,
                          (int)tile_state[unit][t].lt_lookup0_base_index,
                          tile_state[unit][t].lt_lookup0_count,
                          (int)tile_state[unit][t].lt_lookup1_base_index,
                          tile_state[unit][t].lt_lookup1_count,
                          dev->lt_lkup_count);
        }
    }
    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE) && pb) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "%s"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
        pb = NULL;
    }

    /* Tile LT.LOOKUP state memory. */
    alloc_sz = sizeof(*tile_lkup_state[0]) * dev->lt_lkup_count;
    SHR_ALLOC(tile_lkup_state[unit], alloc_sz, "bcmptmSbrTileLtLkupStateArr");
    SHR_NULL_CHECK(tile_lkup_state[unit], SHR_E_MEMORY);
    sal_memset(tile_lkup_state[unit], 0, alloc_sz);
    dev->tile_lkup_state_blk_sz = alloc_sz;
    for (l = 0; l < dev->lt_lkup_count; l++) {
        tile_lkup_state[unit][l].in_use = FALSE;
        tile_lkup_state[unit][l].entry_index = SBR_UINT16_INVALID;
        tile_lkup_state[unit][l].spt_sid = dev->invalid_ptid;
    }

    /* Initialize device info pointers for this unit. */
    dev->spt_state = spt_state[unit];
    dev->spt_estate = spt_estate[unit];
    dev->tile_state = tile_state[unit];
    dev->tile_lkup_state = tile_lkup_state[unit];
exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(spt_state[unit]);
        SHR_FREE(spt_estate[unit]);
        SHR_FREE(tile_state[unit]);
        SHR_FREE(tile_lkup_state[unit]);
    }
    shr_pb_destroy(pb);
    SHR_FREE(final_spt_list[unit]);
    final_spt_list[unit] = NULL;
    tmp_spt_count[unit] = 0;
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_sbr_device_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);
    SHR_FREE(spt_state[unit]);
    SHR_FREE(spt_estate[unit]);
    SHR_FREE(tile_state[unit]);
    SHR_FREE(tile_lkup_state[unit]);
    SHR_FUNC_EXIT();
}
/*******************************************************************************
 * Public Functions
 */
int
bcm56880_a0_sbr_driver_register(int unit, sbr_driver_t *drv)
{
    SHR_FUNC_ENTER(unit);
    if (drv) {
        drv->device_info_init = bcm56880_a0_sbr_device_info_init;
        drv->device_deinit = bcm56880_a0_sbr_device_deinit;
        drv->default_profiles_init = bcm56880_a0_sbr_default_profiles_init;
    } else {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    exit:
        SHR_FUNC_EXIT();
}
