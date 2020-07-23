

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_system_red.h>



extern shr_error_e jr2_a0_data_system_red_attach(
    int unit);
extern shr_error_e jr2_b0_data_system_red_attach(
    int unit);
extern shr_error_e j2c_a0_data_system_red_attach(
    int unit);
extern shr_error_e q2a_a0_data_system_red_attach(
    int unit);
extern shr_error_e j2p_a0_data_system_red_attach(
    int unit);



static shr_error_e
dnx_data_system_red_config_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "config";
    submodule_data->doc = "system red configuration";
    
    submodule_data->nof_features = _dnx_data_system_red_config_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data system_red config features");

    
    submodule_data->nof_defines = _dnx_data_system_red_config_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data system_red config defines");

    submodule_data->defines[dnx_data_system_red_config_define_enable].name = "enable";
    submodule_data->defines[dnx_data_system_red_config_define_enable].doc = "system RED enable configuration";
    
    submodule_data->defines[dnx_data_system_red_config_define_enable].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_system_red_config_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data system_red config tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_system_red_config_feature_get(
    int unit,
    dnx_data_system_red_config_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_system_red, dnx_data_system_red_submodule_config, feature);
}


uint32
dnx_data_system_red_config_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_system_red, dnx_data_system_red_submodule_config, dnx_data_system_red_config_define_enable);
}










