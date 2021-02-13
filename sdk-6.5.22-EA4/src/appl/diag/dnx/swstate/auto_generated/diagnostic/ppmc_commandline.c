/** \file dnx/swstate/auto_generated/diagnostic/ppmc_commandline.c
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

#include <bcm_int/dnx/algo/swstate/auto_generated/diagnostic/ppmc_diagnostic.h>
#include "ppmc_commandline.h"
#include <bcm_int/dnx/algo/swstate/auto_generated/access/ppmc_access.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
sh_sand_cmd_t sh_dnx_swstate_ppmc_cmds[] = {
    {"ppmc_res_manager", NULL, sh_dnx_swstate_ppmc_ppmc_res_manager_cmds, NULL, &dnx_swstate_ppmc_ppmc_res_manager_man},
    {"dump", sh_dnx_swstate_ppmc_dump_cmd, NULL, dnx_swstate_ppmc_dump_options, &dnx_swstate_ppmc_dump_man},
    {"size_get", sh_dnx_swstate_ppmc_size_get_cmd, NULL, dnx_swstate_ppmc_size_get_options, &dnx_swstate_ppmc_size_get_man},
    {"diagnostic_operation_counters", sh_dnx_swstate_ppmc_diagnostic_operation_counters_cmd, NULL, dnx_swstate_ppmc_diagnostic_operation_counters_options, &dnx_swstate_ppmc_diagnostic_operation_counters_man},
    {NULL}
};


shr_error_e sh_dnx_swstate_ppmc_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    uint8 is_init;
    dnx_sw_state_dump_filters_t filters;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("nocontent", filters.nocontent);
    SH_SAND_GET_STR("typefilter", filters.typefilter);
    SH_SAND_GET_STR("namefilter", filters.namefilter);

    ppmc_db.is_init(unit, &is_init);
    if (is_init) {
        SHR_IF_ERR_EXIT(ppmc_db_dump(unit, filters));
        if (!filters.nocontent)
        {
            dnx_sw_state_dump_detach_file(unit);
        }
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_ppmc_dump_options[] = {
    {"nocontent",      SAL_FIELD_TYPE_BOOL, "Decision if to perform content dump or to print the access tree.",  "false"},
    {"typefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable type.",         ""},
    {"namefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable name.",         ""},
    {NULL}
};


sh_sand_man_t dnx_swstate_ppmc_dump_man = {
    "swstate ppmc dump",
    "print the module's content",
    "swstate ppmc dump",
    "swstate ppmc dump nocontent=false typefilter="" namefilter="" ",
};


shr_error_e sh_dnx_swstate_ppmc_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    uint32 size = 0;
    SHR_FUNC_INIT_VARS(unit);

    size = dnx_sw_state_info_size_get(ppmc_db_info[unit], ppmc_db_layout_str, PPMC_DB_INFO_NOF_ENTRIES, "PPMC_DB~", 0);
    dnx_sw_state_size_print(size);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_ppmc_size_get_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_ppmc_size_get_man = {
    "swstate ppmc size_get",
    "print the module's size",
    "swstate ppmc size_get",
    "swstate ppmc size_get",
};


shr_error_e sh_dnx_swstate_ppmc_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{


    SHR_FUNC_INIT_VARS(unit);

    dnx_sw_state_diagnostic_operation_counters_print(ppmc_db_info[unit], ppmc_db_layout_str, PPMC_DB_INFO_NOF_ENTRIES, "PPMC_DB~");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_ppmc_diagnostic_operation_counters_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_ppmc_diagnostic_operation_counters_man = {
    "swstate ppmc diagnostic_operation_counters",
    "print the module's diagnostic operation counters",
    "swstate ppmc diagnostic_operation_counters",
    "swstate ppmc diagnostic_operation_counters",
};


sh_sand_man_t dnx_swstate_ppmc_man = {
    cmd_dnx_swstate_ppmc_desc,
    NULL,
    NULL,
    NULL,
};


const char cmd_dnx_swstate_ppmc_desc[] = "swstate ppmc commands";



sh_sand_cmd_t sh_dnx_swstate_ppmc_ppmc_res_manager_cmds[] = {
    {"dump", sh_dnx_swstate_ppmc_ppmc_res_manager_dump_cmd, NULL, dnx_swstate_ppmc_ppmc_res_manager_dump_options, &dnx_swstate_ppmc_ppmc_res_manager_dump_man},
    {"size_get", sh_dnx_swstate_ppmc_ppmc_res_manager_size_get_cmd, NULL, dnx_swstate_ppmc_ppmc_res_manager_size_get_options, &dnx_swstate_ppmc_ppmc_res_manager_size_get_man},
    {"diagnostic_operation_counters", sh_dnx_swstate_ppmc_ppmc_res_manager_diagnostic_operation_counters_cmd, NULL, dnx_swstate_ppmc_ppmc_res_manager_diagnostic_operation_counters_options, &dnx_swstate_ppmc_ppmc_res_manager_diagnostic_operation_counters_man},
    {NULL}
};


sh_sand_man_t dnx_swstate_ppmc_ppmc_res_manager_man = {
    cmd_dnx_swstate_ppmc_ppmc_res_manager_desc,
    NULL,
    NULL,
    NULL,
};


const char cmd_dnx_swstate_ppmc_ppmc_res_manager_desc[] = "ppmc_db ppmc_res_manager commands";


shr_error_e sh_dnx_swstate_ppmc_ppmc_res_manager_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    dnx_sw_state_dump_filters_t filters;

    SHR_FUNC_INIT_VARS(unit);


    SH_SAND_GET_BOOL("nocontent", filters.nocontent);
    SH_SAND_GET_STR("typefilter", filters.typefilter);
    SH_SAND_GET_STR("namefilter", filters.namefilter);

    SHR_IF_ERR_EXIT(ppmc_db_ppmc_res_manager_dump(unit, filters));

    if (!filters.nocontent)
    {
        dnx_sw_state_dump_detach_file(unit);

    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_ppmc_ppmc_res_manager_dump_options[] = {
    {"nocontent",      SAL_FIELD_TYPE_BOOL, "Decision if to perform content dump or to print the access tree.",  "false"},
    {"typefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable type.",         ""},
    {"namefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable name.",         ""},
    {NULL}
};


sh_sand_man_t dnx_swstate_ppmc_ppmc_res_manager_dump_man = {
    "swstate dump",
    "print the variable value",
    "ppmc_db ppmc_res_manager dump []\n",
    "ppmc_db ppmc_res_manager dump nocontent=false typefilter="" namefilter="" ",
};


shr_error_e sh_dnx_swstate_ppmc_ppmc_res_manager_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{
    uint32 size = 0;

    SHR_FUNC_INIT_VARS(unit);


    size = dnx_sw_state_info_size_get(ppmc_db_info[unit], ppmc_db_layout_str, PPMC_DB_INFO_NOF_ENTRIES, "PPMC_DB~PPMC_RES_MANAGER~", 0);
    dnx_sw_state_size_print(size);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_ppmc_ppmc_res_manager_size_get_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_ppmc_ppmc_res_manager_size_get_man = {
    "swstate size_get",
    "print the variable size",
    "ppmc_db ppmc_res_manager size_get",
    "ppmc_db ppmc_res_manager size_get",
};


shr_error_e sh_dnx_swstate_ppmc_ppmc_res_manager_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    SHR_FUNC_INIT_VARS(unit);


    dnx_sw_state_diagnostic_operation_counters_print(ppmc_db_info[unit], ppmc_db_layout_str, PPMC_DB_INFO_NOF_ENTRIES, "PPMC_DB~PPMC_RES_MANAGER~");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_ppmc_ppmc_res_manager_diagnostic_operation_counters_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_ppmc_ppmc_res_manager_diagnostic_operation_counters_man = {
    "swstate diagnostic_operation_counters",
    "print the diagnostic operation counters",
    "ppmc_db ppmc_res_manager diagnostic_operation_counters",
    "ppmc_db ppmc_res_manager diagnostic_operation_counters",
};


#endif /* DNX_SW_STATE_DIAGNOSTIC */
#undef BSL_LOG_MODULE
