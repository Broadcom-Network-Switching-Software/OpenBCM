

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FABRIC

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_fabric.h>



extern shr_error_e jr2_a0_data_fabric_attach(
    int unit);
extern shr_error_e j2c_a0_data_fabric_attach(
    int unit);
extern shr_error_e q2a_a0_data_fabric_attach(
    int unit);
extern shr_error_e j2p_a0_data_fabric_attach(
    int unit);



static shr_error_e
dnx_data_fabric_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "General data about fabric";
    
    submodule_data->nof_features = _dnx_data_fabric_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data fabric general features");

    submodule_data->features[dnx_data_fabric_general_blocks_exist].name = "blocks_exist";
    submodule_data->features[dnx_data_fabric_general_blocks_exist].doc = "Do fabric blocks exist in device. A device could have its fabric links disabled, but still use the fabric lins for local route.";
    submodule_data->features[dnx_data_fabric_general_blocks_exist].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_fabric_general_clock_power_down].name = "clock_power_down";
    submodule_data->features[dnx_data_fabric_general_clock_power_down].doc = "Can fabric blocks clock be power down";
    submodule_data->features[dnx_data_fabric_general_clock_power_down].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_fabric_general_is_jr1_in_system_supported].name = "is_jr1_in_system_supported";
    submodule_data->features[dnx_data_fabric_general_is_jr1_in_system_supported].doc = "Is the JR1 in system single cell delay optimization supported";
    submodule_data->features[dnx_data_fabric_general_is_jr1_in_system_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_fabric_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data fabric general defines");

    submodule_data->defines[dnx_data_fabric_general_define_nof_lcplls].name = "nof_lcplls";
    submodule_data->defines[dnx_data_fabric_general_define_nof_lcplls].doc = "number of lcplls supported";
    
    submodule_data->defines[dnx_data_fabric_general_define_nof_lcplls].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_general_define_fmac_clock_khz].name = "fmac_clock_khz";
    submodule_data->defines[dnx_data_fabric_general_define_fmac_clock_khz].doc = "FMAC clock frequency [KHz]";
    
    submodule_data->defines[dnx_data_fabric_general_define_fmac_clock_khz].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_general_define_connect_mode].name = "connect_mode";
    submodule_data->defines[dnx_data_fabric_general_define_connect_mode].doc = "Fabric interface connect mode";
    
    submodule_data->defines[dnx_data_fabric_general_define_connect_mode].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_fabric_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data fabric general tables");

    
    submodule_data->tables[dnx_data_fabric_general_table_fmac_bus_size].name = "fmac_bus_size";
    submodule_data->tables[dnx_data_fabric_general_table_fmac_bus_size].doc = "Fabric MAC bus size [bits]";
    submodule_data->tables[dnx_data_fabric_general_table_fmac_bus_size].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_fabric_general_table_fmac_bus_size].size_of_values = sizeof(dnx_data_fabric_general_fmac_bus_size_t);
    submodule_data->tables[dnx_data_fabric_general_table_fmac_bus_size].entry_get = dnx_data_fabric_general_fmac_bus_size_entry_str_get;

    
    submodule_data->tables[dnx_data_fabric_general_table_fmac_bus_size].nof_keys = 1;
    submodule_data->tables[dnx_data_fabric_general_table_fmac_bus_size].keys[0].name = "mode";
    submodule_data->tables[dnx_data_fabric_general_table_fmac_bus_size].keys[0].doc = "Fabric MAC mode (FEC type)";

    
    submodule_data->tables[dnx_data_fabric_general_table_fmac_bus_size].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_fabric_general_table_fmac_bus_size].values, dnxc_data_value_t, submodule_data->tables[dnx_data_fabric_general_table_fmac_bus_size].nof_values, "_dnx_data_fabric_general_table_fmac_bus_size table values");
    submodule_data->tables[dnx_data_fabric_general_table_fmac_bus_size].values[0].name = "size";
    submodule_data->tables[dnx_data_fabric_general_table_fmac_bus_size].values[0].type = "int";
    submodule_data->tables[dnx_data_fabric_general_table_fmac_bus_size].values[0].doc = "The bus size [bits]";
    submodule_data->tables[dnx_data_fabric_general_table_fmac_bus_size].values[0].offset = UTILEX_OFFSETOF(dnx_data_fabric_general_fmac_bus_size_t, size);

    
    submodule_data->tables[dnx_data_fabric_general_table_pll_phys].name = "pll_phys";
    submodule_data->tables[dnx_data_fabric_general_table_pll_phys].doc = "phys controlled by each lcpll";
    submodule_data->tables[dnx_data_fabric_general_table_pll_phys].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_fabric_general_table_pll_phys].size_of_values = sizeof(dnx_data_fabric_general_pll_phys_t);
    submodule_data->tables[dnx_data_fabric_general_table_pll_phys].entry_get = dnx_data_fabric_general_pll_phys_entry_str_get;

    
    submodule_data->tables[dnx_data_fabric_general_table_pll_phys].nof_keys = 1;
    submodule_data->tables[dnx_data_fabric_general_table_pll_phys].keys[0].name = "lcpll";
    submodule_data->tables[dnx_data_fabric_general_table_pll_phys].keys[0].doc = "LCPLL index";

    
    submodule_data->tables[dnx_data_fabric_general_table_pll_phys].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_fabric_general_table_pll_phys].values, dnxc_data_value_t, submodule_data->tables[dnx_data_fabric_general_table_pll_phys].nof_values, "_dnx_data_fabric_general_table_pll_phys table values");
    submodule_data->tables[dnx_data_fabric_general_table_pll_phys].values[0].name = "min_phy_id";
    submodule_data->tables[dnx_data_fabric_general_table_pll_phys].values[0].type = "uint32";
    submodule_data->tables[dnx_data_fabric_general_table_pll_phys].values[0].doc = "The min phy ID controlled by given pll";
    submodule_data->tables[dnx_data_fabric_general_table_pll_phys].values[0].offset = UTILEX_OFFSETOF(dnx_data_fabric_general_pll_phys_t, min_phy_id);
    submodule_data->tables[dnx_data_fabric_general_table_pll_phys].values[1].name = "max_phy_id";
    submodule_data->tables[dnx_data_fabric_general_table_pll_phys].values[1].type = "uint32";
    submodule_data->tables[dnx_data_fabric_general_table_pll_phys].values[1].doc = "The max phy ID controlled by given pll";
    submodule_data->tables[dnx_data_fabric_general_table_pll_phys].values[1].offset = UTILEX_OFFSETOF(dnx_data_fabric_general_pll_phys_t, max_phy_id);

    
    submodule_data->tables[dnx_data_fabric_general_table_pm_properties].name = "pm_properties";
    submodule_data->tables[dnx_data_fabric_general_table_pm_properties].doc = "PM properties per PM";
    submodule_data->tables[dnx_data_fabric_general_table_pm_properties].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_fabric_general_table_pm_properties].size_of_values = sizeof(dnx_data_fabric_general_pm_properties_t);
    submodule_data->tables[dnx_data_fabric_general_table_pm_properties].entry_get = dnx_data_fabric_general_pm_properties_entry_str_get;

    
    submodule_data->tables[dnx_data_fabric_general_table_pm_properties].nof_keys = 1;
    submodule_data->tables[dnx_data_fabric_general_table_pm_properties].keys[0].name = "pm_index";
    submodule_data->tables[dnx_data_fabric_general_table_pm_properties].keys[0].doc = "PM instance number.";

    
    submodule_data->tables[dnx_data_fabric_general_table_pm_properties].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_fabric_general_table_pm_properties].values, dnxc_data_value_t, submodule_data->tables[dnx_data_fabric_general_table_pm_properties].nof_values, "_dnx_data_fabric_general_table_pm_properties table values");
    submodule_data->tables[dnx_data_fabric_general_table_pm_properties].values[0].name = "special_pll_check";
    submodule_data->tables[dnx_data_fabric_general_table_pm_properties].values[0].type = "int";
    submodule_data->tables[dnx_data_fabric_general_table_pm_properties].values[0].doc = "Boolean. Does this PM require special PLL check";
    submodule_data->tables[dnx_data_fabric_general_table_pm_properties].values[0].offset = UTILEX_OFFSETOF(dnx_data_fabric_general_pm_properties_t, special_pll_check);
    submodule_data->tables[dnx_data_fabric_general_table_pm_properties].values[1].name = "nof_plls";
    submodule_data->tables[dnx_data_fabric_general_table_pm_properties].values[1].type = "int";
    submodule_data->tables[dnx_data_fabric_general_table_pm_properties].values[1].doc = "Number of PLLs that the PM has";
    submodule_data->tables[dnx_data_fabric_general_table_pm_properties].values[1].offset = UTILEX_OFFSETOF(dnx_data_fabric_general_pm_properties_t, nof_plls);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_fabric_general_feature_get(
    int unit,
    dnx_data_fabric_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_general, feature);
}


uint32
dnx_data_fabric_general_nof_lcplls_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_general, dnx_data_fabric_general_define_nof_lcplls);
}

uint32
dnx_data_fabric_general_fmac_clock_khz_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_general, dnx_data_fabric_general_define_fmac_clock_khz);
}

uint32
dnx_data_fabric_general_connect_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_general, dnx_data_fabric_general_define_connect_mode);
}



const dnx_data_fabric_general_fmac_bus_size_t *
dnx_data_fabric_general_fmac_bus_size_get(
    int unit,
    int mode)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_general, dnx_data_fabric_general_table_fmac_bus_size);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, mode, 0);
    return (const dnx_data_fabric_general_fmac_bus_size_t *) data;

}

const dnx_data_fabric_general_pll_phys_t *
dnx_data_fabric_general_pll_phys_get(
    int unit,
    int lcpll)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_general, dnx_data_fabric_general_table_pll_phys);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, lcpll, 0);
    return (const dnx_data_fabric_general_pll_phys_t *) data;

}

const dnx_data_fabric_general_pm_properties_t *
dnx_data_fabric_general_pm_properties_get(
    int unit,
    int pm_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_general, dnx_data_fabric_general_table_pm_properties);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, pm_index, 0);
    return (const dnx_data_fabric_general_pm_properties_t *) data;

}


shr_error_e
dnx_data_fabric_general_fmac_bus_size_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_fabric_general_fmac_bus_size_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_general, dnx_data_fabric_general_table_fmac_bus_size);
    data = (const dnx_data_fabric_general_fmac_bus_size_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->size);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_fabric_general_pll_phys_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_fabric_general_pll_phys_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_general, dnx_data_fabric_general_table_pll_phys);
    data = (const dnx_data_fabric_general_pll_phys_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->min_phy_id);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->max_phy_id);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_fabric_general_pm_properties_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_fabric_general_pm_properties_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_general, dnx_data_fabric_general_table_pm_properties);
    data = (const dnx_data_fabric_general_pm_properties_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->special_pll_check);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_plls);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_fabric_general_fmac_bus_size_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_general, dnx_data_fabric_general_table_fmac_bus_size);

}

const dnxc_data_table_info_t *
dnx_data_fabric_general_pll_phys_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_general, dnx_data_fabric_general_table_pll_phys);

}

const dnxc_data_table_info_t *
dnx_data_fabric_general_pm_properties_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_general, dnx_data_fabric_general_table_pm_properties);

}






