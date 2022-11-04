
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INIT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_dev_init.h>
#include <bcm_int/dnx/spb/spb.h>
#include <bcm_int/dnx/init/init_custom_funcs.h>








static shr_error_e
j2p_a0_dnx_data_dev_init_time_init_total_time_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_time;
    int define_index = dnx_data_dev_init_time_define_init_total_time;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 12711600;

    
    define->data = 12711600;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_time_appl_init_total_time_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_time;
    int define_index = dnx_data_dev_init_time_define_appl_init_total_time;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 13669200;

    
    define->data = 13669200;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_time_down_deviation_total_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_time;
    int define_index = dnx_data_dev_init_time_define_down_deviation_total;
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
j2p_a0_dnx_data_dev_init_time_up_deviation_total_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_time;
    int define_index = dnx_data_dev_init_time_define_up_deviation_total;
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
j2p_a0_dnx_data_dev_init_time_test_sensitivity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_time;
    int define_index = dnx_data_dev_init_time_define_test_sensitivity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 100000;

    
    define->data = 100000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2p_a0_dnx_data_dev_init_time_step_time_set(
    int unit)
{
    int step_id_index;
    dnx_data_dev_init_time_step_time_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_time;
    int table_index = dnx_data_dev_init_time_table_step_time;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_INIT_STEP_COUNT;
    table->info_get.key_size[0] = DNX_INIT_STEP_COUNT;

    
    table->values[0].default_val = "DATA(dev_init, time, test_sensitivity)";
    table->values[1].default_val = "10";
    table->values[2].default_val = "10";
    table->values[3].default_val = "10000";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dev_init_time_step_time_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dev_init_time_table_step_time");

    
    default_data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->time = dnx_data_dev_init.time.test_sensitivity_get(unit);
    default_data->down_deviation = 10;
    default_data->up_deviation = 10;
    default_data->non_perc_up_deviation = 10000;
    
    for (step_id_index = 0; step_id_index < table->keys[0].size; step_id_index++)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, step_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_INIT_STEP_ALGO_TUNNEL < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_ALGO_TUNNEL, 0);
        data->time = 456995;
    }
    if (DNX_INIT_STEP_TUNE < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_TUNE, 0);
        data->time = 338644;
    }
    if (DNX_INIT_STEP_TM_MODULES < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_TM_MODULES, 0);
        data->time = 511038;
    }
    if (DNX_INIT_STEP_PP_GENERAL < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_PP_GENERAL, 0);
        data->time = 110785;
    }
    if (DNX_INIT_MDB < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_MDB, 0);
        data->time = 195450;
    }
    if (DNX_INIT_STEP_PEMLA < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_PEMLA, 0);
        data->time = 321850;
    }
    if (DNX_INIT_STEP_MEM_DEFAULTS < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_MEM_DEFAULTS, 0);
        data->time = 317887;
    }
    if (DNX_INIT_STEP_SOFT_RESET < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_SOFT_RESET, 0);
        data->time = 125928;
    }
    if (DNX_INIT_STEP_PP_MODULES < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_PP_MODULES, 0);
        data->time = 333945;
    }
    if (DNX_INIT_STEP_AUX_ACCESS < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_AUX_ACCESS, 0);
        data->time = 222395;
        data->down_deviation = 50;
    }
    if (DNX_INIT_STEP_DYN_PORT_PROP_SET < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_DYN_PORT_PROP_SET, 0);
        data->time = 970358;
    }
    if (DNX_INIT_STEP_ACCESS < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_ACCESS, 0);
        data->time = 128167;
    }
    if (DNX_INIT_STEP_PRE_SOFT_INIT < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_PRE_SOFT_INIT, 0);
        data->time = 5020229;
    }
    if (DNX_INIT_STEP_APPL_PROPERTIES < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_APPL_PROPERTIES, 0);
        data->time = 2091554;
    }
    if (DNX_INIT_STEP_COMMON_MODULES < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_COMMON_MODULES, 0);
        data->time = 494933;
    }
    if (DNX_INIT_STEP_DNX_DATA < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_DNX_DATA, 0);
        data->time = 177691;
        data->up_deviation = 30;
    }
    if (DNX_INIT_STEP_DYN_PORT_ADD < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_DYN_PORT_ADD, 0);
        data->time = 1078490;
    }
    if (DNX_INIT_STEP_CLEAR_COUNTER < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_CLEAR_COUNTER, 0);
        data->time = 140057;
    }
    if (DNX_INIT_STEP_DRAM_INITIAL_CONFIG < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_DRAM_INITIAL_CONFIG, 0);
        data->time = 224831;
    }
    if (DNX_INIT_STEP_DRAM_PHY_BIST < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_DRAM_PHY_BIST, 0);
        data->time = 2180333;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_time_appl_step_time_set(
    int unit)
{
    int step_id_index;
    dnx_data_dev_init_time_appl_step_time_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_time;
    int table_index = dnx_data_dev_init_time_table_appl_step_time;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = APPL_DNX_INIT_STEP_COUNT;
    table->info_get.key_size[0] = APPL_DNX_INIT_STEP_COUNT;

    
    table->values[0].default_val = "DATA(dev_init, time, test_sensitivity)";
    table->values[1].default_val = "10";
    table->values[2].default_val = "10";
    table->values[3].default_val = "10000";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dev_init_time_appl_step_time_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dev_init_time_table_appl_step_time");

    
    default_data = (dnx_data_dev_init_time_appl_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->time = dnx_data_dev_init.time.test_sensitivity_get(unit);
    default_data->down_deviation = 10;
    default_data->up_deviation = 10;
    default_data->non_perc_up_deviation = 10000;
    
    for (step_id_index = 0; step_id_index < table->keys[0].size; step_id_index++)
    {
        data = (dnx_data_dev_init_time_appl_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, step_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (APPL_DNX_STEP_QOS < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_appl_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNX_STEP_QOS, 0);
        data->time = 105610;
    }
    if (APPL_DNX_STEP_SRV6 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_appl_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNX_STEP_SRV6, 0);
        data->time = 100673;
    }
    if (APPL_DNX_STEP_SYS_PORTS < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_appl_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNX_STEP_SYS_PORTS, 0);
        data->time = 115944;
    }
    if (APPL_DNX_STEP_VLAN < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_appl_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNX_STEP_VLAN, 0);
        data->time = 152912;
    }
    if (APPL_DNX_STEP_SDK < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_appl_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNX_STEP_SDK, 0);
        data->time = 8012352;
    }
    if (APPL_DNX_STEP_APPS < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_appl_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNX_STEP_APPS, 0);
        data->time = 1027661;
    }
    if (APPL_DNX_STEP_ITMH_PPH < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_appl_step_time_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNX_STEP_ITMH_PPH, 0);
        data->time = 388524;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2p_a0_dnx_data_dev_init_mem_default_set(
    int unit)
{
    int index_index;
    dnx_data_dev_init_mem_default_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_mem;
    int table_index = dnx_data_dev_init_mem_table_default;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 65;
    table->info_get.key_size[0] = 65;

    
    table->values[0].default_val = "INVALIDr";
    table->values[1].default_val = "dnx_init_mem_default_mode_zero";
    table->values[2].default_val = "NULL";
    table->values[3].default_val = "INVALIDf";
    table->values[4].default_val = "1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dev_init_mem_default_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dev_init_mem_table_default");

    
    default_data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->mem = INVALIDr;
    default_data->mode = dnx_init_mem_default_mode_zero;
    default_data->default_get_cb = NULL;
    default_data->field = INVALIDf;
    default_data->valid = 1;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->mem = IPS_SPMm;
        data->mode = dnx_init_mem_default_mode_all_ones;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->mem = MTM_EGR_BITMAP_MAPPING_TABLE_Am;
        data->mode = dnx_init_mem_default_mode_index;
        data->field = PP_DSPf;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->mem = MTM_EGR_BITMAP_MAPPING_TABLE_Bm;
        data->mode = dnx_init_mem_default_mode_index;
        data->field = PP_DSPf;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->mem = ERPP_PRP_PP_DSP_PTR_TABLEm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_erpp_prp_pp_dsp_ptr_table_get;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->mem = ERPP_TM_PP_DSP_PTR_TABLEm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_erpp_tm_pp_dsp_ptr_table_get;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->mem = ETPPA_DSP_DATA_TABLEm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_etppa_dsp_data_table_get;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->mem = IPPE_PRT_PP_PORT_INFOm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_ippe_prt_pp_port_info_table_get;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->mem = CGM_REP_CMD_FIFO_OCCUPANCY_RJCT_THm;
        data->mode = dnx_init_mem_default_mode_all_ones;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->mem = SPB_CONTEXT_MRUm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_spb_context_mru_default_get_f;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->mem = EPNI_ALIGNER_MIRR_QP_TO_CHANNEL_MAPm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = CHANNELf;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->mem = SCH_FLOW_TO_FIP_MAPPING_FFMm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_sch_flow_to_fip_mapping_default_get;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->mem = ERPP_CFG_ENABLE_FILTER_PER_PORT_TABLEm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = CFG_SAME_INTERFACE_PER_PORTf;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->mem = ETPPB_VID_MIRROR_PROFILE_TABLEm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = OUT_PORT_MIRROR_PROFILEf;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->mem = SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_sch_shaper_descr_default_get;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->mem = SCH_FLOW_DESCRIPTOR_MEMORY_STATIC_FDMSm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_sch_flow_descr_default_get;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->mem = IPT_PCP_CFGm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_fabric_pcp_default_get;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->mem = SCH_SLOW_SCALE_A_SSAm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = MAX_BUCKETf;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->mem = SCH_SLOW_SCALE_B_SSBm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = MAX_BUCKETf;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->mem = IPPE_PINFO_LLRm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = RECYCLE_PROFILE_ENABLEf;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->mem = MTM_MCDBm;
        data->mode = dnx_init_mem_default_mode_all_ones;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->mem = SCH_CIR_SHAPER_CALENDAR_CSCm;
        data->mode = dnx_init_mem_default_mode_index;
        data->field = PG_NUMf;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->mem = SCH_RESERVED_51m;
        data->mode = dnx_init_mem_default_mode_index;
        data->field = HR_NUMf;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->mem = SCH_PORT_SCHEDULER_WEIGHTS_PSWm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_sch_port_schduler_weights_psw_default_get;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->mem = SCH_RCI_CREDIT_JITTER_HIGH_MAPm;
        data->mode = dnx_init_mem_default_mode_all_ones;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->mem = IPPD_DESTINATION_STATUSm;
        data->mode = dnx_init_mem_default_mode_all_ones;
    }
    if (25 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, 0);
        data->mem = IPS_QSPMm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = SYSTEM_PORTf;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->mem = CGM_QSPMm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = SYSTEM_PORTf;
    }
    if (27 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, 0);
        data->mem = IPS_QPRIORITYm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = QPRIORITY_DATAf;
    }
    if (28 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 28, 0);
        data->mem = CGM_IPP_MAPm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_cgm_ipp_map_default_get;
    }
    if (29 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 29, 0);
        data->mem = IPPC_FES_2ND_INSTRUCTIONm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = ACTION_TYPEf;
    }
    if (30 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 30, 0);
        data->mem = IPPD_FES_2ND_INSTRUCTIONm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = ACTION_TYPEf;
    }
    if (31 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 31, 0);
        data->mem = ERPP_FES_2ND_INSTRUCTIONm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = ACTION_TYPEf;
    }
    if (32 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 32, 0);
        data->mem = ETPPA_PRP_FES_PROGRAM_TABLEm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_etppa_prp_fes_program_table_default_get;
    }
    if (33 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 33, 0);
        data->mem = SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_sch_psst_default_get;
    }
    if (34 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 34, 0);
        data->mem = SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_sch_csst_default_get;
    }
    if (35 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 35, 0);
        data->mem = IPPC_FEM_BIT_SELECTm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_ippc_fem_bit_select_default_get;
    }
    if (36 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 36, 0);
        data->mem = IPPC_FEM_MAP_INDEX_TABLEm;
        data->mode = dnx_init_mem_default_mode_zero;
        data->field = VALIDf;
    }
    if (37 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 37, 0);
        data->mem = TCAM_TCAM_BANK_COMMANDm;
        data->mode = dnx_init_mem_default_mode_custom_per_entry;
        data->default_get_cb = dnx_init_custom_tcam_tcam_bank_command_default_get;
    }
    if (38 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 38, 0);
        data->mem = TCAM_TCAM_ACTIONm;
        data->mode = dnx_init_mem_default_mode_zero;
        data->field = ACTIONf;
    }
    if (39 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 39, 0);
        data->mem = TCAM_TCAM_ACTION_SMALLm;
        data->mode = dnx_init_mem_default_mode_zero;
        data->field = ACTIONf;
    }
    if (40 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 40, 0);
        data->mem = ETPPB_MTU_MAP_TABLEm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = MTUf;
    }
    if (41 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 41, 0);
        data->mem = ETPPC_MTU_MAP_TABLEm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = MTUf;
    }
    if (42 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 42, 0);
        data->mem = MACSEC_ESEC_SC_TABLEm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_xflow_macsec_default_esec_sc_entry_get;
        data->valid = dnx_data_macsec.general.is_macsec_enabled_get(unit);
    }
    if (43 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 43, 0);
        data->mem = MACSEC_SUB_PORT_POLICY_TABLEm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_xflow_macsec_default_isec_policy_entry_get;
        data->valid = dnx_data_macsec.general.is_macsec_enabled_get(unit);
    }
    if (44 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 44, 0);
        data->mem = IPPB_PINFO_FLP_MEMm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_sa_not_found_action_profile_default_get;
        data->field = ACTION_PROFILE_SA_NOT_FOUND_INDEXf;
    }
    if (45 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 45, 0);
        data->mem = ERPP_EPMFCS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (46 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 46, 0);
        data->mem = IPPA_VTDCS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (47 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 47, 0);
        data->mem = IPPA_VTECS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (48 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 48, 0);
        data->mem = IPPB_FLPACS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (49 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 49, 0);
        data->mem = IPPB_FLPBCS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (50 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 50, 0);
        data->mem = IPPC_PMFACSA_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (51 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 51, 0);
        data->mem = IPPC_PMFACSB_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (52 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 52, 0);
        data->mem = IPPD_PMFBCS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (53 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 53, 0);
        data->mem = IPPE_LLRCS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (54 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 54, 0);
        data->mem = IPPE_PRTCAM_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (55 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 55, 0);
        data->mem = IPPF_VTACS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (56 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 56, 0);
        data->mem = IPPF_VTBCS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (57 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 57, 0);
        data->mem = IPPF_VTCCS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (58 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 58, 0);
        data->mem = TCAM_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (59 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 59, 0);
        data->mem = SPEED_ID_TABLEm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (60 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 60, 0);
        data->mem = SPEED_PRIORITY_MAP_TBLm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (61 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 61, 0);
        data->mem = OCB_OCB_CPU_ACCESSm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (62 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 62, 0);
        data->mem = DQM_BDMm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (63 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 63, 0);
        data->mem = IPPB_FLPA_FWD_DOMAIN_STATISTICS_OBJECT_COMMAND_MAPm;
        data->mode = dnx_init_mem_default_mode_index;
    }
    if (64 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 64, 0);
        data->mem = IPPB_FLPB_FWD_DOMAIN_STATISTICS_OBJECT_COMMAND_MAPm;
        data->mode = dnx_init_mem_default_mode_index;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}

