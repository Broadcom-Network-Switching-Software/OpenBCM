/*! \file bcmfp_cth_sbr.h
 *
 * Defines, Enums, Structures and APIs corresponding to
 * FP sbr LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_CTH_SBR_H
#define BCMFP_CTH_SBR_H


#include <shr/shr_bitop.h>
#include <bcmfp/bcmfp_action_internal.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmfp/bcmfp_types_internal.h>

/* Priority assigned to actions mapped to SBR internally */
#define BCMFP_SBR_INTERNAL_PRI 0x1

/*!
 * Maximum SBR physical tables required for a single SBR profile
 * entry across all devicessupporting SBR.
 */
#define BCMFP_SBR_PROFILE_PARTS_MAX  0x2

/*!
 * Maximum sbr entries that are required for a single entry
 * across all devices supporting SBR.
 */
#define BCMFP_SBR_ENTRY_CNT_MAX 0x2

/*! Configuration provided in FP SBR LT entry. */
typedef struct bcmfp_sbr_config_s {
    /*! Key */
    uint32_t sbr_id;

    /*! Action priorities */
    bcmfp_action_data_t *action_data;
} bcmfp_sbr_config_t;

/*!
 * \brief Get the information provided in FP sbr LT entry into
 * bcmfp_sbr_config_t strcuture.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] tbl_id             Logical Table ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] key                Key consisting of all key field values.
 * \param [in] data               Data consisting of user specified data
 *                                field values.
 * \param [out] config            FP sbr LT entry configuration
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_sbr_config_get(int unit,
                     const bcmltd_op_arg_t *op_arg,
                     bcmltd_sid_t tbl_id,
                     bcmfp_stage_id_t stage_id,
                     const bcmltd_field_t *key,
                     const bcmltd_field_t *data,
                     bcmfp_sbr_config_t **config);

/*!
 * \brief Get the information provided in FP sbr LT entry with
 * sbr_id as key into bcmfp_sbr_config_t strcuture.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] sbr_id             BCMFP sbr ID.
 * \param [out] config            FP sbr LT entry configuration
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_sbr_remote_config_get(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_sbr_id_t sbr_id,
                            bcmfp_sbr_config_t **config);

/*!
 * \brief Insert the sbr into entry template LT with a given key.
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
 *                                LT sbr in IMM so that application can
 *                                see those values upon LT sbr LOOKUP.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_sbr_config_insert(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        bcmltd_fields_t *output_fields);

/*!
 * \brief Update the sbr in entry template LT with a given key.
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
 *                                LT sbr in IMM so that application can
 *                                see those values upon LT sbr LOOKUP.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_sbr_config_update(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        bcmltd_fields_t *output_fields);

/*!
 * \brief  Delete the sbr from entry template LT with a given key.
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
 *                                LT sbr in IMM so that application can
 *                                see those values upon LT sbr LOOKUP.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_sbr_config_delete(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        bcmltd_fields_t *output_fields);
/*!
 * \brief  Free the memory created for FP sbr LT entry configuration.
 *
 * \param [in] unit        Logical device id.
 * \param [in] sbr_config  FP sbr LT entry configuration
 *
 * \retval SHR_E_NONE     Success
 */
extern int
bcmfp_sbr_config_free(int unit,
                       bcmfp_sbr_config_t *sbr_config);
