
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_adapter.h>



#ifdef BCM_DNX2_SUPPORT

extern shr_error_e jr2_a0_data_adapter_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e jr2_b0_data_adapter_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e j2c_a0_data_adapter_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e q2a_a0_data_adapter_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e q2a_b0_data_adapter_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e j2p_a0_data_adapter_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e j2x_a0_data_adapter_attach(
    int unit);

#endif 



static shr_error_e
dnx_data_adapter_tx_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "tx";
    submodule_data->doc = "adapter tx";
    
    submodule_data->nof_features = _dnx_data_adapter_tx_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data adapter tx features");

    
    submodule_data->nof_defines = _dnx_data_adapter_tx_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data adapter tx defines");

    submodule_data->defines[dnx_data_adapter_tx_define_loopback_enable].name = "loopback_enable";
    submodule_data->defines[dnx_data_adapter_tx_define_loopback_enable].doc = "Use loopback in the adapter server instead of go through the pipe when receiving tx packets";
    
    submodule_data->defines[dnx_data_adapter_tx_define_loopback_enable].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_adapter_tx_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data adapter tx tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_adapter_tx_feature_get(
    int unit,
    dnx_data_adapter_tx_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_tx, feature);
}


uint32
dnx_data_adapter_tx_loopback_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_tx, dnx_data_adapter_tx_define_loopback_enable);
}










static shr_error_e
dnx_data_adapter_rx_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "rx";
    submodule_data->doc = "adapter rx";
    
    submodule_data->nof_features = _dnx_data_adapter_rx_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data adapter rx features");

    
    submodule_data->nof_defines = _dnx_data_adapter_rx_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data adapter rx defines");

    submodule_data->defines[dnx_data_adapter_rx_define_constant_header_size].name = "constant_header_size";
    submodule_data->defines[dnx_data_adapter_rx_define_constant_header_size].doc = "Number of bytes in the constant header of a rx packet";
    
    submodule_data->defines[dnx_data_adapter_rx_define_constant_header_size].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_adapter_rx_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data adapter rx tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_adapter_rx_feature_get(
    int unit,
    dnx_data_adapter_rx_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_rx, feature);
}


uint32
dnx_data_adapter_rx_constant_header_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_rx, dnx_data_adapter_rx_define_constant_header_size);
}










