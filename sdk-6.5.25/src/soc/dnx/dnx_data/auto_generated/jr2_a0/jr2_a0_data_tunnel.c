
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TUNNEL

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_tunnel.h>








static shr_error_e
jr2_a0_dnx_data_tunnel_udp_nof_udp_ports_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tunnel;
    int submodule_index = dnx_data_tunnel_submodule_udp;
    int define_index = dnx_data_tunnel_udp_define_nof_udp_ports_profiles;
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
jr2_a0_dnx_data_tunnel_udp_first_udp_ports_profile_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tunnel;
    int submodule_index = dnx_data_tunnel_submodule_udp;
    int define_index = dnx_data_tunnel_udp_define_first_udp_ports_profile;
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
jr2_a0_dnx_data_tunnel_tunnel_type_udp_tunnel_type_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tunnel;
    int submodule_index = dnx_data_tunnel_submodule_tunnel_type;
    int define_index = dnx_data_tunnel_tunnel_type_define_udp_tunnel_type;
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
jr2_a0_dnx_data_tunnel_tcam_table_has_raw_udp_tunnel_type_is_2nd_pass_key_fields_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tunnel;
    int submodule_index = dnx_data_tunnel_submodule_tcam_table;
    int define_index = dnx_data_tunnel_tcam_table_define_has_raw_udp_tunnel_type_is_2nd_pass_key_fields;
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
jr2_a0_data_tunnel_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_tunnel;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_tunnel_submodule_udp;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_tunnel_udp_define_nof_udp_ports_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tunnel_udp_nof_udp_ports_profiles_set;
    data_index = dnx_data_tunnel_udp_define_first_udp_ports_profile;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tunnel_udp_first_udp_ports_profile_set;

    

    
    
    submodule_index = dnx_data_tunnel_submodule_tunnel_type;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_tunnel_tunnel_type_define_udp_tunnel_type;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tunnel_tunnel_type_udp_tunnel_type_set;

    

    
    
    submodule_index = dnx_data_tunnel_submodule_tcam_table;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_tunnel_tcam_table_define_has_raw_udp_tunnel_type_is_2nd_pass_key_fields;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tunnel_tcam_table_has_raw_udp_tunnel_type_is_2nd_pass_key_fields_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