static shr_error_e
dnx_data_fabric_links_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "links";
    submodule_data->doc = "data about fabric links";
    
    submodule_data->nof_features = _dnx_data_fabric_links_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data fabric links features");

    submodule_data->features[dnx_data_fabric_links_load_balancing_periodic_event_enabled].name = "load_balancing_periodic_event_enabled";
    submodule_data->features[dnx_data_fabric_links_load_balancing_periodic_event_enabled].doc = "Is the load balancing has periodic event that changes its counting mode";
    submodule_data->features[dnx_data_fabric_links_load_balancing_periodic_event_enabled].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_fabric_links_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data fabric links defines");

    submodule_data->defines[dnx_data_fabric_links_define_nof_links].name = "nof_links";
    submodule_data->defines[dnx_data_fabric_links_define_nof_links].doc = "Number of fabric links";
    
    submodule_data->defines[dnx_data_fabric_links_define_nof_links].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_links_define_nof_links_per_core].name = "nof_links_per_core";
    submodule_data->defines[dnx_data_fabric_links_define_nof_links_per_core].doc = "number of fabric links per core supported";
    
    submodule_data->defines[dnx_data_fabric_links_define_nof_links_per_core].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_links_define_max_link_id].name = "max_link_id";
    submodule_data->defines[dnx_data_fabric_links_define_max_link_id].doc = "Maximum id of a fabric link";
    
    submodule_data->defines[dnx_data_fabric_links_define_max_link_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_links_define_max_speed].name = "max_speed";
    submodule_data->defines[dnx_data_fabric_links_define_max_speed].doc = "Maximum speed a fabric link can have";
    
    submodule_data->defines[dnx_data_fabric_links_define_max_speed].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_links_define_usec_between_load_balancing_events].name = "usec_between_load_balancing_events";
    submodule_data->defines[dnx_data_fabric_links_define_usec_between_load_balancing_events].doc = "Time between load balancing periodic events";
    
    submodule_data->defines[dnx_data_fabric_links_define_usec_between_load_balancing_events].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_links_define_kr_fec_supported].name = "kr_fec_supported";
    submodule_data->defines[dnx_data_fabric_links_define_kr_fec_supported].doc = "Indicates if KR FEC is supproted. Used as control for other data tables so that is why its not a feature";
    
    submodule_data->defines[dnx_data_fabric_links_define_kr_fec_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_links_define_core_mapping_mode].name = "core_mapping_mode";
    submodule_data->defines[dnx_data_fabric_links_define_core_mapping_mode].doc = "Fabric link-to-core mapping mode";
    
    submodule_data->defines[dnx_data_fabric_links_define_core_mapping_mode].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_fabric_links_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data fabric links tables");

    
    submodule_data->tables[dnx_data_fabric_links_table_general].name = "general";
    submodule_data->tables[dnx_data_fabric_links_table_general].doc = "general data about links";
    submodule_data->tables[dnx_data_fabric_links_table_general].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_fabric_links_table_general].size_of_values = sizeof(dnx_data_fabric_links_general_t);
    submodule_data->tables[dnx_data_fabric_links_table_general].entry_get = dnx_data_fabric_links_general_entry_str_get;

    
    submodule_data->tables[dnx_data_fabric_links_table_general].nof_keys = 0;

    
    submodule_data->tables[dnx_data_fabric_links_table_general].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_fabric_links_table_general].values, dnxc_data_value_t, submodule_data->tables[dnx_data_fabric_links_table_general].nof_values, "_dnx_data_fabric_links_table_general table values");
    submodule_data->tables[dnx_data_fabric_links_table_general].values[0].name = "supported_links";
    submodule_data->tables[dnx_data_fabric_links_table_general].values[0].type = "bcm_pbmp_t";
    submodule_data->tables[dnx_data_fabric_links_table_general].values[0].doc = "bitmap of supported links";
    submodule_data->tables[dnx_data_fabric_links_table_general].values[0].offset = UTILEX_OFFSETOF(dnx_data_fabric_links_general_t, supported_links);

    
    submodule_data->tables[dnx_data_fabric_links_table_polarity].name = "polarity";
    submodule_data->tables[dnx_data_fabric_links_table_polarity].doc = "Polarity of fabric link";
    submodule_data->tables[dnx_data_fabric_links_table_polarity].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_fabric_links_table_polarity].size_of_values = sizeof(dnx_data_fabric_links_polarity_t);
    submodule_data->tables[dnx_data_fabric_links_table_polarity].entry_get = dnx_data_fabric_links_polarity_entry_str_get;

    
    submodule_data->tables[dnx_data_fabric_links_table_polarity].nof_keys = 1;
    submodule_data->tables[dnx_data_fabric_links_table_polarity].keys[0].name = "link";
    submodule_data->tables[dnx_data_fabric_links_table_polarity].keys[0].doc = "fabric link id";

    
    submodule_data->tables[dnx_data_fabric_links_table_polarity].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_fabric_links_table_polarity].values, dnxc_data_value_t, submodule_data->tables[dnx_data_fabric_links_table_polarity].nof_values, "_dnx_data_fabric_links_table_polarity table values");
    submodule_data->tables[dnx_data_fabric_links_table_polarity].values[0].name = "tx_polarity";
    submodule_data->tables[dnx_data_fabric_links_table_polarity].values[0].type = "uint32";
    submodule_data->tables[dnx_data_fabric_links_table_polarity].values[0].doc = "TX Polarity";
    submodule_data->tables[dnx_data_fabric_links_table_polarity].values[0].offset = UTILEX_OFFSETOF(dnx_data_fabric_links_polarity_t, tx_polarity);
    submodule_data->tables[dnx_data_fabric_links_table_polarity].values[1].name = "rx_polarity";
    submodule_data->tables[dnx_data_fabric_links_table_polarity].values[1].type = "uint32";
    submodule_data->tables[dnx_data_fabric_links_table_polarity].values[1].doc = "RX Polarity";
    submodule_data->tables[dnx_data_fabric_links_table_polarity].values[1].offset = UTILEX_OFFSETOF(dnx_data_fabric_links_polarity_t, rx_polarity);

    
    submodule_data->tables[dnx_data_fabric_links_table_supported_interfaces].name = "supported_interfaces";
    submodule_data->tables[dnx_data_fabric_links_table_supported_interfaces].doc = "table with all supported by the device speeds";
    submodule_data->tables[dnx_data_fabric_links_table_supported_interfaces].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_fabric_links_table_supported_interfaces].size_of_values = sizeof(dnx_data_fabric_links_supported_interfaces_t);
    submodule_data->tables[dnx_data_fabric_links_table_supported_interfaces].entry_get = dnx_data_fabric_links_supported_interfaces_entry_str_get;

    
    submodule_data->tables[dnx_data_fabric_links_table_supported_interfaces].nof_keys = 1;
    submodule_data->tables[dnx_data_fabric_links_table_supported_interfaces].keys[0].name = "index";
    submodule_data->tables[dnx_data_fabric_links_table_supported_interfaces].keys[0].doc = "entry index";

    
    submodule_data->tables[dnx_data_fabric_links_table_supported_interfaces].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_fabric_links_table_supported_interfaces].values, dnxc_data_value_t, submodule_data->tables[dnx_data_fabric_links_table_supported_interfaces].nof_values, "_dnx_data_fabric_links_table_supported_interfaces table values");
    submodule_data->tables[dnx_data_fabric_links_table_supported_interfaces].values[0].name = "speed";
    submodule_data->tables[dnx_data_fabric_links_table_supported_interfaces].values[0].type = "uint32";
    submodule_data->tables[dnx_data_fabric_links_table_supported_interfaces].values[0].doc = "Fabric lane serdes speed.";
    submodule_data->tables[dnx_data_fabric_links_table_supported_interfaces].values[0].offset = UTILEX_OFFSETOF(dnx_data_fabric_links_supported_interfaces_t, speed);
    submodule_data->tables[dnx_data_fabric_links_table_supported_interfaces].values[1].name = "fec_type";
    submodule_data->tables[dnx_data_fabric_links_table_supported_interfaces].values[1].type = "bcm_port_phy_fec_t";
    submodule_data->tables[dnx_data_fabric_links_table_supported_interfaces].values[1].doc = "fec type";
    submodule_data->tables[dnx_data_fabric_links_table_supported_interfaces].values[1].offset = UTILEX_OFFSETOF(dnx_data_fabric_links_supported_interfaces_t, fec_type);
    submodule_data->tables[dnx_data_fabric_links_table_supported_interfaces].values[2].name = "is_valid";
    submodule_data->tables[dnx_data_fabric_links_table_supported_interfaces].values[2].type = "uint32";
    submodule_data->tables[dnx_data_fabric_links_table_supported_interfaces].values[2].doc = "Used to filter the valid table entries per device.";
    submodule_data->tables[dnx_data_fabric_links_table_supported_interfaces].values[2].offset = UTILEX_OFFSETOF(dnx_data_fabric_links_supported_interfaces_t, is_valid);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_fabric_links_feature_get(
    int unit,
    dnx_data_fabric_links_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_links, feature);
}


uint32
dnx_data_fabric_links_nof_links_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_links, dnx_data_fabric_links_define_nof_links);
}

uint32
dnx_data_fabric_links_nof_links_per_core_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_links, dnx_data_fabric_links_define_nof_links_per_core);
}

uint32
dnx_data_fabric_links_max_link_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_links, dnx_data_fabric_links_define_max_link_id);
}

uint32
dnx_data_fabric_links_max_speed_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_links, dnx_data_fabric_links_define_max_speed);
}

uint32
dnx_data_fabric_links_usec_between_load_balancing_events_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_links, dnx_data_fabric_links_define_usec_between_load_balancing_events);
}

uint32
dnx_data_fabric_links_kr_fec_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_links, dnx_data_fabric_links_define_kr_fec_supported);
}

uint32
dnx_data_fabric_links_core_mapping_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_links, dnx_data_fabric_links_define_core_mapping_mode);
}



const dnx_data_fabric_links_general_t *
dnx_data_fabric_links_general_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_links, dnx_data_fabric_links_table_general);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_fabric_links_general_t *) data;

}

const dnx_data_fabric_links_polarity_t *
dnx_data_fabric_links_polarity_get(
    int unit,
    int link)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_links, dnx_data_fabric_links_table_polarity);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, link, 0);
    return (const dnx_data_fabric_links_polarity_t *) data;

}

const dnx_data_fabric_links_supported_interfaces_t *
dnx_data_fabric_links_supported_interfaces_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_links, dnx_data_fabric_links_table_supported_interfaces);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_fabric_links_supported_interfaces_t *) data;

}


shr_error_e
dnx_data_fabric_links_general_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_fabric_links_general_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_links, dnx_data_fabric_links_table_general);
    data = (const dnx_data_fabric_links_general_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_PBMP_STR(buffer, data->supported_links);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_fabric_links_polarity_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_fabric_links_polarity_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_links, dnx_data_fabric_links_table_polarity);
    data = (const dnx_data_fabric_links_polarity_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tx_polarity);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->rx_polarity);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_fabric_links_supported_interfaces_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_fabric_links_supported_interfaces_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_links, dnx_data_fabric_links_table_supported_interfaces);
    data = (const dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->speed);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fec_type);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_valid);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_fabric_links_general_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_links, dnx_data_fabric_links_table_general);

}

const dnxc_data_table_info_t *
dnx_data_fabric_links_polarity_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_links, dnx_data_fabric_links_table_polarity);

}

const dnxc_data_table_info_t *
dnx_data_fabric_links_supported_interfaces_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_links, dnx_data_fabric_links_table_supported_interfaces);

}






