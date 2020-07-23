/*! \file bcmltx_obm_thd_granularity.h
 *
 * Threshold value transform for desired granularity and rounding.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_OBM_THD_GRANULARITY_H
#define BCMLTX_OBM_THD_GRANULARITY_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief OBM threshold value granularity roudning transform.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in_key          LTD key input logical field set.
 * \param [in]  in_value        LTD value input field set.
 * \param [out] out             LTD output field set.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltx_obm_thd_granularity_transform(int unit,
                                     const bcmltd_fields_t *in_key,
                                     const bcmltd_fields_t *in_value,
                                     bcmltd_fields_t *out,
                                     const bcmltd_transform_arg_t *arg);

/*!
 * \brief OBM threshold value granularity roudning reverse transform.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in_key          LTD key input logical field set.
 * \param [in]  in_value        LTD value input field set.
 * \param [out] out             LTD output field set.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical from physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltx_obm_thd_granularity_rev_transform(int unit,
                                         const bcmltd_fields_t *in_key,
                                         const bcmltd_fields_t *in_value,
                                         bcmltd_fields_t *out,
                                         const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_OBM_THD_GRANULARITY_H */
