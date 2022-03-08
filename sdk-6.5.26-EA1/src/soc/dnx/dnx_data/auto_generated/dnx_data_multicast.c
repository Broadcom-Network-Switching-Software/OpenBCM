
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MULTICAST

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_multicast.h>



#ifdef BCM_DNX2_SUPPORT

extern shr_error_e jr2_a0_data_multicast_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e q2a_a0_data_multicast_attach(
    int unit);

#endif 



static shr_error_e
dnx_data_multicast_params_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "params";
    submodule_data->doc = "values for different multicast groups";
    
    submodule_data->nof_features = _dnx_data_multicast_params_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data multicast params features");

    submodule_data->features[dnx_data_multicast_params_mcdb_formats_v2].name = "mcdb_formats_v2";
    submodule_data->features[dnx_data_multicast_params_mcdb_formats_v2].doc = "New (version 2) MCDB formats used.";
    submodule_data->features[dnx_data_multicast_params_mcdb_formats_v2].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_multicast_params_mc_pruning_actions_not_supported].name = "mc_pruning_actions_not_supported";
    submodule_data->features[dnx_data_multicast_params_mc_pruning_actions_not_supported].doc = "Indicates if mc pruning trap actions are supported: REPLICATION_FAILOVER_DIS, FALLBACK_TO_BRIDGE_EN, NETWORK_GROUP_EN, SAME_INTERFACE_EN";
    submodule_data->features[dnx_data_multicast_params_mc_pruning_actions_not_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_multicast_params_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data multicast params defines");

    submodule_data->defines[dnx_data_multicast_params_define_max_ing_mc_groups].name = "max_ing_mc_groups";
    submodule_data->defines[dnx_data_multicast_params_define_max_ing_mc_groups].doc = "maximum ingress multicast groups id";
    
    submodule_data->defines[dnx_data_multicast_params_define_max_ing_mc_groups].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_multicast_params_define_max_egr_mc_groups].name = "max_egr_mc_groups";
    submodule_data->defines[dnx_data_multicast_params_define_max_egr_mc_groups].doc = "maximum egress multicast groups id";
    
    submodule_data->defines[dnx_data_multicast_params_define_max_egr_mc_groups].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_multicast_params_define_nof_ing_mc_bitmaps].name = "nof_ing_mc_bitmaps";
    submodule_data->defines[dnx_data_multicast_params_define_nof_ing_mc_bitmaps].doc = "number of multicast bitmap groups ids";
    
    submodule_data->defines[dnx_data_multicast_params_define_nof_ing_mc_bitmaps].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_multicast_params_define_nof_egr_mc_bitmaps].name = "nof_egr_mc_bitmaps";
    submodule_data->defines[dnx_data_multicast_params_define_nof_egr_mc_bitmaps].doc = "number of multicast bitmap groups ids";
    
    submodule_data->defines[dnx_data_multicast_params_define_nof_egr_mc_bitmaps].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_multicast_params_define_mcdb_allocation_method].name = "mcdb_allocation_method";
    submodule_data->defines[dnx_data_multicast_params_define_mcdb_allocation_method].doc = "The method of mcdb allocation";
    
    submodule_data->defines[dnx_data_multicast_params_define_mcdb_allocation_method].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_multicast_params_define_nof_mcdb_entries_bits].name = "nof_mcdb_entries_bits";
    submodule_data->defines[dnx_data_multicast_params_define_nof_mcdb_entries_bits].doc = "number of multicast entries in bits";
    
    submodule_data->defines[dnx_data_multicast_params_define_nof_mcdb_entries_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_multicast_params_define_ingr_dest_bits].name = "ingr_dest_bits";
    submodule_data->defines[dnx_data_multicast_params_define_ingr_dest_bits].doc = "Ingress Destination number of bits";
    
    submodule_data->defines[dnx_data_multicast_params_define_ingr_dest_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_multicast_params_define_max_static_element_id].name = "max_static_element_id";
    submodule_data->defines[dnx_data_multicast_params_define_max_static_element_id].doc = "Max MC ID for static MC elements (Group/Bitmap/Element). Valid only for new (version 2) MCDB formats";
    
    submodule_data->defines[dnx_data_multicast_params_define_max_static_element_id].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_multicast_params_define_mcdb_block_size].name = "mcdb_block_size";
    submodule_data->defines[dnx_data_multicast_params_define_mcdb_block_size].doc = "number of multicast entries in block";
    
    submodule_data->defines[dnx_data_multicast_params_define_mcdb_block_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_params_define_nof_mcdb_entries].name = "nof_mcdb_entries";
    submodule_data->defines[dnx_data_multicast_params_define_nof_mcdb_entries].doc = "number of multicast entries";
    
    submodule_data->defines[dnx_data_multicast_params_define_nof_mcdb_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_params_define_max_nof_ingress_bmp_dest].name = "max_nof_ingress_bmp_dest";
    submodule_data->defines[dnx_data_multicast_params_define_max_nof_ingress_bmp_dest].doc = "Max number of ingress bitmap destinations. Valid only for new (version 2) MCDB formats.";
    
    submodule_data->defines[dnx_data_multicast_params_define_max_nof_ingress_bmp_dest].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_multicast_params_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data multicast params tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_multicast_params_feature_get(
    int unit,
    dnx_data_multicast_params_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_params, feature);
}


