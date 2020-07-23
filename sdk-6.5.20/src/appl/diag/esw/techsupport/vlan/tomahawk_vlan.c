/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    tomahawk_vlan.c
 * Purpose: Maintains all the debug information for vlan
 *          feature for tomahawk.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_vlan_diag_cmdlist[];
extern char * techsupport_vlan_sw_dump_cmdlist[];

/* "vlan" feature's diag command list only for Tomahawk */
char * techsupport_vlan_tomahawk_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "vlan" feature's software dump command list only for Tomahawk */
char * techsupport_vlan_tomahawk_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "vlan" feature for Tomahawk chipset. */
static soc_mem_t techsupport_vlan_tomahawk_memory_table_list[] = {
   SOURCE_TRUNK_MAP_MODBASEm,
   SYSTEM_CONFIG_TABLEm,
   SYSTEM_CONFIG_TABLE_MODBASEm,
   PORT_TABm,
   ING_VLAN_RANGEm,
   SOURCE_VP_2m,
   LPORT_TABm,
   SOURCE_TRUNK_MAP_TABLEm,
   ING_VLAN_TAG_ACTION_PROFILEm,
   VFP_POLICY_TABLEm,
   VFP_TCAMm,
   VLAN_MACm,
   VLAN_PROTOCOLm,
   VLAN_PROTOCOL_DATAm,
   VLAN_SUBNETm,
   VLAN_XLATEm,
   VLAN_XLATE_LPm,
   L3_IIFm,
   MY_STATION_TCAMm,
   SOURCE_VPm,
   VLAN_MPLSm,
   ING_DVP_TABLEm,
   ING_VP_VLAN_MEMBERSHIPm,
   L2_ENTRY_ISS_LPm,
   L2_ENTRY_LPm,
   L2_ENTRY_ONLY_TILEm,
   L2_ENTRY_TILEm,
   L2_HITDA_ONLYm,
   L2_HITSA_ONLYm,
   L2_USER_ENTRYm,
   L3_DEFIPm,
   RTAG7_PORT_BASED_HASHm,
   VRFm,
   STG_TABm,
   VLAN_TABm,
   ING_UNTAGGED_PHBm,
   INITIAL_L3_ECMPm,
   INITIAL_L3_ECMP_GROUPm,
   INITIAL_PROT_NHI_TABLEm,
   PORT_CBL_TABLEm,
   PORT_CBL_TABLE_MODBASEm,
   TRUNK_CBL_TABLEm,
   INITIAL_ING_L3_NEXT_HOPm,
   L3_DEFIP_HIT_ONLYm,
   L3_ENTRY_HIT_ONLYm,
   ING_DVP_2_TABLEm,
   EGR_MASK_MODBASEm,
   ING_L3_NEXT_HOPm,
   ING_SNATm,
   L2MCm,
   L3_ECMPm,
   L3_IPMCm,
   BCAST_BLOCK_MASKm,
   EGR_MASKm,
   ING_EN_EFILTER_BITMAPm,
   KNOWN_MCAST_BLOCK_MASKm,
   L3_MTU_VALUESm,
   MODPORT_MAP_SWm,
   NONUCAST_TRUNK_BLOCK_MASKm,
   TRUNK_GROUPm,
   TRUNK_MEMBERm,
   UNKNOWN_MCAST_BLOCK_MASKm,
   UNKNOWN_UCAST_BLOCK_MASKm,
   VLAN_PROFILE_2m,
   MAC_BLOCKm,
   TRUNK_BITMAPm,
   VLAN_PROFILE_TABm,
   EMIRROR_CONTROL2m,
   EPC_LINK_BMAPm,
   L2_BULKm,
   L2_LEARN_INSERT_FAILUREm,
   L2_MGMT_SER_FIFOm,
   L2_MOD_FIFOm,
   L2Xm,
   MMU_REPL_GROUP_INITIAL_COPY_COUNT_SC0m,
   MMU_REPL_GROUP_INITIAL_COPY_COUNT_SC1m,
   EGR_DVP_ATTRIBUTEm,
   EGR_IPMCm,
   EGR_L3_INTFm,
   EGR_L3_NEXT_HOPm,
   EGR_MAP_MHm,
   EGR_NAT_PACKET_EDIT_INFOm,
   EGR_PORTm,
   EGR_FRAGMENT_ID_TABLE_PIPE0m,
   EGR_FRAGMENT_ID_TABLE_PIPE1m,
   EGR_FRAGMENT_ID_TABLE_PIPE2m,
   EGR_FRAGMENT_ID_TABLE_PIPE3m,
   EGR_GPP_ATTRIBUTESm,
   EGR_IP_TUNNELm,
   EGR_VLANm,
   EGR_VLAN_STGm,
   EGR_VLAN_TAG_ACTION_PROFILEm,
   EGR_VLAN_XLATEm,
   EGR_VP_VLAN_MEMBERSHIPm,
   EGR_VLAN_CONTROL_2m,
   EGR_VLAN_CONTROL_3m,
   EGR_TRILL_RBRIDGE_NICKNAMESm,
   EGR_ENABLEm,
   EGR_MAX_USED_ENTRIESm,
   EGR_PER_PORT_BUFFER_SFT_RESETm,
   INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "vlan" feature for Tomahawk chipset. */
techsupport_reg_t techsupport_vlan_tomahawk_reg_list[] = {
   {AUX_L2_BULK_CONTROLr, register_type_global },
   {CBL_ATTRIBUTEr, register_type_global },
   {CPU_CONTROL_1r, register_type_global },
   {CPU_CONTROL_Mr, register_type_global },
   {EGR_CONFIGr, register_type_global },
   {EGR_CONFIG_1r, register_type_global },
   {EGR_FLEX_CTR_COUNTER_UPDATE_CONTROL_0r, register_type_global },
   {EGR_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_0r, register_type_global },
   {EGR_INGRESS_PORT_TPID_SELECTr, register_type_global },
   {EGR_L2GRE_CONTROLr, register_type_global },
   {EGR_OUTER_TPIDr, register_type_global },
   {EGR_OUTER_TPID_0r, register_type_global },
   {EGR_OUTER_TPID_1r, register_type_global },
   {EGR_OUTER_TPID_2r, register_type_global },
   {EGR_OUTER_TPID_3r, register_type_global },
   {EGR_PORT_1r, register_type_global },
   {EGR_PVLAN_EPORT_CONTROLr, register_type_global },
   {EGR_TUNNEL_ID_MASKr, register_type_global },
   {EGR_VLAN_CONTROL_1r, register_type_global },
   {EGR_VLAN_CONTROL_2r, register_type_global },
   {EGR_VLAN_CONTROL_3r, register_type_global },
   {EGR_VLAN_XLATE_HASH_CONTROLr, register_type_global },
   {EGR_VP_VLAN_MEMBERSHIP_HASH_CONTROLr, register_type_global },
   {ING_CONFIG_64r, register_type_global },
   {ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_4r, register_type_global },
   {ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_0r, register_type_global },
   {ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_12r, register_type_global },
   {ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_16r, register_type_global },
   {ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_4r, register_type_global },
   {ING_FLEX_CTR_PKT_ATTR_SELECTOR_KEY_8r, register_type_global },
   {ING_L2_TUNNEL_PARSE_CONTROLr, register_type_global },
   {ING_MPLS_INNER_TPIDr, register_type_global },
   {ING_OUTER_TPIDr, register_type_global },
   {ING_OUTER_TPID_0r, register_type_global },
   {ING_OUTER_TPID_1r, register_type_global },
   {ING_OUTER_TPID_2r, register_type_global },
   {ING_OUTER_TPID_3r, register_type_global },
   {ING_VP_VLAN_MEMBERSHIP_HASH_CONTROLr, register_type_global },
   {IRSEL2_SER_CONTROLr, register_type_global },
   {L2_BULK_CONTROLr, register_type_global },
   {L2_BULK_ECC_STATUSr, register_type_global },
   {L2_ENTRY_CONTROL_0_PIPE0r, register_type_global },
   {L2_ENTRY_CONTROL_0_PIPE1r, register_type_global },
   {L2_ENTRY_CONTROL_0_PIPE2r, register_type_global },
   {L2_ENTRY_CONTROL_0_PIPE3r, register_type_global },
   {L2_ENTRY_CONTROL_6r, register_type_global },
   {L2_ENTRY_PARITY_CONTROLr, register_type_global },
   {L2_HIT_DBGCTRL_0_PIPE0r, register_type_global },
   {L2_HIT_DBGCTRL_0_PIPE1r, register_type_global },
   {L2_HIT_DBGCTRL_0_PIPE2r, register_type_global },
   {L2_HIT_DBGCTRL_0_PIPE3r, register_type_global },
   {L2_LP_CONTROL_0_PIPE0r, register_type_global },
   {L2_LP_CONTROL_0_PIPE1r, register_type_global },
   {L2_LP_CONTROL_0_PIPE2r, register_type_global },
   {L2_LP_CONTROL_0_PIPE3r, register_type_global },
   {L2_MGMT_CONTROLr, register_type_global },
   {L2_MGMT_HW_RESET_CONTROL_0r, register_type_global },
   {L2_MGMT_HW_RESET_CONTROL_1r, register_type_global },
   {L2_MGMT_INTRr, register_type_global },
   {L2_MGMT_INTR_MASKr, register_type_global },
   {L2_MGMT_SER_FIFO_CTRLr, register_type_global },
   {L2_MOD_FIFO_CLAIM_AVAILr, register_type_global },
   {L2_MOD_FIFO_CNTr, register_type_global },
   {L2_MOD_FIFO_ENABLEr, register_type_global },
   {L2_MOD_FIFO_MEMORY_CONTROL_0r, register_type_global },
   {L2_MOD_FIFO_PARITY_CONTROLr, register_type_global },
   {L2_MOD_FIFO_RD_PTRr, register_type_global },
   {L2_MOD_FIFO_WR_PTRr, register_type_global },
   {L2_TABLE_HASH_CONTROLr, register_type_global },
   {L2_USER_ENTRY_DATA_PARITY_CONTROLr, register_type_global },
   {L2GRE_CONTROLr, register_type_global },
   {L2GRE_DEFAULT_NETWORK_SVPr, register_type_global },
   {L3_DEFIP_RPF_CONTROLr, register_type_global },
   {LEARN_CONTROLr, register_type_global },
   {MC_CONTROL_1r, register_type_global },
   {OOBFC_ENG_PORT_QSEL2r, register_type_global },
   {PORT_INITIAL_COPY_COUNT_WIDTHr, register_type_global },
   {VFP_KEY_CONTROL_1_PIPE0r, register_type_global },
   {VFP_KEY_CONTROL_1_PIPE1r, register_type_global },
   {VFP_KEY_CONTROL_1_PIPE2r, register_type_global },
   {VFP_KEY_CONTROL_1_PIPE3r, register_type_global },
   {VLAN_CTRLr, register_type_global },
   {VLAN_XLATE_HASH_CONTROLr, register_type_global }, 
   {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "vlan" feature for Tomahawk chipset. */
techsupport_data_t techsupport_vlan_tomahawk_data = {
    techsupport_vlan_diag_cmdlist,
    techsupport_vlan_tomahawk_reg_list,
    techsupport_vlan_tomahawk_memory_table_list,
    techsupport_vlan_tomahawk_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_vlan_sw_dump_cmdlist,
    techsupport_vlan_tomahawk_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};

