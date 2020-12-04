/*! \file bcmltm_md_lt_keyed.c
 *
 * Logical Table Manager - Keyed Logical Table (Hash and TCAM).
 *
 * This file contains implementation to support metadata construction
 * for Keyed, Hash and TCAM Logical Tables.
 *
 * Transforms and Validations
 * --------------------------
 * Field Transforms (Xfrm) and Field Validations clusters are
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
#define LT_OP_DELETE_NUM_ROOTS   1

/* Number of roots for UPDATE operation */
#define LT_OP_UPDATE_NUM_ROOTS   2

/* Number of roots for TRAVERSE operation */
#define LT_OP_TRAVERSE_NUM_ROOTS 2


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Create the FA tree roots for a INSERT operation.
 *
 * Create the FA tree roots for a INSERT operation for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_keyed_fa_node_roots_insert_create(int unit,
                                     bcmlrd_sid_t sid,
                                     bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *keys_node = NULL;
    bcmltm_node_t *values_node = NULL;
    bcmltm_node_t *fixed_key_node = NULL;
    bcmltm_node_t *def_values_node = NULL;
    bcmltm_node_t *fixed_value_node = NULL;
    bcmltm_node_t *ro_check_node = NULL;
    bcmltm_node_t *wide_field_node = NULL;
    bcmltm_node_t *next_chain = NULL;
    bcmltm_fa_node_t *keys_node_data = NULL;
    bcmltm_fa_node_t *values_node_data = NULL;
    bcmltm_fa_node_t *fixed_key_node_data = NULL;
    bcmltm_fa_node_t *ro_check_node_data = NULL;
    bcmltm_fa_node_t *wide_field_node_data = NULL;
    bcmltm_fa_node_t *def_values_node_data = NULL;
    bcmltm_fa_node_t *fixed_value_node_data = NULL;
    const bcmltm_field_select_mapping_t *keys_map = NULL;
    const bcmltm_field_select_mapping_t *values_map = NULL;
    const bcmltm_fixed_field_list_t *fixed_keys = NULL;
    const bcmltm_fixed_field_list_t *fixed_values = NULL;
    const bcmltm_ro_field_list_t *ro_list = NULL;
    const bcmltm_wide_field_list_t *wide_list = NULL;
    int xfrm_type;
    bcmltm_md_lta_fopt_t xfrm_lta_fopt = 0x0;
    bcmltm_field_type_t field_type;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, size, "bcmltmKeyedInsertFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, size);

    /*
     * INSERT
     *
     * FA Tree Roots
     * -----------------------------------------------------------------
     *   Tree_0:                     [Root:Keys]
     *                            (Convert API Keys)
     *                                    |
     *                                    |
     *                     +--------------+-------------+
     *                     |                            |
     *                [Xfrm Keys]                  [Fixed_Key]
     *                     |                     (Set Fixed fields)
     *                     |
     *             +-------+-------+
     *             |               |
     *      [Validate Keys]       ===
     *
     * -----------------------------------------------------------------
     *   Tree_1:                     [Root:Values]
     *                           (Convert API Values)
     *                                    |
     *                                    |
     *                         +----------+---------+
     *                         |                    |
     *                  [Xfrm Values]          [Fixed_Value]
     *                         |
     *                         |
     *                +--------+--------+
     *                |                 |
     *        [Validate Values]        ===
     *                |
     *                |
     *        +-------+-------+
     *        |               |
     *  [Default Values]      ===
     *  (Set table values to
     *   SW defaults from LRD)
     *            |
     *      +-----+-----+
     *      |           |
     *  [RO_Check]   [Wide_Value]
     *
     * -----------------------------------------------------------------
     * For more information on the Xfrm and Validate clusters,
     * please see description at the top of this file.
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Keys: Root
     *
     * Convert API Key fields into physical fields in Working Buffer.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_field_mapping_get(unit, sid,
                                        BCMLTM_FIELD_TYPE_KEY,
                                        BCMLTM_FIELD_DIR_IN,
                                        &keys_map));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_key_fields_to_wb,
                                       (void *)keys_map,
                                       &keys_node_data));
    keys_node = bcmltm_tree_node_create((void *)keys_node_data,
                                        &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(keys_node, SHR_E_MEMORY);
    fa_node_roots[0] = keys_node;

    /* Set node for next chain of Key transforms and validates */
    next_chain = keys_node;

    /*
     * Tree_0 - Xfrm Keys: Left node of Keys
     *
     * Process field transforms for Keys.
     */
    xfrm_type = BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_fwd_create(unit, sid,
                                            xfrm_type, xfrm_lta_fopt,
                                            BCMLT_OPCODE_INSERT,
                                            next_chain,
                                            op_md_ptr,
                                            &next_chain));

    /*
     * Tree_0 - Validate Keys: Left node of Xfrm Keys
     *
     * Process field validations for Keys.
     */
    field_type = BCMLTM_FIELD_TYPE_KEY;
    SHR_IF_ERR_EXIT
        (bcmltm_md_validate_fa_trees_create(unit, sid, field_type,
                                            BCMLT_OPCODE_INSERT,
                                            next_chain,
                                            op_md_ptr,
                                            &next_chain));

    /*
     * Tree_0 - Fixed Key Fields: Right node of Keys
     *
     * Set Fixed key fields.
     * If LT has not fixed fields, node data is NULL.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_fixed_keys_get(unit, sid, &fixed_keys));
    if ((fixed_keys != NULL) && (fixed_keys->num_fixed_fields > 0)) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_fixed_fields_to_wb,
                                           (void *)fixed_keys,
                                           &fixed_key_node_data));
    }
    fixed_key_node = bcmltm_tree_right_node_create(keys_node,
                                                   (void *)fixed_key_node_data,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(fixed_key_node, SHR_E_MEMORY);


    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Values: Root
     *
     * Convert API Value fields into physical fields in Working Buffer.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_field_mapping_get(unit, sid,
                                        BCMLTM_FIELD_TYPE_VALUE,
                                        BCMLTM_FIELD_DIR_IN,
                                        &values_map));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_value_fields_to_wb,
                                       (void *)values_map,
                                       &values_node_data));
    values_node = bcmltm_tree_node_create((void *)values_node_data,
                                          &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(values_node, SHR_E_MEMORY);
    fa_node_roots[1] = values_node;

    /* Set node for next chain of Value transforms and validates */
    next_chain = values_node;

    /*
     * Tree_1 - Xfrm Values: Left node of Values
     *
     * Process field transforms for Values.
     */
    xfrm_type = BCMLTM_DB_XFRM_TYPE_FWD_VALUE_TRANSFORM;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_fwd_create(unit, sid,
                                            xfrm_type, xfrm_lta_fopt,
                                            BCMLT_OPCODE_INSERT,
                                            next_chain,
                                            op_md_ptr,
                                            &next_chain));

    /*
     * Tree_1 - Validate Values: Left node of Xfrm Values
     *
     * Process field validations for Values.
     */
    field_type = BCMLTM_FIELD_TYPE_VALUE;
    SHR_IF_ERR_EXIT
        (bcmltm_md_validate_fa_trees_create(unit, sid, field_type,
                                            BCMLT_OPCODE_INSERT,
                                            next_chain,
                                            op_md_ptr,
                                            &next_chain));

    /*
     * Tree_1 - Default Values: Left node of Validate Values
     *
     * Set table values to SW defaults from LRD.
     *
     * Note that this node could be attached directly to Values
     * or to the Xfrm Values node depending on whether the table has
     * any value field transforms or validations.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_default_fields_to_wb,
                                       (void *)values_map,
                                       &def_values_node_data));
    def_values_node = bcmltm_tree_left_node_create(next_chain,
                                            (void *)def_values_node_data,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(def_values_node, SHR_E_MEMORY);

    /*
     * Tree_1 - Fixed Value Fields: Right node of Values
     *
     * Set Fixed value fields.
     * If LT has no fixed fields, node data is NULL.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_fixed_values_get(unit, sid, &fixed_values));
    if ((fixed_values != NULL) && (fixed_values->num_fixed_fields > 0)) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_fixed_fields_to_wb,
                                           (void *)fixed_values,
                                           &fixed_value_node_data));
    }
    fixed_value_node = bcmltm_tree_right_node_create(values_node,
                                             (void *)fixed_value_node_data,
                                                     &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(fixed_value_node, SHR_E_MEMORY);

    /* [RO_Check]: Verify no read-only fields present in input */
    SHR_IF_ERR_EXIT
        (bcmltm_db_read_only_list_get(unit, sid, &ro_list));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_read_only_input_check,
                                       (void *)ro_list,
                                       &ro_check_node_data));
    ro_check_node = bcmltm_tree_left_node_create(def_values_node,
                                                 (void *)ro_check_node_data,
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
                                       &wide_field_node_data));
    wide_field_node = bcmltm_tree_right_node_create(def_values_node,
                                                 (void *)wide_field_node_data,
                                                 &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(wide_field_node, SHR_E_MEMORY);

    /* Set returning pointer */
    op_md_ptr->fa_node_roots = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        if (keys_node == NULL) {
            SHR_FREE(keys_node_data);
        }
        if (fixed_key_node == NULL) {
            SHR_FREE(fixed_key_node_data);
        }
        if (values_node == NULL) {
            SHR_FREE(values_node_data);
        }
        if (def_values_node == NULL) {
            SHR_FREE(def_values_node_data);
        }
        if (fixed_value_node == NULL) {
            SHR_FREE(fixed_value_node_data);
        }
        if (ro_check_node == NULL) {
            SHR_FREE(ro_check_node_data);
        }
        if (wide_field_node == NULL) {
            SHR_FREE(wide_field_node_data);
        }

        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, fa_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->fa_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the EE tree roots for a INSERT operation.
 *
 * Create the EE tree roots for a INSERT operation for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_keyed_ee_node_roots_insert_create(int unit,
                                     bcmlrd_sid_t sid,
                                     bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *lookup_node = NULL;
    bcmltm_node_t *insert_node = NULL;
    bcmltm_ee_node_t *lookup_node_data = NULL;
    bcmltm_ee_node_t *insert_node_data = NULL;
    const bcmltm_pt_keyed_op_info_t *lookup_info = NULL;
    const bcmltm_pt_keyed_op_info_t *insert_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, size, "bcmltmKeyedInsertEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /*
     * INSERT
     *
     * EE Trees Roots
     * -----------------------------------------------------------------
     *   Tree_0:          [Root:Lookup]
     *                (PTM lookup operation)
     *
     * -----------------------------------------------------------------
     *   Tree_1:          [Root:Insert]
     *                (PTM insert operation)
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Lookup: Root
     *
     * PTM lookup operation (entry should not exist).
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_pt_keyed_get(unit, sid, BCMPTM_OP_LOOKUP,
                                   &lookup_info));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create
            (unit,
             bcmltm_ee_node_model_keyed_lookup_not_found,
             (void *)lookup_info, &lookup_node_data));
    lookup_node = bcmltm_tree_node_create((void *)lookup_node_data,
                                          &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(lookup_node, SHR_E_MEMORY);
    ee_node_roots[0] = lookup_node;

    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Insert: Root
     *
     * Execute PTM insert operation.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_pt_keyed_get(unit, sid, BCMPTM_OP_INSERT,
                                   &insert_info));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_model_keyed_insert,
                                       (void *)insert_info,
                                       &insert_node_data));
    insert_node = bcmltm_tree_node_create((void *)insert_node_data,
                                          &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(insert_node, SHR_E_MEMORY);
    ee_node_roots[1] = insert_node;

    /* Set returning pointer */
    op_md_ptr->ee_node_roots = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the ee_node (node data)
         * if corresponding tree creation was not successful.
         */
        if (lookup_node == NULL) {
            SHR_FREE(lookup_node_data);
        }
        if (insert_node == NULL) {
            SHR_FREE(insert_node_data);
        }

        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, ee_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->ee_node_roots = NULL;
    }

    SHR_FUNC_EXIT();

}


