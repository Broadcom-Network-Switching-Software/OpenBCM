/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    tomahawk_l3mc.c
 * Purpose: Maintains all the debug information for l3mc(layer 3 multicast)
 *          feature for tomahawk.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_l3mc_diag_cmdlist[];
extern char * techsupport_l3mc_sw_dump_cmdlist[];

/* "l3mc" feature's diag command list valid only for Tomahawk */
char * techsupport_l3mc_tomahawk_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "L3MC" feature's software dump command list valid only for Tomahawk */
char * techsupport_l3mc_tomahawk_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "l3mc" feature for Tomahawk chipset. */
static soc_mem_t techsupport_l3mc_tomahawk_memory_table_list[] = {
    SOURCE_TRUNK_MAP_MODBASEm,
    PORT_TABm,
    SOURCE_TRUNK_MAP_TABLEm,
    L3_IIFm,
    MY_STATION_TCAMm,
    VLAN_MPLSm,
    ING_ACTIVE_L3_IIF_PROFILEm,
    ING_DVP_TABLEm,
    ING_IPV6_MC_RESERVED_ADDRESSm,
    IP_MULTICAST_TCAMm,
    L2_HITDA_ONLYm,
    L2_HITSA_ONLYm,
    L3_ENTRY_IPV4_MULTICASTm,
    L3_ENTRY_IPV6_MULTICASTm,
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
    MMU_REPL_GROUP_INITIAL_COPY_COUNT_SC1m,
    MMU_REPL_GROUP_INFO_TBL_PIPE0m,
    MMU_REPL_GROUP_INFO_TBL_PIPE1m,
    MMU_REPL_GROUP_INFO_TBL_PIPE2m,
    MMU_REPL_GROUP_INFO_TBL_PIPE3m,
    MMU_REPL_HEAD_TBLm,
    MMU_REPL_LIST_TBLm,
    EGR_IPMCm,
    EGR_L3_INTFm,
    EGR_L3_NEXT_HOPm,
    EGR_GPP_ATTRIBUTESm,
    EGR_VLANm,
    EGR_TRILL_RBRIDGE_NICKNAMESm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "l3mc" feature for Tomahawk chipset. */
techsupport_reg_t techsupport_l3mc_tomahawk_reg_list[] = {
    {CPU_CONTROL_1r, register_type_global },
    {EGR_IPMC_CFG2r, register_type_global },
    {IIPMC_64r, register_type_global },
    {ING_CONFIG_64r, register_type_global },
    {IPMC_MEMORY_DEBUG_TMr, register_type_global },
    {MC_CONTROL_4r, register_type_global },
    {MMU_SCFG_TOQ_MC_CFG0r, register_type_global },
    {PORT_INITIAL_COPY_COUNT_WIDTHr, register_type_global },    
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "l3mc" feature for Tomahawk chipset. */
techsupport_data_t techsupport_l3mc_tomahawk_data = {
    techsupport_l3mc_diag_cmdlist,
    techsupport_l3mc_tomahawk_reg_list,
    techsupport_l3mc_tomahawk_memory_table_list,
    techsupport_l3mc_tomahawk_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_l3mc_sw_dump_cmdlist,
    techsupport_l3mc_tomahawk_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};

