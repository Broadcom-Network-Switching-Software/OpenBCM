/**
 * \file utilex_integer_arithmetic.c Math utils for integers 
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SHAREDSWDNX_UTILSDNX

#include <shared/bsl.h>
#include <sal/types.h>

#include <shared/utilex/utilex_integer_arithmetic.h>
#include <shared/shrextend/shrextend_error.h>
#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_u64.h>
#include <shared/utilex/utilex_os_interface.h>

/*
 * Integer Logarithmic { 
 */

uint32
utilex_log2_round(
    uint32 x)
{
    uint32 round_up_val;
    uint32 round_down_val;
    uint32 average_val;

    round_up_val = (((uint32) 1) << utilex_log2_round_up(x));
    round_down_val = (((uint32) 1) << utilex_log2_round_down(x));
    average_val = (round_up_val + round_down_val) / 2;

    /**
     * Find out which value would give
     * a closer result to the user input
     */
    if (average_val < x)
    {
        return utilex_log2_round_up(x);
    }
    else
    {
        return utilex_log2_round_down(x);
    }
}

uint32
utilex_log2_round_up(
    uint32 x)
{
    uint32 msb_bit;

    msb_bit = utilex_msb_bit_on(x);

    if (x > (((uint32) 1) << msb_bit))
    {
        /*
         * need to make round up.
         */
        msb_bit++;
    }

    return msb_bit;
}

uint32
utilex_max_value_by_size(
    uint32 size)
{
    uint32 answer;

    if (size > SAL_UINT32_NOF_BITS)
    {
        answer = 0;
    }
    else if (size == SAL_UINT32_NOF_BITS)
    {
        answer = UTILEX_U32_MAX;
    }
    else
    {
        answer = utilex_power_of_2(size) - 1;
    }

    return answer;
}

uint32
utilex_power_of_2(
    uint32 power)
{
    uint32 answer;

    if (power > SAL_UINT32_MAX_BIT)
    {
        answer = 0;
    }
    else
    {
        answer = 1 << power;
    }

    return answer;
}

uint32
utilex_log2_round_down(
    uint32 x)
{
    return utilex_msb_bit_on(x);
}

uint32
utilex_is_power_of_2(
    uint32 x)
{
    uint32 answer;

    /*
     * Power of 2: only one bit is ZERO.
     */
    answer = (utilex_nof_on_bits_in_long(x) == 1);

    return answer;
}

/*
 * }
 */

/*
 * Bit Manipulation
 * {
 */

/*
 * This struct holds the number of ON bits pre entry.
 * Entry N corresponds to the number N.
 */
const unsigned char utilex_nof_on_bit_in_char[256] = {
    0, 1, 1, 2, 1, 2, 2, 3,
    1, 2, 2, 3, 2, 3, 3, 4,
    1, 2, 2, 3, 2, 3, 3, 4,
    2, 3, 3, 4, 3, 4, 4, 5,
    1, 2, 2, 3, 2, 3, 3, 4,
    2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5,
    3, 4, 4, 5, 4, 5, 5, 6,
    1, 2, 2, 3, 2, 3, 3, 4,
    2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5,
    3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5,
    3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6,
    4, 5, 5, 6, 5, 6, 6, 7,
    1, 2, 2, 3, 2, 3, 3, 4,
    2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5,
    3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5,
    3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6,
    4, 5, 5, 6, 5, 6, 6, 7,
    2, 3, 3, 4, 3, 4, 4, 5,
    3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6,
    4, 5, 5, 6, 5, 6, 6, 7,
    3, 4, 4, 5, 4, 5, 5, 6,
    4, 5, 5, 6, 5, 6, 6, 7,
    4, 5, 5, 6, 5, 6, 6, 7,
    5, 6, 6, 7, 6, 7, 7, 8
};

uint32
utilex_xsb_bit_on_core(
    uint32 x_in,
    uint32 lsb_0_msb_1)
{
    uint32 n, x;
    int i;
    x = x_in;

    if (x == 0)
    {
        return (0);
    }

    n = (lsb_0_msb_1) ? 0 : 31;
    for (i = 16; i > 0; i >>= 1)
    {
        if (lsb_0_msb_1)
        {
            if (x & ((~((uint32) 0)) << i))
            {
                n += i;
                x >>= i;
            }
        }
        else
        {
            if (x & ((~((uint32) 0)) >> i))
            {
                n -= i;
                x <<= i;
            }
        }
    }
    return n;

}
uint32
utilex_msb_bit_on(
    uint32 x_in)
{
    return utilex_xsb_bit_on_core(x_in, 1);
}

uint32
utilex_lsb_bit_on(
    uint32 x_in)
{
    return utilex_xsb_bit_on_core(x_in, 0);
}

uint32
utilex_nof_on_bits_in_long(
    uint32 x)
{
    uint32 nof_on_bits;

    nof_on_bits = 0;
    nof_on_bits += utilex_nof_on_bit_in_char[UTILEX_GET_BYTE_0(x)];
    nof_on_bits += utilex_nof_on_bit_in_char[UTILEX_GET_BYTE_1(x)];
    nof_on_bits += utilex_nof_on_bit_in_char[UTILEX_GET_BYTE_2(x)];
    nof_on_bits += utilex_nof_on_bit_in_char[UTILEX_GET_BYTE_3(x)];

    return nof_on_bits;
}

uint32
utilex_nof_on_bits_in_short(
    unsigned short x)
{
    uint32 nof_on_bits;

    nof_on_bits = 0;
    nof_on_bits += utilex_nof_on_bit_in_char[UTILEX_GET_BYTE_2(x)];
    nof_on_bits += utilex_nof_on_bit_in_char[UTILEX_GET_BYTE_3(x)];

    return nof_on_bits;
}

uint32
utilex_nof_on_bits_in_char(
    unsigned char x)
{
    uint32 nof_on_bits;

    nof_on_bits = 0;
    nof_on_bits += utilex_nof_on_bit_in_char[x];

    return nof_on_bits;
}

