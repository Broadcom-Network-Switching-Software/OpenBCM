/*! \file bcmltx_copy_offset.h
 *
 * Copy a field from input to output and apply an offset to the output
 * field. There is no reverse transform for this operation - it is
 * forward only.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_COPY_OFFSET_H
#define BCMLTX_COPY_OFFSET_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Offset copy transform
 *
 * Copy input field to output field and add transform_arg[0] as an
 * offset.
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
bcmltx_copy_offset_transform(int unit,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_COPY_OFFSET_H */
