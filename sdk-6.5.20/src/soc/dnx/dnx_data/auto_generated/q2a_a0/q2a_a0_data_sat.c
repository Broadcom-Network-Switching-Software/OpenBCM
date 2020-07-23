

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SAT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_sat.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>








static shr_error_e
q2a_a0_dnx_data_sat_generator_max_bit_rate_in_64B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_bit_rate_in_64B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 53000000;

    
    define->data = 53000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_sat_generator_max_bit_rate_in_128B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_bit_rate_in_128B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 72000000;

    
    define->data = 72000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_sat_generator_max_bit_rate_in_256B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_bit_rate_in_256B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 84000000;

    
    define->data = 84000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_sat_generator_max_bit_rate_in_512B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_bit_rate_in_512B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 91700000;

    
    define->data = 91700000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_sat_generator_max_bit_rate_in_1024B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_bit_rate_in_1024B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 96000000;

    
    define->data = 96000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_sat_generator_max_bit_rate_in_1280B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_bit_rate_in_1280B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 97000000;

    
    define->data = 97000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_sat_generator_max_bit_rate_in_1518B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_bit_rate_in_1518B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 97600000;

    
    define->data = 97600000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_sat_generator_max_bit_rate_in_9000B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_bit_rate_in_9000B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 99900000;

    
    define->data = 99900000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_sat_generator_max_packet_rate_in_64B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_packet_rate_in_64B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 300000000;

    
    define->data = 300000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_sat_generator_max_packet_rate_in_128B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_packet_rate_in_128B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 300000000;

    
    define->data = 300000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_sat_generator_max_packet_rate_in_256B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_packet_rate_in_256B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 150000000;

    
    define->data = 150000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_sat_generator_max_packet_rate_in_512B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_packet_rate_in_512B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 60000000 ;

    
    define->data = 60000000 ;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_sat_generator_max_packet_rate_in_1024B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_packet_rate_in_1024B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 33333333 ;

    
    define->data = 33333333 ;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_sat_generator_max_packet_rate_in_1280B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_packet_rate_in_1280B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 27200000 ;

    
    define->data = 27200000 ;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_sat_generator_max_packet_rate_in_1518B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_packet_rate_in_1518B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 23075000 ;

    
    define->data = 23075000 ;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_sat_generator_max_packet_rate_in_9000B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_packet_rate_in_9000B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4160000  ;

    
    define->data = 4160000  ;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_64B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_64B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 99000000;

    
    define->data = 99000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_128B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_128B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 68900000;

    
    define->data = 68900000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_256B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_256B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 40500000;

    
    define->data = 40500000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_512B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_512B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 22200000;

    
    define->data = 22200000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_1024B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_1024B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 11600000;

    
    define->data = 11600000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_1280B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_1280B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 9450000;

    
    define->data = 9450000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_1518B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_1518B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8020000;

    
    define->data = 8020000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_9000B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_9000B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1380000;

    
    define->data = 1380000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
q2a_a0_dnx_data_sat_rx_flow_param_natural_id_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_rx_flow_param;
    int feature_index = dnx_data_sat_rx_flow_param_natural_id;
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
q2a_a0_dnx_data_sat_rx_flow_param_odd_even_id_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_rx_flow_param;
    int feature_index = dnx_data_sat_rx_flow_param_odd_even_id;
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
q2a_a0_dnx_data_sat_rx_flow_param_twamp_mode_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_rx_flow_param;
    int feature_index = dnx_data_sat_rx_flow_param_twamp_mode;
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
q2a_a0_dnx_data_sat_rx_flow_param_twamp_rx_time_stamp_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_rx_flow_param;
    int feature_index = dnx_data_sat_rx_flow_param_twamp_rx_time_stamp;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}





shr_error_e
q2a_a0_data_sat_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_sat;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_sat_submodule_generator;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_sat_generator_define_max_bit_rate_in_64B;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_sat_generator_max_bit_rate_in_64B_set;
    data_index = dnx_data_sat_generator_define_max_bit_rate_in_128B;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_sat_generator_max_bit_rate_in_128B_set;
    data_index = dnx_data_sat_generator_define_max_bit_rate_in_256B;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_sat_generator_max_bit_rate_in_256B_set;
    data_index = dnx_data_sat_generator_define_max_bit_rate_in_512B;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_sat_generator_max_bit_rate_in_512B_set;
    data_index = dnx_data_sat_generator_define_max_bit_rate_in_1024B;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_sat_generator_max_bit_rate_in_1024B_set;
    data_index = dnx_data_sat_generator_define_max_bit_rate_in_1280B;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_sat_generator_max_bit_rate_in_1280B_set;
    data_index = dnx_data_sat_generator_define_max_bit_rate_in_1518B;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_sat_generator_max_bit_rate_in_1518B_set;
    data_index = dnx_data_sat_generator_define_max_bit_rate_in_9000B;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_sat_generator_max_bit_rate_in_9000B_set;
    data_index = dnx_data_sat_generator_define_max_packet_rate_in_64B;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_sat_generator_max_packet_rate_in_64B_set;
    data_index = dnx_data_sat_generator_define_max_packet_rate_in_128B;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_sat_generator_max_packet_rate_in_128B_set;
    data_index = dnx_data_sat_generator_define_max_packet_rate_in_256B;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_sat_generator_max_packet_rate_in_256B_set;
    data_index = dnx_data_sat_generator_define_max_packet_rate_in_512B;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_sat_generator_max_packet_rate_in_512B_set;
    data_index = dnx_data_sat_generator_define_max_packet_rate_in_1024B;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_sat_generator_max_packet_rate_in_1024B_set;
    data_index = dnx_data_sat_generator_define_max_packet_rate_in_1280B;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_sat_generator_max_packet_rate_in_1280B_set;
    data_index = dnx_data_sat_generator_define_max_packet_rate_in_1518B;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_sat_generator_max_packet_rate_in_1518B_set;
    data_index = dnx_data_sat_generator_define_max_packet_rate_in_9000B;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_sat_generator_max_packet_rate_in_9000B_set;
    data_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_64B;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_64B_set;
    data_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_128B;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_128B_set;
    data_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_256B;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_256B_set;
    data_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_512B;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_512B_set;
    data_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_1024B;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_1024B_set;
    data_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_1280B;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_1280B_set;
    data_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_1518B;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_1518B_set;
    data_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_9000B;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_9000B_set;

    

    
    
    submodule_index = dnx_data_sat_submodule_rx_flow_param;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_sat_rx_flow_param_natural_id;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_sat_rx_flow_param_natural_id_set;
    data_index = dnx_data_sat_rx_flow_param_odd_even_id;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_sat_rx_flow_param_odd_even_id_set;
    data_index = dnx_data_sat_rx_flow_param_twamp_mode;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_sat_rx_flow_param_twamp_mode_set;
    data_index = dnx_data_sat_rx_flow_param_twamp_rx_time_stamp;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_sat_rx_flow_param_twamp_rx_time_stamp_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

