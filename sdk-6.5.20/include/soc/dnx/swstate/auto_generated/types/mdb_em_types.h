
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __MDB_EM_TYPES_H__
#define __MDB_EM_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <soc/dnxc/swstate/callbacks/sw_state_htb_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>



#define MDB_MAX_EM_KEY_SIZE_IN_UINT32 (5)



typedef struct {
    
    uint8 encoding;
    
    uint8 size;
} vmv_value_parameters_t;

typedef struct {
    
    uint8 value[DNX_DATA_MAX_MDB_EM_NOF_ENCODING_VALUES];
    
    uint8 size[DNX_DATA_MAX_MDB_EM_NOF_ENCODING_VALUES];
    
    vmv_value_parameters_t encoding_map[DNX_DATA_MAX_MDB_EM_VMV_NOF_VALUES];
    
    uint8 app_id_size[DNX_DATA_MAX_MDB_EM_MAX_NOF_TIDS];
    
    int max_payload_size[DNX_DATA_MAX_MDB_EM_MAX_NOF_TIDS];
} vmv_info_t;


typedef struct {
    
    sw_state_htbl_t shadow_em_db[DBAL_NOF_PHYSICAL_TABLES];
    
    vmv_info_t vmv_info[DBAL_NOF_PHYSICAL_TABLES];
    
    uint32 mact_stamp;
} mdb_em_sw_state_t;


#endif 
