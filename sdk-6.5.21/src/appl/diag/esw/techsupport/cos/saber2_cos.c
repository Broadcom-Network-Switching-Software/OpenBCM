/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    saber2_cos.c
 * Purpose: Maintains all the debug information for cos
 *          feature for saber2.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_cos_diag_cmdlist[];
extern char * techsupport_cos_sw_dump_cmdlist[];

/* "cos" feature's diag command list valid only for Saber2 */
char * techsupport_cos_saber2_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "cos" feature's software dump command list valid only for Saber2 */
char * techsupport_cos_saber2_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "cos" feature for Saber2 chipset. */
static soc_mem_t techsupport_cos_saber2_memory_table_list[] = {
    ING_PHYSICAL_PORT_TABLEm,
    SOURCE_TRUNK_MAP_MODBASEm,
    SYSTEM_CONFIG_TABLEm,
    SYSTEM_CONFIG_TABLE_MODBASEm,
    PORT_TABm,
    L3_TUNNELm,
    SOURCE_MOD_PROXY_TABLEm,
    SOURCE_TRUNK_MAP_TABLEm,
    LPORT_TABm,
    MPLS_ENTRYm,
    VLAN_MACm,
    VLAN_PROTOCOLm,
    VLAN_PROTOCOL_DATAm,
    VLAN_SUBNETm,
    VLAN_XLATEm,
    ING_VLAN_TAG_ACTION_PROFILEm,
    VFP_POLICY_TABLEm,
    VFP_TCAMm,
    ING_MPLS_EXP_MAPPINGm,
    L3_IIFm,
    MY_STATION_TCAMm,
    SOURCE_VPm,
    VFIm,
    VFI_1m,
    VLAN_TABm,
    VLAN_MPLSm,
    VLAN_PROFILE_TABm,
    STG_TABm,
    L2Xm,
    L2_USER_ENTRYm,
    L3_ENTRY_IPV4_MULTICASTm,
    L3_ENTRY_IPV4_UNICASTm,
    L3_ENTRY_IPV6_MULTICASTm,
    L3_ENTRY_IPV6_UNICASTm,
    L3_DEFIPm,
    ING_DVP_TABLEm,
    INITIAL_ING_L3_NEXT_HOPm,
    INITIAL_L3_ECMPm,
    INITIAL_L3_ECMP_GROUPm,
    DSCP_TABLEm,
    ING_PRI_CNG_MAPm,
    ING_UNTAGGED_PHBm,
    FP_TCAMm,
    FP_POLICY_TABLEm,
    ING_L3_NEXT_HOPm,
    L2MCm,
    L3_ECMPm,
    L3_ECMP_COUNTm,
    PORT_COS_MAPm,
    COS_MAP_SELm,
    EMIRROR_CONTROLm,
    EMIRROR_CONTROL1m,
    EMIRROR_CONTROL2m,
    EMIRROR_CONTROL3m,
    EM_MTP_INDEXm,
    IFP_COS_MAPm,
    IM_MTP_INDEXm,
    ING_QUEUE_MAPm,
    ING_QUEUE_OFFSET_MAPPING_TABLEm,
    MIRROR_CONTROLm,
    MODPORT_MAP_SWm,
    PHYSICAL_PORT_BASE_QUEUEm,
    SERVICE_COS_MAPm,
    LLS_L0_PARENTm,
    LLS_L1_PARENTm,
    LLS_L2_PARENTm,
    MMU_WRED_QUEUE_CONFIG_BUFFERm,
    EGR_L3_INTFm,
    EGR_L3_NEXT_HOPm,
    EGR_MAC_DA_PROFILEm,
    EGR_MAP_MHm,
    EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
    EGR_PHYSICAL_PORTm,
    EGR_PORTm,
    EGR_QUEUE_TO_PP_PORT_MAPm,
    EGR_SUBPORT_TAG_DOT1P_MAPm,
    EGR_DSCP_TABLEm,
    EGR_EM_MTP_INDEXm,
    EGR_IM_MTP_INDEXm,
    EGR_IP_TUNNELm,
    EGR_IP_TUNNEL_MPLSm,
    EGR_MPLS_EXP_MAPPING_1m,
    EGR_MPLS_EXP_MAPPING_2m,
    EGR_MPLS_PRI_MAPPINGm,
    EGR_PRI_CNG_MAPm,
    EGR_VLANm,
    EGR_VLAN_STGm,
    EGR_VLAN_TAG_ACTION_PROFILEm,
    EGR_VLAN_XLATEm,
    EGR_MPLS_EXP_PRI_MAPPINGm,
    EFP_TCAMm,
    EFP_POLICY_TABLEm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "cos" feature for Saber2 chipset. */
techsupport_reg_t techsupport_cos_saber2_reg_list[] = {
    {EFP_SLICE_CONTROLr, register_type_global},
    {EGR_CONFIGr, register_type_global},
    {EGR_CONFIG_1r, register_type_global},
    {EGR_HG_PPD0_TX_VC_LABEL_OVERLAY_CONTROLr, register_type_global},
    {EGR_INGRESS_PORT_TPID_SELECTr, register_type_global},
    {EGR_MIM_ETHERTYPEr, register_type_global},
    {EGR_MIRROR_SELECTr, register_type_global},
    {EGR_OUTER_TPIDr, register_type_global},
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
    {IHG_LOOKUPr, register_type_global},
    {ING_CONFIGr, register_type_global},
    {ING_COS_MODEr, register_type_global},
    {ING_L2_TUNNEL_PARSE_CONTROLr, register_type_global},
    {ING_MIRROR_COS_CONTROLr, register_type_global},
    {ING_MISC_CONFIGr, register_type_global},
    {ING_MPLS_TPIDr, register_type_global},
    {ING_OUTER_TPIDr, register_type_global},
    {ING_SYS_RSVD_VIDr, register_type_global},
    {MIM_ETHERTYPEr, register_type_global},
    {MIRROR_SELECTr, register_type_global},
    {RQE_PORT_CONFIGr, register_type_global},
    {RXLP_PORT_ENABLEr, register_type_global},
    {VFP_KEY_CONTROL_1r, register_type_global},
    {VLAN_CTRLr, register_type_global},
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "cos" feature for Saber2 chipset. */
techsupport_data_t techsupport_cos_saber2_data = {
    techsupport_cos_diag_cmdlist,
    techsupport_cos_saber2_reg_list,
    techsupport_cos_saber2_memory_table_list,
    techsupport_cos_saber2_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_cos_sw_dump_cmdlist,
    techsupport_cos_saber2_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};

