
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_actions_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_field_action_sw_db_t * dnx_field_action_sw_db_data[SOC_MAX_NUM_DEVICES];



int
dnx_field_action_sw_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_action_sw_db_info_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnx_field_action_sw_db_fem_info_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_action_sw_db_info_dump(int  unit,  int  info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_action_sw_db_info_valid_dump(unit, info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_action_sw_db_info_field_id_dump(unit, info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_action_sw_db_info_size_dump(unit, info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_action_sw_db_info_prefix_dump(unit, info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_action_sw_db_info_prefix_size_dump(unit, info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_action_sw_db_info_bcm_id_dump(unit, info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_action_sw_db_info_ref_count_dump(unit, info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_action_sw_db_info_flags_dump(unit, info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_action_sw_db_info_base_dnx_action_dump(unit, info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_action_sw_db_info_valid_dump(int  unit,  int  info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = info_idx_0, I0 = info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_actions info valid") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_actions info valid\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_action_sw_db/info/valid.txt",
            "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","info[].valid: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_ACTION_USER_NOF;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info
                , sizeof(*((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_ACTION_USER_NOF)
        {
            LOG_CLI((BSL_META("dnx_field_action_sw_db[]->((dnx_field_action_sw_db_t*)sw_state_roots_array[][DNX_FIELD_ACTIONS_MODULE_ID])->info[].valid: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_ACTION_USER_NOF == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","info[%s%d].valid: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info[i0].valid);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_action_sw_db_info_field_id_dump(int  unit,  int  info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = info_idx_0, I0 = info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dbal_fields_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_actions info field_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_actions info field_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_action_sw_db/info/field_id.txt",
            "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","info[].field_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_ACTION_USER_NOF;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info
                , sizeof(*((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_ACTION_USER_NOF)
        {
            LOG_CLI((BSL_META("dnx_field_action_sw_db[]->((dnx_field_action_sw_db_t*)sw_state_roots_array[][DNX_FIELD_ACTIONS_MODULE_ID])->info[].field_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_ACTION_USER_NOF == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info[i0].field_id,
                "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","info[%s%d].field_id: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info[i0].field_id,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_action_sw_db_info_size_dump(int  unit,  int  info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = info_idx_0, I0 = info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_actions info size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_actions info size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_action_sw_db/info/size.txt",
            "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","info[].size: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_ACTION_USER_NOF;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info
                , sizeof(*((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_ACTION_USER_NOF)
        {
            LOG_CLI((BSL_META("dnx_field_action_sw_db[]->((dnx_field_action_sw_db_t*)sw_state_roots_array[][DNX_FIELD_ACTIONS_MODULE_ID])->info[].size: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_ACTION_USER_NOF == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","info[%s%d].size: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info[i0].size);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_action_sw_db_info_prefix_dump(int  unit,  int  info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = info_idx_0, I0 = info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_actions info prefix") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_actions info prefix\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_action_sw_db/info/prefix.txt",
            "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","info[].prefix: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_ACTION_USER_NOF;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info
                , sizeof(*((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_ACTION_USER_NOF)
        {
            LOG_CLI((BSL_META("dnx_field_action_sw_db[]->((dnx_field_action_sw_db_t*)sw_state_roots_array[][DNX_FIELD_ACTIONS_MODULE_ID])->info[].prefix: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_ACTION_USER_NOF == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","info[%s%d].prefix: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info[i0].prefix);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_action_sw_db_info_prefix_size_dump(int  unit,  int  info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = info_idx_0, I0 = info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_actions info prefix_size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_actions info prefix_size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_action_sw_db/info/prefix_size.txt",
            "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","info[].prefix_size: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_ACTION_USER_NOF;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info
                , sizeof(*((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_ACTION_USER_NOF)
        {
            LOG_CLI((BSL_META("dnx_field_action_sw_db[]->((dnx_field_action_sw_db_t*)sw_state_roots_array[][DNX_FIELD_ACTIONS_MODULE_ID])->info[].prefix_size: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_ACTION_USER_NOF == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","info[%s%d].prefix_size: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info[i0].prefix_size);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_action_sw_db_info_bcm_id_dump(int  unit,  int  info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = info_idx_0, I0 = info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_actions info bcm_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_actions info bcm_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_action_sw_db/info/bcm_id.txt",
            "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","info[].bcm_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_ACTION_USER_NOF;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info
                , sizeof(*((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_ACTION_USER_NOF)
        {
            LOG_CLI((BSL_META("dnx_field_action_sw_db[]->((dnx_field_action_sw_db_t*)sw_state_roots_array[][DNX_FIELD_ACTIONS_MODULE_ID])->info[].bcm_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_ACTION_USER_NOF == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","info[%s%d].bcm_id: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info[i0].bcm_id);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_action_sw_db_info_ref_count_dump(int  unit,  int  info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = info_idx_0, I0 = info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_actions info ref_count") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_actions info ref_count\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_action_sw_db/info/ref_count.txt",
            "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","info[].ref_count: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_ACTION_USER_NOF;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info
                , sizeof(*((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_ACTION_USER_NOF)
        {
            LOG_CLI((BSL_META("dnx_field_action_sw_db[]->((dnx_field_action_sw_db_t*)sw_state_roots_array[][DNX_FIELD_ACTIONS_MODULE_ID])->info[].ref_count: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_ACTION_USER_NOF == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","info[%s%d].ref_count: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info[i0].ref_count);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_action_sw_db_info_flags_dump(int  unit,  int  info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = info_idx_0, I0 = info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_action_flags_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_actions info flags") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_actions info flags\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_action_sw_db/info/flags.txt",
            "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","info[].flags: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_ACTION_USER_NOF;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info
                , sizeof(*((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_ACTION_USER_NOF)
        {
            LOG_CLI((BSL_META("dnx_field_action_sw_db[]->((dnx_field_action_sw_db_t*)sw_state_roots_array[][DNX_FIELD_ACTIONS_MODULE_ID])->info[].flags: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_ACTION_USER_NOF == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info[i0].flags,
                "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","info[%s%d].flags: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info[i0].flags,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_action_sw_db_info_base_dnx_action_dump(int  unit,  int  info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = info_idx_0, I0 = info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_action_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_actions info base_dnx_action") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_actions info base_dnx_action\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_action_sw_db/info/base_dnx_action.txt",
            "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","info[].base_dnx_action: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_ACTION_USER_NOF;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info
                , sizeof(*((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_ACTION_USER_NOF)
        {
            LOG_CLI((BSL_META("dnx_field_action_sw_db[]->((dnx_field_action_sw_db_t*)sw_state_roots_array[][DNX_FIELD_ACTIONS_MODULE_ID])->info[].base_dnx_action: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_ACTION_USER_NOF == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info[i0].base_dnx_action,
                "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","info[%s%d].base_dnx_action: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->info[i0].base_dnx_action,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_action_sw_db_fem_info_dump(int  unit,  int  fem_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_action_sw_db_fem_info_fg_id_info_dump(unit, fem_info_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_action_sw_db_fem_info_fg_id_info_dump(int  unit,  int  fem_info_idx_0,  int  fg_id_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_action_sw_db_fem_info_fg_id_info_fg_id_dump(unit, fem_info_idx_0, fg_id_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_action_sw_db_fem_info_fg_id_info_input_offset_dump(unit, fem_info_idx_0, fg_id_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_action_sw_db_fem_info_fg_id_info_input_size_dump(unit, fem_info_idx_0, fg_id_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_action_sw_db_fem_info_fg_id_info_second_fg_id_dump(unit, fem_info_idx_0, fg_id_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_action_sw_db_fem_info_fg_id_info_ignore_actions_dump(unit, fem_info_idx_0, fg_id_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_action_sw_db_fem_info_fg_id_info_fem_encoded_actions_dump(unit, fem_info_idx_0, fg_id_info_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_action_sw_db_fem_info_fg_id_info_fg_id_dump(int  unit,  int  fem_info_idx_0,  int  fg_id_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fem_info_idx_0, I0 = fem_info_idx_0 + 1;
    int i1 = fg_id_info_idx_0, I1 = fg_id_info_idx_0 + 1, org_i1 = fg_id_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_group_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_actions fem_info fg_id_info fg_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_actions fem_info fg_id_info fg_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_action_sw_db/fem_info/fg_id_info/fg_id.txt",
            "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","fem_info[].fg_id_info[].fg_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_ACTIONS_MODULE_ID,
                ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info
                , sizeof(*((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_ACTIONS_MODULE_ID, ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info))
        {
            LOG_CLI((BSL_META("dnx_field_action_sw_db[]->((dnx_field_action_sw_db_t*)sw_state_roots_array[][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[].fg_id_info[].fg_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_ACTIONS_MODULE_ID, ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_FIELD_ACTIONS_MODULE_ID,
                    ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info
                    , sizeof(*((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_ACTIONS_MODULE_ID, ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info))
            {
                LOG_CLI((BSL_META("dnx_field_action_sw_db[]->((dnx_field_action_sw_db_t*)sw_state_roots_array[][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[].fg_id_info[].fg_id: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_ACTIONS_MODULE_ID, ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                    unit,
                    ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info[i1].fg_id,
                    "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","fem_info[%s%d].fg_id_info[%s%d].fg_id: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_OPAQUE_FILE(
                    unit,
                    ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info[i1].fg_id,
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
dnx_field_action_sw_db_fem_info_fg_id_info_input_offset_dump(int  unit,  int  fem_info_idx_0,  int  fg_id_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fem_info_idx_0, I0 = fem_info_idx_0 + 1;
    int i1 = fg_id_info_idx_0, I1 = fg_id_info_idx_0 + 1, org_i1 = fg_id_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_actions fem_info fg_id_info input_offset") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_actions fem_info fg_id_info input_offset\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_action_sw_db/fem_info/fg_id_info/input_offset.txt",
            "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","fem_info[].fg_id_info[].input_offset: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_ACTIONS_MODULE_ID,
                ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info
                , sizeof(*((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_ACTIONS_MODULE_ID, ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info))
        {
            LOG_CLI((BSL_META("dnx_field_action_sw_db[]->((dnx_field_action_sw_db_t*)sw_state_roots_array[][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[].fg_id_info[].input_offset: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_ACTIONS_MODULE_ID, ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_FIELD_ACTIONS_MODULE_ID,
                    ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info
                    , sizeof(*((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_ACTIONS_MODULE_ID, ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info))
            {
                LOG_CLI((BSL_META("dnx_field_action_sw_db[]->((dnx_field_action_sw_db_t*)sw_state_roots_array[][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[].fg_id_info[].input_offset: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_ACTIONS_MODULE_ID, ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","fem_info[%s%d].fg_id_info[%s%d].input_offset: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info[i1].input_offset);

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
dnx_field_action_sw_db_fem_info_fg_id_info_input_size_dump(int  unit,  int  fem_info_idx_0,  int  fg_id_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fem_info_idx_0, I0 = fem_info_idx_0 + 1;
    int i1 = fg_id_info_idx_0, I1 = fg_id_info_idx_0 + 1, org_i1 = fg_id_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_actions fem_info fg_id_info input_size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_actions fem_info fg_id_info input_size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_action_sw_db/fem_info/fg_id_info/input_size.txt",
            "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","fem_info[].fg_id_info[].input_size: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_ACTIONS_MODULE_ID,
                ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info
                , sizeof(*((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_ACTIONS_MODULE_ID, ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info))
        {
            LOG_CLI((BSL_META("dnx_field_action_sw_db[]->((dnx_field_action_sw_db_t*)sw_state_roots_array[][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[].fg_id_info[].input_size: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_ACTIONS_MODULE_ID, ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_FIELD_ACTIONS_MODULE_ID,
                    ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info
                    , sizeof(*((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_ACTIONS_MODULE_ID, ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info))
            {
                LOG_CLI((BSL_META("dnx_field_action_sw_db[]->((dnx_field_action_sw_db_t*)sw_state_roots_array[][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[].fg_id_info[].input_size: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_ACTIONS_MODULE_ID, ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","fem_info[%s%d].fg_id_info[%s%d].input_size: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info[i1].input_size);

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
dnx_field_action_sw_db_fem_info_fg_id_info_second_fg_id_dump(int  unit,  int  fem_info_idx_0,  int  fg_id_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fem_info_idx_0, I0 = fem_info_idx_0 + 1;
    int i1 = fg_id_info_idx_0, I1 = fg_id_info_idx_0 + 1, org_i1 = fg_id_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_group_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_actions fem_info fg_id_info second_fg_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_actions fem_info fg_id_info second_fg_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_action_sw_db/fem_info/fg_id_info/second_fg_id.txt",
            "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","fem_info[].fg_id_info[].second_fg_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_ACTIONS_MODULE_ID,
                ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info
                , sizeof(*((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_ACTIONS_MODULE_ID, ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info))
        {
            LOG_CLI((BSL_META("dnx_field_action_sw_db[]->((dnx_field_action_sw_db_t*)sw_state_roots_array[][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[].fg_id_info[].second_fg_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_ACTIONS_MODULE_ID, ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_FIELD_ACTIONS_MODULE_ID,
                    ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info
                    , sizeof(*((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_ACTIONS_MODULE_ID, ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info))
            {
                LOG_CLI((BSL_META("dnx_field_action_sw_db[]->((dnx_field_action_sw_db_t*)sw_state_roots_array[][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[].fg_id_info[].second_fg_id: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_ACTIONS_MODULE_ID, ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                    unit,
                    ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info[i1].second_fg_id,
                    "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","fem_info[%s%d].fg_id_info[%s%d].second_fg_id: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_OPAQUE_FILE(
                    unit,
                    ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info[i1].second_fg_id,
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
dnx_field_action_sw_db_fem_info_fg_id_info_ignore_actions_dump(int  unit,  int  fem_info_idx_0,  int  fg_id_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fem_info_idx_0, I0 = fem_info_idx_0 + 1;
    int i1 = fg_id_info_idx_0, I1 = fg_id_info_idx_0 + 1, org_i1 = fg_id_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_actions fem_info fg_id_info ignore_actions") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_actions fem_info fg_id_info ignore_actions\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_action_sw_db/fem_info/fg_id_info/ignore_actions.txt",
            "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","fem_info[].fg_id_info[].ignore_actions: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_ACTIONS_MODULE_ID,
                ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info
                , sizeof(*((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_ACTIONS_MODULE_ID, ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info))
        {
            LOG_CLI((BSL_META("dnx_field_action_sw_db[]->((dnx_field_action_sw_db_t*)sw_state_roots_array[][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[].fg_id_info[].ignore_actions: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_ACTIONS_MODULE_ID, ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_FIELD_ACTIONS_MODULE_ID,
                    ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info
                    , sizeof(*((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_ACTIONS_MODULE_ID, ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info))
            {
                LOG_CLI((BSL_META("dnx_field_action_sw_db[]->((dnx_field_action_sw_db_t*)sw_state_roots_array[][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[].fg_id_info[].ignore_actions: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_ACTIONS_MODULE_ID, ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","fem_info[%s%d].fg_id_info[%s%d].ignore_actions: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info[i1].ignore_actions);

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
dnx_field_action_sw_db_fem_info_fg_id_info_fem_encoded_actions_dump(int  unit,  int  fem_info_idx_0,  int  fg_id_info_idx_0,  int  fem_encoded_actions_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fem_info_idx_0, I0 = fem_info_idx_0 + 1;
    int i1 = fg_id_info_idx_0, I1 = fg_id_info_idx_0 + 1, org_i1 = fg_id_info_idx_0;
    int i2 = fem_encoded_actions_idx_0, I2 = fem_encoded_actions_idx_0 + 1, org_i2 = fem_encoded_actions_idx_0;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_action_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_actions fem_info fg_id_info fem_encoded_actions") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_actions fem_info fg_id_info fem_encoded_actions\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_action_sw_db/fem_info/fg_id_info/fem_encoded_actions.txt",
            "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","fem_info[].fg_id_info[].fem_encoded_actions[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_ACTIONS_MODULE_ID,
                ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info
                , sizeof(*((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_ACTIONS_MODULE_ID, ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info))
        {
            LOG_CLI((BSL_META("dnx_field_action_sw_db[]->((dnx_field_action_sw_db_t*)sw_state_roots_array[][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[].fg_id_info[].fem_encoded_actions[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_ACTIONS_MODULE_ID, ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_FIELD_ACTIONS_MODULE_ID,
                    ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info
                    , sizeof(*((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_ACTIONS_MODULE_ID, ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info))
            {
                LOG_CLI((BSL_META("dnx_field_action_sw_db[]->((dnx_field_action_sw_db_t*)sw_state_roots_array[][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[].fg_id_info[].fem_encoded_actions[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_ACTIONS_MODULE_ID, ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                
                
                    DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                        unit,
                        ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info[i1].fem_encoded_actions);

                if (i2 == -1) {
                    I2 = dnx_sw_state_get_nof_elements(unit,
                        DNX_FIELD_ACTIONS_MODULE_ID,
                        ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info[i1].fem_encoded_actions);
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info[i1].fem_encoded_actions
                        , sizeof(*((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info[i1].fem_encoded_actions), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_ACTIONS_MODULE_ID, ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info[i1].fem_encoded_actions))
                {
                    LOG_CLI((BSL_META("dnx_field_action_sw_db[]->((dnx_field_action_sw_db_t*)sw_state_roots_array[][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[].fg_id_info[].fem_encoded_actions[]: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_ACTIONS_MODULE_ID, ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info[i1].fem_encoded_actions) == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                        unit,
                        ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info[i1].fem_encoded_actions[i2],
                        "dnx_field_action_sw_db[%d]->","((dnx_field_action_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_ACTIONS_MODULE_ID])->","fem_info[%s%d].fg_id_info[%s%d].fem_encoded_actions[%s%d]: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_OPAQUE_FILE(
                        unit,
                        ((dnx_field_action_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_ACTIONS_MODULE_ID])->fem_info[i0].fg_id_info[i1].fem_encoded_actions[i2],
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





dnx_sw_state_diagnostic_info_t dnx_field_action_sw_db_info[SOC_MAX_NUM_DEVICES][DNX_FIELD_ACTION_SW_DB_INFO_NOF_ENTRIES];
const char* dnx_field_action_sw_db_layout_str[DNX_FIELD_ACTION_SW_DB_INFO_NOF_ENTRIES] = {
    "DNX_FIELD_ACTION_SW_DB~",
    "DNX_FIELD_ACTION_SW_DB~INFO~",
    "DNX_FIELD_ACTION_SW_DB~INFO~VALID~",
    "DNX_FIELD_ACTION_SW_DB~INFO~FIELD_ID~",
    "DNX_FIELD_ACTION_SW_DB~INFO~SIZE~",
    "DNX_FIELD_ACTION_SW_DB~INFO~PREFIX~",
    "DNX_FIELD_ACTION_SW_DB~INFO~PREFIX_SIZE~",
    "DNX_FIELD_ACTION_SW_DB~INFO~BCM_ID~",
    "DNX_FIELD_ACTION_SW_DB~INFO~REF_COUNT~",
    "DNX_FIELD_ACTION_SW_DB~INFO~FLAGS~",
    "DNX_FIELD_ACTION_SW_DB~INFO~BASE_DNX_ACTION~",
    "DNX_FIELD_ACTION_SW_DB~FEM_INFO~",
    "DNX_FIELD_ACTION_SW_DB~FEM_INFO~FG_ID_INFO~",
    "DNX_FIELD_ACTION_SW_DB~FEM_INFO~FG_ID_INFO~FG_ID~",
    "DNX_FIELD_ACTION_SW_DB~FEM_INFO~FG_ID_INFO~INPUT_OFFSET~",
    "DNX_FIELD_ACTION_SW_DB~FEM_INFO~FG_ID_INFO~INPUT_SIZE~",
    "DNX_FIELD_ACTION_SW_DB~FEM_INFO~FG_ID_INFO~SECOND_FG_ID~",
    "DNX_FIELD_ACTION_SW_DB~FEM_INFO~FG_ID_INFO~IGNORE_ACTIONS~",
    "DNX_FIELD_ACTION_SW_DB~FEM_INFO~FG_ID_INFO~FEM_ENCODED_ACTIONS~",
};
#endif 
#undef BSL_LOG_MODULE
