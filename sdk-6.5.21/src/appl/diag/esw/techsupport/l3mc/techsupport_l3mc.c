/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    techsupport_l3mc.c
 * Purpose: This file implements techsupport utility for
 *          l3mc(layer 3 multicast feature).
*/

#include <appl/diag/techsupport.h>

/* "l3mc" feature's diag command list for all devices */
char * techsupport_l3mc_diag_cmdlist[] = {
    "l2 show",
    "vlan show",
    "l3 intf show",
    "l3 egress show",
    "l3 multipath show",
    "ipmc table show",
    "ipmc egr show",
    "ipmc counter show",
    "ipmc ip6table show",
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "L3MC" feature's software dump command list for all devices */
char * techsupport_l3mc_sw_dump_cmdlist[] = {
    "dump sw mcast",
    "dump sw multicast",
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/* Function:   techsupport_l3mc
 * Purpose :   Implements techsupport utility for l3mc(layer 3 multicast) feature.
 * Parameters: unit - Unit number
 *             a - pointer to argument.
 *             techsupport_data_t -structure that maintains per chip per feature debug info.
 * Returns:    CMD_OK :done
 *             CMD_FAIL : INVALID INPUT
 * Notes:      This function is a place holder to implement feature specific objects.
 */
int techsupport_l3mc(int unit, args_t *a, techsupport_data_t *techsupport_data)
{
    return techsupport_feature_process(unit, a, techsupport_data);
}