static shr_error_e
dnx_data_fabric_blocks_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "blocks";
    submodule_data->doc = "General block information";
    
    submodule_data->nof_features = _dnx_data_fabric_blocks_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data fabric blocks features");

    
    submodule_data->nof_defines = _dnx_data_fabric_blocks_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data fabric blocks defines");

    submodule_data->defines[dnx_data_fabric_blocks_define_nof_instances_fmac].name = "nof_instances_fmac";
    submodule_data->defines[dnx_data_fabric_blocks_define_nof_instances_fmac].doc = "Number of FMAC instances.";
    
    submodule_data->defines[dnx_data_fabric_blocks_define_nof_instances_fmac].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_blocks_define_nof_links_in_fmac].name = "nof_links_in_fmac";
    submodule_data->defines[dnx_data_fabric_blocks_define_nof_links_in_fmac].doc = "Number of links for each FMAC block.";
    
    submodule_data->defines[dnx_data_fabric_blocks_define_nof_links_in_fmac].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_blocks_define_nof_instances_fsrd].name = "nof_instances_fsrd";
    submodule_data->defines[dnx_data_fabric_blocks_define_nof_instances_fsrd].doc = "Number of FSRD instances.";
    
    submodule_data->defines[dnx_data_fabric_blocks_define_nof_instances_fsrd].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_blocks_define_nof_links_in_fsrd].name = "nof_links_in_fsrd";
    submodule_data->defines[dnx_data_fabric_blocks_define_nof_links_in_fsrd].doc = "Number of links for each FSRD block.";
    
    submodule_data->defines[dnx_data_fabric_blocks_define_nof_links_in_fsrd].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_blocks_define_nof_fmacs_in_fsrd].name = "nof_fmacs_in_fsrd";
    submodule_data->defines[dnx_data_fabric_blocks_define_nof_fmacs_in_fsrd].doc = "Number of FMAC blocks for each FSRD block.";
    
    submodule_data->defines[dnx_data_fabric_blocks_define_nof_fmacs_in_fsrd].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_blocks_define_nof_pms].name = "nof_pms";
    submodule_data->defines[dnx_data_fabric_blocks_define_nof_pms].doc = "Number of fabric PMs";
    
    submodule_data->defines[dnx_data_fabric_blocks_define_nof_pms].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_blocks_define_nof_links_in_pm].name = "nof_links_in_pm";
    submodule_data->defines[dnx_data_fabric_blocks_define_nof_links_in_pm].doc = "Number of links for each fabric PM";
    
    submodule_data->defines[dnx_data_fabric_blocks_define_nof_links_in_pm].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_blocks_define_nof_fmacs_in_pm].name = "nof_fmacs_in_pm";
    submodule_data->defines[dnx_data_fabric_blocks_define_nof_fmacs_in_pm].doc = "Number of FMAC blocks for each fabric PM";
    
    submodule_data->defines[dnx_data_fabric_blocks_define_nof_fmacs_in_pm].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_blocks_define_nof_instances_fdtl].name = "nof_instances_fdtl";
    submodule_data->defines[dnx_data_fabric_blocks_define_nof_instances_fdtl].doc = "Number of FDTL block instances";
    
    submodule_data->defines[dnx_data_fabric_blocks_define_nof_instances_fdtl].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_blocks_define_nof_links_in_fdtl].name = "nof_links_in_fdtl";
    submodule_data->defines[dnx_data_fabric_blocks_define_nof_links_in_fdtl].doc = "Number of links per each FDTL block";
    
    submodule_data->defines[dnx_data_fabric_blocks_define_nof_links_in_fdtl].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_blocks_define_nof_sbus_chains].name = "nof_sbus_chains";
    submodule_data->defines[dnx_data_fabric_blocks_define_nof_sbus_chains].doc = "Number of FSRD SBUS chains";
    
    submodule_data->defines[dnx_data_fabric_blocks_define_nof_sbus_chains].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_fabric_blocks_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data fabric blocks tables");

    
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_sbus_chain].name = "fsrd_sbus_chain";
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_sbus_chain].doc = "The order of FSRDs in the SBUS chain";
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_sbus_chain].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_sbus_chain].size_of_values = sizeof(dnx_data_fabric_blocks_fsrd_sbus_chain_t);
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_sbus_chain].entry_get = dnx_data_fabric_blocks_fsrd_sbus_chain_entry_str_get;

    
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_sbus_chain].nof_keys = 1;
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_sbus_chain].keys[0].name = "fsrd_id";
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_sbus_chain].keys[0].doc = "FSRD ID";

    
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_sbus_chain].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_sbus_chain].values, dnxc_data_value_t, submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_sbus_chain].nof_values, "_dnx_data_fabric_blocks_table_fsrd_sbus_chain table values");
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_sbus_chain].values[0].name = "chain";
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_sbus_chain].values[0].type = "int";
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_sbus_chain].values[0].doc = "SBUS chain number";
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_sbus_chain].values[0].offset = UTILEX_OFFSETOF(dnx_data_fabric_blocks_fsrd_sbus_chain_t, chain);
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_sbus_chain].values[1].name = "index_in_chain";
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_sbus_chain].values[1].type = "int";
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_sbus_chain].values[1].doc = "The index of the FSRD in the SBUS chain";
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_sbus_chain].values[1].offset = UTILEX_OFFSETOF(dnx_data_fabric_blocks_fsrd_sbus_chain_t, index_in_chain);

    
    submodule_data->tables[dnx_data_fabric_blocks_table_fmac_sbus_chain].name = "fmac_sbus_chain";
    submodule_data->tables[dnx_data_fabric_blocks_table_fmac_sbus_chain].doc = "The order of FMACs in the SBUS chain";
    submodule_data->tables[dnx_data_fabric_blocks_table_fmac_sbus_chain].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_fabric_blocks_table_fmac_sbus_chain].size_of_values = sizeof(dnx_data_fabric_blocks_fmac_sbus_chain_t);
    submodule_data->tables[dnx_data_fabric_blocks_table_fmac_sbus_chain].entry_get = dnx_data_fabric_blocks_fmac_sbus_chain_entry_str_get;

    
    submodule_data->tables[dnx_data_fabric_blocks_table_fmac_sbus_chain].nof_keys = 1;
    submodule_data->tables[dnx_data_fabric_blocks_table_fmac_sbus_chain].keys[0].name = "fmac_id";
    submodule_data->tables[dnx_data_fabric_blocks_table_fmac_sbus_chain].keys[0].doc = "FMAC ID";

    
    submodule_data->tables[dnx_data_fabric_blocks_table_fmac_sbus_chain].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_fabric_blocks_table_fmac_sbus_chain].values, dnxc_data_value_t, submodule_data->tables[dnx_data_fabric_blocks_table_fmac_sbus_chain].nof_values, "_dnx_data_fabric_blocks_table_fmac_sbus_chain table values");
    submodule_data->tables[dnx_data_fabric_blocks_table_fmac_sbus_chain].values[0].name = "chain";
    submodule_data->tables[dnx_data_fabric_blocks_table_fmac_sbus_chain].values[0].type = "int";
    submodule_data->tables[dnx_data_fabric_blocks_table_fmac_sbus_chain].values[0].doc = "SBUS chain number";
    submodule_data->tables[dnx_data_fabric_blocks_table_fmac_sbus_chain].values[0].offset = UTILEX_OFFSETOF(dnx_data_fabric_blocks_fmac_sbus_chain_t, chain);
    submodule_data->tables[dnx_data_fabric_blocks_table_fmac_sbus_chain].values[1].name = "index_in_chain";
    submodule_data->tables[dnx_data_fabric_blocks_table_fmac_sbus_chain].values[1].type = "int";
    submodule_data->tables[dnx_data_fabric_blocks_table_fmac_sbus_chain].values[1].doc = "The index of the FMAC in the SBUS chain";
    submodule_data->tables[dnx_data_fabric_blocks_table_fmac_sbus_chain].values[1].offset = UTILEX_OFFSETOF(dnx_data_fabric_blocks_fmac_sbus_chain_t, index_in_chain);

    
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_ref_clk_master].name = "fsrd_ref_clk_master";
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_ref_clk_master].doc = "The ref clock dependency between the FSRDs";
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_ref_clk_master].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_ref_clk_master].size_of_values = sizeof(dnx_data_fabric_blocks_fsrd_ref_clk_master_t);
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_ref_clk_master].entry_get = dnx_data_fabric_blocks_fsrd_ref_clk_master_entry_str_get;

    
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_ref_clk_master].nof_keys = 1;
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_ref_clk_master].keys[0].name = "fsrd_index";
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_ref_clk_master].keys[0].doc = "FSRD ID";

    
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_ref_clk_master].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_ref_clk_master].values, dnxc_data_value_t, submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_ref_clk_master].nof_values, "_dnx_data_fabric_blocks_table_fsrd_ref_clk_master table values");
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_ref_clk_master].values[0].name = "master_fsrd_index";
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_ref_clk_master].values[0].type = "int";
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_ref_clk_master].values[0].doc = "The index of the Master FSRD which gives ref clock to the key FSRD";
    submodule_data->tables[dnx_data_fabric_blocks_table_fsrd_ref_clk_master].values[0].offset = UTILEX_OFFSETOF(dnx_data_fabric_blocks_fsrd_ref_clk_master_t, master_fsrd_index);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_fabric_blocks_feature_get(
    int unit,
    dnx_data_fabric_blocks_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_blocks, feature);
}


uint32
dnx_data_fabric_blocks_nof_instances_fmac_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_blocks, dnx_data_fabric_blocks_define_nof_instances_fmac);
}

uint32
dnx_data_fabric_blocks_nof_links_in_fmac_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_blocks, dnx_data_fabric_blocks_define_nof_links_in_fmac);
}

uint32
dnx_data_fabric_blocks_nof_instances_fsrd_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_blocks, dnx_data_fabric_blocks_define_nof_instances_fsrd);
}

uint32
dnx_data_fabric_blocks_nof_links_in_fsrd_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_blocks, dnx_data_fabric_blocks_define_nof_links_in_fsrd);
}

uint32
dnx_data_fabric_blocks_nof_fmacs_in_fsrd_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_blocks, dnx_data_fabric_blocks_define_nof_fmacs_in_fsrd);
}

uint32
dnx_data_fabric_blocks_nof_pms_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_blocks, dnx_data_fabric_blocks_define_nof_pms);
}

uint32
dnx_data_fabric_blocks_nof_links_in_pm_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_blocks, dnx_data_fabric_blocks_define_nof_links_in_pm);
}

uint32
dnx_data_fabric_blocks_nof_fmacs_in_pm_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_blocks, dnx_data_fabric_blocks_define_nof_fmacs_in_pm);
}

uint32
dnx_data_fabric_blocks_nof_instances_fdtl_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_blocks, dnx_data_fabric_blocks_define_nof_instances_fdtl);
}

uint32
dnx_data_fabric_blocks_nof_links_in_fdtl_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_blocks, dnx_data_fabric_blocks_define_nof_links_in_fdtl);
}

uint32
dnx_data_fabric_blocks_nof_sbus_chains_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_blocks, dnx_data_fabric_blocks_define_nof_sbus_chains);
}



const dnx_data_fabric_blocks_fsrd_sbus_chain_t *
dnx_data_fabric_blocks_fsrd_sbus_chain_get(
    int unit,
    int fsrd_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_blocks, dnx_data_fabric_blocks_table_fsrd_sbus_chain);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, fsrd_id, 0);
    return (const dnx_data_fabric_blocks_fsrd_sbus_chain_t *) data;

}

const dnx_data_fabric_blocks_fmac_sbus_chain_t *
dnx_data_fabric_blocks_fmac_sbus_chain_get(
    int unit,
    int fmac_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_blocks, dnx_data_fabric_blocks_table_fmac_sbus_chain);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, fmac_id, 0);
    return (const dnx_data_fabric_blocks_fmac_sbus_chain_t *) data;

}

const dnx_data_fabric_blocks_fsrd_ref_clk_master_t *
dnx_data_fabric_blocks_fsrd_ref_clk_master_get(
    int unit,
    int fsrd_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_blocks, dnx_data_fabric_blocks_table_fsrd_ref_clk_master);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, fsrd_index, 0);
    return (const dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) data;

}


shr_error_e
dnx_data_fabric_blocks_fsrd_sbus_chain_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_fabric_blocks_fsrd_sbus_chain_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_blocks, dnx_data_fabric_blocks_table_fsrd_sbus_chain);
    data = (const dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->chain);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->index_in_chain);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_fabric_blocks_fmac_sbus_chain_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_fabric_blocks_fmac_sbus_chain_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_blocks, dnx_data_fabric_blocks_table_fmac_sbus_chain);
    data = (const dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->chain);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->index_in_chain);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_fabric_blocks_fsrd_ref_clk_master_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_fabric_blocks_fsrd_ref_clk_master_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_blocks, dnx_data_fabric_blocks_table_fsrd_ref_clk_master);
    data = (const dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->master_fsrd_index);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_fabric_blocks_fsrd_sbus_chain_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_blocks, dnx_data_fabric_blocks_table_fsrd_sbus_chain);

}

const dnxc_data_table_info_t *
dnx_data_fabric_blocks_fmac_sbus_chain_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_blocks, dnx_data_fabric_blocks_table_fmac_sbus_chain);

}

const dnxc_data_table_info_t *
dnx_data_fabric_blocks_fsrd_ref_clk_master_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_blocks, dnx_data_fabric_blocks_table_fsrd_ref_clk_master);

}






static shr_error_e
dnx_data_fabric_reachability_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "reachability";
    submodule_data->doc = "General reachability attributes";
    
    submodule_data->nof_features = _dnx_data_fabric_reachability_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data fabric reachability features");

    
    submodule_data->nof_defines = _dnx_data_fabric_reachability_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data fabric reachability defines");

    submodule_data->defines[dnx_data_fabric_reachability_define_resolution].name = "resolution";
    submodule_data->defines[dnx_data_fabric_reachability_define_resolution].doc = "Reachability messages resolution.";
    
    submodule_data->defines[dnx_data_fabric_reachability_define_resolution].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_reachability_define_gen_period].name = "gen_period";
    submodule_data->defines[dnx_data_fabric_reachability_define_gen_period].doc = "Reachability message generation period (in units of chip clocks).";
    
    submodule_data->defines[dnx_data_fabric_reachability_define_gen_period].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_reachability_define_full_cycle_period_usec].name = "full_cycle_period_usec";
    submodule_data->defines[dnx_data_fabric_reachability_define_full_cycle_period_usec].doc = "Full cycle of reachability cells period (in units of micro-seconds).";
    
    submodule_data->defines[dnx_data_fabric_reachability_define_full_cycle_period_usec].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_reachability_define_watchdog_resolution].name = "watchdog_resolution";
    submodule_data->defines[dnx_data_fabric_reachability_define_watchdog_resolution].doc = "Reachability message watchdog resolution..";
    
    submodule_data->defines[dnx_data_fabric_reachability_define_watchdog_resolution].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_reachability_define_watchdog_period_usec].name = "watchdog_period_usec";
    submodule_data->defines[dnx_data_fabric_reachability_define_watchdog_period_usec].doc = "Reachability message watchdog period (in units of micto-seconds).";
    
    submodule_data->defines[dnx_data_fabric_reachability_define_watchdog_period_usec].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_fabric_reachability_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data fabric reachability tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_fabric_reachability_feature_get(
    int unit,
    dnx_data_fabric_reachability_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_reachability, feature);
}


uint32
dnx_data_fabric_reachability_resolution_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_reachability, dnx_data_fabric_reachability_define_resolution);
}

uint32
dnx_data_fabric_reachability_gen_period_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_reachability, dnx_data_fabric_reachability_define_gen_period);
}

uint32
dnx_data_fabric_reachability_full_cycle_period_usec_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_reachability, dnx_data_fabric_reachability_define_full_cycle_period_usec);
}

uint32
dnx_data_fabric_reachability_watchdog_resolution_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_reachability, dnx_data_fabric_reachability_define_watchdog_resolution);
}

uint32
dnx_data_fabric_reachability_watchdog_period_usec_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_reachability, dnx_data_fabric_reachability_define_watchdog_period_usec);
}










static shr_error_e
dnx_data_fabric_mesh_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "mesh";
    submodule_data->doc = "MESH mode attributes";
    
    submodule_data->nof_features = _dnx_data_fabric_mesh_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data fabric mesh features");

    submodule_data->features[dnx_data_fabric_mesh_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_fabric_mesh_is_supported].doc = "Is mesh supported";
    submodule_data->features[dnx_data_fabric_mesh_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_fabric_mesh_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data fabric mesh defines");

    submodule_data->defines[dnx_data_fabric_mesh_define_max_mc_id].name = "max_mc_id";
    submodule_data->defines[dnx_data_fabric_mesh_define_max_mc_id].doc = "Maximum MC ID in MESH mode";
    
    submodule_data->defines[dnx_data_fabric_mesh_define_max_mc_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_mesh_define_max_nof_dest].name = "max_nof_dest";
    submodule_data->defines[dnx_data_fabric_mesh_define_max_nof_dest].doc = "Maximum number of FAP destination in Mesh mode";
    
    submodule_data->defines[dnx_data_fabric_mesh_define_max_nof_dest].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_mesh_define_multicast_enable].name = "multicast_enable";
    submodule_data->defines[dnx_data_fabric_mesh_define_multicast_enable].doc = "Detemines whether the MESH system supports MC";
    
    submodule_data->defines[dnx_data_fabric_mesh_define_multicast_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_fabric_mesh_define_nof_dest].name = "nof_dest";
    submodule_data->defines[dnx_data_fabric_mesh_define_nof_dest].doc = "Number of FAP destination in MESH mode";
    
    submodule_data->defines[dnx_data_fabric_mesh_define_nof_dest].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_fabric_mesh_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data fabric mesh tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_fabric_mesh_feature_get(
    int unit,
    dnx_data_fabric_mesh_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_mesh, feature);
}


