/*! \file bcmltx_port_policy_drop_on_pri.h
 *
 * PORT_POLICY.DROP_ON_PRI Transform Handler
 *
 * This file contains field transform information for PORT_POLICY.DROP_ON_PRI.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_PORT_POLICY_DROP_ON_PRI_H
#define BCMLTX_PORT_POLICY_DROP_ON_PRI_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief PORT_POLICY.DROP_ON_PRI transform
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
bcmltx_port_policy_drop_on_pri_transform(int unit,
                                         const bcmltd_fields_t *in,
                                         bcmltd_fields_t *out,
                                         const bcmltd_transform_arg_t *arg);

/*!
 * \brief PORT_POLICY.DROP_ON_PRI rev transform
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
bcmltx_port_policy_drop_on_pri_rev_transform(int unit,
                                             const bcmltd_fields_t *in,
                                             bcmltd_fields_t *out,
                                             const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_PORT_POLICY_DROP_ON_PRI_H */
