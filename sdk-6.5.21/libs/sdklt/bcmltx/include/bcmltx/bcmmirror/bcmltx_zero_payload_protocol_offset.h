/*! \file bcmltx_zero_payload_protocol_offset.h
 *
 * Mirror zero payload protocol offset handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_ZERO_PAYLOAD_PROTOCOL_OFFSET_H
#define BCMLTX_ZERO_PAYLOAD_PROTOCOL_OFFSET_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Mirror zero payload protocol offset transform
 *
 * This function transforms the zero payload protocol offset
 * into individual HW fields.
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
bcmltx_zero_payload_protocol_offset_rev_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg);

/*!
 * \brief Mirror zero payload protocol offset rev transform
 *
 * This function transforms the HW fields into
 * the zero payload protocol offset.
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
bcmltx_zero_payload_protocol_offset_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_ZERO_PAYLOAD_PROTOCOL_OFFSET_H */
