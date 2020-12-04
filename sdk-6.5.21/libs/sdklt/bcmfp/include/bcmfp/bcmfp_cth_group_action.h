/*! \file bcmfp_cth_group_action.h
 *
 * Defines, Enums, Structures and APIs corresponding to
 * FP Group Action LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_CTH_GROUP_ACTION_H
#define BCMFP_CTH_GROUP_ACTION_H

#include <shr/shr_bitop.h>
#include <bcmfp/bcmfp_group_internal.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmfp/bcmfp_types_internal.h>

/*!
 * \brief Insert the entry into group template action LT with a given key.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] tbl_id             Logical Table ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] key                Key consisting of all key field values.
 * \param [in] data               Data consisting of user specified data
 *                                field values.
 * \param [out] output_fields     If LT has any read only fields, BCMFP
 *                                BCMFP driver can add those fields to
 *                                LT entry in IMM so that application can
 *                                see those values upon LT entry LOOKUP.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_group_template_action_insert(int unit,
                                   const bcmltd_op_arg_t *op_arg,
                                   bcmltd_sid_t tbl_id,
                                   bcmfp_stage_id_t stage_id,
                                   const bcmltd_field_t *key,
                                   const bcmltd_field_t *data,
                                   bcmltd_fields_t *output_fields);

/*!
 * \brief Update the entry into group template action LT with a given key.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] tbl_id             Logical Table ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] key                Key consisting of all key field values.
 * \param [in] data               Data consisting of user specified data
 *                                field values.
 * \param [out] output_fields     If LT has any read only fields, BCMFP
 *                                BCMFP driver can add those fields to
 *                                LT entry in IMM so that application can
 *                                see those values upon LT entry LOOKUP.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_group_template_action_update(int unit,
                                   const bcmltd_op_arg_t *op_arg,
                                   bcmltd_sid_t tbl_id,
                                   bcmfp_stage_id_t stage_id,
                                   const bcmltd_field_t *key,
                                   const bcmltd_field_t *data,
                                   bcmltd_fields_t *output_fields);

/*!
 * \brief Delete the entry into group template action LT with a given key.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] tbl_id             Logical Table ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] key                Key consisting of all key field values.
 * \param [in] data               Data consisting of user specified data
 *                                field values.
 * \param [out] output_fields     If LT has any read only fields, BCMFP
 *                                BCMFP driver can add those fields to
 *                                LT entry in IMM so that application can
 *                                see those values upon LT entry LOOKUP.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_group_template_action_delete(int unit,
                                   const bcmltd_op_arg_t *op_arg,
                                   bcmltd_sid_t tbl_id,
                                   bcmfp_stage_id_t stage_id,
                                   const bcmltd_field_t *key,
                                   const bcmltd_field_t *data,
                                   bcmltd_fields_t *output_fields);

#endif /* BCMFP_CTH_GROUP_ACTION_H */
