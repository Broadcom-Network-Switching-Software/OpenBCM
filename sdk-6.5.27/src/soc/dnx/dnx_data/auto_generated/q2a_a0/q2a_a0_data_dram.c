
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DRAM

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_dram.h>
#include <bcm_int/dnx/dram/gddr6/gddr6.h>
#include <bcm_int/dnx/dram/dram.h>







static shr_error_e
q2a_a0_dnx_data_dram_hbm_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int feature_index = dnx_data_dram_hbm_is_supported;
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
q2a_a0_dnx_data_dram_hbm_is_delete_bdb_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int feature_index = dnx_data_dram_hbm_is_delete_bdb_supported;
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
q2a_a0_dnx_data_dram_gddr6_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int feature_index = dnx_data_dram_gddr6_is_supported;
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
q2a_a0_dnx_data_dram_gddr6_allow_disable_read_crc_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int feature_index = dnx_data_dram_gddr6_allow_disable_read_crc;
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
q2a_a0_dnx_data_dram_gddr6_interleaved_refresh_cycles_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int feature_index = dnx_data_dram_gddr6_interleaved_refresh_cycles;
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
q2a_a0_dnx_data_dram_gddr6_enable_temp_read_samsung_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int feature_index = dnx_data_dram_gddr6_enable_temp_read_samsung;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_EXT_RAM_TEMP_READ_ENABLE;
    feature->property.doc = 
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_enable;
    feature->property.method_str = "enable";
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_gddr6_enable_temp_read_micron_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int feature_index = dnx_data_dram_gddr6_enable_temp_read_micron;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_EXT_RAM_TEMP_READ_ENABLE;
    feature->property.doc = 
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_enable;
    feature->property.method_str = "enable";
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
q2a_a0_dnx_data_dram_gddr6_nof_ca_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int define_index = dnx_data_dram_gddr6_define_nof_ca_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 11;

    
    define->data = 11;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_gddr6_bytes_per_channel_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int define_index = dnx_data_dram_gddr6_define_bytes_per_channel;
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
q2a_a0_dnx_data_dram_gddr6_training_fifo_depth_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int define_index = dnx_data_dram_gddr6_define_training_fifo_depth;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6;

    
    define->data = 6;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_gddr6_refresh_mechanism_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int define_index = dnx_data_dram_gddr6_define_refresh_mechanism_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "refresh_mechanism_enable";
    define->property.doc = 
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
q2a_a0_dnx_data_dram_gddr6_bist_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int define_index = dnx_data_dram_gddr6_define_bist_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_BIST_ENABLE;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_only_enable;
    define->property.method_str = "suffix_only_enable";
    define->property.suffix = "dram";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_gddr6_dynamic_calibration_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int define_index = dnx_data_dram_gddr6_define_dynamic_calibration_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->flags |= DNXC_DATA_F_INIT_ONLY;
    
    define->property.name = spn_EXT_RAM_DYNAMIC_CALIBRATION_ENABLE;
    define->property.doc = 
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
q2a_a0_dnx_data_dram_gddr6_cdr_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int define_index = dnx_data_dram_gddr6_define_cdr_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->flags |= DNXC_DATA_F_INIT_ONLY;
    
    define->property.name = "ext_ram_cdr_enable";
    define->property.doc = 
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
q2a_a0_dnx_data_dram_gddr6_write_recovery_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int define_index = dnx_data_dram_gddr6_define_write_recovery;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 19;

    
    define->data = 19;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "ext_ram_write_recovery";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 15;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_gddr6_cabi_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int define_index = dnx_data_dram_gddr6_define_cabi;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "ext_ram_cabi";
    define->property.doc = 
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
q2a_a0_dnx_data_dram_gddr6_dram_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int define_index = dnx_data_dram_gddr6_define_dram_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "ext_ram_dram_mode";
    define->property.doc = 
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
q2a_a0_dnx_data_dram_gddr6_command_pipe_extra_delay_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int define_index = dnx_data_dram_gddr6_define_command_pipe_extra_delay;
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
q2a_a0_dnx_data_dram_gddr6_use_11bits_ca_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int define_index = dnx_data_dram_gddr6_define_use_11bits_ca;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "ext_ram_use_11bits_ca";
    define->property.doc = 
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
q2a_a0_dnx_data_dram_gddr6_ck_odt_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int define_index = dnx_data_dram_gddr6_define_ck_odt;
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
q2a_a0_dnx_data_dram_gddr6_dynamic_calibration_period_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int define_index = dnx_data_dram_gddr6_define_dynamic_calibration_period;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 700000;

    
    define->data = 700000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "ext_ram_dynamic_calibration_period";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 1000000;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_gddr6_ck_termination_mr_conf_val_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int define_index = dnx_data_dram_gddr6_define_ck_termination_mr_conf_val;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_EXT_RAM_CK_TERMINATION;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 3;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "60_OHM";
    define->property.mapping[0].val = 1;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "120_OHM";
    define->property.mapping[1].val = 2;
    define->property.mapping[2].name = "DISABLED";
    define->property.mapping[2].val = 3;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_gddr6_ck_termination_ca_conf_val_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int define_index = dnx_data_dram_gddr6_define_ck_termination_ca_conf_val;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_EXT_RAM_CK_TERMINATION;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 3;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "DISABLED";
    define->property.mapping[0].val = 0;
    define->property.mapping[1].name = "60_OHM";
    define->property.mapping[1].val = 2;
    define->property.mapping[1].is_default = 1 ;
    define->property.mapping[2].name = "120_OHM";
    define->property.mapping[2].val = 3;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_gddr6_wck_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int define_index = dnx_data_dram_gddr6_define_wck_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_EXT_RAM_WCK_GRANULARITY;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "WCK_PER_BYTE";
    define->property.mapping[0].val = GDDR6_DRAM_WCK_PER_BYTE;
    define->property.mapping[1].name = "WCK_PER_WORD";
    define->property.mapping[1].val = GDDR6_DRAM_WCK_PER_WORD;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_gddr6_ref_clk_bitmap_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int define_index = dnx_data_dram_gddr6_define_ref_clk_bitmap;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x1;

    
    define->data = 0x1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_gddr6_ca_termination_conf_val_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int define_index = dnx_data_dram_gddr6_define_ca_termination_conf_val;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x1;

    
    define->data = 0x1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_EXT_RAM_CA_TERMINATION;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 3;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "DISABLED";
    define->property.mapping[0].val = 0x0;
    define->property.mapping[1].name = "60_OHM";
    define->property.mapping[1].val = 0x1;
    define->property.mapping[1].is_default = 1 ;
    define->property.mapping[2].name = "120_OHM";
    define->property.mapping[2].val = 0x2;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
