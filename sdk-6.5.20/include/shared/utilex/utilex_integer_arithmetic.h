/** \file utilex_integer_arithmetic.h
 *  
 * 
 * All common utilities related to integer arithmetic. 
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef UTILEX_INTEGER_ARITHMETIC_H_INCLUDED
/*
 * { 
 */
#define UTILEX_INTEGER_ARITHMETIC_H_INCLUDED

/*************
* INCLUDES  *
 */

#include <sal/limits.h>
#include <sal/types.h>
#include <shared/shrextend/shrextend_error.h>

/*************
* DEFINES   *
 */

/*
 * Divide Section
 * {
 */

/**
 * \brief
 * Integer division x/y with integer round up. 
 * \par DIRECT INPUT 
 *   \param [in] x dividend 
 *   \param [in] y divisor
 * \par DIRECT OUTPUT: 
 *      int -\n 
 *      Division result
 * \note if y=0 .... ERROR
 */
#define UTILEX_DIV_ROUND_UP(x,y) ( ((x)+((y)-1)) / (y) )

/**
 * \brief
 *Integer division x/y with integer round down.
 * \par DIRECT INPUT 
 *   \param [in] x dividend 
 *   \param [in] y divisor
 * \par DIRECT OUTPUT:
 *      int -\n
 *      division result 
 * \note if y=0 .... ERROR
 */
#define UTILEX_DIV_ROUND_DOWN(x,y) ( (x) / (y) )

/**
 * \brief
 *Align x to the upper boundary of y-sized groups - 
 * \par DIRECT INPUT 
 *   \param [in] x dividend 
 *   \param [in] y divisor
 * \par DIRECT OUTPUT:
 *      int -\n
 *      division result 
 * \note if y=0 .... ERROR
 */
#define UTILEX_ALIGN_UP(x, y) \
  (UTILEX_DIV_ROUND_UP(x,y) * (y))

/**
 * \brief
 *Align x to the lower boundary of y-sized groups -
 * \par DIRECT INPUT
 *   \param [in] x dividend
 *   \param [in] y divisor
 * \par DIRECT OUTPUT:
 *      int -\n
 *      division result
 * \note if y=0 .... ERROR
 */
#define UTILEX_ALIGN_DOWN(x, y) \
  (UTILEX_DIV_ROUND_DOWN(x,y) * (y))

/**
 * \brief
 *Integer division x/y with integer round to nearest.
 * \par DIRECT INPUT 
 *   \param [in] x dividend 
 *   \param [in] y divisor
 * \par DIRECT OUTPUT:
 *      int -\n
 *      division result 
 * \note if y=0 .... ERROR
 */
#define UTILEX_DIV_ROUND(x,y) ( ((x) + ((y)/2) ) / (y) )

/**
 * \brief
 *The absolute (positive) value of the difference between two integers
 * \par DIRECT INPUT 
 *   \param [in] x   
 *   \param [in] y  
 * \par DIRECT OUTPUT:
 *      int -\n
 *      the difference between two integers
 */
#define UTILEX_DELTA(x, y) (((x)>(y))?((x)-(y)):((y)-(x)))

/**
 * \brief
 * Return fixed size two complement representation of given sign number
 * \par DIRECT INPUT 
 *   \param [in] num - the value to be converted to two
 *          complement
 *   \param [in] nof_bits - number of bits in two's complement representation
 * \par DIRECT OUTPUT:
 *      int -\n
 *      fixed size two complement of the number
 */
#define UTILEX_SIGNED_NUM_TO_TWO_COMPLEMENT_METHOD(num,nof_bits) ((num >= 0) ? num : ((~(0-num) + 1)&((1<<nof_bits)-1)))

/**
 * \brief
 * Return the signed number from its fixed size two complement 
 * representation 
 * \par DIRECT INPUT 
 *   \param [in] num - the value to be converted to signed
 *          number
 *   \param [in] nof_bits - number of bits in two's complement representation
 * \par DIRECT OUTPUT:
 *      int -\n
 *      signed number
 */
#define UTILEX_TWO_COMPLEMENT_INTO_SIGNED_NUM(num,nof_bits) ( ((num & (1<<(nof_bits-1))) == 0) ? num : 0-((~(num-1))&((1<<nof_bits)-1)) )
/*
 * General Constants
 * {
 */

/**
 * Log2 of 'number of bits in char (unsigned char) - 8'.
 */
