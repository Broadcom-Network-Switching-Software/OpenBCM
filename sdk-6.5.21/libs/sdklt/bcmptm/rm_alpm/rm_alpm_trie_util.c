/*! \file rm_alpm_trie_util.c
 *
 * Trie utility functions for alpm
 *
 * This file contains the implementation for trie utilities
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
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>

#include "rm_alpm_trie.h"
#include "rm_alpm_trie_util.h"

/*******************************************************************************
 * Defines
 */
#define UTIL_ASSERT(x)   if (!(x)) \
        LOG_ERROR(BSL_LS_BCMPTM_RMALPMTRIE, ( BSL_META("Trie Util Assert Fail\n")));

/*******************************************************************************
 * Private variables
 */
static int _alpm_util_debug_ = FALSE;


/*******************************************************************************
 * Private Functions
 */

/*
 *
 * Function:
 *    lsb_get
 * Input:
 *     max_mask_size  -- number of bits in the mask
 *                      ipv4 == 48
 *                      ipv6 == 144
 *     mask  -- uint32_t array head.
 *              for ipv4. Mask[0].bit15-0 is mask bits 47-32
 *                        Mask[1] is mask bits 31-0
 *              for ipv6. Mask[0].bit15-0 is mask bits 143-128
 *                        Mask[1-4] is mask bits 127-0
 *     lsb   -- -1 if no bit is set. bit position of the least
 *              significant bit that is set to 1
 * Purpose:
 *     get the bit position of the least significant bit that is set in mask.
 *     for example:
 *         for ipv4, max_mask_size == 48
 *         mask[0]=0, mask[1]=0x30 will return lsb=4
 *         mask[0]=3, mask[1]=0 will return lsb=32
 */
static int lsb_get(uint32_t max_mask_size, uint32_t *mask, int *lsb)
{
    int word_idx, bit_idx;

    if (!mask || !lsb) {
        return SHR_E_PARAM;
    }

    *lsb = -1;
    for (word_idx = (BITS2WORDS(max_mask_size) - 1); word_idx >= 0;
         word_idx--) {
        if (mask[word_idx] != 0) {
            for (bit_idx = 0; bit_idx < 32; bit_idx++) {
                if ((mask[word_idx] & (1 << bit_idx)) &&
                    (((BITS2WORDS(max_mask_size) - 1 - word_idx) * 32 +
                      bit_idx) < max_mask_size)) {
                    *lsb = (BITS2WORDS(max_mask_size) - 1 - word_idx) * 32 +
                           bit_idx;
                    return SHR_E_NONE;
                }
            }
        }
    }

    return SHR_E_NONE;
}

/*******************************************************************************
 * Public Functions
 */

/*
 * Get a chunk of bits from a key (MSB bit - on word0, lsb on word 1)..
 */
uint32_t bcmptm_rm_alpm_key_bits_get(uint32_t max_key_len,
                                     uint32_t *key,
                                     uint32_t msb, /* 1based msb bit position */
                                     uint32_t len,
                                     uint32_t skip_len_check)
{
    uint32_t lsb, lsb_idx, data;

    /* coverity[var_deref_op : FALSE] */
#if 0
    if (!key || (msb < len) || (msb > max_key_len) ||
        ((skip_len_check == 0) && (len > MAX_SKIP_LEN))) {
        UTIL_ASSERT(0);
        return 0;
    }
#endif
    lsb = msb - len; /* 0based */
    lsb_idx = KEY_BIT2IDX(max_key_len, lsb + 1);
    /* coverity[result_independent_of_operands] */
    lsb = lsb & NUM_WORD_MASK;
    data = SHR(key[lsb_idx], lsb, NUM_WORD_BITS);
    if (lsb_idx >= 1) {
        data |= SHL(key[lsb_idx - 1], 32 - lsb, NUM_WORD_BITS);
    }

    return data & (BITMASK((len)));
}



