
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
#include <soc/dnx/swstate/auto_generated/diagnostic/l2_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern l2_db_t * l2_db_context_data[SOC_MAX_NUM_DEVICES];



int
l2_db_context_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(l2_db_context_fid_profiles_changed_dump(unit, filters));
    SHR_IF_ERR_EXIT(l2_db_context_opportunistic_learning_ctx_dump(unit, filters));
    SHR_IF_ERR_EXIT(l2_db_context_routing_learning_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_fid_profiles_changed_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "l2 fid_profiles_changed") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate l2 fid_profiles_changed\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "l2_db_context/fid_profiles_changed.txt",
            "l2_db_context[%d]->","((l2_db_t*)sw_state_roots_array[%d][L2_MODULE_ID])->","fid_profiles_changed: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "l2_db_context[%d]->","((l2_db_t*)sw_state_roots_array[%d][L2_MODULE_ID])->","fid_profiles_changed: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint8(
            unit,
            &((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->fid_profiles_changed);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(l2_db_context_opportunistic_learning_ctx_ipv4uc_counter_dump(unit, filters));
    SHR_IF_ERR_EXIT(l2_db_context_opportunistic_learning_ctx_ipv4mc_counter_dump(unit, filters));
    SHR_IF_ERR_EXIT(l2_db_context_opportunistic_learning_ctx_ipv6uc_counter_dump(unit, filters));
    SHR_IF_ERR_EXIT(l2_db_context_opportunistic_learning_ctx_ipv6mc_counter_dump(unit, filters));
    SHR_IF_ERR_EXIT(l2_db_context_opportunistic_learning_ctx_mpls_counter_dump(unit, filters));
    SHR_IF_ERR_EXIT(l2_db_context_opportunistic_learning_ctx_ipv4uc_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(l2_db_context_opportunistic_learning_ctx_ipv4mc_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(l2_db_context_opportunistic_learning_ctx_ipv6uc_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(l2_db_context_opportunistic_learning_ctx_ipv6mc_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(l2_db_context_opportunistic_learning_ctx_mpls_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_ipv4uc_counter_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "l2 opportunistic_learning_ctx ipv4uc_counter") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate l2 opportunistic_learning_ctx ipv4uc_counter\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "l2_db_context/opportunistic_learning_ctx/ipv4uc_counter.txt",
            "l2_db_context[%d]->","((l2_db_t*)sw_state_roots_array[%d][L2_MODULE_ID])->","opportunistic_learning_ctx.ipv4uc_counter: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "l2_db_context[%d]->","((l2_db_t*)sw_state_roots_array[%d][L2_MODULE_ID])->","opportunistic_learning_ctx.ipv4uc_counter: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv4uc_counter);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_ipv4mc_counter_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "l2 opportunistic_learning_ctx ipv4mc_counter") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate l2 opportunistic_learning_ctx ipv4mc_counter\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "l2_db_context/opportunistic_learning_ctx/ipv4mc_counter.txt",
            "l2_db_context[%d]->","((l2_db_t*)sw_state_roots_array[%d][L2_MODULE_ID])->","opportunistic_learning_ctx.ipv4mc_counter: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "l2_db_context[%d]->","((l2_db_t*)sw_state_roots_array[%d][L2_MODULE_ID])->","opportunistic_learning_ctx.ipv4mc_counter: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv4mc_counter);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_ipv6uc_counter_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "l2 opportunistic_learning_ctx ipv6uc_counter") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate l2 opportunistic_learning_ctx ipv6uc_counter\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "l2_db_context/opportunistic_learning_ctx/ipv6uc_counter.txt",
            "l2_db_context[%d]->","((l2_db_t*)sw_state_roots_array[%d][L2_MODULE_ID])->","opportunistic_learning_ctx.ipv6uc_counter: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "l2_db_context[%d]->","((l2_db_t*)sw_state_roots_array[%d][L2_MODULE_ID])->","opportunistic_learning_ctx.ipv6uc_counter: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv6uc_counter);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_ipv6mc_counter_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "l2 opportunistic_learning_ctx ipv6mc_counter") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate l2 opportunistic_learning_ctx ipv6mc_counter\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "l2_db_context/opportunistic_learning_ctx/ipv6mc_counter.txt",
            "l2_db_context[%d]->","((l2_db_t*)sw_state_roots_array[%d][L2_MODULE_ID])->","opportunistic_learning_ctx.ipv6mc_counter: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "l2_db_context[%d]->","((l2_db_t*)sw_state_roots_array[%d][L2_MODULE_ID])->","opportunistic_learning_ctx.ipv6mc_counter: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv6mc_counter);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_mpls_counter_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "l2 opportunistic_learning_ctx mpls_counter") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate l2 opportunistic_learning_ctx mpls_counter\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "l2_db_context/opportunistic_learning_ctx/mpls_counter.txt",
            "l2_db_context[%d]->","((l2_db_t*)sw_state_roots_array[%d][L2_MODULE_ID])->","opportunistic_learning_ctx.mpls_counter: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "l2_db_context[%d]->","((l2_db_t*)sw_state_roots_array[%d][L2_MODULE_ID])->","opportunistic_learning_ctx.mpls_counter: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.mpls_counter);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_ipv4uc_dump(int  unit,  int  ipv4uc_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = ipv4uc_idx_0, I0 = ipv4uc_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "l2 opportunistic_learning_ctx ipv4uc") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate l2 opportunistic_learning_ctx ipv4uc\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "l2_db_context/opportunistic_learning_ctx/ipv4uc.txt",
            "l2_db_context[%d]->","((l2_db_t*)sw_state_roots_array[%d][L2_MODULE_ID])->","opportunistic_learning_ctx.ipv4uc[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID]));

        if (i0 == -1) {
            I0 = 64;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv4uc
                , sizeof(*((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv4uc), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 64)
        {
            LOG_CLI((BSL_META("l2_db_context[]->((l2_db_t*)sw_state_roots_array[][L2_MODULE_ID])->opportunistic_learning_ctx.ipv4uc[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(64 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "l2_db_context[%d]->","((l2_db_t*)sw_state_roots_array[%d][L2_MODULE_ID])->","opportunistic_learning_ctx.ipv4uc[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv4uc[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_ipv4mc_dump(int  unit,  int  ipv4mc_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = ipv4mc_idx_0, I0 = ipv4mc_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "l2 opportunistic_learning_ctx ipv4mc") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate l2 opportunistic_learning_ctx ipv4mc\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "l2_db_context/opportunistic_learning_ctx/ipv4mc.txt",
            "l2_db_context[%d]->","((l2_db_t*)sw_state_roots_array[%d][L2_MODULE_ID])->","opportunistic_learning_ctx.ipv4mc[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID]));

        if (i0 == -1) {
            I0 = 64;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv4mc
                , sizeof(*((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv4mc), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 64)
        {
            LOG_CLI((BSL_META("l2_db_context[]->((l2_db_t*)sw_state_roots_array[][L2_MODULE_ID])->opportunistic_learning_ctx.ipv4mc[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(64 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "l2_db_context[%d]->","((l2_db_t*)sw_state_roots_array[%d][L2_MODULE_ID])->","opportunistic_learning_ctx.ipv4mc[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv4mc[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_ipv6uc_dump(int  unit,  int  ipv6uc_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = ipv6uc_idx_0, I0 = ipv6uc_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "l2 opportunistic_learning_ctx ipv6uc") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate l2 opportunistic_learning_ctx ipv6uc\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "l2_db_context/opportunistic_learning_ctx/ipv6uc.txt",
            "l2_db_context[%d]->","((l2_db_t*)sw_state_roots_array[%d][L2_MODULE_ID])->","opportunistic_learning_ctx.ipv6uc[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID]));

        if (i0 == -1) {
            I0 = 64;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv6uc
                , sizeof(*((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv6uc), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 64)
        {
            LOG_CLI((BSL_META("l2_db_context[]->((l2_db_t*)sw_state_roots_array[][L2_MODULE_ID])->opportunistic_learning_ctx.ipv6uc[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(64 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "l2_db_context[%d]->","((l2_db_t*)sw_state_roots_array[%d][L2_MODULE_ID])->","opportunistic_learning_ctx.ipv6uc[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv6uc[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_ipv6mc_dump(int  unit,  int  ipv6mc_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = ipv6mc_idx_0, I0 = ipv6mc_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "l2 opportunistic_learning_ctx ipv6mc") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate l2 opportunistic_learning_ctx ipv6mc\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "l2_db_context/opportunistic_learning_ctx/ipv6mc.txt",
            "l2_db_context[%d]->","((l2_db_t*)sw_state_roots_array[%d][L2_MODULE_ID])->","opportunistic_learning_ctx.ipv6mc[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID]));

        if (i0 == -1) {
            I0 = 64;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv6mc
                , sizeof(*((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv6mc), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 64)
        {
            LOG_CLI((BSL_META("l2_db_context[]->((l2_db_t*)sw_state_roots_array[][L2_MODULE_ID])->opportunistic_learning_ctx.ipv6mc[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(64 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "l2_db_context[%d]->","((l2_db_t*)sw_state_roots_array[%d][L2_MODULE_ID])->","opportunistic_learning_ctx.ipv6mc[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.ipv6mc[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_opportunistic_learning_ctx_mpls_dump(int  unit,  int  mpls_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = mpls_idx_0, I0 = mpls_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "l2 opportunistic_learning_ctx mpls") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate l2 opportunistic_learning_ctx mpls\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "l2_db_context/opportunistic_learning_ctx/mpls.txt",
            "l2_db_context[%d]->","((l2_db_t*)sw_state_roots_array[%d][L2_MODULE_ID])->","opportunistic_learning_ctx.mpls[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID]));

        if (i0 == -1) {
            I0 = 64;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.mpls
                , sizeof(*((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.mpls), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 64)
        {
            LOG_CLI((BSL_META("l2_db_context[]->((l2_db_t*)sw_state_roots_array[][L2_MODULE_ID])->opportunistic_learning_ctx.mpls[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(64 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "l2_db_context[%d]->","((l2_db_t*)sw_state_roots_array[%d][L2_MODULE_ID])->","opportunistic_learning_ctx.mpls[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->opportunistic_learning_ctx.mpls[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_routing_learning_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "l2 routing_learning") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate l2 routing_learning\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "l2_db_context/routing_learning.txt",
            "l2_db_context[%d]->","((l2_db_t*)sw_state_roots_array[%d][L2_MODULE_ID])->","routing_learning: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "l2_db_context[%d]->","((l2_db_t*)sw_state_roots_array[%d][L2_MODULE_ID])->","routing_learning: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint32(
            unit,
            &((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->routing_learning);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t l2_db_context_info[SOC_MAX_NUM_DEVICES][L2_DB_CONTEXT_INFO_NOF_ENTRIES];
const char* l2_db_context_layout_str[L2_DB_CONTEXT_INFO_NOF_ENTRIES] = {
    "L2_DB_CONTEXT~",
    "L2_DB_CONTEXT~FID_PROFILES_CHANGED~",
    "L2_DB_CONTEXT~OPPORTUNISTIC_LEARNING_CTX~",
    "L2_DB_CONTEXT~OPPORTUNISTIC_LEARNING_CTX~IPV4UC_COUNTER~",
    "L2_DB_CONTEXT~OPPORTUNISTIC_LEARNING_CTX~IPV4MC_COUNTER~",
    "L2_DB_CONTEXT~OPPORTUNISTIC_LEARNING_CTX~IPV6UC_COUNTER~",
    "L2_DB_CONTEXT~OPPORTUNISTIC_LEARNING_CTX~IPV6MC_COUNTER~",
    "L2_DB_CONTEXT~OPPORTUNISTIC_LEARNING_CTX~MPLS_COUNTER~",
    "L2_DB_CONTEXT~OPPORTUNISTIC_LEARNING_CTX~IPV4UC~",
    "L2_DB_CONTEXT~OPPORTUNISTIC_LEARNING_CTX~IPV4MC~",
    "L2_DB_CONTEXT~OPPORTUNISTIC_LEARNING_CTX~IPV6UC~",
    "L2_DB_CONTEXT~OPPORTUNISTIC_LEARNING_CTX~IPV6MC~",
    "L2_DB_CONTEXT~OPPORTUNISTIC_LEARNING_CTX~MPLS~",
    "L2_DB_CONTEXT~ROUTING_LEARNING~",
};
#endif 
#undef BSL_LOG_MODULE
