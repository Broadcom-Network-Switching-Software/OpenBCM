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

#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif

#include <soc/drv.h>
#include <appl/diag/diag.h>

#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/utilex/utilex_bitstream.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnxc/drv_dnxc_utils.h>

#include <bcm_int/dnx/init/init.h>
#ifndef NO_FRAMER_LIB_BUILD
#endif /* NO_FRAMER_LIB_BUILD */

#ifndef NO_FRAMER_LIB_BUILD
extern void debug_cmd_list_run(
    int argc,
    char *argv[]);
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

    if (SOC_IS_DNX(unit))
    {
        /** Not supported for devices without flexe support, don't skip if deliberately including adapter unsupported tests */
        if (!dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_is_supported)
            && !(test_list && test_list->include_unsupported))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_UNIT);
        }
    }
    else
    {
        SHR_SET_CURRENT_ERR(_SHR_E_UNIT);
    }
    SHR_EXIT();
exit:
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
    {NULL}
};
#endif /* NO_FRAMER_LIB_BUILD */
