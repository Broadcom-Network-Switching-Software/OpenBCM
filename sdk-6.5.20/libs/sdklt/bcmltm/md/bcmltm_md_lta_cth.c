/*! \file bcmltm_md_lta_cth.c
 *
 * Logical Table Manager - LTA Custom Table Handler.
 *
 * This file contains implementation to support metadata construction
 * for logical tables with Custom Table Handler.
 *
 * Custom Table Handlers are only supported on Simple Index logical tables.
 * LTM uses the LTA Logical Table Adaptor interface to perform the
 * corresponding operation on the table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_debug.h>

#include <bsl/bsl.h>

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_lt_internal.h>
#include <bcmltm/bcmltm_db_internal.h>
#include <bcmltm/bcmltm_ee_tree_nodes_internal.h>
#include <bcmltm/bcmltm_fa_tree_nodes_internal.h>

#include "bcmltm_md_lt_drv.h"
#include "bcmltm_md_op.h"


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/*
 * Number of roots for INSERT/DELETE/UPDATE operation
 * Note: The INSERT is applicable on Simple Index LTs.
 */
#define LT_OP_INS_DEL_UPD_NUM_ROOTS   1

/* Number of roots for INSERT operation on Index with Allocation LTs */
#define LT_OP_ALLOC_INDEX_INSERT_NUM_ROOTS   2

/* Number of roots for LOOKUP operation */
#define LT_OP_LOOKUP_NUM_ROOTS   2

