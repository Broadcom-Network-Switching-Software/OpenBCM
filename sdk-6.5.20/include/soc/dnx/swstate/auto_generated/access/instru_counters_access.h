
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __INSTRU_COUNTERS_ACCESS_H__
#define __INSTRU_COUNTERS_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/instru_counters_types.h>



typedef int (*instru_sync_counters_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*instru_sync_counters_init_cb)(
    int unit);


typedef int (*instru_sync_counters_icgm_countes_sync_counters_started_set_cb)(
    int unit, uint8 sync_counters_started);


typedef int (*instru_sync_counters_icgm_countes_sync_counters_started_get_cb)(
    int unit, uint8 *sync_counters_started);


typedef int (*instru_sync_counters_icgm_countes_tod_1588_timestamp_start_set_cb)(
    int unit, uint64 tod_1588_timestamp_start);


typedef int (*instru_sync_counters_icgm_countes_tod_1588_timestamp_start_get_cb)(
    int unit, uint64 *tod_1588_timestamp_start);


typedef int (*instru_sync_counters_icgm_countes_per_core_info_alloc_cb)(
    int unit);


typedef int (*instru_sync_counters_icgm_countes_per_core_info_record_last_read_nof_set_cb)(
    int unit, uint32 per_core_info_idx_0, uint32 record_last_read_nof);


typedef int (*instru_sync_counters_icgm_countes_per_core_info_record_last_read_nof_get_cb)(
    int unit, uint32 per_core_info_idx_0, uint32 *record_last_read_nof);


typedef int (*instru_sync_counters_nif_countes_sync_counters_started_set_cb)(
    int unit, uint8 sync_counters_started);


typedef int (*instru_sync_counters_nif_countes_sync_counters_started_get_cb)(
    int unit, uint8 *sync_counters_started);


typedef int (*instru_sync_counters_nif_countes_tod_1588_timestamp_start_set_cb)(
    int unit, uint64 tod_1588_timestamp_start);


typedef int (*instru_sync_counters_nif_countes_tod_1588_timestamp_start_get_cb)(
    int unit, uint64 *tod_1588_timestamp_start);


typedef int (*instru_sync_counters_nif_countes_per_ethu_info_alloc_cb)(
    int unit);


typedef int (*instru_sync_counters_nif_countes_per_ethu_info_record_last_read_nof_set_cb)(
    int unit, uint32 per_ethu_info_idx_0, uint32 per_ethu_info_idx_1, uint32 record_last_read_nof_idx_0, uint32 record_last_read_nof);


typedef int (*instru_sync_counters_nif_countes_per_ethu_info_record_last_read_nof_get_cb)(
    int unit, uint32 per_ethu_info_idx_0, uint32 per_ethu_info_idx_1, uint32 record_last_read_nof_idx_0, uint32 *record_last_read_nof);


typedef int (*instru_sync_counters_nif_countes_per_ethu_info_record_last_read_nof_alloc_cb)(
    int unit, uint32 per_ethu_info_idx_0, uint32 per_ethu_info_idx_1);




typedef struct {
    instru_sync_counters_icgm_countes_sync_counters_started_set_cb set;
    instru_sync_counters_icgm_countes_sync_counters_started_get_cb get;
} instru_sync_counters_icgm_countes_sync_counters_started_cbs;


typedef struct {
    instru_sync_counters_icgm_countes_tod_1588_timestamp_start_set_cb set;
    instru_sync_counters_icgm_countes_tod_1588_timestamp_start_get_cb get;
} instru_sync_counters_icgm_countes_tod_1588_timestamp_start_cbs;


typedef struct {
    instru_sync_counters_icgm_countes_per_core_info_record_last_read_nof_set_cb set;
    instru_sync_counters_icgm_countes_per_core_info_record_last_read_nof_get_cb get;
} instru_sync_counters_icgm_countes_per_core_info_record_last_read_nof_cbs;


typedef struct {
    instru_sync_counters_icgm_countes_per_core_info_alloc_cb alloc;
    
    instru_sync_counters_icgm_countes_per_core_info_record_last_read_nof_cbs record_last_read_nof;
} instru_sync_counters_icgm_countes_per_core_info_cbs;


typedef struct {
    
    instru_sync_counters_icgm_countes_sync_counters_started_cbs sync_counters_started;
    
    instru_sync_counters_icgm_countes_tod_1588_timestamp_start_cbs tod_1588_timestamp_start;
    
    instru_sync_counters_icgm_countes_per_core_info_cbs per_core_info;
} instru_sync_counters_icgm_countes_cbs;


typedef struct {
    instru_sync_counters_nif_countes_sync_counters_started_set_cb set;
    instru_sync_counters_nif_countes_sync_counters_started_get_cb get;
} instru_sync_counters_nif_countes_sync_counters_started_cbs;


typedef struct {
    instru_sync_counters_nif_countes_tod_1588_timestamp_start_set_cb set;
    instru_sync_counters_nif_countes_tod_1588_timestamp_start_get_cb get;
} instru_sync_counters_nif_countes_tod_1588_timestamp_start_cbs;


typedef struct {
    instru_sync_counters_nif_countes_per_ethu_info_record_last_read_nof_set_cb set;
    instru_sync_counters_nif_countes_per_ethu_info_record_last_read_nof_get_cb get;
    instru_sync_counters_nif_countes_per_ethu_info_record_last_read_nof_alloc_cb alloc;
} instru_sync_counters_nif_countes_per_ethu_info_record_last_read_nof_cbs;


typedef struct {
    instru_sync_counters_nif_countes_per_ethu_info_alloc_cb alloc;
    
    instru_sync_counters_nif_countes_per_ethu_info_record_last_read_nof_cbs record_last_read_nof;
} instru_sync_counters_nif_countes_per_ethu_info_cbs;


typedef struct {
    
    instru_sync_counters_nif_countes_sync_counters_started_cbs sync_counters_started;
    
    instru_sync_counters_nif_countes_tod_1588_timestamp_start_cbs tod_1588_timestamp_start;
    
    instru_sync_counters_nif_countes_per_ethu_info_cbs per_ethu_info;
} instru_sync_counters_nif_countes_cbs;


typedef struct {
    instru_sync_counters_is_init_cb is_init;
    instru_sync_counters_init_cb init;
    
    instru_sync_counters_icgm_countes_cbs icgm_countes;
    
    instru_sync_counters_nif_countes_cbs nif_countes;
} instru_sync_counters_cbs;





extern instru_sync_counters_cbs instru_sync_counters;

#endif 
