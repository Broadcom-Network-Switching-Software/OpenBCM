/*! \file bcmltm_md_lta_xfrm_fs.h
 *
 * Logical Table Manager - LTA Field Transform with Field Selection.
 *
 * This file contains interfaces to construct the field transform
 * subtrees with field selections.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_MD_LTA_XFRM_FS_H
#define BCMLTM_MD_LTA_XFRM_FS_H

#include <shr/shr_types.h>

#include <bcmlrd/bcmlrd_types.h>

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_types_internal.h>
#include <bcmltm/bcmltm_md_internal.h>

#include "bcmltm_md_op.h"
#include "bcmltm_md_lta.h"

/*!
 * \brief Create the field transform cluster of FA subtrees.
 *
 * This routine creates the cluster of FA subtrees for field transforms
 * for the specified field selection ID and transform type,
 * for a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] selection_id Field selection ID.
 * \param [in] xfrm_type Transform type.
 * \param [in] lta_flags LTA flags.
 * \param [in] opcode LT opcode.
 * \param [in] node_side Side of parent node to attach cluster to.
 * \param [in] parent_chain Parent node to attach cluster to.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_xfrm_fs_fa_cluster_create(int unit,
                                    bcmlrd_sid_t sid,
                                    bcmltm_field_selection_id_t selection_id,
                                    bcmltm_field_xfrm_type_t xfrm_type,
                                    bcmltm_md_lta_flags_t lta_flags,
                                    bcmlt_opcode_t opcode,
                                    bcmltm_node_side_t node_side,
                                    bcmltm_node_t *parent_chain,
                                    bcmltm_lt_op_md_t *op_md_ptr,
                                    bcmltm_node_t **next_chain);

/*!
 * \brief Create the revere track field transform cluster of FA subtrees.
 *
 * This routine creates the cluster of FA subtrees for
 * reverse track field transforms to obtain the source API fields.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] node_side Side of parent node to attach cluster to.
 * \param [in] parent_chain Parent node to attach cluster to.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_xfrm_track_rev_cluster_create(int unit,
                                        bcmlrd_sid_t sid,
                                        bcmltm_node_side_t node_side,
                                        bcmltm_node_t *parent_chain,
                                        bcmltm_lt_op_md_t *op_md_ptr,
                                        bcmltm_node_t **next_chain);

/*!
 * \brief Create the key forward transform cluster.
 *
 * This routine creates the cluster of FA subtrees for
 * key forward field transforms for a given table.
 *
 * This interface is a helper routine that sets the right arguments
 * for the core cluster create function.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] opcode LT opcode.
 * \param [in] traverse_start Indicates whether to check on TRAVERSE start.
 * \param [in] node_side Side of parent node to attach cluster to.
 * \param [in] parent_node Parent node to attach cluster to.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_xfrm_fs_fwd_key_cluster_create(int unit,
                                         bcmlrd_sid_t sid,
                                         bcmlt_opcode_t opcode,
                                         bool traverse_start,
                                         bcmltm_node_side_t node_side,
                                         bcmltm_node_t *parent_node,
                                         bcmltm_lt_op_md_t *op_md_ptr,
                                         bcmltm_node_t **next_chain);

/*!
 * \brief Create the key reverse transform cluster.
 *
 * This routine creates the cluster of FA subtrees for
 * key reverse field transforms for a given table.
 *
 * This interface is a helper routine that sets the right arguments
 * for the core cluster create function.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] opcode LT opcode.
 * \param [in] node_side Side of parent node to attach cluster to.
 * \param [in] parent_node Parent node to attach cluster to.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_xfrm_fs_rev_key_cluster_create(int unit,
                                         bcmlrd_sid_t sid,
                                         bcmlt_opcode_t opcode,
                                         bcmltm_node_side_t node_side,
                                         bcmltm_node_t *parent_node,
                                         bcmltm_lt_op_md_t *op_md_ptr,
                                         bcmltm_node_t **next_chain);

#endif /* BCMLTM_MD_LTA_XFRM_FS_H */
