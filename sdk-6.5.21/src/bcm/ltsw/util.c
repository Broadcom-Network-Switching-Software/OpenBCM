/*! \file util.c
 *
 * LTSW utility routines.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>
#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

/*!
 * \brief Convert mac array into uint64_t data.
 *
 * \param [out]  dst          Destination data structure.
 * \param [in]   src          Source data structure.
 */
void
bcmi_ltsw_util_mac_to_uint64(uint64_t *dst, uint8_t *src)
{
    uint64_t mac = 0ULL;
    mac |= ((uint64_t)src[0] & 0xff) << (5 * 8);
    mac |= ((uint64_t)src[1] & 0xff) << (4 * 8);
    mac |= ((uint64_t)src[2] & 0xff) << (3 * 8);
    mac |= ((uint64_t)src[3] & 0xff) << (2 * 8);
    mac |= ((uint64_t)src[4] & 0xff) << (1 * 8);
    mac |= ((uint64_t)src[5] & 0xff) << (0 * 8);

    *dst = mac;
    return;
}

/*!
 * \brief Convert uint64_t data into mac array.
 *
 * \param [out]  dst          Destination data structure.
 * \param [in]   src          Source data structure.
 */
void
bcmi_ltsw_util_uint64_to_mac(uint8_t *dst, uint64_t *src)
{
    uint8_t mac[6];
    uint64_t data = *src;
    mac[0] = (uint8_t)(data >> (5 * 8)) & 0xff;
    mac[1] = (uint8_t)(data >> (4 * 8)) & 0xff;
    mac[2] = (uint8_t)(data >> (3 * 8)) & 0xff;
    mac[3] = (uint8_t)(data >> (2 * 8)) & 0xff;
    mac[4] = (uint8_t)(data >> (1 * 8)) & 0xff;
    mac[5] = (uint8_t)(data >> (0 * 8)) & 0xff;

    sal_memcpy(dst, mac, 6);
    return;
}

/*!
 * \brief Convert uint64_t data into generic uint8 array.
 *
 * \param [out]  dst          Destination data structure.
 * \param [in]   src          Source data structure.
 * \param [in]   width        Width in bits of soutrce field.
 */
void
bcmi_ltsw_util_uint64_to_generic(uint8_t *dst, uint64_t *src, uint32_t width)
{
    uint8_t i = 0, num_bytes = 0;
    uint8_t dword_idx = 0, byte_idx = 0;
    uint8_t *arr = NULL;
    uint64_t *data = src;

    arr = sal_alloc((sizeof(uint8_t) * width), "ltswUtil");

    num_bytes = BITS2BYTES(width);
    byte_idx = (num_bytes - 1);
    for (i = 0; i < num_bytes; i++) {
        dword_idx = (byte_idx / 8);
        arr[i] = (uint8_t)(data[dword_idx] >> ((byte_idx % 8) * 8)) & 0xff;
        byte_idx--;
    }

    sal_memcpy(dst, arr, num_bytes);
    sal_free(arr);
    return;
}

/*!
 * \brief Convert generic uint8 array into uint64_t data.
 *
 * \param [out]  dst          Destination data structure.
 * \param [in]   src          Source data structure.
 * \param [in]   width        Width in bits of source field.
 */
void
bcmi_ltsw_util_generic_to_uint64(uint64_t *dst, uint8_t *src, uint32_t width)
{
    uint8_t i = 0, num_bytes = 0;
    uint8_t dword_idx = 0, byte_idx = 0;
    uint64_t *val64 = dst;

    num_bytes = BITS2BYTES(width);
    byte_idx = (num_bytes - 1);
    for (i = 0; i < num_bytes; i++) {
        dword_idx = (byte_idx / 8);
        val64[dword_idx] |= ((uint64_t)src[i] & 0xff) << ((byte_idx % 8) * 8);
        byte_idx--;
    }
    return;
}

/*!
 * \brief Convert IPv6 address into uint64_t data array.
 *
 * \param [out]  dst          Destination data structure.
 * \param [in]   src          IPv6 address.
 */
void
bcmi_ltsw_util_ip6_to_uint64(uint64_t *dst, bcm_ip6_t *src)
{
    uint32_t low, high;
    int i = 0;
    bcm_ip6_t ip6;

    sal_memcpy(ip6, *src, 16);

    high = 0;
    low = 0;
    high |= ip6[i++] << 24;
    high |= ip6[i++] << 16;
    high |= ip6[i++] <<  8;
    high |= ip6[i++] <<  0;
    low  |= ip6[i++] << 24;
    low  |= ip6[i++] << 16;
    low  |= ip6[i++] <<  8;
    low  |= ip6[i++] <<  0;
    COMPILER_64_SET(dst[1], high, low);

    high = 0;
    low = 0;
    high |= ip6[i++] << 24;
    high |= ip6[i++] << 16;
    high |= ip6[i++] <<  8;
    high |= ip6[i++] <<  0;
    low  |= ip6[i++] << 24;
    low  |= ip6[i++] << 16;
    low  |= ip6[i++] <<  8;
    low  |= ip6[i++] <<  0;
    COMPILER_64_SET(dst[0], high, low);

    return;
}

