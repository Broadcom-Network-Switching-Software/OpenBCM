

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INIT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_dev_init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <bcm_int/dnx/spb/spb.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/init/init_custom_funcs.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fc.h>
#include <appl/reference/dnx/appl_ref_init_deinit.h>








static shr_error_e
j2c_a0_dnx_data_dev_init_time_init_total_thresh_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_time;
    int define_index = dnx_data_dev_init_time_define_init_total_thresh;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 14919600;

    
    define->data = 14919600;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_dev_init_time_appl_init_total_thresh_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_time;
    int define_index = dnx_data_dev_init_time_define_appl_init_total_thresh;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16258800;

    
    define->data = 16258800;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2c_a0_dnx_data_dev_init_time_step_thresh_set(
    int unit)
{
    int step_id_index;
    dnx_data_dev_init_time_step_thresh_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_time;
    int table_index = dnx_data_dev_init_time_table_step_thresh;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_INIT_STEP_COUNT;
    table->info_get.key_size[0] = DNX_INIT_STEP_COUNT;

    
    table->values[0].default_val = "BCM_INIT_STEP_TIME_THRESH_DEFAULT";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dev_init_time_step_thresh_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dev_init_time_table_step_thresh");

    
    default_data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->value = BCM_INIT_STEP_TIME_THRESH_DEFAULT;
    
    for (step_id_index = 0; step_id_index < table->keys[0].size; step_id_index++)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, step_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_INIT_STEP_DBAL < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_DBAL, 0);
        data->value = 1182000;
    }
    if (DNX_INIT_STEP_TUNE < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_TUNE, 0);
        data->value = 602000;
    }
    if (DNX_INIT_STEP_TM_MODULES < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_TM_MODULES, 0);
        data->value = 6084000;
    }
    if (DNX_INIT_STEP_PP_GENERAL < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_PP_GENERAL, 0);
        data->value = 1440000;
    }
    if (DNX_INIT_STEP_FLOW_CONTROL_MODULE < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_FLOW_CONTROL_MODULE, 0);
        data->value = 150000;
    }
    if (DNX_INIT_MDB < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_MDB, 0);
        data->value = 790800;
    }
    if (DNX_INIT_STEP_PEMLA < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_PEMLA, 0);
        data->value = 408000;
    }
    if (DNX_INIT_STEP_MEM_DEFAULTS < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_MEM_DEFAULTS, 0);
        data->value = 920400;
    }
    if (DNX_INIT_STEP_VISIBILITY < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_VISIBILITY, 0);
        data->value = 145200;
    }
    if (DNX_INIT_STEP_ALGO < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_ALGO, 0);
        data->value = 109200;
    }
    if (DNX_INIT_STEP_SOFT_RESET < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_SOFT_RESET, 0);
        data->value = 151200;
    }
    if (DNX_INIT_STEP_DBAL_ACCESS_MDB < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_DBAL_ACCESS_MDB, 0);
        data->value = 739200;
    }
    if (DNX_INIT_STEP_DBAL_TABLES < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_DBAL_TABLES, 0);
        data->value = 374400;
    }
    if (DNX_INIT_STEP_PP_MODULES < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_PP_MODULES, 0);
        data->value = 1430400;
    }
    if (DNX_INIT_STEP_SW_STATE < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_SW_STATE, 0);
        data->value = 699600;
    }
    if (DNX_INIT_STEP_LIF < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_LIF, 0);
        data->value = 450000;
    }
    if (DNX_INIT_STEP_ALGO_LIF < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_ALGO_LIF, 0);
        data->value = 450000;
    }
    if (DNX_INIT_STEP_AUX_ACCESS < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_AUX_ACCESS, 0);
        data->value = 678000;
    }
    if (DNX_INIT_STEP_SOFT_INIT < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_SOFT_INIT, 0);
        data->value = 351600;
    }
    if (DNX_INIT_STEP_DYN_PORT_PROP_SET < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_DYN_PORT_PROP_SET, 0);
        data->value = 368000;
    }
    if (DNX_INIT_STEP_ACCESS < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_ACCESS, 0);
        data->value = 174000;
    }
    if (DNX_INIT_STEP_DRAM < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_DRAM, 0);
        data->value = 5529600;
    }
    if (DNX_INIT_STEP_PRE_SOFT_INIT < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_PRE_SOFT_INIT, 0);
        data->value = 962400;
    }
    if (DNX_INIT_STEP_APPL_PROPERTIES < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_APPL_PROPERTIES, 0);
        data->value = 1690800;
    }
    if (DNX_INIT_STEP_COMMON_MODULES < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_COMMON_MODULES, 0);
        data->value = 1122000;
    }
    if (DNX_INIT_STEP_DNX_DATA < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_DNX_DATA, 0);
        data->value = 730000;
    }
    if (DNX_INIT_STEP_DYN_PORT_ADD < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_DYN_PORT_ADD, 0);
        data->value = 1231200;
    }
    if (DNX_INIT_STEP_FIELD < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_FIELD, 0);
        data->value = 196800;
    }
    if (DNX_INIT_STEP_FLD_SWSTATE < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_FLD_SWSTATE, 0);
        data->value = 145200;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_dev_init_time_appl_step_thresh_set(
    int unit)
{
    int step_id_index;
    dnx_data_dev_init_time_appl_step_thresh_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_time;
    int table_index = dnx_data_dev_init_time_table_appl_step_thresh;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = APPL_DNX_INIT_STEP_COUNT;
    table->info_get.key_size[0] = APPL_DNX_INIT_STEP_COUNT;

    
    table->values[0].default_val = "APPL_DNX_INIT_STEP_TIME_THRESH_DEFAULT";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dev_init_time_appl_step_thresh_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dev_init_time_table_appl_step_thresh");

    
    default_data = (dnx_data_dev_init_time_appl_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->value = APPL_DNX_INIT_STEP_TIME_THRESH_DEFAULT;
    
    for (step_id_index = 0; step_id_index < table->keys[0].size; step_id_index++)
    {
        data = (dnx_data_dev_init_time_appl_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, step_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (APPL_DNX_STEP_QOS < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_appl_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNX_STEP_QOS, 0);
        data->value = 171600;
    }
    if (APPL_DNX_STEP_SYS_PORTS < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_appl_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNX_STEP_SYS_PORTS, 0);
        data->value = 252000;
    }
    if (APPL_DNX_STEP_VLAN < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_appl_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNX_STEP_VLAN, 0);
        data->value = 174000;
    }
    if (APPL_DNX_STEP_SDK < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_appl_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNX_STEP_SDK, 0);
        data->value = 14988000;
    }
    if (APPL_DNX_STEP_E2E_SCHEME < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_appl_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNX_STEP_E2E_SCHEME, 0);
        data->value = 152400;
    }
    if (APPL_DNX_STEP_LINKSCAN < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_appl_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNX_STEP_LINKSCAN, 0);
        data->value = 217200;
    }
    if (APPL_DNX_STEP_APPS < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_appl_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNX_STEP_APPS, 0);
        data->value = 1273200;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2c_a0_dnx_data_dev_init_mem_default_set(
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

    
    table->keys[0].size = 54;
    table->info_get.key_size[0] = 54;

    
    table->values[0].default_val = "INVALIDr";
    table->values[1].default_val = "dnx_init_mem_default_mode_zero";
    table->values[2].default_val = "NULL";
    table->values[3].default_val = "INVALIDf";
    
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
        data->mem = MTM_EGR_BITMAP_MAPPING_TABLEm;
        data->mode = dnx_init_mem_default_mode_index;
        data->field = PP_DSPf;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->mem = ERPP_PRP_PP_DSP_PTR_TABLEm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_erpp_prp_pp_dsp_ptr_table_get;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->mem = ERPP_TM_PP_DSP_PTR_TABLEm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_erpp_tm_pp_dsp_ptr_table_get;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->mem = ETPPA_DSP_DATA_TABLEm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_etppa_dsp_data_table_get;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->mem = IPS_FMS_BYPASSm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = FMS_BYPASS_BMPf;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->mem = CGM_REP_CMD_FIFO_OCCUPANCY_RJCT_THm;
        data->mode = dnx_init_mem_default_mode_all_ones;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->mem = SPB_CONTEXT_MRUm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_spb_context_mru_default_get_f;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->mem = EPNI_ALIGNER_MIRR_QP_TO_CHANNEL_MAPm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = CHANNELf;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->mem = SCH_FLOW_TO_FIP_MAPPING_FFMm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_sch_flow_to_fip_mapping_default_get;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->mem = ERPP_CFG_ENABLE_FILTER_PER_PORT_TABLEm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = CFG_SAME_INTERFACE_PER_PORTf;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->mem = ETPPB_VID_MIRROR_PROFILE_TABLEm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = OUT_PORT_MIRROR_PROFILEf;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->mem = SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_sch_shaper_descr_default_get;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->mem = SCH_FLOW_DESCRIPTOR_MEMORY_STATIC_FDMSm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_sch_flow_descr_default_get;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->mem = IPT_PCP_CFGm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_fabric_pcp_default_get;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->mem = SCH_SLOW_SCALE_A_SSAm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = MAX_BUCKETf;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->mem = SCH_SLOW_SCALE_B_SSBm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = MAX_BUCKETf;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->mem = IPPE_PINFO_LLRm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = RECYCLE_PROFILE_ENABLEf;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->mem = MTM_MCDBm;
        data->mode = dnx_init_mem_default_mode_all_ones;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->mem = SCH_CIR_SHAPER_CALENDAR_CSCm;
        data->mode = dnx_init_mem_default_mode_index;
        data->field = PG_NUMf;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->mem = SCH_PIR_SHAPER_CALENDAR_PSCm;
        data->mode = dnx_init_mem_default_mode_index;
        data->field = HR_NUMf;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->mem = SCH_PORT_SCHEDULER_WEIGHTS_PSWm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_sch_port_schduler_weights_psw_default_get;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->mem = SCH_RCI_CREDIT_JITTER_HIGH_MAPm;
        data->mode = dnx_init_mem_default_mode_all_ones;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->mem = IPPD_DESTINATION_STATUSm;
        data->mode = dnx_init_mem_default_mode_all_ones;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->mem = IPS_QSPMm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = SYSTEM_PORTf;
    }
    if (25 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, 0);
        data->mem = IPS_QPRIORITYm;
        data->mode = dnx_init_mem_default_mode_all_ones;
        data->field = QPRIORITY_DATAf;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->mem = CGM_IPP_MAPm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_cgm_ipp_map_default_get;
    }
    if (27 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, 0);
        data->mem = IPPC_FES_2ND_INSTRUCTIONm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_ippc_fes_2nd_instruction_default_get;
    }
    if (28 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 28, 0);
        data->mem = IPPD_FES_2ND_INSTRUCTIONm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_ippd_fes_2nd_instruction_default_get;
    }
    if (29 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 29, 0);
        data->mem = ERPP_FES_2ND_INSTRUCTIONm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_erpp_fes_2nd_instruction_default_get;
    }
    if (30 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 30, 0);
        data->mem = ETPPA_PRP_FES_PROGRAM_TABLEm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_etppa_prp_fes_program_table_default_get;
    }
    if (31 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 31, 0);
        data->mem = SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_sch_psst_default_get;
    }
    if (32 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 32, 0);
        data->mem = SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_sch_csst_default_get;
    }
    if (33 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 33, 0);
        data->mem = IPPC_FEM_BIT_SELECTm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_ippc_fem_bit_select_default_get;
    }
    if (34 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 34, 0);
        data->mem = IPPC_FEM_MAP_INDEX_TABLEm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_ippc_fem_map_index_table_default_get;
    }
    if (35 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 35, 0);
        data->mem = TCAM_TCAM_BANK_COMMANDm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_tcam_tcam_bank_command_default_get;
    }
    if (36 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 36, 0);
        data->mem = TCAM_TCAM_ACTIONm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_tcam_tcam_action_default_get;
    }
    if (37 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 37, 0);
        data->mem = TCAM_TCAM_ACTION_SMALLm;
        data->mode = dnx_init_mem_default_mode_custom;
        data->default_get_cb = dnx_init_custom_tcam_tcam_action_small_default_get;
    }
    if (38 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 38, 0);
        data->mem = ERPP_EPMFCS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (39 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 39, 0);
        data->mem = IPPA_VTDCS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (40 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 40, 0);
        data->mem = IPPA_VTECS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (41 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 41, 0);
        data->mem = IPPB_FLPACS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (42 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 42, 0);
        data->mem = IPPB_FLPBCS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (43 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 43, 0);
        data->mem = IPPC_PMFACSA_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (44 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 44, 0);
        data->mem = IPPC_PMFACSB_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (45 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 45, 0);
        data->mem = IPPD_PMFBCS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (46 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 46, 0);
        data->mem = IPPE_LLRCS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (47 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 47, 0);
        data->mem = IPPE_PRTCAM_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (48 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 48, 0);
        data->mem = IPPF_VTACS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (49 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 49, 0);
        data->mem = IPPF_VTBCS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (50 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 50, 0);
        data->mem = IPPF_VTCCS_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (51 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 51, 0);
        data->mem = TCAM_TCAM_BANKm;
        data->mode = dnx_init_mem_default_mode_none;
    }
    if (52 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 52, 0);
        data->mem = IPPB_FLPA_FWD_DOMAIN_STATISTICS_OBJECT_COMMAND_MAPm;
        data->mode = dnx_init_mem_default_mode_index;
    }
    if (53 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_mem_default_t *) dnxc_data_mgmt_table_data_get(unit, table, 53, 0);
        data->mem = IPPB_FLPB_FWD_DOMAIN_STATISTICS_OBJECT_COMMAND_MAPm;
        data->mode = dnx_init_mem_default_mode_index;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2c_a0_dnx_data_dev_init_properties_unsupported_set(
    int unit)
{
    int index_index;
    dnx_data_dev_init_properties_unsupported_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_properties;
    int table_index = dnx_data_dev_init_properties_table_unsupported;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 199;
    table->info_get.key_size[0] = 199;

    
    table->values[0].default_val = "NULL";
    table->values[1].default_val = "NULL";
    table->values[2].default_val = "-1";
    table->values[3].default_val = "0";
    table->values[4].default_val = "NULL";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dev_init_properties_unsupported_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dev_init_properties_table_unsupported");

    
    default_data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->property = NULL;
    default_data->suffix = NULL;
    default_data->num_max = -1;
    default_data->per_port = 0;
    default_data->err_msg = NULL;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->property = "otm_port_packet_rate";
        data->per_port = 1;
        data->err_msg = "Packet_mode is supported by API bcm_cosq_control_set() (bcmCosqControlShaperPacketMode)";
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->property = "backplane_serdes_encoding";
        data->per_port = 1;
        data->err_msg = "Setting encoding supported by API bcm_port_resource_set()";
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->property = "repeater_link_enable";
        data->num_max = 192;
        data->err_msg = "repeater is not supported";
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->property = "system_is_vcs_128_in_system";
        data->err_msg = "cell format VSC128 is not supported. VSC256 will be set automatically";
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->property = "hqos_mapping_enable";
        data->err_msg = "Mapping of many system ports to single modport is always allowed. No need to set this soc property.";
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->property = "flow_mapping_queue_base";
        data->err_msg = "Feature not supported";
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->property = "system_ref_core_clock";
        data->err_msg = "system_ref_core_clock must be with '_khz' suffix";
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->property = "fap_device_mode";
        data->err_msg = "The device supports only PP mode. TM mode is not supported. TDM functionality is done by other APIs (see bcm_port_add)";
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->property = "custom_feature_l3_mc_use_tcam";
        data->err_msg = "L3 multicast flows are changed. There are no different options for L3 multicast forwarding.";
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->property = "custom_feature_l3_ipv6_uc_use_tcam";
        data->err_msg = "L3 multicast flows are changed. There are no different options for L3 multicast forwarding.";
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->property = "bcm886xx_ipv6_ext_hdr_enable";
        data->err_msg = "Several options of IPv6 Extension headers are supported by default. There is only one mode in the device.";
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->property = "custom_feature_udp_parse_disable";
        data->err_msg = "UDP parse is enabled by default. There is only one mode in the device.";
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->property = "sa_auth_enabled";
        data->err_msg = "SA Authentication feature is no longer done by the LLR block. Instead ACL capabilities are available. See L2 backward compatible section.";
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->property = "logical_port_l2_bridge";
        data->err_msg = "The functionality is no longer set by init-sequence using soc-property.";
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->property = "logical_port_drop";
        data->err_msg = "The functionality is no longer set by init-sequence using soc-property.";
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->property = "evb_enable";
        data->err_msg = "EVB functionality is currently not supported.";
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->property = "vlan_translation_match_ipv4";
        data->err_msg = "Feature not supported";
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->property = "bcm886xx_next_hop_mac_extension_enable";
        data->err_msg = "Global mode is no longer required.";
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->property = "bcm886xx_vlan_translate_mode";
        data->err_msg = "The device supports only advanced vlan translation. Basic vlan translation is not supported anymore. See L2 backward compatible section.";
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->property = "bcm886xx_mpls_termination_database_mode";
        data->err_msg = "MPLS termination flows are changed. There are no different options for MPLS termination.";
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->property = "mpls_termination_label_index_enable";
        data->err_msg = "MPLS termination flows are changed. There are no different options for MPLS termination.";
    }
    if (21 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->property = "fast_reroute_labels_enable";
        data->err_msg = "MPLS termination flows are changed. There are no different options for MPLS termination.";
    }
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->property = "mpls_context_specific_label_enable";
        data->err_msg = "The functionality is no longer supported by a global mode.";
    }
    if (23 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->property = "mpls_context";
        data->err_msg = "he functionality is no longer supported by a global mode.";
    }
    if (24 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->property = "mpls_tp_mymac_reserved_address";
        data->err_msg = "Feature not supported";
    }
    if (25 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, 0);
        data->property = "bcm886xx_qos_l3_l2_marking";
        data->err_msg = "QOS remarking is now supported differently.";
    }
    if (26 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->property = "custom_feature_ipmc_set_entry_type_by_rif";
        data->err_msg = "Feature not supported";
    }
    if (27 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, 0);
        data->property = "bcm886xx_l3_ingress_urpf_enable";
        data->err_msg = "Functionality is supported per LIF(RIF) only.";
    }
    if (28 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 28, 0);
        data->property = "bcm886xx_mpls_termination_key_mode";
        data->err_msg = "MPLS termination flows are changed. There are no different options for MPLS termination.";
    }
    if (29 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 29, 0);
        data->property = "bcm886xx_logical_interface_bridge_filter_enable";
        data->err_msg = "The functionality per LIF is now changed and enabled by default with no need for global mode indication.";
    }
    if (30 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 30, 0);
        data->property = "split_horizon_forwarding_groups_mode";
        data->err_msg = "The functionality per LIF is now changed and enabled by default with no need for global mode indication.";
    }
    if (31 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 31, 0);
        data->property = "oam_enable";
        data->err_msg = "Functionality is enabled by default";
    }
    if (32 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 32, 0);
        data->property = "bfd_enable";
        data->err_msg = "Functionality is enabled by default";
    }
    if (33 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 33, 0);
        data->property = "pwe_termination_port_mode_enable";
        data->err_msg = "Feature not supported";
    }
    if (34 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 34, 0);
        data->property = "mpls_entropy_label_indicator_enable";
        data->err_msg = "MPLS termination flows are changed. There are no different options for MPLS termination.";
    }
    if (35 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 35, 0);
        data->property = "bcm886xx_ipv6_tunnel_enable";
        data->err_msg = "IPv6 tunnel flows are changed. There are no different options for IPv6 tunnels.";
    }
    if (36 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 36, 0);
        data->property = "number_of_inrif_mac_termination_combinations";
        data->err_msg = "The functionality per In-LIF(In-RIF) profile has been changed. Global mode is no longer required.";
    }
    if (37 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 37, 0);
        data->property = "ipmc_l3mcastl2_mode";
        data->err_msg = "IPMC flows are changed. There are no different options for it.";
    }
    if (38 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 38, 0);
        data->property = "ext_ram_t_32aw";
        data->err_msg = "timing parameter no longer relevant";
    }
    if (39 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 39, 0);
        data->property = "ext_ram_t_al";
        data->err_msg = "timing parameter no longer relevant";
    }
    if (40 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 40, 0);
        data->property = "ext_ram_t_crc_alert";
        data->err_msg = "timing parameter no longer relevant";
    }
    if (41 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 41, 0);
        data->property = "ext_ram_t_crc_rd_latency";
        data->err_msg = "timing parameter no longer relevant";
    }
    if (42 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 42, 0);
        data->property = "ext_ram_t_crc_wr_latency";
        data->err_msg = "timing parameter no longer relevant";
    }
    if (43 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 43, 0);
        data->property = "ext_ram_t_ras_enable";
        data->err_msg = "timing parameter no longer relevant";
    }
    if (44 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 44, 0);
        data->property = "ext_ram_t_ref";
        data->err_msg = "timing parameter no longer relevant";
    }
    if (45 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 45, 0);
        data->property = "ext_ram_t_rfc";
        data->err_msg = "timing parameter no longer relevant";
    }
    if (46 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 46, 0);
        data->property = "ext_ram_t_rrd";
        data->err_msg = "timing parameter no longer relevant";
    }
    if (47 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 47, 0);
        data->property = "ext_ram_t_rst";
        data->err_msg = "timing parameter no longer relevant";
    }
    if (48 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 48, 0);
        data->property = "ext_ram_t_rtp";
        data->err_msg = "timing parameter no longer relevant";
    }
    if (49 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 49, 0);
        data->property = "ext_ram_t_zqcs";
        data->err_msg = "timing parameter no longer relevant";
    }
    if (50 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 50, 0);
        data->property = "ext_ram_abi";
        data->err_msg = "address bus inversion is not relevant for jr2, drams address lines are ordered internally";
    }
    if (51 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 51, 0);
        data->property = "ext_ram_addr_bank_swap";
        data->err_msg = "address banks swaps are done internally";
    }
    if (52 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 52, 0);
        data->property = "ext_ram_present";
        data->err_msg = "ext_ram_present was replaced by ext_ram_enabled_bitmap to better represent which drams are available in the system";
    }
    if (53 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 53, 0);
        data->property = "ext_ram_auto_tune";
        data->err_msg = "ext_ram_auto_tune was replaced by dram_phy_tune_mode_on_init to better convey the meaning and avoid confusion between soc property and shell command";
    }
    if (54 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 54, 0);
        data->property = "ext_ram_dq_swap";
        data->err_msg = "data bits swaps are done internally";
    }
    if (55 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 55, 0);
        data->property = "ext_ram_dbuff_mmc_optimized_distribution_enable";
        data->err_msg = "this is a legacy soc property that is relevant only for a very specific scenario which doesn't occur in jer 2";
    }
    if (56 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 56, 0);
        data->property = "ext_ram_total_size";
        data->err_msg = "total dram size is configured internally";
    }
    if (57 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 57, 0);
        data->property = "ext_ram_type";
        data->err_msg = "only one type of dram is supported and configured internally";
    }
    if (58 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 58, 0);
        data->property = "ext_ram_gear_down_mode";
        data->err_msg = "gear down mode not supported in jer 2";
    }
    if (59 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 59, 0);
        data->property = "slow_max_rate_level";
        data->err_msg = "Not supported. No tradeoffs between max slow rate and max burst";
    }
    if (60 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 60, 0);
        data->property = "l3_multiple_mymac_termination_mode";
        data->err_msg = "Multiple mymac termination mode is no longer supported";
    }
    if (61 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 61, 0);
        data->property = "bcm886xx_ingress_protection_coupled_mode";
        data->err_msg = "Ingress protection coupled mode no longer supported";
    }
    if (62 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 62, 0);
        data->property = "bcm_stat_flags";
        data->err_msg = "bcm_stat_flags is not in use in DNX. Please go to AE for further information. Please set this soc property to 0, or delete it from config file";
    }
    if (63 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 63, 0);
        data->property = "soc_counter_control_level";
        data->err_msg = "soc_counter_control_level is not in use in DNX. Please go to AE for further information. Please delete it from config file";
    }
    if (64 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 64, 0);
        data->property = "ilkn_counters_mode";
        data->err_msg = "ilkn_counters_mode is not in use in DNX. only counters per channel are supported";
    }
    if (65 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 65, 0);
        data->property = "voq_mapping_mode";
        data->err_msg = "Only INDIRECT mode is supported, so voq_mapping_mode is not relevant";
    }
    if (66 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 66, 0);
        data->property = "l3_multiple_mymac_termination_enable";
        data->err_msg = "l3_multiple_mymac_termination_enable is not supported in JR2";
    }
    if (67 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 67, 0);
        data->property = "dport_map_enable";
        data->err_msg = "this property is no longer supported";
    }
    if (68 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 68, 0);
        data->property = "ilkn_num_lanes";
        data->err_msg = "ilkn_num_lanes is not in use in DNX. use ilkn_lanes";
    }
    if (69 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 69, 0);
        data->property = "ilkn_invalid_lane_id";
        data->err_msg = "Ilkn_invalid_lane_id is not in use in DNX.";
    }
    if (70 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 70, 0);
        data->property = "ilkn_interface_status_ignore";
        data->err_msg = "ilkn_interface_status_ignore is not in use in DNX.";
    }
    if (71 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 71, 0);
        data->property = "ilkn_interface_status_oob_ignore";
        data->err_msg = "ilkn_interface_status_ignore is not in use in DNX.";
    }
    if (72 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 72, 0);
        data->property = "ilkn_retransmit_buffer_size";
        data->err_msg = "ilkn_retransmit_buffer_size is not in use in DNX. use ilkn_lanes";
    }
    if (73 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 73, 0);
        data->property = "ilkn_retransmit_calendar_mode_rx";
        data->err_msg = "ilkn_retransmit_calendar_mode_rx is not in use in DNX. use ilkn_lanes";
    }
    if (74 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 74, 0);
        data->property = "ilkn_retransmit_calendar_mode_tx";
        data->err_msg = "ilkn_retransmit_calendar_mode_tx is not in use in DNX. use ilkn_lanes";
    }
    if (75 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 75, 0);
        data->property = "ilkn_retransmit_enable_rx";
        data->err_msg = "ilkn_retransmit_enable_rx is not in use in DNX. use ilkn_lanes";
    }
    if (76 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 76, 0);
        data->property = "ilkn_retransmit_enable_tx";
        data->err_msg = "ilkn_retransmit_enable_tx is not in use in DNX. use ilkn_lanes";
    }
    if (77 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 77, 0);
        data->property = "ilkn_retransmit_num_requests_resent";
        data->err_msg = "ilkn_retransmit_num_requests_resent is not in use in DNX. use ilkn_lanes";
    }
    if (78 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 78, 0);
        data->property = "ilkn_retransmit_num_sn_repetitions_rx";
        data->err_msg = "ilkn_retransmit_num_sn_repetitions_rx is not in use in DNX. use ilkn_lanes";
    }
    if (79 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 79, 0);
        data->property = "ilkn_retransmit_num_sn_repetitions_tx";
        data->err_msg = "ilkn_retransmit_num_sn_repetitions_tx is not in use in DNX. use ilkn_lanes";
    }
    if (80 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 80, 0);
        data->property = "ilkn_retransmit_peer_tx_buffer_size";
        data->err_msg = "ilkn_retransmit_peer_tx_buffer_size is not in use in DNX. use ilkn_lanes";
    }
    if (81 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 81, 0);
        data->property = "ilkn_retransmit_reserved_channel_id";
        data->err_msg = "ilkn_retransmit_reserved_channel_id is not in use in DNX. use ilkn_lanes";
    }
    if (82 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 82, 0);
        data->property = "ilkn_retransmit_rx_discontinuity_event_timeout";
        data->err_msg = "ilkn_retransmit_rx_discontinuity_event_timeout is not in use in DNX. use ilkn_lanes";
    }
    if (83 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 83, 0);
        data->property = "ilkn_retransmit_rx_ignore";
        data->err_msg = "ilkn_retransmit_rx_ignore is not in use in DNX. use ilkn_lanes";
    }
    if (84 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 84, 0);
        data->property = "ilkn_retransmit_rx_reset_upon_watchdog_error_enable";
        data->err_msg = "ilkn_retransmit_rx_reset_upon_watchdog_error_enable is not in use in DNX. use ilkn_lanes";
    }
    if (85 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 85, 0);
        data->property = "ilkn_retransmit_rx_reset_when_alligned_error_enable";
        data->err_msg = "ilkn_retransmit_rx_reset_when_alligned_error_enable is not in use in DNX. use ilkn_lanes";
    }
    if (86 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 86, 0);
        data->property = "ilkn_retransmit_rx_reset_when_error_enable";
        data->err_msg = "ilkn_retransmit_rx_reset_when_error_enable is not in use in DNX. use ilkn_lanes";
    }
    if (87 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 87, 0);
        data->property = "ilkn_retransmit_rx_reset_when_retry_error_enable";
        data->err_msg = "ilkn_retransmit_rx_reset_when_retry_error_enable is not in use in DNX. use ilkn_lanes";
    }
    if (88 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 88, 0);
        data->property = "ilkn_retransmit_rx_reset_when_timout_error_enable";
        data->err_msg = "ilkn_retransmit_rx_reset_when_timout_error_enable is not in use in DNX. use ilkn_lanes";
    }
    if (89 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 89, 0);
        data->property = "ilkn_retransmit_rx_reset_when_wrap_after_disc_error_enable";
        data->err_msg = "ilkn_retransmit_rx_reset_when_wrap_after_disc_error_enable is not in use in DNX. use ilkn_lanes";
    }
    if (90 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 90, 0);
        data->property = "ilkn_retransmit_rx_reset_when_wrap_before_disc_error_enable";
        data->err_msg = "ilkn_retransmit_rx_reset_when_wrap_before_disc_error_enable is not in use in DNX. use ilkn_lanes";
    }
    if (91 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 91, 0);
        data->property = "ilkn_retransmit_rx_timeout_sn";
        data->err_msg = "ilkn_retransmit_rx_timeout_sn is not in use in DNX. use ilkn_lanes";
    }
    if (92 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 92, 0);
        data->property = "ilkn_retransmit_rx_timeout_words";
        data->err_msg = "ilkn_retransmit_rx_timeout_words is not in use in DNX. use ilkn_lanes";
    }
    if (93 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 93, 0);
        data->property = "ilkn_retransmit_rx_watchdog";
        data->err_msg = "ilkn_retransmit_rx_watchdog is not in use in DNX. use ilkn_lanes";
    }
    if (94 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 94, 0);
        data->property = "ilkn_retransmit_sn_bits";
        data->err_msg = "ilkn_retransmit_sn_bits is not in use in DNX. use ilkn_lanes";
    }
    if (95 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 95, 0);
        data->property = "ilkn_retransmit_tx_ignore_requests_when_fifo_almost_empty";
        data->err_msg = "ilkn_retransmit_tx_ignore_requests_when_fifo_almost_empty is not in use in DNX. use ilkn_lanes";
    }
    if (96 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 96, 0);
        data->property = "ilkn_retransmit_tx_wait_for_seq_num_change_enable";
        data->err_msg = "ilkn_retransmit_tx_wait_for_seq_num_change_enable is not in use in DNX. use ilkn_lanes";
    }
    if (97 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 97, 0);
        data->property = "stat_if_enable_port";
        data->err_msg = "this property is no longer supported";
    }
    if (98 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 98, 0);
        data->property = "stat_if_tc_source";
        data->err_msg = "this property is no longer supported";
    }
    if (99 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 99, 0);
        data->property = "stat_if_reports_per_packet";
        data->err_msg = "this property is renamed to stat_if_pkt_size";
    }
    if (100 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 100, 0);
        data->property = "stat_if_billing_ingress_queue_stamp_enable";
        data->err_msg = "this property is no longer supported, made via report format API";
    }
    if (101 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 101, 0);
        data->property = "stat_if_billing_ingress_drop_reason_enable";
        data->err_msg = "this property is no longer supported, made via report format API";
    }
    if (102 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 102, 0);
        data->property = "stat_if_billing_filter_reports";
        data->err_msg = "this property is no longer supported, made via source mapping API";
    }
    if (103 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 103, 0);
        data->property = "stat_if_core_mode";
        data->err_msg = "this property is no longer supported";
    }
    if (104 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 104, 0);
        data->property = "stat_if_reports_per packet";
        data->err_msg = "this property is no longer supported";
    }
    if (105 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 105, 0);
        data->property = "stat_if_scrubber_uc_dram_buffer_th";
        data->num_max = 14;
        data->err_msg = "this property is no longer supported";
    }
    if (106 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 106, 0);
        data->property = "stat_if_scrubber_buffer_descr_th";
        data->num_max = 14;
        data->err_msg = "this property is no longer supported";
    }
    if (107 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 107, 0);
        data->property = "stat_if_etpp_counter_mode";
        data->num_max = 1;
        data->err_msg = "this property is no longer supported";
    }
    if (108 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 108, 0);
        data->property = "stat_if_etpp_mode";
        data->num_max = 1;
        data->err_msg = "this property is no longer supported";
    }
    if (109 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 109, 0);
        data->property = "stat_if_rate";
        data->err_msg = "this property is no longer supported";
    }
    if (110 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 110, 0);
        data->property = "counter_engine_source";
        data->num_max = 17;
        data->err_msg = "this property is no longer supported - no soc properties to configure CRPS functionality";
    }
    if (111 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 111, 0);
        data->property = "counter_engine_statistics";
        data->num_max = 17;
        data->err_msg = "this property is no longer supported- no soc properties to configure CRPS functionality";
    }
    if (112 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 112, 0);
        data->property = "counter_engine_format";
        data->num_max = 17;
        data->err_msg = "this property is no longer supported- no soc properties to configure CRPS functionality";
    }
    if (113 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 113, 0);
        data->property = "counter_engine_map_enable";
        data->num_max = 17;
        data->err_msg = "this property is no longer supported- no soc properties to configure CRPS functionality";
    }
    if (114 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 114, 0);
        data->property = "counter_engine_voq_min_queue";
        data->num_max = 17;
        data->err_msg = "this property is no longer supported- no soc properties to configure CRPS functionality";
    }
    if (115 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 115, 0);
        data->property = "counter_engine_voq_queue_set_size";
        data->num_max = 17;
        data->err_msg = "this property is no longer supported- no soc properties to configure CRPS functionality";
    }
    if (116 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 116, 0);
        data->property = "counter_engine_map_drop_green_offset";
        data->num_max = 17;
        data->err_msg = "this property is no longer supported- no soc properties to configure CRPS functionality";
    }
    if (117 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 117, 0);
        data->property = "counter_engine_map_drop_yellow_offset";
        data->num_max = 17;
        data->err_msg = "this property is no longer supported- no soc properties to configure CRPS functionality";
    }
    if (118 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 118, 0);
        data->property = "counter_engine_map_drop_red_offset";
        data->num_max = 17;
        data->err_msg = "this property is no longer supported- no soc properties to configure CRPS functionality";
    }
    if (119 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 119, 0);
        data->property = "counter_engine_map_drop_black_offset";
        data->num_max = 17;
        data->err_msg = "this property is no longer supported- no soc properties to configure CRPS functionality";
    }
    if (120 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 120, 0);
        data->property = "counter_engine_map_fwd_green_offset";
        data->num_max = 17;
        data->err_msg = "this property is no longer supported- no soc properties to configure CRPS functionality";
    }
    if (121 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 121, 0);
        data->property = "counter_engine_map_fwd_yellow_offset";
        data->num_max = 17;
        data->err_msg = "this property is no longer supported- no soc properties to configure CRPS functionality";
    }
    if (122 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 122, 0);
        data->property = "counter_engine_map_fwd_red_offset";
        data->num_max = 17;
        data->err_msg = "this property is no longer supported- no soc properties to configure CRPS functionality";
    }
    if (123 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 123, 0);
        data->property = "counter_engine_map_fwd_black_offset";
        data->num_max = 17;
        data->err_msg = "this property is no longer supported- no soc properties to configure CRPS functionality";
    }
    if (124 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 124, 0);
        data->property = "counter_engine_replicated_packets";
        data->err_msg = "this property is no longer supported- no soc properties to configure CRPS functionality";
    }
    if (125 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 125, 0);
        data->property = "policer_base_rate";
        data->err_msg = "this property is no longer supported";
    }
    if (126 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 126, 0);
        data->property = "policer_ingress_count";
        data->err_msg = "this property is no longer supported";
    }
    if (127 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 127, 0);
        data->property = "policer_egress_count";
        data->err_msg = "this property is no longer supported";
    }
    if (128 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 128, 0);
        data->property = "policer_ingress_sharing_mode";
        data->err_msg = "this property is no longer supported";
    }
    if (129 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 129, 0);
        data->property = "policer_egress_sharing_mode";
        data->err_msg = "this property is no longer supported";
    }
    if (130 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 130, 0);
        data->property = "policer_result_map";
        data->err_msg = "this property is no longer supported";
    }
    if (131 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 131, 0);
        data->property = "policer_result_parallel_color_map";
        data->err_msg = "this property is no longer supported";
    }
    if (132 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 132, 0);
        data->property = "policer_result_parallel_bucket_update";
        data->err_msg = "this property is no longer supported";
    }
    if (133 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 133, 0);
        data->property = "policer_color_resolution_mode";
        data->err_msg = "this property is no longer supported";
    }
    if (134 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 134, 0);
        data->property = "bcm_meter_clear_fe";
        data->err_msg = "this property is no longer supported";
    }
    if (135 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 135, 0);
        data->property = "bcm_meter_clear_ge";
        data->err_msg = "this property is no longer supported";
    }
    if (136 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 136, 0);
        data->property = "bcm_meter_clear_xe";
        data->err_msg = "this property is no longer supported";
    }
    if (137 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 137, 0);
        data->property = "global_meter_control";
        data->err_msg = "this property is no longer supported";
    }
    if (138 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 138, 0);
        data->property = "global_meter_compression_share";
        data->err_msg = "this property is no longer supported";
    }
    if (139 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 139, 0);
        data->property = "global_meter_compaction_support";
        data->err_msg = "this property is no longer supported";
    }
    if (140 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 140, 0);
        data->property = "private_ip_frwrd_table_size";
        data->err_msg = "mdb_kaps_a_size and mdb_kaps_b_size replace the private_ip_frwrd_table_size and public_ip_frwrd_table_size soc properties.";
    }
    if (141 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 141, 0);
        data->property = "public_ip_frwrd_table_size";
        data->err_msg = "mdb_kaps_a_size and mdb_kaps_b_size replace the private_ip_frwrd_table_size and public_ip_frwrd_table_size soc properties.";
    }
    if (142 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 142, 0);
        data->property = "enhanced_fib_scale_prefix_length";
        data->err_msg = "IPv4 FIB scale feature is not supported.";
    }
    if (143 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 143, 0);
        data->property = "enhanced_fib_scale_prefix_length_ipv6_long";
        data->err_msg = "IPv6 FIB scale feature is not supported, please read the backward compatibility document.";
    }
    if (144 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 144, 0);
        data->property = "enhanced_fib_scale_prefix_length_ipv6_short";
        data->err_msg = "IPv6 FIB scale feature is not supported, please read the backward compatibility document.";
    }
    if (145 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 145, 0);
        data->property = "phy_an_c37";
        data->per_port = 1;
        data->err_msg = "CL37 AN mode is not supported";
    }
    if (146 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 146, 0);
        data->property = "mpls_encap_invalid_value";
        data->err_msg = "The soc property is no longer supported, please read the backward compatibility document.";
    }
    if (147 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 147, 0);
        data->property = "mpls_encapsulation_action_swap_or_push_enable";
        data->err_msg = "The soc property is no longer supported, please read the backward compatibility document.";
    }
    if (148 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 148, 0);
        data->property = "port_raw_mpls_enable";
        data->err_msg = "The soc property is no longer supported, please read the backward compatibility document.";
    }
    if (149 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 149, 0);
        data->property = "udp_tunnels_enable";
        data->err_msg = "UDP tunnels are always enabled, please read the backward compatibility document.";
    }
    if (150 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 150, 0);
        data->property = "bcm886xx_ip4_tunnel_termination_mode";
        data->err_msg = "Tunnel termination modes are no longer configured with soc property,please read the backward compatibility document.";
    }
    if (151 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 151, 0);
        data->property = "default_logical_interface_mpls_termination_explicit_null";
        data->err_msg = "The soc property is no longer supported, please read the backward compatibility document.";
    }
    if (152 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 152, 0);
        data->property = "mpls_termination_explicit_null_label_lookup_mode";
        data->err_msg = "The soc property is no longer supported, please read the backward compatibility document.";
    }
    if (153 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 153, 0);
        data->property = "mpls_egress_label_extended_encapsulation_mode";
        data->err_msg = "The soc property is no longer supported, please read the backward compatibility document.";
    }
    if (154 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 154, 0);
        data->property = "bcm886xx_ipv6_tunnel_enable";
        data->err_msg = "IPV6 tunnels are always enabled, please read the backward compatibility document.";
    }
    if (155 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 155, 0);
        data->property = "egress_membership_mode";
        data->err_msg = "Egress membership mode is not supported anymore, please read the backward compatibility document.";
    }
    if (156 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 156, 0);
        data->property = "bcm886xx_ip6_tunnel_termination_mode";
        data->err_msg = "Ipv6 tunnel termination mode is not configured with soc property, please read the backward compatibility document.";
    }
    if (157 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 157, 0);
        data->property = "bcm886xx_ip6_tunnel_encapsulation_mode";
        data->err_msg = "Ipv6 encapsulation mode is not supported anymore, please read the backward compatibility document.";
    }
    if (158 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 158, 0);
        data->property = "custom_feature_allow_modifications_during_traffic";
        data->err_msg = "No need to allow modification during traffic anymore, each case handles its own restrictions internally";
    }
    if (159 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 159, 0);
        data->property = "ext_1588_mac_enable";
        data->per_port = 1;
        data->err_msg = "external mac per port is determinie using API bcm_port_timesync_config_set (flag BCM_PORT_TIMESYNC_EXTERNAL_MAC_ENABLE) ";
    }
    if (160 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 160, 0);
        data->property = "bcm88660_1588_48b_stamping_enable";
        data->err_msg = "1588 48b stamping is not supported";
    }
    if (161 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 161, 0);
        data->property = "ext_ip4_fwd_table_size";
        data->err_msg = "The soc property is no longer supported, please read the backward compatibility document.";
    }
    if (162 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 162, 0);
        data->property = "ext_ip4_uc_fpr_fwd_table_size";
        data->err_msg = "The soc property is no longer supported, please read the backward compatibility document.";
    }
    if (163 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 163, 0);
        data->property = "ext_ip4_mc_fwd_table_size";
        data->err_msg = "The soc property is no longer supported, please read the backward compatibility document.";
    }
    if (164 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 164, 0);
        data->property = "ext_ip6_fwd_table_size";
        data->err_msg = "The soc property is no longer supported, please read the backward compatibility document.";
    }
    if (165 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 165, 0);
        data->property = "ext_ip6_uc_fpr_fwd_table_size";
        data->err_msg = "The soc property is no longer supported, please read the backward compatibility document.";
    }
    if (166 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 166, 0);
        data->property = "ext_ip6_mc_fwd_table_size";
        data->err_msg = "The soc property is no longer supported, please read the backward compatibility document.";
    }
    if (167 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 167, 0);
        data->property = "ext_mpls_fwd_table_size";
        data->err_msg = "The soc property is no longer supported, please read the backward compatibility document.";
    }
    if (168 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 168, 0);
        data->property = "ext_trill_uc_fwd_table_size";
        data->err_msg = "The soc property is no longer supported, please read the backward compatibility document.";
    }
    if (169 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 169, 0);
        data->property = "ext_trill_mc_fwd_table_size";
        data->err_msg = "The soc property is no longer supported, please read the backward compatibility document.";
    }
    if (170 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 170, 0);
        data->property = "ext_ip4_mc_bridge_fwd_table_size";
        data->err_msg = "The soc property is no longer supported, please read the backward compatibility document.";
    }
    if (171 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 171, 0);
        data->property = "ext_ip4_double_capacity_fwd_table_size";
        data->err_msg = "The soc property is no longer supported, please read the backward compatibility document.";
    }
    if (172 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 172, 0);
        data->property = "ext_ip4_uc_rpf_public_fwd_table_size";
        data->err_msg = "The soc property is no longer supported, please read the backward compatibility document.";
    }
    if (173 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 173, 0);
        data->property = "ext_ip6_uc_rpf_public_fwd_table_size";
        data->err_msg = "The soc property is no longer supported, please read the backward compatibility document.";
    }
    if (174 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 174, 0);
        data->property = "custom_feature_ext_tcam_dc_24bit_result";
        data->err_msg = "The soc property is no longer supported, please read the backward compatibility document.";
    }
    if (175 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 175, 0);
        data->property = "ext_tcam_result_size_segment";
        data->num_max = 7;
        data->err_msg = "The soc property is no longer supported, please read the backward compatibility document.";
    }
    if (176 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 176, 0);
        data->property = "ext_tcam_result_size_segment_res1_pad";
        data->err_msg = "The soc property is no longer supported, please read the backward compatibility document.";
    }
    if (177 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 177, 0);
        data->property = "ext_tcam_result_size_segment_res3_pad";
        data->err_msg = "The soc property is no longer supported, please read the backward compatibility document.";
    }
    if (178 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 178, 0);
        data->property = "bcm_linkscan_interval";
        data->err_msg = "The soc property is no longer supported. Please use the API bcm_linkscan_enable_set() instead.";
    }
    if (179 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 179, 0);
        data->property = "num_erp_tm_ports";
        data->err_msg = "The soc property is no longer supported. Please use the soc property ucode_port with interface ERP instead.";
    }
    if (180 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 180, 0);
        data->property = "phy_an_c73";
        data->per_port = 1;
        data->err_msg = "The soc property is no longer supported. Please use bcm_port_autoneg_ability_advert_set instead.";
    }
    if (181 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 181, 0);
        data->property = "port_init_autoneg";
        data->per_port = 1;
        data->err_msg = "The soc property is no longer supported. Please use bcm_port_autoneg_set instead.";
    }
    if (182 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 182, 0);
        data->property = "exact_match_tables_shadow_enable";
        data->err_msg = "Shadow memories are not supported for EM tables, please read the backward compatibility document.";
    }
    if (183 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 183, 0);
        data->property = "cosq_admission_preference_3";
        data->err_msg = "the soc property is not supported for DP3. it is always ADMIT_OVER_GUARANTEE";
    }
    if (184 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 184, 0);
        data->property = "fc_inband_intlkn_type";
        data->per_port = 1;
        data->err_msg = "ILKN Inband Flow Control not supported.";
    }
    if (185 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 185, 0);
        data->property = "fc_inband_intlkn_calender_rep_count";
        data->per_port = 1;
        data->err_msg = "ILKN Inband Flow Control not supported.";
    }
    if (186 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 186, 0);
        data->property = "fc_inband_intlkn_calender_rep_count";
        data->suffix = "rx";
        data->num_max = dnx_data_fc.inband.nof_inband_intlkn_cal_instances_get(unit);
        data->err_msg = "ILKN Inband Flow Control not supported.";
    }
    if (187 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 187, 0);
        data->property = "fc_inband_intlkn_calender_rep_count";
        data->suffix = "tx";
        data->num_max = dnx_data_fc.inband.nof_inband_intlkn_cal_instances_get(unit);
        data->err_msg = "ILKN Inband Flow Control not supported.";
    }
    if (188 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 188, 0);
        data->property = "fc_inband_intlkn_llfc_mub_enable_mask";
        data->per_port = 1;
        data->err_msg = "ILKN Inband Flow Control not supported.";
    }
    if (189 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 189, 0);
        data->property = "fc_inband_intlkn_channel_mub_enable_mask";
        data->per_port = 1;
        data->err_msg = "ILKN Inband Flow Control not supported.";
    }
    if (190 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 190, 0);
        data->property = "fc_inband_intlkn_channel_mub_enable_mask";
        data->suffix = "rx";
        data->num_max = dnx_data_fc.inband.nof_inband_intlkn_cal_instances_get(unit);
        data->err_msg = "ILKN Inband Flow Control not supported.";
    }
    if (191 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 191, 0);
        data->property = "fc_inband_intlkn_channel_mub_enable_mask";
        data->suffix = "tx";
        data->num_max = dnx_data_fc.inband.nof_inband_intlkn_cal_instances_get(unit);
        data->err_msg = "ILKN Inband Flow Control not supported.";
    }
    if (192 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 192, 0);
        data->property = "serdes_fabric_clk_freq";
        data->suffix = "in";
        data->err_msg = "The soc property is no longer supported, please read the backward compatibility document.";
    }
    if (193 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 193, 0);
        data->property = "serdes_fabric_clk_freq";
        data->suffix = "out";
        data->err_msg = "The soc property is no longer supported, please read the backward compatibility document.";
    }
    if (194 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 194, 0);
        data->property = "serdes_nif_clk_freq";
        data->suffix = "in";
        data->err_msg = "The soc property is no longer supported, please read the backward compatibility document.";
    }
    if (195 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 195, 0);
        data->property = "serdes_nif_clk_freq";
        data->suffix = "out";
        data->err_msg = "The soc property is no longer supported, please read the backward compatibility document.";
    }
    if (196 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 196, 0);
        data->property = "ilkn_tdm_dedicated_queuing";
        data->err_msg = "propery is not supported. define interleaving by API";
    }
    if (197 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 197, 0);
        data->property = "custom_feature_ilkn_dedicated_tdm_context";
        data->per_port = 1;
        data->err_msg = "ILKN dedicated TDM context is not supported.";
    }
    if (198 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_properties_unsupported_t *) dnxc_data_mgmt_table_data_get(unit, table, 198, 0);
        data->property = "graceful_lag_modification_enable";
        data->err_msg = "The soc property is no longer supported, please check backward compatibility document.";
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2c_a0_dnx_data_dev_init_general_data_path_hw_set(
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
j2c_a0_dnx_data_dev_init_general_remove_crc_bytes_set(
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
j2c_a0_dnx_data_dev_init_general_remove_crc_bytes_capability_set(
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
j2c_a0_dnx_data_dev_init_general_l4_protocol_fields_config_enable_set(
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
j2c_a0_dnx_data_dev_init_general_access_only_set(
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
        "Set to boot with access only.\n"
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
j2c_a0_dnx_data_dev_init_general_heat_up_set(
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
        "Set to boot for heat-up.\n"
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
j2c_a0_dnx_data_dev_init_ha_warmboot_support_set(
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
        "Specify if WarmBoot is supported\n"
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
j2c_a0_dnx_data_dev_init_ha_sw_state_max_size_set(
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
        "Configures the size of memory to be pre allocated for sw state.\n"
        "The valid range is 0-1000000000 (i.e up to 1TB).\n"
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
j2c_a0_dnx_data_dev_init_ha_stable_location_set(
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
        "Specify the stable cache option for Warm Boot operations.\n"
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
j2c_a0_dnx_data_dev_init_ha_stable_size_set(
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
        "Specify the stable cache size in bytes used for Warm boot operations.\n"
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
j2c_a0_dnx_data_dev_init_ha_stable_filename_set(
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
        "file system will be used to save the stable cache data with this filename\n"
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
j2c_a0_dnx_data_dev_init_shadow_uncacheable_mem_set(
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

    
    table->keys[0].size = 100;
    table->info_get.key_size[0] = 100;

    
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
        data->mem = FSRD_FSRD_PROM_MEMm;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->mem = CLPORT_WC_UCMEM_DATAm;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->mem = ERPP_MEM_F000000m;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->mem = IPPF_MEM_F000000m;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->mem = ERPP_ERPP_DEBUG_UNITm;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->mem = ETPPA_ETPPA_DEBUG_UNITm;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->mem = ETPPB_ETPPB_DEBUG_UNITm;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->mem = ETPPC_ETPPC_DEBUG_UNITm;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->mem = IPPA_IPPA_DEBUG_UNITm;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->mem = IPPB_IPPB_DEBUG_UNITm;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->mem = IPPC_IPPC_DEBUG_UNITm;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->mem = IPPD_IPPD_DEBUG_UNITm;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->mem = IPPE_IPPE_DEBUG_UNITm;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->mem = IPPF_IPPF_DEBUG_UNITm;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->mem = IPPA_IPPA_DEBUG_UNITm;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->mem = IPPB_IPPB_DEBUG_UNITm;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->mem = RX_LKUP_1588_MEM_400Gm;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->mem = RX_LKUP_1588_MEM_MPP0m;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->mem = RX_LKUP_1588_MEM_MPP1m;
    }
    if (25 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, 0);
        data->mem = SPEED_ID_TABLEm;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->mem = SPEED_PRIORITY_MAP_TBLm;
    }
    if (27 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, 0);
        data->mem = TX_LKUP_1588_MEM_400Gm;
    }
    if (28 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 28, 0);
        data->mem = TX_LKUP_1588_MEM_MPP0m;
    }
    if (29 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 29, 0);
        data->mem = TX_LKUP_1588_MEM_MPP1m;
    }
    if (30 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 30, 0);
        data->mem = UM_TABLEm;
    }
    if (31 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 31, 0);
        data->mem = AM_TABLEm;
    }
    if (32 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 32, 0);
        data->mem = MDB_ARM_KAPS_TCMm;
    }
    if (33 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 33, 0);
        data->mem = ERPP_EPMFCS_TCAM_BANK_COMMANDm;
    }
    if (34 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 34, 0);
        data->mem = IPPA_VTDCS_TCAM_BANK_COMMANDm;
    }
    if (35 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 35, 0);
        data->mem = IPPA_VTECS_TCAM_BANK_COMMANDm;
    }
    if (36 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 36, 0);
        data->mem = IPPB_FLPACS_TCAM_BANK_COMMANDm;
    }
    if (37 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 37, 0);
        data->mem = IPPB_FLPBCS_TCAM_BANK_COMMANDm;
    }
    if (38 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 38, 0);
        data->mem = IPPC_PMFACSA_TCAM_BANK_COMMANDm;
    }
    if (39 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 39, 0);
        data->mem = IPPC_PMFACSB_TCAM_BANK_COMMANDm;
    }
    if (40 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 40, 0);
        data->mem = IPPD_PMFBCS_TCAM_BANK_COMMANDm;
    }
    if (41 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 41, 0);
        data->mem = IPPE_LLRCS_TCAM_BANK_COMMANDm;
    }
    if (42 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 42, 0);
        data->mem = IPPE_PRTCAM_TCAM_BANK_COMMANDm;
    }
    if (43 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 43, 0);
        data->mem = IPPF_VTACS_TCAM_BANK_COMMANDm;
    }
    if (44 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 44, 0);
        data->mem = IPPF_VTBCS_TCAM_BANK_COMMANDm;
    }
    if (45 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 45, 0);
        data->mem = IPPF_VTCCS_TCAM_BANK_COMMANDm;
    }
    if (46 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 46, 0);
        data->mem = TCAM_TCAM_BANK_COMMANDm;
    }
    if (47 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 47, 0);
        data->mem = ERPP_EPMFCS_TCAM_BANK_REPLYm;
    }
    if (48 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 48, 0);
        data->mem = IPPA_VTDCS_TCAM_BANK_REPLYm;
    }
    if (49 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 49, 0);
        data->mem = IPPA_VTECS_TCAM_BANK_REPLYm;
    }
    if (50 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 50, 0);
        data->mem = IPPB_FLPACS_TCAM_BANK_REPLYm;
    }
    if (51 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 51, 0);
        data->mem = IPPB_FLPBCS_TCAM_BANK_REPLYm;
    }
    if (52 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 52, 0);
        data->mem = IPPC_PMFACSA_TCAM_BANK_REPLYm;
    }
    if (53 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 53, 0);
        data->mem = IPPC_PMFACSB_TCAM_BANK_REPLYm;
    }
    if (54 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 54, 0);
        data->mem = IPPD_PMFBCS_TCAM_BANK_REPLYm;
    }
    if (55 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 55, 0);
        data->mem = IPPE_LLRCS_TCAM_BANK_REPLYm;
    }
    if (56 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 56, 0);
        data->mem = IPPE_PRTCAM_TCAM_BANK_REPLYm;
    }
    if (57 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 57, 0);
        data->mem = IPPF_VTACS_TCAM_BANK_REPLYm;
    }
    if (58 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 58, 0);
        data->mem = IPPF_VTBCS_TCAM_BANK_REPLYm;
    }
    if (59 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 59, 0);
        data->mem = IPPF_VTCCS_TCAM_BANK_REPLYm;
    }
    if (60 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 60, 0);
        data->mem = TCAM_TCAM_BANK_REPLYm;
    }
    if (61 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 61, 0);
        data->mem = ERPP_EPMFCS_TCAM_BANKm;
    }
    if (62 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 62, 0);
        data->mem = IPPA_VTDCS_TCAM_BANKm;
    }
    if (63 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 63, 0);
        data->mem = IPPA_VTECS_TCAM_BANKm;
    }
    if (64 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 64, 0);
        data->mem = IPPB_FLPACS_TCAM_BANKm;
    }
    if (65 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 65, 0);
        data->mem = IPPB_FLPBCS_TCAM_BANKm;
    }
    if (66 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 66, 0);
        data->mem = IPPC_PMFACSA_TCAM_BANKm;
    }
    if (67 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 67, 0);
        data->mem = IPPC_PMFACSB_TCAM_BANKm;
    }
    if (68 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 68, 0);
        data->mem = IPPD_PMFBCS_TCAM_BANKm;
    }
    if (69 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 69, 0);
        data->mem = IPPE_LLRCS_TCAM_BANKm;
    }
    if (70 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 70, 0);
        data->mem = IPPE_PRTCAM_TCAM_BANKm;
    }
    if (71 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 71, 0);
        data->mem = IPPF_VTACS_TCAM_BANKm;
    }
    if (72 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 72, 0);
        data->mem = IPPF_VTBCS_TCAM_BANKm;
    }
    if (73 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 73, 0);
        data->mem = IPPF_VTCCS_TCAM_BANKm;
    }
    if (74 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 74, 0);
        data->mem = TCAM_TCAM_BANKm;
    }
    if (75 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 75, 0);
        data->mem = SCH_TOKEN_MEMORY_CONTROLLER_TMCm;
    }
    if (76 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 76, 0);
        data->mem = MDB_ARM_MEM_0m;
    }
    if (77 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 77, 0);
        data->mem = MDB_ISEM_1m;
    }
    if (78 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 78, 0);
        data->mem = MDB_ISEM_2m;
    }
    if (79 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 79, 0);
        data->mem = MDB_ISEM_3m;
    }
    if (80 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 80, 0);
        data->mem = MDB_IOEM_0m;
    }
    if (81 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 81, 0);
        data->mem = MDB_IOEM_1m;
    }
    if (82 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 82, 0);
        data->mem = MDB_MC_IDm;
    }
    if (83 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 83, 0);
        data->mem = MDB_GLEM_0m;
    }
    if (84 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 84, 0);
        data->mem = MDB_GLEM_1m;
    }
    if (85 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 85, 0);
        data->mem = MDB_EOEM_0m;
    }
    if (86 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 86, 0);
        data->mem = MDB_EOEM_1m;
    }
    if (87 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 87, 0);
        data->mem = EVNT_ASSIGNED_CMIC_ENGINE_TABLEm;
    }
    if (88 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 88, 0);
        data->mem = EVNT_SPACE_REGIONSm;
    }
    if (89 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 89, 0);
        data->mem = EVNT_CMIC_ENG_PROPERTIESm;
    }
    if (90 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 90, 0);
        data->mem = OAMP_MEP_DBm;
    }
    if (91 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 91, 0);
        data->mem = OAMP_RMEP_DBm;
    }
    if (92 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 92, 0);
        data->mem = MDB_ESEMm;
    }
    if (93 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 93, 0);
        data->mem = MDB_EXEM_1m;
    }
    if (94 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 94, 0);
        data->mem = MDB_EXEM_2m;
    }
    if (95 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 95, 0);
        data->mem = MDB_RMEPm;
    }
    if (96 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 96, 0);
        data->mem = IPPC_PER_PORT_STATISTICSm;
    }
    if (97 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 97, 0);
        data->mem = SCH_RESERVED_54m;
    }
    if (98 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 98, 0);
        data->mem = SCH_RESERVED_37m;
    }
    if (99 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 99, 0);
        data->mem = ETPPC_LATENCY_TABLEm;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
