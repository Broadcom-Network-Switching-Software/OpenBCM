/** \file dnx_data_ingr_reassembly.c
 * 
 * MODULE DEVICE DATA - INGR_REASSEMBLY
 * 
 * Device Data
 * SW component that maintains per device data
 * For additional details about Device Data Component goto 'dnxc_data_mgmt.h'
 *        
 *     
 * 
 * AUTO-GENERATED BY AUTOCODER!
 * DO NOT EDIT THIS FILE!
 */
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT
/*
 * INCLUDE FILES:
 * {
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_ingr_reassembly.h>
/*
 * }
 */

/*
 * Extern per device attach function
 */
#ifdef BCM_DNX2_SUPPORT
/*{*/
extern shr_error_e jr2_a0_data_ingr_reassembly_attach(
    int unit);
/*}*/
#endif /*BCM_DNX2_SUPPORT*/
#ifdef BCM_DNX2_SUPPORT
/*{*/
extern shr_error_e j2c_a0_data_ingr_reassembly_attach(
    int unit);
/*}*/
#endif /*BCM_DNX2_SUPPORT*/
#ifdef BCM_DNX2_SUPPORT
/*{*/
extern shr_error_e q2a_a0_data_ingr_reassembly_attach(
    int unit);
/*}*/
#endif /*BCM_DNX2_SUPPORT*/
#ifdef BCM_DNX2_SUPPORT
/*{*/
extern shr_error_e j2p_a0_data_ingr_reassembly_attach(
    int unit);
/*}*/
#endif /*BCM_DNX2_SUPPORT*/
/*
 * SUBMODULE - CONTEXT:
 * {
 */
/*
 * context init
 */
/**
 * \brief Set submodule data to default value per device type
 * 
 * \param [in] unit - Unit #
 * \param [out] submodule_data - pointer to submodule data
 * 
 * \return
 *     rv - see 'shr_error_e
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
static shr_error_e
dnx_data_ingr_reassembly_context_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "context";
    submodule_data->doc = "Ingress context data";
    /*
     * Features
     */
    submodule_data->nof_features = _dnx_data_ingr_reassembly_context_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ingr_reassembly context features");

    submodule_data->features[dnx_data_ingr_reassembly_context_rcy_interleaving_between_priorities].name = "rcy_interleaving_between_priorities";
    submodule_data->features[dnx_data_ingr_reassembly_context_rcy_interleaving_between_priorities].doc = "RCY interleaving between priorities";
    submodule_data->features[dnx_data_ingr_reassembly_context_rcy_interleaving_between_priorities].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_ingr_reassembly_context_direct_context_table].name = "direct_context_table";
    submodule_data->features[dnx_data_ingr_reassembly_context_direct_context_table].doc = "is context map table accessed directly, without base table";
    submodule_data->features[dnx_data_ingr_reassembly_context_direct_context_table].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_ingr_reassembly_context_port_termination_in_context_table].name = "port_termination_in_context_table";
    submodule_data->features[dnx_data_ingr_reassembly_context_port_termination_in_context_table].doc = "is port termination configured in context table";
    submodule_data->features[dnx_data_ingr_reassembly_context_port_termination_in_context_table].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_ingr_reassembly_context_pp_tm_context].name = "pp_tm_context";
    submodule_data->features[dnx_data_ingr_reassembly_context_pp_tm_context].doc = "Is PP/TM context passed to IRPP/CGM";
    submodule_data->features[dnx_data_ingr_reassembly_context_pp_tm_context].flags |= DNXC_DATA_F_FEATURE;

    /*
     * Defines
     */
    submodule_data->nof_defines = _dnx_data_ingr_reassembly_context_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ingr_reassembly context defines");

    submodule_data->defines[dnx_data_ingr_reassembly_context_define_nof_contexts].name = "nof_contexts";
    submodule_data->defines[dnx_data_ingr_reassembly_context_define_nof_contexts].doc = "Number of ingress reassembly contexts per core";
    /* Set data type flag */
    submodule_data->defines[dnx_data_ingr_reassembly_context_define_nof_contexts].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_reassembly_context_define_invalid_context].name = "invalid_context";
    submodule_data->defines[dnx_data_ingr_reassembly_context_define_invalid_context].doc = "Invalid reassembly context";
    /* Set data type flag */
    submodule_data->defines[dnx_data_ingr_reassembly_context_define_invalid_context].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_reassembly_context_define_nof_cpu_channels_per_core].name = "nof_cpu_channels_per_core";
    submodule_data->defines[dnx_data_ingr_reassembly_context_define_nof_cpu_channels_per_core].doc = "Number of CPU channels per core";
    /* Set data type flag */
    submodule_data->defines[dnx_data_ingr_reassembly_context_define_nof_cpu_channels_per_core].flags |= DNXC_DATA_F_DEFINE;

    /*
     * Tables
     */
    submodule_data->nof_tables = _dnx_data_ingr_reassembly_context_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ingr_reassembly context tables");

    /*
     * Table - context_map
     */
    submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].name = "context_map";
    submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].doc = "Context Map configuration information per port type ";
    submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].size_of_values = sizeof(dnx_data_ingr_reassembly_context_context_map_t);
    submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].entry_get = dnx_data_ingr_reassembly_context_context_map_entry_str_get;

    /* Keys */
    submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].nof_keys = 1;
    submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].keys[0].name = "port_type";
    submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].keys[0].doc = "interface type";

    /* Values */
    submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].nof_values, "_dnx_data_ingr_reassembly_context_table_context_map table values");
    submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].values[0].name = "start_index";
    submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].values[0].type = "uint32";
    submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].values[0].doc = "index of start of specific interface type in reassembly table";
    submodule_data->tables[dnx_data_ingr_reassembly_context_table_context_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_ingr_reassembly_context_context_map_t, start_index);


