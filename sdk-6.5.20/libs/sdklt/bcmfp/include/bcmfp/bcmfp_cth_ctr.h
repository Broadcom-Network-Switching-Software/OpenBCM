/*! \file bcmfp_cth_ctr.h
 *
 * Defines, Enums, Structures and APIs corresponding to
 * FP counters.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_CTH_CTR_H
#define BCMFP_CTH_CTR_H

#include <shr/shr_bitop.h>
#include <bcmfp/bcmfp_group_internal.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmfp/bcmfp_types_internal.h>

/* FP counter colors. */
typedef enum bcmfp_ctr_color_e {
    /* None. */
    CTR_COLOR_NONE         = 0,
    /* Green, yellow or red. */
    CTR_COLOR_ALL          = 1,
    /* Green. */
    CTR_COLOR_GREEN        = 2,
    /* Yellow. */
    CTR_COLOR_YELLOW       = 3,
    /* Red. */
    CTR_COLOR_RED          = 4,
    /* Green or yelow. */
    CTR_COLOR_NOT_RED      = 5,
    /* Yellow or red. */
    CTR_COLOR_NOT_GREEN    = 6,
    /* Not a usable value. */
    CTR_COLOR_COUNT        = 7
} bcmfp_ctr_color_t;

/* FP counter entry configuration. */
typedef struct bcmfp_ctr_entry_config_s {
    /* Counter entry ID */
    bcmfp_ctr_entry_id_t ctr_entry_id;

    /* Counter pipe id.  */
    int pipe_id;

    /* Counter pool id. */
    uint8_t pool_id;

    /* Color of the packets counting is on. */
    bcmfp_ctr_color_t color;

} bcmfp_ctr_entry_config_t;

/* FP counter stats configuration. */
typedef struct bcmfp_ctr_stat_config_s {
    /* Counter stat ID */
    bcmfp_ctr_stat_id_t ctr_stat_id;

    /* Packet count */
    uint64_t pkt_count;

    /* Byte Count*/
    uint64_t byte_count;
} bcmfp_ctr_stat_config_t;

/*
 * Different counter parameters needs to be set in
 * main TCAM policy entry buffers(edw).
 */
typedef struct bcmfp_ctr_params_s {
    /* Counter entry ID */
    uint32_t ctr_id;

    /*! Counter pipe ID */
    int ctr_pipe_id;

    /*! Counter pool ID */
    int ctr_pool_id;

    /*! Counter Color */
    int ctr_color;

    /*! Counter HW Idx */
    int ctr_hw_id;

    /*! Counter HW Mode */
    int ctr_hw_mode;

    /*! Flex counter action */
    uint32_t flex_ctr_action;

    /*! Flex counter object */
    uint32_t flex_ctr_object;

    /*! Flex counter container ID */
    uint32_t flex_ctr_container_id;

    /*! Flex counter base index */
    uint32_t flex_ctr_base_idx;

    /*! Flex counter offset mode */
    uint32_t flex_ctr_offset_mode;

    /*! Flex counter pool ID */
    uint32_t flex_ctr_pool_id;

    /*! Flex counter R count */
    uint32_t flex_ctr_r_count;

    /*! Flex counter Y count */
    uint32_t flex_ctr_y_count;

    /*! Flex counter G count */
    uint32_t flex_ctr_g_count;
} bcmfp_ctr_params_t;

/*!
 * \brief Get the information provided in FP ctr entry LT
 * key and data buffers into bcmfp_ctr_entry_config_t
 * strcuture.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] key                Key consisting of all key field values.
 * \param [in] data               Data consisting of user specified data
 *                                field values.
 * \param [out] config            FP ctr entry LT entry configuration
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_ctr_entry_config_get(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        bcmfp_ctr_entry_config_t **config);
/*!
 * \brief Get the information provided in FP ctr entry LT entry
 * with ctr_entryi_id as key into bcmfp_ctr_entry_config_t strcuture.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] stage_id           BCMFP stage ID.
 * \param [in] ctr_entry_id       BCMFP ctr entry ID.
 * \param [out] config            FP ctr entry LT entry configuration
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_ctr_entry_remote_config_get(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       bcmfp_stage_id_t stage_id,
                       bcmfp_ctr_entry_id_t ctr_entry_id,
                       bcmfp_ctr_entry_config_t **config);
/*!
 * \brief Insert the entry in counter entry LT with a given key.
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
 * \retval SHR_E_PARAM    Invalid paractrs.
 */
extern int
bcmfp_ctr_entry_config_insert(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        bcmltd_fields_t *output_fields);
/*!
 * \brief Update the entry in counter entry LT with a given key.
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
 * \retval SHR_E_PARAM    Invalid paractrs.
 */
extern int
bcmfp_ctr_entry_config_update(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        bcmltd_fields_t *output_fields);
/*!
 * \brief Delete the entry from counter entry LT with a given key.
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
 * \retval SHR_E_PARAM    Invalid paractrs.
 */
extern int
bcmfp_ctr_entry_config_delete(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        bcmltd_fields_t *output_fields);
/*!
 * \brief  Free the memory created for FP ctr_entry LT entry configuration.
 *
 * \param [in] unit              Logical device id.
 * \param [in] ctr_entry_config  FP ctr_entry LT entry configuration
 *
 * \retval SHR_E_NONE     Success
 */
extern int
bcmfp_ctr_entry_config_free(int unit,
                bcmfp_ctr_entry_config_t *ctr_entry_config);
extern int
bcmfp_ctr_hw_index_get(int unit,
                       bcmfp_stage_id_t stage_id,
                       uint32_t ctr_id,
                       uint32_t *hw_index);
