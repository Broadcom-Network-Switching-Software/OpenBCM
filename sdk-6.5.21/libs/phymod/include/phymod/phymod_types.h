/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Basic type definitions.
 */

#ifndef __PHYMOD_TYPES_H__
#define __PHYMOD_TYPES_H__

#include <phymod/phymod_config.h>

#ifndef FUNCTION_NAME
#define FUNCTION_NAME() (__func__)
#endif

#if PHYMOD_CONFIG_DEFINE_UINT8_T == 1
typedef PHYMOD_CONFIG_TYPE_UINT8_T uint8_t; 
#endif

#if PHYMOD_CONFIG_DEFINE_UINT16_T == 1
typedef PHYMOD_CONFIG_TYPE_UINT16_T uint16_t; 
#endif

#if PHYMOD_CONFIG_DEFINE_UINT32_T == 1
typedef PHYMOD_CONFIG_TYPE_UINT32_T uint32_t; 
#endif

#if PHYMOD_CONFIG_DEFINE_UINT64_T == 1
typedef PHYMOD_CONFIG_TYPE_UINT64_T uint64_t;
#endif

#if PHYMOD_CONFIG_DEFINE_INT64_T == 1
typedef PHYMOD_CONFIG_TYPE_INT64_T int64;
#endif

#if PHYMOD_CONFIG_DEFINE_INT8_T == 1
typedef PHYMOD_CONFIG_TYPE_INT8_T int8_t; 
#endif

#if PHYMOD_CONFIG_DEFINE_INT16_T == 1
typedef PHYMOD_CONFIG_TYPE_INT16_T int16_t; 
#endif

#if PHYMOD_CONFIG_DEFINE_SIZE_T == 1
typedef PHYMOD_CONFIG_TYPE_SIZE_T size_t; 
#endif

#if PHYMOD_CONFIG_DEFINE_PRIx32 == 1
#define PRIx32 PHYMOD_CONFIG_MACRO_PRIx32
#endif

#if PHYMOD_CONFIG_DEFINE_PRIu32 == 1
#define PRIu32 PHYMOD_CONFIG_MACRO_PRIu32
#endif

#if PHYMOD_CONFIG_DEFINE_PRIu64 == 1
#define PRIu64 PHYMOD_CONFIG_MACRO_PRIu64
#endif

#if PHYMOD_CONFIG_DEFINE_ERROR_CODES
typedef enum {
    PHYMOD_E_NONE       = 0,
    PHYMOD_E_INTERNAL   = -1,
    PHYMOD_E_MEMORY     = -2,
    PHYMOD_E_IO         = -3,
    PHYMOD_E_PARAM      = -4,
    PHYMOD_E_CORE       = -5,
    PHYMOD_E_PHY        = -6,
    PHYMOD_E_BUSY       = -7,
    PHYMOD_E_FAIL       = -8,
    PHYMOD_E_TIMEOUT    = -9,
    PHYMOD_E_RESOURCE   = -10,
    PHYMOD_E_CONFIG     = -11,
    PHYMOD_E_UNAVAIL    = -12,
    PHYMOD_E_INIT       = -13,
    PHYMOD_E_EMPTY      = -14,
    PHYMOD_E_LIMIT      = -15           /* Must come last */
} phymod_error_t;
#endif

#ifndef NULL
#define NULL (void*)0
#endif

#ifndef STATIC
#define STATIC static
#endif

#ifndef VOLATILE
#define VOLATILE volatile
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef COUNTOF
#define COUNTOF(ary) ((int) (sizeof(ary) / sizeof((ary)[0])))
#endif

#ifndef PTR2INT
#define PTR2INT(_p) ((int)((long)(_p)))
#endif

#ifndef INT2PTR
#define INT2PTR(_i) ((void *)((long)(_i)))
#endif

#define LSHIFT32(_val, _cnt) ((uint32_t)(_val) << (_cnt))

#ifndef COMPILER_REFERENCE
#define COMPILER_REFERENCE(_a) ((void)(_a))
#endif

#ifndef BYTES2BITS
#define BYTES2BITS(x) ((x) * 8)
#endif

#ifndef BYTES2WORDS
#define BYTES2WORDS(x) (((x) + 3) / 4)
#endif

