
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
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
dnx_data_egr_queuing_v1_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "general data for egress queuing, mostly for feature support for dispatching purposes between v1 to other versions";
    
    submodule_data->nof_features = _dnx_data_egr_queuing_v1_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data egr_queuing_v1 general features");

    submodule_data->features[dnx_data_egr_queuing_v1_general_v1_supported].name = "v1_supported";
    submodule_data->features[dnx_data_egr_queuing_v1_general_v1_supported].doc = "inidication that v1 implementation is supported";
    submodule_data->features[dnx_data_egr_queuing_v1_general_v1_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_egr_queuing_v1_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data egr_queuing_v1 general defines");

    
    submodule_data->nof_tables = _dnx_data_egr_queuing_v1_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data egr_queuing_v1 general tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_egr_queuing_v1_general_feature_get(
    int unit,
    dnx_data_egr_queuing_v1_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_general, feature);
}











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

    submodule_data->defines[dnx_data_egr_queuing_v1_egress_interfaces_define_cpu_interface_nof_channels].name = "cpu_interface_nof_channels";
    submodule_data->defines[dnx_data_egr_queuing_v1_egress_interfaces_define_cpu_interface_nof_channels].doc = "max number of channels in CPU interface at the egress core - this is differnet than general number of CPU contexts supported by the CPU interface in the device level";
    
    submodule_data->defines[dnx_data_egr_queuing_v1_egress_interfaces_define_cpu_interface_nof_channels].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_v1_egress_interfaces_define_cpu_nof_tcs].name = "cpu_nof_tcs";
    submodule_data->defines[dnx_data_egr_queuing_v1_egress_interfaces_define_cpu_nof_tcs].doc = "max number of TCs a cpu port supports";
    
    submodule_data->defines[dnx_data_egr_queuing_v1_egress_interfaces_define_cpu_nof_tcs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_v1_egress_interfaces_define_rcy_interface_nof_channels].name = "rcy_interface_nof_channels";
    submodule_data->defines[dnx_data_egr_queuing_v1_egress_interfaces_define_rcy_interface_nof_channels].doc = "max number of channels in RCY interface at the egress core";
    
    submodule_data->defines[dnx_data_egr_queuing_v1_egress_interfaces_define_rcy_interface_nof_channels].flags |= DNXC_DATA_F_DEFINE;

    
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

    
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].values, dnxc_data_value_t, submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].nof_values, "_dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data table values");
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].values[0].name = "base_offset";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].values[0].type = "int";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].values[0].doc = "starting offset for itnerface type";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].values[0].offset = UTILEX_OFFSETOF(dnx_data_egr_queuing_v1_egress_interfaces_mapping_data_t, base_offset);
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].values[1].name = "count";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].values[1].type = "int";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].values[1].doc = "number of interface of interface type";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].values[1].offset = UTILEX_OFFSETOF(dnx_data_egr_queuing_v1_egress_interfaces_mapping_data_t, count);
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].values[2].name = "compensation";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].values[2].type = "int";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].values[2].doc = "default header compensation value";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_mapping_data].values[2].offset = UTILEX_OFFSETOF(dnx_data_egr_queuing_v1_egress_interfaces_mapping_data_t, compensation);

    
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].name = "per_speed_mapping_data";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].doc = "mapping data per egress interface type and interface speed";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].size_of_values = sizeof(dnx_data_egr_queuing_v1_egress_interfaces_per_speed_mapping_data_t);
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].entry_get = dnx_data_egr_queuing_v1_egress_interfaces_per_speed_mapping_data_entry_str_get;

    
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].nof_keys = 1;
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].keys[0].name = "index";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].keys[0].doc = "running index";

    
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].nof_values = 9;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values, dnxc_data_value_t, submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].nof_values, "_dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data table values");
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[0].name = "interface_type";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[0].type = "bcm_port_if_t";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[0].doc = "interface type for which the data is relevant, used as key de facto";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[0].offset = UTILEX_OFFSETOF(dnx_data_egr_queuing_v1_egress_interfaces_per_speed_mapping_data_t, interface_type);
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[1].name = "speed";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[1].type = "int";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[1].doc = "max speed for which the data is relevant, used as key de facto";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[1].offset = UTILEX_OFFSETOF(dnx_data_egr_queuing_v1_egress_interfaces_per_speed_mapping_data_t, speed);
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[2].name = "high_priority_th";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[2].type = "int";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[2].doc = "IRDY threshold per high priority TXQ";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[2].offset = UTILEX_OFFSETOF(dnx_data_egr_queuing_v1_egress_interfaces_per_speed_mapping_data_t, high_priority_th);
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[3].name = "low_priority_th";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[3].type = "int";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[3].doc = "IRDY threshold per low priority TXQ";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[3].offset = UTILEX_OFFSETOF(dnx_data_egr_queuing_v1_egress_interfaces_per_speed_mapping_data_t, low_priority_th);
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[4].name = "txi_irdy_th";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[4].type = "int";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[4].doc = "Define credit balance treshold below which interface is not eligable";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[4].offset = UTILEX_OFFSETOF(dnx_data_egr_queuing_v1_egress_interfaces_per_speed_mapping_data_t, txi_irdy_th);
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[5].name = "fqp_min_gap";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[5].type = "int";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[5].doc = "the minimal gap between consecutive commands to the same egress interface in fqp calendar";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[5].offset = UTILEX_OFFSETOF(dnx_data_egr_queuing_v1_egress_interfaces_per_speed_mapping_data_t, fqp_min_gap);
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[6].name = "eps_min_gap";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[6].type = "int";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[6].doc = "the minimal gap between consecutive commands to the same egress interface in eps calendar";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[6].offset = UTILEX_OFFSETOF(dnx_data_egr_queuing_v1_egress_interfaces_per_speed_mapping_data_t, eps_min_gap);
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[7].name = "allow_consecutive_select";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[7].type = "int";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[7].doc = "allow FQP to serve certain interface consecutively, even if there are other active interfaces";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[7].offset = UTILEX_OFFSETOF(dnx_data_egr_queuing_v1_egress_interfaces_per_speed_mapping_data_t, allow_consecutive_select);
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[8].name = "credit_size";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[8].type = "int";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[8].doc = "TXQ txi credit size";
    submodule_data->tables[dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data].values[8].offset = UTILEX_OFFSETOF(dnx_data_egr_queuing_v1_egress_interfaces_per_speed_mapping_data_t, credit_size);


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

