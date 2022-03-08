
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#include <soc/dnx/field/tcam/tcam_location_manager.h>

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>

#include <soc/dnx/field/tcam/tcam_bank_manager.h>
#include <soc/dnx/field/tcam/tcam_access_profile_manager.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/field/tcam_access/tcam_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_location_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_field.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

typedef enum
{
    DNX_FIELD_TCAM_LOCATION_MANAGER_80_OP_STATE_NEW,
    DNX_FIELD_TCAM_LOCATION_MANAGER_80_OP_STATE_COPY,
    DNX_FIELD_TCAM_LOCATION_MANAGER_80_OP_STATE_KEEP,
} dnx_field_tcam_location_manager_80_op_state_e;

struct dnx_field_tcam_location_manager_80_state
{
    dnx_field_tcam_location_manager_80_op_state_e op_state;
    uint32 free_line;
};

shr_error_e
dnx_field_tcam_location_find(
    int unit,
    bcm_core_t core,
    uint32 handler_id,
    dnx_field_tcam_entry_t * entry,
    dnx_field_tcam_location_t * location,
    uint8 *found)
{
    dnx_field_tcam_access_fg_params_t fg_params;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));

    if (fg_params.bank_allocation_mode == DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT_WITH_LOCATION)
    {
        uint8 handler_on_bank;

        location->bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(entry->priority);
        location->bank_offset = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(entry->priority);

        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_is_handler_on_bank
                        (unit, location->bank_id, handler_id, core, &handler_on_bank));
        if (!handler_on_bank)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Failed to add TCAM entry: Given location is on bank %d which in turn is not allocated by the TCAM handler %d\n",
                         location->bank_id, handler_id);
        }

        *found = TRUE;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_find(unit, core, handler_id, entry, location, found));
    }

    if (*found)
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "Found location for entry %d(prio %d): (%d, %d)\n",
                     entry->id, entry->priority, location->bank_id, location->bank_offset);
    }
    else
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "Failure finding free location! Location for"
                     "entry %d(prio %d): (%d, %d)\n",
                     entry->id, entry->priority, location->bank_id, location->bank_offset);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_is_location_occupied(
    int unit,
    int core,
    uint32 absolute_location,
    int *occupied)
{
    uint32 prio;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.location_priority_arr.get(unit, core, absolute_location, &prio));

    *occupied = (prio != DNX_FIELD_TCAM_ENTRY_PRIORITY_INVALID);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_location_hash_add(
    int unit,
    int core,
    uint32 handler_id,
    uint32 entry_id,
    dnx_field_tcam_location_t * location)
{
    uint32 absolute_location;
    uint8 success;

    SHR_FUNC_INIT_VARS(unit);

    absolute_location = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(location->bank_id, location->bank_offset);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.
                    entry_location_hash.insert_at_index(unit, core, &entry_id, absolute_location, &success));

    if (!success)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Error adding entry 0x%08x of TCAM handler %d to entry_location_hash on core %d at location 0x%08x!",
                     entry_id, handler_id, core, absolute_location);
    }
    else
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "Handler %d entry %d added to location hash at %d at core %d\n", handler_id, entry_id,
                     absolute_location, core);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_location_hash_move_one(
    int unit,
    int core,
    uint32 location_from,
    uint32 location_to)
{
    uint32 entry_id;
    uint8 found;
    uint8 success;
    uint8 valid;
    uint32 prio;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_location_hash.get_by_index
                    (unit, core, location_from, &entry_id, &found));

    if (!found)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Something went wrong while moving entries\r\n"
                     "Entry at location %d not found\r\n", location_from);
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_location_hash.delete_by_index(unit, core, location_from));

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_location_hash.insert_at_index
                    (unit, core, &entry_id, location_to, &success));

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.valid_bmp.v_bit.bit_get(unit, core, location_from, &valid));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.valid_bmp.v_bit.bit_clear(unit, core, location_from));
    if (valid)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.valid_bmp.v_bit.bit_set(unit, core, location_to));
    }

    if (!success)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Something went wrong while moving entries\r\n"
                     "Could not insert entry at absolute location %d\r\n", location_to);
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.location_priority_arr.get(unit, core, location_from, &prio));
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.location_priority_arr.set(unit, core, location_from,
                                                                         DNX_FIELD_TCAM_ENTRY_PRIORITY_INVALID));
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.location_priority_arr.set(unit, core, location_to, prio));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_location_move_line(
    int unit,
    int core,
    uint32 handler_id,
    uint32 line_from,
    uint32 line_to)
{
    int i;

    SHR_FUNC_INIT_VARS(unit);

    for (i = 0; i < 2; i++)
    {
        uint32 entry_location = line_from + i;
        uint32 entry_destination = line_to + i;
        int occupied;

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_is_location_occupied(unit, core, entry_location, &occupied));

        if (occupied)
        {
            uint32 priority;

            SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                            location_priority_arr.get(unit, core, entry_location, &priority));

            SHR_IF_ERR_EXIT(dnx_field_tcam_location_hash_move_one(unit, core, entry_location, entry_destination));

            SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_mark_as_occupied(unit, core, handler_id, entry_destination));

            SHR_IF_ERR_EXIT(dnx_field_tcam_bank_entry_move_update
                            (unit, core, entry_location, entry_destination, handler_id));

            SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_move_one
                            (unit, core, handler_id, entry_location, entry_destination, priority));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_location_free_line_fetch_aux(
    int unit,
    int core,
    uint32 handler_id,
    uint32 line_to_copy,
    uint32 *free_line)
{

    uint32 entry_to_move_location = *free_line;
    uint8 found;
    SHR_FUNC_INIT_VARS(unit);

    LOG_DEBUG_EX(BSL_LOG_MODULE, "<<<<<<<<<<<<Fetching free line at 0x%08x, entry at 0x%08x\n%s%s", *free_line,
                 line_to_copy, EMPTY, EMPTY);

    while (TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_next_get_extended
                        (unit, core, handler_id, *free_line, line_to_copy < *free_line, &entry_to_move_location,
                         &found));

        if (!found)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Something went wrong while moving entries\r\n"
                         "entries location bitmap at location %d has no %s for handler %d\r\n",
                         *free_line, (line_to_copy < *free_line) ? "next" : "prev", handler_id);
        }
        LOG_DEBUG_EX(BSL_LOG_MODULE, "<<<<<<<<<<<<<<<<<<<<<<Next at 0x%08x\n%s%s%s", entry_to_move_location, EMPTY,
                     EMPTY, EMPTY);

        if (DNX_FIELD_TCAM_LOCATION_LINE_CONTAINS(entry_to_move_location, line_to_copy))
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE, "<<<<<<<<<<<<<<<<<<<<<<Done Moving\n%s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);
            break;
        }

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_move_line
                        (unit, core, handler_id, DNX_FIELD_TCAM_LOCATION_LINE_GET(entry_to_move_location), *free_line));

        LOG_DEBUG_EX(BSL_LOG_MODULE, "<<<<<<<<<<<<<<<<<<<<<<Moving free line from 0x%08x to 0x%08x\n%s%s", *free_line,
                     DNX_FIELD_TCAM_LOCATION_LINE_GET(entry_to_move_location), EMPTY, EMPTY);

        SHR_IF_ERR_EXIT(dnx_field_tcam_access_move_line
                        (unit, core, handler_id, DNX_FIELD_TCAM_LOCATION_LINE_GET(entry_to_move_location), *free_line));

        *free_line = DNX_FIELD_TCAM_LOCATION_LINE_GET(entry_to_move_location);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_free_line_fetch(
    int unit,
    int core,
    uint32 handler_id,
    uint32 line_to_copy,
    uint32 *free_line)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_free_line_get(unit, core, line_to_copy, 0, free_line));
    LOG_DEBUG_EX(BSL_LOG_MODULE, "<<<<<<<<<<<<<Found free line at 0x%08x\n%s%s%s", *free_line, EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_free_line_fetch_aux(unit, core, handler_id, line_to_copy, free_line));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_alloc(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_entry_t * entry,
    dnx_field_tcam_location_t * location,
    uint8 is_dt)
{
    uint32 absolute_location;
    uint32 key_size;
    SHR_FUNC_INIT_VARS(unit);

    absolute_location = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(location->bank_id, location->bank_offset);
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.key_size.get(unit, handler_id, &key_size));

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_hash_add(unit, core, handler_id, entry->id, location));

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_update_alloc(unit, core, handler_id, entry, location, is_dt));

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                    location_priority_arr.set(unit, core, absolute_location, entry->priority));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_free(
    int unit,
    int core,
    uint32 handler_id,
    uint32 entry_id,
    uint8 is_dt)
{
    dnx_field_tcam_location_t location;
    uint32 absolute_location;
    uint8 found = FALSE;
    uint32 priority;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.
                    entry_location_hash.find(unit, core, &entry_id, &absolute_location, &found));

    if (!found)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error freeing entry %d, entry not allocated", entry_id);
    }

    if (!is_dt)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.location_priority_arr.get(unit, core, absolute_location, &priority));
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_location_hash.delete(unit, core, &entry_id));
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Entry at location %d removed from hash%s%s%s\n", absolute_location, EMPTY, EMPTY, EMPTY);

    location.bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(absolute_location);
    location.bank_offset = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(absolute_location);
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_update_free(unit, core, handler_id, priority, &location, is_dt));

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                    location_priority_arr.set(unit, core, absolute_location, DNX_FIELD_TCAM_ENTRY_PRIORITY_INVALID));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_location_free_line_align(
    int unit,
    int core,
    uint32 handler_id,
    uint8 backward,
    uint32 *location_to_copy_from,
    uint32 *location_to_copy_to,
    uint32 *location_start,
    uint32 *free_line)
{
    uint32 entry_to_move_location;
    uint8 found;
    SHR_FUNC_INIT_VARS(unit);

    while (backward ? (*free_line < *location_to_copy_to) : (*free_line > *location_to_copy_to))
    {

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_next_get_extended
                        (unit, core, handler_id, *free_line, !backward, &entry_to_move_location, &found));

        if (!found)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Something went wrong while moving entries\r\n"
                         "entries location bitmap at location %d has no %s for handler %d\r\n",
                         *free_line, (backward) ? "next" : "prev", handler_id);
        }

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_move_line
                        (unit, core, handler_id, DNX_FIELD_TCAM_LOCATION_LINE_GET(entry_to_move_location), *free_line));

        SHR_IF_ERR_EXIT(dnx_field_tcam_access_move_line
                        (unit, core, handler_id, DNX_FIELD_TCAM_LOCATION_LINE_GET(entry_to_move_location), *free_line));

        if (DNX_FIELD_TCAM_LOCATION_LINE_CONTAINS(entry_to_move_location, *location_to_copy_from))
        {
            *location_to_copy_from = DNX_FIELD_TCAM_LOCATION_POLARITY(*free_line, *location_to_copy_from);
        }

        if (DNX_FIELD_TCAM_LOCATION_LINE_CONTAINS(entry_to_move_location, *location_start))
        {
            *location_start = DNX_FIELD_TCAM_LOCATION_POLARITY(*free_line, *location_start);
        }

        if (DNX_FIELD_TCAM_LOCATION_LINE_CONTAINS(entry_to_move_location, *location_to_copy_to))
        {
            *location_to_copy_to = DNX_FIELD_TCAM_LOCATION_POLARITY(*free_line, *location_to_copy_to);
        }

        *free_line = DNX_FIELD_TCAM_LOCATION_LINE_GET(entry_to_move_location);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_location_do_move(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location_to_copy_to,
    uint8 backward,
    uint32 *location_to_copy_from,
    struct dnx_field_tcam_location_manager_80_state *move_state,
    uint32 *location_start)
{
    uint32 key_size;
    uint8 copy_next = FALSE;
    uint32 priority;
    uint8 neighbor_occupied;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.key_size.get(unit, handler_id, &key_size));

    LOG_DEBUG_EX(BSL_LOG_MODULE, ">>>>>>>>>>>>>>>>>Moving 0x%08x to 0x%08x op_state: %d, backward: %d\n",
                 *location_to_copy_from, location_to_copy_to, move_state->op_state, backward);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                    entries_occupation_bitmap.is_occupied(unit, handler_id, core,
                                                          DNX_FIELD_TCAM_LOCATION_COMPLEMENTARY_GET
                                                          (location_to_copy_to), &neighbor_occupied));

    if (key_size == dnx_data_field.tcam.key_size_half_get(unit)
        && !dnx_data_field.tcam.feature_get(unit, dnx_data_field_tcam_tcam_80b_traffic_safe_write) && neighbor_occupied)
    {
        uint8 found;
        uint32 entry_to_move_location;

        if (DNX_FIELD_TCAM_LOCATION_LINE_CONTAINS(*location_to_copy_from, location_to_copy_to))
        {
            copy_next = TRUE;
        }

        if (move_state->op_state == DNX_FIELD_TCAM_LOCATION_MANAGER_80_OP_STATE_NEW)
        {
            SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_free_line_get
                            (unit, core, location_to_copy_to, backward, &(move_state->free_line)));
            LOG_DEBUG_EX(BSL_LOG_MODULE, ">>>>>>>>>>>>>>>>>Found free line at 0x%08x\n%s%s%s", move_state->free_line,
                         EMPTY, EMPTY, EMPTY);

            SHR_IF_ERR_EXIT(dnx_field_tcam_location_free_line_align
                            (unit, core, handler_id, backward, location_to_copy_from, &location_to_copy_to,
                             location_start, &(move_state->free_line)));
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         ">>>>>>>>>>>>>>>>>After align///// free line at 0x%08x, location_to: 0x%08x, location_from: 0x%08x, location_start: 0x%08x\n",
                         move_state->free_line, location_to_copy_to, *location_to_copy_from, *location_start);
        }

        while (TRUE)
        {
            if (move_state->op_state == DNX_FIELD_TCAM_LOCATION_MANAGER_80_OP_STATE_COPY)
            {

                break;
            }

            SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_next_get_extended
                            (unit, core, handler_id, move_state->free_line, backward, &entry_to_move_location, &found));

            if (!found)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Something went wrong while moving entries\r\n"
                             "entries location bitmap at location %d has no %s for handler %d\r\n",
                             move_state->free_line, (backward) ? "prev" : "next", handler_id);
            }
            LOG_DEBUG_EX(BSL_LOG_MODULE, ">>>>>>>>>>>>>>>>>Found next entry at 0x%08x\n%s%s%s", entry_to_move_location,
                         EMPTY, EMPTY, EMPTY);

            if (backward ? (DNX_FIELD_TCAM_LOCATION_LINE_GET(entry_to_move_location) <= location_to_copy_to)
                : (entry_to_move_location >= DNX_FIELD_TCAM_LOCATION_LINE_GET(location_to_copy_to)))
            {
                uint32 prev_line = DNX_FIELD_TCAM_LOCATION_LINE_GET(location_to_copy_to);
                location_to_copy_to = DNX_FIELD_TCAM_LOCATION_POLARITY(move_state->free_line, location_to_copy_to);
                LOG_DEBUG_EX(BSL_LOG_MODULE, ">>>>>>>>>>>>>>>>Free line has reached target\n%s%s%s%s", EMPTY, EMPTY,
                             EMPTY, EMPTY);

                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             ">>>>>>>>>>>>>>>>>New location_to_copy_to after reaching free line: 0x%08x\n%s%s%s",
                             location_to_copy_to, EMPTY, EMPTY, EMPTY);

                if (!copy_next)
                {
                    SHR_IF_ERR_EXIT(dnx_field_tcam_location_move_line
                                    (unit, core, handler_id, prev_line, move_state->free_line));
                    SHR_IF_ERR_EXIT(dnx_field_tcam_access_move_line
                                    (unit, core, handler_id, prev_line, move_state->free_line));
                    LOG_DEBUG_EX(BSL_LOG_MODULE,
                                 ">>>>>>>>>>>>>>>>>Moved free_line to: 0x%08x, after being in 0x%08x\n%s%s", prev_line,
                                 move_state->free_line, EMPTY, EMPTY);
                    move_state->free_line = prev_line;
                }
                break;
            }

            SHR_IF_ERR_EXIT(dnx_field_tcam_location_move_line
                            (unit, core, handler_id, DNX_FIELD_TCAM_LOCATION_LINE_GET(entry_to_move_location),
                             move_state->free_line));

            SHR_IF_ERR_EXIT(dnx_field_tcam_access_move_line
                            (unit, core, handler_id, DNX_FIELD_TCAM_LOCATION_LINE_GET(entry_to_move_location),
                             move_state->free_line));

            LOG_DEBUG_EX(BSL_LOG_MODULE, ">>>>>>>>>>>>>>>>>Moving free_line from: 0x%08x, to: 0x%08x\n%s%s",
                         move_state->free_line, DNX_FIELD_TCAM_LOCATION_LINE_GET(entry_to_move_location), EMPTY, EMPTY);
            move_state->free_line = DNX_FIELD_TCAM_LOCATION_LINE_GET(entry_to_move_location);
        }
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_hash_move_one(unit, core, *location_to_copy_from, location_to_copy_to));

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_move(unit, core, handler_id, *location_to_copy_from, location_to_copy_to));
    LOG_DEBUG_EX(BSL_LOG_MODULE, ">>>>>>>>>>>>>>>>>Moved one entry from 0x%08x to 0x%08x\n%s%s", *location_to_copy_from,
                 location_to_copy_to, EMPTY, EMPTY);

    if (key_size == dnx_data_field.tcam.key_size_half_get(unit)
        && !dnx_data_field.tcam.feature_get(unit, dnx_data_field_tcam_tcam_80b_traffic_safe_write))
    {

        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_entry_move_update
                        (unit, core, *location_to_copy_from, location_to_copy_to, handler_id));

        if (!copy_next && neighbor_occupied)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE, ">>>>>>>>>>>>>>>>Deleting free line contents at: 0x%08x\n%s%s%s",
                         move_state->free_line, EMPTY, EMPTY, EMPTY);

            SHR_IF_ERR_EXIT(dnx_field_tcam_access_line_delete(unit, core, handler_id, move_state->free_line));
        }

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                        location_priority_arr.get(unit, core, location_to_copy_to, &priority));

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_move_one
                        (unit, core, handler_id, *location_to_copy_from, location_to_copy_to, priority));

        if (copy_next)
        {
            uint32 prev_free_line = move_state->free_line;
            move_state->free_line = DNX_FIELD_TCAM_LOCATION_LINE_GET(*location_to_copy_from);

            if (*location_to_copy_from == *location_start)
            {
                *location_start = DNX_FIELD_TCAM_LOCATION_POLARITY(prev_free_line, *location_start);
            }

            *location_to_copy_from = DNX_FIELD_TCAM_LOCATION_POLARITY(prev_free_line, *location_to_copy_from);

            move_state->op_state = DNX_FIELD_TCAM_LOCATION_MANAGER_80_OP_STATE_COPY;
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         ">>>>>>>>>>>>>>>>Next iteration: op: COPY, free_line: 0x%08x location_from: 0x%08x\n%s%s",
                         move_state->free_line, *location_to_copy_from, EMPTY, EMPTY);
        }
        else if (DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(move_state->free_line) !=
                 DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(*location_to_copy_from))
        {
            move_state->op_state = DNX_FIELD_TCAM_LOCATION_MANAGER_80_OP_STATE_NEW;
            LOG_DEBUG_EX(BSL_LOG_MODULE, ">>>>>>>>>>>>>>>>Next iteration: op: NEW\n%s%s%s%s", EMPTY, EMPTY, EMPTY,
                         EMPTY);
        }
        else
        {
            move_state->op_state = DNX_FIELD_TCAM_LOCATION_MANAGER_80_OP_STATE_KEEP;
            LOG_DEBUG_EX(BSL_LOG_MODULE, ">>>>>>>>>>>>>>>>Next iteration: op: KEEP\n%s%s%s%s", EMPTY, EMPTY, EMPTY,
                         EMPTY);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_move_extended(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location_start,
    uint32 location_end)
{
    uint32 location_to_copy_to;
    uint8 found;
    uint8 backward;
    dnx_field_tcam_location_find_mode_e find_mode;
    dnx_field_tcam_location_t bank_location;

    uint32 location_to_copy_from = location_end;
    uint32 priority;

    SHR_FUNC_INIT_VARS(unit);

    backward = (location_start < location_end);
    find_mode = (backward) ? DNX_FIELD_TCAM_LOCATION_FIND_MODE_BACKWARD : DNX_FIELD_TCAM_LOCATION_FIND_MODE_FORWARD;

    location_to_copy_to = location_end;
    while (location_to_copy_from != location_start)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_next_get_extended
                        (unit, core, handler_id, location_to_copy_to, backward, &location_to_copy_from, &found));

        if (!found)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Something went wrong while moving entries\r\n"
                         "entries location bitmap at location %d has no %s for handler %d\r\n",
                         location_to_copy_to, (backward) ? "prev" : "next", handler_id);
        }
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                        location_priority_arr.get(unit, core, location_to_copy_from, &priority));

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_hash_move_one(unit, core, location_to_copy_from, location_to_copy_to));

        SHR_IF_ERR_EXIT(dnx_field_tcam_access_move(unit, core, handler_id, location_to_copy_from, location_to_copy_to));

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_mark_as_occupied(unit, core, handler_id, location_to_copy_to));
        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_entry_move_update
                        (unit, core, location_to_copy_from, location_to_copy_to, handler_id));

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_move_one
                        (unit, core, handler_id, location_to_copy_from, location_to_copy_to, priority));

        bank_location.bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(location_to_copy_to);
        bank_location.bank_offset = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(location_to_copy_to);

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_empty_location_find
                        (unit, core, handler_id, &bank_location, find_mode, &location_to_copy_to, &found));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_location_sequential_move(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location_end,
    uint32 *location_start)
{
    uint32 location_to_copy_to;
    uint8 found;
    uint8 backward;
    struct dnx_field_tcam_location_manager_80_state move_state;

    uint32 location_to_copy_from = location_end;
    uint32 key_size;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.key_size.get(unit, handler_id, &key_size));

    location_to_copy_to = location_end;

    backward = (*location_start < location_end);

    move_state.op_state = DNX_FIELD_TCAM_LOCATION_MANAGER_80_OP_STATE_NEW;
    move_state.free_line = DNX_FIELD_TCAM_LOCATION_INVALID;

    while (location_to_copy_from != *location_start)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_next_get_extended
                        (unit, core, handler_id, location_to_copy_to, backward, &location_to_copy_from, &found));

        if (!found)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Something went wrong while moving entries\r\n"
                         "entries location bitmap at location %d has no %s for handler %d\r\n",
                         location_to_copy_to, (backward) ? "prev" : "next", handler_id);
        }

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_do_move
                        (unit, core, handler_id, location_to_copy_to, backward, &location_to_copy_from, &move_state,
                         location_start));

        location_to_copy_to = location_to_copy_from;
    }

    if (key_size == dnx_data_field.tcam.key_size_half_get(unit)
        && !dnx_data_field.tcam.feature_get(unit, dnx_data_field_tcam_tcam_80b_traffic_safe_write)
        && move_state.free_line != DNX_FIELD_TCAM_LOCATION_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_line_delete(unit, core, handler_id, move_state.free_line));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_move(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location_end,
    uint32 *location_start)
{
    uint32 key_size;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sequential_move(unit, core, handler_id, location_end, location_start));

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.key_size.get(unit, handler_id, &key_size));

    if (key_size != dnx_data_field.tcam.key_size_half_get(unit)
        || dnx_data_field.tcam.feature_get(unit, dnx_data_field_tcam_tcam_80b_traffic_safe_write))
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_mark_as_occupied(unit, core, handler_id, location_end));

        SHR_IF_ERR_EXIT(dnx_field_tcam_bank_entry_move_update(unit, core, *location_start, location_end, handler_id));

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_move(unit, core, handler_id, *location_start, location_end));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_location_entry_id_to_abs_location(
    int unit,
    int core,
    uint32 entry_id,
    uint32 *location)
{
    uint8 found;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_location_hash.find(unit, core, &entry_id, location, &found));

    if (!found)
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_location_location_to_entry_id(
    int unit,
    int core,
    uint32 location,
    uint32 *entry_id)
{
    uint8 found = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_location_hash.get_by_index(unit, core, location, entry_id, &found));

    if (!found)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Entry at location: %d not found in location->entry Hash", location);
    }