/*!
 * \brief  Clear the entry at given physical index in SBR physical table.
 *
 * \param [in] unit       Logical device id.
 * \param [in] trans_id   Transactio ID.
 * \param [in] pipe_id    Pipeline number.
 * \param [in] req_ltid   Logical Table ID.
 * \param [in] stage_id   BCMFP stage ID.
 * \param [in] hw_index   SBR physical table index
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_sbr_profile_uninstall(int unit,
                            uint32_t trans_id,
                            int pipe_id,
                            bcmltd_sid_t req_ltid,
                            bcmfp_stage_id_t stage_id,
                            uint32_t hw_index);
/*!
 * \brief  Write the entry at free index available in SBR phyiscal table.
 *
 * \param [in] unit       Logical device id.
 * \param [in] op_arg     Operational args.
 * \param [in] pipe_id    Pipeline number.
 * \param [in] req_ltid   Logical Table ID.
 * \param [in] stage_id   BCMFP stage ID.
 * \param [in] group_id   Group ID in which the SBR profiles are installed.
 * \param [in] entry_id   Entry ID in which the SBR profiles are installed.
 * \param [in] sbr        SBR profile data to be written.
 * \param [in] num_parts  Number of parts SBR spreads.
 * \param [in] hw_index   Free indexes allocated
 * \param [in] sbr_id     sbr profile id
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_sbr_profile_install(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          int pipe_id,
                          bcmltd_sid_t req_ltid,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_group_id_t group_id,
                          bcmfp_entry_id_t entry_id,
                          uint32_t **sbr,
                          uint8_t num_parts,
                          uint32_t *hw_index,
                          bcmfp_sbr_id_t sbr_id);

/*!
 * \brief  Obtain the free sbr index from UFT manager and program the sbr entry.
 *
 * \param [in] unit       Logical device id.
 * \param [in] op_arg     operational args.
 * \param [in] pipe_id    Pipeline number.
 * \param [in] req_ltid   Logical Table ID.
 * \param [in] stage_id   BCMFP stage ID.
 * \param [in] group_id   Group ID for which the new SBR hw index is required.
 * \param [in] entry_id   Entry ID for which the new SBR hw index is required.
 * \param [in] sbr        SBR profile data to be written.
 * \param [in] num_parts  Number of parts SBR spreads.
 * \param [in] hw_index   Free indexes allocated
 *
 * \retval SHR_E_NONE      Success
 * \retval SHR_E_UNAVAIL  UFT manager doesnt support the sbr sid.
 */
extern int
bcmfp_uft_sbr_entry_alloc(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          int pipe_id,
                          bcmltd_sid_t req_ltid,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_group_id_t group_id,
                          bcmfp_entry_id_t entry_id,
                          uint32_t **sbr,
                          uint8_t num_parts,
                          uint32_t *hw_index,
                          bcmfp_sbr_id_t sbr_id);
/*!
 * \brief  Free the sbr index obatined from the UFT manager.
 *
 * \param [in] unit       Logical device id.
 * \param [in] trans_id   Transactio ID.
 * \param [in] pipe_id    Pipeline number.
 * \param [in] req_ltid   Logical Table ID.
 * \param [in] stage_id   BCMFP stage ID.
 * \param [in] hw_index   Free indexes allocated
 *
 * \retval SHR_E_NONE      Success
 * \retval SHR_E_UNAVAIL  UFT manager doesnt support the sbr sid.
 */

extern int
bcmfp_uft_sbr_entry_free(int unit,
                         uint32_t trans_id,
                         int pipe_id,
                         bcmltd_sid_t req_ltid,
                         bcmfp_stage_id_t stage_id,
                         uint32_t hw_index);
/*!
 * \brief  Rewrite the entry at given physical index in SBR phyiscal table.
 *
 * \param [in] unit       Logical device id.
 * \param [in] trans_id   Transactio ID.
 * \param [in] pipe_id    Pipeline number.
 * \param [in] req_ltid   Logical Table ID.
 * \param [in] stage_id   BCMFP stage ID.
 * \param [in] sbr        SBR profile data to be written.
 * \param [in] num_parts  Number of parts SBR spreads.
 * \param [in] hw_index   SBR physical table index
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_sbr_profile_update(int unit,
                         uint32_t trans_id,
                         int pipe_id,
                         bcmltd_sid_t req_ltid,
                         bcmfp_stage_id_t stage_id,
                         uint32_t **sbr,
                         uint8_t num_parts,
                         uint32_t hw_index);
#endif /* BCMFP_CTH_SBR_H */
