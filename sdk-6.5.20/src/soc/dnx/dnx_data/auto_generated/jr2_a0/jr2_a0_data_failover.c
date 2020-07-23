

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FAILOVER

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_failover.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>








static shr_error_e
jr2_a0_dnx_data_failover_path_select_egress_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_failover;
    int submodule_index = dnx_data_failover_submodule_path_select;
    int define_index = dnx_data_failover_path_select_define_egress_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 128*1024;

    
    define->data = 128*1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_failover_path_select_ingress_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_failover;
    int submodule_index = dnx_data_failover_submodule_path_select;
    int define_index = dnx_data_failover_path_select_define_ingress_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16*1024;

    
    define->data = 16*1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_failover_path_select_fec_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_failover;
    int submodule_index = dnx_data_failover_submodule_path_select;
    int define_index = dnx_data_failover_path_select_define_fec_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 256*1024;

    
    define->data = 256*1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_failover_path_select_egr_no_protection_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_failover;
    int submodule_index = dnx_data_failover_submodule_path_select;
    int define_index = dnx_data_failover_path_select_define_egr_no_protection;
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
jr2_a0_dnx_data_failover_path_select_ing_no_protection_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_failover;
    int submodule_index = dnx_data_failover_submodule_path_select;
    int define_index = dnx_data_failover_path_select_define_ing_no_protection;
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
jr2_a0_dnx_data_failover_path_select_fec_no_protection_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_failover;
    int submodule_index = dnx_data_failover_submodule_path_select;
    int define_index = dnx_data_failover_path_select_define_fec_no_protection;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_failover.path_select.fec_size_get(unit)-1;

    
    define->data = dnx_data_failover.path_select.fec_size_get(unit)-1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_failover_path_select_fec_facility_protection_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_failover;
    int submodule_index = dnx_data_failover_submodule_path_select;
    int define_index = dnx_data_failover_path_select_define_fec_facility_protection;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_failover.path_select.fec_size_get(unit)-2;

    
    define->data = dnx_data_failover.path_select.fec_size_get(unit)-2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_failover_path_select_nof_fec_path_select_banks_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_failover;
    int submodule_index = dnx_data_failover_submodule_path_select;
    int define_index = dnx_data_failover_path_select_define_nof_fec_path_select_banks;
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
jr2_a0_dnx_data_failover_path_select_fec_bank_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_failover;
    int submodule_index = dnx_data_failover_submodule_path_select;
    int define_index = dnx_data_failover_path_select_define_fec_bank_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_failover.path_select.fec_size_get(unit)/dnx_data_failover.path_select.nof_fec_path_select_banks_get(unit);

    
    define->data = dnx_data_failover.path_select.fec_size_get(unit)/dnx_data_failover.path_select.nof_fec_path_select_banks_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_failover_path_select_in_lif_protection_pointer_nof_msb_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_failover;
    int submodule_index = dnx_data_failover_submodule_path_select;
    int define_index = dnx_data_failover_path_select_define_in_lif_protection_pointer_nof_msb;
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
jr2_a0_dnx_data_failover_path_select_in_lif_protection_pointer_nof_lsb_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_failover;
    int submodule_index = dnx_data_failover_submodule_path_select;
    int define_index = dnx_data_failover_path_select_define_in_lif_protection_pointer_nof_lsb;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 12;

    
    define->data = 12;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_failover_facility_accelerated_mode_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_failover;
    int submodule_index = dnx_data_failover_submodule_facility;
    int feature_index = dnx_data_failover_facility_accelerated_mode_enable;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_BCM886XX_FEC_ACCELERATED_REROUTE_MODE;
    feature->property.doc = 
        "\n"
        "Enable/disable facility accelerated protection.\n"
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





shr_error_e
jr2_a0_data_failover_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_failover;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_failover_submodule_path_select;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_failover_path_select_define_egress_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_failover_path_select_egress_size_set;
    data_index = dnx_data_failover_path_select_define_ingress_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_failover_path_select_ingress_size_set;
    data_index = dnx_data_failover_path_select_define_fec_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_failover_path_select_fec_size_set;
    data_index = dnx_data_failover_path_select_define_egr_no_protection;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_failover_path_select_egr_no_protection_set;
    data_index = dnx_data_failover_path_select_define_ing_no_protection;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_failover_path_select_ing_no_protection_set;
    data_index = dnx_data_failover_path_select_define_fec_no_protection;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_failover_path_select_fec_no_protection_set;
    data_index = dnx_data_failover_path_select_define_fec_facility_protection;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_failover_path_select_fec_facility_protection_set;
    data_index = dnx_data_failover_path_select_define_nof_fec_path_select_banks;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_failover_path_select_nof_fec_path_select_banks_set;
    data_index = dnx_data_failover_path_select_define_fec_bank_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_failover_path_select_fec_bank_size_set;
    data_index = dnx_data_failover_path_select_define_in_lif_protection_pointer_nof_msb;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_failover_path_select_in_lif_protection_pointer_nof_msb_set;
    data_index = dnx_data_failover_path_select_define_in_lif_protection_pointer_nof_lsb;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_failover_path_select_in_lif_protection_pointer_nof_lsb_set;

    

    
    
    submodule_index = dnx_data_failover_submodule_facility;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_failover_facility_accelerated_mode_enable;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_failover_facility_accelerated_mode_enable_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

