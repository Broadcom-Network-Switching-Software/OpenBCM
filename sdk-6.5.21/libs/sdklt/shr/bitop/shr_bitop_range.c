/*! \file shr_bitop.c
 *
 * Bit array operations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_bitop.h>

/*!
 * INTERNAL USE ONLY.
 *
 * Same as SHRi_BITOP_RANGE, but for a single SHR_BITDCL.
 */
#define SHRi_BITOP_RANGE_ONE_BITDCL(_a1,_a2, _offs, _n, _dest, _op)     \
    {                                                                   \
        SHR_BITDCL _mask = ~0;                                          \
        SHR_BITDCL _data;                                               \
                                                                        \
        _mask >>= (SHR_BITWID - (_n));                                  \
        _mask <<=_offs;                                                 \
        _data = ((_a1) _op (_a2)) & _mask;                              \
        *(_dest) &= ~_mask;                                             \
        *(_dest) |= _data;                                              \
    }

/*!
 * \brief Perform bitwise logical operation on bit arrays.
 *
 * INTERNAL USE ONLY.
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
#define SHRi_BITOP_RANGE(_a1, _a2, _offs, _n, _dest, _op)               \
    {                                                                   \
        const SHR_BITDCL *_ptr_a1;                                      \
        const SHR_BITDCL *_ptr_a2;                                      \
        SHR_BITDCL *_ptr_dest;                                          \
        int _woffs, _wremain;                                           \
                                                                        \
        _ptr_a1 = (_a1) + ((_offs) / SHR_BITWID);                       \
        _ptr_a2 = (_a2) + ((_offs) / SHR_BITWID);                       \
        _ptr_dest = (_dest) + ((_offs) / SHR_BITWID);                   \
        _woffs = ((_offs) % SHR_BITWID);                                \
                                                                        \
        _wremain = SHR_BITWID - _woffs;                                 \
        if ((_n) <= _wremain) {                                         \
            SHRi_BITOP_RANGE_ONE_BITDCL(*_ptr_a1, *_ptr_a2,             \
                                        _woffs, (_n),                   \
                                        _ptr_dest, _op);                \
            return;                                                     \
        }                                                               \
        SHRi_BITOP_RANGE_ONE_BITDCL(*_ptr_a1, *_ptr_a2,                 \
                                    _woffs, _wremain,                   \
                                    _ptr_dest, _op);                    \
        (_n) -= _wremain;                                               \
        ++_ptr_a1; ++_ptr_a2; ++_ptr_dest;                              \
        while ((_n) >= SHR_BITWID) {                                    \
            *_ptr_dest =                                                \
                (*_ptr_a1) _op (*_ptr_a2);                              \
            (_n) -= SHR_BITWID;                                         \
            ++_ptr_a1; ++_ptr_a2; ++_ptr_dest;                          \
        }                                                               \
        if ((_n) > 0) {                                                 \
            SHRi_BITOP_RANGE_ONE_BITDCL(*_ptr_a1, *_ptr_a2,             \
                                        0, (_n),                        \
                                        _ptr_dest, _op);                \
        }                                                               \
    }

/*!
 * \brief Perform bitwise AND operation on bit arrays.
 *
 * INTERNAL USE ONLY.
 *
 * See \ref SHR_BITAND_RANGE for details.
 */
void
shr_bitop_range_and(const SHR_BITDCL *a1, const SHR_BITDCL *a2,
                    const int offs, int n, SHR_BITDCL *dest)
{
    if (n > 0) {
        SHRi_BITOP_RANGE(a1, a2, offs, n, dest, &);
    }
}

/*!
 * \brief Perform bitwise OR operation on bit arrays.
 *
 * INTERNAL USE ONLY.
 *
 * See \ref SHR_BITOR_RANGE for details.
 */
void
shr_bitop_range_or(const SHR_BITDCL *a1, const SHR_BITDCL *a2,
                   const int offs, int n, SHR_BITDCL *dest)
{
    if (n > 0) {
        SHRi_BITOP_RANGE(a1, a2, offs, n, dest, |);
    }
}

/*!
 * \brief Perform bitwise XOR operation on bit arrays.
 *
 * INTERNAL USE ONLY.
 *
 * See \ref SHR_BITXOR_RANGE for details.
 */
void
shr_bitop_range_xor(const SHR_BITDCL *a1, const SHR_BITDCL *a2,
                    const int offs, int n, SHR_BITDCL *dest)
{
    if (n > 0) {
        SHRi_BITOP_RANGE(a1, a2, offs, n, dest, ^);
    }
}

/*!
 * \brief Clear select bits in a bit array.
 *
 * INTERNAL USE ONLY.
 *
 * See \ref SHR_BITREMOVE_RANGE for details.
 */
void
shr_bitop_range_remove(const SHR_BITDCL *a1, const SHR_BITDCL *a2,
                       const int offs, int n, SHR_BITDCL *dest)
{
    if (n > 0) {
        SHRi_BITOP_RANGE(a1, a2, offs, n, dest, & ~);
    }
}
