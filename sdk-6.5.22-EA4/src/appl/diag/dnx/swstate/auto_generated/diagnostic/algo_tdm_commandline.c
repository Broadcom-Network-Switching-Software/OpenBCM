/** \file dnx/swstate/auto_generated/diagnostic/algo_tdm_commandline.c
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

#include <bcm_int/dnx/algo/swstate/auto_generated/diagnostic/algo_tdm_diagnostic.h>
#include "algo_tdm_commandline.h"
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_tdm_access.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
sh_sand_cmd_t sh_dnx_swstate_algo_tdm_cmds[] = {
    {"ingress_context", NULL, sh_dnx_swstate_algo_tdm_ingress_context_cmds, NULL, &dnx_swstate_algo_tdm_ingress_context_man},
    {"link_mask", NULL, sh_dnx_swstate_algo_tdm_link_mask_cmds, NULL, &dnx_swstate_algo_tdm_link_mask_man},
    {"dump", sh_dnx_swstate_algo_tdm_dump_cmd, NULL, dnx_swstate_algo_tdm_dump_options, &dnx_swstate_algo_tdm_dump_man},
    {"size_get", sh_dnx_swstate_algo_tdm_size_get_cmd, NULL, dnx_swstate_algo_tdm_size_get_options, &dnx_swstate_algo_tdm_size_get_man},
    {"diagnostic_operation_counters", sh_dnx_swstate_algo_tdm_diagnostic_operation_counters_cmd, NULL, dnx_swstate_algo_tdm_diagnostic_operation_counters_options, &dnx_swstate_algo_tdm_diagnostic_operation_counters_man},
    {NULL}
};


shr_error_e sh_dnx_swstate_algo_tdm_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    uint8 is_init;
    dnx_sw_state_dump_filters_t filters;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("nocontent", filters.nocontent);
    SH_SAND_GET_STR("typefilter", filters.typefilter);
    SH_SAND_GET_STR("namefilter", filters.namefilter);

    algo_tdm_db.is_init(unit, &is_init);
    if (is_init) {
        SHR_IF_ERR_EXIT(algo_tdm_db_dump(unit, filters));
        if (!filters.nocontent)
        {
            dnx_sw_state_dump_detach_file(unit);
        }
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_tdm_dump_options[] = {
    {"nocontent",      SAL_FIELD_TYPE_BOOL, "Decision if to perform content dump or to print the access tree.",  "false"},
    {"typefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable type.",         ""},
    {"namefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable name.",         ""},
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_tdm_dump_man = {
    "swstate algo_tdm dump",
    "print the module's content",
    "swstate algo_tdm dump",
    "swstate algo_tdm dump nocontent=false typefilter="" namefilter="" ",
};


shr_error_e sh_dnx_swstate_algo_tdm_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    uint32 size = 0;
    SHR_FUNC_INIT_VARS(unit);

    size = dnx_sw_state_info_size_get(algo_tdm_db_info[unit], algo_tdm_db_layout_str, ALGO_TDM_DB_INFO_NOF_ENTRIES, "ALGO_TDM_DB~", 0);
    dnx_sw_state_size_print(size);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_tdm_size_get_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_tdm_size_get_man = {
    "swstate algo_tdm size_get",
    "print the module's size",
    "swstate algo_tdm size_get",
    "swstate algo_tdm size_get",
};


shr_error_e sh_dnx_swstate_algo_tdm_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{


    SHR_FUNC_INIT_VARS(unit);

    dnx_sw_state_diagnostic_operation_counters_print(algo_tdm_db_info[unit], algo_tdm_db_layout_str, ALGO_TDM_DB_INFO_NOF_ENTRIES, "ALGO_TDM_DB~");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_tdm_diagnostic_operation_counters_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_tdm_diagnostic_operation_counters_man = {
    "swstate algo_tdm diagnostic_operation_counters",
    "print the module's diagnostic operation counters",
    "swstate algo_tdm diagnostic_operation_counters",
    "swstate algo_tdm diagnostic_operation_counters",
};


sh_sand_man_t dnx_swstate_algo_tdm_man = {
    cmd_dnx_swstate_algo_tdm_desc,
    NULL,
    NULL,
    NULL,
};


const char cmd_dnx_swstate_algo_tdm_desc[] = "swstate algo_tdm commands";



sh_sand_cmd_t sh_dnx_swstate_algo_tdm_ingress_context_cmds[] = {
    {"dump", sh_dnx_swstate_algo_tdm_ingress_context_dump_cmd, NULL, dnx_swstate_algo_tdm_ingress_context_dump_options, &dnx_swstate_algo_tdm_ingress_context_dump_man},
    {"size_get", sh_dnx_swstate_algo_tdm_ingress_context_size_get_cmd, NULL, dnx_swstate_algo_tdm_ingress_context_size_get_options, &dnx_swstate_algo_tdm_ingress_context_size_get_man},
    {"diagnostic_operation_counters", sh_dnx_swstate_algo_tdm_ingress_context_diagnostic_operation_counters_cmd, NULL, dnx_swstate_algo_tdm_ingress_context_diagnostic_operation_counters_options, &dnx_swstate_algo_tdm_ingress_context_diagnostic_operation_counters_man},
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_tdm_ingress_context_man = {
    cmd_dnx_swstate_algo_tdm_ingress_context_desc,
    NULL,
    NULL,
    NULL,
};


const char cmd_dnx_swstate_algo_tdm_ingress_context_desc[] = "algo_tdm_db ingress_context commands";


shr_error_e sh_dnx_swstate_algo_tdm_ingress_context_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    dnx_sw_state_dump_filters_t filters;

    SHR_FUNC_INIT_VARS(unit);


    SH_SAND_GET_BOOL("nocontent", filters.nocontent);
    SH_SAND_GET_STR("typefilter", filters.typefilter);
    SH_SAND_GET_STR("namefilter", filters.namefilter);

    SHR_IF_ERR_EXIT(algo_tdm_db_ingress_context_dump(unit, filters));

    if (!filters.nocontent)
    {
        dnx_sw_state_dump_detach_file(unit);

    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_tdm_ingress_context_dump_options[] = {
    {"nocontent",      SAL_FIELD_TYPE_BOOL, "Decision if to perform content dump or to print the access tree.",  "false"},
    {"typefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable type.",         ""},
    {"namefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable name.",         ""},
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_tdm_ingress_context_dump_man = {
    "swstate dump",
    "print the variable value",
    "algo_tdm_db ingress_context dump []\n",
    "algo_tdm_db ingress_context dump nocontent=false typefilter="" namefilter="" ",
};


shr_error_e sh_dnx_swstate_algo_tdm_ingress_context_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{
    uint32 size = 0;

    SHR_FUNC_INIT_VARS(unit);


    size = dnx_sw_state_info_size_get(algo_tdm_db_info[unit], algo_tdm_db_layout_str, ALGO_TDM_DB_INFO_NOF_ENTRIES, "ALGO_TDM_DB~INGRESS_CONTEXT~", 0);
    dnx_sw_state_size_print(size);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_tdm_ingress_context_size_get_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_tdm_ingress_context_size_get_man = {
    "swstate size_get",
    "print the variable size",
    "algo_tdm_db ingress_context size_get",
    "algo_tdm_db ingress_context size_get",
};


shr_error_e sh_dnx_swstate_algo_tdm_ingress_context_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    SHR_FUNC_INIT_VARS(unit);


    dnx_sw_state_diagnostic_operation_counters_print(algo_tdm_db_info[unit], algo_tdm_db_layout_str, ALGO_TDM_DB_INFO_NOF_ENTRIES, "ALGO_TDM_DB~INGRESS_CONTEXT~");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_tdm_ingress_context_diagnostic_operation_counters_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_tdm_ingress_context_diagnostic_operation_counters_man = {
    "swstate diagnostic_operation_counters",
    "print the diagnostic operation counters",
    "algo_tdm_db ingress_context diagnostic_operation_counters",
    "algo_tdm_db ingress_context diagnostic_operation_counters",
};



sh_sand_cmd_t sh_dnx_swstate_algo_tdm_link_mask_cmds[] = {
    {"dump", sh_dnx_swstate_algo_tdm_link_mask_dump_cmd, NULL, dnx_swstate_algo_tdm_link_mask_dump_options, &dnx_swstate_algo_tdm_link_mask_dump_man},
    {"size_get", sh_dnx_swstate_algo_tdm_link_mask_size_get_cmd, NULL, dnx_swstate_algo_tdm_link_mask_size_get_options, &dnx_swstate_algo_tdm_link_mask_size_get_man},
    {"diagnostic_operation_counters", sh_dnx_swstate_algo_tdm_link_mask_diagnostic_operation_counters_cmd, NULL, dnx_swstate_algo_tdm_link_mask_diagnostic_operation_counters_options, &dnx_swstate_algo_tdm_link_mask_diagnostic_operation_counters_man},
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_tdm_link_mask_man = {
    cmd_dnx_swstate_algo_tdm_link_mask_desc,
    NULL,
    NULL,
    NULL,
};


const char cmd_dnx_swstate_algo_tdm_link_mask_desc[] = "algo_tdm_db link_mask commands";


shr_error_e sh_dnx_swstate_algo_tdm_link_mask_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    dnx_sw_state_dump_filters_t filters;

    SHR_FUNC_INIT_VARS(unit);


    SH_SAND_GET_BOOL("nocontent", filters.nocontent);
    SH_SAND_GET_STR("typefilter", filters.typefilter);
    SH_SAND_GET_STR("namefilter", filters.namefilter);

    SHR_IF_ERR_EXIT(algo_tdm_db_link_mask_dump(unit, filters));

    if (!filters.nocontent)
    {
        dnx_sw_state_dump_detach_file(unit);

    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_tdm_link_mask_dump_options[] = {
    {"nocontent",      SAL_FIELD_TYPE_BOOL, "Decision if to perform content dump or to print the access tree.",  "false"},
    {"typefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable type.",         ""},
    {"namefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable name.",         ""},
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_tdm_link_mask_dump_man = {
    "swstate dump",
    "print the variable value",
    "algo_tdm_db link_mask dump []\n",
    "algo_tdm_db link_mask dump nocontent=false typefilter="" namefilter="" ",
};


shr_error_e sh_dnx_swstate_algo_tdm_link_mask_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{
    uint32 size = 0;

    SHR_FUNC_INIT_VARS(unit);


    size = dnx_sw_state_info_size_get(algo_tdm_db_info[unit], algo_tdm_db_layout_str, ALGO_TDM_DB_INFO_NOF_ENTRIES, "ALGO_TDM_DB~LINK_MASK~", 0);
    dnx_sw_state_size_print(size);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_tdm_link_mask_size_get_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_tdm_link_mask_size_get_man = {
    "swstate size_get",
    "print the variable size",
    "algo_tdm_db link_mask size_get",
    "algo_tdm_db link_mask size_get",
};


shr_error_e sh_dnx_swstate_algo_tdm_link_mask_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    SHR_FUNC_INIT_VARS(unit);


    dnx_sw_state_diagnostic_operation_counters_print(algo_tdm_db_info[unit], algo_tdm_db_layout_str, ALGO_TDM_DB_INFO_NOF_ENTRIES, "ALGO_TDM_DB~LINK_MASK~");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_tdm_link_mask_diagnostic_operation_counters_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_tdm_link_mask_diagnostic_operation_counters_man = {
    "swstate diagnostic_operation_counters",
    "print the diagnostic operation counters",
    "algo_tdm_db link_mask diagnostic_operation_counters",
    "algo_tdm_db link_mask diagnostic_operation_counters",
};


#endif /* DNX_SW_STATE_DIAGNOSTIC */
#undef BSL_LOG_MODULE
