/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: tomahawk3_mpls.c
 *
 * Purpose:
 *    Maintains all the debug information for mpls feature for tomahawk3
 *
 * Note: This file is auto-generated. Manual edits
 *       to it will be lost when it is auto-regenerated.
 */

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_mpls_diag_cmdlist[];
extern char * techsupport_mpls_sw_dump_cmdlist[];

/* "mpls" feature's diag command list valid only for tomahawk3 */
char * techsupport_mpls_tomahawk3_diag_cmdlist[] = {
    "l2learn",
    "dump sa",
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "mpls" feature's software dump command list valid only for tomahawk3 */
char * techsupport_mpls_tomahawk3_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "mpls" feature for tomahawk3 chipset. */
static soc_mem_t techsupport_mpls_tomahawk3_memory_table_list[] = {
    EGR_IP_TUNNEL_MPLSm,
    EGR_MPLS_EXP_MAPPING_1m,
    EGR_MPLS_EXP_MAPPING_2m,
    EGR_MPLS_EXP_PRI_MAPPINGm,
    EGR_MPLS_PRI_MAPPINGm,
    ING_MPLS_EXP_MAPPINGm,
    L2MCm,
    L2Xm,
    L2_ENTRY_HASH_CONTROLm,
    L2_ENTRY_KEY_ATTRIBUTESm,
    L2_ENTRY_ONLY_SINGLEm,
    L2_USER_ENTRYm,
    L3_DEFIP_ALPM_LEVEL2_ECCm,
    L3_DEFIP_ALPM_LEVEL2_HIT_ONLYm,
    L3_DEFIP_ALPM_LEVEL2_SINGLEm,
    L3_DEFIP_ALPM_LEVEL2m,
    L3_DEFIP_ALPM_LEVEL3_ECCm,
    L3_DEFIP_ALPM_LEVEL3_HIT_ONLYm,
    L3_DEFIP_ALPM_LEVEL3_SINGLEm,
    L3_DEFIP_ALPM_LEVEL3m,
    L3_DEFIP_DATA_LEVEL1m,
    L3_DEFIP_LEVEL1_HIT_ONLYm,
    L3_DEFIP_LEVEL1m,
    L3_DEFIP_PAIR_LEVEL1m,
    L3_DEFIP_TCAM_LEVEL1m,
    MPLS_ENTRY_ACTION_TABLE_Am,
    MPLS_ENTRY_ACTION_TABLE_Bm,
    MPLS_ENTRY_ECCm,
    MPLS_ENTRY_HASH_CONTROLm,
    MPLS_ENTRY_KEY_ATTRIBUTESm,
    MPLS_ENTRY_KEY_BUFFER_0_PIPE0m,
    MPLS_ENTRY_KEY_BUFFER_0_PIPE1m,
    MPLS_ENTRY_KEY_BUFFER_0_PIPE2m,
    MPLS_ENTRY_KEY_BUFFER_0_PIPE3m,
    MPLS_ENTRY_KEY_BUFFER_0_PIPE4m,
    MPLS_ENTRY_KEY_BUFFER_0_PIPE5m,
    MPLS_ENTRY_KEY_BUFFER_0_PIPE6m,
    MPLS_ENTRY_KEY_BUFFER_0_PIPE7m,
    MPLS_ENTRY_KEY_BUFFER_1_PIPE0m,
    MPLS_ENTRY_KEY_BUFFER_1_PIPE1m,
    MPLS_ENTRY_KEY_BUFFER_1_PIPE2m,
    MPLS_ENTRY_KEY_BUFFER_1_PIPE3m,
    MPLS_ENTRY_KEY_BUFFER_1_PIPE4m,
    MPLS_ENTRY_KEY_BUFFER_1_PIPE5m,
    MPLS_ENTRY_KEY_BUFFER_1_PIPE6m,
    MPLS_ENTRY_KEY_BUFFER_1_PIPE7m,
    MPLS_ENTRY_REMAP_TABLE_Am,
    MPLS_ENTRY_REMAP_TABLE_Bm,
    MPLS_ENTRY_SINGLEm,
    MY_STATION_TCAM_DATA_ONLYm,
    MY_STATION_TCAM_ENTRY_ONLYm,
    MY_STATION_TCAMm,
    VLAN_MPLSm,
    VRFm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "mpls" feature for tomahawk3 chipset. */
techsupport_reg_t techsupport_mpls_tomahawk3_reg_list[] = {
    { BFD_RX_ACH_TYPE_MPLSTP1_32r, register_type_global },
    { BFD_RX_ACH_TYPE_MPLSTPr, register_type_global },
    { GLOBAL_MPLS_RANGE_1_LOWERr, register_type_global },
    { GLOBAL_MPLS_RANGE_1_UPPERr, register_type_global },
    { GLOBAL_MPLS_RANGE_2_LOWERr, register_type_global },
    { GLOBAL_MPLS_RANGE_2_UPPERr, register_type_global },
    { ING_MISC_CONFIG2r, register_type_global },
    { ING_MISC_CONFIGr, register_type_global },
    { ING_OUTER_TPID_0r, register_type_global },
    { ING_OUTER_TPID_1r, register_type_global },
    { ING_OUTER_TPID_2r, register_type_global },
    { ING_OUTER_TPID_3r, register_type_global },
    { L2_LEARN_CACHE_STATUS_PIPE0r, register_type_global },
    { L2_LEARN_CACHE_STATUS_PIPE1r, register_type_global },
    { L2_LEARN_CACHE_STATUS_PIPE2r, register_type_global },
    { L2_LEARN_CACHE_STATUS_PIPE3r, register_type_global },
    { L2_LEARN_CACHE_STATUS_PIPE4r, register_type_global },
    { L2_LEARN_CACHE_STATUS_PIPE5r, register_type_global },
    { L2_LEARN_CACHE_STATUS_PIPE6r, register_type_global },
    { L2_LEARN_CACHE_STATUS_PIPE7r, register_type_global },
    { L2_LEARN_COPY_CACHE_CTRLr, register_type_global },
    { RTAG7_MPLS_L3_PAYLOAD_HASH_FIELD_BMAPr, register_type_global },
    { RTAG7_MPLS_OUTER_HASH_FIELD_BMAP_EXTENDEDr, register_type_global },
    { RTAG7_MPLS_OUTER_HASH_FIELD_BMAPr, register_type_global },
    { INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains diag cmdlist, reg_list, mem_list  for
 * "mpls" feature for tomahawk3 chipset. */
techsupport_data_t techsupport_mpls_tomahawk3_data = {
    techsupport_mpls_diag_cmdlist,
    techsupport_mpls_tomahawk3_reg_list,
    techsupport_mpls_tomahawk3_memory_table_list,
    techsupport_mpls_tomahawk3_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_mpls_sw_dump_cmdlist,
    techsupport_mpls_tomahawk3_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};
