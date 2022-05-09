
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_LIF

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_lif.h>




extern shr_error_e jr2_a0_data_lif_attach(
    int unit);


extern shr_error_e j2c_a0_data_lif_attach(
    int unit);


extern shr_error_e q2a_a0_data_lif_attach(
    int unit);


extern shr_error_e j2p_a0_data_lif_attach(
    int unit);


extern shr_error_e j2p_a1_data_lif_attach(
    int unit);


extern shr_error_e j2x_a0_data_lif_attach(
    int unit);




static shr_error_e
dnx_data_lif_global_lif_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "global_lif";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_lif_global_lif_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data lif global_lif features");

    submodule_data->features[dnx_data_lif_global_lif_prevent_tunnel_update_rif].name = "prevent_tunnel_update_rif";
    submodule_data->features[dnx_data_lif_global_lif_prevent_tunnel_update_rif].doc = "";
    submodule_data->features[dnx_data_lif_global_lif_prevent_tunnel_update_rif].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_lif_global_lif_glem_bypass_per_phase].name = "glem_bypass_per_phase";
    submodule_data->features[dnx_data_lif_global_lif_glem_bypass_per_phase].doc = "";
    submodule_data->features[dnx_data_lif_global_lif_glem_bypass_per_phase].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_lif_global_lif_outlif_profile_in_glem].name = "outlif_profile_in_glem";
    submodule_data->features[dnx_data_lif_global_lif_outlif_profile_in_glem].doc = "";
    submodule_data->features[dnx_data_lif_global_lif_outlif_profile_in_glem].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_lif_global_lif_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data lif global_lif defines");

    submodule_data->defines[dnx_data_lif_global_lif_define_use_mdb_size].name = "use_mdb_size";
    submodule_data->defines[dnx_data_lif_global_lif_define_use_mdb_size].doc = "";
    
    submodule_data->defines[dnx_data_lif_global_lif_define_use_mdb_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_global_lif_define_glem_rif_optimization_enabled].name = "glem_rif_optimization_enabled";
    submodule_data->defines[dnx_data_lif_global_lif_define_glem_rif_optimization_enabled].doc = "";
    
    submodule_data->defines[dnx_data_lif_global_lif_define_glem_rif_optimization_enabled].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_global_lif_define_null_lif].name = "null_lif";
    submodule_data->defines[dnx_data_lif_global_lif_define_null_lif].doc = "";
    
    submodule_data->defines[dnx_data_lif_global_lif_define_null_lif].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_global_lif_define_three_intf_glem].name = "three_intf_glem";
    submodule_data->defines[dnx_data_lif_global_lif_define_three_intf_glem].doc = "";
    
    submodule_data->defines[dnx_data_lif_global_lif_define_three_intf_glem].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_global_lif_define_nof_global_in_lifs].name = "nof_global_in_lifs";
    submodule_data->defines[dnx_data_lif_global_lif_define_nof_global_in_lifs].doc = "";
    
    submodule_data->defines[dnx_data_lif_global_lif_define_nof_global_in_lifs].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_global_lif_define_nof_global_out_lifs].name = "nof_global_out_lifs";
    submodule_data->defines[dnx_data_lif_global_lif_define_nof_global_out_lifs].doc = "";
    
    submodule_data->defines[dnx_data_lif_global_lif_define_nof_global_out_lifs].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_global_lif_define_egress_in_lif_null_value].name = "egress_in_lif_null_value";
    submodule_data->defines[dnx_data_lif_global_lif_define_egress_in_lif_null_value].doc = "";
    
    submodule_data->defines[dnx_data_lif_global_lif_define_egress_in_lif_null_value].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_global_lif_define_egress_out_lif_null_value].name = "egress_out_lif_null_value";
    submodule_data->defines[dnx_data_lif_global_lif_define_egress_out_lif_null_value].doc = "";
    
    submodule_data->defines[dnx_data_lif_global_lif_define_egress_out_lif_null_value].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_global_lif_define_nof_global_l2_gport_lifs].name = "nof_global_l2_gport_lifs";
    submodule_data->defines[dnx_data_lif_global_lif_define_nof_global_l2_gport_lifs].doc = "";
    
    submodule_data->defines[dnx_data_lif_global_lif_define_nof_global_l2_gport_lifs].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_global_lif_define_global_lif_bta_max_val].name = "global_lif_bta_max_val";
    submodule_data->defines[dnx_data_lif_global_lif_define_global_lif_bta_max_val].doc = "";
    
    submodule_data->defines[dnx_data_lif_global_lif_define_global_lif_bta_max_val].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_global_lif_define_global_lif_tag_nof_bta_bits].name = "global_lif_tag_nof_bta_bits";
    submodule_data->defines[dnx_data_lif_global_lif_define_global_lif_tag_nof_bta_bits].doc = "";
    
    submodule_data->defines[dnx_data_lif_global_lif_define_global_lif_tag_nof_bta_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_global_lif_define_global_lif_intf_max_val].name = "global_lif_intf_max_val";
    submodule_data->defines[dnx_data_lif_global_lif_define_global_lif_intf_max_val].doc = "";
    
    submodule_data->defines[dnx_data_lif_global_lif_define_global_lif_intf_max_val].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_global_lif_define_global_lif_tag_nof_intf_bits].name = "global_lif_tag_nof_intf_bits";
    submodule_data->defines[dnx_data_lif_global_lif_define_global_lif_tag_nof_intf_bits].doc = "";
    
    submodule_data->defines[dnx_data_lif_global_lif_define_global_lif_tag_nof_intf_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_global_lif_define_global_lif_group_max_val].name = "global_lif_group_max_val";
    submodule_data->defines[dnx_data_lif_global_lif_define_global_lif_group_max_val].doc = "";
    
    submodule_data->defines[dnx_data_lif_global_lif_define_global_lif_group_max_val].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_global_lif_define_global_lif_tag_nof_group_bits].name = "global_lif_tag_nof_group_bits";
    submodule_data->defines[dnx_data_lif_global_lif_define_global_lif_tag_nof_group_bits].doc = "";
    
    submodule_data->defines[dnx_data_lif_global_lif_define_global_lif_tag_nof_group_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_global_lif_define_global_lif_grain_size].name = "global_lif_grain_size";
    submodule_data->defines[dnx_data_lif_global_lif_define_global_lif_grain_size].doc = "";
    
    submodule_data->defines[dnx_data_lif_global_lif_define_global_lif_grain_size].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_lif_global_lif_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data lif global_lif tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_lif_global_lif_feature_get(
    int unit,
    dnx_data_lif_global_lif_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_global_lif, feature);
}


