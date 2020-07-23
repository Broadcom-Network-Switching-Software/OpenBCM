/*! \file bcmltm_md_lt_ltm.c
 *
 * Logical Table Manager - Logical Table with LTM map group.
 *
 * This file contains implementation to support metadata construction
 * for logical tables with LTM map group.
 *
 * LT with LTM map group are those that maps information contained within
 * LTM, such as table configuration (LT_CONFIG) or
 * table statistics (LT_STATS).
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


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/* Number of roots for LOOKUP operation */
#define LT_OP_LOOKUP_NUM_ROOTS      1

/* Number of roots for UPDATE operation */
#define LT_OP_UPDATE_NUM_ROOTS      1

/* Number of roots for TRAVERSE operation */
#define LT_OP_TRAVERSE_NUM_ROOTS    2


/*******************************************************************************
 * Private functions
 */

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
lt_ltm_fa_node_roots_lookup_create(int unit,
                                   bcmlrd_sid_t sid,
                                   bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *keys_node = NULL;
    bcmltm_fa_node_t *keys_node_data = NULL;
    const bcmltm_field_select_mapping_t *ltm_key = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, size, "bcmltmLtmLookupFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, size);

    /*
     * LOOKUP
     *
     * FA Tree Roots
     * -----------------------------------------------------------------
     *   Tree_0:            [Root:Keys]
     *                  (Convert API Keys)
     */

    /*
     * Tree_0 - Keys
     *
     * Convert API Key fields into physical fields in Working Buffer.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_ltm_key_get(unit, sid, &ltm_key));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_key_fields_to_wb,
                                       (void *)ltm_key,
                                       &keys_node_data));
    keys_node = bcmltm_tree_node_create((void *)keys_node_data,
                                        &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(keys_node, SHR_E_MEMORY);
    fa_node_roots[0] = keys_node;

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
lt_ltm_ee_node_roots_lookup_create(int unit,
                                   bcmlrd_sid_t sid,
                                   bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *read_node = NULL;
    bcmltm_ee_node_t *read_node_data = NULL;
    const bcmltm_field_spec_list_t *fs_list = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, size, "bcmltmLtmLookupEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /*
     * LOOKUP
     *
     * EE Trees Roots
     * -----------------------------------------------------------------
     *   Tree_0:           [Root:Read]
     *                 (Field spec read)
     */

    /*
     * Tree_0 - Read: Root
     *
     * Read Field spec.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_ltm_fs_list_get(unit, sid, &fs_list));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_lt_table_read,
                                       (void *)fs_list,
                                       &read_node_data));
    read_node = bcmltm_tree_node_create((void *)read_node_data,
                                        &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(read_node, SHR_E_MEMORY);
    ee_node_roots[0] = read_node;

    /* Set returning pointer */
    op_md_ptr->ee_node_roots = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the ee_node (node data)
         * if corresponding tree creation was not successful.
         */
        if (read_node == NULL) {
            SHR_FREE(read_node_data);
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
lt_ltm_op_lookup_create(int unit, bcmlrd_sid_t sid,
                        bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get working buffer size */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "bcmltmLtmLookupLtOpMd");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_LOOKUP_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lt_ltm_fa_node_roots_lookup_create(unit, sid, op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_ltm_ee_node_roots_lookup_create(unit, sid, op_md));

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
lt_ltm_fa_node_roots_update_create(int unit,
                                   bcmlrd_sid_t sid,
                                   bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *keys_node = NULL;
    bcmltm_fa_node_t *keys_node_data = NULL;
    const bcmltm_field_select_mapping_t *ltm_key = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, size, "bcmltmLtmUpdateFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, size);

    /*
     * UPDATE
     *
     * FA Tree Roots
     * -----------------------------------------------------------------
     *   Tree_0:            [Root:Keys]
     *                  (Convert API Keys)
     */

    /*
     * Tree_0 - Keys
     *
     * Convert API Key fields into physical fields in Working Buffer.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_ltm_key_get(unit, sid, &ltm_key));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_key_fields_to_wb,
                                       (void *)ltm_key,
                                       &keys_node_data));
    keys_node = bcmltm_tree_node_create((void *)keys_node_data,
                                        &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(keys_node, SHR_E_MEMORY);
    fa_node_roots[0] = keys_node;

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
lt_ltm_ee_node_roots_update_create(int unit,
                                   bcmlrd_sid_t sid,
                                   bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *write_node = NULL;
    bcmltm_ee_node_t *write_node_data = NULL;
    const bcmltm_field_spec_list_t *fs_list = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, size, "bcmltmLtmUpdateEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /*
     * UPDATE
     *
     * EE Trees Roots
     * -----------------------------------------------------------------
     *   Tree_0:           [Root:Write]
     *                  (Field spec write)
     */

    /*
     * Tree_0 - Write: Root
     *
     * Write Field spec.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_ltm_fs_list_get(unit, sid, &fs_list));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_lt_table_write,
                                       (void *)fs_list,
                                       &write_node_data));
    write_node = bcmltm_tree_node_create((void *)write_node_data,
                                         &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(write_node, SHR_E_MEMORY);
    ee_node_roots[0] = write_node;

    /* Set returning pointer */
    op_md_ptr->ee_node_roots = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the ee_node (node data)
         * if corresponding tree creation was not successful.
         */
        if (write_node == NULL) {
            SHR_FREE(write_node_data);
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
lt_ltm_op_update_create(int unit, bcmlrd_sid_t sid,
                        bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get working buffer size */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "bcmltmLtmUpdateLtOpMd");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_UPDATE_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lt_ltm_fa_node_roots_update_create(unit, sid, op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_ltm_ee_node_roots_update_create(unit, sid, op_md));

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
 * This function creates the FA tree roots for a TRAVERSE operation
 * for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_ltm_fa_node_roots_traverse_create(int unit,
                                     bcmlrd_sid_t sid,
                                     bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *traverse_node = NULL;
    bcmltm_node_t *keys_node_in  = NULL;
    bcmltm_node_t *keys_node_out = NULL;
    bcmltm_fa_node_t *traverse_node_data = NULL;
    bcmltm_fa_node_t *keys_node_data_in = NULL;
    bcmltm_fa_node_t *keys_node_data_out = NULL;
    const bcmltm_fa_ltm_t *ltm_info = NULL;
    const bcmltm_field_select_mapping_t *ltm_key = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, size, "bcmltmLtmTraverseFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, size);

    /*
     * TRAVERSE
     *
     * FA Tree Roots
     * -----------------------------------------------------------------
     *   Tree_0:              [Root:Traverse Index]
     *                                  |
     *                                  |
     *                      +-----------+----------+
     *                      |                      |
     *                      |                     ===
     *               [API Key fields]
     * -----------------------------------------------------------------
     *   Tree_1:                   [Root:Keys]
     *                         (Convert HW fields to return
     *                          API Keys fields list)
     */
    /*
     * Tree_0 - Traverse Index: Root
     *
     * LTM Index traverse operation.  On first traverse,
     * the first entry index is obtained.  Subsequent traverse
     * will obtained the next entry index.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_ltm_fa_ltm_get(unit, sid, &ltm_info));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_traverse_ltid,
                                       (void *)ltm_info,
                                       &traverse_node_data));
    traverse_node =
        bcmltm_tree_node_create(traverse_node_data,
                                &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(traverse_node, SHR_E_MEMORY);
    fa_node_roots[0] = traverse_node;

    /*
     * Tree_0 - Keys: Left node of Traverse Index
     *
     * Convert API Key fields into corresponding Working Buffer locations.
     * Keys are needed by the traverse index operation to get the next entry.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_ltm_key_get(unit, sid, &ltm_key));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_key_fields_to_wb,
                                       (void *)ltm_key,
                                       &keys_node_data_in));
    keys_node_in = bcmltm_tree_left_node_create(traverse_node,
                                                (void *)keys_node_data_in,
                                                &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(keys_node_in, SHR_E_MEMORY);

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
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_wb_to_api_fields,
                                       (void *)ltm_key,
                                       &keys_node_data_out));
    keys_node_out = bcmltm_tree_node_create((void *)keys_node_data_out,
                                            &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(keys_node_out, SHR_E_MEMORY);
    fa_node_roots[1] = keys_node_out;

    /* Set returning pointer */
    op_md_ptr->fa_node_roots = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        if (traverse_node == NULL) {
            SHR_FREE(traverse_node_data);
        }
        if (keys_node_in == NULL) {
            SHR_FREE(keys_node_data_in);
        }
        if (keys_node_out == NULL) {
            SHR_FREE(keys_node_data_out);
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
 * This function creates the EE tree roots for a TRAVERSE operation
 * for given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_ltm_ee_node_roots_traverse_create(int unit,
                                     bcmlrd_sid_t sid,
                                     bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *read_node = NULL;
    bcmltm_ee_node_t *read_node_data = NULL;
    const bcmltm_field_spec_list_t *fs_list = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, size, "bcmltmLtmTraverseEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, size);

    /*
     * TRAVERSE
     *
     * EE Trees Roots
     * -----------------------------------------------------------------
     *   Tree_0:          [Root:Read]
     *                 (API Key fields)
     *
     * -----------------------------------------------------------------
     *   Tree_1:          NULL
     */

    /*
     * Tree_0 - Root: Field spec
     */
    SHR_IF_ERR_EXIT
        (bcmltm_db_ltm_fs_list_get(unit, sid, &fs_list));
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       bcmltm_ee_node_lt_table_read,
                                       (void *)fs_list,
                                       &read_node_data));
    read_node = bcmltm_tree_node_create((void *)read_node_data,
                                        &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(read_node, SHR_E_MEMORY);
    ee_node_roots[0] = read_node;

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
        if (read_node == NULL) {
            SHR_FREE(read_node_data);
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
lt_ltm_op_traverse_create(int unit, bcmlrd_sid_t sid,
                          bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;
    uint32_t wb_wsize;

    SHR_FUNC_ENTER(unit);

    /* Get working buffer size */
    SHR_IF_ERR_EXIT
        (bcmltm_wb_lt_wsize_get(unit, sid, &wb_wsize));

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "bcmltmLtmTraverseLtOpMd");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = LT_OP_TRAVERSE_NUM_ROOTS;
    op_md->working_buffer_size = BCMLTM_WORDS2BYTES(wb_wsize);

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (lt_ltm_fa_node_roots_traverse_create(unit, sid, op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_ltm_ee_node_roots_traverse_create(unit, sid, op_md));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(op_md);
        *op_md_ptr = NULL;
    }
    SHR_FUNC_EXIT();
}

/*
 * LT Metadata Function Vector for LT with LTM groups mapping.
 */
static bcmltm_md_lt_drv_t lt_ltm_drv = {
    /* op_destroy */            bcmltm_md_op_destroy,
    /* op_insert_create */      NULL,
    /* op_lookup_create */      lt_ltm_op_lookup_create,
    /* op_delete_create */      NULL,
    /* op_update_create */      lt_ltm_op_update_create,
    /* op_traverse_create */    lt_ltm_op_traverse_create,
};


/*******************************************************************************
 * Public functions
 */

const bcmltm_md_lt_drv_t *
bcmltm_md_lt_ltm_drv_get(void)
{
    return &lt_ltm_drv;
}
