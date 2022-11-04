/** \file algo_cal.c
 *  
 *  Calendar calculation
 *  
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <shared/utilex/utilex_u64.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_ofp_rate_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>

#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/cal/algo_cal.h>
#include <bcm_int/dnx/algo/flexe_general/algo_flexe_general.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/init/init_time_analyzer.h>
#include <math.h>

#define DNX_ALGO_CAL_RATE_DEVIATION     (100000000)

typedef struct
{
    int size;
    int nof_slots;
    uint32 *members;
} dnx_algo_cal_list_t;

shr_error_e dnx_algo_cal_adjust(
    int unit,
    int nof_objects,
    int calendar_len,
    uint32 *calendar);

/**
 * see algo_cal.h
 */
int
dnx_algo_cal_u64_to_long(
    int unit,
    uint64 u64,
    uint32 *u32)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Validate no overflow from 32-bit 
     */
    if (COMPILER_64_HI(u64) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "overflow from 32-bit\n");
    }

    *u32 = COMPILER_64_LO(u64);

exit:
    SHR_FUNC_EXIT;
}

/**
 * see algo_cal.h
 *
 */
int
dnx_algo_cal_u64_div_long_and_round_up(
    int unit,
    uint64 dividend,
    uint32 divisor,
    uint64 *result)
{
    SHR_FUNC_INIT_VARS(unit);
    COMPILER_64_ADD_32(dividend, divisor);
    COMPILER_64_SUB_32(dividend, 1);
    COMPILER_64_UDIV_32(dividend, divisor);

    COMPILER_64_COPY(*result, dividend);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * divide unsigned 64 by unsigned 64 and round up
 *
 */
static int
dnx_algo_cal_u64_div_u64_and_round_up(
    int unit,
    uint64 dividend,
    uint64 divisor,
    uint64 *result)
{
    SHR_FUNC_INIT_VARS(unit);
    COMPILER_64_ADD_64(dividend, divisor);
    COMPILER_64_SUB_32(dividend, 1);
    COMPILER_64_UDIV_64(dividend, divisor);

    COMPILER_64_COPY(*result, dividend);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * multiply unsigned 64 by unsigned 32. Returns as unsigned 64
 *
 */
static int
dnx_algo_cal_u64_multiply_longs(
    int unit,
    uint64 multiplier,
    uint32 multiplicand,
    uint64 *result)
{
    uint64 temp_calc;
    SHR_FUNC_INIT_VARS(unit);
    COMPILER_64_COPY(temp_calc, multiplier);
    COMPILER_64_UMUL_32(temp_calc, multiplicand);

    COMPILER_64_COPY(*result, temp_calc);
    SHR_FUNC_EXIT;
}

/*
 * See algo_cal.h
 */
int
dnx_algo_ofp_rates_fill_shaper_generic_calendar_credits(
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
    dnx_ofp_rates_cal_egq_t * calendar)
{
    uint32 slot_idx, port_idx;
    uint32 *port_credits_p = NULL;
    uint32 temp_calendar_length, dummy_tail_entry, calcal_length, calcal_instances;
    int packet_mode;
    int base_q_pair, nof_priorities;
    bcm_port_t logical_port;
    bcm_pbmp_t port_bm;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ports_rates, BCM_E_PARAM, "ports_rates");
    SHR_NULL_CHECK(calendar, BCM_E_PARAM, "calendar");

    SHR_ALLOC_SET_ZERO(port_credits_p, sizeof(*port_credits_p) * dnx_data_egr_queuing.params.nof_q_pairs_get(unit),
                       "port_credits_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    dummy_tail_entry = ((add_dummy_tail == FALSE) ? 0x0 : 0x1);
    /*
     * In case of dummy tail, all slots are taken into account without it
     */
    temp_calendar_length = calendar_len - dummy_tail_entry;
    if (cal_info->cal_type == DNX_OFP_RATES_EGQ_CAL_CHAN_ARB)
    {
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.calcal_len.get(unit, core, &calcal_length));
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.
                        nof_calcal_instances.get(unit, core, cal_info->chan_arb_id, &calcal_instances));
    }
    else
    {
        /*
         * When no calcal to be taken into consideration. assume lengths are 1
         */
        calcal_length = 1;
        calcal_instances = 1;
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING, 0, &port_bm));

    BCM_PBMP_ITER(port_bm, logical_port)
    {
        /*
         * From the logical port get the associated shaper mode (DATA or PACKET), the base Q-pair and number of priorities
         */
        SHR_IF_ERR_EXIT(dnx_egr_queuing_port_packet_mode_get(unit, logical_port, &packet_mode));
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
        SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &nof_priorities));
        for (port_idx = base_q_pair; port_idx < (base_q_pair + nof_priorities); port_idx++)
        {
            uint64 port_rate64;

            if (!ports_rates[port_idx])
            {
                continue;
            }

            COMPILER_64_SET(port_rate64, 0, ports_rates[port_idx]);
            SHR_IF_ERR_EXIT(dnx_ofp_rates_calculate_credits_from_rate
                            (unit, calcal_length, calcal_instances, calendar_len, (uint32) packet_mode, port_rate64,
                             &(port_credits_p[port_idx])));
        }
    }
    for (slot_idx = 0; slot_idx < temp_calendar_length; ++slot_idx)
    {
        port_idx = calendar->slots[slot_idx].base_q_pair;
        calendar->slots[slot_idx].credit = port_credits_p[port_idx];
    }
exit:
    SHR_FREE(port_credits_p);
    SHR_FUNC_EXIT;
}
/**
 * \brief -
 * for calendar of length tentative_len
 * calculate number of slots for each object in the calendar and total deviation of the result calendar
 */
