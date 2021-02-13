/** \file j2p_a0_data_system_red.c
 * 
 * DEVICE DATA - SYSTEM_RED
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
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ
/*
 * INCLUDE FILES:
 * {
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_system_red.h>
#include <bcm_int/dnx/cosq/ingress/system_red.h>
#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
/*
 * }
 */

/*
 * FUNCTIONS:
 * {
 */
/*
 * Submodule: info
 */

/*
 * Features
 */
/*
 * Defines
 */
/*
 * Tables
 */
/**
 * \brief device level function which set values for table resource
 * Module - 'system_red', Submodule - 'info', table - 'resource'
 * system red tracked resources
 * The function set relevant table structure in _dnx_data
 * 
 * \param [in] unit - Unit #
 * 
 * \return
 *     err - See shr_error_e
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
static shr_error_e
j2p_a0_dnx_data_system_red_info_resource_set(
    int unit)
{
    int type_index;
    dnx_data_system_red_info_resource_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_system_red;
    int submodule_index = dnx_data_system_red_submodule_info;
    int table_index = dnx_data_system_red_info_table_resource;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    /* Set data flags as supported */
    table->flags |= DNXC_DATA_F_SUPPORTED;

    /*
     * Set key sizes
     */
    table->keys[0].size = 3;
    table->info_get.key_size[0] = 3;

    /* Info - default values */
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    /* Exit the function if the table size is zero */
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    /* Allocate data buffer */
    DNXC_DATA_ALLOC(table->data, dnx_data_system_red_info_resource_t, (1 * (table->keys[0].size) + 1 /* to store default value */ ), "data of dnx_data_system_red_info_table_resource");

    /* Store Default Values */
    default_data = (dnx_data_system_red_info_resource_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->max = 0;
    default_data->hw_resolution_max = 0;
    default_data->hw_resolution_nof_bits = 0;
    /* Set Default Values */
    for (type_index = 0; type_index < table->keys[0].size; type_index++)
    {
        data = (dnx_data_system_red_info_resource_t *) dnxc_data_mgmt_table_data_get(unit, table, type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    /*
     * Set Values - Entries
     */
    if (DNX_SYSTEM_RED_RESOURCE_SRAM_PDBS < table->keys[0].size)
    {
        data = (dnx_data_system_red_info_resource_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SYSTEM_RED_RESOURCE_SRAM_PDBS, 0);
        data->max = dnx_data_ingr_congestion.info.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_PDS)->max/dnx_data_ingr_congestion.info.nof_pds_in_pdb_get(unit);
        data->hw_resolution_max = dnx_data_system_red.info.resource_get(unit, DNX_SYSTEM_RED_RESOURCE_SRAM_PDBS)->max/dnx_data_ingr_congestion.info.threshold_granularity_get(unit);
        data->hw_resolution_nof_bits = 12;
    }
    if (DNX_SYSTEM_RED_RESOURCE_SRAM_BUFFERS < table->keys[0].size)
    {
        data = (dnx_data_system_red_info_resource_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SYSTEM_RED_RESOURCE_SRAM_BUFFERS, 0);
        data->max = dnx_data_ingr_congestion.info.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS)->max;
        data->hw_resolution_max = dnx_data_ingr_congestion.info.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS)->hw_resolution_max;
        data->hw_resolution_nof_bits = 13;
    }
    if (DNX_SYSTEM_RED_RESOURCE_DRAM_BDBS < table->keys[0].size)
    {
        data = (dnx_data_system_red_info_resource_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SYSTEM_RED_RESOURCE_DRAM_BDBS, 0);
        data->max = dnx_data_dram.buffers.nof_bdbs_get(unit)-1;
        data->hw_resolution_max = dnx_data_system_red.info.resource_get(unit, DNX_SYSTEM_RED_RESOURCE_DRAM_BDBS)->max/dnx_data_ingr_congestion.info.threshold_granularity_get(unit);
        data->hw_resolution_nof_bits = 13;
    }

    /* Override data with dynamic data (if required) */
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}

/*
 * Device attach func
 */
/**
 * \brief Attach device to module - attach set function to data structure
 * 
 * \param [in] unit - Unit #
 * 
 * \return
 *     err - 
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
shr_error_e
j2p_a0_data_system_red_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_system_red;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    /*
     * Attach submodule: info
     */
    submodule_index = dnx_data_system_red_submodule_info;
    submodule = &module->submodules[submodule_index];

    /*
     * Attach defines: 
     */

    /*
     * Attach features: 
     */

    /*
     * Attach tables: 
     */
    data_index = dnx_data_system_red_info_table_resource;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_system_red_info_resource_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE
/* *INDENT-ON* */
