

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_EGRESSDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm/port.h>
#include <soc/mcm/allenum.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>







static shr_error_e
q2a_a0_dnx_data_egr_queuing_params_conditional_crdt_table_access_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int feature_index = dnx_data_egr_queuing_params_conditional_crdt_table_access;
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
q2a_a0_dnx_data_egr_queuing_params_if_attributes_profile_exist_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int feature_index = dnx_data_egr_queuing_params_if_attributes_profile_exist;
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
q2a_a0_dnx_data_egr_queuing_params_cos_map_is_per_pp_port_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int feature_index = dnx_data_egr_queuing_params_cos_map_is_per_pp_port;
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
q2a_a0_dnx_data_egr_queuing_params_almost_empty_delay_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int feature_index = dnx_data_egr_queuing_params_almost_empty_delay;
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
q2a_a0_dnx_data_egr_queuing_params_fqp_calender_set_select_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int feature_index = dnx_data_egr_queuing_params_fqp_calender_set_select;
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
q2a_a0_dnx_data_egr_queuing_params_sub_calendar_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int feature_index = dnx_data_egr_queuing_params_sub_calendar;
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
q2a_a0_dnx_data_egr_queuing_params_txq_tdm_irdy_sel_exist_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int feature_index = dnx_data_egr_queuing_params_txq_tdm_irdy_sel_exist;
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
q2a_a0_dnx_data_egr_queuing_params_total_shaper_max_burst_exist_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int feature_index = dnx_data_egr_queuing_params_total_shaper_max_burst_exist;
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
q2a_a0_dnx_data_egr_queuing_params_nof_bits_in_q_pair_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nof_bits_in_q_pair;
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
q2a_a0_dnx_data_egr_queuing_params_nof_calendars_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nof_calendars;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 33;

    
    define->data = 33;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_egr_queuing_params_nof_bits_in_egr_interface_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nof_bits_in_egr_interface;
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
q2a_a0_dnx_data_egr_queuing_params_nof_bits_mirror_prio_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nof_bits_mirror_prio;
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
q2a_a0_dnx_data_egr_queuing_params_nof_bits_in_egr_ch_interface_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nof_bits_in_egr_ch_interface;
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
q2a_a0_dnx_data_egr_queuing_params_nof_egr_interfaces_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nof_egr_interfaces;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 136;

    
    define->data = 136;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_egr_queuing_params_nof_egr_ch_interfaces_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nof_egr_ch_interfaces;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_egr_queuing.params.nof_bits_in_egr_ch_interface_get(unit);

    
    define->data = 1 << dnx_data_egr_queuing.params.nof_bits_in_egr_ch_interface_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_egr_queuing_params_reserved_if_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_reserved_if;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 255;

    
    define->data = 255;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_egr_queuing_params_egr_if_nif_base_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_egr_if_nif_base;
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
q2a_a0_dnx_data_egr_queuing_params_nof_bits_in_cal_cal_len_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nof_bits_in_cal_cal_len;
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
q2a_a0_dnx_data_egr_queuing_params_cal_cal_len_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_cal_cal_len;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_egr_queuing.params.nof_bits_in_cal_cal_len_get(unit);

    
    define->data = 1 << dnx_data_egr_queuing.params.nof_bits_in_cal_cal_len_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_egr_queuing_params_nof_bits_in_nif_cal_len_pqp_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nof_bits_in_nif_cal_len_pqp;
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
q2a_a0_dnx_data_egr_queuing_params_nof_bits_in_nif_cal_len_fqp_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nof_bits_in_nif_cal_len_fqp;
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
q2a_a0_dnx_data_egr_queuing_params_sub_calendar_ifc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_sub_calendar_ifc;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 192;

    
    define->data = 192;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_egr_queuing_params_txq_max_credits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_txq_max_credits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 24576;

    
    define->data = 24576;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_egr_queuing_params_pqp_ifc_almost_empty_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_pqp_ifc_almost_empty_th;
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
q2a_a0_dnx_data_egr_queuing_params_pqp_port_almost_empty_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_pqp_port_almost_empty_th;
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
q2a_a0_dnx_data_egr_queuing_params_pqp_queue_almost_empty_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_pqp_queue_almost_empty_th;
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
q2a_a0_dnx_data_egr_queuing_params_if_min_gap_priority_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_if_min_gap_priority_size;
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
q2a_a0_dnx_data_egr_queuing_params_if_speed_params_set(
    int unit)
{
    int idx_index;
    int clock_rate_idx_index;
    dnx_data_egr_queuing_params_if_speed_params_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int table_index = dnx_data_egr_queuing_params_table_if_speed_params;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 17;
    table->info_get.key_size[0] = 17;
    table->keys[1].size = 2;
    table->info_get.key_size[1] = 2;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "-1";
    table->values[2].default_val = "-1";
    table->values[3].default_val = "-1";
    table->values[4].default_val = "-1";
    table->values[5].default_val = "-1";
    table->values[6].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_egr_queuing_params_if_speed_params_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_egr_queuing_params_table_if_speed_params");

    
    default_data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->speed = -1;
    default_data->if_id = -1;
    default_data->crdt_size = -1;
    default_data->irdy_thr = -1;
    default_data->txq_max_bytes = -1;
    default_data->min_gap_hp = -1;
    default_data->fqp_min_gap = -1;
    
    for (idx_index = 0; idx_index < table->keys[0].size; idx_index++)
    {
        for (clock_rate_idx_index = 0; clock_rate_idx_index < table->keys[1].size; clock_rate_idx_index++)
        {
            data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, idx_index, clock_rate_idx_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (0 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->if_id = dnx_data_egr_queuing.params.egr_if_cpu_get(unit);
        data->crdt_size = 468;
        data->irdy_thr = 320;
        data->txq_max_bytes = 800;
        data->min_gap_hp = 0;
        data->fqp_min_gap = 3;
    }
    if (1 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->if_id = dnx_data_egr_queuing.params.egr_if_sat_get(unit);
        data->crdt_size = 2400;
        data->irdy_thr = 1377;
        data->txq_max_bytes = 800;
        data->min_gap_hp = 0;
        data->fqp_min_gap = 0;
    }
    if (2 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->if_id = dnx_data_egr_queuing.params.egr_if_oam_get(unit);
        data->crdt_size = 468;
        data->irdy_thr = 320;
        data->txq_max_bytes = 800;
        data->min_gap_hp = 0;
        data->fqp_min_gap = 3;
    }
    if (3 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->if_id = dnx_data_egr_queuing.params.egr_if_olp_get(unit);
        data->crdt_size = 168;
        data->irdy_thr = 116;
        data->txq_max_bytes = 800;
        data->min_gap_hp = 0;
        data->fqp_min_gap = 4;
    }
    if (4 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->if_id = dnx_data_egr_queuing.params.egr_if_rcy_get(unit);
        data->crdt_size = 3000;
        data->irdy_thr = 1377;
        data->txq_max_bytes = 800;
        data->min_gap_hp = 0;
        data->fqp_min_gap = 0;
    }
    if (5 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->if_id = dnx_data_egr_queuing.params.egr_if_eventor_get(unit);
        data->crdt_size = 192;
        data->irdy_thr = 132;
        data->txq_max_bytes = 800;
        data->min_gap_hp = 0;
        data->fqp_min_gap = 4;
    }
    if (6 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->speed = 1000;
        data->crdt_size = 36;
        data->irdy_thr = 30;
        data->txq_max_bytes = 32;
        data->min_gap_hp = 6;
        data->fqp_min_gap = 3;
    }
    if (7 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->speed = 5000;
        data->crdt_size = 108;
        data->irdy_thr = 78;
        data->txq_max_bytes = 32;
        data->min_gap_hp = 6;
        data->fqp_min_gap = 3;
    }
    if (8 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->speed = 10000;
        data->crdt_size = 192;
        data->irdy_thr = 114;
        data->txq_max_bytes = 32;
        data->min_gap_hp = 6;
        data->fqp_min_gap = 3;
    }
    if (9 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->speed = 25000;
        data->crdt_size = 348;
        data->irdy_thr = 238;
        data->txq_max_bytes = 96;
        data->min_gap_hp = 6;
        data->fqp_min_gap = 3;
    }
    if (10 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->speed = 40000;
        data->crdt_size = 540;
        data->irdy_thr = 366;
        data->txq_max_bytes = 96;
        data->min_gap_hp = 6;
        data->fqp_min_gap = 2;
    }
    if (11 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->speed = 50000;
        data->crdt_size = 672;
        data->irdy_thr = 456;
        data->txq_max_bytes = 256;
        data->min_gap_hp = 4;
        data->fqp_min_gap = 2;
    }
    if (12 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->speed = 100000;
        data->crdt_size = 1332;
        data->irdy_thr = 910;
        data->txq_max_bytes = 256;
        data->min_gap_hp = 1;
        data->fqp_min_gap = 0;
    }
    if (13 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->speed = 200000;
        data->crdt_size = 2200;
        data->irdy_thr = 1341;
        data->txq_max_bytes = 400;
        data->min_gap_hp = 0;
        data->fqp_min_gap = 0;
    }
    if (14 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->speed = 300000;
        data->crdt_size = 3900;
        data->irdy_thr = 1365;
        data->txq_max_bytes = 400;
        data->min_gap_hp = 0;
        data->fqp_min_gap = 0;
    }
    if (15 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->speed = 400000;
        data->crdt_size = 3900;
        data->irdy_thr = 1365;
        data->txq_max_bytes = 400;
        data->min_gap_hp = 0;
        data->fqp_min_gap = 0;
    }
    if (16 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->speed = 600000;
        data->crdt_size = 3900;
        data->irdy_thr = 1365;
        data->txq_max_bytes = 400;
        data->min_gap_hp = 0;
        data->fqp_min_gap = 0;
    }
    if (0 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 1);
        data->if_id = dnx_data_egr_queuing.params.egr_if_cpu_get(unit);
        data->crdt_size = 468;
        data->irdy_thr = 320;
        data->txq_max_bytes = 800;
        data->min_gap_hp = 0;
        data->fqp_min_gap = 3;
    }
    if (1 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 1);
        data->if_id = dnx_data_egr_queuing.params.egr_if_sat_get(unit);
        data->crdt_size = 2400;
        data->irdy_thr = 1377;
        data->txq_max_bytes = 800;
        data->min_gap_hp = 0;
        data->fqp_min_gap = 0;
    }
    if (2 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 1);
        data->if_id = dnx_data_egr_queuing.params.egr_if_oam_get(unit);
        data->crdt_size = 468;
        data->irdy_thr = 320;
        data->txq_max_bytes = 800;
        data->min_gap_hp = 0;
        data->fqp_min_gap = 3;
    }
    if (3 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 1);
        data->if_id = dnx_data_egr_queuing.params.egr_if_olp_get(unit);
        data->crdt_size = 168;
        data->irdy_thr = 116;
        data->txq_max_bytes = 800;
        data->min_gap_hp = 0;
        data->fqp_min_gap = 4;
    }
    if (4 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 1);
        data->if_id = dnx_data_egr_queuing.params.egr_if_rcy_get(unit);
        data->crdt_size = 3000;
        data->irdy_thr = 1377;
        data->txq_max_bytes = 800;
        data->min_gap_hp = 0;
        data->fqp_min_gap = 0;
    }
    if (5 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 1);
        data->if_id = dnx_data_egr_queuing.params.egr_if_eventor_get(unit);
        data->crdt_size = 192;
        data->irdy_thr = 132;
        data->txq_max_bytes = 800;
        data->min_gap_hp = 0;
        data->fqp_min_gap = 4;
    }
    if (6 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 1);
        data->speed = 1000;
        data->crdt_size = 24;
        data->irdy_thr = 18;
        data->txq_max_bytes = 32;
        data->min_gap_hp = 6;
        data->fqp_min_gap = 3;
    }
    if (7 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 1);
        data->speed = 5000;
        data->crdt_size = 78;
        data->irdy_thr = 56;
        data->txq_max_bytes = 32;
        data->min_gap_hp = 6;
        data->fqp_min_gap = 3;
    }
    if (8 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 1);
        data->speed = 10000;
        data->crdt_size = 132;
        data->irdy_thr = 92;
        data->txq_max_bytes = 32;
        data->min_gap_hp = 6;
        data->fqp_min_gap = 3;
    }
    if (9 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 1);
        data->speed = 25000;
        data->crdt_size = 264;
        data->irdy_thr = 180;
        data->txq_max_bytes = 96;
        data->min_gap_hp = 6;
        data->fqp_min_gap = 3;
    }
    if (10 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 1);
        data->speed = 40000;
        data->crdt_size = 426;
        data->irdy_thr = 288;
        data->txq_max_bytes = 96;
        data->min_gap_hp = 6;
        data->fqp_min_gap = 2;
    }
    if (11 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 1);
        data->speed = 50000;
        data->crdt_size = 498;
        data->irdy_thr = 336;
        data->txq_max_bytes = 256;
        data->min_gap_hp = 4;
        data->fqp_min_gap = 2;
    }
    if (12 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 1);
        data->speed = 100000;
        data->crdt_size = 1020;
        data->irdy_thr = 688;
        data->txq_max_bytes = 256;
        data->min_gap_hp = 1;
        data->fqp_min_gap = 0;
    }
    if (13 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 1);
        data->speed = 200000;
        data->crdt_size = 2004;
        data->irdy_thr = 1341;
        data->txq_max_bytes = 400;
        data->min_gap_hp = 0;
        data->fqp_min_gap = 0;
    }
    if (14 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 1);
        data->speed = 300000;
        data->crdt_size = 3900;
        data->irdy_thr = 1377;
        data->txq_max_bytes = 400;
        data->min_gap_hp = 0;
        data->fqp_min_gap = 0;
    }
    if (15 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 1);
        data->speed = 400000;
        data->crdt_size = 3900;
        data->irdy_thr = 1377;
        data->txq_max_bytes = 400;
        data->min_gap_hp = 0;
        data->fqp_min_gap = 0;
    }
    if (16 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 1);
        data->speed = 600000;
        data->crdt_size = 3900;
        data->irdy_thr = 1377;
        data->txq_max_bytes = 400;
        data->min_gap_hp = 0;
        data->fqp_min_gap = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_egr_queuing_params_emr_fifo_set(
    int unit)
{
    int idx_index;
    int tdm_index;
    dnx_data_egr_queuing_params_emr_fifo_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int table_index = dnx_data_egr_queuing_params_table_emr_fifo;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_EGR_EMR_FIFO_MAX;
    table->info_get.key_size[0] = DNX_EGR_EMR_FIFO_MAX;
    table->keys[1].size = 2;
    table->info_get.key_size[1] = 2;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "-1";
    table->values[2].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_egr_queuing_params_emr_fifo_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_egr_queuing_params_table_emr_fifo");

    
    default_data = (dnx_data_egr_queuing_params_emr_fifo_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->depth = -1;
    default_data->almost_full = -1;
    default_data->full = -1;
    
    for (idx_index = 0; idx_index < table->keys[0].size; idx_index++)
    {
        for (tdm_index = 0; tdm_index < table->keys[1].size; tdm_index++)
        {
            data = (dnx_data_egr_queuing_params_emr_fifo_t *) dnxc_data_mgmt_table_data_get(unit, table, idx_index, tdm_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (DNX_EGR_EMR_FIFO_TDM < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_emr_fifo_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_EGR_EMR_FIFO_TDM, 0);
        data->depth = 1;
        data->almost_full = 0;
        data->full = 0;
    }
    if (DNX_EGR_EMR_FIFO_UC < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_emr_fifo_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_EGR_EMR_FIFO_UC, 0);
        data->depth = 0;
        data->almost_full = 20;
        data->full = 20;
    }
    if (DNX_EGR_EMR_FIFO_MC_HIGH < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_emr_fifo_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_EGR_EMR_FIFO_MC_HIGH, 0);
        data->depth = 256;
        data->almost_full = 10;
        data->full = 3;
    }
    if (DNX_EGR_EMR_FIFO_MC_LOW < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_emr_fifo_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_EGR_EMR_FIFO_MC_LOW, 0);
        data->depth = 255;
        data->almost_full = 10;
        data->full = 3;
    }
    if (DNX_EGR_EMR_FIFO_TDM < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_emr_fifo_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_EGR_EMR_FIFO_TDM, 1);
        data->depth = 128;
        data->almost_full = 20;
        data->full = 3;
    }
    if (DNX_EGR_EMR_FIFO_UC < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_emr_fifo_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_EGR_EMR_FIFO_UC, 1);
        data->depth = 0;
        data->almost_full = 20;
        data->full = 20;
    }
    if (DNX_EGR_EMR_FIFO_MC_HIGH < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_emr_fifo_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_EGR_EMR_FIFO_MC_HIGH, 1);
        data->depth = 192;
        data->almost_full = 10;
        data->full = 3;
    }
    if (DNX_EGR_EMR_FIFO_MC_LOW < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_emr_fifo_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_EGR_EMR_FIFO_MC_LOW, 1);
        data->depth = 192;
        data->almost_full = 10;
        data->full = 3;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_egr_queuing_params_if_speed_params_clk_th_set(
    int unit)
{
    int idx_index;
    dnx_data_egr_queuing_params_if_speed_params_clk_th_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int table_index = dnx_data_egr_queuing_params_table_if_speed_params_clk_th;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_egr_queuing_params_if_speed_params_clk_th_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_egr_queuing_params_table_if_speed_params_clk_th");

    
    default_data = (dnx_data_egr_queuing_params_if_speed_params_clk_th_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->max_clock_rate_th = -1;
    
    for (idx_index = 0; idx_index < table->keys[0].size; idx_index++)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_clk_th_t *) dnxc_data_mgmt_table_data_get(unit, table, idx_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_clk_th_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->max_clock_rate_th = 450000;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_clk_th_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->max_clock_rate_th = 600000;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
q2a_a0_dnx_data_egr_queuing_rate_measurement_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_rate_measurement;
    int feature_index = dnx_data_egr_queuing_rate_measurement_is_supported;
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
q2a_a0_dnx_data_egr_queuing_rate_measurement_max_interval_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_rate_measurement;
    int define_index = dnx_data_egr_queuing_rate_measurement_define_max_interval;
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
q2a_a0_dnx_data_egr_queuing_rate_measurement_promile_factor_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_rate_measurement;
    int define_index = dnx_data_egr_queuing_rate_measurement_define_promile_factor;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8*1000;

    
    define->data = 8*1000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_egr_queuing_rate_measurement_max_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_rate_measurement;
    int define_index = dnx_data_egr_queuing_rate_measurement_define_max_granularity;
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
q2a_a0_dnx_data_egr_queuing_rate_measurement_min_bytes_for_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_rate_measurement;
    int define_index = dnx_data_egr_queuing_rate_measurement_define_min_bytes_for_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 700;

    
    define->data = 700;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
q2a_a0_dnx_data_egr_queuing_phantom_queues_nof_threshold_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_phantom_queues;
    int define_index = dnx_data_egr_queuing_phantom_queues_define_nof_threshold_profiles;
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
q2a_a0_dnx_data_egr_queuing_phantom_queues_max_threshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_phantom_queues;
    int define_index = dnx_data_egr_queuing_phantom_queues_define_max_threshold;
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
q2a_a0_dnx_data_egr_queuing_reassembly_prs_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_reassembly;
    int feature_index = dnx_data_egr_queuing_reassembly_prs;
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
q2a_a0_data_egr_queuing_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_egr_queuing_submodule_params;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_egr_queuing_params_define_nof_bits_in_q_pair;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_egr_queuing_params_nof_bits_in_q_pair_set;
    data_index = dnx_data_egr_queuing_params_define_nof_calendars;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_egr_queuing_params_nof_calendars_set;
    data_index = dnx_data_egr_queuing_params_define_nof_bits_in_egr_interface;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_egr_queuing_params_nof_bits_in_egr_interface_set;
    data_index = dnx_data_egr_queuing_params_define_nof_bits_mirror_prio;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_egr_queuing_params_nof_bits_mirror_prio_set;
    data_index = dnx_data_egr_queuing_params_define_nof_bits_in_egr_ch_interface;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_egr_queuing_params_nof_bits_in_egr_ch_interface_set;
    data_index = dnx_data_egr_queuing_params_define_nof_egr_interfaces;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_egr_queuing_params_nof_egr_interfaces_set;
    data_index = dnx_data_egr_queuing_params_define_nof_egr_ch_interfaces;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_egr_queuing_params_nof_egr_ch_interfaces_set;
    data_index = dnx_data_egr_queuing_params_define_reserved_if;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_egr_queuing_params_reserved_if_set;
    data_index = dnx_data_egr_queuing_params_define_egr_if_nif_base;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_egr_queuing_params_egr_if_nif_base_set;
    data_index = dnx_data_egr_queuing_params_define_nof_bits_in_cal_cal_len;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_egr_queuing_params_nof_bits_in_cal_cal_len_set;
    data_index = dnx_data_egr_queuing_params_define_cal_cal_len;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_egr_queuing_params_cal_cal_len_set;
    data_index = dnx_data_egr_queuing_params_define_nof_bits_in_nif_cal_len_pqp;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_egr_queuing_params_nof_bits_in_nif_cal_len_pqp_set;
    data_index = dnx_data_egr_queuing_params_define_nof_bits_in_nif_cal_len_fqp;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_egr_queuing_params_nof_bits_in_nif_cal_len_fqp_set;
    data_index = dnx_data_egr_queuing_params_define_sub_calendar_ifc;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_egr_queuing_params_sub_calendar_ifc_set;
    data_index = dnx_data_egr_queuing_params_define_txq_max_credits;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_egr_queuing_params_txq_max_credits_set;
    data_index = dnx_data_egr_queuing_params_define_pqp_ifc_almost_empty_th;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_egr_queuing_params_pqp_ifc_almost_empty_th_set;
    data_index = dnx_data_egr_queuing_params_define_pqp_port_almost_empty_th;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_egr_queuing_params_pqp_port_almost_empty_th_set;
    data_index = dnx_data_egr_queuing_params_define_pqp_queue_almost_empty_th;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_egr_queuing_params_pqp_queue_almost_empty_th_set;
    data_index = dnx_data_egr_queuing_params_define_if_min_gap_priority_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_egr_queuing_params_if_min_gap_priority_size_set;

    
    data_index = dnx_data_egr_queuing_params_conditional_crdt_table_access;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_egr_queuing_params_conditional_crdt_table_access_set;
    data_index = dnx_data_egr_queuing_params_if_attributes_profile_exist;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_egr_queuing_params_if_attributes_profile_exist_set;
    data_index = dnx_data_egr_queuing_params_cos_map_is_per_pp_port;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_egr_queuing_params_cos_map_is_per_pp_port_set;
    data_index = dnx_data_egr_queuing_params_almost_empty_delay;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_egr_queuing_params_almost_empty_delay_set;
    data_index = dnx_data_egr_queuing_params_fqp_calender_set_select;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_egr_queuing_params_fqp_calender_set_select_set;
    data_index = dnx_data_egr_queuing_params_sub_calendar;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_egr_queuing_params_sub_calendar_set;
    data_index = dnx_data_egr_queuing_params_txq_tdm_irdy_sel_exist;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_egr_queuing_params_txq_tdm_irdy_sel_exist_set;
    data_index = dnx_data_egr_queuing_params_total_shaper_max_burst_exist;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_egr_queuing_params_total_shaper_max_burst_exist_set;

    
    data_index = dnx_data_egr_queuing_params_table_if_speed_params;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_egr_queuing_params_if_speed_params_set;
    data_index = dnx_data_egr_queuing_params_table_emr_fifo;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_egr_queuing_params_emr_fifo_set;
    data_index = dnx_data_egr_queuing_params_table_if_speed_params_clk_th;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_egr_queuing_params_if_speed_params_clk_th_set;
    
    submodule_index = dnx_data_egr_queuing_submodule_rate_measurement;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_egr_queuing_rate_measurement_define_max_interval;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_egr_queuing_rate_measurement_max_interval_set;
    data_index = dnx_data_egr_queuing_rate_measurement_define_promile_factor;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_egr_queuing_rate_measurement_promile_factor_set;
    data_index = dnx_data_egr_queuing_rate_measurement_define_max_granularity;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_egr_queuing_rate_measurement_max_granularity_set;
    data_index = dnx_data_egr_queuing_rate_measurement_define_min_bytes_for_granularity;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_egr_queuing_rate_measurement_min_bytes_for_granularity_set;

    
    data_index = dnx_data_egr_queuing_rate_measurement_is_supported;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_egr_queuing_rate_measurement_is_supported_set;

    
    
    submodule_index = dnx_data_egr_queuing_submodule_phantom_queues;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_egr_queuing_phantom_queues_define_nof_threshold_profiles;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_egr_queuing_phantom_queues_nof_threshold_profiles_set;
    data_index = dnx_data_egr_queuing_phantom_queues_define_max_threshold;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_egr_queuing_phantom_queues_max_threshold_set;

    

    
    
    submodule_index = dnx_data_egr_queuing_submodule_reassembly;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_egr_queuing_reassembly_prs;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_egr_queuing_reassembly_prs_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

