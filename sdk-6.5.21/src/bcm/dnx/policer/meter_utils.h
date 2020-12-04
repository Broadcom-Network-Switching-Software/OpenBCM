 /** meter_generic.h
 * 
 *  DNX METER UTILS H FILE. (INTERNAL METER FILE)
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_METER_UTILS_INCLUDED__
/*
 * { 
 */
#define _DNX_METER_UTILS_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * DEFINEs/MACROs
 * {
 */

#define DNX_METER_UTILS_DEFAULT_PROFILE_ID (0)

#define DNX_METER_UTILS_INVALID_IR_REV_EXP (0xFFFFFFFF)
#define DNX_METER_UTILS_MAX_RATE_UNLIMITED (0xFFFFFFFF)
/** const value to devide in the rate formula. */
/** rate[kbps]=CIR_MANT * ((CLK_FREQ_MHZ*(10^6)/2)/(2**(CIR_REV_EXP))) *8* (2^CIR_MANT_EXP))/ (10^3) */
#define DNX_METER_UTILS_MHZ_TO_KBITS_PER_SEC (250)
#define DNX_METER_UTILS_KBITS_TO_BYTES (125) /** const value used in the burst calculation. (1000/8)*/
#define DNX_METER_UTILS_PACKETS_TO_BYTES_FACTOR (64)

#define DNX_METER_UTILS_PROFILE_IR_MAX_UNLIMITED (0xFFFFFFFF)

/* cir, max_cir (if sharing), eir, max_eir
   and if global_sharing: max+min rates of 3 other meters of 4 hierarchical_meters */
#define DNX_METER_UTILS_PROFILE_MAX_RATES_FOR_CALC (6)
#define DNX_METER_UTILS_NOF_METER_CASCADE_MODE (4) /** number of meters to consider in cascade modes (MEF mode) */

#define DNX_METER_UTILS_BURST_MANT_MIN_VAL (1)
#define DNX_METER_UTILS_RATE_MANT_MIN_VAL (1)
#define DNX_METER_UTILS_RATE_EXP_MIN_VAL (0)
#define DNX_METER_UTILS_BURST_EXP_MIN_VAL (0)
#define DNX_METER_UTILS_BURST_MANT_CONST_VAL (0)
#define DNX_METER_UTILS_BURST_MULTI_CONST_VAL (1)
#define DNX_METER_UTILS_RATE_MANT_CONST_VAL (0)

#define DNX_METER_UTILS_ACCURACY_PERCENTAGE (1)
/*
 * }
 */

/*
 * structures
 * {
 */

typedef struct
{
    uint32 color_aware;
    uint32 coupling_flag;
    uint32 sharing_flag;
    uint32 cir_mantissa;
    uint32 cir_mantissa_exponent;
    uint32 reset_cir;
    uint32 cbs_mantissa_64;
    uint32 cbs_exponent;
    uint32 eir_mantissa;
    uint32 eir_mantissa_exponent;
    uint32 max_eir_mantissa;
    uint32 max_eir_mantissa_exponent;
    uint32 reset_eir;
    uint32 ebs_mantissa_64;
    uint32 ebs_exponent;
    uint32 packet_mode;
} meter_utils_profile_table_data;

 /**
 * \brief
 *      Verify that the bucket size is not smaller than the amount of credits it suppose to get in each credits chunk 
 * chunk  [credits] = mant1*2^exp1 + mant2*2^exp2
 * bucket [credits] = (bucket_mant+64)*2^bucket_exp 
 *  if bucket < chunk: bucket=chunk, else: do nothing
 *
 * \param [in] unit -unit id
 * \param [in] rate1_man -rate 1 mantissa
 * \param [in] rate1_exp - rate 1 exponent
 * \param [in] rate2_man -rate 2 mantissa
 * \param [in] rate2_exp -rate 2 exponent
 * \param [out] bucket_man -bucket mantissa 
 * \param [out] bucket_exp - bucket exponent
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
/** see .h file */
shr_error_e dnx_meter_utils_bucket_size_adjust(
    int unit,
    uint32 rate1_man,
    uint32 rate1_exp,
    uint32 rate2_man,
    uint32 rate2_exp,
    uint32 *bucket_man,
    uint32 *bucket_exp);

/**
 * \brief
 *      Break down the burst size to mantissa and exponent.
 * \param [in] unit -unit id
 * \param [in] burst -burst size required
 * \param [in] is_large_bucket_mode -is large bucket mode
 * \param [out] mnt -calculation of mantissa value
 * \param [out] exp -calculation of exp value 
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_meter_utils_profile_burst_to_exp_mnt(
    int unit,
    uint32 burst,
    int is_large_bucket_mode,
    uint32 *mnt,
    uint32 *exp);

/**
 * \brief
 * get back the min rate and max rate, per rev exponent
 * \param [in] unit -unit id
 * \param [in] rev_exp -reverse exponent
 * \param [out] min_rate -min rate calculated
 * \param [out] max_rate -max rate calculated
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_meter_utils_min_max_rate_per_rev_exp_get(
    int unit,
    uint32 rev_exp,
    uint32 *min_rate,
    uint32 *max_rate);

/**
 * \brief
 *      calculate the max and min rate and burst size of the unit and hold it in global structure
 * \param [in] unit -unit id
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   function assume all rates and all burst has the same max values. (ingress/egress/global and cir/eir)
 * \see
 *   NONE
 */
shr_error_e dnx_utils_limits_profile_info_init(
    int unit);

/**
 * \brief
 * Find the best rev_exp for given rate array.
 * find the lowest rev_exp according to the following creteria:
 * find the rev exponent that gives the max average accuracy in percentage. 
 * \param [in] unit -unit id
 * \param [in] rates -array of rates to consider when calc the rev_exp
 * \param [in] nof_rates -size of rates array
 * \param [out] rev_exp -calculated reverse exponent
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 * rev_exp which give rates accuracy better than 1% for all rates, 
 *  will be picked over rev_exp that has better average accuracy, but some of its rates has accurcy worse than 1%.
 * \see
 *   NONE
 */
shr_error_e dnx_meter_utils_rev_exp_optimized_for_bucket_rate(
    int unit,
    uint32 *rates,
    uint32 nof_rates,
    uint32 *rev_exp);

/**
* \brief
*      wrapper function for getting the rate from mantissa, exponent and reverse exponent.
*      the rate is rounded to closest value.
* \param [in] unit -unit id
* \param [in] mnt -mantissa
* \param [in] exp -exponent
* \param [in] rev_exp -reverse exponent
* \param [out] rate -calculated rate
* \return
* shr_error_e
* \remark
*   NONE
* \see
*   NONE
*/
shr_error_e dnx_meter_utils_rate_from_mnt_exp_rev_exp_get(
    int unit,
    uint32 mnt,
    uint32 exp,
    uint32 rev_exp,
    uint32 *rate);

/**
* \brief
*   function return if meter bucket profile is in large bucket mode.
* \param [in] unit        - unit id
* \param [in] cbs - green bucket size
* \param [in] ebs - yellow bucket size
* \param [out] is_large_bucket_mode - TRUE is working in large bucket mode, otherwise FALSE.
* \return
*   shr_error_e - Error Type
* \remark
* 
* \see
*   * None
*/
shr_error_e meter_utils_large_bucket_mode_get(
    int unit,
    uint32 cbs,
    uint32 ebs,
    uint32 *is_large_bucket_mode);

/**
* \brief
*  Get a number, rate (kbps).
*  Return mantissa (mnt) and exponent (exp).
*  Such that, Meter-Rate (bytes) will be the closest smaller equal number to give "rate".
*   Calculation of Meter-Rate (bytes) Update-Rate = Global-Time >> Profile.Rev-Exp 
*   Credit-Per-Update = (Profile.Mant << Profile.Mant-Exp)
*   Meter-Rate (bytes) = Update-Rate * Credit-Per-Update 
*
*  even though the exp is 0 and the mantissa is the minimal mantissa, we get more than rate,
*  The function will set exp to 0 and mnt to min_mnt.
* \param [in] unit        - unit id
* \param [in] rate - rate. Number to dis-assemble
* \param [in] max_mnt - Max size of mantissa
* \param [in] min_mnt - Min size of mantissa
* \param [in] max_exp - Max size of exponent
* \param [in] clock_rate - Multiple const number to the equation
* \param [in] exp_div - Divide const 2^exp number from the equation
* \param [in] eq_const_mnt_inc - Add const number to the equation
* \param [out] mnt - Buffer to load mantissa result.
* \param [out] exp - Buffer to load exponent result.
* \return
*   shr_error_e - Error Type
* \remark
* 
* \see
*   * None
*/
shr_error_e dnx_meter_utils_break_complex_to_mnt_exp(
    int unit,
    uint32 rate,
    uint32 max_mnt,
    uint32 min_mnt,
    uint32 max_exp,
    uint32 clock_rate,
    uint32 exp_div,
    uint32 eq_const_mnt_inc,
    uint32 *mnt,
    uint32 *exp);

/**
* \brief
*  Get mantissa reverse-exponent and exponent .
*  Return Number rate in kbps,
*  calculation: 
*   Update-Rate = Global-Time >> Profile.Rev-Exp.
*   Credit-Per-Update = (Profile.Mant << Profile.Mant-Exp) 
*   Meter-Rate (bytes) = Update-Rate * Credit-Per-Update  
* \param [in] unit        - unit id
* \param [in] mnt -  mantissa to compute from
* \param [in] exp - exponent to compute from
* \param [in] rev_exp - reverse_exponent to compute from
* \param [in] clock_rate - Multiple const number to the equation
* \param [in] eq_const_mnt_inc - Added const number to the equation
* \param [in] round_to_closest - Is the result will be round to closest value (up or down) or always round down.
* \param [in] print_error - print error or not 
* \param [out] rate - Buffer to load Number result
* \return
*   shr_error_e - Error Type
* \remark
* 
* \see
*   * None
*/
shr_error_e dnx_meter_utils_compute_complex_from_mnt_exp_reverse_exp(
    int unit,
    uint32 mnt,
    uint32 exp,
    uint32 rev_exp,
    uint32 clock_rate,
    uint32 eq_const_mnt_inc,
    uint32 round_to_closest,
    int print_error,
    uint32 *rate);

shr_error_e dnx_meter_utils_compute_rate_u64_bytes(
    int unit,
    uint32 mnt,
    uint32 exp,
    uint32 rev_exp,
    uint32 clock_rate,
    uint64 *rate);

#endif/*_DNX_METER_UTILS_INCLUDED__*/
