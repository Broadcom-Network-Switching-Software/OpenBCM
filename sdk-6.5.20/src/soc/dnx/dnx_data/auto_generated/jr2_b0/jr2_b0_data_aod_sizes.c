

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COMMON

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_aod_sizes.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>








static shr_error_e
jr2_b0_dnx_data_aod_sizes_AOD_count_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_aod_sizes;
    int submodule_index = dnx_data_aod_sizes_submodule_AOD;
    int define_index = dnx_data_aod_sizes_AOD_define_count;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 336;

    
    define->data = 336;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_b0_dnx_data_aod_sizes_AOD_sizes_set(
    int unit)
{
    int index_index;
    dnx_data_aod_sizes_AOD_sizes_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_aod_sizes;
    int submodule_index = dnx_data_aod_sizes_submodule_AOD;
    int table_index = dnx_data_aod_sizes_AOD_table_sizes;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_aod_sizes.AOD.count_get(unit);
    table->info_get.key_size[0] = dnx_data_aod_sizes.AOD.count_get(unit);

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_aod_sizes_AOD_sizes_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_aod_sizes_AOD_table_sizes");

    
    default_data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->value = 0;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->value = 6;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->value = 1;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->value = 1;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->value = 1;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->value = 1;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->value = 1;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->value = 1;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->value = 1;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->value = 1;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->value = 1;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->value = 1;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->value = 1;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->value = 1;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->value = 1;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->value = 1;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->value = 1;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->value = 1;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->value = 1;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->value = 1;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->value = 1;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->value = 1;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->value = 1;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->value = 0;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->value = 1;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->value = 1;
    }
    if (25 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, 0);
        data->value = 1;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->value = 1;
    }
    if (27 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, 0);
        data->value = 1;
    }
    if (28 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 28, 0);
        data->value = 1;
    }
    if (29 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 29, 0);
        data->value = 1;
    }
    if (30 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 30, 0);
        data->value = 1;
    }
    if (31 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 31, 0);
        data->value = 1;
    }
    if (32 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 32, 0);
        data->value = 1;
    }
    if (33 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 33, 0);
        data->value = 1;
    }
    if (34 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 34, 0);
        data->value = 1;
    }
    if (35 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 35, 0);
        data->value = 1;
    }
    if (36 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 36, 0);
        data->value = 1;
    }
    if (37 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 37, 0);
        data->value = 2;
    }
    if (38 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 38, 0);
        data->value = 1;
    }
    if (39 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 39, 0);
        data->value = 1;
    }
    if (40 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 40, 0);
        data->value = 1;
    }
    if (41 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 41, 0);
        data->value = 1;
    }
    if (42 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 42, 0);
        data->value = 1;
    }
    if (43 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 43, 0);
        data->value = 1;
    }
    if (44 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 44, 0);
        data->value = 1;
    }
    if (45 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 45, 0);
        data->value = 8;
    }
    if (46 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 46, 0);
        data->value = 3;
    }
    if (47 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 47, 0);
        data->value = 16;
    }
    if (48 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 48, 0);
        data->value = 8;
    }
    if (49 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 49, 0);
        data->value = 8;
    }
    if (50 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 50, 0);
        data->value = 1;
    }
    if (51 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 51, 0);
        data->value = 1;
    }
    if (52 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 52, 0);
        data->value = 1;
    }
    if (53 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 53, 0);
        data->value = 1;
    }
    if (54 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 54, 0);
        data->value = 1;
    }
    if (55 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 55, 0);
        data->value = 1;
    }
    if (56 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 56, 0);
        data->value = 1;
    }
    if (57 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 57, 0);
        data->value = 1;
    }
    if (58 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 58, 0);
        data->value = 2;
    }
    if (59 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 59, 0);
        data->value = 0;
    }
    if (60 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 60, 0);
        data->value = 2;
    }
    if (61 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 61, 0);
        data->value = 3;
    }
    if (62 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 62, 0);
        data->value = 6;
    }
    if (63 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 63, 0);
        data->value = 2;
    }
    if (64 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 64, 0);
        data->value = 5;
    }
    if (65 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 65, 0);
        data->value = 5;
    }
    if (66 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 66, 0);
        data->value = 5;
    }
    if (67 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 67, 0);
        data->value = 1;
    }
    if (68 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 68, 0);
        data->value = 1;
    }
    if (69 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 69, 0);
        data->value = 1;
    }
    if (70 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 70, 0);
        data->value = 1;
    }
    if (71 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 71, 0);
        data->value = 1;
    }
    if (72 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 72, 0);
        data->value = 5;
    }
    if (73 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 73, 0);
        data->value = 4;
    }
    if (74 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 74, 0);
        data->value = 4;
    }
    if (75 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 75, 0);
        data->value = 16;
    }
    if (76 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 76, 0);
        data->value = 0;
    }
    if (77 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 77, 0);
        data->value = 0;
    }
    if (78 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 78, 0);
        data->value = 0;
    }
    if (79 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 79, 0);
        data->value = 0;
    }
    if (80 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 80, 0);
        data->value = 0;
    }
    if (81 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 81, 0);
        data->value = 0;
    }
    if (82 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 82, 0);
        data->value = 0;
    }
    if (83 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 83, 0);
        data->value = 0;
    }
    if (84 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 84, 0);
        data->value = 0;
    }
    if (85 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 85, 0);
        data->value = 0;
    }
    if (86 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 86, 0);
        data->value = 0;
    }
    if (87 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 87, 0);
        data->value = 0;
    }
    if (88 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 88, 0);
        data->value = 0;
    }
    if (89 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 89, 0);
        data->value = 0;
    }
    if (90 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 90, 0);
        data->value = 0;
    }
    if (91 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 91, 0);
        data->value = 0;
    }
    if (92 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 92, 0);
        data->value = 0;
    }
    if (93 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 93, 0);
        data->value = 0;
    }
    if (94 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 94, 0);
        data->value = 0;
    }
    if (95 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 95, 0);
        data->value = 4;
    }
    if (96 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 96, 0);
        data->value = 5;
    }
    if (97 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 97, 0);
        data->value = 4;
    }
    if (98 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 98, 0);
        data->value = 1;
    }
    if (99 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 99, 0);
        data->value = 2;
    }
    if (100 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 100, 0);
        data->value = 2;
    }
    if (101 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 101, 0);
        data->value = 3;
    }
    if (102 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 102, 0);
        data->value = 3;
    }
    if (103 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 103, 0);
        data->value = 4;
    }
    if (104 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 104, 0);
        data->value = 4;
    }
    if (105 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 105, 0);
        data->value = 4;
    }
    if (106 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 106, 0);
        data->value = 6;
    }
    if (107 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 107, 0);
        data->value = 5;
    }
    if (108 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 108, 0);
        data->value = 5;
    }
    if (109 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 109, 0);
        data->value = 3;
    }
    if (110 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 110, 0);
        data->value = 16;
    }
    if (111 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 111, 0);
        data->value = 5;
    }
    if (112 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 112, 0);
        data->value = 5;
    }
    if (113 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 113, 0);
        data->value = 5;
    }
    if (114 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 114, 0);
        data->value = 2;
    }
    if (115 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 115, 0);
        data->value = 6;
    }
    if (116 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 116, 0);
        data->value = 1;
    }
    if (117 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 117, 0);
        data->value = 2;
    }
    if (118 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 118, 0);
        data->value = 0;
    }
    if (119 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 119, 0);
        data->value = 0;
    }
    if (120 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 120, 0);
        data->value = 0;
    }
    if (121 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 121, 0);
        data->value = 0;
    }
    if (122 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 122, 0);
        data->value = 1;
    }
    if (123 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 123, 0);
        data->value = 1;
    }
    if (124 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 124, 0);
        data->value = 1;
    }
    if (125 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 125, 0);
        data->value = 1;
    }
    if (126 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 126, 0);
        data->value = 1;
    }
    if (127 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 127, 0);
        data->value = 1;
    }
    if (128 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 128, 0);
        data->value = 2;
    }
    if (129 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 129, 0);
        data->value = 1;
    }
    if (130 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 130, 0);
        data->value = 3;
    }
    if (131 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 131, 0);
        data->value = 3;
    }
    if (132 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 132, 0);
        data->value = 4;
    }
    if (133 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 133, 0);
        data->value = 4;
    }
    if (134 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 134, 0);
        data->value = 4;
    }
    if (135 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 135, 0);
        data->value = 4;
    }
    if (136 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 136, 0);
        data->value = 4;
    }
    if (137 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 137, 0);
        data->value = 4;
    }
    if (138 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 138, 0);
        data->value = 4;
    }
    if (139 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 139, 0);
        data->value = 1;
    }
    if (140 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 140, 0);
        data->value = 5;
    }
    if (141 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 141, 0);
        data->value = 6;
    }
    if (142 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 142, 0);
        data->value = 6;
    }
    if (143 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 143, 0);
        data->value = 5;
    }
    if (144 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 144, 0);
        data->value = 5;
    }
    if (145 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 145, 0);
        data->value = 5;
    }
    if (146 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 146, 0);
        data->value = 3;
    }
    if (147 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 147, 0);
        data->value = 1;
    }
    if (148 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 148, 0);
        data->value = 3;
    }
    if (149 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 149, 0);
        data->value = 1;
    }
    if (150 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 150, 0);
        data->value = 1;
    }
    if (151 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 151, 0);
        data->value = 1;
    }
    if (152 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 152, 0);
        data->value = 4;
    }
    if (153 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 153, 0);
        data->value = 8;
    }
    if (154 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 154, 0);
        data->value = 8;
    }
    if (155 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 155, 0);
        data->value = 8;
    }
    if (156 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 156, 0);
        data->value = 1;
    }
    if (157 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 157, 0);
        data->value = 4;
    }
    if (158 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 158, 0);
        data->value = 1;
    }
    if (159 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 159, 0);
        data->value = 2;
    }
    if (160 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 160, 0);
        data->value = 1;
    }
    if (161 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 161, 0);
        data->value = 4;
    }
    if (162 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 162, 0);
        data->value = 2;
    }
    if (163 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 163, 0);
        data->value = 4;
    }
    if (164 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 164, 0);
        data->value = 1;
    }
    if (165 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 165, 0);
        data->value = 1;
    }
    if (166 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 166, 0);
        data->value = 1;
    }
    if (167 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 167, 0);
        data->value = 1;
    }
    if (168 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 168, 0);
        data->value = 4;
    }
    if (169 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 169, 0);
        data->value = 1;
    }
    if (170 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 170, 0);
        data->value = 1;
    }
    if (171 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 171, 0);
        data->value = 4;
    }
    if (172 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 172, 0);
        data->value = 4;
    }
    if (173 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 173, 0);
        data->value = 5;
    }
    if (174 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 174, 0);
        data->value = 4;
    }
    if (175 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 175, 0);
        data->value = 4;
    }
    if (176 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 176, 0);
        data->value = 3;
    }
    if (177 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 177, 0);
        data->value = 4;
    }
    if (178 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 178, 0);
        data->value = 2;
    }
    if (179 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 179, 0);
        data->value = 2;
    }
    if (180 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 180, 0);
        data->value = 2;
    }
    if (181 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 181, 0);
        data->value = 2;
    }
    if (182 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 182, 0);
        data->value = 2;
    }
    if (183 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 183, 0);
        data->value = 4;
    }
    if (184 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 184, 0);
        data->value = 2;
    }
    if (185 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 185, 0);
        data->value = 4;
    }
    if (186 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 186, 0);
        data->value = 4;
    }
    if (187 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 187, 0);
        data->value = 4;
    }
    if (188 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 188, 0);
        data->value = 4;
    }
    if (189 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 189, 0);
        data->value = 4;
    }
    if (190 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 190, 0);
        data->value = 4;
    }
    if (191 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 191, 0);
        data->value = 4;
    }
    if (192 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 192, 0);
        data->value = 3;
    }
    if (193 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 193, 0);
        data->value = 3;
    }
    if (194 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 194, 0);
        data->value = 3;
    }
    if (195 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 195, 0);
        data->value = 2;
    }
    if (196 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 196, 0);
        data->value = 2;
    }
    if (197 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 197, 0);
        data->value = 2;
    }
    if (198 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 198, 0);
        data->value = 2;
    }
    if (199 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 199, 0);
        data->value = 2;
    }
    if (200 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 200, 0);
        data->value = 2;
    }
    if (201 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 201, 0);
        data->value = 4;
    }
    if (202 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 202, 0);
        data->value = 4;
    }
    if (203 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 203, 0);
        data->value = 2;
    }
    if (204 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 204, 0);
        data->value = 4;
    }
    if (205 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 205, 0);
        data->value = 2;
    }
    if (206 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 206, 0);
        data->value = 5;
    }
    if (207 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 207, 0);
        data->value = 5;
    }
    if (208 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 208, 0);
        data->value = 4;
    }
    if (209 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 209, 0);
        data->value = 5;
    }
    if (210 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 210, 0);
        data->value = 5;
    }
    if (211 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 211, 0);
        data->value = 5;
    }
    if (212 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 212, 0);
        data->value = 5;
    }
    if (213 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 213, 0);
        data->value = 4;
    }
    if (214 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 214, 0);
        data->value = 4;
    }
    if (215 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 215, 0);
        data->value = 4;
    }
    if (216 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 216, 0);
        data->value = 3;
    }
    if (217 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 217, 0);
        data->value = 4;
    }
    if (218 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 218, 0);
        data->value = 3;
    }
    if (219 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 219, 0);
        data->value = 2;
    }
    if (220 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 220, 0);
        data->value = 3;
    }
    if (221 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 221, 0);
        data->value = 2;
    }
    if (222 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 222, 0);
        data->value = 6;
    }
    if (223 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 223, 0);
        data->value = 24;
    }
    if (224 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 224, 0);
        data->value = 24;
    }
    if (225 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 225, 0);
        data->value = 1;
    }
    if (226 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 226, 0);
        data->value = 1;
    }
    if (227 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 227, 0);
        data->value = 1;
    }
    if (228 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 228, 0);
        data->value = 1;
    }
    if (229 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 229, 0);
        data->value = 1;
    }
    if (230 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 230, 0);
        data->value = 1;
    }
    if (231 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 231, 0);
        data->value = 1;
    }
    if (232 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 232, 0);
        data->value = 1;
    }
    if (233 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 233, 0);
        data->value = 1;
    }
    if (234 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 234, 0);
        data->value = 1;
    }
    if (235 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 235, 0);
        data->value = 1;
    }
    if (236 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 236, 0);
        data->value = 1;
    }
    if (237 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 237, 0);
        data->value = 1;
    }
    if (238 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 238, 0);
        data->value = 1;
    }
    if (239 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 239, 0);
        data->value = 1;
    }
    if (240 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 240, 0);
        data->value = 1;
    }
    if (241 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 241, 0);
        data->value = 1;
    }
    if (242 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 242, 0);
        data->value = 1;
    }
    if (243 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 243, 0);
        data->value = 1;
    }
    if (244 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 244, 0);
        data->value = 1;
    }
    if (245 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 245, 0);
        data->value = 1;
    }
    if (246 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 246, 0);
        data->value = 1;
    }
    if (247 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 247, 0);
        data->value = 1;
    }
    if (248 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 248, 0);
        data->value = 1;
    }
    if (249 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 249, 0);
        data->value = 1;
    }
    if (250 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 250, 0);
        data->value = 1;
    }
    if (251 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 251, 0);
        data->value = 1;
    }
    if (252 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 252, 0);
        data->value = 1;
    }
    if (253 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 253, 0);
        data->value = 1;
    }
    if (254 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 254, 0);
        data->value = 1;
    }
    if (255 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 255, 0);
        data->value = 1;
    }
    if (256 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 256, 0);
        data->value = 1;
    }
    if (257 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 257, 0);
        data->value = 1;
    }
    if (258 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 258, 0);
        data->value = 1;
    }
    if (259 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 259, 0);
        data->value = 1;
    }
    if (260 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 260, 0);
        data->value = 1;
    }
    if (261 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 261, 0);
        data->value = 1;
    }
    if (262 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 262, 0);
        data->value = 1;
    }
    if (263 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 263, 0);
        data->value = 1;
    }
    if (264 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 264, 0);
        data->value = 1;
    }
    if (265 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 265, 0);
        data->value = 1;
    }
    if (266 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 266, 0);
        data->value = 1;
    }
    if (267 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 267, 0);
        data->value = 1;
    }
    if (268 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 268, 0);
        data->value = 1;
    }
    if (269 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 269, 0);
        data->value = 1;
    }
    if (270 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 270, 0);
        data->value = 1;
    }
    if (271 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 271, 0);
        data->value = 1;
    }
    if (272 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 272, 0);
        data->value = 1;
    }
    if (273 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 273, 0);
        data->value = 1;
    }
    if (274 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 274, 0);
        data->value = 1;
    }
    if (275 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 275, 0);
        data->value = 1;
    }
    if (276 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 276, 0);
        data->value = 1;
    }
    if (277 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 277, 0);
        data->value = 1;
    }
    if (278 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 278, 0);
        data->value = 1;
    }
    if (279 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 279, 0);
        data->value = 1;
    }
    if (280 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 280, 0);
        data->value = 3;
    }
    if (281 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 281, 0);
        data->value = 6;
    }
    if (282 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 282, 0);
        data->value = 1;
    }
    if (283 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 283, 0);
        data->value = 1;
    }
    if (284 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 284, 0);
        data->value = 1;
    }
    if (285 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 285, 0);
        data->value = 6;
    }
    if (286 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 286, 0);
        data->value = 0;
    }
    if (287 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 287, 0);
        data->value = 1;
    }
    if (288 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 288, 0);
        data->value = 1;
    }
    if (289 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 289, 0);
        data->value = 1;
    }
    if (290 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 290, 0);
        data->value = 1;
    }
    if (291 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 291, 0);
        data->value = 1;
    }
    if (292 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 292, 0);
        data->value = 1;
    }
    if (293 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 293, 0);
        data->value = 0;
    }
    if (294 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 294, 0);
        data->value = 0;
    }
    if (295 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 295, 0);
        data->value = 1;
    }
    if (296 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 296, 0);
        data->value = 1;
    }
    if (297 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 297, 0);
        data->value = 4;
    }
    if (298 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 298, 0);
        data->value = 1;
    }
    if (299 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 299, 0);
        data->value = 5;
    }
    if (300 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 300, 0);
        data->value = 1;
    }
    if (301 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 301, 0);
        data->value = 2;
    }
    if (302 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 302, 0);
        data->value = 1;
    }
    if (303 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 303, 0);
        data->value = 1;
    }
    if (304 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 304, 0);
        data->value = 1;
    }
    if (305 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 305, 0);
        data->value = 1;
    }
    if (306 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 306, 0);
        data->value = 2;
    }
    if (307 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 307, 0);
        data->value = 1;
    }
    if (308 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 308, 0);
        data->value = 1;
    }
    if (309 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 309, 0);
        data->value = 1;
    }
    if (310 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 310, 0);
        data->value = 3;
    }
    if (311 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 311, 0);
        data->value = 1;
    }
    if (312 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 312, 0);
        data->value = 1;
    }
    if (313 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 313, 0);
        data->value = 1;
    }
    if (314 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 314, 0);
        data->value = 1;
    }
    if (315 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 315, 0);
        data->value = 1;
    }
    if (316 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 316, 0);
        data->value = 1;
    }
    if (317 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 317, 0);
        data->value = 4;
    }
    if (318 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 318, 0);
        data->value = 1;
    }
    if (319 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 319, 0);
        data->value = 1;
    }
    if (320 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 320, 0);
        data->value = 1;
    }
    if (321 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 321, 0);
        data->value = 1;
    }
    if (322 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 322, 0);
        data->value = 0;
    }
    if (323 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 323, 0);
        data->value = 16;
    }
    if (324 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 324, 0);
        data->value = 2;
    }
    if (325 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 325, 0);
        data->value = 0;
    }
    if (326 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 326, 0);
        data->value = 0;
    }
    if (327 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 327, 0);
        data->value = 0;
    }
    if (328 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 328, 0);
        data->value = 0;
    }
    if (329 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 329, 0);
        data->value = 0;
    }
    if (330 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 330, 0);
        data->value = 2;
    }
    if (331 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 331, 0);
        data->value = 2;
    }
    if (332 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 332, 0);
        data->value = 2;
    }
    if (333 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 333, 0);
        data->value = 2;
    }
    if (334 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 334, 0);
        data->value = 2;
    }
    if (335 < table->keys[0].size)
    {
        data = (dnx_data_aod_sizes_AOD_sizes_t *) dnxc_data_mgmt_table_data_get(unit, table, 335, 0);
        data->value = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
jr2_b0_data_aod_sizes_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_aod_sizes;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_aod_sizes_submodule_AOD;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_aod_sizes_AOD_define_count;
    define = &submodule->defines[data_index];
    define->set = jr2_b0_dnx_data_aod_sizes_AOD_count_set;

    

    
    data_index = dnx_data_aod_sizes_AOD_table_sizes;
    table = &submodule->tables[data_index];
    table->set = jr2_b0_dnx_data_aod_sizes_AOD_sizes_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

