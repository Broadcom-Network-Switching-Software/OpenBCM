
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DEVICE

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal_device.h>



#ifdef BCM_DNXF1_SUPPORT

extern shr_error_e ramon_a0_data_device_attach(
    int unit);

#endif 



static shr_error_e
dnxf_data_device_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnxf_data_device_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data device general features");

    submodule_data->features[dnxf_data_device_general_production_ready].name = "production_ready";
    submodule_data->features[dnxf_data_device_general_production_ready].doc = "";
    submodule_data->features[dnxf_data_device_general_production_ready].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnxf_data_device_general_serdes_support].name = "serdes_support";
    submodule_data->features[dnxf_data_device_general_serdes_support].doc = "";
    submodule_data->features[dnxf_data_device_general_serdes_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnxf_data_device_general_fabric_support].name = "fabric_support";
    submodule_data->features[dnxf_data_device_general_fabric_support].doc = "";
    submodule_data->features[dnxf_data_device_general_fabric_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnxf_data_device_general_delay_exist].name = "delay_exist";
    submodule_data->features[dnxf_data_device_general_delay_exist].doc = "";
    submodule_data->features[dnxf_data_device_general_delay_exist].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnxf_data_device_general_cmic_support].name = "cmic_support";
    submodule_data->features[dnxf_data_device_general_cmic_support].doc = "";
    submodule_data->features[dnxf_data_device_general_cmic_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnxf_data_device_general_real_interrupts_support].name = "real_interrupts_support";
    submodule_data->features[dnxf_data_device_general_real_interrupts_support].doc = "";
    submodule_data->features[dnxf_data_device_general_real_interrupts_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnxf_data_device_general_i2c_hw_support].name = "i2c_hw_support";
    submodule_data->features[dnxf_data_device_general_i2c_hw_support].doc = "";
    submodule_data->features[dnxf_data_device_general_i2c_hw_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnxf_data_device_general_hw_support].name = "hw_support";
    submodule_data->features[dnxf_data_device_general_hw_support].doc = "";
    submodule_data->features[dnxf_data_device_general_hw_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnxf_data_device_general_is_not_all_cores_active].name = "is_not_all_cores_active";
    submodule_data->features[dnxf_data_device_general_is_not_all_cores_active].doc = "";
    submodule_data->features[dnxf_data_device_general_is_not_all_cores_active].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnxf_data_device_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data device general defines");

    submodule_data->defines[dnxf_data_device_general_define_nof_pvt_monitors].name = "nof_pvt_monitors";
    submodule_data->defines[dnxf_data_device_general_define_nof_pvt_monitors].doc = "";
    submodule_data->defines[dnxf_data_device_general_define_nof_pvt_monitors].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_device_general_define_nof_pvt_monitors].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_general_define_pvt_base].name = "pvt_base";
    submodule_data->defines[dnxf_data_device_general_define_pvt_base].doc = "";
    submodule_data->defines[dnxf_data_device_general_define_pvt_base].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_device_general_define_pvt_base].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_general_define_pvt_factor].name = "pvt_factor";
    submodule_data->defines[dnxf_data_device_general_define_pvt_factor].doc = "";
    submodule_data->defines[dnxf_data_device_general_define_pvt_factor].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_device_general_define_pvt_factor].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_general_define_nof_cores].name = "nof_cores";
    submodule_data->defines[dnxf_data_device_general_define_nof_cores].doc = "";
    
    submodule_data->defines[dnxf_data_device_general_define_nof_cores].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_general_define_adapter_lib_ver].name = "adapter_lib_ver";
    submodule_data->defines[dnxf_data_device_general_define_adapter_lib_ver].doc = "";
    
    submodule_data->defines[dnxf_data_device_general_define_adapter_lib_ver].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_general_define_nof_mesh_topology_links_data].name = "nof_mesh_topology_links_data";
    submodule_data->defines[dnxf_data_device_general_define_nof_mesh_topology_links_data].doc = "";
    
    submodule_data->defines[dnxf_data_device_general_define_nof_mesh_topology_links_data].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_general_define_hard_reset_disable].name = "hard_reset_disable";
    submodule_data->defines[dnxf_data_device_general_define_hard_reset_disable].doc = "";
    
    submodule_data->defines[dnxf_data_device_general_define_hard_reset_disable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_general_define_core_clock_khz].name = "core_clock_khz";
    submodule_data->defines[dnxf_data_device_general_define_core_clock_khz].doc = "";
    
    submodule_data->defines[dnxf_data_device_general_define_core_clock_khz].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_general_define_system_ref_core_clock_khz].name = "system_ref_core_clock_khz";
    submodule_data->defines[dnxf_data_device_general_define_system_ref_core_clock_khz].doc = "";
    
    submodule_data->defines[dnxf_data_device_general_define_system_ref_core_clock_khz].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_general_define_device_id].name = "device_id";
    submodule_data->defines[dnxf_data_device_general_define_device_id].doc = "";
    
    submodule_data->defines[dnxf_data_device_general_define_device_id].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnxf_data_device_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data device general tables");

    
    submodule_data->tables[dnxf_data_device_general_table_ctest_full_file_indentifier].name = "ctest_full_file_indentifier";
    submodule_data->tables[dnxf_data_device_general_table_ctest_full_file_indentifier].doc = "";
    submodule_data->tables[dnxf_data_device_general_table_ctest_full_file_indentifier].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_device_general_table_ctest_full_file_indentifier].size_of_values = sizeof(dnxf_data_device_general_ctest_full_file_indentifier_t);
    submodule_data->tables[dnxf_data_device_general_table_ctest_full_file_indentifier].entry_get = dnxf_data_device_general_ctest_full_file_indentifier_entry_str_get;

    
    submodule_data->tables[dnxf_data_device_general_table_ctest_full_file_indentifier].nof_keys = 0;

    
    submodule_data->tables[dnxf_data_device_general_table_ctest_full_file_indentifier].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_device_general_table_ctest_full_file_indentifier].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_device_general_table_ctest_full_file_indentifier].nof_values, "_dnxf_data_device_general_table_ctest_full_file_indentifier table values");
    submodule_data->tables[dnxf_data_device_general_table_ctest_full_file_indentifier].values[0].name = "family_name";
    submodule_data->tables[dnxf_data_device_general_table_ctest_full_file_indentifier].values[0].type = "char *";
    submodule_data->tables[dnxf_data_device_general_table_ctest_full_file_indentifier].values[0].doc = "Should be DNXF.";
    submodule_data->tables[dnxf_data_device_general_table_ctest_full_file_indentifier].values[0].offset = UTILEX_OFFSETOF(dnxf_data_device_general_ctest_full_file_indentifier_t, family_name);
    submodule_data->tables[dnxf_data_device_general_table_ctest_full_file_indentifier].values[1].name = "family_number";
    submodule_data->tables[dnxf_data_device_general_table_ctest_full_file_indentifier].values[1].type = "char *";
    submodule_data->tables[dnxf_data_device_general_table_ctest_full_file_indentifier].values[1].doc = "The value can be \0, 3, etc. If the number is \0, define will be counted as NULL. It depends on whether the generated tcl/tlist file names contains number!";
    submodule_data->tables[dnxf_data_device_general_table_ctest_full_file_indentifier].values[1].offset = UTILEX_OFFSETOF(dnxf_data_device_general_ctest_full_file_indentifier_t, family_number);


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_device_general_feature_get(
    int unit,
    dnxf_data_device_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_general, feature);
}


uint32
dnxf_data_device_general_nof_pvt_monitors_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_general, dnxf_data_device_general_define_nof_pvt_monitors);
}

int
dnxf_data_device_general_pvt_base_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_general, dnxf_data_device_general_define_pvt_base);
}

int
dnxf_data_device_general_pvt_factor_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_general, dnxf_data_device_general_define_pvt_factor);
}

