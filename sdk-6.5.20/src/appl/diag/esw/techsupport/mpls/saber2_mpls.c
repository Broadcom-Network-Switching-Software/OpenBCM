/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    saber2_mpls.c
 * Purpose: Maintains all the debug information for mpls
 *          feature for saber2.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_mpls_diag_cmdlist[];
extern char * techsupport_mpls_sw_dump_cmdlist[];

/* "mpls" feature's diag command list valid only for Saber2 */
char * techsupport_mpls_saber2_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "mpls" feature's software dump command list valid only for Saber2 */
char * techsupport_mpls_saber2_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "mpls" feature for Saber2 chipset. */
static soc_mem_t techsupport_mpls_saber2_memory_table_list[] = {
    IARB_ING_PHYSICAL_PORTm,
    LMEPm,
    LMEP_1m,
    ING_PHYSICAL_PORT_TABLEm,
    PP_PORT_GPP_TRANSLATION_1m,
    SOURCE_TRUNK_MAP_MODBASEm,
    SYSTEM_CONFIG_TABLEm,
    SYSTEM_CONFIG_TABLE_MODBASEm,
    PORT_TABm,
    L3_TUNNELm,
    PP_PORT_GPP_TRANSLATION_4m,
    SOURCE_TRUNK_MAP_TABLEm,
    LPORT_TABm,
    MPLS_ENTRYm,
    VLAN_XLATEm,
    ING_VLAN_TAG_ACTION_PROFILEm,
    ING_MPLS_EXP_MAPPINGm,
    ING_NETWORK_PRUNE_CONTROLm,
    L3_IIFm,
    MPLS_OAM_ACH_TYPE_CONFIGm,
    MY_STATION_TCAMm,
    SOURCE_VPm,
    VFIm,
    VFI_1m,
    VLAN_TABm,
    VLAN_MPLSm,
    VLAN_PROFILE_TABm,
    STG_TABm,
    VRFm,
    L2_USER_ENTRYm,
    L2Xm,
    PP_PORT_GPP_TRANSLATION_2m,
    L3_ENTRY_IPV4_MULTICASTm,
    L3_ENTRY_IPV4_UNICASTm,
    L3_ENTRY_IPV6_MULTICASTm,
    L3_ENTRY_IPV6_UNICASTm,
    L3_DEFIPm,
    L3_DEFIP_PAIR_128m,
    RTAG7_PORT_BASED_HASHm,
    ING_DVP_TABLEm,
    INITIAL_ING_L3_NEXT_HOPm,
    INITIAL_L3_ECMPm,
    INITIAL_L3_ECMP_GROUPm,
    INITIAL_PROT_NHI_TABLEm,
    INITIAL_PROT_NHI_TABLE_1m,
    MAID_REDUCTIONm,
    MA_INDEXm,
    MA_STATEm,
    OAM_OPCODE_CONTROL_PROFILEm,
    PORT_CBL_TABLEm,
    PORT_CBL_TABLE_MODBASEm,
    INITIAL_PROT_GROUP_TABLEm,
    DSCP_TABLEm,
    ING_PRI_CNG_MAPm,
    ING_SVM_PKT_RES_MAPm,
    ING_SVM_TOS_MAPm,
    ING_UNTAGGED_PHBm,
    SVM_OFFSET_TABLEm,
    EGR_MASK_MODBASEm,
    ING_L3_NEXT_HOPm,
    L2MCm,
    L3_ECMPm,
    L3_ECMP_COUNTm,
    CPU_COS_MAPm,
    EGR_MASKm,
    HG_TRUNK_BITMAPm,
    HG_TRUNK_GROUPm,
    HG_TRUNK_MEMBERm,
    HIGIG_TRUNK_CONTROLm,
    ING_EN_EFILTER_BITMAPm,
    ING_PW_TERM_SEQ_NUMm,
    L3_MTU_VALUESm,
    MIRROR_CONTROLm,
    MODPORT_MAP_SWm,
    MULTIPASS_LOOPBACK_BITMAPm,
    NONUCAST_TRUNK_BLOCK_MASKm,
    PP_PORT_GPP_TRANSLATION_3m,
    ING_LM_COUNTER_CONTROLm,
    ING_OAM_LM_COUNTERS_0m,
    ING_OAM_LM_COUNTERS_1m,
    ING_OAM_LM_COUNTERS_2m,
    ING_SERVICE_PRI_MAP_0m,
    ING_SERVICE_PRI_MAP_1m,
    ING_SERVICE_PRI_MAP_2m,
    TRUNK_BITMAPm,
    EGR_DVP_ATTRIBUTEm,
    EGR_IPMCm,
    EGR_L3_INTFm,
    EGR_L3_NEXT_HOPm,
    EGR_MAC_DA_PROFILEm,
    EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
    EGR_NETWORK_PRUNE_CONTROLm,
    EGR_PHYSICAL_PORTm,
    EGR_PORTm,
    EGR_VFIm,
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
    EGR_MP_GROUPm,
    EGR_MPLS_EXP_PRI_MAPPINGm,
    EGR_PP_PORT_GPP_TRANSLATION_2m,
    EGR_SAT_SAMP_DATA_1m,
    EGR_OLP_CONFIGm,
    EGR_OLP_DGPP_CONFIGm,
    EGR_OLP_HEADER_TYPE_MAPPINGm,
    EGR_SAT_SAMP_DATAm,
    EGR_PW_INIT_COUNTERSm,
    EGR_LM_COUNTER_CONTROLm,
    EGR_OAM_LM_COUNTERS_0m,
    EGR_OAM_LM_COUNTERS_1m,
    EGR_OAM_LM_COUNTERS_2m,
    EGR_SERVICE_PRI_MAP_0m,
    EGR_SERVICE_PRI_MAP_1m,
    EGR_SERVICE_PRI_MAP_2m,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "mpls" feature for Saber2 chipset. */
techsupport_reg_t techsupport_mpls_saber2_reg_list[] = {
    {CBL_ATTRIBUTEr, register_type_global},
    {CPU_CONTROL_0r, register_type_global},
    {CPU_CONTROL_1r, register_type_global},
    {CPU_CONTROL_Mr, register_type_global},
    {DOS_CONTROLr, register_type_global},
    {DOS_CONTROL_2r, register_type_global},
    {DOS_CONTROL_3r, register_type_global},
    {EGR_CONFIGr, register_type_global},
    {EGR_CONFIG_1r, register_type_global},
    {EGR_CPU_CONTROLr, register_type_global},
    {EGR_HG_HDR_PROT_STATUS_TX_CONTROLr, register_type_global},
    {EGR_HG_PPD0_RX_VC_LABEL_OVERLAY_CONTROLr, register_type_global},
    {EGR_HG_PPD0_TX_VC_LABEL_OVERLAY_CONTROLr, register_type_global},
    {EGR_INGRESS_PORT_TPID_SELECTr, register_type_global},
    {EGR_MIM_ETHERTYPEr, register_type_global},
    {EGR_MIRROR_SELECTr, register_type_global},
    {EGR_MPLS_ENTROPY_LABEL_CONTROLr, register_type_global},
    {EGR_OAM_LM_CNG_CONTROLr, register_type_global},
    {EGR_OAM_RX_MODE_FOR_CPUr, register_type_global},
    {EGR_OLP_VLANr, register_type_global},
    {EGR_OUTER_TPIDr, register_type_global},
    {EGR_PORT_1r, register_type_global},
    {EGR_SYS_RSVD_VIDr, register_type_global},
    {EGR_VLAN_CONTROL_1r, register_type_global},
    {EGR_VLAN_CONTROL_2r, register_type_global},
    {EGR_VLAN_CONTROL_3r, register_type_global},
    {GLOBAL_MPLS_RANGE_1_LOWERr, register_type_global},
    {GLOBAL_MPLS_RANGE_1_UPPERr, register_type_global},
    {GLOBAL_MPLS_RANGE_2_LOWERr, register_type_global},
    {GLOBAL_MPLS_RANGE_2_UPPERr, register_type_global},
    {HASH_CONTROLr, register_type_global},
    {HG_LOOKUP_DESTINATIONr, register_type_global},
    {IARB_OLP_CONFIGr, register_type_global},
    {IARB_OLP_CONFIG_1r, register_type_global},
    {IGMP_MLD_PKT_CONTROLr, register_type_global},
    {IHG_LOOKUPr, register_type_global},
    {ING_CONFIGr, register_type_global},
    {ING_CONFIG_64r, register_type_global},
    {ING_L2_TUNNEL_PARSE_CONTROLr, register_type_global},
    {ING_MISC_CONFIGr, register_type_global},
    {ING_MPLS_INNER_TPIDr, register_type_global},
    {ING_MPLS_TPIDr, register_type_global},
    {ING_OAM_LM_CNG_CONTROLr, register_type_global},
    {ING_OLP_CONFIG_0_64r, register_type_global},
    {ING_OLP_CONFIG_1_64r, register_type_global},
    {ING_OUTER_TPIDr, register_type_global},
    {ING_SVM_PKT_ATTR_SELECTOR_KEY_1r, register_type_global},
    {ING_SVM_PKT_ATTR_SELECTOR_KEY_2r, register_type_global},
    {ING_SYS_RSVD_VIDr, register_type_global},
    {IPV6_MIN_FRAG_SIZEr, register_type_global},
    {L3_DEFIP_KEY_SELr, register_type_global},
    {L3_DEFIP_RPF_CONTROLr, register_type_global},
    {MIM_ETHERTYPEr, register_type_global},
    {MISCCONFIGr, register_type_global},
    {MPLS_ENTRY_HASH_CONTROLr, register_type_global},
    {PROTOCOL_PKT_CONTROLr, register_type_global},
    {RTAG7_HASH_CONTROL_2_64r, register_type_global},
    {RTAG7_HASH_CONTROL_3r, register_type_global},
    {RTAG7_HASH_CONTROL_64r, register_type_global},
    {RTAG7_HASH_FIELD_BMAP_3r, register_type_global},
    {RTAG7_HASH_SEED_Ar, register_type_global},
    {RTAG7_HASH_SEED_Br, register_type_global},
    {RTAG7_HASH_SELr, register_type_global},
    {RTAG7_MPLS_L2_PAYLOAD_HASH_FIELD_BMAPr, register_type_global},
    {RTAG7_MPLS_L3_PAYLOAD_HASH_FIELD_BMAPr, register_type_global},
    {RTAG7_MPLS_OUTER_HASH_FIELD_BMAPr, register_type_global},
    {VLAN_CTRLr, register_type_global},
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "mpls" feature for Saber2 chipset. */
techsupport_data_t techsupport_mpls_saber2_data = {
    techsupport_mpls_diag_cmdlist,
    techsupport_mpls_saber2_reg_list,
    techsupport_mpls_saber2_memory_table_list,
    techsupport_mpls_saber2_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_mpls_sw_dump_cmdlist,
    techsupport_mpls_saber2_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};

