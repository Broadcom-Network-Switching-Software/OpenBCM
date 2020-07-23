/*! \file bcmptm_rm_alpm_common.h
 *
 * <description>
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_RM_ALPM_COMMON_H
#define BCMPTM_RM_ALPM_COMMON_H

/*******************************************************************************
  Includes
 */
#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_rm_alpm_internal.h>

/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief One bit format encoding
 *
 * \param [in] max_bits Max bits
 * \param [in] trie_ip IP address
 * \param [in] len IP address full length
 * \param [in] fmt_len Format length
 * \param [in] kshift Key shift
 * \param [out] format_pfx One bit prefix format
 *
 * \return SHR_E_NONE
 */
extern int
bcmptm_rm_alpm_one_bit_encode(int max_bits,
                              trie_ip_t *trie_ip,
                              int len,
                              int fmt_len,
                              int kshift,
                              uint32_t *format_pfx);

/*!
 * \brief One bit format decoding
 *
 * \param [in] max_bits Max bits
 * \param [in] trie_ip IP address, in as prev level trie_ip, out as trie_ip.s
 * \param [out] len IP address full length
 * \param [in] fmt_len Format length
 * \param [in] kshift Key shift
 * \param [in] format_pfx One bit prefix format
 *
 * \return valid
 */
extern int
bcmptm_rm_alpm_one_bit_decode(int max_bits,
                              trie_ip_t *trie_ip,
                              int *len,
                              int fmt_len,
                              int kshift,
                              uint32_t *format_pfx);

#endif /* BCMPTM_RM_ALPM_COMMON_H */
