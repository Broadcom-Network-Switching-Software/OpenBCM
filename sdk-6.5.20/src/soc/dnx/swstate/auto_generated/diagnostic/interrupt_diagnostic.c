
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
#include <soc/dnx/swstate/auto_generated/diagnostic/interrupt_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern soc_dnx_intr_t * intr_db_data[SOC_MAX_NUM_DEVICES];



int
intr_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(intr_db_flags_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(intr_db_storm_timed_count_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(intr_db_storm_timed_period_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(intr_db_storm_nominal_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
intr_db_flags_dump(int  unit,  int  flags_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = flags_idx_0, I0 = flags_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "interrupt flags") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate interrupt flags\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "intr_db/flags.txt",
            "intr_db[%d]->","((soc_dnx_intr_t*)sw_state_roots_array[%d][INTERRUPT_MODULE_ID])->","flags[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->flags);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                INTERRUPT_MODULE_ID,
                ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->flags);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->flags
                , sizeof(*((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->flags), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, INTERRUPT_MODULE_ID, ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->flags))
        {
            LOG_CLI((BSL_META("intr_db[]->((soc_dnx_intr_t*)sw_state_roots_array[][INTERRUPT_MODULE_ID])->flags[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, INTERRUPT_MODULE_ID, ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->flags) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "intr_db[%d]->","((soc_dnx_intr_t*)sw_state_roots_array[%d][INTERRUPT_MODULE_ID])->","flags[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->flags[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
intr_db_storm_timed_count_dump(int  unit,  int  storm_timed_count_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = storm_timed_count_idx_0, I0 = storm_timed_count_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "interrupt storm_timed_count") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate interrupt storm_timed_count\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "intr_db/storm_timed_count.txt",
            "intr_db[%d]->","((soc_dnx_intr_t*)sw_state_roots_array[%d][INTERRUPT_MODULE_ID])->","storm_timed_count[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_count);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                INTERRUPT_MODULE_ID,
                ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_count);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_count
                , sizeof(*((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_count), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, INTERRUPT_MODULE_ID, ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_count))
        {
            LOG_CLI((BSL_META("intr_db[]->((soc_dnx_intr_t*)sw_state_roots_array[][INTERRUPT_MODULE_ID])->storm_timed_count[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, INTERRUPT_MODULE_ID, ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_count) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "intr_db[%d]->","((soc_dnx_intr_t*)sw_state_roots_array[%d][INTERRUPT_MODULE_ID])->","storm_timed_count[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_count[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
intr_db_storm_timed_period_dump(int  unit,  int  storm_timed_period_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = storm_timed_period_idx_0, I0 = storm_timed_period_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "interrupt storm_timed_period") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate interrupt storm_timed_period\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "intr_db/storm_timed_period.txt",
            "intr_db[%d]->","((soc_dnx_intr_t*)sw_state_roots_array[%d][INTERRUPT_MODULE_ID])->","storm_timed_period[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_period);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                INTERRUPT_MODULE_ID,
                ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_period);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_period
                , sizeof(*((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_period), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, INTERRUPT_MODULE_ID, ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_period))
        {
            LOG_CLI((BSL_META("intr_db[]->((soc_dnx_intr_t*)sw_state_roots_array[][INTERRUPT_MODULE_ID])->storm_timed_period[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, INTERRUPT_MODULE_ID, ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_period) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "intr_db[%d]->","((soc_dnx_intr_t*)sw_state_roots_array[%d][INTERRUPT_MODULE_ID])->","storm_timed_period[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_timed_period[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
intr_db_storm_nominal_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "interrupt storm_nominal") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate interrupt storm_nominal\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "intr_db/storm_nominal.txt",
            "intr_db[%d]->","((soc_dnx_intr_t*)sw_state_roots_array[%d][INTERRUPT_MODULE_ID])->","storm_nominal: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "intr_db[%d]->","((soc_dnx_intr_t*)sw_state_roots_array[%d][INTERRUPT_MODULE_ID])->","storm_nominal: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint32(
            unit,
            &((soc_dnx_intr_t*)sw_state_roots_array[unit][INTERRUPT_MODULE_ID])->storm_nominal);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t intr_db_info[SOC_MAX_NUM_DEVICES][INTR_DB_INFO_NOF_ENTRIES];
const char* intr_db_layout_str[INTR_DB_INFO_NOF_ENTRIES] = {
    "INTR_DB~",
    "INTR_DB~FLAGS~",
    "INTR_DB~STORM_TIMED_COUNT~",
    "INTR_DB~STORM_TIMED_PERIOD~",
    "INTR_DB~STORM_NOMINAL~",
};
#endif 
#undef BSL_LOG_MODULE
