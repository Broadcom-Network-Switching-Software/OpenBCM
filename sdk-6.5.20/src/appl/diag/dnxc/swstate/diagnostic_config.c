/** \file diagnostic_config.c
 *
 * sw state diagnostics config definitions
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/* 
 * Note! 
 * This include statement must be at the top of every sw state .c file 
 * It points to a set of in-code compilation flags that must be taken into 
 * account for every sw state componnent compilation 
 */
#include <soc/dnxc/swstate/sw_state_features.h>
/* ---------- */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnxc/swstate/sw_state_defs.h>

#if defined(DNX_SW_STATE_DIAGNOSTIC)
#include <appl/diag/dnxc/swstate/diagnostic_config.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>

sh_sand_cmd_t dnxc_swstate_config_cmds[] = {
    {"dump", sh_dnxc_swstate_config_dump_cmd, NULL, dnxc_swstate_config_dump_options, &dnxc_swstate_config_dump_man}
    ,
    {NULL}
};

sh_sand_man_t dnxc_swstate_config_man = {
    cmd_dnxc_swstate_config_desc,
    NULL,
    NULL,
    NULL,
};

const char cmd_dnxc_swstate_config_desc[] = "swstate config commands";

shr_error_e
sh_dnxc_swstate_config_dump_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *directory;
    char *mode;
    char *file;
    int close_file;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("file_path", file);
    SH_SAND_GET_STR("directory", directory);
    SH_SAND_GET_STR("mode", mode);
    SH_SAND_GET_BOOL("file_unset", close_file);

    if (strcmp(directory, "keep_the_same_path") != 0)
    {
        dnx_sw_state_dump_directory_set(unit, directory);
    }

    if (strcmp(mode, "short") == 0)
    {
        dnx_sw_state_dump_mode_set(unit, DNX_SW_STATE_DUMP_MODE_SHORT);
    }
    else if (strcmp(mode, "full") == 0)
    {
        dnx_sw_state_dump_mode_set(unit, DNX_SW_STATE_DUMP_MODE_FULL);
    }

    if (strcmp(file, "") != 0)
    {
        dnx_sw_state_dump_directory_set(unit, "");
        dnx_sw_state_dump_file_set(unit, file);
    }

    if (close_file)
    {
        dnx_sw_state_dump_file_unset(unit);
    }

exit:
    SHR_FUNC_EXIT;
}

sh_sand_option_t dnxc_swstate_config_dump_options[] = {
    {"file_path", SAL_FIELD_TYPE_STR, "full file path", ""}
    ,
    {"file_unset", SAL_FIELD_TYPE_BOOL, "close the file", "0"}
    ,
    {"directory", SAL_FIELD_TYPE_STR, "full directory path", "keep_the_same_path"}
    ,
    {"mode", SAL_FIELD_TYPE_STR, "print short/full prefix", "keep_the_same_mode"}
    ,
    {NULL}
};

sh_sand_man_t dnxc_swstate_config_dump_man = {
    "swstate config dump",
    "swstate config dump properties",
    "swstate config dump [directory=<string>] [file=<string>] [mode=<string>] [file_unset]",
    "swstate config dump directory=/projects/NTSW_SW_USRS/username/git/master/tmp/sw_state_dump_output",
};

#endif /* DNX_SW_STATE_DIAGNOSTIC */
#undef _ERR_MSG_MODULE_NAME
