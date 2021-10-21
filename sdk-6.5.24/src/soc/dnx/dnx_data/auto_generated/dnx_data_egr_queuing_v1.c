
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_EGRESSDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_egr_queuing_v1.h>



#ifdef BCM_DNX2_SUPPORT

extern shr_error_e jr2_a0_data_egr_queuing_v1_attach(
    int unit);

#endif 



static shr_error_e
dnx_data_egr_queuing_v1_egress_interfaces_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "egress_interfaces";
    submodule_data->doc = "relevant data for egress interface";
    
    submodule_data->nof_features = _dnx_data_egr_queuing_v1_egress_interfaces_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data egr_queuing_v1 egress_interfaces features");

    submodule_data->features[dnx_data_egr_queuing_v1_egress_interfaces_channelized_interface_mapping].name = "channelized_interface_mapping";
    submodule_data->features[dnx_data_egr_queuing_v1_egress_interfaces_channelized_interface_mapping].doc = "channelized interface are mapped in a seperated pool. channelized interfaces has 2 allocation, one in the main pool and another in the channelized interfaces pool";
    submodule_data->features[dnx_data_egr_queuing_v1_egress_interfaces_channelized_interface_mapping].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_egr_queuing_v1_egress_interfaces_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data egr_queuing_v1 egress_interfaces defines");

    submodule_data->defines[dnx_data_egr_queuing_v1_egress_interfaces_define_nof_egr_interfaces].name = "nof_egr_interfaces";
    submodule_data->defines[dnx_data_egr_queuing_v1_egress_interfaces_define_nof_egr_interfaces].doc = "number of egress interfaces";
    
    submodule_data->defines[dnx_data_egr_queuing_v1_egress_interfaces_define_nof_egr_interfaces].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_v1_egress_interfaces_define_nof_channelized_egr_interfaces].name = "nof_channelized_egr_interfaces";
    submodule_data->defines[dnx_data_egr_queuing_v1_egress_interfaces_define_nof_channelized_egr_interfaces].doc = "number of channelized egress itnerfaces, a channelized interface has both channelized ID and regular ID";
    
    submodule_data->defines[dnx_data_egr_queuing_v1_egress_interfaces_define_nof_channelized_egr_interfaces].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_egr_queuing_v1_egress_interfaces_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data egr_queuing_v1 egress_interfaces tables");

    
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].name = "mapping_data";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].doc = "mapping data per egress interface type";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].size_of_values = sizeof(dnx_data_egr_queuing_v1_egress_interfaces_mapping_data_t);
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].entry_get = dnx_data_egr_queuing_v1_egress_interfaces_mapping_data_entry_str_get;

    
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].nof_keys = 1;
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].keys[0].name = "interface_type";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].keys[0].doc = "interface type";

    
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].values, dnxc_data_value_t, submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].nof_values, "_dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data table values");
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].values[0].name = "base_offset";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].values[0].type = "int";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].values[0].doc = "starting offset for itnerface type";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].values[0].offset = UTILEX_OFFSETOF(dnx_data_egr_queuing_v1_egress_interfaces_mapping_data_t, base_offset);
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].values[1].name = "count";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].values[1].type = "int";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].values[1].doc = "number of interface of interface type";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].values[1].offset = UTILEX_OFFSETOF(dnx_data_egr_queuing_v1_egress_interfaces_mapping_data_t, count);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_egr_queuing_v1_egress_interfaces_feature_get(
    int unit,
    dnx_data_egr_queuing_v1_egress_interfaces_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_egress_interfaces, feature);
}


uint32
dnx_data_egr_queuing_v1_egress_interfaces_nof_egr_interfaces_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_egress_interfaces, dnx_data_egr_queuing_v1_egress_interfaces_define_nof_egr_interfaces);
}

uint32
dnx_data_egr_queuing_v1_egress_interfaces_nof_channelized_egr_interfaces_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_egress_interfaces, dnx_data_egr_queuing_v1_egress_interfaces_define_nof_channelized_egr_interfaces);
}



