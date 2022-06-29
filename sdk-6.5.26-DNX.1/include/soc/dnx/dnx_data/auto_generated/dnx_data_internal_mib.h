
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_MIB_H_

#define _DNX_DATA_INTERNAL_MIB_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mib.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



typedef enum
{
    dnx_data_mib_submodule_general,
    dnx_data_mib_submodule_controlled_counters,

    
    _dnx_data_mib_submodule_nof
} dnx_data_mib_submodule_e;








int dnx_data_mib_general_feature_get(
    int unit,
    dnx_data_mib_general_feature_e feature);



typedef enum
{
    dnx_data_mib_general_define_nof_express_rows_per_cdmib_port,
    dnx_data_mib_general_define_nof_preempt_rows_per_cdmib_port,
    dnx_data_mib_general_define_nof_rows_per_cdmib_port,
    dnx_data_mib_general_define_nof_cdmib_port_per_pm,
    dnx_data_mib_general_define_nof_cdmib_port,
    dnx_data_mib_general_define_stat_interval,
    dnx_data_mib_general_define_stat_jumbo,

    
    _dnx_data_mib_general_define_nof
} dnx_data_mib_general_define_e;



uint32 dnx_data_mib_general_nof_express_rows_per_cdmib_port_get(
    int unit);


uint32 dnx_data_mib_general_nof_preempt_rows_per_cdmib_port_get(
    int unit);


uint32 dnx_data_mib_general_nof_rows_per_cdmib_port_get(
    int unit);


uint32 dnx_data_mib_general_nof_cdmib_port_per_pm_get(
    int unit);


uint32 dnx_data_mib_general_nof_cdmib_port_get(
    int unit);


uint32 dnx_data_mib_general_stat_interval_get(
    int unit);


uint32 dnx_data_mib_general_stat_jumbo_get(
    int unit);



typedef enum
{
    dnx_data_mib_general_table_stat_pbmp,

    
    _dnx_data_mib_general_table_nof
} dnx_data_mib_general_table_e;



const dnx_data_mib_general_stat_pbmp_t * dnx_data_mib_general_stat_pbmp_get(
    int unit);



shr_error_e dnx_data_mib_general_stat_pbmp_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_mib_general_stat_pbmp_info_get(
    int unit);






int dnx_data_mib_controlled_counters_feature_get(
    int unit,
    dnx_data_mib_controlled_counters_feature_e feature);



typedef enum
{
    dnx_data_mib_controlled_counters_define_flexmac_mib_general_cnt_len,
    dnx_data_mib_controlled_counters_define_flexmac_mib_rbyt_cnt_len,
    dnx_data_mib_controlled_counters_define_flexmac_mib_rrbyt_cnt_len,
    dnx_data_mib_controlled_counters_define_flexmac_mib_tbyt_cnt_len,

    
    _dnx_data_mib_controlled_counters_define_nof
} dnx_data_mib_controlled_counters_define_e;



uint32 dnx_data_mib_controlled_counters_flexmac_mib_general_cnt_len_get(
    int unit);


uint32 dnx_data_mib_controlled_counters_flexmac_mib_rbyt_cnt_len_get(
    int unit);


uint32 dnx_data_mib_controlled_counters_flexmac_mib_rrbyt_cnt_len_get(
    int unit);


uint32 dnx_data_mib_controlled_counters_flexmac_mib_tbyt_cnt_len_get(
    int unit);



typedef enum
{
    dnx_data_mib_controlled_counters_table_map_data,
    dnx_data_mib_controlled_counters_table_set_data,
    dnx_data_mib_controlled_counters_table_counter_data,
    dnx_data_mib_controlled_counters_table_flexmac_counter_data,

    
    _dnx_data_mib_controlled_counters_table_nof
} dnx_data_mib_controlled_counters_table_e;



const dnx_data_mib_controlled_counters_map_data_t * dnx_data_mib_controlled_counters_map_data_get(
    int unit,
    int counter_type,
    int set_type);


const dnx_data_mib_controlled_counters_set_data_t * dnx_data_mib_controlled_counters_set_data_get(
    int unit,
    int counter_id,
    int set_type);


const dnx_data_mib_controlled_counters_counter_data_t * dnx_data_mib_controlled_counters_counter_data_get(
    int unit,
    int counter_id);


const dnx_data_mib_controlled_counters_flexmac_counter_data_t * dnx_data_mib_controlled_counters_flexmac_counter_data_get(
    int unit,
    int counter_id);



shr_error_e dnx_data_mib_controlled_counters_map_data_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mib_controlled_counters_set_data_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mib_controlled_counters_counter_data_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_mib_controlled_counters_flexmac_counter_data_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_mib_controlled_counters_map_data_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mib_controlled_counters_set_data_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mib_controlled_counters_counter_data_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_mib_controlled_counters_flexmac_counter_data_info_get(
    int unit);



shr_error_e dnx_data_mib_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

