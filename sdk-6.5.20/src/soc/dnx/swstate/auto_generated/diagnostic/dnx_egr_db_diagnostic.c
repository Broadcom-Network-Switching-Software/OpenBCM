
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_egr_db_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_egr_db_t * dnx_egr_db_data[SOC_MAX_NUM_DEVICES];



int
dnx_egr_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_egr_db_ps_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_egr_db_interface_occ_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(dnx_egr_db_total_egr_if_credits_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_egr_db_ps_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_egr_db_ps_allocation_bmap_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(dnx_egr_db_ps_prio_mode_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(dnx_egr_db_ps_if_idx_dump(unit, -1, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_egr_db_ps_allocation_bmap_dump(int  unit,  int  allocation_bmap_idx_0,  int  allocation_bmap_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = allocation_bmap_idx_0, I0 = allocation_bmap_idx_0 + 1;
    int i1 = allocation_bmap_idx_1, I1 = allocation_bmap_idx_1 + 1, org_i1 = allocation_bmap_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_egr_db ps allocation_bmap") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_egr_db ps allocation_bmap\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_egr_db/ps/allocation_bmap.txt",
            "dnx_egr_db[%d]->","((dnx_egr_db_t*)sw_state_roots_array[%d][DNX_EGR_DB_MODULE_ID])->","ps.allocation_bmap[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_egr_db_t*)sw_state_roots_array[unit][DNX_EGR_DB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_egr_db_t*)sw_state_roots_array[unit][DNX_EGR_DB_MODULE_ID])->ps.allocation_bmap
                , sizeof(*((dnx_egr_db_t*)sw_state_roots_array[unit][DNX_EGR_DB_MODULE_ID])->ps.allocation_bmap), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES)
        {
            LOG_CLI((BSL_META("dnx_egr_db[]->((dnx_egr_db_t*)sw_state_roots_array[][DNX_EGR_DB_MODULE_ID])->ps.allocation_bmap[][]: ")));
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
                    ((dnx_egr_db_t*)sw_state_roots_array[unit][DNX_EGR_DB_MODULE_ID])->ps.allocation_bmap[i0]
                    , sizeof(*((dnx_egr_db_t*)sw_state_roots_array[unit][DNX_EGR_DB_MODULE_ID])->ps.allocation_bmap[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_PORT_SCHEDULERS)
            {
                LOG_CLI((BSL_META("dnx_egr_db[]->((dnx_egr_db_t*)sw_state_roots_array[][DNX_EGR_DB_MODULE_ID])->ps.allocation_bmap[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_PORT_SCHEDULERS == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_egr_db[%d]->","((dnx_egr_db_t*)sw_state_roots_array[%d][DNX_EGR_DB_MODULE_ID])->","ps.allocation_bmap[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((dnx_egr_db_t*)sw_state_roots_array[unit][DNX_EGR_DB_MODULE_ID])->ps.allocation_bmap[i0][i1]);

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
dnx_egr_db_ps_prio_mode_dump(int  unit,  int  prio_mode_idx_0,  int  prio_mode_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = prio_mode_idx_0, I0 = prio_mode_idx_0 + 1;
    int i1 = prio_mode_idx_1, I1 = prio_mode_idx_1 + 1, org_i1 = prio_mode_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_egr_db ps prio_mode") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_egr_db ps prio_mode\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_egr_db/ps/prio_mode.txt",
            "dnx_egr_db[%d]->","((dnx_egr_db_t*)sw_state_roots_array[%d][DNX_EGR_DB_MODULE_ID])->","ps.prio_mode[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_egr_db_t*)sw_state_roots_array[unit][DNX_EGR_DB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_egr_db_t*)sw_state_roots_array[unit][DNX_EGR_DB_MODULE_ID])->ps.prio_mode
                , sizeof(*((dnx_egr_db_t*)sw_state_roots_array[unit][DNX_EGR_DB_MODULE_ID])->ps.prio_mode), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES)
        {
            LOG_CLI((BSL_META("dnx_egr_db[]->((dnx_egr_db_t*)sw_state_roots_array[][DNX_EGR_DB_MODULE_ID])->ps.prio_mode[][]: ")));
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
                    ((dnx_egr_db_t*)sw_state_roots_array[unit][DNX_EGR_DB_MODULE_ID])->ps.prio_mode[i0]
                    , sizeof(*((dnx_egr_db_t*)sw_state_roots_array[unit][DNX_EGR_DB_MODULE_ID])->ps.prio_mode[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_PORT_SCHEDULERS)
            {
                LOG_CLI((BSL_META("dnx_egr_db[]->((dnx_egr_db_t*)sw_state_roots_array[][DNX_EGR_DB_MODULE_ID])->ps.prio_mode[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_PORT_SCHEDULERS == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_egr_db[%d]->","((dnx_egr_db_t*)sw_state_roots_array[%d][DNX_EGR_DB_MODULE_ID])->","ps.prio_mode[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((dnx_egr_db_t*)sw_state_roots_array[unit][DNX_EGR_DB_MODULE_ID])->ps.prio_mode[i0][i1]);

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
dnx_egr_db_ps_if_idx_dump(int  unit,  int  if_idx_idx_0,  int  if_idx_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = if_idx_idx_0, I0 = if_idx_idx_0 + 1;
    int i1 = if_idx_idx_1, I1 = if_idx_idx_1 + 1, org_i1 = if_idx_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_egr_db ps if_idx") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_egr_db ps if_idx\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_egr_db/ps/if_idx.txt",
            "dnx_egr_db[%d]->","((dnx_egr_db_t*)sw_state_roots_array[%d][DNX_EGR_DB_MODULE_ID])->","ps.if_idx[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_egr_db_t*)sw_state_roots_array[unit][DNX_EGR_DB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_egr_db_t*)sw_state_roots_array[unit][DNX_EGR_DB_MODULE_ID])->ps.if_idx
                , sizeof(*((dnx_egr_db_t*)sw_state_roots_array[unit][DNX_EGR_DB_MODULE_ID])->ps.if_idx), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES)
        {
            LOG_CLI((BSL_META("dnx_egr_db[]->((dnx_egr_db_t*)sw_state_roots_array[][DNX_EGR_DB_MODULE_ID])->ps.if_idx[][]: ")));
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
                    ((dnx_egr_db_t*)sw_state_roots_array[unit][DNX_EGR_DB_MODULE_ID])->ps.if_idx[i0]
                    , sizeof(*((dnx_egr_db_t*)sw_state_roots_array[unit][DNX_EGR_DB_MODULE_ID])->ps.if_idx[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_PORT_SCHEDULERS)
            {
                LOG_CLI((BSL_META("dnx_egr_db[]->((dnx_egr_db_t*)sw_state_roots_array[][DNX_EGR_DB_MODULE_ID])->ps.if_idx[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_PORT_SCHEDULERS == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_egr_db[%d]->","((dnx_egr_db_t*)sw_state_roots_array[%d][DNX_EGR_DB_MODULE_ID])->","ps.if_idx[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((dnx_egr_db_t*)sw_state_roots_array[unit][DNX_EGR_DB_MODULE_ID])->ps.if_idx[i0][i1]);

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
dnx_egr_db_interface_occ_dump(int  unit,  int  interface_occ_idx_0,  int  interface_occ_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = interface_occ_idx_0, I0 = interface_occ_idx_0 + 1;
    int i1 = interface_occ_idx_1, I1 = interface_occ_idx_1 + 1, org_i1 = interface_occ_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_egr_db interface_occ") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_egr_db interface_occ\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_egr_db/interface_occ.txt",
            "dnx_egr_db[%d]->","((dnx_egr_db_t*)sw_state_roots_array[%d][DNX_EGR_DB_MODULE_ID])->","interface_occ[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_egr_db_t*)sw_state_roots_array[unit][DNX_EGR_DB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_egr_db_t*)sw_state_roots_array[unit][DNX_EGR_DB_MODULE_ID])->interface_occ
                , sizeof(*((dnx_egr_db_t*)sw_state_roots_array[unit][DNX_EGR_DB_MODULE_ID])->interface_occ), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES)
        {
            LOG_CLI((BSL_META("dnx_egr_db[]->((dnx_egr_db_t*)sw_state_roots_array[][DNX_EGR_DB_MODULE_ID])->interface_occ[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_EGR_INTERFACES;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_egr_db_t*)sw_state_roots_array[unit][DNX_EGR_DB_MODULE_ID])->interface_occ[i0]
                    , sizeof(*((dnx_egr_db_t*)sw_state_roots_array[unit][DNX_EGR_DB_MODULE_ID])->interface_occ[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_EGR_INTERFACES)
            {
                LOG_CLI((BSL_META("dnx_egr_db[]->((dnx_egr_db_t*)sw_state_roots_array[][DNX_EGR_DB_MODULE_ID])->interface_occ[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_EGR_INTERFACES == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_egr_db[%d]->","((dnx_egr_db_t*)sw_state_roots_array[%d][DNX_EGR_DB_MODULE_ID])->","interface_occ[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((dnx_egr_db_t*)sw_state_roots_array[unit][DNX_EGR_DB_MODULE_ID])->interface_occ[i0][i1]);

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
dnx_egr_db_total_egr_if_credits_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_egr_db total_egr_if_credits") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_egr_db total_egr_if_credits\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_egr_db/total_egr_if_credits.txt",
            "dnx_egr_db[%d]->","((dnx_egr_db_t*)sw_state_roots_array[%d][DNX_EGR_DB_MODULE_ID])->","total_egr_if_credits: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_egr_db_t*)sw_state_roots_array[unit][DNX_EGR_DB_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_egr_db[%d]->","((dnx_egr_db_t*)sw_state_roots_array[%d][DNX_EGR_DB_MODULE_ID])->","total_egr_if_credits: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_egr_db_t*)sw_state_roots_array[unit][DNX_EGR_DB_MODULE_ID])->total_egr_if_credits);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_egr_db_info[SOC_MAX_NUM_DEVICES][DNX_EGR_DB_INFO_NOF_ENTRIES];
const char* dnx_egr_db_layout_str[DNX_EGR_DB_INFO_NOF_ENTRIES] = {
    "DNX_EGR_DB~",
    "DNX_EGR_DB~PS~",
    "DNX_EGR_DB~PS~ALLOCATION_BMAP~",
    "DNX_EGR_DB~PS~PRIO_MODE~",
    "DNX_EGR_DB~PS~IF_IDX~",
    "DNX_EGR_DB~INTERFACE_OCC~",
    "DNX_EGR_DB~TOTAL_EGR_IF_CREDITS~",
};
#endif 
#undef BSL_LOG_MODULE
