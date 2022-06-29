
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INIT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_dev_init.h>




extern shr_error_e jr2_a0_data_dev_init_attach(
    int unit);


extern shr_error_e jr2_b0_data_dev_init_attach(
    int unit);


extern shr_error_e j2c_a0_data_dev_init_attach(
    int unit);


extern shr_error_e q2a_a0_data_dev_init_attach(
    int unit);


extern shr_error_e q2a_b0_data_dev_init_attach(
    int unit);


extern shr_error_e j2p_a0_data_dev_init_attach(
    int unit);


extern shr_error_e j2x_a0_data_dev_init_attach(
    int unit);




static shr_error_e
dnx_data_dev_init_time_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "time";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_dev_init_time_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dev_init time features");

    
    submodule_data->nof_defines = _dnx_data_dev_init_time_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dev_init time defines");

    submodule_data->defines[dnx_data_dev_init_time_define_analyze].name = "analyze";
    submodule_data->defines[dnx_data_dev_init_time_define_analyze].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_time_define_analyze].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dev_init_time_define_init_total_thresh].name = "init_total_thresh";
    submodule_data->defines[dnx_data_dev_init_time_define_init_total_thresh].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_time_define_init_total_thresh].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dev_init_time_define_appl_init_total_thresh].name = "appl_init_total_thresh";
    submodule_data->defines[dnx_data_dev_init_time_define_appl_init_total_thresh].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_time_define_appl_init_total_thresh].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_dev_init_time_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dev_init time tables");

    
    submodule_data->tables[dnx_data_dev_init_time_table_step_thresh].name = "step_thresh";
    submodule_data->tables[dnx_data_dev_init_time_table_step_thresh].doc = "";
    submodule_data->tables[dnx_data_dev_init_time_table_step_thresh].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dev_init_time_table_step_thresh].size_of_values = sizeof(dnx_data_dev_init_time_step_thresh_t);
    submodule_data->tables[dnx_data_dev_init_time_table_step_thresh].entry_get = dnx_data_dev_init_time_step_thresh_entry_str_get;

    
    submodule_data->tables[dnx_data_dev_init_time_table_step_thresh].nof_keys = 1;
    submodule_data->tables[dnx_data_dev_init_time_table_step_thresh].keys[0].name = "step_id";
    submodule_data->tables[dnx_data_dev_init_time_table_step_thresh].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dev_init_time_table_step_thresh].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dev_init_time_table_step_thresh].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dev_init_time_table_step_thresh].nof_values, "_dnx_data_dev_init_time_table_step_thresh table values");
    submodule_data->tables[dnx_data_dev_init_time_table_step_thresh].values[0].name = "value";
    submodule_data->tables[dnx_data_dev_init_time_table_step_thresh].values[0].type = "uint32";
    submodule_data->tables[dnx_data_dev_init_time_table_step_thresh].values[0].doc = "";
    submodule_data->tables[dnx_data_dev_init_time_table_step_thresh].values[0].offset = UTILEX_OFFSETOF(dnx_data_dev_init_time_step_thresh_t, value);

    
    submodule_data->tables[dnx_data_dev_init_time_table_appl_step_thresh].name = "appl_step_thresh";
    submodule_data->tables[dnx_data_dev_init_time_table_appl_step_thresh].doc = "";
    submodule_data->tables[dnx_data_dev_init_time_table_appl_step_thresh].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dev_init_time_table_appl_step_thresh].size_of_values = sizeof(dnx_data_dev_init_time_appl_step_thresh_t);
    submodule_data->tables[dnx_data_dev_init_time_table_appl_step_thresh].entry_get = dnx_data_dev_init_time_appl_step_thresh_entry_str_get;

    
    submodule_data->tables[dnx_data_dev_init_time_table_appl_step_thresh].nof_keys = 1;
    submodule_data->tables[dnx_data_dev_init_time_table_appl_step_thresh].keys[0].name = "step_id";
    submodule_data->tables[dnx_data_dev_init_time_table_appl_step_thresh].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dev_init_time_table_appl_step_thresh].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dev_init_time_table_appl_step_thresh].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dev_init_time_table_appl_step_thresh].nof_values, "_dnx_data_dev_init_time_table_appl_step_thresh table values");
    submodule_data->tables[dnx_data_dev_init_time_table_appl_step_thresh].values[0].name = "value";
    submodule_data->tables[dnx_data_dev_init_time_table_appl_step_thresh].values[0].type = "uint32";
    submodule_data->tables[dnx_data_dev_init_time_table_appl_step_thresh].values[0].doc = "";
    submodule_data->tables[dnx_data_dev_init_time_table_appl_step_thresh].values[0].offset = UTILEX_OFFSETOF(dnx_data_dev_init_time_appl_step_thresh_t, value);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_dev_init_time_feature_get(
    int unit,
    dnx_data_dev_init_time_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_time, feature);
}


uint32
dnx_data_dev_init_time_analyze_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_time, dnx_data_dev_init_time_define_analyze);
}

uint32
dnx_data_dev_init_time_init_total_thresh_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_time, dnx_data_dev_init_time_define_init_total_thresh);
}

uint32
dnx_data_dev_init_time_appl_init_total_thresh_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_time, dnx_data_dev_init_time_define_appl_init_total_thresh);
}



const dnx_data_dev_init_time_step_thresh_t *
dnx_data_dev_init_time_step_thresh_get(
    int unit,
    int step_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_time, dnx_data_dev_init_time_table_step_thresh);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, step_id, 0);
    return (const dnx_data_dev_init_time_step_thresh_t *) data;

}

const dnx_data_dev_init_time_appl_step_thresh_t *
dnx_data_dev_init_time_appl_step_thresh_get(
    int unit,
    int step_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_time, dnx_data_dev_init_time_table_appl_step_thresh);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, step_id, 0);
    return (const dnx_data_dev_init_time_appl_step_thresh_t *) data;

}


shr_error_e
dnx_data_dev_init_time_step_thresh_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dev_init_time_step_thresh_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_time, dnx_data_dev_init_time_table_step_thresh);
    data = (const dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->value);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dev_init_time_appl_step_thresh_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dev_init_time_appl_step_thresh_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_time, dnx_data_dev_init_time_table_appl_step_thresh);
    data = (const dnx_data_dev_init_time_appl_step_thresh_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->value);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_dev_init_time_step_thresh_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_time, dnx_data_dev_init_time_table_step_thresh);

}

const dnxc_data_table_info_t *
dnx_data_dev_init_time_appl_step_thresh_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_time, dnx_data_dev_init_time_table_appl_step_thresh);

}






