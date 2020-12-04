/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Bit Array Operations
 */

#ifndef _SHR_BITOP_H
#define _SHR_BITOP_H

#include <sal/types.h>

/* Base type for declarations */
#define    SHR_BITDCL        uint32
#define    SHR_BITWID        32

/* (internal) Number of SHR_BITDCLs needed to contain _max bits */
#define    _SHR_BITDCLSIZE(_max)    (((_max) + SHR_BITWID - 1) / SHR_BITWID)

/* Size for giving to malloc and memset to handle _max bits */
#define    SHR_BITALLOCSIZE(_max) (_SHR_BITDCLSIZE(_max) * sizeof (SHR_BITDCL))


/* (internal) Number of SHR_BITDCLs needed to contain from start bit to start bit + range */
#define _SHR_BITDCLSIZE_FROM_START_BIT(_start_bit, _range) (_range + _start_bit -1)/SHR_BITWID - _start_bit/SHR_BITWID + 1

/* Size of SHR_BITDCLs needed to contain from start bit to start bit + range.
   Needed when you want to do autosync */
#define SHR_BITALLOCSIZE_FROM_START_BIT(_start_bit, _range) (_SHR_BITDCLSIZE_FROM_START_BIT(_start_bit, _range) * sizeof (SHR_BITDCL))



/* Declare bit array _n of size _max bits */
#define    SHR_BITDCLNAME(_n, _max) SHR_BITDCL    _n[_SHR_BITDCLSIZE(_max)]
/* Declare bit array _n of size _max bits, and clear it */
#define    SHR_BIT_DCL_CLR_NAME(_n, _max) SHR_BITDCL _n[_SHR_BITDCLSIZE(_max)] = {0}

/* (internal) Generic operation macro on bit array _a, with bit _b */
#define    _SHR_BITOP(_a, _b, _op)    \
        (((_a)[(_b) / SHR_BITWID]) _op (1U << ((_b) % SHR_BITWID)))

/* Specific operations */
#define    SHR_BITGET(_a, _b)    _SHR_BITOP(_a, _b, &)
#define    SHR_BITSET(_a, _b)    _SHR_BITOP(_a, _b, |=)
#define    SHR_BITCLR(_a, _b)    _SHR_BITOP(_a, _b, &= ~)
#define    SHR_BITWRITE(_a, _b, _val)    ((_val) ? SHR_BITSET(_a, _b) : SHR_BITCLR(_a, _b))
#define    SHR_BIT_ITER(_a, _max, _b)            \
           for ((_b) = 0; (_b) < (_max); (_b)++) \
               if ((_a)[(_b) / SHR_BITWID] == 0) \
                   (_b) += (SHR_BITWID - 1);     \
               else if (SHR_BITGET((_a), (_b)))

#define SHR_IS_BITSET(_a, _b)   (_SHR_BITOP(_a, _b, &) ? TRUE : FALSE)

/* clear _c bits starting from _b in bit array _a */
extern void shr_bitop_range_clear(SHR_BITDCL *a, CONST int b, CONST int c);
#define SHR_BITCLR_RANGE(_a, _b, _c)            \
    (shr_bitop_range_clear(_a, _b, _c))

/* set _c bits starting from _b in bit array _a */
extern void shr_bitop_range_set(SHR_BITDCL *a, CONST int b, CONST int c);
#define SHR_BITSET_RANGE(_a, _b, _c)            \
    (shr_bitop_range_set(_a, _b, _c))

/*
 * Copy _num_bits bits from bit array _src offset _src_offset to bit array _dest offset _dest_offset
 * There should be no overlap between source _src and desstination _dest
 * _dest[_dest_offset:_dest_offset + _num_bits] = _src[_src_offset:_src_offset + _num_bits]
 */
extern void shr_bitop_range_copy(SHR_BITDCL *a,
                                 CONST int b,
                                 CONST SHR_BITDCL *c,
                                 CONST int d,
                                 CONST int e);
#define SHR_BITCOPY_RANGE(_dest, _dest_offset,_src, _src_offset, _num_bits)   \
    (shr_bitop_range_copy(_dest, _dest_offset, _src, _src_offset, _num_bits))

/* Result is 0 only if all bits in the range are 0 */
#define SHR_BITTEST_RANGE(_bits, _first, _bit_count, _result) \
    (_result) = !(shr_bitop_range_null(_bits, _first, _bit_count))

extern void shr_bitop_range_and(CONST SHR_BITDCL *bits1,
                                CONST SHR_BITDCL *bits2,
                                CONST int first,
                                CONST int bit_count,
                                SHR_BITDCL *dest);
extern void shr_bitop_range_or(CONST SHR_BITDCL *bits1,
                               CONST SHR_BITDCL *bits2,
                               CONST int first,
                               CONST int bit_count,
                               SHR_BITDCL *dest);
