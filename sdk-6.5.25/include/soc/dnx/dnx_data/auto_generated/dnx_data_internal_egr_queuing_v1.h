
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_EGR_QUEUING_V1_H_

#define _DNX_DATA_INTERNAL_EGR_QUEUING_V1_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing_v1.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_egr_queuing_v1_submodule_general,
    dnx_data_egr_queuing_v1_submodule_egress_interfaces,
    dnx_data_egr_queuing_v1_submodule_egress_queues,
    dnx_data_egr_queuing_v1_submodule_scheduling,
    dnx_data_egr_queuing_v1_submodule_shaping,

    
    _dnx_data_egr_queuing_v1_submodule_nof
} dnx_data_egr_queuing_v1_submodule_e;








int dnx_data_egr_queuing_v1_general_feature_get(
    int unit,
    dnx_data_egr_queuing_v1_general_feature_e feature);



typedef enum
{

    
    _dnx_data_egr_queuing_v1_general_define_nof
} dnx_data_egr_queuing_v1_general_define_e;




typedef enum
{

    
    _dnx_data_egr_queuing_v1_general_table_nof
} dnx_data_egr_queuing_v1_general_table_e;









int dnx_data_egr_queuing_v1_egress_interfaces_feature_get(
    int unit,
    dnx_data_egr_queuing_v1_egress_interfaces_feature_e feature);



typedef enum
{
    dnx_data_egr_queuing_v1_egress_interfaces_define_nof_egr_interfaces,
    dnx_data_egr_queuing_v1_egress_interfaces_define_nof_channelized_egr_interfaces,

    
    _dnx_data_egr_queuing_v1_egress_interfaces_define_nof
} dnx_data_egr_queuing_v1_egress_interfaces_define_e;



uint32 dnx_data_egr_queuing_v1_egress_interfaces_nof_egr_interfaces_get(
    int unit);


uint32 dnx_data_egr_queuing_v1_egress_interfaces_nof_channelized_egr_interfaces_get(
    int unit);



typedef enum
{
    dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data,
    dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data,

    
    _dnx_data_egr_queuing_v1_egress_interfaces_table_nof
} dnx_data_egr_queuing_v1_egress_interfaces_table_e;



const dnx_data_egr_queuing_v1_egress_interfaces_mapping_data_t * dnx_data_egr_queuing_v1_egress_interfaces_mapping_data_get(
    int unit,
    int interface_type);


const dnx_data_egr_queuing_v1_egress_interfaces_per_speed_mapping_data_t * dnx_data_egr_queuing_v1_egress_interfaces_per_speed_mapping_data_get(
    int unit,
    int index);



shr_error_e dnx_data_egr_queuing_v1_egress_interfaces_mapping_data_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_egr_queuing_v1_egress_interfaces_per_speed_mapping_data_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_egr_queuing_v1_egress_interfaces_mapping_data_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_egr_queuing_v1_egress_interfaces_per_speed_mapping_data_info_get(
    int unit);






int dnx_data_egr_queuing_v1_egress_queues_feature_get(
    int unit,
    dnx_data_egr_queuing_v1_egress_queues_feature_e feature);



typedef enum
{
    dnx_data_egr_queuing_v1_egress_queues_define_nof_egr_queues,
    dnx_data_egr_queuing_v1_egress_queues_define_nof_egr_priorities,
    dnx_data_egr_queuing_v1_egress_queues_define_ps_queue_resolution,
    dnx_data_egr_queuing_v1_egress_queues_define_rcy_reserved_queue,
    dnx_data_egr_queuing_v1_egress_queues_define_nof_queue_pairs,
    dnx_data_egr_queuing_v1_egress_queues_define_total_nof_egr_queues,

    
    _dnx_data_egr_queuing_v1_egress_queues_define_nof
} dnx_data_egr_queuing_v1_egress_queues_define_e;



uint32 dnx_data_egr_queuing_v1_egress_queues_nof_egr_queues_get(
    int unit);


uint32 dnx_data_egr_queuing_v1_egress_queues_nof_egr_priorities_get(
    int unit);


uint32 dnx_data_egr_queuing_v1_egress_queues_ps_queue_resolution_get(
    int unit);


uint32 dnx_data_egr_queuing_v1_egress_queues_rcy_reserved_queue_get(
    int unit);


uint32 dnx_data_egr_queuing_v1_egress_queues_nof_queue_pairs_get(
    int unit);


uint32 dnx_data_egr_queuing_v1_egress_queues_total_nof_egr_queues_get(
    int unit);



typedef enum
{
    dnx_data_egr_queuing_v1_egress_queues_table_supported_nof_priorities,

    
    _dnx_data_egr_queuing_v1_egress_queues_table_nof
} dnx_data_egr_queuing_v1_egress_queues_table_e;



const dnx_data_egr_queuing_v1_egress_queues_supported_nof_priorities_t * dnx_data_egr_queuing_v1_egress_queues_supported_nof_priorities_get(
    int unit,
    int priorities);



shr_error_e dnx_data_egr_queuing_v1_egress_queues_supported_nof_priorities_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_egr_queuing_v1_egress_queues_supported_nof_priorities_info_get(
    int unit);






int dnx_data_egr_queuing_v1_scheduling_feature_get(
    int unit,
    dnx_data_egr_queuing_v1_scheduling_feature_e feature);



typedef enum
{
    dnx_data_egr_queuing_v1_scheduling_define_nof_interface_calendar_slots,

    
    _dnx_data_egr_queuing_v1_scheduling_define_nof
} dnx_data_egr_queuing_v1_scheduling_define_e;



uint32 dnx_data_egr_queuing_v1_scheduling_nof_interface_calendar_slots_get(
    int unit);



typedef enum
{

    
    _dnx_data_egr_queuing_v1_scheduling_table_nof
} dnx_data_egr_queuing_v1_scheduling_table_e;









int dnx_data_egr_queuing_v1_shaping_feature_get(
    int unit,
    dnx_data_egr_queuing_v1_shaping_feature_e feature);



typedef enum
{
    dnx_data_egr_queuing_v1_shaping_define_default_max_burst,

    
    _dnx_data_egr_queuing_v1_shaping_define_nof
} dnx_data_egr_queuing_v1_shaping_define_e;



uint32 dnx_data_egr_queuing_v1_shaping_default_max_burst_get(
    int unit);



typedef enum
{

    
    _dnx_data_egr_queuing_v1_shaping_table_nof
} dnx_data_egr_queuing_v1_shaping_table_e;






shr_error_e dnx_data_egr_queuing_v1_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

