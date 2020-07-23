/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Bit Array routines
 */

#include <shared/bitop.h>
#include <sal/core/libc.h>

/* Same as shr_bitop_range_null, but for a single SHR_BITDCL.
   The following constraints are kept:
   a. first < SHR_BITWID
   b. first + bit_count <= SHR_BITWID
*/
STATIC INLINE int
shr_bitop_range_null_one_bitdcl(CONST SHR_BITDCL bits,
                                CONST int first,
                                CONST int bit_count)
{
    SHR_BITDCL mask = ~0;

    mask >>= (SHR_BITWID - bit_count);
    /* Move the mask to start from 'first' offset */
    mask <<= first;
    return (bits & mask) == 0;
}
/*
 * A local copy of 'strnlen'. Applied because the standard strnlen()
 * has compatibility issues with some of the compilers.
 */
static int
loc_sal_strnlen(const char *s, size_t max_len)
{
    size_t ii = 0;

    while (ii < max_len && s[ii]) {
        ii++;
    }

    return ii;
}

/* returns 1 if the bit array is empty */
int
shr_bitop_range_null(CONST SHR_BITDCL *bits,
                     CONST int first,
                     int bit_count)
{
    CONST SHR_BITDCL *ptr;
    int woff_first, wremain;

    if(bit_count <= 0) {
        return 1;
    }

    /* Pointer to first SHR_BITDCL in 'bits' that contains 'first' */
    ptr = bits + (first / SHR_BITWID);
    
    /* Offset of 'first' bit within this SDH_BITDCL */
    woff_first = first % SHR_BITWID;

    /* Check if 'first' is SHR_BITWID aligned */
    if (woff_first != 0) {
        /*  Get remaining bits in this SDH_BITDCL */
        wremain = SHR_BITWID - woff_first;
        if (bit_count <= wremain) {
            /* All the range is in one SHR_BITDCL */
            return shr_bitop_range_null_one_bitdcl(*ptr,
                                                   woff_first,
                                                   bit_count);
        }
        /* We should check the first SHR_BITDCL, and might also continue */
        if (!shr_bitop_range_null_one_bitdcl(*ptr, woff_first, wremain)) {
            return 0;
        }
        bit_count -= wremain;
        ++ptr;
    }
    while (bit_count >= SHR_BITWID) {
        /* We're testing a full SHR_BITDCL */
        if (*(ptr++)) {
            return 0;
        }
        bit_count -= SHR_BITWID;
    }
    /* This is the last SHR_BITDCL, and it is not SHR_BITWID */
    if(bit_count > 0) {
        return shr_bitop_range_null_one_bitdcl(*ptr, 0, bit_count);
    }
    return 1;
}

/* Same as shr_bitop_range_eq, but for a single SHR_BITDCL.
   The following constraints are kept:
   a. first < SHR_BITWID
   b. first + range <= SHR_BITWID
*/
STATIC INLINE int
shr_bitop_range_eq_one_bitdcl(CONST SHR_BITDCL bits1,
                              CONST SHR_BITDCL bits2,
                              CONST int first,
                              CONST int range)
{
    SHR_BITDCL mask = ~0;
    mask >>= (SHR_BITWID - range);
    /* Move the mask to start from 'first' offset */
    mask <<= first;
    return (bits1 & mask) == (bits2 & mask);
}