static shr_error_e
dnx_data_dev_init_mem_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "mem";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_dev_init_mem_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dev_init mem features");

    
    submodule_data->nof_defines = _dnx_data_dev_init_mem_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dev_init mem defines");

    submodule_data->defines[dnx_data_dev_init_mem_define_force_zeros].name = "force_zeros";
    submodule_data->defines[dnx_data_dev_init_mem_define_force_zeros].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_mem_define_force_zeros].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dev_init_mem_define_reset_mode].name = "reset_mode";
    submodule_data->defines[dnx_data_dev_init_mem_define_reset_mode].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_mem_define_reset_mode].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dev_init_mem_define_defaults_verify].name = "defaults_verify";
    submodule_data->defines[dnx_data_dev_init_mem_define_defaults_verify].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_mem_define_defaults_verify].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dev_init_mem_define_emul_ext_init].name = "emul_ext_init";
    submodule_data->defines[dnx_data_dev_init_mem_define_emul_ext_init].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_mem_define_emul_ext_init].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_dev_init_mem_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dev_init mem tables");

    
    submodule_data->tables[dnx_data_dev_init_mem_table_default].name = "default";
    submodule_data->tables[dnx_data_dev_init_mem_table_default].doc = "";
    submodule_data->tables[dnx_data_dev_init_mem_table_default].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dev_init_mem_table_default].size_of_values = sizeof(dnx_data_dev_init_mem_default_t);
    submodule_data->tables[dnx_data_dev_init_mem_table_default].entry_get = dnx_data_dev_init_mem_default_entry_str_get;

    
    submodule_data->tables[dnx_data_dev_init_mem_table_default].nof_keys = 1;
    submodule_data->tables[dnx_data_dev_init_mem_table_default].keys[0].name = "index";
    submodule_data->tables[dnx_data_dev_init_mem_table_default].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dev_init_mem_table_default].nof_values = 5;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dev_init_mem_table_default].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dev_init_mem_table_default].nof_values, "_dnx_data_dev_init_mem_table_default table values");
    submodule_data->tables[dnx_data_dev_init_mem_table_default].values[0].name = "mem";
    submodule_data->tables[dnx_data_dev_init_mem_table_default].values[0].type = "soc_mem_t";
    submodule_data->tables[dnx_data_dev_init_mem_table_default].values[0].doc = "";
    submodule_data->tables[dnx_data_dev_init_mem_table_default].values[0].offset = UTILEX_OFFSETOF(dnx_data_dev_init_mem_default_t, mem);
    submodule_data->tables[dnx_data_dev_init_mem_table_default].values[1].name = "mode";
    submodule_data->tables[dnx_data_dev_init_mem_table_default].values[1].type = "dnx_init_mem_default_mode_e";
    submodule_data->tables[dnx_data_dev_init_mem_table_default].values[1].doc = "";
    submodule_data->tables[dnx_data_dev_init_mem_table_default].values[1].offset = UTILEX_OFFSETOF(dnx_data_dev_init_mem_default_t, mode);
    submodule_data->tables[dnx_data_dev_init_mem_table_default].values[2].name = "default_get_cb";
    submodule_data->tables[dnx_data_dev_init_mem_table_default].values[2].type = "dnx_init_mem_default_get_f";
    submodule_data->tables[dnx_data_dev_init_mem_table_default].values[2].doc = "";
    submodule_data->tables[dnx_data_dev_init_mem_table_default].values[2].offset = UTILEX_OFFSETOF(dnx_data_dev_init_mem_default_t, default_get_cb);
    submodule_data->tables[dnx_data_dev_init_mem_table_default].values[3].name = "field";
    submodule_data->tables[dnx_data_dev_init_mem_table_default].values[3].type = "soc_field_t";
    submodule_data->tables[dnx_data_dev_init_mem_table_default].values[3].doc = "";
    submodule_data->tables[dnx_data_dev_init_mem_table_default].values[3].offset = UTILEX_OFFSETOF(dnx_data_dev_init_mem_default_t, field);
    submodule_data->tables[dnx_data_dev_init_mem_table_default].values[4].name = "valid";
    submodule_data->tables[dnx_data_dev_init_mem_table_default].values[4].type = "uint32";
    submodule_data->tables[dnx_data_dev_init_mem_table_default].values[4].doc = "";
    submodule_data->tables[dnx_data_dev_init_mem_table_default].values[4].offset = UTILEX_OFFSETOF(dnx_data_dev_init_mem_default_t, valid);

#ifdef BCM_ACCESS_SUPPORT

    
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].name = "sw_mem_init";
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].doc = "";
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].size_of_values = sizeof(dnx_data_dev_init_mem_sw_mem_init_t);
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].entry_get = dnx_data_dev_init_mem_sw_mem_init_entry_str_get;

    
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].nof_keys = 1;
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].keys[0].name = "index";
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].nof_values = 6;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].nof_values, "_dnx_data_dev_init_mem_table_sw_mem_init table values");
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].values[0].name = "mem";
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].values[0].type = "access_regmem_id_t";
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].values[0].doc = "";
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].values[0].offset = UTILEX_OFFSETOF(dnx_data_dev_init_mem_sw_mem_init_t, mem);
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].values[1].name = "mode";
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].values[1].type = "dnx_sw_init_mem_mode_e";
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].values[1].doc = "";
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].values[1].offset = UTILEX_OFFSETOF(dnx_data_dev_init_mem_sw_mem_init_t, mode);
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].values[2].name = "sw_mem_init_get_cb";
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].values[2].type = "dnx_sw_init_mem_custom_value_get_f";
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].values[2].doc = "";
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].values[2].offset = UTILEX_OFFSETOF(dnx_data_dev_init_mem_sw_mem_init_t, sw_mem_init_get_cb);
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].values[3].name = "sw_mem_init_get_fill_custom_value_cb";
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].values[3].type = "dnx_sw_init_mem_custom_fill_value_get_f";
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].values[3].doc = "";
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].values[3].offset = UTILEX_OFFSETOF(dnx_data_dev_init_mem_sw_mem_init_t, sw_mem_init_get_fill_custom_value_cb);
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].values[4].name = "field";
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].values[4].type = "access_field_id_t";
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].values[4].doc = "";
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].values[4].offset = UTILEX_OFFSETOF(dnx_data_dev_init_mem_sw_mem_init_t, field);
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].values[5].name = "valid";
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].values[5].type = "uint32";
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].values[5].doc = "";
    submodule_data->tables[dnx_data_dev_init_mem_table_sw_mem_init].values[5].offset = UTILEX_OFFSETOF(dnx_data_dev_init_mem_sw_mem_init_t, valid);

