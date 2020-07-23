/*! \file bcmcth_lb_hash_info.h
 *
 * This file contains load balance hash handler
 * function declarations and macro definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_LB_HASH_INFO_H
#define BCMCTH_LB_HASH_INFO_H

#include <bcmdrd_config.h>
#include <bcmltd/bcmltd_handler.h>

/*!
 * BCMCTH load balance hashing information.
 */
typedef struct bcmcth_lb_hash_info {

    /*! Number of elements in the array nonconcat_sub_field_width. */
    uint8_t num_nonconcat_sub_field;

    /*! Array of sub-field widths (bits) for non-concatenate mode. */
    uint8_t nonconcat_sub_field_width[8];

    /*! Number of elements in the array concat_sub_field_width. */
    uint8_t num_concat_sub_field;

    /*! Array of sub-field widths (bits) for concatenate mode. */
    uint8_t concat_sub_field_width[8];

} bcmcth_lb_hash_info_t;

/*!
 * \brief Initialize device-specific data.
 *
 * \param [in] unit Unit nunmber.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_lb_hash_info_init(int unit);

/*!
 * \brief LB hash info validation
 *
 * Validate LB hash info parameters.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  opcode        LT opcode.
 * \param [in]  in            Input field values.
 * \param [in]  arg           Custom table handler arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_lb_hash_info_validate(int unit,
                             bcmlt_opcode_t opcode,
                             const bcmltd_fields_t *in,
                             const bcmltd_generic_arg_t *arg);
/*!
 * \brief LB hash info insert.
 *
 * Insert into LB hash info LT. Invalid operation on read-only LT.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values (not applicable).
 * \param [in]  arg           Custom table handler arguments.
 *
 * \retval SHR_E_NONE         No errors.
 */
int
bcmcth_lb_hash_info_insert(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg);

/*!
 * \brief LB hash info lookup.
 *
 * Lookup LB hash info LT.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Custom table handler arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_lb_hash_info_lookup(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg);

/*!
 * \brief LB hash info delete.
 *
 * Delete LB hash info. Invalid operation on read-only LT.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values (not applicable).
 * \param [in]  arg           Custom table handler arguments.
 *
 * \retval SHR_E_NONE         No errors.
 */
int
bcmcth_lb_hash_info_delete(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg);

/*!
 * \brief LB hash info update.
 *
 * Update LB hash info. Invalid operaton on read-only LT.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field value (not applicable).
 * \param [in]  arg           Custom table handler arguments.
 *
 * \retval SHR_E_NONE         No errors.
 */
int
bcmcth_lb_hash_info_update(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg);

/*!
 * \brief LB hash information table first.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values (not applicable).
 * \param [out] out           Output field values.
 * \param [in]  arg           Custom table handler arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_lb_hash_info_first(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out,
                          const bcmltd_generic_arg_t *arg);

/*!
 * \brief LB hash information table next.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Custom table handler arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_lb_hash_info_next(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg);

#endif /* BCMCTH_LB_HASH_INFO_H */
