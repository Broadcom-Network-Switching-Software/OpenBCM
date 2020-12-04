/*! \file util.h
 *
 * LTSW utility routines header file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _LTSW_UTIL_H
#define _LTSW_UTIL_H

#include <sal/sal_types.h>
#include <bcm/types.h>

/*!
 * \brief Convert mac array into uint64_t data.
 *
 * \param [out]  dst          Destination data structure.
 * \param [in]   src          Source data structure.
 */
extern void bcmi_ltsw_util_mac_to_uint64(uint64_t *dst,
                                         uint8_t *src);

/*!
 * \brief Convert uint64_t data into mac array.
 *
 * \param [out]  dst          Destination data structure.
 * \param [in]   src          Source data structure.
 */
extern void bcmi_ltsw_util_uint64_to_mac(uint8_t *dst,
                                         uint64_t *src);

/*!
 * \brief Convert IPv6 address into uint64_t data array.
 *
 * \param [out]  dst          Destination data structure.
 * \param [in]   ip6          IPv6 address.
 */
extern void
bcmi_ltsw_util_ip6_to_uint64(uint64_t *dst, bcm_ip6_t *ip6);

/*!
 * \brief Convert uint64_t data array into  IPv6 address.
 *
 * \param [out]  ip6          IPv6 address.
 * \param [in]   src          Source data structure.
 */
extern void
bcmi_ltsw_util_uint64_to_ip6(bcm_ip6_t *ip6, uint64_t *src);

/*!
 * \brief Convert half of IPv6 address into uint64_t data.
 *
 * \param [out]  dst          Destination data structure.
 * \param [in]   ip6          IPv6 address.
 * \param [in]   is_lower     Lower or upper half of IPv6 address.
 */
extern void
bcmi_ltsw_util_ip6_half_to_uint64(uint64_t *dst, bcm_ip6_t *ip6_addr,
                                  int is_lower);

/*!
 * \brief Convert uint64_t data into half of IPv6 address.
 *
 * \param [out]  ip6          IPv6 address.
 * \param [in]   src          Source data structure.
 * \param [in]   is_lower     Lower or upper half of IPv6 address.
 */
extern void
bcmi_ltsw_util_uint64_to_ip6_half(bcm_ip6_t *ip6_addr, uint64_t *src,
                                  int is_lower);

/*!
 * \brief Convert IPv6 address into a string.
 *
 * \param [out]  buf          Buffer holding the string.
 * \param [in]   ipaddr       IPv6 address.
 */
extern void
bcmi_ltsw_util_ip6_to_str(char *buf, bcm_ip6_t ipaddr);

/*!
 * \brief Convert uint64_t data into generic uint8 array.
 *
 * \param [out]  dst          Destination data structure.
 * \param [in]   src          Source data structure.
 * \param [in]   width        Width in bits of soutrce field.
 */
extern void
bcmi_ltsw_util_uint64_to_generic(uint8_t *dst,
                                 uint64_t *src,
                                 uint32_t width);
/*!
 * \brief Convert generic uint8 array into uint64_t data.
 *
 * \param [out]  dst          Destination data structure.
 * \param [in]   src          Source data structure.
 * \param [in]   width        Width in bits of source field.
 */
extern void
bcmi_ltsw_util_generic_to_uint64(uint64_t *dst,
                                 uint8_t *src,
                                 uint32_t width);
#endif /* _LTSW_UTIL_H */
