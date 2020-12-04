/*! \file bcmltx_mon_etrap_filt_hash_sel.h
 *
 * MON_ETRAP_CANDIDATE_FILTER Transform Handler
 *
 * This file contains field transform information for
 * MON_ETRAP_CANDIDATE_FILTER fields.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_MON_ETRAP_FILT_HASH_SEL_H
#define BCMLTX_MON_ETRAP_FILT_HASH_SEL_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief ETRAP filter hash fields transform
 *
 * This function transforms the hash selection
 * array into individual HW fields.
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
bcmltx_mon_etrap_filt_hash_sel_transform(int unit,
                                         const bcmltd_fields_t *in,
                                         bcmltd_fields_t *out,
                                         const bcmltd_transform_arg_t *arg);

/*!
 * \brief ETRAP filter hsah fields rev transform
 *
 * This function transforms the HE fields into
 * ETRAP hash function selector array.
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
bcmltx_mon_etrap_filt_hash_sel_rev_transform(int unit,
                                             const bcmltd_fields_t *in,
                                             bcmltd_fields_t *out,
                                             const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_MON_ETRAP_FILT_HASH_SEL_H */
