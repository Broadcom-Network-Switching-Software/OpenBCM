/** \file dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_access_mapper_commandline.c
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

#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_access_mapper_diagnostic.h>
#include "dnx_field_tcam_access_mapper_commandline.h"
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_mapper_access.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
sh_sand_cmd_t sh_dnx_swstate_dnx_field_tcam_access_mapper_cmds[] = {
    {"key_2_location_hash", NULL, sh_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_cmds, NULL, &dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_man},
    {"dump", sh_dnx_swstate_dnx_field_tcam_access_mapper_dump_cmd, NULL, dnx_swstate_dnx_field_tcam_access_mapper_dump_options, &dnx_swstate_dnx_field_tcam_access_mapper_dump_man},
    {"size_get", sh_dnx_swstate_dnx_field_tcam_access_mapper_size_get_cmd, NULL, dnx_swstate_dnx_field_tcam_access_mapper_size_get_options, &dnx_swstate_dnx_field_tcam_access_mapper_size_get_man},
    {"diagnostic_operation_counters", sh_dnx_swstate_dnx_field_tcam_access_mapper_diagnostic_operation_counters_cmd, NULL, dnx_swstate_dnx_field_tcam_access_mapper_diagnostic_operation_counters_options, &dnx_swstate_dnx_field_tcam_access_mapper_diagnostic_operation_counters_man},
    {NULL}
};


shr_error_e sh_dnx_swstate_dnx_field_tcam_access_mapper_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    uint8 is_init;
    dnx_sw_state_dump_filters_t filters;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("nocontent", filters.nocontent);
    SH_SAND_GET_STR("typefilter", filters.typefilter);
    SH_SAND_GET_STR("namefilter", filters.namefilter);

    dnx_field_tcam_access_mapper_sw.is_init(unit, &is_init);
    if (is_init) {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_mapper_sw_dump(unit, filters));
        if (!filters.nocontent)
        {
            dnx_sw_state_dump_detach_file(unit);
        }
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_dnx_field_tcam_access_mapper_dump_options[] = {
    {"nocontent",      SAL_FIELD_TYPE_BOOL, "Decision if to perform content dump or to print the access tree.",  "false"},
    {"typefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable type.",         ""},
    {"namefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable name.",         ""},
    {NULL}
};


sh_sand_man_t dnx_swstate_dnx_field_tcam_access_mapper_dump_man = {
    "swstate dnx_field_tcam_access_mapper dump",
    "print the module's content",
    "swstate dnx_field_tcam_access_mapper dump",
    "swstate dnx_field_tcam_access_mapper dump nocontent=false typefilter="" namefilter="" ",
};


shr_error_e sh_dnx_swstate_dnx_field_tcam_access_mapper_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    uint32 size = 0;
    SHR_FUNC_INIT_VARS(unit);

    size = dnx_sw_state_info_size_get(dnx_field_tcam_access_mapper_sw_info[unit], dnx_field_tcam_access_mapper_sw_layout_str, DNX_FIELD_TCAM_ACCESS_MAPPER_SW_INFO_NOF_ENTRIES, "DNX_FIELD_TCAM_ACCESS_MAPPER_SW~", 0);
    dnx_sw_state_size_print(size);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_dnx_field_tcam_access_mapper_size_get_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_dnx_field_tcam_access_mapper_size_get_man = {
    "swstate dnx_field_tcam_access_mapper size_get",
    "print the module's size",
    "swstate dnx_field_tcam_access_mapper size_get",
    "swstate dnx_field_tcam_access_mapper size_get",
};


shr_error_e sh_dnx_swstate_dnx_field_tcam_access_mapper_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{


    SHR_FUNC_INIT_VARS(unit);

    dnx_sw_state_diagnostic_operation_counters_print(dnx_field_tcam_access_mapper_sw_info[unit], dnx_field_tcam_access_mapper_sw_layout_str, DNX_FIELD_TCAM_ACCESS_MAPPER_SW_INFO_NOF_ENTRIES, "DNX_FIELD_TCAM_ACCESS_MAPPER_SW~");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_dnx_field_tcam_access_mapper_diagnostic_operation_counters_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_dnx_field_tcam_access_mapper_diagnostic_operation_counters_man = {
    "swstate dnx_field_tcam_access_mapper diagnostic_operation_counters",
    "print the module's diagnostic operation counters",
    "swstate dnx_field_tcam_access_mapper diagnostic_operation_counters",
    "swstate dnx_field_tcam_access_mapper diagnostic_operation_counters",
};


sh_sand_man_t dnx_swstate_dnx_field_tcam_access_mapper_man = {
    cmd_dnx_swstate_dnx_field_tcam_access_mapper_desc,
    NULL,
    NULL,
    NULL,
};


const char cmd_dnx_swstate_dnx_field_tcam_access_mapper_desc[] = "swstate dnx_field_tcam_access_mapper commands";



sh_sand_cmd_t sh_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_cmds[] = {
    {"head", NULL, sh_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_head_cmds, NULL, &dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_head_man},
    {"next", NULL, sh_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_next_cmds, NULL, &dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_next_man},
    {"dump", sh_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_dump_cmd, NULL, dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_dump_options, &dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_dump_man},
    {"size_get", sh_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_size_get_cmd, NULL, dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_size_get_options, &dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_size_get_man},
    {"diagnostic_operation_counters", sh_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_diagnostic_operation_counters_cmd, NULL, dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_diagnostic_operation_counters_options, &dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_diagnostic_operation_counters_man},
    {NULL}
};


sh_sand_man_t dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_man = {
    cmd_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_desc,
    NULL,
    NULL,
    NULL,
};


const char cmd_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_desc[] = "dnx_field_tcam_access_mapper_sw key_2_location_hash commands";


shr_error_e sh_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{
    int  key_2_location_hash_idx_0 = 0;
    int  key_2_location_hash_idx_1 = 0;

    dnx_sw_state_dump_filters_t filters;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("key_2_location_hash_idx_0", key_2_location_hash_idx_0);
    SH_SAND_GET_INT32("key_2_location_hash_idx_1", key_2_location_hash_idx_1);

    SH_SAND_GET_BOOL("nocontent", filters.nocontent);
    SH_SAND_GET_STR("typefilter", filters.typefilter);
    SH_SAND_GET_STR("namefilter", filters.namefilter);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_mapper_sw_key_2_location_hash_dump(unit, key_2_location_hash_idx_0, key_2_location_hash_idx_1, filters));

    if (!filters.nocontent)
    {
        dnx_sw_state_dump_detach_file(unit);

    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_dump_options[] = {
    {"key_2_location_hash_idx_0", SAL_FIELD_TYPE_INT32, "index", NULL},
    {"key_2_location_hash_idx_1", SAL_FIELD_TYPE_INT32, "index", NULL},
    {"nocontent",      SAL_FIELD_TYPE_BOOL, "Decision if to perform content dump or to print the access tree.",  "false"},
    {"typefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable type.",         ""},
    {"namefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable name.",         ""},
    {NULL}
};


sh_sand_man_t dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_dump_man = {
    "swstate dump",
    "print the variable value",
    "dnx_field_tcam_access_mapper_sw key_2_location_hash dump [,key_2_location_hash_idx_0=<int>,key_2_location_hash_idx_1=<int>]\n",
    "dnx_field_tcam_access_mapper_sw key_2_location_hash dump key_2_location_hash_idx_0=0 key_2_location_hash_idx_1=0 nocontent=false typefilter="" namefilter="" ",
};


shr_error_e sh_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{
    uint32 size = 0;

    SHR_FUNC_INIT_VARS(unit);


    size = dnx_sw_state_info_size_get(dnx_field_tcam_access_mapper_sw_info[unit], dnx_field_tcam_access_mapper_sw_layout_str, DNX_FIELD_TCAM_ACCESS_MAPPER_SW_INFO_NOF_ENTRIES, "DNX_FIELD_TCAM_ACCESS_MAPPER_SW~KEY_2_LOCATION_HASH~", 0);
    dnx_sw_state_size_print(size);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_size_get_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_size_get_man = {
    "swstate size_get",
    "print the variable size",
    "dnx_field_tcam_access_mapper_sw key_2_location_hash size_get",
    "dnx_field_tcam_access_mapper_sw key_2_location_hash size_get",
};


shr_error_e sh_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    SHR_FUNC_INIT_VARS(unit);


    dnx_sw_state_diagnostic_operation_counters_print(dnx_field_tcam_access_mapper_sw_info[unit], dnx_field_tcam_access_mapper_sw_layout_str, DNX_FIELD_TCAM_ACCESS_MAPPER_SW_INFO_NOF_ENTRIES, "DNX_FIELD_TCAM_ACCESS_MAPPER_SW~KEY_2_LOCATION_HASH~");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_diagnostic_operation_counters_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_diagnostic_operation_counters_man = {
    "swstate diagnostic_operation_counters",
    "print the diagnostic operation counters",
    "dnx_field_tcam_access_mapper_sw key_2_location_hash diagnostic_operation_counters",
    "dnx_field_tcam_access_mapper_sw key_2_location_hash diagnostic_operation_counters",
};



sh_sand_cmd_t sh_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_head_cmds[] = {
    {"dump", sh_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_head_dump_cmd, NULL, dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_head_dump_options, &dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_head_dump_man},
    {"size_get", sh_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_head_size_get_cmd, NULL, dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_head_size_get_options, &dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_head_size_get_man},
    {"diagnostic_operation_counters", sh_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_head_diagnostic_operation_counters_cmd, NULL, dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_head_diagnostic_operation_counters_options, &dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_head_diagnostic_operation_counters_man},
    {NULL}
};


sh_sand_man_t dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_head_man = {
    cmd_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_head_desc,
    NULL,
    NULL,
    NULL,
};


const char cmd_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_head_desc[] = "dnx_field_tcam_access_mapper_sw key_2_location_hash head commands";


shr_error_e sh_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_head_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{
    int  key_2_location_hash_idx_0 = 0;
    int  key_2_location_hash_idx_1 = 0;
    int  head_idx_0 = 0;

    dnx_sw_state_dump_filters_t filters;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("key_2_location_hash_idx_0", key_2_location_hash_idx_0);
    SH_SAND_GET_INT32("key_2_location_hash_idx_1", key_2_location_hash_idx_1);
    SH_SAND_GET_INT32("head_idx_0", head_idx_0);

    SH_SAND_GET_BOOL("nocontent", filters.nocontent);
    SH_SAND_GET_STR("typefilter", filters.typefilter);
    SH_SAND_GET_STR("namefilter", filters.namefilter);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_mapper_sw_key_2_location_hash_head_dump(unit, key_2_location_hash_idx_0, key_2_location_hash_idx_1, head_idx_0, filters));

    if (!filters.nocontent)
    {
        dnx_sw_state_dump_detach_file(unit);

    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_head_dump_options[] = {
    {"key_2_location_hash_idx_0", SAL_FIELD_TYPE_INT32, "index", NULL},
    {"key_2_location_hash_idx_1", SAL_FIELD_TYPE_INT32, "index", NULL},
    {"head_idx_0", SAL_FIELD_TYPE_INT32, "index", NULL},
    {"nocontent",      SAL_FIELD_TYPE_BOOL, "Decision if to perform content dump or to print the access tree.",  "false"},
    {"typefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable type.",         ""},
    {"namefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable name.",         ""},
    {NULL}
};


sh_sand_man_t dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_head_dump_man = {
    "swstate dump",
    "print the variable value",
    "dnx_field_tcam_access_mapper_sw key_2_location_hash head dump [,key_2_location_hash_idx_0=<int>,key_2_location_hash_idx_1=<int>,head_idx_0=<int>]\n",
    "dnx_field_tcam_access_mapper_sw key_2_location_hash head dump key_2_location_hash_idx_0=0 key_2_location_hash_idx_1=0 head_idx_0=0 nocontent=false typefilter="" namefilter="" ",
};


shr_error_e sh_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_head_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{
    uint32 size = 0;

    SHR_FUNC_INIT_VARS(unit);


    size = dnx_sw_state_info_size_get(dnx_field_tcam_access_mapper_sw_info[unit], dnx_field_tcam_access_mapper_sw_layout_str, DNX_FIELD_TCAM_ACCESS_MAPPER_SW_INFO_NOF_ENTRIES, "DNX_FIELD_TCAM_ACCESS_MAPPER_SW~KEY_2_LOCATION_HASH~HEAD~", 0);
    dnx_sw_state_size_print(size);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_head_size_get_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_head_size_get_man = {
    "swstate size_get",
    "print the variable size",
    "dnx_field_tcam_access_mapper_sw key_2_location_hash head size_get",
    "dnx_field_tcam_access_mapper_sw key_2_location_hash head size_get",
};


shr_error_e sh_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_head_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    SHR_FUNC_INIT_VARS(unit);


    dnx_sw_state_diagnostic_operation_counters_print(dnx_field_tcam_access_mapper_sw_info[unit], dnx_field_tcam_access_mapper_sw_layout_str, DNX_FIELD_TCAM_ACCESS_MAPPER_SW_INFO_NOF_ENTRIES, "DNX_FIELD_TCAM_ACCESS_MAPPER_SW~KEY_2_LOCATION_HASH~HEAD~");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_head_diagnostic_operation_counters_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_head_diagnostic_operation_counters_man = {
    "swstate diagnostic_operation_counters",
    "print the diagnostic operation counters",
    "dnx_field_tcam_access_mapper_sw key_2_location_hash head diagnostic_operation_counters",
    "dnx_field_tcam_access_mapper_sw key_2_location_hash head diagnostic_operation_counters",
};



sh_sand_cmd_t sh_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_next_cmds[] = {
    {"dump", sh_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_next_dump_cmd, NULL, dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_next_dump_options, &dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_next_dump_man},
    {"size_get", sh_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_next_size_get_cmd, NULL, dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_next_size_get_options, &dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_next_size_get_man},
    {"diagnostic_operation_counters", sh_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_next_diagnostic_operation_counters_cmd, NULL, dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_next_diagnostic_operation_counters_options, &dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_next_diagnostic_operation_counters_man},
    {NULL}
};


sh_sand_man_t dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_next_man = {
    cmd_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_next_desc,
    NULL,
    NULL,
    NULL,
};


const char cmd_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_next_desc[] = "dnx_field_tcam_access_mapper_sw key_2_location_hash next commands";


shr_error_e sh_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_next_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{
    int  key_2_location_hash_idx_0 = 0;
    int  key_2_location_hash_idx_1 = 0;
    int  next_idx_0 = 0;

    dnx_sw_state_dump_filters_t filters;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("key_2_location_hash_idx_0", key_2_location_hash_idx_0);
    SH_SAND_GET_INT32("key_2_location_hash_idx_1", key_2_location_hash_idx_1);
    SH_SAND_GET_INT32("next_idx_0", next_idx_0);

    SH_SAND_GET_BOOL("nocontent", filters.nocontent);
    SH_SAND_GET_STR("typefilter", filters.typefilter);
    SH_SAND_GET_STR("namefilter", filters.namefilter);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_mapper_sw_key_2_location_hash_next_dump(unit, key_2_location_hash_idx_0, key_2_location_hash_idx_1, next_idx_0, filters));

    if (!filters.nocontent)
    {
        dnx_sw_state_dump_detach_file(unit);

    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_next_dump_options[] = {
    {"key_2_location_hash_idx_0", SAL_FIELD_TYPE_INT32, "index", NULL},
    {"key_2_location_hash_idx_1", SAL_FIELD_TYPE_INT32, "index", NULL},
    {"next_idx_0", SAL_FIELD_TYPE_INT32, "index", NULL},
    {"nocontent",      SAL_FIELD_TYPE_BOOL, "Decision if to perform content dump or to print the access tree.",  "false"},
    {"typefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable type.",         ""},
    {"namefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable name.",         ""},
    {NULL}
};


sh_sand_man_t dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_next_dump_man = {
    "swstate dump",
    "print the variable value",
    "dnx_field_tcam_access_mapper_sw key_2_location_hash next dump [,key_2_location_hash_idx_0=<int>,key_2_location_hash_idx_1=<int>,next_idx_0=<int>]\n",
    "dnx_field_tcam_access_mapper_sw key_2_location_hash next dump key_2_location_hash_idx_0=0 key_2_location_hash_idx_1=0 next_idx_0=0 nocontent=false typefilter="" namefilter="" ",
};


shr_error_e sh_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_next_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{
    uint32 size = 0;

    SHR_FUNC_INIT_VARS(unit);


    size = dnx_sw_state_info_size_get(dnx_field_tcam_access_mapper_sw_info[unit], dnx_field_tcam_access_mapper_sw_layout_str, DNX_FIELD_TCAM_ACCESS_MAPPER_SW_INFO_NOF_ENTRIES, "DNX_FIELD_TCAM_ACCESS_MAPPER_SW~KEY_2_LOCATION_HASH~NEXT~", 0);
    dnx_sw_state_size_print(size);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_next_size_get_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_next_size_get_man = {
    "swstate size_get",
    "print the variable size",
    "dnx_field_tcam_access_mapper_sw key_2_location_hash next size_get",
    "dnx_field_tcam_access_mapper_sw key_2_location_hash next size_get",
};


shr_error_e sh_dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_next_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    SHR_FUNC_INIT_VARS(unit);


    dnx_sw_state_diagnostic_operation_counters_print(dnx_field_tcam_access_mapper_sw_info[unit], dnx_field_tcam_access_mapper_sw_layout_str, DNX_FIELD_TCAM_ACCESS_MAPPER_SW_INFO_NOF_ENTRIES, "DNX_FIELD_TCAM_ACCESS_MAPPER_SW~KEY_2_LOCATION_HASH~NEXT~");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_next_diagnostic_operation_counters_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_dnx_field_tcam_access_mapper_key_2_location_hash_next_diagnostic_operation_counters_man = {
    "swstate diagnostic_operation_counters",
    "print the diagnostic operation counters",
    "dnx_field_tcam_access_mapper_sw key_2_location_hash next diagnostic_operation_counters",
    "dnx_field_tcam_access_mapper_sw key_2_location_hash next diagnostic_operation_counters",
};


#endif /* DNX_SW_STATE_DIAGNOSTIC */
#undef BSL_LOG_MODULE