static shr_error_e
dnx_data_adapter_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "adapter general configurations";
    
    submodule_data->nof_features = _dnx_data_adapter_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data adapter general features");

    
    submodule_data->nof_defines = _dnx_data_adapter_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data adapter general defines");

    submodule_data->defines[dnx_data_adapter_general_define_lib_ver].name = "lib_ver";
    submodule_data->defines[dnx_data_adapter_general_define_lib_ver].doc = "Adapter library version";
    
    submodule_data->defines[dnx_data_adapter_general_define_lib_ver].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_adapter_general_define_oamp_ms_id].name = "oamp_ms_id";
    submodule_data->defines[dnx_data_adapter_general_define_oamp_ms_id].doc = "MS Id for OAMP injection";
    
    submodule_data->defines[dnx_data_adapter_general_define_oamp_ms_id].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_adapter_general_define_transaction_type_code].name = "transaction_type_code";
    submodule_data->defines[dnx_data_adapter_general_define_transaction_type_code].doc = "Signal code from transaction type";
    
    submodule_data->defines[dnx_data_adapter_general_define_transaction_type_code].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_adapter_general_define_transaction_type_width].name = "transaction_type_width";
    submodule_data->defines[dnx_data_adapter_general_define_transaction_type_width].doc = "Transaction type nof bits";
    
    submodule_data->defines[dnx_data_adapter_general_define_transaction_type_width].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_adapter_general_define_input_oam_packet_code].name = "input_oam_packet_code";
    submodule_data->defines[dnx_data_adapter_general_define_input_oam_packet_code].doc = "Signal code from input OAM packet";
    
    submodule_data->defines[dnx_data_adapter_general_define_input_oam_packet_code].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_adapter_general_define_input_oam_packet_width].name = "input_oam_packet_width";
    submodule_data->defines[dnx_data_adapter_general_define_input_oam_packet_width].doc = "Input OAM packet nof bits";
    
    submodule_data->defines[dnx_data_adapter_general_define_input_oam_packet_width].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_adapter_general_define_input_packet_size_code].name = "input_packet_size_code";
    submodule_data->defines[dnx_data_adapter_general_define_input_packet_size_code].doc = "Signal code from input packet size";
    
    submodule_data->defines[dnx_data_adapter_general_define_input_packet_size_code].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_adapter_general_define_input_packet_size_width].name = "input_packet_size_width";
    submodule_data->defines[dnx_data_adapter_general_define_input_packet_size_width].doc = "Input packet size nof bits";
    
    submodule_data->defines[dnx_data_adapter_general_define_input_packet_size_width].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_header_signal_id].name = "tx_packet_header_signal_id";
    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_header_signal_id].doc = "TX: Header signal ID for packet injection";
    
    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_header_signal_id].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_tag_swap_res_signal_id].name = "tx_packet_tag_swap_res_signal_id";
    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_tag_swap_res_signal_id].doc = "TX: Tag-Swap-Res signal ID for packet injection";
    
    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_tag_swap_res_signal_id].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_recycle_command_signal_id].name = "tx_packet_recycle_command_signal_id";
    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_recycle_command_signal_id].doc = "TX: Recycle-Command signal ID for packet injection";
    
    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_recycle_command_signal_id].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_recycle_context_signal_id].name = "tx_packet_recycle_context_signal_id";
    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_recycle_context_signal_id].doc = "TX: Recycle-Contextsignal ID for packet injection";
    
    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_recycle_context_signal_id].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_valid_bytes_signal_id].name = "tx_packet_valid_bytes_signal_id";
    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_valid_bytes_signal_id].doc = "TX: Valid-bytes signal ID for packet injection";
    
    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_valid_bytes_signal_id].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_header_signal_width].name = "tx_packet_header_signal_width";
    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_header_signal_width].doc = "TX: Header signal width for packet injection";
    
    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_header_signal_width].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_tag_swap_res_signal_width].name = "tx_packet_tag_swap_res_signal_width";
    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_tag_swap_res_signal_width].doc = "TX: Tag-Swap-Res signal width for packet injection";
    
    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_tag_swap_res_signal_width].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_recycle_command_signal_width].name = "tx_packet_recycle_command_signal_width";
    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_recycle_command_signal_width].doc = "TX: Recycle-Command signal width for packet injection";
    
    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_recycle_command_signal_width].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_recycle_context_signal_width].name = "tx_packet_recycle_context_signal_width";
    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_recycle_context_signal_width].doc = "TX: Recycle-Contextsignal width for packet injection";
    
    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_recycle_context_signal_width].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_valid_bytes_signal_width].name = "tx_packet_valid_bytes_signal_width";
    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_valid_bytes_signal_width].doc = "TX: Valid-bytes signal width for packet injection";
    
    submodule_data->defines[dnx_data_adapter_general_define_tx_packet_valid_bytes_signal_width].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_adapter_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data adapter general tables");

    
    submodule_data->tables[dnx_data_adapter_general_table_Injection].name = "Injection";
    submodule_data->tables[dnx_data_adapter_general_table_Injection].doc = "Injection Packet information";
    submodule_data->tables[dnx_data_adapter_general_table_Injection].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_adapter_general_table_Injection].size_of_values = sizeof(dnx_data_adapter_general_Injection_t);
    submodule_data->tables[dnx_data_adapter_general_table_Injection].entry_get = dnx_data_adapter_general_Injection_entry_str_get;

    
    submodule_data->tables[dnx_data_adapter_general_table_Injection].nof_keys = 0;

    
    submodule_data->tables[dnx_data_adapter_general_table_Injection].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_adapter_general_table_Injection].values, dnxc_data_value_t, submodule_data->tables[dnx_data_adapter_general_table_Injection].nof_values, "_dnx_data_adapter_general_table_Injection table values");
    submodule_data->tables[dnx_data_adapter_general_table_Injection].values[0].name = "Injected_packet_name";
    submodule_data->tables[dnx_data_adapter_general_table_Injection].values[0].type = "char *";
    submodule_data->tables[dnx_data_adapter_general_table_Injection].values[0].doc = "Injected packet name";
    submodule_data->tables[dnx_data_adapter_general_table_Injection].values[0].offset = UTILEX_OFFSETOF(dnx_data_adapter_general_Injection_t, Injected_packet_name);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_adapter_general_feature_get(
    int unit,
    dnx_data_adapter_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_general, feature);
}


