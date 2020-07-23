/*! \file bcmltm_md_lta_xfrm.c
 *
 * Logical Table Manager - LTA Field Transform.
 *
 * This file contains implementation to support metadata construction
 * for logical tables with Field Transform.
 *
 * FA Field Transform Cluster
 * --------------------------
 *
 * The Field Transform cluster (for Keys and Values) is
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
 * Each transform subtree is composed of a set of nodes.
 * For details, refer to diagram in corresponding function
 * that constructs the subtree.
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

#include <shr/shr_debug.h>

#include <bsl/bsl.h>

#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_db_internal.h>
#include <bcmltm/bcmltm_db_core_internal.h>
#include <bcmltm/bcmltm_fa_tree_nodes_internal.h>

#include "bcmltm_md_lt_drv.h"
#include "bcmltm_md_lta.h"
#include "bcmltm_md_lta_xfrm.h"
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
 * \brief Get Forward Transform API to LTA copy function node.
 *
 * This helper routine returns the corresponding FA function node for
 * the forward field transform in the following step of the transform
 * subtree:
 *   API input list (or default) copy to LTA input field list.
 *
 * Depending on the opcode, field transform type, and LTA field options,
 * the returning function node pointer varies:
 * - bcmltm_fa_node_api_key_fields_to_lta()
 *     API entry input key fields are copied to the LTA field list.
 * - bcmltm_fa_node_api_alloc_key_fields_to_lta()
 *     The LTA field list contains optional fields that can be
 *     allocated.
 * - bcmltm_fa_node_api_value_fields_to_lta()
 *     API entry input value fields are copied to the LTA field list.
 * - bcmltm_fa_node_api_fields_default_to_lta()
 *     API entry input fields are copied to the LTA field list and,
 *     in addition, field default values are added to the LTA list for
 *     those fields which are not present in the input list.
 *     is not present in the input.
 * - bcmltm_fa_node_api_fields_update_to_lta()
 *     API entry input fields are copied to an existing LTA
 *     field list.  The API fields values must overwrite matching
 *     field elements in the list.
 * - bcmltm_fa_node_default_fields_to_lta()
 *     The LTA field list is populated with all default field values.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] opcode LT opcode.
 * \param [in] xfrm_type Transform Type.
 * \param [in] lta_fopt LTA field options.
 *
 * \retval Pointer to function node.
 * \retval NULL, if not applicable.
 */
static bcmltm_fa_node_f
fwd_in_api_to_lta_func_get(int unit, bcmlrd_sid_t sid,
                           bcmlt_opcode_t opcode,
                           int xfrm_type,
                           bcmltm_md_lta_fopt_t lta_fopt)
{
    bcmltm_fa_node_f func = NULL;
    bool keys = FALSE;
    bool alloc = FALSE;

    /* Check for transform type */
    if ((xfrm_type != BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM) &&
        (xfrm_type != BCMLTM_DB_XFRM_TYPE_FWD_VALUE_TRANSFORM)) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Invalid transform type: "
                             "%s(ltid=%d) opcode=%d xfrm_type=%d\n"),
                  table_name, sid, opcode, xfrm_type));
        return NULL;
    }

    /* Check to skip api to lta field copy */
    if (lta_fopt & BCMLTM_MD_LTA_FOPT_IN_NO_API) {
        return NULL;
    }
    /* Check if keys are allocatable */
    if (lta_fopt & BCMLTM_MD_LTA_FOPT_IN_API_ALLOC) {
        alloc = TRUE;
    }

    /* Check for key or value transform */
    if (xfrm_type == BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM) {
        keys = TRUE;
    }

    if (opcode == BCMLT_OPCODE_INSERT) {
        if (keys) { /* Keys */
            if (alloc) {
                /* Allocatable keys */
                func = bcmltm_fa_node_api_alloc_key_fields_to_lta;
            } else {
                func = bcmltm_fa_node_api_key_fields_to_lta;
            }
        } else { /* Values */
            func = bcmltm_fa_node_api_fields_default_to_lta;
        }

    } else if (opcode == BCMLT_OPCODE_UPDATE) {
        if (keys) { /* Keys */
            func = bcmltm_fa_node_api_key_fields_to_lta;
        } else { /* Values */
            func = bcmltm_fa_node_api_fields_update_to_lta;
        }

    } else if (opcode == BCMLT_OPCODE_DELETE) {
        if (keys) { /* Keys */
            func = bcmltm_fa_node_api_key_fields_to_lta;
        } else { /* Values */
            func = bcmltm_fa_node_default_fields_to_lta;
        }

    } else if (opcode == BCMLT_OPCODE_LOOKUP) {
        if (keys) { /* Keys */
            func = bcmltm_fa_node_api_key_fields_to_lta;
        }
        /* Not applicable for values */

    } else if (opcode == BCMLT_OPCODE_TRAVERSE) {
        if (keys) { /* Keys */
            func = bcmltm_fa_node_api_key_fields_to_lta;
        }
        /* Not applicable for values */
    }

    return func;
}

/*!
 * \brief Get Forward Transform LTA to LTA copy function node.
 *
 * This helper routine returns the corresponding FA function node for
 * the forward field transform in the following step of the transform
 * subtree:
 *   LTA reverse output input list copy to LTA input field list.
 *
 * Depending on the opcode, field transform type, and LTA field options,
 * the returning function node pointer varies.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] opcode LT opcode.
 * \param [in] to_xfrm_type Transform type to copy to.
 * \param [in] lta_fopt LTA field options.
 * \param [out] from_xfrm_type Transform type to copy from.
 *
 * \retval Pointer to function node.
 * \retval NULL, if not applicable.
 */