#define UTILEX_LOG2_NOF_BITS_IN_CHAR   3
/**
 * Number of bits in char (unsigned char) - 8.
 */
#define UTILEX_NOF_BITS_IN_CHAR        (SAL_BIT(UTILEX_LOG2_NOF_BITS_IN_CHAR))
/**
 * Log2 of 'number of bits in uint32 - 32'.
 */
#define UTILEX_LOG2_NOF_BITS_IN_UINT32 (UTILEX_LOG2_NOF_BITS_IN_CHAR + 2)
/*
 * Number of bits in uint32 - 32.
 */
#define UTILEX_NOF_BITS_IN_UINT32      (SAL_BIT(UTILEX_LOG2_NOF_BITS_IN_UINT32))
/*
 * Min, Max
 * {
 */
#define UTILEX_MIN(x,y) ( (x)<(y)? (x) : (y) )
#define UTILEX_MAX(x,y) ( (x)>(y)? (x) : (y) )
#define UTILEX_MAX3(x,y,z) ( UTILEX_MAX(x, UTILEX_MAX(y, z)) )
#define UTILEX_MAX4(x,y,z,w) ( UTILEX_MAX(x, UTILEX_MAX3(y, z, w)) )

#define UTILEX_RANGE_BOUND(x, lower_bound, upper_bound) \
    UTILEX_MIN(UTILEX_MAX(x, lower_bound), upper_bound)

#define UTILEX_IS_IN_RANGE(x, lower_bound, upper_bound) (x >= lower_bound && x <= upper_bound)

/*
 * }
 */

/*
 * }
 */

/*
 */

/*
 * }
 */

#define UTILEX_LOHALF(x) ((uint32)((x) & 0xffff))
#define UTILEX_HIHALF(x) ((uint32)((x) >> 16 & 0xffff))
#define UTILEX_TOHIGH(x) ((uint32)((x) << 16))
#define UTILEX_HIBITMASK (0x80000000)

/*
 * }
 */

/*
 * Integer Logarithmic
 * {
 */
/**
 * \brief
 * Calculate the log2 with integer round closest.
 * \par DIRECT INPUT
 *   \param [in] x
 * \par DIRECT OUTPUT:
 *      uint32 -\n
 *      the log2 with integer round closest.
 */
uint32 utilex_log2_round(
    uint32 x);
/*
 * extract the log2(x) - Round up.
 */
/**
 * \brief
 * Calculate the log2 with integer round up.
 * \par DIRECT INPUT 
 *   \param [in] x   
 * \par DIRECT OUTPUT:
 *      uint32 -\n
 *      the log2 with integer round up.
 */
uint32 utilex_log2_round_up(
    uint32 x);
/*
 *  Calculate 2^power
 */
uint32 utilex_power_of_2(
    uint32 power);

/** 
 * \brief
 * Calculate the max value according to the size. 
 * \par DIRECT INPUT 
 *   \param [in] size  - max size
 * \par DIRECT OUTPUT:
 *      uint32 -\n
 *      the max value
 */
uint32 utilex_max_value_by_size(
    uint32 size);

/**
 * \brief
 *  Given number - x.
 *  Return the integer round down of log2(x)
 * \par DIRECT INPUT 
 *   \param [in] x -
 *     Number to log.  
 * \par DIRECT OUTPUT:
 *      uint32 -\n
 *      Log with base 2. Round down
 */
uint32 utilex_log2_round_down(
    uint32 x);

/**
 * \brief
 *  Return TRUE: iff the number x is power of 2.
 * \par DIRECT INPUT 
 *   \param [in] x  -\n
 *      Number to check
 * \par DIRECT OUTPUT:
 *      int -\n
 *      True if the number x is power of 2.
 * Otherwise return FALSE 
 * \remark 
 *  Examples: \n
 *    utilex_is_power_of_2(0) = FALSE -- definition \n
 *    utilex_is_power_of_2(1) = TRUE \n
 *    utilex_is_power_of_2(2) = TRUE \n
 *    utilex_is_power_of_2(3) = FALSE 
 */
uint32 utilex_is_power_of_2(
    uint32 x);

/*
 * }
 */

