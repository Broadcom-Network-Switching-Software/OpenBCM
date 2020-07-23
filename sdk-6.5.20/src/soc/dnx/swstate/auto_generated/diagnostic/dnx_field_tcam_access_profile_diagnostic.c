
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_access_profile_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_field_tcam_access_profile_sw_t * dnx_field_tcam_access_profile_sw_data[SOC_MAX_NUM_DEVICES];



int
dnx_field_tcam_access_profile_sw_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw_access_profiles_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_profile_sw_access_profiles_dump(int  unit,  int  access_profiles_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw_access_profiles_occupied_dump(unit, access_profiles_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw_access_profiles_key_size_dump(unit, access_profiles_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw_access_profiles_action_size_dump(unit, access_profiles_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw_access_profiles_bank_ids_bmp_dump(unit, access_profiles_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw_access_profiles_is_direct_dump(unit, access_profiles_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw_access_profiles_prefix_dump(unit, access_profiles_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_sw_access_profiles_handler_id_dump(unit, access_profiles_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_profile_sw_access_profiles_occupied_dump(int  unit,  int  access_profiles_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = access_profiles_idx_0, I0 = access_profiles_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_access_profile access_profiles occupied") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_access_profile access_profiles occupied\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_access_profile_sw/access_profiles/occupied.txt",
            "dnx_field_tcam_access_profile_sw[%d]->","((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->","access_profiles[].occupied: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
                ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles
                , sizeof(*((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID, ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_access_profile_sw[]->((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[].occupied: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID, ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_tcam_access_profile_sw[%d]->","((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->","access_profiles[%s%d].occupied: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[i0].occupied);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_profile_sw_access_profiles_key_size_dump(int  unit,  int  access_profiles_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = access_profiles_idx_0, I0 = access_profiles_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_key_length_type_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_access_profile access_profiles key_size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_access_profile access_profiles key_size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_access_profile_sw/access_profiles/key_size.txt",
            "dnx_field_tcam_access_profile_sw[%d]->","((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->","access_profiles[].key_size: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
                ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles
                , sizeof(*((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID, ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_access_profile_sw[]->((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[].key_size: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID, ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[i0].key_size,
                "dnx_field_tcam_access_profile_sw[%d]->","((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->","access_profiles[%s%d].key_size: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[i0].key_size,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_profile_sw_access_profiles_action_size_dump(int  unit,  int  access_profiles_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = access_profiles_idx_0, I0 = access_profiles_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_action_length_type_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_access_profile access_profiles action_size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_access_profile access_profiles action_size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_access_profile_sw/access_profiles/action_size.txt",
            "dnx_field_tcam_access_profile_sw[%d]->","((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->","access_profiles[].action_size: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
                ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles
                , sizeof(*((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID, ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_access_profile_sw[]->((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[].action_size: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID, ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[i0].action_size,
                "dnx_field_tcam_access_profile_sw[%d]->","((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->","access_profiles[%s%d].action_size: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[i0].action_size,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_profile_sw_access_profiles_bank_ids_bmp_dump(int  unit,  int  access_profiles_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = access_profiles_idx_0, I0 = access_profiles_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint16") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_access_profile access_profiles bank_ids_bmp") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_access_profile access_profiles bank_ids_bmp\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_access_profile_sw/access_profiles/bank_ids_bmp.txt",
            "dnx_field_tcam_access_profile_sw[%d]->","((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->","access_profiles[].bank_ids_bmp: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
                ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles
                , sizeof(*((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID, ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_access_profile_sw[]->((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[].bank_ids_bmp: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID, ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_tcam_access_profile_sw[%d]->","((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->","access_profiles[%s%d].bank_ids_bmp: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint16(
                unit,
                &((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[i0].bank_ids_bmp);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_profile_sw_access_profiles_is_direct_dump(int  unit,  int  access_profiles_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = access_profiles_idx_0, I0 = access_profiles_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_access_profile access_profiles is_direct") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_access_profile access_profiles is_direct\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_access_profile_sw/access_profiles/is_direct.txt",
            "dnx_field_tcam_access_profile_sw[%d]->","((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->","access_profiles[].is_direct: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
                ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles
                , sizeof(*((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID, ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_access_profile_sw[]->((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[].is_direct: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID, ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_tcam_access_profile_sw[%d]->","((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->","access_profiles[%s%d].is_direct: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[i0].is_direct);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_profile_sw_access_profiles_prefix_dump(int  unit,  int  access_profiles_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = access_profiles_idx_0, I0 = access_profiles_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_access_profile access_profiles prefix") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_access_profile access_profiles prefix\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_access_profile_sw/access_profiles/prefix.txt",
            "dnx_field_tcam_access_profile_sw[%d]->","((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->","access_profiles[].prefix: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
                ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles
                , sizeof(*((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID, ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_access_profile_sw[]->((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[].prefix: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID, ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_tcam_access_profile_sw[%d]->","((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->","access_profiles[%s%d].prefix: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[i0].prefix);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_profile_sw_access_profiles_handler_id_dump(int  unit,  int  access_profiles_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = access_profiles_idx_0, I0 = access_profiles_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_access_profile access_profiles handler_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_access_profile access_profiles handler_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_access_profile_sw/access_profiles/handler_id.txt",
            "dnx_field_tcam_access_profile_sw[%d]->","((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->","access_profiles[].handler_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID,
                ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles
                , sizeof(*((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID, ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_access_profile_sw[]->((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[].handler_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID, ((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_tcam_access_profile_sw[%d]->","((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->","access_profiles[%s%d].handler_id: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_field_tcam_access_profile_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID])->access_profiles[i0].handler_id);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_field_tcam_access_profile_sw_info[SOC_MAX_NUM_DEVICES][DNX_FIELD_TCAM_ACCESS_PROFILE_SW_INFO_NOF_ENTRIES];
const char* dnx_field_tcam_access_profile_sw_layout_str[DNX_FIELD_TCAM_ACCESS_PROFILE_SW_INFO_NOF_ENTRIES] = {
    "DNX_FIELD_TCAM_ACCESS_PROFILE_SW~",
    "DNX_FIELD_TCAM_ACCESS_PROFILE_SW~ACCESS_PROFILES~",
    "DNX_FIELD_TCAM_ACCESS_PROFILE_SW~ACCESS_PROFILES~OCCUPIED~",
    "DNX_FIELD_TCAM_ACCESS_PROFILE_SW~ACCESS_PROFILES~KEY_SIZE~",
    "DNX_FIELD_TCAM_ACCESS_PROFILE_SW~ACCESS_PROFILES~ACTION_SIZE~",
    "DNX_FIELD_TCAM_ACCESS_PROFILE_SW~ACCESS_PROFILES~BANK_IDS_BMP~",
    "DNX_FIELD_TCAM_ACCESS_PROFILE_SW~ACCESS_PROFILES~IS_DIRECT~",
    "DNX_FIELD_TCAM_ACCESS_PROFILE_SW~ACCESS_PROFILES~PREFIX~",
    "DNX_FIELD_TCAM_ACCESS_PROFILE_SW~ACCESS_PROFILES~HANDLER_ID~",
};
#endif 
#undef BSL_LOG_MODULE
