

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_MIB_H_

#define _DNX_DATA_MIB_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <bcm/port.h>
#include <bcm/stat.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/stat/mib/mib_stat.h>
#include <soc/counter.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_mib.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_mib_init(
    int unit);






typedef struct
{
    
    bcm_pbmp_t pbmp;
} dnx_data_mib_general_stat_pbmp_t;



typedef enum
{
    
    dnx_data_mib_general_mib_counters_support,

    
    _dnx_data_mib_general_feature_nof
} dnx_data_mib_general_feature_e;



typedef int(
    *dnx_data_mib_general_feature_get_f) (
    int unit,
    dnx_data_mib_general_feature_e feature);


typedef uint32(
    *dnx_data_mib_general_nof_row_per_cdu_lane_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mib_general_stat_interval_get_f) (
    int unit);


typedef uint32(
    *dnx_data_mib_general_stat_jumbo_get_f) (
    int unit);


typedef const dnx_data_mib_general_stat_pbmp_t *(
    *dnx_data_mib_general_stat_pbmp_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_mib_general_feature_get_f feature_get;
    
    dnx_data_mib_general_nof_row_per_cdu_lane_get_f nof_row_per_cdu_lane_get;
    
    dnx_data_mib_general_stat_interval_get_f stat_interval_get;
    
    dnx_data_mib_general_stat_jumbo_get_f stat_jumbo_get;
    
    dnx_data_mib_general_stat_pbmp_get_f stat_pbmp_get;
    
    dnxc_data_table_info_get_f stat_pbmp_info_get;
} dnx_data_if_mib_general_t;






typedef struct
{
    
    uint32 counters_to_add_array[DNX_MIB_MAX_COUNTER_BUNDLE];
    
    uint32 counters_to_subtract_array[DNX_MIB_MAX_COUNTER_BUNDLE];
    
    dnx_mib_counters_map_get_cb_f counters_to_get_cb;
} dnx_data_mib_controlled_counters_map_data_t;


typedef struct
{
    
    int hw_counter_id;
    
    soc_get_counter_f get_cb;
    
    soc_clear_counter_f clear_cb;
} dnx_data_mib_controlled_counters_set_data_t;


typedef struct
{
    
    int length;
    
    int row;
    
    int offset_in_row;
    
    char *doc;
} dnx_data_mib_controlled_counters_counter_data_t;



typedef enum
{

    
    _dnx_data_mib_controlled_counters_feature_nof
} dnx_data_mib_controlled_counters_feature_e;



typedef int(
    *dnx_data_mib_controlled_counters_feature_get_f) (
    int unit,
    dnx_data_mib_controlled_counters_feature_e feature);


typedef const dnx_data_mib_controlled_counters_map_data_t *(
    *dnx_data_mib_controlled_counters_map_data_get_f) (
    int unit,
    int counter_type,
    int set_type);


typedef const dnx_data_mib_controlled_counters_set_data_t *(
    *dnx_data_mib_controlled_counters_set_data_get_f) (
    int unit,
    int counter_id,
    int set_type);


typedef const dnx_data_mib_controlled_counters_counter_data_t *(
    *dnx_data_mib_controlled_counters_counter_data_get_f) (
    int unit,
    int counter_id);



typedef struct
{
    
    dnx_data_mib_controlled_counters_feature_get_f feature_get;
    
    dnx_data_mib_controlled_counters_map_data_get_f map_data_get;
    
    dnxc_data_table_info_get_f map_data_info_get;
    
    dnx_data_mib_controlled_counters_set_data_get_f set_data_get;
    
    dnxc_data_table_info_get_f set_data_info_get;
    
    dnx_data_mib_controlled_counters_counter_data_get_f counter_data_get;
    
    dnxc_data_table_info_get_f counter_data_info_get;
} dnx_data_if_mib_controlled_counters_t;





typedef struct
{
    
    dnx_data_if_mib_general_t general;
    
    dnx_data_if_mib_controlled_counters_t controlled_counters;
} dnx_data_if_mib_t;




extern dnx_data_if_mib_t dnx_data_mib;


#endif 

