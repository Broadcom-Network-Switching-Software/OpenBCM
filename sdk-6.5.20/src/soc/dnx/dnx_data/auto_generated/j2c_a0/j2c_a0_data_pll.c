

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
j2c_a0_dnx_data_pll_general_ts_freq_lock_set(
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
j2c_a0_dnx_data_pll_general_bs_enable_set(
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
j2c_a0_dnx_data_pll_general_nif_pll_set(
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
j2c_a0_dnx_data_pll_general_fabric_pll_set(
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
j2c_a0_dnx_data_pll_pll1_ts_phase_initial_hi_config_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_pll;
    int submodule_index = dnx_data_pll_submodule_pll1;
    int feature_index = dnx_data_pll_pll1_ts_phase_initial_hi_config;
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
j2c_a0_dnx_data_pll_pll1_ts_refclk_source_internal_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_pll;
    int submodule_index = dnx_data_pll_submodule_pll1;
    int feature_index = dnx_data_pll_pll1_ts_refclk_source_internal;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_CUSTOM_FEATURE;
    feature->property.doc = 
        "\n"
        "Determine if the ts pll gets the reference clock from external 25Mhz clock or internal clock.\n"
        "Can be 0 and 1.\n"
        "Value 1 for internal reference clock.\n"
        "Default: 0\n"
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_suffix_enable;
    feature->property.method_str = "suffix_enable";
    feature->property.suffix = "ts_pll_internal_clock_reference";
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2c_a0_dnx_data_pll_pll1_nof_pll1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pll;
    int submodule_index = dnx_data_pll_submodule_pll1;
    int define_index = dnx_data_pll_pll1_define_nof_pll1;
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
j2c_a0_dnx_data_pll_pll1_ts_phase_initial_lo_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pll;
    int submodule_index = dnx_data_pll_submodule_pll1;
    int define_index = dnx_data_pll_pll1_define_ts_phase_initial_lo;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x00000000;

    
    define->data = 0x00000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_PHY_1588_DPLL_PHASE_INITIAL_LO;
    define->property.doc = 
        "\n"
        "Initial phase values for the IEEE1588 DPLL, lower 32 bits\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 4294967295;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_pll_pll1_ts_phase_initial_hi_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pll;
    int submodule_index = dnx_data_pll_submodule_pll1;
    int define_index = dnx_data_pll_pll1_define_ts_phase_initial_hi;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x2000;

    
    define->data = 0x2000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_PHY_1588_DPLL_PHASE_INITIAL_HI;
    define->property.doc = 
        "\n"
        "Initial phase values for the IEEE1588 DPLL, higher 16 bits\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 65535;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_pll_pll1_ts_nof_bits_hi_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pll;
    int submodule_index = dnx_data_pll_submodule_pll1;
    int define_index = dnx_data_pll_pll1_define_ts_nof_bits_hi;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 20;

    
    define->data = 20;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_pll_pll1_ts_pll1_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pll;
    int submodule_index = dnx_data_pll_submodule_pll1;
    int define_index = dnx_data_pll_pll1_define_ts_pll1_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_pll.pll1.config_get(unit, DNX_PLL1_TYPE_TS)->pll1_id;

    
    define->data = dnx_data_pll.pll1.config_get(unit, DNX_PLL1_TYPE_TS)->pll1_id;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_pll_pll1_bs_pll1_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pll;
    int submodule_index = dnx_data_pll_submodule_pll1;
    int define_index = dnx_data_pll_pll1_define_bs_pll1_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_pll.pll1.config_get(unit, DNX_PLL1_TYPE_BS)->pll1_id;

    
    define->data = dnx_data_pll.pll1.config_get(unit, DNX_PLL1_TYPE_BS)->pll1_id;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_pll_pll1_fab_pll1_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pll;
    int submodule_index = dnx_data_pll_submodule_pll1;
    int define_index = dnx_data_pll_pll1_define_fab_pll1_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_pll.pll1.config_get(unit, DNX_PLL1_TYPE_FAB)->pll1_id;

    
    define->data = dnx_data_pll.pll1.config_get(unit, DNX_PLL1_TYPE_FAB)->pll1_id;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2c_a0_dnx_data_pll_pll1_config_set(
    int unit)
{
    int pll1_type_index;
    dnx_data_pll_pll1_config_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_pll;
    int submodule_index = dnx_data_pll_submodule_pll1;
    int table_index = dnx_data_pll_pll1_table_config;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_PLL1_TYPE_COUNT;
    table->info_get.key_size[0] = DNX_PLL1_TYPE_COUNT;

    
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
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_pll_pll1_config_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_pll_pll1_table_config");

    
    default_data = (dnx_data_pll_pll1_config_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->pll1_id = -1;
    default_data->valid = -1;
    default_data->pdiv = -1;
    default_data->ndiv_int = -1;
    default_data->ch0_mdiv = -1;
    default_data->ch1_mdiv = -1;
    default_data->ch2_mdiv = -1;
    default_data->ch3_mdiv = -1;
    default_data->ch4_mdiv = -1;
    default_data->ch5_mdiv = -1;
    default_data->output_cml_en = -1;
    default_data->refclk_source_sel = -1;
    default_data->refclk = -1;
    
    for (pll1_type_index = 0; pll1_type_index < table->keys[0].size; pll1_type_index++)
    {
        data = (dnx_data_pll_pll1_config_t *) dnxc_data_mgmt_table_data_get(unit, table, pll1_type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_PLL1_TYPE_TS < table->keys[0].size)
    {
        data = (dnx_data_pll_pll1_config_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PLL1_TYPE_TS, 0);
        data->pll1_id = 0;
        data->valid = 1;
        data->pdiv = 1;
        data->ndiv_int = 160;
        data->ch0_mdiv = 4;
        data->ch1_mdiv = 8;
        data->ch2_mdiv = 10;
        data->ch3_mdiv = 16;
        data->ch4_mdiv = 40;
        data->output_cml_en = 1;
        data->refclk_source_sel = 1;
        data->refclk = 25;
    }
    if (DNX_PLL1_TYPE_BS < table->keys[0].size)
    {
        data = (dnx_data_pll_pll1_config_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PLL1_TYPE_BS, 0);
        data->pll1_id = 1;
        data->valid = 1;
        data->pdiv = 1;
        data->ndiv_int = 120;
        data->ch0_mdiv = 120;
        data->output_cml_en = 1;
        data->refclk_source_sel = 0;
        data->refclk = 25;
    }
    if (DNX_PLL1_TYPE_FAB < table->keys[0].size)
    {
        data = (dnx_data_pll_pll1_config_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PLL1_TYPE_FAB, 0);
        data->pll1_id = 2;
        data->valid = 1;
        data->pdiv = 4;
        data->ndiv_int = 116;
        data->ch0_mdiv = 5;
        data->ch1_mdiv = 29;
        data->ch2_mdiv = 3;
        data->ch3_mdiv = 12;
        data->output_cml_en = 0;
        data->refclk_source_sel = 1;
        data->refclk = 156;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2c_a0_dnx_data_pll_pll3_nof_pll3_set(
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
j2c_a0_dnx_data_pll_pll4_nof_pll4_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pll;
    int submodule_index = dnx_data_pll_submodule_pll4;
    int define_index = dnx_data_pll_pll4_define_nof_pll4;
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
j2c_a0_dnx_data_pll_synce_pll_present_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_pll;
    int submodule_index = dnx_data_pll_submodule_synce_pll;
    int feature_index = dnx_data_pll_synce_pll_present;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}





shr_error_e
j2c_a0_data_pll_attach(
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
    feature->set = j2c_a0_dnx_data_pll_general_ts_freq_lock_set;
    data_index = dnx_data_pll_general_bs_enable;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_pll_general_bs_enable_set;

    
    data_index = dnx_data_pll_general_table_nif_pll;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_pll_general_nif_pll_set;
    data_index = dnx_data_pll_general_table_fabric_pll;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_pll_general_fabric_pll_set;
    
    submodule_index = dnx_data_pll_submodule_pll1;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_pll_pll1_define_nof_pll1;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_pll_pll1_nof_pll1_set;
    data_index = dnx_data_pll_pll1_define_ts_phase_initial_lo;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_pll_pll1_ts_phase_initial_lo_set;
    data_index = dnx_data_pll_pll1_define_ts_phase_initial_hi;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_pll_pll1_ts_phase_initial_hi_set;
    data_index = dnx_data_pll_pll1_define_ts_nof_bits_hi;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_pll_pll1_ts_nof_bits_hi_set;
    data_index = dnx_data_pll_pll1_define_ts_pll1_id;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_pll_pll1_ts_pll1_id_set;
    data_index = dnx_data_pll_pll1_define_bs_pll1_id;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_pll_pll1_bs_pll1_id_set;
    data_index = dnx_data_pll_pll1_define_fab_pll1_id;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_pll_pll1_fab_pll1_id_set;

    
    data_index = dnx_data_pll_pll1_ts_phase_initial_hi_config;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_pll_pll1_ts_phase_initial_hi_config_set;
    data_index = dnx_data_pll_pll1_ts_refclk_source_internal;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_pll_pll1_ts_refclk_source_internal_set;

    
    data_index = dnx_data_pll_pll1_table_config;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_pll_pll1_config_set;
    
    submodule_index = dnx_data_pll_submodule_pll3;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_pll_pll3_define_nof_pll3;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_pll_pll3_nof_pll3_set;

    

    
    
    submodule_index = dnx_data_pll_submodule_pll4;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_pll_pll4_define_nof_pll4;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_pll_pll4_nof_pll4_set;

    

    
    
    submodule_index = dnx_data_pll_submodule_synce_pll;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_pll_synce_pll_present;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_pll_synce_pll_present_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

