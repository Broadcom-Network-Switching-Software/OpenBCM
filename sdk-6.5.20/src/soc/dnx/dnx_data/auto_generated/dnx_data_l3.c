

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L3

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_l3.h>



extern shr_error_e jr2_a0_data_l3_attach(
    int unit);
extern shr_error_e jr2_b0_data_l3_attach(
    int unit);
extern shr_error_e j2c_a0_data_l3_attach(
    int unit);
extern shr_error_e q2a_a0_data_l3_attach(
    int unit);
extern shr_error_e j2p_a0_data_l3_attach(
    int unit);



static shr_error_e
dnx_data_l3_egr_pointed_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "egr_pointed";
    submodule_data->doc = "Virtual egress objects which do not access GLEM";
    
    submodule_data->nof_features = _dnx_data_l3_egr_pointed_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l3 egr_pointed features");

    
    submodule_data->nof_defines = _dnx_data_l3_egr_pointed_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l3 egr_pointed defines");

    submodule_data->defines[dnx_data_l3_egr_pointed_define_first_egr_pointed_id].name = "first_egr_pointed_id";
    submodule_data->defines[dnx_data_l3_egr_pointed_define_first_egr_pointed_id].doc = "Identifier of very first virtual egress pointed object.                            Range is from 'first_egr_pointed_id' to 'nof_egr_pointed_ids - 1'";
    
    submodule_data->defines[dnx_data_l3_egr_pointed_define_first_egr_pointed_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_egr_pointed_define_nof_egr_pointed_ids].name = "nof_egr_pointed_ids";
    submodule_data->defines[dnx_data_l3_egr_pointed_define_nof_egr_pointed_ids].doc = "Number of virtual egress pointed object ids";
    
    submodule_data->defines[dnx_data_l3_egr_pointed_define_nof_egr_pointed_ids].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l3_egr_pointed_define_nof_bits_in_egr_pointed_id].name = "nof_bits_in_egr_pointed_id";
    submodule_data->defines[dnx_data_l3_egr_pointed_define_nof_bits_in_egr_pointed_id].doc = "Number of bits representing one virtual egress pointed object id";
    
    submodule_data->defines[dnx_data_l3_egr_pointed_define_nof_bits_in_egr_pointed_id].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_l3_egr_pointed_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l3 egr_pointed tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l3_egr_pointed_feature_get(
    int unit,
    dnx_data_l3_egr_pointed_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_egr_pointed, feature);
}


uint32
dnx_data_l3_egr_pointed_first_egr_pointed_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_egr_pointed, dnx_data_l3_egr_pointed_define_first_egr_pointed_id);
}

uint32
dnx_data_l3_egr_pointed_nof_egr_pointed_ids_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_egr_pointed, dnx_data_l3_egr_pointed_define_nof_egr_pointed_ids);
}

uint32
dnx_data_l3_egr_pointed_nof_bits_in_egr_pointed_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_egr_pointed, dnx_data_l3_egr_pointed_define_nof_bits_in_egr_pointed_id);
}










static shr_error_e
dnx_data_l3_fec_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "fec";
    submodule_data->doc = "FEC data";
    
    submodule_data->nof_features = _dnx_data_l3_fec_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l3 fec features");

    
    submodule_data->nof_defines = _dnx_data_l3_fec_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l3 fec defines");

    submodule_data->defines[dnx_data_l3_fec_define_nof_fecs].name = "nof_fecs";
    submodule_data->defines[dnx_data_l3_fec_define_nof_fecs].doc = "The maximal number of FECs supported by the device.";
    
    submodule_data->defines[dnx_data_l3_fec_define_nof_fecs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_first_valid_fec_ecmp_id].name = "first_valid_fec_ecmp_id";
    submodule_data->defines[dnx_data_l3_fec_define_first_valid_fec_ecmp_id].doc = "The ID of the first valid FEC/ECMP group which can be created";
    
    submodule_data->defines[dnx_data_l3_fec_define_first_valid_fec_ecmp_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_max_fec_id_for_single_dhb_cluster_pair_granularity].name = "max_fec_id_for_single_dhb_cluster_pair_granularity";
    submodule_data->defines[dnx_data_l3_fec_define_max_fec_id_for_single_dhb_cluster_pair_granularity].doc = "The maximal FEC ID that support a single DHB cluasters pair.";
    
    submodule_data->defines[dnx_data_l3_fec_define_max_fec_id_for_single_dhb_cluster_pair_granularity].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_max_fec_id_for_double_dhb_cluster_pair_granularity].name = "max_fec_id_for_double_dhb_cluster_pair_granularity";
    submodule_data->defines[dnx_data_l3_fec_define_max_fec_id_for_double_dhb_cluster_pair_granularity].doc = "The maximal FEC ID that requiere an even number of DHB clusters pairs.";
    
    submodule_data->defines[dnx_data_l3_fec_define_max_fec_id_for_double_dhb_cluster_pair_granularity].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_max_default_fec].name = "max_default_fec";
    submodule_data->defines[dnx_data_l3_fec_define_max_default_fec].doc = "The maximal FEC id that can be used for default route.";
    
    submodule_data->defines[dnx_data_l3_fec_define_max_default_fec].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_bank_size].name = "bank_size";
    submodule_data->defines[dnx_data_l3_fec_define_bank_size].doc = "The smallest number of FECs which belong to the same hierarchy";
    
    submodule_data->defines[dnx_data_l3_fec_define_bank_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_bank_size_round_up].name = "bank_size_round_up";
    submodule_data->defines[dnx_data_l3_fec_define_bank_size_round_up].doc = "In some cases FEC bank size contains extra FECs due to round up which results from physical to logical translation";
    
    submodule_data->defines[dnx_data_l3_fec_define_bank_size_round_up].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_max_nof_super_fecs_per_bank].name = "max_nof_super_fecs_per_bank";
    submodule_data->defines[dnx_data_l3_fec_define_max_nof_super_fecs_per_bank].doc = "The max NOF SUPER FEC IDs per resource manager bank.";
    
    submodule_data->defines[dnx_data_l3_fec_define_max_nof_super_fecs_per_bank].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_nof_physical_fecs_per_bank].name = "nof_physical_fecs_per_bank";
    submodule_data->defines[dnx_data_l3_fec_define_nof_physical_fecs_per_bank].doc = "The number of physical FEC IDs per bank.";
    
    submodule_data->defines[dnx_data_l3_fec_define_nof_physical_fecs_per_bank].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_super_fec_size].name = "super_fec_size";
    submodule_data->defines[dnx_data_l3_fec_define_super_fec_size].doc = "The number of bits of a single super FEC in the MDB.";
    
    submodule_data->defines[dnx_data_l3_fec_define_super_fec_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_max_nof_banks].name = "max_nof_banks";
    submodule_data->defines[dnx_data_l3_fec_define_max_nof_banks].doc = "The maximal NOF possible banks in the device (not MDB profile dependent).";
    
    submodule_data->defines[dnx_data_l3_fec_define_max_nof_banks].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_first_bank_without_id_alloc].name = "first_bank_without_id_alloc";
    submodule_data->defines[dnx_data_l3_fec_define_first_bank_without_id_alloc].doc = "the first fec allocation bank that can be used for allocating FECs without an ID";
    
    submodule_data->defines[dnx_data_l3_fec_define_first_bank_without_id_alloc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_max_super_fec_id].name = "max_super_fec_id";
    submodule_data->defines[dnx_data_l3_fec_define_max_super_fec_id].doc = "The max ID of the super FEC";
    
    submodule_data->defines[dnx_data_l3_fec_define_max_super_fec_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fec_define_fer_mdb_granularity_ratio].name = "fer_mdb_granularity_ratio";
    submodule_data->defines[dnx_data_l3_fec_define_fer_mdb_granularity_ratio].doc = "This value indicate the ratio between the FER FEC granularity relative to the MDB";
    
    submodule_data->defines[dnx_data_l3_fec_define_fer_mdb_granularity_ratio].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_l3_fec_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l3 fec tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l3_fec_feature_get(
    int unit,
    dnx_data_l3_fec_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, feature);
}


