/*! \file bcmptm_rm_hash_state_mgmt.c
 *
 * Software State of The Resource Manager for Hash Table
 *
 * This file defines software objects that record software state of resource
 * manager of hash tables, and defines corresponding functions that get and set
 * these software objects.
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
#include <shr/shr_ha.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmissu/issu_api.h>
#include "bcmptm_rm_hash.h"
#include "bcmptm_rm_hash_state_mgmt.h"
#include "bcmptm_rm_hash_trans_mgmt.h"
#include "bcmptm_rm_hash_pt_register.h"
#include "bcmptm_rm_hash_utils.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMHASH

/*******************************************************************************
 * Typedefs
 */

/*
 *! \brief Size of the entry in the unit of base entry.
 *
 * This data structure describes the sizes of all the possible entries.
 */
typedef struct rm_hash_ent_size_map_s {
    /*! \brief Base entry bitmap is valid or not. */
    bool valid;

    /*! \brief Size of the entry. */
    uint8_t size;
} rm_hash_ent_size_map_t;

/*******************************************************************************
 * Private variables
 */
static const rm_hash_ent_size_map_t e_size_map [16] = {
    {FALSE, 0x00}, /* 0000 */
    {TRUE,  0x01}, /* 0001 */
    {TRUE,  0x01}, /* 0010 */
    {TRUE,  0x02}, /* 0011 */
    {TRUE,  0x01}, /* 0100 */
    {FALSE, 0x00}, /* 0101 */
    {FALSE, 0x00}, /* 0110 */
    {FALSE, 0x00}, /* 0111 */
    {TRUE,  0x01}, /* 1000 */
    {FALSE, 0x00}, /* 1001 */
    {FALSE, 0x00}, /* 1010 */
    {FALSE, 0x00}, /* 1011 */
    {TRUE,  0x02}, /* 1100 */
    {FALSE, 0x00}, /* 1101 */
    {FALSE, 0x00}, /* 1110 */
    {TRUE,  0x04}, /* 1111 */
};

/*! \brief Pointer to per unit HA block.
 *
 * The HA block records bucket state HA sub block id for each (SID, bank, pipe)
 * for the device unit.
 */
static rm_hash_bkt_state_blk_ctrl_t *bkt_blk_ctrl[BCMDRD_CONFIG_MAX_UNITS];

/*! \brief Pointer to per unit HA block.
 *
 * The HA block records entry node HA sub block id for each (SID, bank, pipe)
 * for the device unit.
 */
static rm_hash_ent_node_blk_ctrl_t *ent_blk_ctrl[BCMDRD_CONFIG_MAX_UNITS];

/*! \brief Pointer to per unit HA block.
 *
 * The HA block records slb info HA sub block id for each (SID, bank, pipe)
 * for the device unit.
 */
static rm_hash_slb_info_blk_ctrl_t *slb_blk_ctrl[BCMDRD_CONFIG_MAX_UNITS];

/*! \brief Pointer to per unit HA block.
 *
 * The HA block records nme bmp HA sub block id for each (SID, bank, pipe)
 * for the device unit.
 */
static rm_hash_nme_bmp_blk_ctrl_t *nme_bmp_blk_ctrl[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private Functions
 */
/*!
 * \brief Allocate per table bank bucket state.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] pipe Pipe instance.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_slb_state_blk_alloc(int unit,
                            rm_hash_pt_info_t *pt_info,
                            uint8_t rbank,
                            uint8_t pipe)
{
    uint32_t req_size, alloc_size, elem_cnt;
    uint16_t curr, blk_id, base_id;

    SHR_FUNC_ENTER(unit);

    /* Allocate bucket state for the bank. */
    base_id = (rbank * 3) * RM_HASH_MAX_TBL_INST_COUNT +
              BCMPTM_HA_SUBID_RMHASH_BLK_ID_FIRST;
    blk_id = base_id + pipe;

    elem_cnt = pt_info->rbank_num_slbs[rbank];
    req_size = elem_cnt * sizeof(rm_hash_slb_state_t);
    alloc_size = req_size;
    if (req_size > 0) {
        pt_info->bkt_state[rbank][pipe] = shr_ha_mem_alloc(unit,
                                                     BCMMGMT_RM_HASH_COMP_ID,
                                                     blk_id,
                                                     "bcmptmRmhashBktState",
                                                     &alloc_size);
        SHR_NULL_CHECK(pt_info->bkt_state[rbank][pipe], SHR_E_MEMORY);
        SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
        sal_memset(pt_info->bkt_state[rbank][pipe], 0, alloc_size);
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit,
                                        BCMMGMT_RM_HASH_COMP_ID,
                                        blk_id,
                                        0,
                                        sizeof(rm_hash_slb_state_t),
                                        elem_cnt,
                                        RM_HASH_SLB_STATE_T_ID));
        /* Record the mapping between (SID, bank) and block id. */
        curr = bkt_blk_ctrl[unit]->curr;
        bkt_blk_ctrl[unit]->array[curr].sid = pt_info->rbank_sid[rbank];
        bkt_blk_ctrl[unit]->array[curr].bank = rbank;
        bkt_blk_ctrl[unit]->array[curr].tbl_inst = pipe;
        bkt_blk_ctrl[unit]->array[curr].blk = blk_id;
        bkt_blk_ctrl[unit]->curr++;
    }

