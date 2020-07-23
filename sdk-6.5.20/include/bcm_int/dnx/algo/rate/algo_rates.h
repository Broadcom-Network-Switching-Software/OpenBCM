/**
 * \file algo/rate/algo_rates.h 
 *
 * Internal DNX Shapers rates APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __ALGO_RATES_H_INCLUDED__
#define __ALGO_RATES_H_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Includes
 * {
 */

#include <shared/shrextend/shrextend_error.h>

/*
 * }
 */

/*
 * Typedefs
 * {
 */

/*
 * \brief
 *   Shaper rate is composed from credit and interval.
 *   When converting a rate to credit and interval, needs to specify
 *   the HW boundaries for them.
 */
typedef struct
{
    /** Minimum value for credit */
    uint32 credit_min;
    /** Maximum value for credit */
    uint32 credit_max;
    /** Minimum value for interval */
    uint32 interval_min;
    /** Maximum value for interval */
    uint32 interval_max;
} dnx_algo_rates_shaper_bounds_t;

/*
 * }
 */

/*
 * Functions:
 * {
 */

/**
 * \brief - Convert a rate given in Kbits/sec units to an interval
 * between 2 consecutive credits in device clocks.
 *
 * The conversion is done according to:
 *                       Credit [Kbits] * Num_of_clocks_in_sec [clocks/sec]
 *  Rate [Kbits/Sec] =   -------------------------------------------------------------
 *                          interval_between_credits_in_clocks [clocks]
 *  \param [in] unit -
 *    The unit id.
 *  \param [in] rate -
 *      The rate [KBits/Sec].
 *  \param [in] credit -
 *    The credit size [Bytes].
 *  \param [out] interval -
 *      Interval between credits [clocks].
 *
 * \note
 * Due to rounding issues we don't actually divide the credit
 * size by 1000 to get size in KBits. Instead we divide
 * the ticks parameter (which is much bigger) by 1000.
 *
 * \see - None
 */
shr_error_e dnx_algo_rates_kbits_per_sec_to_clocks(
    int unit,
    uint32 rate,
    uint32 credit,
    uint32 *interval);

/**
 * \brief -
 *  Convert an interval between 2 consecutive credits given in
 * device clocks to rate in Kbits/sec units.
 *
 * The conversion is done according to:
 *                       Credit [Kbits] * Num_of_clocks_in_sec [clocks/sec]
 *  Rate [Kbits/Sec] =   -------------------------------------------------------------
 *                          interval_between_credits_in_clocks [clocks]
 *  \param [in] unit -
 *    The unit id.
 *  \param [in] interval -
 *      Interval between credits [clocks].
 *  \param [in] credit -
 *    The credit size [Bytes].
 *  \param [out] rate -
 *      The rate [KBits/Sec].
 * \note
 * For rounding issues we don't actually divide the credit
 * size by 1000 to get size in KBits. Instead we divide
 * the ticks parameter (which is much bigger) by 1000.
 * \see - None
 */
shr_error_e dnx_algo_rates_clocks_to_kbits_per_sec(
    int unit,
    uint32 interval,
    uint32 credit,
    uint32 *rate);

/**
 * \brief
 *   The shaper rate is determind by 'credit' and 'interval':
 *
 *                        Credit [Bytes] * Num_of_clocks_in_sec [Kclocks/sec]
 *   Rate [KBytes/Sec] = -----------------------------------------------------
 *                        Interval_between_credits_in_clocks [clocks]
 *
 *   This function converts shaper rate given in kbits_per_sec to the credit
 *   and interval that compose it:
 *   1. interval: Time interval to add the credit value to the shaper balance.
 *   2. credit: Credit value to add to the shaper balance.
 * 
 *  \param [in] unit -
 *    The unit number.
 *  \param [in] shaper_rate -
 *    The shaper's max rate [KBytes/Sec].
 *  \param [in] shaper_bounds -
 *    Shpaer's params boundaries (HW bounds).
 *  \param [in] interval_resolution -
 *    Clocks resolution of the interval param [clocks].
 *  \param [in] credit_resolution -
 *    Bytes resolution of the credit param [Bytes].
 *  \param [out] shaper_interval -
 *    The shaper's interval part [clocks*interval_resolution].
 *    This is the value that should be set to HW.
 *  \param [out] shaper_credit -
 *    The shaper's credit part [Bytes].
 *    This is the value that should be set to HW.
 *  \retval
 *    See \ref shr_error_e
 *  \remark
 *    * Pay attention to the [units] of each param.
 *    * The returned shaper_interval is in interval_resolution units resolution.
 *  \see
 *    None.
 */
shr_error_e dnx_algo_rates_rate_to_credit_interval(
    int unit,
    uint32 shaper_rate,
    dnx_algo_rates_shaper_bounds_t * shaper_bounds,
    uint32 interval_resolution,
    uint32 credit_resolution,
    uint32 *shaper_interval,
    uint32 *shaper_credit);

/**
 * \brief
 *   The shaper rate is determind by 'credit' and 'interval':
 *
 *                        Credit [Bytes] * Num_of_clocks_in_sec [Kclocks/sec]
 *   Rate [KBytes/Sec] = -----------------------------------------------------
 *                        Interval_between_credits_in_clocks [clocks]
 *
 *   This function converts credit and interval to a rate in kbits_per_sec.
 *   1. interval: Time interval to add the credit value to the shaper balance.
 *   2. credit: Credit value to add to the shaper balance.
 * 
 *  \param [in] unit -
 *    The unit number.
 *  \param [in] credit_resolution -
 *    Bytes resolution of the credit param [Bytes].
 *  \param [in] interval_resolution -
 *    Clocks resolution of the interval param [clocks].
 *  \param [in] shaper_interval -
 *    The shaper's interval part [clocks].
 *    This is the value that retreived from HW.
 *  \param [in] shaper_credit -
 *    The shaper's credit part [Bytes].
 *    This is the value that retreived from HW.
 *  \param [out] shaper_rate -
 *    The shaper's max rate [KBytes/Sec].
 *  \retval
 *    See \ref shr_error_e
 *  \remark
 *    * Pay attention to the [units] of each param.
 *    * The actual interval that will be used to compute the rate is
 *      shaper_interval*interval_resolution.
 *  \see
 *    None.
 */
shr_error_e dnx_algo_rates_credit_interval_to_rate(
    int unit,
    uint32 credit_resolution,
    uint32 interval_resolution,
    uint32 shaper_interval,
    uint32 shaper_credit,
    uint32 *shaper_rate);

/*
 * } 
 */

#endif /* __ALGO_RATES_H_INCLUDED__ */
