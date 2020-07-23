/*! \file sbr.c
 *
 * Strength Based Resolution (SBR) tables Resource Manager (RM).
 *
 * This file contains the main functions for SBR-RM.
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
#include <shr/shr_ha.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_pb.h>
#include <shr/shr_pb_local.h>
#include <bcmbd/bcmbd.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmissu/issu_api.h>
#include <bcmptm/bcmptm_sbr_internal.h>
#include "sbr_device.h"
#include "sbr_internal.h"
#include "sbr_util.h"
/*******************************************************************************
 * Defines
 */
/*! Combined layer/source for BSL logging. */
#define BSL_LOG_MODULE  BSL_LS_BCMPTM_UFT

/*******************************************************************************
 * Private variables
 */
/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Device SBR profile tables state HA block handler function.
 */
static int
sbr_pt_state_ha_alloc(int unit,
                      bool warm,
                      sbr_dev_info_t *di,
                      uint8_t submod_id,
                      bcmptm_sbr_pt_state_t **alloc_ptr,
                      uint32_t *out_ha_alloc_sz)
{
    bcmptm_sbr_pt_state_t *pt_state = NULL;
    uint32_t ha_alloc_sz = 0;
    uint32_t in_req_sz = 0;
    uint16_t t = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(di, SHR_E_INIT);
    SHR_NULL_CHECK(di->var, SHR_E_INIT);
    SHR_NULL_CHECK(di->v_spt_state_ha, SHR_E_INIT);
    SHR_NULL_CHECK(alloc_ptr, SHR_E_PARAM);
    SHR_NULL_CHECK(out_ha_alloc_sz, SHR_E_PARAM);

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "\"warm = %s\", sbr_pt_count=%u.\n"),
               warm ? "TRUE" : "FALSE", di->spt_count));
    /*
     * Current SBR profile tables count match the profile tables count in
     * the variant driver.
     */
    if (di->spt_count != di->var->sbr_profile_table_count) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    /* Allocate one HA block for all SBR profile tables on this device. */
    in_req_sz = sizeof(bcmptm_sbr_pt_state_t) * di->spt_count;
    /*
     * shr_ha_mem_alloc() function will update ha_alloc_sz variable with the
     * actual size of HA block allocated.
     */
    ha_alloc_sz = in_req_sz;
    pt_state = shr_ha_mem_alloc(unit,
                                BCMMGMT_UFT_COMP_ID,
                                submod_id,
                                "ptmUftSbrPtStateHaBlk",
                                &ha_alloc_sz);
    SHR_NULL_CHECK(pt_state, SHR_E_MEMORY);
    if (ha_alloc_sz < in_req_sz) {
        if (warm && bcmissu_is_active() && di->issu_spt_add) {
            /* Reallocate memory based on the new memory requirements. */
            pt_state = shr_ha_mem_realloc(unit, pt_state, in_req_sz);
            SHR_NULL_CHECK(pt_state, SHR_E_MEMORY);
            /* Initialize new HA structure instances. */
            for (t = di->issu_old_spt_count; t < di->spt_count; t++) {
                SHR_IF_ERR_EXIT
                    (bcmptm_sbr_pt_state_t_init(unit, di, &pt_state[t]));
            }
        } else {
            SHR_IF_ERR_MSG_EXIT(SHR_E_MEMORY,
                                (BSL_META_U(unit,
                                            "HA blk req_sz=%u alloc_sz=%u.\n"),
                                 in_req_sz, ha_alloc_sz));
        }
    }
    if (!warm) {
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit,
                                        BCMMGMT_UFT_COMP_ID,
                                        submod_id, 0,
                                        sizeof(bcmptm_sbr_pt_state_t),
                                        di->spt_count,
                                        BCMPTM_SBR_PT_STATE_T_ID));
    }
    *alloc_ptr = pt_state;
    *out_ha_alloc_sz = ha_alloc_sz;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Device SBR profile tables entries state HA block handler function.
 */
static int
sbr_pt_estate_ha_alloc(int unit,
                       bool warm,
                       sbr_dev_info_t *di,
                       uint8_t submod_id,
                       bcmptm_sbr_entry_state_t **alloc_ptr,
                       uint32_t *out_ha_alloc_sz)
{
    bcmptm_sbr_entry_state_t *pt_estate = NULL;
    uint32_t ha_alloc_sz = 0;
    uint32_t in_req_sz = 0, t = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(di, SHR_E_INIT);
    SHR_NULL_CHECK(di->spt_state_ha, SHR_E_INIT);
    SHR_NULL_CHECK(alloc_ptr, SHR_E_PARAM);
    SHR_NULL_CHECK(out_ha_alloc_sz, SHR_E_PARAM);

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "\"warm = %s\", sbr_pt_ecount=%u.\n"),
               warm ? "TRUE" : "FALSE", di->spt_entries_count));

    /* Validate device SBR profile tables entries count. */
    if (di->spt_entries_count == 0
        || di->spt_entries_count < di->spt_count) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    /* Allocate one HA block for all SBR profile tables on this device. */
    in_req_sz = sizeof(bcmptm_sbr_entry_state_t) * di->spt_entries_count;
    /*
     * shr_ha_mem_alloc() function will update ha_alloc_sz variable with the
     * actual size of HA block allocated.
     */
    ha_alloc_sz = in_req_sz;
    pt_estate = shr_ha_mem_alloc(unit,
                                 BCMMGMT_UFT_COMP_ID,
                                 submod_id,
                                 "ptmUftSbrPtEntStateHaBlk",
                                 &ha_alloc_sz);
    SHR_NULL_CHECK(pt_estate, SHR_E_MEMORY);
    /*
     * Calculate index of the last entry (count - 1) in the old pt_estate HA
     * block using the SBR profile table entries_count stored in the
     * spt_state_ha block.
     */
    for (t = 0; t < di->issu_old_spt_count; t++) {
        di->issu_old_spt_ecount += di->spt_state_ha[t].entries_count;
    }

    if (ha_alloc_sz < in_req_sz) {
        if (warm && bcmissu_is_active() && di->issu_spt_add) {
            /* Reallocate memory based on the new memory requirements. */
            pt_estate = shr_ha_mem_realloc(unit, pt_estate, in_req_sz);
            SHR_NULL_CHECK(pt_estate, SHR_E_MEMORY);
            /* Initialize the new structure instances in the HA block. */
            for (t = di->issu_old_spt_ecount; t < di->spt_entries_count; t++) {
                SHR_IF_ERR_EXIT
                    (bcmptm_sbr_entry_state_t_init(unit, di, &pt_estate[t]));
            }
        } else {
            SHR_IF_ERR_MSG_EXIT(SHR_E_MEMORY,
                                (BSL_META_U(unit,
                                            "HA blk req_sz=%u alloc_sz=%u.\n"),
                                 in_req_sz,
                                 ha_alloc_sz));
        }
    }
    if (!warm) {
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit,
                                        BCMMGMT_UFT_COMP_ID,
                                        submod_id, 0,
                                        sizeof(bcmptm_sbr_entry_state_t),
                                        di->spt_entries_count,
                                        BCMPTM_SBR_ENTRY_STATE_T_ID));
    }
    *alloc_ptr = pt_estate;
    *out_ha_alloc_sz = ha_alloc_sz;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Device tiles state HA block handler function.
 */
static int
sbr_tile_state_ha_alloc(int unit,
                        bool warm,
                        sbr_dev_info_t *di,
                        uint8_t submod_id,
                        bcmptm_sbr_tile_state_t **alloc_ptr,
                        uint32_t *out_ha_alloc_sz)
{
    bcmptm_sbr_tile_state_t *tile_state = NULL;
    uint32_t ha_alloc_sz = 0, in_req_sz = 0;
    uint16_t t = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(di, SHR_E_INIT);
    SHR_NULL_CHECK(di->var, SHR_E_INIT);
    SHR_NULL_CHECK(alloc_ptr, SHR_E_PARAM);
    SHR_NULL_CHECK(out_ha_alloc_sz, SHR_E_PARAM);

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "\"warm = %s\", tile_count=%u.\n"),
               warm ? "TRUE" : "FALSE", di->tile_count));

    /* Total tiles count must match the device variant tiles count. */
    if (di->tile_count != di->var->tile_count) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }
    /* Allocate one HA block for all SBR profile tables on this device. */
    in_req_sz = sizeof(bcmptm_sbr_tile_state_t) * di->tile_count;
    /*
     * shr_ha_mem_alloc() function will update ha_alloc_sz variable with the
     * actual size of HA block allocated.
     */
    ha_alloc_sz = in_req_sz;
    tile_state = shr_ha_mem_alloc(unit,
                                  BCMMGMT_UFT_COMP_ID,
                                  submod_id,
                                  "ptmUftSbrTileStateHaBlk",
                                  &ha_alloc_sz);
    SHR_NULL_CHECK(tile_state, SHR_E_MEMORY);
    if (ha_alloc_sz < in_req_sz) {
        if (warm && bcmissu_is_active() && di->issu_tile_add) {
            /* Reallocate memory based on the new memory requirements. */
            tile_state = shr_ha_mem_realloc(unit, tile_state, in_req_sz);
            SHR_NULL_CHECK(tile_state, SHR_E_MEMORY);
            /* Initialize new HA structure instances. */
            for (t = di->issu_old_tile_count; t < di->tile_count; t++) {
                SHR_IF_ERR_EXIT
                    (bcmptm_sbr_tile_state_t_init(unit, &tile_state[t]));
            }
        } else {
            SHR_IF_ERR_MSG_EXIT(SHR_E_MEMORY,
                                (BSL_META_U(unit,
                                            "HA blk req_sz=%u alloc_sz=%u.\n"),
                                 in_req_sz, ha_alloc_sz));
        }
    }
    if (!warm) {
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_UFT_COMP_ID, submod_id, 0,
                                        sizeof(bcmptm_sbr_tile_state_t),
                                        di->tile_count,
                                        BCMPTM_SBR_TILE_STATE_T_ID));
    }
    *alloc_ptr = tile_state;
    *out_ha_alloc_sz = ha_alloc_sz;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Device tiles LT lookup entries state HA block handler function.
 */
