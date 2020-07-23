/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/**
 * \file algo_cal.h
 * Reserved.$ 
 */

#ifndef _DNX_ALGO_CAL__H__INCLUDED_
#define _DNX_ALGO_CAL__H__INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_error.h>
#include <bcm/types.h>
#include <bcm_int/dnx/cosq/egress/egq_ofp_rates.h>

#define DNX_ALGO_CAL_ILLEGAL_OBJECT_ID  (0xffffffff)

/* 
 * \brief -
 *   Given calendar type, rate per each active port, and length of calendar, get
 *   the corresponding calendar with 'calendar_len' slots (but see 'add_dummy_tail').
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] core -
 *   Core to use for getting required info..
 * \param [in] cal_info -
 *   Pointer to dnx_ofp_rates_cal_info_t. The following elements are input:
 *     cal_info->cal_type (dnx_ofp_rates_egq_cal_type_e) - May be:
 *       DNX_OFP_RATES_EGQ_CAL_CHAN_ARB
 *       All other calendar types
 *   The following are also used,  but only for 'channelized calendar' (CAL_CHAN_ARB):
 *     cal_info->chan_arb_id (used to calculate 'offest', key into table).
 * \param [in] ports_rates -
 *   Pointer to array of uint32. Dimension of array is dnx_data_egr_queuing.params.nof_q_pairs
 *   (currently 512). Each entry indicates the rate assigned to indicated port, in kilo
 *   bits per second. A port, whose rate is '0', is assumed to be inactive.
 * \param [in] calendar_len -
 *   uint32. Number of slots in calendar. Each slot refers to one port.
 * \param [in] add_dummy_tail -
 *   uint8. Flag. If set then number of slots, in 'calendar', loaded by rate, is
 *   not 'calendar_len' but 'calendar_len - 1'.
 * \param [in,out] calendar -
 *   Pointer to dnx_ofp_rates_cal_egq_t. 'calendar' is made out of an array of 'slots'. Each
 *   slot is represented by two parameters: 'base qpair' (= port index) and its corresponding number
 *   of 'credit's that the system is supposed to supply to that port when its turn comes.
 *   Of these two parameters, 'base qpair' is INPUT and 'credit' is OUTPUT of this procedure.
 *   Caller is responsible to set 'calendar' with enough space to accommodate 'calendar_length'
 *   slots.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_ofp_rates_from_egq_ports_rates_to_generic_calendar
 */
int dnx_algo_ofp_rates_fill_shaper_generic_calendar_credits(
    int unit,
    int core,
    dnx_ofp_rates_cal_info_t * cal_info,
    uint32 *ports_rates,
    /*
     * Actual Calendar length
     */
    uint32 calendar_len,
    /*
     * Indicate if last entry is dummy or not
     */
    uint8 add_dummy_tail,
    dnx_ofp_rates_cal_egq_t * calendar);
/**
 * \brief
 * build calendar for provided slotes-per-object and provided calendar length.
 * 
 * The algorith works as the following.
 * 
 * Each time algoritm selects object with maximal number of slots. It allocates slots for this object using
 * two sizes of hopes.
 * The hope sizes are : (cal_len/nof_slots_of_max_object + 1) and (cal_len/nof_slots_of_max_object).
 * This results in calendar as spaced as possible without having any leftover while still going over all the calendar.
 *
 * \param [in] unit     - unit id
 * \param [in] slots_per_object - number of slots to allocate to each object
 * \param [in] nof_objects - total number of objects - size of slots_per_object
 * \param [in] calendar_len - length of the required calendar
 * \param [in] max_calendar_len - maximal possible calendar length
 * \param [out] calendar - the result calendar
 * 
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   
 * \see
 *   * None
 */
shr_error_e dnx_algo_cal_simple_fixed_len_cal_build(
    int unit,
    uint32 *slots_per_object,
    uint32 nof_objects,
    uint32 calendar_len,
    uint32 max_calendar_len,
    uint32 *calendar);

/**
 * \brief
 * build best calendar for provided object rates, total bandwidth and maximal calendar length
 *
 * \param [in] unit     - unit id
 * \param [in] rates_per_object - rate of each object port
 * \param [in] nof_objects - number of objects - length of rates_per_object
 * \param [in] total_bandwidth - total bandwidth of the whole calendar
 * \param [in] max_calendar_len - maximal possible calendar length
 * \param [out] calendar_slots - the result calendar
 * \param [out] calendar_len - the length of the result calendar
 * 
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   
 * \see
 *   * None
 */
shr_error_e dnx_algo_cal_simple_from_rates_to_calendar(
    int unit,
    uint32 *rates_per_object,
    uint32 nof_objects,
    uint32 total_bandwidth,
    uint32 max_calendar_len,
    uint32 *calendar_slots,
    uint32 *calendar_len);

/**
 * \brief
 * get rate of the provided object (object_id) in the provided calendar
 *
 * \param [in] unit     - unit id
 * \param [in] calendar - the input calendar
 * \param [in] calendar_len - the length of the calendar
 * \param [in] total_cal_rate - total bandwidth of the whole calendar
 * \param [in] object_id - object ID
 * \param [out] object_rate - the rate of the object in the calendar
 * 
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   
 * \see
 *   * None
 */
shr_error_e dnx_algo_cal_simple_object_rate_get(
    int unit,
    uint32 *calendar,
    uint32 calendar_len,
    uint32 total_cal_rate,
    int object_id,
    uint32 *object_rate);

#endif