#ifdef BCM_ACCESS_SUPPORT


static shr_error_e
j2p_a0_dnx_data_dev_init_mem_sw_mem_init_set(
    int unit)
{
    int index_index;
    dnx_data_dev_init_mem_sw_mem_init_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_mem;
    int table_index = dnx_data_dev_init_mem_table_sw_mem_init;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 65;
    table->info_get.key_size[0] = 65;

    
    table->values[0].default_val = "INVALIDr";
    table->values[1].default_val = "dnx_sw_init_mem_mode_none";
    table->values[2].default_val = "NULL";
    table->values[3].default_val = "NULL";
    table->values[4].default_val = "ACCESS_INVALID_FIELD_ID";
    table->values[5].default_val = "1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dev_init_mem_sw_mem_init_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dev_init_mem_table_sw_mem_init");

    
    default_data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->mem = INVALIDr;
    default_data->mode = dnx_sw_init_mem_mode_none;
    default_data->sw_mem_init_get_cb = NULL;
    default_data->sw_mem_init_get_fill_custom_value_cb = NULL;
    default_data->field = ACCESS_INVALID_FIELD_ID;
    default_data->valid = 1;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->mem = mIPS_SPM;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_fill_all_ones_get;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->mem = mMTM_EGR_BITMAP_MAPPING_TABLE_A;
        data->mode = dnx_sw_init_mem_mode_custom_per_entry;
        data->sw_mem_init_get_cb = dnx_access_init_index_value_get;
        data->field = mMTM_EGR_BITMAP_MAPPING_TABLE_A_fPP_DSP;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->mem = mMTM_EGR_BITMAP_MAPPING_TABLE_B;
        data->mode = dnx_sw_init_mem_mode_custom_per_entry;
        data->sw_mem_init_get_cb = dnx_access_init_index_value_get;
        data->field = mMTM_EGR_BITMAP_MAPPING_TABLE_B_fPP_DSP;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->mem = mERPP_PRP_PP_DSP_PTR_TABLE;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_erpp_prp_pp_dsp_ptr_value_get;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->mem = mERPP_TM_PP_DSP_PTR_TABLE;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_erpp_tm_pp_dsp_ptr_value_get;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->mem = mETPPA_DSP_DATA_TABLE;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_etppa_dsp_data_value_get;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->mem = mIPPE_PRT_PP_PORT_INFO;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_ippe_prt_pp_port_info_value_get;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->mem = mCGM_REP_CMD_FIFO_OCCUPANCY_RJCT_TH;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_fill_all_ones_get;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->mem = mSPB_CONTEXT_MRU;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_spb_context_mru_value_get;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->mem = mEPNI_ALIGNER_MIRR_QP_TO_CHANNEL_MAP;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_epni_aligner_mirr_qp_to_channel_map_value_get;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->mem = mSCH_FLOW_TO_FIP_MAPPING_FFM;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_sch_flow_to_fip_mapping_value_get;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->mem = mERPP_CFG_ENABLE_FILTER_PER_PORT_TABLE;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_erpp_cfg_enable_filter_per_port_table_value_get;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->mem = mETPPB_VID_MIRROR_PROFILE_TABLE;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_fill_all_ones_get;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->mem = mSCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_sch_shaper_descr_value_get;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->mem = mSCH_FLOW_DESCRIPTOR_MEMORY_STATIC_FDMS;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_sch_flow_descr_value_get;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->mem = mIPT_PCP_CFG;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_fabric_pcp_value_get;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->mem = mSCH_SLOW_SCALE_A_SSA;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_sch_slow_scale_a_ssa_value_get;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->mem = mSCH_SLOW_SCALE_B_SSB;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_sch_slow_scale_b_ssb_value_get;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->mem = mIPPE_PINFO_LLR;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_ippe_pinfo_llr_value_get;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->mem = mMTM_MCDB;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_fill_all_ones_get;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->mem = mSCH_CIR_SHAPER_CALENDAR_CSC;
        data->mode = dnx_sw_init_mem_mode_custom_per_entry;
        data->sw_mem_init_get_cb = dnx_access_init_index_value_get;
        data->field = mSCH_CIR_SHAPER_CALENDAR_CSC_fPG_NUM;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->mem = mSCH_RESERVED_51;
        data->mode = dnx_sw_init_mem_mode_custom_per_entry;
        data->sw_mem_init_get_cb = dnx_access_init_index_value_get;
        data->field = mSCH_RESERVED_51_fHR_NUM;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->mem = mSCH_PORT_SCHEDULER_WEIGHTS_PSW;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_sch_port_scheduler_weights_psw_value_get;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->mem = mSCH_RCI_CREDIT_JITTER_HIGH_MAP;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_fill_all_ones_get;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->mem = mIPPD_DESTINATION_STATUS;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_fill_all_ones_get;
    }
    if (25 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, 0);
        data->mem = mIPS_QSPM;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_ips_qspm_value_get;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->mem = mCGM_QSPM;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_cgm_qspm_value_get;
    }
    if (27 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, 0);
        data->mem = mIPS_QPRIORITY;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_ips_qpriority_value_get;
    }
    if (28 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 28, 0);
        data->mem = mCGM_IPP_MAP;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_cgm_ipp_map_value_get;
    }
    if (29 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 29, 0);
        data->mem = mIPPC_FES_2ND_INSTRUCTION;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_ippc_fes_2nd_instruction_value_get;
    }
    if (30 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 30, 0);
        data->mem = mIPPD_FES_2ND_INSTRUCTION;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_ippd_fes_2nd_instruction_value_get;
    }
    if (31 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 31, 0);
        data->mem = mERPP_FES_2ND_INSTRUCTION;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_erpp_fes_2nd_instruction_value_get;
    }
    if (32 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 32, 0);
        data->mem = mETPPA_PRP_FES_PROGRAM_TABLE;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_etppa_prp_fes_program_table_value_get;
    }
    if (33 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 33, 0);
        data->mem = mSCH_PIR_SHAPERS_STATIC_TABEL_PSST;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_sch_psst_value_get;
    }
    if (34 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 34, 0);
        data->mem = mSCH_CIR_SHAPERS_STATIC_TABEL_CSST;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_sch_csst_value_get;
    }
    if (35 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 35, 0);
        data->mem = mIPPC_FEM_BIT_SELECT;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_ippc_fem_bit_select_value_get;
    }
    if (36 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 36, 0);
        data->mem = mIPPC_FEM_MAP_INDEX_TABLE;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_fill_all_zeros_get;
        data->field = mIPPC_FEM_MAP_INDEX_TABLE_fVALID;
    }
    if (37 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 37, 0);
        data->mem = mTCAM_TCAM_BANK_COMMAND;
        data->mode = dnx_sw_init_mem_mode_custom_per_entry;
        data->sw_mem_init_get_cb = dnx_access_init_custom_tcam_tcam_bank_command_value_get;
    }
    if (38 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 38, 0);
        data->mem = mTCAM_TCAM_ACTION;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_fill_all_zeros_get;
        data->field = mTCAM_TCAM_ACTION_fACTION;
    }
    if (39 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 39, 0);
        data->mem = mTCAM_TCAM_ACTION_SMALL;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_fill_all_zeros_get;
        data->field = mTCAM_TCAM_ACTION_SMALL_fACTION;
    }
    if (40 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 40, 0);
        data->mem = mETPPB_MTU_MAP_TABLE;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_etppb_mtu_map_table_value_get;
    }
    if (41 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 41, 0);
        data->mem = mETPPC_MTU_MAP_TABLE;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_etppc_mtu_map_table_value_get;
    }
    if (42 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 42, 0);
        data->mem = mMACSEC_ESEC_SC_TABLE;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_xflow_macsec_esec_sc_entry_value_get;
        data->valid = dnx_data_macsec.general.is_macsec_enabled_get(unit);
    }
    if (43 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 43, 0);
        data->mem = mMACSEC_SUB_PORT_POLICY_TABLE;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_xflow_macsec_isec_policy_entry_value_get;
        data->valid = dnx_data_macsec.general.is_macsec_enabled_get(unit);
    }
    if (44 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 44, 0);
        data->mem = mIPPB_PINFO_FLP_MEM;
        data->mode = dnx_sw_init_mem_mode_custom_fill_value;
        data->sw_mem_init_get_fill_custom_value_cb = dnx_access_init_custom_sa_not_found_action_profile_value_get;
        data->field = mIPPB_PINFO_FLP_MEM_fACTION_PROFILE_SA_NOT_FOUND_INDEX;
    }
    if (45 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 45, 0);
        data->mem = mERPP_EPMFCS_TCAM_BANK;
        data->mode = dnx_sw_init_mem_mode_none;
    }
    if (46 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 46, 0);
        data->mem = mIPPA_VTDCS_TCAM_BANK;
        data->mode = dnx_sw_init_mem_mode_none;
    }
    if (47 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 47, 0);
        data->mem = mIPPA_VTECS_TCAM_BANK;
        data->mode = dnx_sw_init_mem_mode_none;
    }
    if (48 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 48, 0);
        data->mem = mIPPB_FLPACS_TCAM_BANK;
        data->mode = dnx_sw_init_mem_mode_none;
    }
    if (49 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 49, 0);
        data->mem = mIPPB_FLPBCS_TCAM_BANK;
        data->mode = dnx_sw_init_mem_mode_none;
    }
    if (50 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 50, 0);
        data->mem = mIPPC_PMFACSA_TCAM_BANK;
        data->mode = dnx_sw_init_mem_mode_none;
    }
    if (51 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 51, 0);
        data->mem = mIPPC_PMFACSB_TCAM_BANK;
        data->mode = dnx_sw_init_mem_mode_none;
    }
    if (52 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 52, 0);
        data->mem = mIPPD_PMFBCS_TCAM_BANK;
        data->mode = dnx_sw_init_mem_mode_none;
    }
    if (53 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 53, 0);
        data->mem = mIPPE_LLRCS_TCAM_BANK;
        data->mode = dnx_sw_init_mem_mode_none;
    }
    if (54 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 54, 0);
        data->mem = mIPPE_PRTCAM_TCAM_BANK;
        data->mode = dnx_sw_init_mem_mode_none;
    }
    if (55 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 55, 0);
        data->mem = mIPPF_VTACS_TCAM_BANK;
        data->mode = dnx_sw_init_mem_mode_none;
    }
    if (56 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 56, 0);
        data->mem = mIPPF_VTBCS_TCAM_BANK;
        data->mode = dnx_sw_init_mem_mode_none;
    }
    if (57 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 57, 0);
        data->mem = mIPPF_VTCCS_TCAM_BANK;
        data->mode = dnx_sw_init_mem_mode_none;
    }
    if (58 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 58, 0);
        data->mem = mTCAM_TCAM_BANK;
        data->mode = dnx_sw_init_mem_mode_none;
    }
    if (59 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 59, 0);
        data->mem = mSPEED_ID_TABLE;
        data->mode = dnx_sw_init_mem_mode_none;
    }
    if (60 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 60, 0);
        data->mem = mSPEED_PRIORITY_MAP_TBL;
        data->mode = dnx_sw_init_mem_mode_none;
    }
    if (61 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 61, 0);
        data->mem = mOCBM_OCB_CPU_ACCESS;
        data->mode = dnx_sw_init_mem_mode_none;
    }
    if (62 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 62, 0);
        data->mem = mDQM_BDM;
        data->mode = dnx_sw_init_mem_mode_none;
    }
    if (63 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 63, 0);
        data->mem = mIPPB_FLPA_FWD_DOMAIN_STATISTICS_OBJECT_COMMAND_MAP;
        data->mode = dnx_sw_init_mem_mode_custom_per_entry;
        data->sw_mem_init_get_cb = dnx_access_init_index_value_get;
    }
    if (64 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_sw_mem_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 64, 0);
        data->mem = mIPPB_FLPB_FWD_DOMAIN_STATISTICS_OBJECT_COMMAND_MAP;
        data->mode = dnx_sw_init_mem_mode_custom_per_entry;
        data->sw_mem_init_get_cb = dnx_access_init_index_value_get;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


