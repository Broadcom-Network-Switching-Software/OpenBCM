/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    ukernel_debug.c
 * Purpose:  
 */

#include <stddef.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <sal/core/boot.h>
#include <sal/core/dpc.h>
#include <sal/core/time.h>
#include <appl/diag/system.h>
#include <appl/diag/dport.h>
#include <bcm/error.h>
#include <shared/pack.h>

#if defined(UNIX) && !defined(__KERNEL__)
#include <time.h>
#include <stdlib.h>
#endif

#if defined(SOC_UKERNEL_DEBUG)
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT)
#include <soc/uc_dbg.h>
#include <appl/diag/ukernel_debug.h>

#define UCDBG_COMMAND_USAGE \
    "ucdbg <subcommand>\n" \
    "\t ucdbg start     starts showing ukernel debug log\n" \
    "\t ucdbg stop      stops showing ukernel debug log\n" \
    "\t ucdbg status    shows ukernel debug module status\n"

char cmd_cmic_ucdebug_usage[] = "Usages:\n"
#ifdef COMPILER_STRING_CONST_LIMIT
    "   ucdbg <option> [args...]\n"
#else
    "   " UCDBG_COMMAND_USAGE "\n"
#endif /* COMPILER_STRING_CONST_LIMIT */
    ;


cmd_result_t 
cmd_cmic_ucdebug_dump_start(int unit, args_t *a)
{
    int rval = 0;
    
    rval = soc_cmic_ucdebug_dump_start(unit);
    switch (rval) {
        case BCM_E_UNAVAIL:
            return (CMD_NOTIMPL);
            break;
        case BCM_E_FAIL:
            return (CMD_FAIL);
    }
    return (CMD_OK);
}

cmd_result_t
cmd_cmic_ucdebug_dump_stop(int unit, args_t *a)
{
    int rval = 0;
    
    rval = soc_cmic_ucdebug_dump_stop(unit);
    switch (rval) {
        case BCM_E_UNAVAIL:
            return (CMD_NOTIMPL);
            break;
        case BCM_E_FAIL:
            return (CMD_FAIL);
    }
    return (CMD_OK);

}

cmd_result_t
cmd_cmic_ucdebug_status(int unit, args_t *a)
{
    int rval = 0;
    
    rval = soc_cmic_ucdebug_status(unit);
    switch (rval) {
        case BCM_E_UNAVAIL:
            return (CMD_NOTIMPL);
            break;
        case BCM_E_FAIL:
            return (CMD_FAIL);
    }
    return (CMD_OK);
}

/************* Main cmic_uc Debug command *******************/    
typedef struct {
    char *str;
    cmd_result_t (*func)(int unit, args_t *args);
} uc_debug_subcommand_t;

cmd_result_t
cmd_cmic_ucdebug(int unit, args_t *a)
{
    uc_debug_subcommand_t subcommands[] = {
#ifndef __KERNEL__
        {"start", cmd_cmic_ucdebug_dump_start},
        {"stop", cmd_cmic_ucdebug_dump_stop},
        {"status", cmd_cmic_ucdebug_status},
#endif
    };
    int i;
    const char *arg;

    if(!soc_feature(unit, soc_feature_ukernel_debug)) {
        return (CMD_NOTIMPL);
    }
    arg = ARG_GET(a);
    if (!arg) {
       return (CMD_USAGE);
    }

    for (i = 0; i < sizeof(subcommands) / sizeof(subcommands[0]); ++i) {
        if (parse_cmp(subcommands[i].str, arg, 0)) {
            return (*subcommands[i].func)(unit, a);
        }
    }

    return (CMD_USAGE);
}

#endif /* #if defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT) */
#endif
