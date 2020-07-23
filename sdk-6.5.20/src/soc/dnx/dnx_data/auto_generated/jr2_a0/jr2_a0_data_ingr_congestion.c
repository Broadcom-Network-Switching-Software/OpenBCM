

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_ingr_congestion.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_system_red.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>
#include <bcm_int/dnx/cosq/ingress/system_red.h>
#include <soc/dnx/dbal/dbal.h>







static shr_error_e
jr2_a0_dnx_data_ingr_congestion_config_pp_port_by_reassembly_overwrite_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_config;
    int feature_index = dnx_data_ingr_congestion_config_pp_port_by_reassembly_overwrite;
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
jr2_a0_dnx_data_ingr_congestion_config_guarantee_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_config;
    int define_index = dnx_data_ingr_congestion_config_define_guarantee_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_INGRESS_CONGESTION_MANAGEMENT;
    define->property.doc = 
        "\n"
        "legacy SoC property to define CGM guarantee mode\n"
        "defined here just to ensure unsupported value is not specified by user\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_direct_map;
    define->property.method_str = "suffix_direct_map";
    define->property.suffix = "guarantee_mode";
    define->property.nof_mapping = 1;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "LOOSE";
    define->property.mapping[0].val = 0;
    define->property.mapping[0].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_config_wred_packet_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_config;
    int define_index = dnx_data_ingr_congestion_config_define_wred_packet_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_DISCARD_MTU_SIZE;
    define->property.doc = 
        "\n"
        "Maximal packet size\n"
        "used in computation of WRED parameters\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 16*1024-1;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_ingr_congestion_info_latency_based_admission_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_info;
    int feature_index = dnx_data_ingr_congestion_info_latency_based_admission;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_info_cgm_hw_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_info;
    int feature_index = dnx_data_ingr_congestion_info_cgm_hw_support;
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
jr2_a0_dnx_data_ingr_congestion_info_threshold_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_info;
    int define_index = dnx_data_ingr_congestion_info_define_threshold_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_info_words_resolution_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_info;
    int define_index = dnx_data_ingr_congestion_info_define_words_resolution;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_info_bytes_threshold_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_info;
    int define_index = dnx_data_ingr_congestion_info_define_bytes_threshold_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_ingr_congestion.info.threshold_granularity_get(unit)*dnx_data_ingr_congestion.info.words_resolution_get(unit);

    
    define->data = dnx_data_ingr_congestion.info.threshold_granularity_get(unit)*dnx_data_ingr_congestion.info.words_resolution_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_info_nof_dropped_reasons_cgm_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_info;
    int define_index = dnx_data_ingr_congestion_info_define_nof_dropped_reasons_cgm;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 33;

    
    define->data = 33;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_info_wred_max_gain_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_info;
    int define_index = dnx_data_ingr_congestion_info_define_wred_max_gain;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 31;

    
    define->data = 31;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_info_wred_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_info;
    int define_index = dnx_data_ingr_congestion_info_define_wred_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_info_nof_pds_in_pdb_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_info;
    int define_index = dnx_data_ingr_congestion_info_define_nof_pds_in_pdb;
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
jr2_a0_dnx_data_ingr_congestion_info_max_sram_pdbs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_info;
    int define_index = dnx_data_ingr_congestion_info_define_max_sram_pdbs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_ingr_congestion.info.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_PDS)->max/dnx_data_ingr_congestion.info.nof_pds_in_pdb_get(unit);

    
    define->data = dnx_data_ingr_congestion.info.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_PDS)->max/dnx_data_ingr_congestion.info.nof_pds_in_pdb_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_info_max_dram_bdbs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_info;
    int define_index = dnx_data_ingr_congestion_info_define_max_dram_bdbs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_dram.buffers.nof_bdbs_get(unit);

    
    define->data = dnx_data_dram.buffers.nof_bdbs_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_ingr_congestion_info_resource_set(
    int unit)
{
    int type_index;
    dnx_data_ingr_congestion_info_resource_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_info;
    int table_index = dnx_data_ingr_congestion_info_table_resource;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 3;
    table->info_get.key_size[0] = 3;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    table->values[4].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_ingr_congestion_info_resource_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_ingr_congestion_info_table_resource");

    
    default_data = (dnx_data_ingr_congestion_info_resource_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->max = 0;
    default_data->hw_resolution_max = 0;
    default_data->hw_resolution_nof_bits = 0;
    default_data->fadt_alpha_min = 0;
    default_data->fadt_alpha_max = 0;
    
    for (type_index = 0; type_index < table->keys[0].size; type_index++)
    {
        data = (dnx_data_ingr_congestion_info_resource_t *) dnxc_data_mgmt_table_data_get(unit, table, type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_info_resource_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES, 0);
        data->max = 0xffffffff;
        data->hw_resolution_max = dnx_data_ingr_congestion.info.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES)->max/dnx_data_ingr_congestion.info.bytes_threshold_granularity_get(unit);
        data->hw_resolution_nof_bits = 24;
        data->fadt_alpha_min = -15;
        data->fadt_alpha_max = 15;
    }
    if (DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_info_resource_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS, 0);
        data->max = 64*1024-1;
        data->hw_resolution_max = dnx_data_ingr_congestion.info.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS)->max/dnx_data_ingr_congestion.info.threshold_granularity_get(unit);
        data->hw_resolution_nof_bits = 12;
        data->fadt_alpha_min = -7;
        data->fadt_alpha_max = 7;
    }
    if (DNX_INGRESS_CONGESTION_RESOURCE_SRAM_PDS < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_info_resource_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_PDS, 0);
        data->max = 128*1024-1;
        data->hw_resolution_max = dnx_data_ingr_congestion.info.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_PDS)->max/dnx_data_ingr_congestion.info.threshold_granularity_get(unit);
        data->hw_resolution_nof_bits = 13;
        data->fadt_alpha_min = -7;
        data->fadt_alpha_max = 7;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_set(
    int unit)
{
    int dp_index;
    dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_info;
    int table_index = dnx_data_ingr_congestion_info_table_dp_free_res_presentage_drop;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 4;
    table->info_get.key_size[0] = 4;

    
    table->values[0].default_val = "100";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_ingr_congestion_info_table_dp_free_res_presentage_drop");

    
    default_data = (dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->drop_precentage = 100;
    
    for (dp_index = 0; dp_index < table->keys[0].size; dp_index++)
    {
        data = (dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_t *) dnxc_data_mgmt_table_data_get(unit, table, dp_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->drop_precentage = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->drop_precentage = 15;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->drop_precentage = 25;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->drop_precentage = 100;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_info_admission_preferences_set(
    int unit)
{
    int dp_index;
    dnx_data_ingr_congestion_info_admission_preferences_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_info;
    int table_index = dnx_data_ingr_congestion_info_table_admission_preferences;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 3;
    table->info_get.key_size[0] = 3;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_ingr_congestion_info_admission_preferences_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_ingr_congestion_info_table_admission_preferences");

    
    default_data = (dnx_data_ingr_congestion_info_admission_preferences_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->is_guarantee_over_admit = 0;
    
    for (dp_index = 0; dp_index < table->keys[0].size; dp_index++)
    {
        data = (dnx_data_ingr_congestion_info_admission_preferences_t *) dnxc_data_mgmt_table_data_get(unit, table, dp_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_COSQ_ADMISSION_PREFERENCE;
    table->values[0].property.doc =
        "\n"
        "if GUARANTEE_OVER_ADMIT is set, than in case statistic admission tests fail (for example WRED),\n"
        "but the queue level is within the guaranteed, packet will enter.\n"
        "this soc property is only rlevant for DPs 0-2 (DP3 is always dropped).\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_suffix_direct_map;
    table->values[0].property.method_str = "suffix_direct_map";
    table->values[0].property.suffix = "";
    table->values[0].property.nof_mapping = 2;
    DNXC_DATA_ALLOC(table->values[0].property.mapping, dnxc_data_property_map_t, table->values[0].property.nof_mapping, "dnx_data_ingr_congestion_info_admission_preferences_t property mapping");

    table->values[0].property.mapping[0].name = "ADMIT_OVER_GUARANTEE";
    table->values[0].property.mapping[0].val = 0;
    table->values[0].property.mapping[0].is_default = 1 ;
    table->values[0].property.mapping[1].name = "GUARANTEE_OVER_ADMIT";
    table->values[0].property.mapping[1].val = 1;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (dp_index = 0; dp_index < table->keys[0].size; dp_index++)
    {
        data = (dnx_data_ingr_congestion_info_admission_preferences_t *) dnxc_data_mgmt_table_data_get(unit, table, dp_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, dp_index, &data->is_guarantee_over_admit));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_ingr_congestion_fadt_tail_drop_default_max_size_byte_threshold_for_ocb_only_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_fadt_tail_drop;
    int define_index = dnx_data_ingr_congestion_fadt_tail_drop_define_default_max_size_byte_threshold_for_ocb_only;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8*1024*1024;

    
    define->data = 8*1024*1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_ingr_congestion_dram_bound_fadt_alpha_min_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_dram_bound;
    int define_index = dnx_data_ingr_congestion_dram_bound_define_fadt_alpha_min;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = -8;

    
    define->data = -8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_dram_bound_fadt_alpha_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_dram_bound;
    int define_index = dnx_data_ingr_congestion_dram_bound_define_fadt_alpha_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 7;

    
    define->data = 7;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_ingr_congestion_dram_bound_resource_set(
    int unit)
{
    int type_index;
    dnx_data_ingr_congestion_dram_bound_resource_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_dram_bound;
    int table_index = dnx_data_ingr_congestion_dram_bound_table_resource;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 3;
    table->info_get.key_size[0] = 3;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_ingr_congestion_dram_bound_resource_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_ingr_congestion_dram_bound_table_resource");

    
    default_data = (dnx_data_ingr_congestion_dram_bound_resource_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->max = 0;
    default_data->is_resource_range = 0;
    
    for (type_index = 0; type_index < table->keys[0].size; type_index++)
    {
        data = (dnx_data_ingr_congestion_dram_bound_resource_t *) dnxc_data_mgmt_table_data_get(unit, table, type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_SRAM_BYTES < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dram_bound_resource_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_SRAM_BYTES, 0);
        data->max = 0xffffff;
        data->is_resource_range = FALSE;
    }
    if (DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_SRAM_BUFFERS < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dram_bound_resource_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_SRAM_BUFFERS, 0);
        data->max = dnx_data_ingr_congestion.info.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS)->max;
        data->is_resource_range = TRUE;
    }
    if (DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_SRAM_PDS < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dram_bound_resource_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_SRAM_PDS, 0);
        data->max = dnx_data_ingr_congestion.info.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_PDS)->max;
        data->is_resource_range = TRUE;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_ingr_congestion_voq_nof_rate_class_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_voq;
    int define_index = dnx_data_ingr_congestion_voq_define_nof_rate_class;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_voq_rate_class_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_voq;
    int define_index = dnx_data_ingr_congestion_voq_define_rate_class_nof_bits;
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
jr2_a0_dnx_data_ingr_congestion_voq_nof_compensation_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_voq;
    int define_index = dnx_data_ingr_congestion_voq_define_nof_compensation_profiles;
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
jr2_a0_dnx_data_ingr_congestion_voq_default_compensation_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_voq;
    int define_index = dnx_data_ingr_congestion_voq_define_default_compensation;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 24;

    
    define->data = 24;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_voq_latency_bins_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_voq;
    int define_index = dnx_data_ingr_congestion_voq_define_latency_bins;
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
jr2_a0_dnx_data_ingr_congestion_voq_voq_congestion_notification_fifo_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_voq;
    int define_index = dnx_data_ingr_congestion_voq_define_voq_congestion_notification_fifo_size;
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
jr2_a0_dnx_data_ingr_congestion_vsq_size_watermark_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_vsq;
    int feature_index = dnx_data_ingr_congestion_vsq_size_watermark_support;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_ingr_congestion_vsq_vsq_rate_class_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_vsq;
    int define_index = dnx_data_ingr_congestion_vsq_define_vsq_rate_class_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_vsq_vsq_a_rate_class_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_vsq;
    int define_index = dnx_data_ingr_congestion_vsq_define_vsq_a_rate_class_nof;
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
jr2_a0_dnx_data_ingr_congestion_vsq_vsq_a_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_vsq;
    int define_index = dnx_data_ingr_congestion_vsq_define_vsq_a_nof;
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
jr2_a0_dnx_data_ingr_congestion_vsq_vsq_b_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_vsq;
    int define_index = dnx_data_ingr_congestion_vsq_define_vsq_b_nof;
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
jr2_a0_dnx_data_ingr_congestion_vsq_vsq_c_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_vsq;
    int define_index = dnx_data_ingr_congestion_vsq_define_vsq_c_nof;
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
jr2_a0_dnx_data_ingr_congestion_vsq_vsq_d_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_vsq;
    int define_index = dnx_data_ingr_congestion_vsq_define_vsq_d_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 256;

    
    define->data = 256;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_vsq_vsq_e_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_vsq;
    int define_index = dnx_data_ingr_congestion_vsq_define_vsq_e_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_ingr_congestion.vsq.vsq_e_hw_nof_get(unit) - 1;

    
    define->data = dnx_data_ingr_congestion.vsq.vsq_e_hw_nof_get(unit) - 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_vsq_vsq_e_hw_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_vsq;
    int define_index = dnx_data_ingr_congestion_vsq_define_vsq_e_hw_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 48+4;

    
    define->data = 48+4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_vsq_vsq_f_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_vsq;
    int define_index = dnx_data_ingr_congestion_vsq_define_vsq_f_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_ingr_congestion.vsq.vsq_f_hw_nof_get(unit) - 1;

    
    define->data = dnx_data_ingr_congestion.vsq.vsq_f_hw_nof_get(unit) - 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_vsq_vsq_f_hw_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_vsq;
    int define_index = dnx_data_ingr_congestion_vsq_define_vsq_f_hw_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 384+8;

    
    define->data = 384+8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_vsq_nif_vsq_e_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_vsq;
    int define_index = dnx_data_ingr_congestion_vsq_define_nif_vsq_e_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 48;

    
    define->data = 48;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_vsq_non_nif_vsq_f_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_vsq;
    int define_index = dnx_data_ingr_congestion_vsq_define_non_nif_vsq_f_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_ingr_congestion.vsq.vsq_f_nof_get(unit) - dnx_data_nif.phys.nof_phys_per_core_get(unit)*8;

    
    define->data = dnx_data_ingr_congestion.vsq.vsq_f_nof_get(unit) - dnx_data_nif.phys.nof_phys_per_core_get(unit)*8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_vsq_pool_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_vsq;
    int define_index = dnx_data_ingr_congestion_vsq_define_pool_nof;
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
jr2_a0_dnx_data_ingr_congestion_vsq_connection_class_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_vsq;
    int define_index = dnx_data_ingr_congestion_vsq_define_connection_class_nof;
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
jr2_a0_dnx_data_ingr_congestion_vsq_tc_pg_profile_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_vsq;
    int define_index = dnx_data_ingr_congestion_vsq_define_tc_pg_profile_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_vsq_vsq_e_congestion_notification_fifo_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_vsq;
    int define_index = dnx_data_ingr_congestion_vsq_define_vsq_e_congestion_notification_fifo_size;
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
jr2_a0_dnx_data_ingr_congestion_vsq_vsq_f_congestion_notification_fifo_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_vsq;
    int define_index = dnx_data_ingr_congestion_vsq_define_vsq_f_congestion_notification_fifo_size;
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
jr2_a0_dnx_data_ingr_congestion_vsq_vsq_e_default_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_vsq;
    int define_index = dnx_data_ingr_congestion_vsq_define_vsq_e_default;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_ingr_congestion.vsq.vsq_e_nof_get(unit);

    
    define->data = dnx_data_ingr_congestion.vsq.vsq_e_nof_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_vsq_vsq_f_default_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_vsq;
    int define_index = dnx_data_ingr_congestion_vsq_define_vsq_f_default;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_ingr_congestion.vsq.vsq_f_nof_get(unit);

    
    define->data = dnx_data_ingr_congestion.vsq.vsq_f_nof_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_ingr_congestion_vsq_info_set(
    int unit)
{
    int vsq_group_index;
    dnx_data_ingr_congestion_vsq_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_vsq;
    int table_index = dnx_data_ingr_congestion_vsq_table_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_INGRESS_CONGESTION_VSQ_GROUP_NOF;
    table->info_get.key_size[0] = DNX_INGRESS_CONGESTION_VSQ_GROUP_NOF;

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_ingr_congestion_vsq_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_ingr_congestion_vsq_table_info");

    
    default_data = (dnx_data_ingr_congestion_vsq_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->nof = -1;
    
    for (vsq_group_index = 0; vsq_group_index < table->keys[0].size; vsq_group_index++)
    {
        data = (dnx_data_ingr_congestion_vsq_info_t *) dnxc_data_mgmt_table_data_get(unit, table, vsq_group_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_INGRESS_CONGESTION_VSQ_GROUP_CTGRY < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_vsq_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_VSQ_GROUP_CTGRY, 0);
        data->nof = dnx_data_ingr_congestion.vsq.vsq_a_nof_get(unit);
    }
    if (DNX_INGRESS_CONGESTION_VSQ_GROUP_CTGRY_TRAFFIC_CLS < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_vsq_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_VSQ_GROUP_CTGRY_TRAFFIC_CLS, 0);
        data->nof = dnx_data_ingr_congestion.vsq.vsq_b_nof_get(unit);
    }
    if (DNX_INGRESS_CONGESTION_VSQ_GROUP_CTGRY_CNCTN_CLS < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_vsq_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_VSQ_GROUP_CTGRY_CNCTN_CLS, 0);
        data->nof = dnx_data_ingr_congestion.vsq.vsq_c_nof_get(unit);
    }
    if (DNX_INGRESS_CONGESTION_VSQ_GROUP_STTSTCS_TAG < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_vsq_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_VSQ_GROUP_STTSTCS_TAG, 0);
        data->nof = dnx_data_ingr_congestion.vsq.vsq_d_nof_get(unit);
    }
    if (DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_vsq_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT, 0);
        data->nof = dnx_data_ingr_congestion.vsq.vsq_e_nof_get(unit);
    }
    if (DNX_INGRESS_CONGESTION_VSQ_GROUP_PG < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_vsq_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_VSQ_GROUP_PG, 0);
        data->nof = dnx_data_ingr_congestion.vsq.vsq_f_nof_get(unit);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_ingr_congestion_init_fifo_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_init;
    int define_index = dnx_data_ingr_congestion_init_define_fifo_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 256;

    
    define->data = 256;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_ingr_congestion_init_vsq_words_rjct_map_set(
    int unit)
{
    int index_index;
    dnx_data_ingr_congestion_init_vsq_words_rjct_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_init;
    int table_index = dnx_data_ingr_congestion_init_table_vsq_words_rjct_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 6;
    table->info_get.key_size[0] = 6;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    table->values[4].default_val = "0";
    table->values[5].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_ingr_congestion_init_vsq_words_rjct_map_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_ingr_congestion_init_table_vsq_words_rjct_map");

    
    default_data = (dnx_data_ingr_congestion_init_vsq_words_rjct_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->is_total_shared_blocked = 0;
    default_data->is_port_pg_shared_blocked = 0;
    default_data->is_total_headroom_blocked = 0;
    default_data->is_port_pg_headroom_blocked = 0;
    default_data->is_voq_in_guaranteed = 0;
    default_data->vsq_guaranteed_status = 0;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_ingr_congestion_init_vsq_words_rjct_map_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_vsq_words_rjct_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->is_total_shared_blocked = 1;
        data->is_port_pg_shared_blocked = 0;
        data->is_total_headroom_blocked = 1;
        data->is_port_pg_headroom_blocked = 1;
        data->vsq_guaranteed_status = DBAL_ENUM_FVAL_VSQ_GUARANTEED_STATUS_BLOCKED;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_vsq_words_rjct_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->is_total_shared_blocked = 0;
        data->is_port_pg_shared_blocked = 1;
        data->is_total_headroom_blocked = 1;
        data->is_port_pg_headroom_blocked = 1;
        data->vsq_guaranteed_status = DBAL_ENUM_FVAL_VSQ_GUARANTEED_STATUS_BLOCKED;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_vsq_words_rjct_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->is_total_shared_blocked = 1;
        data->is_port_pg_shared_blocked = 1;
        data->is_total_headroom_blocked = 1;
        data->is_port_pg_headroom_blocked = 1;
        data->vsq_guaranteed_status = DBAL_ENUM_FVAL_VSQ_GUARANTEED_STATUS_BLOCKED;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_vsq_words_rjct_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->is_total_shared_blocked = 1;
        data->is_port_pg_shared_blocked = 1;
        data->is_total_headroom_blocked = 1;
        data->is_port_pg_headroom_blocked = 0;
        data->vsq_guaranteed_status = DBAL_ENUM_FVAL_VSQ_GUARANTEED_STATUS_BLOCKED;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_vsq_words_rjct_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->is_total_shared_blocked = 1;
        data->is_port_pg_shared_blocked = 1;
        data->is_total_headroom_blocked = 0;
        data->is_port_pg_headroom_blocked = 1;
        data->vsq_guaranteed_status = DBAL_ENUM_FVAL_VSQ_GUARANTEED_STATUS_BLOCKED;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_vsq_words_rjct_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->is_total_shared_blocked = 1;
        data->is_port_pg_shared_blocked = 1;
        data->is_total_headroom_blocked = 1;
        data->is_port_pg_headroom_blocked = 1;
        data->vsq_guaranteed_status = DBAL_ENUM_FVAL_VSQ_GUARANTEED_STATUS_BLOCKED;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_init_vsq_sram_rjct_map_set(
    int unit)
{
    int index_index;
    dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_init;
    int table_index = dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 9;
    table->info_get.key_size[0] = 9;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    table->values[4].default_val = "0";
    table->values[5].default_val = "0";
    table->values[6].default_val = "0";
    table->values[7].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map");

    
    default_data = (dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->is_total_shared_blocked = 0;
    default_data->is_port_pg_shared_blocked = 0;
    default_data->is_headroom_extension_blocked = 0;
    default_data->is_total_headroom_blocked = 0;
    default_data->is_port_headroom_blocked = 0;
    default_data->is_pg_headroom_blocked = 0;
    default_data->is_voq_in_guaranteed = 0;
    default_data->vsq_guaranteed_status = 0;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->is_total_shared_blocked = 0;
        data->is_port_pg_shared_blocked = 1;
        data->is_headroom_extension_blocked = 1;
        data->is_total_headroom_blocked = 1;
        data->is_port_headroom_blocked = 1;
        data->is_pg_headroom_blocked = 1;
        data->vsq_guaranteed_status = DBAL_ENUM_FVAL_VSQ_GUARANTEED_STATUS_BLOCKED;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->is_total_shared_blocked = 1;
        data->is_port_pg_shared_blocked = 0;
        data->is_headroom_extension_blocked = 1;
        data->is_total_headroom_blocked = 1;
        data->is_port_headroom_blocked = 1;
        data->is_pg_headroom_blocked = 1;
        data->vsq_guaranteed_status = DBAL_ENUM_FVAL_VSQ_GUARANTEED_STATUS_BLOCKED;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->is_total_shared_blocked = 1;
        data->is_port_pg_shared_blocked = 1;
        data->is_headroom_extension_blocked = 1;
        data->is_total_headroom_blocked = 1;
        data->is_port_headroom_blocked = 1;
        data->is_pg_headroom_blocked = 1;
        data->vsq_guaranteed_status = DBAL_ENUM_FVAL_VSQ_GUARANTEED_STATUS_BLOCKED;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->is_total_shared_blocked = 1;
        data->is_port_pg_shared_blocked = 1;
        data->is_headroom_extension_blocked = 0;
        data->is_total_headroom_blocked = 1;
        data->is_port_headroom_blocked = 0;
        data->is_pg_headroom_blocked = 0;
        data->vsq_guaranteed_status = DBAL_ENUM_FVAL_VSQ_GUARANTEED_STATUS_BLOCKED;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->is_total_shared_blocked = 1;
        data->is_port_pg_shared_blocked = 1;
        data->is_headroom_extension_blocked = 0;
        data->is_total_headroom_blocked = 0;
        data->is_port_headroom_blocked = 1;
        data->is_pg_headroom_blocked = 0;
        data->vsq_guaranteed_status = DBAL_ENUM_FVAL_VSQ_GUARANTEED_STATUS_BLOCKED;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->is_total_shared_blocked = 1;
        data->is_port_pg_shared_blocked = 1;
        data->is_headroom_extension_blocked = 1;
        data->is_total_headroom_blocked = 0;
        data->is_port_headroom_blocked = 0;
        data->is_pg_headroom_blocked = 1;
        data->vsq_guaranteed_status = DBAL_ENUM_FVAL_VSQ_GUARANTEED_STATUS_BLOCKED;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->is_total_shared_blocked = 1;
        data->is_port_pg_shared_blocked = 1;
        data->is_headroom_extension_blocked = 1;
        data->is_total_headroom_blocked = 0;
        data->is_port_headroom_blocked = 1;
        data->is_pg_headroom_blocked = 1;
        data->vsq_guaranteed_status = DBAL_ENUM_FVAL_VSQ_GUARANTEED_STATUS_BLOCKED;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->is_total_shared_blocked = 1;
        data->is_port_pg_shared_blocked = 1;
        data->is_headroom_extension_blocked = 0;
        data->is_total_headroom_blocked = 1;
        data->is_port_headroom_blocked = 1;
        data->is_pg_headroom_blocked = 0;
        data->vsq_guaranteed_status = DBAL_ENUM_FVAL_VSQ_GUARANTEED_STATUS_BLOCKED;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->is_total_shared_blocked = 1;
        data->is_port_pg_shared_blocked = 1;
        data->is_headroom_extension_blocked = 1;
        data->is_total_headroom_blocked = 1;
        data->is_port_headroom_blocked = 1;
        data->is_pg_headroom_blocked = 1;
        data->vsq_guaranteed_status = DBAL_ENUM_FVAL_VSQ_GUARANTEED_STATUS_BLOCKED;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_set(
    int unit)
{
    int dp_index;
    dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_init;
    int table_index = dnx_data_ingr_congestion_init_table_dp_global_sram_buffer_drop;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 4;
    table->info_get.key_size[0] = 4;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_ingr_congestion_init_table_dp_global_sram_buffer_drop");

    
    default_data = (dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->set_threshold = 0;
    default_data->clear_threshold = 0;
    
    for (dp_index = 0; dp_index < table->keys[0].size; dp_index++)
    {
        data = (dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_t *) dnxc_data_mgmt_table_data_get(unit, table, dp_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->set_threshold = 64;
        data->clear_threshold = 112;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->set_threshold = 128;
        data->clear_threshold = 176;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->set_threshold = 176;
        data->clear_threshold = 240;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->set_threshold = 240;
        data->clear_threshold = 288;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_set(
    int unit)
{
    int dp_index;
    dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_init;
    int table_index = dnx_data_ingr_congestion_init_table_dp_global_sram_pdb_drop;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 4;
    table->info_get.key_size[0] = 4;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_ingr_congestion_init_table_dp_global_sram_pdb_drop");

    
    default_data = (dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->set_threshold = 0;
    default_data->clear_threshold = 0;
    
    for (dp_index = 0; dp_index < table->keys[0].size; dp_index++)
    {
        data = (dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_t *) dnxc_data_mgmt_table_data_get(unit, table, dp_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->set_threshold = 64;
        data->clear_threshold = 112;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->set_threshold = 128;
        data->clear_threshold = 176;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->set_threshold = 176;
        data->clear_threshold = 240;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->set_threshold = 240;
        data->clear_threshold = 288;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_set(
    int unit)
{
    int dp_index;
    dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_init;
    int table_index = dnx_data_ingr_congestion_init_table_dp_global_dram_bdb_drop;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 4;
    table->info_get.key_size[0] = 4;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_ingr_congestion_init_table_dp_global_dram_bdb_drop");

    
    default_data = (dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->set_threshold = 0;
    default_data->clear_threshold = 0;
    
    for (dp_index = 0; dp_index < table->keys[0].size; dp_index++)
    {
        data = (dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_t *) dnxc_data_mgmt_table_data_get(unit, table, dp_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->set_threshold = 128;
        data->clear_threshold = 128;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->set_threshold = 144;
        data->clear_threshold = 144;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->set_threshold = 192;
        data->clear_threshold = 192;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->set_threshold = 224;
        data->clear_threshold = 224;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_init_equivalent_global_drop_set(
    int unit)
{
    int type_index;
    dnx_data_ingr_congestion_init_equivalent_global_drop_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_init;
    int table_index = dnx_data_ingr_congestion_init_table_equivalent_global_drop;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 3;
    table->info_get.key_size[0] = 3;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_ingr_congestion_init_equivalent_global_drop_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_ingr_congestion_init_table_equivalent_global_drop");

    
    default_data = (dnx_data_ingr_congestion_init_equivalent_global_drop_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->drop = 0;
    
    for (type_index = 0; type_index < table->keys[0].size; type_index++)
    {
        data = (dnx_data_ingr_congestion_init_equivalent_global_drop_t *) dnxc_data_mgmt_table_data_get(unit, table, type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_equivalent_global_drop_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES, 0);
        data->drop = dnx_data_ingr_congestion.init.dp_global_dram_bdb_drop_get(unit, 3)->set_threshold*32*1024*10/3;
    }
    if (DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_equivalent_global_drop_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS, 0);
        data->drop = dnx_data_ingr_congestion.init.dp_global_sram_buffer_drop_get(unit, 3)->set_threshold;
    }
    if (DNX_INGRESS_CONGESTION_RESOURCE_SRAM_PDS < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_init_equivalent_global_drop_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_PDS, 0);
        data->drop = dnx_data_ingr_congestion.init.dp_global_dram_bdb_drop_get(unit, 3)->set_threshold*4*4/3;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_ingr_congestion_dbal_admission_test_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_dbal;
    int define_index = dnx_data_ingr_congestion_dbal_define_admission_test_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 37;

    
    define->data = 37;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_dbal_dram_bdbs_th_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_dbal;
    int define_index = dnx_data_ingr_congestion_dbal_define_dram_bdbs_th_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_system_red.info.resource_get(unit, DNX_SYSTEM_RED_RESOURCE_DRAM_BDBS)->hw_resolution_nof_bits+1;

    
    define->data = dnx_data_system_red.info.resource_get(unit, DNX_SYSTEM_RED_RESOURCE_DRAM_BDBS)->hw_resolution_nof_bits+1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_dbal_sram_pdbs_th_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_dbal;
    int define_index = dnx_data_ingr_congestion_dbal_define_sram_pdbs_th_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_system_red.info.resource_get(unit, DNX_SYSTEM_RED_RESOURCE_SRAM_PDBS)->hw_resolution_nof_bits+1;

    
    define->data = dnx_data_system_red.info.resource_get(unit, DNX_SYSTEM_RED_RESOURCE_SRAM_PDBS)->hw_resolution_nof_bits+1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_dbal_sram_buffer_th_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_dbal;
    int define_index = dnx_data_ingr_congestion_dbal_define_sram_buffer_th_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_ingr_congestion.info.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS)->hw_resolution_nof_bits;

    
    define->data = dnx_data_ingr_congestion.info.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS)->hw_resolution_nof_bits;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_dbal_sram_buffer_free_th_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_dbal;
    int define_index = dnx_data_ingr_congestion_dbal_define_sram_buffer_free_th_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_ingr_congestion.dbal.sram_buffer_th_nof_bits_get(unit)+1;

    
    define->data = dnx_data_ingr_congestion.dbal.sram_buffer_th_nof_bits_get(unit)+1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_dbal_sram_pds_th_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_dbal;
    int define_index = dnx_data_ingr_congestion_dbal_define_sram_pds_th_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_ingr_congestion.info.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_PDS)->hw_resolution_nof_bits;

    
    define->data = dnx_data_ingr_congestion.info.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_PDS)->hw_resolution_nof_bits;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_dbal_total_bytes_th_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_dbal;
    int define_index = dnx_data_ingr_congestion_dbal_define_total_bytes_th_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_ingr_congestion.info.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES)->hw_resolution_nof_bits;

    
    define->data = dnx_data_ingr_congestion.info.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES)->hw_resolution_nof_bits;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_dbal_dram_bdbs_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_dbal;
    int define_index = dnx_data_ingr_congestion_dbal_define_dram_bdbs_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 18;

    
    define->data = 18;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_dbal_sram_pdbs_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_dbal;
    int define_index = dnx_data_ingr_congestion_dbal_define_sram_pdbs_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_dbal_sram_buffer_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_dbal;
    int define_index = dnx_data_ingr_congestion_dbal_define_sram_buffer_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 17;

    
    define->data = 17;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingr_congestion_dbal_sqm_debug_pkt_ctr_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_dbal;
    int define_index = dnx_data_ingr_congestion_dbal_define_sqm_debug_pkt_ctr_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 48;

    
    define->data = 48;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_ingr_congestion_dbal_admission_bits_mapping_set(
    int unit)
{
    int rjct_bit_index;
    dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_dbal;
    int table_index = dnx_data_ingr_congestion_dbal_table_admission_bits_mapping;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_INGRESS_CONGESTION_REJECT_BIT_NOF;
    table->info_get.key_size[0] = DNX_INGRESS_CONGESTION_REJECT_BIT_NOF;

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_ingr_congestion_dbal_admission_bits_mapping_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_ingr_congestion_dbal_table_admission_bits_mapping");

    
    default_data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->index = -1;
    
    for (rjct_bit_index = 0; rjct_bit_index < table->keys[0].size; rjct_bit_index++)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, rjct_bit_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_SRAM_PDS_TOTAL_SHARED < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_SRAM_PDS_TOTAL_SHARED, 0);
        data->index = 0;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_SRAM_PDS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_SRAM_PDS_MAX_SIZE, 0);
        data->index = 1;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_SRAM_BUFFERS_TOTAL_SHARED < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_SRAM_BUFFERS_TOTAL_SHARED, 0);
        data->index = 2;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_SRAM_BUFFERS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_SRAM_BUFFERS_MAX_SIZE, 0);
        data->index = 3;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_WORDS_TOTAL_SHARED < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_WORDS_TOTAL_SHARED, 0);
        data->index = 4;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_WORDS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_WORDS_MAX_SIZE, 0);
        data->index = 5;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_SYSTEM_RED < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_SYSTEM_RED, 0);
        data->index = 6;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_WRED < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_WRED, 0);
        data->index = 7;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_DRAM_BLOCK < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_DRAM_BLOCK, 0);
        data->index = 8;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_SRAM_PDS_TOTAL_SHARED < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_SRAM_PDS_TOTAL_SHARED, 0);
        data->index = 9;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_SRAM_PDS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_SRAM_PDS_MAX_SIZE, 0);
        data->index = 10;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_D_SRAM_PDS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_D_SRAM_PDS_MAX_SIZE, 0);
        data->index = 11;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_C_SRAM_PDS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_C_SRAM_PDS_MAX_SIZE, 0);
        data->index = 12;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_B_SRAM_PDS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_B_SRAM_PDS_MAX_SIZE, 0);
        data->index = 13;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_A_SRAM_PDS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_A_SRAM_PDS_MAX_SIZE, 0);
        data->index = 14;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_SRAM_BUFFERS_TOTAL_SHARED < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_SRAM_BUFFERS_TOTAL_SHARED, 0);
        data->index = 15;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_SRAM_BUFFERS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_SRAM_BUFFERS_MAX_SIZE, 0);
        data->index = 16;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_D_SRAM_BUFFERS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_D_SRAM_BUFFERS_MAX_SIZE, 0);
        data->index = 17;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_C_SRAM_BUFFERS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_C_SRAM_BUFFERS_MAX_SIZE, 0);
        data->index = 18;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_B_SRAM_BUFFERS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_B_SRAM_BUFFERS_MAX_SIZE, 0);
        data->index = 19;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_A_SRAM_BUFFERS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_A_SRAM_BUFFERS_MAX_SIZE, 0);
        data->index = 20;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_WORDS_TOTAL_SHARED < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_WORDS_TOTAL_SHARED, 0);
        data->index = 21;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_WORDS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_WORDS_MAX_SIZE, 0);
        data->index = 22;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_D_WORDS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_D_WORDS_MAX_SIZE, 0);
        data->index = 23;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_C_WORDS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_C_WORDS_MAX_SIZE, 0);
        data->index = 24;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_B_WORDS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_B_WORDS_MAX_SIZE, 0);
        data->index = 25;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_A_WORDS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_A_WORDS_MAX_SIZE, 0);
        data->index = 26;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_F_WORDS_WRED < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_F_WORDS_WRED, 0);
        data->index = 27;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_WORDS_WRED < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_WORDS_WRED, 0);
        data->index = 28;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_D_WORDS_WRED < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_D_WORDS_WRED, 0);
        data->index = 29;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_C_WORDS_WRED < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_C_WORDS_WRED, 0);
        data->index = 30;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_B_WORDS_WRED < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_B_WORDS_WRED, 0);
        data->index = 31;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_A_WORDS_WRED < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_A_WORDS_WRED, 0);
        data->index = 32;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_DRAM_BDBS_OCCUPANCY < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_DRAM_BDBS_OCCUPANCY, 0);
        data->index = 33;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_SRAM_BUFFERS_OCCUPANCY < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_SRAM_BUFFERS_OCCUPANCY, 0);
        data->index = 34;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_SRAM_PDS_OCCUPANCY < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_SRAM_PDS_OCCUPANCY, 0);
        data->index = 35;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_DP_LEVEL < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_DP_LEVEL, 0);
        data->index = 36;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_ingr_congestion_sram_buffer_drop_tresholds_set(
    int unit)
{
    int index_index;
    dnx_data_ingr_congestion_sram_buffer_drop_tresholds_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_sram_buffer;
    int table_index = dnx_data_ingr_congestion_sram_buffer_table_drop_tresholds;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 3;
    table->info_get.key_size[0] = 3;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_ingr_congestion_sram_buffer_drop_tresholds_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_ingr_congestion_sram_buffer_table_drop_tresholds");

    
    default_data = (dnx_data_ingr_congestion_sram_buffer_drop_tresholds_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->value = 0;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_ingr_congestion_sram_buffer_drop_tresholds_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_sram_buffer_drop_tresholds_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->value = 300;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_sram_buffer_drop_tresholds_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->value = 200;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_sram_buffer_drop_tresholds_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->value = 100;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
jr2_a0_data_ingr_congestion_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_ingr_congestion_submodule_config;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_ingr_congestion_config_define_guarantee_mode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_config_guarantee_mode_set;
    data_index = dnx_data_ingr_congestion_config_define_wred_packet_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_config_wred_packet_size_set;

    
    data_index = dnx_data_ingr_congestion_config_pp_port_by_reassembly_overwrite;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_ingr_congestion_config_pp_port_by_reassembly_overwrite_set;

    
    
    submodule_index = dnx_data_ingr_congestion_submodule_info;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_ingr_congestion_info_define_threshold_granularity;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_info_threshold_granularity_set;
    data_index = dnx_data_ingr_congestion_info_define_words_resolution;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_info_words_resolution_set;
    data_index = dnx_data_ingr_congestion_info_define_bytes_threshold_granularity;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_info_bytes_threshold_granularity_set;
    data_index = dnx_data_ingr_congestion_info_define_nof_dropped_reasons_cgm;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_info_nof_dropped_reasons_cgm_set;
    data_index = dnx_data_ingr_congestion_info_define_wred_max_gain;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_info_wred_max_gain_set;
    data_index = dnx_data_ingr_congestion_info_define_wred_granularity;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_info_wred_granularity_set;
    data_index = dnx_data_ingr_congestion_info_define_nof_pds_in_pdb;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_info_nof_pds_in_pdb_set;
    data_index = dnx_data_ingr_congestion_info_define_max_sram_pdbs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_info_max_sram_pdbs_set;
    data_index = dnx_data_ingr_congestion_info_define_max_dram_bdbs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_info_max_dram_bdbs_set;

    
    data_index = dnx_data_ingr_congestion_info_latency_based_admission;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_ingr_congestion_info_latency_based_admission_set;
    data_index = dnx_data_ingr_congestion_info_cgm_hw_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_ingr_congestion_info_cgm_hw_support_set;

    
    data_index = dnx_data_ingr_congestion_info_table_resource;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_ingr_congestion_info_resource_set;
    data_index = dnx_data_ingr_congestion_info_table_dp_free_res_presentage_drop;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_ingr_congestion_info_dp_free_res_presentage_drop_set;
    data_index = dnx_data_ingr_congestion_info_table_admission_preferences;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_ingr_congestion_info_admission_preferences_set;
    
    submodule_index = dnx_data_ingr_congestion_submodule_fadt_tail_drop;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_ingr_congestion_fadt_tail_drop_define_default_max_size_byte_threshold_for_ocb_only;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_fadt_tail_drop_default_max_size_byte_threshold_for_ocb_only_set;

    

    
    
    submodule_index = dnx_data_ingr_congestion_submodule_dram_bound;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_ingr_congestion_dram_bound_define_fadt_alpha_min;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_dram_bound_fadt_alpha_min_set;
    data_index = dnx_data_ingr_congestion_dram_bound_define_fadt_alpha_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_dram_bound_fadt_alpha_max_set;

    

    
    data_index = dnx_data_ingr_congestion_dram_bound_table_resource;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_ingr_congestion_dram_bound_resource_set;
    
    submodule_index = dnx_data_ingr_congestion_submodule_voq;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_ingr_congestion_voq_define_nof_rate_class;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_voq_nof_rate_class_set;
    data_index = dnx_data_ingr_congestion_voq_define_rate_class_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_voq_rate_class_nof_bits_set;
    data_index = dnx_data_ingr_congestion_voq_define_nof_compensation_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_voq_nof_compensation_profiles_set;
    data_index = dnx_data_ingr_congestion_voq_define_default_compensation;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_voq_default_compensation_set;
    data_index = dnx_data_ingr_congestion_voq_define_latency_bins;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_voq_latency_bins_set;
    data_index = dnx_data_ingr_congestion_voq_define_voq_congestion_notification_fifo_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_voq_voq_congestion_notification_fifo_size_set;

    

    
    
    submodule_index = dnx_data_ingr_congestion_submodule_vsq;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_ingr_congestion_vsq_define_vsq_rate_class_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_vsq_vsq_rate_class_nof_set;
    data_index = dnx_data_ingr_congestion_vsq_define_vsq_a_rate_class_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_vsq_vsq_a_rate_class_nof_set;
    data_index = dnx_data_ingr_congestion_vsq_define_vsq_a_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_vsq_vsq_a_nof_set;
    data_index = dnx_data_ingr_congestion_vsq_define_vsq_b_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_vsq_vsq_b_nof_set;
    data_index = dnx_data_ingr_congestion_vsq_define_vsq_c_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_vsq_vsq_c_nof_set;
    data_index = dnx_data_ingr_congestion_vsq_define_vsq_d_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_vsq_vsq_d_nof_set;
    data_index = dnx_data_ingr_congestion_vsq_define_vsq_e_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_vsq_vsq_e_nof_set;
    data_index = dnx_data_ingr_congestion_vsq_define_vsq_e_hw_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_vsq_vsq_e_hw_nof_set;
    data_index = dnx_data_ingr_congestion_vsq_define_vsq_f_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_vsq_vsq_f_nof_set;
    data_index = dnx_data_ingr_congestion_vsq_define_vsq_f_hw_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_vsq_vsq_f_hw_nof_set;
    data_index = dnx_data_ingr_congestion_vsq_define_nif_vsq_e_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_vsq_nif_vsq_e_nof_set;
    data_index = dnx_data_ingr_congestion_vsq_define_non_nif_vsq_f_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_vsq_non_nif_vsq_f_nof_set;
    data_index = dnx_data_ingr_congestion_vsq_define_pool_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_vsq_pool_nof_set;
    data_index = dnx_data_ingr_congestion_vsq_define_connection_class_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_vsq_connection_class_nof_set;
    data_index = dnx_data_ingr_congestion_vsq_define_tc_pg_profile_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_vsq_tc_pg_profile_nof_set;
    data_index = dnx_data_ingr_congestion_vsq_define_vsq_e_congestion_notification_fifo_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_vsq_vsq_e_congestion_notification_fifo_size_set;
    data_index = dnx_data_ingr_congestion_vsq_define_vsq_f_congestion_notification_fifo_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_vsq_vsq_f_congestion_notification_fifo_size_set;
    data_index = dnx_data_ingr_congestion_vsq_define_vsq_e_default;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_vsq_vsq_e_default_set;
    data_index = dnx_data_ingr_congestion_vsq_define_vsq_f_default;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_vsq_vsq_f_default_set;

    
    data_index = dnx_data_ingr_congestion_vsq_size_watermark_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_ingr_congestion_vsq_size_watermark_support_set;

    
    data_index = dnx_data_ingr_congestion_vsq_table_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_ingr_congestion_vsq_info_set;
    
    submodule_index = dnx_data_ingr_congestion_submodule_init;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_ingr_congestion_init_define_fifo_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_init_fifo_size_set;

    

    
    data_index = dnx_data_ingr_congestion_init_table_vsq_words_rjct_map;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_ingr_congestion_init_vsq_words_rjct_map_set;
    data_index = dnx_data_ingr_congestion_init_table_vsq_sram_rjct_map;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_ingr_congestion_init_vsq_sram_rjct_map_set;
    data_index = dnx_data_ingr_congestion_init_table_dp_global_sram_buffer_drop;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_ingr_congestion_init_dp_global_sram_buffer_drop_set;
    data_index = dnx_data_ingr_congestion_init_table_dp_global_sram_pdb_drop;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_ingr_congestion_init_dp_global_sram_pdb_drop_set;
    data_index = dnx_data_ingr_congestion_init_table_dp_global_dram_bdb_drop;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_ingr_congestion_init_dp_global_dram_bdb_drop_set;
    data_index = dnx_data_ingr_congestion_init_table_equivalent_global_drop;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_ingr_congestion_init_equivalent_global_drop_set;
    
    submodule_index = dnx_data_ingr_congestion_submodule_dbal;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_ingr_congestion_dbal_define_admission_test_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_dbal_admission_test_nof_set;
    data_index = dnx_data_ingr_congestion_dbal_define_dram_bdbs_th_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_dbal_dram_bdbs_th_nof_bits_set;
    data_index = dnx_data_ingr_congestion_dbal_define_sram_pdbs_th_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_dbal_sram_pdbs_th_nof_bits_set;
    data_index = dnx_data_ingr_congestion_dbal_define_sram_buffer_th_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_dbal_sram_buffer_th_nof_bits_set;
    data_index = dnx_data_ingr_congestion_dbal_define_sram_buffer_free_th_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_dbal_sram_buffer_free_th_nof_bits_set;
    data_index = dnx_data_ingr_congestion_dbal_define_sram_pds_th_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_dbal_sram_pds_th_nof_bits_set;
    data_index = dnx_data_ingr_congestion_dbal_define_total_bytes_th_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_dbal_total_bytes_th_nof_bits_set;
    data_index = dnx_data_ingr_congestion_dbal_define_dram_bdbs_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_dbal_dram_bdbs_nof_bits_set;
    data_index = dnx_data_ingr_congestion_dbal_define_sram_pdbs_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_dbal_sram_pdbs_nof_bits_set;
    data_index = dnx_data_ingr_congestion_dbal_define_sram_buffer_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_dbal_sram_buffer_nof_bits_set;
    data_index = dnx_data_ingr_congestion_dbal_define_sqm_debug_pkt_ctr_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingr_congestion_dbal_sqm_debug_pkt_ctr_nof_bits_set;

    

    
    data_index = dnx_data_ingr_congestion_dbal_table_admission_bits_mapping;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_ingr_congestion_dbal_admission_bits_mapping_set;
    
    submodule_index = dnx_data_ingr_congestion_submodule_sram_buffer;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_ingr_congestion_sram_buffer_table_drop_tresholds;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_ingr_congestion_sram_buffer_drop_tresholds_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