#endif 
    
    submodule_data->tables[dnx_data_dev_init_mem_table_emul_ext_init_path].name = "emul_ext_init_path";
    submodule_data->tables[dnx_data_dev_init_mem_table_emul_ext_init_path].doc = "";
    submodule_data->tables[dnx_data_dev_init_mem_table_emul_ext_init_path].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dev_init_mem_table_emul_ext_init_path].size_of_values = sizeof(dnx_data_dev_init_mem_emul_ext_init_path_t);
    submodule_data->tables[dnx_data_dev_init_mem_table_emul_ext_init_path].entry_get = dnx_data_dev_init_mem_emul_ext_init_path_entry_str_get;

    
    submodule_data->tables[dnx_data_dev_init_mem_table_emul_ext_init_path].nof_keys = 0;

    
    submodule_data->tables[dnx_data_dev_init_mem_table_emul_ext_init_path].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dev_init_mem_table_emul_ext_init_path].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dev_init_mem_table_emul_ext_init_path].nof_values, "_dnx_data_dev_init_mem_table_emul_ext_init_path table values");
    submodule_data->tables[dnx_data_dev_init_mem_table_emul_ext_init_path].values[0].name = "full_path";
    submodule_data->tables[dnx_data_dev_init_mem_table_emul_ext_init_path].values[0].type = "char *";
    submodule_data->tables[dnx_data_dev_init_mem_table_emul_ext_init_path].values[0].doc = "";
    submodule_data->tables[dnx_data_dev_init_mem_table_emul_ext_init_path].values[0].offset = UTILEX_OFFSETOF(dnx_data_dev_init_mem_emul_ext_init_path_t, full_path);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_dev_init_mem_feature_get(
    int unit,
    dnx_data_dev_init_mem_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_mem, feature);
}


uint32
dnx_data_dev_init_mem_force_zeros_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_mem, dnx_data_dev_init_mem_define_force_zeros);
}

uint32
dnx_data_dev_init_mem_reset_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_mem, dnx_data_dev_init_mem_define_reset_mode);
}

uint32
dnx_data_dev_init_mem_defaults_verify_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_mem, dnx_data_dev_init_mem_define_defaults_verify);
}

uint32
dnx_data_dev_init_mem_emul_ext_init_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_mem, dnx_data_dev_init_mem_define_emul_ext_init);
}



const dnx_data_dev_init_mem_default_t *
dnx_data_dev_init_mem_default_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_mem, dnx_data_dev_init_mem_table_default);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dev_init_mem_default_t *) data;

}

#ifdef BCM_ACCESS_SUPPORT

const dnx_data_dev_init_mem_sw_mem_init_t *
dnx_data_dev_init_mem_sw_mem_init_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_mem, dnx_data_dev_init_mem_table_sw_mem_init);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dev_init_mem_sw_mem_init_t *) data;

}


#endif 
const dnx_data_dev_init_mem_emul_ext_init_path_t *
dnx_data_dev_init_mem_emul_ext_init_path_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_mem, dnx_data_dev_init_mem_table_emul_ext_init_path);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_dev_init_mem_emul_ext_init_path_t *) data;

}


shr_error_e
dnx_data_dev_init_mem_default_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dev_init_mem_default_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_mem, dnx_data_dev_init_mem_table_default);
    data = (const dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mem);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mode);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->default_get_cb == NULL ? "NULL" : "func");
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->field);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
    }

    SHR_FUNC_EXIT;
}

#ifdef BCM_ACCESS_SUPPORT

shr_error_e
dnx_data_dev_init_mem_sw_mem_init_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dev_init_mem_sw_mem_init_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_mem, dnx_data_dev_init_mem_table_sw_mem_init);
    data = (const dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mem);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mode);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->sw_mem_init_get_cb == NULL ? "NULL" : "func");
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->sw_mem_init_get_fill_custom_value_cb == NULL ? "NULL" : "func");
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->field);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
    }

    SHR_FUNC_EXIT;
}


#endif 
shr_error_e
dnx_data_dev_init_mem_emul_ext_init_path_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dev_init_mem_emul_ext_init_path_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_mem, dnx_data_dev_init_mem_table_emul_ext_init_path);
    data = (const dnx_data_dev_init_mem_emul_ext_init_path_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->full_path == NULL ? "" : data->full_path);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_dev_init_mem_default_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_mem, dnx_data_dev_init_mem_table_default);

}

#ifdef BCM_ACCESS_SUPPORT

const dnxc_data_table_info_t *
dnx_data_dev_init_mem_sw_mem_init_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_mem, dnx_data_dev_init_mem_table_sw_mem_init);

}


#endif 
const dnxc_data_table_info_t *
dnx_data_dev_init_mem_emul_ext_init_path_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_mem, dnx_data_dev_init_mem_table_emul_ext_init_path);

}






static shr_error_e
dnx_data_dev_init_properties_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "properties";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_dev_init_properties_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dev_init properties features");

    
    submodule_data->nof_defines = _dnx_data_dev_init_properties_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dev_init properties defines");

    submodule_data->defines[dnx_data_dev_init_properties_define_name_max].name = "name_max";
    submodule_data->defines[dnx_data_dev_init_properties_define_name_max].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_properties_define_name_max].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_dev_init_properties_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dev_init properties tables");

    
    submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].name = "unsupported";
    submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].doc = "";
    submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].size_of_values = sizeof(dnx_data_dev_init_properties_unsupported_t);
    submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].entry_get = dnx_data_dev_init_properties_unsupported_entry_str_get;

    
    submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].nof_keys = 1;
    submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].keys[0].name = "index";
    submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].nof_values = 5;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].nof_values, "_dnx_data_dev_init_properties_table_unsupported table values");
    submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].values[0].name = "property";
    submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].values[0].type = "char *";
    submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].values[0].doc = "";
    submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].values[0].offset = UTILEX_OFFSETOF(dnx_data_dev_init_properties_unsupported_t, property);
    submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].values[1].name = "suffix";
    submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].values[1].type = "char *";
    submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].values[1].doc = "";
    submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].values[1].offset = UTILEX_OFFSETOF(dnx_data_dev_init_properties_unsupported_t, suffix);
    submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].values[2].name = "num_max";
    submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].values[2].type = "int";
    submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].values[2].doc = "";
    submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].values[2].offset = UTILEX_OFFSETOF(dnx_data_dev_init_properties_unsupported_t, num_max);
    submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].values[3].name = "per_port";
    submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].values[3].type = "int";
    submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].values[3].doc = "";
    submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].values[3].offset = UTILEX_OFFSETOF(dnx_data_dev_init_properties_unsupported_t, per_port);
    submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].values[4].name = "err_msg";
    submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].values[4].type = "char *";
    submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].values[4].doc = "";
    submodule_data->tables[dnx_data_dev_init_properties_table_unsupported].values[4].offset = UTILEX_OFFSETOF(dnx_data_dev_init_properties_unsupported_t, err_msg);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_dev_init_properties_feature_get(
    int unit,
    dnx_data_dev_init_properties_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_properties, feature);
}


