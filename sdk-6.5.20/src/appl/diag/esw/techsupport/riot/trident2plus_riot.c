/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    trident2plus_riot.c
 * Purpose: Maintains all the debug information for riot
 *          feature for trident2plus.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_riot_diag_cmdlist[];
extern char * techsupport_riot_sw_dump_cmdlist[];

/* "riot" feature's diag command list valid only for Trident2plus */
char * techsupport_riot_trident2plus_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "RIOT" feature's software dump command list valid only for Trident2plus */
char * techsupport_riot_trident2plus_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/* Structure that maintains memory list for
 * "riot" feature for Trident2plus chipset. */
static soc_mem_t techsupport_riot_trident2plus_memory_table_list[] = {
    SOURCE_TRUNK_MAP_MODBASEm,
    PORT_TABm,
    SOURCE_VP_2m,
    LPORT_TABm,
    SOURCE_TRUNK_MAP_TABLEm,
    ING_VLAN_TAG_ACTION_PROFILEm,
    VLAN_XLATEm,
    MY_STATION_TCAMm,
    SOURCE_VPm,
    VFIm,
    VLAN_MPLSm,
    ING_VLAN_VFI_MEMBERSHIPm,
    L3_IIFm,
    L3_IIF_PROFILEm,
    MY_STATION_TCAM_2m,
    VFI_PROFILEm,
    VLAN_TABm,
    VLAN_PROFILE_TABm,
    L2_USER_ENTRYm,
    L2Xm,
    L3_ENTRY_IPV4_MULTICASTm,
    L3_ENTRY_IPV6_MULTICASTm,
    L3_DEFIPm,
    ING_DVP_TABLEm,
    INITIAL_ING_L3_NEXT_HOPm,
    L3_IPMC_1m,
    INITIAL_L3_ECMP_GROUPm,
    ING_DVP_2_TABLEm,
    ING_L3_NEXT_HOPm,
    L2MCm,
    L3_IPMCm,
    L3_IPMC_REMAPm,
    L3_ECMPm,
    L3_ECMP_COUNTm,
    L3_MTU_VALUESm,
    MMU_REPL_GROUP_INFO0m,
    MMU_REPL_GROUP_INFO1m,
    MMU_REPL_HEAD_TBLm,
    MMU_REPL_LIST_TBLm,
    MMU_REPL_GROUP_INITIAL_COPY_COUNTm,
    EGR_DGPP_TO_NHIm,
    EGR_DGPP_TO_NHI_MODBASEm,
    EGR_DVP_ATTRIBUTEm,
    EGR_DVP_ATTRIBUTE_1m,
    EGR_IPMCm,
    EGR_L3_INTFm,
    EGR_L3_NEXT_HOPm,
    EGR_PORTm,
    EGR_VFIm,
    EGR_FRAGMENT_ID_TABLEm,
    EGR_IP_TUNNELm,
    EGR_VLANm,
    EGR_VLAN_VFI_MEMBERSHIPm,
    EGR_VLAN_XLATEm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "riot" feature for Trident2plus chipset. */
techsupport_reg_t techsupport_riot_trident2plus_reg_list[] = {
    {EGR_TUNNEL_ID_MASKr, register_type_global},
    {EGR_VXLAN_CONTROLr, register_type_global},
    {PORT_INITIAL_COPY_COUNT_WIDTHr, register_type_global},
    {VXLAN_CONTROLr, register_type_global},
    {VXLAN_DEFAULT_NETWORK_SVPr, register_type_global},
    {ING_CONFIG_64r, register_type_global},
    {EGR_CONFIG_1r, register_type_global},
    {EGR_NIV_ETHERTYPEr, register_type_global},
    {EGR_NIV_ETHERTYPE_2r, register_type_global},
    {ING_MISC_CONFIG2r, register_type_global},
    {NIV_ETHERTYPEr, register_type_global},
    {ECMP_CONFIGr, register_type_global},
    {MMU_ENQ_LOGICAL_PORT_TO_PORT_AGG_MAPr, register_type_global},
    {MMU_ENQ_REPL_PORT_AGG_MAPr, register_type_global},
    {MMU_TOQ_REPL_PORT_AGG_MAPr, register_type_global},
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "riot" feature for Trident2plus chipset. */
techsupport_data_t techsupport_riot_trident2plus_data = {
    techsupport_riot_diag_cmdlist,
    techsupport_riot_trident2plus_reg_list,
    techsupport_riot_trident2plus_memory_table_list,
    techsupport_riot_trident2plus_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_riot_sw_dump_cmdlist,
    techsupport_riot_trident2plus_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};


