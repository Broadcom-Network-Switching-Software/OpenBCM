/*! \file bcmltm_md_lta_validate_fs.c
 *
 * Logical Table Manager - LTA Field Validation with Field Selection.
 *
 * This file contains interfaces to construct the field validations
 * subtrees with field selections.
 *
 * FA Field Validation Cluster
 * ---------------------------
 *
 * The field validation cluster (for Keys and Values) is
 * composed of a number of subtrees formed as follows:
 *
 *                         [Parent]
 *                             |
 *                   +---------+---------+
 *                   |                   |
 *            [Validate_1 Subtree]      ===
 *                   |
 *          +--------+--------+
 *          |                 |
 *         ...               ===
 *   [Validate_N Subtree]
 *
 * Where "N" is the number of field validations for a given
 * validation type in a selection group.
 *
 * Each validation subtree is composed of a set of nodes required
 * to perform the validation flow.
 *
 * The order of execution in the cluster follow the same LTM
 * tree traverse order (i.e. post-order), which is:
 * Validate_N subtree, Validate_(N-1) subtree, ..., Validate_1 subtree.
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

#include "bcmltm_md_lta_validate_fs.h"
#include "bcmltm_md_op.h"

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Create the field validation FA subtree.
 *
 * This routine creates the field validation FA subtree
 * for the specified field selection ID, validation type, and index,
 * for a given table.
 *
 * The subtree is attached to the left node of given parent node.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] selection_id Field selection ID.
 * \param [in] val_type Field validation type.
 * \param [in] val_idx Field validation index.
 * \param [in] opcode LT opcode.
 * \param [in] parent_node Parent node for this subtree.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
validate_fs_fa_subtree_create(int unit,
                              bcmlrd_sid_t sid,
                              bcmltm_field_selection_id_t selection_id,
                              bcmltm_field_validate_type_t val_type,
                              uint32_t val_idx,
                              bcmlt_opcode_t opcode,
                              bcmltm_node_t *parent_node,
                              bcmltm_lt_op_md_t *op_md_ptr,
                              bcmltm_node_t **next_chain)
{
    bcmltm_node_t *next_chain_node = NULL;
    bcmltm_node_t *validate_node = NULL;
    bcmltm_node_t *in_init_node = NULL;
    bcmltm_node_t *field_c2v_node = NULL;
    bcmltm_fa_node_t *validate_nd = NULL;
    bcmltm_fa_node_t *in_init_nd = NULL;
    bcmltm_fa_node_t *field_c2v_nd = NULL;
    const bcmltm_lta_validate_params_t *params = NULL;
    const bcmltm_lta_select_field_list_t *in_fields = NULL;

    SHR_FUNC_ENTER(unit);

    /* [Validate]: Invoke validate function */
    SHR_IF_ERR_EXIT
        (bcmltm_db_validate_params_get(unit, sid, selection_id,
                                       val_type, val_idx,
                                       &params));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_validate,
                                       (void *)params,
                                       &validate_nd));
    validate_node = bcmltm_tree_left_node_create(parent_node,
                                                 (void *)validate_nd,
                                                 &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(validate_node, SHR_E_MEMORY);

    /* [In_Init]: Initialize LTA input fields */
    SHR_IF_ERR_EXIT
        (bcmltm_db_validate_in_fields_get(unit, sid, selection_id,
                                          val_type, val_idx,
                                          &in_fields));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_select_input_init,
                                       (void *)in_fields,
                                       &in_init_nd));
    in_init_node = bcmltm_tree_left_node_create(validate_node,
                                                (void *)in_init_nd,
                                                &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(in_init_node, SHR_E_MEMORY);

    /* [Field_C2V]: Link API Cache to LTA input fields */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_select_api_wb_to_lta,
                                       (void *)in_fields,
                                       &field_c2v_nd));
    field_c2v_node = bcmltm_tree_right_node_create(validate_node,
                                                   (void *)field_c2v_nd,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(field_c2v_node, SHR_E_MEMORY);

    /* [Next_Chain]: Chain to additional subtrees */
    next_chain_node = bcmltm_tree_left_node_create(in_init_node,
                                                   (void *)NULL,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(next_chain_node, SHR_E_MEMORY);

    *next_chain = next_chain_node;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy node data if it is not in node tree */
        if (validate_node == NULL) {
            SHR_FREE(validate_nd);
        }
        if (in_init_node == NULL) {
            SHR_FREE(in_init_nd);
        }
    }

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_md_validate_fs_fa_cluster_create(int unit,
                                 bcmlrd_sid_t sid,
                                 bcmltm_field_selection_id_t selection_id,
                                 bcmltm_field_validate_type_t val_type,
                                 bcmlt_opcode_t opcode,
                                 bcmltm_node_side_t node_side,
                                 bcmltm_node_t *parent_chain,
                                 bcmltm_lt_op_md_t *op_md_ptr,
                                 bcmltm_node_t **next_chain)
{
    uint32_t idx;
    uint32_t num_validations = 0;
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
        (bcmltm_db_validate_num_get(unit, sid, selection_id,
                                    val_type,
                                    &num_validations));
    if (num_validations == 0) {
        SHR_EXIT();
    }

    /* Add validate subtrees */
    for (idx = 0; idx < num_validations; idx++) {
        SHR_IF_ERR_EXIT
            (validate_fs_fa_subtree_create(unit, sid, selection_id,
                                           val_type, idx, opcode,
                                           parent_node, op_md_ptr,
                                           next_chain));
        parent_node = *next_chain;
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_md_validate_fs_key_cluster_create(int unit,
                                         bcmlrd_sid_t sid,
                                         bcmlt_opcode_t opcode,
                                         bcmltm_node_side_t node_side,
                                         bcmltm_node_t *parent_node,
                                         bcmltm_lt_op_md_t *op_md_ptr,
                                         bcmltm_node_t **next_chain)
{
    bcmltm_field_validate_type_t val_type;

    SHR_FUNC_ENTER(unit);

    /* [Key_Validate]: Process LTA key field validations */
    val_type = BCMLTM_FIELD_VALIDATE_TYPE_KEY;
    SHR_IF_ERR_EXIT
         (bcmltm_md_validate_fs_fa_cluster_create(unit, sid,
                                                  BCMLTM_FIELD_SELECTION_ID_KEY,
                                                  val_type,
                                                  opcode,
                                                  node_side,
                                                  parent_node,
                                                  op_md_ptr,
                                                  next_chain));
 exit:
    SHR_FUNC_EXIT();
}