exit:
    SHR_FUNC_EXIT;
}

/*
 * context features
 */
int
dnx_data_ingr_reassembly_context_feature_get(
    int unit,
    dnx_data_ingr_reassembly_context_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_context, feature);
}

/*
 * context defines
 */
uint32
dnx_data_ingr_reassembly_context_nof_contexts_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_context, dnx_data_ingr_reassembly_context_define_nof_contexts);
}

uint32
dnx_data_ingr_reassembly_context_invalid_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_context, dnx_data_ingr_reassembly_context_define_invalid_context);
}

uint32
dnx_data_ingr_reassembly_context_nof_cpu_channels_per_core_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_context, dnx_data_ingr_reassembly_context_define_nof_cpu_channels_per_core);
}

/*
 * context tables
 */
/* Table Get */
const dnx_data_ingr_reassembly_context_context_map_t *
dnx_data_ingr_reassembly_context_context_map_get(
    int unit,
    int port_type)
{
    char *data;
    dnxc_data_table_t *table;

    /* Get table */
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_context, dnx_data_ingr_reassembly_context_table_context_map);
    /* Get data */
    data = dnxc_data_mgmt_table_data_get(unit, table, port_type, 0);
    return (const dnx_data_ingr_reassembly_context_context_map_t *) data;

}

/* Get value as str */
shr_error_e
dnx_data_ingr_reassembly_context_context_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_ingr_reassembly_context_context_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    /* Get table */
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_context, dnx_data_ingr_reassembly_context_table_context_map);
    data = (const dnx_data_ingr_reassembly_context_context_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->start_index);
            break;
    }

    SHR_FUNC_EXIT;
}

/* Table Info Get */
const dnxc_data_table_info_t *
dnx_data_ingr_reassembly_context_context_map_info_get(
    int unit)
{

    /* Return table info */
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_context, dnx_data_ingr_reassembly_context_table_context_map);

}

/*
 * }
 */

/*
 * SUBMODULE - ILKN:
 * {
 */
/*
 * ilkn init
 */
