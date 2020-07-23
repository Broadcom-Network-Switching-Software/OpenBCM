/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    trident3_fflow.c
 * Purpose: Maintains all the debug information for flex_flow
 *          feature for trident3.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_flex_flow_diag_cmdlist[];
extern char * techsupport_flex_flow_sw_dump_cmdlist[];

/* "flex_flow" feature's diag command list only for trident3 */
char * techsupport_flex_flow_trident3_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "VXLAN" feature's software dump command list only for trident3 */
char * techsupport_flex_flow_trident3_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "flex_flow" feature for trident3 chipset. */
static soc_mem_t techsupport_flex_flow_trident3_memory_table_list[] = {
    SOURCE_TRUNK_MAP_MODBASEm,
    ING_DEVICE_PORTm,
    SOURCE_VP_2m,
    LPORT_TABm,
    SOURCE_TRUNK_MAP_TABLEm,
    ING_VLAN_TAG_ACTION_PROFILEm,
    VLAN_XLATE_1_DOUBLEm,
    VLAN_XLATE_2_DOUBLEm,
    L3_IIFm,
    MY_STATION_TCAMm,
    MY_STATION_TCAM_2m,
    MY_STATION_PROFILE_1m,
    MY_STATION_PROFILE_2m,
    SOURCE_VPm,
    VFIm,
    VFI_PROFILEm,
    ING_DVP_TABLEm,
    MPLS_ENTRYm,
    L2Xm,
    RTAG7_PORT_BASED_HASHm,
    STG_TABm,
    VLAN_PROFILE_TABm,
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
    L3_ECMP_COUNTm,
    L3_MTU_VALUESm,
    L3_TUNNELm,
    TRUNK_GROUPm,
    TRUNK_MEMBERm,
    TRUNK_BITMAPm,
    EPC_LINK_BMAPm,
    MMU_REPL_GROUP_INITIAL_COPY_COUNT_SC0m,
    MMU_REPL_GROUP_INITIAL_COPY_COUNT_SC1m,
    MMU_REPL_GROUP_INFO_TBLm,
    MMU_REPL_HEAD_TBL_SPLIT0m,
    MMU_REPL_HEAD_TBL_SPLIT1m,
    MMU_REPL_LIST_TBLm,
    EGR_DVP_ATTRIBUTEm,
    EGR_IPMCm,
    EGR_L3_INTFm,
    EGR_L3_NEXT_HOPm,
    EGR_PORTm,
    EGR_VFIm,
    EGR_VLAN_XLATE_1_DOUBLEm,
    EGR_VLAN_XLATE_2_DOUBLEm,
    EGR_FRAGMENT_ID_TABLEm,
    EGR_GPP_ATTRIBUTESm,
    EGR_GPP_ATTRIBUTES_MODBASEm,
    EGR_IP_TUNNELm,
    EGR_VLANm,
    EGR_VLAN_STGm,
    EGR_VPLAG_GROUPm,
    EGR_VPLAG_MEMBERm,
    EGR_IP_CUT_THRU_CLASSm,
    EGR_ENABLEm,
    EGR_DGPP_TO_NHI_MODBASEm,
    EGR_DGPP_TO_NHIm,
    EGR_VLAN_CONTROL_1m,
    EGR_VLAN_CONTROL_2m,
    EGR_VLAN_CONTROL_3m,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "flex_flow" feature for trident3 chipset. */
techsupport_reg_t techsupport_flex_flow_trident3_reg_list[] = {
    {EGR_CONFIG_1r, register_type_global},
    {EGR_NIV_ETHERTYPE_2r, register_type_global},
    {EGR_TUNNEL_ID_MASKr, register_type_global},
    {EGR_VXLAN_CONTROLr, register_type_global},
    {ING_MISC_CONFIG2r, register_type_global},
    {NIV_ETHERTYPEr, register_type_global},
    {PORT_INITIAL_COPY_COUNT_WIDTHr, register_type_per_port},
    {RTAG7_HASH_CONTROL_3r, register_type_global},
    {RTAG7_HASH_FIELD_BMAP_3r, register_type_global},
    {RTAG7_HASH_SEED_Ar, register_type_global},
    {RTAG7_HASH_SEED_Br, register_type_global},
    {VXLAN_CONTROLr, register_type_global},
    {CPU_CONTROL_Mr, register_type_global},
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "flex_flow" feature for Trident3 chipset. */
techsupport_data_t techsupport_flex_flow_trident3_data = {
    techsupport_flex_flow_diag_cmdlist,
    techsupport_flex_flow_trident3_reg_list,
    techsupport_flex_flow_trident3_memory_table_list,
    techsupport_flex_flow_trident3_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_flex_flow_sw_dump_cmdlist,
    techsupport_flex_flow_trident3_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */

};