const dnx_data_egr_queuing_v1_egress_interfaces_mapping_data_t *
dnx_data_egr_queuing_v1_egress_interfaces_mapping_data_get(
    int unit,
    int interface_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_egress_interfaces, dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, interface_type, 0);
    return (const dnx_data_egr_queuing_v1_egress_interfaces_mapping_data_t *) data;

}


shr_error_e
dnx_data_egr_queuing_v1_egress_interfaces_mapping_data_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_egr_queuing_v1_egress_interfaces_mapping_data_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_egress_interfaces, dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data);
    data = (const dnx_data_egr_queuing_v1_egress_interfaces_mapping_data_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->base_offset);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->count);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_egr_queuing_v1_egress_interfaces_mapping_data_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_egress_interfaces, dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data);

}






static shr_error_e
dnx_data_egr_queuing_v1_egress_queues_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "egress_queues";
    submodule_data->doc = "relevant data for egress queues";
    
    submodule_data->nof_features = _dnx_data_egr_queuing_v1_egress_queues_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data egr_queuing_v1 egress_queues features");

    submodule_data->features[dnx_data_egr_queuing_v1_egress_queues_rcy_reserved_queues].name = "rcy_reserved_queues";
    submodule_data->features[dnx_data_egr_queuing_v1_egress_queues_rcy_reserved_queues].doc = "RCY has dedicated reserved queues";
    submodule_data->features[dnx_data_egr_queuing_v1_egress_queues_rcy_reserved_queues].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_egr_queuing_v1_egress_queues_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data egr_queuing_v1 egress_queues defines");

    submodule_data->defines[dnx_data_egr_queuing_v1_egress_queues_define_nof_egr_queues].name = "nof_egr_queues";
    submodule_data->defines[dnx_data_egr_queuing_v1_egress_queues_define_nof_egr_queues].doc = "number of egress queues, qpairs are coutned as a single queue";
    
    submodule_data->defines[dnx_data_egr_queuing_v1_egress_queues_define_nof_egr_queues].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_v1_egress_queues_define_nof_egr_priorities].name = "nof_egr_priorities";
    submodule_data->defines[dnx_data_egr_queuing_v1_egress_queues_define_nof_egr_priorities].doc = "number of egress priorities";
    
    submodule_data->defines[dnx_data_egr_queuing_v1_egress_queues_define_nof_egr_priorities].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_v1_egress_queues_define_ps_queue_resolution].name = "ps_queue_resolution";
    submodule_data->defines[dnx_data_egr_queuing_v1_egress_queues_define_ps_queue_resolution].doc = "the resolution of queues in port scheduler - on how many queues a PS element controls";
    
    submodule_data->defines[dnx_data_egr_queuing_v1_egress_queues_define_ps_queue_resolution].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_v1_egress_queues_define_rcy_reserved_queue].name = "rcy_reserved_queue";
    submodule_data->defines[dnx_data_egr_queuing_v1_egress_queues_define_rcy_reserved_queue].doc = "the reserved base queue ID for recycle port";
    
    submodule_data->defines[dnx_data_egr_queuing_v1_egress_queues_define_rcy_reserved_queue].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_egr_queuing_v1_egress_queues_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data egr_queuing_v1 egress_queues tables");

    
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_queues_table_supported_nof_priorities].name = "supported_nof_priorities";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_queues_table_supported_nof_priorities].doc = "supported number of priorities";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_queues_table_supported_nof_priorities].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_queues_table_supported_nof_priorities].size_of_values = sizeof(dnx_data_egr_queuing_v1_egress_queues_supported_nof_priorities_t);
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_queues_table_supported_nof_priorities].entry_get = dnx_data_egr_queuing_v1_egress_queues_supported_nof_priorities_entry_str_get;

    
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_queues_table_supported_nof_priorities].nof_keys = 1;
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_queues_table_supported_nof_priorities].keys[0].name = "priorities";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_queues_table_supported_nof_priorities].keys[0].doc = "number of priorities";

    
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_queues_table_supported_nof_priorities].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_egr_queuing_v1_egress_queues_table_supported_nof_priorities].values, dnxc_data_value_t, submodule_data->tables[dnx_data_egr_queuing_v1_egress_queues_table_supported_nof_priorities].nof_values, "_dnx_data_egr_queuing_v1_egress_queues_table_supported_nof_priorities table values");
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_queues_table_supported_nof_priorities].values[0].name = "supported";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_queues_table_supported_nof_priorities].values[0].type = "int";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_queues_table_supported_nof_priorities].values[0].doc = "the number of priorities is supported";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_queues_table_supported_nof_priorities].values[0].offset = UTILEX_OFFSETOF(dnx_data_egr_queuing_v1_egress_queues_supported_nof_priorities_t, supported);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_egr_queuing_v1_egress_queues_feature_get(
    int unit,
    dnx_data_egr_queuing_v1_egress_queues_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_egress_queues, feature);
}


