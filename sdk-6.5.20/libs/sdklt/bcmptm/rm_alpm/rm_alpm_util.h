/*! \file rm_alpm_util.h
 *
 * Utilities internal to RM ALPM
 *
 * This file contains utilities which are internal to
 * ALPM Resource Manager
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef RM_ALPM_UTIL_H
#define RM_ALPM_UTIL_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include "rm_alpm.h"


/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief String name for key type
 *
 * \param [in] u Device u.
 * \param [in] kt Key type
 *
 * \return String name.
 */
const char *
bcmptm_rm_alpm_kt_name(int u, int m, alpm_key_type_t kt);

/*!
 * \brief String name for vrf type
 *
 * \param [in] u Device u.
 * \param [in] vt VRF type
 *
 * \return String name.
 */
const char *
bcmptm_rm_alpm_vt_name(int u, int m, alpm_vrf_type_t vt);

/*!
 * \brief String name for pkm type
 *
 * \param [in] u Device u.
 * \param [in] pkm PKM type
 *
 * \return String name.
 */
const char *
bcmptm_rm_alpm_pkm_name(int u, int m, int pkm);

/*!
 * \brief Update HW table entry (through WAL) and PTcache
 *
 * \param [in] u Device u.
 * \param [in] sid ALPM symbols
 * \param [in] index Pt index
 * \param [in] entry_words Entry content
 * \param [in] w_size Pt size
 * \param [in] op_type See definition for bcmptm_rm_op_t.
 * \param [in] dt_bitmap Data type bitmap.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_write(int u, int m, bcmdrd_sid_t sid,
                     int index, uint32_t *entry_words,
                     size_t wsize,
                     bcmptm_rm_op_t op_type,
                     uint32_t dt_bitmap);

/*!
 * \brief Read latest table entry
 *
 * \param [in] u Device u.
 * \param [in] sid ALPM symbols
 * \param [in] index Pt index
 * \param [in] w_size Pt size
 * \param [out] entry_words Entry content
 * \param [out] dt_bitmap Data type bitmap.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_read(int u, int m, bcmdrd_sid_t sid,
                    int index, size_t w_size, uint32_t *entry_words,
                    uint32_t *dt_bitmap);


/*!
 * \brief Initialize alpm arg
 *
 * \param [in] u Device u.
 * \param [out] arg ALPM arg
 *
 * \return nothing.
 */
extern void
bcmptm_rm_alpm_arg_init(int u, int m, alpm_arg_t *arg);

/*!
 * \brief Get IP address mask from IP mask len
 *
 * \param [in] u Device u.
 * \param [in] v6 Is IPv6
 * \param [in] len IP mask len
 * \param [out] mask IP address mask
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_len_to_mask(int u, int m, int v6, int len, uint32_t *mask);


/*!
 * \brief Get trie format key from normal IP address
 *
 * ip_addr[3] -> ip_addr[0] : msb -> lsb (containing zeros).
 * key[0]     -> key[4]     : msb -> lsb (no zeros)
 *
 * \param [in] u Device u.
 * \param [in] ipv IP version
 * \param [in] ip_addr IP address
 * \param [in] len IP mask length
 * \param [out] key Trie format key
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_trie_key_create(int u, int m, int ipv, uint32_t *ip_addr, uint32_t len,
                               trie_ip_t *key);


/*!
 * \brief Get normal IP address from trie format key
 *
 * ip_addr[3] -> ip_addr[0] : msb -> lsb (containing zeros).
 * key[0]     -> key[4]     : msb -> lsb (no zeros)
 *
 * \param [in] u Device u.
 * \param [in] ipv IP version
 * \param [in] key Trie format key
 * \param [in] len IP mask length
 * \param [out] ip_addr IP address
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_api_key_create(int u, int m, int ipv, trie_ip_t *key, uint32_t len,
                              alpm_ip_t ip_addr);

/*!
 * \brief Get IP mask length from IP address mask
 *
 * \param [in] u Device u.
 * \param [in] v6 Is IPv6
 * \param [in] mask IP address mask
 * \param [out] len IP mask length
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_mask_to_len(int u, int m, int v6, uint32_t *mask, int *len);

extern int
bcmptm_rm_alpm_trie_mkl(int u, int m, int v6, uint8_t app, alpm_vrf_type_t vt);

extern int
bcmptm_rm_alpm_max_key_bits(int u, int m, int v6, uint8_t app, alpm_vrf_type_t vt);

extern void
bcmptm_rm_alpm_ipaddr_str(char *ipstr, alpm_ip_ver_t ipv, alpm_ip_t ipaddr);

extern const char *
bcmptm_rm_alpm_app_name(int u, int m, alpm_app_type_t app);

#endif /* RM_ALPM_UTIL_H */