uint32
dnxf_data_device_general_nof_cores_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_general, dnxf_data_device_general_define_nof_cores);
}

uint32
dnxf_data_device_general_adapter_lib_ver_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_general, dnxf_data_device_general_define_adapter_lib_ver);
}

uint32
dnxf_data_device_general_nof_mesh_topology_links_data_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_general, dnxf_data_device_general_define_nof_mesh_topology_links_data);
}

uint32
dnxf_data_device_general_hard_reset_disable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_general, dnxf_data_device_general_define_hard_reset_disable);
}

uint32
dnxf_data_device_general_core_clock_khz_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_general, dnxf_data_device_general_define_core_clock_khz);
}

uint32
dnxf_data_device_general_system_ref_core_clock_khz_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_general, dnxf_data_device_general_define_system_ref_core_clock_khz);
}

uint32
dnxf_data_device_general_device_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_general, dnxf_data_device_general_define_device_id);
}



const dnxf_data_device_general_ctest_full_file_indentifier_t *
dnxf_data_device_general_ctest_full_file_indentifier_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_general, dnxf_data_device_general_table_ctest_full_file_indentifier);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnxf_data_device_general_ctest_full_file_indentifier_t *) data;

}


shr_error_e
dnxf_data_device_general_ctest_full_file_indentifier_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_device_general_ctest_full_file_indentifier_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_general, dnxf_data_device_general_table_ctest_full_file_indentifier);
    data = (const dnxf_data_device_general_ctest_full_file_indentifier_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->family_name == NULL ? "" : data->family_name);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->family_number == NULL ? "" : data->family_number);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnxf_data_device_general_ctest_full_file_indentifier_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_general, dnxf_data_device_general_table_ctest_full_file_indentifier);

}






static shr_error_e
dnxf_data_device_access_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "access";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnxf_data_device_access_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data device access features");

    
    submodule_data->nof_defines = _dnxf_data_device_access_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data device access defines");

    submodule_data->defines[dnxf_data_device_access_define_table_dma_enable].name = "table_dma_enable";
    submodule_data->defines[dnxf_data_device_access_define_table_dma_enable].doc = "";
    submodule_data->defines[dnxf_data_device_access_define_table_dma_enable].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_device_access_define_table_dma_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_access_define_tdma_timeout_usec].name = "tdma_timeout_usec";
    submodule_data->defines[dnxf_data_device_access_define_tdma_timeout_usec].doc = "";
    submodule_data->defines[dnxf_data_device_access_define_tdma_timeout_usec].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_device_access_define_tdma_timeout_usec].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_access_define_tdma_intr_enable].name = "tdma_intr_enable";
    submodule_data->defines[dnxf_data_device_access_define_tdma_intr_enable].doc = "";
    submodule_data->defines[dnxf_data_device_access_define_tdma_intr_enable].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_device_access_define_tdma_intr_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_access_define_tslam_dma_enable].name = "tslam_dma_enable";
    submodule_data->defines[dnxf_data_device_access_define_tslam_dma_enable].doc = "";
    submodule_data->defines[dnxf_data_device_access_define_tslam_dma_enable].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_device_access_define_tslam_dma_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_access_define_tslam_timeout_usec].name = "tslam_timeout_usec";
    submodule_data->defines[dnxf_data_device_access_define_tslam_timeout_usec].doc = "";
    submodule_data->defines[dnxf_data_device_access_define_tslam_timeout_usec].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_device_access_define_tslam_timeout_usec].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_access_define_tslam_int_enable].name = "tslam_int_enable";
    submodule_data->defines[dnxf_data_device_access_define_tslam_int_enable].doc = "";
    submodule_data->defines[dnxf_data_device_access_define_tslam_int_enable].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_device_access_define_tslam_int_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_access_define_mdio_int_divisor].name = "mdio_int_divisor";
    submodule_data->defines[dnxf_data_device_access_define_mdio_int_divisor].doc = "";
    
    submodule_data->defines[dnxf_data_device_access_define_mdio_int_divisor].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_access_define_mdio_ext_divisor].name = "mdio_ext_divisor";
    submodule_data->defines[dnxf_data_device_access_define_mdio_ext_divisor].doc = "";
    
    submodule_data->defines[dnxf_data_device_access_define_mdio_ext_divisor].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_access_define_mdio_int_div_out_delay].name = "mdio_int_div_out_delay";
    submodule_data->defines[dnxf_data_device_access_define_mdio_int_div_out_delay].doc = "";
    
    submodule_data->defines[dnxf_data_device_access_define_mdio_int_div_out_delay].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_access_define_mdio_ext_div_out_delay].name = "mdio_ext_div_out_delay";
    submodule_data->defines[dnxf_data_device_access_define_mdio_ext_div_out_delay].doc = "";
    
    submodule_data->defines[dnxf_data_device_access_define_mdio_ext_div_out_delay].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_access_define_bist_enable].name = "bist_enable";
    submodule_data->defines[dnxf_data_device_access_define_bist_enable].doc = "";
    submodule_data->defines[dnxf_data_device_access_define_bist_enable].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_device_access_define_bist_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_access_define_sbus_dma_interval].name = "sbus_dma_interval";
    submodule_data->defines[dnxf_data_device_access_define_sbus_dma_interval].doc = "";
    submodule_data->defines[dnxf_data_device_access_define_sbus_dma_interval].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_device_access_define_sbus_dma_interval].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_access_define_sbus_dma_intr_enable].name = "sbus_dma_intr_enable";
    submodule_data->defines[dnxf_data_device_access_define_sbus_dma_intr_enable].doc = "";
    submodule_data->defines[dnxf_data_device_access_define_sbus_dma_intr_enable].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_device_access_define_sbus_dma_intr_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_access_define_mem_clear_chunk_size].name = "mem_clear_chunk_size";
    submodule_data->defines[dnxf_data_device_access_define_mem_clear_chunk_size].doc = "";
    submodule_data->defines[dnxf_data_device_access_define_mem_clear_chunk_size].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_device_access_define_mem_clear_chunk_size].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnxf_data_device_access_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data device access tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_device_access_feature_get(
    int unit,
    dnxf_data_device_access_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, feature);
}


uint32
dnxf_data_device_access_table_dma_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, dnxf_data_device_access_define_table_dma_enable);
}

uint32
dnxf_data_device_access_tdma_timeout_usec_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, dnxf_data_device_access_define_tdma_timeout_usec);
}

uint32
dnxf_data_device_access_tdma_intr_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, dnxf_data_device_access_define_tdma_intr_enable);
}

uint32
dnxf_data_device_access_tslam_dma_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, dnxf_data_device_access_define_tslam_dma_enable);
}

uint32
dnxf_data_device_access_tslam_timeout_usec_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, dnxf_data_device_access_define_tslam_timeout_usec);
}

uint32
dnxf_data_device_access_tslam_int_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, dnxf_data_device_access_define_tslam_int_enable);
}

uint32
dnxf_data_device_access_mdio_int_divisor_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, dnxf_data_device_access_define_mdio_int_divisor);
}

uint32
dnxf_data_device_access_mdio_ext_divisor_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, dnxf_data_device_access_define_mdio_ext_divisor);
}

uint32
dnxf_data_device_access_mdio_int_div_out_delay_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, dnxf_data_device_access_define_mdio_int_div_out_delay);
}

uint32
dnxf_data_device_access_mdio_ext_div_out_delay_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, dnxf_data_device_access_define_mdio_ext_div_out_delay);
}

uint32
dnxf_data_device_access_bist_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, dnxf_data_device_access_define_bist_enable);
}

uint32
dnxf_data_device_access_sbus_dma_interval_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, dnxf_data_device_access_define_sbus_dma_interval);
}

uint32
dnxf_data_device_access_sbus_dma_intr_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, dnxf_data_device_access_define_sbus_dma_intr_enable);
}

