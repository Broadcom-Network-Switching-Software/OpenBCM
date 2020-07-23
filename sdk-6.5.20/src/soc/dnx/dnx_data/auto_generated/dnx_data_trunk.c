

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TRUNK

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_trunk.h>



extern shr_error_e jr2_a0_data_trunk_attach(
    int unit);
extern shr_error_e j2c_a0_data_trunk_attach(
    int unit);
extern shr_error_e q2a_a0_data_trunk_attach(
    int unit);
extern shr_error_e j2p_a0_data_trunk_attach(
    int unit);



static shr_error_e
dnx_data_trunk_parameters_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "parameters";
    submodule_data->doc = "Trunk parameters and defines";
    
    submodule_data->nof_features = _dnx_data_trunk_parameters_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data trunk parameters features");

    
    submodule_data->nof_defines = _dnx_data_trunk_parameters_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data trunk parameters defines");

    submodule_data->defines[dnx_data_trunk_parameters_define_nof_pools].name = "nof_pools";
    submodule_data->defines[dnx_data_trunk_parameters_define_nof_pools].doc = "trunk module might be represented with pools in certain architectures, this number represents the number of pools currently supported for this device. each pool has a constant amount of members.";
    
    submodule_data->defines[dnx_data_trunk_parameters_define_nof_pools].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_trunk_parameters_define_max_nof_members_in_pool].name = "max_nof_members_in_pool";
    submodule_data->defines[dnx_data_trunk_parameters_define_max_nof_members_in_pool].doc = "the maximal amount of members that a pool can contain.";
    
    submodule_data->defines[dnx_data_trunk_parameters_define_max_nof_members_in_pool].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_trunk_parameters_define_max_nof_groups_in_pool].name = "max_nof_groups_in_pool";
    submodule_data->defines[dnx_data_trunk_parameters_define_max_nof_groups_in_pool].doc = "the maximal amount of groups that a pool can contain.";
    
    submodule_data->defines[dnx_data_trunk_parameters_define_max_nof_groups_in_pool].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_trunk_parameters_define_spa_pool_shift].name = "spa_pool_shift";
    submodule_data->defines[dnx_data_trunk_parameters_define_spa_pool_shift].doc = "bit shifts to describe the pool of a trunk in a System Port Aggregate";
    
    submodule_data->defines[dnx_data_trunk_parameters_define_spa_pool_shift].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_trunk_parameters_define_spa_type_shift].name = "spa_type_shift";
    submodule_data->defines[dnx_data_trunk_parameters_define_spa_type_shift].doc = "bit shifts to describe trunk type in a System Port Aggregate";
    
    submodule_data->defines[dnx_data_trunk_parameters_define_spa_type_shift].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_trunk_parameters_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data trunk parameters tables");

    
    submodule_data->tables[dnx_data_trunk_parameters_table_pool_info].name = "pool_info";
    submodule_data->tables[dnx_data_trunk_parameters_table_pool_info].doc = "this table contains the information per pool of trunks";
    submodule_data->tables[dnx_data_trunk_parameters_table_pool_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_trunk_parameters_table_pool_info].size_of_values = sizeof(dnx_data_trunk_parameters_pool_info_t);
    submodule_data->tables[dnx_data_trunk_parameters_table_pool_info].entry_get = dnx_data_trunk_parameters_pool_info_entry_str_get;

    
    submodule_data->tables[dnx_data_trunk_parameters_table_pool_info].nof_keys = 1;
    submodule_data->tables[dnx_data_trunk_parameters_table_pool_info].keys[0].name = "pool_index";
    submodule_data->tables[dnx_data_trunk_parameters_table_pool_info].keys[0].doc = "pool index";

    
    submodule_data->tables[dnx_data_trunk_parameters_table_pool_info].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_trunk_parameters_table_pool_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_trunk_parameters_table_pool_info].nof_values, "_dnx_data_trunk_parameters_table_pool_info table values");
    submodule_data->tables[dnx_data_trunk_parameters_table_pool_info].values[0].name = "max_nof_members_in_group";
    submodule_data->tables[dnx_data_trunk_parameters_table_pool_info].values[0].type = "uint32";
    submodule_data->tables[dnx_data_trunk_parameters_table_pool_info].values[0].doc = "maxmimal number of members in a group in this pool";
    submodule_data->tables[dnx_data_trunk_parameters_table_pool_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_trunk_parameters_pool_info_t, max_nof_members_in_group);
    submodule_data->tables[dnx_data_trunk_parameters_table_pool_info].values[1].name = "max_nof_groups_in_pool";
    submodule_data->tables[dnx_data_trunk_parameters_table_pool_info].values[1].type = "uint32";
    submodule_data->tables[dnx_data_trunk_parameters_table_pool_info].values[1].doc = "maximal number of groups in this pool";
    submodule_data->tables[dnx_data_trunk_parameters_table_pool_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_trunk_parameters_pool_info_t, max_nof_groups_in_pool);
    submodule_data->tables[dnx_data_trunk_parameters_table_pool_info].values[2].name = "pool_hw_mode";
    submodule_data->tables[dnx_data_trunk_parameters_table_pool_info].values[2].type = "int";
    submodule_data->tables[dnx_data_trunk_parameters_table_pool_info].values[2].doc = "mapping to the actual value the HW needs to get to represent given pool mode";
    submodule_data->tables[dnx_data_trunk_parameters_table_pool_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_trunk_parameters_pool_info_t, pool_hw_mode);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_trunk_parameters_feature_get(
    int unit,
    dnx_data_trunk_parameters_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_trunk, dnx_data_trunk_submodule_parameters, feature);
}


