/*! \file bcmltm_md_lta_validate.c
 *
 * Logical Table Manager - LTA Field Validation.
 *
 * This file contains implementation to support metadata construction
 * for logical tables with Field Validation.
 *
 * FA Field Validation Cluster
 * ---------------------------
 *
 * The Field Validate cluster (for Keys and Values) is
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
 * Where "N" is the number of field validations for the given
 * field type.
 *
 * Each validation subtree is composed of a set of nodes.
 * For details, refer to diagram in corresponding function
 * that constructs the subtree.
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

#include <shr/shr_debug.h>

#include <bsl/bsl.h>

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_db_internal.h>
#include <bcmltm/bcmltm_fa_tree_nodes_internal.h>

#include "bcmltm_md_lt_drv.h"
#include "bcmltm_md_lta_validate.h"
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
 * \brief Create the FA subtree for Field Validation.
 *
 * Create the FA sub-tree for Field Validation.
 * This sub-tree is inserted in Left node of given parent node.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] field_type Field type.
 * \param [in] field_idx The index of validate field.
 * \param [in] opcode LT opcode.
 * \param [in] fa_node_parent Parent node for the field validation sub tree.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
validate_fa_node_tree_create(int unit,
                             bcmlrd_sid_t sid,
                             bcmltm_field_type_t field_type,
                             uint32_t field_idx,
                             bcmlt_opcode_t opcode,
                             bcmltm_node_t *fa_node_parent,
                             bcmltm_lt_op_md_t *op_md_ptr,
                             bcmltm_node_t **next_chain)
{
    bcmltm_node_t *parent_node = NULL;
    bcmltm_node_t *next_chain_node = NULL;
    /* #1 */
    bcmltm_fa_node_t *init_in_node_data = NULL;
    bcmltm_node_t    *init_in_node = NULL;
    /* #2 */
    const bcmltm_lta_field_list_t *copy_fields = NULL;
    bcmltm_fa_node_t *copy_node_data = NULL;
    bcmltm_node_t    *copy_node = NULL;
    bcmltm_node_t    *copy_node_root = NULL;
    uint32_t rev_xfrm_idx;
    uint32_t num_rev_xfrm = 0;
    int rev_xfrm_type = BCMLTM_DB_XFRM_TYPE_REV_VALUE_TRANSFORM;
    /* #3 */
    const bcmltm_lta_field_list_t *src_fields = NULL;
    bcmltm_fa_node_t *api_to_lta_node_data = NULL;
    bcmltm_node_t    *api_to_lta_node = NULL;
    bcmltm_fa_node_f api_to_lta_func = NULL;
    /* #4 */
    const bcmltm_lta_validate_params_t *lta_validate_params = NULL;
    bcmltm_fa_node_t *lta_validate_param_data = NULL;
    bcmltm_node_t    *lta_validate_param_node = NULL;

    SHR_FUNC_ENTER(unit);

    /*
     * FA Field Validation Sub Tree
     * ---------------------------------------------------------------
     *   Tree_0:                           [Parent]
     *                                        |
     *                                        |
     *                               +--------+-------+
     *                               |                |
     *                          [Left_Node]
     *                              #5
     *                               |
     *                 +-------------+----------------+
     *                 |                              |
     *            [Left_Node]                    [Right_Node]
     *                #1                             #4
     *                 |                              |
     *         +-------+------+           +-----------+-----------+
     *         |              |           |                       |
     *    [Left_Node]        ===     [Left_Node]             [Right_Node]
     *        #0                         #2                      #3
     *
     * #0. next chain node : chain to additional LTA Field Validation subtrees.
     * #1. bcmltm_fa_node_lta_input_init    : Init LTA src fields
     * #2. bcmltm_fa_node_lta_to_lta_fields : Copy LTA rev xfrm fields src
     * #3. bcmltm_fa_node_api_fields_to_lta : Copy LTM entry input fields to
     *                                        LTA(LTA-LTM).
     * #4. bcmltm_fa_node_lta_validate      : Invoke LTA valiate function.
     * #5. bcmltm_fa_node_lta_fields_to_wbc : See below NOTE section.
     * ---------------------------------------------------------------
     * NOTE:
     * #5 lta_fields_to_wbc node is currently NULL node because the API field is
     * copied into PTM by the api_to_schan node.
     */

    /**************************************************************************
     * #5. bcmltm_fa_node_lta_fields_to_wbc : See above NOTE section.
 */
    parent_node =
        bcmltm_tree_left_node_create(fa_node_parent,
                                     (void *)NULL,
                                     &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(parent_node, SHR_E_MEMORY);
    /**************************************************************************
     * #4. bcmltm_fa_node_lta_validate      : Invoke LTA validate function.
 */
    SHR_IF_ERR_EXIT
        (bcmltm_db_fv_validate_params_get(unit, sid, field_type,
                                          field_idx, &lta_validate_params));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit, bcmltm_fa_node_lta_validate,
                                       (void *)lta_validate_params,
                                       &lta_validate_param_data));
    lta_validate_param_node =
        bcmltm_tree_right_node_create(parent_node,
                                      (void *)lta_validate_param_data,
                                      &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(lta_validate_param_node, SHR_E_MEMORY);

    /**************************************************************************
     * #3. bcmltm_fa_node_api_fields_to_lta : Copy LTM entry input fields to
     *                                        LTA(LTA-LTM).
 */
    SHR_IF_ERR_EXIT
        (bcmltm_db_fv_src_fields_get(unit, sid, field_type,
                                     field_idx, &src_fields));
    if (opcode == BCMLT_OPCODE_UPDATE) {
        api_to_lta_func = bcmltm_fa_node_api_fields_update_to_lta;
    } else if (field_type == BCMLTM_FIELD_TYPE_KEY) {
        api_to_lta_func = bcmltm_fa_node_api_key_fields_to_lta;
    } else {
        api_to_lta_func = bcmltm_fa_node_api_value_fields_to_lta;
    }
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       api_to_lta_func,
                                       (void *)src_fields,
                                       &api_to_lta_node_data));
    api_to_lta_node =
        bcmltm_tree_right_node_create(lta_validate_param_node,
                                      (void *)api_to_lta_node_data,
                                      &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(api_to_lta_node, SHR_E_MEMORY);

    /**************************************************************************
     * #2. bcmltm_fa_node_lta_to_lta_fields : Sub-tree of list copies
 */
    /* We need to iterate over all _reverse_ transforms' out_fields lists
     * to see if any of our input fields are from those outputs.
     * They should be, since the reverse transforms should be
     * undoing the work of the forward transforms.
     * We do not presume that the forward and reverse transforms
     * are a 1-1 correspondence.  Only that all fields transformed
     * forward are transformed in reverse.  That is, the overall
     * collection of forward transforms result in the same set of fields
     * upon which the overall collection of reverse transform work.
     * But an individual forward transform does not correspond to
     * any particular subset of reverse transforms, even though this is
     * likely the common case.
     */
    if ((opcode == BCMLT_OPCODE_UPDATE) &&
        (field_type == BCMLTM_FIELD_TYPE_VALUE)) {
        SHR_IF_ERR_EXIT
            (bcmltm_db_xfrm_num_get(unit, sid,
                                    rev_xfrm_type, &num_rev_xfrm));

        if (num_rev_xfrm > 0) {
            /* Add Copy subtrees */
            copy_node_root = lta_validate_param_node;
            for (rev_xfrm_idx = 0;
                 rev_xfrm_idx < num_rev_xfrm;
                 rev_xfrm_idx++) {
                SHR_IF_ERR_EXIT
                    (bcmltm_db_fv_copy_fields_get(unit, sid,
                                                  field_type,
                                                  field_idx,
                                                  rev_xfrm_type,
                                                  rev_xfrm_idx,
                                                  &copy_fields));

                if (copy_fields == NULL) {
                    /* No field in this crossbar, skip. */
                    continue;
                }

                SHR_IF_ERR_EXIT
                    (bcmltm_md_fa_node_data_create(unit,
                                            bcmltm_fa_node_lta_to_lta_fields,
                                                   (void *)copy_fields,
                                                   &copy_node_data));
                copy_node =
                    bcmltm_tree_left_node_create(copy_node_root,
                                                 (void *)copy_node_data,
                                                 &(op_md_ptr->num_nodes));
                SHR_NULL_CHECK(copy_node, SHR_E_MEMORY);
                /* Build a tree downward as needed */
                copy_node_root = copy_node;
            }
        }
    }

     /**************************************************************************
     * #1. bcmltm_fa_node_lta_input_init    : Init LTA Input fields
 */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_input_init,
                                       (void *)src_fields,
                                       &init_in_node_data));
    init_in_node = bcmltm_tree_left_node_create(parent_node,
                                                 (void *)init_in_node_data,
                                                &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(init_in_node, SHR_E_MEMORY);

    /**************************************************************************
     * #0. empty node : chain to additional LTA validation subtrees.
 */
    next_chain_node = bcmltm_tree_left_node_create(init_in_node,
                                                   (void *)NULL,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(next_chain_node, SHR_E_MEMORY);

    *next_chain = next_chain_node;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        if (api_to_lta_node == NULL) {
            SHR_FREE(api_to_lta_node_data);
        }
        if (lta_validate_param_node == NULL) {
            SHR_FREE(lta_validate_param_data);
        }
        if (init_in_node == NULL) {
            SHR_FREE(init_in_node_data);
        }
        if (copy_node == NULL) {
            SHR_FREE(copy_node_data);
        }
    }

    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

int
bcmltm_md_validate_fa_trees_create(int unit,
                                   bcmlrd_sid_t sid,
                                   bcmltm_field_type_t field_type,
                                   bcmlt_opcode_t opcode,
                                   bcmltm_node_t *parent_chain,
                                   bcmltm_lt_op_md_t *op_md_ptr,
                                   bcmltm_node_t **next_chain)
{
    uint32_t idx;
    uint32_t num_validate = 0;
    bcmltm_node_t *parent_node = NULL;

    SHR_FUNC_ENTER(unit);

    *next_chain = parent_chain;

    SHR_IF_ERR_EXIT
        (bcmltm_db_fv_num_get(unit, sid, field_type, &num_validate));
    if (num_validate > 0) {
        /* Set parent node for Validate */
        parent_node = parent_chain;

        /* Add Validate subtrees */
        for (idx = 0; idx < num_validate; idx++) {
            SHR_IF_ERR_EXIT
                (validate_fa_node_tree_create(unit, sid,
                                              field_type, idx, opcode,
                                              parent_node, op_md_ptr,
                                              next_chain));
            parent_node = *next_chain;
        }
    }

 exit:
    SHR_FUNC_EXIT();
}