uint32
dnx_data_adapter_general_lib_ver_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_general, dnx_data_adapter_general_define_lib_ver);
}

uint32
dnx_data_adapter_general_oamp_ms_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_general, dnx_data_adapter_general_define_oamp_ms_id);
}

uint32
dnx_data_adapter_general_transaction_type_code_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_general, dnx_data_adapter_general_define_transaction_type_code);
}

uint32
dnx_data_adapter_general_transaction_type_width_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_general, dnx_data_adapter_general_define_transaction_type_width);
}

uint32
dnx_data_adapter_general_input_oam_packet_code_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_general, dnx_data_adapter_general_define_input_oam_packet_code);
}

uint32
dnx_data_adapter_general_input_oam_packet_width_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_general, dnx_data_adapter_general_define_input_oam_packet_width);
}

uint32
dnx_data_adapter_general_input_packet_size_code_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_general, dnx_data_adapter_general_define_input_packet_size_code);
}

uint32
dnx_data_adapter_general_input_packet_size_width_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_general, dnx_data_adapter_general_define_input_packet_size_width);
}

uint32
dnx_data_adapter_general_tx_packet_header_signal_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_general, dnx_data_adapter_general_define_tx_packet_header_signal_id);
}

uint32
dnx_data_adapter_general_tx_packet_tag_swap_res_signal_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_general, dnx_data_adapter_general_define_tx_packet_tag_swap_res_signal_id);
}

uint32
dnx_data_adapter_general_tx_packet_recycle_command_signal_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_general, dnx_data_adapter_general_define_tx_packet_recycle_command_signal_id);
}

uint32
dnx_data_adapter_general_tx_packet_recycle_context_signal_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_general, dnx_data_adapter_general_define_tx_packet_recycle_context_signal_id);
}

uint32
dnx_data_adapter_general_tx_packet_valid_bytes_signal_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_general, dnx_data_adapter_general_define_tx_packet_valid_bytes_signal_id);
}

uint32
dnx_data_adapter_general_tx_packet_header_signal_width_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_general, dnx_data_adapter_general_define_tx_packet_header_signal_width);
}

uint32
dnx_data_adapter_general_tx_packet_tag_swap_res_signal_width_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_general, dnx_data_adapter_general_define_tx_packet_tag_swap_res_signal_width);
}

uint32
dnx_data_adapter_general_tx_packet_recycle_command_signal_width_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_general, dnx_data_adapter_general_define_tx_packet_recycle_command_signal_width);
}

uint32
dnx_data_adapter_general_tx_packet_recycle_context_signal_width_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_general, dnx_data_adapter_general_define_tx_packet_recycle_context_signal_width);
}

uint32
dnx_data_adapter_general_tx_packet_valid_bytes_signal_width_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_general, dnx_data_adapter_general_define_tx_packet_valid_bytes_signal_width);
}



const dnx_data_adapter_general_Injection_t *
dnx_data_adapter_general_Injection_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_general, dnx_data_adapter_general_table_Injection);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_adapter_general_Injection_t *) data;

}