uint32
dnx_data_trunk_parameters_nof_pools_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_trunk, dnx_data_trunk_submodule_parameters, dnx_data_trunk_parameters_define_nof_pools);
}

uint32
dnx_data_trunk_parameters_max_nof_members_in_pool_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_trunk, dnx_data_trunk_submodule_parameters, dnx_data_trunk_parameters_define_max_nof_members_in_pool);
}

uint32
dnx_data_trunk_parameters_max_nof_groups_in_pool_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_trunk, dnx_data_trunk_submodule_parameters, dnx_data_trunk_parameters_define_max_nof_groups_in_pool);
}

uint32
dnx_data_trunk_parameters_spa_pool_shift_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_trunk, dnx_data_trunk_submodule_parameters, dnx_data_trunk_parameters_define_spa_pool_shift);
}

uint32
dnx_data_trunk_parameters_spa_type_shift_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_trunk, dnx_data_trunk_submodule_parameters, dnx_data_trunk_parameters_define_spa_type_shift);
}



const dnx_data_trunk_parameters_pool_info_t *
dnx_data_trunk_parameters_pool_info_get(
    int unit,
    int pool_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_trunk, dnx_data_trunk_submodule_parameters, dnx_data_trunk_parameters_table_pool_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, pool_index, 0);
    return (const dnx_data_trunk_parameters_pool_info_t *) data;

}


shr_error_e
dnx_data_trunk_parameters_pool_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_trunk_parameters_pool_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_trunk, dnx_data_trunk_submodule_parameters, dnx_data_trunk_parameters_table_pool_info);
    data = (const dnx_data_trunk_parameters_pool_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->max_nof_members_in_group);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->max_nof_groups_in_pool);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pool_hw_mode);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_trunk_parameters_pool_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_trunk, dnx_data_trunk_submodule_parameters, dnx_data_trunk_parameters_table_pool_info);

}






static shr_error_e
dnx_data_trunk_psc_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "psc";
    submodule_data->doc = "Trunk Port Selection Criteria (PSC) Data";
    
    submodule_data->nof_features = _dnx_data_trunk_psc_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data trunk psc features");

    submodule_data->features[dnx_data_trunk_psc_multiply_and_divide].name = "multiply_and_divide";
    submodule_data->features[dnx_data_trunk_psc_multiply_and_divide].doc = "is Multiply and Divide PSC supported";
    submodule_data->features[dnx_data_trunk_psc_multiply_and_divide].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_trunk_psc_smooth_division].name = "smooth_division";
    submodule_data->features[dnx_data_trunk_psc_smooth_division].doc = "is Smooth Division PSC supported";
    submodule_data->features[dnx_data_trunk_psc_smooth_division].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_trunk_psc_consistant_hashing].name = "consistant_hashing";
    submodule_data->features[dnx_data_trunk_psc_consistant_hashing].doc = "is Consistant Hashing PSC supported";
    submodule_data->features[dnx_data_trunk_psc_consistant_hashing].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_trunk_psc_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data trunk psc defines");

    submodule_data->defines[dnx_data_trunk_psc_define_consistant_hashing_small_group_size_in_bits].name = "consistant_hashing_small_group_size_in_bits";
    submodule_data->defines[dnx_data_trunk_psc_define_consistant_hashing_small_group_size_in_bits].doc = "size in bits of small groups in c-lag";
    
    submodule_data->defines[dnx_data_trunk_psc_define_consistant_hashing_small_group_size_in_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_trunk_psc_define_smooth_division_max_nof_member].name = "smooth_division_max_nof_member";
    submodule_data->defines[dnx_data_trunk_psc_define_smooth_division_max_nof_member].doc = "max number of member is trunk group with smooth division as PSC";
    
    submodule_data->defines[dnx_data_trunk_psc_define_smooth_division_max_nof_member].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_trunk_psc_define_smooth_division_entries_per_profile].name = "smooth_division_entries_per_profile";
    submodule_data->defines[dnx_data_trunk_psc_define_smooth_division_entries_per_profile].doc = "logical number of entries dedicated in the HW per SMD profile";
    
    submodule_data->defines[dnx_data_trunk_psc_define_smooth_division_entries_per_profile].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_trunk_psc_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data trunk psc tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_trunk_psc_feature_get(
    int unit,
    dnx_data_trunk_psc_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_trunk, dnx_data_trunk_submodule_psc, feature);
}