int
dnx_data_multicast_params_max_ing_mc_groups_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_params, dnx_data_multicast_params_define_max_ing_mc_groups);
}

int
dnx_data_multicast_params_max_egr_mc_groups_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_params, dnx_data_multicast_params_define_max_egr_mc_groups);
}

uint32
dnx_data_multicast_params_nof_ing_mc_bitmaps_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_params, dnx_data_multicast_params_define_nof_ing_mc_bitmaps);
}

uint32
dnx_data_multicast_params_nof_egr_mc_bitmaps_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_params, dnx_data_multicast_params_define_nof_egr_mc_bitmaps);
}

uint32
dnx_data_multicast_params_mcdb_allocation_method_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_params, dnx_data_multicast_params_define_mcdb_allocation_method);
}

uint32
dnx_data_multicast_params_nof_mcdb_entries_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_params, dnx_data_multicast_params_define_nof_mcdb_entries_bits);
}

uint32
dnx_data_multicast_params_ingr_dest_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_params, dnx_data_multicast_params_define_ingr_dest_bits);
}

uint32
dnx_data_multicast_params_max_static_element_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_params, dnx_data_multicast_params_define_max_static_element_id);
}

uint32
dnx_data_multicast_params_mcdb_block_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_params, dnx_data_multicast_params_define_mcdb_block_size);
}

uint32
dnx_data_multicast_params_nof_mcdb_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_params, dnx_data_multicast_params_define_nof_mcdb_entries);
}

uint32
dnx_data_multicast_params_max_nof_ingress_bmp_dest_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_params, dnx_data_multicast_params_define_max_nof_ingress_bmp_dest);
}