/* returns 1 if the two bitmaps are equal */
int
shr_bitop_range_eq(CONST SHR_BITDCL *bits1,
                   CONST SHR_BITDCL *bits2,
                   CONST int first,
                   int range)
{
    CONST SHR_BITDCL *ptr1;
    CONST SHR_BITDCL *ptr2;
    int woff_first, wremain;

    if(range <= 0) {
        return 1;
    }

    ptr1 = bits1 + (first / SHR_BITWID);
    ptr2 = bits2 + (first / SHR_BITWID);
    
    woff_first = first % SHR_BITWID;
    
    if (woff_first != 0) {
        wremain = SHR_BITWID - woff_first;
        if (range <= wremain) {
            return shr_bitop_range_eq_one_bitdcl(*ptr1,
                                                 *ptr2,
                                                 woff_first,
                                                 range);
        }
        if (!shr_bitop_range_eq_one_bitdcl(*ptr1,
                                          *ptr2,
                                          woff_first,
                                          wremain)) {
            return 0;
        }
        range -= wremain;
        ++ptr1, ++ptr2;
    }
    while (range >= SHR_BITWID) {
        if (*(ptr1++) != *(ptr2++)) {
            return 0;
        }
        range -= SHR_BITWID;
    }
    if(range > 0) {
        return shr_bitop_range_eq_one_bitdcl(*ptr1, *ptr2, 0, range);
    }
    return 1;
}

STATIC INLINE int
shr_bitop_range_count_uchar(CONST uint8 bits)
{
    uint8 tmp_res;
 
    /* Efficient algorithm to count bits */
    tmp_res = (bits & 0x55) + ((bits & 0xaa) >> 1);
    tmp_res = (tmp_res & 0x33) + ((tmp_res & 0xcc) >> 2);
    return (int) ((tmp_res & 0xf) + ((tmp_res & 0xf0) >> 4));
} 

STATIC INLINE int
shr_bitop_range_count_bitdcl_all_bits(CONST SHR_BITDCL bits)
{
    int count = 0, i;
    for(i = 0; i < sizeof(SHR_BITWID); ++i) {
        count += shr_bitop_range_count_uchar((bits >> (8*i)) & 0xff);
    }
    return count;
}

/* Same as shr_bitop_range_count, but for a single SHR_BITDCL.
   The following constraints are kept:
   a. first < SHR_BITWID
   b. first + range <= SHR_BITWID
*/
STATIC INLINE int
shr_bitop_range_count_one_bitdcl(CONST SHR_BITDCL bits,
                                 CONST int first,
                                 CONST int range)
{
    SHR_BITDCL mask = ~0;
    mask >>= (SHR_BITWID - range);
    mask <<= first;
    return shr_bitop_range_count_bitdcl_all_bits(bits & mask);
}

/* returns the number of set bits is the specified range for the bitmap */
void
shr_bitop_range_count(CONST SHR_BITDCL *bits,
                      CONST int first,
                      int range,
                      int *count)
{
    CONST SHR_BITDCL *ptr;
    int woff_first, wremain;

    ptr = bits + (first / SHR_BITWID);
    
    woff_first = first % SHR_BITWID;
    
    *count = 0;

    if(range <= 0) {
        return;
    }

    if (woff_first != 0) {
        wremain = SHR_BITWID - woff_first;
        if (range <= wremain) {
            *count = shr_bitop_range_count_one_bitdcl(*ptr, woff_first, range);
            return;
        }
        *count += shr_bitop_range_count_one_bitdcl(*ptr, woff_first, wremain);
        range -= wremain;
        ++ptr;
    }
    while (range >= SHR_BITWID) {
        *count += shr_bitop_range_count_bitdcl_all_bits(*(ptr++));
        range -= SHR_BITWID;
    }
    if(range > 0) {
        *count += shr_bitop_range_count_one_bitdcl(*ptr, 0, range);
    }
}

/* Same as shr_bitop_range_copy, but for a single SHR_BITDCL.
   The following constraints are kept:
   a. dst_first, src_first < SHR_BITWID
   b. dst_first + range, src_first + range <= SHR_BITWID
*/
STATIC INLINE void
shr_bitop_range_copy_one_bitdcl(SHR_BITDCL *dst_ptr,
                                CONST int dst_first,
                                CONST SHR_BITDCL src,
                                CONST int src_first,
                                CONST int range)
{
    SHR_BITDCL data;
    SHR_BITDCL mask;

    /* no need to check that dst_first == 0 and src_first == 0,
       It must be becuse of the constrains */
    if ((range) == SHR_BITWID) {
        *(dst_ptr) = src;
        return;
    }
    /* get the data */
    data = src >> (src_first);
    /* align the data to the place it may be inserted */
    data <<= (dst_first);

    /* We might have bits in src_ptr above src_first + range
       that need to be cleared */
    mask = ~0;
    mask >>= SHR_BITWID - range;
    mask <<= dst_first;
    data &= mask;
    *(dst_ptr) &= ~mask;
    *(dst_ptr) |= data;
}

