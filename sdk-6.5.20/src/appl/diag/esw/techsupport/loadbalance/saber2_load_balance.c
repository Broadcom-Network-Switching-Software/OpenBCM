/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    saber2_loadbalance.c
 * Purpose: Maintains all the debug information for loadbalance
 *          feature for saber2.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_loadbalance_diag_cmdlist[];
extern char * techsupport_loadbalance_sw_dump_cmdlist[];

/* "loadbalance" feature's diag command list valid only for Saber2 */
char * techsupport_loadbalance_saber2_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "loadbalance" feature's software dump command list valid only for Saber2 */
char * techsupport_loadbalance_saber2_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "loadbalance" feature for Saber2 chipset. */
static soc_mem_t techsupport_loadbalance_saber2_memory_table_list[] = {
    SOURCE_TRUNK_MAP_MODBASEm,
    SYSTEM_CONFIG_TABLEm,
    SYSTEM_CONFIG_TABLE_MODBASEm,
    SOURCE_TRUNK_MAP_TABLEm,
    LPORT_TABm,
    VFP_HASH_FIELD_BMAP_TABLE_Am,
    VFP_HASH_FIELD_BMAP_TABLE_Bm,
    RTAG7_FLOW_BASED_HASHm,
    RTAG7_PORT_BASED_HASHm,
    TRUNK_CBL_TABLEm,
    INITIAL_L3_ECMPm,
    INITIAL_L3_ECMP_GROUPm,
    TRUNK_GROUPm,
    L3_ECMPm,
    L3_ECMP_COUNTm,
    TRUNK_MEMBERm,
    HG_TRUNK_BITMAPm,
    HG_TRUNK_FAILOVER_SETm,
    HG_TRUNK_FAILOVER_ENABLEm,
    HG_TRUNK_GROUPm,
    HG_TRUNK_MEMBERm,
    HIGIG_TRUNK_CONTROLm,
    ING_HIGIG_TRUNK_OVERRIDE_PROFILEm,
    NONUCAST_TRUNK_BLOCK_MASKm,
    TRUNK_BITMAPm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "loadbalance" feature for Saber2 chipset. */
techsupport_reg_t techsupport_loadbalance_saber2_reg_list[] = {
    {RTAG7_HASH_CONTROL_2_64r, register_type_global},
    {RTAG7_HASH_CONTROL_3r, register_type_global},
    {RTAG7_HASH_CONTROL_64r, register_type_global},
    {RTAG7_HASH_FIELD_BMAP_2r, register_type_global},
    {RTAG7_HASH_FIELD_BMAP_3r, register_type_global},
    {RTAG7_HASH_SEED_Ar, register_type_global},
    {RTAG7_HASH_SEED_Br, register_type_global},
    {RTAG7_IPV4_TCP_UDP_HASH_FIELD_BMAP_2r, register_type_global},
    {RTAG7_IPV6_TCP_UDP_HASH_FIELD_BMAP_2r, register_type_global},
    {RTAG7_MIM_OUTER_HASH_FIELD_BMAPr, register_type_global},
    {RTAG7_MIM_PAYLOAD_HASH_FIELD_BMAPr, register_type_global},
    {RTAG7_MIM_PAYLOAD_L3_HASH_FIELD_BMAPr, register_type_global},
    {RTAG7_MPLS_L3_PAYLOAD_HASH_FIELD_BMAPr, register_type_global},
    {RTAG7_MPLS_OUTER_HASH_FIELD_BMAPr, register_type_global},
    {RTAG7_HASH_FIELD_BMAP_5r, register_type_global},
    {RTAG7_HASH_SELr, register_type_global},
    {RTAG7_IPV4_TCP_UDP_HASH_FIELD_BMAP_1r, register_type_global},
    {RTAG7_IPV6_TCP_UDP_HASH_FIELD_BMAP_1r, register_type_global},
    {RTAG7_MPLS_L2_PAYLOAD_HASH_FIELD_BMAPr, register_type_global},
    {RTAG7_MPLS_L2_PAYLOAD_L3_HASH_FIELD_BMAPr, register_type_global},
    {EGR_MP_GROUP_HASH_CONTROLr, register_type_global},
    {EGR_VLAN_XLATE_HASH_CONTROLr, register_type_global},
    {FP_ECMP_HASH_CONTROLr, register_type_global},
    {HASH_CONTROLr, register_type_global},
    {ING_HASH_CONFIG_0r, register_type_global},
    {L2_AUX_HASH_CONTROLr, register_type_global},
    {L3_AUX_HASH_CONTROLr, register_type_global},
    {MPLS_ENTRY_HASH_CONTROLr, register_type_global},
    {MP_GROUP_HASH_CONTROLr, register_type_global},
    {VLAN_XLATE_HASH_CONTROLr, register_type_global},
    {HASH_CONTROLr, register_type_global},
    {IHG_LOOKUPr, register_type_global},
    {ING_CONFIGr, register_type_global},
    {ING_MISC_CONFIGr, register_type_global},
    {MC_CONTROL_4r, register_type_global},
    {MISCCONFIGr, register_type_global},
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "loadbalance" feature for Saber2 chipset. */
techsupport_data_t techsupport_loadbalance_saber2_data = {
    techsupport_loadbalance_diag_cmdlist,
    techsupport_loadbalance_saber2_reg_list,
    techsupport_loadbalance_saber2_memory_table_list,
    techsupport_loadbalance_saber2_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_loadbalance_sw_dump_cmdlist,
    techsupport_loadbalance_saber2_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};

