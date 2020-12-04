/*! \file bcmltx_field_demux_acc.h
 *
 * Accumulated subfield transform handler definitions
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_FIELD_DEMUX_ACC_H
#define BCMLTX_FIELD_DEMUX_ACC_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Field demux transform
 *
 * A LTA Transform function to extract multiple fields values from a
 * single field value.
 *
 * The reverse tranform is provided by the field mux function.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmltx_field_demux_acc_transform(int unit,
                                 const bcmltd_fields_t *in,
                                 bcmltd_fields_t *out,
                                 const bcmltd_transform_arg_t *arg);

/*!
 * \brief Field mux transform
 *
 * A LTA Transform function to combine multiple fields values into a
 * single field value.
 *
 * The reverse tranform is provided by the field demux function.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmltx_field_mux_acc_transform(int unit,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out,
                               const bcmltd_transform_arg_t *arg);

/*!
 * \brief Field mux state management
 *
 * Clears the field mux transform state.
 *
 * The field mux transform state is used by the subfield reverse transform.
 *
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmltx_field_mux_acc_transform_clear_state(const bcmltd_generic_arg_t *arg);


/*!
 * \brief Update accumulated subfield transform state counter and value.
 *
 * Update accumulated subfield transform state counter and value.
 *
 * \param [in]  arg           LTD generic arg.
 * \param [in]  index         Field index.
 * \param [in]  value         Field value.
 *
 * \retval SHR_E_NONE         No error.
 */
extern int
bcmltx_field_mux_acc_transform_update_value(const bcmltd_generic_arg_t *arg,
                                            const uint32_t index,
                                            const uint64_t value);

/*!
 * \brief Get subfield transform state counter and value.
 *
 * Get subfield transform state counter and value.
 *
 * \param [in]  arg           LTD generic arg.
 * \param [in]  index         Field index.
 * \param [in]  value         Pointer to the field value.
 *
 * \retval SHR_E_NONE         No error.
 */
extern int
bcmltx_field_mux_acc_transform_get_value(const bcmltd_generic_arg_t *arg,
                                         const uint32_t index,
                                         uint64_t *value);

/*!
 * \brief Check if value needs to be copied or not.
 *
 * Check if value needs to be copied or not.
 *
 * \param [in]  arg           LTD generic arg.
 * \param [out] need_copy     Flag to tell if copy operation is needed or not.
 *
 * \retval SHR_E_NONE         No error.
 * \retval !SHR_E_NONE        Error.
 */
extern int
bcmltx_field_mux_acc_transform_is_copy_needed(const bcmltd_generic_arg_t *arg,
                                              int *need_copy);

#endif /* BCMLTX_FIELD_DEMUX_ACC_H */