/*
 * This struct the reverse persantation of byte.
 */
#define UTILEX_REVERSE_BYTES_NOF_ENTRIES (256)
static const unsigned char utilex_reverse_bytes[UTILEX_REVERSE_BYTES_NOF_ENTRIES] = {
    0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0,
    0x30, 0xB0, 0x70, 0xF0,
    0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8,
    0x38, 0xB8, 0x78, 0xF8,
    0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4,
    0x34, 0xB4, 0x74, 0xF4,
    0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC,
    0x3C, 0xBC, 0x7C, 0xFC,
    0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2,
    0x32, 0xB2, 0x72, 0xF2,
    0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA,
    0x3A, 0xBA, 0x7A, 0xFA,
    0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6,
    0x36, 0xB6, 0x76, 0xF6,
    0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE,
    0x3E, 0xBE, 0x7E, 0xFE,
    0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1,
    0x31, 0xB1, 0x71, 0xF1,
    0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9,
    0x39, 0xB9, 0x79, 0xF9,
    0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5,
    0x35, 0xB5, 0x75, 0xF5,
    0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD,
    0x3D, 0xBD, 0x7D, 0xFD,
    0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3,
    0x33, 0xB3, 0x73, 0xF3,
    0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB,
    0x3B, 0xBB, 0x7B, 0xFB,
    0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7,
    0x37, 0xB7, 0x77, 0xF7,
    0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF,
    0x3F, 0xBF, 0x7F, 0xFF,
};

uint32
utilex_bits_reverse_long(
    uint32 x)
{
    uint32 reverse;

    reverse = (utilex_reverse_bytes[UTILEX_GET_BYTE_0(x)]) << 0;
    reverse |= (utilex_reverse_bytes[UTILEX_GET_BYTE_1(x)]) << 8;
    reverse |= (utilex_reverse_bytes[UTILEX_GET_BYTE_2(x)]) << 16;
    reverse |= (utilex_reverse_bytes[UTILEX_GET_BYTE_3(x)]) << 24;

    return reverse;
}

/*
 */
unsigned short
utilex_bits_reverse_short(
    unsigned short x)
{
    unsigned short reverse;

    reverse = (utilex_reverse_bytes[UTILEX_GET_BYTE_2(x)]) << 0;
    reverse |= (utilex_reverse_bytes[UTILEX_GET_BYTE_3(x)]) << 8;

    return reverse;
}

unsigned char
utilex_bits_reverse_char(
    unsigned char x)
{
    unsigned char reverse;

    reverse = utilex_reverse_bytes[x];

    return reverse;
}

/*
 * }
 */

shr_error_e
utilex_compute_complex_to_mnt_exp(
    uint32 mnt,
    uint32 exp,
    uint32 eq_const_multi,
    uint32 eq_const_mnt_inc,
    uint32 *result)
{
    shr_error_e ex;

    ex = _SHR_E_NONE;

    if (result == NULL)
    {
        ex = _SHR_E_PARAM;
        goto exit;
    }

    if (exp > UTILEX_BIT_STREAM_FIELD_MAX_SIZE)
    {
        ex = _SHR_E_PARAM;
        goto exit;
    }

    *result = eq_const_multi * (mnt + eq_const_mnt_inc) * (1 << exp);

exit:
    return ex;
}

shr_error_e
utilex_break_to_mnt_exp_round_down(
    uint32 x,
    uint32 mnt_nof_bits,
    uint32 exp_nof_bits,
    uint32 mnt_inc,
    uint32 *mnt,
    uint32 *exp)
{
    uint32 y;
    uint32 mnt_max, exp_max, msb_bit_on;
    int msb_bit_diff;
    shr_error_e ex;

    ex = _SHR_E_NONE;

    /*
     * Inputs checking
     */
    if ((mnt == NULL) ||
        (exp == NULL) ||
        (mnt_nof_bits > 16) || (exp_nof_bits > 10) || (mnt_nof_bits == 0) || (exp_nof_bits == 0) || (mnt_inc > 1))
    {
        /*
         * Invalid Inputs
         */
        ex = _SHR_E_PARAM;
        goto exit;
    }

    mnt_max = (0x1 << mnt_nof_bits) - 1;
    exp_max = (0x1 << exp_nof_bits) - 1;
    msb_bit_on = 0;
    msb_bit_diff = 0;

    if (exp_max >= (sizeof(uint32) * 8 - 1))
    {
        /*
         * Case we got exp_max>=31,
         * make it only 30.
         */
        exp_max = sizeof(uint32) * 8 - 2;
    }

    *mnt = 0;
    *exp = 0;

    if (x <= mnt_inc)
    {   /* 
         * x is too small
         * Put the smallest number it represent
         * Actually we should have returned ERR for (x < mnt_inc), because
         * there is no solution that gives a smaller equal number than x,
         * but for backwards compitability we'll return the smallest solution here
         * as well.
         */
        goto exit;
    }
    y = ((1 /* mnt_max+mnt_inc */ ) << exp_max) * mnt_max;
    if (x > y)
    {   /* 
         * x is too big for our represantation
         * Put the biggest number it can
         */
        *mnt = mnt_max;
        *exp = exp_max;
        goto exit;
    }

    msb_bit_on = utilex_msb_bit_on(x);

    msb_bit_diff = (msb_bit_on + 1) - mnt_nof_bits;
    if (msb_bit_diff <= 0)
    {
        msb_bit_diff = 0;
    }

    *mnt = mnt_max << msb_bit_diff;
    *mnt &= x;

    /*
     * decrease mnt_inc .. because the reverse calculation is (mnt+mnt_inc)<<exp 
     */
    if ((*mnt) >= mnt_inc)
    {
        (*mnt) -= mnt_inc;
    }
    else
    {
        if ((*exp) > 0)
        {
            (*exp)--;
        }
        else
        {
            /*
             * The function does not reach here.
             */
            *exp = 0;
        }
    }
    *mnt >>= msb_bit_diff;
    *exp = msb_bit_diff;
exit:
    return ex;
}

