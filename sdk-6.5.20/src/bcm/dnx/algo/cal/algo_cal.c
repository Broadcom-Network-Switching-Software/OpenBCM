
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <shared/shrextend/shrextend_debug.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <shared/utilex/utilex_u64.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_ofp_rate_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>

#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/cal/algo_cal.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

#define DNX_ALGO_CAL_RATE_DEVIATION     (100000000)


int
dnx_algo_ofp_rates_fill_shaper_generic_calendar_credits(
    int unit,
    int core,
    dnx_ofp_rates_cal_info_t * cal_info,
    uint32 *ports_rates,
    
    uint32 calendar_len,
    
    uint8 add_dummy_tail,
    dnx_ofp_rates_cal_egq_t * calendar)
{
    uint32 slot_idx, port_idx, egq_resolution;
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
    
    temp_calendar_length = calendar_len - dummy_tail_entry;
    if (cal_info->cal_type == DNX_OFP_RATES_EGQ_CAL_CHAN_ARB)
    {
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.calcal_len.get(unit, core, &calcal_length));
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.
                        nof_calcal_instances.get(unit, core, cal_info->chan_arb_id, &calcal_instances));
    }
    else
    {
        
        calcal_length = 1;
        calcal_instances = 1;
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING, 0, &port_bm));

    BCM_PBMP_ITER(port_bm, logical_port)
    {
        
        SHR_IF_ERR_EXIT(dnx_egr_queuing_port_packet_mode_get(unit, logical_port, &packet_mode));
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
        SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &nof_priorities));
        for (port_idx = base_q_pair; port_idx < (base_q_pair + nof_priorities); port_idx++)
        {
            uint32 rate;

            if (!ports_rates[port_idx])
            {
                continue;
            }
            if (packet_mode == 0)
            {
                
                egq_resolution = dnx_data_egr_queuing.params.cal_res_get(unit);
            }
            else
            {
                
                egq_resolution = dnx_data_egr_queuing.params.cal_res_packet_mode_get(unit);
            }
            rate = ports_rates[port_idx];
            SHR_IF_ERR_EXIT(dnx_ofp_rates_calculate_credits_from_rate
                            (unit, calcal_length, calcal_instances, calendar_len, egq_resolution, (uint32) packet_mode,
                             rate, &(port_credits_p[port_idx])));
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

static int
dnx_algo_cal_simple_per_len_build(
    int unit,
    uint32 *rates_per_object,
    uint32 nof_objects,
    uint32 total_bandwidth,
    uint32 max_calendar_len,
    uint32 tentative_len,
    uint32 *actual_len,
    uint32 *slots_per_object_p,
    uint32 *deviation)
{
    uint32 obj_id, total_num_slots = 0, calc_deviation;
    uint32 *slots_rates_p = NULL;
    uint32 rem;
    uint32 num_slots, temp1, temp2;
    UTILEX_U64 u64_1, u64_2;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(slots_rates_p, sizeof(*slots_rates_p) * nof_objects, "slots_rates_p", "%s%s%s", EMPTY, EMPTY,
                       EMPTY);

    
    for (obj_id = 0; obj_id < nof_objects; ++obj_id)
    {
        if (rates_per_object[obj_id] > 0)
        {
            

            utilex_u64_multiply_longs(rates_per_object[obj_id], tentative_len, &u64_1);
            rem = utilex_u64_devide_u64_long(&u64_1, total_bandwidth, &u64_2);
            utilex_u64_to_long(&u64_2, &(num_slots));

            
            num_slots = (rem > 0 ? (num_slots + 1) : ((num_slots == 0) ? num_slots + 1 : num_slots));

            slots_rates_p[obj_id] = num_slots;
            total_num_slots += num_slots;
        }
    }
    if (total_num_slots > max_calendar_len || total_bandwidth == 0 || total_num_slots == 0)
    {
        
        *actual_len = 0;
        sal_memset(slots_per_object_p, 0, sizeof(*slots_per_object_p) * nof_objects);
    }
    else
    {

        calc_deviation = 0;
        for (obj_id = 0; obj_id < nof_objects; ++obj_id)
        {
            utilex_u64_multiply_longs(DNX_ALGO_CAL_RATE_DEVIATION, rates_per_object[obj_id], &u64_1);
            rem = utilex_u64_devide_u64_long(&u64_1, total_bandwidth, &u64_2);
            utilex_u64_to_long(&u64_2, &(temp1));
            temp1 = (rem > 0 ? temp1 + 1 : temp1);

            utilex_u64_multiply_longs(DNX_ALGO_CAL_RATE_DEVIATION, slots_rates_p[obj_id], &u64_1);
            rem = utilex_u64_devide_u64_long(&u64_1, total_num_slots, &u64_2);
            utilex_u64_to_long(&u64_2, &(temp2));
            temp2 = (rem > 0 ? temp2 + 1 : temp2);

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


static int
dnx_algo_cal_simple_len_calculate(
    int unit,
    uint32 *rates_per_object,
    uint32 nof_objects,
    uint32 total_bandwidth,
    uint32 max_calendar_len,
    uint32 *actual_calendar_len,
    uint32 *slots_per_object)
{
    uint32 deviation, min_deviation, tentative_len, best_tentative_len, actual_len;
    uint32 *slots_per_object_temp_p = NULL;

    
    uint32 *rates_per_object_final;
    uint32 nof_objects_final;
    uint32 total_bandwidth_final;
    uint32 min_rate;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(slots_per_object_temp_p, sizeof(*slots_per_object_temp_p) * nof_objects, "slots_per_object_temp_p",
              "%s%s%s", EMPTY, EMPTY, EMPTY);

    
    min_deviation = DNX_ALGO_CAL_RATE_DEVIATION;
    best_tentative_len = 0;
    for (tentative_len = 1; tentative_len <= max_calendar_len; ++tentative_len)
    {
        SHR_IF_ERR_EXIT(dnx_algo_cal_simple_per_len_build(unit,
                                                          rates_per_object, nof_objects,
                                                          total_bandwidth, max_calendar_len,
                                                          tentative_len, &actual_len, slots_per_object_temp_p,
                                                          &deviation));

        
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
    
    if (0 == best_tentative_len)
    {

        

        min_rate = 1;
        rates_per_object_final = &min_rate;
        nof_objects_final = 1;
        total_bandwidth_final = 1;
        best_tentative_len = 1;

    }
    else
    {
        rates_per_object_final = rates_per_object;
        nof_objects_final = nof_objects;
        total_bandwidth_final = total_bandwidth;
    }

    
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


shr_error_e
dnx_algo_cal_simple_fixed_len_cal_build(
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

    
    for (slot_idx = 0; slot_idx < max_calendar_len; ++slot_idx)
    {
        calendar[slot_idx] = DNX_ALGO_CAL_ILLEGAL_OBJECT_ID;
    }

    sal_memcpy(slots_per_object_temp_p, slots_per_object, sizeof(*slots_per_object_temp_p) * nof_objects);

    
    alloc_slots = 0;
    rem_cal_len = calendar_len;
    max_obj_idx = utilex_get_index_of_max_member_in_array(slots_per_object_temp_p, nof_objects);
    while (slots_per_object_temp_p[max_obj_idx] > 0)
    {
        
        hop_size = rem_cal_len / slots_per_object_temp_p[max_obj_idx];

        
        hop_size_small = hop_size - 1;

        
        nof_big_hops = rem_cal_len - slots_per_object[max_obj_idx] * hop_size;
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


shr_error_e
dnx_algo_cal_simple_from_rates_to_calendar(
    int unit,
    uint32 *rates_per_object,
    uint32 nof_objects,
    uint32 total_bandwidth,
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

    
    SHR_IF_ERR_EXIT(dnx_algo_cal_simple_fixed_len_cal_build(unit,
                                                            slots_per_object_p,
                                                            nof_objects,
                                                            *calendar_len, max_calendar_len, calendar_slots));

exit:
    SHR_FREE(slots_per_object_p);
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_algo_cal_simple_object_rate_get(
    int unit,
    uint32 *calendar,
    uint32 calendar_len,
    uint32 total_cal_rate,
    int object_id,
    uint32 *object_rate)
{
    uint32 calc, slot_id;
    UTILEX_U64 u64_1, u64_2;
    uint32 rem;
    int obj_num_of_cal_slots = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (0 == calendar_len)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to calculate calendar");
    }

    *object_rate = 0;

    for (slot_id = 0; slot_id < calendar_len; ++slot_id)
    {
        
        if (calendar[slot_id] == object_id)
        {
            ++obj_num_of_cal_slots;
        }
    }

    
    utilex_u64_multiply_longs(obj_num_of_cal_slots, total_cal_rate, &u64_1);
    rem = utilex_u64_devide_u64_long(&u64_1, calendar_len, &u64_2);
    utilex_u64_to_long(&u64_2, &(calc));
    calc = (rem > 0 ? calc + 1 : calc);

    *object_rate = calc;

exit:
    SHR_FUNC_EXIT;
}
