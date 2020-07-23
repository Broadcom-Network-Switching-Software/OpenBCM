

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_ACCESSDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_access.h>



extern shr_error_e jr2_a0_data_access_attach(
    int unit);
extern shr_error_e j2c_a0_data_access_attach(
    int unit);
extern shr_error_e q2a_a0_data_access_attach(
    int unit);



static shr_error_e
dnx_data_access_mdio_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "mdio";
    submodule_data->doc = "mdio related data";
    
    submodule_data->nof_features = _dnx_data_access_mdio_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data access mdio features");

    
    submodule_data->nof_defines = _dnx_data_access_mdio_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data access mdio defines");

    submodule_data->defines[dnx_data_access_mdio_define_ext_divisor].name = "ext_divisor";
    submodule_data->defines[dnx_data_access_mdio_define_ext_divisor].doc = "external MDIO clock rate divisor";
    submodule_data->defines[dnx_data_access_mdio_define_ext_divisor].labels[0] = "j2p_notrev";
    
    submodule_data->defines[dnx_data_access_mdio_define_ext_divisor].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_access_mdio_define_int_divisor].name = "int_divisor";
    submodule_data->defines[dnx_data_access_mdio_define_int_divisor].doc = "internal MDIO clock rate divisor";
    submodule_data->defines[dnx_data_access_mdio_define_int_divisor].labels[0] = "j2p_notrev";
    
    submodule_data->defines[dnx_data_access_mdio_define_int_divisor].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_access_mdio_define_int_div_out_delay].name = "int_div_out_delay";
    submodule_data->defines[dnx_data_access_mdio_define_int_div_out_delay].doc = "number of clock delay between the rising edge of MDC and the starting data of MDIO for internal divisor";
    submodule_data->defines[dnx_data_access_mdio_define_int_div_out_delay].labels[0] = "j2p_notrev";
    
    submodule_data->defines[dnx_data_access_mdio_define_int_div_out_delay].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_access_mdio_define_ext_div_out_delay].name = "ext_div_out_delay";
    submodule_data->defines[dnx_data_access_mdio_define_ext_div_out_delay].doc = "number of clock delay between the rising edge of MDC and the starting data of MDIO for external divisor";
    submodule_data->defines[dnx_data_access_mdio_define_ext_div_out_delay].labels[0] = "j2p_notrev";
    
    submodule_data->defines[dnx_data_access_mdio_define_ext_div_out_delay].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_access_mdio_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data access mdio tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_access_mdio_feature_get(
    int unit,
    dnx_data_access_mdio_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_access, dnx_data_access_submodule_mdio, feature);
}


uint32
dnx_data_access_mdio_ext_divisor_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_access, dnx_data_access_submodule_mdio, dnx_data_access_mdio_define_ext_divisor);
}

uint32
dnx_data_access_mdio_int_divisor_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_access, dnx_data_access_submodule_mdio, dnx_data_access_mdio_define_int_divisor);
}

uint32
dnx_data_access_mdio_int_div_out_delay_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_access, dnx_data_access_submodule_mdio, dnx_data_access_mdio_define_int_div_out_delay);
}

uint32
dnx_data_access_mdio_ext_div_out_delay_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_access, dnx_data_access_submodule_mdio, dnx_data_access_mdio_define_ext_div_out_delay);
}










