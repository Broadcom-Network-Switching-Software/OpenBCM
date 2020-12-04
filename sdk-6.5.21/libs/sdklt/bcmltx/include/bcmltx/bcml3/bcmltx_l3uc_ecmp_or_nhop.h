/*! \file bcmltx_l3uc_ecmp_or_nhop.h
 *
 * L3 ECMP_NHOP Transform Handler
 *
 * This file contains field transform information for L3 ECMP_NHOP field.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_L3UC_ECMP_OR_NHOP_H
#define BCMLTX_L3UC_ECMP_OR_NHOP_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief ECMP_NHOP transform
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
bcmltx_l3uc_ecmp_or_nhop_transform(int unit,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg);


/*!
 * \brief ECMP_NHOP reverse transform
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
bcmltx_l3uc_ecmp_or_nhop_rev_transform(int unit,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg);


#endif /* BCMLTX_L3UC_ECMP_OR_NHOP_H */
