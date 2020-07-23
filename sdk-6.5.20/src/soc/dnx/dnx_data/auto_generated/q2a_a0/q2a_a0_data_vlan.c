

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_VLAN

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_vlan.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>








static shr_error_e
q2a_a0_dnx_data_vlan_llvp_ext_vcp_en_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_vlan;
    int submodule_index = dnx_data_vlan_submodule_llvp;
    int define_index = dnx_data_vlan_llvp_define_ext_vcp_en;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_L2_PORT_TPID_CLASS_MODE;
    define->property.doc = 
        "\n"
        "Extended Vlan Classification enable\n"
        "Default: 0\n"
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




shr_error_e
q2a_a0_data_vlan_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_vlan;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_vlan_submodule_llvp;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_vlan_llvp_define_ext_vcp_en;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_vlan_llvp_ext_vcp_en_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