void
shr_bitop_range_copy(SHR_BITDCL *dst_ptr,
                     CONST int dst_first,
                     CONST SHR_BITDCL *src_ptr,
                     CONST int src_first,
                     int range)
{
    if(range <= 0) {
        return;
    }

    if ((((dst_first) % SHR_BITWID) == 0) &&
        (((src_first) % SHR_BITWID) == 0) &&
        (((range) % SHR_BITWID) == 0)) {
            sal_memcpy(&((dst_ptr)[(dst_first) / SHR_BITWID]),
                &((src_ptr)[(src_first) / SHR_BITWID]),
                SHR_BITALLOCSIZE(range));
    } else {
        SHR_BITDCL *cur_dst;
        CONST SHR_BITDCL *cur_src;

        int woff_src, woff_dst, wremain;

        cur_dst = (dst_ptr) + ((dst_first) / SHR_BITWID);
        cur_src = (src_ptr) + ((src_first) / SHR_BITWID);

        woff_src = src_first % SHR_BITWID;
        woff_dst = dst_first % SHR_BITWID;

        if (woff_dst >= woff_src) {
            wremain = SHR_BITWID - woff_dst;
        } else {
            wremain = SHR_BITWID - woff_src;
        }
        if (range <= wremain) {
            shr_bitop_range_copy_one_bitdcl(cur_dst,
                                            woff_dst,
                                            *cur_src,
                                            woff_src,
                                            range);
            return;
        }
        shr_bitop_range_copy_one_bitdcl(cur_dst,
                                        woff_dst,
                                        *cur_src,
                                        woff_src,
                                        wremain);
        range -= wremain;
        while (range >= SHR_BITWID) {
            if (woff_dst >= woff_src) {
                ++cur_dst;
                wremain = woff_dst - woff_src;
                if(wremain > 0) {
                    shr_bitop_range_copy_one_bitdcl(cur_dst,
                                                    0,
                                                    *cur_src,
                                                    SHR_BITWID - wremain,
                                                    wremain);
                }
            } else {
                ++cur_src;
                wremain = woff_src - woff_dst;
                shr_bitop_range_copy_one_bitdcl(cur_dst,
                                                SHR_BITWID - wremain,
                                                *cur_src,
                                                0,
                                                wremain);
            }
            range -= wremain;
            wremain = SHR_BITWID - wremain;
            if (woff_dst >= woff_src) {
                ++cur_src;
                shr_bitop_range_copy_one_bitdcl(cur_dst,
                                                SHR_BITWID - wremain,
                                                *cur_src,
                                                0,
                                                wremain);
            } else {
                ++cur_dst;
                shr_bitop_range_copy_one_bitdcl(cur_dst,
                                                0,
                                                *cur_src,
                                                SHR_BITWID - wremain,
                                                wremain);
            }
            range -= wremain;
        }

        if (woff_dst >= woff_src) {
            ++cur_dst;
            wremain = woff_dst - woff_src;
            if (range <= wremain) {
                if(range > 0) {
                    shr_bitop_range_copy_one_bitdcl(cur_dst,
                                                    0,
                                                    *cur_src,
                                                    SHR_BITWID - wremain,
                                                    range);
                }
                return;
            }
            if(wremain > 0) {
                shr_bitop_range_copy_one_bitdcl(cur_dst,
                                                0,
                                                *cur_src,
                                                SHR_BITWID - wremain,
                                                wremain);
            }
        } else {
            ++cur_src;
            wremain = woff_src - woff_dst;
            if (range <= wremain) {
                if(range > 0) {
                    shr_bitop_range_copy_one_bitdcl(cur_dst,
                                                    SHR_BITWID - wremain,
                                                    *cur_src,
                                                    0,
                                                    range);
                }
                return;
            }
            shr_bitop_range_copy_one_bitdcl(cur_dst,
                                            SHR_BITWID - wremain,
                                            *cur_src,
                                            0,
                                            wremain);
        }
        range -= wremain;

        if(range > 0) {
            wremain = SHR_BITWID - wremain;
            if (woff_dst >= woff_src) {
                ++cur_src;
                shr_bitop_range_copy_one_bitdcl(cur_dst,
                                                SHR_BITWID - wremain,
                                                *cur_src,
                                                0,
                                                range);
            } else {
                ++cur_dst;
                shr_bitop_range_copy_one_bitdcl(cur_dst,
                                                0,
                                                *cur_src,
                                                SHR_BITWID - wremain,
                                                range);
            }
        }
    }
}