extern int
bcmfp_ctr_stat_id_set(int unit,
                      bcmfp_stage_id_t stage_id,
                      uint32_t ctr_stat_id,
                      bool set);
extern int
bcmfp_ctr_stat_id_get(int unit,
                      bcmfp_stage_id_t stage_id,
                      uint32_t ctr_stat_id);
extern int
bcmfp_ctr_stat_byte_count_get(int unit,
                              bcmfp_stage_id_t stage_id,
                              uint32_t ctr_id,
                              uint64_t *byte_count);
extern int
bcmfp_ctr_stat_byte_count_set(int unit,
                              bcmfp_stage_id_t stage_id,
                              uint32_t ctr_id,
                              uint64_t byte_count);
extern int
bcmfp_ctr_hw_idx_map_get(int unit,
                         bcmfp_stage_id_t stage_id,
                         uint16_t **ctr_egr_id_to_hw_idx_map);
extern int
bcmfp_ctr_hw_idx_bmp_get(int unit,
                         bcmfp_stage_id_t stage_id,
                         uint32_t **ctr_egr_hw_idx_bmp);
extern int
bcmfp_ctr_stat_pkt_count_set(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t ctr_id,
                             uint64_t pkt_count);
extern int
bcmfp_ctr_stat_hw_get(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      bcmfp_stage_id_t stage_id,
                      uint32_t hw_index,
                      uint8_t pipe,
                      uint64_t *pkt_count,
                      uint64_t *byte_count);

/*!
 * \brief Get first entry in counter entry LT in traverse.
 *
 * \param [in]  unit       Unit number.
 * \param [in]  op_arg     Opration arguments.
 * \param [in]  stage_id   BCMFP_STAGE_ID_XXX.
 * \param [out] out        Output field values.
 * \param [in]  arg        Transform arguments.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid input parameter.
 */
extern int
bcmfp_ctr_stat_first(int unit,
                     const bcmltd_op_arg_t *op_arg,
                     bcmfp_stage_id_t stage_id,
                     bcmltd_fields_t *out,
                     const bcmltd_generic_arg_t *arg);
/*!
 * \brief Get next entry in counter entry LT in trverse.
 *
 * \param [in]  unit      Unit number.
 * \param [in]  op_arg    Operation arguments.
 * \param [in]  stage_id  BCMFP_STAGE_ID_XXX.
 * \param [in]  in        Input field values.
 * \param [out] out       Output field values.
 * \param [in]  arg       Transform arguments.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_NOT_FOUND Next entry not found.
 */
extern int
bcmfp_ctr_stat_next(int unit,
                    const bcmltd_op_arg_t *op_arg,
                    bcmfp_stage_id_t stage_id,
                    const bcmltd_fields_t *in,
                    bcmltd_fields_t *out,
                    const bcmltd_generic_arg_t *arg);
/*!
 * \brief FP egress counters table validation.
 *
 * \param [in]  unit      Unit Number.
 * \param [in]  stage_id  BCMFP_STAGE_ID_XXX.
 * \param [in]  opcode    LT opcode.
 * \param [out] in        Input field values.
 * \param [in]  arg       Custom table handler arguments.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
extern int
bcmfp_ctr_stat_validate(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmlt_opcode_t opcode,
                        const bcmltd_fields_t *in,
                        const bcmltd_generic_arg_t *arg);
/*!
 * \brief FP egress counters table insert.
 *
 * \param [in]  unit      Unit Number.
 * \param [in]  op_arg    Operation arguments.
 * \param [in]  stage_id  BCMFP_STAGE_ID_XXX.
 * \param [in]  in        Input field values.
 * \param [in]  arg       Custom table handler arguments.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
extern int
bcmfp_ctr_stat_insert(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      bcmfp_stage_id_t stage_id,
                      const bcmltd_fields_t *in,
                      const bcmltd_generic_arg_t *arg);
/*!
 * \brief FP egress counters table update.
 *
 * \param [in]  unit      Unit Number.
 * \param [in]  op_arg    Operation arguments.
 * \param [in]  stage_id  BCMFP_STAGE_ID_XXX.
 * \param [in]  in        Input field values.
 * \param [in]  arg       Custom table handler arguments.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
extern int
bcmfp_ctr_stat_update(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      bcmfp_stage_id_t stage_id,
                      const bcmltd_fields_t *in,
                      const bcmltd_generic_arg_t *arg);
/*!
 * \brief FP egress counters table delete.
 *
 * \param [in]  unit      Unit Number.
 * \param [in]  op_arg    Operation arguments.
 * \param [in]  stage_id  BCMFP_STAGE_ID_XXX.
 * \param [in]  in        Input field values.
 * \param [in]  arg       Custom table handler arguments.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
extern int
bcmfp_ctr_stat_delete(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      bcmfp_stage_id_t stage_id,
                      const bcmltd_fields_t *in,
                      const bcmltd_generic_arg_t *arg);
/*!
 * \brief FP egress counters table lookup.
 *
 * \param [in]  unit      Unit Number.
 * \param [in]  op_arg    Operation arguments.
 * \param [in]  stage_id  BCMFP_STAGE_ID_XXX.
 * \param [in]  in        Input field values.
 * \param [out] out       Output field values.
 * \param [in]  arg       Custom table handler arguments.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
extern int
bcmfp_ctr_stat_lookup(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      bcmfp_stage_id_t stage_id,
                      const bcmltd_fields_t *in,
                      bcmltd_fields_t *out,
                      const bcmltd_generic_arg_t *arg);
#endif /* BCMFP_CTH_CTR_H */
