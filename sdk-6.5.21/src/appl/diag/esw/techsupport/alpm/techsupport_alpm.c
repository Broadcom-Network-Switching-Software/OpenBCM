/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: techsupport_alpm.c
 *
 * Purpose:
 *    This file implements techsupport utility for alpm
 *
 * Note: This file is auto-generated. Manual edits
 *       to it will be lost when it is auto-regenerated.
 */

#include <appl/diag/techsupport.h>

/* "alpm" feature's diag command list for all devices */
char * techsupport_alpm_diag_cmdlist[] = {
    "l3 defip show",
    "l3 alpm show all",
    "l3 alpm show brief",
    "l3 alpm sanity",
    "alpm trace dump Showflags=yes",
    "alpm bkt show",
    "alpm fmt show",
    "alpm pvtlen show",
    "alpm stat show",
    "alpm tcam show",
    "alpm config show",
    "alpm pvt show",
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "alpm" feature's software dump command list for all devices */
char * techsupport_alpm_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/* Function:   techsupport_alpm
 * Purpose :   Implements techsupport utility for alpm feature.
 * Parameters: unit - Unit number
 *             a - pointer to argument.
 *             techsupport_data_t -structure that maintains per chip per feature debug info.
 * Returns:    CMD_OK :done
 *             CMD_FAIL : INVALID INPUT
 * Notes:      This function is a place holder to implement feature specific objects.
 */
int techsupport_alpm(int unit, args_t *a, techsupport_data_t *techsupport_data)
{
    return techsupport_feature_process(unit, a, techsupport_data);
}