j2c_a0_data_dev_init_attach(
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

    
    data_index = dnx_data_dev_init_time_define_init_total_thresh;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_dev_init_time_init_total_thresh_set;
    data_index = dnx_data_dev_init_time_define_appl_init_total_thresh;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_dev_init_time_appl_init_total_thresh_set;

    

    
    data_index = dnx_data_dev_init_time_table_step_thresh;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_dev_init_time_step_thresh_set;
    data_index = dnx_data_dev_init_time_table_appl_step_thresh;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_dev_init_time_appl_step_thresh_set;
    
    submodule_index = dnx_data_dev_init_submodule_mem;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_dev_init_mem_table_default;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_dev_init_mem_default_set;
    
    submodule_index = dnx_data_dev_init_submodule_properties;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_dev_init_properties_table_unsupported;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_dev_init_properties_unsupported_set;
    
    submodule_index = dnx_data_dev_init_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dev_init_general_define_access_only;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_dev_init_general_access_only_set;
    data_index = dnx_data_dev_init_general_define_heat_up;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_dev_init_general_heat_up_set;

    
    data_index = dnx_data_dev_init_general_data_path_hw;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_dev_init_general_data_path_hw_set;
    data_index = dnx_data_dev_init_general_remove_crc_bytes;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_dev_init_general_remove_crc_bytes_set;
    data_index = dnx_data_dev_init_general_remove_crc_bytes_capability;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_dev_init_general_remove_crc_bytes_capability_set;
    data_index = dnx_data_dev_init_general_l4_protocol_fields_config_enable;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_dev_init_general_l4_protocol_fields_config_enable_set;

    
    
    submodule_index = dnx_data_dev_init_submodule_ha;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dev_init_ha_define_warmboot_support;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_dev_init_ha_warmboot_support_set;
    data_index = dnx_data_dev_init_ha_define_sw_state_max_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_dev_init_ha_sw_state_max_size_set;
    data_index = dnx_data_dev_init_ha_define_stable_location;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_dev_init_ha_stable_location_set;
    data_index = dnx_data_dev_init_ha_define_stable_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_dev_init_ha_stable_size_set;

    

    
    data_index = dnx_data_dev_init_ha_table_stable_filename;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_dev_init_ha_stable_filename_set;
    
    submodule_index = dnx_data_dev_init_submodule_shadow;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_dev_init_shadow_table_uncacheable_mem;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_dev_init_shadow_uncacheable_mem_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

