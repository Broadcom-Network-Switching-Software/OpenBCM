/*! \file bcmltx_lb_hash_bin_udf_chunk_mask.h
 *
 * Load balance hashing bin udf chunk mask Transform Handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_LB_HASH_BIN_UDF_CHUNK_MASK_H
#define BCMLTX_LB_HASH_BIN_UDF_CHUNK_MASK_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief LB hash bin udf chunk mask transform.
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
bcmltx_lb_hash_bin_udf_chunk_mask_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg);

/*!
 * \brief LB hash bin udf chunk mask reverse transform.
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
bcmltx_lb_hash_bin_udf_chunk_mask_rev_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_LB_HASH_BIN_UDF_CHUNK_MASK_H */
