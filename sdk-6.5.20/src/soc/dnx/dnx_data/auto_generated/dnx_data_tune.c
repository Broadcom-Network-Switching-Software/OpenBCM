

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INITSEQDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_tune.h>



extern shr_error_e jr2_a0_data_tune_attach(
    int unit);
extern shr_error_e j2c_a0_data_tune_attach(
    int unit);
extern shr_error_e q2a_a0_data_tune_attach(
    int unit);
extern shr_error_e j2p_a0_data_tune_attach(
    int unit);



static shr_error_e
dnx_data_tune_ecgm_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ecgm";
    submodule_data->doc = "ECGM tune data";
    
    submodule_data->nof_features = _dnx_data_tune_ecgm_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data tune ecgm features");

    
    submodule_data->nof_defines = _dnx_data_tune_ecgm_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data tune ecgm defines");

    submodule_data->defines[dnx_data_tune_ecgm_define_core_flow_control_percentage_ratio].name = "core_flow_control_percentage_ratio";
    submodule_data->defines[dnx_data_tune_ecgm_define_core_flow_control_percentage_ratio].doc = "ratio used for core/interface resources flow control thresholds calculations. (percentage units)";
    
    submodule_data->defines[dnx_data_tune_ecgm_define_core_flow_control_percentage_ratio].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tune_ecgm_define_port_uc_flow_control_min_divider].name = "port_uc_flow_control_min_divider";
    submodule_data->defines[dnx_data_tune_ecgm_define_port_uc_flow_control_min_divider].doc = "divider factor for uc flow control minimum calculation per port and queue";
    
    submodule_data->defines[dnx_data_tune_ecgm_define_port_uc_flow_control_min_divider].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_tune_ecgm_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data tune ecgm tables");

    
    submodule_data->tables[dnx_data_tune_ecgm_table_port_dp_ratio].name = "port_dp_ratio";
    submodule_data->tables[dnx_data_tune_ecgm_table_port_dp_ratio].doc = "ration per dp used for thresholds calculations";
    submodule_data->tables[dnx_data_tune_ecgm_table_port_dp_ratio].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_tune_ecgm_table_port_dp_ratio].size_of_values = sizeof(dnx_data_tune_ecgm_port_dp_ratio_t);
    submodule_data->tables[dnx_data_tune_ecgm_table_port_dp_ratio].entry_get = dnx_data_tune_ecgm_port_dp_ratio_entry_str_get;

    
    submodule_data->tables[dnx_data_tune_ecgm_table_port_dp_ratio].nof_keys = 1;
    submodule_data->tables[dnx_data_tune_ecgm_table_port_dp_ratio].keys[0].name = "dp";
    submodule_data->tables[dnx_data_tune_ecgm_table_port_dp_ratio].keys[0].doc = "table index";

    
    submodule_data->tables[dnx_data_tune_ecgm_table_port_dp_ratio].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_tune_ecgm_table_port_dp_ratio].values, dnxc_data_value_t, submodule_data->tables[dnx_data_tune_ecgm_table_port_dp_ratio].nof_values, "_dnx_data_tune_ecgm_table_port_dp_ratio table values");
    submodule_data->tables[dnx_data_tune_ecgm_table_port_dp_ratio].values[0].name = "percentage_ratio";
    submodule_data->tables[dnx_data_tune_ecgm_table_port_dp_ratio].values[0].type = "int";
    submodule_data->tables[dnx_data_tune_ecgm_table_port_dp_ratio].values[0].doc = "ratio in percentage, for thresholds calculation per dp (examples to valid values: 100 / 50 / 10)";
    submodule_data->tables[dnx_data_tune_ecgm_table_port_dp_ratio].values[0].offset = UTILEX_OFFSETOF(dnx_data_tune_ecgm_port_dp_ratio_t, percentage_ratio);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_tune_ecgm_feature_get(
    int unit,
    dnx_data_tune_ecgm_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_tune, dnx_data_tune_submodule_ecgm, feature);
}


uint32
dnx_data_tune_ecgm_core_flow_control_percentage_ratio_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tune, dnx_data_tune_submodule_ecgm, dnx_data_tune_ecgm_define_core_flow_control_percentage_ratio);
}

uint32
dnx_data_tune_ecgm_port_uc_flow_control_min_divider_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tune, dnx_data_tune_submodule_ecgm, dnx_data_tune_ecgm_define_port_uc_flow_control_min_divider);
}



const dnx_data_tune_ecgm_port_dp_ratio_t *
dnx_data_tune_ecgm_port_dp_ratio_get(
    int unit,
    int dp)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_tune, dnx_data_tune_submodule_ecgm, dnx_data_tune_ecgm_table_port_dp_ratio);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dp, 0);
    return (const dnx_data_tune_ecgm_port_dp_ratio_t *) data;

}