static shr_error_e
dnx_data_multicast_mcdb_hash_table_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "mcdb_hash_table";
    submodule_data->doc = "Values related to the MCDB entry ID tash table.";
    
    submodule_data->nof_features = _dnx_data_multicast_mcdb_hash_table_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data multicast mcdb_hash_table features");

    
    submodule_data->nof_defines = _dnx_data_multicast_mcdb_hash_table_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data multicast mcdb_hash_table defines");

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_max_key_size_in_words].name = "max_key_size_in_words";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_max_key_size_in_words].doc = "Size of key for MCDB entry ID hash table in bytes.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_max_key_size_in_words].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_destination_start_bit].name = "destination_start_bit";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_destination_start_bit].doc = "Destination start bit in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_destination_start_bit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_destination_nof_bits].name = "destination_nof_bits";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_destination_nof_bits].doc = "Destination number of bits in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_destination_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_reference_num_start_bit].name = "reference_num_start_bit";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_reference_num_start_bit].doc = "Reference number start bit in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_reference_num_start_bit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_reference_num_nof_bits].name = "reference_num_nof_bits";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_reference_num_nof_bits].doc = "Reference number number of bits in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_reference_num_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_destination_type_start_bit].name = "destination_type_start_bit";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_destination_type_start_bit].doc = "Destination type start bit in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_destination_type_start_bit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_destination_type_nof_bits].name = "destination_type_nof_bits";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_destination_type_nof_bits].doc = "Destination type number of bits in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_destination_type_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_info_start_bit].name = "info_start_bit";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_info_start_bit].doc = "Info (Encapsulation ID or Failover ID) start bit in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_info_start_bit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_info_nof_bits].name = "info_nof_bits";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_info_nof_bits].doc = "Info (Encapsulation ID or Failover ID) number of bits in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_info_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_info_type_start_bit].name = "info_type_start_bit";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_info_type_start_bit].doc = "Info type start bit in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_info_type_start_bit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_info_type_nof_bits].name = "info_type_nof_bits";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_info_type_nof_bits].doc = "Info type number of bits in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_info_type_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_group_id_start_bit].name = "group_id_start_bit";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_group_id_start_bit].doc = "Group ID start bit in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_group_id_start_bit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_group_id_nof_bits].name = "group_id_nof_bits";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_group_id_nof_bits].doc = "Group ID number of bits in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_group_id_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_is_ingress_start_bit].name = "is_ingress_start_bit";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_is_ingress_start_bit].doc = "Is Ingress indication start bit in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_is_ingress_start_bit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_is_ingress_nof_bits].name = "is_ingress_nof_bits";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_is_ingress_nof_bits].doc = "Is Ingress indication number of bits in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_is_ingress_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_core_id_start_bit].name = "core_id_start_bit";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_core_id_start_bit].doc = "Core ID start bit in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_core_id_start_bit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_core_id_nof_bits].name = "core_id_nof_bits";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_core_id_nof_bits].doc = "Core ID number of bits in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_core_id_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_destination2_start_bit].name = "destination2_start_bit";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_destination2_start_bit].doc = "Second destination start bit in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_destination2_start_bit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_destination2_nof_bits].name = "destination2_nof_bits";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_destination2_nof_bits].doc = "Second destination number of bits in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_destination2_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_destination2_type_start_bit].name = "destination2_type_start_bit";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_destination2_type_start_bit].doc = "Second destination type start bit in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_destination2_type_start_bit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_destination2_type_nof_bits].name = "destination2_type_nof_bits";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_destination2_type_nof_bits].doc = "Second destination type number of bits in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_destination2_type_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_same_if_dspa_valid_start_bit].name = "same_if_dspa_valid_start_bit";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_same_if_dspa_valid_start_bit].doc = "Same Interface DSPA start bit in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_same_if_dspa_valid_start_bit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_same_if_dspa_valid_nof_bits].name = "same_if_dspa_valid_nof_bits";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_same_if_dspa_valid_nof_bits].doc = "Same Interface DSPA number of bits in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_same_if_dspa_valid_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_same_if_dspa_start_bit].name = "same_if_dspa_start_bit";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_same_if_dspa_start_bit].doc = "Same Interface DSPA valid start bit in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_same_if_dspa_start_bit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_same_if_dspa_nof_bits].name = "same_if_dspa_nof_bits";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_same_if_dspa_nof_bits].doc = "Same Interface DSPA valid number of bits in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_same_if_dspa_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_same_if_outlif_valid_start_bit].name = "same_if_outlif_valid_start_bit";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_same_if_outlif_valid_start_bit].doc = "Same Interface OutLIF valid start bit in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_same_if_outlif_valid_start_bit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_same_if_outlif_valid_nof_bits].name = "same_if_outlif_valid_nof_bits";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_same_if_outlif_valid_nof_bits].doc = "Same Interface OutLIF valid number of bits in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_same_if_outlif_valid_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_same_if_outlif_start_bit].name = "same_if_outlif_start_bit";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_same_if_outlif_start_bit].doc = "Same Interface OutLIF start bit in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_same_if_outlif_start_bit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_same_if_outlif_nof_bits].name = "same_if_outlif_nof_bits";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_same_if_outlif_nof_bits].doc = "Same Interface OutLIF number of bits in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_same_if_outlif_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_forward_group_valid_start_bit].name = "forward_group_valid_start_bit";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_forward_group_valid_start_bit].doc = "Forward Group valid start bit in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_forward_group_valid_start_bit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_forward_group_valid_nof_bits].name = "forward_group_valid_nof_bits";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_forward_group_valid_nof_bits].doc = "Forward Group valid number of bits in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_forward_group_valid_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_forward_group_start_bit].name = "forward_group_start_bit";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_forward_group_start_bit].doc = "Forward Group start bit in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_forward_group_start_bit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_forward_group_nof_bits].name = "forward_group_nof_bits";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_forward_group_nof_bits].doc = "Forward Group number of bits in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_forward_group_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_is_static_start_bit].name = "is_static_start_bit";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_is_static_start_bit].doc = "Is Ingress indication start bit in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_is_static_start_bit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_is_static_nof_bits].name = "is_static_nof_bits";
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_is_static_nof_bits].doc = "Is Ingress indication number of bits in the key for MCDB entry ID hash table.";
    
    submodule_data->defines[dnx_data_multicast_mcdb_hash_table_define_is_static_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_multicast_mcdb_hash_table_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data multicast mcdb_hash_table tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_multicast_mcdb_hash_table_feature_get(
    int unit,
    dnx_data_multicast_mcdb_hash_table_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, feature);
}


uint32
dnx_data_multicast_mcdb_hash_table_max_key_size_in_words_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_max_key_size_in_words);
}

uint32
dnx_data_multicast_mcdb_hash_table_destination_start_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_destination_start_bit);
}

uint32
dnx_data_multicast_mcdb_hash_table_destination_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_destination_nof_bits);
}