/* The same as _SHR_BITOP_RANGE, but for a single SHR_BITDCL.
   The following constraints are kept: 
 * a. _first < SHR_BITWID 
 * b. _first + _bit_count < SHR_BITWID.
 */
#define _SHR_BITOP_RANGE_ONE_BITDCL(_bits1,     \
                                    _bits2,     \
                                    _first,     \
                                    _bit_count, \
                                    _dest,      \
                                    _op)        \
{                                               \
    SHR_BITDCL _mask = ~0;                      \
    SHR_BITDCL _data;                           \
    _mask >>= (SHR_BITWID - (_bit_count));      \
    _mask <<=_first;                            \
    _data = ((_bits1) _op (_bits2)) & _mask;    \
    *(_dest) &= ~_mask;                         \
    *(_dest) |= _data;                          \
}

#define _SHR_BITOP_RANGE(_bits1, _bits2, _first, _bit_count, _dest, _op) \
{                                               \
    CONST SHR_BITDCL *_ptr_bits1;               \
    CONST SHR_BITDCL *_ptr_bits2;               \
    SHR_BITDCL *_ptr_dest;                      \
    int _woff_first, _wremain;                  \
                                                \
    _ptr_bits1 =                                \
        (_bits1) + ((_first) / SHR_BITWID);     \
    _ptr_bits2 =                                \
        (_bits2) + ((_first) / SHR_BITWID);     \
    _ptr_dest =                                 \
        (_dest) + ((_first) / SHR_BITWID);      \
    _woff_first = ((_first) % SHR_BITWID);      \
                                                \
    _wremain = SHR_BITWID - _woff_first;        \
    if ((_bit_count) <= _wremain) {             \
        _SHR_BITOP_RANGE_ONE_BITDCL(*_ptr_bits1,\
            *_ptr_bits2,                        \
            _woff_first,                        \
            (_bit_count),                       \
            _ptr_dest, _op);                    \
        return;                                 \
    }                                           \
    _SHR_BITOP_RANGE_ONE_BITDCL(*_ptr_bits1,    \
        *_ptr_bits2, _woff_first, _wremain,     \
        _ptr_dest, _op);                        \
        (_bit_count) -= _wremain;               \
        ++_ptr_bits1; ++_ptr_bits2; ++_ptr_dest;\
    while ((_bit_count) >= SHR_BITWID) {        \
        *_ptr_dest =                            \
            (*_ptr_bits1) _op (*_ptr_bits2);    \
        (_bit_count) -= SHR_BITWID;             \
        ++_ptr_bits1; ++_ptr_bits2; ++_ptr_dest;\
    }                                           \
    if((_bit_count) > 0) {                      \
        _SHR_BITOP_RANGE_ONE_BITDCL(            \
            *_ptr_bits1,                        \
            *_ptr_bits2,                        \
            0,                                  \
            (_bit_count),                       \
            _ptr_dest,                          \
            _op);                               \
    }                                           \
}

