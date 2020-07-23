

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_BIERDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>








static shr_error_e
jr2_a0_dnx_data_headers_ftmh_packet_size_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_packet_size_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 66;

    
    define->data = 66;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_ftmh_traffic_class_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_traffic_class_offset;
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
jr2_a0_dnx_data_headers_ftmh_src_sys_port_aggregate_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_src_sys_port_aggregate_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 47;

    
    define->data = 47;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_ftmh_pp_dsp_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_pp_dsp_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 39;

    
    define->data = 39;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_ftmh_drop_precedence_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_drop_precedence_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 37;

    
    define->data = 37;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_ftmh_tm_action_type_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_tm_action_type_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 35;

    
    define->data = 35;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_ftmh_tm_action_is_mc_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_tm_action_is_mc_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 34;

    
    define->data = 34;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_ftmh_outlif_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_outlif_offset;
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
jr2_a0_dnx_data_headers_ftmh_cni_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_cni_offset;
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
jr2_a0_dnx_data_headers_ftmh_ecn_enable_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_ecn_enable_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10;

    
    define->data = 10;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_ftmh_tm_profile_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_tm_profile_offset;
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
jr2_a0_dnx_data_headers_ftmh_visibility_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_visibility_offset;
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
jr2_a0_dnx_data_headers_ftmh_tsh_en_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_tsh_en_offset;
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
jr2_a0_dnx_data_headers_ftmh_internal_header_en_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_internal_header_en_offset;
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
jr2_a0_dnx_data_headers_ftmh_tm_dest_ext_present_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_tm_dest_ext_present_offset;
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
jr2_a0_dnx_data_headers_ftmh_ase_present_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_ase_present_offset;
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
jr2_a0_dnx_data_headers_ftmh_flow_id_ext_present_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_flow_id_ext_present_offset;
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
jr2_a0_dnx_data_headers_ftmh_bier_bfr_ext_present_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_bier_bfr_ext_present_offset;
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
jr2_a0_dnx_data_headers_ftmh_stack_route_history_bmp_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_stack_route_history_bmp_offset;
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
jr2_a0_dnx_data_headers_ftmh_tm_dst_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_tm_dst_offset;
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
jr2_a0_dnx_data_headers_ftmh_observation_info_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_observation_info_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 42;

    
    define->data = 42;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_ftmh_destination_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_destination_offset;
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
jr2_a0_dnx_data_headers_ftmh_src_sysport_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_src_sysport_offset;
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
jr2_a0_dnx_data_headers_ftmh_trajectory_trace_type_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_trajectory_trace_type_offset;
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
jr2_a0_dnx_data_headers_ftmh_port_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_port_offset;
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
jr2_a0_dnx_data_headers_ftmh_direction_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_direction_offset;
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
jr2_a0_dnx_data_headers_ftmh_session_id_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_session_id_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 21;

    
    define->data = 21;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_ftmh_truncated_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_truncated_offset;
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
jr2_a0_dnx_data_headers_ftmh_en_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_en_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 18;

    
    define->data = 18;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_ftmh_cos_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_cos_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 15;

    
    define->data = 15;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_ftmh_vlan_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_vlan_offset;
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
jr2_a0_dnx_data_headers_ftmh_erspan_type_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_erspan_type_offset;
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
jr2_a0_dnx_data_headers_ftmh_flow_id_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_flow_id_offset;
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
jr2_a0_dnx_data_headers_ftmh_flow_profile_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_flow_profile_offset;
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
jr2_a0_dnx_data_headers_ftmh_int_profile_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_int_profile_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 40;

    
    define->data = 40;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_ftmh_int_type_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_int_type_offset;
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
jr2_a0_dnx_data_headers_ftmh_packet_size_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_packet_size_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 14;

    
    define->data = 14;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_ftmh_traffic_class_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_traffic_class_bits;
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
jr2_a0_dnx_data_headers_ftmh_src_sys_port_aggregate_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_src_sys_port_aggregate_bits;
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
jr2_a0_dnx_data_headers_ftmh_pp_dsp_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_pp_dsp_bits;
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
jr2_a0_dnx_data_headers_ftmh_drop_precedence_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_drop_precedence_bits;
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
jr2_a0_dnx_data_headers_ftmh_tm_action_type_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_tm_action_type_bits;
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
jr2_a0_dnx_data_headers_ftmh_tm_action_is_mc_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_tm_action_is_mc_bits;
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
jr2_a0_dnx_data_headers_ftmh_outlif_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_outlif_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 22;

    
    define->data = 22;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_ftmh_cni_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_cni_bits;
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
jr2_a0_dnx_data_headers_ftmh_ecn_enable_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_ecn_enable_bits;
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
jr2_a0_dnx_data_headers_ftmh_tm_profile_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_tm_profile_bits;
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
jr2_a0_dnx_data_headers_ftmh_visibility_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_visibility_bits;
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
jr2_a0_dnx_data_headers_ftmh_tsh_en_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_tsh_en_bits;
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
jr2_a0_dnx_data_headers_ftmh_internal_header_en_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_internal_header_en_bits;
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
jr2_a0_dnx_data_headers_ftmh_tm_dest_ext_present_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_tm_dest_ext_present_bits;
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
jr2_a0_dnx_data_headers_ftmh_ase_present_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_ase_present_bits;
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
jr2_a0_dnx_data_headers_ftmh_flow_id_ext_present_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_flow_id_ext_present_bits;
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
jr2_a0_dnx_data_headers_ftmh_bier_bfr_ext_present_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_bier_bfr_ext_present_bits;
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
jr2_a0_dnx_data_headers_ftmh_tm_dst_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_tm_dst_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 21;

    
    define->data = 21;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_ftmh_observation_info_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_observation_info_bits;
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
jr2_a0_dnx_data_headers_ftmh_destination_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_destination_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 22;

    
    define->data = 22;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_ftmh_src_sysport_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_src_sysport_bits;
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
jr2_a0_dnx_data_headers_ftmh_trajectory_trace_type_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_trajectory_trace_type_bits;
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
jr2_a0_dnx_data_headers_ftmh_port_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_port_bits;
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
jr2_a0_dnx_data_headers_ftmh_direction_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_direction_bits;
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
jr2_a0_dnx_data_headers_ftmh_session_id_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_session_id_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10;

    
    define->data = 10;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_ftmh_truncated_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_truncated_bits;
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
jr2_a0_dnx_data_headers_ftmh_en_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_en_bits;
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
jr2_a0_dnx_data_headers_ftmh_cos_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_cos_bits;
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
jr2_a0_dnx_data_headers_ftmh_vlan_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_vlan_bits;
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
jr2_a0_dnx_data_headers_ftmh_erspan_type_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_erspan_type_bits;
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
jr2_a0_dnx_data_headers_ftmh_flow_id_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_flow_id_bits;
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
jr2_a0_dnx_data_headers_ftmh_flow_profile_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_flow_profile_bits;
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
jr2_a0_dnx_data_headers_ftmh_base_header_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_base_header_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10;

    
    define->data = 10;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_ftmh_ase_header_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_ase_header_size;
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
jr2_a0_dnx_data_headers_ftmh_tm_dst_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_tm_dst_size;
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
jr2_a0_dnx_data_headers_ftmh_flow_id_header_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_flow_id_header_size;
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
jr2_a0_dnx_data_headers_ftmh_int_profile_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_int_profile_bits;
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
jr2_a0_dnx_data_headers_ftmh_int_type_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_int_type_bits;
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
jr2_a0_dnx_data_headers_ftmh_add_dsp_ext_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh;
    int define_index = dnx_data_headers_ftmh_define_add_dsp_ext;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FTMH_DSP_EXTENSION_ADD;
    define->property.doc = 
        "\n"
        "adding dsp extension for the ftmh\n"
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
jr2_a0_dnx_data_headers_tsh_base_header_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_tsh;
    int define_index = dnx_data_headers_tsh_define_base_header_size;
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
jr2_a0_dnx_data_headers_otsh_base_header_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_otsh;
    int define_index = dnx_data_headers_otsh_define_base_header_size;
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
jr2_a0_dnx_data_headers_optimized_ftmh_ftmh_opt_parse_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_optimized_ftmh;
    int feature_index = dnx_data_headers_optimized_ftmh_ftmh_opt_parse;
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
jr2_a0_dnx_data_headers_optimized_ftmh_base_header_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_optimized_ftmh;
    int define_index = dnx_data_headers_optimized_ftmh_define_base_header_size;
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
jr2_a0_dnx_data_headers_optimized_ftmh_packet_size_7_0_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_optimized_ftmh;
    int define_index = dnx_data_headers_optimized_ftmh_define_packet_size_7_0_offset;
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
jr2_a0_dnx_data_headers_optimized_ftmh_tm_action_is_mc_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_optimized_ftmh;
    int define_index = dnx_data_headers_optimized_ftmh_define_tm_action_is_mc_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 23;

    
    define->data = 23;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_optimized_ftmh_packet_size_8_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_optimized_ftmh;
    int define_index = dnx_data_headers_optimized_ftmh_define_packet_size_8_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 22;

    
    define->data = 22;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_optimized_ftmh_user_defined_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_optimized_ftmh;
    int define_index = dnx_data_headers_optimized_ftmh_define_user_defined_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 19;

    
    define->data = 19;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_optimized_ftmh_fap_id_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_optimized_ftmh;
    int define_index = dnx_data_headers_optimized_ftmh_define_fap_id_offset;
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
jr2_a0_dnx_data_headers_optimized_ftmh_pp_dsp_or_mc_id_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_optimized_ftmh;
    int define_index = dnx_data_headers_optimized_ftmh_define_pp_dsp_or_mc_id_offset;
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
jr2_a0_dnx_data_headers_internal_header_visibilty_mode_global_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int feature_index = dnx_data_headers_internal_header_visibilty_mode_global;
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
jr2_a0_dnx_data_headers_internal_parsing_start_type_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int define_index = dnx_data_headers_internal_define_parsing_start_type_offset;
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
jr2_a0_dnx_data_headers_internal_parsing_start_type_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int define_index = dnx_data_headers_internal_define_parsing_start_type_bits;
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
jr2_a0_dnx_data_headers_internal_lif_ext_type_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int define_index = dnx_data_headers_internal_define_lif_ext_type_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 13;

    
    define->data = 13;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_internal_lif_ext_type_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int define_index = dnx_data_headers_internal_define_lif_ext_type_bits;
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
jr2_a0_dnx_data_headers_internal_fai_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int define_index = dnx_data_headers_internal_define_fai_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 86;

    
    define->data = 86;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_internal_fai_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int define_index = dnx_data_headers_internal_define_fai_bits;
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
jr2_a0_dnx_data_headers_internal_tail_edit_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int define_index = dnx_data_headers_internal_define_tail_edit_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 83;

    
    define->data = 83;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_internal_tail_edit_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int define_index = dnx_data_headers_internal_define_tail_edit_bits;
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
jr2_a0_dnx_data_headers_internal_forward_domain_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int define_index = dnx_data_headers_internal_define_forward_domain_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 57;

    
    define->data = 57;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_internal_forward_domain_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int define_index = dnx_data_headers_internal_define_forward_domain_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 18;

    
    define->data = 18;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_internal_inlif_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int define_index = dnx_data_headers_internal_define_inlif_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 35;

    
    define->data = 35;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_internal_inlif_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int define_index = dnx_data_headers_internal_define_inlif_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 22;

    
    define->data = 22;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_internal_fhei_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int define_index = dnx_data_headers_internal_define_fhei_size;
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
jr2_a0_dnx_data_headers_internal_fhei_size_sz0_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int define_index = dnx_data_headers_internal_define_fhei_size_sz0;
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
jr2_a0_dnx_data_headers_internal_fhei_size_sz1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int define_index = dnx_data_headers_internal_define_fhei_size_sz1;
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
jr2_a0_dnx_data_headers_internal_fhei_size_sz2_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int define_index = dnx_data_headers_internal_define_fhei_size_sz2;
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
jr2_a0_dnx_data_headers_internal_fhei_size_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int define_index = dnx_data_headers_internal_define_fhei_size_offset;
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
jr2_a0_dnx_data_headers_internal_fhei_size_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int define_index = dnx_data_headers_internal_define_fhei_size_bits;
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
jr2_a0_dnx_data_headers_internal_learning_ext_present_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int define_index = dnx_data_headers_internal_define_learning_ext_present_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 18;

    
    define->data = 18;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_internal_learning_ext_present_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int define_index = dnx_data_headers_internal_define_learning_ext_present_bits;
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
jr2_a0_dnx_data_headers_internal_learning_ext_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int define_index = dnx_data_headers_internal_define_learning_ext_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 19;

    
    define->data = 19;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_internal_ttl_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int define_index = dnx_data_headers_internal_define_ttl_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 19;

    
    define->data = 19;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_internal_ttl_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int define_index = dnx_data_headers_internal_define_ttl_bits;
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
jr2_a0_dnx_data_headers_internal_eth_rm_fli_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int define_index = dnx_data_headers_internal_define_eth_rm_fli_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 90;

    
    define->data = 90;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_internal_eth_rm_fli_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int define_index = dnx_data_headers_internal_define_eth_rm_fli_bits;
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
jr2_a0_dnx_data_headers_internal_eth_rm_pso_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int define_index = dnx_data_headers_internal_define_eth_rm_pso_offset;
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
jr2_a0_dnx_data_headers_internal_eth_rm_pso_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int define_index = dnx_data_headers_internal_define_eth_rm_pso_bits;
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
jr2_a0_dnx_data_headers_internal_cfg_fai_unknown_address_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int define_index = dnx_data_headers_internal_define_cfg_fai_unknown_address_enable;
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
jr2_a0_dnx_data_headers_internal_legacy_fhei_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal_legacy;
    int define_index = dnx_data_headers_internal_legacy_define_fhei_size;
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
jr2_a0_dnx_data_headers_internal_legacy_fhei_size_sz0_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal_legacy;
    int define_index = dnx_data_headers_internal_legacy_define_fhei_size_sz0;
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
jr2_a0_dnx_data_headers_internal_legacy_fhei_size_sz1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal_legacy;
    int define_index = dnx_data_headers_internal_legacy_define_fhei_size_sz1;
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
jr2_a0_dnx_data_headers_internal_legacy_fhei_size_sz2_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal_legacy;
    int define_index = dnx_data_headers_internal_legacy_define_fhei_size_sz2;
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
jr2_a0_dnx_data_headers_internal_legacy_inlif_inrif_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal_legacy;
    int define_index = dnx_data_headers_internal_legacy_define_inlif_inrif_offset;
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
jr2_a0_dnx_data_headers_internal_legacy_inlif_inrif_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal_legacy;
    int define_index = dnx_data_headers_internal_legacy_define_inlif_inrif_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 18;

    
    define->data = 18;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_internal_legacy_unknown_address_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal_legacy;
    int define_index = dnx_data_headers_internal_legacy_define_unknown_address_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 35;

    
    define->data = 35;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_internal_legacy_unknown_address_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal_legacy;
    int define_index = dnx_data_headers_internal_legacy_define_unknown_address_bits;
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
jr2_a0_dnx_data_headers_internal_legacy_forwarding_header_offset_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal_legacy;
    int define_index = dnx_data_headers_internal_legacy_define_forwarding_header_offset_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 41;

    
    define->data = 41;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_internal_legacy_forwarding_header_offset_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal_legacy;
    int define_index = dnx_data_headers_internal_legacy_define_forwarding_header_offset_bits;
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
jr2_a0_dnx_data_headers_internal_legacy_forward_code_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal_legacy;
    int define_index = dnx_data_headers_internal_legacy_define_forward_code_offset;
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
jr2_a0_dnx_data_headers_internal_legacy_forward_code_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal_legacy;
    int define_index = dnx_data_headers_internal_legacy_define_forward_code_bits;
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
jr2_a0_dnx_data_headers_internal_legacy_fhei_size_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal_legacy;
    int define_index = dnx_data_headers_internal_legacy_define_fhei_size_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 52;

    
    define->data = 52;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_internal_legacy_fhei_size_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal_legacy;
    int define_index = dnx_data_headers_internal_legacy_define_fhei_size_bits;
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
jr2_a0_dnx_data_headers_internal_legacy_learning_ext_present_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal_legacy;
    int define_index = dnx_data_headers_internal_legacy_define_learning_ext_present_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 54;

    
    define->data = 54;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_internal_legacy_learning_ext_present_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal_legacy;
    int define_index = dnx_data_headers_internal_legacy_define_learning_ext_present_bits;
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
jr2_a0_dnx_data_headers_internal_legacy_learning_ext_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal_legacy;
    int define_index = dnx_data_headers_internal_legacy_define_learning_ext_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 40;

    
    define->data = 40;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_internal_legacy_eei_ext_present_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal_legacy;
    int define_index = dnx_data_headers_internal_legacy_define_eei_ext_present_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 55;

    
    define->data = 55;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_internal_legacy_eei_ext_present_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal_legacy;
    int define_index = dnx_data_headers_internal_legacy_define_eei_ext_present_bits;
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
jr2_a0_dnx_data_headers_fhei_sz1_trap_sniff_fhei_type_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_fhei_sz1_trap_sniff;
    int define_index = dnx_data_headers_fhei_sz1_trap_sniff_define_fhei_type_offset;
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
jr2_a0_dnx_data_headers_fhei_sz1_trap_sniff_fhei_type_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_fhei_sz1_trap_sniff;
    int define_index = dnx_data_headers_fhei_sz1_trap_sniff_define_fhei_type_bits;
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
jr2_a0_dnx_data_headers_fhei_sz1_trap_sniff_code_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_fhei_sz1_trap_sniff;
    int define_index = dnx_data_headers_fhei_sz1_trap_sniff_define_code_offset;
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
jr2_a0_dnx_data_headers_fhei_sz1_trap_sniff_code_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_fhei_sz1_trap_sniff;
    int define_index = dnx_data_headers_fhei_sz1_trap_sniff_define_code_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 9;

    
    define->data = 9;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_fhei_sz1_trap_sniff_qualifier_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_fhei_sz1_trap_sniff;
    int define_index = dnx_data_headers_fhei_sz1_trap_sniff_define_qualifier_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 13;

    
    define->data = 13;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_fhei_sz1_trap_sniff_qualifier_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_fhei_sz1_trap_sniff;
    int define_index = dnx_data_headers_fhei_sz1_trap_sniff_define_qualifier_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 27;

    
    define->data = 27;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_headers_fhei_trap_sniff_legacy_code_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_fhei_trap_sniff_legacy;
    int define_index = dnx_data_headers_fhei_trap_sniff_legacy_define_code_offset;
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
jr2_a0_dnx_data_headers_fhei_trap_sniff_legacy_code_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_fhei_trap_sniff_legacy;
    int define_index = dnx_data_headers_fhei_trap_sniff_legacy_define_code_bits;
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
jr2_a0_dnx_data_headers_fhei_trap_sniff_legacy_qualifier_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_fhei_trap_sniff_legacy;
    int define_index = dnx_data_headers_fhei_trap_sniff_legacy_define_qualifier_offset;
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
jr2_a0_dnx_data_headers_fhei_trap_sniff_legacy_qualifier_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_fhei_trap_sniff_legacy;
    int define_index = dnx_data_headers_fhei_trap_sniff_legacy_define_qualifier_bits;
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
jr2_a0_dnx_data_headers_udh_data_type0_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_udh;
    int define_index = dnx_data_headers_udh_define_data_type0_offset;
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
jr2_a0_dnx_data_headers_udh_data_type1_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_udh;
    int define_index = dnx_data_headers_udh_define_data_type1_offset;
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
jr2_a0_dnx_data_headers_udh_data_type2_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_udh;
    int define_index = dnx_data_headers_udh_define_data_type2_offset;
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
jr2_a0_dnx_data_headers_udh_data_type3_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_udh;
    int define_index = dnx_data_headers_udh_define_data_type3_offset;
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
jr2_a0_dnx_data_headers_udh_data_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_udh;
    int define_index = dnx_data_headers_udh_define_data_offset;
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
jr2_a0_dnx_data_headers_udh_data_type0_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_udh;
    int define_index = dnx_data_headers_udh_define_data_type0_bits;
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
jr2_a0_dnx_data_headers_udh_data_type1_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_udh;
    int define_index = dnx_data_headers_udh_define_data_type1_bits;
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
jr2_a0_dnx_data_headers_udh_data_type2_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_udh;
    int define_index = dnx_data_headers_udh_define_data_type2_bits;
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
jr2_a0_dnx_data_headers_udh_data_type3_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_udh;
    int define_index = dnx_data_headers_udh_define_data_type3_bits;
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
jr2_a0_dnx_data_headers_udh_data_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_udh;
    int define_index = dnx_data_headers_udh_define_data_bits;
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
jr2_a0_dnx_data_headers_udh_base_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_udh;
    int define_index = dnx_data_headers_udh_define_base_size;
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
jr2_a0_dnx_data_headers_ftmh_legacy_packet_size_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_packet_size_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 58;

    
    define->data = 58;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_ftmh_legacy_traffic_class_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_traffic_class_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 55;

    
    define->data = 55;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_ftmh_legacy_src_sys_port_aggregate_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_src_sys_port_aggregate_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 39;

    
    define->data = 39;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_ftmh_legacy_pp_dsp_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_pp_dsp_offset;
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
jr2_a0_dnx_data_headers_ftmh_legacy_drop_precedence_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_drop_precedence_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 29;

    
    define->data = 29;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_ftmh_legacy_tm_action_type_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_tm_action_type_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 27;

    
    define->data = 27;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_ftmh_legacy_otsh_en_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_otsh_en_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 26;

    
    define->data = 26;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_ftmh_legacy_internal_header_en_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_internal_header_en_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 25;

    
    define->data = 25;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_ftmh_legacy_outbound_mirr_disable_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_outbound_mirr_disable_offset;
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
jr2_a0_dnx_data_headers_ftmh_legacy_tm_action_is_mc_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_tm_action_is_mc_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 23;

    
    define->data = 23;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_ftmh_legacy_outlif_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_outlif_offset;
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
jr2_a0_dnx_data_headers_ftmh_legacy_dsp_ext_present_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_dsp_ext_present_offset;
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
jr2_a0_dnx_data_headers_ftmh_legacy_cni_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_cni_offset;
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
jr2_a0_dnx_data_headers_ftmh_legacy_ecn_enable_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_ecn_enable_offset;
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
jr2_a0_dnx_data_headers_ftmh_legacy_exclude_source_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_exclude_source_offset;
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
jr2_a0_dnx_data_headers_ftmh_legacy_stack_route_history_bmp_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_stack_route_history_bmp_offset;
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
jr2_a0_dnx_data_headers_ftmh_legacy_dsp_ext_header_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_dsp_ext_header_offset;
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
jr2_a0_dnx_data_headers_ftmh_legacy_packet_size_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_packet_size_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 14;

    
    define->data = 14;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_ftmh_legacy_traffic_class_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_traffic_class_bits;
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
jr2_a0_dnx_data_headers_ftmh_legacy_src_sys_port_aggregate_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_src_sys_port_aggregate_bits;
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
jr2_a0_dnx_data_headers_ftmh_legacy_pp_dsp_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_pp_dsp_bits;
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
jr2_a0_dnx_data_headers_ftmh_legacy_drop_precedence_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_drop_precedence_bits;
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
jr2_a0_dnx_data_headers_ftmh_legacy_tm_action_type_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_tm_action_type_bits;
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
jr2_a0_dnx_data_headers_ftmh_legacy_otsh_en_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_otsh_en_bits;
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
jr2_a0_dnx_data_headers_ftmh_legacy_internal_header_en_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_internal_header_en_bits;
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
jr2_a0_dnx_data_headers_ftmh_legacy_outbound_mirr_disable_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_outbound_mirr_disable_bits;
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
jr2_a0_dnx_data_headers_ftmh_legacy_tm_action_is_mc_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_tm_action_is_mc_bits;
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
jr2_a0_dnx_data_headers_ftmh_legacy_outlif_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_outlif_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 19;

    
    define->data = 19;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_ftmh_legacy_dsp_ext_present_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_dsp_ext_present_bits;
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
jr2_a0_dnx_data_headers_ftmh_legacy_cni_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_cni_bits;
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
jr2_a0_dnx_data_headers_ftmh_legacy_ecn_enable_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_ecn_enable_bits;
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
jr2_a0_dnx_data_headers_ftmh_legacy_exclude_source_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_exclude_source_bits;
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
jr2_a0_dnx_data_headers_ftmh_legacy_dsp_ext_header_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_dsp_ext_header_bits;
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
jr2_a0_dnx_data_headers_ftmh_legacy_base_header_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_base_header_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 9;

    
    define->data = 9;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_ftmh_legacy_dsp_ext_header_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    int define_index = dnx_data_headers_ftmh_legacy_define_dsp_ext_header_size;
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
jr2_a0_dnx_data_headers_otsh_legacy_type_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_otsh_legacy;
    int define_index = dnx_data_headers_otsh_legacy_define_type_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 46;

    
    define->data = 46;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_otsh_legacy_type_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_otsh_legacy;
    int define_index = dnx_data_headers_otsh_legacy_define_type_bits;
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
jr2_a0_dnx_data_headers_otsh_legacy_oam_sub_type_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_otsh_legacy;
    int define_index = dnx_data_headers_otsh_legacy_define_oam_sub_type_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 43;

    
    define->data = 43;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_otsh_legacy_oam_sub_type_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_otsh_legacy;
    int define_index = dnx_data_headers_otsh_legacy_define_oam_sub_type_bits;
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
jr2_a0_dnx_data_headers_otsh_legacy_mep_type_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_otsh_legacy;
    int define_index = dnx_data_headers_otsh_legacy_define_mep_type_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 42;

    
    define->data = 42;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_otsh_legacy_mep_type_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_otsh_legacy;
    int define_index = dnx_data_headers_otsh_legacy_define_mep_type_bits;
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
jr2_a0_dnx_data_headers_otsh_legacy_tp_command_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_otsh_legacy;
    int define_index = dnx_data_headers_otsh_legacy_define_tp_command_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 43;

    
    define->data = 43;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_otsh_legacy_tp_command_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_otsh_legacy;
    int define_index = dnx_data_headers_otsh_legacy_define_tp_command_bits;
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
jr2_a0_dnx_data_headers_otsh_legacy_ts_encapsulation_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_otsh_legacy;
    int define_index = dnx_data_headers_otsh_legacy_define_ts_encapsulation_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 42;

    
    define->data = 42;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_otsh_legacy_ts_encapsulation_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_otsh_legacy;
    int define_index = dnx_data_headers_otsh_legacy_define_ts_encapsulation_bits;
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
jr2_a0_dnx_data_headers_otsh_legacy_oam_ts_data_0_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_otsh_legacy;
    int define_index = dnx_data_headers_otsh_legacy_define_oam_ts_data_0_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 40;

    
    define->data = 40;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_otsh_legacy_oam_ts_data_0_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_otsh_legacy;
    int define_index = dnx_data_headers_otsh_legacy_define_oam_ts_data_0_bits;
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
jr2_a0_dnx_data_headers_otsh_legacy_oam_ts_data_1_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_otsh_legacy;
    int define_index = dnx_data_headers_otsh_legacy_define_oam_ts_data_1_offset;
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
jr2_a0_dnx_data_headers_otsh_legacy_oam_ts_data_1_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_otsh_legacy;
    int define_index = dnx_data_headers_otsh_legacy_define_oam_ts_data_1_bits;
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
jr2_a0_dnx_data_headers_otsh_legacy_offset_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_otsh_legacy;
    int define_index = dnx_data_headers_otsh_legacy_define_offset_offset;
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
jr2_a0_dnx_data_headers_otsh_legacy_offset_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_otsh_legacy;
    int define_index = dnx_data_headers_otsh_legacy_define_offset_bits;
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
jr2_a0_dnx_data_headers_system_headers_system_headers_mode_jericho_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_system_headers;
    int define_index = dnx_data_headers_system_headers_define_system_headers_mode_jericho;
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
jr2_a0_dnx_data_headers_system_headers_system_headers_mode_jericho2_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_system_headers;
    int define_index = dnx_data_headers_system_headers_define_system_headers_mode_jericho2;
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
jr2_a0_dnx_data_headers_system_headers_crc_size_counted_in_pkt_len_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_system_headers;
    int define_index = dnx_data_headers_system_headers_define_crc_size_counted_in_pkt_len;
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
jr2_a0_dnx_data_headers_system_headers_system_headers_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_system_headers;
    int define_index = dnx_data_headers_system_headers_define_system_headers_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_SYSTEM_HEADERS_MODE;
    define->property.doc = 
        "\n"
        "System headers mode that the device supports, possible values:\n"
        "0x0 - Jericho-mode - used for Jericho/QMX/QAX/QUX mode\n"
        "0x1 - Jericho2-mode - 0x1 used for Jericho 2 mode\n"
        "Default: 0x1.\n"
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
jr2_a0_dnx_data_headers_system_headers_jr_mode_ftmh_lb_key_ext_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_system_headers;
    int define_index = dnx_data_headers_system_headers_define_jr_mode_ftmh_lb_key_ext_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_SYSTEM_FTMH_LOAD_BALANCING_EXT_MODE;
    define->property.doc = 
        "\n"
        "ftmh load balance key extensiton mode in JR mode:\n"
        "DISABLED - FTMH load balancing extension is disabled\n"
        "ENABLED -  FTMH load balancing extension is enabled.\n"
        "Default: DISABLED\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "DISABLED";
    define->property.mapping[0].val = 0;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "ENABLED";
    define->property.mapping[1].val = 1;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_system_headers_jr_mode_ftmh_stacking_ext_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_system_headers;
    int define_index = dnx_data_headers_system_headers_define_jr_mode_ftmh_stacking_ext_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_STACKING_EXTENSION_ENABLE;
    define->property.doc = 
        "\n"
        "ftmh stacking extension mode in JR sys-headers mode:\n"
        "0x0 - FTMH stacking extension is disabled\n"
        "0x1 - FTMH stacking extension is enabled\n"
        "Defaule: DISABLED\n"
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
jr2_a0_dnx_data_headers_system_headers_udh_base_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_system_headers;
    int define_index = dnx_data_headers_system_headers_define_udh_base_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_SYSTEM_HEADERS_MODE;
    define->property.doc = 
        "\n"
        "UDH Base header\n"
        "JR2 mode - Standard UDH Base with 1B\n"
        "JR1 mode - No UDH Base header.\n"
        "Default: JR2 mode\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "1";
    define->property.mapping[0].val = 8;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "0";
    define->property.mapping[1].val = 0;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_headers_system_headers_jr_mode_ftmh_oam_ts_right_shift_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_system_headers;
    int define_index = dnx_data_headers_system_headers_define_jr_mode_ftmh_oam_ts_right_shift;
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
jr2_a0_dnx_data_headers_otmh_otmh_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_otmh;
    int feature_index = dnx_data_headers_otmh_otmh_enable;
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
jr2_a0_dnx_data_headers_pph_pph_vsi_selection_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_pph;
    int define_index = dnx_data_headers_pph_define_pph_vsi_selection_size;
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
jr2_a0_dnx_data_headers_general_fwd_header_offset_calc_with_egress_pars_idx_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_general;
    int feature_index = dnx_data_headers_general_fwd_header_offset_calc_with_egress_pars_idx;
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
jr2_a0_dnx_data_headers_general_jericho_mode_sys_header_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_general;
    int feature_index = dnx_data_headers_general_jericho_mode_sys_header_is_supported;
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
jr2_a0_dnx_data_headers_feature_fhei_mpls_cmd_from_eei_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_feature;
    int feature_index = dnx_data_headers_feature_fhei_mpls_cmd_from_eei;
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
jr2_a0_dnx_data_headers_feature_unknown_address_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_feature;
    int feature_index = dnx_data_headers_feature_unknown_address;
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
jr2_a0_dnx_data_headers_feature_tsh_ext_with_flow_id_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_feature;
    int feature_index = dnx_data_headers_feature_tsh_ext_with_flow_id;
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
jr2_a0_dnx_data_headers_feature_fhei_mpls_swap_remark_profile_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_feature;
    int define_index = dnx_data_headers_feature_define_fhei_mpls_swap_remark_profile_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5;

    
    define->data = 5;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
