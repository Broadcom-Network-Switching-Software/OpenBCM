
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/dnx/field/tcam/tcam_location_algo.h>
#include <bcm_int/dnx/field/tcam/tcam_access_profile_manager.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_location_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_field.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#define DNX_FIELD_TCAM_LOCATION_IS_LOCATION_BANK_ODD(location) (DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(location) % (2))


static shr_error_e
dnx_field_tcam_location_algo_insertion_range_get(
    int unit,
    int core,
    uint32 handler_id,
    uint32 priority,
    dnx_field_tcam_location_range_t * insertion_range)
{
    uint8 found;
    sw_state_ll_node_t iter, prev_iter, next_iter, last_iter;
    uint32 node_key;
    uint32 last_key;
    dnx_field_tcam_location_range_t next_data, prev_data;

    SHR_FUNC_INIT_VARS(unit);

    
    priority++;

    node_key = priority;

    
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                    priorities_range.get_last(unit, handler_id, core, &last_iter));

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                    priorities_range.previous_node(unit, handler_id, core, last_iter, &iter));

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                    priorities_range.node_key(unit, handler_id, core, iter, &last_key));

    if (last_key <= node_key)
    {
        
        found = (last_key == node_key);
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                        priorities_range.find(unit, handler_id, core, &iter, &node_key, &found));
    }

    if (found)
    {
        
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                        priorities_range.previous_node(unit, handler_id, core, iter, &prev_iter));
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                        priorities_range.node_value(unit, handler_id, core, prev_iter, &prev_data));
    }
    else
    {
        
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                        priorities_range.node_value(unit, handler_id, core, iter, &prev_data));
    }
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                    priorities_range.next_node(unit, handler_id, core, iter, &next_iter));
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                    priorities_range.node_value(unit, handler_id, core, next_iter, &next_data));

    
    insertion_range->min = prev_data.max + 1;
    insertion_range->max = next_data.min - 1;
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "insertion range: min: %d max: %d%s%s\n", insertion_range->min, insertion_range->max, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_location_algo_priority_list_entry_remove(
    int unit,
    int core,
    uint32 handler_id,
    uint32 place)
{
    uint8 found = TRUE;
    sw_state_ll_node_t iter;
    dnx_field_tcam_location_range_t range;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                    priorities_range.get_first(unit, handler_id, core, &iter));
    while (DNX_SW_STATE_LL_IS_NODE_VALID(iter))
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                        priorities_range.node_value(unit, handler_id, core, iter, &range));
        if (range.min <= place && place <= range.max)
        {
            
            if (range.min == range.max)
            {
                SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                                priorities_range.remove_node(unit, handler_id, core, iter));
                break;
            }

            if (range.min == place)
            {
                
                SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_next_get
                                (unit, core, handler_id, range.min, &(range.min), &found));
            }
            else if (range.max == place)
            {
                
                SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_prev_get
                                (unit, core, handler_id, range.max, &(range.max), &found));
            }

            SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                            priorities_range.node_update(unit, handler_id, core, iter, &range));
            break;
        }
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                        priorities_range.next_node(unit, handler_id, core, iter, &iter));
    }

    if (!DNX_SW_STATE_LL_IS_NODE_VALID(iter))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Entry with absolute location %d was not found in priority list", place);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_location_algo_priority_list_entry_add(
    int unit,
    int core,
    uint32 handler_id,
    uint32 priority,
    uint32 place)
{
    uint8 found;
    sw_state_ll_node_t iter;
    sw_state_ll_node_t last_iter;
    uint32 key;
    uint32 last_key;
    dnx_field_tcam_location_range_t range;

    SHR_FUNC_INIT_VARS(unit);

    
    priority++;

    key = priority;

    
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                    priorities_range.get_last(unit, handler_id, core, &last_iter));

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                    priorities_range.previous_node(unit, handler_id, core, last_iter, &iter));

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                    priorities_range.node_key(unit, handler_id, core, iter, &last_key));

    if (last_key <= key)
    {
        
        found = (last_key == key);
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                        priorities_range.find(unit, handler_id, core, &iter, &key, &found));
    }

    if (found)
    {
        
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                        priorities_range.node_value(unit, handler_id, core, iter, &range));
        if (range.min > place)
        {
            range.min = place;
        }
        if (range.max < place)
        {
            range.max = place;
        }
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                        priorities_range.node_update(unit, handler_id, core, iter, &range));
    }
    else
    {
        
        range.min = place;
        range.max = place;

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                        priorities_range.add(unit, handler_id, core, &key, &range));

        LOG_DEBUG_EX(BSL_LOG_MODULE, "Added node %d with place %d%s%s\n", priority, place, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_location_algo_search_for_entry(
    int unit,
    int core,
    uint32 key_size,
    dnx_field_tcam_location_range_t * search_range,
    uint8 forward,
    uint32 *place,
    uint8 *found)
{
    SHR_FUNC_INIT_VARS(unit);

    *found = FALSE;

    while (!*found)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                        tcam_banks_occupation_bitmap.get_next_in_range(unit, core, search_range->min, search_range->max,
                                                                       forward, 0, place, found));
        if (*found)
        {
            
            uint8 occupied1 = FALSE;
            uint8 occupied2 = FALSE;
            uint8 occupied3 = FALSE;

            
            if (key_size != dnx_data_field.tcam.key_size_half_get(unit) && *place % 2)
            {
                
                if (forward)
                {
                    search_range->min = *place + 1;
                }
                else
                {
                    search_range->max = *place - 1;
                }
                *found = FALSE;
                continue;
            }

            
            if (key_size >= dnx_data_field.tcam.key_size_single_get(unit))
            {
                SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                                tcam_banks_occupation_bitmap.is_occupied(unit, core, *place + 1, &occupied1));
            }
            if (key_size == dnx_data_field.tcam.key_size_double_get(unit))
            {
                uint32 next_bank_place = *place + dnx_data_field.tcam.hw_bank_size_get(unit);
                SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                                tcam_banks_occupation_bitmap.is_occupied(unit, core, next_bank_place, &occupied2));
                SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                                tcam_banks_occupation_bitmap.is_occupied(unit, core, next_bank_place + 1, &occupied3));
            }
            
            if (occupied1 | occupied2 | occupied3)
            {
                
                if (forward)
                {
                    search_range->min = *place + 1;
                }
                else
                {
                    search_range->max = *place - 1;
                }
                *found = FALSE;
            }
        }
        else
        {
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_location_algo_suggestion_revise(
    int unit,
    uint32 key_size,
    uint32 *suggestion)
{
    uint32 bank_id;

    SHR_FUNC_INIT_VARS(unit);

    if (*suggestion == FIELD_TCAM_LOCATION_INVALID)
    {
        SHR_EXIT();
    }

    bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(*suggestion);
    if (!FIELD_TCAM_BANK_IS_BIG_BANK(bank_id))
    {
        
        if (*suggestion > FIELD_TCAM_LOCATION_ALGO_MAX_BANK_INDEX(bank_id))
        {
            
            *suggestion = FIELD_TCAM_LOCATION_ALGO_MIN_BANK_INDEX(bank_id + 1);
        }
    }
    if (key_size == dnx_data_field.tcam.key_size_double_get(unit))
    {
        
        bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(*suggestion);
        if (bank_id % 2)
        {
            
            *suggestion = FIELD_TCAM_LOCATION_ALGO_MIN_BANK_INDEX(bank_id + 1);
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_location_algo_search_in_range(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_location_range_t * range,
    int *bank_ids,
    int bank_count,
    uint32 *place,
    uint8 *found)
{
    int bank_index;
    dnx_field_tcam_location_range_t bank_range;
    dnx_field_tcam_location_range_t search_range;
    uint32 key_size;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.key_size.get(unit, handler_id, &key_size));

    *found = FALSE;
    search_range.min = -1;
    search_range.max = -1;

    bank_range.max = dnx_data_field.tcam.tcam_banks_size_get(unit);
    for (bank_index = 0; bank_index < bank_count; bank_index++)
    {
        if (key_size == dnx_data_field.tcam.key_size_double_get(unit) && bank_ids[bank_index] % 2)
        {
            
            continue;
        }
        
        bank_range.min = FIELD_TCAM_LOCATION_ALGO_MIN_BANK_INDEX(bank_ids[bank_index]);
        bank_range.max = FIELD_TCAM_LOCATION_ALGO_MAX_BANK_INDEX(bank_ids[bank_index]);
        
        if ((bank_range.min > range->max) || (range->max == -1))
        {
            
            break;
        }
        if (range->min > bank_range.max)
        {
            
            continue;
        }
        
        search_range.min = MAX(bank_range.min, range->min);
        search_range.max = MIN(bank_range.max, range->max);
        
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_search_for_entry
                        (unit, core, key_size, &search_range, TRUE, place, found));

        if (*found)
        {
            break;
        }
    }
    if (!(*found))
    {
        

        
        uint32 global_min = (bank_count == 0) ? 0 : FIELD_TCAM_LOCATION_ALGO_MIN_BANK_INDEX(bank_ids[0]);
        uint32 global_max =
            (bank_count == 0) ? 0 : FIELD_TCAM_LOCATION_ALGO_MAX_BANK_INDEX(bank_ids[bank_count - 1]) + 1;
        if (range->max == -1)
        {
            
            *place = 0;
        }
        else if (range->max < global_max)
        {
            
            *place = range->max + 1;
        }
        else if (range->min > global_min)
        {
            
            *place = range->min + 1;
        }
        else
        {
            
            *place = FIELD_TCAM_LOCATION_INVALID;
        }
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_suggestion_revise(unit, key_size, place));
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Search in range returned %d found: %d%s%s\n", *place, *found, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_location_algo_occ_bitmap_state_change(
    int unit,
    int core,
    uint32 key_size,
    uint32 location,
    uint8 new_state)
{

    SHR_FUNC_INIT_VARS(unit);

    
    
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                    tcam_banks_occupation_bitmap.status_set(unit, core, location, new_state));
    if (key_size >= dnx_data_field.tcam.key_size_single_get(unit))
    {
        
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                        tcam_banks_occupation_bitmap.status_set(unit, core, location + 1, new_state));
    }
    if (key_size == dnx_data_field.tcam.key_size_double_get(unit))
    {
        
        uint32 next_bank_location = location + dnx_data_field.tcam.hw_bank_size_get(unit);
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                        tcam_banks_occupation_bitmap.status_set(unit, core, next_bank_location, new_state));
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                        tcam_banks_occupation_bitmap.status_set(unit, core, next_bank_location + 1, new_state));
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_location_algo_occ_bitmap_occupy(
    int unit,
    int core,
    uint32 key_size,
    uint32 location)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_occ_bitmap_state_change(unit, core, key_size, location, TRUE));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_location_algo_occ_bitmap_free(
    int unit,
    int core,
    uint32 key_size,
    uint32 location)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_occ_bitmap_state_change(unit, core, key_size, location, FALSE));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_location_algo_handler_bitmap_state_change(
    int unit,
    int core,
    uint32 handler_id,
    uint32 key_size,
    uint32 location,
    uint8 new_state)
{

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                    entries_occupation_bitmap.status_set(unit, handler_id, core, location, new_state));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_location_algo_handler_bitmap_occupy(
    int unit,
    int core,
    uint32 handler_id,
    uint32 key_size,
    uint32 location)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_bitmap_state_change
                    (unit, core, handler_id, key_size, location, TRUE));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_location_algo_handler_bitmap_free(
    int unit,
    int core,
    uint32 handler_id,
    uint32 key_size,
    uint32 location)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_bitmap_state_change
                    (unit, core, handler_id, key_size, location, FALSE));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_algo_update_free(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_location_t * location,
    uint8 is_dt)
{

    uint32 key_size;
    uint32 place;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.key_size.get(unit, handler_id, &key_size));

    place = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(location->bank_id, location->bank_offset);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_bitmap_free(unit, core, handler_id, key_size, place));

    if (!is_dt)
    {
        
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_occ_bitmap_free(unit, core, key_size, place));

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_priority_list_entry_remove(unit, core, handler_id, place));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_algo_update_alloc(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_entry_t * entry,
    dnx_field_tcam_location_t * location,
    uint8 is_dt)
{

    uint32 key_size;
    uint32 insertion_place;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.key_size.get(unit, handler_id, &key_size));

    insertion_place = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(location->bank_id, location->bank_offset);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_bitmap_occupy
                    (unit, core, handler_id, key_size, insertion_place));

    if (!is_dt)
    {
        

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_occ_bitmap_occupy(unit, core, key_size, insertion_place));

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_priority_list_entry_add
                        (unit, core, handler_id, entry->priority, insertion_place));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_algo_find(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_entry_t * entry,
    dnx_field_tcam_location_t * location,
    uint8 *found)
{
    int *bank_ids_p = NULL;

    int bank_count;
    dnx_field_tcam_location_range_t insertion_range;
    uint32 insertion_place;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(bank_ids_p, sizeof(*bank_ids_p) * dnx_data_field.tcam.nof_banks_get(unit), "bank_ids_p", "%s%s%s", EMPTY,
              EMPTY, EMPTY);

    insertion_range.min = 0;
    insertion_range.max = 0;

    
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_banks_sorted_get(unit, handler_id, bank_ids_p, &bank_count));

    
    
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_insertion_range_get
                    (unit, core, handler_id, entry->priority, &insertion_range));

    
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_search_in_range(unit, core, handler_id,
                                                                 &insertion_range,
                                                                 bank_ids_p, bank_count, &insertion_place, found));

    
    location->bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(insertion_place);
    location->bank_offset = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(insertion_place);