uint32
dnx_data_lif_global_lif_use_mdb_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_global_lif, dnx_data_lif_global_lif_define_use_mdb_size);
}

uint32
dnx_data_lif_global_lif_glem_rif_optimization_enabled_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_global_lif, dnx_data_lif_global_lif_define_glem_rif_optimization_enabled);
}

uint32
dnx_data_lif_global_lif_null_lif_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_global_lif, dnx_data_lif_global_lif_define_null_lif);
}

uint32
dnx_data_lif_global_lif_three_intf_glem_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_global_lif, dnx_data_lif_global_lif_define_three_intf_glem);
}

uint32
dnx_data_lif_global_lif_nof_global_in_lifs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_global_lif, dnx_data_lif_global_lif_define_nof_global_in_lifs);
}

uint32
dnx_data_lif_global_lif_nof_global_out_lifs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_global_lif, dnx_data_lif_global_lif_define_nof_global_out_lifs);
}

uint32
dnx_data_lif_global_lif_egress_in_lif_null_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_global_lif, dnx_data_lif_global_lif_define_egress_in_lif_null_value);
}

uint32
dnx_data_lif_global_lif_egress_out_lif_null_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_global_lif, dnx_data_lif_global_lif_define_egress_out_lif_null_value);
}

uint32
dnx_data_lif_global_lif_nof_global_l2_gport_lifs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_global_lif, dnx_data_lif_global_lif_define_nof_global_l2_gport_lifs);
}

uint32
dnx_data_lif_global_lif_global_lif_bta_max_val_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_global_lif, dnx_data_lif_global_lif_define_global_lif_bta_max_val);
}

uint32
dnx_data_lif_global_lif_global_lif_tag_nof_bta_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_global_lif, dnx_data_lif_global_lif_define_global_lif_tag_nof_bta_bits);
}

uint32
dnx_data_lif_global_lif_global_lif_intf_max_val_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_global_lif, dnx_data_lif_global_lif_define_global_lif_intf_max_val);
}

uint32
dnx_data_lif_global_lif_global_lif_tag_nof_intf_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_global_lif, dnx_data_lif_global_lif_define_global_lif_tag_nof_intf_bits);
}

uint32
dnx_data_lif_global_lif_global_lif_group_max_val_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_global_lif, dnx_data_lif_global_lif_define_global_lif_group_max_val);
}

uint32
dnx_data_lif_global_lif_global_lif_tag_nof_group_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_global_lif, dnx_data_lif_global_lif_define_global_lif_tag_nof_group_bits);
}

uint32
dnx_data_lif_global_lif_global_lif_grain_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_global_lif, dnx_data_lif_global_lif_define_global_lif_grain_size);
}










static shr_error_e
dnx_data_lif_virtual_lif_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "virtual_lif";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_lif_virtual_lif_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data lif virtual_lif features");

    
    submodule_data->nof_defines = _dnx_data_lif_virtual_lif_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data lif virtual_lif defines");

    submodule_data->defines[dnx_data_lif_virtual_lif_define_flow_ingress_ids].name = "flow_ingress_ids";
    submodule_data->defines[dnx_data_lif_virtual_lif_define_flow_ingress_ids].doc = "";
    
    submodule_data->defines[dnx_data_lif_virtual_lif_define_flow_ingress_ids].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_virtual_lif_define_flow_egress_ids].name = "flow_egress_ids";
    submodule_data->defines[dnx_data_lif_virtual_lif_define_flow_egress_ids].doc = "";
    
    submodule_data->defines[dnx_data_lif_virtual_lif_define_flow_egress_ids].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_lif_virtual_lif_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data lif virtual_lif tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_lif_virtual_lif_feature_get(
    int unit,
    dnx_data_lif_virtual_lif_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_virtual_lif, feature);
}


uint32
dnx_data_lif_virtual_lif_flow_ingress_ids_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_virtual_lif, dnx_data_lif_virtual_lif_define_flow_ingress_ids);
}

uint32
dnx_data_lif_virtual_lif_flow_egress_ids_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_virtual_lif, dnx_data_lif_virtual_lif_define_flow_egress_ids);
}










