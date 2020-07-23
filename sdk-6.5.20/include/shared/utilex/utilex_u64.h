/** \file utilex_u64.h
 *  
 * 
 * 64 bit unsigned number
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef UTILEX_U64_H_INCLUDED
/* { */
#define UTILEX_U64_H_INCLUDED

#ifdef  __cplusplus
extern "C"
{
#endif

#include <shared/utilex/utilex_framework.h>

#define UTILEX_U64_NOF_UINT32S (2)

    typedef struct
    {
        /*
         * Low  32 bits - arr[0]
         * High 32 bits - arr[1]
         */
        uint32 arr[UTILEX_U64_NOF_UINT32S];
    } UTILEX_U64;

/*****************************************************
*NAME
* soc_long_to_u64
*TYPE:
*  PROC
*DATE:
*  8-Feb-15
*FUNCTION:
*  Creates a u64 out of a uint32
*INPUT:
*  UTILEX_DIRECT:
*    uint32 u32 -
*      Source unit32
*    UTILEX_U64* u64 -
*      Output structre
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
 */
    uint32 utilex_long_to_u64(
    uint32 u32,
    UTILEX_U64 * u64);

/*****************************************************
*NAME
* utilex_u64_copy
*TYPE:
*  PROC
*DATE:
*  8-Feb-15
*FUNCTION:
*  Copy one u64 to the other, return FALSE in case of an error
*INPUT:
*  UTILEX_DIRECT:
*    UTILEX_U64* src -
*      Structure to copy
*    UTILEX_U64* dst -
*      Output structre
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
 */
    uint32 utilex_u64_copy(
    UTILEX_U64 * src,
    UTILEX_U64 * dst);
/*****************************************************
*NAME
* utilex_u64_clear
*TYPE:
*  PROC
*DATE:
*  9-Sep-03
*FUNCTION:
*  Clears all fields in the structure.
*INPUT:
*  UTILEX_DIRECT:
*    UTILEX_U64* u64 -
*      Structure to clear.
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
 */
    void utilex_u64_clear(
    UTILEX_U64 * u64);

/*****************************************************
*NAME
* utilex_u64_to_long
*TYPE:
*  PROC
*DATE:
*  9-Sep-03
*FUNCTION:
*  Convert 'u64' from 64 bit number to 'ulong' 32 bit number.
*  If 'u64' > 0xFFFF_FFFF than 'ulong' is 0xFFFF_FFFF
*  and overflowed indicator is retruned.
*INPUT:
*  UTILEX_DIRECT:
*     UTILEX_U64*      u64 -
*      Structure to convert.
*    uint32* ulong -
*      the number after conversion.
*  UTILEX_INDIRECT:
*    None.
*OUTPUT:
*  UTILEX_DIRECT:
*    uint32
*      Can overflowed
*  UTILEX_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
    uint32 utilex_u64_to_long(
    UTILEX_U64 * u64,
    uint32 *ulong);

/*****************************************************
*NAME
* utilex_u64_multiply_longs
*TYPE:
*  PROC
*DATE:
*  03/03/2004
*FUNCTION:
*  Multiply 2 longs and load result into UTILEX_U64.
*  result = x * y.
*INPUT:
*  UTILEX_DIRECT:
*     uint32 x -
*      value to multipy
*     uint32 y -
*      value to multipy
*    UTILEX_U64*     result -
*      Loaded with result
*  UTILEX_INDIRECT:
*    None.
*OUTPUT:
*  UTILEX_DIRECT:
*    None.
*  UTILEX_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
    void utilex_u64_multiply_longs(
    uint32 x,
    uint32 y,
    UTILEX_U64 * result);

/*****************************************************
*NAME
* utilex_u64_devide_u64_long
*TYPE:
*  PROC
*DATE:
*  03/03/2004
*FUNCTION:
*  Calculates quotient result = x div v
*  Returns remainder r = x mod v
*  where resultq, u are multiprecision integers of 64 bit each
*  and d, v are long digits.
*INPUT:
*  UTILEX_DIRECT:
*     UTILEX_U64*     x -
*      value to device
*     uint32 y  -
*      value to device
*    uint32 result -
*      Loaded with result
*  UTILEX_INDIRECT:
*    None.
*OUTPUT:
*  UTILEX_DIRECT:
*    uint32
*      remainder
*  UTILEX_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
    uint32 utilex_u64_devide_u64_long(
    UTILEX_U64 * x,
    uint32 y,
    UTILEX_U64 * result);

/*****************************************************
*NAME
* utilex_u64_add_long
*TYPE:
*  PROC
*DATE:
*  03/03/2004
*FUNCTION:
*  Calculates x = x + y
*  Returns carryif overflowed
*INPUT:
*  UTILEX_DIRECT:
*     UTILEX_U64*     x -
*      value to add
*        uint32 y -
*      value to add
*  UTILEX_INDIRECT:
*    None.
*OUTPUT:
*  UTILEX_DIRECT:
*    uint32
*      carry.
*  UTILEX_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
    uint32 utilex_u64_add_long(
    UTILEX_U64 * x,
    uint32 y);

/*****************************************************
*NAME
* utilex_u64_add_u64
*TYPE:
*  PROC
*DATE:
*  03/03/2004
*FUNCTION:
*  Calculates result = x + y
*  Returns carryif overflowed
*INPUT:
*  UTILEX_DIRECT:
*     UTILEX_U64*     x -
*      value to add
*     uint32 y  -
*      value to add
*    uint32 result -
*      Loaded with result
*  UTILEX_INDIRECT:
*    None.
*OUTPUT:
*  UTILEX_DIRECT:
*    uint32
*      carry.
*  UTILEX_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
    uint32 utilex_u64_add_u64(
    UTILEX_U64 * x,
    UTILEX_U64 * y);

/*****************************************************
*NAME
* utilex_u64_subtract_u64
*TYPE:
*  PROC
*DATE:
*  03/03/2004
*FUNCTION:
*  Calculates x = x - y
*  Returns FALSE if y > x
*INPUT:
*  UTILEX_DIRECT:
*     UTILEX_U64*     x -
*      value to subtract from
*        UTILEX_U64*     y -
*      value to subtract
*  UTILEX_INDIRECT:
*    None.
*OUTPUT:
*  UTILEX_DIRECT:
*    TRUE x > y.
*    FALSE: x < y.
*  UTILEX_INDIRECT:
*    NON
*REMARKS:
*  If x < y; x<-0, and return FALSE.
*SEE ALSO:
 */
    uint32 utilex_u64_subtract_u64(
    UTILEX_U64 * x,
    UTILEX_U64 * y);

/*****************************************************
*NAME
* utilex_u64_is_bigger
*TYPE:
*  PROC
*DATE:
*  01/07/2004
*FUNCTION:
*  Calculates (x > y)
*  Returns TRUE or FALSE
*INPUT:
*  UTILEX_DIRECT:
*     UTILEX_U64*     x -
*      value to compare
*     UTILEX_U64*     y  -
*      value to compare
*  UTILEX_INDIRECT:
*    None.
*OUTPUT:
*  UTILEX_DIRECT:
*    uint32
*      Indicator.
*      TRUE iff (x > y).
*  UTILEX_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
    uint32 utilex_u64_is_bigger(
    UTILEX_U64 * x,
    UTILEX_U64 * y);

/*****************************************************
*NAME
* utilex_u64_is_zero
*TYPE:
*  PROC
*DATE:
*  01/07/2004
*FUNCTION:
*  Calculates (x == y)
*  Returns TRUE or FALSE
*INPUT:
*  UTILEX_DIRECT:
*     UTILEX_U64*     x -
*      value to compare
*  UTILEX_INDIRECT:
*    None.
*OUTPUT:
*  UTILEX_DIRECT:
*    uint32
*      Indicator.
*      TRUE iff (x == 0).
*  UTILEX_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
    uint32 utilex_u64_is_zero(
    UTILEX_U64 * x);

/*****************************************************
*NAME
* utilex_u64_are_equal
*TYPE:
*  PROC
*DATE:
*  01/07/2004
*FUNCTION:
*  Calculates (x == y)
*  Returns TRUE or FALSE
*INPUT:
*  UTILEX_DIRECT:
*     UTILEX_U64*     x -
*      value to compare
*  UTILEX_DIRECT:
*     UTILEX_U64*     y -
*      value to compare
*  UTILEX_INDIRECT:
*    None.
*OUTPUT:
*  UTILEX_DIRECT:
*    uint32
*      Indicator.
*      TRUE iff (x == y).
*  UTILEX_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
    uint32 utilex_u64_are_equal(
    UTILEX_U64 * x,
    UTILEX_U64 * y);

/*****************************************************
*NAME
*  utilex_u64_log2_round_up
*TYPE:
*  PROC
*DATE:
*  16-Jul-07
*FUNCTION:
*  Given number - x.
*  Return the integer round up of log2(x)
*CALLING SEQUENCE:
*  utilex_log2_round_up(x)
*INPUT:
*  UTILEX_DIRECT:
*    const uint32 x -
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
 */
    uint32 utilex_u64_log2_round_up(
    UTILEX_U64 * x);

/*****************************************************
*NAME
*  utilex_u64_log2_round_down
*TYPE:
*  PROC
*FUNCTION:
*  Given number - x.
*  Return the integer round down of log2(x)
*CALLING SEQUENCE:
*  utilex_u64_log2_round_down(x)
*INPUT:
*  UTILEX_DIRECT:
*    const uint32 x -
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
 */
    uint32 utilex_u64_log2_round_down(
    UTILEX_U64 * x);

/*****************************************************
*NAME
* utilex_u64_shift_left
*TYPE:
*  PROC
*DATE:
*  03/03/2004
*FUNCTION:
*  Computes a = b << x.
*INPUT:
*  UTILEX_DIRECT:
*    UTILEX_U64*    a -
*      loaded with result
*     UTILEX_U64*    b -
*      number to shift
*     uint32 result -
*      Loaded with result
*  UTILEX_INDIRECT:
*    None.
*OUTPUT:
*  UTILEX_DIRECT:
*    uint32 - the carry.
*  UTILEX_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
    uint32 utilex_u64_shift_left(
    UTILEX_U64 * a,
    UTILEX_U64 * b,
    uint32 x);

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
*    UTILEX_U64* u64 -
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
 */
    void utilex_u64_print(
    UTILEX_U64 * u64,
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
 */
    uint32 utilex_u64_test(
    uint32 silent);

#ifdef  __cplusplus
}
#endif

/* } UTILEX_U64_H_INCLUDED*/
#endif
