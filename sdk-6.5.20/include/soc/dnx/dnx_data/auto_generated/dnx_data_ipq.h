

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_IPQ_H_

#define _DNX_DATA_IPQ_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_ipq.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_ipq_init(
    int unit);







typedef enum
{
    
    dnx_data_ipq_queues_voq_resolution_per_dp,
    
    dnx_data_ipq_queues_tc_profile_from_tm_command,

    
    _dnx_data_ipq_queues_feature_nof
} dnx_data_ipq_queues_feature_e;



typedef int(
    *dnx_data_ipq_queues_feature_get_f) (
    int unit,
    dnx_data_ipq_queues_feature_e feature);


typedef uint32(
    *dnx_data_ipq_queues_nof_queues_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ipq_queues_min_bundle_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ipq_queues_fmq_bundle_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ipq_queues_max_fmq_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ipq_queues_queue_id_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ipq_queues_voq_offset_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ipq_queues_min_bundle_size_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ipq_queues_tc_profile_from_tm_command_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ipq_queues_flow_quartet_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ipq_queues_invalid_queue_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_ipq_queues_feature_get_f feature_get;
    
    dnx_data_ipq_queues_nof_queues_get_f nof_queues_get;
    
    dnx_data_ipq_queues_min_bundle_size_get_f min_bundle_size_get;
    
    dnx_data_ipq_queues_fmq_bundle_size_get_f fmq_bundle_size_get;
    
    dnx_data_ipq_queues_max_fmq_id_get_f max_fmq_id_get;
    
    dnx_data_ipq_queues_queue_id_bits_get_f queue_id_bits_get;
    
    dnx_data_ipq_queues_voq_offset_bits_get_f voq_offset_bits_get;
    
    dnx_data_ipq_queues_min_bundle_size_bits_get_f min_bundle_size_bits_get;
    
    dnx_data_ipq_queues_tc_profile_from_tm_command_nof_bits_get_f tc_profile_from_tm_command_nof_bits_get;
    
    dnx_data_ipq_queues_flow_quartet_nof_bits_get_f flow_quartet_nof_bits_get;
    
    dnx_data_ipq_queues_invalid_queue_get_f invalid_queue_get;
} dnx_data_if_ipq_queues_t;







typedef enum
{

    
    _dnx_data_ipq_regions_feature_nof
} dnx_data_ipq_regions_feature_e;



typedef int(
    *dnx_data_ipq_regions_feature_get_f) (
    int unit,
    dnx_data_ipq_regions_feature_e feature);


typedef uint32(
    *dnx_data_ipq_regions_nof_queues_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_ipq_regions_feature_get_f feature_get;
    
    dnx_data_ipq_regions_nof_queues_get_f nof_queues_get;
} dnx_data_if_ipq_regions_t;







typedef enum
{

    
    _dnx_data_ipq_tc_map_feature_nof
} dnx_data_ipq_tc_map_feature_e;



typedef int(
    *dnx_data_ipq_tc_map_feature_get_f) (
    int unit,
    dnx_data_ipq_tc_map_feature_e feature);


typedef uint32(
    *dnx_data_ipq_tc_map_nof_voq_flow_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ipq_tc_map_voq_flow_profiles_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ipq_tc_map_default_voq_flow_profile_idx_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ipq_tc_map_nof_sysport_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ipq_tc_map_default_sysport_profile_idx_get_f) (
    int unit);


typedef uint32(
    *dnx_data_ipq_tc_map_voq_resolution_type_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_ipq_tc_map_feature_get_f feature_get;
    
    dnx_data_ipq_tc_map_nof_voq_flow_profiles_get_f nof_voq_flow_profiles_get;
    
    dnx_data_ipq_tc_map_voq_flow_profiles_bits_get_f voq_flow_profiles_bits_get;
    
    dnx_data_ipq_tc_map_default_voq_flow_profile_idx_get_f default_voq_flow_profile_idx_get;
    
    dnx_data_ipq_tc_map_nof_sysport_profiles_get_f nof_sysport_profiles_get;
    
    dnx_data_ipq_tc_map_default_sysport_profile_idx_get_f default_sysport_profile_idx_get;
    
    dnx_data_ipq_tc_map_voq_resolution_type_max_value_get_f voq_resolution_type_max_value_get;
} dnx_data_if_ipq_tc_map_t;





typedef struct
{
    
    dnx_data_if_ipq_queues_t queues;
    
    dnx_data_if_ipq_regions_t regions;
    
    dnx_data_if_ipq_tc_map_t tc_map;
} dnx_data_if_ipq_t;




extern dnx_data_if_ipq_t dnx_data_ipq;


#endif 

