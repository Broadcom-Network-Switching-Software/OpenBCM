/*! \file bcmltm_md_lt_index.c
 *
 * Logical Table Manager - Index Logical Table.
 *
 * This file contains implementation to support metadata construction
 * for Index Logical Tables with Physical Table destination groups mapping.
 * This driver is for logical tables with no field selection.
 *
 * The coverage of this Index Logical Table driver includes these
 * variations:
 *     Simple Index           Non-Overlay
 *     Simple Index           Overlay
 *     Index with Allocation  Non-Overlay
 *     Index with Allocation  Overlay
 *
 * LT Field Mappings
 * -----------------
 * LT field maps can fall into either of these categories:
 *   1) Full
 *      The LT mapping completely describes one or more physical tables.
 *      This means that all physical fields in a physical table is
 *      mapped within a LT.
 *
 *   2) Split-Entry
 *      PT fields are not fully mapped in a LT.
 *      The LT maps only to a subset of fields in the physical table(s),
 *      so the field mappings do not completely cover all the physical fields
 *      in the corresponding mapped PTs.
 *
 * Example for Full Map:
 *   PT1: has fields             PT1_F1 PT1_F2
 *   LT1: has fields that map to PT1_F1 PT1_F2 (all the PT fields).
 *
 * Example for Split-Entry Map:
 *   PT1: has fields  PT1_F1 PT1_F2 PT1_F3 PT1_F4
 *   LTs: LT1: fields map to PT1_F1
 *        LT2: fields map to PT1_F2 and PT1_F3
 * Note that even if LT2 did not exist (i.e. only LT1 is defined),
 * LT1 will still need be handled as a LT with a split-entry PT map.
 *
 * The split-entry characteristic affects the tree structures for
 * the INSERT and DELETE opcodes slightly.  A split-entry mapped LT
 * needs an addition PTM read operation to get the entire HW entry
 * so only those unmapped PT fields are not modified.
 *
 * The LOOKUP and UPDATE tree structures are the same for split-entry
 * or full maps.
 *
 * Transforms and Validations
 * --------------------------
 * Field Transforms (Xfrm) and Field Validations (FV) clusters are
 * added to the different opcode trees as needed to process the
 * arguments.
 *
 * For more details, see corresponding description in routines
 * that create the clusters of Field Transform and Field Validation
 * trees.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * FIXME: SDKLT-14070
 * Future improvement is to have an LRD indication to identify
 * if a LT mapping is either full or split.
 * For now, all the opcodes assume LTs have split-entry mapping.
 * This results in an extra PTM read operation on LTs that
 * do not need it (i.e. full map).
 */

#include <shr/shr_types.h>
#include <shr/shr_debug.h>

#include <bsl/bsl.h>

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_lt_internal.h>
#include <bcmltm/bcmltm_ee_tree_nodes_internal.h>
#include <bcmltm/bcmltm_fa_tree_nodes_internal.h>
#include <bcmltm/bcmltm_db_internal.h>

#include "bcmltm_md_lt_drv.h"
#include "bcmltm_md_op.h"
#include "bcmltm_md_lta_xfrm.h"
#include "bcmltm_md_lta_validate.h"


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA


/* Number of roots for INSERT operation */
#define LT_OP_INSERT_NUM_ROOTS   2

/* Number of roots for LOOKUP operation */
#define LT_OP_LOOKUP_NUM_ROOTS   2

/* Number of roots for DELETE operation */
#define LT_OP_DELETE_NUM_ROOTS   2

/* Number of roots for UPDATE operation */
#define LT_OP_UPDATE_NUM_ROOTS   2