/*!
 * \brief Create operation metadata for the INSERT opcode.
 *
 * Create the operation metadata for given table ID to use for the INSERT
 * opcode.
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
lt_keyed_op_insert_create(int unit, bcmlrd_sid_t sid,
                          bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get working buffer size */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "bcmltmKeyedInsertLtOpMd");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_INSERT_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lt_keyed_fa_node_roots_insert_create(unit, sid, op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_keyed_ee_node_roots_insert_create(unit, sid, op_md));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(op_md);
        *op_md_ptr = NULL;
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the FA tree roots for a LOOKUP operation.
 *
 * Create the FA tree roots for a LOOKUP operation for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_keyed_fa_node_roots_lookup_create(int unit,
                                     bcmlrd_sid_t sid,
                                     bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *keys_node = NULL;
    bcmltm_node_t *values_node = NULL;
    bcmltm_node_t *fixed_key_node = NULL;
    bcmltm_node_t *next_chain = NULL;
    bcmltm_fa_node_t *keys_node_data = NULL;
    bcmltm_fa_node_t *values_node_data = NULL;
    bcmltm_fa_node_t *fixed_key_node_data = NULL;
    const bcmltm_field_select_mapping_t *keys_map = NULL;
    const bcmltm_field_select_mapping_t *values_map = NULL;
    const bcmltm_fixed_field_list_t *fixed_keys = NULL;
    int xfrm_type;
    bcmltm_md_lta_fopt_t xfrm_lta_fopt = 0x0;
    bcmltm_field_type_t field_type;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, size, "bcmltmKeyedLookupFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, size);

    /*
     * LOOKUP
     *
     * FA Tree Roots
     * -----------------------------------------------------------------
     *   Tree_0:                     [Root:Keys]
     *                            (Convert API Keys)
     *                                    |
     *                                    |
     *                     +--------------+-------------+
     *                     |                            |
     *                [Xfrm Keys]                  [Fixed_Keys]
     *                     |                     (Set Fixed fields)
     *                     |
     *             +-------+-------+
     *             |               |
     *      [Validate Keys]       ===
     *
     * -----------------------------------------------------------------
     *   Tree_1:                     [Root:Values]
     *                         (Convert HW fields to return
     *                          API Values fields list)
     *                                    |
     *                                    |
     *                         +----------+---------+
     *                         |                    |
     *                  [Xfrm Values]              ===
     *
     * -----------------------------------------------------------------
     * For more information on the Xfrm and Validate clusters,
     * please see description at the top of this file.
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Keys: Root
     *
     * Convert API Key fields into physical fields in Working Buffer.
     * Keys are needed by the PTM lookup operation.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_field_mapping_get(unit, sid,
                                        BCMLTM_FIELD_TYPE_KEY,
                                        BCMLTM_FIELD_DIR_IN,
                                        &keys_map));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_key_fields_to_wb,
                                       (void *)keys_map,
                                       &keys_node_data));
    keys_node = bcmltm_tree_node_create((void *)keys_node_data,
                                        &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(keys_node, SHR_E_MEMORY);
    fa_node_roots[0] = keys_node;

    /* Set node for next chain of Key transforms and validates */
    next_chain = keys_node;

    /*
     * Tree_0 - Xfrm Keys: Left node of Keys
     *
     * Process field transforms for Keys.
     */
    xfrm_type = BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_fwd_create(unit, sid,
                                            xfrm_type, xfrm_lta_fopt,
                                            BCMLT_OPCODE_LOOKUP,
                                            next_chain,
                                            op_md_ptr,
                                            &next_chain));

    /*
     * Tree_0 - Validate Keys: Left node of Xfrm Keys
     *
     * Process field validations for Keys.
     */
    field_type = BCMLTM_FIELD_TYPE_KEY;
    SHR_IF_ERR_EXIT
        (bcmltm_md_validate_fa_trees_create(unit, sid, field_type,
                                            BCMLT_OPCODE_LOOKUP,
                                            next_chain,
                                            op_md_ptr,
                                            &next_chain));

    /*
     * Tree_0 - Fixed Key Fields: Right node of Keys
     *
     * Set Fixed key fields.
     * If LT has not fixed fields, node data is NULL.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_fixed_keys_get(unit, sid, &fixed_keys));
    if ((fixed_keys != NULL) && (fixed_keys->num_fixed_fields > 0)) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_fixed_fields_to_wb,
                                           (void *)fixed_keys,
                                           &fixed_key_node_data));
    }
    fixed_key_node = bcmltm_tree_right_node_create(keys_node,
                                                   (void *)fixed_key_node_data,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(fixed_key_node, SHR_E_MEMORY);


    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Values: Root
     *
     * Convert the HW information read from the EE stage into
     * the returning list of API Value fields.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_field_mapping_get(unit, sid,
                                        BCMLTM_FIELD_TYPE_VALUE,
                                        BCMLTM_FIELD_DIR_OUT,
                                        &values_map));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_wb_to_api_fields,
                                       (void *)values_map,
                                       &values_node_data));
    values_node = bcmltm_tree_node_create((void *)values_node_data,
                                          &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(values_node, SHR_E_MEMORY);
    fa_node_roots[1] = values_node;

    /* Set node for next chain of Value transforms and validates */
    next_chain = values_node;

    /*
     * Tree_1 - Xfrm Values: Left node of Values
     *
     * Process field transforms for Values.
     */
    xfrm_type = BCMLTM_DB_XFRM_TYPE_REV_VALUE_TRANSFORM;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_rev_create(unit, sid,
                                            xfrm_type,
                                            BCMLT_OPCODE_LOOKUP,
                                            next_chain,
                                            op_md_ptr,
                                            &next_chain));

    /* Set returning pointer */
    op_md_ptr->fa_node_roots = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        if (keys_node == NULL) {
            SHR_FREE(keys_node_data);
        }
        if (fixed_key_node == NULL) {
            SHR_FREE(fixed_key_node_data);
        }
        if (values_node == NULL) {
            SHR_FREE(values_node_data);
        }

        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, fa_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->fa_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the EE tree roots for a LOOKUP operation.
 *
 * Create the EE tree roots for a LOOKUP operation for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_keyed_ee_node_roots_lookup_create(int unit,
                                     bcmlrd_sid_t sid,
                                     bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *lookup_node = NULL;
    bcmltm_ee_node_t *lookup_node_data = NULL;
    const bcmltm_pt_keyed_op_info_t *lookup_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, size, "bcmltmKeyedLookupEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /*
     * LOOKUP
     *
     * EE Trees Roots
     * -----------------------------------------------------------------
     *   Tree_0:          [Root:Lookup]
     *                (PTM lookup operation)
     *
     * -----------------------------------------------------------------
     *   Tree_1:          NULL
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Lookup: Root
     *
     * Execute PTM lookup operation to get HW data.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_pt_keyed_get(unit, sid, BCMPTM_OP_LOOKUP,
                                   &lookup_info));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_model_keyed_lookup,
                                       (void *)lookup_info,
                                       &lookup_node_data));
    lookup_node = bcmltm_tree_node_create((void *)lookup_node_data,
                                          &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(lookup_node, SHR_E_MEMORY);
    ee_node_roots[0] = lookup_node;

    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Root
     * No actions.
     */
    ee_node_roots[1] = NULL;  /* Not used */

    /* Set returning pointer */
    op_md_ptr->ee_node_roots = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the ee_node (node data)
         * if corresponding tree creation was not successful.
         */
        if (lookup_node == NULL) {
            SHR_FREE(lookup_node_data);
        }

        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, ee_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->ee_node_roots = NULL;
    }

    SHR_FUNC_EXIT();

}