q2a_a0_dnx_data_dram_gddr6_refresh_intervals_set(
    int unit)
{
    dnx_data_dram_gddr6_refresh_intervals_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int table_index = dnx_data_dram_gddr6_table_refresh_intervals;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "3900";
    table->values[2].default_val = "-1";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_gddr6_refresh_intervals_t, (1 + 1  ), "data of dnx_data_dram_gddr6_table_refresh_intervals");

    
    default_data = (dnx_data_dram_gddr6_refresh_intervals_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->trefiab = -1;
    default_data->trefiab_acc = 3900;
    default_data->trefisb = -1;
    
    data = (dnx_data_dram_gddr6_refresh_intervals_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->values[0].property.name = "ext_ram_t_abref_in_ns";
    table->values[0].property.doc =
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_custom;
    table->values[0].property.method_str = "custom";
    
    table->values[1].property.name = "ext_ram_t_abref_acc_in_ns";
    table->values[1].property.doc =
        "\n"
    ;
    table->values[1].property.method = dnxc_data_property_method_range;
    table->values[1].property.method_str = "range";
    table->values[1].property.range_min = 0;
    table->values[1].property.range_max = 8191;
    
    table->values[2].property.name = "ext_ram_t_refi_sb_in_ns";
    table->values[2].property.doc =
        "\n"
    ;
    table->values[2].property.method = dnxc_data_property_method_custom;
    table->values[2].property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnx_data_dram_gddr6_refresh_intervals_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnx_data_property_dram_gddr6_refresh_intervals_trefiab_read(unit, &data->trefiab));
    data = (dnx_data_dram_gddr6_refresh_intervals_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[1].property, -1, &data->trefiab_acc));
    data = (dnx_data_dram_gddr6_refresh_intervals_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnx_data_property_dram_gddr6_refresh_intervals_trefisb_read(unit, &data->trefisb));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_gddr6_dq_map_set(
    int unit)
{
    int dram_index_index;
    int byte_index;
    dnx_data_dram_gddr6_dq_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int table_index = dnx_data_dram_gddr6_table_dq_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_dram.general_info.max_nof_drams_get(unit);
    table->info_get.key_size[0] = dnx_data_dram.general_info.max_nof_drams_get(unit);
    table->keys[1].size = 2*dnx_data_dram.general_info.nof_channels_get(unit);
    table->info_get.key_size[1] = 2*dnx_data_dram.general_info.nof_channels_get(unit);

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_gddr6_dq_map_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_dram_gddr6_table_dq_map");

    
    default_data = (dnx_data_dram_gddr6_dq_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dq_map[0] = 0;
    
    for (dram_index_index = 0; dram_index_index < table->keys[0].size; dram_index_index++)
    {
        for (byte_index = 0; byte_index < table->keys[1].size; byte_index++)
        {
            data = (dnx_data_dram_gddr6_dq_map_t *) dnxc_data_mgmt_table_data_get(unit, table, dram_index_index, byte_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    table->property.name = "ext_ram_dq_swap";
    table->property.doc =
        "\n"
    ;
    table->property.method = dnxc_data_property_method_custom;
    table->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (dram_index_index = 0; dram_index_index < table->keys[0].size; dram_index_index++)
    {
        for (byte_index = 0; byte_index < table->keys[1].size; byte_index++)
        {
            data = (dnx_data_dram_gddr6_dq_map_t *) dnxc_data_mgmt_table_data_get(unit, table, dram_index_index, byte_index);
            SHR_IF_ERR_EXIT(dnx_data_property_dram_gddr6_dq_map_read(unit, dram_index_index, byte_index, data));
        }
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_gddr6_dq_channel_swap_set(
    int unit)
{
    int dram_index_index;
    dnx_data_dram_gddr6_dq_channel_swap_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int table_index = dnx_data_dram_gddr6_table_dq_channel_swap;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_dram.general_info.max_nof_drams_get(unit);
    table->info_get.key_size[0] = dnx_data_dram.general_info.max_nof_drams_get(unit);

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_gddr6_dq_channel_swap_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_gddr6_table_dq_channel_swap");

    
    default_data = (dnx_data_dram_gddr6_dq_channel_swap_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dq_channel_swap = 0;
    
    for (dram_index_index = 0; dram_index_index < table->keys[0].size; dram_index_index++)
    {
        data = (dnx_data_dram_gddr6_dq_channel_swap_t *) dnxc_data_mgmt_table_data_get(unit, table, dram_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->property.name = "ext_ram_channel_swap_en";
    table->property.doc =
        "\n"
    ;
    table->property.method = dnxc_data_property_method_custom;
    table->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (dram_index_index = 0; dram_index_index < table->keys[0].size; dram_index_index++)
    {
        data = (dnx_data_dram_gddr6_dq_channel_swap_t *) dnxc_data_mgmt_table_data_get(unit, table, dram_index_index, 0);
        SHR_IF_ERR_EXIT(dnx_data_property_dram_gddr6_dq_channel_swap_read(unit, dram_index_index, data));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_gddr6_dq_byte_map_set(
    int unit)
{
    int dram_index_index;
    int byte_index;
    dnx_data_dram_gddr6_dq_byte_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int table_index = dnx_data_dram_gddr6_table_dq_byte_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_dram.general_info.max_nof_drams_get(unit);
    table->info_get.key_size[0] = dnx_data_dram.general_info.max_nof_drams_get(unit);
    table->keys[1].size = dnx_data_dram.general_info.nof_channels_get(unit);
    table->info_get.key_size[1] = dnx_data_dram.general_info.nof_channels_get(unit);

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_gddr6_dq_byte_map_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_dram_gddr6_table_dq_byte_map");

    
    default_data = (dnx_data_dram_gddr6_dq_byte_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dq_byte_map = 0;
    
    for (dram_index_index = 0; dram_index_index < table->keys[0].size; dram_index_index++)
    {
        for (byte_index = 0; byte_index < table->keys[1].size; byte_index++)
        {
            data = (dnx_data_dram_gddr6_dq_byte_map_t *) dnxc_data_mgmt_table_data_get(unit, table, dram_index_index, byte_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    table->property.name = "ext_ram_dq_swap";
    table->property.doc =
        "\n"
    ;
    table->property.method = dnxc_data_property_method_custom;
    table->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (dram_index_index = 0; dram_index_index < table->keys[0].size; dram_index_index++)
    {
        for (byte_index = 0; byte_index < table->keys[1].size; byte_index++)
        {
            data = (dnx_data_dram_gddr6_dq_byte_map_t *) dnxc_data_mgmt_table_data_get(unit, table, dram_index_index, byte_index);
            SHR_IF_ERR_EXIT(dnx_data_property_dram_gddr6_dq_byte_map_read(unit, dram_index_index, byte_index, data));
        }
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_gddr6_ca_map_set(
    int unit)
{
    int dram_index_index;
    int channel_index;
    dnx_data_dram_gddr6_ca_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int table_index = dnx_data_dram_gddr6_table_ca_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_dram.general_info.max_nof_drams_get(unit);
    table->info_get.key_size[0] = dnx_data_dram.general_info.max_nof_drams_get(unit);
    table->keys[1].size = dnx_data_dram.general_info.nof_channels_get(unit);
    table->info_get.key_size[1] = dnx_data_dram.general_info.nof_channels_get(unit);

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_gddr6_ca_map_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_dram_gddr6_table_ca_map");

    
    default_data = (dnx_data_dram_gddr6_ca_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->ca_map[0] = 0;
    
    for (dram_index_index = 0; dram_index_index < table->keys[0].size; dram_index_index++)
    {
        for (channel_index = 0; channel_index < table->keys[1].size; channel_index++)
        {
            data = (dnx_data_dram_gddr6_ca_map_t *) dnxc_data_mgmt_table_data_get(unit, table, dram_index_index, channel_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    table->property.name = "ext_ram_addr_bank_swap";
    table->property.doc =
        "\n"
    ;
    table->property.method = dnxc_data_property_method_custom;
    table->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (dram_index_index = 0; dram_index_index < table->keys[0].size; dram_index_index++)
    {
        for (channel_index = 0; channel_index < table->keys[1].size; channel_index++)
        {
            data = (dnx_data_dram_gddr6_ca_map_t *) dnxc_data_mgmt_table_data_get(unit, table, dram_index_index, channel_index);
            SHR_IF_ERR_EXIT(dnx_data_property_dram_gddr6_ca_map_read(unit, dram_index_index, channel_index, data));
        }
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_gddr6_cadt_byte_map_set(
    int unit)
{
    int dram_index_index;
    int channel_index;
    dnx_data_dram_gddr6_cadt_byte_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int table_index = dnx_data_dram_gddr6_table_cadt_byte_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->flags |= DNXC_DATA_F_INIT_ONLY;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;
    table->keys[1].size = dnx_data_dram.general_info.nof_channels_get(unit);
    table->info_get.key_size[1] = dnx_data_dram.general_info.nof_channels_get(unit);

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_gddr6_cadt_byte_map_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_dram_gddr6_table_cadt_byte_map");

    
    default_data = (dnx_data_dram_gddr6_cadt_byte_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->cadt_byte_map = 0;
    
    for (dram_index_index = 0; dram_index_index < table->keys[0].size; dram_index_index++)
    {
        for (channel_index = 0; channel_index < table->keys[1].size; channel_index++)
        {
            data = (dnx_data_dram_gddr6_cadt_byte_map_t *) dnxc_data_mgmt_table_data_get(unit, table, dram_index_index, channel_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (0 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_dram_gddr6_cadt_byte_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->cadt_byte_map = 0;
    }
    if (0 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_dram_gddr6_cadt_byte_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 1);
        data->cadt_byte_map = 1;
    }
    if (1 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_dram_gddr6_cadt_byte_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->cadt_byte_map = 0;
    }
    if (1 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_dram_gddr6_cadt_byte_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 1);
        data->cadt_byte_map = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_gddr6_channel_regs_set(
    int unit)
{
    int index_index;
    dnx_data_dram_gddr6_channel_regs_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int table_index = dnx_data_dram_gddr6_table_channel_regs;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 29;
    table->info_get.key_size[0] = 29;

    
    table->values[0].default_val = "INVALIDr";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_gddr6_channel_regs_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_gddr6_table_channel_regs");

    
    default_data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->reg = INVALIDr;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->reg = DCC_HBM_CONFIGr;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->reg = DCC_HBM_MODE_REGISTERSr;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->reg = DCC_DRAM_MODE_REGISTER_MANAGER_CONFIGr;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->reg = DCC_DATA_SOURCE_CONFIGr;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->reg = DCC_RDR_ASYNC_FIFO_CONFIGr;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->reg = DCC_REQ_ARB_ASYNC_FIFO_CONFIGr;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->reg = DCC_WPBQ_CONFIGr;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->reg = DCC_RPBQ_CONFIGr;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->reg = DCC_DRAM_GENERAL_TIMINGSr;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->reg = DCC_HBM_COLUMN_ACCESS_TIMINGSr;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->reg = DCC_HBM_ROW_ACCESS_TIMINGSr;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->reg = DCC_DRAM_TRAINING_GENERAL_TIMINGSr;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->reg = DCC_HBM_REFRESH_TIMINGSr;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->reg = DCC_DRAM_REFRESH_INTERVALSr;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->reg = DCC_TRAINING_CADT_CONFIGr;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->reg = DCC_TRAINING_BIST_COMMAND_CONFIGr;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->reg = DCC_HW_DYNAMIC_CALIBRATION_CONFIGr;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->reg = DCC_COMMON_DYNAMIC_CALIBRATION_CONFIGr;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->reg = DCC_CDR_WRAPAROUND_UPDATE_CONFIGr;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->reg = DCC_BIST_DATA_CONFIGr;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->reg = DCC_BIST_COMMAND_CONFIGr;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->reg = DCC_BIST_ADDR_CONFIGr;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->reg = DCC_TSM_CONFIGr;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->reg = DCC_TSM_READ_WRITE_SWITCH_CONFIGr;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->reg = DCC_PIPELINES_CONFIGr;
    }
    if (25 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, 0);
        data->reg = DCC_PIPELINES_BANK_REMAPPINGr;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->reg = DCC_HBM_CPU_CONTROLr;
    }
    if (27 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, 0);
        data->reg = DCC_CA_BIT_MAPr;
    }
    if (28 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 28, 0);
        data->reg = DCC_DQ_BIT_MAPr;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_gddr6_controller_regs_set(
    int unit)
{
    int index_index;
    dnx_data_dram_gddr6_controller_regs_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int table_index = dnx_data_dram_gddr6_table_controller_regs;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 9;
    table->info_get.key_size[0] = 9;

    
    table->values[0].default_val = "INVALIDr";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_gddr6_controller_regs_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_gddr6_table_controller_regs");

    
    default_data = (dnx_data_dram_gddr6_controller_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->reg = INVALIDr;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dram_gddr6_controller_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_controller_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->reg = DPC_DRAM_PLL_CONFIGr;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_controller_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->reg = DPC_DRAM_RESET_CONTROLr;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_controller_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->reg = DPC_DRAM_PHY_CONTROLr;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_controller_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->reg = TDU_WRITE_REQUEST_PATH_CONFIGr;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_controller_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->reg = TDU_READ_REQUEST_PATH_CONFIGr;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_controller_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->reg = TDU_READ_DATA_PATH_CONFIGr;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_controller_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->reg = TDU_ADDRESS_TRANSLATION_MATRIXr;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_controller_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->reg = TDU_DRAM_BLOCKED_CONFIGr;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_controller_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->reg = TDU_ADDRESS_TRANSLATION_CONFIGr;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_gddr6_channel_interrupt_regs_set(
    int unit)
{
    int index_index;
    dnx_data_dram_gddr6_channel_interrupt_regs_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int table_index = dnx_data_dram_gddr6_table_channel_interrupt_regs;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 6;
    table->info_get.key_size[0] = 6;

    
    table->values[0].default_val = "INVALIDr";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_gddr6_channel_interrupt_regs_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_gddr6_table_channel_interrupt_regs");

    
    default_data = (dnx_data_dram_gddr6_channel_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->reg = INVALIDr;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dram_gddr6_channel_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->reg = DCC_INTERRUPT_REGISTERr;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->reg = DCC_ECC_INTERRUPT_REGISTERr;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->reg = DCC_DSI_INTERRUPT_REGISTERr;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->reg = DCC_RDR_INTERRUPT_REGISTERr;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->reg = DCC_PIPELINES_INTERRUPT_REGISTERr;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->reg = DCC_BIST_INTERRUPT_REGISTERr;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_gddr6_controller_interrupt_regs_set(
    int unit)
{
    int index_index;
    dnx_data_dram_gddr6_controller_interrupt_regs_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int table_index = dnx_data_dram_gddr6_table_controller_interrupt_regs;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "INVALIDr";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_gddr6_controller_interrupt_regs_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_gddr6_table_controller_interrupt_regs");

    
    default_data = (dnx_data_dram_gddr6_controller_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->reg = INVALIDr;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dram_gddr6_controller_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_controller_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->reg = DPC_INTERRUPT_REGISTERr;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_controller_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->reg = DPC_ECC_INTERRUPT_REGISTERr;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_gddr6_channel_debug_regs_set(
    int unit)
{
    int index_index;
    dnx_data_dram_gddr6_channel_debug_regs_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int table_index = dnx_data_dram_gddr6_table_channel_debug_regs;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 3;
    table->info_get.key_size[0] = 3;

    
    table->values[0].default_val = "INVALIDr";
    table->values[1].default_val = "NULL";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_gddr6_channel_debug_regs_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_gddr6_table_channel_debug_regs");

    
    default_data = (dnx_data_dram_gddr6_channel_debug_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->reg = INVALIDr;
    default_data->reassuring_str = NULL;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dram_gddr6_channel_debug_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_debug_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->reg = DCC_DRAM_ERR_COUNTERSr;
        data->reassuring_str = "DRAM error counters are cleared";
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_debug_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->reg = DCC_BIST_ERR_COUNTERSr;
        data->reassuring_str = "DRAM BIST error counters are cleared";
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_debug_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->reg = DCC_PIPELINES_DEBUGr;
        data->reassuring_str = "Pipeline debug info is cleared";
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_gddr6_channel_counter_regs_set(
    int unit)
{
    int index_index;
    dnx_data_dram_gddr6_channel_counter_regs_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int table_index = dnx_data_dram_gddr6_table_channel_counter_regs;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "INVALIDr";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_gddr6_channel_counter_regs_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_gddr6_table_channel_counter_regs");

    
    default_data = (dnx_data_dram_gddr6_channel_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->reg = INVALIDr;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dram_gddr6_channel_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->reg = DCC_TSM_DEBUGr;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->reg = DCC_DRAM_COMMAND_COUNTERSr;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_gddr6_channel_type_regs_set(
    int unit)
{
    int index_index;
    dnx_data_dram_gddr6_channel_type_regs_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int table_index = dnx_data_dram_gddr6_table_channel_type_regs;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "INVALIDr";
    table->values[1].default_val = "DNX_HBMC_NOF_DIAG_REG_TYPES";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_gddr6_channel_type_regs_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_gddr6_table_channel_type_regs");

    
    default_data = (dnx_data_dram_gddr6_channel_type_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->reg = INVALIDr;
    default_data->type = DNX_HBMC_NOF_DIAG_REG_TYPES;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dram_gddr6_channel_type_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_type_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->reg = DCC_WPBQ_STATUSr;
        data->type = DNX_HBMC_DIAG_REG_HISTOGRAM_32_8;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_channel_type_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->reg = DCC_RPBQ_STATUSr;
        data->type = DNX_HBMC_DIAG_REG_HISTOGRAM_32_8;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_gddr6_controller_info_regs_set(
    int unit)
{
    int index_index;
    dnx_data_dram_gddr6_controller_info_regs_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int table_index = dnx_data_dram_gddr6_table_controller_info_regs;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 4;
    table->info_get.key_size[0] = 4;

    
    table->values[0].default_val = "INVALIDr";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_gddr6_controller_info_regs_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_gddr6_table_controller_info_regs");

    
    default_data = (dnx_data_dram_gddr6_controller_info_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->reg = INVALIDr;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dram_gddr6_controller_info_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_controller_info_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->reg = DPC_DRAM_PLL_STATUSr;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_controller_info_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->reg = DCC_WDB_STATUSr;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_controller_info_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->reg = DCC_BIST_STATUSr;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_controller_info_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->reg = DCC_DYNAMIC_CALIBRATION_STATUSr;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_gddr6_master_phy_set(
    int unit)
{
    int dram_index_index;
    dnx_data_dram_gddr6_master_phy_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int table_index = dnx_data_dram_gddr6_table_master_phy;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_dram.general_info.max_nof_drams_get(unit);
    table->info_get.key_size[0] = dnx_data_dram.general_info.max_nof_drams_get(unit);

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_gddr6_master_phy_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_gddr6_table_master_phy");

    
    default_data = (dnx_data_dram_gddr6_master_phy_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->master_dram_index = 0;
    
    for (dram_index_index = 0; dram_index_index < table->keys[0].size; dram_index_index++)
    {
        data = (dnx_data_dram_gddr6_master_phy_t *) dnxc_data_mgmt_table_data_get(unit, table, dram_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_master_phy_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->master_dram_index = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_master_phy_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->master_dram_index = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_gddr6_ca_system_config_fields_set(
    int unit)
{
    int index_index;
    dnx_data_dram_gddr6_ca_system_config_fields_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int table_index = dnx_data_dram_gddr6_table_ca_system_config_fields;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 3;
    table->info_get.key_size[0] = 3;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "-1";
    table->values[2].default_val = "-1";
    table->values[3].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_gddr6_ca_system_config_fields_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_gddr6_table_ca_system_config_fields");

    
    default_data = (dnx_data_dram_gddr6_ca_system_config_fields_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->ch_a_start_bit = -1;
    default_data->ch_b_start_bit = -1;
    default_data->field_val = -1;
    default_data->nof_bits = -1;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dram_gddr6_ca_system_config_fields_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_ca_system_config_fields_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->ch_a_start_bit = 0;
        data->ch_b_start_bit = 2;
        data->field_val = dnx_data_dram.gddr6.ca_termination_conf_val_get(unit);
        data->nof_bits = 2;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_ca_system_config_fields_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->ch_a_start_bit = 4;
        data->ch_b_start_bit = 4;
        data->field_val = dnx_data_dram.gddr6.ck_termination_ca_conf_val_get(unit);
        data->nof_bits = 2;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_dram_gddr6_ca_system_config_fields_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->ch_a_start_bit = 6;
        data->ch_b_start_bit = 6;
        data->field_val = 0x1;
        data->nof_bits = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
q2a_a0_dnx_data_dram_general_info_is_temperature_reading_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int feature_index = dnx_data_dram_general_info_is_temperature_reading_supported;
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
q2a_a0_dnx_data_dram_general_info_max_nof_drams_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_max_nof_drams;
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
q2a_a0_dnx_data_dram_general_info_nof_channels_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_nof_channels;
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
q2a_a0_dnx_data_dram_general_info_mr_mask_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_mr_mask;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xfff;

    
    define->data = 0xfff;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_general_info_nof_mrs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_nof_mrs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_general_info_phy_address_mask_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_phy_address_mask;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xfff;

    
    define->data = 0xfff;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_general_info_frequency_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_frequency;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 850;

    
    define->data = 850;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_EXT_RAM_FREQ;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 500;
    define->property.range_max = 1400;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_general_info_buffer_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_buffer_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4096;

    
    define->data = 4096;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_general_info_command_address_parity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_command_address_parity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_EXT_RAM_COMMAND_ADDRESS_PARITY;
    define->property.doc = 
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
q2a_a0_dnx_data_dram_general_info_dq_write_parity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_dq_write_parity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "ext_ram_dq_write_parity";
    define->property.doc = 
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
q2a_a0_dnx_data_dram_general_info_dq_read_parity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_dq_read_parity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "ext_ram_dq_read_parity";
    define->property.doc = 
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
q2a_a0_dnx_data_dram_general_info_dbi_read_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_dbi_read;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_EXT_RAM_READ_DBI;
    define->property.doc = 
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
q2a_a0_dnx_data_dram_general_info_dbi_write_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_dbi_write;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_EXT_RAM_WRITE_DBI;
    define->property.doc = 
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
q2a_a0_dnx_data_dram_general_info_write_latency_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_write_latency;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = -1;

    
    define->data = -1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "ext_ram_write_latency";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_custom;
    define->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnx_data_property_dram_general_info_write_latency_read(unit, (uint32 *) &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_general_info_read_latency_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_read_latency;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = -1;

    
    define->data = -1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "ext_ram_read_latency";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_custom;
    define->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnx_data_property_dram_general_info_read_latency_read(unit, (uint32 *) &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_general_info_parity_latency_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_parity_latency;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_EXT_RAM_PARITY_LATENCY;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 3;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_general_info_tune_mode_on_init_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_tune_mode_on_init;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DNX_GDDR6_RESTORE_TUNE_PARAMETERS_OR_RUN_TUNE;

    
    define->data = DNX_GDDR6_RESTORE_TUNE_PARAMETERS_OR_RUN_TUNE;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_DRAM_PHY_TUNE_MODE_ON_INIT;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 4;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "RESTORE_TUNE_PARAMETERS_FROM_SOC_PROPERTIES";
    define->property.mapping[0].val = DNX_GDDR6_RESTORE_TUNE_PARAMETERS;
    define->property.mapping[1].name = "RUN_TUNE";
    define->property.mapping[1].val = DNX_GDDR6_RUN_TUNE;
    define->property.mapping[2].name = "RESTORE_TUNE_PARAMETERS_OR_RUN_TUNE";
    define->property.mapping[2].val = DNX_GDDR6_RESTORE_TUNE_PARAMETERS_OR_RUN_TUNE;
    define->property.mapping[2].is_default = 1 ;
    define->property.mapping[3].name = "SKIP_TUNE";
    define->property.mapping[3].val = DNX_GDDR6_SKIP_TUNE;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_general_info_command_parity_latency_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_command_parity_latency;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5;

    
    define->data = 5;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_EXT_RAM_CMD_PAR_LATENCY;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 8;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_general_info_crc_write_latency_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_crc_write_latency;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = -1;

    
    define->data = -1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "ext_ram_t_crc_wr_latency";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_custom;
    define->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnx_data_property_dram_general_info_crc_write_latency_read(unit, (uint32 *) &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_general_info_crc_read_latency_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_crc_read_latency;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = -1;

    
    define->data = -1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "ext_ram_t_crc_rd_latency";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_custom;
    define->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnx_data_property_dram_general_info_crc_read_latency_read(unit, (uint32 *) &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_general_info_crc_write_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_crc_write;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_EXT_RAM_WRITE_CRC;
    define->property.doc = 
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
q2a_a0_dnx_data_dram_general_info_crc_read_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_crc_read;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_EXT_RAM_READ_CRC;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 1;
    define->property.range_max = 1;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_general_info_device_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_device_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "ext_ram_device_size";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 64;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_general_info_supported_dram_bitmap_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_supported_dram_bitmap;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x3;

    
    define->data = 0x3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_general_info_tertw_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_tertw;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = -1;

    
    define->data = -1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "ext_ram_t_e_rtw";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_custom;
    define->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnx_data_property_dram_general_info_tertw_read(unit, (uint32 *) &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
q2a_a0_dnx_data_dram_general_info_mr_defaults_set(
    int unit)
{
    int index_index;
    dnx_data_dram_general_info_mr_defaults_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int table_index = dnx_data_dram_general_info_table_mr_defaults;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_dram.general_info.nof_mrs_get(unit);
    table->info_get.key_size[0] = dnx_data_dram.general_info.nof_mrs_get(unit);

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_general_info_mr_defaults_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_general_info_table_mr_defaults");

    
    default_data = (dnx_data_dram_general_info_mr_defaults_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->value = 0;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dram_general_info_mr_defaults_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dram_general_info_mr_defaults_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->value = 0xF7F;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_general_info_mr_defaults_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->value = 0x300;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_dram_general_info_mr_defaults_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->value = 0x800;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_dram_general_info_mr_defaults_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->value = 0x1DF;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_dram_general_info_mr_defaults_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->value = 0x3C0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_general_info_dram_info_set(
    int unit)
{
    dnx_data_dram_general_info_dram_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int table_index = dnx_data_dram_general_info_table_dram_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "SHMOO_G6PHY16_DRAM_TYPE_GDDR6";
    table->values[1].default_val = "DATA(dram,general_info,dram_bitmap_internal)";
    table->values[2].default_val = "64";
    table->values[3].default_val = "16384";
    table->values[4].default_val = "16";
    table->values[5].default_val = "DATA(dram, general_info, frequency) * 8";
    table->values[6].default_val = "100";
    table->values[7].default_val = "SHMOO_G6PHY16_CTL_TYPE_1";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_general_info_dram_info_t, (1 + 1  ), "data of dnx_data_dram_general_info_table_dram_info");

    
    default_data = (dnx_data_dram_general_info_dram_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dram_type = SHMOO_G6PHY16_DRAM_TYPE_GDDR6;
    default_data->dram_bitmap = dnx_data_dram.general_info.dram_bitmap_internal_get(unit);
    default_data->nof_columns = 64;
    default_data->nof_rows = 16384;
    default_data->nof_banks = 16;
    default_data->data_rate = dnx_data_dram.general_info.frequency_get(unit) * 8;
    default_data->ref_clock = 100;
    default_data->ctl_type = SHMOO_G6PHY16_CTL_TYPE_1;
    
    data = (dnx_data_dram_general_info_dram_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->values[2].property.name = spn_EXT_RAM_COLUMNS;
    table->values[2].property.doc =
        "\n"
    ;
    table->values[2].property.method = dnxc_data_property_method_range;
    table->values[2].property.method_str = "range";
    table->values[2].property.range_min = 0;
    table->values[2].property.range_max = 65536;
    
    table->values[3].property.name = spn_EXT_RAM_ROWS;
    table->values[3].property.doc =
        "\n"
    ;
    table->values[3].property.method = dnxc_data_property_method_range;
    table->values[3].property.method_str = "range";
    table->values[3].property.range_min = 0;
    table->values[3].property.range_max = 1048576;
    
    table->values[7].property.name = "ext_ram_ctl_type";
    table->values[7].property.doc =
        "\n"
    ;
    table->values[7].property.method = dnxc_data_property_method_direct_map;
    table->values[7].property.method_str = "direct_map";
    table->values[7].property.nof_mapping = 2;
    DNXC_DATA_ALLOC(table->values[7].property.mapping, dnxc_data_property_map_t, table->values[7].property.nof_mapping, "dnx_data_dram_general_info_dram_info_t property mapping");

    table->values[7].property.mapping[0].name = "SHMOO_G6PHY16_CTL_TYPE_1";
    table->values[7].property.mapping[0].val = SHMOO_G6PHY16_CTL_TYPE_1;
    table->values[7].property.mapping[0].is_default = 1 ;
    table->values[7].property.mapping[1].name = "SHMOO_G6PHY16_CTL_TYPE_RSVP";
    table->values[7].property.mapping[1].val = SHMOO_G6PHY16_CTL_TYPE_RSVP;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnx_data_dram_general_info_dram_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[2].property, -1, &data->nof_columns));
    data = (dnx_data_dram_general_info_dram_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[3].property, -1, &data->nof_rows));
    data = (dnx_data_dram_general_info_dram_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[7].property, -1, &data->ctl_type));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_general_info_timing_params_set(
    int unit)
{
    dnx_data_dram_general_info_timing_params_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int table_index = dnx_data_dram_general_info_table_timing_params;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "-1";
    table->values[2].default_val = "-1";
    table->values[3].default_val = "-1";
    table->values[4].default_val = "-1";
    table->values[5].default_val = "-1";
    table->values[6].default_val = "-1";
    table->values[7].default_val = "-1";
    table->values[8].default_val = "-1";
    table->values[9].default_val = "-1";
    table->values[10].default_val = "-1";
    table->values[11].default_val = "-1";
    table->values[12].default_val = "-1";
    table->values[13].default_val = "-1";
    table->values[14].default_val = "-1";
    table->values[15].default_val = "-1";
    table->values[16].default_val = "-1";
    table->values[17].default_val = "-1";
    table->values[18].default_val = "12";
    table->values[19].default_val = "63";
    table->values[20].default_val = "-1";
    table->values[21].default_val = "-1";
    table->values[22].default_val = "10";
    table->values[23].default_val = "-1";
    table->values[24].default_val = "-1";
    table->values[25].default_val = "-1";
    table->values[26].default_val = "-1";
    table->values[27].default_val = "-1";
    table->values[28].default_val = "-1";
    table->values[29].default_val = "-1";
    table->values[30].default_val = "-1";
    table->values[31].default_val = "-1";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_general_info_timing_params_t, (1 + 1  ), "data of dnx_data_dram_general_info_table_timing_params");

    
    default_data = (dnx_data_dram_general_info_timing_params_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->twr = -1;
    default_data->trp = -1;
    default_data->trtps = -1;
    default_data->trtpl = -1;
    default_data->trrds = -1;
    default_data->trrdl = -1;
    default_data->tfaw = -1;
    default_data->trcdwr = -1;
    default_data->trcdrd = -1;
    default_data->tras = -1;
    default_data->tras_hbm2e = -1;
    default_data->trc = -1;
    default_data->twtrl = -1;
    default_data->twtrs = -1;
    default_data->trtw = -1;
    default_data->tccdr = -1;
    default_data->tccds = -1;
    default_data->tccdl = -1;
    default_data->trrefd = 12;
    default_data->trfcsb = 63;
    default_data->trfc = -1;
    default_data->tmrd = -1;
    default_data->tmod = 10;
    default_data->twtrtr = -1;
    default_data->twrwtr = -1;
    default_data->treftr = -1;
    default_data->trdtlt = -1;
    default_data->trcdwtr = -1;
    default_data->trcdrtr = -1;
    default_data->trcdltr = -1;
    default_data->tltrtr = -1;
    default_data->tltltr = -1;
    
    data = (dnx_data_dram_general_info_timing_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->property.name = "ext_ram_t_wr, ext_ram_t_rp, ext_ram_t_rtp_s, ext_ram_t_rtp_l, ext_ram_t_rrd_s, ext_ram_t_rrd_l, ext_ram_t_faw, ext_ram_t_rcd_wr, ext_ram_t_rcd_rd, ext_ram_t_ras, ext_ram_t_rc, ext_ram_t_wtr_l, ext_ram_t_wtr_s, ext_ram_t_rtw, ext_ram_t_e_rtw, ext_ram_t_ccd_s, ext_ram_t_ccd_l,ext_ram_t_rrefd, ext_ram_t_rfcsb, ext_ram_t_rfc, ext_ram_t_mod, ext_ram_t_wtrtr, ext_ram_t_wrwtr, ext_ram_t_reftr, ext_ram_t_rdtlt, ext_ram_t_rcdwtr, ext_ram_t_rcdrtr, ext_ram_t_rcdltr, ext_ram_t_ltrtr, ext_ram_t_ltltr";
    table->property.doc =
        "\n"
    ;
    table->property.method = dnxc_data_property_method_custom;
    table->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnx_data_dram_general_info_timing_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnx_data_property_dram_general_info_timing_params_read(unit, data));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
q2a_a0_dnx_data_dram_address_translation_matrix_column_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_address_translation;
    int define_index = dnx_data_dram_address_translation_define_matrix_column_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 30;

    
    define->data = 30;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_address_translation_physical_address_transaction_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_address_translation;
    int define_index = dnx_data_dram_address_translation_define_physical_address_transaction_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_address_translation_nof_atms_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_address_translation;
    int define_index = dnx_data_dram_address_translation_define_nof_atms;
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
q2a_a0_dnx_data_dram_address_translation_matrix_configuration_set(
    int unit)
{
    int matrix_option_index;
    dnx_data_dram_address_translation_matrix_configuration_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_address_translation;
    int table_index = dnx_data_dram_address_translation_table_matrix_configuration;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_dram.address_translation.nof_atms_get(unit);
    table->info_get.key_size[0] = dnx_data_dram.address_translation.nof_atms_get(unit);

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_address_translation_matrix_configuration_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_address_translation_table_matrix_configuration");

    
    default_data = (dnx_data_dram_address_translation_matrix_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->logical_to_physical[0] = 0;
    
    for (matrix_option_index = 0; matrix_option_index < table->keys[0].size; matrix_option_index++)
    {
        data = (dnx_data_dram_address_translation_matrix_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, matrix_option_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_GDDR6_SINGLE_DRAM_ATM < table->keys[0].size)
    {
        data = (dnx_data_dram_address_translation_matrix_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_GDDR6_SINGLE_DRAM_ATM, 0);
        SHR_RANGE_VERIFY(30, 0, DNX_DATA_MAX_DRAM_ADDRESS_TRANSLATION_MATRIX_COLUMN_SIZE, _SHR_E_INTERNAL, "out of bound access to array")
        data->logical_to_physical[0] = 0x800000;
        data->logical_to_physical[1] = 0x21;
        data->logical_to_physical[2] = 0x800000;
        data->logical_to_physical[3] = 0x4;
        data->logical_to_physical[4] = 0x4008;
        data->logical_to_physical[5] = 0x41;
        data->logical_to_physical[6] = 0x8008;
        data->logical_to_physical[7] = 0x10004;
        data->logical_to_physical[8] = 0x20002;
        data->logical_to_physical[9] = 0x11;
        data->logical_to_physical[10] = 0x80;
        data->logical_to_physical[11] = 0x100;
        data->logical_to_physical[12] = 0x200;
        data->logical_to_physical[13] = 0x400;
        data->logical_to_physical[14] = 0x800;
        data->logical_to_physical[15] = 0x1000;
        data->logical_to_physical[16] = 0x2000;
        data->logical_to_physical[17] = 0x1;
        data->logical_to_physical[18] = 0x2;
        data->logical_to_physical[19] = 0x400000;
        data->logical_to_physical[20] = 0x8;
        data->logical_to_physical[21] = 0;
        data->logical_to_physical[22] = 0;
        data->logical_to_physical[23] = 0;
        data->logical_to_physical[24] = 0;
        data->logical_to_physical[25] = 0;
        data->logical_to_physical[26] = 0x80000;
        data->logical_to_physical[27] = 0x200000;
        data->logical_to_physical[28] = 0x40000;
        data->logical_to_physical[29] = 0;
    }
    if (DNX_GDDR6_2_DRAMS_ATM < table->keys[0].size)
    {
        data = (dnx_data_dram_address_translation_matrix_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_GDDR6_2_DRAMS_ATM, 0);
        SHR_RANGE_VERIFY(30, 0, DNX_DATA_MAX_DRAM_ADDRESS_TRANSLATION_MATRIX_COLUMN_SIZE, _SHR_E_INTERNAL, "out of bound access to array")
        data->logical_to_physical[0] = 0x800000;
        data->logical_to_physical[1] = 0x21;
        data->logical_to_physical[2] = 0x200002;
        data->logical_to_physical[3] = 0x400004;
        data->logical_to_physical[4] = 0x4008;
        data->logical_to_physical[5] = 0x41;
        data->logical_to_physical[6] = 0x8008;
        data->logical_to_physical[7] = 0x10004;
        data->logical_to_physical[8] = 0x20002;
        data->logical_to_physical[9] = 0x11;
        data->logical_to_physical[10] = 0x80;
        data->logical_to_physical[11] = 0x100;
        data->logical_to_physical[12] = 0x200;
        data->logical_to_physical[13] = 0x400;
        data->logical_to_physical[14] = 0x800;
        data->logical_to_physical[15] = 0x1000;
        data->logical_to_physical[16] = 0x2000;
        data->logical_to_physical[17] = 0x1;
        data->logical_to_physical[18] = 0x2;
        data->logical_to_physical[19] = 0x4;
        data->logical_to_physical[20] = 0x8;
        data->logical_to_physical[21] = 0;
        data->logical_to_physical[22] = 0;
        data->logical_to_physical[23] = 0;
        data->logical_to_physical[24] = 0;
        data->logical_to_physical[25] = 0;
        data->logical_to_physical[26] = 0x80000;
        data->logical_to_physical[27] = 0x100000;
        data->logical_to_physical[28] = 0x40000;
        data->logical_to_physical[29] = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_address_translation_interrupt_regs_set(
    int unit)
{
    int index_index;
    dnx_data_dram_address_translation_interrupt_regs_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_address_translation;
    int table_index = dnx_data_dram_address_translation_table_interrupt_regs;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "INVALIDr";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_address_translation_interrupt_regs_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_address_translation_table_interrupt_regs");

    
    default_data = (dnx_data_dram_address_translation_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->reg = INVALIDr;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dram_address_translation_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dram_address_translation_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->reg = TDU_INTERRUPT_REGISTERr;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_address_translation_interrupt_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->reg = TDU_ECC_INTERRUPT_REGISTERr;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_address_translation_counter_regs_set(
    int unit)
{
    int index_index;
    dnx_data_dram_address_translation_counter_regs_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_address_translation;
    int table_index = dnx_data_dram_address_translation_table_counter_regs;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 10;
    table->info_get.key_size[0] = 10;

    
    table->values[0].default_val = "INVALIDr";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_address_translation_counter_regs_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_address_translation_table_counter_regs");

    
    default_data = (dnx_data_dram_address_translation_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->reg = INVALIDr;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dram_address_translation_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dram_address_translation_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->reg = TDU_WRITE_REQUEST_PATH_COUNTERSr;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dram_address_translation_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->reg = TDU_WRITE_REQUEST_PER_CHANNEL_COUNTr;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_dram_address_translation_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->reg = TDU_WRITE_REQUEST_PER_BANK_COUNTr;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_dram_address_translation_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->reg = TDU_WRITE_REQUEST_PER_LOGICAL_ADDRESS_BIT_COUNTr;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_dram_address_translation_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->reg = TDU_READ_REQUEST_PATH_COUNTERSr;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_dram_address_translation_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->reg = TDU_READ_REQUEST_PER_CHANNEL_COUNTr;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_dram_address_translation_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->reg = TDU_READ_REQUEST_PER_BANK_COUNTr;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_dram_address_translation_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->reg = TDU_READ_REQUEST_PER_LOGICAL_ADDRESS_BIT_COUNTr;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_dram_address_translation_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->reg = TDU_READ_DATA_PATH_COUNTERSr;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_dram_address_translation_counter_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->reg = TDU_DRAM_BLOCKED_STATUSr;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
q2a_a0_dnx_data_dram_buffers_quarantine_buffers_if_tdu_corrected_errors_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_buffers;
    int feature_index = dnx_data_dram_buffers_quarantine_buffers_if_tdu_corrected_errors;
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
q2a_a0_dnx_data_dram_buffers_quarantine_buffers_if_tdu_errors_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_buffers;
    int feature_index = dnx_data_dram_buffers_quarantine_buffers_if_tdu_errors;
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
q2a_a0_dnx_data_dram_buffers_nof_bdbs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_buffers;
    int define_index = dnx_data_dram_buffers_define_nof_bdbs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64*1024;

    
    define->data = 64*1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
q2a_a0_dnx_data_dram_dram_block_leaky_bucket_window_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dram_block;
    int define_index = dnx_data_dram_dram_block_define_leaky_bucket_window_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4096;

    
    define->data = 4096;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
q2a_a0_dnx_data_dram_dram_block_dram_block_params_set(
    int unit)
{
    int mechanism_index;
    dnx_data_dram_dram_block_dram_block_params_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dram_block;
    int table_index = dnx_data_dram_dram_block_table_dram_block_params;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_DRAM_BLOCK_MECHANISM_NOF;
    table->info_get.key_size[0] = DNX_DRAM_BLOCK_MECHANISM_NOF;

    
    table->values[0].default_val = "FALSE";
    table->values[1].default_val = "TRUE";
    table->values[2].default_val = "FALSE";
    table->values[3].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[4].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[5].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[6].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[7].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[8].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[9].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[10].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[11].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[12].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[13].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[14].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[15].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[16].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[17].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[18].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[19].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[20].default_val = "TRUE";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_dram_block_dram_block_params_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_dram_block_table_dram_block_params");

    
    default_data = (dnx_data_dram_dram_block_dram_block_params_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->is_valid = FALSE;
    default_data->is_used = TRUE;
    default_data->is_disable_supported = FALSE;
    default_data->dbal_table = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->increment_threshold_0 = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->increment_threshold_1 = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->increment_threshold_2 = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->decrement_threshold_0 = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->decrement_threshold_1 = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->decrement_threshold_2 = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->increment_size_0 = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->increment_size_1 = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->increment_size_2 = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->decrement_size_0 = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->decrement_size_1 = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->decrement_size_2 = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->bucket_full_size = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->bucket_assert_threshold = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->bucket_deassert_threshold = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->log2_window_size = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->reset_on_deassert = TRUE;
    
    for (mechanism_index = 0; mechanism_index < table->keys[0].size; mechanism_index++)
    {
        data = (dnx_data_dram_dram_block_dram_block_params_t *) dnxc_data_mgmt_table_data_get(unit, table, mechanism_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_DRAM_BLOCK_MECHANISM_AVERAGE_READ_INFLIGHTS < table->keys[0].size)
    {
        data = (dnx_data_dram_dram_block_dram_block_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_DRAM_BLOCK_MECHANISM_AVERAGE_READ_INFLIGHTS, 0);
        data->is_valid = TRUE;
        data->dbal_table = DBAL_TABLE_DRAM_COMMON_TDU_DRAM_BLOCK_AVERAGE_READ_INFLIGHTS_LEAKY_BUCKET_CONFIG;
        data->increment_threshold_0 = 450;
        data->increment_threshold_1 = 500;
        data->increment_threshold_2 = 550;
        data->decrement_threshold_0 = 450;
        data->decrement_threshold_1 = 400;
        data->decrement_threshold_2 = 350;
        data->increment_size_0 = 2;
        data->increment_size_1 = 4;
        data->increment_size_2 = 8;
        data->decrement_size_0 = 1;
        data->decrement_size_1 = 2;
        data->decrement_size_2 = 4;
        data->bucket_full_size = 40;
        data->bucket_assert_threshold = 24;
        data->bucket_deassert_threshold = 4;
        data->reset_on_deassert = FALSE;
    }
    if (DNX_DRAM_BLOCK_MECHANISM_WRITE_MINUS_READ < table->keys[0].size)
    {
        data = (dnx_data_dram_dram_block_dram_block_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_DRAM_BLOCK_MECHANISM_WRITE_MINUS_READ, 0);
        data->is_valid = TRUE;
        data->is_used = FALSE;
        data->is_disable_supported = TRUE;
        data->dbal_table = DBAL_TABLE_DRAM_COMMON_TDU_DRAM_BLOCK_WRITE_MINUS_READ_LEAKY_BUCKET_CONFIG;
        data->increment_threshold_0 = 0;
        data->increment_threshold_1 = 2000;
        data->increment_threshold_2 = 4000;
        data->decrement_threshold_0 = 0;
        data->decrement_threshold_1 = 5 * dnx_data_dram.dram_block.wmr_decrement_thr_factor_get(unit) / 100;
        data->decrement_threshold_2 = 10 * dnx_data_dram.dram_block.wmr_decrement_thr_factor_get(unit) / 100;
        data->increment_size_0 = 1;
        data->increment_size_1 = 2;
        data->increment_size_2 = 4;
        data->decrement_size_0 = 0;
        data->decrement_size_1 = 0;
        data->decrement_size_2 = 0;
        data->bucket_full_size = 50;
        data->bucket_assert_threshold = 0;
        data->bucket_deassert_threshold = 0;
        data->reset_on_deassert = TRUE;
    }
    if (DNX_DRAM_BLOCK_MECHANISM_WRITE_PLUS_READ < table->keys[0].size)
    {
        data = (dnx_data_dram_dram_block_dram_block_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_DRAM_BLOCK_MECHANISM_WRITE_PLUS_READ, 0);
        data->is_valid = TRUE;
        data->is_used = FALSE;
        data->is_disable_supported = TRUE;
        data->dbal_table = DBAL_TABLE_DRAM_COMMON_TDU_DRAM_BLOCK_WRITE_PLUS_READ_LEAKY_BUCKET_CONFIG;
        data->increment_threshold_0 = 91 * dnx_data_dram.dram_block.wpr_increment_thr_factor_get(unit) / 100;
        data->increment_threshold_1 = 93 * dnx_data_dram.dram_block.wpr_increment_thr_factor_get(unit) / 100;
        data->increment_threshold_2 = 95 * dnx_data_dram.dram_block.wpr_increment_thr_factor_get(unit) / 100;
        data->increment_size_0 = 2;
        data->increment_size_1 = 3;
        data->increment_size_2 = 4;
        data->decrement_size_0 = 1;
        data->bucket_full_size = 50;
        data->bucket_assert_threshold = 16;
        data->bucket_deassert_threshold = 4;
        data->reset_on_deassert = TRUE;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
q2a_a0_dnx_data_dram_dbal_average_read_inflights_increment_threshold_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dbal;
    int define_index = dnx_data_dram_dbal_define_average_read_inflights_increment_threshold_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 20;

    
    define->data = 20;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_dbal_average_read_inflights_decrement_threshold_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dbal;
    int define_index = dnx_data_dram_dbal_define_average_read_inflights_decrement_threshold_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 20;

    
    define->data = 20;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_dram_dbal_dram_bist_mode_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dbal;
    int define_index = dnx_data_dram_dbal_define_dram_bist_mode_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}








static shr_error_e
q2a_a0_dnx_data_dram_ctests_rate_set(
    int unit)
{
    dnx_data_dram_ctests_rate_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_ctests;
    int table_index = dnx_data_dram_ctests_table_rate;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "CCGE";
    table->values[1].default_val = "8";
    table->values[2].default_val = "94000";
    table->values[3].default_val = "210000";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_ctests_rate_t, (1 + 1  ), "data of dnx_data_dram_ctests_table_rate");

    
    default_data = (dnx_data_dram_ctests_rate_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->snake_ports_type = "CCGE";
    default_data->nof_lanes_in_snake = 8;
    default_data->expected_rate_in_mbps = 94000;
    default_data->expected_dram_rate_in_mbps = 210000;
    
    data = (dnx_data_dram_ctests_rate_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
q2a_a0_data_dram_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_dram;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_dram_submodule_hbm;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_dram_hbm_is_supported;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_dram_hbm_is_supported_set;
    data_index = dnx_data_dram_hbm_is_delete_bdb_supported;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_dram_hbm_is_delete_bdb_supported_set;

    
    
    submodule_index = dnx_data_dram_submodule_gddr6;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dram_gddr6_define_nof_ca_bits;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_gddr6_nof_ca_bits_set;
    data_index = dnx_data_dram_gddr6_define_bytes_per_channel;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_gddr6_bytes_per_channel_set;
    data_index = dnx_data_dram_gddr6_define_training_fifo_depth;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_gddr6_training_fifo_depth_set;
    data_index = dnx_data_dram_gddr6_define_refresh_mechanism_enable;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_gddr6_refresh_mechanism_enable_set;
    data_index = dnx_data_dram_gddr6_define_bist_enable;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_gddr6_bist_enable_set;
    data_index = dnx_data_dram_gddr6_define_dynamic_calibration_enable;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_gddr6_dynamic_calibration_enable_set;
    data_index = dnx_data_dram_gddr6_define_cdr_enable;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_gddr6_cdr_enable_set;
    data_index = dnx_data_dram_gddr6_define_write_recovery;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_gddr6_write_recovery_set;
    data_index = dnx_data_dram_gddr6_define_cabi;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_gddr6_cabi_set;
    data_index = dnx_data_dram_gddr6_define_dram_mode;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_gddr6_dram_mode_set;
    data_index = dnx_data_dram_gddr6_define_command_pipe_extra_delay;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_gddr6_command_pipe_extra_delay_set;
    data_index = dnx_data_dram_gddr6_define_use_11bits_ca;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_gddr6_use_11bits_ca_set;
    data_index = dnx_data_dram_gddr6_define_ck_odt;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_gddr6_ck_odt_set;
    data_index = dnx_data_dram_gddr6_define_dynamic_calibration_period;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_gddr6_dynamic_calibration_period_set;
    data_index = dnx_data_dram_gddr6_define_ck_termination_mr_conf_val;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_gddr6_ck_termination_mr_conf_val_set;
    data_index = dnx_data_dram_gddr6_define_ck_termination_ca_conf_val;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_gddr6_ck_termination_ca_conf_val_set;
    data_index = dnx_data_dram_gddr6_define_wck_granularity;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_gddr6_wck_granularity_set;
    data_index = dnx_data_dram_gddr6_define_ref_clk_bitmap;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_gddr6_ref_clk_bitmap_set;
    data_index = dnx_data_dram_gddr6_define_ca_termination_conf_val;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_gddr6_ca_termination_conf_val_set;

    
    data_index = dnx_data_dram_gddr6_is_supported;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_dram_gddr6_is_supported_set;
    data_index = dnx_data_dram_gddr6_allow_disable_read_crc;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_dram_gddr6_allow_disable_read_crc_set;
    data_index = dnx_data_dram_gddr6_interleaved_refresh_cycles;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_dram_gddr6_interleaved_refresh_cycles_set;
    data_index = dnx_data_dram_gddr6_enable_temp_read_samsung;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_dram_gddr6_enable_temp_read_samsung_set;
    data_index = dnx_data_dram_gddr6_enable_temp_read_micron;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_dram_gddr6_enable_temp_read_micron_set;

    
    data_index = dnx_data_dram_gddr6_table_refresh_intervals;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_dram_gddr6_refresh_intervals_set;
    data_index = dnx_data_dram_gddr6_table_dq_map;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_dram_gddr6_dq_map_set;
    data_index = dnx_data_dram_gddr6_table_dq_channel_swap;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_dram_gddr6_dq_channel_swap_set;
    data_index = dnx_data_dram_gddr6_table_dq_byte_map;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_dram_gddr6_dq_byte_map_set;
    data_index = dnx_data_dram_gddr6_table_ca_map;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_dram_gddr6_ca_map_set;
    data_index = dnx_data_dram_gddr6_table_cadt_byte_map;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_dram_gddr6_cadt_byte_map_set;
    data_index = dnx_data_dram_gddr6_table_channel_regs;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_dram_gddr6_channel_regs_set;
    data_index = dnx_data_dram_gddr6_table_controller_regs;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_dram_gddr6_controller_regs_set;
    data_index = dnx_data_dram_gddr6_table_channel_interrupt_regs;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_dram_gddr6_channel_interrupt_regs_set;
    data_index = dnx_data_dram_gddr6_table_controller_interrupt_regs;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_dram_gddr6_controller_interrupt_regs_set;
    data_index = dnx_data_dram_gddr6_table_channel_debug_regs;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_dram_gddr6_channel_debug_regs_set;
    data_index = dnx_data_dram_gddr6_table_channel_counter_regs;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_dram_gddr6_channel_counter_regs_set;
    data_index = dnx_data_dram_gddr6_table_channel_type_regs;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_dram_gddr6_channel_type_regs_set;
    data_index = dnx_data_dram_gddr6_table_controller_info_regs;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_dram_gddr6_controller_info_regs_set;
    data_index = dnx_data_dram_gddr6_table_master_phy;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_dram_gddr6_master_phy_set;
    data_index = dnx_data_dram_gddr6_table_ca_system_config_fields;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_dram_gddr6_ca_system_config_fields_set;
    
    submodule_index = dnx_data_dram_submodule_general_info;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dram_general_info_define_max_nof_drams;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_general_info_max_nof_drams_set;
    data_index = dnx_data_dram_general_info_define_nof_channels;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_general_info_nof_channels_set;
    data_index = dnx_data_dram_general_info_define_mr_mask;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_general_info_mr_mask_set;
    data_index = dnx_data_dram_general_info_define_nof_mrs;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_general_info_nof_mrs_set;
    data_index = dnx_data_dram_general_info_define_phy_address_mask;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_general_info_phy_address_mask_set;
    data_index = dnx_data_dram_general_info_define_frequency;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_general_info_frequency_set;
    data_index = dnx_data_dram_general_info_define_buffer_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_general_info_buffer_size_set;
    data_index = dnx_data_dram_general_info_define_command_address_parity;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_general_info_command_address_parity_set;
    data_index = dnx_data_dram_general_info_define_dq_write_parity;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_general_info_dq_write_parity_set;
    data_index = dnx_data_dram_general_info_define_dq_read_parity;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_general_info_dq_read_parity_set;
    data_index = dnx_data_dram_general_info_define_dbi_read;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_general_info_dbi_read_set;
    data_index = dnx_data_dram_general_info_define_dbi_write;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_general_info_dbi_write_set;
    data_index = dnx_data_dram_general_info_define_write_latency;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_general_info_write_latency_set;
    data_index = dnx_data_dram_general_info_define_read_latency;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_general_info_read_latency_set;
    data_index = dnx_data_dram_general_info_define_parity_latency;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_general_info_parity_latency_set;
    data_index = dnx_data_dram_general_info_define_tune_mode_on_init;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_general_info_tune_mode_on_init_set;
    data_index = dnx_data_dram_general_info_define_command_parity_latency;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_general_info_command_parity_latency_set;
    data_index = dnx_data_dram_general_info_define_crc_write_latency;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_general_info_crc_write_latency_set;
    data_index = dnx_data_dram_general_info_define_crc_read_latency;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_general_info_crc_read_latency_set;
    data_index = dnx_data_dram_general_info_define_crc_write;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_general_info_crc_write_set;
    data_index = dnx_data_dram_general_info_define_crc_read;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_general_info_crc_read_set;
    data_index = dnx_data_dram_general_info_define_device_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_general_info_device_size_set;
    data_index = dnx_data_dram_general_info_define_supported_dram_bitmap;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_general_info_supported_dram_bitmap_set;
    data_index = dnx_data_dram_general_info_define_tertw;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_general_info_tertw_set;

    
    data_index = dnx_data_dram_general_info_is_temperature_reading_supported;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_dram_general_info_is_temperature_reading_supported_set;

    
    data_index = dnx_data_dram_general_info_table_mr_defaults;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_dram_general_info_mr_defaults_set;
    data_index = dnx_data_dram_general_info_table_dram_info;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_dram_general_info_dram_info_set;
    data_index = dnx_data_dram_general_info_table_timing_params;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_dram_general_info_timing_params_set;
    
    submodule_index = dnx_data_dram_submodule_address_translation;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dram_address_translation_define_matrix_column_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_address_translation_matrix_column_size_set;
    data_index = dnx_data_dram_address_translation_define_physical_address_transaction_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_address_translation_physical_address_transaction_size_set;
    data_index = dnx_data_dram_address_translation_define_nof_atms;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_address_translation_nof_atms_set;

    

    
    data_index = dnx_data_dram_address_translation_table_matrix_configuration;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_dram_address_translation_matrix_configuration_set;
    data_index = dnx_data_dram_address_translation_table_interrupt_regs;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_dram_address_translation_interrupt_regs_set;
    data_index = dnx_data_dram_address_translation_table_counter_regs;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_dram_address_translation_counter_regs_set;
    
    submodule_index = dnx_data_dram_submodule_buffers;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dram_buffers_define_nof_bdbs;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_buffers_nof_bdbs_set;

    
    data_index = dnx_data_dram_buffers_quarantine_buffers_if_tdu_corrected_errors;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_dram_buffers_quarantine_buffers_if_tdu_corrected_errors_set;
    data_index = dnx_data_dram_buffers_quarantine_buffers_if_tdu_errors;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_dram_buffers_quarantine_buffers_if_tdu_errors_set;

    
    
    submodule_index = dnx_data_dram_submodule_dram_block;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dram_dram_block_define_leaky_bucket_window_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_dram_block_leaky_bucket_window_size_set;

    

    
    data_index = dnx_data_dram_dram_block_table_dram_block_params;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_dram_dram_block_dram_block_params_set;
    
    submodule_index = dnx_data_dram_submodule_dbal;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dram_dbal_define_average_read_inflights_increment_threshold_nof_bits;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_dbal_average_read_inflights_increment_threshold_nof_bits_set;
    data_index = dnx_data_dram_dbal_define_average_read_inflights_decrement_threshold_nof_bits;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_dbal_average_read_inflights_decrement_threshold_nof_bits_set;
    data_index = dnx_data_dram_dbal_define_dram_bist_mode_nof_bits;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_dram_dbal_dram_bist_mode_nof_bits_set;

    

    
    
    submodule_index = dnx_data_dram_submodule_ctests;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_dram_ctests_table_rate;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_dram_ctests_rate_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