/* Number of roots for TRAVERSE operation */
#define LT_OP_TRAVERSE_NUM_ROOTS 2


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Create FA tree roots for INSERT on Index LT.
 *
 * This function creates the FA tree roots for a INSERT operation
 * on given Index logical table.
 *
 * Unspecified input fields values are reset to the SW
 * defaults as specified in the LRD map.
 *
 * This routine implements the Disjoint LT design.  The resulting
 * tree can be used for LTs with full or split-entry mapped fields.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_fa_node_roots_insert_create(int unit,
                                     bcmlrd_sid_t sid,
                                     const bcmltm_table_attr_t *attr,
                                     bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bool iwa = FALSE;
    bool shared = FALSE;
    bool track_index_map = FALSE;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *track_t2p_node = NULL;
    bcmltm_node_t *track_index_node = NULL;
    bcmltm_node_t *key_fwd_xfrm_node = NULL;
    bcmltm_node_t *key_fv_node = NULL;
    bcmltm_node_t *ro_check_node = NULL;
    bcmltm_node_t *wide_field_node = NULL;
    bcmltm_node_t *key_req_a2p_node = NULL;
    bcmltm_node_t *key_alloc_a2p_node = NULL;
    bcmltm_node_t *fixed_key_node = NULL;
    bcmltm_node_t *track_dec_node = NULL;
    bcmltm_node_t *track_alloc_node = NULL;
    bcmltm_node_t *value_a2p_node = NULL;
    bcmltm_node_t *value_fwd_xfrm_node = NULL;
    bcmltm_node_t *value_fv_node = NULL;
    bcmltm_node_t *value_def_node = NULL;
    bcmltm_node_t *fixed_value_node = NULL;
    bcmltm_node_t *key_rev_xfrm_node = NULL;
    bcmltm_node_t *key_alloc_p2a_node = NULL;
    bcmltm_fa_node_t *track_t2p_nd = NULL;
    bcmltm_fa_node_t *track_index_nd = NULL;
    bcmltm_fa_node_t *key_req_a2p_nd = NULL;
    bcmltm_fa_node_t *key_alloc_a2p_nd = NULL;
    bcmltm_fa_node_t *fixed_key_nd = NULL;
    bcmltm_fa_node_t *ro_check_nd = NULL;
    bcmltm_fa_node_t *wide_field_nd = NULL;
    bcmltm_fa_node_t *track_dec_nd = NULL;
    bcmltm_fa_node_t *track_alloc_nd = NULL;
    bcmltm_fa_node_t *value_a2p_nd = NULL;
    bcmltm_fa_node_t *value_def_nd = NULL;
    bcmltm_fa_node_t *fixed_value_nd = NULL;
    bcmltm_fa_node_t *key_alloc_p2a_nd = NULL;
    const bcmltm_wb_copy_list_t *track_t2p_list = NULL;
    const bcmltm_track_index_t *track_info = NULL;
    const bcmltm_field_select_mapping_t *key_req_map = NULL;
    const bcmltm_field_select_mapping_t *key_alloc_map = NULL;
    const bcmltm_field_select_mapping_t *value_map = NULL;
    const bcmltm_fixed_field_list_t *fixed_keys = NULL;
    const bcmltm_fixed_field_list_t *fixed_values = NULL;
    const bcmltm_ro_field_list_t *ro_list = NULL;
    const bcmltm_wide_field_list_t *wide_list = NULL;
    bcmltm_fa_node_f node_func = NULL;
    int xfrm_type;
    bcmltm_md_lta_fopt_t xfrm_lta_fopt = 0x0;
    bcmltm_field_type_t field_type;

    SHR_FUNC_ENTER(unit);

    /* Get Index table properties */
    if (BCMLTM_TABLE_ATTR_TYPE_IS_IWA(*attr)) {
        iwa = TRUE;
    }
    if (BCMLTM_TABLE_ATTR_IS_SHARED(*attr)) {
        shared = TRUE;
    }

    /* Track index: derived from map or LTM */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_track_index_is_mapped(unit, sid, &track_index_map));

    /* Allocate pointers to FA tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, size, "bcmltmIndexInsertFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, size);

    /*
     * INSERT - Disjoint
     *
     * Unless specified, the node applies to all Index LT types:
     * Simple Index and Index With Allocation, both as Shared and no-Shared.
     *
     * FA Tree Roots
     * -----------------------------------------------------------------
     * Tree_0:
     *                                  [Track_T2P]
     *                                        |
     *                          +-------------+-------------+
     *                          |                           |
     *                    [Track_Index]               [Track_Dec]
     *                          |                           |
     *                 +--------+--------+         +--------+--------+
     *                 |                 |         |                 |
     *           [Key_Fwd_Xfrm]   [Key_Req_A2P]    =           [Track_Alloc]
     *                 |                 |
     *           +-----+-----+     +-----+-----+
     *           |           |     |           |
     *      [Key_FV]         = [Fixed_Key] [Key_Alloc_A2P]
     *
     * Execution:
     * [Key_FV]       : bcmltm_md_validate_fa_trees_create
     * [Key_Fwd_Xfrm] : <bcmltm_md_xfrm_fa_trees_fwd_create> (SI)
     *                  <bcmltm_md_xfrm_fa_trees_fwd_alloc_create> (IwA)
     * [Fixed_Key]    : bcmltm_fa_node_fixed_fields_to_wb
     * [Key_Alloc_A2P]: bcmltm_fa_node_api_alloc_key_fields_to_wb (IwA)
     * [Key_Req_A2P]  : bcmltm_fa_node_api_key_fields_to_wb
     * [Track_Index]  : bcmltm_fa_node_wbc_to_track (No-TI-Map)
     * [Track_Alloc]  : <bcmltm_fa_node_alloc_track> (IwA:No-Shared)
     *                  <bcmltm_fa_node_global_alloc_track> (IwA:Shared)
     * [Track_Dec]    : bcmltm_fa_node_alloc_track_to_wbc (IwA:No-TI-Map)
     * [Track_T2P]    : bcmltm_fa_node_alloc_wbc_to_wbc (IwA)
     *
     * -----------------------------------------------------------------
     * Tree_1:
     *                               [Value_A2P]
     *                                     |
     *                        +------------+------------+
     *                        |                         |
     *                 [Value_Fwd_Xfrm]              [Fixed_Value]
     *                        |                         |
     *               +--------+--------+       +--------+--------+
     *               |                 |       |                 |
     *           [Value_FV]            =  [Key_Rev_Xfrm]   [Key_Alloc_P2A]
     *               |
     *         +-----+-----+
     *         |           |
     *    [Value_Def]      =
     *           |
     *     +-----+-----+
     *     |           |
     * [RO_Check]   [Wide_Value]
     *
     * Execution:
     * [RO_Check]       : bcmltm_fa_node_read_only_input_check
     * [Wide_Value]     : bcmltm_fa_node_wide_field_check
     * [Value_Def]      : bcmltm_fa_node_default_fields_to_wb
     * [Value_FV]       : bcmltm_md_validate_fa_trees_create
     * [Value_Fwd_Xfrm] : bcmltm_md_xfrm_fa_trees_fwd_create
     * [Key_Rev_Xfrm]   : bcmltm_md_xfrm_fa_trees_rev_alloc_create (IwA)
     * [Key_Alloc_P2A]  : bcmltm_fa_node_wb_to_api_alloc_fields (IwA)
     * [Fixed_Value]    : bcmltm_fa_node_fixed_fields_to_wb
     * [Value_A2P]      : bcmltm_fa_node_api_value_fields_to_wb
     *
     * -----------------------------------------------------------------
     * For more information on Field Transform <Xfrm> and
     * Field Validation <FV> clusters, please see description
     * at the top of this file.
     *
     * Note: No-TI-Map track index is supplied by LTM.
     *       TI-Map    track index is supplied by map field transform.
 */

    /*-----------------
     * Tree_0
     */

    /* [Track_T2P]: Copy tracking elements to PT fields (IwA) */
    if (iwa) {
        SHR_IF_ERR_EXIT
            (bcmltm_db_dm_track_to_pt_copy_get(unit, sid,
                                               &track_t2p_list));
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_alloc_wbc_to_wbc,
                                           (void *)track_t2p_list,
                                           &track_t2p_nd));
    }
    track_t2p_node = bcmltm_tree_node_create((void *)track_t2p_nd,
                                             &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(track_t2p_node, SHR_E_MEMORY);
    fa_node_roots[0] = track_t2p_node;

    /*
     * [Track_Index]: Calculate tracking index from tracking elements
     *                (No-TI-Map).
     */
    if (!track_index_map) {
        SHR_IF_ERR_EXIT
            (bcmltm_db_dm_track_index_get(unit, sid, &track_info));
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_wbc_to_track,
                                           (void *)track_info,
                                           &track_index_nd));
    }
    track_index_node = bcmltm_tree_left_node_create(track_t2p_node,
                                                    (void *)track_index_nd,
                                                    &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(track_index_node, SHR_E_MEMORY);

    /* [Key_Fwd_Xfrm]: Process LTA key forward transforms */
    if (iwa) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_xfrm_fa_trees_fwd_alloc_create(unit, sid,
                                                      BCMLT_OPCODE_INSERT,
                                                      track_index_node,
                                                      op_md_ptr,
                                                      &key_fwd_xfrm_node));
    } else {
        xfrm_type = BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM;
        SHR_IF_ERR_EXIT
            (bcmltm_md_xfrm_fa_trees_fwd_create(unit, sid,
                                                xfrm_type, xfrm_lta_fopt,
                                                BCMLT_OPCODE_INSERT,
                                                track_index_node,
                                                op_md_ptr,
                                                &key_fwd_xfrm_node));
    }

    /* [Key_FV]: Process LTA key field validations */
    field_type = BCMLTM_FIELD_TYPE_KEY;
    SHR_IF_ERR_EXIT
        (bcmltm_md_validate_fa_trees_create(unit, sid, field_type,
                                            BCMLT_OPCODE_INSERT,
                                            key_fwd_xfrm_node,
                                            op_md_ptr,
                                            &key_fv_node));

    /* [Key_Req_A2P]: Convert required key fields from API to physical */
    if (iwa) {
        SHR_IF_ERR_EXIT
            (bcmltm_db_dm_req_key_field_mapping_get(unit, sid, &key_req_map));
    } else {
        SHR_IF_ERR_EXIT
            (bcmltm_db_dm_field_mapping_get(unit, sid,
                                            BCMLTM_FIELD_TYPE_KEY,
                                            BCMLTM_FIELD_DIR_IN,
                                            &key_req_map));
    }
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_key_fields_to_wb,
                                       (void *)key_req_map,
                                       &key_req_a2p_nd));
    key_req_a2p_node = bcmltm_tree_right_node_create(track_index_node,
                                                     (void *)key_req_a2p_nd,
                                                     &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(key_req_a2p_node, SHR_E_MEMORY);

    /* [Key_Alloc_A2P]: Convert optional key fields from API to physical(IwA) */
    if (iwa) {
        SHR_IF_ERR_EXIT
            (bcmltm_db_dm_alloc_key_field_mapping_get(unit, sid,
                                                      &key_alloc_map));
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                    bcmltm_fa_node_api_alloc_key_fields_to_wb,
                                    (void *)key_alloc_map,
                                    &key_alloc_a2p_nd));
    }
    key_alloc_a2p_node =
        bcmltm_tree_right_node_create(key_req_a2p_node,
                                      (void *)key_alloc_a2p_nd,
                                      &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(key_alloc_a2p_node, SHR_E_MEMORY);

    /* [Fixed_Key]: Set Fixed key fields */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_fixed_keys_get(unit, sid, &fixed_keys));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_fixed_fields_to_wb,
                                       (void *)fixed_keys,
                                       &fixed_key_nd));
    fixed_key_node = bcmltm_tree_left_node_create(key_req_a2p_node,
                                                  (void *)fixed_key_nd,
                                                  &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(fixed_key_node, SHR_E_MEMORY);

    /*
     * [Track_Dec]: Decode tracking index to tracking elements
     *              (IwA:No-TI-Map)
     */
    if ((iwa) && (!track_index_map)) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_alloc_track_to_wbc,
                                           (void *)track_info,
                                           &track_dec_nd));
    }
    track_dec_node = bcmltm_tree_right_node_create(track_t2p_node,
                                                   (void *)track_dec_nd,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(track_dec_node, SHR_E_MEMORY);

    /* [Track_Alloc]: Allocate tracking index (IwA) */
    if (iwa) {
        if (shared) {
            node_func = bcmltm_fa_node_global_alloc_track;
        } else {
            node_func = bcmltm_fa_node_alloc_track;
        }
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           node_func,
                                           (void *)track_info,
                                           &track_alloc_nd));
    }
    track_alloc_node = bcmltm_tree_right_node_create(track_dec_node,
                                                     (void *)track_alloc_nd,
                                                     &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(track_alloc_node, SHR_E_MEMORY);

    /*-----------------
     * Tree_1
     */

    /* [Value_A2P]: Convert value fields from API to physical */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_field_mapping_get(unit, sid,
                                        BCMLTM_FIELD_TYPE_VALUE,
                                        BCMLTM_FIELD_DIR_IN,
                                        &value_map));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_value_fields_to_wb,
                                       (void *)value_map,
                                       &value_a2p_nd));
    value_a2p_node = bcmltm_tree_node_create((void *)value_a2p_nd,
                                             &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(value_a2p_node, SHR_E_MEMORY);
    fa_node_roots[1] = value_a2p_node;

    /* [Value_Fwd_Xfrm]: Process LTA value forward transforms */
    xfrm_type = BCMLTM_DB_XFRM_TYPE_FWD_VALUE_TRANSFORM;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_fwd_create(unit, sid,
                                            xfrm_type, xfrm_lta_fopt,
                                            BCMLT_OPCODE_INSERT,
                                            value_a2p_node,
                                            op_md_ptr,
                                            &value_fwd_xfrm_node));

    /* [Value_FV]: Process LTA value field validations */
    field_type = BCMLTM_FIELD_TYPE_VALUE;
    SHR_IF_ERR_EXIT
        (bcmltm_md_validate_fa_trees_create(unit, sid, field_type,
                                            BCMLT_OPCODE_INSERT,
                                            value_fwd_xfrm_node,
                                            op_md_ptr,
                                            &value_fv_node));

    /* [Value_Def]: Set values to SW defaults from LRD */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_default_fields_to_wb,
                                       (void *)value_map,
                                       &value_def_nd));
    value_def_node = bcmltm_tree_left_node_create(value_fv_node,
                                                  (void *)value_def_nd,
                                                  &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(value_def_node, SHR_E_MEMORY);

    /* [RO_Check]: Verify no read-only fields present in input */
    SHR_IF_ERR_EXIT
        (bcmltm_db_read_only_list_get(unit, sid, &ro_list));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_read_only_input_check,
                                       (void *)ro_list,
                                       &ro_check_nd));
    ro_check_node = bcmltm_tree_left_node_create(value_def_node,
                                                 (void *)ro_check_nd,
                                                 &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(ro_check_node, SHR_E_MEMORY);

    /*
     * [Wide_Value]: Verify all or none of wide value field elements
     * present in input
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_wide_field_list_get(unit, sid, &wide_list));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_wide_field_check,
                                       (void *)wide_list,
                                       &wide_field_nd));
    wide_field_node = bcmltm_tree_right_node_create(value_def_node,
                                                 (void *)wide_field_nd,
                                                 &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(wide_field_node, SHR_E_MEMORY);

    /* [Fixed_Value]: Set Fixed value fields */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_fixed_values_get(unit, sid, &fixed_values));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_fixed_fields_to_wb,
                                       (void *)fixed_values,
                                       &fixed_value_nd));
    fixed_value_node = bcmltm_tree_right_node_create(value_a2p_node,
                                                     (void *)fixed_value_nd,
                                                     &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(fixed_value_node, SHR_E_MEMORY);

    /*
     * [Key_Rev_Xfrm]: Process LTA key reverse transforms to return
     *                 allocated optional keys (IwA).
     */
    if (iwa) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_xfrm_fa_trees_rev_alloc_create(unit, sid,
                                                      BCMLT_OPCODE_INSERT,
                                                      fixed_value_node,
                                                      op_md_ptr,
                                                      &key_rev_xfrm_node));
    }

    /*
     * [Key_Alloc_P2A]: Convert optional key fields from physical to API
     *                  to return allocated optional keys (IwA).
     */
    if (iwa) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                    bcmltm_fa_node_wb_to_api_alloc_fields,
                                           (void *)key_alloc_map,
                                           &key_alloc_p2a_nd));
    }
    key_alloc_p2a_node =
        bcmltm_tree_right_node_create(fixed_value_node,
                                      (void *)key_alloc_p2a_nd,
                                      &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(key_alloc_p2a_node, SHR_E_MEMORY);

    /* Set returning pointer */
    op_md_ptr->fa_node_roots = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy node data if it it is not in node tree */
        if (track_t2p_node == NULL) {
            SHR_FREE(track_t2p_nd);
        }
        if (track_index_node == NULL) {
            SHR_FREE(track_index_nd);
        }
        if (key_req_a2p_node == NULL) {
            SHR_FREE(key_req_a2p_nd);
        }
        if (key_alloc_a2p_node == NULL) {
            SHR_FREE(key_alloc_a2p_nd);
        }
        if (fixed_key_node == NULL) {
            SHR_FREE(fixed_key_nd);
        }
        if (track_dec_node == NULL) {
            SHR_FREE(track_dec_nd);
        }
        if (track_alloc_node == NULL) {
            SHR_FREE(track_alloc_nd);
        }
        if (value_a2p_node == NULL) {
            SHR_FREE(value_a2p_nd);
        }
        if (value_def_node == NULL) {
            SHR_FREE(value_def_nd);
        }
        if (fixed_value_node == NULL) {
            SHR_FREE(fixed_value_nd);
        }
        if (ro_check_node == NULL) {
            SHR_FREE(ro_check_nd);
        }
        if (wide_field_node == NULL) {
            SHR_FREE(wide_field_nd);
       }
        if (key_alloc_p2a_node == NULL) {
            SHR_FREE(key_alloc_p2a_nd);
        }

        /* Destroy trees */
        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, fa_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->fa_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create EE tree roots for INSERT on Index LT.
 *
 * This function creates the EE tree roots for a INSERT operation
 * on given Index logical table.
 *
 * This routine implements the Disjoint LT design.  The resulting
 * tree can be used for LTs with full or split-entry mapped fields.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_ee_node_roots_insert_create(int unit,
                                     bcmlrd_sid_t sid,
                                     const bcmltm_table_attr_t *attr,
                                     bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bool shared = FALSE;
    bool interactive = FALSE;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *read_node = NULL;
    bcmltm_node_t *index_lk_node = NULL;
    bcmltm_node_t *write_node = NULL;
    bcmltm_node_t *index_ins_node = NULL;
    bcmltm_node_t *gindex_ins_node = NULL;
    bcmltm_ee_node_t *read_nd = NULL;
    bcmltm_ee_node_t *index_lk_nd = NULL;
    bcmltm_ee_node_t *write_nd = NULL;
    bcmltm_ee_node_t *index_ins_nd = NULL;
    bcmltm_ee_node_t *gindex_ins_nd = NULL;
    bcmltm_ee_node_f read_func = NULL;
    bcmltm_ee_node_f write_func = NULL;
    const bcmltm_pt_list_t *pt_list = NULL;
    const bcmltm_ee_index_info_t *index_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get Index table properties */
    if (BCMLTM_TABLE_ATTR_IS_SHARED(*attr)) {
        shared = TRUE;
    }
    if (BCMLTM_TABLE_ATTR_MODE_IS_INTERACTIVE(*attr)) {
        interactive = TRUE;
    }

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, size, "bcmltmIndexInsertEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /*
     * Get EE node functions for corresponding PTM interface
     * based on table operating mode.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_ptm_index_func_get(interactive,
                                              &read_func,
                                              &write_func));

    /*
     * INSERT - Disjoint
     *
     * Unless specified, the node applies to all Index LT types:
     * Simple Index and Index With Allocation, both as Shared and no-Shared.
     *
     * EE Trees Roots
     * -----------------------------------------------------------------
     * Tree_0:
     *                         [Read]
     *                            |
     *                 +----------+----------+
     *                 |                     |
     *           [Index_Lk]                  =
     *
     * Execution:
     * [Index_Lk]: bcmltm_ee_node_model_index_free
     * [Read]: <bcmltm_ee_node_schan_read> (Interactive)
     *         <bcmltm_ee_node_model_index_read> (Modeled)
     *
     * -----------------------------------------------------------------
     * Tree_1:
     *                         [Write]
     *                            |
     *                 +----------+----------+
     *                 |                     |
     *           [Index_Ins]                 =
     *                 |
     *          +------+------+
     *          |             |
     *     [GIndex_Ins]       =
     *
     * Execution:
     * [GIndex_Ins]: bcmltm_ee_node_global_index_insert (Shared)
     * [Index_Ins] : bcmltm_ee_node_model_index_insert
     * [Write]     : <bcmltm_ee_node_schan_write> (Interactive)
     *               <bcmltm_ee_node_model_index_write> (Modeled)
     *
     * -----------------------------------------------------------------
     * NOTE:
     * The LT index insert operation is placed in Tree_1 (rather than Tree_0)
     * in order to allow Interactive LTs to use this flow.
     * Commit/abort are not applicable on Interactive LTs.  As such,
     * processing of all value fields must be done prior to the index
     * allocation because there is no rollback mechanism to undo the
     * index allocation.
     */

    /*-----------------
     * Tree_0
     */

    /* [Read]: Execute PTM read operation */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_pt_list_get(unit, sid, &pt_list));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       read_func,
                                       (void *)pt_list,
                                       &read_nd));
    read_node = bcmltm_tree_node_create((void *)read_nd,
                                        &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(read_node, SHR_E_MEMORY);
    ee_node_roots[0] = read_node;

    /* [Index_Lk]: LT index lookup resource operation */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_ee_lt_index_get(unit, sid, &index_info));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_model_index_free,
                                       (void *)index_info,
                                       &index_lk_nd));
    index_lk_node = bcmltm_tree_left_node_create(read_node,
                                                 (void *)index_lk_nd,
                                                 &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(index_lk_node, SHR_E_MEMORY);

    /*-----------------
     * Tree_1
     */

    /* [Write]: Execute PTM write operation */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       write_func,
                                       (void *)pt_list,
                                       &write_nd));
    write_node = bcmltm_tree_node_create((void *)write_nd,
                                         &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(write_node, SHR_E_MEMORY);
    ee_node_roots[1] = write_node;

    /* [Index]: LT index insert resource operation */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_model_index_insert,
                                       (void *)index_info,
                                       &index_ins_nd));
    index_ins_node = bcmltm_tree_left_node_create(write_node,
                                                  (void *)index_ins_nd,
                                                  &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(index_ins_node, SHR_E_MEMORY);

    /* [GIndex_Ins]: LT global index insert operation for shared resources */
    if (shared) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_ee_node_data_create(unit,
                                           bcmltm_ee_node_global_index_insert,
                                           (void *)index_info,
                                           &gindex_ins_nd));
    }
    gindex_ins_node = bcmltm_tree_left_node_create(index_ins_node,
                                                   (void *)gindex_ins_nd,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(gindex_ins_node, SHR_E_MEMORY);

    /* Set returning pointer */
    op_md_ptr->ee_node_roots = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy node data if it it is not in node tree */
        if (read_node == NULL) {
            SHR_FREE(read_nd);
        }
        if (write_node == NULL) {
            SHR_FREE(write_nd);
        }
        if (index_ins_node == NULL) {
            SHR_FREE(index_ins_nd);
        }
        if (gindex_ins_node == NULL) {
            SHR_FREE(gindex_ins_nd);
        }

        /* Destroy trees */
        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, ee_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->ee_node_roots = NULL;
    }

    SHR_FUNC_EXIT();

}

