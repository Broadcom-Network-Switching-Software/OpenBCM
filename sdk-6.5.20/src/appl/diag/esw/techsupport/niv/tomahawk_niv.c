/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    tomahawk_niv.c
 * Purpose: Maintains all the debug information for niv
 *          feature for tomahawk.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_niv_diag_cmdlist[];
extern char * techsupport_niv_sw_dump_cmdlist[];

/* "niv" feature's diag command list valid only for Tomahawk */
char * techsupport_niv_tomahawk_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "NIV" feature's software dump command list valid only for Tomahawk */
char * techsupport_niv_tomahawk_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/* Structure that maintains memory list for
 * "niv" feature for Tomahawk chipset. */
static soc_mem_t techsupport_niv_tomahawk_memory_table_list[] = {
    SOURCE_TRUNK_MAP_MODBASEm,
    SOURCE_VP_2m,
    ING_VLAN_TAG_ACTION_PROFILEm,
    VLAN_MACm,
    VLAN_PROTOCOLm,
    VLAN_PROTOCOL_DATAm,
    VLAN_SUBNETm,
    VLAN_XLATEm,
    L3_IIFm,
    MY_STATION_TCAMm,
    SOURCE_VPm,
    VLAN_MPLSm,
    ING_DVP_TABLEm,
    L3_DEFIPm,
    L3_ENTRY_IPV4_MULTICASTm,
    L3_ENTRY_IPV6_MULTICASTm,
    RTAG7_PORT_BASED_HASHm,
    VRFm,
    INITIAL_ING_L3_NEXT_HOPm,
    ING_DVP_2_TABLEm,
    ING_L3_NEXT_HOPm,
    L2MCm,
    L3_IPMCm,
    L3_MTU_VALUESm,
    EGR_DVP_ATTRIBUTEm,
    EGR_IPMCm,
    EGR_L3_INTFm,
    EGR_L3_NEXT_HOPm,
    EGR_PORTm,
    EGR_GPP_ATTRIBUTESm,
    EGR_GPP_ATTRIBUTES_MODBASEm,
    EGR_VLANm,
    EGR_TRILL_RBRIDGE_NICKNAMESm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "niv" feature for Tomahawk chipset. */
techsupport_reg_t techsupport_niv_tomahawk_reg_list[] = {
    {CPU_CONTROL_0r, register_type_global },
    {EGR_NIV_CONFIGr, register_type_global },
    {EGR_NIV_ETHERTYPEr, register_type_global },
    {EGR_NIV_ETHERTYPE_2r, register_type_global },
    {EGR_Q_BEGINr, register_type_global },
    {EGR_VLAN_CONTROL_1r, register_type_global },
    {EGR_VP_VLAN_MEMBERSHIP_HASH_CONTROLr, register_type_global },
    {HG_LOOKUP_DESTINATIONr, register_type_global },
    {IDB_OBM0_NIV_ETHERTYPE_PIPE0r, register_type_global },
    {IDB_OBM0_NIV_ETHERTYPE_PIPE1r, register_type_global },
    {IDB_OBM0_NIV_ETHERTYPE_PIPE2r, register_type_global },
    {IDB_OBM0_NIV_ETHERTYPE_PIPE3r, register_type_global },
    {IDB_OBM1_NIV_ETHERTYPE_PIPE0r, register_type_global },
    {IDB_OBM1_NIV_ETHERTYPE_PIPE1r, register_type_global },
    {IDB_OBM1_NIV_ETHERTYPE_PIPE2r, register_type_global },
    {IDB_OBM1_NIV_ETHERTYPE_PIPE3r, register_type_global },
    {IDB_OBM2_NIV_ETHERTYPE_PIPE0r, register_type_global },
    {IDB_OBM2_NIV_ETHERTYPE_PIPE1r, register_type_global },
    {IDB_OBM2_NIV_ETHERTYPE_PIPE2r, register_type_global },
    {IDB_OBM2_NIV_ETHERTYPE_PIPE3r, register_type_global },
    {IDB_OBM3_NIV_ETHERTYPE_PIPE0r, register_type_global },
    {IDB_OBM3_NIV_ETHERTYPE_PIPE1r, register_type_global },
    {IDB_OBM3_NIV_ETHERTYPE_PIPE2r, register_type_global },
    {IDB_OBM3_NIV_ETHERTYPE_PIPE3r, register_type_global },
    {IDB_OBM4_NIV_ETHERTYPE_PIPE0r, register_type_global },
    {IDB_OBM4_NIV_ETHERTYPE_PIPE1r, register_type_global },
    {IDB_OBM4_NIV_ETHERTYPE_PIPE2r, register_type_global },
    {IDB_OBM4_NIV_ETHERTYPE_PIPE3r, register_type_global },
    {IDB_OBM5_NIV_ETHERTYPE_PIPE0r, register_type_global },
    {IDB_OBM5_NIV_ETHERTYPE_PIPE1r, register_type_global },
    {IDB_OBM5_NIV_ETHERTYPE_PIPE2r, register_type_global },
    {IDB_OBM5_NIV_ETHERTYPE_PIPE3r, register_type_global },
    {IDB_OBM6_NIV_ETHERTYPE_PIPE0r, register_type_global },
    {IDB_OBM6_NIV_ETHERTYPE_PIPE1r, register_type_global },
    {IDB_OBM6_NIV_ETHERTYPE_PIPE2r, register_type_global },
    {IDB_OBM6_NIV_ETHERTYPE_PIPE3r, register_type_global },
    {IDB_OBM7_NIV_ETHERTYPE_PIPE0r, register_type_global },
    {IDB_OBM7_NIV_ETHERTYPE_PIPE1r, register_type_global },
    {IDB_OBM7_NIV_ETHERTYPE_PIPE2r, register_type_global },
    {IDB_OBM7_NIV_ETHERTYPE_PIPE3r, register_type_global },
    {ING_CONFIG_64r, register_type_global },
    {ING_COS_MODE_64r, register_type_global },
    {ING_NIV_CONFIGr, register_type_global },
    {ING_NIV_RX_FRAMES_ERROR_DROP_64r, register_type_global },
    {ING_NIV_RX_FRAMES_FORWARDING_DROP_64r, register_type_global },
    {ING_NIV_RX_FRAMES_VLAN_TAGGED_64r, register_type_global },
    {NIV_ETHERTYPEr, register_type_global },
    {PE_ETHERTYPEr, register_type_global },
    {PORT_INITIAL_COPY_COUNT_WIDTHr, register_type_global },
    {VLAN_CTRLr, register_type_global },
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "niv" feature for Tomahawk chipset. */
techsupport_data_t techsupport_niv_tomahawk_data = {
    techsupport_niv_diag_cmdlist,
    techsupport_niv_tomahawk_reg_list,
    techsupport_niv_tomahawk_memory_table_list,
    techsupport_niv_tomahawk_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_niv_sw_dump_cmdlist,
    techsupport_niv_tomahawk_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};


