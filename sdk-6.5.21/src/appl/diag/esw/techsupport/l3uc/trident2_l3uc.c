/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    trident2_l3uc.c
 * Purpose: Maintains all the debug information for l3uc(layer 3 unicast)
 *          feature for trident2.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_l3uc_diag_cmdlist[];
extern char * techsupport_l3uc_sw_dump_cmdlist[];

/* "l3uc" feature's diag command list valid only for Trident2 */
char * techsupport_l3uc_trident2_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "l3uc" feature's software dump command list valid only for Trident2 */
char * techsupport_l3uc_trident2_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "l3uc" feature for Trident2 chipset. */
static soc_mem_t techsupport_l3uc_trident2_memory_table_list[] = {
    PORT_TABm,
    L3_TUNNELm,
    MY_STATION_TCAMm,
    VLAN_MPLSm,
    L3_IIFm,
    L3_IIF_PROFILEm,
    VRFm,
    VLAN_TABm,
    L3_ENTRY_IPV6_UNICASTm,
    L3_ENTRY_ONLYm,
    ING_ACTIVE_L3_IIF_PROFILEm,
    L3_ENTRY_IPV4_UNICASTm,
    L3_DEFIPm,
    L3_DEFIP_PAIR_128m,
    L3_DEFIP_ALPM_IPV4_1m,
    L3_DEFIP_ALPM_IPV6_64m,
    L3_DEFIP_ALPM_IPV6_128m,
    L3_DEFIP_ALPM_IPV6_64_1m,
    L3_DEFIP_ONLYm,
    L3_DEFIP_DATA_ONLYm,
    L3_DEFIP_ALPM_IPV4m,
    INITIAL_ING_L3_NEXT_HOPm,
    INITIAL_L3_ECMPm,
    INITIAL_L3_ECMP_GROUPm,
    INITIAL_PROT_NHI_TABLEm,
    L3_DEFIP_HIT_ONLY_Xm,
    L3_DEFIP_HIT_ONLY_Ym,
    L3_ENTRY_HIT_ONLY_Xm,
    L3_ENTRY_HIT_ONLY_Ym,
    L3_DEFIP_ALPM_HIT_ONLY_Xm,
    L3_DEFIP_ALPM_HIT_ONLY_Ym,
    L3_DEFIP_PAIR_128_HIT_ONLY_Xm,
    L3_DEFIP_PAIR_128_HIT_ONLY_Ym,
    ING_L3_NEXT_HOPm,
    L3_ECMPm,
    L3_ECMP_COUNTm,
    L3_MTU_VALUESm,
    EGR_L3_INTFm,
    EGR_L3_NEXT_HOPm,
    EGR_IP_TUNNELm,
    EGR_VLANm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "l3uc" feature for Trident2 chipset. */
techsupport_reg_t techsupport_l3uc_trident2_reg_list[] = {
    {ING_CONFIG_64r, register_type_global},
    {L3_DEFIP_RPF_CONTROLr, register_type_global},
    {L2_TABLE_HASH_CONTROLr, register_type_global},
    {L3_TABLE_HASH_CONTROLr, register_type_global},
    {SHARED_TABLE_HASH_CONTROLr, register_type_global},
    {ISS_MEMORY_CONTROL_84r, register_type_global},
    {L3_DEFIP_ALPM_CFGr, register_type_global},
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "l3uc" feature for Trident2 chipset. */
techsupport_data_t techsupport_l3uc_trident2_data = {
    techsupport_l3uc_diag_cmdlist,
    techsupport_l3uc_trident2_reg_list,
    techsupport_l3uc_trident2_memory_table_list,
    techsupport_l3uc_trident2_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_l3uc_sw_dump_cmdlist,
    techsupport_l3uc_trident2_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};