shr_error_e
dnx_data_adapter_general_Injection_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_adapter_general_Injection_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_general, dnx_data_adapter_general_table_Injection);
    data = (const dnx_data_adapter_general_Injection_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->Injected_packet_name == NULL ? "" : data->Injected_packet_name);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_adapter_general_Injection_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_general, dnx_data_adapter_general_table_Injection);

}






static shr_error_e
dnx_data_adapter_reg_mem_access_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "reg_mem_access";
    submodule_data->doc = "adapter reg/mem access";
    
    submodule_data->nof_features = _dnx_data_adapter_reg_mem_access_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data adapter reg_mem_access features");

    
    submodule_data->nof_defines = _dnx_data_adapter_reg_mem_access_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data adapter reg_mem_access defines");

    submodule_data->defines[dnx_data_adapter_reg_mem_access_define_do_collect_enable].name = "do_collect_enable";
    submodule_data->defines[dnx_data_adapter_reg_mem_access_define_do_collect_enable].doc = "Use do_collect in adapter in order to collect writes into one bulk write.";
    
    submodule_data->defines[dnx_data_adapter_reg_mem_access_define_do_collect_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_adapter_reg_mem_access_define_cmic_block_index].name = "cmic_block_index";
    submodule_data->defines[dnx_data_adapter_reg_mem_access_define_cmic_block_index].doc = "CMIC Block index";
    
    submodule_data->defines[dnx_data_adapter_reg_mem_access_define_cmic_block_index].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_adapter_reg_mem_access_define_iproc_block_index].name = "iproc_block_index";
    submodule_data->defines[dnx_data_adapter_reg_mem_access_define_iproc_block_index].doc = "IPROC Block index for Adapter Access";
    
    submodule_data->defines[dnx_data_adapter_reg_mem_access_define_iproc_block_index].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_adapter_reg_mem_access_define_swap_core_index_zero_with_core_index].name = "swap_core_index_zero_with_core_index";
    submodule_data->defines[dnx_data_adapter_reg_mem_access_define_swap_core_index_zero_with_core_index].doc = "swap core zero with other core index";
    
    submodule_data->defines[dnx_data_adapter_reg_mem_access_define_swap_core_index_zero_with_core_index].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_adapter_reg_mem_access_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data adapter reg_mem_access tables");

    
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map].name = "swap_core_ignore_map";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map].doc = "ignore swap core feature, for blocks in this map. the key is enum type soc_block_type_e";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map].size_of_values = sizeof(dnx_data_adapter_reg_mem_access_swap_core_ignore_map_t);
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map].entry_get = dnx_data_adapter_reg_mem_access_swap_core_ignore_map_entry_str_get;

    
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map].nof_keys = 1;
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map].keys[0].name = "block_type";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map].keys[0].doc = "HW block type";

    
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map].nof_values, "_dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map table values");
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map].values[0].name = "ignore";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map].values[0].type = "int";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map].values[0].doc = "ignore swap core";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_adapter_reg_mem_access_swap_core_ignore_map_t, ignore);

    
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map_by_global_block_ids_enum].name = "swap_core_ignore_map_by_global_block_ids_enum";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map_by_global_block_ids_enum].doc = "ignore swap core feature, for blocks in this map. same as swap_core_ignore_map, just different enum key type";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map_by_global_block_ids_enum].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map_by_global_block_ids_enum].size_of_values = sizeof(dnx_data_adapter_reg_mem_access_swap_core_ignore_map_by_global_block_ids_enum_t);
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map_by_global_block_ids_enum].entry_get = dnx_data_adapter_reg_mem_access_swap_core_ignore_map_by_global_block_ids_enum_entry_str_get;

    
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map_by_global_block_ids_enum].nof_keys = 1;
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map_by_global_block_ids_enum].keys[0].name = "block_type";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map_by_global_block_ids_enum].keys[0].doc = "HW block type";

    
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map_by_global_block_ids_enum].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map_by_global_block_ids_enum].values, dnxc_data_value_t, submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map_by_global_block_ids_enum].nof_values, "_dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map_by_global_block_ids_enum table values");
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map_by_global_block_ids_enum].values[0].name = "ignore";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map_by_global_block_ids_enum].values[0].type = "int";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map_by_global_block_ids_enum].values[0].doc = "ignore swap core";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map_by_global_block_ids_enum].values[0].offset = UTILEX_OFFSETOF(dnx_data_adapter_reg_mem_access_swap_core_ignore_map_by_global_block_ids_enum_t, ignore);

    
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_special_swap_core_table].name = "special_swap_core_table";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_special_swap_core_table].doc = "special swap core map, for regmem that are not swapped by the block id.";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_special_swap_core_table].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_special_swap_core_table].size_of_values = sizeof(dnx_data_adapter_reg_mem_access_special_swap_core_table_t);
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_special_swap_core_table].entry_get = dnx_data_adapter_reg_mem_access_special_swap_core_table_entry_str_get;

    
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_special_swap_core_table].nof_keys = 1;
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_special_swap_core_table].keys[0].name = "regmem_id";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_special_swap_core_table].keys[0].doc = "regmem id";

    
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_special_swap_core_table].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_adapter_reg_mem_access_table_special_swap_core_table].values, dnxc_data_value_t, submodule_data->tables[dnx_data_adapter_reg_mem_access_table_special_swap_core_table].nof_values, "_dnx_data_adapter_reg_mem_access_table_special_swap_core_table table values");
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_special_swap_core_table].values[0].name = "type";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_special_swap_core_table].values[0].type = "int";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_special_swap_core_table].values[0].doc = "type of special swap";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_special_swap_core_table].values[0].offset = UTILEX_OFFSETOF(dnx_data_adapter_reg_mem_access_special_swap_core_table_t, type);
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_special_swap_core_table].values[1].name = "arg";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_special_swap_core_table].values[1].type = "int";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_special_swap_core_table].values[1].doc = "basic arg. used to calc the swapped special index/offset";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_special_swap_core_table].values[1].offset = UTILEX_OFFSETOF(dnx_data_adapter_reg_mem_access_special_swap_core_table_t, arg);

    
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_mems].name = "clear_on_read_mems";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_mems].doc = "memories which will be  operated in mode clear on read in adapter";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_mems].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_mems].size_of_values = sizeof(dnx_data_adapter_reg_mem_access_clear_on_read_mems_t);
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_mems].entry_get = dnx_data_adapter_reg_mem_access_clear_on_read_mems_entry_str_get;

    
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_mems].nof_keys = 1;
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_mems].keys[0].name = "index";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_mems].keys[0].doc = "running index";

    
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_mems].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_mems].values, dnxc_data_value_t, submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_mems].nof_values, "_dnx_data_adapter_reg_mem_access_table_clear_on_read_mems table values");
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_mems].values[0].name = "mem";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_mems].values[0].type = "soc_mem_t";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_mems].values[0].doc = "memory";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_mems].values[0].offset = UTILEX_OFFSETOF(dnx_data_adapter_reg_mem_access_clear_on_read_mems_t, mem);

    
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_regs].name = "clear_on_read_regs";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_regs].doc = "registers which will be  operated in mode clear on read in adapter";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_regs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_regs].size_of_values = sizeof(dnx_data_adapter_reg_mem_access_clear_on_read_regs_t);
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_regs].entry_get = dnx_data_adapter_reg_mem_access_clear_on_read_regs_entry_str_get;

    
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_regs].nof_keys = 1;
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_regs].keys[0].name = "index";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_regs].keys[0].doc = "running index";

    
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_regs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_regs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_regs].nof_values, "_dnx_data_adapter_reg_mem_access_table_clear_on_read_regs table values");
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_regs].values[0].name = "reg";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_regs].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_regs].values[0].doc = "register";
    submodule_data->tables[dnx_data_adapter_reg_mem_access_table_clear_on_read_regs].values[0].offset = UTILEX_OFFSETOF(dnx_data_adapter_reg_mem_access_clear_on_read_regs_t, reg);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_adapter_reg_mem_access_feature_get(
    int unit,
    dnx_data_adapter_reg_mem_access_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_reg_mem_access, feature);
}


