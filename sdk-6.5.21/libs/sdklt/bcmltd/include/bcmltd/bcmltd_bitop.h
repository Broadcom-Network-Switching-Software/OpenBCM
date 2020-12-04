/*! \file bcmltd_bitop.h
 *
 * BCMLTD Bit Array Macros header file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTD_BITOP_H
#define BCMLTD_BITOP_H

#include <sal/sal_libc.h>

/*!
 * Base type for bit arrays. This type is essentially a block of bits,
 * and each block contains \ref BCMLTD_BITWID bits.
 */
#define BCMLTD_BITDCL      uint64_t

/*!
 * Number of bits in a bit array base type (\ref BCMLTD_BITDCL). An
 * application should normally not care about the size of the bit
 * array base type.
 */
#define BCMLTD_BITWID      64

/*! \cond */
/*
 * --------------------------------------------------------------------------
 *
 * The following functions and macros should not be called directly.
 */

/*!
 * Number of BCMLTD_BITDCLs needed to contain _max bits, i.e. the \ref
 * BCMLTD_BITWID-aligned number of bits needed to support _max bits.
 */
#define BCMLTDi_BITDCLSIZE(_max) \
    (((_max) + BCMLTD_BITWID - 1) / BCMLTD_BITWID)

/*!
 * Generic operation macro on bit array _a, with bit _b. Used for
 * public set/clear/test macros.
 */
#define BCMLTDi_BITOP(_a, _b, _op)    \
        (((_a)[(_b) / BCMLTD_BITWID]) _op (1ULL << ((_b) % BCMLTD_BITWID)))

/*!
 * Same as BCMLTDi_BITOP_RANGE, but for a single BCMLTD_BITDCL.
 */
#define BCMLTDi_BITOP_RANGE_ONE_BITDCL(_a1,_a2, _offs, _n, _dest, _op)     \
{                                                                          \
    BCMLTD_BITDCL _mask = ~0;                                              \
    BCMLTD_BITDCL _data;                                                   \
                                                                           \
    _mask >>= (BCMLTD_BITWID - (_n));                                      \
    _mask <<=_offs;                                                        \
    _data = ((_a1) _op (_a2)) & _mask;                                     \
    *(_dest) &= ~_mask;                                                    \
    *(_dest) |= _data;                                                     \
}

/*!
 * \brief Perform bitwise logical operation on bit arrays.
 *
 * This macro allows code sharing between different types of bitwise
 * logical operations between two bit arrays.
 *
 * \param [in] _a1 First bit array for operation
 * \param [in] _a2 Second bit array for operation
 * \param [in] _offs Offset (in bits) into the arrays
 * \param [in] _n Number of bits to operate on
 * \param [in] _dest Destination bit array
 * \param [in] _op Type operation (applied in between BITDCLs)
 *
 * \return Nothing
 */
#define BCMLTDi_BITOP_RANGE(_a1, _a2, _offs, _n, _dest, _op)           \
{                                                                      \
    const BCMLTD_BITDCL *_ptr_a1;                                      \
    const BCMLTD_BITDCL *_ptr_a2;                                      \
    BCMLTD_BITDCL *_ptr_dest;                                          \
    int _woffs, _wremain;                                              \
                                                                       \
    _ptr_a1 = (_a1) + ((_offs) / BCMLTD_BITWID);                       \
    _ptr_a2 = (_a2) + ((_offs) / BCMLTD_BITWID);                       \
    _ptr_dest = (_dest) + ((_offs) / BCMLTD_BITWID);                   \
    _woffs = ((_offs) % BCMLTD_BITWID);                                \
                                                                       \
    _wremain = BCMLTD_BITWID - _woffs;                                 \
    if ((_n) <= _wremain) {                                            \
        BCMLTDi_BITOP_RANGE_ONE_BITDCL(*_ptr_a1, *_ptr_a2,             \
                _woffs, (_n),                                          \
                _ptr_dest, _op);                                       \
        return;                                                        \
    }                                                                  \
    BCMLTDi_BITOP_RANGE_ONE_BITDCL(*_ptr_a1, *_ptr_a2,                 \
            _woffs, _wremain,                                          \
            _ptr_dest, _op);                                           \
    (_n) -= _wremain;                                                  \
    ++_ptr_a1; ++_ptr_a2; ++_ptr_dest;                                 \
    while ((_n) >= BCMLTD_BITWID) {                                    \
        *_ptr_dest =                                                   \
        (*_ptr_a1) _op (*_ptr_a2);                                     \
        (_n) -= BCMLTD_BITWID;                                         \
        ++_ptr_a1; ++_ptr_a2; ++_ptr_dest;                             \
    }                                                                  \
    if ((_n) > 0) {                                                    \
        BCMLTDi_BITOP_RANGE_ONE_BITDCL(*_ptr_a1, *_ptr_a2,             \
                0, (_n),                                               \
                _ptr_dest, _op);                                       \
    }                                                                  \
}

