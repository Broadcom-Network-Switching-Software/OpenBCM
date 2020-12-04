/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    techsupport_xflow_macsec.c
 * Purpose: This file implements techsupport utility for xflow-macsec info.
 */

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

#ifdef INCLUDE_XFLOW_MACSEC
/* "xflow-macsec" feature's diag command list for all devices */
char * techsupport_xflow_macsec_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "xflow-macsec" feature's software dump command list for all devices */
char * techsupport_xflow_macsec_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Function:   techsupport_xflow_macsec
 * Purpose :   Implements techsupport utility for xflow-macsec feature.
 * Parameters: unit - Unit number
 *             a - pointer to argument.
 *             techsupport_data_t -structure that maintains per chip per feature debug info.
 * Returns:    CMD_OK :done
 *             CMD_FAIL : INVALID INPUT
 * Notes:      This function is a place holder to implement feature specific objects.
 */
int techsupport_xflow_macsec(int unit, args_t *a, techsupport_data_t *techsupport_data)
{
    return techsupport_feature_process(unit, a, techsupport_data);
}
#endif /* INCLUDE_XFLOW_MACSEC */