static shr_error_e
dnx_data_lif_in_lif_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "in_lif";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_lif_in_lif_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data lif in_lif features");

    submodule_data->features[dnx_data_lif_in_lif_mapped_in_lif_profile].name = "mapped_in_lif_profile";
    submodule_data->features[dnx_data_lif_in_lif_mapped_in_lif_profile].doc = "";
    submodule_data->features[dnx_data_lif_in_lif_mapped_in_lif_profile].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_lif_in_lif_in_lif_profile_iop_mode].name = "in_lif_profile_iop_mode";
    submodule_data->features[dnx_data_lif_in_lif_in_lif_profile_iop_mode].doc = "";
    submodule_data->features[dnx_data_lif_in_lif_in_lif_profile_iop_mode].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_lif_in_lif_phy_db_dpc].name = "phy_db_dpc";
    submodule_data->features[dnx_data_lif_in_lif_phy_db_dpc].doc = "";
    submodule_data->features[dnx_data_lif_in_lif_phy_db_dpc].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_lif_in_lif_cs_in_lif_profile_based_on_stages].name = "cs_in_lif_profile_based_on_stages";
    submodule_data->features[dnx_data_lif_in_lif_cs_in_lif_profile_based_on_stages].doc = "";
    submodule_data->features[dnx_data_lif_in_lif_cs_in_lif_profile_based_on_stages].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_lif_in_lif_p2p_or_mp_based_on_cs_in_lif_profile].name = "p2p_or_mp_based_on_cs_in_lif_profile";
    submodule_data->features[dnx_data_lif_in_lif_p2p_or_mp_based_on_cs_in_lif_profile].doc = "";
    submodule_data->features[dnx_data_lif_in_lif_p2p_or_mp_based_on_cs_in_lif_profile].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_lif_in_lif_use_mapped_in_lif_profile].name = "use_mapped_in_lif_profile";
    submodule_data->features[dnx_data_lif_in_lif_use_mapped_in_lif_profile].doc = "";
    submodule_data->features[dnx_data_lif_in_lif_use_mapped_in_lif_profile].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_lif_in_lif_use_in_lif_profile_range_allocation_per_pmf_profiles].name = "use_in_lif_profile_range_allocation_per_pmf_profiles";
    submodule_data->features[dnx_data_lif_in_lif_use_in_lif_profile_range_allocation_per_pmf_profiles].doc = "";
    submodule_data->features[dnx_data_lif_in_lif_use_in_lif_profile_range_allocation_per_pmf_profiles].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_lif_in_lif_use_eth_rif_profile_range_allocation_per_pmf_profiles].name = "use_eth_rif_profile_range_allocation_per_pmf_profiles";
    submodule_data->features[dnx_data_lif_in_lif_use_eth_rif_profile_range_allocation_per_pmf_profiles].doc = "";
    submodule_data->features[dnx_data_lif_in_lif_use_eth_rif_profile_range_allocation_per_pmf_profiles].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_lif_in_lif_use_mapped_in_lif_profile_field_mapping_table].name = "use_mapped_in_lif_profile_field_mapping_table";
    submodule_data->features[dnx_data_lif_in_lif_use_mapped_in_lif_profile_field_mapping_table].doc = "";
    submodule_data->features[dnx_data_lif_in_lif_use_mapped_in_lif_profile_field_mapping_table].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_lif_in_lif_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data lif in_lif defines");

    submodule_data->defines[dnx_data_lif_in_lif_define_inlif_index_bit_size].name = "inlif_index_bit_size";
    submodule_data->defines[dnx_data_lif_in_lif_define_inlif_index_bit_size].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_inlif_index_bit_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_in_lif_define_inlif_minimum_index_size].name = "inlif_minimum_index_size";
    submodule_data->defines[dnx_data_lif_in_lif_define_inlif_minimum_index_size].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_inlif_minimum_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_in_lif_define_inlif_resource_tag_size].name = "inlif_resource_tag_size";
    submodule_data->defines[dnx_data_lif_in_lif_define_inlif_resource_tag_size].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_inlif_resource_tag_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_in_lif_define_nof_in_lif_profiles].name = "nof_in_lif_profiles";
    submodule_data->defines[dnx_data_lif_in_lif_define_nof_in_lif_profiles].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_nof_in_lif_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_in_lif_define_nof_used_in_lif_profile_bits].name = "nof_used_in_lif_profile_bits";
    submodule_data->defines[dnx_data_lif_in_lif_define_nof_used_in_lif_profile_bits].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_nof_used_in_lif_profile_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_in_lif_define_nof_used_eth_rif_profile_bits].name = "nof_used_eth_rif_profile_bits";
    submodule_data->defines[dnx_data_lif_in_lif_define_nof_used_eth_rif_profile_bits].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_nof_used_eth_rif_profile_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_in_lif_define_nof_in_lif_incoming_orientation].name = "nof_in_lif_incoming_orientation";
    submodule_data->defines[dnx_data_lif_in_lif_define_nof_in_lif_incoming_orientation].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_nof_in_lif_incoming_orientation].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_in_lif_define_nof_eth_rif_profiles].name = "nof_eth_rif_profiles";
    submodule_data->defines[dnx_data_lif_in_lif_define_nof_eth_rif_profiles].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_nof_eth_rif_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_in_lif_define_nof_da_not_found_destination_profiles].name = "nof_da_not_found_destination_profiles";
    submodule_data->defines[dnx_data_lif_in_lif_define_nof_da_not_found_destination_profiles].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_nof_da_not_found_destination_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_in_lif_define_default_da_not_found_destination_profile].name = "default_da_not_found_destination_profile";
    submodule_data->defines[dnx_data_lif_in_lif_define_default_da_not_found_destination_profile].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_default_da_not_found_destination_profile].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_in_lif_define_drop_in_lif].name = "drop_in_lif";
    submodule_data->defines[dnx_data_lif_in_lif_define_drop_in_lif].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_drop_in_lif].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_in_lif_define_mc_bridge_fallback_per_lif_profile].name = "mc_bridge_fallback_per_lif_profile";
    submodule_data->defines[dnx_data_lif_in_lif_define_mc_bridge_fallback_per_lif_profile].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_mc_bridge_fallback_per_lif_profile].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_in_lif_define_max_number_of_orientation_bits_iop_mode].name = "max_number_of_orientation_bits_iop_mode";
    submodule_data->defines[dnx_data_lif_in_lif_define_max_number_of_orientation_bits_iop_mode].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_max_number_of_orientation_bits_iop_mode].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_in_lif_define_max_number_of_egress_lif_profile_bits_iop_mode].name = "max_number_of_egress_lif_profile_bits_iop_mode";
    submodule_data->defines[dnx_data_lif_in_lif_define_max_number_of_egress_lif_profile_bits_iop_mode].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_max_number_of_egress_lif_profile_bits_iop_mode].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_in_lif_define_algo_flow_inlif_bank_size].name = "algo_flow_inlif_bank_size";
    submodule_data->defines[dnx_data_lif_in_lif_define_algo_flow_inlif_bank_size].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_algo_flow_inlif_bank_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_in_lif_define_nof_routing_enable_profiles].name = "nof_routing_enable_profiles";
    submodule_data->defines[dnx_data_lif_in_lif_define_nof_routing_enable_profiles].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_nof_routing_enable_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_in_lif_define_nof_shift_bits].name = "nof_shift_bits";
    submodule_data->defines[dnx_data_lif_in_lif_define_nof_shift_bits].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_nof_shift_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_in_lif_define_in_lif_profile_pmf_reserved_start_bit].name = "in_lif_profile_pmf_reserved_start_bit";
    submodule_data->defines[dnx_data_lif_in_lif_define_in_lif_profile_pmf_reserved_start_bit].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_in_lif_profile_pmf_reserved_start_bit].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_in_lif_define_eth_rif_profile_pmf_reserved_start_bit].name = "eth_rif_profile_pmf_reserved_start_bit";
    submodule_data->defines[dnx_data_lif_in_lif_define_eth_rif_profile_pmf_reserved_start_bit].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_eth_rif_profile_pmf_reserved_start_bit].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_in_lif_define_in_lif_profile_egress_reserved_start_bit].name = "in_lif_profile_egress_reserved_start_bit";
    submodule_data->defines[dnx_data_lif_in_lif_define_in_lif_profile_egress_reserved_start_bit].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_in_lif_profile_egress_reserved_start_bit].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_in_lif_define_in_lif_profile_allocate_indexed_mode].name = "in_lif_profile_allocate_indexed_mode";
    submodule_data->defines[dnx_data_lif_in_lif_define_in_lif_profile_allocate_indexed_mode].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_in_lif_profile_allocate_indexed_mode].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_in_lif_define_nof_in_lif_profile_allocate_orientation_bits].name = "nof_in_lif_profile_allocate_orientation_bits";
    submodule_data->defines[dnx_data_lif_in_lif_define_nof_in_lif_profile_allocate_orientation_bits].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_nof_in_lif_profile_allocate_orientation_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_in_lif_define_nof_in_lif_profile_allocate_policer_inner_dp_bits].name = "nof_in_lif_profile_allocate_policer_inner_dp_bits";
    submodule_data->defines[dnx_data_lif_in_lif_define_nof_in_lif_profile_allocate_policer_inner_dp_bits].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_nof_in_lif_profile_allocate_policer_inner_dp_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_in_lif_define_nof_in_lif_profile_allocate_policer_outer_dp_bits].name = "nof_in_lif_profile_allocate_policer_outer_dp_bits";
    submodule_data->defines[dnx_data_lif_in_lif_define_nof_in_lif_profile_allocate_policer_outer_dp_bits].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_nof_in_lif_profile_allocate_policer_outer_dp_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_in_lif_define_nof_in_lif_profile_allocate_same_interface_mode_bits].name = "nof_in_lif_profile_allocate_same_interface_mode_bits";
    submodule_data->defines[dnx_data_lif_in_lif_define_nof_in_lif_profile_allocate_same_interface_mode_bits].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_nof_in_lif_profile_allocate_same_interface_mode_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_in_lif_define_nof_in_lif_egress_reserved_bits].name = "nof_in_lif_egress_reserved_bits";
    submodule_data->defines[dnx_data_lif_in_lif_define_nof_in_lif_egress_reserved_bits].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_nof_in_lif_egress_reserved_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_in_lif_define_eth_rif_profile_egress_reserved_nof_bits].name = "eth_rif_profile_egress_reserved_nof_bits";
    submodule_data->defines[dnx_data_lif_in_lif_define_eth_rif_profile_egress_reserved_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_eth_rif_profile_egress_reserved_nof_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_in_lif_define_eth_rif_profile_egress_reserved_start_bit].name = "eth_rif_profile_egress_reserved_start_bit";
    submodule_data->defines[dnx_data_lif_in_lif_define_eth_rif_profile_egress_reserved_start_bit].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_eth_rif_profile_egress_reserved_start_bit].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_in_lif_define_in_lif_profile_native_indexed_mode_start_bit].name = "in_lif_profile_native_indexed_mode_start_bit";
    submodule_data->defines[dnx_data_lif_in_lif_define_in_lif_profile_native_indexed_mode_start_bit].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_in_lif_profile_native_indexed_mode_start_bit].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_in_lif_define_nof_in_lif_profile_allocate_indexed_mode_bits].name = "nof_in_lif_profile_allocate_indexed_mode_bits";
    submodule_data->defines[dnx_data_lif_in_lif_define_nof_in_lif_profile_allocate_indexed_mode_bits].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_nof_in_lif_profile_allocate_indexed_mode_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_in_lif_define_in_lif_profile_allocate_orientation].name = "in_lif_profile_allocate_orientation";
    submodule_data->defines[dnx_data_lif_in_lif_define_in_lif_profile_allocate_orientation].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_in_lif_profile_allocate_orientation].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_in_lif_define_in_lif_profile_allocate_policer_inner_dp].name = "in_lif_profile_allocate_policer_inner_dp";
    submodule_data->defines[dnx_data_lif_in_lif_define_in_lif_profile_allocate_policer_inner_dp].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_in_lif_profile_allocate_policer_inner_dp].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_in_lif_define_in_lif_profile_allocate_policer_outer_dp].name = "in_lif_profile_allocate_policer_outer_dp";
    submodule_data->defines[dnx_data_lif_in_lif_define_in_lif_profile_allocate_policer_outer_dp].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_in_lif_profile_allocate_policer_outer_dp].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_in_lif_define_in_lif_profile_allocate_same_interface_mode].name = "in_lif_profile_allocate_same_interface_mode";
    submodule_data->defines[dnx_data_lif_in_lif_define_in_lif_profile_allocate_same_interface_mode].doc = "";
    
    submodule_data->defines[dnx_data_lif_in_lif_define_in_lif_profile_allocate_same_interface_mode].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_lif_in_lif_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data lif in_lif tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_lif_in_lif_feature_get(
    int unit,
    dnx_data_lif_in_lif_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, feature);
}


