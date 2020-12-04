/*! \file bcmltm_md_op.c
 *
 * Logical Table Manager - Operation Metadata Definitions.
 *
 * This file contains implementations that are generic and
 * can be used by different types of Logical Tables during the
 * metadata construction.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_types.h>
#include <shr/shr_debug.h>

#include <bsl/bsl.h>

#include <bcmltd/bcmltd_types.h>

#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_ee_tree_nodes_internal.h>

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
 * \brief Create FA node data with given information.
 *
 * This routine creates a FA node data if all following conditions are met:
 *   - Node function must not be null.
       Tree traverse always calls the node function.
 *   - If node_cookie_required is TRUE, node cookie must not be null.
 *     Some node functions expect the presence of a node cookie whereas
 *     others do not.
 *
 * If the above conditions are not met, the node data returned is null.
 * A null node data is useful to create empty nodes in the tree, where
 * a particular step does not apply, and produce the same tree layout.
 *
 * \param [in] unit Unit number.
 * \param [in] node_function FA function for node.
 * \param [in] node_cookie Context information for given FA function.
 * \param [in] node_cookie_required Indicates whether node cookie is needed.
 * \param [out] fa_node_ptr Returning pointer to FA node data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
fa_node_data_create(int unit,
                    bcmltm_fa_node_f node_function,
                    void *node_cookie,
                    bool node_cookie_required,
                    bcmltm_fa_node_t **fa_node_ptr)
{
    bcmltm_fa_node_t *fa_node = NULL;

    SHR_FUNC_ENTER(unit);

    *fa_node_ptr = NULL;

    /* Checks to create node data */
    if (node_function == NULL) {
        SHR_EXIT();
    }
    if (node_cookie_required && (node_cookie == NULL)) {
        SHR_EXIT();
    }

    /* Allocate */
    SHR_ALLOC(fa_node, sizeof(*fa_node), "bcmltmFaNode");
    SHR_NULL_CHECK(fa_node, SHR_E_MEMORY);
    sal_memset(fa_node, 0, sizeof(*fa_node));

    /* Fill data */
    fa_node->node_function = node_function;
    fa_node->node_cookie = node_cookie;

    *fa_node_ptr = fa_node;

 exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(fa_node);
        *fa_node_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Create EE node data with given information.
 *
 * This routine creates a EE node data if all following conditions are met:
 *   - Node function must not be null.
       Tree traverse always calls the node function.
 *   - If node_cookie_required is TRUE, node cookie must not be null.
 *     Some node functions expect the presence of a node cookie whereas
 *     others do not.
 *
 * If the above conditions are not met, the node data returned is null.
 * A null node data is useful to create empty nodes in the tree, where
 * a particular step does not apply, and produce the same tree layout.
 *
 * \param [in] unit Unit number.
 * \param [in] node_function EE function for node.
 * \param [in] node_cookie Context information for given EE function.
 * \param [in] node_cookie_required Indicates whether node cookie is needed.
 * \param [out] ee_node_ptr Returning pointer to EE node data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ee_node_data_create(int unit,
                    bcmltm_ee_node_f node_function,
                    void *node_cookie,
                    bool node_cookie_required,
                    bcmltm_ee_node_t **ee_node_ptr)
{
    bcmltm_ee_node_t *ee_node = NULL;

    SHR_FUNC_ENTER(unit);

    *ee_node_ptr = NULL;

    /* Checks to create node data */
    if (node_function == NULL) {
        SHR_EXIT();
    }
    if (node_cookie_required && (node_cookie == NULL)) {
        SHR_EXIT();
    }

    /* Allocate */
    SHR_ALLOC(ee_node, sizeof(*ee_node), "bcmltmEeNode");
    SHR_NULL_CHECK(ee_node, SHR_E_MEMORY);
    sal_memset(ee_node, 0, sizeof(*ee_node));

    /* Fill data */
    ee_node->node_function = node_function;
    ee_node->node_cookie = node_cookie;

    *ee_node_ptr = ee_node;

 exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(ee_node);
        *ee_node_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert one binary tree of function nodes to simple array
 *
 * This routine traverses one binary tree of LTM op function nodes in
 * post-order.  It apends the node structure pointer to an array of
 * such pointers.
 *
 * This process results in the set of node trees expressed instead as
 * a simple array of nodes for faster runtime processing.
 *
 * \param [in] unit Unit number.
 * \param [in] root_node Pointer to root node of tree.
 * \param [in] tree_size Pointer to tree node counting structure.
 * \param [in,out] parent_nodes Pointer to stack of tree node pointers.
 * \param [in,out] node_array Pointer to a simple array of nodes.
 * \param [in,out] array_idx Pointer to running count of nodes in node_array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
md_op_tree_to_array(int unit,
                    bcmltm_node_t *root_node,
                    bcmltm_tree_size_t *tree_size,
                    bcmltm_node_t **parent_nodes,
                    bcmltm_op_node_t **node_array,
                    uint32_t *array_idx)
{
    uint32_t parent_idx;
    bcmltm_node_t *current_node;
    bcmltm_node_traverse_state_t state = BCMLTM_TREE_TRAVERSE_STATE_LEFT;

    SHR_FUNC_ENTER(unit);

    parent_idx = 0; /* Stack pointer for parent nodes back to root. */
    current_node = root_node;

    while (!((current_node == root_node) &&
            (state == BCMLTM_TREE_TRAVERSE_STATE_UP))) {
        if (parent_idx >= tree_size->node_count) {
            /* Exhausted parent node stack space. */
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        switch (state) {
        case BCMLTM_TREE_TRAVERSE_STATE_LEFT:
            if (current_node->left != NULL) {
                /* If left branch exists, continue down that side. */
                parent_nodes[parent_idx++] = current_node;
                current_node = current_node->left;
            } else {
                /* Else check right side. */
                state = BCMLTM_TREE_TRAVERSE_STATE_RIGHT;
            }
            break;
        case BCMLTM_TREE_TRAVERSE_STATE_RIGHT:
            if (current_node->right != NULL) {
                /* If right branch exists, continue down that side. */
                parent_nodes[parent_idx++] = current_node;
                current_node = current_node->right;
                state = BCMLTM_TREE_TRAVERSE_STATE_LEFT;
            } else {
                /* Else record this node. */
                state = BCMLTM_TREE_TRAVERSE_STATE_CURRENT;
            }
            break;
        case BCMLTM_TREE_TRAVERSE_STATE_CURRENT:
            if (current_node->node_data != NULL) {
                /* Add this node to array. */
                if (*array_idx >= tree_size->active_node_count) {
                    /* Too many nodes for array. */
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
                node_array[*array_idx] =
                    (bcmltm_op_node_t *) current_node->node_data;
                (*array_idx)++;
            } /* Else blank node to skip */
            state = BCMLTM_TREE_TRAVERSE_STATE_UP;
            break;
        case BCMLTM_TREE_TRAVERSE_STATE_UP:
            /*
             * Ascend tree to parent node.
             * If this node was left of parent, check right.
             * Otherwise process parent.
             */
            parent_idx--;
            state = (current_node == parent_nodes[parent_idx]->left) ?
                BCMLTM_TREE_TRAVERSE_STATE_RIGHT :
                BCMLTM_TREE_TRAVERSE_STATE_CURRENT;
            current_node = parent_nodes[parent_idx];
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

 exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_md_fa_node_data_create(int unit,
                              bcmltm_fa_node_f node_function,
                              void *node_cookie,
                              bcmltm_fa_node_t **fa_node_ptr)
{
    return fa_node_data_create(unit, node_function, node_cookie, TRUE,
                               fa_node_ptr);
}


int
bcmltm_md_fa_node_data_nc_opt_create(int unit,
                                     bcmltm_fa_node_f node_function,
                                     void *node_cookie,
                                     bcmltm_fa_node_t **fa_node_ptr)
{
    return fa_node_data_create(unit, node_function, node_cookie, FALSE,
                               fa_node_ptr);
}

int
bcmltm_md_fa_node_create(int unit,
                         bcmltm_fa_node_f node_func,
                         void *node_cookie,
                         bcmltm_node_side_t node_side,
                         bcmltm_node_t *parent_node,
                         bcmltm_lt_op_md_t *op_md_ptr,
                         bcmltm_node_t **node_ptr)
{
    bcmltm_node_t *node = NULL;
    bcmltm_fa_node_t *node_data = NULL;

    SHR_FUNC_ENTER(unit);

    *node_ptr = NULL;

    /* Create node data */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_create(unit,
                                       node_func, node_cookie,
                                       &node_data));

    /* Create new node in specified side/location */
    if (node_side == BCMLTM_NODE_SIDE_LEFT) {
        node = bcmltm_tree_left_node_create(parent_node,
                                            (void *)node_data,
                                            &(op_md_ptr->num_nodes));
    } else if (node_side == BCMLTM_NODE_SIDE_RIGHT) {
        node = bcmltm_tree_right_node_create(parent_node,
                                             (void *)node_data,
                                             &(op_md_ptr->num_nodes));
    } else {
        node = bcmltm_tree_node_create((void *)node_data,
                                       &(op_md_ptr->num_nodes));
    }

    SHR_NULL_CHECK(node, SHR_E_MEMORY);

    *node_ptr = node;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy node data if it is not in node tree */
        if (node == NULL) {
            SHR_FREE(node_data);
        }
    }

    SHR_FUNC_EXIT();
}

int
bcmltm_md_fa_node_nc_opt_create(int unit,
                                bcmltm_fa_node_f node_func,
                                void *node_cookie,
                                bcmltm_node_side_t node_side,
                                bcmltm_node_t *parent_node,
                                bcmltm_lt_op_md_t *op_md_ptr,
                                bcmltm_node_t **node_ptr)
{
    bcmltm_node_t *node = NULL;
    bcmltm_fa_node_t *node_data = NULL;

    SHR_FUNC_ENTER(unit);

    *node_ptr = NULL;

    /* Create node data */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_data_nc_opt_create(unit,
                                              node_func, node_cookie,
                                              &node_data));

    /* Create new node in specified side/location */
    if (node_side == BCMLTM_NODE_SIDE_LEFT) {
        node = bcmltm_tree_left_node_create(parent_node,
                                            (void *)node_data,
                                            &(op_md_ptr->num_nodes));
    } else if (node_side == BCMLTM_NODE_SIDE_RIGHT) {
        node = bcmltm_tree_right_node_create(parent_node,
                                             (void *)node_data,
                                             &(op_md_ptr->num_nodes));
    } else {
        node = bcmltm_tree_node_create((void *)node_data,
                                       &(op_md_ptr->num_nodes));
    }

    SHR_NULL_CHECK(node, SHR_E_MEMORY);

    *node_ptr = node;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy node data if it is not in node tree */
        if (node == NULL) {
            SHR_FREE(node_data);
        }
    }

    SHR_FUNC_EXIT();
}

int
bcmltm_md_ee_node_data_create(int unit,
                              bcmltm_ee_node_f node_function,
                              void *node_cookie,
                              bcmltm_ee_node_t **ee_node_ptr)
{
    return ee_node_data_create(unit, node_function, node_cookie, TRUE,
                               ee_node_ptr);
}


int
bcmltm_md_ee_node_data_nc_opt_create(int unit,
                                     bcmltm_ee_node_f node_function,
                                     void *node_cookie,
                                     bcmltm_ee_node_t **ee_node_ptr)
{
    return ee_node_data_create(unit, node_function, node_cookie, FALSE,
                               ee_node_ptr);
}


int
bcmltm_md_ee_node_create(int unit,
                         bcmltm_ee_node_f node_func,
                         void *node_cookie,
                         bcmltm_node_side_t node_side,
                         bcmltm_node_t *parent_node,
                         bcmltm_lt_op_md_t *op_md_ptr,
                         bcmltm_node_t **node_ptr)
{
    bcmltm_node_t *node = NULL;
    bcmltm_ee_node_t *node_data = NULL;

    SHR_FUNC_ENTER(unit);

    *node_ptr = NULL;

    /* Create node data */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_create(unit,
                                       node_func, node_cookie,
                                       &node_data));

    /* Create new node in specified side/location */
    if (node_side == BCMLTM_NODE_SIDE_LEFT) {
        node = bcmltm_tree_left_node_create(parent_node,
                                            (void *)node_data,
                                            &(op_md_ptr->num_nodes));
    } else if (node_side == BCMLTM_NODE_SIDE_RIGHT) {
        node = bcmltm_tree_right_node_create(parent_node,
                                             (void *)node_data,
                                             &(op_md_ptr->num_nodes));
    } else {
        node = bcmltm_tree_node_create((void *)node_data,
                                       &(op_md_ptr->num_nodes));
    }

    SHR_NULL_CHECK(node, SHR_E_MEMORY);

    *node_ptr = node;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy node data if it is not in node tree */
        if (node == NULL) {
            SHR_FREE(node_data);
        }
    }

    SHR_FUNC_EXIT();
}