shr_error_e
utilex_break_to_mnt_exp_round_up(
    uint32 x,
    uint32 mnt_nof_bits,
    uint32 exp_nof_bits,
    uint32 mnt_inc,
    uint32 *mnt,
    uint32 *exp)
{
    uint32 mnt_max, exp_max;
    uint32 num;
    shr_error_e ex;

    mnt_max = (0x1 << mnt_nof_bits) - 1, exp_max = (0x1 << exp_nof_bits) - 1;
    ex = _SHR_E_NONE;

    /*
     * utilex_break_to_mnt_exp_round_down() is checking the valifdity of the inputs.
     */
    ex = utilex_break_to_mnt_exp_round_down(x, mnt_nof_bits, exp_nof_bits, mnt_inc, mnt, exp);
    if (ex != _SHR_E_NONE)
    {
        goto exit;
    }
    if (exp_max >= (sizeof(uint32) * 8 - 1))
    {
        exp_max = sizeof(uint32) * 8 - 2;
    }

    num = (*mnt + mnt_inc) << (*exp);
    if (num < x)
    {
        /*
         * Round it up.
         */
        (*mnt)++;
        if (*mnt > mnt_max)
        {
            *mnt = *mnt / 2;
            (*exp)++;
            if ((*exp) > exp_max)
            {
                *exp = exp_max;
                *mnt = mnt_max;
            }
        }
    }

exit:
    return ex;
}

uint32
utilex_abs(
    int x)
{
    int y;
    if (x < 0)
    {
        y = -x;
    }
    else
    {
        y = x;
    }
    return y;
}

int
utilex_sign(
    int x)
{
    int sign;

    if (x < 0)
    {
        sign = -1;
    }
    else if (x == 0)
    {
        sign = 0;
    }
    else
    {
        sign = 1;
    }
    return sign;
}

int
utilex_is_neg(
    int x)
{
    int is_neg = 0;

    if (x < 0)
    {
        is_neg = 1;
    }

    return is_neg;
}

int
utilex_is_even(
    uint32 x)
{
    int ret;

    if (x % 2)
    {
        ret = FALSE;
    }
    else
    {
        ret = TRUE;
    }

    return ret;
}

uint32
utilex_get_index_of_max_member_in_array(
    uint32 array[],
    uint32 len)
{
    uint32 index, index_of_max, max_val;

    index_of_max = 0;
    max_val = 0;

    for (index = 0; index < len; index++)
    {
        if (array[index] > max_val)
        {
            max_val = array[index];
            index_of_max = index;
        }
    }
    return index_of_max;
}

uint32
utilex_unsigned_integer_array_average_get(
    uint32 *array,
    uint32 len)
{
    uint32 index;
    uint64 accum_u64;
    uint32 average;

    COMPILER_64_ZERO(accum_u64);
    for (index = 0; index < len; index++)
    {
        COMPILER_64_ADD_32(accum_u64, array[index]);
    }
    COMPILER_64_UDIV_32(accum_u64, len);
    COMPILER_64_TO_32_LO(average, accum_u64);
    return average;
}

/*
 */

/*****************************************************
*NAME
*   utilex_compute_complex_to_mnt_reverse_exp
*TYPE:
*  PROC
*DATE:
*  06/06/2010
*FUNCTION:
*  Get mantissa and exponent .
*  Return Number x,
*  Such that, "x = eq_const_multi * (eq_const_mnt_inc + mnt) / 2^exp".
*CALLING SEQUENCE:
*   utilex_compute_comlicated_to_mnt_reverse_exp(mnt, exp, eq_const_multi, eq_const_mnt_inc, x)
*INPUT:
*  DIRECT:
*    uint32  mnt,
*       mantissa to compute from
*    uint32  exp,
*       exponent to compute from
*    uint32 eq_const_multi,
*       Added const number to the equation
*    uint32 eq_const_mnt_inc,
*       Multiple const number to the equation
*    uint32* x,
*       Buffer to load Number result
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    None.
*  INDIRECT:
*    x.
*REMARKS:
* None.
*SEE ALSO:
 */

shr_error_e
utilex_compute_complex_to_mnt_reverse_exp(
    uint32 mnt,
    uint32 exp,
    uint32 eq_const_multi,
    uint32 eq_const_mnt_inc,
    uint32 *x)
{
    shr_error_e ex = _SHR_E_NONE;
    UTILEX_U64 tmp_result, result;
    uint32 res;

    if (x == NULL)
    {
        ex = _SHR_E_PARAM;
        goto exit;
    }

    if (exp > SAL_UINT32_MAX_BIT)
    {
        ex = _SHR_E_PARAM;
        goto exit;
    }

    utilex_u64_multiply_longs(eq_const_multi, (mnt + eq_const_mnt_inc), &tmp_result);   /* eq_const_multi * (mnt +
                                                                                         * eq_const_mnt_inc) */

    res = utilex_u64_devide_u64_long(&tmp_result, (1 << exp), &result); /* tmp_result / 2^exp */

    res = utilex_u64_to_long(&result, x);
    if (res)    /* overflow */
    {
        ex = _SHR_E_PARAM;
        goto exit;
    }

exit:
    return ex;
}

/*****************************************************
*NAME
*   utilex_compute_complex_to_mnt_exp_reverse_exp
*TYPE:
*  PROC
*DATE:
*  29/01/2012
*FUNCTION:
*  Get mantissa reverse-exponent and exponent .
*  Return Number x,
*  Such that, "x = eq_const_multi * (eq_const_mnt_inc + mnt) * 2^exp / (eq_const_div * 2^rev_exp)".
*CALLING SEQUENCE:
*   utilex_compute_comlicated_to_mnt_reverse_exp(mnt, exp, rev_exp, eq_const_multi, eq_const_div, eq_const_mnt_inc, x)
*INPUT:
*  DIRECT:
*    uint32  mnt,
*       mantissa to compute from
*    uint32  exp,
*       exponent to compute from
*    uint32  rev_exp,
*       reverse_exponent to compute from
*    uint32 eq_const_multi,
*       Multiple const number to the equation
*    uint32 eq_const_div,
*       Divided const number from the equation
*    uint32 eq_const_mnt_inc,
*       Added const number to the equation
*    uint32 round_to_closest,
*       Is the result will be round to closest value (up or down) or always round down.
*    uint32* x,
*       Buffer to load Number result
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    None.
*  INDIRECT:
*    x.
*REMARKS:
* None.
*SEE ALSO:
 */

