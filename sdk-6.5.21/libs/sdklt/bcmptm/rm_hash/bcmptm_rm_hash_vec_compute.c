/*! \file bcmptm_rm_hash_vec_compute.c
 *
 * Routines that implement hash vector computing.
 *
 * This file contains the functions that implements hash vector computing
 * for hash table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_crc.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include "bcmptm_rm_hash.h"
#include "bcmptm_rm_hash_vec_compute.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMHASH


/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */
/*!
 * \brief Compute CRC16 hash vector.
 *
 * \param [in] data Pointer to the buffer that contains the key.
 * \param [in] data_nbits Key size in unit of bit.
 */
static uint16_t
rm_hash_crc16b(uint8_t *data, int data_nbits)
{
    uint16_t rv;
    rv = shr_crc16b(0, data, data_nbits);
    rv = shr_bit_rev16(rv);
    return rv;
}

/*!
 * \brief Compute CRC32 hash vector.
 *
 * \param [in] data Pointer to the buffer that contains the key.
 * \param [in] data_nbits Key size in unit of bit.
 */
static uint32_t
rm_hash_crc32b(uint8_t *data, int data_nbits)
{
    uint32_t rv;
    rv = shr_crc32b(0, data, data_nbits);
    rv = shr_bit_rev_by_byte_word32(rv);
    return rv;
}

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_hash_vector_compute(int unit,
                              uint32_t *entry,
                              uint8_t *key_a,
                              uint8_t *key_b,
                              int key_size,
                              const bcmptm_rm_hash_vec_attr_t *vec_attr,
                              rm_hash_bm_t e_bm,
                              rm_hash_inst_srch_info_t *inst_srch_info)
{
    uint32_t lsb_val = 0, crc16_val = 0, crc32_val = 0;
    uint32_t vector, upper_half = 0, lower_half = 0;
    uint16_t sbit, ebit, uppermost = 0;
    uint8_t idx, num_en_rbanks, *en_rbank;
    bool upper_half_computed = FALSE;
    bool lower_half_computed = FALSE;
    uint64_t hash_mask, tmp_vector;
    rm_hash_vector_info_t *vec_info = NULL;
    rm_hash_pt_info_t *pt_info = NULL;

    SHR_FUNC_ENTER(unit);

    pt_info = inst_srch_info->fmt_info->pt_info;
    num_en_rbanks = inst_srch_info->fmt_info->num_en_rbanks;
    en_rbank = inst_srch_info->fmt_info->en_rbank_list;

    for (idx = 0; idx < num_en_rbanks; idx++) {
        bcmptm_rm_hash_vector_type_t type;
        uint16_t offset;
        uint32_t mask;
        uint8_t rbank;

        /*
         * lbank_list contains the logical banks in a pt info that are
         * to calculate hash vectors.
         */
        rbank = en_rbank[idx];
        vec_info = &pt_info->vec_info[rbank];
        type = vec_info->type;
        offset = vec_info->offset;
        /*
         * The mask used for generating hash vector should be in the unit of
         * hardware logical buckets.
         */
        switch (e_bm) {
        case RM_HASH_BM_N:
        case RM_HASH_BM_0:
            mask = vec_info->mask;
            break;
        case RM_HASH_BM_1:
            mask = (vec_info->mask + 1) / 2 - 1;
            break;
        case RM_HASH_BM_2:
            mask = (vec_info->mask + 1) / 4 - 1;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        switch (type) {
        case BCMPTM_RM_HASH_VEC_CRC16:
            if (idx == 0) {
                crc16_val = rm_hash_crc16b(key_a, key_size);
            } else {
                crc16_val = rm_hash_crc16b(key_b, key_size);
            }
            inst_srch_info->hlb_list[idx] = (crc16_val >> offset) & mask;
            break;
        case BCMPTM_RM_HASH_VEC_CRC32:
            if (idx == 0) {
                crc32_val = rm_hash_crc32b(key_a, key_size);
            } else {
                crc32_val = rm_hash_crc32b(key_b, key_size);
            }
            inst_srch_info->hlb_list[idx] = (crc32_val >> offset) & mask;
            break;
        case BCMPTM_RM_HASH_VEC_ZERO:
            inst_srch_info->hlb_list[idx] = 0;
            break;
        case BCMPTM_RM_HASH_VEC_LSB:
            sbit = vec_attr->hw_lsb_field_start_bit;
            ebit = sbit + vec_attr->hw_lsb_field_width - 1;
            bcmdrd_field_get(entry, sbit, ebit, &lsb_val);
            inst_srch_info->hlb_list[idx] = lsb_val;
            break;
        /* 64-bit length shared hash vector */
        case BCMPTM_RM_HASH_VEC_CRC32_CRC16_LSB:
            sbit = vec_attr->hw_lsb_field_start_bit;
            ebit = sbit + vec_attr->hw_lsb_field_width - 1;
            bcmdrd_field_get(entry, sbit, ebit, &lsb_val);
            uppermost = (uint16_t)lsb_val;
        /* coverity[fallthrough: FALSE] */
        case BCMPTM_RM_HASH_VEC_CRC32_CRC16_ZERO:
            if (offset >= 48) {
                vector = uppermost;
                if (offset > 48) {
                    vector >>= offset - 48;
                }
                inst_srch_info->hlb_list[idx] = vector & mask;
            } else if (offset >= 32) {
                if (upper_half_computed == FALSE) {
                    upper_half = rm_hash_crc16b(key_a, key_size) |
                             (uppermost << 16);
                    upper_half_computed = TRUE;
                }
                vector = upper_half;
                if (offset > 32) {
                    vector >>= offset - 32;
                }
                inst_srch_info->hlb_list[idx] = vector & mask;
            } else {
                if (lower_half_computed == FALSE) {
                    lower_half = rm_hash_crc32b(key_a, key_size);
                    lower_half_computed = TRUE;
                }
                if (upper_half_computed == FALSE) {
                    upper_half = rm_hash_crc16b(key_a, key_size) |
                             (uppermost << 16);
                    upper_half_computed = TRUE;
                }
                vector = lower_half;
                if (offset > 0) {
                    vector >>= offset;
                    vector |= upper_half << (32 - offset);
                }
                inst_srch_info->hlb_list[idx] = vector & mask;
            }
            break;
        case BCMPTM_RM_HASH_VEC_CRC32A_CRC32B:
            /*  64 vector {crc32_B[31:0], crc32_A[31:0]} */
            if (inst_srch_info->hash_vector->valid == FALSE) {
                upper_half = rm_hash_crc32b(key_b, key_size);
                lower_half = rm_hash_crc32b(key_a, key_size);
                inst_srch_info->hash_vector->vec = upper_half;
                inst_srch_info->hash_vector->vec <<= 32;
                inst_srch_info->hash_vector->vec |= lower_half;
                inst_srch_info->hash_vector->valid = TRUE;
            }
            tmp_vector = inst_srch_info->hash_vector->vec;
            hash_mask = mask;
            tmp_vector >>= offset;
            tmp_vector &= hash_mask;
            inst_srch_info->hlb_list[idx] = (uint32_t)tmp_vector;
            break;
        default:
            break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}
