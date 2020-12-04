/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    trident3_l3mc.c
 * Purpose: Maintains all the debug information for l3mc(layer 3 multicast)
 *          feature for trident3.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_l3mc_diag_cmdlist[];
extern char * techsupport_l3mc_sw_dump_cmdlist[];

/* "l3mc" feature's diag command list valid only for trident3 */
char * techsupport_l3mc_trident3_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "L3MC" feature's software dump command list valid only for trident3 */
char * techsupport_l3mc_trident3_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "l3mc" feature for trident3 chipset. */
static soc_mem_t techsupport_l3mc_trident3_memory_table_list[] = {
    SOURCE_TRUNK_MAP_MODBASEm,
    ING_DEVICE_PORTm,
    SOURCE_TRUNK_MAP_TABLEm,
    L3_IIFm,
    MY_STATION_TCAMm,
    ING_ACTIVE_L3_IIF_PROFILEm,
    ING_DVP_TABLEm,
    ING_IPV6_MC_RESERVED_ADDRESSm,
    IP_MULTICAST_TCAMm,
    L2_HITDA_ONLYm,
    L2_HITSA_ONLYm,
    L3_ENTRY_DOUBLEm,
    VLAN_TABm,
    INITIAL_ING_L3_NEXT_HOPm,
    ING_DVP_2_TABLEm,
    ING_L3_NEXT_HOPm,
    L2MCm,
    L3_IPMCm,
    L3_IPMC_REMAPm,
    KNOWN_MCAST_BLOCK_MASKm,
    L3_MTU_VALUESm,
    TRUNK_GROUPm,
    TRUNK_MEMBERm,
    TRUNK_BITMAPm,
    MMU_REPL_GROUP_INITIAL_COPY_COUNTm,
    MMU_REPL_GROUP_INITIAL_COPY_COUNT_SC0m,
    MMU_REPL_GROUP_INFO_TBL_PIPE0m,
    MMU_REPL_GROUP_INFO_TBL_PIPE1m,
    MMU_REPL_HEAD_TBL_SPLIT0_PIPE0m,
    MMU_REPL_HEAD_TBL_SPLIT0_PIPE1m,
    MMU_REPL_HEAD_TBL_SPLIT1_PIPE0m,
    MMU_REPL_HEAD_TBL_SPLIT1_PIPE1m,
    MMU_REPL_LIST_TBL_PIPE0m,
    MMU_REPL_LIST_TBL_PIPE1m,
    EGR_IPMCm,
    EGR_L3_INTFm,
    EGR_L3_NEXT_HOPm,
    EGR_GPP_ATTRIBUTESm,
    EGR_VLANm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "l3mc" feature for trident3 chipset. */
techsupport_reg_t techsupport_l3mc_trident3_reg_list[] = {
    {CPU_CONTROL_1r, register_type_global },
    {IIPMC_64r, register_type_global },
    {ING_CONFIG_64r, register_type_global },
    {IPMC_MEMORY_DEBUG_TMr, register_type_global },
    {MC_CONTROL_4r, register_type_global },
    {MMU_SCFG_TOQ_MC_CFG0r, register_type_global },
    {PORT_INITIAL_COPY_COUNT_WIDTHr, register_type_global },
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "l3mc" feature for trident3 chipset. */
techsupport_data_t techsupport_l3mc_trident3_data = {
    techsupport_l3mc_diag_cmdlist,
    techsupport_l3mc_trident3_reg_list,
    techsupport_l3mc_trident3_memory_table_list,
    techsupport_l3mc_trident3_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_l3mc_sw_dump_cmdlist,
    techsupport_l3mc_trident3_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};

