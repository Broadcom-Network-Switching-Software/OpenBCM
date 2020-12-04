/*! \file bcmptm_rm_hash_key_extract.c
 *
 * Key extraction implementation for hash table
 *
 * This file contains the functions that implements key extraction for hash table
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
#include <bcmdrd/bcmdrd_field.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include "bcmptm_rm_hash.h"
#include "bcmptm_rm_hash_key_extract.h"
#include "bcmptm_rm_hash_robust_hash.h"
#include "bcmptm_rm_hash_lmem.h"

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


/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_hash_key_extract(int unit,
                           uint32_t *entry,
                           const bcmlrd_hw_field_list_t *key_flds,
                           uint16_t max_key_len,
                           rm_hash_rhash_ctrl rhash_ctrl,
                           uint8_t *key,
                           int *raw_key_length,
                           int *key_size)
{
    uint16_t num, idx;
    int key_index, val_index, fval_index;
    int rshft_cnt, lshft_cnt;
    rm_hash_entry_buf_t *ent_buf = NULL, *key_field_buf = NULL;
    int val_bits, fld_width, bits;
    bool alignment = FALSE, enable = FALSE;

    SHR_FUNC_ENTER(unit);

    ent_buf = bcmptm_rm_hash_entry_buf_alloc();
    if (ent_buf == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    key_field_buf = bcmptm_rm_hash_entry_buf_alloc();
    if (key_field_buf == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(ent_buf->e_words, 0, sizeof(ent_buf->e_words));
    num = key_flds->num_fields;
    if (rhash_ctrl != NULL) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_rbust_alignment_get(unit,
                                                rhash_ctrl,
                                                &alignment));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_rbust_enable_get(unit,
                                             rhash_ctrl,
                                             &enable));
    }

    if ((rhash_ctrl != NULL) && ((alignment == TRUE) || (enable == TRUE))) {
        /*
         * Some devices have word alignment requirement, no matter if
         * robust hash is enabled.
         */
        bits = max_key_len;
        val_bits = 0;
    } else {
        bits = (max_key_len + 7) & ~0x7;
        val_bits = bits - max_key_len;
    }
    for (idx = 0; idx < num; idx++) {
        int sbit, ebit;
        uint16_t num_words;
        num_words = (key_flds->field_width[idx] + 31) / 32;
        key_field_buf->e_words[num_words - 1] = 0;
        sbit = key_flds->field_start_bit[idx];
        ebit = key_flds->field_start_bit[idx] + key_flds->field_width[idx] - 1;
        bcmdrd_field_get(entry, sbit, ebit, key_field_buf->e_words);
        fld_width = key_flds->field_width[idx];
        val_index = val_bits >> 5;
        fval_index = 0;
        lshft_cnt = val_bits & 0x1f;
        rshft_cnt = 32 - lshft_cnt;
        val_bits += fld_width;
        if (lshft_cnt) {
            for (; fld_width > 0; fld_width -= 32) {
                ent_buf->e_words[val_index++] |=
                    key_field_buf->e_words[fval_index] << lshft_cnt;
                ent_buf->e_words[val_index] |=
                    key_field_buf->e_words[fval_index++] >> rshft_cnt;
            }
        } else {
            for (; fld_width > 0; fld_width -= 32) {
                ent_buf->e_words[val_index++] = key_field_buf->e_words[fval_index++];
            }
        }
    }
    key_index = 0;
    for (val_index = 0; val_bits > 0; val_index++) {
        for (rshft_cnt = 0; rshft_cnt < 32;
             rshft_cnt += 8) {
            if (val_bits <= 0) {
                break;
            }
            key[key_index++] = (ent_buf->e_words[val_index] >> rshft_cnt) & 0xff;
            val_bits -= 8;
        }
    }
    if ((bits + 7) / 8 > key_index) {
        sal_memset(&key[key_index], 0, (bits + 7) / 8 - key_index);
    }
    if ((rhash_ctrl != NULL) && (alignment == FALSE)) {
        if (enable) {
            *raw_key_length = bits;
        }
        /*
         * crc generator has the same key length irrespective
         * of whether robust hash is enabled. And for some device,
         * there is no word alignment requirement.
         */
        bits = (bits + 7) & ~0x7;
        bits += BCMPTM_RM_HASH_RHASH_KEY_SPACE_WIDTH;
    } else if ((rhash_ctrl != NULL) && (alignment == TRUE)) {
        if (enable) {
            *raw_key_length = max_key_len;
        }
         /*
         * Some devices have word alignment requirement, no matter if
         * robust hash is enabled.
         */
        bits = (((max_key_len - 1) / 32) + 1) * 32;
        bits += BCMPTM_RM_HASH_RHASH_KEY_SPACE_WIDTH;
    }
    *key_size = bits;

exit:
    if (ent_buf != NULL) {
        bcmptm_rm_hash_entry_buf_free(ent_buf);
    }
    if (key_field_buf != NULL) {
        bcmptm_rm_hash_entry_buf_free(key_field_buf);
    }
    SHR_FUNC_EXIT();
}
