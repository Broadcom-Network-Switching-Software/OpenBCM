/*! \file bcmltx_divmod_multi_view.h
 *
 * Calculate quotient and remainder from dividend and divisor.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_DIVMOD_MULTI_VIEW_H
#define BCMLTX_DIVMOD_MULTI_VIEW_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Field quotient and remainder transform.
 *
 * Calculate quotient and remainder from dividend and divisor.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in_key        Input key values.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */

extern int
bcmltx_divmod_multi_view_transform(int unit,
                                   const bcmltd_fields_t *in_key,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_transform_arg_t *arg);

/*!
 * \brief Field quotient and remainder transform.
 *
 * Calculate quotient and remainder from dividend and divisor.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in_key        Input key values.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */

extern int
bcmltx_divmod_multi_view_rev_transform(int unit,
                                       const bcmltd_fields_t *in_key,
                                       const bcmltd_fields_t *in,
                                       bcmltd_fields_t *out,
                                       const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_DIVMOD_MULTI_VIEW_H */
