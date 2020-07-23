/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: tomahawk3_alpm.c
 *
 * Purpose:
 *    Maintains all the debug information for alpm feature for tomahawk3
 *
 * Note: This file is auto-generated. Manual edits
 *       to it will be lost when it is auto-regenerated.
 */

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_alpm_diag_cmdlist[];
extern char * techsupport_alpm_sw_dump_cmdlist[];

/* "alpm" feature's diag command list valid only for tomahawk3 */
char * techsupport_alpm_tomahawk3_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "alpm" feature's software dump command list valid only for tomahawk3 */
char * techsupport_alpm_tomahawk3_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "alpm" feature for tomahawk3 chipset. */
static soc_mem_t techsupport_alpm_tomahawk3_memory_table_list[] = {
    L3_DEFIP_ALPM_LEVEL2_HIT_ONLYm,
    L3_DEFIP_ALPM_LEVEL2m,
    L3_DEFIP_ALPM_LEVEL3_HIT_ONLYm,
    L3_DEFIP_ALPM_LEVEL3m,
    L3_DEFIP_LEVEL1_HIT_ONLYm,
    L3_DEFIP_LEVEL1m,
    L3_DEFIP_PAIR_LEVEL1m,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "alpm" feature for tomahawk3 chipset. */
techsupport_reg_t techsupport_alpm_tomahawk3_reg_list[] = {
    { INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains diag cmdlist, reg_list, mem_list  for
 * "alpm" feature for tomahawk3 chipset. */
techsupport_data_t techsupport_alpm_tomahawk3_data = {
    techsupport_alpm_diag_cmdlist,
    techsupport_alpm_tomahawk3_reg_list,
    techsupport_alpm_tomahawk3_memory_table_list,
    techsupport_alpm_tomahawk3_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_alpm_sw_dump_cmdlist,
    techsupport_alpm_tomahawk3_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};