static int
sbr_tile_lt_lookup_state_ha_alloc(int unit,
                                  bool warm,
                                  sbr_dev_info_t *di,
                                  uint8_t submod_id,
                                  bcmptm_sbr_lt_lookup_state_tile_t **alloc_ptr,
                                  uint32_t *out_ha_alloc_sz)
{
    bcmptm_sbr_lt_lookup_state_tile_t *tile_lt_lkup = NULL;
    uint32_t ha_alloc_sz = 0;
    uint32_t in_req_sz = 0;
    uint16_t t = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(di, SHR_E_INIT);
    SHR_NULL_CHECK(di->tile_state_ha, SHR_E_INIT);
    SHR_NULL_CHECK(alloc_ptr, SHR_E_PARAM);
    SHR_NULL_CHECK(out_ha_alloc_sz, SHR_E_PARAM);

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                         "\"warm = %s\", lt_lookup_count=%u.\n"),
               warm ? "TRUE" : "FALSE", di->lt_lkup_count));

    /* Device LT.LOOKUP count must be valid. */
    if (di->lt_lkup_count == 0) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }
    /*
     * Calculate the LT lookup state entries HA block size for all tiles on
     * this device.
     */
    in_req_sz = sizeof(bcmptm_sbr_lt_lookup_state_tile_t) * di->lt_lkup_count;
    /*
     * shr_ha_mem_alloc() function will update ha_alloc_sz variable with the
     * actual size of HA block allocated.
     */
    ha_alloc_sz = in_req_sz;
    tile_lt_lkup = shr_ha_mem_alloc(unit,
                                    BCMMGMT_UFT_COMP_ID,
                                    submod_id,
                                    "ptmUftSbrTileLtLkupStateHaBlk",
                                    &ha_alloc_sz);
    SHR_NULL_CHECK(tile_lt_lkup, SHR_E_MEMORY);
    if (ha_alloc_sz < in_req_sz) {
        if (warm && bcmissu_is_active() && di->issu_tile_lkup_add) {
            /* Reallocate memory based on the new memory requirements. */
            tile_lt_lkup = shr_ha_mem_realloc(unit, tile_lt_lkup, in_req_sz);
            SHR_NULL_CHECK(tile_lt_lkup, SHR_E_MEMORY);
            /* Initialize the new structure instances in the HA block. */
            for (t = di->issu_old_lt_lkup_count; t < di->lt_lkup_count; t++) {
                SHR_IF_ERR_EXIT
                    (bcmptm_sbr_lt_lookup_state_tile_t_init(unit, di,
                                                            &tile_lt_lkup[t]));
            }
        } else {
            SHR_IF_ERR_MSG_EXIT(SHR_E_MEMORY,
                                (BSL_META_U(unit,
                                            "HA blk req_sz=%u alloc_sz=%u.\n"),
                                 in_req_sz,
                                 ha_alloc_sz));
        }
    }
    if (!warm) {
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit,
                                    BCMMGMT_UFT_COMP_ID,
                                    submod_id, 0,
                                    sizeof(bcmptm_sbr_lt_lookup_state_tile_t),
                                    di->lt_lkup_count,
                                    BCMPTM_SBR_LT_LOOKUP_STATE_TILE_T_ID));
    }
    *alloc_ptr = tile_lt_lkup;
    *out_ha_alloc_sz = ha_alloc_sz;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Store SBR profile table details specific to this variant driver in HA.
 */
static int
bcmptm_sbr_var_spt_state_init(int unit,
                              uint16_t spt_count,
                              bcmptm_sbr_var_spt_state_t *spt_state)
{
    uint16_t t = 0;
    sbr_dev_info_t *di = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get the SBR device info pointer for the given unit. */
    di = bcmptm_sbr_device_info_get(unit);
    SHR_NULL_CHECK(di, SHR_E_INIT);
    SHR_NULL_CHECK(di->var, SHR_E_INIT);
    SHR_NULL_CHECK(di->var->sbr_profile_table, SHR_E_INIT);

    for (t = 0; t < spt_count; t++) {
        spt_state->array[t] = di->var->sbr_profile_table[t];
    }
    spt_state->array_size = spt_count;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Device variant SBR physical table state HA block alloc function.
 */
static int
sbr_var_spt_state_ha_alloc(int unit,
                           bool warm,
                           sbr_dev_info_t *di,
                           uint8_t submod_id,
                           bcmptm_sbr_var_spt_state_t **alloc_ptr,
                           uint32_t *out_ha_alloc_sz)
{
    bcmptm_sbr_var_spt_state_t *spt_state = NULL;
    uint32_t ha_alloc_sz = 0;
    uint32_t in_req_sz = 0;
    uint32_t offset = 0, t;
    int rv = SHR_E_INTERNAL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(alloc_ptr, SHR_E_PARAM);
    SHR_NULL_CHECK(out_ha_alloc_sz, SHR_E_PARAM);
    SHR_NULL_CHECK(di, SHR_E_INIT);
    SHR_NULL_CHECK(di->var, SHR_E_INIT);
    SHR_NULL_CHECK(di->var->sbr_profile_table, SHR_E_INIT);

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                         "\"warm = %s\", spt_count=%u.\n"),
               warm ? "TRUE" : "FALSE", di->spt_count));

    /*
     * Current SBR profile tables count match the profile tables count in
     * the variant driver.
     */
    if (di->spt_count != di->var->sbr_profile_table_count) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }
    /*
     * Calculate variant SBR profile tables state HA block size for all the
     * profile tables in this version.
     */
    in_req_sz = (sizeof(bcmptm_sbr_var_spt_state_t)
                 + sizeof(uint32_t) * di->spt_count);
    /*
     * shr_ha_mem_alloc() function will update ha_alloc_sz variable with the
     * actual size of HA block allocated.
     */
    ha_alloc_sz = in_req_sz;
    spt_state = shr_ha_mem_alloc(unit, BCMMGMT_UFT_COMP_ID, submod_id,
                                 "ptmUftSbrVarSptStateHaBlk",
                                 &ha_alloc_sz);
    SHR_NULL_CHECK(spt_state, SHR_E_MEMORY);
    if (bcmissu_is_active() && warm) {
        /*
         * If ISSU is active, check if this HA block has been previously
         * reported to ISSU.
         */
        rv = bcmissu_struct_info_get(unit,
                                     BCMPTM_SBR_VAR_SPT_STATE_T_ID,
                                     BCMMGMT_UFT_COMP_ID,
                                     submod_id,
                                     &offset);
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "BCMPTM_SBR_VAR_SPT_STATE_T_ID: "
                              "bcmissu_struct_info_get(rv=%s) offset=%u\n"),
                   shr_errmsg(rv), offset));
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
        if (rv == SHR_E_NOT_FOUND) {
            /*
             * If E_NOT_FOUND is returned, then this block did not exist in the
             * older version, report the new block structure details to ISSU.
             */
            SHR_IF_ERR_EXIT
                (bcmissu_struct_info_report(unit,
                                            BCMMGMT_UFT_COMP_ID,
                                            submod_id, 0, in_req_sz, 1,
                                            BCMPTM_SBR_VAR_SPT_STATE_T_ID));
            /*
             * Store the SBR profile table details for this SDK version in the
             * HA for use during ISSU.
             */
            SHR_IF_ERR_EXIT
                (bcmptm_sbr_var_spt_state_init(unit, di->spt_count, spt_state));
        } else {
            /* Get the old SPT count. */
            di->issu_old_spt_count = spt_state->array_size;
        }
    }

    if (ha_alloc_sz < in_req_sz) {
        if (warm && bcmissu_is_active()
            && spt_state->array_size < di->spt_count) {
            /*
             * Array size must be valid and non-zero if this HA block was valid
             * and in-use previously.
             */
            if (spt_state->array_size == 0) {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
            /* Reallocate memory based on the new memory requirements. */
            spt_state = shr_ha_mem_realloc(unit, spt_state, in_req_sz);
            SHR_NULL_CHECK(spt_state, SHR_E_MEMORY);
            for (t = spt_state->array_size; t < di->spt_count; t++) {
                /*
                 * Initialize to invalid memory value, which is the BCMDRD ID
                 * value defined in DRD module.
                 */
                spt_state->array[t] = di->invalid_ptid;
            }
            /*
             * New profile tables have been added, review and rebuild HA
             * state.
             */
            di->issu_spt_add = TRUE;
            /* Save the old SPT count for reference to rebuild the state. */
            di->issu_old_spt_count = spt_state->array_size;
            /* Update the new array size. */
            spt_state->array_size = di->spt_count;
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "spt_add: issu_spt_add=%u "
                                  "issu_old_spt_count=%u "
                                  "spt_state->array_size=%u\n"),
                       di->issu_spt_add, di->issu_old_spt_count,
                       spt_state->array_size));
        } else {
            SHR_IF_ERR_MSG_EXIT(SHR_E_MEMORY,
                                (BSL_META_U(unit,
                                            "HA blk req_sz=%u alloc_sz=%u.\n"),
                                 in_req_sz, ha_alloc_sz));
        }
    }
    if (warm && spt_state->array_size > di->spt_count) {
        if (bcmissu_is_active() && ha_alloc_sz > in_req_sz) {
            /*
             * Some profile tables have been deleted in new SDK version, review
             * and rebuild the HA state.
             */
            di->issu_spt_del = TRUE;
            di->issu_old_spt_count = spt_state->array_size;
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "spt_del: issu_spt_del=%u "
                                  "issu_old_spt_count=%u \n"),
                       di->issu_spt_del, di->issu_old_spt_count));
        } else {
            /*
             * Regular warm-boot should not result in SBR profile tables being
             * deleted.
             */
            SHR_IF_ERR_MSG_EXIT(SHR_E_MEMORY,
                                (BSL_META_U(unit,
                                            "HA blk req_sz=%u alloc_sz=%u.\n"),
                                 in_req_sz, ha_alloc_sz));
        }
    }
    if (!warm) {
        /* Report HA block info to ISSU during regular cold-boot switch init. */
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_UFT_COMP_ID,
                                        submod_id, 0, in_req_sz, 1,
                                        BCMPTM_SBR_VAR_SPT_STATE_T_ID));
        /*
         * Store the SBR profile table details for this SDK version in HA for
         * use during ISSU.
         */
        SHR_IF_ERR_EXIT
            (bcmptm_sbr_var_spt_state_init(unit, di->spt_count, spt_state));
    }
    /* Assign to function out parameters. */
    *alloc_ptr = spt_state;
    *out_ha_alloc_sz = ha_alloc_sz;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Store Tile details specific to this variant driver in HA.
 */
