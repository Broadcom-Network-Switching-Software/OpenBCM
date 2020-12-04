/*! \file bcmfp_cth_policy.h
 *
 * Defines, Enums, Structures and APIs corresponding to
 * FP policy LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_CTH_POLICY_H
#define BCMFP_CTH_POLICY_H

#include <shr/shr_bitop.h>
#include <bcmfp/bcmfp_action_internal.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmfp/bcmfp_types_internal.h>

/*! Configuration provided in FP policy LT entry. */
typedef struct bcmfp_policy_config_s {
    /*! Key */
    uint32_t policy_id;

    /*!
     * Flag to indicate whether to counter red
     * packets with flex counter. Also indicates
     * what is the offset for red packet counter
     * from the base index.
     */
    uint8_t flex_ctr_r_count;

    /*!
     * Flag to indicate whether to counter yellow
     * packets with flex counter. Also indicates
     * what is the offset for yellow packet counter
     * from the base index.
     */
    uint8_t flex_ctr_y_count;

    /*!
     * Flag to indicate whether to counter green
     * packets with flex counter. Also indicates
     * what is the offset for green packet counter
     * from the base index.
     */
    uint8_t flex_ctr_g_count;

    /*! Flex counter object */
    uint32_t flex_ctr_object;

    /*! Flex counter container ID */
    uint16_t flex_ctr_container_id;

    /*! Actions data to be configured in part 0 */
    bcmfp_action_data_t *action_data;

    /*! Actions data to be configured in part 1 */
    bcmfp_action_data_t *action_data1;

    /*! Actions data to be configured in part 2 */
    bcmfp_action_data_t *action_data2;

    /*! viewtype of the action data */
    bcmfp_action_viewtype_t viewtype;

} bcmfp_policy_config_t;

/*!
 * \brief Get the information provided in FP policy LT entry into
 * bcmfp_policy_config_t strcuture.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] tbl_id             Logical Table ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] key                Key consisting of all key field values.
 * \param [in] data               Data consisting of user specified data
 *                                field values.
 * \param [out] config            FP policy LT entry configuration
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_policy_config_get(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       bcmltd_sid_t tbl_id,
                       bcmfp_stage_id_t stage_id,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data,
                       bcmfp_policy_config_t **config);

/*!
 * \brief Get the information provided in FP policy LT entry with
 * policy_id as key into bcmfp_policy_config_t strcuture.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] policy_id          BCMFP policy ID.
 * \param [out] config            FP entry LT entry configuration
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_policy_remote_config_get(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmfp_stage_id_t stage_id,
                              bcmfp_policy_id_t policy_id,
                              bcmfp_policy_config_t **config);
/*!
 * \brief Insert the entry into policy template LT with a given key.
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
bcmfp_policy_config_insert(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           bcmltd_sid_t tbl_id,
                           bcmfp_stage_id_t stage_id,
                           const bcmltd_field_t *key,
                           const bcmltd_field_t *data,
                           bcmltd_fields_t *output_fields);

/*!
 * \brief Update the entry in policy template LT with a given key.
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
bcmfp_policy_config_update(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           bcmltd_sid_t tbl_id,
                           bcmfp_stage_id_t stage_id,
                           const bcmltd_field_t *key,
                           const bcmltd_field_t *data,
                           bcmltd_fields_t *output_fields);

/*!
 * \brief  Delete the entry from policy template LT with a given key.
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
bcmfp_policy_config_delete(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           bcmltd_sid_t tbl_id,
                           bcmfp_stage_id_t stage_id,
                           const bcmltd_field_t *key,
                           const bcmltd_field_t *data,
                           bcmltd_fields_t *output_fields);
/*!
 * \brief  Free the memory created for FP policy LT entry configuration.
 *
 * \param [in] unit           Logical device id.
 * \param [in] policy_config  FP policy LT entry configuration
 *
 * \retval SHR_E_NONE     Success
 */
extern int
bcmfp_policy_config_free(int unit,
                       bcmfp_policy_config_t *policy_config);
/*!
 * \brief  Compare two policy configurations and check if they are
 *  same w.r.t actions and actions data.
 *
 * \param [in] unit       Logical device id.
 * \param [in] p1         First policy configuration.
 * \param [in] p2         Second policy configuration.
 * \param [out] matched   TRUE = if p1 and p2 are same
 *                        FALSE = if p1 and p2 are not same.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_policy_config_compare(int unit,
                            bcmfp_policy_config_t *p1,
                            bcmfp_policy_config_t *p2,
                            bool *matched);
#endif /* BCMFP_CTH_POLICY_H */
