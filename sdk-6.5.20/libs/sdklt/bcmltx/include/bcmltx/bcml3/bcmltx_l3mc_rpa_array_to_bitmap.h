/*! \file bcmltx_l3mc_rpa_array_to_bitmap.h
 *
 * RPA Array Transform Handler
 *
 * This file contains field transform information for RPA Array.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_L3MC_RPA_ARRAY_TO_BITMAP_H
#define BCMLTX_L3MC_RPA_ARRAY_TO_BITMAP_H

#include <bcmltd/bcmltd_handler.h>

/*! RPA profile length. */
#define RPA_PROFILE_ID_LEN 64

/*!
 * \brief RPA Array transform
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
bcmltx_l3mc_rpa_array_to_bitmap_transform(int unit,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg);


/*!
 * \brief RPA Array reverse transform
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
bcmltx_l3mc_rpa_array_to_bitmap_rev_transform(int unit,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg);


#endif /* BCMLTX_L3MC_RPA_ARRAY_TO_BITMAP_H */
