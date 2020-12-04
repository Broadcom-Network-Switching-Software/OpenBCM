/*! \file bcmfp_cth_src_class.h
 *
 * Defines, Enums, Structures and APIs corresponding to
 * FP src_class LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_CTH_SRC_CLASS_H
#define BCMFP_CTH_SRC_CLASS_H

#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmfp/bcmfp_types_internal.h>

/*! Configuration provided in FP source class mode LT entry. */
typedef struct bcmfp_src_class_config_s {
    /*! Key */
    int pipe_id;

    /*! Source Class Mode */
    uint32_t mode;
} bcmfp_src_class_config_t;

/*!
 * \brief  Set the source class mode.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] tbl_id             Logical Table ID.
 * \param [in] key                Key consisting of all key field values.
 * \param [in] data               Data consisting of user specified data
 *                                field values.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_src_class_insert(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       bcmltd_sid_t tbl_id,
                       bcmfp_stage_id_t stage_id,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data,
                       bcmltd_fields_t *output_fields);
/*!
 * \brief  Unset the source class mode.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] tbl_id             Logical Table ID.
 * \param [in] key                Key consisting of all key field values.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_src_class_delete(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       bcmltd_sid_t tbl_id,
                       bcmfp_stage_id_t stage_id,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data,
                       bcmltd_fields_t *output_fields);
#endif /* BCMFP_CTH_SRC_CLASS_H */