uint32
dnx_data_egr_queuing_v1_egress_interfaces_cpu_interface_nof_channels_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_egress_interfaces, dnx_data_egr_queuing_v1_egress_interfaces_define_cpu_interface_nof_channels);
}

uint32
dnx_data_egr_queuing_v1_egress_interfaces_cpu_nof_tcs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_egress_interfaces, dnx_data_egr_queuing_v1_egress_interfaces_define_cpu_nof_tcs);
}

uint32
dnx_data_egr_queuing_v1_egress_interfaces_rcy_interface_nof_channels_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_egress_interfaces, dnx_data_egr_queuing_v1_egress_interfaces_define_rcy_interface_nof_channels);
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

const dnx_data_egr_queuing_v1_egress_interfaces_per_speed_mapping_data_t *
dnx_data_egr_queuing_v1_egress_interfaces_per_speed_mapping_data_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_egress_interfaces, dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_egr_queuing_v1_egress_interfaces_per_speed_mapping_data_t *) data;

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
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->compensation);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_egr_queuing_v1_egress_interfaces_per_speed_mapping_data_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_egr_queuing_v1_egress_interfaces_per_speed_mapping_data_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_egress_interfaces, dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data);
    data = (const dnx_data_egr_queuing_v1_egress_interfaces_per_speed_mapping_data_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->interface_type);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->speed);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->high_priority_th);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->low_priority_th);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->txi_irdy_th);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fqp_min_gap);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->eps_min_gap);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->allow_consecutive_select);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->credit_size);
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

const dnxc_data_table_info_t *
dnx_data_egr_queuing_v1_egress_interfaces_per_speed_mapping_data_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_egress_interfaces, dnx_data_egr_queuing_v1_egress_interfaces_table_per_speed_mapping_data);

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

    submodule_data->defines[dnx_data_egr_queuing_v1_egress_queues_define_nof_queue_pairs].name = "nof_queue_pairs";
    submodule_data->defines[dnx_data_egr_queuing_v1_egress_queues_define_nof_queue_pairs].doc = "number of qpairs";
    
    submodule_data->defines[dnx_data_egr_queuing_v1_egress_queues_define_nof_queue_pairs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_v1_egress_queues_define_total_nof_egr_queues].name = "total_nof_egr_queues";
    submodule_data->defines[dnx_data_egr_queuing_v1_egress_queues_define_total_nof_egr_queues].doc = "total number of egress queues, qpairs are counted as 2 queues";
    
    submodule_data->defines[dnx_data_egr_queuing_v1_egress_queues_define_total_nof_egr_queues].flags |= DNXC_DATA_F_DEFINE;

    
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

