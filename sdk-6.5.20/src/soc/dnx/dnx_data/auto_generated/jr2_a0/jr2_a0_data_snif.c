

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MIRROR

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_snif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>








static shr_error_e
jr2_a0_dnx_data_snif_general_cud_type_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_snif;
    int submodule_index = dnx_data_snif_submodule_general;
    int define_index = dnx_data_snif_general_define_cud_type;
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
jr2_a0_dnx_data_snif_ingress_nof_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_snif;
    int submodule_index = dnx_data_snif_submodule_ingress;
    int define_index = dnx_data_snif_ingress_define_nof_profiles;
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
jr2_a0_dnx_data_snif_ingress_mirror_nof_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_snif;
    int submodule_index = dnx_data_snif_submodule_ingress;
    int define_index = dnx_data_snif_ingress_define_mirror_nof_profiles;
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
jr2_a0_dnx_data_snif_ingress_nof_mirror_on_drop_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_snif;
    int submodule_index = dnx_data_snif_submodule_ingress;
    int define_index = dnx_data_snif_ingress_define_nof_mirror_on_drop_profiles;
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
jr2_a0_dnx_data_snif_ingress_mirror_nof_vlan_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_snif;
    int submodule_index = dnx_data_snif_submodule_ingress;
    int define_index = dnx_data_snif_ingress_define_mirror_nof_vlan_profiles;
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
jr2_a0_dnx_data_snif_ingress_mirror_default_tagged_profile_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_snif;
    int submodule_index = dnx_data_snif_submodule_ingress;
    int define_index = dnx_data_snif_ingress_define_mirror_default_tagged_profile;
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
jr2_a0_dnx_data_snif_ingress_mirror_untagged_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_snif;
    int submodule_index = dnx_data_snif_submodule_ingress;
    int define_index = dnx_data_snif_ingress_define_mirror_untagged_profiles;
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
jr2_a0_dnx_data_snif_ingress_prob_max_val_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_snif;
    int submodule_index = dnx_data_snif_submodule_ingress;
    int define_index = dnx_data_snif_ingress_define_prob_max_val;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xffffff;

    
    define->data = 0xffffff;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_snif_ingress_crop_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_snif;
    int submodule_index = dnx_data_snif_submodule_ingress;
    int define_index = dnx_data_snif_ingress_define_crop_size;
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
jr2_a0_dnx_data_snif_ingress_keep_orig_dest_on_dsp_ext_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_snif;
    int submodule_index = dnx_data_snif_submodule_ingress;
    int define_index = dnx_data_snif_ingress_define_keep_orig_dest_on_dsp_ext;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_MIRROR_STAMP_SYS_ON_DSP_EXT;
    define->property.doc = 
        "\n"
        "stamping destination on FTM dsp extension\n"
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
jr2_a0_dnx_data_snif_egress_mirror_snoop_nof_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_snif;
    int submodule_index = dnx_data_snif_submodule_egress;
    int define_index = dnx_data_snif_egress_define_mirror_snoop_nof_profiles;
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
jr2_a0_dnx_data_snif_egress_mirror_nof_vlan_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_snif;
    int submodule_index = dnx_data_snif_submodule_egress;
    int define_index = dnx_data_snif_egress_define_mirror_nof_vlan_profiles;
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
jr2_a0_dnx_data_snif_egress_mirror_default_profile_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_snif;
    int submodule_index = dnx_data_snif_submodule_egress;
    int define_index = dnx_data_snif_egress_define_mirror_default_profile;
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
jr2_a0_dnx_data_snif_egress_prob_max_val_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_snif;
    int submodule_index = dnx_data_snif_submodule_egress;
    int define_index = dnx_data_snif_egress_define_prob_max_val;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = -1;

    
    define->data = -1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_snif_egress_crop_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_snif;
    int submodule_index = dnx_data_snif_submodule_egress;
    int define_index = dnx_data_snif_egress_define_crop_size;
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
jr2_a0_data_snif_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_snif;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_snif_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_snif_general_define_cud_type;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_snif_general_cud_type_set;

    

    
    
    submodule_index = dnx_data_snif_submodule_ingress;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_snif_ingress_define_nof_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_snif_ingress_nof_profiles_set;
    data_index = dnx_data_snif_ingress_define_mirror_nof_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_snif_ingress_mirror_nof_profiles_set;
    data_index = dnx_data_snif_ingress_define_nof_mirror_on_drop_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_snif_ingress_nof_mirror_on_drop_profiles_set;
    data_index = dnx_data_snif_ingress_define_mirror_nof_vlan_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_snif_ingress_mirror_nof_vlan_profiles_set;
    data_index = dnx_data_snif_ingress_define_mirror_default_tagged_profile;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_snif_ingress_mirror_default_tagged_profile_set;
    data_index = dnx_data_snif_ingress_define_mirror_untagged_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_snif_ingress_mirror_untagged_profiles_set;
    data_index = dnx_data_snif_ingress_define_prob_max_val;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_snif_ingress_prob_max_val_set;
    data_index = dnx_data_snif_ingress_define_crop_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_snif_ingress_crop_size_set;
    data_index = dnx_data_snif_ingress_define_keep_orig_dest_on_dsp_ext;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_snif_ingress_keep_orig_dest_on_dsp_ext_set;

    

    
    
    submodule_index = dnx_data_snif_submodule_egress;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_snif_egress_define_mirror_snoop_nof_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_snif_egress_mirror_snoop_nof_profiles_set;
    data_index = dnx_data_snif_egress_define_mirror_nof_vlan_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_snif_egress_mirror_nof_vlan_profiles_set;
    data_index = dnx_data_snif_egress_define_mirror_default_profile;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_snif_egress_mirror_default_profile_set;
    data_index = dnx_data_snif_egress_define_prob_max_val;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_snif_egress_prob_max_val_set;
    data_index = dnx_data_snif_egress_define_crop_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_snif_egress_crop_size_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