uint32
dnxf_data_device_access_mem_clear_chunk_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, dnxf_data_device_access_define_mem_clear_chunk_size);
}










static shr_error_e
dnxf_data_device_blocks_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "blocks";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnxf_data_device_blocks_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data device blocks features");

    submodule_data->features[dnxf_data_device_blocks_dch_reset_restore_support].name = "dch_reset_restore_support";
    submodule_data->features[dnxf_data_device_blocks_dch_reset_restore_support].doc = "";
    submodule_data->features[dnxf_data_device_blocks_dch_reset_restore_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnxf_data_device_blocks_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data device blocks defines");

    submodule_data->defines[dnxf_data_device_blocks_define_nof_all_blocks].name = "nof_all_blocks";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_all_blocks].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_all_blocks].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_fmac].name = "nof_instances_fmac";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_fmac].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_fmac].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_fmac].name = "nof_links_in_fmac";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_fmac].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_fmac].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_fsrd].name = "nof_instances_fsrd";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_fsrd].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_fsrd].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_fsrd].name = "nof_instances_brdc_fsrd";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_fsrd].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_fsrd].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_dch].name = "nof_instances_dch";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_dch].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_dch].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_dch].name = "nof_links_in_dch";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_dch].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_dch].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_dch_link_groups].name = "nof_dch_link_groups";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_dch_link_groups].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_dch_link_groups].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_cch].name = "nof_instances_cch";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_cch].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_cch].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_cch].name = "nof_links_in_cch";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_cch].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_cch].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_dcq].name = "nof_links_in_dcq";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_dcq].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_dcq].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_rtp].name = "nof_instances_rtp";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_rtp].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_rtp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_rtp].name = "nof_links_in_rtp";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_rtp].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_rtp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_occg].name = "nof_instances_occg";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_occg].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_occg].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_eci].name = "nof_instances_eci";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_eci].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_eci].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_cmic].name = "nof_instances_cmic";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_cmic].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_cmic].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_mesh_topology].name = "nof_instances_mesh_topology";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_mesh_topology].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_mesh_topology].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_fmach].name = "nof_instances_brdc_fmach";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_fmach].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_fmach].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_fmacl].name = "nof_instances_brdc_fmacl";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_fmacl].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_fmacl].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_lcm].name = "nof_instances_lcm";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_lcm].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_lcm].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_mct].name = "nof_instances_mct";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_mct].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_mct].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_qrh].name = "nof_instances_qrh";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_qrh].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_qrh].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_dcml].name = "nof_instances_dcml";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_dcml].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_dcml].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_dcml].name = "nof_links_in_dcml";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_dcml].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_dcml].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_lcm].name = "nof_links_in_lcm";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_lcm].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_lcm].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_qrh].name = "nof_links_in_qrh";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_qrh].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_qrh].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_dtm_fifos].name = "nof_dtm_fifos";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_dtm_fifos].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_dtm_fifos].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_fsrd].name = "nof_links_in_fsrd";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_fsrd].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_fsrd].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_phy_core].name = "nof_links_in_phy_core";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_phy_core].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_phy_core].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_fmacs_in_fsrd].name = "nof_fmacs_in_fsrd";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_fmacs_in_fsrd].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_fmacs_in_fsrd].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_pcu].name = "nof_instances_pcu";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_pcu].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_pcu].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_ccml].name = "nof_instances_ccml";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_ccml].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_ccml].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_dfl].name = "nof_instances_dfl";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_dfl].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_dfl].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_dtm].name = "nof_instances_dtm";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_dtm].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_dtm].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_pads].name = "nof_instances_pads";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_pads].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_pads].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_dfl_fbc].name = "nof_instances_dfl_fbc";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_dfl_fbc].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_dfl_fbc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_dtl].name = "nof_instances_dtl";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_dtl].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_dtl].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_mesh_topology_wrap].name = "nof_instances_mesh_topology_wrap";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_mesh_topology_wrap].doc = "";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_mesh_topology_wrap].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnxf_data_device_blocks_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data device blocks tables");

    
    submodule_data->tables[dnxf_data_device_blocks_table_override].name = "override";
    submodule_data->tables[dnxf_data_device_blocks_table_override].doc = "";
    submodule_data->tables[dnxf_data_device_blocks_table_override].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_device_blocks_table_override].size_of_values = sizeof(dnxf_data_device_blocks_override_t);
    submodule_data->tables[dnxf_data_device_blocks_table_override].entry_get = dnxf_data_device_blocks_override_entry_str_get;

    
    submodule_data->tables[dnxf_data_device_blocks_table_override].nof_keys = 1;
    submodule_data->tables[dnxf_data_device_blocks_table_override].keys[0].name = "index";
    submodule_data->tables[dnxf_data_device_blocks_table_override].keys[0].doc = "";

    
    submodule_data->tables[dnxf_data_device_blocks_table_override].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_device_blocks_table_override].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_device_blocks_table_override].nof_values, "_dnxf_data_device_blocks_table_override table values");
    submodule_data->tables[dnxf_data_device_blocks_table_override].values[0].name = "block_type";
    submodule_data->tables[dnxf_data_device_blocks_table_override].values[0].type = "char *";
    submodule_data->tables[dnxf_data_device_blocks_table_override].values[0].doc = "See soc_block_type_t";
    submodule_data->tables[dnxf_data_device_blocks_table_override].values[0].offset = UTILEX_OFFSETOF(dnxf_data_device_blocks_override_t, block_type);
    submodule_data->tables[dnxf_data_device_blocks_table_override].values[1].name = "block_instance";
    submodule_data->tables[dnxf_data_device_blocks_table_override].values[1].type = "int";
    submodule_data->tables[dnxf_data_device_blocks_table_override].values[1].doc = "instance ID within block type";
    submodule_data->tables[dnxf_data_device_blocks_table_override].values[1].offset = UTILEX_OFFSETOF(dnxf_data_device_blocks_override_t, block_instance);
    submodule_data->tables[dnxf_data_device_blocks_table_override].values[2].name = "value";
    submodule_data->tables[dnxf_data_device_blocks_table_override].values[2].type = "int";
    submodule_data->tables[dnxf_data_device_blocks_table_override].values[2].doc = "The value to override";
    submodule_data->tables[dnxf_data_device_blocks_table_override].values[2].offset = UTILEX_OFFSETOF(dnxf_data_device_blocks_override_t, value);

    
    submodule_data->tables[dnxf_data_device_blocks_table_fsrd_sbus_chain].name = "fsrd_sbus_chain";
    submodule_data->tables[dnxf_data_device_blocks_table_fsrd_sbus_chain].doc = "";
    submodule_data->tables[dnxf_data_device_blocks_table_fsrd_sbus_chain].labels[0] = "data_notrev";
    submodule_data->tables[dnxf_data_device_blocks_table_fsrd_sbus_chain].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_device_blocks_table_fsrd_sbus_chain].size_of_values = sizeof(dnxf_data_device_blocks_fsrd_sbus_chain_t);
    submodule_data->tables[dnxf_data_device_blocks_table_fsrd_sbus_chain].entry_get = dnxf_data_device_blocks_fsrd_sbus_chain_entry_str_get;

    
    submodule_data->tables[dnxf_data_device_blocks_table_fsrd_sbus_chain].nof_keys = 1;
    submodule_data->tables[dnxf_data_device_blocks_table_fsrd_sbus_chain].keys[0].name = "fsrd_id";
    submodule_data->tables[dnxf_data_device_blocks_table_fsrd_sbus_chain].keys[0].doc = "";

    
    submodule_data->tables[dnxf_data_device_blocks_table_fsrd_sbus_chain].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_device_blocks_table_fsrd_sbus_chain].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_device_blocks_table_fsrd_sbus_chain].nof_values, "_dnxf_data_device_blocks_table_fsrd_sbus_chain table values");
    submodule_data->tables[dnxf_data_device_blocks_table_fsrd_sbus_chain].values[0].name = "index";
    submodule_data->tables[dnxf_data_device_blocks_table_fsrd_sbus_chain].values[0].type = "int";
    submodule_data->tables[dnxf_data_device_blocks_table_fsrd_sbus_chain].values[0].doc = "The index of the FSRD in the SBUS chain";
    submodule_data->tables[dnxf_data_device_blocks_table_fsrd_sbus_chain].values[0].offset = UTILEX_OFFSETOF(dnxf_data_device_blocks_fsrd_sbus_chain_t, index);

    
    submodule_data->tables[dnxf_data_device_blocks_table_fmac_sbus_chain].name = "fmac_sbus_chain";
    submodule_data->tables[dnxf_data_device_blocks_table_fmac_sbus_chain].doc = "";
    submodule_data->tables[dnxf_data_device_blocks_table_fmac_sbus_chain].labels[0] = "data_notrev";
    submodule_data->tables[dnxf_data_device_blocks_table_fmac_sbus_chain].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_device_blocks_table_fmac_sbus_chain].size_of_values = sizeof(dnxf_data_device_blocks_fmac_sbus_chain_t);
    submodule_data->tables[dnxf_data_device_blocks_table_fmac_sbus_chain].entry_get = dnxf_data_device_blocks_fmac_sbus_chain_entry_str_get;

    
    submodule_data->tables[dnxf_data_device_blocks_table_fmac_sbus_chain].nof_keys = 1;
    submodule_data->tables[dnxf_data_device_blocks_table_fmac_sbus_chain].keys[0].name = "fmac_id";
    submodule_data->tables[dnxf_data_device_blocks_table_fmac_sbus_chain].keys[0].doc = "";

    
    submodule_data->tables[dnxf_data_device_blocks_table_fmac_sbus_chain].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_device_blocks_table_fmac_sbus_chain].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_device_blocks_table_fmac_sbus_chain].nof_values, "_dnxf_data_device_blocks_table_fmac_sbus_chain table values");
    submodule_data->tables[dnxf_data_device_blocks_table_fmac_sbus_chain].values[0].name = "index";
    submodule_data->tables[dnxf_data_device_blocks_table_fmac_sbus_chain].values[0].type = "int";
    submodule_data->tables[dnxf_data_device_blocks_table_fmac_sbus_chain].values[0].doc = "The index of the FMAC in the SBUS chain";
    submodule_data->tables[dnxf_data_device_blocks_table_fmac_sbus_chain].values[0].offset = UTILEX_OFFSETOF(dnxf_data_device_blocks_fmac_sbus_chain_t, index);

    
    submodule_data->tables[dnxf_data_device_blocks_table_fmac_lane_to_block_map].name = "fmac_lane_to_block_map";
    submodule_data->tables[dnxf_data_device_blocks_table_fmac_lane_to_block_map].doc = "";
    submodule_data->tables[dnxf_data_device_blocks_table_fmac_lane_to_block_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_device_blocks_table_fmac_lane_to_block_map].size_of_values = sizeof(dnxf_data_device_blocks_fmac_lane_to_block_map_t);
    submodule_data->tables[dnxf_data_device_blocks_table_fmac_lane_to_block_map].entry_get = dnxf_data_device_blocks_fmac_lane_to_block_map_entry_str_get;

    
    submodule_data->tables[dnxf_data_device_blocks_table_fmac_lane_to_block_map].nof_keys = 1;
    submodule_data->tables[dnxf_data_device_blocks_table_fmac_lane_to_block_map].keys[0].name = "fmac_lane";
    submodule_data->tables[dnxf_data_device_blocks_table_fmac_lane_to_block_map].keys[0].doc = "";

    
    submodule_data->tables[dnxf_data_device_blocks_table_fmac_lane_to_block_map].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_device_blocks_table_fmac_lane_to_block_map].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_device_blocks_table_fmac_lane_to_block_map].nof_values, "_dnxf_data_device_blocks_table_fmac_lane_to_block_map table values");
    submodule_data->tables[dnxf_data_device_blocks_table_fmac_lane_to_block_map].values[0].name = "rtp_link";
    submodule_data->tables[dnxf_data_device_blocks_table_fmac_lane_to_block_map].values[0].type = "int";
    submodule_data->tables[dnxf_data_device_blocks_table_fmac_lane_to_block_map].values[0].doc = "rtp_link. holds rtp_link of all cores, 0-nof_links";
    submodule_data->tables[dnxf_data_device_blocks_table_fmac_lane_to_block_map].values[0].offset = UTILEX_OFFSETOF(dnxf_data_device_blocks_fmac_lane_to_block_map_t, rtp_link);

    
    submodule_data->tables[dnxf_data_device_blocks_table_dch_instances_to_device_stage].name = "dch_instances_to_device_stage";
    submodule_data->tables[dnxf_data_device_blocks_table_dch_instances_to_device_stage].doc = "";
    submodule_data->tables[dnxf_data_device_blocks_table_dch_instances_to_device_stage].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_device_blocks_table_dch_instances_to_device_stage].size_of_values = sizeof(dnxf_data_device_blocks_dch_instances_to_device_stage_t);
    submodule_data->tables[dnxf_data_device_blocks_table_dch_instances_to_device_stage].entry_get = dnxf_data_device_blocks_dch_instances_to_device_stage_entry_str_get;

    
    submodule_data->tables[dnxf_data_device_blocks_table_dch_instances_to_device_stage].nof_keys = 1;
    submodule_data->tables[dnxf_data_device_blocks_table_dch_instances_to_device_stage].keys[0].name = "device_stage";
    submodule_data->tables[dnxf_data_device_blocks_table_dch_instances_to_device_stage].keys[0].doc = "";

    
    submodule_data->tables[dnxf_data_device_blocks_table_dch_instances_to_device_stage].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_device_blocks_table_dch_instances_to_device_stage].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_device_blocks_table_dch_instances_to_device_stage].nof_values, "_dnxf_data_device_blocks_table_dch_instances_to_device_stage table values");
    submodule_data->tables[dnxf_data_device_blocks_table_dch_instances_to_device_stage].values[0].name = "dch_instance_bmp";
    submodule_data->tables[dnxf_data_device_blocks_table_dch_instances_to_device_stage].values[0].type = "soc_pbmp_t";
    submodule_data->tables[dnxf_data_device_blocks_table_dch_instances_to_device_stage].values[0].doc = "a bitmap of the CDR instanceds facing specific device type";
    submodule_data->tables[dnxf_data_device_blocks_table_dch_instances_to_device_stage].values[0].offset = UTILEX_OFFSETOF(dnxf_data_device_blocks_dch_instances_to_device_stage_t, dch_instance_bmp);

    
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_rtp].name = "power_down_per_rtp";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_rtp].doc = "";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_rtp].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_rtp].size_of_values = sizeof(dnxf_data_device_blocks_power_down_per_rtp_t);
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_rtp].entry_get = dnxf_data_device_blocks_power_down_per_rtp_entry_str_get;

    
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_rtp].nof_keys = 1;
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_rtp].keys[0].name = "index";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_rtp].keys[0].doc = "";

    
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_rtp].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_rtp].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_rtp].nof_values, "_dnxf_data_device_blocks_table_power_down_per_rtp table values");
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_rtp].values[0].name = "block_type";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_rtp].values[0].type = "int";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_rtp].values[0].doc = "One of block type names in soc_block_type_e";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_rtp].values[0].offset = UTILEX_OFFSETOF(dnxf_data_device_blocks_power_down_per_rtp_t, block_type);
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_rtp].values[1].name = "clock_reg";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_rtp].values[1].type = "int";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_rtp].values[1].doc = "Reg per block of type block_SPECIAL_CLOCK_CONTROLSr to stop the clock to the block";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_rtp].values[1].offset = UTILEX_OFFSETOF(dnxf_data_device_blocks_power_down_per_rtp_t, clock_reg);
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_rtp].values[2].name = "soft_init_fields";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_rtp].values[2].type = "uint32[DNXF_DATA_MAX_DEVICE_BLOCKS_NOF_INSTANCES_RTP]";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_rtp].values[2].doc = "Field inside ECI_BLOCKS_SOFT_INIT to put the block in reset";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_rtp].values[2].offset = UTILEX_OFFSETOF(dnxf_data_device_blocks_power_down_per_rtp_t, soft_init_fields);

    
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dch].name = "power_down_per_dch";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dch].doc = "";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dch].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dch].size_of_values = sizeof(dnxf_data_device_blocks_power_down_per_dch_t);
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dch].entry_get = dnxf_data_device_blocks_power_down_per_dch_entry_str_get;

    
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dch].nof_keys = 1;
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dch].keys[0].name = "index";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dch].keys[0].doc = "";

    
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dch].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dch].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dch].nof_values, "_dnxf_data_device_blocks_table_power_down_per_dch table values");
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dch].values[0].name = "block_type";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dch].values[0].type = "int";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dch].values[0].doc = "One of block type names in soc_block_type_e";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dch].values[0].offset = UTILEX_OFFSETOF(dnxf_data_device_blocks_power_down_per_dch_t, block_type);
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dch].values[1].name = "clock_reg";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dch].values[1].type = "int";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dch].values[1].doc = "Reg per block of type block_SPECIAL_CLOCK_CONTROLSr to stop the clock to the block";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dch].values[1].offset = UTILEX_OFFSETOF(dnxf_data_device_blocks_power_down_per_dch_t, clock_reg);
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dch].values[2].name = "soft_init_fields";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dch].values[2].type = "uint32[DNXF_DATA_MAX_DEVICE_BLOCKS_NOF_INSTANCES_DCH]";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dch].values[2].doc = "Field inside ECI_BLOCKS_SOFT_INIT to put the block in reset";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dch].values[2].offset = UTILEX_OFFSETOF(dnxf_data_device_blocks_power_down_per_dch_t, soft_init_fields);

    
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_fmac].name = "power_down_per_fmac";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_fmac].doc = "";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_fmac].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_fmac].size_of_values = sizeof(dnxf_data_device_blocks_power_down_per_fmac_t);
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_fmac].entry_get = dnxf_data_device_blocks_power_down_per_fmac_entry_str_get;

    
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_fmac].nof_keys = 1;
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_fmac].keys[0].name = "index";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_fmac].keys[0].doc = "";

    
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_fmac].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_fmac].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_fmac].nof_values, "_dnxf_data_device_blocks_table_power_down_per_fmac table values");
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_fmac].values[0].name = "block_type";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_fmac].values[0].type = "int";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_fmac].values[0].doc = "One of block type names in soc_block_type_e";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_fmac].values[0].offset = UTILEX_OFFSETOF(dnxf_data_device_blocks_power_down_per_fmac_t, block_type);
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_fmac].values[1].name = "clock_reg";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_fmac].values[1].type = "int";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_fmac].values[1].doc = "Reg per block of type block_SPECIAL_CLOCK_CONTROLSr to stop the clock to the block";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_fmac].values[1].offset = UTILEX_OFFSETOF(dnxf_data_device_blocks_power_down_per_fmac_t, clock_reg);
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_fmac].values[2].name = "soft_init_fields";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_fmac].values[2].type = "uint32[DNXF_DATA_MAX_DEVICE_BLOCKS_NOF_INSTANCES_FMAC]";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_fmac].values[2].doc = "Field inside ECI_BLOCKS_SOFT_INIT to put the block in reset";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_fmac].values[2].offset = UTILEX_OFFSETOF(dnxf_data_device_blocks_power_down_per_fmac_t, soft_init_fields);

    
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dfl_fbc].name = "power_down_per_dfl_fbc";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dfl_fbc].doc = "";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dfl_fbc].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dfl_fbc].size_of_values = sizeof(dnxf_data_device_blocks_power_down_per_dfl_fbc_t);
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dfl_fbc].entry_get = dnxf_data_device_blocks_power_down_per_dfl_fbc_entry_str_get;

    
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dfl_fbc].nof_keys = 1;
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dfl_fbc].keys[0].name = "index";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dfl_fbc].keys[0].doc = "";

    
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dfl_fbc].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dfl_fbc].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dfl_fbc].nof_values, "_dnxf_data_device_blocks_table_power_down_per_dfl_fbc table values");
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dfl_fbc].values[0].name = "block_type";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dfl_fbc].values[0].type = "int";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dfl_fbc].values[0].doc = "One of block type names in soc_block_type_e";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dfl_fbc].values[0].offset = UTILEX_OFFSETOF(dnxf_data_device_blocks_power_down_per_dfl_fbc_t, block_type);
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dfl_fbc].values[1].name = "clock_reg";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dfl_fbc].values[1].type = "int";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dfl_fbc].values[1].doc = "Reg per block of type block_SPECIAL_CLOCK_CONTROLSr to stop the clock to the block";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dfl_fbc].values[1].offset = UTILEX_OFFSETOF(dnxf_data_device_blocks_power_down_per_dfl_fbc_t, clock_reg);
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dfl_fbc].values[2].name = "soft_init_fields";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dfl_fbc].values[2].type = "uint32[DNXF_DATA_MAX_DEVICE_BLOCKS_NOF_LINKS_IN_DCH]";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dfl_fbc].values[2].doc = "Field inside ECI_BLOCKS_SOFT_INIT to put the block in reset";
    submodule_data->tables[dnxf_data_device_blocks_table_power_down_per_dfl_fbc].values[2].offset = UTILEX_OFFSETOF(dnxf_data_device_blocks_power_down_per_dfl_fbc_t, soft_init_fields);


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_device_blocks_feature_get(
    int unit,
    dnxf_data_device_blocks_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, feature);
}


