/*! \file bcmltx_field_demux_util.c
 *
 * Utility functions for subfield transform.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bcmltx/bcmltx_field_demux_util.h>

/*! BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL

/*!
 * \brief Create bitmask
 *
 * Create bitmask based on given bitwidth.
 *
 * \param [in]  bitwidth      Bitwidth.
 *
 * \retval bitmask            Bitmask.
 */
static uint64_t
bcmltx_create_bitmask(uint32_t bitwidth)
{
    return (bitwidth < 64) ?
           ((1ULL << bitwidth) - 1) : (~0ULL);
}

int
bcmltx_chunk_split(int unit,
                   const uint16_t src_minbit,
                   const uint16_t src_maxbit,
                   bcmltx_chunk_t **chunks,
                   uint32_t *nchunks)
{
    uint32_t n_chunks;
    uint32_t min_idx, max_idx, diff_idx;
    uint32_t idx;
    bcmltx_chunk_t *info = NULL;
    uint32_t size;
    uint32_t offset = 0;
    uint32_t dst_idx = 0;
    uint16_t minbit = src_minbit;
    uint16_t maxbit = src_maxbit;

    SHR_FUNC_ENTER(unit);
    /* Number of chunks */
    min_idx = src_minbit / BCM_FIELD_SIZE_BITS;
    max_idx = src_maxbit / BCM_FIELD_SIZE_BITS;
    diff_idx = max_idx - min_idx;
    if ((src_minbit % BCM_FIELD_SIZE_BITS) == 0) {
        /* Words aligned on src and dst */
        n_chunks = diff_idx + 1;
    } else {
         /* Words alignment must shift between src and dst */
         if (diff_idx == 0) {
             n_chunks = 1;
         } else if (src_minbit > 0 &&
                    src_maxbit >= (src_minbit +
                                   (BCM_FIELD_SIZE_BITS * diff_idx))) {
             n_chunks = diff_idx * 2 + 1;
         } else {
             n_chunks = diff_idx * 2;
         }
     }

    /* Alloc memory for chunks */
    size = sizeof(bcmltx_chunk_t) * n_chunks;
    SHR_ALLOC(info, size, "bcmltxChunkInfo");
    SHR_NULL_CHECK(info, SHR_E_MEMORY);
    sal_memset(info, 0, size);

    /* Fill each chunk information. */
    for (idx = 0; idx < n_chunks; idx++) {
        if (idx == 0) {
            /* First chunk */
            minbit = src_minbit;
            maxbit = (diff_idx == 0) ? src_maxbit :
                                       (min_idx + idx + 1) * BCM_FIELD_SIZE_BITS - 1;
            /* Current chunk bitwidth is the offset of next chunk. */
            offset = maxbit - minbit + 1;
        } else if (idx == (n_chunks - 1)) {
            /* Last chunk */
            minbit = maxbit + 1;
            maxbit = src_maxbit;
        } else {
            /* Middle chunks */
            minbit = maxbit + 1;
            if (minbit % BCM_FIELD_SIZE_BITS) {
                /* MSB chunk */
                maxbit = minbit + BCM_FIELD_SIZE_BITS - 1;
            } else {
                if (src_minbit % BCM_FIELD_SIZE_BITS) {
                    /* LSB chunk */
                    maxbit = minbit + (src_minbit % BCM_FIELD_SIZE_BITS) - 1;
                } else {
                    /* Aligned word copy */
                    maxbit = minbit + BCM_FIELD_SIZE_BITS - 1;
                }
            }
        }

        info[idx].minbit = minbit;
        info[idx].maxbit = maxbit;
        info[idx].bitwidth = maxbit - minbit + 1;
        info[idx].bitmask = bcmltx_create_bitmask(info[idx].bitwidth);
        info[idx].src_idx  = maxbit / BCM_FIELD_SIZE_BITS;
        info[idx].dst_idx = dst_idx;
        /* The dst_idx is the index to field buffer in/out->field[dst_idx].
         * In case data is wider than 64 bits and across multiple indexes,
         * dst_idx would be incremented when
         * - if the index is odd number if src_minbit is non-0.
         * - if the index is even number if src_minbit is 0.
         *
         * - Example of wide field demux accmulated transform.
         * in->field(src)                       out->field(dst)
         *   +---------+--------+---------+       +---------+-----------+---------+
         *   | src_idx |63     x|x-1     0|       | dst_idx |63   63-x+1|63-x    0|
         *   +---------+--------+---------+       +---------+-----------+---------+
         *   | 0       |  data1 |         |       | 0       |   data2   |  data1  |
         *   +---------+--------+---------+       +---------+-----------+---------+
         *   | 1       |  data3 |  data2  |       | 1       |   data4   |  data3  |
         *   +---------+--------+---------+       +---------+-----------+---------+
         *   | 2       |  data5 |  data4  |       | 2       |           |  data5  |
         *   +---------+--------+---------+       +---------+-----------+---------+
         */
        if ((src_minbit % BCM_FIELD_SIZE_BITS) == 0) {
            /* Increment dst_idx for aligned copy*/
            dst_idx++;
            info[idx].dst_offset = 0;
            info[idx].src_offset = 0;
        } else {
            if ((idx % 2) == 1) {
                /* Increment dst_idx */
                dst_idx++;
                info[idx].dst_offset = offset;
                info[idx].src_offset = 0;
            } else {
                info[idx].dst_offset = 0;
                info[idx].src_offset = (minbit % BCM_FIELD_SIZE_BITS);
            }
        }
    }

    *chunks = &info[0];
    *nchunks = n_chunks;
    SHR_EXIT();

 exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(info);
    }
    SHR_FUNC_EXIT();
}

int
bcmltx_lta_field_data_set(int unit,
                          const bcmltd_fields_t *out,
                          uint32_t out_idx,
                          uint32_t fid,
                          uint32_t fidx,
                          uint64_t lta_data)
{
    bcmltd_field_t *cur_lta_field;

    SHR_FUNC_ENTER(unit);

    cur_lta_field = out->field[out_idx];
    cur_lta_field->id = fid;
    cur_lta_field->idx = fidx;
    cur_lta_field->data |= lta_data;

    SHR_EXIT();
 exit:
    SHR_FUNC_EXIT();
}