void
shr_bitop_range_and(CONST SHR_BITDCL *bits1,
                    CONST SHR_BITDCL *bits2,
                    CONST int first,
                    int bit_count,
                    SHR_BITDCL *dest)
{
    if(bit_count > 0) {
        _SHR_BITOP_RANGE(bits1, bits2, first, bit_count, dest, &);
    }
}

void
shr_bitop_range_or(CONST SHR_BITDCL *bits1,
                   CONST SHR_BITDCL *bits2,
                   CONST int first,
                   int bit_count,
                   SHR_BITDCL *dest)
{
    if(bit_count > 0) {
        _SHR_BITOP_RANGE(bits1, bits2, first, bit_count, dest, |);
    }
}

void
shr_bitop_range_xor(CONST SHR_BITDCL *bits1,
                    CONST SHR_BITDCL *bits2,
                    CONST int first,
                    int bit_count,
                    SHR_BITDCL *dest)
{
    if(bit_count > 0) {
        _SHR_BITOP_RANGE(bits1, bits2, first, bit_count, dest, ^);
    }
}

void
shr_bitop_range_remove(CONST SHR_BITDCL *bits1,
                       CONST SHR_BITDCL *bits2,
                       CONST int first,
                       int bit_count,
                       SHR_BITDCL *dest)
{
    if(bit_count > 0) {
        _SHR_BITOP_RANGE(bits1, bits2, first, bit_count, dest, & ~);
    }
}

/* The same as _SHR_BITNEGATE_RANGE, but for a single SHR_BITDCL.
   The following constraints are kept:
 * a. _first < SHR_BITWID 
 * b. _first + _bit_count < SHR_BITWID.
 */
#define _SHR_BITNEGATE_RANGE_ONE_BITDCL(_bits1, _first, _bit_count, _dest) \
{                                               \
    SHR_BITDCL _mask = ~0;                      \
    SHR_BITDCL _data;                           \
    _mask >>= (SHR_BITWID - (_bit_count));      \
    _mask <<=_first;                            \
    _data =  ~(_bits1) & _mask;                 \
    *(_dest) &= ~_mask;                         \
    *(_dest) |= _data;                          \
}

#define _SHR_BITNEGATE_RANGE(_bits1, _first, _bit_count, _dest) \
{                                               \
    CONST SHR_BITDCL *_ptr_bits1;               \
    SHR_BITDCL *_ptr_dest;                      \
    int _woff_first, _wremain;                  \
                                                \
    _ptr_bits1 =                                \
        (_bits1) + ((_first) / SHR_BITWID);     \
    _ptr_dest =                                 \
        (_dest) + ((_first) / SHR_BITWID);      \
    _woff_first = ((_first) % SHR_BITWID);      \
                                                \
    _wremain = SHR_BITWID - _woff_first;        \
    if ((_bit_count) <= _wremain) {             \
        _SHR_BITNEGATE_RANGE_ONE_BITDCL(        \
            *_ptr_bits1,                        \
            _woff_first,                        \
            (_bit_count),                       \
            _ptr_dest);                         \
        return;                                 \
    }                                           \
    _SHR_BITNEGATE_RANGE_ONE_BITDCL(*_ptr_bits1,\
        _woff_first, _wremain, _ptr_dest);      \
        (_bit_count) -= _wremain;               \
        ++_ptr_bits1; ++_ptr_dest;              \
    while ((_bit_count) >= SHR_BITWID) {        \
        *_ptr_dest = ~(*_ptr_bits1);            \
        (_bit_count) -= SHR_BITWID;             \
        ++_ptr_bits1; ++_ptr_dest;              \
    }                                           \
    if((_bit_count) > 0) {                      \
        _SHR_BITNEGATE_RANGE_ONE_BITDCL(        \
            *_ptr_bits1,                        \
            0,                                  \
            (_bit_count),                       \
            _ptr_dest);                         \
    }                                           \
}

