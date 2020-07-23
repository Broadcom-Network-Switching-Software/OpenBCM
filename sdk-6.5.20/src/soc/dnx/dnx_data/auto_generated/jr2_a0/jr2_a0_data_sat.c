

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
jr2_a0_dnx_data_sat_general_is_sat_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_general;
    int feature_index = dnx_data_sat_general_is_sat_supported;
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
jr2_a0_dnx_data_sat_general_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_general;
    int define_index = dnx_data_sat_general_define_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64000;

    
    define->data = 64000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_sat_generator_nof_evc_ids_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_nof_evc_ids;
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
jr2_a0_dnx_data_sat_generator_nof_gtf_ids_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_nof_gtf_ids;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_generator_gtf_pkt_header_max_length_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_gtf_pkt_header_max_length;
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
jr2_a0_dnx_data_sat_generator_gtf_pkt_length_max_patn_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_gtf_pkt_length_max_patn;
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
jr2_a0_dnx_data_sat_generator_gtf_max_seq_period_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_gtf_max_seq_period;
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
jr2_a0_dnx_data_sat_generator_gtf_stamp_max_inc_step_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_gtf_stamp_max_inc_step;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 127;

    
    define->data = 127;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_generator_gtf_stamp_max_inc_period_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_gtf_stamp_max_inc_period;
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
jr2_a0_dnx_data_sat_generator_gtf_bw_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_gtf_bw_max_value;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 17179607;

    
    define->data = 17179607;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_generator_gtf_rate_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_gtf_rate_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 400000000;

    
    define->data = 400000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_generator_gtf_pps_rate_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_gtf_pps_rate_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 500000000;

    
    define->data = 500000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_generator_gtf_pkt_header_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_gtf_pkt_header_entries;
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
jr2_a0_dnx_data_sat_generator_gtf_pkt_len_patten_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_gtf_pkt_len_patten_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 96;

    
    define->data = 96;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_generator_max_bit_rate_in_64B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_bit_rate_in_64B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 180000000;

    
    define->data = 180000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_generator_max_bit_rate_in_128B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_bit_rate_in_128B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 400000000;

    
    define->data = 400000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_generator_max_bit_rate_in_256B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_bit_rate_in_256B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 336000000;

    
    define->data = 336000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_generator_max_bit_rate_in_512B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_bit_rate_in_512B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 400000000;

    
    define->data = 400000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_generator_max_bit_rate_in_1024B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_bit_rate_in_1024B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 400000000;

    
    define->data = 400000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_generator_max_bit_rate_in_1280B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_bit_rate_in_1280B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 400000000;

    
    define->data = 400000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_generator_max_bit_rate_in_1518B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_bit_rate_in_1518B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 390000000;

    
    define->data = 390000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_generator_max_bit_rate_in_9000B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_bit_rate_in_9000B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 399000000;

    
    define->data = 399000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_generator_max_packet_rate_in_64B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_packet_rate_in_64B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 500000000;

    
    define->data = 500000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_generator_max_packet_rate_in_128B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_packet_rate_in_128B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 333000000;

    
    define->data = 333000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_generator_max_packet_rate_in_256B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_packet_rate_in_256B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 179000000;

    
    define->data = 179000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_generator_max_packet_rate_in_512B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_packet_rate_in_512B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 93600000;

    
    define->data = 93600000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_generator_max_packet_rate_in_1024B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_packet_rate_in_1024B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 45400000;

    
    define->data = 45400000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_generator_max_packet_rate_in_1280B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_packet_rate_in_1280B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 38400000;

    
    define->data = 38400000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_generator_max_packet_rate_in_1518B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_packet_rate_in_1518B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32400000;

    
    define->data = 32400000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_generator_max_packet_rate_in_9000B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_packet_rate_in_9000B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5540000;

    
    define->data = 5540000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_64B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_64B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 335000000;

    
    define->data = 335000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_128B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_128B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 249000000;

    
    define->data = 249000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_256B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_256B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 162000000;

    
    define->data = 162000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_512B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_512B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 83300000 ;

    
    define->data = 83300000 ;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_1024B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_1024B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 43400000 ;

    
    define->data = 43400000 ;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_1280B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_1280B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 37000000 ;

    
    define->data = 37000000 ;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_1518B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_1518B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32000000 ;

    
    define->data = 32000000 ;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_9000B_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_generator;
    int define_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_9000B;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5540000  ;

    
    define->data = 5540000  ;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_sat_collector_nof_ctf_ids_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_collector;
    int define_index = dnx_data_sat_collector_define_nof_ctf_ids;
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
jr2_a0_dnx_data_sat_collector_nof_oam_ids_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_collector;
    int define_index = dnx_data_sat_collector_define_nof_oam_ids;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sat_collector_mef_payload_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_collector;
    int define_index = dnx_data_sat_collector_define_mef_payload_offset;
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
jr2_a0_dnx_data_sat_collector_mef_seq_num_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_collector;
    int define_index = dnx_data_sat_collector_define_mef_seq_num_offset;
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
jr2_a0_dnx_data_sat_collector_mef_time_stamp_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_collector;
    int define_index = dnx_data_sat_collector_define_mef_time_stamp_offset;
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
jr2_a0_dnx_data_sat_collector_mef_crc_byte_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_collector;
    int define_index = dnx_data_sat_collector_define_mef_crc_byte_offset;
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
jr2_a0_dnx_data_sat_collector_y1731_payload_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_collector;
    int define_index = dnx_data_sat_collector_define_y1731_payload_offset;
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
jr2_a0_dnx_data_sat_collector_y1731_seq_num_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_collector;
    int define_index = dnx_data_sat_collector_define_y1731_seq_num_offset;
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
jr2_a0_dnx_data_sat_collector_y1731_time_stamp_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_collector;
    int define_index = dnx_data_sat_collector_define_y1731_time_stamp_offset;
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
jr2_a0_dnx_data_sat_collector_y1731_crc_byte_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_collector;
    int define_index = dnx_data_sat_collector_define_y1731_crc_byte_offset;
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
jr2_a0_dnx_data_sat_collector_ctf_bins_limit_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_collector;
    int define_index = dnx_data_sat_collector_define_ctf_bins_limit_entries;
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
jr2_a0_dnx_data_sat_collector_trap_id_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_collector;
    int define_index = dnx_data_sat_collector_define_trap_id_entries;
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
jr2_a0_dnx_data_sat_collector_trap_data_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_collector;
    int define_index = dnx_data_sat_collector_define_trap_data_entries;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_sat_rx_flow_param_natural_id_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_rx_flow_param;
    int feature_index = dnx_data_sat_rx_flow_param_natural_id;
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
jr2_a0_dnx_data_sat_rx_flow_param_odd_even_id_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_rx_flow_param;
    int feature_index = dnx_data_sat_rx_flow_param_odd_even_id;
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
jr2_a0_dnx_data_sat_tx_flow_param_seq_number_wrap_around_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_tx_flow_param;
    int feature_index = dnx_data_sat_tx_flow_param_seq_number_wrap_around;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}