shr_error_e
utilex_compute_complex_to_mnt_exp_reverse_exp(
    uint32 mnt,
    uint32 exp,
    uint32 rev_exp,
    uint32 eq_const_multi,
    uint32 eq_const_div,
    uint32 eq_const_mnt_inc,
    uint32 round_to_closest,
    uint32 *x)
{
    shr_error_e ex = _SHR_E_NONE;
    UTILEX_U64 tmp_result, tmp_result_2, result;
    uint32 tmp_res, res;

    if (x == NULL)
    {
        ex = _SHR_E_PARAM;
        goto exit;
    }

    utilex_u64_multiply_longs(eq_const_multi, (1 << exp), &tmp_result); /* eq_const_multi * 2^exp */

    /*
     * check if the multiply result is > uint32_max 
     */
    res = utilex_u64_to_long(&tmp_result, &tmp_res);
    if (res)    /* overflow */
    {
        if (round_to_closest)
        {
            res = utilex_u64_add_long(&tmp_result, (((1 << rev_exp) * eq_const_div) / 2));      /* assumption: (1 <<
                                                                                                 * rev_exp) *
                                                                                                 * eq_const_div < 2^32 */
        }
        /*
         * divide first and then continue multiplying 
         */
        res = utilex_u64_devide_u64_long(&tmp_result, (1 << rev_exp) * eq_const_div, &tmp_result_2);    /* tmp_result / 
                                                                                                         * (2^rev_exp * 
                                                                                                         * eq_const_div) 
                                                                                                         */

        res = utilex_u64_to_long(&tmp_result_2, &tmp_res);
        if (res)        /* overflow */
        {
            ex = _SHR_E_PARAM;
            goto exit;
        }

        utilex_u64_multiply_longs(tmp_res, (mnt + eq_const_mnt_inc), &result);  /* tmp_res * (mnt + eq_const_mnt_inc) */
    }
    else
    {
        /*
         * continue multiplying and then divide 
         */
        utilex_u64_multiply_longs(tmp_res, (mnt + eq_const_mnt_inc), &tmp_result);      /* tmp_res * (mnt +
                                                                                         * eq_const_mnt_inc) */
        if (round_to_closest)
        {
            res = utilex_u64_add_long(&tmp_result, (((1 << rev_exp) * eq_const_div) / 2));      /* assumption: (1 <<
                                                                                                 * rev_exp) *
                                                                                                 * eq_const_div < 2^32 */
        }

        res = utilex_u64_devide_u64_long(&tmp_result, (1 << rev_exp) * eq_const_div, &result);  /* tmp_result /
                                                                                                 * (2^rev_exp *
                                                                                                 * eq_const_div) */
    }

    res = utilex_u64_to_long(&result, x);
    if (res)    /* overflow */
    {
        ex = _SHR_E_PARAM;
        goto exit;
    }

exit:
    return ex;
}

/*****************************************************
*NAME
*   utilex_break_complex_to_mnt_exp_round_down
*TYPE:
*  PROC
*DATE:
*  06/06/2010
*FUNCTION:
*  Get a number, x.
*  Return mantissa (mnt) and exponent (exp).
*  Such that, "x = eq_const_multi * (eq_const_mnt_inc + mnt) * 2^exp" is the closest smaller equal number to x.
*CALLING SEQUENCE:
*   utilex_break_comlicated_to_mnt_exp_round_down(x, max_mnt, max_exp, eq_const_multi, eq_const_mnt_inc, mnt, exp)
*INPUT:
*  DIRECT:
*    uint32  x,
*       Number to dis-assemble
*    uint32 max_mnt,
*       Max size of mantissa
*    uint32 max_exp,
*       Max size of exponent
*    uint32 eq_const_multi,
*       Added const number to the equation
*    uint32 eq_const_mnt_inc,
*       Multiple const number to the equation
*    uint32* mnt,
*       Buffer to load mantissa result
*    uint32* exp
*       Buffer to load exponent result
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    None.
*  INDIRECT:
*    mnt, exp.
*REMARKS:
* None.
*SEE ALSO:
 */

shr_error_e
utilex_break_complex_to_mnt_exp_round_down(
    uint32 x,
    uint32 max_mnt,
    uint32 max_exp,
    uint32 eq_const_multi,
    uint32 eq_const_mnt_inc,
    uint32 *mnt,
    uint32 *exp)
{
    shr_error_e ex = _SHR_E_NONE;
    uint32
        bin_serach_beg,
        bin_serach_end, bin_serach_num_elements, bin_serach_mid, bin_serach_low_val, bin_serach_high_val, exp_man;

    if ((mnt == NULL) || (exp == NULL) || (eq_const_multi == 0))
    {
        ex = _SHR_E_PARAM;
        goto exit;
    }

    exp_man = eq_const_multi * (eq_const_mnt_inc + max_mnt);

    if (x <= exp_man)
    {
        *exp = 0;
        if (x < (eq_const_multi * eq_const_mnt_inc))
        {
            *mnt = 0;
        }
        else
        {
            *mnt = (x / eq_const_multi) - eq_const_mnt_inc;
        }
        goto exit;
    }

    if (x >= exp_man * (1 << max_exp))
    {
        *exp = max_exp;
        *mnt = max_mnt;
        goto exit;
    }

    bin_serach_beg = 0;
    bin_serach_end = max_exp;
    bin_serach_num_elements = max_exp;
    bin_serach_mid = bin_serach_beg + bin_serach_num_elements / 2;
    bin_serach_low_val = exp_man * (1 << bin_serach_mid);
    bin_serach_high_val = exp_man * (1 << (bin_serach_mid + 1));

    while ((bin_serach_beg <= bin_serach_end) && !((x > bin_serach_low_val) && (x <= bin_serach_high_val)))
    {
        if (x <= bin_serach_low_val)
        {
            bin_serach_end = bin_serach_mid - 1;
            bin_serach_num_elements = bin_serach_num_elements / 2;
            bin_serach_mid = bin_serach_beg + bin_serach_num_elements / 2;
        }
        else
        {
            bin_serach_beg = bin_serach_mid + 1;
            bin_serach_num_elements = bin_serach_num_elements / 2;
            bin_serach_mid = bin_serach_beg + bin_serach_num_elements / 2;
        }
        bin_serach_low_val = exp_man * (1 << bin_serach_mid);
        bin_serach_high_val = exp_man * (1 << (bin_serach_mid + 1));
    }

    *exp = bin_serach_mid + 1;

    if (x < (eq_const_multi * eq_const_mnt_inc * (1 << *exp)))
    {
        *exp -= 1;
        *mnt = max_mnt;
    }
    else
    {
        *mnt = (x - (eq_const_multi * eq_const_mnt_inc * (1 << *exp))) / (eq_const_multi * (1 << *exp));
    }

exit:
    return ex;

}

