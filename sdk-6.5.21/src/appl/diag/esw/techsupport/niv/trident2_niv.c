/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    trident2_niv.c
 * Purpose: Maintains all the debug information for niv
 *          feature for trident2.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_niv_diag_cmdlist[];
extern char * techsupport_niv_sw_dump_cmdlist[];

/* "niv" feature's diag command list valid only for Trident2 */
char * techsupport_niv_trident2_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "NIV" feature's software dump command list valid only for Trident2 */
char * techsupport_niv_trident2_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/* Structure that maintains memory list for
 * "niv" feature for Trident2 chipset. */
static soc_mem_t techsupport_niv_trident2_memory_table_list[] = {
    SOURCE_TRUNK_MAP_MODBASEm,
    PORT_TABm,
    SOURCE_VP_2m,
    LPORT_TABm,
    SOURCE_TRUNK_MAP_TABLEm,
    ING_VLAN_TAG_ACTION_PROFILEm,
    VLAN_XLATEm,
    MY_STATION_TCAMm,
    SOURCE_VPm,
    VLAN_MPLSm,
    L3_IIFm,
    VLAN_PROFILE_TABm,
    VLAN_TABm,
    L2Xm,
    L3_ENTRY_IPV4_MULTICASTm,
    L3_ENTRY_IPV6_MULTICASTm,
    L3_DEFIPm,
    ING_DVP_TABLEm,
    INITIAL_ING_L3_NEXT_HOPm,
    L3_IPMC_1m,
    ING_DVP_2_TABLEm,
    ING_L3_NEXT_HOPm,
    L2MCm,
    L3_IPMCm,
    L3_MTU_VALUESm,
    MMU_REPL_GROUP_INFO0m,
    MMU_REPL_GROUP_INFO1m,
    MMU_REPL_HEAD_TBLm,
    MMU_REPL_LIST_TBLm,
    MMU_REPL_GROUP_INITIAL_COPY_COUNTm,
    EGR_DVP_ATTRIBUTEm,
    EGR_GPP_ATTRIBUTESm,
    EGR_IPMCm,
    EGR_L3_INTFm,
    EGR_L3_NEXT_HOPm,
    EGR_PORTm,
    EGR_GPP_ATTRIBUTES_MODBASEm,
    EGR_VLANm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "niv" feature for Trident2 chipset. */
techsupport_reg_t techsupport_niv_trident2_reg_list[] = {
    {CPU_CONTROL_0r, register_type_global},
    {CPU_CONTROL_1r, register_type_global},
    {EGR_NIV_ETHERTYPEr, register_type_global},
    {EGR_NIV_ETHERTYPE_2r, register_type_global},
    {NIV_ETHERTYPEr, register_type_global},
    {PORT_INITIAL_COPY_COUNT_WIDTHr, register_type_global},
    {EGR_NIV_CONFIGr, register_type_global},
    {ING_NIV_CONFIGr, register_type_global},
    {EGR_VLAN_CONTROL_1r, register_type_global},
    {EGR_VLAN_CONTROL_2r, register_type_global},
    {EGR_VLAN_CONTROL_3r, register_type_global},
    {EGR_PORT_1r, register_type_global},
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "niv" feature for Trident2 chipset. */
techsupport_data_t techsupport_niv_trident2_data = {
    techsupport_niv_diag_cmdlist,
    techsupport_niv_trident2_reg_list,
    techsupport_niv_trident2_memory_table_list,
    techsupport_niv_trident2_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_niv_sw_dump_cmdlist,
    techsupport_niv_trident2_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};