uint32
dnx_data_fabric_mesh_max_mc_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_mesh, dnx_data_fabric_mesh_define_max_mc_id);
}

uint32
dnx_data_fabric_mesh_max_nof_dest_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_mesh, dnx_data_fabric_mesh_define_max_nof_dest);
}

uint32
dnx_data_fabric_mesh_multicast_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_mesh, dnx_data_fabric_mesh_define_multicast_enable);
}

uint32
dnx_data_fabric_mesh_nof_dest_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_mesh, dnx_data_fabric_mesh_define_nof_dest);
}










static shr_error_e
dnx_data_fabric_cell_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "cell";
    submodule_data->doc = "cell attributes";
    
    submodule_data->nof_features = _dnx_data_fabric_cell_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data fabric cell features");

    
    submodule_data->nof_defines = _dnx_data_fabric_cell_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data fabric cell defines");

    submodule_data->defines[dnx_data_fabric_cell_define_nof_priorities].name = "nof_priorities";
    submodule_data->defines[dnx_data_fabric_cell_define_nof_priorities].doc = "nof of supported fabric priorities";
    
    submodule_data->defines[dnx_data_fabric_cell_define_nof_priorities].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_cell_define_vsc256_max_size].name = "vsc256_max_size";
    submodule_data->defines[dnx_data_fabric_cell_define_vsc256_max_size].doc = "Max cell size in VSC256 configuration";
    
    submodule_data->defines[dnx_data_fabric_cell_define_vsc256_max_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_cell_define_sr_cell_header_offset].name = "sr_cell_header_offset";
    submodule_data->defines[dnx_data_fabric_cell_define_sr_cell_header_offset].doc = "Offset in the header fields of sr cells (bytes)";
    
    submodule_data->defines[dnx_data_fabric_cell_define_sr_cell_header_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_cell_define_sr_cell_nof_instances].name = "sr_cell_nof_instances";
    submodule_data->defines[dnx_data_fabric_cell_define_sr_cell_nof_instances].doc = "Number of source routed cell instances";
    
    submodule_data->defines[dnx_data_fabric_cell_define_sr_cell_nof_instances].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_cell_define_default_packing_mode].name = "default_packing_mode";
    submodule_data->defines[dnx_data_fabric_cell_define_default_packing_mode].doc = "Default packing mode";
    
    submodule_data->defines[dnx_data_fabric_cell_define_default_packing_mode].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_fabric_cell_define_pcp_enable].name = "pcp_enable";
    submodule_data->defines[dnx_data_fabric_cell_define_pcp_enable].doc = "packet cell packing enable";
    
    submodule_data->defines[dnx_data_fabric_cell_define_pcp_enable].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_fabric_cell_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data fabric cell tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_fabric_cell_feature_get(
    int unit,
    dnx_data_fabric_cell_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_cell, feature);
}


uint32
dnx_data_fabric_cell_nof_priorities_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_cell, dnx_data_fabric_cell_define_nof_priorities);
}

uint32
dnx_data_fabric_cell_vsc256_max_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_cell, dnx_data_fabric_cell_define_vsc256_max_size);
}

uint32
dnx_data_fabric_cell_sr_cell_header_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_cell, dnx_data_fabric_cell_define_sr_cell_header_offset);
}

uint32
dnx_data_fabric_cell_sr_cell_nof_instances_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_cell, dnx_data_fabric_cell_define_sr_cell_nof_instances);
}

uint32
dnx_data_fabric_cell_default_packing_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_cell, dnx_data_fabric_cell_define_default_packing_mode);
}

uint32
dnx_data_fabric_cell_pcp_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_cell, dnx_data_fabric_cell_define_pcp_enable);
}










static shr_error_e
dnx_data_fabric_pipes_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "pipes";
    submodule_data->doc = "pipes attributes";
    
    submodule_data->nof_features = _dnx_data_fabric_pipes_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data fabric pipes features");

    submodule_data->features[dnx_data_fabric_pipes_ocb_wfq].name = "ocb_wfq";
    submodule_data->features[dnx_data_fabric_pipes_ocb_wfq].doc = "is there a WFQ between OCB contexts in the ingress";
    submodule_data->features[dnx_data_fabric_pipes_ocb_wfq].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_fabric_pipes_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data fabric pipes defines");

    submodule_data->defines[dnx_data_fabric_pipes_define_max_nof_pipes].name = "max_nof_pipes";
    submodule_data->defines[dnx_data_fabric_pipes_define_max_nof_pipes].doc = "max supported fabric pipes";
    
    submodule_data->defines[dnx_data_fabric_pipes_define_max_nof_pipes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_pipes_define_max_nof_contexts].name = "max_nof_contexts";
    submodule_data->defines[dnx_data_fabric_pipes_define_max_nof_contexts].doc = "max supported ingress contexts";
    
    submodule_data->defines[dnx_data_fabric_pipes_define_max_nof_contexts].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_pipes_define_max_nof_subcontexts].name = "max_nof_subcontexts";
    submodule_data->defines[dnx_data_fabric_pipes_define_max_nof_subcontexts].doc = "max supported ingress sub contexts";
    
    submodule_data->defines[dnx_data_fabric_pipes_define_max_nof_subcontexts].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_pipes_define_nof_pipes].name = "nof_pipes";
    submodule_data->defines[dnx_data_fabric_pipes_define_nof_pipes].doc = "number of actual (configured) pipes";
    
    submodule_data->defines[dnx_data_fabric_pipes_define_nof_pipes].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_fabric_pipes_define_multi_pipe_system].name = "multi_pipe_system";
    submodule_data->defines[dnx_data_fabric_pipes_define_multi_pipe_system].doc = "Is there a device in the system in multi pipes mode";
    
    submodule_data->defines[dnx_data_fabric_pipes_define_multi_pipe_system].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_fabric_pipes_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data fabric pipes tables");

    
    submodule_data->tables[dnx_data_fabric_pipes_table_map].name = "map";
    submodule_data->tables[dnx_data_fabric_pipes_table_map].doc = "map traffic cast X priority to fabric pipe";
    submodule_data->tables[dnx_data_fabric_pipes_table_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_fabric_pipes_table_map].size_of_values = sizeof(dnx_data_fabric_pipes_map_t);
    submodule_data->tables[dnx_data_fabric_pipes_table_map].entry_get = dnx_data_fabric_pipes_map_entry_str_get;

    
    submodule_data->tables[dnx_data_fabric_pipes_table_map].nof_keys = 0;

    
    submodule_data->tables[dnx_data_fabric_pipes_table_map].nof_values = 6;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_fabric_pipes_table_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_fabric_pipes_table_map].nof_values, "_dnx_data_fabric_pipes_table_map table values");
    submodule_data->tables[dnx_data_fabric_pipes_table_map].values[0].name = "uc";
    submodule_data->tables[dnx_data_fabric_pipes_table_map].values[0].type = "uint32[DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES]";
    submodule_data->tables[dnx_data_fabric_pipes_table_map].values[0].doc = "each element represents uc priority pipe";
    submodule_data->tables[dnx_data_fabric_pipes_table_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_fabric_pipes_map_t, uc);
    submodule_data->tables[dnx_data_fabric_pipes_table_map].values[1].name = "mc";
    submodule_data->tables[dnx_data_fabric_pipes_table_map].values[1].type = "uint32[DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES]";
    submodule_data->tables[dnx_data_fabric_pipes_table_map].values[1].doc = "each element represents mc priority pipe";
    submodule_data->tables[dnx_data_fabric_pipes_table_map].values[1].offset = UTILEX_OFFSETOF(dnx_data_fabric_pipes_map_t, mc);
    submodule_data->tables[dnx_data_fabric_pipes_table_map].values[2].name = "name";
    submodule_data->tables[dnx_data_fabric_pipes_table_map].values[2].type = "char *";
    submodule_data->tables[dnx_data_fabric_pipes_table_map].values[2].doc = "The configuration display name";
    submodule_data->tables[dnx_data_fabric_pipes_table_map].values[2].offset = UTILEX_OFFSETOF(dnx_data_fabric_pipes_map_t, name);
    submodule_data->tables[dnx_data_fabric_pipes_table_map].values[3].name = "type";
    submodule_data->tables[dnx_data_fabric_pipes_table_map].values[3].type = "soc_dnxc_fabric_pipe_map_type_t";
    submodule_data->tables[dnx_data_fabric_pipes_table_map].values[3].doc = "The pipe mapping type";
    submodule_data->tables[dnx_data_fabric_pipes_table_map].values[3].offset = UTILEX_OFFSETOF(dnx_data_fabric_pipes_map_t, type);
    submodule_data->tables[dnx_data_fabric_pipes_table_map].values[4].name = "min_hp_mc";
    submodule_data->tables[dnx_data_fabric_pipes_table_map].values[4].type = "int";
    submodule_data->tables[dnx_data_fabric_pipes_table_map].values[4].doc = "Minimum priority to be considered as high priority multicast";
    submodule_data->tables[dnx_data_fabric_pipes_table_map].values[4].offset = UTILEX_OFFSETOF(dnx_data_fabric_pipes_map_t, min_hp_mc);
    submodule_data->tables[dnx_data_fabric_pipes_table_map].values[5].name = "tdm_pipe";
    submodule_data->tables[dnx_data_fabric_pipes_table_map].values[5].type = "int";
    submodule_data->tables[dnx_data_fabric_pipes_table_map].values[5].doc = "TDM pipe id";
    submodule_data->tables[dnx_data_fabric_pipes_table_map].values[5].offset = UTILEX_OFFSETOF(dnx_data_fabric_pipes_map_t, tdm_pipe);

    
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].name = "valid_map_config";
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].doc = "pipes map valid configurations";
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].size_of_values = sizeof(dnx_data_fabric_pipes_valid_map_config_t);
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].entry_get = dnx_data_fabric_pipes_valid_map_config_entry_str_get;

    
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].nof_keys = 1;
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].keys[0].name = "config_id";
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].keys[0].doc = "The configuration running number";

    
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].nof_values = 7;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].values, dnxc_data_value_t, submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].nof_values, "_dnx_data_fabric_pipes_table_valid_map_config table values");
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].values[0].name = "nof_pipes";
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].values[0].type = "uint32";
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].values[0].doc = "Number of pipes";
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].values[0].offset = UTILEX_OFFSETOF(dnx_data_fabric_pipes_valid_map_config_t, nof_pipes);
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].values[1].name = "uc";
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].values[1].type = "uint32[DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES]";
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].values[1].doc = "map unicast cell priority to pipe";
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].values[1].offset = UTILEX_OFFSETOF(dnx_data_fabric_pipes_valid_map_config_t, uc);
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].values[2].name = "mc";
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].values[2].type = "uint32[DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES]";
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].values[2].doc = "map multicast cell priority to pipe";
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].values[2].offset = UTILEX_OFFSETOF(dnx_data_fabric_pipes_valid_map_config_t, mc);
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].values[3].name = "name";
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].values[3].type = "char *";
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].values[3].doc = "The configuration display name";
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].values[3].offset = UTILEX_OFFSETOF(dnx_data_fabric_pipes_valid_map_config_t, name);
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].values[4].name = "type";
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].values[4].type = "soc_dnxc_fabric_pipe_map_type_t";
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].values[4].doc = "The pipe mapping type";
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].values[4].offset = UTILEX_OFFSETOF(dnx_data_fabric_pipes_valid_map_config_t, type);
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].values[5].name = "min_hp_mc";
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].values[5].type = "int";
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].values[5].doc = "Minimum priority to be considered as high priority multicast";
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].values[5].offset = UTILEX_OFFSETOF(dnx_data_fabric_pipes_valid_map_config_t, min_hp_mc);
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].values[6].name = "tdm_pipe";
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].values[6].type = "int";
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].values[6].doc = "TDM pipe id";
    submodule_data->tables[dnx_data_fabric_pipes_table_valid_map_config].values[6].offset = UTILEX_OFFSETOF(dnx_data_fabric_pipes_valid_map_config_t, tdm_pipe);

    
    submodule_data->tables[dnx_data_fabric_pipes_table_dtq_subcontexts].name = "dtq_subcontexts";
    submodule_data->tables[dnx_data_fabric_pipes_table_dtq_subcontexts].doc = "DTQ subcontexts ids";
    submodule_data->tables[dnx_data_fabric_pipes_table_dtq_subcontexts].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_fabric_pipes_table_dtq_subcontexts].size_of_values = sizeof(dnx_data_fabric_pipes_dtq_subcontexts_t);
    submodule_data->tables[dnx_data_fabric_pipes_table_dtq_subcontexts].entry_get = dnx_data_fabric_pipes_dtq_subcontexts_entry_str_get;

    
    submodule_data->tables[dnx_data_fabric_pipes_table_dtq_subcontexts].nof_keys = 2;
    submodule_data->tables[dnx_data_fabric_pipes_table_dtq_subcontexts].keys[0].name = "dtq";
    submodule_data->tables[dnx_data_fabric_pipes_table_dtq_subcontexts].keys[0].doc = "The DTQ number";
    submodule_data->tables[dnx_data_fabric_pipes_table_dtq_subcontexts].keys[1].name = "is_ocb_only";
    submodule_data->tables[dnx_data_fabric_pipes_table_dtq_subcontexts].keys[1].doc = "Whether the subcontext is OCB only or Mixed";

    
    submodule_data->tables[dnx_data_fabric_pipes_table_dtq_subcontexts].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_fabric_pipes_table_dtq_subcontexts].values, dnxc_data_value_t, submodule_data->tables[dnx_data_fabric_pipes_table_dtq_subcontexts].nof_values, "_dnx_data_fabric_pipes_table_dtq_subcontexts table values");
    submodule_data->tables[dnx_data_fabric_pipes_table_dtq_subcontexts].values[0].name = "index";
    submodule_data->tables[dnx_data_fabric_pipes_table_dtq_subcontexts].values[0].type = "uint32";
    submodule_data->tables[dnx_data_fabric_pipes_table_dtq_subcontexts].values[0].doc = "The index of the subcontext";
    submodule_data->tables[dnx_data_fabric_pipes_table_dtq_subcontexts].values[0].offset = UTILEX_OFFSETOF(dnx_data_fabric_pipes_dtq_subcontexts_t, index);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_fabric_pipes_feature_get(
    int unit,
    dnx_data_fabric_pipes_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_pipes, feature);
}