#endif 

static shr_error_e
j2p_a0_dnx_data_dev_init_mem_emul_ext_init_path_set(
    int unit)
{
    dnx_data_dev_init_mem_emul_ext_init_path_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_mem;
    int table_index = dnx_data_dev_init_mem_table_emul_ext_init_path;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "/projects/ntsw_dnx/leon/reset.sh.j2p";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_dev_init_mem_emul_ext_init_path_t, (1 + 1  ), "data of dnx_data_dev_init_mem_table_emul_ext_init_path");

    
    default_data = (dnx_data_dev_init_mem_emul_ext_init_path_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->full_path = "/projects/ntsw_dnx/leon/reset.sh.j2p";
    
    data = (dnx_data_dev_init_mem_emul_ext_init_path_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->values[0].property.name = spn_CUSTOM_FEATURE;
    table->values[0].property.doc =
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_suffix_str;
    table->values[0].property.method_str = "suffix_str";
    table->values[0].property.suffix = "emul_ext_init_path";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnx_data_dev_init_mem_emul_ext_init_path_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, -1, &data->full_path));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);
    table->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2p_a0_dnx_data_dev_init_general_data_path_hw_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_data_path_hw;
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
j2p_a0_dnx_data_dev_init_general_remove_crc_bytes_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_remove_crc_bytes;
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
j2p_a0_dnx_data_dev_init_general_remove_crc_bytes_capability_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_remove_crc_bytes_capability;
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
j2p_a0_dnx_data_dev_init_general_erpp_compensate_crc_size_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_erpp_compensate_crc_size;
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
j2p_a0_dnx_data_dev_init_general_etpp_compensate_crc_size_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_etpp_compensate_crc_size;
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
j2p_a0_dnx_data_dev_init_general_tail_editing_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_tail_editing_enable;
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
j2p_a0_dnx_data_dev_init_general_tail_editing_append_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_tail_editing_append_enable;
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
j2p_a0_dnx_data_dev_init_general_tail_editing_truncate_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_tail_editing_truncate_enable;
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
j2p_a0_dnx_data_dev_init_general_current_packet_size_compensation_term_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_current_packet_size_compensation_term;
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
j2p_a0_dnx_data_dev_init_general_erpp_filters_non_separate_enablers_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_erpp_filters_non_separate_enablers;
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
j2p_a0_dnx_data_dev_init_general_etppa_bypass_stamp_ftmh_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_etppa_bypass_stamp_ftmh_enable;
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
j2p_a0_dnx_data_dev_init_general_l4_protocol_fields_config_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_l4_protocol_fields_config_enable;
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
j2p_a0_dnx_data_dev_init_general_da_type_map_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_da_type_map_enable;
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
j2p_a0_dnx_data_dev_init_general_ecologic_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_ecologic_support;
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
j2p_a0_dnx_data_dev_init_general_context_selection_index_valid_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_context_selection_index_valid;
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
j2p_a0_dnx_data_dev_init_general_network_header_termination_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_network_header_termination;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = dnx_data_headers.system_headers.system_headers_mode_get(unit)==dnx_data_headers.system_headers.system_headers_mode_jericho2_get(unit)?1:0;

    
    feature->data = dnx_data_headers.system_headers.system_headers_mode_get(unit)==dnx_data_headers.system_headers.system_headers_mode_jericho2_get(unit)?1:0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_general_network_offset_for_system_headers_valid_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_network_offset_for_system_headers_valid;
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
j2p_a0_dnx_data_dev_init_general_egress_estimated_bta_btr_hw_config_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_egress_estimated_bta_btr_hw_config;
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
j2p_a0_dnx_data_dev_init_general_erpp_threshold_prp_per_port_info_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_erpp_threshold_prp_per_port_info;
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
j2p_a0_dnx_data_dev_init_general_ippe_sync_counter_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_ippe_sync_counter;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 675;

    
    feature->data = 675;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_general_mem_conf_test_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int feature_index = dnx_data_dev_init_general_mem_conf_test;
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
j2p_a0_dnx_data_dev_init_general_access_only_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int define_index = dnx_data_dev_init_general_define_access_only;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "access_only";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_general_heat_up_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int define_index = dnx_data_dev_init_general_define_heat_up;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "heat_up";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2p_a0_dnx_data_dev_init_general_clock_power_down_set(
    int unit)
{
    int index_index;
    dnx_data_dev_init_general_clock_power_down_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_general;
    int table_index = dnx_data_dev_init_general_table_clock_power_down;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 3;
    table->info_get.key_size[0] = 3;

    
    table->values[0].default_val = "INVALIDr";
    table->values[1].default_val = "INVALIDf";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dev_init_general_clock_power_down_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dev_init_general_table_clock_power_down");

    
    default_data = (dnx_data_dev_init_general_clock_power_down_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->reg = INVALIDr;
    default_data->field = INVALIDf;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dev_init_general_clock_power_down_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_general_clock_power_down_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->reg = CDBM_SPECIAL_CLOCK_CONTROLSr;
        data->field = CLOCK_PD_POWER_DOWNf;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_general_clock_power_down_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->reg = CDB_SPECIAL_CLOCK_CONTROLSr;
        data->field = CLOCK_PD_POWER_DOWNf;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_general_clock_power_down_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->reg = ILE_SPECIAL_CLOCK_CONTROLSr;
        data->field = CLOCK_PD_POWER_DOWNf;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2p_a0_dnx_data_dev_init_context_context_selection_profile_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_context;
    int feature_index = dnx_data_dev_init_context_context_selection_profile;
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
j2p_a0_dnx_data_dev_init_context_oam_sub_type_in_context_selection_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_context;
    int feature_index = dnx_data_dev_init_context_oam_sub_type_in_context_selection;
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
j2p_a0_dnx_data_dev_init_context_int_profile_in_context_selection_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_context;
    int feature_index = dnx_data_dev_init_context_int_profile_in_context_selection;
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
j2p_a0_dnx_data_dev_init_context_etps_selection_profile_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_context;
    int feature_index = dnx_data_dev_init_context_etps_selection_profile;
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
j2p_a0_dnx_data_dev_init_context_forwarding_context_selection_mask_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_context;
    int define_index = dnx_data_dev_init_context_define_forwarding_context_selection_mask_offset;
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
j2p_a0_dnx_data_dev_init_context_forwarding_context_selection_result_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_context;
    int define_index = dnx_data_dev_init_context_define_forwarding_context_selection_result_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 78;

    
    define->data = 78;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_context_termination_context_selection_mask_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_context;
    int define_index = dnx_data_dev_init_context_define_termination_context_selection_mask_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 50;

    
    define->data = 50;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_context_termination_context_selection_result_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_context;
    int define_index = dnx_data_dev_init_context_define_termination_context_selection_result_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 100;

    
    define->data = 100;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_context_trap_context_selection_mask_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_context;
    int define_index = dnx_data_dev_init_context_define_trap_context_selection_mask_offset;
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
j2p_a0_dnx_data_dev_init_context_trap_context_selection_result_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_context;
    int define_index = dnx_data_dev_init_context_define_trap_context_selection_result_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 62;

    
    define->data = 62;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_context_prp_pem_context_selection_mask_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_context;
    int define_index = dnx_data_dev_init_context_define_prp_pem_context_selection_mask_offset;
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
j2p_a0_dnx_data_dev_init_context_prp_context_selection_result_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_context;
    int define_index = dnx_data_dev_init_context_define_prp_context_selection_result_offset;
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
j2p_a0_dnx_data_dev_init_context_fwd_reycle_priority_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_context;
    int define_index = dnx_data_dev_init_context_define_fwd_reycle_priority_size;
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
j2p_a0_dnx_data_dev_init_ha_warmboot_support_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_ha;
    int define_index = dnx_data_dev_init_ha_define_warmboot_support;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_WARMBOOT_SUPPORT;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "on";
    define->property.mapping[0].val = 1;
    define->property.mapping[1].name = "off";
    define->property.mapping[1].val = 0;
    define->property.mapping[1].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_ha_sw_state_max_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_ha;
    int define_index = dnx_data_dev_init_ha_define_sw_state_max_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_SW_STATE_MAX_SIZE;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 1000000000;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_ha_stable_location_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_ha;
    int define_index = dnx_data_dev_init_ha_define_stable_location;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_STABLE_LOCATION;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 5;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dev_init_ha_stable_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_ha;
    int define_index = dnx_data_dev_init_ha_define_stable_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_STABLE_SIZE;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 1000000000;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2p_a0_dnx_data_dev_init_ha_stable_filename_set(
    int unit)
{
    dnx_data_dev_init_ha_stable_filename_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_ha;
    int table_index = dnx_data_dev_init_ha_table_stable_filename;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "NULL";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_dev_init_ha_stable_filename_t, (1 + 1  ), "data of dnx_data_dev_init_ha_table_stable_filename");

    
    default_data = (dnx_data_dev_init_ha_stable_filename_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->val = NULL;
    
    data = (dnx_data_dev_init_ha_stable_filename_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->values[0].property.name = spn_STABLE_FILENAME;
    table->values[0].property.doc =
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_str;
    table->values[0].property.method_str = "str";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnx_data_dev_init_ha_stable_filename_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, -1, &data->val));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2p_a0_dnx_data_dev_init_shadow_uncacheable_mem_set(
    int unit)
{
    int index_index;
    dnx_data_dev_init_shadow_uncacheable_mem_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_shadow;
    int table_index = dnx_data_dev_init_shadow_table_uncacheable_mem;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 130;
    table->info_get.key_size[0] = 130;

    
    table->values[0].default_val = "INVALIDm";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dev_init_shadow_uncacheable_mem_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dev_init_shadow_table_uncacheable_mem");

    
    default_data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->mem = INVALIDm;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->mem = CDPORT_TSC_UCMEM_DATAm;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->mem = FSRD_FSRD_PROM_MEMm;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->mem = ILE_PORT_0_CPU_ACCESSm;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->mem = ILE_PORT_1_CPU_ACCESSm;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->mem = IPPF_LEARN_PAYLOAD_CONTEXT_MAPPINGm;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->mem = SCH_SCHEDULER_INITm;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->mem = ERPP_ERPP_DEBUG_UNITm;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->mem = ETPPA_ETPPA_DEBUG_UNITm;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->mem = ETPPB_ETPPB_DEBUG_UNITm;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->mem = ETPPC_ETPPC_DEBUG_UNITm;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->mem = IPPA_IPPA_DEBUG_UNITm;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->mem = IPPB_IPPB_DEBUG_UNITm;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->mem = IPPC_IPPC_DEBUG_UNITm;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->mem = IPPD_IPPD_DEBUG_UNITm;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->mem = IPPE_IPPE_DEBUG_UNITm;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->mem = IPPF_IPPF_DEBUG_UNITm;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->mem = RX_LKUP_1588_MEM_400Gm;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->mem = RX_LKUP_1588_MEM_MPP0m;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->mem = RX_LKUP_1588_MEM_MPP1m;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->mem = SPEED_ID_TABLEm;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->mem = SPEED_PRIORITY_MAP_TBLm;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->mem = TX_LKUP_1588_MEM_400Gm;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->mem = TX_LKUP_1588_MEM_MPP0m;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->mem = TX_LKUP_1588_MEM_MPP1m;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->mem = UM_TABLEm;
    }
    if (25 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, 0);
        data->mem = AM_TABLEm;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->mem = MDB_ARM_KAPS_TCMm;
    }
    if (27 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, 0);
        data->mem = ERPP_EPMFCS_TCAM_BANK_COMMANDm;
    }
    if (28 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 28, 0);
        data->mem = IPPA_VTDCS_TCAM_BANK_COMMANDm;
    }
    if (29 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 29, 0);
        data->mem = IPPA_VTECS_TCAM_BANK_COMMANDm;
    }
    if (30 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 30, 0);
        data->mem = IPPB_FLPACS_TCAM_BANK_COMMANDm;
    }
    if (31 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 31, 0);
        data->mem = IPPB_FLPBCS_TCAM_BANK_COMMANDm;
    }
    if (32 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 32, 0);
        data->mem = IPPC_PMFACSA_TCAM_BANK_COMMANDm;
    }
    if (33 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 33, 0);
        data->mem = IPPC_PMFACSB_TCAM_BANK_COMMANDm;
    }
    if (34 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 34, 0);
        data->mem = IPPD_PMFBCS_TCAM_BANK_COMMANDm;
    }
    if (35 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 35, 0);
        data->mem = IPPE_LLRCS_TCAM_BANK_COMMANDm;
    }
    if (36 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 36, 0);
        data->mem = IPPE_PRTCAM_TCAM_BANK_COMMANDm;
    }
    if (37 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 37, 0);
        data->mem = IPPF_VTACS_TCAM_BANK_COMMANDm;
    }
    if (38 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 38, 0);
        data->mem = IPPF_VTBCS_TCAM_BANK_COMMANDm;
    }
    if (39 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 39, 0);
        data->mem = IPPF_VTCCS_TCAM_BANK_COMMANDm;
    }
    if (40 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 40, 0);
        data->mem = TCAM_TCAM_BANK_COMMANDm;
    }
    if (41 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 41, 0);
        data->mem = ERPP_EPMFCS_TCAM_BANK_REPLYm;
    }
    if (42 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 42, 0);
        data->mem = IPPA_VTDCS_TCAM_BANK_REPLYm;
    }
    if (43 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 43, 0);
        data->mem = IPPA_VTECS_TCAM_BANK_REPLYm;
    }
    if (44 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 44, 0);
        data->mem = IPPB_FLPACS_TCAM_BANK_REPLYm;
    }
    if (45 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 45, 0);
        data->mem = IPPB_FLPBCS_TCAM_BANK_REPLYm;
    }
    if (46 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 46, 0);
        data->mem = IPPC_PMFACSA_TCAM_BANK_REPLYm;
    }
    if (47 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 47, 0);
        data->mem = IPPC_PMFACSB_TCAM_BANK_REPLYm;
    }
    if (48 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 48, 0);
        data->mem = IPPD_PMFBCS_TCAM_BANK_REPLYm;
    }
    if (49 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 49, 0);
        data->mem = IPPE_LLRCS_TCAM_BANK_REPLYm;
    }
    if (50 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 50, 0);
        data->mem = IPPE_PRTCAM_TCAM_BANK_REPLYm;
    }
    if (51 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 51, 0);
        data->mem = IPPF_VTACS_TCAM_BANK_REPLYm;
    }
    if (52 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 52, 0);
        data->mem = IPPF_VTBCS_TCAM_BANK_REPLYm;
    }
    if (53 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 53, 0);
        data->mem = IPPF_VTCCS_TCAM_BANK_REPLYm;
    }
    if (54 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 54, 0);
        data->mem = TCAM_TCAM_BANK_REPLYm;
    }
    if (55 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 55, 0);
        data->mem = ERPP_EPMFCS_TCAM_BANKm;
    }
    if (56 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 56, 0);
        data->mem = IPPA_VTDCS_TCAM_BANKm;
    }
    if (57 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 57, 0);
        data->mem = IPPA_VTECS_TCAM_BANKm;
    }
    if (58 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 58, 0);
        data->mem = IPPB_FLPACS_TCAM_BANKm;
    }
    if (59 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 59, 0);
        data->mem = IPPB_FLPBCS_TCAM_BANKm;
    }
    if (60 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 60, 0);
        data->mem = IPPC_PMFACSA_TCAM_BANKm;
    }
    if (61 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 61, 0);
        data->mem = IPPC_PMFACSB_TCAM_BANKm;
    }
    if (62 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 62, 0);
        data->mem = IPPD_PMFBCS_TCAM_BANKm;
    }
    if (63 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 63, 0);
        data->mem = IPPE_LLRCS_TCAM_BANKm;
    }
    if (64 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 64, 0);
        data->mem = IPPE_PRTCAM_TCAM_BANKm;
    }
    if (65 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 65, 0);
        data->mem = IPPF_VTACS_TCAM_BANKm;
    }
    if (66 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 66, 0);
        data->mem = IPPF_VTBCS_TCAM_BANKm;
    }
    if (67 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 67, 0);
        data->mem = IPPF_VTCCS_TCAM_BANKm;
    }
    if (68 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 68, 0);
        data->mem = TCAM_TCAM_BANKm;
    }
    if (69 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 69, 0);
        data->mem = SCH_TOKEN_MEMORY_CONTROLLER_TMCm;
    }
    if (70 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 70, 0);
        data->mem = MDB_ARM_MEM_0m;
    }
    if (71 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 71, 0);
        data->mem = MDB_ISEM_1m;
    }
    if (72 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 72, 0);
        data->mem = MDB_ISEM_2m;
    }
    if (73 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 73, 0);
        data->mem = MDB_ISEM_3m;
    }
    if (74 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 74, 0);
        data->mem = MDB_IOEM_0m;
    }
    if (75 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 75, 0);
        data->mem = MDB_IOEM_1m;
    }
    if (76 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 76, 0);
        data->mem = MDB_MC_IDm;
    }
    if (77 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 77, 0);
        data->mem = MDB_GLEM_0m;
    }
    if (78 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 78, 0);
        data->mem = MDB_GLEM_1m;
    }
    if (79 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 79, 0);
        data->mem = MDB_EOEM_0m;
    }
    if (80 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 80, 0);
        data->mem = MDB_EOEM_1m;
    }
    if (81 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 81, 0);
        data->mem = EVNT_ASSIGNED_CMIC_ENGINE_TABLEm;
    }
    if (82 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 82, 0);
        data->mem = EVNT_SPACE_REGIONSm;
    }
    if (83 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 83, 0);
        data->mem = EVNT_CMIC_ENG_PROPERTIESm;
    }
    if (84 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 84, 0);
        data->mem = OAMP_MEP_DBm;
    }
    if (85 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 85, 0);
        data->mem = OAMP_RMEP_DBm;
    }
    if (86 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 86, 0);
        data->mem = MDB_ESEMm;
    }
    if (87 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 87, 0);
        data->mem = MDB_EXEM_1m;
    }
    if (88 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 88, 0);
        data->mem = MDB_EXEM_2m;
    }
    if (89 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 89, 0);
        data->mem = MDB_RMEPm;
    }
    if (90 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 90, 0);
        data->mem = SCH_RESERVED_54m;
    }
    if (91 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 91, 0);
        data->mem = SCH_RESERVED_37m;
    }
    if (92 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 92, 0);
        data->mem = ETPPC_LATENCY_TABLEm;
    }
    if (93 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 93, 0);
        data->mem = OAMP_RXP_R_MEP_INDEX_FIFOm;
    }
    if (94 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 94, 0);
        data->mem = HBMC_HBM_PHY_REGISTER_ACCESSm;
    }
    if (95 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 95, 0);
        data->mem = IPS_MEM_740000m;
    }
    if (96 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 96, 0);
        data->mem = IPS_MEM_D60000m;
    }
    if (97 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 97, 0);
        data->mem = MACSEC_ISEC_SA_HASH_TABLEm;
    }
    if (98 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 98, 0);
        data->mem = MACSEC_ESEC_SA_HASH_TABLEm;
    }
    if (99 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 99, 0);
        data->mem = MACSEC_ESEC_MIB_MISCm;
    }
    if (100 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 100, 0);
        data->mem = MACSEC_ESEC_MIB_SAm;
    }
    if (101 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 101, 0);
        data->mem = MACSEC_ESEC_MIB_SCm;
    }
    if (102 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 102, 0);
        data->mem = MACSEC_ESEC_MIB_SC_CTRLm;
    }
    if (103 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 103, 0);
        data->mem = MACSEC_ESEC_MIB_SC_UN_CTRLm;
    }
    if (104 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 104, 0);
        data->mem = MACSEC_ESEC_SA_TABLEm;
    }
    if (105 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 105, 0);
        data->mem = MACSEC_ESEC_SC_TABLEm;
    }
    if (106 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 106, 0);
        data->mem = MACSEC_ISEC_MIB_SAm;
    }
    if (107 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 107, 0);
        data->mem = MACSEC_ISEC_MIB_SCm;
    }
    if (108 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 108, 0);
        data->mem = MACSEC_ISEC_MIB_SP_CTRL_1m;
    }
    if (109 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 109, 0);
        data->mem = MACSEC_ISEC_MIB_SP_CTRL_2m;
    }
    if (110 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 110, 0);
        data->mem = MACSEC_ISEC_MIB_SP_UNCTRLm;
    }
    if (111 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 111, 0);
        data->mem = MACSEC_ISEC_PORT_COUNTERSm;
    }
    if (112 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 112, 0);
        data->mem = MACSEC_ISEC_SA_TABLEm;
    }
    if (113 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 113, 0);
        data->mem = MACSEC_ISEC_SCTCAM_HIT_COUNTm;
    }
    if (114 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 114, 0);
        data->mem = MACSEC_ISEC_SC_TABLEm;
    }
    if (115 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 115, 0);
        data->mem = MACSEC_ISEC_SC_TCAMm;
    }
    if (116 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 116, 0);
        data->mem = MACSEC_ISEC_SPTCAM_HIT_COUNTm;
    }
    if (117 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 117, 0);
        data->mem = MACSEC_ISEC_SP_TCAMm;
    }
    if (118 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 118, 0);
        data->mem = MACSEC_ISEC_SVTAG_CPU_FLEX_MAPm;
    }
    if (119 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 119, 0);
        data->mem = MACSEC_MACSEC_TDM_2_CALENDARm;
    }
    if (120 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 120, 0);
        data->mem = MACSEC_MACSEC_TDM_CALENDARm;
    }
    if (121 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 121, 0);
        data->mem = MACSEC_SC_MAP_TABLEm;
    }
    if (122 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 122, 0);
        data->mem = MACSEC_SUB_PORT_MAP_TABLEm;
    }
    if (123 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 123, 0);
        data->mem = MACSEC_SUB_PORT_POLICY_TABLEm;
    }
    if (124 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 124, 0);
        data->mem = OAMP_SAT_RX_FLOW_PARAMS_EVENm;
    }
    if (125 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 125, 0);
        data->mem = OAMP_SAT_RX_FLOW_PARAMS_ODDm;
    }
    if (126 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 126, 0);
        data->mem = OAMP_SAT_RX_FLOW_ACCUM_STATS_1m;
    }
    if (127 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 127, 0);
        data->mem = OAMP_SAT_RX_FLOW_ACCUM_STATS_2m;
    }
    if (128 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 128, 0);
        data->mem = OAMP_SAT_RX_FLOW_ACCUM_STATS_3m;
    }
    if (129 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 129, 0);
        data->mem = OAMP_SAT_RX_FLOW_ACCUM_STATS_4m;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}

