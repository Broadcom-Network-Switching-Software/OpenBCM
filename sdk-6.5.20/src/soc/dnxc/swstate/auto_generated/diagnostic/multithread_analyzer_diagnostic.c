
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
#include <soc/dnxc/swstate/auto_generated/diagnostic/multithread_analyzer_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern multithread_analyzer_t * multithread_analyzer_root[SOC_MAX_NUM_DEVICES];
extern multithread_analyzer_t * multithread_analyzer_data[SOC_MAX_NUM_DEVICES];



int
multithread_analyzer_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(multithread_analyzer_htb_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
multithread_analyzer_htb_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_htbl_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "multithread_analyzer htb") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate multithread_analyzer htb\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "multithread_analyzer/htb.txt",
            "multithread_analyzer[%d]->","((multithread_analyzer_t*)multithread_analyzer_root[%d])->","htb: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((multithread_analyzer_t*)multithread_analyzer_root[unit]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "multithread_analyzer[%d]->","((multithread_analyzer_t*)multithread_analyzer_root[%d])->","htb: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        SW_STATE_HTB_DEFAULT_PRINT(
            unit,
            MULTITHREAD_ANALYZER_MODULE_ID,
            &((multithread_analyzer_t*)multithread_analyzer_root[unit])->htb);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t multithread_analyzer_info[SOC_MAX_NUM_DEVICES][MULTITHREAD_ANALYZER_INFO_NOF_ENTRIES];
const char* multithread_analyzer_layout_str[MULTITHREAD_ANALYZER_INFO_NOF_ENTRIES] = {
    "MULTITHREAD_ANALYZER~",
    "MULTITHREAD_ANALYZER~HTB~",
};
#endif 
#undef BSL_LOG_MODULE