/*****************************************************
*NAME
*   utilex_break_complex_to_mnt_exp_round_down_2
*TYPE:
*  PROC
*DATE:
*  06/06/2010
*FUNCTION:
*  Get a number, x.
*  Return mantissa (mnt) and exponent (exp).
*  Such that, "x = eq_const_multi * (eq_const_mnt_inc + mnt) * 2^exp / eq_const_div" is the closest smaller equal number to x.
*  If x < eq_const_multi * (eq_const_mnt_inc + min_mnt) * 2^0 / eq_const_div,
*  i.e. even though the exp is 0 and the mantissa is the minimal mantissa, we get more than x,
*  The function will set exp to 0 and mnt to min_mnt.
*  Else
*  If x <= eq_const_multi * (eq_const_mnt_inc + max_mnt) * 2^0 / eq_const_div,
*  will extract the mantissa out of the equation.
*CALLING SEQUENCE:
*   utilex_break_comlicated_to_mnt_exp_round_down_2(x, max_mnt, max_exp, eq_const_multi, eq_const_div, eq_const_mnt_inc, mnt, exp)
*INPUT:
*  DIRECT:
*    uint32  x,
*       Number to dis-assemble
*    uint32 max_mnt,
*       Max size of mantissa
*    uint32 min_mnt,
*       Min size of mantissa
*    uint32 max_exp,
*       Max size of exponent
*    uint32 eq_const_multi,
*       Multiple const number to the equation
*    uint32 eq_const_div,
*       Divide const number from the equation
*    uint32 eq_const_mnt_inc,
*       Add const number to the equation
*    uint32* mnt,
*       Buffer to load mantissa result
*    uint32* exp
*       Buffer to load exponent result
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    None.
*  INDIRECT:
*    mnt, exp.
*REMARKS:
* None.
*SEE ALSO:
 */