uint32
dnx_data_lif_in_lif_inlif_index_bit_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_inlif_index_bit_size);
}

uint32
dnx_data_lif_in_lif_inlif_minimum_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_inlif_minimum_index_size);
}

uint32
dnx_data_lif_in_lif_inlif_resource_tag_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_inlif_resource_tag_size);
}

uint32
dnx_data_lif_in_lif_nof_in_lif_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_nof_in_lif_profiles);
}

uint32
dnx_data_lif_in_lif_nof_used_in_lif_profile_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_nof_used_in_lif_profile_bits);
}

uint32
dnx_data_lif_in_lif_nof_used_eth_rif_profile_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_nof_used_eth_rif_profile_bits);
}

uint32
dnx_data_lif_in_lif_nof_in_lif_incoming_orientation_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_nof_in_lif_incoming_orientation);
}

uint32
dnx_data_lif_in_lif_nof_eth_rif_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_nof_eth_rif_profiles);
}

uint32
dnx_data_lif_in_lif_nof_da_not_found_destination_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_nof_da_not_found_destination_profiles);
}

uint32
dnx_data_lif_in_lif_default_da_not_found_destination_profile_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_default_da_not_found_destination_profile);
}

uint32
dnx_data_lif_in_lif_drop_in_lif_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_drop_in_lif);
}

uint32
dnx_data_lif_in_lif_mc_bridge_fallback_per_lif_profile_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_mc_bridge_fallback_per_lif_profile);
}

uint32
dnx_data_lif_in_lif_max_number_of_orientation_bits_iop_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_max_number_of_orientation_bits_iop_mode);
}

uint32
dnx_data_lif_in_lif_max_number_of_egress_lif_profile_bits_iop_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_max_number_of_egress_lif_profile_bits_iop_mode);
}

uint32
dnx_data_lif_in_lif_algo_flow_inlif_bank_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_algo_flow_inlif_bank_size);
}

uint32
dnx_data_lif_in_lif_nof_routing_enable_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_nof_routing_enable_profiles);
}

