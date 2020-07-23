

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_lane_map.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <bcm/port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>









static shr_error_e
jr2_a0_dnx_data_lane_map_nif_mapping_set(
    int unit)
{
    int lane_id_index;
    dnx_data_lane_map_nif_mapping_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_lane_map;
    int submodule_index = dnx_data_lane_map_submodule_nif;
    int table_index = dnx_data_lane_map_nif_table_mapping;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->flags |= DNXC_DATA_F_INIT_ONLY;

    
    table->keys[0].size = dnx_data_nif.phys.nof_phys_get(unit);
    table->info_get.key_size[0] = dnx_data_nif.phys.nof_phys_get(unit);

    
    table->values[0].default_val = "BCM_PORT_LANE_TO_SERDES_NOT_MAPPED";
    table->values[1].default_val = "BCM_PORT_LANE_TO_SERDES_NOT_MAPPED";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_lane_map_nif_mapping_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_lane_map_nif_table_mapping");

    
    default_data = (dnx_data_lane_map_nif_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->serdes_rx_id = BCM_PORT_LANE_TO_SERDES_NOT_MAPPED;
    default_data->serdes_tx_id = BCM_PORT_LANE_TO_SERDES_NOT_MAPPED;
    
    for (lane_id_index = 0; lane_id_index < table->keys[0].size; lane_id_index++)
    {
        data = (dnx_data_lane_map_nif_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, lane_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->property.name = "lane_to_serdes_map";
    table->property.doc =
        "\n"
        "Define paired serdes rx id and tx id with the same lane number.\n"
        "\n"
    ;
    table->property.method = dnxc_data_property_method_custom;
    table->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (lane_id_index = 0; lane_id_index < table->keys[0].size; lane_id_index++)
    {
        data = (dnx_data_lane_map_nif_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, lane_id_index, 0);
        SHR_IF_ERR_EXIT(dnx_data_property_lane_map_nif_mapping_read(unit, lane_id_index, data));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_lane_map_fabric_resolution_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_lane_map;
    int submodule_index = dnx_data_lane_map_submodule_fabric;
    int define_index = dnx_data_lane_map_fabric_define_resolution;
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
jr2_a0_dnx_data_lane_map_fabric_mapping_set(
    int unit)
{
    int lane_id_index;
    dnx_data_lane_map_fabric_mapping_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_lane_map;
    int submodule_index = dnx_data_lane_map_submodule_fabric;
    int table_index = dnx_data_lane_map_fabric_table_mapping;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->flags |= DNXC_DATA_F_INIT_ONLY;

    
    table->keys[0].size = dnx_data_fabric.links.nof_links_get(unit);
    table->info_get.key_size[0] = dnx_data_fabric.links.nof_links_get(unit);

    
    table->values[0].default_val = "BCM_PORT_LANE_TO_SERDES_NOT_MAPPED";
    table->values[1].default_val = "BCM_PORT_LANE_TO_SERDES_NOT_MAPPED";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_lane_map_fabric_mapping_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_lane_map_fabric_table_mapping");

    
    default_data = (dnx_data_lane_map_fabric_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->serdes_rx_id = BCM_PORT_LANE_TO_SERDES_NOT_MAPPED;
    default_data->serdes_tx_id = BCM_PORT_LANE_TO_SERDES_NOT_MAPPED;
    
    for (lane_id_index = 0; lane_id_index < table->keys[0].size; lane_id_index++)
    {
        data = (dnx_data_lane_map_fabric_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, lane_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->property.name = "lane_to_serdes_map";
    table->property.doc =
        "\n"
        "Define paired serdes rx id and tx id with the same lane number.\n"
        "\n"
    ;
    table->property.method = dnxc_data_property_method_custom;
    table->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (lane_id_index = 0; lane_id_index < table->keys[0].size; lane_id_index++)
    {
        data = (dnx_data_lane_map_fabric_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, lane_id_index, 0);
        SHR_IF_ERR_EXIT(dnx_data_property_lane_map_fabric_mapping_read(unit, lane_id_index, data));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_lane_map_ilkn_remapping_set(
    int unit)
{
    int lane_id_index;
    int ilkn_id_index;
    dnx_data_lane_map_ilkn_remapping_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_lane_map;
    int submodule_index = dnx_data_lane_map_submodule_ilkn;
    int table_index = dnx_data_lane_map_ilkn_table_remapping;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->flags |= DNXC_DATA_F_INIT_ONLY;

    
    table->keys[0].size = dnx_data_nif.phys.nof_phys_get(unit);
    table->info_get.key_size[0] = dnx_data_nif.phys.nof_phys_get(unit);
    table->keys[1].size = 2;
    table->info_get.key_size[1] = 2;

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_lane_map_ilkn_remapping_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_lane_map_ilkn_table_remapping");

    
    default_data = (dnx_data_lane_map_ilkn_remapping_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->ilkn_lane_id = -1;
    
    for (lane_id_index = 0; lane_id_index < table->keys[0].size; lane_id_index++)
    {
        for (ilkn_id_index = 0; ilkn_id_index < table->keys[1].size; ilkn_id_index++)
        {
            data = (dnx_data_lane_map_ilkn_remapping_t *) dnxc_data_mgmt_table_data_get(unit, table, lane_id_index, ilkn_id_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    table->property.name = "ilkn_lane_map";
    table->property.doc =
        "\n"
        "Re-assign the ILKN lane order.\n"
        "\n"
    ;
    table->property.method = dnxc_data_property_method_custom;
    table->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (lane_id_index = 0; lane_id_index < table->keys[0].size; lane_id_index++)
    {
        for (ilkn_id_index = 0; ilkn_id_index < table->keys[1].size; ilkn_id_index++)
        {
            data = (dnx_data_lane_map_ilkn_remapping_t *) dnxc_data_mgmt_table_data_get(unit, table, lane_id_index, ilkn_id_index);
            SHR_IF_ERR_EXIT(dnx_data_property_lane_map_ilkn_remapping_read(unit, lane_id_index, ilkn_id_index, data));
        }
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
jr2_a0_data_lane_map_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_lane_map;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_lane_map_submodule_nif;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_lane_map_nif_table_mapping;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_lane_map_nif_mapping_set;
    
    submodule_index = dnx_data_lane_map_submodule_fabric;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_lane_map_fabric_define_resolution;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_lane_map_fabric_resolution_set;

    

    
    data_index = dnx_data_lane_map_fabric_table_mapping;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_lane_map_fabric_mapping_set;
    
    submodule_index = dnx_data_lane_map_submodule_ilkn;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_lane_map_ilkn_table_remapping;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_lane_map_ilkn_remapping_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

