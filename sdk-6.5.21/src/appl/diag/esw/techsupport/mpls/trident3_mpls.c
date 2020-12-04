/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    trident3_mpls.c
 * Purpose: Maintains all the debug information for mpls
 *          feature for trident3.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_mpls_diag_cmdlist[];
extern char * techsupport_mpls_sw_dump_cmdlist[];

/* "mpls" feature's diag command list valid only for Trident3 */
char * techsupport_mpls_trident3_diag_cmdlist[] = {
     NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "MPLS" feature's software dump command list valid  only for Trident3 */
char * techsupport_mpls_trident3_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "mpls" feature for Trident3 chipset. */
static soc_mem_t techsupport_mpls_trident3_memory_table_list[] = {
    SOURCE_TRUNK_MAP_MODBASEm,
    ING_VLAN_TAG_ACTION_PROFILE_1m,
    ING_VLAN_TAG_ACTION_PROFILE_2m,
    MPLS_ENTRYm,
    MPLS_ENTRY_ECCm,
    VLAN_PROTOCOLm,
    VLAN_PROTOCOL_DATAm,
    VLAN_XLATE_1_DOUBLEm,
    L3_IIFm,
    MY_STATION_TCAMm,
    MY_STATION_TCAM_2m,
    SOURCE_VPm,
    VFIm,
    ING_DVP_TABLEm,
    ING_DVP_2_TABLEm,
    PHB_MAPPING_TBL_3m,
    L2_USER_ENTRYm,
    L3_DEFIPm,
    L3_IIF_PROFILEm,
    RTAG7_PORT_BASED_HASHm,
    VRF_ATTRS_2m,
    IFP_KEY_GEN_PROGRAM_PROFILE2_PIPE0m,
    IFP_KEY_GEN_PROGRAM_PROFILE2_PIPE1m,
    IFP_KEY_GEN_PROGRAM_PROFILE_PIPE0m,
    IFP_KEY_GEN_PROGRAM_PROFILE_PIPE1m,
    IFP_LOGICAL_TABLE_SELECT_PIPE0m,
    IFP_LOGICAL_TABLE_SELECT_PIPE1m,
    INITIAL_L3_ECMPm,
    INITIAL_L3_ECMP_GROUPm,
    INITIAL_PROT_NHI_TABLEm,
    INITIAL_ING_L3_NEXT_HOPm,
    IFP_POLICY_TABLE_PIPE0m,
    IFP_POLICY_TABLE_PIPE1m,
    IFP_TCAM_PIPE0m,
    IFP_TCAM_PIPE1m,
    IFP_LOGICAL_TABLE_ACTION_PRIORITY_PIPE0m,
    IFP_LOGICAL_TABLE_ACTION_PRIORITY_PIPE1m,
    ING_L3_NEXT_HOPm,
    L2MCm,
    L3_ECMPm,
    L3_IPMCm,
    ING_EN_EFILTER_BITMAPm,
    L3_MTU_VALUESm,
    TRUNK_GROUPm,
    TRUNK_MEMBERm,
    VLAN_PROFILE_2m,
    EGR_DVP_ATTRIBUTEm,
    EGR_IPMCm,
    EGR_L3_INTFm,
    EGR_L3_NEXT_HOPm,
    EGR_MAC_DA_PROFILEm,
    EGR_PORTm,
    EGR_VFIm,
    EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
    EGR_GPP_ATTRIBUTESm,
    EGR_VLANm,
    EGR_IP_TUNNEL_MPLS_DOUBLE_WIDEm,
    EGR_MPLS_EXP_MAPPING_1m,
    EGR_MPLS_EXP_MAPPING_2m,
    EGR_MPLS_PRI_MAPPINGm,
    EGR_IP_TUNNELm,
    EGR_IP_TUNNEL_IPV6m,
    EGR_VLAN_STGm,
    EGR_SEQUENCE_NUMBER_TABLEm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "mpls" feature for Trident3 chipset. */
techsupport_reg_t techsupport_mpls_trident3_reg_list[] = {
    {CPU_CONTROL_1r, register_type_global },
    {CPU_CONTROL_Mr, register_type_global },
    {EGR_CONFIGr, register_type_global },
    {EGR_INGRESS_PORT_TPID_SELECTr, register_type_global },
    {GLOBAL_MPLS_RANGE_1_LOWERr, register_type_global },
    {GLOBAL_MPLS_RANGE_1_UPPERr, register_type_global },
    {GLOBAL_MPLS_RANGE_2_LOWERr, register_type_global },
    {GLOBAL_MPLS_RANGE_2_UPPERr, register_type_global },
    {HASH_CONTROLr, register_type_global },
    {ING_CONFIG_64r, register_type_global },
    {ING_MISC_CONFIGr, register_type_global },
    {L3_DEFIP_RPF_CONTROLr, register_type_global },
    {MPLS_ENTRY_DBGCTRL_0_PIPE0r, register_type_global },
    {MPLS_ENTRY_DBGCTRL_0_PIPE1r, register_type_global },
    {MPLS_ENTRY_LP_DATA_DBGCTRL_0_PIPE0r, register_type_global },
    {MPLS_ENTRY_LP_DATA_DBGCTRL_0_PIPE1r, register_type_global },
    {PORT_INITIAL_COPY_COUNT_WIDTHr, register_type_global },
    {PROTOCOL_PKT_CONTROLr, register_type_global },
    {RTAG7_HASH_CONTROL_3r, register_type_global },
    {RTAG7_MPLS_L2_PAYLOAD_HASH_FIELD_BMAPr, register_type_global },
    {RTAG7_MPLS_L2_PAYLOAD_L3_HASH_FIELD_BMAPr, register_type_global },
    {RTAG7_MPLS_L3_PAYLOAD_HASH_FIELD_BMAPr, register_type_global },
    {RTAG7_MPLS_OUTER_HASH_FIELD_BMAPr, register_type_global },
    {SW2_IFP_DST_ACTION_CONTROLr, register_type_global },
    {VFP_KEY_CONTROL_1_PIPE0r, register_type_global },
    {VFP_KEY_CONTROL_1_PIPE1r, register_type_global },
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "mpls" feature for Trident3 chipset. */
techsupport_data_t techsupport_mpls_trident3_data = {
    techsupport_mpls_diag_cmdlist,
    techsupport_mpls_trident3_reg_list,
    techsupport_mpls_trident3_memory_table_list,
    techsupport_mpls_trident3_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_mpls_sw_dump_cmdlist,
    techsupport_mpls_trident3_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};


