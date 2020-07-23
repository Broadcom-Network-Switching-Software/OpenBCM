/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    trident3_loadbalance.c
 * Purpose: Maintains all the debug information for loadbalance
 *          feature for trident3.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_loadbalance_diag_cmdlist[];
extern char * techsupport_loadbalance_sw_dump_cmdlist[];

/* "loadbalance" feature's diag command list only for Trident3 */
char * techsupport_loadbalance_trident3_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "loadbalance" feature's software dump command list only for Trident3 */
char * techsupport_loadbalance_trident3_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "loadbalance" feature for Trident3 chipset. */
static soc_mem_t techsupport_loadbalance_trident3_memory_table_list[] = {
    SOURCE_TRUNK_MAP_MODBASEm,
    PORT_TABm,
    L3_TUNNELm,
    LPORT_TABm,
    L3_IIFm,
    MY_STATION_TCAMm,
    GTP_PORT_TABLEm,
    ING_VP_VLAN_MEMBERSHIPm,
    L3_DEFIPm,
    RTAG7_FLOW_BASED_HASHm,
    RTAG7_PORT_BASED_HASHm,
    VFP_HASH_FIELD_BMAP_TABLE_Am,
    VFP_HASH_FIELD_BMAP_TABLE_Bm,
    VLAN_TABm,
    INITIAL_L3_ECMPm,
    INITIAL_L3_ECMP_GROUPm,
    INITIAL_PROT_NHI_TABLEm,
    TRUNK_CBL_TABLEm,
    INITIAL_ING_L3_NEXT_HOPm,
    ICONTROL_OPCODE_BITMAPm,
    ING_L3_NEXT_HOPm,
    L3_ECMPm,
    L3_ECMP_COUNTm,
    HG_TRUNK_BITMAPm,
    HG_TRUNK_FAILOVER_ENABLEm,
    HG_TRUNK_FAILOVER_SETm,
    HG_TRUNK_GROUPm,
    HG_TRUNK_MEMBERm,
    HG_TRUNK_MODEm,
    HIGIG_TRUNK_CONTROLm,
    ING_EN_EFILTER_BITMAPm,
    ING_HIGIG_TRUNK_OVERRIDE_PROFILEm,
    L3_MTU_VALUESm,
    MODPORT_MAP_M0m,
    MODPORT_MAP_SWm,
    NONUCAST_TRUNK_BLOCK_MASKm,
    PORT_LAG_FAILOVER_SETm,
    RANDOM_RRLB_HG_TRUNK_MEMBERm,
    TRUNK_GROUPm,
    TRUNK_MEMBERm,
    HG_TRUNK_RR_CNT_PIPE0m,
    HG_TRUNK_RR_CNT_PIPE1m,
    MULTIPASS_LOOPBACK_BITMAP_PIPE0m,
    MULTIPASS_LOOPBACK_BITMAP_PIPE1m,
    TRUNK_BITMAPm,
    TRUNK_RR_CNT_PIPE0m,
    TRUNK_RR_CNT_PIPE1m,
    VLAN_PROFILE_TABm,
    EGR_L3_INTFm,
    EGR_L3_NEXT_HOPm,
    EGR_GPP_ATTRIBUTESm,
    EGR_VLANm,
    EGR_VP_VLAN_MEMBERSHIPm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "loadbalance" feature for Trident3 chipset. */
techsupport_reg_t techsupport_loadbalance_trident3_reg_list[] = {
       {CLMAC_CTRLr, register_type_global },
    {CLMAC_LAG_FAILOVER_STATUSr, register_type_global },
    {CLMAC_RX_LSS_CTRLr, register_type_global },
    {CPU_CONTROL_1r, register_type_global },
    {ECMP_CONFIGr, register_type_global },
    {EGR_CONFIGr, register_type_global },
    {EGR_CONFIG_1r, register_type_global },
    {EGR_MC_CONTROL_1r, register_type_global },
    {EGR_MC_CONTROL_2r, register_type_global },
    {EGR_OUTER_TPIDr, register_type_global },
    {EGR_OUTER_TPID_1r, register_type_global },
    {HASH_CONTROLr, register_type_global },
    {HG_TRUNK_RAND_LB_SEEDr, register_type_global },
    {ING_CONFIGr, register_type_global },
    {ING_FCOE_ETHERTYPEr, register_type_global },
    {ING_GTP_CONTROLr, register_type_global },
    {ING_HASH_CONFIG_0r, register_type_global },
    {ING_L2_TUNNEL_PARSE_CONTROLr, register_type_global },
    {ING_MISC_CONFIGr, register_type_global },
    {ING_SCTP_CONTROLr, register_type_global },
    {MC_CONTROL_1r, register_type_global },
    {MC_CONTROL_2r, register_type_global },
    {MC_CONTROL_3r, register_type_global },
    {MC_CONTROL_4r, register_type_global },
    {RTAG7_FCOE_HASH_FIELD_BMAPr, register_type_global },
    {RTAG7_HASH_CONTROLr, register_type_global },
    {RTAG7_HASH_CONTROL_2r, register_type_global },
    {RTAG7_HASH_CONTROL_4r, register_type_global },
    {RTAG7_HASH_CONTROL_L2GRE_MASK_Ar, register_type_global },
    {RTAG7_HASH_CONTROL_L2GRE_MASK_Br, register_type_global },
    {RTAG7_HASH_FIELD_BMAP_1r, register_type_global },
    {RTAG7_HASH_FIELD_BMAP_2r, register_type_global },
    {RTAG7_HASH_FIELD_BMAP_3r, register_type_global },
    {RTAG7_HASH_FIELD_BMAP_5r, register_type_global },
    {RTAG7_HASH_SEED_A_PIPE0r, register_type_global },
    {RTAG7_HASH_SEED_A_PIPE1r, register_type_global },
    {RTAG7_HASH_SEED_B_PIPE0r, register_type_global },
    {RTAG7_HASH_SEED_B_PIPE1r, register_type_global },
    {RTAG7_HASH_SELr, register_type_global },
    {RTAG7_IPV4_TCP_UDP_HASH_FIELD_BMAP_1r, register_type_global },
    {RTAG7_IPV4_TCP_UDP_HASH_FIELD_BMAP_2r, register_type_global },
    {RTAG7_IPV6_TCP_UDP_HASH_FIELD_BMAP_1r, register_type_global },
    {RTAG7_IPV6_TCP_UDP_HASH_FIELD_BMAP_2r, register_type_global },
    {RTAG7_L2GRE_PAYLOAD_L2_HASH_FIELD_BMAPr, register_type_global },
    {RTAG7_L2GRE_PAYLOAD_L3_HASH_FIELD_BMAPr, register_type_global },
    {RTAG7_MIM_OUTER_HASH_FIELD_BMAPr, register_type_global },
    {RTAG7_MIM_PAYLOAD_HASH_FIELD_BMAPr, register_type_global },
    {RTAG7_MIM_PAYLOAD_L3_HASH_FIELD_BMAPr, register_type_global },
    {RTAG7_MPLS_L2_PAYLOAD_HASH_FIELD_BMAPr, register_type_global },
    {RTAG7_MPLS_L2_PAYLOAD_L3_HASH_FIELD_BMAPr, register_type_global },
    {RTAG7_MPLS_L3_PAYLOAD_HASH_FIELD_BMAPr, register_type_global },
    {RTAG7_MPLS_OUTER_HASH_FIELD_BMAPr, register_type_global },
    {RTAG7_VXLAN_PAYLOAD_L2_HASH_FIELD_BMAPr, register_type_global },
    {RTAG7_VXLAN_PAYLOAD_L3_HASH_FIELD_BMAPr, register_type_global },
    {SW2_IFP_DST_ACTION_CONTROLr, register_type_global },
    {TRUNK_RAND_LB_SEED_PIPE0r, register_type_global },
    {TRUNK_RAND_LB_SEED_PIPE1r, register_type_global },
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "loadbalance" feature for Trident3 chipset. */
techsupport_data_t techsupport_loadbalance_trident3_data = {
    techsupport_loadbalance_diag_cmdlist,
    techsupport_loadbalance_trident3_reg_list,
    techsupport_loadbalance_trident3_memory_table_list,
    techsupport_loadbalance_trident3_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_loadbalance_sw_dump_cmdlist,
    techsupport_loadbalance_trident3_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};

