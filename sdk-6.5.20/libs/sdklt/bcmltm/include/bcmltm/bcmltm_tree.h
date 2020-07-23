/*! \file bcmltm_tree.h
 *
 * Logical Table Manager Binary Tree.
 *
 * A set of functions and definitions for binary trees used for the LTM
 * stages.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_TREE_H
#define BCMLTM_TREE_H

#include <shr/shr_types.h>

/*!
 * \brief Tree traverse state
 *
 * Indicates the current status of a tree post-order traversal.
 */
typedef enum bcmltm_node_traverse_state_e {
    BCMLTM_TREE_TRAVERSE_STATE_LEFT,
    BCMLTM_TREE_TRAVERSE_STATE_RIGHT,
    BCMLTM_TREE_TRAVERSE_STATE_CURRENT,
    BCMLTM_TREE_TRAVERSE_STATE_UP,
    BCMLTM_TREE_TRAVERSE_STATE_COUNT
} bcmltm_node_traverse_state_t;

/*!
 * \brief  Basic LTM tree structure component.
 */
typedef struct bcmltm_node_s {
    /*! Untyped pointer to structure comprising this node. */
    void *node_data;

    /*! Pointer to a subnode on the "left" side of the binary tree. */
    struct bcmltm_node_s *left;

    /*! Pointer to a subnode on the "right" side of the binary tree. */
    struct bcmltm_node_s *right;
} bcmltm_node_t;

/*!
 * \brief  Counters for tree nodes.
 */
typedef struct bcmltm_tree_size_s {
    /*! The total number of nodes in a tree set. */
    uint32_t node_count;

    /*! The total number of active nodes (node_data != NULL) in a tree set. */
    uint32_t active_node_count;

    /*! The number of active nodes in the first FA tree. */
    uint32_t first_tree_node_count;
} bcmltm_tree_size_t;

/*!
 * \brief Node operation function callback.
 *
 * The traversal operation function to invoke on each node
 * of an LTM binary tree in post-order.
 *
 * \param [in] unit Unit number.
 * \param [in] node_data The context information supplied to the
 *             per-node operational function.
 * \param [in] traverse_cookie A pointer to the context information for this
 *             traverse function.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*bcmltm_tree_traverse_op_f)(int unit,
                                         void *node_data,
                                         void *traverse_cookie);

/*!
 * \brief Create a generic LTM binary tree node.
 *
 * Allocate and initialize a node which may be part of a binary tree.
 *
 * \param [in] node_data Generic pointer to the structure for this node.
 * \param [in,out] tree_size Pointer to bcmltm_tree_size_t structure.
 *
 * \retval Pointer to the new allocated tree, or NULL if failure to allocate.
 */
extern bcmltm_node_t *
bcmltm_tree_node_create(void *node_data,
                        bcmltm_tree_size_t *tree_size);

/*!
 * \brief Create a left-hand generic LTM binary tree node.
 *
 * Allocate and initialize a node which may be part of a binary tree.
 * Attach the new node to the left-hand node pointer of the provided node.
 *
 * \param [in] parent_node An existing tree node to which a new
 *        left-hand node should be attached.
 * \param [in] node_data A generic pointer to information describing
 *        the new node.
 * \param [in,out] tree_size Pointer to bcmltm_tree_size_t structure.
 *
 * \retval Pointer to the new allocated tree, or NULL if failure to allocate.
 */
extern bcmltm_node_t *
bcmltm_tree_left_node_create(bcmltm_node_t *parent_node,
                             void *node_data,
                             bcmltm_tree_size_t *tree_size);

/*!
 * \brief Create a right-hand generic LTM binary tree node.
 *
 * Allocate and initialize a node which may be part of a binary tree.
 * Attach the new node to the right-hand node pointer of the provided node.
 *
 * \param [in] parent_node An existing tree node to which a new
 *        right-hand node should be attached.
 * \param [in] node_data A generic pointer to information describing
 *        the new node.
 * \param [in,out] tree_size Pointer to bcmltm_tree_size_t structure.
 *
 * \retval Pointer to the new allocated tree, or NULL if failure to allocate.
 */
extern bcmltm_node_t *
bcmltm_tree_right_node_create(bcmltm_node_t *parent_node,
                              void *node_data,
                              bcmltm_tree_size_t *tree_size);

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
extern void
bcmltm_tree_destroy(bcmltm_node_t *root_node,
                    bcmltm_tree_size_t *tree_size,
                    bool free_data);

#endif /* BCMLTM_TREE_H */
