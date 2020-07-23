/*! \file bcmltm_md_lta_xfrm.h
 *
 * Logical Table Manager - LTA Field Transform.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_MD_LTA_XFRM_H
#define BCMLTM_MD_LTA_XFRM_H


#include <shr/shr_types.h>

#include <bcmlrd/bcmlrd_types.h>
#include <bcmltm/bcmltm_md_internal.h>

#include "bcmltm_md_lta.h"


/*!
 * \brief Create the Forward Field Transform cluster of FA trees.
 *
 * This function creates the cluster of FA trees for Forward Field Transforms
 * for the given LT and transform type.
 *
 * This can be used by any operation that requires a forward transform
 * from LT source to PT destination.
 *
 * The cluster is inserted in the Left node of given parent node.
 * It is composed by a number of Field Transform subtrees.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] xfrm_type Transform Type.
 * \param [in] lta_fopt LTA field options.
 * \param [in] opcode LT opcode.
 * \param [in] parent_chain Parent node for this transform cluster.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_xfrm_fa_trees_fwd_create(int unit,
                                   bcmlrd_sid_t sid,
                                   int xfrm_type,
                                   bcmltm_md_lta_fopt_t lta_fopt,
                                   bcmlt_opcode_t opcode,
                                   bcmltm_node_t *parent_chain,
                                   bcmltm_lt_op_md_t *op_md_ptr,
                                   bcmltm_node_t **next_chain);


/*!
 * \brief Create the Reverse Field Transform cluster of FA trees.
 *
 * This function creates the cluster of FA trees for Reverse Field Transforms
 * for the given LT and transform type.
 *
 * This can be used by any operation that requires a reverse transform
 * from LT source to PT destination, such as LOOKUP.
 *
 * The cluster is inserted in the Left node of given parent node.
 * It is composed by a number of Field Transform subtrees.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] xfrm_type Transform Type.
 * \param [in] opcode LT opcode.
 * \param [in] parent_chain Parent node for this transform cluster.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_xfrm_fa_trees_rev_create(int unit,
                                   bcmlrd_sid_t sid,
                                   int xfrm_type,
                                   bcmlt_opcode_t opcode,
                                   bcmltm_node_t *parent_chain,
                                   bcmltm_lt_op_md_t *op_md_ptr,
                                   bcmltm_node_t **next_chain);

/*!
 * \brief Create the Key Allocation Forward Field Transform cluster of FA trees.
 *
 * This function creates the cluster of FA trees for Key Forward
 * Field Transforms to be used for INSERT on IwA LTs.
 * It installs both, transforms with required keys as well as
 * transforms with optional keys.
 *
 * The cluster is inserted in the Left node of given parent node.
 * It is composed by a number of Field Transform subtrees.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] opcode LT opcode.
 * \param [in] parent_chain Parent node for this transform cluster.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_xfrm_fa_trees_fwd_alloc_create(int unit,
                                         bcmlrd_sid_t sid,
                                         bcmlt_opcode_t opcode,
                                         bcmltm_node_t *parent_chain,
                                         bcmltm_lt_op_md_t *op_md_ptr,
                                         bcmltm_node_t **next_chain);

/*!
 * \brief Create the Key Allocation Reverse Field Transform cluster of FA trees.
 *
 * This function creates the cluster of FA trees for Key Reverse
 * Field Transforms to be used for INSERT on IwA LTs.
 * It installs transforms that contain optional keys.
 *
 * If optional keys are absent during an INSERT, they are allocated
 * and returned in the API field list.
 *
 * The cluster is inserted in the Left node of given parent node.
 * It is composed by a number of Field Transform subtrees.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] opcode LT opcode.
 * \param [in] parent_chain Parent node for this transform cluster.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_xfrm_fa_trees_rev_alloc_create(int unit,
                                         bcmlrd_sid_t sid,
                                         bcmlt_opcode_t opcode,
                                         bcmltm_node_t *parent_chain,
                                         bcmltm_lt_op_md_t *op_md_ptr,
                                         bcmltm_node_t **next_chain);

#endif /* BCMLTM_MD_LTA_XFRM_H */