uint32
dnxf_data_device_blocks_nof_all_blocks_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_all_blocks);
}

uint32
dnxf_data_device_blocks_nof_instances_fmac_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_fmac);
}

uint32
dnxf_data_device_blocks_nof_links_in_fmac_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_links_in_fmac);
}

uint32
dnxf_data_device_blocks_nof_instances_fsrd_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_fsrd);
}

uint32
dnxf_data_device_blocks_nof_instances_brdc_fsrd_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_brdc_fsrd);
}

uint32
dnxf_data_device_blocks_nof_instances_dch_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_dch);
}

uint32
dnxf_data_device_blocks_nof_links_in_dch_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_links_in_dch);
}

uint32
dnxf_data_device_blocks_nof_dch_link_groups_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_dch_link_groups);
}

uint32
dnxf_data_device_blocks_nof_instances_cch_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_cch);
}

uint32
dnxf_data_device_blocks_nof_links_in_cch_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_links_in_cch);
}

uint32
dnxf_data_device_blocks_nof_links_in_dcq_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_links_in_dcq);
}

uint32
dnxf_data_device_blocks_nof_instances_rtp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_rtp);
}

uint32
dnxf_data_device_blocks_nof_links_in_rtp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_links_in_rtp);
}

uint32
dnxf_data_device_blocks_nof_instances_occg_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_occg);
}

