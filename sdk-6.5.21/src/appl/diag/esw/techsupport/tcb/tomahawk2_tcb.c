/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    tomahawk2_tcb.c
 * Purpose: Maintains all the debug information for tcb
 *          feature for tomahawk2.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_tcb_diag_cmdlist[];
extern char * techsupport_tcb_sw_dump_cmdlist[];

/* "tcb" feature's diag command list valid only for tomahawk2 */
char * techsupport_tcb_tomahawk2_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "tcb" feature's software dump command list valid only for tomahawk2 */
char * techsupport_tcb_tomahawk2_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "tcb" feature for tomahawk2 chipset. */
static soc_mem_t techsupport_tcb_tomahawk2_memory_table_list[] = {
   TCB_BUFFER_METADATA_XPE0m,
   TCB_BUFFER_METADATA_XPE1m,
   TCB_BUFFER_METADATA_XPE2m,
   TCB_BUFFER_METADATA_XPE3m,
   TCB_EVENT_BUFFER_XPE0m,
   TCB_EVENT_BUFFER_XPE1m,
   TCB_EVENT_BUFFER_XPE2m,
   TCB_EVENT_BUFFER_XPE3m,
   TCB_THRESHOLD_PROFILE_MAP_XPE0m,
   TCB_THRESHOLD_PROFILE_MAP_XPE1m,
   TCB_THRESHOLD_PROFILE_MAP_XPE2m,
   TCB_THRESHOLD_PROFILE_MAP_XPE3m,
   MMU_TCB_BUFFER_CELL_DATA_LOWER_XPE0m,
   MMU_TCB_BUFFER_CELL_DATA_LOWER_XPE1m,
   MMU_TCB_BUFFER_CELL_DATA_LOWER_XPE2m,
   MMU_TCB_BUFFER_CELL_DATA_LOWER_XPE3m,
   MMU_TCB_BUFFER_CELL_DATA_UPPER_XPE0m,
   MMU_TCB_BUFFER_CELL_DATA_UPPER_XPE1m,
   MMU_TCB_BUFFER_CELL_DATA_UPPER_XPE2m,
   MMU_TCB_BUFFER_CELL_DATA_UPPER_XPE3m,
   INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "tcb" feature for tomahawk2 chipset. */
techsupport_reg_t techsupport_tcb_tomahawk2_reg_list[] = {
   {MMU_GCFG_TCB_FREEZEr, register_type_global },
   {MMU_XCFG_XPE_CPU_INT_ENr, register_type_global },
   {TCB_BUFFER_READYr, register_type_global },
   {TCB_BUFFER_STATUSr, register_type_global },
   {TCB_CONFIGr, register_type_global },
   {TCB_CURRENT_PHASEr, register_type_global },
   {TCB_SAMPLE_RANDOM_SEEDr, register_type_global },
   {TCB_THRESHOLD_PROFILEr, register_type_global },
   {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "tcb" feature for tomahawk2 chipset. */
techsupport_data_t techsupport_tcb_tomahawk2_data = {
    techsupport_tcb_diag_cmdlist,
    techsupport_tcb_tomahawk2_reg_list,
    techsupport_tcb_tomahawk2_memory_table_list,
    techsupport_tcb_tomahawk2_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_tcb_sw_dump_cmdlist,
    techsupport_tcb_tomahawk2_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};

