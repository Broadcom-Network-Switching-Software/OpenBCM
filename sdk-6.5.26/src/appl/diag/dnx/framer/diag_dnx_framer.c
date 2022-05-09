/**
 * \file diag_dnx_framer.c Contains all of the framer diag commands
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_INTR
#include <shared/bsl.h>

/*************
 * INCLUDES  *
 *************/
#include <appl/diag/diag.h>

#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/utilex/utilex_bitstream.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnxc/drv_dnxc_utils.h>

#include <bcm_int/dnx/init/init.h>
#ifndef NO_FRAMER_LIB_BUILD
#include "diag_framer_regram_test.h"
#endif /* NO_FRAMER_LIB_BUILD */

/*************
 * DEFINES   *
 *************/
#define DNX_DIAG_FRAMER_UCINT_MAX_NOF_FILES      (20)
#define DNX_DIAG_FRAMER_UCINT_MAX_LENGTH         (100)
#define DNX_DIAG_FRAMER_LOGSTORE_MAX_STR_LENGTH  (10)

/*************
* FUNCTIONS *
*************/

#ifndef NO_FRAMER_LIB_BUILD
extern void debug_cmd_list_run(
    int argc,
    char *argv[]);
extern cmd_result_t cmd_dump(
    int unit,
    args_t * a);
extern char cmd_dump_usage[];

extern cmd_result_t cmd_write(
    int unit,
    args_t * a);
extern char cmd_write_usage[];

extern cmd_result_t cmd_modify(
    int unit,
    args_t * a);
extern char cmd_modify_usage[];

extern cmd_result_t cmd_list(
    int unit,
    args_t * a);
extern char cmd_list_usage[];

extern cmd_result_t cmd_reg_get(
    int unit,
    args_t * a);
extern char cmd_reg_get_usage[];

extern cmd_result_t cmd_reg_set(
    int unit,
    args_t * a);
extern char cmd_reg_set_usage[];

extern cmd_result_t cmd_reg_modify(
    int unit,
    args_t * a);
extern char cmd_reg_modify_usage[];

extern cmd_result_t cmd_testrun(
    int unit,
    args_t * a);
extern char cmd_testrun_usage[];
#endif /* NO_FRAMER_LIB_BUILD */
/*
 * See .h file
 */
shr_error_e
sh_cmd_is_framer_available(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Not supported for devices without flexe support, don't skip if deliberately including adapter unsupported tests */
    if (!dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_is_supported)
        && !(test_list && test_list->include_unsupported))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_UNIT);
    }

    SHR_FUNC_EXIT;
}

#ifndef NO_FRAMER_LIB_BUILD
static shr_error_e
dnx_diag_framer_debug_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    ARG_PREV(args);
    debug_cmd_list_run(ARG_CNT(args) - 1, &args->a_argv[args->a_arg]);

    SHR_FUNC_EXIT;
}

static sh_sand_man_t dnx_diag_framer_debug_man = {
    .brief = "Debug command for Framer \n",
    .full = NULL
};

static shr_error_e
dnx_diag_framer_dump_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    (void) cmd_dump(unit, args);

    SHR_FUNC_EXIT;
}

static sh_sand_man_t dnx_diag_framer_dump_man = {
    .brief = "Dump memories of Framer\n",
    .full = cmd_dump_usage
};

static shr_error_e
dnx_diag_framer_write_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    (void) cmd_write(unit, args);

    SHR_FUNC_EXIT;
}

static sh_sand_man_t dnx_diag_framer_write_man = {
    .brief = "Write memories of Framer\n",
    .full = cmd_write_usage
};

static shr_error_e
dnx_diag_framer_modify_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    (void) cmd_modify(unit, args);

    SHR_FUNC_EXIT;
}

static sh_sand_man_t dnx_diag_framer_modify_man = {
    .brief = "Modify memories of Framer\n",
    .full = cmd_modify_usage
};

static shr_error_e
dnx_diag_framer_list_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    (void) cmd_list(unit, args);

    SHR_FUNC_EXIT;
}

static sh_sand_man_t dnx_diag_framer_list_man = {
    .brief = "LIST register/memories of Framer\n",
    .full = cmd_list_usage
};

static shr_error_e
dnx_diag_framer_getreg_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    (void) cmd_reg_get(unit, args);

    SHR_FUNC_EXIT;
}

static sh_sand_man_t dnx_diag_framer_getreg_man = {
    .brief = "Get registers of Framer\n",
    .full = cmd_reg_get_usage
};

static shr_error_e
dnx_diag_framer_setreg_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    (void) cmd_reg_set(unit, args);

    SHR_FUNC_EXIT;
}

static sh_sand_man_t dnx_diag_framer_setreg_man = {
    .brief = "Set registers of Framer\n",
    .full = cmd_reg_set_usage
};

static shr_error_e
dnx_diag_framer_modreg_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    (void) cmd_reg_modify(unit, args);

    SHR_FUNC_EXIT;
}

static sh_sand_man_t dnx_diag_framer_modreg_man = {
    .brief = "Modify registers of Framer\n",
    .full = cmd_reg_modify_usage
};

static shr_error_e
dnx_diag_framer_testrun_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    (void) cmd_testrun(unit, args);

    SHR_FUNC_EXIT;
}

static sh_sand_man_t dnx_diag_framer_testrun_man = {
    .brief = "TestRun a case on registers/memories.\n",
    .full = cmd_testrun_usage
};

static sh_sand_man_t dnx_diag_framer_regram_test_man = {
    .brief = "List FlexE core register/memory infomation\n",
    .examples = "list MODULE=acc_reg memory\n" "dump MODULE=acc_reg register UPDate PatTeRN=0xff DeBuG=1"
};

