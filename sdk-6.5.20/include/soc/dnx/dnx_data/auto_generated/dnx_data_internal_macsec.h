

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_MACSEC_H_

#define _DNX_DATA_INTERNAL_MACSEC_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_macsec.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_macsec_submodule_general,
    dnx_data_macsec_submodule_ingress,
    dnx_data_macsec_submodule_egress,

    
    _dnx_data_macsec_submodule_nof
} dnx_data_macsec_submodule_e;








int dnx_data_macsec_general_feature_get(
    int unit,
    dnx_data_macsec_general_feature_e feature);



typedef enum
{
    dnx_data_macsec_general_define_macsec_nof,
    dnx_data_macsec_general_define_macsec_nof_bits,
    dnx_data_macsec_general_define_ports_in_macsec_nof,
    dnx_data_macsec_general_define_ports_in_macsec_nof_bits,
    dnx_data_macsec_general_define_etype_nof,

    
    _dnx_data_macsec_general_define_nof
} dnx_data_macsec_general_define_e;



uint32 dnx_data_macsec_general_macsec_nof_get(
    int unit);


uint32 dnx_data_macsec_general_macsec_nof_bits_get(
    int unit);


uint32 dnx_data_macsec_general_ports_in_macsec_nof_get(
    int unit);


uint32 dnx_data_macsec_general_ports_in_macsec_nof_bits_get(
    int unit);


uint32 dnx_data_macsec_general_etype_nof_get(
    int unit);



typedef enum
{
    dnx_data_macsec_general_table_pm_to_macsec,
    dnx_data_macsec_general_table_macsec_instances,

    
    _dnx_data_macsec_general_table_nof
} dnx_data_macsec_general_table_e;



const dnx_data_macsec_general_pm_to_macsec_t * dnx_data_macsec_general_pm_to_macsec_get(
    int unit,
    int ethu_index);


const dnx_data_macsec_general_macsec_instances_t * dnx_data_macsec_general_macsec_instances_get(
    int unit,
    int macsec_id);



shr_error_e dnx_data_macsec_general_pm_to_macsec_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_macsec_general_macsec_instances_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_macsec_general_pm_to_macsec_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_macsec_general_macsec_instances_info_get(
    int unit);






int dnx_data_macsec_ingress_feature_get(
    int unit,
    dnx_data_macsec_ingress_feature_e feature);



typedef enum
{
    dnx_data_macsec_ingress_define_flow_nof,
    dnx_data_macsec_ingress_define_policy_nof,
    dnx_data_macsec_ingress_define_secure_channel_nof,
    dnx_data_macsec_ingress_define_secure_assoc_nof,
    dnx_data_macsec_ingress_define_udf_nof_bits,
    dnx_data_macsec_ingress_define_mgmt_rule_exact_nof,
    dnx_data_macsec_ingress_define_tpid_nof,
    dnx_data_macsec_ingress_define_cpu_flex_map_nof,
    dnx_data_macsec_ingress_define_sc_tcam_nof,
    dnx_data_macsec_ingress_define_sa_per_sc_nof,
    dnx_data_macsec_ingress_define_invalidate_sa,

    
    _dnx_data_macsec_ingress_define_nof
} dnx_data_macsec_ingress_define_e;



uint32 dnx_data_macsec_ingress_flow_nof_get(
    int unit);


uint32 dnx_data_macsec_ingress_policy_nof_get(
    int unit);


uint32 dnx_data_macsec_ingress_secure_channel_nof_get(
    int unit);


uint32 dnx_data_macsec_ingress_secure_assoc_nof_get(
    int unit);


uint32 dnx_data_macsec_ingress_udf_nof_bits_get(
    int unit);


uint32 dnx_data_macsec_ingress_mgmt_rule_exact_nof_get(
    int unit);


uint32 dnx_data_macsec_ingress_tpid_nof_get(
    int unit);


uint32 dnx_data_macsec_ingress_cpu_flex_map_nof_get(
    int unit);


uint32 dnx_data_macsec_ingress_sc_tcam_nof_get(
    int unit);


uint32 dnx_data_macsec_ingress_sa_per_sc_nof_get(
    int unit);


uint32 dnx_data_macsec_ingress_invalidate_sa_get(
    int unit);



typedef enum
{

    
    _dnx_data_macsec_ingress_table_nof
} dnx_data_macsec_ingress_table_e;









int dnx_data_macsec_egress_feature_get(
    int unit,
    dnx_data_macsec_egress_feature_e feature);



typedef enum
{
    dnx_data_macsec_egress_define_secure_channel_nof,
    dnx_data_macsec_egress_define_secure_assoc_nof,
    dnx_data_macsec_egress_define_soft_expiry_threshold_profiles_nof,
    dnx_data_macsec_egress_define_sa_per_sc_nof,

    
    _dnx_data_macsec_egress_define_nof
} dnx_data_macsec_egress_define_e;



uint32 dnx_data_macsec_egress_secure_channel_nof_get(
    int unit);


uint32 dnx_data_macsec_egress_secure_assoc_nof_get(
    int unit);


uint32 dnx_data_macsec_egress_soft_expiry_threshold_profiles_nof_get(
    int unit);


uint32 dnx_data_macsec_egress_sa_per_sc_nof_get(
    int unit);



typedef enum
{

    
    _dnx_data_macsec_egress_table_nof
} dnx_data_macsec_egress_table_e;






shr_error_e dnx_data_macsec_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