shr_error_e
dnx_data_tune_ecgm_port_dp_ratio_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_tune_ecgm_port_dp_ratio_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_tune, dnx_data_tune_submodule_ecgm, dnx_data_tune_ecgm_table_port_dp_ratio);
    data = (const dnx_data_tune_ecgm_port_dp_ratio_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->percentage_ratio);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_tune_ecgm_port_dp_ratio_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_tune, dnx_data_tune_submodule_ecgm, dnx_data_tune_ecgm_table_port_dp_ratio);

}






static shr_error_e
dnx_data_tune_iqs_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "iqs";
    submodule_data->doc = "IQS tune data";
    
    submodule_data->nof_features = _dnx_data_tune_iqs_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data tune iqs features");

    
    submodule_data->nof_defines = _dnx_data_tune_iqs_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data tune iqs defines");

    submodule_data->defines[dnx_data_tune_iqs_define_credit_resolution_up_th].name = "credit_resolution_up_th";
    submodule_data->defines[dnx_data_tune_iqs_define_credit_resolution_up_th].doc = "[Byte]. threshold over the delayed size (2 * fabric_delay * port rate) to move to the higher credit resolution";
    
    submodule_data->defines[dnx_data_tune_iqs_define_credit_resolution_up_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tune_iqs_define_fabric_delay].name = "fabric_delay";
    submodule_data->defines[dnx_data_tune_iqs_define_fabric_delay].doc = "[nano-sec] credit ingress to egress delay";
    
    submodule_data->defines[dnx_data_tune_iqs_define_fabric_delay].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_tune_iqs_define_fabric_multicast_delay].name = "fabric_multicast_delay";
    submodule_data->defines[dnx_data_tune_iqs_define_fabric_multicast_delay].doc = "[nano-sec] credit delay for fmqs";
    
    submodule_data->defines[dnx_data_tune_iqs_define_fabric_multicast_delay].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_tune_iqs_define_fmq_max_rate].name = "fmq_max_rate";
    submodule_data->defines[dnx_data_tune_iqs_define_fmq_max_rate].doc = "[Gbps] fmq credit generator rate";
    
    submodule_data->defines[dnx_data_tune_iqs_define_fmq_max_rate].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_tune_iqs_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data tune iqs tables");

    
    submodule_data->tables[dnx_data_tune_iqs_table_bw_level_rate].name = "bw_level_rate";
    submodule_data->tables[dnx_data_tune_iqs_table_bw_level_rate].doc = "stores the read weight profiles that should configured to HW";
    submodule_data->tables[dnx_data_tune_iqs_table_bw_level_rate].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_tune_iqs_table_bw_level_rate].size_of_values = sizeof(dnx_data_tune_iqs_bw_level_rate_t);
    submodule_data->tables[dnx_data_tune_iqs_table_bw_level_rate].entry_get = dnx_data_tune_iqs_bw_level_rate_entry_str_get;

    
    submodule_data->tables[dnx_data_tune_iqs_table_bw_level_rate].nof_keys = 1;
    submodule_data->tables[dnx_data_tune_iqs_table_bw_level_rate].keys[0].name = "bw_level";
    submodule_data->tables[dnx_data_tune_iqs_table_bw_level_rate].keys[0].doc = "bandwidth level";

    
    submodule_data->tables[dnx_data_tune_iqs_table_bw_level_rate].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_tune_iqs_table_bw_level_rate].values, dnxc_data_value_t, submodule_data->tables[dnx_data_tune_iqs_table_bw_level_rate].nof_values, "_dnx_data_tune_iqs_table_bw_level_rate table values");
    submodule_data->tables[dnx_data_tune_iqs_table_bw_level_rate].values[0].name = "max";
    submodule_data->tables[dnx_data_tune_iqs_table_bw_level_rate].values[0].type = "uint32";
    submodule_data->tables[dnx_data_tune_iqs_table_bw_level_rate].values[0].doc = "max rate for bandwidth level";
    submodule_data->tables[dnx_data_tune_iqs_table_bw_level_rate].values[0].offset = UTILEX_OFFSETOF(dnx_data_tune_iqs_bw_level_rate_t, max);
    submodule_data->tables[dnx_data_tune_iqs_table_bw_level_rate].values[1].name = "min";
    submodule_data->tables[dnx_data_tune_iqs_table_bw_level_rate].values[1].type = "uint32";
    submodule_data->tables[dnx_data_tune_iqs_table_bw_level_rate].values[1].doc = "min rate for bandwidth level";
    submodule_data->tables[dnx_data_tune_iqs_table_bw_level_rate].values[1].offset = UTILEX_OFFSETOF(dnx_data_tune_iqs_bw_level_rate_t, min);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_tune_iqs_feature_get(
    int unit,
    dnx_data_tune_iqs_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_tune, dnx_data_tune_submodule_iqs, feature);
}


