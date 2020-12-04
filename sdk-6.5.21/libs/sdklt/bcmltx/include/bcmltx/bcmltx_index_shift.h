/*! \file bcmltx_index_shift.h
 *
 * Index shift handler definitions,
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_INDEX_SHIFT_H
#define BCMLTX_INDEX_SHIFT_H

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
 *     value = index shift offset;
 *     tables = 0;
 *     table = NULL;
 *     fields = 1;
 *     field = pointer to single entry uint32_t array with
 *             [field ID of single combined field];
 *     rfields = 1;
 *     rfield = pointer to single entry uint32_t array with
 *             [field ID of single combined field];
 *     comp_data = NULL;
 *
 * For forward transform, the input field with id field[0] is changed
 * to (value + index shift) and placed into the output field list
 * with id rfield[0].  The field index is 0 for both of these cases.
 *
 * The reverse transform subtracts the index shift instead.
 */

/*!
 * \brief Index shift transform
 *
 * A LTA Transform function to adjust the zero-based index range of a
 * LT to the physical index range when that value is different.
 * Intended for use in the case of multiple LTs mapped to the same
 * PT allocated with separate index range subsets.
 *
 * Logical Table  LT index     Index xform     PT index
 *  LT_A           [0:M-1]      N/A             [0:M-1]
 *  LT_B           [0:K-1]      +M              [M:M+K-1]
 *  LT_C           [0:J-1]      +M+K            [M+K:M+K+J-1]
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
bcmltx_index_shift_transform(int unit,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_transform_arg_t *arg);

/*!
 * \brief Index shift reverse transform
 *
 * A LTA Transform function to determine the zero-based index range of a
 * LT from the physical index range when that value is different.
 * Intended for use in the case of multiple LTs mapped to the same
 * PT allocated with separate index range subsets.
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
bcmltx_index_shift_rev_transform(int unit,
                                 const bcmltd_fields_t *in,
                                 bcmltd_fields_t *out,
                                 const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_INDEX_SHIFT_H */