uint32
dnx_data_egr_queuing_v1_egress_queues_nof_egr_queues_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_egress_queues, dnx_data_egr_queuing_v1_egress_queues_define_nof_egr_queues);
}

uint32
dnx_data_egr_queuing_v1_egress_queues_nof_egr_priorities_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_egress_queues, dnx_data_egr_queuing_v1_egress_queues_define_nof_egr_priorities);
}

uint32
dnx_data_egr_queuing_v1_egress_queues_ps_queue_resolution_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_egress_queues, dnx_data_egr_queuing_v1_egress_queues_define_ps_queue_resolution);
}

uint32
dnx_data_egr_queuing_v1_egress_queues_rcy_reserved_queue_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_egress_queues, dnx_data_egr_queuing_v1_egress_queues_define_rcy_reserved_queue);
}



const dnx_data_egr_queuing_v1_egress_queues_supported_nof_priorities_t *
dnx_data_egr_queuing_v1_egress_queues_supported_nof_priorities_get(
    int unit,
    int priorities)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_egress_queues, dnx_data_egr_queuing_v1_egress_queues_table_supported_nof_priorities);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, priorities, 0);
    return (const dnx_data_egr_queuing_v1_egress_queues_supported_nof_priorities_t *) data;

}


shr_error_e
dnx_data_egr_queuing_v1_egress_queues_supported_nof_priorities_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_egr_queuing_v1_egress_queues_supported_nof_priorities_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_egress_queues, dnx_data_egr_queuing_v1_egress_queues_table_supported_nof_priorities);
    data = (const dnx_data_egr_queuing_v1_egress_queues_supported_nof_priorities_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->supported);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_egr_queuing_v1_egress_queues_supported_nof_priorities_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_egress_queues, dnx_data_egr_queuing_v1_egress_queues_table_supported_nof_priorities);

}



shr_error_e
dnx_data_egr_queuing_v1_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "egr_queuing_v1";
    module_data->nof_submodules = _dnx_data_egr_queuing_v1_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data egr_queuing_v1 submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_egr_queuing_v1_egress_interfaces_init(unit, &module_data->submodules[dnx_data_egr_queuing_v1_submodule_egress_interfaces]));
    SHR_IF_ERR_EXIT(dnx_data_egr_queuing_v1_egress_queues_init(unit, &module_data->submodules[dnx_data_egr_queuing_v1_submodule_egress_queues]));
    
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_egr_queuing_v1_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_egr_queuing_v1_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_egr_queuing_v1_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_egr_queuing_v1_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_egr_queuing_v1_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_egr_queuing_v1_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_egr_queuing_v1_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_egr_queuing_v1_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_egr_queuing_v1_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_egr_queuing_v1_attach(unit));
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

