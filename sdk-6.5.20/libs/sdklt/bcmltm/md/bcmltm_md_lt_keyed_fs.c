/*! \file bcmltm_md_lt_keyed_fs.c
 *
 * Logical Table Manager - Keyed Field Select Logical Table (Hash and TCAM).
 *
 * This file contains implementation to support metadata construction
 * for Direct Mapped Keyed, Hash and TCAM, Logical Tables with
 * Field Selections.
 *
 * This implementation is based on the following field selection premises:
 * - A field selection group of KEY selector type cannot have any
 *   parent (i.e. level should be 1).
 * - Selectees field maps can only involve value fields (thus,
 *   a key field cannot be subject to a selection).
 *
 * Field Selects
 * -------------
 * Field Select clusters are added to the different opcode trees
 * as needed to process the maps corresponding for each field selection:
 *    Level 0: maps with no selections
 *    Level 1: maps with 1 parent
 *    Level 2: maps with 2 parents
 *    etc.
 *
 * The maps in each Field Select cluster includes:
 *    - Direct mapped field values
 *    - Fixed fields
 *    - Field LTA (transforms and validations)
 *
 * Transforms and Validations
 * --------------------------
 * Field Transforms and Field Validations clusters are
 * added to the different opcode trees as needed to process the
 * arguments.
 *
 * For more details, see corresponding description in routines
 * that create the clusters of Field Transform and Field Validation
 * subtrees.
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
#include "bcmltm_md_lta_xfrm_fs.h"
#include "bcmltm_md_lta_validate_fs.h"
#include "bcmltm_md_fs.h"

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
 * \brief Create FA tree to process input key fields.
 *
 * This routine creates the FA tree to process the
 * input of API key fields for Keyed LTs.
 * The process involves performing all necessary steps
 * to translate the API input fields to their corresponding
 * destination working buffers ready for the next stage.
 *
 * Flow:
 *    API input --> API cache --> PTM
 *
 * Processing of key fields includes the following:
 *   - field validations
 *   - field transforms
 *   - direct field maps
 *   - fixed fields
 *   - field selectors (key field selector value to SMI)
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] opcode LT opcode.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] fa_tree Root of FA tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_keyed_fs_key_write_fa_tree_create(int unit,
                                     bcmlrd_sid_t sid,
                                     bcmlt_opcode_t opcode,
                                     bcmltm_lt_op_md_t *op_md_ptr,
                                     bcmltm_node_t **fa_tree)
{
    bcmltm_node_t *root_node = NULL;
    bcmltm_node_t *key_fsm_map_node = NULL;
    bcmltm_node_t *key_c2p_node = NULL;
    bcmltm_node_t *fixed_key_node = NULL;
    bcmltm_node_t *key_fwd_xfrm_node = NULL;
    bcmltm_node_t *key_validate_node = NULL;
    bcmltm_node_t *key_a2c_node = NULL;
    bcmltm_fa_node_f key_c2p_func = NULL;
    const bcmltm_fixed_field_mapping_t *fixed_keys = NULL;
    const bcmltm_field_select_mapping_t *apic_in_keys = NULL;
    const bcmltm_field_select_mapping_t *in_keys = NULL;
    bool traverse_start = FALSE;

    SHR_FUNC_ENTER(unit);

    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_fixed_key_fields_get(unit, sid,
                                           &fixed_keys));
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_apic_key_fields_get(unit, sid,
                                          BCMLTM_FIELD_DIR_IN,
                                          &apic_in_keys));
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_key_fields_get(unit, sid,
                                     BCMLTM_FIELD_DIR_IN,
                                     &in_keys));

    /* [Root]: Nothing to do (used for chaining nodes) */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  NULL,
                                  NULL,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &root_node));

    /* [Key_Fsm_Map]: Key field selector value to field select map index */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fs_key_fsm_map_cluster_create(unit, sid,
                                                 BCMLTM_NODE_SIDE_LEFT,
                                                 root_node,
                                                 op_md_ptr,
                                                 &key_fsm_map_node));

    /* [Key_C2P]: API cache keys to PT params */
    if (opcode == BCMLT_OPCODE_TRAVERSE) {
        /*
         * Use traverse version.  This version checks on
         * traverse start flag, and if so, it skips copy of
         * API cache to PTM (key fields are not supplied).
         */
        key_c2p_func = bcmltm_fa_node_key_traverse_api_wb_to_wbc;
    } else {
        /* Use regular version to copy API cache to PTM  */
        key_c2p_func = bcmltm_fa_node_api_wb_to_wbc;
    }
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  key_c2p_func,
                                  (void *)in_keys,
                                  BCMLTM_NODE_SIDE_RIGHT,
                                  key_fsm_map_node,
                                  op_md_ptr,
                                  &key_c2p_node));

    /* [Fixed_Key]: Set fixed key fields from LRD */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_select_fixed_fields_to_wb,
                                  (void *)fixed_keys,
                                  BCMLTM_NODE_SIDE_RIGHT,
                                  key_c2p_node,
                                  op_md_ptr,
                                  &fixed_key_node));

    /* [Key_Fwd_Xfrm]: Process LTA key forward transforms */
    if (opcode == BCMLT_OPCODE_TRAVERSE) {
        /*
         * Check on traverse start flag.
         * If operation is a traverse start, transform is skipped.
         * (key fields are not supplied).
         */
        traverse_start = TRUE;
    }
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fs_fwd_key_cluster_create(unit, sid,
                                                  opcode, traverse_start,
                                                  BCMLTM_NODE_SIDE_LEFT,
                                                  key_fsm_map_node,
                                                  op_md_ptr,
                                                  &key_fwd_xfrm_node));

    /* [Key_Validate]: Process LTA key field validations */
    SHR_IF_ERR_EXIT
        (bcmltm_md_validate_fs_key_cluster_create(unit, sid,
                                                  opcode,
                                                  BCMLTM_NODE_SIDE_LEFT,
                                                  key_fwd_xfrm_node,
                                                  op_md_ptr,
                                                  &key_validate_node));

    /* [Key_A2C]: API keys to API cache */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_in_key_fields_to_api_wb,
                                  (void *)apic_in_keys,
                                  BCMLTM_NODE_SIDE_LEFT,
                                  key_validate_node,
                                  op_md_ptr,
                                  &key_a2c_node));

    /* Set returning pointer */
    *fa_tree = root_node;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy trees */
        bcmltm_tree_destroy(root_node, &(op_md_ptr->num_nodes), TRUE);
        *fa_tree = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create FA tree cluster to process output key fields.
 *
 * This routine creates the FA subtree to process the
 * output of API key fields for Keyed LTs.
 * The process involves performing all necessary steps
 * to translate the data from the destination working buffers
 * to their corresponding API output fields.
 *
 * Flow:
 *    PTM --> API cache --> API output
 *
 * Processing of key fields includes the following:
 *   - field validations
 *   - field transforms
 *   - direct field maps
 *   - fixed fields
 *   - field selectors (key field selector value to SMI)
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] opcode LT opcode.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] fa_tree Root of FA tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_keyed_fs_key_read_fa_cluster_create(int unit,
                                       bcmlrd_sid_t sid,
                                       bcmlt_opcode_t opcode,
                                       bcmltm_node_side_t node_side,
                                       bcmltm_node_t *parent_chain,
                                       bcmltm_lt_op_md_t *op_md_ptr,
                                       bcmltm_node_t **next_chain)
{
    bcmltm_node_t *key_fsm_map_node = NULL;
    bcmltm_node_t *key_c2a_node = NULL;
    bcmltm_node_t *key_rev_xfrm_node = NULL;
    bcmltm_node_t *key_p2c_node = NULL;
    const bcmltm_field_select_mapping_t *apic_out_keys = NULL;
    const bcmltm_field_select_mapping_t *out_keys = NULL;

    SHR_FUNC_ENTER(unit);

    /*
     * ROOT is not valid since current routine interface
     * does not return new root.  If needed, this can be enhanced
     * to accommodate ROOT.
     */
    if (node_side == BCMLTM_NODE_SIDE_ROOT) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_apic_key_fields_get(unit, sid,
                                          BCMLTM_FIELD_DIR_OUT,
                                          &apic_out_keys));
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_key_fields_get(unit, sid,
                                     BCMLTM_FIELD_DIR_OUT,
                                     &out_keys));

    /* [Key_Fsm_Map]: Key field selector value to field select map index */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fs_key_fsm_map_cluster_create(unit, sid,
                                                 node_side,
                                                 parent_chain,
                                                 op_md_ptr,
                                                 &key_fsm_map_node));

    /* [Key_C2A]: Copy API cache keys to API LT entry output */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_api_wb_to_out_key_fields,
                                  (void *)apic_out_keys,
                                  BCMLTM_NODE_SIDE_LEFT,
                                  key_fsm_map_node,
                                  op_md_ptr,
                                  &key_c2a_node));

    /* [Key_Rev_Xfrm]: Process LTA key reverse transforms */
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fs_rev_key_cluster_create(unit, sid,
                                                  opcode,
                                                  BCMLTM_NODE_SIDE_RIGHT,
                                                  key_c2a_node,
                                                  op_md_ptr,
                                                  &key_rev_xfrm_node));

    /* [Key_P2C]: Copy PTM physical fields to API cache */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_wbc_to_api_wb,
                                  (void *)out_keys,
                                  BCMLTM_NODE_SIDE_LEFT,
                                  key_c2a_node,
                                  op_md_ptr,
                                  &key_p2c_node));

    /*
     * Chain node to additional subtrees.
     * A node used for chaining must be:
     * - the first node to execute in this cluster
     * - a node without any child node (to allow chaining to either side).
     */
    *next_chain = key_p2c_node;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create FA tree roots for INSERT on Keyed LT.
 *
 * This function creates the FA tree roots for an INSERT operation
 * on given Keyed logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_keyed_fs_fa_node_roots_insert_create(int unit,
                                        bcmlrd_sid_t sid,
                                        bcmltm_lt_op_md_t *op_md_ptr)
{
    bcmlt_opcode_t opcode = BCMLT_OPCODE_INSERT;
    unsigned int alloc_size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *key_write_tree = NULL;
    bcmltm_node_t *root1_node = NULL;
    bcmltm_node_t *value_write_fs_node = NULL;
    bcmltm_node_t *ro_check_node = NULL;
    bcmltm_node_t *wide_field_node = NULL;
    const bcmltm_ro_field_list_t *ro_list = NULL;
    const bcmltm_wide_field_list_t *wide_list = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    alloc_size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (alloc_size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, alloc_size, "bcmltmKeyedInsertFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, alloc_size);

    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_read_only_list_get(unit, sid, &ro_list));
    SHR_IF_ERR_EXIT
        (bcmltm_db_wide_field_list_get(unit, sid, &wide_list));

    /*-----------------
     * Tree_0
     */

    /* [Key_Write]: Process input key fields */
    SHR_IF_ERR_EXIT
        (lt_keyed_fs_key_write_fa_tree_create(unit, sid,
                                              opcode,
                                              op_md_ptr,
                                              &key_write_tree));
    fa_node_roots[0] = key_write_tree;

    /*-----------------
     * Tree_1
     */

    /* [Root1]: Nothing to do (used for chaining nodes) */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  NULL,
                                  NULL,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &root1_node));
    fa_node_roots[1] = root1_node;

    /* [Value_Write_Fs]: Process value writes field selections */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fs_value_write_cluster_create(unit, sid,
                                                 opcode,
                                                 BCMLTM_NODE_SIDE_LEFT,
                                                 root1_node,
                                                 op_md_ptr,
                                                 &value_write_fs_node));

    /* [RO_Check]: Verify no read-only fields present in input */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_read_only_input_check,
                                  (void *)ro_list,
                                  BCMLTM_NODE_SIDE_LEFT,
                                  value_write_fs_node,
                                  op_md_ptr,
                                  &ro_check_node));

    /*
     * [Wide_Value]: Verify all or none of wide value field elements
     * present in input
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_wide_field_check,
                                  (void *)wide_list,
                                  BCMLTM_NODE_SIDE_RIGHT,
                                  value_write_fs_node,
                                  op_md_ptr,
                                  &wide_field_node));

    /* Set returning pointer */
    op_md_ptr->fa_node_roots = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy trees */
        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, fa_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->fa_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create EE tree roots for INSERT on Keyed LT.
 *
 * This function creates the EE tree roots for an INSERT operation
 * on given Keyed logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_keyed_fs_ee_node_roots_insert_create(int unit,
                                        bcmlrd_sid_t sid,
                                        bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int alloc_size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *lookup_node = NULL;
    bcmltm_node_t *insert_node = NULL;
    const bcmltm_pt_keyed_op_info_t *lookup_info = NULL;
    const bcmltm_pt_keyed_op_info_t *insert_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    alloc_size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (alloc_size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, alloc_size, "bcmltmKeyedInsertEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, alloc_size);

    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_pt_keyed_get(unit, sid, BCMPTM_OP_LOOKUP,
                                   &lookup_info));
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_pt_keyed_get(unit, sid, BCMPTM_OP_INSERT,
                                   &insert_info));

    /*-----------------
     * Tree_0
     */

    /* [Lookup]: Execute PTM lookup operation (entry should not exist) */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_create(unit,
                                  bcmltm_ee_node_model_keyed_lookup_not_found,
                                  (void *)lookup_info,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &lookup_node));
    ee_node_roots[0] = lookup_node;

    /*-----------------
     * Tree_1
     */

    /* [Insert]: Execute PTM insert operation */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_create(unit,
                                  bcmltm_ee_node_model_keyed_insert,
                                  (void *)insert_info,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &insert_node));
    ee_node_roots[1] = insert_node;

    /* Set returning pointer */
    op_md_ptr->ee_node_roots = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy trees */
        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, ee_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->ee_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create INSERT operation metadata for Keyed LT.
 *
 * This routine creates the operation metadata for given table ID
 * to use for the INSERT opcode on Keyed LTs.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_keyed_fs_op_insert_create(int unit,
                             bcmlrd_sid_t sid,
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
        (lt_keyed_fs_fa_node_roots_insert_create(unit, sid, op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_keyed_fs_ee_node_roots_insert_create(unit, sid, op_md));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(op_md);
        *op_md_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create FA tree roots for LOOKUP on Keyed LT.
 *
 * This function creates the FA tree roots for a LOOKUP operation
 * on given Keyed logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_keyed_fs_fa_node_roots_lookup_create(int unit,
                                        bcmlrd_sid_t sid,
                                        bcmltm_lt_op_md_t *op_md_ptr)
{
    bcmlt_opcode_t opcode = BCMLT_OPCODE_LOOKUP;
    unsigned int alloc_size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *key_write_tree = NULL;
    bcmltm_node_t *root1_node = NULL;
    bcmltm_node_t *value_read_fs_node = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    alloc_size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (alloc_size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, alloc_size, "bcmltmKeyedLookupFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, alloc_size);

    /*-----------------
     * Tree_0
     */

    /* [Key_Write]: Process input key fields */
    SHR_IF_ERR_EXIT
        (lt_keyed_fs_key_write_fa_tree_create(unit, sid,
                                              opcode,
                                              op_md_ptr,
                                              &key_write_tree));
    fa_node_roots[0] = key_write_tree;

    /*-----------------
     * Tree_1
     */

    /* [Root1]: Nothing to do (used for chaining nodes) */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  NULL,
                                  NULL,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &root1_node));
    fa_node_roots[1] = root1_node;

    /* [Value_Read_Fs]: Process value reads field selections */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fs_value_read_cluster_create(unit, sid,
                                                opcode,
                                                BCMLTM_NODE_SIDE_LEFT,
                                                root1_node,
                                                op_md_ptr,
                                                &value_read_fs_node));

    /* Set returning pointer */
    op_md_ptr->fa_node_roots = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy trees */
        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, fa_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->fa_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create EE tree roots for LOOKUP on Keyed LT.
 *
 * This function creates the EE tree roots for a LOOKUP operation
 * on given Keyed logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_keyed_fs_ee_node_roots_lookup_create(int unit,
                                        bcmlrd_sid_t sid,
                                        bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int alloc_size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *lookup_node = NULL;
    const bcmltm_pt_keyed_op_info_t *lookup_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    alloc_size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (alloc_size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, alloc_size, "bcmltmKeyedLookupEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, alloc_size);

    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_pt_keyed_get(unit, sid, BCMPTM_OP_LOOKUP,
                                   &lookup_info));

    /*-----------------
     * Tree_0
     */

    /* [Lookup]: Execute PTM lookup operation */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_create(unit,
                                  bcmltm_ee_node_model_keyed_lookup,
                                  (void *)lookup_info,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &lookup_node));
    ee_node_roots[0] = lookup_node;

    /*-----------------
     * Tree_1
     */

    /* [NULL]: No actions */
    ee_node_roots[1] = NULL;  /* Not used */

    /* Set returning pointer */
    op_md_ptr->ee_node_roots = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy trees */
        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, ee_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->ee_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create LOOKUP operation metadata for Keyed LT.
 *
 * This routine creates the operation metadata for given table ID
 * to use for the LOOKUP opcode on Keyed LTs.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_keyed_fs_op_lookup_create(int unit,
                             bcmlrd_sid_t sid,
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
        (lt_keyed_fs_fa_node_roots_lookup_create(unit, sid, op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_keyed_fs_ee_node_roots_lookup_create(unit, sid, op_md));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(op_md);
        *op_md_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create FA tree roots for DELETE on Keyed LT.
 *
 * This function creates the FA tree roots for a DELETE operation
 * on given Keyed logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_keyed_fs_fa_node_roots_delete_create(int unit,
                                        bcmlrd_sid_t sid,
                                        bcmltm_lt_op_md_t *op_md_ptr)
{
    bcmlt_opcode_t opcode = BCMLT_OPCODE_DELETE;
    unsigned int alloc_size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *key_write_tree = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    alloc_size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (alloc_size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, alloc_size, "bcmltmKeyedDeleteFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, alloc_size);

    /*-----------------
     * Tree_0
     */

    /* [Key_Write]: Process input key fields */
    SHR_IF_ERR_EXIT
        (lt_keyed_fs_key_write_fa_tree_create(unit, sid,
                                              opcode,
                                              op_md_ptr,
                                              &key_write_tree));
    fa_node_roots[0] = key_write_tree;

    /* Set returning pointer */
    op_md_ptr->fa_node_roots = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy trees */
        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, fa_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->fa_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create EE tree roots for DELETE on Keyed LT.
 *
 * This function creates the EE tree roots for a DELETE operation
 * on given Keyed logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_keyed_fs_ee_node_roots_delete_create(int unit,
                                        bcmlrd_sid_t sid,
                                        bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int alloc_size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *delete_node = NULL;
    const bcmltm_pt_keyed_op_info_t *delete_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    alloc_size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (alloc_size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, alloc_size, "bcmltmKeyedDeleteEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, alloc_size);

    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_pt_keyed_get(unit, sid, BCMPTM_OP_DELETE,
                                   &delete_info));

    /*-----------------
     * Tree_0
     */

    /* [Delete]: Execute PTM delete operation */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_create(unit,
                                  bcmltm_ee_node_model_keyed_delete,
                                  (void *)delete_info,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &delete_node));
    ee_node_roots[0] = delete_node;

    /* Set returning pointer */
    op_md_ptr->ee_node_roots = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy trees */
        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, ee_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->ee_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create DELETE operation metadata for Keyed LT.
 *
 * This routine creates the operation metadata for given table ID
 * to use for the DELETE opcode on Keyed LTs.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_keyed_fs_op_delete_create(int unit,
                             bcmlrd_sid_t sid,
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
        (lt_keyed_fs_fa_node_roots_delete_create(unit, sid, op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_keyed_fs_ee_node_roots_delete_create(unit, sid, op_md));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(op_md);
        *op_md_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create FA tree roots for UPDATE on Keyed LT.
 *
 * This function creates the FA tree roots for an UPDATE operation
 * on given Keyed logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_keyed_fs_fa_node_roots_update_create(int unit,
                                        bcmlrd_sid_t sid,
                                        bcmltm_lt_op_md_t *op_md_ptr)
{
    bcmlt_opcode_t opcode = BCMLT_OPCODE_UPDATE;
    unsigned int alloc_size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *key_write_tree = NULL;
    bcmltm_node_t *root1_node = NULL;
    bcmltm_node_t *value_write_fs_node = NULL;
    bcmltm_node_t *pt_clear_node = NULL;
    bcmltm_node_t *value_read_fs_node = NULL;
    bcmltm_node_t *value_def_node = NULL;
    bcmltm_node_t *ro_check_node = NULL;
    bcmltm_node_t *wide_field_node = NULL;
    const bcmltm_field_select_mapping_t *apic_in_all_values = NULL;
    const bcmltm_ro_field_list_t *ro_list = NULL;
    const bcmltm_wide_field_list_t *wide_list = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    alloc_size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (alloc_size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, alloc_size, "bcmltmKeyedUpdateFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, alloc_size);

    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_apic_all_value_fields_get(unit, sid,
                                                BCMLTM_FIELD_DIR_IN,
                                                &apic_in_all_values));
    SHR_IF_ERR_EXIT
        (bcmltm_db_read_only_list_get(unit, sid, &ro_list));
    SHR_IF_ERR_EXIT
        (bcmltm_db_wide_field_list_get(unit, sid, &wide_list));

    /*-----------------
     * Tree_0
     */

    /* [Key_Write]: Process input key fields */
    SHR_IF_ERR_EXIT
        (lt_keyed_fs_key_write_fa_tree_create(unit, sid,
                                              opcode,
                                              op_md_ptr,
                                              &key_write_tree));
    fa_node_roots[0] = key_write_tree;

    /*-----------------
     * Tree_1
     */

    /* [Root1]: Nothing to do (used for chaining nodes) */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  NULL,
                                  NULL,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &root1_node));
    fa_node_roots[1] = root1_node;

    /* [Value_Write_Fs]: Process value writes field selections */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fs_value_write_cluster_create(unit, sid,
                                                 opcode,
                                                 BCMLTM_NODE_SIDE_RIGHT,
                                                 root1_node,
                                                 op_md_ptr,
                                                 &value_write_fs_node));

    /* [PT_Clear]: PT entry bulk default set */
    
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_nc_opt_create(unit,
                                         bcmltm_fa_node_pt_clear,
                                         NULL,
                                         BCMLTM_NODE_SIDE_LEFT,
                                         value_write_fs_node,
                                         op_md_ptr,
                                         &pt_clear_node));

    /* [Value_Read_Fs]: Process value reads field selections */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fs_value_read_cluster_create(unit, sid,
                                                opcode,
                                                BCMLTM_NODE_SIDE_LEFT,
                                                root1_node,
                                                op_md_ptr,
                                                &value_read_fs_node));

    /* [Value_Def]: API cache value default fields initialization */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_api_wb_defaults,
                                  (void *)apic_in_all_values,
                                  BCMLTM_NODE_SIDE_LEFT,
                                  value_read_fs_node,
                                  op_md_ptr,
                                  &value_def_node));

    /* [RO_Check]: Verify no read-only fields present in input */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_read_only_input_check,
                                  (void *)ro_list,
                                  BCMLTM_NODE_SIDE_LEFT,
                                  value_def_node,
                                  op_md_ptr,
                                  &ro_check_node));

    /*
     * [Wide_Value]: Verify all or none of wide value field elements
     * present in input
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_wide_field_check,
                                  (void *)wide_list,
                                  BCMLTM_NODE_SIDE_RIGHT,
                                  value_def_node,
                                  op_md_ptr,
                                  &wide_field_node));

    /* Set returning pointer */
    op_md_ptr->fa_node_roots = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy trees */
        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, fa_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->fa_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create EE tree roots for UPDATE on Keyed LT.
 *
 * This function creates the EE tree roots for an UPDATE operation
 * on given Keyed logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_keyed_fs_ee_node_roots_update_create(int unit,
                                        bcmlrd_sid_t sid,
                                        bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int alloc_size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *lookup_node = NULL;
    bcmltm_node_t *insert_node = NULL;
    const bcmltm_pt_keyed_op_info_t *lookup_info = NULL;
    const bcmltm_pt_keyed_op_info_t *insert_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    alloc_size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (alloc_size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, alloc_size, "bcmltmKeyedUpdateEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, alloc_size);

    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_pt_keyed_get(unit, sid, BCMPTM_OP_LOOKUP,
                                   &lookup_info));
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_pt_keyed_get(unit, sid, BCMPTM_OP_INSERT,
                                   &insert_info));

    /*-----------------
     * Tree_0
     */

    /* [Lookup]: Execute PTM lookup operation */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_create(unit,
                                  bcmltm_ee_node_model_keyed_lookup,
                                  (void *)lookup_info,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &lookup_node));
    ee_node_roots[0] = lookup_node;

    /*-----------------
     * Tree_1
     */

    /* [Insert]: Execute PTM insert operation */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_create(unit,
                                  bcmltm_ee_node_model_keyed_insert,
                                  (void *)insert_info,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &insert_node));
    ee_node_roots[1] = insert_node;

    /* Set returning pointer */
    op_md_ptr->ee_node_roots = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy trees */
        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, ee_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->ee_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create UPDATE operation metadata for Keyed LT.
 *
 * This routine creates the operation metadata for given table ID
 * to use for the UPDATE opcode on Keyed LTs.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_keyed_fs_op_update_create(int unit,
                             bcmlrd_sid_t sid,
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
        (lt_keyed_fs_fa_node_roots_update_create(unit, sid, op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_keyed_fs_ee_node_roots_update_create(unit, sid, op_md));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(op_md);
        *op_md_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create FA tree roots for TRAVERSE on Keyed LT.
 *
 * This function creates the FA tree roots for a TRAVERSE operation
 * on given Keyed logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_keyed_fs_fa_node_roots_traverse_create(int unit,
                                          bcmlrd_sid_t sid,
                                          bcmltm_lt_op_md_t *op_md_ptr)
{
    bcmlt_opcode_t opcode = BCMLT_OPCODE_TRAVERSE;
    unsigned int alloc_size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *key_write_tree = NULL;
    bcmltm_node_t *root1_node = NULL;
    bcmltm_node_t *next_value_read_fs_node = NULL;
    bcmltm_node_t *next_key_read_node = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    alloc_size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (alloc_size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, alloc_size, "bcmltmKeyedTraverseFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, alloc_size);

    /*-----------------
     * Tree_0
     */

    /* [Key_Write]: Process input key fields */
    SHR_IF_ERR_EXIT
        (lt_keyed_fs_key_write_fa_tree_create(unit, sid,
                                              opcode,
                                              op_md_ptr,
                                              &key_write_tree));
    fa_node_roots[0] = key_write_tree;

    /*-----------------
     * Tree_1
     */

    /* [Root1]: Nothing to do (used for chaining nodes) */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  NULL,
                                  NULL,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &root1_node));
    fa_node_roots[1] = root1_node;

    /* [Next_Value_Read_Fs]: Process next value reads field selections */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fs_value_read_cluster_create(unit, sid,
                                                opcode,
                                                BCMLTM_NODE_SIDE_RIGHT,
                                                root1_node,
                                                op_md_ptr,
                                                &next_value_read_fs_node));

    /* [Next_Key_Read]: Process next key reads field selections */
    SHR_IF_ERR_EXIT
        (lt_keyed_fs_key_read_fa_cluster_create(unit, sid,
                                                opcode,
                                                BCMLTM_NODE_SIDE_LEFT,
                                                root1_node,
                                                op_md_ptr,
                                                &next_key_read_node));

    /* Set returning pointer */
    op_md_ptr->fa_node_roots = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy trees */
        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, fa_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->fa_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create EE tree roots for TRAVERSE on Keyed LT.
 *
 * This function creates the EE tree roots for a TRAVERSE operation
 * on given Keyed logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_keyed_fs_ee_node_roots_traverse_create(int unit,
                                          bcmlrd_sid_t sid,
                                          bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int alloc_size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *traverse_node = NULL;
    const bcmltm_pt_keyed_op_info_t *traverse_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    alloc_size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (alloc_size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, alloc_size, "bcmltmKeyedTraverseEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, alloc_size);

    /* Gather all metadata node cookies */
    /* Note that Get-First and Get-Next use same node cookie */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_pt_keyed_get(unit, sid, BCMPTM_OP_GET_FIRST,
                                   &traverse_info));

    /*-----------------
     * Tree_0
     */

    /* [Traverse]: Execute PTM traverse operation */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_create(unit,
                                  bcmltm_ee_node_model_keyed_traverse,
                                  (void *)traverse_info,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &traverse_node));
    ee_node_roots[0] = traverse_node;

    /*-----------------
     * Tree_1
     */

    /* [NULL]: No actions */
    ee_node_roots[1] = NULL;  /* Not used */

    /* Set returning pointer */
    op_md_ptr->ee_node_roots = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy trees */
        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, ee_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->ee_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create TRAVERSE operation metadata for Keyed LT.
 *
 * This routine creates the operation metadata for given table ID
 * to use for the TRAVERSE opcode on Keyed LTs.
 *
 * This routine populates the required metadata information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_keyed_fs_op_traverse_create(int unit,
                               bcmlrd_sid_t sid,
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
        (lt_keyed_fs_fa_node_roots_traverse_create(unit, sid, op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_keyed_fs_ee_node_roots_traverse_create(unit, sid, op_md));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(op_md);
        *op_md_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*
 * LT Metadata Function Vector for Direct Mapped Keyed Field Select LT.
 */
static bcmltm_md_lt_drv_t lt_keyed_fs_drv = {
    /* op_destroy */            bcmltm_md_op_destroy,
    /* op_insert_create */      lt_keyed_fs_op_insert_create,
    /* op_lookup_create */      lt_keyed_fs_op_lookup_create,
    /* op_delete_create */      lt_keyed_fs_op_delete_create,
    /* op_update_create */      lt_keyed_fs_op_update_create,
    /* op_traverse_create */    lt_keyed_fs_op_traverse_create,
};

/*******************************************************************************
 * Public functions
 */

const bcmltm_md_lt_drv_t *
bcmltm_md_lt_keyed_fs_drv_get(void)
{
    return &lt_keyed_fs_drv;
}