uint32
dnx_data_multicast_mcdb_hash_table_reference_num_start_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_reference_num_start_bit);
}

uint32
dnx_data_multicast_mcdb_hash_table_reference_num_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_reference_num_nof_bits);
}

uint32
dnx_data_multicast_mcdb_hash_table_destination_type_start_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_destination_type_start_bit);
}

uint32
dnx_data_multicast_mcdb_hash_table_destination_type_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_destination_type_nof_bits);
}

uint32
dnx_data_multicast_mcdb_hash_table_info_start_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_info_start_bit);
}

uint32
dnx_data_multicast_mcdb_hash_table_info_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_info_nof_bits);
}

uint32
dnx_data_multicast_mcdb_hash_table_info_type_start_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_info_type_start_bit);
}

uint32
dnx_data_multicast_mcdb_hash_table_info_type_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_info_type_nof_bits);
}

uint32
dnx_data_multicast_mcdb_hash_table_group_id_start_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_group_id_start_bit);
}

uint32
dnx_data_multicast_mcdb_hash_table_group_id_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_group_id_nof_bits);
}

uint32
dnx_data_multicast_mcdb_hash_table_is_ingress_start_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_is_ingress_start_bit);
}

uint32
dnx_data_multicast_mcdb_hash_table_is_ingress_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_is_ingress_nof_bits);
}

uint32
dnx_data_multicast_mcdb_hash_table_core_id_start_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_core_id_start_bit);
}

uint32
dnx_data_multicast_mcdb_hash_table_core_id_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_core_id_nof_bits);
}

uint32
dnx_data_multicast_mcdb_hash_table_destination2_start_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_destination2_start_bit);
}

uint32
dnx_data_multicast_mcdb_hash_table_destination2_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_destination2_nof_bits);
}

uint32
dnx_data_multicast_mcdb_hash_table_destination2_type_start_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_destination2_type_start_bit);
}

uint32
dnx_data_multicast_mcdb_hash_table_destination2_type_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_destination2_type_nof_bits);
}

uint32
dnx_data_multicast_mcdb_hash_table_same_if_dspa_valid_start_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_same_if_dspa_valid_start_bit);
}

uint32
dnx_data_multicast_mcdb_hash_table_same_if_dspa_valid_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_same_if_dspa_valid_nof_bits);
}

uint32
dnx_data_multicast_mcdb_hash_table_same_if_dspa_start_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_same_if_dspa_start_bit);
}

uint32
dnx_data_multicast_mcdb_hash_table_same_if_dspa_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_same_if_dspa_nof_bits);
}

uint32
dnx_data_multicast_mcdb_hash_table_same_if_outlif_valid_start_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_same_if_outlif_valid_start_bit);
}

uint32
dnx_data_multicast_mcdb_hash_table_same_if_outlif_valid_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_same_if_outlif_valid_nof_bits);
}

uint32
dnx_data_multicast_mcdb_hash_table_same_if_outlif_start_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_same_if_outlif_start_bit);
}

uint32
dnx_data_multicast_mcdb_hash_table_same_if_outlif_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_same_if_outlif_nof_bits);
}

uint32
dnx_data_multicast_mcdb_hash_table_forward_group_valid_start_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_forward_group_valid_start_bit);
}

uint32
dnx_data_multicast_mcdb_hash_table_forward_group_valid_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_forward_group_valid_nof_bits);
}

uint32
dnx_data_multicast_mcdb_hash_table_forward_group_start_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_forward_group_start_bit);
}

uint32
dnx_data_multicast_mcdb_hash_table_forward_group_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_forward_group_nof_bits);
}

uint32
dnx_data_multicast_mcdb_hash_table_is_static_start_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_is_static_start_bit);
}

uint32
dnx_data_multicast_mcdb_hash_table_is_static_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_mcdb_hash_table, dnx_data_multicast_mcdb_hash_table_define_is_static_nof_bits);
}







shr_error_e
dnx_data_multicast_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "multicast";
    module_data->nof_submodules = _dnx_data_multicast_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data multicast submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_multicast_params_init(unit, &module_data->submodules[dnx_data_multicast_submodule_params]));
    SHR_IF_ERR_EXIT(dnx_data_multicast_mcdb_hash_table_init(unit, &module_data->submodules[dnx_data_multicast_submodule_mcdb_hash_table]));
    
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_multicast_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_multicast_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_multicast_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_multicast_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_multicast_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_multicast_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_multicast_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_multicast_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_multicast_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_multicast_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_multicast_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_multicast_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_multicast_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_multicast_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_multicast_attach(unit));
    }
    else

#endif 
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