exit:
    SHR_FREE(bank_ids_p);
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_location_algo_prio_list_move(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location_from,
    uint32 location_to)
{
    sw_state_ll_node_t iter;
    dnx_field_tcam_location_range_t range;
    uint8 found;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                    priorities_range.get_first(unit, handler_id, core, &iter));
    while (DNX_SW_STATE_LL_IS_NODE_VALID(iter))
    {
        found = TRUE;
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                        priorities_range.node_value(unit, handler_id, core, iter, &range));
        
        if (range.min >= location_from && range.min <= location_to)
        {
            
            SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_next_get
                            (unit, core, handler_id, range.min, &(range.min), &found));
        }
        else if (range.min <= location_from && range.min >= location_to)
        {
            
            SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_prev_get
                            (unit, core, handler_id, range.min, &(range.min), &found));
        }
        
        if (!found)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error while moving in priority list for"
                         " handler %d\r\n No next or prev found for location %d\r\n", handler_id, range.min);
        }

        if (range.max >= location_from && range.max <= location_to)
        {
            
            SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_next_get
                            (unit, core, handler_id, range.max, &(range.max), &found));
        }
        else if (range.max <= location_from && range.max >= location_to)
        {
            
            SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_prev_get
                            (unit, core, handler_id, range.max, &(range.max), &found));
        }
        
        if (!found)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error while moving in priority list for"
                         "handler %d\r\n No next or prev found for location %d\r\n", handler_id, range.max);
        }

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                        priorities_range.node_update(unit, handler_id, core, iter, &range));

        
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                        priorities_range.next_node(unit, handler_id, core, iter, &iter));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_algo_mark_as_occupied(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location)
{
    uint32 key_size;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.key_size.get(unit, handler_id, &key_size));
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_occ_bitmap_occupy(unit, core, key_size, location));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_algo_move(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location_from,
    uint32 location_to)
{
    uint32 key_size;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.key_size.get(unit, handler_id, &key_size));

    
    
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_occ_bitmap_free(unit, core, key_size, location_from));
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_occ_bitmap_occupy(unit, core, key_size, location_to));

    
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_bitmap_free(unit, core, handler_id, key_size, location_from));
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_bitmap_occupy(unit, core, handler_id, key_size, location_to));

    
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_prio_list_move(unit, core, handler_id, location_from, location_to));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_algo_handler_next_get_extended(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location,
    uint8 backward,
    uint32 *next_location,
    uint8 *found)
{
    SHR_FUNC_INIT_VARS(unit);

    if (backward)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_prev_get
                        (unit, core, handler_id, location, next_location, found));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_next_get
                        (unit, core, handler_id, location, next_location, found));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_algo_handler_next_get(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location,
    uint32 *next_location,
    uint8 *found)
{
    uint32 max_size;

    SHR_FUNC_INIT_VARS(unit);

    max_size = dnx_data_field.tcam.tcam_banks_size_get(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                    entries_occupation_bitmap.get_next_in_range(unit, handler_id, core, location + 1, max_size, TRUE, 1,
                                                                next_location, found));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_algo_handler_prev_get(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location,
    uint32 *prev_location,
    uint8 *found)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                    entries_occupation_bitmap.get_next_in_range(unit, handler_id, core, 0, location - 1, FALSE, 1,
                                                                prev_location, found));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_algo_entry_comp_state(
    int unit,
    int core,
    uint32 location,
    uint8 *single_occupied,
    uint8 *double_occupied)
{
    uint8 d1_occupied;
    uint8 d2_occupied;
    uint32 comp_single_location;
    uint32 bank_id;
    uint32 comp_bank_id;
    uint32 comp_double_location1;
    uint32 comp_double_location2;

    SHR_FUNC_INIT_VARS(unit);

    

    comp_single_location = location + ((location % 2) ? -1 : 1);
    bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(location);
    comp_bank_id = bank_id + ((bank_id % 2) ? -1 : 1);
    comp_double_location1 = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(comp_bank_id,
                                                                     DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET
                                                                     (location));
    comp_double_location2 =
        DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(comp_bank_id,
                                                 DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(comp_single_location));

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                    tcam_banks_occupation_bitmap.is_occupied(unit, core, comp_single_location, single_occupied));
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                    tcam_banks_occupation_bitmap.is_occupied(unit, core, comp_double_location1, &d1_occupied));
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                    tcam_banks_occupation_bitmap.is_occupied(unit, core, comp_double_location2, &d2_occupied));

    *double_occupied = d1_occupied | d2_occupied;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_algo_empty_location_find(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_location_t * location,
    dnx_field_tcam_location_find_mode_e find_mode,
    uint32 *empty_place,
    uint8 *found)
{
    dnx_field_tcam_location_range_t search_range;
    uint32 absolute_location;
    int *bank_ids_p = NULL;

    int bank_count;
    int bank_i;
    int search_start_i = -1;
    uint32 key_size;
    uint32 forward_empty_place = 0;
    uint32 backward_empty_place = 0;
    uint8 forward_found = FALSE;
    uint8 backward_found = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(bank_ids_p, sizeof(*bank_ids_p) * dnx_data_field.tcam.nof_banks_get(unit), "bank_ids_p", "%s%s%s", EMPTY,
              EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.key_size.get(unit, handler_id, &key_size));

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_banks_sorted_get(unit, handler_id, bank_ids_p, &bank_count));

    absolute_location = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(location->bank_id, location->bank_offset);

    *found = FALSE;

    
    for (bank_i = 0; bank_i < bank_count; bank_i++)
    {
        if (bank_ids_p[bank_i] >= location->bank_id)
        {
            search_start_i = bank_i;
            break;
        }
    }

    if (search_start_i == -1)
    {
        
        search_start_i = bank_count - 1;
    }
    else if (find_mode & DNX_FIELD_TCAM_LOCATION_FIND_MODE_FORWARD)
    {

        
        for (bank_i = search_start_i; bank_i < bank_count; bank_i++)
        {
            if (key_size == dnx_data_field.tcam.key_size_double_get(unit) && bank_ids_p[bank_i] % 2)
            {
                
                continue;
            }

            search_range.min = FIELD_TCAM_LOCATION_ALGO_MIN_BANK_INDEX(bank_ids_p[bank_i]);
            search_range.max = FIELD_TCAM_LOCATION_ALGO_MAX_BANK_INDEX(bank_ids_p[bank_i]);

            if (location->bank_id == bank_ids_p[bank_i])
            {
                search_range.min = absolute_location;
            }
            SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_search_for_entry
                            (unit, core, key_size, &search_range, TRUE, &forward_empty_place, &forward_found));

            if (forward_found)
            {
                break;
            }
        }
    }

    if (find_mode & DNX_FIELD_TCAM_LOCATION_FIND_MODE_BACKWARD)
    {
        
        for (bank_i = search_start_i; bank_i >= 0; bank_i--)
        {
            if (key_size == dnx_data_field.tcam.key_size_double_get(unit) && bank_ids_p[bank_i] % 2)
            {
                
                continue;
            }

            search_range.min = FIELD_TCAM_LOCATION_ALGO_MIN_BANK_INDEX(bank_ids_p[bank_i]);
            search_range.max = FIELD_TCAM_LOCATION_ALGO_MAX_BANK_INDEX(bank_ids_p[bank_i]);

            if (location->bank_id == bank_ids_p[bank_i])
            {
                
                search_range.max = absolute_location - 1;
            }

            SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_search_for_entry
                            (unit, core, key_size, &search_range, FALSE, &backward_empty_place, &backward_found));

            if (backward_found)
            {
                break;
            }
        }
    }

    if (forward_found || backward_found)
    {
        uint32 backward_diff = absolute_location - backward_empty_place;
        uint32 forward_diff = forward_empty_place - absolute_location;
        *found = TRUE;
        
        *empty_place = (!forward_found) ?
            backward_empty_place :
            (backward_found && backward_diff < forward_diff) ? backward_empty_place : forward_empty_place;
    }

exit:
    SHR_FREE(bank_ids_p);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_algo_allocating_handler(
    int unit,
    int core,
    dnx_field_tcam_location_t * location,
    uint32 *handler_id)
{
    uint32 handler_index;
    uint32 absolute_location;
    uint8 is_occupied = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    absolute_location = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(location->bank_id, location->bank_offset);
    for (handler_index = 0; handler_index < dnx_data_field.tcam.nof_tcam_handlers_get(unit); handler_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                        entries_occupation_bitmap.is_occupied(unit, handler_index, core, absolute_location,
                                                              &is_occupied));

        if (is_occupied)
        {
            *handler_id = handler_index;
            break;
        }
    }

    if (!is_occupied)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Given location is not associated with any TCAM handler\n");
    }

exit:
    SHR_FUNC_EXIT;
}
