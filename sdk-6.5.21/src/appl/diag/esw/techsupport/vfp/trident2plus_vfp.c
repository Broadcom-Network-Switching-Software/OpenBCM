/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    trident2plus_vfp.c
 * Purpose: Maintains all the debug information for vfp
 *          feature for trident2plus.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_vfp_diag_cmdlist[];
extern char * techsupport_vfp_sw_dump_cmdlist[];

/* "vfp" feature's diag command list valid only for Trident2plus */
char * techsupport_vfp_trident2plus_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "vfp" feature's software dump command list valid only for Trident2plus */
char * techsupport_vfp_trident2plus_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "vfp" feature for Trident2plus chipset. */
static soc_mem_t techsupport_vfp_trident2plus_memory_table_list[] = {
    PORT_TABm,
    LPORT_TABm,
    VFP_TCAMm,
    VFP_POLICY_TABLEm,
    VFP_HASH_FIELD_BMAP_TABLE_Bm,
    VFP_HASH_FIELD_BMAP_TABLE_Am,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "vfp" feature for Trident2plus chipset. */
techsupport_reg_t techsupport_vfp_trident2plus_reg_list[] = {
    {VFP_KEY_CONTROL_1r, register_type_global},
    {VFP_KEY_CONTROL_2r, register_type_global},
    {VFP_SLICE_CONTROLr, register_type_global},
    {VFP_SLICE_MAPr, register_type_global},
    {VFP_POLICY_TABLE_RAM_CONTROLr, register_type_global},
    {ING_CONFIG_64r, register_type_global},
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "vfp" feature for Trident2plus chipset. */
techsupport_data_t techsupport_vfp_trident2plus_data = {
    techsupport_vfp_diag_cmdlist,
    techsupport_vfp_trident2plus_reg_list,
    techsupport_vfp_trident2plus_memory_table_list,
    techsupport_vfp_trident2plus_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_vfp_sw_dump_cmdlist,
    techsupport_vfp_trident2plus_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};

