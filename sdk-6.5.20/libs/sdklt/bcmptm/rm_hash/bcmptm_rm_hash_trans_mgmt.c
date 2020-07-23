/*! \file bcmptm_rm_hash_trans_mgmt.c
 *
 * Software State of The Resource Manager for Hash Table
 *
 * This file defines software objects that record transaction state of resource
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
#include <bcmptm/bcmptm.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmcfg/comp/bcmcfg_feature_ctl.h>
#include <bcmissu/issu_api.h>
#include "bcmptm_rm_hash.h"
#include "bcmptm_rm_hash_pt_register.h"
#include "bcmptm_rm_hash_state_mgmt.h"
#include "bcmptm_rm_hash_trans_mgmt.h"
#include "bcmptm_rm_hash_utils.h"
/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMHASH

#define RM_HASH_INIT_UNDO_CHUNK_SIZE           1024
#define RM_HASH_MAX_NUM_UNDO_CHUNKS            32

/*******************************************************************************
 * Typedefs
 */
/*! \brief Structure used for recording undo list memory chunks. */
typedef struct bcmptm_rm_hash_undo_chunk_s {
    /*! Pointer to undo HA memory chunk. */
    rm_hash_trans_undo_node_t *base[RM_HASH_MAX_NUM_UNDO_CHUNKS];
} bcmptm_rm_hash_undo_chunk_t;

/*******************************************************************************
 * Private variables
 */
static rm_hash_trans_record_t *dev_trans_rec[BCMDRD_CONFIG_MAX_UNITS];

static rm_hash_trans_undo_ctrl_t *trans_undo_ctrl[BCMDRD_CONFIG_MAX_UNITS];

static bcmptm_rm_hash_undo_chunk_t trans_unode_chunk[BCMDRD_CONFIG_MAX_UNITS];

static bool rm_hash_trans_undo_enable[BCMDRD_CONFIG_MAX_UNITS];

static bool rm_hash_dynamic_atomic_trans[BCMDRD_CONFIG_MAX_UNITS];
/*******************************************************************************
 * Private Functions
 */
