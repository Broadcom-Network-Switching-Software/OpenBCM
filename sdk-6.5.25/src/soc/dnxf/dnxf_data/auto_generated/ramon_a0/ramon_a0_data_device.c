
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DEVICE

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal_device.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/cmn/dnxf_fabric.h>
#include <soc/dnxf/ramon/ramon_intr.h>
#include <soc/dnxc/dnxc_defs.h>







static shr_error_e
ramon_a0_dnxf_data_device_general_production_ready_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_general;
    int feature_index = dnxf_data_device_general_production_ready;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_general_serdes_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_general;
    int feature_index = dnxf_data_device_general_serdes_support;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_general_fabric_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_general;
    int feature_index = dnxf_data_device_general_fabric_support;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_general_delay_exist_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_general;
    int feature_index = dnxf_data_device_general_delay_exist;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_general_cmic_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_general;
    int feature_index = dnxf_data_device_general_cmic_support;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_general_real_interrupts_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_general;
    int feature_index = dnxf_data_device_general_real_interrupts_support;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_general_i2c_hw_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_general;
    int feature_index = dnxf_data_device_general_i2c_hw_support;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_general_hw_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_general;
    int feature_index = dnxf_data_device_general_hw_support;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
ramon_a0_dnxf_data_device_general_nof_pvt_monitors_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_general;
    int define_index = dnxf_data_device_general_define_nof_pvt_monitors;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_general_pvt_base_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_general;
    int define_index = dnxf_data_device_general_define_pvt_base;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 43410000;

    
    define->data = 43410000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_general_pvt_factor_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_general;
    int define_index = dnxf_data_device_general_define_pvt_factor;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 53504;

    
    define->data = 53504;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_general_nof_cores_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_general;
    int define_index = dnxf_data_device_general_define_nof_cores;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_general_adapter_lib_ver_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_general;
    int define_index = dnxf_data_device_general_define_adapter_lib_ver;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5;

    
    define->data = 5;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_general_hard_reset_disable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_general;
    int define_index = dnxf_data_device_general_define_hard_reset_disable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
        "Init the device without hard reset.\n"
        "Can be 0 and 1\n"
        "Default: 0\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "init_without_device_hard_reset";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_general_core_clock_khz_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_general;
    int define_index = dnxf_data_device_general_define_core_clock_khz;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1000000;

    
    define->data = 1000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CORE_CLOCK_SPEED;
    define->property.doc = 
        "\n"
        "Device core clock frequency in kHZ\n"
        "Synopsis: core_clock_speed_khz=#freq#\n"
        "Only supported value is 1G\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_direct_map;
    define->property.method_str = "suffix_direct_map";
    define->property.suffix = "khz";
    define->property.nof_mapping = 1;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "1000000";
    define->property.mapping[0].val = 1000000;
    define->property.mapping[0].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_general_system_ref_core_clock_khz_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_general;
    int define_index = dnxf_data_device_general_define_system_ref_core_clock_khz;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1200000;

    
    define->data = 1200000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_SYSTEM_REF_CORE_CLOCK;
    define->property.doc = 
        "\n"
        "Specifies system reference clock speed in kHz.\n"
        "For a system containing a legacy device, the system reference clock needs to be set according to the legacy device.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "khz";
    define->property.range_min = 300000;
    define->property.range_max = 3000000;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_general_device_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_general;
    int define_index = dnxf_data_device_general_define_device_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x8790;

    
    define->data = 0x8790;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