shr_error_e
utilex_break_complex_to_mnt_exp_round_down_2(
    uint32 x,
    uint32 max_mnt,
    uint32 min_mnt,
    uint32 max_exp,
    uint32 eq_const_multi,
    uint32 eq_const_div,
    uint32 eq_const_mnt_inc,
    uint32 *mnt,
    uint32 *exp)
{
    shr_error_e ex = _SHR_E_NONE;
    uint32
        bin_serach_beg,
        bin_serach_end, bin_serach_num_elements, bin_serach_mid, bin_serach_low_val, bin_serach_high_val, exp_man, res,
        tmp;
    UTILEX_U64 tmp_64, tmp_res_64, tmp_val, quotient_mul_dividor_64, remainder, divisor_half;

    if ((mnt == NULL) || (exp == NULL) || (eq_const_multi == 0) || (eq_const_div == 0))
    {
        ex = _SHR_E_PARAM;
        goto exit;
    }

    utilex_u64_multiply_longs(eq_const_multi, (max_mnt + eq_const_mnt_inc), &tmp_64);
    res = utilex_u64_devide_u64_long(&tmp_64, eq_const_div, &tmp_res_64);
    res = utilex_u64_to_long(&tmp_res_64, &exp_man);
    if (res)    /* overflow */
    {
        ex = _SHR_E_PARAM;
        goto exit;
    }

    /*
     * if x <= (eq_const_multi * (eq_const_mnt_inc + max_mnt) / eq_const_div)
     */
    if (x <= exp_man)
    {
        *exp = 0;
        utilex_u64_multiply_longs(eq_const_multi, eq_const_mnt_inc, &tmp_64);
        res = utilex_u64_devide_u64_long(&tmp_64, eq_const_div, &tmp_res_64);
        res = utilex_u64_to_long(&tmp_res_64, &tmp);
        if (res)        /* overflow */
        {
            ex = _SHR_E_PARAM;
            goto exit;
        }
        /*
         * if (x < (eq_const_multi * (eq_const_mnt_inc + 0) / eq_const_div)) 
         */
        if (x < tmp)
        {
            *mnt = min_mnt;
        }
        /*
         * (x >= (eq_const_multi * (eq_const_mnt_inc + 0) / eq_const_div)) 
         */
        else
        {
            /*
             * mantissa + eq_const_mnt_inc =< (x * eq_const_div / eq_const_multi) 
             */
            utilex_u64_multiply_longs(x, eq_const_div, &tmp_64);
            res = utilex_u64_devide_u64_long(&tmp_64, eq_const_multi, &tmp_res_64);
            res = utilex_u64_to_long(&tmp_res_64, &tmp);
            if (res)    /* overflow */
            {
                ex = _SHR_E_PARAM;
                goto exit;
            }

            /*
             * get the closest whole number, either cieling or floor
             */
            utilex_u64_multiply_longs(tmp, eq_const_multi, &quotient_mul_dividor_64);
            utilex_u64_copy(&tmp_64, &remainder);
            if (utilex_u64_subtract_u64(&remainder, &quotient_mul_dividor_64))
                /*
                 * (remainder = (x * eq_const_div) - (eq_const_multi * quotient)) >= 0 
                 */
            {
                utilex_long_to_u64(eq_const_multi / 2, &divisor_half);
                if (utilex_u64_is_bigger(&remainder, &divisor_half) && (tmp - eq_const_mnt_inc < max_mnt))
                {
                    /*
                     * if (remainder > (eq_const_multi / 2))
                     * and we're not exceeding the maximum mantissa take the round up
                     */
                    tmp += 1;
                }
            }

            *mnt = tmp - eq_const_mnt_inc;
            if (*mnt < min_mnt)
            {
                *mnt = min_mnt;
            }
        }
        goto exit;
    }

    /*
     * if (x >= exp_man * (1 << max_exp)) 
     */
    utilex_u64_multiply_longs(exp_man, (uint32) (1 << max_exp), &tmp_val);
    res = utilex_u64_to_long(&tmp_val, &tmp);
    if (!res /* no overflow */  && (x >= tmp))
    {
        *exp = max_exp;
        *mnt = max_mnt;
        goto exit;
    }

    bin_serach_beg = 0;
    bin_serach_end = max_exp;
    bin_serach_num_elements = max_exp;
    bin_serach_mid = bin_serach_beg + bin_serach_num_elements / 2;
    bin_serach_low_val = exp_man * (1 << bin_serach_mid);
    bin_serach_high_val = exp_man * (1 << (bin_serach_mid + 1));

    while ((bin_serach_beg <= bin_serach_end) && !((x > bin_serach_low_val) && (x <= bin_serach_high_val)))
    {
        if (x <= bin_serach_low_val)
        {
            bin_serach_end = bin_serach_mid - 1;
            bin_serach_num_elements = bin_serach_num_elements / 2;
            bin_serach_mid = bin_serach_beg + bin_serach_num_elements / 2;
        }
        else
        {
            bin_serach_beg = bin_serach_mid + 1;
            bin_serach_num_elements = bin_serach_num_elements / 2;
            bin_serach_mid = bin_serach_beg + bin_serach_num_elements / 2;
        }
        bin_serach_low_val = exp_man * (1 << bin_serach_mid);
        bin_serach_high_val = exp_man * (1 << (bin_serach_mid + 1));
    }

    *exp = bin_serach_mid + 1;
    /*
     * (x * eq_const_div) / (eq_const_multi * (1 << *exp)) should be equal to eq_const_mnt_inc + mnt 
     */
    utilex_u64_multiply_longs(x, eq_const_div, &tmp_64);
    res = utilex_u64_devide_u64_long(&tmp_64, eq_const_multi, &tmp_res_64);
    res = utilex_u64_devide_u64_long(&tmp_res_64, (1 << *exp), &tmp_64);
    res = utilex_u64_to_long(&tmp_64, &tmp);
    if (res)    /* overflow */
    {
        ex = _SHR_E_PARAM;
        goto exit;
    }
    *mnt = tmp - eq_const_mnt_inc;

    /*
     * It must hold that *mnt <= max_mnt:
     * * With the current exponent and the maximal mantissa the value is not greater than x (the binary search uses max_mnt to find the exponent). 
     */
    /*
     * assert(*mnt <= max_mnt); 
     */
    if (mnt && *mnt < min_mnt)
    {
        /*
         * The exponent must be greater than 0 - this is checked in the beginning of the function. 
         */
        /*
         * assert(*exp > 0); 
         */
        --(*exp);
        *mnt = 2 * tmp - eq_const_mnt_inc;

        /*
         * we don't really need to check that *mnt >= min_mnt becuase according to the results of the binary search of adequate exponent (searched with max_mnt),
         * after the decrement of the exponent, with max_mnt we get a value smaller than x. 
         */
        /*
         * assert(*mnt >= min_mnt); 
         */
        if (*mnt > max_mnt)
        {
            *mnt = max_mnt;
        }
    }

exit:
    return ex;
}

/*****************************************************
*NAME
*   utilex_break_complex_to_mnt_reverse_exp_round_down
*TYPE:
*  PROC
*DATE:
*  06/06/2010
*FUNCTION:
*  Get a number, x.
*  Return mantissa (mnt) and exponent (exp).
*  Such that, "y = eq_const_multi * (eq_const_mnt_inc + mnt) / 2^exp" is the closest smaller equal number to x.
*CALLING SEQUENCE:
*   utilex_break_comlicated_to_mnt_exp_round_down(x, max_mnt, max_exp, eq_const_multi, eq_const_mnt_inc, mnt, exp)
*INPUT:
*  DIRECT:
*    uint32  x,
*       Number to dis-assemble
*    uint32 max_mnt,
*       Max size of mantissa
*    uint32 max_exp,
*       Max size of exponent
*    uint32 eq_const_multi,
*       Added const number to the equation
*    uint32 eq_const_mnt_inc,
*       Multiple const number to the equation
*    uint32* mnt,
*       Buffer to load mantissa result
*    uint32* exp
*       Buffer to load exponent result
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    None.
*  INDIRECT:
*    mnt, exp.
*REMARKS:
* None.
*SEE ALSO:
 */