static shr_error_e
dnx_data_system_red_info_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "info";
    submodule_data->doc = "system red related info";
    
    submodule_data->nof_features = _dnx_data_system_red_info_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data system_red info features");

    submodule_data->features[dnx_data_system_red_info_is_share_res_with_latency_based_admission].name = "is_share_res_with_latency_based_admission";
    submodule_data->features[dnx_data_system_red_info_is_share_res_with_latency_based_admission].doc = "indication whether latency based admission and system RED share the same HW resources (hence mutually exclusive)";
    submodule_data->features[dnx_data_system_red_info_is_share_res_with_latency_based_admission].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_system_red_info_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data system_red info defines");

    submodule_data->defines[dnx_data_system_red_info_define_nof_red_q_size].name = "nof_red_q_size";
    submodule_data->defines[dnx_data_system_red_info_define_nof_red_q_size].doc = "number of system red RED-Q-Size values.";
    
    submodule_data->defines[dnx_data_system_red_info_define_nof_red_q_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_system_red_info_define_max_red_q_size].name = "max_red_q_size";
    submodule_data->defines[dnx_data_system_red_info_define_max_red_q_size].doc = "max value for system red RED-Q-Size. this is the value used to determoine system level congestion level";
    
    submodule_data->defines[dnx_data_system_red_info_define_max_red_q_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_system_red_info_define_max_ing_aging_period].name = "max_ing_aging_period";
    submodule_data->defines[dnx_data_system_red_info_define_max_ing_aging_period].doc = "max supported aging period on ingress side in msec";
    
    submodule_data->defines[dnx_data_system_red_info_define_max_ing_aging_period].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_system_red_info_define_max_sch_aging_period].name = "max_sch_aging_period";
    submodule_data->defines[dnx_data_system_red_info_define_max_sch_aging_period].doc = "max supported aging period on scheduler side in msec";
    
    submodule_data->defines[dnx_data_system_red_info_define_max_sch_aging_period].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_system_red_info_define_sch_aging_period_factor].name = "sch_aging_period_factor";
    submodule_data->defines[dnx_data_system_red_info_define_sch_aging_period_factor].doc = "factor (in clocks) for sch aging period calculation: aging_period = nof_ports * factor * hw_value";
    
    submodule_data->defines[dnx_data_system_red_info_define_sch_aging_period_factor].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_system_red_info_define_max_free_res_thr_range].name = "max_free_res_thr_range";
    submodule_data->defines[dnx_data_system_red_info_define_max_free_res_thr_range].doc = "max range id for system RED free resources thresholds.";
    
    submodule_data->defines[dnx_data_system_red_info_define_max_free_res_thr_range].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_system_red_info_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data system_red info tables");

    
    submodule_data->tables[dnx_data_system_red_info_table_resource].name = "resource";
    submodule_data->tables[dnx_data_system_red_info_table_resource].doc = "system red tracked resources";
    submodule_data->tables[dnx_data_system_red_info_table_resource].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_system_red_info_table_resource].size_of_values = sizeof(dnx_data_system_red_info_resource_t);
    submodule_data->tables[dnx_data_system_red_info_table_resource].entry_get = dnx_data_system_red_info_resource_entry_str_get;

    
    submodule_data->tables[dnx_data_system_red_info_table_resource].nof_keys = 1;
    submodule_data->tables[dnx_data_system_red_info_table_resource].keys[0].name = "type";
    submodule_data->tables[dnx_data_system_red_info_table_resource].keys[0].doc = "resource type";

    
    submodule_data->tables[dnx_data_system_red_info_table_resource].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_system_red_info_table_resource].values, dnxc_data_value_t, submodule_data->tables[dnx_data_system_red_info_table_resource].nof_values, "_dnx_data_system_red_info_table_resource table values");
    submodule_data->tables[dnx_data_system_red_info_table_resource].values[0].name = "max";
    submodule_data->tables[dnx_data_system_red_info_table_resource].values[0].type = "uint32";
    submodule_data->tables[dnx_data_system_red_info_table_resource].values[0].doc = "Maximal resource size";
    submodule_data->tables[dnx_data_system_red_info_table_resource].values[0].offset = UTILEX_OFFSETOF(dnx_data_system_red_info_resource_t, max);
    submodule_data->tables[dnx_data_system_red_info_table_resource].values[1].name = "hw_resolution_max";
    submodule_data->tables[dnx_data_system_red_info_table_resource].values[1].type = "uint32";
    submodule_data->tables[dnx_data_system_red_info_table_resource].values[1].doc = "Maximal resource size in HW granularity";
    submodule_data->tables[dnx_data_system_red_info_table_resource].values[1].offset = UTILEX_OFFSETOF(dnx_data_system_red_info_resource_t, hw_resolution_max);
    submodule_data->tables[dnx_data_system_red_info_table_resource].values[2].name = "hw_resolution_nof_bits";
    submodule_data->tables[dnx_data_system_red_info_table_resource].values[2].type = "uint32";
    submodule_data->tables[dnx_data_system_red_info_table_resource].values[2].doc = "Maximal nof bits to describe resource size in HW granularity";
    submodule_data->tables[dnx_data_system_red_info_table_resource].values[2].offset = UTILEX_OFFSETOF(dnx_data_system_red_info_resource_t, hw_resolution_nof_bits);

    
    submodule_data->tables[dnx_data_system_red_info_table_dbal].name = "dbal";
    submodule_data->tables[dnx_data_system_red_info_table_dbal].doc = "system red tracked resources";
    submodule_data->tables[dnx_data_system_red_info_table_dbal].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_system_red_info_table_dbal].size_of_values = sizeof(dnx_data_system_red_info_dbal_t);
    submodule_data->tables[dnx_data_system_red_info_table_dbal].entry_get = dnx_data_system_red_info_dbal_entry_str_get;

    
    submodule_data->tables[dnx_data_system_red_info_table_dbal].nof_keys = 1;
    submodule_data->tables[dnx_data_system_red_info_table_dbal].keys[0].name = "type";
    submodule_data->tables[dnx_data_system_red_info_table_dbal].keys[0].doc = "resource type";

    
    submodule_data->tables[dnx_data_system_red_info_table_dbal].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_system_red_info_table_dbal].values, dnxc_data_value_t, submodule_data->tables[dnx_data_system_red_info_table_dbal].nof_values, "_dnx_data_system_red_info_table_dbal table values");
    submodule_data->tables[dnx_data_system_red_info_table_dbal].values[0].name = "offset";
    submodule_data->tables[dnx_data_system_red_info_table_dbal].values[0].type = "uint32";
    submodule_data->tables[dnx_data_system_red_info_table_dbal].values[0].doc = "Offset of the first field for the type";
    submodule_data->tables[dnx_data_system_red_info_table_dbal].values[0].offset = UTILEX_OFFSETOF(dnx_data_system_red_info_dbal_t, offset);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_system_red_info_feature_get(
    int unit,
    dnx_data_system_red_info_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_system_red, dnx_data_system_red_submodule_info, feature);
}


