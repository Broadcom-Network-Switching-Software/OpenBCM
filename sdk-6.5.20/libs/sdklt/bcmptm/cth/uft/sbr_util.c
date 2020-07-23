/*! \file sbr_util.c
 *
 * SBR utility functions.
 *
 * This file contains SBR utility functions.
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
#include <bcmbd/bcmbd.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include "sbr_util.h"
#include "spm_util.h"

/*******************************************************************************
 * Private variables
 */
/*!
 * \brief The SBR profile table entry usage mode names.
 *
 * The list of SBR entry usage modes must match the corresponding modes in
 * \ref bcmptm_sbr_entry_usage_t enum.
 */
static const char *usage_str[] = {
    "FREE",
    "RSVD_MISS",
    "RSVD_HIT",
    "RSVD_DEFAULT",
    "IN_USE"
};

/*!
 * \brief The Tile LT lookup type names.
 *
 * The list of LT lookup type strings must match the corresponding types in
 * \ref bcmptm_sbr_lt_lookup_t enum.
 */
static const char *lt_lookup_str[] = {
    "INVALID",
    "LOOKUP0",
    "LOOKUP1",
    "RESERVED"
};

/*!
 * \brief The SBR profile table entry operation names.
 *
 * The list of SBR profile table entry operations below must match the
 * corresponding constants defined in \ref sbr_pt_ent_oper_t enum.
 */
static const char *spt_ent_oper_str[] = {
    "NOOP",
    "ALLOC_INS",
    "FREE_CLR",
    "REF_INCR",
    "ALLOC",
    "FREE"
};

/*!
 * \brief Tile mode change LT.LOOKUP operation names.
 *
 * The list of LT.LOOKUP operations below must match the corresponding
 * constants defined in \ref sbr_tm_lt_lookup_oper_t enum.
 */
static const char *tm_lt_lookup_oper_str[] = {
    "NOOP",
    "ADD",
    "DELETE",
    "UPDATE"
};

/*******************************************************************************
 * Local Defintions
 */
/*! Combined layer/source for BSL logging. */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_UFT

/*! Read/Write all pipe instances for a given memory or register. */
#define SBR_PT_ALL_PIPES_INST (-1)
/*******************************************************************************
 * Public Functions
 */

int
bcmptm_sbr_lt_hw_read(int unit,
                      bool only_check_rsrc,
                      bcmltd_sid_t lt_id,
                      const bcmltd_op_arg_t *op_arg,
                      bcmdrd_sid_t pt_id,
                      int index,
                      uint32_t entry_size,
                      void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t rsp_entry_wsize;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;
    uint64_t req_flags = 0;
    const char *lt_name = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_value_to_symbol(unit,
                                      TABLE_INFOt,
                                      TABLE_INFOt_TABLE_IDf,
                                      lt_id,
                                      &lt_name));
    /* Skip HW entry write operation when only_check_rsrc is TRUE. */
    if (only_check_rsrc) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "(only_check_rsrc=%u): lt_id=%s pt_id=%s "
                              "index=%d e_sz=%u.\n"),
                   only_check_rsrc, lt_name,
                   bcmdrd_pt_sid_to_name(unit, pt_id), index, entry_size));
        SHR_EXIT();
    }
    pt_info.index = index;
    pt_info.tbl_inst = SBR_PT_ALL_PIPES_INST;
    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, pt_id);
    if (entry_size < rsp_entry_wsize) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "(only_check_rsrc=%u): lt_id=%s pt_id=%s "
                              "index=%d e_sz=%u.\n"),
                   only_check_rsrc, lt_name, bcmdrd_pt_sid_to_name(unit, pt_id),
                   index, entry_size));
    }
    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit, req_flags, pt_id, &pt_info,
                                    NULL, NULL, BCMPTM_OP_READ,
                                    NULL, rsp_entry_wsize, lt_id,
                                    op_arg->trans_id, NULL, NULL,
                                    entry_data, &rsp_ltid, &rsp_flags));
 exit:
    SHR_FUNC_EXIT();
}


