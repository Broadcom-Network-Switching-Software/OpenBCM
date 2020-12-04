/*! \file bcmltx_sec_track_index.h
 *
 * TM table's LT key combination to LTM track index transform handler.
 *
 * Some LTs have multiple keys, and each key combination should be associated to
 * one logical entry. In such cases, a transform function should be provided to
 * let LTM track the entries as it is a 1D indexed table.
 * The transform functions in this files is generic so that they can be applied
 * to all PTs with such fields.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_SEC_TRACK_INDEX_H
#define BCMLTX_SEC_TRACK_INDEX_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief TM table's LT key combination to LTM track index transform
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
bcmltx_sec_track_index_transform(int unit,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_transform_arg_t *arg);

/*!
 * \brief TM table's LT key combination to LTM track index reverse transform
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
bcmltx_sec_track_index_rev_transform(int unit,
                                 const bcmltd_fields_t *in,
                                 bcmltd_fields_t *out,
                                 const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_SEC_TRACK_INDEX_H */