static int
bcmptm_sbr_var_tile_id_state_init(int unit,
                                  uint16_t tile_count,
                                  bcmptm_sbr_var_tile_id_state_t *tile_id)
{
    uint16_t t = 0;
    sbr_dev_info_t *di = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get the SBR device info pointer for the given unit. */
    di = bcmptm_sbr_device_info_get(unit);
    SHR_NULL_CHECK(di, SHR_E_INIT);
    SHR_NULL_CHECK(di->var, SHR_E_INIT);
    SHR_NULL_CHECK(di->var->sbr_tile_info, SHR_E_INIT);

    for (t = 0; t < tile_count; t++) {
        tile_id->array[t] = di->var->sbr_tile_info[t].tile_id;
    }
    tile_id->array_size = tile_count;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Device variant tile ID state HA block alloc function.
 */
static int
sbr_var_tileid_state_ha_alloc(int unit,
                              bool warm,
                              sbr_dev_info_t *di,
                              uint8_t submod_id,
                              bcmptm_sbr_var_tile_id_state_t **alloc_ptr,
                              uint32_t *out_ha_alloc_sz)
{
    bcmptm_sbr_var_tile_id_state_t *tile_id = NULL;
    uint32_t ha_alloc_sz = 0;
    uint32_t in_req_sz = 0;
    uint32_t offset = 0, t;
    int rv = SHR_E_INTERNAL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(di, SHR_E_INIT);
    SHR_NULL_CHECK(di->var, SHR_E_INIT);
    SHR_NULL_CHECK(di->var->sbr_tile_info, SHR_E_INIT);
    SHR_NULL_CHECK(alloc_ptr, SHR_E_PARAM);
    SHR_NULL_CHECK(out_ha_alloc_sz, SHR_E_PARAM);

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                         "\"warm = %s\", tile_count=%u.\n"),
               warm ? "TRUE" : "FALSE", di->tile_count));

    /* Tiles count must match the device variant tiles count. */
    if (di->tile_count != di->var->tile_count) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    /*
     * Calculate variant tile ID state HA block size for all tiles in this
     * version.
     */
    in_req_sz = (sizeof(bcmptm_sbr_var_tile_id_state_t)
                 + sizeof(uint32_t) * di->tile_count);
    /*
     * shr_ha_mem_alloc() function will update ha_alloc_sz variable with the
     * actual size of HA block allocated.
     */
    ha_alloc_sz = in_req_sz;
    tile_id = shr_ha_mem_alloc(unit, BCMMGMT_UFT_COMP_ID, submod_id,
                               "ptmUftSbrVarTileIdStateHaBlk",
                               &ha_alloc_sz);
    SHR_NULL_CHECK(tile_id, SHR_E_MEMORY);
    if (bcmissu_is_active() && warm) {
        /*
         * If ISSU is active, check if this HA block has been previously
         * reported to ISSU.
         */
        rv = bcmissu_struct_info_get(unit,
                                     BCMPTM_SBR_VAR_TILE_ID_STATE_T_ID,
                                     BCMMGMT_UFT_COMP_ID,
                                     submod_id,
                                     &offset);
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "BCMPTM_SBR_VAR_TILE_ID_STATE_T_ID: "
                              "bcmissu_struct_info_get(rv=%s) offset=%u\n"),
                   shr_errmsg(rv), offset));
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
        if (rv == SHR_E_NOT_FOUND) {
            /*
             * If E_NOT_FOUND is returned, then this block did not exist in the
             * older version, report the new block structure details to ISSU.
             */
            SHR_IF_ERR_EXIT
                (bcmissu_struct_info_report(unit,
                                            BCMMGMT_UFT_COMP_ID,
                                            submod_id, 0, in_req_sz, 1,
                                            BCMPTM_SBR_VAR_TILE_ID_STATE_T_ID));
            /*
             * Store the tile details for this SDK versionin HA, for use
             * during ISSU.
             */
            SHR_IF_ERR_EXIT
                (bcmptm_sbr_var_tile_id_state_init(unit, di->tile_count,
                                                   tile_id));
        } else {
            /*
             * Update the old tile count, this info is still required
             * incase the tile LT.LOOKUP has changed in the new version.
             */
            di->issu_old_tile_count = tile_id->array_size;
        }
    }
    if (ha_alloc_sz < in_req_sz) {
        if (warm && bcmissu_is_active()
            && tile_id->array_size < di->tile_count) {
            /*
             * Array size must be valid and non-zero if this HA block was valid
             * and in-use previously.
             */
            if (tile_id->array_size == 0) {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
            /* Reallocate memory based on the new memory requirements. */
            tile_id = shr_ha_mem_realloc(unit, tile_id, in_req_sz);
            SHR_NULL_CHECK(tile_id, SHR_E_MEMORY);
            /* Initialize new elements of the array. */
            for (t = tile_id->array_size; t < di->tile_count; t++) {
                tile_id->array[t] = di->var->sbr_tile_info[t].tile_id;
            }
            di->issu_tile_add = TRUE;
            /* Save the old tile count for reference to rebuild the state. */
            di->issu_old_tile_count = tile_id->array_size;
            /* Update the new array size. */
            tile_id->array_size = di->tile_count;
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "tile_add: issu_tile_add=%u "
                                  "issu_old_tile_count=%u "
                                  "tile_id->array_size=%u\n"),
                       di->issu_tile_add, di->issu_old_tile_count,
                       tile_id->array_size));
        } else {
            SHR_IF_ERR_MSG_EXIT(SHR_E_MEMORY,
                                (BSL_META_U(unit,
                                            "HA blk req_sz=%u alloc_sz=%u.\n"),
                                 in_req_sz, ha_alloc_sz));
        }
    }
    if (warm && tile_id->array_size > di->tile_count) {
        if (bcmissu_is_active() && ha_alloc_sz > in_req_sz) {
            /*
             * Some tiles have been deleted in new SDK version, review and
             * rebuild the HA state.
             */
            di->issu_tile_del = TRUE;
            di->issu_old_tile_count = tile_id->array_size;
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "tile_del: issu_tile_del=%u "
                                  "issu_old_tile_count=%u "),
                       di->issu_tile_del, di->issu_old_tile_count));
        } else {
            /* Regular warm-boot should not result in tiles being deleted. */
            SHR_IF_ERR_MSG_EXIT(SHR_E_MEMORY,
                                (BSL_META_U(unit,
                                            "HA blk req_sz=%u alloc_sz=%u.\n"),
                                 in_req_sz, ha_alloc_sz));
        }
    }
    if (!warm) {
        /* Report HA block info to ISSU during regular cold-boot switch init. */
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_UFT_COMP_ID,
                                        submod_id, 0, in_req_sz, 1,
                                        BCMPTM_SBR_VAR_TILE_ID_STATE_T_ID));
        /*
         * Store the tile details for this SDK version in HA for use during
         * ISSU.
         */
        SHR_IF_ERR_EXIT
            (bcmptm_sbr_var_tile_id_state_init(unit, di->tile_count, tile_id));
    }
    /* Assign to function out parameters. */
    *alloc_ptr = tile_id;
    *out_ha_alloc_sz = ha_alloc_sz;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Store Tile LT.LOOKUP details specific to this variant driver in HA.
 */
