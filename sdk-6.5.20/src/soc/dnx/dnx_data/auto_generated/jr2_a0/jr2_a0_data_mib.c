

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COMMON

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_mib.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>







static shr_error_e
jr2_a0_dnx_data_mib_general_mib_counters_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mib;
    int submodule_index = dnx_data_mib_submodule_general;
    int feature_index = dnx_data_mib_general_mib_counters_support;
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
jr2_a0_dnx_data_mib_general_nof_row_per_cdu_lane_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mib;
    int submodule_index = dnx_data_mib_submodule_general;
    int define_index = dnx_data_mib_general_define_nof_row_per_cdu_lane;
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
jr2_a0_dnx_data_mib_general_stat_interval_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mib;
    int submodule_index = dnx_data_mib_submodule_general;
    int define_index = dnx_data_mib_general_define_stat_interval;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1000000;

    
    define->data = 1000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_BCM_STAT_INTERVAL;
    define->property.doc = 
        "\n"
        "Set statistics collection interval in microseconds\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 0xffffffff;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mib_general_stat_jumbo_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mib;
    int submodule_index = dnx_data_mib_submodule_general;
    int define_index = dnx_data_mib_general_define_stat_jumbo;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1518;

    
    define->data = 1518;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->flags |= DNXC_DATA_F_INIT_ONLY;
    
    define->property.name = spn_BCM_STAT_JUMBO;
    define->property.doc = 
        "\n"
        "Maximum packet size used in statistics counter update\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 0x3fff;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_mib_general_stat_pbmp_set(
    int unit)
{
    dnx_data_mib_general_stat_pbmp_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mib;
    int submodule_index = dnx_data_mib_submodule_general;
    int table_index = dnx_data_mib_general_table_stat_pbmp;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->flags |= DNXC_DATA_F_INIT_ONLY;

    
    table->values[0].default_val = "0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_mib_general_stat_pbmp_t, (1 + 1  ), "data of dnx_data_mib_general_table_stat_pbmp");

    
    default_data = (dnx_data_mib_general_stat_pbmp_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    #if 40 > _SHR_PBMP_WORD_MAX
    #error "out of bound access to array"
    #endif
    _SHR_PBMP_WORD_SET(default_data->pbmp, 0, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 1, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 2, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 3, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 4, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 5, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 6, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 7, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 8, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 9, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 10, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 11, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 12, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 13, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 14, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 15, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 16, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 17, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 18, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 19, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 20, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 21, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 22, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 23, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 24, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 25, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 26, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 27, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 28, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 29, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 30, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 31, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 32, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 33, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 34, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 35, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 36, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 37, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 38, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 39, 0xffffffff);
    
    data = (dnx_data_mib_general_stat_pbmp_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->values[0].property.name = spn_BCM_STAT_PBMP;
    table->values[0].property.doc =
        "\n"
        "Bitmap of ports to enable statistic counter collection\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_pbmp;
    table->values[0].property.method_str = "pbmp";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnx_data_mib_general_stat_pbmp_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, -1, &data->pbmp));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_mib_controlled_counters_map_data_set(
    int unit)
{
    int counter_type_index;
    int set_type_index;
    dnx_data_mib_controlled_counters_map_data_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mib;
    int submodule_index = dnx_data_mib_submodule_controlled_counters;
    int table_index = dnx_data_mib_controlled_counters_table_map_data;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = snmpValCount;
    table->info_get.key_size[0] = snmpValCount;
    table->keys[1].size = DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NOF;
    table->info_get.key_size[1] = DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NOF;

    
    table->values[0].default_val = "-1, -1, -1, -1";
    table->values[1].default_val = "-1, -1, -1, -1";
    table->values[2].default_val = "NULL";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mib_controlled_counters_map_data_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_mib_controlled_counters_table_map_data");

    
    default_data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    SHR_RANGE_VERIFY(4, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
    default_data->counters_to_add_array[0] = -1;
    default_data->counters_to_add_array[1] = -1;
    default_data->counters_to_add_array[2] = -1;
    default_data->counters_to_add_array[3] = -1;
    SHR_RANGE_VERIFY(4, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
    default_data->counters_to_subtract_array[0] = -1;
    default_data->counters_to_subtract_array[1] = -1;
    default_data->counters_to_subtract_array[2] = -1;
    default_data->counters_to_subtract_array[3] = -1;
    default_data->counters_to_get_cb = NULL;
    
    for (counter_type_index = 0; counter_type_index < table->keys[0].size; counter_type_index++)
    {
        for (set_type_index = 0; set_type_index < table->keys[1].size; set_type_index++)
        {
            data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, counter_type_index, set_type_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (snmpBcmTxControlCells < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTxControlCells, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->counters_to_add_array[0] = dnx_mib_counter_tx_control_cells;
    }
    if (snmpBcmTxDataCells < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTxDataCells, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->counters_to_add_array[0] = dnx_mib_counter_tx_data_cells;
    }
    if (snmpBcmTxDataBytes < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTxDataBytes, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->counters_to_add_array[0] = dnx_mib_counter_tx_data_bytes;
    }
    if (snmpBcmRxCrcErrors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxCrcErrors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        SHR_RANGE_VERIFY(3, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_rx_crc_errors_data_cells;
        data->counters_to_add_array[1] = dnx_mib_counter_rx_crc_errors_control_cells_nonbypass;
        data->counters_to_add_array[2] = dnx_mib_counter_rx_crc_errors_control_cells_bypass;
    }
    if (snmpBcmRxFecCorrectable < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxFecCorrectable, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->counters_to_add_array[0] = dnx_mib_counter_rx_fec_correctable_error;
    }
    if (snmpBcmRxFecCorrectable < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxFecCorrectable, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_rx_fec_correctable_errors;
    }
    if (snmpBcmRxControlCells < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxControlCells, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->counters_to_add_array[0] = dnx_mib_counter_rx_control_cells;
    }
    if (snmpBcmRxDataCells < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxDataCells, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->counters_to_add_array[0] = dnx_mib_counter_rx_data_cells;
    }
    if (snmpBcmRxDataBytes < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxDataBytes, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->counters_to_add_array[0] = dnx_mib_counter_rx_data_bytes;
    }
    if (snmpBcmRxDroppedRetransmittedControl < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxDroppedRetransmittedControl, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->counters_to_add_array[0] = dnx_mib_counter_rx_dropped_retransmitted_control;
    }
    if (snmpBcmTxAsynFifoRate < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTxAsynFifoRate, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->counters_to_add_array[0] = dnx_mib_counter_tx_asyn_fifo_rate;
    }
    if (snmpBcmTxAsynFifoRate < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN_OVER_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTxAsynFifoRate, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN_OVER_FABRIC);
        data->counters_to_add_array[0] = dnx_mib_counter_tx_asyn_fifo_rate;
    }
    if (snmpBcmRxAsynFifoRate < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxAsynFifoRate, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->counters_to_add_array[0] = dnx_mib_counter_rx_asyn_fifo_rate;
    }
    if (snmpBcmRxAsynFifoRate < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN_OVER_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxAsynFifoRate, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN_OVER_FABRIC);
        data->counters_to_add_array[0] = dnx_mib_counter_rx_asyn_fifo_rate;
    }
    if (snmpBcmRxFecUncorrectable < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxFecUncorrectable, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->counters_to_add_array[0] = dnx_mib_counter_rx_lfec_fec_uncorrrectable_errors;
    }
    if (snmpBcmRxFecUncorrectable < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxFecUncorrectable, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_rx_fec_uncorrectable_errors;
    }
    if (snmpBcmRxBipErrorCount < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxBipErrorCount, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_rx_bip_errors;
    }
    if (snmpBcmRxBipErrorCount < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxBipErrorCount, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->counters_to_get_cb = dnx_mib_counters_map_get_dummy_cb;
    }
    if (snmpBcmRxRsFecSymbolError < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxRsFecSymbolError, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->counters_to_add_array[0] = dnx_mib_counter_rx_rs_fec_symbol_error_rate;
    }
    if (snmpIfInOctets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfInOctets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rbyt;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_rrbyt;
    }
    if (snmpIfInOctets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfInOctets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rbyt;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_rrbyt;
    }
    if (snmpIfInOctets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_CPU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfInOctets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_CPU);
        data->counters_to_get_cb = dnx_mib_counters_map_get_dummy_cb;
    }
    if (snmpIfInUcastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfInUcastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_ruca;
    }
    if (snmpIfInUcastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfInUcastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_ruca;
    }
    if (snmpIfInUcastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_CPU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfInUcastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_CPU);
        data->counters_to_get_cb = dnx_mib_counters_map_get_dummy_cb;
    }
    if (snmpIfInNUcastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfInNUcastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rmca;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_rbca;
    }
    if (snmpIfInNUcastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfInNUcastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rmca;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_rbca;
    }
    if (snmpIfInNUcastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfInNUcastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->counters_to_get_cb = dnx_mib_counters_map_get_dummy_cb;
    }
    if (snmpIfInNUcastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfInNUcastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN);
        data->counters_to_get_cb = dnx_mib_counters_map_get_dummy_cb;
    }
    if (snmpIfInBroadcastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfInBroadcastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rbca;
    }
    if (snmpIfInBroadcastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfInBroadcastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rbca;
    }
    if (snmpIfInMulticastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfInMulticastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rmca;
    }
    if (snmpIfInMulticastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfInMulticastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rmca;
    }
    if (snmpIfInErrors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfInErrors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        SHR_RANGE_VERIFY(4, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rfcs;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_rjbr;
        data->counters_to_add_array[2] = dnx_mib_counter_nif_eth_rrpkt;
        data->counters_to_add_array[3] = dnx_mib_counter_nif_eth_rmtue;
    }
    if (snmpIfInErrors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfInErrors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        SHR_RANGE_VERIFY(4, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rfcs;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_rjbr;
        data->counters_to_add_array[2] = dnx_mib_counter_nif_eth_rrpkt;
        data->counters_to_add_array[3] = dnx_mib_counter_nif_eth_rmtue;
    }
    if (snmpIfInErrors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfInErrors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->counters_to_get_cb = dnx_mib_counters_map_get_dummy_cb;
    }
    if (snmpIfInErrors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfInErrors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN);
        data->counters_to_add_array[0] = dnx_mib_counter_ilkn_rx_err_pkt;
    }
    if (snmpIfOutOctets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfOutOctets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tbyt;
    }
    if (snmpIfOutOctets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfOutOctets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tbyt;
    }
    if (snmpIfOutOctets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_CPU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfOutOctets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_CPU);
        data->counters_to_get_cb = dnx_mib_counters_map_get_dummy_cb;
    }
    if (snmpIfOutUcastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfOutUcastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tuca;
    }
    if (snmpIfOutUcastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfOutUcastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tuca;
    }
    if (snmpIfOutUcastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_CPU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfOutUcastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_CPU);
        data->counters_to_get_cb = dnx_mib_counters_map_get_dummy_cb;
    }
    if (snmpIfOutNUcastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfOutNUcastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tmca;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_tbca;
    }
    if (snmpIfOutNUcastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfOutNUcastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tmca;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_tbca;
    }
    if (snmpIfOutNUcastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfOutNUcastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN);
        data->counters_to_get_cb = dnx_mib_counters_map_get_dummy_cb;
    }
    if (snmpIfOutNUcastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfOutNUcastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->counters_to_get_cb = dnx_mib_counters_map_get_dummy_cb;
    }
    if (snmpIfOutBroadcastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfOutBroadcastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tbca;
    }
    if (snmpIfOutBroadcastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfOutBroadcastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tbca;
    }
    if (snmpIfOutMulticastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfOutMulticastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tmca;
    }
    if (snmpIfOutMulticastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfOutMulticastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tmca;
    }
    if (snmpIfOutErrors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfOutErrors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_get_cb = dnx_mib_counters_map_get_cb;
    }
    if (snmpIfOutErrors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfOutErrors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_get_cb = dnx_mib_counters_map_get_cb;
    }
    if (snmpIfOutErrors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfOutErrors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN);
        data->counters_to_add_array[0] = dnx_mib_counter_ilkn_tx_err_pkt;
    }
    if (snmpIfOutErrors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfOutErrors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->counters_to_get_cb = dnx_mib_counters_map_get_dummy_cb;
    }
    if (snmpDot1dBasePortMtuExceededDiscards < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpDot1dBasePortMtuExceededDiscards, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rmtue;
    }
    if (snmpDot1dTpPortInFrames < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpDot1dTpPortInFrames, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpkt;
    }
    if (snmpDot1dTpPortOutFrames < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpDot1dTpPortOutFrames, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tpkt;
    }
    if (snmpDot1dBasePortMtuExceededDiscards < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpDot1dBasePortMtuExceededDiscards, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rmtue;
    }
    if (snmpDot1dTpPortInFrames < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpDot1dTpPortInFrames, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpkt;
    }
    if (snmpDot1dTpPortOutFrames < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpDot1dTpPortOutFrames, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tpkt;
    }
    if (snmpEtherStatsDropEvents < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsDropEvents, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_rx_stats_drop_events_sch_low;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_rx_stats_drop_events_sch_tdm;
    }
    if (snmpEtherStatsDropEvents < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsDropEvents, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        SHR_RANGE_VERIFY(3, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_rx_stats_drop_events_sch_low;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_rx_stats_drop_events_sch_high;
        data->counters_to_add_array[2] = dnx_mib_counter_nif_rx_stats_drop_events_sch_tdm;
    }
    if (snmpEtherStatsOctets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsOctets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        SHR_RANGE_VERIFY(3, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rbyt;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_tbyt;
        data->counters_to_add_array[2] = dnx_mib_counter_nif_eth_rrbyt;
    }
    if (snmpEtherStatsPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        SHR_RANGE_VERIFY(4, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpkt;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_rflr;
        data->counters_to_add_array[2] = dnx_mib_counter_nif_eth_tpkt;
        data->counters_to_add_array[3] = dnx_mib_counter_nif_eth_rrpkt;
    }
    if (snmpEtherStatsBroadcastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsBroadcastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rbca;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_tbca;
    }
    if (snmpEtherStatsMulticastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsMulticastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rmca;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_tmca;
    }
    if (snmpEtherStatsCRCAlignErrors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsCRCAlignErrors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rfcs;
    }
    if (snmpEtherStatsTxCRCAlignErrors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsTxCRCAlignErrors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tfcs;
    }
    if (snmpEtherStatsUndersizePkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsUndersizePkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rund;
    }
    if (snmpEtherStatsOversizePkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsOversizePkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rovr;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_tovr;
    }
    if (snmpEtherRxOversizePkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherRxOversizePkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rovr;
    }
    if (snmpEtherTxOversizePkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherTxOversizePkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tovr;
    }
    if (snmpEtherStatsFragments < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsFragments, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rfrg;
    }
    if (snmpEtherStatsJabbers < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsJabbers, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rjbr;
    }
    if (snmpEtherStatsTxJabbers < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsTxJabbers, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tjbr;
    }
    if (snmpEtherStatsPkts64Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsPkts64Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r64;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_t64;
    }
    if (snmpEtherStatsPkts65to127Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsPkts65to127Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r127;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_t127;
    }
    if (snmpEtherStatsPkts128to255Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsPkts128to255Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r255;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_t255;
    }
    if (snmpEtherStatsPkts256to511Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsPkts256to511Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r511;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_t511;
    }
    if (snmpEtherStatsPkts512to1023Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsPkts512to1023Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r1023;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_t1023;
    }
    if (snmpEtherStatsPkts1024to1518Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsPkts1024to1518Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r1518;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_t1518;
    }
    if (snmpBcmEtherStatsPkts1519to1522Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmEtherStatsPkts1519to1522Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rmgv;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_tmgv;
    }
    if (snmpBcmEtherStatsPkts1522to2047Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmEtherStatsPkts1522to2047Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r2047;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_t2047;
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_subtract_array[0] = dnx_mib_counter_nif_eth_rmgv;
        data->counters_to_subtract_array[1] = dnx_mib_counter_nif_eth_tmgv;
    }
    if (snmpBcmEtherStatsPkts2048to4095Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmEtherStatsPkts2048to4095Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r4095;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_t4095;
    }
    if (snmpBcmEtherStatsPkts4095to9216Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmEtherStatsPkts4095to9216Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r9216;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_t9216;
    }
    if (snmpBcmEtherStatsPkts9217to16383Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmEtherStatsPkts9217to16383Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r16383;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_t16383;
    }
    if (snmpBcmReceivedPkts64Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmReceivedPkts64Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r64;
    }
    if (snmpBcmReceivedPkts65to127Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmReceivedPkts65to127Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r127;
    }
    if (snmpBcmReceivedPkts128to255Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmReceivedPkts128to255Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r255;
    }
    if (snmpBcmReceivedPkts256to511Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmReceivedPkts256to511Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r511;
    }
    if (snmpBcmReceivedPkts512to1023Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmReceivedPkts512to1023Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r1023;
    }
    if (snmpBcmReceivedPkts1024to1518Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmReceivedPkts1024to1518Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r1518;
    }
    if (snmpBcmReceivedPkts1519to2047Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmReceivedPkts1519to2047Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r2047;
    }
    if (snmpBcmReceivedPkts2048to4095Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmReceivedPkts2048to4095Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r4095;
    }
    if (snmpBcmReceivedPkts4095to9216Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmReceivedPkts4095to9216Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r9216;
    }
    if (snmpBcmReceivedPkts9217to16383Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmReceivedPkts9217to16383Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r16383;
    }
    if (snmpBcmTransmittedPkts64Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTransmittedPkts64Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_t64;
    }
    if (snmpBcmTransmittedPkts65to127Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTransmittedPkts65to127Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_t127;
    }
    if (snmpBcmTransmittedPkts128to255Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTransmittedPkts128to255Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_t255;
    }
    if (snmpBcmTransmittedPkts256to511Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTransmittedPkts256to511Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_t511;
    }
    if (snmpBcmTransmittedPkts512to1023Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTransmittedPkts512to1023Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_t1023;
    }
    if (snmpBcmTransmittedPkts1024to1518Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTransmittedPkts1024to1518Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_t1518;
    }
    if (snmpBcmTransmittedPkts1519to2047Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTransmittedPkts1519to2047Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_t2047;
    }
    if (snmpBcmTransmittedPkts2048to4095Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTransmittedPkts2048to4095Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_t4095;
    }
    if (snmpBcmTransmittedPkts4095to9216Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTransmittedPkts4095to9216Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_t9216;
    }
    if (snmpBcmTransmittedPkts9217to16383Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTransmittedPkts9217to16383Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_t16383;
    }
    if (snmpEtherStatsTXNoErrors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsTXNoErrors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tpok;
    }
    if (snmpEtherStatsTXNoErrors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsTXNoErrors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN);
        data->counters_to_add_array[0] = dnx_mib_counter_ilkn_tx_pkt;
    }
    if (snmpEtherStatsRXNoErrors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsRXNoErrors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpok;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_rflr;
    }
    if (snmpEtherStatsRXNoErrors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsRXNoErrors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN);
        data->counters_to_add_array[0] = dnx_mib_counter_ilkn_rx_pkt;
    }
    if (snmpEtherStatsDropEvents < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsDropEvents, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        SHR_RANGE_VERIFY(3, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_rx_stats_drop_events_sch_low;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_rx_stats_drop_events_sch_high;
        data->counters_to_add_array[2] = dnx_mib_counter_nif_rx_stats_drop_events_sch_tdm;
    }
    if (snmpEtherStatsOctets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsOctets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        SHR_RANGE_VERIFY(3, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rbyt;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_tbyt;
        data->counters_to_add_array[2] = dnx_mib_counter_nif_eth_rrbyt;
    }
    if (snmpEtherStatsPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        SHR_RANGE_VERIFY(4, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpkt;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_rflr;
        data->counters_to_add_array[2] = dnx_mib_counter_nif_eth_tpkt;
        data->counters_to_add_array[3] = dnx_mib_counter_nif_eth_rrpkt;
    }
    if (snmpEtherStatsBroadcastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsBroadcastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rbca;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_tbca;
    }
    if (snmpEtherStatsMulticastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsMulticastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rmca;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_tmca;
    }
    if (snmpEtherStatsCRCAlignErrors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsCRCAlignErrors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rfcs;
    }
    if (snmpEtherStatsTxCRCAlignErrors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsTxCRCAlignErrors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tfcs;
    }
    if (snmpEtherStatsUndersizePkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsUndersizePkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rund;
    }
    if (snmpEtherStatsOversizePkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsOversizePkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rovr;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_tovr;
    }
    if (snmpEtherRxOversizePkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherRxOversizePkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rovr;
    }
    if (snmpEtherTxOversizePkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherTxOversizePkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tovr;
    }
    if (snmpEtherStatsFragments < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsFragments, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rfrg;
    }
    if (snmpEtherStatsJabbers < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsJabbers, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rjbr;
    }
    if (snmpEtherStatsTxJabbers < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsTxJabbers, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tjbr;
    }
    if (snmpEtherStatsPkts64Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsPkts64Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r64;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_t64;
    }
    if (snmpEtherStatsPkts65to127Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsPkts65to127Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r127;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_t127;
    }
    if (snmpEtherStatsPkts128to255Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsPkts128to255Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r255;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_t255;
    }
    if (snmpEtherStatsPkts256to511Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsPkts256to511Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r511;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_t511;
    }
    if (snmpEtherStatsPkts512to1023Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsPkts512to1023Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r1023;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_t1023;
    }
    if (snmpEtherStatsPkts1024to1518Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsPkts1024to1518Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r1518;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_t1518;
    }
    if (snmpBcmEtherStatsPkts1519to1522Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmEtherStatsPkts1519to1522Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rmgv;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_tmgv;
    }
    if (snmpBcmEtherStatsPkts1522to2047Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmEtherStatsPkts1522to2047Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r2047;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_t2047;
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_subtract_array[0] = dnx_mib_counter_nif_eth_rmgv;
        data->counters_to_subtract_array[1] = dnx_mib_counter_nif_eth_tmgv;
    }
    if (snmpBcmEtherStatsPkts2048to4095Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmEtherStatsPkts2048to4095Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r4095;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_t4095;
    }
    if (snmpBcmEtherStatsPkts4095to9216Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmEtherStatsPkts4095to9216Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r9216;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_t9216;
    }
    if (snmpBcmEtherStatsPkts9217to16383Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmEtherStatsPkts9217to16383Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r16383;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_t16383;
    }
    if (snmpBcmReceivedPkts64Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmReceivedPkts64Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r64;
    }
    if (snmpBcmReceivedPkts65to127Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmReceivedPkts65to127Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r127;
    }
    if (snmpBcmReceivedPkts128to255Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmReceivedPkts128to255Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r255;
    }
    if (snmpBcmReceivedPkts256to511Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmReceivedPkts256to511Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r511;
    }
    if (snmpBcmReceivedPkts512to1023Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmReceivedPkts512to1023Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r1023;
    }
    if (snmpBcmReceivedPkts1024to1518Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmReceivedPkts1024to1518Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r1518;
    }
    if (snmpBcmReceivedPkts1519to2047Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmReceivedPkts1519to2047Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r2047;
    }
    if (snmpBcmReceivedPkts2048to4095Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmReceivedPkts2048to4095Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r4095;
    }
    if (snmpBcmReceivedPkts4095to9216Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmReceivedPkts4095to9216Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r9216;
    }
    if (snmpBcmReceivedPkts9217to16383Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmReceivedPkts9217to16383Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_r16383;
    }
    if (snmpBcmTransmittedPkts64Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTransmittedPkts64Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_t64;
    }
    if (snmpBcmTransmittedPkts65to127Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTransmittedPkts65to127Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_t127;
    }
    if (snmpBcmTransmittedPkts128to255Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTransmittedPkts128to255Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_t255;
    }
    if (snmpBcmTransmittedPkts256to511Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTransmittedPkts256to511Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_t511;
    }
    if (snmpBcmTransmittedPkts512to1023Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTransmittedPkts512to1023Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_t1023;
    }
    if (snmpBcmTransmittedPkts1024to1518Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTransmittedPkts1024to1518Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_t1518;
    }
    if (snmpBcmTransmittedPkts1519to2047Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTransmittedPkts1519to2047Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_t2047;
    }
    if (snmpBcmTransmittedPkts2048to4095Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTransmittedPkts2048to4095Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_t4095;
    }
    if (snmpBcmTransmittedPkts4095to9216Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTransmittedPkts4095to9216Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_t9216;
    }
    if (snmpBcmTransmittedPkts9217to16383Octets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTransmittedPkts9217to16383Octets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_t16383;
    }
    if (snmpEtherStatsTXNoErrors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsTXNoErrors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tpok;
    }
    if (snmpEtherStatsRXNoErrors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpEtherStatsRXNoErrors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpok;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_rflr;
    }
    if (snmpDot3StatsFCSErrors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpDot3StatsFCSErrors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rfcs;
    }
    if (snmpDot3StatsInternalMacTransmitErrors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpDot3StatsInternalMacTransmitErrors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tufl;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_terr;
    }
    if (snmpDot3StatsFrameTooLongs < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpDot3StatsFrameTooLongs, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rmtue;
    }
    if (snmpDot3StatsInternalMacReceiveErrors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpDot3StatsInternalMacReceiveErrors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rrpkt;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_rerpkt;
    }
    if (snmpDot3StatsSymbolErrors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpDot3StatsSymbolErrors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rerpkt;
    }
    if (snmpDot3ControlInUnknownOpcodes < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpDot3ControlInUnknownOpcodes, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rxuo;
    }
    if (snmpDot3InPauseFrames < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpDot3InPauseFrames, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rxpf;
    }
    if (snmpDot3OutPauseFrames < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpDot3OutPauseFrames, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_txpf;
    }
    if (snmpIfHCInOctets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfHCInOctets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rbyt;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_rrbyt;
    }
    if (snmpIfHCInUcastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfHCInUcastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_ruca;
    }
    if (snmpIfHCInMulticastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfHCInMulticastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rmca;
    }
    if (snmpIfHCInBroadcastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfHCInBroadcastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rbca;
    }
    if (snmpIfHCOutOctets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfHCOutOctets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tbyt;
    }
    if (snmpIfHCOutUcastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfHCOutUcastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tuca;
    }
    if (snmpIfHCOutMulticastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfHCOutMulticastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tmca;
    }
    if (snmpIfHCOutBroadcastPckts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfHCOutBroadcastPckts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tbca;
    }
    if (snmpDot3StatsFCSErrors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpDot3StatsFCSErrors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rfcs;
    }
    if (snmpDot3StatsInternalMacTransmitErrors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpDot3StatsInternalMacTransmitErrors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tufl;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_terr;
    }
    if (snmpDot3StatsFrameTooLongs < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpDot3StatsFrameTooLongs, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rmtue;
    }
    if (snmpDot3StatsInternalMacReceiveErrors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpDot3StatsInternalMacReceiveErrors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rrpkt;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_rerpkt;
    }
    if (snmpDot3StatsSymbolErrors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpDot3StatsSymbolErrors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rerpkt;
    }
    if (snmpDot3ControlInUnknownOpcodes < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpDot3ControlInUnknownOpcodes, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rxuo;
    }
    if (snmpDot3InPauseFrames < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpDot3InPauseFrames, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rxpf;
    }
    if (snmpDot3OutPauseFrames < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpDot3OutPauseFrames, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_txpf;
    }
    if (snmpIfHCInOctets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfHCInOctets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        SHR_RANGE_VERIFY(2, 0, DNX_MIB_MAX_COUNTER_BUNDLE, _SHR_E_INTERNAL, "out of bound access to array")
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rbyt;
        data->counters_to_add_array[1] = dnx_mib_counter_nif_eth_rrbyt;
    }
    if (snmpIfHCInUcastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfHCInUcastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_ruca;
    }
    if (snmpIfHCInMulticastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfHCInMulticastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rmca;
    }
    if (snmpIfHCInBroadcastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfHCInBroadcastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rbca;
    }
    if (snmpIfHCOutOctets < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfHCOutOctets, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tbyt;
    }
    if (snmpIfHCOutUcastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfHCOutUcastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tuca;
    }
    if (snmpIfHCOutMulticastPkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfHCOutMulticastPkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tmca;
    }
    if (snmpIfHCOutBroadcastPckts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIfHCOutBroadcastPckts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tbca;
    }
    if (snmpIeee8021PfcRequests < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIeee8021PfcRequests, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_txpp;
    }
    if (snmpIeee8021PfcIndications < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIeee8021PfcIndications, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rxpp;
    }
    if (snmpBcmReceivedUndersizePkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmReceivedUndersizePkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rrpkt;
    }
    if (snmpBcmTransmittedUndersizePkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTransmittedUndersizePkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_trpkt;
    }
    if (snmpIeee8021PfcRequests < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIeee8021PfcRequests, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_txpp;
    }
    if (snmpIeee8021PfcIndications < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpIeee8021PfcIndications, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rxpp;
    }
    if (snmpBcmReceivedUndersizePkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmReceivedUndersizePkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rrpkt;
    }
    if (snmpBcmTransmittedUndersizePkts < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTransmittedUndersizePkts, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_trpkt;
    }
    if (snmpBcmRxPFCFramePriority0 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFramePriority0, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfc0;
    }
    if (snmpBcmRxPFCFramePriority1 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFramePriority1, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfc1;
    }
    if (snmpBcmRxPFCFramePriority2 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFramePriority2, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfc2;
    }
    if (snmpBcmRxPFCFramePriority3 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFramePriority3, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfc3;
    }
    if (snmpBcmRxPFCFramePriority4 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFramePriority4, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfc4;
    }
    if (snmpBcmRxPFCFramePriority5 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFramePriority5, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfc5;
    }
    if (snmpBcmRxPFCFramePriority6 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFramePriority6, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfc6;
    }
    if (snmpBcmRxPFCFramePriority7 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFramePriority7, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfc7;
    }
    if (snmpBcmTxPFCFramePriority0 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTxPFCFramePriority0, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tpfc0;
    }
    if (snmpBcmTxPFCFramePriority1 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTxPFCFramePriority1, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tpfc1;
    }
    if (snmpBcmTxPFCFramePriority2 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTxPFCFramePriority2, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tpfc2;
    }
    if (snmpBcmTxPFCFramePriority3 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTxPFCFramePriority3, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tpfc3;
    }
    if (snmpBcmTxPFCFramePriority4 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTxPFCFramePriority4, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tpfc4;
    }
    if (snmpBcmTxPFCFramePriority5 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTxPFCFramePriority5, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tpfc5;
    }
    if (snmpBcmTxPFCFramePriority6 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTxPFCFramePriority6, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tpfc6;
    }
    if (snmpBcmTxPFCFramePriority7 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTxPFCFramePriority7, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tpfc7;
    }
    if (snmpBcmRxPFCFrameXonPriority0 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFrameXonPriority0, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfcoff0;
    }
    if (snmpBcmRxPFCFrameXonPriority1 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFrameXonPriority1, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfcoff1;
    }
    if (snmpBcmRxPFCFrameXonPriority2 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFrameXonPriority2, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfcoff2;
    }
    if (snmpBcmRxPFCFrameXonPriority3 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFrameXonPriority3, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfcoff3;
    }
    if (snmpBcmRxPFCFrameXonPriority4 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFrameXonPriority4, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfcoff4;
    }
    if (snmpBcmRxPFCFrameXonPriority5 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFrameXonPriority5, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfcoff5;
    }
    if (snmpBcmRxPFCFrameXonPriority6 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFrameXonPriority6, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfcoff6;
    }
    if (snmpBcmRxPFCFrameXonPriority7 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFrameXonPriority7, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfcoff7;
    }
    if (snmpBcmRxPFCFramePriority0 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFramePriority0, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfc0;
    }
    if (snmpBcmRxPFCFramePriority1 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFramePriority1, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfc1;
    }
    if (snmpBcmRxPFCFramePriority2 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFramePriority2, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfc2;
    }
    if (snmpBcmRxPFCFramePriority3 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFramePriority3, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfc3;
    }
    if (snmpBcmRxPFCFramePriority4 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFramePriority4, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfc4;
    }
    if (snmpBcmRxPFCFramePriority5 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFramePriority5, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfc5;
    }
    if (snmpBcmRxPFCFramePriority6 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFramePriority6, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfc6;
    }
    if (snmpBcmRxPFCFramePriority7 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFramePriority7, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfc7;
    }
    if (snmpBcmTxPFCFramePriority0 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTxPFCFramePriority0, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tpfc0;
    }
    if (snmpBcmTxPFCFramePriority1 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTxPFCFramePriority1, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tpfc1;
    }
    if (snmpBcmTxPFCFramePriority2 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTxPFCFramePriority2, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tpfc2;
    }
    if (snmpBcmTxPFCFramePriority3 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTxPFCFramePriority3, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tpfc3;
    }
    if (snmpBcmTxPFCFramePriority4 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTxPFCFramePriority4, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tpfc4;
    }
    if (snmpBcmTxPFCFramePriority5 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTxPFCFramePriority5, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tpfc5;
    }
    if (snmpBcmTxPFCFramePriority6 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTxPFCFramePriority6, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tpfc6;
    }
    if (snmpBcmTxPFCFramePriority7 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmTxPFCFramePriority7, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_tpfc7;
    }
    if (snmpBcmRxPFCFrameXonPriority0 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFrameXonPriority0, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfcoff0;
    }
    if (snmpBcmRxPFCFrameXonPriority1 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFrameXonPriority1, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfcoff1;
    }
    if (snmpBcmRxPFCFrameXonPriority2 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFrameXonPriority2, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfcoff2;
    }
    if (snmpBcmRxPFCFrameXonPriority3 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFrameXonPriority3, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfcoff3;
    }
    if (snmpBcmRxPFCFrameXonPriority4 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFrameXonPriority4, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfcoff4;
    }
    if (snmpBcmRxPFCFrameXonPriority5 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFrameXonPriority5, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfcoff5;
    }
    if (snmpBcmRxPFCFrameXonPriority6 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFrameXonPriority6, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfcoff6;
    }
    if (snmpBcmRxPFCFrameXonPriority7 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_get(unit, table, snmpBcmRxPFCFrameXonPriority7, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->counters_to_add_array[0] = dnx_mib_counter_nif_eth_rpfcoff7;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mib_controlled_counters_set_data_set(
    int unit)
{
    int counter_id_index;
    int set_type_index;
    dnx_data_mib_controlled_counters_set_data_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mib;
    int submodule_index = dnx_data_mib_submodule_controlled_counters;
    int table_index = dnx_data_mib_controlled_counters_table_set_data;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_mib_counter_nof;
    table->info_get.key_size[0] = dnx_mib_counter_nof;
    table->keys[1].size = DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NOF;
    table->info_get.key_size[1] = DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NOF;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "NULL";
    table->values[2].default_val = "NULL";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mib_controlled_counters_set_data_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_mib_controlled_counters_table_set_data");

    
    default_data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->hw_counter_id = -1;
    default_data->get_cb = NULL;
    default_data->clear_cb = NULL;
    
    for (counter_id_index = 0; counter_id_index < table->keys[0].size; counter_id_index++)
    {
        for (set_type_index = 0; set_type_index < table->keys[1].size; set_type_index++)
        {
            data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, counter_id_index, set_type_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (dnx_mib_counter_tx_control_cells < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_tx_control_cells, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->hw_counter_id = 0;
        data->get_cb = dnx_mib_fmac_counter_get;
        data->clear_cb = dnx_mib_fmac_counter_clear;
    }
    if (dnx_mib_counter_tx_data_cells < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_tx_data_cells, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->hw_counter_id = 1;
        data->get_cb = dnx_mib_fmac_counter_get;
        data->clear_cb = dnx_mib_fmac_counter_clear;
    }
    if (dnx_mib_counter_tx_data_bytes < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_tx_data_bytes, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->hw_counter_id = 2;
        data->get_cb = dnx_mib_fmac_counter_get;
        data->clear_cb = dnx_mib_fmac_counter_clear;
    }
    if (dnx_mib_counter_rx_crc_errors_data_cells < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_crc_errors_data_cells, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->hw_counter_id = 3;
        data->get_cb = dnx_mib_fmac_counter_get;
        data->clear_cb = dnx_mib_fmac_counter_clear;
    }
    if (dnx_mib_counter_rx_crc_errors_control_cells_nonbypass < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_crc_errors_control_cells_nonbypass, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->hw_counter_id = 4;
        data->get_cb = dnx_mib_fmac_counter_get;
        data->clear_cb = dnx_mib_fmac_counter_clear;
    }
    if (dnx_mib_counter_rx_crc_errors_control_cells_bypass < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_crc_errors_control_cells_bypass, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->hw_counter_id = 5;
        data->get_cb = dnx_mib_fmac_counter_get;
        data->clear_cb = dnx_mib_fmac_counter_clear;
    }
    if (dnx_mib_counter_rx_fec_correctable_error < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_fec_correctable_error, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->hw_counter_id = 6;
        data->get_cb = dnx_mib_fmac_counter_get;
        data->clear_cb = dnx_mib_fmac_counter_clear;
    }
    if (dnx_mib_counter_rx_control_cells < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_control_cells, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->hw_counter_id = 7;
        data->get_cb = dnx_mib_fmac_counter_get;
        data->clear_cb = dnx_mib_fmac_counter_clear;
    }
    if (dnx_mib_counter_rx_data_cells < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_data_cells, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->hw_counter_id = 8;
        data->get_cb = dnx_mib_fmac_counter_get;
        data->clear_cb = dnx_mib_fmac_counter_clear;
    }
    if (dnx_mib_counter_rx_data_bytes < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_data_bytes, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->hw_counter_id = 9;
        data->get_cb = dnx_mib_fmac_counter_get;
        data->clear_cb = dnx_mib_fmac_counter_clear;
    }
    if (dnx_mib_counter_rx_dropped_retransmitted_control < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_dropped_retransmitted_control, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->hw_counter_id = 10;
        data->get_cb = dnx_mib_fmac_counter_get;
        data->clear_cb = dnx_mib_fmac_counter_clear;
    }
    if (dnx_mib_counter_tx_asyn_fifo_rate < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_tx_asyn_fifo_rate, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->hw_counter_id = 13;
        data->get_cb = dnx_mib_fmac_counter_get;
        data->clear_cb = dnx_mib_fmac_counter_clear;
    }
    if (dnx_mib_counter_rx_asyn_fifo_rate < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_asyn_fifo_rate, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->hw_counter_id = 14;
        data->get_cb = dnx_mib_fmac_counter_get;
        data->clear_cb = dnx_mib_fmac_counter_clear;
    }
    if (dnx_mib_counter_tx_asyn_fifo_rate < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN_OVER_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_tx_asyn_fifo_rate, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN_OVER_FABRIC);
        data->hw_counter_id = 13;
        data->get_cb = dnx_mib_fmac_counter_get;
        data->clear_cb = dnx_mib_fmac_counter_clear;
    }
    if (dnx_mib_counter_rx_asyn_fifo_rate < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN_OVER_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_asyn_fifo_rate, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN_OVER_FABRIC);
        data->hw_counter_id = 14;
        data->get_cb = dnx_mib_fmac_counter_get;
        data->clear_cb = dnx_mib_fmac_counter_clear;
    }
    if (dnx_mib_counter_rx_lfec_fec_uncorrrectable_errors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_lfec_fec_uncorrrectable_errors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->hw_counter_id = 15;
        data->get_cb = dnx_mib_fmac_counter_get;
        data->clear_cb = dnx_mib_fmac_counter_clear;
    }
    if (dnx_mib_counter_rx_llfc_primary_pipe < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_llfc_primary_pipe, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->hw_counter_id = 16;
        data->get_cb = dnx_mib_fmac_counter_get;
        data->clear_cb = dnx_mib_fmac_counter_clear;
    }
    if (dnx_mib_counter_rx_llfc_second_pipe < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_llfc_second_pipe, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->hw_counter_id = 17;
        data->get_cb = dnx_mib_fmac_counter_get;
        data->clear_cb = dnx_mib_fmac_counter_clear;
    }
    if (dnx_mib_counter_rx_llfc_third_pipe < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_llfc_third_pipe, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->hw_counter_id = 18;
        data->get_cb = dnx_mib_fmac_counter_get;
        data->clear_cb = dnx_mib_fmac_counter_clear;
    }
    if (dnx_mib_counter_rx_kpcs_errors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_kpcs_errors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->hw_counter_id = 19;
        data->get_cb = dnx_mib_fmac_counter_get;
        data->clear_cb = dnx_mib_fmac_counter_clear;
    }
    if (dnx_mib_counter_rx_kpcs_bypass_errors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_kpcs_bypass_errors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->hw_counter_id = 20;
        data->get_cb = dnx_mib_fmac_counter_get;
        data->clear_cb = dnx_mib_fmac_counter_clear;
    }
    if (dnx_mib_counter_rx_rs_fec_bit_error < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_rs_fec_bit_error, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->hw_counter_id = 21;
        data->get_cb = dnx_mib_fmac_counter_get;
        data->clear_cb = dnx_mib_fmac_counter_clear;
    }
    if (dnx_mib_counter_rx_rs_fec_symbol_error_rate < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_rs_fec_symbol_error_rate, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC);
        data->hw_counter_id = 22;
        data->get_cb = dnx_mib_fmac_counter_get;
        data->clear_cb = dnx_mib_fmac_counter_clear;
    }
    if (dnx_mib_counter_ilkn_rx_pkt < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_ilkn_rx_pkt, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN);
        data->get_cb = dnx_mib_ilkn_counter_get;
        data->clear_cb = dnx_mib_ilkn_counter_clear;
    }
    if (dnx_mib_counter_ilkn_tx_pkt < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_ilkn_tx_pkt, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN);
        data->get_cb = dnx_mib_ilkn_counter_get;
        data->clear_cb = dnx_mib_ilkn_counter_clear;
    }
    if (dnx_mib_counter_ilkn_rx_bytes < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_ilkn_rx_bytes, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN);
        data->get_cb = dnx_mib_ilkn_counter_get;
        data->clear_cb = dnx_mib_ilkn_counter_clear;
    }
    if (dnx_mib_counter_ilkn_tx_bytes < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_ilkn_tx_bytes, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN);
        data->get_cb = dnx_mib_ilkn_counter_get;
        data->clear_cb = dnx_mib_ilkn_counter_clear;
    }
    if (dnx_mib_counter_ilkn_rx_err_pkt < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_ilkn_rx_err_pkt, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN);
        data->get_cb = dnx_mib_ilkn_counter_get;
        data->clear_cb = dnx_mib_ilkn_counter_clear;
    }
    if (dnx_mib_counter_ilkn_tx_err_pkt < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_ilkn_tx_err_pkt, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN);
        data->get_cb = dnx_mib_ilkn_counter_get;
        data->clear_cb = dnx_mib_ilkn_counter_clear;
    }
    if (dnx_mib_counter_nif_rx_fec_correctable_errors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_rx_fec_correctable_errors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF);
        data->get_cb = dnx_mib_nif_counter_get;
        data->clear_cb = dnx_mib_nif_counter_clear;
    }
    if (dnx_mib_counter_nif_rx_fec_uncorrectable_errors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_rx_fec_uncorrectable_errors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF);
        data->get_cb = dnx_mib_nif_counter_get;
        data->clear_cb = dnx_mib_nif_counter_clear;
    }
    if (dnx_mib_counter_nif_rx_bip_errors < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_rx_bip_errors, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF);
        data->get_cb = dnx_mib_nif_counter_get;
        data->clear_cb = dnx_mib_nif_counter_clear;
    }
    if (dnx_mib_counter_nif_rx_stats_drop_events_sch_low < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_rx_stats_drop_events_sch_low, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF);
        data->get_cb = dnx_mib_nif_counter_get;
        data->clear_cb = dnx_mib_nif_counter_clear;
    }
    if (dnx_mib_counter_nif_rx_stats_drop_events_sch_high < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_rx_stats_drop_events_sch_high, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF);
        data->get_cb = dnx_mib_nif_counter_get;
        data->clear_cb = dnx_mib_nif_counter_clear;
    }
    if (dnx_mib_counter_nif_rx_stats_drop_events_sch_tdm < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_rx_stats_drop_events_sch_tdm, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF);
        data->get_cb = dnx_mib_nif_counter_get;
        data->clear_cb = dnx_mib_nif_counter_clear;
    }
    if (dnx_mib_counter_nif_rx_stats_drop_events_sch_low < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_rx_stats_drop_events_sch_low, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_nif_counter_get;
        data->clear_cb = dnx_mib_nif_counter_clear;
    }
    if (dnx_mib_counter_nif_rx_stats_drop_events_sch_high < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_rx_stats_drop_events_sch_high, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_nif_counter_get;
        data->clear_cb = dnx_mib_nif_counter_clear;
    }
    if (dnx_mib_counter_nif_rx_stats_drop_events_sch_tdm < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_rx_stats_drop_events_sch_tdm, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_nif_counter_get;
        data->clear_cb = dnx_mib_nif_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_r64 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r64, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_r127 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r127, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_r255 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r255, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_r511 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r511, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_r1023 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r1023, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_r1518 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r1518, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rmgv < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rmgv, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_r2047 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r2047, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_r4095 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r4095, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_r9216 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r9216, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_r16383 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r16383, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rbca < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rbca, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rprog0 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rprog0, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rprog1 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rprog1, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rprog2 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rprog2, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rprog3 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rprog3, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpkt < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpkt, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpok < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpok, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_ruca < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_ruca, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rmca < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rmca, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rxpf < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rxpf, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rxpp < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rxpp, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rxcf < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rxcf, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rfcs < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rfcs, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rerpkt < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rerpkt, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rflr < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rflr, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rjbr < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rjbr, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rmtue < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rmtue, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rovr < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rovr, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rvln < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rvln, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rdvln < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rdvln, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rxuo < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rxuo, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rxuda < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rxuda, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rxwsa < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rxwsa, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rprm < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rprm, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfc0 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc0, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfcoff0 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff0, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfc1 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc1, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfcoff1 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff1, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfc2 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc2, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfcoff2 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff2, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfc3 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc3, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfcoff3 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff3, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfc4 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc4, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfcoff4 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff4, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfc5 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc5, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfcoff5 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff5, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfc6 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc6, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfcoff6 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff6, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfc7 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc7, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfcoff7 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff7, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rund < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rund, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rfrg < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rfrg, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rrpkt < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rrpkt, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_t64 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t64, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_t127 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t127, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_t255 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t255, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_t511 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t511, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_t1023 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t1023, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_t1518 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t1518, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tmgv < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tmgv, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_t2047 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t2047, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_t4095 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t4095, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_t9216 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t9216, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_t16383 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t16383, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tbca < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tbca, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfc0 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc0, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfcoff0 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff0, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfc1 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc1, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfcoff1 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff1, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfc2 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc2, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfcoff2 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff2, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfc3 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc3, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfcoff3 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff3, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfc4 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc4, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfcoff4 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff4, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfc5 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc5, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfcoff5 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff5, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfc6 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc6, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfcoff6 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff6, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfc7 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc7, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfcoff7 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff7, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpkt < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpkt, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpok < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpok, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tuca < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tuca, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tufl < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tufl, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tmca < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tmca, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_txpf < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_txpf, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_txpp < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_txpp, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_txcf < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_txcf, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tfcs < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tfcs, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_terr < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_terr, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tovr < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tovr, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tjbr < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tjbr, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_trpkt < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_trpkt, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tfrg < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tfrg, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tvln < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tvln, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tdvln < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tdvln, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rbyt < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rbyt, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rrbyt < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rrbyt, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tbyt < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tbyt, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rverify < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rverify, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rreply < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rreply, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rtrfu < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rtrfu, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rschcrc < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rschcrc, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rx_eee_lpi_event < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rx_eee_lpi_event, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rx_eee_lpi_duration < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rx_eee_lpi_duration, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rx_llfc_phy < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rx_llfc_phy, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rx_llfc_log < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rx_llfc_log, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rx_llfc_crc < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rx_llfc_crc, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rx_hcfc < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rx_hcfc, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rx_hcfc_crc < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rx_hcfc_crc, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tverify < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tverify, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_treply < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_treply, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tscl < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tscl, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tmcl < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tmcl, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tlcl < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tlcl, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_txcl < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_txcl, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tx_eee_lpi_event < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tx_eee_lpi_event, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tx_eee_lpi_duration < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tx_eee_lpi_duration, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tx_llfc_log < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tx_llfc_log, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tx_hcfc < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tx_hcfc, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tncl < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tncl, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_xthol < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_xthol, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tspare0 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tspare0, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tspare1 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tspare1, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tspare2 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tspare2, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tspare3 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tspare3, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU);
        data->get_cb = dnx_mib_nif_eth_counter_get;
        data->clear_cb = dnx_mib_nif_eth_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_r64 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r64, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_r127 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r127, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_r255 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r255, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_r511 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r511, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_r1023 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r1023, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_r1518 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r1518, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rmgv < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rmgv, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_r2047 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r2047, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_r4095 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r4095, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_r9216 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r9216, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_r16383 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r16383, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rbca < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rbca, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rprog0 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rprog0, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rprog1 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rprog1, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rprog2 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rprog2, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rprog3 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rprog3, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpkt < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpkt, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpok < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpok, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_ruca < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_ruca, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rmca < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rmca, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rxpf < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rxpf, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rxpp < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rxpp, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rxcf < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rxcf, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rfcs < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rfcs, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rerpkt < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rerpkt, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rflr < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rflr, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rjbr < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rjbr, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rmtue < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rmtue, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rovr < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rovr, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rvln < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rvln, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rdvln < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rdvln, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rxuo < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rxuo, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rxuda < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rxuda, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rxwsa < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rxwsa, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rprm < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rprm, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfc0 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc0, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfcoff0 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff0, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfc1 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc1, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfcoff1 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff1, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfc2 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc2, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfcoff2 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff2, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfc3 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc3, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfcoff3 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff3, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfc4 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc4, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfcoff4 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff4, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfc5 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc5, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfcoff5 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff5, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfc6 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc6, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfcoff6 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff6, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfc7 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc7, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rpfcoff7 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff7, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rund < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rund, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rfrg < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rfrg, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rrpkt < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rrpkt, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_t64 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t64, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_t127 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t127, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_t255 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t255, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_t511 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t511, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_t1023 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t1023, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_t1518 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t1518, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tmgv < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tmgv, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_t2047 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t2047, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_t4095 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t4095, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_t9216 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t9216, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_t16383 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t16383, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tbca < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tbca, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfc0 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc0, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfcoff0 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff0, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfc1 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc1, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfcoff1 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff1, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfc2 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc2, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfcoff2 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff2, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfc3 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc3, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfcoff3 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff3, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfc4 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc4, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfcoff4 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff4, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfc5 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc5, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfcoff5 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff5, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfc6 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc6, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfcoff6 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff6, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfc7 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc7, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpfcoff7 < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff7, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpkt < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpkt, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tpok < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpok, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tuca < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tuca, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tufl < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tufl, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tmca < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tmca, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_txpf < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_txpf, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_txpp < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_txpp, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_txcf < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_txcf, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tfcs < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tfcs, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_terr < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_terr, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tovr < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tovr, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tjbr < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tjbr, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_trpkt < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_trpkt, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tfrg < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tfrg, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tvln < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tvln, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tdvln < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tdvln, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rbyt < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rbyt, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_rrbyt < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rrbyt, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }
    if (dnx_mib_counter_nif_eth_tbyt < table->keys[0].size && DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC < table->keys[1].size)
    {
        data = (dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tbyt, DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC);
        data->get_cb = dnx_mib_flexe_mac_counter_get;
        data->clear_cb = dnx_mib_flexe_mac_counter_clear;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mib_controlled_counters_counter_data_set(
    int unit)
{
    int counter_id_index;
    dnx_data_mib_controlled_counters_counter_data_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mib;
    int submodule_index = dnx_data_mib_submodule_controlled_counters;
    int table_index = dnx_data_mib_controlled_counters_table_counter_data;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_mib_counter_nof;
    table->info_get.key_size[0] = dnx_mib_counter_nof;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "-1";
    table->values[2].default_val = "-1";
    table->values[3].default_val = "N/A";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mib_controlled_counters_counter_data_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mib_controlled_counters_table_counter_data");

    
    default_data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->length = -1;
    default_data->row = -1;
    default_data->offset_in_row = -1;
    default_data->doc = "N/A";
    
    for (counter_id_index = 0; counter_id_index < table->keys[0].size; counter_id_index++)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, counter_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (dnx_mib_counter_tx_control_cells < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_tx_control_cells, 0);
        data->length = 48;
        data->doc = "TX Control cells";
    }
    if (dnx_mib_counter_tx_data_cells < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_tx_data_cells, 0);
        data->length = 48;
        data->doc = "TX Data cells";
    }
    if (dnx_mib_counter_tx_data_bytes < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_tx_data_bytes, 0);
        data->length = 48;
        data->doc = "TX Data bytes";
    }
    if (dnx_mib_counter_rx_crc_errors_data_cells < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_crc_errors_data_cells, 0);
        data->length = 32;
        data->doc = "RX CRC errors";
    }
    if (dnx_mib_counter_rx_crc_errors_control_cells_nonbypass < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_crc_errors_control_cells_nonbypass, 0);
        data->length = 16;
        data->doc = "RX CRC errors nonbypass";
    }
    if (dnx_mib_counter_rx_crc_errors_control_cells_bypass < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_crc_errors_control_cells_bypass, 0);
        data->length = 16;
        data->doc = "RX CRC errors bypass";
    }
    if (dnx_mib_counter_rx_fec_correctable_error < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_fec_correctable_error, 0);
        data->length = 32;
        data->doc = "RX FEC correctable";
    }
    if (dnx_mib_counter_rx_control_cells < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_control_cells, 0);
        data->length = 48;
        data->doc = "RX Control cells";
    }
    if (dnx_mib_counter_rx_data_cells < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_data_cells, 0);
        data->length = 48;
        data->doc = "RX Data cells";
    }
    if (dnx_mib_counter_rx_data_bytes < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_data_bytes, 0);
        data->length = 48;
        data->doc = "RX Data bytes";
    }
    if (dnx_mib_counter_rx_dropped_retransmitted_control < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_dropped_retransmitted_control, 0);
        data->length = 0;
        data->doc = "RX Dropped retransmitted control";
    }
    if (dnx_mib_counter_tx_asyn_fifo_rate < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_tx_asyn_fifo_rate, 0);
        data->length = 32;
        data->doc = "TX Asyn fifo rate";
    }
    if (dnx_mib_counter_rx_asyn_fifo_rate < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_asyn_fifo_rate, 0);
        data->length = 32;
        data->doc = "RX Asyn fifo rate";
    }
    if (dnx_mib_counter_rx_lfec_fec_uncorrrectable_errors < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_lfec_fec_uncorrrectable_errors, 0);
        data->length = 16;
        data->doc = "RX FEC uncorrectable errors";
    }
    if (dnx_mib_counter_rx_llfc_primary_pipe < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_llfc_primary_pipe, 0);
        data->length = 64;
        data->doc = "RX Llfc primary pipe";
    }
    if (dnx_mib_counter_rx_llfc_second_pipe < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_llfc_second_pipe, 0);
        data->length = 64;
        data->doc = "RX Llfc second pipe";
    }
    if (dnx_mib_counter_rx_llfc_third_pipe < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_llfc_third_pipe, 0);
        data->length = 64;
        data->doc = "RX Llfc third pipe";
    }
    if (dnx_mib_counter_rx_kpcs_errors < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_kpcs_errors, 0);
        data->length = 16;
        data->doc = "RX Kpcs errors";
    }
    if (dnx_mib_counter_rx_kpcs_bypass_errors < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_kpcs_bypass_errors, 0);
        data->length = 16;
        data->doc = "RX Kpcs bypass errors";
    }
    if (dnx_mib_counter_rx_rs_fec_bit_error < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_rs_fec_bit_error, 0);
        data->length = 48;
        data->doc = "RX RS-FEC bit error";
    }
    if (dnx_mib_counter_rx_rs_fec_symbol_error_rate < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_rx_rs_fec_symbol_error_rate, 0);
        data->length = 48;
        data->doc = "RX RS-FEC symbol error rate";
    }
    if (dnx_mib_counter_ilkn_rx_pkt < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_ilkn_rx_pkt, 0);
        data->length = 64;
        data->doc = "RX snmp stats no errors";
    }
    if (dnx_mib_counter_ilkn_tx_pkt < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_ilkn_tx_pkt, 0);
        data->length = 64;
        data->doc = "TX snmp stats no errors";
    }
    if (dnx_mib_counter_ilkn_rx_bytes < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_ilkn_rx_bytes, 0);
        data->length = 64;
        data->doc = "RX ILKN bytes counter";
    }
    if (dnx_mib_counter_ilkn_tx_bytes < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_ilkn_tx_bytes, 0);
        data->length = 64;
        data->doc = "TX ILKN bytes counter";
    }
    if (dnx_mib_counter_ilkn_rx_err_pkt < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_ilkn_rx_err_pkt, 0);
        data->length = 64;
        data->doc = "RX snmp If in err pkts";
    }
    if (dnx_mib_counter_ilkn_tx_err_pkt < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_ilkn_tx_err_pkt, 0);
        data->length = 64;
        data->doc = "RX snmp If out err pkts";
    }
    if (dnx_mib_counter_nif_rx_fec_correctable_errors < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_rx_fec_correctable_errors, 0);
        data->length = 64;
        data->doc = "RX NIF FEC correctable";
    }
    if (dnx_mib_counter_nif_rx_fec_uncorrectable_errors < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_rx_fec_uncorrectable_errors, 0);
        data->length = 64;
        data->doc = "RX NIF FEC uncorrectable";
    }
    if (dnx_mib_counter_nif_rx_bip_errors < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_rx_bip_errors, 0);
        data->length = 64;
        data->doc = "RX NIF BIP error count";
    }
    if (dnx_mib_counter_nif_rx_stats_drop_events_sch_low < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_rx_stats_drop_events_sch_low, 0);
        data->length = 64;
        data->doc = "RX nif drop low events";
    }
    if (dnx_mib_counter_nif_rx_stats_drop_events_sch_high < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_rx_stats_drop_events_sch_high, 0);
        data->length = 64;
        data->doc = "RX nif drop high events";
    }
    if (dnx_mib_counter_nif_rx_stats_drop_events_sch_tdm < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_rx_stats_drop_events_sch_tdm, 0);
        data->length = 64;
        data->doc = "RX nif drop tdm events";
    }
    if (dnx_mib_counter_nif_eth_r64 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r64, 0);
        data->length = 40;
        data->row = 0;
        data->offset_in_row = 0;
        data->doc = "RX 64-byte frame";
    }
    if (dnx_mib_counter_nif_eth_r127 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r127, 0);
        data->length = 40;
        data->row = 0;
        data->offset_in_row = 1;
        data->doc = "RX 65 to 127 byte frame";
    }
    if (dnx_mib_counter_nif_eth_r255 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r255, 0);
        data->length = 40;
        data->row = 0;
        data->offset_in_row = 2;
        data->doc = "RX 128 to 255 byte frame";
    }
    if (dnx_mib_counter_nif_eth_r511 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r511, 0);
        data->length = 40;
        data->row = 0;
        data->offset_in_row = 3;
        data->doc = "RX 256 to 511 byte frame";
    }
    if (dnx_mib_counter_nif_eth_r1023 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r1023, 0);
        data->length = 40;
        data->row = 0;
        data->offset_in_row = 4;
        data->doc = "RX 512 to 1023 byte frame";
    }
    if (dnx_mib_counter_nif_eth_r1518 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r1518, 0);
        data->length = 40;
        data->row = 0;
        data->offset_in_row = 5;
        data->doc = "RX 1024 to 1518 byte frame";
    }
    if (dnx_mib_counter_nif_eth_rmgv < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rmgv, 0);
        data->length = 40;
        data->row = 0;
        data->offset_in_row = 6;
        data->doc = "RX 1519 to 1522 byte frame";
    }
    if (dnx_mib_counter_nif_eth_r2047 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r2047, 0);
        data->length = 40;
        data->row = 0;
        data->offset_in_row = 7;
        data->doc = "RX 1519 to 2047 byte frame";
    }
    if (dnx_mib_counter_nif_eth_r4095 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r4095, 0);
        data->length = 40;
        data->row = 1;
        data->offset_in_row = 0;
        data->doc = "RX 2048 to 4095 byte frame";
    }
    if (dnx_mib_counter_nif_eth_r9216 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r9216, 0);
        data->length = 40;
        data->row = 1;
        data->offset_in_row = 1;
        data->doc = "RX 4096 to 9216 byte frame";
    }
    if (dnx_mib_counter_nif_eth_r16383 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r16383, 0);
        data->length = 40;
        data->row = 1;
        data->offset_in_row = 2;
        data->doc = "RX 9217 to 16383 byte frame";
    }
    if (dnx_mib_counter_nif_eth_rbca < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rbca, 0);
        data->length = 40;
        data->row = 1;
        data->offset_in_row = 3;
        data->doc = "RX BC frame";
    }
    if (dnx_mib_counter_nif_eth_rprog0 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rprog0, 0);
        data->length = 40;
        data->row = 1;
        data->offset_in_row = 4;
        data->doc = "RX RPROG0 frame";
    }
    if (dnx_mib_counter_nif_eth_rprog1 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rprog1, 0);
        data->length = 40;
        data->row = 1;
        data->offset_in_row = 5;
        data->doc = "RX RPROG1 frame";
    }
    if (dnx_mib_counter_nif_eth_rprog2 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rprog2, 0);
        data->length = 40;
        data->row = 1;
        data->offset_in_row = 6;
        data->doc = "RX RPROG2 frame";
    }
    if (dnx_mib_counter_nif_eth_rprog3 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rprog3, 0);
        data->length = 40;
        data->row = 1;
        data->offset_in_row = 7;
        data->doc = "RX RPROG3 frame";
    }
    if (dnx_mib_counter_nif_eth_rpkt < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpkt, 0);
        data->length = 40;
        data->row = 2;
        data->offset_in_row = 0;
        data->doc = "RX all packets frame";
    }
    if (dnx_mib_counter_nif_eth_rpok < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpok, 0);
        data->length = 40;
        data->row = 2;
        data->offset_in_row = 1;
        data->doc = "RX good frame";
    }
    if (dnx_mib_counter_nif_eth_ruca < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_ruca, 0);
        data->length = 40;
        data->row = 2;
        data->offset_in_row = 2;
        data->doc = "RX UC frame";
    }
    if (dnx_mib_counter_nif_eth_rmca < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rmca, 0);
        data->length = 40;
        data->row = 2;
        data->offset_in_row = 4;
        data->doc = "RX MC frame";
    }
    if (dnx_mib_counter_nif_eth_rxpf < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rxpf, 0);
        data->length = 40;
        data->row = 2;
        data->offset_in_row = 5;
        data->doc = "RX pause frame";
    }
    if (dnx_mib_counter_nif_eth_rxpp < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rxpp, 0);
        data->length = 40;
        data->row = 2;
        data->offset_in_row = 6;
        data->doc = "RX PFC frame";
    }
    if (dnx_mib_counter_nif_eth_rxcf < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rxcf, 0);
        data->length = 40;
        data->row = 2;
        data->offset_in_row = 7;
        data->doc = "RX control frame";
    }
    if (dnx_mib_counter_nif_eth_rfcs < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rfcs, 0);
        data->length = 40;
        data->row = 3;
        data->offset_in_row = 0;
        data->doc = "RX FCS error frame";
    }
    if (dnx_mib_counter_nif_eth_rerpkt < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rerpkt, 0);
        data->length = 40;
        data->row = 3;
        data->offset_in_row = 1;
        data->doc = "RX code error frame";
    }
    if (dnx_mib_counter_nif_eth_rflr < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rflr, 0);
        data->length = 40;
        data->row = 3;
        data->offset_in_row = 2;
        data->doc = "RX out-of-range length frame";
    }
    if (dnx_mib_counter_nif_eth_rjbr < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rjbr, 0);
        data->length = 40;
        data->row = 3;
        data->offset_in_row = 3;
        data->doc = "RX jabber frame";
    }
    if (dnx_mib_counter_nif_eth_rmtue < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rmtue, 0);
        data->length = 40;
        data->row = 3;
        data->offset_in_row = 4;
        data->doc = "RX MTU check error frame";
    }
    if (dnx_mib_counter_nif_eth_rovr < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rovr, 0);
        data->length = 40;
        data->row = 3;
        data->offset_in_row = 5;
        data->doc = "RX oversized frame";
    }
    if (dnx_mib_counter_nif_eth_rvln < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rvln, 0);
        data->length = 40;
        data->row = 3;
        data->offset_in_row = 6;
        data->doc = "RX single and double VLAN tagged frame";
    }
    if (dnx_mib_counter_nif_eth_rdvln < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rdvln, 0);
        data->length = 40;
        data->row = 3;
        data->offset_in_row = 7;
        data->doc = "RX double VLAN tagged frame";
    }
    if (dnx_mib_counter_nif_eth_rxuo < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rxuo, 0);
        data->length = 40;
        data->row = 4;
        data->offset_in_row = 0;
        data->doc = "RX unsupported opcode frame";
    }
    if (dnx_mib_counter_nif_eth_rxuda < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rxuda, 0);
        data->length = 40;
        data->row = 4;
        data->offset_in_row = 1;
        data->doc = "RX unsupported DA for pause/PFC frame";
    }
    if (dnx_mib_counter_nif_eth_rxwsa < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rxwsa, 0);
        data->length = 40;
        data->row = 4;
        data->offset_in_row = 2;
        data->doc = "RX incorrect SA frame";
    }
    if (dnx_mib_counter_nif_eth_rprm < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rprm, 0);
        data->length = 40;
        data->row = 4;
        data->offset_in_row = 3;
        data->doc = "RX promiscuous frame";
    }
    if (dnx_mib_counter_nif_eth_rpfc0 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc0, 0);
        data->length = 40;
        data->row = 4;
        data->offset_in_row = 4;
        data->doc = "RX PFC frame with enable bit set for Priority0";
    }
    if (dnx_mib_counter_nif_eth_rpfcoff0 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff0, 0);
        data->length = 40;
        data->row = 4;
        data->offset_in_row = 5;
        data->doc = "RX PFC frame transition XON to XOFF for Priority";
    }
    if (dnx_mib_counter_nif_eth_rpfc1 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc1, 0);
        data->length = 40;
        data->row = 4;
        data->offset_in_row = 6;
        data->doc = "RX PFC frame with enable bit set for Priority1";
    }
    if (dnx_mib_counter_nif_eth_rpfcoff1 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff1, 0);
        data->length = 40;
        data->row = 4;
        data->offset_in_row = 7;
        data->doc = "RX PFC frame transition XON to XOFF for Priority1";
    }
    if (dnx_mib_counter_nif_eth_rpfc2 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc2, 0);
        data->length = 40;
        data->row = 5;
        data->offset_in_row = 0;
        data->doc = "RX PFC frame with enable bit set for Priority2";
    }
    if (dnx_mib_counter_nif_eth_rpfcoff2 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff2, 0);
        data->length = 40;
        data->row = 5;
        data->offset_in_row = 1;
        data->doc = "RX PFC frame transition XON to XOFF for Priority2";
    }
    if (dnx_mib_counter_nif_eth_rpfc3 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc3, 0);
        data->length = 40;
        data->row = 5;
        data->offset_in_row = 2;
        data->doc = "RX PFC frame with enable bit set for Priority3";
    }
    if (dnx_mib_counter_nif_eth_rpfcoff3 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff3, 0);
        data->length = 40;
        data->row = 5;
        data->offset_in_row = 3;
        data->doc = "RX PFC frame transition XON to XOFF for Priority3";
    }
    if (dnx_mib_counter_nif_eth_rpfc4 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc4, 0);
        data->length = 40;
        data->row = 5;
        data->offset_in_row = 4;
        data->doc = "RX PFC frame with enable bit set for Priority4";
    }
    if (dnx_mib_counter_nif_eth_rpfcoff4 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff4, 0);
        data->length = 40;
        data->row = 5;
        data->offset_in_row = 5;
        data->doc = "RX PFC frame transition XON to XOFF for Priority4";
    }
    if (dnx_mib_counter_nif_eth_rpfc5 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc5, 0);
        data->length = 40;
        data->row = 5;
        data->offset_in_row = 6;
        data->doc = "RX PFC frame with enable bit set for Priority5";
    }
    if (dnx_mib_counter_nif_eth_rpfcoff5 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff5, 0);
        data->length = 40;
        data->row = 5;
        data->offset_in_row = 7;
        data->doc = "RX PFC frame transition XON to XOFF for Priority5";
    }
    if (dnx_mib_counter_nif_eth_rpfc6 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc6, 0);
        data->length = 40;
        data->row = 6;
        data->offset_in_row = 0;
        data->doc = "RX PFC frame with enable bit set for Priority6";
    }
    if (dnx_mib_counter_nif_eth_rpfcoff6 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff6, 0);
        data->length = 40;
        data->row = 6;
        data->offset_in_row = 1;
        data->doc = "RX PFC frame transition XON to XOFF for Priority6";
    }
    if (dnx_mib_counter_nif_eth_rpfc7 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc7, 0);
        data->length = 40;
        data->row = 6;
        data->offset_in_row = 2;
        data->doc = "RX PFC frame with enable bit set for Priority7";
    }
    if (dnx_mib_counter_nif_eth_rpfcoff7 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff7, 0);
        data->length = 40;
        data->row = 6;
        data->offset_in_row = 3;
        data->doc = "RX PFC frame transition XON to XOFF for Priority7";
    }
    if (dnx_mib_counter_nif_eth_rund < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rund, 0);
        data->length = 40;
        data->row = 6;
        data->offset_in_row = 4;
        data->doc = "RX undersized frame";
    }
    if (dnx_mib_counter_nif_eth_rfrg < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rfrg, 0);
        data->length = 40;
        data->row = 6;
        data->offset_in_row = 5;
        data->doc = "RX fragment frame";
    }
    if (dnx_mib_counter_nif_eth_rrpkt < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rrpkt, 0);
        data->length = 40;
        data->row = 6;
        data->offset_in_row = 6;
        data->doc = "RX RUNT frame";
    }
    if (dnx_mib_counter_nif_eth_t64 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t64, 0);
        data->length = 40;
        data->row = 7;
        data->offset_in_row = 0;
        data->doc = "TX 64-byte frame";
    }
    if (dnx_mib_counter_nif_eth_t127 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t127, 0);
        data->length = 40;
        data->row = 7;
        data->offset_in_row = 1;
        data->doc = "TX 65 to 127 byte frame";
    }
    if (dnx_mib_counter_nif_eth_t255 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t255, 0);
        data->length = 40;
        data->row = 7;
        data->offset_in_row = 2;
        data->doc = "TX 128 to 255 byte frame";
    }
    if (dnx_mib_counter_nif_eth_t511 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t511, 0);
        data->length = 40;
        data->row = 7;
        data->offset_in_row = 3;
        data->doc = "TX 256 to 511 byte frame";
    }
    if (dnx_mib_counter_nif_eth_t1023 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t1023, 0);
        data->length = 40;
        data->row = 7;
        data->offset_in_row = 4;
        data->doc = "TX 512 to 1023 byte frame";
    }
    if (dnx_mib_counter_nif_eth_t1518 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t1518, 0);
        data->length = 40;
        data->row = 7;
        data->offset_in_row = 5;
        data->doc = "TX 1024 to 1518 byte frame";
    }
    if (dnx_mib_counter_nif_eth_tmgv < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tmgv, 0);
        data->length = 40;
        data->row = 7;
        data->offset_in_row = 6;
        data->doc = "TX 1519 to 1522 byte frame";
    }
    if (dnx_mib_counter_nif_eth_t2047 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t2047, 0);
        data->length = 40;
        data->row = 7;
        data->offset_in_row = 7;
        data->doc = "TX 1519 to 2047 byte frame";
    }
    if (dnx_mib_counter_nif_eth_t4095 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t4095, 0);
        data->length = 40;
        data->row = 8;
        data->offset_in_row = 0;
        data->doc = "TX 2048 to 4095 byte frame";
    }
    if (dnx_mib_counter_nif_eth_t9216 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t9216, 0);
        data->length = 40;
        data->row = 8;
        data->offset_in_row = 1;
        data->doc = "TX 4096 to 9216 byte frame";
    }
    if (dnx_mib_counter_nif_eth_t16383 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t16383, 0);
        data->length = 40;
        data->row = 8;
        data->offset_in_row = 2;
        data->doc = "TX 9217 to 16383 byte frame";
    }
    if (dnx_mib_counter_nif_eth_tbca < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tbca, 0);
        data->length = 40;
        data->row = 8;
        data->offset_in_row = 3;
        data->doc = "TX Broadcast frame";
    }
    if (dnx_mib_counter_nif_eth_tpfc0 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc0, 0);
        data->length = 40;
        data->row = 8;
        data->offset_in_row = 4;
        data->doc = "TX PFC frame with enable bit set for Priority0";
    }
    if (dnx_mib_counter_nif_eth_tpfcoff0 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff0, 0);
        data->length = 40;
        data->row = 8;
        data->offset_in_row = 5;
        data->doc = "TX PFC frame transition XON to XOFF for Priority0";
    }
    if (dnx_mib_counter_nif_eth_tpfc1 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc1, 0);
        data->length = 40;
        data->row = 8;
        data->offset_in_row = 6;
        data->doc = "TX PFC frame with enable bit set for Priority1";
    }
    if (dnx_mib_counter_nif_eth_tpfcoff1 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff1, 0);
        data->length = 40;
        data->row = 8;
        data->offset_in_row = 7;
        data->doc = "TX PFC frame transition XON to XOFF for Priority1";
    }
    if (dnx_mib_counter_nif_eth_tpfc2 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc2, 0);
        data->length = 40;
        data->row = 9;
        data->offset_in_row = 0;
        data->doc = "TX PFC frame with enable bit set for Priority2";
    }
    if (dnx_mib_counter_nif_eth_tpfcoff2 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff2, 0);
        data->length = 40;
        data->row = 9;
        data->offset_in_row = 1;
        data->doc = "TX PFC frame transition XON to XOFF for Priority2";
    }
    if (dnx_mib_counter_nif_eth_tpfc3 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc3, 0);
        data->length = 40;
        data->row = 9;
        data->offset_in_row = 2;
        data->doc = "TX PFC frame with enable bit set for Priority3";
    }
    if (dnx_mib_counter_nif_eth_tpfcoff3 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff3, 0);
        data->length = 40;
        data->row = 9;
        data->offset_in_row = 3;
        data->doc = "TX PFC frame transition XON to XOFF for Priority3";
    }
    if (dnx_mib_counter_nif_eth_tpfc4 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc4, 0);
        data->length = 40;
        data->row = 9;
        data->offset_in_row = 4;
        data->doc = "TX PFC frame with enable bit set for Priority4";
    }
    if (dnx_mib_counter_nif_eth_tpfcoff4 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff4, 0);
        data->length = 40;
        data->row = 9;
        data->offset_in_row = 5;
        data->doc = "TX PFC frame transition XON to XOFF for Priority4";
    }
    if (dnx_mib_counter_nif_eth_tpfc5 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc5, 0);
        data->length = 40;
        data->row = 9;
        data->offset_in_row = 6;
        data->doc = "TX PFC frame with enable bit set for Priority5";
    }
    if (dnx_mib_counter_nif_eth_tpfcoff5 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff5, 0);
        data->length = 40;
        data->row = 9;
        data->offset_in_row = 7;
        data->doc = "TX PFC frame transition XON to XOFF for Priority5";
    }
    if (dnx_mib_counter_nif_eth_tpfc6 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc6, 0);
        data->length = 40;
        data->row = 10;
        data->offset_in_row = 0;
        data->doc = "TX PFC frame with enable bit set for Priority6";
    }
    if (dnx_mib_counter_nif_eth_tpfcoff6 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff6, 0);
        data->length = 40;
        data->row = 10;
        data->offset_in_row = 1;
        data->doc = "TX PFC frame transition XON to XOFF for Priority6";
    }
    if (dnx_mib_counter_nif_eth_tpfc7 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc7, 0);
        data->length = 40;
        data->row = 10;
        data->offset_in_row = 2;
        data->doc = "TX PFC frame with enable bit set for Priority7";
    }
    if (dnx_mib_counter_nif_eth_tpfcoff7 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff7, 0);
        data->length = 40;
        data->row = 10;
        data->offset_in_row = 3;
        data->doc = "TX PFC frame transition XON to XOFF for Priority7";
    }
    if (dnx_mib_counter_nif_eth_tpkt < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpkt, 0);
        data->length = 40;
        data->row = 10;
        data->offset_in_row = 4;
        data->doc = "TX all packets frame";
    }
    if (dnx_mib_counter_nif_eth_tpok < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpok, 0);
        data->length = 40;
        data->row = 10;
        data->offset_in_row = 5;
        data->doc = "TX good frame";
    }
    if (dnx_mib_counter_nif_eth_tuca < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tuca, 0);
        data->length = 40;
        data->row = 10;
        data->offset_in_row = 6;
        data->doc = "TX UC frame";
    }
    if (dnx_mib_counter_nif_eth_tufl < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tufl, 0);
        data->length = 40;
        data->row = 10;
        data->offset_in_row = 7;
        data->doc = "TX FIFO Underrun frame";
    }
    if (dnx_mib_counter_nif_eth_tmca < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tmca, 0);
        data->length = 40;
        data->row = 11;
        data->offset_in_row = 0;
        data->doc = "TX MC frame";
    }
    if (dnx_mib_counter_nif_eth_txpf < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_txpf, 0);
        data->length = 40;
        data->row = 11;
        data->offset_in_row = 1;
        data->doc = "TX pause frame";
    }
    if (dnx_mib_counter_nif_eth_txpp < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_txpp, 0);
        data->length = 40;
        data->row = 11;
        data->offset_in_row = 2;
        data->doc = "TX PFC frame";
    }
    if (dnx_mib_counter_nif_eth_txcf < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_txcf, 0);
        data->length = 40;
        data->row = 11;
        data->offset_in_row = 3;
        data->doc = "TX control frame";
    }
    if (dnx_mib_counter_nif_eth_tfcs < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tfcs, 0);
        data->length = 40;
        data->row = 11;
        data->offset_in_row = 4;
        data->doc = "TX FCS error frame";
    }
    if (dnx_mib_counter_nif_eth_terr < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_terr, 0);
        data->length = 40;
        data->row = 11;
        data->offset_in_row = 5;
        data->doc = "TX Error frame";
    }
    if (dnx_mib_counter_nif_eth_tovr < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tovr, 0);
        data->length = 40;
        data->row = 11;
        data->offset_in_row = 6;
        data->doc = "TX Oversize frame";
    }
    if (dnx_mib_counter_nif_eth_tjbr < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tjbr, 0);
        data->length = 40;
        data->row = 11;
        data->offset_in_row = 7;
        data->doc = "TX jabber frame";
    }
    if (dnx_mib_counter_nif_eth_trpkt < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_trpkt, 0);
        data->length = 40;
        data->row = 12;
        data->offset_in_row = 0;
        data->doc = "TX RUNT frame";
    }
    if (dnx_mib_counter_nif_eth_tfrg < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tfrg, 0);
        data->length = 40;
        data->row = 12;
        data->offset_in_row = 1;
        data->doc = "TX runt packet with invalid FCS frame";
    }
    if (dnx_mib_counter_nif_eth_tvln < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tvln, 0);
        data->length = 40;
        data->row = 12;
        data->offset_in_row = 2;
        data->doc = "TX single and double VLAN tagged frame";
    }
    if (dnx_mib_counter_nif_eth_tdvln < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tdvln, 0);
        data->length = 40;
        data->row = 12;
        data->offset_in_row = 3;
        data->doc = "TX double VLAN tagged frame";
    }
    if (dnx_mib_counter_nif_eth_rbyt < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rbyt, 0);
        data->length = 48;
        data->row = 12;
        data->offset_in_row = 4;
        data->doc = "RX Byte frame";
    }
    if (dnx_mib_counter_nif_eth_rrbyt < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rrbyt, 0);
        data->length = 48;
        data->row = 12;
        data->offset_in_row = 5;
        data->doc = "RX Runt Byte frame";
    }
    if (dnx_mib_counter_nif_eth_tbyt < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tbyt, 0);
        data->length = 48;
        data->row = 12;
        data->offset_in_row = 6;
        data->doc = "TX Byte frame";
    }
    if (dnx_mib_counter_nif_eth_rverify < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rverify, 0);
        data->length = 40;
        data->doc = "RX MAC Verify MFrames";
    }
    if (dnx_mib_counter_nif_eth_rreply < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rreply, 0);
        data->length = 40;
        data->doc = "RX MAC Reply MFrames";
    }
    if (dnx_mib_counter_nif_eth_rmcrc < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rmcrc, 0);
        data->length = 40;
        data->doc = "RX Matched CRC Frames";
    }
    if (dnx_mib_counter_nif_eth_rtrfu < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rtrfu, 0);
        data->length = 40;
        data->doc = "RX Truncated Frames";
    }
    if (dnx_mib_counter_nif_eth_rschcrc < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rschcrc, 0);
        data->length = 40;
        data->doc = "RX SCH CRC Errors";
    }
    if (dnx_mib_counter_nif_eth_rx_eee_lpi_event < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rx_eee_lpi_event, 0);
        data->length = 40;
        data->doc = "RX EEE LPI Events";
    }
    if (dnx_mib_counter_nif_eth_rx_eee_lpi_duration < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rx_eee_lpi_duration, 0);
        data->length = 40;
        data->doc = "RX EEE LPI Duration";
    }
    if (dnx_mib_counter_nif_eth_rx_llfc_phy < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rx_llfc_phy, 0);
        data->length = 40;
        data->doc = "RX Physical Type LLFC messages";
    }
    if (dnx_mib_counter_nif_eth_rx_llfc_log < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rx_llfc_log, 0);
        data->length = 40;
        data->doc = "RX Logical Type LLFC messages";
    }
    if (dnx_mib_counter_nif_eth_rx_llfc_crc < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rx_llfc_crc, 0);
        data->length = 40;
        data->doc = "RX Type LLFC message with CRC errors";
    }
    if (dnx_mib_counter_nif_eth_rx_hcfc < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rx_hcfc, 0);
        data->length = 40;
        data->doc = "RX HCFC messages";
    }
    if (dnx_mib_counter_nif_eth_rx_hcfc_crc < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rx_hcfc_crc, 0);
        data->length = 40;
        data->doc = "RX HCFC messages with CRC Error";
    }
    if (dnx_mib_counter_nif_eth_tverify < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tverify, 0);
        data->length = 40;
        data->doc = "TX MAC Verify MFrames";
    }
    if (dnx_mib_counter_nif_eth_treply < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_treply, 0);
        data->length = 40;
        data->doc = "TX MAC Reply MFrames";
    }
    if (dnx_mib_counter_nif_eth_tscl < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tscl, 0);
        data->length = 40;
        data->doc = "TX Single Collision Frames";
    }
    if (dnx_mib_counter_nif_eth_tmcl < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tmcl, 0);
        data->length = 40;
        data->doc = "TX Multiple Collision Frames";
    }
    if (dnx_mib_counter_nif_eth_tlcl < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tlcl, 0);
        data->length = 40;
        data->doc = "TX Late Collision Frames";
    }
    if (dnx_mib_counter_nif_eth_txcl < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_txcl, 0);
        data->length = 40;
        data->doc = "TX Excessive Collision Frames";
    }
    if (dnx_mib_counter_nif_eth_tx_eee_lpi_event < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tx_eee_lpi_event, 0);
        data->length = 40;
        data->doc = "TX EEE LPI Events";
    }
    if (dnx_mib_counter_nif_eth_tx_eee_lpi_duration < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tx_eee_lpi_duration, 0);
        data->length = 40;
        data->doc = "TX EEE LPI Duration";
    }
    if (dnx_mib_counter_nif_eth_tx_llfc_log < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tx_llfc_log, 0);
        data->length = 40;
        data->doc = "TX Logical Type LLFC messages";
    }
    if (dnx_mib_counter_nif_eth_tx_hcfc < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tx_hcfc, 0);
        data->length = 40;
        data->doc = "TX HCFC messages";
    }
    if (dnx_mib_counter_nif_eth_tncl < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tncl, 0);
        data->length = 40;
        data->doc = "TX Total Collisions";
    }
    if (dnx_mib_counter_nif_eth_xthol < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_xthol, 0);
        data->length = 40;
        data->doc = "TX End-to-End HOL packet (E2ECC/VOQFC)";
    }
    if (dnx_mib_counter_nif_eth_tspare0 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tspare0, 0);
        data->length = 48;
        data->doc = "TX Spare0";
    }
    if (dnx_mib_counter_nif_eth_tspare1 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tspare1, 0);
        data->length = 48;
        data->doc = "TX Spare1";
    }
    if (dnx_mib_counter_nif_eth_tspare2 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tspare2, 0);
        data->length = 48;
        data->doc = "TX Spare2";
    }
    if (dnx_mib_counter_nif_eth_tspare3 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tspare3, 0);
        data->length = 48;
        data->doc = "TX Spare3";
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
jr2_a0_data_mib_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_mib;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_mib_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_mib_general_define_nof_row_per_cdu_lane;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mib_general_nof_row_per_cdu_lane_set;
    data_index = dnx_data_mib_general_define_stat_interval;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mib_general_stat_interval_set;
    data_index = dnx_data_mib_general_define_stat_jumbo;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mib_general_stat_jumbo_set;

    
    data_index = dnx_data_mib_general_mib_counters_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mib_general_mib_counters_support_set;

    
    data_index = dnx_data_mib_general_table_stat_pbmp;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mib_general_stat_pbmp_set;
    
    submodule_index = dnx_data_mib_submodule_controlled_counters;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_mib_controlled_counters_table_map_data;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mib_controlled_counters_map_data_set;
    data_index = dnx_data_mib_controlled_counters_table_set_data;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mib_controlled_counters_set_data_set;
    data_index = dnx_data_mib_controlled_counters_table_counter_data;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_mib_controlled_counters_counter_data_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