/**
 * \brief
 *  Given number - x.
 *  Return the msb bit number that is on.
 * \par DIRECT INPUT 
 *   \param [in]  x -\n
 *     Number to evaluate
 * \par DIRECT OUTPUT:
 *      uint32 -\n
 *     the bit number
 * \remark 
 *    utilex_msb_bit_on(0) = 0 -- definition
 *    utilex_msb_bit_on(1) = 0
 *    utilex_msb_bit_on(2) = 1
 *    utilex_msb_bit_on(3) = 1
 *    utilex_msb_bit_on(4) = 2
 *    utilex_msb_bit_on(5) = 2
 *    utilex_msb_bit_on(0xFF121212) = 31 
 */
uint32 utilex_msb_bit_on(
    uint32 x);

/**
 * \brief
 *  Given number - x.
 *  Return the lsb bit number that is on.
 * \par DIRECT INPUT 
 *   \param [in]  x -\n
 *     Number to evaluate
 * \par DIRECT OUTPUT:
 *      uint32 -\n
 *     the bit number
 * \remark 
 *    utilex_lsb_bit_on(0) = 0 -- definition
 *    utilex_lsb_bit_on(1) = 0
 *    utilex_lsb_bit_on(2) = 1
 *    utilex_lsb_bit_on(4) = 2
 *    utilex_lsb_bit_on(0x108) = 3
 *    utilex_lsb_bit_on(0x440) = 6
 *    utilex_lsb_bit_on(0xFF121212) = 1
 */
uint32 utilex_lsb_bit_on(
    uint32 x);

/**
 * \brief
 *  Given number - x.
 *  Get the number of bits are on uint32
 * \par DIRECT INPUT 
 *   \param [in]  x -\n
 *     Number to evaluate
 * \par DIRECT OUTPUT:
 *      uint32 -\n
 *     the number of ON
 * \remark 
 *    utilex_nof_on_bits_in_long(0) = 0
 *    utilex_nof_on_bits_in_long(1) = 1
 *    utilex_nof_on_bits_in_long(2) = 1
 *    utilex_nof_on_bits_in_long(3) = 2
 *    utilex_nof_on_bits_in_long(4) = 1
 */
uint32 utilex_nof_on_bits_in_long(
    uint32 x);

/** 
 * \see utilex_nof_on_bits_in_long
 */
uint32 utilex_nof_on_bits_in_short(
    unsigned short x);

/** 
 * \see utilex_nof_on_bits_in_long
 */
uint32 utilex_nof_on_bits_in_char(
    unsigned char x);

/**
 * \brief
 *  Given number - x,
 *  reverse (bit wise) the number for uint32/SHORT/CHAR (32/16/8 bits)
 * \par DIRECT INPUT 
 *   \param [in]  x -\n
 *     Number to evaluate
 * \par DIRECT OUTPUT:
 *    uint32 -\n
 *    unsigned short -\n
 *    unsigned char -\n
 *     Bit reverese of the number
 * \remark 
 *    utilex_bits_reverse_char(0) = 0 \n
 *    utilex_bits_reverse_char(1) = 0x80 \n
 *    utilex_bits_reverse_char(2) = 0x40 \n
 *    utilex_bits_reverse_char(3) = 0xC0 \n
 *    utilex_bits_reverse_char(4) = 0x20 \n
 */
uint32 utilex_bits_reverse_long(
    uint32 x);

/** 
 * \see utilex_bits_reverse_long
 */
unsigned short utilex_bits_reverse_short(
    unsigned short x);

/** 
 * \see utilex_bits_reverse_long
 */
unsigned char utilex_bits_reverse_char(
    unsigned char x);

/**
 * \brief
 *  Given array and length,
 *  Return the index of maximal memeber in the array
 *   \param [in] array -
 *     array to find maximum in
 *   \param [in] len -
 *     the length of the array
 * \return
 *   \retval index of the maximal member in the array
 *      
 * \remark
 *   NONE
 * \see
 *   NONE 
 */
uint32 utilex_get_index_of_max_member_in_array(
    uint32 array[],
    uint32 len);

/**
 * \brief
 *  Given array of unsigned integers and length,
 *  Return the average of the array.
 *   \param [in] array -
 *     array to find maximum in
 *   \param [in] len -
 *     the length of the array
 * \return
 *   \retval average of the array
 *      
 * \remark
 *   NONE
 * \see
 *   NONE 
 */
uint32 utilex_unsigned_integer_array_average_get(
    uint32 *array,
    uint32 len);

