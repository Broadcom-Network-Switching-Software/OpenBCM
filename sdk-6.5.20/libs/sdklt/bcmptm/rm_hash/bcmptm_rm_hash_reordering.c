/*! \file bcmptm_rm_hash_reordering.c
 *
 * Non-recurrsive Hash Reordering for Hash Table Resource management
 *
 * This file contains the resource manager for physical hash tables
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
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include "bcmptm_rm_hash.h"
#include "bcmptm_rm_hash_state_mgmt.h"
#include "bcmptm_rm_hash_trans_mgmt.h"
#include "bcmptm_rm_hash_utils.h"
#include "bcmptm_rm_hash_reordering.h"
#include "bcmptm_rm_hash_lmem.h"
#include "bcmptm_rm_hash_lt_ctrl.h"
#include "bcmptm_rm_hash_pt_register.h"
#include "bcmptm_rm_hash_hit_mgmt.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMHASH
#define RM_HASH_MAX_ENTRY_NUM_PER_BUCKET           16
#define RM_HASH_MAX_DST_BUCKET_NUM ((RM_HASH_MAX_RSRC_MAP_COUNT - 1) * RM_HASH_MAX_ENTRY_NUM_PER_BUCKET)
#define RM_HASH_ENTRY_MOVE_LIST_DEF_NUM           256
#define RM_HASH_ENTRY_MOVE_LIST_INC_NUM           100

/*******************************************************************************
 * Typedefs
 */
/*!
 * \brief Information about a potential entry move item.
 *
 * This data structure describes the sequence number of an entry and
 * its SLB (software logical bucket), and the valid base bucket bitmap in
 * the SLB.
 */
typedef struct rm_hash_ptnl_move_info_s {
    /*! \brief Sequence number in the entry node list of a slb. */
    uint8_t e_seq;

    /*! \brief SLB info. */
    rm_hash_ent_slb_info_t slb_info;

    /*! \brief Entry node. */
    rm_hash_ent_node_t *e_node;
} rm_hash_ptnl_move_info_t;

/*!
 * \brief Information about an entry which is to be moved.
 *
 * This data structure describes an entry which is to be moved within its valid
 * buckets. It contains the attributes of an entry involved in the entry move.
 * To be more specific, it contains the bucket mode of the entry, the base
 * bucket bitmap indicating which base bucket this entry resides currently,
 * the base entry and logical entry bitmap within the base bucket. And it also
 * contains the size of the entry.
 */
typedef struct rm_hash_move_ent_info_s {
    /*! \brief Bucket mode of the entry */
    rm_hash_bm_t bm;

    /*! \brief Location of the entry */
    rm_hash_ent_loc_t e_loc;

    /*! \brief Size of the entry */
    uint8_t e_size;

    /*! \brief Entry node */
    rm_hash_ent_node_t *e_node;
} rm_hash_move_ent_info_t;

/*!
 * \brief Type of an entry move operation.
 *
 * This data structure describes if an entry move operation is between two
 * single-element groups, from a single-element group to a multiple-element
 * group, from a multiple-element group to a single-element group or between
 * multiple-element group.
 */
typedef enum rm_hash_move_op_type_e {
    /*! \brief Unknown type move. */
    RM_HASH_UNKNOWN_MOVE = 0,

    /*! \brief Move between two single element group. */
    RM_HASH_SEGRP_TO_SEGRP_MOVE,

    /*! \brief Move from single element group to multiple elements group. */
    RM_HASH_SEGRP_TO_MEGRP_MOVE,

    /*! \brief Move from multiple element group to single element group. */
    RM_HASH_MEGRP_TO_SEGRP_MOVE,

    /*! \brief Move from multiple element group to multiple element group. */
    RM_HASH_MEGRP_TO_MEGRP_MOVE
} rm_hash_move_op_type_t;

/*!
 * \brief Opcode of an entry move operation.
 *
 * This data structure describes if an entry move operation is essentially
 * moving an entry from one location to another or an entry clear operation.
 */
typedef enum rm_hash_move_opcode_e {
    /*! \brief Entry move. */
    RM_HASH_ENTRY_MOVE = 0,

    /*! \brief Entry clear. Used in MEGRP_TO_SEGRP move. */
    RM_HASH_ENTRY_CLEAR,

    /*! \brief Entry delete. Used in MEGRP_TO_MEGRP move.
     * To delete the entry in the 2nd bank.
     */
    RM_HASH_ENTRY_DELETE,

    /*! \brief Entry duplicate. */
    RM_HASH_ENTRY_DUPLICATE,
} rm_hash_move_opcode_t;

/*!
 * \brief Information about an entry move operation.
 *
 * This data structure describes an entry move operation. For example
 * the source bank and destination bank, the source SID and destination
 * bank of the move, etc.
 */
typedef struct rm_hash_ent_move_op_s {
    /*! \brief Source SID of the entry to be moved. */
    bcmdrd_sid_t s_sid;

    /*! \brief Destination SID of the entry to be moved. */
    bcmdrd_sid_t d_sid;

    /*! \brief Number of base entries used by the entry to be moved.*/
    uint8_t e_size;

    /*! \brief Entry bucket mode. */
    rm_hash_bm_t e_bm;

    /*! \brief Source SLB where the entry will be moved from. */
    rm_hash_ent_slb_info_t s_slb_info;

    /*! \brief Destination SLB where the entry will be moved to. */
    rm_hash_ent_slb_info_t d_slb_info;

    /*! \brief Source entry location info. */
    rm_hash_ent_loc_t se_loc;

    /*! \brief Destination entry location info. */
    rm_hash_ent_loc_t de_loc;

    /*! \brief Source bank. */
    uint8_t s_rbank;

    /*! \brief Destination bank. */
    uint8_t d_rbank;

    /*! \brief Type of the move. */
    rm_hash_move_op_type_t move_op_type;

    /*! \brief Opcode of the move. */
    rm_hash_move_opcode_t opcode;

    /*! \brief Parent move. */
    int parent_offset;

    /*! \brief Destination SLB sequence. */
    uint8_t d_slb_seq;

    /*! \brief Destination counter index. */
    uint32_t dst_ctr_index;

    /*! \brief Source counter index. */
    uint32_t src_ctr_index;

    /*! \brief Destination entry node. */
    rm_hash_ent_node_t *d_e_node;

    /*! \brief Entry flag. */
    uint8_t flag;

    /*! \brief Indicates if the move eventually provide the free space */
    bool slot_provider;
} rm_hash_ent_move_op_t;

/*!
 * \brief Information about the stack frame used for analyzing the entries.
 *
 * This data structure describes the elements in a stack frame. A stack frame
 * must correspond to a SLB.
 */
typedef struct rm_hash_ent_move_stack_s {
    /*! \brief The bkt that needs to be analyzed. Either it can provide some free
     * space to accomadate the moved entry coming from the last stack frame, or
     * it will have to select one of its entry to move outside.
     */
    uint32_t slb;

    /* \brief Bank of the slb. */
    uint8_t rbank;

    /*! \brief Info about requested entry. It describes the free space
     * requirement that the specified SLB on the stack frame should provide.
     */
    rm_hash_req_ent_info_t req_e_info;

    /*! \brief Number of eligible potential entry move operations. */
    uint8_t num_epm;

    /*! \brief Eligible potential entry move operations list. */
    rm_hash_ptnl_move_info_t epm_info[RM_HASH_MAX_DST_BUCKET_NUM];

    /*! \brief Index of next eligible potential entry move operation. */
    uint8_t curr_epm_idx;

    /*! \brief Actual SLB entry bitmap plus applying the pending move ops. */
    rm_hash_slb_ent_bmp_t e_bmp;

    /*! \brief The info regarding the entry to be moved out. */
    rm_hash_move_ent_info_t move_ent;

    /*! \brief Indicates if this SLB actually provide the space eventually. */
    bool  slot_provider;
} rm_hash_ent_move_stack_t;

/*!
 * \brief Information about entry move operation list
 *
 * This data structure describes the information about the entry move info list.
 */
typedef struct rm_hash_ent_move_op_list_s {
    /*! \brief Indicates the start address of the entry move info list. */
    rm_hash_ent_move_op_t *e_move_op;

    /*! \brief Indicates the count of entry move items currently. */
    uint32_t count;

    /*! \brief Indicates if multiple instances mapping group involved. */
    bool multi_minst_involved;
} rm_hash_ent_move_op_list_t;

/*******************************************************************************
 * Private variables
 */
static rm_hash_ent_move_op_list_t e_move_op_list[BCMDRD_CONFIG_MAX_UNITS];

static uint32_t e_move_num[BCMDRD_CONFIG_MAX_UNITS];

static rm_hash_ent_move_stack_t ent_move_stack[BCMDRD_CONFIG_MAX_UNITS][BCMPTM_RM_HASH_MAX_HASH_REORDERING_DEPTH + 1];

/*******************************************************************************
 * Private Functions
 */
/*!
 * \brief Update entry move operation type info.
 *
 * \param [in] unit Unit number.
 * \param [in] move_op Pointer to rm_hash_ent_move_op_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static void
rm_hash_ent_move_op_type_update(int unit,
                                rm_hash_ent_move_op_t *move_op)
{
    bool src_multi_inst = FALSE, dst_multi_inst = FALSE;

    if (move_op->s_slb_info.map & RM_HASH_MULTI_INST_GRP) {
        src_multi_inst = TRUE;
    }
    if (move_op->d_slb_info.map & RM_HASH_MULTI_INST_GRP) {
        dst_multi_inst = TRUE;
    }

    if (src_multi_inst == TRUE) {
        if (dst_multi_inst == TRUE) {
            move_op->move_op_type = RM_HASH_MEGRP_TO_MEGRP_MOVE;
        } else {
            move_op->move_op_type = RM_HASH_MEGRP_TO_SEGRP_MOVE;
        }
    } else {
        if (dst_multi_inst == TRUE) {
            move_op->move_op_type = RM_HASH_SEGRP_TO_MEGRP_MOVE;
        } else {
            move_op->move_op_type = RM_HASH_SEGRP_TO_SEGRP_MOVE;
        }
    }
}

/*!
 * \brief Save the information about an entry move operation into a move node.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] s_slb Source SLB of the entry to be moved.
 * \param [in] se_loc Source entry location in the source SLB.
 * \param [in] d_slb Destination SLB of the entry to be moved.
 * \param [in] de_loc Destination entry location in the destination SLB.
 * \param [in] s_rbank Source bank of the entry move.
 * \param [in] d_rbank Destination bank of the entry move.
 * \param [in] e_size Size of the entry to be moved.
 * \param [in] e_bm Bucket mode of the entry.
 * \param [in] op_type Entry move type.
 * \param [in] opcode Opcode of the entry move.
 * \param [in] parent Pointer to rm_hash_ent_move_op_s structure.
 * \param [in] d_slb_seq SLB sequence number.
 * \param [in] slot_provider Flag indicating if this move provides the real
 *             space.
 * \param [in] flag Flag of the entry to be moved.
 * \param [out] move_op pointer to rm_hash_ent_move_op_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static void
rm_hash_ent_move_op_node_create(int unit,
                                rm_hash_pt_info_t *pt_info,
                                rm_hash_ent_slb_info_t s_slb_info,
                                rm_hash_ent_loc_t se_loc,
                                rm_hash_ent_slb_info_t d_slb_info,
                                rm_hash_ent_loc_t de_loc,
                                uint8_t s_rbank,
                                uint8_t d_rbank,
                                uint8_t e_size,
                                rm_hash_bm_t e_bm,
                                rm_hash_move_op_type_t op_type,
                                rm_hash_move_opcode_t opcode,
                                int parent_offset,
                                uint8_t d_slb_seq,
                                bool slot_provider,
                                uint8_t flag,
                                rm_hash_ent_move_op_t *move_op)
{
    bcmdrd_sid_t ssid, dsid;

    ssid = pt_info->sid_list[s_rbank].view_sid[e_size];
    dsid = pt_info->sid_list[d_rbank].view_sid[e_size];

    sal_memset(move_op, 0, sizeof(rm_hash_ent_move_op_t));

    move_op->s_slb_info = s_slb_info;
    move_op->se_loc = se_loc;
    move_op->s_sid = ssid;
    move_op->d_slb_info = d_slb_info;
    move_op->de_loc = de_loc;
    move_op->d_sid = dsid;
    move_op->s_rbank = s_rbank;
    move_op->d_rbank = d_rbank;
    move_op->slot_provider = slot_provider;
    move_op->e_size = e_size;
    move_op->e_bm = e_bm;
    move_op->opcode = opcode;
    move_op->move_op_type = op_type;
    move_op->parent_offset = parent_offset;
    move_op->d_slb_seq = d_slb_seq;
    move_op->flag = flag;

    if (op_type == RM_HASH_UNKNOWN_MOVE) {
        rm_hash_ent_move_op_type_update(unit, move_op);
    }
}

/*!
 * \brief Save the information about an entry move operation into the array of
 * the device.
 *
 * \param [in] unit Unit number.
 * \param [in] src_move_op Pointer to rm_hash_ent_move_op_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_move_op_set(int unit,
                        rm_hash_ent_move_op_t *src_move_op)
{
    uint32_t move_no = e_move_num[unit];
    uint32_t list_len = e_move_op_list[unit].count;
    rm_hash_ent_move_op_t *move_op = NULL;
    size_t info_size = 0;

    SHR_FUNC_ENTER(unit);

    if (move_no >= list_len) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "hash_entry_move_list for unit %d is full.\n"),
                                unit));
        /*
         * There is no free space in the existing move info list.
         * Some extra space is required.
         */
        info_size = sizeof(rm_hash_ent_move_op_t) *
                    (list_len + RM_HASH_ENTRY_MOVE_LIST_INC_NUM);
        move_op = sal_alloc(info_size, "bcmptmRmhashEntryMoveInfo");
        if (move_op == NULL) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        /*
         * Copy existing entry move info from the existing list to the
         * new one.
         */
        sal_memcpy(move_op,
                   e_move_op_list[unit].e_move_op,
                   sizeof(rm_hash_ent_move_op_t) * list_len);
        sal_free(e_move_op_list[unit].e_move_op);
        e_move_op_list[unit].e_move_op = move_op;
        e_move_op_list[unit].count += RM_HASH_ENTRY_MOVE_LIST_INC_NUM;
    }

    move_op = e_move_op_list[unit].e_move_op + move_no;
    *move_op = *src_move_op;
    e_move_num[unit]++;

    if ((src_move_op->s_slb_info.map & RM_HASH_MULTI_INST_GRP) ||
        (src_move_op->d_slb_info.map & RM_HASH_MULTI_INST_GRP)) {
        e_move_op_list[unit].multi_minst_involved = TRUE;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the entry move operation from the pending move list.
 *
 * \param [in] unit Unit number.
 * \param [in] move_idx Index into the pending move list.
 * \param [out] pmove_info Pointer to the buffer for recording the address
 *              of rm_hash_ent_move_info_t object.
 *
 * \retval TRUE The entry move information has been got successfully.
 * \retval FALSE The entry move information has not been got.
 */
static bool
rm_hash_ent_move_op_get(int unit,
                        uint32_t move_idx,
                        rm_hash_ent_move_op_t **pmove_op)
{
    if (move_idx >= e_move_num[unit]) {
        return FALSE;
    }
    *pmove_op = &e_move_op_list[unit].e_move_op[move_idx];
    return TRUE;
}

/*!
 * \brief Get the number of entry move operations stored in the operation list.
 *
 * \param [in] unit Unit number.
 *
 * \retval The number of pending entry move
 */
static uint32_t
rm_hash_ent_move_num_get(int unit)
{
    return e_move_num[unit];
}

/*!
 * \brief Clear the entry move info list.
 *
 * \param [in] unit Unit number.
 */
static void
rm_hash_ent_move_info_clear(int unit)
{
    e_move_num[unit] = 0;
    e_move_op_list[unit].multi_minst_involved = FALSE;
}

/*!
 * \brief Report an entry move event during hash reordering.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] tbl_inst Physical table pipe instance.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] move_op Pointer to rm_hash_ent_move_op_t structure.
 * \param [in] dst_entry_index Destination physical entry index.
 * \param [in] src_entry_index Source physical entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_move_report(int unit, bcmltd_sid_t ltid,
                        int tbl_inst,
                        rm_hash_pt_info_t *pt_info,
                        rm_hash_ent_move_op_t *move_op,
                        uint32_t dst_entry_index,
                        uint32_t src_entry_index)
{
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    rm_hash_lt_flex_ctr_info_t *fc_info = NULL;
    uint8_t idx, lookup_idx;
    uint32_t dst_ctr_index[RM_HASH_LOOKUP_MAX_CNT];
    uint32_t src_ctr_index[RM_HASH_LOOKUP_MAX_CNT];
    rm_hash_ent_move_op_t *parent_move_op = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_lt_ctrl_get(unit, ltid, &lt_ctrl));
    fc_info = &lt_ctrl->flex_ctr_info;
    if ((fc_info->num_lookups == 0) || (fc_info->entry_index_cb == NULL)) {
        SHR_EXIT();
    }

    switch (move_op->move_op_type) {
    case RM_HASH_SEGRP_TO_SEGRP_MOVE:
    case RM_HASH_MEGRP_TO_SEGRP_MOVE:
        /* Construct destination counter index. */
        for (idx = 0; idx < fc_info->num_lookups; idx++) {
            if (fc_info->lookup_entry_move_reporting[idx] == TRUE) {
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_flex_ctr_offset_get(unit,
                                                        lt_ctrl,
                                                        move_op->d_sid,
                                                        tbl_inst,
                                                        dst_entry_index,
                                                        move_op->e_size,
                                                        &dst_ctr_index[idx]));
            } else {
                dst_ctr_index[idx] = BCMPTM_INVALID_HIT_INDEX;
            }
        }
        /* Construct source counter index. */
        for (idx = 0; idx < fc_info->num_lookups; idx++) {
            if (fc_info->lookup_entry_move_reporting[idx] == TRUE) {
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_flex_ctr_offset_get(unit,
                                                        lt_ctrl,
                                                        move_op->s_sid,
                                                        tbl_inst,
                                                        src_entry_index,
                                                        move_op->e_size,
                                                        &src_ctr_index[idx]));
            } else {
                src_ctr_index[idx] = BCMPTM_INVALID_HIT_INDEX;
            }
        }
        bcmptm_rm_hash_move_event_notify(unit, ltid, tbl_inst, fc_info->num_lookups,
                                         fc_info->context,
                                         dst_ctr_index, src_ctr_index);
        break;
    case RM_HASH_MEGRP_TO_MEGRP_MOVE:
    case RM_HASH_SEGRP_TO_MEGRP_MOVE:
        lookup_idx = move_op->d_slb_seq;
        if (fc_info->lookup_entry_move_reporting[lookup_idx] == TRUE) {
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_flex_ctr_offset_get(unit,
                                                    lt_ctrl,
                                                    move_op->d_sid,
                                                    tbl_inst,
                                                    dst_entry_index,
                                                    move_op->e_size,
                                                    &move_op->dst_ctr_index));
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_flex_ctr_offset_get(unit,
                                                    lt_ctrl,
                                                    move_op->s_sid,
                                                    tbl_inst,
                                                    src_entry_index,
                                                    move_op->e_size,
                                                    &move_op->src_ctr_index));
        } else {
            move_op->dst_ctr_index = BCMPTM_INVALID_HIT_INDEX;
            move_op->src_ctr_index = BCMPTM_INVALID_HIT_INDEX;
        }
        if (move_op->parent_offset != 0) {
            /* This is the 2nd entry move op. It is time to report the event. */
            parent_move_op = move_op - move_op->parent_offset;
            dst_ctr_index[0] = parent_move_op->dst_ctr_index;
            dst_ctr_index[1] = move_op->dst_ctr_index;
            src_ctr_index[0] = parent_move_op->src_ctr_index;
            src_ctr_index[1] = move_op->src_ctr_index;
            bcmptm_rm_hash_move_event_notify(unit, ltid,
                                             tbl_inst,
                                             fc_info->num_lookups,
                                             fc_info->context,
                                             dst_ctr_index,
                                             src_ctr_index);
        }
        break;
    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear the entry move operation list.
 *
 * \param [in] unit Unit number.
 * \param [in] slb Software logical bucket.
 * \param [in] depth The depth of hash reordering.
 */
