/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    saber2_vlan.c
 * Purpose: Maintains all the debug information for vlan
 *          feature for saber2.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_vlan_diag_cmdlist[];
extern char * techsupport_vlan_sw_dump_cmdlist[];

/* "vlan" feature's diag command list valid only for Saber2 */
char * techsupport_vlan_saber2_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "vlan" feature's software dump command list valid only for Saber2 */
char * techsupport_vlan_saber2_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "vlan" feature for Saber2 chipset. */
static soc_mem_t techsupport_vlan_saber2_memory_table_list[] = {
    ING_PHYSICAL_PORT_TABLEm,
    PP_PORT_GPP_TRANSLATION_1m,
    SOURCE_TRUNK_MAP_MODBASEm,
    SYSTEM_CONFIG_TABLEm,
    SYSTEM_CONFIG_TABLE_MODBASEm,
    PORT_TABm,
    ING_VLAN_RANGEm,
    PP_PORT_GPP_TRANSLATION_4m,
    SOURCE_TRUNK_MAP_TABLEm,
    LPORT_TABm,
    MPLS_ENTRYm,
    VLAN_MACm,
    VLAN_MAC_OVERFLOWm,
    VLAN_PROTOCOLm,
    VLAN_PROTOCOL_DATAm,
    VLAN_SUBNETm,
    VLAN_XLATEm,
    VLAN_XLATE_OVERFLOWm,
    ING_VLAN_TAG_ACTION_PROFILEm,
    ING_NETWORK_PRUNE_CONTROLm,
    MY_STATION_TCAMm,
    RX_PROT_GROUP_TABLEm,
    SOURCE_VPm,
    VFIm,
    VLAN_TABm,
    VLAN_MPLSm,
    STG_TABm,
    VLAN_PROFILE_TABm,
    L2Xm,
    L2_BULK_MATCH_DATAm,
    L2_BULK_MATCH_MASKm,
    L2_BULK_MATCH_VLANS_PORT_Am,
    L2_BULK_MATCH_VLANS_PORT_Bm,
    L2_BULK_REPLACE_DATAm,
    L2_BULK_REPLACE_MASKm,
    L2_ENTRY_ONLYm,
    L2_ENTRY_OVERFLOWm,
    L2_HITDA_ONLYm,
    L2_HITSA_ONLYm,
    L2_MOD_FIFOm,
    L2_USER_ENTRYm,
    L2_BULK_MATCH_VLANS_PORT_Am,
    L2_BULK_MATCH_VLANS_PORT_Bm,
    PORT_OR_TRUNK_MAC_COUNTm,
    PORT_OR_TRUNK_MAC_LIMITm,
    PP_PORT_GPP_TRANSLATION_2m,
    VLAN_OR_VFI_MAC_COUNTm,
    VLAN_OR_VFI_MAC_LIMITm,
    FLOOD_LEARN_MATCH_VLANS_PORT_Am,
    FLOOD_LEARN_MATCH_VLANS_PORT_Bm,
    ING_DVP_TABLEm,
    INITIAL_ING_L3_NEXT_HOPm,
    INITIAL_L3_ECMP_GROUPm,
    INITIAL_PROT_NHI_TABLEm,
    INITIAL_PROT_NHI_TABLE_1m,
    PORT_CBL_TABLEm,
    PORT_CBL_TABLE_MODBASEm,
    TRUNK_CBL_TABLEm,
    DSCP_TABLEm,
    ING_PRI_CNG_MAPm,
    ING_UNTAGGED_PHBm,
    CPU_PBMm,
    EGR_MASK_MODBASEm,
    ICONTROL_OPCODE_BITMAPm,
    ING_L3_NEXT_HOPm,
    L2MCm,
    BCAST_BLOCK_MASKm,
    CPU_COS_MAPm,
    CPU_PBM_2m,
    EGR_MASKm,
    EPC_LINK_BMAPm,
    ING_EGRMSKBMAPm,
    ING_EN_EFILTER_BITMAPm,
    KNOWN_MCAST_BLOCK_MASKm,
    LOCAL_SW_DISABLE_DEFAULT_PBMm,
    LOCAL_SW_DISABLE_DEFAULT_PBM_MIRRm,
    MODPORT_MAP_SWm,
    NONUCAST_TRUNK_BLOCK_MASKm,
    PHB2_COS_MAPm,
    PORT_BRIDGE_BMAPm,
    PORT_BRIDGE_MIRROR_BMAPm,
    PP_PORT_GPP_TRANSLATION_3m,
    PP_PORT_TO_PHYSICAL_PORT_MAPm,
    UNKNOWN_HGI_BITMAPm,
    UNKNOWN_MCAST_BLOCK_MASKm,
    UNKNOWN_UCAST_BLOCK_MASKm,
    VLAN_PROFILE_2m,
    EGR_MASKm,
    MAC_BLOCKm,
    MMU_REPL_GRP_TBL0m,
    MMU_REPL_GRP_TBL1m,
    MMU_REPL_HEAD_TBLm,
    MMU_REPL_LIST_TBLm,
    MMU_EXT_MC_QUEUE_LIST0m,
    MMU_EXT_MC_QUEUE_LIST4m,
    MMU_EXT_MC_GROUP_MAPm,
    MMU_INITIAL_NHOP_TBLm,
    MMU_PROT_GROUP_TABLEm,
    EGR_DVP_ATTRIBUTEm,
    EGR_L3_INTFm,
    EGR_L3_NEXT_HOPm,
    EGR_MAC_DA_PROFILEm,
    EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
    EGR_NETWORK_PRUNE_CONTROLm,
    EGR_PHYSICAL_PORTm,
    EGR_PORTm,
    EGR_QUEUE_TO_PP_PORT_MAPm,
    EGR_DSCP_TABLEm,
    EGR_IP_TUNNELm,
    EGR_IP_TUNNEL_MPLSm,
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
 * "vlan" feature for Saber2 chipset. */
techsupport_reg_t techsupport_vlan_saber2_reg_list[] = {
    {L2_AGE_DEBUGr, register_type_global},
    {AUX_L2_BULK_CONTROLr, register_type_global},
    {CBL_ATTRIBUTEr, register_type_global},
    {CPU_CONTROL_0r, register_type_global},
    {CPU_CONTROL_1r, register_type_global},
    {DOS_CONTROLr, register_type_global},
    {DOS_CONTROL_2r, register_type_global},
    {EGR_CONFIGr, register_type_global},
    {EGR_CONFIG_1r, register_type_global},
    {EGR_HG_HDR_PROT_STATUS_TX_CONTROLr, register_type_global},
    {EGR_INGRESS_PORT_TPID_SELECTr, register_type_global},
    {EGR_IPMC_CFG2r, register_type_global},
    {EGR_MIM_ETHERTYPEr, register_type_global},
    {EGR_MTUr, register_type_global},
    {EGR_OAM_RX_MODE_FOR_CPUr, register_type_global},
    {EGR_OUTER_TPIDr, register_type_global},
    {EGR_PORT_1r, register_type_global},
    {EGR_PVLAN_EPORT_CONTROLr, register_type_global},
    {EGR_SYS_RSVD_VIDr, register_type_global},
    {EGR_VLAN_CONTROL_1r, register_type_global},
    {EGR_VLAN_CONTROL_2r, register_type_global},
    {EGR_VLAN_CONTROL_3r, register_type_global},
    {EGR_VLAN_XLATE_HASH_CONTROLr, register_type_global},
    {FLOOD_LEARN_CONTROLr, register_type_global},
    {FLOOD_LEARN_KEY_TYPE_PORT_Ar, register_type_global},
    {FLOOD_LEARN_KEY_TYPE_PORT_Br, register_type_global},
    {FLOOD_LEARN_MATCH_PORT_Ar, register_type_global},
    {FLOOD_LEARN_MATCH_PORT_Br, register_type_global},
    {HASH_CONTROLr, register_type_global},
    {HG_LOOKUP_DESTINATIONr, register_type_global},
    {IARB_LEARN_CONTROLr, register_type_global},
    {IGMP_MLD_PKT_CONTROLr, register_type_global},
    {IHG_LOOKUPr, register_type_global},
    {ING_CONFIGr, register_type_global},
    {ING_CONFIG_64r, register_type_global},
    {ING_COS_MODEr, register_type_global},
    {ING_MISC_CONFIGr, register_type_global},
    {ING_MISC_CONFIG2r, register_type_global},
    {ING_MPLS_INNER_TPIDr, register_type_global},
    {ING_MPLS_TPIDr, register_type_global},
    {ING_OUTER_TPIDr, register_type_global},
    {ING_SYS_RSVD_VIDr, register_type_global},
    {L2_AUX_HASH_CONTROLr, register_type_global},
    {L2_BULK_CONTROLr, register_type_global},
    {L2_BULK_KEY_TYPE_PORT_Ar, register_type_global},
    {L2_BULK_KEY_TYPE_PORT_Br, register_type_global},
    {L2_BULK_MATCH_PORT_Ar, register_type_global},
    {L2_BULK_MATCH_PORT_Br, register_type_global},
    {LOCAL_SW_DISABLE_CTRLr, register_type_global},
    {MC_CONTROL_4r, register_type_global},
    {MIM_ETHERTYPEr, register_type_global},
    {MISCCONFIGr, register_type_global},
    {MODPORT_MAP_SELr, register_type_global},
    {PROTOCOL_PKT_CONTROLr, register_type_global},
    {SYS_MAC_COUNTr, register_type_global},
    {SYS_MAC_LIMIT_CONTROLr, register_type_global},
    {VLAN_CTRLr, register_type_global},
    {VLAN_XLATE_HASH_CONTROLr, register_type_global},
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "vlan" feature for Saber2 chipset. */
techsupport_data_t techsupport_vlan_saber2_data = {
    techsupport_vlan_diag_cmdlist,
    techsupport_vlan_saber2_reg_list,
    techsupport_vlan_saber2_memory_table_list,
    techsupport_vlan_saber2_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_vlan_sw_dump_cmdlist,
    techsupport_vlan_saber2_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};

