/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    trident3_vlan.c
 * Purpose: Maintains all the debug information for vlan
 *          feature for trident3.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_vlan_diag_cmdlist[];
extern char * techsupport_vlan_sw_dump_cmdlist[];

/* "vlan" feature's diag command list only for Trident3 */
char * techsupport_vlan_trident3_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "vlan" feature's software dump command list only for Trident3 */
char * techsupport_vlan_trident3_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "vlan" feature for Trident3 chipset. */
static soc_mem_t techsupport_vlan_trident3_memory_table_list[] = {
    SOURCE_TRUNK_MAP_MODBASEm,
    ING_DEVICE_PORTm,
    ING_VLAN_RANGEm,
    SOURCE_VP_2m,
    LPORT_TABm,
    SOURCE_TRUNK_MAP_TABLEm,
    ING_VLAN_TAG_ACTION_PROFILE_1m,
    ING_VLAN_TAG_ACTION_PROFILE_2m,
    VLAN_PROTOCOLm,
    VLAN_PROTOCOL_DATAm,
    VLAN_SUBNETm,
    VLAN_XLATE_1_LPm,
    VLAN_XLATE_2_LPm,
    EGR_VLAN_XLATE_1_LPm,
    EGR_VLAN_XLATE_2_LPm,
    VLAN_ATTRS_1m,
    VLAN_XLATE_1_DOUBLEm,
    VLAN_XLATE_2_DOUBLEm,
    VLAN_XLATE_1_SINGLEm,
    VLAN_XLATE_2_SINGLEm,
    SOURCE_VPm,
    ING_VLAN_VFI_MEMBERSHIPm,
    STG_TABm,
    VLAN_PROFILE_TABm,
    VLAN_TABm,
    L2_HITDA_ONLYm,
    L2_HITSA_ONLYm,
    L2_USER_ENTRYm,
    L2_LEARN_INSERT_FAILUREm,
    L2_BULKm,
    L2_MOD_FIFOm,
    L2Xm,
    ING_DVP_TABLEm,
    TRUNK_CBL_TABLEm,
    PORT_CBL_TABLE_MODBASEm,
    ING_DVP_2_TABLEm,
    L2MCm,
    TRUNK_GROUPm,
    TRUNK_MEMBERm,
    VLAN_PROFILE_2m,
    VFI_PROFILE_2m,
    ING_EN_EFILTER_BITMAPm,
    TRUNK_BITMAPm,
    EGR_DVP_ATTRIBUTEm,
    EGR_GPP_ATTRIBUTESm,
    EGR_MAP_MHm,
    EGR_PORTm,
    EGR_VLAN_CONTROL_1m,
    EGR_VLANm,
    EGR_VLAN_TAG_ACTION_PROFILEm,
    EGR_VLAN_VFI_MEMBERSHIPm,
    EGR_VLAN_XLATE_1_DOUBLEm,
    EGR_VLAN_XLATE_2_DOUBLEm,
    EGR_VLAN_STGm,
    EGR_VLAN_CONTROL_3m,
    EGR_VLAN_CONTROL_2m,
    VLAN_XLATE_1_HASH_CONTROLm,
    VLAN_XLATE_2_HASH_CONTROLm,
    L2_DESTINATION_STRENGTH_PROFILEm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "vlan" feature for Trident3 chipset. */
techsupport_reg_t techsupport_vlan_trident3_reg_list[] = {
    {CPU_CONTROL_1r, register_type_global},
    {EGR_CONFIGr, register_type_global},
    {EGR_INGRESS_PORT_TPID_SELECTr, register_type_global},
    {EGR_PVLAN_EPORT_CONTROLr, register_type_global},
    {ING_CONFIG_64r, register_type_global},
    {PORT_INITIAL_COPY_COUNT_WIDTHr, register_type_global},
    {VLAN_CTRLr, register_type_global},
    {ING_OUTER_TPID_0r, register_type_global},
    {ING_OUTER_TPID_1r, register_type_global},
    {ING_OUTER_TPID_2r, register_type_global},
    {ING_OUTER_TPID_3r, register_type_global},
    {PARSER1_ING_OUTER_TPIDr, register_type_global},
    {PARSER2_ING_OUTER_TPIDr, register_type_global},
    {ING_MPLS_INNER_TPIDr, register_type_global},
    {EGR_OUTER_TPIDr, register_type_global},
    {CBL_ATTRIBUTEr, register_type_global},
    {MC_CONTROL_1r, register_type_global},
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "vlan" feature for Trident3 chipset. */
techsupport_data_t techsupport_vlan_trident3_data = {
    techsupport_vlan_diag_cmdlist,
    techsupport_vlan_trident3_reg_list,
    techsupport_vlan_trident3_memory_table_list,
    techsupport_vlan_trident3_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_vlan_sw_dump_cmdlist,
    techsupport_vlan_trident3_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};

