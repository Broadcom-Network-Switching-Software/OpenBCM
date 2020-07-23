/*! \file bcmltm_md_lta_validate_fs.h
 *
 * Logical Table Manager - LTA Field Validation with Field Selection.
 *
 * This file contains interfaces to construct the field validations
 * subtrees with field selections.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_MD_LTA_VALIDATE_FS_H
#define BCMLTM_MD_LTA_VALIDATE_FS_H

#include <shr/shr_types.h>

#include <bcmlrd/bcmlrd_types.h>

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_types_internal.h>
#include <bcmltm/bcmltm_md_internal.h>

#include "bcmltm_md_op.h"

/*!
 * \brief Create the field validation cluster of FA subtrees.
 *
 * This routine creates the cluster of FA subtrees for field validations
 * for the specified field selection ID and validation type,
 * for a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] selection_id Field selection ID.
 * \param [in] val_type Field validation type.
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
bcmltm_md_validate_fs_fa_cluster_create(int unit,
                                 bcmlrd_sid_t sid,
                                 bcmltm_field_selection_id_t selection_id,
                                 bcmltm_field_validate_type_t val_type,
                                 bcmlt_opcode_t opcode,
                                 bcmltm_node_side_t node_side,
                                 bcmltm_node_t *parent_chain,
                                 bcmltm_lt_op_md_t *op_md_ptr,
                                 bcmltm_node_t **next_chain);

/*!
 * \brief Create the key field validation cluster.
 *
 * This routine creates the cluster of FA subtrees for
 * key field validations for a given table.
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
bcmltm_md_validate_fs_key_cluster_create(int unit,
                                         bcmlrd_sid_t sid,
                                         bcmlt_opcode_t opcode,
                                         bcmltm_node_side_t node_side,
                                         bcmltm_node_t *parent_node,
                                         bcmltm_lt_op_md_t *op_md_ptr,
                                         bcmltm_node_t **next_chain);

#endif /* BCMLTM_MD_LTA_VALIDATE_FS_H */