uint32
dnx_data_dev_init_properties_name_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_properties, dnx_data_dev_init_properties_define_name_max);
}



const dnx_data_dev_init_properties_unsupported_t *
dnx_data_dev_init_properties_unsupported_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_properties, dnx_data_dev_init_properties_table_unsupported);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dev_init_properties_unsupported_t *) data;

}


shr_error_e
dnx_data_dev_init_properties_unsupported_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dev_init_properties_unsupported_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_properties, dnx_data_dev_init_properties_table_unsupported);
    data = (const dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->property == NULL ? "" : data->property);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->suffix == NULL ? "" : data->suffix);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->num_max);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->per_port);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->err_msg == NULL ? "" : data->err_msg);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_dev_init_properties_unsupported_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_properties, dnx_data_dev_init_properties_table_unsupported);

}






static shr_error_e
dnx_data_dev_init_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_dev_init_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dev_init general features");

    submodule_data->features[dnx_data_dev_init_general_multithreading].name = "multithreading";
    submodule_data->features[dnx_data_dev_init_general_multithreading].doc = "";
    submodule_data->features[dnx_data_dev_init_general_multithreading].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_general_data_path_hw].name = "data_path_hw";
    submodule_data->features[dnx_data_dev_init_general_data_path_hw].doc = "";
    submodule_data->features[dnx_data_dev_init_general_data_path_hw].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_general_remove_crc_bytes].name = "remove_crc_bytes";
    submodule_data->features[dnx_data_dev_init_general_remove_crc_bytes].doc = "";
    submodule_data->features[dnx_data_dev_init_general_remove_crc_bytes].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_general_remove_crc_bytes_capability].name = "remove_crc_bytes_capability";
    submodule_data->features[dnx_data_dev_init_general_remove_crc_bytes_capability].doc = "";
    submodule_data->features[dnx_data_dev_init_general_remove_crc_bytes_capability].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_general_erpp_compensate_crc_size].name = "erpp_compensate_crc_size";
    submodule_data->features[dnx_data_dev_init_general_erpp_compensate_crc_size].doc = "";
    submodule_data->features[dnx_data_dev_init_general_erpp_compensate_crc_size].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_general_etpp_compensate_crc_size].name = "etpp_compensate_crc_size";
    submodule_data->features[dnx_data_dev_init_general_etpp_compensate_crc_size].doc = "";
    submodule_data->features[dnx_data_dev_init_general_etpp_compensate_crc_size].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_general_tail_editing_enable].name = "tail_editing_enable";
    submodule_data->features[dnx_data_dev_init_general_tail_editing_enable].doc = "";
    submodule_data->features[dnx_data_dev_init_general_tail_editing_enable].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_general_tail_editing_append_enable].name = "tail_editing_append_enable";
    submodule_data->features[dnx_data_dev_init_general_tail_editing_append_enable].doc = "";
    submodule_data->features[dnx_data_dev_init_general_tail_editing_append_enable].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_general_tail_editing_truncate_enable].name = "tail_editing_truncate_enable";
    submodule_data->features[dnx_data_dev_init_general_tail_editing_truncate_enable].doc = "";
    submodule_data->features[dnx_data_dev_init_general_tail_editing_truncate_enable].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_general_current_packet_size_compensation_term].name = "current_packet_size_compensation_term";
    submodule_data->features[dnx_data_dev_init_general_current_packet_size_compensation_term].doc = "";
    submodule_data->features[dnx_data_dev_init_general_current_packet_size_compensation_term].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_general_erpp_filters_non_separate_enablers].name = "erpp_filters_non_separate_enablers";
    submodule_data->features[dnx_data_dev_init_general_erpp_filters_non_separate_enablers].doc = "";
    submodule_data->features[dnx_data_dev_init_general_erpp_filters_non_separate_enablers].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_general_etppa_bypass_stamp_ftmh_enable].name = "etppa_bypass_stamp_ftmh_enable";
    submodule_data->features[dnx_data_dev_init_general_etppa_bypass_stamp_ftmh_enable].doc = "";
    submodule_data->features[dnx_data_dev_init_general_etppa_bypass_stamp_ftmh_enable].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_general_l4_protocol_fields_config_enable].name = "l4_protocol_fields_config_enable";
    submodule_data->features[dnx_data_dev_init_general_l4_protocol_fields_config_enable].doc = "";
    submodule_data->features[dnx_data_dev_init_general_l4_protocol_fields_config_enable].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_general_da_type_map_enable].name = "da_type_map_enable";
    submodule_data->features[dnx_data_dev_init_general_da_type_map_enable].doc = "";
    submodule_data->features[dnx_data_dev_init_general_da_type_map_enable].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_general_ecologic_support].name = "ecologic_support";
    submodule_data->features[dnx_data_dev_init_general_ecologic_support].doc = "";
    submodule_data->features[dnx_data_dev_init_general_ecologic_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_general_ecologic_container_support].name = "ecologic_container_support";
    submodule_data->features[dnx_data_dev_init_general_ecologic_container_support].doc = "";
    submodule_data->features[dnx_data_dev_init_general_ecologic_container_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_general_context_selection_index_valid].name = "context_selection_index_valid";
    submodule_data->features[dnx_data_dev_init_general_context_selection_index_valid].doc = "";
    submodule_data->features[dnx_data_dev_init_general_context_selection_index_valid].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_general_network_header_termination].name = "network_header_termination";
    submodule_data->features[dnx_data_dev_init_general_network_header_termination].doc = "";
    submodule_data->features[dnx_data_dev_init_general_network_header_termination].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_general_network_offset_for_system_headers_valid].name = "network_offset_for_system_headers_valid";
    submodule_data->features[dnx_data_dev_init_general_network_offset_for_system_headers_valid].doc = "";
    submodule_data->features[dnx_data_dev_init_general_network_offset_for_system_headers_valid].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_general_egress_estimated_bta_btr_hw_config].name = "egress_estimated_bta_btr_hw_config";
    submodule_data->features[dnx_data_dev_init_general_egress_estimated_bta_btr_hw_config].doc = "";
    submodule_data->features[dnx_data_dev_init_general_egress_estimated_bta_btr_hw_config].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_general_outlif_intf_hw_config].name = "outlif_intf_hw_config";
    submodule_data->features[dnx_data_dev_init_general_outlif_intf_hw_config].doc = "";
    submodule_data->features[dnx_data_dev_init_general_outlif_intf_hw_config].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_general_outlif_group_hw_config].name = "outlif_group_hw_config";
    submodule_data->features[dnx_data_dev_init_general_outlif_group_hw_config].doc = "";
    submodule_data->features[dnx_data_dev_init_general_outlif_group_hw_config].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_general_application_v1].name = "application_v1";
    submodule_data->features[dnx_data_dev_init_general_application_v1].doc = "";
    submodule_data->features[dnx_data_dev_init_general_application_v1].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_general_application_v2].name = "application_v2";
    submodule_data->features[dnx_data_dev_init_general_application_v2].doc = "";
    submodule_data->features[dnx_data_dev_init_general_application_v2].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_general_erpp_threshold_prp_per_port_info].name = "erpp_threshold_prp_per_port_info";
    submodule_data->features[dnx_data_dev_init_general_erpp_threshold_prp_per_port_info].doc = "";
    submodule_data->features[dnx_data_dev_init_general_erpp_threshold_prp_per_port_info].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_general_ippe_sync_counter].name = "ippe_sync_counter";
    submodule_data->features[dnx_data_dev_init_general_ippe_sync_counter].doc = "";
    submodule_data->features[dnx_data_dev_init_general_ippe_sync_counter].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_general_fai_namespace].name = "fai_namespace";
    submodule_data->features[dnx_data_dev_init_general_fai_namespace].doc = "";
    submodule_data->features[dnx_data_dev_init_general_fai_namespace].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_general_rch_remove_field_program_enable].name = "rch_remove_field_program_enable";
    submodule_data->features[dnx_data_dev_init_general_rch_remove_field_program_enable].doc = "";
    submodule_data->features[dnx_data_dev_init_general_rch_remove_field_program_enable].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_dev_init_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dev_init general defines");

    submodule_data->defines[dnx_data_dev_init_general_define_access_only].name = "access_only";
    submodule_data->defines[dnx_data_dev_init_general_define_access_only].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_general_define_access_only].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dev_init_general_define_heat_up].name = "heat_up";
    submodule_data->defines[dnx_data_dev_init_general_define_heat_up].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_general_define_heat_up].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dev_init_general_define_flexe_core_drv_select].name = "flexe_core_drv_select";
    submodule_data->defines[dnx_data_dev_init_general_define_flexe_core_drv_select].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_general_define_flexe_core_drv_select].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_dev_init_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dev_init general tables");

    
    submodule_data->tables[dnx_data_dev_init_general_table_clock_power_down].name = "clock_power_down";
    submodule_data->tables[dnx_data_dev_init_general_table_clock_power_down].doc = "";
    submodule_data->tables[dnx_data_dev_init_general_table_clock_power_down].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dev_init_general_table_clock_power_down].size_of_values = sizeof(dnx_data_dev_init_general_clock_power_down_t);
    submodule_data->tables[dnx_data_dev_init_general_table_clock_power_down].entry_get = dnx_data_dev_init_general_clock_power_down_entry_str_get;

    
    submodule_data->tables[dnx_data_dev_init_general_table_clock_power_down].nof_keys = 1;
    submodule_data->tables[dnx_data_dev_init_general_table_clock_power_down].keys[0].name = "index";
    submodule_data->tables[dnx_data_dev_init_general_table_clock_power_down].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dev_init_general_table_clock_power_down].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dev_init_general_table_clock_power_down].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dev_init_general_table_clock_power_down].nof_values, "_dnx_data_dev_init_general_table_clock_power_down table values");
    submodule_data->tables[dnx_data_dev_init_general_table_clock_power_down].values[0].name = "reg";
    submodule_data->tables[dnx_data_dev_init_general_table_clock_power_down].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_dev_init_general_table_clock_power_down].values[0].doc = "";
    submodule_data->tables[dnx_data_dev_init_general_table_clock_power_down].values[0].offset = UTILEX_OFFSETOF(dnx_data_dev_init_general_clock_power_down_t, reg);
    submodule_data->tables[dnx_data_dev_init_general_table_clock_power_down].values[1].name = "field";
    submodule_data->tables[dnx_data_dev_init_general_table_clock_power_down].values[1].type = "soc_field_t";
    submodule_data->tables[dnx_data_dev_init_general_table_clock_power_down].values[1].doc = "";
    submodule_data->tables[dnx_data_dev_init_general_table_clock_power_down].values[1].offset = UTILEX_OFFSETOF(dnx_data_dev_init_general_clock_power_down_t, field);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_dev_init_general_feature_get(
    int unit,
    dnx_data_dev_init_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_general, feature);
}


