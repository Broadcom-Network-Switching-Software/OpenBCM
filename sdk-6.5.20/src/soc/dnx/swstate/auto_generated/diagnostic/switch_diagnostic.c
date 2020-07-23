
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
    SHR_IF_ERR_EXIT(switch_db_l3mcastl2_fwd_type_dump(unit, filters));
    SHR_IF_ERR_EXIT(switch_db_header_enablers_hashing_dump(unit, -1, filters));
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
switch_db_l3mcastl2_fwd_type_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "switch l3mcastl2_fwd_type") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate switch l3mcastl2_fwd_type\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "switch_db/l3mcastl2_fwd_type.txt",
            "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","l3mcastl2_fwd_type: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "switch_db[%d]->","((switch_sw_state_t*)sw_state_roots_array[%d][SWITCH_MODULE_ID])->","l3mcastl2_fwd_type: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint8(
            unit,
            &((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->l3mcastl2_fwd_type);

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





dnx_sw_state_diagnostic_info_t switch_db_info[SOC_MAX_NUM_DEVICES][SWITCH_DB_INFO_NOF_ENTRIES];
const char* switch_db_layout_str[SWITCH_DB_INFO_NOF_ENTRIES] = {
    "SWITCH_DB~",
    "SWITCH_DB~MODULE_VERIFICATION~",
    "SWITCH_DB~MODULE_ERROR_RECOVERY~",
    "SWITCH_DB~L3MCASTL2_FWD_TYPE~",
    "SWITCH_DB~HEADER_ENABLERS_HASHING~",
};
#endif 
#undef BSL_LOG_MODULE
