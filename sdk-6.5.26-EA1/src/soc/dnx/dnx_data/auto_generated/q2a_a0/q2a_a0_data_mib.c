
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

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_mib.h>








static shr_error_e
q2a_a0_dnx_data_mib_controlled_counters_flexmac_mib_general_cnt_len_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mib;
    int submodule_index = dnx_data_mib_submodule_controlled_counters;
    int define_index = dnx_data_mib_controlled_counters_define_flexmac_mib_general_cnt_len;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 40;

    
    define->data = 40;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_mib_controlled_counters_flexmac_mib_rbyt_cnt_len_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mib;
    int submodule_index = dnx_data_mib_submodule_controlled_counters;
    int define_index = dnx_data_mib_controlled_counters_define_flexmac_mib_rbyt_cnt_len;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 40;

    
    define->data = 40;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_mib_controlled_counters_flexmac_mib_rrbyt_cnt_len_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mib;
    int submodule_index = dnx_data_mib_submodule_controlled_counters;
    int define_index = dnx_data_mib_controlled_counters_define_flexmac_mib_rrbyt_cnt_len;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 40;

    
    define->data = 40;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_mib_controlled_counters_flexmac_mib_tbyt_cnt_len_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mib;
    int submodule_index = dnx_data_mib_submodule_controlled_counters;
    int define_index = dnx_data_mib_controlled_counters_define_flexmac_mib_tbyt_cnt_len;
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
q2a_a0_dnx_data_mib_controlled_counters_flexmac_counter_data_set(
    int unit)
{
    int counter_id_index;
    dnx_data_mib_controlled_counters_flexmac_counter_data_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mib;
    int submodule_index = dnx_data_mib_submodule_controlled_counters;
    int table_index = dnx_data_mib_controlled_counters_table_flexmac_counter_data;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_mib_counter_nof;
    table->info_get.key_size[0] = dnx_mib_counter_nof;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "N/A";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mib_controlled_counters_flexmac_counter_data_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mib_controlled_counters_table_flexmac_counter_data");

    
    default_data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->length = -1;
    default_data->doc = "N/A";
    
    for (counter_id_index = 0; counter_id_index < table->keys[0].size; counter_id_index++)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, counter_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (dnx_mib_counter_nif_eth_r64 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r64, 0);
        data->length = 40;
        data->doc = "RX 64-byte frame";
    }
    if (dnx_mib_counter_nif_eth_r127 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r127, 0);
        data->length = 40;
        data->doc = "RX 65 to 127 byte frame";
    }
    if (dnx_mib_counter_nif_eth_r255 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r255, 0);
        data->length = 40;
        data->doc = "RX 128 to 255 byte frame";
    }
    if (dnx_mib_counter_nif_eth_r511 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r511, 0);
        data->length = 40;
        data->doc = "RX 256 to 511 byte frame";
    }
    if (dnx_mib_counter_nif_eth_r1023 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r1023, 0);
        data->length = 40;
        data->doc = "RX 512 to 1023 byte frame";
    }
    if (dnx_mib_counter_nif_eth_r1518 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r1518, 0);
        data->length = 40;
        data->doc = "RX 1024 to 1518 byte frame";
    }
    if (dnx_mib_counter_nif_eth_rmgv < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rmgv, 0);
        data->length = 40;
        data->doc = "RX 1519 to 1522 byte frame";
    }
    if (dnx_mib_counter_nif_eth_r2047 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r2047, 0);
        data->length = 40;
        data->doc = "RX 1519 to 2047 byte frame";
    }
    if (dnx_mib_counter_nif_eth_r4095 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r4095, 0);
        data->length = 40;
        data->doc = "RX 2048 to 4095 byte frame";
    }
    if (dnx_mib_counter_nif_eth_r9216 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r9216, 0);
        data->length = 40;
        data->doc = "RX 4096 to 9216 byte frame";
    }
    if (dnx_mib_counter_nif_eth_r16383 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_r16383, 0);
        data->length = 40;
        data->doc = "RX 9217 to 16383 byte frame";
    }
    if (dnx_mib_counter_nif_eth_rbca < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rbca, 0);
        data->length = 40;
        data->doc = "RX BC frame";
    }
    if (dnx_mib_counter_nif_eth_rprog0 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rprog0, 0);
        data->length = 40;
        data->doc = "RX RPROG0 frame";
    }
    if (dnx_mib_counter_nif_eth_rprog1 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rprog1, 0);
        data->length = 40;
        data->doc = "RX RPROG1 frame";
    }
    if (dnx_mib_counter_nif_eth_rprog2 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rprog2, 0);
        data->length = 40;
        data->doc = "RX RPROG2 frame";
    }
    if (dnx_mib_counter_nif_eth_rprog3 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rprog3, 0);
        data->length = 40;
        data->doc = "RX RPROG3 frame";
    }
    if (dnx_mib_counter_nif_eth_rpkt < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpkt, 0);
        data->length = 40;
        data->doc = "RX all packets frame";
    }
    if (dnx_mib_counter_nif_eth_rpok < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpok, 0);
        data->length = 40;
        data->doc = "RX good frame";
    }
    if (dnx_mib_counter_nif_eth_ruca < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_ruca, 0);
        data->length = 40;
        data->doc = "RX UC frame";
    }
    if (dnx_mib_counter_nif_eth_rmca < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rmca, 0);
        data->length = 40;
        data->doc = "RX MC frame";
    }
    if (dnx_mib_counter_nif_eth_rxpf < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rxpf, 0);
        data->length = 40;
        data->doc = "RX pause frame";
    }
    if (dnx_mib_counter_nif_eth_rxpp < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rxpp, 0);
        data->length = 40;
        data->doc = "RX PFC frame";
    }
    if (dnx_mib_counter_nif_eth_rxcf < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rxcf, 0);
        data->length = 40;
        data->doc = "RX control frame";
    }
    if (dnx_mib_counter_nif_eth_rfcs < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rfcs, 0);
        data->length = 40;
        data->doc = "RX FCS error frame";
    }
    if (dnx_mib_counter_nif_eth_rerpkt < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rerpkt, 0);
        data->length = 40;
        data->doc = "RX code error frame";
    }
    if (dnx_mib_counter_nif_eth_rflr < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rflr, 0);
        data->length = 40;
        data->doc = "RX out-of-range length frame";
    }
    if (dnx_mib_counter_nif_eth_rjbr < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rjbr, 0);
        data->length = 40;
        data->doc = "RX jabber frame";
    }
    if (dnx_mib_counter_nif_eth_rmtue < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rmtue, 0);
        data->length = 40;
        data->doc = "RX MTU check error frame";
    }
    if (dnx_mib_counter_nif_eth_rovr < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rovr, 0);
        data->length = 40;
        data->doc = "RX oversized frame";
    }
    if (dnx_mib_counter_nif_eth_rvln < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rvln, 0);
        data->length = 40;
        data->doc = "RX single and double VLAN tagged frame";
    }
    if (dnx_mib_counter_nif_eth_rdvln < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rdvln, 0);
        data->length = 40;
        data->doc = "RX double VLAN tagged frame";
    }
    if (dnx_mib_counter_nif_eth_rxuo < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rxuo, 0);
        data->length = 40;
        data->doc = "RX unsupported opcode frame";
    }
    if (dnx_mib_counter_nif_eth_rxuda < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rxuda, 0);
        data->length = 40;
        data->doc = "RX unsupported DA for pause/PFC frame";
    }
    if (dnx_mib_counter_nif_eth_rxwsa < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rxwsa, 0);
        data->length = 40;
        data->doc = "RX incorrect SA frame";
    }
    if (dnx_mib_counter_nif_eth_rprm < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rprm, 0);
        data->length = 40;
        data->doc = "RX promiscuous frame";
    }
    if (dnx_mib_counter_nif_eth_rpfc0 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc0, 0);
        data->length = 40;
        data->doc = "RX PFC frame with enable bit set for Priority0";
    }
    if (dnx_mib_counter_nif_eth_rpfcoff0 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff0, 0);
        data->length = 40;
        data->doc = "RX PFC frame transition XON to XOFF for Priority";
    }
    if (dnx_mib_counter_nif_eth_rpfc1 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc1, 0);
        data->length = 40;
        data->doc = "RX PFC frame with enable bit set for Priority1";
    }
    if (dnx_mib_counter_nif_eth_rpfcoff1 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff1, 0);
        data->length = 40;
        data->doc = "RX PFC frame transition XON to XOFF for Priority1";
    }
    if (dnx_mib_counter_nif_eth_rpfc2 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc2, 0);
        data->length = 40;
        data->doc = "RX PFC frame with enable bit set for Priority2";
    }
    if (dnx_mib_counter_nif_eth_rpfcoff2 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff2, 0);
        data->length = 40;
        data->doc = "RX PFC frame transition XON to XOFF for Priority2";
    }
    if (dnx_mib_counter_nif_eth_rpfc3 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc3, 0);
        data->length = 40;
        data->doc = "RX PFC frame with enable bit set for Priority3";
    }
    if (dnx_mib_counter_nif_eth_rpfcoff3 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff3, 0);
        data->length = 40;
        data->doc = "RX PFC frame transition XON to XOFF for Priority3";
    }
    if (dnx_mib_counter_nif_eth_rpfc4 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc4, 0);
        data->length = 40;
        data->doc = "RX PFC frame with enable bit set for Priority4";
    }
    if (dnx_mib_counter_nif_eth_rpfcoff4 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff4, 0);
        data->length = 40;
        data->doc = "RX PFC frame transition XON to XOFF for Priority4";
    }
    if (dnx_mib_counter_nif_eth_rpfc5 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc5, 0);
        data->length = 40;
        data->doc = "RX PFC frame with enable bit set for Priority5";
    }
    if (dnx_mib_counter_nif_eth_rpfcoff5 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff5, 0);
        data->length = 40;
        data->doc = "RX PFC frame transition XON to XOFF for Priority5";
    }
    if (dnx_mib_counter_nif_eth_rpfc6 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc6, 0);
        data->length = 40;
        data->doc = "RX PFC frame with enable bit set for Priority6";
    }
    if (dnx_mib_counter_nif_eth_rpfcoff6 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff6, 0);
        data->length = 40;
        data->doc = "RX PFC frame transition XON to XOFF for Priority6";
    }
    if (dnx_mib_counter_nif_eth_rpfc7 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfc7, 0);
        data->length = 40;
        data->doc = "RX PFC frame with enable bit set for Priority7";
    }
    if (dnx_mib_counter_nif_eth_rpfcoff7 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rpfcoff7, 0);
        data->length = 40;
        data->doc = "RX PFC frame transition XON to XOFF for Priority7";
    }
    if (dnx_mib_counter_nif_eth_rund < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rund, 0);
        data->length = 40;
        data->doc = "RX undersized frame";
    }
    if (dnx_mib_counter_nif_eth_rfrg < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rfrg, 0);
        data->length = 40;
        data->doc = "RX fragment frame";
    }
    if (dnx_mib_counter_nif_eth_rrpkt < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rrpkt, 0);
        data->length = 40;
        data->doc = "RX RUNT frame";
    }
    if (dnx_mib_counter_nif_eth_t64 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t64, 0);
        data->length = 40;
        data->doc = "TX 64-byte frame";
    }
    if (dnx_mib_counter_nif_eth_t127 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t127, 0);
        data->length = 40;
        data->doc = "TX 65 to 127 byte frame";
    }
    if (dnx_mib_counter_nif_eth_t255 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t255, 0);
        data->length = 40;
        data->doc = "TX 128 to 255 byte frame";
    }
    if (dnx_mib_counter_nif_eth_t511 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t511, 0);
        data->length = 40;
        data->doc = "TX 256 to 511 byte frame";
    }
    if (dnx_mib_counter_nif_eth_t1023 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t1023, 0);
        data->length = 40;
        data->doc = "TX 512 to 1023 byte frame";
    }
    if (dnx_mib_counter_nif_eth_t1518 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t1518, 0);
        data->length = 40;
        data->doc = "TX 1024 to 1518 byte frame";
    }
    if (dnx_mib_counter_nif_eth_tmgv < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tmgv, 0);
        data->length = 40;
        data->doc = "TX 1519 to 1522 byte frame";
    }
    if (dnx_mib_counter_nif_eth_t2047 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t2047, 0);
        data->length = 40;
        data->doc = "TX 1519 to 2047 byte frame";
    }
    if (dnx_mib_counter_nif_eth_t4095 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t4095, 0);
        data->length = 40;
        data->doc = "TX 2048 to 4095 byte frame";
    }
    if (dnx_mib_counter_nif_eth_t9216 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t9216, 0);
        data->length = 40;
        data->doc = "TX 4096 to 9216 byte frame";
    }
    if (dnx_mib_counter_nif_eth_t16383 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_t16383, 0);
        data->length = 40;
        data->doc = "TX 9217 to 16383 byte frame";
    }
    if (dnx_mib_counter_nif_eth_tbca < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tbca, 0);
        data->length = 40;
        data->doc = "TX Broadcast frame";
    }
    if (dnx_mib_counter_nif_eth_tpfc0 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc0, 0);
        data->length = 40;
        data->doc = "TX PFC frame with enable bit set for Priority0";
    }
    if (dnx_mib_counter_nif_eth_tpfcoff0 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff0, 0);
        data->length = 40;
        data->doc = "TX PFC frame transition XON to XOFF for Priority0";
    }
    if (dnx_mib_counter_nif_eth_tpfc1 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc1, 0);
        data->length = 40;
        data->doc = "TX PFC frame with enable bit set for Priority1";
    }
    if (dnx_mib_counter_nif_eth_tpfcoff1 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff1, 0);
        data->length = 40;
        data->doc = "TX PFC frame transition XON to XOFF for Priority1";
    }
    if (dnx_mib_counter_nif_eth_tpfc2 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc2, 0);
        data->length = 40;
        data->doc = "TX PFC frame with enable bit set for Priority2";
    }
    if (dnx_mib_counter_nif_eth_tpfcoff2 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff2, 0);
        data->length = 40;
        data->doc = "TX PFC frame transition XON to XOFF for Priority2";
    }
    if (dnx_mib_counter_nif_eth_tpfc3 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc3, 0);
        data->length = 40;
        data->doc = "TX PFC frame with enable bit set for Priority3";
    }
    if (dnx_mib_counter_nif_eth_tpfcoff3 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff3, 0);
        data->length = 40;
        data->doc = "TX PFC frame transition XON to XOFF for Priority3";
    }
    if (dnx_mib_counter_nif_eth_tpfc4 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc4, 0);
        data->length = 40;
        data->doc = "TX PFC frame with enable bit set for Priority4";
    }
    if (dnx_mib_counter_nif_eth_tpfcoff4 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff4, 0);
        data->length = 40;
        data->doc = "TX PFC frame transition XON to XOFF for Priority4";
    }
    if (dnx_mib_counter_nif_eth_tpfc5 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc5, 0);
        data->length = 40;
        data->doc = "TX PFC frame with enable bit set for Priority5";
    }
    if (dnx_mib_counter_nif_eth_tpfcoff5 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff5, 0);
        data->length = 40;
        data->doc = "TX PFC frame transition XON to XOFF for Priority5";
    }
    if (dnx_mib_counter_nif_eth_tpfc6 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc6, 0);
        data->length = 40;
        data->doc = "TX PFC frame with enable bit set for Priority6";
    }
    if (dnx_mib_counter_nif_eth_tpfcoff6 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff6, 0);
        data->length = 40;
        data->doc = "TX PFC frame transition XON to XOFF for Priority6";
    }
    if (dnx_mib_counter_nif_eth_tpfc7 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfc7, 0);
        data->length = 40;
        data->doc = "TX PFC frame with enable bit set for Priority7";
    }
    if (dnx_mib_counter_nif_eth_tpfcoff7 < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpfcoff7, 0);
        data->length = 40;
        data->doc = "TX PFC frame transition XON to XOFF for Priority7";
    }
    if (dnx_mib_counter_nif_eth_tpkt < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpkt, 0);
        data->length = 40;
        data->doc = "TX all packets frame";
    }
    if (dnx_mib_counter_nif_eth_tpok < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tpok, 0);
        data->length = 40;
        data->doc = "TX good frame";
    }
    if (dnx_mib_counter_nif_eth_tuca < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tuca, 0);
        data->length = 40;
        data->doc = "TX UC frame";
    }
    if (dnx_mib_counter_nif_eth_tufl < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tufl, 0);
        data->length = 40;
        data->doc = "TX FIFO Underrun frame";
    }
    if (dnx_mib_counter_nif_eth_tmca < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tmca, 0);
        data->length = 40;
        data->doc = "TX MC frame";
    }
    if (dnx_mib_counter_nif_eth_txpf < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_txpf, 0);
        data->length = 40;
        data->doc = "TX pause frame";
    }
    if (dnx_mib_counter_nif_eth_txpp < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_txpp, 0);
        data->length = 40;
        data->doc = "TX PFC frame";
    }
    if (dnx_mib_counter_nif_eth_txcf < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_txcf, 0);
        data->length = 40;
        data->doc = "TX control frame";
    }
    if (dnx_mib_counter_nif_eth_tfcs < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tfcs, 0);
        data->length = 40;
        data->doc = "TX FCS error frame";
    }
    if (dnx_mib_counter_nif_eth_terr < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_terr, 0);
        data->length = 40;
        data->doc = "TX Error frame";
    }
    if (dnx_mib_counter_nif_eth_tovr < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tovr, 0);
        data->length = 40;
        data->doc = "TX Oversize frame";
    }
    if (dnx_mib_counter_nif_eth_tjbr < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tjbr, 0);
        data->length = 40;
        data->doc = "TX jabber frame";
    }
    if (dnx_mib_counter_nif_eth_trpkt < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_trpkt, 0);
        data->length = 40;
        data->doc = "TX RUNT frame";
    }
    if (dnx_mib_counter_nif_eth_tfrg < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tfrg, 0);
        data->length = 40;
        data->doc = "TX runt packet with invalid FCS frame";
    }
    if (dnx_mib_counter_nif_eth_tvln < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tvln, 0);
        data->length = 40;
        data->doc = "TX single and double VLAN tagged frame";
    }
    if (dnx_mib_counter_nif_eth_tdvln < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tdvln, 0);
        data->length = 40;
        data->doc = "TX double VLAN tagged frame";
    }
    if (dnx_mib_counter_nif_eth_rbyt < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rbyt, 0);
        data->length = 40;
        data->doc = "RX Byte frame";
    }
    if (dnx_mib_counter_nif_eth_rrbyt < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_rrbyt, 0);
        data->length = 40;
        data->doc = "RX Runt Byte frame";
    }
    if (dnx_mib_counter_nif_eth_tbyt < table->keys[0].size)
    {
        data = (dnx_data_mib_controlled_counters_flexmac_counter_data_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_mib_counter_nif_eth_tbyt, 0);
        data->length = 48;
        data->doc = "TX Byte frame";
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
q2a_a0_data_mib_attach(
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
    
    submodule_index = dnx_data_mib_submodule_controlled_counters;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_mib_controlled_counters_define_flexmac_mib_general_cnt_len;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_mib_controlled_counters_flexmac_mib_general_cnt_len_set;
    data_index = dnx_data_mib_controlled_counters_define_flexmac_mib_rbyt_cnt_len;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_mib_controlled_counters_flexmac_mib_rbyt_cnt_len_set;
    data_index = dnx_data_mib_controlled_counters_define_flexmac_mib_rrbyt_cnt_len;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_mib_controlled_counters_flexmac_mib_rrbyt_cnt_len_set;
    data_index = dnx_data_mib_controlled_counters_define_flexmac_mib_tbyt_cnt_len;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_mib_controlled_counters_flexmac_mib_tbyt_cnt_len_set;

    

    
    data_index = dnx_data_mib_controlled_counters_table_flexmac_counter_data;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_mib_controlled_counters_flexmac_counter_data_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

