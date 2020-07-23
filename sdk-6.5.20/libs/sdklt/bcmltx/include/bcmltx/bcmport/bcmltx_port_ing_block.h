/*! \file bcmltx_port_ing_block.h
 *
 * PORT_ING_BLOCK_CONTROL.SKIP_ING_PORT_BLOCK Transform Handler
 *
 * This file contains field transform information for PORT_ING_BLOCK_CONTROL.SKIP_ING_PORT_BLOCK.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_PORT_ING_BLOCK_H
#define BCMLTX_PORT_ING_BLOCK_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief PORT_ING_BLOCK_CONTROL.SKIP_ING_PORT_BLOCK transform
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
bcmltx_port_ing_block_transform(
    int unit, const bcmltd_fields_t *in,
    bcmltd_fields_t *out, const bcmltd_transform_arg_t *arg);

/*!
 * \brief PORT_ING_BLOCK_CONTROL.SKIP_ING_PORT_BLOCK rev transform
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
bcmltx_port_ing_block_rev_transform(
    int unit, const bcmltd_fields_t *in,
    bcmltd_fields_t *out, const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_PORT_ING_BLOCK_H */