uint32
dnx_data_l3_fec_nof_fecs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_nof_fecs);
}

uint32
dnx_data_l3_fec_first_valid_fec_ecmp_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_first_valid_fec_ecmp_id);
}

uint32
dnx_data_l3_fec_max_fec_id_for_single_dhb_cluster_pair_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_max_fec_id_for_single_dhb_cluster_pair_granularity);
}

uint32
dnx_data_l3_fec_max_fec_id_for_double_dhb_cluster_pair_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_max_fec_id_for_double_dhb_cluster_pair_granularity);
}

uint32
dnx_data_l3_fec_max_default_fec_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_max_default_fec);
}

uint32
dnx_data_l3_fec_bank_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_bank_size);
}

uint32
dnx_data_l3_fec_bank_size_round_up_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_bank_size_round_up);
}

uint32
dnx_data_l3_fec_max_nof_super_fecs_per_bank_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_max_nof_super_fecs_per_bank);
}

uint32
dnx_data_l3_fec_nof_physical_fecs_per_bank_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_nof_physical_fecs_per_bank);
}

uint32
dnx_data_l3_fec_super_fec_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_super_fec_size);
}

uint32
dnx_data_l3_fec_max_nof_banks_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_max_nof_banks);
}

uint32
dnx_data_l3_fec_first_bank_without_id_alloc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_first_bank_without_id_alloc);
}

uint32
dnx_data_l3_fec_max_super_fec_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_max_super_fec_id);
}

uint32
dnx_data_l3_fec_fer_mdb_granularity_ratio_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fec, dnx_data_l3_fec_define_fer_mdb_granularity_ratio);
}










static shr_error_e
dnx_data_l3_source_address_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "source_address";
    submodule_data->doc = "Source address data.";
    
    submodule_data->nof_features = _dnx_data_l3_source_address_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l3 source_address features");

    
    submodule_data->nof_defines = _dnx_data_l3_source_address_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l3 source_address defines");

    submodule_data->defines[dnx_data_l3_source_address_define_source_address_table_size].name = "source_address_table_size";
    submodule_data->defines[dnx_data_l3_source_address_define_source_address_table_size].doc = "The number of entries in the source address table.";
    
    submodule_data->defines[dnx_data_l3_source_address_define_source_address_table_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_source_address_define_custom_sa_use_dual_homing].name = "custom_sa_use_dual_homing";
    submodule_data->defines[dnx_data_l3_source_address_define_custom_sa_use_dual_homing].doc = "Indication whether ARP custom sa feature is using EEDB dual homing field or full mac address from source address table.";
    
    submodule_data->defines[dnx_data_l3_source_address_define_custom_sa_use_dual_homing].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_l3_source_address_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l3 source_address tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l3_source_address_feature_get(
    int unit,
    dnx_data_l3_source_address_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_source_address, feature);
}


uint32
dnx_data_l3_source_address_source_address_table_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_source_address, dnx_data_l3_source_address_define_source_address_table_size);
}

uint32
dnx_data_l3_source_address_custom_sa_use_dual_homing_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_source_address, dnx_data_l3_source_address_define_custom_sa_use_dual_homing);
}










