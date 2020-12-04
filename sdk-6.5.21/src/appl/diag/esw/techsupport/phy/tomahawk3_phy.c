/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: tomahawk3_phy.c
 *
 * Purpose:
 *    Maintains all the debug information for phy feature for tomahawk3
 *
 * Note: This file is auto-generated. Manual edits
 *       to it will be lost when it is auto-regenerated.
 */

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_phy_diag_cmdlist[];
extern char * techsupport_phy_sw_dump_cmdlist[];

/* "phy" feature's diag command list valid only for tomahawk3 */
char * techsupport_phy_tomahawk3_diag_cmdlist[] = {
    "phy xe",
    "phy ce",
    "phy cd",
    "phy diag xe dsc",
    "phy diag ce dsc",
    "phy diag cd dsc",
    "phy diag xe dsc config",
    "phy diag ce dsc config",
    "phy diag cd dsc config",
    "phy diag xe eyescan",
    "phy diag ce eyescan",
    "phy diag cd eyescan",
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "phy" feature's software dump command list valid only for tomahawk3 */
char * techsupport_phy_tomahawk3_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "phy" feature for tomahawk3 chipset. */
static soc_mem_t techsupport_phy_tomahawk3_memory_table_list[] = {
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "phy" feature for tomahawk3 chipset. */
techsupport_reg_t techsupport_phy_tomahawk3_reg_list[] = {
    { CDMAC_CTRLr, register_type_per_port },
    { CDMAC_FIFO_STATUSr, register_type_per_port },
    { CDMAC_RX_LSS_STATUSr, register_type_per_port },
    { CDMAC_TXFIFO_CELL_CNTr, register_type_per_port },
    { CDPORT_MODE_REGr, register_type_global },
    { CDPORT_XGXS0_CTRL_REGr, register_type_global },
    { XLMAC_CTRLr, register_type_per_port },
    { XLMAC_FIFO_STATUSr, register_type_per_port },
    { XLMAC_RX_LSS_STATUSr, register_type_per_port },
    { XLMAC_TXFIFO_CELL_CNTr, register_type_per_port },
    { XLPORT_MODE_REGr, register_type_global },
    { XLPORT_XGXS0_CTRL_REGr, register_type_global },
    { INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains diag cmdlist, reg_list, mem_list  for
 * "phy" feature for tomahawk3 chipset. */
techsupport_data_t techsupport_phy_tomahawk3_data = {
    techsupport_phy_diag_cmdlist,
    techsupport_phy_tomahawk3_reg_list,
    techsupport_phy_tomahawk3_memory_table_list,
    techsupport_phy_tomahawk3_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_phy_sw_dump_cmdlist,
    techsupport_phy_tomahawk3_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};