static int
dnx_algo_cal_simple_per_len_build(
    int unit,
    uint64 *rates_per_object,
    uint32 nof_objects,
    uint64 total_bandwidth,
    uint32 max_calendar_len,
    uint32 tentative_len,
    uint32 *actual_len,
    uint32 *slots_per_object_p,
    uint32 *deviation)
{
    uint32 obj_id, total_num_slots = 0, calc_deviation;
    uint32 *slots_rates_p = NULL;
    uint32 num_slots = 0, temp1 = 0, temp2 = 0;
    uint64 temp_calc;
    uint64 zero = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(slots_rates_p, sizeof(*slots_rates_p) * nof_objects, "slots_rates_p", "%s%s%s", EMPTY, EMPTY,
                       EMPTY);

    /*
     * Calculate the number of calendar slots per object
     */
    for (obj_id = 0; obj_id < nof_objects; ++obj_id)
    {
        if (rates_per_object[obj_id] > 0)
        {
            SHR_IF_ERR_EXIT(dnx_algo_cal_u64_multiply_longs(unit, rates_per_object[obj_id], tentative_len, &temp_calc));
            SHR_IF_ERR_EXIT(dnx_algo_cal_u64_div_u64_and_round_up(unit, temp_calc, total_bandwidth, &temp_calc));
            SHR_IF_ERR_EXIT(dnx_algo_cal_u64_to_long(unit, temp_calc, &num_slots));

             /** in case num_slots is zero increase to '1' */
            num_slots = (num_slots == 0 ? 1 : num_slots);

            slots_rates_p[obj_id] = num_slots;
            total_num_slots += num_slots;
        }
    }
    if (total_num_slots > max_calendar_len || (COMPILER_64_EQ(total_bandwidth, zero)) || total_num_slots == 0)
    {
        /** This solution is not acceptable, so return zero */
        *actual_len = 0;
        sal_memset(slots_per_object_p, 0, sizeof(*slots_per_object_p) * nof_objects);
    }
    else
    {

        calc_deviation = 0;
        for (obj_id = 0; obj_id < nof_objects; ++obj_id)
        {

            SHR_IF_ERR_EXIT(dnx_algo_cal_u64_multiply_longs
                            (unit, DNX_ALGO_CAL_RATE_DEVIATION, rates_per_object[obj_id], &temp_calc));
            SHR_IF_ERR_EXIT(dnx_algo_cal_u64_div_u64_and_round_up(unit, temp_calc, total_bandwidth, &temp_calc));
            SHR_IF_ERR_EXIT(dnx_algo_cal_u64_to_long(unit, temp_calc, &temp1));

            SHR_IF_ERR_EXIT(dnx_algo_cal_u64_multiply_longs
                            (unit, DNX_ALGO_CAL_RATE_DEVIATION, slots_rates_p[obj_id], &temp_calc));
            SHR_IF_ERR_EXIT(dnx_algo_cal_u64_div_long_and_round_up(unit, temp_calc, total_num_slots, &temp_calc));
            SHR_IF_ERR_EXIT(dnx_algo_cal_u64_to_long(unit, temp_calc, &temp2));

            calc_deviation += utilex_abs(temp2 - temp1);
        }

        *actual_len = total_num_slots;
        sal_memcpy(slots_per_object_p, slots_rates_p, sizeof(*slots_rates_p) * nof_objects);
        *deviation = calc_deviation;
    }
exit:
    SHR_FREE(slots_rates_p);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * find calendar length which provides best deviation 
 * and calculate number of slots for each obj_id in this calendar
 */
static int
dnx_algo_cal_simple_len_calculate(
    int unit,
    uint64 *rates_per_object,
    uint32 nof_objects,
    uint64 total_bandwidth,
    uint32 max_calendar_len,
    uint32 *actual_calendar_len,
    uint32 *slots_per_object)
{
    uint32 deviation, min_deviation, tentative_len, best_tentative_len, actual_len;
    uint32 *slots_per_object_temp_p = NULL;

    /** variables for final calendar calculation */
    uint64 *rates_per_object_final;
    uint32 nof_objects_final;
    uint64 total_bandwidth_final;
    uint64 min_rate;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC_ERR_EXIT(slots_per_object_temp_p, sizeof(*slots_per_object_temp_p) * nof_objects,
                       "slots_per_object_temp_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    min_deviation = DNX_ALGO_CAL_RATE_DEVIATION;
    best_tentative_len = 0;
    for (tentative_len = 1; tentative_len <= max_calendar_len; ++tentative_len)
    {
        SHR_IF_ERR_EXIT(dnx_algo_cal_simple_per_len_build(unit,
                                                          rates_per_object, nof_objects,
                                                          total_bandwidth, max_calendar_len,
                                                          tentative_len, &actual_len, slots_per_object_temp_p,
                                                          &deviation));

        /*
         * Check if we received a legal solution for this tentative length
         */
        if (actual_len == 0)
        {
            continue;
        }
        if (deviation < min_deviation)
        {
            min_deviation = deviation;
            best_tentative_len = tentative_len;
            if (0 == min_deviation)
            {
                break;
            }
        }
    }
    /*
     * sanity check
     */
    if (0 == best_tentative_len)
    {

        /*
         * Calendar length is 0, means that all ports' bandwidth is 0.
         * calculate calendar with minimal parameters just to have something to configure
         */

        COMPILER_64_SET(min_rate, 0, 1);
        rates_per_object_final = &min_rate;
        nof_objects_final = 1;
        COMPILER_64_SET(total_bandwidth_final, 0, 1);
        best_tentative_len = 1;

    }
    else
    {
        rates_per_object_final = rates_per_object;
        nof_objects_final = nof_objects;
        COMPILER_64_COPY(total_bandwidth_final, total_bandwidth);
    }

    /*
     * Rebuild the best calendar that we found
     */
    SHR_IF_ERR_EXIT(dnx_algo_cal_simple_per_len_build(unit,
                                                      rates_per_object_final, nof_objects_final,
                                                      total_bandwidth_final, max_calendar_len,
                                                      best_tentative_len, &actual_len, slots_per_object_temp_p,
                                                      &deviation));

    *actual_calendar_len = actual_len;
    sal_memcpy(slots_per_object, slots_per_object_temp_p, sizeof(uint32) * nof_objects);

exit:
    SHR_FREE(slots_per_object_temp_p);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Build fixed length calendar 
 * when all big hopes are configured before all small hopes
 *
 * The algorith works as the following.
 * 
 * Each time algoritm selects object with maximal number of slots. It allocates slots for this object using
 * two sizes of hopes.
 * The hope sizes are : (cal_len/nof_slots_of_max_object + 1) and (cal_len/nof_slots_of_max_object).
 * This results in calendar as spaced as possible without having any leftover while still going over all the calendar.
 *
 */
shr_error_e
dnx_algo_cal_fixed_len_big_hope_first_build(
    int unit,
    uint32 *slots_per_object,
    uint32 nof_objects,
    uint32 calendar_len,
    uint32 max_calendar_len,
    uint32 *calendar)
{
    uint32 slot_idx;
    uint32 *slots_per_object_temp_p = NULL;

    uint32 alloc_slots, rem_cal_len, max_obj_idx;
    uint32 hop_size = 0, hop_size_small = 0, nof_big_hops = 0, obj_alloc_slots, free_slot_cnt;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(slots_per_object_temp_p, sizeof(*slots_per_object_temp_p) * nof_objects, "slots_per_object_temp_p",
              "%s%s%s", EMPTY, EMPTY, EMPTY);

    /** Clear the calendar */
    for (slot_idx = 0; slot_idx < max_calendar_len; ++slot_idx)
    {
        calendar[slot_idx] = DNX_ALGO_CAL_ILLEGAL_OBJECT_ID;
    }

    sal_memcpy(slots_per_object_temp_p, slots_per_object, sizeof(*slots_per_object_temp_p) * nof_objects);

    /**  For each object, try to allocate slots using 2 hop sizes*/
    alloc_slots = 0;
    rem_cal_len = calendar_len;
    max_obj_idx = utilex_get_index_of_max_member_in_array(slots_per_object_temp_p, nof_objects);
    while (slots_per_object_temp_p[max_obj_idx] > 0)
    {
        /** used as big hop size for the alternate algorithm  */
        hop_size = rem_cal_len / slots_per_object_temp_p[max_obj_idx];

        /*
         * calculating small skip for alternate algorithm 
         */
        hop_size_small = hop_size - 1;

        nof_big_hops = rem_cal_len - slots_per_object[max_obj_idx] * hop_size;
        /** if nof big hops == 0, we use small hops only */
        if (nof_big_hops == 0)
        {
            hop_size = hop_size_small;
        }
        obj_alloc_slots = 0;
        free_slot_cnt = hop_size;
        for (slot_idx = 0; slot_idx < calendar_len && obj_alloc_slots < slots_per_object[max_obj_idx]; ++slot_idx)
        {
            if (calendar[slot_idx] == DNX_ALGO_CAL_ILLEGAL_OBJECT_ID)
            {
                if (free_slot_cnt < hop_size)
                {
                    ++free_slot_cnt;
                }
                else
                {
                    calendar[slot_idx] = max_obj_idx;
                    ++alloc_slots;
                    ++obj_alloc_slots;
                    free_slot_cnt = 0;
                    /*
                     * in the alt algo, we use big hops until we reach nof_big_hops == 0, then we move to small hops 
                     * that way we spread the max port on the calendar as much as possible
                     */
                    if (nof_big_hops <= 0)
                    {
                        hop_size = hop_size_small;
                    }
                    else
                    {
                        nof_big_hops--;
                    }
                }
            }
        }

        slots_per_object_temp_p[max_obj_idx] = 0;
        rem_cal_len -= obj_alloc_slots;

        /*
         *  Make sure all slots are allocated and no leftovers remained
         */
        if (slots_per_object[max_obj_idx] != obj_alloc_slots)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Calendar calculation failed - not all slots are allocated\n");
        }

        max_obj_idx = utilex_get_index_of_max_member_in_array(slots_per_object_temp_p, nof_objects);
    }

exit:
    SHR_FREE(slots_per_object_temp_p);
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_cal_fixed_len_ratio_based_build(
    int unit,
    uint32 *slots_per_object,
    int nof_objects,
    int calendar_len,
    int max_calendar_len,
    uint32 *calendar)
{
    uint32 slot_idx, *slots_per_object_temp = NULL;
    uint32 choose_slots, other_slots, max_slots;
    int max_slots_obj, choose_ratio, other_ratio, allocated_slots;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Clear the calendar
     */
    for (slot_idx = 0; slot_idx < max_calendar_len; ++slot_idx)
    {
        calendar[slot_idx] = DNX_ALGO_CAL_ILLEGAL_OBJECT_ID;
    }
    SHR_ALLOC_ERR_EXIT(slots_per_object_temp, nof_objects * sizeof(uint32), "slots_per_object_temp", "%s%s%s\r\n",
                       EMPTY, EMPTY, EMPTY);
    sal_memcpy(slots_per_object_temp, slots_per_object, sizeof(uint32) * nof_objects);
    /** Get the object index which needs max slots */
    max_slots_obj = utilex_get_index_of_max_member_in_array(slots_per_object_temp, nof_objects);

    choose_slots = slots_per_object_temp[max_slots_obj];
    max_slots = calendar_len;
    other_slots = max_slots - choose_slots;

    while (choose_slots > 0)
    {
        /*
         * Get the ratio for choose slots and other slots.
         *    For example:
         *        Calendar length: 8, Choose slots: 6, other slots: 2
         *        The choose ratio is 3 and other ratio is 1.
         */
        allocated_slots = 0;
        choose_ratio = (other_slots > choose_slots) ? 1 : ((other_slots > 0) ?
                                                           UTILEX_DIV_ROUND(choose_slots, other_slots) : 1);
        other_ratio = (other_slots == 0) ? 0 : ((other_slots > choose_slots) ? (other_slots / choose_slots) : 1);

        for (slot_idx = 0; slot_idx < calendar_len; ++slot_idx)
        {
            if (calendar[slot_idx] == -1)
            {
                if (choose_ratio > 0)
                {
                    /*
                     * Allocate slots for choose_slots first.
                     */
                    calendar[slot_idx] = max_slots_obj;
                    --choose_slots;
                    --choose_ratio;
                    ++allocated_slots;
                }
                else if ((choose_ratio == 0) && (other_ratio > 0))
                {
                    /*
                     * If finishing allocating slots for choose slots,
                     * keep the slots position for other_slots.
                     */
                    --other_ratio;
                    --other_slots;
                }
                if ((choose_slots == 0) && (other_slots == 0))
                {
                    /*
                     * Finish allocating slots for the current object.
                     */
                    break;
                }
                if (choose_ratio == 0 && other_ratio == 0)
                {
                    /*
                     * Re-calculate the choose slots after keeping the
                     * position for other slots.
                     */
                    choose_ratio = (other_slots > choose_slots) ? 1 :
                        ((other_slots > 0) ? UTILEX_DIV_ROUND(choose_slots, other_slots) : 1);
                    /*
                     * Coverity:
                     * If the choose_slots is "0", it means all the slots have been allocated, so
                     * the other_slots should also be "0".
                     */
                     /* coverity[divide_by_zero : FALSE]  */
                    other_ratio = (other_slots == 0) ? 0 :
                        ((other_slots > choose_slots) ? (other_slots / choose_slots) : 1);
                }
            }
        }
        if (slots_per_object_temp[max_slots_obj] != allocated_slots)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Not all slots are allocated.\n");
        }
        /*
         * Remove the object whose slots have been allocated.
         */
        max_slots -= slots_per_object_temp[max_slots_obj];
        slots_per_object_temp[max_slots_obj] = 0;
        /*
         * Get the next oject that occupied the MAX slots.
         */
        max_slots_obj = utilex_get_index_of_max_member_in_array(slots_per_object_temp, nof_objects);
        choose_slots = slots_per_object_temp[max_slots_obj];
        other_slots = max_slots - choose_slots;
    }
exit:
    SHR_FREE(slots_per_object_temp);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_cal_fixed_len_ratio_based_build_cb(
    int unit,
    dnx_algo_cal_object_t * object_info,
    int nof_objects,
    int calendar_len,
    int max_calendar_len,
    void *addition_info,
    uint32 *calendar)
{
    uint32 *slots_per_object = NULL;
    int i;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(slots_per_object, sizeof(*slots_per_object) * nof_objects, "slots_per_object", "%s%s%s", EMPTY,
              EMPTY, EMPTY);
    for (i = 0; i < nof_objects; i++)
    {
        slots_per_object[i] = object_info[i].nof_slots;
    }

    SHR_IF_ERR_EXIT(dnx_algo_cal_fixed_len_ratio_based_build(unit, slots_per_object, nof_objects,
                                                             calendar_len, max_calendar_len, calendar));
exit:
    SHR_FREE(slots_per_object);
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_algo_cal_group_and_build(
    int unit,
    dnx_algo_cal_build_func_t build_cb,
    uint32 *slots_per_object,
    int nof_objects,
    int calendar_len,
    int max_calendar_len,
    void *additional_info,
    uint32 *calendar)
{

    int nof_groups = 0;
    int group_id;

    dnx_algo_cal_object_t *slots_per_group = NULL;
    int *group_per_slot = NULL;
    dnx_algo_cal_list_t *ids_per_group = NULL;

    int i, nof_slots;
    int slot_idx;
    uint32 *group_calendar = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_ERR_EXIT(group_per_slot, sizeof(*group_per_slot) * (calendar_len + 1), "group_per_slot",
                       "%s%s%s", EMPTY, EMPTY, EMPTY);

    for (i = 0; i <= calendar_len; i++)
    {
        group_per_slot[i] = -1;
    }

    /*
     * we replace objects with the same number of slots by a grouped object
     * we manage what IDs belonf to each group
     */
    SHR_ALLOC_ERR_EXIT(ids_per_group, sizeof(*ids_per_group) * nof_objects, "ids_per_group", "%s%s%s",
                       EMPTY, EMPTY, EMPTY);

    sal_memset(ids_per_group, 0, sizeof(*ids_per_group) * nof_objects);

    SHR_ALLOC_ERR_EXIT(group_calendar, sizeof(*group_calendar) * max_calendar_len, "group_calendar",
                       "%s%s%s", EMPTY, EMPTY, EMPTY);

    sal_memset(group_calendar, 0, sizeof(*group_calendar) * max_calendar_len);

    for (i = 0; i < nof_objects; i++)
    {
        nof_slots = slots_per_object[i];
        if (nof_slots == 0)
        {
            continue;
        }
        if (group_per_slot[nof_slots] == -1)
        {
            SHR_ALLOC_ERR_EXIT(ids_per_group[nof_groups].members, sizeof(uint32) * nof_objects, "members", "%s%s%s",
                               EMPTY, EMPTY, EMPTY);

            sal_memset(ids_per_group[nof_groups].members, 0, sizeof(uint32) * nof_objects);

            /** map nof_slots -> group ID */
            group_per_slot[nof_slots] = nof_groups;
            /** map group ID -> nof_slots */
            ids_per_group[nof_groups].nof_slots = nof_slots;

            nof_groups++;
        }
        group_id = group_per_slot[nof_slots];
        /** Add the object to the mebers list */
        ids_per_group[group_id].members[ids_per_group[group_id].size] = i;
        ids_per_group[group_id].size++;
    }

    if (group_per_slot[1] != -1)
    {
        int single_slot_group_id;

        /** there is a group of single slot objects */
        single_slot_group_id = group_per_slot[1];
        for (i = ids_per_group[single_slot_group_id].size - 1; i > 0; i--)
        {
            SHR_ALLOC_ERR_EXIT(ids_per_group[nof_groups].members, sizeof(uint32), "members", "%s%s%s",
                               EMPTY, EMPTY, EMPTY);

            sal_memset(ids_per_group[nof_groups].members, 0, sizeof(uint32));

            group_id = nof_groups;
            ids_per_group[group_id].nof_slots = 1;
            ids_per_group[group_id].size = 1;
            ids_per_group[group_id].members[0] = ids_per_group[single_slot_group_id].members[i];
            ids_per_group[single_slot_group_id].size--;
            nof_groups++;

        }
    }

    SHR_ALLOC_ERR_EXIT(slots_per_group, sizeof(*slots_per_group) * nof_groups, "slots_per_group",
                       "%s%s%s", EMPTY, EMPTY, EMPTY);

    for (group_id = 0; group_id < nof_groups; group_id++)
    {
        slots_per_group[group_id].nof_slots = ids_per_group[group_id].size * ids_per_group[group_id].nof_slots;
        slots_per_group[group_id].id = group_id;
        slots_per_group[group_id].orig_id = ids_per_group[group_id].members[0];
        slots_per_group[group_id].orig_nof_slots = ids_per_group[group_id].nof_slots;
    }

    SHR_IF_ERR_EXIT(build_cb(unit, slots_per_group, nof_groups, calendar_len, max_calendar_len, additional_info,
                             group_calendar));

    /** Clear the calendar */
    for (slot_idx = 0; slot_idx < max_calendar_len; ++slot_idx)
    {
        calendar[slot_idx] = DNX_ALGO_CAL_ILLEGAL_OBJECT_ID;
    }

    for (group_id = 0; group_id < nof_groups; group_id++)
    {
        i = 0;

        for (slot_idx = 0; slot_idx < calendar_len; slot_idx++)
        {
            if (group_calendar[slot_idx] != group_id)
            {
                continue;
            }
            calendar[slot_idx] = ids_per_group[group_id].members[i];
            i = (i + 1) % ids_per_group[group_id].size;
        }
    }

exit:
    SHR_FREE(group_per_slot);
    SHR_FREE(group_calendar);
    if (ids_per_group != NULL)
    {

        for (i = 0; i < nof_objects; i++)
        {
            SHR_FREE(ids_per_group[i].members);
        }
    }
    SHR_FREE(slots_per_group);
    SHR_FREE(ids_per_group);

    SHR_FUNC_EXIT;
}

uint32
dnx_algo_cal_index_of_max_nof_slots_get(
    dnx_algo_cal_object_t array[],
    uint32 len)
{
    uint32 index, index_of_max, max_val;

    index_of_max = 0;
    max_val = 0;

    for (index = 0; index < len; index++)
    {
        if (array[index].nof_slots > max_val)
        {
            max_val = array[index].nof_slots;
            index_of_max = index;
        }
    }
    return index_of_max;
}

shr_error_e
dnx_algo_cal_recursive_build_and_validate(
    int unit,
    dnx_algo_cal_build_func_t build_cb,
    dnx_algo_cal_object_t * slots_per_object,
    int nof_objects,
    int calendar_len,
    int max_calendar_len,
    void *additional_info,
    int do_validate,
    uint32 *calendar)
{

    dnx_algo_cal_object_t *slots_per_object_sub_cal = NULL;
    dnx_algo_cal_object_t select_slots[2];

    uint32 *objects_sub_cal = NULL;

    uint32 *sub_calendar = NULL, *select_calendar = NULL;

    int total_slots_sub_cal = 0;
    int nof_objects_sub_cal = 0;
    int i;
    int slot_idx, curr_sub_calendar;
    int select_index;

    SHR_FUNC_INIT_VARS(unit);

    if (nof_objects > 2)
    {
        select_index = dnx_algo_cal_index_of_max_nof_slots_get(slots_per_object, nof_objects);

        SHR_ALLOC_ERR_EXIT(objects_sub_cal, sizeof(uint32) * nof_objects, "objects_sub_cal", "%s%s%s",
                           EMPTY, EMPTY, EMPTY);

        SHR_ALLOC_ERR_EXIT(slots_per_object_sub_cal, sizeof(dnx_algo_cal_object_t) * nof_objects,
                           "slots_per_object_sub_cal", "%s%s%s", EMPTY, EMPTY, EMPTY);

        for (i = 0; i < nof_objects; i++)
        {
            if (i != select_index)
            {
                objects_sub_cal[nof_objects_sub_cal] = i;
                slots_per_object_sub_cal[nof_objects_sub_cal].nof_slots = slots_per_object[i].nof_slots;
                slots_per_object_sub_cal[nof_objects_sub_cal].id = nof_objects_sub_cal;
                slots_per_object_sub_cal[nof_objects_sub_cal].orig_id = nof_objects_sub_cal;
                slots_per_object_sub_cal[nof_objects_sub_cal].orig_nof_slots = slots_per_object[i].nof_slots;
                nof_objects_sub_cal++;
            }
        }
        total_slots_sub_cal = calendar_len - slots_per_object[select_index].nof_slots;

        SHR_ALLOC_ERR_EXIT(sub_calendar, sizeof(uint32) * total_slots_sub_cal, "sub_calendar", "%s%s%s",
                           EMPTY, EMPTY, EMPTY);

        SHR_IF_ERR_EXIT(dnx_algo_cal_recursive_build_and_validate(unit, build_cb, slots_per_object_sub_cal,
                                                                  nof_objects_sub_cal, total_slots_sub_cal,
                                                                  total_slots_sub_cal, additional_info,
                                                                  do_validate, sub_calendar));
        SHR_ALLOC_ERR_EXIT(select_calendar, sizeof(uint32) * calendar_len, "select_calendar",
                           "%s%s%s", EMPTY, EMPTY, EMPTY);

        select_slots[0].id = 0;
        select_slots[0].orig_id = 0;
        select_slots[0].nof_slots = total_slots_sub_cal;
        select_slots[0].orig_nof_slots = total_slots_sub_cal;
        select_slots[1].id = 1;
        select_slots[1].orig_id = 1;
        select_slots[1].nof_slots = slots_per_object[select_index].nof_slots;
        select_slots[1].orig_nof_slots = slots_per_object[select_index].nof_slots;
        SHR_IF_ERR_EXIT(build_cb(unit, select_slots, 2, calendar_len, calendar_len, additional_info, select_calendar));

        curr_sub_calendar = total_slots_sub_cal - 1;

        /** Clear the calendar */
        for (slot_idx = 0; slot_idx < max_calendar_len; ++slot_idx)
        {
            calendar[slot_idx] = DNX_ALGO_CAL_ILLEGAL_OBJECT_ID;
        }

        for (slot_idx = 0; slot_idx < calendar_len; ++slot_idx)
        {
            if (select_calendar[slot_idx] == 0)
            {
                if (sub_calendar[curr_sub_calendar] != -1)
                {
                    calendar[slot_idx] = objects_sub_cal[sub_calendar[curr_sub_calendar]];
                }
                else
                {
                    calendar[slot_idx] = -1;
                }
                curr_sub_calendar--;
            }
            else if (select_calendar[slot_idx] == 1)
            {
                calendar[slot_idx] = select_index;
            }
        }
        if (do_validate && nof_objects % 5 == 0 && nof_objects > 20)
        {
            SHR_IF_ERR_EXIT(dnx_algo_cal_adjust(unit, nof_objects, calendar_len, calendar));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(build_cb(unit, slots_per_object, nof_objects, calendar_len, max_calendar_len, additional_info,
                                 calendar));
    }
exit:
    SHR_FREE(objects_sub_cal);
    SHR_FREE(slots_per_object_sub_cal);
    SHR_FREE(sub_calendar);
    SHR_FREE(select_calendar);

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_cal_recursive_ratio_based_build_cb(
    int unit,
    dnx_algo_cal_object_t * slots_per_object,
    int nof_objects,
    int calendar_len,
    int max_calendar_len,
    void *additional_data,
    uint32 *calendar)
{

    SHR_FUNC_INIT_VARS(unit);

    DNX_INIT_TIME_ANALYZER_ALGO_CAL_START(unit, DNX_INIT_TIME_ANALYZER_ALGO_CAL_RECURSIVE);

    SHR_IF_ERR_EXIT(dnx_algo_cal_recursive_build_and_validate(unit, dnx_algo_cal_fixed_len_ratio_based_build_cb,
                                                              slots_per_object, nof_objects, calendar_len,
                                                              max_calendar_len, additional_data, FALSE, calendar));

    DNX_INIT_TIME_ANALYZER_ALGO_CAL_STOP(unit, DNX_INIT_TIME_ANALYZER_ALGO_CAL_RECURSIVE);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_cal_recursive_ratio_based_build_and_validate_cb(
    int unit,
    dnx_algo_cal_object_t * slots_per_object,
    int nof_objects,
    int calendar_len,
    int max_calendar_len,
    void *additional_data,
    uint32 *calendar)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_cal_recursive_build_and_validate(unit, dnx_algo_cal_fixed_len_ratio_based_build_cb,
                                                              slots_per_object, nof_objects, calendar_len,
                                                              max_calendar_len, additional_data, TRUE, calendar));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_cal_recursive_ratio_based_build(
    int unit,
    uint32 *slots_per_object,
    int nof_objects,
    int calendar_len,
    int max_calendar_len,
    uint32 *calendar)
{

    dnx_algo_cal_object_t *object_info = NULL;
    int i;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(object_info, sizeof(*object_info) * nof_objects, "object_info", "%s%s%s", EMPTY, EMPTY, EMPTY);
    for (i = 0; i < nof_objects; i++)
    {
        object_info[i].nof_slots = slots_per_object[i];
        object_info[i].id = i;
        object_info[i].orig_id = i;
        object_info[i].orig_nof_slots = slots_per_object[i];
    }
    SHR_IF_ERR_EXIT(dnx_algo_cal_recursive_ratio_based_build_cb(unit, object_info, nof_objects, calendar_len,
                                                                max_calendar_len, NULL, calendar));
exit:
    SHR_FREE(object_info);
    SHR_FUNC_EXIT;
}
/**
 * See algo_cal.h
 */
shr_error_e
dnx_algo_cal_simple_fixed_len_cal_build(
    int unit,
    dnx_algo_cal_algorithm_t cal_alg_mode,
    uint32 *slots_per_object,
    uint32 nof_objects,
    uint32 calendar_len,
    uint32 max_calendar_len,
    uint32 *calendar)
{

    SHR_FUNC_INIT_VARS(unit);
    switch (cal_alg_mode)
    {
        case DNX_ALGO_CAL_ALGORITHM_RATIO_BASED:
            SHR_IF_ERR_EXIT(dnx_algo_cal_fixed_len_ratio_based_build(unit, slots_per_object, nof_objects,
                                                                     calendar_len, max_calendar_len, calendar));
            break;

        case DNX_ALGO_CAL_ALGORITHM_BIG_HOP_SMALL_HOP:
            SHR_IF_ERR_EXIT(dnx_algo_cal_fixed_len_big_hope_first_build(unit, slots_per_object, nof_objects,
                                                                        calendar_len, max_calendar_len, calendar));
            break;

        case DNX_ALGO_CAL_ALGORITHM_RATIO_BASED_GROUPED:
            SHR_IF_ERR_EXIT(dnx_algo_cal_group_and_build(unit, dnx_algo_cal_fixed_len_ratio_based_build_cb,
                                                         slots_per_object, nof_objects, calendar_len, max_calendar_len,
                                                         NULL, calendar));
            break;
        case DNX_ALGO_CAL_ALGORITHM_RATIO_BASED_GROUPED_RECURSIVE:
            SHR_IF_ERR_EXIT(dnx_algo_cal_group_and_build(unit, dnx_algo_cal_recursive_ratio_based_build_cb,
                                                         slots_per_object, nof_objects, calendar_len, max_calendar_len,
                                                         NULL, calendar));
            break;

        case DNX_ALGO_CAL_ALGORITHM_RATIO_BASED_GROUPED_RECURSIVE_WITH_VALIDATE:
            SHR_IF_ERR_EXIT(dnx_algo_cal_group_and_build(unit, dnx_algo_cal_recursive_ratio_based_build_and_validate_cb,
                                                         slots_per_object, nof_objects, calendar_len, max_calendar_len,
                                                         NULL, calendar));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected mode %d\n", cal_alg_mode);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * build best calendar for provided object rates, total bandwidth and maximal calendar length
 *
 */
shr_error_e
dnx_algo_cal_simple_from_rates_to_calendar(
    int unit,
    int alt_cal_alg,
    uint64 *rates_per_object,
    uint32 nof_objects,
    uint64 total_bandwidth,
    uint32 max_calendar_len,
    uint32 *calendar_slots,
    uint32 *calendar_len)
{
    uint32 *slots_per_object_p = NULL;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(slots_per_object_p, sizeof(*slots_per_object_p) * nof_objects, "slots_per_object_p", "%s%s%s", EMPTY,
              EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_algo_cal_simple_len_calculate(unit,
                                                      rates_per_object,
                                                      nof_objects,
                                                      total_bandwidth,
                                                      max_calendar_len, calendar_len, slots_per_object_p));

    /*
     * Given the optimal calendar length and the
     * corresponding weight (in slots) of each port,
     * build a calendar that will avoid burstiness
     * behavior as much as possible.
     */
    SHR_IF_ERR_EXIT(dnx_algo_cal_simple_fixed_len_cal_build(unit, alt_cal_alg,
                                                            slots_per_object_p,
                                                            nof_objects,
                                                            *calendar_len, max_calendar_len, calendar_slots));

exit:
    SHR_FREE(slots_per_object_p);
    SHR_FUNC_EXIT;
}

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
shr_error_e
dnx_algo_cal_simple_object_rate_get(
    int unit,
    uint32 *calendar,
    uint32 calendar_len,
    uint64 total_cal_rate,
    int object_id,
    uint32 *object_rate)
{
    uint32 slot_id;
    uint64 calc;
    int obj_num_of_cal_slots = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (0 == calendar_len)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to calculate calendar");
    }

    *object_rate = 0;

    for (slot_id = 0; slot_id < calendar_len; ++slot_id)
    {
        /*
         * Increase the number of calendar slots of this object by 1 
         */
        if (calendar[slot_id] == object_id)
        {
            ++obj_num_of_cal_slots;
        }
    }

    COMPILER_64_UMUL_32(total_cal_rate, obj_num_of_cal_slots);
    dnx_algo_cal_u64_div_long_and_round_up(unit, total_cal_rate, calendar_len, &calc);
    SHR_IF_ERR_EXIT(dnx_algo_cal_u64_to_long(unit, calc, object_rate));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
algo_cal_distance_get(
    int unit,
    int left_index,
    int right_index,
    int calendar_len,
    int *distance)
{
    SHR_FUNC_INIT_VARS(unit);

    *distance = right_index - left_index;
    if (*distance < 0)
    {
        *distance += calendar_len;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
algo_cal_dist_calc(
    int unit,
    uint32 nof_objects,
    uint32 calendar_len,
    uint32 *calendar,
    int level,
    double deviation,
    int extra,
    int check_abs_dist,
    int *is_valid_test,
    dnx_algo_cal_detailed_entry_t * cal_dist_info)
{
    uint32 p, i, j;
    uint32 *slots_per_object = NULL;
    double optimal_gap;
    int actual_gap;
    int first_position[3], curr_position[3];
    int initial_position;
    int curr_index;
    int max_allowed_gap, min_allowed_gap;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_ERR_EXIT(slots_per_object, sizeof(int) * nof_objects, "Slots per object", "%s%s%s", EMPTY, EMPTY, EMPTY);
    sal_memset(slots_per_object, 0, sizeof(uint32) * nof_objects);
    sal_memset(cal_dist_info, 0, sizeof(dnx_algo_cal_detailed_entry_t) * calendar_len);

    for (i = 0; i < calendar_len; i++)
    {
        if (calendar[i] != -1)
        {
            slots_per_object[calendar[i]]++;
        }
        cal_dist_info[i].object = calendar[i];
    }

    for (p = 0; p < nof_objects; p++)
    {
        if (slots_per_object[p] == 0)
        {
            continue;
        }
        if (slots_per_object[p] <= level)
        {
            continue;
        }

        optimal_gap = 1.0 * calendar_len / slots_per_object[p];
        if (is_valid_test && !is_valid_test[p])
        {
            cal_dist_info[p].valid = FALSE;
            continue;
        }

        if (check_abs_dist)
        {
            min_allowed_gap = deviation + 1;
            max_allowed_gap = calendar_len;
        }
        else
        {
            min_allowed_gap = trunc((level - deviation) * optimal_gap) + extra;
            max_allowed_gap = ceil((level + deviation) * optimal_gap) - extra;
        }

        for (i = 0; i < level; i++)
        {
            first_position[i] = -1;
            curr_position[i] = -1;
        }

        curr_index = 0;
        for (i = 0; i < calendar_len; i++)
        {
            if (calendar[i] == p)
            {
                initial_position = FALSE;
                for (j = 0; j < level; j++)
                {
                    if (first_position[j] == -1)
                    {
                        first_position[j] = i;
                        curr_position[j] = i;
                        initial_position = TRUE;
                        break;
                    }
                }
                if (initial_position)
                {
                    continue;
                }

                actual_gap = i - curr_position[curr_index];
                cal_dist_info[i].valid = TRUE;
                cal_dist_info[i].optimal_dist = optimal_gap;
                cal_dist_info[i].min_allowed_gap = min_allowed_gap;
                cal_dist_info[i].max_allowed_gap = max_allowed_gap;
                cal_dist_info[i].prev_index = curr_position[curr_index];
                cal_dist_info[i].actual_dist_left = actual_gap;
                cal_dist_info[curr_position[curr_index]].next_index = i;
                cal_dist_info[curr_position[curr_index]].actual_dist_right = actual_gap;
                curr_position[curr_index] = i;
                curr_index = (curr_index + 1) % level;
            }
        }

        if (curr_position[curr_index] != first_position[0])
        {
            for (j = 0; j < level; j++)
            {
                i = curr_position[(curr_index + j) % level];
                actual_gap = (calendar_len - i + first_position[j]);
                cal_dist_info[first_position[j]].valid = TRUE;
                cal_dist_info[first_position[j]].optimal_dist = optimal_gap;
                cal_dist_info[first_position[j]].min_allowed_gap = min_allowed_gap;
                cal_dist_info[first_position[j]].max_allowed_gap = max_allowed_gap;
                cal_dist_info[first_position[j]].actual_dist_left = actual_gap;
                cal_dist_info[first_position[j]].prev_index = i;
                cal_dist_info[i].next_index = first_position[j];
                cal_dist_info[i].actual_dist_right = actual_gap;
            }
        }

    }

exit:
    SHR_FREE(slots_per_object);
    SHR_FUNC_EXIT;
}

shr_error_e
algo_cal_can_switch(
    int unit,
    dnx_algo_cal_detailed_entry_t * cal_dist_info,
    int calendar_len,
    int nof_tests,
    int j,
    int k,
    int move_j_right,
    int *can_switch)
{
    int delta;

    int curr_test;

    int actual_dist_left_k, actual_dist_right_k;

    SHR_FUNC_INIT_VARS(unit);
    *can_switch = 1;

    for (curr_test = 0; curr_test < nof_tests; curr_test++)
    {
        dnx_algo_cal_detailed_entry_t *cal_dist_info_curr = &cal_dist_info[curr_test * calendar_len];

        actual_dist_left_k = cal_dist_info_curr[k].actual_dist_left;
        actual_dist_right_k = cal_dist_info_curr[k].actual_dist_right;

        if (move_j_right)
        {
            delta = k - j;
            if (delta < 0)
            {
                delta += calendar_len;
            }
            if (cal_dist_info_curr[k].valid &&
                (actual_dist_right_k + delta > cal_dist_info_curr[k].max_allowed_gap ||
                 actual_dist_left_k - delta < cal_dist_info_curr[k].min_allowed_gap))
            {
                *can_switch = 0;
            }
        }
        else
        {
            delta = j - k;
            if (delta < 0)
            {
                delta += calendar_len;
            }

            if (cal_dist_info_curr[k].valid &&
                (actual_dist_left_k + delta > cal_dist_info_curr[k].max_allowed_gap ||
                 actual_dist_right_k - delta < cal_dist_info_curr[k].min_allowed_gap))
            {
                *can_switch = 0;
            }
        }
    }

    SHR_FUNC_EXIT;
}

shr_error_e
algo_cal_do_switch(
    int unit,
    dnx_algo_cal_detailed_entry_t * cal_dist_info,
    int calendar_len,
    int nof_tests,
    int j,
    int k)
{
    int t;
    int delta;
    dnx_algo_cal_detailed_entry_t temp_cal_entry;

    SHR_FUNC_INIT_VARS(unit);

    delta = k - j;
    if (delta < 0)
    {
        delta += calendar_len;
    }

    for (t = 0; t < nof_tests; t++)
    {
        dnx_algo_cal_detailed_entry_t *cal_dist_info_temp = &cal_dist_info[t * calendar_len];
        cal_dist_info_temp[j].actual_dist_left += delta;
        cal_dist_info_temp[j].actual_dist_right -= delta;
        cal_dist_info_temp[cal_dist_info_temp[j].next_index].actual_dist_left -= delta;
        cal_dist_info_temp[cal_dist_info_temp[j].prev_index].actual_dist_right += delta;
        cal_dist_info_temp[cal_dist_info_temp[j].next_index].prev_index = k;
        cal_dist_info_temp[cal_dist_info_temp[j].prev_index].next_index = k;

        if (cal_dist_info_temp[k].valid)
        {
            cal_dist_info_temp[k].actual_dist_left -= delta;
            cal_dist_info_temp[k].actual_dist_right += delta;
            cal_dist_info_temp[cal_dist_info_temp[k].next_index].actual_dist_left += delta;
            cal_dist_info_temp[cal_dist_info_temp[k].prev_index].actual_dist_right -= delta;
            cal_dist_info_temp[cal_dist_info_temp[k].next_index].prev_index = j;
            cal_dist_info_temp[cal_dist_info_temp[k].prev_index].next_index = j;
        }
        sal_memcpy(&temp_cal_entry, &cal_dist_info_temp[j], sizeof(dnx_algo_cal_detailed_entry_t));
        sal_memcpy(&cal_dist_info_temp[j], &cal_dist_info_temp[k], sizeof(dnx_algo_cal_detailed_entry_t));
        sal_memcpy(&cal_dist_info_temp[k], &temp_cal_entry, sizeof(dnx_algo_cal_detailed_entry_t));

    }

    SHR_FUNC_EXIT;
}

shr_error_e
algo_cal_trans_can_switch(
    int unit,
    dnx_algo_cal_random_object_transition_info_t * trans_info,
    int j,
    int k,
    int obj_j,
    int obj_k,
    int move_j_right,
    int *can_switch)
{
    int l;

    SHR_FUNC_INIT_VARS(unit);

    *can_switch = 1;

    if (obj_k == -1)
    {
        SHR_EXIT();
    }
    for (l = 0; l < 3; l++)
    {
        if (move_j_right)
        {

            if (trans_info[obj_k].slot_info[l].valid &&
                trans_info[obj_k].slot_info[l].current == k && j < trans_info[obj_k].slot_info[l].min_first_slot)
            {
                *can_switch = 0;
                break;
            }

        }
        else
        {
            if (trans_info[obj_k].slot_info[l].valid &&
                trans_info[obj_k].slot_info[l].current == k && j > trans_info[obj_k].slot_info[l].max_first_slot)
            {
                *can_switch = 0;
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
algo_cal_fix(
    int unit,
    uint32 calendar_length,
    int nof_tests,
    int nof_tests_to_fix,
    int complete_fix,
    dnx_algo_cal_detailed_entry_t * cal_dist_info)
{
    int i, j, k;
    int gap;
    int delta;
    int curr_test;

    int nof_swaps = 0;
    int nof_unfixed = 0;

    int actual_dist_left_i, actual_dist_right_i;
    int actual_dist_left_j, actual_dist_right_j;

    int can_switch;
    SHR_FUNC_INIT_VARS(unit);

    do
    {
        nof_swaps = 0;
        for (curr_test = 0; curr_test < nof_tests_to_fix; curr_test++)
        {
            dnx_algo_cal_detailed_entry_t *cal_dist_info_curr = &cal_dist_info[curr_test * calendar_length];
            for (i = calendar_length - 1; i >= 0; i--)
            {

                if (!cal_dist_info_curr[i].valid)
                {
                    continue;
                }

                actual_dist_left_i = cal_dist_info_curr[i].actual_dist_left;
                actual_dist_right_i = cal_dist_info_curr[i].actual_dist_right;

                if (actual_dist_left_i < cal_dist_info_curr[i].min_allowed_gap)
                {
                    j = i;
                    for (gap = 1;; gap++)
                    {
                        actual_dist_left_j = cal_dist_info_curr[j].actual_dist_left;
                        actual_dist_right_j = cal_dist_info_curr[j].actual_dist_right;

                        k = (i + gap);
                        if (k >= calendar_length)
                        {
                            k -= calendar_length;
                        }
                        if (actual_dist_left_j >= cal_dist_info_curr[j].min_allowed_gap)
                        {
                            /** the distance is within limits */
                            break;
                        }
                        if (cal_dist_info_curr[k].object == cal_dist_info_curr[j].object)
                        {
                            /*
                             * we reached previous instance of the same object
                             * cannot move further
                             */
                            nof_unfixed++;
                            break;
                        }
                        delta = k - j;
                        if (delta < 0)
                        {
                            delta += calendar_length;
                        }

                        if (actual_dist_right_j - delta < cal_dist_info_curr[j].min_allowed_gap)
                        {
                            /** distance right does not allow to move further */
                            nof_unfixed++;
                            break;
                        }

                        SHR_IF_ERR_EXIT(algo_cal_can_switch(unit, cal_dist_info, calendar_length, nof_tests, j, k, TRUE,
                                                            &can_switch));
                        if (!can_switch)
                        {
                            continue;
                        }

                        SHR_IF_ERR_EXIT(algo_cal_do_switch(unit, cal_dist_info, calendar_length, nof_tests, j, k));

                        j = k;
                        nof_swaps++;
                    }
                }
                if (actual_dist_right_i > cal_dist_info_curr[i].max_allowed_gap)
                {
                    j = i;
                    for (gap = 1;; gap++)
                    {
                        actual_dist_left_j = cal_dist_info_curr[j].actual_dist_left;
                        actual_dist_right_j = cal_dist_info_curr[j].actual_dist_right;
                        k = (i + gap);
                        if (k >= calendar_length)
                        {
                            k -= calendar_length;
                        }
                        if (actual_dist_right_j <= cal_dist_info_curr[j].max_allowed_gap)
                        {
                            /** the distance is within limits */
                            break;
                        }
                        if (cal_dist_info_curr[k].object == cal_dist_info_curr[j].object)
                        {
                            /*
                             * we reached previous instance of the same object
                             * cannot move further
                             */
                            nof_unfixed++;
                            break;
                        }
                        delta = k - j;
                        if (delta < 0)
                        {
                            delta += calendar_length;
                        }

                        if (actual_dist_left_j + delta > cal_dist_info_curr[j].max_allowed_gap)
                        {
                            /** distance left does not allow to move further */
                            nof_unfixed++;
                            break;
                        }

                        SHR_IF_ERR_EXIT(algo_cal_can_switch(unit, cal_dist_info, calendar_length, nof_tests, j, k, TRUE,
                                                            &can_switch));
                        if (!can_switch)
                        {
                            continue;
                        }

                        SHR_IF_ERR_EXIT(algo_cal_do_switch(unit, cal_dist_info, calendar_length, nof_tests, j, k));
                        j = k;
                        nof_swaps++;

                    }
                }
            }
            for (i = 0; i < calendar_length; i++)
            {
                if (!cal_dist_info_curr[i].valid)
                {
                    continue;
                }
                actual_dist_left_i = cal_dist_info_curr[i].actual_dist_left;
                actual_dist_right_i = cal_dist_info_curr[i].actual_dist_right;
                if (actual_dist_right_i < cal_dist_info_curr[i].min_allowed_gap)
                {
                    j = i;
                    for (gap = 1;; gap++)
                    {
                        actual_dist_left_j = cal_dist_info_curr[j].actual_dist_left;
                        actual_dist_right_j = cal_dist_info_curr[j].actual_dist_right;
                        k = (i - gap);
                        if (k < 0)
                        {
                            k += calendar_length;
                        }
                        if (actual_dist_right_j >= cal_dist_info_curr[j].min_allowed_gap)
                        {
                            /** the distance is within limits */
                            break;
                        }
                        if (cal_dist_info_curr[k].object == cal_dist_info_curr[j].object)
                        {
                            /*
                             * we reached previous instance of the same object
                             * cannot move further
                             */
                            nof_unfixed++;
                            break;
                        }
                        delta = j - k;
                        if (delta < 0)
                        {
                            delta += calendar_length;
                        }

                        if (actual_dist_left_j - delta < cal_dist_info_curr[j].min_allowed_gap)
                        {
                            /** distance left does not allow to move further */
                            nof_unfixed++;
                            break;
                        }

                        SHR_IF_ERR_EXIT(algo_cal_can_switch
                                        (unit, cal_dist_info, calendar_length, nof_tests, j, k, FALSE, &can_switch));
                        if (!can_switch)
                        {
                            continue;
                        }
                        SHR_IF_ERR_EXIT(algo_cal_do_switch(unit, cal_dist_info, calendar_length, nof_tests, k, j));
                        j = k;
                        nof_swaps++;
                    }
                }
                if (actual_dist_left_i > cal_dist_info_curr[i].max_allowed_gap)
                {
                    j = i;
                    for (gap = 1;; gap++)
                    {
                        actual_dist_left_j = cal_dist_info_curr[j].actual_dist_left;
                        actual_dist_right_j = cal_dist_info_curr[j].actual_dist_right;
                        k = (i - gap);
                        if (k < 0)
                        {
                            k += calendar_length;
                        }
                        if (actual_dist_left_j <= cal_dist_info_curr[j].max_allowed_gap)
                        {
                            /** the distance is within limits */
                            break;
                        }
                        if (cal_dist_info_curr[k].object == cal_dist_info_curr[j].object)
                        {
                            /*
                             * we reached previous instance of the same object
                             * cannot move further
                             */
                            nof_unfixed++;
                            break;
                        }
                        delta = j - k;
                        if (delta < 0)
                        {
                            delta += calendar_length;
                        }
                        if (actual_dist_right_j + delta > cal_dist_info_curr[j].max_allowed_gap)
                        {
                            /** distance right does not allow to move further */
                            nof_unfixed++;
                            break;
                        }

                        SHR_IF_ERR_EXIT(algo_cal_can_switch
                                        (unit, cal_dist_info, calendar_length, nof_tests, j, k, FALSE, &can_switch));
                        if (!can_switch)
                        {
                            continue;
                        }

                        SHR_IF_ERR_EXIT(algo_cal_do_switch(unit, cal_dist_info, calendar_length, nof_tests, k, j));
                        j = k;
                        nof_swaps++;
                    }
                }
            }
        }
    }
    while (complete_fix && nof_unfixed > 0 && nof_swaps > 0);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
algo_cal_transition_fix(
    int unit,
    uint32 calendar_len,
    int nof_objects,
    dnx_algo_cal_random_object_transition_info_t * trans_info,
    int nof_tests,
    dnx_algo_cal_detailed_entry_t * cal_dist_info)
{
    int i, j, k;
    int gap;
    int level, l;

    int nof_swaps = 0;
    int nof_unfixed = 0;

    int can_switch;

    int p, other;

    SHR_FUNC_INIT_VARS(unit);

    for (i = 0; i < nof_objects; i++)
    {
        for (level = 0; level < 3; level++)
        {
            trans_info[i].slot_info[level].current = -1;
        }
    }
    for (i = 0; i < calendar_len; i++)
    {
        p = cal_dist_info[i].object;

        if (p == -1)
        {
            continue;
        }
        for (level = 0; level < 3; level++)
        {
            if (trans_info[p].slot_info[level].current == -1)
            {
                trans_info[p].slot_info[level].current = i;
                break;
            }
        }
    }

    for (level = 0; level < 3; level++)
    {
        for (p = 0; p < nof_objects; p++)
        {
            if (trans_info[p].slot_info[level].valid == FALSE)
            {
                continue;
            }
            if (trans_info[p].slot_info[level].current < trans_info[p].slot_info[level].min_first_slot)
            {
                /** move right */
                i = trans_info[p].slot_info[level].current;
                j = i;
                gap = 1;
                for (gap = 1; j < trans_info[p].slot_info[level].min_first_slot; gap++)
                {
                    k = (i + gap);
                    if (j >= trans_info[p].slot_info[level].min_first_slot)
                    {
                        /** within limits */
                        break;
                    }
                    if (cal_dist_info[k].object == cal_dist_info[j].object)
                    {
                        /*
                         * we reached next instance of the same object
                         * cannot move further
                         */
                        nof_unfixed++;
                        break;
                    }

                    SHR_IF_ERR_EXIT(algo_cal_can_switch(unit, cal_dist_info, calendar_len, nof_tests, j, k, TRUE,
                                                        &can_switch));
                    if (!can_switch)
                    {
                        continue;
                    }

                    other = cal_dist_info[k].object;
                    SHR_IF_ERR_EXIT(algo_cal_trans_can_switch(unit, trans_info, j, k, p, other, TRUE, &can_switch));
                    if (!can_switch)
                    {
                        continue;
                    }

                    SHR_IF_ERR_EXIT(algo_cal_do_switch(unit, cal_dist_info, calendar_len, nof_tests, j, k));
                    if (other != -1)
                    {
                        for (l = 0; l < 3; l++)
                        {
                            if (trans_info[other].slot_info[l].current == k)
                            {
                                trans_info[other].slot_info[l].current = j;
                                break;
                            }
                        }
                    }
                    trans_info[p].slot_info[level].current = k;
                    j = k;

                    nof_swaps++;
                }
            }
            if (trans_info[p].slot_info[level].current > trans_info[p].slot_info[level].max_first_slot)
            {
                /** move left */
                i = trans_info[p].slot_info[level].current;
                j = i;
                for (gap = 1; j > trans_info[p].slot_info[level].max_first_slot; gap++)
                {
                    k = (i - gap);
                    if (k < 0)
                    {
                        k += calendar_len;
                    }
                    if (cal_dist_info[k].object == cal_dist_info[j].object)
                    {
                        /*
                         * we reached previous instance of the same object
                         * cannot move further
                         */
                        nof_unfixed++;
                        break;
                    }

                    SHR_IF_ERR_EXIT(algo_cal_can_switch(unit, cal_dist_info, calendar_len, nof_tests, j, k, FALSE,
                                                        &can_switch));
                    if (!can_switch)
                    {
                        continue;
                    }

                    other = cal_dist_info[k].object;
                    SHR_IF_ERR_EXIT(algo_cal_trans_can_switch(unit, trans_info, j, k, p, other, FALSE, &can_switch));
                    if (!can_switch)
                    {
                        continue;
                    }

                    SHR_IF_ERR_EXIT(algo_cal_do_switch(unit, cal_dist_info, calendar_len, nof_tests, k, j));
                    if (other != -1)
                    {
                        for (l = 0; l < 3; l++)
                        {
                            if (trans_info[other].slot_info[l].current == k)
                            {
                                trans_info[other].slot_info[l].current = j;
                                break;
                            }
                        }
                    }
                    trans_info[p].slot_info[level].current = k;
                    j = k;
                    nof_swaps++;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_cal_adjust(
    int unit,
    int nof_objects,
    int calendar_len,
    uint32 *calendar)
{
    int i;
    int extra = 0;
    dnx_algo_cal_detailed_entry_t *cal_dist_info = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_ERR_EXIT(cal_dist_info, sizeof(dnx_algo_cal_detailed_entry_t) * calendar_len * 2, "calendar info",
                       "%s%s%s", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(algo_cal_dist_calc
                    (unit, nof_objects, calendar_len, calendar, 3, 1.0, extra, FALSE, NULL, cal_dist_info));
    SHR_IF_ERR_EXIT(algo_cal_dist_calc
                    (unit, nof_objects, calendar_len, calendar, 2, 0.8, 0, FALSE, NULL, &cal_dist_info[calendar_len]));
    SHR_IF_ERR_EXIT(algo_cal_fix(unit, calendar_len, 2, (extra == 0 ? 2 : 1), 0, cal_dist_info));

    for (i = 0; i < calendar_len; i++)
    {
        calendar[i] = cal_dist_info[i].object;
    }

exit:
    SHR_FREE(cal_dist_info);
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_algo_cal_simple_alt_grouped_recursive_build(
    int unit,
    uint32 *slots_per_object,
    int nof_objects,
    int calendar_len,
    int max_calendar_len,
    uint32 *calendar)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_cal_simple_fixed_len_cal_build(unit, DNX_ALGO_CAL_ALGORITHM_RATIO_BASED_GROUPED_RECURSIVE,
                                                            slots_per_object, nof_objects, calendar_len,
                                                            max_calendar_len, calendar));

exit:
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_algo_cal_simple_alt_grouped_recursive_build_and_validate(
    int unit,
    uint32 *slots_per_object,
    int nof_objects,
    int calendar_len,
    int max_calendar_len,
    uint32 *calendar)
{
    int i;
    int extra;
    dnx_algo_cal_detailed_entry_t *cal_dist_info = NULL;

    SHR_FUNC_INIT_VARS(unit);

    DNX_INIT_TIME_ANALYZER_ALGO_CAL_START(unit, DNX_INIT_TIME_ANALYZER_ALGO_CAL_RECURSIVE_VALIDATE);

    SHR_IF_ERR_EXIT(dnx_algo_cal_simple_fixed_len_cal_build(unit,
                                                            DNX_ALGO_CAL_ALGORITHM_RATIO_BASED_GROUPED_RECURSIVE_WITH_VALIDATE,
                                                            slots_per_object, nof_objects, calendar_len,
                                                            max_calendar_len, calendar));

    SHR_ALLOC_ERR_EXIT(cal_dist_info, sizeof(dnx_algo_cal_detailed_entry_t) * calendar_len * 3, "calendar info",
                       "%s%s%s", EMPTY, EMPTY, EMPTY);

    for (extra = 4; extra >= 0; extra -= 2)
    {
        SHR_IF_ERR_EXIT(algo_cal_dist_calc
                        (unit, nof_objects, calendar_len, calendar, 3, 1.0, extra, FALSE, NULL, cal_dist_info));
        SHR_IF_ERR_EXIT(algo_cal_dist_calc
                        (unit, nof_objects, calendar_len, calendar, 2, 0.8, 0, FALSE, NULL,
                         &cal_dist_info[calendar_len]));
        SHR_IF_ERR_EXIT(algo_cal_dist_calc
                        (unit, nof_objects, calendar_len, calendar, 1, 0.5, 0, FALSE, NULL,
                         &cal_dist_info[2 * calendar_len]));
        SHR_IF_ERR_EXIT(algo_cal_fix(unit, calendar_len, 3, (extra == 0 ? 3 : 1), 1, cal_dist_info));

        for (i = 0; i < calendar_len; i++)
        {
            calendar[i] = cal_dist_info[i].object;
        }
    }

    DNX_INIT_TIME_ANALYZER_ALGO_CAL_STOP(unit, DNX_INIT_TIME_ANALYZER_ALGO_CAL_RECURSIVE_VALIDATE);

exit:
    SHR_FREE(cal_dist_info);
    SHR_FUNC_EXIT;

}