uint32
dnx_data_egr_queuing_v1_egress_queues_nof_queue_pairs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_egress_queues, dnx_data_egr_queuing_v1_egress_queues_define_nof_queue_pairs);
}

uint32
dnx_data_egr_queuing_v1_egress_queues_total_nof_egr_queues_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_egress_queues, dnx_data_egr_queuing_v1_egress_queues_define_total_nof_egr_queues);
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






static shr_error_e
dnx_data_egr_queuing_v1_scheduling_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "scheduling";
    submodule_data->doc = "egress scheduling related data";
    
    submodule_data->nof_features = _dnx_data_egr_queuing_v1_scheduling_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data egr_queuing_v1 scheduling features");

    
    submodule_data->nof_defines = _dnx_data_egr_queuing_v1_scheduling_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data egr_queuing_v1 scheduling defines");

    submodule_data->defines[dnx_data_egr_queuing_v1_scheduling_define_nof_interface_calendar_slots].name = "nof_interface_calendar_slots";
    submodule_data->defines[dnx_data_egr_queuing_v1_scheduling_define_nof_interface_calendar_slots].doc = "the number of slots in the interface calendar(mux)";
    
    submodule_data->defines[dnx_data_egr_queuing_v1_scheduling_define_nof_interface_calendar_slots].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_egr_queuing_v1_scheduling_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data egr_queuing_v1 scheduling tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_egr_queuing_v1_scheduling_feature_get(
    int unit,
    dnx_data_egr_queuing_v1_scheduling_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_scheduling, feature);
}


uint32
dnx_data_egr_queuing_v1_scheduling_nof_interface_calendar_slots_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_scheduling, dnx_data_egr_queuing_v1_scheduling_define_nof_interface_calendar_slots);
}










static shr_error_e
dnx_data_egr_queuing_v1_shaping_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "shaping";
    submodule_data->doc = "egress shaping related data";
    
    submodule_data->nof_features = _dnx_data_egr_queuing_v1_shaping_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data egr_queuing_v1 shaping features");

    
    submodule_data->nof_defines = _dnx_data_egr_queuing_v1_shaping_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data egr_queuing_v1 shaping defines");

    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_default_max_burst].name = "default_max_burst";
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_default_max_burst].doc = "default max burst size to be used in shapers if user didn't used API to define it, common value is order of magnitude of MTU size ~16k";
    
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_default_max_burst].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_max_max_burst_value].name = "max_max_burst_value";
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_max_max_burst_value].doc = "max value max burst can get";
    
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_max_max_burst_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_max_queue_max_burst_value].name = "max_queue_max_burst_value";
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_max_queue_max_burst_value].doc = "max value for queue max burst configuration";
    
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_max_queue_max_burst_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_max_interface_credits_value].name = "max_interface_credits_value";
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_max_interface_credits_value].doc = "max value for interface credits";
    
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_max_interface_credits_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_max_port_credits_value].name = "max_port_credits_value";
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_max_port_credits_value].doc = "max value for port credits";
    
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_max_port_credits_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_slots_per_clock].name = "slots_per_clock";
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_slots_per_clock].doc = "number of calendar slots per clock";
    
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_slots_per_clock].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_credits_per_bit].name = "credits_per_bit";
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_credits_per_bit].doc = "number of credits per bit of traffic, used in shaping calendar's rate calculation";
    
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_credits_per_bit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_credits_per_packet].name = "credits_per_packet";
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_credits_per_packet].doc = "number of credits each packet shpuld be assighned in shaping calendar's rate calculation";
    
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_credits_per_packet].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_calcal_channelized_entries].name = "calcal_channelized_entries";
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_calcal_channelized_entries].doc = "number of entries of channelized calendar";
    
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_calcal_channelized_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_calcal_non_channelized_entries].name = "calcal_non_channelized_entries";
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_calcal_non_channelized_entries].doc = "number of entries of non channelized calendar";
    
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_calcal_non_channelized_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_calcal_length].name = "calcal_length";
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_calcal_length].doc = "calcal shaper numbers of all entries";
    
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_calcal_length].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_nof_port_shaper_entries].name = "nof_port_shaper_entries";
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_nof_port_shaper_entries].doc = "number of entries in port calendar shaper";
    
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_nof_port_shaper_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_port_cal_unused_entry].name = "port_cal_unused_entry";
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_port_cal_unused_entry].doc = "port calendar entry saved for unused interface calendars";
    
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_port_cal_unused_entry].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_port_shaper_start_entry].name = "port_shaper_start_entry";
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_port_shaper_start_entry].doc = "entry index from which the port calendar will start";
    
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_port_shaper_start_entry].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_nof_queue_shaper_entries].name = "nof_queue_shaper_entries";
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_nof_queue_shaper_entries].doc = "number of entries in queue calendar shaper";
    
    submodule_data->defines[dnx_data_egr_queuing_v1_shaping_define_nof_queue_shaper_entries].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_egr_queuing_v1_shaping_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data egr_queuing_v1 shaping tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_egr_queuing_v1_shaping_feature_get(
    int unit,
    dnx_data_egr_queuing_v1_shaping_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_shaping, feature);
}