/*!
 * \brief Create INSERT operation metadata for Index LT.
 *
 * This routine creates the operation metadata for given table ID
 * to use for the INSERT opcode on Index LTs.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_op_insert_create(int unit, bcmlrd_sid_t sid,
                          bcmltm_lt_op_md_t **op_md_ptr)
{
    const bcmltm_table_attr_t *table_attr;
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get table attributes */
    SHR_IF_ERR_EXIT
        (bcmltm_db_table_attr_get(unit, sid, &table_attr));

    /* Get working buffer size */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "bcmltmIndexInsertLtOpMd");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_INSERT_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lt_index_fa_node_roots_insert_create(unit, sid, table_attr,
                                              op_md));
    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_index_ee_node_roots_insert_create(unit, sid, table_attr,
                                              op_md));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(op_md);
        *op_md_ptr = NULL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create FA tree roots for LOOKUP on Index LT.
 *
 * This function creates the FA tree roots for a LOOKUP operation
 * on given Index logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_fa_node_roots_lookup_create(int unit,
                                     bcmlrd_sid_t sid,
                                     const bcmltm_table_attr_t *attr,
                                     bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bool track_index_map = FALSE;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *track_index_node = NULL;
    bcmltm_node_t *key_fwd_xfrm_node = NULL;
    bcmltm_node_t *key_fv_node = NULL;
    bcmltm_node_t *key_a2p_node = NULL;
    bcmltm_node_t *fixed_key_node = NULL;
    bcmltm_node_t *value_p2a_node = NULL;
    bcmltm_node_t *value_rev_xfrm_node = NULL;
    bcmltm_fa_node_t *track_index_nd = NULL;
    bcmltm_fa_node_t *key_a2p_nd = NULL;
    bcmltm_fa_node_t *fixed_key_nd = NULL;
    bcmltm_fa_node_t *value_p2a_nd = NULL;
    const bcmltm_track_index_t *track_info = NULL;
    const bcmltm_field_select_mapping_t *key_map = NULL;
    const bcmltm_field_select_mapping_t *value_map = NULL;
    const bcmltm_fixed_field_list_t *fixed_keys = NULL;
    int xfrm_type;
    bcmltm_md_lta_fopt_t xfrm_lta_fopt = 0x0;
    bcmltm_field_type_t field_type;

    SHR_FUNC_ENTER(unit);

    /* Track index: derived from map or LTM */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_track_index_is_mapped(unit, sid, &track_index_map));

    /* Allocate pointers to FA tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, size, "bcmltmIndexLookupFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, size);

    /*
     * LOOKUP
     *
     * FA Tree Roots
     * -----------------------------------------------------------------
     * Tree_0:
     *                          [Track_Index]
     *                               |
     *                     +---------+---------+
     *                     |                   |
     *               [Key_Fwd_Xfrm]        [Key_A2P]
     *                     |                   |
     *               +-----+-----+       +-----+-----+
     *               |           |       |           |
     *           [Key_FV]        =  [Fixed_Key]      =
     *
     * Execution:
     * [Key_FV]      : bcmltm_md_validate_fa_trees_create
     * [Key_Fwd_Xfrm]: bcmltm_md_xfrm_fa_trees_fwd_create
     * [Fixed_Key]   : bcmltm_fa_node_fixed_fields_to_wb
     * [Key_A2P]     : bcmltm_fa_node_api_key_fields_to_wb
     * [Track_Index] : bcmltm_fa_node_wbc_to_track (No-TI-Map)
     *
     * -----------------------------------------------------------------
     * Tree_1:
     *
     *                          [Value_P2A]
     *                               |
     *                     +---------+---------+
     *                     |                   |
     *               [Value_Rev_Xfrm]          =
     *
     * Execution:
     * [Value_Rev_Xfrm]: bcmltm_md_xfrm_fa_trees_rev_create
     * [Value_P2A]     : bcmltm_fa_node_wb_to_api_fields
     *
     * -----------------------------------------------------------------
     * For more information on Field Transform <Xfrm> and
     * Field Validation <FV> clusters, please see description
     * at the top of this file.
     *
     * Note: No-TI-Map track index is supplied by LTM.
     *       TI-Map    track index is supplied by map field transform.
 */

    /*-----------------
     * Tree_0
     */

    /*
     * [Track_Index]: Calculate tracking index from tracking elements
     *                (No-TI-Map).
     */
    if (!track_index_map) {
        SHR_IF_ERR_EXIT
            (bcmltm_db_dm_track_index_get(unit, sid, &track_info));
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_wbc_to_track,
                                           (void *)track_info,
                                           &track_index_nd));
    }
    track_index_node = bcmltm_tree_node_create((void *)track_index_nd,
                                               &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(track_index_node, SHR_E_MEMORY);
    fa_node_roots[0] = track_index_node;

    /* [Key_Fwd_Xfrm]: Process LTA key forward transforms */
    xfrm_type = BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_fwd_create(unit, sid,
                                            xfrm_type, xfrm_lta_fopt,
                                            BCMLT_OPCODE_LOOKUP,
                                            track_index_node,
                                            op_md_ptr,
                                            &key_fwd_xfrm_node));

    /* [Key_FV]: Process LTA key field validations */
    field_type = BCMLTM_FIELD_TYPE_KEY;
    SHR_IF_ERR_EXIT
        (bcmltm_md_validate_fa_trees_create(unit, sid, field_type,
                                            BCMLT_OPCODE_LOOKUP,
                                            key_fwd_xfrm_node,
                                            op_md_ptr,
                                            &key_fv_node));

    /* [Key_A2P]: Convert key fields from API to physical */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_field_mapping_get(unit, sid,
                                        BCMLTM_FIELD_TYPE_KEY,
                                        BCMLTM_FIELD_DIR_IN,
                                        &key_map));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_key_fields_to_wb,
                                       (void *)key_map,
                                       &key_a2p_nd));
    key_a2p_node = bcmltm_tree_right_node_create(track_index_node,
                                                 (void *)key_a2p_nd,
                                                 &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(key_a2p_node, SHR_E_MEMORY);

    /* [Fixed_Key]: Set Fixed key fields */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_fixed_keys_get(unit, sid, &fixed_keys));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_fixed_fields_to_wb,
                                       (void *)fixed_keys,
                                       &fixed_key_nd));
    fixed_key_node = bcmltm_tree_left_node_create(key_a2p_node,
                                                  (void *)fixed_key_nd,
                                                  &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(fixed_key_node, SHR_E_MEMORY);

    /*-----------------
     * Tree_1
     */

    /* [Value_P2A]: Convert value fields from physical to API */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_field_mapping_get(unit, sid,
                                        BCMLTM_FIELD_TYPE_VALUE,
                                        BCMLTM_FIELD_DIR_OUT,
                                        &value_map));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_wb_to_api_fields,
                                       (void *)value_map,
                                       &value_p2a_nd));
    value_p2a_node = bcmltm_tree_node_create((void *)value_p2a_nd,
                                             &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(value_p2a_node, SHR_E_MEMORY);
    fa_node_roots[1] = value_p2a_node;


    /* [Value_Rev_Xfrm]: Process LTA value reverse transforms */
    xfrm_type = BCMLTM_DB_XFRM_TYPE_REV_VALUE_TRANSFORM;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_rev_create(unit, sid,
                                            xfrm_type,
                                            BCMLT_OPCODE_LOOKUP,
                                            value_p2a_node,
                                            op_md_ptr,
                                            &value_rev_xfrm_node));

    /* Set returning pointer */
    op_md_ptr->fa_node_roots = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy node data if it it is not in node tree */
        if (track_index_node == NULL) {
            SHR_FREE(track_index_nd);
        }
        if (key_a2p_node == NULL) {
            SHR_FREE(key_a2p_nd);
        }
        if (fixed_key_node == NULL) {
            SHR_FREE(fixed_key_nd);
        }
        if (value_p2a_node == NULL) {
            SHR_FREE(value_p2a_nd);
        }

        /* Destroy trees */
        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, fa_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->fa_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create EE tree roots for LOOKUP on Index LT.
 *
 * This function creates the EE tree roots for a LOOKUP operation
 * on given Index logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_ee_node_roots_lookup_create(int unit,
                                     bcmlrd_sid_t sid,
                                     const bcmltm_table_attr_t *attr,
                                     bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bool interactive = FALSE;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *read_node = NULL;
    bcmltm_node_t *index_lk_node = NULL;
    bcmltm_ee_node_t *read_nd = NULL;
    bcmltm_ee_node_t *index_lk_nd = NULL;
    const bcmltm_pt_list_t *pt_list = NULL;
    const bcmltm_ee_index_info_t *index_info = NULL;
    bcmltm_ee_node_f read_func = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get Index table properties */
    if (BCMLTM_TABLE_ATTR_MODE_IS_INTERACTIVE(*attr)) {
        interactive = TRUE;
    }

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, size, "bcmltmIndexLookupEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /*
     * Get EE node functions for corresponding PTM interface
     * based on table operating mode.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_ptm_index_func_get(interactive,
                                              &read_func,
                                              NULL));

    /*
     * LOOKUP
     *
     * EE Trees Roots
     * -----------------------------------------------------------------
     * Tree_0:
     *                         [Read]
     *                            |
     *                 +----------+----------+
     *                 |                     |
     *           [Index_Lk]                  =
     *
     * Execution:
     * [Index_Lk]: bcmltm_ee_node_model_index_exists
     * [Read]    : <bcmltm_ee_node_schan_read> (Interactive)
     *             <bcmltm_ee_node_model_index_read> (Modeled)
     *
     * -----------------------------------------------------------------
     * Tree_1:                 [NULL]
     */

    /*-----------------
     * Tree_0
     */

    /* [Read]: Execute PTM read operation */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_pt_list_get(unit, sid, &pt_list));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       read_func,
                                       (void *)pt_list,
                                       &read_nd));
    read_node = bcmltm_tree_node_create((void *)read_nd,
                                        &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(read_node, SHR_E_MEMORY);
    ee_node_roots[0] = read_node;

    /* [Index]: LT index lookup resource operation */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_ee_lt_index_get(unit, sid, &index_info));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_model_index_exists,
                                       (void *)index_info,
                                       &index_lk_nd));
    index_lk_node = bcmltm_tree_left_node_create(read_node,
                                                 (void *)index_lk_nd,
                                                 &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(index_lk_node, SHR_E_MEMORY);

    /*-----------------
     * Tree_1
     */

    /* [NULL]: No actions */
    ee_node_roots[1] = NULL;  /* Not used */

    /* Set returning pointer */
    op_md_ptr->ee_node_roots = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy node data if it it is not in node tree */
        if (read_node == NULL) {
            SHR_FREE(read_nd);
        }
        if (index_lk_node == NULL) {
            SHR_FREE(index_lk_nd);
        }

        /* Destroy trees */
        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, ee_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->ee_node_roots = NULL;
    }

    SHR_FUNC_EXIT();

}