uint32
dnx_data_adapter_reg_mem_access_do_collect_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_reg_mem_access, dnx_data_adapter_reg_mem_access_define_do_collect_enable);
}

uint32
dnx_data_adapter_reg_mem_access_cmic_block_index_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_reg_mem_access, dnx_data_adapter_reg_mem_access_define_cmic_block_index);
}

uint32
dnx_data_adapter_reg_mem_access_iproc_block_index_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_reg_mem_access, dnx_data_adapter_reg_mem_access_define_iproc_block_index);
}

uint32
dnx_data_adapter_reg_mem_access_swap_core_index_zero_with_core_index_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_reg_mem_access, dnx_data_adapter_reg_mem_access_define_swap_core_index_zero_with_core_index);
}



const dnx_data_adapter_reg_mem_access_swap_core_ignore_map_t *
dnx_data_adapter_reg_mem_access_swap_core_ignore_map_get(
    int unit,
    int block_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_reg_mem_access, dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, block_type, 0);
    return (const dnx_data_adapter_reg_mem_access_swap_core_ignore_map_t *) data;

}

const dnx_data_adapter_reg_mem_access_swap_core_ignore_map_by_global_block_ids_enum_t *
dnx_data_adapter_reg_mem_access_swap_core_ignore_map_by_global_block_ids_enum_get(
    int unit,
    int block_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_reg_mem_access, dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map_by_global_block_ids_enum);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, block_type, 0);
    return (const dnx_data_adapter_reg_mem_access_swap_core_ignore_map_by_global_block_ids_enum_t *) data;

}