static shr_error_e
dnx_data_l3_vrf_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "vrf";
    submodule_data->doc = "vrf data.";
    
    submodule_data->nof_features = _dnx_data_l3_vrf_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l3 vrf features");

    
    submodule_data->nof_defines = _dnx_data_l3_vrf_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l3 vrf defines");

    submodule_data->defines[dnx_data_l3_vrf_define_nof_vrf].name = "nof_vrf";
    submodule_data->defines[dnx_data_l3_vrf_define_nof_vrf].doc = "The maximal number of VRFs supported by the device.";
    
    submodule_data->defines[dnx_data_l3_vrf_define_nof_vrf].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_vrf_define_nof_vrf_ipv6].name = "nof_vrf_ipv6";
    submodule_data->defines[dnx_data_l3_vrf_define_nof_vrf_ipv6].doc = "The maximal number of VRFs supported by the device for an IPv6 MC table.";
    
    submodule_data->defines[dnx_data_l3_vrf_define_nof_vrf_ipv6].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_l3_vrf_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l3 vrf tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l3_vrf_feature_get(
    int unit,
    dnx_data_l3_vrf_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrf, feature);
}


uint32
dnx_data_l3_vrf_nof_vrf_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrf, dnx_data_l3_vrf_define_nof_vrf);
}

uint32
dnx_data_l3_vrf_nof_vrf_ipv6_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrf, dnx_data_l3_vrf_define_nof_vrf_ipv6);
}










static shr_error_e
dnx_data_l3_routing_enablers_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "routing_enablers";
    submodule_data->doc = "routing enablers data.";
    
    submodule_data->nof_features = _dnx_data_l3_routing_enablers_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l3 routing_enablers features");

    
    submodule_data->nof_defines = _dnx_data_l3_routing_enablers_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l3 routing_enablers defines");

    submodule_data->defines[dnx_data_l3_routing_enablers_define_nof_routing_enablers_profiles].name = "nof_routing_enablers_profiles";
    submodule_data->defines[dnx_data_l3_routing_enablers_define_nof_routing_enablers_profiles].doc = "The maximal number of references for a routing enablers vector.";
    
    submodule_data->defines[dnx_data_l3_routing_enablers_define_nof_routing_enablers_profiles].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_l3_routing_enablers_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l3 routing_enablers tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l3_routing_enablers_feature_get(
    int unit,
    dnx_data_l3_routing_enablers_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_routing_enablers, feature);
}


uint32
dnx_data_l3_routing_enablers_nof_routing_enablers_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_routing_enablers, dnx_data_l3_routing_enablers_define_nof_routing_enablers_profiles);
}










static shr_error_e
dnx_data_l3_rif_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "rif";
    submodule_data->doc = "Routing interface";
    
    submodule_data->nof_features = _dnx_data_l3_rif_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l3 rif features");

    
    submodule_data->nof_defines = _dnx_data_l3_rif_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l3 rif defines");

    submodule_data->defines[dnx_data_l3_rif_define_max_nof_rifs].name = "max_nof_rifs";
    submodule_data->defines[dnx_data_l3_rif_define_max_nof_rifs].doc = "Maximum number of rifs supported by the device.";
    
    submodule_data->defines[dnx_data_l3_rif_define_max_nof_rifs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_rif_define_nof_rifs].name = "nof_rifs";
    submodule_data->defines[dnx_data_l3_rif_define_nof_rifs].doc = "Number of rifs available in the system.";
    
    submodule_data->defines[dnx_data_l3_rif_define_nof_rifs].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_l3_rif_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l3 rif tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l3_rif_feature_get(
    int unit,
    dnx_data_l3_rif_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_rif, feature);
}


uint32
dnx_data_l3_rif_max_nof_rifs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_rif, dnx_data_l3_rif_define_max_nof_rifs);
}

uint32
dnx_data_l3_rif_nof_rifs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_rif, dnx_data_l3_rif_define_nof_rifs);
}