uint32
dnx_data_tune_iqs_credit_resolution_up_th_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tune, dnx_data_tune_submodule_iqs, dnx_data_tune_iqs_define_credit_resolution_up_th);
}

uint32
dnx_data_tune_iqs_fabric_delay_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tune, dnx_data_tune_submodule_iqs, dnx_data_tune_iqs_define_fabric_delay);
}

uint32
dnx_data_tune_iqs_fabric_multicast_delay_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tune, dnx_data_tune_submodule_iqs, dnx_data_tune_iqs_define_fabric_multicast_delay);
}

uint32
dnx_data_tune_iqs_fmq_max_rate_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tune, dnx_data_tune_submodule_iqs, dnx_data_tune_iqs_define_fmq_max_rate);
}



const dnx_data_tune_iqs_bw_level_rate_t *
dnx_data_tune_iqs_bw_level_rate_get(
    int unit,
    int bw_level)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_tune, dnx_data_tune_submodule_iqs, dnx_data_tune_iqs_table_bw_level_rate);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, bw_level, 0);
    return (const dnx_data_tune_iqs_bw_level_rate_t *) data;

}


shr_error_e
dnx_data_tune_iqs_bw_level_rate_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_tune_iqs_bw_level_rate_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_tune, dnx_data_tune_submodule_iqs, dnx_data_tune_iqs_table_bw_level_rate);
    data = (const dnx_data_tune_iqs_bw_level_rate_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->max);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->min);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_tune_iqs_bw_level_rate_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_tune, dnx_data_tune_submodule_iqs, dnx_data_tune_iqs_table_bw_level_rate);

}






static shr_error_e
dnx_data_tune_fabric_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "fabric";
    submodule_data->doc = "fabric tune data";
    
    submodule_data->nof_features = _dnx_data_tune_fabric_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data tune fabric features");

    
    submodule_data->nof_defines = _dnx_data_tune_fabric_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data tune fabric defines");

    submodule_data->defines[dnx_data_tune_fabric_define_cgm_llfc_pipe_th].name = "cgm_llfc_pipe_th";
    submodule_data->defines[dnx_data_tune_fabric_define_cgm_llfc_pipe_th].doc = "threshold for bcmFabricCgmLlfcPipeTh";
    
    submodule_data->defines[dnx_data_tune_fabric_define_cgm_llfc_pipe_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tune_fabric_define_cgm_drop_fabric_class_th].name = "cgm_drop_fabric_class_th";
    submodule_data->defines[dnx_data_tune_fabric_define_cgm_drop_fabric_class_th].doc = "threshold for bcmFabricCgmDropFabricClassTh";
    
    submodule_data->defines[dnx_data_tune_fabric_define_cgm_drop_fabric_class_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tune_fabric_define_cgm_rci_high_sev_min_links_param].name = "cgm_rci_high_sev_min_links_param";
    submodule_data->defines[dnx_data_tune_fabric_define_cgm_rci_high_sev_min_links_param].doc = "threshold for bcmFabricCgmRciHighSeverityMinLinksParam";
    
    submodule_data->defines[dnx_data_tune_fabric_define_cgm_rci_high_sev_min_links_param].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_tune_fabric_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data tune fabric tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_tune_fabric_feature_get(
    int unit,
    dnx_data_tune_fabric_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_tune, dnx_data_tune_submodule_fabric, feature);
}


uint32
dnx_data_tune_fabric_cgm_llfc_pipe_th_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tune, dnx_data_tune_submodule_fabric, dnx_data_tune_fabric_define_cgm_llfc_pipe_th);
}

uint32
dnx_data_tune_fabric_cgm_drop_fabric_class_th_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tune, dnx_data_tune_submodule_fabric, dnx_data_tune_fabric_define_cgm_drop_fabric_class_th);
}

uint32
dnx_data_tune_fabric_cgm_rci_high_sev_min_links_param_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tune, dnx_data_tune_submodule_fabric, dnx_data_tune_fabric_define_cgm_rci_high_sev_min_links_param);
}







shr_error_e
dnx_data_tune_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "tune";
    module_data->nof_submodules = _dnx_data_tune_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data tune submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_tune_ecgm_init(unit, &module_data->submodules[dnx_data_tune_submodule_ecgm]));
    SHR_IF_ERR_EXIT(dnx_data_tune_iqs_init(unit, &module_data->submodules[dnx_data_tune_submodule_iqs]));
    SHR_IF_ERR_EXIT(dnx_data_tune_fabric_init(unit, &module_data->submodules[dnx_data_tune_submodule_fabric]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tune_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tune_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tune_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tune_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_tune_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tune_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_tune_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tune_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_tune_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tune_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_tune_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tune_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_tune_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tune_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_tune_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

