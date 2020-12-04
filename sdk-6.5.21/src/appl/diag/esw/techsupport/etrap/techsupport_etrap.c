/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: techsupport_etrap.c
 *
 * Purpose:
 *    This file implements techsupport utility for etrap
 *
 * Note: This file is auto-generated. Manual edits
 *       to it will be lost when it is auto-regenerated.
 */

#include <appl/diag/techsupport.h>

/* "etrap" feature's diag command list for all devices */
char * techsupport_etrap_diag_cmdlist[] = {
    "ft etrap stats",
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "etrap" feature's software dump command list for all devices */
char * techsupport_etrap_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/* Function:   techsupport_etrap
 * Purpose :   Implements techsupport utility for etrap feature.
 * Parameters: unit - Unit number
 *             a - pointer to argument.
 *             techsupport_data_t -structure that maintains per chip per feature debug info.
 * Returns:    CMD_OK :done
 *             CMD_FAIL : INVALID INPUT
 * Notes:      This function is a place holder to implement feature specific objects.
 */
int techsupport_etrap(int unit, args_t *a, techsupport_data_t *techsupport_data)
{
    return techsupport_feature_process(unit, a, techsupport_data);
}