#ifndef WORDS2BITS
#define WORDS2BITS(x) ((x) * 32)
#endif

#ifndef WORDS2BYTES
#define WORDS2BYTES(x) ((x) * 4)
#endif


/* These must be moved */
#ifndef __F_MASK
#define __F_MASK(w) \
        (((uint32_t)1 << w) - 1)
#endif

#ifndef __F_GET
#define __F_GET(d,o,w) \
        (((d) >> o) & __F_MASK(w))
#endif

#ifndef __F_SET
#define __F_SET(d,o,w,v) \
        (d = ((d & ~(__F_MASK(w) << o)) | (((v) & __F_MASK(w)) << o)))
#endif

#ifndef __F_ENCODE

/* Encode a value of a given width at a given offset. Performs compile-time error checking on the value */
/* To ensure it fits within the given width */
#define __F_ENCODE(v,o,w) \
        ( ((v & __F_MASK(w)) == v) ? /* Value fits in width */ ( (uint32_t)(v) << o ) : /* Value does not fit -- compile time error */ 1 << 99)

#endif

typedef uint32_t phy_id_t;

typedef uint32_t core_id_t;

/* Assume that all modern 32-bit compilers have native 64-bit support */
#ifndef COMPILER_INT64
#define COMPILER_INT64 int64_t
#endif

#ifndef COMPILER_UINT64
#define COMPILER_UINT64 uint64_t
#define COMPILER_64_ADD_64(dst, src)    ((dst) += (src))
#define COMPILER_64_ADD_32(dst, src)    ((dst) += (src))
#define COMPILER_64_SUB_64(dst, src)    ((dst) -= (src))
#define COMPILER_64_SUB_32(dst, src)    ((dst) -= (src))

#define COMPILER_64_AND(dst, src)    ((dst) &= (src))
#define COMPILER_64_OR(dst, src)    ((dst) |= (src))
#define COMPILER_64_XOR(dst, src)    ((dst) ^= (src))
#define COMPILER_64_NOT(dst)        ((dst) = ~(dst))

#define COMPILER_64_GE(src1, src2)      ((src1) >= (src2))

#define COMPILER_64_SHL(dst, bits)    ((dst) <<= (bits))
#define COMPILER_64_SHR(dst, bits)    ((dst) >>= (bits))

#define COMPILER_64_UDIV_64(dst, src)   ((dst) /= (src))
#define COMPILER_64_UDIV_32(dst, src)                                   \
    do {                                                                \
           uint64_t u64_src;                                            \
           COMPILER_64_SET(u64_src, 0, src);                            \
           COMPILER_64_UDIV_64(dst, u64_src);                           \
    } while(0)

#define COMPILER_64_EQ(src1, src2)    ((src1) == (src2))
#define COMPILER_64_NE(src1, src2)    ((src1) != (src2))
#define COMPILER_64_LT(src1, src2)    ((src1) <  (src2))
#define COMPILER_64_LE(src1, src2)    ((src1) <= (src2))
#define COMPILER_64_GT(src1, src2)    ((src1) >  (src2))
#define COMPILER_64_GE(src1, src2)    ((src1) >= (src2))

#define COMPILER_64_AND(dst, src)    ((dst) &= (src))
#define COMPILER_64_OR(dst, src)    ((dst) |= (src))
#define COMPILER_64_XOR(dst, src)    ((dst) ^= (src))
#define COMPILER_64_NOT(dst)        ((dst) = ~(dst))

#define COMPILER_64_HI(src)             ((uint32_t) ((src) >> 32))
#define COMPILER_64_LO(src)             ((uint32_t) (src))
#define COMPILER_64_UMUL_32(dst, src)   ((dst) *= (src))
#define COMPILER_64_ZERO(dst)           ((dst) = 0)
#define COMPILER_64_IS_ZERO(src)        ((src) == 0)
#define COMPILER_64_SET(dst, src_hi, src_lo)             \
    ((dst) = (((uint64_t) ((uint32_t)(src_hi))) << 32) | \
              ((uint64_t) ((uint32_t)(src_lo))))
#define COMPILER_64_COPY(dst, src)      (dst = src)
#define COMPILER_64_LT(src1, src2)    ((src1) <  (src2))
#endif

#endif /* __PHYMOD_TYPES_H__ */