static bcmltm_fa_node_f
fwd_in_lta_to_lta_func_get(int unit, bcmlrd_sid_t sid,
                           bcmlt_opcode_t opcode,
                           int to_xfrm_type,
                           bcmltm_md_lta_fopt_t lta_fopt,
                           int *from_xfrm_type)
{
    bcmltm_fa_node_f func = NULL;
    bool keys = FALSE;

    /* Check for transform type */
    if ((to_xfrm_type != BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM) &&
        (to_xfrm_type != BCMLTM_DB_XFRM_TYPE_FWD_VALUE_TRANSFORM)) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Invalid transform type: "
                             "%s(ltid=%d) opcode=%d xfrm_type=%d\n"),
                  table_name, sid, opcode, to_xfrm_type));
        return NULL;
    }

    if (to_xfrm_type == BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM) {
        keys = TRUE;
        *from_xfrm_type = BCMLTM_DB_XFRM_TYPE_REV_KEY_TRANSFORM;
    } else {
        *from_xfrm_type = BCMLTM_DB_XFRM_TYPE_REV_VALUE_TRANSFORM;
    }

    /*
     * LTA to LTA copies reverse transform out fields to forward input
     */
    if (opcode == BCMLT_OPCODE_INSERT) {
        

    } else if (opcode == BCMLT_OPCODE_UPDATE) {
        /*
         * This assumes UPDATE always need a lta_to_lta copy.
         * This can potentially changed to check the field options.
         * If so, callers need to set the options accordingly.
         */
        if (!keys) {  /* Needed for values only */
            func = bcmltm_fa_node_lta_to_lta_fields;
        }

    } else if (opcode == BCMLT_OPCODE_TRAVERSE) {
        if (lta_fopt & BCMLTM_MD_LTA_FOPT_IN_LTA) {
            func = bcmltm_fa_node_lta_to_lta_fields;
        }
    }

    return func;
}

