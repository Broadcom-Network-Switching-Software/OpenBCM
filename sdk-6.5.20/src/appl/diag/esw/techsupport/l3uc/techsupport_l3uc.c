/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    techsupport_l3uc.c
 * Purpose: This file implements techsupport utility for
 *          l3uc(layer 3 unicast feature.
*/

#include <appl/diag/techsupport.h>

/* "l3uc" feature's diag command list for all devices */
char * techsupport_l3uc_diag_cmdlist[] = {
    "l2 show",
    "vlan show",
    "l3 intf show",
    "l3 egress show",
    "l3 multipath show",
    "l3 l3table show",
    "l3 defip show",
#ifdef ALPM_ENABLE
    "l3 alpm show",
#endif
    "l3 ip6host show",
    "l3 ip6route show",
    "l3 tunnel_init show",
    "l3 tunnel_term show",
    "trunk show",
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "l3uc" feature's software dump command list for all devices */
char * techsupport_l3uc_sw_dump_cmdlist[] = {
    "dump sw l3",
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/* Function:   techsupport_l3uc
 * Purpose :   Implements techsupport utility for l3uc(layer 3 unicast) feature.
 * Parameters: unit - Unit number
 *             a - pointer to argument.
 *             techsupport_data_t -structure that maintains per chip per feature debug info.
 * Returns:    CMD_OK :done
 *             CMD_FAIL : INVALID INPUT
 * Notes:      This function is a place holder to implement feature specific objects.
 */
int techsupport_l3uc(int unit, args_t *a, techsupport_data_t *techsupport_data)
{
    return techsupport_feature_process(unit, a, techsupport_data);
}
