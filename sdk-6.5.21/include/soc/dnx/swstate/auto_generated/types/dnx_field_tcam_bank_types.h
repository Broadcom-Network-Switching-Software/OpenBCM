
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_BANK_TYPES_H__
#define __DNX_FIELD_TCAM_BANK_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/callbacks/sw_state_ll_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_linked_list.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_access_types.h>


#define FIELD_TCAM_BANK_IS_BIG_BANK(bank_id) ((bank_id)<dnx_data_field.tcam.nof_big_banks_get(unit))

#define FIELD_TCAM_BANK_NOF_ENTRIES(bank_id) ((FIELD_TCAM_BANK_IS_BIG_BANK(bank_id))?dnx_data_field.tcam.nof_big_bank_lines_get(unit):dnx_data_field.tcam.nof_small_bank_lines_get(unit))

#define FIELD_TCAM_BANK_IS_VALID(bank_id) ((bank_id)<dnx_data_field.tcam.nof_banks_get(unit))




typedef enum {
    
    DNX_FIELD_TCAM_BANK_MODE_INVALID = -1,
    
    DNX_FIELD_TCAM_BANK_MODE_FIRST = 0,
    
    DNX_FIELD_TCAM_BANK_MODE_NONE = DNX_FIELD_TCAM_BANK_MODE_FIRST,
    
    DNX_FIELD_TCAM_BANK_MODE_OPEN,
    
    DNX_FIELD_TCAM_BANK_MODE_RESTRICTED,
    
    DNX_FIELD_TCAM_BANK_MODE_RESTRICTED_DT,
    
    DNX_FIELD_TCAM_BANK_MODE_RESTRICTED_LOCATION,
    
    DNX_FIELD_TCAM_BANK_MODE_NOF
} dnx_field_tcam_bank_mode_e;




typedef struct {
    
    dnx_field_tcam_stage_e owner_stage;
    
    sw_state_ll_t active_handlers_id;
    
    uint32** nof_free_entries;
    
    dnx_field_tcam_bank_mode_e bank_mode;
} dnx_field_tcam_bank_t;


typedef struct {
    
    dnx_field_tcam_bank_t* tcam_banks;
} dnx_field_tcam_bank_sw_t;


#endif 
