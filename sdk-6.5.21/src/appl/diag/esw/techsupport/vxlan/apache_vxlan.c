/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    apache_vxlan.c
 * Purpose: Maintains all the debug information for vxlan
 *          feature for apache.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_vxlan_diag_cmdlist[];
extern char * techsupport_vxlan_sw_dump_cmdlist[];

/* "vxlan" feature's diag command list only for apache */
char * techsupport_vxlan_apache_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "VXLAN" feature's software dump command list only for apache */
char * techsupport_vxlan_apache_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "vxlan" feature for apache chipset. */
static soc_mem_t techsupport_vxlan_apache_memory_table_list[] = {
    SOURCE_TRUNK_MAP_MODBASEm,
    PORT_TABm,
    SOURCE_VP_2m,
    LPORT_TABm,
    SOURCE_TRUNK_MAP_TABLEm,
    ING_VLAN_TAG_ACTION_PROFILEm,
    MPLS_ENTRYm,
    VLAN_XLATEm,
    MY_STATION_TCAMm,
    SOURCE_VPm,
    VFIm,
    VLAN_MPLSm,
    ING_VLAN_VFI_MEMBERSHIPm,
    L3_IIFm,
    L3_IIF_PROFILEm,
    STG_TABm,
    VLAN_PROFILE_TABm,
    VLAN_TABm,
    L2_USER_ENTRYm,
    L2_HITDA_ONLYm,
    L2_HITSA_ONLYm,
    L2Xm,
    L3_ENTRY_IPV4_MULTICASTm,
    L3_ENTRY_IPV6_MULTICASTm,
    RTAG7_PORT_BASED_HASHm,
    ING_DVP_TABLEm,
    INITIAL_ING_L3_NEXT_HOPm,
    INITIAL_L3_ECMPm,
    INITIAL_L3_ECMP_GROUPm,
    INITIAL_PROT_NHI_TABLEm,
    L3_IPMC_1m,
    ING_DVP_2_TABLEm,
    ING_L3_NEXT_HOPm,
    L2MCm,
    L3_ECMPm,
    L3_IPMCm,
    TRUNK_GROUPm,
    L3_IPMC_REMAPm,
    L3_ECMP_COUNTm,
    L3_MTU_VALUESm,
    TRUNK_MEMBERm,
    TRUNK_BITMAPm,
    MMU_REPL_GROUP_INFO0m,
    MMU_REPL_HEAD_TBLm,
    MMU_REPL_LIST_TBLm,
    MMU_REPL_GROUP_INITIAL_COPY_COUNTm,
    EGR_DGPP_TO_NHIm,
    EGR_DGPP_TO_NHI_MODBASEm,
    EGR_DVP_ATTRIBUTEm,
    EGR_DVP_ATTRIBUTE_1m,
    EGR_GPP_ATTRIBUTESm,
    EGR_IPMCm,
    EGR_L3_INTFm,
    EGR_L3_NEXT_HOPm,
    EGR_PORTm,
    EGR_VFIm,
    EGR_FRAGMENT_ID_TABLEm,
    EGR_IP_TUNNELm,
    EGR_VLANm,
    EGR_VLAN_VFI_MEMBERSHIPm,
    EGR_VPLAG_GROUPm,
    EGR_VPLAG_MEMBERm,
    EGR_VLAN_XLATEm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "vxlan" feature for apache chipset. */
techsupport_reg_t techsupport_vxlan_apache_reg_list[] = {
    {EGR_CONFIG_1r, register_type_global},
    {EGR_NIV_ETHERTYPEr, register_type_global},
    {EGR_NIV_ETHERTYPE_2r, register_type_global},
    {EGR_TUNNEL_ID_MASKr, register_type_global},
    {EGR_VXLAN_CONTROLr, register_type_global},
    {ING_MISC_CONFIG2r, register_type_global},
    {NIV_ETHERTYPEr, register_type_global},
    {PORT_INITIAL_COPY_COUNT_WIDTHr, register_type_global},
    {RTAG7_HASH_CONTROL_3r, register_type_global},
    {RTAG7_HASH_FIELD_BMAP_3r, register_type_global},
    {RTAG7_HASH_SEED_Ar, register_type_global},
    {RTAG7_HASH_SEED_Br, register_type_global},
    {VXLAN_CONTROLr, register_type_global},
    {VXLAN_DEFAULT_NETWORK_SVPr, register_type_global},
    {ING_CONFIG_64r, register_type_global},
    {MMU_TOQ_REPL_PORT_AGG_MAPr, register_type_global},
    {MMU_ENQ_REPL_PORT_AGG_MAPr, register_type_global},
    {MMU_ENQ_LOGICAL_PORT_TO_PORT_AGG_MAPr, register_type_global},
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "vxlan" feature for apache chipset. */
techsupport_data_t techsupport_vxlan_apache_data = {
    techsupport_vxlan_diag_cmdlist,
    techsupport_vxlan_apache_reg_list,
    techsupport_vxlan_apache_memory_table_list,
    techsupport_vxlan_apache_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_vxlan_sw_dump_cmdlist,
    techsupport_vxlan_apache_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};