const dnx_data_adapter_reg_mem_access_special_swap_core_table_t *
dnx_data_adapter_reg_mem_access_special_swap_core_table_get(
    int unit,
    int regmem_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_reg_mem_access, dnx_data_adapter_reg_mem_access_table_special_swap_core_table);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, regmem_id, 0);
    return (const dnx_data_adapter_reg_mem_access_special_swap_core_table_t *) data;

}

const dnx_data_adapter_reg_mem_access_clear_on_read_mems_t *
dnx_data_adapter_reg_mem_access_clear_on_read_mems_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_reg_mem_access, dnx_data_adapter_reg_mem_access_table_clear_on_read_mems);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_adapter_reg_mem_access_clear_on_read_mems_t *) data;

}

const dnx_data_adapter_reg_mem_access_clear_on_read_regs_t *
dnx_data_adapter_reg_mem_access_clear_on_read_regs_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_reg_mem_access, dnx_data_adapter_reg_mem_access_table_clear_on_read_regs);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_adapter_reg_mem_access_clear_on_read_regs_t *) data;

}


shr_error_e
dnx_data_adapter_reg_mem_access_swap_core_ignore_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_adapter_reg_mem_access_swap_core_ignore_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_reg_mem_access, dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map);
    data = (const dnx_data_adapter_reg_mem_access_swap_core_ignore_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ignore);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_adapter_reg_mem_access_swap_core_ignore_map_by_global_block_ids_enum_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_adapter_reg_mem_access_swap_core_ignore_map_by_global_block_ids_enum_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_reg_mem_access, dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map_by_global_block_ids_enum);
    data = (const dnx_data_adapter_reg_mem_access_swap_core_ignore_map_by_global_block_ids_enum_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ignore);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_adapter_reg_mem_access_special_swap_core_table_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_adapter_reg_mem_access_special_swap_core_table_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_reg_mem_access, dnx_data_adapter_reg_mem_access_table_special_swap_core_table);
    data = (const dnx_data_adapter_reg_mem_access_special_swap_core_table_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->type);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->arg);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_adapter_reg_mem_access_clear_on_read_mems_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_adapter_reg_mem_access_clear_on_read_mems_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_reg_mem_access, dnx_data_adapter_reg_mem_access_table_clear_on_read_mems);
    data = (const dnx_data_adapter_reg_mem_access_clear_on_read_mems_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mem);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_adapter_reg_mem_access_clear_on_read_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_adapter_reg_mem_access_clear_on_read_regs_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_reg_mem_access, dnx_data_adapter_reg_mem_access_table_clear_on_read_regs);
    data = (const dnx_data_adapter_reg_mem_access_clear_on_read_regs_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reg);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_adapter_reg_mem_access_swap_core_ignore_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_reg_mem_access, dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map);

}