extern void
bcmltd_bitop_range_clear(BCMLTD_BITDCL *a, int offs, int n);

extern void
bcmltd_bitop_range_set(BCMLTD_BITDCL *a, int offs, int n);

extern void
bcmltd_bitop_range_copy(BCMLTD_BITDCL *a1, int offs1, const BCMLTD_BITDCL *a2,
                     int offs2, int n);

extern int
bcmltd_bitop_range_null(const BCMLTD_BITDCL *a,
                     int offs, int n);

extern int
bcmltd_bitop_range_eq(const BCMLTD_BITDCL *a1, const BCMLTD_BITDCL *a2,
                   int offs, int n);

extern void
bcmltd_bitop_range_count(const BCMLTD_BITDCL *a,
                      int offs, int n, int *count);

extern void
bcmltd_bitop_range_and(const BCMLTD_BITDCL *a1, const BCMLTD_BITDCL *a2,
                    int offs, int n, BCMLTD_BITDCL *dest);

extern void
bcmltd_bitop_range_or(const BCMLTD_BITDCL *a1, const BCMLTD_BITDCL *a2,
                   int offs, int n, BCMLTD_BITDCL *dest);

extern void
bcmltd_bitop_range_xor(const BCMLTD_BITDCL *a1, const BCMLTD_BITDCL *a2,
                    int offs, int n, BCMLTD_BITDCL *dest);

extern void
bcmltd_bitop_range_remove(const BCMLTD_BITDCL *a1, const BCMLTD_BITDCL *a2,
                       int offs, int n, BCMLTD_BITDCL *dest);

extern void
bcmltd_bitop_range_negate(const BCMLTD_BITDCL *a1,
                       int offs, int n, BCMLTD_BITDCL *dest);

extern int
bcmltd_bitop_str_decode(const char *str, BCMLTD_BITDCL *dest, int max_bitdcl);

/*
 * End of internal functions and macros.
 *--------------------------------------------------------------------------
 */
/*! \endcond */

/*!
 * Size in bytes of a bit array containing _max bits.
 *
 * This size is useful when calling malloc, memcpy, etc.
 */
#define BCMLTD_BIT_ALLOCSIZE(_max) \
    (BCMLTDi_BITDCLSIZE(_max) * sizeof (BCMLTD_BITDCL))

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
 *     BCMLTD_BIT_DCLNAME(my_bit_array, 80);
 *     ...
 * }
 * \endcode
 */
#define BCMLTD_BIT_DCLNAME(_name, _max) \
    BCMLTD_BITDCL _name[BCMLTDi_BITDCLSIZE(_max)]


/*!
 * \brief Test if a bit is set in a bit array.
 *
 * \param [in] _a Bit array to operate on
 * \param [in] _b Bit number to test (first is 0)
 *
 * \return 0 if not set, otherwise non-zero.
 */
#define BCMLTD_BIT_GET(_a, _b) \
    BCMLTDi_BITOP(_a, _b, &)

