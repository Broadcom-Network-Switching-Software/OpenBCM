/*! \file bcmfp_cth_conflict_qset.h
 *
 * Defines, Enums, Structures and APIs corresponding to
 * FP conflict qset LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_CTH_CONFLICT_QSET_H
#define BCMFP_CTH_CONFLICT_QSET_H

#include <shr/shr_bitop.h>
#include <bcmfp/bcmfp_group_internal.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmfp/bcmfp_types_internal.h>

/*!
 * Max possible qualifiers allowed to enable in
 * conflict QSET LT entry. This is just a soft
 * limit and can be increased on need basis.
 */
#define BCMFP_CONFLICT_QUALIFIER_SETS_MAX 5

/*!
 * Maximum number of qualifiers sharing the same offsets
 * in the final key.
 */
#define BCMFP_CONFLICT_QUALIFIER_MAX 5

/*! Configuration of qualifiers in conflicting qset. */
typedef struct bcmfp_conflict_qset_config_s {
    /*! Conflicting QSET ID. */
    uint16_t conflict_qset_id;

    /*! Array of qualifiers enabled in conflict QSET. */
    uint32_t *qual_id;

    /*! Values assigned to qualifiers enabled in conflict QSET. */
    uint32_t *qual_number;

    /*! Number of valid qualifiers in conflict qset. */
    uint16_t num_quals;
} bcmfp_conflict_qset_config_t;

/*!
 * \brief Get the information provided in FP conflict QSET LT
 * entry into bcmfp_conflict_qset_config_t strcuture.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] tbl_id             Logical Table ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] key                Key consisting of all key field values.
 * \param [in] data               Data consisting of user specified data
 *                                field values.
 * \param [out] config            FP conflict QSET LT entry configuration
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_conflict_qset_config_get(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               bcmltd_sid_t tbl_id,
                               bcmfp_stage_id_t stage_id,
                               const bcmltd_field_t *key,
                               const bcmltd_field_t *data,
                               bcmfp_conflict_qset_config_t **config);

/*!
 * \brief Get the information provided in FP conflict QSET LT
 * entry with cqset_id as key into bcmfp_conflict_qset_config_t
 * strcuture.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] cqset_id           BCMFP conflict qset ID.
 * \param [out] config            FP conflict QSET LT entry configuration
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_conflict_qset_remote_config_get(int unit,
                                  const bcmltd_op_arg_t *op_arg,
                                  bcmfp_stage_id_t stage_id,
                                  bcmfp_conflict_qset_id_t cqset_id,
                                  bcmfp_conflict_qset_config_t **config);

/*!
 * \brief Insert the entry into conflict QSET LT with a given key.
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
bcmfp_conflict_qset_config_insert(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmltd_sid_t tbl_id,
                          bcmfp_stage_id_t stage_id,
                          const bcmltd_field_t *key,
                          const bcmltd_field_t *data,
                          bcmltd_fields_t *output_fields);

/*!
 * \brief Update the entry in conflict QSET LT with a given key.
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
bcmfp_conflict_qset_config_update(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmltd_sid_t tbl_id,
                          bcmfp_stage_id_t stage_id,
                          const bcmltd_field_t *key,
                          const bcmltd_field_t *data,
                          bcmltd_fields_t *output_fields);

/*!
 * \brief Delete the entry from conflict QSET LT with a given key.
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
bcmfp_conflict_qset_config_delete(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmltd_sid_t tbl_id,
                          bcmfp_stage_id_t stage_id,
                          const bcmltd_field_t *key,
                          const bcmltd_field_t *data,
                          bcmltd_fields_t *output_fields);
/*!
 * \brief  Free the memory created for FP group LT entry configuration.
 *
 * \param [in] unit          Logical device id.
 * \param [in] group_config  FP group LT entry configuration
 *
 * \retval SHR_E_NONE     Success
 */
extern int
bcmfp_conflict_qset_config_free(int unit,
                            bcmfp_conflict_qset_config_t *config);


extern int
bcmfp_conflict_qset_ext_codes_get(int unit,
                                  bcmfp_stage_id_t stage_id,
                                  bcmfp_group_id_t group_id,
                                  bcmfp_conflict_qset_id_t cqset_id,
                                  bcmfp_ext_codes_t **ext_codes);
extern int
bcmfp_conflict_qset_oper_info_set(int unit,
                                  bcmfp_stage_id_t stage_id,
                                  bcmfp_group_id_t group_id,
                                  bcmfp_conflict_qset_id_t cqset_id,
                                  bcmfp_ext_codes_t *ext_codes);
extern int
bcmfp_conflict_qset_oper_info_release(int unit,
                                      bcmfp_stage_id_t stage_id,
                                      uint8_t blk_id,
                                      uint32_t blk_offset);
extern int
bcmfp_conflict_qset_oper_info_get(int unit,
                                  uint8_t blk_id,
                                  uint32_t blk_offset,
                                  bcmfp_conflict_qset_oper_info_t **cqset_opinfo);
#endif /* BCMFP_CTH_CONFLICT_QSET_H */
