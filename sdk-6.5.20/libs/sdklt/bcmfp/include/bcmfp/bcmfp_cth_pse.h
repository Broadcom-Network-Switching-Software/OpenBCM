/*! \file bcmfp_cth_pse.h
 *
 * Defines, Enums, Structures and APIs corresponding to
 * FP pse LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_CTH_PSE_H
#define BCMFP_CTH_PSE_H

#include <shr/shr_bitop.h>
#include <bcmfp/bcmfp_qual_internal.h>
#include <bcmfp/bcmfp_action_internal.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmfp/bcmfp_types_internal.h>

/*! Configuration provided in FP presel entry LT entry */
typedef struct bcmfp_pse_config_s {

    /*! Key */
    uint32_t pse_id;

    /*! Presel entry priority */
    uint32_t priority;

    /*! Qualifiers data(key, mask) */
    bcmfp_qual_data_t *qual_data;

    /*! Action values */
    bcmfp_action_data_t *action_data;

    /*! Enable */
    bool enable;

} bcmfp_pse_config_t;

/*!
 * \brief Get the information provided in FP presel entry LT entry into
 * bcmfp_pse_config_t strcuture.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] tbl_id             Logical Table ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] key                Key consisting of all key field values.
 * \param [in] data               Data consisting of user specified data
 *                                field values.
 * \param [out] config            FP presel entry LT entry configuration
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_pse_config_get(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       bcmltd_sid_t tbl_id,
                       bcmfp_stage_id_t stage_id,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data,
                       bcmfp_pse_config_t **config);
/*!
 * \brief Get the information provided in FP presel entry LT entry
 * with pse_id as key into bcmfp_pse_config_t
 * strcuture.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] pse_id    BCMFP presel entry ID.
 * \param [out] config            FP presel entry LT entry
 *                                configuration
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_pse_remote_config_get(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_pse_id_t pse_id,
                          bcmfp_pse_config_t **config);
/*!
 * \brief Insert the entry into pse template LT with a given key.
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
bcmfp_pse_config_insert(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 bcmltd_sid_t tbl_id,
                                 bcmfp_stage_id_t stage_id,
                                 const bcmltd_field_t *key,
                                 const bcmltd_field_t *data,
                                 bcmltd_fields_t *output_fields);

/*!
 * \brief  HW lookup the entry from presel entry template LT with a given key.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] tbl_id             Logical Table ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] lkup_type          IMM lookup type.
 * \param [in] key                Key consisting of all key field values.
 * \param [out] data              Data consisting of user specified data
 *                                field values.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_pse_config_lookup(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        bcmimm_lookup_type_t lkup_type,
                        const bcmltd_fields_t *key,
                        bcmltd_fields_t *data);

/*!
 * \brief Update the entry in pse template LT with a given key.
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
bcmfp_pse_config_update(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 bcmltd_sid_t tbl_id,
                                 bcmfp_stage_id_t stage_id,
                                 const bcmltd_field_t *key,
                                 const bcmltd_field_t *data,
                                 bcmltd_fields_t *output_fields);

/*!
 * \brief  Delete the entry from pse template LT with a given key.
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
bcmfp_pse_config_delete(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 bcmltd_sid_t tbl_id,
                                 bcmfp_stage_id_t stage_id,
                                 const bcmltd_field_t *key,
                                 const bcmltd_field_t *data,
                                 bcmltd_fields_t *output_fields);
/*!
 * \brief  Free the memory created for presel entry configuration.
 *
 * \param [in] unit                 Logical device id.
 * \param [in] pse_config  FP presel entry LT entry configuration
 *
 * \retval SHR_E_NONE     Success
 */
extern int
bcmfp_pse_config_free(int unit,
             bcmfp_pse_config_t *pse_config);
#endif /* BCMFP_CTH_PSE_H */
