

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_SNIF_H_

#define _DNX_DATA_SNIF_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_snif.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
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
    *dnx_data_snif_general_cud_type_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_snif_general_feature_get_f feature_get;
    
    dnx_data_snif_general_cud_type_get_f cud_type_get;
} dnx_data_if_snif_general_t;







typedef enum
{

    
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







typedef enum
{
    
    dnx_data_snif_egress_probability_sample,

    
    _dnx_data_snif_egress_feature_nof
} dnx_data_snif_egress_feature_e;



typedef int(
    *dnx_data_snif_egress_feature_get_f) (
    int unit,
    dnx_data_snif_egress_feature_e feature);


typedef uint32(
    *dnx_data_snif_egress_mirror_snoop_nof_profiles_get_f) (
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
    *dnx_data_snif_egress_crop_size_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_snif_egress_feature_get_f feature_get;
    
    dnx_data_snif_egress_mirror_snoop_nof_profiles_get_f mirror_snoop_nof_profiles_get;
    
    dnx_data_snif_egress_mirror_nof_vlan_profiles_get_f mirror_nof_vlan_profiles_get;
    
    dnx_data_snif_egress_mirror_default_profile_get_f mirror_default_profile_get;
    
    dnx_data_snif_egress_prob_max_val_get_f prob_max_val_get;
    
    dnx_data_snif_egress_crop_size_get_f crop_size_get;
} dnx_data_if_snif_egress_t;





typedef struct
{
    
    dnx_data_if_snif_general_t general;
    
    dnx_data_if_snif_ingress_t ingress;
    
    dnx_data_if_snif_egress_t egress;
} dnx_data_if_snif_t;




extern dnx_data_if_snif_t dnx_data_snif;


#endif 