jr2_a0_data_headers_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_headers;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_headers_submodule_ftmh;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_headers_ftmh_define_packet_size_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_packet_size_offset_set;
    data_index = dnx_data_headers_ftmh_define_traffic_class_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_traffic_class_offset_set;
    data_index = dnx_data_headers_ftmh_define_src_sys_port_aggregate_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_src_sys_port_aggregate_offset_set;
    data_index = dnx_data_headers_ftmh_define_pp_dsp_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_pp_dsp_offset_set;
    data_index = dnx_data_headers_ftmh_define_drop_precedence_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_drop_precedence_offset_set;
    data_index = dnx_data_headers_ftmh_define_tm_action_type_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_tm_action_type_offset_set;
    data_index = dnx_data_headers_ftmh_define_tm_action_is_mc_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_tm_action_is_mc_offset_set;
    data_index = dnx_data_headers_ftmh_define_outlif_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_outlif_offset_set;
    data_index = dnx_data_headers_ftmh_define_cni_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_cni_offset_set;
    data_index = dnx_data_headers_ftmh_define_ecn_enable_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_ecn_enable_offset_set;
    data_index = dnx_data_headers_ftmh_define_tm_profile_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_tm_profile_offset_set;
    data_index = dnx_data_headers_ftmh_define_visibility_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_visibility_offset_set;
    data_index = dnx_data_headers_ftmh_define_tsh_en_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_tsh_en_offset_set;
    data_index = dnx_data_headers_ftmh_define_internal_header_en_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_internal_header_en_offset_set;
    data_index = dnx_data_headers_ftmh_define_tm_dest_ext_present_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_tm_dest_ext_present_offset_set;
    data_index = dnx_data_headers_ftmh_define_ase_present_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_ase_present_offset_set;
    data_index = dnx_data_headers_ftmh_define_flow_id_ext_present_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_flow_id_ext_present_offset_set;
    data_index = dnx_data_headers_ftmh_define_bier_bfr_ext_present_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_bier_bfr_ext_present_offset_set;
    data_index = dnx_data_headers_ftmh_define_stack_route_history_bmp_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_stack_route_history_bmp_offset_set;
    data_index = dnx_data_headers_ftmh_define_tm_dst_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_tm_dst_offset_set;
    data_index = dnx_data_headers_ftmh_define_observation_info_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_observation_info_offset_set;
    data_index = dnx_data_headers_ftmh_define_destination_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_destination_offset_set;
    data_index = dnx_data_headers_ftmh_define_src_sysport_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_src_sysport_offset_set;
    data_index = dnx_data_headers_ftmh_define_trajectory_trace_type_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_trajectory_trace_type_offset_set;
    data_index = dnx_data_headers_ftmh_define_port_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_port_offset_set;
    data_index = dnx_data_headers_ftmh_define_direction_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_direction_offset_set;
    data_index = dnx_data_headers_ftmh_define_session_id_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_session_id_offset_set;
    data_index = dnx_data_headers_ftmh_define_truncated_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_truncated_offset_set;
    data_index = dnx_data_headers_ftmh_define_en_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_en_offset_set;
    data_index = dnx_data_headers_ftmh_define_cos_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_cos_offset_set;
    data_index = dnx_data_headers_ftmh_define_vlan_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_vlan_offset_set;
    data_index = dnx_data_headers_ftmh_define_erspan_type_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_erspan_type_offset_set;
    data_index = dnx_data_headers_ftmh_define_flow_id_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_flow_id_offset_set;
    data_index = dnx_data_headers_ftmh_define_flow_profile_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_flow_profile_offset_set;
    data_index = dnx_data_headers_ftmh_define_int_profile_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_int_profile_offset_set;
    data_index = dnx_data_headers_ftmh_define_int_type_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_int_type_offset_set;
    data_index = dnx_data_headers_ftmh_define_packet_size_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_packet_size_bits_set;
    data_index = dnx_data_headers_ftmh_define_traffic_class_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_traffic_class_bits_set;
    data_index = dnx_data_headers_ftmh_define_src_sys_port_aggregate_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_src_sys_port_aggregate_bits_set;
    data_index = dnx_data_headers_ftmh_define_pp_dsp_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_pp_dsp_bits_set;
    data_index = dnx_data_headers_ftmh_define_drop_precedence_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_drop_precedence_bits_set;
    data_index = dnx_data_headers_ftmh_define_tm_action_type_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_tm_action_type_bits_set;
    data_index = dnx_data_headers_ftmh_define_tm_action_is_mc_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_tm_action_is_mc_bits_set;
    data_index = dnx_data_headers_ftmh_define_outlif_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_outlif_bits_set;
    data_index = dnx_data_headers_ftmh_define_cni_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_cni_bits_set;
    data_index = dnx_data_headers_ftmh_define_ecn_enable_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_ecn_enable_bits_set;
    data_index = dnx_data_headers_ftmh_define_tm_profile_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_tm_profile_bits_set;
    data_index = dnx_data_headers_ftmh_define_visibility_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_visibility_bits_set;
    data_index = dnx_data_headers_ftmh_define_tsh_en_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_tsh_en_bits_set;
    data_index = dnx_data_headers_ftmh_define_internal_header_en_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_internal_header_en_bits_set;
    data_index = dnx_data_headers_ftmh_define_tm_dest_ext_present_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_tm_dest_ext_present_bits_set;
    data_index = dnx_data_headers_ftmh_define_ase_present_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_ase_present_bits_set;
    data_index = dnx_data_headers_ftmh_define_flow_id_ext_present_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_flow_id_ext_present_bits_set;
    data_index = dnx_data_headers_ftmh_define_bier_bfr_ext_present_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_bier_bfr_ext_present_bits_set;
    data_index = dnx_data_headers_ftmh_define_tm_dst_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_tm_dst_bits_set;
    data_index = dnx_data_headers_ftmh_define_observation_info_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_observation_info_bits_set;
    data_index = dnx_data_headers_ftmh_define_destination_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_destination_bits_set;
    data_index = dnx_data_headers_ftmh_define_src_sysport_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_src_sysport_bits_set;
    data_index = dnx_data_headers_ftmh_define_trajectory_trace_type_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_trajectory_trace_type_bits_set;
    data_index = dnx_data_headers_ftmh_define_port_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_port_bits_set;
    data_index = dnx_data_headers_ftmh_define_direction_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_direction_bits_set;
    data_index = dnx_data_headers_ftmh_define_session_id_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_session_id_bits_set;
    data_index = dnx_data_headers_ftmh_define_truncated_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_truncated_bits_set;
    data_index = dnx_data_headers_ftmh_define_en_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_en_bits_set;
    data_index = dnx_data_headers_ftmh_define_cos_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_cos_bits_set;
    data_index = dnx_data_headers_ftmh_define_vlan_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_vlan_bits_set;
    data_index = dnx_data_headers_ftmh_define_erspan_type_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_erspan_type_bits_set;
    data_index = dnx_data_headers_ftmh_define_flow_id_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_flow_id_bits_set;
    data_index = dnx_data_headers_ftmh_define_flow_profile_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_flow_profile_bits_set;
    data_index = dnx_data_headers_ftmh_define_base_header_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_base_header_size_set;
    data_index = dnx_data_headers_ftmh_define_ase_header_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_ase_header_size_set;
    data_index = dnx_data_headers_ftmh_define_tm_dst_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_tm_dst_size_set;
    data_index = dnx_data_headers_ftmh_define_flow_id_header_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_flow_id_header_size_set;
    data_index = dnx_data_headers_ftmh_define_int_profile_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_int_profile_bits_set;
    data_index = dnx_data_headers_ftmh_define_int_type_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_int_type_bits_set;
    data_index = dnx_data_headers_ftmh_define_add_dsp_ext;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_add_dsp_ext_set;

    

    
    
    submodule_index = dnx_data_headers_submodule_tsh;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_headers_tsh_define_base_header_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_tsh_base_header_size_set;

    

    
    
    submodule_index = dnx_data_headers_submodule_otsh;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_headers_otsh_define_base_header_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_otsh_base_header_size_set;

    

    
    
    submodule_index = dnx_data_headers_submodule_optimized_ftmh;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_headers_optimized_ftmh_define_base_header_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_optimized_ftmh_base_header_size_set;
    data_index = dnx_data_headers_optimized_ftmh_define_packet_size_7_0_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_optimized_ftmh_packet_size_7_0_offset_set;
    data_index = dnx_data_headers_optimized_ftmh_define_tm_action_is_mc_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_optimized_ftmh_tm_action_is_mc_offset_set;
    data_index = dnx_data_headers_optimized_ftmh_define_packet_size_8_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_optimized_ftmh_packet_size_8_offset_set;
    data_index = dnx_data_headers_optimized_ftmh_define_user_defined_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_optimized_ftmh_user_defined_offset_set;
    data_index = dnx_data_headers_optimized_ftmh_define_fap_id_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_optimized_ftmh_fap_id_offset_set;
    data_index = dnx_data_headers_optimized_ftmh_define_pp_dsp_or_mc_id_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_optimized_ftmh_pp_dsp_or_mc_id_offset_set;

    
    data_index = dnx_data_headers_optimized_ftmh_ftmh_opt_parse;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_headers_optimized_ftmh_ftmh_opt_parse_set;

    
    
    submodule_index = dnx_data_headers_submodule_internal;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_headers_internal_define_parsing_start_type_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_parsing_start_type_offset_set;
    data_index = dnx_data_headers_internal_define_parsing_start_type_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_parsing_start_type_bits_set;
    data_index = dnx_data_headers_internal_define_lif_ext_type_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_lif_ext_type_offset_set;
    data_index = dnx_data_headers_internal_define_lif_ext_type_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_lif_ext_type_bits_set;
    data_index = dnx_data_headers_internal_define_fai_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_fai_offset_set;
    data_index = dnx_data_headers_internal_define_fai_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_fai_bits_set;
    data_index = dnx_data_headers_internal_define_tail_edit_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_tail_edit_offset_set;
    data_index = dnx_data_headers_internal_define_tail_edit_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_tail_edit_bits_set;
    data_index = dnx_data_headers_internal_define_forward_domain_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_forward_domain_offset_set;
    data_index = dnx_data_headers_internal_define_forward_domain_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_forward_domain_bits_set;
    data_index = dnx_data_headers_internal_define_inlif_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_inlif_offset_set;
    data_index = dnx_data_headers_internal_define_inlif_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_inlif_bits_set;
    data_index = dnx_data_headers_internal_define_fhei_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_fhei_size_set;
    data_index = dnx_data_headers_internal_define_fhei_size_sz0;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_fhei_size_sz0_set;
    data_index = dnx_data_headers_internal_define_fhei_size_sz1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_fhei_size_sz1_set;
    data_index = dnx_data_headers_internal_define_fhei_size_sz2;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_fhei_size_sz2_set;
    data_index = dnx_data_headers_internal_define_fhei_size_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_fhei_size_offset_set;
    data_index = dnx_data_headers_internal_define_fhei_size_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_fhei_size_bits_set;
    data_index = dnx_data_headers_internal_define_learning_ext_present_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_learning_ext_present_offset_set;
    data_index = dnx_data_headers_internal_define_learning_ext_present_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_learning_ext_present_bits_set;
    data_index = dnx_data_headers_internal_define_learning_ext_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_learning_ext_size_set;
    data_index = dnx_data_headers_internal_define_ttl_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_ttl_offset_set;
    data_index = dnx_data_headers_internal_define_ttl_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_ttl_bits_set;
    data_index = dnx_data_headers_internal_define_eth_rm_fli_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_eth_rm_fli_offset_set;
    data_index = dnx_data_headers_internal_define_eth_rm_fli_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_eth_rm_fli_bits_set;
    data_index = dnx_data_headers_internal_define_eth_rm_pso_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_eth_rm_pso_offset_set;
    data_index = dnx_data_headers_internal_define_eth_rm_pso_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_eth_rm_pso_bits_set;
    data_index = dnx_data_headers_internal_define_cfg_fai_unknown_address_enable;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_cfg_fai_unknown_address_enable_set;

    
    data_index = dnx_data_headers_internal_header_visibilty_mode_global;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_headers_internal_header_visibilty_mode_global_set;

    
    
    submodule_index = dnx_data_headers_submodule_internal_legacy;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_headers_internal_legacy_define_fhei_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_legacy_fhei_size_set;
    data_index = dnx_data_headers_internal_legacy_define_fhei_size_sz0;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_legacy_fhei_size_sz0_set;
    data_index = dnx_data_headers_internal_legacy_define_fhei_size_sz1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_legacy_fhei_size_sz1_set;
    data_index = dnx_data_headers_internal_legacy_define_fhei_size_sz2;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_legacy_fhei_size_sz2_set;
    data_index = dnx_data_headers_internal_legacy_define_inlif_inrif_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_legacy_inlif_inrif_offset_set;
    data_index = dnx_data_headers_internal_legacy_define_inlif_inrif_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_legacy_inlif_inrif_bits_set;
    data_index = dnx_data_headers_internal_legacy_define_unknown_address_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_legacy_unknown_address_offset_set;
    data_index = dnx_data_headers_internal_legacy_define_unknown_address_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_legacy_unknown_address_bits_set;
    data_index = dnx_data_headers_internal_legacy_define_forwarding_header_offset_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_legacy_forwarding_header_offset_offset_set;
    data_index = dnx_data_headers_internal_legacy_define_forwarding_header_offset_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_legacy_forwarding_header_offset_bits_set;
    data_index = dnx_data_headers_internal_legacy_define_forward_code_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_legacy_forward_code_offset_set;
    data_index = dnx_data_headers_internal_legacy_define_forward_code_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_legacy_forward_code_bits_set;
    data_index = dnx_data_headers_internal_legacy_define_fhei_size_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_legacy_fhei_size_offset_set;
    data_index = dnx_data_headers_internal_legacy_define_fhei_size_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_legacy_fhei_size_bits_set;
    data_index = dnx_data_headers_internal_legacy_define_learning_ext_present_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_legacy_learning_ext_present_offset_set;
    data_index = dnx_data_headers_internal_legacy_define_learning_ext_present_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_legacy_learning_ext_present_bits_set;
    data_index = dnx_data_headers_internal_legacy_define_learning_ext_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_legacy_learning_ext_size_set;
    data_index = dnx_data_headers_internal_legacy_define_eei_ext_present_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_legacy_eei_ext_present_offset_set;
    data_index = dnx_data_headers_internal_legacy_define_eei_ext_present_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_internal_legacy_eei_ext_present_bits_set;

    

    
    
    submodule_index = dnx_data_headers_submodule_fhei_sz1_trap_sniff;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_headers_fhei_sz1_trap_sniff_define_fhei_type_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_fhei_sz1_trap_sniff_fhei_type_offset_set;
    data_index = dnx_data_headers_fhei_sz1_trap_sniff_define_fhei_type_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_fhei_sz1_trap_sniff_fhei_type_bits_set;
    data_index = dnx_data_headers_fhei_sz1_trap_sniff_define_code_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_fhei_sz1_trap_sniff_code_offset_set;
    data_index = dnx_data_headers_fhei_sz1_trap_sniff_define_code_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_fhei_sz1_trap_sniff_code_bits_set;
    data_index = dnx_data_headers_fhei_sz1_trap_sniff_define_qualifier_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_fhei_sz1_trap_sniff_qualifier_offset_set;
    data_index = dnx_data_headers_fhei_sz1_trap_sniff_define_qualifier_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_fhei_sz1_trap_sniff_qualifier_bits_set;

    

    
    
    submodule_index = dnx_data_headers_submodule_fhei_trap_sniff_legacy;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_headers_fhei_trap_sniff_legacy_define_code_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_fhei_trap_sniff_legacy_code_offset_set;
    data_index = dnx_data_headers_fhei_trap_sniff_legacy_define_code_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_fhei_trap_sniff_legacy_code_bits_set;
    data_index = dnx_data_headers_fhei_trap_sniff_legacy_define_qualifier_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_fhei_trap_sniff_legacy_qualifier_offset_set;
    data_index = dnx_data_headers_fhei_trap_sniff_legacy_define_qualifier_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_fhei_trap_sniff_legacy_qualifier_bits_set;

    

    
    
    submodule_index = dnx_data_headers_submodule_udh;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_headers_udh_define_data_type0_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_udh_data_type0_offset_set;
    data_index = dnx_data_headers_udh_define_data_type1_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_udh_data_type1_offset_set;
    data_index = dnx_data_headers_udh_define_data_type2_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_udh_data_type2_offset_set;
    data_index = dnx_data_headers_udh_define_data_type3_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_udh_data_type3_offset_set;
    data_index = dnx_data_headers_udh_define_data_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_udh_data_offset_set;
    data_index = dnx_data_headers_udh_define_data_type0_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_udh_data_type0_bits_set;
    data_index = dnx_data_headers_udh_define_data_type1_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_udh_data_type1_bits_set;
    data_index = dnx_data_headers_udh_define_data_type2_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_udh_data_type2_bits_set;
    data_index = dnx_data_headers_udh_define_data_type3_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_udh_data_type3_bits_set;
    data_index = dnx_data_headers_udh_define_data_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_udh_data_bits_set;
    data_index = dnx_data_headers_udh_define_base_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_udh_base_size_set;

    

    
    
    submodule_index = dnx_data_headers_submodule_ftmh_legacy;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_headers_ftmh_legacy_define_packet_size_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_packet_size_offset_set;
    data_index = dnx_data_headers_ftmh_legacy_define_traffic_class_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_traffic_class_offset_set;
    data_index = dnx_data_headers_ftmh_legacy_define_src_sys_port_aggregate_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_src_sys_port_aggregate_offset_set;
    data_index = dnx_data_headers_ftmh_legacy_define_pp_dsp_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_pp_dsp_offset_set;
    data_index = dnx_data_headers_ftmh_legacy_define_drop_precedence_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_drop_precedence_offset_set;
    data_index = dnx_data_headers_ftmh_legacy_define_tm_action_type_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_tm_action_type_offset_set;
    data_index = dnx_data_headers_ftmh_legacy_define_otsh_en_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_otsh_en_offset_set;
    data_index = dnx_data_headers_ftmh_legacy_define_internal_header_en_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_internal_header_en_offset_set;
    data_index = dnx_data_headers_ftmh_legacy_define_outbound_mirr_disable_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_outbound_mirr_disable_offset_set;
    data_index = dnx_data_headers_ftmh_legacy_define_tm_action_is_mc_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_tm_action_is_mc_offset_set;
    data_index = dnx_data_headers_ftmh_legacy_define_outlif_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_outlif_offset_set;
    data_index = dnx_data_headers_ftmh_legacy_define_dsp_ext_present_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_dsp_ext_present_offset_set;
    data_index = dnx_data_headers_ftmh_legacy_define_cni_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_cni_offset_set;
    data_index = dnx_data_headers_ftmh_legacy_define_ecn_enable_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_ecn_enable_offset_set;
    data_index = dnx_data_headers_ftmh_legacy_define_exclude_source_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_exclude_source_offset_set;
    data_index = dnx_data_headers_ftmh_legacy_define_stack_route_history_bmp_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_stack_route_history_bmp_offset_set;
    data_index = dnx_data_headers_ftmh_legacy_define_dsp_ext_header_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_dsp_ext_header_offset_set;
    data_index = dnx_data_headers_ftmh_legacy_define_packet_size_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_packet_size_bits_set;
    data_index = dnx_data_headers_ftmh_legacy_define_traffic_class_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_traffic_class_bits_set;
    data_index = dnx_data_headers_ftmh_legacy_define_src_sys_port_aggregate_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_src_sys_port_aggregate_bits_set;
    data_index = dnx_data_headers_ftmh_legacy_define_pp_dsp_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_pp_dsp_bits_set;
    data_index = dnx_data_headers_ftmh_legacy_define_drop_precedence_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_drop_precedence_bits_set;
    data_index = dnx_data_headers_ftmh_legacy_define_tm_action_type_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_tm_action_type_bits_set;
    data_index = dnx_data_headers_ftmh_legacy_define_otsh_en_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_otsh_en_bits_set;
    data_index = dnx_data_headers_ftmh_legacy_define_internal_header_en_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_internal_header_en_bits_set;
    data_index = dnx_data_headers_ftmh_legacy_define_outbound_mirr_disable_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_outbound_mirr_disable_bits_set;
    data_index = dnx_data_headers_ftmh_legacy_define_tm_action_is_mc_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_tm_action_is_mc_bits_set;
    data_index = dnx_data_headers_ftmh_legacy_define_outlif_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_outlif_bits_set;
    data_index = dnx_data_headers_ftmh_legacy_define_dsp_ext_present_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_dsp_ext_present_bits_set;
    data_index = dnx_data_headers_ftmh_legacy_define_cni_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_cni_bits_set;
    data_index = dnx_data_headers_ftmh_legacy_define_ecn_enable_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_ecn_enable_bits_set;
    data_index = dnx_data_headers_ftmh_legacy_define_exclude_source_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_exclude_source_bits_set;
    data_index = dnx_data_headers_ftmh_legacy_define_dsp_ext_header_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_dsp_ext_header_bits_set;
    data_index = dnx_data_headers_ftmh_legacy_define_base_header_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_base_header_size_set;
    data_index = dnx_data_headers_ftmh_legacy_define_dsp_ext_header_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_ftmh_legacy_dsp_ext_header_size_set;

    

    
    
    submodule_index = dnx_data_headers_submodule_otsh_legacy;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_headers_otsh_legacy_define_type_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_otsh_legacy_type_offset_set;
    data_index = dnx_data_headers_otsh_legacy_define_type_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_otsh_legacy_type_bits_set;
    data_index = dnx_data_headers_otsh_legacy_define_oam_sub_type_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_otsh_legacy_oam_sub_type_offset_set;
    data_index = dnx_data_headers_otsh_legacy_define_oam_sub_type_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_otsh_legacy_oam_sub_type_bits_set;
    data_index = dnx_data_headers_otsh_legacy_define_mep_type_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_otsh_legacy_mep_type_offset_set;
    data_index = dnx_data_headers_otsh_legacy_define_mep_type_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_otsh_legacy_mep_type_bits_set;
    data_index = dnx_data_headers_otsh_legacy_define_tp_command_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_otsh_legacy_tp_command_offset_set;
    data_index = dnx_data_headers_otsh_legacy_define_tp_command_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_otsh_legacy_tp_command_bits_set;
    data_index = dnx_data_headers_otsh_legacy_define_ts_encapsulation_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_otsh_legacy_ts_encapsulation_offset_set;
    data_index = dnx_data_headers_otsh_legacy_define_ts_encapsulation_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_otsh_legacy_ts_encapsulation_bits_set;
    data_index = dnx_data_headers_otsh_legacy_define_oam_ts_data_0_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_otsh_legacy_oam_ts_data_0_offset_set;
    data_index = dnx_data_headers_otsh_legacy_define_oam_ts_data_0_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_otsh_legacy_oam_ts_data_0_bits_set;
    data_index = dnx_data_headers_otsh_legacy_define_oam_ts_data_1_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_otsh_legacy_oam_ts_data_1_offset_set;
    data_index = dnx_data_headers_otsh_legacy_define_oam_ts_data_1_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_otsh_legacy_oam_ts_data_1_bits_set;
    data_index = dnx_data_headers_otsh_legacy_define_offset_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_otsh_legacy_offset_offset_set;
    data_index = dnx_data_headers_otsh_legacy_define_offset_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_otsh_legacy_offset_bits_set;

    

    
    
    submodule_index = dnx_data_headers_submodule_system_headers;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_headers_system_headers_define_system_headers_mode_jericho;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_system_headers_system_headers_mode_jericho_set;
    data_index = dnx_data_headers_system_headers_define_system_headers_mode_jericho2;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_system_headers_system_headers_mode_jericho2_set;
    data_index = dnx_data_headers_system_headers_define_crc_size_counted_in_pkt_len;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_system_headers_crc_size_counted_in_pkt_len_set;
    data_index = dnx_data_headers_system_headers_define_system_headers_mode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_system_headers_system_headers_mode_set;
    data_index = dnx_data_headers_system_headers_define_jr_mode_ftmh_lb_key_ext_mode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_system_headers_jr_mode_ftmh_lb_key_ext_mode_set;
    data_index = dnx_data_headers_system_headers_define_jr_mode_ftmh_stacking_ext_mode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_system_headers_jr_mode_ftmh_stacking_ext_mode_set;
    data_index = dnx_data_headers_system_headers_define_udh_base_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_system_headers_udh_base_size_set;
    data_index = dnx_data_headers_system_headers_define_jr_mode_ftmh_oam_ts_right_shift;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_system_headers_jr_mode_ftmh_oam_ts_right_shift_set;

    

    
    
    submodule_index = dnx_data_headers_submodule_otmh;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_headers_otmh_otmh_enable;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_headers_otmh_otmh_enable_set;

    
    
    submodule_index = dnx_data_headers_submodule_pph;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_headers_pph_define_pph_vsi_selection_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_pph_pph_vsi_selection_size_set;

    

    
    
    submodule_index = dnx_data_headers_submodule_general;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_headers_general_fwd_header_offset_calc_with_egress_pars_idx;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_headers_general_fwd_header_offset_calc_with_egress_pars_idx_set;
    data_index = dnx_data_headers_general_jericho_mode_sys_header_is_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_headers_general_jericho_mode_sys_header_is_supported_set;

    
    
    submodule_index = dnx_data_headers_submodule_feature;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_headers_feature_define_fhei_mpls_swap_remark_profile_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_headers_feature_fhei_mpls_swap_remark_profile_size_set;

    
    data_index = dnx_data_headers_feature_fhei_mpls_cmd_from_eei;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_headers_feature_fhei_mpls_cmd_from_eei_set;
    data_index = dnx_data_headers_feature_unknown_address;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_headers_feature_unknown_address_set;
    data_index = dnx_data_headers_feature_tsh_ext_with_flow_id;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_headers_feature_tsh_ext_with_flow_id_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