uint32
dnx_data_egr_queuing_v1_shaping_default_max_burst_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_shaping, dnx_data_egr_queuing_v1_shaping_define_default_max_burst);
}

uint32
dnx_data_egr_queuing_v1_shaping_max_max_burst_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_shaping, dnx_data_egr_queuing_v1_shaping_define_max_max_burst_value);
}

uint32
dnx_data_egr_queuing_v1_shaping_max_queue_max_burst_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_shaping, dnx_data_egr_queuing_v1_shaping_define_max_queue_max_burst_value);
}

uint32
dnx_data_egr_queuing_v1_shaping_max_interface_credits_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_shaping, dnx_data_egr_queuing_v1_shaping_define_max_interface_credits_value);
}

uint32
dnx_data_egr_queuing_v1_shaping_max_port_credits_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_shaping, dnx_data_egr_queuing_v1_shaping_define_max_port_credits_value);
}

uint32
dnx_data_egr_queuing_v1_shaping_slots_per_clock_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_shaping, dnx_data_egr_queuing_v1_shaping_define_slots_per_clock);
}

uint32
dnx_data_egr_queuing_v1_shaping_credits_per_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_shaping, dnx_data_egr_queuing_v1_shaping_define_credits_per_bit);
}

uint32
dnx_data_egr_queuing_v1_shaping_credits_per_packet_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_shaping, dnx_data_egr_queuing_v1_shaping_define_credits_per_packet);
}

uint32
dnx_data_egr_queuing_v1_shaping_calcal_channelized_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_shaping, dnx_data_egr_queuing_v1_shaping_define_calcal_channelized_entries);
}

uint32
dnx_data_egr_queuing_v1_shaping_calcal_non_channelized_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_shaping, dnx_data_egr_queuing_v1_shaping_define_calcal_non_channelized_entries);
}

uint32
dnx_data_egr_queuing_v1_shaping_calcal_length_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_shaping, dnx_data_egr_queuing_v1_shaping_define_calcal_length);
}

uint32
dnx_data_egr_queuing_v1_shaping_nof_port_shaper_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_shaping, dnx_data_egr_queuing_v1_shaping_define_nof_port_shaper_entries);
}

uint32
dnx_data_egr_queuing_v1_shaping_port_cal_unused_entry_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_shaping, dnx_data_egr_queuing_v1_shaping_define_port_cal_unused_entry);
}

uint32
dnx_data_egr_queuing_v1_shaping_port_shaper_start_entry_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_shaping, dnx_data_egr_queuing_v1_shaping_define_port_shaper_start_entry);
}

uint32
dnx_data_egr_queuing_v1_shaping_nof_queue_shaper_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_egr_queuing_v1, dnx_data_egr_queuing_v1_submodule_shaping, dnx_data_egr_queuing_v1_shaping_define_nof_queue_shaper_entries);
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

    
    SHR_IF_ERR_EXIT(dnx_data_egr_queuing_v1_general_init(unit, &module_data->submodules[dnx_data_egr_queuing_v1_submodule_general]));
    SHR_IF_ERR_EXIT(dnx_data_egr_queuing_v1_egress_interfaces_init(unit, &module_data->submodules[dnx_data_egr_queuing_v1_submodule_egress_interfaces]));
    SHR_IF_ERR_EXIT(dnx_data_egr_queuing_v1_egress_queues_init(unit, &module_data->submodules[dnx_data_egr_queuing_v1_submodule_egress_queues]));
    SHR_IF_ERR_EXIT(dnx_data_egr_queuing_v1_scheduling_init(unit, &module_data->submodules[dnx_data_egr_queuing_v1_submodule_scheduling]));
    SHR_IF_ERR_EXIT(dnx_data_egr_queuing_v1_shaping_init(unit, &module_data->submodules[dnx_data_egr_queuing_v1_submodule_shaping]));
    
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
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_egr_queuing_v1_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2x_a0(unit))
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

