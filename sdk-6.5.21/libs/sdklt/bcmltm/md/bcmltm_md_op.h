/*! \file bcmltm_md_op.h
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

#ifndef BCMLTM_MD_OP_H
#define BCMLTM_MD_OP_H

#include <shr/shr_types.h>

#include <bcmltd/bcmltd_types.h>

#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_tree.h>

/*!
 * \brief Node side indicator.
 *
 * Indicates which side of a parent to attach new node to.
 * If ROOT type is specified, a root node is created (left/right
 * side is not applicable).
 */
typedef enum bcmltm_node_side_e {
    /*! Attach new node to left side of parent. */
    BCMLTM_NODE_SIDE_LEFT,

    /*! Attach new node to right side of parent. */
    BCMLTM_NODE_SIDE_RIGHT,

    /*! New node is a root. */
    BCMLTM_NODE_SIDE_ROOT,
} bcmltm_node_side_t;


/*!
 * \brief Create FA node data on valid node cookie.
 *
 * Create FA node data only if the given node cookie is not null.
 * The node data must always have a valid node function.
 *
 * This function is used for node functions that expect a node cookie.
 *
 * A null node data is useful to create empty nodes in the tree, where
 * a particular step do no apply, and produce the same tree layout.
 *
 * \param [in] unit Unit number.
 * \param [in] node_function FA function for node.
 * \param [in] node_cookie Context information for given FA function.
 * \param [out] fa_node_ptr Returning pointer to FA node data.
 *                          A null is returned if the node function or
 *                          node cookie is null.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_fa_node_data_create(int unit,
                              bcmltm_fa_node_f node_function,
                              void *node_cookie,
                              bcmltm_fa_node_t **fa_node_ptr);

/*!
 * \brief Create FA node data with optional node cookie.
 *
 * Create FA node data with an optional node cookie.
 * The node data must always have a valid node function.
 *
 * This function is used for node functions that may or may not
 * expect a node cookie.
 *
 * A null node data is useful to create empty nodes in the tree, where
 * a particular step does not apply, and produce the same tree layout.
 *
 * \param [in] unit Unit number.
 * \param [in] node_function FA function for node.
 * \param [in] node_cookie Context information for given FA function.
 * \param [out] fa_node_ptr Returning pointer to FA node data.
 *                          A null is returned if the node function is null.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_fa_node_data_nc_opt_create(int unit,
                                     bcmltm_fa_node_f node_function,
                                     void *node_cookie,
                                     bcmltm_fa_node_t **fa_node_ptr);

/*!
 * \brief Create FA node with given node information.
 *
 * This routine creates an FA node with the given
 * information and attach it to the specified side
 * of the parent node, when applicable (not a ROOT).
 *
 * If given node function or node cookie is NULL, an empty pass-thru
 * node is created (node data is NULL).  This is useful to create
 * a tree layout that applies to various cases where some
 * steps may not apply.
 *
 * This function is used for node functions that expect a node cookie.
 *
 * \param [in] unit Unit number.
 * \param [in] node_func FA function node.
 * \param [in] node_cookie FA node cookie.
 * \param [in] node_side Side of parent node to attach node to or ROOT.
 * \param [in] parent_node Parent node to attach new node to.
 *                         Ignored if specified side is a ROOT.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] node_ptr Returning pointer to created node.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_fa_node_create(int unit,
                         bcmltm_fa_node_f node_func,
                         void *node_cookie,
                         bcmltm_node_side_t node_side,
                         bcmltm_node_t *parent_node,
                         bcmltm_lt_op_md_t *op_md_ptr,
                         bcmltm_node_t **node_ptr);

/*!
 * \brief Create FA node with given node information (optional node cookie).
 *
 * This routine creates an FA node with the given
 * information and attach it to the specified side
 * of the parent node, when applicable (not a ROOT).
 *
 * If given node function is NULL, an empty pass-thru
 * node is created (node data is NULL).  This is useful to create
 * a tree layout that applies to various cases where some
 * steps may not apply.
 *
 * This function is used for node functions that may or may not
 * expect a node cookie.
 *
 * \param [in] unit Unit number.
 * \param [in] node_func FA function node.
 * \param [in] node_cookie FA node cookie.
 * \param [in] node_side Side of parent node to attach node to or ROOT.
 * \param [in] parent_node Parent node to attach new node to.
 *                         Ignored if specified side is a ROOT.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] node_ptr Returning pointer to created node.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_fa_node_nc_opt_create(int unit,
                                bcmltm_fa_node_f node_func,
                                void *node_cookie,
                                bcmltm_node_side_t node_side,
                                bcmltm_node_t *parent_node,
                                bcmltm_lt_op_md_t *op_md_ptr,
                                bcmltm_node_t **node_ptr);

/*!
 * \brief Create EE node data on valid node cookie.
 *
 * Create EE node data only if the given node cookie is not null.
 * The node data must always have a valid node function.
 *
 * This function is used for node functions that expect a node cookie.
 *
 * A null node data is useful to create empty nodes in the tree, where
 * a particular step does not apply, and produce the same tree layout.
 *
 * \param [in] unit Unit number.
 * \param [in] node_function EE function for node.
 * \param [in] node_cookie Context information for given EE function.
 * \param [out] ee_node_ptr Returning pointer to EE node data.
 *                          A null is returned if the node function or
 *                          node cookie is null.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_ee_node_data_create(int unit,
                              bcmltm_ee_node_f node_function,
                              void *node_cookie,
                              bcmltm_ee_node_t **ee_node_ptr);

/*!
 * \brief Create EE node data with optional node cookie.
 *
 * Create EE node data with an optional node cookie.
 * The node data must always have a valid node function.
 *
 * This function is used for node functions that may or may not
 * expect a node cookie.
 *
 * A null node data is useful to create empty nodes in the tree, where
 * a particular step does not apply, and produce the same tree layout.
 *
 * \param [in] unit Unit number.
 * \param [in] node_function EE function for node.
 * \param [in] node_cookie Context information for given EE function.
 * \param [out] ee_node_ptr Returning pointer to EE node data.
 *                          A null is returned if the node function is null.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_ee_node_data_nc_opt_create(int unit,
                                     bcmltm_ee_node_f node_function,
                                     void *node_cookie,
                                     bcmltm_ee_node_t **ee_node_ptr);

/*!
 * \brief Create EE node with given node information.
 *
 * This routine creates an EE node with the given
 * information and attach it to the specified side
 * of the parent node, when applicable (not a ROOT).
 *
 * If given node function or node cookie is NULL, an empty pass-thru
 * node is created (node data is NULL).  This is useful to create
 * a tree layout that applies to various cases where some
 * steps may not apply.
 *
 * This function is used for node functions that expect a node cookie.
 *
 * \param [in] unit Unit number.
 * \param [in] node_func EE function node.
 * \param [in] node_cookie EE node cookie.
 * \param [in] node_side Side of parent node to attach node to or ROOT.
 * \param [in] parent_node Parent node to attach new node to.
 *                         Ignored if specified side is a ROOT.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] node_ptr Returning pointer to created node.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_ee_node_create(int unit,
                         bcmltm_ee_node_f node_func,
                         void *node_cookie,
                         bcmltm_node_side_t node_side,
                         bcmltm_node_t *parent_node,
                         bcmltm_lt_op_md_t *op_md_ptr,
                         bcmltm_node_t **node_ptr);

/*!
 * \brief Create EE node with given node information (optional node cookie).
 *
 * This routine creates an EE node with the given
 * information and attach it to the specified side
 * of the parent node, when applicable (not a ROOT).
 *
 * If given node function is NULL, an empty pass-thru
 * node is created (node data is NULL).  This is useful to create
 * a tree layout that applies to various cases where some
 * steps may not apply.
 *
 * This function is used for node functions that may or may not
 * expect a node cookie.
 *
 * \param [in] unit Unit number.
 * \param [in] node_func EE function node.
 * \param [in] node_cookie EE node cookie.
 * \param [in] node_side Side of parent node to attach node to or ROOT.
 * \param [in] parent_node Parent node to attach new node to.
 *                         Ignored if specified side is a ROOT.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] node_ptr Returning pointer to created node.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_ee_node_nc_opt_create(int unit,
                                bcmltm_ee_node_f node_func,
                                void *node_cookie,
                                bcmltm_node_side_t node_side,
                                bcmltm_node_t *parent_node,
                                bcmltm_lt_op_md_t *op_md_ptr,
                                bcmltm_node_t **node_ptr);

/*!
 * \brief Destroy the list of given tree roots.
 *
 * This routine destroys the given array of tree roots
 * and the node data stored in each of the nodes.
 *
 * This routine also frees the given array list.
 *
 * \param [in] num_roots Number of tree roots.
 * \param [in] ee_node_roots List of tree roots to destroy.
 * \param [in] tree_size Pointer to tree node counting structure.
 * \param [in] free_data TRUE if node_data should be freed also.
 *
 * \retval None.
 */
