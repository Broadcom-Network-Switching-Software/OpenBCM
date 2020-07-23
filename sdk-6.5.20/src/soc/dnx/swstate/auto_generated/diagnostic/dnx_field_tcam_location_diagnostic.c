
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_location_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_field_tcam_location_sw_t * dnx_field_tcam_location_sw_data[SOC_MAX_NUM_DEVICES];



int
dnx_field_tcam_location_sw_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw_tcam_handlers_info_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw_location_priority_arr_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_handlers_info_dump(int  unit,  int  tcam_handlers_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_dump(unit, tcam_handlers_info_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_dump(unit, tcam_handlers_info_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_dump(int  unit,  int  tcam_handlers_info_idx_0,  int  entries_occupation_bitmap_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = tcam_handlers_info_idx_0, I0 = tcam_handlers_info_idx_0 + 1;
    int i1 = entries_occupation_bitmap_idx_0, I1 = entries_occupation_bitmap_idx_0 + 1, org_i1 = entries_occupation_bitmap_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_occ_bm_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_location tcam_handlers_info entries_occupation_bitmap") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_location tcam_handlers_info entries_occupation_bitmap\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_location_sw/tcam_handlers_info/entries_occupation_bitmap.txt",
            "dnx_field_tcam_location_sw[%d]->","((dnx_field_tcam_location_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->","tcam_handlers_info[].entries_occupation_bitmap[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_LOCATION_MODULE_ID,
                ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info
                , sizeof(*((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_LOCATION_MODULE_ID, ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_location_sw[]->((dnx_field_tcam_location_sw_t*)sw_state_roots_array[][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[].entries_occupation_bitmap[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_LOCATION_MODULE_ID, ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[i0].entries_occupation_bitmap);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_FIELD_TCAM_LOCATION_MODULE_ID,
                    ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[i0].entries_occupation_bitmap);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[i0].entries_occupation_bitmap
                    , sizeof(*((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[i0].entries_occupation_bitmap), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_LOCATION_MODULE_ID, ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[i0].entries_occupation_bitmap))
            {
                LOG_CLI((BSL_META("dnx_field_tcam_location_sw[]->((dnx_field_tcam_location_sw_t*)sw_state_roots_array[][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[].entries_occupation_bitmap[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_LOCATION_MODULE_ID, ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[i0].entries_occupation_bitmap) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_field_tcam_location_sw[%d]->","((dnx_field_tcam_location_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->","tcam_handlers_info[%s%d].entries_occupation_bitmap[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                SW_STATE_OCC_BM_DEFAULT_PRINT(
                    unit,
                    &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[i0].entries_occupation_bitmap[i1]);

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
dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_dump(int  unit,  int  tcam_handlers_info_idx_0,  int  priorities_range_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = tcam_handlers_info_idx_0, I0 = tcam_handlers_info_idx_0 + 1;
    int i1 = priorities_range_idx_0, I1 = priorities_range_idx_0 + 1, org_i1 = priorities_range_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_sorted_ll_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_location tcam_handlers_info priorities_range") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_location tcam_handlers_info priorities_range\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_location_sw/tcam_handlers_info/priorities_range.txt",
            "dnx_field_tcam_location_sw[%d]->","((dnx_field_tcam_location_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->","tcam_handlers_info[].priorities_range[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_LOCATION_MODULE_ID,
                ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info
                , sizeof(*((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_LOCATION_MODULE_ID, ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_location_sw[]->((dnx_field_tcam_location_sw_t*)sw_state_roots_array[][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[].priorities_range[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_LOCATION_MODULE_ID, ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[i0].priorities_range);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_FIELD_TCAM_LOCATION_MODULE_ID,
                    ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[i0].priorities_range);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[i0].priorities_range
                    , sizeof(*((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[i0].priorities_range), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_LOCATION_MODULE_ID, ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[i0].priorities_range))
            {
                LOG_CLI((BSL_META("dnx_field_tcam_location_sw[]->((dnx_field_tcam_location_sw_t*)sw_state_roots_array[][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[].priorities_range[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_LOCATION_MODULE_ID, ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[i0].priorities_range) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_field_tcam_location_sw[%d]->","((dnx_field_tcam_location_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->","tcam_handlers_info[%s%d].priorities_range[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                SW_STATE_LL_DEFAULT_PRINT(
                    unit,
                    &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[i0].priorities_range[i1]);

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
dnx_field_tcam_location_sw_location_priority_arr_dump(int  unit,  int  location_priority_arr_idx_0,  int  location_priority_arr_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = location_priority_arr_idx_0, I0 = location_priority_arr_idx_0 + 1;
    int i1 = location_priority_arr_idx_1, I1 = location_priority_arr_idx_1 + 1, org_i1 = location_priority_arr_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_location location_priority_arr") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_location location_priority_arr\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_location_sw/location_priority_arr.txt",
            "dnx_field_tcam_location_sw[%d]->","((dnx_field_tcam_location_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->","location_priority_arr[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->location_priority_arr
                , sizeof(*((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->location_priority_arr), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES)
        {
            LOG_CLI((BSL_META("dnx_field_tcam_location_sw[]->((dnx_field_tcam_location_sw_t*)sw_state_roots_array[][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->location_priority_arr[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->location_priority_arr[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_FIELD_TCAM_LOCATION_MODULE_ID,
                    ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->location_priority_arr[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->location_priority_arr[i0]
                    , sizeof(*((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->location_priority_arr[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_LOCATION_MODULE_ID, ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->location_priority_arr[i0]))
            {
                LOG_CLI((BSL_META("dnx_field_tcam_location_sw[]->((dnx_field_tcam_location_sw_t*)sw_state_roots_array[][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->location_priority_arr[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_LOCATION_MODULE_ID, ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->location_priority_arr[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_field_tcam_location_sw[%d]->","((dnx_field_tcam_location_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->","location_priority_arr[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->location_priority_arr[i0][i1]);

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
dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_dump(int  unit,  int  tcam_banks_occupation_bitmap_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = tcam_banks_occupation_bitmap_idx_0, I0 = tcam_banks_occupation_bitmap_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_occ_bm_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_location tcam_banks_occupation_bitmap") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_location tcam_banks_occupation_bitmap\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_location_sw/tcam_banks_occupation_bitmap.txt",
            "dnx_field_tcam_location_sw[%d]->","((dnx_field_tcam_location_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->","tcam_banks_occupation_bitmap[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_LOCATION_MODULE_ID,
                ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap
                , sizeof(*((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_LOCATION_MODULE_ID, ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_location_sw[]->((dnx_field_tcam_location_sw_t*)sw_state_roots_array[][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_LOCATION_MODULE_ID, ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_tcam_location_sw[%d]->","((dnx_field_tcam_location_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->","tcam_banks_occupation_bitmap[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            SW_STATE_OCC_BM_DEFAULT_PRINT(
                unit,
                &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_field_tcam_location_sw_info[SOC_MAX_NUM_DEVICES][DNX_FIELD_TCAM_LOCATION_SW_INFO_NOF_ENTRIES];
const char* dnx_field_tcam_location_sw_layout_str[DNX_FIELD_TCAM_LOCATION_SW_INFO_NOF_ENTRIES] = {
    "DNX_FIELD_TCAM_LOCATION_SW~",
    "DNX_FIELD_TCAM_LOCATION_SW~TCAM_HANDLERS_INFO~",
    "DNX_FIELD_TCAM_LOCATION_SW~TCAM_HANDLERS_INFO~ENTRIES_OCCUPATION_BITMAP~",
    "DNX_FIELD_TCAM_LOCATION_SW~TCAM_HANDLERS_INFO~PRIORITIES_RANGE~",
    "DNX_FIELD_TCAM_LOCATION_SW~LOCATION_PRIORITY_ARR~",
    "DNX_FIELD_TCAM_LOCATION_SW~TCAM_BANKS_OCCUPATION_BITMAP~",
};
#endif 
#undef BSL_LOG_MODULE
