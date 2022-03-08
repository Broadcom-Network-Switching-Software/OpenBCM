
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef TCAM_LOCATION_ALGO_H_INCLUDED

#define TCAM_LOCATION_ALGO_H_INCLUDED

#include <soc/dnx/field/tcam/tcam_entities.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_location_types.h>

typedef enum
{
    DNX_FIELD_TCAM_LOCATION_FIND_MODE_FORWARD = 1,
    DNX_FIELD_TCAM_LOCATION_FIND_MODE_BACKWARD = 2,
    DNX_FIELD_TCAM_LOCATION_FIND_MODE_CLOSEST = 3,
} dnx_field_tcam_location_find_mode_e;

shr_error_e dnx_field_tcam_location_priorities_range_init(
    int unit);

shr_error_e dnx_field_tcam_location_algo_update_free(
    int unit,
    int core,
    uint32 handler_id,
    uint32 priority,
    dnx_field_tcam_location_t * location,
    uint8 is_dt);

shr_error_e dnx_field_tcam_location_algo_update_alloc(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_entry_t * entry,
    dnx_field_tcam_location_t * location,
    uint8 is_dt);

shr_error_e dnx_field_tcam_location_algo_find(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_entry_t * entry,
    dnx_field_tcam_location_t * location,
    uint8 *found);

shr_error_e dnx_field_tcam_location_algo_mark_as_occupied(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location);

shr_error_e dnx_field_tcam_location_algo_move(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location_from,
    uint32 location_to);

shr_error_e dnx_field_tcam_location_algo_move_one(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location_from,
    uint32 location_to,
    uint32 priority);

shr_error_e dnx_field_tcam_location_algo_handler_next_get(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location,
    uint32 *next_location,
    uint8 *found);

shr_error_e dnx_field_tcam_location_algo_handler_is_bank_handler_free(
    int unit,
    int core,
    uint32 handler_id,
    uint32 bank_id,
    uint8 *found);

shr_error_e dnx_field_tcam_location_algo_handler_prev_get(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location,
    uint32 *prev_location,
    uint8 *found);

shr_error_e dnx_field_tcam_location_algo_handler_next_get_extended(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location,
    uint8 backward,
    uint32 *next_location,
    uint8 *found);

shr_error_e dnx_field_tcam_location_algo_entry_comp_state(
    int unit,
    int core,
    uint32 location,
    uint8 *single_occupied,
    uint8 *double_occupied);

shr_error_e dnx_field_tcam_location_algo_empty_location_find(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_location_t * location,
    dnx_field_tcam_location_find_mode_e find_mode,
    uint32 *empty_place,
    uint8 *found);

shr_error_e dnx_field_tcam_location_algo_allocating_handler(
    int unit,
    int core,
    dnx_field_tcam_location_t * location,
    uint32 *handler_id);

shr_error_e dnx_field_tcam_location_algo_free_line_get(
    int unit,
    int core,
    uint32 location,
    uint8 direction,
    uint32 *free_line);

#endif