extern void shr_bitop_range_xor(CONST SHR_BITDCL *bits1,
                                CONST SHR_BITDCL *bits2,
                                CONST int first,
                                CONST int bit_count,
                                SHR_BITDCL *dest);
extern void shr_bitop_range_remove(CONST SHR_BITDCL *bits1,
                                   CONST SHR_BITDCL *bits2,
                                   CONST int first,
                                   CONST int bit_count,
                                   SHR_BITDCL *dest);

#define SHR_BITAND_RANGE(_bits1, _bits2, _first, _bit_count, _dest) \
    (shr_bitop_range_and(_bits1, _bits2, _first, _bit_count, _dest))

#define SHR_BITOR_RANGE(_bits1, _bits2, _first, _bit_count, _dest) \
    (shr_bitop_range_or(_bits1, _bits2, _first, _bit_count, _dest))

#define SHR_BITXOR_RANGE(_bits1, _bits2, _first, _bit_count, _dest) \
    (shr_bitop_range_xor(_bits1, _bits2, _first, _bit_count, _dest))

#define SHR_BITREMOVE_RANGE(_bits1, _bits2, _first, _bit_count, _dest) \
    (shr_bitop_range_remove(_bits1, _bits2, _first, _bit_count, _dest))

extern void shr_bitop_range_negate(CONST SHR_BITDCL *bits1,
                                   CONST int first,
                                   CONST int bit_count,
                                   SHR_BITDCL *dest);

#define SHR_BITNEGATE_RANGE(_bits1, _first, _bit_count, _dest) \
    (shr_bitop_range_negate(_bits1, _first, _bit_count, _dest))

extern int shr_bitop_range_null(CONST SHR_BITDCL *a, CONST int first, CONST int bit_count);
extern int shr_bitop_range_eq(CONST SHR_BITDCL *bits1, CONST SHR_BITDCL *bits2,
                         CONST int first, CONST int range);
extern void shr_bitop_range_count(CONST SHR_BITDCL *bits, CONST int first,
                                 CONST int range, int *count);

#define SHR_BITNULL_RANGE(_bits, _first, _range) \
    (shr_bitop_range_null(_bits, _first, _range))
#define SHR_BITEQ_RANGE(_bits1, _bits2, _first, _range) \
    (shr_bitop_range_eq(_bits1, _bits2, _first, _range))
#define SHR_BITCOUNT_RANGE(_bits, _count, _first, _range) \
    shr_bitop_range_count(_bits, _first, _range, &(_count))

extern int shr_bitop_str_decode(const char *str_value,  SHR_BITDCL *dst_ptr, int max_words);

/* the number of chars requires to represent a bitmap word in string with the following format :"0xffffffff "
 * not including '\0' at the end of the str */
#define SHR_BITWORD_HEX_STR_CHARS 11

/*
 * encoding a bitmap to string using the following format: "0x00000002 0x00000fff 0x00000023 0x00005869"
 * if source bitmap cannot fit into destination_str, destination_str will be filled with an empty string.
 * bit_count is the number of bits in source_bitmap
 * destination_str_max_length is the number of chars in destination_str
 */
void
shr_bitop_str_encode(
    SHR_BITDCL *source_bitmap,
    int bit_count,
    char *destination_str,
    int destination_str_max_length);

/** get first set bit in bitmap, if empty return -1 */
int
shr_bitop_first_bit_get(
    CONST SHR_BITDCL *bits,
    int bit_count);

/** get last set bit in bitmap, if empty return -1 */
int
shr_bitop_last_bit_get(
    CONST SHR_BITDCL *bits,
    int bit_count);

/*Will return either -1 if number of bits is bigger or less than minimum 
  number of bits needed to represent given number.
  Will return postive number only if it is exactly the number of bits needed to represent a given number*/
#define SHR_IS_REPRESENTIBLE_IN_D_BITS(D, N)                \
  (( N >= (1UL << (D - 1)) && N < (1UL << D)) ? D : -1)
/**
 *  MACRO to return the minimum number of bits needed to
 *  represent a given number
 *  Can be used in precompiler time.
    */
/* Each SHR_IS_REPRESENTIBLE_IN_D_BITS will return either "-1" or the number spoecified in the first input paramter
This will result in the end in an equation (31 + 31*(-1) + MIN_NOF_BITS_TO_REPRESENT)*/
#define SHR_BITS_TO_REPRESENT(N)                            \
  (N == 0 ? 1 : (31                                     \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS( 1, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS( 2, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS( 3, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS( 4, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS( 5, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS( 6, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS( 7, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS( 8, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS( 9, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS(10, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS(11, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS(12, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS(13, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS(14, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS(15, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS(16, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS(17, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS(18, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS(19, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS(20, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS(21, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS(22, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS(23, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS(24, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS(25, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS(26, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS(27, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS(28, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS(29, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS(30, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS(31, N)    \
                 + SHR_IS_REPRESENTIBLE_IN_D_BITS(32, N)    \
                 )                                      \
   )

#endif    /* !_SHR_BITOP_H */