/* Number of roots for TRAVERSE operation */
#define LT_OP_TRAVERSE_NUM_ROOTS    2


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Create the FA tree roots for INSERT/DELETE/UPDATE operation.
 *
 * Create the FA tree roots for any of the following operations:
 *     INSERT (this is for Simple Index LT, i.e. non IwA)
 *     DELETE
 *     UPDATE
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] opcode LT opcode INSERT, DELETE or UPDATE.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lta_cth_fa_node_roots_create(int unit,
                             bcmlrd_sid_t sid,
                             bcmlt_opcode_t opcode,
                             bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *to_lta_node = NULL;
    bcmltm_node_t *init_in_node = NULL;
    bcmltm_node_t *to_lta_key_node = NULL;
    bcmltm_fa_node_t *to_lta_node_data = NULL;
    bcmltm_fa_node_t *init_in_node_data = NULL;
    bcmltm_fa_node_t *to_lta_key_node_data = NULL;
    const bcmltm_lta_field_list_t *input_fields = NULL;
    const bcmltm_lta_field_list_t *input_key_fields = NULL;
    const bcmltm_lta_field_list_t *init_fields = NULL;
    bcmltm_cth_handler_intf_type_t cth_intf;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, size, "bcmltmCthFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, size);

    /* Check for valid opcode */
    if ((opcode != BCMLT_OPCODE_INSERT) &&
        (opcode != BCMLT_OPCODE_DELETE) &&
        (opcode != BCMLT_OPCODE_UPDATE)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /*
     * INSERT/DELETE/UPDATE
     *
     * FA Tree Roots
     *   -------------------------------------------------------------
     *   Tree_0:          [Root]
     *                    (Convert value fields to
     *                     LTA Input fields)
     *                       |
     *                       |
     *              +--------+---------------+
     *              |                        |
     *         [Left_Node]             [Right_Node]
     *    (Init LTA Input fields)      (Convert key fields
     *                                  to LTA Input fields)
     *             #1                       #2
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Root
     * Initialize the LTA Input fields list and
     * convert API fields into LTA fields list in Working Buffer.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_cth_in_key_fields_get(unit, sid, &input_key_fields));
    SHR_IF_ERR_EXIT
        (bcmltm_db_cth_handler_intf_type_get(unit, sid, &cth_intf));

    if ((opcode == BCMLT_OPCODE_INSERT) ||
        (opcode == BCMLT_OPCODE_UPDATE)) {
        /* We will need value fields eventually */
        SHR_IF_ERR_EXIT
            (bcmltm_db_cth_in_fields_get(unit, sid, &input_fields));
        init_fields = input_fields;
    } else {
        /* DELETE */
        init_fields = input_key_fields;
    }

    if ((cth_intf == BCMLTM_CTH_HANDLER_INTF_TYPE_EXPEDITED) &&
        (opcode != BCMLT_OPCODE_DELETE)){
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                               bcmltm_fa_node_api_fields_cth_to_lta_expedited,
                                           (void *)input_fields,
                                           &to_lta_node_data));
        to_lta_node = bcmltm_tree_node_create((void *)to_lta_node_data,
                                              &(op_md_ptr->num_nodes));
        SHR_NULL_CHECK(to_lta_node, SHR_E_MEMORY);
        fa_node_roots[0] = to_lta_node;
    } else {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_fields_cth_to_lta,
                                           (void *)input_fields,
                                           &to_lta_node_data));
        to_lta_node = bcmltm_tree_node_create((void *)to_lta_node_data,
                                              &(op_md_ptr->num_nodes));
        SHR_NULL_CHECK(to_lta_node, SHR_E_MEMORY);
        fa_node_roots[0] = to_lta_node;

        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_key_fields_to_lta,
                                           (void *)input_key_fields,
                                           &to_lta_key_node_data));
        to_lta_key_node = bcmltm_tree_right_node_create(to_lta_node,
                                       (void *)to_lta_key_node_data,
                                       &(op_md_ptr->num_nodes));
        SHR_NULL_CHECK(to_lta_key_node, SHR_E_MEMORY);
    }

    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_input_init,
                                       (void *)init_fields,
                                       &init_in_node_data));
    init_in_node = bcmltm_tree_left_node_create(to_lta_node,
                                                (void *)init_in_node_data,
                                                &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(init_in_node, SHR_E_MEMORY);

    /* Set returning pointer */
    op_md_ptr->fa_node_roots = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        if (to_lta_node == NULL) {
            SHR_FREE(to_lta_node_data);
        }
        if (to_lta_key_node == NULL) {
            SHR_FREE(to_lta_key_node_data);
        }
        if (init_in_node == NULL) {
            SHR_FREE(init_in_node_data);
        }

        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, fa_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->fa_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the EE tree roots for a INSERT/DELETE/UPDATE operation.
 *
 * Create the EE tree roots for any of the following operations:
 *     INSERT (this is for Simple Index LT, i.e. non IwA)
 *     DELETE
 *     UPDATE
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] opcode LT opcode INSERT, DELETE or UPDATE.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lta_cth_ee_node_roots_create(int unit,
                             bcmlrd_sid_t sid,
                             bcmlt_opcode_t opcode,
                             bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *op_node = NULL;
    bcmltm_ee_node_t *op_node_data = NULL;
    const bcmltm_lta_table_params_t *table_params = NULL;
    bcmltm_ee_node_f node_function = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, size, "bcmltmCthEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /* Check for valid opcode */
    if ((opcode != BCMLT_OPCODE_INSERT) &&
        (opcode != BCMLT_OPCODE_DELETE) &&
        (opcode != BCMLT_OPCODE_UPDATE)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /*
     * INSERT/DELETE/UPDATE
     *
     * EE Trees Roots
     *   -------------------------------------------------------------
     *   Tree_0:          [Root]
     *                    (LTA CTH operation)
 */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Root
     * Execute LTA Custom Table Handler operation.
     */
    if (opcode == BCMLT_OPCODE_INSERT) {
        node_function = bcmltm_ee_node_lta_insert;
    } else if (opcode == BCMLT_OPCODE_DELETE) {
        node_function = bcmltm_ee_node_lta_delete;
    } else if (opcode == BCMLT_OPCODE_UPDATE) {
        node_function = bcmltm_ee_node_lta_update;
    }
    SHR_IF_ERR_EXIT
        (bcmltm_db_cth_table_params_get(unit, sid, &table_params));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       node_function,
                                       (void *)table_params,
                                       &op_node_data));
    op_node = bcmltm_tree_node_create((void *)op_node_data,
                                      &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(op_node, SHR_E_MEMORY);
    ee_node_roots[0] = op_node;

    /* Set returning pointer */
    op_md_ptr->ee_node_roots = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the ee_node (node data)
         * if corresponding tree creation was not successful.
         */
        if (op_node == NULL) {
            SHR_FREE(op_node_data);
        }

        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, ee_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->ee_node_roots = NULL;
    }

    SHR_FUNC_EXIT();

}


