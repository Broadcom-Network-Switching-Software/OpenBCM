
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PVT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_pvt.h>




extern shr_error_e jr2_a0_data_pvt_attach(
    int unit);


extern shr_error_e j2c_a0_data_pvt_attach(
    int unit);


extern shr_error_e q2a_a0_data_pvt_attach(
    int unit);


extern shr_error_e j2p_a0_data_pvt_attach(
    int unit);


extern shr_error_e j2x_a0_data_pvt_attach(
    int unit);




static shr_error_e
dnx_data_pvt_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_pvt_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data pvt general features");

    submodule_data->features[dnx_data_pvt_general_pvt_temp_monitors_hw_support].name = "pvt_temp_monitors_hw_support";
    submodule_data->features[dnx_data_pvt_general_pvt_temp_monitors_hw_support].doc = "";
    submodule_data->features[dnx_data_pvt_general_pvt_temp_monitors_hw_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_pvt_general_pvt_peak_clear_support].name = "pvt_peak_clear_support";
    submodule_data->features[dnx_data_pvt_general_pvt_peak_clear_support].doc = "";
    submodule_data->features[dnx_data_pvt_general_pvt_peak_clear_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_pvt_general_pvt_peak_faulty].name = "pvt_peak_faulty";
    submodule_data->features[dnx_data_pvt_general_pvt_peak_faulty].doc = "";
    submodule_data->features[dnx_data_pvt_general_pvt_peak_faulty].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_pvt_general_pvt_lock_support].name = "pvt_lock_support";
    submodule_data->features[dnx_data_pvt_general_pvt_lock_support].doc = "";
    submodule_data->features[dnx_data_pvt_general_pvt_lock_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_pvt_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data pvt general defines");

    submodule_data->defines[dnx_data_pvt_general_define_nof_pvt_monitors].name = "nof_pvt_monitors";
    submodule_data->defines[dnx_data_pvt_general_define_nof_pvt_monitors].doc = "";
    
    submodule_data->defines[dnx_data_pvt_general_define_nof_pvt_monitors].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pvt_general_define_pvt_base].name = "pvt_base";
    submodule_data->defines[dnx_data_pvt_general_define_pvt_base].doc = "";
    
    submodule_data->defines[dnx_data_pvt_general_define_pvt_base].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pvt_general_define_pvt_factor].name = "pvt_factor";
    submodule_data->defines[dnx_data_pvt_general_define_pvt_factor].doc = "";
    
    submodule_data->defines[dnx_data_pvt_general_define_pvt_factor].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pvt_general_define_nof_main_monitors].name = "nof_main_monitors";
    submodule_data->defines[dnx_data_pvt_general_define_nof_main_monitors].doc = "";
    
    submodule_data->defines[dnx_data_pvt_general_define_nof_main_monitors].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pvt_general_define_pvt_mon_control_sel_nof_bits].name = "pvt_mon_control_sel_nof_bits";
    submodule_data->defines[dnx_data_pvt_general_define_pvt_mon_control_sel_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_pvt_general_define_pvt_mon_control_sel_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pvt_general_define_thermal_data_nof_bits].name = "thermal_data_nof_bits";
    submodule_data->defines[dnx_data_pvt_general_define_thermal_data_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_pvt_general_define_thermal_data_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pvt_general_define_vol_sel].name = "vol_sel";
    submodule_data->defines[dnx_data_pvt_general_define_vol_sel].doc = "";
    
    submodule_data->defines[dnx_data_pvt_general_define_vol_sel].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pvt_general_define_vol_factor].name = "vol_factor";
    submodule_data->defines[dnx_data_pvt_general_define_vol_factor].doc = "";
    
    submodule_data->defines[dnx_data_pvt_general_define_vol_factor].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pvt_general_define_vol_multiple].name = "vol_multiple";
    submodule_data->defines[dnx_data_pvt_general_define_vol_multiple].doc = "";
    
    submodule_data->defines[dnx_data_pvt_general_define_vol_multiple].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pvt_general_define_vol_divisor].name = "vol_divisor";
    submodule_data->defines[dnx_data_pvt_general_define_vol_divisor].doc = "";
    
    submodule_data->defines[dnx_data_pvt_general_define_vol_divisor].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_pvt_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data pvt general tables");

    
    submodule_data->tables[dnx_data_pvt_general_table_pvt_name].name = "pvt_name";
    submodule_data->tables[dnx_data_pvt_general_table_pvt_name].doc = "";
    submodule_data->tables[dnx_data_pvt_general_table_pvt_name].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_pvt_general_table_pvt_name].size_of_values = sizeof(dnx_data_pvt_general_pvt_name_t);
    submodule_data->tables[dnx_data_pvt_general_table_pvt_name].entry_get = dnx_data_pvt_general_pvt_name_entry_str_get;

    
    submodule_data->tables[dnx_data_pvt_general_table_pvt_name].nof_keys = 1;
    submodule_data->tables[dnx_data_pvt_general_table_pvt_name].keys[0].name = "index";
    submodule_data->tables[dnx_data_pvt_general_table_pvt_name].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_pvt_general_table_pvt_name].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_pvt_general_table_pvt_name].values, dnxc_data_value_t, submodule_data->tables[dnx_data_pvt_general_table_pvt_name].nof_values, "_dnx_data_pvt_general_table_pvt_name table values");
    submodule_data->tables[dnx_data_pvt_general_table_pvt_name].values[0].name = "display_name";
    submodule_data->tables[dnx_data_pvt_general_table_pvt_name].values[0].type = "char *";
    submodule_data->tables[dnx_data_pvt_general_table_pvt_name].values[0].doc = "";
    submodule_data->tables[dnx_data_pvt_general_table_pvt_name].values[0].offset = UTILEX_OFFSETOF(dnx_data_pvt_general_pvt_name_t, display_name);
    submodule_data->tables[dnx_data_pvt_general_table_pvt_name].values[1].name = "valid";
    submodule_data->tables[dnx_data_pvt_general_table_pvt_name].values[1].type = "uint32";
    submodule_data->tables[dnx_data_pvt_general_table_pvt_name].values[1].doc = "";
    submodule_data->tables[dnx_data_pvt_general_table_pvt_name].values[1].offset = UTILEX_OFFSETOF(dnx_data_pvt_general_pvt_name_t, valid);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_pvt_general_feature_get(
    int unit,
    dnx_data_pvt_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_pvt, dnx_data_pvt_submodule_general, feature);
}


