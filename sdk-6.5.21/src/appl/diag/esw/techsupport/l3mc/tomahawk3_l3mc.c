/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: tomahawk3_l3mc.c
 *
 * Purpose:
 *    Maintains all the debug information for l3mc feature for tomahawk3
 *
 * Note: This file is auto-generated. Manual edits
 *       to it will be lost when it is auto-regenerated.
 */

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_l3mc_diag_cmdlist[];
extern char * techsupport_l3mc_sw_dump_cmdlist[];

/* "l3mc" feature's diag command list valid only for tomahawk3 */
char * techsupport_l3mc_tomahawk3_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "l3mc" feature's software dump command list valid only for tomahawk3 */
char * techsupport_l3mc_tomahawk3_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "l3mc" feature for tomahawk3 chipset. */
static soc_mem_t techsupport_l3mc_tomahawk3_memory_table_list[] = {
    EGR_IPMCm,
    ING_IPV6_MC_RESERVED_ADDRESSm,
    KNOWN_MCAST_BLOCK_MASKm,
    L3_DEFIP_ALPM_LEVEL2_ECCm,
    L3_DEFIP_ALPM_LEVEL2_HIT_ONLYm,
    L3_DEFIP_ALPM_LEVEL2_SINGLEm,
    L3_DEFIP_ALPM_LEVEL2m,
    L3_DEFIP_ALPM_LEVEL3_ECCm,
    L3_DEFIP_ALPM_LEVEL3_HIT_ONLYm,
    L3_DEFIP_ALPM_LEVEL3_SINGLEm,
    L3_DEFIP_ALPM_LEVEL3m,
    L3_DEFIP_DATA_LEVEL1m,
    L3_DEFIP_LEVEL1_HIT_ONLYm,
    L3_DEFIP_LEVEL1m,
    L3_DEFIP_PAIR_LEVEL1m,
    L3_DEFIP_TCAM_LEVEL1m,
    L3_IPMCm,
    MMU_REPL_GROUP_INFO_TBL0m,
    MMU_REPL_GROUP_INFO_TBL1m,
    MMU_REPL_GROUP_INFO_TBL2m,
    MMU_REPL_GROUP_INFO_TBL3m,
    MMU_REPL_GROUP_INFO_TBLm,
    MMU_REPL_HEAD_TBLm,
    MMU_REPL_LIST_TBLm,
    MMU_REPL_MEMBER_ICC_SC0m,
    MMU_REPL_MEMBER_ICC_SC1m,
    MMU_REPL_MEMBER_ICC_SC2m,
    MMU_REPL_MEMBER_ICC_SC3m,
    MMU_REPL_MEMBER_ICCm,
    MMU_REPL_STATE_TBL_ITM0m,
    MMU_REPL_STATE_TBL_ITM1m,
    UNKNOWN_MCAST_BLOCK_MASKm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "l3mc" feature for tomahawk3 chipset. */
techsupport_reg_t techsupport_l3mc_tomahawk3_reg_list[] = {
    { EGR_IPMC_CFG2r, register_type_per_port },
    { IGMP_MLD_PKT_CONTROLr, register_type_global },
    { INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains diag cmdlist, reg_list, mem_list  for
 * "l3mc" feature for tomahawk3 chipset. */
techsupport_data_t techsupport_l3mc_tomahawk3_data = {
    techsupport_l3mc_diag_cmdlist,
    techsupport_l3mc_tomahawk3_reg_list,
    techsupport_l3mc_tomahawk3_memory_table_list,
    techsupport_l3mc_tomahawk3_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_l3mc_sw_dump_cmdlist,
    techsupport_l3mc_tomahawk3_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};
