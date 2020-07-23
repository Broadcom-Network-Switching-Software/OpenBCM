
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __ALGO_LIF_TYPES_H__
#define __ALGO_LIF_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnxc/swstate/callbacks/sw_state_htb_callbacks.h>
#include <soc/dnxc/swstate/callbacks/sw_state_ll_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>
#include <soc/dnxc/swstate/types/sw_state_linked_list.h>


#define MAX_EEDB_BANKS_PER_OUTLIF_BANK 4




typedef enum {
    
    DNX_LOCAL_OUTLIF_LL_ALWAYS = 2,
    
    DNX_LOCAL_OUTLIF_LL_OPTIONAL = 1,
    
    DNX_LOCAL_OUTLIF_LL_NEVER = 0
} dnx_local_outlif_ll_indication_e;




typedef struct {
    
    dbal_tables_e dbal_table;
    
    uint8 dbal_result_type;
} dbal_to_phase_info_t;


typedef struct {
    
    uint8 assigned;
    
    uint8 in_use;
    
    uint8 used_logical_phase;
    
    uint8 ll_in_use;
    
    uint8 ll_index;
    
    uint8 eedb_data_banks[MAX_EEDB_BANKS_PER_OUTLIF_BANK];
} local_outlif_bank_info_t;


typedef struct {
    
    uint8 physical_phase;
    
    uint8 address_granularity;
    
    uint32 first_bank;
    
    uint32 last_bank;
    
    uint8 first_ll_bank;
    
    uint8 last_ll_bank;
    sw_state_htbl_t dbal_valid_combinations;
} outlif_logical_phase_info_t;


typedef struct {
    
    SHR_BITDCL* bank_occupation_bitmap;
    
    SHR_BITDCL* outlif_prefix_occupation_bitmap;
    
    sw_state_multihead_ll_t used_data_bank_per_logical_phase;
} eedb_banks_info_t;

typedef struct {
    
    uint8 disable_mdb_clusters;
    
    uint8 disable_eedb_data_banks;
    
    local_outlif_bank_info_t* outlif_bank_info;
    
    outlif_logical_phase_info_t* logical_phase_info;
    
    uint8* physical_phase_to_logical_phase_map;
    
    eedb_banks_info_t eedb_banks_info;
} local_outlif_info_t;


#endif 
