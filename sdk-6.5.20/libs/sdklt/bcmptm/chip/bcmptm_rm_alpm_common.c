/*! \file bcmptm_rm_alpm_common.c
 *
 * Common functions for rm alpm device driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include "bcmptm_rm_alpm_common.h"

/*******************************************************************************
 * Defines
 */

#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMALPM

/*******************************************************************************
 * Private Functions
 */

static inline uint32_t
log2n(uint32_t n)
{
   return (n > 1) ? 1 + log2n(n/2) : 0;
}

/*!
 * \brief One bit prefix length in this segment (level).
 *
 * \param [in] fmt_len Format length.
 * \param [in] format_pfx One bit prefix format.
 *
 * \return Prefix length (in bits) or -1 if error.
 */
static int
one_bit_pfx_seg_len(int fmt_len, uint32_t *format_pfx)
{

    int i, bp = 0; /* Bit pos for least significant bit 1 */
    int count = sizeof(alpm_format_ip_t) / sizeof(*format_pfx);

    for (i = 0; i < count; i++) { /* Optimized for performance */
        if (format_pfx[i] == 0) {
            bp += 32;
        } else {
            bp += log2n(format_pfx[i] & (~format_pfx[i] + 1));
            break;
        }
    }
    if (i == count) {
        return -1; /* indicating invalid */
    } else { /* at least 1 valid bit */
        return (fmt_len - bp);
    }
}

/*!
 * \brief One bit format encoding.
 *
 * \param [in] max_bits Max bits
 * \param [in] ip_addr IP address
 * \param [in] len IP address full length
 * \param [in] fmt_len Format length
 * \param [in] kshift Key shift
 * \param [out] format_pfx One bit prefix format
 *
 * \return Always SHR_E_NONE.
 */
static int
one_bit_pfx_encode(int max_bits,
                   uint32_t *ip_addr,
                   int len,
                   int fmt_len,
                   int kshift,
                   uint32_t *format_pfx)
{
    int src_sbit;
    int seg_len = len - kshift;

    ALPM_ASSERT(sizeof(alpm_format_ip_t) / sizeof(alpm_ip_t) == 2);
    sal_memset(format_pfx, 0, sizeof(alpm_format_ip_t));
    ALPM_ASSERT(seg_len <= fmt_len);
    src_sbit = seg_len - fmt_len + (sizeof(alpm_ip_t) * 8);
    SHR_BITCOPY_RANGE(format_pfx, 1, ip_addr, src_sbit, fmt_len);
    SHR_BITSET(format_pfx, fmt_len - seg_len);
#ifdef ALPM_DEBUG
    if (LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        int seg_len2;
        seg_len2 = one_bit_pfx_seg_len(fmt_len, format_pfx);
        assert(seg_len == seg_len2);
    }
#endif
    return SHR_E_NONE;
}

/*!
 * \brief Reverse IP and copy to Trie IP.
 *
 * \param [in] max_bits Max bits
 * \param [in] ip Normal IP.
 * \param [out] trie_ip Trie format IP.
 *
 * \return Nothing.
 */
static inline void
reverse_ip_to_trie_ip(int max_bits, alpm_ip_t ip, trie_ip_t *trie_ip)
{
    int i;
    int key_words = max_bits / 32;

    for (i = 0; i < key_words; i++) {
        trie_ip->key[key_words - i] = ip[i];
    }
    if (max_bits % 32) {
        trie_ip->key[0] = ip[key_words];
    } else {
        ALPM_ASSERT(trie_ip->key[0] == 0);
    }
}

/*!
 * \brief Reverse Trie IP and copy to IP.
 *
 * \param [in] v6 Non-zero if IPv6, otherwise IPv4.
 * \param [in] trie_ip Trie format IP.
 * \param [out] ip Normal IP.
 *
 * \return Nothing.
 */
static void
reverse_trie_ip_to_ip(int max_bits, trie_ip_t *trie_ip, alpm_ip_t ip)
{
    int i;
    int key_words = max_bits / 32;

    for (i = 0; i < key_words; i++) {
        ip[i] = trie_ip->key[key_words - i];
    }
    if (max_bits % 32) {
        ip[key_words] = trie_ip->key[0];
    } else {
        ALPM_ASSERT(trie_ip->key[0] == 0);
    }
}

/*******************************************************************************
 * Public Functions
 */

int
bcmptm_rm_alpm_one_bit_encode(int max_bits,
                              trie_ip_t *trie_ip,
                              int len,
                              int fmt_len,
                              int kshift,
                              uint32_t *format_pfx)
{
    alpm_ip_t ip_addr[2] = {{0}};

    ALPM_ASSERT(max_bits);
    ALPM_ASSERT(len - kshift <= fmt_len);
    reverse_trie_ip_to_ip(max_bits, trie_ip, ip_addr[1]);
    return one_bit_pfx_encode(max_bits, ip_addr[0], len, fmt_len, kshift, format_pfx);
}

int
bcmptm_rm_alpm_one_bit_decode(int max_bits,
                              trie_ip_t *trie_ip,
                              int *len,
                              int fmt_len,
                              int kshift,
                              uint32_t *format_pfx)
{
    int seg_len, src_bit;
    alpm_ip_t src_ip = {0};
    alpm_ip_t dst_ip = {0};

    ALPM_ASSERT(max_bits);
    reverse_trie_ip_to_ip(max_bits, trie_ip, src_ip);

    seg_len = one_bit_pfx_seg_len(fmt_len, format_pfx);
    if (seg_len < 0) {
        return 0;
    }
    src_bit = fmt_len - seg_len;
    SHR_BITCOPY_RANGE(dst_ip, seg_len, src_ip, 0, kshift);
    SHR_BITCOPY_RANGE(dst_ip, 0, format_pfx, src_bit + 1, seg_len);

    *len = (seg_len + kshift);
    reverse_ip_to_trie_ip(max_bits, dst_ip, trie_ip);
    return 1;
}
