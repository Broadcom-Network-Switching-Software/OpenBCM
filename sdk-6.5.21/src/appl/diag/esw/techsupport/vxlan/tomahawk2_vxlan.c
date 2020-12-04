/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    tomahawk2_vxlan.c
 * Purpose: Maintains all the debug information for vxlan
 *          feature for tomahawk2.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_vxlan_diag_cmdlist[];
extern char * techsupport_vxlan_sw_dump_cmdlist[];

/* "vxlan" feature's diag command list only for Tomahawk */
char * techsupport_vxlan_tomahawk2_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "VXLAN" feature's software dump command list only for Tomahawk */
char * techsupport_vxlan_tomahawk2_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "vxlan" feature for tomahawk2 chipset. */
static soc_mem_t techsupport_vxlan_tomahawk2_memory_table_list[] = {
    SOURCE_TRUNK_MAP_MODBASEm,
    PORT_TABm,
    SOURCE_VP_2m,
    LPORT_TABm,
    SOURCE_TRUNK_MAP_TABLEm,
    ING_VLAN_TAG_ACTION_PROFILEm,
    MPLS_ENTRYm,
    VLAN_XLATEm,
    L3_IIFm,
    MY_STATION_TCAMm,
    SOURCE_VPm,
    VFIm,
    VLAN_MPLSm,
    ING_DVP_TABLEm,
    L2_HITDA_ONLYm,
    L2_HITSA_ONLYm,
    L2_USER_ENTRYm,
    L3_ENTRY_IPV4_MULTICASTm,
    L3_ENTRY_IPV6_MULTICASTm,
    L3_IIF_PROFILEm,
    RTAG7_PORT_BASED_HASHm,
    STG_TABm,
    VLAN_TABm,
    INITIAL_L3_ECMPm,
    INITIAL_L3_ECMP_GROUPm,
    INITIAL_PROT_NHI_TABLEm,
    INITIAL_ING_L3_NEXT_HOPm,
    ING_DVP_2_TABLEm,
    ING_L3_NEXT_HOPm,
    L2MCm,
    L3_ECMPm,
    L3_IPMCm,
    L3_IPMC_REMAPm,
    L3_ECMP_COUNTm,
    L3_MTU_VALUESm,
    TRUNK_GROUPm,
    TRUNK_MEMBERm,
    TRUNK_BITMAPm,
    VLAN_PROFILE_TABm,
    EPC_LINK_BMAPm,
    L2Xm,
    MMU_REPL_GROUP_INITIAL_COPY_COUNTm,
    MMU_REPL_GROUP_INITIAL_COPY_COUNT_SC0m,
    MMU_REPL_GROUP_INITIAL_COPY_COUNT_SC1m,
    MMU_REPL_GROUP_INFO_TBL_PIPE0m,
    MMU_REPL_GROUP_INFO_TBL_PIPE1m,
    MMU_REPL_GROUP_INFO_TBL_PIPE2m,
    MMU_REPL_GROUP_INFO_TBL_PIPE3m,
    MMU_REPL_HEAD_TBL_PIPE0m,
    MMU_REPL_HEAD_TBL_PIPE1m,
    MMU_REPL_HEAD_TBL_PIPE2m,
    MMU_REPL_HEAD_TBL_PIPE3m,
    MMU_REPL_LIST_TBL_PIPE0m,
    MMU_REPL_LIST_TBL_PIPE1m,
    MMU_REPL_LIST_TBL_PIPE2m,
    MMU_REPL_LIST_TBL_PIPE3m,
    EGR_DVP_ATTRIBUTEm,
    EGR_DVP_ATTRIBUTE_1m,
    EGR_IPMCm,
    EGR_L3_INTFm,
    EGR_L3_NEXT_HOPm,
    EGR_PORTm,
    EGR_VFIm,
    EGR_FRAGMENT_ID_TABLEm,
    EGR_GPP_ATTRIBUTESm,
    EGR_GPP_ATTRIBUTES_MODBASEm,
    EGR_IP_TUNNELm,
    EGR_VLANm,
    EGR_VLAN_STGm,
    EGR_VLAN_XLATEm,
    EGR_VPLAG_GROUPm,
    EGR_VPLAG_MEMBERm,
    EGR_TRILL_RBRIDGE_NICKNAMESm,
    EGR_IP_CUT_THRU_CLASSm,
    EGR_ENABLEm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "vxlan" feature for tomahawk2 chipset. */
techsupport_reg_t techsupport_vxlan_tomahawk2_reg_list[] = {
    {EGR_CONFIG_1r, register_type_global},
    {EGR_NIV_ETHERTYPEr, register_type_global},
    {EGR_NIV_ETHERTYPE_2r, register_type_global},
    {EGR_PORT_TO_NHI_MAPPINGr, register_type_per_port},
    {EGR_TUNNEL_ID_MASKr, register_type_global},
    {EGR_VXLAN_CONTROLr, register_type_global},
    {ING_CONFIG_64r, register_type_global },
    {ING_MISC_CONFIG2r, register_type_global},
    {NIV_ETHERTYPEr, register_type_global},
    {PORT_INITIAL_COPY_COUNT_WIDTHr, register_type_per_port},
    {RTAG7_HASH_CONTROL_3_64r, register_type_global },
    {RTAG7_HASH_FIELD_BMAP_3r, register_type_global},
    {RTAG7_HASH_SEED_Ar, register_type_global},
    {RTAG7_HASH_SEED_Br, register_type_global},
    {VXLAN_CONTROLr, register_type_global},
    {VXLAN_DEFAULT_NETWORK_SVPr, register_type_global},
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "vxlan" feature for Trident2plus chipset. */
techsupport_data_t techsupport_vxlan_tomahawk2_data = {
    techsupport_vxlan_diag_cmdlist,
    techsupport_vxlan_tomahawk2_reg_list,
    techsupport_vxlan_tomahawk2_memory_table_list,
    techsupport_vxlan_tomahawk2_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_vxlan_sw_dump_cmdlist,
    techsupport_vxlan_tomahawk2_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */

};

