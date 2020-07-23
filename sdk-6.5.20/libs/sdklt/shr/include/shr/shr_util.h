/*! \file shr_util.h
 *
 * Common utility routines.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SHR_UTIL_H
#define SHR_UTIL_H

#include <shr/shr_types.h>

/*!
 * \brief Find the minimum of _val1 and _val2
 *
 * \param [in] _val1 First value to be compared
 * \param [in] _val2 Second value to be compared
 *
 * \return Minimum value between the input parameters
 */
#define SHR_MIN(_val1, _val2)   ((_val1) < (_val2) ? (_val1) : (_val2))


/*!
 * \brief Convert a count of bytes to a number of uint32_t words
 *
 * \param [in] \_bytes\_ count of bytes
 */
#define SHR_BYTES2WORDS(_bytes_)   (((_bytes_) + 3) / 4)


/*!
 * \brief Convert a count of words to a number of bytes
 *
 * \param [in] \_words\_ count of words
 */
#define SHR_WORDS2BYTES(_words_)   ((_words_) * 4)


/*!
 * \brief Format a uint32_t as a string of ASCII digits.
 *
 * Prepare a formatted string of ASCII digits to express a uint32_t
 * quantity for use in BSL output.
 *
 * \param [in,out] buf Character arry to hold output string
 * \param [in] n Input uint32_t value to render as a string
 * \param [in] min_digits Minimum ASCII digits to use as string expression
 * \param [in] base Numeric base in which to express output
 *
 * Note - this function is used in the implementation of
 * shr_format_long_integer
 */
extern void
shr_util_format_uint32(char *buf, uint32_t n, int min_digits, int base);

/*!
 * \brief Format an arbitrary precision long integer.
 *
 * Prepare a formatted string of ASCII digits to express an
 * arbitrary-length integer encoded within a uint32_t array for use
 * in BSL output.
 *
 * If the value is less than 10, prints one decimal digit;
 * otherwise output is in hex format with 0x prefix.
 *
 * \param [in,out] buf Character arry to hold output string
 * \param [in] val Input uint32_t array holding the number to render
 *                 val[0] is the least significant word.
 * \param [in] nval the number of bytes in the value.
 *
 * Note - this function uses shr_format_long_integer
 */
extern void
shr_util_format_uint32_array(char *buf, uint32_t *val, int nval);

/*!
 * \brief Format an byte array for hexadecimal ASCII output.
 *
 * Output is in hex format with 0x prefix, null terminated.
 *
 * \param [out] buf String buffer for output
 * \param [in] val  Pointer to byte array holding the value
 *             val[0] is the least significant byte.
 * \param [in] nval is the number of bytes in the value.
 */
extern void
shr_util_format_uint8_array(char *buf, const uint8_t *val, int nval);

/*!
 * \brief Return the number of bits set in a uint32_t.
 *
 * \param [in] n A unit32_t value to check the set bits.
 *
 * \return The number of bits set in \c n.
 */
extern int
shr_util_popcount(uint32_t n);

/*!
 * \brief Find the first set bit.
 *
 * This function will return the position of the first set bit in a uint32_t
 * value. The position starts from 0.
 *
 * \param [in] n A uint32_t value to check the set bit.
 *
 * \return The the position of the first set bit or -1 when \c n is 0.
 */
extern int
shr_util_ffs(uint32_t n);

/*!
 * \brief Logarithm to the base of 2.
 *
 * This function will return the integer value of log2(n).
 *
 * The original purpose of this function is to calculate the number of
 * bits it takes to hold a given value, and for that purpose the
 * function is defined such that shr_util_log2(0) = 0 (instead of
 * undefined).
 *
 * The number of bits to hold a value is then calculated as:
 *
 *   num_bits(n) = shr_util_log2(n) + 1
 *
 * \param [in] n A uint32_t value for which to calculate log2.
 *
 * \return log2(n).
 */
extern int
shr_util_log2(uint32_t n);

/*!
 * \brief Convert ASCII hex character to integer.
 *
 * This function will convert an ASCII hexadecimal character to an
 * integer value, for example '3' will be converted to 3 and 'b' will
 * be converted to 11.
 *
 * \param [in] ch ASCII hexadecimal character.
 *
 * \return Integer value of input character or -1 if invalid input.
 */
