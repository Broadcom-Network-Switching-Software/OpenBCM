
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/diagnostic/switch_diagnostic.h>
#include <bcm/switch.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern switch_sw_state_t * switch_db_data[SOC_MAX_NUM_DEVICES];



int
switch_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(switch_db_module_verification_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(switch_db_module_error_recovery_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(switch_db_l3mcastl2_ipv4_fwd_type_dump(unit, filters));
    SHR_IF_ERR_EXIT(switch_db_l3mcastl2_ipv6_fwd_type_dump(unit, filters));
    SHR_IF_ERR_EXIT(switch_db_header_enablers_hashing_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(switch_db_tunnel_route_disabled_ctx_dump(unit, filters));
    SHR_IF_ERR_EXIT(switch_db_tunnel_route_disabled_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_module_verification_dump(int  unit,  int  module_verification_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = module_verification_idx_0, I0 = module_verification_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "switch module_verification") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate switch module_verification\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "switch_db/module_verification.txt",
            "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","module_verification[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID]));

        if (i0 == -1) {
            I0 = bcmModuleCount;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->module_verification
                , sizeof(*((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->module_verification), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= bcmModuleCount)
        {
            LOG_CLI((BSL_META("switch_db[]->((switch_sw_state_t*)sw_state_roots_array[][SWITCH_MODULE_ID])->module_verification[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(bcmModuleCount == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","module_verification[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->module_verification[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_module_error_recovery_dump(int  unit,  int  module_error_recovery_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = module_error_recovery_idx_0, I0 = module_error_recovery_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "switch module_error_recovery") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate switch module_error_recovery\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "switch_db/module_error_recovery.txt",
            "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","module_error_recovery[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID]));

        if (i0 == -1) {
            I0 = bcmModuleCount;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->module_error_recovery
                , sizeof(*((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->module_error_recovery), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= bcmModuleCount)
        {
            LOG_CLI((BSL_META("switch_db[]->((switch_sw_state_t*)sw_state_roots_array[][SWITCH_MODULE_ID])->module_error_recovery[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(bcmModuleCount == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","module_error_recovery[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->module_error_recovery[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_l3mcastl2_ipv4_fwd_type_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "switch l3mcastl2_ipv4_fwd_type") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate switch l3mcastl2_ipv4_fwd_type\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "switch_db/l3mcastl2_ipv4_fwd_type.txt",
            "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","l3mcastl2_ipv4_fwd_type: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","l3mcastl2_ipv4_fwd_type: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint8(
            unit,
            &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->l3mcastl2_ipv4_fwd_type);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_l3mcastl2_ipv6_fwd_type_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "switch l3mcastl2_ipv6_fwd_type") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate switch l3mcastl2_ipv6_fwd_type\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "switch_db/l3mcastl2_ipv6_fwd_type.txt",
            "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","l3mcastl2_ipv6_fwd_type: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","l3mcastl2_ipv6_fwd_type: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint8(
            unit,
            &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->l3mcastl2_ipv6_fwd_type);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_header_enablers_hashing_dump(int  unit,  int  header_enablers_hashing_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = header_enablers_hashing_idx_0, I0 = header_enablers_hashing_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "switch header_enablers_hashing") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate switch header_enablers_hashing\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "switch_db/header_enablers_hashing.txt",
            "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","header_enablers_hashing[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID]));

        if (i0 == -1) {
            I0 = DBAL_NOF_ENUM_HASH_FIELD_ENABLERS_HEADER_VALUES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->header_enablers_hashing
                , sizeof(*((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->header_enablers_hashing), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DBAL_NOF_ENUM_HASH_FIELD_ENABLERS_HEADER_VALUES)
        {
            LOG_CLI((BSL_META("switch_db[]->((switch_sw_state_t*)sw_state_roots_array[][SWITCH_MODULE_ID])->header_enablers_hashing[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DBAL_NOF_ENUM_HASH_FIELD_ENABLERS_HEADER_VALUES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","header_enablers_hashing[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->header_enablers_hashing[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(switch_db_tunnel_route_disabled_ctx_icmp_counter_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(switch_db_tunnel_route_disabled_ctx_igmp_counter_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(switch_db_tunnel_route_disabled_ctx_udp_counter_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(switch_db_tunnel_route_disabled_ctx_tcp_counter_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(switch_db_tunnel_route_disabled_ctx_unknown_counter_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(switch_db_tunnel_route_disabled_ctx_gre_keep_alive_counter_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(switch_db_tunnel_route_disabled_ctx_icmp_ctx_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(switch_db_tunnel_route_disabled_ctx_igmp_ctx_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(switch_db_tunnel_route_disabled_ctx_udp_ctx_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(switch_db_tunnel_route_disabled_ctx_tcp_ctx_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(switch_db_tunnel_route_disabled_ctx_unknown_ctx_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(switch_db_tunnel_route_disabled_ctx_gre_keep_alive_ctx_dump(unit, -1, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_icmp_counter_dump(int  unit,  int  icmp_counter_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = icmp_counter_idx_0, I0 = icmp_counter_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "switch tunnel_route_disabled_ctx icmp_counter") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate switch tunnel_route_disabled_ctx icmp_counter\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "switch_db/tunnel_route_disabled_ctx/icmp_counter.txt",
            "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","tunnel_route_disabled_ctx.icmp_counter[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID]));

        if (i0 == -1) {
            I0 = 4;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.icmp_counter
                , sizeof(*((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.icmp_counter), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 4)
        {
            LOG_CLI((BSL_META("switch_db[]->((switch_sw_state_t*)sw_state_roots_array[][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.icmp_counter[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(4 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","tunnel_route_disabled_ctx.icmp_counter[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.icmp_counter[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_igmp_counter_dump(int  unit,  int  igmp_counter_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = igmp_counter_idx_0, I0 = igmp_counter_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "switch tunnel_route_disabled_ctx igmp_counter") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate switch tunnel_route_disabled_ctx igmp_counter\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "switch_db/tunnel_route_disabled_ctx/igmp_counter.txt",
            "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","tunnel_route_disabled_ctx.igmp_counter[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID]));

        if (i0 == -1) {
            I0 = 4;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.igmp_counter
                , sizeof(*((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.igmp_counter), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 4)
        {
            LOG_CLI((BSL_META("switch_db[]->((switch_sw_state_t*)sw_state_roots_array[][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.igmp_counter[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(4 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","tunnel_route_disabled_ctx.igmp_counter[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.igmp_counter[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_udp_counter_dump(int  unit,  int  udp_counter_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = udp_counter_idx_0, I0 = udp_counter_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "switch tunnel_route_disabled_ctx udp_counter") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate switch tunnel_route_disabled_ctx udp_counter\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "switch_db/tunnel_route_disabled_ctx/udp_counter.txt",
            "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","tunnel_route_disabled_ctx.udp_counter[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID]));

        if (i0 == -1) {
            I0 = 4;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.udp_counter
                , sizeof(*((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.udp_counter), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 4)
        {
            LOG_CLI((BSL_META("switch_db[]->((switch_sw_state_t*)sw_state_roots_array[][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.udp_counter[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(4 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","tunnel_route_disabled_ctx.udp_counter[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.udp_counter[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_tcp_counter_dump(int  unit,  int  tcp_counter_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = tcp_counter_idx_0, I0 = tcp_counter_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "switch tunnel_route_disabled_ctx tcp_counter") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate switch tunnel_route_disabled_ctx tcp_counter\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "switch_db/tunnel_route_disabled_ctx/tcp_counter.txt",
            "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","tunnel_route_disabled_ctx.tcp_counter[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID]));

        if (i0 == -1) {
            I0 = 4;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.tcp_counter
                , sizeof(*((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.tcp_counter), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 4)
        {
            LOG_CLI((BSL_META("switch_db[]->((switch_sw_state_t*)sw_state_roots_array[][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.tcp_counter[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(4 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","tunnel_route_disabled_ctx.tcp_counter[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.tcp_counter[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_unknown_counter_dump(int  unit,  int  unknown_counter_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = unknown_counter_idx_0, I0 = unknown_counter_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "switch tunnel_route_disabled_ctx unknown_counter") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate switch tunnel_route_disabled_ctx unknown_counter\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "switch_db/tunnel_route_disabled_ctx/unknown_counter.txt",
            "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","tunnel_route_disabled_ctx.unknown_counter[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID]));

        if (i0 == -1) {
            I0 = 4;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.unknown_counter
                , sizeof(*((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.unknown_counter), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 4)
        {
            LOG_CLI((BSL_META("switch_db[]->((switch_sw_state_t*)sw_state_roots_array[][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.unknown_counter[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(4 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","tunnel_route_disabled_ctx.unknown_counter[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.unknown_counter[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_gre_keep_alive_counter_dump(int  unit,  int  gre_keep_alive_counter_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = gre_keep_alive_counter_idx_0, I0 = gre_keep_alive_counter_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "switch tunnel_route_disabled_ctx gre_keep_alive_counter") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate switch tunnel_route_disabled_ctx gre_keep_alive_counter\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "switch_db/tunnel_route_disabled_ctx/gre_keep_alive_counter.txt",
            "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","tunnel_route_disabled_ctx.gre_keep_alive_counter[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID]));

        if (i0 == -1) {
            I0 = 4;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.gre_keep_alive_counter
                , sizeof(*((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.gre_keep_alive_counter), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 4)
        {
            LOG_CLI((BSL_META("switch_db[]->((switch_sw_state_t*)sw_state_roots_array[][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.gre_keep_alive_counter[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(4 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","tunnel_route_disabled_ctx.gre_keep_alive_counter[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.gre_keep_alive_counter[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
switch_db_tunnel_route_disabled_ctx_icmp_ctx_dump(int  unit,  int  icmp_ctx_idx_0,  int  icmp_ctx_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = icmp_ctx_idx_0, I0 = icmp_ctx_idx_0 + 1;
    int i1 = icmp_ctx_idx_1, I1 = icmp_ctx_idx_1 + 1, org_i1 = icmp_ctx_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "switch tunnel_route_disabled_ctx icmp_ctx") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate switch tunnel_route_disabled_ctx icmp_ctx\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "switch_db/tunnel_route_disabled_ctx/icmp_ctx.txt",
            "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","tunnel_route_disabled_ctx.icmp_ctx[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID]));

        if (i0 == -1) {
            I0 = 4;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.icmp_ctx
                , sizeof(*((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.icmp_ctx), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 4)
        {
            LOG_CLI((BSL_META("switch_db[]->((switch_sw_state_t*)sw_state_roots_array[][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.icmp_ctx[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(4 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = 7;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.icmp_ctx[i0]
                    , sizeof(*((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.icmp_ctx[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= 7)
            {
                LOG_CLI((BSL_META("switch_db[]->((switch_sw_state_t*)sw_state_roots_array[][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.icmp_ctx[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(7 == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","tunnel_route_disabled_ctx.icmp_ctx[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.icmp_ctx[i0][i1]);

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
switch_db_tunnel_route_disabled_ctx_igmp_ctx_dump(int  unit,  int  igmp_ctx_idx_0,  int  igmp_ctx_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = igmp_ctx_idx_0, I0 = igmp_ctx_idx_0 + 1;
    int i1 = igmp_ctx_idx_1, I1 = igmp_ctx_idx_1 + 1, org_i1 = igmp_ctx_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "switch tunnel_route_disabled_ctx igmp_ctx") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate switch tunnel_route_disabled_ctx igmp_ctx\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "switch_db/tunnel_route_disabled_ctx/igmp_ctx.txt",
            "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","tunnel_route_disabled_ctx.igmp_ctx[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID]));

        if (i0 == -1) {
            I0 = 4;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.igmp_ctx
                , sizeof(*((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.igmp_ctx), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 4)
        {
            LOG_CLI((BSL_META("switch_db[]->((switch_sw_state_t*)sw_state_roots_array[][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.igmp_ctx[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(4 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = 7;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.igmp_ctx[i0]
                    , sizeof(*((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.igmp_ctx[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= 7)
            {
                LOG_CLI((BSL_META("switch_db[]->((switch_sw_state_t*)sw_state_roots_array[][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.igmp_ctx[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(7 == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","tunnel_route_disabled_ctx.igmp_ctx[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.igmp_ctx[i0][i1]);

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
switch_db_tunnel_route_disabled_ctx_udp_ctx_dump(int  unit,  int  udp_ctx_idx_0,  int  udp_ctx_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = udp_ctx_idx_0, I0 = udp_ctx_idx_0 + 1;
    int i1 = udp_ctx_idx_1, I1 = udp_ctx_idx_1 + 1, org_i1 = udp_ctx_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "switch tunnel_route_disabled_ctx udp_ctx") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate switch tunnel_route_disabled_ctx udp_ctx\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "switch_db/tunnel_route_disabled_ctx/udp_ctx.txt",
            "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","tunnel_route_disabled_ctx.udp_ctx[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID]));

        if (i0 == -1) {
            I0 = 4;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.udp_ctx
                , sizeof(*((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.udp_ctx), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 4)
        {
            LOG_CLI((BSL_META("switch_db[]->((switch_sw_state_t*)sw_state_roots_array[][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.udp_ctx[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(4 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = 7;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.udp_ctx[i0]
                    , sizeof(*((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.udp_ctx[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= 7)
            {
                LOG_CLI((BSL_META("switch_db[]->((switch_sw_state_t*)sw_state_roots_array[][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.udp_ctx[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(7 == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","tunnel_route_disabled_ctx.udp_ctx[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.udp_ctx[i0][i1]);

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
switch_db_tunnel_route_disabled_ctx_tcp_ctx_dump(int  unit,  int  tcp_ctx_idx_0,  int  tcp_ctx_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = tcp_ctx_idx_0, I0 = tcp_ctx_idx_0 + 1;
    int i1 = tcp_ctx_idx_1, I1 = tcp_ctx_idx_1 + 1, org_i1 = tcp_ctx_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "switch tunnel_route_disabled_ctx tcp_ctx") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate switch tunnel_route_disabled_ctx tcp_ctx\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "switch_db/tunnel_route_disabled_ctx/tcp_ctx.txt",
            "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","tunnel_route_disabled_ctx.tcp_ctx[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID]));

        if (i0 == -1) {
            I0 = 4;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.tcp_ctx
                , sizeof(*((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.tcp_ctx), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 4)
        {
            LOG_CLI((BSL_META("switch_db[]->((switch_sw_state_t*)sw_state_roots_array[][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.tcp_ctx[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(4 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = 7;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.tcp_ctx[i0]
                    , sizeof(*((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.tcp_ctx[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= 7)
            {
                LOG_CLI((BSL_META("switch_db[]->((switch_sw_state_t*)sw_state_roots_array[][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.tcp_ctx[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(7 == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","tunnel_route_disabled_ctx.tcp_ctx[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.tcp_ctx[i0][i1]);

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
switch_db_tunnel_route_disabled_ctx_unknown_ctx_dump(int  unit,  int  unknown_ctx_idx_0,  int  unknown_ctx_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = unknown_ctx_idx_0, I0 = unknown_ctx_idx_0 + 1;
    int i1 = unknown_ctx_idx_1, I1 = unknown_ctx_idx_1 + 1, org_i1 = unknown_ctx_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "switch tunnel_route_disabled_ctx unknown_ctx") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate switch tunnel_route_disabled_ctx unknown_ctx\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "switch_db/tunnel_route_disabled_ctx/unknown_ctx.txt",
            "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","tunnel_route_disabled_ctx.unknown_ctx[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID]));

        if (i0 == -1) {
            I0 = 4;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.unknown_ctx
                , sizeof(*((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.unknown_ctx), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 4)
        {
            LOG_CLI((BSL_META("switch_db[]->((switch_sw_state_t*)sw_state_roots_array[][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.unknown_ctx[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(4 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = 7;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.unknown_ctx[i0]
                    , sizeof(*((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.unknown_ctx[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= 7)
            {
                LOG_CLI((BSL_META("switch_db[]->((switch_sw_state_t*)sw_state_roots_array[][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.unknown_ctx[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(7 == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","tunnel_route_disabled_ctx.unknown_ctx[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.unknown_ctx[i0][i1]);

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
switch_db_tunnel_route_disabled_ctx_gre_keep_alive_ctx_dump(int  unit,  int  gre_keep_alive_ctx_idx_0,  int  gre_keep_alive_ctx_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = gre_keep_alive_ctx_idx_0, I0 = gre_keep_alive_ctx_idx_0 + 1;
    int i1 = gre_keep_alive_ctx_idx_1, I1 = gre_keep_alive_ctx_idx_1 + 1, org_i1 = gre_keep_alive_ctx_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "switch tunnel_route_disabled_ctx gre_keep_alive_ctx") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate switch tunnel_route_disabled_ctx gre_keep_alive_ctx\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "switch_db/tunnel_route_disabled_ctx/gre_keep_alive_ctx.txt",
            "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","tunnel_route_disabled_ctx.gre_keep_alive_ctx[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID]));

        if (i0 == -1) {
            I0 = 4;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.gre_keep_alive_ctx
                , sizeof(*((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.gre_keep_alive_ctx), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 4)
        {
            LOG_CLI((BSL_META("switch_db[]->((switch_sw_state_t*)sw_state_roots_array[][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.gre_keep_alive_ctx[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(4 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = 14;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.gre_keep_alive_ctx[i0]
                    , sizeof(*((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.gre_keep_alive_ctx[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= 14)
            {
                LOG_CLI((BSL_META("switch_db[]->((switch_sw_state_t*)sw_state_roots_array[][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.gre_keep_alive_ctx[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(14 == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","tunnel_route_disabled_ctx.gre_keep_alive_ctx[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled_ctx.gre_keep_alive_ctx[i0][i1]);

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
switch_db_tunnel_route_disabled_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "switch tunnel_route_disabled") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate switch tunnel_route_disabled\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "switch_db/tunnel_route_disabled.txt",
            "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","tunnel_route_disabled: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","tunnel_route_disabled: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint32(
            unit,
            &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->tunnel_route_disabled);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t switch_db_info[SOC_MAX_NUM_DEVICES][SWITCH_DB_INFO_NOF_ENTRIES];
const char* switch_db_layout_str[SWITCH_DB_INFO_NOF_ENTRIES] = {
    "SWITCH_DB~",
    "SWITCH_DB~MODULE_VERIFICATION~",
    "SWITCH_DB~MODULE_ERROR_RECOVERY~",
    "SWITCH_DB~L3MCASTL2_IPV4_FWD_TYPE~",
    "SWITCH_DB~L3MCASTL2_IPV6_FWD_TYPE~",
    "SWITCH_DB~HEADER_ENABLERS_HASHING~",
    "SWITCH_DB~TUNNEL_ROUTE_DISABLED_CTX~",
    "SWITCH_DB~TUNNEL_ROUTE_DISABLED_CTX~ICMP_COUNTER~",
    "SWITCH_DB~TUNNEL_ROUTE_DISABLED_CTX~IGMP_COUNTER~",
    "SWITCH_DB~TUNNEL_ROUTE_DISABLED_CTX~UDP_COUNTER~",
    "SWITCH_DB~TUNNEL_ROUTE_DISABLED_CTX~TCP_COUNTER~",
    "SWITCH_DB~TUNNEL_ROUTE_DISABLED_CTX~UNKNOWN_COUNTER~",
    "SWITCH_DB~TUNNEL_ROUTE_DISABLED_CTX~GRE_KEEP_ALIVE_COUNTER~",
    "SWITCH_DB~TUNNEL_ROUTE_DISABLED_CTX~ICMP_CTX~",
    "SWITCH_DB~TUNNEL_ROUTE_DISABLED_CTX~IGMP_CTX~",
    "SWITCH_DB~TUNNEL_ROUTE_DISABLED_CTX~UDP_CTX~",
    "SWITCH_DB~TUNNEL_ROUTE_DISABLED_CTX~TCP_CTX~",
    "SWITCH_DB~TUNNEL_ROUTE_DISABLED_CTX~UNKNOWN_CTX~",
    "SWITCH_DB~TUNNEL_ROUTE_DISABLED_CTX~GRE_KEEP_ALIVE_CTX~",
    "SWITCH_DB~TUNNEL_ROUTE_DISABLED~",
};
#endif 
#undef BSL_LOG_MODULE
