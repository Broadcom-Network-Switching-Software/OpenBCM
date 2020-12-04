/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    tomahawk_mpls.c
 * Purpose: Maintains all the debug information for mpls
 *          feature for tomahawk.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_mpls_diag_cmdlist[];
extern char * techsupport_mpls_sw_dump_cmdlist[];

/* "mpls" feature's diag command list valid only for Tomahawk */
char * techsupport_mpls_tomahawk_diag_cmdlist[] = {
     NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "MPLS" feature's software dump command list valid  only for Tomahawk */
char * techsupport_mpls_tomahawk_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "mpls" feature for Tomahawk chipset. */
static soc_mem_t techsupport_mpls_tomahawk_memory_table_list[] = {
    SOURCE_TRUNK_MAP_MODBASEm,
    SYSTEM_CONFIG_TABLEm,
    SYSTEM_CONFIG_TABLE_MODBASEm,
    PORT_TABm,
    LPORT_TABm,
    SOURCE_TRUNK_MAP_TABLEm,
    ING_MPLS_ENTRY_ACTION_TABLE_Am,
    ING_MPLS_ENTRY_ACTION_TABLE_Bm,
    ING_VLAN_TAG_ACTION_PROFILEm,
    MPLS_ENTRYm,
    MPLS_ENTRY_ECCm,
    VLAN_PROTOCOLm,
    VLAN_PROTOCOL_DATAm,
    VLAN_XLATEm,
    L3_IIFm,
    MY_STATION_TCAMm,
    SOURCE_VPm,
    VFIm,
    VLAN_MPLSm,
    ING_DVP_TABLEm,
    ING_MPLS_EXP_MAPPINGm,
    L2_USER_ENTRYm,
    L3_DEFIPm,
    L3_IIF_PROFILEm,
    RTAG7_PORT_BASED_HASHm,
    VRFm,
    VLAN_TABm,
    IFP_KEY_GEN_PROGRAM_PROFILE_PIPE0m,
    IFP_KEY_GEN_PROGRAM_PROFILE_PIPE1m,
    IFP_KEY_GEN_PROGRAM_PROFILE_PIPE2m,
    IFP_KEY_GEN_PROGRAM_PROFILE_PIPE3m,
    IFP_KEY_GEN_PROGRAM_PROFILE2_PIPE0m,
    IFP_KEY_GEN_PROGRAM_PROFILE2_PIPE1m,
    IFP_KEY_GEN_PROGRAM_PROFILE2_PIPE2m,
    IFP_KEY_GEN_PROGRAM_PROFILE2_PIPE3m,
    IFP_LOGICAL_TABLE_SELECT_PIPE0m,
    IFP_LOGICAL_TABLE_SELECT_PIPE1m,
    IFP_LOGICAL_TABLE_SELECT_PIPE2m,
    IFP_LOGICAL_TABLE_SELECT_PIPE3m,
    INITIAL_L3_ECMPm,
    INITIAL_L3_ECMP_GROUPm,
    INITIAL_PROT_NHI_TABLEm,
    INITIAL_ING_L3_NEXT_HOPm,
    IFP_LOGICAL_TABLE_ACTION_PRIORITY_PIPE0m,
    IFP_LOGICAL_TABLE_ACTION_PRIORITY_PIPE1m,
    IFP_LOGICAL_TABLE_ACTION_PRIORITY_PIPE2m,
    IFP_LOGICAL_TABLE_ACTION_PRIORITY_PIPE3m,
    IFP_POLICY_TABLE_PIPE0m,
    IFP_POLICY_TABLE_PIPE1m,
    IFP_POLICY_TABLE_PIPE2m,
    IFP_POLICY_TABLE_PIPE3m,
    IFP_TCAM_PIPE0m,
    IFP_TCAM_PIPE1m,
    IFP_TCAM_PIPE2m,
    IFP_TCAM_PIPE3m,
    ING_DVP_2_TABLEm,
    ING_L3_NEXT_HOPm,
    L2MCm,
    L3_ECMPm,
    L3_IPMCm,
    ING_EN_EFILTER_BITMAPm,
    L3_MTU_VALUESm,
    TRUNK_GROUPm,
    TRUNK_MEMBERm,
    VLAN_PROFILE_2m,
    VLAN_PROFILE_TABm,
    L2Xm,
    MMU_REPL_GROUP_INFO_TBL_PIPE0m,
    MMU_REPL_GROUP_INFO_TBL_PIPE1m,
    MMU_REPL_GROUP_INFO_TBL_PIPE2m,
    MMU_REPL_GROUP_INFO_TBL_PIPE3m,
    MMU_REPL_GROUP_INITIAL_COPY_COUNT0m,
    MMU_REPL_GROUP_INITIAL_COPY_COUNT1m,
    MMU_REPL_HEAD_TBLm,
    MMU_REPL_LIST_TBLm,
    EGR_DVP_ATTRIBUTEm,
    EGR_IPMCm,
    EGR_L3_INTFm,
    EGR_L3_NEXT_HOPm,
    EGR_MAC_DA_PROFILEm,
    EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
    EGR_PORTm,
    EGR_VFIm,
    EGR_GPP_ATTRIBUTESm,
    EGR_IP_TUNNELm,
    EGR_IP_TUNNEL_IPV6m,
    EGR_IP_TUNNEL_MPLSm,
    EGR_MPLS_EXP_MAPPING_1m,
    EGR_MPLS_EXP_MAPPING_2m,
    EGR_MPLS_PRI_MAPPINGm,
    EGR_VLANm,
    EGR_VLAN_STGm,
    EGR_MPLS_EXP_PRI_MAPPINGm,
    EGR_TRILL_RBRIDGE_NICKNAMESm,
    EGR_PW_INIT_COUNTERS_PIPE0m,
    EGR_PW_INIT_COUNTERS_PIPE1m,
    EGR_PW_INIT_COUNTERS_PIPE2m,
    EGR_PW_INIT_COUNTERS_PIPE3m,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "mpls" feature for Tomahawk chipset. */
techsupport_reg_t techsupport_mpls_tomahawk_reg_list[] = {
   {CPU_CONTROL_1r, register_type_global },
   {CPU_CONTROL_Mr, register_type_global },
   {EGR_CONFIGr, register_type_global },
   {EGR_INGRESS_PORT_TPID_SELECTr, register_type_global },
   {GLOBAL_MPLS_RANGE_1_LOWERr, register_type_global },
   {GLOBAL_MPLS_RANGE_1_UPPERr, register_type_global },
   {GLOBAL_MPLS_RANGE_2_LOWERr, register_type_global },
   {GLOBAL_MPLS_RANGE_2_UPPERr, register_type_global },
   {HASH_CONTROLr, register_type_global },
   {ING_MISC_CONFIGr, register_type_global },
   {ING_MPLS_INNER_TPIDr, register_type_global },
   {ING_MPLS_TPIDr, register_type_global },
   {L3_DEFIP_RPF_CONTROLr, register_type_global },
   {MPLS_ENTRY_HASH_CONTROLr, register_type_global },
   {PORT_INITIAL_COPY_COUNT_WIDTHr, register_type_global },
   {RTAG7_HASH_CONTROLr, register_type_global },
   {RTAG7_HASH_CONTROL_3r, register_type_global },
   {RTAG7_HASH_SELr, register_type_global },
   {RTAG7_MPLS_L2_PAYLOAD_HASH_FIELD_BMAPr, register_type_global },
   {RTAG7_MPLS_L2_PAYLOAD_L3_HASH_FIELD_BMAPr, register_type_global },
   {RTAG7_MPLS_L3_PAYLOAD_HASH_FIELD_BMAPr, register_type_global },
   {RTAG7_MPLS_OUTER_HASH_FIELD_BMAPr, register_type_global },
   {VLAN_CTRLr, register_type_global },
   {XLMAC_RX_VLAN_TAGr, register_type_global },
   {BFD_RX_ACH_TYPE_MPLSTPr, register_type_global },
   {BFD_RX_ACH_TYPE_MPLSTP1_32r, register_type_global },
   {EGR_PORT_1r, register_type_global },
   {EGR_VLAN_CONTROL_1r, register_type_global },
   {ING_MPLS_TPID_0r, register_type_global },
   {ING_MPLS_TPID_1r, register_type_global },
   {ING_MPLS_TPID_2r, register_type_global },
   {ING_MPLS_TPID_3r, register_type_global },
   {IHG_LOOKUPr, register_type_global },
   {ING_CONFIGr, register_type_global },
   {ING_CONFIG_64r, register_type_global },
   {ING_OUTER_TPIDr, register_type_global },
   {ING_OUTER_TPID_2r, register_type_global },
   {MPLS_CAM_DBGCTRL_0_PIPE0r, register_type_global },
   {MPLS_CAM_DBGCTRL_0_PIPE1r, register_type_global },
   {MPLS_CAM_DBGCTRL_0_PIPE2r, register_type_global },
   {MPLS_CAM_DBGCTRL_0_PIPE3r, register_type_global },
   {MPLS_ENTRY_DBGCTRL_0_PIPE0r, register_type_global },
   {MPLS_ENTRY_DBGCTRL_0_PIPE1r, register_type_global },
   {MPLS_ENTRY_DBGCTRL_0_PIPE2r, register_type_global },
   {MPLS_ENTRY_DBGCTRL_0_PIPE3r, register_type_global },
   {MPLS_ENTRY_LP_DATA_DBGCTRL_0_PIPE0r, register_type_global },
   {MPLS_ENTRY_LP_DATA_DBGCTRL_0_PIPE1r, register_type_global },
   {MPLS_ENTRY_LP_DATA_DBGCTRL_0_PIPE2r, register_type_global },
   {MPLS_ENTRY_LP_DATA_DBGCTRL_0_PIPE3r, register_type_global },
   {MPLS_MEMORY_DBGCTRL_0_64_PIPE0r, register_type_global },
   {MPLS_MEMORY_DBGCTRL_0_64_PIPE1r, register_type_global },
   {MPLS_MEMORY_DBGCTRL_0_64_PIPE2r, register_type_global },
   {MPLS_MEMORY_DBGCTRL_0_64_PIPE3r, register_type_global },
   {MPLS_MEMORY_DBGCTRL_1_64_PIPE0r, register_type_global },
   {MPLS_MEMORY_DBGCTRL_1_64_PIPE1r, register_type_global },
   {MPLS_MEMORY_DBGCTRL_1_64_PIPE2r, register_type_global },
   {MPLS_MEMORY_DBGCTRL_1_64_PIPE3r, register_type_global },
   {MPLS_MEMORY_DBGCTRL_2_PIPE0r, register_type_global },
   {MPLS_MEMORY_DBGCTRL_2_PIPE1r, register_type_global },
   {MPLS_MEMORY_DBGCTRL_2_PIPE2r, register_type_global },
   {MPLS_MEMORY_DBGCTRL_2_PIPE3r, register_type_global },
   {MPLS_MEMORY_DBGCTRL_3_PIPE0r, register_type_global },
   {MPLS_MEMORY_DBGCTRL_3_PIPE1r, register_type_global },
   {MPLS_MEMORY_DBGCTRL_3_PIPE2r, register_type_global },
   {MPLS_MEMORY_DBGCTRL_3_PIPE3r, register_type_global },
   {PROTOCOL_PKT_CONTROLr, register_type_global },
   {SW2_IFP_DST_ACTION_CONTROLr, register_type_global },
   {VFP_KEY_CONTROL_1_PIPE0r, register_type_global },
   {VFP_KEY_CONTROL_1_PIPE1r, register_type_global },
   {VFP_KEY_CONTROL_1_PIPE2r, register_type_global },
   {VFP_KEY_CONTROL_1_PIPE3r, register_type_global },
   {EGR_VLAN_CONTROL_2r, register_type_global },
   {EGR_VLAN_CONTROL_3r, register_type_global }, 
   {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "mpls" feature for Tomahawk chipset. */
techsupport_data_t techsupport_mpls_tomahawk_data = {
    techsupport_mpls_diag_cmdlist,
    techsupport_mpls_tomahawk_reg_list,
    techsupport_mpls_tomahawk_memory_table_list,
    techsupport_mpls_tomahawk_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_mpls_sw_dump_cmdlist,
    techsupport_mpls_tomahawk_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};