uint32
dnx_data_lif_in_lif_nof_shift_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_nof_shift_bits);
}

uint32
dnx_data_lif_in_lif_in_lif_profile_pmf_reserved_start_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_in_lif_profile_pmf_reserved_start_bit);
}

uint32
dnx_data_lif_in_lif_eth_rif_profile_pmf_reserved_start_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_eth_rif_profile_pmf_reserved_start_bit);
}

uint32
dnx_data_lif_in_lif_in_lif_profile_egress_reserved_start_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_in_lif_profile_egress_reserved_start_bit);
}

uint32
dnx_data_lif_in_lif_in_lif_profile_allocate_indexed_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_in_lif_profile_allocate_indexed_mode);
}

uint32
dnx_data_lif_in_lif_nof_in_lif_profile_allocate_orientation_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_nof_in_lif_profile_allocate_orientation_bits);
}

uint32
dnx_data_lif_in_lif_nof_in_lif_profile_allocate_policer_inner_dp_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_nof_in_lif_profile_allocate_policer_inner_dp_bits);
}

uint32
dnx_data_lif_in_lif_nof_in_lif_profile_allocate_policer_outer_dp_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_nof_in_lif_profile_allocate_policer_outer_dp_bits);
}

uint32
dnx_data_lif_in_lif_nof_in_lif_profile_allocate_same_interface_mode_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_nof_in_lif_profile_allocate_same_interface_mode_bits);
}

uint32
dnx_data_lif_in_lif_nof_in_lif_egress_reserved_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_nof_in_lif_egress_reserved_bits);
}

uint32
dnx_data_lif_in_lif_eth_rif_profile_egress_reserved_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_eth_rif_profile_egress_reserved_nof_bits);
}

uint32
dnx_data_lif_in_lif_eth_rif_profile_egress_reserved_start_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_eth_rif_profile_egress_reserved_start_bit);
}

uint32
dnx_data_lif_in_lif_in_lif_profile_native_indexed_mode_start_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_in_lif_profile_native_indexed_mode_start_bit);
}

uint32
dnx_data_lif_in_lif_nof_in_lif_profile_allocate_indexed_mode_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_nof_in_lif_profile_allocate_indexed_mode_bits);
}

uint32
dnx_data_lif_in_lif_in_lif_profile_allocate_orientation_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_in_lif_profile_allocate_orientation);
}

uint32
dnx_data_lif_in_lif_in_lif_profile_allocate_policer_inner_dp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_in_lif_profile_allocate_policer_inner_dp);
}

uint32
dnx_data_lif_in_lif_in_lif_profile_allocate_policer_outer_dp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_in_lif_profile_allocate_policer_outer_dp);
}

uint32
dnx_data_lif_in_lif_in_lif_profile_allocate_same_interface_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_in_lif, dnx_data_lif_in_lif_define_in_lif_profile_allocate_same_interface_mode);
}










