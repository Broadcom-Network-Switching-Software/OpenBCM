
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_TSN_H_

#define _DNX_DATA_INTERNAL_TSN_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tsn.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



typedef enum
{
    dnx_data_tsn_submodule_general,
    dnx_data_tsn_submodule_preemption,
    dnx_data_tsn_submodule_time,
    dnx_data_tsn_submodule_tas,

    
    _dnx_data_tsn_submodule_nof
} dnx_data_tsn_submodule_e;








int dnx_data_tsn_general_feature_get(
    int unit,
    dnx_data_tsn_general_feature_e feature);



typedef enum
{

    
    _dnx_data_tsn_general_define_nof
} dnx_data_tsn_general_define_e;




typedef enum
{

    
    _dnx_data_tsn_general_table_nof
} dnx_data_tsn_general_table_e;









int dnx_data_tsn_preemption_feature_get(
    int unit,
    dnx_data_tsn_preemption_feature_e feature);



typedef enum
{
    dnx_data_tsn_preemption_define_verify_max_time,
    dnx_data_tsn_preemption_define_verify_max_attempts,
    dnx_data_tsn_preemption_define_non_final_frag_size_tx,
    dnx_data_tsn_preemption_define_final_frag_size_tx,
    dnx_data_tsn_preemption_define_non_final_frag_size_rx,
    dnx_data_tsn_preemption_define_final_frag_size_rx,
    dnx_data_tsn_preemption_define_frag_size_bytes_in_hw_unit,

    
    _dnx_data_tsn_preemption_define_nof
} dnx_data_tsn_preemption_define_e;



uint32 dnx_data_tsn_preemption_verify_max_time_get(
    int unit);


uint32 dnx_data_tsn_preemption_verify_max_attempts_get(
    int unit);


uint32 dnx_data_tsn_preemption_non_final_frag_size_tx_get(
    int unit);


uint32 dnx_data_tsn_preemption_final_frag_size_tx_get(
    int unit);


uint32 dnx_data_tsn_preemption_non_final_frag_size_rx_get(
    int unit);


uint32 dnx_data_tsn_preemption_final_frag_size_rx_get(
    int unit);


uint32 dnx_data_tsn_preemption_frag_size_bytes_in_hw_unit_get(
    int unit);



typedef enum
{

    
    _dnx_data_tsn_preemption_table_nof
} dnx_data_tsn_preemption_table_e;









int dnx_data_tsn_time_feature_get(
    int unit,
    dnx_data_tsn_time_feature_e feature);



typedef enum
{
    dnx_data_tsn_time_define_tick_period_ns,
    dnx_data_tsn_time_define_tick_period_min_ns,
    dnx_data_tsn_time_define_tsn_counter_nof_bits,
    dnx_data_tsn_time_define_tas_tod_nof_bits,
    dnx_data_tsn_time_define_tas_tod_source,

    
    _dnx_data_tsn_time_define_nof
} dnx_data_tsn_time_define_e;



uint32 dnx_data_tsn_time_tick_period_ns_get(
    int unit);


uint32 dnx_data_tsn_time_tick_period_min_ns_get(
    int unit);


uint32 dnx_data_tsn_time_tsn_counter_nof_bits_get(
    int unit);


uint32 dnx_data_tsn_time_tas_tod_nof_bits_get(
    int unit);


uint32 dnx_data_tsn_time_tas_tod_source_get(
    int unit);



typedef enum
{

    
    _dnx_data_tsn_time_table_nof
} dnx_data_tsn_time_table_e;









int dnx_data_tsn_tas_feature_get(
    int unit,
    dnx_data_tsn_tas_feature_e feature);



typedef enum
{
    dnx_data_tsn_tas_define_max_time_intervals_entries,
    dnx_data_tsn_tas_define_nof_tas_ports,
    dnx_data_tsn_tas_define_max_profiles_per_port,

    
    _dnx_data_tsn_tas_define_nof
} dnx_data_tsn_tas_define_e;



uint32 dnx_data_tsn_tas_max_time_intervals_entries_get(
    int unit);


uint32 dnx_data_tsn_tas_nof_tas_ports_get(
    int unit);


uint32 dnx_data_tsn_tas_max_profiles_per_port_get(
    int unit);



typedef enum
{

    
    _dnx_data_tsn_tas_table_nof
} dnx_data_tsn_tas_table_e;






shr_error_e dnx_data_tsn_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