/*!
 * \brief Create the FA subtree for Forward Field Transform.
 *
 * Create the FA sub-tree for a Forward Field Transform.
 * This sub-tree is inserted in Left node of given parent node.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] xfrm_type Transform Type.
 * \param [in] xfrm_idx Transform Index.
 * \param [in] lta_fopt LTA field options.
 * \param [in] opcode LT opcode.
 * \param [in] fa_node_parent Parent node for this transform tree.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_fa_node_tree_fwd_create(int unit,
                             bcmlrd_sid_t sid,
                             int xfrm_type,
                             uint32_t xfrm_idx,
                             bcmltm_md_lta_fopt_t lta_fopt,
                             bcmlt_opcode_t opcode,
                             bcmltm_node_t *fa_node_parent,
                             bcmltm_lt_op_md_t *op_md_ptr,
                             bcmltm_node_t **next_chain)
{
    bcmltm_node_t *next_chain_node = NULL;
    /* #1 */
    const bcmltm_lta_field_list_t *in_key_fields = NULL;
    bcmltm_fa_node_t *init_in_key_node_data = NULL;
    bcmltm_node_t    *init_in_key_node = NULL;
    /* #2 */
    const bcmltm_lta_field_list_t *out_fields = NULL;
    bcmltm_fa_node_t *init_out_node_data = NULL;
    bcmltm_node_t    *init_out_node = NULL;
    /* #3 */
    bcmltm_fa_node_t *init_in_node_data = NULL;
    bcmltm_node_t    *init_in_node = NULL;
    /* #4 */
    const bcmltm_lta_field_list_t *copy_fields = NULL;
    bcmltm_fa_node_t *copy_node_data = NULL;
    bcmltm_node_t    *copy_node = NULL;
    bcmltm_node_t    *copy_node_root = NULL;
    bcmltm_fa_node_f lta_to_lta_func = NULL;
    uint32_t rev_xfrm_idx;
    uint32_t num_rev_xfrm = 0;
    int rev_xfrm_type = 0;
    /* #5 */
    bcmltm_fa_node_t *api_key_to_lta_node_data = NULL;
    bcmltm_node_t    *api_key_to_lta_node = NULL;
    /* #6 */
    const bcmltm_lta_field_list_t *in_fields = NULL;
    bcmltm_fa_node_t *api_to_lta_node_data = NULL;
    bcmltm_node_t    *api_to_lta_node = NULL;
    /* #7 */
    const bcmltm_lta_transform_params_t *lta_xfrm_params = NULL;
    bcmltm_fa_node_t *lta_xfrm_param_data = NULL;
    bcmltm_node_t    *lta_xfrm_param_node = NULL;
    bcmltm_fa_node_f lta_transform_func = NULL;
    /* #8 */
    bcmltm_fa_node_t *lta_to_wbc_node_data = NULL;
    bcmltm_node_t    *lta_to_wbc_node = NULL;
    bcmltm_fa_node_f api_to_lta_func = NULL;
    /* #9 */
    bcmltm_fa_node_t *lta_pt_sid_map_node_data = NULL;
    bcmltm_node_t    *lta_pt_sid_map_node = NULL;

    bcmltm_node_t    *root_node = NULL;

    int is_value_xfrm = (xfrm_type == BCMLTM_DB_XFRM_TYPE_FWD_VALUE_TRANSFORM) ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    /*
     * FA FORWARD XFRM Sub Tree
     * ---------------------------------------------------------------
     *   Tree_0:                           [Parent]
     *                                        |
     *                                        |
     *                               +--------+-------+
     *                               |                |
     *                          [Left_Node]      [Right_Node]
     *                              #8               #9
     *                               |
     *                 +-------------+-------------+
     *                 |                           |
     *            [Left_Node]                 [Right_Node]
     *                #3                          #7
     *                 |                           |
     *         +-------+------+            +-------+------+
     *         |              |            |              |
     *    [Left_Node]    [Right_Node]  [Left_Node]   [Right_Node]
     *        #0             #2            #4            #6
     *                        |                           |
     *                 +------+------+             +------+------+
     *                 |             |             |             |
     *            [Left_Node]       ===       [Left_Node]       ===
     *                #1                          #5
     *
     * #0. empty node : chain to additional LTA transform subtrees.
     * #1. bcmltm_fa_node_lta_input_init    : Init LTA in(key_src) fields
     * #2. bcmltm_fa_node_lta_output_init   : Init LTA out(dst) fields(LTA-PTM)
     * #3. bcmltm_fa_node_lta_input_init    : Init LTA in(src) fields
     * #4. bcmltm_fa_node_lta_to_lta_fields : Copy LTA fields to input in(src)
     * #5. bcmltm_fa_node_api_key_fields_to_lta : Copy API fields to LTA
     *                                       in_key(key_src) fields
     * #6. <fields_to_lta>                  : Init LTA in(src) fields
     *     Depending on the opcode, transform type, and LTA field options,
     *     the LTA input field list is populated different.
     *     See fwd_in_api_to_lta_func_get for more information.
     * #7. bcmltm_fa_node_lta_transform          : Invoke LTA Transform function
     *     or
     *     bcmltm_fa_node_lta_extended_transform (for LTA extended transform
     *                                            function)
     *     or
     *     bcmltm_fa_node_lta_traverse_transform (for Key on TRAVERSE)
     * #8. bcmltm_fa_node_lta_fields_to_wbc : Copy LTA transform output fields
     *                                        to PTM entry fields.
     * #9. bcmltm_fa_node_lta_pt_sid_map    : Convert LTA PT select index to
     *                                        PT SID
     *   -------------------------------------------------------------
     *
     * NOTE:
     * The Forward Transform function is invoked.
     * As such, the transform function expects the following:
     *    Input:     Source fields
     *    Input Key: Source Key fields
     *    Ouput:     Destination fields
     */

    root_node = bcmltm_tree_left_node_create(fa_node_parent,
                                             (void *)NULL,
                                             &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(root_node, SHR_E_MEMORY);

    /**************************************************************************
     * #9. bcmltm_fa_node_lta_pt_sid_map: Convert LTA PT select index to PT SID
 */
    SHR_IF_ERR_EXIT
        (bcmltm_db_xfrm_params_get(unit, sid, xfrm_type, xfrm_idx,
                                   &lta_xfrm_params));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_pt_sid_map,
                                       (void *)lta_xfrm_params,
                                       &lta_pt_sid_map_node_data));
    lta_pt_sid_map_node = bcmltm_tree_right_node_create(root_node,
                                       (void *)lta_pt_sid_map_node_data,
                                                        &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(lta_pt_sid_map_node, SHR_E_MEMORY);

    /**************************************************************************
     * #8. bcmltm_fa_node_lta_fields_to_wbc : Copy LTA transform output fields
     *                                        to PTM entry fields.
 */
    SHR_IF_ERR_EXIT
        (bcmltm_db_xfrm_out_fields_get(unit, sid, xfrm_type, xfrm_idx,
                                       &out_fields));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_fields_to_wbc,
                                       (void *)out_fields,
                                       &lta_to_wbc_node_data));
    lta_to_wbc_node = bcmltm_tree_left_node_create(root_node,
                                       (void *)lta_to_wbc_node_data,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(lta_to_wbc_node, SHR_E_MEMORY);

    /**************************************************************************
     * #7. <transform> : Invoke LTA Transform function
     *     - bcmltm_fa_node_lta_traverse_transform (for TRAVERSE)
     *     or
     *     - bcmltm_fa_node_lta_transform (for non-TRAVERSE, normal transform)
     *     or
     *     - bcmltm_fa_node_lta_ext_transform (for non-TRAVERSE,
     *                                         extended transform)
 */
    if (opcode == BCMLT_OPCODE_TRAVERSE) {
        /* If fields are copied from LTA, always call the transform */
        if (lta_fopt & BCMLTM_MD_LTA_FOPT_IN_LTA) {
            lta_transform_func = bcmltm_fa_node_lta_transform;
        } else {
            lta_transform_func = bcmltm_fa_node_lta_traverse_transform;
        }
    } else {
        if (lta_xfrm_params->lta_ext_transform) {
            lta_transform_func = bcmltm_fa_node_lta_ext_transform;
        } else {
            lta_transform_func = bcmltm_fa_node_lta_transform;
        }
    }
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       lta_transform_func,
                                       (void *)lta_xfrm_params,
                                       &lta_xfrm_param_data));
    lta_xfrm_param_node = bcmltm_tree_right_node_create(lta_to_wbc_node,
                                                (void *)lta_xfrm_param_data,
                                                        &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(lta_xfrm_param_node, SHR_E_MEMORY);

    /**************************************************************************
     * #6. <fields_to_lta> : Init LTA in(src) fields.
     *                       Populate LTA field list with one of these:
     *                       - LTM entry input only, overwrite
     *                       - LTM entry input and defaults (unspecified fields)
     *                       - Defaults only
     *                       - LTM entry input only, keys optional (alloc)
     *                       - LTM entry input only, keys required
     *                       - LTM entry input only, values (optional)
 */
    SHR_IF_ERR_EXIT
        (bcmltm_db_xfrm_in_fields_get(unit, sid, xfrm_type, xfrm_idx,
                                      &in_fields));
    api_to_lta_func = fwd_in_api_to_lta_func_get(unit, sid, opcode,
                                                 xfrm_type, lta_fopt);
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       api_to_lta_func,
                                       (void *)in_fields,
                                       &api_to_lta_node_data));
    api_to_lta_node = bcmltm_tree_right_node_create(lta_xfrm_param_node,
                                       (void *)api_to_lta_node_data,
                                                    &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(api_to_lta_node, SHR_E_MEMORY);

    /**************************************************************************
     * #4. bcmltm_fa_node_lta_to_lta_fields : Sub-tree of list copies
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
    lta_to_lta_func = fwd_in_lta_to_lta_func_get(unit, sid, opcode,
                                                 xfrm_type, lta_fopt,
                                                 &rev_xfrm_type);
    if (lta_to_lta_func != NULL) {
        SHR_IF_ERR_EXIT
            (bcmltm_db_xfrm_num_get(unit, sid,
                                    rev_xfrm_type, &num_rev_xfrm));

        if (num_rev_xfrm > 0) {
            /* Add Copy subtrees */
            copy_node_root = lta_xfrm_param_node;
            for (rev_xfrm_idx = 0;
                 rev_xfrm_idx < num_rev_xfrm;
                 rev_xfrm_idx++) {
                SHR_IF_ERR_EXIT
                    (bcmltm_db_xfrm_copy_fields_get(unit, sid,
                                                    xfrm_type,
                                                    xfrm_idx,
                                                    rev_xfrm_type,
                                                    rev_xfrm_idx,
                                                    &copy_fields));

                if (copy_fields == NULL) {
                    /* No field in this crossbar, skip. */
                    continue;
                }

                SHR_IF_ERR_EXIT
                    (bcmltm_md_fa_node_data_create(unit,
                                                   lta_to_lta_func,
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
     * #5. bcmltm_fa_node_api_key_fields_to_lta : Copy API fields to
     *                                            LTA in_key(key_src) fields
 */
    if (is_value_xfrm) {
        SHR_IF_ERR_EXIT
            (bcmltm_db_xfrm_in_key_fields_get(unit, sid, xfrm_type,
                                              xfrm_idx, &in_key_fields));
        if (in_key_fields != NULL) {
            SHR_IF_ERR_EXIT
                (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_api_key_fields_to_lta,
                                           (void *)in_key_fields,
                                           &api_key_to_lta_node_data));
            api_key_to_lta_node =
                bcmltm_tree_left_node_create(api_to_lta_node,
                                             (void *)api_key_to_lta_node_data,
                                             &(op_md_ptr->num_nodes));
            SHR_NULL_CHECK(api_key_to_lta_node, SHR_E_MEMORY);
        }
    }
    /**************************************************************************
     * #3. bcmltm_fa_node_lta_input_init    : Init LTA Input fields
 */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                      bcmltm_fa_node_lta_input_init,
                                       (void *)in_fields,
                                       &init_in_node_data));
    init_in_node = bcmltm_tree_left_node_create(lta_to_wbc_node,
                                                 (void *)init_in_node_data,
                                                &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(init_in_node, SHR_E_MEMORY);

    /**************************************************************************
     * #0. empty node : Chain to additional LTA transform subtrees.
 */
    next_chain_node = bcmltm_tree_left_node_create(init_in_node,
                                                   (void *)NULL,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(next_chain_node, SHR_E_MEMORY);

    /**************************************************************************
     * #2. bcmltm_fa_node_lta_output_init   : Init LTA Output fields
 */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_output_init,
                                       (void *)out_fields,
                                       &init_out_node_data));
    init_out_node = bcmltm_tree_right_node_create(init_in_node,
                                                (void *)init_out_node_data,
                                                  &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(init_out_node, SHR_E_MEMORY);

    /**************************************************************************
     * #1. bcmltm_fa_node_lta_input_init: Init LTA Input key fields
 */
    if (is_value_xfrm) {
        if (in_key_fields != NULL) {
            SHR_IF_ERR_EXIT
                (bcmltm_md_fa_node_data_create(unit,
                                               bcmltm_fa_node_lta_input_init,
                                               (void *)in_key_fields,
                                               &init_in_key_node_data));
            init_in_key_node =
                bcmltm_tree_left_node_create(init_out_node,
                                             (void *)init_in_key_node_data,
                                             &(op_md_ptr->num_nodes));
            SHR_NULL_CHECK(init_in_key_node, SHR_E_MEMORY);
        }
    }

    *next_chain = next_chain_node;
 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        if (lta_to_wbc_node == NULL) {
            SHR_FREE(lta_to_wbc_node_data);
        }

        if (api_to_lta_node == NULL) {
            SHR_FREE(api_to_lta_node_data);
        }

        if (lta_xfrm_param_node == NULL) {
            SHR_FREE(lta_xfrm_param_data);
        }

        if (api_key_to_lta_node == NULL) {
            SHR_FREE(api_key_to_lta_node_data);
        }

        if (init_out_node == NULL) {
            SHR_FREE(init_out_node_data);
        }

        if (init_in_node == NULL) {
            SHR_FREE(init_in_node_data);
        }

        if (init_in_key_node == NULL) {
            SHR_FREE(init_in_key_node_data);
        }
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create the FA subtree for Reverse Field Transform.
 *
 * Create the FA sub-tree for a Reverse Field Transform.
 * This sub-tree is inserted in Left node of given parent node.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] xfrm_type Transform Type.
 * \param [in] xfrm_idx Transform Index.
 * \param [in] opcode LT opcode.
 * \param [in] fa_node_parent Parent node for this transform tree.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfrm_fa_node_tree_rev_create(int unit,
                             bcmlrd_sid_t sid,
                             int xfrm_type,
                             uint32_t xfrm_idx,
                             bcmlt_opcode_t opcode,
                             bcmltm_node_t *fa_node_parent,
                             bcmltm_lt_op_md_t *op_md_ptr,
                             bcmltm_node_t **next_chain)
{
    bcmltm_node_t *next_chain_node = NULL;
    const bcmltm_lta_field_list_t *out_fields = NULL; /* Used by several */
    const bcmltm_lta_field_list_t *in_fields = NULL; /* Used by several */
    const bcmltm_lta_field_list_t *in_key_fields = NULL;
    /* #1 */
    bcmltm_fa_node_t *init_in_key_node_data = NULL;
    bcmltm_node_t    *init_in_key_node = NULL;
    /* #2 */
    bcmltm_fa_node_t *init_out_node_data = NULL;
    bcmltm_node_t    *init_out_node = NULL;
    /* #3 */
    bcmltm_fa_node_t *init_in_node_data = NULL;
    bcmltm_node_t    *init_in_node = NULL;
    /* #4 */
    bcmltm_fa_node_t *wbc_to_lta_node_data = NULL;
    bcmltm_node_t    *wbc_to_lta_node = NULL;
    /* #5 */
    bcmltm_fa_node_t *api_key_to_lta_node_data = NULL;
    bcmltm_node_t    *api_key_to_lta_node = NULL;
    const bcmltm_lta_field_list_t *copy_fields = NULL;
    bcmltm_fa_node_t *copy_node_data = NULL;
    bcmltm_node_t    *copy_node = NULL;
    bcmltm_node_t    *copy_node_root = NULL;
    bcmltm_fa_node_f lta_to_lta_func = NULL;
    uint32_t rev_xfrm_idx;
    uint32_t num_rev_xfrm = 0;
    int rev_xfrm_type = 0;
    /* #6 */
    const bcmltm_lta_transform_params_t *lta_xfrm_params = NULL;
    bcmltm_fa_node_t *lta_xfrm_param_data = NULL;
    bcmltm_node_t    *lta_xfrm_param_node = NULL;
    bcmltm_fa_node_f lta_transform_func = NULL;
    /* #7 */
    bcmltm_fa_node_t *lta_to_api_node_data = NULL;
    bcmltm_node_t    *lta_to_api_node = NULL;

    int is_value_xfrm = (xfrm_type == BCMLTM_DB_XFRM_TYPE_REV_VALUE_TRANSFORM) ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    /*
     * FA REVERSE XFRM Sub Tree
     * ---------------------------------------------------------------
     *   Tree_0:                            [Parent]
     *                                         |
     *                                         |
     *                                +--------+-------+
     *                                |               ===
     *                           [Left_Node]
     *                               #7
     *                                |
     *                 +--------------+--------------------------+
     *                 |                                         |
     *            [Left_Node]                               [Right_Node]
     *                #3                                        #6
     *                 |                                         |
     *         +-------+------+                          +-------+------+
     *         |              |                          |              |
     *    [Left_Node]    [Right_Node]                [Left_Node]   [Right_Node]
     *        #0             #2                         #4             #5
     *                        |
     *                +-------+------+
     *                |              |
     *           [Left_Node]        ===
     *               #1
     *
     * #0. next chain node : chain to additional LTA transform subtrees.
     * #1. bcmltm_fa_node_lta_input_init        : Init LTA in(key_src) fields.
     * #2. bcmltm_fa_node_lta_output_init       : Init LTA out(src) fields.
     * #3. bcmltm_fa_node_lta_input_init        : Init LTA in(dst) fields.
     * #4. bcmltm_fa_node_wbc_to_lta_fields     : Copy PTM to LTA in(dst) fields.
     * #5. bcmltm_fa_node_api_key_fields_to_lta : Copy API key fields
     *                                            to LTA in_key(key_src) fields.
     *     or
     *     bcmltm_fa_node_lta_to_lta_fields     : Copy key_src value
     *                                            from LTA out in key reverse transform
     *                                            to LTA in_key in reverse value transform.
     * #6. bcmltm_fa_node_lta_transform         : Invoke LTA rev Transform function
     *     or
     *     bcmltm_fa_node_lta_ext_transform     : Invoke LTA rev Extended Transform function
     * #7. <lta_to_api_fields>                  : Copy LTA out(src) fields to LTM.
     *     bcmltm_fa_node_lta_to_api_fields()
     *     bcmltm_fa_node_lta_to_api_alloc_fields()
     *   -------------------------------------------------------------
     * NOTE:
     * The Reverse Transform function is invoked.
     * As such, the reverse transform function expects the following:
     *    Input:     Destination fields
     *    Input Key: Source Key fields
     *    Ouput:     Source fields
     */
    /**************************************************************************
     * #7. <lta_to_api_fields> : Copy LTA out(src) fields to API field list.
     *     - bcmltm_fa_node_lta_to_api_fields()
     *       Copy all LTA out(src) fields to API field list.
     *       Valid for TRAVERSE/UPDATE opcodes.
     *     - bcmltm_fa_node_lta_to_api_alloc_fields()
     *       Copy all LTA out(src) fields to API field list only if
     *       fields were absent during INSERT.
     *       Valid for INSERT on IwA LTs.
     *
     *     Note:  Not present for UPDATE opcode.  Replace with empty node.
 */
    /* We need the out fields list later for the LTA output init */
    SHR_IF_ERR_EXIT
        (bcmltm_db_xfrm_out_fields_get(unit, sid, xfrm_type,
                                       xfrm_idx, &out_fields));
    if ((opcode == BCMLT_OPCODE_TRAVERSE) ||
        (opcode == BCMLT_OPCODE_LOOKUP)) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                           bcmltm_fa_node_lta_to_api_fields,
                                           (void *)out_fields,
                                           &lta_to_api_node_data));
    }  else if (opcode == BCMLT_OPCODE_INSERT) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_fa_node_data_create(unit,
                                    bcmltm_fa_node_lta_to_api_alloc_fields,
                                           (void *)out_fields,
                                           &lta_to_api_node_data));
    }  /* Else, null node data, tree traverse will skip */
    lta_to_api_node = bcmltm_tree_left_node_create(fa_node_parent,
                                           (void *)lta_to_api_node_data,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(lta_to_api_node, SHR_E_MEMORY);

    /**************************************************************************
     * #3. bcmltm_fa_node_lta_input_init : Init LTA in(dst) fields (input).
 */
    SHR_IF_ERR_EXIT
        (bcmltm_db_xfrm_in_fields_get(unit, sid, xfrm_type, xfrm_idx,
                                      &in_fields));
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_input_init,
                                       (void *)in_fields,
                                       &init_in_node_data));
    init_in_node = bcmltm_tree_left_node_create(lta_to_api_node,
                                                  (void *)init_in_node_data,
                                                &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(init_in_node, SHR_E_MEMORY);

    /**************************************************************************
     * #6. <transform> : Invoke LTA rev Transform function
     *     - bcmltm_fa_node_lta_transform
     *     or
     *     - bcmltm_fa_node_lta_ext_transform
 */
    SHR_IF_ERR_EXIT
        (bcmltm_db_xfrm_params_get(unit, sid, xfrm_type, xfrm_idx,
                                   &lta_xfrm_params));

    if (lta_xfrm_params->lta_ext_transform) {
        lta_transform_func = bcmltm_fa_node_lta_ext_transform;
    } else {
        lta_transform_func = bcmltm_fa_node_lta_transform;
    }
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       lta_transform_func,
                                       (void *)lta_xfrm_params,
                                       &lta_xfrm_param_data));
    lta_xfrm_param_node = bcmltm_tree_right_node_create(lta_to_api_node,
                                                (void *)lta_xfrm_param_data,
                                                        &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(lta_xfrm_param_node, SHR_E_MEMORY);

    /**************************************************************************
     * #0. next chain node : chain to additional LTA transform subtrees.
 */
    next_chain_node = bcmltm_tree_left_node_create(init_in_node,
                                               (void *)NULL,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(next_chain_node, SHR_E_MEMORY);

    /**************************************************************************
     * #2. bcmltm_fa_node_lta_output_init   : Init LTA out(src) fields (output).
 */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_lta_output_init,
                                       (void *)out_fields,
                                       &init_out_node_data));
    init_out_node = bcmltm_tree_right_node_create(init_in_node,
                                                  (void *)init_out_node_data,
                                                  &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(init_out_node, SHR_E_MEMORY);

    /**************************************************************************
     * #1. bcmltm_fa_node_lta_input_init : Init LTA in_key(src_key) fields (input).
 */
    if (is_value_xfrm) {
        SHR_IF_ERR_EXIT
            (bcmltm_db_xfrm_in_key_fields_get(unit, sid, xfrm_type, xfrm_idx,
                                              &in_key_fields));
        if (in_key_fields != NULL) {
            SHR_IF_ERR_EXIT
                (bcmltm_md_fa_node_data_create(unit,
                                               bcmltm_fa_node_lta_input_init,
                                               (void *)in_key_fields,
                                               &init_in_key_node_data));
            init_in_key_node =
                bcmltm_tree_left_node_create(init_out_node,
                                             (void *)init_in_key_node_data,
                                             &(op_md_ptr->num_nodes));
            SHR_NULL_CHECK(init_in_key_node, SHR_E_MEMORY);
        }
    }
    /**************************************************************************
     * #4. bcmltm_fa_node_wbc_to_lta_fields : Copy PTM to LTA in(dst) fields.
 */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       bcmltm_fa_node_wbc_to_lta_fields,
                                       (void *)in_fields,
                                       &wbc_to_lta_node_data));
    wbc_to_lta_node = bcmltm_tree_left_node_create(lta_xfrm_param_node,
                                                  (void *)wbc_to_lta_node_data,
                                                   &(op_md_ptr->num_nodes));
    SHR_NULL_CHECK(wbc_to_lta_node, SHR_E_MEMORY);

    /**************************************************************************
     * #5. bcmltm_fa_node_api_key_fields_to_lta: Copy API key fields to LTA in_key(src_key) fields.
 */
    if (is_value_xfrm) {
        if (in_key_fields != NULL) {
            if (opcode == BCMLT_OPCODE_TRAVERSE) {
                lta_to_lta_func = bcmltm_fa_node_lta_to_lta_fields;
                rev_xfrm_type = BCMLTM_DB_XFRM_TYPE_REV_KEY_TRANSFORM;
                SHR_IF_ERR_EXIT
                    (bcmltm_db_xfrm_num_get(unit, sid,
                                            rev_xfrm_type, &num_rev_xfrm));

                if (num_rev_xfrm > 0) {
                    /* Add Copy subtrees */
                    copy_node_root = lta_xfrm_param_node;
                    for (rev_xfrm_idx = 0;
                         rev_xfrm_idx < num_rev_xfrm;
                         rev_xfrm_idx++) {
                        SHR_IF_ERR_EXIT
                            (bcmltm_db_xfrm_copy_fields_get(unit, sid,
                                                            xfrm_type,
                                                            xfrm_idx,
                                                            rev_xfrm_type,
                                                            rev_xfrm_idx,
                                                            &copy_fields));

                        if (copy_fields == NULL) {
                            /* No field in this crossbar, skip. */
                            continue;
                        }

                        SHR_IF_ERR_EXIT
                            (bcmltm_md_fa_node_data_create(unit,
                                                           lta_to_lta_func,
                                                           (void *)copy_fields,
                                                           &copy_node_data));
                        copy_node =
                            bcmltm_tree_right_node_create(copy_node_root,
                                                   (void *)copy_node_data,
                                                   &(op_md_ptr->num_nodes));
                        SHR_NULL_CHECK(copy_node, SHR_E_MEMORY);
                        /* Build a tree downward as needed */
                        copy_node_root = copy_node;
                    }
                }
            } else {
                SHR_IF_ERR_EXIT
                    (bcmltm_md_fa_node_data_create(unit,
                                               bcmltm_fa_node_api_key_fields_to_lta,
                                               (void *)in_key_fields,
                                               &api_key_to_lta_node_data));
                api_key_to_lta_node =
                    bcmltm_tree_right_node_create(lta_xfrm_param_node,
                                             (void *)api_key_to_lta_node_data,
                                                  &(op_md_ptr->num_nodes));
                SHR_NULL_CHECK(api_key_to_lta_node, SHR_E_MEMORY);
            }
        }
    }

    *next_chain = next_chain_node;
 exit:
    if (SHR_FUNC_ERR()) {
        /*
         * Need to explicitly destroy the fa_node (node data)
         * if corresponding tree creation was not successful.
         */
        /* #8 */
        if (lta_to_api_node == NULL) {
            SHR_FREE(lta_to_api_node_data);
        }
        /* #3 */
        if (init_in_node == NULL) {
            SHR_FREE(init_in_node_data);
        }
        /* #1 */
        if (init_in_key_node == NULL) {
            SHR_FREE(init_in_key_node_data);
        }
        /* #6 */
        if (lta_xfrm_param_node == NULL) {
            SHR_FREE(lta_xfrm_param_data);
        }
        /* #2 */
        if (init_out_node == NULL) {
            SHR_FREE(init_out_node_data);
        }
        /* #4 */
        if (wbc_to_lta_node == NULL) {
            SHR_FREE(wbc_to_lta_node_data);
        }
        /* #5 */
        if (api_key_to_lta_node == NULL) {
            SHR_FREE(api_key_to_lta_node_data);
        }
    }

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_md_xfrm_fa_trees_fwd_create(int unit,
                                   bcmlrd_sid_t sid,
                                   int xfrm_type,
                                   bcmltm_md_lta_fopt_t lta_fopt,
                                   bcmlt_opcode_t opcode,
                                   bcmltm_node_t *parent_chain,
                                   bcmltm_lt_op_md_t *op_md_ptr,
                                   bcmltm_node_t **next_chain)
{
    uint32_t idx;
    uint32_t num_xfrm = 0;
    bcmltm_node_t *parent_node = NULL;

    SHR_FUNC_ENTER(unit);

    *next_chain = parent_chain;

    SHR_IF_ERR_EXIT
        (bcmltm_db_xfrm_num_get(unit, sid, xfrm_type, &num_xfrm));

    if (num_xfrm > 0) {
        /* Set parent node for Xfrm */
        parent_node = parent_chain;

        /* Add Xfrm subtrees */
        for (idx = 0; idx < num_xfrm; idx++) {
            SHR_IF_ERR_EXIT
                (xfrm_fa_node_tree_fwd_create(unit, sid,
                                              xfrm_type, idx, lta_fopt,
                                              opcode,
                                              parent_node, op_md_ptr,
                                              next_chain));
            parent_node = *next_chain;
        }
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_md_xfrm_fa_trees_rev_create(int unit,
                                   bcmlrd_sid_t sid,
                                   int xfrm_type,
                                   bcmlt_opcode_t opcode,
                                   bcmltm_node_t *parent_chain,
                                   bcmltm_lt_op_md_t *op_md_ptr,
                                   bcmltm_node_t **next_chain)
{
    uint32_t idx;
    uint32_t num_xfrm = 0;
    bcmltm_node_t *parent_node = NULL;

    SHR_FUNC_ENTER(unit);

    *next_chain = parent_chain;

    SHR_IF_ERR_EXIT
        (bcmltm_db_xfrm_num_get(unit, sid, xfrm_type, &num_xfrm));

    if (num_xfrm > 0) {
        /* Set parent node for Xfrm */
        parent_node = parent_chain;

        /* Add Xfrm subtrees */
        for (idx = 0; idx < num_xfrm; idx++) {
            SHR_IF_ERR_EXIT
                (xfrm_fa_node_tree_rev_create(unit, sid,
                                              xfrm_type, idx,
                                              opcode,
                                              parent_node, op_md_ptr,
                                              next_chain));
            parent_node = *next_chain;
        }
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_md_xfrm_fa_trees_fwd_alloc_create(int unit,
                                         bcmlrd_sid_t sid,
                                         bcmlt_opcode_t opcode,
                                         bcmltm_node_t *parent_chain,
                                         bcmltm_lt_op_md_t *op_md_ptr,
                                         bcmltm_node_t **next_chain)
{
    const bcmltm_table_attr_t *table_attr;
    uint32_t idx;
    uint32_t num_xfrm = 0;
    bcmltm_node_t *parent_node = NULL;
    int xfrm_type = BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM;
    bcmltm_md_lta_fopt_t lta_fopt = 0x0;
    const bcmltm_lta_transform_params_t *xfrm_params = NULL;
    bool alloc;

    SHR_FUNC_ENTER(unit);

    *next_chain = parent_chain;

    /* Get table attributes */
    SHR_IF_ERR_EXIT
        (bcmltm_db_table_attr_get(unit, sid, &table_attr));

    SHR_IF_ERR_EXIT
        (bcmltm_db_xfrm_num_get(unit, sid, xfrm_type, &num_xfrm));

    if (num_xfrm > 0) {
        /* Set parent node for Xfrm */
        parent_node = parent_chain;

        /* Add Xfrm subtrees */
        for (idx = 0; idx < num_xfrm; idx++) {

            /*
             * Determine if transform contains allocatable (optional)
             * key fields.
             * Assumes transform construction is correct, i.e. IwA
             * transform should not contain a mix of required and optional
             * fields.
             */
            SHR_IF_ERR_EXIT
                (bcmltm_db_xfrm_params_get(unit, sid,
                                           xfrm_type, idx,
                                           &xfrm_params));

            alloc = bcmltm_db_xfrm_key_is_alloc(table_attr,
                                                xfrm_type,
                                                xfrm_params->lta_args);
            if (alloc) {
                /* Keys are allocatable */
                lta_fopt |= BCMLTM_MD_LTA_FOPT_IN_API_ALLOC;
            } else {
                /* Keys are required */
                lta_fopt = 0x0;
            }

            SHR_IF_ERR_EXIT
                (xfrm_fa_node_tree_fwd_create(unit, sid,
                                              xfrm_type, idx, lta_fopt,
                                              opcode,
                                              parent_node, op_md_ptr,
                                              next_chain));
            parent_node = *next_chain;
        }
    }

 exit:
    SHR_FUNC_EXIT();
}


int
bcmltm_md_xfrm_fa_trees_rev_alloc_create(int unit,
                                         bcmlrd_sid_t sid,
                                         bcmlt_opcode_t opcode,
                                         bcmltm_node_t *parent_chain,
                                         bcmltm_lt_op_md_t *op_md_ptr,
                                         bcmltm_node_t **next_chain)
{
    const bcmltm_table_attr_t *table_attr;
    uint32_t idx;
    uint32_t num_xfrm = 0;
    bcmltm_node_t *parent_node = NULL;
    int xfrm_type = BCMLTM_DB_XFRM_TYPE_REV_KEY_TRANSFORM;
    const bcmltm_lta_transform_params_t *xfrm_params = NULL;
    bool alloc;

    SHR_FUNC_ENTER(unit);

    *next_chain = parent_chain;

    /* Sanity check */
    if (opcode != BCMLT_OPCODE_INSERT) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Invalid opcode for "
                              "xfrm_fa_trees_rev_alloc_create: "
                              "%s(ltid=%d) expected_op=%d actual_op=%d\n"),
                   table_name, sid, BCMLT_OPCODE_INSERT, opcode));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get table attributes */
    SHR_IF_ERR_EXIT
        (bcmltm_db_table_attr_get(unit, sid, &table_attr));

    SHR_IF_ERR_EXIT
        (bcmltm_db_xfrm_num_get(unit, sid, xfrm_type, &num_xfrm));

    if (num_xfrm > 0) {
        /* Set parent node for Xfrm */
        parent_node = parent_chain;

        /* Add Xfrm subtrees */
        for (idx = 0; idx < num_xfrm; idx++) {

            /*
             * Install only transforms that contain the allocatable
             * (optional) key fields.
             * If optional key fields are absent during input, they
             * are allocated and returned in the API field list.
             */
            SHR_IF_ERR_EXIT
                (bcmltm_db_xfrm_params_get(unit, sid,
                                           xfrm_type, idx,
                                           &xfrm_params));
            alloc = bcmltm_db_xfrm_key_is_alloc(table_attr,
                                                xfrm_type,
                                                xfrm_params->lta_args);
            if (!alloc) {
                continue;
            }

            /* Transform contains optional keys */
            SHR_IF_ERR_EXIT
                (xfrm_fa_node_tree_rev_create(unit, sid,
                                              xfrm_type, idx,
                                              opcode,
                                              parent_node, op_md_ptr,
                                              next_chain));
            parent_node = *next_chain;
        }
    }

 exit:
    SHR_FUNC_EXIT();
}