uint32
dnx_data_fabric_pipes_max_nof_pipes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_pipes, dnx_data_fabric_pipes_define_max_nof_pipes);
}

uint32
dnx_data_fabric_pipes_max_nof_contexts_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_pipes, dnx_data_fabric_pipes_define_max_nof_contexts);
}

uint32
dnx_data_fabric_pipes_max_nof_subcontexts_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_pipes, dnx_data_fabric_pipes_define_max_nof_subcontexts);
}

uint32
dnx_data_fabric_pipes_nof_pipes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_pipes, dnx_data_fabric_pipes_define_nof_pipes);
}

uint32
dnx_data_fabric_pipes_multi_pipe_system_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_pipes, dnx_data_fabric_pipes_define_multi_pipe_system);
}



const dnx_data_fabric_pipes_map_t *
dnx_data_fabric_pipes_map_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_pipes, dnx_data_fabric_pipes_table_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_fabric_pipes_map_t *) data;

}

const dnx_data_fabric_pipes_valid_map_config_t *
dnx_data_fabric_pipes_valid_map_config_get(
    int unit,
    int config_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_pipes, dnx_data_fabric_pipes_table_valid_map_config);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, config_id, 0);
    return (const dnx_data_fabric_pipes_valid_map_config_t *) data;

}

const dnx_data_fabric_pipes_dtq_subcontexts_t *
dnx_data_fabric_pipes_dtq_subcontexts_get(
    int unit,
    int dtq,
    int is_ocb_only)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_pipes, dnx_data_fabric_pipes_table_dtq_subcontexts);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dtq, is_ocb_only);
    return (const dnx_data_fabric_pipes_dtq_subcontexts_t *) data;

}


shr_error_e
dnx_data_fabric_pipes_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_fabric_pipes_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_pipes, dnx_data_fabric_pipes_table_map);
    data = (const dnx_data_fabric_pipes_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, data->uc);
            break;
        case 1:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, data->mc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->name == NULL ? "" : data->name);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->type);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->min_hp_mc);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tdm_pipe);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_fabric_pipes_valid_map_config_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_fabric_pipes_valid_map_config_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_pipes, dnx_data_fabric_pipes_table_valid_map_config);
    data = (const dnx_data_fabric_pipes_valid_map_config_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_pipes);
            break;
        case 1:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, data->uc);
            break;
        case 2:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, data->mc);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->name == NULL ? "" : data->name);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->type);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->min_hp_mc);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tdm_pipe);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_fabric_pipes_dtq_subcontexts_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_fabric_pipes_dtq_subcontexts_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_pipes, dnx_data_fabric_pipes_table_dtq_subcontexts);
    data = (const dnx_data_fabric_pipes_dtq_subcontexts_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->index);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_fabric_pipes_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_pipes, dnx_data_fabric_pipes_table_map);

}

const dnxc_data_table_info_t *
dnx_data_fabric_pipes_valid_map_config_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_pipes, dnx_data_fabric_pipes_table_valid_map_config);

}

const dnxc_data_table_info_t *
dnx_data_fabric_pipes_dtq_subcontexts_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_pipes, dnx_data_fabric_pipes_table_dtq_subcontexts);

}






static shr_error_e
dnx_data_fabric_debug_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "debug";
    submodule_data->doc = "debug configurations";
    
    submodule_data->nof_features = _dnx_data_fabric_debug_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data fabric debug features");

    
    submodule_data->nof_defines = _dnx_data_fabric_debug_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data fabric debug defines");

    submodule_data->defines[dnx_data_fabric_debug_define_mesh_topology_size].name = "mesh_topology_size";
    submodule_data->defines[dnx_data_fabric_debug_define_mesh_topology_size].doc = "Size of mesh topology";
    
    submodule_data->defines[dnx_data_fabric_debug_define_mesh_topology_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_fabric_debug_define_mesh_topology_fast].name = "mesh_topology_fast";
    submodule_data->defines[dnx_data_fabric_debug_define_mesh_topology_fast].doc = "Device is in mesh topology fast mode.";
    
    submodule_data->defines[dnx_data_fabric_debug_define_mesh_topology_fast].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_fabric_debug_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data fabric debug tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_fabric_debug_feature_get(
    int unit,
    dnx_data_fabric_debug_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_debug, feature);
}


int
dnx_data_fabric_debug_mesh_topology_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_debug, dnx_data_fabric_debug_define_mesh_topology_size);
}

uint32
dnx_data_fabric_debug_mesh_topology_fast_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_debug, dnx_data_fabric_debug_define_mesh_topology_fast);
}










static shr_error_e
dnx_data_fabric_mesh_topology_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "mesh_topology";
    submodule_data->doc = "mesh topology configurations";
    
    submodule_data->nof_features = _dnx_data_fabric_mesh_topology_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data fabric mesh_topology features");

    submodule_data->features[dnx_data_fabric_mesh_topology_calendar_mode_supported].name = "calendar_mode_supported";
    submodule_data->features[dnx_data_fabric_mesh_topology_calendar_mode_supported].doc = "Is calendar mode supported";
    submodule_data->features[dnx_data_fabric_mesh_topology_calendar_mode_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_fabric_mesh_topology_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data fabric mesh_topology defines");

    submodule_data->defines[dnx_data_fabric_mesh_topology_define_mesh_init].name = "mesh_init";
    submodule_data->defines[dnx_data_fabric_mesh_topology_define_mesh_init].doc = "mesh init value";
    
    submodule_data->defines[dnx_data_fabric_mesh_topology_define_mesh_init].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_mesh_topology_define_mesh_config_1].name = "mesh_config_1";
    submodule_data->defines[dnx_data_fabric_mesh_topology_define_mesh_config_1].doc = "mesh config 1 value";
    
    submodule_data->defines[dnx_data_fabric_mesh_topology_define_mesh_config_1].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_fabric_mesh_topology_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data fabric mesh_topology tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_fabric_mesh_topology_feature_get(
    int unit,
    dnx_data_fabric_mesh_topology_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_mesh_topology, feature);
}


uint32
dnx_data_fabric_mesh_topology_mesh_init_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_mesh_topology, dnx_data_fabric_mesh_topology_define_mesh_init);
}

uint32
dnx_data_fabric_mesh_topology_mesh_config_1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_mesh_topology, dnx_data_fabric_mesh_topology_define_mesh_config_1);
}










static shr_error_e
dnx_data_fabric_tdm_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "tdm";
    submodule_data->doc = "TDM attributes";
    
    submodule_data->nof_features = _dnx_data_fabric_tdm_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data fabric tdm features");

    
    submodule_data->nof_defines = _dnx_data_fabric_tdm_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data fabric tdm defines");

    submodule_data->defines[dnx_data_fabric_tdm_define_priority].name = "priority";
    submodule_data->defines[dnx_data_fabric_tdm_define_priority].doc = "The TDM fabric priority";
    
    submodule_data->defines[dnx_data_fabric_tdm_define_priority].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_fabric_tdm_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data fabric tdm tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_fabric_tdm_feature_get(
    int unit,
    dnx_data_fabric_tdm_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_tdm, feature);
}


int
dnx_data_fabric_tdm_priority_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_tdm, dnx_data_fabric_tdm_define_priority);
}










static shr_error_e
dnx_data_fabric_cgm_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "cgm";
    submodule_data->doc = "CGM info";
    
    submodule_data->nof_features = _dnx_data_fabric_cgm_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data fabric cgm features");

    submodule_data->features[dnx_data_fabric_cgm_egress_drop_low_prio_mc].name = "egress_drop_low_prio_mc";
    submodule_data->features[dnx_data_fabric_cgm_egress_drop_low_prio_mc].doc = "is egress can drop low priority multicast cells";
    submodule_data->features[dnx_data_fabric_cgm_egress_drop_low_prio_mc].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_fabric_cgm_egress_fifo_static_size].name = "egress_fifo_static_size";
    submodule_data->features[dnx_data_fabric_cgm_egress_fifo_static_size].doc = "is egress fifos size is static or can be changed according to number of pipes";
    submodule_data->features[dnx_data_fabric_cgm_egress_fifo_static_size].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_fabric_cgm_is_egress_drop_tdm_latency_supported].name = "is_egress_drop_tdm_latency_supported";
    submodule_data->features[dnx_data_fabric_cgm_is_egress_drop_tdm_latency_supported].doc = "Is egress grop tdm latency supported";
    submodule_data->features[dnx_data_fabric_cgm_is_egress_drop_tdm_latency_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_fabric_cgm_is_static_shaper_supported].name = "is_static_shaper_supported";
    submodule_data->features[dnx_data_fabric_cgm_is_static_shaper_supported].doc = "Is fabric static shaper supported";
    submodule_data->features[dnx_data_fabric_cgm_is_static_shaper_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_fabric_cgm_static_shaper_fix_factor].name = "static_shaper_fix_factor";
    submodule_data->features[dnx_data_fabric_cgm_static_shaper_fix_factor].doc = "Is static shaper supports a fix factor";
    submodule_data->features[dnx_data_fabric_cgm_static_shaper_fix_factor].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_fabric_cgm_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data fabric cgm defines");

    submodule_data->defines[dnx_data_fabric_cgm_define_nof_rci_levels].name = "nof_rci_levels";
    submodule_data->defines[dnx_data_fabric_cgm_define_nof_rci_levels].doc = "Number of RCI levels";
    
    submodule_data->defines[dnx_data_fabric_cgm_define_nof_rci_levels].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_cgm_define_nof_rci_threshold_levels].name = "nof_rci_threshold_levels";
    submodule_data->defines[dnx_data_fabric_cgm_define_nof_rci_threshold_levels].doc = "Number of RCI Threshold levels";
    
    submodule_data->defines[dnx_data_fabric_cgm_define_nof_rci_threshold_levels].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_cgm_define_egress_drop_th_clocks_resolution].name = "egress_drop_th_clocks_resolution";
    submodule_data->defines[dnx_data_fabric_cgm_define_egress_drop_th_clocks_resolution].doc = "Clocks resolution in HW";
    
    submodule_data->defines[dnx_data_fabric_cgm_define_egress_drop_th_clocks_resolution].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_cgm_define_egress_fifo_fc_threshold].name = "egress_fifo_fc_threshold";
    submodule_data->defines[dnx_data_fabric_cgm_define_egress_fifo_fc_threshold].doc = "Threshold for egress fifo flow control (the threshold below the fifo size)";
    
    submodule_data->defines[dnx_data_fabric_cgm_define_egress_fifo_fc_threshold].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_cgm_define_static_shaper_credit_resolution_bytes].name = "static_shaper_credit_resolution_bytes";
    submodule_data->defines[dnx_data_fabric_cgm_define_static_shaper_credit_resolution_bytes].doc = "The credit resoultion of the static shaper [bytes]";
    
    submodule_data->defines[dnx_data_fabric_cgm_define_static_shaper_credit_resolution_bytes].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_fabric_cgm_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data fabric cgm tables");

    
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].name = "access_map";
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].doc = "DBAL information per threshold type";
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].size_of_values = sizeof(dnx_data_fabric_cgm_access_map_t);
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].entry_get = dnx_data_fabric_cgm_access_map_entry_str_get;

    
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].nof_keys = 1;
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].keys[0].name = "control_type";
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].keys[0].doc = "Fabric CGM control type";

    
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].nof_values = 7;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_fabric_cgm_table_access_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_fabric_cgm_table_access_map].nof_values, "_dnx_data_fabric_cgm_table_access_map table values");
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].values[0].type = "dbal_tables_e";
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].values[0].doc = "DBAL table";
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_fabric_cgm_access_map_t, dbal_table);
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].values[1].name = "dbal_key1";
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].values[1].type = "dbal_fields_e";
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].values[1].doc = "DBAL key1";
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].values[1].offset = UTILEX_OFFSETOF(dnx_data_fabric_cgm_access_map_t, dbal_key1);
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].values[2].name = "dbal_key2";
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].values[2].type = "dbal_fields_e";
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].values[2].doc = "DBAL key2";
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].values[2].offset = UTILEX_OFFSETOF(dnx_data_fabric_cgm_access_map_t, dbal_key2);
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].values[3].name = "dbal_result";
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].values[3].type = "dbal_fields_e";
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].values[3].doc = "DBAL result";
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].values[3].offset = UTILEX_OFFSETOF(dnx_data_fabric_cgm_access_map_t, dbal_result);
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].values[4].name = "index_type";
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].values[4].type = "dnx_fabric_cgm_control_index_type_e";
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].values[4].doc = "DBAL table index type";
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].values[4].offset = UTILEX_OFFSETOF(dnx_data_fabric_cgm_access_map_t, index_type);
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].values[5].name = "is_valid";
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].values[5].type = "int";
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].values[5].doc = "is control_type valid?";
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].values[5].offset = UTILEX_OFFSETOF(dnx_data_fabric_cgm_access_map_t, is_valid);
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].values[6].name = "err_msg";
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].values[6].type = "char *";
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].values[6].doc = "error message to be printed when using invalid control_type";
    submodule_data->tables[dnx_data_fabric_cgm_table_access_map].values[6].offset = UTILEX_OFFSETOF(dnx_data_fabric_cgm_access_map_t, err_msg);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_fabric_cgm_feature_get(
    int unit,
    dnx_data_fabric_cgm_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_cgm, feature);
}