static void
rm_hash_ent_move_trace_clear(int unit,
                             uint32_t slb,
                             int depth)
{
    int idx = 0;
    rm_hash_ent_move_op_t *move_op = NULL, *prev_move_op = NULL;

    if (e_move_num[unit] == 0) {
        return;
    }
    idx = e_move_num[unit] - 1;
    move_op = &e_move_op_list[unit].e_move_op[idx];
    if (move_op->s_slb_info.slb != slb) {
        return;
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Need to clear move path of SLB[%d], "
                            "operation number [%d]\n"), slb, idx));
    for (; idx > 0; idx--) {
        move_op = &e_move_op_list[unit].e_move_op[idx];
        prev_move_op = &e_move_op_list[unit].e_move_op[idx - 1];
        if (move_op->slot_provider == TRUE) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Clear movement operation[%d] "
                                    "source bkt[%d], destination bkt[%d]\n"),
                         idx, move_op->s_slb_info.slb,
                         move_op->d_slb_info.slb));
            sal_memset(move_op, 0, sizeof(rm_hash_ent_move_op_t));
            if (prev_move_op->s_slb_info.slb != slb) {
                break;
            } else {
                continue;
            }
        }
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Clear movement operation[%d] source bkt[%d], "
                                "destination bkt[%d]\n"),
                     idx, move_op->s_slb_info.slb, move_op->d_slb_info.slb));
        sal_memset(move_op, 0, sizeof(rm_hash_ent_move_op_t));
    }
    e_move_num[unit] = idx;
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "push iteration depth [%d]: all destination bkts "
                            "of slb [%d] were traversed\n"), depth, slb));
}

/*!
 * \brief Determine if the given SLB is eligible for analysis.
 *
 * \param [in] slb Software logical bucket.
 * \param [in] depth Depth to be tried.
 * \param [in] stack_base Stack base address.
 */
static bool
rm_hash_move_info_is_valid(uint32_t slb,
                           int depth,
                           rm_hash_ent_move_stack_t *stack_base)
{
    int layer;

    for (layer = 0; layer < depth; layer++) {
        if (slb == stack_base[layer].slb) {
            return FALSE;
        }
    }
    return TRUE;
}

/*!
 * \brief Determine if the given SLB appears in the pending move list.
 *
 * \param [in] unit Device unit.
 * \param [in] slb Software logical bucket.
 */
static bool
rm_hash_slb_is_pending_move(int unit, uint32_t slb)
{
    uint32_t idx = 0;
    rm_hash_ent_move_op_t *move_op = NULL;

    for (idx = 0; idx < e_move_num[unit]; idx++) {
        move_op = &e_move_op_list[unit].e_move_op[idx];
        /*
         * If there is a match, that means it is possible that the free
         * slb is due to the move.
         */
        if (slb == move_op->s_slb_info.slb) {
            return TRUE;
        }
    }

    return FALSE;
}

/*!
 * \brief Insert a new move op into the pending list.
 *
 * \param [in] unit Device unit.
 * \param [in] index Index in the pending list to insert the node.
 * \param [in] move_op The element to be inserted.
 */
static int
rm_hash_ent_move_info_insert(int unit,
                             int index,
                             rm_hash_ent_move_op_t *move_op_node)
{
    int idx;
    uint32_t move_no = e_move_num[unit];
    uint32_t list_len = e_move_op_list[unit].count;
    rm_hash_ent_move_op_t *move_op = NULL;
    size_t info_size = 0;

    SHR_FUNC_ENTER(unit);

    if (move_no >= list_len) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "hash_entry_move_list for unit %d is full.\n"),
                                unit));
        /*
         * There is no free space in the existing move info list.
         * Some extra space is required.
         */
        info_size = sizeof(rm_hash_ent_move_op_t) *
                    (list_len + RM_HASH_ENTRY_MOVE_LIST_INC_NUM);
        move_op = sal_alloc(info_size, "bcmptmRmhashEntryMoveInfo");
        if (move_op == NULL) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        /*
         * Copy existing entry move info from the existing list to the
         * new one.
         */
        sal_memcpy(move_op,
                   e_move_op_list[unit].e_move_op,
                   sizeof(rm_hash_ent_move_op_t) * list_len);
        sal_free(e_move_op_list[unit].e_move_op);
        e_move_op_list[unit].e_move_op = move_op;
        e_move_op_list[unit].count += RM_HASH_ENTRY_MOVE_LIST_INC_NUM;
    }
    for (idx = move_no - 1; idx >= index; idx--) {
        e_move_op_list[unit].e_move_op[idx + 1] = e_move_op_list[unit].e_move_op[idx];
    }
    e_move_op_list[unit].e_move_op[index] = *move_op_node;
    e_move_num[unit]++;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get SLB sequence number for a SLB in an entry group.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] e_node Pointer to rm_hash_ent_node_t structure.
 * \param [in] slb_info Pointer to rm_hash_ent_slb_info_t structure.
 * \param [in] slb SLB.
 * \param [out] seq Buffer to hold the result.
 */
static int
rm_hash_slb_loc_in_grp_get(int unit,
                           rm_hash_pt_info_t *pt_info,
                           uint8_t rbank,
                           rm_hash_ent_node_t *e_node,
                           rm_hash_ent_slb_info_t *slb_info,
                           uint32_t slb,
                           uint8_t *loc)
{
    uint8_t num_slbs = 0, idx;
    rm_hash_ent_slb_info_t *tmp_slb_info = NULL, *e_slb_info = slb_info;

    SHR_FUNC_ENTER(unit);

    if (pt_info->bank_separate_sid == FALSE) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    num_slbs = e_node->ent_slb.num_slb_info;

    for (idx = 0; idx < num_slbs; idx++, e_slb_info++) {
        if (e_slb_info->slb == slb) {
            if ((e_slb_info->map & RM_HASH_MULTI_INST_GRP) == 0) {
                *loc = 0;
                SHR_EXIT();
            } else {
                if (idx == 0) {
                    *loc = 0;
                    SHR_EXIT();
                } else if (idx == num_slbs - 1) {
                    *loc = 1;
                    SHR_EXIT();
                } else {
                    tmp_slb_info = e_slb_info - 1;
                    if (tmp_slb_info->map == e_slb_info->map) {
                        *loc = 1;
                        SHR_EXIT();
                    }
                    tmp_slb_info = e_slb_info + 1;
                    if (tmp_slb_info->map == e_slb_info->map) {
                        *loc = 0;
                        SHR_EXIT();
                    }
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
            }
        }
    }
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Determine if the paired entry has been put into the move list.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] pipe Pipe Instance.
 * \param [in] e_node Pointer to rm_hash_ent_node_t structure.
 * \param [in] slb SLB of the entry.
 * \param [out] pending_move Buffer to record the result.
 * \param [out] paired_slb Buffer to record the result.
 */
static int
rm_hash_paired_ent_pending_move(int unit,
                                rm_hash_pt_info_t *pt_info,
                                uint8_t rbank,
                                uint8_t pipe,
                                rm_hash_ent_node_t *e_node,
                                uint32_t slb,
                                bool *pending_move,
                                uint32_t *paired_slb)
{
    rm_hash_ent_slb_info_t paired_slb_info;
    uint8_t paired_slb_loc = 0, paired_rbank = 0;
    rm_hash_ent_node_t *paired_e_node = NULL;
    uint32_t move_idx;
    rm_hash_ent_move_op_t *move_op = NULL;
    bool exist = FALSE;

    SHR_FUNC_ENTER(unit);

    /* Default value is FALSE */
    *pending_move = FALSE;

    /* Get the corresponding paired SLB of the move source entry node. */
    if (pt_info->bank_separate_sid == FALSE) {
        *pending_move = FALSE;
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_paired_slb_get(unit,
                                       pt_info,
                                       rbank,
                                       pipe,
                                       e_node,
                                       slb,
                                       &exist,
                                       &paired_slb_info,
                                       &paired_slb_loc));
    *paired_slb = paired_slb_info.slb;
    if (exist == TRUE) {
        /* Get the corresponding paired entry node of the entry node. */
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_slb_to_rbank_get(unit, pt_info, paired_slb_info.slb,
                                             &paired_rbank));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_paired_e_node_get(unit,
                                              pt_info,
                                              paired_rbank,
                                              pipe,
                                              e_node,
                                              paired_slb_loc,
                                              paired_slb_info.slb,
                                              &paired_e_node));
        /*
         * Determine if the paired entry has been put into
         * the pending move list.
         */
        for (move_idx = 0; move_idx < e_move_num[unit]; move_idx++) {
             move_op = &e_move_op_list[unit].e_move_op[move_idx];
             if ((move_op->s_slb_info.slb == paired_slb_info.slb) &&
                 (sal_memcmp(&move_op->se_loc,
                             &paired_e_node->e_loc,
                             sizeof(move_op->se_loc)) == 0)) {
                /* The paired entry has been put into the pending move list. */
                *pending_move = TRUE;
                SHR_EXIT();
            }
        }
    } else {
        *pending_move = FALSE;
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate a fresh new stack frame for a bucket.
 *
 * Essentially, a specified bucket need to provide some free space to hold
 * an entry. This entry has the size specified via ent_size.
 * When a frame is to be allocated, we will save the following items into the
 * stack: SLB, the free space this bucket needs to provide, candidate
 * entry move <entry, bucket> tuple list, and the total number of elements in
 * the list.

 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to lt rm_hash_pt_info_t structure.
 * \param [in] pipe Pipe instance.
 * \param [in] slb Software logical bucket.
 * \param [in] e_info Information about the requested entry.
 * \param [in] frm_idx Index of the stack frame.
 * \param [out] stack_base Stack of entry move.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_move_stk_frm_alloc(int unit,
                           rm_hash_pt_info_t *pt_info,
                           uint8_t pipe,
                           uint32_t slb,
                           rm_hash_req_ent_info_t e_info,
                           int frm_idx,
                           rm_hash_ent_move_stack_t *stack_base)
{
    rm_hash_slb_state_t *b_state = NULL;
    uint32_t e_node_offset = 0, e_slb, paired_slb = 0;
    rm_hash_ent_node_t *e_node = NULL;
    uint8_t slb_idx = 0, e_idx = 0, epm_idx = 0, rbank = 0;
    uint8_t num_slb_info = 0;
    rm_hash_ptnl_move_info_t *epm_info = stack_base[frm_idx].epm_info;
    rm_hash_ent_slb_info_t *e_slb_info = NULL;
    uint8_t vbb_bmp = 0;
    bool pending_move = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_slb_to_rbank_get(unit, pt_info, slb, &rbank));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_slb_state_get(unit, pt_info, rbank, pipe,
                                      slb, &b_state));
    e_node_offset = b_state->e_node_offset;
    bcmptm_rm_hash_base_ent_node_get(unit, pt_info, rbank, pipe,
                                     e_node_offset, &e_node);
    stack_base[frm_idx].slb = slb;
    stack_base[frm_idx].rbank = rbank;
    stack_base[frm_idx].req_e_info = e_info;
    stack_base[frm_idx].num_epm = 0;
    /*
     * slb is the software logical bucket to be analyzed. First of all
     * it should be checked if it really contains valid entries.
     */
    if (b_state->ve_cnt == 0) {
        SHR_EXIT();
    }
    /*
     * A slb may contain entries of multiple bucket modes. For example,
     * a BM2 slb may contain bm0, bm1 and bm2 entries. So we need to
     * figure out which entries are helpful to accomadate the requested
     * entry with a specific bucket mode.
     */
    for (e_idx = 0; e_idx < pt_info->slb_num_entries; e_idx++, e_node++) {
        if (((e_node->flag & RM_HASH_NODE_IN_USE) == 0) ||
            ((e_node->e_loc.bb_bmp & e_info.vbb_bmp) == 0)) {
            continue;
        }
        if (e_node->ent_slb.num_rbanks > 1) {
            SHR_IF_ERR_EXIT
                (rm_hash_paired_ent_pending_move(unit,
                                                 pt_info,
                                                 rbank,
                                                 pipe,
                                                 e_node,
                                                 slb,
                                                 &pending_move,
                                                 &paired_slb));
        }
        if (pending_move == FALSE) {
           /* For shared hash table, the bank number will be dynamic. */
            num_slb_info = e_node->ent_slb.num_slb_info;
            bcmptm_rm_hash_ent_slb_info_get(unit,
                                            pt_info,
                                            rbank,
                                            pipe,
                                            e_node->ent_slb.slb_info_offset,
                                            &e_slb_info);
            for (slb_idx = 0; slb_idx < num_slb_info; slb_idx++) {
                e_slb = e_slb_info[slb_idx].slb;
                vbb_bmp = e_slb_info[slb_idx].vbb_bmp;
                if ((e_slb == slb) || (vbb_bmp == 0) ||
                    (!rm_hash_move_info_is_valid(e_slb, frm_idx, stack_base)) ||
                    ((e_node->ent_slb.num_rbanks > 1) && (e_slb == paired_slb))) {
                    /*
                     * Currently, the entry resides on the specified slb,
                     * so the slb is not a valid move destination, ignore it.
                     */
                    continue;
                }
                /*
                 * The entry occupies some space in the base bucket. So it
                 * is possible that if we move the entry into another bank,
                 * this may help to accomadate the requested entry.
                 * The following info will be recorded:
                 * 1. The sequence number of entry in the slb entry node list
                 * 2. The slb and vbb bitmap on other banks.
                 */
                epm_info[epm_idx].e_seq = e_idx;
                epm_info[epm_idx].slb_info = e_slb_info[slb_idx];
                epm_info[epm_idx].e_node = e_node;
                epm_idx++;
            }
        }
    }

    stack_base[frm_idx].num_epm = epm_idx;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Save the information of the entry that is selected to be moved out
 * into the stack frame.
 *
 * Essentially, a specified bucket will have multiple candidate entry move
 * <entry, bucket> tuple list. At a given time, one entry in the bucket will
 * be selected to move out. Correspondingly, the information about the selected
 * entry needs to recorded into the stack frame.
 *
 * \param [in] stack Pointer to rm_hash_ent_move_stack_t structure.
 * \param [in] me_info Pointer to lt rm_hash_move_ent_info_t structure.
 */
