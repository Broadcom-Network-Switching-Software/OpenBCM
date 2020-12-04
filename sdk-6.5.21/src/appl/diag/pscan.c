/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	mcs.c
 * Purpose: 	Micro Controller Subsystem Support
 */

#include <assert.h>
#include <sal/core/libc.h>
#include <bcm/error.h>
#include <soc/pscan.h>
#include <soc/types.h>
#include <soc/dma.h>
#include <appl/diag/shell.h>
#include <soc/drv.h>
#include <appl/diag/system.h>

#if defined(BCM_CMICM_SUPPORT)

#include <soc/uc_msg.h>

char pscan_usage[] =
    "Parameters: \n\t"
    "Control the pscan on the Uc.\n\t"
    "init\n\t"
    "delay <usec>\n\t"
    "enable <port>\n\t"
    "disable <port>\n\t"
    "config <port> <flags>\n"
    "update\n\t"
    ;

cmd_result_t
pscan_cmd(int unit, args_t *a)
/*
 * Function: 	pscan_cmd
 * Purpose:	Control uKernel pscan interface
 * Parameters:	unit - unit
 *		a - args
 * Returns:	CMD_OK/CMD_FAIL/CMD_USAGE
 */
{
    cmd_result_t rv = CMD_OK;
    char *c;
    int count, delay, port, flags;

#ifndef NO_CTRL_C
    jmp_buf ctrl_c;
#endif    

    if (!sh_check_attached("pscan", unit)) {
        return(CMD_FAIL);
    }

    if (!soc_feature(unit, soc_feature_uc)) {
        return (CMD_FAIL);
    } 

    if (ARG_CNT(a) < 1) {
        return(CMD_USAGE);
    }

    /* check for simulation*/
    if (SAL_BOOT_BCMSIM) {
        return(rv);
    }

#ifndef NO_CTRL_C    
    if (!setjmp(ctrl_c)) {
        sh_push_ctrl_c(&ctrl_c);
#endif
        c = ARG_GET(a);
        count = ARG_CNT(a);
        if ((count == 0) && !sal_strcmp(c, "init")) {
            rv = soc_pscan_init(unit);
        }
        else if ((count == 2) && !sal_strcmp(c, "config")) {
            c = ARG_GET(a);
            port = parse_integer(c);
            c = ARG_GET(a);
            flags = parse_integer(c);
            rv = soc_pscan_port_config(unit, port, flags);
        }
        else if ((count == 1) && !sal_strcmp(c, "delay")) {
            c = ARG_GET(a);
            delay = parse_integer(c);
            rv = soc_pscan_delay(unit, delay);
        }
        else if ((count == 1) && !sal_strcmp(c, "disable")) {
            c = ARG_GET(a);
            port = parse_integer(c);
            rv = soc_pscan_port_enable(unit, port, 0);
        }
        else if ((count == 1) && !sal_strcmp(c, "enable")) {
            c = ARG_GET(a);
            port = parse_integer(c);
            rv = soc_pscan_port_enable(unit, port, 1);
        }
        else if ((count == 0) && !sal_strcmp(c, "update")) {
            rv = soc_pscan_update(unit);
        }
        else {
            rv = CMD_USAGE;
        }
#ifndef NO_CTRL_C    
    } else {
        rv = CMD_INTR;
    }

    sh_pop_ctrl_c();
#endif    

    return(rv);
}

#endif /* CMICM support */