uint32
dnx_data_fabric_cgm_nof_rci_levels_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_cgm, dnx_data_fabric_cgm_define_nof_rci_levels);
}

uint32
dnx_data_fabric_cgm_nof_rci_threshold_levels_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_cgm, dnx_data_fabric_cgm_define_nof_rci_threshold_levels);
}

uint32
dnx_data_fabric_cgm_egress_drop_th_clocks_resolution_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_cgm, dnx_data_fabric_cgm_define_egress_drop_th_clocks_resolution);
}

uint32
dnx_data_fabric_cgm_egress_fifo_fc_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_cgm, dnx_data_fabric_cgm_define_egress_fifo_fc_threshold);
}

uint32
dnx_data_fabric_cgm_static_shaper_credit_resolution_bytes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_cgm, dnx_data_fabric_cgm_define_static_shaper_credit_resolution_bytes);
}



const dnx_data_fabric_cgm_access_map_t *
dnx_data_fabric_cgm_access_map_get(
    int unit,
    int control_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_cgm, dnx_data_fabric_cgm_table_access_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, control_type, 0);
    return (const dnx_data_fabric_cgm_access_map_t *) data;

}


shr_error_e
dnx_data_fabric_cgm_access_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_fabric_cgm_access_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_cgm, dnx_data_fabric_cgm_table_access_map);
    data = (const dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_key1);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_key2);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_result);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->index_type);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->err_msg == NULL ? "" : data->err_msg);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_fabric_cgm_access_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_cgm, dnx_data_fabric_cgm_table_access_map);

}






static shr_error_e
dnx_data_fabric_dtqs_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "dtqs";
    submodule_data->doc = "DTQs info";
    
    submodule_data->nof_features = _dnx_data_fabric_dtqs_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data fabric dtqs features");

    
    submodule_data->nof_defines = _dnx_data_fabric_dtqs_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data fabric dtqs defines");

    submodule_data->defines[dnx_data_fabric_dtqs_define_max_nof_subdtqs].name = "max_nof_subdtqs";
    submodule_data->defines[dnx_data_fabric_dtqs_define_max_nof_subdtqs].doc = "Number of sub-DTQs";
    
    submodule_data->defines[dnx_data_fabric_dtqs_define_max_nof_subdtqs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_dtqs_define_max_size].name = "max_size";
    submodule_data->defines[dnx_data_fabric_dtqs_define_max_size].doc = "Maximum size for all DTQs together";
    
    submodule_data->defines[dnx_data_fabric_dtqs_define_max_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_dtqs_define_ocb_only_size].name = "ocb_only_size";
    submodule_data->defines[dnx_data_fabric_dtqs_define_ocb_only_size].doc = "Size of OCB-Only DTQs";
    
    submodule_data->defines[dnx_data_fabric_dtqs_define_ocb_only_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_dtqs_define_nof_active_dtqs].name = "nof_active_dtqs";
    submodule_data->defines[dnx_data_fabric_dtqs_define_nof_active_dtqs].doc = "Number of active DTQs";
    
    submodule_data->defines[dnx_data_fabric_dtqs_define_nof_active_dtqs].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_fabric_dtqs_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data fabric dtqs tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_fabric_dtqs_feature_get(
    int unit,
    dnx_data_fabric_dtqs_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_dtqs, feature);
}


uint32
dnx_data_fabric_dtqs_max_nof_subdtqs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_dtqs, dnx_data_fabric_dtqs_define_max_nof_subdtqs);
}

uint32
dnx_data_fabric_dtqs_max_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_dtqs, dnx_data_fabric_dtqs_define_max_size);
}

uint32
dnx_data_fabric_dtqs_ocb_only_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_dtqs, dnx_data_fabric_dtqs_define_ocb_only_size);
}

uint32
dnx_data_fabric_dtqs_nof_active_dtqs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_dtqs, dnx_data_fabric_dtqs_define_nof_active_dtqs);
}










static shr_error_e
dnx_data_fabric_ilkn_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ilkn";
    submodule_data->doc = "data about ILKN over fabric";
    
    submodule_data->nof_features = _dnx_data_fabric_ilkn_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data fabric ilkn features");

    
    submodule_data->nof_defines = _dnx_data_fabric_ilkn_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data fabric ilkn defines");

    
    submodule_data->nof_tables = _dnx_data_fabric_ilkn_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data fabric ilkn tables");

    
    submodule_data->tables[dnx_data_fabric_ilkn_table_bypass_info].name = "bypass_info";
    submodule_data->tables[dnx_data_fabric_ilkn_table_bypass_info].doc = "data about bypassed links";
    submodule_data->tables[dnx_data_fabric_ilkn_table_bypass_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_fabric_ilkn_table_bypass_info].size_of_values = sizeof(dnx_data_fabric_ilkn_bypass_info_t);
    submodule_data->tables[dnx_data_fabric_ilkn_table_bypass_info].entry_get = dnx_data_fabric_ilkn_bypass_info_entry_str_get;

    
    submodule_data->tables[dnx_data_fabric_ilkn_table_bypass_info].nof_keys = 1;
    submodule_data->tables[dnx_data_fabric_ilkn_table_bypass_info].keys[0].name = "pm_id";
    submodule_data->tables[dnx_data_fabric_ilkn_table_bypass_info].keys[0].doc = "Internal Port Macro identifier";

    
    submodule_data->tables[dnx_data_fabric_ilkn_table_bypass_info].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_fabric_ilkn_table_bypass_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_fabric_ilkn_table_bypass_info].nof_values, "_dnx_data_fabric_ilkn_table_bypass_info table values");
    submodule_data->tables[dnx_data_fabric_ilkn_table_bypass_info].values[0].name = "links";
    submodule_data->tables[dnx_data_fabric_ilkn_table_bypass_info].values[0].type = "uint32";
    submodule_data->tables[dnx_data_fabric_ilkn_table_bypass_info].values[0].doc = "Links in bypass mode";
    submodule_data->tables[dnx_data_fabric_ilkn_table_bypass_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_fabric_ilkn_bypass_info_t, links);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_fabric_ilkn_feature_get(
    int unit,
    dnx_data_fabric_ilkn_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_ilkn, feature);
}




const dnx_data_fabric_ilkn_bypass_info_t *
dnx_data_fabric_ilkn_bypass_info_get(
    int unit,
    int pm_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_ilkn, dnx_data_fabric_ilkn_table_bypass_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, pm_id, 0);
    return (const dnx_data_fabric_ilkn_bypass_info_t *) data;

}


shr_error_e
dnx_data_fabric_ilkn_bypass_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_fabric_ilkn_bypass_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_ilkn, dnx_data_fabric_ilkn_table_bypass_info);
    data = (const dnx_data_fabric_ilkn_bypass_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->links);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_fabric_ilkn_bypass_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_ilkn, dnx_data_fabric_ilkn_table_bypass_info);

}






static shr_error_e
dnx_data_fabric_dbal_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "dbal";
    submodule_data->doc = "fabric dbal defines";
    
    submodule_data->nof_features = _dnx_data_fabric_dbal_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data fabric dbal features");

    
    submodule_data->nof_defines = _dnx_data_fabric_dbal_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data fabric dbal defines");

    submodule_data->defines[dnx_data_fabric_dbal_define_link_rci_threshold_bits_nof].name = "link_rci_threshold_bits_nof";
    submodule_data->defines[dnx_data_fabric_dbal_define_link_rci_threshold_bits_nof].doc = "number of bits in LINK_RCI_THRESHOLD field";
    
    submodule_data->defines[dnx_data_fabric_dbal_define_link_rci_threshold_bits_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_llfc_threshold_bits_nof].name = "cgm_llfc_threshold_bits_nof";
    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_llfc_threshold_bits_nof].doc = "number of bits in link level flow control thresholds";
    
    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_llfc_threshold_bits_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_llfc_threshold_max_value].name = "cgm_llfc_threshold_max_value";
    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_llfc_threshold_max_value].doc = "maximum value of link level flow control threshold";
    
    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_llfc_threshold_max_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_dbal_define_egress_fifo_drop_threshold_bits_nof].name = "egress_fifo_drop_threshold_bits_nof";
    submodule_data->defines[dnx_data_fabric_dbal_define_egress_fifo_drop_threshold_bits_nof].doc = "number of bits in egress fifo drop thresholds";
    
    submodule_data->defines[dnx_data_fabric_dbal_define_egress_fifo_drop_threshold_bits_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_dbal_define_egress_fifo_drop_threshold_max_value].name = "egress_fifo_drop_threshold_max_value";
    submodule_data->defines[dnx_data_fabric_dbal_define_egress_fifo_drop_threshold_max_value].doc = "maximum value of egress fifo drop threshold";
    
    submodule_data->defines[dnx_data_fabric_dbal_define_egress_fifo_drop_threshold_max_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_link_fifo_base_address_bits_nof].name = "cgm_link_fifo_base_address_bits_nof";
    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_link_fifo_base_address_bits_nof].doc = "number of bits in link fifo base address";
    
    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_link_fifo_base_address_bits_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_link_fifo_base_address_max_value].name = "cgm_link_fifo_base_address_max_value";
    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_link_fifo_base_address_max_value].doc = "maximum value of link fifo base address";
    
    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_link_fifo_base_address_max_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_egress_fifo_base_address_bits_nof].name = "cgm_egress_fifo_base_address_bits_nof";
    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_egress_fifo_base_address_bits_nof].doc = "number of bits in egress fifo base address";
    
    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_egress_fifo_base_address_bits_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_egress_fifo_base_address_max_value].name = "cgm_egress_fifo_base_address_max_value";
    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_egress_fifo_base_address_max_value].doc = "maximum value of egress fifo base address";
    
    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_egress_fifo_base_address_max_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_egress_fifo_fc_threshold_bits_nof].name = "cgm_egress_fifo_fc_threshold_bits_nof";
    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_egress_fifo_fc_threshold_bits_nof].doc = "number of bits in egress fifo flow control threshold";
    
    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_egress_fifo_fc_threshold_bits_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_egress_fifo_fc_threshold_max_value].name = "cgm_egress_fifo_fc_threshold_max_value";
    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_egress_fifo_fc_threshold_max_value].doc = "maximum value of egress fifo flow control threshold";
    
    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_egress_fifo_fc_threshold_max_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_egress_drop_mc_low_prio_th_bits_nof].name = "cgm_egress_drop_mc_low_prio_th_bits_nof";
    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_egress_drop_mc_low_prio_th_bits_nof].doc = "number of bits in low priority multicast cells drop threshold in egress";
    
    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_egress_drop_mc_low_prio_th_bits_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_ingress_fc_threshold_max_value].name = "cgm_ingress_fc_threshold_max_value";
    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_ingress_fc_threshold_max_value].doc = "maximum value of ingress flow control threshold";
    
    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_ingress_fc_threshold_max_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_static_shaper_calendar_bits_nof].name = "cgm_static_shaper_calendar_bits_nof";
    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_static_shaper_calendar_bits_nof].doc = "number of bits in static shaper's calendar";
    
    submodule_data->defines[dnx_data_fabric_dbal_define_cgm_static_shaper_calendar_bits_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_dbal_define_fabric_transmit_wfq_index_bits_nof].name = "fabric_transmit_wfq_index_bits_nof";
    submodule_data->defines[dnx_data_fabric_dbal_define_fabric_transmit_wfq_index_bits_nof].doc = "number of bits in fabric transmit wfq index";
    
    submodule_data->defines[dnx_data_fabric_dbal_define_fabric_transmit_wfq_index_bits_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_dbal_define_fabric_transmit_wfq_index_max_value].name = "fabric_transmit_wfq_index_max_value";
    submodule_data->defines[dnx_data_fabric_dbal_define_fabric_transmit_wfq_index_max_value].doc = "maximum value of fabric transmit wfq index";
    
    submodule_data->defines[dnx_data_fabric_dbal_define_fabric_transmit_wfq_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_dbal_define_mesh_topology_init_config_2_bits_nof].name = "mesh_topology_init_config_2_bits_nof";
    submodule_data->defines[dnx_data_fabric_dbal_define_mesh_topology_init_config_2_bits_nof].doc = "number of bits in mesh topology init config 2";
    
    submodule_data->defines[dnx_data_fabric_dbal_define_mesh_topology_init_config_2_bits_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_dbal_define_fabric_transmit_dtq_fc_thresholds_bits_nof].name = "fabric_transmit_dtq_fc_thresholds_bits_nof";
    submodule_data->defines[dnx_data_fabric_dbal_define_fabric_transmit_dtq_fc_thresholds_bits_nof].doc = "number of bits in fabric transmit dtq fc thresholds";
    
    submodule_data->defines[dnx_data_fabric_dbal_define_fabric_transmit_dtq_fc_thresholds_bits_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_dbal_define_fabric_transmit_pdq_size_bits_nof].name = "fabric_transmit_pdq_size_bits_nof";
    submodule_data->defines[dnx_data_fabric_dbal_define_fabric_transmit_pdq_size_bits_nof].doc = "number of bits in fabric transmit pdq size";
    
    submodule_data->defines[dnx_data_fabric_dbal_define_fabric_transmit_pdq_size_bits_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_dbal_define_priority_bits_nof].name = "priority_bits_nof";
    submodule_data->defines[dnx_data_fabric_dbal_define_priority_bits_nof].doc = "Number of bits in FABRIC_PRIORITY field";
    
    submodule_data->defines[dnx_data_fabric_dbal_define_priority_bits_nof].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_fabric_dbal_define_nof_links_bits_nof].name = "nof_links_bits_nof";
    submodule_data->defines[dnx_data_fabric_dbal_define_nof_links_bits_nof].doc = "Number of bits in FABRIC_LINK_ID field";
    
    submodule_data->defines[dnx_data_fabric_dbal_define_nof_links_bits_nof].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_fabric_dbal_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data fabric dbal tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_fabric_dbal_feature_get(
    int unit,
    dnx_data_fabric_dbal_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_dbal, feature);
}


