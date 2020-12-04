/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    trident2plus_oam.c
 * Purpose: Maintains all the debug information for mmu
 *          feature for trident2plus.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_oam_diag_cmdlist[];
extern char * techsupport_oam_sw_dump_cmdlist[];

/* "oam" feature's diag command list valid for Trident2Plus */
char * techsupport_oam_trident2plus_diag_cmdlist[] = {
     NULL /* Must be the last element in this structure */
 };

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "OAM" feature's software dump command list valid for trident2plus */
char * techsupport_oam_trident2plus_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/* Structure that maintains memory list for
 * "oam" feature for Trident2plus chipset. */
static soc_mem_t techsupport_oam_trident2plus_memory_table_list[] = {
    IARB_ING_PORT_TABLEm,
    LPORT_TABm,
    PORT_TABm,
    L3_TUNNELm,
    MPLS_ENTRYm,
    L2Xm,
    L3_ENTRY_IPV4_UNICASTm,
    L3_ENTRY_IPV6_UNICASTm,
    L3_ENTRY_IPV4_MULTICASTm,
    L3_ENTRY_IPV6_MULTICASTm,
    EGR_OLP_CONFIGm,
    EGR_OLP_DGPP_CONFIGm,
    EGR_OLP_HEADER_TYPE_MAPPINGm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "oam" feature for Trident2plus chipset. */
techsupport_reg_t techsupport_oam_trident2plus_reg_list[] = {
    {BFD_RX_ACH_TYPE_CONTROL0r, register_type_global},
    {BFD_RX_ACH_TYPE_CONTROL1r, register_type_global},
    {BFD_RX_ACH_TYPE_MPLSTPr, register_type_global},
    {BFD_RX_ACH_TYPE_MPLSTP1r, register_type_global},
    {BFD_RX_UDP_CONTROLr, register_type_global},
    {BFD_RX_UDP_CONTROL_1r, register_type_global},
    {CPU_CONTROL_0r, register_type_global},
    {EGR_OLP_VLANr, register_type_global},
    {IARB_OLP_CONFIG_1r, register_type_global},
    {IARB_OLP_CONFIGr, register_type_global},
    {ING_OLP_CONFIG_0_64r, register_type_global},
    {ING_OLP_CONFIG_1_64r, register_type_global},
    {EGR_OLP_SGPP_CONFIGr, register_type_global},
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "oam" feature for Trident2plus chipset. */
techsupport_data_t techsupport_oam_trident2plus_data = {
    techsupport_oam_diag_cmdlist,
    techsupport_oam_trident2plus_reg_list,
    techsupport_oam_trident2plus_memory_table_list,
    techsupport_oam_trident2plus_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_oam_sw_dump_cmdlist,
   techsupport_oam_trident2plus_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};

