/*! \file bcmltx_mon_etrap_array_to_bmp.h
 *
 * Elephant Trap(ETRAP) table's array to bitmap field Transform Handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_MON_ETRAP_ARRAY_TO_BMP_H
#define BCMLTX_MON_ETRAP_ARRAY_TO_BMP_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief ETRAP table's array to bitmap fields transform
 *
 * This function transforms the ETRAP table's array
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
bcmltx_mon_etrap_array_to_bmp_rev_transform(int unit,
                                            const bcmltd_fields_t *in,
                                            bcmltd_fields_t *out,
                                            const bcmltd_transform_arg_t *arg);
/*!
 * \brief ETRAP table's array to bitmap fields rev transform
 *
 * This function transforms the HW fields into
 * ETRAP table's array.
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
bcmltx_mon_etrap_array_to_bmp_transform(int unit,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_MON_ETRAP_ARRAY_TO_BMP_H */
