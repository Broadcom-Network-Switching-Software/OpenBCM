/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    techsupport_mmu.c
 * Purpose: This file implements techsupport utility for
 *          mmu.
*/

#include <appl/diag/techsupport.h>

/* "mmu" feature's diag command list for all devices */
char * techsupport_mmu_diag_cmdlist[] = {
    "ps",
    "show pmap",
    "lls sw",
    "hsp",
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "MMU" feature's software dump command list for all devices */
char * techsupport_mmu_sw_dump_cmdlist[] = {
    "d sw cosq",
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Function:   techsupport_mmu
 * Purpose :   Implements techsupport utility for mmu feature.
 * Parameters: unit - Unit number
 *             a - pointer to argument.
 *             techsupport_data_t -structure that maintains per chip per feature debug info.
 * Returns:    CMD_OK :done
 *             CMD_FAIL : INVALID INPUT
 * Notes:      This function is a place holder to implement feature specific objects.
 */
int techsupport_mmu(int unit, args_t *a, techsupport_data_t *techsupport_data)
{
    return techsupport_feature_process(unit, a, techsupport_data);
}
