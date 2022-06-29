
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/diagnostic/mdb_kaps_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/mdb_kaps_layout.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern mdb_kaps_sw_state_t * mdb_kaps_db_data[SOC_MAX_NUM_DEVICES];



int
mdb_kaps_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(mdb_kaps_db_db_info_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(mdb_kaps_db_search_instruction_info_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(mdb_kaps_db_big_kaps_large_bb_mode_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(mdb_kaps_db_ad_size_supported_dump(unit, -1, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_kaps_db_db_info_dump(int  unit,  int  db_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(mdb_kaps_db_db_info_db_p_dump(unit, db_info_idx_0, filters));
    SHR_IF_ERR_EXIT(mdb_kaps_db_db_info_hb_db_p_dump(unit, db_info_idx_0, filters));
    SHR_IF_ERR_EXIT(mdb_kaps_db_db_info_ad_db_p_arr_dump(unit, db_info_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(mdb_kaps_db_db_info_ad_db_size_arr_dump(unit, db_info_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(mdb_kaps_db_db_info_is_valid_dump(unit, db_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_kaps_db_db_info_db_p_dump(int  unit,  int  db_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_kaps_db_db_info_hb_db_p_dump(int  unit,  int  db_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_kaps_db_db_info_ad_db_p_arr_dump(int  unit,  int  db_info_idx_0,  int  ad_db_p_arr_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_kaps_db_db_info_ad_db_size_arr_dump(int  unit,  int  db_info_idx_0,  int  ad_db_size_arr_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = db_info_idx_0, I0 = db_info_idx_0 + 1;
    int i1 = ad_db_size_arr_idx_0, I1 = ad_db_size_arr_idx_0 + 1, org_i1 = ad_db_size_arr_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "mdb_lpm_ad_size_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb_kaps db_info ad_db_size_arr") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb_kaps db_info ad_db_size_arr\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_kaps_db/db_info/ad_db_size_arr.txt",
            "mdb_kaps_db[%d]->","((mdb_kaps_sw_state_t*)sw_state_roots_array[%d][MDB_KAPS_MODULE_ID])->","db_info[].ad_db_size_arr[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_MDB_KAPS_DB__DB_INFO__AD_DB_SIZE_ARR,
                ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info
                , sizeof(*((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_MDB_KAPS_DB__DB_INFO__AD_DB_SIZE_ARR, ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info))
        {
            LOG_CLI((BSL_META("mdb_kaps_db[]->((mdb_kaps_sw_state_t*)sw_state_roots_array[][MDB_KAPS_MODULE_ID])->db_info[].ad_db_size_arr[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_MDB_KAPS_DB__DB_INFO__AD_DB_SIZE_ARR, ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_MDB_KAPS_MAX_NOF_RESULT_TYPES;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info[i0].ad_db_size_arr
                    , sizeof(*((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info[i0].ad_db_size_arr), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_MDB_KAPS_MAX_NOF_RESULT_TYPES)
            {
                LOG_CLI((BSL_META("mdb_kaps_db[]->((mdb_kaps_sw_state_t*)sw_state_roots_array[][MDB_KAPS_MODULE_ID])->db_info[].ad_db_size_arr[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_MDB_KAPS_MAX_NOF_RESULT_TYPES == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                    unit,
                    ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info[i0].ad_db_size_arr[i1],
                    "mdb_kaps_db[%d]->","((mdb_kaps_sw_state_t*)sw_state_roots_array[%d][MDB_KAPS_MODULE_ID])->","db_info[%s%d].ad_db_size_arr[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_OPAQUE_FILE(
                    unit,
                    ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info[i0].ad_db_size_arr[i1],
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
mdb_kaps_db_db_info_is_valid_dump(int  unit,  int  db_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = db_info_idx_0, I0 = db_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb_kaps db_info is_valid") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb_kaps db_info is_valid\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_kaps_db/db_info/is_valid.txt",
            "mdb_kaps_db[%d]->","((mdb_kaps_sw_state_t*)sw_state_roots_array[%d][MDB_KAPS_MODULE_ID])->","db_info[].is_valid: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_MDB_KAPS_DB__DB_INFO__IS_VALID,
                ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info
                , sizeof(*((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_MDB_KAPS_DB__DB_INFO__IS_VALID, ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info))
        {
            LOG_CLI((BSL_META("mdb_kaps_db[]->((mdb_kaps_sw_state_t*)sw_state_roots_array[][MDB_KAPS_MODULE_ID])->db_info[].is_valid: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_MDB_KAPS_DB__DB_INFO__IS_VALID, ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "mdb_kaps_db[%d]->","((mdb_kaps_sw_state_t*)sw_state_roots_array[%d][MDB_KAPS_MODULE_ID])->","db_info[%s%d].is_valid: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->db_info[i0].is_valid);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_kaps_db_search_instruction_info_dump(int  unit,  int  search_instruction_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(mdb_kaps_db_search_instruction_info_inst_p_dump(unit, search_instruction_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_kaps_db_search_instruction_info_inst_p_dump(int  unit,  int  search_instruction_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_kaps_db_big_kaps_large_bb_mode_dump(int  unit,  int  big_kaps_large_bb_mode_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = big_kaps_large_bb_mode_idx_0, I0 = big_kaps_large_bb_mode_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb_kaps big_kaps_large_bb_mode") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb_kaps big_kaps_large_bb_mode\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_kaps_db/big_kaps_large_bb_mode.txt",
            "mdb_kaps_db[%d]->","((mdb_kaps_sw_state_t*)sw_state_roots_array[%d][MDB_KAPS_MODULE_ID])->","big_kaps_large_bb_mode[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_MDB_KAPS_NOF_BIG_BBS_BLK_IDS;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->big_kaps_large_bb_mode
                , sizeof(*((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->big_kaps_large_bb_mode), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_MDB_KAPS_NOF_BIG_BBS_BLK_IDS)
        {
            LOG_CLI((BSL_META("mdb_kaps_db[]->((mdb_kaps_sw_state_t*)sw_state_roots_array[][MDB_KAPS_MODULE_ID])->big_kaps_large_bb_mode[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_MDB_KAPS_NOF_BIG_BBS_BLK_IDS == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "mdb_kaps_db[%d]->","((mdb_kaps_sw_state_t*)sw_state_roots_array[%d][MDB_KAPS_MODULE_ID])->","big_kaps_large_bb_mode[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->big_kaps_large_bb_mode[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_kaps_db_ad_size_supported_dump(int  unit,  int  ad_size_supported_idx_0,  int  ad_size_supported_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = ad_size_supported_idx_0, I0 = ad_size_supported_idx_0 + 1;
    int i1 = ad_size_supported_idx_1, I1 = ad_size_supported_idx_1 + 1, org_i1 = ad_size_supported_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "mdb_kaps ad_size_supported") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate mdb_kaps ad_size_supported\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "mdb_kaps_db/ad_size_supported.txt",
            "mdb_kaps_db[%d]->","((mdb_kaps_sw_state_t*)sw_state_roots_array[%d][MDB_KAPS_MODULE_ID])->","ad_size_supported[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->ad_size_supported);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_MDB_KAPS_DB__AD_SIZE_SUPPORTED,
                ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->ad_size_supported);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->ad_size_supported
                , sizeof(*((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->ad_size_supported), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_MDB_KAPS_DB__AD_SIZE_SUPPORTED, ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->ad_size_supported))
        {
            LOG_CLI((BSL_META("mdb_kaps_db[]->((mdb_kaps_sw_state_t*)sw_state_roots_array[][MDB_KAPS_MODULE_ID])->ad_size_supported[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_MDB_KAPS_DB__AD_SIZE_SUPPORTED, ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->ad_size_supported) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = MDB_NOF_LPM_AD_SIZES;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->ad_size_supported[i0]
                    , sizeof(*((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->ad_size_supported[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= MDB_NOF_LPM_AD_SIZES)
            {
                LOG_CLI((BSL_META("mdb_kaps_db[]->((mdb_kaps_sw_state_t*)sw_state_roots_array[][MDB_KAPS_MODULE_ID])->ad_size_supported[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(MDB_NOF_LPM_AD_SIZES == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "mdb_kaps_db[%d]->","((mdb_kaps_sw_state_t*)sw_state_roots_array[%d][MDB_KAPS_MODULE_ID])->","ad_size_supported[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((mdb_kaps_sw_state_t*)sw_state_roots_array[unit][MDB_KAPS_MODULE_ID])->ad_size_supported[i0][i1]);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




dnx_sw_state_diagnostic_info_t mdb_kaps_db_info[SOC_MAX_NUM_DEVICES][MDB_KAPS_DB_INFO_NOF_ENTRIES];
const char* mdb_kaps_db_layout_str[MDB_KAPS_DB_INFO_NOF_ENTRIES] = {
    "MDB_KAPS_DB~",
    "MDB_KAPS_DB~DB_INFO~",
    "MDB_KAPS_DB~DB_INFO~DB_P~",
    "MDB_KAPS_DB~DB_INFO~HB_DB_P~",
    "MDB_KAPS_DB~DB_INFO~AD_DB_P_ARR~",
    "MDB_KAPS_DB~DB_INFO~AD_DB_SIZE_ARR~",
    "MDB_KAPS_DB~DB_INFO~IS_VALID~",
    "MDB_KAPS_DB~SEARCH_INSTRUCTION_INFO~",
    "MDB_KAPS_DB~SEARCH_INSTRUCTION_INFO~INST_P~",
    "MDB_KAPS_DB~BIG_KAPS_LARGE_BB_MODE~",
    "MDB_KAPS_DB~AD_SIZE_SUPPORTED~",
};
#endif 
#undef BSL_LOG_MODULE