static shr_error_e
dnx_data_l3_fwd_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "fwd";
    submodule_data->doc = "Forwarding data";
    
    submodule_data->nof_features = _dnx_data_l3_fwd_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l3 fwd features");

    
    submodule_data->nof_defines = _dnx_data_l3_fwd_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l3 fwd defines");

    submodule_data->defines[dnx_data_l3_fwd_define_flp_fragment_support].name = "flp_fragment_support";
    submodule_data->defines[dnx_data_l3_fwd_define_flp_fragment_support].doc = "Indicate whether the LB fragmentation is handled by the FLP.";
    
    submodule_data->defines[dnx_data_l3_fwd_define_flp_fragment_support].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fwd_define_max_mc_group_lpm].name = "max_mc_group_lpm";
    submodule_data->defines[dnx_data_l3_fwd_define_max_mc_group_lpm].doc = "The maximum number of MC group ID for IPMC route entries.";
    
    submodule_data->defines[dnx_data_l3_fwd_define_max_mc_group_lpm].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fwd_define_max_mc_group_em].name = "max_mc_group_em";
    submodule_data->defines[dnx_data_l3_fwd_define_max_mc_group_em].doc = "The maximum number of MC group ID for IPMC host entries.";
    
    submodule_data->defines[dnx_data_l3_fwd_define_max_mc_group_em].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fwd_define_nof_my_mac_prefixes].name = "nof_my_mac_prefixes";
    submodule_data->defines[dnx_data_l3_fwd_define_nof_my_mac_prefixes].doc = "The number of my MACs prefixes.";
    
    submodule_data->defines[dnx_data_l3_fwd_define_nof_my_mac_prefixes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fwd_define_compressed_sip_svl_size].name = "compressed_sip_svl_size";
    submodule_data->defines[dnx_data_l3_fwd_define_compressed_sip_svl_size].doc = "The size in bits of the IPMC IP6 compressed SIP for SVL table";
    
    submodule_data->defines[dnx_data_l3_fwd_define_compressed_sip_svl_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fwd_define_nof_compressed_svl_sip].name = "nof_compressed_svl_sip";
    submodule_data->defines[dnx_data_l3_fwd_define_nof_compressed_svl_sip].doc = "Number of supported IPMC compressed SIPs.";
    
    submodule_data->defines[dnx_data_l3_fwd_define_nof_compressed_svl_sip].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fwd_define_lpm_strength_profile_support].name = "lpm_strength_profile_support";
    submodule_data->defines[dnx_data_l3_fwd_define_lpm_strength_profile_support].doc = "Indicates whether the KAPS strength mapping is different per LPM profile.";
    
    submodule_data->defines[dnx_data_l3_fwd_define_lpm_strength_profile_support].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fwd_define_nof_lpm_profiles].name = "nof_lpm_profiles";
    submodule_data->defines[dnx_data_l3_fwd_define_nof_lpm_profiles].doc = "The number of LPM profiles";
    
    submodule_data->defines[dnx_data_l3_fwd_define_nof_lpm_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_fwd_define_nof_prefix_ranges].name = "nof_prefix_ranges";
    submodule_data->defines[dnx_data_l3_fwd_define_nof_prefix_ranges].doc = "The number of KAPS prefix ranges.";
    
    submodule_data->defines[dnx_data_l3_fwd_define_nof_prefix_ranges].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_l3_fwd_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l3 fwd tables");

    
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].name = "lpm_profile_to_entry_strength";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].doc = "LPM profile to entry strength mapping";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].size_of_values = sizeof(dnx_data_l3_fwd_lpm_profile_to_entry_strength_t);
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].entry_get = dnx_data_l3_fwd_lpm_profile_to_entry_strength_entry_str_get;

    
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].nof_keys = 2;
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].keys[0].name = "lpm_profile";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].keys[0].doc = "LPM profile.";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].keys[1].name = "kaps_intf";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].keys[1].doc = "KAPS interface.";

    
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].nof_values, "_dnx_data_l3_fwd_table_lpm_profile_to_entry_strength table values");
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[0].name = "prefix_len_non_def";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[0].type = "uint32[8]";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[0].doc = "Prefix length ranges for non-default entries.The value in each array cell is the highest value of the range.";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[0].offset = UTILEX_OFFSETOF(dnx_data_l3_fwd_lpm_profile_to_entry_strength_t, prefix_len_non_def);
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[1].name = "prefix_len_def";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[1].type = "uint32[8]";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[1].doc = "Prefix length ranges for non-default entries.The value in each array cell is the highest value of the range.";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[1].offset = UTILEX_OFFSETOF(dnx_data_l3_fwd_lpm_profile_to_entry_strength_t, prefix_len_def);
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[2].name = "entry_strength_non_def";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[2].type = "uint32[8]";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[2].doc = "Entry strength.";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[2].offset = UTILEX_OFFSETOF(dnx_data_l3_fwd_lpm_profile_to_entry_strength_t, entry_strength_non_def);
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[3].name = "entry_strength_def";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[3].type = "uint32[8]";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[3].doc = "Default entry strength.";
    submodule_data->tables[dnx_data_l3_fwd_table_lpm_profile_to_entry_strength].values[3].offset = UTILEX_OFFSETOF(dnx_data_l3_fwd_lpm_profile_to_entry_strength_t, entry_strength_def);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l3_fwd_feature_get(
    int unit,
    dnx_data_l3_fwd_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, feature);
}


uint32
dnx_data_l3_fwd_flp_fragment_support_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_define_flp_fragment_support);
}

uint32
dnx_data_l3_fwd_max_mc_group_lpm_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_define_max_mc_group_lpm);
}

uint32
dnx_data_l3_fwd_max_mc_group_em_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_define_max_mc_group_em);
}

uint32
dnx_data_l3_fwd_nof_my_mac_prefixes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_define_nof_my_mac_prefixes);
}

uint32
dnx_data_l3_fwd_compressed_sip_svl_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_define_compressed_sip_svl_size);
}

uint32
dnx_data_l3_fwd_nof_compressed_svl_sip_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_define_nof_compressed_svl_sip);
}

uint32
dnx_data_l3_fwd_lpm_strength_profile_support_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_define_lpm_strength_profile_support);
}

uint32
dnx_data_l3_fwd_nof_lpm_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_define_nof_lpm_profiles);
}

uint32
dnx_data_l3_fwd_nof_prefix_ranges_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_define_nof_prefix_ranges);
}



const dnx_data_l3_fwd_lpm_profile_to_entry_strength_t *
dnx_data_l3_fwd_lpm_profile_to_entry_strength_get(
    int unit,
    int lpm_profile,
    int kaps_intf)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_table_lpm_profile_to_entry_strength);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, lpm_profile, kaps_intf);
    return (const dnx_data_l3_fwd_lpm_profile_to_entry_strength_t *) data;

}


shr_error_e
dnx_data_l3_fwd_lpm_profile_to_entry_strength_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l3_fwd_lpm_profile_to_entry_strength_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_table_lpm_profile_to_entry_strength);
    data = (const dnx_data_l3_fwd_lpm_profile_to_entry_strength_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, 8, data->prefix_len_non_def);
            break;
        case 1:
            DNXC_DATA_MGMT_ARR_STR(buffer, 8, data->prefix_len_def);
            break;
        case 2:
            DNXC_DATA_MGMT_ARR_STR(buffer, 8, data->entry_strength_non_def);
            break;
        case 3:
            DNXC_DATA_MGMT_ARR_STR(buffer, 8, data->entry_strength_def);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_l3_fwd_lpm_profile_to_entry_strength_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_fwd, dnx_data_l3_fwd_table_lpm_profile_to_entry_strength);

}






