/*! \file bcmltx_sectag_icv_xform.h
 *
 * TM table's index Transform Handler
 *
 * This file contains field transform information for SEC.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_SECTAG_ICV_XFORM_H
#define BCMLTX_SECTAG_ICV_XFORM_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Component extended transform operation function pointer type
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              LTD key input logical field set.
 * \param [out] out             LTD output field set.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltx_sectag_icv_xform_transform(int unit,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_transform_arg_t *arg);

/*!
 * \brief Component extended transform operation function pointer type
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              LTD key input logical field set.
 * \param [out] out             LTD output field set.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical from physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltx_sectag_icv_xform_rev_transform(int unit,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_SECTAG_ICV_XFORM_H */