uint32
dnx_data_dev_init_general_access_only_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_general, dnx_data_dev_init_general_define_access_only);
}

uint32
dnx_data_dev_init_general_heat_up_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_general, dnx_data_dev_init_general_define_heat_up);
}

uint32
dnx_data_dev_init_general_flexe_core_drv_select_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_general, dnx_data_dev_init_general_define_flexe_core_drv_select);
}



const dnx_data_dev_init_general_clock_power_down_t *
dnx_data_dev_init_general_clock_power_down_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_general, dnx_data_dev_init_general_table_clock_power_down);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dev_init_general_clock_power_down_t *) data;

}


shr_error_e
dnx_data_dev_init_general_clock_power_down_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dev_init_general_clock_power_down_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_general, dnx_data_dev_init_general_table_clock_power_down);
    data = (const dnx_data_dev_init_general_clock_power_down_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reg);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->field);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_dev_init_general_clock_power_down_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_general, dnx_data_dev_init_general_table_clock_power_down);

}






static shr_error_e
dnx_data_dev_init_context_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "context";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_dev_init_context_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dev_init context features");

    submodule_data->features[dnx_data_dev_init_context_context_selection_profile].name = "context_selection_profile";
    submodule_data->features[dnx_data_dev_init_context_context_selection_profile].doc = "";
    submodule_data->features[dnx_data_dev_init_context_context_selection_profile].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_context_oam_sub_type_in_context_selection].name = "oam_sub_type_in_context_selection";
    submodule_data->features[dnx_data_dev_init_context_oam_sub_type_in_context_selection].doc = "";
    submodule_data->features[dnx_data_dev_init_context_oam_sub_type_in_context_selection].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_context_int_profile_in_context_selection].name = "int_profile_in_context_selection";
    submodule_data->features[dnx_data_dev_init_context_int_profile_in_context_selection].doc = "";
    submodule_data->features[dnx_data_dev_init_context_int_profile_in_context_selection].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_dev_init_context_etps_selection_profile].name = "etps_selection_profile";
    submodule_data->features[dnx_data_dev_init_context_etps_selection_profile].doc = "";
    submodule_data->features[dnx_data_dev_init_context_etps_selection_profile].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_dev_init_context_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dev_init context defines");

    submodule_data->defines[dnx_data_dev_init_context_define_forwarding_context_selection_mask_offset].name = "forwarding_context_selection_mask_offset";
    submodule_data->defines[dnx_data_dev_init_context_define_forwarding_context_selection_mask_offset].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_context_define_forwarding_context_selection_mask_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dev_init_context_define_forwarding_context_selection_result_offset].name = "forwarding_context_selection_result_offset";
    submodule_data->defines[dnx_data_dev_init_context_define_forwarding_context_selection_result_offset].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_context_define_forwarding_context_selection_result_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dev_init_context_define_termination_context_selection_mask_offset].name = "termination_context_selection_mask_offset";
    submodule_data->defines[dnx_data_dev_init_context_define_termination_context_selection_mask_offset].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_context_define_termination_context_selection_mask_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dev_init_context_define_termination_context_selection_result_offset].name = "termination_context_selection_result_offset";
    submodule_data->defines[dnx_data_dev_init_context_define_termination_context_selection_result_offset].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_context_define_termination_context_selection_result_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dev_init_context_define_trap_context_selection_mask_offset].name = "trap_context_selection_mask_offset";
    submodule_data->defines[dnx_data_dev_init_context_define_trap_context_selection_mask_offset].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_context_define_trap_context_selection_mask_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dev_init_context_define_trap_context_selection_result_offset].name = "trap_context_selection_result_offset";
    submodule_data->defines[dnx_data_dev_init_context_define_trap_context_selection_result_offset].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_context_define_trap_context_selection_result_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dev_init_context_define_prp_pem_context_selection_mask_offset].name = "prp_pem_context_selection_mask_offset";
    submodule_data->defines[dnx_data_dev_init_context_define_prp_pem_context_selection_mask_offset].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_context_define_prp_pem_context_selection_mask_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dev_init_context_define_prp_context_selection_result_offset].name = "prp_context_selection_result_offset";
    submodule_data->defines[dnx_data_dev_init_context_define_prp_context_selection_result_offset].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_context_define_prp_context_selection_result_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dev_init_context_define_fwd_reycle_priority_size].name = "fwd_reycle_priority_size";
    submodule_data->defines[dnx_data_dev_init_context_define_fwd_reycle_priority_size].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_context_define_fwd_reycle_priority_size].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_dev_init_context_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dev_init context tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_dev_init_context_feature_get(
    int unit,
    dnx_data_dev_init_context_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_context, feature);
}