/*!
 * \brief Convert uint64_t data array into  IPv6 address.
 *
 * \param [out]  ip6          IPv6 address.
 * \param [in]   src          Source data structure.
 */
void
bcmi_ltsw_util_uint64_to_ip6(bcm_ip6_t *dst, uint64_t *src)
{
    uint32_t low, high;
    int i = 0;
    bcm_ip6_t ip6;

    COMPILER_64_TO_32_LO(low, src[1]);
    COMPILER_64_TO_32_HI(high, src[1]);
    ip6[i++] = (uint8_t)(high >> 24);
    ip6[i++] = (uint8_t)(high >> 16);
    ip6[i++] = (uint8_t)(high >>  8);
    ip6[i++] = (uint8_t)(high >>  0);
    ip6[i++] = (uint8_t)(low  >> 24);
    ip6[i++] = (uint8_t)(low  >> 16);
    ip6[i++] = (uint8_t)(low  >>  8);
    ip6[i++] = (uint8_t)(low  >>  0);

    COMPILER_64_TO_32_LO(low, src[0]);
    COMPILER_64_TO_32_HI(high, src[0]);
    ip6[i++] = (uint8_t)(high >> 24);
    ip6[i++] = (uint8_t)(high >> 16);
    ip6[i++] = (uint8_t)(high >>  8);
    ip6[i++] = (uint8_t)(high >>  0);
    ip6[i++] = (uint8_t)(low  >> 24);
    ip6[i++] = (uint8_t)(low  >> 16);
    ip6[i++] = (uint8_t)(low  >>  8);
    ip6[i++] = (uint8_t)(low  >>  0);

    sal_memcpy(*dst, ip6, 16);

    return;
}

/*!
 * \brief Convert half of IPv6 address into uint64_t data.
 *
 * \param [out]  dst          Destination data structure.
 * \param [in]   src          IPv6 address.
 * \param [in]   is_lower     Lower or upper half of IPv6 address.
 */
void
bcmi_ltsw_util_ip6_half_to_uint64(uint64_t *dst, bcm_ip6_t *src, int is_lower)
{
    uint32_t low, high;
    int i = 0;
    bcm_ip6_t ip6;

    if (is_lower) {
        i = 8;
    } else {
        i = 0;
    }

    sal_memcpy(ip6, *src, 16);

    high = 0;
    low = 0;
    high |= ip6[i++] << 24;
    high |= ip6[i++] << 16;
    high |= ip6[i++] <<  8;
    high |= ip6[i++] <<  0;
    low  |= ip6[i++] << 24;
    low  |= ip6[i++] << 16;
    low  |= ip6[i++] <<  8;
    low  |= ip6[i++] <<  0;
    COMPILER_64_SET(*dst, high, low);

    return;
}

/*!
 * \brief Convert uint64_t data into half of IPv6 address.
 *
 * \param [out]  dst          IPv6 address.
 * \param [in]   src          Source data structure.
 * \param [in]   is_lower     Lower or upper half of IPv6 address.
 */
void
bcmi_ltsw_util_uint64_to_ip6_half(bcm_ip6_t *dst, uint64_t *src, int is_lower)
{
    uint32_t low, high;
    int i = 0, offset = 0;
    bcm_ip6_t ip6;

    if (is_lower) {
        offset = 8;
    } else {
        offset = 0;
    }

    i = offset;
    COMPILER_64_TO_32_LO(low, *src);
    COMPILER_64_TO_32_HI(high, *src);
    ip6[i++] = (uint8_t)(high >> 24);
    ip6[i++] = (uint8_t)(high >> 16);
    ip6[i++] = (uint8_t)(high >>  8);
    ip6[i++] = (uint8_t)(high >>  0);
    ip6[i++] = (uint8_t)(low  >> 24);
    ip6[i++] = (uint8_t)(low  >> 16);
    ip6[i++] = (uint8_t)(low  >>  8);
    ip6[i++] = (uint8_t)(low  >>  0);

    sal_memcpy(*dst + offset, ip6 + offset, 8);

    return;
}

void
bcmi_ltsw_util_ip6_to_str(char *buf, bcm_ip6_t ipaddr)
{
    sal_sprintf(buf, "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x",
            (((uint16)ipaddr[0] << 8) | ipaddr[1]),
            (((uint16)ipaddr[2] << 8) | ipaddr[3]),
            (((uint16)ipaddr[4] << 8) | ipaddr[5]),
            (((uint16)ipaddr[6] << 8) | ipaddr[7]),
            (((uint16)ipaddr[8] << 8) | ipaddr[9]),
            (((uint16)ipaddr[10] << 8) | ipaddr[11]),
            (((uint16)ipaddr[12] << 8) | ipaddr[13]),
            (((uint16)ipaddr[14] << 8) | ipaddr[15]));
    return;
}
