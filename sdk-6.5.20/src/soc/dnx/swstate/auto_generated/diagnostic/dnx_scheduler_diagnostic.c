
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_scheduler_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_scheduler_db_t * dnx_scheduler_db_data[SOC_MAX_NUM_DEVICES];



int
dnx_scheduler_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_scheduler_db_port_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnx_scheduler_db_hr_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(dnx_scheduler_db_interface_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnx_scheduler_db_fmq_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnx_scheduler_db_general_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_port_dump(int  unit,  int  port_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_scheduler_db_port_rate_dump(unit, port_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_scheduler_db_port_valid_dump(unit, port_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_port_rate_dump(int  unit,  int  port_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = port_idx_0, I0 = port_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_scheduler port rate") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_scheduler port rate\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_scheduler_db/port/rate.txt",
            "dnx_scheduler_db[%d]->","((dnx_scheduler_db_t*)sw_state_roots_array[%d][DNX_SCHEDULER_MODULE_ID])->","port[].rate: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SCHEDULER_MODULE_ID,
                ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port
                , sizeof(*((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SCHEDULER_MODULE_ID, ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port))
        {
            LOG_CLI((BSL_META("dnx_scheduler_db[]->((dnx_scheduler_db_t*)sw_state_roots_array[][DNX_SCHEDULER_MODULE_ID])->port[].rate: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SCHEDULER_MODULE_ID, ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_scheduler_db[%d]->","((dnx_scheduler_db_t*)sw_state_roots_array[%d][DNX_SCHEDULER_MODULE_ID])->","port[%s%d].rate: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port[i0].rate);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_port_valid_dump(int  unit,  int  port_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = port_idx_0, I0 = port_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_scheduler port valid") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_scheduler port valid\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_scheduler_db/port/valid.txt",
            "dnx_scheduler_db[%d]->","((dnx_scheduler_db_t*)sw_state_roots_array[%d][DNX_SCHEDULER_MODULE_ID])->","port[].valid: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SCHEDULER_MODULE_ID,
                ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port
                , sizeof(*((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SCHEDULER_MODULE_ID, ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port))
        {
            LOG_CLI((BSL_META("dnx_scheduler_db[]->((dnx_scheduler_db_t*)sw_state_roots_array[][DNX_SCHEDULER_MODULE_ID])->port[].valid: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SCHEDULER_MODULE_ID, ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_scheduler_db[%d]->","((dnx_scheduler_db_t*)sw_state_roots_array[%d][DNX_SCHEDULER_MODULE_ID])->","port[%s%d].valid: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port[i0].valid);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_hr_dump(int  unit,  int  hr_idx_0,  int  hr_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_scheduler_db_hr_is_colored_dump(unit, hr_idx_0,hr_idx_1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_hr_is_colored_dump(int  unit,  int  hr_idx_0,  int  hr_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = hr_idx_0, I0 = hr_idx_0 + 1;
    int i1 = hr_idx_1, I1 = hr_idx_1 + 1, org_i1 = hr_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_scheduler hr is_colored") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_scheduler hr is_colored\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_scheduler_db/hr/is_colored.txt",
            "dnx_scheduler_db[%d]->","((dnx_scheduler_db_t*)sw_state_roots_array[%d][DNX_SCHEDULER_MODULE_ID])->","hr[][].is_colored: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SCHEDULER_MODULE_ID,
                ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr
                , sizeof(*((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SCHEDULER_MODULE_ID, ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr))
        {
            LOG_CLI((BSL_META("dnx_scheduler_db[]->((dnx_scheduler_db_t*)sw_state_roots_array[][DNX_SCHEDULER_MODULE_ID])->hr[][].is_colored: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SCHEDULER_MODULE_ID, ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_SCHEDULER_MODULE_ID,
                    ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr[i0]
                    , sizeof(*((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_SCHEDULER_MODULE_ID, ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr[i0]))
            {
                LOG_CLI((BSL_META("dnx_scheduler_db[]->((dnx_scheduler_db_t*)sw_state_roots_array[][DNX_SCHEDULER_MODULE_ID])->hr[][].is_colored: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_SCHEDULER_MODULE_ID, ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_scheduler_db[%d]->","((dnx_scheduler_db_t*)sw_state_roots_array[%d][DNX_SCHEDULER_MODULE_ID])->","hr[%s%d][%s%d].is_colored: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr[i0][i1].is_colored);

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
dnx_scheduler_db_interface_dump(int  unit,  int  interface_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_scheduler_db_interface_modified_dump(unit, interface_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_interface_modified_dump(int  unit,  int  interface_idx_0,  int  modified_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = interface_idx_0, I0 = interface_idx_0 + 1;
    int i1 = modified_idx_0, I1 = modified_idx_0 + 1, org_i1 = modified_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "SHR_BITDCL") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_scheduler interface modified") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_scheduler interface modified\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_scheduler_db/interface/modified.txt",
            "dnx_scheduler_db[%d]->","((dnx_scheduler_db_t*)sw_state_roots_array[%d][DNX_SCHEDULER_MODULE_ID])->","interface[].modified[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SCHEDULER_MODULE_ID,
                ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface
                , sizeof(*((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SCHEDULER_MODULE_ID, ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface))
        {
            LOG_CLI((BSL_META("dnx_scheduler_db[]->((dnx_scheduler_db_t*)sw_state_roots_array[][DNX_SCHEDULER_MODULE_ID])->interface[].modified[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SCHEDULER_MODULE_ID, ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[i0].modified);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_SCHEDULER_MODULE_ID,
                    ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[i0].modified);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[i0].modified
                    , sizeof(*((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[i0].modified), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_SCHEDULER_MODULE_ID, ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[i0].modified))
            {
                LOG_CLI((BSL_META("dnx_scheduler_db[]->((dnx_scheduler_db_t*)sw_state_roots_array[][DNX_SCHEDULER_MODULE_ID])->interface[].modified[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_SCHEDULER_MODULE_ID, ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[i0].modified) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_scheduler_db[%d]->","((dnx_scheduler_db_t*)sw_state_roots_array[%d][DNX_SCHEDULER_MODULE_ID])->","interface[%s%d].modified[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_shr_bitdcl(
                    unit,
                    &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[i0].modified[i1]);

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
dnx_scheduler_db_fmq_dump(int  unit,  int  fmq_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_scheduler_db_fmq_reserved_hr_flow_id_dump(unit, fmq_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dnx_scheduler_db_fmq_nof_reserved_hr_dump(unit, fmq_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_scheduler_db_fmq_fmq_flow_id_dump(unit, fmq_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_fmq_reserved_hr_flow_id_dump(int  unit,  int  fmq_idx_0,  int  reserved_hr_flow_id_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fmq_idx_0, I0 = fmq_idx_0 + 1;
    int i1 = reserved_hr_flow_id_idx_0, I1 = reserved_hr_flow_id_idx_0 + 1, org_i1 = reserved_hr_flow_id_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_scheduler fmq reserved_hr_flow_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_scheduler fmq reserved_hr_flow_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_scheduler_db/fmq/reserved_hr_flow_id.txt",
            "dnx_scheduler_db[%d]->","((dnx_scheduler_db_t*)sw_state_roots_array[%d][DNX_SCHEDULER_MODULE_ID])->","fmq[].reserved_hr_flow_id[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SCHEDULER_MODULE_ID,
                ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq
                , sizeof(*((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SCHEDULER_MODULE_ID, ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq))
        {
            LOG_CLI((BSL_META("dnx_scheduler_db[]->((dnx_scheduler_db_t*)sw_state_roots_array[][DNX_SCHEDULER_MODULE_ID])->fmq[].reserved_hr_flow_id[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SCHEDULER_MODULE_ID, ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[i0].reserved_hr_flow_id);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_SCHEDULER_MODULE_ID,
                    ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[i0].reserved_hr_flow_id);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[i0].reserved_hr_flow_id
                    , sizeof(*((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[i0].reserved_hr_flow_id), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_SCHEDULER_MODULE_ID, ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[i0].reserved_hr_flow_id))
            {
                LOG_CLI((BSL_META("dnx_scheduler_db[]->((dnx_scheduler_db_t*)sw_state_roots_array[][DNX_SCHEDULER_MODULE_ID])->fmq[].reserved_hr_flow_id[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_SCHEDULER_MODULE_ID, ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[i0].reserved_hr_flow_id) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_scheduler_db[%d]->","((dnx_scheduler_db_t*)sw_state_roots_array[%d][DNX_SCHEDULER_MODULE_ID])->","fmq[%s%d].reserved_hr_flow_id[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[i0].reserved_hr_flow_id[i1]);

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
dnx_scheduler_db_fmq_nof_reserved_hr_dump(int  unit,  int  fmq_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fmq_idx_0, I0 = fmq_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_scheduler fmq nof_reserved_hr") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_scheduler fmq nof_reserved_hr\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_scheduler_db/fmq/nof_reserved_hr.txt",
            "dnx_scheduler_db[%d]->","((dnx_scheduler_db_t*)sw_state_roots_array[%d][DNX_SCHEDULER_MODULE_ID])->","fmq[].nof_reserved_hr: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SCHEDULER_MODULE_ID,
                ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq
                , sizeof(*((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SCHEDULER_MODULE_ID, ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq))
        {
            LOG_CLI((BSL_META("dnx_scheduler_db[]->((dnx_scheduler_db_t*)sw_state_roots_array[][DNX_SCHEDULER_MODULE_ID])->fmq[].nof_reserved_hr: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SCHEDULER_MODULE_ID, ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_scheduler_db[%d]->","((dnx_scheduler_db_t*)sw_state_roots_array[%d][DNX_SCHEDULER_MODULE_ID])->","fmq[%s%d].nof_reserved_hr: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[i0].nof_reserved_hr);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_fmq_fmq_flow_id_dump(int  unit,  int  fmq_idx_0,  int  fmq_flow_id_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fmq_idx_0, I0 = fmq_idx_0 + 1;
    int i1 = fmq_flow_id_idx_0, I1 = fmq_flow_id_idx_0 + 1, org_i1 = fmq_flow_id_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_scheduler fmq fmq_flow_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_scheduler fmq fmq_flow_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_scheduler_db/fmq/fmq_flow_id.txt",
            "dnx_scheduler_db[%d]->","((dnx_scheduler_db_t*)sw_state_roots_array[%d][DNX_SCHEDULER_MODULE_ID])->","fmq[].fmq_flow_id[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SCHEDULER_MODULE_ID,
                ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq
                , sizeof(*((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SCHEDULER_MODULE_ID, ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq))
        {
            LOG_CLI((BSL_META("dnx_scheduler_db[]->((dnx_scheduler_db_t*)sw_state_roots_array[][DNX_SCHEDULER_MODULE_ID])->fmq[].fmq_flow_id[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SCHEDULER_MODULE_ID, ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[i0].fmq_flow_id);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_SCHEDULER_MODULE_ID,
                    ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[i0].fmq_flow_id);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[i0].fmq_flow_id
                    , sizeof(*((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[i0].fmq_flow_id), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_SCHEDULER_MODULE_ID, ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[i0].fmq_flow_id))
            {
                LOG_CLI((BSL_META("dnx_scheduler_db[]->((dnx_scheduler_db_t*)sw_state_roots_array[][DNX_SCHEDULER_MODULE_ID])->fmq[].fmq_flow_id[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_SCHEDULER_MODULE_ID, ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[i0].fmq_flow_id) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_scheduler_db[%d]->","((dnx_scheduler_db_t*)sw_state_roots_array[%d][DNX_SCHEDULER_MODULE_ID])->","fmq[%s%d].fmq_flow_id[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[i0].fmq_flow_id[i1]);

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
dnx_scheduler_db_general_dump(int  unit,  int  general_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_scheduler_db_general_reserved_erp_hr_flow_id_dump(unit, general_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dnx_scheduler_db_general_nof_reserved_erp_hr_dump(unit, general_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_general_reserved_erp_hr_flow_id_dump(int  unit,  int  general_idx_0,  int  reserved_erp_hr_flow_id_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = general_idx_0, I0 = general_idx_0 + 1;
    int i1 = reserved_erp_hr_flow_id_idx_0, I1 = reserved_erp_hr_flow_id_idx_0 + 1, org_i1 = reserved_erp_hr_flow_id_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_scheduler general reserved_erp_hr_flow_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_scheduler general reserved_erp_hr_flow_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_scheduler_db/general/reserved_erp_hr_flow_id.txt",
            "dnx_scheduler_db[%d]->","((dnx_scheduler_db_t*)sw_state_roots_array[%d][DNX_SCHEDULER_MODULE_ID])->","general[].reserved_erp_hr_flow_id[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SCHEDULER_MODULE_ID,
                ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general
                , sizeof(*((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SCHEDULER_MODULE_ID, ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general))
        {
            LOG_CLI((BSL_META("dnx_scheduler_db[]->((dnx_scheduler_db_t*)sw_state_roots_array[][DNX_SCHEDULER_MODULE_ID])->general[].reserved_erp_hr_flow_id[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SCHEDULER_MODULE_ID, ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = 8;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general[i0].reserved_erp_hr_flow_id
                    , sizeof(*((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general[i0].reserved_erp_hr_flow_id), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= 8)
            {
                LOG_CLI((BSL_META("dnx_scheduler_db[]->((dnx_scheduler_db_t*)sw_state_roots_array[][DNX_SCHEDULER_MODULE_ID])->general[].reserved_erp_hr_flow_id[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(8 == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_scheduler_db[%d]->","((dnx_scheduler_db_t*)sw_state_roots_array[%d][DNX_SCHEDULER_MODULE_ID])->","general[%s%d].reserved_erp_hr_flow_id[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general[i0].reserved_erp_hr_flow_id[i1]);

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
dnx_scheduler_db_general_nof_reserved_erp_hr_dump(int  unit,  int  general_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = general_idx_0, I0 = general_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_scheduler general nof_reserved_erp_hr") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_scheduler general nof_reserved_erp_hr\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_scheduler_db/general/nof_reserved_erp_hr.txt",
            "dnx_scheduler_db[%d]->","((dnx_scheduler_db_t*)sw_state_roots_array[%d][DNX_SCHEDULER_MODULE_ID])->","general[].nof_reserved_erp_hr: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SCHEDULER_MODULE_ID,
                ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general
                , sizeof(*((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SCHEDULER_MODULE_ID, ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general))
        {
            LOG_CLI((BSL_META("dnx_scheduler_db[]->((dnx_scheduler_db_t*)sw_state_roots_array[][DNX_SCHEDULER_MODULE_ID])->general[].nof_reserved_erp_hr: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SCHEDULER_MODULE_ID, ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_scheduler_db[%d]->","((dnx_scheduler_db_t*)sw_state_roots_array[%d][DNX_SCHEDULER_MODULE_ID])->","general[%s%d].nof_reserved_erp_hr: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general[i0].nof_reserved_erp_hr);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_scheduler_db_info[SOC_MAX_NUM_DEVICES][DNX_SCHEDULER_DB_INFO_NOF_ENTRIES];
const char* dnx_scheduler_db_layout_str[DNX_SCHEDULER_DB_INFO_NOF_ENTRIES] = {
    "DNX_SCHEDULER_DB~",
    "DNX_SCHEDULER_DB~PORT~",
    "DNX_SCHEDULER_DB~PORT~RATE~",
    "DNX_SCHEDULER_DB~PORT~VALID~",
    "DNX_SCHEDULER_DB~HR~",
    "DNX_SCHEDULER_DB~HR~IS_COLORED~",
    "DNX_SCHEDULER_DB~INTERFACE~",
    "DNX_SCHEDULER_DB~INTERFACE~MODIFIED~",
    "DNX_SCHEDULER_DB~FMQ~",
    "DNX_SCHEDULER_DB~FMQ~RESERVED_HR_FLOW_ID~",
    "DNX_SCHEDULER_DB~FMQ~NOF_RESERVED_HR~",
    "DNX_SCHEDULER_DB~FMQ~FMQ_FLOW_ID~",
    "DNX_SCHEDULER_DB~GENERAL~",
    "DNX_SCHEDULER_DB~GENERAL~RESERVED_ERP_HR_FLOW_ID~",
    "DNX_SCHEDULER_DB~GENERAL~NOF_RESERVED_ERP_HR~",
};
#endif 
#undef BSL_LOG_MODULE