/**
 * \brief Set submodule data to default value per device type
 * 
 * \param [in] unit - Unit #
 * \param [out] submodule_data - pointer to submodule data
 * 
 * \return
 *     rv - see 'shr_error_e
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
static shr_error_e
dnx_data_ingr_reassembly_ilkn_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ilkn";
    submodule_data->doc = "Interlaken data";
    /*
     * Features
     */
    submodule_data->nof_features = _dnx_data_ingr_reassembly_ilkn_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ingr_reassembly ilkn features");

    submodule_data->features[dnx_data_ingr_reassembly_ilkn_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_ingr_reassembly_ilkn_is_supported].doc = "set if device supports ILKN ports (not including ELK)";
    submodule_data->features[dnx_data_ingr_reassembly_ilkn_is_supported].flags |= DNXC_DATA_F_FEATURE;

    /*
     * Defines
     */
    submodule_data->nof_defines = _dnx_data_ingr_reassembly_ilkn_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ingr_reassembly ilkn defines");

    /*
     * Tables
     */
    submodule_data->nof_tables = _dnx_data_ingr_reassembly_ilkn_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ingr_reassembly ilkn tables");


exit:
    SHR_FUNC_EXIT;
}

/*
 * ilkn features
 */
int
dnx_data_ingr_reassembly_ilkn_feature_get(
    int unit,
    dnx_data_ingr_reassembly_ilkn_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_ilkn, feature);
}

/*
 * ilkn defines
 */
/*
 * ilkn tables
 */
/* Table Get */
/* Get value as str */
/* Table Info Get */
/*
 * }
 */

/*
 * SUBMODULE - PRIORITY:
 * {
 */
/*
 * priority init
 */
/**
 * \brief Set submodule data to default value per device type
 * 
 * \param [in] unit - Unit #
 * \param [out] submodule_data - pointer to submodule data
 * 
 * \return
 *     rv - see 'shr_error_e
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
static shr_error_e
dnx_data_ingr_reassembly_priority_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "priority";
    submodule_data->doc = "information about priorities";
    /*
     * Features
     */
    submodule_data->nof_features = _dnx_data_ingr_reassembly_priority_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ingr_reassembly priority features");

    /*
     * Defines
     */
    submodule_data->nof_defines = _dnx_data_ingr_reassembly_priority_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ingr_reassembly priority defines");

    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_nif_eth_priorities_nof].name = "nif_eth_priorities_nof";
    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_nif_eth_priorities_nof].doc = "Number of priorities for NIF ETH port";
    /* Set data type flag */
    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_nif_eth_priorities_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_mirror_priorities_nof].name = "mirror_priorities_nof";
    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_mirror_priorities_nof].doc = "Number of priorities for mirror channel";
    /* Set data type flag */
    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_mirror_priorities_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_rcy_priorities_nof].name = "rcy_priorities_nof";
    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_rcy_priorities_nof].doc = "Number of priorities (groups) for RCY interface";
    /* Set data type flag */
    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_rcy_priorities_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_priorities_nof].name = "priorities_nof";
    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_priorities_nof].doc = "Number of priorities (groups) for any interface type";
    /* Set data type flag */
    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_priorities_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_cgm_priorities_nof].name = "cgm_priorities_nof";
    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_cgm_priorities_nof].doc = "Number of priorities (groups) for any interface type as considered by CGM module";
    /* Set data type flag */
    submodule_data->defines[dnx_data_ingr_reassembly_priority_define_cgm_priorities_nof].flags |= DNXC_DATA_F_DEFINE;

    /*
     * Tables
     */
    submodule_data->nof_tables = _dnx_data_ingr_reassembly_priority_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ingr_reassembly priority tables");


exit:
    SHR_FUNC_EXIT;
}

/*
 * priority features
 */
int
dnx_data_ingr_reassembly_priority_feature_get(
    int unit,
    dnx_data_ingr_reassembly_priority_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_priority, feature);
}

/*
 * priority defines
 */
uint32
dnx_data_ingr_reassembly_priority_nif_eth_priorities_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_priority, dnx_data_ingr_reassembly_priority_define_nif_eth_priorities_nof);
}

uint32
dnx_data_ingr_reassembly_priority_mirror_priorities_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_priority, dnx_data_ingr_reassembly_priority_define_mirror_priorities_nof);
}