static int
bcmptm_sbr_var_tile_lt_lookup_state_init(
                                int unit,
                                uint16_t lt_lookup_count,
                                bcmptm_sbr_var_tile_ltlkup_state_t *ltlkup_arr)
{
    uint32_t e = 0, l;
    uint16_t t = 0;
    sbr_dev_info_t *di = NULL;
    const bcmptm_sbr_tile_info_t *v_ti = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ltlkup_arr, SHR_E_PARAM);

    di = bcmptm_sbr_device_info_get(unit);
    SHR_NULL_CHECK(di, SHR_E_INIT);

    /*
     * Save per Tile LT.LOOKUP0 and LT.LOOKUP1 LTID values for use during
     * ISSU.
     */
    for (t = 0, e = 0; t < di->tile_count && e < lt_lookup_count; t++) {
        v_ti = di->var->sbr_tile_info + t;
        if (v_ti->sbr_lt_lookup0_info_count) {
            SHR_NULL_CHECK(v_ti->sbr_lt_lookup0_info, SHR_E_INTERNAL);
        }
        if (v_ti->sbr_lt_lookup1_info_count) {
            SHR_NULL_CHECK(v_ti->sbr_lt_lookup1_info, SHR_E_INTERNAL);
        }
        for (l = 0; l < v_ti->sbr_lt_lookup0_info_count; l++) {
            ltlkup_arr->array[e].ltid = v_ti->sbr_lt_lookup0_info[l].ltid;
            ltlkup_arr->array[e++].lookup_type
                                    = v_ti->sbr_lt_lookup0_info[l].lookup_type;
        }
        for (l = 0; l < v_ti->sbr_lt_lookup1_info_count; l++) {
            ltlkup_arr->array[e].ltid = v_ti->sbr_lt_lookup1_info[l].ltid;
            ltlkup_arr->array[e++].lookup_type
                                    = v_ti->sbr_lt_lookup1_info[l].lookup_type;
        }
    }
    /*
     * Verify all the LT.LOOKUP entries have been stored and update the
     * array_size value.
     */
    if (e == lt_lookup_count) {
        ltlkup_arr->array_size = lt_lookup_count;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Device variant tiles LT.LOOKUP state HA block alloc function.
 */
static int
sbr_var_tile_ltlkup_state_ha_alloc(int unit,
                                   bool warm,
                                   sbr_dev_info_t *di,
                                   uint8_t submod_id,
                                   bcmptm_sbr_var_tile_ltlkup_state_t **p_alloc,
                                   uint32_t *out_ha_alloc_sz)
{
    bcmptm_sbr_var_tile_ltlkup_state_t *lt_lookup = NULL;
    uint32_t ha_alloc_sz = 0;
    uint32_t in_req_sz = 0;
    uint32_t offset = 0;
    uint16_t l = 0; /* LT.LOOKUP index. */
    int rv = SHR_E_INTERNAL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(p_alloc, SHR_E_PARAM);
    SHR_NULL_CHECK(out_ha_alloc_sz, SHR_E_PARAM);
    SHR_NULL_CHECK(di, SHR_E_INIT);
    SHR_NULL_CHECK(di->var, SHR_E_INIT);
    SHR_NULL_CHECK(di->var->sbr_tile_info, SHR_E_INIT);

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                         "\"warm = %s\", lt_lookup_count=%u.\n"),
               warm ? "TRUE" : "FALSE", di->lt_lkup_count));

    /*
     * Calculate variant tiles LT.LOOKUP state HA block size for all tiles in
     * this version.
     */
    in_req_sz = (sizeof(bcmptm_sbr_var_tile_ltlkup_state_t)
                 + sizeof(bcmptm_sbr_var_tile_ltlkup_t) * di->lt_lkup_count);
    /*
     * shr_ha_mem_alloc() function will update ha_alloc_sz variable with the
     * actual size of HA block allocated.
     */
    ha_alloc_sz = in_req_sz;
    lt_lookup = shr_ha_mem_alloc(unit, BCMMGMT_UFT_COMP_ID, submod_id,
                                 "ptmUftSbrVarTileLtLkupStateHaBlk",
                                 &ha_alloc_sz);
    SHR_NULL_CHECK(lt_lookup, SHR_E_MEMORY);
    if (bcmissu_is_active() && warm) {
        /*
         * If ISSU is active, check if this HA block has been previously
         * reported to ISSU.
         */
        rv = bcmissu_struct_info_get(unit,
                                     BCMPTM_SBR_VAR_TILE_LTLKUP_STATE_T_ID,
                                     BCMMGMT_UFT_COMP_ID,
                                     submod_id,
                                     &offset);
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "BCMPTM_SBR_VAR_TILE_LTLKUP_STATE_T_ID: "
                              "bcmissu_struct_info_get(rv=%s) offset=%u "
                              "arr_sz=%u ltlkup_cnt=%u\n"),
                   shr_errmsg(rv), offset, lt_lookup->array_size,
                   di->lt_lkup_count));
        if (rv == SHR_E_NOT_FOUND) {
            /*
             * If E_NOT_FOUND is returned, then this block did not exist in the
             * older version, report the new block structure details to ISSU.
             */
            SHR_IF_ERR_EXIT
                (bcmissu_struct_info_report(unit, BCMMGMT_UFT_COMP_ID,
                                        submod_id, 0, in_req_sz, 1,
                                        BCMPTM_SBR_VAR_TILE_LTLKUP_STATE_T_ID));
            /*
             * Store the tile LT.LOOKUP details for this SDK version/variant in
             * HA, for use during ISSU.
             */
            SHR_IF_ERR_EXIT
                (bcmptm_sbr_var_tile_lt_lookup_state_init(unit,
                                                          di->lt_lkup_count,
                                                          lt_lookup));
        } else {
            di->issu_old_lt_lkup_count = lt_lookup->array_size;
        }
    }
    if (ha_alloc_sz < in_req_sz) {
        if (warm && bcmissu_is_active()
            && lt_lookup->array_size < di->lt_lkup_count) {
            /*
             * Array size must be valid and non-zero if this HA block was valid
             * and in-use previously.
             */
            if (lt_lookup->array_size == 0) {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
            /* Reallocate memory based on the new memory requirements. */
            lt_lookup = shr_ha_mem_realloc(unit, lt_lookup, in_req_sz);
            SHR_NULL_CHECK(lt_lookup, SHR_E_MEMORY);
            for (l = lt_lookup->array_size; l < di->lt_lkup_count; l++) {
                lt_lookup->array[l].ltid = di->invalid_ltid;
                lt_lookup->array[l].lookup_type = BCMPTM_SBR_LOOKUP_INVALID;
            }
            /*
             * New tiles have been added, review and rebuild tile LT.LOOKUP
             * state.
             */
            di->issu_tile_lkup_add = TRUE;
            /* Save the old tile count for reference to rebuild the state. */
            di->issu_old_lt_lkup_count = lt_lookup->array_size;
            /* Update the new array size. */
            lt_lookup->array_size = di->lt_lkup_count;
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "ltlkup_add: issu_tile_lkup_add=%u "
                                  "issu_old_lt_lkup_count=%u "
                                  "lt_lookup->array_size=%u\n"),
                       di->issu_tile_lkup_add, di->issu_old_lt_lkup_count,
                       lt_lookup->array_size));
        } else {
            SHR_IF_ERR_MSG_EXIT(SHR_E_MEMORY,
                                (BSL_META_U(unit,
                                            "HA blk req_sz=%u alloc_sz=%u.\n"),
                                 in_req_sz, ha_alloc_sz));
        }
    }
    if (warm && lt_lookup->array_size > di->lt_lkup_count) {
        if (bcmissu_is_active() && ha_alloc_sz > in_req_sz) {
            /*
             * Some tile LT.LOOKUPs have been deleted in new SDK version,
             * review and rebuild the HA state.
             */
            di->issu_tile_lkup_del = TRUE;
            di->issu_old_lt_lkup_count = lt_lookup->array_size;
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "ltlkup_add: issu_tile_lkup_add=%u "
                                  "issu_old_lt_lkup_count=%u\n"),
                       di->issu_tile_lkup_add, di->issu_old_lt_lkup_count));
        } else {
            /*
             * Regular warm-boot should not result in tile LT.LOOKUPs being
             * deleted.
             */
            SHR_IF_ERR_MSG_EXIT(SHR_E_MEMORY,
                                (BSL_META_U(unit,
                                            "HA blk req_sz=%u alloc_sz=%u.\n"),
                                 in_req_sz, ha_alloc_sz));
        }
    }
    if (!warm) {
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_UFT_COMP_ID,
                                        submod_id, 0, in_req_sz, 1,
                                        BCMPTM_SBR_VAR_TILE_LTLKUP_STATE_T_ID));
        /*
         * Store the tile LT.LOOKUP details for this SDK version/variant in
         * HA, for use during ISSU.
         */
        SHR_IF_ERR_EXIT
            (bcmptm_sbr_var_tile_lt_lookup_state_init(unit,
                                                      di->lt_lkup_count,
                                                      lt_lookup));
    }
    *p_alloc = lt_lookup;
    *out_ha_alloc_sz = ha_alloc_sz;
exit:
    SHR_FUNC_EXIT();
}