uint32
dnx_data_dev_init_context_forwarding_context_selection_mask_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_context, dnx_data_dev_init_context_define_forwarding_context_selection_mask_offset);
}

uint32
dnx_data_dev_init_context_forwarding_context_selection_result_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_context, dnx_data_dev_init_context_define_forwarding_context_selection_result_offset);
}

uint32
dnx_data_dev_init_context_termination_context_selection_mask_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_context, dnx_data_dev_init_context_define_termination_context_selection_mask_offset);
}

uint32
dnx_data_dev_init_context_termination_context_selection_result_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_context, dnx_data_dev_init_context_define_termination_context_selection_result_offset);
}

uint32
dnx_data_dev_init_context_trap_context_selection_mask_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_context, dnx_data_dev_init_context_define_trap_context_selection_mask_offset);
}

uint32
dnx_data_dev_init_context_trap_context_selection_result_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_context, dnx_data_dev_init_context_define_trap_context_selection_result_offset);
}

uint32
dnx_data_dev_init_context_prp_pem_context_selection_mask_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_context, dnx_data_dev_init_context_define_prp_pem_context_selection_mask_offset);
}

uint32
dnx_data_dev_init_context_prp_context_selection_result_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_context, dnx_data_dev_init_context_define_prp_context_selection_result_offset);
}

uint32
dnx_data_dev_init_context_fwd_reycle_priority_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_context, dnx_data_dev_init_context_define_fwd_reycle_priority_size);
}










static shr_error_e
dnx_data_dev_init_ha_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ha";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_dev_init_ha_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dev_init ha features");

    submodule_data->features[dnx_data_dev_init_ha_sw_state_diagnostics].name = "sw_state_diagnostics";
    submodule_data->features[dnx_data_dev_init_ha_sw_state_diagnostics].doc = "";
    submodule_data->features[dnx_data_dev_init_ha_sw_state_diagnostics].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_dev_init_ha_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dev_init ha defines");

    submodule_data->defines[dnx_data_dev_init_ha_define_warmboot_support].name = "warmboot_support";
    submodule_data->defines[dnx_data_dev_init_ha_define_warmboot_support].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_ha_define_warmboot_support].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dev_init_ha_define_sw_state_max_size].name = "sw_state_max_size";
    submodule_data->defines[dnx_data_dev_init_ha_define_sw_state_max_size].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_ha_define_sw_state_max_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dev_init_ha_define_stable_location].name = "stable_location";
    submodule_data->defines[dnx_data_dev_init_ha_define_stable_location].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_ha_define_stable_location].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dev_init_ha_define_stable_size].name = "stable_size";
    submodule_data->defines[dnx_data_dev_init_ha_define_stable_size].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_ha_define_stable_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dev_init_ha_define_error_recovery_support].name = "error_recovery_support";
    submodule_data->defines[dnx_data_dev_init_ha_define_error_recovery_support].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_ha_define_error_recovery_support].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_dev_init_ha_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dev_init ha tables");

    
    submodule_data->tables[dnx_data_dev_init_ha_table_stable_filename].name = "stable_filename";
    submodule_data->tables[dnx_data_dev_init_ha_table_stable_filename].doc = "";
    submodule_data->tables[dnx_data_dev_init_ha_table_stable_filename].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dev_init_ha_table_stable_filename].size_of_values = sizeof(dnx_data_dev_init_ha_stable_filename_t);
    submodule_data->tables[dnx_data_dev_init_ha_table_stable_filename].entry_get = dnx_data_dev_init_ha_stable_filename_entry_str_get;

    
    submodule_data->tables[dnx_data_dev_init_ha_table_stable_filename].nof_keys = 0;

    
    submodule_data->tables[dnx_data_dev_init_ha_table_stable_filename].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dev_init_ha_table_stable_filename].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dev_init_ha_table_stable_filename].nof_values, "_dnx_data_dev_init_ha_table_stable_filename table values");
    submodule_data->tables[dnx_data_dev_init_ha_table_stable_filename].values[0].name = "val";
    submodule_data->tables[dnx_data_dev_init_ha_table_stable_filename].values[0].type = "char *";
    submodule_data->tables[dnx_data_dev_init_ha_table_stable_filename].values[0].doc = "";
    submodule_data->tables[dnx_data_dev_init_ha_table_stable_filename].values[0].offset = UTILEX_OFFSETOF(dnx_data_dev_init_ha_stable_filename_t, val);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_dev_init_ha_feature_get(
    int unit,
    dnx_data_dev_init_ha_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_ha, feature);
}


uint32
dnx_data_dev_init_ha_warmboot_support_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_ha, dnx_data_dev_init_ha_define_warmboot_support);
}

uint32
dnx_data_dev_init_ha_sw_state_max_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_ha, dnx_data_dev_init_ha_define_sw_state_max_size);
}

uint32
dnx_data_dev_init_ha_stable_location_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_ha, dnx_data_dev_init_ha_define_stable_location);
}