void
shr_bitop_range_negate(CONST SHR_BITDCL *bits1,
                       CONST int first,
                       int bit_count,
                       SHR_BITDCL *dest)
{
    if(bit_count > 0) {
        _SHR_BITNEGATE_RANGE(bits1, first, bit_count, dest);
    }
}

/* The same as shr_bitop_range_clear, but for a single SHR_BITDCL.
   The following constraints are kept:
 * a. b < SHR_BITWID 
 * b. b + c < SHR_BITWID.
 */
STATIC INLINE void
shr_bitop_range_clear_one_bitdcl(SHR_BITDCL *a, CONST int b, CONST int c)
{
    SHR_BITDCL mask = ~0;
    mask >>= (SHR_BITWID - c);
    mask <<= b;
    *a &= ~mask;
}

void
shr_bitop_range_clear(SHR_BITDCL *a, CONST int b, int c)
{
    SHR_BITDCL *ptr;
    int woff_first, wremain;

    if(c <= 0) {
        return;
    }

    ptr = a + (b / SHR_BITWID);
    
    woff_first = b % SHR_BITWID;
    
    if (woff_first != 0) {
        wremain = SHR_BITWID - woff_first;
        if (c <= wremain) {
            shr_bitop_range_clear_one_bitdcl(ptr, woff_first, c);
            return;
        }
        shr_bitop_range_clear_one_bitdcl(ptr, woff_first, wremain);
        c -= wremain;
        ++ptr;
    }
    while (c >= SHR_BITWID) {
        *(ptr++) = 0;
        c -= SHR_BITWID;
    }

    if(c > 0) {
        shr_bitop_range_clear_one_bitdcl(ptr, 0, c);
    }
}

/* The same as shr_bitop_range_set, but for a single SHR_BITDCL.
   The following constraints are kept:
 * a. b < SHR_BITWID 
 * b. b + c < SHR_BITWID.
 */
STATIC INLINE void
shr_bitop_range_set_one_bitdcl(SHR_BITDCL *a, CONST int b, CONST int c)
{
    SHR_BITDCL mask = ~0;
    mask >>= (SHR_BITWID - c);
    mask <<= b;
    *a |= mask;
}

void
shr_bitop_range_set(SHR_BITDCL *a, CONST int b, int c)
{
    SHR_BITDCL *ptr;
    int woff_first, wremain;

    if(c <= 0) {
        return;
    }

    ptr = a + (b / SHR_BITWID);
    
    woff_first = b % SHR_BITWID;
    
    if (woff_first != 0) {
        wremain = SHR_BITWID - woff_first;
        if (c <= wremain) {
            shr_bitop_range_set_one_bitdcl(ptr, woff_first, c);
            return;
        }
        shr_bitop_range_set_one_bitdcl(ptr, woff_first, wremain);
        c -= wremain;
        ++ptr;
    }
    while (c >= SHR_BITWID) {
        *(ptr++) = ~0;
        c -= SHR_BITWID;
    }
    if(c > 0) {
        shr_bitop_range_set_one_bitdcl(ptr, 0, c);
    }
}

/* 
 * Function: shr_bitop_str_decode 
 *  
 * decode a string in hex format into a bitmap
 * returns 0 on success, -1 on error 
 *  
 * The string can be more than 32 bits worth of
 * data if it is in hex format (0x...).  If not
 * hex, it is treated as a single value and not 
 * as a bit map, meaning only a single bit will 
 * be set. 
 */

