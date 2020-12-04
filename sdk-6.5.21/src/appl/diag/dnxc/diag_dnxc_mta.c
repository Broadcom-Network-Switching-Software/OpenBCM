/** \file diag_dnxc_mta.c
 *
 * Diagnostic pack for multi thread analyzer
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_COMMON

/* shared */
#include <shared/bsl.h>

/* appl */
#include <appl/diag/sand/diag_sand_framework.h>

/* soc */
#include <soc/drv.h>
#include <soc/dnxc/multithread_analyzer.h>
#include <soc/dnxc/swstate/dnxc_sw_state_wb.h>
#include <soc/dnxc/dnxc_wb_test_utils.h>

/*
 * add documentation here
 */
shr_error_e
cmd_dnxc_mta_start(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
#if defined(DNXC_MTA_ENABLED)
    int backgroundthreads;
    int mainthread;
    int memreg;
    int tcl;
    uint32 allocmgr;
    uint32 flags;
#endif

    SHR_FUNC_INIT_VARS(unit);

#if defined(DNXC_MTA_ENABLED)
    SH_SAND_GET_BOOL("mainthread", mainthread);
    SH_SAND_GET_BOOL("memreg", memreg);
    SH_SAND_GET_BOOL("backgroundthreads", backgroundthreads);
    SH_SAND_GET_BOOL("tcl", tcl);
    SH_SAND_GET_UINT32("ALLocaTion", allocmgr);

    if (tcl)
    {
        flags = MTA_TCL_FLAG_SET;
    }
    else
    {
        flags = (memreg ? MTA_FLAG_DONT_LOG_MEM_REG : 0) |
            (allocmgr == 1 ? MTA_FLAG_DONT_LOG_ALLOC_MNGR : 0) |
            (allocmgr == 2 ? MTA_FLAG_LOG_ONLY_ALLOC_MNGR : 0) |
            (mainthread ? MTA_FLAG_LOG_ONLY_BACKGROUND : 0) | (backgroundthreads ? MTA_FLAG_LOG_ONLY_MAIN : 0);
    }

    dnxc_multithread_analyzer_start(unit, flags);
#endif

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * add documentation here
 */
shr_error_e
cmd_dnxc_mta_stop(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
#if defined(DNXC_MTA_ENABLED)
    int screen = 0;
    char *filename;
#endif

    SHR_FUNC_INIT_VARS(unit);

#if defined(DNXC_MTA_ENABLED)
    SH_SAND_GET_STR("FILENaMe", filename);
    SH_SAND_GET_BOOL("screen", screen);
#endif

    DNXC_MTA(dnxc_multithread_analyzer_generate_summary_files(unit, screen ? MTA_FLAG_PRINT_TO_SCREEN : 0, filename));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * add documentation here
 */
shr_error_e
cmd_dnxc_mta_mark(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
#if defined(DNXC_MTA_ENABLED)
    char *start;
    char *stop;
#endif

    SHR_FUNC_INIT_VARS(unit);

#if defined(DNXC_MTA_ENABLED)
    SH_SAND_GET_STR("start", start);
    SH_SAND_GET_STR("stop", stop);

    if (sal_strncmp(start, "none", 4) != 0)
    {
        DNXC_MTA(dnxc_multithread_analyzer_declare_api_in_play(unit, start, MTA_FLAG_CINT, TRUE));
    }

    if (sal_strncmp(stop, "none", 4) != 0)
    {
        DNXC_MTA(dnxc_multithread_analyzer_declare_api_in_play(unit, stop, MTA_FLAG_CINT, FALSE));
    }
#endif

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
sh_sand_man_t sh_dnxc_mta_man = {
    .brief =    "Various Multithread Analyzer controls.",
    .full =     "This command is used to control Multithread analyzer\n"
                "A special tool to analyze resource access by different threads\n",
};

sh_sand_man_t sh_dnxc_mta_start_man = {
    .brief =    "Start Multithread Analyzer.",
    .examples = "mainthread\n"
                "backgroundthreads\n"
                "memreg\n"
                "ALLocaTion=1\n"
                "tcl\n"
};

sh_sand_man_t sh_dnxc_mta_stop_man = {
    .brief =    "Stop Multithread Analyzer.",
    .examples = "screen\n"
                "filename=my_example_file.txt\n"
};

sh_sand_man_t sh_dnxc_mta_mark_man = {
    .brief =    "Mark cint start/end to be logged upon resource access.",
    .examples = "start=name_of_cint_to_be_marked\n"
                "stop=name_of_cint_to_be_marked\n"
};

static sh_sand_option_t cmd_dnxc_mta_start_options[] = {
    {"tcl",                 SAL_FIELD_TYPE_BOOL,   "overide flags with MTA_TCL_FLAG_SET", "false"},
    {"mainthread",          SAL_FIELD_TYPE_BOOL,   "mask logging for main thread",        "false"},
    {"backgroundthreads",   SAL_FIELD_TYPE_BOOL,   "mask logging for background threads", "false"},
    {"memreg",              SAL_FIELD_TYPE_BOOL,   "mask logging for mems & regs",        "false"},
    {"ALLocaTion",          SAL_FIELD_TYPE_UINT32, "1: mask logging for alloc mngr 2: log only alloc mngr", "0"},
    {NULL}
};

static sh_sand_option_t cmd_dnxc_mta_stop_options[] = {
    {"FILENaMe", SAL_FIELD_TYPE_STR,  "multi thread analyzer output file name", "access_log.csv"},
    {"screen",   SAL_FIELD_TYPE_BOOL, "print to screen instead of to file",     "false"},
    {NULL}
};

static sh_sand_option_t cmd_dnxc_mta_mark_options[] = {
    {"start", SAL_FIELD_TYPE_STR, "call it right before cint call", "none"},
    {"stop",  SAL_FIELD_TYPE_STR, "call it right after cint call",   "none"},
    {NULL}
};

sh_sand_cmd_t sh_dnxc_mta_cmds[] = {
    /*
     * keyword, action, command, options, man callback legacy
     */
    {"start",     cmd_dnxc_mta_start,     NULL,   cmd_dnxc_mta_start_options,     &sh_dnxc_mta_start_man},
    {"stop",      cmd_dnxc_mta_stop,      NULL,   cmd_dnxc_mta_stop_options,      &sh_dnxc_mta_stop_man},
    {"mark",      cmd_dnxc_mta_mark,      NULL,   cmd_dnxc_mta_mark_options,      &sh_dnxc_mta_mark_man},
    {NULL}
};

/* *INDENT-ON* */