exit:
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_field_tcam_location_handler_first_get(
    int unit,
    int core,
    uint32 handler_id,
    uint32 *first_entry_id)
{
    uint32 first_location;
    uint8 found;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_next_get(unit, core, handler_id, -1, &first_location, &found));

    if (!found)
    {
        *first_entry_id = DNX_FIELD_ENTRY_ACCESS_ID_INVALID;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_location_to_entry_id(unit, core, first_location, first_entry_id));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_handler_next_get(
    int unit,
    int core,
    uint32 handler_id,
    uint32 entry_id,
    uint32 *next_entry_id)
{
    uint32 location;
    uint32 next_location;
    uint8 found;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_entry_id_to_abs_location(unit, core, entry_id, &location));

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_next_get
                    (unit, core, handler_id, location, &next_location, &found));

    if (!found)
    {
        *next_entry_id = DNX_FIELD_ENTRY_ACCESS_ID_INVALID;
    }
    else
    {

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_location_to_entry_id(unit, core, next_location, next_entry_id));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_entry_id_to_location(
    int unit,
    int core,
    uint32 entry_id,
    dnx_field_tcam_location_t * location)
{
    uint32 abs_location;
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = dnx_field_tcam_location_entry_id_to_abs_location(unit, core, entry_id, &abs_location);
    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_IF_ERR_EXIT_NO_MSG(rv);
    }
    SHR_IF_ERR_EXIT(rv);
    location->bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(abs_location);
    location->bank_offset = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(abs_location);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_bank_entries_count(
    int unit,
    int core,
    uint32 handler_id,
    uint32 bank_id,
    uint32 *entries_count_out)
{
    uint32 bank_min_address;
    uint32 bank_max_address;
    uint32 address_iter;
    uint32 entries_count;
    uint8 found;

    SHR_FUNC_INIT_VARS(unit);

    entries_count = 0;
    bank_min_address = DNX_FIELD_TCAM_LOCATION_BANK_FIRST(bank_id);
    bank_max_address = DNX_FIELD_TCAM_LOCATION_BANK_LAST(bank_id);

    address_iter = bank_min_address - 1;
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_next_get(unit, core,
                                                                  handler_id, address_iter, &address_iter, &found));

    while (found && address_iter <= bank_max_address)
    {

        entries_count++;
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_next_get(unit, core,
                                                                      handler_id, address_iter, &address_iter, &found));
    }

    *entries_count_out = entries_count;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_entry_priority_get(
    int unit,
    uint32 handler_id,
    uint32 entry_id,
    uint32 *priority)
{
    uint32 entry_location;
    bcm_core_t core_iter;
    uint8 entry_found = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_iter)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_entry_id_exists(unit, core_iter, entry_id, &entry_found));
        if (!entry_found)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_entry_id_to_abs_location(unit, core_iter, entry_id, &entry_location));
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                        location_priority_arr.get(unit, core_iter, entry_location, priority));
        break;
    }
    if (!entry_found)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Entry with entry_id: (0x%x) not found in TCAM\n", entry_id);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_entry_id_exists(
    int unit,
    int core,
    uint32 entry_id,
    uint8 *found_p)
{
    uint32 abs_location;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_location_hash.find(unit, core, &entry_id, &abs_location, found_p));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_allocating_handler(
    int unit,
    int core,
    dnx_field_tcam_location_t * location,
    uint32 *handler_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_allocating_handler(unit, core, location, handler_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_location_vacant_80b_entries_get(
    int unit,
    int core,
    uint32 handler_id,
    uint32 bank_id,
    uint8 *is_vacant_80b)
{
    uint32 bank_min_address;
    uint32 bank_max_address;
    uint32 address_iter;

    SHR_FUNC_INIT_VARS(unit);

    bank_min_address = DNX_FIELD_TCAM_LOCATION_BANK_FIRST(bank_id);
    bank_max_address = DNX_FIELD_TCAM_LOCATION_BANK_LAST(bank_id);

    *is_vacant_80b = FALSE;
    address_iter = bank_min_address - 1;
    while (TRUE)
    {
        uint8 found;
        uint8 is_occupied;

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.
                        tcam_banks_occupation_bitmap.get_next_in_range(unit, core, address_iter + 1, bank_max_address,
                                                                       TRUE, 0, &address_iter, &found));

        if (!found)
        {
            break;
        }

        SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw.tcam_handlers_info.
                        entries_occupation_bitmap.is_occupied(unit, handler_id, core,
                                                              DNX_FIELD_TCAM_LOCATION_COMPLEMENTARY_GET(address_iter),
                                                              &is_occupied));

        if (is_occupied)
        {
            *is_vacant_80b = TRUE;
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}
