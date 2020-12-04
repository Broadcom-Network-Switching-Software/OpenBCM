/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: tomahawk3_int.c
 *
 * Purpose:
 *    Maintains all the debug information for int feature for tomahawk3
 *
 * Note: This file is auto-generated. Manual edits
 *       to it will be lost when it is auto-regenerated.
 */

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_int_diag_cmdlist[];
extern char * techsupport_int_sw_dump_cmdlist[];

/* "int" feature's diag command list valid only for tomahawk3 */
char * techsupport_int_tomahawk3_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "int" feature's software dump command list valid only for tomahawk3 */
char * techsupport_int_tomahawk3_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "int" feature for tomahawk3 chipset. */
static soc_mem_t techsupport_int_tomahawk3_memory_table_list[] = {
    INT_TURNAROUND_EN_BMAPm,
    L3_IIFm,
    PORT_TABm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "int" feature for tomahawk3 chipset. */
techsupport_reg_t techsupport_int_tomahawk3_reg_list[] = {
    { EGR_INT_EGRESS_TIME_DELTAr, register_type_global },
    { EGR_INT_PORT_META_DATAr, register_type_per_port },
    { EGR_INT_SWITCH_IDr, register_type_global },
    { INT_DETECT_CONFIG_0r, register_type_global },
    { INT_DETECT_CONFIG_1r, register_type_global },
    { INT_DETECT_CONFIG_2r, register_type_global },
    { INT_PROCESS_CONFIGr, register_type_global },
    { INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains diag cmdlist, reg_list, mem_list  for
 * "int" feature for tomahawk3 chipset. */
techsupport_data_t techsupport_int_tomahawk3_data = {
    techsupport_int_diag_cmdlist,
    techsupport_int_tomahawk3_reg_list,
    techsupport_int_tomahawk3_memory_table_list,
    techsupport_int_tomahawk3_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_int_sw_dump_cmdlist,
    techsupport_int_tomahawk3_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};
