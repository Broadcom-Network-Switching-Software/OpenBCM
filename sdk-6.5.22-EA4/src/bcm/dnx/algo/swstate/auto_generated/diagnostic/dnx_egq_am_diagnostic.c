/** \file algo/swstate/auto_generated/diagnostic/dnx_egq_am_diagnostic.c
 *
 * sw state functions definitions
 *
 * DO NOT EDIT THIS FILE!
 * This file is auto-generated.
 * Edits to this file will be lost when it is regenerated.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <bcm_int/dnx/algo/swstate/auto_generated/diagnostic/dnx_egq_am_diagnostic.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
/*
 * Global Variables
 */

extern dnx_egq_am_db_t * dnx_egq_am_db_data[SOC_MAX_NUM_DEVICES];
/*
 * FUNCTIONs
 */

/*
 *
 * dump function for the variable dnx_egq_am_db
 * AUTO-GENERATED - DO NOT MODIFY
 */
int
dnx_egq_am_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_egq_am_db_egr_tc_dp_mapping_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnx_egq_am_db_fqp_profile_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



/*
 *
 * dump function for the variable egr_tc_dp_mapping
 * AUTO-GENERATED - DO NOT MODIFY
 */
int
dnx_egq_am_db_egr_tc_dp_mapping_dump(int  unit,  int  egr_tc_dp_mapping_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = egr_tc_dp_mapping_idx_0, I0 = egr_tc_dp_mapping_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_algo_template_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_egq_am egr_tc_dp_mapping") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_egq_am egr_tc_dp_mapping\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_egq_am_db/egr_tc_dp_mapping.txt",
            "dnx_egq_am_db[%d]->","((dnx_egq_am_db_t*)sw_state_roots_array[%d][DNX_EGQ_AM_MODULE_ID])->","egr_tc_dp_mapping[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_egq_am_db_t*)sw_state_roots_array[unit][DNX_EGQ_AM_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_egq_am_db_t*)sw_state_roots_array[unit][DNX_EGQ_AM_MODULE_ID])->egr_tc_dp_mapping);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_EGQ_AM_MODULE_ID,
                ((dnx_egq_am_db_t*)sw_state_roots_array[unit][DNX_EGQ_AM_MODULE_ID])->egr_tc_dp_mapping);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_egq_am_db_t*)sw_state_roots_array[unit][DNX_EGQ_AM_MODULE_ID])->egr_tc_dp_mapping
                , sizeof(*((dnx_egq_am_db_t*)sw_state_roots_array[unit][DNX_EGQ_AM_MODULE_ID])->egr_tc_dp_mapping), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_EGQ_AM_MODULE_ID, ((dnx_egq_am_db_t*)sw_state_roots_array[unit][DNX_EGQ_AM_MODULE_ID])->egr_tc_dp_mapping))
        {
            LOG_CLI((BSL_META("dnx_egq_am_db[]->((dnx_egq_am_db_t*)sw_state_roots_array[][DNX_EGQ_AM_MODULE_ID])->egr_tc_dp_mapping[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_EGQ_AM_MODULE_ID, ((dnx_egq_am_db_t*)sw_state_roots_array[unit][DNX_EGQ_AM_MODULE_ID])->egr_tc_dp_mapping) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_egq_am_db[%d]->","((dnx_egq_am_db_t*)sw_state_roots_array[%d][DNX_EGQ_AM_MODULE_ID])->","egr_tc_dp_mapping[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            DNX_ALGO_TEMP_MNGR_PRINT(
                unit,
                DNX_EGQ_AM_MODULE_ID,
                &((dnx_egq_am_db_t*)sw_state_roots_array[unit][DNX_EGQ_AM_MODULE_ID])->egr_tc_dp_mapping[i0],
                dnx_algo_egr_tc_dp_mapping_print_cb);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



/*
 *
 * dump function for the variable fqp_profile
 * AUTO-GENERATED - DO NOT MODIFY
 */
int
dnx_egq_am_db_fqp_profile_dump(int  unit,  int  fqp_profile_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fqp_profile_idx_0, I0 = fqp_profile_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_algo_template_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_egq_am fqp_profile") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_egq_am fqp_profile\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_egq_am_db/fqp_profile.txt",
            "dnx_egq_am_db[%d]->","((dnx_egq_am_db_t*)sw_state_roots_array[%d][DNX_EGQ_AM_MODULE_ID])->","fqp_profile[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_egq_am_db_t*)sw_state_roots_array[unit][DNX_EGQ_AM_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_egq_am_db_t*)sw_state_roots_array[unit][DNX_EGQ_AM_MODULE_ID])->fqp_profile);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_EGQ_AM_MODULE_ID,
                ((dnx_egq_am_db_t*)sw_state_roots_array[unit][DNX_EGQ_AM_MODULE_ID])->fqp_profile);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_egq_am_db_t*)sw_state_roots_array[unit][DNX_EGQ_AM_MODULE_ID])->fqp_profile
                , sizeof(*((dnx_egq_am_db_t*)sw_state_roots_array[unit][DNX_EGQ_AM_MODULE_ID])->fqp_profile), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_EGQ_AM_MODULE_ID, ((dnx_egq_am_db_t*)sw_state_roots_array[unit][DNX_EGQ_AM_MODULE_ID])->fqp_profile))
        {
            LOG_CLI((BSL_META("dnx_egq_am_db[]->((dnx_egq_am_db_t*)sw_state_roots_array[][DNX_EGQ_AM_MODULE_ID])->fqp_profile[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_EGQ_AM_MODULE_ID, ((dnx_egq_am_db_t*)sw_state_roots_array[unit][DNX_EGQ_AM_MODULE_ID])->fqp_profile) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_egq_am_db[%d]->","((dnx_egq_am_db_t*)sw_state_roots_array[%d][DNX_EGQ_AM_MODULE_ID])->","fqp_profile[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            DNX_ALGO_TEMP_MNGR_PRINT(
                unit,
                DNX_EGQ_AM_MODULE_ID,
                &((dnx_egq_am_db_t*)sw_state_roots_array[unit][DNX_EGQ_AM_MODULE_ID])->fqp_profile[i0],
                dnx_algo_fqp_profile_params_print_cb);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



/*
 * Global Variables
 */

dnx_sw_state_diagnostic_info_t dnx_egq_am_db_info[SOC_MAX_NUM_DEVICES][DNX_EGQ_AM_DB_INFO_NOF_ENTRIES];
const char* dnx_egq_am_db_layout_str[DNX_EGQ_AM_DB_INFO_NOF_ENTRIES] = {
    "DNX_EGQ_AM_DB~",
    "DNX_EGQ_AM_DB~EGR_TC_DP_MAPPING~",
    "DNX_EGQ_AM_DB~FQP_PROFILE~",
};
#endif /* DNX_SW_STATE_DIAGNOSTIC */
#undef BSL_LOG_MODULE