uint32
dnx_data_ingr_reassembly_priority_rcy_priorities_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_priority, dnx_data_ingr_reassembly_priority_define_rcy_priorities_nof);
}

uint32
dnx_data_ingr_reassembly_priority_priorities_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_priority, dnx_data_ingr_reassembly_priority_define_priorities_nof);
}

uint32
dnx_data_ingr_reassembly_priority_cgm_priorities_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_priority, dnx_data_ingr_reassembly_priority_define_cgm_priorities_nof);
}

/*
 * priority tables
 */
/* Table Get */
/* Get value as str */
/* Table Info Get */
/*
 * }
 */

/*
 * SUBMODULE - DBAL:
 * {
 */
/*
 * dbal init
 */
/**
 * \brief Set submodule data to default value per device type
 * 
 * \param [in] unit - Unit #
 * \param [out] submodule_data - pointer to submodule data
 * 
 * \return
 *     rv - see 'shr_error_e
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
static shr_error_e
dnx_data_ingr_reassembly_dbal_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "dbal";
    submodule_data->doc = "information required for dbal";
    /*
     * Features
     */
    submodule_data->nof_features = _dnx_data_ingr_reassembly_dbal_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ingr_reassembly dbal features");

    /*
     * Defines
     */
    submodule_data->nof_defines = _dnx_data_ingr_reassembly_dbal_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ingr_reassembly dbal defines");

    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_reassembly_context_bits].name = "reassembly_context_bits";
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_reassembly_context_bits].doc = "number of bits in reassembly context";
    /* Set data type flag */
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_reassembly_context_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_interface_bits].name = "interface_bits";
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_interface_bits].doc = "number of bits in ingress reassembly interface ID";
    /* Set data type flag */
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_interface_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_interface_max].name = "interface_max";
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_interface_max].doc = "maximal ingress reassembly interface ID";
    /* Set data type flag */
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_interface_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_context_map_base_address_bits].name = "context_map_base_address_bits";
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_context_map_base_address_bits].doc = "number of bits in context_map_base_address";
    /* Set data type flag */
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_context_map_base_address_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_context_map_base_address_max].name = "context_map_base_address_max";
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_context_map_base_address_max].doc = "maximal value of context_map_base_address";
    /* Set data type flag */
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_context_map_base_address_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_context_map_base_offest_ext_max].name = "context_map_base_offest_ext_max";
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_context_map_base_offest_ext_max].doc = "maximal value of offset ext field";
    /* Set data type flag */
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_context_map_base_offest_ext_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_interleaved_interface_max].name = "interleaved_interface_max";
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_interleaved_interface_max].doc = "maximal interleaved interface ID";
    /* Set data type flag */
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_interleaved_interface_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_interleaved_interface_bits].name = "interleaved_interface_bits";
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_interleaved_interface_bits].doc = "number of bits in interleaved interface ID";
    /* Set data type flag */
    submodule_data->defines[dnx_data_ingr_reassembly_dbal_define_interleaved_interface_bits].flags |= DNXC_DATA_F_NUMERIC;

    /*
     * Tables
     */
    submodule_data->nof_tables = _dnx_data_ingr_reassembly_dbal_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ingr_reassembly dbal tables");


exit:
    SHR_FUNC_EXIT;
}

/*
 * dbal features
 */
int
dnx_data_ingr_reassembly_dbal_feature_get(
    int unit,
    dnx_data_ingr_reassembly_dbal_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_dbal, feature);
}

/*
 * dbal defines
 */
uint32
dnx_data_ingr_reassembly_dbal_reassembly_context_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_dbal, dnx_data_ingr_reassembly_dbal_define_reassembly_context_bits);
}

uint32
dnx_data_ingr_reassembly_dbal_interface_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_dbal, dnx_data_ingr_reassembly_dbal_define_interface_bits);
}

uint32
dnx_data_ingr_reassembly_dbal_interface_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_dbal, dnx_data_ingr_reassembly_dbal_define_interface_max);
}

uint32
dnx_data_ingr_reassembly_dbal_context_map_base_address_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_dbal, dnx_data_ingr_reassembly_dbal_define_context_map_base_address_bits);
}

