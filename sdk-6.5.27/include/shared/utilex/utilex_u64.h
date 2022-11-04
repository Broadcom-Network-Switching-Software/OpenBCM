/** \file utilex_u64.h
 *  
 * 
 * 64 bit unsigned number
 *  
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef UTILEX_U64_H_INCLUDED
/* { */
#define UTILEX_U64_H_INCLUDED

#ifdef  __cplusplus
extern "C"
{
#endif

#include <shared/utilex/utilex_framework.h>
/*
 * MACROs
 * {
 */
/**
 * \brief Divide a 64 bits variable ('dst') by a 32 bits variable ('src') and
 *   store the result into the original 'dst'. If 'src' is zero then do nothing
 *   and return as is.
 *   The operation is, in terms of C language,    dst /= src
 *
 *   \param [in, out] dst
 *     64 bits value to divide. The result is stored back into this variable.
 *   \param [in] src
 *     32 bits divisor to apply on 'dst'
 * \remark
 *   Note that when input divisor is zero, no operation is carried out.
 */
#define UTILEX_64_UDIV_32(dst,src)                 \
    if (src != 0)                                  \
    {                                              \
        /*                                         \
         * Note that we actually divide only if    \
         * the divisor is non-zero                 \
         */                                        \
        COMPILER_64_UDIV_32(dst, src);             \
    }

/**
 * \brief Divide a 64 bits variable ('dst') by a 32 bits variable ('src').
 *   Store the result into the original 'dst' and the remainder in 'remainder'.
 *   If 'src' is zero then do nothing except loading of 'remainder' by '0'.
 *   The operation is, in terms of C language,
 *      if (src == 0) {remainder = 0; break} ; remainder = dst % src ; dst /= src 
 *
 *   \param [in, out] dst
 *     64 bits value to divide. The result is stored back into this variable.
 *   \param [in] src
 *     32 bits divisor to apply on 'dst'
 *   \param [out] remainder
 *     32 bits value to contain remainder of division. Set to zero if input 'src'
 *     is '0'
 * \remark
 *   Note that when input divisor is zero, 'remainder' is set to zero and no other
 *   operation is carried out.
 */
#define UTILEX_64_UDIV_32_WITH_REMAINDER(dst,src,remainder)  \
    if (src != 0)                                            \
    {                                                        \
        /*                                                   \
         * Note that we actually divide only if              \
         * the divisor is non-zero                           \
         */                                                  \
        COMPILER_64_UDIV_32_WITH_REMAINDER(dst, src, remainder);  \
    }                                                        \
    else                                                     \
    {                                                        \
        remainder = 0;                                       \
    }

/*
 * }
 */
/*
 * Procedure prototypes.
 * {
 */
/*****************************************************
*NAME
*  utilex_u64_log2_round_up
*TYPE:
*  PROC
*DATE:
*  16-Jul-07
*FUNCTION:
*  Given number - xx.
*  Return the integer round up of log2(xx)
*CALLING SEQUENCE:
*  utilex_log2_round_up(xx)
*INPUT:
*  UTILEX_DIRECT:
*    const uint64 xx -
*     Number to log.
*  UTILEX_INDIRECT:
*    None.
*OUTPUT:
*  UTILEX_DIRECT:
*    uint32 -
*       Log with base 2. Round up
*  UTILEX_INDIRECT:
*    NON
*REMARKS:
*  Examples:
*    utilex_log2_round_up(0) = 0 -- definition
*    utilex_log2_round_up(1) = 0
*    utilex_log2_round_up(2) = 1
*    utilex_log2_round_up(3) = 2
*    utilex_log2_round_up(4) = 2
*    utilex_log2_round_up(5) = 3
*SEE ALSO:
*****************************************************/
    uint32 utilex_u64_log2_round_up(
    COMPILER_UINT64 xx);

/*****************************************************
*NAME
*  utilex_u64_log2_round_down
*TYPE:
*  PROC
*FUNCTION:
*  Given number - xx.
*  Return the integer round down of log2(x)
*CALLING SEQUENCE:
*  utilex_u64_log2_round_down(xx)
*INPUT:
*  UTILEX_DIRECT:
*    const uint64 xx -
*     Number to log.
*  UTILEX_INDIRECT:
*    None.
*OUTPUT:
*  UTILEX_DIRECT:
*    uint32 -
*       Log with base 2. Round down
*  UTILEX_INDIRECT:
*    NON
*REMARKS:
*  Examples:
*    utilex_log2_round_down(0) = 0 -- definition
*    utilex_log2_round_down(1) = 0
*    utilex_log2_round_down(2) = 1
*    utilex_log2_round_down(3) = 1
*    utilex_log2_round_down(4) = 2
*    utilex_log2_round_down(5) = 2
*SEE ALSO:
*****************************************************/
    uint32 utilex_u64_log2_round_down(
    COMPILER_UINT64 xx);

/*****************************************************
*NAME
* utilex_u64_div_and_round_up
*TYPE:
*  PROC
*FUNCTION:
*  Divide 64-bit unsigned int by 64-bit unsigned int and round up
*INPUT:
*  UTILEX_DIRECT:
*    uint64 dividend -
*      64-bit Dividend
*    uint64 divisor -
*      64-bit Divisor
*    uint64 result -
*      64-bit division result
*  UTILEX_INDIRECT:
*    None.
*OUTPUT:
*  UTILEX_DIRECT:
*    void
*  UTILEX_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
    void utilex_u64_div_and_round_up(
    uint64 dividend,
    uint64 divisor,
    uint64 *result);

/*****************************************************
*NAME
* utilex_u64_print
*TYPE:
*  PROC
*DATE:
*  9-Sep-03
*FUNCTION:
*  Print service to UTILEX_U64.
*INPUT:
*  UTILEX_DIRECT:
*    COMPILER_UINT64 u64 -
*      Structure to print.
*    uint32 print_type -
*      0 - Decimal format.
*      1 - Hexadecimal format.
*    uint32 short_format -
*      Short or long print format.
*  UTILEX_INDIRECT:
*    None.
*OUTPUT:
*  UTILEX_DIRECT:
*    void
*  UTILEX_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
    void utilex_u64_print(
    COMPILER_UINT64 u64,
    uint32 print_type,
    uint32 short_format);

/*****************************************************
*NAME
* utilex_u64_test
*TYPE:
*  PROC
*DATE:
*  9-Sep-03
*FUNCTION:
*  Test function of UTILEX_U64.
*  View prints and return value to see pass/fail
*INPUT:
*  UTILEX_DIRECT:
*    None.
*  UTILEX_INDIRECT:
*    None.
*OUTPUT:
*  UTILEX_DIRECT:
*    uint32
*       TRUE  - PASS.
*       FALSE - FAIL
*  UTILEX_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
    uint32 utilex_u64_test(
    uint32 silent);

/*
 * }
 */
#ifdef  __cplusplus
}
#endif

/* } UTILEX_U64_H_INCLUDED*/
#endif