int
bcmptm_sbr_lt_hw_write(int unit,
                       bool only_check_rsrc,
                       bcmltd_sid_t lt_id,
                       const bcmltd_op_arg_t *op_arg,
                       bcmdrd_sid_t pt_id,
                       int index,
                       void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;
    uint64_t req_flags = 0;
    const char *lt_name = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_value_to_symbol(unit,
                                      TABLE_INFOt,
                                      TABLE_INFOt_TABLE_IDf,
                                      lt_id,
                                      &lt_name));

    /* Skip HW entry write operation when only_check_rsrc is TRUE. */
    if (only_check_rsrc) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "(only_check_rsrc=%u): lt_id=%s pt_id=%s "
                              "index=%d.\n"),
                   only_check_rsrc, lt_name,
                   bcmdrd_pt_sid_to_name(unit, pt_id), index));
        SHR_EXIT();
    }
    pt_info.index = index;
    pt_info.tbl_inst = SBR_PT_ALL_PIPES_INST;
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "(only_check_rsrc=%u): lt_id=%s pt_id=%s "
                              "index=%d.\n"),
                   only_check_rsrc, lt_name, bcmdrd_pt_sid_to_name(unit, pt_id),
                   index));
    }
    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit, req_flags, pt_id, &pt_info,
                                    NULL, NULL, BCMPTM_OP_WRITE,
                                    entry_data, 0, lt_id,
                                    op_arg->trans_id, NULL, NULL,
                                    NULL, &rsp_ltid, &rsp_flags));
 exit:
    SHR_FUNC_EXIT();
}

const char *
bcmptm_sbr_ent_usage_str(bcmptm_sbr_entry_usage_t usage)
{
    uint8_t idx = BCMPTM_SBR_ENTRY_FREE;

    if (usage >= BCMPTM_SBR_ENTRY_FREE
        && usage <= BCMPTM_SBR_ENTRY_IN_USE) {
        idx = usage;
    }
    return (usage_str[idx]);
}

const char *
bcmptm_sbr_lt_lkup_str(bcmptm_sbr_lt_lookup_t lkup)
{
    uint8_t idx = BCMPTM_SBR_LOOKUP_INVALID;

    if (lkup >= BCMPTM_SBR_LOOKUP_INVALID
        && lkup <= BCMPTM_SBR_LOOKUP_RESERVED) {
        idx = lkup;
    }
    return (lt_lookup_str[idx]);
}

const char *
bcmptm_sbr_pt_ent_oper_str(sbr_pt_ent_oper_t spt_eoper)
{
    uint8_t idx = SBR_PT_ENT_NOOP;

    if (spt_eoper >= SBR_PT_ENT_NOOP
        && spt_eoper < SBR_PT_OPER_COUNT) {
        idx = spt_eoper;
    }
    return (spt_ent_oper_str[idx]);
}

const char *
bcmptm_sbr_tm_lt_lookup_oper_str(sbr_tm_lt_lookup_oper_t ltlkup_oper)
{
    uint8_t idx = SBR_TM_LT_LOOKUP_NOOP;

    if (ltlkup_oper >= SBR_TM_LT_LOOKUP_NOOP
        && ltlkup_oper < SBR_TM_LT_LOOKUP_OPER_COUNT) {
        idx = ltlkup_oper;
    }
    return (tm_lt_lookup_oper_str[idx]);
}