static shr_error_e
dnx_data_l3_ecmp_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ecmp";
    submodule_data->doc = "ECMP data";
    
    submodule_data->nof_features = _dnx_data_l3_ecmp_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l3 ecmp features");

    submodule_data->features[dnx_data_l3_ecmp_ecmp_extended].name = "ecmp_extended";
    submodule_data->features[dnx_data_l3_ecmp_ecmp_extended].doc = "Allows extended number of ECMPs.";
    submodule_data->features[dnx_data_l3_ecmp_ecmp_extended].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_l3_ecmp_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l3 ecmp defines");

    submodule_data->defines[dnx_data_l3_ecmp_define_group_size_multiply_and_divide_nof_bits].name = "group_size_multiply_and_divide_nof_bits";
    submodule_data->defines[dnx_data_l3_ecmp_define_group_size_multiply_and_divide_nof_bits].doc = "The NOF bit of the multiply and division group size field.";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_group_size_multiply_and_divide_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_group_size_consistent_nof_bits].name = "group_size_consistent_nof_bits";
    submodule_data->defines[dnx_data_l3_ecmp_define_group_size_consistent_nof_bits].doc = "The NOF bit of the consistent group size field.";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_group_size_consistent_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_group_size_nof_bits].name = "group_size_nof_bits";
    submodule_data->defines[dnx_data_l3_ecmp_define_group_size_nof_bits].doc = "The max NOF bit of the ECMP group size field.";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_group_size_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_max_group_size].name = "max_group_size";
    submodule_data->defines[dnx_data_l3_ecmp_define_max_group_size].doc = "The maximal size of an ECMP group.";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_max_group_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp_per_bank_of_low_ecmps].name = "nof_ecmp_per_bank_of_low_ecmps";
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp_per_bank_of_low_ecmps].doc = "The low 32k ECMP are placed in a memory of 16 banks, each bank holds 2k ECMPS.";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp_per_bank_of_low_ecmps].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp_basic_mode].name = "nof_ecmp_basic_mode";
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp_basic_mode].doc = "Number of ECMP groups that can be created in basic mode as opposed to extended mode";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp_basic_mode].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_total_nof_ecmp].name = "total_nof_ecmp";
    submodule_data->defines[dnx_data_l3_ecmp_define_total_nof_ecmp].doc = "The total number of IDs that are considered to be in the ECMP range.";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_total_nof_ecmp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_max_ecmp_basic_mode].name = "max_ecmp_basic_mode";
    submodule_data->defines[dnx_data_l3_ecmp_define_max_ecmp_basic_mode].doc = "Maximum ECMP group ID that can be created in basic mode as opposed to extended mode";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_max_ecmp_basic_mode].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_max_ecmp_extended_mode].name = "max_ecmp_extended_mode";
    submodule_data->defines[dnx_data_l3_ecmp_define_max_ecmp_extended_mode].doc = "Maximum ECMP group ID that can be created in extended mode";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_max_ecmp_extended_mode].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp_per_bank_of_extended_ecmps].name = "nof_ecmp_per_bank_of_extended_ecmps";
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp_per_bank_of_extended_ecmps].doc = "The upper 8k ECMP (from 32k to 40k) are placed in a memory of 16 banks, each bank holds 512 ECMPS.";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp_per_bank_of_extended_ecmps].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_nof_fec_banks_per_extended_ecmp_bank].name = "nof_fec_banks_per_extended_ecmp_bank";
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_fec_banks_per_extended_ecmp_bank].doc = "nof fec protection banks to be used for 1 ecmp extended bank";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_fec_banks_per_extended_ecmp_bank].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_nof_group_profiles_per_hierarchy].name = "nof_group_profiles_per_hierarchy";
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_group_profiles_per_hierarchy].doc = "Number of profiles that are available for each ECMP hierarchy.";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_group_profiles_per_hierarchy].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_member_table_nof_rows_size_in_bits].name = "member_table_nof_rows_size_in_bits";
    submodule_data->defines[dnx_data_l3_ecmp_define_member_table_nof_rows_size_in_bits].doc = "The NOF bits required to represent the NOF rows in the consistent members memory.";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_member_table_nof_rows_size_in_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_member_table_address_size_in_bits].name = "member_table_address_size_in_bits";
    submodule_data->defines[dnx_data_l3_ecmp_define_member_table_address_size_in_bits].doc = "The member table address size in bits when it used by the ECMP profile.";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_member_table_address_size_in_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_ecmp_stage_map_size].name = "ecmp_stage_map_size";
    submodule_data->defines[dnx_data_l3_ecmp_define_ecmp_stage_map_size].doc = "The smallest number of consecutive ECMP groups which belong to the same hierarchy";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_ecmp_stage_map_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp_banks_basic].name = "nof_ecmp_banks_basic";
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp_banks_basic].doc = "The number of ECMP banks in the basic range of IDs.";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp_banks_basic].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp_banks_extended].name = "nof_ecmp_banks_extended";
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp_banks_extended].doc = "The number of ECMP banks in the extended range of IDs.";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp_banks_extended].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp].name = "nof_ecmp";
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp].doc = "Number of ECMPs in the system.";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_ecmp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_profile_id_size].name = "profile_id_size";
    submodule_data->defines[dnx_data_l3_ecmp_define_profile_id_size].doc = "The size of the profile ID field";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_profile_id_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_profile_id_offset].name = "profile_id_offset";
    submodule_data->defines[dnx_data_l3_ecmp_define_profile_id_offset].doc = "The offset of the profile ID field";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_profile_id_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_member_table_row_width].name = "member_table_row_width";
    submodule_data->defines[dnx_data_l3_ecmp_define_member_table_row_width].doc = "The width in bits of a row in the ECMP member table.";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_member_table_row_width].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_tunnel_priority_support].name = "tunnel_priority_support";
    submodule_data->defines[dnx_data_l3_ecmp_define_tunnel_priority_support].doc = "Device has tunnel priority support.";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_tunnel_priority_support].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_nof_tunnel_priority_map_profiles].name = "nof_tunnel_priority_map_profiles";
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_tunnel_priority_map_profiles].doc = "The NOF tunnle priority map profiles";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_nof_tunnel_priority_map_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_tunnel_priority_field_width].name = "tunnel_priority_field_width";
    submodule_data->defines[dnx_data_l3_ecmp_define_tunnel_priority_field_width].doc = "The width in bits of the tunnel priority field.";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_tunnel_priority_field_width].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_tunnel_priority_index_field_width].name = "tunnel_priority_index_field_width";
    submodule_data->defines[dnx_data_l3_ecmp_define_tunnel_priority_index_field_width].doc = "The size of the possible TP indexes field.";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_tunnel_priority_index_field_width].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_ecmp_define_consistent_mem_row_size_in_bits].name = "consistent_mem_row_size_in_bits";
    submodule_data->defines[dnx_data_l3_ecmp_define_consistent_mem_row_size_in_bits].doc = "The size in bits of a row in the consistent members memory.";
    
    submodule_data->defines[dnx_data_l3_ecmp_define_consistent_mem_row_size_in_bits].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_l3_ecmp_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l3 ecmp tables");

    
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].name = "consistent_tables_info";
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].doc = "information on the different consistent tables sizes";
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].size_of_values = sizeof(dnx_data_l3_ecmp_consistent_tables_info_t);
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].entry_get = dnx_data_l3_ecmp_consistent_tables_info_entry_str_get;

    
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].nof_keys = 1;
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].keys[0].name = "table_type";
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].keys[0].doc = "The table type.";

    
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].nof_values, "_dnx_data_l3_ecmp_table_consistent_tables_info table values");
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].values[0].name = "nof_entries";
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].values[0].type = "uint32";
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].values[0].doc = "The NOF entries this tables holds";
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_l3_ecmp_consistent_tables_info_t, nof_entries);
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].values[1].name = "entry_size_in_bits";
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].values[1].doc = "A single table entry size in bits.";
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_l3_ecmp_consistent_tables_info_t, entry_size_in_bits);
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].values[2].name = "max_nof_unique_members";
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].values[2].type = "uint32";
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].values[2].doc = "The max NOF unique members that this table can hold.";
    submodule_data->tables[dnx_data_l3_ecmp_table_consistent_tables_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_l3_ecmp_consistent_tables_info_t, max_nof_unique_members);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l3_ecmp_feature_get(
    int unit,
    dnx_data_l3_ecmp_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, feature);
}


