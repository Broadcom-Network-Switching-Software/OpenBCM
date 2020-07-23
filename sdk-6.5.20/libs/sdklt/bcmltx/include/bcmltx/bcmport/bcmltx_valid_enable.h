/*! \file bcmltx_valid_enable.h
 *
 * Set the enable/valid field given a non-zero input.
 *
 * This file contains field transform information for PORT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_VALID_ENABLE_H
#define BCMLTX_VALID_ENABLE_H

#include <bcmltd/bcmltd_handler.h>

 /*!
 * \brief Enable a field given a non-zero input
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
bcmltx_valid_enable_transform(int unit,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_VALID_ENABLE_H */
