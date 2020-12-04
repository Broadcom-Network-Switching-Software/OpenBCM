/*! \file bcmltx_mon_gen_index.h
 *
 * Egress drop mask profile LT key combination to
 * LTM track index transform handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_MON_GEN_INDEX_H
#define BCMLTX_MON_GEN_INDEX_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief MON_EGR_DROP_MASK_PROFILE table's
 * LT key combination to LTM track index transform.
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
bcmltx_mon_gen_index_transform(int unit,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out,
                               const bcmltd_transform_arg_t *arg);

/*!
 * \brief MON_EGR_DROP_MASK_PROFILE table's
 * LT key combination to LTM track index reverse transform.
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
bcmltx_mon_gen_index_rev_transform(int unit,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_MON_GEN_INDEX_H */