uint32
dnx_data_l3_ecmp_group_size_multiply_and_divide_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_group_size_multiply_and_divide_nof_bits);
}

uint32
dnx_data_l3_ecmp_group_size_consistent_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_group_size_consistent_nof_bits);
}

uint32
dnx_data_l3_ecmp_group_size_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_group_size_nof_bits);
}

uint32
dnx_data_l3_ecmp_max_group_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_max_group_size);
}

uint32
dnx_data_l3_ecmp_nof_ecmp_per_bank_of_low_ecmps_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_nof_ecmp_per_bank_of_low_ecmps);
}

uint32
dnx_data_l3_ecmp_nof_ecmp_basic_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_nof_ecmp_basic_mode);
}

uint32
dnx_data_l3_ecmp_total_nof_ecmp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_total_nof_ecmp);
}

uint32
dnx_data_l3_ecmp_max_ecmp_basic_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_max_ecmp_basic_mode);
}

uint32
dnx_data_l3_ecmp_max_ecmp_extended_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_max_ecmp_extended_mode);
}

uint32
dnx_data_l3_ecmp_nof_ecmp_per_bank_of_extended_ecmps_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_nof_ecmp_per_bank_of_extended_ecmps);
}

uint32
dnx_data_l3_ecmp_nof_fec_banks_per_extended_ecmp_bank_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_nof_fec_banks_per_extended_ecmp_bank);
}

uint32
dnx_data_l3_ecmp_nof_group_profiles_per_hierarchy_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_nof_group_profiles_per_hierarchy);
}

uint32
dnx_data_l3_ecmp_member_table_nof_rows_size_in_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_member_table_nof_rows_size_in_bits);
}

uint32
dnx_data_l3_ecmp_member_table_address_size_in_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_member_table_address_size_in_bits);
}

uint32
dnx_data_l3_ecmp_ecmp_stage_map_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_ecmp_stage_map_size);
}

uint32
dnx_data_l3_ecmp_nof_ecmp_banks_basic_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_nof_ecmp_banks_basic);
}

uint32
dnx_data_l3_ecmp_nof_ecmp_banks_extended_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_nof_ecmp_banks_extended);
}

uint32
dnx_data_l3_ecmp_nof_ecmp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_nof_ecmp);
}

uint32
dnx_data_l3_ecmp_profile_id_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_profile_id_size);
}

uint32
dnx_data_l3_ecmp_profile_id_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_profile_id_offset);
}

uint32
dnx_data_l3_ecmp_member_table_row_width_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_member_table_row_width);
}

uint32
dnx_data_l3_ecmp_tunnel_priority_support_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_tunnel_priority_support);
}

uint32
dnx_data_l3_ecmp_nof_tunnel_priority_map_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_nof_tunnel_priority_map_profiles);
}

uint32
dnx_data_l3_ecmp_tunnel_priority_field_width_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_tunnel_priority_field_width);
}

uint32
dnx_data_l3_ecmp_tunnel_priority_index_field_width_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_tunnel_priority_index_field_width);
}

uint32
dnx_data_l3_ecmp_consistent_mem_row_size_in_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_define_consistent_mem_row_size_in_bits);
}



const dnx_data_l3_ecmp_consistent_tables_info_t *
dnx_data_l3_ecmp_consistent_tables_info_get(
    int unit,
    int table_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_table_consistent_tables_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, table_type, 0);
    return (const dnx_data_l3_ecmp_consistent_tables_info_t *) data;

}


shr_error_e
dnx_data_l3_ecmp_consistent_tables_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l3_ecmp_consistent_tables_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_table_consistent_tables_info);
    data = (const dnx_data_l3_ecmp_consistent_tables_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_entries);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->entry_size_in_bits);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->max_nof_unique_members);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_l3_ecmp_consistent_tables_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_ecmp, dnx_data_l3_ecmp_table_consistent_tables_info);

}