static void
rm_hash_move_ent_info_set(rm_hash_ent_move_stack_t *stack,
                          rm_hash_move_ent_info_t *me_info)
{
    stack->move_ent = *me_info;
}

/*!
 * \brief Compute empty bucket bitmap for a bucket if it appears in
 * the pending list.
 *
 * \param [in] unit Unit number.
 * \param [in] slb logical bucket index.
 * \param [out] slb_e_bmp Pointer to rm_hash_slb_ent_bmp_t structure.
 * \param [out] src_e_bmp Pointer to rm_hash_slb_ent_bmp_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_slb_ent_bmp_update(int unit,
                           uint32_t slb,
                           rm_hash_slb_ent_bmp_t *slb_e_bmp,
                           rm_hash_slb_ent_bmp_t *src_e_bmp)
{
    uint32_t idx;
    rm_hash_ent_move_op_t *move_op = NULL;
    uint16_t be_bmp = 0, le_bmp = 0;
    uint32_t move_num = rm_hash_ent_move_num_get(unit);

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < move_num; idx++) {
        (void)rm_hash_ent_move_op_get(unit, idx, &move_op);
        if (slb == move_op->d_slb_info.slb) {
            /*
             * slb appears as the destination of an entry move operation.
             * the base entry and logical entry bitmap from the slb point
             * of view need to be reflected as well.
             * First, get the base entry bitmap of the "comming in" entry.
             * Second, get the logical entry bitmap and update these bitmaps
             * into slb state.
             */
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_ent_slb_view_be_bmp_get(unit,
                                                        &move_op->de_loc,
                                                        &be_bmp));
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_ent_slb_view_le_bmp_get(unit,
                                                        &move_op->de_loc,
                                                        &le_bmp));
            /* The be and le info need to be updated into slb state */
            slb_e_bmp->be_bmp |= be_bmp;
            slb_e_bmp->le_bmp[move_op->e_bm] |= le_bmp;
        }
        if ((slb == move_op->s_slb_info.slb) &&
            (move_op->opcode != RM_HASH_ENTRY_DUPLICATE)) {
            /*
             * slb appears as the source of an entry move operation.
             * the base entry and logical entry bitmap change due to
             * the entry moving out should be reflected in slb state as
             * well.
             * For RM_HASH_ENTRY_DUPLICATE operation, the source entry bitmap
             * shouldn't be cleared.
             */
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_ent_slb_view_be_bmp_get(unit,
                                                        &move_op->se_loc,
                                                        &be_bmp));
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_ent_slb_view_le_bmp_get(unit,
                                                        &move_op->se_loc,
                                                        &le_bmp));
            slb_e_bmp->be_bmp &= ~be_bmp;
            slb_e_bmp->le_bmp[move_op->e_bm] &= ~le_bmp;
            /*
             * Record be and le bitmap of all the moved out entries
             * from slb point of view.
             */
            if (src_e_bmp != NULL) {
                src_e_bmp->be_bmp |= be_bmp;
                src_e_bmp->le_bmp[move_op->e_bm] |= le_bmp;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Compute how many entry have been analyzed.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] pipe Pipe instance.
 * \param [in] stack_frm Stack frame.
 * \param [out] ent_cnt Entry count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_analyzed_ment_cnt_get(int unit,
                              rm_hash_pt_info_t *pt_info,
                              uint8_t pipe,
                              rm_hash_ent_move_stack_t *stack_frm,
                              uint8_t *ent_cnt)
{
    uint8_t e_seq = 0, rbank = 0;
    rm_hash_slb_state_t *b_state = NULL;
    uint32_t e_node_offset = 0;
    uint16_t be_bmp = 0, le_bmp = 0;
    rm_hash_ent_node_t *e_node = NULL;
    rm_hash_slb_ent_bmp_t slb_e_bmp;
    rm_hash_slb_ent_bmp_t src_e_bmp;
    uint32_t slb = stack_frm->slb;

    SHR_FUNC_ENTER(unit);

    sal_memset(&slb_e_bmp, 0, sizeof(slb_e_bmp));
    sal_memset(&src_e_bmp, 0, sizeof(src_e_bmp));

    rbank = stack_frm->rbank;
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_slb_state_get(unit, pt_info, rbank, pipe,
                                      slb, &b_state));
    e_node_offset = b_state->e_node_offset;
    bcmptm_rm_hash_base_ent_node_get(unit, pt_info, rbank, pipe,
                                     e_node_offset, &e_node);
    slb_e_bmp = b_state->e_bmp;
    SHR_IF_ERR_EXIT
        (rm_hash_slb_ent_bmp_update(unit, slb, &slb_e_bmp, &src_e_bmp));

    while ((e_seq < pt_info->slb_num_entries) && (src_e_bmp.be_bmp != 0)) {
        /*
         * There may be multiple entries with multiple BM. For each entry,
         * we need to determine if it appears in the pending move list.
         */
        if (e_node->flag & RM_HASH_NODE_IN_USE) {
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_ent_slb_view_be_bmp_get(unit,
                                                        &e_node->e_loc,
                                                        &be_bmp));
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_ent_slb_view_le_bmp_get(unit,
                                                        &e_node->e_loc,
                                                        &le_bmp));
            /*
             * be and le for this entry node has been retrieved.
             * Check if this entry node has been put into pending move list.
             */
            if (((be_bmp & src_e_bmp.be_bmp) == be_bmp) &&
                ((le_bmp & src_e_bmp.le_bmp[e_node->ent_slb.bm]) == le_bmp)) {
                src_e_bmp.be_bmp &= ~(be_bmp);
                src_e_bmp.le_bmp[e_node->ent_slb.bm] &= ~(le_bmp);
            }
        }
        e_node++;
        e_seq++;
    }
    *ent_cnt = e_seq;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Determine if all the pending move entries have been analyzed.
 *
 * \param [in] stack_ptr Pointer to rm_hash_ent_move_stack_t structure.
 * \param [in] ana_e_cnt Analyzed entry count
 *
 * \retval TRUE.
 * \retval FALSE.
 */
static bool
rm_hash_move_curr_bkt_idx_valid(rm_hash_ent_move_stack_t *stack_ptr,
                                uint8_t ana_e_cnt)
{
    uint8_t idx = stack_ptr->num_epm - 1;

    if (stack_ptr->epm_info[idx].e_seq < ana_e_cnt) {
        /* All the entries have been placed into the pending move list */
        return FALSE;
    }
    return TRUE;
}

/*!
 * \brief Get used base entry bitmap for an slb
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] pipe Pipe instance.
 * \param [in] slb software logical bucket.
 * \param [out] slb_e_bmp software logical bucket used base entry bitmap
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_slb_used_bmp_get(int unit,
                         rm_hash_pt_info_t *pt_info,
                         uint8_t pipe,
                         uint32_t slb,
                         uint8_t *prbank,
                         rm_hash_slb_ent_bmp_t *slb_e_bmp)
{
    rm_hash_slb_state_t *b_state = NULL;
    rm_hash_slb_ent_bmp_t e_bmp;
    uint8_t rbank = 0;

    SHR_FUNC_ENTER(unit);

    if (prbank == NULL) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_slb_to_rbank_get(unit, pt_info, slb, &rbank));
    } else {
        rbank = *prbank;
    }

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_slb_state_get(unit, pt_info, rbank, pipe, slb, &b_state));
    /* Initial value retrieval */
    e_bmp = b_state->e_bmp;
    /* Update the value according to the move list */
    SHR_IF_ERR_EXIT
        (rm_hash_slb_ent_bmp_update(unit, slb, &e_bmp, NULL));
    *slb_e_bmp = e_bmp;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get used base entry bitmap for an slb
 *
 * \param [in] unit Unit number.
 * \param [in] stack_frm Pointer to rm_hash_ent_move_stack_t structure.
 * \param [in] ana_ent_cnt Analyzed entry count
 * \param [in] curr_idx Current index
 * \param [out] next_idx Next index
 */
static void
rm_hash_next_epm_index_get(int unit,
                           rm_hash_ent_move_stack_t *stack_frm,
                           uint8_t ana_ent_cnt,
                           uint8_t curr_idx,
                           uint8_t *next_idx)
{
    uint8_t epm_idx = curr_idx + 1;

    while ((stack_frm->epm_info[epm_idx].e_seq < ana_ent_cnt) &&
           (epm_idx < stack_frm->num_epm)) {
        epm_idx++;
    }
    *next_idx = epm_idx;
}