static shr_error_e
dnx_data_lif_out_lif_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "out_lif";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_lif_out_lif_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data lif out_lif features");

    submodule_data->features[dnx_data_lif_out_lif_per_phase_algo].name = "per_phase_algo";
    submodule_data->features[dnx_data_lif_out_lif_per_phase_algo].doc = "";
    submodule_data->features[dnx_data_lif_out_lif_per_phase_algo].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_lif_out_lif_data_entries_supported].name = "data_entries_supported";
    submodule_data->features[dnx_data_lif_out_lif_data_entries_supported].doc = "";
    submodule_data->features[dnx_data_lif_out_lif_data_entries_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_lif_out_lif_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data lif out_lif defines");

    submodule_data->defines[dnx_data_lif_out_lif_define_nof_out_lif_profiles].name = "nof_out_lif_profiles";
    submodule_data->defines[dnx_data_lif_out_lif_define_nof_out_lif_profiles].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_nof_out_lif_profiles].flags |= DNXC_DATA_F_DEFINE;


    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_lif_out_lif_define_nof_erpp_out_lif_profiles].name = "nof_erpp_out_lif_profiles";
        submodule_data->defines[dnx_data_lif_out_lif_define_nof_erpp_out_lif_profiles].doc = "";
        
        submodule_data->defines[dnx_data_lif_out_lif_define_nof_erpp_out_lif_profiles].flags |= DNXC_DATA_F_DEFINE;
    }

    submodule_data->defines[dnx_data_lif_out_lif_define_nof_out_lif_outgoing_orientation].name = "nof_out_lif_outgoing_orientation";
    submodule_data->defines[dnx_data_lif_out_lif_define_nof_out_lif_outgoing_orientation].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_nof_out_lif_outgoing_orientation].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_out_lif_define_outrif_profile_width].name = "outrif_profile_width";
    submodule_data->defines[dnx_data_lif_out_lif_define_outrif_profile_width].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_outrif_profile_width].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_out_lif_define_glem_result].name = "glem_result";
    submodule_data->defines[dnx_data_lif_out_lif_define_glem_result].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_glem_result].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_out_lif_define_physical_bank_pointer_size].name = "physical_bank_pointer_size";
    submodule_data->defines[dnx_data_lif_out_lif_define_physical_bank_pointer_size].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_physical_bank_pointer_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_out_lif_define_outlif_eedb_banks_pointer_size].name = "outlif_eedb_banks_pointer_size";
    submodule_data->defines[dnx_data_lif_out_lif_define_outlif_eedb_banks_pointer_size].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_outlif_eedb_banks_pointer_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_out_lif_define_global_outlif_bta_sop_resolution].name = "global_outlif_bta_sop_resolution";
    submodule_data->defines[dnx_data_lif_out_lif_define_global_outlif_bta_sop_resolution].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_global_outlif_bta_sop_resolution].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_out_lif_define_pmf_outlif_profile_width].name = "pmf_outlif_profile_width";
    submodule_data->defines[dnx_data_lif_out_lif_define_pmf_outlif_profile_width].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_pmf_outlif_profile_width].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_out_lif_define_pmf_outrif_profile_width].name = "pmf_outrif_profile_width";
    submodule_data->defines[dnx_data_lif_out_lif_define_pmf_outrif_profile_width].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_pmf_outrif_profile_width].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_out_lif_define_data_entries_size].name = "data_entries_size";
    submodule_data->defines[dnx_data_lif_out_lif_define_data_entries_size].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_data_entries_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_out_lif_define_etpp_out_lif_profile_default_mpls_profile].name = "etpp_out_lif_profile_default_mpls_profile";
    submodule_data->defines[dnx_data_lif_out_lif_define_etpp_out_lif_profile_default_mpls_profile].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_etpp_out_lif_profile_default_mpls_profile].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_out_lif_define_etpp_out_lif_profile_default_port_profile].name = "etpp_out_lif_profile_default_port_profile";
    submodule_data->defines[dnx_data_lif_out_lif_define_etpp_out_lif_profile_default_port_profile].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_etpp_out_lif_profile_default_port_profile].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_out_lif_define_etpp_out_lif_profile_default_orientation_profile].name = "etpp_out_lif_profile_default_orientation_profile";
    submodule_data->defines[dnx_data_lif_out_lif_define_etpp_out_lif_profile_default_orientation_profile].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_etpp_out_lif_profile_default_orientation_profile].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_out_lif_define_outlif_pointer_size].name = "outlif_pointer_size";
    submodule_data->defines[dnx_data_lif_out_lif_define_outlif_pointer_size].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_outlif_pointer_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_out_lif_define_allocation_bank_size].name = "allocation_bank_size";
    submodule_data->defines[dnx_data_lif_out_lif_define_allocation_bank_size].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_allocation_bank_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_out_lif_define_local_outlif_width].name = "local_outlif_width";
    submodule_data->defines[dnx_data_lif_out_lif_define_local_outlif_width].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_local_outlif_width].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_out_lif_define_outlif_profile_width].name = "outlif_profile_width";
    submodule_data->defines[dnx_data_lif_out_lif_define_outlif_profile_width].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_outlif_profile_width].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_out_lif_define_nof_local_out_lifs].name = "nof_local_out_lifs";
    submodule_data->defines[dnx_data_lif_out_lif_define_nof_local_out_lifs].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_nof_local_out_lifs].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_out_lif_define_physical_bank_size].name = "physical_bank_size";
    submodule_data->defines[dnx_data_lif_out_lif_define_physical_bank_size].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_physical_bank_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_out_lif_define_global_ipv6_outlif_bta_sop].name = "global_ipv6_outlif_bta_sop";
    submodule_data->defines[dnx_data_lif_out_lif_define_global_ipv6_outlif_bta_sop].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_global_ipv6_outlif_bta_sop].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_out_lif_define_global_vxlan_outlif_bta_sop].name = "global_vxlan_outlif_bta_sop";
    submodule_data->defines[dnx_data_lif_out_lif_define_global_vxlan_outlif_bta_sop].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_global_vxlan_outlif_bta_sop].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_out_lif_define_global_mpls_outlif_bta_sop].name = "global_mpls_outlif_bta_sop";
    submodule_data->defines[dnx_data_lif_out_lif_define_global_mpls_outlif_bta_sop].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_global_mpls_outlif_bta_sop].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_out_lif_define_global_srv6_outlif_bta_sop].name = "global_srv6_outlif_bta_sop";
    submodule_data->defines[dnx_data_lif_out_lif_define_global_srv6_outlif_bta_sop].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_global_srv6_outlif_bta_sop].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_out_lif_define_global_sflow_extended_gateway_two_outlif_bta_sop].name = "global_sflow_extended_gateway_two_outlif_bta_sop";
    submodule_data->defines[dnx_data_lif_out_lif_define_global_sflow_extended_gateway_two_outlif_bta_sop].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_global_sflow_extended_gateway_two_outlif_bta_sop].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_out_lif_define_global_sflow_extended_gateway_one_outlif_bta_sop].name = "global_sflow_extended_gateway_one_outlif_bta_sop";
    submodule_data->defines[dnx_data_lif_out_lif_define_global_sflow_extended_gateway_one_outlif_bta_sop].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_global_sflow_extended_gateway_one_outlif_bta_sop].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_out_lif_define_global_sflow_header_sampling_outlif_bta_sop].name = "global_sflow_header_sampling_outlif_bta_sop";
    submodule_data->defines[dnx_data_lif_out_lif_define_global_sflow_header_sampling_outlif_bta_sop].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_global_sflow_header_sampling_outlif_bta_sop].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_out_lif_define_global_sflow_dp_outlif_bta_sop].name = "global_sflow_dp_outlif_bta_sop";
    submodule_data->defines[dnx_data_lif_out_lif_define_global_sflow_dp_outlif_bta_sop].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_global_sflow_dp_outlif_bta_sop].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_out_lif_define_global_gtp_concat_ip_outlif_bta_sop].name = "global_gtp_concat_ip_outlif_bta_sop";
    submodule_data->defines[dnx_data_lif_out_lif_define_global_gtp_concat_ip_outlif_bta_sop].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_global_gtp_concat_ip_outlif_bta_sop].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_out_lif_define_global_bier_outlif_bta_sop].name = "global_bier_outlif_bta_sop";
    submodule_data->defines[dnx_data_lif_out_lif_define_global_bier_outlif_bta_sop].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_global_bier_outlif_bta_sop].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_out_lif_define_global_lif_allocation_estimated_encap_size_enable].name = "global_lif_allocation_estimated_encap_size_enable";
    submodule_data->defines[dnx_data_lif_out_lif_define_global_lif_allocation_estimated_encap_size_enable].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_global_lif_allocation_estimated_encap_size_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_out_lif_define_global_lif_allocation_outlif_intf_enable].name = "global_lif_allocation_outlif_intf_enable";
    submodule_data->defines[dnx_data_lif_out_lif_define_global_lif_allocation_outlif_intf_enable].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_global_lif_allocation_outlif_intf_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_lif_out_lif_define_global_lif_allocation_outlif_group_enable].name = "global_lif_allocation_outlif_group_enable";
    submodule_data->defines[dnx_data_lif_out_lif_define_global_lif_allocation_outlif_group_enable].doc = "";
    
    submodule_data->defines[dnx_data_lif_out_lif_define_global_lif_allocation_outlif_group_enable].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_lif_out_lif_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data lif out_lif tables");

    
    submodule_data->tables[dnx_data_lif_out_lif_table_logical_to_physical_phase_map].name = "logical_to_physical_phase_map";
    submodule_data->tables[dnx_data_lif_out_lif_table_logical_to_physical_phase_map].doc = "";
    submodule_data->tables[dnx_data_lif_out_lif_table_logical_to_physical_phase_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_lif_out_lif_table_logical_to_physical_phase_map].size_of_values = sizeof(dnx_data_lif_out_lif_logical_to_physical_phase_map_t);
    submodule_data->tables[dnx_data_lif_out_lif_table_logical_to_physical_phase_map].entry_get = dnx_data_lif_out_lif_logical_to_physical_phase_map_entry_str_get;

    
    submodule_data->tables[dnx_data_lif_out_lif_table_logical_to_physical_phase_map].nof_keys = 1;
    submodule_data->tables[dnx_data_lif_out_lif_table_logical_to_physical_phase_map].keys[0].name = "logical_phase";
    submodule_data->tables[dnx_data_lif_out_lif_table_logical_to_physical_phase_map].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_lif_out_lif_table_logical_to_physical_phase_map].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_lif_out_lif_table_logical_to_physical_phase_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_lif_out_lif_table_logical_to_physical_phase_map].nof_values, "_dnx_data_lif_out_lif_table_logical_to_physical_phase_map table values");
    submodule_data->tables[dnx_data_lif_out_lif_table_logical_to_physical_phase_map].values[0].name = "physical_phase";
    submodule_data->tables[dnx_data_lif_out_lif_table_logical_to_physical_phase_map].values[0].type = "int";
    submodule_data->tables[dnx_data_lif_out_lif_table_logical_to_physical_phase_map].values[0].doc = "Mapped physical phase.";
    submodule_data->tables[dnx_data_lif_out_lif_table_logical_to_physical_phase_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_lif_out_lif_logical_to_physical_phase_map_t, physical_phase);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_lif_out_lif_feature_get(
    int unit,
    dnx_data_lif_out_lif_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, feature);
}


