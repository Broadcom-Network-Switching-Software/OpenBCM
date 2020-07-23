/*! \file bcmltm_md_fs.c
 *
 * Logical Table Manager - Field Selection.
 *
 * This file contains interfaces to construct the field select
 * subtrees.
 *
 * FA Field Select Cluster
 * -----------------------
 *
 * A field select cluster is composed of a number of subtrees
 * formed as follows:
 *
 *                         [Parent]
 *                             |
 *                   +---------+---------+
 *                   |                   |
 *            [FS_1 Subtree]            ===
 *                   |
 *          +--------+--------+
 *          |                 |
 *         ...               ===
 *    [FS_N Subtree]
 *
 * Where "N" is the number of field select subtrees.
 * One field select subtree is required per selection group.
 *
 * Each field select subtree is composed of a set of nodes required
 * to handle the following field map types that are part of
 * a field selection group:
 *   - Direct field maps
 *   - Fixed fields
 *   - Field transforms
 *   - Field validations.
 *
 * The order of execution in the cluster follow the same LTM
 * tree traverse order (i.e. post-order), which is:
 * FS_N subtree, FS_(N-1) subtree, ..., FS_1 subtree.
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
#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_db_internal.h>
#include <bcmltm/bcmltm_fa_tree_nodes_internal.h>

#include "bcmltm_md_op.h"
#include "bcmltm_md_lta_xfrm_fs.h"
#include "bcmltm_md_lta_validate_fs.h"
#include "bcmltm_md_fs.h"

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/*!
 * \brief Create the field select map FA subtree.
 *
 * This routine creates the field select map FA subtree for
 * the specified field selection for a given table.
 *
 * The subtree is attached to the left node of given parent node.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] fs Field selection information.
 * \param [in] parent_node Parent node for this subtree.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*fs_fsm_fa_subtree_create_f)(int unit,
                                          bcmlrd_sid_t sid,
                                          const bcmltm_db_fs_core_t *fs,
                                          bcmltm_node_t *parent_node,
                                          bcmltm_lt_op_md_t *op_md_ptr,
                                          bcmltm_node_t **next_chain);

/*!
 * \brief Create the field select value FA subtree.
 *
 * This routine creates the field select value FA subtree
 * for the specified field selection for a given table.
 *
 * The subtree is attached to the left node of given parent node.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] fs Field selection information.
 * \param [in] fsc_flags Field select cluster flags.
 * \param [in] opcode LT opcode.
 * \param [in] parent_node Parent node for this subtree.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*fs_value_fa_subtree_create_f)(int unit,
                               bcmlrd_sid_t sid,
                               const bcmltm_db_fs_core_t *fs,
                               const bcmltm_md_fs_cluster_flags_t *fsc_flags,
                               bcmlt_opcode_t opcode,
                               bcmltm_node_t *parent_node,
                               bcmltm_lt_op_md_t *op_md_ptr,
                               bcmltm_node_t **next_chain);

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Create empty anchor node.
 *
 * This routine creates an empty node that can serve as an anchor
 * to a parent node to attach subtrees.
 *
 * \param [in] unit Unit number.
 * \param [in] node_side Side of parent node to attach anchor to.
 * \param [in] parent_chain Parent node to attach anchor node to.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] next_chain Anchor node that can be used to chain cluster.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
fs_anchor_node_create(int unit,
                      bcmltm_node_side_t node_side,
                      bcmltm_node_t *parent_chain,
                      bcmltm_lt_op_md_t *op_md_ptr,
                      bcmltm_node_t **next_chain)
{
    bcmltm_node_t *anchor_node = NULL;

    SHR_FUNC_ENTER(unit);

    *next_chain = parent_chain;

    /* Create anchor node for cluster in specified side/location */
    if (node_side == BCMLTM_NODE_SIDE_LEFT) {
        anchor_node = bcmltm_tree_left_node_create(parent_chain,
                                                   (void *)NULL,
                                                   &(op_md_ptr->num_nodes));
    } else if (node_side == BCMLTM_NODE_SIDE_RIGHT) {
        anchor_node = bcmltm_tree_right_node_create(parent_chain,
                                                    (void *)NULL,
                                                    &(op_md_ptr->num_nodes));
    } else {
        anchor_node = bcmltm_tree_node_create((void *)NULL,
                                              &(op_md_ptr->num_nodes));
    }

    SHR_NULL_CHECK(anchor_node, SHR_E_MEMORY);

    *next_chain = anchor_node;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the field select map FA subtree.
 *
 * This routine creates the field select map FA subtree for
 * the specified field selection for a given table.
 *
 * The FSM subtree converts the field selector value to the
 * corresponding field select map index.  This can be used
 * to process a key field selector map index in a stage
 * different from its selected fields.
 *
 * The subtree is attached to the left node of given parent node.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] fs Field selection information.
 * \param [in] parent_node Parent node for this subtree.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
