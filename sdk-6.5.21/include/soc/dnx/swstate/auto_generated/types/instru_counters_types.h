
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __INSTRU_COUNTERS_TYPES_H__
#define __INSTRU_COUNTERS_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>



typedef struct {
    
    uint32 record_last_read_nof;
} sync_counters_icgm_per_core_t;


typedef struct {
    
    uint8 sync_counters_started;
    
    uint64 tod_1588_timestamp_start;
    
    sync_counters_icgm_per_core_t* per_core_info;
} icgm_sync_counters_info_t;


typedef struct {
    
    uint32* record_last_read_nof;
} sync_counters_nif_per_eth_info_t;


typedef struct {
    
    uint8 sync_counters_started;
    
    uint64 tod_1588_timestamp_start;
    
    sync_counters_nif_per_eth_info_t** per_ethu_info;
} nif_sync_counters_info_t;


typedef struct {
    
    icgm_sync_counters_info_t icgm_countes;
    
    nif_sync_counters_info_t nif_countes;
} instru_sync_counters_sw_state_t;


#endif 