/*
 *
 * Function:
 *     bcmptm_rm_alpm_key_shift
 * Input:
 *     max_key_size  -- max number of bits in the key
 *                      ipv4 == 48
 *                      ipv6 == 144
 *     key   -- uint32_t array head. Only "length" number of bits
 *              is passed in.
 *              for ipv4. Key[0].bit15-0 is key bits 47-32
 *                        Key[1] is key bits 31-0
 *              for ipv6. Key[0].bit15-0 is key bits 143-128
 *                        Key[1-4] is key bits 127-0
 *     length-- number of valid bits in key array. This would be
 *              valid MSB bits of the route. For example,
 *              (vrf=0x1234, ip=0xf0000000, length=20) would store
 *              as key[0] = 0, key[1]=0x1234F, length=20.
 *     shift -- positive means right shift, negative means left shift
 *              routine will check if the shifted key is out of
 *              max_key_size boundary.
 *     NOTE: if length is not cared, input max_len for >>, 0 for <<.
 */
int bcmptm_rm_alpm_key_shift(uint32_t max_key_size,
                             uint32_t *key,
                             uint32_t length,
                             int      shift)
{
    uint32_t word_idx, lsb;

    if ((key == NULL) ||
        (length > max_key_size)) {
        return SHR_E_PARAM;
    }

    if (((length - shift) > max_key_size) ||
        ((shift > 0) && ((uint32_t)shift > length))) {
        /* left shift resulted in key longer than max_key_size or
         * right shift resulted in key shorter than 0
         */
        return SHR_E_PARAM;
    }

    if (_alpm_util_debug_) {
        LOG_VERBOSE(BSL_LS_BCMPTM_RMALPMTRIE,
                    (BSL_META("Original key before shift:\n")));
        bcmptm_rm_alpm_prefix_show(max_key_size, key, length);
    }

    if (shift > 0) {
        /* right shift */
        for (lsb = shift, word_idx = BITS2WORDS(max_key_size) - 1;
             (int)word_idx >= 0;
             lsb += 32, word_idx--) {
            if (lsb < length) {
                uint32_t len = ((length - lsb) >= 32) ? 32 : (length - lsb);
                key[word_idx] = bcmptm_rm_alpm_key_bits_get(max_key_size,
                                                            key, lsb + len, len,
                                                            1);
            } else {
                key[word_idx] = 0;
            }
        }
    } else if (shift < 0) {
        /* left shift */
        shift = 0 - shift;

        /* whole words shifting first */
        for (word_idx = 0;
             ((shift / 32) != 0) && (word_idx < BITS2WORDS(max_key_size));
             word_idx++) {
            if ((word_idx + (shift / 32)) >= BITS2WORDS(max_key_size)) {
                key[word_idx] = 0;
            } else {
                key[word_idx] = key[word_idx + (shift / 32)];
            }
        }

        /* shifting remaining bits */
        for (word_idx = 0;
             ((shift % 32) != 0) && (word_idx < BITS2WORDS(max_key_size));
             word_idx++) {
            if (word_idx == KEY_BIT2IDX(max_key_size, 1)) {
                /* at bit 0 word, next word doesn't exist */
                key[word_idx] = SHL(key[word_idx], (shift % 32), NUM_WORD_BITS);
            } else {
                key[word_idx] =
                    SHL(key[word_idx], (shift % 32), NUM_WORD_BITS) | \
                    SHR(key[word_idx + 1], 32 - (shift % 32),
                        NUM_WORD_BITS);
            }
        }

        /* mask off bits higher than max_key_size */
        /* coverity[result_independent_of_operands] */
        key[0] &= BITMASK(max_key_size % 32);
    }

    if (_alpm_util_debug_) {
        LOG_VERBOSE(BSL_LS_BCMPTM_RMALPMTRIE,
                    (BSL_META("Resulted key after shift:\n")));
        bcmptm_rm_alpm_prefix_show(max_key_size, key, length - shift);
    }

    return SHR_E_NONE;
}

/*
 *
 * Function:
 *     bcmptm_rm_alpm_key_match
 * Input:
 *     max_key_size  -- max number of bits in the key
 *                      ipv4 == 48
 *                      ipv6 == 144
 *     key1  -- uint32_t array head for first key. Only "length1" number of bits
 *              is passed in.
 *              for ipv4. Key[0].bit15-0 is key bits 47-32
 *                        Key[1] is key bits 31-0
 *              for ipv6. Key[0].bit15-0 is key bits 143-128
 *                        Key[1-4] is key bits 127-0
 *     length1-- number of valid bits in key1 array. This would be
 *              valid MSB bits of the route. For example,
 *              (vrf=0x1234, ip=0xf0000000, length=20) would store
 *              as key[0] = 0, key[1]=0x1234F, length=20.
 *     key2  -- uint32_t array head for second key. Only "length2" number of
 *              bits is passed in.
 *              for ipv4. Key[0].bit15-0 is key bits 47-32
 *                        Key[1] is key bits 31-0
 *              for ipv6. Key[0].bit15-0 is key bits 143-128
 *                        Key[1-4] is key bits 127-0
 *     length2-- number of valid bits in key2 array. This would be
 *              valid MSB bits of the route. For example,
 *              (vrf=0x1234, ip=0xf0000000, length=20) would store
 *              as key[0] = 0, key[1]=0x1234F, length=20.
 * Purpose:
 *     Return TRUE if the all the valid bits of the shorter key matches
 *     the corresponding bits of the longer key. Otherwise return FALSE.
 *
 *     This function does NOT modify either keys.
 *
 *     max_key_length supported is 256 bits.
 */