/*!
 * \brief Create operation metadata for the LOOKUP opcode.
 *
 * Create the operation metadata for given table ID to use for the LOOKUP
 * opcode.
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
lt_keyed_op_lookup_create(int unit, bcmlrd_sid_t sid,
                          bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get working buffer size */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "bcmltmKeyedLookupLtOpMd");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_LOOKUP_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lt_keyed_fa_node_roots_lookup_create(unit, sid, op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_keyed_ee_node_roots_lookup_create(unit, sid, op_md));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(op_md);
        *op_md_ptr = NULL;
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the FA tree roots for a DELETE operation.
 *
 * Create the FA tree roots for a DELETE operation for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_keyed_fa_node_roots_delete_create(int unit,
                                     bcmlrd_sid_t sid,
                                     bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *keys_node = NULL;
    bcmltm_node_t *fixed_key_node = NULL;
    bcmltm_node_t *next_chain = NULL;
    bcmltm_fa_node_t *keys_node_data = NULL;
    bcmltm_fa_node_t *fixed_key_node_data = NULL;
    const bcmltm_field_select_mapping_t *keys_map = NULL;
    const bcmltm_fixed_field_list_t *fixed_keys = NULL;
    int xfrm_type;
    bcmltm_md_lta_fopt_t xfrm_lta_fopt = 0x0;
    bcmltm_field_type_t field_type;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, size, "bcmltmKeyedDeleteFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, size);

    /*
     * DELETE
     *
     * FA Tree Roots
     * -----------------------------------------------------------------
     *   Tree_0:                     [Root:Keys]
     *                            (Convert API Keys)
     *                                    |
     *                                    |
     *                     +--------------+-------------+
     *                     |                            |
     *                [Xfrm Keys]                  [Fixed_Key]
     *                     |                     (Set Fixed fields)
     *                     |
     *             +-------+-------+
     *             |               |
     *      [Validate Keys]       ===
 */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Keys: Root
     *
     * Convert API Key fields into physical fields in Working Buffer.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_field_mapping_get(unit, sid,
                                        BCMLTM_FIELD_TYPE_KEY,
                                        BCMLTM_FIELD_DIR_IN,
                                        &keys_map));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_key_fields_to_wb,
                                       (void *)keys_map,
                                       &keys_node_data));
    keys_node = bcmltm_tree_node_create((void *)keys_node_data,
                                        &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(keys_node, SHR_E_MEMORY);
    fa_node_roots[0] = keys_node;

    /* Set node for next chain of Key transforms and validates */
    next_chain = keys_node;

    /*
     * Tree_0 - Xfrm Keys: Left node of Keys
     *
     * Process field transforms for Keys.
     */
    xfrm_type = BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_fwd_create(unit, sid,
                                            xfrm_type, xfrm_lta_fopt,
                                            BCMLT_OPCODE_DELETE,
                                            next_chain,
                                            op_md_ptr,
                                            &next_chain));

    /*
     * Tree_0 - Validate Keys: Left node of Xfrm Keys
     *
     * Process field validations for Keys.
     */
    field_type = BCMLTM_FIELD_TYPE_KEY;
    SHR_IF_ERR_EXIT
        (bcmltm_md_validate_fa_trees_create(unit, sid, field_type,
                                            BCMLT_OPCODE_DELETE,
                                            next_chain,
                                            op_md_ptr,
                                            &next_chain));

    /*
     * Tree_0 - Fixed Key Fields: Right node of Keys
     *
     * Set Fixed key fields.
     * If LT has not fixed fields, node data is NULL.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_fixed_keys_get(unit, sid, &fixed_keys));
    if ((fixed_keys != NULL) && (fixed_keys->num_fixed_fields > 0)) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_fixed_fields_to_wb,
                                           (void *)fixed_keys,
                                           &fixed_key_node_data));
    }
    fixed_key_node = bcmltm_tree_right_node_create(keys_node,
                                                   (void *)fixed_key_node_data,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(fixed_key_node, SHR_E_MEMORY);


    /* Set returning pointer */
    op_md_ptr->fa_node_roots = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        if (keys_node == NULL) {
            SHR_FREE(keys_node_data);
        }
        if (fixed_key_node == NULL) {
            SHR_FREE(fixed_key_node_data);
        }

        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, fa_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->fa_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the EE tree roots for a DELETE operation.
 *
 * Create the EE tree roots for a DELETE operation for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_keyed_ee_node_roots_delete_create(int unit,
                                     bcmlrd_sid_t sid,
                                     bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *delete_node = NULL;
    bcmltm_ee_node_t *delete_node_data = NULL;
    const bcmltm_pt_keyed_op_info_t *delete_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, size, "bcmltmKeyedDeleteEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /*
     * DELETE
     *
     * EE Tree Roots
     * -----------------------------------------------------------------
     *   Tree_0:           [Root:Delete]
     *                 (PTM delete operation)
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Delete: Root
     *
     * Execute PTM delete operation.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_pt_keyed_get(unit, sid, BCMPTM_OP_DELETE,
                                   &delete_info));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_model_keyed_delete,
                                       (void *)delete_info,
                                       &delete_node_data));
    delete_node = bcmltm_tree_node_create((void *)delete_node_data,
                                          &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(delete_node, SHR_E_MEMORY);
    ee_node_roots[0] = delete_node;

    /* Set returning pointer */
    op_md_ptr->ee_node_roots = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the ee_node (node data)
         * if corresponding tree creation was not successful.
         */
        if (delete_node == NULL) {
            SHR_FREE(delete_node_data);
        }

        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, ee_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->ee_node_roots = NULL;
    }

    SHR_FUNC_EXIT();

}