/*!
 * \brief Set a bit in a bit array.
 *
 * \param [in] _a Bit array to operate on
 * \param [in] _b Bit number to set (first is 0)
 *
 * \return Nothing
 */
#define BCMLTD_BIT_SET(_a, _b) \
    BCMLTDi_BITOP(_a, _b, |=)

/*!
 * \brief Clear a bit in a bit array.
 *
 * \param [in] _a Bit array to operate on
 * \param [in] _b Bit number to clear (first is 0)
 *
 * \return Nothing
 */
#define BCMLTD_BIT_CLR(_a, _b) \
    BCMLTDi_BITOP(_a, _b, &= ~)

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
 * static BCMLTD_BITDCLNAME(my_bit_array, MAX_BITS);
 *
 * void my_biterator(void)
 * {
 *  int bitnum;
 *
 *  BCMLTD_BIT_ITER(my_bit_array, MAX_BITS, bitnum) {
 *  printf("Bit number %d is set\n", bitnum);
 *  }
 * }
 * \endcode
 *
 * \param [in] _a Bit array to iterate over
 * \param [in] _max Number of bits to iterate over (starting from 0)
 * \param [in] _b Iterator variable (should be same type as _max)
 */
#define BCMLTD_BIT_ITER(_a, _max, _b)               \
    for ((_b) = 0; (_b) < (_max); (_b)++)        \
        if ((_a)[(_b) / BCMLTD_BITWID] == 0)        \
            (_b) += (BCMLTD_BITWID - 1);            \
        else if (BCMLTD_BIT_GET((_a), (_b)))

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
#define BCMLTD_BIT_SET_RANGE(_a, _offs, _n) \
    (bcmltd_bitop_range_set(_a, _offs, _n))

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
#define BCMLTD_BIT_CLR_RANGE(_a, _offs, _n) \
    (bcmltd_bitop_range_clear(_a, _offs, _n))

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
#define BCMLTD_BIT_COPY_RANGE(_a1, _offs1, _a2, _offs2, _n)  \
    (bcmltd_bitop_range_copy(_a1, _offs1, _a2, _offs2, _n))

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
#define BCMLTD_BIT_AND_RANGE(_a1, _a2, _offs, _n, _dest) \
    (bcmltd_bitop_range_and(_a1, _a2, _offs, _n, _dest))

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
#define BCMLTD_BIT_OR_RANGE(_a1, _a2, _offs, _n, _dest) \
    (bcmltd_bitop_range_or(_a1, _a2, _offs, _n, _dest))

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
#define BCMLTD_BIT_XOR_RANGE(_a1, _a2, _offs, _n, _dest) \
    (bcmltd_bitop_range_xor(_a1, _a2, _offs, _n, _dest))

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
#define BCMLTD_BIT_REMOVE_RANGE(_a1, _a2, _offs, _n, _dest) \
    (bcmltd_bitop_range_remove(_a1, _a2, _offs, _n, _dest))

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
#define BCMLTD_BIT_NEGATE_RANGE(_a, _offs, _n, _dest) \
    (bcmltd_bitop_range_negate(_a, _offs, _n, _dest))

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
#define BCMLTD_BIT_NULL_RANGE(_a, _offs, _n) \
    (bcmltd_bitop_range_null(_a, _offs, _n))

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
#define BCMLTD_BIT_EQ_RANGE(_a1, _a2, _offs, _n) \
    (bcmltd_bitop_range_eq(_a1, _a2, _offs, _n))

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
#define BCMLTD_BIT_COUNT_RANGE(_bits, _count, _first, _range) \
    bcmltd_bitop_range_count(_bits, _first, _range, &(_count))

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
#define BCMLTD_BIT_STR_DECODE(_str, _a, _max) \
    bcmltd_bitop_str_decode(_str, _a, BCMLTDi_BITDCLSIZE(_max))

#endif /* BCMLTD_BITOP_H */