const dnxc_data_table_info_t *
dnx_data_adapter_reg_mem_access_swap_core_ignore_map_by_global_block_ids_enum_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_reg_mem_access, dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map_by_global_block_ids_enum);

}

const dnxc_data_table_info_t *
dnx_data_adapter_reg_mem_access_special_swap_core_table_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_reg_mem_access, dnx_data_adapter_reg_mem_access_table_special_swap_core_table);

}

const dnxc_data_table_info_t *
dnx_data_adapter_reg_mem_access_clear_on_read_mems_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_reg_mem_access, dnx_data_adapter_reg_mem_access_table_clear_on_read_mems);

}

const dnxc_data_table_info_t *
dnx_data_adapter_reg_mem_access_clear_on_read_regs_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_reg_mem_access, dnx_data_adapter_reg_mem_access_table_clear_on_read_regs);

}






static shr_error_e
dnx_data_adapter_mdb_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "mdb";
    submodule_data->doc = "Adapter mdb info";
    
    submodule_data->nof_features = _dnx_data_adapter_mdb_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data adapter mdb features");

    
    submodule_data->nof_defines = _dnx_data_adapter_mdb_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data adapter mdb defines");

    
    submodule_data->nof_tables = _dnx_data_adapter_mdb_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data adapter mdb tables");

    
    submodule_data->tables[dnx_data_adapter_mdb_table_lookup_caller_id_mapping].name = "lookup_caller_id_mapping";
    submodule_data->tables[dnx_data_adapter_mdb_table_lookup_caller_id_mapping].doc = "";
    submodule_data->tables[dnx_data_adapter_mdb_table_lookup_caller_id_mapping].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_adapter_mdb_table_lookup_caller_id_mapping].size_of_values = sizeof(dnx_data_adapter_mdb_lookup_caller_id_mapping_t);
    submodule_data->tables[dnx_data_adapter_mdb_table_lookup_caller_id_mapping].entry_get = dnx_data_adapter_mdb_lookup_caller_id_mapping_entry_str_get;

    
    submodule_data->tables[dnx_data_adapter_mdb_table_lookup_caller_id_mapping].nof_keys = 2;
    submodule_data->tables[dnx_data_adapter_mdb_table_lookup_caller_id_mapping].keys[0].name = "caller_id";
    submodule_data->tables[dnx_data_adapter_mdb_table_lookup_caller_id_mapping].keys[0].doc = "";
    submodule_data->tables[dnx_data_adapter_mdb_table_lookup_caller_id_mapping].keys[1].name = "stage_mdb_key";
    submodule_data->tables[dnx_data_adapter_mdb_table_lookup_caller_id_mapping].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_adapter_mdb_table_lookup_caller_id_mapping].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_adapter_mdb_table_lookup_caller_id_mapping].values, dnxc_data_value_t, submodule_data->tables[dnx_data_adapter_mdb_table_lookup_caller_id_mapping].nof_values, "_dnx_data_adapter_mdb_table_lookup_caller_id_mapping table values");
    submodule_data->tables[dnx_data_adapter_mdb_table_lookup_caller_id_mapping].values[0].name = "stage";
    submodule_data->tables[dnx_data_adapter_mdb_table_lookup_caller_id_mapping].values[0].type = "int";
    submodule_data->tables[dnx_data_adapter_mdb_table_lookup_caller_id_mapping].values[0].doc = "";
    submodule_data->tables[dnx_data_adapter_mdb_table_lookup_caller_id_mapping].values[0].offset = UTILEX_OFFSETOF(dnx_data_adapter_mdb_lookup_caller_id_mapping_t, stage);
    submodule_data->tables[dnx_data_adapter_mdb_table_lookup_caller_id_mapping].values[1].name = "mdb";
    submodule_data->tables[dnx_data_adapter_mdb_table_lookup_caller_id_mapping].values[1].type = "int";
    submodule_data->tables[dnx_data_adapter_mdb_table_lookup_caller_id_mapping].values[1].doc = "";
    submodule_data->tables[dnx_data_adapter_mdb_table_lookup_caller_id_mapping].values[1].offset = UTILEX_OFFSETOF(dnx_data_adapter_mdb_lookup_caller_id_mapping_t, mdb);
    submodule_data->tables[dnx_data_adapter_mdb_table_lookup_caller_id_mapping].values[2].name = "interface";
    submodule_data->tables[dnx_data_adapter_mdb_table_lookup_caller_id_mapping].values[2].type = "char *";
    submodule_data->tables[dnx_data_adapter_mdb_table_lookup_caller_id_mapping].values[2].doc = "";
    submodule_data->tables[dnx_data_adapter_mdb_table_lookup_caller_id_mapping].values[2].offset = UTILEX_OFFSETOF(dnx_data_adapter_mdb_lookup_caller_id_mapping_t, interface);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_adapter_mdb_feature_get(
    int unit,
    dnx_data_adapter_mdb_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_mdb, feature);
}




