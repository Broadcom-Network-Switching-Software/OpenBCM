/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    saber2_vfp.c
 * Purpose: Maintains all the debug information for vfp
 *          feature for saber2.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_vfp_diag_cmdlist[];
extern char * techsupport_vfp_sw_dump_cmdlist[];

/* "vfp" feature's diag command list valid only for Saber2 */
char * techsupport_vfp_saber2_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "vfp" feature's software dump command list valid only for Saber2 */
char * techsupport_vfp_saber2_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "vfp" feature for Saber2 chipset. */
static soc_mem_t techsupport_vfp_saber2_memory_table_list[] = {
    PORT_TABm,
    L3_TUNNELm,
    SOURCE_TRUNK_MAP_TABLEm,
    LPORT_TABm,
    VFP_TCAMm,
    VFP_POLICY_TABLEm,
    ING_VLAN_TAG_ACTION_PROFILEm,
    SOURCE_VPm,
    VLAN_TABm,
    VLAN_MPLSm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "vfp" feature for Saber2 chipset. */
techsupport_reg_t techsupport_vfp_saber2_reg_list[] = {
    {ING_CONFIGr, register_type_global},
    {VFP_KEY_CONTROL_1r, register_type_global},
    {VFP_KEY_CONTROL_2r, register_type_global},
    {VFP_SLICE_MAPr, register_type_global},
    {VLAN_CTRLr, register_type_global},
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "vfp" feature for Saber2 chipset. */
techsupport_data_t techsupport_vfp_saber2_data = {
    techsupport_vfp_diag_cmdlist,
    techsupport_vfp_saber2_reg_list,
    techsupport_vfp_saber2_memory_table_list,
    techsupport_vfp_saber2_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_vfp_sw_dump_cmdlist,
    techsupport_vfp_saber2_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};