shr_error_e
utilex_break_complex_to_mnt_reverse_exp_round_down(
    uint32 x,
    uint32 max_mnt,
    uint32 max_exp,
    uint32 eq_const_multi,
    uint32 eq_const_mnt_inc,
    uint32 *mnt,
    uint32 *exp)
{
    shr_error_e ex = _SHR_E_NONE;
    uint32
        bin_serach_beg,
        bin_serach_end, bin_serach_num_elements, bin_serach_mid, bin_serach_low_val, bin_serach_high_val, exp_man;

    if ((mnt == NULL) || (exp == NULL) || (eq_const_multi == 0))
    {
        ex = _SHR_E_PARAM;
        goto exit;
    }

    exp_man = eq_const_multi * (eq_const_mnt_inc + max_mnt);

    if (x <= exp_man / (1 << max_exp))
    {
        *exp = max_exp;
        *mnt = 0;
        goto exit;
    }

    if (x >= exp_man)
    {
        *exp = 0;
        *mnt = max_mnt;
        goto exit;
    }

    bin_serach_beg = 0;
    bin_serach_end = max_exp;
    bin_serach_num_elements = max_exp;
    bin_serach_mid = bin_serach_beg + bin_serach_num_elements / 2;
    bin_serach_low_val = exp_man / (1 << bin_serach_mid);
    bin_serach_high_val = exp_man / (1 << (bin_serach_mid + 1));

    while ((bin_serach_beg <= bin_serach_end) && !((x >= bin_serach_high_val) && (x <= bin_serach_low_val)))
    {
        if (x > bin_serach_low_val)
        {
            bin_serach_end = bin_serach_mid - 1;
            bin_serach_num_elements = bin_serach_num_elements / 2;
            bin_serach_mid = bin_serach_beg + bin_serach_num_elements / 2;
        }
        else
        {
            bin_serach_beg = bin_serach_mid + 1;
            bin_serach_num_elements = bin_serach_num_elements / 2;
            bin_serach_mid = bin_serach_beg + bin_serach_num_elements / 2;
        }
        bin_serach_low_val = exp_man / (1 << bin_serach_mid);
        bin_serach_high_val = exp_man / (1 << (bin_serach_mid + 1));
    }

    *exp = bin_serach_mid;
    if ((x * (1 << *exp) / eq_const_multi) < eq_const_mnt_inc)
    {
        *mnt = 0;
    }
    else
    {
        *mnt = (x * (1 << *exp) / eq_const_multi) - eq_const_mnt_inc;
    }

exit:
    return ex;

}

/*
 * See header in utilex_integer_arithmetic.h
 */
shr_error_e
utilex_abs_val_to_mnt_binary_fraction_exp(
    uint32 abs_val_numerator,
    uint32 abs_val_denominator,
    uint32 mnt_nof_bits,
    uint32 exp_nof_bits,
    uint32 max_val,
    uint32 *mnt_bin_fraction,
    uint32 *exp)
{
    shr_error_e ex;
    uint32 tmp_num, tmp_exp, real_max_val;
    UTILEX_U64 max_val_mult_u64, abs_val_numerator_u64;

    ex = _SHR_E_NONE;
    real_max_val = (1 << ((1 << exp_nof_bits) + mnt_nof_bits - 1));

    if (abs_val_numerator <= abs_val_denominator)
    {
        *mnt_bin_fraction = (1 << mnt_nof_bits) - 1;
        if (*mnt_bin_fraction > max_val - 1)
        {
            *mnt_bin_fraction = max_val - 1;
        }
        *exp = 0;
        goto exit;
    }
    tmp_exp = (1 << exp_nof_bits) - 1;

    utilex_u64_multiply_longs(max_val, abs_val_denominator, &max_val_mult_u64);
    utilex_long_to_u64(abs_val_numerator, &abs_val_numerator_u64);

    if (utilex_u64_is_bigger(&abs_val_numerator_u64, &max_val_mult_u64))
    {
        tmp_num = (real_max_val * abs_val_denominator) / max_val;
    }
    else
    {
        tmp_num = (real_max_val * abs_val_denominator) / abs_val_numerator;
    }

    if (tmp_num == 0)
    {
        tmp_num = 1;
    }

    while (tmp_num >= (uint32) (1 << mnt_nof_bits))
    {
        tmp_num /= 2;
        tmp_exp -= 1;
    }

    *mnt_bin_fraction = tmp_num;
    *exp = tmp_exp;
exit:
    return ex;
}

shr_error_e
utilex_mnt_binary_fraction_exp_to_abs_val(
    uint32 mnt_nof_bits,
    uint32 exp_nof_bits,
    uint32 max_val,
    uint32 mnt_bin_fraction,
    uint32 exp,
    uint32 *abs_val_numerator,
    uint32 *abs_val_denominator)
{
    shr_error_e ex;
    uint32 tmp_num, tmp_exp, real_max_val;

    ex = _SHR_E_NONE;

    *abs_val_denominator = 1;
    if ((mnt_bin_fraction == (uint32) ((1 << mnt_nof_bits) - 1)) && (exp == 0))
    {
        *abs_val_numerator = 1;
        goto exit;
    }
    tmp_num = mnt_bin_fraction;
    tmp_exp = (1 << exp_nof_bits) - 1;
    real_max_val = (1 << ((1 << exp_nof_bits) + mnt_nof_bits - 1));

    while (tmp_exp > exp)
    {
        tmp_num *= 2;
        tmp_exp -= 1;
    }

    *abs_val_numerator = 0;
    if (tmp_num)
    {
        if (tmp_num > ((1 << mnt_nof_bits) - 1))
        {
            *abs_val_denominator = (1 << mnt_nof_bits);
        }
        *abs_val_numerator = (real_max_val * (*abs_val_denominator)) / tmp_num;
    }

    if ((*abs_val_numerator) / (*abs_val_denominator) > max_val)
    {
        (*abs_val_numerator) = max_val * (*abs_val_denominator);
    }
exit:
    return ex;
}

#if defined (SOC_SAND_DEBUG)
/*
 * { 
 */
/*
 */

static uint32
utilex_msb_bit_on_test(
    uint32 x)
{
    int ii;

    if (x == 0)
    {
        return 0;
    }

    ii = (sizeof(uint32) * 8) - 1;
    for (; ii >= 0; --ii)
    {
        if ((UTILEX_BIT(ii) & x) != 0)
        {
            return (ii);
        }
    }
    /*
     * the function can't reach here.
     */
    return 0;
}