uint32
dnx_data_dev_init_ha_stable_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_ha, dnx_data_dev_init_ha_define_stable_size);
}

uint32
dnx_data_dev_init_ha_error_recovery_support_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_ha, dnx_data_dev_init_ha_define_error_recovery_support);
}



const dnx_data_dev_init_ha_stable_filename_t *
dnx_data_dev_init_ha_stable_filename_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_ha, dnx_data_dev_init_ha_table_stable_filename);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_dev_init_ha_stable_filename_t *) data;

}


shr_error_e
dnx_data_dev_init_ha_stable_filename_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dev_init_ha_stable_filename_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_ha, dnx_data_dev_init_ha_table_stable_filename);
    data = (const dnx_data_dev_init_ha_stable_filename_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->val == NULL ? "" : data->val);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_dev_init_ha_stable_filename_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_ha, dnx_data_dev_init_ha_table_stable_filename);

}






static shr_error_e
dnx_data_dev_init_shadow_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "shadow";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_dev_init_shadow_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dev_init shadow features");

    
    submodule_data->nof_defines = _dnx_data_dev_init_shadow_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dev_init shadow defines");

    submodule_data->defines[dnx_data_dev_init_shadow_define_cache_enable_all].name = "cache_enable_all";
    submodule_data->defines[dnx_data_dev_init_shadow_define_cache_enable_all].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_shadow_define_cache_enable_all].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dev_init_shadow_define_cache_enable_ecc].name = "cache_enable_ecc";
    submodule_data->defines[dnx_data_dev_init_shadow_define_cache_enable_ecc].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_shadow_define_cache_enable_ecc].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dev_init_shadow_define_cache_enable_parity].name = "cache_enable_parity";
    submodule_data->defines[dnx_data_dev_init_shadow_define_cache_enable_parity].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_shadow_define_cache_enable_parity].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dev_init_shadow_define_cache_enable_specific].name = "cache_enable_specific";
    submodule_data->defines[dnx_data_dev_init_shadow_define_cache_enable_specific].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_shadow_define_cache_enable_specific].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dev_init_shadow_define_cache_disable_specific].name = "cache_disable_specific";
    submodule_data->defines[dnx_data_dev_init_shadow_define_cache_disable_specific].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_shadow_define_cache_disable_specific].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_dev_init_shadow_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dev_init shadow tables");

    
    submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_mem].name = "uncacheable_mem";
    submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_mem].doc = "";
    submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_mem].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_mem].size_of_values = sizeof(dnx_data_dev_init_shadow_uncacheable_mem_t);
    submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_mem].entry_get = dnx_data_dev_init_shadow_uncacheable_mem_entry_str_get;

    
    submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_mem].nof_keys = 1;
    submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_mem].keys[0].name = "index";
    submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_mem].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_mem].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_mem].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_mem].nof_values, "_dnx_data_dev_init_shadow_table_uncacheable_mem table values");
    submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_mem].values[0].name = "mem";
    submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_mem].values[0].type = "soc_mem_t";
    submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_mem].values[0].doc = "";
    submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_mem].values[0].offset = UTILEX_OFFSETOF(dnx_data_dev_init_shadow_uncacheable_mem_t, mem);

#ifdef BCM_ACCESS_SUPPORT

    
    submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_regmem].name = "uncacheable_regmem";
    submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_regmem].doc = "";
    submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_regmem].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_regmem].size_of_values = sizeof(dnx_data_dev_init_shadow_uncacheable_regmem_t);
    submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_regmem].entry_get = dnx_data_dev_init_shadow_uncacheable_regmem_entry_str_get;

    
    submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_regmem].nof_keys = 1;
    submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_regmem].keys[0].name = "index";
    submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_regmem].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_regmem].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_regmem].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_regmem].nof_values, "_dnx_data_dev_init_shadow_table_uncacheable_regmem table values");
    submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_regmem].values[0].name = "regmem";
    submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_regmem].values[0].type = "uint32";
    submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_regmem].values[0].doc = "";
    submodule_data->tables[dnx_data_dev_init_shadow_table_uncacheable_regmem].values[0].offset = UTILEX_OFFSETOF(dnx_data_dev_init_shadow_uncacheable_regmem_t, regmem);

#endif 

exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_dev_init_shadow_feature_get(
    int unit,
    dnx_data_dev_init_shadow_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_shadow, feature);
}


uint32
dnx_data_dev_init_shadow_cache_enable_all_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_shadow, dnx_data_dev_init_shadow_define_cache_enable_all);
}

uint32
dnx_data_dev_init_shadow_cache_enable_ecc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_shadow, dnx_data_dev_init_shadow_define_cache_enable_ecc);
}

uint32
dnx_data_dev_init_shadow_cache_enable_parity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_shadow, dnx_data_dev_init_shadow_define_cache_enable_parity);
}

uint32
dnx_data_dev_init_shadow_cache_enable_specific_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_shadow, dnx_data_dev_init_shadow_define_cache_enable_specific);
}

uint32
dnx_data_dev_init_shadow_cache_disable_specific_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_shadow, dnx_data_dev_init_shadow_define_cache_disable_specific);
}



const dnx_data_dev_init_shadow_uncacheable_mem_t *
dnx_data_dev_init_shadow_uncacheable_mem_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_shadow, dnx_data_dev_init_shadow_table_uncacheable_mem);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dev_init_shadow_uncacheable_mem_t *) data;

}

#ifdef BCM_ACCESS_SUPPORT

const dnx_data_dev_init_shadow_uncacheable_regmem_t *
dnx_data_dev_init_shadow_uncacheable_regmem_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_shadow, dnx_data_dev_init_shadow_table_uncacheable_regmem);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dev_init_shadow_uncacheable_regmem_t *) data;

}


#endif 

shr_error_e
dnx_data_dev_init_shadow_uncacheable_mem_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dev_init_shadow_uncacheable_mem_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_shadow, dnx_data_dev_init_shadow_table_uncacheable_mem);
    data = (const dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mem);
            break;
    }

    SHR_FUNC_EXIT;
}

#ifdef BCM_ACCESS_SUPPORT

shr_error_e
dnx_data_dev_init_shadow_uncacheable_regmem_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dev_init_shadow_uncacheable_regmem_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_shadow, dnx_data_dev_init_shadow_table_uncacheable_regmem);
    data = (const dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->regmem);
            break;
    }

    SHR_FUNC_EXIT;
}


#endif 

const dnxc_data_table_info_t *
dnx_data_dev_init_shadow_uncacheable_mem_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_shadow, dnx_data_dev_init_shadow_table_uncacheable_mem);

}

#ifdef BCM_ACCESS_SUPPORT

