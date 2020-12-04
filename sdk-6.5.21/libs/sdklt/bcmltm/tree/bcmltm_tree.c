/*! \file bcmltm_tree.c
 *
 * Logical Table Manager Binary Tree utilities.
 *
 * This file contains the LTM binary tree functions which assemble,
 * maintain, and execute the binary trees which comprise the
 * Field Adaptation and Execution Engine stages of LTM.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <bcmdrd/bcmdrd_chip.h>
#include <bcmltm/bcmltm_tree.h>


/*******************************************************************************
 * Private functions
 */

/*******************************************************************************
 * Public functions
 */

/*
 * Allocate space for a root binary tree node and return pointer to it.
 *
 * @@@ Do we need any other arguments here?
 */
bcmltm_node_t *
bcmltm_tree_node_create(void *node_data,
                        bcmltm_tree_size_t *tree_size)
{
    bcmltm_node_t *new_node;

    /* @@@ Allocate the node memory in some way @@@ */
    new_node = sal_alloc(sizeof(bcmltm_node_t), "bcmltmTreeNode");

    if (new_node != NULL) {
        sal_memset(new_node, 0, sizeof(bcmltm_node_t));
        new_node->node_data = node_data;
        if (node_data != NULL) {
            /* Count this as an active node. */
            tree_size->active_node_count++;
        }
        tree_size->node_count++;
    }

    return new_node;
}

/*
 * Allocate space for a binary tree node, attach to left of parent node,
 * and return pointer the new node.
 *
 * @@@ Do we need any other arguments here?
 */
bcmltm_node_t *
bcmltm_tree_left_node_create(bcmltm_node_t *parent_node,
                             void *node_data,
                             bcmltm_tree_size_t *tree_size)
{
    if (parent_node->left != NULL) {
        /* This will leak memory, clear old node first. */
        return NULL;
    }

    parent_node->left = bcmltm_tree_node_create(node_data, tree_size);

    return parent_node->left;
}

/*
 * Allocate space for a binary tree node, attach to right of parent node,
 * and return pointer the new node.
 *
 * @@@ Do we need any other arguments here?
 */
bcmltm_node_t *
bcmltm_tree_right_node_create(bcmltm_node_t *parent_node,
                              void *node_data,
                              bcmltm_tree_size_t *tree_size)
{
    if (parent_node->right != NULL) {
        /* This will leak memory, clear old node first. */
        return NULL;
    }

    parent_node->right = bcmltm_tree_node_create(node_data, tree_size);

    return parent_node->right;
}

/*!
 * \brief Release the memory of a binary LTM op tree.
 *
 * This routine traverses one binary tree of LTM op function nodes in
 * post-order.  It releases the memory used by the tree nodes.
 *
 * Optionally, the node_data stored by each tree node may be released to
 * cover error conditions during initialization.
 *
 * \param [in] root_node Pointer to root node of tree.
 * \param [in] tree_size Pointer to tree node counting structure.
 * \param [in] free_data TRUE if node_data should be freed also.
 *
 * \retval None.
 */
void
bcmltm_tree_destroy(bcmltm_node_t *root_node,
                    bcmltm_tree_size_t *tree_size,
                    bool free_data)
{
    uint32_t size;
    uint32_t parent_idx;
    bcmltm_node_t **parent_nodes = NULL;
    bcmltm_node_t *current_node;
    bcmltm_node_traverse_state_t state = BCMLTM_TREE_TRAVERSE_STATE_LEFT;

    if (root_node == NULL) {
        /* Nothing to do */
        return;
    }

    /* Allocate tree traverse parent records */
    size = sizeof(bcmltm_node_t *) * tree_size->node_count;
    if (size == 0) {
        /* No nodes */
        return;
    }
    parent_nodes = sal_alloc(size, "bcmltmTreeTraverseParents");
    if (parent_nodes == NULL) {
        /* Failed to get memory for parent node record */
        return;
    }
    sal_memset(parent_nodes, 0, size);

    parent_idx = 0; /* Stack pointer for parent nodes back to root. */
    current_node = root_node;

    while (!((current_node == root_node) &&
            (state == BCMLTM_TREE_TRAVERSE_STATE_UP))) {
        if (parent_idx >= tree_size->node_count) {
            /* Exhausted parent node stack space. */
            SHR_FREE(parent_nodes);
            return;
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
            /* Check to discard node_data */
            if (free_data && (current_node->node_data != NULL)) {
                SHR_FREE(current_node->node_data);
            }
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
            /* Free node structure, now that we are finished with it. */
            SHR_FREE(current_node);
            current_node = parent_nodes[parent_idx];
            break;
        default:
            /* Unexpected state */
            SHR_FREE(parent_nodes);
            return;
        }
    }

    /* Free root_node structure, now that we are finished with it. */
    SHR_FREE(root_node);
    SHR_FREE(parent_nodes);
}


