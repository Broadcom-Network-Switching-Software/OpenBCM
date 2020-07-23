/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    trident2plus_mpls.c
 * Purpose: Maintains all the debug information for mpls
 *          feature for trident2plus.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_mpls_diag_cmdlist[];
extern char * techsupport_mpls_sw_dump_cmdlist[];

/* "mpls" feature's diag command list valid only for Trident2plus */
char * techsupport_mpls_trident2plus_diag_cmdlist[] = {
     NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "MPLS" feature's software dump command list valid  only for Trident2plus */
char * techsupport_mpls_trident2plus_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "mpls" feature for Trident2plus chipset. */
static soc_mem_t techsupport_mpls_trident2plus_memory_table_list[] = {
    SOURCE_TRUNK_MAP_MODBASEm,
    SYSTEM_CONFIG_TABLEm,
    SYSTEM_CONFIG_TABLE_MODBASEm,
    PORT_TABm,
    LPORT_TABm,
    SOURCE_TRUNK_MAP_TABLEm,
    ING_VLAN_TAG_ACTION_PROFILEm,
    MPLS_ENTROPY_LABEL_DATAm,
    MPLS_ENTRYm,
    VLAN_XLATEm,
    MY_STATION_TCAMm,
    SOURCE_VPm,
    VFIm,
    VLAN_MPLSm,
    VFI_1m,
    ING_MPLS_EXP_MAPPINGm,
    ING_VLAN_VFI_MEMBERSHIPm,
    L3_IIFm,
    L3_IIF_PROFILEm,
    VRFm,
    VLAN_PROFILE_TABm,
    VLAN_TABm,
    L2_USER_ENTRYm,
    L2_HITDA_ONLY_Xm,
    L2_HITDA_ONLY_Ym,
    L2_HITSA_ONLY_Xm,
    L2_HITSA_ONLY_Ym,
    L2Xm,
    L3_DEFIPm,
    RTAG7_PORT_BASED_HASHm,
    L3_DEFIP_PAIR_128m,
    ING_DVP_TABLEm,
    INITIAL_ING_L3_NEXT_HOPm,
    INITIAL_L3_ECMPm,
    INITIAL_L3_ECMP_GROUPm,
    INITIAL_PROT_NHI_TABLEm,
    INITIAL_PROT_NHI_TABLE_1m,
    TX_INITIAL_PROT_GROUP_TABLEm,
    ING_DVP_2_TABLEm,
    ING_L3_NEXT_HOPm,
    L2MCm,
    L3_ECMPm,
    L3_IPMCm,
    TRUNK_GROUPm,
    L3_ECMP_COUNTm,
    ING_EN_EFILTER_BITMAPm,
    L3_MTU_VALUESm,
    TRUNK_MEMBERm,
    VLAN_PROFILE_2m,
    ING_PW_TERM_SEQ_NUMm,
    TRUNK_BITMAPm,
    MMU_REPL_GROUP_INFO0m,
    MMU_REPL_GROUP_INFO1m,
    MMU_REPL_HEAD_TBLm,
    MMU_REPL_LIST_TBLm,
    MMU_REPL_GROUP_INITIAL_COPY_COUNTm,
    EGR_DVP_ATTRIBUTEm,
    EGR_GPP_ATTRIBUTESm,
    EGR_IPMCm,
    EGR_L3_INTFm,
    EGR_L3_NEXT_HOPm,
    EGR_MAC_DA_PROFILEm,
    EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
    EGR_PORTm,
    EGR_VFIm,
    EGR_VLAN_CONTROL_1m,
    EGR_IP_TUNNEL_MPLSm,
    EGR_VLANm,
    EGR_VLAN_VFI_MEMBERSHIPm,
    EGR_MPLS_EXP_MAPPING_1m,
    EGR_MPLS_EXP_MAPPING_2m,
    EGR_MPLS_PRI_MAPPINGm,
    EGR_TRILL_RBRIDGE_NICKNAMESm,
    EGR_MPLS_EXP_PRI_MAPPINGm,
    EGR_PORT_1m,
    EGR_PW_INIT_COUNTERSm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "mpls" feature for Trident2plus chipset. */
techsupport_reg_t techsupport_mpls_trident2plus_reg_list[] = {
    {CPU_CONTROL_1r, register_type_global},
    {CPU_CONTROL_Mr, register_type_global},
    {EGR_CONFIGr, register_type_global},
    {EGR_INGRESS_PORT_TPID_SELECTr, register_type_global},
    {GLOBAL_MPLS_RANGE_1_LOWERr, register_type_global},
    {GLOBAL_MPLS_RANGE_1_UPPERr, register_type_global},
    {GLOBAL_MPLS_RANGE_2_LOWERr, register_type_global},
    {GLOBAL_MPLS_RANGE_2_UPPERr, register_type_global},
    {HASH_CONTROLr, register_type_global},
    {L3_DEFIP_RPF_CONTROLr, register_type_global},
    {MPLS_ENTROPY_LABEL_CONFIG_64r, register_type_global},
    {PORT_INITIAL_COPY_COUNT_WIDTHr, register_type_global},
    {RTAG7_HASH_CONTROL_3r, register_type_global},
    {RTAG7_HASH_SELr, register_type_global},
    {RTAG7_MPLS_OUTER_HASH_FIELD_BMAPr, register_type_global},
    {VLAN_CTRLr, register_type_global},
    {XLMAC_RX_VLAN_TAGr, register_type_global},
    {RTAG7_MPLS_L2_PAYLOAD_HASH_FIELD_BMAPr, register_type_global},
    {RTAG7_MPLS_L3_PAYLOAD_HASH_FIELD_BMAPr, register_type_global},
    {RTAG7_HASH_CONTROLr, register_type_global},
    {ING_MPLS_TPIDr, register_type_global},
    {ING_MISC_CONFIGr, register_type_global},
    {ING_MPLS_INNER_TPIDr, register_type_global},
    {MPLS_ENTRY_HASH_CONTROLr, register_type_global},
    {RTAG7_MPLS_L2_PAYLOAD_L3_HASH_FIELD_BMAPr, register_type_global},
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "mpls" feature for Trident2plus chipset. */
techsupport_data_t techsupport_mpls_trident2plus_data = {
    techsupport_mpls_diag_cmdlist,
    techsupport_mpls_trident2plus_reg_list,
    techsupport_mpls_trident2plus_memory_table_list,
    techsupport_mpls_trident2plus_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_mpls_sw_dump_cmdlist,
    techsupport_mpls_trident2plus_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};


