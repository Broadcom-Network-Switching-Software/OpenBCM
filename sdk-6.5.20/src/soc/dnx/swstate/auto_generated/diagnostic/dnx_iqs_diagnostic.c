
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_iqs_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_iqs_db_t * dnx_iqs_db_data[SOC_MAX_NUM_DEVICES];



int
dnx_iqs_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_iqs_db_credit_request_profile_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnx_iqs_db_credit_watchdog_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_iqs_db_credit_request_profile_dump(int  unit,  int  credit_request_profile_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_iqs_db_credit_request_profile_valid_dump(unit, credit_request_profile_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_iqs_db_credit_request_profile_thresholds_dump(unit, credit_request_profile_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_iqs_db_credit_request_profile_valid_dump(int  unit,  int  credit_request_profile_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = credit_request_profile_idx_0, I0 = credit_request_profile_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_iqs credit_request_profile valid") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_iqs credit_request_profile valid\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_iqs_db/credit_request_profile/valid.txt",
            "dnx_iqs_db[%d]->","((dnx_iqs_db_t*)sw_state_roots_array[%d][DNX_IQS_MODULE_ID])->","credit_request_profile[].valid: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID])->credit_request_profile);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_IQS_MODULE_ID,
                ((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID])->credit_request_profile);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID])->credit_request_profile
                , sizeof(*((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID])->credit_request_profile), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_IQS_MODULE_ID, ((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID])->credit_request_profile))
        {
            LOG_CLI((BSL_META("dnx_iqs_db[]->((dnx_iqs_db_t*)sw_state_roots_array[][DNX_IQS_MODULE_ID])->credit_request_profile[].valid: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_IQS_MODULE_ID, ((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID])->credit_request_profile) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_iqs_db[%d]->","((dnx_iqs_db_t*)sw_state_roots_array[%d][DNX_IQS_MODULE_ID])->","credit_request_profile[%s%d].valid: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID])->credit_request_profile[i0].valid);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_iqs_db_credit_request_profile_thresholds_dump(int  unit,  int  credit_request_profile_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = credit_request_profile_idx_0, I0 = credit_request_profile_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "bcm_cosq_delay_tolerance_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_iqs credit_request_profile thresholds") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_iqs credit_request_profile thresholds\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_iqs_db/credit_request_profile/thresholds.txt",
            "dnx_iqs_db[%d]->","((dnx_iqs_db_t*)sw_state_roots_array[%d][DNX_IQS_MODULE_ID])->","credit_request_profile[].thresholds: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID])->credit_request_profile);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_IQS_MODULE_ID,
                ((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID])->credit_request_profile);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID])->credit_request_profile
                , sizeof(*((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID])->credit_request_profile), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_IQS_MODULE_ID, ((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID])->credit_request_profile))
        {
            LOG_CLI((BSL_META("dnx_iqs_db[]->((dnx_iqs_db_t*)sw_state_roots_array[][DNX_IQS_MODULE_ID])->credit_request_profile[].thresholds: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_IQS_MODULE_ID, ((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID])->credit_request_profile) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID])->credit_request_profile[i0].thresholds,
                "dnx_iqs_db[%d]->","((dnx_iqs_db_t*)sw_state_roots_array[%d][DNX_IQS_MODULE_ID])->","credit_request_profile[%s%d].thresholds: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID])->credit_request_profile[i0].thresholds,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_iqs_db_credit_watchdog_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_iqs_db_credit_watchdog_queue_min_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_iqs_db_credit_watchdog_queue_max_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_iqs_db_credit_watchdog_enable_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_iqs_db_credit_watchdog_retransmit_th_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_iqs_db_credit_watchdog_delete_th_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_iqs_db_credit_watchdog_queue_min_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_iqs credit_watchdog queue_min") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_iqs credit_watchdog queue_min\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_iqs_db/credit_watchdog/queue_min.txt",
            "dnx_iqs_db[%d]->","((dnx_iqs_db_t*)sw_state_roots_array[%d][DNX_IQS_MODULE_ID])->","credit_watchdog.queue_min: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_iqs_db[%d]->","((dnx_iqs_db_t*)sw_state_roots_array[%d][DNX_IQS_MODULE_ID])->","credit_watchdog.queue_min: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID])->credit_watchdog.queue_min);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_iqs_db_credit_watchdog_queue_max_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_iqs credit_watchdog queue_max") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_iqs credit_watchdog queue_max\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_iqs_db/credit_watchdog/queue_max.txt",
            "dnx_iqs_db[%d]->","((dnx_iqs_db_t*)sw_state_roots_array[%d][DNX_IQS_MODULE_ID])->","credit_watchdog.queue_max: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_iqs_db[%d]->","((dnx_iqs_db_t*)sw_state_roots_array[%d][DNX_IQS_MODULE_ID])->","credit_watchdog.queue_max: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID])->credit_watchdog.queue_max);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_iqs_db_credit_watchdog_enable_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_iqs credit_watchdog enable") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_iqs credit_watchdog enable\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_iqs_db/credit_watchdog/enable.txt",
            "dnx_iqs_db[%d]->","((dnx_iqs_db_t*)sw_state_roots_array[%d][DNX_IQS_MODULE_ID])->","credit_watchdog.enable: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_iqs_db[%d]->","((dnx_iqs_db_t*)sw_state_roots_array[%d][DNX_IQS_MODULE_ID])->","credit_watchdog.enable: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID])->credit_watchdog.enable);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_iqs_db_credit_watchdog_retransmit_th_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_iqs credit_watchdog retransmit_th") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_iqs credit_watchdog retransmit_th\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_iqs_db/credit_watchdog/retransmit_th.txt",
            "dnx_iqs_db[%d]->","((dnx_iqs_db_t*)sw_state_roots_array[%d][DNX_IQS_MODULE_ID])->","credit_watchdog.retransmit_th: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_iqs_db[%d]->","((dnx_iqs_db_t*)sw_state_roots_array[%d][DNX_IQS_MODULE_ID])->","credit_watchdog.retransmit_th: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID])->credit_watchdog.retransmit_th);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_iqs_db_credit_watchdog_delete_th_dump(int  unit,  int  delete_th_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = delete_th_idx_0, I0 = delete_th_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_iqs credit_watchdog delete_th") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_iqs credit_watchdog delete_th\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_iqs_db/credit_watchdog/delete_th.txt",
            "dnx_iqs_db[%d]->","((dnx_iqs_db_t*)sw_state_roots_array[%d][DNX_IQS_MODULE_ID])->","credit_watchdog.delete_th[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID])->credit_watchdog.delete_th);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_IQS_MODULE_ID,
                ((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID])->credit_watchdog.delete_th);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID])->credit_watchdog.delete_th
                , sizeof(*((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID])->credit_watchdog.delete_th), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_IQS_MODULE_ID, ((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID])->credit_watchdog.delete_th))
        {
            LOG_CLI((BSL_META("dnx_iqs_db[]->((dnx_iqs_db_t*)sw_state_roots_array[][DNX_IQS_MODULE_ID])->credit_watchdog.delete_th[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_IQS_MODULE_ID, ((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID])->credit_watchdog.delete_th) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_iqs_db[%d]->","((dnx_iqs_db_t*)sw_state_roots_array[%d][DNX_IQS_MODULE_ID])->","credit_watchdog.delete_th[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_iqs_db_t*)sw_state_roots_array[unit][DNX_IQS_MODULE_ID])->credit_watchdog.delete_th[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_iqs_db_info[SOC_MAX_NUM_DEVICES][DNX_IQS_DB_INFO_NOF_ENTRIES];
const char* dnx_iqs_db_layout_str[DNX_IQS_DB_INFO_NOF_ENTRIES] = {
    "DNX_IQS_DB~",
    "DNX_IQS_DB~CREDIT_REQUEST_PROFILE~",
    "DNX_IQS_DB~CREDIT_REQUEST_PROFILE~VALID~",
    "DNX_IQS_DB~CREDIT_REQUEST_PROFILE~THRESHOLDS~",
    "DNX_IQS_DB~CREDIT_WATCHDOG~",
    "DNX_IQS_DB~CREDIT_WATCHDOG~QUEUE_MIN~",
    "DNX_IQS_DB~CREDIT_WATCHDOG~QUEUE_MAX~",
    "DNX_IQS_DB~CREDIT_WATCHDOG~ENABLE~",
    "DNX_IQS_DB~CREDIT_WATCHDOG~RETRANSMIT_TH~",
    "DNX_IQS_DB~CREDIT_WATCHDOG~DELETE_TH~",
};
#endif 
#undef BSL_LOG_MODULE