uint32
dnx_data_lif_out_lif_nof_out_lif_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_nof_out_lif_profiles);
}


uint32
dnx_data_lif_out_lif_nof_erpp_out_lif_profiles_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "nof_erpp_out_lif_profiles", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_nof_erpp_out_lif_profiles);
}


uint32
dnx_data_lif_out_lif_nof_out_lif_outgoing_orientation_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_nof_out_lif_outgoing_orientation);
}

uint32
dnx_data_lif_out_lif_outrif_profile_width_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_outrif_profile_width);
}

uint32
dnx_data_lif_out_lif_glem_result_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_glem_result);
}

uint32
dnx_data_lif_out_lif_physical_bank_pointer_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_physical_bank_pointer_size);
}

uint32
dnx_data_lif_out_lif_outlif_eedb_banks_pointer_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_outlif_eedb_banks_pointer_size);
}

uint32
dnx_data_lif_out_lif_global_outlif_bta_sop_resolution_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_global_outlif_bta_sop_resolution);
}

uint32
dnx_data_lif_out_lif_pmf_outlif_profile_width_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_pmf_outlif_profile_width);
}

uint32
dnx_data_lif_out_lif_pmf_outrif_profile_width_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_pmf_outrif_profile_width);
}

uint32
dnx_data_lif_out_lif_data_entries_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_data_entries_size);
}

uint32
dnx_data_lif_out_lif_etpp_out_lif_profile_default_mpls_profile_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_etpp_out_lif_profile_default_mpls_profile);
}

uint32
dnx_data_lif_out_lif_etpp_out_lif_profile_default_port_profile_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_etpp_out_lif_profile_default_port_profile);
}

uint32
dnx_data_lif_out_lif_etpp_out_lif_profile_default_orientation_profile_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_etpp_out_lif_profile_default_orientation_profile);
}

uint32
dnx_data_lif_out_lif_outlif_pointer_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_outlif_pointer_size);
}

uint32
dnx_data_lif_out_lif_allocation_bank_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_allocation_bank_size);
}

uint32
dnx_data_lif_out_lif_local_outlif_width_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_local_outlif_width);
}

uint32
dnx_data_lif_out_lif_outlif_profile_width_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_outlif_profile_width);
}

uint32
dnx_data_lif_out_lif_nof_local_out_lifs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_nof_local_out_lifs);
}

uint32
dnx_data_lif_out_lif_physical_bank_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_physical_bank_size);
}

uint32
dnx_data_lif_out_lif_global_ipv6_outlif_bta_sop_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_global_ipv6_outlif_bta_sop);
}

uint32
dnx_data_lif_out_lif_global_vxlan_outlif_bta_sop_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_global_vxlan_outlif_bta_sop);
}

uint32
dnx_data_lif_out_lif_global_mpls_outlif_bta_sop_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_global_mpls_outlif_bta_sop);
}

uint32
dnx_data_lif_out_lif_global_srv6_outlif_bta_sop_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_global_srv6_outlif_bta_sop);
}

uint32
dnx_data_lif_out_lif_global_sflow_extended_gateway_two_outlif_bta_sop_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_global_sflow_extended_gateway_two_outlif_bta_sop);
}

uint32
dnx_data_lif_out_lif_global_sflow_extended_gateway_one_outlif_bta_sop_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_global_sflow_extended_gateway_one_outlif_bta_sop);
}

uint32
dnx_data_lif_out_lif_global_sflow_header_sampling_outlif_bta_sop_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_global_sflow_header_sampling_outlif_bta_sop);
}

uint32
dnx_data_lif_out_lif_global_sflow_dp_outlif_bta_sop_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_global_sflow_dp_outlif_bta_sop);
}

uint32
dnx_data_lif_out_lif_global_gtp_concat_ip_outlif_bta_sop_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_global_gtp_concat_ip_outlif_bta_sop);
}

uint32
dnx_data_lif_out_lif_global_bier_outlif_bta_sop_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_global_bier_outlif_bta_sop);
}

uint32
dnx_data_lif_out_lif_global_lif_allocation_estimated_encap_size_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_global_lif_allocation_estimated_encap_size_enable);
}

uint32
dnx_data_lif_out_lif_global_lif_allocation_outlif_intf_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_global_lif_allocation_outlif_intf_enable);
}

uint32
dnx_data_lif_out_lif_global_lif_allocation_outlif_group_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_define_global_lif_allocation_outlif_group_enable);
}



const dnx_data_lif_out_lif_logical_to_physical_phase_map_t *
dnx_data_lif_out_lif_logical_to_physical_phase_map_get(
    int unit,
    int logical_phase)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_table_logical_to_physical_phase_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, logical_phase, 0);
    return (const dnx_data_lif_out_lif_logical_to_physical_phase_map_t *) data;

}


shr_error_e
dnx_data_lif_out_lif_logical_to_physical_phase_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_lif_out_lif_logical_to_physical_phase_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_table_logical_to_physical_phase_map);
    data = (const dnx_data_lif_out_lif_logical_to_physical_phase_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->physical_phase);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_lif_out_lif_logical_to_physical_phase_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_out_lif, dnx_data_lif_out_lif_table_logical_to_physical_phase_map);

}