/**
 * \brief
 * Function get mantissa and exponent .
 *  Return Number x,
 *  Such that, "x = eq_const_multi * (eq_const_mnt_inc + mnt) * 2^exp".
 * \param [in] mnt - mantissa to compute from
 * \param [in] exp - exponent to compute from
 * \param [in] eq_const_multi - Added const number to the equation
 * \param [in] eq_const_mnt_inc - Multiple const number to the equation
 * \param [out] result -  Buffer to load Number result
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE 
 */

shr_error_e utilex_compute_complex_to_mnt_exp(
    uint32 mnt,
    uint32 exp,
    uint32 eq_const_multi,
    uint32 eq_const_mnt_inc,
    uint32 *result);

/**
 * \brief
 *  Given: 1. Number - x, .
 *         2. Number of mantissa bits - mnt_nof_bits
 *         3. Number of exponent bits - exp_nof_bits
 *  Return mantissa (mnt) and exponent (exp).
 *  Such that, y = (mnt+mnt_inc)<<exp - is the closest smaller equal number than x.
 * \par DIRECT INPUT 
 *   \param [in] x - \n
 *     Number to break
 *   \param [in] man_nof_bits - \n
 *     Size in bits of mantissa. \n
 *     Range 1:10
 *   \param [in] exp_nof_bits - \n
 *     Size in bits of exponent
 *     Range 1:10
 *   \param [in] mnt_inc -\n
 *     A number added to the mantissa for the calculation\n
 *     Range 0:1
 *    \param [out] man -\n
 *     Buffer to load mantissa result
 *    \param [out] exp -\n
 *     Buffer to load exponent result
 * \par DIRECT OUTPUT 
 *    shr_error_e -
 *
 * \par INDIRECT:
 *    mnt, exp.
 * \remark
 *     the mantissa and exponent number of bits \n
 *     ranges from 1 to 10 each. This range may \n
 *     be enlarge with some minor code changes \n
 *     (if needed at all)
 */
shr_error_e utilex_break_to_mnt_exp_round_down(
    uint32 x,
    uint32 man_nof_bits,
    uint32 exp_nof_bits,
    uint32 mnt_inc,
    uint32 *man,
    uint32 *exp);

/**
 * \brief
 *  Get a number, x.
 *  Return mantissa (mnt) and exponent (exp).
 *  Such that, y = (mnt+mnt_inc)<<exp is the closest bigger equal number than x.
 * \par DIRECT INPUT 
 *   \param [in] x - \n
*     Number to dis-assemble
 *   \param [in] man_nof_bits - \n
 *     Size in bits of mantissa. \n
 *     Range 1:10
 *   \param [in] exp_nof_bits - \n
 *     Size in bits of exponent
 *     Range 1:10
 *   \param [in] mnt_inc -\n
 *     A number added to the mantissa for the calculation\n
 *     Range 0:1
 *    \param [out] man -\n
 *     Buffer to load mantissa result
 *    \param [out] exp -\n
 *     Buffer to load exponent result
 * \par DIRECT OUTPUT 
 *    shr_error_e -
 *
 * \par INDIRECT:
 *    mnt, exp.
 */
shr_error_e utilex_break_to_mnt_exp_round_up(
    uint32 x,
    uint32 man_nof_bits,
    uint32 exp_nof_bits,
    uint32 mnt_inc,
    uint32 *man,
    uint32 *exp);

/*
 * return abs number
 */
uint32 utilex_abs(
    int);

/*
 * return sign of a number
 */
int utilex_sign(
    int);

/*
 * return true if x < 0
 */
int utilex_is_neg(
    int);

/**
 * \brief
 *  return TRUE iff x is even.
 * \par DIRECT INPUT 
 *   \param [in] x - \n
 * \par DIRECT OUTPUT 
 *    int -\n
 *      TRUE iff x is even
 */
int utilex_is_even(
    uint32 x);

/*
 * Mantissa, Exponent
 * {
 */
/*
 * Translate an absolute value and reference value to a fraction (given as a binary fraction
 * mantissa and an exponent)
 */

