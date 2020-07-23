/*! \file bcmltm_md_lta_validate.h
 *
 * Logical Table Manager - LTA Field Validation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_MD_LTA_VALIDATE_H
#define BCMLTM_MD_LTA_VALIDATE_H


#include <shr/shr_types.h>

#include <bcmlrd/bcmlrd_types.h>
#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_md_internal.h>


/*!
 * \brief Create the Field Validation cluster of FA trees.
 *
 * This function creates the cluster of FA trees for Field Validations
 * for the given LT and field type.
 *
 * This can be used by any operation that requires field validations.
 *
 * The cluster is inserted in the Left node of given parent node.
 * It is composed by a number of Field Validation subtrees.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] field_type Field type.
 * \param [in] opcode LT opcode.
 * \param [in] parent_chain Parent node for this validation cluster.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmltm_md_validate_fa_trees_create(int unit,
                                   bcmlrd_sid_t sid,
                                   bcmltm_field_type_t field_type,
                                   bcmlt_opcode_t opcode,
                                   bcmltm_node_t *parent_chain,
                                   bcmltm_lt_op_md_t *op_md_ptr,
                                   bcmltm_node_t **next_chain);

#endif /* BCMLTM_MD_LTA_VALIDATE_H */
