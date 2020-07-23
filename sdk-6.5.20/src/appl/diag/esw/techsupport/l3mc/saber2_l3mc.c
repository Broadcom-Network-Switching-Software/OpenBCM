/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    saber2_l3mc.c
 * Purpose: Maintains all the debug information for l3mc(layer 3 multicast)
 *          feature for saber2.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_l3mc_diag_cmdlist[];
extern char * techsupport_l3mc_sw_dump_cmdlist[];

/* "l3mc" feature's diag command list valid only for Saber2 */
char * techsupport_l3mc_saber2_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "l3mc" feature's software dump command list valid only for Saber2 */
char * techsupport_l3mc_saber2_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "l3mc" feature for Saber2 chipset. */
static soc_mem_t techsupport_l3mc_saber2_memory_table_list[] = {
    ING_PHYSICAL_PORT_TABLEm,
    PP_PORT_GPP_TRANSLATION_1m,
    SOURCE_TRUNK_MAP_MODBASEm,
    SYSTEM_CONFIG_TABLEm,
    SYSTEM_CONFIG_TABLE_MODBASEm,
    PORT_TABm,
    IPV4_IN_IPV6_PREFIX_MATCH_TABLEm,
    L3_TUNNELm,
    PP_PORT_GPP_TRANSLATION_4m,
    SOURCE_TRUNK_MAP_TABLEm,
    LPORT_TABm,
    SOURCE_MOD_PROXY_TABLEm,
    MPLS_ENTRYm,
    VLAN_MACm,
    VLAN_XLATEm,
    VLAN_XLATE_OVERFLOWm,
    ING_VLAN_TAG_ACTION_PROFILEm,
    ING_NETWORK_PRUNE_CONTROLm,
    L3_IIFm,
    MY_STATION_TCAMm,
    SOURCE_VPm,
    VFI_1m,
    VLAN_TABm,
    VLAN_MPLSm,
    VLAN_PROFILE_TABm,
    STG_TABm,
    VRFm,
    PP_PORT_GPP_TRANSLATION_2m,
    L3_ENTRY_IPV4_MULTICASTm,
    L3_ENTRY_IPV4_UNICASTm,
    L3_ENTRY_IPV6_MULTICASTm,
    L3_ENTRY_IPV6_UNICASTm,
    L3_DEFIPm,
    L3_DEFIP_PAIR_128m,
    ING_DVP_TABLEm,
    INITIAL_ING_L3_NEXT_HOPm,
    INITIAL_L3_ECMPm,
    INITIAL_L3_ECMP_GROUPm,
    INITIAL_PROT_NHI_TABLEm,
    INITIAL_PROT_NHI_TABLE_1m,
    L3_IPMC_1m,
    DSCP_TABLEm,
    ING_PRI_CNG_MAPm,
    ING_UNTAGGED_PHBm,
    EGR_MASK_MODBASEm,
    ING_L3_NEXT_HOPm,
    L2MCm,
    L3_ECMPm,
    L3_ECMP_COUNTm,
    L3_IPMCm,
    L3_IPMC_2m,
    L3_IPMC_REMAPm,
    BCAST_BLOCK_MASKm,
    CPU_COS_MAPm,
    EGR_MASKm,
    ING_EGRMSKBMAPm,
    ING_EN_EFILTER_BITMAPm,
    KNOWN_MCAST_BLOCK_MASKm,
    L3_MTU_VALUESm,
    MODPORT_MAP_SWm,
    MULTIPASS_LOOPBACK_BITMAPm,
    PP_PORT_GPP_TRANSLATION_3m,
    UNKNOWN_MCAST_BLOCK_MASKm,
    UNKNOWN_UCAST_BLOCK_MASKm,
    VLAN_PROFILE_2m,
    MMU_REPL_GRP_TBL0m,
    MMU_REPL_GRP_TBL1m,
    MMU_REPL_HEAD_TBLm,
    MMU_REPL_LIST_TBLm,
    EGR_IPMCm,
    EGR_L3_INTFm,
    EGR_L3_NEXT_HOPm,
    EGR_MAC_DA_PROFILEm,
    EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
    EGR_NETWORK_PRUNE_CONTROLm,
    EGR_PHYSICAL_PORTm,
    EGR_PORTm,
    EGR_DSCP_ECN_MAPm,
    EGR_DSCP_TABLEm,
    EGR_IP_TUNNELm,
    EGR_IP_TUNNEL_IPV6m,
    EGR_IP_TUNNEL_MPLSm,
    EGR_MPLS_EXP_MAPPING_1m,
    EGR_MPLS_EXP_MAPPING_2m,
    EGR_MPLS_PRI_MAPPINGm,
    EGR_PP_PORT_GPP_TRANSLATION_1m,
    EGR_PRI_CNG_MAPm,
    EGR_VLANm,
    EGR_VLAN_STGm,
    EGR_VLAN_TAG_ACTION_PROFILEm,
    EGR_VLAN_XLATEm,
    EGR_VLAN_XLATE_OVERFLOWm,
    EGR_PP_PORT_GPP_TRANSLATION_2m,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "l3mc" feature for Saber2 chipset. */
techsupport_reg_t techsupport_l3mc_saber2_reg_list[] = {
    {CPU_CONTROL_0r, register_type_global},
    {CPU_CONTROL_1r, register_type_global},
    {DOS_CONTROLr, register_type_global},
    {DOS_CONTROL_2r, register_type_global},
    {EGR_CONFIGr, register_type_global},
    {EGR_CONFIG_1r, register_type_global},
    {EGR_CPU_CONTROLr, register_type_global},
    {EGR_IPMC_CFG2r, register_type_global},
    {EGR_MTUr, register_type_global},
    {EGR_OUTER_TPIDr, register_type_global},
    {EGR_PORT_1r, register_type_global},
    {EGR_SYS_RSVD_VIDr, register_type_global},
    {EGR_TUNNEL_PIMDR1_CFG0r, register_type_global},
    {EGR_TUNNEL_PIMDR1_CFG1r, register_type_global},
    {EGR_TUNNEL_PIMDR2_CFG0r, register_type_global},
    {EGR_TUNNEL_PIMDR2_CFG1r, register_type_global},
    {EGR_VLAN_CONTROL_1r, register_type_global},
    {EGR_VLAN_CONTROL_3r, register_type_global},
    {HASH_CONTROLr, register_type_global},
    {HG_LOOKUP_DESTINATIONr, register_type_global},
    {IGMP_MLD_PKT_CONTROLr, register_type_global},
    {IHG_LOOKUPr, register_type_global},
    {ING_CONFIGr, register_type_global},
    {ING_CONFIG_64r, register_type_global},
    {ING_MISC_CONFIGr, register_type_global},
    {ING_MISC_CONFIG2r, register_type_global},
    {ING_OUTER_TPIDr, register_type_global},
    {ING_SYS_RSVD_VIDr, register_type_global},
    {IPV6_MIN_FRAG_SIZEr, register_type_global},
    {L3_DEFIP_KEY_SELr, register_type_global},
    {L3_DEFIP_RPF_CONTROLr, register_type_global},
    {MC_CONTROL_4r, register_type_global},
    {MISCCONFIGr, register_type_global},
    {MODPORT_MAP_SELr, register_type_global},
    {PROTOCOL_PKT_CONTROLr, register_type_global},
    {VLAN_CTRLr, register_type_global},
    {VRF_MASKr, register_type_global},
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "l3mc" feature for Saber2 chipset. */
techsupport_data_t techsupport_l3mc_saber2_data = {
    techsupport_l3mc_diag_cmdlist,
    techsupport_l3mc_saber2_reg_list,
    techsupport_l3mc_saber2_memory_table_list,
    techsupport_l3mc_saber2_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_l3mc_sw_dump_cmdlist,
    techsupport_l3mc_saber2_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};

