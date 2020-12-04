/*! \file bcmltm_md_lt_index_fs.c
 *
 * Logical Table Manager - Index Field Select Logical Table.
 *
 * This file contains implementation to support metadata construction
 * for Direct Mapped Index Logical Tables with Field Selections.
 *
 * This implementation is based on the following field selection premises:
 * - A field selection group of KEY selector type cannot have any
 *   parent (i.e. level should be 1).
 * - Selectees field maps can only involve value fields (thus,
 *   a key field cannot be subject to a selection).
 *
 * The coverage of this Index Field Select Logical Table driver
 * includes these variations:
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
#include <bcmltm/bcmltm_util_internal.h>
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
 * \param [in] sid Logical table ID.
 * \param [in] attr Table attributes.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_fs_fa_node_roots_insert_create(int unit,
                                        bcmlrd_sid_t sid,
                                        const bcmltm_table_attr_t *attr,
                                        bcmltm_lt_op_md_t *op_md_ptr)
{
    bcmlt_opcode_t opcode = BCMLT_OPCODE_INSERT;
    unsigned int alloc_size;
    bool iwa = FALSE;
    bool track_index_map = FALSE;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *alloc_node = NULL;
    bcmltm_node_t *track_enc_node = NULL;
    bcmltm_node_t *key_fsm_map_node = NULL;
    bcmltm_node_t *req_key_c2p_node = NULL;
    bcmltm_node_t *fixed_key_node = NULL;
    bcmltm_node_t *key_fwd_xfrm_node = NULL;
    bcmltm_node_t *key_validate_node = NULL;
    bcmltm_node_t *req_key_a2c_node = NULL;
    bcmltm_node_t *root1_node = NULL;
    bcmltm_node_t *value_write_fs_node = NULL;
    bcmltm_node_t *ro_check_node = NULL;
    bcmltm_node_t *wide_field_node = NULL;
    bcmltm_fa_node_f track_enc_func = NULL;
    const bcmltm_track_index_t *track_index = NULL;
    const bcmltm_fixed_field_mapping_t *fixed_keys = NULL;
    const bcmltm_field_select_mapping_t *apic_in_req_keys = NULL;
    const bcmltm_field_select_mapping_t *in_req_keys = NULL;
    const bcmltm_ro_field_list_t *ro_list = NULL;
    const bcmltm_wide_field_list_t *wide_list = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    alloc_size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (alloc_size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, alloc_size, "bcmltmIndexInsertFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, alloc_size);

    /* Get table properties */
    if (BCMLTM_TABLE_ATTR_TYPE_IS_IWA(*attr)) {
        iwa = TRUE;
    }

    /* Track index: is this mapped or calculated by LTM */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_track_index_is_mapped(unit, sid,
                                            &track_index_map));

    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_track_index_get(unit, sid,
                                      &track_index));
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_fixed_key_fields_get(unit, sid,
                                           &fixed_keys));
    SHR_IF_ERR_EXIT
        (bcmltm_db_read_only_list_get(unit, sid, &ro_list));
    SHR_IF_ERR_EXIT
        (bcmltm_db_wide_field_list_get(unit, sid, &wide_list));
    if (iwa) {
        SHR_IF_ERR_EXIT
            (bcmltm_db_dm_apic_req_key_fields_get(unit, sid,
                                                  BCMLTM_FIELD_DIR_IN,
                                                  &apic_in_req_keys));
        SHR_IF_ERR_EXIT
            (bcmltm_db_dm_req_key_fields_get(unit, sid,
                                             BCMLTM_FIELD_DIR_IN,
                                             &in_req_keys));
    } else {
        SHR_IF_ERR_EXIT
            (bcmltm_db_dm_apic_key_fields_get(unit, sid,
                                              BCMLTM_FIELD_DIR_IN,
                                              &apic_in_req_keys));
        SHR_IF_ERR_EXIT
            (bcmltm_db_dm_key_fields_get(unit, sid,
                                         BCMLTM_FIELD_DIR_IN,
                                         &in_req_keys));
    }

    
    if (iwa) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Field select not supported in IwA LT: "
                              "%s(ltid=%d)\n"),
                   table_name, sid));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /*-----------------
     * Tree_0
     */

    /* [Alloc...]: Handle key allocation for IwA */
    
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  NULL,
                                  NULL,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &alloc_node));
    fa_node_roots[0] = alloc_node;

    /* [Track_Enc]: Calculate track index (No-TI-Map) */
    if (!track_index_map) {
        track_enc_func = bcmltm_fa_node_wbc_to_track;
    } /* Else: track index is supplied by map (transform) */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  track_enc_func,
                                  (void *)track_index,
                                  BCMLTM_NODE_SIDE_LEFT,
                                  alloc_node,
                                  op_md_ptr,
                                  &track_enc_node));

    /* [Key_Fsm_Map]: Key field selector value to field select map index */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fs_key_fsm_map_cluster_create(unit, sid,
                                                 BCMLTM_NODE_SIDE_RIGHT,
                                                 track_enc_node,
                                                 op_md_ptr,
                                                 &key_fsm_map_node));

    /* [Req_Key_C2P]: API cache required keys to PT params */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_api_wb_to_wbc,
                                  (void *)in_req_keys,
                                  BCMLTM_NODE_SIDE_RIGHT,
                                  key_fsm_map_node,
                                  op_md_ptr,
                                  &req_key_c2p_node));

    /* [Fixed_Key]: Set fixed key fields from LRD */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_select_fixed_fields_to_wb,
                                  (void *)fixed_keys,
                                  BCMLTM_NODE_SIDE_LEFT,
                                  req_key_c2p_node,
                                  op_md_ptr,
                                  &fixed_key_node));

    

    /* [Key_Fwd_Xfrm]: Process LTA key forward transforms */
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fs_fwd_key_cluster_create(unit, sid,
                                                  opcode, FALSE,
                                                  BCMLTM_NODE_SIDE_LEFT,
                                                  track_enc_node,
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

    /* [Req_Key_A2C]: API required keys to API cache */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_in_key_fields_to_api_wb,
                                  (void *)apic_in_req_keys,
                                  BCMLTM_NODE_SIDE_LEFT,
                                  key_validate_node,
                                  op_md_ptr,
                                  &req_key_a2c_node));

    

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
 * \brief Create EE tree roots for INSERT on Index LT.
 *
 * This function creates the EE tree roots for a INSERT operation
 * on given Index logical table.
 *
 * This routine implements the Disjoint LT design.  The resulting
 * tree can be used for LTs with full or split-entry mapped fields.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] attr Table attributes.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_fs_ee_node_roots_insert_create(int unit,
                                        bcmlrd_sid_t sid,
                                        const bcmltm_table_attr_t *attr,
                                        bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int alloc_size;
    bool shared = FALSE;
    bool interactive = FALSE;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *read_node = NULL;
    bcmltm_node_t *index_free_node = NULL;
    bcmltm_node_t *write_node = NULL;
    bcmltm_node_t *index_ins_node = NULL;
    bcmltm_node_t *gindex_ins_node = NULL;
    bcmltm_ee_node_f read_func = NULL;
    bcmltm_ee_node_f write_func = NULL;
    bcmltm_ee_node_f gindex_ins_func = NULL;
    const bcmltm_pt_list_t *pt_list = NULL;
    const bcmltm_ee_index_info_t *index_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    alloc_size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (alloc_size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, alloc_size, "bcmltmIndexInsertEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, alloc_size);

    /* Get Index table properties */
    if (BCMLTM_TABLE_ATTR_IS_SHARED(*attr)) {
        shared = TRUE;
    }
    if (BCMLTM_TABLE_ATTR_MODE_IS_INTERACTIVE(*attr)) {
        interactive = TRUE;
    }

    /*
     * Get EE node functions for corresponding PTM interface
     * based on table operating mode.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_ptm_index_func_get(interactive,
                                              &read_func,
                                              &write_func));

    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_pt_list_get(unit, sid, &pt_list));
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_ee_lt_index_get(unit, sid, &index_info));

    /*-----------------
     * Tree_0
     */

    /* [Read]: Execute PTM read operation */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_create(unit,
                                  read_func,
                                  (void *)pt_list,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &read_node));
    ee_node_roots[0] = read_node;

    /* [Index_Free]: LTM resource operation index check free */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_create(unit,
                                  bcmltm_ee_node_model_index_free,
                                  (void *)index_info,
                                  BCMLTM_NODE_SIDE_LEFT,
                                  read_node,
                                  op_md_ptr,
                                  &index_free_node));

    /*-----------------
     * Tree_1
     */

    /* [Write]: Execute PTM write operation */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_create(unit,
                                  write_func,
                                  (void *)pt_list,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &write_node));
    ee_node_roots[1] = write_node;

    /* [Index_Ins]: LTM resource operation index insert */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_create(unit,
                                  bcmltm_ee_node_model_index_insert,
                                  (void *)index_info,
                                  BCMLTM_NODE_SIDE_LEFT,
                                  write_node,
                                  op_md_ptr,
                                  &index_ins_node));

    /* [GIndex_Ins]: LTM resource operation global index insert (Shared) */
    if (shared) {
        gindex_ins_func = bcmltm_ee_node_global_index_insert;
    } /* Else: not applicable */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_create(unit,
                                  gindex_ins_func,
                                  (void *)index_info,
                                  BCMLTM_NODE_SIDE_LEFT,
                                  index_ins_node,
                                  op_md_ptr,
                                  &gindex_ins_node));

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
 * \brief Create INSERT operation metadata for Index LT.
 *
 * This routine creates the operation metadata for given table ID
 * to use for the INSERT opcode on Index LTs.
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
lt_index_fs_op_insert_create(int unit,
                             bcmlrd_sid_t sid,
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
        (lt_index_fs_fa_node_roots_insert_create(unit, sid, table_attr,
                                                 op_md));
    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_index_fs_ee_node_roots_insert_create(unit, sid, table_attr,
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
 * \param [in] sid Logical table ID.
 * \param [in] attr Table attributes.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_fs_fa_node_roots_lookup_create(int unit,
                                        bcmlrd_sid_t sid,
                                        const bcmltm_table_attr_t *attr,
                                        bcmltm_lt_op_md_t *op_md_ptr)
{
    bcmlt_opcode_t opcode = BCMLT_OPCODE_LOOKUP;
    unsigned int alloc_size;
    bool track_index_map = FALSE;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *track_enc_node = NULL;
    bcmltm_node_t *key_fsm_map_node = NULL;
    bcmltm_node_t *key_c2p_node = NULL;
    bcmltm_node_t *fixed_key_node = NULL;
    bcmltm_node_t *key_fwd_xfrm_node = NULL;
    bcmltm_node_t *key_validate_node = NULL;
    bcmltm_node_t *key_a2c_node = NULL;
    bcmltm_node_t *root1_node = NULL;
    bcmltm_node_t *value_read_fs_node = NULL;
    bcmltm_fa_node_f track_enc_func = NULL;
    const bcmltm_track_index_t *track_index = NULL;
    const bcmltm_fixed_field_mapping_t *fixed_keys = NULL;
    const bcmltm_field_select_mapping_t *apic_in_keys = NULL;
    const bcmltm_field_select_mapping_t *in_keys = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    alloc_size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (alloc_size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, alloc_size, "bcmltmIndexLookupFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, alloc_size);

    /* Track index: is this mapped or calculated by LTM */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_track_index_is_mapped(unit, sid, &track_index_map));

    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_track_index_get(unit, sid,
                                      &track_index));
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

    /*-----------------
     * Tree_0
     */

    /* [Track_Enc]: Calculate track index (No-TI-Map) */
    if (!track_index_map) {
        track_enc_func = bcmltm_fa_node_wbc_to_track;
    } /* Else: track index is supplied by map (transform) */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  track_enc_func,
                                  (void *)track_index,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &track_enc_node));
    fa_node_roots[0] = track_enc_node;

    /* [Key_Fsm_Map]: Key field selector value to field select map index */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fs_key_fsm_map_cluster_create(unit, sid,
                                                 BCMLTM_NODE_SIDE_RIGHT,
                                                 track_enc_node,
                                                 op_md_ptr,
                                                 &key_fsm_map_node));

    /* [Key_C2P]: API cache keys to PT params */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_api_wb_to_wbc,
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
                                  BCMLTM_NODE_SIDE_LEFT,
                                  key_c2p_node,
                                  op_md_ptr,
                                  &fixed_key_node));

    /* [Key_Fwd_Xfrm]: Process LTA key forward transforms */
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fs_fwd_key_cluster_create(unit, sid,
                                                  opcode, FALSE,
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
                                  track_enc_node,
                                  op_md_ptr,
                                  &key_a2c_node));


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
 * \brief Create EE tree roots for LOOKUP on Index LT.
 *
 * This function creates the EE tree roots for a LOOKUP operation
 * on given Index logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] attr Table attributes.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_fs_ee_node_roots_lookup_create(int unit,
                                        bcmlrd_sid_t sid,
                                        const bcmltm_table_attr_t *attr,
                                        bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int alloc_size;
    bool interactive = FALSE;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *read_node = NULL;
    bcmltm_node_t *index_exist_node = NULL;
    bcmltm_ee_node_f read_func = NULL;
    const bcmltm_pt_list_t *pt_list = NULL;
    const bcmltm_ee_index_info_t *index_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    alloc_size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (alloc_size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, alloc_size, "bcmltmIndexLookupEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, alloc_size);

    /* Get Index table properties */
    if (BCMLTM_TABLE_ATTR_MODE_IS_INTERACTIVE(*attr)) {
        interactive = TRUE;
    }

    /*
     * Get EE node functions for corresponding PTM interface
     * based on table operating mode.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_ptm_index_func_get(interactive,
                                              &read_func,
                                              NULL));
    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_pt_list_get(unit, sid, &pt_list));
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_ee_lt_index_get(unit, sid, &index_info));

    /*-----------------
     * Tree_0
     */

    /* [Read]: Execute PTM read operation */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_create(unit,
                                  read_func,
                                  (void *)pt_list,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &read_node));
    ee_node_roots[0] = read_node;

    /* [Index_Exist]: LTM resource operation index check exist */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_create(unit,
                                  bcmltm_ee_node_model_index_exists,
                                  (void *)index_info,
                                  BCMLTM_NODE_SIDE_LEFT,
                                  read_node,
                                  op_md_ptr,
                                  &index_exist_node));

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
 * \brief Create LOOKUP operation metadata for Index LT.
 *
 * This routine creates the operation metadata for given table ID
 * to use for the LOOKUP opcode on Index LTs.
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
lt_index_fs_op_lookup_create(int unit,
                             bcmlrd_sid_t sid,
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
        (lt_index_fs_fa_node_roots_lookup_create(unit, sid, table_attr,
                                                 op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_index_fs_ee_node_roots_lookup_create(unit, sid, table_attr,
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
 * \param [in] sid Logical table ID.
 * \param [in] attr Table attributes.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_fs_fa_node_roots_delete_create(int unit,
                                        bcmlrd_sid_t sid,
                                        const bcmltm_table_attr_t *attr,
                                        bcmltm_lt_op_md_t *op_md_ptr)
{
    bcmlt_opcode_t opcode = BCMLT_OPCODE_DELETE;
    unsigned int alloc_size;
    bool track_index_map = FALSE;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *track_enc_node = NULL;
    bcmltm_node_t *key_fsm_map_node = NULL;
    bcmltm_node_t *key_c2p_node = NULL;
    bcmltm_node_t *fixed_key_node = NULL;
    bcmltm_node_t *key_fwd_xfrm_node = NULL;
    bcmltm_node_t *key_validate_node = NULL;
    bcmltm_node_t *key_a2c_node = NULL;
    bcmltm_node_t *pt_clear_node = NULL;
    bcmltm_node_t *value_write_fs_node = NULL;
    bcmltm_node_t *value_def_node = NULL;
    bcmltm_fa_node_f track_enc_func = NULL;
    const bcmltm_track_index_t *track_index = NULL;
    const bcmltm_fixed_field_mapping_t *fixed_keys = NULL;
    const bcmltm_field_select_mapping_t *apic_in_keys = NULL;
    const bcmltm_field_select_mapping_t *in_keys = NULL;
    const bcmltm_field_select_mapping_t *apic_in_all_values = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    alloc_size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (alloc_size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, alloc_size, "bcmltmIndexDeleteFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, alloc_size);

    /* Track index: is this mapped or calculated by LTM */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_track_index_is_mapped(unit, sid, &track_index_map));

    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_track_index_get(unit, sid,
                                      &track_index));
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
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_apic_all_value_fields_get(unit, sid,
                                                BCMLTM_FIELD_DIR_IN,
                                                &apic_in_all_values));

    /*-----------------
     * Tree_0
     */

    /* [Track_Enc]: Calculate track index (No-TI-Map) */
    if (!track_index_map) {
        track_enc_func = bcmltm_fa_node_wbc_to_track;
    } /* Else: track index is supplied by map (transform) */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  track_enc_func,
                                  (void *)track_index,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &track_enc_node));
    fa_node_roots[0] = track_enc_node;

    /* [Key_Fsm_Map]: Key field selector value to field select map index */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fs_key_fsm_map_cluster_create(unit, sid,
                                                 BCMLTM_NODE_SIDE_RIGHT,
                                                 track_enc_node,
                                                 op_md_ptr,
                                                 &key_fsm_map_node));

    /* [Key_C2P]: API cache keys to PT params */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_api_wb_to_wbc,
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
                                  BCMLTM_NODE_SIDE_LEFT,
                                  key_c2p_node,
                                  op_md_ptr,
                                  &fixed_key_node));

    /* [Key_Fwd_Xfrm]: Process LTA key forward transforms */
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fs_fwd_key_cluster_create(unit, sid,
                                                  opcode, FALSE,
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
                                  track_enc_node,
                                  op_md_ptr,
                                  &key_a2c_node));

    /*-----------------
     * Tree_1
     */

    /* [PT_Clear]: PT entry bulk default set */
    
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_nc_opt_create(unit,
                                         bcmltm_fa_node_pt_clear,
                                         NULL,
                                         BCMLTM_NODE_SIDE_ROOT,
                                         NULL,
                                         op_md_ptr,
                                         &pt_clear_node));
    fa_node_roots[1] = pt_clear_node;

    /* [Value_Write_Fs]: Process value writes field selections */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fs_value_write_cluster_create(unit, sid,
                                                 opcode,
                                                 BCMLTM_NODE_SIDE_RIGHT,
                                                 pt_clear_node,
                                                 op_md_ptr,
                                                 &value_write_fs_node));

    /* [Value_Def]: API cache value default fields initialization */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_api_wb_defaults,
                                  (void *)apic_in_all_values,
                                  BCMLTM_NODE_SIDE_LEFT,
                                  pt_clear_node,
                                  op_md_ptr,
                                  &value_def_node));

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
 * \brief Create EE tree roots for DELETE on Index LT.
 *
 * This function creates the EE tree roots for a DELETE operation
 * on given Index logical table.
 *
 * This routine implements the Disjoint LT design.  The resulting
 * tree can be used for LTs with full or split-entry mapped fields.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] attr Table attributes.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_fs_ee_node_roots_delete_create(int unit,
                                        bcmlrd_sid_t sid,
                                        const bcmltm_table_attr_t *attr,
                                        bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int alloc_size;
    bool shared = FALSE;
    bool interactive = FALSE;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *read_node = NULL;
    bcmltm_node_t *index_exist_node = NULL;
    bcmltm_node_t *write_node = NULL;
    bcmltm_node_t *gindex_del_node = NULL;
    bcmltm_node_t *index_del_node = NULL;
    bcmltm_ee_node_f read_func = NULL;
    bcmltm_ee_node_f write_func = NULL;
    bcmltm_ee_node_f gindex_del_func = NULL;
    const bcmltm_pt_list_t *pt_list = NULL;
    const bcmltm_ee_index_info_t *index_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    alloc_size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (alloc_size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, alloc_size, "bcmltmIndexDeleteEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, alloc_size);

    /* Get Index table properties */
    if (BCMLTM_TABLE_ATTR_IS_SHARED(*attr)) {
        shared = TRUE;
    }
    if (BCMLTM_TABLE_ATTR_MODE_IS_INTERACTIVE(*attr)) {
        interactive = TRUE;
    }

    /*
     * Get EE node functions for corresponding PTM interface
     * based on table operating mode.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_ptm_index_func_get(interactive,
                                              &read_func,
                                              &write_func));
    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_pt_list_get(unit, sid, &pt_list));
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_ee_lt_index_get(unit, sid, &index_info));

    /*-----------------
     * Tree_0
     */

    /* [Read]: Execute PTM read operation */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_create(unit,
                                  read_func,
                                  (void *)pt_list,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &read_node));
    ee_node_roots[0] = read_node;

    /* [Index_Exist]: LTM resource operation index check exist */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_create(unit,
                                  bcmltm_ee_node_model_index_exists,
                                  (void *)index_info,
                                  BCMLTM_NODE_SIDE_LEFT,
                                  read_node,
                                  op_md_ptr,
                                  &index_exist_node));

    /*-----------------
     * Tree_1
     */

    /* [Write]: Execute PTM write operation */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_create(unit,
                                  write_func,
                                  (void *)pt_list,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &write_node));
    ee_node_roots[1] = write_node;

    /* [GIndex_Del]: LTM resource operation global index delete (Shared) */
    if (shared) {
        gindex_del_func = bcmltm_ee_node_global_index_delete;
    } /* Else: not applicable */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_create(unit,
                                  gindex_del_func,
                                  (void *)index_info,
                                  BCMLTM_NODE_SIDE_RIGHT,
                                  write_node,
                                  op_md_ptr,
                                  &gindex_del_node));

    /* [Index_Del]: LTM resource operation index delete */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_create(unit,
                                  bcmltm_ee_node_model_index_delete,
                                  (void *)index_info,
                                  BCMLTM_NODE_SIDE_LEFT,
                                  write_node,
                                  op_md_ptr,
                                  &index_del_node));

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
 * \brief Create DELETE operation metadata for Index LT.
 *
 * This routine creates the operation metadata for given table ID
 * to use for the DELETE opcode on Index LTs.
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
lt_index_fs_op_delete_create(int unit,
                             bcmlrd_sid_t sid,
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
        (lt_index_fs_fa_node_roots_delete_create(unit, sid, table_attr,
                                                 op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_index_fs_ee_node_roots_delete_create(unit, sid, table_attr,
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
 * \param [in] sid Logical table ID.
 * \param [in] attr Table attributes.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_fs_fa_node_roots_update_create(int unit,
                                        bcmlrd_sid_t sid,
                                        const bcmltm_table_attr_t *attr,
                                        bcmltm_lt_op_md_t *op_md_ptr)
{
    bcmlt_opcode_t opcode = BCMLT_OPCODE_UPDATE;
    unsigned int alloc_size;
    bool track_index_map = FALSE;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *track_enc_node = NULL;
    bcmltm_node_t *key_fsm_map_node = NULL;
    bcmltm_node_t *key_c2p_node = NULL;
    bcmltm_node_t *fixed_key_node = NULL;
    bcmltm_node_t *key_fwd_xfrm_node = NULL;
    bcmltm_node_t *key_validate_node = NULL;
    bcmltm_node_t *key_a2c_node = NULL;
    bcmltm_node_t *root1_node = NULL;
    bcmltm_node_t *value_write_fs_node = NULL;
    bcmltm_node_t *pt_clear_node = NULL;
    bcmltm_node_t *value_read_fs_node = NULL;
    bcmltm_node_t *value_def_node = NULL;
    bcmltm_node_t *ro_check_node = NULL;
    bcmltm_node_t *wide_field_node = NULL;
    bcmltm_fa_node_f track_enc_func = NULL;
    const bcmltm_track_index_t *track_index = NULL;
    const bcmltm_fixed_field_mapping_t *fixed_keys = NULL;
    const bcmltm_field_select_mapping_t *apic_in_keys = NULL;
    const bcmltm_field_select_mapping_t *in_keys = NULL;
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
    SHR_ALLOC(fa_node_roots, alloc_size, "bcmltmIndexUpdateFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, alloc_size);

    /* Track index: is this mapped or calculated by LTM */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_track_index_is_mapped(unit, sid, &track_index_map));

    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_track_index_get(unit, sid,
                                      &track_index));
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_fixed_key_fields_get(unit, sid,
                                           &fixed_keys));
    SHR_IF_ERR_EXIT
        (bcmltm_db_read_only_list_get(unit, sid, &ro_list));
    SHR_IF_ERR_EXIT
        (bcmltm_db_wide_field_list_get(unit, sid, &wide_list));
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_apic_key_fields_get(unit, sid,
                                          BCMLTM_FIELD_DIR_IN,
                                          &apic_in_keys));
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_key_fields_get(unit, sid,
                                     BCMLTM_FIELD_DIR_IN,
                                     &in_keys));
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_apic_all_value_fields_get(unit, sid,
                                                BCMLTM_FIELD_DIR_IN,
                                                &apic_in_all_values));

    /*-----------------
     * Tree_0
     */

    /* [Track_Enc]: Calculate track index (No-TI-Map) */
    if (!track_index_map) {
        track_enc_func = bcmltm_fa_node_wbc_to_track;
    } /* Else: track index is supplied by map (transform) */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  track_enc_func,
                                  (void *)track_index,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &track_enc_node));
    fa_node_roots[0] = track_enc_node;

    /* [Key_Fsm_Map]: Key field selector value to field select map index */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fs_key_fsm_map_cluster_create(unit, sid,
                                                 BCMLTM_NODE_SIDE_RIGHT,
                                                 track_enc_node,
                                                 op_md_ptr,
                                                 &key_fsm_map_node));

    /* [Key_C2P]: API cache keys to PT params */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_api_wb_to_wbc,
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
                                  BCMLTM_NODE_SIDE_LEFT,
                                  key_c2p_node,
                                  op_md_ptr,
                                  &fixed_key_node));

    /* [Key_Fwd_Xfrm]: Process LTA key forward transforms */
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fs_fwd_key_cluster_create(unit, sid,
                                                  opcode, FALSE,
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
                                  track_enc_node,
                                  op_md_ptr,
                                  &key_a2c_node));

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
 * \brief Create EE tree roots for UPDATE on Index LT.
 *
 * This function creates the EE tree roots for a UPDATE operation
 * on given Index logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] attr Table attributes.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_fs_ee_node_roots_update_create(int unit,
                                        bcmlrd_sid_t sid,
                                        const bcmltm_table_attr_t *attr,
                                        bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int alloc_size;
    bool interactive = FALSE;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *read_node = NULL;
    bcmltm_node_t *index_exist_node = NULL;
    bcmltm_node_t *write_node = NULL;
    bcmltm_ee_node_f read_func = NULL;
    bcmltm_ee_node_f write_func = NULL;
    const bcmltm_pt_list_t *pt_list = NULL;
    const bcmltm_ee_index_info_t *index_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    alloc_size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (alloc_size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, alloc_size, "bcmltmIndexUpdateEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, alloc_size);

    /* Get Index table properties */
    if (BCMLTM_TABLE_ATTR_MODE_IS_INTERACTIVE(*attr)) {
        interactive = TRUE;
    }

    /*
     * Get EE node functions for corresponding PTM interface
     * based on table operating mode.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_ptm_index_func_get(interactive,
                                              &read_func,
                                              &write_func));
    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_pt_list_get(unit, sid, &pt_list));
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_ee_lt_index_get(unit, sid, &index_info));

    /*-----------------
     * Tree_0
     */

    /* [Read]: Execute PTM read operation */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_create(unit,
                                  read_func,
                                  (void *)pt_list,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &read_node));
    ee_node_roots[0] = read_node;

    /* [Index_Exist]: LTM resource operation index check exist */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_create(unit,
                                  bcmltm_ee_node_model_index_exists,
                                  (void *)index_info,
                                  BCMLTM_NODE_SIDE_LEFT,
                                  read_node,
                                  op_md_ptr,
                                  &index_exist_node));

    /*-----------------
     * Tree_1
     */

    /* [Write]: Execute PTM write operation */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_create(unit,
                                  write_func,
                                  (void *)pt_list,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &write_node));
    ee_node_roots[1] = write_node;

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
 * \brief Create UPDATE operation metadata for Index LT.
 *
 * This routine creates the operation metadata for given table ID
 * to use for the UPDATE opcode on Index LTs.
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
lt_index_fs_op_update_create(int unit,
                             bcmlrd_sid_t sid,
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
        (lt_index_fs_fa_node_roots_update_create(unit, sid, table_attr,
                                                 op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_index_fs_ee_node_roots_update_create(unit, sid, table_attr,
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
 * \param [in] sid Logical table ID.
 * \param [in] attr Table attributes.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_fs_fa_node_roots_traverse_create(int unit,
                                          bcmlrd_sid_t sid,
                                          const bcmltm_table_attr_t *attr,
                                          bcmltm_lt_op_md_t *op_md_ptr)
{
    bcmlt_opcode_t opcode = BCMLT_OPCODE_TRAVERSE;
    unsigned int alloc_size;
    bool track_index_map = FALSE;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *root0_node = NULL;
    bcmltm_node_t *next_key_fsm_map_node = NULL;
    bcmltm_node_t *next_key_c2p_node = NULL;
    bcmltm_node_t *next_key_fwd_xfrm_node = NULL;
    bcmltm_node_t *next_track_p2c_node = NULL;
    bcmltm_node_t *next_track_rev_xfrm_node = NULL;
    bcmltm_node_t *next_track_dec_node = NULL;
    bcmltm_node_t *track_trv_node = NULL;
    bcmltm_node_t *track_enc_node = NULL;
    bcmltm_node_t *key_fsm_map_node = NULL;
    bcmltm_node_t *key_c2p_node = NULL;
    bcmltm_node_t *fixed_key_node = NULL;
    bcmltm_node_t *key_fwd_xfrm_node = NULL;
    bcmltm_node_t *key_validate_node = NULL;
    bcmltm_node_t *key_a2c_node = NULL;
    bcmltm_node_t *next_key_c2a_node = NULL;
    bcmltm_node_t *next_value_read_fs_node = NULL;
    bcmltm_fa_node_f track_enc_func = NULL;
    const bcmltm_track_index_t *track_index = NULL;
    const bcmltm_fixed_field_mapping_t *fixed_keys = NULL;
    const bcmltm_field_select_mapping_t *track_fields = NULL;
    const bcmltm_field_select_mapping_t *apic_in_keys = NULL;
    const bcmltm_field_select_mapping_t *apic_out_keys = NULL;
    const bcmltm_field_select_mapping_t *in_keys = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    alloc_size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (alloc_size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, alloc_size, "bcmltmIndexInsertFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, alloc_size);

    /* Track index: is this mapped or calculated by LTM */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_track_index_is_mapped(unit, sid,
                                            &track_index_map));

    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_track_index_get(unit, sid,
                                      &track_index));
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_track_to_api_fields_get(unit, sid,
                                              &track_fields));
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_fixed_key_fields_get(unit, sid,
                                           &fixed_keys));
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_apic_key_fields_get(unit, sid,
                                          BCMLTM_FIELD_DIR_IN,
                                          &apic_in_keys));
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_apic_key_fields_get(unit, sid,
                                          BCMLTM_FIELD_DIR_OUT,
                                          &apic_out_keys));
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_key_fields_get(unit, sid,
                                     BCMLTM_FIELD_DIR_IN,
                                     &in_keys));

    /*-----------------
     * Tree_0
     */
    /* [Root0]: Nothing to do (used for chaining nodes) */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  NULL,
                                  NULL,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &root0_node));
    fa_node_roots[0] = root0_node;

    /* [Next_Key_Fsm_Map]: Key field selector value to field select map index */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fs_key_fsm_map_cluster_create(unit, sid,
                                                 BCMLTM_NODE_SIDE_LEFT,
                                                 root0_node,
                                                 op_md_ptr,
                                                 &next_key_fsm_map_node));

    /* [Next_Key_C2P]: API cache next keys to PT params */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_api_wb_to_wbc,
                                  (void *)in_keys,
                                  BCMLTM_NODE_SIDE_LEFT,
                                  next_key_fsm_map_node,
                                  op_md_ptr,
                                  &next_key_c2p_node));

    /* [Next_Key_Fwd_Xfrm]: Process LTA next key forward transforms */
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fs_fwd_key_cluster_create(unit, sid,
                                                  opcode, FALSE,
                                                  BCMLTM_NODE_SIDE_RIGHT,
                                                  next_key_c2p_node,
                                                  op_md_ptr,
                                                  &next_key_fwd_xfrm_node));

    /* [Next_Track_P2C]: Copy next physical track fields to API cache  */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_wbc_to_api_wb,
                                  (void *)track_fields,
                                  BCMLTM_NODE_SIDE_RIGHT,
                                  next_key_fwd_xfrm_node,
                                  op_md_ptr,
                                  &next_track_p2c_node));

    /* [Next_Track_Rev_Xfrm]: Process LTA next track key reverse transforms */
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_track_rev_cluster_create(unit, sid,
                                                 BCMLTM_NODE_SIDE_RIGHT,
                                                 next_track_p2c_node,
                                                 op_md_ptr,
                                                 &next_track_rev_xfrm_node));

    /*
     * [Next_Track_Dec]: Decode next tracking index to tracking elements.
     *                   (No-TI-Map) only or all?
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_track_to_wbc,
                                  (void *)track_index,
                                  BCMLTM_NODE_SIDE_RIGHT,
                                  next_track_rev_xfrm_node,
                                  op_md_ptr,
                                  &next_track_dec_node));

    /* [Track_Trv]: Determine next tracking index */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_traverse_track,
                                  (void *)track_index,
                                  BCMLTM_NODE_SIDE_LEFT,
                                  next_key_c2p_node,
                                  op_md_ptr,
                                  &track_trv_node));

    /* [Track_Enc]: Calculate track index (No-TI-Map) */
    if (!track_index_map) {
        track_enc_func = bcmltm_fa_node_wbc_to_track;
    } /* Else: track index is supplied by map (transform) */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  track_enc_func,
                                  (void *)track_index,
                                  BCMLTM_NODE_SIDE_RIGHT,
                                  track_trv_node,
                                  op_md_ptr,
                                  &track_enc_node));

    /* [Key_Fsm_Map]: Key field selector value to field select map index */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fs_key_fsm_map_cluster_create(unit, sid,
                                                 BCMLTM_NODE_SIDE_RIGHT,
                                                 track_enc_node,
                                                 op_md_ptr,
                                                 &key_fsm_map_node));

    /*
     * [Key_C2P]: API cache keys to PT params (first round).
     *            The keys are the input provided with the TRAVERSE operation.
     *            Keys are required if this is not a TRAVERSE start.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_key_traverse_api_wb_to_wbc,
                                  (void *)in_keys,
                                  BCMLTM_NODE_SIDE_LEFT,
                                  track_enc_node,
                                  op_md_ptr,
                                  &key_c2p_node));

    /* [Fixed_Key]: Set fixed key fields from LRD */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_select_fixed_fields_to_wb,
                                  (void *)fixed_keys,
                                  BCMLTM_NODE_SIDE_LEFT,
                                  key_c2p_node,
                                  op_md_ptr,
                                  &fixed_key_node));

    /*
     * [Key_Fwd_Xfrm]: Process LTA key forward transforms (first round).
     *                 Check on TRAVERSE start.
     *                 The keys are the input provided with the TRAVERSE
     *                 operation.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fs_fwd_key_cluster_create(unit, sid,
                                                  opcode, TRUE,
                                                  BCMLTM_NODE_SIDE_LEFT,
                                                  track_trv_node,
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

    /*-----------------
     * Tree_1
     */

    /* [Next_Key_C2A]: Copy API cache next keys to API LT entry output */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_api_wb_to_out_key_fields,
                                  (void *)apic_out_keys,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &next_key_c2a_node));
    fa_node_roots[1] = next_key_c2a_node;

    /* [Next_Value_Read_Fs]: Process next value reads field selections */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fs_value_read_cluster_create(unit, sid,
                                                opcode,
                                                BCMLTM_NODE_SIDE_LEFT,
                                                next_key_c2a_node,
                                                op_md_ptr,
                                                &next_value_read_fs_node));

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
 * \brief Create EE tree roots for TRAVERSE on Index LT.
 *
 * This function creates the EE tree roots for a TRAVERSE operation
 * on given Index logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] attr Table attributes.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_fs_ee_node_roots_traverse_create(int unit,
                                          bcmlrd_sid_t sid,
                                          const bcmltm_table_attr_t *attr,
                                          bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int alloc_size;
    bool interactive = FALSE;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *read_node = NULL;
    bcmltm_ee_node_f read_func = NULL;
    const bcmltm_pt_list_t *pt_list = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    alloc_size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (alloc_size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, alloc_size, "bcmltmIndexTraverseEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, alloc_size);

    /* Get Index table properties */
    if (BCMLTM_TABLE_ATTR_MODE_IS_INTERACTIVE(*attr)) {
        interactive = TRUE;
    }

    /*
     * Get EE node functions for corresponding PTM interface
     * based on table operating mode.
     */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_ptm_index_func_get(interactive,
                                              &read_func,
                                              NULL));
    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_pt_list_get(unit, sid, &pt_list));

    /*-----------------
     * Tree_0
     */

    /* [Read]: Execute PTM read operation */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_create(unit,
                                  read_func,
                                  (void *)pt_list,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &read_node));
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
 * \param [in] sid Logical table ID.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_index_fs_op_traverse_create(int unit,
                               bcmlrd_sid_t sid,
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
        (lt_index_fs_fa_node_roots_traverse_create(unit, sid, table_attr,
                                                   op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (lt_index_fs_ee_node_roots_traverse_create(unit, sid, table_attr,
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
 * LT Metadata Function Vector for Direct Mapped Index Field Select LT.
 */
static bcmltm_md_lt_drv_t lt_index_fs_drv = {
    /* op_destroy */            bcmltm_md_op_destroy,
    /* op_insert_create */      lt_index_fs_op_insert_create,
    /* op_lookup_create */      lt_index_fs_op_lookup_create,
    /* op_delete_create */      lt_index_fs_op_delete_create,
    /* op_update_create */      lt_index_fs_op_update_create,
    /* op_traverse_create */    lt_index_fs_op_traverse_create,
};

/*******************************************************************************
 * Public functions
 */

const bcmltm_md_lt_drv_t *
bcmltm_md_lt_index_fs_drv_get(void)
{
    return &lt_index_fs_drv;
}