/*!
 * \brief Create LOOKUP operation metadata for Index LT.
 *
 * This routine creates the operation metadata for given table ID
 * to use for the LOOKUP opcode on Index LTs.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_op_lookup_create(int unit, bcmlrd_sid_t sid,
                          bcmltm_lt_op_md_t **op_md_ptr)
{
    const bcmltm_table_attr_t *table_attr;
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get table attributes */
    SHR_IF_ERR_EXIT
        (bcmltm_db_table_attr_get(unit, sid, &table_attr));

    /* Get working buffer size */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "bcmltmIndexLookupLtOpMd");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_LOOKUP_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lt_index_fa_node_roots_lookup_create(unit, sid, table_attr,
                                              op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_index_ee_node_roots_lookup_create(unit, sid, table_attr,
                                              op_md));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(op_md);
        *op_md_ptr = NULL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create FA tree roots for DELETE on Index LT.
 *
 * This function creates the FA tree roots for a DELETE operation
 * on given Index logical table.
 *
 * The field values are reset to the SW defaults as specified in the LRD map.
 *
 * This routine implements the Disjoint LT design.  The resulting
 * tree can be used for LTs with full or split-entry mapped fields.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_fa_node_roots_delete_create(int unit,
                                     bcmlrd_sid_t sid,
                                     const bcmltm_table_attr_t *attr,
                                     bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bool track_index_map = FALSE;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *track_index_node = NULL;
    bcmltm_node_t *key_fwd_xfrm_node = NULL;
    bcmltm_node_t *key_fv_node = NULL;
    bcmltm_node_t *key_a2p_node = NULL;
    bcmltm_node_t *fixed_key_node = NULL;
    bcmltm_node_t *root_node = NULL;
    bcmltm_node_t *value_def_xfrm_node = NULL;
    bcmltm_node_t *value_def_node = NULL;
    bcmltm_node_t *fixed_value_def_node = NULL;
    bcmltm_fa_node_t *track_index_nd = NULL;
    bcmltm_fa_node_t *key_a2p_nd = NULL;
    bcmltm_fa_node_t *fixed_key_nd = NULL;
    bcmltm_fa_node_t *value_def_nd = NULL;
    bcmltm_fa_node_t *fixed_value_def_nd = NULL;
    const bcmltm_track_index_t *track_info = NULL;
    const bcmltm_field_select_mapping_t *key_map = NULL;
    const bcmltm_field_select_mapping_t *value_map = NULL;
    const bcmltm_fixed_field_list_t *fixed_keys = NULL;
    const bcmltm_fixed_field_list_t *fixed_values = NULL;
    int xfrm_type;
    bcmltm_md_lta_fopt_t xfrm_lta_fopt = 0x0;
    bcmltm_field_type_t field_type;

    SHR_FUNC_ENTER(unit);

    /* Track index: derived from map or LTM */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_track_index_is_mapped(unit, sid, &track_index_map));

    /* Allocate pointers to FA tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, size, "bcmltmIndexDeleteFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, size);

    /*
     * DELETE - Disjoint
     *
     * FA Tree Roots
     * -----------------------------------------------------------------
     * Tree_0:
     *                          [Track_Index]
     *                               |
     *                     +---------+---------+
     *                     |                   |
     *               [Key_Fwd_Xfrm]        [Key_A2P]
     *                     |                   |
     *               +-----+-----+       +-----+-----+
     *               |           |       |           |
     *           [Key_FV]        =  [Fixed_Key]      =
     *
     * Execution:
     * [Key_FV]      : bcmltm_md_validate_fa_trees_create
     * [Key_Fwd_Xfrm]: bcmltm_md_xfrm_fa_trees_fwd_create
     * [Fixed_Key]   : bcmltm_fa_node_fixed_fields_to_wb
     * [Key_A2P]     : bcmltm_fa_node_api_key_fields_to_wb
     * [Track_Index] : bcmltm_fa_node_wbc_to_track (No-TI-Map)
     *
     * -----------------------------------------------------------------
     * Tree_1:
     *                                 [Root]
     *                                    |
     *                         +----------+---------+
     *                         |                    |
     *                  [Value_Def_Xfrm]       [Fixed_Value_Def]
     *                         |
     *                +--------+--------+
     *                |                 |
     *           [Value_Def]            =
     *
     * Execution:
     * [Value_Def]      : bcmltm_fa_node_default_fields_to_wb
     * [Value_Def_Xfrm] : bcmltm_md_xfrm_fa_trees_fwd_create
     * [Fixed_Value_Def]: bcmltm_fa_node_delete_fixed_fields_to_wb
     *
     * -----------------------------------------------------------------
     * For more information on Field Transform <Xfrm> and
     * Field Validation <FV> clusters, please see description
     * at the top of this file.
     *
     * Note: No-TI-Map track index is supplied by LTM.
     *       TI-Map    track index is supplied by map field transform.
     */

    /*-----------------
     * Tree_0
     */

    /*
     * [Track_Index]: Calculate tracking index from tracking elements
     *                (No-TI-Map).
     */
    if (!track_index_map) {
        SHR_IF_ERR_EXIT
            (bcmltm_db_dm_track_index_get(unit, sid, &track_info));
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_wbc_to_track,
                                           (void *)track_info,
                                           &track_index_nd));
    }
    track_index_node = bcmltm_tree_node_create((void *)track_index_nd,
                                               &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(track_index_node, SHR_E_MEMORY);
    fa_node_roots[0] = track_index_node;

    /* [Key_Fwd_Xfrm]: Process LTA key forward transforms */
    xfrm_type = BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_fwd_create(unit, sid,
                                            xfrm_type, xfrm_lta_fopt,
                                            BCMLT_OPCODE_DELETE,
                                            track_index_node,
                                            op_md_ptr,
                                            &key_fwd_xfrm_node));

    /* [Key_FV]: Process LTA key field validations */
    field_type = BCMLTM_FIELD_TYPE_KEY;
    SHR_IF_ERR_EXIT
        (bcmltm_md_validate_fa_trees_create(unit, sid, field_type,
                                            BCMLT_OPCODE_DELETE,
                                            key_fwd_xfrm_node,
                                            op_md_ptr,
                                            &key_fv_node));

    /* [Key_A2P]: Convert key fields from API to physical */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_field_mapping_get(unit, sid,
                                        BCMLTM_FIELD_TYPE_KEY,
                                        BCMLTM_FIELD_DIR_IN,
                                        &key_map));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_key_fields_to_wb,
                                       (void *)key_map,
                                       &key_a2p_nd));
    key_a2p_node = bcmltm_tree_right_node_create(track_index_node,
                                                 (void *)key_a2p_nd,
                                                 &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(key_a2p_node, SHR_E_MEMORY);

    /* [Fixed_Key]: Set Fixed key fields */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_fixed_keys_get(unit, sid, &fixed_keys));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_fixed_fields_to_wb,
                                       (void *)fixed_keys,
                                       &fixed_key_nd));
    fixed_key_node = bcmltm_tree_left_node_create(key_a2p_node,
                                                  (void *)fixed_key_nd,
                                                  &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(fixed_key_node, SHR_E_MEMORY);

    /*-----------------
     * Tree_1
     */

    /* [Root]: Nothing to do (used for chaining nodes) */
    root_node = bcmltm_tree_node_create((void *)NULL,
                                        &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(root_node, SHR_E_MEMORY);
    fa_node_roots[1] = root_node;

    /* [Value_Def_Xfrm]: Process LTA value forward transforms with defaults */
    xfrm_type = BCMLTM_DB_XFRM_TYPE_FWD_VALUE_TRANSFORM;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_fwd_create(unit, sid,
                                            xfrm_type, xfrm_lta_fopt,
                                            BCMLT_OPCODE_DELETE,
                                            root_node,
                                            op_md_ptr,
                                            &value_def_xfrm_node));

    /* [Value_Def]: Set table values to SW defaults from LRD */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_field_mapping_get(unit, sid,
                                        BCMLTM_FIELD_TYPE_VALUE,
                                        BCMLTM_FIELD_DIR_IN,
                                        &value_map));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_default_fields_to_wb,
                                       (void *)value_map,
                                       &value_def_nd));
    value_def_node = bcmltm_tree_left_node_create(value_def_xfrm_node,
                                                  (void *)value_def_nd,
                                                  &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(value_def_node, SHR_E_MEMORY);


    /* [Fixed_Value_Def]: Set fixed value fields to default from LRD */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_fixed_values_get(unit, sid, &fixed_values));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_delete_fixed_fields_to_wb,
                                       (void *)fixed_values,
                                       &fixed_value_def_nd));
    fixed_value_def_node =
        bcmltm_tree_right_node_create(root_node,
                                      (void *)fixed_value_def_nd,
                                      &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(fixed_value_def_node, SHR_E_MEMORY);

    /* Set returning pointer */
    op_md_ptr->fa_node_roots = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy node data if it it is not in node tree */
        if (track_index_node == NULL) {
            SHR_FREE(track_index_nd);
        }
        if (key_a2p_node == NULL) {
            SHR_FREE(key_a2p_nd);
        }
        if (fixed_key_node == NULL) {
            SHR_FREE(fixed_key_nd);
        }
        if (value_def_node == NULL) {
            SHR_FREE(value_def_nd);
        }
        if (fixed_value_def_node == NULL) {
            SHR_FREE(fixed_value_def_nd);
        }

        /* Destroy trees */
        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, fa_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->fa_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create EE tree roots for DELETE on Index LT.
 *
 * This function creates the EE tree roots for a DELETE operation
 * on given Index logical table.
 *
 * This routine implements the Disjoint LT design.  The resulting
 * tree can be used for LTs with full or split-entry mapped fields.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_ee_node_roots_delete_create(int unit,
                                     bcmlrd_sid_t sid,
                                     const bcmltm_table_attr_t *attr,
                                     bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bool shared = FALSE;
    bool interactive = FALSE;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *read_node = NULL;
    bcmltm_node_t *index_lk_node = NULL;
    bcmltm_node_t *write_node = NULL;
    bcmltm_node_t *index_del_node = NULL;
    bcmltm_node_t *gindex_del_node = NULL;
    bcmltm_ee_node_t *read_nd = NULL;
    bcmltm_ee_node_t *index_lk_nd = NULL;
    bcmltm_ee_node_t *write_nd = NULL;
    bcmltm_ee_node_t *index_del_nd = NULL;
    bcmltm_ee_node_t *gindex_del_nd = NULL;
    bcmltm_ee_node_f read_func = NULL;
    bcmltm_ee_node_f write_func = NULL;
    const bcmltm_pt_list_t *pt_list = NULL;
    const bcmltm_ee_index_info_t *index_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get Index table properties */
    if (BCMLTM_TABLE_ATTR_IS_SHARED(*attr)) {
        shared = TRUE;
    }
    if (BCMLTM_TABLE_ATTR_MODE_IS_INTERACTIVE(*attr)) {
        interactive = TRUE;
    }

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, size, "bcmltmIndexDeleteEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /*
     * Get EE node functions for corresponding PTM interface
     * based on table operating mode.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_ptm_index_func_get(interactive,
                                              &read_func,
                                              &write_func));

    /*
     * DELETE - Disjoint
     *
     * Unless specified, the node applies to all Index LT types:
     * Simple Index and Index With Allocation, both as Shared and no-Shared.
     *
     * EE Tree Roots
     * -----------------------------------------------------------------
     * Tree_0:
     *                         [Read]
     *                            |
     *                 +----------+----------+
     *                 |                     |
     *           [Index_Lk]                  =
     *
     * Execution:
     * [Index_Lk]: bcmltm_ee_node_model_index_exists
     * [Read]    : <bcmltm_ee_node_schan_read> (Interactive)
     *             <bcmltm_ee_node_model_index_read> (Modeled)
     *
     * -----------------------------------------------------------------
     * Tree_0:
     *                         [Write]
     *                            |
     *                 +----------+----------+
     *                 |                     |
     *           [Index_Del]            [GIndex_Del]
     *
     * Execution:
     * [Index_Del] : bcmltm_ee_node_model_index_delete
     * [GIndex_Del]: bcmltm_ee_node_global_index_delete (IwA)
     * [Write]     : <bcmltm_ee_node_schan_write> (Interactive)
     *               <bcmltm_ee_node_model_index_write> (Modeled)
     */

    /*-----------------
     * Tree_0
     */

    /* [Read]: Execute PTM read operation */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_pt_list_get(unit, sid, &pt_list));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       read_func,
                                       (void *)pt_list,
                                       &read_nd));
    read_node = bcmltm_tree_node_create((void *)read_nd,
                                        &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(read_node, SHR_E_MEMORY);
    ee_node_roots[0] = read_node;

    /* [Index_Lk]: LT index lookup resource operation */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_ee_lt_index_get(unit, sid, &index_info));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_model_index_exists,
                                       (void *)index_info,
                                       &index_lk_nd));
    index_lk_node = bcmltm_tree_left_node_create(read_node,
                                                 (void *)index_lk_nd,
                                                 &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(index_lk_node, SHR_E_MEMORY);

    /*-----------------
     * Tree_1
     */

    /* [Write]: Execute PTM write operation */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       write_func,
                                       (void *)pt_list,
                                       &write_nd));
    write_node = bcmltm_tree_node_create((void *)write_nd,
                                         &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(write_node, SHR_E_MEMORY);
    ee_node_roots[1] = write_node;

    /* [Index_Del]: LT index delete resource operation */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_ee_lt_index_get(unit, sid, &index_info));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_model_index_delete,
                                       (void *)index_info,
                                       &index_del_nd));
    index_del_node = bcmltm_tree_left_node_create(write_node,
                                                  (void *)index_del_nd,
                                                  &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(index_del_node, SHR_E_MEMORY);

    /* [GIndex_Del]: bcmltm_ee_node_global_index_delete (IwA) */
    if (shared) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_ee_node_data_create(unit,
                                           bcmltm_ee_node_global_index_delete,
                                           (void *)index_info,
                                           &gindex_del_nd));
    }
    gindex_del_node = bcmltm_tree_right_node_create(write_node,
                                                    (void *)gindex_del_nd,
                                                    &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(gindex_del_node, SHR_E_MEMORY);

    /* Set returning pointer */
    op_md_ptr->ee_node_roots = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy node data if it it is not in node tree */
        if (read_node == NULL) {
            SHR_FREE(read_nd);
        }
        if (index_lk_node == NULL) {
            SHR_FREE(index_lk_nd);
        }
        if (write_node == NULL) {
            SHR_FREE(write_nd);
        }
        if (index_del_node == NULL) {
            SHR_FREE(index_del_nd);
        }
        if (gindex_del_node == NULL) {
            SHR_FREE(gindex_del_nd);
        }

        /* Destroy trees */
        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, ee_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->ee_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create DELETE operation metadata for Index LT.
 *
 * This routine creates the operation metadata for given table ID
 * to use for the DELETE opcode on Index LTs.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_op_delete_create(int unit, bcmlrd_sid_t sid,
                          bcmltm_lt_op_md_t **op_md_ptr)
{
    const bcmltm_table_attr_t *table_attr;
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get table attributes */
    SHR_IF_ERR_EXIT
        (bcmltm_db_table_attr_get(unit, sid, &table_attr));

    /* Get working buffer size */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "bcmltmIndexDeleteLtOpMd");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_DELETE_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lt_index_fa_node_roots_delete_create(unit, sid, table_attr,
                                              op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_index_ee_node_roots_delete_create(unit, sid, table_attr,
                                              op_md));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(op_md);
        *op_md_ptr = NULL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create FA tree roots for UPDATE on Index LT.
 *
 * This function creates the FA tree roots for a UPDATE operation
 * on given Index logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_fa_node_roots_update_create(int unit,
                                     bcmlrd_sid_t sid,
                                     const bcmltm_table_attr_t *attr,
                                     bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bool track_index_map = FALSE;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *track_index_node = NULL;
    bcmltm_node_t *key_fwd_xfrm_node = NULL;
    bcmltm_node_t *key_fv_node = NULL;
    bcmltm_node_t *ro_check_node = NULL;
    bcmltm_node_t *wide_field_node = NULL;
    bcmltm_node_t *key_a2p_node = NULL;
    bcmltm_node_t *fixed_key_node = NULL;
    bcmltm_node_t *value_a2p_node = NULL;
    bcmltm_node_t *value_fwd_xfrm_node = NULL;
    bcmltm_node_t *value_fv_node = NULL;
    bcmltm_node_t *value_rev_xfrm_node = NULL;
    bcmltm_fa_node_t *track_index_nd = NULL;
    bcmltm_fa_node_t *key_a2p_nd = NULL;
    bcmltm_fa_node_t *fixed_key_nd = NULL;
    bcmltm_fa_node_t *ro_check_nd = NULL;
    bcmltm_fa_node_t *wide_field_nd = NULL;
    bcmltm_fa_node_t *value_a2p_nd = NULL;
    const bcmltm_track_index_t *track_info = NULL;
    const bcmltm_field_select_mapping_t *key_map = NULL;
    const bcmltm_field_select_mapping_t *value_map = NULL;
    const bcmltm_fixed_field_list_t *fixed_keys = NULL;
    const bcmltm_ro_field_list_t *ro_list = NULL;
    const bcmltm_wide_field_list_t *wide_list = NULL;
    int xfrm_type;
    bcmltm_md_lta_fopt_t xfrm_lta_fopt = 0x0;
    bcmltm_field_type_t field_type;

    SHR_FUNC_ENTER(unit);

    /* Track index: derived from map or LTM */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_track_index_is_mapped(unit, sid, &track_index_map));

    /* Allocate pointers to FA tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, size, "bcmltmIndexUpdateFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, size);

    /*
     * UPDATE
     *
     * FA Tree Roots
     * -----------------------------------------------------------------
     * Tree_0:
     *                          [Track_Index]
     *                               |
     *                     +---------+---------+
     *                     |                   |
     *               [Key_Fwd_Xfrm]        [Key_A2P]
     *                     |                   |
     *               +-----+-----+       +-----+-----+
     *               |           |       |           |
     *           [Key_FV]        =  [Fixed_Key]      =
     *
     * Execution:
     * [Key_FV]      : bcmltm_md_validate_fa_trees_create
     * [Key_Fwd_Xfrm]: bcmltm_md_xfrm_fa_trees_fwd_create
     * [Fixed_Key]   : bcmltm_fa_node_fixed_fields_to_wb
     * [Key_A2P]     : bcmltm_fa_node_api_key_fields_to_wb
     * [Track_Index] : bcmltm_fa_node_wbc_to_track (No-TI-Map)
     *
     * -----------------------------------------------------------------
     * Tree_1:
     *
     *                          [Value_A2P]
     *                               |
     *                     +---------+---------+
     *                     |                   |
     *               [Value_Fwd_Xfrm]          =
     *                     |
     *               +-----+-----+
     *               |           |
     *           [Value_FV]      =
     *               |
     *        +------+-----+
     *        |            |
     *  [Value_Rev_Xfrm]   =
     *               |
     *         +-----+-----+
     *         |           |
     *     [RO_Check]   [Wide_Value]
     *
     * Execution:
     * [RO_Check]      : bcmltm_fa_node_read_only_input_check
     * [Wide_Value]    : bcmltm_fa_node_wide_field_check
     * [Value_Rev_Xfrm]: bcmltm_md_xfrm_fa_trees_rev_create
     * [Value_FV]      : bcmltm_md_validate_fa_trees_create
     * [Value_Fwd_Xfrm]: bcmltm_md_xfrm_fa_trees_fwd_create
     * [Value_A2P]     : bcmltm_fa_node_api_fields_unset_to_wb
     *
     * -----------------------------------------------------------------
     * For more information on Field Transform <Xfrm> and
     * Field Validation <FV> clusters, please see description
     * at the top of this file.
     *
     * Note: No-TI-Map track index is supplied by LTM.
     *       TI-Map    track index is supplied by map field transform.
     */

    /*-----------------
     * Tree_0
     */

    /*
     * [Track_Index]: Calculate tracking index from tracking elements
     *                (No-TI-Map).
     */
    if (!track_index_map) {
        SHR_IF_ERR_EXIT
            (bcmltm_db_dm_track_index_get(unit, sid, &track_info));
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_wbc_to_track,
                                           (void *)track_info,
                                           &track_index_nd));
    }
    track_index_node =
        bcmltm_tree_node_create((void *)track_index_nd,
                                &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(track_index_node, SHR_E_MEMORY);
    fa_node_roots[0] = track_index_node;

    /* [Key_Fwd_Xfrm]: Process LTA key forward transforms */
    xfrm_type = BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_fwd_create(unit, sid,
                                            xfrm_type, xfrm_lta_fopt,
                                            BCMLT_OPCODE_UPDATE,
                                            track_index_node,
                                            op_md_ptr,
                                            &key_fwd_xfrm_node));

    /* [Key_FV]: Process LTA key field validations */
    field_type = BCMLTM_FIELD_TYPE_KEY;
    SHR_IF_ERR_EXIT
        (bcmltm_md_validate_fa_trees_create(unit, sid, field_type,
                                            BCMLT_OPCODE_UPDATE,
                                            key_fwd_xfrm_node,
                                            op_md_ptr,
                                            &key_fv_node));

    /* [Key_A2P]: Convert key fields from API to physical */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_field_mapping_get(unit, sid,
                                        BCMLTM_FIELD_TYPE_KEY,
                                        BCMLTM_FIELD_DIR_IN,
                                        &key_map));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_key_fields_to_wb,
                                       (void *)key_map,
                                       &key_a2p_nd));
    key_a2p_node = bcmltm_tree_right_node_create(track_index_node,
                                                 (void *)key_a2p_nd,
                                                 &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(key_a2p_node, SHR_E_MEMORY);

    /* [Fixed_Key]: Set Fixed key fields */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_fixed_keys_get(unit, sid, &fixed_keys));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_fixed_fields_to_wb,
                                       (void *)fixed_keys,
                                       &fixed_key_nd));
    fixed_key_node = bcmltm_tree_left_node_create(key_a2p_node,
                                                  (void *)fixed_key_nd,
                                                  &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(fixed_key_node, SHR_E_MEMORY);

    /*-----------------
     * Tree_1
     */

    /*
     * [Value_A2P]: Convert value fields from API to physical.
     *              Unset marked API value fields (revert to default).
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_field_mapping_get(unit, sid,
                                        BCMLTM_FIELD_TYPE_VALUE,
                                        BCMLTM_FIELD_DIR_IN,
                                        &value_map));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_fields_unset_to_wb,
                                       (void *)value_map,
                                       &value_a2p_nd));
    value_a2p_node = bcmltm_tree_node_create((void *)value_a2p_nd,
                                             &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(value_a2p_node, SHR_E_MEMORY);
    fa_node_roots[1] = value_a2p_node;

    /* [Value_Fwd_Xfrm]: Process LTA value forward transforms */
    xfrm_type = BCMLTM_DB_XFRM_TYPE_FWD_VALUE_TRANSFORM;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_fwd_create(unit, sid,
                                            xfrm_type, xfrm_lta_fopt,
                                            BCMLT_OPCODE_UPDATE,
                                            value_a2p_node,
                                            op_md_ptr,
                                            &value_fwd_xfrm_node));

    /* [Value_FV]: Process LTA value field validations */
    field_type = BCMLTM_FIELD_TYPE_VALUE;
    SHR_IF_ERR_EXIT
        (bcmltm_md_validate_fa_trees_create(unit, sid, field_type,
                                            BCMLT_OPCODE_UPDATE,
                                            value_fwd_xfrm_node,
                                            op_md_ptr,
                                            &value_fv_node));

    /* [Value_Rev_Xfrm]: Process LTA value reverse transforms */
    xfrm_type = BCMLTM_DB_XFRM_TYPE_REV_VALUE_TRANSFORM;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_rev_create(unit, sid,
                                            xfrm_type,
                                            BCMLT_OPCODE_UPDATE,
                                            value_fv_node,
                                            op_md_ptr,
                                            &value_rev_xfrm_node));

    /* [RO_Check]: Verify no read-only fields present in input */
    SHR_IF_ERR_EXIT
        (bcmltm_db_read_only_list_get(unit, sid, &ro_list));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_read_only_input_check,
                                       (void *)ro_list,
                                       &ro_check_nd));
    ro_check_node = bcmltm_tree_left_node_create(value_rev_xfrm_node,
                                                 (void *)ro_check_nd,
                                                 &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(ro_check_node, SHR_E_MEMORY);

    /*
     * [Wide_Value]: Verify all or none of wide value field elements
     * present in input
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_wide_field_list_get(unit, sid, &wide_list));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_wide_field_check,
                                       (void *)wide_list,
                                       &wide_field_nd));
    wide_field_node = bcmltm_tree_right_node_create(value_rev_xfrm_node,
                                                 (void *)wide_field_nd,
                                                 &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(wide_field_node, SHR_E_MEMORY);

    /* Set returning pointer */
    op_md_ptr->fa_node_roots = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy node data if it it is not in node tree */
        if (wide_field_node == NULL) {
            SHR_FREE(wide_field_nd);
        }
        if (ro_check_node == NULL) {
            SHR_FREE(ro_check_nd);
        }
        if (track_index_node == NULL) {
            SHR_FREE(track_index_nd);
        }
        if (key_a2p_node == NULL) {
            SHR_FREE(key_a2p_nd);
        }
        if (fixed_key_node == NULL) {
            SHR_FREE(fixed_key_nd);
        }
        if (value_a2p_node == NULL) {
            SHR_FREE(value_a2p_nd);
        }

        /* Destroy trees */
        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, fa_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->fa_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create EE tree roots for UPDATE on Index LT.
 *
 * This function creates the EE tree roots for a UPDATE operation
 * on given Index logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_ee_node_roots_update_create(int unit,
                                     bcmlrd_sid_t sid,
                                     const bcmltm_table_attr_t *attr,
                                     bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bool interactive = FALSE;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *read_node = NULL;
    bcmltm_node_t *index_lk_node = NULL;
    bcmltm_node_t *write_node = NULL;
    bcmltm_ee_node_t *read_nd = NULL;
    bcmltm_ee_node_t *index_lk_nd = NULL;
    bcmltm_ee_node_t *write_nd = NULL;
    const bcmltm_pt_list_t *pt_list = NULL;
    const bcmltm_ee_index_info_t *index_info = NULL;
    bcmltm_ee_node_f read_func = NULL;
    bcmltm_ee_node_f write_func = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get Index table properties */
    if (BCMLTM_TABLE_ATTR_MODE_IS_INTERACTIVE(*attr)) {
        interactive = TRUE;
    }

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, size, "bcmltmIndexUpdateEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /*
     * Get EE node functions for corresponding PTM interface
     * based on table operating mode.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_ptm_index_func_get(interactive,
                                              &read_func,
                                              &write_func));

    /*
     * UPDATE
     *
     * EE Tree Roots
     * -----------------------------------------------------------------
     * Tree_0:
     *                         [Read]
     *                            |
     *                 +----------+----------+
     *                 |                     |
     *           [Index_Lk]                  =
     *
     * Execution:
     * [Index_Lk]: bcmltm_ee_node_model_index_exists
     * [Read]    : <bcmltm_ee_node_schan_read> (Interactive)
     *             <bcmltm_ee_node_model_index_read> (Modeled)
     *
     * -----------------------------------------------------------------
     * Tree_0:
     *                         [Write]
     *
     * Execution:
     * [Write]     : <bcmltm_ee_node_schan_write> (Interactive)
     *               <bcmltm_ee_node_model_index_write> (Modeled)
     */

    /*-----------------
     * Tree_0
     */

    /* [Read]: Execute PTM read operation */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_pt_list_get(unit, sid, &pt_list));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       read_func,
                                       (void *)pt_list,
                                       &read_nd));
    read_node = bcmltm_tree_node_create((void *)read_nd,
                                        &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(read_node, SHR_E_MEMORY);
    ee_node_roots[0] = read_node;

    /* [Index_Lk]: LT index lookup resource operation */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_ee_lt_index_get(unit, sid, &index_info));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_model_index_exists,
                                       (void *)index_info,
                                       &index_lk_nd));
    index_lk_node = bcmltm_tree_left_node_create(read_node,
                                                 (void *)index_lk_nd,
                                                 &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(index_lk_node, SHR_E_MEMORY);

    /*-----------------
     * Tree_1
     */

    /* [Write]: Execute PTM write operation */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       write_func,
                                       (void *)pt_list,
                                       &write_nd));
    write_node = bcmltm_tree_node_create((void *)write_nd,
                                         &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(write_node, SHR_E_MEMORY);
    ee_node_roots[1] = write_node;

    /* Set returning pointer */
    op_md_ptr->ee_node_roots = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy node data if it it is not in node tree */
        if (read_node == NULL) {
            SHR_FREE(read_nd);
        }
        if (index_lk_node == NULL) {
            SHR_FREE(index_lk_nd);
        }
        if (write_node == NULL) {
            SHR_FREE(write_nd);
        }

        /* Destroy trees */
        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, ee_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->ee_node_roots = NULL;
    }

    SHR_FUNC_EXIT();

}

