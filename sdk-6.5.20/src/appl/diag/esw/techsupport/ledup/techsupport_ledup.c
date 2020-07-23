/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    techsupport_ledup.c
 * Purpose: This file implements techsupport utility for
 *          CMIC led-processor.
 */

#include <appl/diag/techsupport.h>

/* "ledup" feature's diag command list for all devices*/
char * techsupport_ledup_diag_cmdlist[] = {
    "ver",
    "soc",
    "ps",
    "config",
    "phy info",
    "show features",
    "linkscan",
    "led",
    "led dump",
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "ledup" feature's software dump command list for all devices */
char * techsupport_ledup_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Function:   techsupport_ledup
 * Purpose :   Implements techsupport utility for ledup feature.
 * Parameters: unit - Unit number
 *             a - pointer to argument.
 *             techsupport_data_t -structure that maintains per chip per feature debug info.
 * Returns:    CMD_OK :done
 *             CMD_FAIL : INVALID INPUT
 * Notes:      This function is a place holder to implement feature specific objects.
 */
int techsupport_ledup(int unit, args_t *a, techsupport_data_t *techsupport_data)
{
    return techsupport_feature_process(unit, a, techsupport_data);
}