exit:
    if (SHR_FUNC_ERR()) {
        pt_info->bkt_state[rbank][pipe] = NULL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Restore per table bank bucket state.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table SID.
 * \param [in] rbank Bank number.
 * \param [in] pipe Pipe instance.
 * \param [in] blk Block ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_slb_state_blk_restore(int unit,
                              bcmdrd_sid_t sid,
                              uint16_t rbank,
                              uint8_t pipe,
                              uint16_t blk)
{
    rm_hash_pt_info_t *pt_info = NULL;
    uint32_t req_size, alloc_size, elem_cnt;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_pt_info_get(unit, &pt_info));

    /* Restore bucket state for the bank. */
    elem_cnt = pt_info->rbank_num_slbs[rbank];
    req_size = elem_cnt * sizeof(rm_hash_slb_state_t);
    alloc_size = req_size;
    if (req_size > 0) {
        pt_info->bkt_state[rbank][pipe] = shr_ha_mem_alloc(unit,
                                                     BCMMGMT_RM_HASH_COMP_ID,
                                                     blk,
                                                     "bcmptmRmhashBktState",
                                                     &alloc_size);
        SHR_NULL_CHECK(pt_info->bkt_state[rbank][pipe], SHR_E_MEMORY);
        SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
    }

exit:
    if (SHR_FUNC_ERR()) {
        pt_info->bkt_state[rbank][pipe] = NULL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate per table bank entry node state.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] pipe Pipe instance.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_node_blk_alloc(int unit,
                           rm_hash_pt_info_t *pt_info,
                           uint8_t rbank,
                           uint8_t pipe)
{
    uint32_t req_size, alloc_size, elem_cnt;
    uint16_t curr, blk_id, base_id;

    SHR_FUNC_ENTER(unit);

    /* Allocate entry node for the bank. */
    base_id = (rbank * 3 + 1) * RM_HASH_MAX_TBL_INST_COUNT +
              BCMPTM_HA_SUBID_RMHASH_BLK_ID_FIRST;
    blk_id = base_id + pipe;
    elem_cnt = pt_info->rbank_num_slbs[rbank] * pt_info->slb_num_entries;
    if (elem_cnt > 0) {
        req_size = elem_cnt * sizeof(rm_hash_ent_node_t);
        alloc_size = req_size;
        pt_info->ent_node[rbank][pipe] = shr_ha_mem_alloc(unit,
                                                    BCMMGMT_RM_HASH_COMP_ID,
                                                    blk_id,
                                                    "bcmptmRmhashEntNode",
                                                    &alloc_size);
        SHR_NULL_CHECK(pt_info->ent_node[rbank][pipe], SHR_E_MEMORY);
        SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
        sal_memset(pt_info->ent_node[rbank][pipe], 0, alloc_size);
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit,
                                              BCMMGMT_RM_HASH_COMP_ID,
                                              blk_id,
                                              0,
                                              sizeof(rm_hash_ent_node_t),
                                              elem_cnt,
                                              RM_HASH_ENT_NODE_T_ID));
        /* Record the mapping between (SID, bank) and block id. */
        curr = ent_blk_ctrl[unit]->curr;
        ent_blk_ctrl[unit]->array[curr].sid = pt_info->rbank_sid[rbank];
        ent_blk_ctrl[unit]->array[curr].bank = rbank;
        ent_blk_ctrl[unit]->array[curr].tbl_inst = pipe;
        ent_blk_ctrl[unit]->array[curr].blk = blk_id;
        ent_blk_ctrl[unit]->curr++;
    }

exit:
    if (SHR_FUNC_ERR()) {
        pt_info->ent_node[rbank][pipe] = NULL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Restore per table bank entry node state.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table SID.
 * \param [in] rbank Bank number.
 * \param [in] pipe Pipe instance.
 * \param [in] blk Block ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_node_blk_restore(int unit,
                             bcmdrd_sid_t sid,
                             uint16_t rbank,
                             uint8_t pipe,
                             uint16_t blk)
{
    rm_hash_pt_info_t *pt_info = NULL;
    uint32_t req_size, alloc_size, elem_cnt;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_pt_info_get(unit, &pt_info));

    /* Restore entry node state for the bank. */
    elem_cnt = pt_info->rbank_num_slbs[rbank] * pt_info->slb_num_entries;
    req_size = elem_cnt * sizeof(rm_hash_ent_node_t);
    alloc_size = req_size;
    if (req_size > 0) {
        pt_info->ent_node[rbank][pipe] = shr_ha_mem_alloc(unit,
                                                    BCMMGMT_RM_HASH_COMP_ID,
                                                    blk,
                                                    "bcmptmRmhashEntNode",
                                                    &alloc_size);
        SHR_NULL_CHECK(pt_info->ent_node[rbank][pipe], SHR_E_MEMORY);
        SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
    }

exit:
    if (SHR_FUNC_ERR()) {
        pt_info->ent_node[rbank][pipe] = NULL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate per table bank HA memory.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] pipe Pipe instance.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_slb_info_blk_alloc(int unit,
                           rm_hash_pt_info_t *pt_info,
                           uint8_t rbank,
                           uint8_t pipe)
{
    uint32_t req_size, alloc_size, elem_cnt;
    uint16_t curr, num_slb_info, blk_id, base_id;

    SHR_FUNC_ENTER(unit);

    /* Allocate software logical bucket info for the bank. */
    base_id = (rbank * 3 + 2) * RM_HASH_MAX_TBL_INST_COUNT +
              BCMPTM_HA_SUBID_RMHASH_BLK_ID_FIRST;
    blk_id = base_id + pipe;
    num_slb_info = pt_info->max_num_mapping_banks;
    elem_cnt = pt_info->rbank_num_slbs[rbank] * pt_info->slb_num_entries * num_slb_info;
    if (elem_cnt > 0) {
        req_size = elem_cnt * sizeof(rm_hash_ent_slb_info_t);
        alloc_size = req_size;
        pt_info->slb_info[rbank][pipe] = shr_ha_mem_alloc(unit,
                                                    BCMMGMT_RM_HASH_COMP_ID,
                                                    blk_id,
                                                    "bcmptmRmhashSlbInfo",
                                                    &alloc_size);
        SHR_NULL_CHECK(pt_info->slb_info[rbank][pipe], SHR_E_MEMORY);
        SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
        sal_memset(pt_info->slb_info[rbank][pipe], 0, alloc_size);
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit,
                                        BCMMGMT_RM_HASH_COMP_ID,
                                        blk_id,
                                        0,
                                        sizeof(rm_hash_ent_slb_info_t),
                                        elem_cnt,
                                        RM_HASH_ENT_SLB_INFO_T_ID));
        /* Record the mapping between (SID, bank) and block id. */
        curr = slb_blk_ctrl[unit]->curr;
        slb_blk_ctrl[unit]->array[curr].sid = pt_info->rbank_sid[rbank];
        slb_blk_ctrl[unit]->array[curr].bank = rbank;
        slb_blk_ctrl[unit]->array[curr].tbl_inst = pipe;
        slb_blk_ctrl[unit]->array[curr].blk = blk_id;
        slb_blk_ctrl[unit]->curr++;
    }

