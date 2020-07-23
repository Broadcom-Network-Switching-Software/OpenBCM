
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
#include <soc/dnx/swstate/auto_generated/diagnostic/mdb_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern mdb_tables_info * mdb_db_infos_data[SOC_MAX_NUM_DEVICES];



int
mdb_db_infos_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(mdb_db_infos_em_key_format_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_db_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_capacity_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_em_entry_count_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_em_entry_capacity_estimate_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_em_entry_capacity_estimate_average_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_eedb_payload_type_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_mdb_cluster_infos_dump(unit, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_max_fec_id_value_dump(unit, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_eedb_bank_traffic_lock_dump(unit, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_fec_hierarchy_info_dump(unit, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_mdb_profile_dump(unit, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_mdb_em_init_done_dump(unit, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_kaps_scan_cfg_on_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_kaps_scan_cfg_paused_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_db_infos_em_key_format_dump(int  unit,  int  em_key_format_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(mdb_db_infos_em_key_format_key_size_dump(unit, em_key_format_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_em_key_format_format_count_dump(unit, em_key_format_idx_0, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_em_key_format_key_to_format_map_dump(unit, em_key_format_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_em_key_format_em_bank_info_dump(unit, em_key_format_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_em_key_format_nof_address_bits_dump(unit, em_key_format_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_db_infos_em_key_format_key_size_dump(int  unit,  int  em_key_format_idx_0,  int  key_size_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = em_key_format_idx_0, I0 = em_key_format_idx_0 + 1;
    int i1 = key_size_idx_0, I1 = key_size_idx_0 + 1, org_i1 = key_size_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb em_key_format key_size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb em_key_format key_size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/em_key_format/key_size.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","em_key_format[].key_size[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_PHYSICAL_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_PHYSICAL_TABLES)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->em_key_format[].key_size[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_PHYSICAL_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_MDB_EM_MAX_NOF_TIDS;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[i0].key_size
                    , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[i0].key_size), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_MDB_EM_MAX_NOF_TIDS)
            {
                LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->em_key_format[].key_size[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_MDB_EM_MAX_NOF_TIDS == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","em_key_format[%s%d].key_size[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[i0].key_size[i1]);

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
mdb_db_infos_em_key_format_format_count_dump(int  unit,  int  em_key_format_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = em_key_format_idx_0, I0 = em_key_format_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb em_key_format format_count") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb em_key_format format_count\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/em_key_format/format_count.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","em_key_format[].format_count: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_PHYSICAL_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_PHYSICAL_TABLES)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->em_key_format[].format_count: ")));
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
                "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","em_key_format[%s%d].format_count: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[i0].format_count);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_db_infos_em_key_format_key_to_format_map_dump(int  unit,  int  em_key_format_idx_0,  int  key_to_format_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = em_key_format_idx_0, I0 = em_key_format_idx_0 + 1;
    int i1 = key_to_format_map_idx_0, I1 = key_to_format_map_idx_0 + 1, org_i1 = key_to_format_map_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb em_key_format key_to_format_map") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb em_key_format key_to_format_map\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/em_key_format/key_to_format_map.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","em_key_format[].key_to_format_map[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_PHYSICAL_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_PHYSICAL_TABLES)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->em_key_format[].key_to_format_map[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_PHYSICAL_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = MDB_EM_MAX_KEYS_PLUS_TID_VALS_SIZE;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[i0].key_to_format_map
                    , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[i0].key_to_format_map), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= MDB_EM_MAX_KEYS_PLUS_TID_VALS_SIZE)
            {
                LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->em_key_format[].key_to_format_map[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(MDB_EM_MAX_KEYS_PLUS_TID_VALS_SIZE == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","em_key_format[%s%d].key_to_format_map[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[i0].key_to_format_map[i1]);

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
mdb_db_infos_em_key_format_em_bank_info_dump(int  unit,  int  em_key_format_idx_0,  int  em_bank_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(mdb_db_infos_em_key_format_em_bank_info_is_flex_dump(unit, em_key_format_idx_0, em_bank_info_idx_0, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_em_key_format_em_bank_info_bank_size_used_macro_b_dump(unit, em_key_format_idx_0, em_bank_info_idx_0, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_em_key_format_em_bank_info_macro_index_dump(unit, em_key_format_idx_0, em_bank_info_idx_0, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_em_key_format_em_bank_info_macro_type_dump(unit, em_key_format_idx_0, em_bank_info_idx_0, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_em_key_format_em_bank_info_cluster_mapping_dump(unit, em_key_format_idx_0, em_bank_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_db_infos_em_key_format_em_bank_info_is_flex_dump(int  unit,  int  em_key_format_idx_0,  int  em_bank_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = em_key_format_idx_0, I0 = em_key_format_idx_0 + 1;
    int i1 = em_bank_info_idx_0, I1 = em_bank_info_idx_0 + 1, org_i1 = em_bank_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb em_key_format em_bank_info is_flex") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb em_key_format em_bank_info is_flex\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/em_key_format/em_bank_info/is_flex.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","em_key_format[].em_bank_info[].is_flex: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_PHYSICAL_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_PHYSICAL_TABLES)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->em_key_format[].em_bank_info[].is_flex: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_PHYSICAL_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[i0].em_bank_info
                    , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[i0].em_bank_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS)
            {
                LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->em_key_format[].em_bank_info[].is_flex: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","em_key_format[%s%d].em_bank_info[%s%d].is_flex: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[i0].em_bank_info[i1].is_flex);

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
mdb_db_infos_em_key_format_em_bank_info_bank_size_used_macro_b_dump(int  unit,  int  em_key_format_idx_0,  int  em_bank_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = em_key_format_idx_0, I0 = em_key_format_idx_0 + 1;
    int i1 = em_bank_info_idx_0, I1 = em_bank_info_idx_0 + 1, org_i1 = em_bank_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb em_key_format em_bank_info bank_size_used_macro_b") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb em_key_format em_bank_info bank_size_used_macro_b\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/em_key_format/em_bank_info/bank_size_used_macro_b.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","em_key_format[].em_bank_info[].bank_size_used_macro_b: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_PHYSICAL_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_PHYSICAL_TABLES)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->em_key_format[].em_bank_info[].bank_size_used_macro_b: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_PHYSICAL_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[i0].em_bank_info
                    , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[i0].em_bank_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS)
            {
                LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->em_key_format[].em_bank_info[].bank_size_used_macro_b: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","em_key_format[%s%d].em_bank_info[%s%d].bank_size_used_macro_b: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[i0].em_bank_info[i1].bank_size_used_macro_b);

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
mdb_db_infos_em_key_format_em_bank_info_macro_index_dump(int  unit,  int  em_key_format_idx_0,  int  em_bank_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = em_key_format_idx_0, I0 = em_key_format_idx_0 + 1;
    int i1 = em_bank_info_idx_0, I1 = em_bank_info_idx_0 + 1, org_i1 = em_bank_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb em_key_format em_bank_info macro_index") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb em_key_format em_bank_info macro_index\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/em_key_format/em_bank_info/macro_index.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","em_key_format[].em_bank_info[].macro_index: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_PHYSICAL_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_PHYSICAL_TABLES)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->em_key_format[].em_bank_info[].macro_index: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_PHYSICAL_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[i0].em_bank_info
                    , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[i0].em_bank_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS)
            {
                LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->em_key_format[].em_bank_info[].macro_index: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","em_key_format[%s%d].em_bank_info[%s%d].macro_index: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[i0].em_bank_info[i1].macro_index);

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
mdb_db_infos_em_key_format_em_bank_info_macro_type_dump(int  unit,  int  em_key_format_idx_0,  int  em_bank_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = em_key_format_idx_0, I0 = em_key_format_idx_0 + 1;
    int i1 = em_bank_info_idx_0, I1 = em_bank_info_idx_0 + 1, org_i1 = em_bank_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "mdb_macro_types_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb em_key_format em_bank_info macro_type") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb em_key_format em_bank_info macro_type\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/em_key_format/em_bank_info/macro_type.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","em_key_format[].em_bank_info[].macro_type: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_PHYSICAL_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_PHYSICAL_TABLES)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->em_key_format[].em_bank_info[].macro_type: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_PHYSICAL_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[i0].em_bank_info
                    , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[i0].em_bank_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS)
            {
                LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->em_key_format[].em_bank_info[].macro_type: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                    unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[i0].em_bank_info[i1].macro_type,
                    "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","em_key_format[%s%d].em_bank_info[%s%d].macro_type: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_OPAQUE_FILE(
                    unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[i0].em_bank_info[i1].macro_type,
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
mdb_db_infos_em_key_format_em_bank_info_cluster_mapping_dump(int  unit,  int  em_key_format_idx_0,  int  em_bank_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = em_key_format_idx_0, I0 = em_key_format_idx_0 + 1;
    int i1 = em_bank_info_idx_0, I1 = em_bank_info_idx_0 + 1, org_i1 = em_bank_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb em_key_format em_bank_info cluster_mapping") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb em_key_format em_bank_info cluster_mapping\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/em_key_format/em_bank_info/cluster_mapping.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","em_key_format[].em_bank_info[].cluster_mapping: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_PHYSICAL_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_PHYSICAL_TABLES)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->em_key_format[].em_bank_info[].cluster_mapping: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_PHYSICAL_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[i0].em_bank_info
                    , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[i0].em_bank_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS)
            {
                LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->em_key_format[].em_bank_info[].cluster_mapping: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","em_key_format[%s%d].em_bank_info[%s%d].cluster_mapping: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[i0].em_bank_info[i1].cluster_mapping);

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
mdb_db_infos_em_key_format_nof_address_bits_dump(int  unit,  int  em_key_format_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = em_key_format_idx_0, I0 = em_key_format_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb em_key_format nof_address_bits") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb em_key_format nof_address_bits\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/em_key_format/nof_address_bits.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","em_key_format[].nof_address_bits: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_PHYSICAL_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_PHYSICAL_TABLES)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->em_key_format[].nof_address_bits: ")));
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
                "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","em_key_format[%s%d].nof_address_bits: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[i0].nof_address_bits);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_db_infos_db_dump(int  unit,  int  db_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(mdb_db_infos_db_number_of_alloc_clusters_dump(unit, db_idx_0, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_db_number_of_clusters_dump(unit, db_idx_0, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_db_nof_cluster_per_entry_dump(unit, db_idx_0, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_db_clusters_info_dump(unit, db_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_db_infos_db_number_of_alloc_clusters_dump(int  unit,  int  db_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = db_idx_0, I0 = db_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb db number_of_alloc_clusters") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb db number_of_alloc_clusters\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/db/number_of_alloc_clusters.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","db[].number_of_alloc_clusters: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->db[].number_of_alloc_clusters: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","db[%s%d].number_of_alloc_clusters: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].number_of_alloc_clusters);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_db_infos_db_number_of_clusters_dump(int  unit,  int  db_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = db_idx_0, I0 = db_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb db number_of_clusters") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb db number_of_clusters\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/db/number_of_clusters.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","db[].number_of_clusters: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->db[].number_of_clusters: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","db[%s%d].number_of_clusters: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].number_of_clusters);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_db_infos_db_nof_cluster_per_entry_dump(int  unit,  int  db_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = db_idx_0, I0 = db_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb db nof_cluster_per_entry") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb db nof_cluster_per_entry\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/db/nof_cluster_per_entry.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","db[].nof_cluster_per_entry: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->db[].nof_cluster_per_entry: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","db[%s%d].nof_cluster_per_entry: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].nof_cluster_per_entry);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_db_infos_db_clusters_info_dump(int  unit,  int  db_idx_0,  int  clusters_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(mdb_db_infos_db_clusters_info_macro_type_dump(unit, db_idx_0, clusters_info_idx_0, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_db_clusters_info_macro_index_dump(unit, db_idx_0, clusters_info_idx_0, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_db_clusters_info_cluster_index_dump(unit, db_idx_0, clusters_info_idx_0, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_db_clusters_info_cluster_position_in_entry_dump(unit, db_idx_0, clusters_info_idx_0, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_db_clusters_info_start_address_dump(unit, db_idx_0, clusters_info_idx_0, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_db_clusters_info_end_address_dump(unit, db_idx_0, clusters_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_db_infos_db_clusters_info_macro_type_dump(int  unit,  int  db_idx_0,  int  clusters_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = db_idx_0, I0 = db_idx_0 + 1;
    int i1 = clusters_info_idx_0, I1 = clusters_info_idx_0 + 1, org_i1 = clusters_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "mdb_macro_types_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb db clusters_info macro_type") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb db clusters_info macro_type\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/db/clusters_info/macro_type.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","db[].clusters_info[].macro_type: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->db[].clusters_info[].macro_type: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    MDB_MODULE_ID,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info
                    , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info))
            {
                LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->db[].clusters_info[].macro_type: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                    unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info[i1].macro_type,
                    "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","db[%s%d].clusters_info[%s%d].macro_type: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_OPAQUE_FILE(
                    unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info[i1].macro_type,
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
mdb_db_infos_db_clusters_info_macro_index_dump(int  unit,  int  db_idx_0,  int  clusters_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = db_idx_0, I0 = db_idx_0 + 1;
    int i1 = clusters_info_idx_0, I1 = clusters_info_idx_0 + 1, org_i1 = clusters_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb db clusters_info macro_index") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb db clusters_info macro_index\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/db/clusters_info/macro_index.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","db[].clusters_info[].macro_index: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->db[].clusters_info[].macro_index: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    MDB_MODULE_ID,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info
                    , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info))
            {
                LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->db[].clusters_info[].macro_index: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","db[%s%d].clusters_info[%s%d].macro_index: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info[i1].macro_index);

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
mdb_db_infos_db_clusters_info_cluster_index_dump(int  unit,  int  db_idx_0,  int  clusters_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = db_idx_0, I0 = db_idx_0 + 1;
    int i1 = clusters_info_idx_0, I1 = clusters_info_idx_0 + 1, org_i1 = clusters_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb db clusters_info cluster_index") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb db clusters_info cluster_index\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/db/clusters_info/cluster_index.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","db[].clusters_info[].cluster_index: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->db[].clusters_info[].cluster_index: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    MDB_MODULE_ID,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info
                    , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info))
            {
                LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->db[].clusters_info[].cluster_index: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","db[%s%d].clusters_info[%s%d].cluster_index: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info[i1].cluster_index);

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
mdb_db_infos_db_clusters_info_cluster_position_in_entry_dump(int  unit,  int  db_idx_0,  int  clusters_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = db_idx_0, I0 = db_idx_0 + 1;
    int i1 = clusters_info_idx_0, I1 = clusters_info_idx_0 + 1, org_i1 = clusters_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb db clusters_info cluster_position_in_entry") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb db clusters_info cluster_position_in_entry\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/db/clusters_info/cluster_position_in_entry.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","db[].clusters_info[].cluster_position_in_entry: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->db[].clusters_info[].cluster_position_in_entry: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    MDB_MODULE_ID,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info
                    , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info))
            {
                LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->db[].clusters_info[].cluster_position_in_entry: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","db[%s%d].clusters_info[%s%d].cluster_position_in_entry: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info[i1].cluster_position_in_entry);

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
mdb_db_infos_db_clusters_info_start_address_dump(int  unit,  int  db_idx_0,  int  clusters_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = db_idx_0, I0 = db_idx_0 + 1;
    int i1 = clusters_info_idx_0, I1 = clusters_info_idx_0 + 1, org_i1 = clusters_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb db clusters_info start_address") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb db clusters_info start_address\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/db/clusters_info/start_address.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","db[].clusters_info[].start_address: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->db[].clusters_info[].start_address: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    MDB_MODULE_ID,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info
                    , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info))
            {
                LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->db[].clusters_info[].start_address: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","db[%s%d].clusters_info[%s%d].start_address: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info[i1].start_address);

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
mdb_db_infos_db_clusters_info_end_address_dump(int  unit,  int  db_idx_0,  int  clusters_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = db_idx_0, I0 = db_idx_0 + 1;
    int i1 = clusters_info_idx_0, I1 = clusters_info_idx_0 + 1, org_i1 = clusters_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb db clusters_info end_address") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb db clusters_info end_address\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/db/clusters_info/end_address.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","db[].clusters_info[].end_address: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->db[].clusters_info[].end_address: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    MDB_MODULE_ID,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info
                    , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info))
            {
                LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->db[].clusters_info[].end_address: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","db[%s%d].clusters_info[%s%d].end_address: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[i0].clusters_info[i1].end_address);

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
mdb_db_infos_capacity_dump(int  unit,  int  capacity_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = capacity_idx_0, I0 = capacity_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb capacity") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb capacity\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/capacity.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","capacity[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_PHYSICAL_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->capacity
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->capacity), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_PHYSICAL_TABLES)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->capacity[]: ")));
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
                "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","capacity[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->capacity[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_db_infos_em_entry_count_dump(int  unit,  int  em_entry_count_idx_0,  int  em_entry_count_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = em_entry_count_idx_0, I0 = em_entry_count_idx_0 + 1;
    int i1 = em_entry_count_idx_1, I1 = em_entry_count_idx_1 + 1, org_i1 = em_entry_count_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb em_entry_count") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb em_entry_count\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/em_entry_count.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","em_entry_count[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_PHYSICAL_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_entry_count
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_entry_count), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_PHYSICAL_TABLES)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->em_entry_count[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_PHYSICAL_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_MDB_EM_NOF_ENCODING_TYPES;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_entry_count[i0]
                    , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_entry_count[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_MDB_EM_NOF_ENCODING_TYPES)
            {
                LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->em_entry_count[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_MDB_EM_NOF_ENCODING_TYPES == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","em_entry_count[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_entry_count[i0][i1]);

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
mdb_db_infos_em_entry_capacity_estimate_dump(int  unit,  int  em_entry_capacity_estimate_idx_0,  int  em_entry_capacity_estimate_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = em_entry_capacity_estimate_idx_0, I0 = em_entry_capacity_estimate_idx_0 + 1;
    int i1 = em_entry_capacity_estimate_idx_1, I1 = em_entry_capacity_estimate_idx_1 + 1, org_i1 = em_entry_capacity_estimate_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb em_entry_capacity_estimate") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb em_entry_capacity_estimate\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/em_entry_capacity_estimate.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","em_entry_capacity_estimate[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_PHYSICAL_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_entry_capacity_estimate
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_entry_capacity_estimate), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_PHYSICAL_TABLES)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->em_entry_capacity_estimate[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_PHYSICAL_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_MDB_EM_NOF_ENCODING_TYPES;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_entry_capacity_estimate[i0]
                    , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_entry_capacity_estimate[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_MDB_EM_NOF_ENCODING_TYPES)
            {
                LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->em_entry_capacity_estimate[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_MDB_EM_NOF_ENCODING_TYPES == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","em_entry_capacity_estimate[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_entry_capacity_estimate[i0][i1]);

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
mdb_db_infos_em_entry_capacity_estimate_average_dump(int  unit,  int  em_entry_capacity_estimate_average_idx_0,  int  em_entry_capacity_estimate_average_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = em_entry_capacity_estimate_average_idx_0, I0 = em_entry_capacity_estimate_average_idx_0 + 1;
    int i1 = em_entry_capacity_estimate_average_idx_1, I1 = em_entry_capacity_estimate_average_idx_1 + 1, org_i1 = em_entry_capacity_estimate_average_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb em_entry_capacity_estimate_average") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb em_entry_capacity_estimate_average\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/em_entry_capacity_estimate_average.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","em_entry_capacity_estimate_average[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_PHYSICAL_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_entry_capacity_estimate_average
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_entry_capacity_estimate_average), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_PHYSICAL_TABLES)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->em_entry_capacity_estimate_average[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_PHYSICAL_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_MDB_EM_NOF_ENCODING_TYPES;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_entry_capacity_estimate_average[i0]
                    , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_entry_capacity_estimate_average[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_MDB_EM_NOF_ENCODING_TYPES)
            {
                LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->em_entry_capacity_estimate_average[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_MDB_EM_NOF_ENCODING_TYPES == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","em_entry_capacity_estimate_average[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_entry_capacity_estimate_average[i0][i1]);

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
mdb_db_infos_eedb_payload_type_dump(int  unit,  int  eedb_payload_type_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = eedb_payload_type_idx_0, I0 = eedb_payload_type_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb eedb_payload_type") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb eedb_payload_type\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/eedb_payload_type.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","eedb_payload_type[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_payload_type
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_payload_type), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->eedb_payload_type[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","eedb_payload_type[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_payload_type[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_db_infos_mdb_cluster_infos_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(mdb_db_infos_mdb_cluster_infos_cache_enabled_dump(unit, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_mdb_cluster_infos_eedb_banks_hitbit_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_mdb_cluster_infos_small_kaps_bb_hitbit_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_mdb_cluster_infos_small_kaps_rpb_hitbit_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_mdb_cluster_infos_direct_address_mapping_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_mdb_cluster_infos_eedb_address_mapping_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_db_infos_mdb_cluster_infos_cache_enabled_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb mdb_cluster_infos cache_enabled") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb mdb_cluster_infos cache_enabled\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/mdb_cluster_infos/cache_enabled.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","mdb_cluster_infos.cache_enabled: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","mdb_cluster_infos.cache_enabled: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint8(
            unit,
            &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.cache_enabled);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_dump(int  unit,  int  macro_cluster_assoc_idx_0,  int  macro_cluster_assoc_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_pdb_dump(unit, macro_cluster_assoc_idx_0,macro_cluster_assoc_idx_1, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_flags_dump(unit, macro_cluster_assoc_idx_0,macro_cluster_assoc_idx_1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_pdb_dump(int  unit,  int  macro_cluster_assoc_idx_0,  int  macro_cluster_assoc_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = macro_cluster_assoc_idx_0, I0 = macro_cluster_assoc_idx_0 + 1;
    int i1 = macro_cluster_assoc_idx_1, I1 = macro_cluster_assoc_idx_1 + 1, org_i1 = macro_cluster_assoc_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dbal_enum_value_field_mdb_physical_table_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb mdb_cluster_infos macro_cluster_assoc pdb") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb mdb_cluster_infos macro_cluster_assoc pdb\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/mdb_cluster_infos/macro_cluster_assoc/pdb.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","mdb_cluster_infos.macro_cluster_assoc[][].pdb: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = MDB_NOF_MACRO_TYPES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= MDB_NOF_MACRO_TYPES)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[][].pdb: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(MDB_NOF_MACRO_TYPES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    MDB_MODULE_ID,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[i0]
                    , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[i0]))
            {
                LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[][].pdb: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","mdb_cluster_infos.macro_cluster_assoc[%s%d][%s%d].pdb: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[i0][i1].pdb);

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
mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_flags_dump(int  unit,  int  macro_cluster_assoc_idx_0,  int  macro_cluster_assoc_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = macro_cluster_assoc_idx_0, I0 = macro_cluster_assoc_idx_0 + 1;
    int i1 = macro_cluster_assoc_idx_1, I1 = macro_cluster_assoc_idx_1 + 1, org_i1 = macro_cluster_assoc_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb mdb_cluster_infos macro_cluster_assoc flags") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb mdb_cluster_infos macro_cluster_assoc flags\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/mdb_cluster_infos/macro_cluster_assoc/flags.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","mdb_cluster_infos.macro_cluster_assoc[][].flags: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = MDB_NOF_MACRO_TYPES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= MDB_NOF_MACRO_TYPES)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[][].flags: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(MDB_NOF_MACRO_TYPES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    MDB_MODULE_ID,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[i0]
                    , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[i0]))
            {
                LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[][].flags: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","mdb_cluster_infos.macro_cluster_assoc[%s%d][%s%d].flags: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[i0][i1].flags);

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
mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_dump(int  unit,  int  macro_cluster_cache_idx_0,  int  macro_cluster_cache_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_dump(unit, macro_cluster_cache_idx_0,macro_cluster_cache_idx_1, -1, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_dump(int  unit,  int  macro_cluster_cache_idx_0,  int  macro_cluster_cache_idx_1,  int  data_idx_0,  int  data_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = macro_cluster_cache_idx_0, I0 = macro_cluster_cache_idx_0 + 1;
    int i1 = macro_cluster_cache_idx_1, I1 = macro_cluster_cache_idx_1 + 1, org_i1 = macro_cluster_cache_idx_1;
    int i2 = data_idx_0, I2 = data_idx_0 + 1, org_i2 = data_idx_0;
    int i3 = data_idx_1, I3 = data_idx_1 + 1, org_i3 = data_idx_1;
    char *s0 = "", *s1 = "", *s2 = "", *s3 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb mdb_cluster_infos macro_cluster_cache data") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb mdb_cluster_infos macro_cluster_cache data\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/mdb_cluster_infos/macro_cluster_cache/data.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","mdb_cluster_infos.macro_cluster_cache[][].data[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = MDB_NOF_MACRO_TYPES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= MDB_NOF_MACRO_TYPES)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[][].data[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(MDB_NOF_MACRO_TYPES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    MDB_MODULE_ID,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[i0]
                    , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[i0]))
            {
                LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[][].data[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                
                
                    DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                        unit,
                        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[i0][i1].data);

                if (i2 == -1) {
                    I2 = dnx_sw_state_get_nof_elements(unit,
                        MDB_MODULE_ID,
                        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[i0][i1].data);
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[i0][i1].data
                        , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[i0][i1].data), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[i0][i1].data))
                {
                    LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[][].data[][]: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[i0][i1].data) == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    if (i3 == -1) {
                        I3 = DNX_DATA_MAX_MDB_DH_CLUSTER_ROW_WIDTH_UINT32;
                        i3 = dnx_sw_state_dump_check_all_the_same(unit,
                            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[i0][i1].data[i2]
                            , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[i0][i1].data[i2]), I3, &s3) ? I3 - 1 : 0;
                    }

                    if(i3 >= DNX_DATA_MAX_MDB_DH_CLUSTER_ROW_WIDTH_UINT32)
                    {
                        LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[][].data[][]: ")));
                        LOG_CLI((BSL_META("Invalid index: %d \n"),i3));
                        SHR_EXIT();
                    }

                    if(DNX_DATA_MAX_MDB_DH_CLUSTER_ROW_WIDTH_UINT32 == 0)
                    {
                        SHR_EXIT();
                    }

                    for(; i3 < I3; i3++) {
                        dnx_sw_state_dump_update_current_idx(unit, i3);
                        DNX_SW_STATE_PRINT_MONITOR(
                            unit,
                            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","mdb_cluster_infos.macro_cluster_cache[%s%d][%s%d].data[%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2, s3, i3);

                        DNX_SW_STATE_PRINT_FILE(
                            unit,
                            "[%s%d][%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2, s3, i3);

                        dnx_sw_state_print_uint32(
                            unit,
                            &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[i0][i1].data[i2][i3]);

                    }
                    i3 = org_i3;
                    dnx_sw_state_dump_end_of_stride(unit);
                }
                i2 = org_i2;
            }
            i1 = org_i1;
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_db_infos_mdb_cluster_infos_eedb_banks_hitbit_dump(int  unit,  int  eedb_banks_hitbit_idx_0,  int  eedb_banks_hitbit_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = eedb_banks_hitbit_idx_0, I0 = eedb_banks_hitbit_idx_0 + 1;
    int i1 = eedb_banks_hitbit_idx_1, I1 = eedb_banks_hitbit_idx_1 + 1, org_i1 = eedb_banks_hitbit_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb mdb_cluster_infos eedb_banks_hitbit") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb mdb_cluster_infos eedb_banks_hitbit\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/mdb_cluster_infos/eedb_banks_hitbit.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","mdb_cluster_infos.eedb_banks_hitbit[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_MDB_EEDB_NOF_EEDB_BANKS;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_banks_hitbit
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_banks_hitbit), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_MDB_EEDB_NOF_EEDB_BANKS)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->mdb_cluster_infos.eedb_banks_hitbit[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_MDB_EEDB_NOF_EEDB_BANKS == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_banks_hitbit[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    MDB_MODULE_ID,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_banks_hitbit[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_banks_hitbit[i0]
                    , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_banks_hitbit[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_banks_hitbit[i0]))
            {
                LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->mdb_cluster_infos.eedb_banks_hitbit[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_banks_hitbit[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","mdb_cluster_infos.eedb_banks_hitbit[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_banks_hitbit[i0][i1]);

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
mdb_db_infos_mdb_cluster_infos_small_kaps_bb_hitbit_dump(int  unit,  int  small_kaps_bb_hitbit_idx_0,  int  small_kaps_bb_hitbit_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = small_kaps_bb_hitbit_idx_0, I0 = small_kaps_bb_hitbit_idx_0 + 1;
    int i1 = small_kaps_bb_hitbit_idx_1, I1 = small_kaps_bb_hitbit_idx_1 + 1, org_i1 = small_kaps_bb_hitbit_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb mdb_cluster_infos small_kaps_bb_hitbit") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb mdb_cluster_infos small_kaps_bb_hitbit\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/mdb_cluster_infos/small_kaps_bb_hitbit.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","mdb_cluster_infos.small_kaps_bb_hitbit[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                MDB_MODULE_ID,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit))
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    MDB_MODULE_ID,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit[i0]
                    , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit[i0]))
            {
                LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","mdb_cluster_infos.small_kaps_bb_hitbit[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit[i0][i1]);

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
mdb_db_infos_mdb_cluster_infos_small_kaps_rpb_hitbit_dump(int  unit,  int  small_kaps_rpb_hitbit_idx_0,  int  small_kaps_rpb_hitbit_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = small_kaps_rpb_hitbit_idx_0, I0 = small_kaps_rpb_hitbit_idx_0 + 1;
    int i1 = small_kaps_rpb_hitbit_idx_1, I1 = small_kaps_rpb_hitbit_idx_1 + 1, org_i1 = small_kaps_rpb_hitbit_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb mdb_cluster_infos small_kaps_rpb_hitbit") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb mdb_cluster_infos small_kaps_rpb_hitbit\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/mdb_cluster_infos/small_kaps_rpb_hitbit.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","mdb_cluster_infos.small_kaps_rpb_hitbit[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                MDB_MODULE_ID,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit))
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    MDB_MODULE_ID,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit[i0]
                    , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit[i0]))
            {
                LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","mdb_cluster_infos.small_kaps_rpb_hitbit[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit[i0][i1]);

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
mdb_db_infos_mdb_cluster_infos_direct_address_mapping_dump(int  unit,  int  direct_address_mapping_idx_0,  int  direct_address_mapping_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = direct_address_mapping_idx_0, I0 = direct_address_mapping_idx_0 + 1;
    int i1 = direct_address_mapping_idx_1, I1 = direct_address_mapping_idx_1 + 1, org_i1 = direct_address_mapping_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb mdb_cluster_infos direct_address_mapping") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb mdb_cluster_infos direct_address_mapping\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/mdb_cluster_infos/direct_address_mapping.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","mdb_cluster_infos.direct_address_mapping[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.direct_address_mapping
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.direct_address_mapping), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->mdb_cluster_infos.direct_address_mapping[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.direct_address_mapping[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    MDB_MODULE_ID,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.direct_address_mapping[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.direct_address_mapping[i0]
                    , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.direct_address_mapping[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.direct_address_mapping[i0]))
            {
                LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->mdb_cluster_infos.direct_address_mapping[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.direct_address_mapping[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","mdb_cluster_infos.direct_address_mapping[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.direct_address_mapping[i0][i1]);

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
mdb_db_infos_mdb_cluster_infos_eedb_address_mapping_dump(int  unit,  int  eedb_address_mapping_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = eedb_address_mapping_idx_0, I0 = eedb_address_mapping_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb mdb_cluster_infos eedb_address_mapping") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb mdb_cluster_infos eedb_address_mapping\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/mdb_cluster_infos/eedb_address_mapping.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","mdb_cluster_infos.eedb_address_mapping[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_address_mapping);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                MDB_MODULE_ID,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_address_mapping);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_address_mapping
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_address_mapping), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_address_mapping))
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->mdb_cluster_infos.eedb_address_mapping[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_address_mapping) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","mdb_cluster_infos.eedb_address_mapping[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_address_mapping[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_db_infos_max_fec_id_value_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb max_fec_id_value") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb max_fec_id_value\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/max_fec_id_value.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","max_fec_id_value: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","max_fec_id_value: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint32(
            unit,
            &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->max_fec_id_value);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_db_infos_eedb_bank_traffic_lock_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(mdb_db_infos_eedb_bank_traffic_lock_local_lifs_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_eedb_bank_traffic_lock_global_lif_destination_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_db_infos_eedb_bank_traffic_lock_local_lifs_dump(int  unit,  int  local_lifs_idx_0,  int  local_lifs_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = local_lifs_idx_0, I0 = local_lifs_idx_0 + 1;
    int i1 = local_lifs_idx_1, I1 = local_lifs_idx_1 + 1, org_i1 = local_lifs_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb eedb_bank_traffic_lock local_lifs") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb eedb_bank_traffic_lock local_lifs\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/eedb_bank_traffic_lock/local_lifs.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","eedb_bank_traffic_lock.local_lifs[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = 2;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_bank_traffic_lock.local_lifs
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_bank_traffic_lock.local_lifs), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 2)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->eedb_bank_traffic_lock.local_lifs[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(2 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_bank_traffic_lock.local_lifs[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    MDB_MODULE_ID,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_bank_traffic_lock.local_lifs[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_bank_traffic_lock.local_lifs[i0]
                    , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_bank_traffic_lock.local_lifs[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_bank_traffic_lock.local_lifs[i0]))
            {
                LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->eedb_bank_traffic_lock.local_lifs[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_bank_traffic_lock.local_lifs[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","eedb_bank_traffic_lock.local_lifs[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_bank_traffic_lock.local_lifs[i0][i1]);

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
mdb_db_infos_eedb_bank_traffic_lock_global_lif_destination_dump(int  unit,  int  global_lif_destination_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = global_lif_destination_idx_0, I0 = global_lif_destination_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb eedb_bank_traffic_lock global_lif_destination") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb eedb_bank_traffic_lock global_lif_destination\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/eedb_bank_traffic_lock/global_lif_destination.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","eedb_bank_traffic_lock.global_lif_destination[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = 2;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_bank_traffic_lock.global_lif_destination
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_bank_traffic_lock.global_lif_destination), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 2)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->eedb_bank_traffic_lock.global_lif_destination[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(2 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","eedb_bank_traffic_lock.global_lif_destination[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_bank_traffic_lock.global_lif_destination[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_db_infos_fec_hierarchy_info_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(mdb_db_infos_fec_hierarchy_info_fec_hierarchy_map_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_fec_hierarchy_info_fec_id_range_start_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(mdb_db_infos_fec_hierarchy_info_fec_id_range_size_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_db_infos_fec_hierarchy_info_fec_hierarchy_map_dump(int  unit,  int  fec_hierarchy_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fec_hierarchy_map_idx_0, I0 = fec_hierarchy_map_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb fec_hierarchy_info fec_hierarchy_map") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb fec_hierarchy_info fec_hierarchy_map\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/fec_hierarchy_info/fec_hierarchy_map.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","fec_hierarchy_info.fec_hierarchy_map[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_hierarchy_info.fec_hierarchy_map);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                MDB_MODULE_ID,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_hierarchy_info.fec_hierarchy_map);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_hierarchy_info.fec_hierarchy_map
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_hierarchy_info.fec_hierarchy_map), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_hierarchy_info.fec_hierarchy_map))
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->fec_hierarchy_info.fec_hierarchy_map[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_hierarchy_info.fec_hierarchy_map) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","fec_hierarchy_info.fec_hierarchy_map[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_hierarchy_info.fec_hierarchy_map[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_db_infos_fec_hierarchy_info_fec_id_range_start_dump(int  unit,  int  fec_id_range_start_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fec_id_range_start_idx_0, I0 = fec_id_range_start_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb fec_hierarchy_info fec_id_range_start") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb fec_hierarchy_info fec_id_range_start\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/fec_hierarchy_info/fec_id_range_start.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","fec_hierarchy_info.fec_id_range_start[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_hierarchy_info.fec_id_range_start);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                MDB_MODULE_ID,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_hierarchy_info.fec_id_range_start);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_hierarchy_info.fec_id_range_start
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_hierarchy_info.fec_id_range_start), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_hierarchy_info.fec_id_range_start))
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->fec_hierarchy_info.fec_id_range_start[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_hierarchy_info.fec_id_range_start) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","fec_hierarchy_info.fec_id_range_start[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_hierarchy_info.fec_id_range_start[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_db_infos_fec_hierarchy_info_fec_id_range_size_dump(int  unit,  int  fec_id_range_size_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fec_id_range_size_idx_0, I0 = fec_id_range_size_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb fec_hierarchy_info fec_id_range_size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb fec_hierarchy_info fec_id_range_size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/fec_hierarchy_info/fec_id_range_size.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","fec_hierarchy_info.fec_id_range_size[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_hierarchy_info.fec_id_range_size);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                MDB_MODULE_ID,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_hierarchy_info.fec_id_range_size);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_hierarchy_info.fec_id_range_size
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_hierarchy_info.fec_id_range_size), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_hierarchy_info.fec_id_range_size))
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->fec_hierarchy_info.fec_id_range_size[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, MDB_MODULE_ID, ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_hierarchy_info.fec_id_range_size) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","fec_hierarchy_info.fec_id_range_size[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_hierarchy_info.fec_id_range_size[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_db_infos_mdb_profile_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb mdb_profile") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb mdb_profile\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/mdb_profile.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","mdb_profile: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","mdb_profile: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint8(
            unit,
            &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_profile);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_db_infos_mdb_em_init_done_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb mdb_em_init_done") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb mdb_em_init_done\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/mdb_em_init_done.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","mdb_em_init_done: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","mdb_em_init_done: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint8(
            unit,
            &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_em_init_done);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_db_infos_kaps_scan_cfg_on_dump(int  unit,  int  kaps_scan_cfg_on_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = kaps_scan_cfg_on_idx_0, I0 = kaps_scan_cfg_on_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb kaps_scan_cfg_on") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb kaps_scan_cfg_on\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/kaps_scan_cfg_on.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","kaps_scan_cfg_on[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = 2;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->kaps_scan_cfg_on
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->kaps_scan_cfg_on), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 2)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->kaps_scan_cfg_on[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(2 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","kaps_scan_cfg_on[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->kaps_scan_cfg_on[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_db_infos_kaps_scan_cfg_paused_dump(int  unit,  int  kaps_scan_cfg_paused_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = kaps_scan_cfg_paused_idx_0, I0 = kaps_scan_cfg_paused_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb kaps_scan_cfg_paused") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb kaps_scan_cfg_paused\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_db_infos/kaps_scan_cfg_paused.txt",
            "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","kaps_scan_cfg_paused[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));

        if (i0 == -1) {
            I0 = 2;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->kaps_scan_cfg_paused
                , sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->kaps_scan_cfg_paused), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 2)
        {
            LOG_CLI((BSL_META("mdb_db_infos[]->((mdb_tables_info*)sw_state_roots_array[][MDB_MODULE_ID])->kaps_scan_cfg_paused[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(2 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "mdb_db_infos[%d]->","((mdb_tables_info*)sw_state_roots_array[%d][MDB_MODULE_ID])->","kaps_scan_cfg_paused[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->kaps_scan_cfg_paused[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t mdb_db_infos_info[SOC_MAX_NUM_DEVICES][MDB_DB_INFOS_INFO_NOF_ENTRIES];
const char* mdb_db_infos_layout_str[MDB_DB_INFOS_INFO_NOF_ENTRIES] = {
    "MDB_DB_INFOS~",
    "MDB_DB_INFOS~EM_KEY_FORMAT~",
    "MDB_DB_INFOS~EM_KEY_FORMAT~KEY_SIZE~",
    "MDB_DB_INFOS~EM_KEY_FORMAT~FORMAT_COUNT~",
    "MDB_DB_INFOS~EM_KEY_FORMAT~KEY_TO_FORMAT_MAP~",
    "MDB_DB_INFOS~EM_KEY_FORMAT~EM_BANK_INFO~",
    "MDB_DB_INFOS~EM_KEY_FORMAT~EM_BANK_INFO~IS_FLEX~",
    "MDB_DB_INFOS~EM_KEY_FORMAT~EM_BANK_INFO~BANK_SIZE_USED_MACRO_B~",
    "MDB_DB_INFOS~EM_KEY_FORMAT~EM_BANK_INFO~MACRO_INDEX~",
    "MDB_DB_INFOS~EM_KEY_FORMAT~EM_BANK_INFO~MACRO_TYPE~",
    "MDB_DB_INFOS~EM_KEY_FORMAT~EM_BANK_INFO~CLUSTER_MAPPING~",
    "MDB_DB_INFOS~EM_KEY_FORMAT~NOF_ADDRESS_BITS~",
    "MDB_DB_INFOS~DB~",
    "MDB_DB_INFOS~DB~NUMBER_OF_ALLOC_CLUSTERS~",
    "MDB_DB_INFOS~DB~NUMBER_OF_CLUSTERS~",
    "MDB_DB_INFOS~DB~NOF_CLUSTER_PER_ENTRY~",
    "MDB_DB_INFOS~DB~CLUSTERS_INFO~",
    "MDB_DB_INFOS~DB~CLUSTERS_INFO~MACRO_TYPE~",
    "MDB_DB_INFOS~DB~CLUSTERS_INFO~MACRO_INDEX~",
    "MDB_DB_INFOS~DB~CLUSTERS_INFO~CLUSTER_INDEX~",
    "MDB_DB_INFOS~DB~CLUSTERS_INFO~CLUSTER_POSITION_IN_ENTRY~",
    "MDB_DB_INFOS~DB~CLUSTERS_INFO~START_ADDRESS~",
    "MDB_DB_INFOS~DB~CLUSTERS_INFO~END_ADDRESS~",
    "MDB_DB_INFOS~CAPACITY~",
    "MDB_DB_INFOS~EM_ENTRY_COUNT~",
    "MDB_DB_INFOS~EM_ENTRY_CAPACITY_ESTIMATE~",
    "MDB_DB_INFOS~EM_ENTRY_CAPACITY_ESTIMATE_AVERAGE~",
    "MDB_DB_INFOS~EEDB_PAYLOAD_TYPE~",
    "MDB_DB_INFOS~MDB_CLUSTER_INFOS~",
    "MDB_DB_INFOS~MDB_CLUSTER_INFOS~CACHE_ENABLED~",
    "MDB_DB_INFOS~MDB_CLUSTER_INFOS~MACRO_CLUSTER_ASSOC~",
    "MDB_DB_INFOS~MDB_CLUSTER_INFOS~MACRO_CLUSTER_ASSOC~PDB~",
    "MDB_DB_INFOS~MDB_CLUSTER_INFOS~MACRO_CLUSTER_ASSOC~FLAGS~",
    "MDB_DB_INFOS~MDB_CLUSTER_INFOS~MACRO_CLUSTER_CACHE~",
    "MDB_DB_INFOS~MDB_CLUSTER_INFOS~MACRO_CLUSTER_CACHE~DATA~",
    "MDB_DB_INFOS~MDB_CLUSTER_INFOS~EEDB_BANKS_HITBIT~",
    "MDB_DB_INFOS~MDB_CLUSTER_INFOS~SMALL_KAPS_BB_HITBIT~",
    "MDB_DB_INFOS~MDB_CLUSTER_INFOS~SMALL_KAPS_RPB_HITBIT~",
    "MDB_DB_INFOS~MDB_CLUSTER_INFOS~DIRECT_ADDRESS_MAPPING~",
    "MDB_DB_INFOS~MDB_CLUSTER_INFOS~EEDB_ADDRESS_MAPPING~",
    "MDB_DB_INFOS~MAX_FEC_ID_VALUE~",
    "MDB_DB_INFOS~EEDB_BANK_TRAFFIC_LOCK~",
    "MDB_DB_INFOS~EEDB_BANK_TRAFFIC_LOCK~LOCAL_LIFS~",
    "MDB_DB_INFOS~EEDB_BANK_TRAFFIC_LOCK~GLOBAL_LIF_DESTINATION~",
    "MDB_DB_INFOS~FEC_HIERARCHY_INFO~",
    "MDB_DB_INFOS~FEC_HIERARCHY_INFO~FEC_HIERARCHY_MAP~",
    "MDB_DB_INFOS~FEC_HIERARCHY_INFO~FEC_ID_RANGE_START~",
    "MDB_DB_INFOS~FEC_HIERARCHY_INFO~FEC_ID_RANGE_SIZE~",
    "MDB_DB_INFOS~MDB_PROFILE~",
    "MDB_DB_INFOS~MDB_EM_INIT_DONE~",
    "MDB_DB_INFOS~KAPS_SCAN_CFG_ON~",
    "MDB_DB_INFOS~KAPS_SCAN_CFG_PAUSED~",
};
#endif 
#undef BSL_LOG_MODULE