static shr_error_e
dnx_data_l3_vip_ecmp_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "vip_ecmp";
    submodule_data->doc = "VIRTUAL IP ECMP group data";
    
    submodule_data->nof_features = _dnx_data_l3_vip_ecmp_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l3 vip_ecmp features");

    
    submodule_data->nof_defines = _dnx_data_l3_vip_ecmp_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l3 vip_ecmp defines");

    submodule_data->defines[dnx_data_l3_vip_ecmp_define_nof_vip_ecmp].name = "nof_vip_ecmp";
    submodule_data->defines[dnx_data_l3_vip_ecmp_define_nof_vip_ecmp].doc = "Number of VIP ECMP groups";
    
    submodule_data->defines[dnx_data_l3_vip_ecmp_define_nof_vip_ecmp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_vip_ecmp_define_max_vip_ecmp_table_size].name = "max_vip_ecmp_table_size";
    submodule_data->defines[dnx_data_l3_vip_ecmp_define_max_vip_ecmp_table_size].doc = "Maximum number entries per VIP ECMP group";
    
    submodule_data->defines[dnx_data_l3_vip_ecmp_define_max_vip_ecmp_table_size].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_l3_vip_ecmp_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l3 vip_ecmp tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l3_vip_ecmp_feature_get(
    int unit,
    dnx_data_l3_vip_ecmp_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vip_ecmp, feature);
}


uint32
dnx_data_l3_vip_ecmp_nof_vip_ecmp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vip_ecmp, dnx_data_l3_vip_ecmp_define_nof_vip_ecmp);
}

uint32
dnx_data_l3_vip_ecmp_max_vip_ecmp_table_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vip_ecmp, dnx_data_l3_vip_ecmp_define_max_vip_ecmp_table_size);
}










static shr_error_e
dnx_data_l3_source_address_ethernet_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "source_address_ethernet";
    submodule_data->doc = "SOURCE_ADDRESS_ETHERNET data";
    
    submodule_data->nof_features = _dnx_data_l3_source_address_ethernet_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l3 source_address_ethernet features");

    
    submodule_data->nof_defines = _dnx_data_l3_source_address_ethernet_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l3 source_address_ethernet defines");

    submodule_data->defines[dnx_data_l3_source_address_ethernet_define_nof_mymac_prefixes].name = "nof_mymac_prefixes";
    submodule_data->defines[dnx_data_l3_source_address_ethernet_define_nof_mymac_prefixes].doc = "Number of mymac prefixes";
    
    submodule_data->defines[dnx_data_l3_source_address_ethernet_define_nof_mymac_prefixes].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_l3_source_address_ethernet_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l3 source_address_ethernet tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l3_source_address_ethernet_feature_get(
    int unit,
    dnx_data_l3_source_address_ethernet_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_source_address_ethernet, feature);
}


uint32
dnx_data_l3_source_address_ethernet_nof_mymac_prefixes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_source_address_ethernet, dnx_data_l3_source_address_ethernet_define_nof_mymac_prefixes);
}










static shr_error_e
dnx_data_l3_vrrp_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "vrrp";
    submodule_data->doc = "VRRP (multiple my mac) data";
    
    submodule_data->nof_features = _dnx_data_l3_vrrp_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l3 vrrp features");

    
    submodule_data->nof_defines = _dnx_data_l3_vrrp_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l3 vrrp defines");

    submodule_data->defines[dnx_data_l3_vrrp_define_nof_protocol_groups].name = "nof_protocol_groups";
    submodule_data->defines[dnx_data_l3_vrrp_define_nof_protocol_groups].doc = "Number of L3 protocol groups";
    
    submodule_data->defines[dnx_data_l3_vrrp_define_nof_protocol_groups].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_vrrp_define_nof_tcam_entries].name = "nof_tcam_entries";
    submodule_data->defines[dnx_data_l3_vrrp_define_nof_tcam_entries].doc = "Size of VRRP TCAM table";
    
    submodule_data->defines[dnx_data_l3_vrrp_define_nof_tcam_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_vrrp_define_nof_vsi_tcam_entries].name = "nof_vsi_tcam_entries";
    submodule_data->defines[dnx_data_l3_vrrp_define_nof_vsi_tcam_entries].doc = "Number of enries in VRRP TCAM table that can be used by VSI table";
    
    submodule_data->defines[dnx_data_l3_vrrp_define_nof_vsi_tcam_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_vrrp_define_exem_vrid_ipv4_tcam_index].name = "exem_vrid_ipv4_tcam_index";
    submodule_data->defines[dnx_data_l3_vrrp_define_exem_vrid_ipv4_tcam_index].doc = "TCAM index for EXEM VRRP VRID IPv4 protocol";
    
    submodule_data->defines[dnx_data_l3_vrrp_define_exem_vrid_ipv4_tcam_index].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_vrrp_define_exem_vrid_ipv6_tcam_index].name = "exem_vrid_ipv6_tcam_index";
    submodule_data->defines[dnx_data_l3_vrrp_define_exem_vrid_ipv6_tcam_index].doc = "TCAM index for EXEM VRRP VRID IPv6 protocol";
    
    submodule_data->defines[dnx_data_l3_vrrp_define_exem_vrid_ipv6_tcam_index].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_vrrp_define_exem_default_tcam_index].name = "exem_default_tcam_index";
    submodule_data->defines[dnx_data_l3_vrrp_define_exem_default_tcam_index].doc = "TCAM index for EXEM VSI LSB or SOURCE-PORT multiple my mac";
    
    submodule_data->defines[dnx_data_l3_vrrp_define_exem_default_tcam_index].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_vrrp_define_exem_default_tcam_index_msb].name = "exem_default_tcam_index_msb";
    submodule_data->defines[dnx_data_l3_vrrp_define_exem_default_tcam_index_msb].doc = "TCAM index for EXEM VSI MSB multiple my mac";
    
    submodule_data->defines[dnx_data_l3_vrrp_define_exem_default_tcam_index_msb].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l3_vrrp_define_vrrp_default_num_entries].name = "vrrp_default_num_entries";
    submodule_data->defines[dnx_data_l3_vrrp_define_vrrp_default_num_entries].doc = "Number of default VRRP entries in TCAM";
    
    submodule_data->defines[dnx_data_l3_vrrp_define_vrrp_default_num_entries].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_l3_vrrp_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l3 vrrp tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l3_vrrp_feature_get(
    int unit,
    dnx_data_l3_vrrp_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrrp, feature);
}