/*!
 * \brief Create operation metadata for the DELETE opcode.
 *
 * Create the operation metadata for given table ID to use for the DELETE
 * opcode.
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
lt_keyed_op_delete_create(int unit, bcmlrd_sid_t sid,
                          bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get working buffer size */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "bcmltmKeyedDeleteLtOpMd");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_DELETE_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lt_keyed_fa_node_roots_delete_create(unit, sid, op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_keyed_ee_node_roots_delete_create(unit, sid, op_md));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(op_md);
        *op_md_ptr = NULL;
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the FA tree roots for a UPDATE operation.
 *
 * Create the FA tree roots for a UPDATE operation for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_keyed_fa_node_roots_update_create(int unit,
                                     bcmlrd_sid_t sid,
                                     bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *keys_node = NULL;
    bcmltm_node_t *values_node = NULL;
    bcmltm_node_t *fixed_key_node = NULL;
    bcmltm_node_t *ro_check_node = NULL;
    bcmltm_node_t *wide_field_node = NULL;
    bcmltm_node_t *next_chain = NULL;
    bcmltm_fa_node_t *keys_node_data = NULL;
    bcmltm_fa_node_t *values_node_data = NULL;
    bcmltm_fa_node_t *fixed_key_node_data = NULL;
    bcmltm_fa_node_t *ro_check_node_data = NULL;
    bcmltm_fa_node_t *wide_field_node_data = NULL;
    const bcmltm_field_select_mapping_t *keys_map = NULL;
    const bcmltm_field_select_mapping_t *values_map = NULL;
    const bcmltm_fixed_field_list_t *fixed_keys = NULL;
    const bcmltm_ro_field_list_t *ro_list = NULL;
    const bcmltm_wide_field_list_t *wide_list = NULL;
    int xfrm_type;
    bcmltm_md_lta_fopt_t xfrm_lta_fopt = 0x0;
    bcmltm_field_type_t field_type;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, size, "bcmltmKeyedUpdateFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, size);

    /*
     * UPDATE
     *
     * FA Tree Roots
     * -----------------------------------------------------------------
     *   Tree_0:                     [Root:Keys]
     *                            (Convert API Keys)
     *                                    |
     *                                    |
     *                     +--------------+-------------+
     *                     |                            |
     *                [Xfrm Keys]                  [Fixed_Key]
     *                     |                     (Set Fixed fields)
     *                     |
     *             +-------+-------+
     *             |               |
     *      [Validate Keys]       ===
     *
     * -----------------------------------------------------------------
     *   Tree_1:                     [Root:Values]
     *                           (Update API Values)
     *                                    |
     *                                    |
     *                         +----------+---------+
     *                         |                    |
     *                  [Xfrm Values]              ===
     *                         |
     *                         |
     *                +--------+--------+
     *                |                 |
     *        [Validate Values]        ===
     *                |
     *                |
     *        +-------+-------+
     *        |               |
     * [Rev Xfrm Values]     ===
     *             |
     *       +-----+-----+
     *       |           |
     *   [RO_Check]   [Wide_Value]
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Keys: Root
     *
     * Convert API Key fields into physical fields in Working Buffer.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_field_mapping_get(unit, sid,
                                        BCMLTM_FIELD_TYPE_KEY,
                                        BCMLTM_FIELD_DIR_IN,
                                        &keys_map));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_key_fields_to_wb,
                                       (void *)keys_map,
                                       &keys_node_data));
    keys_node = bcmltm_tree_node_create((void *)keys_node_data,
                                        &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(keys_node, SHR_E_MEMORY);
    fa_node_roots[0] = keys_node;


    /* Set node for next chain of Key transforms and validates */
    next_chain = keys_node;

    /*
     * Tree_0 - Xfrm Keys: Left node of Keys
     *
     * Process field transforms for Keys.
     */
    xfrm_type = BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_fwd_create(unit, sid,
                                            xfrm_type, xfrm_lta_fopt,
                                            BCMLT_OPCODE_UPDATE,
                                            next_chain,
                                            op_md_ptr,
                                            &next_chain));

    /*
     * Tree_0 - Validate Keys: Left node of Xfrm Keys
     *
     * Process field validations for Keys.
     */
    field_type = BCMLTM_FIELD_TYPE_KEY;
    SHR_IF_ERR_EXIT
        (bcmltm_md_validate_fa_trees_create(unit, sid, field_type,
                                            BCMLT_OPCODE_UPDATE,
                                            next_chain,
                                            op_md_ptr,
                                            &next_chain));

    /*
     * Tree_0 - Fixed Key Fields: Right node of Keys
     *
     * Set Fixed key fields.
     * If LT has not fixed fields, node data is NULL.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_fixed_keys_get(unit, sid, &fixed_keys));
    if ((fixed_keys != NULL) && (fixed_keys->num_fixed_fields > 0)) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_fixed_fields_to_wb,
                                           (void *)fixed_keys,
                                           &fixed_key_node_data));
    }
    fixed_key_node = bcmltm_tree_right_node_create(keys_node,
                                                   (void *)fixed_key_node_data,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(fixed_key_node, SHR_E_MEMORY);


    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Values: Root
     *
     * Update API Value fields into physical fields in Working Buffer.
     * Unset marked API Value fields (revert to default) for UPDATE.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_field_mapping_get(unit, sid,
                                        BCMLTM_FIELD_TYPE_VALUE,
                                        BCMLTM_FIELD_DIR_IN,
                                        &values_map));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_fields_unset_to_wb,
                                       (void *)values_map,
                                       &values_node_data));
    values_node = bcmltm_tree_node_create((void *)values_node_data,
                                          &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(values_node, SHR_E_MEMORY);
    fa_node_roots[1] = values_node;

    /* Set node for next chain of Value transforms and validates */
    next_chain = values_node;

    /*
     * Tree_1 - Xfrm Values: Left node of Values
     *
     * Process field transforms for Values.
     */
    xfrm_type = BCMLTM_DB_XFRM_TYPE_FWD_VALUE_TRANSFORM;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_fwd_create(unit, sid,
                                            xfrm_type, xfrm_lta_fopt,
                                            BCMLT_OPCODE_UPDATE,
                                            next_chain,
                                            op_md_ptr,
                                            &next_chain));

    /*
     * Tree_1 - Validate Values: Left node of Xfrm Values
     *
     * Process field validations for Values.
     */
    field_type = BCMLTM_FIELD_TYPE_VALUE;
    SHR_IF_ERR_EXIT
        (bcmltm_md_validate_fa_trees_create(unit, sid, field_type,
                                            BCMLT_OPCODE_UPDATE,
                                            next_chain,
                                            op_md_ptr,
                                            &next_chain));

    /*
     * Tree_1 - Rev Xfrm Values: Left node of Validate Values
     *
     * Process reverse field transforms for Values read from PTM.
     */
    xfrm_type = BCMLTM_DB_XFRM_TYPE_REV_VALUE_TRANSFORM;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_rev_create(unit, sid,
                                            xfrm_type,
                                            BCMLT_OPCODE_UPDATE,
                                            next_chain,
                                            op_md_ptr,
                                            &next_chain));

    /* [RO_Check]: Verify no read-only fields present in input */
    SHR_IF_ERR_EXIT
        (bcmltm_db_read_only_list_get(unit, sid, &ro_list));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_read_only_input_check,
                                       (void *)ro_list,
                                       &ro_check_node_data));
    ro_check_node = bcmltm_tree_left_node_create(next_chain,
                                                 (void *)ro_check_node_data,
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
                                       &wide_field_node_data));
    wide_field_node = bcmltm_tree_right_node_create(next_chain,
                                                 (void *)wide_field_node_data,
                                                 &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(wide_field_node, SHR_E_MEMORY);

    /* Set returning pointer */
    op_md_ptr->fa_node_roots = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        if (keys_node == NULL) {
            SHR_FREE(keys_node_data);
        }
        if (fixed_key_node == NULL) {
            SHR_FREE(fixed_key_node_data);
        }
        if (values_node == NULL) {
            SHR_FREE(values_node_data);
        }
        if (ro_check_node == NULL) {
            SHR_FREE(ro_check_node_data);
        }
        if (wide_field_node == NULL) {
            SHR_FREE(wide_field_node_data);
        }

        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, fa_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->fa_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the EE tree roots for a UPDATE operation.
 *
 * Create the EE tree roots for a UPDATE operation for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_keyed_ee_node_roots_update_create(int unit,
                                     bcmlrd_sid_t sid,
                                     bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *lookup_node = NULL;
    bcmltm_node_t *insert_node = NULL;
    bcmltm_ee_node_t *lookup_node_data = NULL;
    bcmltm_ee_node_t *insert_node_data = NULL;
    const bcmltm_pt_keyed_op_info_t *lookup_info = NULL;
    const bcmltm_pt_keyed_op_info_t *insert_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, size, "bcmltmKeyedUpdateEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /*
     * UPDATE
     *
     * EE Tree Roots
     * -----------------------------------------------------------------
     *   Tree_0:           [Root:Lookup]
     *                (PTM lookup operation)
     *
     * -----------------------------------------------------------------
     *   Tree_1:           [Root:Insert]
     *                (PTM insert operation)
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Lookup: Root
     *
     * Execute PTM lookup operation to get HW data.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_pt_keyed_get(unit, sid, BCMPTM_OP_LOOKUP,
                                   &lookup_info));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_model_keyed_lookup,
                                       (void *)lookup_info,
                                       &lookup_node_data));
    lookup_node = bcmltm_tree_node_create((void *)lookup_node_data,
                                          &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(lookup_node, SHR_E_MEMORY);
    ee_node_roots[0] = lookup_node;


    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Insert: Root
     *
     * Execute PTM insert operation.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_pt_keyed_get(unit, sid, BCMPTM_OP_INSERT,
                                   &insert_info));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_model_keyed_insert,
                                       (void *)insert_info,
                                       &insert_node_data));
    insert_node = bcmltm_tree_node_create((void *)insert_node_data,
                                          &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(insert_node, SHR_E_MEMORY);
    ee_node_roots[1] = insert_node;

    /* Set returning pointer */
    op_md_ptr->ee_node_roots = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the ee_node (node data)
         * if corresponding tree creation was not successful.
         */
        if (lookup_node == NULL) {
            SHR_FREE(lookup_node_data);
        }
        if (insert_node == NULL) {
            SHR_FREE(insert_node_data);
        }

        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, ee_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->ee_node_roots = NULL;
    }

    SHR_FUNC_EXIT();

}