const dnx_data_adapter_mdb_lookup_caller_id_mapping_t *
dnx_data_adapter_mdb_lookup_caller_id_mapping_get(
    int unit,
    int caller_id,
    int stage_mdb_key)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_mdb, dnx_data_adapter_mdb_table_lookup_caller_id_mapping);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, caller_id, stage_mdb_key);
    return (const dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) data;

}


shr_error_e
dnx_data_adapter_mdb_lookup_caller_id_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_adapter_mdb_lookup_caller_id_mapping_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_mdb, dnx_data_adapter_mdb_table_lookup_caller_id_mapping);
    data = (const dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->stage);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mdb);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->interface == NULL ? "" : data->interface);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_adapter_mdb_lookup_caller_id_mapping_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_mdb, dnx_data_adapter_mdb_table_lookup_caller_id_mapping);

}



shr_error_e
dnx_data_adapter_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "adapter";
    module_data->nof_submodules = _dnx_data_adapter_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data adapter submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_adapter_tx_init(unit, &module_data->submodules[dnx_data_adapter_submodule_tx]));
    SHR_IF_ERR_EXIT(dnx_data_adapter_rx_init(unit, &module_data->submodules[dnx_data_adapter_submodule_rx]));
    SHR_IF_ERR_EXIT(dnx_data_adapter_general_init(unit, &module_data->submodules[dnx_data_adapter_submodule_general]));
    SHR_IF_ERR_EXIT(dnx_data_adapter_reg_mem_access_init(unit, &module_data->submodules[dnx_data_adapter_submodule_reg_mem_access]));
    SHR_IF_ERR_EXIT(dnx_data_adapter_mdb_init(unit, &module_data->submodules[dnx_data_adapter_submodule_mdb]));
    
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_adapter_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_adapter_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_adapter_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_adapter_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_adapter_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_adapter_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_adapter_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_adapter_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_adapter_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_adapter_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_adapter_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_adapter_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_adapter_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_adapter_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_adapter_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_adapter_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_adapter_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_adapter_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_adapter_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_adapter_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_adapter_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_adapter_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_adapter_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_adapter_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_adapter_attach(unit));
    }
    else

#endif 
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

