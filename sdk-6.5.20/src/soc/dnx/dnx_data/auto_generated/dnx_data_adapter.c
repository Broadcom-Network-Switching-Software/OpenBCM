

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_adapter.h>



extern shr_error_e jr2_a0_data_adapter_attach(
    int unit);
extern shr_error_e jr2_b0_data_adapter_attach(
    int unit);
extern shr_error_e j2c_a0_data_adapter_attach(
    int unit);
extern shr_error_e q2a_a0_data_adapter_attach(
    int unit);
extern shr_error_e q2a_b0_data_adapter_attach(
    int unit);
extern shr_error_e j2p_a0_data_adapter_attach(
    int unit);



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

    submodule_data->defines[dnx_data_adapter_rx_define_packet_header_signal_id].name = "packet_header_signal_id";
    submodule_data->defines[dnx_data_adapter_rx_define_packet_header_signal_id].doc = "The adater signal id to identify the packet header";
    
    submodule_data->defines[dnx_data_adapter_rx_define_packet_header_signal_id].flags |= DNXC_DATA_F_NUMERIC;

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
dnx_data_adapter_rx_packet_header_signal_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_adapter, dnx_data_adapter_submodule_rx, dnx_data_adapter_rx_define_packet_header_signal_id);
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

    
    submodule_data->nof_tables = _dnx_data_adapter_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data adapter general tables");


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

    
    submodule_data->nof_tables = _dnx_data_adapter_reg_mem_access_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data adapter reg_mem_access tables");


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
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_adapter_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_adapter_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_adapter_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_adapter_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_adapter_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_adapter_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_adapter_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_adapter_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_adapter_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_adapter_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_adapter_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_adapter_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_adapter_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_adapter_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_adapter_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_adapter_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_adapter_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_adapter_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_adapter_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

