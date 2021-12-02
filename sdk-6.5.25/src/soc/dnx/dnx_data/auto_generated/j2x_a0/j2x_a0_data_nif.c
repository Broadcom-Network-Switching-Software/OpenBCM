
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

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_nif.h>
#include <soc/dnx/intr/auto_generated/j2x/j2x_intr.h>
#include <bcm_int/dnx/algo/flexe_general/algo_flexe_general.h>
#include <include/bcm_int/dnx/port/port.h>







static shr_error_e
j2x_a0_dnx_data_nif_global_reassembler_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int feature_index = dnx_data_nif_global_reassembler;
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
j2x_a0_dnx_data_nif_global_clock_power_down_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int feature_index = dnx_data_nif_global_clock_power_down;
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
j2x_a0_dnx_data_nif_global_is_shared_arch_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int feature_index = dnx_data_nif_global_is_shared_arch;
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
j2x_a0_dnx_data_nif_global_is_ilkn_after_flexe_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int feature_index = dnx_data_nif_global_is_ilkn_after_flexe;
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
j2x_a0_dnx_data_nif_global_nof_lcplls_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int define_index = dnx_data_nif_global_define_nof_lcplls;
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
j2x_a0_dnx_data_nif_global_nof_nif_interfaces_per_core_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int define_index = dnx_data_nif_global_define_nof_nif_interfaces_per_core;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 386;

    
    define->data = 386;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_global_nif_interface_id_to_unit_id_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int define_index = dnx_data_nif_global_define_nif_interface_id_to_unit_id_granularity;
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
j2x_a0_dnx_data_nif_global_nof_nif_units_per_core_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int define_index = dnx_data_nif_global_define_nof_nif_units_per_core;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.eth.ethu_nof_per_core_get(unit) + 1 + dnx_data_nif.ilkn.ilkn_unit_nof_get(unit);

    
    define->data = dnx_data_nif.eth.ethu_nof_per_core_get(unit) + 1 + dnx_data_nif.ilkn.ilkn_unit_nof_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_global_reassembler_fifo_threshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int define_index = dnx_data_nif_global_define_reassembler_fifo_threshold;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 90;

    
    define->data = 90;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_global_l1_only_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int define_index = dnx_data_nif_global_define_l1_only_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DNX_PORT_NIF_L1_ONLY_MODE_DISABLED;

    
    define->data = DNX_PORT_NIF_L1_ONLY_MODE_DISABLED;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_L1_ONLY_MODE;
    define->property.doc = 
        "\n"
        "Enable/disable power optimization for L1 only, and set its mode\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 3;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "DISABLED";
    define->property.mapping[0].val = DNX_PORT_NIF_L1_ONLY_MODE_DISABLED;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "L1_ONLY_WITHOUT_ILKN";
    define->property.mapping[1].val = DNX_PORT_NIF_L1_ONLY_MODE_WITHOUT_ILKN;
    define->property.mapping[2].name = "L1_ONLY_WITH_ILKN";
    define->property.mapping[2].val = DNX_PORT_NIF_L1_ONLY_MODE_WITH_ILKN;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2x_a0_dnx_data_nif_global_nif_interface_id_to_unit_id_set(
    int unit)
{
    int nif_interface_id_index;
    dnx_data_nif_global_nif_interface_id_to_unit_id_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int table_index = dnx_data_nif_global_table_nif_interface_id_to_unit_id;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_nif.global.nof_nif_interfaces_per_core_get(unit);
    table->info_get.key_size[0] = dnx_data_nif.global.nof_nif_interfaces_per_core_get(unit);

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_global_nif_interface_id_to_unit_id_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_global_table_nif_interface_id_to_unit_id");

    
    default_data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->unit_id = 0;
    default_data->internal_offset = 0;
    
    for (nif_interface_id_index = 0; nif_interface_id_index < table->keys[0].size; nif_interface_id_index++)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, nif_interface_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->unit_id = 0;
        data->internal_offset = 0;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->unit_id = 1;
        data->internal_offset = 0;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->unit_id = 2;
        data->internal_offset = 0;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->unit_id = 3;
        data->internal_offset = 0;
    }
    if (32 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 32, 0);
        data->unit_id = 4;
        data->internal_offset = 0;
    }
    if (40 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 40, 0);
        data->unit_id = 5;
        data->internal_offset = 0;
    }
    if (48 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 48, 0);
        data->unit_id = 6;
        data->internal_offset = 0;
    }
    if (56 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 56, 0);
        data->unit_id = 7;
        data->internal_offset = 0;
    }
    if (64 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 64, 0);
        data->unit_id = 8;
        data->internal_offset = 0;
    }
    if (72 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 72, 0);
        data->unit_id = 8;
        data->internal_offset = 8;
    }
    if (80 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 80, 0);
        data->unit_id = 8;
        data->internal_offset = 16;
    }
    if (88 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 88, 0);
        data->unit_id = 8;
        data->internal_offset = 24;
    }
    if (96 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 96, 0);
        data->unit_id = 8;
        data->internal_offset = 32;
    }
    if (104 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 104, 0);
        data->unit_id = 8;
        data->internal_offset = 40;
    }
    if (112 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 112, 0);
        data->unit_id = 8;
        data->internal_offset = 48;
    }
    if (120 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 120, 0);
        data->unit_id = 8;
        data->internal_offset = 56;
    }
    if (128 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 128, 0);
        data->unit_id = 8;
        data->internal_offset = 64;
    }
    if (136 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 136, 0);
        data->unit_id = 8;
        data->internal_offset = 72;
    }
    if (144 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 144, 0);
        data->unit_id = 8;
        data->internal_offset = 80;
    }
    if (152 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 152, 0);
        data->unit_id = 8;
        data->internal_offset = 88;
    }
    if (160 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 160, 0);
        data->unit_id = 8;
        data->internal_offset = 96;
    }
    if (168 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 168, 0);
        data->unit_id = 8;
        data->internal_offset = 104;
    }
    if (176 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 176, 0);
        data->unit_id = 8;
        data->internal_offset = 112;
    }
    if (184 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 184, 0);
        data->unit_id = 8;
        data->internal_offset = 120;
    }
    if (192 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 192, 0);
        data->unit_id = 8;
        data->internal_offset = 128;
    }
    if (200 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 200, 0);
        data->unit_id = 8;
        data->internal_offset = 136;
    }
    if (208 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 208, 0);
        data->unit_id = 8;
        data->internal_offset = 144;
    }
    if (216 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 216, 0);
        data->unit_id = 8;
        data->internal_offset = 152;
    }
    if (224 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 224, 0);
        data->unit_id = 8;
        data->internal_offset = 160;
    }
    if (232 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 232, 0);
        data->unit_id = 8;
        data->internal_offset = 168;
    }
    if (240 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 240, 0);
        data->unit_id = 8;
        data->internal_offset = 176;
    }
    if (248 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 248, 0);
        data->unit_id = 8;
        data->internal_offset = 184;
    }
    if (256 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 256, 0);
        data->unit_id = 8;
        data->internal_offset = 192;
    }
    if (264 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 264, 0);
        data->unit_id = 8;
        data->internal_offset = 200;
    }
    if (272 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 272, 0);
        data->unit_id = 8;
        data->internal_offset = 208;
    }
    if (280 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 280, 0);
        data->unit_id = 8;
        data->internal_offset = 216;
    }
    if (288 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 288, 0);
        data->unit_id = 8;
        data->internal_offset = 224;
    }
    if (296 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 296, 0);
        data->unit_id = 8;
        data->internal_offset = 232;
    }
    if (304 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 304, 0);
        data->unit_id = 8;
        data->internal_offset = 240;
    }
    if (312 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 312, 0);
        data->unit_id = 8;
        data->internal_offset = 248;
    }
    if (320 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 320, 0);
        data->unit_id = 8;
        data->internal_offset = 256;
    }
    if (328 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 328, 0);
        data->unit_id = 8;
        data->internal_offset = 264;
    }
    if (336 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 336, 0);
        data->unit_id = 8;
        data->internal_offset = 272;
    }
    if (344 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 344, 0);
        data->unit_id = 8;
        data->internal_offset = 280;
    }
    if (352 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 352, 0);
        data->unit_id = 8;
        data->internal_offset = 288;
    }
    if (360 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 360, 0);
        data->unit_id = 8;
        data->internal_offset = 296;
    }
    if (368 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 368, 0);
        data->unit_id = 8;
        data->internal_offset = 304;
    }
    if (376 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 376, 0);
        data->unit_id = 8;
        data->internal_offset = 312;
    }
    if (384 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 384, 0);
        data->unit_id = 9;
        data->internal_offset = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2x_a0_dnx_data_nif_phys_nof_phys_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_phys;
    int define_index = dnx_data_nif_phys_define_nof_phys;
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
j2x_a0_dnx_data_nif_phys_max_phys_in_core_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_phys;
    int define_index = dnx_data_nif_phys_define_max_phys_in_core;
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
j2x_a0_dnx_data_nif_phys_pm8x50_gen_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_phys;
    int define_index = dnx_data_nif_phys_define_pm8x50_gen;
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
j2x_a0_dnx_data_nif_phys_general_set(
    int unit)
{
    dnx_data_nif_phys_general_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_phys;
    int table_index = dnx_data_nif_phys_table_general;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "0xFFFFFFFF,0xFFFFFFFF";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_phys_general_t, (1 + 1  ), "data of dnx_data_nif_phys_table_general");

    
    default_data = (dnx_data_nif_phys_general_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    #if 2 > _SHR_PBMP_WORD_MAX
    #error "out of bound access to array"
    #endif
    _SHR_PBMP_WORD_SET(default_data->supported_phys, 0, 0xFFFFFFFF);
    _SHR_PBMP_WORD_SET(default_data->supported_phys, 1, 0xFFFFFFFF);
    
    data = (dnx_data_nif_phys_general_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_phys_core_phys_map_set(
    int unit)
{
    int core_index_index;
    dnx_data_nif_phys_core_phys_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_phys;
    int table_index = dnx_data_nif_phys_table_core_phys_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_device.general.nof_cores_get(unit);
    table->info_get.key_size[0] = dnx_data_device.general.nof_cores_get(unit);

    
    table->values[0].default_val = "0x0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_phys_core_phys_map_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_phys_table_core_phys_map");

    
    default_data = (dnx_data_nif_phys_core_phys_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    _SHR_PBMP_WORD_SET(default_data->phys, 0, 0x0);
    
    for (core_index_index = 0; core_index_index < table->keys[0].size; core_index_index++)
    {
        data = (dnx_data_nif_phys_core_phys_map_t *) dnxc_data_mgmt_table_data_get(unit, table, core_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_core_phys_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        #if 2 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0xFFFFFFFF);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0xFFFFFFFF);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_phys_vco_div_set(
    int unit)
{
    int ethu_index_index;
    dnx_data_nif_phys_vco_div_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_phys;
    int table_index = dnx_data_nif_phys_table_vco_div;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_nif.eth.ethu_nof_get(unit);
    table->info_get.key_size[0] = dnx_data_nif.eth.ethu_nof_get(unit);

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_phys_vco_div_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_phys_table_vco_div");

    
    default_data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->vco_div = 0;
    
    for (ethu_index_index = 0; ethu_index_index < table->keys[0].size; ethu_index_index++)
    {
        data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, ethu_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->vco_div = 20;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->vco_div = 20;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->vco_div = 20;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->vco_div = 20;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->vco_div = 20;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->vco_div = 20;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->vco_div = 20;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->vco_div = 20;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_phys_nof_phys_per_core_set(
    int unit)
{
    int core_index_index;
    dnx_data_nif_phys_nof_phys_per_core_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_phys;
    int table_index = dnx_data_nif_phys_table_nof_phys_per_core;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_device.general.nof_cores_get(unit);
    table->info_get.key_size[0] = dnx_data_device.general.nof_cores_get(unit);

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_phys_nof_phys_per_core_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_phys_table_nof_phys_per_core");

    
    default_data = (dnx_data_nif_phys_nof_phys_per_core_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->nof_phys = 0;
    default_data->first_phy = 0;
    
    for (core_index_index = 0; core_index_index < table->keys[0].size; core_index_index++)
    {
        data = (dnx_data_nif_phys_nof_phys_per_core_t *) dnxc_data_mgmt_table_data_get(unit, table, core_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_nof_phys_per_core_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->nof_phys = 64;
        data->first_phy = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2x_a0_dnx_data_nif_ilkn_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_is_supported;
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
j2x_a0_dnx_data_nif_ilkn_is_elk_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_is_elk_supported;
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
j2x_a0_dnx_data_nif_ilkn_over_fabric_per_core_selector_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_over_fabric_per_core_selector;
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
j2x_a0_dnx_data_nif_ilkn_frmr_ilkn_selector_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_frmr_ilkn_selector;
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
j2x_a0_dnx_data_nif_ilkn_support_burst_interleaving_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_support_burst_interleaving;
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
j2x_a0_dnx_data_nif_ilkn_scheduler_context_per_port_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_scheduler_context_per_port_support;
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
j2x_a0_dnx_data_nif_ilkn_support_ilu_burst_max_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_support_ilu_burst_max;
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
j2x_a0_dnx_data_nif_ilkn_is_fec_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_is_fec_supported;
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
j2x_a0_dnx_data_nif_ilkn_is_20p6G_speed_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_is_20p6G_speed_supported;
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
j2x_a0_dnx_data_nif_ilkn_is_25G_speed_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_is_25G_speed_supported;
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
j2x_a0_dnx_data_nif_ilkn_is_28p1G_speed_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_is_28p1G_speed_supported;
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
j2x_a0_dnx_data_nif_ilkn_ilu_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_ilu_nof;
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
j2x_a0_dnx_data_nif_ilkn_ilkn_unit_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_ilkn_unit_nof;
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
j2x_a0_dnx_data_nif_ilkn_ilkn_unit_if_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_ilkn_unit_if_nof;
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
j2x_a0_dnx_data_nif_ilkn_fec_units_per_core_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_fec_units_per_core_nof;
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
j2x_a0_dnx_data_nif_ilkn_lanes_max_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_lanes_max_nof;
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
j2x_a0_dnx_data_nif_ilkn_lanes_max_nof_using_fec_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_lanes_max_nof_using_fec;
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
j2x_a0_dnx_data_nif_ilkn_lanes_allowed_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_lanes_allowed_nof;
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
j2x_a0_dnx_data_nif_ilkn_ilkn_over_eth_pms_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_ilkn_over_eth_pms_max;
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
j2x_a0_dnx_data_nif_ilkn_segments_max_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_segments_max_nof;
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
j2x_a0_dnx_data_nif_ilkn_segments_half_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_segments_half_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.ilkn.segments_max_nof_get(unit)/2;

    
    define->data = dnx_data_nif.ilkn.segments_max_nof_get(unit)/2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ilkn_pms_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_pms_nof;
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
j2x_a0_dnx_data_nif_ilkn_fmac_bus_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_fmac_bus_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 70;

    
    define->data = 70;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ilkn_ilkn_rx_hrf_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_ilkn_rx_hrf_nof;
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
j2x_a0_dnx_data_nif_ilkn_ilkn_tx_hrf_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_ilkn_tx_hrf_nof;
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
j2x_a0_dnx_data_nif_ilkn_data_rx_hrf_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_data_rx_hrf_size;
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
j2x_a0_dnx_data_nif_ilkn_tdm_rx_hrf_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_tdm_rx_hrf_size;
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
j2x_a0_dnx_data_nif_ilkn_tx_hrf_credits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_tx_hrf_credits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 192;

    
    define->data = 192;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ilkn_watermark_high_data_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_watermark_high_data;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 11;

    
    define->data = 11;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ilkn_watermark_low_data_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_watermark_low_data;
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
j2x_a0_dnx_data_nif_ilkn_pad_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_pad_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 60;

    
    define->data = 60;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ilkn_burst_max_range_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_burst_max_range_max;
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
j2x_a0_dnx_data_nif_ilkn_burst_min_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_burst_min;
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
j2x_a0_dnx_data_nif_ilkn_burst_min_range_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_burst_min_range_max;
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
j2x_a0_dnx_data_nif_ilkn_port_0_status_intr_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_port_0_status_intr_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = J2X_INT_ILE_ILKN_RX_PORT_0_STATUS_CHANGE_INT;

    
    define->data = J2X_INT_ILE_ILKN_RX_PORT_0_STATUS_CHANGE_INT;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ilkn_port_1_status_intr_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_port_1_status_intr_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = J2X_INT_ILE_ILKN_RX_PORT_1_STATUS_CHANGE_INT;

    
    define->data = J2X_INT_ILE_ILKN_RX_PORT_1_STATUS_CHANGE_INT;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ilkn_interleaved_error_drop_single_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_interleaved_error_drop_single_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
        "When enabled, IRE is configured to treat ILKN ports as non-channelized.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "interleaved_error_drop_single_context";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2x_a0_dnx_data_nif_ilkn_supported_phys_set(
    int unit)
{
    int ilkn_id_index;
    dnx_data_nif_ilkn_supported_phys_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int table_index = dnx_data_nif_ilkn_table_supported_phys;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_ilkn_supported_phys_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_ilkn_table_supported_phys");

    
    default_data = (dnx_data_nif_ilkn_supported_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->is_supported = 0;
    default_data->max_phys = 0;
    _SHR_PBMP_WORD_SET(default_data->nif_phys, 0, 0);
    _SHR_PBMP_WORD_SET(default_data->fabric_phys, 0, 0);
    
    for (ilkn_id_index = 0; ilkn_id_index < table->keys[0].size; ilkn_id_index++)
    {
        data = (dnx_data_nif_ilkn_supported_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, ilkn_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->is_supported = 1;
        data->max_phys = 24;
        _SHR_PBMP_WORD_SET(data->nif_phys, 0, 0x00ffffff);
        _SHR_PBMP_WORD_SET(data->fabric_phys, 0, 0x0);
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->is_supported = 1;
        data->max_phys = 12;
        _SHR_PBMP_WORD_SET(data->nif_phys, 0, 0x00ffffff);
        _SHR_PBMP_WORD_SET(data->fabric_phys, 0, 0x0);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ilkn_supported_interfaces_set(
    int unit)
{
    int index_index;
    dnx_data_nif_ilkn_supported_interfaces_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int table_index = dnx_data_nif_ilkn_table_supported_interfaces;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 7;
    table->info_get.key_size[0] = 7;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "portmodDispatchTypeCount";
    table->values[2].default_val = "bcmPortPhyFecInvalid";
    table->values[3].default_val = "1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_ilkn_supported_interfaces_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_ilkn_table_supported_interfaces");

    
    default_data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->speed = -1;
    default_data->pm_dispatch_type = portmodDispatchTypeCount;
    default_data->fec_type = bcmPortPhyFecInvalid;
    default_data->is_valid = 1;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->speed = 10312;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->is_valid = dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_is_10p3G_speed_supported) && !dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_ilkn_over_fabric_only);
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->speed = 12500;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->is_valid = dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_is_12p5G_speed_supported) && !dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_ilkn_over_fabric_only);
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->speed = 20625;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->is_valid = !dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_ilkn_over_fabric_only) && dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_is_20p6G_speed_supported) > 0 ? TRUE : FALSE;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->speed = 25000;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->is_valid = !dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_ilkn_over_fabric_only) && dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_is_25G_speed_supported) > 0 ? TRUE : FALSE;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->speed = 25781;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->is_valid = dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_is_25p7G_speed_supported) && !dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_ilkn_over_fabric_only);
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->speed = 27343;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->is_valid = !dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_ilkn_over_fabric_only) && dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_is_27p3G_speed_supported) > 0 ? TRUE : FALSE;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->speed = 28125;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->is_valid = !dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_ilkn_over_fabric_only) && dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_is_28p1G_speed_supported) > 0 ? TRUE : FALSE;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ilkn_ilkn_cores_set(
    int unit)
{
    int ilkn_core_id_index;
    dnx_data_nif_ilkn_ilkn_cores_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int table_index = dnx_data_nif_ilkn_table_ilkn_cores;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 1;
    table->info_get.key_size[0] = 1;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_ilkn_ilkn_cores_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_ilkn_table_ilkn_cores");

    
    default_data = (dnx_data_nif_ilkn_ilkn_cores_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->is_elk_supported = 0;
    default_data->is_data_supported = 0;
    default_data->fec_disable_by_bypass = 0;
    default_data->fec_dependency = 0;
    
    for (ilkn_core_id_index = 0; ilkn_core_id_index < table->keys[0].size; ilkn_core_id_index++)
    {
        data = (dnx_data_nif_ilkn_ilkn_cores_t *) dnxc_data_mgmt_table_data_get(unit, table, ilkn_core_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_ilkn_cores_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->is_elk_supported = 0;
        data->is_data_supported = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ilkn_supported_device_core_set(
    int unit)
{
    int ilkn_id_index;
    dnx_data_nif_ilkn_supported_device_core_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int table_index = dnx_data_nif_ilkn_table_supported_device_core;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_ilkn_supported_device_core_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_ilkn_table_supported_device_core");

    
    default_data = (dnx_data_nif_ilkn_supported_device_core_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->core = 0;
    
    for (ilkn_id_index = 0; ilkn_id_index < table->keys[0].size; ilkn_id_index++)
    {
        data = (dnx_data_nif_ilkn_supported_device_core_t *) dnxc_data_mgmt_table_data_get(unit, table, ilkn_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ilkn_nif_pms_set(
    int unit)
{
    int pm_id_index;
    dnx_data_nif_ilkn_nif_pms_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int table_index = dnx_data_nif_ilkn_table_nif_pms;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 8;
    table->info_get.key_size[0] = 8;

    
    table->values[0].default_val = "portmodDispatchTypeCount";
    table->values[1].default_val = "-1";
    table->values[2].default_val = "-1";
    table->values[3].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_ilkn_nif_pms_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_ilkn_table_nif_pms");

    
    default_data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dispatch_type = portmodDispatchTypeCount;
    default_data->first_phy = -1;
    default_data->nof_phys = -1;
    default_data->facing_core_index = -1;
    
    for (pm_id_index = 0; pm_id_index < table->keys[0].size; pm_id_index++)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, pm_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->first_phy = 0;
        data->nof_phys = 8;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->first_phy = 8;
        data->nof_phys = 8;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->first_phy = 16;
        data->nof_phys = 8;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ilkn_nif_lanes_map_set(
    int unit)
{
    int ilkn_core_index;
    int range_id_index;
    dnx_data_nif_ilkn_nif_lanes_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int table_index = dnx_data_nif_ilkn_table_nif_lanes_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 1;
    table->info_get.key_size[0] = 1;
    table->keys[1].size = 1;
    table->info_get.key_size[1] = 1;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "-1";
    table->values[2].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_ilkn_nif_lanes_map_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_nif_ilkn_table_nif_lanes_map");

    
    default_data = (dnx_data_nif_ilkn_nif_lanes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->first_phy = -1;
    default_data->ilkn_lane_offset = -1;
    default_data->nof_lanes = -1;
    
    for (ilkn_core_index = 0; ilkn_core_index < table->keys[0].size; ilkn_core_index++)
    {
        for (range_id_index = 0; range_id_index < table->keys[1].size; range_id_index++)
        {
            data = (dnx_data_nif_ilkn_nif_lanes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, ilkn_core_index, range_id_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (0 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_nif_ilkn_nif_lanes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->first_phy = 0;
        data->ilkn_lane_offset = 0;
        data->nof_lanes = 24;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ilkn_start_tx_threshold_table_set(
    int unit)
{
    int idx_index;
    dnx_data_nif_ilkn_start_tx_threshold_table_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int table_index = dnx_data_nif_ilkn_table_start_tx_threshold_table;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 1;
    table->info_get.key_size[0] = 1;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_ilkn_start_tx_threshold_table_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_ilkn_table_start_tx_threshold_table");

    
    default_data = (dnx_data_nif_ilkn_start_tx_threshold_table_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->speed = 0;
    default_data->start_thr = 0;
    
    for (idx_index = 0; idx_index < table->keys[0].size; idx_index++)
    {
        data = (dnx_data_nif_ilkn_start_tx_threshold_table_t *) dnxc_data_mgmt_table_data_get(unit, table, idx_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_start_tx_threshold_table_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->speed = 600000;
        data->start_thr = 13;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2x_a0_dnx_data_nif_eth_gear_shifter_exists_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int feature_index = dnx_data_nif_eth_gear_shifter_exists;
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
j2x_a0_dnx_data_nif_eth_cdu_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_cdu_nof;
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
j2x_a0_dnx_data_nif_eth_clu_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_clu_nof;
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
j2x_a0_dnx_data_nif_eth_cdum_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_cdum_nof;
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
j2x_a0_dnx_data_nif_eth_nof_pms_in_cdu_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_nof_pms_in_cdu;
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
j2x_a0_dnx_data_nif_eth_nof_lanes_in_cdu_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_nof_lanes_in_cdu;
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
j2x_a0_dnx_data_nif_eth_cdu_mac_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_cdu_mac_nof;
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
j2x_a0_dnx_data_nif_eth_mac_lanes_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_mac_lanes_nof;
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
j2x_a0_dnx_data_nif_eth_cdu_memory_entries_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_cdu_memory_entries_nof;
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
j2x_a0_dnx_data_nif_eth_priority_groups_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_priority_groups_nof;
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
j2x_a0_dnx_data_nif_eth_phy_map_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_phy_map_granularity;
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
j2x_a0_dnx_data_nif_eth_pm_properties_set(
    int unit)
{
    int pm_index_index;
    dnx_data_nif_eth_pm_properties_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int table_index = dnx_data_nif_eth_table_pm_properties;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_nif.eth.total_nof_ethu_pms_in_device_get(unit);
    table->info_get.key_size[0] = dnx_data_nif.eth.total_nof_ethu_pms_in_device_get(unit);

    
    table->values[0].default_val = "0x0,0x0,0x0,0x0";
    table->values[1].default_val = "0x1";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    table->values[4].default_val = "portmodDispatchTypeCount";
    table->values[5].default_val = "0";
    table->values[6].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_eth_pm_properties_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_eth_table_pm_properties");

    
    default_data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    #if 4 > _SHR_PBMP_WORD_MAX
    #error "out of bound access to array"
    #endif
    _SHR_PBMP_WORD_SET(default_data->phys, 0, 0x0);
    _SHR_PBMP_WORD_SET(default_data->phys, 1, 0x0);
    _SHR_PBMP_WORD_SET(default_data->phys, 2, 0x0);
    _SHR_PBMP_WORD_SET(default_data->phys, 3, 0x0);
    default_data->tvco_pll_index = 0x1;
    default_data->phy_addr = 0;
    default_data->is_master = 0;
    default_data->dispatch_type = portmodDispatchTypeCount;
    default_data->special_pll_check = 0;
    default_data->flr_support = 0;
    
    for (pm_index_index = 0; pm_index_index < table->keys[0].size; pm_index_index++)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, pm_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        #if 2 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x000000FF);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        data->tvco_pll_index = 1;
        data->phy_addr = 0;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        #if 2 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x0000FF00);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        data->tvco_pll_index = 1;
        data->phy_addr = 8;
        data->is_master = 1;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        #if 2 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00FF0000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        data->tvco_pll_index = 1;
        data->phy_addr = 16;
        data->is_master = 1;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        #if 2 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0xFF000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        data->tvco_pll_index = 1;
        data->phy_addr = 24;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        #if 2 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x000000FF);
        data->tvco_pll_index = 1;
        data->phy_addr = 32;
        data->is_master = 1;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        #if 2 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x0000FF00);
        data->tvco_pll_index = 1;
        data->phy_addr = 40;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        #if 2 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00FF0000);
        data->tvco_pll_index = 1;
        data->phy_addr = 48;
        data->is_master = 1;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        #if 2 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0xFF000000);
        data->tvco_pll_index = 1;
        data->phy_addr = 56;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_eth_ethu_properties_set(
    int unit)
{
    int ethu_index_index;
    dnx_data_nif_eth_ethu_properties_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int table_index = dnx_data_nif_eth_table_ethu_properties;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_nif.eth.ethu_nof_get(unit);
    table->info_get.key_size[0] = dnx_data_nif.eth.ethu_nof_get(unit);

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "imbDispatchTypeNone";
    table->values[2].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_eth_ethu_properties_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_eth_table_ethu_properties");

    
    default_data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->pms[0] = -1;
    default_data->type = imbDispatchTypeNone;
    default_data->type_index = -1;
    
    for (ethu_index_index = 0; ethu_index_index < table->keys[0].size; ethu_index_index++)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, ethu_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->pms[0] = 0;
        data->type = imbDispatchTypeImb_cdu_shr;
        data->type_index = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->pms[0] = 1;
        data->type = imbDispatchTypeImb_cdu_shr;
        data->type_index = 1;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->pms[0] = 2;
        data->type = imbDispatchTypeImb_cdu_shr;
        data->type_index = 2;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->pms[0] = 3;
        data->type = imbDispatchTypeImb_cdu_shr;
        data->type_index = 3;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->pms[0] = 4;
        data->type = imbDispatchTypeImb_cdu_shr;
        data->type_index = 4;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->pms[0] = 5;
        data->type = imbDispatchTypeImb_cdu_shr;
        data->type_index = 5;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->pms[0] = 6;
        data->type = imbDispatchTypeImb_cdu_shr;
        data->type_index = 6;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->pms[0] = 7;
        data->type = imbDispatchTypeImb_cdu_shr;
        data->type_index = 7;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_eth_phy_map_set(
    int unit)
{
    int idx_index;
    dnx_data_nif_eth_phy_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int table_index = dnx_data_nif_eth_table_phy_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_nif.phys.nof_phys_get(unit)/dnx_data_nif.eth.phy_map_granularity_get(unit);
    table->info_get.key_size[0] = dnx_data_nif.phys.nof_phys_get(unit)/dnx_data_nif.eth.phy_map_granularity_get(unit);

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "-1";
    table->values[2].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_eth_phy_map_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_eth_table_phy_map");

    
    default_data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->phy_index = -1;
    default_data->ethu_index = -1;
    default_data->pm_index = -1;
    
    for (idx_index = 0; idx_index < table->keys[0].size; idx_index++)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, idx_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->phy_index = 0;
        data->ethu_index = 0;
        data->pm_index = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->phy_index = 8;
        data->ethu_index = 1;
        data->pm_index = 1;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->phy_index = 16;
        data->ethu_index = 2;
        data->pm_index = 2;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->phy_index = 24;
        data->ethu_index = 3;
        data->pm_index = 3;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->phy_index = 32;
        data->ethu_index = 4;
        data->pm_index = 4;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->phy_index = 40;
        data->ethu_index = 5;
        data->pm_index = 5;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->phy_index = 48;
        data->ethu_index = 6;
        data->pm_index = 6;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->phy_index = 56;
        data->ethu_index = 7;
        data->pm_index = 7;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_eth_supported_interfaces_set(
    int unit)
{
    int idx_index;
    dnx_data_nif_eth_supported_interfaces_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int table_index = dnx_data_nif_eth_table_supported_interfaces;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 28;
    table->info_get.key_size[0] = 28;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "-1";
    table->values[2].default_val = "bcmPortPhyFecInvalid";
    table->values[3].default_val = "portmodDispatchTypeCount";
    table->values[4].default_val = "1";
    table->values[5].default_val = "1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_eth_supported_interfaces_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_eth_table_supported_interfaces");

    
    default_data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->speed = 0;
    default_data->nof_lanes = -1;
    default_data->fec_type = bcmPortPhyFecInvalid;
    default_data->dispatch_type = portmodDispatchTypeCount;
    default_data->is_valid = 1;
    default_data->ptp_support = 1;
    
    for (idx_index = 0; idx_index < table->keys[0].size; idx_index++)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, idx_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->speed = 10000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->speed = 10000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecBaseR;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->ptp_support = FALSE;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->speed = 12000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->ptp_support = dnx_data_nif.phys.pm8x50_gen_get(unit) > 1 ? TRUE : FALSE;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->speed = 25000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->speed = 25000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecBaseR;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->ptp_support = FALSE;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->speed = 25000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecRsFec;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->speed = 40000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->speed = 40000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->speed = 40000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecBaseR;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->ptp_support = FALSE;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->speed = 50000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecRsFec;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->speed = 50000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecRs544;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->speed = 50000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecRs272;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) > 0 ? TRUE : FALSE;
        data->ptp_support = dnx_data_nif.phys.pm8x50_gen_get(unit) > 1 ? TRUE : FALSE;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->speed = 50000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) && dnx_data_nif.phys.pm8x50_gen_get(unit) > 1 ? TRUE : FALSE;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->speed = 50000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->speed = 50000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecRsFec;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->speed = 50000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecRs544;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->speed = 100000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->speed = 100000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecRsFec;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->speed = 100000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecRs544;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->speed = 100000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecRs272;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) > 0 ? TRUE : FALSE;
        data->ptp_support = dnx_data_nif.phys.pm8x50_gen_get(unit) > 1 ? TRUE : FALSE;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->speed = 100000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->speed = 100000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecRsFec;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->speed = 100000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecRs544;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->speed = 200000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->speed = 200000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecRs544_2xN;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (25 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, 0);
        data->speed = 200000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecRs272_2xN;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) && dnx_data_nif.phys.pm8x50_gen_get(unit) > 1 ? TRUE : FALSE;
        data->ptp_support = dnx_data_nif.phys.pm8x50_gen_get(unit) > 1 ? TRUE : FALSE;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->speed = 400000;
        data->nof_lanes = 8;
        data->fec_type = bcmPortPhyFecRs544_2xN;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) && dnx_data_nif.eth.is_400G_supported_get(unit) == 1 ? TRUE : FALSE;
    }
    if (27 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, 0);
        data->speed = 400000;
        data->nof_lanes = 8;
        data->fec_type = bcmPortPhyFecRs272_2xN;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) && dnx_data_nif.phys.pm8x50_gen_get(unit) > 1 ? TRUE : FALSE && dnx_data_nif.eth.is_400G_supported_get(unit) == 1 ? TRUE : FALSE ;
        data->ptp_support = dnx_data_nif.phys.pm8x50_gen_get(unit) > 1 ? TRUE : FALSE;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2x_a0_dnx_data_nif_simulator_cdu_type_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_simulator;
    int define_index = dnx_data_nif_simulator_define_cdu_type;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = phymodDispatchTypeTscbh_fe_gen2;

    
    define->data = phymodDispatchTypeTscbh_fe_gen2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2x_a0_dnx_data_nif_flexe_nof_clients_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nof_clients;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 320;

    
    define->data = 320;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2x_a0_dnx_data_nif_prd_tcam_key_offset_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_tcam_key_offset_size;
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
j2x_a0_dnx_data_nif_prd_rmc_threshold_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_rmc_threshold_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xffff;

    
    define->data = 0xffff;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_prd_nof_ofr_port_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_nof_ofr_port_profiles;
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
j2x_a0_dnx_data_nif_prd_nof_ofr_nif_interfaces_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_nof_ofr_nif_interfaces;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.global.nof_nif_interfaces_per_core_get(unit) - dnx_data_nif.ilkn.ilu_nof_get(unit)*dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit);

    
    define->data = dnx_data_nif.global.nof_nif_interfaces_per_core_get(unit) - dnx_data_nif.ilkn.ilu_nof_get(unit)*dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2x_a0_dnx_data_nif_portmod_pm_types_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_portmod;
    int define_index = dnx_data_nif_portmod_define_pm_types_nof;
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
j2x_a0_dnx_data_nif_portmod_pm_types_and_interfaces_set(
    int unit)
{
    int index_index;
    dnx_data_nif_portmod_pm_types_and_interfaces_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_portmod;
    int table_index = dnx_data_nif_portmod_table_pm_types_and_interfaces;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_DATA_MAX_NIF_PORTMOD_PM_TYPES_NOF;
    table->info_get.key_size[0] = DNX_DATA_MAX_NIF_PORTMOD_PM_TYPES_NOF;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_portmod_pm_types_and_interfaces_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_portmod_table_pm_types_and_interfaces");

    
    default_data = (dnx_data_nif_portmod_pm_types_and_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->type = 0;
    default_data->instances = 0;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_nif_portmod_pm_types_and_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_portmod_pm_types_and_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->type = portmodDispatchTypePm8x50_flexe_gen2;
        data->instances = dnx_data_nif.eth.cdu_nof_get(unit);
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_portmod_pm_types_and_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->type = portmodDispatchTypePmOsILKN_50G;
        data->instances = dnx_data_nif.ilkn.ilkn_unit_nof_get(unit);
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_portmod_pm_types_and_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->type = portmodDispatchTypePm8x50_fabric;
        data->instances = dnx_data_fabric.blocks.nof_pms_get(unit);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2x_a0_dnx_data_nif_dbal_cdu_rx_rmc_entry_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_dbal;
    int define_index = dnx_data_nif_dbal_define_cdu_rx_rmc_entry_size;
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
j2x_a0_dnx_data_nif_dbal_cdu_tx_start_threshold_entry_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_dbal;
    int define_index = dnx_data_nif_dbal_define_cdu_tx_start_threshold_entry_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 11;

    
    define->data = 11;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_dbal_ilu_burst_min_entry_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_dbal;
    int define_index = dnx_data_nif_dbal_define_ilu_burst_min_entry_size;
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
j2x_a0_dnx_data_nif_dbal_ethu_rx_rmc_counter_entry_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_dbal;
    int define_index = dnx_data_nif_dbal_define_ethu_rx_rmc_counter_entry_size;
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
j2x_a0_dnx_data_nif_dbal_cdu_lane_fifo_level_and_occupancy_entry_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_dbal;
    int define_index = dnx_data_nif_dbal_define_cdu_lane_fifo_level_and_occupancy_entry_size;
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
j2x_a0_dnx_data_nif_features_txpi_supported_in_pm8x50_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_features;
    int feature_index = dnx_data_nif_features_txpi_supported_in_pm8x50;
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
j2x_a0_dnx_data_nif_arb_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int feature_index = dnx_data_nif_arb_is_supported;
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
j2x_a0_dnx_data_nif_arb_is_used_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int feature_index = dnx_data_nif_arb_is_used;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = dnx_data_nif.global.l1_only_mode_get(unit)  == DNX_PORT_NIF_L1_ONLY_MODE_DISABLED;

    
    feature->data = dnx_data_nif.global.l1_only_mode_get(unit)  == DNX_PORT_NIF_L1_ONLY_MODE_DISABLED;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2x_a0_dnx_data_nif_arb_nof_contexts_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_nof_contexts;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.phys.nof_phys_get(unit) + dnx_data_nif.flexe.nof_clients_get(unit);

    
    define->data = dnx_data_nif.phys.nof_phys_get(unit) + dnx_data_nif.flexe.nof_clients_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_nof_rx_qpms_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_nof_rx_qpms;
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
j2x_a0_dnx_data_nif_arb_nof_rx_ppms_in_qpm_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_nof_rx_ppms_in_qpm;
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
j2x_a0_dnx_data_nif_arb_nof_lanes_in_qpm_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_nof_lanes_in_qpm;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.arb.nof_rx_ppms_in_qpm_get(unit) * dnx_data_nif.eth.nof_lanes_in_cdu_get(unit);

    
    define->data = dnx_data_nif.arb.nof_rx_ppms_in_qpm_get(unit) * dnx_data_nif.eth.nof_lanes_in_cdu_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_nof_map_destinations_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_nof_map_destinations;
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
j2x_a0_dnx_data_nif_arb_max_calendar_length_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_max_calendar_length;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6800;

    
    define->data = 6800;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_nof_rx_sources_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_nof_rx_sources;
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
j2x_a0_dnx_data_nif_arb_rx_sch_calendar_nof_modes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_rx_sch_calendar_nof_modes;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.arb.nof_rx_sources_get(unit) * dnx_data_nif.arb.nof_map_destinations_get(unit);

    
    define->data = dnx_data_nif.arb.nof_rx_sources_get(unit) * dnx_data_nif.arb.nof_map_destinations_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_rx_qpm_calendar_length_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_rx_qpm_calendar_length;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 340;

    
    define->data = 340;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_rx_sch_calendar_length_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_rx_sch_calendar_length;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6800;

    
    define->data = 6800;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_cdpm_calendar_length_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_cdpm_calendar_length;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 80;

    
    define->data = 80;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_tmac_calendar_length_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_tmac_calendar_length;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6800;

    
    define->data = 6800;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_rx_qpm_calendar_nof_clients_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_rx_qpm_calendar_nof_clients;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.arb.nof_rx_ppms_in_qpm_get(unit) * dnx_data_nif.eth.nof_lanes_in_cdu_get(unit);

    
    define->data = dnx_data_nif.arb.nof_rx_ppms_in_qpm_get(unit) * dnx_data_nif.eth.nof_lanes_in_cdu_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_rx_sch_calendar_nof_clients_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_rx_sch_calendar_nof_clients;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.arb.nof_contexts_get(unit);

    
    define->data = dnx_data_nif.arb.nof_contexts_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_rx_qpm_port_speed_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_rx_qpm_port_speed_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5000;

    
    define->data = 5000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_rx_sch_port_speed_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_rx_sch_port_speed_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 250;

    
    define->data = 250;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_tx_eth_port_speed_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_tx_eth_port_speed_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5000;

    
    define->data = 5000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_tx_tmac_port_speed_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_tx_tmac_port_speed_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 250;

    
    define->data = 250;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_tx_tmac_nof_sections_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_tx_tmac_nof_sections;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 680;

    
    define->data = 680;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_tx_tmac_link_list_speed_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_tx_tmac_link_list_speed_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5000;

    
    define->data = 5000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_nof_entries_in_memory_row_for_rx_calendar_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_nof_entries_in_memory_row_for_rx_calendar;
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
j2x_a0_dnx_data_nif_arb_nof_bit_per_entry_in_rx_calendar_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_nof_bit_per_entry_in_rx_calendar;
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
j2x_a0_dnx_data_nif_arb_nof_entries_in_tmac_calendar_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_nof_entries_in_tmac_calendar;
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
j2x_a0_dnx_data_nif_arb_nof_bit_per_entry_in_tmac_calendar_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_nof_bit_per_entry_in_tmac_calendar;
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
j2x_a0_dnx_data_nif_arb_nof_bit_per_entry_in_ppm_calendar_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_nof_bit_per_entry_in_ppm_calendar;
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
j2x_a0_dnx_data_nif_arb_min_port_speed_for_link_list_section_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_min_port_speed_for_link_list_section;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3000;

    
    define->data = 3000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_link_list_sections_denominator_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_link_list_sections_denominator;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10000;

    
    define->data = 10000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2x_a0_dnx_data_nif_ofr_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ofr;
    int feature_index = dnx_data_nif_ofr_is_supported;
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
j2x_a0_dnx_data_nif_ofr_is_used_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ofr;
    int feature_index = dnx_data_nif_ofr_is_used;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = dnx_data_nif.global.l1_only_mode_get(unit)  == DNX_PORT_NIF_L1_ONLY_MODE_DISABLED || dnx_data_nif.global.l1_only_mode_get(unit)  == DNX_PORT_NIF_L1_ONLY_MODE_WITH_ILKN || dnx_data_fabric.general.connect_mode_get(unit) != DNX_FABRIC_CONNECT_MODE_SINGLE_FAP;

    
    feature->data = dnx_data_nif.global.l1_only_mode_get(unit)  == DNX_PORT_NIF_L1_ONLY_MODE_DISABLED || dnx_data_nif.global.l1_only_mode_get(unit)  == DNX_PORT_NIF_L1_ONLY_MODE_WITH_ILKN || dnx_data_fabric.general.connect_mode_get(unit) != DNX_FABRIC_CONNECT_MODE_SINGLE_FAP;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2x_a0_dnx_data_nif_ofr_nof_contexts_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ofr;
    int define_index = dnx_data_nif_ofr_define_nof_contexts;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.phys.nof_phys_get(unit) + dnx_data_nif.flexe.nof_clients_get(unit);

    
    define->data = dnx_data_nif.phys.nof_phys_get(unit) + dnx_data_nif.flexe.nof_clients_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ofr_nof_rmc_per_priority_group_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ofr;
    int define_index = dnx_data_nif_ofr_define_nof_rmc_per_priority_group;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 384;

    
    define->data = 384;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ofr_nof_total_rmcs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ofr;
    int define_index = dnx_data_nif_ofr_define_nof_total_rmcs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.ofr.nof_rmc_per_priority_group_get(unit) * dnx_data_nif.eth.priority_groups_nof_get(unit);

    
    define->data = dnx_data_nif.ofr.nof_rmc_per_priority_group_get(unit) * dnx_data_nif.eth.priority_groups_nof_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ofr_nof_rx_mem_sections_per_group_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ofr;
    int define_index = dnx_data_nif_ofr_define_nof_rx_mem_sections_per_group;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 160;

    
    define->data = 160;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ofr_nof_rx_memory_groups_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ofr;
    int define_index = dnx_data_nif_ofr_define_nof_rx_memory_groups;
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
j2x_a0_dnx_data_nif_ofr_nof_rx_memory_sections_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ofr;
    int define_index = dnx_data_nif_ofr_define_nof_rx_memory_sections;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.ofr.nof_rx_memory_groups_get(unit) * dnx_data_nif.ofr.nof_rx_mem_sections_per_group_get(unit);

    
    define->data = dnx_data_nif.ofr.nof_rx_memory_groups_get(unit) * dnx_data_nif.ofr.nof_rx_mem_sections_per_group_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ofr_nof_rx_memory_entries_per_section_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ofr;
    int define_index = dnx_data_nif_ofr_define_nof_rx_memory_entries_per_section;
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
j2x_a0_dnx_data_nif_ofr_rx_memory_link_list_speed_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ofr;
    int define_index = dnx_data_nif_ofr_define_rx_memory_link_list_speed_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5000;

    
    define->data = 5000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ofr_rx_sch_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ofr;
    int define_index = dnx_data_nif_ofr_define_rx_sch_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 250;

    
    define->data = 250;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2x_a0_dnx_data_nif_oft_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_oft;
    int feature_index = dnx_data_nif_oft_is_supported;
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
j2x_a0_dnx_data_nif_oft_is_used_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_oft;
    int feature_index = dnx_data_nif_oft_is_used;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = dnx_data_nif.global.l1_only_mode_get(unit) == DNX_PORT_NIF_L1_ONLY_MODE_DISABLED || dnx_data_nif.global.l1_only_mode_get(unit) == DNX_PORT_NIF_L1_ONLY_MODE_WITH_ILKN;

    
    feature->data = dnx_data_nif.global.l1_only_mode_get(unit) == DNX_PORT_NIF_L1_ONLY_MODE_DISABLED || dnx_data_nif.global.l1_only_mode_get(unit) == DNX_PORT_NIF_L1_ONLY_MODE_WITH_ILKN;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2x_a0_dnx_data_nif_oft_calendar_length_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_oft;
    int define_index = dnx_data_nif_oft_define_calendar_length;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6800;

    
    define->data = 6800;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_oft_nof_sections_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_oft;
    int define_index = dnx_data_nif_oft_define_nof_sections;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 640;

    
    define->data = 640;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_oft_nof_internal_calendar_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_oft;
    int define_index = dnx_data_nif_oft_define_nof_internal_calendar_entries;
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
j2x_a0_dnx_data_nif_oft_nof_bit_per_internal_entry_in_calendar_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_oft;
    int define_index = dnx_data_nif_oft_define_nof_bit_per_internal_entry_in_calendar;
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
j2x_a0_dnx_data_nif_oft_nof_contexts_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_oft;
    int define_index = dnx_data_nif_oft_define_nof_contexts;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.phys.nof_phys_get(unit) + dnx_data_nif.flexe.nof_clients_get(unit);

    
    define->data = dnx_data_nif.phys.nof_phys_get(unit) + dnx_data_nif.flexe.nof_clients_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_oft_port_speed_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_oft;
    int define_index = dnx_data_nif_oft_define_port_speed_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5000;

    
    define->data = 5000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_oft_calendar_speed_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_oft;
    int define_index = dnx_data_nif_oft_define_calendar_speed_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 250;

    
    define->data = 250;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
j2x_a0_data_nif_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_nif;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_nif_submodule_global;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_global_define_nof_lcplls;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_global_nof_lcplls_set;
    data_index = dnx_data_nif_global_define_nof_nif_interfaces_per_core;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_global_nof_nif_interfaces_per_core_set;
    data_index = dnx_data_nif_global_define_nif_interface_id_to_unit_id_granularity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_global_nif_interface_id_to_unit_id_granularity_set;
    data_index = dnx_data_nif_global_define_nof_nif_units_per_core;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_global_nof_nif_units_per_core_set;
    data_index = dnx_data_nif_global_define_reassembler_fifo_threshold;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_global_reassembler_fifo_threshold_set;
    data_index = dnx_data_nif_global_define_l1_only_mode;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_global_l1_only_mode_set;

    
    data_index = dnx_data_nif_global_reassembler;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_global_reassembler_set;
    data_index = dnx_data_nif_global_clock_power_down;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_global_clock_power_down_set;
    data_index = dnx_data_nif_global_is_shared_arch;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_global_is_shared_arch_set;
    data_index = dnx_data_nif_global_is_ilkn_after_flexe;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_global_is_ilkn_after_flexe_set;

    
    data_index = dnx_data_nif_global_table_nif_interface_id_to_unit_id;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_global_nif_interface_id_to_unit_id_set;
    
    submodule_index = dnx_data_nif_submodule_phys;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_phys_define_nof_phys;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_phys_nof_phys_set;
    data_index = dnx_data_nif_phys_define_max_phys_in_core;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_phys_max_phys_in_core_set;
    data_index = dnx_data_nif_phys_define_pm8x50_gen;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_phys_pm8x50_gen_set;

    

    
    data_index = dnx_data_nif_phys_table_general;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_phys_general_set;
    data_index = dnx_data_nif_phys_table_core_phys_map;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_phys_core_phys_map_set;
    data_index = dnx_data_nif_phys_table_vco_div;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_phys_vco_div_set;
    data_index = dnx_data_nif_phys_table_nof_phys_per_core;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_phys_nof_phys_per_core_set;
    
    submodule_index = dnx_data_nif_submodule_ilkn;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_ilkn_define_ilu_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_ilu_nof_set;
    data_index = dnx_data_nif_ilkn_define_ilkn_unit_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_ilkn_unit_nof_set;
    data_index = dnx_data_nif_ilkn_define_ilkn_unit_if_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_ilkn_unit_if_nof_set;
    data_index = dnx_data_nif_ilkn_define_fec_units_per_core_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_fec_units_per_core_nof_set;
    data_index = dnx_data_nif_ilkn_define_lanes_max_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_lanes_max_nof_set;
    data_index = dnx_data_nif_ilkn_define_lanes_max_nof_using_fec;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_lanes_max_nof_using_fec_set;
    data_index = dnx_data_nif_ilkn_define_lanes_allowed_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_lanes_allowed_nof_set;
    data_index = dnx_data_nif_ilkn_define_ilkn_over_eth_pms_max;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_ilkn_over_eth_pms_max_set;
    data_index = dnx_data_nif_ilkn_define_segments_max_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_segments_max_nof_set;
    data_index = dnx_data_nif_ilkn_define_segments_half_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_segments_half_nof_set;
    data_index = dnx_data_nif_ilkn_define_pms_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_pms_nof_set;
    data_index = dnx_data_nif_ilkn_define_fmac_bus_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_fmac_bus_size_set;
    data_index = dnx_data_nif_ilkn_define_ilkn_rx_hrf_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_ilkn_rx_hrf_nof_set;
    data_index = dnx_data_nif_ilkn_define_ilkn_tx_hrf_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_ilkn_tx_hrf_nof_set;
    data_index = dnx_data_nif_ilkn_define_data_rx_hrf_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_data_rx_hrf_size_set;
    data_index = dnx_data_nif_ilkn_define_tdm_rx_hrf_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_tdm_rx_hrf_size_set;
    data_index = dnx_data_nif_ilkn_define_tx_hrf_credits;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_tx_hrf_credits_set;
    data_index = dnx_data_nif_ilkn_define_watermark_high_data;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_watermark_high_data_set;
    data_index = dnx_data_nif_ilkn_define_watermark_low_data;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_watermark_low_data_set;
    data_index = dnx_data_nif_ilkn_define_pad_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_pad_size_set;
    data_index = dnx_data_nif_ilkn_define_burst_max_range_max;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_burst_max_range_max_set;
    data_index = dnx_data_nif_ilkn_define_burst_min;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_burst_min_set;
    data_index = dnx_data_nif_ilkn_define_burst_min_range_max;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_burst_min_range_max_set;
    data_index = dnx_data_nif_ilkn_define_port_0_status_intr_id;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_port_0_status_intr_id_set;
    data_index = dnx_data_nif_ilkn_define_port_1_status_intr_id;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_port_1_status_intr_id_set;
    data_index = dnx_data_nif_ilkn_define_interleaved_error_drop_single_context;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_interleaved_error_drop_single_context_set;

    
    data_index = dnx_data_nif_ilkn_is_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_ilkn_is_supported_set;
    data_index = dnx_data_nif_ilkn_is_elk_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_ilkn_is_elk_supported_set;
    data_index = dnx_data_nif_ilkn_over_fabric_per_core_selector;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_ilkn_over_fabric_per_core_selector_set;
    data_index = dnx_data_nif_ilkn_frmr_ilkn_selector;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_ilkn_frmr_ilkn_selector_set;
    data_index = dnx_data_nif_ilkn_support_burst_interleaving;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_ilkn_support_burst_interleaving_set;
    data_index = dnx_data_nif_ilkn_scheduler_context_per_port_support;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_ilkn_scheduler_context_per_port_support_set;
    data_index = dnx_data_nif_ilkn_support_ilu_burst_max;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_ilkn_support_ilu_burst_max_set;
    data_index = dnx_data_nif_ilkn_is_fec_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_ilkn_is_fec_supported_set;
    data_index = dnx_data_nif_ilkn_is_20p6G_speed_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_ilkn_is_20p6G_speed_supported_set;
    data_index = dnx_data_nif_ilkn_is_25G_speed_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_ilkn_is_25G_speed_supported_set;
    data_index = dnx_data_nif_ilkn_is_28p1G_speed_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_ilkn_is_28p1G_speed_supported_set;

    
    data_index = dnx_data_nif_ilkn_table_supported_phys;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_ilkn_supported_phys_set;
    data_index = dnx_data_nif_ilkn_table_supported_interfaces;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_ilkn_supported_interfaces_set;
    data_index = dnx_data_nif_ilkn_table_ilkn_cores;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_ilkn_ilkn_cores_set;
    data_index = dnx_data_nif_ilkn_table_supported_device_core;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_ilkn_supported_device_core_set;
    data_index = dnx_data_nif_ilkn_table_nif_pms;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_ilkn_nif_pms_set;
    data_index = dnx_data_nif_ilkn_table_fabric_pms;
    table = &submodule->tables[data_index];
    table->set = NULL;
    data_index = dnx_data_nif_ilkn_table_nif_lanes_map;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_ilkn_nif_lanes_map_set;
    data_index = dnx_data_nif_ilkn_table_start_tx_threshold_table;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_ilkn_start_tx_threshold_table_set;
    
    submodule_index = dnx_data_nif_submodule_eth;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_eth_define_cdu_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_eth_cdu_nof_set;
    data_index = dnx_data_nif_eth_define_clu_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_eth_clu_nof_set;
    data_index = dnx_data_nif_eth_define_cdum_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_eth_cdum_nof_set;
    data_index = dnx_data_nif_eth_define_nof_pms_in_cdu;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_eth_nof_pms_in_cdu_set;
    data_index = dnx_data_nif_eth_define_nof_lanes_in_cdu;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_eth_nof_lanes_in_cdu_set;
    data_index = dnx_data_nif_eth_define_cdu_mac_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_eth_cdu_mac_nof_set;
    data_index = dnx_data_nif_eth_define_mac_lanes_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_eth_mac_lanes_nof_set;
    data_index = dnx_data_nif_eth_define_cdu_memory_entries_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_eth_cdu_memory_entries_nof_set;
    data_index = dnx_data_nif_eth_define_priority_groups_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_eth_priority_groups_nof_set;
    data_index = dnx_data_nif_eth_define_phy_map_granularity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_eth_phy_map_granularity_set;

    
    data_index = dnx_data_nif_eth_gear_shifter_exists;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_eth_gear_shifter_exists_set;

    
    data_index = dnx_data_nif_eth_table_pm_properties;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_eth_pm_properties_set;
    data_index = dnx_data_nif_eth_table_ethu_properties;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_eth_ethu_properties_set;
    data_index = dnx_data_nif_eth_table_phy_map;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_eth_phy_map_set;
    data_index = dnx_data_nif_eth_table_supported_interfaces;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_eth_supported_interfaces_set;
    
    submodule_index = dnx_data_nif_submodule_simulator;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_simulator_define_cdu_type;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_simulator_cdu_type_set;

    

    
    
    submodule_index = dnx_data_nif_submodule_flexe;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_flexe_define_nof_clients;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_flexe_nof_clients_set;

    

    
    
    submodule_index = dnx_data_nif_submodule_prd;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_prd_define_tcam_key_offset_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_prd_tcam_key_offset_size_set;
    data_index = dnx_data_nif_prd_define_rmc_threshold_max;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_prd_rmc_threshold_max_set;
    data_index = dnx_data_nif_prd_define_nof_ofr_port_profiles;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_prd_nof_ofr_port_profiles_set;
    data_index = dnx_data_nif_prd_define_nof_ofr_nif_interfaces;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_prd_nof_ofr_nif_interfaces_set;

    

    
    
    submodule_index = dnx_data_nif_submodule_portmod;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_portmod_define_pm_types_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_portmod_pm_types_nof_set;

    

    
    data_index = dnx_data_nif_portmod_table_pm_types_and_interfaces;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_portmod_pm_types_and_interfaces_set;
    
    submodule_index = dnx_data_nif_submodule_dbal;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_dbal_define_cdu_rx_rmc_entry_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_dbal_cdu_rx_rmc_entry_size_set;
    data_index = dnx_data_nif_dbal_define_cdu_tx_start_threshold_entry_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_dbal_cdu_tx_start_threshold_entry_size_set;
    data_index = dnx_data_nif_dbal_define_ilu_burst_min_entry_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_dbal_ilu_burst_min_entry_size_set;
    data_index = dnx_data_nif_dbal_define_ethu_rx_rmc_counter_entry_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_dbal_ethu_rx_rmc_counter_entry_size_set;
    data_index = dnx_data_nif_dbal_define_cdu_lane_fifo_level_and_occupancy_entry_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_dbal_cdu_lane_fifo_level_and_occupancy_entry_size_set;

    

    
    
    submodule_index = dnx_data_nif_submodule_features;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_nif_features_txpi_supported_in_pm8x50;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_features_txpi_supported_in_pm8x50_set;

    
    
    submodule_index = dnx_data_nif_submodule_arb;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_arb_define_nof_contexts;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_nof_contexts_set;
    data_index = dnx_data_nif_arb_define_nof_rx_qpms;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_nof_rx_qpms_set;
    data_index = dnx_data_nif_arb_define_nof_rx_ppms_in_qpm;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_nof_rx_ppms_in_qpm_set;
    data_index = dnx_data_nif_arb_define_nof_lanes_in_qpm;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_nof_lanes_in_qpm_set;
    data_index = dnx_data_nif_arb_define_nof_map_destinations;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_nof_map_destinations_set;
    data_index = dnx_data_nif_arb_define_max_calendar_length;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_max_calendar_length_set;
    data_index = dnx_data_nif_arb_define_nof_rx_sources;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_nof_rx_sources_set;
    data_index = dnx_data_nif_arb_define_rx_sch_calendar_nof_modes;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_rx_sch_calendar_nof_modes_set;
    data_index = dnx_data_nif_arb_define_rx_qpm_calendar_length;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_rx_qpm_calendar_length_set;
    data_index = dnx_data_nif_arb_define_rx_sch_calendar_length;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_rx_sch_calendar_length_set;
    data_index = dnx_data_nif_arb_define_cdpm_calendar_length;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_cdpm_calendar_length_set;
    data_index = dnx_data_nif_arb_define_tmac_calendar_length;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_tmac_calendar_length_set;
    data_index = dnx_data_nif_arb_define_rx_qpm_calendar_nof_clients;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_rx_qpm_calendar_nof_clients_set;
    data_index = dnx_data_nif_arb_define_rx_sch_calendar_nof_clients;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_rx_sch_calendar_nof_clients_set;
    data_index = dnx_data_nif_arb_define_rx_qpm_port_speed_granularity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_rx_qpm_port_speed_granularity_set;
    data_index = dnx_data_nif_arb_define_rx_sch_port_speed_granularity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_rx_sch_port_speed_granularity_set;
    data_index = dnx_data_nif_arb_define_tx_eth_port_speed_granularity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_tx_eth_port_speed_granularity_set;
    data_index = dnx_data_nif_arb_define_tx_tmac_port_speed_granularity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_tx_tmac_port_speed_granularity_set;
    data_index = dnx_data_nif_arb_define_tx_tmac_nof_sections;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_tx_tmac_nof_sections_set;
    data_index = dnx_data_nif_arb_define_tx_tmac_link_list_speed_granularity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_tx_tmac_link_list_speed_granularity_set;
    data_index = dnx_data_nif_arb_define_nof_entries_in_memory_row_for_rx_calendar;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_nof_entries_in_memory_row_for_rx_calendar_set;
    data_index = dnx_data_nif_arb_define_nof_bit_per_entry_in_rx_calendar;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_nof_bit_per_entry_in_rx_calendar_set;
    data_index = dnx_data_nif_arb_define_nof_entries_in_tmac_calendar;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_nof_entries_in_tmac_calendar_set;
    data_index = dnx_data_nif_arb_define_nof_bit_per_entry_in_tmac_calendar;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_nof_bit_per_entry_in_tmac_calendar_set;
    data_index = dnx_data_nif_arb_define_nof_bit_per_entry_in_ppm_calendar;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_nof_bit_per_entry_in_ppm_calendar_set;
    data_index = dnx_data_nif_arb_define_min_port_speed_for_link_list_section;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_min_port_speed_for_link_list_section_set;
    data_index = dnx_data_nif_arb_define_link_list_sections_denominator;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_link_list_sections_denominator_set;

    
    data_index = dnx_data_nif_arb_is_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_arb_is_supported_set;
    data_index = dnx_data_nif_arb_is_used;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_arb_is_used_set;

    
    
    submodule_index = dnx_data_nif_submodule_ofr;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_ofr_define_nof_contexts;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ofr_nof_contexts_set;
    data_index = dnx_data_nif_ofr_define_nof_rmc_per_priority_group;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ofr_nof_rmc_per_priority_group_set;
    data_index = dnx_data_nif_ofr_define_nof_total_rmcs;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ofr_nof_total_rmcs_set;
    data_index = dnx_data_nif_ofr_define_nof_rx_mem_sections_per_group;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ofr_nof_rx_mem_sections_per_group_set;
    data_index = dnx_data_nif_ofr_define_nof_rx_memory_groups;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ofr_nof_rx_memory_groups_set;
    data_index = dnx_data_nif_ofr_define_nof_rx_memory_sections;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ofr_nof_rx_memory_sections_set;
    data_index = dnx_data_nif_ofr_define_nof_rx_memory_entries_per_section;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ofr_nof_rx_memory_entries_per_section_set;
    data_index = dnx_data_nif_ofr_define_rx_memory_link_list_speed_granularity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ofr_rx_memory_link_list_speed_granularity_set;
    data_index = dnx_data_nif_ofr_define_rx_sch_granularity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ofr_rx_sch_granularity_set;

    
    data_index = dnx_data_nif_ofr_is_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_ofr_is_supported_set;
    data_index = dnx_data_nif_ofr_is_used;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_ofr_is_used_set;

    
    
    submodule_index = dnx_data_nif_submodule_oft;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_oft_define_calendar_length;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_oft_calendar_length_set;
    data_index = dnx_data_nif_oft_define_nof_sections;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_oft_nof_sections_set;
    data_index = dnx_data_nif_oft_define_nof_internal_calendar_entries;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_oft_nof_internal_calendar_entries_set;
    data_index = dnx_data_nif_oft_define_nof_bit_per_internal_entry_in_calendar;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_oft_nof_bit_per_internal_entry_in_calendar_set;
    data_index = dnx_data_nif_oft_define_nof_contexts;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_oft_nof_contexts_set;
    data_index = dnx_data_nif_oft_define_port_speed_granularity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_oft_port_speed_granularity_set;
    data_index = dnx_data_nif_oft_define_calendar_speed_granularity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_oft_calendar_speed_granularity_set;

    
    data_index = dnx_data_nif_oft_is_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_oft_is_supported_set;
    data_index = dnx_data_nif_oft_is_used;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_oft_is_used_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