uint32
dnxf_data_device_blocks_nof_instances_eci_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_eci);
}

uint32
dnxf_data_device_blocks_nof_instances_cmic_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_cmic);
}

uint32
dnxf_data_device_blocks_nof_instances_mesh_topology_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_mesh_topology);
}

uint32
dnxf_data_device_blocks_nof_instances_brdc_fmach_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_brdc_fmach);
}

uint32
dnxf_data_device_blocks_nof_instances_brdc_fmacl_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_brdc_fmacl);
}

uint32
dnxf_data_device_blocks_nof_instances_lcm_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_lcm);
}

uint32
dnxf_data_device_blocks_nof_instances_mct_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_mct);
}

uint32
dnxf_data_device_blocks_nof_instances_qrh_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_qrh);
}

uint32
dnxf_data_device_blocks_nof_instances_dcml_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_dcml);
}

uint32
dnxf_data_device_blocks_nof_links_in_dcml_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_links_in_dcml);
}

uint32
dnxf_data_device_blocks_nof_links_in_lcm_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_links_in_lcm);
}

uint32
dnxf_data_device_blocks_nof_links_in_qrh_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_links_in_qrh);
}

uint32
dnxf_data_device_blocks_nof_dtm_fifos_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_dtm_fifos);
}

uint32
dnxf_data_device_blocks_nof_links_in_fsrd_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_links_in_fsrd);
}

