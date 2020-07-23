

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_BIERDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_bier.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>







static shr_error_e
jr2_a0_dnx_data_bier_params_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_bier;
    int submodule_index = dnx_data_bier_submodule_params;
    int feature_index = dnx_data_bier_params_is_supported;
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
jr2_a0_dnx_data_bier_params_bitstring_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_bier;
    int submodule_index = dnx_data_bier_submodule_params;
    int define_index = dnx_data_bier_params_define_bitstring_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 256;

    
    define->data = 256;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_bier_params_bfr_entry_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_bier;
    int submodule_index = dnx_data_bier_submodule_params;
    int define_index = dnx_data_bier_params_define_bfr_entry_size;
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
jr2_a0_dnx_data_bier_params_bank_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_bier;
    int submodule_index = dnx_data_bier_submodule_params;
    int define_index = dnx_data_bier_params_define_bank_entries;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2048;

    
    define->data = 2048;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_bier_params_bundle_nof_banks_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_bier;
    int submodule_index = dnx_data_bier_submodule_params;
    int define_index = dnx_data_bier_params_define_bundle_nof_banks;
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
jr2_a0_dnx_data_bier_params_bundle_nof_banks_net_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_bier;
    int submodule_index = dnx_data_bier_submodule_params;
    int define_index = dnx_data_bier_params_define_bundle_nof_banks_net;
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
jr2_a0_dnx_data_bier_params_egress_only_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_bier;
    int submodule_index = dnx_data_bier_submodule_params;
    int define_index = dnx_data_bier_params_define_egress_only;
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
jr2_a0_dnx_data_bier_params_nof_egr_table_copies_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_bier;
    int submodule_index = dnx_data_bier_submodule_params;
    int define_index = dnx_data_bier_params_define_nof_egr_table_copies;
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
jr2_a0_dnx_data_bier_params_ingress_bfr_pos_msb_start_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_bier;
    int submodule_index = dnx_data_bier_submodule_params;
    int define_index = dnx_data_bier_params_define_ingress_bfr_pos_msb_start;
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
jr2_a0_dnx_data_bier_params_nof_bfr_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_bier;
    int submodule_index = dnx_data_bier_submodule_params;
    int define_index = dnx_data_bier_params_define_nof_bfr_entries;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_BIER_NOF_BFR_ENTRIES;
    define->property.doc = 
        "\n"
        "bier supported bfr size\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 32768;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}




shr_error_e
jr2_a0_data_bier_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_bier;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_bier_submodule_params;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_bier_params_define_bitstring_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_bier_params_bitstring_size_set;
    data_index = dnx_data_bier_params_define_bfr_entry_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_bier_params_bfr_entry_size_set;
    data_index = dnx_data_bier_params_define_bank_entries;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_bier_params_bank_entries_set;
    data_index = dnx_data_bier_params_define_bundle_nof_banks;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_bier_params_bundle_nof_banks_set;
    data_index = dnx_data_bier_params_define_bundle_nof_banks_net;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_bier_params_bundle_nof_banks_net_set;
    data_index = dnx_data_bier_params_define_egress_only;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_bier_params_egress_only_set;
    data_index = dnx_data_bier_params_define_nof_egr_table_copies;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_bier_params_nof_egr_table_copies_set;
    data_index = dnx_data_bier_params_define_ingress_bfr_pos_msb_start;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_bier_params_ingress_bfr_pos_msb_start_set;
    data_index = dnx_data_bier_params_define_nof_bfr_entries;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_bier_params_nof_bfr_entries_set;

    
    data_index = dnx_data_bier_params_is_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_bier_params_is_supported_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

