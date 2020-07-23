

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
#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>
#include <soc/dnx/dbal/dbal.h>







static shr_error_e
j2c_a0_dnx_data_ingr_congestion_info_soft_dram_block_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_info;
    int feature_index = dnx_data_ingr_congestion_info_soft_dram_block_supported;
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
j2c_a0_dnx_data_ingr_congestion_info_latency_based_admission_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_info;
    int feature_index = dnx_data_ingr_congestion_info_latency_based_admission;
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
j2c_a0_dnx_data_ingr_congestion_info_latency_based_admission_reject_flow_profile_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_info;
    int define_index = dnx_data_ingr_congestion_info_define_latency_based_admission_reject_flow_profile_value;
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
j2c_a0_dnx_data_ingr_congestion_info_latency_based_admission_max_supported_profile_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_info;
    int define_index = dnx_data_ingr_congestion_info_define_latency_based_admission_max_supported_profile;
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
j2c_a0_dnx_data_ingr_congestion_vsq_size_watermark_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_vsq;
    int feature_index = dnx_data_ingr_congestion_vsq_size_watermark_support;
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
j2c_a0_dnx_data_ingr_congestion_vsq_vsq_e_hw_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_vsq;
    int define_index = dnx_data_ingr_congestion_vsq_define_vsq_e_hw_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 128+16+4;

    
    define->data = 128+16+4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_ingr_congestion_vsq_vsq_f_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_vsq;
    int define_index = dnx_data_ingr_congestion_vsq_define_vsq_f_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_ingr_congestion.vsq.vsq_f_hw_nof_get(unit);

    
    define->data = dnx_data_ingr_congestion.vsq.vsq_f_hw_nof_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_ingr_congestion_vsq_vsq_f_hw_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_vsq;
    int define_index = dnx_data_ingr_congestion_vsq_define_vsq_f_hw_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1024;

    
    define->data = 1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_ingr_congestion_vsq_nif_vsq_e_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_vsq;
    int define_index = dnx_data_ingr_congestion_vsq_define_nif_vsq_e_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 144;

    
    define->data = 144;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_ingr_congestion_vsq_vsq_f_default_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_vsq;
    int define_index = dnx_data_ingr_congestion_vsq_define_vsq_f_default;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_ingr_congestion.vsq.vsq_f_nof_get(unit)-1;

    
    define->data = dnx_data_ingr_congestion.vsq.vsq_f_nof_get(unit)-1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "reduced_priority_groups_lane";
    define->property.doc = 
        "\n"
        "Configure the default VSQ-F by device lane supporting reduced number of priority groups.\n"
        "Default VSQ-F = (reduced lane * BCM_COSQ_VSQ_NOF_PG) + BCM_COSQ_VSQ_NOF_PG - 1\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_custom;
    define->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnx_data_property_ingr_congestion_vsq_vsq_f_default_read(unit, (uint32 *) &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2c_a0_dnx_data_ingr_congestion_dbal_admission_test_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_dbal;
    int define_index = dnx_data_ingr_congestion_dbal_define_admission_test_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 38;

    
    define->data = 38;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2c_a0_dnx_data_ingr_congestion_dbal_admission_bits_mapping_set(
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
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_LATENCY < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_LATENCY, 0);
        data->index = 7;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_WRED < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_WRED, 0);
        data->index = 8;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_DRAM_BLOCK < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VOQ_DRAM_BLOCK, 0);
        data->index = 9;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_SRAM_PDS_TOTAL_SHARED < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_SRAM_PDS_TOTAL_SHARED, 0);
        data->index = 10;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_SRAM_PDS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_SRAM_PDS_MAX_SIZE, 0);
        data->index = 11;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_D_SRAM_PDS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_D_SRAM_PDS_MAX_SIZE, 0);
        data->index = 12;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_C_SRAM_PDS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_C_SRAM_PDS_MAX_SIZE, 0);
        data->index = 13;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_B_SRAM_PDS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_B_SRAM_PDS_MAX_SIZE, 0);
        data->index = 14;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_A_SRAM_PDS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_A_SRAM_PDS_MAX_SIZE, 0);
        data->index = 15;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_SRAM_BUFFERS_TOTAL_SHARED < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_SRAM_BUFFERS_TOTAL_SHARED, 0);
        data->index = 16;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_SRAM_BUFFERS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_SRAM_BUFFERS_MAX_SIZE, 0);
        data->index = 17;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_D_SRAM_BUFFERS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_D_SRAM_BUFFERS_MAX_SIZE, 0);
        data->index = 18;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_C_SRAM_BUFFERS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_C_SRAM_BUFFERS_MAX_SIZE, 0);
        data->index = 19;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_B_SRAM_BUFFERS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_B_SRAM_BUFFERS_MAX_SIZE, 0);
        data->index = 20;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_A_SRAM_BUFFERS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_A_SRAM_BUFFERS_MAX_SIZE, 0);
        data->index = 21;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_WORDS_TOTAL_SHARED < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_WORDS_TOTAL_SHARED, 0);
        data->index = 22;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_WORDS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_F_WORDS_MAX_SIZE, 0);
        data->index = 23;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_D_WORDS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_D_WORDS_MAX_SIZE, 0);
        data->index = 24;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_C_WORDS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_C_WORDS_MAX_SIZE, 0);
        data->index = 25;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_B_WORDS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_B_WORDS_MAX_SIZE, 0);
        data->index = 26;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_A_WORDS_MAX_SIZE < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_A_WORDS_MAX_SIZE, 0);
        data->index = 27;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_F_WORDS_WRED < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_F_WORDS_WRED, 0);
        data->index = 28;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_WORDS_WRED < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_E_WORDS_WRED, 0);
        data->index = 29;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_D_WORDS_WRED < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_D_WORDS_WRED, 0);
        data->index = 30;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_C_WORDS_WRED < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_C_WORDS_WRED, 0);
        data->index = 31;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_B_WORDS_WRED < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_B_WORDS_WRED, 0);
        data->index = 32;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_A_WORDS_WRED < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_VSQ_A_WORDS_WRED, 0);
        data->index = 33;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_DRAM_BDBS_OCCUPANCY < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_DRAM_BDBS_OCCUPANCY, 0);
        data->index = 34;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_SRAM_BUFFERS_OCCUPANCY < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_SRAM_BUFFERS_OCCUPANCY, 0);
        data->index = 35;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_SRAM_PDS_OCCUPANCY < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_SRAM_PDS_OCCUPANCY, 0);
        data->index = 36;
    }
    if (DNX_INGRESS_CONGESTION_REJECT_BIT_DP_LEVEL < table->keys[0].size)
    {
        data = (dnx_data_ingr_congestion_dbal_admission_bits_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_INGRESS_CONGESTION_REJECT_BIT_DP_LEVEL, 0);
        data->index = 37;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2c_a0_dnx_data_ingr_congestion_mirror_on_drop_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_mirror_on_drop;
    int feature_index = dnx_data_ingr_congestion_mirror_on_drop_is_supported;
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
j2c_a0_dnx_data_ingr_congestion_mirror_on_drop_nof_groups_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_mirror_on_drop;
    int define_index = dnx_data_ingr_congestion_mirror_on_drop_define_nof_groups;
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
j2c_a0_dnx_data_ingr_congestion_mirror_on_drop_aging_clocks_resolution_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_congestion;
    int submodule_index = dnx_data_ingr_congestion_submodule_mirror_on_drop;
    int define_index = dnx_data_ingr_congestion_mirror_on_drop_define_aging_clocks_resolution;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1024;

    
    define->data = 1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
j2c_a0_data_ingr_congestion_attach(
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
    
    submodule_index = dnx_data_ingr_congestion_submodule_info;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_ingr_congestion_info_define_latency_based_admission_reject_flow_profile_value;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_ingr_congestion_info_latency_based_admission_reject_flow_profile_value_set;
    data_index = dnx_data_ingr_congestion_info_define_latency_based_admission_max_supported_profile;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_ingr_congestion_info_latency_based_admission_max_supported_profile_set;

    
    data_index = dnx_data_ingr_congestion_info_soft_dram_block_supported;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_ingr_congestion_info_soft_dram_block_supported_set;
    data_index = dnx_data_ingr_congestion_info_latency_based_admission;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_ingr_congestion_info_latency_based_admission_set;

    
    
    submodule_index = dnx_data_ingr_congestion_submodule_vsq;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_ingr_congestion_vsq_define_vsq_e_hw_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_ingr_congestion_vsq_vsq_e_hw_nof_set;
    data_index = dnx_data_ingr_congestion_vsq_define_vsq_f_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_ingr_congestion_vsq_vsq_f_nof_set;
    data_index = dnx_data_ingr_congestion_vsq_define_vsq_f_hw_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_ingr_congestion_vsq_vsq_f_hw_nof_set;
    data_index = dnx_data_ingr_congestion_vsq_define_nif_vsq_e_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_ingr_congestion_vsq_nif_vsq_e_nof_set;
    data_index = dnx_data_ingr_congestion_vsq_define_vsq_f_default;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_ingr_congestion_vsq_vsq_f_default_set;

    
    data_index = dnx_data_ingr_congestion_vsq_size_watermark_support;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_ingr_congestion_vsq_size_watermark_support_set;

    
    
    submodule_index = dnx_data_ingr_congestion_submodule_dbal;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_ingr_congestion_dbal_define_admission_test_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_ingr_congestion_dbal_admission_test_nof_set;

    

    
    data_index = dnx_data_ingr_congestion_dbal_table_admission_bits_mapping;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_ingr_congestion_dbal_admission_bits_mapping_set;
    
    submodule_index = dnx_data_ingr_congestion_submodule_mirror_on_drop;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_ingr_congestion_mirror_on_drop_define_nof_groups;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_ingr_congestion_mirror_on_drop_nof_groups_set;
    data_index = dnx_data_ingr_congestion_mirror_on_drop_define_aging_clocks_resolution;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_ingr_congestion_mirror_on_drop_aging_clocks_resolution_set;

    
    data_index = dnx_data_ingr_congestion_mirror_on_drop_is_supported;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_ingr_congestion_mirror_on_drop_is_supported_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