uint32
dnx_data_ingr_reassembly_dbal_context_map_base_address_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_dbal, dnx_data_ingr_reassembly_dbal_define_context_map_base_address_max);
}

uint32
dnx_data_ingr_reassembly_dbal_context_map_base_offest_ext_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_dbal, dnx_data_ingr_reassembly_dbal_define_context_map_base_offest_ext_max);
}

uint32
dnx_data_ingr_reassembly_dbal_interleaved_interface_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_dbal, dnx_data_ingr_reassembly_dbal_define_interleaved_interface_max);
}

uint32
dnx_data_ingr_reassembly_dbal_interleaved_interface_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingr_reassembly, dnx_data_ingr_reassembly_submodule_dbal, dnx_data_ingr_reassembly_dbal_define_interleaved_interface_bits);
}

/*
 * dbal tables
 */
/* Table Get */
/* Get value as str */
/* Table Info Get */
/*
 * }
 */

shr_error_e
dnx_data_ingr_reassembly_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    /* Data Struct Init */
    module_data->name = "ingr_reassembly";
    module_data->nof_submodules = _dnx_data_ingr_reassembly_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data ingr_reassembly submodules");

    /*
     * Init Submodule data
     */
    SHR_IF_ERR_EXIT(dnx_data_ingr_reassembly_context_init(unit, &module_data->submodules[dnx_data_ingr_reassembly_submodule_context]));
    SHR_IF_ERR_EXIT(dnx_data_ingr_reassembly_ilkn_init(unit, &module_data->submodules[dnx_data_ingr_reassembly_submodule_ilkn]));
    SHR_IF_ERR_EXIT(dnx_data_ingr_reassembly_priority_init(unit, &module_data->submodules[dnx_data_ingr_reassembly_submodule_priority]));
    SHR_IF_ERR_EXIT(dnx_data_ingr_reassembly_dbal_init(unit, &module_data->submodules[dnx_data_ingr_reassembly_submodule_dbal]));
    /*
     * Attach device module
     */
#ifdef BCM_DNX2_SUPPORT
/*{*/
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingr_reassembly_attach(unit));
    }
    else
/*}*/
#endif /*BCM_DNX2_SUPPORT*/
#ifdef BCM_DNX2_SUPPORT
/*{*/
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingr_reassembly_attach(unit));
    }
    else
/*}*/
#endif /*BCM_DNX2_SUPPORT*/
#ifdef BCM_DNX2_SUPPORT
/*{*/
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingr_reassembly_attach(unit));
    }
    else
/*}*/
#endif /*BCM_DNX2_SUPPORT*/
#ifdef BCM_DNX2_SUPPORT
/*{*/
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingr_reassembly_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_ingr_reassembly_attach(unit));
    }
    else
/*}*/
#endif /*BCM_DNX2_SUPPORT*/
#ifdef BCM_DNX2_SUPPORT
/*{*/
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingr_reassembly_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_ingr_reassembly_attach(unit));
    }
    else
/*}*/
#endif /*BCM_DNX2_SUPPORT*/
#ifdef BCM_DNX2_SUPPORT
/*{*/
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingr_reassembly_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_ingr_reassembly_attach(unit));
    }
    else
/*}*/
#endif /*BCM_DNX2_SUPPORT*/
#ifdef BCM_DNX2_SUPPORT
/*{*/
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingr_reassembly_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_ingr_reassembly_attach(unit));
    }
    else
/*}*/
#endif /*BCM_DNX2_SUPPORT*/
#ifdef BCM_DNX2_SUPPORT
/*{*/
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingr_reassembly_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_ingr_reassembly_attach(unit));
    }
    else
/*}*/
#endif /*BCM_DNX2_SUPPORT*/
#ifdef BCM_DNX2_SUPPORT
/*{*/
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingr_reassembly_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_ingr_reassembly_attach(unit));
    }
    else
/*}*/
#endif /*BCM_DNX2_SUPPORT*/
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE
/* *INDENT-ON* */