#ifdef BCM_ACCESS_SUPPORT


static shr_error_e
j2p_a0_dnx_data_dev_init_shadow_uncacheable_regmem_set(
    int unit)
{
    int index_index;
    dnx_data_dev_init_shadow_uncacheable_regmem_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_shadow;
    int table_index = dnx_data_dev_init_shadow_table_uncacheable_regmem;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 127;
    table->info_get.key_size[0] = 127;

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dev_init_shadow_uncacheable_regmem_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dev_init_shadow_table_uncacheable_regmem");

    
    default_data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->regmem = -1;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->regmem = mCDPORT_TSC_UCMEM_DATA;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->regmem = mFSRD_FSRD_PROM_MEM;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->regmem = mILE_PORT_0_CPU_ACCESS;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->regmem = mILE_PORT_1_CPU_ACCESS;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->regmem = mIPPF_LEARN_PAYLOAD_CONTEXT_MAPPING;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->regmem = mSCH_SCHEDULER_INIT;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->regmem = mERPP_ERPP_DEBUG_UNIT;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->regmem = mETPPA_ETPPA_DEBUG_UNIT;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->regmem = mETPPB_ETPPB_DEBUG_UNIT;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->regmem = mETPPC_ETPPC_DEBUG_UNIT;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->regmem = mIPPA_IPPA_DEBUG_UNIT;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->regmem = mIPPB_IPPB_DEBUG_UNIT;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->regmem = mIPPC_IPPC_DEBUG_UNIT;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->regmem = mIPPD_IPPD_DEBUG_UNIT;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->regmem = mIPPE_IPPE_DEBUG_UNIT;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->regmem = mIPPF_IPPF_DEBUG_UNIT;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->regmem = mRX_LKUP_1588_MEM_MPP0;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->regmem = mRX_LKUP_1588_MEM_MPP1;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->regmem = mSPEED_ID_TABLE;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->regmem = mSPEED_PRIORITY_MAP_TBL;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->regmem = mTX_LKUP_1588_MEM_MPP0;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->regmem = mTX_LKUP_1588_MEM_MPP1;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->regmem = mUM_TABLE;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->regmem = mAM_TABLE;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->regmem = mMDB_ARM_KAPS_TCM;
    }
    if (25 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, 0);
        data->regmem = mERPP_EPMFCS_TCAM_BANK_COMMAND;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->regmem = mIPPA_VTDCS_TCAM_BANK_COMMAND;
    }
    if (27 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, 0);
        data->regmem = mIPPA_VTECS_TCAM_BANK_COMMAND;
    }
    if (28 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 28, 0);
        data->regmem = mIPPB_FLPACS_TCAM_BANK_COMMAND;
    }
    if (29 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 29, 0);
        data->regmem = mIPPB_FLPBCS_TCAM_BANK_COMMAND;
    }
    if (30 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 30, 0);
        data->regmem = mIPPC_PMFACSA_TCAM_BANK_COMMAND;
    }
    if (31 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 31, 0);
        data->regmem = mIPPC_PMFACSB_TCAM_BANK_COMMAND;
    }
    if (32 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 32, 0);
        data->regmem = mIPPD_PMFBCS_TCAM_BANK_COMMAND;
    }
    if (33 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 33, 0);
        data->regmem = mIPPE_LLRCS_TCAM_BANK_COMMAND;
    }
    if (34 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 34, 0);
        data->regmem = mIPPE_PRTCAM_TCAM_BANK_COMMAND;
    }
    if (35 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 35, 0);
        data->regmem = mIPPF_VTACS_TCAM_BANK_COMMAND;
    }
    if (36 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 36, 0);
        data->regmem = mIPPF_VTBCS_TCAM_BANK_COMMAND;
    }
    if (37 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 37, 0);
        data->regmem = mIPPF_VTCCS_TCAM_BANK_COMMAND;
    }
    if (38 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 38, 0);
        data->regmem = mTCAM_TCAM_BANK_COMMAND;
    }
    if (39 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 39, 0);
        data->regmem = mERPP_EPMFCS_TCAM_BANK_REPLY;
    }
    if (40 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 40, 0);
        data->regmem = mIPPA_VTDCS_TCAM_BANK_REPLY;
    }
    if (41 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 41, 0);
        data->regmem = mIPPA_VTECS_TCAM_BANK_REPLY;
    }
    if (42 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 42, 0);
        data->regmem = mIPPB_FLPACS_TCAM_BANK_REPLY;
    }
    if (43 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 43, 0);
        data->regmem = mIPPB_FLPBCS_TCAM_BANK_REPLY;
    }
    if (44 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 44, 0);
        data->regmem = mIPPC_PMFACSA_TCAM_BANK_REPLY;
    }
    if (45 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 45, 0);
        data->regmem = mIPPC_PMFACSB_TCAM_BANK_REPLY;
    }
    if (46 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 46, 0);
        data->regmem = mIPPD_PMFBCS_TCAM_BANK_REPLY;
    }
    if (47 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 47, 0);
        data->regmem = mIPPE_LLRCS_TCAM_BANK_REPLY;
    }
    if (48 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 48, 0);
        data->regmem = mIPPE_PRTCAM_TCAM_BANK_REPLY;
    }
    if (49 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 49, 0);
        data->regmem = mIPPF_VTACS_TCAM_BANK_REPLY;
    }
    if (50 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 50, 0);
        data->regmem = mIPPF_VTBCS_TCAM_BANK_REPLY;
    }
    if (51 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 51, 0);
        data->regmem = mIPPF_VTCCS_TCAM_BANK_REPLY;
    }
    if (52 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 52, 0);
        data->regmem = mTCAM_TCAM_BANK_REPLY;
    }
    if (53 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 53, 0);
        data->regmem = mERPP_EPMFCS_TCAM_BANK;
    }
    if (54 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 54, 0);
        data->regmem = mIPPA_VTDCS_TCAM_BANK;
    }
    if (55 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 55, 0);
        data->regmem = mIPPA_VTECS_TCAM_BANK;
    }
    if (56 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 56, 0);
        data->regmem = mIPPB_FLPACS_TCAM_BANK;
    }
    if (57 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 57, 0);
        data->regmem = mIPPB_FLPBCS_TCAM_BANK;
    }
    if (58 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 58, 0);
        data->regmem = mIPPC_PMFACSA_TCAM_BANK;
    }
    if (59 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 59, 0);
        data->regmem = mIPPC_PMFACSB_TCAM_BANK;
    }
    if (60 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 60, 0);
        data->regmem = mIPPD_PMFBCS_TCAM_BANK;
    }
    if (61 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 61, 0);
        data->regmem = mIPPE_LLRCS_TCAM_BANK;
    }
    if (62 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 62, 0);
        data->regmem = mIPPE_PRTCAM_TCAM_BANK;
    }
    if (63 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 63, 0);
        data->regmem = mIPPF_VTACS_TCAM_BANK;
    }
    if (64 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 64, 0);
        data->regmem = mIPPF_VTBCS_TCAM_BANK;
    }
    if (65 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 65, 0);
        data->regmem = mIPPF_VTCCS_TCAM_BANK;
    }
    if (66 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 66, 0);
        data->regmem = mTCAM_TCAM_BANK;
    }
    if (67 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 67, 0);
        data->regmem = mSCH_TOKEN_MEMORY_CONTROLLER_TMC;
    }
    if (68 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 68, 0);
        data->regmem = mMDB_ISEM_1;
    }
    if (69 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 69, 0);
        data->regmem = mMDB_ISEM_2;
    }
    if (70 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 70, 0);
        data->regmem = mMDB_ISEM_3;
    }
    if (71 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 71, 0);
        data->regmem = mMDB_IOEM_0;
    }
    if (72 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 72, 0);
        data->regmem = mMDB_IOEM_1;
    }
    if (73 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 73, 0);
        data->regmem = mMDB_MC_ID;
    }
    if (74 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 74, 0);
        data->regmem = mMDB_GLEM_0;
    }
    if (75 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 75, 0);
        data->regmem = mMDB_GLEM_1;
    }
    if (76 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 76, 0);
        data->regmem = mMDB_EOEM_0;
    }
    if (77 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 77, 0);
        data->regmem = mMDB_EOEM_1;
    }
    if (78 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 78, 0);
        data->regmem = mEVNT_ASSIGNED_CMIC_ENGINE_TABLE;
    }
    if (79 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 79, 0);
        data->regmem = mEVNT_SPACE_REGIONS;
    }
    if (80 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 80, 0);
        data->regmem = mEVNT_CMIC_ENG_PROPERTIES;
    }
    if (81 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 81, 0);
        data->regmem = mOAMP_MEP_DB;
    }
    if (82 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 82, 0);
        data->regmem = mOAMP_RMEP_DB;
    }
    if (83 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 83, 0);
        data->regmem = mMDB_ESEM;
    }
    if (84 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 84, 0);
        data->regmem = mMDB_EXEM_1;
    }
    if (85 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 85, 0);
        data->regmem = mMDB_EXEM_2;
    }
    if (86 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 86, 0);
        data->regmem = mMDB_RMEP;
    }
    if (87 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 87, 0);
        data->regmem = mSCH_RESERVED_54;
    }
    if (88 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 88, 0);
        data->regmem = mSCH_RESERVED_37;
    }
    if (89 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 89, 0);
        data->regmem = mETPPC_LATENCY_TABLE;
    }
    if (90 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 90, 0);
        data->regmem = mOAMP_RXP_R_MEP_INDEX_FIFO;
    }
    if (91 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 91, 0);
        data->regmem = mHBMC_HBM_PHY_REGISTER_ACCESS;
    }
    if (92 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 92, 0);
        data->regmem = mIPS_MEM_740000;
    }
    if (93 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 93, 0);
        data->regmem = mIPS_MEM_D60000;
    }
    if (94 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 94, 0);
        data->regmem = mMACSEC_ISEC_SA_HASH_TABLE;
    }
    if (95 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 95, 0);
        data->regmem = mMACSEC_ESEC_SA_HASH_TABLE;
    }
    if (96 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 96, 0);
        data->regmem = mMACSEC_ESEC_MIB_MISC;
    }
    if (97 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 97, 0);
        data->regmem = mMACSEC_ESEC_MIB_SA;
    }
    if (98 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 98, 0);
        data->regmem = mMACSEC_ESEC_MIB_SC;
    }
    if (99 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 99, 0);
        data->regmem = mMACSEC_ESEC_MIB_SC_CTRL;
    }
    if (100 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 100, 0);
        data->regmem = mMACSEC_ESEC_MIB_SC_UN_CTRL;
    }
    if (101 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 101, 0);
        data->regmem = mMACSEC_ESEC_SA_TABLE;
    }
    if (102 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 102, 0);
        data->regmem = mMACSEC_ESEC_SC_TABLE;
    }
    if (103 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 103, 0);
        data->regmem = mMACSEC_ISEC_MIB_SA;
    }
    if (104 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 104, 0);
        data->regmem = mMACSEC_ISEC_MIB_SC;
    }
    if (105 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 105, 0);
        data->regmem = mMACSEC_ISEC_MIB_SP_CTRL_1;
    }
    if (106 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 106, 0);
        data->regmem = mMACSEC_ISEC_MIB_SP_CTRL_2;
    }
    if (107 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 107, 0);
        data->regmem = mMACSEC_ISEC_MIB_SP_UNCTRL;
    }
    if (108 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 108, 0);
        data->regmem = mMACSEC_ISEC_PORT_COUNTERS;
    }
    if (109 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 109, 0);
        data->regmem = mMACSEC_ISEC_SA_TABLE;
    }
    if (110 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 110, 0);
        data->regmem = mMACSEC_ISEC_SCTCAM_HIT_COUNT;
    }
    if (111 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 111, 0);
        data->regmem = mMACSEC_ISEC_SC_TABLE;
    }
    if (112 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 112, 0);
        data->regmem = mMACSEC_ISEC_SC_TCAM;
    }
    if (113 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 113, 0);
        data->regmem = mMACSEC_ISEC_SPTCAM_HIT_COUNT;
    }
    if (114 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 114, 0);
        data->regmem = mMACSEC_ISEC_SP_TCAM;
    }
    if (115 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 115, 0);
        data->regmem = mMACSEC_ISEC_SVTAG_CPU_FLEX_MAP;
    }
    if (116 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 116, 0);
        data->regmem = mMACSEC_MACSEC_TDM_2_CALENDAR;
    }
    if (117 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 117, 0);
        data->regmem = mMACSEC_MACSEC_TDM_CALENDAR;
    }
    if (118 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 118, 0);
        data->regmem = mMACSEC_SC_MAP_TABLE;
    }
    if (119 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 119, 0);
        data->regmem = mMACSEC_SUB_PORT_MAP_TABLE;
    }
    if (120 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 120, 0);
        data->regmem = mMACSEC_SUB_PORT_POLICY_TABLE;
    }
    if (121 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 121, 0);
        data->regmem = mOAMP_SAT_RX_FLOW_PARAMS_EVEN;
    }
    if (122 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 122, 0);
        data->regmem = mOAMP_SAT_RX_FLOW_PARAMS_ODD;
    }
    if (123 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 123, 0);
        data->regmem = mOAMP_SAT_RX_FLOW_ACCUM_STATS_1;
    }
    if (124 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 124, 0);
        data->regmem = mOAMP_SAT_RX_FLOW_ACCUM_STATS_2;
    }
    if (125 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 125, 0);
        data->regmem = mOAMP_SAT_RX_FLOW_ACCUM_STATS_3;
    }
    if (126 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_regmem_t *) dnxc_data_mgmt_table_data_get(unit, table, 126, 0);
        data->regmem = mOAMP_SAT_RX_FLOW_ACCUM_STATS_4;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


