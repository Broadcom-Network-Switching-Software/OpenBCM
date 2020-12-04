/*! \file bcmltx_field_demux_wide.h
 *
 * Field demux_wide handler definitions
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_FIELD_DEMUX_WIDE_H
#define BCMLTX_FIELD_DEMUX_WIDE_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Field demux/mux transform arguments
 *
 * These LTA Transform functions use a common set of
 * bcmltd_tranform_arg_t values.  The required arguments for the
 * transformations are as follows:
 *
 *
 * bcmltd_transform_arg_t
 *     values = 1;
 *     value = pointer to single entry uint32_t array with
 *             [field index of single combined field];
 *     tables = 0;
 *     table = NULL;
 *     fields = 1;
 *     field = pointer to single entry uint32_t array with
 *             [field ID of single combined field];
 *     rfields = 1;
 *     rfield = pointer to single entry uint32_t array with
 *             [field ID of single combined field];
 *     comp_data = pointer to bcmltd_generic_arg_t described below;
 *
 * Note that the fields/field and rfields/rfield members are identical.
 * This is to preserve the symmetry between the demux and mux
 * operations.  The implementation in each direction only uses one
 * of the two sets.  The single entry uint32_t array may be shared
 * between field and rfield.
 *
 * bcmltd_generic_arg_t
 *     sid = N/A
 *     values = 0;
 *     value = NULL;
 *     user_data = pointer to bcmltd_field_list_t containing the
 *                 specifications of the subfields contained within
 *                 the combined field.
 */

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
bcmltx_field_demux_wide_transform(int unit,
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
bcmltx_field_mux_wide_transform(int unit,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_FIELD_DEMUX_WIDE_H */