const dnxc_data_table_info_t *
dnx_data_dev_init_shadow_uncacheable_regmem_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_shadow, dnx_data_dev_init_shadow_table_uncacheable_regmem);

}


#endif 





static shr_error_e
dnx_data_dev_init_cmc_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "cmc";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_dev_init_cmc_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dev_init cmc features");

    
    submodule_data->nof_defines = _dnx_data_dev_init_cmc_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dev_init cmc defines");

    submodule_data->defines[dnx_data_dev_init_cmc_define_cmc_num].name = "cmc_num";
    submodule_data->defines[dnx_data_dev_init_cmc_define_cmc_num].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_cmc_define_cmc_num].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dev_init_cmc_define_num_cpu_cosq].name = "num_cpu_cosq";
    submodule_data->defines[dnx_data_dev_init_cmc_define_num_cpu_cosq].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_cmc_define_num_cpu_cosq].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_dev_init_cmc_define_cmc_pci].name = "cmc_pci";
    submodule_data->defines[dnx_data_dev_init_cmc_define_cmc_pci].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_cmc_define_cmc_pci].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dev_init_cmc_define_pci_cmc_num].name = "pci_cmc_num";
    submodule_data->defines[dnx_data_dev_init_cmc_define_pci_cmc_num].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_cmc_define_pci_cmc_num].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dev_init_cmc_define_cmc_uc0].name = "cmc_uc0";
    submodule_data->defines[dnx_data_dev_init_cmc_define_cmc_uc0].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_cmc_define_cmc_uc0].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dev_init_cmc_define_cmc_uc1].name = "cmc_uc1";
    submodule_data->defines[dnx_data_dev_init_cmc_define_cmc_uc1].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_cmc_define_cmc_uc1].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dev_init_cmc_define_num_queues_pci].name = "num_queues_pci";
    submodule_data->defines[dnx_data_dev_init_cmc_define_num_queues_pci].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_cmc_define_num_queues_pci].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dev_init_cmc_define_num_queues_uc0].name = "num_queues_uc0";
    submodule_data->defines[dnx_data_dev_init_cmc_define_num_queues_uc0].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_cmc_define_num_queues_uc0].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_dev_init_cmc_define_num_queues_uc1].name = "num_queues_uc1";
    submodule_data->defines[dnx_data_dev_init_cmc_define_num_queues_uc1].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_cmc_define_num_queues_uc1].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_dev_init_cmc_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dev_init cmc tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_dev_init_cmc_feature_get(
    int unit,
    dnx_data_dev_init_cmc_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_cmc, feature);
}


uint32
dnx_data_dev_init_cmc_cmc_num_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_cmc, dnx_data_dev_init_cmc_define_cmc_num);
}

uint32
dnx_data_dev_init_cmc_num_cpu_cosq_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_cmc, dnx_data_dev_init_cmc_define_num_cpu_cosq);
}

uint32
dnx_data_dev_init_cmc_cmc_pci_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_cmc, dnx_data_dev_init_cmc_define_cmc_pci);
}

uint32
dnx_data_dev_init_cmc_pci_cmc_num_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_cmc, dnx_data_dev_init_cmc_define_pci_cmc_num);
}

uint32
dnx_data_dev_init_cmc_cmc_uc0_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_cmc, dnx_data_dev_init_cmc_define_cmc_uc0);
}

uint32
dnx_data_dev_init_cmc_cmc_uc1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_cmc, dnx_data_dev_init_cmc_define_cmc_uc1);
}

uint32
dnx_data_dev_init_cmc_num_queues_pci_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_cmc, dnx_data_dev_init_cmc_define_num_queues_pci);
}

uint32
dnx_data_dev_init_cmc_num_queues_uc0_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_cmc, dnx_data_dev_init_cmc_define_num_queues_uc0);
}

uint32
dnx_data_dev_init_cmc_num_queues_uc1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_cmc, dnx_data_dev_init_cmc_define_num_queues_uc1);
}










static shr_error_e
dnx_data_dev_init_rx_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "rx";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_dev_init_rx_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dev_init rx features");

    
    submodule_data->nof_defines = _dnx_data_dev_init_rx_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dev_init rx defines");

    submodule_data->defines[dnx_data_dev_init_rx_define_rx_pool_nof_pkts].name = "rx_pool_nof_pkts";
    submodule_data->defines[dnx_data_dev_init_rx_define_rx_pool_nof_pkts].doc = "";
    
    submodule_data->defines[dnx_data_dev_init_rx_define_rx_pool_nof_pkts].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_dev_init_rx_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dev_init rx tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_dev_init_rx_feature_get(
    int unit,
    dnx_data_dev_init_rx_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_rx, feature);
}


uint32
dnx_data_dev_init_rx_rx_pool_nof_pkts_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dev_init, dnx_data_dev_init_submodule_rx, dnx_data_dev_init_rx_define_rx_pool_nof_pkts);
}







shr_error_e
dnx_data_dev_init_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "dev_init";
    module_data->nof_submodules = _dnx_data_dev_init_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data dev_init submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_dev_init_time_init(unit, &module_data->submodules[dnx_data_dev_init_submodule_time]));
    SHR_IF_ERR_EXIT(dnx_data_dev_init_mem_init(unit, &module_data->submodules[dnx_data_dev_init_submodule_mem]));
    SHR_IF_ERR_EXIT(dnx_data_dev_init_properties_init(unit, &module_data->submodules[dnx_data_dev_init_submodule_properties]));
    SHR_IF_ERR_EXIT(dnx_data_dev_init_general_init(unit, &module_data->submodules[dnx_data_dev_init_submodule_general]));
    SHR_IF_ERR_EXIT(dnx_data_dev_init_context_init(unit, &module_data->submodules[dnx_data_dev_init_submodule_context]));
    SHR_IF_ERR_EXIT(dnx_data_dev_init_ha_init(unit, &module_data->submodules[dnx_data_dev_init_submodule_ha]));
    SHR_IF_ERR_EXIT(dnx_data_dev_init_shadow_init(unit, &module_data->submodules[dnx_data_dev_init_submodule_shadow]));
    SHR_IF_ERR_EXIT(dnx_data_dev_init_cmc_init(unit, &module_data->submodules[dnx_data_dev_init_submodule_cmc]));
    SHR_IF_ERR_EXIT(dnx_data_dev_init_rx_init(unit, &module_data->submodules[dnx_data_dev_init_submodule_rx]));
    

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dev_init_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dev_init_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_dev_init_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dev_init_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_dev_init_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dev_init_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_dev_init_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dev_init_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_dev_init_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dev_init_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_dev_init_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dev_init_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_dev_init_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_dev_init_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dev_init_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_dev_init_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_dev_init_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dev_init_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_dev_init_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dev_init_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_dev_init_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dev_init_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_dev_init_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dev_init_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_dev_init_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