/*!
 * \brief Create operation metadata for the UPDATE opcode.
 *
 * Create the operation metadata for given table ID to use for the UPDATE
 * opcode.
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
lt_keyed_op_update_create(int unit, bcmlrd_sid_t sid,
                          bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get working buffer size */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "bcmltmKeyedUpdateLtOpMd");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_UPDATE_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lt_keyed_fa_node_roots_update_create(unit, sid, op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_keyed_ee_node_roots_update_create(unit, sid, op_md));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(op_md);
        *op_md_ptr = NULL;
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the FA tree roots for a TRAVERSE operation.
 *
 * Create the FA tree roots for a TRAVERSE operation for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_keyed_fa_node_roots_traverse_create(int unit,
                                       bcmlrd_sid_t sid,
                                       bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *prev_keys_node = NULL;
    bcmltm_node_t *keys_node = NULL;
    bcmltm_node_t *values_node = NULL;
    bcmltm_node_t *fixed_key_node = NULL;
    bcmltm_node_t *next_chain = NULL;
    bcmltm_fa_node_t *prev_keys_node_data = NULL;
    bcmltm_fa_node_t *keys_node_data = NULL;
    bcmltm_fa_node_t *values_node_data = NULL;
    bcmltm_fa_node_t *fixed_key_node_data = NULL;
    const bcmltm_field_select_mapping_t *keys_map_in = NULL;
    const bcmltm_field_select_mapping_t *keys_map_out = NULL;
    const bcmltm_field_select_mapping_t *values_map = NULL;
    const bcmltm_fixed_field_list_t *fixed_keys = NULL;
    int xfrm_type;
    bcmltm_md_lta_fopt_t xfrm_lta_fopt = 0x0;
    bcmltm_field_type_t field_type;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, size, "bcmltmKeyedTraverseFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, size);

    /*
     * TRAVERSE
     *
     * FA Tree Roots
     * -----------------------------------------------------------------
     *   Tree_0:                     [Root:Prev Keys]
     *                             (Convert API Keys)
     *                                    |
     *                                    |
     *                     +--------------+-------------+
     *                     |                            |
     *                [Xfrm Keys]                  [Fixed_Key]
     *                     |                     (Set Fixed fields)
     *                     |
     *             +-------+-------+
     *             |               |
     *      [Validate Keys]       ===
     *
     * -----------------------------------------------------------------
     *   Tree_1:                     [Root:Keys]
     *                         (Convert HW fields to return
     *                          API Keys fields list)
     *                                    |
     *                                    |
     *                         +----------+---------+
     *                         |                    |
     *                  [Xfrm Values]            [Values]
     *                         |          (Convert HW fields to return
     *                         |            API Values fields list)
     *                 +-------+-------+
     *                 |               |
     *            [Xfrm Keys]         ===
     *
     * -----------------------------------------------------------------
     * For more information on the Xfrm and Validate clusters,
     * please see description at the top of this file.
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Prev Keys: Root
     *
     * Convert API Key fields into physical fields in Working Buffer.
     * Keys are needed by the PTM operation to get the next entry.
     *
     * Note that on first traverse (start), there are no input keys.
     * In this case, the operation returns the first entry.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_field_mapping_get(unit, sid,
                                        BCMLTM_FIELD_TYPE_KEY,
                                        BCMLTM_FIELD_DIR_IN,
                                        &keys_map_in));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_key_fields_to_wb,
                                       (void *)keys_map_in,
                                       &prev_keys_node_data));
    prev_keys_node =
        bcmltm_tree_node_create((void *)prev_keys_node_data,
                                &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(prev_keys_node, SHR_E_MEMORY);
    fa_node_roots[0] = prev_keys_node;

    /* Set node for next chain of Key transforms and validates */
    next_chain = prev_keys_node;

    /*
     * Tree_0 - Xfrm Keys: Left node of Prev Keys
     *
     * Process field transforms for Keys.
     */
    xfrm_type = BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_fwd_create(unit, sid,
                                            xfrm_type, xfrm_lta_fopt,
                                            BCMLT_OPCODE_TRAVERSE,
                                            next_chain,
                                            op_md_ptr,
                                            &next_chain));

    /*
     * Tree_0 - Validate Keys: Left node of Xfrm Keys
     *
     * Process field validations for Keys.
     */
    field_type = BCMLTM_FIELD_TYPE_KEY;
    SHR_IF_ERR_EXIT
        (bcmltm_md_validate_fa_trees_create(unit, sid, field_type,
                                            BCMLT_OPCODE_TRAVERSE,
                                            next_chain,
                                            op_md_ptr,
                                            &next_chain));

    /*
     * Tree_0 - Fixed Key Fields: Right node of Prev Keys
     *
     * Set Fixed key fields.
     * If LT has not fixed fields, node data is NULL.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_fixed_keys_get(unit, sid, &fixed_keys));
    if ((fixed_keys != NULL) && (fixed_keys->num_fixed_fields > 0)) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_fixed_fields_to_wb,
                                           (void *)fixed_keys,
                                           &fixed_key_node_data));
    }
    fixed_key_node = bcmltm_tree_right_node_create(prev_keys_node,
                                                   (void *)fixed_key_node_data,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(fixed_key_node, SHR_E_MEMORY);


    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Keys: Root
     *
     * Convert the HW information read from the EE stage into
     * the returning list of API Key fields.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_field_mapping_get(unit, sid,
                                        BCMLTM_FIELD_TYPE_KEY,
                                        BCMLTM_FIELD_DIR_OUT,
                                        &keys_map_out));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_wb_to_api_key_fields,
                                       (void *)keys_map_out,
                                       &keys_node_data));
    keys_node = bcmltm_tree_node_create((void *)keys_node_data,
                                        &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(keys_node, SHR_E_MEMORY);
    fa_node_roots[1] = keys_node;

    /* Set node for next chain of Value transforms and validates */
    next_chain = keys_node;

    /*
     * Tree_1 - Xfrm Values: Left node of Keys
     *
     * Process field transforms for Values.
     */
    xfrm_type = BCMLTM_DB_XFRM_TYPE_REV_VALUE_TRANSFORM;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_rev_create(unit, sid,
                                            xfrm_type,
                                            BCMLT_OPCODE_TRAVERSE,
                                            next_chain,
                                            op_md_ptr,
                                            &next_chain));

    /*
     * Tree_1 - Xfrm Keys: Left node of Xfrm Values
     *
     * Process field transforms for Keys.
     */
    xfrm_type = BCMLTM_DB_XFRM_TYPE_REV_KEY_TRANSFORM;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fa_trees_rev_create(unit, sid,
                                            xfrm_type,
                                            BCMLT_OPCODE_TRAVERSE,
                                            next_chain,
                                            op_md_ptr,
                                            &next_chain));

    /*
     * Tree_1 - Values: Right node of Keys
     *
     * Convert the HW information read from the EE stage into
     * the returning list of API Value fields.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_field_mapping_get(unit, sid,
                                        BCMLTM_FIELD_TYPE_VALUE,
                                        BCMLTM_FIELD_DIR_OUT,
                                        &values_map));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_wb_to_api_fields,
                                       (void *)values_map,
                                       &values_node_data));
    values_node = bcmltm_tree_right_node_create(keys_node,
                                                (void *)values_node_data,
                                                &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(values_node, SHR_E_MEMORY);

    /* Set returning pointer */
    op_md_ptr->fa_node_roots = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        if (prev_keys_node == NULL) {
            SHR_FREE(prev_keys_node_data);
        }
        if (fixed_key_node == NULL) {
            SHR_FREE(fixed_key_node_data);
        }
        if (keys_node == NULL) {
            SHR_FREE(keys_node_data);
        }
        if (values_node == NULL) {
            SHR_FREE(values_node_data);
        }

        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, fa_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->fa_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the EE tree roots for a TRAVERSE operation.
 *
 * Create the EE tree roots for a TRAVERSE operation for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_keyed_ee_node_roots_traverse_create(int unit,
                                       bcmlrd_sid_t sid,
                                       bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *traverse_node = NULL;
    bcmltm_ee_node_t *traverse_node_data = NULL;
    const bcmltm_pt_keyed_op_info_t *traverse_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, size, "bcmltmKeyedTraverseEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /*
     * TRAVERSE
     *
     * EE Trees Roots
     * -----------------------------------------------------------------
     *   Tree_0:          [Root:Traverse]
     *                (PTM traverse operation)
     *
     * -----------------------------------------------------------------
     *   Tree_1:          NULL
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Traverse: Root
     *
     * Execute PTM traverse operation to get HW data.
     * Note that Get-First and Get-Next use same node cookie.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_pt_keyed_get(unit, sid, BCMPTM_OP_GET_FIRST,
                                   &traverse_info));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_model_keyed_traverse,
                                       (void *)traverse_info,
                                       &traverse_node_data));
    traverse_node =
        bcmltm_tree_node_create(traverse_node_data,
                                &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(traverse_node, SHR_E_MEMORY);
    ee_node_roots[0] = traverse_node;

    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Root
     * No actions.
     */
    ee_node_roots[1] = NULL;  /* Not used */

    /* Set returning pointer */
    op_md_ptr->ee_node_roots = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the ee_node (node data)
         * if corresponding tree creation was not successful.
         */
        if (traverse_node == NULL) {
            SHR_FREE(traverse_node_data);
        }

        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, ee_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->ee_node_roots = NULL;
    }

    SHR_FUNC_EXIT();

}


