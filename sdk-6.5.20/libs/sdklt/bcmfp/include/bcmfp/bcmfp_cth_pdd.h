/*! \file bcmfp_cth_pdd.h
 *
 * Defines, Enums, Structures and APIs corresponding to
 * FP pdd LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_CTH_PDD_H
#define BCMFP_CTH_PDD_H

#include <shr/shr_bitop.h>
#include <bcmfp/bcmfp_types_internal.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmfp/bcmfp_action_internal.h>
#include <bcmfp/bcmfp_pdd.h>

/*! Some PDD attributes sued to decide the PDD type. */
typedef struct bcmfp_pdd_attrs_s {
    /*!
     * TRUE -PDD is associated group template LT.
     * FALSE - PDD is associated to entry LT.
     */
    bool pdd_per_group;

    /*!
     * TRUE -SBR is associated group template LT.
     * FALSE - SBR is associated to entry LT.
     */
    bool sbr_per_group;

    /*! PDD is for default policy associated to group. */
    bool default_policy_pdd;

    /*! SBR is not supported for the FP stage. */
    bool sbr_not_supported;
} bcmfp_pdd_attrs_t;

/*! Configuration provided in FP PDD LT entry. */
typedef struct bcmfp_pdd_config_s {
    /*! Key */
    uint32_t pdd_id;

    /*! Action data */
    bcmfp_action_data_t *action_data;
} bcmfp_pdd_config_t;

/*!
 * \brief Get the information provided in FP pdd LT entry into
 * bcmfp_pdd_config_t strcuture.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] tbl_id             Logical Table ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] key                Key consisting of all key field values.
 * \param [in] data               Data consisting of user specified data
 *                                field values.
 * \param [out] config            FP pdd LT entry configuration
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_pdd_config_get(int unit,
                     uint32_t trans_id,
                     bcmltd_sid_t tbl_id,
                     bcmfp_stage_id_t stage_id,
                     const bcmltd_field_t *key,
                     const bcmltd_field_t *data,
                     bcmfp_pdd_config_t **config);
/*!
 * \brief Get the information provided in FP pdd LT entry with
 * pdd_id as key into bcmfp_pdd_config_t strcuture.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] pdd_id             BCMFP pdd ID.
 * \param [out] config            FP pdd LT entry configuration
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_pdd_remote_config_get(int unit,
                            uint32_t trans_id,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_pdd_id_t pdd_id,
                            bcmfp_pdd_config_t **config);
/*!
 * \brief Insert the pdd into entry template LT with a given key.
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
 *                                LT pdd in IMM so that application can
 *                                see those values upon LT pdd LOOKUP.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_pdd_config_insert(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        bcmltd_fields_t *output_fields);

/*!
 * \brief Update the pdd in entry template LT with a given key.
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
 *                                LT pdd in IMM so that application can
 *                                see those values upon LT pdd LOOKUP.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_pdd_config_update(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        bcmltd_fields_t *output_fields);

/*!
 * \brief  Delete the pdd from entry template LT with a given key.
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
 *                                LT pdd in IMM so that application can
 *                                see those values upon LT pdd LOOKUP.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_pdd_config_delete(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        bcmltd_fields_t *output_fields);
/*!
 * \brief  Convert the data in bcmfp_pdd_config_t into bcmfp_pdd_cfg_t.
 *  This API is invoked before PDD library APIs are to be invoked.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] mode               Group mode to which PDD id is
 *                                associated to.
 * \param [in] is_default_policy  Is this conversion for groups
 *                                default PDD?.
 * \param [in] is_pdd_per_group   Group has PDD ID?
 * \param [in] is_sbr_per_group   Group has SBR ID?
 * \param [in] stage              BCMFP stage structure.
 * \param [in] pdd_config         PDD configuration from IMM.
 * \param [out] valid_pdd_data    Actions are present in PDD.
 * \param [out] pdd_cfg           Converted PDD configuration.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_pdd_cfg_get(int unit,
                  uint32_t trans_id,
                  bcmfp_group_mode_t mode,
                  bool is_default_policy,
                  bool is_pdd_per_group,
                  bool is_sbr_per_group,
                  bcmfp_stage_id_t stage_id,
                  bcmfp_pdd_config_t *pdd_config,
                  uint8_t *valid_pdd_data,
                  bcmfp_pdd_cfg_t **pdd_cfg);
/*!
 * \brief Free the memory allocated for bcmfp_pdd_config_t.
 *
 * \param [in] unit        Logical device id.
 * \param [in] pdd_config  PDD configuration from IMM.
 *
 * \retval SHR_E_NONE     Success
 */
extern int
bcmfp_pdd_config_free(int unit,
                      bcmfp_pdd_config_t *pdd_config);

/*!
 * \brief Free the memory allocated for bcmfp_pdd_cfg_t.
 *
 * \param [in] unit        Logical device id.
 * \param [in] pdd_ccfg    PDD configuration.
 *
 * \retval SHR_E_NONE     Success
 */
extern int
bcmfp_pdd_cfg_free(int unit,
                   bcmfp_pdd_cfg_t *pdd_cfg);
/*!
 * \brief Process the PDD configuration in different possible modes
 *  until it succedes.
 *
 * \param [in] unit           Logical device id.
 * \param [in] op_arg         LTM Operational arguments.
 * \param [in] tbl_id         FP logical tabe ID.
 * \param [in] stage_id       FP stage ID.
 * \param [in] pdd_cfg        PDD configuration.
 * \param [in] pdd_id         PDD ID.
 * \param [in] pdd_attrs      PDD attributes.
 * \param [in] min_mode       Minimum group mode to start PDD
 *                            configuration processing.
 *
 * \retval SHR_E_NONE     Success
 */
extern int
bcmfp_pdd_config_process(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         bcmltd_sid_t tbl_id,
                         bcmfp_stage_id_t stage_id,
                         bcmfp_pdd_config_t *pdd_config,
                         uint32_t pdd_id,
                         bcmfp_pdd_attrs_t *pdd_attrs,
                         bcmfp_group_mode_t min_mode);
/*!
 * \brief Get the PDD operational information type based on
 *  association of pDD and SBR IDs to either group or entry.
 *
 * \param [in] unit           Logical device id.
 * \param [in] pdd_attrs      PDD attributes.
 * \param [out] pdd_type      PDD operational type
 *
 * \retval SHR_E_NONE     Success
 */
extern int
bcmfp_pdd_oper_type_get(int unit,
                        bcmfp_pdd_attrs_t *pdd_attrs,
                        bcmfp_pdd_oper_type_t *pdd_type);
#endif /* BCMFP_CTH_PDD_H */