uint32
dnx_data_pvt_general_nof_pvt_monitors_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pvt, dnx_data_pvt_submodule_general, dnx_data_pvt_general_define_nof_pvt_monitors);
}

uint32
dnx_data_pvt_general_pvt_base_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pvt, dnx_data_pvt_submodule_general, dnx_data_pvt_general_define_pvt_base);
}

uint32
dnx_data_pvt_general_pvt_factor_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pvt, dnx_data_pvt_submodule_general, dnx_data_pvt_general_define_pvt_factor);
}

uint32
dnx_data_pvt_general_nof_main_monitors_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pvt, dnx_data_pvt_submodule_general, dnx_data_pvt_general_define_nof_main_monitors);
}

uint32
dnx_data_pvt_general_pvt_mon_control_sel_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pvt, dnx_data_pvt_submodule_general, dnx_data_pvt_general_define_pvt_mon_control_sel_nof_bits);
}

uint32
dnx_data_pvt_general_thermal_data_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pvt, dnx_data_pvt_submodule_general, dnx_data_pvt_general_define_thermal_data_nof_bits);
}

uint32
dnx_data_pvt_general_vol_sel_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pvt, dnx_data_pvt_submodule_general, dnx_data_pvt_general_define_vol_sel);
}

uint32
dnx_data_pvt_general_vol_factor_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pvt, dnx_data_pvt_submodule_general, dnx_data_pvt_general_define_vol_factor);
}

uint32
dnx_data_pvt_general_vol_multiple_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pvt, dnx_data_pvt_submodule_general, dnx_data_pvt_general_define_vol_multiple);
}

uint32
dnx_data_pvt_general_vol_divisor_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pvt, dnx_data_pvt_submodule_general, dnx_data_pvt_general_define_vol_divisor);
}



const dnx_data_pvt_general_pvt_name_t *
dnx_data_pvt_general_pvt_name_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_pvt, dnx_data_pvt_submodule_general, dnx_data_pvt_general_table_pvt_name);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_pvt_general_pvt_name_t *) data;

}


shr_error_e
dnx_data_pvt_general_pvt_name_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_pvt_general_pvt_name_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_pvt, dnx_data_pvt_submodule_general, dnx_data_pvt_general_table_pvt_name);
    data = (const dnx_data_pvt_general_pvt_name_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->display_name == NULL ? "" : data->display_name);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_pvt_general_pvt_name_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_pvt, dnx_data_pvt_submodule_general, dnx_data_pvt_general_table_pvt_name);

}






