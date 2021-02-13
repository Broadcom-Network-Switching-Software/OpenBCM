/** \file dnx/swstate/auto_generated/diagnostic/algo_ecgm_commandline.c
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

#include <bcm_int/dnx/algo/swstate/auto_generated/diagnostic/algo_ecgm_diagnostic.h>
#include "algo_ecgm_commandline.h"
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_ecgm_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
sh_sand_cmd_t sh_dnx_swstate_algo_ecgm_cmds[] = {
    {"ecgm_port_profile", NULL, sh_dnx_swstate_algo_ecgm_ecgm_port_profile_cmds, NULL, &dnx_swstate_algo_ecgm_ecgm_port_profile_man},
    {"ecgm_interface_profile", NULL, sh_dnx_swstate_algo_ecgm_ecgm_interface_profile_cmds, NULL, &dnx_swstate_algo_ecgm_ecgm_interface_profile_man},
    {"dump", sh_dnx_swstate_algo_ecgm_dump_cmd, NULL, dnx_swstate_algo_ecgm_dump_options, &dnx_swstate_algo_ecgm_dump_man},
    {"size_get", sh_dnx_swstate_algo_ecgm_size_get_cmd, NULL, dnx_swstate_algo_ecgm_size_get_options, &dnx_swstate_algo_ecgm_size_get_man},
    {"diagnostic_operation_counters", sh_dnx_swstate_algo_ecgm_diagnostic_operation_counters_cmd, NULL, dnx_swstate_algo_ecgm_diagnostic_operation_counters_options, &dnx_swstate_algo_ecgm_diagnostic_operation_counters_man},
    {NULL}
};


shr_error_e sh_dnx_swstate_algo_ecgm_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    uint8 is_init;
    dnx_sw_state_dump_filters_t filters;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("nocontent", filters.nocontent);
    SH_SAND_GET_STR("typefilter", filters.typefilter);
    SH_SAND_GET_STR("namefilter", filters.namefilter);

    algo_ecgm_db.is_init(unit, &is_init);
    if (is_init) {
        SHR_IF_ERR_EXIT(algo_ecgm_db_dump(unit, filters));
        if (!filters.nocontent)
        {
            dnx_sw_state_dump_detach_file(unit);
        }
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_ecgm_dump_options[] = {
    {"nocontent",      SAL_FIELD_TYPE_BOOL, "Decision if to perform content dump or to print the access tree.",  "false"},
    {"typefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable type.",         ""},
    {"namefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable name.",         ""},
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_ecgm_dump_man = {
    "swstate algo_ecgm dump",
    "print the module's content",
    "swstate algo_ecgm dump",
    "swstate algo_ecgm dump nocontent=false typefilter="" namefilter="" ",
};


shr_error_e sh_dnx_swstate_algo_ecgm_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    uint32 size = 0;
    SHR_FUNC_INIT_VARS(unit);

    size = dnx_sw_state_info_size_get(algo_ecgm_db_info[unit], algo_ecgm_db_layout_str, ALGO_ECGM_DB_INFO_NOF_ENTRIES, "ALGO_ECGM_DB~", 0);
    dnx_sw_state_size_print(size);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_ecgm_size_get_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_ecgm_size_get_man = {
    "swstate algo_ecgm size_get",
    "print the module's size",
    "swstate algo_ecgm size_get",
    "swstate algo_ecgm size_get",
};


shr_error_e sh_dnx_swstate_algo_ecgm_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{


    SHR_FUNC_INIT_VARS(unit);

    dnx_sw_state_diagnostic_operation_counters_print(algo_ecgm_db_info[unit], algo_ecgm_db_layout_str, ALGO_ECGM_DB_INFO_NOF_ENTRIES, "ALGO_ECGM_DB~");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_ecgm_diagnostic_operation_counters_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_ecgm_diagnostic_operation_counters_man = {
    "swstate algo_ecgm diagnostic_operation_counters",
    "print the module's diagnostic operation counters",
    "swstate algo_ecgm diagnostic_operation_counters",
    "swstate algo_ecgm diagnostic_operation_counters",
};


sh_sand_man_t dnx_swstate_algo_ecgm_man = {
    cmd_dnx_swstate_algo_ecgm_desc,
    NULL,
    NULL,
    NULL,
};


const char cmd_dnx_swstate_algo_ecgm_desc[] = "swstate algo_ecgm commands";



sh_sand_cmd_t sh_dnx_swstate_algo_ecgm_ecgm_port_profile_cmds[] = {
    {"dump", sh_dnx_swstate_algo_ecgm_ecgm_port_profile_dump_cmd, NULL, dnx_swstate_algo_ecgm_ecgm_port_profile_dump_options, &dnx_swstate_algo_ecgm_ecgm_port_profile_dump_man},
    {"size_get", sh_dnx_swstate_algo_ecgm_ecgm_port_profile_size_get_cmd, NULL, dnx_swstate_algo_ecgm_ecgm_port_profile_size_get_options, &dnx_swstate_algo_ecgm_ecgm_port_profile_size_get_man},
    {"diagnostic_operation_counters", sh_dnx_swstate_algo_ecgm_ecgm_port_profile_diagnostic_operation_counters_cmd, NULL, dnx_swstate_algo_ecgm_ecgm_port_profile_diagnostic_operation_counters_options, &dnx_swstate_algo_ecgm_ecgm_port_profile_diagnostic_operation_counters_man},
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_ecgm_ecgm_port_profile_man = {
    cmd_dnx_swstate_algo_ecgm_ecgm_port_profile_desc,
    NULL,
    NULL,
    NULL,
};


const char cmd_dnx_swstate_algo_ecgm_ecgm_port_profile_desc[] = "algo_ecgm_db ecgm_port_profile commands";


shr_error_e sh_dnx_swstate_algo_ecgm_ecgm_port_profile_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{
    int  ecgm_port_profile_idx_0 = 0;

    dnx_sw_state_dump_filters_t filters;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("ecgm_port_profile_idx_0", ecgm_port_profile_idx_0);

    SH_SAND_GET_BOOL("nocontent", filters.nocontent);
    SH_SAND_GET_STR("typefilter", filters.typefilter);
    SH_SAND_GET_STR("namefilter", filters.namefilter);

    SHR_IF_ERR_EXIT(algo_ecgm_db_ecgm_port_profile_dump(unit, ecgm_port_profile_idx_0, filters));

    if (!filters.nocontent)
    {
        dnx_sw_state_dump_detach_file(unit);

    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_ecgm_ecgm_port_profile_dump_options[] = {
    {"ecgm_port_profile_idx_0", SAL_FIELD_TYPE_INT32, "index", NULL},
    {"nocontent",      SAL_FIELD_TYPE_BOOL, "Decision if to perform content dump or to print the access tree.",  "false"},
    {"typefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable type.",         ""},
    {"namefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable name.",         ""},
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_ecgm_ecgm_port_profile_dump_man = {
    "swstate dump",
    "print the variable value",
    "algo_ecgm_db ecgm_port_profile dump [,ecgm_port_profile_idx_0=<int>]\n",
    "algo_ecgm_db ecgm_port_profile dump ecgm_port_profile_idx_0=0 nocontent=false typefilter="" namefilter="" ",
};


shr_error_e sh_dnx_swstate_algo_ecgm_ecgm_port_profile_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{
    uint32 size = 0;

    SHR_FUNC_INIT_VARS(unit);


    size = dnx_sw_state_info_size_get(algo_ecgm_db_info[unit], algo_ecgm_db_layout_str, ALGO_ECGM_DB_INFO_NOF_ENTRIES, "ALGO_ECGM_DB~ECGM_PORT_PROFILE~", 0);
    dnx_sw_state_size_print(size);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_ecgm_ecgm_port_profile_size_get_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_ecgm_ecgm_port_profile_size_get_man = {
    "swstate size_get",
    "print the variable size",
    "algo_ecgm_db ecgm_port_profile size_get",
    "algo_ecgm_db ecgm_port_profile size_get",
};


shr_error_e sh_dnx_swstate_algo_ecgm_ecgm_port_profile_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    SHR_FUNC_INIT_VARS(unit);


    dnx_sw_state_diagnostic_operation_counters_print(algo_ecgm_db_info[unit], algo_ecgm_db_layout_str, ALGO_ECGM_DB_INFO_NOF_ENTRIES, "ALGO_ECGM_DB~ECGM_PORT_PROFILE~");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_ecgm_ecgm_port_profile_diagnostic_operation_counters_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_ecgm_ecgm_port_profile_diagnostic_operation_counters_man = {
    "swstate diagnostic_operation_counters",
    "print the diagnostic operation counters",
    "algo_ecgm_db ecgm_port_profile diagnostic_operation_counters",
    "algo_ecgm_db ecgm_port_profile diagnostic_operation_counters",
};



sh_sand_cmd_t sh_dnx_swstate_algo_ecgm_ecgm_interface_profile_cmds[] = {
    {"dump", sh_dnx_swstate_algo_ecgm_ecgm_interface_profile_dump_cmd, NULL, dnx_swstate_algo_ecgm_ecgm_interface_profile_dump_options, &dnx_swstate_algo_ecgm_ecgm_interface_profile_dump_man},
    {"size_get", sh_dnx_swstate_algo_ecgm_ecgm_interface_profile_size_get_cmd, NULL, dnx_swstate_algo_ecgm_ecgm_interface_profile_size_get_options, &dnx_swstate_algo_ecgm_ecgm_interface_profile_size_get_man},
    {"diagnostic_operation_counters", sh_dnx_swstate_algo_ecgm_ecgm_interface_profile_diagnostic_operation_counters_cmd, NULL, dnx_swstate_algo_ecgm_ecgm_interface_profile_diagnostic_operation_counters_options, &dnx_swstate_algo_ecgm_ecgm_interface_profile_diagnostic_operation_counters_man},
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_ecgm_ecgm_interface_profile_man = {
    cmd_dnx_swstate_algo_ecgm_ecgm_interface_profile_desc,
    NULL,
    NULL,
    NULL,
};


const char cmd_dnx_swstate_algo_ecgm_ecgm_interface_profile_desc[] = "algo_ecgm_db ecgm_interface_profile commands";


shr_error_e sh_dnx_swstate_algo_ecgm_ecgm_interface_profile_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{
    int  ecgm_interface_profile_idx_0 = 0;

    dnx_sw_state_dump_filters_t filters;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("ecgm_interface_profile_idx_0", ecgm_interface_profile_idx_0);

    SH_SAND_GET_BOOL("nocontent", filters.nocontent);
    SH_SAND_GET_STR("typefilter", filters.typefilter);
    SH_SAND_GET_STR("namefilter", filters.namefilter);

    SHR_IF_ERR_EXIT(algo_ecgm_db_ecgm_interface_profile_dump(unit, ecgm_interface_profile_idx_0, filters));

    if (!filters.nocontent)
    {
        dnx_sw_state_dump_detach_file(unit);

    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_ecgm_ecgm_interface_profile_dump_options[] = {
    {"ecgm_interface_profile_idx_0", SAL_FIELD_TYPE_INT32, "index", NULL},
    {"nocontent",      SAL_FIELD_TYPE_BOOL, "Decision if to perform content dump or to print the access tree.",  "false"},
    {"typefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable type.",         ""},
    {"namefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable name.",         ""},
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_ecgm_ecgm_interface_profile_dump_man = {
    "swstate dump",
    "print the variable value",
    "algo_ecgm_db ecgm_interface_profile dump [,ecgm_interface_profile_idx_0=<int>]\n",
    "algo_ecgm_db ecgm_interface_profile dump ecgm_interface_profile_idx_0=0 nocontent=false typefilter="" namefilter="" ",
};


shr_error_e sh_dnx_swstate_algo_ecgm_ecgm_interface_profile_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{
    uint32 size = 0;

    SHR_FUNC_INIT_VARS(unit);


    size = dnx_sw_state_info_size_get(algo_ecgm_db_info[unit], algo_ecgm_db_layout_str, ALGO_ECGM_DB_INFO_NOF_ENTRIES, "ALGO_ECGM_DB~ECGM_INTERFACE_PROFILE~", 0);
    dnx_sw_state_size_print(size);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_ecgm_ecgm_interface_profile_size_get_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_ecgm_ecgm_interface_profile_size_get_man = {
    "swstate size_get",
    "print the variable size",
    "algo_ecgm_db ecgm_interface_profile size_get",
    "algo_ecgm_db ecgm_interface_profile size_get",
};


shr_error_e sh_dnx_swstate_algo_ecgm_ecgm_interface_profile_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    SHR_FUNC_INIT_VARS(unit);


    dnx_sw_state_diagnostic_operation_counters_print(algo_ecgm_db_info[unit], algo_ecgm_db_layout_str, ALGO_ECGM_DB_INFO_NOF_ENTRIES, "ALGO_ECGM_DB~ECGM_INTERFACE_PROFILE~");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_ecgm_ecgm_interface_profile_diagnostic_operation_counters_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_ecgm_ecgm_interface_profile_diagnostic_operation_counters_man = {
    "swstate diagnostic_operation_counters",
    "print the diagnostic operation counters",
    "algo_ecgm_db ecgm_interface_profile diagnostic_operation_counters",
    "algo_ecgm_db ecgm_interface_profile diagnostic_operation_counters",
};


#endif /* DNX_SW_STATE_DIAGNOSTIC */
#undef BSL_LOG_MODULE