exit:
    if (SHR_FUNC_ERR()) {
        pt_info->slb_info[rbank][pipe] = NULL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Restore per table bank slb info state.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table SID.
 * \param [in] rbank Bank number.
 * \param [in] pipe Pipe instance.
 * \param [in] blk Block ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_slb_info_blk_restore(int unit,
                             bcmdrd_sid_t sid,
                             uint16_t rbank,
                             uint8_t pipe,
                             uint16_t blk)
{
    rm_hash_pt_info_t *pt_info = NULL;
    uint32_t req_size, alloc_size, elem_cnt;
    uint16_t num_slb_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_pt_info_get(unit, &pt_info));
    num_slb_info = pt_info->max_num_mapping_banks;
    /* Restore slb info state for the bank. */
    elem_cnt = pt_info->rbank_num_slbs[rbank] * pt_info->slb_num_entries * num_slb_info;
    req_size = elem_cnt * sizeof(rm_hash_ent_slb_info_t);
    alloc_size = req_size;
    if (req_size > 0) {
        pt_info->slb_info[rbank][pipe] = shr_ha_mem_alloc(unit,
                                                    BCMMGMT_RM_HASH_COMP_ID,
                                                    blk,
                                                    "bcmptmRmhashSlbInfo",
                                                    &alloc_size);
        SHR_NULL_CHECK(pt_info->slb_info[rbank][pipe], SHR_E_MEMORY);
        SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
    }

exit:
    if (SHR_FUNC_ERR()) {
        pt_info->slb_info[rbank][pipe] = NULL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate per table bank HA memory.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] pipe Pipe instance.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_nme_bmp_blk_alloc(int unit,
                          rm_hash_pt_info_t *pt_info,
                          uint8_t rbank,
                          uint8_t pipe)
{
    uint32_t req_size, alloc_size, elem_cnt, idx, loc;
    uint16_t curr, blk_id, base_id;
    rm_hash_nme_bmp_t *nme_bmp = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate narrow mode entry bmp for the bank. */
    base_id = rbank * RM_HASH_MAX_TBL_INST_COUNT +
              BCMPTM_HA_SUBID_RMHASH_NMBLK_ID_FIRST;
    blk_id = base_id + pipe;

    elem_cnt = pt_info->rbank_num_slbs[rbank] * pt_info->slb_size;
    if (elem_cnt > 0) {
        req_size = elem_cnt * sizeof(rm_hash_nme_bmp_t);
        alloc_size = req_size;
        pt_info->nme_bmp[rbank][pipe] = shr_ha_mem_alloc(unit,
                                                    BCMMGMT_RM_HASH_COMP_ID,
                                                    blk_id,
                                                    "bcmptmRmhashNmeBmp",
                                                    &alloc_size);
        SHR_NULL_CHECK(pt_info->nme_bmp[rbank][pipe], SHR_E_MEMORY);
        SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
        sal_memset(pt_info->nme_bmp[rbank][pipe], 0, alloc_size);
        nme_bmp = pt_info->nme_bmp[rbank][pipe];
        /* Initialize the ltid as invalid value. */
        for (idx = 0; idx < elem_cnt; idx++, nme_bmp++) {
            for (loc = 0; loc < RM_HASH_NARROW_ENTRY_LITD_NUM; loc++) {
                nme_bmp->ltid[loc] = BCMLTD_SID_INVALID;
            }
        }
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit,
                                        BCMMGMT_RM_HASH_COMP_ID,
                                        blk_id,
                                        0,
                                        sizeof(rm_hash_nme_bmp_t),
                                        elem_cnt,
                                        RM_HASH_NME_BMP_T_ID));
        /* Record the mapping between (SID, bank) and block id. */
        curr = nme_bmp_blk_ctrl[unit]->curr;
        nme_bmp_blk_ctrl[unit]->array[curr].sid = pt_info->rbank_sid[rbank];
        nme_bmp_blk_ctrl[unit]->array[curr].bank = rbank;
        nme_bmp_blk_ctrl[unit]->array[curr].tbl_inst = pipe;
        nme_bmp_blk_ctrl[unit]->array[curr].blk = blk_id;
        nme_bmp_blk_ctrl[unit]->curr++;
    }