uint32
dnx_data_trunk_psc_consistant_hashing_small_group_size_in_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_trunk, dnx_data_trunk_submodule_psc, dnx_data_trunk_psc_define_consistant_hashing_small_group_size_in_bits);
}

uint32
dnx_data_trunk_psc_smooth_division_max_nof_member_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_trunk, dnx_data_trunk_submodule_psc, dnx_data_trunk_psc_define_smooth_division_max_nof_member);
}

uint32
dnx_data_trunk_psc_smooth_division_entries_per_profile_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_trunk, dnx_data_trunk_submodule_psc, dnx_data_trunk_psc_define_smooth_division_entries_per_profile);
}










static shr_error_e
dnx_data_trunk_egress_trunk_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "egress_trunk";
    submodule_data->doc = "Egress Trunks - for egress MC";
    
    submodule_data->nof_features = _dnx_data_trunk_egress_trunk_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data trunk egress_trunk features");

    submodule_data->features[dnx_data_trunk_egress_trunk_multiple_egress_trunk_sizes].name = "multiple_egress_trunk_sizes";
    submodule_data->features[dnx_data_trunk_egress_trunk_multiple_egress_trunk_sizes].doc = "device is supporting multiple modes for sizes of egress lags";
    submodule_data->features[dnx_data_trunk_egress_trunk_multiple_egress_trunk_sizes].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_trunk_egress_trunk_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data trunk egress_trunk defines");

    submodule_data->defines[dnx_data_trunk_egress_trunk_define_nof].name = "nof";
    submodule_data->defines[dnx_data_trunk_egress_trunk_define_nof].doc = "number of egress trunks";
    
    submodule_data->defines[dnx_data_trunk_egress_trunk_define_nof].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_trunk_egress_trunk_define_nof_lb_keys_per_profile].name = "nof_lb_keys_per_profile";
    submodule_data->defines[dnx_data_trunk_egress_trunk_define_nof_lb_keys_per_profile].doc = "number of lb keys per Egress trunk";
    
    submodule_data->defines[dnx_data_trunk_egress_trunk_define_nof_lb_keys_per_profile].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_trunk_egress_trunk_define_dbal_egress_trunk_index_multiplier].name = "dbal_egress_trunk_index_multiplier";
    submodule_data->defines[dnx_data_trunk_egress_trunk_define_dbal_egress_trunk_index_multiplier].doc = "multiplier size for mapping dbal key EGRESS_TRUNK_INDEX in dbal table TRUNK_EGRESS_MC_RESOLUTION";
    
    submodule_data->defines[dnx_data_trunk_egress_trunk_define_dbal_egress_trunk_index_multiplier].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_trunk_egress_trunk_define_dbal_entry_index_divider].name = "dbal_entry_index_divider";
    submodule_data->defines[dnx_data_trunk_egress_trunk_define_dbal_entry_index_divider].doc = "divider size for mapping dbal key ENTRY_INDEX in dbal table TRUNK_EGRESS_MC_RESOLUTION";
    
    submodule_data->defines[dnx_data_trunk_egress_trunk_define_dbal_entry_index_divider].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_trunk_egress_trunk_define_dbal_entry_index_msb_multiplier].name = "dbal_entry_index_msb_multiplier";
    submodule_data->defines[dnx_data_trunk_egress_trunk_define_dbal_entry_index_msb_multiplier].doc = "multiplier size for mapping dbal key ENTRY_INDEX msb in dbal table TRUNK_EGRESS_MC_RESOLUTION";
    
    submodule_data->defines[dnx_data_trunk_egress_trunk_define_dbal_entry_index_msb_multiplier].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_trunk_egress_trunk_define_size_mode].name = "size_mode";
    submodule_data->defines[dnx_data_trunk_egress_trunk_define_size_mode].doc = "HW mode relevant for group size";
    
    submodule_data->defines[dnx_data_trunk_egress_trunk_define_size_mode].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_trunk_egress_trunk_define_invalid_pp_dsp].name = "invalid_pp_dsp";
    submodule_data->defines[dnx_data_trunk_egress_trunk_define_invalid_pp_dsp].doc = "invalid pp dsp that should be connected to invalid out port for egress trunk";
    
    submodule_data->defines[dnx_data_trunk_egress_trunk_define_invalid_pp_dsp].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_trunk_egress_trunk_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data trunk egress_trunk tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_trunk_egress_trunk_feature_get(
    int unit,
    dnx_data_trunk_egress_trunk_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_trunk, dnx_data_trunk_submodule_egress_trunk, feature);
}