fs_fsm_map_fa_subtree_create(int unit,
                             bcmlrd_sid_t sid,
                             const bcmltm_db_fs_core_t *fs,
                             bcmltm_node_t *parent_node,
                             bcmltm_lt_op_md_t *op_md_ptr,
                             bcmltm_node_t **next_chain)
{
    bcmltm_node_t *fsm_node = NULL;
    const bcmltm_field_select_maps_t *fsm = NULL;
    bcmltm_field_selection_id_t selection_id;

    SHR_FUNC_ENTER(unit);

    selection_id = fs->selection_id;

    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_fsm_get(unit, sid, selection_id,
                              &fsm));

    /* [Fsm]: Field selector value to field select map index. */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_select_field_map,
                                  (void *)fsm,
                                  BCMLTM_NODE_SIDE_LEFT,
                                  parent_node,
                                  op_md_ptr,
                                  &fsm_node));

    /*
     * Return chain to additional subtrees.
     *
     * FSM node can be used directly for chaining since it
     * does not have child node.
     */
    *next_chain = fsm_node;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the field select value write FA subtree.
 *
 * This routine creates the field select value write FA subtree
 * for the specified field selection for a given table.
 *
 * The subtree is attached to the left node of given parent node.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] fs Field selection information.
 * \param [in] fsc_flags Field select cluster flags.
 * \param [in] opcode LT opcode.
 * \param [in] parent_node Parent node for this subtree.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
