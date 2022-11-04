
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_port_nif_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_port_nif_layout.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_port_nif_db_t * dnx_port_nif_db_data[SOC_MAX_NUM_DEVICES];



int
dnx_port_nif_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_port_nif_db_arb_link_list_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_port_nif_db_oft_link_list_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_port_nif_db_ofr_link_list_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_port_nif_db_active_calendars_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnx_port_nif_db_g_hao_active_client_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_port_nif_db_priority_group_num_of_entries_dump(unit, -1, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_arb_link_list_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_port_nif_db_arb_link_list_list_info_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_port_nif_db_arb_link_list_linking_info_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnx_port_nif_db_arb_link_list_allocated_list_info_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_arb_link_list_list_info_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_port_nif_db_arb_link_list_list_info_head_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_port_nif_db_arb_link_list_list_info_tail_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_port_nif_db_arb_link_list_list_info_size_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_arb_link_list_list_info_head_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_nif arb_link_list list_info head") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_nif arb_link_list list_info head\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_port_nif_db/arb_link_list/list_info/head.txt",
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","arb_link_list.list_info.head: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","arb_link_list.list_info.head: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.list_info.head);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_arb_link_list_list_info_tail_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_nif arb_link_list list_info tail") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_nif arb_link_list list_info tail\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_port_nif_db/arb_link_list/list_info/tail.txt",
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","arb_link_list.list_info.tail: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","arb_link_list.list_info.tail: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.list_info.tail);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_arb_link_list_list_info_size_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_nif arb_link_list list_info size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_nif arb_link_list list_info size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_port_nif_db/arb_link_list/list_info/size.txt",
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","arb_link_list.list_info.size: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","arb_link_list.list_info.size: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.list_info.size);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_arb_link_list_linking_info_dump(int  unit,  int  linking_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_port_nif_db_arb_link_list_linking_info_next_section_dump(unit, linking_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_arb_link_list_linking_info_next_section_dump(int  unit,  int  linking_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = linking_info_idx_0, I0 = linking_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_nif arb_link_list linking_info next_section") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_nif arb_link_list linking_info next_section\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_port_nif_db/arb_link_list/linking_info/next_section.txt",
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","arb_link_list.linking_info[].next_section: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.linking_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LINKING_INFO__NEXT_SECTION,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.linking_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.linking_info
                , sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.linking_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LINKING_INFO__NEXT_SECTION, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.linking_info))
        {
            LOG_CLI((BSL_META("dnx_port_nif_db[]->((dnx_port_nif_db_t*)sw_state_roots_array[][DNX_PORT_NIF_MODULE_ID])->arb_link_list.linking_info[].next_section: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__LINKING_INFO__NEXT_SECTION, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.linking_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","arb_link_list.linking_info[%s%d].next_section: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.linking_info[i0].next_section);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_arb_link_list_allocated_list_info_dump(int  unit,  int  allocated_list_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_port_nif_db_arb_link_list_allocated_list_info_head_dump(unit, allocated_list_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_port_nif_db_arb_link_list_allocated_list_info_tail_dump(unit, allocated_list_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_port_nif_db_arb_link_list_allocated_list_info_size_dump(unit, allocated_list_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_arb_link_list_allocated_list_info_head_dump(int  unit,  int  allocated_list_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = allocated_list_info_idx_0, I0 = allocated_list_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_nif arb_link_list allocated_list_info head") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_nif arb_link_list allocated_list_info head\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_port_nif_db/arb_link_list/allocated_list_info/head.txt",
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","arb_link_list.allocated_list_info[].head: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info
                , sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__HEAD, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info))
        {
            LOG_CLI((BSL_META("dnx_port_nif_db[]->((dnx_port_nif_db_t*)sw_state_roots_array[][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info[].head: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__HEAD, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","arb_link_list.allocated_list_info[%s%d].head: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info[i0].head);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_arb_link_list_allocated_list_info_tail_dump(int  unit,  int  allocated_list_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = allocated_list_info_idx_0, I0 = allocated_list_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_nif arb_link_list allocated_list_info tail") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_nif arb_link_list allocated_list_info tail\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_port_nif_db/arb_link_list/allocated_list_info/tail.txt",
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","arb_link_list.allocated_list_info[].tail: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info
                , sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__TAIL, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info))
        {
            LOG_CLI((BSL_META("dnx_port_nif_db[]->((dnx_port_nif_db_t*)sw_state_roots_array[][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info[].tail: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__TAIL, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","arb_link_list.allocated_list_info[%s%d].tail: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info[i0].tail);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_arb_link_list_allocated_list_info_size_dump(int  unit,  int  allocated_list_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = allocated_list_info_idx_0, I0 = allocated_list_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_nif arb_link_list allocated_list_info size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_nif arb_link_list allocated_list_info size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_port_nif_db/arb_link_list/allocated_list_info/size.txt",
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","arb_link_list.allocated_list_info[].size: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info
                , sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__SIZE, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info))
        {
            LOG_CLI((BSL_META("dnx_port_nif_db[]->((dnx_port_nif_db_t*)sw_state_roots_array[][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info[].size: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__ARB_LINK_LIST__ALLOCATED_LIST_INFO__SIZE, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","arb_link_list.allocated_list_info[%s%d].size: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->arb_link_list.allocated_list_info[i0].size);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_oft_link_list_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_port_nif_db_oft_link_list_list_info_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_port_nif_db_oft_link_list_linking_info_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnx_port_nif_db_oft_link_list_allocated_list_info_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_oft_link_list_list_info_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_port_nif_db_oft_link_list_list_info_head_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_port_nif_db_oft_link_list_list_info_tail_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_port_nif_db_oft_link_list_list_info_size_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_oft_link_list_list_info_head_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_nif oft_link_list list_info head") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_nif oft_link_list list_info head\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_port_nif_db/oft_link_list/list_info/head.txt",
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","oft_link_list.list_info.head: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","oft_link_list.list_info.head: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.list_info.head);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_oft_link_list_list_info_tail_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_nif oft_link_list list_info tail") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_nif oft_link_list list_info tail\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_port_nif_db/oft_link_list/list_info/tail.txt",
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","oft_link_list.list_info.tail: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","oft_link_list.list_info.tail: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.list_info.tail);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_oft_link_list_list_info_size_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_nif oft_link_list list_info size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_nif oft_link_list list_info size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_port_nif_db/oft_link_list/list_info/size.txt",
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","oft_link_list.list_info.size: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","oft_link_list.list_info.size: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.list_info.size);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_oft_link_list_linking_info_dump(int  unit,  int  linking_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_port_nif_db_oft_link_list_linking_info_next_section_dump(unit, linking_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_oft_link_list_linking_info_next_section_dump(int  unit,  int  linking_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = linking_info_idx_0, I0 = linking_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_nif oft_link_list linking_info next_section") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_nif oft_link_list linking_info next_section\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_port_nif_db/oft_link_list/linking_info/next_section.txt",
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","oft_link_list.linking_info[].next_section: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.linking_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LINKING_INFO__NEXT_SECTION,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.linking_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.linking_info
                , sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.linking_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LINKING_INFO__NEXT_SECTION, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.linking_info))
        {
            LOG_CLI((BSL_META("dnx_port_nif_db[]->((dnx_port_nif_db_t*)sw_state_roots_array[][DNX_PORT_NIF_MODULE_ID])->oft_link_list.linking_info[].next_section: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__LINKING_INFO__NEXT_SECTION, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.linking_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","oft_link_list.linking_info[%s%d].next_section: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.linking_info[i0].next_section);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_oft_link_list_allocated_list_info_dump(int  unit,  int  allocated_list_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_port_nif_db_oft_link_list_allocated_list_info_head_dump(unit, allocated_list_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_port_nif_db_oft_link_list_allocated_list_info_tail_dump(unit, allocated_list_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_port_nif_db_oft_link_list_allocated_list_info_size_dump(unit, allocated_list_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_oft_link_list_allocated_list_info_head_dump(int  unit,  int  allocated_list_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = allocated_list_info_idx_0, I0 = allocated_list_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_nif oft_link_list allocated_list_info head") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_nif oft_link_list allocated_list_info head\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_port_nif_db/oft_link_list/allocated_list_info/head.txt",
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","oft_link_list.allocated_list_info[].head: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info
                , sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__HEAD, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info))
        {
            LOG_CLI((BSL_META("dnx_port_nif_db[]->((dnx_port_nif_db_t*)sw_state_roots_array[][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info[].head: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__HEAD, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","oft_link_list.allocated_list_info[%s%d].head: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info[i0].head);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_oft_link_list_allocated_list_info_tail_dump(int  unit,  int  allocated_list_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = allocated_list_info_idx_0, I0 = allocated_list_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_nif oft_link_list allocated_list_info tail") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_nif oft_link_list allocated_list_info tail\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_port_nif_db/oft_link_list/allocated_list_info/tail.txt",
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","oft_link_list.allocated_list_info[].tail: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info
                , sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__TAIL, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info))
        {
            LOG_CLI((BSL_META("dnx_port_nif_db[]->((dnx_port_nif_db_t*)sw_state_roots_array[][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info[].tail: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__TAIL, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","oft_link_list.allocated_list_info[%s%d].tail: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info[i0].tail);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_oft_link_list_allocated_list_info_size_dump(int  unit,  int  allocated_list_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = allocated_list_info_idx_0, I0 = allocated_list_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_nif oft_link_list allocated_list_info size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_nif oft_link_list allocated_list_info size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_port_nif_db/oft_link_list/allocated_list_info/size.txt",
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","oft_link_list.allocated_list_info[].size: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info
                , sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__SIZE, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info))
        {
            LOG_CLI((BSL_META("dnx_port_nif_db[]->((dnx_port_nif_db_t*)sw_state_roots_array[][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info[].size: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFT_LINK_LIST__ALLOCATED_LIST_INFO__SIZE, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","oft_link_list.allocated_list_info[%s%d].size: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->oft_link_list.allocated_list_info[i0].size);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_ofr_link_list_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_port_nif_db_ofr_link_list_list_info_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnx_port_nif_db_ofr_link_list_linking_info_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(dnx_port_nif_db_ofr_link_list_allocated_list_info_dump(unit, -1, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_ofr_link_list_list_info_dump(int  unit,  int  list_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_port_nif_db_ofr_link_list_list_info_head_dump(unit, list_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_port_nif_db_ofr_link_list_list_info_tail_dump(unit, list_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_port_nif_db_ofr_link_list_list_info_size_dump(unit, list_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_ofr_link_list_list_info_head_dump(int  unit,  int  list_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = list_info_idx_0, I0 = list_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_nif ofr_link_list list_info head") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_nif ofr_link_list list_info head\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_port_nif_db/ofr_link_list/list_info/head.txt",
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","ofr_link_list.list_info[].head: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__HEAD,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info
                , sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__HEAD, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info))
        {
            LOG_CLI((BSL_META("dnx_port_nif_db[]->((dnx_port_nif_db_t*)sw_state_roots_array[][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info[].head: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__HEAD, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","ofr_link_list.list_info[%s%d].head: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info[i0].head);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_ofr_link_list_list_info_tail_dump(int  unit,  int  list_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = list_info_idx_0, I0 = list_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_nif ofr_link_list list_info tail") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_nif ofr_link_list list_info tail\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_port_nif_db/ofr_link_list/list_info/tail.txt",
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","ofr_link_list.list_info[].tail: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__TAIL,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info
                , sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__TAIL, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info))
        {
            LOG_CLI((BSL_META("dnx_port_nif_db[]->((dnx_port_nif_db_t*)sw_state_roots_array[][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info[].tail: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__TAIL, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","ofr_link_list.list_info[%s%d].tail: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info[i0].tail);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_ofr_link_list_list_info_size_dump(int  unit,  int  list_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = list_info_idx_0, I0 = list_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_nif ofr_link_list list_info size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_nif ofr_link_list list_info size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_port_nif_db/ofr_link_list/list_info/size.txt",
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","ofr_link_list.list_info[].size: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__SIZE,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info
                , sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__SIZE, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info))
        {
            LOG_CLI((BSL_META("dnx_port_nif_db[]->((dnx_port_nif_db_t*)sw_state_roots_array[][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info[].size: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LIST_INFO__SIZE, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","ofr_link_list.list_info[%s%d].size: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.list_info[i0].size);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_ofr_link_list_linking_info_dump(int  unit,  int  linking_info_idx_0,  int  linking_info_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_port_nif_db_ofr_link_list_linking_info_next_section_dump(unit, linking_info_idx_0,linking_info_idx_1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_ofr_link_list_linking_info_next_section_dump(int  unit,  int  linking_info_idx_0,  int  linking_info_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = linking_info_idx_0, I0 = linking_info_idx_0 + 1;
    int i1 = linking_info_idx_1, I1 = linking_info_idx_1 + 1, org_i1 = linking_info_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_nif ofr_link_list linking_info next_section") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_nif ofr_link_list linking_info next_section\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_port_nif_db/ofr_link_list/linking_info/next_section.txt",
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","ofr_link_list.linking_info[][].next_section: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO__NEXT_SECTION,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info
                , sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO__NEXT_SECTION, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info))
        {
            LOG_CLI((BSL_META("dnx_port_nif_db[]->((dnx_port_nif_db_t*)sw_state_roots_array[][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info[][].next_section: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO__NEXT_SECTION, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO__NEXT_SECTION,
                    ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info[i0]
                    , sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO__NEXT_SECTION, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info[i0]))
            {
                LOG_CLI((BSL_META("dnx_port_nif_db[]->((dnx_port_nif_db_t*)sw_state_roots_array[][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info[][].next_section: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__LINKING_INFO__NEXT_SECTION, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","ofr_link_list.linking_info[%s%d][%s%d].next_section: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.linking_info[i0][i1].next_section);

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
dnx_port_nif_db_ofr_link_list_allocated_list_info_dump(int  unit,  int  allocated_list_info_idx_0,  int  allocated_list_info_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_port_nif_db_ofr_link_list_allocated_list_info_head_dump(unit, allocated_list_info_idx_0,allocated_list_info_idx_1, filters));
    SHR_IF_ERR_EXIT(dnx_port_nif_db_ofr_link_list_allocated_list_info_tail_dump(unit, allocated_list_info_idx_0,allocated_list_info_idx_1, filters));
    SHR_IF_ERR_EXIT(dnx_port_nif_db_ofr_link_list_allocated_list_info_size_dump(unit, allocated_list_info_idx_0,allocated_list_info_idx_1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_ofr_link_list_allocated_list_info_head_dump(int  unit,  int  allocated_list_info_idx_0,  int  allocated_list_info_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = allocated_list_info_idx_0, I0 = allocated_list_info_idx_0 + 1;
    int i1 = allocated_list_info_idx_1, I1 = allocated_list_info_idx_1 + 1, org_i1 = allocated_list_info_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_nif ofr_link_list allocated_list_info head") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_nif ofr_link_list allocated_list_info head\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_port_nif_db/ofr_link_list/allocated_list_info/head.txt",
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","ofr_link_list.allocated_list_info[][].head: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info
                , sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__HEAD, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info))
        {
            LOG_CLI((BSL_META("dnx_port_nif_db[]->((dnx_port_nif_db_t*)sw_state_roots_array[][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[][].head: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__HEAD, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__HEAD,
                    ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[i0]
                    , sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__HEAD, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[i0]))
            {
                LOG_CLI((BSL_META("dnx_port_nif_db[]->((dnx_port_nif_db_t*)sw_state_roots_array[][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[][].head: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__HEAD, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","ofr_link_list.allocated_list_info[%s%d][%s%d].head: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[i0][i1].head);

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
dnx_port_nif_db_ofr_link_list_allocated_list_info_tail_dump(int  unit,  int  allocated_list_info_idx_0,  int  allocated_list_info_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = allocated_list_info_idx_0, I0 = allocated_list_info_idx_0 + 1;
    int i1 = allocated_list_info_idx_1, I1 = allocated_list_info_idx_1 + 1, org_i1 = allocated_list_info_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_nif ofr_link_list allocated_list_info tail") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_nif ofr_link_list allocated_list_info tail\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_port_nif_db/ofr_link_list/allocated_list_info/tail.txt",
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","ofr_link_list.allocated_list_info[][].tail: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info
                , sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__TAIL, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info))
        {
            LOG_CLI((BSL_META("dnx_port_nif_db[]->((dnx_port_nif_db_t*)sw_state_roots_array[][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[][].tail: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__TAIL, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__TAIL,
                    ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[i0]
                    , sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__TAIL, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[i0]))
            {
                LOG_CLI((BSL_META("dnx_port_nif_db[]->((dnx_port_nif_db_t*)sw_state_roots_array[][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[][].tail: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__TAIL, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","ofr_link_list.allocated_list_info[%s%d][%s%d].tail: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[i0][i1].tail);

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
dnx_port_nif_db_ofr_link_list_allocated_list_info_size_dump(int  unit,  int  allocated_list_info_idx_0,  int  allocated_list_info_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = allocated_list_info_idx_0, I0 = allocated_list_info_idx_0 + 1;
    int i1 = allocated_list_info_idx_1, I1 = allocated_list_info_idx_1 + 1, org_i1 = allocated_list_info_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_nif ofr_link_list allocated_list_info size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_nif ofr_link_list allocated_list_info size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_port_nif_db/ofr_link_list/allocated_list_info/size.txt",
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","ofr_link_list.allocated_list_info[][].size: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info
                , sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__SIZE, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info))
        {
            LOG_CLI((BSL_META("dnx_port_nif_db[]->((dnx_port_nif_db_t*)sw_state_roots_array[][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[][].size: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__SIZE, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__SIZE,
                    ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[i0]
                    , sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__SIZE, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[i0]))
            {
                LOG_CLI((BSL_META("dnx_port_nif_db[]->((dnx_port_nif_db_t*)sw_state_roots_array[][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[][].size: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__OFR_LINK_LIST__ALLOCATED_LIST_INFO__SIZE, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","ofr_link_list.allocated_list_info[%s%d][%s%d].size: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->ofr_link_list.allocated_list_info[i0][i1].size);

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
dnx_port_nif_db_active_calendars_dump(int  unit,  int  active_calendars_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_port_nif_db_active_calendars_client_last_objects_dump(unit, active_calendars_idx_0, -1, -1, filters));
    SHR_IF_ERR_EXIT(dnx_port_nif_db_active_calendars_nof_client_slots_dump(unit, active_calendars_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dnx_port_nif_db_active_calendars_last_client_dump(unit, active_calendars_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_active_calendars_client_last_objects_dump(int  unit,  int  active_calendars_idx_0,  int  client_last_objects_idx_0,  int  client_last_objects_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = active_calendars_idx_0, I0 = active_calendars_idx_0 + 1;
    int i1 = client_last_objects_idx_0, I1 = client_last_objects_idx_0 + 1, org_i1 = client_last_objects_idx_0;
    int i2 = client_last_objects_idx_1, I2 = client_last_objects_idx_1 + 1, org_i2 = client_last_objects_idx_1;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_nif active_calendars client_last_objects") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_nif active_calendars client_last_objects\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_port_nif_db/active_calendars/client_last_objects.txt",
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","active_calendars[].client_last_objects[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars
                , sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars))
        {
            LOG_CLI((BSL_META("dnx_port_nif_db[]->((dnx_port_nif_db_t*)sw_state_roots_array[][DNX_PORT_NIF_MODULE_ID])->active_calendars[].client_last_objects[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[i0].client_last_objects);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
                    ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[i0].client_last_objects);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[i0].client_last_objects
                    , sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[i0].client_last_objects), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[i0].client_last_objects))
            {
                LOG_CLI((BSL_META("dnx_port_nif_db[]->((dnx_port_nif_db_t*)sw_state_roots_array[][DNX_PORT_NIF_MODULE_ID])->active_calendars[].client_last_objects[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[i0].client_last_objects) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                
                
                    DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                        unit,
                        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[i0].client_last_objects[i1]);

                if (i2 == -1) {
                    I2 = dnx_sw_state_get_nof_elements(unit,
                        DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS,
                        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[i0].client_last_objects[i1]);
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[i0].client_last_objects[i1]
                        , sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[i0].client_last_objects[i1]), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[i0].client_last_objects[i1]))
                {
                    LOG_CLI((BSL_META("dnx_port_nif_db[]->((dnx_port_nif_db_t*)sw_state_roots_array[][DNX_PORT_NIF_MODULE_ID])->active_calendars[].client_last_objects[][]: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__CLIENT_LAST_OBJECTS, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[i0].client_last_objects[i1]) == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","active_calendars[%s%d].client_last_objects[%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_uint32(
                        unit,
                        &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[i0].client_last_objects[i1][i2]);

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
dnx_port_nif_db_active_calendars_nof_client_slots_dump(int  unit,  int  active_calendars_idx_0,  int  nof_client_slots_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = active_calendars_idx_0, I0 = active_calendars_idx_0 + 1;
    int i1 = nof_client_slots_idx_0, I1 = nof_client_slots_idx_0 + 1, org_i1 = nof_client_slots_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_nif active_calendars nof_client_slots") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_nif active_calendars nof_client_slots\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_port_nif_db/active_calendars/nof_client_slots.txt",
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","active_calendars[].nof_client_slots[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars
                , sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars))
        {
            LOG_CLI((BSL_META("dnx_port_nif_db[]->((dnx_port_nif_db_t*)sw_state_roots_array[][DNX_PORT_NIF_MODULE_ID])->active_calendars[].nof_client_slots[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[i0].nof_client_slots);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS,
                    ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[i0].nof_client_slots);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[i0].nof_client_slots
                    , sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[i0].nof_client_slots), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[i0].nof_client_slots))
            {
                LOG_CLI((BSL_META("dnx_port_nif_db[]->((dnx_port_nif_db_t*)sw_state_roots_array[][DNX_PORT_NIF_MODULE_ID])->active_calendars[].nof_client_slots[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__NOF_CLIENT_SLOTS, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[i0].nof_client_slots) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","active_calendars[%s%d].nof_client_slots[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[i0].nof_client_slots[i1]);

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
dnx_port_nif_db_active_calendars_last_client_dump(int  unit,  int  active_calendars_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = active_calendars_idx_0, I0 = active_calendars_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_nif active_calendars last_client") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_nif active_calendars last_client\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_port_nif_db/active_calendars/last_client.txt",
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","active_calendars[].last_client: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__LAST_CLIENT,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars
                , sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__LAST_CLIENT, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars))
        {
            LOG_CLI((BSL_META("dnx_port_nif_db[]->((dnx_port_nif_db_t*)sw_state_roots_array[][DNX_PORT_NIF_MODULE_ID])->active_calendars[].last_client: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_PORT_NIF_DB__ACTIVE_CALENDARS__LAST_CLIENT, ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","active_calendars[%s%d].last_client: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->active_calendars[i0].last_client);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_g_hao_active_client_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_nif g_hao_active_client") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_nif g_hao_active_client\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_port_nif_db/g_hao_active_client.txt",
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","g_hao_active_client: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","g_hao_active_client: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->g_hao_active_client);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_port_nif_db_priority_group_num_of_entries_dump(int  unit,  int  priority_group_num_of_entries_idx_0,  int  priority_group_num_of_entries_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = priority_group_num_of_entries_idx_0, I0 = priority_group_num_of_entries_idx_0 + 1;
    int i1 = priority_group_num_of_entries_idx_1, I1 = priority_group_num_of_entries_idx_1 + 1, org_i1 = priority_group_num_of_entries_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_nif priority_group_num_of_entries") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_nif priority_group_num_of_entries\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_port_nif_db/priority_group_num_of_entries.txt",
            "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","priority_group_num_of_entries[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_NIF_ETH_PRIORITY_GROUPS_NOF;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->priority_group_num_of_entries
                , sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->priority_group_num_of_entries), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_NIF_ETH_PRIORITY_GROUPS_NOF)
        {
            LOG_CLI((BSL_META("dnx_port_nif_db[]->((dnx_port_nif_db_t*)sw_state_roots_array[][DNX_PORT_NIF_MODULE_ID])->priority_group_num_of_entries[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_NIF_ETH_PRIORITY_GROUPS_NOF == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_NIF_OFR_NOF_RMC_PER_PRIORITY_GROUP;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->priority_group_num_of_entries[i0]
                    , sizeof(*((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->priority_group_num_of_entries[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_NIF_OFR_NOF_RMC_PER_PRIORITY_GROUP)
            {
                LOG_CLI((BSL_META("dnx_port_nif_db[]->((dnx_port_nif_db_t*)sw_state_roots_array[][DNX_PORT_NIF_MODULE_ID])->priority_group_num_of_entries[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_NIF_OFR_NOF_RMC_PER_PRIORITY_GROUP == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_port_nif_db[%d]->","((dnx_port_nif_db_t*)sw_state_roots_array[%d][DNX_PORT_NIF_MODULE_ID])->","priority_group_num_of_entries[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((dnx_port_nif_db_t*)sw_state_roots_array[unit][DNX_PORT_NIF_MODULE_ID])->priority_group_num_of_entries[i0][i1]);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




dnx_sw_state_diagnostic_info_t dnx_port_nif_db_info[SOC_MAX_NUM_DEVICES][DNX_PORT_NIF_DB_INFO_NOF_ENTRIES];
const char* dnx_port_nif_db_layout_str[DNX_PORT_NIF_DB_INFO_NOF_ENTRIES] = {
    "DNX_PORT_NIF_DB~",
    "DNX_PORT_NIF_DB~ARB_LINK_LIST~",
    "DNX_PORT_NIF_DB~ARB_LINK_LIST~LIST_INFO~",
    "DNX_PORT_NIF_DB~ARB_LINK_LIST~LIST_INFO~HEAD~",
    "DNX_PORT_NIF_DB~ARB_LINK_LIST~LIST_INFO~TAIL~",
    "DNX_PORT_NIF_DB~ARB_LINK_LIST~LIST_INFO~SIZE~",
    "DNX_PORT_NIF_DB~ARB_LINK_LIST~LINKING_INFO~",
    "DNX_PORT_NIF_DB~ARB_LINK_LIST~LINKING_INFO~NEXT_SECTION~",
    "DNX_PORT_NIF_DB~ARB_LINK_LIST~ALLOCATED_LIST_INFO~",
    "DNX_PORT_NIF_DB~ARB_LINK_LIST~ALLOCATED_LIST_INFO~HEAD~",
    "DNX_PORT_NIF_DB~ARB_LINK_LIST~ALLOCATED_LIST_INFO~TAIL~",
    "DNX_PORT_NIF_DB~ARB_LINK_LIST~ALLOCATED_LIST_INFO~SIZE~",
    "DNX_PORT_NIF_DB~OFT_LINK_LIST~",
    "DNX_PORT_NIF_DB~OFT_LINK_LIST~LIST_INFO~",
    "DNX_PORT_NIF_DB~OFT_LINK_LIST~LIST_INFO~HEAD~",
    "DNX_PORT_NIF_DB~OFT_LINK_LIST~LIST_INFO~TAIL~",
    "DNX_PORT_NIF_DB~OFT_LINK_LIST~LIST_INFO~SIZE~",
    "DNX_PORT_NIF_DB~OFT_LINK_LIST~LINKING_INFO~",
    "DNX_PORT_NIF_DB~OFT_LINK_LIST~LINKING_INFO~NEXT_SECTION~",
    "DNX_PORT_NIF_DB~OFT_LINK_LIST~ALLOCATED_LIST_INFO~",
    "DNX_PORT_NIF_DB~OFT_LINK_LIST~ALLOCATED_LIST_INFO~HEAD~",
    "DNX_PORT_NIF_DB~OFT_LINK_LIST~ALLOCATED_LIST_INFO~TAIL~",
    "DNX_PORT_NIF_DB~OFT_LINK_LIST~ALLOCATED_LIST_INFO~SIZE~",
    "DNX_PORT_NIF_DB~OFR_LINK_LIST~",
    "DNX_PORT_NIF_DB~OFR_LINK_LIST~LIST_INFO~",
    "DNX_PORT_NIF_DB~OFR_LINK_LIST~LIST_INFO~HEAD~",
    "DNX_PORT_NIF_DB~OFR_LINK_LIST~LIST_INFO~TAIL~",
    "DNX_PORT_NIF_DB~OFR_LINK_LIST~LIST_INFO~SIZE~",
    "DNX_PORT_NIF_DB~OFR_LINK_LIST~LINKING_INFO~",
    "DNX_PORT_NIF_DB~OFR_LINK_LIST~LINKING_INFO~NEXT_SECTION~",
    "DNX_PORT_NIF_DB~OFR_LINK_LIST~ALLOCATED_LIST_INFO~",
    "DNX_PORT_NIF_DB~OFR_LINK_LIST~ALLOCATED_LIST_INFO~HEAD~",
    "DNX_PORT_NIF_DB~OFR_LINK_LIST~ALLOCATED_LIST_INFO~TAIL~",
    "DNX_PORT_NIF_DB~OFR_LINK_LIST~ALLOCATED_LIST_INFO~SIZE~",
    "DNX_PORT_NIF_DB~ACTIVE_CALENDARS~",
    "DNX_PORT_NIF_DB~ACTIVE_CALENDARS~CLIENT_LAST_OBJECTS~",
    "DNX_PORT_NIF_DB~ACTIVE_CALENDARS~NOF_CLIENT_SLOTS~",
    "DNX_PORT_NIF_DB~ACTIVE_CALENDARS~LAST_CLIENT~",
    "DNX_PORT_NIF_DB~G_HAO_ACTIVE_CLIENT~",
    "DNX_PORT_NIF_DB~PRIORITY_GROUP_NUM_OF_ENTRIES~",
};
#endif 
#undef BSL_LOG_MODULE