int
bcmltm_md_ee_node_nc_opt_create(int unit,
                                bcmltm_ee_node_f node_func,
                                void *node_cookie,
                                bcmltm_node_side_t node_side,
                                bcmltm_node_t *parent_node,
                                bcmltm_lt_op_md_t *op_md_ptr,
                                bcmltm_node_t **node_ptr)
{
    bcmltm_node_t *node = NULL;
    bcmltm_ee_node_t *node_data = NULL;

    SHR_FUNC_ENTER(unit);

    *node_ptr = NULL;

    /* Create node data */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_data_nc_opt_create(unit,
                                              node_func, node_cookie,
                                              &node_data));

    /* Create new node in specified side/location */
    if (node_side == BCMLTM_NODE_SIDE_LEFT) {
        node = bcmltm_tree_left_node_create(parent_node,
                                            (void *)node_data,
                                            &(op_md_ptr->num_nodes));
    } else if (node_side == BCMLTM_NODE_SIDE_RIGHT) {
        node = bcmltm_tree_right_node_create(parent_node,
                                             (void *)node_data,
                                             &(op_md_ptr->num_nodes));
    } else {
        node = bcmltm_tree_node_create((void *)node_data,
                                       &(op_md_ptr->num_nodes));
    }

    SHR_NULL_CHECK(node, SHR_E_MEMORY);

    *node_ptr = node;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy node data if it is not in node tree */
        if (node == NULL) {
            SHR_FREE(node_data);
        }
    }

    SHR_FUNC_EXIT();
}

