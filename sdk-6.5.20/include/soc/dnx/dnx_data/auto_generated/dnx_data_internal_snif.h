

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_SNIF_H_

#define _DNX_DATA_INTERNAL_SNIF_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_snif.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_snif_submodule_general,
    dnx_data_snif_submodule_ingress,
    dnx_data_snif_submodule_egress,

    
    _dnx_data_snif_submodule_nof
} dnx_data_snif_submodule_e;








int dnx_data_snif_general_feature_get(
    int unit,
    dnx_data_snif_general_feature_e feature);



typedef enum
{
    dnx_data_snif_general_define_cud_type,

    
    _dnx_data_snif_general_define_nof
} dnx_data_snif_general_define_e;



uint32 dnx_data_snif_general_cud_type_get(
    int unit);



typedef enum
{

    
    _dnx_data_snif_general_table_nof
} dnx_data_snif_general_table_e;









int dnx_data_snif_ingress_feature_get(
    int unit,
    dnx_data_snif_ingress_feature_e feature);



typedef enum
{
    dnx_data_snif_ingress_define_nof_profiles,
    dnx_data_snif_ingress_define_mirror_nof_profiles,
    dnx_data_snif_ingress_define_nof_mirror_on_drop_profiles,
    dnx_data_snif_ingress_define_mirror_nof_vlan_profiles,
    dnx_data_snif_ingress_define_mirror_default_tagged_profile,
    dnx_data_snif_ingress_define_mirror_untagged_profiles,
    dnx_data_snif_ingress_define_prob_max_val,
    dnx_data_snif_ingress_define_crop_size,
    dnx_data_snif_ingress_define_keep_orig_dest_on_dsp_ext,

    
    _dnx_data_snif_ingress_define_nof
} dnx_data_snif_ingress_define_e;



uint32 dnx_data_snif_ingress_nof_profiles_get(
    int unit);


uint32 dnx_data_snif_ingress_mirror_nof_profiles_get(
    int unit);


uint32 dnx_data_snif_ingress_nof_mirror_on_drop_profiles_get(
    int unit);


uint32 dnx_data_snif_ingress_mirror_nof_vlan_profiles_get(
    int unit);


uint32 dnx_data_snif_ingress_mirror_default_tagged_profile_get(
    int unit);


uint32 dnx_data_snif_ingress_mirror_untagged_profiles_get(
    int unit);


uint32 dnx_data_snif_ingress_prob_max_val_get(
    int unit);


uint32 dnx_data_snif_ingress_crop_size_get(
    int unit);


uint32 dnx_data_snif_ingress_keep_orig_dest_on_dsp_ext_get(
    int unit);



typedef enum
{

    
    _dnx_data_snif_ingress_table_nof
} dnx_data_snif_ingress_table_e;









int dnx_data_snif_egress_feature_get(
    int unit,
    dnx_data_snif_egress_feature_e feature);



typedef enum
{
    dnx_data_snif_egress_define_mirror_snoop_nof_profiles,
    dnx_data_snif_egress_define_mirror_nof_vlan_profiles,
    dnx_data_snif_egress_define_mirror_default_profile,
    dnx_data_snif_egress_define_prob_max_val,
    dnx_data_snif_egress_define_crop_size,

    
    _dnx_data_snif_egress_define_nof
} dnx_data_snif_egress_define_e;



uint32 dnx_data_snif_egress_mirror_snoop_nof_profiles_get(
    int unit);


uint32 dnx_data_snif_egress_mirror_nof_vlan_profiles_get(
    int unit);


uint32 dnx_data_snif_egress_mirror_default_profile_get(
    int unit);


uint32 dnx_data_snif_egress_prob_max_val_get(
    int unit);


uint32 dnx_data_snif_egress_crop_size_get(
    int unit);



typedef enum
{

    
    _dnx_data_snif_egress_table_nof
} dnx_data_snif_egress_table_e;






shr_error_e dnx_data_snif_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

