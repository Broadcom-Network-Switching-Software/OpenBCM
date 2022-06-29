
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef TCAM_LOCATION_MANAGER_H_INCLUDED

#define TCAM_LOCATION_MANAGER_H_INCLUDED

#include <soc/dnx/field/tcam/tcam_location_algo.h>

shr_error_e dnx_field_tcam_location_find(
    int unit,
    bcm_core_t core,
    uint32 handler_id,
    dnx_field_tcam_entry_t * entry,
    dnx_field_tcam_location_t * location,
    uint8 *found);

shr_error_e dnx_field_tcam_location_is_location_occupied(
    int unit,
    int core,
    uint32 absolute_location,
    int *occupied);

shr_error_e dnx_field_tcam_location_alloc(
    int unit,
    int core,
    uint32 handler_id,
    dnx_field_tcam_entry_t * entry,
    dnx_field_tcam_location_t * location,
    uint8 is_dt);

shr_error_e dnx_field_tcam_location_free(
    int unit,
    int core,
    uint32 handler_id,
    uint32 entry_id,
    uint8 is_dt);

shr_error_e dnx_field_tcam_location_move(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location_end,
    uint32 *location_start);

shr_error_e dnx_field_tcam_location_move_extended(
    int unit,
    int core,
    uint32 handler_id,
    uint32 location_start,
    uint32 location_end);

shr_error_e dnx_field_tcam_location_handler_first_get(
    int unit,
    int core,
    uint32 handler_id,
    uint32 *first_entry_id);

shr_error_e dnx_field_tcam_location_handler_next_get(
    int unit,
    int core,
    uint32 handler_id,
    uint32 entry_id,
    uint32 *next_entry_id);

shr_error_e dnx_field_tcam_location_entry_id_to_location(
    int unit,
    int core,
    uint32 entry_id,
    dnx_field_tcam_location_t * location);

shr_error_e dnx_field_tcam_location_bank_entries_count(
    int unit,
    int core,
    uint32 handler_id,
    uint32 bank_id,
    uint32 *entries_count);

shr_error_e dnx_field_tcam_location_entry_priority_get(
    int unit,
    uint32 handler_id,
    uint32 entry_id,
    uint32 *priority);

shr_error_e dnx_field_tcam_location_entry_id_exists(
    int unit,
    int core,
    uint32 entry_id,
    uint8 *found_p);

shr_error_e dnx_field_tcam_location_allocating_handler(
    int unit,
    int core,
    dnx_field_tcam_location_t * location,
    uint32 *handler_id);

shr_error_e dnx_field_tcam_location_vacant_80b_entries_get(
    int unit,
    int core,
    uint32 handler_id,
    uint32 bank_id,
    uint8 *is_vacant_80b);

shr_error_e dnx_field_tcam_location_free_line_fetch(
    int unit,
    int core,
    uint32 handler_id,
    uint32 line_to_copy,
    uint32 *free_line);

#endif