uint32
dnxf_data_device_blocks_nof_links_in_phy_core_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_links_in_phy_core);
}

uint32
dnxf_data_device_blocks_nof_fmacs_in_fsrd_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_fmacs_in_fsrd);
}

uint32
dnxf_data_device_blocks_nof_instances_pcu_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_pcu);
}

uint32
dnxf_data_device_blocks_nof_instances_ccml_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_ccml);
}

uint32
dnxf_data_device_blocks_nof_instances_dfl_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_dfl);
}

uint32
dnxf_data_device_blocks_nof_instances_dtm_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_dtm);
}

uint32
dnxf_data_device_blocks_nof_instances_pads_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_pads);
}

uint32
dnxf_data_device_blocks_nof_instances_dfl_fbc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_dfl_fbc);
}

uint32
dnxf_data_device_blocks_nof_instances_dtl_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_dtl);
}

uint32
dnxf_data_device_blocks_nof_instances_mesh_topology_wrap_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_mesh_topology_wrap);
}



const dnxf_data_device_blocks_override_t *
dnxf_data_device_blocks_override_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_override);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnxf_data_device_blocks_override_t *) data;

}

const dnxf_data_device_blocks_fsrd_sbus_chain_t *
dnxf_data_device_blocks_fsrd_sbus_chain_get(
    int unit,
    int fsrd_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_fsrd_sbus_chain);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, fsrd_id, 0);
    return (const dnxf_data_device_blocks_fsrd_sbus_chain_t *) data;

}

const dnxf_data_device_blocks_fmac_sbus_chain_t *
dnxf_data_device_blocks_fmac_sbus_chain_get(
    int unit,
    int fmac_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_fmac_sbus_chain);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, fmac_id, 0);
    return (const dnxf_data_device_blocks_fmac_sbus_chain_t *) data;

}

const dnxf_data_device_blocks_fmac_lane_to_block_map_t *
dnxf_data_device_blocks_fmac_lane_to_block_map_get(
    int unit,
    int fmac_lane)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_fmac_lane_to_block_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, fmac_lane, 0);
    return (const dnxf_data_device_blocks_fmac_lane_to_block_map_t *) data;

}

const dnxf_data_device_blocks_dch_instances_to_device_stage_t *
dnxf_data_device_blocks_dch_instances_to_device_stage_get(
    int unit,
    int device_stage)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_dch_instances_to_device_stage);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, device_stage, 0);
    return (const dnxf_data_device_blocks_dch_instances_to_device_stage_t *) data;

}

const dnxf_data_device_blocks_power_down_per_rtp_t *
dnxf_data_device_blocks_power_down_per_rtp_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_power_down_per_rtp);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnxf_data_device_blocks_power_down_per_rtp_t *) data;

}

const dnxf_data_device_blocks_power_down_per_dch_t *
dnxf_data_device_blocks_power_down_per_dch_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_power_down_per_dch);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnxf_data_device_blocks_power_down_per_dch_t *) data;

}

const dnxf_data_device_blocks_power_down_per_fmac_t *
dnxf_data_device_blocks_power_down_per_fmac_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_power_down_per_fmac);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnxf_data_device_blocks_power_down_per_fmac_t *) data;

}

const dnxf_data_device_blocks_power_down_per_dfl_fbc_t *
dnxf_data_device_blocks_power_down_per_dfl_fbc_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_power_down_per_dfl_fbc);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnxf_data_device_blocks_power_down_per_dfl_fbc_t *) data;

}


shr_error_e
dnxf_data_device_blocks_override_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_device_blocks_override_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_override);
    data = (const dnxf_data_device_blocks_override_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->block_type == NULL ? "" : data->block_type);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->block_instance);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->value);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnxf_data_device_blocks_fsrd_sbus_chain_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_device_blocks_fsrd_sbus_chain_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_fsrd_sbus_chain);
    data = (const dnxf_data_device_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->index);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnxf_data_device_blocks_fmac_sbus_chain_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_device_blocks_fmac_sbus_chain_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_fmac_sbus_chain);
    data = (const dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->index);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnxf_data_device_blocks_fmac_lane_to_block_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_device_blocks_fmac_lane_to_block_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_fmac_lane_to_block_map);
    data = (const dnxf_data_device_blocks_fmac_lane_to_block_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->rtp_link);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnxf_data_device_blocks_dch_instances_to_device_stage_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_device_blocks_dch_instances_to_device_stage_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_dch_instances_to_device_stage);
    data = (const dnxf_data_device_blocks_dch_instances_to_device_stage_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_PBMP_STR(buffer, data->dch_instance_bmp);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnxf_data_device_blocks_power_down_per_rtp_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_device_blocks_power_down_per_rtp_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_power_down_per_rtp);
    data = (const dnxf_data_device_blocks_power_down_per_rtp_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->block_type);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->clock_reg);
            break;
        case 2:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNXF_DATA_MAX_DEVICE_BLOCKS_NOF_INSTANCES_RTP, data->soft_init_fields);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnxf_data_device_blocks_power_down_per_dch_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_device_blocks_power_down_per_dch_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_power_down_per_dch);
    data = (const dnxf_data_device_blocks_power_down_per_dch_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->block_type);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->clock_reg);
            break;
        case 2:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNXF_DATA_MAX_DEVICE_BLOCKS_NOF_INSTANCES_DCH, data->soft_init_fields);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnxf_data_device_blocks_power_down_per_fmac_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_device_blocks_power_down_per_fmac_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_power_down_per_fmac);
    data = (const dnxf_data_device_blocks_power_down_per_fmac_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->block_type);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->clock_reg);
            break;
        case 2:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNXF_DATA_MAX_DEVICE_BLOCKS_NOF_INSTANCES_FMAC, data->soft_init_fields);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnxf_data_device_blocks_power_down_per_dfl_fbc_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_device_blocks_power_down_per_dfl_fbc_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_power_down_per_dfl_fbc);
    data = (const dnxf_data_device_blocks_power_down_per_dfl_fbc_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->block_type);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->clock_reg);
            break;
        case 2:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNXF_DATA_MAX_DEVICE_BLOCKS_NOF_LINKS_IN_DCH, data->soft_init_fields);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnxf_data_device_blocks_override_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_override);

}

const dnxc_data_table_info_t *
dnxf_data_device_blocks_fsrd_sbus_chain_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_fsrd_sbus_chain);

}

const dnxc_data_table_info_t *
dnxf_data_device_blocks_fmac_sbus_chain_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_fmac_sbus_chain);

}

const dnxc_data_table_info_t *
dnxf_data_device_blocks_fmac_lane_to_block_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_fmac_lane_to_block_map);

}

const dnxc_data_table_info_t *
dnxf_data_device_blocks_dch_instances_to_device_stage_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_dch_instances_to_device_stage);

}

const dnxc_data_table_info_t *
dnxf_data_device_blocks_power_down_per_rtp_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_power_down_per_rtp);

}