void
bcmltm_md_node_roots_destroy(size_t num_roots,
                             bcmltm_node_t **node_roots,
                             bcmltm_tree_size_t *tree_size,
                             bool free_data)
{
    size_t root;

    if (node_roots == NULL) {
        return;
    }

    for (root = 0; root < num_roots; root++) {
        bcmltm_tree_destroy(node_roots[root], tree_size, free_data);
    }

    SHR_FREE(node_roots);

    return;
}

void
bcmltm_md_op_destroy(bcmltm_lt_op_md_t *op_md)
{
    uint32_t nix;

    if (op_md == NULL) {
        return;
    }

    /* Destroy EE and FA trees */
    bcmltm_md_node_roots_destroy(op_md->num_roots, op_md->ee_node_roots,
                                 &(op_md->num_nodes), TRUE);
    bcmltm_md_node_roots_destroy(op_md->num_roots, op_md->fa_node_roots,
                                 &(op_md->num_nodes), TRUE);

    if (op_md->nodes_array != NULL) {
        for (nix = 0; nix < op_md->num_nodes.active_node_count; nix++) {
            SHR_FREE(op_md->nodes_array[nix]);
        }
    }

    SHR_FREE(op_md->nodes_array);
    SHR_FREE(op_md);

    return;
}

int
bcmltm_md_ee_node_ptm_index_func_get(bool interactive,
                                     bcmltm_ee_node_f *read_func,
                                     bcmltm_ee_node_f *write_func)
{
    /* Set corresponding EE node function based on table operating mode */
    if (interactive) {
        /* Interactive path */
        if (read_func != NULL) {
            *read_func = bcmltm_ee_node_schan_read;
        }
        if (write_func != NULL) {
            *write_func = bcmltm_ee_node_schan_write;
        }

    } else {
        /* Modeled path */
        if (read_func != NULL) {
            *read_func = bcmltm_ee_node_model_index_read;
        }
        if (write_func != NULL) {
            *write_func = bcmltm_ee_node_model_index_write;
        }
    }

    return SHR_E_NONE;
}

