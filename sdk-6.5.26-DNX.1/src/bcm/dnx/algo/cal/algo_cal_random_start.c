/** \file algo_cal_random_start.c
 *  
 *  Calendar distribution calculation
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
#include <math.h>
#include <shared/utilex/utilex_rand.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <bcm_int/dnx/algo/cal/algo_cal.h>
#include <bcm_int/dnx/init/init_time_analyzer.h>

const int dnx_algo_cal_verbose = 0;
const int dnx_algo_cal_verify_debug = 0;

#define DNX_ALGO_CAL_MAX_LEVEL_DISTANCE_CHECK 3

shr_error_e
dnx_algo_cal_consecutive_cyclic_check(
    int unit,
    uint32 object,
    int nof_slots,
    uint32 *calendar,
    int calendar_length,
    int *passed)
{
    double distance;
    int slot_index, first_slot, prev_slot;
    SHR_FUNC_INIT_VARS(unit);

    *passed = TRUE;
    if (nof_slots == 0 || nof_slots == 1)
    {
        SHR_EXIT();
    }

    first_slot = -1;
    prev_slot = -1;
    for (slot_index = 0; slot_index < calendar_length; slot_index++)
    {
        if (calendar[slot_index] != object)
        {
            continue;
        }
        if (first_slot == -1)
        {
            first_slot = slot_index;
            prev_slot = slot_index;
        }
        else
        {
            distance = slot_index - prev_slot;
            /*
             * Hard-coded '2' will prevent two consecutive entries 
             */
            if (distance < 2)
            {
                *passed = FALSE;
                break;
            }
            prev_slot = slot_index;
        }
    }
    if (*passed)
    {
        distance = calendar_length - prev_slot + first_slot;
        if (distance < 2)
        {
            *passed = FALSE;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_cal_min_distance_transition_check(
    int unit,
    bcm_pbmp_t objects_to_check,
    uint32 *calendar,
    uint32 *old_calendar,
    int calendar_length,
    int limit_value,
    int max_occurrences,
    int *passed)
{
    int old_cal_slot, slot_index, meeting_limit_counter = 0;
    SHR_FUNC_INIT_VARS(unit);

    for (old_cal_slot = calendar_length - limit_value; old_cal_slot < calendar_length; ++old_cal_slot)
    {
        if (_SHR_PBMP_MEMBER(objects_to_check, old_calendar[old_cal_slot]))
        {
            for (slot_index = 0; slot_index <= limit_value - (calendar_length - old_cal_slot); ++slot_index)
            {
                if (calendar[slot_index] == old_calendar[old_cal_slot])
                {
                    ++meeting_limit_counter;
                    _SHR_PBMP_PORT_REMOVE(objects_to_check, calendar[slot_index]);
                    break;
                }
            }
        }
    }

    SHR_IF_ERR_EXIT(dnx_algo_cal_min_distance_cyclic_check
                    (unit, objects_to_check, calendar, calendar_length, limit_value,
                     max_occurrences - meeting_limit_counter, passed));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_cal_min_distance_cyclic_check(
    int unit,
    bcm_pbmp_t objects_to_check,
    uint32 *calendar,
    int calendar_length,
    int limit_value,
    int max_occurrences,
    int *passed)
{
    bcm_port_t object;
    int slot_index, prev_slot, first_slot, meeting_limit_counter = 0;
    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_ITER(objects_to_check, object)
    {
        first_slot = -1;
        prev_slot = -1;
        for (slot_index = 0; slot_index < calendar_length; ++slot_index)
        {
            if (calendar[slot_index] != object)
            {
                continue;
            }

            if (first_slot == -1)
            {
                first_slot = slot_index;
                prev_slot = slot_index;
            }
            else
            {
                if (slot_index - prev_slot <= limit_value)
                {
                    ++meeting_limit_counter;
                    break;
                }
                prev_slot = slot_index;
            }
        }

        /*
         * If we didn't meet the limit in the loop before, check cyclic for limit 
         */
        if (slot_index - prev_slot > limit_value && calendar_length - prev_slot + first_slot <= limit_value)
        {
            ++meeting_limit_counter;
        }
    }

    *passed = (meeting_limit_counter <= max_occurrences);

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_cal_distance_cyclic_check(
    int unit,
    int object,
    int nof_slots,
    float deviation,
    uint32 *calendar,
    int calendar_len,
    int *passed)
{
    double avrg_distance, distance;
    int slot_index, first_slot, prev_slot, level = 1;
    SHR_FUNC_INIT_VARS(unit);

    *passed = TRUE;
    if (nof_slots == 0 || nof_slots == 1)
    {
        SHR_EXIT();
    }
    if (1.0 * nof_slots / calendar_len * 100 > 0.3)
    {
        SHR_EXIT();
    }
    avrg_distance = 1.0 * calendar_len / nof_slots;

    first_slot = -1;
    prev_slot = -1;
    for (slot_index = 0; slot_index < calendar_len; slot_index++)
    {
        if (calendar[slot_index] != object)
        {
            continue;
        }
        if (first_slot == -1)
        {
            first_slot = slot_index;
            prev_slot = slot_index;
        }
        else
        {
            distance = slot_index - prev_slot;
            if (distance < trunc((level - deviation) * avrg_distance)
                || distance > ceil((level + deviation) * avrg_distance))
            {
                *passed = FALSE;
                if (dnx_algo_cal_verify_debug)
                {
                    LOG_ERROR_EX(BSL_LOG_MODULE, "check %d < distance < %d failed. distance = %d, avrg_distance = %f\n",
                                 (int) trunc((level - deviation) * avrg_distance),
                                 (int) ceil((level + deviation) * avrg_distance), (int) distance, avrg_distance);
                }
                break;
            }
            prev_slot = slot_index;
        }
    }
    if (*passed)
    {
        distance = calendar_len - prev_slot + first_slot;
        if (distance < trunc((level - deviation) * avrg_distance)
            || distance > ceil((level + deviation) * avrg_distance))
        {
            *passed = FALSE;
            if (dnx_algo_cal_verify_debug)
            {
                LOG_ERROR_EX(BSL_LOG_MODULE, "check %d < distance < %d failed. distance = %d, avrg_distance = %f\n",
                             (int) trunc((level - deviation) * avrg_distance),
                             (int) ceil((level + deviation) * avrg_distance), (int) distance, avrg_distance);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_algo_cal_distance_all_check(
    int unit,
    int nof_objects,
    int calendar_len,
    uint32 *calendar,
    int *passed)
{
    int slot_index, object, nof_slots;
    SHR_FUNC_INIT_VARS(unit);

    *passed = TRUE;

    for (object = 0; object < nof_objects; object++)
    {
        nof_slots = 0;
        for (slot_index = 0; slot_index < calendar_len; slot_index++)
        {
            if (calendar[slot_index] == object)
            {
                nof_slots++;
            }
        }

        SHR_IF_ERR_EXIT(dnx_algo_cal_distance_cyclic_check
                        (unit, object, nof_slots, 0.5, calendar, calendar_len, passed));

        if (*passed == FALSE)
        {
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_cal_double_distance_cyclic_check(
    int unit,
    int object,
    int nof_slots,
    float deviation,
    uint32 *calendar,
    int calendar_len,
    int *passed)
{
    double avrg_distance, distance;
    int slot_index, first_slot, second_slot, prev_slot, last_slot, level = 2;
    SHR_FUNC_INIT_VARS(unit);

    *passed = TRUE;
    if (nof_slots <= 2)
    {
        SHR_EXIT();
    }
    if (1.0 * nof_slots / calendar_len * 100 > 6.0)
    {
        SHR_EXIT();
    }
    avrg_distance = 1.0 * calendar_len / nof_slots;

    first_slot = -1;
    second_slot = -1;
    prev_slot = -1;
    last_slot = -1;
    for (slot_index = 0; slot_index < calendar_len; slot_index++)
    {
        if (calendar[slot_index] != object)
        {
            continue;
        }
        if (first_slot == -1)
        {
            first_slot = slot_index;
            prev_slot = slot_index;
            continue;
        }
        if (second_slot == -1)
        {
            second_slot = slot_index;
            last_slot = slot_index;
        }
        else
        {
            distance = slot_index - prev_slot;
            if (distance < trunc((level - deviation) * avrg_distance)
                || distance > ceil((level + deviation) * avrg_distance))
            {
                *passed = FALSE;
                if (dnx_algo_cal_verify_debug)
                {
                    LOG_ERROR_EX(BSL_LOG_MODULE, "check %d < distance < %d failed. distance = %d, avrg_distance = %f\n",
                                 (int) trunc((level - deviation) * avrg_distance),
                                 (int) ceil((level + deviation) * avrg_distance), (int) distance, avrg_distance);
                }
                break;
            }
            prev_slot = last_slot;
            last_slot = slot_index;
        }
    }
    if (*passed)
    {
        distance = calendar_len - prev_slot + first_slot;
        if (distance < trunc((level - deviation) * avrg_distance)
            || distance > ceil((level + deviation) * avrg_distance))
        {
            *passed = FALSE;
            if (dnx_algo_cal_verify_debug)
            {
                LOG_ERROR_EX(BSL_LOG_MODULE, "check %d < distance < %d failed. distance = %d, avrg_distance = %f\n",
                             (int) trunc((level - deviation) * avrg_distance),
                             (int) ceil((level + deviation) * avrg_distance), (int) distance, avrg_distance);
            }
        }

        distance = calendar_len - last_slot + second_slot;
        if (distance < trunc((level - deviation) * avrg_distance)
            || distance > ceil((level + deviation) * avrg_distance))
        {
            *passed = FALSE;
            if (dnx_algo_cal_verify_debug)
            {
                LOG_ERROR_EX(BSL_LOG_MODULE, "check %d < distance < %d failed. distance = %d, avrg_distance = %f\n",
                             (int) trunc((level - deviation) * avrg_distance),
                             (int) ceil((level + deviation) * avrg_distance), (int) distance, avrg_distance);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_algo_cal_double_distance_all_check(
    int unit,
    int nof_objects,
    int calendar_len,
    uint32 *calendar,
    int *passed)
{
    int object, nof_slots, slot_index;
    SHR_FUNC_INIT_VARS(unit);

    *passed = TRUE;

    for (object = 0; object < nof_objects; object++)
    {
        nof_slots = 0;
        for (slot_index = 0; slot_index < calendar_len; slot_index++)
        {
            if (calendar[slot_index] == object)
            {
                nof_slots++;
            }
        }

        SHR_IF_ERR_EXIT(dnx_algo_cal_double_distance_cyclic_check
                        (unit, object, nof_slots, 0.4, calendar, calendar_len, passed));

        if (*passed == FALSE)
        {
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_cal_distance_transition_check(
    int unit,
    int object,
    int nof_slots,
    int level,
    float deviation,
    uint32 *calendar,
    uint32 *old_calendar,
    int calendar_length,
    int *passed)
{
    double avrg_distance;
    int slot_index, new_counter = 0, old_counter = 0, distance;
    int old_slots[DNX_ALGO_CAL_MAX_LEVEL_DISTANCE_CHECK] = { 0 };
    int new_slots[DNX_ALGO_CAL_MAX_LEVEL_DISTANCE_CHECK] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    *passed = TRUE;
    if (nof_slots < level)
    {
        SHR_EXIT();
    }
    avrg_distance = 1.0 * calendar_length / nof_slots;
    for (slot_index = 0; slot_index < calendar_length; ++slot_index)
    {
        if (calendar[slot_index] != object)
        {
            continue;
        }
        else
        {
            new_slots[new_counter] = slot_index;
            ++new_counter;
            if (new_counter == level)
            {
                break;
            }
        }
    }

    for (slot_index = calendar_length - 1; slot_index >= 0; --slot_index)
    {
        if (old_calendar[slot_index] != object)
        {
            continue;
        }
        else
        {
            old_slots[(level - 1) - old_counter] = slot_index;
            ++old_counter;
            if (old_counter == level)
            {
                break;
            }
        }
    }

    for (slot_index = 0; slot_index < level; ++slot_index)
    {
        distance = new_slots[slot_index] - old_slots[slot_index] + calendar_length;
        if (distance < trunc((level - deviation) * avrg_distance)
            || distance > ceil((level + deviation) * avrg_distance))
        {
            *passed = FALSE;
            if (dnx_algo_cal_verify_debug)
            {
                cli_out
                    ("check transition from old calendar to new calendar of level %d: %d < distance < %d failed. distance = %d, avrg_distance = %f\n",
                     level, (int) trunc((level - deviation) * avrg_distance),
                     (int) ceil((level + deviation) * avrg_distance), (int) distance, avrg_distance);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_cal_triple_distance_cyclic_check(
    int unit,
    int object,
    int nof_slots,
    float deviation,
    uint32 *calendar,
    int calendar_len,
    int *passed)
{
    double avrg_distance, distance;
    int slot_index, first_slot, second_slot, third_slot, before_prev_slot, prev_slot, last_slot, level = 3;

    SHR_FUNC_INIT_VARS(unit);

    *passed = TRUE;
    if (nof_slots <= 3)
    {
        SHR_EXIT();
    }
    avrg_distance = 1.0 * calendar_len / nof_slots;
    first_slot = -1;
    second_slot = -1;
    third_slot = -1;
    before_prev_slot = -1;
    prev_slot = -1;
    last_slot = -1;
    for (slot_index = 0; slot_index < calendar_len; slot_index++)
    {
        if (calendar[slot_index] != object)
        {
            continue;
        }
        if (first_slot == -1)
        {
            first_slot = slot_index;
            before_prev_slot = slot_index;
            continue;
        }
        if (second_slot == -1)
        {
            second_slot = slot_index;
            prev_slot = slot_index;
            continue;
        }
        if (third_slot == -1)
        {
            third_slot = slot_index;
            last_slot = slot_index;
            continue;
        }
        else
        {
            distance = slot_index - before_prev_slot;
            if (distance < trunc((level - deviation) * avrg_distance)
                || distance > ceil((level + deviation) * avrg_distance))
            {
                *passed = FALSE;
                if (dnx_algo_cal_verify_debug)
                {
                    LOG_ERROR_EX(BSL_LOG_MODULE, "check %d < distance < %d failed. distance = %d, avrg_distance = %f\n",
                                 (int) trunc((level - deviation) * avrg_distance),
                                 (int) ceil((level + deviation) * avrg_distance), (int) distance, avrg_distance);
                }
                break;
            }
            before_prev_slot = prev_slot;
            prev_slot = last_slot;
            last_slot = slot_index;
        }
    }
    if (*passed)
    {
        distance = calendar_len - before_prev_slot + first_slot;
        if (distance < trunc((level - deviation) * avrg_distance)
            || distance > ceil((level + deviation) * avrg_distance))
        {
            *passed = FALSE;
            if (dnx_algo_cal_verify_debug)
            {
                LOG_ERROR_EX(BSL_LOG_MODULE, "check %d < distance < %d failed. distance = %d, avrg_distance = %f\n",
                             (int) trunc((level - deviation) * avrg_distance),
                             (int) ceil((level + deviation) * avrg_distance), (int) distance, avrg_distance);
            }
        }

        distance = calendar_len - prev_slot + second_slot;
        if (distance < trunc((level - deviation) * avrg_distance)
            || distance > ceil((level + deviation) * avrg_distance))
        {
            *passed = FALSE;
            if (dnx_algo_cal_verify_debug)
            {
                LOG_ERROR_EX(BSL_LOG_MODULE, "check %d < distance < %d failed. distance = %d, avrg_distance = %f\n",
                             (int) trunc((level - deviation) * avrg_distance),
                             (int) ceil((level + deviation) * avrg_distance), (int) distance, avrg_distance);
            }
        }

        distance = calendar_len - last_slot + third_slot;
        if (distance < trunc((level - deviation) * avrg_distance)
            || distance > ceil((level + deviation) * avrg_distance))
        {
            *passed = FALSE;
            if (dnx_algo_cal_verify_debug)
            {
                LOG_ERROR_EX(BSL_LOG_MODULE, "check %d < distance < %d failed. distance = %d, avrg_distance = %f\n",
                             (int) trunc((level - deviation) * avrg_distance),
                             (int) ceil((level + deviation) * avrg_distance), (int) distance, avrg_distance);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_algo_cal_triple_distance_all_check(
    int unit,
    int nof_objects,
    int calendar_len,
    uint32 *calendar,
    int *passed)
{
    int slot_index, object, nof_slots;
    SHR_FUNC_INIT_VARS(unit);

    *passed = TRUE;

    for (object = 0; object < nof_objects; object++)
    {
        nof_slots = 0;
        for (slot_index = 0; slot_index < calendar_len; slot_index++)
        {
            if (calendar[slot_index] == object)
            {
                nof_slots++;
            }
        }

        SHR_IF_ERR_EXIT(dnx_algo_cal_triple_distance_cyclic_check
                        (unit, object, nof_slots, 1.0, calendar, calendar_len, passed));

        if (*passed == FALSE)
        {
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

typedef struct
{
    int id;
    int nof_slots;
} dnx_algo_cal_object_t;

/*
 * \brief
 *   Compare between nof slots in 2 dnx_algo_cal_object_t structs.
 */
int
dnx_algo_cal_random_start_cmp(
    void *a,
    void *b)
{
    /*
     * ordering should be in a decreasing order
     */
    return ((dnx_algo_cal_object_t *) b)->nof_slots - ((dnx_algo_cal_object_t *) a)->nof_slots;
}

int
dnx_algo_cal_random_int_cmp(
    void *a,
    void *b)
{
    /*
     * ordering should be in a increasing order
     */
    return (*(int *) a) - (*(int *) b);
}

shr_error_e
dnx_algo_cal_random_start_build(
    int unit,
    uint32 *slots_per_object,
    int nof_objects,
    int calendar_len,
    int max_calendar_len,
    UTILEX_RAND * rand_state,
    dnx_algo_cal_random_object_transition_info_t * trans_info,
    uint32 *calendar)
{
    uint32 slot_idx, obj_index;
    dnx_algo_cal_object_t *slots_per_object_temp = NULL;
    uint32 *calendar_temp_p = NULL;
    uint32 *calendar_free_slots_p = NULL;
    uint32 *slot_to_free_index_map = NULL;
    int free_slots_total;
    double distance, cnt;
    int curr_slot;
    int obj_slots_nof;
    int y, neg;
    int i;
    int free_index;
    int swap_slot_index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_ERR_EXIT(slots_per_object_temp, sizeof(*slots_per_object_temp) * nof_objects, "slots_per_object_temp",
                       "%s%s%s", EMPTY, EMPTY, EMPTY);

    for (i = 0; i < nof_objects; i++)
    {
        slots_per_object_temp[i].id = i;
        slots_per_object_temp[i].nof_slots = slots_per_object[i];
    }

    /** Sort objects according to number of slots */
    _shr_sort(slots_per_object_temp, nof_objects, sizeof(*slots_per_object_temp), &dnx_algo_cal_random_start_cmp);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(calendar_temp_p, sizeof(*calendar_temp_p) * calendar_len, "calendar_temp_p",
                                "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO_ERR_EXIT(calendar_free_slots_p, sizeof(*calendar_free_slots_p) * calendar_len,
                                "calendar_free_slots_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO_ERR_EXIT(slot_to_free_index_map, sizeof(*slot_to_free_index_map) * calendar_len,
                                "slot_to_free_index_map", "%s%s%s", EMPTY, EMPTY, EMPTY);

    /** Clear the calendar */
    for (slot_idx = 0; slot_idx < calendar_len; ++slot_idx)
    {
        calendar_temp_p[slot_idx] = DNX_ALGO_CAL_ILLEGAL_OBJECT_ID;
        calendar_free_slots_p[slot_idx] = slot_idx;
        slot_to_free_index_map[slot_idx] = slot_idx;
    }
    free_slots_total = calendar_len;

    for (obj_index = 0; obj_index < nof_objects; obj_index++)
    {
        obj_slots_nof = slots_per_object_temp[obj_index].nof_slots;
        if (obj_slots_nof == 0)
        {
            break;
        }
        distance = calendar_len * 1.0 / slots_per_object_temp[obj_index].nof_slots;
        if (trans_info && trans_info[slots_per_object_temp[obj_index].id].slot_info[0].valid)
        {
            int start_index, end_index;

            _shr_sort(calendar_free_slots_p, free_slots_total, sizeof(int), &dnx_algo_cal_random_int_cmp);
            for (i = 0; i < free_slots_total; ++i)
            {
                slot_to_free_index_map[calendar_free_slots_p[i]] = i;
            }

            for (i = 0; i < free_slots_total && calendar_free_slots_p[i] <
                 trans_info[slots_per_object_temp[obj_index].id].slot_info[0].min_first_slot; i++)
            {
            }
            start_index = i;
            for (i = start_index; i < free_slots_total && calendar_free_slots_p[i] <
                 trans_info[slots_per_object_temp[obj_index].id].slot_info[0].max_first_slot; i++)
            {
            }
            end_index = i - 1;
            if (end_index < start_index)
            {
                free_index = utilex_rand_modulo(rand_state, free_slots_total);
            }
            else
            {
                free_index = utilex_rand_range(rand_state, start_index, end_index);
            }
        }
        else
        {
            free_index = utilex_rand_modulo(rand_state, free_slots_total);
        }
        curr_slot = calendar_free_slots_p[free_index];
        cnt = curr_slot;

        neg = 0;
        for (i = 0; i < obj_slots_nof; i++)
        {
            y = 1;
            while (calendar_temp_p[curr_slot] != DNX_ALGO_CAL_ILLEGAL_OBJECT_ID)
            {
                curr_slot = (neg == 0 ? curr_slot + y : curr_slot - y);
                curr_slot = curr_slot % calendar_len;
                if (curr_slot < 0)
                {
                    curr_slot += calendar_len;
                }
                y++;
                neg = !neg;
            }
            calendar_temp_p[curr_slot] = slots_per_object_temp[obj_index].id;

            /** remove curr_slot from free list */
            free_index = slot_to_free_index_map[curr_slot];

            swap_slot_index = calendar_free_slots_p[free_slots_total - 1];
            calendar_free_slots_p[free_index] = swap_slot_index;
            slot_to_free_index_map[swap_slot_index] = free_index;

            free_slots_total--;

            cnt += distance;
            curr_slot = ((int) round(cnt)) % calendar_len;
            neg = (cnt > round(cnt) ? 0 : 1);
        }
    }

    for (slot_idx = 0; slot_idx < max_calendar_len; ++slot_idx)
    {
        calendar[slot_idx] = DNX_ALGO_CAL_ILLEGAL_OBJECT_ID;
    }
    sal_memcpy(calendar, calendar_temp_p, sizeof(*calendar) * calendar_len);

exit:
    SHR_FREE(slots_per_object_temp);
    SHR_FREE(calendar_temp_p);
    SHR_FREE(calendar_free_slots_p);
    SHR_FREE(slot_to_free_index_map);

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_cal_random_start_build_cb(
    int unit,
    uint32 *slots_per_object,
    int nof_objects,
    int calendar_len,
    int max_calendar_len,
    void *additional_info,
    uint32 *calendar)
{
    UTILEX_RAND *rand_state = (UTILEX_RAND *) additional_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_cal_random_start_build(unit, slots_per_object, nof_objects, calendar_len, max_calendar_len,
                                                    rand_state, NULL, calendar));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_cal_validate(
    int unit,
    int nof_objects,
    int calendar_len,
    uint32 *calendar,
    int *passed)
{

    SHR_FUNC_INIT_VARS(unit);

    *passed = TRUE;

    if (*passed)
    {
        SHR_IF_ERR_EXIT(dnx_algo_cal_distance_all_check(unit, nof_objects, calendar_len, calendar, passed));
    }

    if (*passed)
    {
        SHR_IF_ERR_EXIT(dnx_algo_cal_double_distance_all_check(unit, nof_objects, calendar_len, calendar, passed));
    }

    if (*passed)
    {
        SHR_IF_ERR_EXIT(dnx_algo_cal_triple_distance_all_check(unit, nof_objects, calendar_len, calendar, passed));
    }

exit:
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_algo_cal_random_start_build_and_validate(
    int unit,
    int group_by_nof_slots,
    uint32 *slots_per_object,
    int nof_objects,
    int calendar_len,
    int max_calendar_len,
    uint32 *calendar)
{
    int iterations = 10;
    int i;
    int passed = FALSE;

    UTILEX_RAND rand_state;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&rand_state, 0, sizeof(rand_state));

    for (i = 0; i < iterations; i++)
    {
        if (group_by_nof_slots)
        {
            SHR_IF_ERR_EXIT(dnx_algo_cal_group_and_build(unit, dnx_algo_cal_random_start_build_cb,
                                                         slots_per_object, nof_objects, calendar_len, max_calendar_len,
                                                         &rand_state, calendar));

        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_cal_random_start_build(unit, slots_per_object, nof_objects, calendar_len,
                                                            max_calendar_len, &rand_state, NULL, calendar));
        }
        SHR_IF_ERR_EXIT(dnx_algo_cal_validate(unit, nof_objects, calendar_len, calendar, &passed));
        if (passed)
        {
            if (dnx_algo_cal_verbose)
            {
                cli_out("Passed after %d iterations\n", i);
            }
            break;
        }
    }

    if (!passed)
    {
        if (dnx_algo_cal_verbose)
        {
            cli_out("nof_objects=%d calendar_len=%d\n", nof_objects, calendar_len);
            for (i = 0; i < nof_objects; i++)
            {
                cli_out("%d ", slots_per_object[i]);
            }
            cli_out("\n");
        }
        SHR_ERR_EXIT(_SHR_E_FAIL, "Calendar calculation failed\n");

    }
exit:
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_algo_cal_random_start_group_build(
    int unit,
    uint32 *slots_per_object,
    int nof_objects,
    int calendar_len,
    int max_calendar_len,
    uint32 *calendar)
{

    SHR_FUNC_INIT_VARS(unit);

    DNX_INIT_TIME_ANALYZER_ALGO_CAL_START(unit, DNX_INIT_TIME_ANALYZER_ALGO_CAL_RANDOM);

    SHR_IF_ERR_EXIT(dnx_algo_cal_random_start_build_and_validate(unit, TRUE, slots_per_object, nof_objects,
                                                                 calendar_len, max_calendar_len, calendar));
    DNX_INIT_TIME_ANALYZER_ALGO_CAL_STOP(unit, DNX_INIT_TIME_ANALYZER_ALGO_CAL_RANDOM);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_cal_random_build_and_fix_impl(
    int unit,
    uint32 *slots_per_object,
    int nof_objects,
    int calendar_len,
    int max_calendar_len,
    int do_group,
    dnx_algo_cal_random_object_transition_info_t * trans_info,
    uint32 *calendar)
{
    int i;
    dnx_algo_cal_detailed_entry_t *cal_dist_info = NULL;

    UTILEX_RAND rand_state;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&rand_state, 0, sizeof(rand_state));

    if (do_group)
    {
        /*
         * when grouping is used, 
         * first slot constraint is ignored
         */
        SHR_IF_ERR_EXIT(dnx_algo_cal_group_and_build(unit, dnx_algo_cal_random_start_build_cb,
                                                     slots_per_object, nof_objects, calendar_len, max_calendar_len,
                                                     &rand_state, calendar));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_cal_random_start_build(unit,
                                                        slots_per_object, nof_objects, calendar_len, max_calendar_len,
                                                        &rand_state, trans_info, calendar));

    }

    SHR_ALLOC_ERR_EXIT(cal_dist_info, sizeof(dnx_algo_cal_detailed_entry_t) * calendar_len * 3, "calendar info",
                       "%s%s%s", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(algo_cal_dist_calc
                    (unit, nof_objects, calendar_len, calendar, 3, 1.0, 0, FALSE, NULL, cal_dist_info));
    SHR_IF_ERR_EXIT(algo_cal_dist_calc
                    (unit, nof_objects, calendar_len, calendar, 2, 0.8, 0, FALSE, NULL, &cal_dist_info[calendar_len]));
    SHR_IF_ERR_EXIT(algo_cal_dist_calc
                    (unit, nof_objects, calendar_len, calendar, 1, 0.5, 0, FALSE, NULL,
                     &cal_dist_info[2 * calendar_len]));
    SHR_IF_ERR_EXIT(algo_cal_fix(unit, calendar_len, 3, 3, 1, cal_dist_info));

    if (trans_info)
    {
        SHR_IF_ERR_EXIT(algo_cal_transition_fix(unit, calendar_len, nof_objects, trans_info, 3, cal_dist_info));
    }
    for (i = 0; i < calendar_len; i++)
    {
        calendar[i] = cal_dist_info[i].object;
    }

exit:
    SHR_FREE(cal_dist_info);
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_algo_cal_random_build_and_fix(
    int unit,
    uint32 *slots_per_object,
    int nof_objects,
    int calendar_len,
    int max_calendar_len,
    uint32 *calendar)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_cal_random_build_and_fix_impl(unit, slots_per_object, nof_objects, calendar_len,
                                                           max_calendar_len, TRUE, NULL, calendar));

exit:
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_algo_cal_random_build_and_fix_no_groupping(
    int unit,
    uint32 *slots_per_object,
    int nof_objects,
    int calendar_len,
    int max_calendar_len,
    uint32 *calendar)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_cal_random_build_and_fix_impl(unit, slots_per_object, nof_objects, calendar_len,
                                                           max_calendar_len, FALSE, NULL, calendar));

exit:
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_algo_cal_random_constraint_build_and_fix(
    int unit,
    uint32 *slots_per_object,
    int nof_objects,
    int calendar_len,
    int max_calendar_len,
    dnx_algo_cal_random_object_transition_info_t * trans_info,
    uint32 *calendar)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_cal_random_build_and_fix_impl(unit, slots_per_object, nof_objects, calendar_len,
                                                           max_calendar_len, FALSE, trans_info, calendar));

exit:
    SHR_FUNC_EXIT;

}