extern int
shr_util_xch2i(int ch);

/*!
 * \brief  Calculate IP style checksum
 *
 * This function will calculate IP style checksum
 *
 * \param [in] length length of data in bytes
 * \param [in] data   pointer to data
 */
extern unsigned short
shr_ip_chksum(unsigned int length, unsigned char *data);

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
        uint8_t *_dst = (uint8_t *)(_dptr);      \
        uint32_t _d32 = *((uint32_t *)(_sptr));  \
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
        uint8_t *_dst = (uint8_t *)(_dptr);      \
        uint32_t _d32 = *((uint32_t *)(_sptr));  \
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
shr_util_host_to_be32(uint32_t *dst, uint32_t *src, size_t wlen);

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
shr_util_host_to_le32(uint32_t *dst, uint32_t *src, size_t wlen);

/*!
 * \brief Implementation of realloc on top of sal_alloc API.
 *
 * Grow or shrink a block of memory while preserving the contents.
 *
 * If \c old_ptr is NULL, the call is equivalent to sal_alloc.
 *
 * If the function returns NULL, then the existing block of memory is
 * left unchanged.
 *
 * \param [in] old_ptr Pointer to current block of memory (or NULL).
 * \param [in] old_size Size of current block of memory.
 * \param [in] size Requested new size of memory block.
 * \param [in] str ID string for \c sal_alloc.
 *
 * \return Pointer to new block of memory or NULL on error.
 */
extern void *
shr_util_realloc(void *old_ptr, size_t old_size, size_t size, char *str);

#ifdef htonl
/*!
 * \brief Convert 32-bit value from host-endian to big endian.
 */
#define shr_htonl htonl
#else
/*!
 * \brief Convert 32-bit value from host-endian to big endian.
 *
 * \param [in] src Value to be converted
 *
 * \return Value with big endian (network byte order).
 */
static inline uint32_t shr_htonl(uint32_t src)
{
    uint32_t dst = 0;
    uint8_t *ptr = (uint8_t *)&dst;
    ptr[0] = (src >> 24) & 0xff;
    ptr[1] = (src >> 16) & 0xff;
    ptr[2] = (src >> 8) & 0xff;
    ptr[3] = (src >> 0) & 0xff;
    return dst;
}
#endif

#ifdef htons
/*!
 * \brief Convert 16-bit value from host-endian to big endian.
 */
#define shr_htons htons
#else
/*!
 * \brief Convert 16-bit value from host-endian to big endian.
 *
 * \param [in] src Value to be converted
 *
 * \return Value with big endian (network byte order).
 */
static inline uint16_t shr_htons(uint16_t src)
{
    uint16_t dst = 0;
    uint8_t *ptr = (uint8_t *)&dst;
    ptr[0] = (src >> 8) & 0xff;
    ptr[1] = (src >> 0) & 0xff;
    return dst;
}
#endif
/*!
 * \brief Convert 32-bit value from big endian to host-endian.
 */
#define shr_ntohl shr_htonl
/*!
 * \brief Convert 16-bit value from big endian to host-endian.
 */
#define shr_ntohs shr_htons

/*!
 * \brief Convert uint64 values into uint32 array.
 *
 *
 * \param [in] field_val64 uint64_t data value.
 * \param [out] field_val uint32_t array large enough to hold the data value.
 *
 * \retval None.
 */
static inline void
shr_uint64_to_uint32_array(uint64_t field_val64, uint32_t *field_val)
{
    field_val[0] = field_val64 & 0xffffffff;
    field_val[1] = (field_val64 >> 32) & 0xffffffff;
}

/*!
 * \brief Convert a uint32_t array into a uint64_t value.
 *
 * \param [in] field_val uint32_t array large enough to hold the data value.
 * \param [out] field_val64 uint64_t data value.
 *
 * \retval None.
 */
static inline void
shr_uint32_array_to_uint64(uint32_t *field_val, uint64_t *field_val64)
{
    *field_val64 = (((uint64_t)field_val[1]) << 32) | field_val[0];
}

#endif /* SHR_UTIL_H */