static int
sbr_issu_spt_state_update(int unit, sbr_dev_info_t *di)
{
    bcmptm_sbr_pt_state_t *issu_spt = NULL;
    bcmptm_sbr_entry_state_t *issu_spt_estate = NULL;
    bcmptm_sbr_pt_state_t *pt_ha = NULL;
    bcmptm_sbr_lt_lookup_state_tile_t *ltlkup_ha = NULL;
    size_t alloc_sz; /* Malloc size. */
    uint16_t pt = 0, eoffset = 0, new_eidx = 0;
    uint16_t e, i, l;
    uint16_t spt_index = SBR_UINT16_INVALID;
    uint16_t spt_match = 0;
    int rv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(di, SHR_E_INIT);
    SHR_NULL_CHECK(di->spt_estate_ha, SHR_E_INIT);
    SHR_NULL_CHECK(di->spt_state_ha, SHR_E_INIT);
    SHR_NULL_CHECK(di->tile_lkup_state_ha, SHR_E_INIT);
    SHR_NULL_CHECK(di->var, SHR_E_INIT);
    SHR_NULL_CHECK(di->var->sbr_profile_table, SHR_E_INIT);

    /* Allocate SPT state array memory. */
    SHR_ALLOC(issu_spt, di->spt_state_blk_sz, "bcmptmSbrIssuPtStateArr");
    SHR_NULL_CHECK(issu_spt, SHR_E_MEMORY);
    sal_memcpy(issu_spt, di->spt_state, di->spt_state_blk_sz);

    /* SBR table entries memory. */
    SHR_ALLOC(issu_spt_estate, di->spt_estate_blk_sz,
              "bcmptmSbrIssuPtEntStateArr");
    SHR_NULL_CHECK(issu_spt_estate, SHR_E_MEMORY);
    sal_memcpy(issu_spt_estate, di->spt_estate, di->spt_estate_blk_sz);

    for (pt = 0; pt < di->spt_count; pt++) {
        rv = bcmptm_sbr_find_spt_by_spt_id(unit, di,
                                           di->var->sbr_profile_table[pt],
                                           &spt_index);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
        if (SHR_FAILURE(rv)) {
            continue;
        }
        spt_match++;
        pt_ha = &di->spt_state_ha[spt_index];
        /*
         * SBR profile table entries count cannot change between SDK versions.
         */
        if (issu_spt[pt].entries_count != pt_ha->entries_count) {
            SHR_IF_ERR_MSG_EXIT
                (SHR_E_INIT, (BSL_META_U(unit,
                                         "SPT(%s): Physical table entries "
                                         "count mismatch - exp/act=%u/%u\n"),
                              bcmdrd_pt_sid_to_name(unit, pt_ha->drd_sid),
                              pt_ha->entries_count,
                              issu_spt[pt].entries_count));
        }
        issu_spt[pt].ext_drd_sid = pt_ha->ext_drd_sid;
        issu_spt[pt].free_entries_count = pt_ha->free_entries_count;
        /* Copy the SBR profile table entries state. */
        for (i = 0, e = pt_ha->estate_base_index;
             e < (pt_ha->estate_base_index + pt_ha->entries_count);
             e++, i++) {
            issu_spt_estate[i] = di->spt_estate_ha[e];
        }
        /*
         * If the SBR profile table position has changed in the new version,
         * then update SPT entry_index values for in_use LT.LOOKUP entries in
         * tile_lkup_state_ha[] array.
         */
        if (pt != spt_index) {
            for (l = 0; l < di->issu_old_lt_lkup_count; l++) {
                ltlkup_ha = &di->tile_lkup_state_ha[l];
                if (ltlkup_ha->in_use && ltlkup_ha->spt_sid == pt_ha->drd_sid) {
                    /*
                     * Determine the new shared entry index for this entry
                     * based on the new base index of this entry.
                     */
                    eoffset = ltlkup_ha->entry_index - pt_ha->estate_base_index;
                    new_eidx = (issu_spt[pt].estate_base_index
                                 + eoffset);
                    ltlkup_ha->entry_index = new_eidx;
                }
            }
        }
    }
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "match_stat: spt_count=%u.\n"),
               spt_match));
    /* Clear the old state. */
    alloc_sz = sizeof(*di->spt_state_ha) * di->issu_old_spt_count;
    sal_memset(di->spt_state_ha, 0, alloc_sz);
    /* Copy new SPT HA state. */
    sal_memcpy(di->spt_state_ha, issu_spt, di->spt_state_blk_sz);

    /* Clear the old state*/
    alloc_sz = sizeof(*di->spt_estate_ha) * di->issu_old_spt_ecount;
    sal_memset(di->spt_estate_ha, 0, alloc_sz);
    /* Copy new SBR profile tables entries HA state. */
    sal_memcpy(di->spt_estate_ha, issu_spt_estate, di->spt_estate_blk_sz);
exit:
    SHR_FREE(issu_spt);
    SHR_FREE(issu_spt_estate);
    SHR_FUNC_EXIT();
}

static int
sbr_issu_tile_state_update(int unit, sbr_dev_info_t *di)
{
    bcmptm_sbr_lt_lookup_state_tile_t *issu_tltlkup = NULL;
    bcmptm_sbr_tile_state_t *issu_tile = NULL;
    bcmptm_sbr_lt_lookup_state_tile_t *ltlkup = NULL;
    bcmptm_sbr_tile_state_t *tstate_ha = NULL;
    const bcmptm_sbr_tile_info_t *v_ti = NULL;
    const bcmptm_sbr_lt_lookup_info_tile_t *v_tltlkup = NULL;
    size_t alloc_sz; /* Malloc size. */
    uint16_t t, tidx = SBR_UINT16_INVALID, l;
    uint16_t ltlkup_idx = SBR_UINT16_INVALID;
    uint16_t v_ltlkup_idx = SBR_UINT16_INVALID;
    uint16_t tile_match = 0;
    uint16_t lt_lkup0_match = 0, lt_lkup1_match = 0;
    int rv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(di, SHR_E_INIT);
    SHR_NULL_CHECK(di->tile_state_ha, SHR_E_INIT);
    SHR_NULL_CHECK(di->tile_lkup_state_ha, SHR_E_INIT);
    SHR_NULL_CHECK(di->v_tileid_state_ha, SHR_E_INIT);
    SHR_NULL_CHECK(di->v_tltlkup_state_ha, SHR_E_INIT);
    SHR_NULL_CHECK(di->var, SHR_E_INIT);

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "TS_UPDT: tile-add=%u/del=%u ltlkup-add=%u/del=%u "
                          "tile_count(o/n)=%u/%u tltkup_cnt(o/n)=%u/%u\n"),
               di->issu_tile_add, di->issu_tile_del,
               di->issu_tile_lkup_add, di->issu_tile_lkup_del,
               di->issu_old_tile_count, di->tile_count,
               di->issu_old_lt_lkup_count, di->lt_lkup_count));

    tstate_ha = di->tile_state_ha;

    /* Allocate memory to re-build the tiles state for tiles in new version. */
    SHR_ALLOC(issu_tile, di->tile_state_blk_sz,
              "bcmptmSbrIssuTileStateArr");
    SHR_NULL_CHECK(issu_tile, SHR_E_MEMORY);
    sal_memcpy(issu_tile, di->tile_state, di->tile_state_blk_sz);

    /*
     * Allocate memory to re-build the tiles lt.lookup state for tiles in new
     * version.
     */
    SHR_ALLOC(issu_tltlkup, di->tile_lkup_state_blk_sz,
              "bcmptmSbrIssuTileLtLkupStateArr");
    SHR_NULL_CHECK(issu_tltlkup, SHR_E_MEMORY);
    sal_memcpy(issu_tltlkup, di->tile_lkup_state,
               di->tile_lkup_state_blk_sz);

    /*
     * For the tiles in the new version, search for their state in the
     * tile_state_ha and tile_ltkup_state_ha blocks, if the state exists, then
     * copy it.
     */
    for (t = 0; t < di->tile_count; t++) {
        v_ti = &di->var->sbr_tile_info[t];
        tidx = SBR_UINT16_INVALID;
        rv = bcmptm_sbr_find_tile_by_tile_id(unit, di, v_ti->tile_id, &tidx);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
        if (SHR_FAILURE(rv)) {
            /*
             * This is a new tile in this new version, initialize its tile_mode
             * from variant driver tile_mode data provided by tooling.
             */
            if (v_ti->initial_tile_mode == di->var->tile_mode_disable) {
                issu_tile[t].tile_mode = di->var->tile_mode_disable;
            } else {
                issu_tile[t].tile_mode = v_ti->tile_mode[0];
            }
            /* SHR_E_NOT_FOUND. */
            continue;
        }
        /* A tile_id match was found in the old version. */
        tile_match++;
        /* Copy the tile mode. */
        issu_tile[t].tile_mode = tstate_ha[t].tile_mode;
        /*
         * For each tile LT.LOOKUP in the new SDK version for the current tile,
         * search the tile LT.LOOKUP state in HA block, if a match is found and
         * the LT.LOOKUP is in use, then copy the state.
         */
        for (l = 0; l < v_ti->sbr_lt_lookup0_info_count; l++) {
            ltlkup_idx = SBR_UINT16_INVALID;
            v_tltlkup = &v_ti->sbr_lt_lookup0_info[l];
            rv = bcmptm_sbr_find_tile_ltlkup_by_lkupltid(unit,
                                                         di,
                                                         tidx,
                                                         v_tltlkup->ltid,
                                                         v_tltlkup->lookup_type,
                                                         &ltlkup_idx);
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
            if (SHR_FAILURE(rv)) {
                /* SHR_E_NOT_FOUND. */
                continue;
            }
            lt_lkup0_match++;
            ltlkup = &di->tile_lkup_state_ha[ltlkup_idx];
            v_ltlkup_idx = (issu_tile[t].lt_lookup0_base_index + l);
            issu_tltlkup[v_ltlkup_idx].in_use = ltlkup->in_use;
            issu_tltlkup[v_ltlkup_idx].entry_index = ltlkup->entry_index;
            issu_tltlkup[v_ltlkup_idx].spt_sid = ltlkup->spt_sid;
        }
        /* Search for LT.LOOKUP1 match and update the state. */
        for (l = 0; l < v_ti->sbr_lt_lookup1_info_count; l++) {
            ltlkup_idx = SBR_UINT16_INVALID;
            v_tltlkup = &v_ti->sbr_lt_lookup1_info[l];
            rv = bcmptm_sbr_find_tile_ltlkup_by_lkupltid(unit,
                                                         di,
                                                         tidx,
                                                         v_tltlkup->ltid,
                                                         v_tltlkup->lookup_type,
                                                         &ltlkup_idx);
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
            if (SHR_FAILURE(rv)) {
                /* SHR_E_NOT_FOUND. */
                continue;
            }
            lt_lkup1_match++;
            ltlkup = &di->tile_lkup_state_ha[ltlkup_idx];
            v_ltlkup_idx = (issu_tile[t].lt_lookup1_base_index + l);
            issu_tltlkup[v_ltlkup_idx].in_use = ltlkup->in_use;
            issu_tltlkup[v_ltlkup_idx].entry_index = ltlkup->entry_index;
            issu_tltlkup[v_ltlkup_idx].spt_sid = ltlkup->spt_sid;
        }
    }
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "match_stat: tiles=%u lt_lookup0=%u lt_lookup1=%u "
                          "total:lt_lookup_cnt=%u\n"),
               tile_match, lt_lkup0_match, lt_lkup1_match,
               (lt_lkup0_match + lt_lkup1_match)));

    /* Clear the old state. */
    alloc_sz = sizeof(*di->tile_state_ha) * di->issu_old_tile_count;
    sal_memset(di->tile_state_ha, 0, alloc_sz);
    /* Copy the new Tile HA state. */
    sal_memcpy(di->tile_state_ha, issu_tile, di->tile_state_blk_sz);

    /* Clear the old state*/
    alloc_sz = sizeof(*di->tile_lkup_state_ha) * di->issu_old_lt_lkup_count;
    sal_memset(di->tile_lkup_state_ha, 0, alloc_sz);
    /* Copy the new Tile LT.LOOKUP HA state. */
    sal_memcpy(di->tile_lkup_state_ha, issu_tltlkup,
               di->tile_lkup_state_blk_sz);