/*!
 * \brief Create operation metadata for the TRAVERSE opcode.
 *
 * Create the operation metadata for given table ID to use for the TRAVERSE
 * opcode.
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
lt_keyed_op_traverse_create(int unit, bcmlrd_sid_t sid,
                            bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get working buffer size */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "bcmltmKeyedTraverseLtOpMd");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_TRAVERSE_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lt_keyed_fa_node_roots_traverse_create(unit, sid, op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_keyed_ee_node_roots_traverse_create(unit, sid, op_md));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(op_md);
        *op_md_ptr = NULL;
    }
    SHR_FUNC_EXIT();
}


/*
 * LT Metadata Function Vector
 */
static bcmltm_md_lt_drv_t lt_keyed_drv = {
    /* op_destroy */            bcmltm_md_op_destroy,
    /* op_insert_create */      lt_keyed_op_insert_create,
    /* op_lookup_create */      lt_keyed_op_lookup_create,
    /* op_delete_create */      lt_keyed_op_delete_create,
    /* op_update_create */      lt_keyed_op_update_create,
    /* op_traverse_create */    lt_keyed_op_traverse_create,
};

/*******************************************************************************
 * Public functions
 */

const bcmltm_md_lt_drv_t *
bcmltm_md_lt_keyed_drv_get(void)
{
    return &lt_keyed_drv;
}
