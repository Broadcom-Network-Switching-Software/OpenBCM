/*! \file bcmltm_md_lta_xfrm_fs.c
 *
 * Logical Table Manager - LTA Field Transform with Field Selection.
 *
 * This file contains interfaces to construct the field transform
 * subtrees with field selections.
 *
 * FA Field Transform Cluster
 * --------------------------
 *
 * The field transform cluster (for Keys and Values) is
 * composed of a number of subtrees formed as follows:
 *
 *                         [Parent]
 *                             |
 *                   +---------+---------+
 *                   |                   |
 *            [Xfrm_1 Subtree]          ===
 *                   |
 *          +--------+--------+
 *          |                 |
 *         ...               ===
 *    [Xfrm_N Subtree]
 *
 * Where "N" is the number of field transforms for the given
 * transform type.
 *
 * Each transform subtree is composed of a set of nodes required
 * to perform the transform flow.
 *
 * The order of execution in the cluster follow the same LTM
 * tree traverse order (i.e. post-order), which is:
 * Xfrm_N subtree, Xfrm_(N-1) subtree, ..., Xfrm_1 subtree.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_types.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>

#include <bcmlrd/bcmlrd_types.h>

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_types_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_db_internal.h>
#include <bcmltm/bcmltm_fa_tree_nodes_internal.h>

#include "bcmltm_md_op.h"
#include "bcmltm_md_lta.h"
#include "bcmltm_md_lta_xfrm_fs.h"

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/*!
 * \brief Create the field transform FA subtree.
 *
 * This routine creates the field transform FA subtree
 * for the specified field selection ID, validation type, and index,
 * for a given table.
 *
 * The subtree is attached to the left node of given parent node.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] selection_id Field selection ID.
 * \param [in] xfrm_type Field transform type.
 * \param [in] xfrm_idx Field transform index.
 * \param [in] lta_flags LTA flags.
 * \param [in] opcode LT opcode.
 * \param [in] parent_node Parent node for this subtree.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*xfrm_fa_subtree_create_f)(int unit,
                                  bcmlrd_sid_t sid,
                                  bcmltm_field_selection_id_t selection_id,
                                  bcmltm_field_xfrm_type_t xfrm_type,
                                  uint32_t xfrm_idx,
                                  bcmltm_md_lta_flags_t lta_flags,
                                  bcmlt_opcode_t opcode,
                                  bcmltm_node_t *parent_node,
                                  bcmltm_lt_op_md_t *op_md_ptr,
                                  bcmltm_node_t **next_chain);

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Create the forward field transform FA subtree.
 *
 * This routine creates the forward field transform FA subtree
 * for the specified field selection ID, validation type, and index,
 * for a given table.
 *
 * The subtree is attached to the left node of given parent node.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] selection_id Field selection ID.
 * \param [in] xfrm_type Field transform type.
 * \param [in] xfrm_idx Field transform index.
 * \param [in] lta_flags LTA flags.
 * \param [in] opcode LT opcode.
 * \param [in] parent_node Parent node for this subtree.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_fs_fwd_fa_subtree_create(int unit,
                              bcmlrd_sid_t sid,
                              bcmltm_field_selection_id_t selection_id,
                              bcmltm_field_xfrm_type_t xfrm_type,
                              uint32_t xfrm_idx,
                              bcmltm_md_lta_flags_t lta_flags,
                              bcmlt_opcode_t opcode,
                              bcmltm_node_t *parent_node,
                              bcmltm_lt_op_md_t *op_md_ptr,
                              bcmltm_node_t **next_chain)
{
    bcmltm_node_t *next_chain_node = NULL;
    bcmltm_node_t *pt_sid_map_node = NULL;
    bcmltm_fa_node_t *pt_sid_map_nd = NULL;
    bcmltm_node_t *field_x2p_node = NULL;
    bcmltm_fa_node_t *field_x2p_nd = NULL;
    bcmltm_node_t *in_init_node = NULL;
    bcmltm_fa_node_t *in_init_nd = NULL;
    bcmltm_node_t *transform_node = NULL;
    bcmltm_fa_node_t *transform_nd = NULL;
    bcmltm_node_t *out_init_node = NULL;
    bcmltm_fa_node_t *out_init_nd = NULL;
    bcmltm_node_t *in_key_init_node = NULL;
    bcmltm_fa_node_t *in_key_init_nd = NULL;
    bcmltm_node_t *field_c2x_node = NULL;
    bcmltm_fa_node_t *field_c2x_nd = NULL;
    bcmltm_node_t *in_key_c2x_node = NULL;
    bcmltm_fa_node_t *in_key_c2x_nd = NULL;
    bcmltm_fa_node_f transform_func = NULL;
    const bcmltm_lta_transform_params_t *params = NULL;
    const bcmltm_lta_select_field_list_t *in_fields = NULL;
    const bcmltm_lta_select_field_list_t *out_fields = NULL;
    const bcmltm_lta_select_field_list_t *in_keys = NULL;
    bool ext_transform = FALSE;

    SHR_FUNC_ENTER(unit);

    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_xfrm_params_get(unit, sid, selection_id,
                                      xfrm_type, xfrm_idx,
                                      &params));
    if ((params != NULL) && (params->lta_ext_transform != NULL)) {
        ext_transform = TRUE;
    }
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_xfrm_in_fields_get(unit, sid, selection_id,
                                         xfrm_type, xfrm_idx,
                                         &in_fields));
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_xfrm_out_fields_get(unit, sid, selection_id,
                                          xfrm_type, xfrm_idx,
                                          &out_fields));
    if (ext_transform) {
        SHR_IF_ERR_EXIT
            (bcmltm_db_dm_xfrm_in_key_fields_get(unit, sid, selection_id,
                                                 xfrm_type, xfrm_idx,
                                                 &in_keys));
    }

    /*
     * Construct subtree
     */

    /* [PT_SID_Map]: Convert LTA PT select index to PT SID */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_pt_sid_map,
                                       (void *)params,
                                       &pt_sid_map_nd));
    pt_sid_map_node = bcmltm_tree_left_node_create(parent_node,
                                                   (void *)pt_sid_map_nd,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(pt_sid_map_node, SHR_E_MEMORY);

    /* [Field_X2P]: Copy LTA output fields to PTM entries */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_select_lta_to_wbc,
                                       (void *)out_fields,
                                       &field_x2p_nd));
    field_x2p_node = bcmltm_tree_left_node_create(pt_sid_map_node,
                                                  (void *)field_x2p_nd,
                                                  &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(field_x2p_node, SHR_E_MEMORY);

    /* [In_Init]: Initialize LTA input fields */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_select_input_init,
                                       (void *)in_fields,
                                       &in_init_nd));
    in_init_node = bcmltm_tree_left_node_create(field_x2p_node,
                                                (void *)in_init_nd,
                                                &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(in_init_node, SHR_E_MEMORY);

    /* [Transform]: Invoke transform function */
    if (lta_flags & BCMLTM_MD_LTA_FLAG_TRAVERSE_START) {
        /* If this is a traverse start, skip transform (no keys supplied). */
        transform_func = bcmltm_fa_node_lta_traverse_transform;
    } else if (lta_flags & BCMLTM_MD_LTA_FLAG_ALLOC_SECOND) {
        /* If key allocation did not occur, skip transform */
        transform_func = bcmltm_fa_node_lta_alloc_second_transform;
    } else if (ext_transform) {
        /* This is an extended transform */
        transform_func = bcmltm_fa_node_lta_ext_transform;
    } else {
        transform_func = bcmltm_fa_node_lta_transform;
    }
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       transform_func,
                                       (void *)params,
                                       &transform_nd));
    transform_node = bcmltm_tree_right_node_create(field_x2p_node,
                                                   (void *)transform_nd,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(transform_node, SHR_E_MEMORY);

    /* [Next_Chain]: Chain to additional subtrees */
    next_chain_node = bcmltm_tree_left_node_create(in_init_node,
                                                   (void *)NULL,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(next_chain_node, SHR_E_MEMORY);

    /* [Out_Init]: Initialize LTA output fields */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_select_output_init,
                                       (void *)out_fields,
                                       &out_init_nd));
    out_init_node = bcmltm_tree_right_node_create(in_init_node,
                                                  (void *)out_init_nd,
                                                  &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(out_init_node, SHR_E_MEMORY);

    /* [In_Key_Init]: Initialize LTA input key fields (extended transform) */
    if (in_keys != NULL) {
        /* Node install is optional since it does not have any child */
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_lta_select_input_init,
                                           (void *)in_keys,
                                           &in_key_init_nd));
        in_key_init_node = bcmltm_tree_left_node_create(out_init_node,
                                                        (void *)in_key_init_nd,
                                                        &(op_md_ptr->num_nodes));
        SHR_NULL_CHECK(in_key_init_node, SHR_E_MEMORY);
    }

    /* [Field_C2X]: Link LTA input fields to API cache fields */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_select_api_wb_to_lta,
                                       (void *)in_fields,
                                       &field_c2x_nd));
    field_c2x_node = bcmltm_tree_right_node_create(transform_node,
                                                   (void *)field_c2x_nd,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(field_c2x_node, SHR_E_MEMORY);


    /* [In_Key_C2X]: Link LTA input key fields to API cache fields */
    if (in_keys != NULL) {
        /* Node install is optional since it does not have any child */
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_select_api_wb_to_lta,
                                           (void *)in_keys,
                                           &in_key_c2x_nd));
        in_key_c2x_node = bcmltm_tree_right_node_create(field_c2x_node,
                                                        (void *)in_key_c2x_nd,
                                                        &(op_md_ptr->num_nodes));
        SHR_NULL_CHECK(in_key_c2x_node, SHR_E_MEMORY);
    }

    /* Return chain to additional subtrees */
    *next_chain = next_chain_node;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy node data if it is not in node tree */
        if (pt_sid_map_node == NULL) {
            SHR_FREE(pt_sid_map_nd);
        }
        if (field_x2p_node == NULL) {
            SHR_FREE(field_x2p_nd);
        }
        if (in_init_node == NULL) {
            SHR_FREE(in_init_nd);
        }
        if (transform_node == NULL) {
            SHR_FREE(transform_nd);
        }
        if (out_init_node == NULL) {
            SHR_FREE(out_init_nd);
        }
        if (in_key_init_node == NULL) {
            SHR_FREE(in_key_init_nd);
        }
        if (field_c2x_node == NULL) {
            SHR_FREE(field_c2x_nd);
        }
        if (in_key_c2x_node == NULL) {
            SHR_FREE(in_key_c2x_nd);
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the reverse field transform FA subtree.
 *
 * This routine creates the reverse field transform FA subtree
 * for the specified field selection ID, validation type, and index,
 * for a given table.
 *
 * The subtree is attached to the left node of given parent node.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] selection_id Field selection ID.
 * \param [in] xfrm_type Field transform type.
 * \param [in] xfrm_idx Field transform index.
 * \param [in] lta_flags LTA flags.
 * \param [in] opcode LT opcode.
 * \param [in] parent_node Parent node for this subtree.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_fs_rev_fa_subtree_create(int unit,
                              bcmlrd_sid_t sid,
                              bcmltm_field_selection_id_t selection_id,
                              bcmltm_field_xfrm_type_t xfrm_type,
                              uint32_t xfrm_idx,
                              bcmltm_md_lta_flags_t lta_flags,
                              bcmlt_opcode_t opcode,
                              bcmltm_node_t *parent_node,
                              bcmltm_lt_op_md_t *op_md_ptr,
                              bcmltm_node_t **next_chain)
{
    bcmltm_node_t *next_chain_node = NULL;
    bcmltm_node_t *field_x2c_node = NULL;
    bcmltm_fa_node_t *field_x2c_nd = NULL;
    bcmltm_node_t *in_init_node = NULL;
    bcmltm_fa_node_t *in_init_nd = NULL;
    bcmltm_node_t *transform_node = NULL;
    bcmltm_fa_node_t *transform_nd = NULL;
    bcmltm_node_t *out_init_node = NULL;
    bcmltm_fa_node_t *out_init_nd = NULL;
    bcmltm_node_t *in_key_init_node = NULL;
    bcmltm_fa_node_t *in_key_init_nd = NULL;
    bcmltm_node_t *field_p2x_node = NULL;
    bcmltm_fa_node_t *field_p2x_nd = NULL;
    bcmltm_node_t *in_key_c2x_node = NULL;
    bcmltm_fa_node_t *in_key_c2x_nd = NULL;
    bcmltm_fa_node_f field_x2c_func = NULL;
    bcmltm_fa_node_f transform_func = NULL;
    const bcmltm_lta_transform_params_t *params = NULL;
    const bcmltm_lta_select_field_list_t *in_fields = NULL;
    const bcmltm_lta_select_field_list_t *out_fields = NULL;
    const bcmltm_lta_select_field_list_t *in_keys = NULL;
    bool ext_transform = FALSE;

    SHR_FUNC_ENTER(unit);

    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_xfrm_params_get(unit, sid, selection_id,
                                      xfrm_type, xfrm_idx,
                                      &params));
    if ((params != NULL) && (params->lta_ext_transform != NULL)) {
        ext_transform = TRUE;
    }
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_xfrm_in_fields_get(unit, sid, selection_id,
                                         xfrm_type, xfrm_idx,
                                         &in_fields));
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_xfrm_out_fields_get(unit, sid, selection_id,
                                          xfrm_type, xfrm_idx,
                                          &out_fields));
    if (ext_transform) {
        SHR_IF_ERR_EXIT
            (bcmltm_db_dm_xfrm_in_key_fields_get(unit, sid, selection_id,
                                                 xfrm_type, xfrm_idx,
                                                 &in_keys));
    }

    /*
     * Construct subtree
     */

    /* [Field_X2C]: Copy LTA output fields to API cache fields */
    if (lta_flags & BCMLTM_MD_LTA_FLAG_ALLOC_OUT) {
        /* Copy output fields only if key allocation occurred */
        field_x2c_func = bcmltm_fa_node_select_alloc_lta_to_api_wb;
    } else {
        field_x2c_func = bcmltm_fa_node_select_lta_to_api_wb;
    }
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       field_x2c_func,
                                       (void *)out_fields,
                                       &field_x2c_nd));
    field_x2c_node = bcmltm_tree_left_node_create(parent_node,
                                                 (void *)field_x2c_nd,
                                                  &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(field_x2c_node, SHR_E_MEMORY);

    /* [In_Init]: Initialize LTA input fields */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_select_input_init,
                                       (void *)in_fields,
                                       &in_init_nd));
    in_init_node = bcmltm_tree_left_node_create(field_x2c_node,
                                                (void *)in_init_nd,
                                                &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(in_init_node, SHR_E_MEMORY);

    /* [Transform]: Invoke transform function */
    if (lta_flags & BCMLTM_MD_LTA_FLAG_ALLOC_SECOND) {
        /* If key allocation did not occur, skip transform */
        transform_func = bcmltm_fa_node_lta_alloc_second_transform;
    } else if (ext_transform) {
        /* This is an extended transform */
        transform_func = bcmltm_fa_node_lta_ext_transform;
    } else {
        transform_func = bcmltm_fa_node_lta_transform;
    }
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       transform_func,
                                       (void *)params,
                                       &transform_nd));
    transform_node = bcmltm_tree_right_node_create(field_x2c_node,
                                                   (void *)transform_nd,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(transform_node, SHR_E_MEMORY);

    /* [Next_Chain]: Chain to additional subtrees */
    next_chain_node = bcmltm_tree_left_node_create(in_init_node,
                                                   (void *)NULL,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(next_chain_node, SHR_E_MEMORY);

    /* [Out_Init]: Initialize LTA output fields */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_select_output_init,
                                       (void *)out_fields,
                                       &out_init_nd));
    out_init_node = bcmltm_tree_right_node_create(in_init_node,
                                                  (void *)out_init_nd,
                                                  &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(out_init_node, SHR_E_MEMORY);

    /* [In_Key_Init]: Initialize LTA input key fields (extended transform) */
    if (in_keys != NULL) {
        /* Node install is optional since it does not have any child */
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_lta_select_input_init,
                                           (void *)in_keys,
                                           &in_key_init_nd));
        in_key_init_node = bcmltm_tree_left_node_create(out_init_node,
                                                        (void *)in_key_init_nd,
                                                        &(op_md_ptr->num_nodes));
        SHR_NULL_CHECK(in_key_init_node, SHR_E_MEMORY);
    }

    /* [Field_P2X]: Append PTM entry to LTA input fields */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_select_wbc_to_lta,
                                       (void *)in_fields,
                                       &field_p2x_nd));
    field_p2x_node = bcmltm_tree_left_node_create(transform_node,
                                                  (void *)field_p2x_nd,
                                                  &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(field_p2x_node, SHR_E_MEMORY);

    /* [In_Key_C2X]: Link LTA input key fields to API cache fields */
    if (in_keys != NULL) {
        /* Node install is optional since it does not have any child */
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_select_api_wb_to_lta,
                                           (void *)in_keys,
                                           &in_key_c2x_nd));
        in_key_c2x_node = bcmltm_tree_right_node_create(transform_node,
                                                        (void *)in_key_c2x_nd,
                                                        &(op_md_ptr->num_nodes));
        SHR_NULL_CHECK(in_key_c2x_node, SHR_E_MEMORY);
    }

    /* Return chain to additional subtrees */
    *next_chain = next_chain_node;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy node data if it is not in node tree */
        if (field_x2c_node == NULL) {
            SHR_FREE(field_x2c_nd);
        }
        if (in_init_node == NULL) {
            SHR_FREE(in_init_nd);
        }
        if (transform_node == NULL) {
            SHR_FREE(transform_nd);
        }
        if (out_init_node == NULL) {
            SHR_FREE(out_init_nd);
        }
        if (in_key_init_node == NULL) {
            SHR_FREE(in_key_init_nd);
        }
        if (field_p2x_node == NULL) {
            SHR_FREE(field_p2x_nd);
        }
        if (in_key_c2x_node == NULL) {
            SHR_FREE(in_key_c2x_nd);
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the reverse track field transform FA subtree.
 *
 * This routine creates the reverse field transform FA subtree
 * for the specified track reverse transform index,
 * for a given table.
 *
 * The subtree is attached to the left node of given parent node.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] xfrm_idx Field transform index.
 * \param [in] parent_node Parent node for this subtree.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_rev_track_fa_subtree_create(int unit,
                                 bcmlrd_sid_t sid,
                                 uint32_t xfrm_idx,
                                 bcmltm_node_t *parent_node,
                                 bcmltm_lt_op_md_t *op_md_ptr,
                                 bcmltm_node_t **next_chain)
{
    bcmltm_node_t *next_chain_node = NULL;
    bcmltm_node_t *field_x2c_node = NULL;
    bcmltm_fa_node_t *field_x2c_nd = NULL;
    bcmltm_node_t *in_init_node = NULL;
    bcmltm_fa_node_t *in_init_nd = NULL;
    bcmltm_node_t *transform_node = NULL;
    bcmltm_fa_node_t *transform_nd = NULL;
    bcmltm_node_t *out_init_node = NULL;
    bcmltm_fa_node_t *out_init_nd = NULL;
    bcmltm_node_t *field_p2x_node = NULL;
    bcmltm_fa_node_t *field_p2x_nd = NULL;
    bcmltm_fa_node_f field_x2c_func = NULL;
    bcmltm_fa_node_f transform_func = NULL;
    const bcmltm_lta_transform_params_t *params = NULL;
    const bcmltm_lta_select_field_list_t *in_fields = NULL;
    const bcmltm_lta_select_field_list_t *out_fields = NULL;

    SHR_FUNC_ENTER(unit);

    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_track_rev_xfrm_params_get(unit, sid,
                                                xfrm_idx,
                                                &params));
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_track_rev_xfrm_in_fields_get(unit, sid,
                                                   xfrm_idx,
                                                   &in_fields));
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_track_rev_xfrm_out_fields_get(unit, sid,
                                                    xfrm_idx,
                                                    &out_fields));
    /*
     * Construct subtree
     */

    /* [Field_X2C]: Copy LTA output fields to API cache fields */
    field_x2c_func = bcmltm_fa_node_select_lta_to_api_wb;
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       field_x2c_func,
                                       (void *)out_fields,
                                       &field_x2c_nd));
    field_x2c_node = bcmltm_tree_left_node_create(parent_node,
                                                 (void *)field_x2c_nd,
                                                  &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(field_x2c_node, SHR_E_MEMORY);

    /* [In_Init]: Initialize LTA input fields */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_select_input_init,
                                       (void *)in_fields,
                                       &in_init_nd));
    in_init_node = bcmltm_tree_left_node_create(field_x2c_node,
                                                (void *)in_init_nd,
                                                &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(in_init_node, SHR_E_MEMORY);

    /* [Transform]: Invoke transform function */
    transform_func = bcmltm_fa_node_lta_transform;
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       transform_func,
                                       (void *)params,
                                       &transform_nd));
    transform_node = bcmltm_tree_right_node_create(field_x2c_node,
                                                   (void *)transform_nd,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(transform_node, SHR_E_MEMORY);

    /* [Next_Chain]: Chain to additional subtrees */
    next_chain_node = bcmltm_tree_left_node_create(in_init_node,
                                                   (void *)NULL,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(next_chain_node, SHR_E_MEMORY);

    /* [Out_Init]: Initialize LTA output fields */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_select_output_init,
                                       (void *)out_fields,
                                       &out_init_nd));
    out_init_node = bcmltm_tree_right_node_create(in_init_node,
                                                  (void *)out_init_nd,
                                                  &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(out_init_node, SHR_E_MEMORY);

    /* [Field_P2X]: Append PTM entry to LTA input fields */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_select_wbc_to_lta,
                                       (void *)in_fields,
                                       &field_p2x_nd));
    field_p2x_node = bcmltm_tree_left_node_create(transform_node,
                                                  (void *)field_p2x_nd,
                                                  &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(field_p2x_node, SHR_E_MEMORY);

    /* Return chain to additional subtrees */
    *next_chain = next_chain_node;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy node data if it is not in node tree */
        if (field_x2c_node == NULL) {
            SHR_FREE(field_x2c_nd);
        }
        if (in_init_node == NULL) {
            SHR_FREE(in_init_nd);
        }
        if (transform_node == NULL) {
            SHR_FREE(transform_nd);
        }
        if (out_init_node == NULL) {
            SHR_FREE(out_init_nd);
        }
        if (field_p2x_node == NULL) {
            SHR_FREE(field_p2x_nd);
        }
    }

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_md_xfrm_fs_fa_cluster_create(int unit,
                                    bcmlrd_sid_t sid,
                                    bcmltm_field_selection_id_t selection_id,
                                    bcmltm_field_xfrm_type_t xfrm_type,
                                    bcmltm_md_lta_flags_t lta_flags,
                                    bcmlt_opcode_t opcode,
                                    bcmltm_node_side_t node_side,
                                    bcmltm_node_t *parent_chain,
                                    bcmltm_lt_op_md_t *op_md_ptr,
                                    bcmltm_node_t **next_chain)
{
    uint32_t idx;
    uint32_t num_xfrms = 0;
    bcmltm_node_t *parent_node = NULL;
    xfrm_fa_subtree_create_f xfrm_subtree_create = NULL;

    SHR_FUNC_ENTER(unit);

    *next_chain = parent_chain;

    /* Create anchor node for cluster in specified side/location */
    if (node_side == BCMLTM_NODE_SIDE_LEFT) {
        parent_node = bcmltm_tree_left_node_create(parent_chain,
                                                   (void *)NULL,
                                                   &(op_md_ptr->num_nodes));
    } else if (node_side == BCMLTM_NODE_SIDE_RIGHT) {
        parent_node = bcmltm_tree_right_node_create(parent_chain,
                                                    (void *)NULL,
                                                    &(op_md_ptr->num_nodes));
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_NULL_CHECK(parent_node, SHR_E_MEMORY);

    *next_chain = parent_node;

    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_xfrm_num_get(unit, sid, selection_id,
                                   xfrm_type, &num_xfrms));
    if (num_xfrms == 0) {
        SHR_EXIT();
    }

    /* Transform to call, forward or reverse */
    if ((xfrm_type == BCMLTM_FIELD_XFRM_TYPE_FWD_KEY) ||
        (xfrm_type == BCMLTM_FIELD_XFRM_TYPE_FWD_VALUE)) {
        xfrm_subtree_create = xfrm_fs_fwd_fa_subtree_create;
    } else {
        xfrm_subtree_create = xfrm_fs_rev_fa_subtree_create;
    }

    /* Add transform subtrees */
    for (idx = 0; idx < num_xfrms; idx++) {
        SHR_IF_ERR_EXIT
            (xfrm_subtree_create(unit, sid, selection_id,
                                 xfrm_type, idx,
                                 lta_flags, opcode,
                                 parent_node, op_md_ptr,
                                 next_chain));
        parent_node = *next_chain;
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_md_xfrm_track_rev_cluster_create(int unit,
                                        bcmlrd_sid_t sid,
                                        bcmltm_node_side_t node_side,
                                        bcmltm_node_t *parent_chain,
                                        bcmltm_lt_op_md_t *op_md_ptr,
                                        bcmltm_node_t **next_chain)
{
    uint32_t idx;
    uint32_t num_xfrms = 0;
    bcmltm_node_t *parent_node = NULL;

    SHR_FUNC_ENTER(unit);

    *next_chain = parent_chain;

    /* Create anchor node for cluster in specified side/location */
    if (node_side == BCMLTM_NODE_SIDE_LEFT) {
        parent_node = bcmltm_tree_left_node_create(parent_chain,
                                                   (void *)NULL,
                                                   &(op_md_ptr->num_nodes));
    } else if (node_side == BCMLTM_NODE_SIDE_RIGHT) {
        parent_node = bcmltm_tree_right_node_create(parent_chain,
                                                    (void *)NULL,
                                                    &(op_md_ptr->num_nodes));
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_NULL_CHECK(parent_node, SHR_E_MEMORY);

    *next_chain = parent_node;

    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_track_rev_xfrm_num_get(unit, sid,
                                             &num_xfrms));
    if (num_xfrms == 0) {
        SHR_EXIT();
    }

    /* Add transform subtrees */
    for (idx = 0; idx < num_xfrms; idx++) {
        SHR_IF_ERR_EXIT
            (xfrm_rev_track_fa_subtree_create(unit, sid, idx,
                                              parent_node, op_md_ptr,
                                              next_chain));
        parent_node = *next_chain;
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_md_xfrm_fs_fwd_key_cluster_create(int unit,
                                         bcmlrd_sid_t sid,
                                         bcmlt_opcode_t opcode,
                                         bool traverse_start,
                                         bcmltm_node_side_t node_side,
                                         bcmltm_node_t *parent_node,
                                         bcmltm_lt_op_md_t *op_md_ptr,
                                         bcmltm_node_t **next_chain)
{
    bcmltm_field_xfrm_type_t xfrm_type;
    bcmltm_md_lta_flags_t lta_flags;

    SHR_FUNC_ENTER(unit);

    /* [Key_Fwd_Xfrm]: Process LTA key forward transforms */
    xfrm_type = BCMLTM_FIELD_XFRM_TYPE_FWD_KEY;
    lta_flags = BCMLTM_MD_LTA_FLAG_DEFAULTS;

    if (opcode == BCMLT_OPCODE_TRAVERSE) {
        if (traverse_start) {
            /*
             * Check on TRAVERSE start flag.
             *
             * This is normally the case for the first round of call
             * to the forward key transform.
             *
             * On TRAVERSE, the driver for Index LTs calls the forward
             * key transforms twice (current keys and next keys)
             * whereas the Keyed LT driver only calls them once
             * (PTM returns the next keys).
             */
            lta_flags |= BCMLTM_MD_LTA_FLAG_TRAVERSE_START;
        }
    }

    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fs_fa_cluster_create(unit, sid,
                                             BCMLTM_FIELD_SELECTION_ID_KEY,
                                             xfrm_type,
                                             lta_flags,
                                             opcode,
                                             node_side,
                                             parent_node,
                                             op_md_ptr,
                                             next_chain));
 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_md_xfrm_fs_rev_key_cluster_create(int unit,
                                         bcmlrd_sid_t sid,
                                         bcmlt_opcode_t opcode,
                                         bcmltm_node_side_t node_side,
                                         bcmltm_node_t *parent_node,
                                         bcmltm_lt_op_md_t *op_md_ptr,
                                         bcmltm_node_t **next_chain)
{
    bcmltm_field_xfrm_type_t xfrm_type;
    bcmltm_md_lta_flags_t lta_flags;

    SHR_FUNC_ENTER(unit);

    /* [Key_Rev_Xfrm]: Process LTA key reverse transforms */
    xfrm_type = BCMLTM_FIELD_XFRM_TYPE_REV_KEY;
    lta_flags = BCMLTM_MD_LTA_FLAG_DEFAULTS;

    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fs_fa_cluster_create(unit, sid,
                                             BCMLTM_FIELD_SELECTION_ID_KEY,
                                             xfrm_type,
                                             lta_flags,
                                             opcode,
                                             node_side,
                                             parent_node,
                                             op_md_ptr,
                                             next_chain));
 exit:
    SHR_FUNC_EXIT();
}