/**
 * \brief
 *   Get an absolute value and a maximum (reference) value, and calculate
 *   the fraction (in a binary fraction mantissa and an exponent) that matches this value.
 *   The value is rounded down.
 * \param [in] abs_val_numerator
 *   uint32. Numerator of absolute value that should be converted to a fraction
 * \param [in] abs_val_denominator
 *   uint32. Denominator of absolute value that should be converted to a fraction
 * \param [in] mnt_nof_bits
 *   uint32. Size in bits of the binary fraction mantissa
 * \param [in] exp_nof_bits
 *   uint32. Size in bits of the exponent
 * \param [in] max_val
 *   uint32. The maximum (reference) value from which to calculate a fraction
 * \param [in] mnt_bin_fraction
 *   uint32 *. This procedure loads pointed memory by the resultant mantissa (binary fraction)
 * \param [in] exp
 *   uint32 *. This procedure loads pointed memory by the resultant exponent.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   *RESULT:
 *     m0 = mantissa[bit0]
 *
 *    (m0/(2^1) + m1/(2^2) + ... +mN/(2^N)/2^exp = 1/abs_val
 * \see
 *   * None
 */
shr_error_e utilex_abs_val_to_mnt_binary_fraction_exp(
    uint32 abs_val_numerator,
    uint32 abs_val_denominator,
    uint32 mnt_nof_bits,
    uint32 exp_nof_bits,
    uint32 max_val,
    uint32 *mnt_bin_fraction,
    uint32 *exp);
/*
 * Translate a fraction (given as a binary fraction mantissa and an exponent)
 * and reference value to an absolute value
 */
shr_error_e utilex_mnt_binary_fraction_exp_to_abs_val(
    uint32 mnt_nof_bits,
    uint32 exp_nof_bits,
    uint32 max_val,
    uint32 mnt_bin_fraction,
    uint32 exp,
    uint32 *abs_val_numerator,
    uint32 *abs_val_denominator);

/*
 * {
 */
/*
 * break a number to mantissa and exponent "x = eq_const_multi * (eq_const_mnt_inc + mnt) * 2^exp" 
 */
shr_error_e utilex_break_complex_to_mnt_exp_round_down(
    uint32 x,
    uint32 max_mnt,
    uint32 max_exp,
    uint32 eq_const_multi,
    uint32 eq_const_man_inc,
    uint32 *mnt,
    uint32 *exp);

/*
 * break a number to mantissa and reverse exponent "x = eq_const_multi * (eq_const_mnt_inc + mnt) / 2^exp" 
 */
shr_error_e utilex_break_complex_to_mnt_reverse_exp_round_down(
    uint32 x,
    uint32 max_mnt,
    uint32 max_exp,
    uint32 eq_const_multi,
    uint32 eq_const_man_inc,
    uint32 *mnt,
    uint32 *exp);

/*
 * compute a number from mantissa and reverse exponent "x = eq_const_multi * (eq_const_mnt_inc + mnt) / 2^exp" 
 */
shr_error_e utilex_compute_complex_to_mnt_reverse_exp(
    uint32 mnt,
    uint32 exp,
    uint32 eq_const_multi,
    uint32 eq_const_mnt_inc,
    uint32 *x);

/*
 * compute a number from mantissa, exponent and reverse exponent "x =
 * eq_const_multi * (eq_const_mnt_inc + mnt) * 2^exp / (eq_const_div * 2^rev_exp)" 
 */
shr_error_e utilex_compute_complex_to_mnt_exp_reverse_exp(
    uint32 mnt,
    uint32 exp,
    uint32 rev_exp,
    uint32 eq_const_multi,
    uint32 eq_const_div,
    uint32 eq_const_mnt_inc,
    uint32 round_to_closest,
    uint32 *x);

/*
 * break a number to mantissa and exponent "x = eq_const_multi * (eq_const_mnt_inc + mnt) * 2^exp / eq_const_div" 
 */
shr_error_e utilex_break_complex_to_mnt_exp_round_down_2(
    uint32 x,
    uint32 max_mnt,
    uint32 min_mnt,
    uint32 max_exp,
    uint32 eq_const_multi,
    uint32 eq_const_div,
    uint32 eq_const_mnt_inc,
    uint32 *mnt,
    uint32 *exp);

/*
 * }
 */
#if 0
#if defined (utilex_DEBUG)
/*
 * { 
 */
/*
 */
uint32 utilex_integer_arithmetic_test(
    uint32 silent);
uint32 utilex_mnt_binary_fraction_test(
    uint32 silent);

void utilex_print_u_long_binary_format(
    uint32 x,
    uint32 max_digits_in_number);
/*
 * }
 */
#endif

/*
 * } 0 
 */
#endif

/*
 * } UTILEX_INTEGER_ARITHMETIC_H_INCLUDED
 */
#endif