#endif 


shr_error_e
j2p_a0_data_dev_init_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_dev_init_submodule_time;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dev_init_time_define_init_total_time;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_time_init_total_time_set;
    data_index = dnx_data_dev_init_time_define_appl_init_total_time;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_time_appl_init_total_time_set;
    data_index = dnx_data_dev_init_time_define_down_deviation_total;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_time_down_deviation_total_set;
    data_index = dnx_data_dev_init_time_define_up_deviation_total;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_time_up_deviation_total_set;
    data_index = dnx_data_dev_init_time_define_test_sensitivity;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_time_test_sensitivity_set;

    

    
    data_index = dnx_data_dev_init_time_table_step_time;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dev_init_time_step_time_set;
    data_index = dnx_data_dev_init_time_table_appl_step_time;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dev_init_time_appl_step_time_set;
    
    submodule_index = dnx_data_dev_init_submodule_mem;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_dev_init_mem_table_default;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dev_init_mem_default_set;
#ifdef BCM_ACCESS_SUPPORT

    data_index = dnx_data_dev_init_mem_table_sw_mem_init;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dev_init_mem_sw_mem_init_set;

#endif 
    data_index = dnx_data_dev_init_mem_table_emul_ext_init_path;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dev_init_mem_emul_ext_init_path_set;
    
    submodule_index = dnx_data_dev_init_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dev_init_general_define_access_only;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_general_access_only_set;
    data_index = dnx_data_dev_init_general_define_heat_up;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_general_heat_up_set;

    
    data_index = dnx_data_dev_init_general_data_path_hw;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_data_path_hw_set;
    data_index = dnx_data_dev_init_general_remove_crc_bytes;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_remove_crc_bytes_set;
    data_index = dnx_data_dev_init_general_remove_crc_bytes_capability;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_remove_crc_bytes_capability_set;
    data_index = dnx_data_dev_init_general_erpp_compensate_crc_size;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_erpp_compensate_crc_size_set;
    data_index = dnx_data_dev_init_general_etpp_compensate_crc_size;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_etpp_compensate_crc_size_set;
    data_index = dnx_data_dev_init_general_tail_editing_enable;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_tail_editing_enable_set;
    data_index = dnx_data_dev_init_general_tail_editing_append_enable;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_tail_editing_append_enable_set;
    data_index = dnx_data_dev_init_general_tail_editing_truncate_enable;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_tail_editing_truncate_enable_set;
    data_index = dnx_data_dev_init_general_current_packet_size_compensation_term;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_current_packet_size_compensation_term_set;
    data_index = dnx_data_dev_init_general_erpp_filters_non_separate_enablers;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_erpp_filters_non_separate_enablers_set;
    data_index = dnx_data_dev_init_general_etppa_bypass_stamp_ftmh_enable;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_etppa_bypass_stamp_ftmh_enable_set;
    data_index = dnx_data_dev_init_general_l4_protocol_fields_config_enable;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_l4_protocol_fields_config_enable_set;
    data_index = dnx_data_dev_init_general_da_type_map_enable;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_da_type_map_enable_set;
    data_index = dnx_data_dev_init_general_ecologic_support;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_ecologic_support_set;
    data_index = dnx_data_dev_init_general_context_selection_index_valid;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_context_selection_index_valid_set;
    data_index = dnx_data_dev_init_general_network_header_termination;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_network_header_termination_set;
    data_index = dnx_data_dev_init_general_network_offset_for_system_headers_valid;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_network_offset_for_system_headers_valid_set;
    data_index = dnx_data_dev_init_general_egress_estimated_bta_btr_hw_config;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_egress_estimated_bta_btr_hw_config_set;
    data_index = dnx_data_dev_init_general_erpp_threshold_prp_per_port_info;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_erpp_threshold_prp_per_port_info_set;
    data_index = dnx_data_dev_init_general_ippe_sync_counter;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_ippe_sync_counter_set;
    data_index = dnx_data_dev_init_general_mem_conf_test;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_general_mem_conf_test_set;

    
    data_index = dnx_data_dev_init_general_table_clock_power_down;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dev_init_general_clock_power_down_set;
    
    submodule_index = dnx_data_dev_init_submodule_context;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dev_init_context_define_forwarding_context_selection_mask_offset;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_context_forwarding_context_selection_mask_offset_set;
    data_index = dnx_data_dev_init_context_define_forwarding_context_selection_result_offset;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_context_forwarding_context_selection_result_offset_set;
    data_index = dnx_data_dev_init_context_define_termination_context_selection_mask_offset;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_context_termination_context_selection_mask_offset_set;
    data_index = dnx_data_dev_init_context_define_termination_context_selection_result_offset;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_context_termination_context_selection_result_offset_set;
    data_index = dnx_data_dev_init_context_define_trap_context_selection_mask_offset;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_context_trap_context_selection_mask_offset_set;
    data_index = dnx_data_dev_init_context_define_trap_context_selection_result_offset;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_context_trap_context_selection_result_offset_set;
    data_index = dnx_data_dev_init_context_define_prp_pem_context_selection_mask_offset;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_context_prp_pem_context_selection_mask_offset_set;
    data_index = dnx_data_dev_init_context_define_prp_context_selection_result_offset;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_context_prp_context_selection_result_offset_set;
    data_index = dnx_data_dev_init_context_define_fwd_reycle_priority_size;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_context_fwd_reycle_priority_size_set;

    
    data_index = dnx_data_dev_init_context_context_selection_profile;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_context_context_selection_profile_set;
    data_index = dnx_data_dev_init_context_oam_sub_type_in_context_selection;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_context_oam_sub_type_in_context_selection_set;
    data_index = dnx_data_dev_init_context_int_profile_in_context_selection;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_context_int_profile_in_context_selection_set;
    data_index = dnx_data_dev_init_context_etps_selection_profile;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_dev_init_context_etps_selection_profile_set;

    
    
    submodule_index = dnx_data_dev_init_submodule_ha;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dev_init_ha_define_warmboot_support;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_ha_warmboot_support_set;
    data_index = dnx_data_dev_init_ha_define_sw_state_max_size;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_ha_sw_state_max_size_set;
    data_index = dnx_data_dev_init_ha_define_stable_location;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_ha_stable_location_set;
    data_index = dnx_data_dev_init_ha_define_stable_size;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dev_init_ha_stable_size_set;

    

    
    data_index = dnx_data_dev_init_ha_table_stable_filename;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dev_init_ha_stable_filename_set;
    
    submodule_index = dnx_data_dev_init_submodule_shadow;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_dev_init_shadow_table_uncacheable_mem;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dev_init_shadow_uncacheable_mem_set;
#ifdef BCM_ACCESS_SUPPORT

    data_index = dnx_data_dev_init_shadow_table_uncacheable_regmem;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_dev_init_shadow_uncacheable_regmem_set;

#endif 

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