const dnxc_data_table_info_t *
dnxf_data_device_blocks_power_down_per_dch_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_power_down_per_dch);

}

const dnxc_data_table_info_t *
dnxf_data_device_blocks_power_down_per_fmac_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_power_down_per_fmac);

}

const dnxc_data_table_info_t *
dnxf_data_device_blocks_power_down_per_dfl_fbc_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_power_down_per_dfl_fbc);

}






static shr_error_e
dnxf_data_device_interrupts_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "interrupts";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnxf_data_device_interrupts_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data device interrupts features");

    
    submodule_data->nof_defines = _dnxf_data_device_interrupts_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data device interrupts defines");

    submodule_data->defines[dnxf_data_device_interrupts_define_nof_interrupts].name = "nof_interrupts";
    submodule_data->defines[dnxf_data_device_interrupts_define_nof_interrupts].doc = "";
    
    submodule_data->defines[dnxf_data_device_interrupts_define_nof_interrupts].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnxf_data_device_interrupts_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data device interrupts tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_device_interrupts_feature_get(
    int unit,
    dnxf_data_device_interrupts_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_interrupts, feature);
}


uint32
dnxf_data_device_interrupts_nof_interrupts_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_interrupts, dnxf_data_device_interrupts_define_nof_interrupts);
}










static shr_error_e
dnxf_data_device_custom_features_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "custom_features";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnxf_data_device_custom_features_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data device custom_features features");

    
    submodule_data->nof_defines = _dnxf_data_device_custom_features_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data device custom_features defines");

    submodule_data->defines[dnxf_data_device_custom_features_define_mesh_topology_size].name = "mesh_topology_size";
    submodule_data->defines[dnxf_data_device_custom_features_define_mesh_topology_size].doc = "";
    submodule_data->defines[dnxf_data_device_custom_features_define_mesh_topology_size].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_device_custom_features_define_mesh_topology_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_custom_features_define_mesh_topology_fast].name = "mesh_topology_fast";
    submodule_data->defines[dnxf_data_device_custom_features_define_mesh_topology_fast].doc = "";
    submodule_data->defines[dnxf_data_device_custom_features_define_mesh_topology_fast].labels[0] = "data_notrev";
    
    submodule_data->defines[dnxf_data_device_custom_features_define_mesh_topology_fast].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_custom_features_define_active_core].name = "active_core";
    submodule_data->defines[dnxf_data_device_custom_features_define_active_core].doc = "";
    
    submodule_data->defines[dnxf_data_device_custom_features_define_active_core].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnxf_data_device_custom_features_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data device custom_features tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_device_custom_features_feature_get(
    int unit,
    dnxf_data_device_custom_features_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_custom_features, feature);
}


int
dnxf_data_device_custom_features_mesh_topology_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_custom_features, dnxf_data_device_custom_features_define_mesh_topology_size);
}

uint32
dnxf_data_device_custom_features_mesh_topology_fast_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_custom_features, dnxf_data_device_custom_features_define_mesh_topology_fast);
}

uint32
dnxf_data_device_custom_features_active_core_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_custom_features, dnxf_data_device_custom_features_define_active_core);
}










static shr_error_e
dnxf_data_device_properties_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "properties";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnxf_data_device_properties_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data device properties features");

    
    submodule_data->nof_defines = _dnxf_data_device_properties_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data device properties defines");

    
    submodule_data->nof_tables = _dnxf_data_device_properties_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data device properties tables");

    
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].name = "unsupported";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].doc = "";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].size_of_values = sizeof(dnxf_data_device_properties_unsupported_t);
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].entry_get = dnxf_data_device_properties_unsupported_entry_str_get;

    
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].nof_keys = 1;
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].keys[0].name = "index";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].keys[0].doc = "";

    
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].nof_values = 5;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_device_properties_table_unsupported].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_device_properties_table_unsupported].nof_values, "_dnxf_data_device_properties_table_unsupported table values");
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[0].name = "property";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[0].type = "char *";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[0].doc = "soc property name";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[0].offset = UTILEX_OFFSETOF(dnxf_data_device_properties_unsupported_t, property);
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[1].name = "suffix";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[1].type = "char *";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[1].doc = "used in case the soc property read using suffix method";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[1].offset = UTILEX_OFFSETOF(dnxf_data_device_properties_unsupported_t, suffix);
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[2].name = "num_max";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[2].type = "int";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[2].doc = "max number of index to check in case using suffix_num method";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[2].offset = UTILEX_OFFSETOF(dnxf_data_device_properties_unsupported_t, num_max);
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[3].name = "per_port";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[3].type = "int";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[3].doc = "1 if the soc property might be read per port";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[3].offset = UTILEX_OFFSETOF(dnxf_data_device_properties_unsupported_t, per_port);
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[4].name = "err_msg";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[4].type = "char *";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[4].doc = "detailed error message with instructions what to do instead";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[4].offset = UTILEX_OFFSETOF(dnxf_data_device_properties_unsupported_t, err_msg);


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_device_properties_feature_get(
    int unit,
    dnxf_data_device_properties_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_properties, feature);
}




const dnxf_data_device_properties_unsupported_t *
dnxf_data_device_properties_unsupported_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_properties, dnxf_data_device_properties_table_unsupported);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnxf_data_device_properties_unsupported_t *) data;

}


shr_error_e
dnxf_data_device_properties_unsupported_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_device_properties_unsupported_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_properties, dnxf_data_device_properties_table_unsupported);
    data = (const dnxf_data_device_properties_unsupported_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
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
dnxf_data_device_properties_unsupported_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_properties, dnxf_data_device_properties_table_unsupported);

}






static shr_error_e
dnxf_data_device_emulation_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "emulation";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnxf_data_device_emulation_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data device emulation features");

    
    submodule_data->nof_defines = _dnxf_data_device_emulation_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data device emulation defines");

    submodule_data->defines[dnxf_data_device_emulation_define_emulation_system].name = "emulation_system";
    submodule_data->defines[dnxf_data_device_emulation_define_emulation_system].doc = "";
    
    submodule_data->defines[dnxf_data_device_emulation_define_emulation_system].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnxf_data_device_emulation_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data device emulation tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_device_emulation_feature_get(
    int unit,
    dnxf_data_device_emulation_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_emulation, feature);
}


uint32
dnxf_data_device_emulation_emulation_system_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_emulation, dnxf_data_device_emulation_define_emulation_system);
}