uint32
dnx_data_l3_vrrp_nof_protocol_groups_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrrp, dnx_data_l3_vrrp_define_nof_protocol_groups);
}

uint32
dnx_data_l3_vrrp_nof_tcam_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrrp, dnx_data_l3_vrrp_define_nof_tcam_entries);
}

uint32
dnx_data_l3_vrrp_nof_vsi_tcam_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrrp, dnx_data_l3_vrrp_define_nof_vsi_tcam_entries);
}

uint32
dnx_data_l3_vrrp_exem_vrid_ipv4_tcam_index_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrrp, dnx_data_l3_vrrp_define_exem_vrid_ipv4_tcam_index);
}

uint32
dnx_data_l3_vrrp_exem_vrid_ipv6_tcam_index_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrrp, dnx_data_l3_vrrp_define_exem_vrid_ipv6_tcam_index);
}

uint32
dnx_data_l3_vrrp_exem_default_tcam_index_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrrp, dnx_data_l3_vrrp_define_exem_default_tcam_index);
}

uint32
dnx_data_l3_vrrp_exem_default_tcam_index_msb_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrrp, dnx_data_l3_vrrp_define_exem_default_tcam_index_msb);
}

uint32
dnx_data_l3_vrrp_vrrp_default_num_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_vrrp, dnx_data_l3_vrrp_define_vrrp_default_num_entries);
}










static shr_error_e
dnx_data_l3_feature_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "feature";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_l3_feature_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l3 feature features");

    submodule_data->features[dnx_data_l3_feature_ipv6_mc_compatible_dmac].name = "ipv6_mc_compatible_dmac";
    submodule_data->features[dnx_data_l3_feature_ipv6_mc_compatible_dmac].doc = "IPv6 compatible MC destination MAC is considered UC";
    submodule_data->features[dnx_data_l3_feature_ipv6_mc_compatible_dmac].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_feature_fec_hit_bit].name = "fec_hit_bit";
    submodule_data->features[dnx_data_l3_feature_fec_hit_bit].doc = "Hit bit retrieval for FEC is not supported.";
    submodule_data->features[dnx_data_l3_feature_fec_hit_bit].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_feature_fer_fec_granularity_double_size].name = "fer_fec_granularity_double_size";
    submodule_data->features[dnx_data_l3_feature_fer_fec_granularity_double_size].doc = "The FER granularity is double the size of the MDB granularity.";
    submodule_data->features[dnx_data_l3_feature_fer_fec_granularity_double_size].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_feature_mc_bridge_fallback].name = "mc_bridge_fallback";
    submodule_data->features[dnx_data_l3_feature_mc_bridge_fallback].doc = "Multicast bridge fallback is not supported.";
    submodule_data->features[dnx_data_l3_feature_mc_bridge_fallback].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l3_feature_nat_on_a_stick].name = "nat_on_a_stick";
    submodule_data->features[dnx_data_l3_feature_nat_on_a_stick].doc = "NAT on a stick feature";
    submodule_data->features[dnx_data_l3_feature_nat_on_a_stick].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_l3_feature_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l3 feature defines");

    
    submodule_data->nof_tables = _dnx_data_l3_feature_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l3 feature tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l3_feature_feature_get(
    int unit,
    dnx_data_l3_feature_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l3, dnx_data_l3_submodule_feature, feature);
}








shr_error_e
dnx_data_l3_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "l3";
    module_data->nof_submodules = _dnx_data_l3_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data l3 submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_l3_egr_pointed_init(unit, &module_data->submodules[dnx_data_l3_submodule_egr_pointed]));
    SHR_IF_ERR_EXIT(dnx_data_l3_fec_init(unit, &module_data->submodules[dnx_data_l3_submodule_fec]));
    SHR_IF_ERR_EXIT(dnx_data_l3_source_address_init(unit, &module_data->submodules[dnx_data_l3_submodule_source_address]));
    SHR_IF_ERR_EXIT(dnx_data_l3_vrf_init(unit, &module_data->submodules[dnx_data_l3_submodule_vrf]));
    SHR_IF_ERR_EXIT(dnx_data_l3_routing_enablers_init(unit, &module_data->submodules[dnx_data_l3_submodule_routing_enablers]));
    SHR_IF_ERR_EXIT(dnx_data_l3_rif_init(unit, &module_data->submodules[dnx_data_l3_submodule_rif]));
    SHR_IF_ERR_EXIT(dnx_data_l3_fwd_init(unit, &module_data->submodules[dnx_data_l3_submodule_fwd]));
    SHR_IF_ERR_EXIT(dnx_data_l3_ecmp_init(unit, &module_data->submodules[dnx_data_l3_submodule_ecmp]));
    SHR_IF_ERR_EXIT(dnx_data_l3_vip_ecmp_init(unit, &module_data->submodules[dnx_data_l3_submodule_vip_ecmp]));
    SHR_IF_ERR_EXIT(dnx_data_l3_source_address_ethernet_init(unit, &module_data->submodules[dnx_data_l3_submodule_source_address_ethernet]));
    SHR_IF_ERR_EXIT(dnx_data_l3_vrrp_init(unit, &module_data->submodules[dnx_data_l3_submodule_vrrp]));
    SHR_IF_ERR_EXIT(dnx_data_l3_feature_init(unit, &module_data->submodules[dnx_data_l3_submodule_feature]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l3_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l3_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_l3_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l3_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_l3_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l3_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_l3_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l3_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_l3_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l3_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_l3_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l3_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_l3_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l3_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_l3_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l3_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_l3_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

