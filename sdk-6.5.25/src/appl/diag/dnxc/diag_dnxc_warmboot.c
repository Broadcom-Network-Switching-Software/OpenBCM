/** \file diag_dnxc_warmboot.c
 *
 * Diagnostic pack for warmboot
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_COMMON

#ifdef BCM_WARM_BOOT_SUPPORT
#if ((defined(LINUX) || defined(UNIX)) && (!defined(NO_FILEIO)))
#include <unistd.h>
#endif

/* shared */
#include <shared/bsl.h>

/* appl */
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/diag.h>
#include <appl/diag/dport.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_utils.h>

/* bcm */
#include <bcm/switch.h>

/* soc */
#include <soc/drv.h>
#include <soc/dnxc/multithread_analyzer.h>

#include <soc/dnxc/swstate/dnxc_sw_state_wb.h>
#include <soc/dnxc/dnxc_wb_test_utils.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>

/*
 * warmboot on command functions
 */
static shr_error_e
cmd_dnxc_warmboot_on(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SOC_WARM_BOOT_START(SOC_NDEV_IDX2DEV(unit));
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/*
 * warmboot off command functions
 */
static shr_error_e
cmd_dnxc_warmboot_off(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SOC_WARM_BOOT_DONE(SOC_NDEV_IDX2DEV(unit));
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * warmboot sync command functions
 */
static shr_error_e
cmd_dnxc_warmboot_sync(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    cli_out("syncing state to WB external storage.\n");
    if (bcm_switch_control_set(unit, bcmSwitchControlSync, 1) != _SHR_E_NONE)
    {
        cli_out("bcm_switch_control_set with bcmSwitchControlSync failed");
        SHR_EXIT();
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * warmboot memoryuse command functions
 */
static shr_error_e
cmd_dnxc_warmboot_memoryuse(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
#if (defined(LINUX) || defined(UNIX))
#ifndef NO_FILEIO
    int values[7];
    int i = 0;
    int page_size = getpagesize();
    FILE *status = NULL;
#endif
#endif

    SHR_FUNC_INIT_VARS(unit);

#if ((defined(LINUX) || defined(UNIX)) && (!defined(NO_FILEIO)))
    status = sal_fopen("/proc/self/statm", "r");
    if (!status)
    {
        cli_out("Error: Unable to open statm file");
        SHR_EXIT();
    }

    cli_out("\n for WarmBoot external storage sizes chek your wb and kaps wb and kbp wb file sizes.\n");

    while (fscanf(status, "%d", &values[i]) == 1)
    {
        i++;
    };
    sal_printf("Total process size:   %d MB\n", BYTES_TO_MBS(values[0] * page_size));
    sal_printf("  -data+stack:        %d MB\n", BYTES_TO_MBS(values[5] * page_size));
    sal_printf("  -text:              %d MB\n", BYTES_TO_MBS(values[3] * page_size));
    sal_printf("  -resident set size: %d MB\n", BYTES_TO_MBS(values[1] * page_size));
    sal_printf("  -shared:            %d MB\n", BYTES_TO_MBS(values[2] * page_size));
    sal_printf("Memory usage:         %d MB\r\n\n", BYTES_TO_MBS(values[0] * page_size));

    sal_fclose((FILE *) status);
    SHR_EXIT();

#else
    cli_out("\nThis command is not supported with this compilation.\n");
#endif

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * warmboot sw_state_size command functions
 */
static shr_error_e
cmd_dnxc_warmboot_sw_state_size(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 left = 0;
    uint32 in_use = 0;
    int rv = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);

    rv = dnxc_sw_state_wb_sizes_get(unit, &in_use, &left);
    if (rv == _SHR_E_UNAVAIL)
    {
        cli_out("Warmboot is not enabled hence there is no bound on SW State's size.\n");
        SHR_EXIT();
    }
    if (left > 1048576 && in_use > 1048576)
    {
        cli_out("total: %d MB. (%d bytes)\n", BYTES_TO_MBS(left) + BYTES_TO_MBS(in_use), left + in_use);
        cli_out("in use: %d MB. (%d bytes)\n", BYTES_TO_MBS(in_use), in_use);
        cli_out("left: %d MB. (%d bytes)\n", BYTES_TO_MBS(left), left);
    }
    else
    {
        cli_out("total: %dKB. (%d bytes)\n", BYTES_TO_KBS(left) + BYTES_TO_KBS(in_use), left + in_use);
        cli_out("in use: %dKB. (%d bytes)\n", BYTES_TO_KBS(in_use), in_use);
        cli_out("left: %dKB. (%d bytes)\n", BYTES_TO_KBS(left), left);
    }

    cli_out("\nnote: you can also try swstate size_get command tog get individual modules memory consumption.\n");

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static int
diag_dnxc_warmboot_test_tr_141_command(
    int unit)
{
    return sh_process_command(unit, "tr 141 w=1");
}

static int
diag_dnxc_warmboot_test_tr_141_nodump_command(
    int unit)
{
    return sh_process_command(unit, "tr 141 w=1 nodump=1");
}

/*
 * add documentation here
 */
static shr_error_e
cmd_dnxc_warmboot_test_mode_on(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int dump;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("dump", dump);

    dnxc_wb_test_mode_set(unit, DNXC_WARM_BOOT_API_TEST_MODE_AFTER_EVERY_API);
    if (dump)
    {
        dnxc_warmboot_test_tr_141_command_set(unit, diag_dnxc_warmboot_test_tr_141_command);
    }
    else
    {
        dnxc_warmboot_test_tr_141_command_set(unit, diag_dnxc_warmboot_test_tr_141_nodump_command);
    }
    dnxc_wb_test_callback_unregister(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * add documentation here
 */
static shr_error_e
cmd_dnxc_warmboot_test_mode_off(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    SHR_FUNC_INIT_VARS(unit);

    dnxc_wb_test_mode_set(unit, DNXC_WARM_BOOT_API_TEST_MODE_NONE);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * add documentation here
 */
static shr_error_e
cmd_dnxc_warmboot_test_mode_filter(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 min = 0;
    uint32 max = 0;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("min", min);
    SH_SAND_GET_UINT32("max", max);

    dnxc_wb_api_test_filter_set(unit, 0, min);
    dnxc_wb_api_test_filter_set(unit, 1, max);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * add documentation here
 */
static shr_error_e
cmd_dnxc_warmboot_test_mode_skip(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    SHR_FUNC_INIT_VARS(unit);

    cli_out("ERROR: skip command is not supported anymore.\n");

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * add documentation here
 */
static shr_error_e
cmd_dnxc_warmboot_test_mode_no_wb_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    SHR_FUNC_INIT_VARS(unit);

    dnxc_wb_no_wb_test_set(unit, 1);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * add documentation here
 */
static shr_error_e
cmd_dnxc_warmboot_test_mode_allow_wb_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    SHR_FUNC_INIT_VARS(unit);

    dnxc_wb_no_wb_test_set(unit, 0);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


/* *INDENT-OFF* */

sh_sand_man_t sh_dnx_warmboot_test_mode_man = {
    .brief =    "Various Warmboot test mode controls.",
    .full =     "This command is used to control warmboot test_mode\n"
                "A special mode that is used in warmboot regression\n"
                "to perform warm reboot simulation after every API call\n",
};

sh_sand_man_t sh_dnx_warmboot_memoryuse_man = {
    .brief =    "Show memory usage of the current proccess.",
};

sh_sand_man_t sh_dnx_warmboot_sw_state_size_man = {
    .brief =    "Show the in-use sw state size and the remaining sw state size.",
    .full = "When running with warmboot_support=on, sw state is\n"
            "pre-allocating a large chunk of memory at the beginning of init for\n"
            "the different modules to consume during device init, the size of\n"
            "the pre-allocated memory is configurable with soc prop sw_state_max_size=<number>\n"
            "This diag will show the amount of memory in-use outside of the pre-allocate block\n",
};

sh_sand_man_t sh_dnx_warmboot_sync_man = {
    .brief =    "Sync the system state to external NV storage.",
};

sh_sand_man_t sh_dnx_warmboot_off_man = {
    .brief =    "raise a flag to indicate the system is not under warm reboot.",
};

sh_sand_man_t sh_dnx_warmboot_on_man = {
    .brief =    "raise a flag to indicate the system is under warm reboot.",
};

sh_sand_man_t sh_dnxc_warmboot_man = {
    .brief =    "Various Warmboot controls.",
};

sh_sand_man_t sh_dnxc_warmboot_test_mode_on_man = {
    .brief =    "Enable warmboot test mode.",
    .full =     "(perform reboot after every API call)\n",
    .examples = "dump=false\n"
};

sh_sand_man_t sh_dnxc_warmboot_test_mode_off_man = {
    .brief =    "Disable warmboot test mode.",
};

sh_sand_man_t sh_dnxc_warmboot_test_mode_skip_test_man = {
    .brief =    "This command is deprecated and is not supported anymore.",
};

sh_sand_man_t sh_dnxc_warmboot_test_mode_filter_test_man = {
    .brief =    "filter the APIs that get wb test (filter is min/max on the counter).",
    .examples = "min=3 max=7\n"
};

sh_sand_man_t sh_dnxc_warmboot_test_mode_no_wb_test_man = {
    .brief =    "Temporarily disable warm reboot tests.",
    .full =     "(until warmboot test_mode allow_wb_test is called)\n",
};

sh_sand_man_t sh_dnxc_warmboot_test_mode_allow_wb_test_man = {
    .brief =    "revert a former no_wb_test call.",
};

static sh_sand_option_t cmd_dnxc_warmboot_filter_options[] = {
    {"min",               SAL_FIELD_TYPE_UINT32, "the min test conter to perform test on", "0"},
    {"max",               SAL_FIELD_TYPE_UINT32, "the max test conter to perform test on", "0"},
    {NULL}
};

static sh_sand_option_t cmd_dnxc_warmboot_on_options[] = {
    {"dump",               SAL_FIELD_TYPE_BOOL, "decision if to perform dump during tr 141 w=1 test", "true"},
    {NULL}
};

sh_sand_cmd_t sh_dnxc_warmboot_test_mode_cmds[] = {
    /*
     * keyword, action, command, options, man callback legacy
     */
    {"on",   cmd_dnxc_warmboot_test_mode_on,   NULL,   cmd_dnxc_warmboot_on_options,   &sh_dnxc_warmboot_test_mode_on_man},
    {"off",   cmd_dnxc_warmboot_test_mode_off,   NULL,   NULL,   &sh_dnxc_warmboot_test_mode_off_man},
    {"skip",   cmd_dnxc_warmboot_test_mode_skip,   NULL,   NULL,   &sh_dnxc_warmboot_test_mode_skip_test_man},
    {"filter",   cmd_dnxc_warmboot_test_mode_filter,   NULL,   cmd_dnxc_warmboot_filter_options,   &sh_dnxc_warmboot_test_mode_filter_test_man},
    {"no_wb_test",   cmd_dnxc_warmboot_test_mode_no_wb_test,   NULL,   NULL,   &sh_dnxc_warmboot_test_mode_no_wb_test_man},
    {"allow_wb_test",   cmd_dnxc_warmboot_test_mode_allow_wb_test,   NULL,   NULL,   &sh_dnxc_warmboot_test_mode_allow_wb_test_man},
    {NULL}
};

sh_sand_cmd_t sh_dnxc_warmboot_cmds[] = {
    /*
     * keyword, action, command, options, man callback legacy
     */
    {"on",   cmd_dnxc_warmboot_on,   NULL,   NULL,   &sh_dnx_warmboot_on_man},
    {"off",   cmd_dnxc_warmboot_off,   NULL,   NULL,   &sh_dnx_warmboot_off_man},
    /*
     * Skip sync when running ctest as it locks the KBPSDK
     */
    {"sync",   cmd_dnxc_warmboot_sync,   NULL, NULL, &sh_dnx_warmboot_sync_man, NULL, NULL, SH_CMD_SKIP_EXEC},
    {"memoryuse",   cmd_dnxc_warmboot_memoryuse,   NULL,   NULL,   &sh_dnx_warmboot_memoryuse_man},
    {"sw_state_size",   cmd_dnxc_warmboot_sw_state_size,   NULL,   NULL,   &sh_dnx_warmboot_sw_state_size_man},
    {"test_mode",   NULL,   sh_dnxc_warmboot_test_mode_cmds,   NULL,   &sh_dnx_warmboot_test_mode_man},
    {NULL}
};
#else /* BCM_WARM_BOOT_SUPPORT */
/**
 * This is DUMMY code. It is only for compilers that do not accept empty files
 * and is never to be used.
 */
int
cmd_dnxc_warmboot_dummy_empty_function_to_make_compiler_happy(
    int unit)
{
    return 0;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

/* *INDENT-ON* */
