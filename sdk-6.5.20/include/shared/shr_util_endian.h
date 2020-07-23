/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Utility functions for endianness-dependent code.
 */

#ifndef _SHR_UTIL_ENDIAN_H_
#define _SHR_UTIL_ENDIAN_H_

/*!
 * \brief Check if host is in big endian.
 *
 * \retval 0  Host is in little endian
 * \retval 1  Host is in big endian
 */
int
shr_util_host_is_be(void);

/*!
 * \brief Convert a 32-bit word from host endian to big endian.
 *
 * \param [in] _dptr Pointer to destination word.
 * \param [in] _sptr Pointer to source word.
 *
 * \return Nothing.
 */
#define SHR_UTIL_HOST_WORD_TO_BE32(_dptr, _sptr) \
    do {                                         \
        uint8 *_dst = (uint8 *)(_dptr);      \
        uint32 _d32 = *((uint32 *)(_sptr));  \
        *_dst++ = (_d32 >> 24) & 0xff;           \
        *_dst++ = (_d32 >> 16) & 0xff;           \
        *_dst++ = (_d32 >> 8) & 0xff;            \
        *_dst++ = _d32 & 0xff;                   \
    } while (0)

/*!
 * \brief Convert a 32-bit word from host endian to little endian.
 *
 * \param [in] _dptr Pointer to destination word.
 * \param [in] _sptr Pointer to source word.
 *
 * \return Nothing.
 */
#define SHR_UTIL_HOST_WORD_TO_LE32(_dptr, _sptr) \
    do {                                         \
        uint8 *_dst = (uint8 *)(_dptr);      \
        uint32 _d32 = *((uint32 *)(_sptr));  \
        *_dst++ = _d32 & 0xff;                   \
        *_dst++ = (_d32 >> 8) & 0xff;            \
        *_dst++ = (_d32 >> 16) & 0xff;           \
        *_dst++ = (_d32 >> 24) & 0xff;           \
    } while (0)

/*!
 * \brief Copy buffer from host endian to big endian.
 *
 * When device DMA requires endianess handling in software, we
 * typically need to copy from host endian to a fixed endian
 * (determined by the device).
 *
 * This function copies a 32-bit-aligned buffer from host endian
 * layout to big endian layout and vice versa.
 *
 * \param [in] dst 32-bit-aligned destination buffer.
 * \param [in] src 32-bit-aligned source buffer.
 * \param [in] wlen Buffer length in 32-bit words.
 *
 * \retval 0 No errors.
 */
int
shr_util_host_to_be32(uint32 *dst, uint32 *src, size_t wlen);

/*!
 * \brief Copy buffer from host endian to little endian.
 *
 * When device DMA requires endianess handling in software, we
 * typically need to copy from host endian to a fixed endian
 * (determined by the device).
 *
 * This function copies a 32-bit-aligned buffer from host endian
 * layout to little endian layout and vice versa.
 *
 * \param [in] dst 32-bit-aligned destination buffer.
 * \param [in] src 32-bit-aligned source buffer.
 * \param [in] wlen Buffer length in 32-bit words.
 *
 * \retval 0 No errors.
 */
int
shr_util_host_to_le32(uint32 *dst, uint32 *src, size_t wlen);

#endif /* _SHR_UTIL_ENDIAN_H_ */
