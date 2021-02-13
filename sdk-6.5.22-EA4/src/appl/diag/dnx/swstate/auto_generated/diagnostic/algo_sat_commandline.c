/** \file dnx/swstate/auto_generated/diagnostic/algo_sat_commandline.c
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

#include <bcm_int/dnx/algo/swstate/auto_generated/diagnostic/algo_sat_diagnostic.h>
#include "algo_sat_commandline.h"
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_sat_access.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
sh_sand_cmd_t sh_dnx_swstate_algo_sat_cmds[] = {
    {"sat_res_alloc_gtf_id", NULL, sh_dnx_swstate_algo_sat_sat_res_alloc_gtf_id_cmds, NULL, &dnx_swstate_algo_sat_sat_res_alloc_gtf_id_man},
    {"sat_res_alloc_ctf_id", NULL, sh_dnx_swstate_algo_sat_sat_res_alloc_ctf_id_cmds, NULL, &dnx_swstate_algo_sat_sat_res_alloc_ctf_id_man},
    {"sat_res_alloc_trap_id", NULL, sh_dnx_swstate_algo_sat_sat_res_alloc_trap_id_cmds, NULL, &dnx_swstate_algo_sat_sat_res_alloc_trap_id_man},
    {"sat_res_alloc_trap_data", NULL, sh_dnx_swstate_algo_sat_sat_res_alloc_trap_data_cmds, NULL, &dnx_swstate_algo_sat_sat_res_alloc_trap_data_man},
    {"sat_pkt_header", NULL, sh_dnx_swstate_algo_sat_sat_pkt_header_cmds, NULL, &dnx_swstate_algo_sat_sat_pkt_header_man},
    {"dump", sh_dnx_swstate_algo_sat_dump_cmd, NULL, dnx_swstate_algo_sat_dump_options, &dnx_swstate_algo_sat_dump_man},
    {"size_get", sh_dnx_swstate_algo_sat_size_get_cmd, NULL, dnx_swstate_algo_sat_size_get_options, &dnx_swstate_algo_sat_size_get_man},
    {"diagnostic_operation_counters", sh_dnx_swstate_algo_sat_diagnostic_operation_counters_cmd, NULL, dnx_swstate_algo_sat_diagnostic_operation_counters_options, &dnx_swstate_algo_sat_diagnostic_operation_counters_man},
    {NULL}
};


shr_error_e sh_dnx_swstate_algo_sat_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    uint8 is_init;
    dnx_sw_state_dump_filters_t filters;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("nocontent", filters.nocontent);
    SH_SAND_GET_STR("typefilter", filters.typefilter);
    SH_SAND_GET_STR("namefilter", filters.namefilter);

    algo_sat_db.is_init(unit, &is_init);
    if (is_init) {
        SHR_IF_ERR_EXIT(algo_sat_db_dump(unit, filters));
        if (!filters.nocontent)
        {
            dnx_sw_state_dump_detach_file(unit);
        }
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_sat_dump_options[] = {
    {"nocontent",      SAL_FIELD_TYPE_BOOL, "Decision if to perform content dump or to print the access tree.",  "false"},
    {"typefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable type.",         ""},
    {"namefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable name.",         ""},
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_sat_dump_man = {
    "swstate algo_sat dump",
    "print the module's content",
    "swstate algo_sat dump",
    "swstate algo_sat dump nocontent=false typefilter="" namefilter="" ",
};


shr_error_e sh_dnx_swstate_algo_sat_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    uint32 size = 0;
    SHR_FUNC_INIT_VARS(unit);

    size = dnx_sw_state_info_size_get(algo_sat_db_info[unit], algo_sat_db_layout_str, ALGO_SAT_DB_INFO_NOF_ENTRIES, "ALGO_SAT_DB~", 0);
    dnx_sw_state_size_print(size);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_sat_size_get_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_sat_size_get_man = {
    "swstate algo_sat size_get",
    "print the module's size",
    "swstate algo_sat size_get",
    "swstate algo_sat size_get",
};


shr_error_e sh_dnx_swstate_algo_sat_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{


    SHR_FUNC_INIT_VARS(unit);

    dnx_sw_state_diagnostic_operation_counters_print(algo_sat_db_info[unit], algo_sat_db_layout_str, ALGO_SAT_DB_INFO_NOF_ENTRIES, "ALGO_SAT_DB~");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_sat_diagnostic_operation_counters_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_sat_diagnostic_operation_counters_man = {
    "swstate algo_sat diagnostic_operation_counters",
    "print the module's diagnostic operation counters",
    "swstate algo_sat diagnostic_operation_counters",
    "swstate algo_sat diagnostic_operation_counters",
};


sh_sand_man_t dnx_swstate_algo_sat_man = {
    cmd_dnx_swstate_algo_sat_desc,
    NULL,
    NULL,
    NULL,
};


const char cmd_dnx_swstate_algo_sat_desc[] = "swstate algo_sat commands";



sh_sand_cmd_t sh_dnx_swstate_algo_sat_sat_res_alloc_gtf_id_cmds[] = {
    {"dump", sh_dnx_swstate_algo_sat_sat_res_alloc_gtf_id_dump_cmd, NULL, dnx_swstate_algo_sat_sat_res_alloc_gtf_id_dump_options, &dnx_swstate_algo_sat_sat_res_alloc_gtf_id_dump_man},
    {"size_get", sh_dnx_swstate_algo_sat_sat_res_alloc_gtf_id_size_get_cmd, NULL, dnx_swstate_algo_sat_sat_res_alloc_gtf_id_size_get_options, &dnx_swstate_algo_sat_sat_res_alloc_gtf_id_size_get_man},
    {"diagnostic_operation_counters", sh_dnx_swstate_algo_sat_sat_res_alloc_gtf_id_diagnostic_operation_counters_cmd, NULL, dnx_swstate_algo_sat_sat_res_alloc_gtf_id_diagnostic_operation_counters_options, &dnx_swstate_algo_sat_sat_res_alloc_gtf_id_diagnostic_operation_counters_man},
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_gtf_id_man = {
    cmd_dnx_swstate_algo_sat_sat_res_alloc_gtf_id_desc,
    NULL,
    NULL,
    NULL,
};


const char cmd_dnx_swstate_algo_sat_sat_res_alloc_gtf_id_desc[] = "algo_sat_db sat_res_alloc_gtf_id commands";


shr_error_e sh_dnx_swstate_algo_sat_sat_res_alloc_gtf_id_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    dnx_sw_state_dump_filters_t filters;

    SHR_FUNC_INIT_VARS(unit);


    SH_SAND_GET_BOOL("nocontent", filters.nocontent);
    SH_SAND_GET_STR("typefilter", filters.typefilter);
    SH_SAND_GET_STR("namefilter", filters.namefilter);

    SHR_IF_ERR_EXIT(algo_sat_db_sat_res_alloc_gtf_id_dump(unit, filters));

    if (!filters.nocontent)
    {
        dnx_sw_state_dump_detach_file(unit);

    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_sat_sat_res_alloc_gtf_id_dump_options[] = {
    {"nocontent",      SAL_FIELD_TYPE_BOOL, "Decision if to perform content dump or to print the access tree.",  "false"},
    {"typefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable type.",         ""},
    {"namefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable name.",         ""},
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_gtf_id_dump_man = {
    "swstate dump",
    "print the variable value",
    "algo_sat_db sat_res_alloc_gtf_id dump []\n",
    "algo_sat_db sat_res_alloc_gtf_id dump nocontent=false typefilter="" namefilter="" ",
};


shr_error_e sh_dnx_swstate_algo_sat_sat_res_alloc_gtf_id_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{
    uint32 size = 0;

    SHR_FUNC_INIT_VARS(unit);


    size = dnx_sw_state_info_size_get(algo_sat_db_info[unit], algo_sat_db_layout_str, ALGO_SAT_DB_INFO_NOF_ENTRIES, "ALGO_SAT_DB~SAT_RES_ALLOC_GTF_ID~", 0);
    dnx_sw_state_size_print(size);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_sat_sat_res_alloc_gtf_id_size_get_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_gtf_id_size_get_man = {
    "swstate size_get",
    "print the variable size",
    "algo_sat_db sat_res_alloc_gtf_id size_get",
    "algo_sat_db sat_res_alloc_gtf_id size_get",
};


shr_error_e sh_dnx_swstate_algo_sat_sat_res_alloc_gtf_id_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    SHR_FUNC_INIT_VARS(unit);


    dnx_sw_state_diagnostic_operation_counters_print(algo_sat_db_info[unit], algo_sat_db_layout_str, ALGO_SAT_DB_INFO_NOF_ENTRIES, "ALGO_SAT_DB~SAT_RES_ALLOC_GTF_ID~");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_sat_sat_res_alloc_gtf_id_diagnostic_operation_counters_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_gtf_id_diagnostic_operation_counters_man = {
    "swstate diagnostic_operation_counters",
    "print the diagnostic operation counters",
    "algo_sat_db sat_res_alloc_gtf_id diagnostic_operation_counters",
    "algo_sat_db sat_res_alloc_gtf_id diagnostic_operation_counters",
};



sh_sand_cmd_t sh_dnx_swstate_algo_sat_sat_res_alloc_ctf_id_cmds[] = {
    {"dump", sh_dnx_swstate_algo_sat_sat_res_alloc_ctf_id_dump_cmd, NULL, dnx_swstate_algo_sat_sat_res_alloc_ctf_id_dump_options, &dnx_swstate_algo_sat_sat_res_alloc_ctf_id_dump_man},
    {"size_get", sh_dnx_swstate_algo_sat_sat_res_alloc_ctf_id_size_get_cmd, NULL, dnx_swstate_algo_sat_sat_res_alloc_ctf_id_size_get_options, &dnx_swstate_algo_sat_sat_res_alloc_ctf_id_size_get_man},
    {"diagnostic_operation_counters", sh_dnx_swstate_algo_sat_sat_res_alloc_ctf_id_diagnostic_operation_counters_cmd, NULL, dnx_swstate_algo_sat_sat_res_alloc_ctf_id_diagnostic_operation_counters_options, &dnx_swstate_algo_sat_sat_res_alloc_ctf_id_diagnostic_operation_counters_man},
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_ctf_id_man = {
    cmd_dnx_swstate_algo_sat_sat_res_alloc_ctf_id_desc,
    NULL,
    NULL,
    NULL,
};


const char cmd_dnx_swstate_algo_sat_sat_res_alloc_ctf_id_desc[] = "algo_sat_db sat_res_alloc_ctf_id commands";


shr_error_e sh_dnx_swstate_algo_sat_sat_res_alloc_ctf_id_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    dnx_sw_state_dump_filters_t filters;

    SHR_FUNC_INIT_VARS(unit);


    SH_SAND_GET_BOOL("nocontent", filters.nocontent);
    SH_SAND_GET_STR("typefilter", filters.typefilter);
    SH_SAND_GET_STR("namefilter", filters.namefilter);

    SHR_IF_ERR_EXIT(algo_sat_db_sat_res_alloc_ctf_id_dump(unit, filters));

    if (!filters.nocontent)
    {
        dnx_sw_state_dump_detach_file(unit);

    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_sat_sat_res_alloc_ctf_id_dump_options[] = {
    {"nocontent",      SAL_FIELD_TYPE_BOOL, "Decision if to perform content dump or to print the access tree.",  "false"},
    {"typefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable type.",         ""},
    {"namefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable name.",         ""},
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_ctf_id_dump_man = {
    "swstate dump",
    "print the variable value",
    "algo_sat_db sat_res_alloc_ctf_id dump []\n",
    "algo_sat_db sat_res_alloc_ctf_id dump nocontent=false typefilter="" namefilter="" ",
};


shr_error_e sh_dnx_swstate_algo_sat_sat_res_alloc_ctf_id_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{
    uint32 size = 0;

    SHR_FUNC_INIT_VARS(unit);


    size = dnx_sw_state_info_size_get(algo_sat_db_info[unit], algo_sat_db_layout_str, ALGO_SAT_DB_INFO_NOF_ENTRIES, "ALGO_SAT_DB~SAT_RES_ALLOC_CTF_ID~", 0);
    dnx_sw_state_size_print(size);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_sat_sat_res_alloc_ctf_id_size_get_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_ctf_id_size_get_man = {
    "swstate size_get",
    "print the variable size",
    "algo_sat_db sat_res_alloc_ctf_id size_get",
    "algo_sat_db sat_res_alloc_ctf_id size_get",
};


shr_error_e sh_dnx_swstate_algo_sat_sat_res_alloc_ctf_id_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    SHR_FUNC_INIT_VARS(unit);


    dnx_sw_state_diagnostic_operation_counters_print(algo_sat_db_info[unit], algo_sat_db_layout_str, ALGO_SAT_DB_INFO_NOF_ENTRIES, "ALGO_SAT_DB~SAT_RES_ALLOC_CTF_ID~");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_sat_sat_res_alloc_ctf_id_diagnostic_operation_counters_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_ctf_id_diagnostic_operation_counters_man = {
    "swstate diagnostic_operation_counters",
    "print the diagnostic operation counters",
    "algo_sat_db sat_res_alloc_ctf_id diagnostic_operation_counters",
    "algo_sat_db sat_res_alloc_ctf_id diagnostic_operation_counters",
};



sh_sand_cmd_t sh_dnx_swstate_algo_sat_sat_res_alloc_trap_id_cmds[] = {
    {"dump", sh_dnx_swstate_algo_sat_sat_res_alloc_trap_id_dump_cmd, NULL, dnx_swstate_algo_sat_sat_res_alloc_trap_id_dump_options, &dnx_swstate_algo_sat_sat_res_alloc_trap_id_dump_man},
    {"size_get", sh_dnx_swstate_algo_sat_sat_res_alloc_trap_id_size_get_cmd, NULL, dnx_swstate_algo_sat_sat_res_alloc_trap_id_size_get_options, &dnx_swstate_algo_sat_sat_res_alloc_trap_id_size_get_man},
    {"diagnostic_operation_counters", sh_dnx_swstate_algo_sat_sat_res_alloc_trap_id_diagnostic_operation_counters_cmd, NULL, dnx_swstate_algo_sat_sat_res_alloc_trap_id_diagnostic_operation_counters_options, &dnx_swstate_algo_sat_sat_res_alloc_trap_id_diagnostic_operation_counters_man},
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_trap_id_man = {
    cmd_dnx_swstate_algo_sat_sat_res_alloc_trap_id_desc,
    NULL,
    NULL,
    NULL,
};


const char cmd_dnx_swstate_algo_sat_sat_res_alloc_trap_id_desc[] = "algo_sat_db sat_res_alloc_trap_id commands";


shr_error_e sh_dnx_swstate_algo_sat_sat_res_alloc_trap_id_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    dnx_sw_state_dump_filters_t filters;

    SHR_FUNC_INIT_VARS(unit);


    SH_SAND_GET_BOOL("nocontent", filters.nocontent);
    SH_SAND_GET_STR("typefilter", filters.typefilter);
    SH_SAND_GET_STR("namefilter", filters.namefilter);

    SHR_IF_ERR_EXIT(algo_sat_db_sat_res_alloc_trap_id_dump(unit, filters));

    if (!filters.nocontent)
    {
        dnx_sw_state_dump_detach_file(unit);

    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_sat_sat_res_alloc_trap_id_dump_options[] = {
    {"nocontent",      SAL_FIELD_TYPE_BOOL, "Decision if to perform content dump or to print the access tree.",  "false"},
    {"typefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable type.",         ""},
    {"namefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable name.",         ""},
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_trap_id_dump_man = {
    "swstate dump",
    "print the variable value",
    "algo_sat_db sat_res_alloc_trap_id dump []\n",
    "algo_sat_db sat_res_alloc_trap_id dump nocontent=false typefilter="" namefilter="" ",
};


shr_error_e sh_dnx_swstate_algo_sat_sat_res_alloc_trap_id_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{
    uint32 size = 0;

    SHR_FUNC_INIT_VARS(unit);


    size = dnx_sw_state_info_size_get(algo_sat_db_info[unit], algo_sat_db_layout_str, ALGO_SAT_DB_INFO_NOF_ENTRIES, "ALGO_SAT_DB~SAT_RES_ALLOC_TRAP_ID~", 0);
    dnx_sw_state_size_print(size);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_sat_sat_res_alloc_trap_id_size_get_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_trap_id_size_get_man = {
    "swstate size_get",
    "print the variable size",
    "algo_sat_db sat_res_alloc_trap_id size_get",
    "algo_sat_db sat_res_alloc_trap_id size_get",
};


shr_error_e sh_dnx_swstate_algo_sat_sat_res_alloc_trap_id_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    SHR_FUNC_INIT_VARS(unit);


    dnx_sw_state_diagnostic_operation_counters_print(algo_sat_db_info[unit], algo_sat_db_layout_str, ALGO_SAT_DB_INFO_NOF_ENTRIES, "ALGO_SAT_DB~SAT_RES_ALLOC_TRAP_ID~");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_sat_sat_res_alloc_trap_id_diagnostic_operation_counters_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_trap_id_diagnostic_operation_counters_man = {
    "swstate diagnostic_operation_counters",
    "print the diagnostic operation counters",
    "algo_sat_db sat_res_alloc_trap_id diagnostic_operation_counters",
    "algo_sat_db sat_res_alloc_trap_id diagnostic_operation_counters",
};



sh_sand_cmd_t sh_dnx_swstate_algo_sat_sat_res_alloc_trap_data_cmds[] = {
    {"dump", sh_dnx_swstate_algo_sat_sat_res_alloc_trap_data_dump_cmd, NULL, dnx_swstate_algo_sat_sat_res_alloc_trap_data_dump_options, &dnx_swstate_algo_sat_sat_res_alloc_trap_data_dump_man},
    {"size_get", sh_dnx_swstate_algo_sat_sat_res_alloc_trap_data_size_get_cmd, NULL, dnx_swstate_algo_sat_sat_res_alloc_trap_data_size_get_options, &dnx_swstate_algo_sat_sat_res_alloc_trap_data_size_get_man},
    {"diagnostic_operation_counters", sh_dnx_swstate_algo_sat_sat_res_alloc_trap_data_diagnostic_operation_counters_cmd, NULL, dnx_swstate_algo_sat_sat_res_alloc_trap_data_diagnostic_operation_counters_options, &dnx_swstate_algo_sat_sat_res_alloc_trap_data_diagnostic_operation_counters_man},
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_trap_data_man = {
    cmd_dnx_swstate_algo_sat_sat_res_alloc_trap_data_desc,
    NULL,
    NULL,
    NULL,
};


const char cmd_dnx_swstate_algo_sat_sat_res_alloc_trap_data_desc[] = "algo_sat_db sat_res_alloc_trap_data commands";


shr_error_e sh_dnx_swstate_algo_sat_sat_res_alloc_trap_data_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    dnx_sw_state_dump_filters_t filters;

    SHR_FUNC_INIT_VARS(unit);


    SH_SAND_GET_BOOL("nocontent", filters.nocontent);
    SH_SAND_GET_STR("typefilter", filters.typefilter);
    SH_SAND_GET_STR("namefilter", filters.namefilter);

    SHR_IF_ERR_EXIT(algo_sat_db_sat_res_alloc_trap_data_dump(unit, filters));

    if (!filters.nocontent)
    {
        dnx_sw_state_dump_detach_file(unit);

    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_sat_sat_res_alloc_trap_data_dump_options[] = {
    {"nocontent",      SAL_FIELD_TYPE_BOOL, "Decision if to perform content dump or to print the access tree.",  "false"},
    {"typefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable type.",         ""},
    {"namefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable name.",         ""},
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_trap_data_dump_man = {
    "swstate dump",
    "print the variable value",
    "algo_sat_db sat_res_alloc_trap_data dump []\n",
    "algo_sat_db sat_res_alloc_trap_data dump nocontent=false typefilter="" namefilter="" ",
};


shr_error_e sh_dnx_swstate_algo_sat_sat_res_alloc_trap_data_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{
    uint32 size = 0;

    SHR_FUNC_INIT_VARS(unit);


    size = dnx_sw_state_info_size_get(algo_sat_db_info[unit], algo_sat_db_layout_str, ALGO_SAT_DB_INFO_NOF_ENTRIES, "ALGO_SAT_DB~SAT_RES_ALLOC_TRAP_DATA~", 0);
    dnx_sw_state_size_print(size);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_sat_sat_res_alloc_trap_data_size_get_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_trap_data_size_get_man = {
    "swstate size_get",
    "print the variable size",
    "algo_sat_db sat_res_alloc_trap_data size_get",
    "algo_sat_db sat_res_alloc_trap_data size_get",
};


shr_error_e sh_dnx_swstate_algo_sat_sat_res_alloc_trap_data_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    SHR_FUNC_INIT_VARS(unit);


    dnx_sw_state_diagnostic_operation_counters_print(algo_sat_db_info[unit], algo_sat_db_layout_str, ALGO_SAT_DB_INFO_NOF_ENTRIES, "ALGO_SAT_DB~SAT_RES_ALLOC_TRAP_DATA~");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_sat_sat_res_alloc_trap_data_diagnostic_operation_counters_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_sat_sat_res_alloc_trap_data_diagnostic_operation_counters_man = {
    "swstate diagnostic_operation_counters",
    "print the diagnostic operation counters",
    "algo_sat_db sat_res_alloc_trap_data diagnostic_operation_counters",
    "algo_sat_db sat_res_alloc_trap_data diagnostic_operation_counters",
};



sh_sand_cmd_t sh_dnx_swstate_algo_sat_sat_pkt_header_cmds[] = {
    {"dump", sh_dnx_swstate_algo_sat_sat_pkt_header_dump_cmd, NULL, dnx_swstate_algo_sat_sat_pkt_header_dump_options, &dnx_swstate_algo_sat_sat_pkt_header_dump_man},
    {"size_get", sh_dnx_swstate_algo_sat_sat_pkt_header_size_get_cmd, NULL, dnx_swstate_algo_sat_sat_pkt_header_size_get_options, &dnx_swstate_algo_sat_sat_pkt_header_size_get_man},
    {"diagnostic_operation_counters", sh_dnx_swstate_algo_sat_sat_pkt_header_diagnostic_operation_counters_cmd, NULL, dnx_swstate_algo_sat_sat_pkt_header_diagnostic_operation_counters_options, &dnx_swstate_algo_sat_sat_pkt_header_diagnostic_operation_counters_man},
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_sat_sat_pkt_header_man = {
    cmd_dnx_swstate_algo_sat_sat_pkt_header_desc,
    NULL,
    NULL,
    NULL,
};


const char cmd_dnx_swstate_algo_sat_sat_pkt_header_desc[] = "algo_sat_db sat_pkt_header commands";


shr_error_e sh_dnx_swstate_algo_sat_sat_pkt_header_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    dnx_sw_state_dump_filters_t filters;

    SHR_FUNC_INIT_VARS(unit);


    SH_SAND_GET_BOOL("nocontent", filters.nocontent);
    SH_SAND_GET_STR("typefilter", filters.typefilter);
    SH_SAND_GET_STR("namefilter", filters.namefilter);

    SHR_IF_ERR_EXIT(algo_sat_db_sat_pkt_header_dump(unit, filters));

    if (!filters.nocontent)
    {
        dnx_sw_state_dump_detach_file(unit);

    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_sat_sat_pkt_header_dump_options[] = {
    {"nocontent",      SAL_FIELD_TYPE_BOOL, "Decision if to perform content dump or to print the access tree.",  "false"},
    {"typefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable type.",         ""},
    {"namefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable name.",         ""},
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_sat_sat_pkt_header_dump_man = {
    "swstate dump",
    "print the variable value",
    "algo_sat_db sat_pkt_header dump []\n",
    "algo_sat_db sat_pkt_header dump nocontent=false typefilter="" namefilter="" ",
};


shr_error_e sh_dnx_swstate_algo_sat_sat_pkt_header_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{
    uint32 size = 0;

    SHR_FUNC_INIT_VARS(unit);


    size = dnx_sw_state_info_size_get(algo_sat_db_info[unit], algo_sat_db_layout_str, ALGO_SAT_DB_INFO_NOF_ENTRIES, "ALGO_SAT_DB~SAT_PKT_HEADER~", 0);
    dnx_sw_state_size_print(size);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_sat_sat_pkt_header_size_get_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_sat_sat_pkt_header_size_get_man = {
    "swstate size_get",
    "print the variable size",
    "algo_sat_db sat_pkt_header size_get",
    "algo_sat_db sat_pkt_header size_get",
};


shr_error_e sh_dnx_swstate_algo_sat_sat_pkt_header_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    SHR_FUNC_INIT_VARS(unit);


    dnx_sw_state_diagnostic_operation_counters_print(algo_sat_db_info[unit], algo_sat_db_layout_str, ALGO_SAT_DB_INFO_NOF_ENTRIES, "ALGO_SAT_DB~SAT_PKT_HEADER~");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_algo_sat_sat_pkt_header_diagnostic_operation_counters_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_algo_sat_sat_pkt_header_diagnostic_operation_counters_man = {
    "swstate diagnostic_operation_counters",
    "print the diagnostic operation counters",
    "algo_sat_db sat_pkt_header diagnostic_operation_counters",
    "algo_sat_db sat_pkt_header diagnostic_operation_counters",
};


#endif /* DNX_SW_STATE_DIAGNOSTIC */
#undef BSL_LOG_MODULE