shr_error_e
jr2_a0_data_sat_attach(
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
    
    submodule_index = dnx_data_sat_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_sat_general_define_granularity;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_general_granularity_set;

    
    data_index = dnx_data_sat_general_is_sat_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_sat_general_is_sat_supported_set;

    
    
    submodule_index = dnx_data_sat_submodule_generator;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_sat_generator_define_nof_evc_ids;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_nof_evc_ids_set;
    data_index = dnx_data_sat_generator_define_nof_gtf_ids;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_nof_gtf_ids_set;
    data_index = dnx_data_sat_generator_define_gtf_pkt_header_max_length;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_gtf_pkt_header_max_length_set;
    data_index = dnx_data_sat_generator_define_gtf_pkt_length_max_patn;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_gtf_pkt_length_max_patn_set;
    data_index = dnx_data_sat_generator_define_gtf_max_seq_period;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_gtf_max_seq_period_set;
    data_index = dnx_data_sat_generator_define_gtf_stamp_max_inc_step;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_gtf_stamp_max_inc_step_set;
    data_index = dnx_data_sat_generator_define_gtf_stamp_max_inc_period;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_gtf_stamp_max_inc_period_set;
    data_index = dnx_data_sat_generator_define_gtf_bw_max_value;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_gtf_bw_max_value_set;
    data_index = dnx_data_sat_generator_define_gtf_rate_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_gtf_rate_max_set;
    data_index = dnx_data_sat_generator_define_gtf_pps_rate_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_gtf_pps_rate_max_set;
    data_index = dnx_data_sat_generator_define_gtf_pkt_header_entries;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_gtf_pkt_header_entries_set;
    data_index = dnx_data_sat_generator_define_gtf_pkt_len_patten_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_gtf_pkt_len_patten_size_set;
    data_index = dnx_data_sat_generator_define_max_bit_rate_in_64B;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_max_bit_rate_in_64B_set;
    data_index = dnx_data_sat_generator_define_max_bit_rate_in_128B;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_max_bit_rate_in_128B_set;
    data_index = dnx_data_sat_generator_define_max_bit_rate_in_256B;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_max_bit_rate_in_256B_set;
    data_index = dnx_data_sat_generator_define_max_bit_rate_in_512B;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_max_bit_rate_in_512B_set;
    data_index = dnx_data_sat_generator_define_max_bit_rate_in_1024B;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_max_bit_rate_in_1024B_set;
    data_index = dnx_data_sat_generator_define_max_bit_rate_in_1280B;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_max_bit_rate_in_1280B_set;
    data_index = dnx_data_sat_generator_define_max_bit_rate_in_1518B;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_max_bit_rate_in_1518B_set;
    data_index = dnx_data_sat_generator_define_max_bit_rate_in_9000B;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_max_bit_rate_in_9000B_set;
    data_index = dnx_data_sat_generator_define_max_packet_rate_in_64B;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_max_packet_rate_in_64B_set;
    data_index = dnx_data_sat_generator_define_max_packet_rate_in_128B;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_max_packet_rate_in_128B_set;
    data_index = dnx_data_sat_generator_define_max_packet_rate_in_256B;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_max_packet_rate_in_256B_set;
    data_index = dnx_data_sat_generator_define_max_packet_rate_in_512B;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_max_packet_rate_in_512B_set;
    data_index = dnx_data_sat_generator_define_max_packet_rate_in_1024B;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_max_packet_rate_in_1024B_set;
    data_index = dnx_data_sat_generator_define_max_packet_rate_in_1280B;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_max_packet_rate_in_1280B_set;
    data_index = dnx_data_sat_generator_define_max_packet_rate_in_1518B;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_max_packet_rate_in_1518B_set;
    data_index = dnx_data_sat_generator_define_max_packet_rate_in_9000B;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_max_packet_rate_in_9000B_set;
    data_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_64B;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_64B_set;
    data_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_128B;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_128B_set;
    data_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_256B;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_256B_set;
    data_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_512B;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_512B_set;
    data_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_1024B;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_1024B_set;
    data_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_1280B;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_1280B_set;
    data_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_1518B;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_1518B_set;
    data_index = dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_9000B;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_generator_max_ctf_recv_packet_rate_in_9000B_set;

    

    
    
    submodule_index = dnx_data_sat_submodule_collector;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_sat_collector_define_nof_ctf_ids;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_collector_nof_ctf_ids_set;
    data_index = dnx_data_sat_collector_define_nof_oam_ids;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_collector_nof_oam_ids_set;
    data_index = dnx_data_sat_collector_define_mef_payload_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_collector_mef_payload_offset_set;
    data_index = dnx_data_sat_collector_define_mef_seq_num_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_collector_mef_seq_num_offset_set;
    data_index = dnx_data_sat_collector_define_mef_time_stamp_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_collector_mef_time_stamp_offset_set;
    data_index = dnx_data_sat_collector_define_mef_crc_byte_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_collector_mef_crc_byte_offset_set;
    data_index = dnx_data_sat_collector_define_y1731_payload_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_collector_y1731_payload_offset_set;
    data_index = dnx_data_sat_collector_define_y1731_seq_num_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_collector_y1731_seq_num_offset_set;
    data_index = dnx_data_sat_collector_define_y1731_time_stamp_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_collector_y1731_time_stamp_offset_set;
    data_index = dnx_data_sat_collector_define_y1731_crc_byte_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_collector_y1731_crc_byte_offset_set;
    data_index = dnx_data_sat_collector_define_ctf_bins_limit_entries;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_collector_ctf_bins_limit_entries_set;
    data_index = dnx_data_sat_collector_define_trap_id_entries;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_collector_trap_id_entries_set;
    data_index = dnx_data_sat_collector_define_trap_data_entries;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sat_collector_trap_data_entries_set;

    

    
    
    submodule_index = dnx_data_sat_submodule_rx_flow_param;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_sat_rx_flow_param_natural_id;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_sat_rx_flow_param_natural_id_set;
    data_index = dnx_data_sat_rx_flow_param_odd_even_id;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_sat_rx_flow_param_odd_even_id_set;

    
    
    submodule_index = dnx_data_sat_submodule_tx_flow_param;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_sat_tx_flow_param_seq_number_wrap_around;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_sat_tx_flow_param_seq_number_wrap_around_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