fs_value_write_fa_subtree_create(int unit,
                                 bcmlrd_sid_t sid,
                                 const bcmltm_db_fs_core_t *fs,
                                 const bcmltm_md_fs_cluster_flags_t *fsc_flags,
                                 bcmlt_opcode_t opcode,
                                 bcmltm_node_t *parent_node,
                                 bcmltm_lt_op_md_t *op_md_ptr,
                                 bcmltm_node_t **next_chain)
{
    bcmltm_node_t *next_chain_node = NULL;
    bcmltm_node_t *fixed_value_node = NULL;
    bcmltm_fa_node_t *fixed_value_nd = NULL;
    bcmltm_node_t *value_a2c_node = NULL;
    bcmltm_fa_node_t *value_a2c_nd = NULL;
    bcmltm_node_t *value_c2p_node = NULL;
    bcmltm_fa_node_t *value_c2p_nd = NULL;
    bcmltm_node_t *fsm_node = NULL;
    bcmltm_fa_node_t *fsm_nd = NULL;
    bcmltm_node_t *value_fwd_xfrm_node = NULL;
    bcmltm_node_t *value_validate_node = NULL;
    bcmltm_fa_node_f fixed_value_func = NULL;
    bcmltm_fa_node_f value_a2c_func = NULL;
    bcmltm_fa_node_f fsm_func = NULL;
    const bcmltm_fixed_field_mapping_t *fixed_values = NULL;
    const bcmltm_field_select_mapping_t *apic_in_values = NULL;
    const bcmltm_field_select_mapping_t *in_values = NULL;
    const bcmltm_field_select_maps_t *fsm = NULL;
    bcmltm_field_selection_id_t selection_id;
    bcmltm_md_fs_flags_t fs_flags = fsc_flags->fs_flags;
    bcmltm_node_side_t node_side;
    bcmltm_field_xfrm_type_t xfrm_type;
    bcmltm_field_validate_type_t val_type;

    SHR_FUNC_ENTER(unit);

    selection_id = fs->selection_id;

    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_fixed_value_fields_get(unit, sid, selection_id,
                                             &fixed_values));
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_apic_value_fields_get(unit, sid, selection_id,
                                            BCMLTM_FIELD_DIR_IN,
                                            &apic_in_values));
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_value_fields_get(unit, sid, selection_id,
                                       BCMLTM_FIELD_DIR_IN,
                                       &in_values));
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_fsm_get(unit, sid, selection_id,
                              &fsm));

    /*
     * Construct subtree
     */

    /* [Fixed_Value]: Set fixed value fields from LRD */
    if (fs_flags & BCMLTM_MD_FS_FLAG_FIXED_DEL) {
        /* Apply fixed field delete value */
        fixed_value_func = bcmltm_fa_node_delete_select_fixed_fields_to_wb;
    } else {
        /* Apply fixed field value */
        fixed_value_func = bcmltm_fa_node_select_fixed_fields_to_wb;
    }
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       fixed_value_func,
                                       (void *)fixed_values,
                                       &fixed_value_nd));
    fixed_value_node = bcmltm_tree_left_node_create(parent_node,
                                                    (void *)fixed_value_nd,
                                                    &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(fixed_value_node, SHR_E_MEMORY);

    /* [Value_A2C]: Copy API values to API cache */
    if (fs_flags & BCMLTM_MD_FS_FLAG_APIC_IN_SKIP) {
        /*
         * Skip copy of the LTM entry input fields into the API cache.
         *
         * This is used to avoid overriding the API cache which already
         * has the desired values.
         *
         * For instance, a logical key field can map to both, a PT key
         * and an PT value.  This results in 2 different maps, a key map
         * and value map.  On DELETE, key fields are provided, yet
         * they should not be copied into the API cache when processing
         * the value field maps.
         */
        value_a2c_func = NULL;
    } else if (fs_flags & BCMLTM_MD_FS_FLAG_APIC_IN_UPDATE) {
        /* Unset copy version */
        value_a2c_func = bcmltm_fa_node_in_fields_unset_to_api_wb;
    } else {
        /* Copy API input values to API cache */
        value_a2c_func = bcmltm_fa_node_in_value_fields_to_api_wb;
    }
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       value_a2c_func,
                                       (void *)apic_in_values,
                                       &value_a2c_nd));
    value_a2c_node = bcmltm_tree_left_node_create(fixed_value_node,
                                                  (void *)value_a2c_nd,
                                                  &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(value_a2c_node, SHR_E_MEMORY);

    /* [Value_C2P]: Copy API cache values to PTM */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_api_wb_to_wbc,
                                       (void *)in_values,
                                       &value_c2p_nd));
    value_c2p_node = bcmltm_tree_right_node_create(fixed_value_node,
                                                   (void *)value_c2p_nd,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(value_c2p_node, SHR_E_MEMORY);

    /*
     * [Fsm]: Field selector value to field select map index.
     *
     * The top layer, level 0, should have a NULL fsm node cookie.
     * The result is that an empty pass-thru node tree is created.
     */
    if (fs->selector_type == BCMLTM_FIELD_SELECTOR_TYPE_KEY) {
        /*
         * Skip node if field selector type is key.
         *
         * This step (selector value to field select map index)
         * is handled in another subtree, during the keys processing.
         * The rest of the steps for the selectee fields (all selectees
         * should be value fields) are still handled in this cluster.
         */
        fsm_func = NULL;
    } else {
        fsm_func = bcmltm_fa_node_select_field_map;
    }
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       fsm_func,
                                       (void *)fsm,
                                       &fsm_nd));
    fsm_node = bcmltm_tree_left_node_create(value_a2c_node,
                                            (void *)fsm_nd,
                                            &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(fsm_node, SHR_E_MEMORY);

    /* [Next_Chain]: Chain to additional subtrees */
    next_chain_node = bcmltm_tree_left_node_create(fsm_node,
                                                   (void *)NULL,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(next_chain_node, SHR_E_MEMORY);

    /* [Value_Fwd_Xfrm]: Process LTA value forward transforms */
    xfrm_type = BCMLTM_FIELD_XFRM_TYPE_FWD_VALUE;
    node_side = BCMLTM_NODE_SIDE_RIGHT;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fs_fa_cluster_create(unit, sid, selection_id,
                                             xfrm_type,
                                             fsc_flags->lta_flags,
                                             opcode,
                                             node_side,
                                             value_c2p_node,
                                             op_md_ptr,
                                             &value_fwd_xfrm_node));

    /* [Value_Validate]: Process LTA value field validations */
    val_type = BCMLTM_FIELD_VALIDATE_TYPE_VALUE;
    node_side = BCMLTM_NODE_SIDE_RIGHT;
    SHR_IF_ERR_EXIT
         (bcmltm_md_validate_fs_fa_cluster_create(unit, sid, selection_id,
                                                  val_type,
                                                  opcode,
                                                  node_side,
                                                  value_fwd_xfrm_node,
                                                  op_md_ptr,
                                                  &value_validate_node));

    /* Return chain to additional subtrees */
    *next_chain = next_chain_node;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy node data if it is not in node tree */
        if (fixed_value_node == NULL) {
            SHR_FREE(fixed_value_nd);
        }
        if (value_a2c_node == NULL) {
            SHR_FREE(value_a2c_nd);
        }
        if (value_c2p_node == NULL) {
            SHR_FREE(value_c2p_nd);
        }
        if (fsm_node == NULL) {
            SHR_FREE(fsm_nd);
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the field select value read FA subtree.
 *
 * This routine creates the field select value read FA subtree
 * for the specified field selection for a given table.
 *
 * The subtree is attached to the left node of given parent node.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] fs Field selection information.
 * \param [in] fsc_flags Field select cluster flags.
 * \param [in] opcode LT opcode.
 * \param [in] parent_node Parent node for this subtree.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
fs_value_read_fa_subtree_create(int unit,
                                bcmlrd_sid_t sid,
                                const bcmltm_db_fs_core_t *fs,
                                const bcmltm_md_fs_cluster_flags_t *fsc_flags,
                                bcmlt_opcode_t opcode,
                                bcmltm_node_t *parent_node,
                                bcmltm_lt_op_md_t *op_md_ptr,
                                bcmltm_node_t **next_chain)
{
    bcmltm_node_t *next_chain_node = NULL;
    bcmltm_node_t *value_c2a_node = NULL;
    bcmltm_fa_node_t *value_c2a_nd = NULL;
    bcmltm_node_t *value_p2c_node = NULL;
    bcmltm_fa_node_t *value_p2c_nd = NULL;
    bcmltm_node_t *value_rev_xfrm_node = NULL;
    bcmltm_node_t *fsm_node = NULL;
    bcmltm_fa_node_t *fsm_nd = NULL;
    bcmltm_fa_node_f fsm_func = NULL;
    const bcmltm_field_select_mapping_t *apic_out_values = NULL;
    const bcmltm_field_select_mapping_t *out_values = NULL;
    const bcmltm_field_select_maps_t *fsm = NULL;
    bcmltm_field_selection_id_t selection_id;
    bcmltm_md_fs_flags_t fs_flags = fsc_flags->fs_flags;
    bcmltm_node_side_t node_side;
    bcmltm_field_xfrm_type_t xfrm_type;

    SHR_FUNC_ENTER(unit);

    selection_id = fs->selection_id;

    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_apic_value_fields_get(unit, sid, selection_id,
                                            BCMLTM_FIELD_DIR_OUT,
                                            &apic_out_values));
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_value_fields_get(unit, sid, selection_id,
                                       BCMLTM_FIELD_DIR_OUT,
                                       &out_values));
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_fsm_get(unit, sid, selection_id,
                              &fsm));

    /*
     * Construct subtree
     */

    /* [Value_C2A]: Copy API cache fields to API LT entry output */
    if (!(fs_flags & BCMLTM_MD_FS_FLAG_API_OUT_SKIP)) {
         SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_api_wb_to_out_fields,
                                           (void *)apic_out_values,
                                           &value_c2a_nd));
    } /* Else: skip copy to API LT output */
    value_c2a_node = bcmltm_tree_left_node_create(parent_node,
                                                  (void *)value_c2a_nd,
                                                  &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(value_c2a_node, SHR_E_MEMORY);

    /* [Value_P2C]: Copy PTM physical fields to API cache */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_wbc_to_api_wb,
                                       (void *)out_values,
                                       &value_p2c_nd));
    value_p2c_node = bcmltm_tree_left_node_create(value_c2a_node,
                                                  (void *)value_p2c_nd,
                                                  &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(value_p2c_node, SHR_E_MEMORY);

    /* [Value_Rev_Xfrm]: Process LTA value reverse transforms */
    xfrm_type = BCMLTM_FIELD_XFRM_TYPE_REV_VALUE;
    node_side = BCMLTM_NODE_SIDE_RIGHT;
    SHR_IF_ERR_EXIT
        (bcmltm_md_xfrm_fs_fa_cluster_create(unit, sid, selection_id,
                                             xfrm_type,
                                             fsc_flags->lta_flags,
                                             opcode,
                                             node_side,
                                             value_c2a_node,
                                             op_md_ptr,
                                             &value_rev_xfrm_node));

    /*
     * [Fsm]: Field selector value to field select map index.
     *
     * The top layer, level 0, should have a NULL fsm node cookie.
     * The result is that an empty pass-thru node tree is created.
     */
    if (fs->selector_type == BCMLTM_FIELD_SELECTOR_TYPE_KEY) {
        /*
         * Skip node if field selector type is key.
         *
         * This step (selector value to field select map index)
         * is handled in another subtree, during the keys processing.
         * The rest of the steps for the selectee fields (all selectees
         * should be value fields) are still handled in this cluster.
         */
        fsm_func = NULL;
    } else {
        fsm_func = bcmltm_fa_node_select_field_map;
    }
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       fsm_func,
                                       (void *)fsm,
                                       &fsm_nd));
    fsm_node = bcmltm_tree_left_node_create(value_p2c_node,
                                            (void *)fsm_nd,
                                            &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(fsm_node, SHR_E_MEMORY);

    /* [Next_Chain]: Chain to additional subtrees */
    next_chain_node = bcmltm_tree_left_node_create(fsm_node,
                                                   (void *)NULL,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(next_chain_node, SHR_E_MEMORY);

    /* Return chain to additional subtrees */
    *next_chain = next_chain_node;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy node data if it is not in node tree */
        if (value_c2a_node == NULL) {
            SHR_FREE(value_c2a_nd);
        }
        if (value_p2c_node == NULL) {
            SHR_FREE(value_p2c_nd);
        }
        if (fsm_node == NULL) {
            SHR_FREE(fsm_nd);
        }
    }

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_md_fs_key_fsm_fa_cluster_create(int unit,
                                       bcmlrd_sid_t sid,
                                       bcmltm_fs_cluster_type_t fs_type,
                                       bcmltm_node_side_t node_side,
                                       bcmltm_node_t *parent_chain,
                                       bcmltm_lt_op_md_t *op_md_ptr,
                                       bcmltm_node_t **next_chain)
{
    bcmltm_node_t *parent_node = NULL;
    bcmltm_db_fs_core_info_t fs_info;
    bcmltm_db_fs_core_t *fs;
    uint32_t sel_idx;
    fs_fsm_fa_subtree_create_f fs_subtree_create = NULL;

    SHR_FUNC_ENTER(unit);

    /*
     * ROOT is not valid since routine does not return
     * new root if subtrees are created.  Pointers to root nodes
     * need to be stored in the metadata.
     */
    if (node_side == BCMLTM_NODE_SIDE_ROOT) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Create anchor node for cluster in specified side/location */
    SHR_IF_ERR_EXIT
        (fs_anchor_node_create(unit, node_side,
                               parent_chain, op_md_ptr,
                               next_chain));
    parent_node = *next_chain;

    /* Get field selection information */
    SHR_IF_ERR_EXIT
        (bcmltm_db_fs_core_info_get(unit, sid, &fs_info));

    if (fs_info.num_levels == 0) {
        /* Nothing to do */
        SHR_EXIT();
    }

    /* Field select cluster to create */
    if (fs_type == BCMLTM_FS_CLUSTER_TYPE_KEY_FSM_MAP) {
        /* Key field selector map */
        fs_subtree_create = fs_fsm_map_fa_subtree_create;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /*
     * Create FSM cluster for key field selectors.
     *
     * The cluster contains the step to convert the field selector
     * value to the corresponding select map index.
     *
     * The processing of the selectee field maps (selectees are always
     * value fields) takes places in the value write/read clusters.
     */
    for (sel_idx = 0; sel_idx < fs_info.num_selections; sel_idx++) {
        fs = &fs_info.selections[sel_idx];

        if (fs->selector_type != BCMLTM_FIELD_SELECTOR_TYPE_KEY) {
            /* Skip non-key field selectors */
            continue;
        }

        /* Sanity check */
        if (fs->level != BCMLTM_FIELD_SELECTION_LEVEL_KEY_SELECTOR) {
            const char *table_name;

            table_name = bcmltm_lt_table_sid_to_name(unit, sid);
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Field selection level for Key selector "
                                  "is not %d: "
                                  "%s(ltid=%d) selection_id=%d level=%d\n"),
                       BCMLTM_FIELD_SELECTION_LEVEL_KEY_SELECTOR,
                       table_name, sid, fs->selection_id, fs->level));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        /* Create subtree for selection */
        SHR_IF_ERR_EXIT
            (fs_subtree_create(unit, sid, fs,
                               parent_node, op_md_ptr, next_chain));
        parent_node = *next_chain;
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_md_fs_key_fsm_map_cluster_create(int unit,
                                        bcmlrd_sid_t sid,
                                        bcmltm_node_side_t node_side,
                                        bcmltm_node_t *parent_node,
                                        bcmltm_lt_op_md_t *op_md_ptr,
                                        bcmltm_node_t **next_chain)
{
    bcmltm_fs_cluster_type_t fs_type;

    SHR_FUNC_ENTER(unit);

    /* [Key_Fsm_Map]: Key field selector value to field select map index */
    fs_type = BCMLTM_FS_CLUSTER_TYPE_KEY_FSM_MAP;

    SHR_IF_ERR_EXIT
        (bcmltm_md_fs_key_fsm_fa_cluster_create(unit, sid,
                                                fs_type,
                                                node_side,
                                                parent_node,
                                                op_md_ptr,
                                                next_chain));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_md_fs_value_fa_cluster_create(int unit,
                               bcmlrd_sid_t sid,
                               bcmltm_fs_cluster_type_t fs_type,
                               const bcmltm_md_fs_cluster_flags_t *fsc_flags,
                               bcmlt_opcode_t opcode,
                               bcmltm_node_side_t node_side,
                               bcmltm_node_t *parent_chain,
                               bcmltm_lt_op_md_t *op_md_ptr,
                               bcmltm_node_t **next_chain)
{
    bcmltm_node_t *parent_node = NULL;
    bcmltm_db_fs_core_info_t fs_info;
    bcmltm_db_fs_core_t *fs;
    uint32_t sel_idx;
    uint32_t level;
    uint32_t level_count;
    fs_value_fa_subtree_create_f fs_subtree_create = NULL;

    SHR_FUNC_ENTER(unit);

    /*
     * ROOT is not valid since routine does not return
     * new root if subtrees are created.  Pointers to root nodes
     * need to be stored in the metadata.
     */
    if (node_side == BCMLTM_NODE_SIDE_ROOT) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Create anchor node for cluster in specified side/location */
    SHR_IF_ERR_EXIT
        (fs_anchor_node_create(unit, node_side,
                               parent_chain, op_md_ptr,
                               next_chain));
    parent_node = *next_chain;

    /* Get field selection information */
    SHR_IF_ERR_EXIT
        (bcmltm_db_fs_core_info_get(unit, sid, &fs_info));

    if (fs_info.num_levels == 0) {
        /* Nothing to do */
        SHR_EXIT();
    }

    /* Field select cluster to create */
    if (fs_type == BCMLTM_FS_CLUSTER_TYPE_VALUE_WRITE) {
        /* Field select value write cluster */
        fs_subtree_create = fs_value_write_fa_subtree_create;
    } else if (fs_type == BCMLTM_FS_CLUSTER_TYPE_VALUE_READ) {
        /* Field select value read cluster */
        fs_subtree_create = fs_value_read_fa_subtree_create;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /*
     * Create field selection subtrees for each level.
     *
     * Selection groups with the lowest level must be executed first.
     * Based on the LTM tree execution order, field select subtrees
     * with higher levels are installed higher in the tree.
     */
    level = fs_info.num_levels - 1;
    for (level_count = 0; level_count < fs_info.num_levels; level_count++) {

        /*
         * Create subtrees for all field selections for
         * the current level.
         */
        for (sel_idx = 0; sel_idx < fs_info.num_selections; sel_idx++) {
            fs = &fs_info.selections[sel_idx];

            if (fs->level != level) {
                /* Not the current level, skip */
                continue;
            }

            /* Create subtree for selection */
            SHR_IF_ERR_EXIT
                (fs_subtree_create(unit, sid, fs,
                                   fsc_flags,
                                   opcode,
                                   parent_node,
                                   op_md_ptr,
                                   next_chain));
            parent_node = *next_chain;
        }

        level--;
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_md_fs_value_write_cluster_create(int unit,
                                        bcmlrd_sid_t sid,
                                        bcmlt_opcode_t opcode,
                                        bcmltm_node_side_t node_side,
                                        bcmltm_node_t *parent_node,
                                        bcmltm_lt_op_md_t *op_md_ptr,
                                        bcmltm_node_t **next_chain)
{
    bcmltm_fs_cluster_type_t fs_type;
    bcmltm_md_fs_cluster_flags_t fsc_flags;

    SHR_FUNC_ENTER(unit);

    /* [Value_Write_Fs]: Process value writes field selections */
    fs_type = BCMLTM_FS_CLUSTER_TYPE_VALUE_WRITE;
    fsc_flags.fs_flags = BCMLTM_MD_FS_FLAG_DEFAULTS;
    fsc_flags.lta_flags = BCMLTM_MD_LTA_FLAG_DEFAULTS;

    if (opcode == BCMLT_OPCODE_UPDATE) {
        /*
         * Special handling on UPDATE:
         * Do not apply defaults if field is not present in API input.
         */
        fsc_flags.fs_flags |= BCMLTM_MD_FS_FLAG_APIC_IN_UPDATE;
    } else if (opcode == BCMLT_OPCODE_DELETE) {
        /*
         * Special handling on DELETE:
         * - Skip copy of API input into API cache (to avoid overriding
         *   default values that are already in API cache).
         * - Apply the LRD delete value of a fixed field rather
         *   than its fixed field value.
         */
        fsc_flags.fs_flags |= BCMLTM_MD_FS_FLAG_APIC_IN_SKIP;
        fsc_flags.fs_flags |= BCMLTM_MD_FS_FLAG_FIXED_DEL;
    }

    SHR_IF_ERR_EXIT
        (bcmltm_md_fs_value_fa_cluster_create(unit, sid,
                                              fs_type, &fsc_flags,
                                              opcode,
                                              node_side,
                                              parent_node,
                                              op_md_ptr,
                                              next_chain));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_md_fs_value_read_cluster_create(int unit,
                                       bcmlrd_sid_t sid,
                                       bcmlt_opcode_t opcode,
                                       bcmltm_node_side_t node_side,
                                       bcmltm_node_t *parent_node,
                                       bcmltm_lt_op_md_t *op_md_ptr,
                                       bcmltm_node_t **next_chain)
{
    bcmltm_fs_cluster_type_t fs_type;
    bcmltm_md_fs_cluster_flags_t fsc_flags;

    SHR_FUNC_ENTER(unit);

    /* [Value_Read_Fs]: Process value reads field selections */
    fs_type = BCMLTM_FS_CLUSTER_TYPE_VALUE_READ;
    fsc_flags.fs_flags = BCMLTM_MD_FS_FLAG_DEFAULTS;
    fsc_flags.lta_flags = BCMLTM_MD_LTA_FLAG_DEFAULTS;

    if (opcode == BCMLT_OPCODE_UPDATE) {
        /*
         * Special handling on UPDATE:
         * Do not copy fields to API output.
         */
        fsc_flags.fs_flags |= BCMLTM_MD_FS_FLAG_API_OUT_SKIP;
    }

    SHR_IF_ERR_EXIT
        (bcmltm_md_fs_value_fa_cluster_create(unit, sid,
                                              fs_type, &fsc_flags,
                                              opcode,
                                              node_side,
                                              parent_node,
                                              op_md_ptr,
                                              next_chain));

 exit:
    SHR_FUNC_EXIT();
}
