

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_EGRESSDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_esb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>







static shr_error_e
j2c_a0_dnx_data_esb_general_esb_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_esb;
    int submodule_index = dnx_data_esb_submodule_general;
    int feature_index = dnx_data_esb_general_esb_support;
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
j2c_a0_dnx_data_esb_general_slow_port_dedicated_queue_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_esb;
    int submodule_index = dnx_data_esb_submodule_general;
    int feature_index = dnx_data_esb_general_slow_port_dedicated_queue_support;
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
j2c_a0_dnx_data_esb_general_total_nof_esb_queues_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esb;
    int submodule_index = dnx_data_esb_submodule_general;
    int define_index = dnx_data_esb_general_define_total_nof_esb_queues;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_esb.general.nof_nif_eth_queues_get(unit)+dnx_data_esb.general.nof_nif_ilkn_queues_get(unit)+dnx_data_esb.general.nof_ilkn_tdm_queues_get(unit)+dnx_data_esb.general.nof_special_queues_get(unit);

    
    define->data = dnx_data_esb.general.nof_nif_eth_queues_get(unit)+dnx_data_esb.general.nof_nif_ilkn_queues_get(unit)+dnx_data_esb.general.nof_ilkn_tdm_queues_get(unit)+dnx_data_esb.general.nof_special_queues_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_esb_general_nof_nif_eth_queues_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esb;
    int submodule_index = dnx_data_esb_submodule_general;
    int define_index = dnx_data_esb_general_define_nof_nif_eth_queues;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.phys.nof_phys_get(unit);

    
    define->data = dnx_data_nif.phys.nof_phys_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_esb_general_nof_nif_ilkn_queues_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esb;
    int submodule_index = dnx_data_esb_submodule_general;
    int define_index = dnx_data_esb_general_define_nof_nif_ilkn_queues;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.ilkn.ilkn_if_nof_get(unit);

    
    define->data = dnx_data_nif.ilkn.ilkn_if_nof_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_esb_general_nof_ilkn_tdm_queues_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esb;
    int submodule_index = dnx_data_esb_submodule_general;
    int define_index = dnx_data_esb_general_define_nof_ilkn_tdm_queues;
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
j2c_a0_dnx_data_esb_general_nof_flexe_queues_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esb;
    int submodule_index = dnx_data_esb_submodule_general;
    int define_index = dnx_data_esb_general_define_nof_flexe_queues;
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
j2c_a0_dnx_data_esb_general_nof_special_queues_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esb;
    int submodule_index = dnx_data_esb_submodule_general;
    int define_index = dnx_data_esb_general_define_nof_special_queues;
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
j2c_a0_dnx_data_esb_general_nof_reserved_queues_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esb;
    int submodule_index = dnx_data_esb_submodule_general;
    int define_index = dnx_data_esb_general_define_nof_reserved_queues;
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
j2c_a0_dnx_data_esb_general_nof_fpc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esb;
    int submodule_index = dnx_data_esb_submodule_general;
    int define_index = dnx_data_esb_general_define_nof_fpc;
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
j2c_a0_dnx_data_esb_general_mux_cal_len_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esb;
    int submodule_index = dnx_data_esb_submodule_general;
    int define_index = dnx_data_esb_general_define_mux_cal_len;
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
j2c_a0_dnx_data_esb_general_ilkn_credits_resolution_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esb;
    int submodule_index = dnx_data_esb_submodule_general;
    int define_index = dnx_data_esb_general_define_ilkn_credits_resolution;
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
j2c_a0_dnx_data_esb_general_ilkn_credits_rate_resolution_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esb;
    int submodule_index = dnx_data_esb_submodule_general;
    int define_index = dnx_data_esb_general_define_ilkn_credits_rate_resolution;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 50000;

    
    define->data = 50000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_esb_general_fpc_ptr_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esb;
    int submodule_index = dnx_data_esb_submodule_general;
    int define_index = dnx_data_esb_general_define_fpc_ptr_size;
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
j2c_a0_dnx_data_esb_general_txi_irdy_th_factor_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esb;
    int submodule_index = dnx_data_esb_submodule_general;
    int define_index = dnx_data_esb_general_define_txi_irdy_th_factor;
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
j2c_a0_dnx_data_esb_general_txi_irdy_th_divident_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esb;
    int submodule_index = dnx_data_esb_submodule_general;
    int define_index = dnx_data_esb_general_define_txi_irdy_th_divident;
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
j2c_a0_dnx_data_esb_general_slow_port_dedicated_queue_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esb;
    int submodule_index = dnx_data_esb_submodule_general;
    int define_index = dnx_data_esb_general_define_slow_port_dedicated_queue;
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
j2c_a0_dnx_data_esb_general_if_esb_params_set(
    int unit)
{
    int idx_index;
    dnx_data_esb_general_if_esb_params_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_esb;
    int submodule_index = dnx_data_esb_submodule_general;
    int table_index = dnx_data_esb_general_table_if_esb_params;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 12;
    table->info_get.key_size[0] = 12;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "-1";
    table->values[2].default_val = "-1";
    table->values[3].default_val = "-1";
    table->values[4].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_esb_general_if_esb_params_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_esb_general_table_if_esb_params");

    
    default_data = (dnx_data_esb_general_if_esb_params_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->speed = -1;
    default_data->if_id = -1;
    default_data->queue_id = -1;
    default_data->interleave_queue_id = -1;
    default_data->min_gap = -1;
    
    for (idx_index = 0; idx_index < table->keys[0].size; idx_index++)
    {
        data = (dnx_data_esb_general_if_esb_params_t *) dnxc_data_mgmt_table_data_get(unit, table, idx_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_esb_general_if_esb_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->if_id = dnx_data_egr_queuing.params.egr_if_cpu_get(unit);
        data->queue_id = 144;
        data->min_gap = 3;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_esb_general_if_esb_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->if_id = dnx_data_egr_queuing.params.egr_if_sat_get(unit);
        data->queue_id = 145;
        data->min_gap = 0;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_esb_general_if_esb_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->if_id = dnx_data_egr_queuing.params.egr_if_oam_get(unit);
        data->queue_id = 146;
        data->min_gap = 3;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_esb_general_if_esb_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->if_id = dnx_data_egr_queuing.params.egr_if_olp_get(unit);
        data->queue_id = 147;
        data->min_gap = 9;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_esb_general_if_esb_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->if_id = dnx_data_egr_queuing.params.egr_if_rcy_get(unit);
        data->queue_id = 148;
        data->interleave_queue_id = 151;
        data->min_gap = 0;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_esb_general_if_esb_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->if_id = dnx_data_egr_queuing.params.egr_if_txi_rcy_get(unit);
        data->queue_id = 150;
        data->interleave_queue_id = 152;
        data->min_gap = 0;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_esb_general_if_esb_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->if_id = dnx_data_egr_queuing.params.egr_if_eventor_get(unit);
        data->queue_id = 149;
        data->min_gap = 9;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_esb_general_if_esb_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->speed = 40000;
        data->min_gap = 8;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_esb_general_if_esb_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->speed = 50000;
        data->min_gap = 6;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_esb_general_if_esb_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->speed = 100000;
        data->min_gap = 3;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_esb_general_if_esb_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->speed = 200000;
        data->min_gap = 1;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_esb_general_if_esb_params_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->speed = 640000;
        data->min_gap = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2c_a0_dnx_data_esb_dbal_pm_internal_port_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esb;
    int submodule_index = dnx_data_esb_submodule_dbal;
    int define_index = dnx_data_esb_dbal_define_pm_internal_port_size;
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
j2c_a0_dnx_data_esb_dbal_counter_config_mask_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esb;
    int submodule_index = dnx_data_esb_submodule_dbal;
    int define_index = dnx_data_esb_dbal_define_counter_config_mask_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
j2c_a0_data_esb_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_esb;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_esb_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_esb_general_define_total_nof_esb_queues;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_esb_general_total_nof_esb_queues_set;
    data_index = dnx_data_esb_general_define_nof_nif_eth_queues;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_esb_general_nof_nif_eth_queues_set;
    data_index = dnx_data_esb_general_define_nof_nif_ilkn_queues;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_esb_general_nof_nif_ilkn_queues_set;
    data_index = dnx_data_esb_general_define_nof_ilkn_tdm_queues;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_esb_general_nof_ilkn_tdm_queues_set;
    data_index = dnx_data_esb_general_define_nof_flexe_queues;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_esb_general_nof_flexe_queues_set;
    data_index = dnx_data_esb_general_define_nof_special_queues;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_esb_general_nof_special_queues_set;
    data_index = dnx_data_esb_general_define_nof_reserved_queues;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_esb_general_nof_reserved_queues_set;
    data_index = dnx_data_esb_general_define_nof_fpc;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_esb_general_nof_fpc_set;
    data_index = dnx_data_esb_general_define_mux_cal_len;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_esb_general_mux_cal_len_set;
    data_index = dnx_data_esb_general_define_ilkn_credits_resolution;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_esb_general_ilkn_credits_resolution_set;
    data_index = dnx_data_esb_general_define_ilkn_credits_rate_resolution;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_esb_general_ilkn_credits_rate_resolution_set;
    data_index = dnx_data_esb_general_define_fpc_ptr_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_esb_general_fpc_ptr_size_set;
    data_index = dnx_data_esb_general_define_txi_irdy_th_factor;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_esb_general_txi_irdy_th_factor_set;
    data_index = dnx_data_esb_general_define_txi_irdy_th_divident;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_esb_general_txi_irdy_th_divident_set;
    data_index = dnx_data_esb_general_define_slow_port_dedicated_queue;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_esb_general_slow_port_dedicated_queue_set;

    
    data_index = dnx_data_esb_general_esb_support;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_esb_general_esb_support_set;
    data_index = dnx_data_esb_general_slow_port_dedicated_queue_support;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_esb_general_slow_port_dedicated_queue_support_set;

    
    data_index = dnx_data_esb_general_table_if_esb_params;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_esb_general_if_esb_params_set;
    
    submodule_index = dnx_data_esb_submodule_dbal;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_esb_dbal_define_pm_internal_port_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_esb_dbal_pm_internal_port_size_set;
    data_index = dnx_data_esb_dbal_define_counter_config_mask_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_esb_dbal_counter_config_mask_size_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

