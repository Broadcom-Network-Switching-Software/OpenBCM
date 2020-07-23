
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
#include <soc/dnx/swstate/auto_generated/diagnostic/mdb_em_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern mdb_em_sw_state_t * mdb_em_db_data[SOC_MAX_NUM_DEVICES];



int
mdb_em_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(mdb_em_db_shadow_em_db_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(mdb_em_db_vmv_info_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(mdb_em_db_mact_stamp_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_em_db_shadow_em_db_dump(int  unit,  int  shadow_em_db_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = shadow_em_db_idx_0, I0 = shadow_em_db_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_htbl_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb_em shadow_em_db") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb_em shadow_em_db\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_em_db/shadow_em_db.txt",
            "mdb_em_db[%d]->","((mdb_em_sw_state_t*)sw_state_roots_array[%d][MDB_EM_MODULE_ID])->","shadow_em_db[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_PHYSICAL_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->shadow_em_db
                , sizeof(*((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->shadow_em_db), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_PHYSICAL_TABLES)
        {
            LOG_CLI((BSL_META("mdb_em_db[]->((mdb_em_sw_state_t*)sw_state_roots_array[][MDB_EM_MODULE_ID])->shadow_em_db[]: ")));
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
                "mdb_em_db[%d]->","((mdb_em_sw_state_t*)sw_state_roots_array[%d][MDB_EM_MODULE_ID])->","shadow_em_db[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            SW_STATE_HTB_DEFAULT_PRINT(
                unit,
                MDB_EM_MODULE_ID,
                &((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->shadow_em_db[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_em_db_vmv_info_dump(int  unit,  int  vmv_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(mdb_em_db_vmv_info_value_dump(unit, vmv_info_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(mdb_em_db_vmv_info_size_dump(unit, vmv_info_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(mdb_em_db_vmv_info_encoding_map_dump(unit, vmv_info_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(mdb_em_db_vmv_info_app_id_size_dump(unit, vmv_info_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(mdb_em_db_vmv_info_max_payload_size_dump(unit, vmv_info_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_em_db_vmv_info_value_dump(int  unit,  int  vmv_info_idx_0,  int  value_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = vmv_info_idx_0, I0 = vmv_info_idx_0 + 1;
    int i1 = value_idx_0, I1 = value_idx_0 + 1, org_i1 = value_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb_em vmv_info value") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb_em vmv_info value\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_em_db/vmv_info/value.txt",
            "mdb_em_db[%d]->","((mdb_em_sw_state_t*)sw_state_roots_array[%d][MDB_EM_MODULE_ID])->","vmv_info[].value[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_PHYSICAL_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info
                , sizeof(*((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_PHYSICAL_TABLES)
        {
            LOG_CLI((BSL_META("mdb_em_db[]->((mdb_em_sw_state_t*)sw_state_roots_array[][MDB_EM_MODULE_ID])->vmv_info[].value[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_PHYSICAL_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_MDB_EM_NOF_ENCODING_VALUES;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[i0].value
                    , sizeof(*((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[i0].value), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_MDB_EM_NOF_ENCODING_VALUES)
            {
                LOG_CLI((BSL_META("mdb_em_db[]->((mdb_em_sw_state_t*)sw_state_roots_array[][MDB_EM_MODULE_ID])->vmv_info[].value[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_MDB_EM_NOF_ENCODING_VALUES == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "mdb_em_db[%d]->","((mdb_em_sw_state_t*)sw_state_roots_array[%d][MDB_EM_MODULE_ID])->","vmv_info[%s%d].value[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[i0].value[i1]);

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
mdb_em_db_vmv_info_size_dump(int  unit,  int  vmv_info_idx_0,  int  size_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = vmv_info_idx_0, I0 = vmv_info_idx_0 + 1;
    int i1 = size_idx_0, I1 = size_idx_0 + 1, org_i1 = size_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb_em vmv_info size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb_em vmv_info size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_em_db/vmv_info/size.txt",
            "mdb_em_db[%d]->","((mdb_em_sw_state_t*)sw_state_roots_array[%d][MDB_EM_MODULE_ID])->","vmv_info[].size[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_PHYSICAL_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info
                , sizeof(*((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_PHYSICAL_TABLES)
        {
            LOG_CLI((BSL_META("mdb_em_db[]->((mdb_em_sw_state_t*)sw_state_roots_array[][MDB_EM_MODULE_ID])->vmv_info[].size[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_PHYSICAL_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_MDB_EM_NOF_ENCODING_VALUES;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[i0].size
                    , sizeof(*((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[i0].size), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_MDB_EM_NOF_ENCODING_VALUES)
            {
                LOG_CLI((BSL_META("mdb_em_db[]->((mdb_em_sw_state_t*)sw_state_roots_array[][MDB_EM_MODULE_ID])->vmv_info[].size[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_MDB_EM_NOF_ENCODING_VALUES == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "mdb_em_db[%d]->","((mdb_em_sw_state_t*)sw_state_roots_array[%d][MDB_EM_MODULE_ID])->","vmv_info[%s%d].size[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[i0].size[i1]);

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
mdb_em_db_vmv_info_encoding_map_dump(int  unit,  int  vmv_info_idx_0,  int  encoding_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(mdb_em_db_vmv_info_encoding_map_encoding_dump(unit, vmv_info_idx_0, encoding_map_idx_0, filters));
    SHR_IF_ERR_EXIT(mdb_em_db_vmv_info_encoding_map_size_dump(unit, vmv_info_idx_0, encoding_map_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
mdb_em_db_vmv_info_encoding_map_encoding_dump(int  unit,  int  vmv_info_idx_0,  int  encoding_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = vmv_info_idx_0, I0 = vmv_info_idx_0 + 1;
    int i1 = encoding_map_idx_0, I1 = encoding_map_idx_0 + 1, org_i1 = encoding_map_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb_em vmv_info encoding_map encoding") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb_em vmv_info encoding_map encoding\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_em_db/vmv_info/encoding_map/encoding.txt",
            "mdb_em_db[%d]->","((mdb_em_sw_state_t*)sw_state_roots_array[%d][MDB_EM_MODULE_ID])->","vmv_info[].encoding_map[].encoding: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_PHYSICAL_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info
                , sizeof(*((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_PHYSICAL_TABLES)
        {
            LOG_CLI((BSL_META("mdb_em_db[]->((mdb_em_sw_state_t*)sw_state_roots_array[][MDB_EM_MODULE_ID])->vmv_info[].encoding_map[].encoding: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_PHYSICAL_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_MDB_EM_VMV_NOF_VALUES;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[i0].encoding_map
                    , sizeof(*((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[i0].encoding_map), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_MDB_EM_VMV_NOF_VALUES)
            {
                LOG_CLI((BSL_META("mdb_em_db[]->((mdb_em_sw_state_t*)sw_state_roots_array[][MDB_EM_MODULE_ID])->vmv_info[].encoding_map[].encoding: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_MDB_EM_VMV_NOF_VALUES == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "mdb_em_db[%d]->","((mdb_em_sw_state_t*)sw_state_roots_array[%d][MDB_EM_MODULE_ID])->","vmv_info[%s%d].encoding_map[%s%d].encoding: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[i0].encoding_map[i1].encoding);

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
mdb_em_db_vmv_info_encoding_map_size_dump(int  unit,  int  vmv_info_idx_0,  int  encoding_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = vmv_info_idx_0, I0 = vmv_info_idx_0 + 1;
    int i1 = encoding_map_idx_0, I1 = encoding_map_idx_0 + 1, org_i1 = encoding_map_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb_em vmv_info encoding_map size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb_em vmv_info encoding_map size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_em_db/vmv_info/encoding_map/size.txt",
            "mdb_em_db[%d]->","((mdb_em_sw_state_t*)sw_state_roots_array[%d][MDB_EM_MODULE_ID])->","vmv_info[].encoding_map[].size: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_PHYSICAL_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info
                , sizeof(*((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_PHYSICAL_TABLES)
        {
            LOG_CLI((BSL_META("mdb_em_db[]->((mdb_em_sw_state_t*)sw_state_roots_array[][MDB_EM_MODULE_ID])->vmv_info[].encoding_map[].size: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_PHYSICAL_TABLES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_MDB_EM_VMV_NOF_VALUES;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[i0].encoding_map
                    , sizeof(*((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[i0].encoding_map), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_MDB_EM_VMV_NOF_VALUES)
            {
                LOG_CLI((BSL_META("mdb_em_db[]->((mdb_em_sw_state_t*)sw_state_roots_array[][MDB_EM_MODULE_ID])->vmv_info[].encoding_map[].size: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_MDB_EM_VMV_NOF_VALUES == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "mdb_em_db[%d]->","((mdb_em_sw_state_t*)sw_state_roots_array[%d][MDB_EM_MODULE_ID])->","vmv_info[%s%d].encoding_map[%s%d].size: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[i0].encoding_map[i1].size);

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
mdb_em_db_vmv_info_app_id_size_dump(int  unit,  int  vmv_info_idx_0,  int  app_id_size_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = vmv_info_idx_0, I0 = vmv_info_idx_0 + 1;
    int i1 = app_id_size_idx_0, I1 = app_id_size_idx_0 + 1, org_i1 = app_id_size_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb_em vmv_info app_id_size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb_em vmv_info app_id_size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_em_db/vmv_info/app_id_size.txt",
            "mdb_em_db[%d]->","((mdb_em_sw_state_t*)sw_state_roots_array[%d][MDB_EM_MODULE_ID])->","vmv_info[].app_id_size[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_PHYSICAL_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info
                , sizeof(*((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_PHYSICAL_TABLES)
        {
            LOG_CLI((BSL_META("mdb_em_db[]->((mdb_em_sw_state_t*)sw_state_roots_array[][MDB_EM_MODULE_ID])->vmv_info[].app_id_size[]: ")));
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
                    ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[i0].app_id_size
                    , sizeof(*((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[i0].app_id_size), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_MDB_EM_MAX_NOF_TIDS)
            {
                LOG_CLI((BSL_META("mdb_em_db[]->((mdb_em_sw_state_t*)sw_state_roots_array[][MDB_EM_MODULE_ID])->vmv_info[].app_id_size[]: ")));
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
                    "mdb_em_db[%d]->","((mdb_em_sw_state_t*)sw_state_roots_array[%d][MDB_EM_MODULE_ID])->","vmv_info[%s%d].app_id_size[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[i0].app_id_size[i1]);

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
mdb_em_db_vmv_info_max_payload_size_dump(int  unit,  int  vmv_info_idx_0,  int  max_payload_size_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = vmv_info_idx_0, I0 = vmv_info_idx_0 + 1;
    int i1 = max_payload_size_idx_0, I1 = max_payload_size_idx_0 + 1, org_i1 = max_payload_size_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb_em vmv_info max_payload_size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb_em vmv_info max_payload_size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_em_db/vmv_info/max_payload_size.txt",
            "mdb_em_db[%d]->","((mdb_em_sw_state_t*)sw_state_roots_array[%d][MDB_EM_MODULE_ID])->","vmv_info[].max_payload_size[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_PHYSICAL_TABLES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info
                , sizeof(*((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_PHYSICAL_TABLES)
        {
            LOG_CLI((BSL_META("mdb_em_db[]->((mdb_em_sw_state_t*)sw_state_roots_array[][MDB_EM_MODULE_ID])->vmv_info[].max_payload_size[]: ")));
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
                    ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[i0].max_payload_size
                    , sizeof(*((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[i0].max_payload_size), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_MDB_EM_MAX_NOF_TIDS)
            {
                LOG_CLI((BSL_META("mdb_em_db[]->((mdb_em_sw_state_t*)sw_state_roots_array[][MDB_EM_MODULE_ID])->vmv_info[].max_payload_size[]: ")));
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
                    "mdb_em_db[%d]->","((mdb_em_sw_state_t*)sw_state_roots_array[%d][MDB_EM_MODULE_ID])->","vmv_info[%s%d].max_payload_size[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->vmv_info[i0].max_payload_size[i1]);

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
mdb_em_db_mact_stamp_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb_em mact_stamp") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb_em mact_stamp\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_em_db/mact_stamp.txt",
            "mdb_em_db[%d]->","((mdb_em_sw_state_t*)sw_state_roots_array[%d][MDB_EM_MODULE_ID])->","mact_stamp: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "mdb_em_db[%d]->","((mdb_em_sw_state_t*)sw_state_roots_array[%d][MDB_EM_MODULE_ID])->","mact_stamp: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint32(
            unit,
            &((mdb_em_sw_state_t*)sw_state_roots_array[unit][MDB_EM_MODULE_ID])->mact_stamp);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t mdb_em_db_info[SOC_MAX_NUM_DEVICES][MDB_EM_DB_INFO_NOF_ENTRIES];
const char* mdb_em_db_layout_str[MDB_EM_DB_INFO_NOF_ENTRIES] = {
    "MDB_EM_DB~",
    "MDB_EM_DB~SHADOW_EM_DB~",
    "MDB_EM_DB~VMV_INFO~",
    "MDB_EM_DB~VMV_INFO~VALUE~",
    "MDB_EM_DB~VMV_INFO~SIZE~",
    "MDB_EM_DB~VMV_INFO~ENCODING_MAP~",
    "MDB_EM_DB~VMV_INFO~ENCODING_MAP~ENCODING~",
    "MDB_EM_DB~VMV_INFO~ENCODING_MAP~SIZE~",
    "MDB_EM_DB~VMV_INFO~APP_ID_SIZE~",
    "MDB_EM_DB~VMV_INFO~MAX_PAYLOAD_SIZE~",
    "MDB_EM_DB~MACT_STAMP~",
};
#endif 
#undef BSL_LOG_MODULE