exit:
    if (SHR_FUNC_ERR()) {
        pt_info->nme_bmp[rbank][pipe] = NULL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Restore per table bank nme bmp info state.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table SID.
 * \param [in] rbank Bank number.
 * \param [in] pipe Pipe instance.
 * \param [in] blk Block ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_nme_bmp_blk_restore(int unit,
                            bcmdrd_sid_t sid,
                            uint16_t rbank,
                            uint8_t pipe,
                            uint16_t blk)
{
    rm_hash_pt_info_t *pt_info = NULL;
    uint32_t req_size, alloc_size, elem_cnt;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_pt_info_get(unit, &pt_info));

    /* Restore narrow mode bmp state for the bank. */
    elem_cnt = pt_info->rbank_num_slbs[rbank] * pt_info->slb_size;
    req_size = elem_cnt * sizeof(rm_hash_nme_bmp_t);
    alloc_size = req_size;
    if (req_size > 0) {
        pt_info->nme_bmp[rbank][pipe] = shr_ha_mem_alloc(unit,
                                                    BCMMGMT_RM_HASH_COMP_ID,
                                                    blk,
                                                    "bcmptmRmhashNmeBmp",
                                                    &alloc_size);
        SHR_NULL_CHECK(pt_info->nme_bmp[rbank][pipe], SHR_E_MEMORY);
        SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
    }

exit:
    if (SHR_FUNC_ERR()) {
        pt_info->nme_bmp[rbank][pipe] = NULL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure per table bank HA memory.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] Pipe Pipe instance.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static void
rm_hash_blk_state_config(int unit,
                         rm_hash_pt_info_t *pt_info,
                         uint8_t rbank,
                         uint8_t pipe)
{
    rm_hash_slb_state_t *b_state = NULL;
    uint32_t e_node_offset = 0, idx, e_slb_offset = 0, e_node_cnt;
    rm_hash_ent_node_t *e_node = NULL;
    uint16_t num_slb_info;

    b_state = pt_info->bkt_state[rbank][pipe];
    for (idx = 0; idx < pt_info->rbank_num_slbs[rbank]; idx++, b_state++) {
        /* Configure entry_node_offset for each bucket state. */
        b_state->e_node_offset = e_node_offset;
        e_node_offset += pt_info->slb_num_entries;
    }

    e_node = pt_info->ent_node[rbank][pipe];
    e_node_cnt = pt_info->rbank_num_slbs[rbank] * pt_info->slb_num_entries;
    num_slb_info = pt_info->max_num_mapping_banks;
    for (idx = 0; idx < e_node_cnt; idx++, e_node++) {
        e_node->ent_slb.slb_info_offset = e_slb_offset;
        /*
         * Note the num_slb_info field in the entry node
         * has not been configured.
         */
        e_slb_offset += num_slb_info;
    }
}

/*!
 * \brief Allocate per table bank HA memory.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] Pipe Pipe instance.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_bkt_state_alloc(int unit,
                        rm_hash_pt_info_t *pt_info,
                        uint8_t rbank,
                        uint8_t pipe)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (rm_hash_slb_state_blk_alloc(unit, pt_info, rbank, pipe));
    SHR_IF_ERR_EXIT
        (rm_hash_ent_node_blk_alloc(unit, pt_info, rbank, pipe));
    SHR_IF_ERR_EXIT
        (rm_hash_slb_info_blk_alloc(unit, pt_info, rbank, pipe));
    rm_hash_blk_state_config(unit, pt_info, rbank, pipe);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Determine if an entry node has the specified mode and location.
 *
 * \param [in] unit Unit number.
 * \param [in] nme_info Pointer to rm_hash_nme_info_t structure.
 * \param [in] e_node Pointer to rm_hash_ent_node_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static bool
rm_hash_nm_ent_node_match(int unit,
                          rm_hash_nme_info_t *nme_info,
                          rm_hash_ent_node_t *e_node)
{

    if ((e_node->flag & RM_HASH_NODE_NARROWMODEHALF) &&
        (nme_info->e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_HALF) &&
        (e_node->nm_loc == nme_info->nm_loc)) {
        return TRUE;
    }
    if ((e_node->flag & RM_HASH_NODE_NARROWMODETHIRD) &&
        (nme_info->e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_THIRD) &&
        (e_node->nm_loc == nme_info->nm_loc)) {
        return TRUE;
    }
    return FALSE;
}

/*!
 * \brief Get the used bitmap for the narrow mode entry.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] pipe Pipe number.
 * \param [in] be_offset Base entry offset.
 * \param [out] used_bmp Pointer to the buffer recording the result.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_nme_used_bmp_get(int unit,
                         rm_hash_pt_info_t *pt_info,
                         uint8_t rbank,
                         uint8_t pipe,
                         uint32_t be_offset,
                         uint8_t *used_bmp)
{
    rm_hash_nme_bmp_t *nme_bmp = NULL;
    rm_hash_nme_bmp_t *bmp_base = pt_info->nme_bmp[rbank][pipe];

    SHR_FUNC_ENTER(unit);

    if (bmp_base == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    nme_bmp = bmp_base + be_offset;
    *used_bmp = nme_bmp->bmp;

exit:
SHR_FUNC_EXIT();
}

/*!
 * \brief Set the used bitmap for the narrow mode entry.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] pipe Pipe number.
 * \param [in] be_offset Base entry offset.
 * \param [in] nme_info Pointer to rm_hash_nme_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_nme_bmp_set(int unit,
                    rm_hash_pt_info_t *pt_info,
                    uint8_t rbank,
                    uint8_t pipe,
                    uint32_t be_offset,
                    rm_hash_nme_info_t *nme_info)
{
    rm_hash_nme_bmp_t *nme_bmp;
    uint8_t *bmp, nm_loc;
    rm_hash_nme_bmp_t *bmp_base = pt_info->nme_bmp[rbank][pipe];

    SHR_FUNC_ENTER(unit);

    nm_loc = nme_info->nm_loc;

    if (bmp_base == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    nme_bmp = bmp_base + be_offset;
    bmp = &nme_bmp->bmp;

    switch (nme_info->e_nm) {
    case BCMPTM_RM_HASH_ENTRY_NARROW_MODE_HALF:
        *bmp |= (0x07 << nm_loc);
        nme_bmp->ltid[nm_loc] = nme_info->ltid;
        nme_bmp->nm[nm_loc] = nme_info->e_nm;
        break;
    case BCMPTM_RM_HASH_ENTRY_NARROW_MODE_THIRD:
        *bmp |= (0x03 << nm_loc);
        nme_bmp->ltid[nm_loc] = nme_info->ltid;
        nme_bmp->nm[nm_loc] = nme_info->e_nm;
        break;
    case BCMPTM_RM_HASH_ENTRY_NARROW_MODE_NONE:
        *bmp |= 0x3F;
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
SHR_FUNC_EXIT();
}

/*!
 * \brief Clear the used bitmap for the narrow mode entry.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] pipe Pipe number.
 * \param [in] be_offset Base entry offset.
 * \param [in] e_nm Entry narrow mode.
 * \param [in] nm_loc Narrow mode entry location.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_nme_bmp_clear(int unit,
                      rm_hash_pt_info_t *pt_info,
                      uint8_t rbank,
                      uint8_t pipe,
                      uint32_t be_offset,
                      rm_hash_entry_narrow_mode_t e_nm,
                      uint8_t nm_loc)
{
    rm_hash_nme_bmp_t *nme_bmp;
    uint8_t *bmp;
    rm_hash_nme_bmp_t *bmp_base = pt_info->nme_bmp[rbank][pipe];

    SHR_FUNC_ENTER(unit);

    if (bmp_base == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    nme_bmp = bmp_base + be_offset;
    bmp = &nme_bmp->bmp;

    switch (e_nm) {
    case BCMPTM_RM_HASH_ENTRY_NARROW_MODE_HALF:
        *bmp &= ~(0x07 << nm_loc);
        nme_bmp->ltid[nm_loc] = BCMLTD_SID_INVALID;
        nme_bmp->nm[nm_loc] = 0;
        break;
    case BCMPTM_RM_HASH_ENTRY_NARROW_MODE_THIRD:
        *bmp &= ~(0x03 << nm_loc);
        nme_bmp->ltid[nm_loc] = BCMLTD_SID_INVALID;
        nme_bmp->nm[nm_loc] = 0;
        break;
    case BCMPTM_RM_HASH_ENTRY_NARROW_MODE_NONE:
        *bmp = 0x0;
        /* For non-narrow mode entries, the ltid id maintained in PTcache. */
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_hash_nm_ent_node_match(int unit,
                                 rm_hash_ent_node_t *e_node,
                                 rm_hash_entry_narrow_mode_t enm,
                                 bool *match,
                                 uint8_t *loc)
{
    SHR_FUNC_ENTER(unit);

    *match = FALSE;

    if ((enm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_HALF) &&
        (e_node->flag & RM_HASH_NODE_NARROWMODEHALF)) {
        *match = TRUE;
        *loc = e_node->nm_loc;
    }
    if ((enm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_THIRD) &&
        (e_node->flag & RM_HASH_NODE_NARROWMODETHIRD)) {
        *match = TRUE;
        *loc = e_node->nm_loc;
    }

    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_nme_bmp_get(int unit,
                           rm_hash_pt_info_t *pt_info,
                           uint8_t rbank,
                           uint8_t pipe,
                           uint32_t be_offset,
                           rm_hash_nme_bmp_t **nme_bmp)
{
    rm_hash_nme_bmp_t *bmp_base = pt_info->nme_bmp[rbank][pipe];

    SHR_FUNC_ENTER(unit);

    if (bmp_base == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    *nme_bmp = bmp_base + be_offset;

exit:
SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_ent_slb_view_be_bmp_get(int unit,
                                       rm_hash_ent_loc_t *e_loc,
                                       uint16_t *slb_be_bmp)
{
    uint16_t tmp = 0;

    SHR_FUNC_ENTER(unit);

    tmp = 0x00FF & e_loc->be_bmp;
    switch (e_loc->bb_bmp) {
    case 0x0001:
        *slb_be_bmp = tmp;
        SHR_EXIT();
    case 0x0002:
        *slb_be_bmp = tmp << 4;
        SHR_EXIT();
    case 0x0004:
        *slb_be_bmp = tmp << 8;
        SHR_EXIT();
    case 0x0008:
        *slb_be_bmp = tmp << 12;
        SHR_EXIT();
    default:
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_ent_slb_view_le_bmp_get(int unit,
                                       rm_hash_ent_loc_t *e_loc,
                                       uint16_t *slb_le_bmp)
{
    uint16_t tmp = 0;

    SHR_FUNC_ENTER(unit);

    tmp = 0x00FF & e_loc->le_bmp;
    switch (e_loc->bb_bmp) {
    case 0x0001:
        *slb_le_bmp = tmp;
        SHR_EXIT();
    case 0x0002:
        *slb_le_bmp = tmp << 4;
        SHR_EXIT();
    case 0x0004:
        *slb_le_bmp = tmp << 8;
        SHR_EXIT();
    case 0x0008:
        *slb_le_bmp = tmp << 12;
        SHR_EXIT();
    default:
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_ha_state_alloc(int unit,
                              rm_hash_pt_info_t *pt_info,
                              uint8_t rbank,
                              int tbl_inst)
{
    rm_hash_bank_mode_t mode;
    uint8_t pipe, idx;
    int pipe_num;
    bcmdrd_sid_t sid;

    SHR_FUNC_ENTER(unit);

    mode = pt_info->bank_mode[rbank];

    if (tbl_inst == -1) {
        /* This is for global LT, the SW state for pipe 0 can be used. */
        switch (mode) {
        case RM_HASH_BANK_MODE_GLOBAL:
            if (pt_info->bkt_state[rbank][0] == NULL) {
                SHR_IF_ERR_EXIT
                    (rm_hash_bkt_state_alloc(unit, pt_info, rbank, 0));
            }
            /* Allocate narrow mode entry bitmap when this mode is enabled. */
            if ((pt_info->bank_in_narrow_mode[rbank] == TRUE) &&
                (pt_info->nme_bmp[rbank][0] == NULL)) {
                SHR_IF_ERR_EXIT
                    (rm_hash_nme_bmp_blk_alloc(unit, pt_info, rbank, 0));
            }
            SHR_EXIT();
        case RM_HASH_BANK_MODE_MIXED:
            /*
             * The physical table bank works in mixed mode, and the SW state for
             * some pipe must have been allocated.
             */
            sid = *pt_info->single_view_sid[rbank];
            pipe_num = bcmdrd_pt_num_tbl_inst(unit, sid);
            for (idx = 0; idx < pipe_num; idx++) {
                if (pt_info->bkt_state[rbank][idx] == NULL) {
                    SHR_IF_ERR_EXIT
                        (rm_hash_bkt_state_alloc(unit, pt_info, rbank, idx));
                }
                /* Allocate narrow mode entry bitmap when this mode is enabled. */
                if ((pt_info->bank_in_narrow_mode[rbank] == TRUE) &&
                    (pt_info->nme_bmp[rbank][idx] == NULL)) {
                    SHR_IF_ERR_EXIT
                        (rm_hash_nme_bmp_blk_alloc(unit, pt_info, rbank, idx));
                }
            }
            SHR_EXIT();
        case RM_HASH_BANK_MODE_PERPILE:
            SHR_ERR_EXIT(SHR_E_PARAM);
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    } else if (tbl_inst >= 0) {
        /* This is for per pipe LT, the SW state for the specified pipe can be used. */
        switch (mode) {
        case RM_HASH_BANK_MODE_PERPILE:
        case RM_HASH_BANK_MODE_MIXED:
            pipe = tbl_inst;
            if (pt_info->bkt_state[rbank][pipe] == NULL) {
                SHR_IF_ERR_EXIT
                    (rm_hash_bkt_state_alloc(unit, pt_info, rbank, pipe));
            }
            /* Allocate narrow mode entry bitmap when this mode is enabled. */
            if ((pt_info->bank_in_narrow_mode[rbank] == TRUE) &&
                (pt_info->nme_bmp[rbank][pipe] == NULL)) {
                SHR_IF_ERR_EXIT
                    (rm_hash_nme_bmp_blk_alloc(unit, pt_info, rbank, pipe));
            }
            SHR_EXIT();
        case RM_HASH_BANK_MODE_GLOBAL:
            SHR_ERR_EXIT(SHR_E_PARAM);
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_slb_state_get(int unit,
                             rm_hash_pt_info_t *pt_info,
                             uint8_t rbank,
                             uint8_t pipe,
                             uint32_t slb,
                             rm_hash_slb_state_t **b_state)
{
    uint32_t slb_offset = slb - pt_info->rbank_slb_base[rbank];

    SHR_FUNC_ENTER(unit);

    if (pt_info->bkt_state[rbank][pipe] == NULL) {
        *b_state = NULL;
    } else {
        *b_state = pt_info->bkt_state[rbank][pipe] + slb_offset;
    }

    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_ent_node_allocate(int unit,
                                 rm_hash_pt_info_t *pt_info,
                                 uint8_t rbank,
                                 uint8_t pipe,
                                 uint32_t slb,
                                 uint8_t *e_node_idx,
                                 rm_hash_ent_node_t **pe_node)
{
    rm_hash_ent_node_t *e_node = NULL, *base_e_node = NULL;
    uint8_t idx;
    rm_hash_slb_state_t *b_state = NULL;
    uint32_t offset;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_slb_state_get(unit, pt_info, rbank, pipe, slb,
                                      &b_state));
    offset = b_state->e_node_offset;

    if (e_node_idx != NULL) {
        bcmptm_rm_hash_base_ent_node_get(unit, pt_info, rbank, pipe, offset,
                                         &base_e_node);
        e_node = base_e_node + *e_node_idx;
        e_node->flag |= RM_HASH_NODE_IN_USE;
        *pe_node = e_node;
    } else {
        bcmptm_rm_hash_base_ent_node_get(unit, pt_info, rbank, pipe, offset,
                                         &e_node);
        for (idx = 0; idx < pt_info->slb_num_entries; idx++, e_node++) {
            if ((e_node->flag & RM_HASH_NODE_IN_USE) == 0) {
                e_node->flag |= RM_HASH_NODE_IN_USE;
                *pe_node = e_node;
                SHR_EXIT();
            }
        }
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_free_ent_node_idx_get(int unit,
                                     rm_hash_pt_info_t *pt_info,
                                     uint8_t rbank,
                                     uint8_t pipe,
                                     uint32_t slb,
                                     uint8_t *e_node_idx)
{
    rm_hash_slb_state_t *b_state = NULL;
    rm_hash_ent_node_t *e_node = NULL;
    uint8_t idx;
    uint32_t offset;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_slb_state_get(unit, pt_info, rbank, pipe, slb,
                                      &b_state));
    offset = b_state->e_node_offset;
    bcmptm_rm_hash_base_ent_node_get(unit, pt_info, rbank, pipe, offset,
                                     &e_node);
    for (idx = 0; idx < pt_info->slb_num_entries; idx++, e_node++) {
        if ((e_node->flag & RM_HASH_NODE_IN_USE) == 0) {
            *e_node_idx = idx;
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(SHR_E_INTERNAL);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_ent_node_insert(int unit,
                               rm_hash_pt_info_t *pt_info,
                               uint8_t rbank,
                               uint8_t pipe,
                               uint32_t slb,
                               rm_hash_nme_info_t *nme_info,
                               rm_hash_ent_node_t *e_node)
{
    uint32_t e_index;
    uint16_t be_bmp, le_bmp;
    rm_hash_bm_t bm;
    rm_hash_slb_state_t *b_state = NULL;
    rm_hash_ent_loc_t tmp_e_loc;
    uint8_t be_idx;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_slb_state_get(unit, pt_info, rbank, pipe, slb,
                                      &b_state));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_slb_view_be_bmp_get(unit,
                                                &e_node->e_loc,
                                                &be_bmp));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_slb_view_le_bmp_get(unit,
                                                &e_node->e_loc,
                                                &le_bmp));
    bm = e_node->ent_slb.bm;
    b_state->e_bmp.be_bmp |= be_bmp;
    b_state->e_bmp.le_bmp[bm] |= le_bmp;
    b_state->ve_cnt++;

    if (pt_info->bank_in_narrow_mode[rbank] == TRUE) {
        /*
         * When the bank works in narrow entry mode, the additional bitmap
         * indicating the usage of sub-base entry will be set,
         * regardless of the entry is narrow or not.
         */
        for (be_idx = 0; be_idx < pt_info->base_bkt_size[rbank]; be_idx++) {
            if ((0x01 << be_idx) & e_node->e_loc.be_bmp) {
                sal_memset(&tmp_e_loc, 0, sizeof(tmp_e_loc));
                tmp_e_loc.bb_bmp = e_node->e_loc.bb_bmp;
                tmp_e_loc.be_bmp = 0x01 << be_idx;
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_ent_index_get(unit, pt_info, slb,
                                                  &tmp_e_loc, 1,
                                                  &rbank, &e_index));
                SHR_IF_ERR_EXIT
                    (rm_hash_nme_bmp_set(unit, pt_info, rbank, pipe,
                                         e_index,
                                         nme_info));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_ent_node_search(int unit,
                               rm_hash_pt_info_t *pt_info,
                               uint8_t rbank,
                               uint8_t pipe,
                               rm_hash_slb_state_t *b_state,
                               uint8_t bb_bmp,
                               uint8_t be_bmp,
                               rm_hash_nme_info_t *nme_info,
                               rm_hash_ent_node_t **pe_node)
{
    rm_hash_ent_node_t *e_node = NULL;
    uint32_t offset = b_state->e_node_offset;
    uint8_t idx;
    bool match = FALSE;

    SHR_FUNC_ENTER(unit);

    if (b_state->ve_cnt == 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    bcmptm_rm_hash_base_ent_node_get(unit, pt_info, rbank, pipe, offset,
                                     &e_node);
    for (idx = 0; idx < pt_info->slb_num_entries; idx++, e_node++) {
        if ((e_node->flag & RM_HASH_NODE_IN_USE) &&
            (e_node->e_loc.bb_bmp == bb_bmp) &&
            (e_node->e_loc.be_bmp == be_bmp)) {
            if (nme_info != NULL) {
                match = rm_hash_nm_ent_node_match(unit, nme_info, e_node);
                if (match == FALSE) {
                    continue;
                }
            }
            if (pe_node != NULL) {
                *pe_node = e_node;
                SHR_EXIT();
            }
        }
    }
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_paired_e_node_get(int unit,
                                 rm_hash_pt_info_t *pt_info,
                                 uint8_t pair_rbank,
                                 uint8_t pipe,
                                 rm_hash_ent_node_t *e_node,
                                 uint8_t paired_slb_loc,
                                 uint32_t pair_slb,
                                 rm_hash_ent_node_t **pe_node)
{
    uint8_t e_node_idx;
    rm_hash_slb_state_t *b_state = NULL;
    rm_hash_ent_node_t *base_e_node = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_slb_state_get(unit, pt_info, pair_rbank, pipe, pair_slb,
                                      &b_state));
    if (b_state->ve_cnt > 0) {
        bcmptm_rm_hash_base_ent_node_get(unit,
                                         pt_info,
                                         pair_rbank,
                                         pipe,
                                         b_state->e_node_offset,
                                         &base_e_node);
    }
    e_node_idx = e_node->ent_slb.e_node_idx[paired_slb_loc];
    *pe_node = base_e_node + e_node_idx;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_paired_slb_get(int unit,
                              rm_hash_pt_info_t *pt_info,
                              uint8_t rbank,
                              uint8_t pipe,
                              rm_hash_ent_node_t *e_node,
                              uint32_t slb,
                              bool *exist,
                              rm_hash_ent_slb_info_t *paired_slb_info,
                              uint8_t *paired_slb_loc)
{
    uint8_t idx;
    rm_hash_ent_slb_info_t *e_slb_info = NULL, *tmp_slb_info = NULL;
    uint8_t num_slbs = 0;

    SHR_FUNC_ENTER(unit);

    *exist = FALSE;
    bcmptm_rm_hash_ent_slb_info_get(unit,
                                    pt_info,
                                    rbank,
                                    pipe,
                                    e_node->ent_slb.slb_info_offset,
                                    &e_slb_info);
    num_slbs = e_node->ent_slb.num_slb_info;

    for (idx = 0; idx < num_slbs; idx++, e_slb_info++) {
        if (e_slb_info->slb == slb) {
            if ((e_slb_info->map & RM_HASH_MULTI_INST_GRP) == 0) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            } else {
                /* The specified SLB belongs to multi-instance group. */
                if (idx == 0) {
                    /* The paired SLB will be at index 1 */
                    tmp_slb_info = e_slb_info + 1;
                    if (tmp_slb_info->map != e_slb_info->map) {
                        SHR_ERR_EXIT(SHR_E_INTERNAL);
                    }
                    *exist = TRUE;
                    *paired_slb_info = *tmp_slb_info;
                    *paired_slb_loc = 1;
                    SHR_EXIT();
                } else if (idx == num_slbs - 1) {
                    /* The paired SLB will be at index num - 1. */
                    tmp_slb_info = e_slb_info - 1;
                    if (tmp_slb_info->map != e_slb_info->map) {
                        SHR_ERR_EXIT(SHR_E_INTERNAL);
                    }
                    *exist = TRUE;
                    *paired_slb_info = *tmp_slb_info;
                    *paired_slb_loc = 0;
                    SHR_EXIT();
                } else {
                    /* Examine adjacent locations. */
                    tmp_slb_info = e_slb_info - 1;
                    if (tmp_slb_info->map == e_slb_info->map) {
                        *exist = TRUE;
                        *paired_slb_info = *tmp_slb_info;
                        *paired_slb_loc = 0;
                        SHR_EXIT();
                    }
                    tmp_slb_info = e_slb_info + 1;
                    if (tmp_slb_info->map == e_slb_info->map) {
                        *exist = TRUE;
                        *paired_slb_info = *tmp_slb_info;
                        *paired_slb_loc = 1;
                        SHR_EXIT();
                    }
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_ent_node_delete(int unit,
                               rm_hash_pt_info_t *pt_info,
                               uint8_t rbank,
                               uint8_t pipe,
                               uint32_t slb,
                               uint8_t bb_bmp,
                               uint8_t be_bmp,
                               rm_hash_nme_info_t *nme_info,
                               rm_hash_ent_node_t **pe_node)
{
    rm_hash_slb_state_t *b_state = NULL;
    rm_hash_ent_node_t *e_node = NULL;
    uint32_t offset;
    uint8_t be_idx, idx, used_bmp = 0;
    uint16_t slb_be_bmp = 0, slb_le_bmp = 0;
    uint32_t e_index;
    bool nm_node_found = FALSE, clear_slb_bmp = TRUE;
    rm_hash_ent_loc_t tmp_e_loc;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_slb_state_get(unit, pt_info, rbank, pipe, slb,
                                      &b_state));
    offset = b_state->e_node_offset;
    if (b_state->ve_cnt == 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    bcmptm_rm_hash_base_ent_node_get(unit, pt_info, rbank, pipe, offset,
                                     &e_node);

    for (idx = 0; idx < pt_info->slb_num_entries; idx++, e_node++) {
        if ((e_node->flag & RM_HASH_NODE_IN_USE) &&
            (e_node->e_loc.bb_bmp == bb_bmp) &&
            (e_node->e_loc.be_bmp == be_bmp)) {
            if ((nme_info != NULL) &&
                ((nme_info->e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_HALF) ||
                 (nme_info->e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_THIRD))) {
                /*
                 * For narrow mode entry, still need to examine the narrow
                 * entry location.
                 */
                nm_node_found = rm_hash_nm_ent_node_match(unit, nme_info,
                                                          e_node);
                if (nm_node_found == FALSE) {
                    continue;
                }
            }
            if (pt_info->bank_in_narrow_mode[rbank] == TRUE) {
                for (be_idx = 0; be_idx < pt_info->base_bkt_size[rbank]; be_idx++) {
                    if ((0x01 << be_idx) & e_node->e_loc.be_bmp) {
                        sal_memset(&tmp_e_loc, 0, sizeof(tmp_e_loc));
                        tmp_e_loc.bb_bmp = e_node->e_loc.bb_bmp;
                        tmp_e_loc.be_bmp = 0x01 << be_idx;
                        /* Get the index of the first base entry in the bucket. */
                        SHR_IF_ERR_EXIT
                            (bcmptm_rm_hash_ent_index_get(unit, pt_info, slb,
                                                          &tmp_e_loc, 1,
                                                          &rbank, &e_index));
                        if (nme_info != NULL) {
                            SHR_IF_ERR_EXIT
                                (rm_hash_nme_bmp_clear(unit, pt_info, rbank,
                                                       pipe, e_index,
                                                       nme_info->e_nm,
                                                       nme_info->nm_loc));
                        }
                        SHR_IF_ERR_EXIT
                            (rm_hash_nme_used_bmp_get(unit, pt_info, rbank,
                                                      pipe, e_index,
                                                      &used_bmp));
                        if (used_bmp != 0) {
                            /* There are still narrow mode entries in the base entry. */
                            clear_slb_bmp = FALSE;
                        }
                    }
                }
            }
            if (clear_slb_bmp == TRUE) {
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_ent_slb_view_be_bmp_get(unit,
                                                            &e_node->e_loc,
                                                            &slb_be_bmp));
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_ent_slb_view_le_bmp_get(unit,
                                                            &e_node->e_loc,
                                                            &slb_le_bmp));
                b_state->e_bmp.be_bmp &= ~slb_be_bmp;
                b_state->e_bmp.le_bmp[e_node->ent_slb.bm] &= ~slb_le_bmp;
            }

            b_state->ve_cnt--;
            /* Should only clear the IN_USE flag. */
            e_node->flag &= ~RM_HASH_NODE_IN_USE;
            if (pe_node != NULL) {
                *pe_node = e_node;
            }
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

void
bcmptm_rm_hash_ent_node_set(int unit,
                            rm_hash_pt_info_t *pt_info,
                            rm_hash_ent_loc_t *e_loc,
                            rm_hash_ent_slb_info_t *slb_info,
                            uint8_t num_slb_info,
                            uint8_t rbank,
                            uint8_t pipe,
                            uint8_t *e_node_idx_list,
                            uint8_t num_insts,
                            bcmdrd_sid_t sid,
                            uint32_t e_sig,
                            rm_hash_bm_t bm,
                            rm_hash_ent_node_t *e_node)
{
    rm_hash_ent_slb_info_t *e_slb_info = NULL;
    uint8_t idx;

    e_node->sid = sid;
    e_node->sig = e_sig;
    e_node->ent_slb.bm = bm;
    e_node->e_loc = *e_loc;

    LOG_VERBOSE(BSL_LOG_MODULE,
               (BSL_META_U(unit,
                           "sig:[%d], "
                           "bucket mode:[%d], "),
                           e_node->sig,
                           e_node->ent_slb.bm));
    for (idx = 0; idx < num_insts; idx++) {
        e_node->ent_slb.e_node_idx[idx] = e_node_idx_list[idx];
    }
    e_node->ent_slb.num_rbanks = num_insts;
    bcmptm_rm_hash_ent_slb_info_get(unit,
                                    pt_info,
                                    rbank,
                                    pipe,
                                    e_node->ent_slb.slb_info_offset,
                                    &e_slb_info);
    for (idx = 0; idx < num_slb_info; idx++) {
        e_slb_info[idx] = slb_info[idx];
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "software logical bucket:[%d], "
                                "valid base bucket bitmap:[%d]\n"),
                                e_slb_info[idx].slb,
                                e_slb_info[idx].vbb_bmp));
    }
    e_node->ent_slb.num_slb_info = num_slb_info;
}

uint8_t
bcmptm_rm_hash_ent_size_get(uint8_t be_bmp)
{
    uint8_t nibble_lo = 0, nibble_hi = 0;

    nibble_lo = be_bmp & 0x0F;
    nibble_hi = (be_bmp >> 4) & 0x0F;
    return e_size_map[nibble_lo].size + e_size_map[nibble_hi].size;
}

void
bcmptm_rm_hash_base_ent_node_get(int unit,
                                 rm_hash_pt_info_t *pt_info,
                                 uint8_t rbank,
                                 uint8_t pipe,
                                 uint32_t e_node_offset,
                                 rm_hash_ent_node_t **pe_node)
{
    rm_hash_ent_node_t *e_node_base = pt_info->ent_node[rbank][pipe];

    *pe_node = e_node_base + e_node_offset;
}

void
bcmptm_rm_hash_ent_slb_info_get(int unit,
                                rm_hash_pt_info_t *pt_info,
                                uint8_t rbank,
                                uint8_t pipe,
                                uint32_t e_slb_info_offset,
                                rm_hash_ent_slb_info_t **pe_slb_info)
{
    rm_hash_ent_slb_info_t *slb_info_base = pt_info->slb_info[rbank][pipe];

    *pe_slb_info = slb_info_base + e_slb_info_offset;
}

int
bcmptm_rm_hash_blk_ctrl_alloc(int unit, bool warm)
{
    uint32_t req_size, alloc_size;
    uint16_t idx, blk, rbank, tbl_inst;
    bcmdrd_sid_t sid;
    rm_hash_pt_info_t *pt_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate bucket state control structure. */
    req_size = sizeof(rm_hash_bkt_state_blk_ctrl_t);
    alloc_size = req_size;
    bkt_blk_ctrl[unit] = shr_ha_mem_alloc(unit,
                                          BCMMGMT_RM_HASH_COMP_ID,
                                          BCMPTM_HA_SUBID_RMHASH_BKT_STATE_CTRL,
                                          "bcmptmRmhashBktStateCtrl",
                                          &alloc_size);
    SHR_NULL_CHECK(bkt_blk_ctrl[unit], SHR_E_MEMORY);
    SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
    if (warm == FALSE) {
        sal_memset(bkt_blk_ctrl[unit], 0, alloc_size);
        bcmissu_struct_info_report(unit,
                                   BCMMGMT_RM_HASH_COMP_ID,
                                   BCMPTM_HA_SUBID_RMHASH_BKT_STATE_CTRL,
                                   0,
                                   req_size, 1,
                                   RM_HASH_BKT_STATE_BLK_CTRL_T_ID);
    } else {
        /* Restore bucket state in warmboot. */
        for (idx = 0; idx < bkt_blk_ctrl[unit]->curr; idx++) {
            blk = bkt_blk_ctrl[unit]->array[idx].blk;
            sid = bkt_blk_ctrl[unit]->array[idx].sid;
            rbank = bkt_blk_ctrl[unit]->array[idx].bank;
            tbl_inst = bkt_blk_ctrl[unit]->array[idx].tbl_inst;
            SHR_IF_ERR_EXIT
                (rm_hash_slb_state_blk_restore(unit, sid, rbank, tbl_inst, blk));
        }
    }

    /* Allocate entry node control structure. */
    req_size = sizeof(rm_hash_ent_node_blk_ctrl_t);
    alloc_size = req_size;
    ent_blk_ctrl[unit] = shr_ha_mem_alloc(unit,
                                          BCMMGMT_RM_HASH_COMP_ID,
                                          BCMPTM_HA_SUBID_RMHASH_ENT_NODE_CTRL,
                                          "bcmptmRmhashEntNodeCtrl",
                                          &alloc_size);
    SHR_NULL_CHECK(ent_blk_ctrl[unit], SHR_E_MEMORY);
    SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
    if (warm == FALSE) {
        sal_memset(ent_blk_ctrl[unit], 0, alloc_size);
        bcmissu_struct_info_report(unit,
                                   BCMMGMT_RM_HASH_COMP_ID,
                                   BCMPTM_HA_SUBID_RMHASH_ENT_NODE_CTRL,
                                   0,
                                   req_size, 1,
                                   RM_HASH_ENT_NODE_BLK_CTRL_T_ID);
    } else {
        /* Restore entry node state in warmboot. */
        for (idx = 0; idx < ent_blk_ctrl[unit]->curr; idx++) {
            blk = ent_blk_ctrl[unit]->array[idx].blk;
            sid = ent_blk_ctrl[unit]->array[idx].sid;
            rbank = ent_blk_ctrl[unit]->array[idx].bank;
            tbl_inst = ent_blk_ctrl[unit]->array[idx].tbl_inst;
            SHR_IF_ERR_EXIT
                (rm_hash_ent_node_blk_restore(unit, sid, rbank, tbl_inst, blk));
        }
    }

    /* Allocate slb info control structure. */
    req_size = sizeof(rm_hash_slb_info_blk_ctrl_t);
    alloc_size = req_size;
    slb_blk_ctrl[unit] = shr_ha_mem_alloc(unit,
                                          BCMMGMT_RM_HASH_COMP_ID,
                                          BCMPTM_HA_SUBID_RMHASH_SLB_INFO_CTRL,
                                          "bcmptmRmhashSlbInfoCtrl",
                                          &alloc_size);
    SHR_NULL_CHECK(slb_blk_ctrl[unit], SHR_E_MEMORY);
    SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
    if (warm == FALSE) {
        sal_memset(slb_blk_ctrl[unit], 0, alloc_size);
        bcmissu_struct_info_report(unit,
                                   BCMMGMT_RM_HASH_COMP_ID,
                                   BCMPTM_HA_SUBID_RMHASH_SLB_INFO_CTRL,
                                   0,
                                   req_size, 1,
                                   RM_HASH_SLB_INFO_BLK_CTRL_T_ID);
    } else {
        /* Restore slb info state in warmboot. */
        for (idx = 0; idx < slb_blk_ctrl[unit]->curr; idx++) {
            blk = slb_blk_ctrl[unit]->array[idx].blk;
            sid = slb_blk_ctrl[unit]->array[idx].sid;
            rbank = slb_blk_ctrl[unit]->array[idx].bank;
            tbl_inst = slb_blk_ctrl[unit]->array[idx].tbl_inst;
            SHR_IF_ERR_EXIT
                (rm_hash_slb_info_blk_restore(unit, sid, rbank, tbl_inst, blk));
        }
    }

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_pt_info_get(unit, &pt_info));
    if (pt_info->narrow_mode_supported == TRUE) {
        /* Allocate narrow mode entry bitmap structure. */
        req_size = sizeof(rm_hash_nme_bmp_blk_ctrl_t);
        alloc_size = req_size;
        nme_bmp_blk_ctrl[unit] = shr_ha_mem_alloc(unit,
                                              BCMMGMT_RM_HASH_COMP_ID,
                                              BCMPTM_HA_SUBID_RMHASH_NME_BMP_CTRL,
                                              "bcmptmRmhashNmeBmpCtrl",
                                              &alloc_size);
        SHR_NULL_CHECK(nme_bmp_blk_ctrl[unit], SHR_E_MEMORY);
        SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
        if (warm == FALSE) {
            sal_memset(nme_bmp_blk_ctrl[unit], 0, alloc_size);
            bcmissu_struct_info_report(unit,
                                       BCMMGMT_RM_HASH_COMP_ID,
                                       BCMPTM_HA_SUBID_RMHASH_NME_BMP_CTRL,
                                       0,
                                       req_size, 1,
                                       RM_HASH_NME_BMP_BLK_CTRL_T_ID);
        } else {
            /* Restore narrow mode entry bitmap in warmboot. */
            for (idx = 0; idx < nme_bmp_blk_ctrl[unit]->curr; idx++) {
                blk = nme_bmp_blk_ctrl[unit]->array[idx].blk;
                sid = nme_bmp_blk_ctrl[unit]->array[idx].sid;
                rbank = nme_bmp_blk_ctrl[unit]->array[idx].bank;
                tbl_inst = nme_bmp_blk_ctrl[unit]->array[idx].tbl_inst;
                SHR_IF_ERR_EXIT
                    (rm_hash_nme_bmp_blk_restore(unit, sid, rbank, tbl_inst, blk));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

void
bcmptm_rm_hash_blk_ctrl_cleanup(int unit)
{
    bkt_blk_ctrl[unit] = NULL;
    ent_blk_ctrl[unit] = NULL;
    slb_blk_ctrl[unit] = NULL;
    nme_bmp_blk_ctrl[unit] = NULL;
}