/*!
 * \brief Clear static variables for recording HA memory buffer.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_state_elems_free(int unit)
{

    SHR_FUNC_ENTER(unit);

    dev_trans_rec[unit] = NULL;

    trans_undo_ctrl[unit] = NULL;

    sal_memset(&trans_unode_chunk[unit], 0, sizeof(trans_unode_chunk[unit]));

    rm_hash_trans_undo_enable[unit] = FALSE;

    rm_hash_dynamic_atomic_trans[unit] = FALSE;

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get transaction record of a device.
 *
 * \param [in] unit Unit number.
 * \param [out] ptrans_rec Pointer to rm_hash_trans_record_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_trans_record_get(int unit, rm_hash_trans_record_t **ptrans_rec)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ptrans_rec, SHR_E_PARAM);

    *ptrans_rec = dev_trans_rec[unit];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Invalidate the undo list for a device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_trans_undo_list_invalidate(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(trans_undo_ctrl[unit], SHR_E_PARAM);

    trans_undo_ctrl[unit]->full_chunk_cnt = 0;
    trans_undo_ctrl[unit]->used_node_cnt = 0;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Restore the undo list for a device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_trans_undo_list_restore(int unit)
{
    int idx = 0, slb_idx = 0, chunk_idx, last_node_idx, total_chunk_cnt = 0;
    rm_hash_trans_undo_node_t *undo_node = NULL, *trans_unode_base = NULL;
    rm_hash_ent_node_t  *e_node = NULL;
    uint32_t slb_info_offset = 0;
    rm_hash_pt_info_t *pt_info = NULL;
    rm_hash_ent_slb_info_t *e_slb_info = NULL;
    uint8_t rbank;
    rm_hash_nme_info_t nm_info = {0, 0};

    SHR_FUNC_ENTER(unit);

    total_chunk_cnt = trans_undo_ctrl[unit]->full_chunk_cnt + 1;
    last_node_idx  = trans_undo_ctrl[unit]->used_node_cnt - 1;

    for (chunk_idx = total_chunk_cnt - 1; chunk_idx >= 0; chunk_idx--) {
        if (chunk_idx != (total_chunk_cnt - 1)) {
            /* If the chunk is not the last one, restore each node in it. */
            last_node_idx = RM_HASH_INIT_UNDO_CHUNK_SIZE - 1;
        }
        trans_unode_base = trans_unode_chunk[unit].base[chunk_idx];
        for (idx = last_node_idx; idx >= 0; idx--) {
            undo_node = &trans_unode_base[idx];
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_pt_info_get(unit, &pt_info));
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_slb_to_rbank_get(unit, pt_info,
                                                 undo_node->b_state_offset,
                                                 &rbank));
            if (undo_node->opcode == BCMPTM_RM_HASH_UNDO_ENT_INSERT) {
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_ent_node_allocate(unit,
                                                      pt_info,
                                                      rbank,
                                                      undo_node->pipe,
                                                      undo_node->b_state_offset,
                                                      NULL,
                                                      &e_node));
                /* Record the slb offset of the entry node allocated. */
                slb_info_offset = e_node->ent_slb.slb_info_offset;
                *e_node = undo_node->e_node;
                e_node->flag |= RM_HASH_NODE_IN_USE;
                /* Other attributes will be assigned by source node. */
                e_node->ent_slb.slb_info_offset = slb_info_offset;
                /* Restore the SLB information. */
                bcmptm_rm_hash_ent_slb_info_get(unit,
                                                pt_info,
                                                rbank,
                                                undo_node->pipe,
                                                e_node->ent_slb.slb_info_offset,
                                                &e_slb_info);
                for (slb_idx = 0; slb_idx < undo_node->num_slb_info; slb_idx++) {
                    e_slb_info[slb_idx] = undo_node->slb_info[slb_idx];
                }
                e_node->ent_slb.num_slb_info = undo_node->num_slb_info;
                /* Entry node setting completed. Insert it into entry node list. */
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_ent_node_insert(unit,
                                                    pt_info,
                                                    rbank,
                                                    undo_node->pipe,
                                                    undo_node->b_state_offset,
                                                    &nm_info,
                                                    e_node));
            } else if (undo_node->opcode == BCMPTM_RM_HASH_UNDO_ENT_DELETE) {
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_ent_node_delete(unit,
                                                    pt_info,
                                                    rbank,
                                                    undo_node->pipe,
                                                    undo_node->b_state_offset,
                                                    undo_node->e_node.e_loc.bb_bmp,
                                                    undo_node->e_node.e_loc.be_bmp,
                                                    NULL,
                                                    &e_node));
            } else {
                SHR_ERR_EXIT(SHR_E_FAIL);
            }
        }
    }
    trans_undo_ctrl[unit]->full_chunk_cnt = 0;
    trans_undo_ctrl[unit]->used_node_cnt = 0;

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
bool
bcmptm_rm_hash_undo_list_maintained(int unit)
{
    return rm_hash_trans_undo_enable[unit] || rm_hash_dynamic_atomic_trans[unit];
}

