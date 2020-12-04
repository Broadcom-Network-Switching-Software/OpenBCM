/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    saber2_niv.c
 * Purpose: Maintains all the debug information for niv
 *          feature for saber2.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_niv_diag_cmdlist[];
extern char * techsupport_niv_sw_dump_cmdlist[];

/* "niv" feature's diag command list valid only for Saber2 */
char * techsupport_niv_saber2_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "niv" feature's software dump command list valid only for Saber2 */
char * techsupport_niv_saber2_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "niv" feature for Saber2 chipset. */
static soc_mem_t techsupport_niv_saber2_memory_table_list[] = {
    ING_PHYSICAL_PORT_TABLEm,
    PORT_TABm,
    SOURCE_TRUNK_MAP_MODBASEm,
    SUBPORT_TAG_TO_PP_PORT_MAPm,
    SYSTEM_CONFIG_TABLEm,
    SYSTEM_CONFIG_TABLE_MODBASEm,
    ING_TRILL_PARSE_CONTROLm,
    SOURCE_TRUNK_MAP_TABLEm,
    LPORT_TABm,
    MPLS_ENTRYm,
    VLAN_MACm,
    VLAN_MAC_OVERFLOWm,
    VLAN_PROTOCOLm,
    VLAN_PROTOCOL_DATAm,
    VLAN_SUBNETm,
    VLAN_XLATEm,
    ING_VLAN_TAG_ACTION_PROFILEm,
    ING_NETWORK_PRUNE_CONTROLm,
    ING_TRILL_PAYLOAD_PARSE_CONTROLm,
    MY_STATION_TCAMm,
    SOURCE_VPm,
    VFIm,
    VFI_1m,
    VLAN_TABm,
    VLAN_MPLSm,
    STG_TABm,
    L2_USER_ENTRYm,
    L2Xm,
    L3_ENTRY_IPV4_UNICASTm,
    ING_DVP_TABLEm,
    INITIAL_ING_L3_NEXT_HOPm,
    ING_PRI_CNG_MAPm,
    EGR_MASK_MODBASEm,
    ING_L3_NEXT_HOPm,
    L2MCm,
    EGR_MASKm,
    ING_EN_EFILTER_BITMAPm,
    MULTIPASS_LOOPBACK_BITMAPm,
    NONUCAST_TRUNK_BLOCK_MASKm,
    PP_PORT_TO_PHYSICAL_PORT_MAPm,
    EGR_DVP_ATTRIBUTEm,
    EGR_IPMCm,
    EGR_L3_INTFm,
    EGR_L3_NEXT_HOPm,
    EGR_MAC_DA_PROFILEm,
    EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
    EGR_NETWORK_PRUNE_CONTROLm,
    EGR_PHYSICAL_PORTm,
    EGR_PORTm,
    EGR_QCN_CNM_CONTROL_TABLEm,
    EGR_QUEUE_TO_PP_PORT_MAPm,
    EGR_SUBPORT_TAG_DOT1P_MAPm,
    EGR_TRILL_PARSE_CONTROLm,
    EGR_TRILL_TREE_PROFILEm,
    EGR_GPP_ATTRIBUTESm,
    EGR_GPP_ATTRIBUTES_MODBASEm,
    EGR_PRI_CNG_MAPm,
    EGR_VLANm,
    EGR_VLAN_TAG_ACTION_PROFILEm,
    EGR_VLAN_XLATEm,
    EGR_MIRROR_ENCAP_DATA_2m,
    EGR_TRILL_PARSE_CONTROL_2m,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "niv" feature for Saber2 chipset. */
techsupport_reg_t techsupport_niv_saber2_reg_list[] = {
    {CPU_CONTROL_0r, register_type_global},
    {CPU_CONTROL_1r, register_type_global},
    {DOS_CONTROLr, register_type_global},
    {EGR_CONFIGr, register_type_global},
    {EGR_CONFIG_1r, register_type_global},
    {EGR_ETAG_MULTICAST_RANGEr, register_type_global},
    {EGR_MIRROR_SELECTr, register_type_global},
    {EGR_MTUr, register_type_global},
    {EGR_NIV_CONFIGr, register_type_global},
    {EGR_NIV_ETHERTYPEr, register_type_global},
    {EGR_NIV_ETHERTYPE_2r, register_type_global},
    {EGR_OUTER_TPIDr, register_type_global},
    {EGR_PE_ETHERTYPEr, register_type_global},
    {EGR_PE_ETHERTYPE_2r, register_type_global},
    {EGR_VLAN_CONTROL_1r, register_type_global},
    {EGR_VLAN_CONTROL_3r, register_type_global},
    {EGR_VLAN_XLATE_HASH_CONTROLr, register_type_global},
    {ETAG_MULTICAST_RANGEr, register_type_global},
    {HASH_CONTROLr, register_type_global},
    {IGMP_MLD_PKT_CONTROLr, register_type_global},
    {ING_CONFIGr, register_type_global},
    {ING_MISC_CONFIGr, register_type_global},
    {ING_MPLS_TPIDr, register_type_global},
    {ING_NIV_CONFIGr, register_type_global},
    {ING_NIV_RX_FRAMES_ERROR_DROPr, register_type_global},
    {ING_NIV_RX_FRAMES_FORWARDING_DROPr, register_type_global},
    {ING_NIV_RX_FRAMES_VLAN_TAGGEDr, register_type_global},
    {ING_OUTER_TPIDr, register_type_global},
    {NIV_ETHERTYPEr, register_type_global},
    {PE_ETHERTYPEr, register_type_global},
    {VLAN_CTRLr, register_type_global},
    {VLAN_XLATE_HASH_CONTROLr, register_type_global},
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "niv" feature for Saber2 chipset. */
techsupport_data_t techsupport_niv_saber2_data = {
    techsupport_niv_diag_cmdlist,
    techsupport_niv_saber2_reg_list,
    techsupport_niv_saber2_memory_table_list,
    techsupport_niv_saber2_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_niv_sw_dump_cmdlist,
    techsupport_niv_saber2_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};

