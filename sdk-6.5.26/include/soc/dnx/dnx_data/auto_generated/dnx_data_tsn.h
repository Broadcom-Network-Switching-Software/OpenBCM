
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_TSN_H_

#define _DNX_DATA_TSN_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_tsn.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_tsn_init(
    int unit);







typedef enum
{
    dnx_data_tsn_general_is_supported,

    
    _dnx_data_tsn_general_feature_nof
} dnx_data_tsn_general_feature_e;



typedef int(
    *dnx_data_tsn_general_feature_get_f) (
    int unit,
    dnx_data_tsn_general_feature_e feature);



typedef struct
{
    
    dnx_data_tsn_general_feature_get_f feature_get;
} dnx_data_if_tsn_general_t;







typedef enum
{

    
    _dnx_data_tsn_preemption_feature_nof
} dnx_data_tsn_preemption_feature_e;



typedef int(
    *dnx_data_tsn_preemption_feature_get_f) (
    int unit,
    dnx_data_tsn_preemption_feature_e feature);


typedef uint32(
    *dnx_data_tsn_preemption_verify_max_time_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tsn_preemption_verify_max_attempts_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tsn_preemption_non_final_frag_size_tx_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tsn_preemption_final_frag_size_tx_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tsn_preemption_non_final_frag_size_rx_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tsn_preemption_final_frag_size_rx_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tsn_preemption_frag_size_bytes_in_hw_unit_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_tsn_preemption_feature_get_f feature_get;
    
    dnx_data_tsn_preemption_verify_max_time_get_f verify_max_time_get;
    
    dnx_data_tsn_preemption_verify_max_attempts_get_f verify_max_attempts_get;
    
    dnx_data_tsn_preemption_non_final_frag_size_tx_get_f non_final_frag_size_tx_get;
    
    dnx_data_tsn_preemption_final_frag_size_tx_get_f final_frag_size_tx_get;
    
    dnx_data_tsn_preemption_non_final_frag_size_rx_get_f non_final_frag_size_rx_get;
    
    dnx_data_tsn_preemption_final_frag_size_rx_get_f final_frag_size_rx_get;
    
    dnx_data_tsn_preemption_frag_size_bytes_in_hw_unit_get_f frag_size_bytes_in_hw_unit_get;
} dnx_data_if_tsn_preemption_t;







typedef enum
{

    
    _dnx_data_tsn_time_feature_nof
} dnx_data_tsn_time_feature_e;



typedef int(
    *dnx_data_tsn_time_feature_get_f) (
    int unit,
    dnx_data_tsn_time_feature_e feature);


typedef uint32(
    *dnx_data_tsn_time_tick_period_ns_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tsn_time_tick_period_min_ns_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tsn_time_tsn_counter_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tsn_time_tas_tod_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tsn_time_tas_tod_source_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_tsn_time_feature_get_f feature_get;
    
    dnx_data_tsn_time_tick_period_ns_get_f tick_period_ns_get;
    
    dnx_data_tsn_time_tick_period_min_ns_get_f tick_period_min_ns_get;
    
    dnx_data_tsn_time_tsn_counter_nof_bits_get_f tsn_counter_nof_bits_get;
    
    dnx_data_tsn_time_tas_tod_nof_bits_get_f tas_tod_nof_bits_get;
    
    dnx_data_tsn_time_tas_tod_source_get_f tas_tod_source_get;
} dnx_data_if_tsn_time_t;







typedef enum
{

    
    _dnx_data_tsn_tas_feature_nof
} dnx_data_tsn_tas_feature_e;



typedef int(
    *dnx_data_tsn_tas_feature_get_f) (
    int unit,
    dnx_data_tsn_tas_feature_e feature);


typedef uint32(
    *dnx_data_tsn_tas_max_time_intervals_entries_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tsn_tas_nof_tas_ports_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tsn_tas_max_profiles_per_port_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_tsn_tas_feature_get_f feature_get;
    
    dnx_data_tsn_tas_max_time_intervals_entries_get_f max_time_intervals_entries_get;
    
    dnx_data_tsn_tas_nof_tas_ports_get_f nof_tas_ports_get;
    
    dnx_data_tsn_tas_max_profiles_per_port_get_f max_profiles_per_port_get;
} dnx_data_if_tsn_tas_t;





typedef struct
{
    
    dnx_data_if_tsn_general_t general;
    
    dnx_data_if_tsn_preemption_t preemption;
    
    dnx_data_if_tsn_time_t time;
    
    dnx_data_if_tsn_tas_t tas;
} dnx_data_if_tsn_t;




extern dnx_data_if_tsn_t dnx_data_tsn;


#endif 