int bcmptm_rm_alpm_key_match(uint32_t max_key_size,
                             uint32_t *key1, uint32_t length1,
                             uint32_t *key2, uint32_t length2)
{
#define MAX_MATCH_KEY_SIZE (256)

    uint32_t tmp_key1[BITS2WORDS(MAX_MATCH_KEY_SIZE)];
    uint32_t tmp_key2[BITS2WORDS(MAX_MATCH_KEY_SIZE)];
    int rv = SHR_E_NONE;
    uint32_t index;

    if (_alpm_util_debug_) {
        LOG_VERBOSE(BSL_LS_BCMPTM_RMALPMTRIE,
                    (BSL_META("key1:\n")));
        bcmptm_rm_alpm_prefix_show(max_key_size, key1, length1);

        LOG_VERBOSE(BSL_LS_BCMPTM_RMALPMTRIE,
                    (BSL_META("key2:\n")));
        bcmptm_rm_alpm_prefix_show(max_key_size, key2, length2);
    }

    /* copy key bits */
    for (index = 0; index < BITS2WORDS(max_key_size); index++) {
        tmp_key1[index] = key1[index];
        tmp_key2[index] = key2[index];
    }

    /* shift out the LSBs of the longer key */
    if (length1 > length2) {
        rv = bcmptm_rm_alpm_key_shift(max_key_size,
                                      tmp_key1, length1, (length1 - length2));
    } else if (length2 > length1) {
        rv = bcmptm_rm_alpm_key_shift(max_key_size,
                                      tmp_key2, length2, (length2 - length1));
    }
    if (rv != SHR_E_NONE) {
        LOG_VERBOSE(BSL_LS_BCMPTM_RMALPMTRIE,
                    (BSL_META("taps key shift failed 0x%x\n"), rv));
    }

    for (index = 0; index < BITS2WORDS(max_key_size); index++) {
        if (tmp_key1[index] != tmp_key2[index]) {
            /* some bits not matching */
            if (_alpm_util_debug_) {
                LOG_VERBOSE(BSL_LS_BCMPTM_RMALPMTRIE,
                            (BSL_META("Key1 and key2 not matching:\n")));
            }
            return FALSE;
        }
    }

    /* all matched */
    if (_alpm_util_debug_) {
        LOG_VERBOSE(BSL_LS_BCMPTM_RMALPMTRIE,
                    (BSL_META("Key1 and key2 matched:\n")));
    }
    return TRUE;
}

/*
 * Function:
 *     bcmptm_rm_alpm_key_append
 * Purpose:
 *     Append addr to existing key
 * Note:
 *     Assumes the layout for
 *     0 - most significant word
 *     _MAX_KEY_WORDS_ - least significant word
 *     eg., for key size of 48, word0-[bits 48-32] word1-[bits31-0]
 */
int bcmptm_rm_alpm_key_append(uint32_t max_key_len,
                              uint32_t *key,
                              uint32_t *length,
                              uint32_t skip_addr,
                              uint32_t skip_len)
{
    int rv = SHR_E_NONE;

    if (!key || !length || ((skip_len + *length) > max_key_len) ||
        (skip_len > MAX_SKIP_LEN) ) {
        return SHR_E_PARAM;
    }

    rv = bcmptm_rm_alpm_key_shift(max_key_len, key, *length, 0 - (int)skip_len);
    if (SHR_SUCCESS(rv)) {
        key[KEY_BIT2IDX(max_key_len, 1)] |= skip_addr;
        *length += skip_len;
    }

    return rv;
}

