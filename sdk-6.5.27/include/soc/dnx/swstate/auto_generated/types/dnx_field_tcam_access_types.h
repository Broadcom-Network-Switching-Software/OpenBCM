
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_ACCESS_TYPES_H__
#define __DNX_FIELD_TCAM_ACCESS_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/callbacks/sw_state_htb_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
#include <soc/dnxc/swstate/types/sw_state_index_htb.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_manager_types.h>


typedef enum {
    DNX_FIELD_TCAM_STAGE_INVALID = -1,
    DNX_FIELD_TCAM_STAGE_IPMF1 = 0,
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
    DNX_FIELD_TCAM_STAGE_NOF,
    
    DNX_FIELD_TCAM_STAGE_E_DUMMY
} dnx_field_tcam_stage_e;

typedef enum {
    DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_INVALID = -1,
    DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_AUTO = 0,
    DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT,
    DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT_WITH_LOCATION,
    DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_NOF,
    
    DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_E_DUMMY
} dnx_field_tcam_bank_allocation_mode_e;

typedef enum {
    DNX_FIELD_TCAM_ENTRY_SIZE_INVALID = -1,
    DNX_FIELD_TCAM_ENTRY_SIZE_HALF = 0,
    DNX_FIELD_TCAM_ENTRY_SIZE_SINGLE = 1,
    DNX_FIELD_TCAM_ENTRY_SIZE_DOUBLE = 2,
    DNX_FIELD_TCAM_ENTRY_SIZE_COUNT = 3,
    
    DNX_FIELD_TCAM_ENTRY_SIZE_E_DUMMY
} dnx_field_tcam_entry_size_e;

typedef enum {
    DNX_FIELD_TCAM_CORE_INVALID = -1,
    DNX_FIELD_TCAM_CORE_0 = 0,
    DNX_FIELD_TCAM_CORE_1 = 1,
    DNX_FIELD_TCAM_CORE_ALL = 2,
    DNX_FIELD_TCAM_CORE_COUNT = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES,
    
    DNX_FIELD_TCAM_CORE_E_DUMMY
} dnx_field_tcam_core_e;



typedef struct {
    int dt_bank_id;
    dnx_field_tcam_stage_e stage;
    dnx_field_tcam_bank_allocation_mode_e bank_allocation_mode;
    uint32 key_size;
    uint32 action_size;
    uint32 actual_action_size;
    uint32 prefix_size;
    uint8 direct_table;
    dnx_field_tcam_context_sharing_handlers_get_p context_sharing_handlers_cb;
} dnx_field_tcam_access_fg_params_t;

typedef struct {
    SHR_BITDCL* v_bit;
} dnx_field_tcam_access_v_bit_t;

typedef struct {
    sw_state_index_htb_t* entry_location_hash;
    dnx_field_tcam_access_v_bit_t* valid_bmp;
    dnx_field_tcam_access_v_bit_t* entry_in_use_bmp;
    dnx_field_tcam_access_fg_params_t* fg_params;
    uint32** fg_prefix_per_core;
    uint8 cache_install_in_progress;
} dnx_field_tcam_access_t;


#endif 
