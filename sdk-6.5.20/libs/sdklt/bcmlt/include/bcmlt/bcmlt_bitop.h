/*! \file bcmlt_bitop.h
 *  \brief BCMLT Bit Array Macros Header File
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef   BCMLT_BITOP_H
#define   BCMLT_BITOP_H

#include <bcmltd/bcmltd_bitop.h>

/*!
 * Base type for bit arrays. This type is essentially a block of bits,
 * and each block contains \ref BCMLT_BITWID bits.
 */
#define BCMLT_BITDCL     BCMLTD_BITDCL

/*!
 * Number of bits in a bit array base type (\ref BCMLT_BITDCL). An
 * applcation should normally not care about the size of the bit
 * array base type.
 */
#define BCMLT_BITWID     BCMLTD_BITWID

/*!
 * Size in bytes of a bit array containing _max bits.
 *
 * This size is useful when calling malloc, memcpy, etc.
 */
#define BCMLT_BIT_ALLOCSIZE(_max)  \
    BCMLTD_BIT_ALLOCSIZE(_max)

/*!
 * Declare bit array _name of size _max bits.
 *
 * Used for declaring a bit array on the stack (or as a static).
 *
 * For example, to declare an array containing 80 bits:
 *
 * \code{.c}
 * void my_func(void)
 * {
 *     BCMLT_BIT_DCLNAME(my_bit_array, 80);
 *     ...
 * }
 * \endcode
 */
#define BCMLT_BIT_DCLNAME(_name, _max)  \
    BCMLTD_BIT_DCLNAME(_name, _max)

/*!
 * \brief Test if a bit is set in a bit array.
 *
 * \param [in] _a Bit array to operate on
 * \param [in] _b Bit number to test (first is 0)
 *
 * \return 0 if not set, otherwise non-zero.
 */
#define BCMLT_BIT_GET(_a, _b) \
    BCMLTD_BIT_GET(_a, _b)

/*!
 * \brief Set a bit in a bit array.
 *
 * \param [in] _a Bit array to operate on
 * \param [in] _b Bit number to set (first is 0)
 *
 * \return Nothing
 */
#define BCMLT_BIT_SET(_a, _b) \
    BCMLTD_BIT_SET(_a, _b)

/*!
 * \brief Clear a bit in a bit array.
 *
 * \param [in] _a Bit array to operate on
 * \param [in] _b Bit number to clear (first is 0)
 *
 * \return Nothing
 */
#define BCMLT_BIT_CLR(_a, _b) \
    BCMLTD_BIT_CLR(_a, _b)

/*!
 * \brief Iterate over bits in bit array.
 *
 * Used to perform a specific action for all the bits which are set in
 * a bit array.
 *
 * Example:
 *
 * \code{.c}
 * #define MAX_BITS 80
 *
 * static BCMLT_BITDCLNAME(my_bit_array, MAX_BITS);
 *
 * void my_biterator(void)
 * {
 *  int bitnum;
 *
 *  BCMLT_BIT_ITER(my_bit_array, MAX_BITS, bitnum) {
 *  printf("Bit number %d is set\n", bitnum);
 *  }
 * }
 * \endcode
 *
 * \param [in] _a Bit array to iterate over
 * \param [in] _max Number of bits to iterate over (starting from 0)
 * \param [in] _b Iterator variable (should be same type as _max)
 */
#define BCMLT_BIT_ITER(_a, _max, _b)               \
    BCMLTD_BIT_ITER(_a, _max, _b)

/*!
 * \brief Set range of bits in a bit array.
 *
 * This macro sets a specified range of bits within a bit array.
 *
 * \param [in] _a Bit array in which to set bits
 * \param [in] _offs Offset (in bits) into the array
 * \param [in] _n Number of bits to set
 *
 * \return Nothing
 */
#define BCMLT_BIT_SET_RANGE(_a, _offs, _n) \
    BCMLTD_BIT_SET_RANGE(_a, _offs, _n)

/*!
 * \brief Clear range of bits in a bit array.
 *
 * This macro clears a specified range of bits within a bit array.
 *
 * \param [in] _a Bit array in which to clear bits
 * \param [in] _offs Offset (in bits) into the array
 * \param [in] _n Number of bits to clear
 *
 * \return Nothing
 */
#define BCMLT_BIT_CLR_RANGE(_a, _offs, _n) \
    BCMLTD_BIT_CLR_RANGE(_a, _offs, _n)

/*!
 * \brief Copy a range of bits between two bit arrays.
 *
 * This macro copies a range of bits from one bit array to
 * another. The bit range may reside at different offsets within the
 * source and destination bit arrays.
 *
 * If source and destination are within the same bit array, then the
 * ranges must not overlap.
 *
 * \param [in] _a1 Destination bit array
 * \param [in] _offs1 Offset (in bits) in destination bit array
 * \param [in] _a2 Source bit array
 * \param [in] _offs2 Offset (in bits) in source bit array
 * \param [in] _n Number of bits to copy
 *
 * \return Nothing
 */
#define BCMLT_BIT_COPY_RANGE(_a1, _offs1, _a2, _offs2, _n)  \
    BCMLTD_BIT_COPY_RANGE(_a1, _offs1, _a2, _offs2, _n)

