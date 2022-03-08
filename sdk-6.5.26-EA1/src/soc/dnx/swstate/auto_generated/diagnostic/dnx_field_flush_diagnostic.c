
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_flush_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_field_flush_layout.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_field_flush_sw_t * dnx_field_flush_sw_data[SOC_MAX_NUM_DEVICES];



int
dnx_field_flush_sw_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_flush_sw_flush_profile_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_flush_sw_flush_profile_dump(int  unit,  int  flush_profile_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_flush_sw_flush_profile_is_mapped_dump(unit, flush_profile_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_flush_sw_flush_profile_mapped_fg_dump(unit, flush_profile_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_flush_sw_flush_profile_is_mapped_dump(int  unit,  int  flush_profile_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = flush_profile_idx_0, I0 = flush_profile_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_flush flush_profile is_mapped") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_flush flush_profile is_mapped\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_flush_sw/flush_profile/is_mapped.txt",
            "dnx_field_flush_sw[%d]->","((dnx_field_flush_sw_t*)sw_state_roots_array[%d][DNX_FIELD_FLUSH_MODULE_ID])->","flush_profile[].is_mapped: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__IS_MAPPED,
                ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile
                , sizeof(*((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__IS_MAPPED, ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile))
        {
            LOG_CLI((BSL_META("dnx_field_flush_sw[]->((dnx_field_flush_sw_t*)sw_state_roots_array[][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile[].is_mapped: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__IS_MAPPED, ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_flush_sw[%d]->","((dnx_field_flush_sw_t*)sw_state_roots_array[%d][DNX_FIELD_FLUSH_MODULE_ID])->","flush_profile[%s%d].is_mapped: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile[i0].is_mapped);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_flush_sw_flush_profile_mapped_fg_dump(int  unit,  int  flush_profile_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = flush_profile_idx_0, I0 = flush_profile_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_group_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_flush flush_profile mapped_fg") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_flush flush_profile mapped_fg\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_flush_sw/flush_profile/mapped_fg.txt",
            "dnx_field_flush_sw[%d]->","((dnx_field_flush_sw_t*)sw_state_roots_array[%d][DNX_FIELD_FLUSH_MODULE_ID])->","flush_profile[].mapped_fg: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__MAPPED_FG,
                ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile
                , sizeof(*((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__MAPPED_FG, ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile))
        {
            LOG_CLI((BSL_META("dnx_field_flush_sw[]->((dnx_field_flush_sw_t*)sw_state_roots_array[][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile[].mapped_fg: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__MAPPED_FG, ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile[i0].mapped_fg,
                "dnx_field_flush_sw[%d]->","((dnx_field_flush_sw_t*)sw_state_roots_array[%d][DNX_FIELD_FLUSH_MODULE_ID])->","flush_profile[%s%d].mapped_fg: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_field_flush_sw_t*)sw_state_roots_array[unit][DNX_FIELD_FLUSH_MODULE_ID])->flush_profile[i0].mapped_fg,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_field_flush_sw_info[SOC_MAX_NUM_DEVICES][DNX_FIELD_FLUSH_SW_INFO_NOF_ENTRIES];
const char* dnx_field_flush_sw_layout_str[DNX_FIELD_FLUSH_SW_INFO_NOF_ENTRIES] = {
    "DNX_FIELD_FLUSH_SW~",
    "DNX_FIELD_FLUSH_SW~FLUSH_PROFILE~",
    "DNX_FIELD_FLUSH_SW~FLUSH_PROFILE~IS_MAPPED~",
    "DNX_FIELD_FLUSH_SW~FLUSH_PROFILE~MAPPED_FG~",
};
#endif 
#undef BSL_LOG_MODULE
