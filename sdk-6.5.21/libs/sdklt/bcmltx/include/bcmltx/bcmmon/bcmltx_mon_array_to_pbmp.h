/*! \file bcmltx_mon_array_to_pbmp.h
 *
 * MON LT's array to bitmap field Transform Handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_MON_ARRAY_TO_PBMP_H
#define BCMLTX_MON_ARRAY_TO_PBMP_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Table's array to bitmap fields transform
 *
 * This function transforms the table's array
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
bcmltx_mon_array_to_pbmp_rev_transform(int unit,
                                       const bcmltd_fields_t *in,
                                       bcmltd_fields_t *out,
                                       const bcmltd_transform_arg_t *arg);
/*!
 * \brief Table's array to bitmap fields rev transform
 *
 * This function transforms the HW fields into
 * table's array.
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
bcmltx_mon_array_to_pbmp_transform(int unit,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_transform_arg_t *arg);


#endif /* BCMLTX_MON_ARRAY_TO_PBMP_H */
