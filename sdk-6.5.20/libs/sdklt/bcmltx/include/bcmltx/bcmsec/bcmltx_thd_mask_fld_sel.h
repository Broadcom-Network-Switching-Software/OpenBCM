/*! \file bcmltx_thd_mask_fld_sel.h
 *
 * SEC table's Transform Handler
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_THD_MASK_FLD_SEL_H
#define BCMLTX_THD_MASK_FLD_SEL_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Component extended transform operation function pointer type
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in_key          LTD key input logical field set.
 * \param [in]  in              LTD value input field set.
 * \param [out] out             LTD output field set.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltx_thd_mask_fld_sel_transform (int unit,
                                   const bcmltd_fields_t *in_key,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_transform_arg_t *arg);

/*!
 * \brief Component extended transform operation function pointer type
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in_key          LTD key input logical field set.
 * \param [in]  in              LTD value input field set.
 * \param [out] out             LTD output field set.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltx_thd_mask_fld_sel_rev_transform(int unit,
                                      const bcmltd_fields_t *in_key,
                                      const bcmltd_fields_t *in,
                                      bcmltd_fields_t *out,
                                      const bcmltd_transform_arg_t *arg);
#endif /* BCMLTX_THD_MASK_FLD_SEL_H */