static shr_error_e
dnxf_data_device_otp_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "otp";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnxf_data_device_otp_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data device otp features");

    
    submodule_data->nof_defines = _dnxf_data_device_otp_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data device otp defines");

    submodule_data->defines[dnxf_data_device_otp_define_nof_words_in_row].name = "nof_words_in_row";
    submodule_data->defines[dnxf_data_device_otp_define_nof_words_in_row].doc = "";
    
    submodule_data->defines[dnxf_data_device_otp_define_nof_words_in_row].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_otp_define_read_timeout].name = "read_timeout";
    submodule_data->defines[dnxf_data_device_otp_define_read_timeout].doc = "";
    
    submodule_data->defines[dnxf_data_device_otp_define_read_timeout].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnxf_data_device_otp_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data device otp tables");

    
    submodule_data->tables[dnxf_data_device_otp_table_otp_instance_attributes].name = "otp_instance_attributes";
    submodule_data->tables[dnxf_data_device_otp_table_otp_instance_attributes].doc = "";
    submodule_data->tables[dnxf_data_device_otp_table_otp_instance_attributes].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_device_otp_table_otp_instance_attributes].size_of_values = sizeof(dnxf_data_device_otp_otp_instance_attributes_t);
    submodule_data->tables[dnxf_data_device_otp_table_otp_instance_attributes].entry_get = dnxf_data_device_otp_otp_instance_attributes_entry_str_get;

    
    submodule_data->tables[dnxf_data_device_otp_table_otp_instance_attributes].nof_keys = 1;
    submodule_data->tables[dnxf_data_device_otp_table_otp_instance_attributes].keys[0].name = "otp_instance_index";
    submodule_data->tables[dnxf_data_device_otp_table_otp_instance_attributes].keys[0].doc = "";

    
    submodule_data->tables[dnxf_data_device_otp_table_otp_instance_attributes].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_device_otp_table_otp_instance_attributes].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_device_otp_table_otp_instance_attributes].nof_values, "_dnxf_data_device_otp_table_otp_instance_attributes table values");
    submodule_data->tables[dnxf_data_device_otp_table_otp_instance_attributes].values[0].name = "block_id";
    submodule_data->tables[dnxf_data_device_otp_table_otp_instance_attributes].values[0].type = "int";
    submodule_data->tables[dnxf_data_device_otp_table_otp_instance_attributes].values[0].doc = "";
    submodule_data->tables[dnxf_data_device_otp_table_otp_instance_attributes].values[0].offset = UTILEX_OFFSETOF(dnxf_data_device_otp_otp_instance_attributes_t, block_id);
    submodule_data->tables[dnxf_data_device_otp_table_otp_instance_attributes].values[1].name = "is_supported";
    submodule_data->tables[dnxf_data_device_otp_table_otp_instance_attributes].values[1].type = "int";
    submodule_data->tables[dnxf_data_device_otp_table_otp_instance_attributes].values[1].doc = "";
    submodule_data->tables[dnxf_data_device_otp_table_otp_instance_attributes].values[1].offset = UTILEX_OFFSETOF(dnxf_data_device_otp_otp_instance_attributes_t, is_supported);
    submodule_data->tables[dnxf_data_device_otp_table_otp_instance_attributes].values[2].name = "size";
    submodule_data->tables[dnxf_data_device_otp_table_otp_instance_attributes].values[2].type = "int";
    submodule_data->tables[dnxf_data_device_otp_table_otp_instance_attributes].values[2].doc = "";
    submodule_data->tables[dnxf_data_device_otp_table_otp_instance_attributes].values[2].offset = UTILEX_OFFSETOF(dnxf_data_device_otp_otp_instance_attributes_t, size);

    
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].name = "reg_addresses";
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].doc = "";
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].size_of_values = sizeof(dnxf_data_device_otp_reg_addresses_t);
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].entry_get = dnxf_data_device_otp_reg_addresses_entry_str_get;

    
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].nof_keys = 0;

    
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].nof_values = 8;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].nof_values, "_dnxf_data_device_otp_table_reg_addresses table values");
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[0].name = "mode";
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[0].type = "uint32";
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[0].doc = "";
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[0].offset = UTILEX_OFFSETOF(dnxf_data_device_otp_reg_addresses_t, mode);
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[1].name = "cpu_addr";
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[1].type = "uint32";
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[1].doc = "";
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[1].offset = UTILEX_OFFSETOF(dnxf_data_device_otp_reg_addresses_t, cpu_addr);
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[2].name = "cmd";
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[2].type = "uint32";
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[2].doc = "";
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[2].offset = UTILEX_OFFSETOF(dnxf_data_device_otp_reg_addresses_t, cmd);
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[3].name = "cmd_start";
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[3].type = "uint32";
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[3].doc = "";
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[3].offset = UTILEX_OFFSETOF(dnxf_data_device_otp_reg_addresses_t, cmd_start);
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[4].name = "cpu_status";
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[4].type = "uint32";
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[4].doc = "";
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[4].offset = UTILEX_OFFSETOF(dnxf_data_device_otp_reg_addresses_t, cpu_status);
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[5].name = "cpu_data_0";
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[5].type = "uint32";
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[5].doc = "";
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[5].offset = UTILEX_OFFSETOF(dnxf_data_device_otp_reg_addresses_t, cpu_data_0);
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[6].name = "cpu_data_1";
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[6].type = "uint32";
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[6].doc = "";
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[6].offset = UTILEX_OFFSETOF(dnxf_data_device_otp_reg_addresses_t, cpu_data_1);
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[7].name = "cpu_data_2";
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[7].type = "uint32";
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[7].doc = "";
    submodule_data->tables[dnxf_data_device_otp_table_reg_addresses].values[7].offset = UTILEX_OFFSETOF(dnxf_data_device_otp_reg_addresses_t, cpu_data_2);


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_device_otp_feature_get(
    int unit,
    dnxf_data_device_otp_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_otp, feature);
}


uint32
dnxf_data_device_otp_nof_words_in_row_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_otp, dnxf_data_device_otp_define_nof_words_in_row);
}

uint32
dnxf_data_device_otp_read_timeout_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_otp, dnxf_data_device_otp_define_read_timeout);
}



const dnxf_data_device_otp_otp_instance_attributes_t *
dnxf_data_device_otp_otp_instance_attributes_get(
    int unit,
    int otp_instance_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_otp, dnxf_data_device_otp_table_otp_instance_attributes);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, otp_instance_index, 0);
    return (const dnxf_data_device_otp_otp_instance_attributes_t *) data;

}

const dnxf_data_device_otp_reg_addresses_t *
dnxf_data_device_otp_reg_addresses_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_otp, dnxf_data_device_otp_table_reg_addresses);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnxf_data_device_otp_reg_addresses_t *) data;

}


shr_error_e
dnxf_data_device_otp_otp_instance_attributes_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_device_otp_otp_instance_attributes_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_otp, dnxf_data_device_otp_table_otp_instance_attributes);
    data = (const dnxf_data_device_otp_otp_instance_attributes_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->block_id);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_supported);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->size);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnxf_data_device_otp_reg_addresses_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_device_otp_reg_addresses_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_otp, dnxf_data_device_otp_table_reg_addresses);
    data = (const dnxf_data_device_otp_reg_addresses_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mode);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->cpu_addr);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->cmd);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->cmd_start);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->cpu_status);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->cpu_data_0);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->cpu_data_1);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->cpu_data_2);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnxf_data_device_otp_otp_instance_attributes_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_otp, dnxf_data_device_otp_table_otp_instance_attributes);

}

const dnxc_data_table_info_t *
dnxf_data_device_otp_reg_addresses_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_otp, dnxf_data_device_otp_table_reg_addresses);

}



shr_error_e
dnxf_data_device_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "device";
    module_data->nof_submodules = _dnxf_data_device_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data device submodules");

    
    SHR_IF_ERR_EXIT(dnxf_data_device_general_init(unit, &module_data->submodules[dnxf_data_device_submodule_general]));
    SHR_IF_ERR_EXIT(dnxf_data_device_access_init(unit, &module_data->submodules[dnxf_data_device_submodule_access]));
    SHR_IF_ERR_EXIT(dnxf_data_device_blocks_init(unit, &module_data->submodules[dnxf_data_device_submodule_blocks]));
    SHR_IF_ERR_EXIT(dnxf_data_device_interrupts_init(unit, &module_data->submodules[dnxf_data_device_submodule_interrupts]));
    SHR_IF_ERR_EXIT(dnxf_data_device_custom_features_init(unit, &module_data->submodules[dnxf_data_device_submodule_custom_features]));
    SHR_IF_ERR_EXIT(dnxf_data_device_properties_init(unit, &module_data->submodules[dnxf_data_device_submodule_properties]));
    SHR_IF_ERR_EXIT(dnxf_data_device_emulation_init(unit, &module_data->submodules[dnxf_data_device_submodule_emulation]));
    SHR_IF_ERR_EXIT(dnxf_data_device_otp_init(unit, &module_data->submodules[dnxf_data_device_submodule_otp]));
    
#ifdef BCM_DNXF1_SUPPORT

    if (dnxc_data_mgmt_is_ramon(unit))
    {
        SHR_IF_ERR_EXIT(ramon_a0_data_device_attach(unit));
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