exit:
    SHR_FREE(issu_tile);
    SHR_FREE(issu_tltlkup);
    SHR_FUNC_EXIT();
}
/*******************************************************************************
 * Public Functions
 */

int
bcmptm_sbr_dump_sw_state(int unit,
                         shr_pb_t *pb)
{
    uint16_t pt, e, tile; /* Index variables. */
    uint32_t l; /* LT lookup state entry index. */
    bcmptm_sbr_pt_state_t *spt_state = NULL;
    bcmptm_sbr_entry_state_t *estate = NULL; /* SPT entry state pointer. */
    bcmptm_sbr_lt_lookup_state_tile_t *tlkup_state = NULL; /* Lkup state. */
    sbr_dev_info_t *di = NULL;
    const char *lt_name = NULL;
    const char *tm_name = NULL, *tile_name = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pb, SHR_E_PARAM);

    /* Get the SBR device info pointer for the given unit. */
    di = bcmptm_sbr_device_info_get(unit);
    SHR_NULL_CHECK(di, SHR_E_INIT);

    shr_pb_printf(pb, "SPT Entry Max Word Size:%-2u var:invalid_ltid=%u\n",
                      di->spt_entry_max_wsize, di->invalid_ltid);

    shr_pb_printf(pb, "SPT state - summary: (total=%u)\n"
                      "===================================\n",
                      di->spt_count);
    for (pt = 0; pt < di->spt_count; pt++) {
        spt_state = di->spt_state + pt;
        shr_pb_printf(pb, "%-2u.SPT=%-32s(%-5u)/EXTN=%-32s(%-5d) "
                          "tot_ecnt=%u free_ecnt=%u\n",
                      pt,
                      bcmdrd_pt_sid_to_name(unit, spt_state->drd_sid),
                      spt_state->drd_sid,
                      bcmdrd_pt_sid_to_name(unit, spt_state->ext_drd_sid),
                      (int)spt_state->ext_drd_sid,
                      spt_state->entries_count,
                      spt_state->free_entries_count);
        for (e = 0; e < spt_state->entries_count; e++) {
            estate = di->spt_estate + (spt_state->estate_base_index + e);
            if (estate->usage_mode != BCMPTM_SBR_ENTRY_FREE) {
                if (estate->ltid != BCMLTD_SID_INVALID
                    && estate->ltid != di->invalid_ltid) {
                    /* Transform the ltid to lt_name. */
                    SHR_IF_ERR_EXIT
                        (bcmlrd_field_value_to_symbol(unit,
                                                      TABLE_INFOt,
                                                      TABLE_INFOt_TABLE_IDf,
                                                      estate->ltid,
                                                      &lt_name));
                } else {
                    lt_name = "NULL";
                }
                shr_pb_printf(pb,
                              "   pt_eidx=%2u(sw_eidx=%-2u): rcnt=%u um=%-14s "
                              "ltid=%-40s(%-5u) lk_typ=%-10s "
                              "spt_tbl_idx(sw)=%u.\n",
                              e,
                              (spt_state->estate_base_index + e),
                              estate->ref_count,
                              bcmptm_sbr_ent_usage_str(estate->usage_mode),
                              lt_name,
                              (estate->ltid != BCMLTD_SID_INVALID
                               && estate->ltid != di->invalid_ltid)
                                    ? estate->ltid : 0,
                              bcmptm_sbr_lt_lkup_str(estate->lookup_type),
                              estate->pt_state_index);
            }
        }
    }
    shr_pb_printf(pb,
                  "Tile + TileMode(TM) lt_lookup0/1 state - summary:"
                  "(total=%u)\n"
                  "================================================="
                  "============\n", di->tile_count);
    for (tile = 0; tile < di->tile_count; tile++) {
        /* Get tile name. */
        SHR_IF_ERR_EXIT
            (bcmlrd_field_value_to_symbol(unit, DEVICE_EM_TILE_INFOt,
                                        DEVICE_EM_TILE_INFOt_DEVICE_EM_TILE_IDf,
                                        tile, &tile_name));
        SHR_IF_ERR_EXIT
            (bcmlrd_field_value_to_symbol(unit,
                                          DEVICE_EM_TILE_INFOt,
                                          DEVICE_EM_TILE_INFOt_MODEf,
                                          di->tile_state[tile].tile_mode,
                                          &tm_name));
        shr_pb_printf(pb,
                      "[%2u]: Tile=%-16s TM=%-22s(%-3u) "
                      "lkup0:{base=%-4d count=%-4u} lkup1:{base=%-4d "
                      "count=%-4u}\n",
                      tile, tile_name, tm_name,
                      di->tile_state[tile].tile_mode,
                      (int)di->tile_state[tile].lt_lookup0_base_index,
                      di->tile_state[tile].lt_lookup0_count,
                      (int)di->tile_state[tile].lt_lookup1_base_index,
                      di->tile_state[tile].lt_lookup1_count);
    }

    shr_pb_printf(pb,
                  "Tile lt.lookup index to SPT entry map state "
                  "- summary: (total=%u)\n"
                  "==========================================="
                  "=======================\n", di->lt_lkup_count);
    for (l = 0; l < di->lt_lkup_count; l++) {
        tlkup_state = di->tile_lkup_state + l;
        if (tlkup_state->in_use) {
            estate = di->spt_estate + tlkup_state->entry_index;
            /* Transform the ltid to lt_name. */
            if (estate->ltid != BCMLTD_SID_INVALID
                && estate->ltid != di->invalid_ltid) {
                SHR_IF_ERR_EXIT
                    (bcmlrd_field_value_to_symbol(unit,
                                                  TABLE_INFOt,
                                                  TABLE_INFOt_TABLE_IDf,
                                                  estate->ltid,
                                                  &lt_name));
            } else {
                lt_name = "NULL";
            }
            shr_pb_printf(pb,
                          "lkup_idx[%4u]: SPT=%-32s(%-5u) spt_idx(sw)=%u "
                          "spt_estate_idx=%-2u: (r=%u um=%s ltid=%s "
                          "lk_typ=%s)\n",
                          l,
                          bcmdrd_pt_sid_to_name(unit, tlkup_state->spt_sid),
                          tlkup_state->spt_sid,
                          estate->pt_state_index,
                          tlkup_state->entry_index,
                          estate->ref_count,
                          bcmptm_sbr_ent_usage_str(estate->usage_mode),
                          lt_name,
                          bcmptm_sbr_lt_lkup_str(estate->lookup_type));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_sbr_dump_ha_sw_state(int unit,
                            shr_pb_t *pb)
{
    uint16_t pt, e, tile; /* Index variables. */
    uint32_t l; /* LT lookup state entry index. */
    bcmptm_sbr_pt_state_t *spt_state = NULL;
    bcmptm_sbr_entry_state_t *estate = NULL; /* SPT entry state pointer. */
    bcmptm_sbr_lt_lookup_state_tile_t *tlkup_state = NULL; /* Lkup state. */
    sbr_dev_info_t *di = NULL;
    const char *lt_name = NULL;
    const char *tm_name = NULL, *tile_name = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pb, SHR_E_PARAM);

    /* Get the SBR device info pointer for the given unit. */
    di = bcmptm_sbr_device_info_get(unit);
    SHR_NULL_CHECK(di, SHR_E_INIT);
    SHR_NULL_CHECK(di->spt_state_ha, SHR_E_INTERNAL);
    SHR_NULL_CHECK(di->spt_estate_ha, SHR_E_INTERNAL);
    SHR_NULL_CHECK(di->tile_state_ha, SHR_E_INTERNAL);
    SHR_NULL_CHECK(di->tile_lkup_state_ha, SHR_E_INTERNAL);
    SHR_NULL_CHECK(di->v_tileid_state_ha, SHR_E_INTERNAL);
    SHR_NULL_CHECK(di->v_tltlkup_state_ha, SHR_E_INTERNAL);

    shr_pb_printf(pb, "HA: SPT state - summary: (total=%u)\n"
                      "=======================================\n",
                      di->spt_count);
    for (pt = 0; pt < di->spt_count; pt++) {
        spt_state = di->spt_state_ha + pt;
        shr_pb_printf(pb, "%-2u.SPT=%-32s(%-5u)/EXTN=%-32s(%-5d) "
                          "tot_ecnt=%u free_ecnt=%u\n",
                      pt,
                      bcmdrd_pt_sid_to_name(unit, spt_state->drd_sid),
                      spt_state->drd_sid,
                      bcmdrd_pt_sid_to_name(unit, spt_state->ext_drd_sid),
                      (int)spt_state->ext_drd_sid,
                      spt_state->entries_count,
                      spt_state->free_entries_count);
        for (e = 0; e < spt_state->entries_count; e++) {
            estate = di->spt_estate_ha + (spt_state->estate_base_index + e);
            if (estate->usage_mode != BCMPTM_SBR_ENTRY_FREE) {
                /* Transform the ltid to lt_name. */
                if (estate->ltid != BCMLTD_SID_INVALID
                    && estate->ltid != di->invalid_ltid) {
                    SHR_IF_ERR_EXIT
                        (bcmlrd_field_value_to_symbol(unit,
                                                      TABLE_INFOt,
                                                      TABLE_INFOt_TABLE_IDf,
                                                      estate->ltid,
                                                      &lt_name));
                } else {
                    lt_name = "NULL";
                }
                shr_pb_printf(pb, "   pt_eidx=%2u(sw_eidx=%-2u): rcnt=%u "
                              "um=%-14s ltid=%-40s(%-5u) lk_typ=%-10s "
                              "spt_tbl_idx(sw)=%u.\n",
                              e,
                              (spt_state->estate_base_index + e),
                              estate->ref_count,
                              bcmptm_sbr_ent_usage_str(estate->usage_mode),
                              lt_name,
                              (estate->ltid != BCMLTD_SID_INVALID
                               && estate->ltid != di->invalid_ltid)
                                    ? estate->ltid : 0,
                              bcmptm_sbr_lt_lkup_str(estate->lookup_type),
                              estate->pt_state_index);
            }
        }
    }
    shr_pb_printf(pb,
                  "HA: Tile + TileMode(TM) lt_lookup0/1 state - summary:"
                  "(total=%u)\n"
                  "====================================================="
                  "=============\n", di->tile_count);
    for (tile = 0; tile < di->tile_count; tile++) {
        /* Get tile name. */
        SHR_IF_ERR_EXIT
            (bcmlrd_field_value_to_symbol(unit, DEVICE_EM_TILE_INFOt,
                                        DEVICE_EM_TILE_INFOt_DEVICE_EM_TILE_IDf,
                                        di->v_tileid_state_ha->array[tile],
                                        &tile_name));
        SHR_IF_ERR_EXIT
            (bcmlrd_field_value_to_symbol(unit,
                                          DEVICE_EM_TILE_INFOt,
                                          DEVICE_EM_TILE_INFOt_MODEf,
                                          di->tile_state_ha[tile].tile_mode,
                                          &tm_name));
        shr_pb_printf(pb,
                      "[%2u]: Tile=%-16s TM=%-22s(%-3u) "
                      "lkup0:{base=%-4d count=%-4u} lkup1:{base=%-4d "
                      "count=%-4u}\n",
                      tile, tile_name, tm_name,
                      di->tile_state_ha[tile].tile_mode,
                      (int)di->tile_state_ha[tile].lt_lookup0_base_index,
                      di->tile_state_ha[tile].lt_lookup0_count,
                      (int)di->tile_state_ha[tile].lt_lookup1_base_index,
                      di->tile_state_ha[tile].lt_lookup1_count);
    }
    shr_pb_printf(pb,
                  "HA: Tile lt.lookup index to SPT entry map state "
                  "- summary: (total=%u)\n"
                  "================================================"
                  "=======================\n", di->lt_lkup_count);
    for (l = 0; l < di->lt_lkup_count; l++) {
        tlkup_state = di->tile_lkup_state_ha + l;
        if (tlkup_state->in_use) {
            estate = di->spt_estate_ha + tlkup_state->entry_index;
            /* Transform the ltid to lt_name. */
            if (estate->ltid != BCMLTD_SID_INVALID
                && estate->ltid != di->invalid_ltid) {
                SHR_IF_ERR_EXIT
                    (bcmlrd_field_value_to_symbol(unit,
                                                  TABLE_INFOt,
                                                  TABLE_INFOt_TABLE_IDf,
                                                  estate->ltid,
                                                  &lt_name));
            } else {
                lt_name = "NULL";
            }
            shr_pb_printf(pb,
                          "lkup_idx[%4u]: SPT=%-32s(%-5u) spt_idx(sw)=%u "
                          "spt_estate_idx=%-2u: (r=%u um=%s ltid=%s "
                          "lk_typ=%s)\n",
                          l,
                          bcmdrd_pt_sid_to_name(unit, tlkup_state->spt_sid),
                          tlkup_state->spt_sid,
                          estate->pt_state_index,
                          tlkup_state->entry_index,
                          estate->ref_count,
                          bcmptm_sbr_ent_usage_str(estate->usage_mode),
                          lt_name,
                          bcmptm_sbr_lt_lkup_str(estate->lookup_type));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_sbr_tile_mode_commit(int unit)
{
    sbr_dev_info_t *di = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get the SBR device info pointer for the given unit. */
    di = bcmptm_sbr_device_info_get(unit);
    SHR_NULL_CHECK(di, SHR_E_INTERNAL);

    /* Copy current SBR profile tables state to HA. */
    sal_memcpy(di->spt_state_ha, di->spt_state, di->spt_state_blk_sz);

    /* Copy current  SBR profile tables entries state to HA. */
    sal_memcpy(di->spt_estate_ha, di->spt_estate, di->spt_estate_blk_sz);

    /* Copy current Tiles state to HA. */
    sal_memcpy(di->tile_state_ha, di->tile_state, di->tile_state_blk_sz);

    /* Copy current Tiles LT.LOOKUP state to HA. */
    sal_memcpy(di->tile_lkup_state_ha, di->tile_lkup_state,
               di->tile_lkup_state_blk_sz);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Commit completed.\n")));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_sbr_tile_mode_abort(int unit)
{
    sbr_dev_info_t *di = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get the SBR device info pointer for the given unit. */
    di = bcmptm_sbr_device_info_get(unit);
    SHR_NULL_CHECK(di, SHR_E_INTERNAL);

    /* Copy HA SBR profile tables state to current. */
    sal_memcpy(di->spt_state, di->spt_state_ha, di->spt_state_blk_sz);

    /* Copy HA  SBR profile tables entries state to current. */
    sal_memcpy(di->spt_estate, di->spt_estate_ha, di->spt_estate_blk_sz);

    /* Copy HA Tiles state to current. */
    sal_memcpy(di->tile_state, di->tile_state_ha, di->tile_state_blk_sz);

    /* Copy HA Tiles LT.LOOKUP state to current. */
    sal_memcpy(di->tile_lkup_state, di->tile_lkup_state_ha,
               di->tile_lkup_state_blk_sz);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Abort completed.\n")));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_sbr_init(int unit, bool warm)
{
    int rv = SHR_E_NONE;
    sbr_dev_info_t *di = NULL;
    uint32_t spt_blk_sz = 0, spt_estate_blk_sz = 0;
    uint32_t tiles_blk_sz = 0, tiles_lt_lkup_blk_sz = 0;
    uint32_t var_tile_id_blk_sz = 0, var_tile_lt_lkup_blk_sz = 0;
    uint32_t v_spt_blk_sz = 0;
    uint16_t pt, t;
    bcmptm_sbr_pt_state_t *pt_state = NULL; /* SBR profile table state. */
    bcmptm_sbr_tile_state_t *ts = NULL; /* Tile state. */
    shr_pb_t *pb = NULL;

    SHR_FUNC_ENTER(unit);

    /* Initialize the unit SBR info data structures. */
    rv = bcmptm_sbr_device_init(unit, warm);
    if (rv == SHR_E_UNAVAIL) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Device SBR drv register fn ptr is NULL!.\n")));
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(rv);

    /* Get the SBR device info pointer for the given unit. */
    di = bcmptm_sbr_device_info_get(unit);
    SHR_NULL_CHECK(di, SHR_E_INIT);

    /*
     * Validate Tile and SBR profile tables current state pointers/buffers
     * post device initialization sequence, prior to accessing them in this
     * function.
     */
    SHR_NULL_CHECK(di->spt_state, SHR_E_INIT);
    SHR_NULL_CHECK(di->spt_estate, SHR_E_INIT);
    SHR_NULL_CHECK(di->tile_state, SHR_E_INIT);
    SHR_NULL_CHECK(di->tile_lkup_state, SHR_E_INIT);

    /* SBR HA initialization sequence list of function calls. */
    SHR_IF_ERR_EXIT
        (sbr_var_spt_state_ha_alloc(unit, warm, di,
                                    BCMPMT_HA_SUBID_UFT_SBR_VAR_SPT_STATE,
                                    &di->v_spt_state_ha,
                                    &v_spt_blk_sz));
    SHR_IF_ERR_EXIT
        (sbr_var_tileid_state_ha_alloc(unit, warm, di,
                                    BCMPMT_HA_SUBID_UFT_SBR_VAR_TILE_ID_STATE,
                                    &di->v_tileid_state_ha,
                                    &var_tile_id_blk_sz));
    SHR_IF_ERR_EXIT
        (sbr_var_tile_ltlkup_state_ha_alloc(unit, warm, di,
                            BCMPMT_HA_SUBID_UFT_SBR_VAR_TILE_LT_LOOKUP_STATE,
                            &di->v_tltlkup_state_ha,
                            &var_tile_lt_lkup_blk_sz));
    SHR_IF_ERR_EXIT
        (sbr_pt_state_ha_alloc(unit, warm, di,
                               BCMPMT_HA_SUBID_UFT_SBR_PT_STATE,
                               &di->spt_state_ha, &spt_blk_sz));
    SHR_IF_ERR_EXIT
        (sbr_pt_estate_ha_alloc(unit, warm, di,
                                BCMPMT_HA_SUBID_UFT_SBR_PT_ESTATE,
                                &di->spt_estate_ha, &spt_estate_blk_sz));
    SHR_IF_ERR_EXIT
        (sbr_tile_state_ha_alloc(unit, warm, di,
                                 BCMPMT_HA_SUBID_UFT_SBR_TILE_STATE,
                                 &di->tile_state_ha, &tiles_blk_sz));
    SHR_IF_ERR_EXIT
        (sbr_tile_lt_lookup_state_ha_alloc(unit, warm, di,
                                    BCMPMT_HA_SUBID_UFT_SBR_TILE_LT_LKUP_STATE,
                                    &di->tile_lkup_state_ha,
                                    &tiles_lt_lkup_blk_sz));
    /* SBR profile tables changed in new SDK version. */
    if ((di->issu_spt_del || di->issu_spt_add)
        && (di->issu_old_spt_count == di->spt_count)
        && bcmissu_is_active()) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_INIT,
                            (BSL_META_U(unit,
                                        "SPT(%s): Cannot rebuild HA"
                                        "state - spt_count=%u.\n"),
                             di->issu_spt_del ? "deleted" : "added",
                             di->spt_count));

    }
    /* Tiles changed in new SDK version. */
    if ((di->issu_tile_del || di->issu_tile_add)
        && (di->issu_old_tile_count == di->tile_count)
        && bcmissu_is_active()) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_INIT,
                            (BSL_META_U(unit,
                                        "TILES(%s): Cannot rebuild HA"
                                        "state - tile_count=%u.\n"),
                             di->issu_tile_del ? "deleted" : "added",
                             di->tile_count));

    }
    /* Tiles LT.LOOKUP changed in new SDK version. */
    if ((di->issu_tile_lkup_del || di->issu_tile_lkup_add)
        && (di->issu_old_lt_lkup_count == di->lt_lkup_count)
        && bcmissu_is_active()) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_INIT,
                            (BSL_META_U(unit,
                                        "Tiles_LT.LOOKUP(%s): Cannot rebuild "
                                        "HA state - lt_lkup_count=%u.\n"),
                             di->issu_tile_lkup_del ? "deleted" : "added",
                             di->lt_lkup_count));

    }
    if (warm && BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        pb = shr_pb_create();
        SHR_IF_ERR_EXIT
            (bcmptm_sbr_dump_ha_sw_state(unit, pb));
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "HA state before update: %s"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
        pb = NULL;
    }
    if ((di->issu_spt_del || di->issu_spt_add) && bcmissu_is_active()) {
        SHR_IF_ERR_EXIT
            (sbr_issu_spt_state_update(unit, di));
        di->issu_spt_del = FALSE;
        di->issu_spt_add = FALSE;
    }
    if ((di->issu_tile_del || di->issu_tile_add || di->issu_tile_lkup_add
        || di->issu_tile_lkup_del) && bcmissu_is_active()) {
        SHR_IF_ERR_EXIT
            (sbr_issu_tile_state_update(unit, di));
        di->issu_tile_del = FALSE;
        di->issu_tile_add = FALSE;
    }
    if (warm && BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        pb = shr_pb_create();
        SHR_IF_ERR_EXIT
            (bcmptm_sbr_dump_ha_sw_state(unit, pb));
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "HA state after update:%s"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
        pb = NULL;
    }
    if (!warm) {
        sal_memcpy(di->spt_state_ha, di->spt_state, di->spt_state_blk_sz);
        sal_memcpy(di->spt_estate_ha, di->spt_estate, di->spt_estate_blk_sz);
        sal_memcpy(di->tile_state_ha, di->tile_state, di->tile_state_blk_sz);
        sal_memcpy(di->tile_lkup_state_ha, di->tile_lkup_state,
                   di->tile_lkup_state_blk_sz);
    } else {
        /* Copy the SBR profile tables free entries count from HA. */
        for (pt = 0; pt < di->spt_count; pt++) {
            pt_state = di->spt_state + pt;
            if ((pt_state->estate_base_index
                    != di->spt_state_ha[pt].estate_base_index)
                || pt_state->drd_sid != di->spt_state_ha[pt].drd_sid
                || (pt_state->entries_count
                    != di->spt_state_ha[pt].entries_count)) {
                LOG_DEBUG(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "Exp/Act@spt=%-3u: base_idx=%u/%u "
                                     "DRD_SID=%u/%u ecount=%u/%u\n"),
                          pt,
                          di->spt_state_ha[pt].estate_base_index,
                          pt_state->estate_base_index,
                          di->spt_state_ha[pt].drd_sid,
                          pt_state->drd_sid,
                          di->spt_state_ha[pt].entries_count,
                          pt_state->entries_count));

                SHR_IF_ERR_MSG_EXIT(SHR_E_INIT,
                                    (BSL_META_U(unit,
                                                "SPT=%u state mismatch.\n"),
                                     pt));
            }
            pt_state->free_entries_count =
                                    di->spt_state_ha[pt].free_entries_count;
        }
        for (t = 0; t < di->tile_count; t++) {
            ts = di->tile_state + t;
            SHR_NULL_CHECK(ts, SHR_E_INIT);
            if ((ts->lt_lookup0_base_index
                        != di->tile_state_ha[t].lt_lookup0_base_index)
                || (ts->lt_lookup0_count
                    != di->tile_state_ha[t].lt_lookup0_count)
                || (ts->lt_lookup1_base_index
                    != di->tile_state_ha[t].lt_lookup1_base_index)
                || (ts->lt_lookup1_count
                    != di->tile_state_ha[t].lt_lookup1_count)) {
                LOG_DEBUG(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "Exp/Act@tile=%-3u: lkup0_base=%d/%d "
                                     "lkup0_cnt=%u/%u lkup1_base=%d/%d "
                                     "lkup0_cnt=%u/%u\n"),
                          t,
                          (int)di->tile_state_ha[t].lt_lookup0_base_index,
                          (int)ts->lt_lookup0_base_index,
                          di->tile_state_ha[t].lt_lookup0_count,
                          ts->lt_lookup0_count,
                          (int)di->tile_state_ha[t].lt_lookup1_base_index,
                          (int)ts->lt_lookup1_base_index,
                          di->tile_state_ha[t].lt_lookup1_count,
                          ts->lt_lookup1_count));
                SHR_IF_ERR_MSG_EXIT(SHR_E_INIT,
                                    (BSL_META_U(unit,
                                                "Tile=%u state mismatch.\n"),
                                     t));
            }
            di->tile_state[t].tile_mode = di->tile_state_ha[t].tile_mode;
        }
        sal_memcpy(di->spt_estate, di->spt_estate_ha, di->spt_estate_blk_sz);
        sal_memcpy(di->tile_lkup_state, di->tile_lkup_state_ha,
                   di->tile_lkup_state_blk_sz);
    }

    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        pb = shr_pb_create();
        SHR_IF_ERR_EXIT
            (bcmptm_sbr_dump_sw_state(unit, pb));
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "%s"), shr_pb_str(pb)));
    }
exit:
    if (SHR_FUNC_ERR()) {
        bcmptm_sbr_device_cleanup(unit);
    }
    shr_pb_destroy(pb);
    SHR_FUNC_EXIT();
}

int
bcmptm_sbr_cleanup(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);
    bcmptm_sbr_device_cleanup(unit);
    SHR_FUNC_EXIT();
}
