
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_ofp_rate_diagnostic.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_ofp_rate_db_t * dnx_ofp_rate_db_data[SOC_MAX_NUM_DEVICES];



int
dnx_ofp_rate_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db_otm_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db_qpair_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db_tcg_dump(unit, -1, -1, -1, filters));
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db_otm_cal_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db_calcal_is_modified_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db_calcal_len_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ofp_rate_db_otm_dump(int  unit,  int  otm_idx_0,  int  otm_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db_otm_cal_instance_dump(unit, otm_idx_0,otm_idx_1, filters));
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db_otm_shaping_dump(unit, otm_idx_0,otm_idx_1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ofp_rate_db_otm_cal_instance_dump(int  unit,  int  otm_idx_0,  int  otm_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = otm_idx_0, I0 = otm_idx_0 + 1;
    int i1 = otm_idx_1, I1 = otm_idx_1 + 1, org_i1 = otm_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_ofp_rate otm cal_instance") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_ofp_rate otm cal_instance\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_ofp_rate_db/otm/cal_instance.txt",
            "dnx_ofp_rate_db[%d]->","((dnx_ofp_rate_db_t*)sw_state_roots_array[%d][DNX_OFP_RATE_MODULE_ID])->","otm[][].cal_instance: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm
                , sizeof(*((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES)
        {
            LOG_CLI((BSL_META("dnx_ofp_rate_db[]->((dnx_ofp_rate_db_t*)sw_state_roots_array[][DNX_OFP_RATE_MODULE_ID])->otm[][].cal_instance: ")));
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
                    ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_OFP_RATE_MODULE_ID,
                    ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[i0]
                    , sizeof(*((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_OFP_RATE_MODULE_ID, ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[i0]))
            {
                LOG_CLI((BSL_META("dnx_ofp_rate_db[]->((dnx_ofp_rate_db_t*)sw_state_roots_array[][DNX_OFP_RATE_MODULE_ID])->otm[][].cal_instance: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_OFP_RATE_MODULE_ID, ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_ofp_rate_db[%d]->","((dnx_ofp_rate_db_t*)sw_state_roots_array[%d][DNX_OFP_RATE_MODULE_ID])->","otm[%s%d][%s%d].cal_instance: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[i0][i1].cal_instance);

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
dnx_ofp_rate_db_otm_shaping_dump(int  unit,  int  otm_idx_0,  int  otm_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db_otm_shaping_rate_dump(unit, otm_idx_0, otm_idx_1, filters));
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db_otm_shaping_burst_dump(unit, otm_idx_0, otm_idx_1, filters));
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db_otm_shaping_valid_dump(unit, otm_idx_0, otm_idx_1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ofp_rate_db_otm_shaping_rate_dump(int  unit,  int  otm_idx_0,  int  otm_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = otm_idx_0, I0 = otm_idx_0 + 1;
    int i1 = otm_idx_1, I1 = otm_idx_1 + 1, org_i1 = otm_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_ofp_rate otm shaping rate") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_ofp_rate otm shaping rate\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_ofp_rate_db/otm/shaping/rate.txt",
            "dnx_ofp_rate_db[%d]->","((dnx_ofp_rate_db_t*)sw_state_roots_array[%d][DNX_OFP_RATE_MODULE_ID])->","otm[][].shaping.rate: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm
                , sizeof(*((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES)
        {
            LOG_CLI((BSL_META("dnx_ofp_rate_db[]->((dnx_ofp_rate_db_t*)sw_state_roots_array[][DNX_OFP_RATE_MODULE_ID])->otm[][].shaping.rate: ")));
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
                    ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_OFP_RATE_MODULE_ID,
                    ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[i0]
                    , sizeof(*((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_OFP_RATE_MODULE_ID, ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[i0]))
            {
                LOG_CLI((BSL_META("dnx_ofp_rate_db[]->((dnx_ofp_rate_db_t*)sw_state_roots_array[][DNX_OFP_RATE_MODULE_ID])->otm[][].shaping.rate: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_OFP_RATE_MODULE_ID, ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_ofp_rate_db[%d]->","((dnx_ofp_rate_db_t*)sw_state_roots_array[%d][DNX_OFP_RATE_MODULE_ID])->","otm[%s%d][%s%d].shaping.rate: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[i0][i1].shaping.rate);

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
dnx_ofp_rate_db_otm_shaping_burst_dump(int  unit,  int  otm_idx_0,  int  otm_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = otm_idx_0, I0 = otm_idx_0 + 1;
    int i1 = otm_idx_1, I1 = otm_idx_1 + 1, org_i1 = otm_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_ofp_rate otm shaping burst") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_ofp_rate otm shaping burst\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_ofp_rate_db/otm/shaping/burst.txt",
            "dnx_ofp_rate_db[%d]->","((dnx_ofp_rate_db_t*)sw_state_roots_array[%d][DNX_OFP_RATE_MODULE_ID])->","otm[][].shaping.burst: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm
                , sizeof(*((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES)
        {
            LOG_CLI((BSL_META("dnx_ofp_rate_db[]->((dnx_ofp_rate_db_t*)sw_state_roots_array[][DNX_OFP_RATE_MODULE_ID])->otm[][].shaping.burst: ")));
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
                    ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_OFP_RATE_MODULE_ID,
                    ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[i0]
                    , sizeof(*((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_OFP_RATE_MODULE_ID, ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[i0]))
            {
                LOG_CLI((BSL_META("dnx_ofp_rate_db[]->((dnx_ofp_rate_db_t*)sw_state_roots_array[][DNX_OFP_RATE_MODULE_ID])->otm[][].shaping.burst: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_OFP_RATE_MODULE_ID, ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_ofp_rate_db[%d]->","((dnx_ofp_rate_db_t*)sw_state_roots_array[%d][DNX_OFP_RATE_MODULE_ID])->","otm[%s%d][%s%d].shaping.burst: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[i0][i1].shaping.burst);

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
dnx_ofp_rate_db_otm_shaping_valid_dump(int  unit,  int  otm_idx_0,  int  otm_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = otm_idx_0, I0 = otm_idx_0 + 1;
    int i1 = otm_idx_1, I1 = otm_idx_1 + 1, org_i1 = otm_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_ofp_rate otm shaping valid") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_ofp_rate otm shaping valid\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_ofp_rate_db/otm/shaping/valid.txt",
            "dnx_ofp_rate_db[%d]->","((dnx_ofp_rate_db_t*)sw_state_roots_array[%d][DNX_OFP_RATE_MODULE_ID])->","otm[][].shaping.valid: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm
                , sizeof(*((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES)
        {
            LOG_CLI((BSL_META("dnx_ofp_rate_db[]->((dnx_ofp_rate_db_t*)sw_state_roots_array[][DNX_OFP_RATE_MODULE_ID])->otm[][].shaping.valid: ")));
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
                    ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_OFP_RATE_MODULE_ID,
                    ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[i0]
                    , sizeof(*((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_OFP_RATE_MODULE_ID, ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[i0]))
            {
                LOG_CLI((BSL_META("dnx_ofp_rate_db[]->((dnx_ofp_rate_db_t*)sw_state_roots_array[][DNX_OFP_RATE_MODULE_ID])->otm[][].shaping.valid: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_OFP_RATE_MODULE_ID, ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_ofp_rate_db[%d]->","((dnx_ofp_rate_db_t*)sw_state_roots_array[%d][DNX_OFP_RATE_MODULE_ID])->","otm[%s%d][%s%d].shaping.valid: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[i0][i1].shaping.valid);

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
dnx_ofp_rate_db_qpair_dump(int  unit,  int  qpair_idx_0,  int  qpair_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db_qpair_rate_dump(unit, qpair_idx_0,qpair_idx_1, filters));
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db_qpair_valid_dump(unit, qpair_idx_0,qpair_idx_1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ofp_rate_db_qpair_rate_dump(int  unit,  int  qpair_idx_0,  int  qpair_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = qpair_idx_0, I0 = qpair_idx_0 + 1;
    int i1 = qpair_idx_1, I1 = qpair_idx_1 + 1, org_i1 = qpair_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_ofp_rate qpair rate") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_ofp_rate qpair rate\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_ofp_rate_db/qpair/rate.txt",
            "dnx_ofp_rate_db[%d]->","((dnx_ofp_rate_db_t*)sw_state_roots_array[%d][DNX_OFP_RATE_MODULE_ID])->","qpair[][].rate: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->qpair
                , sizeof(*((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->qpair), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES)
        {
            LOG_CLI((BSL_META("dnx_ofp_rate_db[]->((dnx_ofp_rate_db_t*)sw_state_roots_array[][DNX_OFP_RATE_MODULE_ID])->qpair[][].rate: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_Q_PAIRS;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->qpair[i0]
                    , sizeof(*((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->qpair[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_Q_PAIRS)
            {
                LOG_CLI((BSL_META("dnx_ofp_rate_db[]->((dnx_ofp_rate_db_t*)sw_state_roots_array[][DNX_OFP_RATE_MODULE_ID])->qpair[][].rate: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_Q_PAIRS == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_ofp_rate_db[%d]->","((dnx_ofp_rate_db_t*)sw_state_roots_array[%d][DNX_OFP_RATE_MODULE_ID])->","qpair[%s%d][%s%d].rate: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->qpair[i0][i1].rate);

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
dnx_ofp_rate_db_qpair_valid_dump(int  unit,  int  qpair_idx_0,  int  qpair_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = qpair_idx_0, I0 = qpair_idx_0 + 1;
    int i1 = qpair_idx_1, I1 = qpair_idx_1 + 1, org_i1 = qpair_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_ofp_rate qpair valid") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_ofp_rate qpair valid\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_ofp_rate_db/qpair/valid.txt",
            "dnx_ofp_rate_db[%d]->","((dnx_ofp_rate_db_t*)sw_state_roots_array[%d][DNX_OFP_RATE_MODULE_ID])->","qpair[][].valid: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->qpair
                , sizeof(*((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->qpair), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES)
        {
            LOG_CLI((BSL_META("dnx_ofp_rate_db[]->((dnx_ofp_rate_db_t*)sw_state_roots_array[][DNX_OFP_RATE_MODULE_ID])->qpair[][].valid: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_Q_PAIRS;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->qpair[i0]
                    , sizeof(*((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->qpair[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_Q_PAIRS)
            {
                LOG_CLI((BSL_META("dnx_ofp_rate_db[]->((dnx_ofp_rate_db_t*)sw_state_roots_array[][DNX_OFP_RATE_MODULE_ID])->qpair[][].valid: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_Q_PAIRS == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_ofp_rate_db[%d]->","((dnx_ofp_rate_db_t*)sw_state_roots_array[%d][DNX_OFP_RATE_MODULE_ID])->","qpair[%s%d][%s%d].valid: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->qpair[i0][i1].valid);

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
dnx_ofp_rate_db_tcg_dump(int  unit,  int  tcg_idx_0,  int  tcg_idx_1,  int  tcg_idx_2,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db_tcg_rate_dump(unit, tcg_idx_0,tcg_idx_1,tcg_idx_2, filters));
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db_tcg_valid_dump(unit, tcg_idx_0,tcg_idx_1,tcg_idx_2, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ofp_rate_db_tcg_rate_dump(int  unit,  int  tcg_idx_0,  int  tcg_idx_1,  int  tcg_idx_2,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = tcg_idx_0, I0 = tcg_idx_0 + 1;
    int i1 = tcg_idx_1, I1 = tcg_idx_1 + 1, org_i1 = tcg_idx_1;
    int i2 = tcg_idx_2, I2 = tcg_idx_2 + 1, org_i2 = tcg_idx_2;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_ofp_rate tcg rate") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_ofp_rate tcg rate\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_ofp_rate_db/tcg/rate.txt",
            "dnx_ofp_rate_db[%d]->","((dnx_ofp_rate_db_t*)sw_state_roots_array[%d][DNX_OFP_RATE_MODULE_ID])->","tcg[][][].rate: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->tcg
                , sizeof(*((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->tcg), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES)
        {
            LOG_CLI((BSL_META("dnx_ofp_rate_db[]->((dnx_ofp_rate_db_t*)sw_state_roots_array[][DNX_OFP_RATE_MODULE_ID])->tcg[][][].rate: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_PORT_SCHEDULERS;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->tcg[i0]
                    , sizeof(*((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->tcg[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_PORT_SCHEDULERS)
            {
                LOG_CLI((BSL_META("dnx_ofp_rate_db[]->((dnx_ofp_rate_db_t*)sw_state_roots_array[][DNX_OFP_RATE_MODULE_ID])->tcg[][][].rate: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_PORT_SCHEDULERS == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                if (i2 == -1) {
                    I2 = DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_TCG;
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->tcg[i0][i1]
                        , sizeof(*((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->tcg[i0][i1]), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_TCG)
                {
                    LOG_CLI((BSL_META("dnx_ofp_rate_db[]->((dnx_ofp_rate_db_t*)sw_state_roots_array[][DNX_OFP_RATE_MODULE_ID])->tcg[][][].rate: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_TCG == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "dnx_ofp_rate_db[%d]->","((dnx_ofp_rate_db_t*)sw_state_roots_array[%d][DNX_OFP_RATE_MODULE_ID])->","tcg[%s%d][%s%d][%s%d].rate: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_uint32(
                        unit,
                        &((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->tcg[i0][i1][i2].rate);

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




int
dnx_ofp_rate_db_tcg_valid_dump(int  unit,  int  tcg_idx_0,  int  tcg_idx_1,  int  tcg_idx_2,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = tcg_idx_0, I0 = tcg_idx_0 + 1;
    int i1 = tcg_idx_1, I1 = tcg_idx_1 + 1, org_i1 = tcg_idx_1;
    int i2 = tcg_idx_2, I2 = tcg_idx_2 + 1, org_i2 = tcg_idx_2;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_ofp_rate tcg valid") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_ofp_rate tcg valid\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_ofp_rate_db/tcg/valid.txt",
            "dnx_ofp_rate_db[%d]->","((dnx_ofp_rate_db_t*)sw_state_roots_array[%d][DNX_OFP_RATE_MODULE_ID])->","tcg[][][].valid: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->tcg
                , sizeof(*((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->tcg), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES)
        {
            LOG_CLI((BSL_META("dnx_ofp_rate_db[]->((dnx_ofp_rate_db_t*)sw_state_roots_array[][DNX_OFP_RATE_MODULE_ID])->tcg[][][].valid: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_PORT_SCHEDULERS;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->tcg[i0]
                    , sizeof(*((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->tcg[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_PORT_SCHEDULERS)
            {
                LOG_CLI((BSL_META("dnx_ofp_rate_db[]->((dnx_ofp_rate_db_t*)sw_state_roots_array[][DNX_OFP_RATE_MODULE_ID])->tcg[][][].valid: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_PORT_SCHEDULERS == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                if (i2 == -1) {
                    I2 = DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_TCG;
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->tcg[i0][i1]
                        , sizeof(*((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->tcg[i0][i1]), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_TCG)
                {
                    LOG_CLI((BSL_META("dnx_ofp_rate_db[]->((dnx_ofp_rate_db_t*)sw_state_roots_array[][DNX_OFP_RATE_MODULE_ID])->tcg[][][].valid: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_TCG == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "dnx_ofp_rate_db[%d]->","((dnx_ofp_rate_db_t*)sw_state_roots_array[%d][DNX_OFP_RATE_MODULE_ID])->","tcg[%s%d][%s%d][%s%d].valid: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_uint32(
                        unit,
                        &((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->tcg[i0][i1][i2].valid);

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




int
dnx_ofp_rate_db_otm_cal_dump(int  unit,  int  otm_cal_idx_0,  int  otm_cal_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db_otm_cal_rate_dump(unit, otm_cal_idx_0,otm_cal_idx_1, filters));
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db_otm_cal_modified_dump(unit, otm_cal_idx_0,otm_cal_idx_1, filters));
    SHR_IF_ERR_EXIT(dnx_ofp_rate_db_otm_cal_nof_calcal_instances_dump(unit, otm_cal_idx_0,otm_cal_idx_1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ofp_rate_db_otm_cal_rate_dump(int  unit,  int  otm_cal_idx_0,  int  otm_cal_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = otm_cal_idx_0, I0 = otm_cal_idx_0 + 1;
    int i1 = otm_cal_idx_1, I1 = otm_cal_idx_1 + 1, org_i1 = otm_cal_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_ofp_rate otm_cal rate") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_ofp_rate otm_cal rate\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_ofp_rate_db/otm_cal/rate.txt",
            "dnx_ofp_rate_db[%d]->","((dnx_ofp_rate_db_t*)sw_state_roots_array[%d][DNX_OFP_RATE_MODULE_ID])->","otm_cal[][].rate: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm_cal
                , sizeof(*((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm_cal), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES)
        {
            LOG_CLI((BSL_META("dnx_ofp_rate_db[]->((dnx_ofp_rate_db_t*)sw_state_roots_array[][DNX_OFP_RATE_MODULE_ID])->otm_cal[][].rate: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_CALENDARS;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm_cal[i0]
                    , sizeof(*((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm_cal[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_CALENDARS)
            {
                LOG_CLI((BSL_META("dnx_ofp_rate_db[]->((dnx_ofp_rate_db_t*)sw_state_roots_array[][DNX_OFP_RATE_MODULE_ID])->otm_cal[][].rate: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_CALENDARS == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_ofp_rate_db[%d]->","((dnx_ofp_rate_db_t*)sw_state_roots_array[%d][DNX_OFP_RATE_MODULE_ID])->","otm_cal[%s%d][%s%d].rate: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm_cal[i0][i1].rate);

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
dnx_ofp_rate_db_otm_cal_modified_dump(int  unit,  int  otm_cal_idx_0,  int  otm_cal_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = otm_cal_idx_0, I0 = otm_cal_idx_0 + 1;
    int i1 = otm_cal_idx_1, I1 = otm_cal_idx_1 + 1, org_i1 = otm_cal_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_ofp_rate otm_cal modified") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_ofp_rate otm_cal modified\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_ofp_rate_db/otm_cal/modified.txt",
            "dnx_ofp_rate_db[%d]->","((dnx_ofp_rate_db_t*)sw_state_roots_array[%d][DNX_OFP_RATE_MODULE_ID])->","otm_cal[][].modified: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm_cal
                , sizeof(*((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm_cal), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES)
        {
            LOG_CLI((BSL_META("dnx_ofp_rate_db[]->((dnx_ofp_rate_db_t*)sw_state_roots_array[][DNX_OFP_RATE_MODULE_ID])->otm_cal[][].modified: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_CALENDARS;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm_cal[i0]
                    , sizeof(*((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm_cal[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_CALENDARS)
            {
                LOG_CLI((BSL_META("dnx_ofp_rate_db[]->((dnx_ofp_rate_db_t*)sw_state_roots_array[][DNX_OFP_RATE_MODULE_ID])->otm_cal[][].modified: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_CALENDARS == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_ofp_rate_db[%d]->","((dnx_ofp_rate_db_t*)sw_state_roots_array[%d][DNX_OFP_RATE_MODULE_ID])->","otm_cal[%s%d][%s%d].modified: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm_cal[i0][i1].modified);

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
dnx_ofp_rate_db_otm_cal_nof_calcal_instances_dump(int  unit,  int  otm_cal_idx_0,  int  otm_cal_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = otm_cal_idx_0, I0 = otm_cal_idx_0 + 1;
    int i1 = otm_cal_idx_1, I1 = otm_cal_idx_1 + 1, org_i1 = otm_cal_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_ofp_rate otm_cal nof_calcal_instances") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_ofp_rate otm_cal nof_calcal_instances\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_ofp_rate_db/otm_cal/nof_calcal_instances.txt",
            "dnx_ofp_rate_db[%d]->","((dnx_ofp_rate_db_t*)sw_state_roots_array[%d][DNX_OFP_RATE_MODULE_ID])->","otm_cal[][].nof_calcal_instances: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm_cal
                , sizeof(*((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm_cal), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES)
        {
            LOG_CLI((BSL_META("dnx_ofp_rate_db[]->((dnx_ofp_rate_db_t*)sw_state_roots_array[][DNX_OFP_RATE_MODULE_ID])->otm_cal[][].nof_calcal_instances: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_CALENDARS;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm_cal[i0]
                    , sizeof(*((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm_cal[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_CALENDARS)
            {
                LOG_CLI((BSL_META("dnx_ofp_rate_db[]->((dnx_ofp_rate_db_t*)sw_state_roots_array[][DNX_OFP_RATE_MODULE_ID])->otm_cal[][].nof_calcal_instances: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_CALENDARS == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_ofp_rate_db[%d]->","((dnx_ofp_rate_db_t*)sw_state_roots_array[%d][DNX_OFP_RATE_MODULE_ID])->","otm_cal[%s%d][%s%d].nof_calcal_instances: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm_cal[i0][i1].nof_calcal_instances);

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
dnx_ofp_rate_db_calcal_is_modified_dump(int  unit,  int  calcal_is_modified_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = calcal_is_modified_idx_0, I0 = calcal_is_modified_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_ofp_rate calcal_is_modified") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_ofp_rate calcal_is_modified\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_ofp_rate_db/calcal_is_modified.txt",
            "dnx_ofp_rate_db[%d]->","((dnx_ofp_rate_db_t*)sw_state_roots_array[%d][DNX_OFP_RATE_MODULE_ID])->","calcal_is_modified[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->calcal_is_modified
                , sizeof(*((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->calcal_is_modified), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES)
        {
            LOG_CLI((BSL_META("dnx_ofp_rate_db[]->((dnx_ofp_rate_db_t*)sw_state_roots_array[][DNX_OFP_RATE_MODULE_ID])->calcal_is_modified[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_ofp_rate_db[%d]->","((dnx_ofp_rate_db_t*)sw_state_roots_array[%d][DNX_OFP_RATE_MODULE_ID])->","calcal_is_modified[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->calcal_is_modified[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ofp_rate_db_calcal_len_dump(int  unit,  int  calcal_len_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = calcal_len_idx_0, I0 = calcal_len_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_ofp_rate calcal_len") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_ofp_rate calcal_len\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_ofp_rate_db/calcal_len.txt",
            "dnx_ofp_rate_db[%d]->","((dnx_ofp_rate_db_t*)sw_state_roots_array[%d][DNX_OFP_RATE_MODULE_ID])->","calcal_len[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->calcal_len
                , sizeof(*((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->calcal_len), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES)
        {
            LOG_CLI((BSL_META("dnx_ofp_rate_db[]->((dnx_ofp_rate_db_t*)sw_state_roots_array[][DNX_OFP_RATE_MODULE_ID])->calcal_len[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_ofp_rate_db[%d]->","((dnx_ofp_rate_db_t*)sw_state_roots_array[%d][DNX_OFP_RATE_MODULE_ID])->","calcal_len[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->calcal_len[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_ofp_rate_db_info[SOC_MAX_NUM_DEVICES][DNX_OFP_RATE_DB_INFO_NOF_ENTRIES];
const char* dnx_ofp_rate_db_layout_str[DNX_OFP_RATE_DB_INFO_NOF_ENTRIES] = {
    "DNX_OFP_RATE_DB~",
    "DNX_OFP_RATE_DB~OTM~",
    "DNX_OFP_RATE_DB~OTM~CAL_INSTANCE~",
    "DNX_OFP_RATE_DB~OTM~SHAPING~",
    "DNX_OFP_RATE_DB~OTM~SHAPING~RATE~",
    "DNX_OFP_RATE_DB~OTM~SHAPING~BURST~",
    "DNX_OFP_RATE_DB~OTM~SHAPING~VALID~",
    "DNX_OFP_RATE_DB~QPAIR~",
    "DNX_OFP_RATE_DB~QPAIR~RATE~",
    "DNX_OFP_RATE_DB~QPAIR~VALID~",
    "DNX_OFP_RATE_DB~TCG~",
    "DNX_OFP_RATE_DB~TCG~RATE~",
    "DNX_OFP_RATE_DB~TCG~VALID~",
    "DNX_OFP_RATE_DB~OTM_CAL~",
    "DNX_OFP_RATE_DB~OTM_CAL~RATE~",
    "DNX_OFP_RATE_DB~OTM_CAL~MODIFIED~",
    "DNX_OFP_RATE_DB~OTM_CAL~NOF_CALCAL_INSTANCES~",
    "DNX_OFP_RATE_DB~CALCAL_IS_MODIFIED~",
    "DNX_OFP_RATE_DB~CALCAL_LEN~",
};
#endif 
#undef BSL_LOG_MODULE