/*!
 * \brief Perform bitwise AND for a range of bits in two bit arrays.
 *
 * The destination bit array may the same as one of the source bit
 * arrays.
 *
 * \param [in] _a1 First source bit array
 * \param [in] _a2 Second source bit array
 * \param [in] _offs Offset (in bits) in bit array
 * \param [in] _n Number of bits to operate on
 * \param [in] _dest Destination bit array
 *
 * \return Nothing
 */
#define BCMLT_BIT_AND_RANGE(_a1, _a2, _offs, _n, _dest) \
    BCMLTD_BIT_AND_RANGE(_a1, _a2, _offs, _n, _dest)

/*!
 * \brief Perform bitwise OR for a range of bits in two bit arrays.
 *
 * This macro performs a logical AND operation between each bit of a
 *  specified range of bits within two bit arrays.
 *
 * The destination bit array may the same as one of the source bit
 *  arrays.
 *
 * \param [in] _a1 First bit array for operation
 * \param [in] _a2 Second bit array for operation
 * \param [in] _offs Offset (in bits) into the arrays
 * \param [in] _n Number of bits to operate on
 * \param [in] _dest Destination bit array
 *
 * \return Nothing
 */
#define BCMLT_BIT_OR_RANGE(_a1, _a2, _offs, _n, _dest) \
    BCMLTD_BIT_OR_RANGE(_a1, _a2, _offs, _n, _dest)

/*!
 * \brief Perform bitwise XOR operation on bit arrays.
 *
 * This macro performs a logical XOR operation between each bit of a
 * specified range of bits within two bit arrays.
 *
 * The destination bit array may the same as one of the source bit
 * arrays.
 *
 * \param [in] _a1 First bit array for operation
 * \param [in] _a2 Second bit array for operation
 * \param [in] _offs Offset (in bits) into the arrays
 * \param [in] _n Number of bits to operate on
 * \param [in] _dest Destination bit array
 *
 * \return Nothing
 */
#define BCMLT_BIT_XOR_RANGE(_a1, _a2, _offs, _n, _dest) \
    BCMLTD_BIT_XOR_RANGE(_a1, _a2, _offs, _n, _dest)

/*!
 * \brief Clear select bits in a bit array.
 *
 * This macro clears bits within a specified range of bits in a bit
 * array. Only the bits which are set in the supplied mask bit array
 * will be cleared, i.e. any bit not set in the mask (or outside the
 * range) are unaffected.
 *
 * The destination bit array may the same as the source bit array.
 *
 * \param [in] _a1 Source bit array
 * \param [in] _a2 Mask bit array
 * \param [in] _offs Offset (in bits) into the arrays
 * \param [in] _n Number of bits to operate on
 * \param [in] _dest Destination bit array
 *
 * \return Nothing
 */
#define BCMLT_BIT_REMOVE_RANGE(_a1, _a2, _offs, _n, _dest) \
    BCMLTD_BIT_REMOVE_RANGE(_a1, _a2, _offs, _n, _dest)

/*!
 * \brief Negate a range of bits in a bit array.
 *
 * This operation is essentially the same as performing an XOR with a
 * array consisting of all 1s.
 *
 * \param [in] _a Source bit array
 * \param [in] _offs Offset (in bits) into the array
 * \param [in] _n Number of bits to negate
 * \param [in] _dest Destination bit array
 */
#define BCMLT_BIT_NEGATE_RANGE(_a, _offs, _n, _dest) \
    BCMLTD_BIT_NEGATE_RANGE(_a, _offs, _n, _dest)

/*!
 * \brief Test if bits are cleared in a bit array.
 *
 *  <long-description>
 *
 * \param [in] _a Source bit array
 * \param [in] _offs Offset (in bits) into the array
 * \param [in] _n Number of bits to negate
 *
 * \return 1 if all bits in range are cleared, otherwise 0.
 */
#define BCMLT_BIT_NULL_RANGE(_a, _offs, _n) \
    BCMLTD_BIT_NULL_RANGE(_a, _offs, _n)

/*!
 * \brief Test if two bit array bit ranges are equal.
 *
 * This macro compares a range of bits at the same offset within two
 * bit arrays.
 *
 * \param [in] _a1 First bit array
 * \param [in] _a2 Second bit array
 * \param [in] _offs Offset (in bits) into the arrays
 * \param [in] _n Number of bits to compare
 *
 * \return 1 if all bits in range are equal, otherwise 0.
 */
#define BCMLT_BIT_EQ_RANGE(_a1, _a2, _offs, _n) \
    BCMLTD_BIT_EQ_RANGE(_a1, _a2, _offs, _n)

/*!
 * \brief Count the number of bits set in a range of bits in a bit array.
 *
 * This macro counts the number of bits that are set in a range of \c
 * _n bits starting at offset \c _offs within bit array \c _a.
 *
 * \param [in] _bits
 * \param [in] _count
 * \param [in] _first
 * \param [in] _range
 */
#define BCMLT_BIT_COUNT_RANGE(_bits, _count, _first, _range) \
    BCMLTD_BIT_COUNT_RANGE(_bits, _count, _first, _range)

/*!
 * \brief Decode a string in hex format into a bitmap.
 *
 * The string can be more than 32 bits worth of data if it is in hex
 * format (0x...).  If not hex, it is treated as a 32 bit value.
 *
 * \param [in] _str
 * \param [in] _a
 * \param [in] _max
 *
 * \retval 0 No errors
 */
#define BCMLT_BIT_STR_DECODE(_str, _a, _max) \
    BCMLTD_BIT_STR_DECODE(_str, _a, _max)

#endif /* BCMLT_BITOP_H */
