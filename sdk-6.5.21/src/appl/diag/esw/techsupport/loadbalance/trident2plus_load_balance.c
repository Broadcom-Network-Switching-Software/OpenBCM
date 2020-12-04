/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    trident2plus_loadbalance.c
 * Purpose: Maintains all the debug information for loadbalance
 *          feature for trident2plus.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_loadbalance_diag_cmdlist[];
extern char * techsupport_loadbalance_sw_dump_cmdlist[];

/* "loadbalance" feature's diag command list only for Trident2plus */
char * techsupport_loadbalance_trident2plus_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "loadbalance" feature's software dump command list only for Trident2plus */
char * techsupport_loadbalance_trident2plus_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "loadbalance" feature for Trident2plus chipset. */
static soc_mem_t techsupport_loadbalance_trident2plus_memory_table_list[] = {
    VFP_HASH_FIELD_BMAP_TABLE_Am,
    VFP_HASH_FIELD_BMAP_TABLE_Bm,
    GTP_PORT_TABLEm,
    ING_VP_VLAN_MEMBERSHIP_ACTION_Am,
    ING_VP_VLAN_MEMBERSHIP_ACTION_Bm,
    ING_VP_VLAN_MEMBERSHIP_REMAP_Am,
    ING_VP_VLAN_MEMBERSHIP_REMAP_Bm,
    RTAG7_FLOW_BASED_HASHm,
    RTAG7_PORT_BASED_HASHm,
    TRUNK_CBL_TABLEm,
    INITIAL_L3_ECMPm,
    INITIAL_L3_ECMP_GROUPm,
    DEST_TRUNK_BITMAPm,
    TRUNK_GROUPm,
    RH_LAG_ETHERTYPE_ELIGIBILITY_MAPm,
    RH_LAG_FLOWSETm,
    L3_ECMPm,
    L3_ECMP_COUNTm,
    RH_ECMP_ETHERTYPE_ELIGIBILITY_MAPm,
    RH_ECMP_FLOWSETm,
    HG_TRUNK_BITMAPm,
    HG_TRUNK_FAILOVER_ENABLEm,
    HG_TRUNK_FAILOVER_SETm,
    HG_TRUNK_GROUPm,
    HG_TRUNK_MEMBERm,
    HIGIG_TRUNK_CONTROLm,
    ING_HIGIG_TRUNK_OVERRIDE_PROFILEm,
    DLB_HGT_ETHERTYPE_ELIGIBILITY_MAPm,
    DLB_HGT_FLOWSETm,
    DLB_HGT_GROUP_MEMBERSHIPm,
    DLB_HGT_HIST_LOADm,
    DLB_HGT_INST_LOADm,
    DLB_HGT_MEMBER_ATTRIBUTEm,
    DLB_HGT_MEMBER_HW_STATEm,
    DLB_HGT_MEMBER_STATUSm,
    DLB_HGT_MEMBER_SW_STATEm,
    DLB_HGT_OPTIMAL_CANDIDATEm,
    DLB_HGT_PLA_QUANTIZE_THRESHOLDm,
    DLB_HGT_PORT_MEMBER_MAPm,
    DLB_HGT_QUALITY_CONTROLm,
    DLB_HGT_QUALITY_MAPPINGm,
    DLB_HGT_QUALITY_RESULTm,
    DLB_HGT_RR_SELECTION_POINTERm,
    RH_HGT_ETHERTYPE_ELIGIBILITY_MAPm,
    RH_HGT_FLOWSETm,
    HG_TRUNK_BITMAPm,
    HG_TRUNK_FAILOVER_ENABLEm,
    HG_TRUNK_FAILOVER_SETm,
    HG_TRUNK_GROUPm,
    HG_TRUNK_MEMBERm,
    HIGIG_TRUNK_CONTROLm,
    ING_HIGIG_TRUNK_OVERRIDE_PROFILEm,
    NONUCAST_TRUNK_BLOCK_MASKm,
    TRUNK_MEMBERm,
    PORT_LAG_FAILOVER_SETm,
    TRUNK_BITMAPm,
    DLB_HGT_GROUP_CONTROLm,
    RH_HGT_GROUP_CONTROLm,
    DLB_HGT_FLOWSET_TIMESTAMP_PAGE_Xm,
    DLB_HGT_FLOWSET_TIMESTAMP_PAGE_Ym,
    DLB_HGT_FLOWSET_Xm,
    DLB_HGT_FLOWSET_Ym,
    DLB_HGT_GROUP_STATS_Xm,
    DLB_HGT_GROUP_STATS_Ym,
    DLB_HGT_OPTIMAL_CANDIDATE_Xm,
    DLB_HGT_OPTIMAL_CANDIDATE_Ym,
    RH_HGT_DROPS_Xm,
    RH_HGT_DROPS_Ym,
    RH_LAG_DROPS_Xm,
    RH_LAG_DROPS_Ym,
    RH_ECMP_DROPS_Xm,
    RH_ECMP_DROPS_Ym,
    EGR_VP_VLAN_MEMBERSHIP_ACTION_Am,
    EGR_VP_VLAN_MEMBERSHIP_ACTION_Bm,
    EGR_VP_VLAN_MEMBERSHIP_REMAP_Am,
    EGR_VP_VLAN_MEMBERSHIP_REMAP_Bm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "loadbalance" feature for Trident2plus chipset. */
techsupport_reg_t techsupport_loadbalance_trident2plus_reg_list[] = {
    {DLB_HGT_CURRENT_TIMEr, register_type_global},
    {DLB_HGT_FLOWSET_PDA_CONTROLr, register_type_global},
    {DLB_HGT_QUALITY_MEASURE_CONTROLr, register_type_global},
    {DLB_HGT_RANDOM_SELECTION_CONTROLr, register_type_global},
    {DLB_HGT_RANDOM_SELECTION_CONTROL_Xr, register_type_global},
    {DLB_HGT_RANDOM_SELECTION_CONTROL_Yr, register_type_global},
    {DLB_HGT_REFRESH_INDEXr, register_type_global},
    {DLB_HGT_REFRESH_INDEX_Xr, register_type_global},
    {DLB_HGT_REFRESH_INDEX_Yr, register_type_global},
    {DLB_HGT_SER_CONTROLr, register_type_global},
    {ECMP_CONFIGr, register_type_global},
    {EGR_MC_CONTROL_1r, register_type_global},
    {EGR_MC_CONTROL_2r, register_type_global},
    {EGR_VLAN_XLATE_HASH_CONTROLr, register_type_global},
    {EGR_VP_VLAN_MEMBERSHIP_HASH_CONTROLr, register_type_global},
    {ENDPOINT_QUEUE_MAP_HASH_CONTROLr, register_type_global},
    {ENHANCED_HASHING_CONTROLr, register_type_global},
    {FP_ECMP_HASH_CONTROLr, register_type_global},
    {HASH_CONTROLr, register_type_global},
    {ING_DNAT_ADDRESS_TYPE_HASH_CONTROLr, register_type_global},
    {ING_GTP_CONTROLr, register_type_global},
    {ING_HASH_CONFIG_0r, register_type_global},
    {ING_SCTP_CONTROLr, register_type_global},
    {ING_VP_VLAN_MEMBERSHIP_HASH_CONTROLr, register_type_global},
    {L2_ENDPOINT_ID_HASH_CONTROLr, register_type_global},
    {L2_TABLE_HASH_CONTROLr, register_type_global},
    {L3_TABLE_HASH_CONTROLr, register_type_global},
    {MC_CONTROL_1r, register_type_global},
    {MC_CONTROL_2r, register_type_global},
    {MC_CONTROL_3r, register_type_global},
    {MC_CONTROL_4r, register_type_global},
    {MC_CONTROL_5r, register_type_global},
    {MPLS_ENTRY_HASH_CONTROLr, register_type_global},
    {RH_ECMP_ETHERTYPE_ELIGIBILITY_CONTROLr, register_type_global},
    {RH_ECMP_FLOWSET_BANK_SELr, register_type_global},
    {RH_HGT_ETHERTYPE_ELIGIBILITY_CONTROLr, register_type_global},
    {RH_LAG_ETHERTYPE_ELIGIBILITY_CONTROLr, register_type_global},
    {RTAG7_FCOE_HASH_FIELD_BMAPr, register_type_global},
    {RTAG7_HASH_CONTROLr, register_type_global},
    {RTAG7_HASH_CONTROL_2r, register_type_global},
    {RTAG7_HASH_CONTROL_3r, register_type_global},
    {RTAG7_HASH_CONTROL_4r, register_type_global},
    {RTAG7_HASH_CONTROL_L2GRE_MASK_Ar, register_type_global},
    {RTAG7_HASH_CONTROL_L2GRE_MASK_Br, register_type_global},
    {RTAG7_HASH_FIELD_BMAP_1r, register_type_global},
    {RTAG7_HASH_FIELD_BMAP_2r, register_type_global},
    {RTAG7_HASH_FIELD_BMAP_3r, register_type_global},
    {RTAG7_HASH_FIELD_BMAP_5r, register_type_global},
    {RTAG7_HASH_SEED_Ar, register_type_global},
    {RTAG7_HASH_SEED_Br, register_type_global},
    {RTAG7_HASH_SELr, register_type_global},
    {RTAG7_IPV4_TCP_UDP_HASH_FIELD_BMAP_1r, register_type_global},
    {RTAG7_IPV4_TCP_UDP_HASH_FIELD_BMAP_2r, register_type_global},
    {RTAG7_IPV6_TCP_UDP_HASH_FIELD_BMAP_1r, register_type_global},
    {RTAG7_IPV6_TCP_UDP_HASH_FIELD_BMAP_2r, register_type_global},
    {RTAG7_L2GRE_PAYLOAD_L2_HASH_FIELD_BMAPr, register_type_global},
    {RTAG7_L2GRE_PAYLOAD_L3_HASH_FIELD_BMAPr, register_type_global},
    {RTAG7_MIM_OUTER_HASH_FIELD_BMAPr, register_type_global},
    {RTAG7_MIM_PAYLOAD_HASH_FIELD_BMAPr, register_type_global},
    {RTAG7_MIM_PAYLOAD_L3_HASH_FIELD_BMAPr, register_type_global},
    {RTAG7_MPLS_L2_PAYLOAD_HASH_FIELD_BMAPr, register_type_global},
    {RTAG7_MPLS_L2_PAYLOAD_L3_HASH_FIELD_BMAPr, register_type_global},
    {RTAG7_MPLS_L3_PAYLOAD_HASH_FIELD_BMAPr, register_type_global},
    {RTAG7_MPLS_OUTER_HASH_FIELD_BMAPr, register_type_global},
    {RTAG7_TRILL_PAYLOAD_L2_HASH_FIELD_BMAPr, register_type_global},
    {RTAG7_TRILL_PAYLOAD_L3_HASH_FIELD_BMAPr, register_type_global},
    {RTAG7_TRILL_TUNNEL_HASH_FIELD_BMAPr, register_type_global},
    {RTAG7_VXLAN_PAYLOAD_L2_HASH_FIELD_BMAPr, register_type_global},
    {RTAG7_VXLAN_PAYLOAD_L3_HASH_FIELD_BMAPr, register_type_global},
    {SHARED_TABLE_HASH_CONTROLr, register_type_global},
    {VLAN_XLATE_HASH_CONTROLr, register_type_global},
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "loadbalance" feature for Trident2plus chipset. */
techsupport_data_t techsupport_loadbalance_trident2plus_data = {
    techsupport_loadbalance_diag_cmdlist,
    techsupport_loadbalance_trident2plus_reg_list,
    techsupport_loadbalance_trident2plus_memory_table_list,
    techsupport_loadbalance_trident2plus_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_loadbalance_sw_dump_cmdlist,
    techsupport_loadbalance_trident2plus_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};

