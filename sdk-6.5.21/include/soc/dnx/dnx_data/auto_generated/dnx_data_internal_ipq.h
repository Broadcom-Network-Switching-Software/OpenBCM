

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_IPQ_H_

#define _DNX_DATA_INTERNAL_IPQ_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_ipq_submodule_queues,
    dnx_data_ipq_submodule_regions,
    dnx_data_ipq_submodule_tc_map,

    
    _dnx_data_ipq_submodule_nof
} dnx_data_ipq_submodule_e;








int dnx_data_ipq_queues_feature_get(
    int unit,
    dnx_data_ipq_queues_feature_e feature);



typedef enum
{
    dnx_data_ipq_queues_define_nof_queues,
    dnx_data_ipq_queues_define_min_bundle_size,
    dnx_data_ipq_queues_define_fmq_bundle_size,
    dnx_data_ipq_queues_define_max_fmq_id,
    dnx_data_ipq_queues_define_queue_id_bits,
    dnx_data_ipq_queues_define_voq_offset_bits,
    dnx_data_ipq_queues_define_min_bundle_size_bits,
    dnx_data_ipq_queues_define_tc_profile_from_tm_command_nof_bits,
    dnx_data_ipq_queues_define_flow_quartet_nof_bits,
    dnx_data_ipq_queues_define_invalid_queue,

    
    _dnx_data_ipq_queues_define_nof
} dnx_data_ipq_queues_define_e;



uint32 dnx_data_ipq_queues_nof_queues_get(
    int unit);


uint32 dnx_data_ipq_queues_min_bundle_size_get(
    int unit);


uint32 dnx_data_ipq_queues_fmq_bundle_size_get(
    int unit);


uint32 dnx_data_ipq_queues_max_fmq_id_get(
    int unit);


uint32 dnx_data_ipq_queues_queue_id_bits_get(
    int unit);


uint32 dnx_data_ipq_queues_voq_offset_bits_get(
    int unit);


uint32 dnx_data_ipq_queues_min_bundle_size_bits_get(
    int unit);


uint32 dnx_data_ipq_queues_tc_profile_from_tm_command_nof_bits_get(
    int unit);


uint32 dnx_data_ipq_queues_flow_quartet_nof_bits_get(
    int unit);


uint32 dnx_data_ipq_queues_invalid_queue_get(
    int unit);



typedef enum
{

    
    _dnx_data_ipq_queues_table_nof
} dnx_data_ipq_queues_table_e;









int dnx_data_ipq_regions_feature_get(
    int unit,
    dnx_data_ipq_regions_feature_e feature);



typedef enum
{
    dnx_data_ipq_regions_define_nof_queues,

    
    _dnx_data_ipq_regions_define_nof
} dnx_data_ipq_regions_define_e;



uint32 dnx_data_ipq_regions_nof_queues_get(
    int unit);



typedef enum
{

    
    _dnx_data_ipq_regions_table_nof
} dnx_data_ipq_regions_table_e;









int dnx_data_ipq_tc_map_feature_get(
    int unit,
    dnx_data_ipq_tc_map_feature_e feature);



typedef enum
{
    dnx_data_ipq_tc_map_define_nof_voq_flow_profiles,
    dnx_data_ipq_tc_map_define_voq_flow_profiles_bits,
    dnx_data_ipq_tc_map_define_default_voq_flow_profile_idx,
    dnx_data_ipq_tc_map_define_nof_sysport_profiles,
    dnx_data_ipq_tc_map_define_default_sysport_profile_idx,
    dnx_data_ipq_tc_map_define_voq_resolution_type_max_value,

    
    _dnx_data_ipq_tc_map_define_nof
} dnx_data_ipq_tc_map_define_e;



uint32 dnx_data_ipq_tc_map_nof_voq_flow_profiles_get(
    int unit);


uint32 dnx_data_ipq_tc_map_voq_flow_profiles_bits_get(
    int unit);


uint32 dnx_data_ipq_tc_map_default_voq_flow_profile_idx_get(
    int unit);


uint32 dnx_data_ipq_tc_map_nof_sysport_profiles_get(
    int unit);


uint32 dnx_data_ipq_tc_map_default_sysport_profile_idx_get(
    int unit);


uint32 dnx_data_ipq_tc_map_voq_resolution_type_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_ipq_tc_map_table_nof
} dnx_data_ipq_tc_map_table_e;






shr_error_e dnx_data_ipq_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

