/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:    extdpll.c
 * Purpose: IDT Clock management diag shell commands
 */

#include <bcm/error.h>
#include <shared/bsl.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <soc/iproc.h>

#ifdef NO_SAL_APPL
#include <string.h>
#else
#include <sal/appl/sal.h>
#endif

#ifdef BCM_EXT_DPLL_DRV_SUPPORT
#include "idt_clockmatrix_api/idt_clockmatrix_api.h"

#if defined(UNIX) && !defined(__KERNEL__)
#include <time.h>
#endif

char cmd_extdpll_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "Usage: extdpll <option> [args...]\n"
#else
    "\nUsage: \n"
    "   extdpll config <TCS_file_name>\n"
    "   extdpll chandump <chan_id 0:7> - Dump the channel status\n"
    "   extdpll activeref  - Dump the Active reference of all the channels\n"
    "   extdpll qual       - Dump the Qualification status of all references\n"
    "   extdpll debug      - Dump the complete debug info\n"
    "   Note:[Please refer 'idt_clockmatrix_api.h' for interpretations]\n\n"
#endif
    ;

cmd_result_t cmd_extdpll(int unit, args_t *args)
{
    char *arg_string_p = NULL;
    Channel_id_t dpll_ch_id;
    idt_status_t rv;
    PLL_ref_id_t ref_id;
    long int chan;

    arg_string_p = ARG_GET(args);
    if (arg_string_p == NULL) {
        return CMD_USAGE;
    }

    if (!sh_check_attached(ARG_CMD(args), unit)) {
        return CMD_FAIL;
    }

    if (parse_cmp("Activeref", arg_string_p, 0)) {
        cli_out("\nStatus: Channel_id_t / PLL_ref_id_t\n");
        for (dpll_ch_id = CHANNEL_ID_0; dpll_ch_id < CHANNEL_ID_MAX; dpll_ch_id++) {
            rv = PLL_get_dpll_mode_ref_sel(dpll_ch_id, &ref_id);
            if (rv == IDT_EOK) {
                cli_out("   Channel: %d   Ref: %d\n", dpll_ch_id, ref_id);
            } else {
                cli_out("   Channel: %d   PLL_get_dpll_mode_ref_sel() failed with ret: %d\n", dpll_ch_id, rv);
            }
        }

    } else if (parse_cmp("Config", arg_string_p, 0)) {
        char const *tcs_fname;
        arg_string_p = ARG_GET(args);
        if (arg_string_p == NULL) {
            return CMD_USAGE;
        }
        tcs_fname = arg_string_p;

        cli_out("\nTCS file programming\n");
        rv = PLL_reg_static_config(tcs_fname/*NULL char const *tcs_file_name*/);
        if (IDT_EOK != rv) {
            cli_out("### ERROR PLL_reg_static_config(): ret:%d\n", rv);
            return(CMD_EXIT);
        }
        cli_out("### PLL_reg_static_config() Done!!\n");

    } else if (parse_cmp("Chandump", arg_string_p, 0)) {
        arg_string_p = ARG_GET(args);
        if (arg_string_p == NULL) {
            return CMD_USAGE;
        }
        if (!isint(arg_string_p)) {
            cli_out("%s: Error: Channel number \n", ARG_CMD(args));
            return(CMD_USAGE);
        }
        chan = parse_integer(arg_string_p);

        cli_out("\nChannel status: DEBUG_INFO_CHANNEL_0 -> DEBUG_INFO_CHANNEL_7 for channel:%d\n", (int)chan);
        rv = PLL_debug_info(DEBUG_INFO_CHANNEL_0+chan);
        if (rv != IDT_EOK)
            cli_out("   Chan: %d   PLL_debug_info() Failed with ret: %d\n", (int)chan, rv);

    } else if (parse_cmp("Debug", arg_string_p, 0)) {
        /*PLL_debug_info(DEBUG_INFO_GENERAL_STATUS);*/
        /*PLL_debug_info(DEBUG_INFO_STATUS);*/
        PLL_debug_info(DEBUG_INFO_ALL);

    } else if (parse_cmp("Qual", arg_string_p, 0)) {
        bool stat;
        cli_out("\nQualified References: PLL_ref_id_t / bool(yes:no)\n");
        for (ref_id = PLL_REF_ID_0; ref_id <= PLL_REF_ID_15; ref_id++) {
            rv = PLL_is_ref_qualified(ref_id, &stat);
            if (rv == IDT_EOK) {
                cli_out("   Ref: %d   Qual: %d\n", ref_id, stat);
            } else {
                cli_out("   Ref: %d   PLL_is_ref_qualified() failed with ret: %d\n", ref_id, rv);
            }
        }

    } else {
        cli_out("%s: Error: extdpll command\n", ARG_CMD(args));
        return(CMD_USAGE);
    }

    return CMD_OK;
}
#endif
