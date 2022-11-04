
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef TCAM_BANK_MANAGER_H_INCLUDED

#define TCAM_BANK_MANAGER_H_INCLUDED

#include <include/bcm_int/dnx/field/field.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_field.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_access_types.h>

shr_error_e dnx_field_tcam_bank_alloc(
    int unit,
    uint32 handler_id,
    int nof_banks_to_alloc,
    int banks_to_alloc[],
    bcm_core_t core_ids[]);

shr_error_e dnx_field_tcam_bank_single_alloc_any(
    int unit,
    uint32 handler_id,
    bcm_core_t core_id);

shr_error_e dnx_field_tcam_bank_handler_free(
    int unit,
    uint32 handler_id);

shr_error_e dnx_field_tcam_bank_entry_alloc_update(
    int unit,
    int core,
    uint32 location,
    uint32 handler_id);

shr_error_e dnx_field_tcam_bank_entry_free_update(
    int unit,
    int core,
    uint32 location,
    uint32 handler_id);

shr_error_e dnx_field_tcam_bank_entry_move_update(
    int unit,
    int core,
    uint32 location_from,
    uint32 location_to,
    uint32 handler_id);

shr_error_e dnx_field_tcam_bank_available_entries_get(
    int unit,
    int core,
    int bank_id,
    dnx_field_tcam_entry_size_e entry_size,
    uint32 *nof_free_entries);

shr_error_e dnx_field_tcam_bank_is_full(
    int unit,
    int core,
    int bank_id,
    uint32 handler_id,
    uint8 *full);

shr_error_e dnx_field_tcam_bank_is_handler_on_bank(
    int unit,
    int bank_id,
    uint32 handler_id,
    bcm_core_t core_id,
    uint8 *found);

shr_error_e dnx_field_tcam_bank_remove_handler_from_bank(
    int unit,
    int bank_id,
    uint32 handler_id,
    bcm_core_t core_id,
    int *found);

shr_error_e dnx_field_tcam_bank_add_handler_to_bank(
    int unit,
    int bank_id,
    uint32 handler_id,
    bcm_core_t core_id);

#endif