static shr_error_e
dnx_data_lif_lif_table_manager_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "lif_table_manager";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_lif_lif_table_manager_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data lif lif_table_manager features");

    submodule_data->features[dnx_data_lif_lif_table_manager_skip_in_lif_example_table].name = "skip_in_lif_example_table";
    submodule_data->features[dnx_data_lif_lif_table_manager_skip_in_lif_example_table].doc = "";
    submodule_data->features[dnx_data_lif_lif_table_manager_skip_in_lif_example_table].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_lif_lif_table_manager_use_per_field_attributes].name = "use_per_field_attributes";
    submodule_data->features[dnx_data_lif_lif_table_manager_use_per_field_attributes].doc = "";
    submodule_data->features[dnx_data_lif_lif_table_manager_use_per_field_attributes].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_lif_lif_table_manager_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data lif lif_table_manager defines");

    submodule_data->defines[dnx_data_lif_lif_table_manager_define_max_nof_lif_tables].name = "max_nof_lif_tables";
    submodule_data->defines[dnx_data_lif_lif_table_manager_define_max_nof_lif_tables].doc = "";
    
    submodule_data->defines[dnx_data_lif_lif_table_manager_define_max_nof_lif_tables].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_lif_table_manager_define_max_nof_result_types].name = "max_nof_result_types";
    submodule_data->defines[dnx_data_lif_lif_table_manager_define_max_nof_result_types].doc = "";
    
    submodule_data->defines[dnx_data_lif_lif_table_manager_define_max_nof_result_types].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_lif_table_manager_define_max_fields_per_result_type].name = "max_fields_per_result_type";
    submodule_data->defines[dnx_data_lif_lif_table_manager_define_max_fields_per_result_type].doc = "";
    
    submodule_data->defines[dnx_data_lif_lif_table_manager_define_max_fields_per_result_type].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_lif_lif_table_manager_define_max_fields_per_table].name = "max_fields_per_table";
    submodule_data->defines[dnx_data_lif_lif_table_manager_define_max_fields_per_table].doc = "";
    
    submodule_data->defines[dnx_data_lif_lif_table_manager_define_max_fields_per_table].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_lif_lif_table_manager_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data lif lif_table_manager tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_lif_lif_table_manager_feature_get(
    int unit,
    dnx_data_lif_lif_table_manager_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_lif_table_manager, feature);
}


uint32
dnx_data_lif_lif_table_manager_max_nof_lif_tables_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_lif_table_manager, dnx_data_lif_lif_table_manager_define_max_nof_lif_tables);
}

uint32
dnx_data_lif_lif_table_manager_max_nof_result_types_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_lif_table_manager, dnx_data_lif_lif_table_manager_define_max_nof_result_types);
}

uint32
dnx_data_lif_lif_table_manager_max_fields_per_result_type_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_lif_table_manager, dnx_data_lif_lif_table_manager_define_max_fields_per_result_type);
}

uint32
dnx_data_lif_lif_table_manager_max_fields_per_table_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_lif_table_manager, dnx_data_lif_lif_table_manager_define_max_fields_per_table);
}










static shr_error_e
dnx_data_lif_feature_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "feature";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_lif_feature_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data lif feature features");

    submodule_data->features[dnx_data_lif_feature_iop_mode_outlif_selection].name = "iop_mode_outlif_selection";
    submodule_data->features[dnx_data_lif_feature_iop_mode_outlif_selection].doc = "";
    submodule_data->features[dnx_data_lif_feature_iop_mode_outlif_selection].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_lif_feature_iop_mode_orientation_selection].name = "iop_mode_orientation_selection";
    submodule_data->features[dnx_data_lif_feature_iop_mode_orientation_selection].doc = "";
    submodule_data->features[dnx_data_lif_feature_iop_mode_orientation_selection].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_lif_feature_in_lif_tunnel_wide_data].name = "in_lif_tunnel_wide_data";
    submodule_data->features[dnx_data_lif_feature_in_lif_tunnel_wide_data].doc = "";
    submodule_data->features[dnx_data_lif_feature_in_lif_tunnel_wide_data].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_lif_feature_ingress_egress_vsi_separation].name = "ingress_egress_vsi_separation";
    submodule_data->features[dnx_data_lif_feature_ingress_egress_vsi_separation].doc = "";
    submodule_data->features[dnx_data_lif_feature_ingress_egress_vsi_separation].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_lif_feature_copy_outlif0_to_egress_container_by_epmf].name = "copy_outlif0_to_egress_container_by_epmf";
    submodule_data->features[dnx_data_lif_feature_copy_outlif0_to_egress_container_by_epmf].doc = "";
    submodule_data->features[dnx_data_lif_feature_copy_outlif0_to_egress_container_by_epmf].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_lif_feature_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data lif feature defines");

    
    submodule_data->nof_tables = _dnx_data_lif_feature_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data lif feature tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_lif_feature_feature_get(
    int unit,
    dnx_data_lif_feature_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_lif, dnx_data_lif_submodule_feature, feature);
}








shr_error_e
dnx_data_lif_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "lif";
    module_data->nof_submodules = _dnx_data_lif_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data lif submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_lif_global_lif_init(unit, &module_data->submodules[dnx_data_lif_submodule_global_lif]));
    SHR_IF_ERR_EXIT(dnx_data_lif_virtual_lif_init(unit, &module_data->submodules[dnx_data_lif_submodule_virtual_lif]));
    SHR_IF_ERR_EXIT(dnx_data_lif_in_lif_init(unit, &module_data->submodules[dnx_data_lif_submodule_in_lif]));
    SHR_IF_ERR_EXIT(dnx_data_lif_out_lif_init(unit, &module_data->submodules[dnx_data_lif_submodule_out_lif]));
    SHR_IF_ERR_EXIT(dnx_data_lif_lif_table_manager_init(unit, &module_data->submodules[dnx_data_lif_submodule_lif_table_manager]));
    SHR_IF_ERR_EXIT(dnx_data_lif_feature_init(unit, &module_data->submodules[dnx_data_lif_submodule_feature]));
    

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_lif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_lif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_lif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_lif_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_lif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_lif_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_lif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_lif_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_lif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_lif_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_lif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_lif_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_lif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_lif_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_lif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_lif_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_lif_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a1_data_lif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_lif_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_lif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_lif_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_lif_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

