
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#ifdef BCM_DNX_SUPPORT
#if defined(INCLUDE_KBP)
#include <soc/dnxc/swstate/dnxc_sw_state_c_includes.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/kbp_common_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern kbp_common_sw_state_t * kbp_common_sw_state_data[SOC_MAX_NUM_DEVICES];



int
kbp_common_sw_state_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(kbp_common_sw_state_kbp_fwd_caching_enabled_dump(unit, filters));
    SHR_IF_ERR_EXIT(kbp_common_sw_state_kbp_acl_caching_enabled_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_common_sw_state_kbp_fwd_caching_enabled_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "kbp_common kbp_fwd_caching_enabled") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate kbp_common kbp_fwd_caching_enabled\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "kbp_common_sw_state/kbp_fwd_caching_enabled.txt",
            "kbp_common_sw_state[%d]->","((kbp_common_sw_state_t*)sw_state_roots_array[%d][KBP_COMMON_MODULE_ID])->","kbp_fwd_caching_enabled: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((kbp_common_sw_state_t*)sw_state_roots_array[unit][KBP_COMMON_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "kbp_common_sw_state[%d]->","((kbp_common_sw_state_t*)sw_state_roots_array[%d][KBP_COMMON_MODULE_ID])->","kbp_fwd_caching_enabled: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((kbp_common_sw_state_t*)sw_state_roots_array[unit][KBP_COMMON_MODULE_ID])->kbp_fwd_caching_enabled);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_common_sw_state_kbp_acl_caching_enabled_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "kbp_common kbp_acl_caching_enabled") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate kbp_common kbp_acl_caching_enabled\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "kbp_common_sw_state/kbp_acl_caching_enabled.txt",
            "kbp_common_sw_state[%d]->","((kbp_common_sw_state_t*)sw_state_roots_array[%d][KBP_COMMON_MODULE_ID])->","kbp_acl_caching_enabled: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((kbp_common_sw_state_t*)sw_state_roots_array[unit][KBP_COMMON_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "kbp_common_sw_state[%d]->","((kbp_common_sw_state_t*)sw_state_roots_array[%d][KBP_COMMON_MODULE_ID])->","kbp_acl_caching_enabled: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((kbp_common_sw_state_t*)sw_state_roots_array[unit][KBP_COMMON_MODULE_ID])->kbp_acl_caching_enabled);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t kbp_common_sw_state_info[SOC_MAX_NUM_DEVICES][KBP_COMMON_SW_STATE_INFO_NOF_ENTRIES];
const char* kbp_common_sw_state_layout_str[KBP_COMMON_SW_STATE_INFO_NOF_ENTRIES] = {
    "KBP_COMMON_SW_STATE~",
    "KBP_COMMON_SW_STATE~KBP_FWD_CACHING_ENABLED~",
    "KBP_COMMON_SW_STATE~KBP_ACL_CACHING_ENABLED~",
};
#endif 
#endif  
#endif  
#undef BSL_LOG_MODULE
