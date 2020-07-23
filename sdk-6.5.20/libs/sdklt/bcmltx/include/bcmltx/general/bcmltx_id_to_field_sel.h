/*! \file bcmltx_id_to_field_sel.h
 *
 * PT field select Transform Handler
 *
 * This file contains field transform information.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_ID_TO_FIELD_SEL_H
#define BCMLTX_ID_TO_FIELD_SEL_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Component extended transform operation function pointer type
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
bcmltx_id_to_field_sel_transform(int unit,
                              const bcmltd_fields_t *in_key,
                              const bcmltd_fields_t *in_value,
                              bcmltd_fields_t *out,
                              const bcmltd_transform_arg_t *arg);

/*!
 * \brief Component extended transform operation function pointer type
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
bcmltx_id_to_field_sel_rev_transform(int unit,
                                  const bcmltd_fields_t *in_key,
                                  const bcmltd_fields_t *in_value,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_ID_TO_FIELD_SEL_H */
