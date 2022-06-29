
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef TCAM_DATABASE_H_INCLUDED

#define TCAM_DATABASE_H_INCLUDED

#include <include/bcm_int/dnx/field/field.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_access_types.h>

typedef struct
{

    uint32 id;

    uint32 priority;

    uint8 valid_bit;
} dnx_field_tcam_entry_t;

typedef struct
{

    dnx_field_tcam_stage_e stage;

    uint8 is_direct;

    dnx_field_key_length_type_e key_size;

    dnx_field_action_length_type_e action_size;

    uint32 actual_action_size;

    uint32 prefix_size;

    uint32 prefix_value;

    dnx_field_tcam_bank_allocation_mode_e bank_allocation_mode;

    int nof_banks_to_alloc;

    int banks_to_alloc[DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS];

    int core_ids[DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS];
} dnx_field_tcam_database_t;

shr_error_e dnx_field_tcam_database_t_init(
    int unit,
    dnx_field_tcam_database_t * db);

#endif