uint32
utilex_mnt_binary_fraction_test(
    uint32 silent)
{
    uint32 res;
    uint32
        abs_val_numerator_i,
        abs_val_denominator_i, exact_abs_val_numerator_i, exact_abs_val_denominator_i, max_val_i, mnt, exp, cycle_count;
    const char *proc_name = "utilex_mnt_binary_fraction_test";

    SHR_FUNC_INIT_VARS(NO_UNIT);
    res = 0;
    if (!silent)
    {
        LOG_CLI_EX("****%s**** START %s%s%s\r\n", proc_name, EMPTY, EMPTY, EMPTY);
    }
    cycle_count = 0;
    for (max_val_i = 100; max_val_i < 4000; max_val_i += 100)
    {
        for (abs_val_denominator_i = 1; abs_val_denominator_i < 60; abs_val_denominator_i++)
        {
            for (abs_val_numerator_i = abs_val_denominator_i; abs_val_numerator_i < 600; abs_val_numerator_i++)
            {
                cycle_count++;
                if (!silent)
                {
                    if ((cycle_count % 100000) == 0)
                    {
                        LOG_CLI_EX("%s(): cycle %d, abs_val_denominator_i %d abs_val_numerator_i %d\r\n",
                                   proc_name, cycle_count, abs_val_denominator_i, abs_val_numerator_i);
                    }
                }
                utilex_abs_val_to_mnt_binary_fraction_exp(abs_val_numerator_i,
                                                          abs_val_denominator_i, 5, 3, max_val_i, &mnt, &exp);
                utilex_mnt_binary_fraction_exp_to_abs_val(5, 3, max_val_i, mnt,
                                                          exp, &exact_abs_val_numerator_i,
                                                          &exact_abs_val_denominator_i);
                if ((abs_val_numerator_i * 100 / abs_val_denominator_i) >
                    (exact_abs_val_numerator_i * 110 / exact_abs_val_denominator_i))
                {
                    if (max_val_i * abs_val_denominator_i < abs_val_numerator_i)
                    {
                        if ((abs_val_numerator_i * 100 / abs_val_denominator_i) >
                            (max_val_i * 110 / abs_val_denominator_i))
                        {
                            continue;
                        }
                    }
                    res = 1;
                    goto exit;
                }
                if ((abs_val_numerator_i * 100 / abs_val_denominator_i) <
                    (exact_abs_val_numerator_i * 50 / exact_abs_val_denominator_i))
                {
                    res = 2;
                    goto exit;
                }
            }

        }
    }

exit:
    if (!silent)
    {
        if (res)
        {
            LOG_CLI_EX("\r\n"
                       "utilex_mnt_binary_fraction_test() returned with an error (res %d). %s%s%s\n\r", res, EMPTY,
                       EMPTY, EMPTY);
        }
        else
        {
            LOG_CLI_EX("\r\n"
                       "utilex_mnt_binary_fraction_test() completed without errors (res %d). %s%s%s\n\r", res, EMPTY,
                       EMPTY, EMPTY);
        }
    }
    /*
     * Set return value to 'res'
     */
    SHR_SET_CURRENT_ERR(res);
    SHR_FUNC_EXIT;
}

uint32
utilex_integer_arithmetic_test(
    uint32 silent)
{
    uint32 pass, seed;
    uint32 seconds, nano_seconds, nof_test, test_number, answer_1, answer_2;
    const char *proc_name = "utilex_integer_arithmetic_test";

    SHR_FUNC_INIT_VARS(NO_UNIT);

    pass = TRUE;
    if (!silent)
    {
        LOG_CLI_EX("****%s**** START %s%s%s\r\n", proc_name, EMPTY, EMPTY, EMPTY);
    }

    utilex_os_get_time(&seconds, &nano_seconds);
    seed = (uint32) (nano_seconds ^ seconds);

    utilex_os_srand(seed);

    /*
     * Test Type 1:
     * Random Number
     */
    nof_test = 20000;
    while (--nof_test)
    {
        test_number = utilex_os_rand();

        answer_1 = utilex_msb_bit_on_test(test_number);
        answer_2 = utilex_msb_bit_on(test_number);

        if (answer_1 != answer_2)
        {
            LOG_CLI_EX("**** Test Type 1: %s - FAIL %s%s%s\r\n", proc_name, EMPTY, EMPTY, EMPTY);
            LOG_CLI_EX("**** answer_1 != answer_2, test_number = 0x%08X, nof_test=%u, seed=%u %s\r\n",
                       test_number, nof_test, seed, EMPTY);
            pass = FALSE;
            goto exit;
        }
    }

    /*
     * Test Type 1:
     * Random leadinf zeros Number
     */
    nof_test = 20000;
    while (--nof_test)
    {
        test_number = utilex_os_rand();
        test_number >>= (utilex_os_rand() % 32);

        answer_1 = utilex_msb_bit_on_test(test_number);
        answer_2 = utilex_msb_bit_on(test_number);

        if (answer_1 != answer_2)
        {
            LOG_CLI_EX("**** Test Type 2: %s - FAIL %s%s%s\r\n", proc_name, EMPTY, EMPTY, EMPTY);
            LOG_CLI_EX("**** answer_1 != answer_2, test_number = 0x%X, nof_test=%u, seed=%u %s\r\n",
                       test_number, nof_test, seed, EMPTY);
            pass = FALSE;
            goto exit;
        }
    }

exit:
    /*
     * Set return value to 'pass'
     */
    SHR_SET_CURRENT_ERR(pass);
    SHR_FUNC_EXIT;
}

void
utilex_print_u_long_binary_format(
    uint32 x,
    uint32 max_digits_in_number)
{
    uint32 bit_i;
    SHR_FUNC_INIT_VARS(NO_UNIT);

    for (bit_i = UTILEX_MIN(32, max_digits_in_number); bit_i > 0;)
    {
        if (x & UTILEX_BIT(--bit_i))
        {
            LOG_CLI_EX("1%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);
        }
        else
        {
            LOG_CLI_EX("0%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);
        }
    }
    SHR_EXIT();
exit:
SHR_VOID_FUNC_EXIT}

/*
 * }
 */
#endif