uint32
dnx_data_fabric_dbal_link_rci_threshold_bits_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_dbal, dnx_data_fabric_dbal_define_link_rci_threshold_bits_nof);
}

uint32
dnx_data_fabric_dbal_cgm_llfc_threshold_bits_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_dbal, dnx_data_fabric_dbal_define_cgm_llfc_threshold_bits_nof);
}

uint32
dnx_data_fabric_dbal_cgm_llfc_threshold_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_dbal, dnx_data_fabric_dbal_define_cgm_llfc_threshold_max_value);
}

uint32
dnx_data_fabric_dbal_egress_fifo_drop_threshold_bits_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_dbal, dnx_data_fabric_dbal_define_egress_fifo_drop_threshold_bits_nof);
}

uint32
dnx_data_fabric_dbal_egress_fifo_drop_threshold_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_dbal, dnx_data_fabric_dbal_define_egress_fifo_drop_threshold_max_value);
}

uint32
dnx_data_fabric_dbal_cgm_link_fifo_base_address_bits_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_dbal, dnx_data_fabric_dbal_define_cgm_link_fifo_base_address_bits_nof);
}

uint32
dnx_data_fabric_dbal_cgm_link_fifo_base_address_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_dbal, dnx_data_fabric_dbal_define_cgm_link_fifo_base_address_max_value);
}

uint32
dnx_data_fabric_dbal_cgm_egress_fifo_base_address_bits_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_dbal, dnx_data_fabric_dbal_define_cgm_egress_fifo_base_address_bits_nof);
}

uint32
dnx_data_fabric_dbal_cgm_egress_fifo_base_address_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_dbal, dnx_data_fabric_dbal_define_cgm_egress_fifo_base_address_max_value);
}

uint32
dnx_data_fabric_dbal_cgm_egress_fifo_fc_threshold_bits_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_dbal, dnx_data_fabric_dbal_define_cgm_egress_fifo_fc_threshold_bits_nof);
}

uint32
dnx_data_fabric_dbal_cgm_egress_fifo_fc_threshold_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_dbal, dnx_data_fabric_dbal_define_cgm_egress_fifo_fc_threshold_max_value);
}

uint32
dnx_data_fabric_dbal_cgm_egress_drop_mc_low_prio_th_bits_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_dbal, dnx_data_fabric_dbal_define_cgm_egress_drop_mc_low_prio_th_bits_nof);
}

uint32
dnx_data_fabric_dbal_cgm_ingress_fc_threshold_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_dbal, dnx_data_fabric_dbal_define_cgm_ingress_fc_threshold_max_value);
}

uint32
dnx_data_fabric_dbal_cgm_static_shaper_calendar_bits_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_dbal, dnx_data_fabric_dbal_define_cgm_static_shaper_calendar_bits_nof);
}

uint32
dnx_data_fabric_dbal_fabric_transmit_wfq_index_bits_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_dbal, dnx_data_fabric_dbal_define_fabric_transmit_wfq_index_bits_nof);
}

uint32
dnx_data_fabric_dbal_fabric_transmit_wfq_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_dbal, dnx_data_fabric_dbal_define_fabric_transmit_wfq_index_max_value);
}

uint32
dnx_data_fabric_dbal_mesh_topology_init_config_2_bits_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_dbal, dnx_data_fabric_dbal_define_mesh_topology_init_config_2_bits_nof);
}

uint32
dnx_data_fabric_dbal_fabric_transmit_dtq_fc_thresholds_bits_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_dbal, dnx_data_fabric_dbal_define_fabric_transmit_dtq_fc_thresholds_bits_nof);
}

uint32
dnx_data_fabric_dbal_fabric_transmit_pdq_size_bits_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_dbal, dnx_data_fabric_dbal_define_fabric_transmit_pdq_size_bits_nof);
}

uint32
dnx_data_fabric_dbal_priority_bits_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_dbal, dnx_data_fabric_dbal_define_priority_bits_nof);
}

uint32
dnx_data_fabric_dbal_nof_links_bits_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_dbal, dnx_data_fabric_dbal_define_nof_links_bits_nof);
}










static shr_error_e
dnx_data_fabric_mesh_mc_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "mesh_mc";
    submodule_data->doc = "fabric mesh mc defines";
    
    submodule_data->nof_features = _dnx_data_fabric_mesh_mc_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data fabric mesh_mc features");

    
    submodule_data->nof_defines = _dnx_data_fabric_mesh_mc_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data fabric mesh_mc defines");

    submodule_data->defines[dnx_data_fabric_mesh_mc_define_nof_replication].name = "nof_replication";
    submodule_data->defines[dnx_data_fabric_mesh_mc_define_nof_replication].doc = "number of replication";
    
    submodule_data->defines[dnx_data_fabric_mesh_mc_define_nof_replication].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_fabric_mesh_mc_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data fabric mesh_mc tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_fabric_mesh_mc_feature_get(
    int unit,
    dnx_data_fabric_mesh_mc_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_mesh_mc, feature);
}


uint32
dnx_data_fabric_mesh_mc_nof_replication_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_mesh_mc, dnx_data_fabric_mesh_mc_define_nof_replication);
}










static shr_error_e
dnx_data_fabric_transmit_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "transmit";
    submodule_data->doc = "fabric transmit defines";
    
    submodule_data->nof_features = _dnx_data_fabric_transmit_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data fabric transmit features");

    submodule_data->features[dnx_data_fabric_transmit_eir_fc_leaky_backet].name = "eir_fc_leaky_backet";
    submodule_data->features[dnx_data_fabric_transmit_eir_fc_leaky_backet].doc = "EIR FC uses leaky bucket mechanism";
    submodule_data->features[dnx_data_fabric_transmit_eir_fc_leaky_backet].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_fabric_transmit_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data fabric transmit defines");

    submodule_data->defines[dnx_data_fabric_transmit_define_s2d_pdq_size].name = "s2d_pdq_size";
    submodule_data->defines[dnx_data_fabric_transmit_define_s2d_pdq_size].doc = "Sram to Dram PDQ size";
    
    submodule_data->defines[dnx_data_fabric_transmit_define_s2d_pdq_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_sram_fc_th].name = "pdq_sram_fc_th";
    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_sram_fc_th].doc = "PDQ to SRAM DQCQ flow control threshold";
    
    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_sram_fc_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_mixs_fc_th].name = "pdq_mixs_fc_th";
    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_mixs_fc_th].doc = "PDQ to MIX-S DQCQ flow control threshold";
    
    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_mixs_fc_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_mixd_fc_th].name = "pdq_mixd_fc_th";
    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_mixd_fc_th].doc = "PDQ to MIX-D DQCQ flow control threshold";
    
    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_mixd_fc_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_mix_mixs_fc_th].name = "pdq_mix_mixs_fc_th";
    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_mix_mixs_fc_th].doc = "PDQ to MIX MIX-S DQCQ flow control threshold";
    
    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_mix_mixs_fc_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_mix_mixd_fc_th].name = "pdq_mix_mixd_fc_th";
    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_mix_mixd_fc_th].doc = "PDQ to MIX MIX-D DQCQ flow control threshold";
    
    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_mix_mixd_fc_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_ocb_fc_th].name = "pdq_ocb_fc_th";
    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_ocb_fc_th].doc = "PDQ OCB flow control threshold";
    
    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_ocb_fc_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_dram_delete_fc_th].name = "pdq_dram_delete_fc_th";
    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_dram_delete_fc_th].doc = "PDQ dram delete flow control threshold";
    
    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_dram_delete_fc_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_sram_delete_fc_th].name = "pdq_sram_delete_fc_th";
    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_sram_delete_fc_th].doc = "PDQ sram delete flow control threshold";
    
    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_sram_delete_fc_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_s2d_fc_th].name = "pdq_s2d_fc_th";
    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_s2d_fc_th].doc = "Sram to Dram PDQ flow control threshold";
    
    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_s2d_fc_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_sram_eir_fc_th].name = "pdq_sram_eir_fc_th";
    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_sram_eir_fc_th].doc = "PDQ to sram EIR flow control";
    
    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_sram_eir_fc_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_mixs_eir_fc_th].name = "pdq_mixs_eir_fc_th";
    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_mixs_eir_fc_th].doc = "PDQ to mix-s EIR flow control";
    
    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_mixs_eir_fc_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_mixd_eir_fc_th].name = "pdq_mixd_eir_fc_th";
    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_mixd_eir_fc_th].doc = "PDQ to mix-d EIR flow control";
    
    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_mixd_eir_fc_th].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_ocb_eir_fc_th].name = "pdq_ocb_eir_fc_th";
    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_ocb_eir_fc_th].doc = "PDQ ocb EIR flow control";
    
    submodule_data->defines[dnx_data_fabric_transmit_define_pdq_ocb_eir_fc_th].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_fabric_transmit_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data fabric transmit tables");

    
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].name = "pdq_to_dqcq_fc_th_select";
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].doc = "Select PDQs to DQCQs FC thresholds index";
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].size_of_values = sizeof(dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_t);
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].entry_get = dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_entry_str_get;

    
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].nof_keys = 1;
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].keys[0].name = "dqcq_priority";
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].keys[0].doc = "DQCQ priority";

    
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].nof_values = 6;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].values, dnxc_data_value_t, submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].nof_values, "_dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select table values");
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].values[0].name = "sram";
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].values[0].type = "uint32";
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].values[0].doc = "SRAM DQCQ";
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].values[0].offset = UTILEX_OFFSETOF(dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_t, sram);
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].values[1].name = "mixs";
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].values[1].type = "uint32";
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].values[1].doc = "MIXS DQCQ";
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].values[1].offset = UTILEX_OFFSETOF(dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_t, mixs);
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].values[2].name = "mixd";
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].values[2].type = "uint32";
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].values[2].doc = "MIXD DQCQ";
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].values[2].offset = UTILEX_OFFSETOF(dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_t, mixd);
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].values[3].name = "mix_mixs";
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].values[3].type = "uint32";
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].values[3].doc = "MIX_MIXS DQCQ";
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].values[3].offset = UTILEX_OFFSETOF(dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_t, mix_mixs);
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].values[4].name = "mix_mixd";
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].values[4].type = "uint32";
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].values[4].doc = "MIX_MIXD DQCQ";
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].values[4].offset = UTILEX_OFFSETOF(dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_t, mix_mixd);
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].values[5].name = "s2d";
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].values[5].type = "uint32";
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].values[5].doc = "S2D DQCQ";
    submodule_data->tables[dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select].values[5].offset = UTILEX_OFFSETOF(dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_t, s2d);

    
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].name = "eir_fc_leaky_bucket";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].doc = "Configure the EIR FC leaky bucket machine";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].size_of_values = sizeof(dnx_data_fabric_transmit_eir_fc_leaky_bucket_t);
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].entry_get = dnx_data_fabric_transmit_eir_fc_leaky_bucket_entry_str_get;

    
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].nof_keys = 0;

    
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].nof_values = 9;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values, dnxc_data_value_t, submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].nof_values, "_dnx_data_fabric_transmit_table_eir_fc_leaky_bucket table values");
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[0].name = "almost_full_th";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[0].type = "uint32";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[0].doc = "FIFO almost full threshold";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[0].offset = UTILEX_OFFSETOF(dnx_data_fabric_transmit_eir_fc_leaky_bucket_t, almost_full_th);
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[1].name = "almost_empty_th";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[1].type = "uint32";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[1].doc = "FIFO almost empty threshold";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[1].offset = UTILEX_OFFSETOF(dnx_data_fabric_transmit_eir_fc_leaky_bucket_t, almost_empty_th);
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[2].name = "bucket_size";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[2].type = "uint32";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[2].doc = "Bucket size";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[2].offset = UTILEX_OFFSETOF(dnx_data_fabric_transmit_eir_fc_leaky_bucket_t, bucket_size);
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[3].name = "bucket_fc_threshold";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[3].type = "uint32";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[3].doc = "Bucket flow control threshold";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[3].offset = UTILEX_OFFSETOF(dnx_data_fabric_transmit_eir_fc_leaky_bucket_t, bucket_fc_threshold);
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[4].name = "period";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[4].type = "uint32";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[4].doc = "Token update period";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[4].offset = UTILEX_OFFSETOF(dnx_data_fabric_transmit_eir_fc_leaky_bucket_t, period);
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[5].name = "tokens_delta0";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[5].type = "uint32";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[5].doc = "Tokens delta 0";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[5].offset = UTILEX_OFFSETOF(dnx_data_fabric_transmit_eir_fc_leaky_bucket_t, tokens_delta0);
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[6].name = "tokens_delta1";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[6].type = "uint32";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[6].doc = "Tokens delta 1";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[6].offset = UTILEX_OFFSETOF(dnx_data_fabric_transmit_eir_fc_leaky_bucket_t, tokens_delta1);
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[7].name = "tokens_delta2";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[7].type = "uint32";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[7].doc = "Tokens delta 2";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[7].offset = UTILEX_OFFSETOF(dnx_data_fabric_transmit_eir_fc_leaky_bucket_t, tokens_delta2);
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[8].name = "tokens_delta3";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[8].type = "uint32";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[8].doc = "Tokens delta 3";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket].values[8].offset = UTILEX_OFFSETOF(dnx_data_fabric_transmit_eir_fc_leaky_bucket_t, tokens_delta3);

    
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].name = "eir_fc_leaky_bucket_delta_select";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].doc = "Select which delta will be associated with FIFO state";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].size_of_values = sizeof(dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_t);
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].entry_get = dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_entry_str_get;

    
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].nof_keys = 0;

    
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].nof_values = 6;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].values, dnxc_data_value_t, submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].nof_values, "_dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select table values");
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].values[0].name = "fifo_in_between";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].values[0].type = "uint32";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].values[0].doc = "Select delta for the case where FIFO is between almost full and almost empty thresholds";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].values[0].offset = UTILEX_OFFSETOF(dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_t, fifo_in_between);
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].values[1].name = "fifo_partially_almost_full";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].values[1].type = "uint32";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].values[1].doc = "Select delta for the case where FIFO is partially almost full";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].values[1].offset = UTILEX_OFFSETOF(dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_t, fifo_partially_almost_full);
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].values[2].name = "fifo_almost_full";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].values[2].type = "uint32";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].values[2].doc = "Select delta for the case where FIFO is almost full";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].values[2].offset = UTILEX_OFFSETOF(dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_t, fifo_almost_full);
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].values[3].name = "fifo_partially_almost_empty";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].values[3].type = "uint32";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].values[3].doc = "Select delta for the case where FIFO is partially almost empty";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].values[3].offset = UTILEX_OFFSETOF(dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_t, fifo_partially_almost_empty);
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].values[4].name = "fifo_not_stable";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].values[4].type = "uint32";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].values[4].doc = "Select delta for the case where FIFO is not stable";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].values[4].offset = UTILEX_OFFSETOF(dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_t, fifo_not_stable);
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].values[5].name = "fifo_almost_empty";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].values[5].type = "uint32";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].values[5].doc = "Select delta for the case where FIFO is almost empty";
    submodule_data->tables[dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select].values[5].offset = UTILEX_OFFSETOF(dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_t, fifo_almost_empty);

    
    submodule_data->tables[dnx_data_fabric_transmit_table_dtq_fc_local_thresholds].name = "dtq_fc_local_thresholds";
    submodule_data->tables[dnx_data_fabric_transmit_table_dtq_fc_local_thresholds].doc = "DTQ FC local thresholds";
    submodule_data->tables[dnx_data_fabric_transmit_table_dtq_fc_local_thresholds].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_fabric_transmit_table_dtq_fc_local_thresholds].size_of_values = sizeof(dnx_data_fabric_transmit_dtq_fc_local_thresholds_t);
    submodule_data->tables[dnx_data_fabric_transmit_table_dtq_fc_local_thresholds].entry_get = dnx_data_fabric_transmit_dtq_fc_local_thresholds_entry_str_get;

    
    submodule_data->tables[dnx_data_fabric_transmit_table_dtq_fc_local_thresholds].nof_keys = 1;
    submodule_data->tables[dnx_data_fabric_transmit_table_dtq_fc_local_thresholds].keys[0].name = "dtq_fc";
    submodule_data->tables[dnx_data_fabric_transmit_table_dtq_fc_local_thresholds].keys[0].doc = "DTQ FC";

    
    submodule_data->tables[dnx_data_fabric_transmit_table_dtq_fc_local_thresholds].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_fabric_transmit_table_dtq_fc_local_thresholds].values, dnxc_data_value_t, submodule_data->tables[dnx_data_fabric_transmit_table_dtq_fc_local_thresholds].nof_values, "_dnx_data_fabric_transmit_table_dtq_fc_local_thresholds table values");
    submodule_data->tables[dnx_data_fabric_transmit_table_dtq_fc_local_thresholds].values[0].name = "threshold";
    submodule_data->tables[dnx_data_fabric_transmit_table_dtq_fc_local_thresholds].values[0].type = "uint32";
    submodule_data->tables[dnx_data_fabric_transmit_table_dtq_fc_local_thresholds].values[0].doc = "FC threshold";
    submodule_data->tables[dnx_data_fabric_transmit_table_dtq_fc_local_thresholds].values[0].offset = UTILEX_OFFSETOF(dnx_data_fabric_transmit_dtq_fc_local_thresholds_t, threshold);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_fabric_transmit_feature_get(
    int unit,
    dnx_data_fabric_transmit_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_transmit, feature);
}