uint32
dnx_data_trunk_egress_trunk_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_trunk, dnx_data_trunk_submodule_egress_trunk, dnx_data_trunk_egress_trunk_define_nof);
}

uint32
dnx_data_trunk_egress_trunk_nof_lb_keys_per_profile_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_trunk, dnx_data_trunk_submodule_egress_trunk, dnx_data_trunk_egress_trunk_define_nof_lb_keys_per_profile);
}

uint32
dnx_data_trunk_egress_trunk_dbal_egress_trunk_index_multiplier_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_trunk, dnx_data_trunk_submodule_egress_trunk, dnx_data_trunk_egress_trunk_define_dbal_egress_trunk_index_multiplier);
}

uint32
dnx_data_trunk_egress_trunk_dbal_entry_index_divider_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_trunk, dnx_data_trunk_submodule_egress_trunk, dnx_data_trunk_egress_trunk_define_dbal_entry_index_divider);
}

uint32
dnx_data_trunk_egress_trunk_dbal_entry_index_msb_multiplier_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_trunk, dnx_data_trunk_submodule_egress_trunk, dnx_data_trunk_egress_trunk_define_dbal_entry_index_msb_multiplier);
}

uint32
dnx_data_trunk_egress_trunk_size_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_trunk, dnx_data_trunk_submodule_egress_trunk, dnx_data_trunk_egress_trunk_define_size_mode);
}

uint32
dnx_data_trunk_egress_trunk_invalid_pp_dsp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_trunk, dnx_data_trunk_submodule_egress_trunk, dnx_data_trunk_egress_trunk_define_invalid_pp_dsp);
}










static shr_error_e
dnx_data_trunk_graceful_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "graceful";
    submodule_data->doc = "data regarding graceful modification of trunks";
    
    submodule_data->nof_features = _dnx_data_trunk_graceful_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data trunk graceful features");

    submodule_data->features[dnx_data_trunk_graceful_allow].name = "allow";
    submodule_data->features[dnx_data_trunk_graceful_allow].doc = "allow changing lag in an atomic action in the device's local scope";
    submodule_data->features[dnx_data_trunk_graceful_allow].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_trunk_graceful_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data trunk graceful defines");

    submodule_data->defines[dnx_data_trunk_graceful_define_first_hw_configuration].name = "first_hw_configuration";
    submodule_data->defines[dnx_data_trunk_graceful_define_first_hw_configuration].doc = "hw value to indicate first configuration is currently used";
    
    submodule_data->defines[dnx_data_trunk_graceful_define_first_hw_configuration].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_trunk_graceful_define_second_hw_configuration].name = "second_hw_configuration";
    submodule_data->defines[dnx_data_trunk_graceful_define_second_hw_configuration].doc = "hw value to indicate second configuration is currently used";
    
    submodule_data->defines[dnx_data_trunk_graceful_define_second_hw_configuration].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_trunk_graceful_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data trunk graceful tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_trunk_graceful_feature_get(
    int unit,
    dnx_data_trunk_graceful_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_trunk, dnx_data_trunk_submodule_graceful, feature);
}


uint32
dnx_data_trunk_graceful_first_hw_configuration_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_trunk, dnx_data_trunk_submodule_graceful, dnx_data_trunk_graceful_define_first_hw_configuration);
}

uint32
dnx_data_trunk_graceful_second_hw_configuration_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_trunk, dnx_data_trunk_submodule_graceful, dnx_data_trunk_graceful_define_second_hw_configuration);
}







shr_error_e
dnx_data_trunk_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "trunk";
    module_data->nof_submodules = _dnx_data_trunk_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data trunk submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_trunk_parameters_init(unit, &module_data->submodules[dnx_data_trunk_submodule_parameters]));
    SHR_IF_ERR_EXIT(dnx_data_trunk_psc_init(unit, &module_data->submodules[dnx_data_trunk_submodule_psc]));
    SHR_IF_ERR_EXIT(dnx_data_trunk_egress_trunk_init(unit, &module_data->submodules[dnx_data_trunk_submodule_egress_trunk]));
    SHR_IF_ERR_EXIT(dnx_data_trunk_graceful_init(unit, &module_data->submodules[dnx_data_trunk_submodule_graceful]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_trunk_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_trunk_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_trunk_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_trunk_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_trunk_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_trunk_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_trunk_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_trunk_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_trunk_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_trunk_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_trunk_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_trunk_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_trunk_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_trunk_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_trunk_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

