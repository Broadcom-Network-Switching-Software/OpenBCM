

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INSTRU

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_instru.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <bcm/instru.h>
#include <bcm_int/dnx/instru/instru.h>
#include <sal/limits.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_snif.h>
#include <shared/utilex/utilex_integer_arithmetic.h>







static shr_error_e
jr2_a0_dnx_data_instru_ipt_advanced_ipt_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_ipt;
    int feature_index = dnx_data_instru_ipt_advanced_ipt;
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
jr2_a0_dnx_data_instru_ipt_profile_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_ipt;
    int define_index = dnx_data_instru_ipt_define_profile_size;
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
jr2_a0_dnx_data_instru_ipt_metadata_bitmap_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_ipt;
    int define_index = dnx_data_instru_ipt_define_metadata_bitmap_size;
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
jr2_a0_dnx_data_instru_ipt_metadata_edit_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_ipt;
    int define_index = dnx_data_instru_ipt_define_metadata_edit_size;
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
jr2_a0_dnx_data_instru_ipt_node_id_padding_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_ipt;
    int define_index = dnx_data_instru_ipt_define_node_id_padding_size;
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
jr2_a0_dnx_data_instru_ipt_profile_info_set(
    int unit)
{
    int profile_id_index;
    dnx_data_instru_ipt_profile_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_ipt;
    int table_index = dnx_data_instru_ipt_table_profile_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 8;
    table->info_get.key_size[0] = 8;

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_instru_ipt_profile_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_instru_ipt_table_profile_info");

    
    default_data = (dnx_data_instru_ipt_profile_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->type = -1;
    
    for (profile_id_index = 0; profile_id_index < table->keys[0].size; profile_id_index++)
    {
        data = (dnx_data_instru_ipt_profile_info_t *) dnxc_data_mgmt_table_data_get(unit, table, profile_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_profile_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->type = bcmInstruIptNodeTypeTail;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_profile_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->type = bcmInstruIptNodeTypeIntOverVxlanIntermediate;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_profile_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->type = bcmInstruIptNodeTypeIntOverVxlanFirst;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_profile_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->type = bcmInstruIptNodeTypeIntOverVxlanLast;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_instru_ipt_metadata_set(
    int unit)
{
    int flag_id_index;
    dnx_data_instru_ipt_metadata_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_ipt;
    int table_index = dnx_data_instru_ipt_table_metadata;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 32;
    table->info_get.key_size[0] = 32;

    
    table->values[0].default_val = "SAL_UINT32_MAX";
    table->values[1].default_val = "SAL_UINT32_MAX";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_instru_ipt_metadata_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_instru_ipt_table_metadata");

    
    default_data = (dnx_data_instru_ipt_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->bit = SAL_UINT32_MAX;
    default_data->size = SAL_UINT32_MAX;
    
    for (flag_id_index = 0; flag_id_index < table->keys[0].size; flag_id_index++)
    {
        data = (dnx_data_instru_ipt_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, flag_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->bit = 0;
        data->size = 6;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->bit = 1;
        data->size = 2;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->bit = 2;
        data->size = 2;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->bit = 3;
        data->size = 4;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->bit = 4;
        data->size = 6;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->bit = 5;
        data->size = 2;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->bit = 6;
        data->size = 2;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->bit = 7;
        data->size = 2;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->bit = 8;
        data->size = 2;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_instru_ipt_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->bit = 3;
        data->size = 4;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_instru_sflow_max_nof_sflow_encaps_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_sflow;
    int define_index = dnx_data_instru_sflow_define_max_nof_sflow_encaps;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_snif.ingress.mirror_nof_profiles_get(unit);

    
    define->data = dnx_data_snif.ingress.mirror_nof_profiles_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_instru_sflow_nof_sflow_raw_entries_per_stack_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_sflow;
    int define_index = dnx_data_instru_sflow_define_nof_sflow_raw_entries_per_stack;
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
jr2_a0_dnx_data_instru_ifa_ingress_tod_feature_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_ifa;
    int define_index = dnx_data_instru_ifa_define_ingress_tod_feature;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_instru_ifa_max_lenght_check_for_ifa2_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_ifa;
    int define_index = dnx_data_instru_ifa_define_max_lenght_check_for_ifa2;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_instru_eventor_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_eventor;
    int feature_index = dnx_data_instru_eventor_is_supported;
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
jr2_a0_dnx_data_instru_eventor_rx_evpck_header_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_eventor;
    int define_index = dnx_data_instru_eventor_define_rx_evpck_header_size;
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
jr2_a0_dnx_data_instru_eventor_tx_builder_max_header_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_eventor;
    int define_index = dnx_data_instru_eventor_define_tx_builder_max_header_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 124;

    
    define->data = 124;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_instru_eventor_nof_contexts_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_eventor;
    int define_index = dnx_data_instru_eventor_define_nof_contexts;
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
jr2_a0_dnx_data_instru_eventor_nof_contexts_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_eventor;
    int define_index = dnx_data_instru_eventor_define_nof_contexts_bits;
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
jr2_a0_dnx_data_instru_eventor_nof_builders_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_eventor;
    int define_index = dnx_data_instru_eventor_define_nof_builders;
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
jr2_a0_dnx_data_instru_eventor_nof_builders_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_eventor;
    int define_index = dnx_data_instru_eventor_define_nof_builders_bits;
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
jr2_a0_dnx_data_instru_eventor_nof_sram_banks_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_eventor;
    int define_index = dnx_data_instru_eventor_define_nof_sram_banks;
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
jr2_a0_dnx_data_instru_eventor_sram_bank_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_eventor;
    int define_index = dnx_data_instru_eventor_define_sram_bank_bits;
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
jr2_a0_dnx_data_instru_eventor_sram_banks_full_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_eventor;
    int define_index = dnx_data_instru_eventor_define_sram_banks_full_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16 * 1024;

    
    define->data = 16 * 1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_instru_eventor_sram_banks_net_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_eventor;
    int define_index = dnx_data_instru_eventor_define_sram_banks_net_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 13 * 1024;

    
    define->data = 13 * 1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_instru_eventor_builder_max_buffer_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_eventor;
    int define_index = dnx_data_instru_eventor_define_builder_max_buffer_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1600;

    
    define->data = 1600;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_instru_eventor_rx_max_buffer_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_eventor;
    int define_index = dnx_data_instru_eventor_define_rx_max_buffer_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1600;

    
    define->data = 1600;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_instru_eventor_axi_sram_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_eventor;
    int define_index = dnx_data_instru_eventor_define_axi_sram_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xe8000000;

    
    define->data = 0xe8000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_instru_eventor_builder_max_timeout_ms_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_eventor;
    int define_index = dnx_data_instru_eventor_define_builder_max_timeout_ms;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = SAL_UINT32_MAX / 1000000;

    
    define->data = SAL_UINT32_MAX / 1000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_instru_eventor_sram_bank_mask_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_eventor;
    int define_index = dnx_data_instru_eventor_define_sram_bank_mask;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = utilex_power_of_2(dnx_data_instru.eventor.sram_bank_bits_get(unit)) - 1;

    
    define->data = utilex_power_of_2(dnx_data_instru.eventor.sram_bank_bits_get(unit)) - 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_instru_synced_counters_icgm_type_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_synced_counters;
    int define_index = dnx_data_instru_synced_counters_define_icgm_type;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = bcmInstruSyncedCountersTypeIcgm;

    
    define->data = bcmInstruSyncedCountersTypeIcgm;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_instru_synced_counters_nif_type_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_synced_counters;
    int define_index = dnx_data_instru_synced_counters_define_nif_type;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = bcmInstruSyncedCountersTypeNif;

    
    define->data = bcmInstruSyncedCountersTypeNif;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_instru_synced_counters_nof_types_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_synced_counters;
    int define_index = dnx_data_instru_synced_counters_define_nof_types;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = bcmInstruSyncedCountersTypeCount;

    
    define->data = bcmInstruSyncedCountersTypeCount;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_instru_synced_counters_interval_period_size_hw_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_synced_counters;
    int define_index = dnx_data_instru_synced_counters_define_interval_period_size_hw;
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
jr2_a0_dnx_data_instru_synced_counters_max_interval_period_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_synced_counters;
    int define_index = dnx_data_instru_synced_counters_define_max_interval_period_size;
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
jr2_a0_dnx_data_instru_synced_counters_max_nof_intervals_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_synced_counters;
    int define_index = dnx_data_instru_synced_counters_define_max_nof_intervals;
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
jr2_a0_dnx_data_instru_synced_counters_cgm_write_counter_wrap_around_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_synced_counters;
    int define_index = dnx_data_instru_synced_counters_define_cgm_write_counter_wrap_around;
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
jr2_a0_dnx_data_instru_synced_counters_bytes_in_word_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_synced_counters;
    int define_index = dnx_data_instru_synced_counters_define_bytes_in_word;
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
jr2_a0_dnx_data_instru_synced_counters_counters_info_set(
    int unit)
{
    int entry_index_index;
    int source_type_index;
    dnx_data_instru_synced_counters_counters_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_synced_counters;
    int table_index = dnx_data_instru_synced_counters_table_counters_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = BCM_INSTRU_SYNCED_COUNTERS_STATS_MAX;
    table->info_get.key_size[0] = BCM_INSTRU_SYNCED_COUNTERS_STATS_MAX;
    table->keys[1].size = bcmInstruSyncedCountersTypeCount;
    table->info_get.key_size[1] = bcmInstruSyncedCountersTypeCount;

    
    table->values[0].default_val = "FALSE";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "DBAL_FIELD_EMPTY";
    table->values[4].default_val = "FALSE";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_instru_synced_counters_counters_info_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_instru_synced_counters_table_counters_info");

    
    default_data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid = FALSE;
    default_data->bcm_counter_type = 0;
    default_data->counter_index = 0;
    default_data->dbal_field = DBAL_FIELD_EMPTY;
    default_data->words_to_bytes_conv_needed = FALSE;
    
    for (entry_index_index = 0; entry_index_index < table->keys[0].size; entry_index_index++)
    {
        for (source_type_index = 0; source_type_index < table->keys[1].size; source_type_index++)
        {
            data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, entry_index_index, source_type_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (0 < table->keys[0].size && bcmInstruSyncedCountersTypeIcgm < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, bcmInstruSyncedCountersTypeIcgm);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersIcgmTypeMinFreeOcbBuffers;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_OCB_BUFFERS;
    }
    if (1 < table->keys[0].size && bcmInstruSyncedCountersTypeIcgm < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, bcmInstruSyncedCountersTypeIcgm);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersIcgmTypeMinFreeOcbPDBs;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_OCB_PDBS;
    }
    if (2 < table->keys[0].size && bcmInstruSyncedCountersTypeIcgm < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, bcmInstruSyncedCountersTypeIcgm);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersIcgmTypeMinFreeDramBDBs;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_DRAM_BDBS;
    }
    if (3 < table->keys[0].size && bcmInstruSyncedCountersTypeIcgm < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, bcmInstruSyncedCountersTypeIcgm);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersIcgmTypeMinFreeVoqSharedBytes;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VOQ_SHARED_WORDS;
        data->words_to_bytes_conv_needed = TRUE;
    }
    if (4 < table->keys[0].size && bcmInstruSyncedCountersTypeIcgm < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, bcmInstruSyncedCountersTypeIcgm);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersIcgmTypeMinFreeVoqOcbSharedBuffers;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VOQ_OCBS_SHARED_BUFFERS;
    }
    if (5 < table->keys[0].size && bcmInstruSyncedCountersTypeIcgm < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, bcmInstruSyncedCountersTypeIcgm);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersIcgmTypeMinFreeVoqOcbSharedPDs;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VOQ_OCBS_SHARED_PDS;
    }
    if (6 < table->keys[0].size && bcmInstruSyncedCountersTypeIcgm < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, bcmInstruSyncedCountersTypeIcgm);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersIcgmTypeMinFreeVsqHeadroomBytes;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_HEADROOM_WORDS;
        data->words_to_bytes_conv_needed = TRUE;
    }
    if (7 < table->keys[0].size && bcmInstruSyncedCountersTypeIcgm < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, bcmInstruSyncedCountersTypeIcgm);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersIcgmTypeMinFreeVsqSharedPoolIndexBytes;
        data->counter_index = 0;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_SHARED_POOL_INDEX_0_WORDS;
        data->words_to_bytes_conv_needed = TRUE;
    }
    if (8 < table->keys[0].size && bcmInstruSyncedCountersTypeIcgm < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, bcmInstruSyncedCountersTypeIcgm);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersIcgmTypeMinFreeVsqSharedPoolIndexBytes;
        data->counter_index = 1;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_SHARED_POOL_INDEX_1_WORDS;
        data->words_to_bytes_conv_needed = TRUE;
    }
    if (9 < table->keys[0].size && bcmInstruSyncedCountersTypeIcgm < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, bcmInstruSyncedCountersTypeIcgm);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersIcgmTypeMinFreeVsqOcbHeadroomBuffers;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_OCB_HEADROOM_BUFFERS;
    }
    if (10 < table->keys[0].size && bcmInstruSyncedCountersTypeIcgm < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, bcmInstruSyncedCountersTypeIcgm);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersIcgmTypeMinFreeVsqOcbSharedPoolIndexBuffers;
        data->counter_index = 0;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_OCB_SHARED_POOL_INDEX_0_BUFFERS;
    }
    if (11 < table->keys[0].size && bcmInstruSyncedCountersTypeIcgm < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, bcmInstruSyncedCountersTypeIcgm);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersIcgmTypeMinFreeVsqOcbSharedPoolIndexBuffers;
        data->counter_index = 1;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_OCB_SHARED_POOL_INDEX_1_BUFFERS;
    }
    if (12 < table->keys[0].size && bcmInstruSyncedCountersTypeIcgm < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, bcmInstruSyncedCountersTypeIcgm);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersIcgmTypeMinFreeVsqOcbHeadroomPDBs;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_OCB_HEADROOM_PDBS;
    }
    if (13 < table->keys[0].size && bcmInstruSyncedCountersTypeIcgm < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, bcmInstruSyncedCountersTypeIcgm);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersIcgmTypeMinFreeVsqOcbSharedPoolIndexPDBs;
        data->counter_index = 0;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_SHARED_POOL_INDEX_0_PDBS;
    }
    if (14 < table->keys[0].size && bcmInstruSyncedCountersTypeIcgm < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, bcmInstruSyncedCountersTypeIcgm);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersIcgmTypeMinFreeVsqOcbSharedPoolIndexPDBs;
        data->counter_index = 1;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_SHARED_POOL_INDEX_1_PDBS;
    }
    if (15 < table->keys[0].size && bcmInstruSyncedCountersTypeIcgm < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, bcmInstruSyncedCountersTypeIcgm);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersIcgmTypeEnqueuePackets;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_ENQUEUE_PACKETS;
    }
    if (16 < table->keys[0].size && bcmInstruSyncedCountersTypeIcgm < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, bcmInstruSyncedCountersTypeIcgm);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersIcgmTypeEnqueueBytes;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_ENQUEUE_BYTES;
    }
    if (17 < table->keys[0].size && bcmInstruSyncedCountersTypeIcgm < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, bcmInstruSyncedCountersTypeIcgm);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersIcgmTypeRejectPackets;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_REJECT_PACKETS;
    }
    if (18 < table->keys[0].size && bcmInstruSyncedCountersTypeIcgm < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, bcmInstruSyncedCountersTypeIcgm);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersIcgmTypeRejectBytes;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_REJECT_BYTES;
    }
    if (19 < table->keys[0].size && bcmInstruSyncedCountersTypeIcgm < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, bcmInstruSyncedCountersTypeIcgm);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersIcgmTypeDequeuePackets;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_DEQUEUE_PACKETS;
    }
    if (20 < table->keys[0].size && bcmInstruSyncedCountersTypeIcgm < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, bcmInstruSyncedCountersTypeIcgm);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersIcgmTypeDequeueBytes;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_DEQUEUE_BYTES;
    }
    if (0 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypeReceivedPackets;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PKT_RECEIVED;
    }
    if (1 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypeReceivedBytes;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_BYTES_RECEIVED;
    }
    if (2 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypeDroppedTdmPackets;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_TDM;
    }
    if (3 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypeDroppedTdmBytes;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_TDM;
    }
    if (4 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypeDroppedPriorityIndexPackets;
        data->counter_index = 0;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_0;
    }
    if (5 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypeDroppedPriorityIndexBytes;
        data->counter_index = 0;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_0;
    }
    if (6 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypeDroppedPriorityIndexPackets;
        data->counter_index = 1;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_1;
    }
    if (7 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypeDroppedPriorityIndexBytes;
        data->counter_index = 1;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_1;
    }
    if (8 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypeDroppedPriorityIndexPackets;
        data->counter_index = 2;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_2;
    }
    if (9 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypeDroppedPriorityIndexBytes;
        data->counter_index = 2;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_2;
    }
    if (10 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypeDroppedPriorityIndexPackets;
        data->counter_index = 3;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_3;
    }
    if (11 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypeDroppedPriorityIndexBytes;
        data->counter_index = 3;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_3;
    }
    if (12 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypePfcIndexNofOnTransitions;
        data->counter_index = 0;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PFC_0_NOF_ON_TRANSITIONS;
    }
    if (13 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypePfcIndexMaxClocksDuration;
        data->counter_index = 0;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PFC_0_MAX_CLOCKS_DURATION;
    }
    if (14 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypePfcIndexOnClockCyclesDuration;
        data->counter_index = 0;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PFC_0_ON_CLOCK_CYCLES_DURATION;
    }
    if (15 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypePfcIndexNofOnTransitions;
        data->counter_index = 1;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PFC_1_NOF_ON_TRANSITIONS;
    }
    if (16 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypePfcIndexMaxClocksDuration;
        data->counter_index = 1;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PFC_1_MAX_CLOCKS_DURATION;
    }
    if (17 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypePfcIndexOnClockCyclesDuration;
        data->counter_index = 1;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PFC_1_ON_CLOCK_CYCLES_DURATION;
    }
    if (18 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypePfcIndexNofOnTransitions;
        data->counter_index = 2;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PFC_2_NOF_ON_TRANSITIONS;
    }
    if (19 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypePfcIndexMaxClocksDuration;
        data->counter_index = 2;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PFC_2_MAX_CLOCKS_DURATION;
    }
    if (20 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypePfcIndexOnClockCyclesDuration;
        data->counter_index = 2;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PFC_2_ON_CLOCK_CYCLES_DURATION;
    }
    if (21 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypePfcIndexNofOnTransitions;
        data->counter_index = 3;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PFC_3_NOF_ON_TRANSITIONS;
    }
    if (22 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypePfcIndexMaxClocksDuration;
        data->counter_index = 3;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PFC_3_MAX_CLOCKS_DURATION;
    }
    if (23 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypePfcIndexOnClockCyclesDuration;
        data->counter_index = 3;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PFC_3_ON_CLOCK_CYCLES_DURATION;
    }
    if (24 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypePfcIndexNofOnTransitions;
        data->counter_index = 4;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PFC_4_NOF_ON_TRANSITIONS;
    }
    if (25 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypePfcIndexMaxClocksDuration;
        data->counter_index = 4;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PFC_4_MAX_CLOCKS_DURATION;
    }
    if (26 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypePfcIndexOnClockCyclesDuration;
        data->counter_index = 4;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PFC_4_ON_CLOCK_CYCLES_DURATION;
    }
    if (27 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypePfcIndexNofOnTransitions;
        data->counter_index = 5;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PFC_5_NOF_ON_TRANSITIONS;
    }
    if (28 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 28, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypePfcIndexMaxClocksDuration;
        data->counter_index = 5;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PFC_5_MAX_CLOCKS_DURATION;
    }
    if (29 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 29, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypePfcIndexOnClockCyclesDuration;
        data->counter_index = 5;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PFC_5_ON_CLOCK_CYCLES_DURATION;
    }
    if (30 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 30, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypePfcIndexNofOnTransitions;
        data->counter_index = 6;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PFC_6_NOF_ON_TRANSITIONS;
    }
    if (31 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 31, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypePfcIndexMaxClocksDuration;
        data->counter_index = 6;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PFC_6_MAX_CLOCKS_DURATION;
    }
    if (32 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 32, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypePfcIndexOnClockCyclesDuration;
        data->counter_index = 6;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PFC_6_ON_CLOCK_CYCLES_DURATION;
    }
    if (33 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 33, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypePfcIndexNofOnTransitions;
        data->counter_index = 7;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PFC_7_NOF_ON_TRANSITIONS;
    }
    if (34 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 34, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypePfcIndexMaxClocksDuration;
        data->counter_index = 7;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PFC_7_MAX_CLOCKS_DURATION;
    }
    if (35 < table->keys[0].size && bcmInstruSyncedCountersTypeNif < table->keys[1].size)
    {
        data = (dnx_data_instru_synced_counters_counters_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 35, bcmInstruSyncedCountersTypeNif);
        data->valid = TRUE;
        data->bcm_counter_type = bcmInstruSyncedCountersNifTypePfcIndexOnClockCyclesDuration;
        data->counter_index = 7;
        data->dbal_field = DBAL_FIELD_SYNCED_COUNTERS_PFC_7_ON_CLOCK_CYCLES_DURATION;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
jr2_a0_data_instru_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_instru;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_instru_submodule_ipt;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_instru_ipt_define_profile_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_ipt_profile_size_set;
    data_index = dnx_data_instru_ipt_define_metadata_bitmap_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_ipt_metadata_bitmap_size_set;
    data_index = dnx_data_instru_ipt_define_metadata_edit_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_ipt_metadata_edit_size_set;
    data_index = dnx_data_instru_ipt_define_node_id_padding_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_ipt_node_id_padding_size_set;

    
    data_index = dnx_data_instru_ipt_advanced_ipt;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_instru_ipt_advanced_ipt_set;

    
    data_index = dnx_data_instru_ipt_table_profile_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_instru_ipt_profile_info_set;
    data_index = dnx_data_instru_ipt_table_metadata;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_instru_ipt_metadata_set;
    
    submodule_index = dnx_data_instru_submodule_sflow;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_instru_sflow_define_max_nof_sflow_encaps;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_sflow_max_nof_sflow_encaps_set;
    data_index = dnx_data_instru_sflow_define_nof_sflow_raw_entries_per_stack;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_sflow_nof_sflow_raw_entries_per_stack_set;

    

    
    
    submodule_index = dnx_data_instru_submodule_ifa;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_instru_ifa_define_ingress_tod_feature;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_ifa_ingress_tod_feature_set;
    data_index = dnx_data_instru_ifa_define_max_lenght_check_for_ifa2;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_ifa_max_lenght_check_for_ifa2_set;

    

    
    
    submodule_index = dnx_data_instru_submodule_eventor;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_instru_eventor_define_rx_evpck_header_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_eventor_rx_evpck_header_size_set;
    data_index = dnx_data_instru_eventor_define_tx_builder_max_header_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_eventor_tx_builder_max_header_size_set;
    data_index = dnx_data_instru_eventor_define_nof_contexts;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_eventor_nof_contexts_set;
    data_index = dnx_data_instru_eventor_define_nof_contexts_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_eventor_nof_contexts_bits_set;
    data_index = dnx_data_instru_eventor_define_nof_builders;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_eventor_nof_builders_set;
    data_index = dnx_data_instru_eventor_define_nof_builders_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_eventor_nof_builders_bits_set;
    data_index = dnx_data_instru_eventor_define_nof_sram_banks;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_eventor_nof_sram_banks_set;
    data_index = dnx_data_instru_eventor_define_sram_bank_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_eventor_sram_bank_bits_set;
    data_index = dnx_data_instru_eventor_define_sram_banks_full_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_eventor_sram_banks_full_size_set;
    data_index = dnx_data_instru_eventor_define_sram_banks_net_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_eventor_sram_banks_net_size_set;
    data_index = dnx_data_instru_eventor_define_builder_max_buffer_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_eventor_builder_max_buffer_size_set;
    data_index = dnx_data_instru_eventor_define_rx_max_buffer_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_eventor_rx_max_buffer_size_set;
    data_index = dnx_data_instru_eventor_define_axi_sram_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_eventor_axi_sram_offset_set;
    data_index = dnx_data_instru_eventor_define_builder_max_timeout_ms;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_eventor_builder_max_timeout_ms_set;
    data_index = dnx_data_instru_eventor_define_sram_bank_mask;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_eventor_sram_bank_mask_set;

    
    data_index = dnx_data_instru_eventor_is_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_instru_eventor_is_supported_set;

    
    
    submodule_index = dnx_data_instru_submodule_synced_counters;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_instru_synced_counters_define_icgm_type;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_synced_counters_icgm_type_set;
    data_index = dnx_data_instru_synced_counters_define_nif_type;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_synced_counters_nif_type_set;
    data_index = dnx_data_instru_synced_counters_define_nof_types;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_synced_counters_nof_types_set;
    data_index = dnx_data_instru_synced_counters_define_interval_period_size_hw;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_synced_counters_interval_period_size_hw_set;
    data_index = dnx_data_instru_synced_counters_define_max_interval_period_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_synced_counters_max_interval_period_size_set;
    data_index = dnx_data_instru_synced_counters_define_max_nof_intervals;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_synced_counters_max_nof_intervals_set;
    data_index = dnx_data_instru_synced_counters_define_cgm_write_counter_wrap_around;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_synced_counters_cgm_write_counter_wrap_around_set;
    data_index = dnx_data_instru_synced_counters_define_bytes_in_word;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_instru_synced_counters_bytes_in_word_set;

    

    
    data_index = dnx_data_instru_synced_counters_table_counters_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_instru_synced_counters_counters_info_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