uint32
dnx_data_fabric_transmit_s2d_pdq_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_transmit, dnx_data_fabric_transmit_define_s2d_pdq_size);
}

uint32
dnx_data_fabric_transmit_pdq_sram_fc_th_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_transmit, dnx_data_fabric_transmit_define_pdq_sram_fc_th);
}

uint32
dnx_data_fabric_transmit_pdq_mixs_fc_th_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_transmit, dnx_data_fabric_transmit_define_pdq_mixs_fc_th);
}

uint32
dnx_data_fabric_transmit_pdq_mixd_fc_th_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_transmit, dnx_data_fabric_transmit_define_pdq_mixd_fc_th);
}

uint32
dnx_data_fabric_transmit_pdq_mix_mixs_fc_th_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_transmit, dnx_data_fabric_transmit_define_pdq_mix_mixs_fc_th);
}

uint32
dnx_data_fabric_transmit_pdq_mix_mixd_fc_th_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_transmit, dnx_data_fabric_transmit_define_pdq_mix_mixd_fc_th);
}

uint32
dnx_data_fabric_transmit_pdq_ocb_fc_th_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_transmit, dnx_data_fabric_transmit_define_pdq_ocb_fc_th);
}

uint32
dnx_data_fabric_transmit_pdq_dram_delete_fc_th_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_transmit, dnx_data_fabric_transmit_define_pdq_dram_delete_fc_th);
}

uint32
dnx_data_fabric_transmit_pdq_sram_delete_fc_th_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_transmit, dnx_data_fabric_transmit_define_pdq_sram_delete_fc_th);
}

uint32
dnx_data_fabric_transmit_pdq_s2d_fc_th_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_transmit, dnx_data_fabric_transmit_define_pdq_s2d_fc_th);
}

uint32
dnx_data_fabric_transmit_pdq_sram_eir_fc_th_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_transmit, dnx_data_fabric_transmit_define_pdq_sram_eir_fc_th);
}

uint32
dnx_data_fabric_transmit_pdq_mixs_eir_fc_th_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_transmit, dnx_data_fabric_transmit_define_pdq_mixs_eir_fc_th);
}

uint32
dnx_data_fabric_transmit_pdq_mixd_eir_fc_th_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_transmit, dnx_data_fabric_transmit_define_pdq_mixd_eir_fc_th);
}

uint32
dnx_data_fabric_transmit_pdq_ocb_eir_fc_th_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_transmit, dnx_data_fabric_transmit_define_pdq_ocb_eir_fc_th);
}



const dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_t *
dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_get(
    int unit,
    int dqcq_priority)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_transmit, dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dqcq_priority, 0);
    return (const dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_t *) data;

}

const dnx_data_fabric_transmit_eir_fc_leaky_bucket_t *
dnx_data_fabric_transmit_eir_fc_leaky_bucket_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_transmit, dnx_data_fabric_transmit_table_eir_fc_leaky_bucket);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_fabric_transmit_eir_fc_leaky_bucket_t *) data;

}

const dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_t *
dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_transmit, dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_t *) data;

}

const dnx_data_fabric_transmit_dtq_fc_local_thresholds_t *
dnx_data_fabric_transmit_dtq_fc_local_thresholds_get(
    int unit,
    int dtq_fc)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_transmit, dnx_data_fabric_transmit_table_dtq_fc_local_thresholds);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dtq_fc, 0);
    return (const dnx_data_fabric_transmit_dtq_fc_local_thresholds_t *) data;

}


shr_error_e
dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_transmit, dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select);
    data = (const dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->sram);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mixs);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mixd);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mix_mixs);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mix_mixd);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->s2d);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_fabric_transmit_eir_fc_leaky_bucket_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_fabric_transmit_eir_fc_leaky_bucket_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_transmit, dnx_data_fabric_transmit_table_eir_fc_leaky_bucket);
    data = (const dnx_data_fabric_transmit_eir_fc_leaky_bucket_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->almost_full_th);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->almost_empty_th);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->bucket_size);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->bucket_fc_threshold);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->period);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tokens_delta0);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tokens_delta1);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tokens_delta2);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tokens_delta3);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_transmit, dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select);
    data = (const dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fifo_in_between);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fifo_partially_almost_full);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fifo_almost_full);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fifo_partially_almost_empty);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fifo_not_stable);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fifo_almost_empty);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_fabric_transmit_dtq_fc_local_thresholds_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_fabric_transmit_dtq_fc_local_thresholds_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_transmit, dnx_data_fabric_transmit_table_dtq_fc_local_thresholds);
    data = (const dnx_data_fabric_transmit_dtq_fc_local_thresholds_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->threshold);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_transmit, dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select);

}

const dnxc_data_table_info_t *
dnx_data_fabric_transmit_eir_fc_leaky_bucket_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_transmit, dnx_data_fabric_transmit_table_eir_fc_leaky_bucket);

}

const dnxc_data_table_info_t *
dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_transmit, dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select);

}

const dnxc_data_table_info_t *
dnx_data_fabric_transmit_dtq_fc_local_thresholds_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_fabric, dnx_data_fabric_submodule_transmit, dnx_data_fabric_transmit_table_dtq_fc_local_thresholds);

}



shr_error_e
dnx_data_fabric_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "fabric";
    module_data->nof_submodules = _dnx_data_fabric_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data fabric submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_fabric_general_init(unit, &module_data->submodules[dnx_data_fabric_submodule_general]));
    SHR_IF_ERR_EXIT(dnx_data_fabric_links_init(unit, &module_data->submodules[dnx_data_fabric_submodule_links]));
    SHR_IF_ERR_EXIT(dnx_data_fabric_blocks_init(unit, &module_data->submodules[dnx_data_fabric_submodule_blocks]));
    SHR_IF_ERR_EXIT(dnx_data_fabric_reachability_init(unit, &module_data->submodules[dnx_data_fabric_submodule_reachability]));
    SHR_IF_ERR_EXIT(dnx_data_fabric_mesh_init(unit, &module_data->submodules[dnx_data_fabric_submodule_mesh]));
    SHR_IF_ERR_EXIT(dnx_data_fabric_cell_init(unit, &module_data->submodules[dnx_data_fabric_submodule_cell]));
    SHR_IF_ERR_EXIT(dnx_data_fabric_pipes_init(unit, &module_data->submodules[dnx_data_fabric_submodule_pipes]));
    SHR_IF_ERR_EXIT(dnx_data_fabric_debug_init(unit, &module_data->submodules[dnx_data_fabric_submodule_debug]));
    SHR_IF_ERR_EXIT(dnx_data_fabric_mesh_topology_init(unit, &module_data->submodules[dnx_data_fabric_submodule_mesh_topology]));
    SHR_IF_ERR_EXIT(dnx_data_fabric_tdm_init(unit, &module_data->submodules[dnx_data_fabric_submodule_tdm]));
    SHR_IF_ERR_EXIT(dnx_data_fabric_cgm_init(unit, &module_data->submodules[dnx_data_fabric_submodule_cgm]));
    SHR_IF_ERR_EXIT(dnx_data_fabric_dtqs_init(unit, &module_data->submodules[dnx_data_fabric_submodule_dtqs]));
    SHR_IF_ERR_EXIT(dnx_data_fabric_ilkn_init(unit, &module_data->submodules[dnx_data_fabric_submodule_ilkn]));
    SHR_IF_ERR_EXIT(dnx_data_fabric_dbal_init(unit, &module_data->submodules[dnx_data_fabric_submodule_dbal]));
    SHR_IF_ERR_EXIT(dnx_data_fabric_mesh_mc_init(unit, &module_data->submodules[dnx_data_fabric_submodule_mesh_mc]));
    SHR_IF_ERR_EXIT(dnx_data_fabric_transmit_init(unit, &module_data->submodules[dnx_data_fabric_submodule_transmit]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_fabric_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_fabric_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_fabric_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_fabric_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_fabric_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_fabric_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_fabric_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_fabric_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_fabric_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_fabric_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_fabric_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_fabric_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_fabric_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_fabric_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_fabric_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

