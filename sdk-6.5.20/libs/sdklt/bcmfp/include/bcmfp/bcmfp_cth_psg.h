/*! \file bcmfp_cth_psg.h
 *
 * Defines, Enums, Structures and APIs corresponding to
 * FP psg LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_CTH_PSG_H
#define BCMFP_CTH_PSG_H

#include <shr/shr_bitop.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmfp/bcmfp_types_internal.h>
#include <bcmfp/bcmfp_qual_internal.h>

/*! Configuration provided in FP presel group LT entry. */
typedef struct bcmfp_psg_config_s {
    /*! Key */
    uint32_t psg_id;

    /*! Qualifiers bitmap */
    bcmfp_qual_bitmap_t *qual_bitmap;

} bcmfp_psg_config_t;

/*!
 * \brief Get the information provided in FP presel group LT entry into
 * bcmfp_psg_config_t strcuture.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] tbl_id             Logical Table ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] key                Key consisting of all key field values.
 * \param [in] data               Data consisting of user specified data
 *                                field values.
 * \param [out] config            FP presel group LT entry configuration
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_psg_config_get(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       bcmltd_sid_t tbl_id,
                       bcmfp_stage_id_t stage_id,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data,
                       bcmfp_psg_config_t **config);
/*!
 * \brief Get the information provided in FP presel group LT entry
 *  with psg_id as key into bcmfp_psg_config_t
 *  strcuture.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] psg_id    BCMFP presel group ID.
 * \param [out] config            FP presel group LT entry
 *                                configuration
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_psg_remote_config_get(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_psg_id_t psg_id,
                          bcmfp_psg_config_t **config);
/*!
 * \brief Insert the entry into psg template LT with a given key.
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
bcmfp_psg_config_insert(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 bcmltd_sid_t tbl_id,
                                 bcmfp_stage_id_t stage_id,
                                 const bcmltd_field_t *key,
                                 const bcmltd_field_t *data,
                                 bcmltd_fields_t *output_fields);

/*!
 * \brief Update the entry in psg template LT with a given key.
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
bcmfp_psg_config_update(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 bcmltd_sid_t tbl_id,
                                 bcmfp_stage_id_t stage_id,
                                 const bcmltd_field_t *key,
                                 const bcmltd_field_t *data,
                                 bcmltd_fields_t *output_fields);

/*!
 * \brief  Delete the entry from psg template LT with a given key.
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
bcmfp_psg_config_delete(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 bcmltd_sid_t tbl_id,
                                 bcmfp_stage_id_t stage_id,
                                 const bcmltd_field_t *key,
                                 const bcmltd_field_t *data,
                                 bcmltd_fields_t *output_fields);
/*!
 * \brief  Free the memory created for presel group configuration.
 *
 * \param [in] unit                 Logical device id.
 * \param [in] psg_config  FP presel group LT entry configuration
 *
 * \retval SHR_E_NONE     Success
 */
extern int
bcmfp_psg_config_free(int unit,
             bcmfp_psg_config_t *psg_config);
#endif /* BCMFP_CTH_PSG_H */