extern void
bcmltm_md_node_roots_destroy(size_t num_roots,
                             bcmltm_node_t **ee_node_roots,
                             bcmltm_tree_size_t *tree_size,
                             bool free_data);


/*!
 * \brief Destroy given operation metadata object.
 *
 * Destroy given operation metadata.
 *
 * \param [in] op_md Operation metadata to destroy.
 */
extern void
bcmltm_md_op_destroy(bcmltm_lt_op_md_t *op_md);


/*!
 * \brief Get the EE node functions for PTM index read and write access.
 *
 * This routine gets the EE node functions for the corresponding
 * PTM index read and write interfaces based on the table operating mode,
 * Modeled (default) or Interactive.
 *
 * \param [in] bool interactive If true, operating mode is interactive;
 *                              otherwise it is modeled.
 * \param [out] read_func EE node function for PTM read operation.
 * \param [out] write_func EE node function for PTM write operation.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_md_ee_node_ptm_index_func_get(bool interactive,
                                     bcmltm_ee_node_f *read_func,
                                     bcmltm_ee_node_f *write_func);

/*!
 * \brief Transform node functions binary trees to ordered array.
 *
 * This routine examines the binary trees of node functions constructred
 * to implement a logical table opcode.  It traverses the trees in the
 * order of execusion to produce a simple ordered array with the
 * necessary node function pointers and data.  This transformation
 * avoids the overhead of recursive tree traversal during runtime.
 *
 * \param [in] unit Unit number.
 * \param [in,out] op_md Operation metadata to convert.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_md_op_trees_to_array(int unit,
                            bcmltm_lt_op_md_t *op_md);

#endif /* BCMLTM_MD_OP_H */
