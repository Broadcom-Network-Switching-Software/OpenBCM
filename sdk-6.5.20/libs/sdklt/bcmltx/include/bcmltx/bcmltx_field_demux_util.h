/*! \file bcmltx_field_demux_util.h
 *
 * Utility functions for subfield transform.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_FIELD_DEMUX_UTIL_H
#define BCMLTX_FIELD_DEMUX_UTIL_H

#include <bcmltd/bcmltd_handler.h>

/*! Macro for bcmltx validation */
#define BCMLTX_VALIDATE_ERR(err_cond)        \
    if ((err_cond)) {                        \
        SHR_ERR_EXIT(SHR_E_INTERNAL); \
    }

/*! Data structure for 64-bit chunk. */
typedef struct bcmltx_chunk_s {

    /*! Minbit. */
    uint16_t minbit;

    /*! Maxbit. */
    uint16_t maxbit;

    /*! Bitwidth. */
    uint32_t bitwidth;

    /*! Bitmask. */
    uint64_t bitmask;

    /*! Index to src(LT) field buffer. */
    uint32_t src_idx;

    /*! Index to dst(PT) field buffer. */
    uint32_t dst_idx;

    /*! Offset value used when packing dst(PT) data */
    uint32_t dst_offset;

    /*! Offset value used when packing src(LT) data */
    uint32_t src_offset;
} bcmltx_chunk_t;

/*!
 * \brief Split field range into 64bit chunks.
 *
 * Split field range into 64bit chunks.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  minbit        Minimum bit.
 * \param [in]  maxbit        Maximum bit.
 * \param [out] chunks        Array of chunks.
 * \param [out] nchunks       Number of chunk.
 *
 * \retval SHR_E_NONE         No Error.
 */
extern int
bcmltx_chunk_split(int unit,
                   const uint16_t minbit,
                   const uint16_t maxbit,
                   bcmltx_chunk_t **chunks,
                   uint32_t *nchunks);

/*!
 * \brief Set field data to out buffer.
 *
 * Set field data to out buffer.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  out           Output field buffer.
 * \param [in]  out_index     Index to output buffer.
 * \param [in]  fid           Field ID.
 * \param [in]  fidx          Field index.
 * \param [in]  lta_data      LTA Data.
 *
 * \retval SHR_E_NONE         No Error.
 */
extern int
bcmltx_lta_field_data_set(int unit,
                          const bcmltd_fields_t *out,
                          uint32_t out_index,
                          uint32_t fid,
                          uint32_t fidx,
                          uint64_t lta_data);

#endif /* BCMLTX_FIELD_DEMUX_UTIL_H */
