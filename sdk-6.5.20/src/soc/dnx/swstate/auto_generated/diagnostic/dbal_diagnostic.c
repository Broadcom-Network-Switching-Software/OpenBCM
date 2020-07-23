
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnxc/swstate/dnxc_sw_state_c_includes.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dbal_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_dbal_sw_state_t * dbal_db_data[SOC_MAX_NUM_DEVICES];



int
dbal_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dbal_db_sw_tables_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dbal_db_mdb_access_dump(unit, filters));
    SHR_IF_ERR_EXIT(dbal_db_mdb_phy_tables_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dbal_db_tables_properties_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dbal_db_dbal_dynamic_tables_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dbal_db_dbal_dynamic_xml_tables_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dbal_db_dbal_fields_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dbal_db_dbal_ppmc_table_res_types_dump(unit, filters));
    SHR_IF_ERR_EXIT(dbal_db_hw_ent_direct_map_hash_tbl_dump(unit, filters));
    SHR_IF_ERR_EXIT(dbal_db_hw_ent_groups_map_hash_tbl_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_sw_tables_dump(int  unit,  int  sw_tables_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dbal_db_sw_tables_table_type_dump(unit, sw_tables_idx_0, filters));
    SHR_IF_ERR_EXIT(dbal_db_sw_tables_entries_dump(unit, sw_tables_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dbal_db_sw_tables_hash_table_dump(unit, sw_tables_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_sw_tables_table_type_dump(int  unit,  int  sw_tables_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = sw_tables_idx_0, I0 = sw_tables_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dbal_sw_state_table_type_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal sw_tables table_type") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal sw_tables table_type\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/sw_tables/table_type.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","sw_tables[].table_type: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_TABLES)
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->sw_tables[].table_type: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[i0].table_type,
                "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","sw_tables[%s%d].table_type: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[i0].table_type,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_sw_tables_entries_dump(int  unit,  int  sw_tables_idx_0,  int  entries_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = sw_tables_idx_0, I0 = sw_tables_idx_0 + 1;
    int i1 = entries_idx_0, I1 = entries_idx_0 + 1, org_i1 = entries_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "SHR_BITDCL") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal sw_tables entries") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal sw_tables entries\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/sw_tables/entries.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","sw_tables[].entries[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_TABLES)
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->sw_tables[].entries[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[i0].entries);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DBAL_MODULE_ID,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[i0].entries);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[i0].entries
                    , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[i0].entries), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[i0].entries))
            {
                LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->sw_tables[].entries[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[i0].entries) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","sw_tables[%s%d].entries[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_shr_bitdcl(
                    unit,
                    &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[i0].entries[i1]);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_sw_tables_hash_table_dump(int  unit,  int  sw_tables_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = sw_tables_idx_0, I0 = sw_tables_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_htbl_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal sw_tables hash_table") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal sw_tables hash_table\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/sw_tables/hash_table.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","sw_tables[].hash_table: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_TABLES)
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->sw_tables[].hash_table: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","sw_tables[%s%d].hash_table: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            SW_STATE_HTB_DEFAULT_PRINT(
                unit,
                DBAL_MODULE_ID,
                &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[i0].hash_table);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_mdb_access_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dbal_db_mdb_access_skip_read_from_shadow_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_mdb_access_skip_read_from_shadow_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal mdb_access skip_read_from_shadow") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal mdb_access skip_read_from_shadow\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/mdb_access/skip_read_from_shadow.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","mdb_access.skip_read_from_shadow: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","mdb_access.skip_read_from_shadow: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint8(
            unit,
            &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->mdb_access.skip_read_from_shadow);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_mdb_phy_tables_dump(int  unit,  int  mdb_phy_tables_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dbal_db_mdb_phy_tables_nof_entries_dump(unit, mdb_phy_tables_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_mdb_phy_tables_nof_entries_dump(int  unit,  int  mdb_phy_tables_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = mdb_phy_tables_idx_0, I0 = mdb_phy_tables_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal mdb_phy_tables nof_entries") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal mdb_phy_tables nof_entries\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/mdb_phy_tables/nof_entries.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","mdb_phy_tables[].nof_entries: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_PHYSICAL_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->mdb_phy_tables
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->mdb_phy_tables), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_PHYSICAL_TABLES)
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->mdb_phy_tables[].nof_entries: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_PHYSICAL_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","mdb_phy_tables[%s%d].nof_entries: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->mdb_phy_tables[i0].nof_entries);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_tables_properties_dump(int  unit,  int  tables_properties_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dbal_db_tables_properties_table_status_dump(unit, tables_properties_idx_0, filters));
    SHR_IF_ERR_EXIT(dbal_db_tables_properties_nof_set_operations_dump(unit, tables_properties_idx_0, filters));
    SHR_IF_ERR_EXIT(dbal_db_tables_properties_nof_get_operations_dump(unit, tables_properties_idx_0, filters));
    SHR_IF_ERR_EXIT(dbal_db_tables_properties_nof_entries_dump(unit, tables_properties_idx_0, filters));
    SHR_IF_ERR_EXIT(dbal_db_tables_properties_tcam_handler_id_dump(unit, tables_properties_idx_0, filters));
    SHR_IF_ERR_EXIT(dbal_db_tables_properties_indications_bm_dump(unit, tables_properties_idx_0, filters));
    SHR_IF_ERR_EXIT(dbal_db_tables_properties_iterator_optimized_dump(unit, tables_properties_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_tables_properties_table_status_dump(int  unit,  int  tables_properties_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = tables_properties_idx_0, I0 = tables_properties_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dbal_table_status_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal tables_properties table_status") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal tables_properties table_status\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/tables_properties/table_status.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","tables_properties[].table_status: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_STATIC_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_STATIC_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES)
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->tables_properties[].table_status: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_STATIC_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[i0].table_status,
                "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","tables_properties[%s%d].table_status: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[i0].table_status,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_tables_properties_nof_set_operations_dump(int  unit,  int  tables_properties_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_tables_properties_nof_get_operations_dump(int  unit,  int  tables_properties_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_tables_properties_nof_entries_dump(int  unit,  int  tables_properties_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = tables_properties_idx_0, I0 = tables_properties_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal tables_properties nof_entries") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal tables_properties nof_entries\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/tables_properties/nof_entries.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","tables_properties[].nof_entries: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_STATIC_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_STATIC_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES)
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->tables_properties[].nof_entries: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_STATIC_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","tables_properties[%s%d].nof_entries: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[i0].nof_entries);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_tables_properties_tcam_handler_id_dump(int  unit,  int  tables_properties_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = tables_properties_idx_0, I0 = tables_properties_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal tables_properties tcam_handler_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal tables_properties tcam_handler_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/tables_properties/tcam_handler_id.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","tables_properties[].tcam_handler_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_STATIC_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_STATIC_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES)
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->tables_properties[].tcam_handler_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_STATIC_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","tables_properties[%s%d].tcam_handler_id: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[i0].tcam_handler_id);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_tables_properties_indications_bm_dump(int  unit,  int  tables_properties_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = tables_properties_idx_0, I0 = tables_properties_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal tables_properties indications_bm") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal tables_properties indications_bm\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/tables_properties/indications_bm.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","tables_properties[].indications_bm: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_STATIC_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_STATIC_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES)
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->tables_properties[].indications_bm: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_STATIC_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","tables_properties[%s%d].indications_bm: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[i0].indications_bm);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_tables_properties_iterator_optimized_dump(int  unit,  int  tables_properties_idx_0,  int  iterator_optimized_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dbal_db_tables_properties_iterator_optimized_bitmap_dump(unit, tables_properties_idx_0, iterator_optimized_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_tables_properties_iterator_optimized_bitmap_dump(int  unit,  int  tables_properties_idx_0,  int  iterator_optimized_idx_0,  int  bitmap_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = tables_properties_idx_0, I0 = tables_properties_idx_0 + 1;
    int i1 = iterator_optimized_idx_0, I1 = iterator_optimized_idx_0 + 1, org_i1 = iterator_optimized_idx_0;
    int i2 = bitmap_idx_0, I2 = bitmap_idx_0 + 1, org_i2 = bitmap_idx_0;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "SHR_BITDCL") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal tables_properties iterator_optimized bitmap") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal tables_properties iterator_optimized bitmap\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/tables_properties/iterator_optimized/bitmap.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","tables_properties[].iterator_optimized[].bitmap[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_STATIC_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_STATIC_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES)
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->tables_properties[].iterator_optimized[].bitmap[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_STATIC_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES+DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[i0].iterator_optimized);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DBAL_MODULE_ID,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[i0].iterator_optimized);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[i0].iterator_optimized
                    , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[i0].iterator_optimized), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[i0].iterator_optimized))
            {
                LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->tables_properties[].iterator_optimized[].bitmap[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[i0].iterator_optimized) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                
                
                    DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                        unit,
                        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[i0].iterator_optimized[i1].bitmap);

                if (i2 == -1) {
                    I2 = dnx_sw_state_get_nof_elements(unit,
                        DBAL_MODULE_ID,
                        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[i0].iterator_optimized[i1].bitmap);
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[i0].iterator_optimized[i1].bitmap
                        , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[i0].iterator_optimized[i1].bitmap), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[i0].iterator_optimized[i1].bitmap))
                {
                    LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->tables_properties[].iterator_optimized[].bitmap[]: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[i0].iterator_optimized[i1].bitmap) == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","tables_properties[%s%d].iterator_optimized[%s%d].bitmap[%s%d]: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_shr_bitdcl(
                        unit,
                        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[i0].iterator_optimized[i1].bitmap[i2]);

                }
                i2 = org_i2;
                dnx_sw_state_dump_end_of_stride(unit);
            }
            i1 = org_i1;
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_dbal_dynamic_tables_dump(int  unit,  int  dbal_dynamic_tables_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dbal_db_dbal_dynamic_tables_table_id_dump(unit, dbal_dynamic_tables_idx_0, filters));
    SHR_IF_ERR_EXIT(dbal_db_dbal_dynamic_tables_table_name_dump(unit, dbal_dynamic_tables_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dbal_db_dbal_dynamic_tables_labels_dump(unit, dbal_dynamic_tables_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dbal_db_dbal_dynamic_tables_data_dump(unit, dbal_dynamic_tables_idx_0, filters));
    SHR_IF_ERR_EXIT(dbal_db_dbal_dynamic_tables_key_info_data_dump(unit, dbal_dynamic_tables_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dbal_db_dbal_dynamic_tables_multi_res_info_data_dump(unit, dbal_dynamic_tables_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dbal_db_dbal_dynamic_tables_res_info_data_dump(unit, dbal_dynamic_tables_idx_0, -1, -1, filters));
    SHR_IF_ERR_EXIT(dbal_db_dbal_dynamic_tables_access_info_dump(unit, dbal_dynamic_tables_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_dbal_dynamic_tables_table_id_dump(int  unit,  int  dbal_dynamic_tables_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = dbal_dynamic_tables_idx_0, I0 = dbal_dynamic_tables_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dbal_tables_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal dbal_dynamic_tables table_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal dbal_dynamic_tables table_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/dbal_dynamic_tables/table_id.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_tables[].table_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES)
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_tables[].table_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].table_id,
                "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_tables[%s%d].table_id: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].table_id,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_dbal_dynamic_tables_table_name_dump(int  unit,  int  dbal_dynamic_tables_idx_0,  int  table_name_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = dbal_dynamic_tables_idx_0, I0 = dbal_dynamic_tables_idx_0 + 1;
    int i1 = table_name_idx_0, I1 = table_name_idx_0 + 1, org_i1 = table_name_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "char") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal dbal_dynamic_tables table_name") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal dbal_dynamic_tables table_name\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/dbal_dynamic_tables/table_name.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_tables[].table_name[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES)
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_tables[].table_name[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DBAL_MAX_STRING_LENGTH;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].table_name
                    , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].table_name), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DBAL_MAX_STRING_LENGTH)
            {
                LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_tables[].table_name[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DBAL_MAX_STRING_LENGTH == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_tables[%s%d].table_name[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_char(
                    unit,
                    &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].table_name[i1]);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_dbal_dynamic_tables_labels_dump(int  unit,  int  dbal_dynamic_tables_idx_0,  int  labels_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = dbal_dynamic_tables_idx_0, I0 = dbal_dynamic_tables_idx_0 + 1;
    int i1 = labels_idx_0, I1 = labels_idx_0 + 1, org_i1 = labels_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dbal_labels_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal dbal_dynamic_tables labels") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal dbal_dynamic_tables labels\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/dbal_dynamic_tables/labels.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_tables[].labels[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES)
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_tables[].labels[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].labels);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DBAL_MODULE_ID,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].labels);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].labels
                    , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].labels), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].labels))
            {
                LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_tables[].labels[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].labels) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                    unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].labels[i1],
                    "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_tables[%s%d].labels[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_OPAQUE_FILE(
                    unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].labels[i1],
                        "[%s%d][%s%d]: ", s0, i0, s1, i1);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_dbal_dynamic_tables_data_dump(int  unit,  int  dbal_dynamic_tables_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = dbal_dynamic_tables_idx_0, I0 = dbal_dynamic_tables_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dbal_logical_table_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal dbal_dynamic_tables data") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal dbal_dynamic_tables data\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/dbal_dynamic_tables/data.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_tables[].data: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES)
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_tables[].data: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].data,
                "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_tables[%s%d].data: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].data,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_dbal_dynamic_tables_key_info_data_dump(int  unit,  int  dbal_dynamic_tables_idx_0,  int  key_info_data_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = dbal_dynamic_tables_idx_0, I0 = dbal_dynamic_tables_idx_0 + 1;
    int i1 = key_info_data_idx_0, I1 = key_info_data_idx_0 + 1, org_i1 = key_info_data_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dbal_table_field_info_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal dbal_dynamic_tables key_info_data") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal dbal_dynamic_tables key_info_data\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/dbal_dynamic_tables/key_info_data.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_tables[].key_info_data[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES)
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_tables[].key_info_data[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].key_info_data);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DBAL_MODULE_ID,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].key_info_data);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].key_info_data
                    , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].key_info_data), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].key_info_data))
            {
                LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_tables[].key_info_data[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].key_info_data) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                    unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].key_info_data[i1],
                    "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_tables[%s%d].key_info_data[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_OPAQUE_FILE(
                    unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].key_info_data[i1],
                        "[%s%d][%s%d]: ", s0, i0, s1, i1);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_dbal_dynamic_tables_multi_res_info_data_dump(int  unit,  int  dbal_dynamic_tables_idx_0,  int  multi_res_info_data_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = dbal_dynamic_tables_idx_0, I0 = dbal_dynamic_tables_idx_0 + 1;
    int i1 = multi_res_info_data_idx_0, I1 = multi_res_info_data_idx_0 + 1, org_i1 = multi_res_info_data_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "multi_res_info_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal dbal_dynamic_tables multi_res_info_data") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal dbal_dynamic_tables multi_res_info_data\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/dbal_dynamic_tables/multi_res_info_data.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_tables[].multi_res_info_data[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES)
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_tables[].multi_res_info_data[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].multi_res_info_data);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DBAL_MODULE_ID,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].multi_res_info_data);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].multi_res_info_data
                    , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].multi_res_info_data), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].multi_res_info_data))
            {
                LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_tables[].multi_res_info_data[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].multi_res_info_data) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                    unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].multi_res_info_data[i1],
                    "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_tables[%s%d].multi_res_info_data[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_OPAQUE_FILE(
                    unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].multi_res_info_data[i1],
                        "[%s%d][%s%d]: ", s0, i0, s1, i1);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_dbal_dynamic_tables_res_info_data_dump(int  unit,  int  dbal_dynamic_tables_idx_0,  int  res_info_data_idx_0,  int  res_info_data_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = dbal_dynamic_tables_idx_0, I0 = dbal_dynamic_tables_idx_0 + 1;
    int i1 = res_info_data_idx_0, I1 = res_info_data_idx_0 + 1, org_i1 = res_info_data_idx_0;
    int i2 = res_info_data_idx_1, I2 = res_info_data_idx_1 + 1, org_i2 = res_info_data_idx_1;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dbal_table_field_info_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal dbal_dynamic_tables res_info_data") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal dbal_dynamic_tables res_info_data\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/dbal_dynamic_tables/res_info_data.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_tables[].res_info_data[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES)
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_tables[].res_info_data[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].res_info_data);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DBAL_MODULE_ID,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].res_info_data);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].res_info_data
                    , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].res_info_data), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].res_info_data))
            {
                LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_tables[].res_info_data[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].res_info_data) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                
                
                    DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                        unit,
                        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].res_info_data[i1]);

                if (i2 == -1) {
                    I2 = dnx_sw_state_get_nof_elements(unit,
                        DBAL_MODULE_ID,
                        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].res_info_data[i1]);
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].res_info_data[i1]
                        , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].res_info_data[i1]), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].res_info_data[i1]))
                {
                    LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_tables[].res_info_data[][]: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].res_info_data[i1]) == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                        unit,
                        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].res_info_data[i1][i2],
                        "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_tables[%s%d].res_info_data[%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_OPAQUE_FILE(
                        unit,
                        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].res_info_data[i1][i2],
                            "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                }
                i2 = org_i2;
                dnx_sw_state_dump_end_of_stride(unit);
            }
            i1 = org_i1;
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_dbal_dynamic_tables_access_info_dump(int  unit,  int  dbal_dynamic_tables_idx_0,  int  access_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = dbal_dynamic_tables_idx_0, I0 = dbal_dynamic_tables_idx_0 + 1;
    int i1 = access_info_idx_0, I1 = access_info_idx_0 + 1, org_i1 = access_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "DNX_SW_STATE_BUFF") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal dbal_dynamic_tables access_info") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal dbal_dynamic_tables access_info\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/dbal_dynamic_tables/access_info.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_tables[].access_info[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES)
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_tables[].access_info[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].access_info);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DBAL_MODULE_ID,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].access_info);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].access_info
                    , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].access_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].access_info))
            {
                LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_tables[].access_info[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].access_info) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_tables[%s%d].access_info[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[i0].access_info[i1]);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_dbal_dynamic_xml_tables_dump(int  unit,  int  dbal_dynamic_xml_tables_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dbal_db_dbal_dynamic_xml_tables_table_id_dump(unit, dbal_dynamic_xml_tables_idx_0, filters));
    SHR_IF_ERR_EXIT(dbal_db_dbal_dynamic_xml_tables_table_name_dump(unit, dbal_dynamic_xml_tables_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dbal_db_dbal_dynamic_xml_tables_labels_dump(unit, dbal_dynamic_xml_tables_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dbal_db_dbal_dynamic_xml_tables_data_dump(unit, dbal_dynamic_xml_tables_idx_0, filters));
    SHR_IF_ERR_EXIT(dbal_db_dbal_dynamic_xml_tables_key_info_data_dump(unit, dbal_dynamic_xml_tables_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dbal_db_dbal_dynamic_xml_tables_multi_res_info_data_dump(unit, dbal_dynamic_xml_tables_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dbal_db_dbal_dynamic_xml_tables_res_info_data_dump(unit, dbal_dynamic_xml_tables_idx_0, -1, -1, filters));
    SHR_IF_ERR_EXIT(dbal_db_dbal_dynamic_xml_tables_access_info_dump(unit, dbal_dynamic_xml_tables_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_dbal_dynamic_xml_tables_table_id_dump(int  unit,  int  dbal_dynamic_xml_tables_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = dbal_dynamic_xml_tables_idx_0, I0 = dbal_dynamic_xml_tables_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dbal_tables_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal dbal_dynamic_xml_tables table_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal dbal_dynamic_xml_tables table_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/dbal_dynamic_xml_tables/table_id.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_xml_tables[].table_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES)
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[].table_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].table_id,
                "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_xml_tables[%s%d].table_id: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].table_id,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_dbal_dynamic_xml_tables_table_name_dump(int  unit,  int  dbal_dynamic_xml_tables_idx_0,  int  table_name_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = dbal_dynamic_xml_tables_idx_0, I0 = dbal_dynamic_xml_tables_idx_0 + 1;
    int i1 = table_name_idx_0, I1 = table_name_idx_0 + 1, org_i1 = table_name_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "char") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal dbal_dynamic_xml_tables table_name") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal dbal_dynamic_xml_tables table_name\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/dbal_dynamic_xml_tables/table_name.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_xml_tables[].table_name[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES)
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[].table_name[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DBAL_MAX_STRING_LENGTH;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].table_name
                    , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].table_name), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DBAL_MAX_STRING_LENGTH)
            {
                LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[].table_name[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DBAL_MAX_STRING_LENGTH == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_xml_tables[%s%d].table_name[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_char(
                    unit,
                    &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].table_name[i1]);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_dbal_dynamic_xml_tables_labels_dump(int  unit,  int  dbal_dynamic_xml_tables_idx_0,  int  labels_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = dbal_dynamic_xml_tables_idx_0, I0 = dbal_dynamic_xml_tables_idx_0 + 1;
    int i1 = labels_idx_0, I1 = labels_idx_0 + 1, org_i1 = labels_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dbal_labels_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal dbal_dynamic_xml_tables labels") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal dbal_dynamic_xml_tables labels\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/dbal_dynamic_xml_tables/labels.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_xml_tables[].labels[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES)
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[].labels[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].labels);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DBAL_MODULE_ID,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].labels);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].labels
                    , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].labels), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].labels))
            {
                LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[].labels[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].labels) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                    unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].labels[i1],
                    "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_xml_tables[%s%d].labels[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_OPAQUE_FILE(
                    unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].labels[i1],
                        "[%s%d][%s%d]: ", s0, i0, s1, i1);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_dbal_dynamic_xml_tables_data_dump(int  unit,  int  dbal_dynamic_xml_tables_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = dbal_dynamic_xml_tables_idx_0, I0 = dbal_dynamic_xml_tables_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dbal_logical_table_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal dbal_dynamic_xml_tables data") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal dbal_dynamic_xml_tables data\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/dbal_dynamic_xml_tables/data.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_xml_tables[].data: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES)
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[].data: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].data,
                "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_xml_tables[%s%d].data: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].data,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_dbal_dynamic_xml_tables_key_info_data_dump(int  unit,  int  dbal_dynamic_xml_tables_idx_0,  int  key_info_data_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = dbal_dynamic_xml_tables_idx_0, I0 = dbal_dynamic_xml_tables_idx_0 + 1;
    int i1 = key_info_data_idx_0, I1 = key_info_data_idx_0 + 1, org_i1 = key_info_data_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dbal_table_field_info_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal dbal_dynamic_xml_tables key_info_data") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal dbal_dynamic_xml_tables key_info_data\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/dbal_dynamic_xml_tables/key_info_data.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_xml_tables[].key_info_data[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES)
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[].key_info_data[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].key_info_data);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DBAL_MODULE_ID,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].key_info_data);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].key_info_data
                    , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].key_info_data), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].key_info_data))
            {
                LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[].key_info_data[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].key_info_data) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                    unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].key_info_data[i1],
                    "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_xml_tables[%s%d].key_info_data[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_OPAQUE_FILE(
                    unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].key_info_data[i1],
                        "[%s%d][%s%d]: ", s0, i0, s1, i1);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_dbal_dynamic_xml_tables_multi_res_info_data_dump(int  unit,  int  dbal_dynamic_xml_tables_idx_0,  int  multi_res_info_data_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = dbal_dynamic_xml_tables_idx_0, I0 = dbal_dynamic_xml_tables_idx_0 + 1;
    int i1 = multi_res_info_data_idx_0, I1 = multi_res_info_data_idx_0 + 1, org_i1 = multi_res_info_data_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "multi_res_info_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal dbal_dynamic_xml_tables multi_res_info_data") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal dbal_dynamic_xml_tables multi_res_info_data\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/dbal_dynamic_xml_tables/multi_res_info_data.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_xml_tables[].multi_res_info_data[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES)
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[].multi_res_info_data[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].multi_res_info_data);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DBAL_MODULE_ID,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].multi_res_info_data);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].multi_res_info_data
                    , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].multi_res_info_data), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].multi_res_info_data))
            {
                LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[].multi_res_info_data[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].multi_res_info_data) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                    unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].multi_res_info_data[i1],
                    "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_xml_tables[%s%d].multi_res_info_data[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_OPAQUE_FILE(
                    unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].multi_res_info_data[i1],
                        "[%s%d][%s%d]: ", s0, i0, s1, i1);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_dbal_dynamic_xml_tables_res_info_data_dump(int  unit,  int  dbal_dynamic_xml_tables_idx_0,  int  res_info_data_idx_0,  int  res_info_data_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = dbal_dynamic_xml_tables_idx_0, I0 = dbal_dynamic_xml_tables_idx_0 + 1;
    int i1 = res_info_data_idx_0, I1 = res_info_data_idx_0 + 1, org_i1 = res_info_data_idx_0;
    int i2 = res_info_data_idx_1, I2 = res_info_data_idx_1 + 1, org_i2 = res_info_data_idx_1;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dbal_table_field_info_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal dbal_dynamic_xml_tables res_info_data") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal dbal_dynamic_xml_tables res_info_data\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/dbal_dynamic_xml_tables/res_info_data.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_xml_tables[].res_info_data[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES)
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[].res_info_data[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].res_info_data);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DBAL_MODULE_ID,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].res_info_data);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].res_info_data
                    , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].res_info_data), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].res_info_data))
            {
                LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[].res_info_data[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].res_info_data) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                
                
                    DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                        unit,
                        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].res_info_data[i1]);

                if (i2 == -1) {
                    I2 = dnx_sw_state_get_nof_elements(unit,
                        DBAL_MODULE_ID,
                        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].res_info_data[i1]);
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].res_info_data[i1]
                        , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].res_info_data[i1]), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].res_info_data[i1]))
                {
                    LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[].res_info_data[][]: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].res_info_data[i1]) == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                        unit,
                        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].res_info_data[i1][i2],
                        "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_xml_tables[%s%d].res_info_data[%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_OPAQUE_FILE(
                        unit,
                        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].res_info_data[i1][i2],
                            "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                }
                i2 = org_i2;
                dnx_sw_state_dump_end_of_stride(unit);
            }
            i1 = org_i1;
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_dbal_dynamic_xml_tables_access_info_dump(int  unit,  int  dbal_dynamic_xml_tables_idx_0,  int  access_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = dbal_dynamic_xml_tables_idx_0, I0 = dbal_dynamic_xml_tables_idx_0 + 1;
    int i1 = access_info_idx_0, I1 = access_info_idx_0 + 1, org_i1 = access_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "DNX_SW_STATE_BUFF") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal dbal_dynamic_xml_tables access_info") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal dbal_dynamic_xml_tables access_info\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/dbal_dynamic_xml_tables/access_info.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_xml_tables[].access_info[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES)
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[].access_info[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DBAL_TABLE_NOF_DYNAMIC_XML_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].access_info);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DBAL_MODULE_ID,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].access_info);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].access_info
                    , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].access_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].access_info))
            {
                LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[].access_info[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].access_info) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_dynamic_xml_tables[%s%d].access_info[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[i0].access_info[i1]);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_dbal_fields_dump(int  unit,  int  dbal_fields_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dbal_db_dbal_fields_field_id_dump(unit, dbal_fields_idx_0, filters));
    SHR_IF_ERR_EXIT(dbal_db_dbal_fields_field_type_dump(unit, dbal_fields_idx_0, filters));
    SHR_IF_ERR_EXIT(dbal_db_dbal_fields_field_name_dump(unit, dbal_fields_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_dbal_fields_field_id_dump(int  unit,  int  dbal_fields_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = dbal_fields_idx_0, I0 = dbal_fields_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dbal_fields_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal dbal_fields field_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal dbal_fields field_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/dbal_fields/field_id.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_fields[].field_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_QUAL_USER_NOF+DNX_DATA_MAX_FIELD_ACTION_USER_NOF;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_fields
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_fields), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_QUAL_USER_NOF+DNX_DATA_MAX_FIELD_ACTION_USER_NOF)
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_fields[].field_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_QUAL_USER_NOF+DNX_DATA_MAX_FIELD_ACTION_USER_NOF == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_fields[i0].field_id,
                "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_fields[%s%d].field_id: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_fields[i0].field_id,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_dbal_fields_field_type_dump(int  unit,  int  dbal_fields_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = dbal_fields_idx_0, I0 = dbal_fields_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dbal_field_types_defs_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal dbal_fields field_type") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal dbal_fields field_type\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/dbal_fields/field_type.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_fields[].field_type: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_QUAL_USER_NOF+DNX_DATA_MAX_FIELD_ACTION_USER_NOF;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_fields
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_fields), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_QUAL_USER_NOF+DNX_DATA_MAX_FIELD_ACTION_USER_NOF)
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_fields[].field_type: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_QUAL_USER_NOF+DNX_DATA_MAX_FIELD_ACTION_USER_NOF == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_fields[i0].field_type,
                "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_fields[%s%d].field_type: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_fields[i0].field_type,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_dbal_fields_field_name_dump(int  unit,  int  dbal_fields_idx_0,  int  field_name_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = dbal_fields_idx_0, I0 = dbal_fields_idx_0 + 1;
    int i1 = field_name_idx_0, I1 = field_name_idx_0 + 1, org_i1 = field_name_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "char") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal dbal_fields field_name") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal dbal_fields field_name\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/dbal_fields/field_name.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_fields[].field_name[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_QUAL_USER_NOF+DNX_DATA_MAX_FIELD_ACTION_USER_NOF;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_fields
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_fields), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_QUAL_USER_NOF+DNX_DATA_MAX_FIELD_ACTION_USER_NOF)
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_fields[].field_name[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_QUAL_USER_NOF+DNX_DATA_MAX_FIELD_ACTION_USER_NOF == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DBAL_MAX_STRING_LENGTH;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_fields[i0].field_name
                    , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_fields[i0].field_name), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DBAL_MAX_STRING_LENGTH)
            {
                LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_fields[].field_name[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DBAL_MAX_STRING_LENGTH == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_fields[%s%d].field_name[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_char(
                    unit,
                    &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_fields[i0].field_name[i1]);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_dbal_ppmc_table_res_types_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dbal_db_dbal_ppmc_table_res_types_multi_res_info_status_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_dbal_ppmc_table_res_types_multi_res_info_status_dump(int  unit,  int  multi_res_info_status_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = multi_res_info_status_idx_0, I0 = multi_res_info_status_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dbal_multi_res_info_status_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal dbal_ppmc_table_res_types multi_res_info_status") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal dbal_ppmc_table_res_types multi_res_info_status\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/dbal_ppmc_table_res_types/multi_res_info_status.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_ppmc_table_res_types.multi_res_info_status[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_status);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DBAL_MODULE_ID,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_status);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_status
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_status), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_status))
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_status[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_status) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_status[i0],
                "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_ppmc_table_res_types.multi_res_info_status[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_status[i0],
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_dump(int  unit,  int  multi_res_info_data_sep_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_multi_res_info_data_dump(unit, multi_res_info_data_sep_idx_0, filters));
    SHR_IF_ERR_EXIT(dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_res_info_data_dump(unit, multi_res_info_data_sep_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_multi_res_info_data_dump(int  unit,  int  multi_res_info_data_sep_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = multi_res_info_data_sep_idx_0, I0 = multi_res_info_data_sep_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "multi_res_info_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal dbal_ppmc_table_res_types multi_res_info_data_sep multi_res_info_data") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal dbal_ppmc_table_res_types multi_res_info_data_sep multi_res_info_data\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/dbal_ppmc_table_res_types/multi_res_info_data_sep/multi_res_info_data.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_ppmc_table_res_types.multi_res_info_data_sep[].multi_res_info_data: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DBAL_MODULE_ID,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep))
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep[].multi_res_info_data: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep[i0].multi_res_info_data,
                "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_ppmc_table_res_types.multi_res_info_data_sep[%s%d].multi_res_info_data: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep[i0].multi_res_info_data,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_res_info_data_dump(int  unit,  int  multi_res_info_data_sep_idx_0,  int  res_info_data_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = multi_res_info_data_sep_idx_0, I0 = multi_res_info_data_sep_idx_0 + 1;
    int i1 = res_info_data_idx_0, I1 = res_info_data_idx_0 + 1, org_i1 = res_info_data_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dbal_table_field_info_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal dbal_ppmc_table_res_types multi_res_info_data_sep res_info_data") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal dbal_ppmc_table_res_types multi_res_info_data_sep res_info_data\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/dbal_ppmc_table_res_types/multi_res_info_data_sep/res_info_data.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_ppmc_table_res_types.multi_res_info_data_sep[].res_info_data[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DBAL_MODULE_ID,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep
                , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep))
        {
            LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep[].res_info_data[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep[i0].res_info_data);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DBAL_MODULE_ID,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep[i0].res_info_data);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep[i0].res_info_data
                    , sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep[i0].res_info_data), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep[i0].res_info_data))
            {
                LOG_CLI((BSL_META("dbal_db[]->((dnx_dbal_sw_state_t*)sw_state_roots_array[][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep[].res_info_data[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DBAL_MODULE_ID, ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep[i0].res_info_data) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                    unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep[i0].res_info_data[i1],
                    "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","dbal_ppmc_table_res_types.multi_res_info_data_sep[%s%d].res_info_data[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_OPAQUE_FILE(
                    unit,
                    ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep[i0].res_info_data[i1],
                        "[%s%d][%s%d]: ", s0, i0, s1, i1);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_hw_ent_direct_map_hash_tbl_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_htbl_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal hw_ent_direct_map_hash_tbl") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal hw_ent_direct_map_hash_tbl\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/hw_ent_direct_map_hash_tbl.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","hw_ent_direct_map_hash_tbl: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","hw_ent_direct_map_hash_tbl: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        SW_STATE_HTB_DEFAULT_PRINT(
            unit,
            DBAL_MODULE_ID,
            &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_direct_map_hash_tbl);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dbal_db_hw_ent_groups_map_hash_tbl_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_htbl_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dbal hw_ent_groups_map_hash_tbl") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dbal hw_ent_groups_map_hash_tbl\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dbal_db/hw_ent_groups_map_hash_tbl.txt",
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","hw_ent_groups_map_hash_tbl: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dbal_db[%d]->","((dnx_dbal_sw_state_t*)sw_state_roots_array[%d][DBAL_MODULE_ID])->","hw_ent_groups_map_hash_tbl: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        SW_STATE_HTB_DEFAULT_PRINT(
            unit,
            DBAL_MODULE_ID,
            &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_groups_map_hash_tbl);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dbal_db_info[SOC_MAX_NUM_DEVICES][DBAL_DB_INFO_NOF_ENTRIES];
const char* dbal_db_layout_str[DBAL_DB_INFO_NOF_ENTRIES] = {
    "DBAL_DB~",
    "DBAL_DB~SW_TABLES~",
    "DBAL_DB~SW_TABLES~TABLE_TYPE~",
    "DBAL_DB~SW_TABLES~ENTRIES~",
    "DBAL_DB~SW_TABLES~HASH_TABLE~",
    "DBAL_DB~MDB_ACCESS~",
    "DBAL_DB~MDB_ACCESS~SKIP_READ_FROM_SHADOW~",
    "DBAL_DB~MDB_PHY_TABLES~",
    "DBAL_DB~MDB_PHY_TABLES~NOF_ENTRIES~",
    "DBAL_DB~TABLES_PROPERTIES~",
    "DBAL_DB~TABLES_PROPERTIES~TABLE_STATUS~",
    "DBAL_DB~TABLES_PROPERTIES~NOF_SET_OPERATIONS~",
    "DBAL_DB~TABLES_PROPERTIES~NOF_GET_OPERATIONS~",
    "DBAL_DB~TABLES_PROPERTIES~NOF_ENTRIES~",
    "DBAL_DB~TABLES_PROPERTIES~TCAM_HANDLER_ID~",
    "DBAL_DB~TABLES_PROPERTIES~INDICATIONS_BM~",
    "DBAL_DB~TABLES_PROPERTIES~ITERATOR_OPTIMIZED~",
    "DBAL_DB~TABLES_PROPERTIES~ITERATOR_OPTIMIZED~BITMAP~",
    "DBAL_DB~DBAL_DYNAMIC_TABLES~",
    "DBAL_DB~DBAL_DYNAMIC_TABLES~TABLE_ID~",
    "DBAL_DB~DBAL_DYNAMIC_TABLES~TABLE_NAME~",
    "DBAL_DB~DBAL_DYNAMIC_TABLES~LABELS~",
    "DBAL_DB~DBAL_DYNAMIC_TABLES~DATA~",
    "DBAL_DB~DBAL_DYNAMIC_TABLES~KEY_INFO_DATA~",
    "DBAL_DB~DBAL_DYNAMIC_TABLES~MULTI_RES_INFO_DATA~",
    "DBAL_DB~DBAL_DYNAMIC_TABLES~RES_INFO_DATA~",
    "DBAL_DB~DBAL_DYNAMIC_TABLES~ACCESS_INFO~",
    "DBAL_DB~DBAL_DYNAMIC_XML_TABLES~",
    "DBAL_DB~DBAL_DYNAMIC_XML_TABLES~TABLE_ID~",
    "DBAL_DB~DBAL_DYNAMIC_XML_TABLES~TABLE_NAME~",
    "DBAL_DB~DBAL_DYNAMIC_XML_TABLES~LABELS~",
    "DBAL_DB~DBAL_DYNAMIC_XML_TABLES~DATA~",
    "DBAL_DB~DBAL_DYNAMIC_XML_TABLES~KEY_INFO_DATA~",
    "DBAL_DB~DBAL_DYNAMIC_XML_TABLES~MULTI_RES_INFO_DATA~",
    "DBAL_DB~DBAL_DYNAMIC_XML_TABLES~RES_INFO_DATA~",
    "DBAL_DB~DBAL_DYNAMIC_XML_TABLES~ACCESS_INFO~",
    "DBAL_DB~DBAL_FIELDS~",
    "DBAL_DB~DBAL_FIELDS~FIELD_ID~",
    "DBAL_DB~DBAL_FIELDS~FIELD_TYPE~",
    "DBAL_DB~DBAL_FIELDS~FIELD_NAME~",
    "DBAL_DB~DBAL_PPMC_TABLE_RES_TYPES~",
    "DBAL_DB~DBAL_PPMC_TABLE_RES_TYPES~MULTI_RES_INFO_STATUS~",
    "DBAL_DB~DBAL_PPMC_TABLE_RES_TYPES~MULTI_RES_INFO_DATA_SEP~",
    "DBAL_DB~DBAL_PPMC_TABLE_RES_TYPES~MULTI_RES_INFO_DATA_SEP~MULTI_RES_INFO_DATA~",
    "DBAL_DB~DBAL_PPMC_TABLE_RES_TYPES~MULTI_RES_INFO_DATA_SEP~RES_INFO_DATA~",
    "DBAL_DB~HW_ENT_DIRECT_MAP_HASH_TBL~",
    "DBAL_DB~HW_ENT_GROUPS_MAP_HASH_TBL~",
};
#endif 
#undef BSL_LOG_MODULE
