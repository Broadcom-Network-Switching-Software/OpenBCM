/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    tomahawk_oam.c
 * Purpose: Maintains all the debug information for mmu
 *          feature for tomahawk.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_oam_diag_cmdlist[];
extern char * techsupport_oam_sw_dump_cmdlist[];

/* "mmu" feature's diag command list valid for Tomahawk */
char * techsupport_oam_tomahawk_diag_cmdlist[] = {
     NULL /* Must be the last element in this structure */
 };

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "MMU" feature's software dump command list valid for tomahawk */
char * techsupport_oam_tomahawk_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/* Structure that maintains memory list for
 * "oam" feature for tomahawk chipset. */
static soc_mem_t techsupport_oam_tomahawk_memory_table_list[] = {
    L3_TUNNELm,
    MPLS_ENTRYm,
    L3_ENTRY_IPV4_UNICASTm,
    L3_ENTRY_IPV4_MULTICASTm,
    L3_ENTRY_IPV6_UNICASTm,
    L3_ENTRY_IPV6_MULTICASTm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "oam" feature for tomahawk chipset. */
techsupport_reg_t techsupport_oam_tomahawk_reg_list[] = {
    {BFD_RX_ACH_TYPE_CONTROL0r, register_type_global},
    {BFD_RX_ACH_TYPE_CONTROL1r, register_type_global},
    {BFD_RX_ACH_TYPE_MPLSTPr, register_type_global},
    {BFD_RX_ACH_TYPE_MPLSTP1_32r, register_type_global},
    {BFD_RX_UDP_CONTROLr, register_type_global},
    {BFD_RX_UDP_CONTROL_1r, register_type_global},
    {CPU_CONTROL_0r, register_type_global},
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "oam" feature for tomahawk chipset. */
techsupport_data_t techsupport_oam_tomahawk_data = {
    techsupport_oam_diag_cmdlist,
    techsupport_oam_tomahawk_reg_list,
    techsupport_oam_tomahawk_memory_table_list,
    techsupport_oam_tomahawk_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_oam_sw_dump_cmdlist,
   techsupport_oam_tomahawk_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};