/*!
 * \brief Create the FA tree roots for INSERT on IwA LT.
 *
 * Create the FA tree roots for an INSERT operation for given
 * Index with Allocation logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lta_cth_alloc_index_fa_node_roots_insert_create(int unit,
                                                bcmlrd_sid_t sid,
                                                bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *to_lta_node = NULL;
    bcmltm_node_t *init_in_node = NULL;
    bcmltm_node_t *init_out_node = NULL;
    bcmltm_node_t *to_api_node = NULL;
    bcmltm_fa_node_t *to_lta_node_data = NULL;
    bcmltm_fa_node_t *init_in_node_data = NULL;
    bcmltm_fa_node_t *init_out_node_data = NULL;
    bcmltm_fa_node_t *to_api_node_data = NULL;
    const bcmltm_lta_field_list_t *input_fields = NULL;
    const bcmltm_lta_field_list_t *output_fields = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, size, "bcmltmCthInsertFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, size);

    /*
     * INSERT - IwA
     *
     * FA Tree Roots
     *   -------------------------------------------------------------
     *   Tree_0:          [Root]
     *                    (Convert to
     *                     LTA Input fields)
     *                       |
     *                       |
     *         +-------------+-------------+
     *         |                           |
     *    [Left_Node]                 [Right_Node]
     *    (Init LTA Output fields)    (Init LTA Input fields)
     *
     *   -------------------------------------------------------------
     *   Tree_1:          [Root]
     *                    (Copy LTA Output fields
     *                     to API fields list)
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Root
     * Initialize the LTA Input fields list and
     * convert API fields into LTA fields list in Working Buffer.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_cth_in_fields_get(unit, sid, &input_fields));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_value_fields_to_lta,
                                       (void *)input_fields,
                                       &to_lta_node_data));
    to_lta_node = bcmltm_tree_node_create((void *)to_lta_node_data,
                                          &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(to_lta_node, SHR_E_MEMORY);
    fa_node_roots[0] = to_lta_node;

    /*
     * Tree_0 - Left_Node
     * Initialize the LTA Output fields list.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_cth_out_fields_get(unit, sid, &output_fields));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_output_init,
                                       (void *)output_fields,
                                       &init_out_node_data));
    init_out_node = bcmltm_tree_left_node_create(to_lta_node,
                                                 (void *)init_out_node_data,
                                                 &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(init_out_node, SHR_E_MEMORY);

    /*
     * Tree_0 - Right_Node
     * Initialize the LTA Input fields list.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_input_init,
                                       (void *)input_fields,
                                       &init_in_node_data));
    init_in_node = bcmltm_tree_right_node_create(to_lta_node,
                                                 (void *)init_in_node_data,
                                                 &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(init_in_node, SHR_E_MEMORY);

    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Root
     * Copy the LTA Output fields list to API fields list.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_to_api_fields,
                                       (void *)output_fields,
                                       &to_api_node_data));
    to_api_node = bcmltm_tree_node_create((void *)to_api_node_data,
                                          &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(to_api_node, SHR_E_MEMORY);
    fa_node_roots[1] = to_api_node;

    /* Set returning pointer */
    op_md_ptr->fa_node_roots = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        if (to_lta_node == NULL) {
            SHR_FREE(to_lta_node_data);
        }
        if (init_in_node == NULL) {
            SHR_FREE(init_in_node_data);
        }
        if (init_out_node == NULL) {
            SHR_FREE(init_out_node_data);
        }
        if (to_api_node == NULL) {
            SHR_FREE(to_api_node_data);
        }

        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, fa_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->fa_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the EE tree roots for INSERT on IwA LT.
 *
 * Create the EE tree roots for an INSERT operation for given
 * Index with Allocation logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lta_cth_alloc_index_ee_node_roots_insert_create(int unit,
                                                bcmlrd_sid_t sid,
                                                bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *insert_node = NULL;
    bcmltm_ee_node_t *insert_node_data = NULL;
    const bcmltm_lta_table_params_t *table_params = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, size, "bcmltmCthInsertEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /*
     * INSERT - IwA
     *
     * EE Trees Roots
     *   -------------------------------------------------------------
     *   Tree_0:          [Root]
     *                    (LTA CTH insert with allocation operation)
     *
     *   -------------------------------------------------------------
     *   Tree_1:          NULL
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Root
     * Execute LTA Custom Table Handler insert with allocation operation.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_cth_table_params_get(unit, sid, &table_params));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_lta_alloc_insert,
                                       (void *)table_params,
                                       &insert_node_data));
    insert_node = bcmltm_tree_node_create((void *)insert_node_data,
                                          &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(insert_node, SHR_E_MEMORY);
    ee_node_roots[0] = insert_node;

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
 * \brief Create operation metadata for the INSERT opcode on Simple Index LT.
 *
 * Create the operation metadata for given table ID to use for the INSERT
 * opcode on Simple Index LTs.
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
lta_cth_simple_index_op_insert_create(int unit, bcmlrd_sid_t sid,
                                      bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get working buffer size */
    SHR_IF_ERR_EXIT(bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "bcmltmCthInsertLtOpMd");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_INS_DEL_UPD_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lta_cth_fa_node_roots_create(unit, sid,
                                      BCMLT_OPCODE_INSERT,
                                      op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lta_cth_ee_node_roots_create(unit, sid,
                                      BCMLT_OPCODE_INSERT,
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
 * \brief Create operation metadata for the INSERT opcode on IwA LT.
 *
 * Create the operation metadata for given table ID to use for the INSERT
 * opcode on Index with Allocation LTs.
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
lta_cth_alloc_index_op_insert_create(int unit, bcmlrd_sid_t sid,
                                     bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get working buffer size */
    SHR_IF_ERR_EXIT(bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "bcmltmCthInsertLtOpMd");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_ALLOC_INDEX_INSERT_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lta_cth_alloc_index_fa_node_roots_insert_create(unit, sid,
                                                  op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lta_cth_alloc_index_ee_node_roots_insert_create(unit, sid,
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
lta_cth_op_insert_create(int unit, bcmlrd_sid_t sid,
                         bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_table_type_t table_type;

    SHR_FUNC_ENTER(unit);

    /* Get table type */
    SHR_IF_ERR_EXIT
        (bcmltm_db_table_type_get(unit, sid, &table_type));

    if (BCMLTM_TABLE_TYPE_IS_ALLOC_INDEX(table_type)) {
        /* IwA CTH */
        SHR_IF_ERR_EXIT
            (lta_cth_alloc_index_op_insert_create(unit, sid,
                                                  op_md_ptr));
    } else {
        /* SI or CONFIG CTH */
        SHR_IF_ERR_EXIT
            (lta_cth_simple_index_op_insert_create(unit, sid,
                                                   op_md_ptr));
    }

 exit:
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
lta_cth_fa_node_roots_lookup_create(int unit,
                                    bcmlrd_sid_t sid,
                                    bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *to_lta_node = NULL;
    bcmltm_node_t *init_in_node = NULL;
    bcmltm_node_t *init_out_node = NULL;
    bcmltm_node_t *to_api_node = NULL;
    bcmltm_fa_node_t *to_lta_node_data = NULL;
    bcmltm_fa_node_t *init_in_node_data = NULL;
    bcmltm_fa_node_t *init_out_node_data = NULL;
    bcmltm_fa_node_t *to_api_node_data = NULL;
    const bcmltm_lta_field_list_t *input_fields = NULL;
    const bcmltm_lta_field_list_t *output_fields = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, size, "bcmltmCthLookupFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, size);

    /*
     * LOOKUP
     *
     * FA Tree Roots
     *   -------------------------------------------------------------
     *   Tree_0:          [Root]
     *                    (Convert to
     *                     LTA Input fields)
     *                       |
     *                       |
     *         +-------------+-------------+
     *         |                           |
     *    [Left_Node]                 [Right_Node]
     *    (Init LTA Output fields)    (Init LTA Input fields)
     *
     *   -------------------------------------------------------------
     *   Tree_1:          [Root]
     *                    (Copy LTA Output fields
     *                     to API fields list)
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Root
     * Initialize the LTA Input fields list and
     * convert API fields into LTA fields list in Working Buffer.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_cth_in_key_fields_get(unit, sid, &input_fields));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_key_fields_to_lta,
                                       (void *)input_fields,
                                       &to_lta_node_data));
    to_lta_node = bcmltm_tree_node_create((void *)to_lta_node_data,
                                          &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(to_lta_node, SHR_E_MEMORY);
    fa_node_roots[0] = to_lta_node;

    /*
     * Tree_0 - Left_Node
     * Initialize the LTA Output fields list.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_cth_out_fields_get(unit, sid, &output_fields));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_output_init,
                                       (void *)output_fields,
                                       &init_out_node_data));
    init_out_node = bcmltm_tree_left_node_create(to_lta_node,
                                                 (void *)init_out_node_data,
                                                 &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(init_out_node, SHR_E_MEMORY);

    /*
     * Tree_0 - Right_Node
     * Initialize the LTA Input fields list.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_input_init,
                                       (void *)input_fields,
                                       &init_in_node_data));
    init_in_node = bcmltm_tree_right_node_create(to_lta_node,
                                                 (void *)init_in_node_data,
                                                 &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(init_in_node, SHR_E_MEMORY);

    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Root
     * Copy the LTA Output fields list to API fields list.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_to_api_fields,
                                       (void *)output_fields,
                                       &to_api_node_data));
    to_api_node = bcmltm_tree_node_create((void *)to_api_node_data,
                                          &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(to_api_node, SHR_E_MEMORY);
    fa_node_roots[1] = to_api_node;

    /* Set returning pointer */
    op_md_ptr->fa_node_roots = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        if (to_lta_node == NULL) {
            SHR_FREE(to_lta_node_data);
        }
        if (init_in_node == NULL) {
            SHR_FREE(init_in_node_data);
        }
        if (init_out_node == NULL) {
            SHR_FREE(init_out_node_data);
        }
        if (to_api_node == NULL) {
            SHR_FREE(to_api_node_data);
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
lta_cth_ee_node_roots_lookup_create(int unit,
                                    bcmlrd_sid_t sid,
                                    bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *lookup_node = NULL;
    bcmltm_ee_node_t *lookup_node_data = NULL;
    const bcmltm_lta_table_params_t *table_params = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, size, "bcmltmCthLookupEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /*
     * LOOKUP
     *
     * EE Trees Roots
     *   -------------------------------------------------------------
     *   Tree_0:          [Root]
     *                    (LTA CTH lookup operation)
     *
     *   -------------------------------------------------------------
     *   Tree_1:          NULL
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Root
     * Execute LTA Custom Table Handler lookup operation.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_cth_table_params_get(unit, sid, &table_params));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_lta_lookup,
                                       (void *)table_params,
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
lta_cth_op_lookup_create(int unit, bcmlrd_sid_t sid,
                         bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get working buffer size */
    SHR_IF_ERR_EXIT(bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "bcmltmCthLookupLtOpMd");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_LOOKUP_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lta_cth_fa_node_roots_lookup_create(unit, sid,
                                             op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lta_cth_ee_node_roots_lookup_create(unit, sid,
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
lta_cth_op_delete_create(int unit, bcmlrd_sid_t sid,
                         bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get working buffer size */
    SHR_IF_ERR_EXIT(bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "bcmltmCthDeleteLtOpMd");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_INS_DEL_UPD_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lta_cth_fa_node_roots_create(unit, sid,
                                      BCMLT_OPCODE_DELETE,
                                      op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lta_cth_ee_node_roots_create(unit, sid,
                                      BCMLT_OPCODE_DELETE,
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
lta_cth_op_update_create(int unit, bcmlrd_sid_t sid,
                         bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get working buffer size */
    SHR_IF_ERR_EXIT(bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "bcmltmCthUpdateLtOpMd");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_INS_DEL_UPD_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lta_cth_fa_node_roots_create(unit, sid,
                                      BCMLT_OPCODE_UPDATE,
                                      op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lta_cth_ee_node_roots_create(unit, sid,
                                      BCMLT_OPCODE_UPDATE,
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
lta_cth_fa_node_roots_traverse_create(int unit,
                                      bcmlrd_sid_t sid,
                                      bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *to_lta_node = NULL;
    bcmltm_node_t *init_in_node = NULL;
    bcmltm_node_t *init_out_node = NULL;
    bcmltm_node_t *to_api_node = NULL;
    bcmltm_fa_node_t *to_lta_node_data = NULL;
    bcmltm_fa_node_t *init_in_node_data = NULL;
    bcmltm_fa_node_t *init_out_node_data = NULL;
    bcmltm_fa_node_t *to_api_node_data = NULL;
    const bcmltm_lta_field_list_t *input_fields = NULL;
    const bcmltm_lta_field_list_t *output_fields = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, size, "bcmltmCthTraverseFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, size);

    /*
     * TRAVERSE
     *
     * FA Tree Roots
     * -------------------------------------------------------------
     *   Tree_0:          [Root]
     *                    (Convert to
     *                     LTA Input fields)
     *                       |
     *                       |
     *         +-------------+-------------+
     *         |                           |
     *    [Left_Node]                 [Right_Node]
     *    (Init LTA Output fields)    (Init LTA Input fields)
     *
     * -------------------------------------------------------------
     *   Tree_1:          [Root]
     *                    (Copy LTA Output fields
     *                     to API fields list)
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Root
     * Initialize the LTA Input fields list and
     * convert API fields into LTA fields list in Working Buffer.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_cth_in_key_fields_get(unit, sid, &input_fields));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_key_fields_to_lta,
                                       (void *)input_fields,
                                       &to_lta_node_data));
    to_lta_node = bcmltm_tree_node_create((void *)to_lta_node_data,
                                          &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(to_lta_node, SHR_E_MEMORY);
    fa_node_roots[0] = to_lta_node;

    /*
     * Tree_0 - Left_Node
     * Initialize the LTA Output fields list.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_cth_out_fields_get(unit, sid, &output_fields));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_output_init,
                                       (void *)output_fields,
                                       &init_out_node_data));
    init_out_node = bcmltm_tree_left_node_create(to_lta_node,
                                                 (void *)init_out_node_data,
                                                 &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(init_out_node, SHR_E_MEMORY);

    /*
     * Tree_0 - Right_Node
     * Initialize the LTA Input fields list.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_input_init,
                                       (void *)input_fields,
                                       &init_in_node_data));
    init_in_node = bcmltm_tree_right_node_create(to_lta_node,
                                                 (void *)init_in_node_data,
                                                 &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(init_in_node, SHR_E_MEMORY);

    /*-----------------
     * Tree_1
     */
    /*
     * Tree_1 - Root
     * Copy the LTA Output fields list to API fields list.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_to_api_fields,
                                       (void *)output_fields,
                                       &to_api_node_data));
    to_api_node = bcmltm_tree_node_create((void *)to_api_node_data,
                                          &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(to_api_node, SHR_E_MEMORY);
    fa_node_roots[1] = to_api_node;

    /* Set returning pointer */
    op_md_ptr->fa_node_roots = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        if (to_lta_node == NULL) {
            SHR_FREE(to_lta_node_data);
        }
        if (init_in_node == NULL) {
            SHR_FREE(init_in_node_data);
        }
        if (init_out_node == NULL) {
            SHR_FREE(init_out_node_data);
        }
        if (to_api_node == NULL) {
            SHR_FREE(to_api_node_data);
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
lta_cth_ee_node_roots_traverse_create(int unit,
                                      bcmlrd_sid_t sid,
                                      bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *traverse_node = NULL;
    bcmltm_ee_node_t *traverse_node_data = NULL;
    const bcmltm_lta_table_params_t *table_params = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, size, "bcmltmCthTraverseEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /*
     * TRAVERSE
     *
     * EE Trees Roots
     * -------------------------------------------------------------
     *   Tree_0:          [Root]
     *                    (LTA CTH traverse operation)
     *
     *   -------------------------------------------------------------
     *   Tree_1:          NULL
     */

    /*-----------------
     * Tree_0
     */
    /*
     * Tree_0 - Root
     * Execute LTA Custom Table Handler traverse operation.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_cth_table_params_get(unit, sid, &table_params));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_lta_traverse,
                                       (void *)table_params,
                                       &traverse_node_data));
    traverse_node = bcmltm_tree_node_create((void *)traverse_node_data,
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
lta_cth_op_traverse_create(int unit, bcmlrd_sid_t sid,
                           bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;
    const bcmltd_table_handler_t *cth_handler = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get CTH handler */
    SHR_IF_ERR_EXIT(bcmltm_db_cth_handler_get(unit, sid, &cth_handler));

    /*
     * Check for NULL traverse handler.
     *
     * Currently, the traverse interface is optional.  This
     * temporary feature was provided to allow CTH LTs to
     * transition and implement the new traverse opcode.
     * The goal is to make traverse mandatory (like other opcodes).
     * Once all CTHs have the traverse interface, traverse will be
     * mandatory (FLTG check) and this check can be removed.
     */
    if ((cth_handler == NULL) ||
        ((cth_handler->traverse == NULL) &&
         (cth_handler->op_first == NULL) &&
         (cth_handler->op_next == NULL))) {
        *op_md_ptr = NULL;
        SHR_EXIT();
    }

    /* Get working buffer size */
    SHR_IF_ERR_EXIT(bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "bcmltmCthTraverseLtOpMd");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_TRAVERSE_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lta_cth_fa_node_roots_traverse_create(unit, sid, op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lta_cth_ee_node_roots_traverse_create(unit, sid,op_md));

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
static bcmltm_md_lt_drv_t lta_cth_drv = {
    /* op_destroy */            bcmltm_md_op_destroy,
    /* op_insert_create */      lta_cth_op_insert_create,
    /* op_lookup_create */      lta_cth_op_lookup_create,
    /* op_delete_create */      lta_cth_op_delete_create,
    /* op_update_create */      lta_cth_op_update_create,
    /* op_traverse_create */    lta_cth_op_traverse_create,
};

/*******************************************************************************
 * Public functions
 */

const bcmltm_md_lt_drv_t *
bcmltm_md_lta_cth_drv_get(void)
{
    return &lta_cth_drv;
}
