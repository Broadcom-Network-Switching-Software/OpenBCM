

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
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <bcm/fabric.h>
#include <bcm/port.h>
#include <include/bcm_int/dnx/fabric/fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <shared/utilex/utilex_integer_arithmetic.h>







static shr_error_e
jr2_a0_dnx_data_fabric_general_blocks_exist_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_general;
    int feature_index = dnx_data_fabric_general_blocks_exist;
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
jr2_a0_dnx_data_fabric_general_is_jr1_in_system_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_general;
    int feature_index = dnx_data_fabric_general_is_jr1_in_system_supported;
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
jr2_a0_dnx_data_fabric_general_nof_lcplls_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_general;
    int define_index = dnx_data_fabric_general_define_nof_lcplls;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_general_fmac_clock_khz_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_general;
    int define_index = dnx_data_fabric_general_define_fmac_clock_khz;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 892000;

    
    define->data = 892000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_general_connect_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_general;
    int define_index = dnx_data_fabric_general_define_connect_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DNX_FABRIC_NOF_CONNECT_MODES;

    
    define->data = DNX_FABRIC_NOF_CONNECT_MODES;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FABRIC_CONNECT_MODE;
    define->property.doc = 
        "\n"
        "Set the connect mode of the Fabric:\n"
        "FE - fabric is connected to FE device.\n"
        "SINGLE_FAP - stand-alone device.\n"
        "MESH - fabric is connected to other FAPs directly.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 3;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "FE";
    define->property.mapping[0].val = DNX_FABRIC_CONNECT_MODE_FE;
    define->property.mapping[1].name = "SINGLE_FAP";
    define->property.mapping[1].val = DNX_FABRIC_CONNECT_MODE_SINGLE_FAP;
    define->property.mapping[2].name = "MESH";
    define->property.mapping[2].val = DNX_FABRIC_CONNECT_MODE_MESH;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_fabric_general_fmac_bus_size_key_map(
    int unit,
    int key0_val,
    int key1_val,
    int *key0_index,
    int *key1_index)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (key0_val)
    {
        case bcmPortPhyFecBaseR:
            *key0_index = 0;
            break;
        case bcmPortPhyFecNone:
            *key0_index = 1;
            break;
        case bcmPortPhyFecRs108:
            *key0_index = 2;
            break;
        case bcmPortPhyFecRs304:
            *key0_index = 3;
            break;
        case bcmPortPhyFecRs206:
            *key0_index = 4;
            break;
        case bcmPortPhyFecRs545:
            *key0_index = 5;
            break;
        default:
            *key0_index = -1;
            break;
    }

    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_general_fmac_bus_size_key_reverse_map(
    int unit,
    int key0_index,
    int key1_index,
    int *key0_val,
    int *key1_val)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (key0_index)
    {
        case 0:
            *key0_val = bcmPortPhyFecBaseR;
            break;
        case 1:
            *key0_val = bcmPortPhyFecNone;
            break;
        case 2:
            *key0_val = bcmPortPhyFecRs108;
            break;
        case 3:
            *key0_val = bcmPortPhyFecRs304;
            break;
        case 4:
            *key0_val = bcmPortPhyFecRs206;
            break;
        case 5:
            *key0_val = bcmPortPhyFecRs545;
            break;
        default:
            *key0_val = -1;
            break;
    }

    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_general_fmac_bus_size_set(
    int unit)
{
    int mode_index;
    dnx_data_fabric_general_fmac_bus_size_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_general;
    int table_index = dnx_data_fabric_general_table_fmac_bus_size;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 6;
    table->info_get.key_size[0] = 6;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fabric_general_fmac_bus_size_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_fabric_general_table_fmac_bus_size");

    
    default_data = (dnx_data_fabric_general_fmac_bus_size_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->size = 0;
    
    for (mode_index = 0; mode_index < table->keys[0].size; mode_index++)
    {
        data = (dnx_data_fabric_general_fmac_bus_size_t *) dnxc_data_mgmt_table_data_get(unit, table, mode_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    table->key_map = jr2_a0_dnx_data_fabric_general_fmac_bus_size_key_map;
    table->key_map_reverse = jr2_a0_dnx_data_fabric_general_fmac_bus_size_key_reverse_map;
    
    data = (dnx_data_fabric_general_fmac_bus_size_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmPortPhyFecBaseR, 0);
    data->size = 66;
    data = (dnx_data_fabric_general_fmac_bus_size_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmPortPhyFecNone, 0);
    data->size = 66;
    data = (dnx_data_fabric_general_fmac_bus_size_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmPortPhyFecRs206, 0);
    data->size = 70;
    data = (dnx_data_fabric_general_fmac_bus_size_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmPortPhyFecRs108, 0);
    data->size = 70;
    data = (dnx_data_fabric_general_fmac_bus_size_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmPortPhyFecRs545, 0);
    data->size = 70;
    data = (dnx_data_fabric_general_fmac_bus_size_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmPortPhyFecRs304, 0);
    data->size = 70;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_general_pll_phys_set(
    int unit)
{
    int lcpll_index;
    dnx_data_fabric_general_pll_phys_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_general;
    int table_index = dnx_data_fabric_general_table_pll_phys;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_fabric.general.nof_lcplls_get(unit);
    table->info_get.key_size[0] = dnx_data_fabric.general.nof_lcplls_get(unit);

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fabric_general_pll_phys_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_fabric_general_table_pll_phys");

    
    default_data = (dnx_data_fabric_general_pll_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->min_phy_id = 0;
    default_data->max_phy_id = 0;
    
    for (lcpll_index = 0; lcpll_index < table->keys[0].size; lcpll_index++)
    {
        data = (dnx_data_fabric_general_pll_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, lcpll_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_fabric_general_pll_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->min_phy_id = 0;
        data->max_phy_id = 55;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_fabric_general_pll_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->min_phy_id = 56;
        data->max_phy_id = 111;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_general_pm_properties_set(
    int unit)
{
    int pm_index_index;
    dnx_data_fabric_general_pm_properties_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_general;
    int table_index = dnx_data_fabric_general_table_pm_properties;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_fabric.blocks.nof_pms_get(unit);
    table->info_get.key_size[0] = dnx_data_fabric.blocks.nof_pms_get(unit);

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "2";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fabric_general_pm_properties_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_fabric_general_table_pm_properties");

    
    default_data = (dnx_data_fabric_general_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->special_pll_check = 0;
    default_data->nof_plls = 2;
    
    for (pm_index_index = 0; pm_index_index < table->keys[0].size; pm_index_index++)
    {
        data = (dnx_data_fabric_general_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, pm_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_fabric_general_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->special_pll_check = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_fabric_links_load_balancing_periodic_event_enabled_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_links;
    int feature_index = dnx_data_fabric_links_load_balancing_periodic_event_enabled;
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
jr2_a0_dnx_data_fabric_links_nof_links_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_links;
    int define_index = dnx_data_fabric_links_define_nof_links;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 112;

    
    define->data = 112;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_links_nof_links_per_core_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_links;
    int define_index = dnx_data_fabric_links_define_nof_links_per_core;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_fabric.links.nof_links_get(unit)/dnx_data_device.general.nof_cores_get(unit);

    
    define->data = dnx_data_fabric.links.nof_links_get(unit)/dnx_data_device.general.nof_cores_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_links_max_link_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_links;
    int define_index = dnx_data_fabric_links_define_max_link_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_fabric.links.nof_links_get(unit)-1;

    
    define->data = dnx_data_fabric.links.nof_links_get(unit)-1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_links_max_speed_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_links;
    int define_index = dnx_data_fabric_links_define_max_speed;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 53125;

    
    define->data = 53125;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_links_usec_between_load_balancing_events_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_links;
    int define_index = dnx_data_fabric_links_define_usec_between_load_balancing_events;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1000000;

    
    define->data = 1000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_links_kr_fec_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_links;
    int define_index = dnx_data_fabric_links_define_kr_fec_supported;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_links_core_mapping_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_links;
    int define_index = dnx_data_fabric_links_define_core_mapping_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FABRIC_LINKS_TO_CORE_MAPPING_MODE;
    define->property.doc = 
        "\n"
        "Set Fabric link-to-core mapping mode:\n"
        "SHARED - the links are shared between the cores.\n"
        "Default: SHARED\n"
        "Note: this soc property exist only for backward compatibility.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 1;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "SHARED";
    define->property.mapping[0].val = 0;
    define->property.mapping[0].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_fabric_links_general_set(
    int unit)
{
    dnx_data_fabric_links_general_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_links;
    int table_index = dnx_data_fabric_links_table_general;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "0xffffffff, 0xffffffff, 0xffffffff, 0xffff";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_fabric_links_general_t, (1 + 1  ), "data of dnx_data_fabric_links_table_general");

    
    default_data = (dnx_data_fabric_links_general_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    #if 4 > _SHR_PBMP_WORD_MAX
    #error "out of bound access to array"
    #endif
    _SHR_PBMP_WORD_SET(default_data->supported_links, 0, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->supported_links, 1, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->supported_links, 2, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->supported_links, 3, 0xffff);
    
    data = (dnx_data_fabric_links_general_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_links_polarity_set(
    int unit)
{
    int link_index;
    dnx_data_fabric_links_polarity_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_links;
    int table_index = dnx_data_fabric_links_table_polarity;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->flags |= DNXC_DATA_F_INIT_ONLY;

    
    table->keys[0].size = dnx_data_fabric.links.nof_links_get(unit);
    table->info_get.key_size[0] = dnx_data_fabric.links.nof_links_get(unit);

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fabric_links_polarity_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_fabric_links_table_polarity");

    
    default_data = (dnx_data_fabric_links_polarity_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->tx_polarity = 0;
    default_data->rx_polarity = 0;
    
    for (link_index = 0; link_index < table->keys[0].size; link_index++)
    {
        data = (dnx_data_fabric_links_polarity_t *) dnxc_data_mgmt_table_data_get(unit, table, link_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = "phy_tx_polarity_flip";
    table->values[0].property.doc =
        "\n"
        "Flips PHY TX polarity if enabled\n"
        "phy_tx_polarity_flip_fabric#link# = 0 / 1\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_custom;
    table->values[0].property.method_str = "custom";
    
    table->values[1].property.name = "phy_rx_polarity_flip";
    table->values[1].property.doc =
        "\n"
        "Flips PHY RX polarity if enabled\n"
        "phy_rx_polarity_flip_fabric#link# = 0 / 1\n"
        "\n"
    ;
    table->values[1].property.method = dnxc_data_property_method_custom;
    table->values[1].property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (link_index = 0; link_index < table->keys[0].size; link_index++)
    {
        data = (dnx_data_fabric_links_polarity_t *) dnxc_data_mgmt_table_data_get(unit, table, link_index, 0);
        SHR_IF_ERR_EXIT(dnx_data_property_fabric_links_polarity_tx_polarity_read(unit, link_index, &data->tx_polarity));
    }
    for (link_index = 0; link_index < table->keys[0].size; link_index++)
    {
        data = (dnx_data_fabric_links_polarity_t *) dnxc_data_mgmt_table_data_get(unit, table, link_index, 0);
        SHR_IF_ERR_EXIT(dnx_data_property_fabric_links_polarity_rx_polarity_read(unit, link_index, &data->rx_polarity));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_links_supported_interfaces_set(
    int unit)
{
    int index_index;
    dnx_data_fabric_links_supported_interfaces_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_links;
    int table_index = dnx_data_fabric_links_table_supported_interfaces;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 30;
    table->info_get.key_size[0] = 30;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "bcmPortPhyFecInvalid";
    table->values[2].default_val = "1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fabric_links_supported_interfaces_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_fabric_links_table_supported_interfaces");

    
    default_data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->speed = -1;
    default_data->fec_type = bcmPortPhyFecInvalid;
    default_data->is_valid = 1;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->speed = 10312;
        data->fec_type = bcmPortPhyFecNone;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->speed = 10312;
        data->fec_type = bcmPortPhyFecBaseR;
        data->is_valid = dnx_data_fabric.links.kr_fec_supported_get(unit);
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->speed = 10312;
        data->fec_type = bcmPortPhyFecRs206;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->speed = 10312;
        data->fec_type = bcmPortPhyFecRs108;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->speed = 11500;
        data->fec_type = bcmPortPhyFecNone;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->speed = 11500;
        data->fec_type = bcmPortPhyFecBaseR;
        data->is_valid = dnx_data_fabric.links.kr_fec_supported_get(unit);
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->speed = 11500;
        data->fec_type = bcmPortPhyFecRs206;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->speed = 11500;
        data->fec_type = bcmPortPhyFecRs108;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->speed = 20625;
        data->fec_type = bcmPortPhyFecNone;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->speed = 20625;
        data->fec_type = bcmPortPhyFecRs206;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->speed = 20625;
        data->fec_type = bcmPortPhyFecRs108;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->speed = 23000;
        data->fec_type = bcmPortPhyFecNone;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->speed = 23000;
        data->fec_type = bcmPortPhyFecRs206;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->speed = 23000;
        data->fec_type = bcmPortPhyFecRs108;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->speed = 25000;
        data->fec_type = bcmPortPhyFecNone;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->speed = 25000;
        data->fec_type = bcmPortPhyFecRs206;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->speed = 25000;
        data->fec_type = bcmPortPhyFecRs108;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->speed = 25781;
        data->fec_type = bcmPortPhyFecNone;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->speed = 25781;
        data->fec_type = bcmPortPhyFecRs206;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->speed = 25781;
        data->fec_type = bcmPortPhyFecRs108;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->speed = 50000;
        data->fec_type = bcmPortPhyFecNone;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->speed = 50000;
        data->fec_type = bcmPortPhyFecRs206;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->speed = 50000;
        data->fec_type = bcmPortPhyFecRs108;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->speed = 50000;
        data->fec_type = bcmPortPhyFecRs545;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->speed = 50000;
        data->fec_type = bcmPortPhyFecRs304;
    }
    if (25 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, 0);
        data->speed = 53125;
        data->fec_type = bcmPortPhyFecNone;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->speed = 53125;
        data->fec_type = bcmPortPhyFecRs206;
    }
    if (27 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, 0);
        data->speed = 53125;
        data->fec_type = bcmPortPhyFecRs108;
    }
    if (28 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 28, 0);
        data->speed = 53125;
        data->fec_type = bcmPortPhyFecRs545;
    }
    if (29 < table->keys[0].size)
    {
        data = (dnx_data_fabric_links_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 29, 0);
        data->speed = 53125;
        data->fec_type = bcmPortPhyFecRs304;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_fabric_blocks_nof_instances_fmac_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_blocks;
    int define_index = dnx_data_fabric_blocks_define_nof_instances_fmac;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_fabric.links.nof_links_get(unit)/dnx_data_fabric.blocks.nof_links_in_fmac_get(unit));

    
    define->data = (dnx_data_fabric.links.nof_links_get(unit)/dnx_data_fabric.blocks.nof_links_in_fmac_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_blocks_nof_links_in_fmac_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_blocks;
    int define_index = dnx_data_fabric_blocks_define_nof_links_in_fmac;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_blocks_nof_instances_fsrd_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_blocks;
    int define_index = dnx_data_fabric_blocks_define_nof_instances_fsrd;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_fabric.links.nof_links_get(unit)/dnx_data_fabric.blocks.nof_links_in_fsrd_get(unit));

    
    define->data = (dnx_data_fabric.links.nof_links_get(unit)/dnx_data_fabric.blocks.nof_links_in_fsrd_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_blocks_nof_links_in_fsrd_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_blocks;
    int define_index = dnx_data_fabric_blocks_define_nof_links_in_fsrd;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_blocks_nof_fmacs_in_fsrd_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_blocks;
    int define_index = dnx_data_fabric_blocks_define_nof_fmacs_in_fsrd;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_blocks_nof_pms_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_blocks;
    int define_index = dnx_data_fabric_blocks_define_nof_pms;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_fabric.blocks.nof_instances_fsrd_get(unit);

    
    define->data = dnx_data_fabric.blocks.nof_instances_fsrd_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_blocks_nof_links_in_pm_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_blocks;
    int define_index = dnx_data_fabric_blocks_define_nof_links_in_pm;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_fabric.blocks.nof_links_in_fsrd_get(unit);

    
    define->data = dnx_data_fabric.blocks.nof_links_in_fsrd_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_blocks_nof_fmacs_in_pm_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_blocks;
    int define_index = dnx_data_fabric_blocks_define_nof_fmacs_in_pm;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_fabric.blocks.nof_fmacs_in_fsrd_get(unit);

    
    define->data = dnx_data_fabric.blocks.nof_fmacs_in_fsrd_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_blocks_nof_instances_fdtl_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_blocks;
    int define_index = dnx_data_fabric_blocks_define_nof_instances_fdtl;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_blocks_nof_links_in_fdtl_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_blocks;
    int define_index = dnx_data_fabric_blocks_define_nof_links_in_fdtl;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_fabric.links.nof_links_get(unit)/dnx_data_fabric.blocks.nof_instances_fdtl_get(unit));

    
    define->data = (dnx_data_fabric.links.nof_links_get(unit)/dnx_data_fabric.blocks.nof_instances_fdtl_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_blocks_nof_sbus_chains_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_blocks;
    int define_index = dnx_data_fabric_blocks_define_nof_sbus_chains;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_fabric_blocks_fsrd_sbus_chain_set(
    int unit)
{
    int fsrd_id_index;
    dnx_data_fabric_blocks_fsrd_sbus_chain_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_blocks;
    int table_index = dnx_data_fabric_blocks_table_fsrd_sbus_chain;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_fabric.blocks.nof_instances_fsrd_get(unit);
    table->info_get.key_size[0] = dnx_data_fabric.blocks.nof_instances_fsrd_get(unit);

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fabric_blocks_fsrd_sbus_chain_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_fabric_blocks_table_fsrd_sbus_chain");

    
    default_data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->chain = -1;
    default_data->index_in_chain = -1;
    
    for (fsrd_id_index = 0; fsrd_id_index < table->keys[0].size; fsrd_id_index++)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, fsrd_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->chain = 0;
        data->index_in_chain = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->chain = 0;
        data->index_in_chain = 1;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->chain = 0;
        data->index_in_chain = 2;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->chain = 0;
        data->index_in_chain = 3;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->chain = 0;
        data->index_in_chain = 4;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->chain = 0;
        data->index_in_chain = 5;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->chain = 0;
        data->index_in_chain = 6;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->chain = 0;
        data->index_in_chain = 13;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->chain = 0;
        data->index_in_chain = 12;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->chain = 0;
        data->index_in_chain = 11;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->chain = 0;
        data->index_in_chain = 10;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->chain = 0;
        data->index_in_chain = 9;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->chain = 0;
        data->index_in_chain = 8;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->chain = 0;
        data->index_in_chain = 7;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_blocks_fmac_sbus_chain_set(
    int unit)
{
    int fmac_id_index;
    dnx_data_fabric_blocks_fmac_sbus_chain_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_blocks;
    int table_index = dnx_data_fabric_blocks_table_fmac_sbus_chain;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_fabric.blocks.nof_instances_fmac_get(unit);
    table->info_get.key_size[0] = dnx_data_fabric.blocks.nof_instances_fmac_get(unit);

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fabric_blocks_fmac_sbus_chain_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_fabric_blocks_table_fmac_sbus_chain");

    
    default_data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->chain = -1;
    default_data->index_in_chain = -1;
    
    for (fmac_id_index = 0; fmac_id_index < table->keys[0].size; fmac_id_index++)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, fmac_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->chain = 0;
        data->index_in_chain = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->chain = 0;
        data->index_in_chain = 1;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->chain = 0;
        data->index_in_chain = 2;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->chain = 0;
        data->index_in_chain = 3;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->chain = 0;
        data->index_in_chain = 4;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->chain = 0;
        data->index_in_chain = 5;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->chain = 0;
        data->index_in_chain = 6;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->chain = 0;
        data->index_in_chain = 7;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->chain = 0;
        data->index_in_chain = 8;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->chain = 0;
        data->index_in_chain = 9;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->chain = 0;
        data->index_in_chain = 10;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->chain = 0;
        data->index_in_chain = 11;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->chain = 0;
        data->index_in_chain = 12;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->chain = 0;
        data->index_in_chain = 13;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->chain = 0;
        data->index_in_chain = 26;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->chain = 0;
        data->index_in_chain = 27;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->chain = 0;
        data->index_in_chain = 24;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->chain = 0;
        data->index_in_chain = 25;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->chain = 0;
        data->index_in_chain = 22;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->chain = 0;
        data->index_in_chain = 23;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->chain = 0;
        data->index_in_chain = 20;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->chain = 0;
        data->index_in_chain = 21;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->chain = 0;
        data->index_in_chain = 18;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->chain = 0;
        data->index_in_chain = 19;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->chain = 0;
        data->index_in_chain = 16;
    }
    if (25 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, 0);
        data->chain = 0;
        data->index_in_chain = 17;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->chain = 0;
        data->index_in_chain = 14;
    }
    if (27 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, 0);
        data->chain = 0;
        data->index_in_chain = 15;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_blocks_fsrd_ref_clk_master_set(
    int unit)
{
    int fsrd_index_index;
    dnx_data_fabric_blocks_fsrd_ref_clk_master_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_blocks;
    int table_index = dnx_data_fabric_blocks_table_fsrd_ref_clk_master;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_fabric.blocks.nof_instances_fsrd_get(unit);
    table->info_get.key_size[0] = dnx_data_fabric.blocks.nof_instances_fsrd_get(unit);

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fabric_blocks_fsrd_ref_clk_master_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_fabric_blocks_table_fsrd_ref_clk_master");

    
    default_data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->master_fsrd_index = -1;
    
    for (fsrd_index_index = 0; fsrd_index_index < table->keys[0].size; fsrd_index_index++)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, fsrd_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->master_fsrd_index = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->master_fsrd_index = 1;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->master_fsrd_index = 2;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->master_fsrd_index = 3;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->master_fsrd_index = 4;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->master_fsrd_index = 5;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->master_fsrd_index = 6;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->master_fsrd_index = 7;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->master_fsrd_index = 8;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->master_fsrd_index = 9;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->master_fsrd_index = 10;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->master_fsrd_index = 11;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->master_fsrd_index = 12;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->master_fsrd_index = 13;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_fabric_reachability_resolution_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_reachability;
    int define_index = dnx_data_fabric_reachability_define_resolution;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_reachability_gen_period_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_reachability;
    int define_index = dnx_data_fabric_reachability_define_gen_period;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6;

    
    define->data = 6;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_reachability_full_cycle_period_usec_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_reachability;
    int define_index = dnx_data_fabric_reachability_define_full_cycle_period_usec;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 50;

    
    define->data = 50;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_reachability_watchdog_resolution_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_reachability;
    int define_index = dnx_data_fabric_reachability_define_watchdog_resolution;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4096;

    
    define->data = 4096;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_reachability_watchdog_period_usec_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_reachability;
    int define_index = dnx_data_fabric_reachability_define_watchdog_period_usec;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 100;

    
    define->data = 100;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_fabric_mesh_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_mesh;
    int feature_index = dnx_data_fabric_mesh_is_supported;
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
jr2_a0_dnx_data_fabric_mesh_max_mc_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_mesh;
    int define_index = dnx_data_fabric_mesh_define_max_mc_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (128*1024)-1;

    
    define->data = (128*1024)-1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_mesh_max_nof_dest_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_mesh;
    int define_index = dnx_data_fabric_mesh_define_max_nof_dest;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_mesh_multicast_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_mesh;
    int define_index = dnx_data_fabric_mesh_define_multicast_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "fabric_mesh_multicast_enable";
    define->property.doc = 
        "\n"
        "Enable / Disable MESH MC mode:\n"
        "fabric_mesh_multicast_enable = 1 / 0\n"
        "Relevant only when fabric connect mode is MESH.\n"
        "Default: 1\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_custom;
    define->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnx_data_property_fabric_mesh_multicast_enable_read(unit, (uint32 *) &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_mesh_nof_dest_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_mesh;
    int define_index = dnx_data_fabric_mesh_define_nof_dest;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_fabric.mesh.max_nof_dest_get(unit)-dnx_data_fabric.mesh.multicast_enable_get(unit);

    
    define->data = dnx_data_fabric.mesh.max_nof_dest_get(unit)-dnx_data_fabric.mesh.multicast_enable_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_fabric_cell_nof_priorities_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cell;
    int define_index = dnx_data_fabric_cell_define_nof_priorities;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_cell_vsc256_max_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cell;
    int define_index = dnx_data_fabric_cell_define_vsc256_max_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 256+16;

    
    define->data = 256+16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_cell_sr_cell_header_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cell;
    int define_index = dnx_data_fabric_cell_define_sr_cell_header_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_cell_sr_cell_nof_instances_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cell;
    int define_index = dnx_data_fabric_cell_define_sr_cell_nof_instances;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_cell_default_packing_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cell;
    int define_index = dnx_data_fabric_cell_define_default_packing_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DBAL_ENUM_FVAL_FABRIC_PCP_MODE_CONTINUOUS;

    
    define->data = DBAL_ENUM_FVAL_FABRIC_PCP_MODE_CONTINUOUS;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_cell_pcp_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cell;
    int define_index = dnx_data_fabric_cell_define_pcp_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FABRIC_PCP_ENABLE;
    define->property.doc = 
        "\n"
        "Enable PCP:\n"
        "fabric_pcp_enable=1\n"
        "Default: 1\n"
        "Note: this soc property exist only for backward compatibility.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 1;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "1";
    define->property.mapping[0].val = 1;
    define->property.mapping[0].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_fabric_pipes_max_nof_pipes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_pipes;
    int define_index = dnx_data_fabric_pipes_define_max_nof_pipes;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_pipes_max_nof_contexts_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_pipes;
    int define_index = dnx_data_fabric_pipes_define_max_nof_contexts;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_pipes_max_nof_subcontexts_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_pipes;
    int define_index = dnx_data_fabric_pipes_define_max_nof_subcontexts;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_pipes_nof_pipes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_pipes;
    int define_index = dnx_data_fabric_pipes_define_nof_pipes;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FABRIC_NUM_PIPES;
    define->property.doc = 
        "\n"
        "Configure number of fabric pipes:\n"
        "fabric_num_pipes= 1/2/3\n"
        "Default: 1\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 1;
    define->property.range_max = dnx_data_fabric.pipes.max_nof_pipes_get(unit);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_pipes_multi_pipe_system_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_pipes;
    int define_index = dnx_data_fabric_pipes_define_multi_pipe_system;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_SYSTEM_CONTAINS_MULTIPLE_PIPE_DEVICE;
    define->property.doc = 
        "\n"
        "To determine if there is a multiple pipe device in the system:\n"
        "system_contains_multiple_pipe_device = 0/1\n"
        "Default: 0\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_enable;
    define->property.method_str = "enable";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_fabric_pipes_map_set(
    int unit)
{
    dnx_data_fabric_pipes_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_pipes;
    int table_index = dnx_data_fabric_pipes_table_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "0, 0, 0, 0";
    table->values[1].default_val = "0, 0, 0, 0";
    table->values[2].default_val = "SINGLE PIPE";
    table->values[3].default_val = "soc_dnxc_fabric_pipe_map_single";
    table->values[4].default_val = "-1";
    table->values[5].default_val = "-1";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_fabric_pipes_map_t, (1 + 1  ), "data of dnx_data_fabric_pipes_table_map");

    
    default_data = (dnx_data_fabric_pipes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
    default_data->uc[0] = 0;
    default_data->uc[1] = 0;
    default_data->uc[2] = 0;
    default_data->uc[3] = 0;
    SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
    default_data->mc[0] = 0;
    default_data->mc[1] = 0;
    default_data->mc[2] = 0;
    default_data->mc[3] = 0;
    default_data->name = "SINGLE PIPE";
    default_data->type = soc_dnxc_fabric_pipe_map_single;
    default_data->min_hp_mc = -1;
    default_data->tdm_pipe = -1;
    
    data = (dnx_data_fabric_pipes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->property.name = "fabric_pipe_map";
    table->property.doc =
        "\n"
        "Assign traffic cast (unicast/multicast) and priority (0/1/2/3) to a pipe.\n"
        "fabric_pipe_map=0/1/2\n"
        "Assigning a specific traffic cast is done by a suffix uc/mc and a number which represents the cell priority.\n"
        "Not all the options supported, please find the supported options in UM\n"
        "\n"
    ;
    table->property.method = dnxc_data_property_method_custom;
    table->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnx_data_fabric_pipes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnx_data_property_fabric_pipes_map_read(unit, data));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_pipes_valid_map_config_set(
    int unit)
{
    int config_id_index;
    dnx_data_fabric_pipes_valid_map_config_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_pipes;
    int table_index = dnx_data_fabric_pipes_table_valid_map_config;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 5;
    table->info_get.key_size[0] = 5;

    
    table->values[0].default_val = "1";
    table->values[1].default_val = "0, 0, 0, 0";
    table->values[2].default_val = "0, 0, 0, 0";
    table->values[3].default_val = "SINGLE PIPE";
    table->values[4].default_val = "soc_dnxc_fabric_pipe_map_single";
    table->values[5].default_val = "-1";
    table->values[6].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fabric_pipes_valid_map_config_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_fabric_pipes_table_valid_map_config");

    
    default_data = (dnx_data_fabric_pipes_valid_map_config_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->nof_pipes = 1;
    SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
    default_data->uc[0] = 0;
    default_data->uc[1] = 0;
    default_data->uc[2] = 0;
    default_data->uc[3] = 0;
    SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
    default_data->mc[0] = 0;
    default_data->mc[1] = 0;
    default_data->mc[2] = 0;
    default_data->mc[3] = 0;
    default_data->name = "SINGLE PIPE";
    default_data->type = soc_dnxc_fabric_pipe_map_single;
    default_data->min_hp_mc = -1;
    default_data->tdm_pipe = -1;
    
    for (config_id_index = 0; config_id_index < table->keys[0].size; config_id_index++)
    {
        data = (dnx_data_fabric_pipes_valid_map_config_t *) dnxc_data_mgmt_table_data_get(unit, table, config_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_fabric_pipes_valid_map_config_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->nof_pipes = 1;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->uc[0] = 0;
        data->uc[1] = 0;
        data->uc[2] = 0;
        data->uc[3] = 0;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->mc[0] = 0;
        data->mc[1] = 0;
        data->mc[2] = 0;
        data->mc[3] = 0;
        data->name = "SINGLE PIPE";
        data->type = soc_dnxc_fabric_pipe_map_single;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_fabric_pipes_valid_map_config_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->nof_pipes = 2;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->uc[0] = 0;
        data->uc[1] = 0;
        data->uc[2] = 0;
        data->uc[3] = 0;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->mc[0] = 1;
        data->mc[1] = 1;
        data->mc[2] = 1;
        data->mc[3] = 1;
        data->name = "UC,MC";
        data->type = soc_dnxc_fabric_pipe_map_dual_uc_mc;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_fabric_pipes_valid_map_config_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->nof_pipes = 3;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->uc[0] = 0;
        data->uc[1] = 0;
        data->uc[2] = 0;
        data->uc[3] = 0;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->mc[0] = 2;
        data->mc[1] = 2;
        data->mc[2] = 2;
        data->mc[3] = 1;
        data->name = "UC,HP MC (3),LP MC";
        data->type = soc_dnxc_fabric_pipe_map_triple_uc_hp_mc_lp_mc;
        data->min_hp_mc = 3;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_fabric_pipes_valid_map_config_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->nof_pipes = 3;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->uc[0] = 0;
        data->uc[1] = 0;
        data->uc[2] = 0;
        data->uc[3] = 0;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->mc[0] = 2;
        data->mc[1] = 2;
        data->mc[2] = 1;
        data->mc[3] = 1;
        data->name = "UC,HP MC (2,3),LP MC";
        data->type = soc_dnxc_fabric_pipe_map_triple_uc_hp_mc_lp_mc;
        data->min_hp_mc = 2;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_fabric_pipes_valid_map_config_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->nof_pipes = 3;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->uc[0] = 0;
        data->uc[1] = 0;
        data->uc[2] = 0;
        data->uc[3] = 0;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->mc[0] = 2;
        data->mc[1] = 1;
        data->mc[2] = 1;
        data->mc[3] = 1;
        data->name = "UC,HP MC (1,2,3),LP MC";
        data->type = soc_dnxc_fabric_pipe_map_triple_uc_hp_mc_lp_mc;
        data->min_hp_mc = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_fabric_debug_mesh_topology_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_debug;
    int define_index = dnx_data_fabric_debug_define_mesh_topology_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = -1;

    
    define->data = -1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
        "To determine if there is a multiple pipe device in the system:\n"
        "custom_feature_mesh_topology_size=-1/0/1/2/3\n"
        "Default: -1\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range_signed;
    define->property.method_str = "suffix_range_signed";
    define->property.suffix = "mesh_topology_size";
    define->property.range_min = -1;
    define->property.range_max = 3;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_debug_mesh_topology_fast_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_debug;
    int define_index = dnx_data_fabric_debug_define_mesh_topology_fast;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
        "Device is in mesh topology fast mode.\n"
        "Used for internal validation only!\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "mesh_topology_fast";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_fabric_mesh_topology_calendar_mode_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_mesh_topology;
    int feature_index = dnx_data_fabric_mesh_topology_calendar_mode_supported;
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
jr2_a0_dnx_data_fabric_mesh_topology_mesh_init_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_mesh_topology;
    int define_index = dnx_data_fabric_mesh_topology_define_mesh_init;
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
jr2_a0_dnx_data_fabric_mesh_topology_mesh_config_1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_mesh_topology;
    int define_index = dnx_data_fabric_mesh_topology_define_mesh_config_1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_fabric_tdm_priority_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_tdm;
    int define_index = dnx_data_fabric_tdm_define_priority;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = -1;

    
    define->data = -1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_fabric_cgm_nof_rci_levels_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cgm;
    int define_index = dnx_data_fabric_cgm_define_nof_rci_levels;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_cgm_nof_rci_threshold_levels_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cgm;
    int define_index = dnx_data_fabric_cgm_define_nof_rci_threshold_levels;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_cgm_egress_fifo_fc_threshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cgm;
    int define_index = dnx_data_fabric_cgm_define_egress_fifo_fc_threshold;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_fabric_cgm_access_map_set(
    int unit)
{
    int control_type_index;
    dnx_data_fabric_cgm_access_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cgm;
    int table_index = dnx_data_fabric_cgm_table_access_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = bcmFabricCgmNof;
    table->info_get.key_size[0] = bcmFabricCgmNof;

    
    table->values[0].default_val = "DBAL_NOF_TABLES";
    table->values[1].default_val = "DBAL_FIELD_EMPTY";
    table->values[2].default_val = "DBAL_FIELD_EMPTY";
    table->values[3].default_val = "DBAL_FIELD_EMPTY";
    table->values[4].default_val = "DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE";
    table->values[5].default_val = "0";
    table->values[6].default_val = "Unknown control_type";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fabric_cgm_access_map_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_fabric_cgm_table_access_map");

    
    default_data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = DBAL_NOF_TABLES;
    default_data->dbal_key1 = DBAL_FIELD_EMPTY;
    default_data->dbal_key2 = DBAL_FIELD_EMPTY;
    default_data->dbal_result = DBAL_FIELD_EMPTY;
    default_data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
    default_data->is_valid = 0;
    default_data->err_msg = "Unknown control_type";
    
    for (control_type_index = 0; control_type_index < table->keys[0].size; control_type_index++)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, control_type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (bcmFabricCgmRciLinkPipeLevelTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmRciLinkPipeLevelTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_RCI_THRESHOLDS;
        data->dbal_key1 = DBAL_FIELD_PIPE;
        data->dbal_key2 = DBAL_FIELD_LEVEL;
        data->dbal_result = DBAL_FIELD_LINK_RCI_THRESHOLD;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_PIPE_LEVEL;
        data->is_valid = 1;
    }
    if (bcmFabricCgmRciEgressPipeLevelTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmRciEgressPipeLevelTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_RCI_THRESHOLDS;
        data->dbal_key1 = DBAL_FIELD_PIPE;
        data->dbal_key2 = DBAL_FIELD_LEVEL;
        data->dbal_result = DBAL_FIELD_EGRESS_RCI_THRESHOLD;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_PIPE_LEVEL;
        data->is_valid = 1;
    }
    if (bcmFabricCgmRciTotalEgressPipeLevelTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmRciTotalEgressPipeLevelTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_RCI_THRESHOLDS;
        data->dbal_key1 = DBAL_FIELD_PIPE;
        data->dbal_key2 = DBAL_FIELD_LEVEL;
        data->dbal_result = DBAL_FIELD_TOTAL_EGRESS_RCI_THRESHOLD;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_PIPE_LEVEL;
        data->is_valid = 1;
    }
    if (bcmFabricCgmRciLocalLevelTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmRciLocalLevelTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_RCI_LOCAL_THRESHOLDS;
        data->dbal_key1 = DBAL_FIELD_LEVEL;
        data->dbal_result = DBAL_FIELD_LOCAL_RCI_THRESHOLD;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_LEVEL;
        data->is_valid = 1;
    }
    if (bcmFabricCgmRciTotalLocalLevelTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmRciTotalLocalLevelTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_RCI_LOCAL_THRESHOLDS;
        data->dbal_key1 = DBAL_FIELD_LEVEL;
        data->dbal_result = DBAL_FIELD_TOTAL_LOCAL_RCI_THRESHOLD;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_LEVEL;
        data->is_valid = 1;
    }
    if (bcmFabricCgmRciLinkLevelFactor < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmRciLinkLevelFactor, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_RCI_SCORE_MAPPING;
        data->dbal_key1 = DBAL_FIELD_LEVEL;
        data->dbal_result = DBAL_FIELD_LINK_RCI_FACTOR;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_LEVEL;
        data->is_valid = 1;
    }
    if (bcmFabricCgmRciEgressLevelFactor < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmRciEgressLevelFactor, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_RCI_SCORE_MAPPING;
        data->dbal_key1 = DBAL_FIELD_LEVEL;
        data->dbal_result = DBAL_FIELD_EGRESS_RCI_SCORE;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_LEVEL;
        data->is_valid = 1;
    }
    if (bcmFabricCgmRciHighSeverityMinLinksParam < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmRciHighSeverityMinLinksParam, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_RCI_HIGH_SEVERITY;
        data->dbal_result = DBAL_FIELD_MIN_NOF_LINKS;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
        data->is_valid = 1;
    }
    if (bcmFabricCgmRciHighSeverityFactor < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmRciHighSeverityFactor, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_RCI_HIGH_SEVERITY;
        data->dbal_result = DBAL_FIELD_SCORE;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
        data->is_valid = 1;
    }
    if (bcmFabricCgmRciCoreLevelMappingTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmRciCoreLevelMappingTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_RCI_SCORE_TO_LEVEL_THRESHOLDS;
        data->dbal_key1 = DBAL_FIELD_LEVEL;
        data->dbal_result = DBAL_FIELD_CORE_THRESHOLD;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_LEVEL;
        data->is_valid = 1;
    }
    if (bcmFabricCgmRciDeviceLevelMappingTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmRciDeviceLevelMappingTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_RCI_SCORE_TO_LEVEL_THRESHOLDS;
        data->dbal_key1 = DBAL_FIELD_LEVEL;
        data->dbal_result = DBAL_FIELD_DEVICE_THRESHOLD;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_LEVEL;
        data->is_valid = 1;
    }
    if (bcmFabricCgmGciLeakyBucketEn < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmGciLeakyBucketEn, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_GCI_LEAKY_BUCKET_FC_ENABLE;
        data->dbal_result = DBAL_FIELD_LEAKY_BUCKET_ENABLE;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
        data->is_valid = 1;
    }
    if (bcmFabricCgmGciLeakyBucketSizeLeakyBucketTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmGciLeakyBucketSizeLeakyBucketTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_GCI_LEAKY_BUCKET_CONFIG;
        data->dbal_key1 = DBAL_FIELD_LEAKY_BUCKET_ID;
        data->dbal_result = DBAL_FIELD_MAX_VALUE;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_LEAKY_BUCKET;
        data->is_valid = 1;
    }
    if (bcmFabricCgmGciLeakyBucketFCLeakyBucketTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmGciLeakyBucketFCLeakyBucketTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_GCI_LEAKY_BUCKET_CONFIG;
        data->dbal_key1 = DBAL_FIELD_LEAKY_BUCKET_ID;
        data->dbal_result = DBAL_FIELD_THRESHOLD;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_LEAKY_BUCKET;
        data->is_valid = 1;
    }
    if (bcmFabricCgmGciBackoffEn < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmGciBackoffEn, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_GCI_BACKOFF_FC_ENABLE;
        data->dbal_result = DBAL_FIELD_BACKOFF_ENABLE;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
        data->is_valid = 1;
    }
    if (bcmFabricCgmGciBackoffLevelTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmGciBackoffLevelTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_GCI_BACKOFF_CONGESTION_SEVERITY_THRESHOLDS;
        data->dbal_key1 = DBAL_FIELD_LEVEL_SEPARATE;
        data->dbal_result = DBAL_FIELD_THRESHOLD;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_LEVEL_SEPARATE;
        data->is_valid = 1;
    }
    if (bcmFabricCgmSlowStartEn < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmSlowStartEn, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_SLOW_START_ENABLE;
        data->dbal_key1 = DBAL_FIELD_FMC_SHAPER_ID;
        data->dbal_result = DBAL_FIELD_ENABLE;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_FMC_SHAPER;
        data->is_valid = 1;
    }
    if (bcmFabricCgmSlowStartRateShaperPhaseParam < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmSlowStartRateShaperPhaseParam, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_SLOW_START_CONFIG;
        data->dbal_key1 = DBAL_FIELD_FMC_SHAPER_ID;
        data->dbal_key2 = DBAL_FIELD_PHASE;
        data->dbal_result = DBAL_FIELD_EMPTY;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_FMC_SLOW_START_PHASE;
        data->is_valid = 1;
    }
    if (bcmFabricCgmLlfcPipeTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmLlfcPipeTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_LLFC_THRESHOLDS;
        data->dbal_key1 = DBAL_FIELD_PIPE_ID;
        data->dbal_result = DBAL_FIELD_THRESHOLD;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_PIPE;
        data->is_valid = 1;
    }
    if (bcmFabricCgmDropFabricClassTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmDropFabricClassTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_EGRESS_FIFO_DROP_THRESHOLDS;
        data->dbal_key1 = DBAL_FIELD_CAST;
        data->dbal_key2 = DBAL_FIELD_FABRIC_PRIORITY;
        data->dbal_result = DBAL_FIELD_THRESHOLD;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_CAST_PRIORITY;
        data->is_valid = 1;
    }
    if (bcmFabricCgmDropMeshMcPriorityTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmDropMeshMcPriorityTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_MESH_MC_FIFO_DROP_THRESHOLDS;
        data->dbal_key1 = DBAL_FIELD_FABRIC_PRIORITY;
        data->dbal_result = DBAL_FIELD_THRESHOLD;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_PRIORITY;
        data->is_valid = 1;
    }
    if (bcmFabricCgmFcIngressTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmFcIngressTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_FC_INGRESS_THRESHOLDS;
        data->dbal_result = DBAL_FIELD_LINK_FIFO_SIZE_THRESHOLD;
        data->is_valid = 1;
    }
    if (bcmFabricCgmEgressDynamicWfqTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmEgressDynamicWfqTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_EGRESS_DYNAMIC_WFQ;
        data->dbal_result = DBAL_FIELD_THRESHOLD;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
        data->is_valid = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_fabric_dtqs_max_nof_subdtqs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dtqs;
    int define_index = dnx_data_fabric_dtqs_define_max_nof_subdtqs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6;

    
    define->data = 6;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_dtqs_max_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dtqs;
    int define_index = dnx_data_fabric_dtqs_define_max_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2048;

    
    define->data = 2048;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_dtqs_ocb_only_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dtqs;
    int define_index = dnx_data_fabric_dtqs_define_ocb_only_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 128;

    
    define->data = 128;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_dtqs_nof_active_dtqs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dtqs;
    int define_index = dnx_data_fabric_dtqs_define_nof_active_dtqs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "single_dtq";
    define->property.doc = 
        "\n"
        "Configure DTQ mode:\n"
        "single_dtq = 1/0\n"
        "Default: Number of DTQs will be according to pipes configuration\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_custom;
    define->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnx_data_property_fabric_dtqs_nof_active_dtqs_read(unit, (uint32 *) &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}








static shr_error_e
jr2_a0_dnx_data_fabric_ilkn_bypass_info_set(
    int unit)
{
    int pm_id_index;
    dnx_data_fabric_ilkn_bypass_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_ilkn;
    int table_index = dnx_data_fabric_ilkn_table_bypass_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_fabric.blocks.nof_instances_fsrd_get(unit);
    table->info_get.key_size[0] = dnx_data_fabric.blocks.nof_instances_fsrd_get(unit);

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fabric_ilkn_bypass_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_fabric_ilkn_table_bypass_info");

    
    default_data = (dnx_data_fabric_ilkn_bypass_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->links = 0;
    
    for (pm_id_index = 0; pm_id_index < table->keys[0].size; pm_id_index++)
    {
        data = (dnx_data_fabric_ilkn_bypass_info_t *) dnxc_data_mgmt_table_data_get(unit, table, pm_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = "ilkn_use_fabric_links";
    table->values[0].property.doc =
        "\n"
        "bitmap of fabrics link in bypass mode per PM (0x00 - 0xff)\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_custom;
    table->values[0].property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (pm_id_index = 0; pm_id_index < table->keys[0].size; pm_id_index++)
    {
        data = (dnx_data_fabric_ilkn_bypass_info_t *) dnxc_data_mgmt_table_data_get(unit, table, pm_id_index, 0);
        SHR_IF_ERR_EXIT(dnx_data_property_fabric_ilkn_bypass_info_links_read(unit, pm_id_index, &data->links));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_fabric_dbal_link_rci_threshold_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_link_rci_threshold_bits_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10;

    
    define->data = 10;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_dbal_cgm_llfc_threshold_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_cgm_llfc_threshold_bits_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10;

    
    define->data = 10;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_dbal_cgm_llfc_threshold_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_cgm_llfc_threshold_max_value;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 512;

    
    define->data = 512;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_dbal_egress_fifo_drop_threshold_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_egress_fifo_drop_threshold_bits_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 12;

    
    define->data = 12;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_dbal_egress_fifo_drop_threshold_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_egress_fifo_drop_threshold_max_value;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2048;

    
    define->data = 2048;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_dbal_cgm_link_fifo_base_address_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_cgm_link_fifo_base_address_bits_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10;

    
    define->data = 10;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_dbal_cgm_link_fifo_base_address_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_cgm_link_fifo_base_address_max_value;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 512;

    
    define->data = 512;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_dbal_cgm_egress_fifo_base_address_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_cgm_egress_fifo_base_address_bits_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 12;

    
    define->data = 12;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_dbal_cgm_egress_fifo_base_address_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_cgm_egress_fifo_base_address_max_value;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2048;

    
    define->data = 2048;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_dbal_cgm_egress_fifo_fc_threshold_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_cgm_egress_fifo_fc_threshold_bits_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 12;

    
    define->data = 12;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_dbal_cgm_egress_fifo_fc_threshold_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_cgm_egress_fifo_fc_threshold_max_value;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2048;

    
    define->data = 2048;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_dbal_cgm_ingress_fc_threshold_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_cgm_ingress_fc_threshold_max_value;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 12;

    
    define->data = 12;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_dbal_fabric_transmit_wfq_index_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_fabric_transmit_wfq_index_bits_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_dbal_fabric_transmit_wfq_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_fabric_transmit_wfq_index_max_value;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 9;

    
    define->data = 9;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_dbal_mesh_topology_init_config_2_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_mesh_topology_init_config_2_bits_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_dbal_fabric_transmit_dtq_fc_thresholds_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_fabric_transmit_dtq_fc_thresholds_bits_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 12;

    
    define->data = 12;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_dbal_fabric_transmit_pdq_size_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_fabric_transmit_pdq_size_bits_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 15;

    
    define->data = 15;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_dbal_priority_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_priority_bits_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = utilex_msb_bit_on(dnx_data_fabric.cell.nof_priorities_get(unit)-1)+1;

    
    define->data = utilex_msb_bit_on(dnx_data_fabric.cell.nof_priorities_get(unit)-1)+1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_dbal_nof_links_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_nof_links_bits_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = utilex_msb_bit_on(dnx_data_fabric.links.nof_links_get(unit))+1;

    
    define->data = utilex_msb_bit_on(dnx_data_fabric.links.nof_links_get(unit))+1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_fabric_mesh_mc_nof_replication_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_mesh_mc;
    int define_index = dnx_data_fabric_mesh_mc_define_nof_replication;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5;

    
    define->data = 5;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_fabric_transmit_s2d_pdq_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_transmit;
    int define_index = dnx_data_fabric_transmit_define_s2d_pdq_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1023;

    
    define->data = 1023;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_transmit_pdq_sram_fc_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_transmit;
    int define_index = dnx_data_fabric_transmit_define_pdq_sram_fc_th;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 700;

    
    define->data = 700;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_transmit_pdq_mixs_fc_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_transmit;
    int define_index = dnx_data_fabric_transmit_define_pdq_mixs_fc_th;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 700;

    
    define->data = 700;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_transmit_pdq_mixd_fc_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_transmit;
    int define_index = dnx_data_fabric_transmit_define_pdq_mixd_fc_th;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 700;

    
    define->data = 700;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_transmit_pdq_mix_mixs_fc_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_transmit;
    int define_index = dnx_data_fabric_transmit_define_pdq_mix_mixs_fc_th;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 600;

    
    define->data = 600;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_transmit_pdq_mix_mixd_fc_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_transmit;
    int define_index = dnx_data_fabric_transmit_define_pdq_mix_mixd_fc_th;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 600;

    
    define->data = 600;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_transmit_pdq_ocb_fc_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_transmit;
    int define_index = dnx_data_fabric_transmit_define_pdq_ocb_fc_th;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 700;

    
    define->data = 700;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_transmit_pdq_dram_delete_fc_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_transmit;
    int define_index = dnx_data_fabric_transmit_define_pdq_dram_delete_fc_th;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 900;

    
    define->data = 900;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_transmit_pdq_sram_delete_fc_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_transmit;
    int define_index = dnx_data_fabric_transmit_define_pdq_sram_delete_fc_th;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 200;

    
    define->data = 200;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_transmit_pdq_s2d_fc_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_transmit;
    int define_index = dnx_data_fabric_transmit_define_pdq_s2d_fc_th;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_fabric.transmit.s2d_pdq_size_get(unit)-150;

    
    define->data = dnx_data_fabric.transmit.s2d_pdq_size_get(unit)-150;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_transmit_pdq_sram_eir_fc_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_transmit;
    int define_index = dnx_data_fabric_transmit_define_pdq_sram_eir_fc_th;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 800;

    
    define->data = 800;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_transmit_pdq_mixs_eir_fc_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_transmit;
    int define_index = dnx_data_fabric_transmit_define_pdq_mixs_eir_fc_th;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 800;

    
    define->data = 800;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_transmit_pdq_mixd_eir_fc_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_transmit;
    int define_index = dnx_data_fabric_transmit_define_pdq_mixd_eir_fc_th;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 800;

    
    define->data = 800;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fabric_transmit_pdq_ocb_eir_fc_th_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_transmit;
    int define_index = dnx_data_fabric_transmit_define_pdq_ocb_eir_fc_th;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 700;

    
    define->data = 700;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_fabric_transmit_dtq_fc_local_thresholds_set(
    int unit)
{
    int dtq_fc_index;
    dnx_data_fabric_transmit_dtq_fc_local_thresholds_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_transmit;
    int table_index = dnx_data_fabric_transmit_table_dtq_fc_local_thresholds;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 4;
    table->info_get.key_size[0] = 4;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fabric_transmit_dtq_fc_local_thresholds_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_fabric_transmit_table_dtq_fc_local_thresholds");

    
    default_data = (dnx_data_fabric_transmit_dtq_fc_local_thresholds_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->threshold = 0;
    
    for (dtq_fc_index = 0; dtq_fc_index < table->keys[0].size; dtq_fc_index++)
    {
        data = (dnx_data_fabric_transmit_dtq_fc_local_thresholds_t *) dnxc_data_mgmt_table_data_get(unit, table, dtq_fc_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_fabric_transmit_dtq_fc_local_thresholds_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->threshold = 80;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_fabric_transmit_dtq_fc_local_thresholds_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->threshold = 60;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_fabric_transmit_dtq_fc_local_thresholds_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->threshold = 40;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_fabric_transmit_dtq_fc_local_thresholds_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->threshold = 20;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
jr2_a0_data_fabric_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_fabric;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_fabric_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_general_define_nof_lcplls;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_general_nof_lcplls_set;
    data_index = dnx_data_fabric_general_define_fmac_clock_khz;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_general_fmac_clock_khz_set;
    data_index = dnx_data_fabric_general_define_connect_mode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_general_connect_mode_set;

    
    data_index = dnx_data_fabric_general_blocks_exist;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_fabric_general_blocks_exist_set;
    data_index = dnx_data_fabric_general_is_jr1_in_system_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_fabric_general_is_jr1_in_system_supported_set;

    
    data_index = dnx_data_fabric_general_table_fmac_bus_size;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_fabric_general_fmac_bus_size_set;
    data_index = dnx_data_fabric_general_table_pll_phys;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_fabric_general_pll_phys_set;
    data_index = dnx_data_fabric_general_table_pm_properties;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_fabric_general_pm_properties_set;
    
    submodule_index = dnx_data_fabric_submodule_links;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_links_define_nof_links;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_links_nof_links_set;
    data_index = dnx_data_fabric_links_define_nof_links_per_core;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_links_nof_links_per_core_set;
    data_index = dnx_data_fabric_links_define_max_link_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_links_max_link_id_set;
    data_index = dnx_data_fabric_links_define_max_speed;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_links_max_speed_set;
    data_index = dnx_data_fabric_links_define_usec_between_load_balancing_events;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_links_usec_between_load_balancing_events_set;
    data_index = dnx_data_fabric_links_define_kr_fec_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_links_kr_fec_supported_set;
    data_index = dnx_data_fabric_links_define_core_mapping_mode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_links_core_mapping_mode_set;

    
    data_index = dnx_data_fabric_links_load_balancing_periodic_event_enabled;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_fabric_links_load_balancing_periodic_event_enabled_set;

    
    data_index = dnx_data_fabric_links_table_general;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_fabric_links_general_set;
    data_index = dnx_data_fabric_links_table_polarity;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_fabric_links_polarity_set;
    data_index = dnx_data_fabric_links_table_supported_interfaces;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_fabric_links_supported_interfaces_set;
    
    submodule_index = dnx_data_fabric_submodule_blocks;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_blocks_define_nof_instances_fmac;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_blocks_nof_instances_fmac_set;
    data_index = dnx_data_fabric_blocks_define_nof_links_in_fmac;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_blocks_nof_links_in_fmac_set;
    data_index = dnx_data_fabric_blocks_define_nof_instances_fsrd;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_blocks_nof_instances_fsrd_set;
    data_index = dnx_data_fabric_blocks_define_nof_links_in_fsrd;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_blocks_nof_links_in_fsrd_set;
    data_index = dnx_data_fabric_blocks_define_nof_fmacs_in_fsrd;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_blocks_nof_fmacs_in_fsrd_set;
    data_index = dnx_data_fabric_blocks_define_nof_pms;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_blocks_nof_pms_set;
    data_index = dnx_data_fabric_blocks_define_nof_links_in_pm;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_blocks_nof_links_in_pm_set;
    data_index = dnx_data_fabric_blocks_define_nof_fmacs_in_pm;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_blocks_nof_fmacs_in_pm_set;
    data_index = dnx_data_fabric_blocks_define_nof_instances_fdtl;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_blocks_nof_instances_fdtl_set;
    data_index = dnx_data_fabric_blocks_define_nof_links_in_fdtl;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_blocks_nof_links_in_fdtl_set;
    data_index = dnx_data_fabric_blocks_define_nof_sbus_chains;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_blocks_nof_sbus_chains_set;

    

    
    data_index = dnx_data_fabric_blocks_table_fsrd_sbus_chain;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_fabric_blocks_fsrd_sbus_chain_set;
    data_index = dnx_data_fabric_blocks_table_fmac_sbus_chain;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_fabric_blocks_fmac_sbus_chain_set;
    data_index = dnx_data_fabric_blocks_table_fsrd_ref_clk_master;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_fabric_blocks_fsrd_ref_clk_master_set;
    
    submodule_index = dnx_data_fabric_submodule_reachability;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_reachability_define_resolution;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_reachability_resolution_set;
    data_index = dnx_data_fabric_reachability_define_gen_period;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_reachability_gen_period_set;
    data_index = dnx_data_fabric_reachability_define_full_cycle_period_usec;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_reachability_full_cycle_period_usec_set;
    data_index = dnx_data_fabric_reachability_define_watchdog_resolution;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_reachability_watchdog_resolution_set;
    data_index = dnx_data_fabric_reachability_define_watchdog_period_usec;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_reachability_watchdog_period_usec_set;

    

    
    
    submodule_index = dnx_data_fabric_submodule_mesh;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_mesh_define_max_mc_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_mesh_max_mc_id_set;
    data_index = dnx_data_fabric_mesh_define_max_nof_dest;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_mesh_max_nof_dest_set;
    data_index = dnx_data_fabric_mesh_define_multicast_enable;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_mesh_multicast_enable_set;
    data_index = dnx_data_fabric_mesh_define_nof_dest;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_mesh_nof_dest_set;

    
    data_index = dnx_data_fabric_mesh_is_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_fabric_mesh_is_supported_set;

    
    
    submodule_index = dnx_data_fabric_submodule_cell;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_cell_define_nof_priorities;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_cell_nof_priorities_set;
    data_index = dnx_data_fabric_cell_define_vsc256_max_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_cell_vsc256_max_size_set;
    data_index = dnx_data_fabric_cell_define_sr_cell_header_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_cell_sr_cell_header_offset_set;
    data_index = dnx_data_fabric_cell_define_sr_cell_nof_instances;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_cell_sr_cell_nof_instances_set;
    data_index = dnx_data_fabric_cell_define_default_packing_mode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_cell_default_packing_mode_set;
    data_index = dnx_data_fabric_cell_define_pcp_enable;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_cell_pcp_enable_set;

    

    
    
    submodule_index = dnx_data_fabric_submodule_pipes;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_pipes_define_max_nof_pipes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_pipes_max_nof_pipes_set;
    data_index = dnx_data_fabric_pipes_define_max_nof_contexts;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_pipes_max_nof_contexts_set;
    data_index = dnx_data_fabric_pipes_define_max_nof_subcontexts;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_pipes_max_nof_subcontexts_set;
    data_index = dnx_data_fabric_pipes_define_nof_pipes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_pipes_nof_pipes_set;
    data_index = dnx_data_fabric_pipes_define_multi_pipe_system;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_pipes_multi_pipe_system_set;

    

    
    data_index = dnx_data_fabric_pipes_table_map;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_fabric_pipes_map_set;
    data_index = dnx_data_fabric_pipes_table_valid_map_config;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_fabric_pipes_valid_map_config_set;
    
    submodule_index = dnx_data_fabric_submodule_debug;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_debug_define_mesh_topology_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_debug_mesh_topology_size_set;
    data_index = dnx_data_fabric_debug_define_mesh_topology_fast;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_debug_mesh_topology_fast_set;

    

    
    
    submodule_index = dnx_data_fabric_submodule_mesh_topology;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_mesh_topology_define_mesh_init;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_mesh_topology_mesh_init_set;
    data_index = dnx_data_fabric_mesh_topology_define_mesh_config_1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_mesh_topology_mesh_config_1_set;

    
    data_index = dnx_data_fabric_mesh_topology_calendar_mode_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_fabric_mesh_topology_calendar_mode_supported_set;

    
    
    submodule_index = dnx_data_fabric_submodule_tdm;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_tdm_define_priority;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_tdm_priority_set;

    

    
    
    submodule_index = dnx_data_fabric_submodule_cgm;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_cgm_define_nof_rci_levels;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_cgm_nof_rci_levels_set;
    data_index = dnx_data_fabric_cgm_define_nof_rci_threshold_levels;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_cgm_nof_rci_threshold_levels_set;
    data_index = dnx_data_fabric_cgm_define_egress_fifo_fc_threshold;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_cgm_egress_fifo_fc_threshold_set;

    

    
    data_index = dnx_data_fabric_cgm_table_access_map;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_fabric_cgm_access_map_set;
    
    submodule_index = dnx_data_fabric_submodule_dtqs;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_dtqs_define_max_nof_subdtqs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_dtqs_max_nof_subdtqs_set;
    data_index = dnx_data_fabric_dtqs_define_max_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_dtqs_max_size_set;
    data_index = dnx_data_fabric_dtqs_define_ocb_only_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_dtqs_ocb_only_size_set;
    data_index = dnx_data_fabric_dtqs_define_nof_active_dtqs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_dtqs_nof_active_dtqs_set;

    

    
    
    submodule_index = dnx_data_fabric_submodule_ilkn;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_fabric_ilkn_table_bypass_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_fabric_ilkn_bypass_info_set;
    
    submodule_index = dnx_data_fabric_submodule_dbal;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_dbal_define_link_rci_threshold_bits_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_dbal_link_rci_threshold_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_cgm_llfc_threshold_bits_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_dbal_cgm_llfc_threshold_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_cgm_llfc_threshold_max_value;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_dbal_cgm_llfc_threshold_max_value_set;
    data_index = dnx_data_fabric_dbal_define_egress_fifo_drop_threshold_bits_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_dbal_egress_fifo_drop_threshold_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_egress_fifo_drop_threshold_max_value;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_dbal_egress_fifo_drop_threshold_max_value_set;
    data_index = dnx_data_fabric_dbal_define_cgm_link_fifo_base_address_bits_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_dbal_cgm_link_fifo_base_address_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_cgm_link_fifo_base_address_max_value;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_dbal_cgm_link_fifo_base_address_max_value_set;
    data_index = dnx_data_fabric_dbal_define_cgm_egress_fifo_base_address_bits_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_dbal_cgm_egress_fifo_base_address_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_cgm_egress_fifo_base_address_max_value;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_dbal_cgm_egress_fifo_base_address_max_value_set;
    data_index = dnx_data_fabric_dbal_define_cgm_egress_fifo_fc_threshold_bits_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_dbal_cgm_egress_fifo_fc_threshold_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_cgm_egress_fifo_fc_threshold_max_value;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_dbal_cgm_egress_fifo_fc_threshold_max_value_set;
    data_index = dnx_data_fabric_dbal_define_cgm_ingress_fc_threshold_max_value;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_dbal_cgm_ingress_fc_threshold_max_value_set;
    data_index = dnx_data_fabric_dbal_define_fabric_transmit_wfq_index_bits_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_dbal_fabric_transmit_wfq_index_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_fabric_transmit_wfq_index_max_value;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_dbal_fabric_transmit_wfq_index_max_value_set;
    data_index = dnx_data_fabric_dbal_define_mesh_topology_init_config_2_bits_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_dbal_mesh_topology_init_config_2_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_fabric_transmit_dtq_fc_thresholds_bits_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_dbal_fabric_transmit_dtq_fc_thresholds_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_fabric_transmit_pdq_size_bits_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_dbal_fabric_transmit_pdq_size_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_priority_bits_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_dbal_priority_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_nof_links_bits_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_dbal_nof_links_bits_nof_set;

    

    
    
    submodule_index = dnx_data_fabric_submodule_mesh_mc;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_mesh_mc_define_nof_replication;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_mesh_mc_nof_replication_set;

    

    
    
    submodule_index = dnx_data_fabric_submodule_transmit;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_transmit_define_s2d_pdq_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_transmit_s2d_pdq_size_set;
    data_index = dnx_data_fabric_transmit_define_pdq_sram_fc_th;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_transmit_pdq_sram_fc_th_set;
    data_index = dnx_data_fabric_transmit_define_pdq_mixs_fc_th;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_transmit_pdq_mixs_fc_th_set;
    data_index = dnx_data_fabric_transmit_define_pdq_mixd_fc_th;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_transmit_pdq_mixd_fc_th_set;
    data_index = dnx_data_fabric_transmit_define_pdq_mix_mixs_fc_th;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_transmit_pdq_mix_mixs_fc_th_set;
    data_index = dnx_data_fabric_transmit_define_pdq_mix_mixd_fc_th;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_transmit_pdq_mix_mixd_fc_th_set;
    data_index = dnx_data_fabric_transmit_define_pdq_ocb_fc_th;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_transmit_pdq_ocb_fc_th_set;
    data_index = dnx_data_fabric_transmit_define_pdq_dram_delete_fc_th;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_transmit_pdq_dram_delete_fc_th_set;
    data_index = dnx_data_fabric_transmit_define_pdq_sram_delete_fc_th;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_transmit_pdq_sram_delete_fc_th_set;
    data_index = dnx_data_fabric_transmit_define_pdq_s2d_fc_th;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_transmit_pdq_s2d_fc_th_set;
    data_index = dnx_data_fabric_transmit_define_pdq_sram_eir_fc_th;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_transmit_pdq_sram_eir_fc_th_set;
    data_index = dnx_data_fabric_transmit_define_pdq_mixs_eir_fc_th;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_transmit_pdq_mixs_eir_fc_th_set;
    data_index = dnx_data_fabric_transmit_define_pdq_mixd_eir_fc_th;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_transmit_pdq_mixd_eir_fc_th_set;
    data_index = dnx_data_fabric_transmit_define_pdq_ocb_eir_fc_th;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fabric_transmit_pdq_ocb_eir_fc_th_set;

    

    
    data_index = dnx_data_fabric_transmit_table_dtq_fc_local_thresholds;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_fabric_transmit_dtq_fc_local_thresholds_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