static shr_error_e
dnx_data_access_rcpu_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "rcpu";
    submodule_data->doc = "rcpu related data";
    
    submodule_data->nof_features = _dnx_data_access_rcpu_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data access rcpu features");

    
    submodule_data->nof_defines = _dnx_data_access_rcpu_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data access rcpu defines");

    
    submodule_data->nof_tables = _dnx_data_access_rcpu_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data access rcpu tables");

    
    submodule_data->tables[dnx_data_access_rcpu_table_rx].name = "rx";
    submodule_data->tables[dnx_data_access_rcpu_table_rx].doc = "general data about rx";
    submodule_data->tables[dnx_data_access_rcpu_table_rx].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_access_rcpu_table_rx].size_of_values = sizeof(dnx_data_access_rcpu_rx_t);
    submodule_data->tables[dnx_data_access_rcpu_table_rx].entry_get = dnx_data_access_rcpu_rx_entry_str_get;

    
    submodule_data->tables[dnx_data_access_rcpu_table_rx].nof_keys = 0;

    
    submodule_data->tables[dnx_data_access_rcpu_table_rx].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_access_rcpu_table_rx].values, dnxc_data_value_t, submodule_data->tables[dnx_data_access_rcpu_table_rx].nof_values, "_dnx_data_access_rcpu_table_rx table values");
    submodule_data->tables[dnx_data_access_rcpu_table_rx].values[0].name = "pbmp";
    submodule_data->tables[dnx_data_access_rcpu_table_rx].values[0].type = "bcm_pbmp_t";
    submodule_data->tables[dnx_data_access_rcpu_table_rx].values[0].doc = "valid ports on which RCPU packets can be received by slave device";
    submodule_data->tables[dnx_data_access_rcpu_table_rx].values[0].offset = UTILEX_OFFSETOF(dnx_data_access_rcpu_rx_t, pbmp);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_access_rcpu_feature_get(
    int unit,
    dnx_data_access_rcpu_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_access, dnx_data_access_submodule_rcpu, feature);
}




const dnx_data_access_rcpu_rx_t *
dnx_data_access_rcpu_rx_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_access, dnx_data_access_submodule_rcpu, dnx_data_access_rcpu_table_rx);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_access_rcpu_rx_t *) data;

}


shr_error_e
dnx_data_access_rcpu_rx_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_access_rcpu_rx_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_access, dnx_data_access_submodule_rcpu, dnx_data_access_rcpu_table_rx);
    data = (const dnx_data_access_rcpu_rx_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_PBMP_STR(buffer, data->pbmp);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_access_rcpu_rx_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_access, dnx_data_access_submodule_rcpu, dnx_data_access_rcpu_table_rx);

}






static shr_error_e
dnx_data_access_blocks_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "blocks";
    submodule_data->doc = "HW blocks related data";
    
    submodule_data->nof_features = _dnx_data_access_blocks_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data access blocks features");

    
    submodule_data->nof_defines = _dnx_data_access_blocks_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data access blocks defines");

    
    submodule_data->nof_tables = _dnx_data_access_blocks_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data access blocks tables");

    
    submodule_data->tables[dnx_data_access_blocks_table_override].name = "override";
    submodule_data->tables[dnx_data_access_blocks_table_override].doc = "blocks enable/disable override";
    submodule_data->tables[dnx_data_access_blocks_table_override].labels[0] = "j2p_notrev";
    submodule_data->tables[dnx_data_access_blocks_table_override].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_access_blocks_table_override].size_of_values = sizeof(dnx_data_access_blocks_override_t);
    submodule_data->tables[dnx_data_access_blocks_table_override].entry_get = dnx_data_access_blocks_override_entry_str_get;

    
    submodule_data->tables[dnx_data_access_blocks_table_override].nof_keys = 1;
    submodule_data->tables[dnx_data_access_blocks_table_override].keys[0].name = "index";
    submodule_data->tables[dnx_data_access_blocks_table_override].keys[0].doc = "running index";

    
    submodule_data->tables[dnx_data_access_blocks_table_override].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_access_blocks_table_override].values, dnxc_data_value_t, submodule_data->tables[dnx_data_access_blocks_table_override].nof_values, "_dnx_data_access_blocks_table_override table values");
    submodule_data->tables[dnx_data_access_blocks_table_override].values[0].name = "block_type";
    submodule_data->tables[dnx_data_access_blocks_table_override].values[0].type = "char *";
    submodule_data->tables[dnx_data_access_blocks_table_override].values[0].doc = "See soc_block_type_t";
    submodule_data->tables[dnx_data_access_blocks_table_override].values[0].offset = UTILEX_OFFSETOF(dnx_data_access_blocks_override_t, block_type);
    submodule_data->tables[dnx_data_access_blocks_table_override].values[1].name = "block_instance";
    submodule_data->tables[dnx_data_access_blocks_table_override].values[1].type = "int";
    submodule_data->tables[dnx_data_access_blocks_table_override].values[1].doc = "instance ID within block type";
    submodule_data->tables[dnx_data_access_blocks_table_override].values[1].offset = UTILEX_OFFSETOF(dnx_data_access_blocks_override_t, block_instance);
    submodule_data->tables[dnx_data_access_blocks_table_override].values[2].name = "value";
    submodule_data->tables[dnx_data_access_blocks_table_override].values[2].type = "int";
    submodule_data->tables[dnx_data_access_blocks_table_override].values[2].doc = "The value to override";
    submodule_data->tables[dnx_data_access_blocks_table_override].values[2].offset = UTILEX_OFFSETOF(dnx_data_access_blocks_override_t, value);

    
    submodule_data->tables[dnx_data_access_blocks_table_soft_init].name = "soft_init";
    submodule_data->tables[dnx_data_access_blocks_table_soft_init].doc = "soft_init info";
    submodule_data->tables[dnx_data_access_blocks_table_soft_init].labels[0] = "j2p_notrev";
    submodule_data->tables[dnx_data_access_blocks_table_soft_init].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_access_blocks_table_soft_init].size_of_values = sizeof(dnx_data_access_blocks_soft_init_t);
    submodule_data->tables[dnx_data_access_blocks_table_soft_init].entry_get = dnx_data_access_blocks_soft_init_entry_str_get;

    
    submodule_data->tables[dnx_data_access_blocks_table_soft_init].nof_keys = 0;

    
    submodule_data->tables[dnx_data_access_blocks_table_soft_init].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_access_blocks_table_soft_init].values, dnxc_data_value_t, submodule_data->tables[dnx_data_access_blocks_table_soft_init].nof_values, "_dnx_data_access_blocks_table_soft_init table values");
    submodule_data->tables[dnx_data_access_blocks_table_soft_init].values[0].name = "trigger_value";
    submodule_data->tables[dnx_data_access_blocks_table_soft_init].values[0].type = "soc_pbmp_t";
    submodule_data->tables[dnx_data_access_blocks_table_soft_init].values[0].doc = "Value to be written to initialize soft init";
    submodule_data->tables[dnx_data_access_blocks_table_soft_init].values[0].offset = UTILEX_OFFSETOF(dnx_data_access_blocks_soft_init_t, trigger_value);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_access_blocks_feature_get(
    int unit,
    dnx_data_access_blocks_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_access, dnx_data_access_submodule_blocks, feature);
}




