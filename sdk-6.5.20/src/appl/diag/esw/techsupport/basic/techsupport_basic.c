/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    techsupport_basic.c
 * Purpose: This file contains the implementation to collect
 *          basic config/setup information from switch.
*/

#include <appl/diag/parse.h>
#include <appl/diag/techsupport.h>
#include <soc/drv.h>
#include <sal/appl/sal.h>

/* The list commands exucuted as part of "techsupport basic" */
char * techsupport_basic_diag_cmdlist [] = {
    "a" ,
    "version",
    "config show",
    "show counters",
    "linkscan",
    "ps",
    "lls sw",
    "hsp",
    "soc",
    "fp show",
    "show pmap",
    "phy info",
    "show params",
    "show features",
    "switchcontrol",
    "switchcontrol pbm=all",
    "dump soc diff",
    "dump pcic",
#if defined(CANCUN_SUPPORT)
#ifndef NO_SAL_APPL
    "cancun stat",
#endif /* NO_SAL_APPL */
#endif /* CANCUN_SUPPORT */
    NULL /* Must be the last element in this structure */
};
/* Function:    techsupport_basic
 * Purpose :    Executes the basic commands defined in cmdlist irrespect of chipset.
 * Parameters:  u - Unit number
 *              a - pointer to argument.
 * Returns    : CMD_OK :done
 *              CMD_FAIL : INVALID INPUT
 */

int
techsupport_basic(int unit,args_t *a, techsupport_data_t *techsupport_feature_data)
{
    int i;
    for(i = 0; techsupport_basic_diag_cmdlist[i] != NULL; i++) {
#if defined(CANCUN_SUPPORT)
#ifndef NO_SAL_APPL
        if ((sal_strcasecmp(techsupport_basic_diag_cmdlist[i], "cancun stat") == 0)
                    && !SOC_IS_TRIDENT3X(unit)) {
            continue;
        }
#endif /* NO_SAL_APPL */
#endif /* CANCUN_SUPPORT */
        techsupport_command_execute(unit, techsupport_basic_diag_cmdlist[i]);
    }
    return 0;
}