int
shr_bitop_str_decode(const char *str_value, 
                     SHR_BITDCL *dst_ptr,
                     int max_words)
{
    const char    *e;
    uint32  v;
    int     bit;

    shr_bitop_range_clear(dst_ptr, 0,  SHR_BITWID * max_words);

    if (str_value[0] == '0' && (str_value[1] == 'x' || str_value[1] == 'X')) 
    {
        /* get end of string */
        str_value += 2;
        for (e = str_value; *e; e++) ;

        e -= 1;
        /* back up to beginning of string, setting ports as we go */
        bit = 0;
        while (e >= str_value) 
        {
            if (*e >= '0' && *e <= '9') {
                v = *e - '0';
            } else if (*e >= 'a' && *e <= 'f') {
                v = *e - 'a' + 10;
            } else if (*e >= 'A' && *e <= 'F') {
                v = *e - 'A' + 10;
            } else {
                /* error: invalid hex digits */
                return -1;
            }
            e -= 1;
            /* now set a nibble's worth of ports */
            if ((v & 1) && bit < SHR_BITWID * max_words) {
                SHR_BITSET(dst_ptr, bit);
            }
            bit += 1;
            if ((v & 2) && bit < SHR_BITWID * max_words) {
                SHR_BITSET(dst_ptr, bit);
            }
            bit += 1;
            if ((v & 4) && bit < SHR_BITWID * max_words) {
                SHR_BITSET(dst_ptr, bit);
            }
            bit += 1;
            if ((v & 8) && bit < SHR_BITWID * max_words) {
                SHR_BITSET(dst_ptr, bit);
            }
            bit += 1;
        }
    } 
    else 
    {
        v = 0;
        /* get decimal nuber */
        while (*str_value >= '0' && *str_value <= '9') 
        {
            v = v * 10 + (*str_value++ - '0');
        }

        if (*str_value != '\0') 
        {
            /* error: invalid decimal digits */
            return -1;
        }

        /* set only recieved decimal value */
        if (v < SHR_BITWID * max_words) 
        {
            SHR_BITSET(dst_ptr, v); 
        }
    }
    return 0;
}

void
shr_bitop_str_encode(SHR_BITDCL *source_bitmap,
                     int bit_count,
                     char *destination_str,
                     int destination_str_max_length)
{
    char local_buffer[20];
    int bitmap_words = (bit_count / SHR_BITWID) + (bit_count % SHR_BITWID == 0 ? 0 : 1);
    int word;

    /** if the bitmap needed words is 0, set it to contain at least one word to print 0 properly without infinite loops */
    if(bitmap_words == 0)
    {
        bitmap_words = 1;
    }

    /* clear str */
    *destination_str = '\0';

    /* if bitmap can fit in str*/
    if(bitmap_words * SHR_BITWORD_HEX_STR_CHARS + 1 < destination_str_max_length)
    {
        /* populate str */
        sal_snprintf(local_buffer, sizeof(local_buffer) -1, "0x");
        sal_strncat(destination_str, local_buffer, destination_str_max_length);
        destination_str[destination_str_max_length - 1] = 0;

        for(word = 0; word < bitmap_words; ++word)
        {
            sal_snprintf(local_buffer, sizeof(local_buffer) -1, "%08x", source_bitmap[word]);
            sal_strncat(destination_str, local_buffer, destination_str_max_length - loc_sal_strnlen(destination_str,destination_str_max_length));
            destination_str[destination_str_max_length - 1] = 0;
        }
    }
}

int
shr_bitop_first_bit_get(
    CONST SHR_BITDCL *bits,
    int bit_count)
{
    int bit;
    int found_index = -1;

    for(bit = 0; bit < bit_count; ++bit)
    {
        if (SHR_BITGET(bits, bit))
        {
            found_index = bit;
            break;
        }
    }

    return found_index;
}

int
shr_bitop_last_bit_get(
    CONST SHR_BITDCL *bits,
    int bit_count)
{
    int bit;
    int found_index = -1;

    for(bit = bit_count - 1; bit >= 0; --bit)
    {
        if (SHR_BITGET(bits, bit))
        {
            found_index = bit;
            break;
        }
    }

    return found_index;
}
