/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    techsupport_fflow.c
 * Purpose: This file implements techsupport utility for
 *          flex_flow.
*/

#include <appl/diag/techsupport.h>

/* "flex_flow" feature's diag command list for all devices */
char * techsupport_flex_flow_diag_cmdlist[] = {
    "l2 show",
    "vlan show",
    "vlan translate show",
    "vlan action translate show",
    "vlan translate egress show",
    "vlan action translate egress show",
    "l3 intf show",
    "l3 egress show",
    "l3 multipath show",
    "l3 l3table show",
    "l3 defip show",
    "multicast show",
    "trunk show",
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "flex_flow" feature's software dump command list for all devices */
char * techsupport_flex_flow_sw_dump_cmdlist[] = {
    "dump sw flex_flow",
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Function:   techsupport_flex_flow
 * Purpose :   Implements techsupport utility for flex_flow feature.
 * Parameters: unit - Unit number
 *             a - pointer to argument.
 *             techsupport_data_t -structure that maintains per chip per feature debug info.
 * Returns:    CMD_OK :done
 *             CMD_FAIL : INVALID INPUT
 * Notes:      This function is a place holder to implement feature specific objects.
 */
int techsupport_flex_flow(int unit, args_t *a, techsupport_data_t *techsupport_data)
{
    return techsupport_feature_process(unit, a, techsupport_data);
}
