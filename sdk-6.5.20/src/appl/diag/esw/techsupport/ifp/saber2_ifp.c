/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    saber2_ifp.c
 * Purpose: Maintains all the debug information for ifp
 *          feature for saber2.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_ifp_diag_cmdlist[];
extern char * techsupport_ifp_sw_dump_cmdlist[];

/* "ifp" feature's diag command list valid only for Saber2 */
char * techsupport_ifp_saber2_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "ifp" feature's software dump command list valid only for Saber2 */
char * techsupport_ifp_saber2_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "ifp" feature for Saber2 chipset. */
static soc_mem_t techsupport_ifp_saber2_memory_table_list[] = {
    UDF_OFFSETm,
    FP_UDF_TCAMm,
    PORT_TABm,
    LPORT_TABm,
    FP_PORT_FIELD_SELm,
    TCP_FNm,
    TOS_FNm,
    TTL_FNm,
    FP_GLOBAL_MASK_TCAMm,
    FP_HG_CLASSID_SELECTm,
    FP_I2E_CLASSID_SELECTm,
    FP_POLICY_TABLEm,
    FP_COUNTER_TABLEm,
    FP_RANGE_CHECKm,
    FP_SLICE_KEY_CONTROLm,
    FP_TCAMm,
    IFP_REDIRECTION_PROFILEm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "ifp" feature for Saber2 chipset. */
techsupport_reg_t techsupport_ifp_saber2_reg_list[] = {
    {ING_BYPASS_CTRLr, register_type_global},
    {SW2_FP_DST_ACTION_CONTROLr, register_type_global},
    {FP_SLICE_METER_MAP_ENABLEr, register_type_global},
    {FP_SLICE_ENABLEr, register_type_global},
    {FP_SLICE_INDEX_CONTROLr, register_type_global},
    {FP_FORCE_FORWARDING_FIELDr, register_type_global},
    {FP_METER_CONTROLr, register_type_global},
    {ING_CONFIG_64r, register_type_global},
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "ifp" feature for Saber2 chipset. */
techsupport_data_t techsupport_ifp_saber2_data = {
    techsupport_ifp_diag_cmdlist,
    techsupport_ifp_saber2_reg_list,
    techsupport_ifp_saber2_memory_table_list,
    techsupport_ifp_saber2_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_ifp_sw_dump_cmdlist,
    techsupport_ifp_saber2_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};

