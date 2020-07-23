/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    techsupport_vlan.c
 * Purpose: This file implements techsupport utility for
 *          vlan.
*/

#include <appl/diag/techsupport.h>

/* "vlan" feature's diag command list for all devices */
char * techsupport_vlan_diag_cmdlist[] = {
    "l2 show",
    "vlan show",
    "vlan translate show",
    "vlan action translate show",
    "vlan translate egress show",
    "vlan action translate egress show",
    "vlan translate range show",
    "vlan action translate range show",
    "vlan translate dtag show",
    "trunk show",
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "vlan" feature's software dump command list for all devices */
char * techsupport_vlan_sw_dump_cmdlist[] = {
    "dump sw vlan",
    "dump sw stg",
    "dump sw port",
    "dump sw link",
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/* Function:   techsupport_vlan
 * Purpose :   Implements techsupport utility for vlan feature.
 * Parameters: unit - Unit number
 *             a - pointer to argument.
 *             techsupport_data_t -structure that maintains per chip per feature debug info.
 * Returns:    CMD_OK :done
 *             CMD_FAIL : INVALID INPUT
 * Notes:      This function is a place holder to implement feature specific objects.
 */
int techsupport_vlan(int unit, args_t *a, techsupport_data_t *techsupport_data)
{
    return techsupport_feature_process(unit, a, techsupport_data);
}