ramon_a0_dnxf_data_device_general_ctest_full_file_indentifier_set(
    int unit)
{
    dnxf_data_device_general_ctest_full_file_indentifier_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_general;
    int table_index = dnxf_data_device_general_table_ctest_full_file_indentifier;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "DNXF";
    table->values[1].default_val = "3";
    
    DNXC_DATA_ALLOC(table->data, dnxf_data_device_general_ctest_full_file_indentifier_t, (1 + 1  ), "data of dnxf_data_device_general_table_ctest_full_file_indentifier");

    
    default_data = (dnxf_data_device_general_ctest_full_file_indentifier_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->family_name = "DNXF";
    default_data->family_number = "3";
    
    data = (dnxf_data_device_general_ctest_full_file_indentifier_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
ramon_a0_dnxf_data_device_access_table_dma_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_access;
    int define_index = dnxf_data_device_access_define_table_dma_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_TABLE_DMA_ENABLE;
    define->property.doc = 
        "\n"
        "Enable/disable table DMA operations.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_enable;
    define->property.method_str = "enable";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_access_tdma_timeout_usec_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_access;
    int define_index = dnxf_data_device_access_define_tdma_timeout_usec;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1000;

    
    define->data = 1000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_TDMA_TIMEOUT_USEC;
    define->property.doc = 
        "\n"
        "Table DMA operation timeout.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 10000000;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_access_tdma_intr_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_access;
    int define_index = dnxf_data_device_access_define_tdma_intr_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_TDMA_INTR_ENABLE;
    define->property.doc = 
        "\n"
        "Table DMA done is interrupt driven or by polling.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_enable;
    define->property.method_str = "enable";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_access_tslam_dma_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_access;
    int define_index = dnxf_data_device_access_define_tslam_dma_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_TSLAM_DMA_ENABLE;
    define->property.doc = 
        "\n"
        "Enable/disable tslam DMA operations.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_enable;
    define->property.method_str = "enable";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_access_tslam_timeout_usec_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_access;
    int define_index = dnxf_data_device_access_define_tslam_timeout_usec;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = SOC_DNXF_PROPERTY_UNAVAIL;

    
    define->data = SOC_DNXF_PROPERTY_UNAVAIL;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_TSLAM_TIMEOUT_USEC;
    define->property.doc = 
        "\n"
        "Tslam DMA operation timeout.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 10000000;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_access_tslam_int_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_access;
    int define_index = dnxf_data_device_access_define_tslam_int_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_TSLAM_INTR_ENABLE;
    define->property.doc = 
        "\n"
        "Tslam DMA done is interrupt driven or by polling.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_enable;
    define->property.method_str = "enable";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_access_mdio_int_divisor_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_access;
    int define_index = dnxf_data_device_access_define_mdio_int_divisor;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 50;

    
    define->data = 50;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_RATE_INT_MDIO_DIVISOR;
    define->property.doc = 
        "\n"
        "mdio_int_divisor\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 10000000;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_access_mdio_ext_divisor_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_access;
    int define_index = dnxf_data_device_access_define_mdio_ext_divisor;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 14;

    
    define->data = 14;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_RATE_EXT_MDIO_DIVISOR;
    define->property.doc = 
        "\n"
        "mdio_ext_divisor\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 10000000;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_access_mdio_int_div_out_delay_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_access;
    int define_index = dnxf_data_device_access_define_mdio_int_div_out_delay;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 25;

    
    define->data = 25;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_MDIO_OUTPUT_DELAY;
    define->property.doc = 
        "\n"
        "Configure number of clock delay between the rising edge of MDC and the starting data of MDIO\n"
        "for internal access\n"
        "mdio_output_delay= 0~255\n"
        "Default: 9\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "int";
    define->property.range_min = 0;
    define->property.range_max = 255;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_access_mdio_ext_div_out_delay_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_access;
    int define_index = dnxf_data_device_access_define_mdio_ext_div_out_delay;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 14;

    
    define->data = 14;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_MDIO_OUTPUT_DELAY;
    define->property.doc = 
        "\n"
        "Configure number of clock delay between the rising edge of MDC and the starting data of MDIO\n"
        "for external access\n"
        "mdio_output_delay= 0~255\n"
        "Default: 9\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "ext";
    define->property.range_min = 0;
    define->property.range_max = 255;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_access_bist_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_access;
    int define_index = dnxf_data_device_access_define_bist_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_BIST_ENABLE;
    define->property.doc = 
        "\n"
        "Determines if to run Memory Built-In Self-Test (MBIST) of internal memory (tables) during startup.\n"
        "0 - do not run\n"
        "1 - run\n"
        "2 - run with extra logs\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 2;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_access_sbus_dma_interval_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_access;
    int define_index = dnxf_data_device_access_define_sbus_dma_interval;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_DMA_DESC_TIMEOUT_USEC;
    define->property.doc = 
        "\n"
        "SBUSDMA descriptor mode operation timeout in microseconds.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 1000000000;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_access_sbus_dma_intr_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_access;
    int define_index = dnxf_data_device_access_define_sbus_dma_intr_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_DMA_DESC_INTR_ENABLE;
    define->property.doc = 
        "\n"
        "SBUSDMA descriptor mode operation should use interrupt rather than poll for completion.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_enable;
    define->property.method_str = "enable";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_access_mem_clear_chunk_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_access;
    int define_index = dnxf_data_device_access_define_mem_clear_chunk_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_MEM_CLEAR_CHUNK_SIZE;
    define->property.doc = 
        "\n"
        "SBUS DMA chunk size configuration.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 1000;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
ramon_a0_dnxf_data_device_blocks_dch_reset_restore_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int feature_index = dnxf_data_device_blocks_dch_reset_restore_support;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_all_blocks_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_all_blocks;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 128;

    
    define->data = 128;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_instances_fmac_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_instances_fmac;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 48;

    
    define->data = 48;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_links_in_fmac_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_links_in_fmac;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_instances_fsrd_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_instances_fsrd;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 24;

    
    define->data = 24;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_instances_brdc_fsrd_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_instances_brdc_fsrd;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_instances_dch_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_instances_dch;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_links_in_dch_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_links_in_dch;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 24;

    
    define->data = 24;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_dch_link_groups_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_dch_link_groups;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_instances_cch_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_instances_cch;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_links_in_cch_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_links_in_cch;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 24;

    
    define->data = 24;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_links_in_dcq_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_links_in_dcq;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 24;

    
    define->data = 24;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_instances_rtp_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_instances_rtp;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_links_in_rtp_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_links_in_rtp;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 192;

    
    define->data = 192;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_instances_occg_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_instances_occg;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_instances_eci_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_instances_eci;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_instances_cmic_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_instances_cmic;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_instances_mesh_topology_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_instances_mesh_topology;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_instances_brdc_fmach_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_instances_brdc_fmach;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_instances_brdc_fmacl_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_instances_brdc_fmacl;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_instances_lcm_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_instances_lcm;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_instances_mct_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_instances_mct;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_instances_qrh_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_instances_qrh;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_instances_dcml_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_instances_dcml;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_links_in_dcml_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_links_in_dcml;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 24;

    
    define->data = 24;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_links_in_lcm_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_links_in_lcm;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 24;

    
    define->data = 24;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_links_in_qrh_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_links_in_qrh;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 24;

    
    define->data = 24;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_dtm_fifos_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_dtm_fifos;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_links_in_fsrd_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_links_in_fsrd;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_links_in_phy_core_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_links_in_phy_core;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_nof_fmacs_in_fsrd_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int define_index = dnxf_data_device_blocks_define_nof_fmacs_in_fsrd;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
ramon_a0_dnxf_data_device_blocks_override_set(
    int unit)
{
    int index_index;
    dnxf_data_device_blocks_override_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int table_index = dnxf_data_device_blocks_table_override;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 10;
    table->info_get.key_size[0] = 10;

    
    table->values[0].default_val = "NULL";
    table->values[1].default_val = "-1";
    table->values[2].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_device_blocks_override_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_device_blocks_table_override");

    
    default_data = (dnxf_data_device_blocks_override_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->block_type = NULL;
    default_data->block_instance = -1;
    default_data->value = -1;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnxf_data_device_blocks_override_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_fsrd_sbus_chain_set(
    int unit)
{
    int fsrd_id_index;
    dnxf_data_device_blocks_fsrd_sbus_chain_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int table_index = dnxf_data_device_blocks_table_fsrd_sbus_chain;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnxf_data_device.blocks.nof_instances_fsrd_get(unit);
    table->info_get.key_size[0] = dnxf_data_device.blocks.nof_instances_fsrd_get(unit);

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_device_blocks_fsrd_sbus_chain_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_device_blocks_table_fsrd_sbus_chain");

    
    default_data = (dnxf_data_device_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->index = -1;
    
    for (fsrd_id_index = 0; fsrd_id_index < table->keys[0].size; fsrd_id_index++)
    {
        data = (dnxf_data_device_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, fsrd_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->index = 9;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->index = 10;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->index = 11;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->index = 8;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->index = 7;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->index = 6;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->index = 0;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->index = 1;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->index = 2;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->index = 3;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->index = 4;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->index = 5;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->index = 21;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->index = 22;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->index = 23;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->index = 20;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->index = 19;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->index = 18;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->index = 12;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->index = 13;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->index = 14;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->index = 15;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->index = 16;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->index = 17;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_fmac_sbus_chain_set(
    int unit)
{
    int fmac_id_index;
    dnxf_data_device_blocks_fmac_sbus_chain_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int table_index = dnxf_data_device_blocks_table_fmac_sbus_chain;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnxf_data_device.blocks.nof_instances_fmac_get(unit);
    table->info_get.key_size[0] = dnxf_data_device.blocks.nof_instances_fmac_get(unit);

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_device_blocks_fmac_sbus_chain_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_device_blocks_table_fmac_sbus_chain");

    
    default_data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->index = -1;
    
    for (fmac_id_index = 0; fmac_id_index < table->keys[0].size; fmac_id_index++)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, fmac_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->index = 19;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->index = 18;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->index = 21;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->index = 20;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->index = 23;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->index = 22;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->index = 16;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->index = 17;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->index = 15;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->index = 14;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->index = 12;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->index = 13;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->index = 0;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->index = 1;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->index = 2;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->index = 3;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->index = 4;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->index = 5;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->index = 7;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->index = 6;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->index = 8;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->index = 9;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->index = 11;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->index = 10;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->index = 43;
    }
    if (25 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, 0);
        data->index = 42;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->index = 45;
    }
    if (27 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, 0);
        data->index = 44;
    }
    if (28 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 28, 0);
        data->index = 47;
    }
    if (29 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 29, 0);
        data->index = 46;
    }
    if (30 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 30, 0);
        data->index = 40;
    }
    if (31 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 31, 0);
        data->index = 41;
    }
    if (32 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 32, 0);
        data->index = 39;
    }
    if (33 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 33, 0);
        data->index = 38;
    }
    if (34 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 34, 0);
        data->index = 36;
    }
    if (35 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 35, 0);
        data->index = 37;
    }
    if (36 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 36, 0);
        data->index = 24;
    }
    if (37 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 37, 0);
        data->index = 25;
    }
    if (38 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 38, 0);
        data->index = 26;
    }
    if (39 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 39, 0);
        data->index = 27;
    }
    if (40 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 40, 0);
        data->index = 28;
    }
    if (41 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 41, 0);
        data->index = 29;
    }
    if (42 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 42, 0);
        data->index = 31;
    }
    if (43 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 43, 0);
        data->index = 30;
    }
    if (44 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 44, 0);
        data->index = 32;
    }
    if (45 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 45, 0);
        data->index = 33;
    }
    if (46 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 46, 0);
        data->index = 35;
    }
    if (47 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 47, 0);
        data->index = 34;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_blocks_dch_instances_to_device_stage_set(
    int unit)
{
    int device_stage_index;
    dnxf_data_device_blocks_dch_instances_to_device_stage_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_blocks;
    int table_index = dnxf_data_device_blocks_table_dch_instances_to_device_stage;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 3;
    table->info_get.key_size[0] = 3;

    
    table->values[0].default_val = "0xff";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_device_blocks_dch_instances_to_device_stage_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_device_blocks_table_dch_instances_to_device_stage");

    
    default_data = (dnxf_data_device_blocks_dch_instances_to_device_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    _SHR_PBMP_WORD_SET(default_data->dch_instance_bmp, 0, 0xff);
    
    for (device_stage_index = 0; device_stage_index < table->keys[0].size; device_stage_index++)
    {
        data = (dnxf_data_device_blocks_dch_instances_to_device_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, device_stage_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (soc_dnxf_fabric_device_stage_fe1 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_dch_instances_to_device_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, soc_dnxf_fabric_device_stage_fe1, 0);
        _SHR_PBMP_WORD_SET(data->dch_instance_bmp, 0, 0x0f);
    }
    if (soc_dnxf_fabric_device_stage_fe3 < table->keys[0].size)
    {
        data = (dnxf_data_device_blocks_dch_instances_to_device_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, soc_dnxf_fabric_device_stage_fe3, 0);
        _SHR_PBMP_WORD_SET(data->dch_instance_bmp, 0, 0xf0);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
ramon_a0_dnxf_data_device_interrupts_nof_interrupts_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_interrupts;
    int define_index = dnxf_data_device_interrupts_define_nof_interrupts;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = RAMON_INT_LAST;

    
    define->data = RAMON_INT_LAST;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
ramon_a0_dnxf_data_device_custom_features_mesh_topology_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_custom_features;
    int define_index = dnxf_data_device_custom_features_define_mesh_topology_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = -1;

    
    define->data = -1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
        "To determine if there is a multiple pipe device in the system:\n"
        "custom_feature_mesh_topology_size=-1/0/1/2/3\n"
        "Default: -1\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range_signed;
    define->property.method_str = "suffix_range_signed";
    define->property.suffix = "mesh_topology_size";
    define->property.range_min = -1;
    define->property.range_max = 3;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_custom_features_mesh_topology_fast_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_custom_features;
    int define_index = dnxf_data_device_custom_features_define_mesh_topology_fast;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
        "Device is in mesh topology fast mode.\n"
        "Used for internal validation only!\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "mesh_topology_fast";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_device_custom_features_active_core_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_custom_features;
    int define_index = dnxf_data_device_custom_features_define_active_core;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}








static shr_error_e
ramon_a0_dnxf_data_device_properties_unsupported_set(
    int unit)
{
    int index_index;
    dnxf_data_device_properties_unsupported_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_properties;
    int table_index = dnxf_data_device_properties_table_unsupported;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "NULL";
    table->values[1].default_val = "NULL";
    table->values[2].default_val = "-1";
    table->values[3].default_val = "0";
    table->values[4].default_val = "NULL";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_device_properties_unsupported_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_device_properties_table_unsupported");

    
    default_data = (dnxf_data_device_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->property = NULL;
    default_data->suffix = NULL;
    default_data->num_max = -1;
    default_data->per_port = 0;
    default_data->err_msg = NULL;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnxf_data_device_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnxf_data_device_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->property = "phy_rx_lane_map";
        data->suffix = "quad";
        data->err_msg = "legacy SOC property configuration phy_rx_lane_map_quad not supported for this device";
    }
    if (1 < table->keys[0].size)
    {
        data = (dnxf_data_device_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->property = "phy_tx_lane_map";
        data->suffix = "quad";
        data->err_msg = "legacy SOC property configuration phy_tx_lane_map_quad not supported for this device";
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
ramon_a0_dnxf_data_device_emulation_emulation_system_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_device;
    int submodule_index = dnxf_data_device_submodule_emulation;
    int define_index = dnxf_data_device_emulation_define_emulation_system;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
ramon_a0_data_device_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnxf_data_module_device;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnxf_data_device_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnxf_data_device_general_define_nof_pvt_monitors;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_general_nof_pvt_monitors_set;
    data_index = dnxf_data_device_general_define_pvt_base;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_general_pvt_base_set;
    data_index = dnxf_data_device_general_define_pvt_factor;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_general_pvt_factor_set;
    data_index = dnxf_data_device_general_define_nof_cores;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_general_nof_cores_set;
    data_index = dnxf_data_device_general_define_adapter_lib_ver;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_general_adapter_lib_ver_set;
    data_index = dnxf_data_device_general_define_hard_reset_disable;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_general_hard_reset_disable_set;
    data_index = dnxf_data_device_general_define_core_clock_khz;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_general_core_clock_khz_set;
    data_index = dnxf_data_device_general_define_system_ref_core_clock_khz;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_general_system_ref_core_clock_khz_set;
    data_index = dnxf_data_device_general_define_device_id;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_general_device_id_set;

    
    data_index = dnxf_data_device_general_production_ready;
    feature = &submodule->features[data_index];
    feature->set = ramon_a0_dnxf_data_device_general_production_ready_set;
    data_index = dnxf_data_device_general_serdes_support;
    feature = &submodule->features[data_index];
    feature->set = ramon_a0_dnxf_data_device_general_serdes_support_set;
    data_index = dnxf_data_device_general_fabric_support;
    feature = &submodule->features[data_index];
    feature->set = ramon_a0_dnxf_data_device_general_fabric_support_set;
    data_index = dnxf_data_device_general_delay_exist;
    feature = &submodule->features[data_index];
    feature->set = ramon_a0_dnxf_data_device_general_delay_exist_set;
    data_index = dnxf_data_device_general_cmic_support;
    feature = &submodule->features[data_index];
    feature->set = ramon_a0_dnxf_data_device_general_cmic_support_set;
    data_index = dnxf_data_device_general_real_interrupts_support;
    feature = &submodule->features[data_index];
    feature->set = ramon_a0_dnxf_data_device_general_real_interrupts_support_set;
    data_index = dnxf_data_device_general_i2c_hw_support;
    feature = &submodule->features[data_index];
    feature->set = ramon_a0_dnxf_data_device_general_i2c_hw_support_set;
    data_index = dnxf_data_device_general_hw_support;
    feature = &submodule->features[data_index];
    feature->set = ramon_a0_dnxf_data_device_general_hw_support_set;

    
    data_index = dnxf_data_device_general_table_ctest_full_file_indentifier;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_device_general_ctest_full_file_indentifier_set;
    
    submodule_index = dnxf_data_device_submodule_access;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnxf_data_device_access_define_table_dma_enable;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_access_table_dma_enable_set;
    data_index = dnxf_data_device_access_define_tdma_timeout_usec;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_access_tdma_timeout_usec_set;
    data_index = dnxf_data_device_access_define_tdma_intr_enable;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_access_tdma_intr_enable_set;
    data_index = dnxf_data_device_access_define_tslam_dma_enable;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_access_tslam_dma_enable_set;
    data_index = dnxf_data_device_access_define_tslam_timeout_usec;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_access_tslam_timeout_usec_set;
    data_index = dnxf_data_device_access_define_tslam_int_enable;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_access_tslam_int_enable_set;
    data_index = dnxf_data_device_access_define_mdio_int_divisor;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_access_mdio_int_divisor_set;
    data_index = dnxf_data_device_access_define_mdio_ext_divisor;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_access_mdio_ext_divisor_set;
    data_index = dnxf_data_device_access_define_mdio_int_div_out_delay;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_access_mdio_int_div_out_delay_set;
    data_index = dnxf_data_device_access_define_mdio_ext_div_out_delay;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_access_mdio_ext_div_out_delay_set;
    data_index = dnxf_data_device_access_define_bist_enable;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_access_bist_enable_set;
    data_index = dnxf_data_device_access_define_sbus_dma_interval;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_access_sbus_dma_interval_set;
    data_index = dnxf_data_device_access_define_sbus_dma_intr_enable;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_access_sbus_dma_intr_enable_set;
    data_index = dnxf_data_device_access_define_mem_clear_chunk_size;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_access_mem_clear_chunk_size_set;

    

    
    
    submodule_index = dnxf_data_device_submodule_blocks;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnxf_data_device_blocks_define_nof_all_blocks;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_all_blocks_set;
    data_index = dnxf_data_device_blocks_define_nof_instances_fmac;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_instances_fmac_set;
    data_index = dnxf_data_device_blocks_define_nof_links_in_fmac;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_links_in_fmac_set;
    data_index = dnxf_data_device_blocks_define_nof_instances_fsrd;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_instances_fsrd_set;
    data_index = dnxf_data_device_blocks_define_nof_instances_brdc_fsrd;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_instances_brdc_fsrd_set;
    data_index = dnxf_data_device_blocks_define_nof_instances_dch;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_instances_dch_set;
    data_index = dnxf_data_device_blocks_define_nof_links_in_dch;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_links_in_dch_set;
    data_index = dnxf_data_device_blocks_define_nof_dch_link_groups;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_dch_link_groups_set;
    data_index = dnxf_data_device_blocks_define_nof_instances_cch;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_instances_cch_set;
    data_index = dnxf_data_device_blocks_define_nof_links_in_cch;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_links_in_cch_set;
    data_index = dnxf_data_device_blocks_define_nof_links_in_dcq;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_links_in_dcq_set;
    data_index = dnxf_data_device_blocks_define_nof_instances_rtp;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_instances_rtp_set;
    data_index = dnxf_data_device_blocks_define_nof_links_in_rtp;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_links_in_rtp_set;
    data_index = dnxf_data_device_blocks_define_nof_instances_occg;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_instances_occg_set;
    data_index = dnxf_data_device_blocks_define_nof_instances_eci;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_instances_eci_set;
    data_index = dnxf_data_device_blocks_define_nof_instances_cmic;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_instances_cmic_set;
    data_index = dnxf_data_device_blocks_define_nof_instances_mesh_topology;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_instances_mesh_topology_set;
    data_index = dnxf_data_device_blocks_define_nof_instances_brdc_fmach;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_instances_brdc_fmach_set;
    data_index = dnxf_data_device_blocks_define_nof_instances_brdc_fmacl;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_instances_brdc_fmacl_set;
    data_index = dnxf_data_device_blocks_define_nof_instances_lcm;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_instances_lcm_set;
    data_index = dnxf_data_device_blocks_define_nof_instances_mct;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_instances_mct_set;
    data_index = dnxf_data_device_blocks_define_nof_instances_qrh;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_instances_qrh_set;
    data_index = dnxf_data_device_blocks_define_nof_instances_dcml;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_instances_dcml_set;
    data_index = dnxf_data_device_blocks_define_nof_links_in_dcml;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_links_in_dcml_set;
    data_index = dnxf_data_device_blocks_define_nof_links_in_lcm;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_links_in_lcm_set;
    data_index = dnxf_data_device_blocks_define_nof_links_in_qrh;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_links_in_qrh_set;
    data_index = dnxf_data_device_blocks_define_nof_dtm_fifos;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_dtm_fifos_set;
    data_index = dnxf_data_device_blocks_define_nof_links_in_fsrd;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_links_in_fsrd_set;
    data_index = dnxf_data_device_blocks_define_nof_links_in_phy_core;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_links_in_phy_core_set;
    data_index = dnxf_data_device_blocks_define_nof_fmacs_in_fsrd;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_blocks_nof_fmacs_in_fsrd_set;

    
    data_index = dnxf_data_device_blocks_dch_reset_restore_support;
    feature = &submodule->features[data_index];
    feature->set = ramon_a0_dnxf_data_device_blocks_dch_reset_restore_support_set;

    
    data_index = dnxf_data_device_blocks_table_override;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_device_blocks_override_set;
    data_index = dnxf_data_device_blocks_table_fsrd_sbus_chain;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_device_blocks_fsrd_sbus_chain_set;
    data_index = dnxf_data_device_blocks_table_fmac_sbus_chain;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_device_blocks_fmac_sbus_chain_set;
    data_index = dnxf_data_device_blocks_table_dch_instances_to_device_stage;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_device_blocks_dch_instances_to_device_stage_set;
    
    submodule_index = dnxf_data_device_submodule_interrupts;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnxf_data_device_interrupts_define_nof_interrupts;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_interrupts_nof_interrupts_set;

    

    
    
    submodule_index = dnxf_data_device_submodule_custom_features;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnxf_data_device_custom_features_define_mesh_topology_size;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_custom_features_mesh_topology_size_set;
    data_index = dnxf_data_device_custom_features_define_mesh_topology_fast;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_custom_features_mesh_topology_fast_set;
    data_index = dnxf_data_device_custom_features_define_active_core;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_custom_features_active_core_set;

    

    
    
    submodule_index = dnxf_data_device_submodule_properties;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnxf_data_device_properties_table_unsupported;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_device_properties_unsupported_set;
    
    submodule_index = dnxf_data_device_submodule_emulation;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnxf_data_device_emulation_define_emulation_system;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_device_emulation_emulation_system_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