uint32
dnx_data_system_red_info_nof_red_q_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_system_red, dnx_data_system_red_submodule_info, dnx_data_system_red_info_define_nof_red_q_size);
}

uint32
dnx_data_system_red_info_max_red_q_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_system_red, dnx_data_system_red_submodule_info, dnx_data_system_red_info_define_max_red_q_size);
}

uint32
dnx_data_system_red_info_max_ing_aging_period_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_system_red, dnx_data_system_red_submodule_info, dnx_data_system_red_info_define_max_ing_aging_period);
}

uint32
dnx_data_system_red_info_max_sch_aging_period_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_system_red, dnx_data_system_red_submodule_info, dnx_data_system_red_info_define_max_sch_aging_period);
}

uint32
dnx_data_system_red_info_sch_aging_period_factor_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_system_red, dnx_data_system_red_submodule_info, dnx_data_system_red_info_define_sch_aging_period_factor);
}

uint32
dnx_data_system_red_info_max_free_res_thr_range_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_system_red, dnx_data_system_red_submodule_info, dnx_data_system_red_info_define_max_free_res_thr_range);
}



const dnx_data_system_red_info_resource_t *
dnx_data_system_red_info_resource_get(
    int unit,
    int type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_system_red, dnx_data_system_red_submodule_info, dnx_data_system_red_info_table_resource);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, type, 0);
    return (const dnx_data_system_red_info_resource_t *) data;

}

const dnx_data_system_red_info_dbal_t *
dnx_data_system_red_info_dbal_get(
    int unit,
    int type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_system_red, dnx_data_system_red_submodule_info, dnx_data_system_red_info_table_dbal);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, type, 0);
    return (const dnx_data_system_red_info_dbal_t *) data;

}


shr_error_e
dnx_data_system_red_info_resource_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_system_red_info_resource_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_system_red, dnx_data_system_red_submodule_info, dnx_data_system_red_info_table_resource);
    data = (const dnx_data_system_red_info_resource_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->max);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->hw_resolution_max);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->hw_resolution_nof_bits);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_system_red_info_dbal_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_system_red_info_dbal_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_system_red, dnx_data_system_red_submodule_info, dnx_data_system_red_info_table_dbal);
    data = (const dnx_data_system_red_info_dbal_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->offset);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_system_red_info_resource_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_system_red, dnx_data_system_red_submodule_info, dnx_data_system_red_info_table_resource);

}

const dnxc_data_table_info_t *
dnx_data_system_red_info_dbal_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_system_red, dnx_data_system_red_submodule_info, dnx_data_system_red_info_table_dbal);

}



shr_error_e
dnx_data_system_red_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "system_red";
    module_data->nof_submodules = _dnx_data_system_red_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data system_red submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_system_red_config_init(unit, &module_data->submodules[dnx_data_system_red_submodule_config]));
    SHR_IF_ERR_EXIT(dnx_data_system_red_info_init(unit, &module_data->submodules[dnx_data_system_red_submodule_info]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_system_red_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_system_red_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_system_red_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_system_red_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_system_red_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_system_red_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_system_red_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_system_red_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_system_red_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_system_red_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_system_red_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_system_red_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_system_red_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_system_red_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_system_red_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_system_red_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_system_red_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

