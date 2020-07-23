/*! \file bcmltx_port_policy_egr_cfi_as_cng.h
 *
 * PORT_POLICY.EGR_CFI_AS_CNG Transform Handler
 *
 * This file contains field transform information for PORT_POLICY.EGR_CFI_AS_CNG.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_PORT_POLICY_EGR_CFI_AS_CNG_H
#define BCMLTX_PORT_POLICY_EGR_CFI_AS_CNG_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief PORT_POLICY.EGR_CFI_AS_CNG transform
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
bcmltx_port_policy_egr_cfi_as_cng_transform(int unit,
                                            const bcmltd_fields_t *in,
                                            bcmltd_fields_t *out,
                                            const bcmltd_transform_arg_t *arg);

/*!
 * \brief PORT_POLICY.EGR_CFI_AS_CNG rev transform
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
bcmltx_port_policy_egr_cfi_as_cng_rev_transform(int unit,
                                                const bcmltd_fields_t *in,
                                                bcmltd_fields_t *out,
                                                const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_PORT_POLICY_EGR_CFI_AS_CNG_H */
