

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_iqs.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>







static shr_error_e
jr2_a0_dnx_data_iqs_credit_watchdog_fixed_period_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_credit;
    int feature_index = dnx_data_iqs_credit_watchdog_fixed_period;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_CUSTOM_FEATURE;
    feature->property.doc = 
        "\n"
        "custom_feature_watchdog_fixed_period='0' or '1'\n"
        "if 1 watchdog has a fixed value, otherwise can be controlled by the user\n"
        "Default - '1'\n"
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_suffix_enable;
    feature->property.method_str = "suffix_enable";
    feature->property.suffix = "watchdog_fixed_period";
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_iqs_credit_fmq_shp_zero_rate_allowed_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_credit;
    int feature_index = dnx_data_iqs_credit_fmq_shp_zero_rate_allowed;
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
jr2_a0_dnx_data_iqs_credit_credit_balance_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_credit;
    int feature_index = dnx_data_iqs_credit_credit_balance_support;
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
jr2_a0_dnx_data_iqs_credit_nof_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_credit;
    int define_index = dnx_data_iqs_credit_define_nof_profiles;
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
jr2_a0_dnx_data_iqs_credit_max_credit_balance_threshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_credit;
    int define_index = dnx_data_iqs_credit_define_max_credit_balance_threshold;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x1ffff;

    
    define->data = 0x1ffff;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_iqs_credit_max_worth_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_credit;
    int define_index = dnx_data_iqs_credit_define_max_worth;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8191;

    
    define->data = 8191;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_iqs_credit_satisified_back_min_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_credit;
    int define_index = dnx_data_iqs_credit_define_satisified_back_min;
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
jr2_a0_dnx_data_iqs_credit_satisified_back_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_credit;
    int define_index = dnx_data_iqs_credit_define_satisified_back_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x3ffff;

    
    define->data = 0x3ffff;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_iqs_credit_satisified_empty_min_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_credit;
    int define_index = dnx_data_iqs_credit_define_satisified_empty_min;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = -1 * 0x1ffff;

    
    define->data = -1 * 0x1ffff;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_iqs_credit_satisified_empty_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_credit;
    int define_index = dnx_data_iqs_credit_define_satisified_empty_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x1ffff;

    
    define->data = 0x1ffff;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_iqs_credit_hungry_min_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_credit;
    int define_index = dnx_data_iqs_credit_define_hungry_min;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = -1 * 0x3ffff;

    
    define->data = -1 * 0x3ffff;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_iqs_credit_hungry_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_credit;
    int define_index = dnx_data_iqs_credit_define_hungry_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x3ffff;

    
    define->data = 0x3ffff;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_iqs_credit_hungry_mult_min_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_credit;
    int define_index = dnx_data_iqs_credit_define_hungry_mult_min;
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
jr2_a0_dnx_data_iqs_credit_hungry_mult_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_credit;
    int define_index = dnx_data_iqs_credit_define_hungry_mult_max;
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
jr2_a0_dnx_data_iqs_credit_nof_bw_levels_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_credit;
    int define_index = dnx_data_iqs_credit_define_nof_bw_levels;
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
jr2_a0_dnx_data_iqs_credit_fmq_credit_fc_on_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_credit;
    int define_index = dnx_data_iqs_credit_define_fmq_credit_fc_on_th;
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
jr2_a0_dnx_data_iqs_credit_fmq_credit_fc_off_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_credit;
    int define_index = dnx_data_iqs_credit_define_fmq_credit_fc_off_th;
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
jr2_a0_dnx_data_iqs_credit_fmq_byte_fc_on_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_credit;
    int define_index = dnx_data_iqs_credit_define_fmq_byte_fc_on_th;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1000;

    
    define->data = 1000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_iqs_credit_fmq_byte_fc_off_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_credit;
    int define_index = dnx_data_iqs_credit_define_fmq_byte_fc_off_th;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 800;

    
    define->data = 800;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_iqs_credit_fmq_eir_credit_fc_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_credit;
    int define_index = dnx_data_iqs_credit_define_fmq_eir_credit_fc_th;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x1fff;

    
    define->data = 0x1fff;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_iqs_credit_fmq_eir_byte_fc_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_credit;
    int define_index = dnx_data_iqs_credit_define_fmq_eir_byte_fc_th;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x1fffff;

    
    define->data = 0x1fffff;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_iqs_credit_fmq_nof_be_classes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_credit;
    int define_index = dnx_data_iqs_credit_define_fmq_nof_be_classes;
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
jr2_a0_dnx_data_iqs_credit_fmq_max_be_weight_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_credit;
    int define_index = dnx_data_iqs_credit_define_fmq_max_be_weight;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 31;

    
    define->data = 31;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_iqs_credit_fmq_max_burst_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_credit;
    int define_index = dnx_data_iqs_credit_define_fmq_max_burst_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 63;

    
    define->data = 63;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_iqs_credit_fmq_shp_crdt_rate_mltp_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_credit;
    int define_index = dnx_data_iqs_credit_define_fmq_shp_crdt_rate_mltp;
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
jr2_a0_dnx_data_iqs_credit_fmq_shp_crdt_rate_delta_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_credit;
    int define_index = dnx_data_iqs_credit_define_fmq_shp_crdt_rate_delta;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 7;

    
    define->data = 7;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_iqs_credit_max_rate_profile_preset_gbps_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_credit;
    int define_index = dnx_data_iqs_credit_define_max_rate_profile_preset_gbps;
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
jr2_a0_dnx_data_iqs_credit_worth_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_credit;
    int define_index = dnx_data_iqs_credit_define_worth;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4096;

    
    define->data = 4096;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CREDIT_SIZE;
    define->property.doc = 
        "\n"
        "Set local credit worth. Recommended value is 4096 Bytes.\n"
        "Credit worth has significant implications on the scheduled traffic.\n"
        "Consult with Broadcom applications engineer before changing the credit size value.\n"
        "Supported range [1-8191]\n"
        "Syntax:\n"
        "credit_size=#credit size in bytes#\n"
        "Default:\n"
        "4096\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 1;
    define->property.range_max = dnx_data_iqs.credit.max_worth_get(unit);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_iqs_credit_fmq_shp_rate_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_credit;
    int define_index = dnx_data_iqs_credit_define_fmq_shp_rate_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_device.general.maximal_core_bandwidth_kbps_get(unit);

    
    define->data = dnx_data_device.general.maximal_core_bandwidth_kbps_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_iqs_deq_default_dqcq_fc_to_dram_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_deq_default;
    int feature_index = dnx_data_iqs_deq_default_dqcq_fc_to_dram;
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
jr2_a0_dnx_data_iqs_deq_default_low_delay_deq_bytes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_deq_default;
    int define_index = dnx_data_iqs_deq_default_define_low_delay_deq_bytes;
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
jr2_a0_dnx_data_iqs_deq_default_credit_balance_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_deq_default;
    int define_index = dnx_data_iqs_deq_default_define_credit_balance_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 128 * 1024;

    
    define->data = 128 * 1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_iqs_deq_default_credit_balance_resolution_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_deq_default;
    int define_index = dnx_data_iqs_deq_default_define_credit_balance_resolution;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4 * 1024;

    
    define->data = 4 * 1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_iqs_deq_default_s2d_credit_balance_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_deq_default;
    int define_index = dnx_data_iqs_deq_default_define_s2d_credit_balance_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2 * 1024 * 1024;

    
    define->data = 2 * 1024 * 1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_iqs_deq_default_s2d_credit_balance_resolution_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_deq_default;
    int define_index = dnx_data_iqs_deq_default_define_s2d_credit_balance_resolution;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64 * 1024;

    
    define->data = 64 * 1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_iqs_deq_default_sram_to_fabric_credit_lfsr_thr_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_deq_default;
    int define_index = dnx_data_iqs_deq_default_define_sram_to_fabric_credit_lfsr_thr;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x70;

    
    define->data = 0x70;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_iqs_deq_default_sram_to_fabric_credit_lfsr_mask_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_deq_default;
    int define_index = dnx_data_iqs_deq_default_define_sram_to_fabric_credit_lfsr_mask;
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
jr2_a0_dnx_data_iqs_deq_default_sqm_read_weight_profiles_set(
    int unit)
{
    int profile_id_index;
    dnx_data_iqs_deq_default_sqm_read_weight_profiles_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_deq_default;
    int table_index = dnx_data_iqs_deq_default_table_sqm_read_weight_profiles;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 8;
    table->info_get.key_size[0] = 8;

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_iqs_deq_default_sqm_read_weight_profiles_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_iqs_deq_default_table_sqm_read_weight_profiles");

    
    default_data = (dnx_data_iqs_deq_default_sqm_read_weight_profiles_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->weight = -1;
    
    for (profile_id_index = 0; profile_id_index < table->keys[0].size; profile_id_index++)
    {
        data = (dnx_data_iqs_deq_default_sqm_read_weight_profiles_t *) dnxc_data_mgmt_table_data_get(unit, table, profile_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_iqs_deq_default_sqm_read_weight_profiles_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->weight = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_iqs_deq_default_sqm_read_weight_profiles_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->weight = 0;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_iqs_deq_default_sqm_read_weight_profiles_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->weight = 1;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_iqs_deq_default_sqm_read_weight_profiles_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->weight = 1;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_iqs_deq_default_sqm_read_weight_profiles_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->weight = 3;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_iqs_deq_default_sqm_read_weight_profiles_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->weight = 6;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_iqs_deq_default_sqm_read_weight_profiles_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->weight = 7;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_iqs_deq_default_sqm_read_weight_profiles_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->weight = 10;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_iqs_deq_default_dqm_read_weight_profiles_set(
    int unit)
{
    int profile_id_index;
    dnx_data_iqs_deq_default_dqm_read_weight_profiles_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_deq_default;
    int table_index = dnx_data_iqs_deq_default_table_dqm_read_weight_profiles;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 8;
    table->info_get.key_size[0] = 8;

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_iqs_deq_default_dqm_read_weight_profiles_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_iqs_deq_default_table_dqm_read_weight_profiles");

    
    default_data = (dnx_data_iqs_deq_default_dqm_read_weight_profiles_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->weight = -1;
    
    for (profile_id_index = 0; profile_id_index < table->keys[0].size; profile_id_index++)
    {
        data = (dnx_data_iqs_deq_default_dqm_read_weight_profiles_t *) dnxc_data_mgmt_table_data_get(unit, table, profile_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_iqs_deq_default_dqm_read_weight_profiles_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->weight = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_iqs_deq_default_dqm_read_weight_profiles_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->weight = 0;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_iqs_deq_default_dqm_read_weight_profiles_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->weight = 0;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_iqs_deq_default_dqm_read_weight_profiles_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->weight = 0;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_iqs_deq_default_dqm_read_weight_profiles_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->weight = 1;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_iqs_deq_default_dqm_read_weight_profiles_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->weight = 2;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_iqs_deq_default_dqm_read_weight_profiles_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->weight = 2;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_iqs_deq_default_dqm_read_weight_profiles_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->weight = 3;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_iqs_deq_default_params_set(
    int unit)
{
    int bw_level_index;
    dnx_data_iqs_deq_default_params_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_deq_default;
    int table_index = dnx_data_iqs_deq_default_table_params;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_iqs.credit.nof_bw_levels_get(unit);
    table->info_get.key_size[0] = dnx_data_iqs.credit.nof_bw_levels_get(unit);

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    table->values[4].default_val = "0";
    table->values[5].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_iqs_deq_default_params_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_iqs_deq_default_table_params");

    
    default_data = (dnx_data_iqs_deq_default_params_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->sram_read_weight_profile_val[0] = 0;
    default_data->s2d_read_weight_profile_val[0] = 0;
    default_data->sram_extra_credits_val[0] = 0;
    default_data->s2d_extra_credits_val[0] = 0;
    default_data->sram_extra_credits_lfsr_val[0] = 0;
    default_data->s2d_extra_credits_lfsr_val[0] = 0;
    
    for (bw_level_index = 0; bw_level_index < table->keys[0].size; bw_level_index++)
    {
        data = (dnx_data_iqs_deq_default_params_t *) dnxc_data_mgmt_table_data_get(unit, table, bw_level_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_iqs_deq_default_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        SHR_RANGE_VERIFY(2, 0, DNX_IQS_DEQ_PARAM_TYPE_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->sram_read_weight_profile_val[0] = 0;
        data->sram_read_weight_profile_val[1] = 2;
        SHR_RANGE_VERIFY(2, 0, DNX_IQS_DEQ_PARAM_TYPE_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->s2d_read_weight_profile_val[0] = 0;
        data->s2d_read_weight_profile_val[1] = 2;
        SHR_RANGE_VERIFY(2, 0, DNX_IQS_DEQ_PARAM_TYPE_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->sram_extra_credits_val[0] = 0;
        data->sram_extra_credits_val[1] = 4;
        SHR_RANGE_VERIFY(2, 0, DNX_IQS_DEQ_PARAM_TYPE_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->s2d_extra_credits_val[0] = 0;
        data->s2d_extra_credits_val[1] = 4;
        SHR_RANGE_VERIFY(2, 0, DNX_IQS_DEQ_PARAM_TYPE_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->sram_extra_credits_lfsr_val[0] = 0;
        data->sram_extra_credits_lfsr_val[1] = 1;
        SHR_RANGE_VERIFY(2, 0, DNX_IQS_DEQ_PARAM_TYPE_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->s2d_extra_credits_lfsr_val[0] = 0;
        data->s2d_extra_credits_lfsr_val[1] = 1;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_iqs_deq_default_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        SHR_RANGE_VERIFY(2, 0, DNX_IQS_DEQ_PARAM_TYPE_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->sram_read_weight_profile_val[0] = 0;
        data->sram_read_weight_profile_val[1] = 3;
        SHR_RANGE_VERIFY(2, 0, DNX_IQS_DEQ_PARAM_TYPE_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->s2d_read_weight_profile_val[0] = 0;
        data->s2d_read_weight_profile_val[1] = 3;
        SHR_RANGE_VERIFY(2, 0, DNX_IQS_DEQ_PARAM_TYPE_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->sram_extra_credits_val[0] = 0;
        data->sram_extra_credits_val[1] = 4;
        SHR_RANGE_VERIFY(2, 0, DNX_IQS_DEQ_PARAM_TYPE_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->s2d_extra_credits_val[0] = 0;
        data->s2d_extra_credits_val[1] = 4;
        SHR_RANGE_VERIFY(2, 0, DNX_IQS_DEQ_PARAM_TYPE_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->sram_extra_credits_lfsr_val[0] = 0;
        data->sram_extra_credits_lfsr_val[1] = 1;
        SHR_RANGE_VERIFY(2, 0, DNX_IQS_DEQ_PARAM_TYPE_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->s2d_extra_credits_lfsr_val[0] = 0;
        data->s2d_extra_credits_lfsr_val[1] = 1;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_iqs_deq_default_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        SHR_RANGE_VERIFY(3, 0, DNX_IQS_DEQ_PARAM_TYPE_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->sram_read_weight_profile_val[0] = 0;
        data->sram_read_weight_profile_val[1] = 2;
        data->sram_read_weight_profile_val[2] = 7;
        SHR_RANGE_VERIFY(3, 0, DNX_IQS_DEQ_PARAM_TYPE_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->s2d_read_weight_profile_val[0] = 0;
        data->s2d_read_weight_profile_val[1] = 2;
        data->s2d_read_weight_profile_val[2] = 7;
        SHR_RANGE_VERIFY(3, 0, DNX_IQS_DEQ_PARAM_TYPE_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->sram_extra_credits_val[0] = 0;
        data->sram_extra_credits_val[1] = 12;
        data->sram_extra_credits_val[2] = 12;
        SHR_RANGE_VERIFY(3, 0, DNX_IQS_DEQ_PARAM_TYPE_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->s2d_extra_credits_val[0] = 0;
        data->s2d_extra_credits_val[1] = 12;
        data->s2d_extra_credits_val[2] = 12;
        SHR_RANGE_VERIFY(3, 0, DNX_IQS_DEQ_PARAM_TYPE_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->sram_extra_credits_lfsr_val[0] = 0;
        data->sram_extra_credits_lfsr_val[1] = 0;
        data->sram_extra_credits_lfsr_val[2] = 0;
        SHR_RANGE_VERIFY(3, 0, DNX_IQS_DEQ_PARAM_TYPE_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->s2d_extra_credits_lfsr_val[0] = 0;
        data->s2d_extra_credits_lfsr_val[1] = 0;
        data->s2d_extra_credits_lfsr_val[2] = 0;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_iqs_deq_default_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        SHR_RANGE_VERIFY(3, 0, DNX_IQS_DEQ_PARAM_TYPE_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->sram_read_weight_profile_val[0] = 0;
        data->sram_read_weight_profile_val[1] = 2;
        data->sram_read_weight_profile_val[2] = 7;
        SHR_RANGE_VERIFY(3, 0, DNX_IQS_DEQ_PARAM_TYPE_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->s2d_read_weight_profile_val[0] = 0;
        data->s2d_read_weight_profile_val[1] = 2;
        data->s2d_read_weight_profile_val[2] = 7;
        SHR_RANGE_VERIFY(3, 0, DNX_IQS_DEQ_PARAM_TYPE_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->sram_extra_credits_val[0] = 0;
        data->sram_extra_credits_val[1] = 12;
        data->sram_extra_credits_val[2] = 12;
        SHR_RANGE_VERIFY(3, 0, DNX_IQS_DEQ_PARAM_TYPE_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->s2d_extra_credits_val[0] = 0;
        data->s2d_extra_credits_val[1] = 1;
        data->s2d_extra_credits_val[2] = 12;
        SHR_RANGE_VERIFY(3, 0, DNX_IQS_DEQ_PARAM_TYPE_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->sram_extra_credits_lfsr_val[0] = 0;
        data->sram_extra_credits_lfsr_val[1] = 1;
        data->sram_extra_credits_lfsr_val[2] = 1;
        SHR_RANGE_VERIFY(3, 0, DNX_IQS_DEQ_PARAM_TYPE_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->s2d_extra_credits_lfsr_val[0] = 0;
        data->s2d_extra_credits_lfsr_val[1] = 1;
        data->s2d_extra_credits_lfsr_val[2] = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_iqs_deq_default_dram_params_set(
    int unit)
{
    int index_index;
    dnx_data_iqs_deq_default_dram_params_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_deq_default;
    int table_index = dnx_data_iqs_deq_default_table_dram_params;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 8;
    table->info_get.key_size[0] = 8;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_iqs_deq_default_dram_params_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_iqs_deq_default_table_dram_params");

    
    default_data = (dnx_data_iqs_deq_default_dram_params_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->read_weight_profile_val = 0;
    default_data->read_weight_profile_th = 0;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_iqs_deq_default_dram_params_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_iqs_deq_default_dram_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->read_weight_profile_val = 0;
        data->read_weight_profile_th = 8 * 1024;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_iqs_deq_default_dram_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->read_weight_profile_val = 0;
        data->read_weight_profile_th = 16 * 1024;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_iqs_deq_default_dram_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->read_weight_profile_val = 1;
        data->read_weight_profile_th = 24 * 1024;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_iqs_deq_default_dram_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->read_weight_profile_val = 2;
        data->read_weight_profile_th = 32 * 1024;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_iqs_deq_default_dram_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->read_weight_profile_val = 2;
        data->read_weight_profile_th = 50 * 1024;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_iqs_deq_default_dram_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->read_weight_profile_val = 2;
        data->read_weight_profile_th = 75 * 1024;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_iqs_deq_default_dram_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->read_weight_profile_val = 3;
        data->read_weight_profile_th = 100 * 1024;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_iqs_deq_default_dram_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->read_weight_profile_val = 4;
        data->read_weight_profile_th = 128 * 1024;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_iqs_deq_default_max_deq_cmd_set(
    int unit)
{
    int bw_level_index;
    int nof_active_queues_index;
    dnx_data_iqs_deq_default_max_deq_cmd_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_deq_default;
    int table_index = dnx_data_iqs_deq_default_table_max_deq_cmd;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_iqs.credit.nof_bw_levels_get(unit);
    table->info_get.key_size[0] = dnx_data_iqs.credit.nof_bw_levels_get(unit);
    table->keys[1].size = DBAL_NOF_ENUM_IQS_ACTIVE_QUEUES_VALUES;
    table->info_get.key_size[1] = DBAL_NOF_ENUM_IQS_ACTIVE_QUEUES_VALUES;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_iqs_deq_default_max_deq_cmd_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_iqs_deq_default_table_max_deq_cmd");

    
    default_data = (dnx_data_iqs_deq_default_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->value = 0;
    
    for (bw_level_index = 0; bw_level_index < table->keys[0].size; bw_level_index++)
    {
        for (nof_active_queues_index = 0; nof_active_queues_index < table->keys[1].size; nof_active_queues_index++)
        {
            data = (dnx_data_iqs_deq_default_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, bw_level_index, nof_active_queues_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (0 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_ONE < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_ONE);
        data->value = 8*1024/256;
    }
    if (1 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_ONE < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_ONE);
        data->value = 30*1024/256;
    }
    if (2 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_ONE < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_ONE);
        data->value = 50*1024/256;
    }
    if (3 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_ONE < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_ONE);
        data->value = 127*1024/256;
    }
    if (0 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_THREE < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_THREE);
        data->value = 8*1024/256;
    }
    if (1 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_THREE < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_THREE);
        data->value = 30*1024/256;
    }
    if (2 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_THREE < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_THREE);
        data->value = 50*1024/256;
    }
    if (3 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_THREE < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_THREE);
        data->value = 127*1024/256;
    }
    if (0 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_SEVEN < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_SEVEN);
        data->value = 8*1024/256;
    }
    if (1 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_SEVEN < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_SEVEN);
        data->value = 30*1024/256;
    }
    if (2 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_SEVEN < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_SEVEN);
        data->value = 50*1024/256;
    }
    if (3 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_SEVEN < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_SEVEN);
        data->value = 127*1024/256;
    }
    if (0 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_REST < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_REST);
        data->value = 4*1024/256;
    }
    if (1 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_REST < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_REST);
        data->value = 15*1024/256;
    }
    if (2 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_REST < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_REST);
        data->value = 25*1024/256;
    }
    if (3 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_REST < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_REST);
        data->value = 64*1024/256;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_iqs_deq_default_s2d_max_deq_cmd_set(
    int unit)
{
    int bw_level_index;
    int nof_active_queues_index;
    dnx_data_iqs_deq_default_s2d_max_deq_cmd_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_deq_default;
    int table_index = dnx_data_iqs_deq_default_table_s2d_max_deq_cmd;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_iqs.credit.nof_bw_levels_get(unit);
    table->info_get.key_size[0] = dnx_data_iqs.credit.nof_bw_levels_get(unit);
    table->keys[1].size = DBAL_NOF_ENUM_IQS_ACTIVE_QUEUES_VALUES;
    table->info_get.key_size[1] = DBAL_NOF_ENUM_IQS_ACTIVE_QUEUES_VALUES;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_iqs_deq_default_s2d_max_deq_cmd_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_iqs_deq_default_table_s2d_max_deq_cmd");

    
    default_data = (dnx_data_iqs_deq_default_s2d_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->value = 0;
    
    for (bw_level_index = 0; bw_level_index < table->keys[0].size; bw_level_index++)
    {
        for (nof_active_queues_index = 0; nof_active_queues_index < table->keys[1].size; nof_active_queues_index++)
        {
            data = (dnx_data_iqs_deq_default_s2d_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, bw_level_index, nof_active_queues_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (0 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_ONE < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_s2d_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_ONE);
        data->value = 8*1024/256;
    }
    if (1 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_ONE < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_s2d_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_ONE);
        data->value = 30*1024/256;
    }
    if (2 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_ONE < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_s2d_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_ONE);
        data->value = 50*1024/256;
    }
    if (3 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_ONE < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_s2d_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_ONE);
        data->value = 127*1024/256;
    }
    if (0 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_THREE < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_s2d_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_THREE);
        data->value = 8*1024/256;
    }
    if (1 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_THREE < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_s2d_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_THREE);
        data->value = 30*1024/256;
    }
    if (2 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_THREE < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_s2d_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_THREE);
        data->value = 50*1024/256;
    }
    if (3 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_THREE < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_s2d_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_THREE);
        data->value = 127*1024/256;
    }
    if (0 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_SEVEN < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_s2d_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_SEVEN);
        data->value = 8*1024/256;
    }
    if (1 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_SEVEN < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_s2d_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_SEVEN);
        data->value = 30*1024/256;
    }
    if (2 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_SEVEN < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_s2d_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_SEVEN);
        data->value = 50*1024/256;
    }
    if (3 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_SEVEN < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_s2d_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_UP_TO_SEVEN);
        data->value = 127*1024/256;
    }
    if (0 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_REST < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_s2d_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_REST);
        data->value = 4*1024/256;
    }
    if (1 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_REST < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_s2d_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_REST);
        data->value = 15*1024/256;
    }
    if (2 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_REST < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_s2d_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_REST);
        data->value = 25*1024/256;
    }
    if (3 < table->keys[0].size && DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_REST < table->keys[1].size)
    {
        data = (dnx_data_iqs_deq_default_s2d_max_deq_cmd_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, DBAL_ENUM_FVAL_IQS_ACTIVE_QUEUES_REST);
        data->value = 64*1024/256;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_iqs_dqcq_8_priorities_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_dqcq;
    int feature_index = dnx_data_iqs_dqcq_8_priorities;
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
jr2_a0_dnx_data_iqs_dqcq_max_nof_contexts_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_dqcq;
    int define_index = dnx_data_iqs_dqcq_define_max_nof_contexts;
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
jr2_a0_dnx_data_iqs_dqcq_nof_priorities_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_dqcq;
    int define_index = dnx_data_iqs_dqcq_define_nof_priorities;
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
jr2_a0_dnx_data_iqs_dbal_ipt_counters_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_dbal;
    int define_index = dnx_data_iqs_dbal_define_ipt_counters_nof_bits;
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
jr2_a0_dnx_data_iqs_flush_bytes_units_to_send_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_iqs;
    int submodule_index = dnx_data_iqs_submodule_flush;
    int define_index = dnx_data_iqs_flush_define_bytes_units_to_send;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 128;

    
    define->data = 128;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
jr2_a0_data_iqs_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_iqs;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_iqs_submodule_credit;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_iqs_credit_define_nof_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_credit_nof_profiles_set;
    data_index = dnx_data_iqs_credit_define_max_credit_balance_threshold;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_credit_max_credit_balance_threshold_set;
    data_index = dnx_data_iqs_credit_define_max_worth;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_credit_max_worth_set;
    data_index = dnx_data_iqs_credit_define_satisified_back_min;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_credit_satisified_back_min_set;
    data_index = dnx_data_iqs_credit_define_satisified_back_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_credit_satisified_back_max_set;
    data_index = dnx_data_iqs_credit_define_satisified_empty_min;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_credit_satisified_empty_min_set;
    data_index = dnx_data_iqs_credit_define_satisified_empty_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_credit_satisified_empty_max_set;
    data_index = dnx_data_iqs_credit_define_hungry_min;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_credit_hungry_min_set;
    data_index = dnx_data_iqs_credit_define_hungry_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_credit_hungry_max_set;
    data_index = dnx_data_iqs_credit_define_hungry_mult_min;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_credit_hungry_mult_min_set;
    data_index = dnx_data_iqs_credit_define_hungry_mult_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_credit_hungry_mult_max_set;
    data_index = dnx_data_iqs_credit_define_nof_bw_levels;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_credit_nof_bw_levels_set;
    data_index = dnx_data_iqs_credit_define_fmq_credit_fc_on_th;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_credit_fmq_credit_fc_on_th_set;
    data_index = dnx_data_iqs_credit_define_fmq_credit_fc_off_th;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_credit_fmq_credit_fc_off_th_set;
    data_index = dnx_data_iqs_credit_define_fmq_byte_fc_on_th;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_credit_fmq_byte_fc_on_th_set;
    data_index = dnx_data_iqs_credit_define_fmq_byte_fc_off_th;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_credit_fmq_byte_fc_off_th_set;
    data_index = dnx_data_iqs_credit_define_fmq_eir_credit_fc_th;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_credit_fmq_eir_credit_fc_th_set;
    data_index = dnx_data_iqs_credit_define_fmq_eir_byte_fc_th;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_credit_fmq_eir_byte_fc_th_set;
    data_index = dnx_data_iqs_credit_define_fmq_nof_be_classes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_credit_fmq_nof_be_classes_set;
    data_index = dnx_data_iqs_credit_define_fmq_max_be_weight;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_credit_fmq_max_be_weight_set;
    data_index = dnx_data_iqs_credit_define_fmq_max_burst_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_credit_fmq_max_burst_max_set;
    data_index = dnx_data_iqs_credit_define_fmq_shp_crdt_rate_mltp;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_credit_fmq_shp_crdt_rate_mltp_set;
    data_index = dnx_data_iqs_credit_define_fmq_shp_crdt_rate_delta;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_credit_fmq_shp_crdt_rate_delta_set;
    data_index = dnx_data_iqs_credit_define_max_rate_profile_preset_gbps;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_credit_max_rate_profile_preset_gbps_set;
    data_index = dnx_data_iqs_credit_define_worth;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_credit_worth_set;
    data_index = dnx_data_iqs_credit_define_fmq_shp_rate_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_credit_fmq_shp_rate_max_set;

    
    data_index = dnx_data_iqs_credit_watchdog_fixed_period;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_iqs_credit_watchdog_fixed_period_set;
    data_index = dnx_data_iqs_credit_fmq_shp_zero_rate_allowed;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_iqs_credit_fmq_shp_zero_rate_allowed_set;
    data_index = dnx_data_iqs_credit_credit_balance_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_iqs_credit_credit_balance_support_set;

    
    
    submodule_index = dnx_data_iqs_submodule_deq_default;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_iqs_deq_default_define_low_delay_deq_bytes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_deq_default_low_delay_deq_bytes_set;
    data_index = dnx_data_iqs_deq_default_define_credit_balance_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_deq_default_credit_balance_max_set;
    data_index = dnx_data_iqs_deq_default_define_credit_balance_resolution;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_deq_default_credit_balance_resolution_set;
    data_index = dnx_data_iqs_deq_default_define_s2d_credit_balance_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_deq_default_s2d_credit_balance_max_set;
    data_index = dnx_data_iqs_deq_default_define_s2d_credit_balance_resolution;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_deq_default_s2d_credit_balance_resolution_set;
    data_index = dnx_data_iqs_deq_default_define_sram_to_fabric_credit_lfsr_thr;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_deq_default_sram_to_fabric_credit_lfsr_thr_set;
    data_index = dnx_data_iqs_deq_default_define_sram_to_fabric_credit_lfsr_mask;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_deq_default_sram_to_fabric_credit_lfsr_mask_set;

    
    data_index = dnx_data_iqs_deq_default_dqcq_fc_to_dram;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_iqs_deq_default_dqcq_fc_to_dram_set;

    
    data_index = dnx_data_iqs_deq_default_table_sqm_read_weight_profiles;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_iqs_deq_default_sqm_read_weight_profiles_set;
    data_index = dnx_data_iqs_deq_default_table_dqm_read_weight_profiles;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_iqs_deq_default_dqm_read_weight_profiles_set;
    data_index = dnx_data_iqs_deq_default_table_params;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_iqs_deq_default_params_set;
    data_index = dnx_data_iqs_deq_default_table_dram_params;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_iqs_deq_default_dram_params_set;
    data_index = dnx_data_iqs_deq_default_table_max_deq_cmd;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_iqs_deq_default_max_deq_cmd_set;
    data_index = dnx_data_iqs_deq_default_table_s2d_max_deq_cmd;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_iqs_deq_default_s2d_max_deq_cmd_set;
    
    submodule_index = dnx_data_iqs_submodule_dqcq;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_iqs_dqcq_define_max_nof_contexts;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_dqcq_max_nof_contexts_set;
    data_index = dnx_data_iqs_dqcq_define_nof_priorities;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_dqcq_nof_priorities_set;

    
    data_index = dnx_data_iqs_dqcq_8_priorities;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_iqs_dqcq_8_priorities_set;

    
    
    submodule_index = dnx_data_iqs_submodule_dbal;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_iqs_dbal_define_ipt_counters_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_dbal_ipt_counters_nof_bits_set;

    

    
    
    submodule_index = dnx_data_iqs_submodule_flush;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_iqs_flush_define_bytes_units_to_send;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_iqs_flush_bytes_units_to_send_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

