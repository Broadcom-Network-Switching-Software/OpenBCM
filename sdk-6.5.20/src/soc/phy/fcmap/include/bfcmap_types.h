/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef __BFCMAP_TYPES_H
#define __BFCMAP_TYPES_H

#include <bfcmap_config.h>
#include <bbase_types.h>


#define BFCMAP_TRUE 1
#define BFCMAP_FALSE 0

#ifndef NULL
#define NULL (void*)0
#endif

#ifndef STATIC
#define STATIC static
#endif

/*
 * BFCMAP port identifier.
 */
typedef buint32_t      bfcmap_port_t;



#define BFCMAP_SAL_DEV_ADDR_CMP(a,b)    ((a) == (b))

/*
 * BFCMAP SCI.
 */
typedef buint8_t    BFCMAP_SCI[8];

#define BFCMAP_COUNTOF(ary)		((int) (sizeof (ary) / sizeof ((ary)[0])))


/*
 * The following macro is to satidfy compiler unused variable warnings.
 */
#define   BFCMAP_COMPILER_SATISFY(v) \
        BCOMPILER_COMPILER_SATISFY(v)

/*
 * 64-bit oper
 */

#define   BFCMAP_COMPILER_64_TO_32_LO(dst, src) \
        BCOMPILER_COMPILER_64_TO_32_LO(dst, src)
#define   BFCMAP_COMPILER_64_TO_32_HI(dst, src) \
        BCOMPILER_COMPILER_64_TO_32_HI(dst, src)
#define   BFCMAP_COMPILER_64_HI(src) \
        BCOMPILER_COMPILER_64_HI(src)
#define   BFCMAP_COMPILER_64_LO(src) \
        BCOMPILER_COMPILER_64_LO(src)
#define   BFCMAP_COMPILER_64_ZERO(dst) \
        BCOMPILER_COMPILER_64_ZERO(dst)
#define   BFCMAP_COMPILER_64_IS_ZERO(src) \
        BCOMPILER_COMPILER_64_IS_ZERO(src)
#define   BFCMAP_COMPILER_64_SET(dst, src_hi, src_lo) \
        BCOMPILER_COMPILER_64_SET(dst, src_hi, src_lo)

/*
 * 64-bit addition and subtraction
 */


#define   BFCMAP_COMPILER_64_ADD_64(dst, src) \
        BCOMPILER_COMPILER_64_ADD_64(dst, src)
#define   BFCMAP_COMPILER_64_ADD_32(dst, src) \
        BCOMPILER_COMPILER_64_ADD_32(dst, src)
#define   BFCMAP_COMPILER_64_SUB_64(dst, src) \
        BCOMPILER_COMPILER_64_SUB_64(dst, src)
#define   BFCMAP_COMPILER_64_SUB_32(dst, src) \
        BCOMPILER_COMPILER_64_SUB_32(dst, src)

/*
 * 64-bit bitwise binary oper
 */


#define   BFCMAP_COMPILER_64_AND(dst, src) \
        BCOMPILER_COMPILER_64_AND(dst, src)
#define   BFCMAP_COMPILER_64_OR(dst, src) \
        BCOMPILER_COMPILER_64_OR(dst, src)
#define   BFCMAP_COMPILER_64_XOR(dst, src) \
        BCOMPILER_COMPILER_64_XOR(dst, src)
#define   BFCMAP_COMPILER_64_NOT(dst) \
        BCOMPILER_COMPILER_64_NOT(dst)


#define   BFCMAP_COMPILER_64_ALLONES(dst) \
        BCOMPILER_COMPILER_64_ALLONES(dst)

/*
 * 64-bit shift
 */


#define   BFCMAP_COMPILER_64_SHL(dst, bits) \
        BCOMPILER_COMPILER_64_SHL(dst, bits)
#define   BFCMAP_COMPILER_64_SHR(dst, bits) \
        BCOMPILER_COMPILER_64_SHR(dst, bits)

#define   BFCMAP_COMPILER_64_BITTEST(val, n) \
        BCOMPILER_COMPILER_64_BITTEST(val, n)


/*
 * 64-bit compare operations
 */


#define   BFCMAP_COMPILER_64_EQ(src1, src2) \
        BCOMPILER_COMPILER_64_EQ(src1, src2) 
#define   BFCMAP_COMPILER_64_NE(src1, src2) \
        BCOMPILER_COMPILER_64_NE(src1, src2)
#define   BFCMAP_COMPILER_64_LT(src1, src2) \
        BCOMPILER_COMPILER_64_LT(src1, src2)
#define   BFCMAP_COMPILER_64_LE(src1, src2) \
        BCOMPILER_COMPILER_64_LE(src1, src2)
#define   BFCMAP_COMPILER_64_GT(src1, src2) \
        BCOMPILER_COMPILER_64_GT(src1, src2)
#define   BFCMAP_COMPILER_64_GE(src1, src2) \
        BCOMPILER_COMPILER_64_GE(src1, src2)


/* Set up a mask of width bits offset lft_shft.  No error checking */
#define   BFCMAP_COMPILER_64_MASK_CREATE(dst, width, lft_shift) \
        BCOMPILER_COMPILER_64_MASK_CREATE(dst, width, lft_shift)

#define   BFCMAP_COMPILER_64_DELTA(src, last, new) \
        BCOMPILER_COMPILER_64_DELTA(src, last, new)

#if 0
/*
 * Some macros for double support
 *
 * You can use the COMPILER_DOUBLE macro
 * if you would prefer double precision, but it is not necessary.
 * If you need more control (or you need to print :), then
 * then you should use the COMPILER_HAS_DOUBLE macro explicitly.
 */
#define BCOMPILER_COMPILER_DOUBLE double
#define BCOMPILER_COMPILER_DOUBLE_FORMAT "f"
#define BCOMPILER_COMPILER_64_TO_DOUBLE(f, i64) \
    ((f) = BCOMPILER_COMPILER_64_HI(i64) * 4294967296.0 + BCOMPILER_COMPILER_64_LO(i64))
#define BCOMPILER_COMPILER_32_TO_DOUBLE(f, i32) \
    ((f) = (double) (i32))


#endif

#endif /* __BFCMAP_TYPES_H */

