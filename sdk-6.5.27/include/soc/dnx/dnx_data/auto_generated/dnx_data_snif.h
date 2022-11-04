
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_SNIF_H_

#define _DNX_DATA_SNIF_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_snif.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_snif_init(
    int unit);







typedef enum
{
    dnx_data_snif_general_original_destination_is_supported,

    
    _dnx_data_snif_general_feature_nof
} dnx_data_snif_general_feature_e;



typedef int(
    *dnx_data_snif_general_feature_get_f) (
    int unit,
    dnx_data_snif_general_feature_e feature);


typedef uint32(
    *dnx_data_snif_general_cud_type_standard_get_f) (
    int unit);


typedef uint32(
    *dnx_data_snif_general_cud_type_sflow_get_f) (
    int unit);


typedef uint32(
    *dnx_data_snif_general_cud_type_mirror_on_drop_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_snif_general_feature_get_f feature_get;
    
    dnx_data_snif_general_cud_type_standard_get_f cud_type_standard_get;
    
    dnx_data_snif_general_cud_type_sflow_get_f cud_type_sflow_get;
    
    dnx_data_snif_general_cud_type_mirror_on_drop_get_f cud_type_mirror_on_drop_get;
} dnx_data_if_snif_general_t;







typedef enum
{
    dnx_data_snif_ingress_crop_size_config_is_supported,
    dnx_data_snif_ingress_mirror_on_drop_admit_profile_supported,
    dnx_data_snif_ingress_ext_stat_dedicated_config,
    dnx_data_snif_ingress_itpp_delta_supported,

    
    _dnx_data_snif_ingress_feature_nof
} dnx_data_snif_ingress_feature_e;



typedef int(
    *dnx_data_snif_ingress_feature_get_f) (
    int unit,
    dnx_data_snif_ingress_feature_e feature);


typedef uint32(
    *dnx_data_snif_ingress_nof_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_snif_ingress_mirror_nof_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_snif_ingress_nof_mirror_on_drop_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_snif_ingress_mirror_nof_vlan_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_snif_ingress_mirror_default_tagged_profile_get_f) (
    int unit);


typedef uint32(
    *dnx_data_snif_ingress_mirror_untagged_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_snif_ingress_prob_max_val_get_f) (
    int unit);


typedef uint32(
    *dnx_data_snif_ingress_crop_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_snif_ingress_keep_orig_dest_on_dsp_ext_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_snif_ingress_feature_get_f feature_get;
    
    dnx_data_snif_ingress_nof_profiles_get_f nof_profiles_get;
    
    dnx_data_snif_ingress_mirror_nof_profiles_get_f mirror_nof_profiles_get;
    
    dnx_data_snif_ingress_nof_mirror_on_drop_profiles_get_f nof_mirror_on_drop_profiles_get;
    
    dnx_data_snif_ingress_mirror_nof_vlan_profiles_get_f mirror_nof_vlan_profiles_get;
    
    dnx_data_snif_ingress_mirror_default_tagged_profile_get_f mirror_default_tagged_profile_get;
    
    dnx_data_snif_ingress_mirror_untagged_profiles_get_f mirror_untagged_profiles_get;
    
    dnx_data_snif_ingress_prob_max_val_get_f prob_max_val_get;
    
    dnx_data_snif_ingress_crop_size_get_f crop_size_get;
    
    dnx_data_snif_ingress_keep_orig_dest_on_dsp_ext_get_f keep_orig_dest_on_dsp_ext_get;
} dnx_data_if_snif_ingress_t;






typedef struct
{
    int forward_port;
} dnx_data_snif_egress_rcy_mirror_to_forward_port_map_t;



typedef enum
{
    dnx_data_snif_egress_probability_sample,
    dnx_data_snif_egress_egress_null_profile,
    dnx_data_snif_egress_probability_profiles,
    dnx_data_snif_egress_crop_size_profiles,
    dnx_data_snif_egress_egress_mirror_mode,
    dnx_data_snif_egress_high_scale_mapping_db_supported,

    
    _dnx_data_snif_egress_feature_nof
} dnx_data_snif_egress_feature_e;



typedef int(
    *dnx_data_snif_egress_feature_get_f) (
    int unit,
    dnx_data_snif_egress_feature_e feature);


typedef uint32(
    *dnx_data_snif_egress_mirror_nof_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_snif_egress_snoop_nof_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_snif_egress_mirror_nof_vlan_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_snif_egress_mirror_default_profile_get_f) (
    int unit);


typedef uint32(
    *dnx_data_snif_egress_prob_max_val_get_f) (
    int unit);


typedef uint32(
    *dnx_data_snif_egress_prob_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_snif_egress_probability_nof_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_snif_egress_crop_size_nof_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_snif_egress_crop_size_granularity_get_f) (
    int unit);


typedef const dnx_data_snif_egress_rcy_mirror_to_forward_port_map_t *(
    *dnx_data_snif_egress_rcy_mirror_to_forward_port_map_get_f) (
    int unit,
    int port);



typedef struct
{
    
    dnx_data_snif_egress_feature_get_f feature_get;
    
    dnx_data_snif_egress_mirror_nof_profiles_get_f mirror_nof_profiles_get;
    
    dnx_data_snif_egress_snoop_nof_profiles_get_f snoop_nof_profiles_get;
    
    dnx_data_snif_egress_mirror_nof_vlan_profiles_get_f mirror_nof_vlan_profiles_get;
    
    dnx_data_snif_egress_mirror_default_profile_get_f mirror_default_profile_get;
    
    dnx_data_snif_egress_prob_max_val_get_f prob_max_val_get;
    
    dnx_data_snif_egress_prob_nof_bits_get_f prob_nof_bits_get;
    
    dnx_data_snif_egress_probability_nof_profiles_get_f probability_nof_profiles_get;
    
    dnx_data_snif_egress_crop_size_nof_profiles_get_f crop_size_nof_profiles_get;
    
    dnx_data_snif_egress_crop_size_granularity_get_f crop_size_granularity_get;
    
    dnx_data_snif_egress_rcy_mirror_to_forward_port_map_get_f rcy_mirror_to_forward_port_map_get;
    
    dnxc_data_table_info_get_f rcy_mirror_to_forward_port_map_info_get;
} dnx_data_if_snif_egress_t;





typedef struct
{
    
    dnx_data_if_snif_general_t general;
    
    dnx_data_if_snif_ingress_t ingress;
    
    dnx_data_if_snif_egress_t egress;
} dnx_data_if_snif_t;




extern dnx_data_if_snif_t dnx_data_snif;


#endif 

