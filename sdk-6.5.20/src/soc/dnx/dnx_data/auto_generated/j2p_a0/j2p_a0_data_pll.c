

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PLL

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_pll.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <soc/dnx/pll/pll.h>







static shr_error_e
j2p_a0_dnx_data_pll_general_ts_freq_lock_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_pll;
    int submodule_index = dnx_data_pll_submodule_general;
    int feature_index = dnx_data_pll_general_ts_freq_lock;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_PHY_1588_DPLL_FREQUENCY_LOCK;
    feature->property.doc = 
        "\n"
        "IEEE1588 DPLL mode\n"
        "Supported values: 0 - phase lock, 1 - frequency lock\n"
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_enable;
    feature->property.method_str = "enable";
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_pll_general_bs_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_pll;
    int submodule_index = dnx_data_pll_submodule_general;
    int feature_index = dnx_data_pll_general_bs_enable;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_BROADSYNC_ENABLE_CLK;
    feature->property.doc = 
        "\n"
        "Broadsync clock enable.\n"
        "Supported values: 0,1 (disable/enable)\n"
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_enable;
    feature->property.method_str = "enable";
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}




static shr_error_e
j2p_a0_dnx_data_pll_general_nif_pll_set(
    int unit)
{
    int pll_index_index;
    dnx_data_pll_general_nif_pll_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_pll;
    int submodule_index = dnx_data_pll_submodule_general;
    int table_index = dnx_data_pll_general_table_nif_pll;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "DNX_SERDES_REF_CLOCK_156_25";
    table->values[1].default_val = "DNX_SERDES_REF_CLOCK_156_25";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_pll_general_nif_pll_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_pll_general_table_nif_pll");

    
    default_data = (dnx_data_pll_general_nif_pll_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->in_freq = DNX_SERDES_REF_CLOCK_156_25;
    default_data->out_freq = DNX_SERDES_REF_CLOCK_156_25;
    
    for (pll_index_index = 0; pll_index_index < table->keys[0].size; pll_index_index++)
    {
        data = (dnx_data_pll_general_nif_pll_t *) dnxc_data_mgmt_table_data_get(unit, table, pll_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_pll_general_nif_pll_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->in_freq = DNX_SERDES_REF_CLOCK_156_25;
        data->out_freq = DNX_SERDES_REF_CLOCK_156_25;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_pll_general_fabric_pll_set(
    int unit)
{
    int pll_index_index;
    dnx_data_pll_general_fabric_pll_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_pll;
    int submodule_index = dnx_data_pll_submodule_general;
    int table_index = dnx_data_pll_general_table_fabric_pll;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "DNX_SERDES_REF_CLOCK_156_25";
    table->values[1].default_val = "DNX_SERDES_REF_CLOCK_156_25";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_pll_general_fabric_pll_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_pll_general_table_fabric_pll");

    
    default_data = (dnx_data_pll_general_fabric_pll_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->in_freq = DNX_SERDES_REF_CLOCK_156_25;
    default_data->out_freq = DNX_SERDES_REF_CLOCK_156_25;
    
    for (pll_index_index = 0; pll_index_index < table->keys[0].size; pll_index_index++)
    {
        data = (dnx_data_pll_general_fabric_pll_t *) dnxc_data_mgmt_table_data_get(unit, table, pll_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_pll_general_fabric_pll_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->in_freq = DNX_SERDES_REF_CLOCK_156_25;
        data->out_freq = DNX_SERDES_REF_CLOCK_156_25;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2p_a0_dnx_data_pll_pll1_nof_pll1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pll;
    int submodule_index = dnx_data_pll_submodule_pll1;
    int define_index = dnx_data_pll_pll1_define_nof_pll1;
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
j2p_a0_dnx_data_pll_pll3_nof_pll3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pll;
    int submodule_index = dnx_data_pll_submodule_pll3;
    int define_index = dnx_data_pll_pll3_define_nof_pll3;
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
j2p_a0_dnx_data_pll_pll4_nof_pll4_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pll;
    int submodule_index = dnx_data_pll_submodule_pll4;
    int define_index = dnx_data_pll_pll4_define_nof_pll4;
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
j2p_a0_dnx_data_pll_pll4_ts_pll4_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pll;
    int submodule_index = dnx_data_pll_submodule_pll4;
    int define_index = dnx_data_pll_pll4_define_ts_pll4_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_pll.pll4.config_get(unit, DNX_PLL4_TYPE_TS)->pll4_id;

    
    define->data = dnx_data_pll.pll4.config_get(unit, DNX_PLL4_TYPE_TS)->pll4_id;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_pll_pll4_bs_pll4_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pll;
    int submodule_index = dnx_data_pll_submodule_pll4;
    int define_index = dnx_data_pll_pll4_define_bs_pll4_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_pll.pll4.config_get(unit, DNX_PLL4_TYPE_BS)->pll4_id;

    
    define->data = dnx_data_pll.pll4.config_get(unit, DNX_PLL4_TYPE_BS)->pll4_id;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_pll_pll4_nif_pll4_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pll;
    int submodule_index = dnx_data_pll_submodule_pll4;
    int define_index = dnx_data_pll_pll4_define_nif_pll4_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_pll.pll4.config_get(unit, DNX_PLL4_TYPE_NIF)->pll4_id;

    
    define->data = dnx_data_pll.pll4.config_get(unit, DNX_PLL4_TYPE_NIF)->pll4_id;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_pll_pll4_fab_pll4_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pll;
    int submodule_index = dnx_data_pll_submodule_pll4;
    int define_index = dnx_data_pll_pll4_define_fab_pll4_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_pll.pll4.config_get(unit, DNX_PLL4_TYPE_FAB)->pll4_id;

    
    define->data = dnx_data_pll.pll4.config_get(unit, DNX_PLL4_TYPE_FAB)->pll4_id;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2p_a0_dnx_data_pll_pll4_config_set(
    int unit)
{
    int pll4_type_index;
    dnx_data_pll_pll4_config_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_pll;
    int submodule_index = dnx_data_pll_submodule_pll4;
    int table_index = dnx_data_pll_pll4_table_config;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_PLL4_TYPE_COUNT;
    table->info_get.key_size[0] = DNX_PLL4_TYPE_COUNT;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "-1";
    table->values[2].default_val = "-1";
    table->values[3].default_val = "-1";
    table->values[4].default_val = "-1";
    table->values[5].default_val = "-1";
    table->values[6].default_val = "-1";
    table->values[7].default_val = "-1";
    table->values[8].default_val = "-1";
    table->values[9].default_val = "-1";
    table->values[10].default_val = "-1";
    table->values[11].default_val = "-1";
    table->values[12].default_val = "-1";
    table->values[13].default_val = "-1";
    table->values[14].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_pll_pll4_config_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_pll_pll4_table_config");

    
    default_data = (dnx_data_pll_pll4_config_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->pll4_id = -1;
    default_data->valid = -1;
    default_data->ref_clk_src_sel = -1;
    default_data->ref_clk = -1;
    default_data->dll_ref_pdiv = -1;
    default_data->dll_post_en = -1;
    default_data->aux_post_en = -1;
    default_data->pdiv = -1;
    default_data->ndiv_int = -1;
    default_data->d2c_en = -1;
    default_data->ch0_mdiv = -1;
    default_data->ch1_mdiv = -1;
    default_data->ch2_mdiv = -1;
    default_data->ch3_mdiv = -1;
    default_data->ch6_mdiv = -1;
    
    for (pll4_type_index = 0; pll4_type_index < table->keys[0].size; pll4_type_index++)
    {
        data = (dnx_data_pll_pll4_config_t *) dnxc_data_mgmt_table_data_get(unit, table, pll4_type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_PLL4_TYPE_TS < table->keys[0].size)
    {
        data = (dnx_data_pll_pll4_config_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PLL4_TYPE_TS, 0);
        data->pll4_id = 0;
        data->valid = 1;
        data->ref_clk_src_sel = 1;
        data->ref_clk = 25;
        data->dll_ref_pdiv = 1;
        data->dll_post_en = 15;
        data->aux_post_en = 15;
        data->pdiv = 1;
        data->ndiv_int = 240;
        data->d2c_en = 1;
        data->ch0_mdiv = 6;
        data->ch6_mdiv = 3;
    }
    if (DNX_PLL4_TYPE_BS < table->keys[0].size)
    {
        data = (dnx_data_pll_pll4_config_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PLL4_TYPE_BS, 0);
        data->pll4_id = 1;
        data->valid = 1;
        data->ref_clk_src_sel = 0;
        data->ref_clk = 25;
        data->dll_ref_pdiv = 1;
        data->dll_post_en = 15;
        data->aux_post_en = 15;
        data->pdiv = 1;
        data->ndiv_int = 240;
        data->d2c_en = 0;
        data->ch0_mdiv = 200;
        data->ch6_mdiv = 60;
    }
    if (DNX_PLL4_TYPE_NIF < table->keys[0].size)
    {
        data = (dnx_data_pll_pll4_config_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PLL4_TYPE_NIF, 0);
        data->pll4_id = 2;
        data->valid = 1;
        data->ref_clk_src_sel = 1;
        data->ref_clk = 156;
        data->dll_ref_pdiv = 1;
        data->dll_post_en = 15;
        data->aux_post_en = 15;
        data->pdiv = 1;
        data->ndiv_int = 32;
        data->d2c_en = 1;
        data->ch0_mdiv = 50;
        data->ch1_mdiv = 10;
        data->ch2_mdiv = 25;
        data->ch3_mdiv = 3;
    }
    if (DNX_PLL4_TYPE_FAB < table->keys[0].size)
    {
        data = (dnx_data_pll_pll4_config_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PLL4_TYPE_FAB, 0);
        data->pll4_id = 3;
        data->valid = 1;
        data->ref_clk_src_sel = 1;
        data->ref_clk = 156;
        data->dll_ref_pdiv = 1;
        data->dll_post_en = 15;
        data->aux_post_en = 15;
        data->pdiv = 1;
        data->ndiv_int = 29;
        data->d2c_en = 1;
        data->ch0_mdiv = 5;
        data->ch1_mdiv = 29;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2p_a0_dnx_data_pll_synce_pll_present_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_pll;
    int submodule_index = dnx_data_pll_submodule_synce_pll;
    int feature_index = dnx_data_pll_synce_pll_present;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}





shr_error_e
j2p_a0_data_pll_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_pll;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_pll_submodule_general;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_pll_general_ts_freq_lock;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_pll_general_ts_freq_lock_set;
    data_index = dnx_data_pll_general_bs_enable;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_pll_general_bs_enable_set;

    
    data_index = dnx_data_pll_general_table_nif_pll;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_pll_general_nif_pll_set;
    data_index = dnx_data_pll_general_table_fabric_pll;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_pll_general_fabric_pll_set;
    
    submodule_index = dnx_data_pll_submodule_pll1;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_pll_pll1_define_nof_pll1;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_pll_pll1_nof_pll1_set;

    

    
    
    submodule_index = dnx_data_pll_submodule_pll3;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_pll_pll3_define_nof_pll3;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_pll_pll3_nof_pll3_set;

    

    
    
    submodule_index = dnx_data_pll_submodule_pll4;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_pll_pll4_define_nof_pll4;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_pll_pll4_nof_pll4_set;
    data_index = dnx_data_pll_pll4_define_ts_pll4_id;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_pll_pll4_ts_pll4_id_set;
    data_index = dnx_data_pll_pll4_define_bs_pll4_id;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_pll_pll4_bs_pll4_id_set;
    data_index = dnx_data_pll_pll4_define_nif_pll4_id;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_pll_pll4_nif_pll4_id_set;
    data_index = dnx_data_pll_pll4_define_fab_pll4_id;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_pll_pll4_fab_pll4_id_set;

    

    
    data_index = dnx_data_pll_pll4_table_config;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_pll_pll4_config_set;
    
    submodule_index = dnx_data_pll_submodule_synce_pll;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_pll_synce_pll_present;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_pll_synce_pll_present_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

