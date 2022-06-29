
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef TCAM_HANDLER_H_INCLUDED

#define TCAM_HANDLER_H_INCLUDED

#include <soc/dnx/field/tcam/tcam_entities.h>
#include <soc/dnx/field/tcam/tcam_bank_manager.h>
#include <soc/dnx/field/tcam/tcam_access_profile_manager.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_manager_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_location_types.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>

#define DNX_FIELD_TCAM_HANDLER_ID_INVALID 0x1023

shr_error_e dnx_field_tcam_handler_create(
    int unit,
    dnx_field_tcam_database_t * db,
    int access_profile_id,
    dnx_field_tcam_handler_mode_e mode,
    dnx_field_tcam_context_sharing_handlers_get_p context_sharing_handlers_cb,
    uint32 *handler_id);

shr_error_e dnx_field_tcam_handler_destroy(
    int unit,
    uint32 handler_id);

shr_error_e dnx_field_tcam_handler_access_profile_id_get(
    int unit,
    uint32 handler_id,
    int *access_profile_id);

shr_error_e dnx_field_tcam_handler_app_db_id_get(
    int unit,
    uint32 handler_id,
    dnx_field_app_db_id_t * app_db_id);

shr_error_e dnx_field_tcam_handler_is_direct(
    int unit,
    uint32 handler_id,
    uint8 *is_direct_table_p);

shr_error_e dnx_field_tcam_handler_entry_dt_location_alloc(
    int unit,
    bcm_core_t core,
    uint32 handler_id,
    dnx_field_tcam_entry_t * entry);

shr_error_e dnx_field_tcam_handler_entry_location_alloc(
    int unit,
    bcm_core_t core,
    uint32 handler_id,
    dnx_field_tcam_entry_t * entry);

shr_error_e dnx_field_tcam_handler_entry_valid_set(
    int unit,
    bcm_core_t core,
    uint32 entry_id,
    uint8 valid_bit);

shr_error_e dnx_field_tcam_handler_entry_valid_get(
    int unit,
    bcm_core_t core,
    uint32 entry_id,
    uint8 *valid_bit);

shr_error_e dnx_field_tcam_handler_entry_location_free(
    int unit,
    uint32 handler_id,
    uint32 entry_id);

shr_error_e dnx_field_tcam_handler_state_get(
    int unit,
    uint32 handler_id,
    dnx_field_tcam_handler_state_e * state);

shr_error_e dnx_field_tcam_handler_program_attach(
    int unit,
    uint32 handler_id,
    uint32 *same_program_handler_ids,
    int handlers_count);

shr_error_e dnx_field_tcam_handler_entry_first(
    int unit,
    int core,
    uint32 handler_id,
    uint32 *first_entry_id);

shr_error_e dnx_field_tcam_handler_entry_next(
    int unit,
    int core,
    uint32 handler_id,
    uint32 entry_id,
    uint32 *next_entry_id);

shr_error_e dnx_field_tcam_handler_entry_location(
    int unit,
    bcm_core_t core,
    uint32 entry_id,
    dnx_field_tcam_location_t * location);

shr_error_e dnx_field_tcam_handler_entries_info(
    int unit,
    int core,
    uint32 handler_id,
    uint32 bank_entries_count[DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS],
    uint32 *banks_bmp);

shr_error_e dnx_field_tcam_handler_entry_priority_get(
    int unit,
    uint32 handler_id,
    uint32 entry_id,
    uint32 *priority);

shr_error_e dnx_field_tcam_handler_bank_allocation_mode_get(
    int unit,
    uint32 handler_id,
    dnx_field_tcam_bank_allocation_mode_e * bank_allocation_mode);

shr_error_e dnx_field_tcam_handler_bank_evacuate(
    int unit,
    uint32 handler_id,
    int nof_banks,
    int bank_ids[],
    bcm_core_t core_ids[]);

shr_error_e dnx_field_tcam_handler_bank_add(
    int unit,
    uint32 handler_id,
    int nof_banks,
    int bank_ids[],
    bcm_core_t core_ids[]);

shr_error_e dnx_field_tcam_handler_entry_id_exists(
    int unit,
    int core,
    uint32 entry_id,
    uint8 *found_p);

shr_error_e dnx_field_tcam_handler_entry_handler(
    int unit,
    uint32 entry_id,
    uint32 *handler_id);

shr_error_e dnx_field_tcam_handler_entry_core_get(
    int unit,
    uint32 entry_id,
    int *core);

#endif