/*
 * Function:
 *     bcmptm_rm_alpm_bpm_append
 * Purpose:
 *     Append addr to existing key
 */
int bcmptm_rm_alpm_bpm_append(uint32_t max_key_len,
                              uint32_t *key,
                              uint32_t *length,
                              uint32_t skip_addr,
                              uint32_t skip_len)
{
    int rv = SHR_E_NONE;

    if (!key || !length || ((skip_len + *length) > max_key_len) ||
        (skip_len > (MAX_SKIP_LEN + 1)) ) {
        return SHR_E_PARAM;
    }

    rv = bcmptm_rm_alpm_key_shift(max_key_len, key, *length, 0 - (int)skip_len);
    if (SHR_SUCCESS(rv)) {
        key[KEY_BIT2IDX(max_key_len, 1)] |= skip_addr;
        *length += skip_len;
    }

    return rv;
}

/*
 * Function:
 *     bcmptm_rm_alpm_bpm_pfx_get
 * Purpose:
 *     finds best prefix match length given a bpm bitmap & key
 */
int bcmptm_rm_alpm_bpm_pfx_get(unsigned int *bpm,
                               unsigned int key_len,
                               unsigned int max_key_len,
                               /* OUT */
                               unsigned int *pfx_len)
{
    int rv = SHR_E_NONE, pos = 0;

    if (!bpm || !pfx_len || (key_len > max_key_len)) {
        return SHR_E_PARAM;
    }

    *pfx_len = 0;

    rv = lsb_get(max_key_len, bpm, &pos);
    if (SHR_SUCCESS(rv)) {
        *pfx_len = (pos < 0) ? 0 : (key_len - pos);
    }
    return rv;
}

/*
 * Function:
 *     bcmptm_rm_alpm_lcplen
 * Purpose:
 *     returns longest common prefix length provided a key & skip address
 */
uint32_t
bcmptm_rm_alpm_lcplen(uint32_t max_key_len,
                      uint32_t *key,
                      uint32_t len1,
                      uint32_t skip_addr,
                      uint32_t len2)
{
    uint32_t diff;
    uint32_t lcp;

#if 0
    if ((len1 > max_key_len) || (len2 > max_key_len)) {
        LOG_CLI((BSL_META("len1 %d or len2 %d is larger than %d\n"),
                 len1, len2, max_key_len));
        UTIL_ASSERT(0);
    }
#endif
    if ((len1 == 0) || (len2 == 0)) {
        return 0;
    }

    lcp = len1 < len2 ? len1 : len2;
    diff = bcmptm_rm_alpm_key_bits_get(max_key_len, key, len1, lcp, 0);
    diff ^= (SHR(skip_addr, len2 - lcp, MAX_SKIP_LEN) & MASK(lcp));

    while (diff) {
        diff >>= 1;
        --lcp;
    }

    return lcp;
}


/*
 *
 * Function:
 *     bcmptm_rm_alpm_prefix_show
 * Input:
 *     key   -- uint32_t array head. Only "length" number of bits
 *              is passed in.
 *              for ipv4. Key[0].bit15-0 is key bits 47-32
 *                        Key[1] is key bits 31-0
 *              for ipv6. Key[0].bit15-0 is key bits 143-128
 *                        Key[1-4] is key bits 127-0
 *     length-- number of valid bits in key array. This would be
 *              valid MSB bits of the route. For example,
 *              (vrf=0x1234, ip=0xf0000000, length=20) would store
 *              as key[0] = 0, key[1]=0x1234F, length=20.
 * Purpose:
 *     print a prefix if "dbm soc verbose"
 */
int bcmptm_rm_alpm_prefix_show(uint32_t max_key_size, uint32_t *key,
                               uint32_t length)
{
    int word, max_words, key_words;

    if ((key == NULL) ||
        (length > max_key_size)) {
        return SHR_E_PARAM;
    }

    LOG_CLI((BSL_META("prefix length %d key 0x"), length));

    key_words = (length + 31) / 32;
    max_words = (max_key_size + 31) / 32;
    for (word = 0; word < max_words; word++) {
        if (word == (max_words - key_words)) {
            LOG_CLI((BSL_META("%x"), key[word]));
        } else if (word > (max_words - key_words)) {
            LOG_CLI((BSL_META("%08x"), key[word]));
        }
    }

    LOG_CLI((BSL_META("\n")));

    return SHR_E_NONE;
}

