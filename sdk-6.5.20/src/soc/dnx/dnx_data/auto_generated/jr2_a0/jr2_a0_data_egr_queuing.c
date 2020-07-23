

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
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>







static shr_error_e
jr2_a0_dnx_data_egr_queuing_params_conditional_crdt_table_access_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int feature_index = dnx_data_egr_queuing_params_conditional_crdt_table_access;
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
jr2_a0_dnx_data_egr_queuing_params_non_j2b0_counters_are_valid_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int feature_index = dnx_data_egr_queuing_params_non_j2b0_counters_are_valid;
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
jr2_a0_dnx_data_egr_queuing_params_if_attributes_profile_exist_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int feature_index = dnx_data_egr_queuing_params_if_attributes_profile_exist;
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
jr2_a0_dnx_data_egr_queuing_params_cos_map_is_per_pp_port_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int feature_index = dnx_data_egr_queuing_params_cos_map_is_per_pp_port;
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
jr2_a0_dnx_data_egr_queuing_params_phantom_queues_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int feature_index = dnx_data_egr_queuing_params_phantom_queues;
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
jr2_a0_dnx_data_egr_queuing_params_almost_empty_delay_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int feature_index = dnx_data_egr_queuing_params_almost_empty_delay;
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
jr2_a0_dnx_data_egr_queuing_params_fqp_calender_set_select_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int feature_index = dnx_data_egr_queuing_params_fqp_calender_set_select;
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
jr2_a0_dnx_data_egr_queuing_params_sub_calendar_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int feature_index = dnx_data_egr_queuing_params_sub_calendar;
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
jr2_a0_dnx_data_egr_queuing_params_txq_tdm_irdy_sel_exist_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int feature_index = dnx_data_egr_queuing_params_txq_tdm_irdy_sel_exist;
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
jr2_a0_dnx_data_egr_queuing_params_total_shaper_max_burst_exist_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int feature_index = dnx_data_egr_queuing_params_total_shaper_max_burst_exist;
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
jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_tcg_weight_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nof_bits_in_tcg_weight;
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
jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_nof_q_pairs_in_ps_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nof_bits_in_nof_q_pairs_in_ps;
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
jr2_a0_dnx_data_egr_queuing_params_nof_q_pairs_in_ps_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nof_q_pairs_in_ps;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_egr_queuing.params.nof_bits_in_nof_q_pairs_in_ps_get(unit);

    
    define->data = 1 << dnx_data_egr_queuing.params.nof_bits_in_nof_q_pairs_in_ps_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_q_pair_set(
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
jr2_a0_dnx_data_egr_queuing_params_nof_q_pairs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nof_q_pairs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_egr_queuing.params.nof_bits_in_q_pair_get(unit);

    
    define->data = 1 << dnx_data_egr_queuing.params.nof_bits_in_q_pair_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_nof_port_schedulers_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nof_bits_in_nof_port_schedulers;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_egr_queuing.params.nof_bits_in_q_pair_get(unit) - dnx_data_egr_queuing.params.nof_bits_in_nof_q_pairs_in_ps_get(unit);

    
    define->data = dnx_data_egr_queuing.params.nof_bits_in_q_pair_get(unit) - dnx_data_egr_queuing.params.nof_bits_in_nof_q_pairs_in_ps_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_egr_queuing_params_nof_port_schedulers_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nof_port_schedulers;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_egr_queuing.params.nof_bits_in_nof_port_schedulers_get(unit);

    
    define->data = 1 << dnx_data_egr_queuing.params.nof_bits_in_nof_port_schedulers_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_chan_arb_calendar_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nof_bits_in_chan_arb_calendar_size;
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
jr2_a0_dnx_data_egr_queuing_params_chan_arb_calendar_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_chan_arb_calendar_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_egr_queuing.params.nof_bits_in_chan_arb_calendar_size_get(unit);

    
    define->data = 1 << dnx_data_egr_queuing.params.nof_bits_in_chan_arb_calendar_size_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_port_prio_calendar_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nof_bits_in_port_prio_calendar_size;
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
jr2_a0_dnx_data_egr_queuing_params_port_prio_calendar_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_port_prio_calendar_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_egr_queuing.params.nof_bits_in_port_prio_calendar_size_get(unit);

    
    define->data = 1 << dnx_data_egr_queuing.params.nof_bits_in_port_prio_calendar_size_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_tcg_calendar_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nof_bits_in_tcg_calendar_size;
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
jr2_a0_dnx_data_egr_queuing_params_tcg_calendar_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_tcg_calendar_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_egr_queuing.params.nof_bits_in_tcg_calendar_size_get(unit);

    
    define->data = 1 << dnx_data_egr_queuing.params.nof_bits_in_tcg_calendar_size_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_nof_calendars_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nof_bits_in_nof_calendars;
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
jr2_a0_dnx_data_egr_queuing_params_nof_calendars_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nof_calendars;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_egr_queuing.params.nof_bits_in_nof_calendars_get(unit);

    
    define->data = 1 << dnx_data_egr_queuing.params.nof_bits_in_nof_calendars_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_egr_interface_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nof_bits_in_egr_interface;
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
jr2_a0_dnx_data_egr_queuing_params_nof_bits_mirror_prio_set(
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
jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_egr_ch_interface_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nof_bits_in_egr_ch_interface;
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
jr2_a0_dnx_data_egr_queuing_params_nof_egr_interfaces_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nof_egr_interfaces;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 56;

    
    define->data = 56;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_egr_queuing_params_nof_egr_ch_interfaces_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nof_egr_ch_interfaces;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 56;

    
    define->data = 56;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_egr_queuing_params_reserved_if_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_reserved_if;
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
jr2_a0_dnx_data_egr_queuing_params_egr_if_cpu_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_egr_if_cpu;
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
jr2_a0_dnx_data_egr_queuing_params_egr_if_sat_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_egr_if_sat;
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
jr2_a0_dnx_data_egr_queuing_params_egr_if_oam_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_egr_if_oam;
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
jr2_a0_dnx_data_egr_queuing_params_egr_if_olp_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_egr_if_olp;
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
jr2_a0_dnx_data_egr_queuing_params_egr_if_rcy_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_egr_if_rcy;
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
jr2_a0_dnx_data_egr_queuing_params_egr_if_txi_rcy_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_egr_if_txi_rcy;
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
jr2_a0_dnx_data_egr_queuing_params_egr_if_eventor_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_egr_if_eventor;
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
jr2_a0_dnx_data_egr_queuing_params_egr_if_nif_base_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_egr_if_nif_base;
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
jr2_a0_dnx_data_egr_queuing_params_cal_res_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_cal_res;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_egr_queuing.params.cal_burst_res_get(unit)/8;

    
    define->data = dnx_data_egr_queuing.params.cal_burst_res_get(unit)/8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_egr_queuing_params_cal_res_packet_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_cal_res_packet_mode;
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
jr2_a0_dnx_data_egr_queuing_params_cal_burst_res_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_cal_burst_res;
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
jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_cal_cal_len_set(
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
jr2_a0_dnx_data_egr_queuing_params_cal_cal_len_set(
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
jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_nif_cal_len_pqp_set(
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
jr2_a0_dnx_data_egr_queuing_params_nif_cal_len_pqp_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nif_cal_len_pqp;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_egr_queuing.params.nof_bits_in_nif_cal_len_pqp_get(unit);

    
    define->data = 1 << dnx_data_egr_queuing.params.nof_bits_in_nif_cal_len_pqp_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_nif_cal_len_fqp_set(
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
jr2_a0_dnx_data_egr_queuing_params_nif_cal_len_fqp_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nif_cal_len_fqp;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_egr_queuing.params.nof_bits_in_nif_cal_len_fqp_get(unit);

    
    define->data = 1 << dnx_data_egr_queuing.params.nof_bits_in_nif_cal_len_fqp_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_nof_tcg_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nof_bits_in_nof_tcg;
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
jr2_a0_dnx_data_egr_queuing_params_nof_tcg_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nof_tcg;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_egr_queuing.params.nof_bits_in_nof_tcg_get(unit);

    
    define->data = 1 << dnx_data_egr_queuing.params.nof_bits_in_nof_tcg_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_nof_egr_q_prio_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nof_bits_in_nof_egr_q_prio;
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
jr2_a0_dnx_data_egr_queuing_params_nof_egr_q_prio_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_nof_egr_q_prio;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_egr_queuing.params.nof_bits_in_nof_egr_q_prio_get(unit);

    
    define->data = 1 << dnx_data_egr_queuing.params.nof_bits_in_nof_egr_q_prio_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_egr_queuing_params_tcg_min_priorities_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_tcg_min_priorities;
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
jr2_a0_dnx_data_egr_queuing_params_max_credit_number_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_max_credit_number;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x7fffff;

    
    define->data = 0x7fffff;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_egr_queuing_params_max_gbps_rate_egq_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_max_gbps_rate_egq;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2500;

    
    define->data = 2500;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_egr_queuing_params_txq_iready_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_txq_iready_th;
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
jr2_a0_dnx_data_egr_queuing_params_txq_tdm_iready_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_txq_tdm_iready_th;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 28;

    
    define->data = 28;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_egr_queuing_params_initial_packet_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_initial_packet_mode;
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
jr2_a0_dnx_data_egr_queuing_params_txq_max_credits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_txq_max_credits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xFFFFFFFF;

    
    define->data = 0xFFFFFFFF;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_egr_queuing_params_pqp_ifc_almost_empty_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_pqp_ifc_almost_empty_th;
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
jr2_a0_dnx_data_egr_queuing_params_pqp_port_almost_empty_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_pqp_port_almost_empty_th;
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
jr2_a0_dnx_data_egr_queuing_params_pqp_queue_almost_empty_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_pqp_queue_almost_empty_th;
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
jr2_a0_dnx_data_egr_queuing_params_if_min_gap_priority_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_if_min_gap_priority_size;
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
jr2_a0_dnx_data_egr_queuing_params_invalid_otm_port_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_invalid_otm_port;
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
jr2_a0_dnx_data_egr_queuing_params_burst_size_below_mtu_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_burst_size_below_mtu;
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
jr2_a0_dnx_data_egr_queuing_params_if_speed_params_set(
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

    
    table->keys[0].size = 13;
    table->info_get.key_size[0] = 13;
    table->keys[1].size = 1;
    table->info_get.key_size[1] = 1;

    
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
        data->crdt_size = 640;
        data->irdy_thr = 456;
        data->txq_max_bytes = 768;
        data->min_gap_hp = 0;
        data->fqp_min_gap = 0;
    }
    if (1 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->if_id = dnx_data_egr_queuing.params.egr_if_sat_get(unit);
        data->crdt_size = 2560;
        data->irdy_thr = 1589;
        data->txq_max_bytes = 768;
        data->min_gap_hp = 0;
        data->fqp_min_gap = 0;
    }
    if (2 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->if_id = dnx_data_egr_queuing.params.egr_if_oam_get(unit);
        data->crdt_size = 640;
        data->irdy_thr = 456;
        data->txq_max_bytes = 768;
        data->min_gap_hp = 0;
        data->fqp_min_gap = 0;
    }
    if (3 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->if_id = dnx_data_egr_queuing.params.egr_if_olp_get(unit);
        data->crdt_size = 220;
        data->irdy_thr = 176;
        data->txq_max_bytes = 768;
        data->min_gap_hp = 0;
        data->fqp_min_gap = 0;
    }
    if (4 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->if_id = dnx_data_egr_queuing.params.egr_if_rcy_get(unit);
        data->crdt_size = 7040;
        data->irdy_thr = 1589;
        data->txq_max_bytes = 768;
        data->min_gap_hp = 0;
        data->fqp_min_gap = 0;
    }
    if (5 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->if_id = dnx_data_egr_queuing.params.egr_if_eventor_get(unit);
        data->crdt_size = 220;
        data->irdy_thr = 176;
        data->txq_max_bytes = 768;
        data->min_gap_hp = 0;
        data->fqp_min_gap = 0;
    }
    if (6 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->speed = 10000;
        data->crdt_size = 400;
        data->irdy_thr = 365;
        data->txq_max_bytes = 32;
        data->min_gap_hp = 6;
        data->fqp_min_gap = 8;
    }
    if (7 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->speed = 25000;
        data->crdt_size = 400;
        data->irdy_thr = 336;
        data->txq_max_bytes = 48;
        data->min_gap_hp = 6;
        data->fqp_min_gap = 8;
    }
    if (8 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->speed = 40000;
        data->crdt_size = 400;
        data->irdy_thr = 286;
        data->txq_max_bytes = 48;
        data->min_gap_hp = 6;
        data->fqp_min_gap = 8;
    }
    if (9 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->speed = 50000;
        data->crdt_size = 400;
        data->irdy_thr = 286;
        data->txq_max_bytes = 48;
        data->min_gap_hp = 4;
        data->fqp_min_gap = 6;
    }
    if (10 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->speed = 100000;
        data->crdt_size = 800;
        data->irdy_thr = 580;
        data->txq_max_bytes = 128;
        data->min_gap_hp = 1;
        data->fqp_min_gap = 3;
    }
    if (11 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->speed = 200000;
        data->crdt_size = 1600;
        data->irdy_thr = 1169;
        data->txq_max_bytes = 400;
        data->min_gap_hp = 1;
        data->fqp_min_gap = 1;
    }
    if (12 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->speed = 400000;
        data->crdt_size = 3200;
        data->irdy_thr = 2346;
        data->txq_max_bytes = 400;
        data->min_gap_hp = 0;
        data->fqp_min_gap = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_egr_queuing_params_emr_fifo_set(
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
        data->depth = 63;
        data->almost_full = 20;
        data->full = 20;
    }
    if (DNX_EGR_EMR_FIFO_MC_HIGH < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_emr_fifo_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_EGR_EMR_FIFO_MC_HIGH, 0);
        data->depth = 224;
        data->almost_full = 10;
        data->full = 3;
    }
    if (DNX_EGR_EMR_FIFO_MC_LOW < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_emr_fifo_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_EGR_EMR_FIFO_MC_LOW, 0);
        data->depth = 224;
        data->almost_full = 10;
        data->full = 3;
    }
    if (DNX_EGR_EMR_FIFO_TDM < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_emr_fifo_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_EGR_EMR_FIFO_TDM, 1);
        data->depth = 64;
        data->almost_full = 20;
        data->full = 3;
    }
    if (DNX_EGR_EMR_FIFO_UC < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_egr_queuing_params_emr_fifo_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_EGR_EMR_FIFO_UC, 1);
        data->depth = 64;
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
jr2_a0_dnx_data_egr_queuing_params_if_speed_params_clk_th_set(
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

    
    table->keys[0].size = 1;
    table->info_get.key_size[0] = 1;

    
    table->values[0].default_val = "DATA(device, general, core_clock_khz)";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_egr_queuing_params_if_speed_params_clk_th_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_egr_queuing_params_table_if_speed_params_clk_th");

    
    default_data = (dnx_data_egr_queuing_params_if_speed_params_clk_th_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->max_clock_rate_th = dnx_data_device.general.core_clock_khz_get(unit);
    
    for (idx_index = 0; idx_index < table->keys[0].size; idx_index++)
    {
        data = (dnx_data_egr_queuing_params_if_speed_params_clk_th_t *) dnxc_data_mgmt_table_data_get(unit, table, idx_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_egr_queuing_common_max_val_nof_bits_in_calendar_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_common_max_val;
    int define_index = dnx_data_egr_queuing_common_max_val_define_nof_bits_in_calendar_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX3(dnx_data_egr_queuing.params.nof_bits_in_tcg_calendar_size_get(unit),                 dnx_data_egr_queuing.params.nof_bits_in_port_prio_calendar_size_get(unit),                   dnx_data_egr_queuing.params.nof_bits_in_chan_arb_calendar_size_get(unit));

    
    define->data = UTILEX_MAX3(dnx_data_egr_queuing.params.nof_bits_in_tcg_calendar_size_get(unit),                 dnx_data_egr_queuing.params.nof_bits_in_port_prio_calendar_size_get(unit),                   dnx_data_egr_queuing.params.nof_bits_in_chan_arb_calendar_size_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_egr_queuing_common_max_val_calendar_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_common_max_val;
    int define_index = dnx_data_egr_queuing_common_max_val_define_calendar_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_egr_queuing.common_max_val.nof_bits_in_calendar_size_get(unit);

    
    define->data = 1 << dnx_data_egr_queuing.common_max_val.nof_bits_in_calendar_size_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_egr_queuing_common_max_val_slow_port_speed_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_common_max_val;
    int define_index = dnx_data_egr_queuing_common_max_val_define_slow_port_speed;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 12500;

    
    define->data = 12500;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_egr_queuing_common_max_val_mux_calendar_resolution_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_common_max_val;
    int define_index = dnx_data_egr_queuing_common_max_val_define_mux_calendar_resolution;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_egr_queuing.common_max_val.slow_port_speed_get(unit);

    
    define->data = dnx_data_egr_queuing.common_max_val.slow_port_speed_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_egr_queuing_reassembly_prs_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_reassembly;
    int feature_index = dnx_data_egr_queuing_reassembly_prs;
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
jr2_a0_data_egr_queuing_attach(
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

    
    data_index = dnx_data_egr_queuing_params_define_nof_bits_in_tcg_weight;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_tcg_weight_set;
    data_index = dnx_data_egr_queuing_params_define_nof_bits_in_nof_q_pairs_in_ps;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_nof_q_pairs_in_ps_set;
    data_index = dnx_data_egr_queuing_params_define_nof_q_pairs_in_ps;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_nof_q_pairs_in_ps_set;
    data_index = dnx_data_egr_queuing_params_define_nof_bits_in_q_pair;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_q_pair_set;
    data_index = dnx_data_egr_queuing_params_define_nof_q_pairs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_nof_q_pairs_set;
    data_index = dnx_data_egr_queuing_params_define_nof_bits_in_nof_port_schedulers;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_nof_port_schedulers_set;
    data_index = dnx_data_egr_queuing_params_define_nof_port_schedulers;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_nof_port_schedulers_set;
    data_index = dnx_data_egr_queuing_params_define_nof_bits_in_chan_arb_calendar_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_chan_arb_calendar_size_set;
    data_index = dnx_data_egr_queuing_params_define_chan_arb_calendar_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_chan_arb_calendar_size_set;
    data_index = dnx_data_egr_queuing_params_define_nof_bits_in_port_prio_calendar_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_port_prio_calendar_size_set;
    data_index = dnx_data_egr_queuing_params_define_port_prio_calendar_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_port_prio_calendar_size_set;
    data_index = dnx_data_egr_queuing_params_define_nof_bits_in_tcg_calendar_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_tcg_calendar_size_set;
    data_index = dnx_data_egr_queuing_params_define_tcg_calendar_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_tcg_calendar_size_set;
    data_index = dnx_data_egr_queuing_params_define_nof_bits_in_nof_calendars;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_nof_calendars_set;
    data_index = dnx_data_egr_queuing_params_define_nof_calendars;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_nof_calendars_set;
    data_index = dnx_data_egr_queuing_params_define_nof_bits_in_egr_interface;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_egr_interface_set;
    data_index = dnx_data_egr_queuing_params_define_nof_bits_mirror_prio;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_nof_bits_mirror_prio_set;
    data_index = dnx_data_egr_queuing_params_define_nof_bits_in_egr_ch_interface;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_egr_ch_interface_set;
    data_index = dnx_data_egr_queuing_params_define_nof_egr_interfaces;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_nof_egr_interfaces_set;
    data_index = dnx_data_egr_queuing_params_define_nof_egr_ch_interfaces;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_nof_egr_ch_interfaces_set;
    data_index = dnx_data_egr_queuing_params_define_reserved_if;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_reserved_if_set;
    data_index = dnx_data_egr_queuing_params_define_egr_if_cpu;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_egr_if_cpu_set;
    data_index = dnx_data_egr_queuing_params_define_egr_if_sat;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_egr_if_sat_set;
    data_index = dnx_data_egr_queuing_params_define_egr_if_oam;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_egr_if_oam_set;
    data_index = dnx_data_egr_queuing_params_define_egr_if_olp;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_egr_if_olp_set;
    data_index = dnx_data_egr_queuing_params_define_egr_if_rcy;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_egr_if_rcy_set;
    data_index = dnx_data_egr_queuing_params_define_egr_if_txi_rcy;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_egr_if_txi_rcy_set;
    data_index = dnx_data_egr_queuing_params_define_egr_if_eventor;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_egr_if_eventor_set;
    data_index = dnx_data_egr_queuing_params_define_egr_if_nif_base;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_egr_if_nif_base_set;
    data_index = dnx_data_egr_queuing_params_define_cal_res;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_cal_res_set;
    data_index = dnx_data_egr_queuing_params_define_cal_res_packet_mode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_cal_res_packet_mode_set;
    data_index = dnx_data_egr_queuing_params_define_cal_burst_res;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_cal_burst_res_set;
    data_index = dnx_data_egr_queuing_params_define_nof_bits_in_cal_cal_len;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_cal_cal_len_set;
    data_index = dnx_data_egr_queuing_params_define_cal_cal_len;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_cal_cal_len_set;
    data_index = dnx_data_egr_queuing_params_define_nof_bits_in_nif_cal_len_pqp;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_nif_cal_len_pqp_set;
    data_index = dnx_data_egr_queuing_params_define_nif_cal_len_pqp;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_nif_cal_len_pqp_set;
    data_index = dnx_data_egr_queuing_params_define_nof_bits_in_nif_cal_len_fqp;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_nif_cal_len_fqp_set;
    data_index = dnx_data_egr_queuing_params_define_nif_cal_len_fqp;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_nif_cal_len_fqp_set;
    data_index = dnx_data_egr_queuing_params_define_nof_bits_in_nof_tcg;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_nof_tcg_set;
    data_index = dnx_data_egr_queuing_params_define_nof_tcg;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_nof_tcg_set;
    data_index = dnx_data_egr_queuing_params_define_nof_bits_in_nof_egr_q_prio;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_nof_bits_in_nof_egr_q_prio_set;
    data_index = dnx_data_egr_queuing_params_define_nof_egr_q_prio;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_nof_egr_q_prio_set;
    data_index = dnx_data_egr_queuing_params_define_tcg_min_priorities;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_tcg_min_priorities_set;
    data_index = dnx_data_egr_queuing_params_define_max_credit_number;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_max_credit_number_set;
    data_index = dnx_data_egr_queuing_params_define_max_gbps_rate_egq;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_max_gbps_rate_egq_set;
    data_index = dnx_data_egr_queuing_params_define_txq_iready_th;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_txq_iready_th_set;
    data_index = dnx_data_egr_queuing_params_define_txq_tdm_iready_th;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_txq_tdm_iready_th_set;
    data_index = dnx_data_egr_queuing_params_define_initial_packet_mode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_initial_packet_mode_set;
    data_index = dnx_data_egr_queuing_params_define_txq_max_credits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_txq_max_credits_set;
    data_index = dnx_data_egr_queuing_params_define_pqp_ifc_almost_empty_th;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_pqp_ifc_almost_empty_th_set;
    data_index = dnx_data_egr_queuing_params_define_pqp_port_almost_empty_th;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_pqp_port_almost_empty_th_set;
    data_index = dnx_data_egr_queuing_params_define_pqp_queue_almost_empty_th;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_pqp_queue_almost_empty_th_set;
    data_index = dnx_data_egr_queuing_params_define_if_min_gap_priority_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_if_min_gap_priority_size_set;
    data_index = dnx_data_egr_queuing_params_define_invalid_otm_port;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_invalid_otm_port_set;
    data_index = dnx_data_egr_queuing_params_define_burst_size_below_mtu;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_params_burst_size_below_mtu_set;

    
    data_index = dnx_data_egr_queuing_params_conditional_crdt_table_access;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_egr_queuing_params_conditional_crdt_table_access_set;
    data_index = dnx_data_egr_queuing_params_non_j2b0_counters_are_valid;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_egr_queuing_params_non_j2b0_counters_are_valid_set;
    data_index = dnx_data_egr_queuing_params_if_attributes_profile_exist;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_egr_queuing_params_if_attributes_profile_exist_set;
    data_index = dnx_data_egr_queuing_params_cos_map_is_per_pp_port;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_egr_queuing_params_cos_map_is_per_pp_port_set;
    data_index = dnx_data_egr_queuing_params_phantom_queues;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_egr_queuing_params_phantom_queues_set;
    data_index = dnx_data_egr_queuing_params_almost_empty_delay;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_egr_queuing_params_almost_empty_delay_set;
    data_index = dnx_data_egr_queuing_params_fqp_calender_set_select;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_egr_queuing_params_fqp_calender_set_select_set;
    data_index = dnx_data_egr_queuing_params_sub_calendar;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_egr_queuing_params_sub_calendar_set;
    data_index = dnx_data_egr_queuing_params_txq_tdm_irdy_sel_exist;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_egr_queuing_params_txq_tdm_irdy_sel_exist_set;
    data_index = dnx_data_egr_queuing_params_total_shaper_max_burst_exist;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_egr_queuing_params_total_shaper_max_burst_exist_set;

    
    data_index = dnx_data_egr_queuing_params_table_if_speed_params;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_egr_queuing_params_if_speed_params_set;
    data_index = dnx_data_egr_queuing_params_table_emr_fifo;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_egr_queuing_params_emr_fifo_set;
    data_index = dnx_data_egr_queuing_params_table_if_speed_params_clk_th;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_egr_queuing_params_if_speed_params_clk_th_set;
    
    submodule_index = dnx_data_egr_queuing_submodule_common_max_val;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_egr_queuing_common_max_val_define_nof_bits_in_calendar_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_common_max_val_nof_bits_in_calendar_size_set;
    data_index = dnx_data_egr_queuing_common_max_val_define_calendar_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_common_max_val_calendar_size_set;
    data_index = dnx_data_egr_queuing_common_max_val_define_slow_port_speed;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_common_max_val_slow_port_speed_set;
    data_index = dnx_data_egr_queuing_common_max_val_define_mux_calendar_resolution;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_egr_queuing_common_max_val_mux_calendar_resolution_set;

    

    
    
    submodule_index = dnx_data_egr_queuing_submodule_reassembly;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_egr_queuing_reassembly_prs;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_egr_queuing_reassembly_prs_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

