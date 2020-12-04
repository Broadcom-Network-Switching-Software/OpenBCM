/*! \file bcmltx_array_to_multi_fields_set.h
 *
 * Mirror Config Handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_ARRAY_TO_MULTI_FIELDS_SET_H
#define BCMLTX_ARRAY_TO_MULTI_FIELDS_SET_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Mirror Config fields transform
 *
 * This function transforms the MIRROR_ING/EGR_MEMBER.IPV4
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
bcmltx_array_to_multi_fields_set_rev_transform(int unit,
                                       const bcmltd_fields_t *in,
                                       bcmltd_fields_t *out,
                                       const bcmltd_transform_arg_t *arg);

/*!
 * \brief Mirrro IPV4 fields rev transform
 *
 * This function transforms the HW fields into
 * the MIRROR_ING/EGR_MEMBER.IPV4.
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
bcmltx_array_to_multi_fields_set_transform(int unit,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_ARRAY_TO_MULTI_FIELDS_SET_H */