int
bcmptm_rm_hash_trans_state_elems_alloc(int unit, bool warm)
{
    uint32_t req_size, alloc_size, num_chunks, chunk;
    bcmcfg_feature_ctl_config_t feature_conf;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /* Allocate transaction record. */
    req_size = sizeof(rm_hash_trans_record_t);
    alloc_size = req_size;
    dev_trans_rec[unit] = shr_ha_mem_alloc(unit,
                                           BCMMGMT_RM_HASH_COMP_ID,
                                           BCMPTM_HA_SUBID_RMHASH_TRANS_REC,
                                           "bcmptmRmhashTransRec",
                                           &alloc_size);
    SHR_NULL_CHECK(dev_trans_rec[unit], SHR_E_MEMORY);
    SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
    if (warm == FALSE) {
        sal_memset(dev_trans_rec[unit], 0, alloc_size);
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit,
                                        BCMMGMT_RM_HASH_COMP_ID,
                                        BCMPTM_HA_SUBID_RMHASH_TRANS_REC,
                                        0,
                                        req_size, 1,
                                        RM_HASH_TRANS_RECORD_T_ID));
    }

    /* Always allocate trans undo node control to support dynamic atomic trans. */
    req_size = sizeof(rm_hash_trans_undo_ctrl_t);
    alloc_size = req_size;
    trans_undo_ctrl[unit] = shr_ha_mem_alloc(unit,
                                             BCMMGMT_RM_HASH_COMP_ID,
                                             BCMPTM_HA_SUBID_RMHASH_UNDO_CTRL,
                                             "bcmptmRmhashUnodeCtrl",
                                             &alloc_size);
    SHR_NULL_CHECK(trans_undo_ctrl[unit], SHR_E_MEMORY);
    SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
    if (warm == FALSE) {
        sal_memset(trans_undo_ctrl[unit], 0, alloc_size);
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit,
                                        BCMMGMT_RM_HASH_COMP_ID,
                                        BCMPTM_HA_SUBID_RMHASH_UNDO_CTRL,
                                        0,
                                        req_size, 1,
                                        RM_HASH_TRANS_UNDO_CTRL_T_ID));
    }

    /* Always allocate at least a trans undo list to support dynamic atomic trans. */
    if (warm == FALSE) {
        req_size = RM_HASH_INIT_UNDO_CHUNK_SIZE * sizeof(rm_hash_trans_undo_node_t);
        alloc_size = req_size;
        trans_unode_chunk[unit].base[0] = shr_ha_mem_alloc(unit,
                                              BCMMGMT_RM_HASH_COMP_ID,
                                              BCMPTM_HA_SUBID_RMHASH_UNDO_CHUNKID_BASE,
                                              "bcmptmRmhashUndoList",
                                              &alloc_size);
        SHR_NULL_CHECK(trans_unode_chunk[unit].base[0], SHR_E_MEMORY);
        SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
        sal_memset(trans_unode_chunk[unit].base[0], 0, alloc_size);
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit,
                                              BCMMGMT_RM_HASH_COMP_ID,
                                              BCMPTM_HA_SUBID_RMHASH_UNDO_CHUNKID_BASE,
                                              0,
                                              sizeof(rm_hash_trans_undo_node_t),
                                              RM_HASH_INIT_UNDO_CHUNK_SIZE,
                                              RM_HASH_TRANS_UNDO_NODE_T_ID));
        trans_undo_ctrl[unit]->num_chunks = 1;
    } else {
        num_chunks = trans_undo_ctrl[unit]->num_chunks;
        /* Restore every undo node chunk. */
        for (chunk = 0; chunk < num_chunks; chunk++) {
            req_size = RM_HASH_INIT_UNDO_CHUNK_SIZE * sizeof(rm_hash_trans_undo_node_t);
            alloc_size = req_size;
            trans_unode_chunk[unit].base[chunk] = shr_ha_mem_alloc(unit,
                                              BCMMGMT_RM_HASH_COMP_ID,
                                              BCMPTM_HA_SUBID_RMHASH_UNDO_CHUNKID_BASE + chunk,
                                              "bcmptmRmhashUndoList",
                                              &alloc_size);
            SHR_NULL_CHECK(trans_unode_chunk[unit].base[chunk], SHR_E_MEMORY);
            SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
        }
    }

    rv = bcmcfg_feature_ctl_config_get(&feature_conf);
    if (SHR_FAILURE(rv) || (feature_conf.enable_atomic_trans == 0)) {
        rm_hash_trans_undo_enable[unit] = FALSE;
        /*
         * Record whether RM is in dynamic atomic transaction mode.
         * Default value is FALSE.
         */
        rm_hash_dynamic_atomic_trans[unit] = FALSE;
    } else {
        rm_hash_trans_undo_enable[unit] = TRUE;
    }