int
bcmltm_md_op_trees_to_array(int unit,
                            bcmltm_lt_op_md_t *op_md)
{
    uint32_t size;
    bcmltm_op_node_t **node_array = NULL;
    bcmltm_node_t **parent_nodes = NULL;
    uint32_t trix, array_idx;
    bcmltm_tree_size_t *tree_size;

    SHR_FUNC_ENTER(unit);

    tree_size = &(op_md->num_nodes);

    /* Allocate node function array */
    size = sizeof(bcmltm_op_node_t *) * tree_size->active_node_count;
    if (size == 0) {
        /* No active nodes */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(node_array, size, "bcmltmOpNodeArray");
    SHR_NULL_CHECK(node_array, SHR_E_MEMORY);
    sal_memset(node_array, 0, size);

    array_idx = 0;

    /* Allocate tree traverse parent records */
    size = sizeof(bcmltm_node_t *) * tree_size->node_count;
    if (size == 0) {
        /* No nodes */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(parent_nodes, size, "bcmltmTreeTraverseParents");
    SHR_NULL_CHECK(parent_nodes, SHR_E_MEMORY);
    sal_memset(parent_nodes, 0, size);

    for (trix = 0; trix < op_md->num_roots; trix++) {
        /* Field Adaptation stage */
        if (op_md->fa_node_roots[trix] != NULL) {
            SHR_IF_ERR_EXIT
                (md_op_tree_to_array(unit,
                                     op_md->fa_node_roots[trix],
                                     tree_size,
                                     parent_nodes,
                                     node_array,
                                     &array_idx));
        }

        if (trix == 0) {
            /*
             * Record the number of active nodes in the first FA tree
             * for valid bitmap construction later.
             */
            tree_size->first_tree_node_count = array_idx;
        }

        /* Execution Engine stage */
        if (op_md->ee_node_roots[trix] != NULL) {
            SHR_IF_ERR_EXIT
                (md_op_tree_to_array(unit,
                                     op_md->ee_node_roots[trix],
                                     tree_size,
                                     parent_nodes,
                                     node_array,
                                     &array_idx));
        }
    }

    if (array_idx != tree_size->active_node_count) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    op_md->nodes_array = node_array;

 exit:
    /* Discard trees at this point. If error, also free node data. */
    bcmltm_md_node_roots_destroy(op_md->num_roots, op_md->fa_node_roots,
                                 tree_size, SHR_FUNC_ERR());
    op_md->fa_node_roots = NULL;
    bcmltm_md_node_roots_destroy(op_md->num_roots, op_md->ee_node_roots,
                                 tree_size, SHR_FUNC_ERR());
    op_md->ee_node_roots = NULL;

    SHR_FREE(parent_nodes);
    if (SHR_FUNC_ERR()) {
        SHR_FREE(node_array);
        op_md->nodes_array = NULL;
    }
    SHR_FUNC_EXIT();
}