static shr_error_e
dnx_data_pvt_vtmon_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "vtmon";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_pvt_vtmon_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data pvt vtmon features");

    submodule_data->features[dnx_data_pvt_vtmon_remote_temp_sensors_supported].name = "remote_temp_sensors_supported";
    submodule_data->features[dnx_data_pvt_vtmon_remote_temp_sensors_supported].doc = "";
    submodule_data->features[dnx_data_pvt_vtmon_remote_temp_sensors_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_pvt_vtmon_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data pvt vtmon defines");

    
    submodule_data->nof_tables = _dnx_data_pvt_vtmon_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data pvt vtmon tables");

    
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].name = "vtmon_access";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].doc = "";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].size_of_values = sizeof(dnx_data_pvt_vtmon_vtmon_access_t);
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].entry_get = dnx_data_pvt_vtmon_vtmon_access_entry_str_get;

    
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].nof_keys = 1;
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].keys[0].name = "index";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].nof_values = 14;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values, dnxc_data_value_t, submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].nof_values, "_dnx_data_pvt_vtmon_table_vtmon_access table values");
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[0].name = "name";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[0].type = "char *";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[0].doc = "";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[0].offset = UTILEX_OFFSETOF(dnx_data_pvt_vtmon_vtmon_access_t, name);
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[1].name = "control_reg";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[1].type = "int";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[1].doc = "";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[1].offset = UTILEX_OFFSETOF(dnx_data_pvt_vtmon_vtmon_access_t, control_reg);
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[2].name = "lock_bit";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[2].type = "int";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[2].doc = "";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[2].offset = UTILEX_OFFSETOF(dnx_data_pvt_vtmon_vtmon_access_t, lock_bit);
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[3].name = "remote_sensor_supported";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[3].type = "int";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[3].doc = "";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[3].offset = UTILEX_OFFSETOF(dnx_data_pvt_vtmon_vtmon_access_t, remote_sensor_supported);
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[4].name = "vtmon_sel_field";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[4].type = "uint32";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[4].doc = "";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[4].offset = UTILEX_OFFSETOF(dnx_data_pvt_vtmon_vtmon_access_t, vtmon_sel_field);
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[5].name = "peak_data_clear_field";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[5].type = "int";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[5].doc = "";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[5].offset = UTILEX_OFFSETOF(dnx_data_pvt_vtmon_vtmon_access_t, peak_data_clear_field);
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[6].name = "temp_mon_sel";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[6].type = "int";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[6].doc = "";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[6].offset = UTILEX_OFFSETOF(dnx_data_pvt_vtmon_vtmon_access_t, temp_mon_sel);
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[7].name = "data_reg";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[7].type = "int";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[7].doc = "";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[7].offset = UTILEX_OFFSETOF(dnx_data_pvt_vtmon_vtmon_access_t, data_reg);
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[8].name = "thermal_data_field";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[8].type = "int";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[8].doc = "";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[8].offset = UTILEX_OFFSETOF(dnx_data_pvt_vtmon_vtmon_access_t, thermal_data_field);
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[9].name = "peak_data_field";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[9].type = "int";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[9].doc = "";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[9].offset = UTILEX_OFFSETOF(dnx_data_pvt_vtmon_vtmon_access_t, peak_data_field);
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[10].name = "block_instance";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[10].type = "int";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[10].doc = "";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[10].offset = UTILEX_OFFSETOF(dnx_data_pvt_vtmon_vtmon_access_t, block_instance);
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[11].name = "voltage_sensing";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[11].type = "int";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[11].doc = "";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[11].offset = UTILEX_OFFSETOF(dnx_data_pvt_vtmon_vtmon_access_t, voltage_sensing);
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[12].name = "voltage_sel";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[12].type = "uint32";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[12].doc = "";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[12].offset = UTILEX_OFFSETOF(dnx_data_pvt_vtmon_vtmon_access_t, voltage_sel);
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[13].name = "voltage_bit";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[13].type = "int";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[13].doc = "";
    submodule_data->tables[dnx_data_pvt_vtmon_table_vtmon_access].values[13].offset = UTILEX_OFFSETOF(dnx_data_pvt_vtmon_vtmon_access_t, voltage_bit);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_pvt_vtmon_feature_get(
    int unit,
    dnx_data_pvt_vtmon_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_pvt, dnx_data_pvt_submodule_vtmon, feature);
}




const dnx_data_pvt_vtmon_vtmon_access_t *
dnx_data_pvt_vtmon_vtmon_access_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_pvt, dnx_data_pvt_submodule_vtmon, dnx_data_pvt_vtmon_table_vtmon_access);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_pvt_vtmon_vtmon_access_t *) data;

}


shr_error_e
dnx_data_pvt_vtmon_vtmon_access_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_pvt_vtmon_vtmon_access_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_pvt, dnx_data_pvt_submodule_vtmon, dnx_data_pvt_vtmon_table_vtmon_access);
    data = (const dnx_data_pvt_vtmon_vtmon_access_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->name == NULL ? "" : data->name);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->control_reg);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->lock_bit);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->remote_sensor_supported);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->vtmon_sel_field);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->peak_data_clear_field);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->temp_mon_sel);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->data_reg);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->thermal_data_field);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->peak_data_field);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->block_instance);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->voltage_sensing);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->voltage_sel);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->voltage_bit);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_pvt_vtmon_vtmon_access_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_pvt, dnx_data_pvt_submodule_vtmon, dnx_data_pvt_vtmon_table_vtmon_access);

}



shr_error_e
dnx_data_pvt_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "pvt";
    module_data->nof_submodules = _dnx_data_pvt_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data pvt submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_pvt_general_init(unit, &module_data->submodules[dnx_data_pvt_submodule_general]));
    SHR_IF_ERR_EXIT(dnx_data_pvt_vtmon_init(unit, &module_data->submodules[dnx_data_pvt_submodule_vtmon]));
    

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pvt_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pvt_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pvt_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pvt_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_pvt_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pvt_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_pvt_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pvt_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_pvt_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pvt_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_pvt_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pvt_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_pvt_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pvt_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_pvt_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pvt_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_pvt_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pvt_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_pvt_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pvt_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_pvt_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

