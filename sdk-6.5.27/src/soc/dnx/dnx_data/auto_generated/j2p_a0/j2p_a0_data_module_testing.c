
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COMMON

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_module_testing.h>









static shr_error_e
j2p_a0_dnx_data_module_testing_example_tests_mem_consumption_set(
    int unit)
{
    int module_id_index;
    dnx_data_module_testing_example_tests_mem_consumption_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_module_testing;
    int submodule_index = dnx_data_module_testing_submodule_example_tests;
    int table_index = dnx_data_module_testing_example_tests_table_mem_consumption;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = NOF_MODULE_ID;
    table->info_get.key_size[0] = NOF_MODULE_ID;

    
    table->values[0].default_val = "DATA(module_testing, example_tests, default_memory_size)";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_module_testing_example_tests_mem_consumption_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_module_testing_example_tests_table_mem_consumption");

    
    default_data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->memory_size = dnx_data_module_testing.example_tests.default_memory_size_get(unit);
    
    for (module_id_index = 0; module_id_index < table->keys[0].size; module_id_index++)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, module_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (ALGO_BFD_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, ALGO_BFD_MODULE_ID, 0);
        data->memory_size = 45232;
    }
    if (ALGO_CONSISTENT_HASHING_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, ALGO_CONSISTENT_HASHING_MODULE_ID, 0);
        data->memory_size = 60304;
    }
    if (ALGO_ECGM_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, ALGO_ECGM_MODULE_ID, 0);
        data->memory_size = 78416;
    }
    if (ALGO_FAILOVER_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, ALGO_FAILOVER_MODULE_ID, 0);
        data->memory_size = 88560;
    }
    if (DNX_ALGO_FIELD_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_ALGO_FIELD_MODULE_ID, 0);
        data->memory_size = 1438096;
    }
    if (ALGO_L2_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, ALGO_L2_MODULE_ID, 0);
        data->memory_size = 10464;
    }
    if (ALGO_L3_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, ALGO_L3_MODULE_ID, 0);
        data->memory_size = 2975688;
    }
    if (ALGO_LIF_PROFILE_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, ALGO_LIF_PROFILE_MODULE_ID, 0);
        data->memory_size = 51840;
    }
    if (ALGO_OAM_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, ALGO_OAM_MODULE_ID, 0);
        data->memory_size = 118624;
    }
    if (ALGO_OAMP_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, ALGO_OAMP_MODULE_ID, 0);
        data->memory_size = 47560;
    }
    if (DNX_ALGO_PORT_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_ALGO_PORT_MODULE_ID, 0);
        data->memory_size = 683656;
    }
    if (DNX_ALGO_PORT_IMB_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_ALGO_PORT_IMB_MODULE_ID, 0);
        data->memory_size = 14840;
    }
    if (ALGO_PORT_PP_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, ALGO_PORT_PP_MODULE_ID, 0);
        data->memory_size = 54600;
    }
    if (PPMC_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, PPMC_MODULE_ID, 0);
        data->memory_size = 99968;
    }
    if (ALGO_QOS_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, ALGO_QOS_MODULE_ID, 0);
        data->memory_size = 23264;
    }
    if (ALGO_RX_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, ALGO_RX_MODULE_ID, 0);
        data->memory_size = 15200;
    }
    if (ALGO_SAT_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, ALGO_SAT_MODULE_ID, 0);
        data->memory_size = 27792;
    }
    if (ALGO_TRUNK_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, ALGO_TRUNK_MODULE_ID, 0);
        data->memory_size = 954120;
    }
    if (ALGO_TUNNEL_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, ALGO_TUNNEL_MODULE_ID, 0);
        data->memory_size = 5599816;
    }
    if (CRPS_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, CRPS_MODULE_ID, 0);
        data->memory_size = 33084776;
    }
    if (ECMP_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, ECMP_MODULE_ID, 0);
        data->memory_size = 356384;
    }
    if (DNX_EGQ_AM_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_EGQ_AM_MODULE_ID, 0);
        data->memory_size = 37608;
    }
    if (FLOW_CONTROL_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, FLOW_CONTROL_MODULE_ID, 0);
        data->memory_size = 34664;
    }
    if (DNX_INGRESS_CONGESTION_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_MODULE_ID, 0);
        data->memory_size = 168080;
    }
    if (DNX_INGRESS_REASSEMBLY_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_REASSEMBLY_MODULE_ID, 0);
        data->memory_size = 23584;
    }
    if (INSTRU_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, INSTRU_MODULE_ID, 0);
        data->memory_size = 20048;
    }
    if (DNX_IPQ_ALLOC_MNGR_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_IPQ_ALLOC_MNGR_MODULE_ID, 0);
        data->memory_size = 39256;
    }
    if (LIF_MNGR_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, LIF_MNGR_MODULE_ID, 0);
        data->memory_size = 19461912;
    }
    if (LIF_TABLE_MNGR_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, LIF_TABLE_MNGR_MODULE_ID, 0);
        data->memory_size = 5494688;
    }
    if (METER_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, METER_MODULE_ID, 0);
        data->memory_size = 1597440;
    }
    if (DNX_MIRROR_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_MIRROR_MODULE_ID, 0);
        data->memory_size = 16968;
    }
    if (MULTICAST_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, MULTICAST_MODULE_ID, 0);
        data->memory_size = 4423440;
    }
    if (PORT_TPID_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, PORT_TPID_MODULE_ID, 0);
        data->memory_size = 10768;
    }
    if (DNX_SCH_ALLOC_MNGR_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_ALLOC_MNGR_MODULE_ID, 0);
        data->memory_size = 912272;
    }
    if (SEC_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, SEC_MODULE_ID, 0);
        data->memory_size = 7787576;
    }
    if (VLAN_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, VLAN_MODULE_ID, 0);
        data->memory_size = 68360;
    }
    if (DNX_ALGO_FIELD_ACTION_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_ALGO_FIELD_ACTION_MODULE_ID, 0);
        data->memory_size = 38016;
    }
    if (DNX_ALGO_LANE_MAP_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_ALGO_LANE_MAP_MODULE_ID, 0);
        data->memory_size = 11624;
    }
    if (ALGO_LIF_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, ALGO_LIF_MODULE_ID, 0);
        data->memory_size = 26160;
    }
    if (ALGO_MDB_EM_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, ALGO_MDB_EM_MODULE_ID, 0);
        data->memory_size = 36936;
    }
    if (DBAL_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_MODULE_ID, 0);
        data->memory_size = 91453584;
    }
    if (DNX_EGR_DB_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_EGR_DB_MODULE_ID, 0);
        data->memory_size = 10936;
    }
    if (DNX_FIFODMA_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIFODMA_MODULE_ID, 0);
        data->memory_size = 12776;
    }
    if (DNX_OFP_RATE_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_OFP_RATE_MODULE_ID, 0);
        data->memory_size = 58040;
    }
    if (DNX_RX_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_RX_MODULE_ID, 0);
        data->memory_size = 16792;
    }
    if (DNX_DRAM_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_DRAM_MODULE_ID, 0);
        data->memory_size = 16768;
    }
    if (DNX_FABRIC_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FABRIC_MODULE_ID, 0);
        data->memory_size = 18000;
    }
    if (DNX_FIELD_ACTIONS_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_ACTIONS_MODULE_ID, 0);
        data->memory_size = 27456;
    }
    if (DNX_FIELD_CONTEXT_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_CONTEXT_MODULE_ID, 0);
        data->memory_size = 332600;
    }
    if (DNX_FIELD_GROUP_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_GROUP_MODULE_ID, 0);
        data->memory_size = 4587360;
    }
    if (DNX_FIELD_QUALIFIER_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_QUALIFIER_MODULE_ID, 0);
        data->memory_size = 13200;
    }
    if (DNX_FIELD_TCAM_ACCESS_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_TCAM_ACCESS_MODULE_ID, 0);
        data->memory_size = 1913032;
    }
    if (DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID, 0);
        data->memory_size = 824512;
    }
    if (DNX_FIELD_TCAM_BANK_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_TCAM_BANK_MODULE_ID, 0);
        data->memory_size = 47520;
    }
    if (DNX_FIELD_TCAM_CACHE_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_TCAM_CACHE_MODULE_ID, 0);
        data->memory_size = 2569600;
    }
    if (DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID, 0);
        data->memory_size = 17960;
    }
    if (DNX_FIELD_TCAM_LOCATION_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_TCAM_LOCATION_MODULE_ID, 0);
        data->memory_size = 4535568;
    }
    if (FLOW_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, FLOW_MODULE_ID, 0);
        data->memory_size = 12872;
    }
    if (INTERRUPT_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, INTERRUPT_MODULE_ID, 0);
        data->memory_size = 30824;
    }
    if (DNX_IPQ_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_IPQ_MODULE_ID, 0);
        data->memory_size = 4229240;
    }
    if (MDB_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_MODULE_ID, 0);
        data->memory_size = 10715848;
    }
    if (MDB_EM_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_EM_MODULE_ID, 0);
        data->memory_size = 128394328;
    }
    if (OAM_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, OAM_MODULE_ID, 0);
        data->memory_size = 6405392;
    }
    if (RX_TRAP_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, RX_TRAP_MODULE_ID, 0);
        data->memory_size = 28872;
    }
    if (DNX_PORT_IMB_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PORT_IMB_MODULE_ID, 0);
        data->memory_size = 13552;
    }
    if (DNX_SCH_CONFIG_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCH_CONFIG_MODULE_ID, 0);
        data->memory_size = 9708232;
    }
    if (DNX_SCHEDULER_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SCHEDULER_MODULE_ID, 0);
        data->memory_size = 45792;
    }
    if (TRUNK_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, TRUNK_MODULE_ID, 0);
        data->memory_size = 1016552;
    }
    if (WB_ENGINE_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, WB_ENGINE_MODULE_ID, 0);
        data->memory_size = 347632;
    }
    if (ALGO_FLOW_LIF_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, ALGO_FLOW_LIF_MODULE_ID, 0);
        data->memory_size = 108792;
    }
    if (SW_STATE_EXTERNAL_MODULE_ID < table->keys[0].size)
    {
        data = (dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_get(unit, table, SW_STATE_EXTERNAL_MODULE_ID, 0);
        data->memory_size = 59863536;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
j2p_a0_data_module_testing_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_module_testing;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_module_testing_submodule_example_tests;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_module_testing_example_tests_table_mem_consumption;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_module_testing_example_tests_mem_consumption_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