/*!
 * \brief Create UPDATE operation metadata for Index LT.
 *
 * This routine creates the operation metadata for given table ID
 * to use for the UPDATE opcode on Index LTs.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_op_update_create(int unit, bcmlrd_sid_t sid,
                          bcmltm_lt_op_md_t **op_md_ptr)
{
    const bcmltm_table_attr_t *table_attr;
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get table attributes */
    SHR_IF_ERR_EXIT
        (bcmltm_db_table_attr_get(unit, sid, &table_attr));

    /* Get working buffer size */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "bcmltmIndexUpdateLtOpMd");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_UPDATE_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lt_index_fa_node_roots_update_create(unit, sid, table_attr,
                                              op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_index_ee_node_roots_update_create(unit, sid, table_attr,
                                              op_md));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(op_md);
        *op_md_ptr = NULL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create FA tree roots for TRAVERSE on Index LT.
 *
 * This function creates the FA tree roots for a TRAVERSE operation
 * on given Index logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_fa_node_roots_traverse_create(int unit,
                                       bcmlrd_sid_t sid,
                                       const bcmltm_table_attr_t *attr,
                                       bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bool track_index_map = FALSE;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *root_node = NULL;
    bcmltm_node_t *track_t2p_node = NULL;
    bcmltm_node_t *track_trv_node = NULL;
    bcmltm_node_t *key_fwd_xfrm_node = NULL;
    bcmltm_node_t *key_fv_node = NULL;
    bcmltm_node_t *track_index_node = NULL;
    bcmltm_node_t *key_a2p_node = NULL;
    bcmltm_node_t *fixed_key_node = NULL;
    bcmltm_node_t *track_dec_node = NULL;
    bcmltm_node_t *key_p2a_node = NULL;
    bcmltm_node_t *value_rev_xfrm_node = NULL;
    bcmltm_node_t *key_rev_xfrm_node = NULL;
    bcmltm_node_t *value_p2a_node = NULL;
    bcmltm_fa_node_t *track_t2p_nd = NULL;
    bcmltm_fa_node_t *track_trv_nd = NULL;
    bcmltm_fa_node_t *track_index_nd = NULL;
    bcmltm_fa_node_t *key_a2p_nd = NULL;
    bcmltm_fa_node_t *fixed_key_nd = NULL;
    bcmltm_fa_node_t *track_dec_nd = NULL;
    bcmltm_fa_node_t *key_p2a_nd = NULL;
    bcmltm_fa_node_t *value_p2a_nd = NULL;
    const bcmltm_wb_copy_list_t *track_t2p_list = NULL;
    const bcmltm_track_index_t *track_info = NULL;
    const bcmltm_field_select_mapping_t *key_in_map = NULL;
    const bcmltm_field_select_mapping_t *key_out_map = NULL;
    const bcmltm_field_select_mapping_t *value_map = NULL;
    const bcmltm_fixed_field_list_t *fixed_keys = NULL;
    int xfrm_type;
    bcmltm_md_lta_fopt_t xfrm_lta_fopt = 0x0;
    bcmltm_md_lta_fopt_t xfrm_lta_fopt_no_api;
    bcmltm_field_type_t field_type;

    SHR_FUNC_ENTER(unit);

    /* Track index: derived from map or LTM */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_track_index_is_mapped(unit, sid, &track_index_map));

    /* Allocate pointers to FA tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, size, "bcmltmIndexTraverseFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, size);

    /*
     * TRAVERSE
     *
     * FA Tree Roots
     * -----------------------------------------------------------------
     * Tree_0:
     *                                                 [Root]
     *                                                    |
     *                                              +-----+-----+
     *                                              |           |
     *                                       [Key_Fwd_Xfrm]     =
     *                                              |
     *                                        +-----+-----+
     *                                        |           |
     *                                 [Key_Rev_Xfrm]     =
     *                                        |
     *                          +-------------+-------------+
     *                          |                           |
     *                    [Track_Trv]                 [Track_T2P]
     *                          |                           |
     *                 +--------+--------+             +----+----+
     *                 |                 |             |         |
     *           [Key_Fwd_Xfrm]    [Track_Index]       =     [Track_Dec]
     *                 |                 |
     *           +-----+-----+     +-----+-----+
     *           |           |     |           |
     *       [Key_FV]        =     =       [Key_A2P]
     *                                         |
     *                                   +-----+-----+
     *                                   |           |
     *                              [Fixed_Key]      =
     *
     * Execution:
     * [Key_FV]      : bcmltm_md_validate_fa_trees_create
     * [Key_Fwd_Xfrm]: bcmltm_md_xfrm_fa_trees_fwd_create
     * [Fixed_Key]   : bcmltm_fa_node_fixed_fields_to_wb
     * [Key_A2P]     : bcmltm_fa_node_api_key_fields_to_wb
     * [Track_Index] : bcmltm_fa_node_wbc_to_track (No-TI-Map)
     * [Track_Trv]   : bcmltm_fa_node_traverse_track
     * [Track_Dec]   : bcmltm_fa_node_track_to_wbc (No-TI-Map)
     * [Track_T2P]   : bcmltm_fa_node_wbc_to_wbc
     * [Key_Rev_Xfrm]: bcmltm_md_xfrm_fa_trees_rev_create
     * [Key_Fwd_Xfrm]: bcmltm_md_xfrm_fa_trees_fwd_create
     *
     * -----------------------------------------------------------------
     * Tree_1:
     *                                 [Key_P2A]
     *                                     |
     *                        +------------+------------+
     *                        |                         |
     *                 [Value_Rev_Xfrm]              [Value_P2A]
     *
     * Execution:
     *
     * [Value_Rev_Xfrm]: bcmltm_md_xfrm_fa_trees_rev_create
     * [Value_P2A]     : bcmltm_fa_node_wb_to_api_fields
     * [Key_P2A]       : bcmltm_fa_node_wb_to_api_key_fields (No-TI-Map)
     *
     * -----------------------------------------------------------------
     * For more information on Field Transform <Xfrm> and
     * Field Validation <FV> clusters, please see description
     * at the top of this file.
     *
     * Note: No-TI-Map track index is supplied by LTM.
     *       TI-Map    track index is supplied by map field transform.
     */

    /*-----------------
     * Tree_0
     */

    /* [Root]: Nothing to do (used for chaining nodes) */
    root_node = bcmltm_tree_node_create((void *)NULL,
                                        &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(root_node, SHR_E_MEMORY);
    fa_node_roots[0] = root_node;

    /*
     * [Key_Fwd_Xfrm]: Process LTA key forward transforms.
     *                 Propagate the next track index into the
     *                 corresponding PT key destinations.
     */
    /* Indicate to skip api input and copy reverse output instead */
    xfrm_lta_fopt_no_api = BCMLTM_MD_LTA_FOPT_IN_NO_API |
        BCMLTM_MD_LTA_FOPT_IN_LTA;
    xfrm_type = BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_fwd_create(unit, sid,
                                            xfrm_type,
                                            xfrm_lta_fopt_no_api,
                                            BCMLT_OPCODE_TRAVERSE,
                                            root_node,
                                            op_md_ptr,
                                            &key_fwd_xfrm_node));

    /* [Key_Rev_Xfrm]: Process LTA key reverse transforms */
    xfrm_type = BCMLTM_DB_XFRM_TYPE_REV_KEY_TRANSFORM;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_rev_create(unit, sid,
                                            xfrm_type,
                                            BCMLT_OPCODE_TRAVERSE,
                                            key_fwd_xfrm_node,
                                            op_md_ptr,
                                            &key_rev_xfrm_node));


    /* [Track_Trv]: Determine next tracking index */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_track_index_get(unit, sid, &track_info));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_traverse_track,
                                       (void *)track_info,
                                       &track_trv_nd));
    track_trv_node = bcmltm_tree_left_node_create(key_rev_xfrm_node,
                                                  (void *)track_trv_nd,
                                                  &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(track_trv_node, SHR_E_MEMORY);

    /* [Key_Fwd_Xfrm]: Process LTA key forward transforms */
    xfrm_type = BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_fwd_create(unit, sid,
                                            xfrm_type, xfrm_lta_fopt,
                                            BCMLT_OPCODE_TRAVERSE,
                                            track_trv_node,
                                            op_md_ptr,
                                            &key_fwd_xfrm_node));

    /* [Key_FV]: Process LTA key field validations */
    field_type = BCMLTM_FIELD_TYPE_KEY;
    SHR_IF_ERR_EXIT
        (bcmltm_md_validate_fa_trees_create(unit, sid, field_type,
                                            BCMLT_OPCODE_TRAVERSE,
                                            key_fwd_xfrm_node,
                                            op_md_ptr,
                                            &key_fv_node));

    /*
     * [Track_Index]: Calculate tracking index from tracking elements
     *                (No-TI-Map).
     */
    if (!track_index_map) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_wbc_to_track,
                                           (void *)track_info,
                                           &track_index_nd));
    }
    track_index_node = bcmltm_tree_right_node_create(track_trv_node,
                                                     (void *)track_index_nd,
                                                     &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(track_index_node, SHR_E_MEMORY);

    /* [Key_A2P]: Convert key fields from API to physical */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_field_mapping_get(unit, sid,
                                        BCMLTM_FIELD_TYPE_KEY,
                                        BCMLTM_FIELD_DIR_IN,
                                        &key_in_map));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_key_fields_to_wb,
                                       (void *)key_in_map,
                                       &key_a2p_nd));
    key_a2p_node = bcmltm_tree_right_node_create(track_index_node,
                                                 (void *)key_a2p_nd,
                                                 &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(key_a2p_node, SHR_E_MEMORY);

    /* [Fixed_Key]: Set Fixed key fields */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_fixed_keys_get(unit, sid, &fixed_keys));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_fixed_fields_to_wb,
                                       (void *)fixed_keys,
                                       &fixed_key_nd));
    fixed_key_node = bcmltm_tree_left_node_create(key_a2p_node,
                                                  (void *)fixed_key_nd,
                                                  &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(fixed_key_node, SHR_E_MEMORY);

    /* [Track_T2P]: Copy tracking elements to PT fields */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_track_to_pt_copy_get(unit, sid, &track_t2p_list));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_wbc_to_wbc,
                                       (void *)track_t2p_list,
                                       &track_t2p_nd));
    track_t2p_node = bcmltm_tree_right_node_create(key_rev_xfrm_node,
                                                   (void *)track_t2p_nd,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(track_t2p_node, SHR_E_MEMORY);

    /* [Track_Dec]: Decode tracking index to tracking elements (No-TI-Map) */
    if (!track_index_map) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_track_to_wbc,
                                           (void *)track_info,
                                           &track_dec_nd));
    }
    track_dec_node = bcmltm_tree_right_node_create(track_t2p_node,
                                                   (void *)track_dec_nd,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(track_dec_node, SHR_E_MEMORY);


    /*-----------------
     * Tree_1
     */

    /* [Key_P2A]: Convert key fields from physical to API */
    if (!track_index_map) {
        SHR_IF_ERR_EXIT
            (bcmltm_db_dm_field_mapping_get(unit, sid,
                                            BCMLTM_FIELD_TYPE_KEY,
                                            BCMLTM_FIELD_DIR_OUT,
                                            &key_out_map));
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_wb_to_api_key_fields,
                                           (void *)key_out_map,
                                           &key_p2a_nd));
    }
    key_p2a_node = bcmltm_tree_node_create((void *)key_p2a_nd,
                                           &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(key_p2a_node, SHR_E_MEMORY);
    fa_node_roots[1] = key_p2a_node;

    /* [Value_Rev_Xfrm]: Process LTA value reverse transforms */
    xfrm_type = BCMLTM_DB_XFRM_TYPE_REV_VALUE_TRANSFORM;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_rev_create(unit, sid,
                                            xfrm_type,
                                            BCMLT_OPCODE_TRAVERSE,
                                            key_p2a_node,
                                            op_md_ptr,
                                            &value_rev_xfrm_node));

    /* [Value_P2A]: Convert value fields from physical to API */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_field_mapping_get(unit, sid,
                                        BCMLTM_FIELD_TYPE_VALUE,
                                        BCMLTM_FIELD_DIR_OUT,
                                        &value_map));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_wb_to_api_fields,
                                       (void *)value_map,
                                       &value_p2a_nd));
    value_p2a_node = bcmltm_tree_right_node_create(key_p2a_node,
                                                   (void *)value_p2a_nd,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(value_p2a_node, SHR_E_MEMORY);

    /* Set returning pointer */
    op_md_ptr->fa_node_roots = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {

        /* Destroy node data if it it is not in node tree */
        if (track_t2p_node == NULL) {
            SHR_FREE(track_t2p_nd);
        }
        if (track_trv_node == NULL) {
            SHR_FREE(track_trv_nd);
        }
        if (track_index_node == NULL) {
            SHR_FREE(track_index_nd);
        }
        if (key_a2p_node == NULL) {
            SHR_FREE(key_a2p_nd);
        }
        if (fixed_key_node == NULL) {
            SHR_FREE(fixed_key_nd);
        }
        if (track_dec_node == NULL) {
            SHR_FREE(track_dec_nd);
        }
        if (key_p2a_node == NULL) {
            SHR_FREE(key_p2a_nd);
        }
        if (value_p2a_node == NULL) {
            SHR_FREE(value_p2a_nd);
        }

        /* Destroy trees */
        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, fa_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->fa_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create EE tree roots for TRAVERSE on Index LT.
 *
 * This function creates the EE tree roots for a TRAVERSE operation
 * on given Index logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_ee_node_roots_traverse_create(int unit,
                                       bcmlrd_sid_t sid,
                                       const bcmltm_table_attr_t *attr,
                                       bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bool interactive = FALSE;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *read_node = NULL;
    bcmltm_ee_node_t *read_nd = NULL;
    bcmltm_ee_node_f read_func = NULL;
    const bcmltm_pt_list_t *pt_list = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get Index table properties */
    if (BCMLTM_TABLE_ATTR_MODE_IS_INTERACTIVE(*attr)) {
        interactive = TRUE;
    }

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, size, "bcmltmIndexTraverseEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /*
     * Get EE node functions for corresponding PTM interface
     * based on table operating mode.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_ptm_index_func_get(interactive,
                                              &read_func,
                                              NULL));

    /*
     * TRAVERSE
     *
     * EE Trees Roots
     * -----------------------------------------------------------------
     * Tree_0:
     *                         [Read]
     *
     * Execution:
     * [Read]    : <bcmltm_ee_node_schan_read> (Interactive)
     *             <bcmltm_ee_node_model_index_read> (Modeled)
     *
     * -----------------------------------------------------------------
     * Tree_1:                 [NULL]
     */

    /*-----------------
     * Tree_0
     */

    /* [Read]: Execute PTM read operation */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_pt_list_get(unit, sid, &pt_list));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       read_func,
                                       (void *)pt_list,
                                       &read_nd));
    read_node = bcmltm_tree_node_create((void *)read_nd,
                                        &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(read_node, SHR_E_MEMORY);
    ee_node_roots[0] = read_node;

    /*-----------------
     * Tree_1
     */

    /* [NULL]: No actions */
    ee_node_roots[1] = NULL;  /* Not used */

    /* Set returning pointer */
    op_md_ptr->ee_node_roots = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy node data if it it is not in node tree */
        if (read_node == NULL) {
            SHR_FREE(read_nd);
        }

        /* Destroy trees */
        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, ee_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->ee_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create TRAVERSE operation metadata for Index LT.
 *
 * This routine creates the operation metadata for given table ID
 * to use for the TRAVERSE opcode on Index LTs.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_op_traverse_create(int unit, bcmlrd_sid_t sid,
                            bcmltm_lt_op_md_t **op_md_ptr)
{
    const bcmltm_table_attr_t *table_attr;
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get table attributes */
    SHR_IF_ERR_EXIT
        (bcmltm_db_table_attr_get(unit, sid, &table_attr));

    /* Get working buffer size */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "bcmltmIndexTraverseLtOpMd");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_TRAVERSE_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lt_index_fa_node_roots_traverse_create(unit, sid, table_attr,
                                                op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_index_ee_node_roots_traverse_create(unit, sid, table_attr,
                                                op_md));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(op_md);
        *op_md_ptr = NULL;
    }
    SHR_FUNC_EXIT();
}

/*
 * LT Metadata Function Vector for Direct Mapped Index LT (no field select).
 */
static bcmltm_md_lt_drv_t lt_index_drv = {
    /* op_destroy */            bcmltm_md_op_destroy,
    /* op_insert_create */      lt_index_op_insert_create,
    /* op_lookup_create */      lt_index_op_lookup_create,
    /* op_delete_create */      lt_index_op_delete_create,
    /* op_update_create */      lt_index_op_update_create,
    /* op_traverse_create */    lt_index_op_traverse_create,
};

/*******************************************************************************
 * Public functions
 */

const bcmltm_md_lt_drv_t *
bcmltm_md_lt_index_drv_get(void)
{
    return &lt_index_drv;
}