static sh_sand_option_t dnx_diag_framer_regram_test_options[] = {

    {"type", SAL_FIELD_TYPE_STR, "Framer test type", "", NULL, NULL, SH_SAND_ARG_FREE},
    {"MODULE", SAL_FIELD_TYPE_STR, "Framer module name", ""},
    {"register", SAL_FIELD_TYPE_BOOL, "specify that if test register", "No"},
    {"memory", SAL_FIELD_TYPE_BOOL, "specify that if test memory", "No"},
    {"UPDate", SAL_FIELD_TYPE_BOOL, "specify if write reg/mem before dump", "No"},
    {"PatTeRN", SAL_FIELD_TYPE_UINT32, "Pattern value for reg mem write", "0"},
    {"DeBuG", SAL_FIELD_TYPE_UINT32, "Debug parameter", "0"},
    {NULL}
};

static shr_error_e
dnx_diag_framer_regram_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *test_type;
    char *module_name;
    int reg, mem, if_update, pattern_present;
    uint32 flags = 0, pattern, debug;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("type", test_type);
    SH_SAND_GET_STR("MODULE", module_name);
    SH_SAND_GET_BOOL("register", reg);
    SH_SAND_GET_BOOL("memory", mem);
    SH_SAND_GET_BOOL("UPDate", if_update);
    SH_SAND_GET_UINT32("PatTeRN", pattern);
    SH_SAND_GET_UINT32("DeBuG", debug);
    SH_SAND_IS_PRESENT("PatTeRN", pattern_present);

    flags = 0;
    if (if_update)
    {
        flags |= DIAG_FRAMER_TEST_FLAG_SET;
    }
    if (pattern_present)
    {
        flags |= DIAG_FRAMER_TEST_FLAG_SET_PATTEN;
    }
    if (reg)
    {
        flags |= DIAG_FRAMER_TEST_FLAG_REG;
    }
    if (mem)
    {
        flags |= DIAG_FRAMER_TEST_FLAG_MEM;
    }
    if (debug)
    {
        flags |= DIAG_FRAMER_TEST_FLAG_DEBUG_PRINT;
    }
    cli_out("test_type=%s, module_name=%s, reg=%d, mem=%d, flags=0x%x, pattern=0x%x\n",
            test_type, module_name, reg, mem, flags, pattern);
    if (test_type)
    {
        if (sal_strncasecmp(test_type, "list", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
        {
            SHR_IF_ERR_EXIT(diag_framer_test_list_module(unit, flags, module_name, pattern, sand_control));
        }
        else if (sal_strncasecmp(test_type, "dump", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
        {
            flags |= DIAG_FRAMER_TEST_FLAG_SKIP;
            if (reg)
            {
                SHR_IF_ERR_EXIT(diag_framer_test_reg_default(unit, flags, module_name, pattern, sand_control));
            }
            if (mem)
            {
                SHR_IF_ERR_EXIT(diag_framer_test_ram_default(unit, flags, module_name, pattern, sand_control));
            }
        }
    }

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_FUNC_EXIT;
}

/**
 * List of the supported commands, pointer to command function and command usage function.
 */
sh_sand_man_t sh_dnx_diag_framer_man = {
    .brief = "Framer commands",
    .full = "The dispatcher for the different Framer diagnostics commands\n"
};

/**
 * \brief DNX Framer diagnostics
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for flexe diagnostic commands
 */
sh_sand_cmd_t sh_dnx_diag_framer_cmds[] = {
    /*
     * Name |    Leaf Action | Junction Array | Options for Leaf | Usage | Options call back for Leaf | Invoke | flags
     */
    {"debug", dnx_diag_framer_debug_cmd, NULL, NULL, &dnx_diag_framer_debug_man, NULL, NULL, SH_CMD_LEGACY}
    ,
    {"Dump", dnx_diag_framer_dump_cmd, NULL, NULL, &dnx_diag_framer_dump_man, NULL, NULL, SH_CMD_LEGACY}
    ,
    {"Get", dnx_diag_framer_getreg_cmd, NULL, NULL, &dnx_diag_framer_getreg_man, NULL, NULL, SH_CMD_LEGACY}
    ,
    {"LIST", dnx_diag_framer_list_cmd, NULL, NULL, &dnx_diag_framer_list_man, NULL, NULL, SH_CMD_LEGACY}
    ,
    {"MODify", dnx_diag_framer_modify_cmd, NULL, NULL, &dnx_diag_framer_modify_man, NULL, NULL, SH_CMD_LEGACY}
    ,
    {"Modreg", dnx_diag_framer_modreg_cmd, NULL, NULL, &dnx_diag_framer_modreg_man, NULL, NULL, SH_CMD_LEGACY}
    ,
    {"Set", dnx_diag_framer_setreg_cmd, NULL, NULL, &dnx_diag_framer_setreg_man, NULL, NULL, SH_CMD_LEGACY}
    ,
    {"TestRun", dnx_diag_framer_testrun_cmd, NULL, NULL, &dnx_diag_framer_testrun_man, NULL, NULL, SH_CMD_LEGACY}
    ,
    {"TeST", dnx_diag_framer_regram_test_cmd, NULL, dnx_diag_framer_regram_test_options,
     &dnx_diag_framer_regram_test_man}
    ,
    {"Write", dnx_diag_framer_write_cmd, NULL, NULL, &dnx_diag_framer_write_man, NULL, NULL, SH_CMD_LEGACY}
    ,
    {NULL}
};
#endif /* NO_FRAMER_LIB_BUILD */
