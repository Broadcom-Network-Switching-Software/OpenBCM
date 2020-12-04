
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_ACCESS_TYPES_H__
#define __DNX_FIELD_TCAM_ACCESS_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/callbacks/sw_state_htb_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_manager_types.h>



typedef enum {
    
    DNX_FIELD_TCAM_STAGE_INVALID = -1,
    DNX_FIELD_TCAM_STAGE_FIRST = 0,
    
    DNX_FIELD_TCAM_STAGE_IPMF1 = DNX_FIELD_STAGE_FIRST,
    
    DNX_FIELD_TCAM_STAGE_IPMF2,
    
    DNX_FIELD_TCAM_STAGE_IPMF3,
    
    DNX_FIELD_TCAM_STAGE_EPMF,
    
    DNX_FIELD_TCAM_STAGE_VTT1,
    
    DNX_FIELD_TCAM_STAGE_VTT2,
    
    DNX_FIELD_TCAM_STAGE_VTT3,
    
    DNX_FIELD_TCAM_STAGE_VTT4,
    
    DNX_FIELD_TCAM_STAGE_VTT5,
    
    DNX_FIELD_TCAM_STAGE_FWD1,
    
    DNX_FIELD_TCAM_STAGE_FWD2,
    
    DNX_FIELD_TCAM_STAGE_NOF
} dnx_field_tcam_stage_e;


typedef enum {
    DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_INVALID = -1,
    DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_FIRST = 0,
    
    DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_AUTO = DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_FIRST,
    
    DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT,
    
    DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT_WITH_LOCATION,
    
    DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_NOF
} dnx_field_tcam_bank_allocation_mode_e;


typedef enum {
    
    DNX_FIELD_TCAM_ENTRY_SIZE_INVALID = -1,
    
    DNX_FIELD_TCAM_ENTRY_SIZE_FIRST = 0,
    
    DNX_FIELD_TCAM_ENTRY_SIZE_HALF = DNX_FIELD_TCAM_ENTRY_SIZE_FIRST,
    
    DNX_FIELD_TCAM_ENTRY_SIZE_SINGLE = 1,
    
    DNX_FIELD_TCAM_ENTRY_SIZE_DOUBLE = 2,
    
    DNX_FIELD_TCAM_ENTRY_SIZE_COUNT = 3
} dnx_field_tcam_entry_size_e;


typedef enum {
    
    DNX_FIELD_TCAM_CORE_INVALID = -1,
    
    DNX_FIELD_TCAM_CORE_FIRST = 0,
    
    DNX_FIELD_TCAM_CORE_0 = DNX_FIELD_TCAM_CORE_FIRST,
    
    DNX_FIELD_TCAM_CORE_1 = 1,
    
    DNX_FIELD_TCAM_CORE_ALL = 2,
    
    DNX_FIELD_TCAM_CORE_COUNT = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES
} dnx_field_tcam_core_e;




typedef struct {
    
    uint32 key_size;
    
    uint32 action_size;
    
    uint32 actual_action_size;
    
    dnx_field_tcam_stage_e stage;
    
    uint32 prefix_size;
    
    uint32 prefix_value;
    
    uint8 direct_table;
    
    int dt_bank_id;
    
    dnx_field_tcam_bank_allocation_mode_e bank_allocation_mode;
    
    dnx_field_tcam_context_sharing_handlers_get_p context_sharing_handlers_cb;
} dnx_field_tcam_access_fg_params_t;


typedef struct {
    SHR_BITDCL* v_bit;
} dnx_field_tcam_access_v_bit_t;


typedef struct {
    
    sw_state_htbl_t* entry_location_hash;
    
    dnx_field_tcam_access_v_bit_t* valid_bmp;
    
    dnx_field_tcam_access_v_bit_t* entry_in_use_bmp;
    
    dnx_field_tcam_access_fg_params_t* fg_params;
} dnx_field_tcam_access_t;


#endif 
