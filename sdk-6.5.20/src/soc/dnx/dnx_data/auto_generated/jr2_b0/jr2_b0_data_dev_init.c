

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
#include <bcm_int/dnx/init/init.h>
#include <appl/reference/dnx/appl_ref_init_deinit.h>








static shr_error_e
jr2_b0_dnx_data_dev_init_time_init_total_thresh_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_time;
    int define_index = dnx_data_dev_init_time_define_init_total_thresh;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 18034800;

    
    define->data = 18034800;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_b0_dnx_data_dev_init_time_appl_init_total_thresh_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dev_init;
    int submodule_index = dnx_data_dev_init_submodule_time;
    int define_index = dnx_data_dev_init_time_define_appl_init_total_thresh;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 19550400;

    
    define->data = 19550400;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_b0_dnx_data_dev_init_time_step_thresh_set(
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
        data->value = 1112400;
    }
    if (DNX_INIT_STEP_TUNE < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_TUNE, 0);
        data->value = 1112000;
    }
    if (DNX_INIT_STEP_TM_MODULES < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_TM_MODULES, 0);
        data->value = 6356400;
    }
    if (DNX_INIT_STEP_PP_GENERAL < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_PP_GENERAL, 0);
        data->value = 2080000;
    }
    if (DNX_INIT_MDB < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_MDB, 0);
        data->value = 801600;
    }
    if (DNX_INIT_STEP_PEMLA < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_PEMLA, 0);
        data->value = 1341600;
    }
    if (DNX_INIT_STEP_FABRIC_MODULE < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_FABRIC_MODULE, 0);
        data->value = 5458800;
    }
    if (DNX_INIT_STEP_MEM_DEFAULTS < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_MEM_DEFAULTS, 0);
        data->value = 1068000;
    }
    if (DNX_INIT_STEP_ALGO < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_ALGO, 0);
        data->value = 594000;
    }
    if (DNX_INIT_STEP_SOFT_RESET < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_SOFT_RESET, 0);
        data->value = 226800;
    }
    if (DNX_INIT_STEP_DBAL_ACCESS_MDB < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_DBAL_ACCESS_MDB, 0);
        data->value = 673200;
    }
    if (DNX_INIT_STEP_DBAL_TABLES < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_DBAL_TABLES, 0);
        data->value = 367200;
    }
    if (DNX_INIT_STEP_PP_MODULES < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_PP_MODULES, 0);
        data->value = 4341600;
    }
    if (DNX_INIT_STEP_SW_STATE < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_SW_STATE, 0);
        data->value = 936000;
    }
    if (DNX_INIT_STEP_AUX_ACCESS < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_AUX_ACCESS, 0);
        data->value = 1905600;
    }
    if (DNX_INIT_STEP_SOFT_INIT < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_SOFT_INIT, 0);
        data->value = 330000;
    }
    if (DNX_INIT_STEP_DYN_PORT_PROP_SET < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_DYN_PORT_PROP_SET, 0);
        data->value = 788400;
    }
    if (DNX_INIT_STEP_ACCESS < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_ACCESS, 0);
        data->value = 249600;
    }
    if (DNX_INIT_STEP_DRAM < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_DRAM, 0);
        data->value = 456000;
    }
    if (DNX_INIT_STEP_PRE_SOFT_INIT < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_PRE_SOFT_INIT, 0);
        data->value = 1130400;
    }
    if (DNX_INIT_STEP_APPL_PROPERTIES < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_APPL_PROPERTIES, 0);
        data->value = 2264400;
    }
    if (DNX_INIT_STEP_COMMON_MODULES < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_COMMON_MODULES, 0);
        data->value = 2616000;
    }
    if (DNX_INIT_STEP_DNX_DATA < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_DNX_DATA, 0);
        data->value = 1395600;
    }
    if (DNX_INIT_STEP_DYN_PORT_ADD < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_DYN_PORT_ADD, 0);
        data->value = 1143600;
    }
    if (DNX_INIT_STEP_FIELD < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_FIELD, 0);
        data->value = 346800;
    }
    if (DNX_INIT_STEP_FLD_SWSTATE < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_FLD_SWSTATE, 0);
        data->value = 280800;
    }
    if (DNX_INIT_STEP_ALGO_LIF < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_ALGO_LIF, 0);
        data->value = 450000;
    }
    if (DNX_INIT_STEP_SCH_MODULE < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_SCH_MODULE, 0);
        data->value = 150000;
    }
    if (DNX_INIT_STEP_LIF < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_LIF, 0);
        data->value = 206400;
    }
    if (DNX_INIT_STEP_OAM < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_OAM, 0);
        data->value = 136800;
    }
    if (DNX_INIT_STEP_VISIBILITY < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INIT_STEP_VISIBILITY, 0);
        data->value = 123600;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_b0_dnx_data_dev_init_time_appl_step_thresh_set(
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
        data->value = 423020;
    }
    if (APPL_DNX_STEP_SRV6 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_appl_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNX_STEP_SRV6, 0);
        data->value = 87570;
    }
    if (APPL_DNX_STEP_SYS_PORTS < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_appl_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNX_STEP_SYS_PORTS, 0);
        data->value = 535340;
    }
    if (APPL_DNX_STEP_VLAN < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_appl_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNX_STEP_VLAN, 0);
        data->value = 577676;
    }
    if (APPL_DNX_STEP_SDK < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_appl_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNX_STEP_SDK, 0);
        data->value = 15891222;
    }
    if (APPL_DNX_STEP_E2E_SCHEME < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_appl_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNX_STEP_E2E_SCHEME, 0);
        data->value = 100000;
    }
    if (APPL_DNX_STEP_LINKSCAN < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_appl_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNX_STEP_LINKSCAN, 0);
        data->value = 10030;
    }
    if (APPL_DNX_STEP_APPS < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_appl_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNX_STEP_APPS, 0);
        data->value = 2476900;
    }
    if (APPL_DNX_STEP_ITMH_PPH < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_appl_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNX_STEP_ITMH_PPH, 0);
        data->value = 174829;
    }
    if (APPL_DNX_STEP_OAM < table->keys[0].size)
    {
        data = (dnx_data_dev_init_time_appl_step_thresh_t *) dnxc_data_mgmt_table_data_get(unit, table, APPL_DNX_STEP_OAM, 0);
        data->value = 223374;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_b0_dnx_data_dev_init_shadow_uncacheable_mem_set(
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

    
    table->keys[0].size = 99;
    table->info_get.key_size[0] = 99;

    
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
        data->mem = ILE_PORT_0_CPU_ACCESSm;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->mem = ILE_PORT_1_CPU_ACCESSm;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->mem = IPPF_LEARN_PAYLOAD_CONTEXT_MAPPINGm;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->mem = SCH_SCHEDULER_INITm;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->mem = ERPP_MEM_F000000m;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->mem = IPPF_MEM_F000000m;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->mem = RX_LKUP_1588_MEM_400Gm;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->mem = RX_LKUP_1588_MEM_MPP0m;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->mem = RX_LKUP_1588_MEM_MPP1m;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->mem = SPEED_ID_TABLEm;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->mem = SPEED_PRIORITY_MAP_TBLm;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->mem = TX_LKUP_1588_MEM_400Gm;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->mem = TX_LKUP_1588_MEM_MPP0m;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->mem = TX_LKUP_1588_MEM_MPP1m;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->mem = UM_TABLEm;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->mem = MDB_ARM_KAPS_TCMm;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->mem = ERPP_EPMFCS_TCAM_BANK_COMMANDm;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->mem = IPPA_VTDCS_TCAM_BANK_COMMANDm;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->mem = IPPA_VTECS_TCAM_BANK_COMMANDm;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->mem = IPPB_FLPACS_TCAM_BANK_COMMANDm;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->mem = IPPB_FLPBCS_TCAM_BANK_COMMANDm;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->mem = IPPC_PMFACSA_TCAM_BANK_COMMANDm;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->mem = IPPC_PMFACSB_TCAM_BANK_COMMANDm;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->mem = IPPD_PMFBCS_TCAM_BANK_COMMANDm;
    }
    if (25 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, 0);
        data->mem = IPPE_LLRCS_TCAM_BANK_COMMANDm;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->mem = IPPE_PRTCAM_TCAM_BANK_COMMANDm;
    }
    if (27 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, 0);
        data->mem = IPPF_VTACS_TCAM_BANK_COMMANDm;
    }
    if (28 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 28, 0);
        data->mem = IPPF_VTBCS_TCAM_BANK_COMMANDm;
    }
    if (29 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 29, 0);
        data->mem = IPPF_VTCCS_TCAM_BANK_COMMANDm;
    }
    if (30 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 30, 0);
        data->mem = TCAM_TCAM_BANK_COMMANDm;
    }
    if (31 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 31, 0);
        data->mem = ERPP_EPMFCS_TCAM_BANK_REPLYm;
    }
    if (32 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 32, 0);
        data->mem = IPPA_VTDCS_TCAM_BANK_REPLYm;
    }
    if (33 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 33, 0);
        data->mem = IPPA_VTECS_TCAM_BANK_REPLYm;
    }
    if (34 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 34, 0);
        data->mem = IPPB_FLPACS_TCAM_BANK_REPLYm;
    }
    if (35 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 35, 0);
        data->mem = IPPB_FLPBCS_TCAM_BANK_REPLYm;
    }
    if (36 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 36, 0);
        data->mem = IPPC_PMFACSA_TCAM_BANK_REPLYm;
    }
    if (37 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 37, 0);
        data->mem = IPPC_PMFACSB_TCAM_BANK_REPLYm;
    }
    if (38 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 38, 0);
        data->mem = IPPD_PMFBCS_TCAM_BANK_REPLYm;
    }
    if (39 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 39, 0);
        data->mem = IPPE_LLRCS_TCAM_BANK_REPLYm;
    }
    if (40 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 40, 0);
        data->mem = IPPE_PRTCAM_TCAM_BANK_REPLYm;
    }
    if (41 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 41, 0);
        data->mem = IPPF_VTACS_TCAM_BANK_REPLYm;
    }
    if (42 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 42, 0);
        data->mem = IPPF_VTBCS_TCAM_BANK_REPLYm;
    }
    if (43 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 43, 0);
        data->mem = IPPF_VTCCS_TCAM_BANK_REPLYm;
    }
    if (44 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 44, 0);
        data->mem = TCAM_TCAM_BANK_REPLYm;
    }
    if (45 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 45, 0);
        data->mem = ERPP_EPMFCS_TCAM_BANKm;
    }
    if (46 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 46, 0);
        data->mem = IPPA_VTDCS_TCAM_BANKm;
    }
    if (47 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 47, 0);
        data->mem = IPPA_VTECS_TCAM_BANKm;
    }
    if (48 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 48, 0);
        data->mem = IPPB_FLPACS_TCAM_BANKm;
    }
    if (49 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 49, 0);
        data->mem = IPPB_FLPBCS_TCAM_BANKm;
    }
    if (50 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 50, 0);
        data->mem = IPPC_PMFACSA_TCAM_BANKm;
    }
    if (51 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 51, 0);
        data->mem = IPPC_PMFACSB_TCAM_BANKm;
    }
    if (52 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 52, 0);
        data->mem = IPPD_PMFBCS_TCAM_BANKm;
    }
    if (53 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 53, 0);
        data->mem = IPPE_LLRCS_TCAM_BANKm;
    }
    if (54 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 54, 0);
        data->mem = IPPE_PRTCAM_TCAM_BANKm;
    }
    if (55 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 55, 0);
        data->mem = IPPF_VTACS_TCAM_BANKm;
    }
    if (56 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 56, 0);
        data->mem = IPPF_VTBCS_TCAM_BANKm;
    }
    if (57 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 57, 0);
        data->mem = IPPF_VTCCS_TCAM_BANKm;
    }
    if (58 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 58, 0);
        data->mem = TCAM_TCAM_BANKm;
    }
    if (59 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 59, 0);
        data->mem = AM_TABLEm;
    }
    if (60 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 60, 0);
        data->mem = SCH_TOKEN_MEMORY_CONTROLLER_TMCm;
    }
    if (61 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 61, 0);
        data->mem = MDB_ARM_MEM_0m;
    }
    if (62 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 62, 0);
        data->mem = MDB_ISEM_1m;
    }
    if (63 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 63, 0);
        data->mem = MDB_ISEM_2m;
    }
    if (64 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 64, 0);
        data->mem = MDB_ISEM_3m;
    }
    if (65 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 65, 0);
        data->mem = MDB_IOEM_0m;
    }
    if (66 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 66, 0);
        data->mem = MDB_IOEM_1m;
    }
    if (67 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 67, 0);
        data->mem = MDB_MC_IDm;
    }
    if (68 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 68, 0);
        data->mem = MDB_GLEM_0m;
    }
    if (69 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 69, 0);
        data->mem = MDB_GLEM_1m;
    }
    if (70 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 70, 0);
        data->mem = MDB_EOEM_0m;
    }
    if (71 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 71, 0);
        data->mem = MDB_EOEM_1m;
    }
    if (72 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 72, 0);
        data->mem = EVNT_ASSIGNED_CMIC_ENGINE_TABLEm;
    }
    if (73 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 73, 0);
        data->mem = EVNT_SPACE_REGIONSm;
    }
    if (74 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 74, 0);
        data->mem = EVNT_CMIC_ENG_PROPERTIESm;
    }
    if (75 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 75, 0);
        data->mem = OAMP_MEP_DBm;
    }
    if (76 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 76, 0);
        data->mem = OAMP_RMEP_DBm;
    }
    if (77 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 77, 0);
        data->mem = MDB_ESEMm;
    }
    if (78 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 78, 0);
        data->mem = MDB_EXEM_1m;
    }
    if (79 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 79, 0);
        data->mem = MDB_EXEM_2m;
    }
    if (80 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 80, 0);
        data->mem = MDB_RMEPm;
    }
    if (81 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 81, 0);
        data->mem = IPPC_PER_PORT_STATISTICSm;
    }
    if (82 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 82, 0);
        data->mem = ETPPA_MEM_F000000m;
    }
    if (83 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 83, 0);
        data->mem = ETPPA_PARSING_START_TYPE_MAPPING_TABLEm;
    }
    if (84 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 84, 0);
        data->mem = ETPPB_MEM_F000000m;
    }
    if (85 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 85, 0);
        data->mem = ETPPB_MTU_MAP_TABLEm;
    }
    if (86 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 86, 0);
        data->mem = ETPPC_MEM_F000000m;
    }
    if (87 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 87, 0);
        data->mem = ETPPC_MTU_MAP_TABLEm;
    }
    if (88 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 88, 0);
        data->mem = IPPA_MEM_F000000m;
    }
    if (89 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 89, 0);
        data->mem = IPPB_MEM_F000000m;
    }
    if (90 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 90, 0);
        data->mem = IPPB_MPLS_LABEL_TYPES_CAM_0m;
    }
    if (91 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 91, 0);
        data->mem = IPPB_MPLS_LABEL_TYPES_CAM_1m;
    }
    if (92 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 92, 0);
        data->mem = IPPC_MEM_F000000m;
    }
    if (93 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 93, 0);
        data->mem = IPPD_MEM_F000000m;
    }
    if (94 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 94, 0);
        data->mem = IPPD_MIRROR_CODE_TO_CMDm;
    }
    if (95 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 95, 0);
        data->mem = IPPE_MEM_B900000m;
    }
    if (96 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 96, 0);
        data->mem = IPPE_MEM_F0000m;
    }
    if (97 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 97, 0);
        data->mem = SCH_RESERVED_37m;
    }
    if (98 < table->keys[0].size)
    {
        data = (dnx_data_dev_init_shadow_uncacheable_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 98, 0);
        data->mem = SCH_RESERVED_54m;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
jr2_b0_data_dev_init_attach(
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
    define->set = jr2_b0_dnx_data_dev_init_time_init_total_thresh_set;
    data_index = dnx_data_dev_init_time_define_appl_init_total_thresh;
    define = &submodule->defines[data_index];
    define->set = jr2_b0_dnx_data_dev_init_time_appl_init_total_thresh_set;

    

    
    data_index = dnx_data_dev_init_time_table_step_thresh;
    table = &submodule->tables[data_index];
    table->set = jr2_b0_dnx_data_dev_init_time_step_thresh_set;
    data_index = dnx_data_dev_init_time_table_appl_step_thresh;
    table = &submodule->tables[data_index];
    table->set = jr2_b0_dnx_data_dev_init_time_appl_step_thresh_set;
    
    submodule_index = dnx_data_dev_init_submodule_shadow;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_dev_init_shadow_table_uncacheable_mem;
    table = &submodule->tables[data_index];
    table->set = jr2_b0_dnx_data_dev_init_shadow_uncacheable_mem_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