/*!
 * \brief Update the entry move info in the stack.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] pipe Pipe instance.
 * \param [in] stack_frm Pointer to rm_hash_ent_move_stack_t structure.
 * \param [in] curr_epm_idx Current move index.
 * \param [in] e_bmp SLB bitmap.
 * \param [out] ment_found If entry has been found.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_move_stk_frm_ment_record(int unit,
                                 rm_hash_pt_info_t *pt_info,
                                 uint8_t pipe,
                                 rm_hash_ent_move_stack_t *stack_frm,
                                 uint8_t curr_epm_idx,
                                 rm_hash_slb_ent_bmp_t e_bmp,
                                 bool *ment_found)
{
    uint8_t ament_cnt = 0, act_epm_idx = 0;
    rm_hash_move_ent_info_t ment_info;
    uint16_t be_bmp = 0, le_bmp = 0;
    rm_hash_ent_node_t *e_node = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&ment_info, 0, sizeof(ment_info));

    /* Determine how many entries should be skipped */
    SHR_IF_ERR_EXIT
        (rm_hash_analyzed_ment_cnt_get(unit,
                                       pt_info,
                                       pipe,
                                       stack_frm,
                                       &ament_cnt));
    /*
     * The provided tuple index exceeds the maximum value, Or all the entries
     * have been analyzed.
     */
    if ((curr_epm_idx >= stack_frm->num_epm) ||
        (!rm_hash_move_curr_bkt_idx_valid(stack_frm, ament_cnt))){
        *ment_found = FALSE;
        SHR_EXIT();
    }
    /*
     * If the entry indexed by the provided tuple index has been already
     * analyzed, we need to increase the tuple index until a un-analyzed one
     */
    if (stack_frm->epm_info[curr_epm_idx].e_seq < ament_cnt) {
        rm_hash_next_epm_index_get(unit,
                                   stack_frm,
                                   ament_cnt,
                                   curr_epm_idx,
                                   &act_epm_idx);
        curr_epm_idx = act_epm_idx;
    }
    if (curr_epm_idx >= stack_frm->num_epm) {
        *ment_found = FALSE;
        SHR_EXIT();
    }
    /* Get the move entry based on tuple index */
    e_node = stack_frm->epm_info[curr_epm_idx].e_node;
    ment_info.bm = e_node->ent_slb.bm;
    ment_info.e_loc  = e_node->e_loc;
    ment_info.e_size = bcmptm_rm_hash_ent_size_get(e_node->e_loc.be_bmp);
    ment_info.e_node = e_node;
    /* Update the stack frame accordingly */
    rm_hash_move_ent_info_set(stack_frm, &ment_info);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_slb_view_be_bmp_get(unit,
                                                &ment_info.e_loc,
                                                &be_bmp));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_slb_view_le_bmp_get(unit,
                                                &ment_info.e_loc,
                                                &le_bmp));

    /* Update ent bitmap */
    e_bmp.be_bmp &= ~be_bmp;
    e_bmp.le_bmp[ment_info.bm] &= ~le_bmp;
    stack_frm->e_bmp = e_bmp;

    stack_frm->curr_epm_idx = curr_epm_idx;
    stack_frm->slot_provider = FALSE;
    *ment_found = TRUE;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Push the bucket information onto the bucket stack.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] pipe Pipe instance.
 * \param [in] slb_info Software logical bucket index.
 * \param [in] stack Stack of entry move.
 * \param [in] req_e_info Information of the requested entry.
 * \param [in] sfrm_idx Starting frame index.
 * \param [in] frm_num_limit Maximum stack frame number to be tried.
 * \param [out] frm_num Actual depth of the push operation.
 * \param [out] slot_found Flag indicating if free slot has been found.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_move_bkt_stack_push(int unit,
                                rm_hash_pt_info_t *pt_info,
                                uint8_t pipe,
                                rm_hash_ent_slb_info_t slb_info,
                                rm_hash_ent_move_stack_t *stack,
                                rm_hash_req_ent_info_t req_e_info,
                                uint8_t sfrm_idx,
                                uint8_t frm_num_limit,
                                uint8_t *frm_num,
                                bool *slot_found)
{
    int fidx = 0, epm_idx = 0;
    uint8_t curr_epm_idx = 0;
    bool accom = FALSE, ment_found = FALSE;
    rm_hash_slb_ent_bmp_t slb_e_bmp;
    rm_hash_ent_loc_t e_loc;
    rm_hash_req_ent_info_t e_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&slb_e_bmp, 0, sizeof(slb_e_bmp));
    sal_memset(&e_loc, 0, sizeof(e_loc));
    fidx = sfrm_idx;
    e_info = req_e_info;
    if (fidx > 0) {
        curr_epm_idx = stack[fidx - 1].curr_epm_idx;
        SHR_IF_ERR_EXIT
            (rm_hash_slb_used_bmp_get(unit,
                                      pt_info,
                                      pipe,
                                      stack[fidx - 1].slb,
                                      &stack[fidx - 1].rbank,
                                      &slb_e_bmp));
        /* Try to find an entry that can be moved out. */
        SHR_IF_ERR_EXIT
            (rm_hash_move_stk_frm_ment_record(unit,
                                              pt_info,
                                              pipe,
                                              &stack[fidx - 1],
                                              curr_epm_idx,
                                              slb_e_bmp,
                                              &ment_found));
        if (ment_found == FALSE) {
             rm_hash_ent_move_trace_clear(unit, stack[fidx - 1].slb, fidx - 1);
             *frm_num = fidx - 1;
             *slot_found = FALSE;
             SHR_EXIT();
        }
    }

    for (; fidx < frm_num_limit; fidx++) {
        if (fidx > 0) {
            epm_idx = stack[fidx - 1].curr_epm_idx;
            slb_info = stack[fidx - 1].epm_info[epm_idx].slb_info;
            e_info.e_size  = stack[fidx - 1].move_ent.e_size;
            e_info.vbb_bmp = stack[fidx - 1].epm_info[epm_idx].slb_info.vbb_bmp;
            e_info.e_bm    = stack[fidx - 1].move_ent.bm;
            stack[fidx - 1].curr_epm_idx += 1;
        }
        rm_hash_move_stk_frm_alloc(unit, pt_info, pipe, slb_info.slb,
                                   e_info, fidx, stack);
        SHR_IF_ERR_EXIT
            (rm_hash_slb_used_bmp_get(unit,
                                      pt_info,
                                      pipe,
                                      slb_info.slb,
                                      &stack[fidx].rbank,
                                      &slb_e_bmp));
        stack[fidx].e_bmp = slb_e_bmp;
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_bkt_free_ent_space_get(unit,
                                                   pt_info,
                                                   &slb_e_bmp,
                                                   &e_info,
                                                   &accom,
                                                   &e_loc));
        if (accom == TRUE) {
            *frm_num = fidx + 1;
            *slot_found = TRUE;
            stack[fidx].slot_provider = TRUE;
            SHR_EXIT();
        }
        if ((fidx == (frm_num_limit - 1)) && accom == FALSE) {
            *frm_num = fidx;
            *slot_found = FALSE;
            stack[fidx].slot_provider = FALSE;
            SHR_EXIT();
        }
        curr_epm_idx = 0;
        SHR_IF_ERR_EXIT
            (rm_hash_move_stk_frm_ment_record(unit,
                                              pt_info,
                                              pipe,
                                              &stack[fidx],
                                              curr_epm_idx,
                                              slb_e_bmp,
                                              &ment_found));
        if (ment_found == FALSE) {
            rm_hash_ent_move_trace_clear(unit, stack[fidx].slb, fidx);
            *frm_num = fidx;
            *slot_found = FALSE;
            SHR_EXIT();
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get entry group number based on slb.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] pipe Pipe instance.
 * \param [in] e_node Pointer to rm_hash_ent_node_t structure.
 * \param [in] slb Software logical bucket.
 * \param [out] e_grp Returned group number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_slb_grp_get(int unit,
                        rm_hash_pt_info_t *pt_info,
                        uint8_t rbank,
                        uint8_t pipe,
                        rm_hash_ent_node_t *e_node,
                        uint32_t slb,
                        uint8_t *e_grp)
{
    rm_hash_ent_slb_info_t *e_slb_info = NULL;
    uint8_t num_slb_info, idx;

    SHR_FUNC_ENTER(unit);

    bcmptm_rm_hash_ent_slb_info_get(unit,
                                    pt_info,
                                    rbank,
                                    pipe,
                                    e_node->ent_slb.slb_info_offset,
                                    &e_slb_info);

    num_slb_info = e_node->ent_slb.num_slb_info;
    for (idx = 0; idx < num_slb_info; idx++) {
        if (e_slb_info[idx].slb == slb) {
            *e_grp = e_slb_info[idx].map;
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Pop the stack
 *
 * The stack to be popped will contain 1 or multiple frames. The total frame number
 * of the stack is described by the parameter cur_frm_num. Starting from the top of
 * stack, this routine will analyze whether current frame can accommodate the requested
 * entry size. The requested entry comes from last frame.
 *
 * If the current frame can accommodate the requested entry size, that means the entry
 * in current - 1 frame can be moved into current frame. So an entry move operation
 * will be recorded in the move pending list.
 *
 * In the process of popping from top of the stack to the bottom of the stack, once
 * a frame at layer n cannot accommodate its requested entry, the pop operation will
 * terminate and the stack depth will be updated as n. in another word, it consists
 * of frame 0.. n-1.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] pipe Pipe instance.
 * \param [in] stack Entry move stack.
 * \param [in] cur_frm_num Current depth of entry move stack.
 * \param [out] new_frm_num Depth of entry move stack after the pop is completed.
 * \param [out] slot_found Flag indicating if free slot has been found.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_move_bkt_stack_pop(int unit,
                               rm_hash_pt_info_t *pt_info,
                               uint8_t pipe,
                               rm_hash_ent_move_stack_t *stack,
                               uint8_t cur_frm_num,
                               uint8_t *new_frm_num,
                               bool *slot_found)
{
    int frm_idx;
    rm_hash_ent_slb_info_t s_slb_info, d_slb_info;
    bool accom = FALSE;
    rm_hash_ent_loc_t se_loc, de_loc;
    rm_hash_bm_t e_bm;
    rm_hash_req_ent_info_t e_info;
    uint8_t s_rbank;
    uint8_t d_rbank;
    uint8_t s_grp = 0, d_grp = 0;
    rm_hash_ent_move_op_t src_move_op;

    SHR_FUNC_ENTER(unit);

    sal_memset(&se_loc, 0, sizeof(se_loc));
    sal_memset(&de_loc, 0, sizeof(de_loc));
    sal_memset(&s_slb_info, 0, sizeof(s_slb_info));
    sal_memset(&d_slb_info, 0, sizeof(d_slb_info));
    for (frm_idx = cur_frm_num - 1; frm_idx >= 0; frm_idx--) {
        e_info = stack[frm_idx].req_e_info;
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_bkt_free_ent_space_get(unit,
                                                   pt_info,
                                                   &stack[frm_idx].e_bmp,
                                                   &e_info,
                                                   &accom,
                                                   &de_loc));

        if (accom == TRUE) {
            if (frm_idx == 0) {
                *slot_found = TRUE;
                SHR_EXIT();
            }
            s_slb_info.slb = stack[frm_idx - 1].slb;
            se_loc = stack[frm_idx - 1].move_ent.e_loc;
            e_bm = stack[frm_idx - 1].move_ent.bm;
            d_slb_info.slb = stack[frm_idx].slb;
            s_rbank = stack[frm_idx - 1].rbank;
            d_rbank = stack[frm_idx].rbank;
            /* Get the src group of the moved entry. */
            SHR_IF_ERR_EXIT
                (rm_hash_ent_slb_grp_get(unit,
                                         pt_info,
                                         s_rbank,
                                         pipe,
                                         stack[frm_idx - 1].move_ent.e_node,
                                         s_slb_info.slb,
                                         &s_grp));
            s_slb_info.map = s_grp;
            /* Get the dst group of the moved entry. */
            SHR_IF_ERR_EXIT
                (rm_hash_ent_slb_grp_get(unit,
                                         pt_info,
                                         s_rbank,
                                         pipe,
                                         stack[frm_idx - 1].move_ent.e_node,
                                         d_slb_info.slb,
                                         &d_grp));
            d_slb_info.map = d_grp;
            rm_hash_ent_move_op_node_create(unit,
                                            pt_info,
                                            s_slb_info,
                                            se_loc,
                                            d_slb_info,
                                            de_loc,
                                            s_rbank,
                                            d_rbank,
                                            stack[frm_idx].req_e_info.e_size,
                                            e_bm,
                                            RM_HASH_UNKNOWN_MOVE,
                                            RM_HASH_ENTRY_MOVE,
                                            0,
                                            0,
                                            stack[frm_idx].slot_provider,
                                            stack[frm_idx - 1].move_ent.e_node->flag,
                                            &src_move_op);
            SHR_IF_ERR_EXIT
                (rm_hash_ent_move_op_set(unit, &src_move_op));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "movement operation save: bkt_id [%d] \
                                     bkt_pair_id [%d] num [%d]\n"),
                                    s_slb_info.slb, d_slb_info.slb,
                                    stack[frm_idx].req_e_info.e_size));
        } else {
            *new_frm_num = frm_idx;
            *slot_found = FALSE;
            SHR_EXIT();
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the entry move path
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] pipe Pipe instance.
 * \param [in] num_slb Number of software logical buckets.
 * \param [in] slb Software logical bucket list.
 * \param [in] req_e_info_list Request entry information.
 * \param [in] trying_depth Depth of entry move stack.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_move_path_get(int unit,
                          rm_hash_pt_info_t *pt_info,
                          uint8_t pipe,
                          uint8_t num_slb,
                          rm_hash_ent_slb_info_t *slb_info_list,
                          rm_hash_req_ent_info_t *req_e_info_list,
                          uint8_t trying_depth)
{
    uint8_t frmnum_pushed, pop_depth, idx = 0;
    bool slot_found_in_push = FALSE, path_avail = FALSE;

    SHR_FUNC_ENTER(unit);

    rm_hash_ent_move_info_clear(unit);

    for (idx = 0; idx < num_slb; idx++) {
        frmnum_pushed = 0;
        pop_depth = 0;

        sal_memset(ent_move_stack[unit], 0,
                   sizeof(rm_hash_ent_move_stack_t) *
                   (BCMPTM_RM_HASH_MAX_HASH_REORDERING_DEPTH + 1));

        while (1) {
            SHR_IF_ERR_EXIT
                (rm_hash_ent_move_bkt_stack_push(unit,
                                                 pt_info,
                                                 pipe,
                                                 slb_info_list[idx],
                                                 ent_move_stack[unit],
                                                 req_e_info_list[idx],
                                                 pop_depth,
                                                 trying_depth,
                                                 &frmnum_pushed,
                                                 &slot_found_in_push));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "slot_found_in_push:[%d], start depth:[%d],"
                                    "end depth [%d]\n"),
                                    slot_found_in_push, pop_depth, frmnum_pushed));
            if (slot_found_in_push == FALSE) {
                if (frmnum_pushed == 0) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit, "empty entry is not found\n")));
                    SHR_ERR_EXIT(SHR_E_RESOURCE);
                } else {
                    pop_depth = frmnum_pushed;
                }
            } else {
                SHR_IF_ERR_EXIT
                    (rm_hash_ent_move_bkt_stack_pop(unit,
                                                    pt_info,
                                                    pipe,
                                                    ent_move_stack[unit],
                                                    frmnum_pushed,
                                                    &pop_depth,
                                                    &path_avail));
                if (path_avail == TRUE) {
                    if (idx == (num_slb - 1)) {
                        SHR_EXIT();
                    } else {
                        break;
                    }

                } else {
                    pop_depth += 1;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an entry move operation into pending move list.
 *
 * This entry move is corresponding to a move between multiple-element groups.
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] pipe Pipe instance.
 * \param [in] move_op Pointer to rm_hash_ent_move_op_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_megrp_to_megrp_node_add(int unit,
                                    rm_hash_pt_info_t *pt_info,
                                    uint8_t pipe,
                                    rm_hash_ent_move_op_t *move_op,
                                    int index)
{
    rm_hash_slb_state_t *s_b_state = NULL;
    rm_hash_ent_node_t *match_ent = NULL, *paired_e_node = NULL;
    rm_hash_ent_slb_info_t paired_s_slb_info, paired_d_slb_info, d_slb_info;
    uint8_t paired_slb_seq, s_rbank, d_rbank, psrbank;
    rm_hash_slb_ent_bmp_t slb_e_bmp;
    rm_hash_req_ent_info_t e_info;
    bool accom_in_pair_slb = FALSE;
    bool pair_d_pending_move = FALSE;
    bool pair_s_pending_move = FALSE;
    rm_hash_ent_loc_t e_loc_for_pair;
    bool exist = FALSE;
    rm_hash_ent_move_op_t src_move_op;

    SHR_FUNC_ENTER(unit);

    /* The paired entry should be installed as well. */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_slb_state_get(unit, pt_info, move_op->s_rbank, pipe,
                                      move_op->s_slb_info.slb,
                                      &s_b_state));
    /* Get the move source entry node. */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_node_search(unit,
                                        pt_info,
                                        move_op->s_rbank,
                                        pipe,
                                        s_b_state,
                                        move_op->se_loc.bb_bmp,
                                        move_op->se_loc.be_bmp,
                                        NULL,
                                        &match_ent));
    /* Get the corresponding paired SLB of the move source entry node. */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_paired_slb_get(unit,
                                       pt_info,
                                       move_op->s_rbank,
                                       pipe,
                                       match_ent,
                                       move_op->s_slb_info.slb,
                                       &exist,
                                       &paired_s_slb_info,
                                       &paired_slb_seq));
    /* Get the corresponding paired entry node of the move source entry node. */
    if (exist == FALSE) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_slb_to_rbank_get(unit, pt_info, paired_s_slb_info.slb,
                                         &psrbank));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_paired_e_node_get(unit,
                                          pt_info,
                                          psrbank,
                                          pipe,
                                          match_ent,
                                          paired_slb_seq,
                                          paired_s_slb_info.slb,
                                          &paired_e_node));
    /*
     * All the information about source paired entry has been obtained.
     * Analyze if the destination pair SLB is able to accomadate the entry.
     */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_paired_slb_get(unit,
                                       pt_info,
                                       move_op->s_rbank,
                                       pipe,
                                       match_ent,
                                       move_op->d_slb_info.slb,
                                       &exist,
                                       &paired_d_slb_info,
                                       &paired_slb_seq));
    if (exist == FALSE) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    /* Determine if the paired destination has enough space. */
    sal_memset(&slb_e_bmp, 0, sizeof(slb_e_bmp));
    SHR_IF_ERR_EXIT
        (rm_hash_slb_used_bmp_get(unit,
                                  pt_info,
                                  pipe,
                                  paired_d_slb_info.slb,
                                  NULL,
                                  &slb_e_bmp));
    sal_memset(&e_info, 0, sizeof(e_info));
    e_info.e_size = move_op->e_size;
    e_info.vbb_bmp = paired_d_slb_info.vbb_bmp;
    e_info.e_bm = move_op->e_bm;
    sal_memset(&e_loc_for_pair, 0, sizeof(e_loc_for_pair));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_bkt_free_ent_space_get(unit,
                                               pt_info,
                                               &slb_e_bmp,
                                               &e_info,
                                               &accom_in_pair_slb,
                                               &e_loc_for_pair));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_slb_to_rbank_get(unit,
                                         pt_info,
                                         paired_s_slb_info.slb,
                                         &s_rbank));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_slb_to_rbank_get(unit,
                                         pt_info,
                                         paired_d_slb_info.slb,
                                         &d_rbank));
    pair_d_pending_move = rm_hash_slb_is_pending_move(unit, paired_d_slb_info.slb);
    pair_s_pending_move = rm_hash_slb_is_pending_move(unit, paired_s_slb_info.slb);
    if ((accom_in_pair_slb == TRUE) && (pair_d_pending_move == FALSE) &&
        (pair_s_pending_move == FALSE)) {
        /* The corresponding paired entry can be moved as well. */
        rm_hash_ent_move_op_node_create(unit,
                                        pt_info,
                                        paired_s_slb_info,
                                        paired_e_node->e_loc,
                                        paired_d_slb_info,
                                        e_loc_for_pair,
                                        s_rbank,
                                        d_rbank,
                                        e_info.e_size,
                                        e_info.e_bm,
                                        RM_HASH_MEGRP_TO_MEGRP_MOVE,
                                        RM_HASH_ENTRY_MOVE,
                                        1,
                                        paired_slb_seq,
                                        FALSE,
                                        paired_e_node->flag,
                                        &src_move_op);
        move_op->d_slb_seq = paired_slb_seq > 0 ? 0 : 1;
        SHR_IF_ERR_EXIT
            (rm_hash_ent_move_info_insert(unit, index + 1, &src_move_op));
        /*
         * The paired entry can be removed. Essentially it is same as
         * RM_HASH_MEGRP_TO_SEGRP_MOVE.
         */
        sal_memset(&e_loc_for_pair, 0, sizeof(e_loc_for_pair));
        sal_memset(&d_slb_info, 0, sizeof(d_slb_info));
        rm_hash_ent_move_op_node_create(unit,
                                        pt_info,
                                        paired_s_slb_info,
                                        paired_e_node->e_loc,
                                        d_slb_info,
                                        e_loc_for_pair,
                                        s_rbank,
                                        0,
                                        e_info.e_size,
                                        e_info.e_bm,
                                        RM_HASH_MEGRP_TO_MEGRP_MOVE,
                                        RM_HASH_ENTRY_DELETE,
                                        2,
                                        0,
                                        FALSE,
                                        paired_e_node->flag,
                                        &src_move_op);
        SHR_IF_ERR_EXIT
            (rm_hash_ent_move_info_insert(unit, index + 2, &src_move_op));
    } else {
        /*
         * The corresponding paired entry can not be moved. The pending
         * move list should be cleared.
         */
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an entry move operation into pending move list.
 *
 * This entry move is corresponding to a move from multiple-element group to
 * single-element group.
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] pipe Pipe instance.
 * \param [in] move_op Pointer to rm_hash_ent_move_op_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_megrp_to_segrp_node_add(int unit,
                                    rm_hash_pt_info_t *pt_info,
                                    uint8_t pipe,
                                    rm_hash_ent_move_op_t *move_op,
                                    int index)
{
    rm_hash_slb_state_t *s_b_state = NULL;
    rm_hash_ent_node_t *match_ent = NULL, *paired_e_node = NULL;
    rm_hash_ent_slb_info_t paired_s_slb_info, dst_slb_info;
    uint8_t paired_slb_seq;
    bool exist = FALSE;
    bool pair_s_pending_move = FALSE;
    rm_hash_ent_move_op_t src_move_op;
    rm_hash_ent_loc_t e_loc_for_pair;
    uint8_t psrbank;

    SHR_FUNC_ENTER(unit);

    /* The paired entry should be deleted. */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_slb_state_get(unit, pt_info, move_op->s_rbank, pipe,
                                      move_op->s_slb_info.slb,
                                      &s_b_state));
    /* Get the move source entry node. */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_node_search(unit,
                                        pt_info,
                                        move_op->s_rbank,
                                        pipe,
                                        s_b_state,
                                        move_op->se_loc.bb_bmp,
                                        move_op->se_loc.be_bmp,
                                        NULL,
                                        &match_ent));
    /* Get the corresponding paired SLB of the move source entry node. */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_paired_slb_get(unit,
                                       pt_info,
                                       move_op->s_rbank,
                                       pipe,
                                       match_ent,
                                       move_op->s_slb_info.slb,
                                       &exist,
                                       &paired_s_slb_info,
                                       &paired_slb_seq));
    if (exist == FALSE) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_slb_to_rbank_get(unit, pt_info, paired_s_slb_info.slb,
                                         &psrbank));
    /* Get the corresponding paired entry node of the move source entry node. */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_paired_e_node_get(unit,
                                          pt_info,
                                          psrbank,
                                          pipe,
                                          match_ent,
                                          paired_slb_seq,
                                          paired_s_slb_info.slb,
                                          &paired_e_node));
    pair_s_pending_move = rm_hash_slb_is_pending_move(unit, paired_s_slb_info.slb);
    if (pair_s_pending_move == FALSE) {
        /* The corresponding paired entry can be moved as well. */
        sal_memset(&e_loc_for_pair, 0, sizeof(e_loc_for_pair));
        sal_memset(&dst_slb_info, 0, sizeof(dst_slb_info));
        rm_hash_ent_move_op_node_create(unit,
                                        pt_info,
                                        paired_s_slb_info,
                                        paired_e_node->e_loc,
                                        dst_slb_info,
                                        e_loc_for_pair,
                                        psrbank,
                                        0,
                                        move_op->e_size,
                                        move_op->e_bm,
                                        RM_HASH_MEGRP_TO_SEGRP_MOVE,
                                        RM_HASH_ENTRY_CLEAR,
                                        1,
                                        0,
                                        FALSE,
                                        paired_e_node->flag,
                                        &src_move_op);
        SHR_IF_ERR_EXIT
            (rm_hash_ent_move_info_insert(unit, index + 1, &src_move_op));
    } else {
        /*
         * The corresponding paired entry can not be moved. The pending
         * move list should be cleared.
         */
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an entry move operation into pending move list.
 *
 * This entry move is corresponding to a move from single-element group to
 * multiple-element group.
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] pipe Pipe instance.
 * \param [in] move_op Pointer to rm_hash_ent_move_op_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_segrp_to_megrp_node_add(int unit,
                                    rm_hash_pt_info_t *pt_info,
                                    uint8_t pipe,
                                    rm_hash_ent_move_op_t *move_op,
                                    int index)
{
    rm_hash_slb_state_t *s_b_state = NULL;
    rm_hash_ent_node_t *match_ent = NULL;
    rm_hash_ent_slb_info_t paired_d_slb_info;
    uint8_t paired_slb_seq, d_rbank;
    rm_hash_slb_ent_bmp_t slb_e_bmp;
    rm_hash_req_ent_info_t e_info;
    bool accom_in_pair_slb = FALSE, pending_move = FALSE;
    rm_hash_ent_loc_t e_loc_for_pair;
    bool exist = FALSE;
    rm_hash_ent_move_op_t src_move_op;

    SHR_FUNC_ENTER(unit);

    /* The paired entry should be installed as well. */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_slb_state_get(unit, pt_info, move_op->s_rbank, pipe,
                                      move_op->s_slb_info.slb,
                                      &s_b_state));

    /* Get the move source entry node. */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_node_search(unit,
                                        pt_info,
                                        move_op->s_rbank,
                                        pipe,
                                        s_b_state,
                                        move_op->se_loc.bb_bmp,
                                        move_op->se_loc.be_bmp,
                                        NULL,
                                        &match_ent));
    /* Get the corresponding paired SLB of the move destination entry node. */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_paired_slb_get(unit,
                                       pt_info,
                                       move_op->s_rbank,
                                       pipe,
                                       match_ent,
                                       move_op->d_slb_info.slb,
                                       &exist,
                                       &paired_d_slb_info,
                                       &paired_slb_seq));
    if (exist == FALSE) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Determine if the paired destination has enough space. */
    sal_memset(&slb_e_bmp, 0, sizeof(slb_e_bmp));
    SHR_IF_ERR_EXIT
        (rm_hash_slb_used_bmp_get(unit,
                                  pt_info,
                                  pipe,
                                  paired_d_slb_info.slb,
                                  NULL,
                                  &slb_e_bmp));
    sal_memset(&e_info, 0, sizeof(e_info));
    e_info.e_size = move_op->e_size;
    e_info.vbb_bmp = paired_d_slb_info.vbb_bmp;
    e_info.e_bm = move_op->e_bm;
    sal_memset(&e_loc_for_pair, 0, sizeof(e_loc_for_pair));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_bkt_free_ent_space_get(unit,
                                               pt_info,
                                               &slb_e_bmp,
                                               &e_info,
                                               &accom_in_pair_slb,
                                               &e_loc_for_pair));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_slb_to_rbank_get(unit,
                                         pt_info,
                                         paired_d_slb_info.slb,
                                         &d_rbank));
    pending_move = rm_hash_slb_is_pending_move(unit, paired_d_slb_info.slb);
    if ((accom_in_pair_slb == TRUE) && (pending_move == FALSE)) {
        /* The corresponding paired entry can be moved as well. */
        rm_hash_ent_move_op_node_create(unit,
                                        pt_info,
                                        move_op->d_slb_info,
                                        move_op->de_loc,
                                        paired_d_slb_info,
                                        e_loc_for_pair,
                                        move_op->d_rbank,
                                        d_rbank,
                                        e_info.e_size,
                                        e_info.e_bm,
                                        RM_HASH_SEGRP_TO_MEGRP_MOVE,
                                        RM_HASH_ENTRY_DUPLICATE,
                                        1,
                                        paired_slb_seq,
                                        FALSE,
                                        match_ent->flag,
                                        &src_move_op);
        move_op->d_slb_seq = paired_slb_seq > 0 ? 0 : 1;
        SHR_IF_ERR_EXIT
            (rm_hash_ent_move_info_insert(unit, index + 1, &src_move_op));
    } else {
        /*
         * The corresponding paired entry can not be moved. The pending
         * move list should be cleared.
         */
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update Entry move list.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] pipe Pipe instance.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_move_path_update(int unit,
                             rm_hash_pt_info_t *pt_info,
                             uint8_t pipe)
{
    rm_hash_ent_move_op_t *move_op = NULL;
    uint32_t move_idx;

    SHR_FUNC_ENTER(unit);

    for (move_idx = 0; move_idx < e_move_num[unit]; move_idx++) {
        move_op = &e_move_op_list[unit].e_move_op[move_idx];
        if (move_op->parent_offset == 0) {
            switch (move_op->move_op_type) {
            case RM_HASH_SEGRP_TO_SEGRP_MOVE:
                break;
            case RM_HASH_MEGRP_TO_MEGRP_MOVE:
                SHR_IF_ERR_VERBOSE_EXIT
                    (rm_hash_ent_megrp_to_megrp_node_add(unit, pt_info, pipe, move_op, move_idx));
                break;
            case RM_HASH_MEGRP_TO_SEGRP_MOVE:
                SHR_IF_ERR_EXIT
                    (rm_hash_ent_megrp_to_segrp_node_add(unit, pt_info, pipe, move_op, move_idx));
                break;
            case RM_HASH_SEGRP_TO_MEGRP_MOVE:
                SHR_IF_ERR_VERBOSE_EXIT
                    (rm_hash_ent_segrp_to_megrp_node_add(unit, pt_info, pipe, move_op, move_idx));
                break;
            default:
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Conduct an entry move operation in the pending move list.
 *
 * This entry move is corresponding to a move from single-element group to
 * multiple-element group.
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] pipe Pipe instance.
 * \param [in] move_op Pointer to rm_hash_ent_move_op_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_1st_segrp_to_megrp_node_move(int unit,
                                         rm_hash_pt_info_t *pt_info,
                                         uint8_t pipe,
                                         rm_hash_ent_move_op_t *move_op)
{
    rm_hash_ent_node_t *src_e_node = NULL, *dst_e1_node = NULL;
    uint8_t idx, free_node_idx = 0, loc = 0;
    rm_hash_ent_slb_info_t *e_slb_info = NULL, *me_slb_info = NULL;
    uint32_t slb_info_offset;
    rm_hash_nme_info_t nm_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&nm_info, 0, sizeof(nm_info));
    /* Delete the entry node in the source bucket. */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_node_delete(unit,
                                        pt_info,
                                        move_op->s_rbank,
                                        pipe,
                                        move_op->s_slb_info.slb,
                                        move_op->se_loc.bb_bmp,
                                        move_op->se_loc.be_bmp,
                                        NULL,
                                        &src_e_node));
    /*
     * Bucket state and entry node has been updated, corresponding
     * undo operation will be recorded in the undo list.
     */
    if (bcmptm_rm_hash_undo_list_maintained(unit) == TRUE) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_trans_undo_node_insert(unit,
                                                   pt_info,
                                                   move_op->s_slb_info.slb,
                                                   pipe,
                                                   src_e_node,
                                                   BCMPTM_RM_HASH_UNDO_ENT_INSERT));
    }
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_free_ent_node_idx_get(unit,
                                              pt_info,
                                              move_op->d_rbank,
                                              pipe,
                                              move_op->d_slb_info.slb,
                                              &free_node_idx));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_node_allocate(unit,
                                          pt_info,
                                          move_op->d_rbank,
                                          pipe,
                                          move_op->d_slb_info.slb,
                                          &free_node_idx,
                                          &dst_e1_node));
    /* Record the slb offset of the entry node allocated. */
    slb_info_offset = dst_e1_node->ent_slb.slb_info_offset;
    *dst_e1_node = *src_e_node;
    dst_e1_node->ent_slb.slb_info_offset = slb_info_offset;
    bcmptm_rm_hash_ent_slb_info_get(unit,
                                    pt_info,
                                    move_op->d_rbank,
                                    pipe,
                                    slb_info_offset,
                                    &e_slb_info);
    bcmptm_rm_hash_ent_slb_info_get(unit,
                                    pt_info,
                                    move_op->s_rbank,
                                    pipe,
                                    src_e_node->ent_slb.slb_info_offset,
                                    &me_slb_info);
    for (idx = 0; idx < src_e_node->ent_slb.num_slb_info; idx++) {
        e_slb_info[idx] = me_slb_info[idx];
    }
    dst_e1_node->sid = move_op->d_sid;
    dst_e1_node->e_loc = move_op->de_loc;
    dst_e1_node->ent_slb.num_rbanks = 2;
    dst_e1_node->flag |= RM_HASH_NODE_IN_USE;
    /* Other attributes will be assigned by source node. */
    /* Get the sequence number of destination SLB in the destination group. */
    SHR_IF_ERR_EXIT
        (rm_hash_slb_loc_in_grp_get(unit,
                                    pt_info,
                                    move_op->d_rbank,
                                    dst_e1_node,
                                    e_slb_info,
                                    move_op->d_slb_info.slb,
                                    &loc));
    dst_e1_node->ent_slb.e_node_idx[loc] = free_node_idx;

    /*
     * All the elements in entry node have been updated, except bank and
     * entry node index in the paired bank.
     */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_node_insert(unit,
                                        pt_info,
                                        move_op->d_rbank,
                                        pipe,
                                        move_op->d_slb_info.slb,
                                        &nm_info,
                                        dst_e1_node));
    /*
     * Bucket state and entry node has been updated, corresponding
     * undo operation will be recorded in the undo list.
     */
    if (bcmptm_rm_hash_undo_list_maintained(unit) == TRUE) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_trans_undo_node_insert(unit,
                                                   pt_info,
                                                   move_op->d_slb_info.slb,
                                                   pipe,
                                                   dst_e1_node,
                                                   BCMPTM_RM_HASH_UNDO_ENT_DELETE));
    }
    /* Record the 1st entry move into the move list */
    move_op->d_e_node = dst_e1_node;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Conduct an entry move operation in the pending move list.
 *
 * This entry move is corresponding to a move from single-element group to
 * multiple-element group.
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] pipe Pipe instance.
 * \param [in] move_op Pointer to rm_hash_ent_move_op_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_2nd_segrp_to_megrp_node_move(int unit,
                                         rm_hash_pt_info_t *pt_info,
                                         uint8_t pipe,
                                         rm_hash_ent_move_op_t *move_op)
{
    rm_hash_ent_node_t *dst_e1_node = NULL, *dst_e2_node = NULL;
    uint8_t idx, free_node_idx = 0, loc = 0;
    rm_hash_ent_slb_info_t *e_slb_info = NULL, *me_slb_info;
    uint32_t slb_info_offset;
    rm_hash_ent_move_op_t *parent_move_op = NULL;
    rm_hash_nme_info_t nm_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&nm_info, 0, sizeof(nm_info));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_free_ent_node_idx_get(unit,
                                              pt_info,
                                              move_op->d_rbank,
                                              pipe,
                                              move_op->d_slb_info.slb,
                                              &free_node_idx));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_node_allocate(unit,
                                          pt_info,
                                          move_op->d_rbank,
                                          pipe,
                                          move_op->d_slb_info.slb,
                                          &free_node_idx,
                                          &dst_e2_node));
    /* Record the slb offset of the entry node allocated. */
    slb_info_offset = dst_e2_node->ent_slb.slb_info_offset;
    /* The 1st entry move has been performed and obtain the updated 1st entry node. */
    parent_move_op = move_op - move_op->parent_offset;
    dst_e1_node = parent_move_op->d_e_node;
    *dst_e2_node = *dst_e1_node;
    dst_e2_node->ent_slb.slb_info_offset = slb_info_offset;
    bcmptm_rm_hash_ent_slb_info_get(unit,
                                    pt_info,
                                    move_op->d_rbank,
                                    pipe,
                                    slb_info_offset,
                                    &e_slb_info);
    bcmptm_rm_hash_ent_slb_info_get(unit,
                                    pt_info,
                                    parent_move_op->d_rbank,
                                    pipe,
                                    dst_e1_node->ent_slb.slb_info_offset,
                                    &me_slb_info);
    for (idx = 0; idx < dst_e1_node->ent_slb.num_slb_info; idx++) {
        e_slb_info[idx] = me_slb_info[idx];
    }
    dst_e2_node->sid   = move_op->d_sid;
    dst_e2_node->e_loc = move_op->de_loc;
    dst_e2_node->ent_slb.num_rbanks = 2;
    dst_e2_node->flag |= RM_HASH_NODE_IN_USE;
    /* Other attributes will be assigned by source node. */
    /* Get the sequence number of destination SLB in the destination group. */
    SHR_IF_ERR_EXIT
        (rm_hash_slb_loc_in_grp_get(unit,
                                    pt_info,
                                    move_op->d_rbank,
                                    dst_e2_node,
                                    e_slb_info,
                                    move_op->d_slb_info.slb,
                                    &loc));
    dst_e2_node->ent_slb.e_node_idx[loc] = free_node_idx;

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_node_insert(unit,
                                        pt_info,
                                        move_op->d_rbank,
                                        pipe,
                                        move_op->d_slb_info.slb,
                                        &nm_info,
                                        dst_e2_node));
    /*
     * Bucket state and entry node has been updated, corresponding
     * undo operation will be recorded in the undo list.
     */
    if (bcmptm_rm_hash_undo_list_maintained(unit) == TRUE) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_trans_undo_node_insert(unit,
                                                   pt_info,
                                                   move_op->d_slb_info.slb,
                                                   pipe,
                                                   dst_e2_node,
                                                   BCMPTM_RM_HASH_UNDO_ENT_DELETE));
    }
    /* The entry node for the parent should be updated as well. */
    dst_e1_node->ent_slb.e_node_idx[loc] = free_node_idx;
    if ((bcmptm_rm_hash_undo_list_maintained(unit) == TRUE) &&
        (dst_e1_node->ent_slb.num_rbanks > 1)){
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_trans_undo_node_update(unit,
                                                   pt_info,
                                                   parent_move_op->d_slb_info.slb,
                                                   dst_e1_node));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Conduct an entry move operation in the pending move list.
 *
 * This entry move is corresponding to a move from single-element group to
 * single-element group.
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] pipe Pipe instance.
 * \param [in] move_op Pointer to rm_hash_ent_move_op_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_segrp_to_segrp_node_move(int unit,
                                     rm_hash_pt_info_t *pt_info,
                                     uint8_t pipe,
                                     rm_hash_ent_move_op_t *move_op)
{
    rm_hash_ent_node_t *src_ent = NULL, *dst_ent = NULL;
    uint8_t idx, free_node_idx = 0;
    rm_hash_ent_slb_info_t *e_slb_info = NULL, *me_slb_info;
    uint32_t slb_info_offset;
    rm_hash_nme_info_t nm_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&nm_info, 0, sizeof(nm_info));
    /* Delete the entry node in the source bucket. */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_node_delete(unit,
                                        pt_info,
                                        move_op->s_rbank,
                                        pipe,
                                        move_op->s_slb_info.slb,
                                        move_op->se_loc.bb_bmp,
                                        move_op->se_loc.be_bmp,
                                        NULL,
                                        &src_ent));
    /*
     * Bucket state and entry node has been updated, corresponding
     * undo operation will be recorded in the undo list.
     */
    if (bcmptm_rm_hash_undo_list_maintained(unit) == TRUE) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_trans_undo_node_insert(unit,
                                                   pt_info,
                                                   move_op->s_slb_info.slb,
                                                   pipe,
                                                   src_ent,
                                                   BCMPTM_RM_HASH_UNDO_ENT_INSERT));
    }
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_free_ent_node_idx_get(unit,
                                              pt_info,
                                              move_op->d_rbank,
                                              pipe,
                                              move_op->d_slb_info.slb,
                                              &free_node_idx));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_node_allocate(unit,
                                          pt_info,
                                          move_op->d_rbank,
                                          pipe,
                                          move_op->d_slb_info.slb,
                                          &free_node_idx,
                                          &dst_ent));
    /* Record the slb offset of the entry node allocated. */
    slb_info_offset = dst_ent->ent_slb.slb_info_offset;
    *dst_ent = *src_ent;
    dst_ent->ent_slb.slb_info_offset = slb_info_offset;
    bcmptm_rm_hash_ent_slb_info_get(unit,
                                    pt_info,
                                    move_op->d_rbank,
                                    pipe,
                                    slb_info_offset,
                                    &e_slb_info);
    bcmptm_rm_hash_ent_slb_info_get(unit,
                                    pt_info,
                                    move_op->s_rbank,
                                    pipe,
                                    src_ent->ent_slb.slb_info_offset,
                                    &me_slb_info);
    for (idx = 0; idx < src_ent->ent_slb.num_slb_info; idx++) {
        e_slb_info[idx] = me_slb_info[idx];
    }
    dst_ent->sid   = move_op->d_sid;
    dst_ent->e_loc = move_op->de_loc;
    dst_ent->ent_slb.num_rbanks = 1;
    dst_ent->ent_slb.e_node_idx[0] = free_node_idx;
    dst_ent->flag |= RM_HASH_NODE_IN_USE;
    /* Other attributes will be assigned by source node. */

    /*
     * All the elements in entry node have been updated, except bank and
     * entry node index in the paired bank.
     */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_node_insert(unit,
                                        pt_info,
                                        move_op->d_rbank,
                                        pipe,
                                        move_op->d_slb_info.slb,
                                        &nm_info,
                                        dst_ent));
    /*
     * Bucket state and entry node has been updated, corresponding
     * undo operation will be recorded in the undo list.
     */
    if (bcmptm_rm_hash_undo_list_maintained(unit) == TRUE) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_trans_undo_node_insert(unit,
                                                   pt_info,
                                                   move_op->d_slb_info.slb,
                                                   pipe,
                                                   dst_ent,
                                                   BCMPTM_RM_HASH_UNDO_ENT_DELETE));
    }
    move_op->d_e_node = dst_ent;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Conduct an entry move operation in the pending move list.
 *
 * This entry move is corresponding to a move from multiple-element group to
 * single-element group.
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] pipe Pipe instance.
 * \param [in] move_op Pointer to rm_hash_ent_move_op_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_1st_megrp_to_segrp_node_move(int unit,
                                         rm_hash_pt_info_t *pt_info,
                                         uint8_t pipe,
                                         rm_hash_ent_move_op_t *move_op)
{
    rm_hash_ent_node_t *src_e1_node = NULL, *dst_ent = NULL;
    uint8_t idx, free_node_idx = 0;
    rm_hash_ent_slb_info_t *e_slb_info = NULL, *me_slb_info;
    uint32_t slb_info_offset;
    rm_hash_nme_info_t nm_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&nm_info, 0, sizeof(nm_info));
    /* Delete the entry node in the source bucket. */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_node_delete(unit,
                                        pt_info,
                                        move_op->s_rbank,
                                        pipe,
                                        move_op->s_slb_info.slb,
                                        move_op->se_loc.bb_bmp,
                                        move_op->se_loc.be_bmp,
                                        NULL,
                                        &src_e1_node));
    /*
     * Bucket state and entry node has been updated, corresponding
     * undo operation will be recorded in the undo list.
     */
    if (bcmptm_rm_hash_undo_list_maintained(unit) == TRUE) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_trans_undo_node_insert(unit,
                                                   pt_info,
                                                   move_op->s_slb_info.slb,
                                                   pipe,
                                                   src_e1_node,
                                                   BCMPTM_RM_HASH_UNDO_ENT_INSERT));
    }
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_free_ent_node_idx_get(unit,
                                              pt_info,
                                              move_op->d_rbank,
                                              pipe,
                                              move_op->d_slb_info.slb,
                                              &free_node_idx));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_node_allocate(unit,
                                          pt_info,
                                          move_op->d_rbank,
                                          pipe,
                                          move_op->d_slb_info.slb,
                                          &free_node_idx,
                                          &dst_ent));
    /* Record the slb offset of the entry node allocated. */
    slb_info_offset = dst_ent->ent_slb.slb_info_offset;
    *dst_ent = *src_e1_node;
    dst_ent->ent_slb.slb_info_offset = slb_info_offset;
    bcmptm_rm_hash_ent_slb_info_get(unit,
                                    pt_info,
                                    move_op->d_rbank,
                                    pipe,
                                    slb_info_offset,
                                    &e_slb_info);
    bcmptm_rm_hash_ent_slb_info_get(unit,
                                    pt_info,
                                    move_op->s_rbank,
                                    pipe,
                                    src_e1_node->ent_slb.slb_info_offset,
                                    &me_slb_info);
    for (idx = 0; idx < src_e1_node->ent_slb.num_slb_info; idx++) {
        e_slb_info[idx] = me_slb_info[idx];
    }
    dst_ent->sid = move_op->d_sid;
    dst_ent->e_loc = move_op->de_loc;
    dst_ent->ent_slb.num_rbanks = 1;
    dst_ent->ent_slb.e_node_idx[0] = free_node_idx;
    dst_ent->flag |= RM_HASH_NODE_IN_USE;
    /* Other attributes will be assigned by source node. */

    /*
     * All the elements in entry node have been updated, except bank and
     * entry node index in the paired bank.
     */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_node_insert(unit,
                                        pt_info,
                                        move_op->d_rbank,
                                        pipe,
                                        move_op->d_slb_info.slb,
                                        &nm_info,
                                        dst_ent));
    /*
     * Bucket state and entry node has been updated, corresponding
     * undo operation will be recorded in the undo list.
     */
    if (bcmptm_rm_hash_undo_list_maintained(unit) == TRUE) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_trans_undo_node_insert(unit,
                                                   pt_info,
                                                   move_op->d_slb_info.slb,
                                                   pipe,
                                                   dst_ent,
                                                   BCMPTM_RM_HASH_UNDO_ENT_DELETE));
    }
    move_op->d_e_node = dst_ent;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Conduct an entry move operation in the pending move list.
 *
 * This entry move is corresponding to a move from multiple-element group to
 * single-element group.
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] pipe Pipe instance.
 * \param [in] move_op Pointer to rm_hash_ent_move_op_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_2nd_megrp_to_segrp_node_move(int unit,
                                         rm_hash_pt_info_t *pt_info,
                                         uint8_t pipe,
                                         rm_hash_ent_move_op_t *move_op)
{
    rm_hash_ent_node_t *src_e2_node = NULL;

    SHR_FUNC_ENTER(unit);

    /* Delete the entry node in the source bucket. */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_node_delete(unit,
                                        pt_info,
                                        move_op->s_rbank,
                                        pipe,
                                        move_op->s_slb_info.slb,
                                        move_op->se_loc.bb_bmp,
                                        move_op->se_loc.be_bmp,
                                        NULL,
                                        &src_e2_node));
    /*
     * Bucket state and entry node has been updated, corresponding
     * undo operation will be recorded in the undo list.
     */
    if (bcmptm_rm_hash_undo_list_maintained(unit) == TRUE) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_trans_undo_node_insert(unit,
                                                   pt_info,
                                                   move_op->s_slb_info.slb,
                                                   pipe,
                                                   src_e2_node,
                                                   BCMPTM_RM_HASH_UNDO_ENT_INSERT));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Conduct an entry move operation in the pending move list.
 *
 * This entry move is corresponding to a move from multiple-element group to
 * multiple-element group.
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] pipe Pipe instance.
 * \param [in] move_op Pointer to rm_hash_ent_move_op_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_1st_megrp_to_megrp_node_move(int unit,
                                         rm_hash_pt_info_t *pt_info,
                                         uint8_t pipe,
                                         rm_hash_ent_move_op_t *move_op)
{
    rm_hash_ent_node_t *src_e1_node = NULL, *dst_e1_node = NULL;
    uint8_t idx, free_node_idx = 0, loc = 0;
    rm_hash_ent_slb_info_t *e_slb_info = NULL, *me_slb_info = NULL;
    uint32_t slb_info_offset;
    rm_hash_nme_info_t nm_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&nm_info, 0, sizeof(nm_info));
    /* Delete the entry node in the source bucket. */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_node_delete(unit,
                                        pt_info,
                                        move_op->s_rbank,
                                        pipe,
                                        move_op->s_slb_info.slb,
                                        move_op->se_loc.bb_bmp,
                                        move_op->se_loc.be_bmp,
                                        NULL,
                                        &src_e1_node));
    /*
     * Bucket state and entry node has been updated, corresponding
     * undo operation will be recorded in the undo list.
     */
    if (bcmptm_rm_hash_undo_list_maintained(unit) == TRUE) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_trans_undo_node_insert(unit,
                                                   pt_info,
                                                   move_op->s_slb_info.slb,
                                                   pipe,
                                                   src_e1_node,
                                                   BCMPTM_RM_HASH_UNDO_ENT_INSERT));
    }
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_free_ent_node_idx_get(unit,
                                              pt_info,
                                              move_op->d_rbank,
                                              pipe,
                                              move_op->d_slb_info.slb,
                                              &free_node_idx));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_node_allocate(unit,
                                          pt_info,
                                          move_op->d_rbank,
                                          pipe,
                                          move_op->d_slb_info.slb,
                                          &free_node_idx,
                                          &dst_e1_node));
    /* Record the slb offset of the entry node allocated. */
    slb_info_offset = dst_e1_node->ent_slb.slb_info_offset;
    *dst_e1_node = *src_e1_node;
    dst_e1_node->ent_slb.slb_info_offset = slb_info_offset;
    bcmptm_rm_hash_ent_slb_info_get(unit,
                                    pt_info,
                                    move_op->d_rbank,
                                    pipe,
                                    slb_info_offset,
                                    &e_slb_info);
    bcmptm_rm_hash_ent_slb_info_get(unit,
                                    pt_info,
                                    move_op->s_rbank,
                                    pipe,
                                    src_e1_node->ent_slb.slb_info_offset,
                                    &me_slb_info);
    for (idx = 0; idx < src_e1_node->ent_slb.num_slb_info; idx++) {
        e_slb_info[idx] = me_slb_info[idx];
    }
    dst_e1_node->sid = move_op->d_sid;
    dst_e1_node->e_loc = move_op->de_loc;
    /* Get the sequence number of destination SLB in the destination group. */
    SHR_IF_ERR_EXIT
        (rm_hash_slb_loc_in_grp_get(unit,
                                    pt_info,
                                    move_op->d_rbank,
                                    dst_e1_node,
                                    e_slb_info,
                                    move_op->d_slb_info.slb,
                                    &loc));
    dst_e1_node->ent_slb.e_node_idx[loc] = free_node_idx;
    dst_e1_node->flag |= RM_HASH_NODE_IN_USE;
    /* Other attributes will be assigned by source node. */

    /*
     * All the elements in entry node have been updated, except bank and
     * entry node index in the paired bank.
     */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_node_insert(unit,
                                        pt_info,
                                        move_op->d_rbank,
                                        pipe,
                                        move_op->d_slb_info.slb,
                                        &nm_info,
                                        dst_e1_node));
    /*
     * Bucket state and entry node has been updated, corresponding
     * undo operation will be recorded in the undo list.
     */
    if (bcmptm_rm_hash_undo_list_maintained(unit) == TRUE) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_trans_undo_node_insert(unit,
                                                   pt_info,
                                                   move_op->d_slb_info.slb,
                                                   pipe,
                                                   dst_e1_node,
                                                   BCMPTM_RM_HASH_UNDO_ENT_DELETE));
    }
    move_op->d_e_node = dst_e1_node;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Conduct an entry move operation in the pending move list.
 *
 * This entry move is corresponding to a move from multiple-element group to
 * multiple-element group.
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] pipe Pipe instance.
 * \param [in] move_op Pointer to rm_hash_ent_move_op_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_2nd_megrp_to_megrp_node_move(int unit,
                                         rm_hash_pt_info_t *pt_info,
                                         uint8_t pipe,
                                         rm_hash_ent_move_op_t *move_op)
{
    rm_hash_ent_node_t *src_e2_node = NULL, *dst_e2_node = NULL, *dst_e1_node = NULL;
    uint8_t idx, free_node_idx = 0, loc = 0;
    rm_hash_ent_slb_info_t *e_slb_info = NULL, *me_slb_info;
    uint32_t slb_info_offset;
    rm_hash_ent_move_op_t *parent_move_op = NULL;
    rm_hash_nme_info_t nm_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&nm_info, 0, sizeof(nm_info));
    if (move_op->opcode == RM_HASH_ENTRY_DELETE) {
        /* For the delete operation, there is no entry node to operate. */
        SHR_EXIT();
    }

    parent_move_op = move_op - move_op->parent_offset;
    dst_e1_node = parent_move_op->d_e_node;

    /* Delete the entry node in the source bucket. */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_node_delete(unit,
                                        pt_info,
                                        move_op->s_rbank,
                                        pipe,
                                        move_op->s_slb_info.slb,
                                        move_op->se_loc.bb_bmp,
                                        move_op->se_loc.be_bmp,
                                        NULL,
                                        &src_e2_node));
    /*
     * Bucket state and entry node has been updated, corresponding
     * undo operation will be recorded in the undo list.
     */
    if (bcmptm_rm_hash_undo_list_maintained(unit) == TRUE) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_trans_undo_node_insert(unit,
                                                   pt_info,
                                                   move_op->s_slb_info.slb,
                                                   pipe,
                                                   src_e2_node,
                                                   BCMPTM_RM_HASH_UNDO_ENT_INSERT));
    }
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_free_ent_node_idx_get(unit,
                                              pt_info,
                                              move_op->d_rbank,
                                              pipe,
                                              move_op->d_slb_info.slb,
                                              &free_node_idx));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_node_allocate(unit,
                                          pt_info,
                                          move_op->d_rbank,
                                          pipe,
                                          move_op->d_slb_info.slb,
                                          &free_node_idx,
                                          &dst_e2_node));
    /* Record the slb offset of the entry node allocated. */
    slb_info_offset = dst_e2_node->ent_slb.slb_info_offset;
    *dst_e2_node = *dst_e1_node;
    dst_e2_node->ent_slb.slb_info_offset = slb_info_offset;
    bcmptm_rm_hash_ent_slb_info_get(unit,
                                    pt_info,
                                    move_op->d_rbank,
                                    pipe,
                                    slb_info_offset,
                                    &e_slb_info);
    bcmptm_rm_hash_ent_slb_info_get(unit,
                                    pt_info,
                                    parent_move_op->d_rbank,
                                    pipe,
                                    dst_e1_node->ent_slb.slb_info_offset,
                                    &me_slb_info);
    for (idx = 0; idx < src_e2_node->ent_slb.num_slb_info; idx++) {
        e_slb_info[idx] = me_slb_info[idx];
    }
    dst_e2_node->sid = move_op->d_sid;
    dst_e2_node->e_loc = move_op->de_loc;
    /* Get the sequence number of destination SLB in the destination group. */
    SHR_IF_ERR_EXIT
        (rm_hash_slb_loc_in_grp_get(unit,
                                    pt_info,
                                    move_op->d_rbank,
                                    dst_e2_node,
                                    e_slb_info,
                                    move_op->d_slb_info.slb,
                                    &loc));
    dst_e2_node->ent_slb.e_node_idx[loc] = free_node_idx;
    dst_e2_node->flag |= RM_HASH_NODE_IN_USE;
    /* Other attributes will be assigned by source node. */
    /*
     * All the elements in entry node have been updated, except bank and
     * entry node index in the paired bank.
     */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_node_insert(unit,
                                        pt_info,
                                        move_op->d_rbank,
                                        pipe,
                                        move_op->d_slb_info.slb,
                                        &nm_info,
                                        dst_e2_node));
    /*
     * Bucket state and entry node has been updated, corresponding
     * undo operation will be recorded in the undo list.
     */
    if (bcmptm_rm_hash_undo_list_maintained(unit) == TRUE) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_trans_undo_node_insert(unit,
                                                   pt_info,
                                                   move_op->d_slb_info.slb,
                                                   pipe,
                                                   dst_e2_node,
                                                   BCMPTM_RM_HASH_UNDO_ENT_DELETE));
    }
    /* The entry node for the parent should be updated as well. */
    dst_e1_node->ent_slb.e_node_idx[loc] = free_node_idx;
    if ((bcmptm_rm_hash_undo_list_maintained(unit) == TRUE) &&
        (dst_e1_node->ent_slb.num_rbanks > 1)) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_trans_undo_node_update(unit,
                                                   pt_info,
                                                   parent_move_op->d_slb_info.slb,
                                                   dst_e1_node));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Move an entry node from a bkt to another
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to lt control structure of the PT.
 * \param [in] pipe Pipe instance.
 * \param [in] move_op Pointer to rm_hash_ent_move_op_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_node_move(int unit,
                      rm_hash_pt_info_t *pt_info,
                      uint8_t pipe,
                      rm_hash_ent_move_op_t *move_op)
{

    SHR_FUNC_ENTER(unit);

    if (move_op->parent_offset == 0) {
        /* For those entry move operations generated by stack based analysis. */
        switch (move_op->move_op_type) {
        case RM_HASH_SEGRP_TO_MEGRP_MOVE:
            SHR_IF_ERR_EXIT
                (rm_hash_ent_1st_segrp_to_megrp_node_move(unit, pt_info, pipe, move_op));
            break;
        case RM_HASH_MEGRP_TO_MEGRP_MOVE:
            SHR_IF_ERR_EXIT
                (rm_hash_ent_1st_megrp_to_megrp_node_move(unit, pt_info, pipe, move_op));
            break;
        case RM_HASH_MEGRP_TO_SEGRP_MOVE:
            SHR_IF_ERR_EXIT
                (rm_hash_ent_1st_megrp_to_segrp_node_move(unit, pt_info, pipe, move_op));
            break;
        case RM_HASH_SEGRP_TO_SEGRP_MOVE:
        case RM_HASH_UNKNOWN_MOVE:
            SHR_IF_ERR_EXIT
                (rm_hash_ent_segrp_to_segrp_node_move(unit, pt_info, pipe, move_op));
            break;
        default:
            break;
        }
    } else {
        /* For those entry move operations generated post stack based analysis. */
        switch (move_op->move_op_type) {
        case RM_HASH_SEGRP_TO_MEGRP_MOVE:
            SHR_IF_ERR_EXIT
                (rm_hash_ent_2nd_segrp_to_megrp_node_move(unit, pt_info, pipe, move_op));
            break;
        case RM_HASH_MEGRP_TO_MEGRP_MOVE:
            SHR_IF_ERR_EXIT
                (rm_hash_ent_2nd_megrp_to_megrp_node_move(unit, pt_info, pipe, move_op));
            break;
        case RM_HASH_MEGRP_TO_SEGRP_MOVE:
            SHR_IF_ERR_EXIT
                (rm_hash_ent_2nd_megrp_to_segrp_node_move(unit, pt_info, pipe, move_op));
            break;
        default:
            break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update software bkt state according to move operation.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] pipe Pipe instance.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_move_bkt_state_update(int unit, rm_hash_pt_info_t *pt_info, uint8_t pipe)
{
    rm_hash_ent_move_op_t *move_op = NULL;
    int idx;
    bool ret = FALSE;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; ; idx++) {
        ret = rm_hash_ent_move_op_get(unit, idx, &move_op);
        if (ret == FALSE) {
            SHR_EXIT();
        }

        SHR_IF_ERR_EXIT
            (rm_hash_ent_node_move(unit, pt_info, pipe, move_op));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Examine if a bucket has enough space to store an entry.
 *
 * \param [in] unit Unit number.
 * \param [in] req_e_size Requested entry size.
 * \param [in] vbb_bmp Valid base bucket bitmap for the requested entry.
 * \param [in] req_e_bm Bucket mode of the requested entry.
 * \param [in] b_state Software logical bucket state.
 * \param [out] accom Flag indicating if free space is available.
 * \param [out] e_loc Detailed information about location.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */

static int
rm_hash_slb_can_hold(int unit,
                     rm_hash_pt_info_t *pt_info,
                     uint8_t req_e_size,
                     uint8_t vbb_bmp,
                     rm_hash_bm_t req_e_bm,
                     rm_hash_slb_state_t *b_state,
                     bool *accom,
                     rm_hash_ent_loc_t *e_loc)
{
    rm_hash_slb_ent_bmp_t e_bmp;
    rm_hash_bm_t bm_idx;
    rm_hash_req_ent_info_t e_info;

    SHR_FUNC_ENTER(unit);

    e_bmp.be_bmp = b_state->e_bmp.be_bmp;
    for (bm_idx = RM_HASH_BM_N; bm_idx < RM_HASH_BM_CNT; bm_idx++) {
        e_bmp.le_bmp[bm_idx] = b_state->e_bmp.le_bmp[bm_idx];
    }

    e_info.e_size = req_e_size;
    e_info.vbb_bmp = vbb_bmp;
    e_info.e_bm = req_e_bm;

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_bkt_free_ent_space_get(unit,
                                               pt_info,
                                               &e_bmp,
                                               &e_info,
                                               accom,
                                               e_loc));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Conduct an entry move operation in hw in the pending move list.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] move_op Pointer to rm_hash_ent_move_op_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_move_update(int unit,
                        rm_hash_req_context_t *req_ctxt,
                        rm_hash_pt_info_t *pt_info,
                        rm_hash_ent_move_op_t *move_op,
                        rm_hash_entry_buf_t *ent_from)
{
    bool rsp_ent_valid = FALSE;
    uint8_t ent_num_move = 0, d_rbank, e_size;
    uint32_t ent_index, dst_ent_index;
    bcmbd_pt_dyn_info_t ent_dyn_info = *req_ctxt->pt_dyn_info;
    rm_hash_ent_loc_t se_loc, de_loc;
    uint32_t cmdproc_rsp_flags = 0, rsp_flags = 0;
    bcmltd_sid_t rsp_ltid;
    uint16_t rsp_dfield_format_id;
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    const bcmptm_rm_hash_view_info_t *view_info = NULL;
    uint8_t grp_idx;
    bool separate = FALSE;

    SHR_FUNC_ENTER(unit);

    ent_num_move = move_op->e_size;
    se_loc = move_op->se_loc;
    de_loc = move_op->de_loc;
    sal_memset(ent_from->e_words, 0, sizeof(ent_from->e_words));

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_index_get(unit,
                                      pt_info,
                                      move_op->s_slb_info.slb,
                                      &se_loc,
                                      ent_num_move,
                                      NULL,
                                      &ent_index));
    ent_dyn_info.index = ent_index;

    SHR_IF_ERR_EXIT
        (bcmptm_cmdproc_read(unit,
                             BCMPTM_REQ_FLAGS_NO_FLAGS,
                             0,
                             move_op->s_sid,
                             &ent_dyn_info,
                             NULL,
                             COUNTOF(ent_from->e_words),
                             ent_from->e_words,
                             &rsp_ent_valid,
                             &rsp_ltid,
                             &rsp_dfield_format_id,
                             &rsp_flags));
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "entry move src mem:[%s], "
                            "entry move src idx:[%d] \n"),
                            bcmdrd_pt_sid_to_name(unit, move_op->s_sid),
                            ent_dyn_info.index));
    /*
     * Currently, it is assumed that key and data fields position are
     * identical among different banks. If it is not true in the future,
     * key and data field exchange will be required.
     */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_index_get(unit,
                                      pt_info,
                                      move_op->d_slb_info.slb,
                                      &de_loc,
                                      ent_num_move,
                                      NULL,
                                      &dst_ent_index));
    ent_dyn_info.index = dst_ent_index;
    SHR_IF_ERR_EXIT
        (bcmptm_cmdproc_write(unit,
                              req_ctxt->req_flags,
                              0,
                              move_op->d_sid,
                              &ent_dyn_info,
                              NULL,
                              ent_from->e_words,
                              TRUE,
                              TRUE,
                              TRUE,
                              BCMPTM_RM_OP_NORMAL, NULL,
                              rsp_ltid,
                              rsp_dfield_format_id,
                              &cmdproc_rsp_flags));
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "entry move dst mem:[%s], "
                            "entry move dst idx:[%d] \n"),
                            bcmdrd_pt_sid_to_name(unit, move_op->d_sid),
                            ent_dyn_info.index));
    d_rbank = move_op->d_rbank;
    e_size = move_op->e_size;
    /* Low power table needs to be updated. */
    if (pt_info->lp_sid[d_rbank] != NULL) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_lp_ent_update(unit,
                                          req_ctxt->pt_dyn_info,
                                          *pt_info->single_view_sid[d_rbank],
                                          *pt_info->key_attrib[d_rbank].attr_sid,
                                          *pt_info->lp_sid[d_rbank],
                                          move_op->e_bm,
                                          e_size,
                                          pt_info->base_bkt_cnt[d_rbank],
                                          dst_ent_index));
    }
    /* This is an extra write to invalidate the vbit in PTcache. */
    ent_dyn_info.index = ent_index;
    sal_memset(ent_from->e_words, 0, sizeof(ent_from->e_words));
    SHR_IF_ERR_EXIT
        (bcmptm_cmdproc_write(unit,
                              req_ctxt->req_flags,
                              0,
                              move_op->s_sid,
                              &ent_dyn_info,
                              NULL,
                              ent_from->e_words,
                              TRUE,
                              TRUE,
                              FALSE,
                              BCMPTM_RM_OP_NORMAL, NULL,
                              rsp_ltid,
                              rsp_dfield_format_id,
                              &cmdproc_rsp_flags));

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_bank_has_separated_sid(unit, &separate));
    if (separate == FALSE) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_lt_ctrl_get(unit, rsp_ltid, &lt_ctrl));
        grp_idx = lt_ctrl->map_info[0].grp_idx[0];
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_base_view_info_get(unit,
                                               &lt_ctrl->lt_info,
                                               grp_idx,
                                               &view_info));
        if (view_info->hit_info != NULL) {
            /* LT has HIT fields and these HIT fields should be updated as well. */
            uint32_t hit_list[RM_HASH_HIT_MAX_CNT];
            sal_memset(hit_list, 0xff, sizeof(hit_list));
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_hit_table_update(unit,
                                                 req_ctxt,
                                                 &ent_dyn_info,
                                                 view_info->hit_info,
                                                 hit_list,
                                                 view_info->hit_info->hit_map_count));
        }
    }

    SHR_IF_ERR_EXIT
        (rm_hash_ent_move_report(unit, rsp_ltid, ent_dyn_info.tbl_inst,
                                 pt_info, move_op, dst_ent_index,
                                 ent_index));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Duplicate an entry move operation in hw in the pending move list.
 *
 * \param [in] unit Unit number.
 * \param [in] req_ctxt Pointer to rm_hash_req_context_t structure.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] move_op Pointer to rm_hash_ent_move_op_t structure.
 * \param [in] ent_from Pointer to rm_hash_entry_buf_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_move_duplicate(int unit,
                           rm_hash_req_context_t *req_ctxt,
                           rm_hash_pt_info_t *pt_info,
                           rm_hash_ent_move_op_t *move_op,
                           rm_hash_entry_buf_t *ent_from)
{
    bool rsp_ent_valid;
    uint8_t ent_num_move = 0, d_rbank, e_size;
    uint32_t ent_index, dst_ent_index;
    bcmbd_pt_dyn_info_t ent_dyn_info = *req_ctxt->pt_dyn_info;
    rm_hash_ent_loc_t se_loc, de_loc;
    uint32_t cmdproc_rsp_flags = 0, rsp_flags = 0;
    bcmltd_sid_t rsp_ltid;
    uint16_t rsp_dfield_format_id;

    SHR_FUNC_ENTER(unit);

    ent_num_move = move_op->e_size;
    se_loc = move_op->se_loc;
    de_loc = move_op->de_loc;
    sal_memset(ent_from->e_words, 0, sizeof(ent_from->e_words));

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_index_get(unit,
                                      pt_info,
                                      move_op->s_slb_info.slb,
                                      &se_loc,
                                      ent_num_move,
                                      NULL,
                                      &ent_index));
    ent_dyn_info.index = ent_index;
    /*
     * By default, the hit bits of the entry will NOT be preserved
     * during entry move. The content of the entries to be moved
     * will be retrieved from PTcache, not from HW table.
     * In the future, a configurable variable may be considered
     * to control if the hit bits need to be preserved. It should
     * be noted that reading HW table to get the hit bit will
     * completely stall Modeled path.
     */
    SHR_IF_ERR_EXIT
        (bcmptm_cmdproc_read(unit,
                             BCMPTM_REQ_FLAGS_NO_FLAGS,
                             0,
                             move_op->s_sid,
                             &ent_dyn_info,
                             NULL,
                             COUNTOF(ent_from->e_words),
                             ent_from->e_words,
                             &rsp_ent_valid,
                             &rsp_ltid,
                             &rsp_dfield_format_id,
                             &rsp_flags));
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "entry move src mem:[%s], "
                            "entry move src idx:[%d] \n"),
                            bcmdrd_pt_sid_to_name(unit, move_op->s_sid),
                            ent_dyn_info.index));
    /*
     * Currently, it is assumed that key and data fields position are
     * identical among different banks. If it is not true in the future,
     * key and data field exchange will be required.
     */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_index_get(unit,
                                      pt_info,
                                      move_op->d_slb_info.slb,
                                      &de_loc,
                                      ent_num_move,
                                      NULL,
                                      &dst_ent_index));
    ent_dyn_info.index = dst_ent_index;
    SHR_IF_ERR_EXIT
        (bcmptm_cmdproc_write(unit,
                              req_ctxt->req_flags,
                              0,
                              move_op->d_sid,
                              &ent_dyn_info,
                              NULL,
                              ent_from->e_words,
                              TRUE,
                              TRUE,
                              TRUE,
                              BCMPTM_RM_OP_NORMAL, NULL,
                              rsp_ltid,
                              rsp_dfield_format_id,
                              &cmdproc_rsp_flags));
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "entry move dst mem:[%s], "
                            "entry move dst idx:[%d] \n"),
                            bcmdrd_pt_sid_to_name(unit, move_op->d_sid),
                            ent_dyn_info.index));
    d_rbank = move_op->d_rbank;
    e_size = move_op->e_size;
    /* Low power table needs to be updated. */
    if (pt_info->lp_sid[d_rbank] != NULL) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_lp_ent_update(unit,
                                          req_ctxt->pt_dyn_info,
                                          *pt_info->single_view_sid[d_rbank],
                                          *pt_info->key_attrib[d_rbank].attr_sid,
                                          *pt_info->lp_sid[d_rbank],
                                          move_op->e_bm,
                                          e_size,
                                          pt_info->base_bkt_cnt[d_rbank],
                                          dst_ent_index));
    }
    SHR_IF_ERR_EXIT
        (rm_hash_ent_move_report(unit, rsp_ltid, ent_dyn_info.tbl_inst,
                                 pt_info, move_op, dst_ent_index,
                                 ent_index));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Conduct an entry clear operation in hw in the pending move list.
 *
 * \param [in] unit Unit number.
 * \param [in] req_ctxt Pointer to rm_hash_req_context_t structure.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] move_op Pointer to rm_hash_ent_move_op_t structure.
 * \param [in] ent_from Pointer to rm_hash_entry_buf_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_move_clear(int unit,
                       rm_hash_req_context_t *req_ctxt,
                       rm_hash_pt_info_t *pt_info,
                       rm_hash_ent_move_op_t *move_op,
                       rm_hash_entry_buf_t *ent_from)
{
    uint8_t ent_num_move = 0;
    uint32_t src_ent_index;
    bcmbd_pt_dyn_info_t ent_dyn_info = *req_ctxt->pt_dyn_info;
    rm_hash_ent_loc_t se_loc;
    uint32_t cmdproc_rsp_flags = 0;
    bcmltd_sid_t rsp_ltid = 0;
    uint16_t rsp_dfield_format_id = 0;

    SHR_FUNC_ENTER(unit);

    ent_num_move = move_op->e_size;
    se_loc = move_op->se_loc;
    sal_memset(ent_from->e_words, 0, sizeof(ent_from->e_words));
    /*
     * Currently, it is assumed that key and data fields position are
     * identical among different banks. If it is not true in the future,
     * key and data field exchange will be required.
     */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_index_get(unit,
                                      pt_info,
                                      move_op->s_slb_info.slb,
                                      &se_loc,
                                      ent_num_move,
                                      NULL,
                                      &src_ent_index));
    ent_dyn_info.index = src_ent_index;
    SHR_IF_ERR_EXIT
        (bcmptm_cmdproc_write(unit,
                              req_ctxt->req_flags,
                              0,
                              move_op->s_sid,
                              &ent_dyn_info,
                              NULL,
                              ent_from->e_words,
                              TRUE,
                              TRUE,
                              TRUE,
                              BCMPTM_RM_OP_NORMAL, NULL,
                              rsp_ltid,
                              rsp_dfield_format_id,
                              &cmdproc_rsp_flags));
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "entry clear mem:[%s], "
                            "entry clear idx:[%d] \n"),
                            bcmdrd_pt_sid_to_name(unit, move_op->s_sid),
                            ent_dyn_info.index));

    /* This is an extra write to invalidate the vbit in PTcache. */
    ent_dyn_info.index = src_ent_index;
    SHR_IF_ERR_EXIT
        (bcmptm_cmdproc_write(unit,
                              req_ctxt->req_flags,
                              0,
                              move_op->s_sid,
                              &ent_dyn_info,
                              NULL,
                              ent_from->e_words,
                              TRUE,
                              TRUE,
                              FALSE,
                              BCMPTM_RM_OP_NORMAL, NULL,
                              rsp_ltid,
                              rsp_dfield_format_id,
                              &cmdproc_rsp_flags));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Conduct memory update according to move operation list.
 *
 * \param [in] unit Unit number.
 * \param [in] req_ctxt Pointer to rm_hash_req_context_t structure.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_move_mem_update(int unit,
                            rm_hash_req_context_t *req_ctxt,
                            rm_hash_pt_info_t *pt_info)
{
    rm_hash_ent_move_op_t *move_op = NULL;
    uint32_t idx = 0;

    uint32_t num_move = 0;
    rm_hash_entry_buf_t *ent_from = NULL;

    SHR_FUNC_ENTER(unit);

    ent_from = bcmptm_rm_hash_entry_buf_alloc();
    if (ent_from == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    num_move = rm_hash_ent_move_num_get(unit);
    for (idx = 0; idx < num_move; idx++) {
        (void)rm_hash_ent_move_op_get(unit, idx, &move_op);
        if (move_op->opcode == RM_HASH_ENTRY_MOVE) {
            SHR_IF_ERR_EXIT
                (rm_hash_ent_move_update(unit, req_ctxt, pt_info, move_op, ent_from));
        } else if (move_op->opcode == RM_HASH_ENTRY_DUPLICATE) {
            SHR_IF_ERR_EXIT
                (rm_hash_ent_move_duplicate(unit, req_ctxt, pt_info, move_op, ent_from));
        } else {
            SHR_IF_ERR_EXIT
                (rm_hash_ent_move_clear(unit, req_ctxt, pt_info, move_op, ent_from));
        }
    }

exit:
    if(ent_from != NULL) {
        bcmptm_rm_hash_entry_buf_free(ent_from);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Filter the entry move list contains both global and per pipe entry.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] per_pipe_insert Flag indicating if the insert is for per pipe.
 * \param [out] mixed Pointer to buffer recording the result.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ent_move_mixed_entry_filter(int unit,
                                    rm_hash_pt_info_t *pt_info,
                                    bool per_pipe_insert,
                                    bool *mixed)
{
    rm_hash_ent_move_op_t *move_op = NULL;
    uint32_t idx = 0, num_move;
    uint8_t flag = 0;

    SHR_FUNC_ENTER(unit);

    *mixed = FALSE;
    if (pt_info->bank_mixed_mode == FALSE) {
        /* There is no mixed mode bank on this device. */
        SHR_EXIT();
    }
    num_move = rm_hash_ent_move_num_get(unit);
    for (idx = 0; idx < num_move; idx++) {
        (void)rm_hash_ent_move_op_get(unit, idx, &move_op);
        flag |= move_op->flag;
    }
    if ((flag & RM_HASH_NODE_GLOBAL) && (per_pipe_insert == TRUE)) {
        /* Don't move global entry for per pipe LT entry insert. */
        *mixed = TRUE;
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_hash_ent_move(int unit,
                        rm_hash_req_context_t *req_ctxt,
                        rm_hash_pt_info_t *pt_info,
                        int depth,
                        uint8_t num_move_req,
                        rm_hash_ent_slb_info_t *slb_info_list,
                        uint8_t ent_size,
                        rm_hash_bm_t e_bm,
                        rm_hash_ent_loc_t *e_loc,
                        uint8_t *rbank)
{
    rm_hash_req_ent_info_t req_e_info_list[RM_HASH_MAX_GRP_INST_COUNT];
    int tmp_rv;
    rm_hash_slb_state_t *b_state = NULL;
    bool accom[RM_HASH_MAX_GRP_INST_COUNT] = {FALSE, FALSE};
    bool mixed_entry_move = FALSE, per_pipe_lt = FALSE;
    uint8_t idx, bank;
    uint8_t pipe;
    bcmbd_pt_dyn_info_t *pt_dyn_info = req_ctxt->pt_dyn_info;

    SHR_FUNC_ENTER(unit);

    pipe = (pt_dyn_info->tbl_inst == -1) ? 0 : pt_dyn_info->tbl_inst;
    per_pipe_lt = (pt_dyn_info->tbl_inst == -1) ? FALSE : TRUE;
    for (idx = 0; idx < num_move_req; idx++) {
        req_e_info_list[idx].e_size = ent_size;
        req_e_info_list[idx].vbb_bmp = slb_info_list[idx].vbb_bmp;
        req_e_info_list[idx].e_bm = e_bm;
    }
    tmp_rv = rm_hash_ent_move_path_get(unit,
                                       pt_info,
                                       pipe,
                                       num_move_req,
                                       slb_info_list,
                                       req_e_info_list,
                                       depth + 1);
    if (tmp_rv == SHR_E_RESOURCE) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    } else {
        SHR_IF_ERR_EXIT(tmp_rv);
    }
    if (e_move_op_list[unit].multi_minst_involved == TRUE) {
        tmp_rv = rm_hash_ent_move_path_update(unit, pt_info, pipe);
        if (tmp_rv == SHR_E_RESOURCE) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        } else {
            SHR_IF_ERR_EXIT(tmp_rv);
        }
    }
    SHR_IF_ERR_EXIT
        (rm_hash_ent_move_mixed_entry_filter(unit, pt_info, per_pipe_lt,
                                             &mixed_entry_move));
    if (mixed_entry_move == TRUE) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }
    SHR_IF_ERR_EXIT
        (rm_hash_ent_move_mem_update(unit, req_ctxt, pt_info));
    SHR_IF_ERR_EXIT
        (rm_hash_ent_move_bkt_state_update(unit, pt_info, pipe));
    for (idx = 0; idx < num_move_req; idx++) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_slb_to_rbank_get(unit, pt_info,
                                             slb_info_list[idx].slb,
                                             &bank));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_slb_state_get(unit,
                                          pt_info,
                                          bank,
                                          pipe,
                                          slb_info_list[idx].slb,
                                          &b_state));
        SHR_IF_ERR_EXIT
            (rm_hash_slb_can_hold(unit,
                                  pt_info,
                                  ent_size,
                                  slb_info_list[idx].vbb_bmp,
                                  e_bm,
                                  b_state,
                                  &accom[idx],
                                  &e_loc[idx]));
        if (accom[idx] == FALSE) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_slb_to_rbank_get(unit,
                                             pt_info,
                                             slb_info_list[idx].slb,
                                             &rbank[idx]));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_ent_move_info_init(int unit)
{
    rm_hash_ent_move_op_t *move_op = NULL;
    size_t op_size = 0;

    SHR_FUNC_ENTER(unit);

    op_size = sizeof(rm_hash_ent_move_op_t) * RM_HASH_ENTRY_MOVE_LIST_DEF_NUM;
    move_op = sal_alloc(op_size, "bcmptmRmhashEntryMoveInfo");
    if (move_op == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(move_op, 0, op_size);
    e_move_op_list[unit].e_move_op = move_op;
    e_move_op_list[unit].count = RM_HASH_ENTRY_MOVE_LIST_DEF_NUM;
    e_move_op_list[unit].multi_minst_involved = FALSE;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_ent_move_info_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (e_move_op_list[unit].e_move_op != NULL) {
        sal_free(e_move_op_list[unit].e_move_op);
        e_move_op_list[unit].e_move_op = NULL;
        e_move_op_list[unit].count = 0;
    }

    SHR_FUNC_EXIT();
}