const dnx_data_access_blocks_override_t *
dnx_data_access_blocks_override_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_access, dnx_data_access_submodule_blocks, dnx_data_access_blocks_table_override);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_access_blocks_override_t *) data;

}

const dnx_data_access_blocks_soft_init_t *
dnx_data_access_blocks_soft_init_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_access, dnx_data_access_submodule_blocks, dnx_data_access_blocks_table_soft_init);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_access_blocks_soft_init_t *) data;

}


shr_error_e
dnx_data_access_blocks_override_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_access_blocks_override_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_access, dnx_data_access_submodule_blocks, dnx_data_access_blocks_table_override);
    data = (const dnx_data_access_blocks_override_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->block_type == NULL ? "" : data->block_type);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->block_instance);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->value);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_access_blocks_soft_init_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_access_blocks_soft_init_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_access, dnx_data_access_submodule_blocks, dnx_data_access_blocks_table_soft_init);
    data = (const dnx_data_access_blocks_soft_init_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_PBMP_STR(buffer, data->trigger_value);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_access_blocks_override_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_access, dnx_data_access_submodule_blocks, dnx_data_access_blocks_table_override);

}

const dnxc_data_table_info_t *
dnx_data_access_blocks_soft_init_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_access, dnx_data_access_submodule_blocks, dnx_data_access_blocks_table_soft_init);

}



shr_error_e
dnx_data_access_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "access";
    module_data->nof_submodules = _dnx_data_access_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data access submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_access_mdio_init(unit, &module_data->submodules[dnx_data_access_submodule_mdio]));
    SHR_IF_ERR_EXIT(dnx_data_access_rcpu_init(unit, &module_data->submodules[dnx_data_access_submodule_rcpu]));
    SHR_IF_ERR_EXIT(dnx_data_access_blocks_init(unit, &module_data->submodules[dnx_data_access_submodule_blocks]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_access_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_access_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_access_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_access_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_access_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_access_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_access_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_access_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_access_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_access_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_access_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_access_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_access_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_access_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

