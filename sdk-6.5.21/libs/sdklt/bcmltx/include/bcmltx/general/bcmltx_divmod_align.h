/*! \file bcmltx_divmod_align.h
 *
 * Calculate quotient and remainder from dividend and divisor.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_DIVMOD_ALIGN_H
#define BCMLTX_DIVMOD_ALIGN_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Aligned field quotient transform.
 *
 * Calculate quotient from dividend and divisor.
 * Return SHR_E_PARAM error for non-zero remainder.
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
bcmltx_divmod_align_transform(int unit,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_DIVMOD_ALIGN_H */
