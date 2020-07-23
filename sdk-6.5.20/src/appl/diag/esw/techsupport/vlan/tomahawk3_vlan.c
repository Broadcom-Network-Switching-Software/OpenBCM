/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: tomahawk3_vlan.c
 *
 * Purpose:
 *    Maintains all the debug information for vlan feature for tomahawk3
 *
 * Note: This file is auto-generated. Manual edits
 *       to it will be lost when it is auto-regenerated.
 */

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_vlan_diag_cmdlist[];
extern char * techsupport_vlan_sw_dump_cmdlist[];

/* "vlan" feature's diag command list valid only for tomahawk3 */
char * techsupport_vlan_tomahawk3_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "vlan" feature's software dump command list valid only for tomahawk3 */
char * techsupport_vlan_tomahawk3_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "vlan" feature for tomahawk3 chipset. */
static soc_mem_t techsupport_vlan_tomahawk3_memory_table_list[] = {
    BCAST_BLOCK_MASKm,
    EGR_ENABLEm,
    EGR_FRAGMENT_ID_TABLE_PIPE0m,
    EGR_FRAGMENT_ID_TABLE_PIPE1m,
    EGR_FRAGMENT_ID_TABLE_PIPE2m,
    EGR_FRAGMENT_ID_TABLE_PIPE3m,
    EGR_FRAGMENT_ID_TABLE_PIPE4m,
    EGR_FRAGMENT_ID_TABLE_PIPE5m,
    EGR_FRAGMENT_ID_TABLE_PIPE6m,
    EGR_FRAGMENT_ID_TABLE_PIPE7m,
    EGR_GPP_ATTRIBUTESm,
    EGR_IP_TUNNELm,
    EGR_L3_INTFm,
    EGR_MASKm,
    EGR_MAX_USED_ENTRIESm,
    EGR_PER_PORT_BUFFER_SFT_RESETm,
    EGR_PORTm,
    EGR_VLAN_STGm,
    EGR_VLAN_TAG_ACTION_PROFILEm,
    EGR_VLANm,
    EMIRROR_CONTROL2m,
    EPC_LINK_BMAPm,
    ING_EN_EFILTER_BITMAPm,
    ING_UNTAGGED_PHBm,
    ING_VLAN_TAG_ACTION_PROFILEm,
    KNOWN_MCAST_BLOCK_MASKm,
    L2MCm,
    L2Xm,
    L2_HITDA_ONLYm,
    L2_HITSA_ONLYm,
    L2_USER_ENTRYm,
    L3_IIFm,
    L3_MTU_VALUESm,
    LPORT_TABm,
    MAC_BLOCKm,
    MY_STATION_TCAMm,
    NONUCAST_TRUNK_BLOCK_MASKm,
    PORT_BITMAP_PROFILEm,
    SOURCE_TRUNK_MAP_TABLEm,
    STG_TAB_2m,
    STG_TABm,
    UNKNOWN_MCAST_BLOCK_MASKm,
    UNKNOWN_UCAST_BLOCK_MASKm,
    VFP_POLICY_TABLE_PIPE0m,
    VFP_POLICY_TABLE_PIPE1m,
    VFP_POLICY_TABLE_PIPE2m,
    VFP_POLICY_TABLE_PIPE3m,
    VFP_POLICY_TABLE_PIPE4m,
    VFP_POLICY_TABLE_PIPE5m,
    VFP_POLICY_TABLE_PIPE6m,
    VFP_POLICY_TABLE_PIPE7m,
    VFP_TCAM_PIPE0m,
    VFP_TCAM_PIPE1m,
    VFP_TCAM_PIPE2m,
    VFP_TCAM_PIPE3m,
    VFP_TCAM_PIPE4m,
    VFP_TCAM_PIPE5m,
    VFP_TCAM_PIPE6m,
    VFP_TCAM_PIPE7m,
    VLAN_2_TABm,
    VLAN_MPLSm,
    VLAN_PROFILE_2m,
    VLAN_PROFILE_TABm,
    VLAN_TABm,
    VRFm,
    PORT_TABm,
    MMU_REPL_LIST_TBLm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "vlan" feature for tomahawk3 chipset. */
techsupport_reg_t techsupport_vlan_tomahawk3_reg_list[] = {
    { CDMAC_RX_VLAN_TAGr, register_type_per_port },
    { CMIC_RPE_PKT_VLANr, register_type_global },
    { CPU_CONTROL_0r, register_type_global },
    { CPU_CONTROL_1r, register_type_global },
    { CPU_CONTROL_Mr, register_type_global },
    { EGR_CONFIGr, register_type_global },
    { EGR_CONFIG_1r, register_type_global },
    { EGR_INGRESS_PORT_TPID_SELECTr, register_type_per_port },
    { EGR_OUTER_TPID_0r, register_type_global },
    { EGR_OUTER_TPID_1r, register_type_global },
    { EGR_OUTER_TPID_2r, register_type_global },
    { EGR_OUTER_TPID_3r, register_type_global },
    { EGR_OUTER_TPIDr, register_type_global },
    { EGR_PORT_1r, register_type_per_port },
    { EGR_PVLAN_EPORT_CONTROLr, register_type_per_port },
    { EGR_TUNNEL_ID_MASKr, register_type_global },
    { EGR_VLAN_CONTROL_1r, register_type_per_port },
    { EGR_VLAN_CONTROL_2r, register_type_per_port },
    { EGR_VLAN_CONTROL_3r, register_type_per_port },
    { IGMP_MLD_PKT_CONTROLr, register_type_global },
    { ING_CONFIG_64r, register_type_global },
    { ING_OUTER_TPID_0r, register_type_global },
    { ING_OUTER_TPID_1r, register_type_global },
    { ING_OUTER_TPID_2r, register_type_global },
    { ING_OUTER_TPID_3r, register_type_global },
    { ING_OUTER_TPIDr, register_type_global },
    { IRSEL_SER_CONTROLr, register_type_global },
    { MC_CONTROL_4r, register_type_global },
    { MC_CONTROL_5r, register_type_global },
    { PROTOCOL_PKT_CONTROLr, register_type_global },
    { VFP_KEY_CONTROL_1_PIPE0r, register_type_global },
    { VFP_KEY_CONTROL_1_PIPE1r, register_type_global },
    { VFP_KEY_CONTROL_1_PIPE2r, register_type_global },
    { VFP_KEY_CONTROL_1_PIPE3r, register_type_global },
    { VFP_KEY_CONTROL_1_PIPE4r, register_type_global },
    { VFP_KEY_CONTROL_1_PIPE5r, register_type_global },
    { VFP_KEY_CONTROL_1_PIPE6r, register_type_global },
    { VFP_KEY_CONTROL_1_PIPE7r, register_type_global },
    { VFP_KEY_CONTROL_2_PIPE0r, register_type_global },
    { VFP_KEY_CONTROL_2_PIPE1r, register_type_global },
    { VFP_KEY_CONTROL_2_PIPE2r, register_type_global },
    { VFP_KEY_CONTROL_2_PIPE3r, register_type_global },
    { VFP_KEY_CONTROL_2_PIPE4r, register_type_global },
    { VFP_KEY_CONTROL_2_PIPE5r, register_type_global },
    { VFP_KEY_CONTROL_2_PIPE6r, register_type_global },
    { VFP_KEY_CONTROL_2_PIPE7r, register_type_global },
    { VLAN_CTRLr, register_type_global },
    { XLMAC_RX_VLAN_TAGr, register_type_per_port },
    { INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains diag cmdlist, reg_list, mem_list  for
 * "vlan" feature for tomahawk3 chipset. */
techsupport_data_t techsupport_vlan_tomahawk3_data = {
    techsupport_vlan_diag_cmdlist,
    techsupport_vlan_tomahawk3_reg_list,
    techsupport_vlan_tomahawk3_memory_table_list,
    techsupport_vlan_tomahawk3_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_vlan_sw_dump_cmdlist,
    techsupport_vlan_tomahawk3_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};