int
bcmptm_sbr_tile_state_t_init(int unit,
                             bcmptm_sbr_tile_state_t *ts)
{
    uint32_t tm = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ts, SHR_E_PARAM);
    if (ts) {
        SHR_IF_ERR_EXIT
            (bcmlrd_field_symbol_to_value(unit, DEVICE_EM_TILEt,
                                          DEVICE_EM_TILEt_MODEf,
                                          "INVALID", &tm));
        ts->tile_mode = tm;
        ts->lt_lookup0_base_index = SBR_UINT32_INVALID;
        ts->lt_lookup0_count = 0;
        ts->lt_lookup1_base_index = SBR_UINT32_INVALID;
        ts->lt_lookup1_count = 0;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_sbr_lt_lookup_state_tile_t_init(
                                int unit,
                                sbr_dev_info_t *di,
                                bcmptm_sbr_lt_lookup_state_tile_t *ltlkup)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(di, SHR_E_PARAM);
    SHR_NULL_CHECK(ltlkup, SHR_E_PARAM);

    ltlkup->in_use = FALSE;
    ltlkup->entry_index = SBR_UINT16_INVALID;
    ltlkup->spt_sid = di->invalid_ptid;
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_sbr_pt_state_t_init(int unit,
                           sbr_dev_info_t *di,
                           bcmptm_sbr_pt_state_t *spt)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(di, SHR_E_PARAM);
    SHR_NULL_CHECK(spt, SHR_E_PARAM);

    spt->drd_sid = di->invalid_ptid;
    spt->ext_drd_sid = di->invalid_ptid;
    spt->entries_count = 0;
    spt->free_entries_count = 0;
    spt->estate_base_index = SBR_UINT16_INVALID;
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_sbr_entry_state_t_init(int unit,
                              sbr_dev_info_t *di,
                              bcmptm_sbr_entry_state_t *spt_estate)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(di, SHR_E_PARAM);
    SHR_NULL_CHECK(spt_estate, SHR_E_PARAM);

    spt_estate->ref_count = 0;
    spt_estate->usage_mode = BCMPTM_SBR_ENTRY_FREE;
    spt_estate->ltid = di->invalid_ltid;
    spt_estate->lookup_type = BCMPTM_SBR_LOOKUP_INVALID;
    spt_estate->pt_state_index = SBR_UINT16_INVALID;
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_sbr_find_tile_by_tile_mode(int unit,
                                  const bcmptm_sbr_var_drv_t *drv,
                                  uint32_t tile_mode,
                                  uint16_t *tile_index)
{
    uint16_t t, tmi; /* tile mode index. */
    const bcmptm_sbr_tile_info_t *v_ti = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(drv, SHR_E_PARAM);
    SHR_NULL_CHECK(drv->sbr_tile_info, SHR_E_PARAM);
    SHR_NULL_CHECK(tile_index, SHR_E_PARAM);

    for (t = 0; t < drv->tile_count; t++) {
        v_ti = drv->sbr_tile_info + t;
        for (tmi = 0; tmi < v_ti->tile_mode_count; tmi++) {
            if (tile_mode == v_ti->tile_mode[tmi]) {
                /*
                 * Input tile_mode matches the variant driver tile_mode value
                 * for the tile at this index, return the tile index value.
                 */
                *tile_index = t;
                SHR_EXIT();
            }
        }
    }
    /*
     * Input tile_mode not found in the variant driver tile_info array,
     * after searching in all the tiles. This tile/tile_mode has been deleted
     * in this variant driver.
     */
    SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_sbr_find_tile_by_tile_id(int unit,
                                sbr_dev_info_t *di,
                                uint32_t tile_id,
                                uint16_t *tile_index)
{
    uint16_t idx = 0, low_idx = 0, high_idx = 0; /* Index iterator variables. */
    uint32_t *ha_tile_id = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(di, SHR_E_PARAM);
    SHR_NULL_CHECK(di->v_tileid_state_ha, SHR_E_PARAM);
    SHR_NULL_CHECK(di->tile_state_ha, SHR_E_PARAM);
    SHR_NULL_CHECK(tile_index, SHR_E_PARAM);

    if (di->issu_old_tile_count == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    low_idx = 0;
    high_idx = di->issu_old_tile_count - 1;
    /* Array is sorted by the tile_id value, so do a binary search. */
    while (low_idx <= high_idx) {
        idx = (low_idx + high_idx) / 2;
         ha_tile_id = &di->v_tileid_state_ha->array[idx];
        if (*ha_tile_id == tile_id) {
            *tile_index = idx;
            SHR_EXIT();
        }
        if (*ha_tile_id > tile_id) {
            high_idx = idx - 1;
        } else {
            low_idx = idx + 1;
        }
    }
    SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_sbr_find_spt_by_spt_id(int unit,
                                sbr_dev_info_t *di,
                                uint32_t spt_drd_sid,
                                uint16_t *spt_index)
{
    uint16_t idx = 0, low_idx = 0, high_idx = 0; /* Index iterator variables. */
    uint32_t *ha_spt_id = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(di, SHR_E_PARAM);
    SHR_NULL_CHECK(di->v_spt_state_ha, SHR_E_PARAM);
    SHR_NULL_CHECK(di->spt_state_ha, SHR_E_PARAM);
    SHR_NULL_CHECK(spt_index, SHR_E_PARAM);

    if (di->issu_old_spt_count == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    low_idx = 0;
    high_idx = di->issu_old_spt_count - 1;
    /*
     * sbr_profile_table[] array is sorted by the drd_sid value, so do a binary
     * search.
     */
    while (low_idx <= high_idx) {
        idx = (low_idx + high_idx) / 2;
        ha_spt_id = &di->v_spt_state_ha->array[idx];
        if (*ha_spt_id == spt_drd_sid) {
            *spt_index = idx;
            SHR_EXIT();
        }
        if (*ha_spt_id > spt_drd_sid) {
            high_idx = idx - 1;
        } else {
            low_idx = idx + 1;
        }
    }
    SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_sbr_find_tile_ltlkup_by_lkupltid(int unit,
                                        sbr_dev_info_t *di,
                                        uint16_t tile_index,
                                        uint32_t ltid,
                                        uint16_t lookup_type,
                                        uint16_t *ltlkup_index)
{
    uint16_t idx = 0, low_idx = 0, high_idx = 0; /* Index iterator variables. */
    bcmptm_sbr_tile_state_t *ha_ts = NULL;
    bcmptm_sbr_var_tile_ltlkup_t *v_tltlkup = NULL;
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(di, SHR_E_PARAM);
    SHR_NULL_CHECK(di->v_tltlkup_state_ha, SHR_E_PARAM);
    SHR_NULL_CHECK(di->tile_lkup_state_ha, SHR_E_PARAM);
    SHR_NULL_CHECK(di->tile_state_ha, SHR_E_PARAM);
    SHR_NULL_CHECK(ltlkup_index, SHR_E_PARAM);

    ha_ts = &di->tile_state_ha[tile_index];
    low_idx = 0;
    switch (lookup_type) {
        case BCMPTM_SBR_LOOKUP0:
            if (ha_ts->lt_lookup0_count == 0) {
                SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
            }
            low_idx = ha_ts->lt_lookup0_base_index;
            high_idx = (ha_ts->lt_lookup0_base_index
                        + ha_ts->lt_lookup0_count) - 1;
            break;
        case BCMPTM_SBR_LOOKUP1:
            if (ha_ts->lt_lookup1_count == 0) {
                SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
            }
            low_idx = ha_ts->lt_lookup1_base_index;
            high_idx = (ha_ts->lt_lookup1_base_index
                        + ha_ts->lt_lookup1_count) - 1;
            break;
        default:
            SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
    for (idx = low_idx; idx <= high_idx; idx++) {
        v_tltlkup = &di->v_tltlkup_state_ha->array[idx];
        if (v_tltlkup->ltid == ltid
            && v_tltlkup->lookup_type == lookup_type) {
            /* Set match index and return from here. */
            *ltlkup_index = idx;
            SHR_EXIT();
        }
    }
    SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT();
}