exit:
    if (SHR_FUNC_ERR()) {
        rm_hash_state_elems_free(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_trans_state_elems_config(int unit, bool warm)
{
    rm_hash_trans_record_t *trans_rec = NULL;

    SHR_FUNC_ENTER(unit);

    if (warm) {
        SHR_EXIT();
    }

    /* Clear transaction record in cold boot */
    SHR_IF_ERR_EXIT
        (rm_hash_trans_record_get(unit, &trans_rec));
    trans_rec->cseq_id = 0;
    trans_rec->trans_state = BCMPTM_RM_HASH_STATE_IDLE;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_trans_undo_node_update(int unit,
                                      rm_hash_pt_info_t *pt_info,
                                      uint32_t b_state_offset,
                                      rm_hash_ent_node_t *e_node)
{
    rm_hash_trans_undo_node_t *trans_unode = NULL, *trans_unode_base = NULL;
    int curr = 0, chunk_idx, total_chunk_cnt;
    uint8_t idx;
    rm_hash_ent_slb_t *src_e_slb = NULL, *dst_e_slb = NULL;
    uint32_t last_node_idx;

    SHR_FUNC_ENTER(unit);

    total_chunk_cnt = trans_undo_ctrl[unit]->full_chunk_cnt + 1;
    last_node_idx = trans_undo_ctrl[unit]->used_node_cnt - 1;

    for (chunk_idx = total_chunk_cnt - 1; chunk_idx >= 0; chunk_idx--) {
        if (chunk_idx != (total_chunk_cnt - 1)) {
            /* If the chunk is not the last one, restore each node in it. */
            last_node_idx = RM_HASH_INIT_UNDO_CHUNK_SIZE - 1;
        }
        trans_unode_base = trans_unode_chunk[unit].base[chunk_idx];
        for (curr = last_node_idx; curr >= 0; curr--) {
            trans_unode = &trans_unode_base[curr];
            if (trans_unode->b_state_offset == b_state_offset) {
                if ((trans_unode->e_node.e_loc.bb_bmp == e_node->e_loc.bb_bmp) &&
                    (trans_unode->e_node.e_loc.be_bmp == e_node->e_loc.be_bmp)) {
                    /* The undo node found. */
                    src_e_slb = &e_node->ent_slb;
                    dst_e_slb = &trans_unode->e_node.ent_slb;
                    for (idx = 0; idx < e_node->ent_slb.num_rbanks; idx++) {
                        dst_e_slb->e_node_idx[idx] = src_e_slb->e_node_idx[idx];
                    }
                    SHR_EXIT();
                }
            }
        }
    }
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_trans_undo_node_insert(int unit,
                                      rm_hash_pt_info_t *pt_info,
                                      uint32_t b_state_offset,
                                      uint8_t pipe,
                                      rm_hash_ent_node_t *e_node,
                                      rm_hash_trans_undo_opcode_t opcode)
{
    rm_hash_trans_undo_node_t *trans_unode_base = NULL;
    uint32_t curr = 0, curr_chunk = 0, full_chunk_cnt, num_chunks;
    rm_hash_ent_slb_info_t *e_slb_info = NULL;
    uint8_t idx, rbank;
    uint32_t req_size, alloc_size;

    SHR_FUNC_ENTER(unit);

    if (trans_undo_ctrl[unit]->used_node_cnt >= RM_HASH_INIT_UNDO_CHUNK_SIZE) {
        /* There is no space in the current chunk. */
        full_chunk_cnt = trans_undo_ctrl[unit]->full_chunk_cnt;
        num_chunks = trans_undo_ctrl[unit]->num_chunks;

        if ((full_chunk_cnt + 1) < num_chunks) {
            /* Some additional chunks have been allocated already. */
            trans_undo_ctrl[unit]->full_chunk_cnt++;
            trans_undo_ctrl[unit]->used_node_cnt = 0;
        } else {
            /* Some additional chunks are required. */
            if (num_chunks >= RM_HASH_MAX_NUM_UNDO_CHUNKS) {
                SHR_ERR_EXIT(SHR_E_MEMORY);
            }
            req_size = RM_HASH_INIT_UNDO_CHUNK_SIZE * sizeof(rm_hash_trans_undo_node_t);
            alloc_size = req_size;
            trans_unode_chunk[unit].base[num_chunks] = shr_ha_mem_alloc(unit,
                                              BCMMGMT_RM_HASH_COMP_ID,
                                              BCMPTM_HA_SUBID_RMHASH_UNDO_CHUNKID_BASE + num_chunks,
                                              "bcmptmRmhashUndoList",
                                              &alloc_size);
            SHR_NULL_CHECK(trans_unode_chunk[unit].base[num_chunks], SHR_E_MEMORY);
            SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
            sal_memset(trans_unode_chunk[unit].base[num_chunks], 0, alloc_size);
            SHR_IF_ERR_EXIT
                (bcmissu_struct_info_report(unit,
                                                  BCMMGMT_RM_HASH_COMP_ID,
                                                  BCMPTM_HA_SUBID_RMHASH_UNDO_CHUNKID_BASE + num_chunks,
                                                  0,
                                                  sizeof(rm_hash_trans_undo_node_t),
                                                  RM_HASH_INIT_UNDO_CHUNK_SIZE,
                                                  RM_HASH_TRANS_UNDO_NODE_T_ID));
            trans_undo_ctrl[unit]->num_chunks++;
            trans_undo_ctrl[unit]->full_chunk_cnt++;
            trans_undo_ctrl[unit]->used_node_cnt = 0;
        }
    }
    curr_chunk = trans_undo_ctrl[unit]->full_chunk_cnt;
    trans_unode_base = trans_unode_chunk[unit].base[curr_chunk];
    curr = trans_undo_ctrl[unit]->used_node_cnt;
    /*
     * opcode records when this node in the undo list is executed,
     * the operation will be performed, either insert or delete.
     */
    trans_unode_base[curr].opcode = opcode;
    /*
     * b_state_offset records when this node in the undo list is executed,
     * the software logical bucket within which the op will be performed.
     */
    trans_unode_base[curr].b_state_offset = b_state_offset;
    /*
     * e_node records when this node in the undo list is executed,
     * the information of the entry on which the undo operation will be
     * performed. The most important portion is SID from which the pt
     * information is able to be retrieved.
     */
    trans_unode_base[curr].pipe = pipe;
    trans_unode_base[curr].e_node = *e_node;
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_slb_to_rbank_get(unit, pt_info,
                                         b_state_offset,
                                         &rbank));
    bcmptm_rm_hash_ent_slb_info_get(unit,
                                    pt_info,
                                    rbank,
                                    pipe,
                                    e_node->ent_slb.slb_info_offset,
                                    &e_slb_info);
    /*
     * Record the SLB list of the entry into undo list node. When the node
     * is executed, the information about SLB list can be restored into
     * the new entry node if the undo operation is INSERT.
     */
    for (idx = 0; idx < e_node->ent_slb.num_slb_info; idx++) {
        trans_unode_base[curr].slb_info[idx] = e_slb_info[idx];
    }
    trans_unode_base[curr].num_slb_info = e_node->ent_slb.num_slb_info;
    trans_undo_ctrl[unit]->used_node_cnt++;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_trans_ent_req_cmd(int unit,
                                 uint32_t cseq_id)
{
    rm_hash_trans_record_t *trans_rec = NULL;

    SHR_FUNC_ENTER(unit);

    if (bcmptm_rm_hash_undo_list_maintained(unit) == FALSE) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (rm_hash_trans_record_get(unit, &trans_rec));
    switch (trans_rec->trans_state) {
    case BCMPTM_RM_HASH_STATE_IDLE:
        trans_rec->trans_state = BCMPTM_RM_HASH_STATE_UC_A;
        trans_rec->cseq_id = cseq_id;
        break;
    case BCMPTM_RM_HASH_STATE_UC_A:
        if (trans_rec->cseq_id != cseq_id) {
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
        break;
    default:
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_trans_state_elems_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(rm_hash_state_elems_free(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_trans_cmd(int unit,
                         uint64_t flags,
                         uint32_t cseq_id,
                         bcmptm_trans_cmd_t trans_cmd)
{
    rm_hash_trans_record_t *trans_rec = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (rm_hash_trans_record_get(unit, &trans_rec));
    if (!trans_rec) {
        SHR_EXIT(); /* RM is not initialized */
    }

    switch (trans_cmd) {
        case BCMPTM_TRANS_CMD_COMMIT:
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Hash rsrc_mgr got Commit \n")));
            if (trans_rec->trans_state == BCMPTM_RM_HASH_STATE_IDLE) {
                /*
                 * If transaction state is in IDLE and rm receives commit command,
                 * it stays in IDLE state and ignores cseq_id.
                 */
                 SHR_EXIT();
            } else if ((trans_rec->trans_state == BCMPTM_RM_HASH_STATE_UC_A) ||
                       (trans_rec->trans_state == BCMPTM_RM_HASH_STATE_INVALIDATING_U)) {
                /*
                 * If transaction state is in UC_A and it receives commit command,
                 * it checks if the cseq_id is expected or not. If yes, it commits
                 * the changes to sw_state and goes to IDLE.
                 */
                if (cseq_id == trans_rec->cseq_id) {
                    trans_rec->trans_state = BCMPTM_RM_HASH_STATE_INVALIDATING_U;
                    if (bcmptm_rm_hash_undo_list_maintained(unit)) {
                        SHR_IF_ERR_EXIT
                            (rm_hash_trans_undo_list_invalidate(unit));
                    }
                    trans_rec->trans_state = BCMPTM_RM_HASH_STATE_IDLE;
                } else {
                    /* If the cseq_id is not expected, then ignore the commit */
                    SHR_EXIT();
                }
            } else {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            break;
        case BCMPTM_TRANS_CMD_ABORT:
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Hash rsrc_mgr got Abort \n")));
            if (trans_rec->trans_state == BCMPTM_RM_HASH_STATE_IDLE) {
                /*
                 * If transaction state is in IDLE and rm receives abort command,
                 * it stays in IDLE state and ignores cseq_id.
                 */
                 SHR_EXIT();
            } else if ((trans_rec->trans_state == BCMPTM_RM_HASH_STATE_UC_A) ||
                       (trans_rec->trans_state == BCMPTM_RM_HASH_STATE_RESTORING_U)) {
                /*
                 * If transaction state is in UC_A and rm receives abort command,
                 * it checks if the cseq_id is expected or not. If not, declare
                 * SHR_E_FAIL. If yes, execute the operations in the undo list.
                 */
                if (cseq_id == trans_rec->cseq_id) {
                    trans_rec->trans_state = BCMPTM_RM_HASH_STATE_RESTORING_U;
                    if (bcmptm_rm_hash_undo_list_maintained(unit)) {
                        SHR_IF_ERR_EXIT
                            (rm_hash_trans_undo_list_restore(unit));
                    }
                    trans_rec->trans_state = BCMPTM_RM_HASH_STATE_IDLE;
                } else {
                    SHR_ERR_EXIT(SHR_E_FAIL);
                }
            }
            break;
        case BCMPTM_TRANS_CMD_IDLE_CHECK:
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Hash rsrc_mgr got Idle_Check \n")));
            if (trans_rec->trans_state == BCMPTM_RM_HASH_STATE_IDLE) {
                SHR_EXIT();
            } else {
                SHR_ERR_EXIT(SHR_E_FAIL);
            }
            break;
        case BCMPTM_TRANS_CMD_ATOMIC_TRANS_ENABLE:
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Hash rsrc_mgr got atomic trans enable \n")));
            rm_hash_dynamic_atomic_trans[unit] = TRUE;
            SHR_EXIT();
        case BCMPTM_TRANS_CMD_ATOMIC_TRANS_DISABLE:
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Hash rsrc_mgr got atomic trans disable \n")));
            rm_hash_dynamic_atomic_trans[unit] = FALSE;
            SHR_EXIT();
        default:
            LOG_ERROR(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "Hash rsrc_mgr got Unknown trans_cmd \n")));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
            break;
    }

    /* Rsrc_mgr starts in IDLE state.
     * In IDLE state, ptrm_state_cseq_id is dont_care.
     * - if it gets commit()
     *          it stays in IDLE state. Ignores cseq_id.
     *
     * - if it gets abort()
     *          stays in IDLE state. Ignores cseq_id.
     *
     * - if it gets bcmptm_rm_hash_req()
     *          goes into CHANGED state and
     *          remembers cseq_id as ptrm_state_cseq_id
     *
     * - if it gets idle_check:
     *          stays in IDLE state and returns SHR_E_NONE.
     *          cseq_id should be ignored
     *
     * In CHANGED state:
     * - if it gets bcmptm_rm_hash_req()
     *          Must declare SHR_E_FAIL if cseq_id does not match ptrm_state_cseq_id
     *          Remain in CHANGED state.
     *
     * - if it gets commit()
     *          if cseq_id != ptrm_state_cseq_id, ignore commit.
     *          Else, it commits the changes to sw_state and goes to IDLE
     *
     * - if it gets abort()
     *          if cseq_id != ptrm_state_cseq_id, declare SHR_E_FAIL
     *          Else, it aborts the changes to sw_state and goes to IDLE
     *
     * - if it gets idle_check()
     *          it stays in CHANGED state and returns SHR_E_FAIL
     *          (cseq_id should be ignored)
     *
     * PTRM_state_cseq_id must be in HA space - just like rest of PTRM SW_state
 */

exit:
    SHR_FUNC_EXIT();
}
