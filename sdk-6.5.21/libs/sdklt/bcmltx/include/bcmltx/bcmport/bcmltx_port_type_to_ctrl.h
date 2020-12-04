/*! \file bcmltx_port_type_to_ctrl.h
 *
 * This file contains field transform
 * utilities for PORT LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_PORT_TYPE_TO_CTRL_H
#define BCMLTX_PORT_TYPE_TO_CTRL_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Forward transform for port type to
 *        port control conversion.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              in fields list array.
 * \param [out] out             out fields list array.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltx_port_type_to_ctrl_transform(int unit,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_transform_arg_t *arg);

/*!
 * \brief Reverse transform for port type to
 *        port control conversion.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              in fields list array.
 * \param [out] out             out fields list array.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltx_port_type_to_ctrl_rev_transform(int unit,
                                       const bcmltd_fields_t *in,
                                       bcmltd_fields_t *out,
                                       const bcmltd_transform_arg_t *arg);
#endif /* BCMLTX_PORT_TYPE_TO_CTRL_H */
