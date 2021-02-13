/** \file dnx/swstate/auto_generated/diagnostic/switch_commandline.c
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

#include <soc/dnx/swstate/auto_generated/diagnostic/switch_diagnostic.h>
#include "switch_commandline.h"
#include <soc/dnx/swstate/auto_generated/access/switch_access.h>
#include <bcm/switch.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
sh_sand_cmd_t sh_dnx_swstate_switch_cmds[] = {
    {"module_verification", NULL, sh_dnx_swstate_switch_module_verification_cmds, NULL, &dnx_swstate_switch_module_verification_man},
    {"module_error_recovery", NULL, sh_dnx_swstate_switch_module_error_recovery_cmds, NULL, &dnx_swstate_switch_module_error_recovery_man},
    {"l3mcastl2_ipv4_fwd_type", NULL, sh_dnx_swstate_switch_l3mcastl2_ipv4_fwd_type_cmds, NULL, &dnx_swstate_switch_l3mcastl2_ipv4_fwd_type_man},
    {"l3mcastl2_ipv6_fwd_type", NULL, sh_dnx_swstate_switch_l3mcastl2_ipv6_fwd_type_cmds, NULL, &dnx_swstate_switch_l3mcastl2_ipv6_fwd_type_man},
    {"header_enablers_hashing", NULL, sh_dnx_swstate_switch_header_enablers_hashing_cmds, NULL, &dnx_swstate_switch_header_enablers_hashing_man},
    {"dump", sh_dnx_swstate_switch_dump_cmd, NULL, dnx_swstate_switch_dump_options, &dnx_swstate_switch_dump_man},
    {"size_get", sh_dnx_swstate_switch_size_get_cmd, NULL, dnx_swstate_switch_size_get_options, &dnx_swstate_switch_size_get_man},
    {"diagnostic_operation_counters", sh_dnx_swstate_switch_diagnostic_operation_counters_cmd, NULL, dnx_swstate_switch_diagnostic_operation_counters_options, &dnx_swstate_switch_diagnostic_operation_counters_man},
    {NULL}
};


shr_error_e sh_dnx_swstate_switch_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    uint8 is_init;
    dnx_sw_state_dump_filters_t filters;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("nocontent", filters.nocontent);
    SH_SAND_GET_STR("typefilter", filters.typefilter);
    SH_SAND_GET_STR("namefilter", filters.namefilter);

    switch_db.is_init(unit, &is_init);
    if (is_init) {
        SHR_IF_ERR_EXIT(switch_db_dump(unit, filters));
        if (!filters.nocontent)
        {
            dnx_sw_state_dump_detach_file(unit);
        }
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_switch_dump_options[] = {
    {"nocontent",      SAL_FIELD_TYPE_BOOL, "Decision if to perform content dump or to print the access tree.",  "false"},
    {"typefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable type.",         ""},
    {"namefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable name.",         ""},
    {NULL}
};


sh_sand_man_t dnx_swstate_switch_dump_man = {
    "swstate switch dump",
    "print the module's content",
    "swstate switch dump",
    "swstate switch dump nocontent=false typefilter="" namefilter="" ",
};


shr_error_e sh_dnx_swstate_switch_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    uint32 size = 0;
    SHR_FUNC_INIT_VARS(unit);

    size = dnx_sw_state_info_size_get(switch_db_info[unit], switch_db_layout_str, SWITCH_DB_INFO_NOF_ENTRIES, "SWITCH_DB~", 0);
    dnx_sw_state_size_print(size);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_switch_size_get_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_switch_size_get_man = {
    "swstate switch size_get",
    "print the module's size",
    "swstate switch size_get",
    "swstate switch size_get",
};


shr_error_e sh_dnx_swstate_switch_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{


    SHR_FUNC_INIT_VARS(unit);

    dnx_sw_state_diagnostic_operation_counters_print(switch_db_info[unit], switch_db_layout_str, SWITCH_DB_INFO_NOF_ENTRIES, "SWITCH_DB~");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_switch_diagnostic_operation_counters_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_switch_diagnostic_operation_counters_man = {
    "swstate switch diagnostic_operation_counters",
    "print the module's diagnostic operation counters",
    "swstate switch diagnostic_operation_counters",
    "swstate switch diagnostic_operation_counters",
};


sh_sand_man_t dnx_swstate_switch_man = {
    cmd_dnx_swstate_switch_desc,
    NULL,
    NULL,
    NULL,
};


const char cmd_dnx_swstate_switch_desc[] = "swstate switch commands";



sh_sand_cmd_t sh_dnx_swstate_switch_module_verification_cmds[] = {
    {"dump", sh_dnx_swstate_switch_module_verification_dump_cmd, NULL, dnx_swstate_switch_module_verification_dump_options, &dnx_swstate_switch_module_verification_dump_man},
    {"size_get", sh_dnx_swstate_switch_module_verification_size_get_cmd, NULL, dnx_swstate_switch_module_verification_size_get_options, &dnx_swstate_switch_module_verification_size_get_man},
    {"diagnostic_operation_counters", sh_dnx_swstate_switch_module_verification_diagnostic_operation_counters_cmd, NULL, dnx_swstate_switch_module_verification_diagnostic_operation_counters_options, &dnx_swstate_switch_module_verification_diagnostic_operation_counters_man},
    {NULL}
};


sh_sand_man_t dnx_swstate_switch_module_verification_man = {
    cmd_dnx_swstate_switch_module_verification_desc,
    NULL,
    NULL,
    NULL,
};


const char cmd_dnx_swstate_switch_module_verification_desc[] = "switch_db module_verification commands";


shr_error_e sh_dnx_swstate_switch_module_verification_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{
    int  module_verification_idx_0 = 0;

    dnx_sw_state_dump_filters_t filters;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("module_verification_idx_0", module_verification_idx_0);

    SH_SAND_GET_BOOL("nocontent", filters.nocontent);
    SH_SAND_GET_STR("typefilter", filters.typefilter);
    SH_SAND_GET_STR("namefilter", filters.namefilter);

    SHR_IF_ERR_EXIT(switch_db_module_verification_dump(unit, module_verification_idx_0, filters));

    if (!filters.nocontent)
    {
        dnx_sw_state_dump_detach_file(unit);

    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_switch_module_verification_dump_options[] = {
    {"module_verification_idx_0", SAL_FIELD_TYPE_INT32, "index", NULL},
    {"nocontent",      SAL_FIELD_TYPE_BOOL, "Decision if to perform content dump or to print the access tree.",  "false"},
    {"typefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable type.",         ""},
    {"namefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable name.",         ""},
    {NULL}
};


sh_sand_man_t dnx_swstate_switch_module_verification_dump_man = {
    "swstate dump",
    "print the variable value",
    "switch_db module_verification dump [,module_verification_idx_0=<int>]\n",
    "switch_db module_verification dump module_verification_idx_0=0 nocontent=false typefilter="" namefilter="" ",
};


shr_error_e sh_dnx_swstate_switch_module_verification_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{
    uint32 size = 0;

    SHR_FUNC_INIT_VARS(unit);


    size = dnx_sw_state_info_size_get(switch_db_info[unit], switch_db_layout_str, SWITCH_DB_INFO_NOF_ENTRIES, "SWITCH_DB~MODULE_VERIFICATION~", sizeof(((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->module_verification));
    dnx_sw_state_size_print(size);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_switch_module_verification_size_get_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_switch_module_verification_size_get_man = {
    "swstate size_get",
    "print the variable size",
    "switch_db module_verification size_get",
    "switch_db module_verification size_get",
};


shr_error_e sh_dnx_swstate_switch_module_verification_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    SHR_FUNC_INIT_VARS(unit);


    dnx_sw_state_diagnostic_operation_counters_print(switch_db_info[unit], switch_db_layout_str, SWITCH_DB_INFO_NOF_ENTRIES, "SWITCH_DB~MODULE_VERIFICATION~");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_switch_module_verification_diagnostic_operation_counters_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_switch_module_verification_diagnostic_operation_counters_man = {
    "swstate diagnostic_operation_counters",
    "print the diagnostic operation counters",
    "switch_db module_verification diagnostic_operation_counters",
    "switch_db module_verification diagnostic_operation_counters",
};



sh_sand_cmd_t sh_dnx_swstate_switch_module_error_recovery_cmds[] = {
    {"dump", sh_dnx_swstate_switch_module_error_recovery_dump_cmd, NULL, dnx_swstate_switch_module_error_recovery_dump_options, &dnx_swstate_switch_module_error_recovery_dump_man},
    {"size_get", sh_dnx_swstate_switch_module_error_recovery_size_get_cmd, NULL, dnx_swstate_switch_module_error_recovery_size_get_options, &dnx_swstate_switch_module_error_recovery_size_get_man},
    {"diagnostic_operation_counters", sh_dnx_swstate_switch_module_error_recovery_diagnostic_operation_counters_cmd, NULL, dnx_swstate_switch_module_error_recovery_diagnostic_operation_counters_options, &dnx_swstate_switch_module_error_recovery_diagnostic_operation_counters_man},
    {NULL}
};


sh_sand_man_t dnx_swstate_switch_module_error_recovery_man = {
    cmd_dnx_swstate_switch_module_error_recovery_desc,
    NULL,
    NULL,
    NULL,
};


const char cmd_dnx_swstate_switch_module_error_recovery_desc[] = "switch_db module_error_recovery commands";


shr_error_e sh_dnx_swstate_switch_module_error_recovery_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{
    int  module_error_recovery_idx_0 = 0;

    dnx_sw_state_dump_filters_t filters;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("module_error_recovery_idx_0", module_error_recovery_idx_0);

    SH_SAND_GET_BOOL("nocontent", filters.nocontent);
    SH_SAND_GET_STR("typefilter", filters.typefilter);
    SH_SAND_GET_STR("namefilter", filters.namefilter);

    SHR_IF_ERR_EXIT(switch_db_module_error_recovery_dump(unit, module_error_recovery_idx_0, filters));

    if (!filters.nocontent)
    {
        dnx_sw_state_dump_detach_file(unit);

    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_switch_module_error_recovery_dump_options[] = {
    {"module_error_recovery_idx_0", SAL_FIELD_TYPE_INT32, "index", NULL},
    {"nocontent",      SAL_FIELD_TYPE_BOOL, "Decision if to perform content dump or to print the access tree.",  "false"},
    {"typefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable type.",         ""},
    {"namefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable name.",         ""},
    {NULL}
};


sh_sand_man_t dnx_swstate_switch_module_error_recovery_dump_man = {
    "swstate dump",
    "print the variable value",
    "switch_db module_error_recovery dump [,module_error_recovery_idx_0=<int>]\n",
    "switch_db module_error_recovery dump module_error_recovery_idx_0=0 nocontent=false typefilter="" namefilter="" ",
};


shr_error_e sh_dnx_swstate_switch_module_error_recovery_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{
    uint32 size = 0;

    SHR_FUNC_INIT_VARS(unit);


    size = dnx_sw_state_info_size_get(switch_db_info[unit], switch_db_layout_str, SWITCH_DB_INFO_NOF_ENTRIES, "SWITCH_DB~MODULE_ERROR_RECOVERY~", sizeof(((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->module_error_recovery));
    dnx_sw_state_size_print(size);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_switch_module_error_recovery_size_get_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_switch_module_error_recovery_size_get_man = {
    "swstate size_get",
    "print the variable size",
    "switch_db module_error_recovery size_get",
    "switch_db module_error_recovery size_get",
};


shr_error_e sh_dnx_swstate_switch_module_error_recovery_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    SHR_FUNC_INIT_VARS(unit);


    dnx_sw_state_diagnostic_operation_counters_print(switch_db_info[unit], switch_db_layout_str, SWITCH_DB_INFO_NOF_ENTRIES, "SWITCH_DB~MODULE_ERROR_RECOVERY~");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_switch_module_error_recovery_diagnostic_operation_counters_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_switch_module_error_recovery_diagnostic_operation_counters_man = {
    "swstate diagnostic_operation_counters",
    "print the diagnostic operation counters",
    "switch_db module_error_recovery diagnostic_operation_counters",
    "switch_db module_error_recovery diagnostic_operation_counters",
};



sh_sand_cmd_t sh_dnx_swstate_switch_l3mcastl2_ipv4_fwd_type_cmds[] = {
    {"dump", sh_dnx_swstate_switch_l3mcastl2_ipv4_fwd_type_dump_cmd, NULL, dnx_swstate_switch_l3mcastl2_ipv4_fwd_type_dump_options, &dnx_swstate_switch_l3mcastl2_ipv4_fwd_type_dump_man},
    {"size_get", sh_dnx_swstate_switch_l3mcastl2_ipv4_fwd_type_size_get_cmd, NULL, dnx_swstate_switch_l3mcastl2_ipv4_fwd_type_size_get_options, &dnx_swstate_switch_l3mcastl2_ipv4_fwd_type_size_get_man},
    {"diagnostic_operation_counters", sh_dnx_swstate_switch_l3mcastl2_ipv4_fwd_type_diagnostic_operation_counters_cmd, NULL, dnx_swstate_switch_l3mcastl2_ipv4_fwd_type_diagnostic_operation_counters_options, &dnx_swstate_switch_l3mcastl2_ipv4_fwd_type_diagnostic_operation_counters_man},
    {NULL}
};


sh_sand_man_t dnx_swstate_switch_l3mcastl2_ipv4_fwd_type_man = {
    cmd_dnx_swstate_switch_l3mcastl2_ipv4_fwd_type_desc,
    NULL,
    NULL,
    NULL,
};


const char cmd_dnx_swstate_switch_l3mcastl2_ipv4_fwd_type_desc[] = "switch_db l3mcastl2_ipv4_fwd_type commands";


shr_error_e sh_dnx_swstate_switch_l3mcastl2_ipv4_fwd_type_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    dnx_sw_state_dump_filters_t filters;

    SHR_FUNC_INIT_VARS(unit);


    SH_SAND_GET_BOOL("nocontent", filters.nocontent);
    SH_SAND_GET_STR("typefilter", filters.typefilter);
    SH_SAND_GET_STR("namefilter", filters.namefilter);

    SHR_IF_ERR_EXIT(switch_db_l3mcastl2_ipv4_fwd_type_dump(unit, filters));

    if (!filters.nocontent)
    {
        dnx_sw_state_dump_detach_file(unit);

    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_switch_l3mcastl2_ipv4_fwd_type_dump_options[] = {
    {"nocontent",      SAL_FIELD_TYPE_BOOL, "Decision if to perform content dump or to print the access tree.",  "false"},
    {"typefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable type.",         ""},
    {"namefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable name.",         ""},
    {NULL}
};


sh_sand_man_t dnx_swstate_switch_l3mcastl2_ipv4_fwd_type_dump_man = {
    "swstate dump",
    "print the variable value",
    "switch_db l3mcastl2_ipv4_fwd_type dump []\n",
    "switch_db l3mcastl2_ipv4_fwd_type dump nocontent=false typefilter="" namefilter="" ",
};


shr_error_e sh_dnx_swstate_switch_l3mcastl2_ipv4_fwd_type_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{
    uint32 size = 0;

    SHR_FUNC_INIT_VARS(unit);


    size = dnx_sw_state_info_size_get(switch_db_info[unit], switch_db_layout_str, SWITCH_DB_INFO_NOF_ENTRIES, "SWITCH_DB~L3MCASTL2_IPV4_FWD_TYPE~", sizeof(((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->l3mcastl2_ipv4_fwd_type));
    dnx_sw_state_size_print(size);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_switch_l3mcastl2_ipv4_fwd_type_size_get_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_switch_l3mcastl2_ipv4_fwd_type_size_get_man = {
    "swstate size_get",
    "print the variable size",
    "switch_db l3mcastl2_ipv4_fwd_type size_get",
    "switch_db l3mcastl2_ipv4_fwd_type size_get",
};


shr_error_e sh_dnx_swstate_switch_l3mcastl2_ipv4_fwd_type_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    SHR_FUNC_INIT_VARS(unit);


    dnx_sw_state_diagnostic_operation_counters_print(switch_db_info[unit], switch_db_layout_str, SWITCH_DB_INFO_NOF_ENTRIES, "SWITCH_DB~L3MCASTL2_IPV4_FWD_TYPE~");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_switch_l3mcastl2_ipv4_fwd_type_diagnostic_operation_counters_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_switch_l3mcastl2_ipv4_fwd_type_diagnostic_operation_counters_man = {
    "swstate diagnostic_operation_counters",
    "print the diagnostic operation counters",
    "switch_db l3mcastl2_ipv4_fwd_type diagnostic_operation_counters",
    "switch_db l3mcastl2_ipv4_fwd_type diagnostic_operation_counters",
};



sh_sand_cmd_t sh_dnx_swstate_switch_l3mcastl2_ipv6_fwd_type_cmds[] = {
    {"dump", sh_dnx_swstate_switch_l3mcastl2_ipv6_fwd_type_dump_cmd, NULL, dnx_swstate_switch_l3mcastl2_ipv6_fwd_type_dump_options, &dnx_swstate_switch_l3mcastl2_ipv6_fwd_type_dump_man},
    {"size_get", sh_dnx_swstate_switch_l3mcastl2_ipv6_fwd_type_size_get_cmd, NULL, dnx_swstate_switch_l3mcastl2_ipv6_fwd_type_size_get_options, &dnx_swstate_switch_l3mcastl2_ipv6_fwd_type_size_get_man},
    {"diagnostic_operation_counters", sh_dnx_swstate_switch_l3mcastl2_ipv6_fwd_type_diagnostic_operation_counters_cmd, NULL, dnx_swstate_switch_l3mcastl2_ipv6_fwd_type_diagnostic_operation_counters_options, &dnx_swstate_switch_l3mcastl2_ipv6_fwd_type_diagnostic_operation_counters_man},
    {NULL}
};


sh_sand_man_t dnx_swstate_switch_l3mcastl2_ipv6_fwd_type_man = {
    cmd_dnx_swstate_switch_l3mcastl2_ipv6_fwd_type_desc,
    NULL,
    NULL,
    NULL,
};


const char cmd_dnx_swstate_switch_l3mcastl2_ipv6_fwd_type_desc[] = "switch_db l3mcastl2_ipv6_fwd_type commands";


shr_error_e sh_dnx_swstate_switch_l3mcastl2_ipv6_fwd_type_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    dnx_sw_state_dump_filters_t filters;

    SHR_FUNC_INIT_VARS(unit);


    SH_SAND_GET_BOOL("nocontent", filters.nocontent);
    SH_SAND_GET_STR("typefilter", filters.typefilter);
    SH_SAND_GET_STR("namefilter", filters.namefilter);

    SHR_IF_ERR_EXIT(switch_db_l3mcastl2_ipv6_fwd_type_dump(unit, filters));

    if (!filters.nocontent)
    {
        dnx_sw_state_dump_detach_file(unit);

    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_switch_l3mcastl2_ipv6_fwd_type_dump_options[] = {
    {"nocontent",      SAL_FIELD_TYPE_BOOL, "Decision if to perform content dump or to print the access tree.",  "false"},
    {"typefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable type.",         ""},
    {"namefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable name.",         ""},
    {NULL}
};


sh_sand_man_t dnx_swstate_switch_l3mcastl2_ipv6_fwd_type_dump_man = {
    "swstate dump",
    "print the variable value",
    "switch_db l3mcastl2_ipv6_fwd_type dump []\n",
    "switch_db l3mcastl2_ipv6_fwd_type dump nocontent=false typefilter="" namefilter="" ",
};


shr_error_e sh_dnx_swstate_switch_l3mcastl2_ipv6_fwd_type_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{
    uint32 size = 0;

    SHR_FUNC_INIT_VARS(unit);


    size = dnx_sw_state_info_size_get(switch_db_info[unit], switch_db_layout_str, SWITCH_DB_INFO_NOF_ENTRIES, "SWITCH_DB~L3MCASTL2_IPV6_FWD_TYPE~", sizeof(((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->l3mcastl2_ipv6_fwd_type));
    dnx_sw_state_size_print(size);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_switch_l3mcastl2_ipv6_fwd_type_size_get_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_switch_l3mcastl2_ipv6_fwd_type_size_get_man = {
    "swstate size_get",
    "print the variable size",
    "switch_db l3mcastl2_ipv6_fwd_type size_get",
    "switch_db l3mcastl2_ipv6_fwd_type size_get",
};


shr_error_e sh_dnx_swstate_switch_l3mcastl2_ipv6_fwd_type_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    SHR_FUNC_INIT_VARS(unit);


    dnx_sw_state_diagnostic_operation_counters_print(switch_db_info[unit], switch_db_layout_str, SWITCH_DB_INFO_NOF_ENTRIES, "SWITCH_DB~L3MCASTL2_IPV6_FWD_TYPE~");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_switch_l3mcastl2_ipv6_fwd_type_diagnostic_operation_counters_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_switch_l3mcastl2_ipv6_fwd_type_diagnostic_operation_counters_man = {
    "swstate diagnostic_operation_counters",
    "print the diagnostic operation counters",
    "switch_db l3mcastl2_ipv6_fwd_type diagnostic_operation_counters",
    "switch_db l3mcastl2_ipv6_fwd_type diagnostic_operation_counters",
};



sh_sand_cmd_t sh_dnx_swstate_switch_header_enablers_hashing_cmds[] = {
    {"dump", sh_dnx_swstate_switch_header_enablers_hashing_dump_cmd, NULL, dnx_swstate_switch_header_enablers_hashing_dump_options, &dnx_swstate_switch_header_enablers_hashing_dump_man},
    {"size_get", sh_dnx_swstate_switch_header_enablers_hashing_size_get_cmd, NULL, dnx_swstate_switch_header_enablers_hashing_size_get_options, &dnx_swstate_switch_header_enablers_hashing_size_get_man},
    {"diagnostic_operation_counters", sh_dnx_swstate_switch_header_enablers_hashing_diagnostic_operation_counters_cmd, NULL, dnx_swstate_switch_header_enablers_hashing_diagnostic_operation_counters_options, &dnx_swstate_switch_header_enablers_hashing_diagnostic_operation_counters_man},
    {NULL}
};


sh_sand_man_t dnx_swstate_switch_header_enablers_hashing_man = {
    cmd_dnx_swstate_switch_header_enablers_hashing_desc,
    NULL,
    NULL,
    NULL,
};


const char cmd_dnx_swstate_switch_header_enablers_hashing_desc[] = "switch_db header_enablers_hashing commands";


shr_error_e sh_dnx_swstate_switch_header_enablers_hashing_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{
    int  header_enablers_hashing_idx_0 = 0;

    dnx_sw_state_dump_filters_t filters;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("header_enablers_hashing_idx_0", header_enablers_hashing_idx_0);

    SH_SAND_GET_BOOL("nocontent", filters.nocontent);
    SH_SAND_GET_STR("typefilter", filters.typefilter);
    SH_SAND_GET_STR("namefilter", filters.namefilter);

    SHR_IF_ERR_EXIT(switch_db_header_enablers_hashing_dump(unit, header_enablers_hashing_idx_0, filters));

    if (!filters.nocontent)
    {
        dnx_sw_state_dump_detach_file(unit);

    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_switch_header_enablers_hashing_dump_options[] = {
    {"header_enablers_hashing_idx_0", SAL_FIELD_TYPE_INT32, "index", NULL},
    {"nocontent",      SAL_FIELD_TYPE_BOOL, "Decision if to perform content dump or to print the access tree.",  "false"},
    {"typefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable type.",         ""},
    {"namefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable name.",         ""},
    {NULL}
};


sh_sand_man_t dnx_swstate_switch_header_enablers_hashing_dump_man = {
    "swstate dump",
    "print the variable value",
    "switch_db header_enablers_hashing dump [,header_enablers_hashing_idx_0=<int>]\n",
    "switch_db header_enablers_hashing dump header_enablers_hashing_idx_0=0 nocontent=false typefilter="" namefilter="" ",
};


shr_error_e sh_dnx_swstate_switch_header_enablers_hashing_size_get_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{
    uint32 size = 0;

    SHR_FUNC_INIT_VARS(unit);


    size = dnx_sw_state_info_size_get(switch_db_info[unit], switch_db_layout_str, SWITCH_DB_INFO_NOF_ENTRIES, "SWITCH_DB~HEADER_ENABLERS_HASHING~", sizeof(((switch_sw_state_t*)sw_state_roots_array[unit][SWITCH_MODULE_ID])->header_enablers_hashing));
    dnx_sw_state_size_print(size);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_switch_header_enablers_hashing_size_get_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_switch_header_enablers_hashing_size_get_man = {
    "swstate size_get",
    "print the variable size",
    "switch_db header_enablers_hashing size_get",
    "switch_db header_enablers_hashing size_get",
};


shr_error_e sh_dnx_swstate_switch_header_enablers_hashing_diagnostic_operation_counters_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{

    SHR_FUNC_INIT_VARS(unit);


    dnx_sw_state_diagnostic_operation_counters_print(switch_db_info[unit], switch_db_layout_str, SWITCH_DB_INFO_NOF_ENTRIES, "SWITCH_DB~HEADER_ENABLERS_HASHING~");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



sh_sand_option_t dnx_swstate_switch_header_enablers_hashing_diagnostic_operation_counters_options[] = {
    {NULL}
};


sh_sand_man_t dnx_swstate_switch_header_enablers_hashing_diagnostic_operation_counters_man = {
    "swstate diagnostic_operation_counters",
    "print the diagnostic operation counters",
    "switch_db header_enablers_hashing diagnostic_operation_counters",
    "switch_db header_enablers_hashing diagnostic_operation_counters",
};


#endif /* DNX_SW_STATE_DIAGNOSTIC */
#undef BSL_LOG_MODULE
