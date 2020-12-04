/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: tomahawk3_efp.c
 *
 * Purpose:
 *    Maintains all the debug information for efp feature for tomahawk3
 *
 * Note: This file is auto-generated. Manual edits
 *       to it will be lost when it is auto-regenerated.
 */

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_efp_diag_cmdlist[];
extern char * techsupport_efp_sw_dump_cmdlist[];

/* "efp" feature's diag command list valid only for tomahawk3 */
char * techsupport_efp_tomahawk3_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "efp" feature's software dump command list valid only for tomahawk3 */
char * techsupport_efp_tomahawk3_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "efp" feature for tomahawk3 chipset. */
static soc_mem_t techsupport_efp_tomahawk3_memory_table_list[] = {
    EFP_COUNTER_TABLE_PIPE0m,
    EFP_COUNTER_TABLE_PIPE1m,
    EFP_COUNTER_TABLE_PIPE2m,
    EFP_COUNTER_TABLE_PIPE3m,
    EFP_COUNTER_TABLE_PIPE4m,
    EFP_COUNTER_TABLE_PIPE5m,
    EFP_COUNTER_TABLE_PIPE6m,
    EFP_COUNTER_TABLE_PIPE7m,
    EFP_POLICY_TABLE_PIPE0m,
    EFP_POLICY_TABLE_PIPE1m,
    EFP_POLICY_TABLE_PIPE2m,
    EFP_POLICY_TABLE_PIPE3m,
    EFP_POLICY_TABLE_PIPE4m,
    EFP_POLICY_TABLE_PIPE5m,
    EFP_POLICY_TABLE_PIPE6m,
    EFP_POLICY_TABLE_PIPE7m,
    EFP_TCAM_PIPE0m,
    EFP_TCAM_PIPE1m,
    EFP_TCAM_PIPE2m,
    EFP_TCAM_PIPE3m,
    EFP_TCAM_PIPE4m,
    EFP_TCAM_PIPE5m,
    EFP_TCAM_PIPE6m,
    EFP_TCAM_PIPE7m,
    PORT_TABm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "efp" feature for tomahawk3 chipset. */
techsupport_reg_t techsupport_efp_tomahawk3_reg_list[] = {
    { EFP_CLASSID_SELECTOR_PIPE0r, register_type_global },
    { EFP_CLASSID_SELECTOR_PIPE1r, register_type_global },
    { EFP_CLASSID_SELECTOR_PIPE2r, register_type_global },
    { EFP_CLASSID_SELECTOR_PIPE3r, register_type_global },
    { EFP_CLASSID_SELECTOR_PIPE4r, register_type_global },
    { EFP_CLASSID_SELECTOR_PIPE5r, register_type_global },
    { EFP_CLASSID_SELECTOR_PIPE6r, register_type_global },
    { EFP_CLASSID_SELECTOR_PIPE7r, register_type_global },
    { EFP_KEY4_L3_CLASSID_SELECTOR_PIPE0r, register_type_global },
    { EFP_KEY4_L3_CLASSID_SELECTOR_PIPE1r, register_type_global },
    { EFP_KEY4_L3_CLASSID_SELECTOR_PIPE2r, register_type_global },
    { EFP_KEY4_L3_CLASSID_SELECTOR_PIPE3r, register_type_global },
    { EFP_KEY4_L3_CLASSID_SELECTOR_PIPE4r, register_type_global },
    { EFP_KEY4_L3_CLASSID_SELECTOR_PIPE5r, register_type_global },
    { EFP_KEY4_L3_CLASSID_SELECTOR_PIPE6r, register_type_global },
    { EFP_KEY4_L3_CLASSID_SELECTOR_PIPE7r, register_type_global },
    { EFP_KEY8_L3_CLASSID_SELECTOR_PIPE0r, register_type_global },
    { EFP_KEY8_L3_CLASSID_SELECTOR_PIPE1r, register_type_global },
    { EFP_KEY8_L3_CLASSID_SELECTOR_PIPE2r, register_type_global },
    { EFP_KEY8_L3_CLASSID_SELECTOR_PIPE3r, register_type_global },
    { EFP_KEY8_L3_CLASSID_SELECTOR_PIPE4r, register_type_global },
    { EFP_KEY8_L3_CLASSID_SELECTOR_PIPE5r, register_type_global },
    { EFP_KEY8_L3_CLASSID_SELECTOR_PIPE6r, register_type_global },
    { EFP_KEY8_L3_CLASSID_SELECTOR_PIPE7r, register_type_global },
    { EFP_SLICE_CONTROL_PIPE0r, register_type_global },
    { EFP_SLICE_CONTROL_PIPE1r, register_type_global },
    { EFP_SLICE_CONTROL_PIPE2r, register_type_global },
    { EFP_SLICE_CONTROL_PIPE3r, register_type_global },
    { EFP_SLICE_CONTROL_PIPE4r, register_type_global },
    { EFP_SLICE_CONTROL_PIPE5r, register_type_global },
    { EFP_SLICE_CONTROL_PIPE6r, register_type_global },
    { EFP_SLICE_CONTROL_PIPE7r, register_type_global },
    { EFP_SLICE_MAP_PIPE0r, register_type_global },
    { EFP_SLICE_MAP_PIPE1r, register_type_global },
    { EFP_SLICE_MAP_PIPE2r, register_type_global },
    { EFP_SLICE_MAP_PIPE3r, register_type_global },
    { EFP_SLICE_MAP_PIPE4r, register_type_global },
    { EFP_SLICE_MAP_PIPE5r, register_type_global },
    { EFP_SLICE_MAP_PIPE6r, register_type_global },
    { EFP_SLICE_MAP_PIPE7r, register_type_global },
    { EGR_CONFIGr, register_type_global },
    { EGR_CONFIG_1r, register_type_global },
    { EGR_EFP_CNTR_UPDATE_CONTROLr, register_type_global },
    { EGR_EFP_EVICTION_CONTROL_PIPE0r, register_type_global },
    { EGR_EFP_EVICTION_CONTROL_PIPE1r, register_type_global },
    { EGR_EFP_EVICTION_CONTROL_PIPE2r, register_type_global },
    { EGR_EFP_EVICTION_CONTROL_PIPE3r, register_type_global },
    { EGR_EFP_EVICTION_CONTROL_PIPE4r, register_type_global },
    { EGR_EFP_EVICTION_CONTROL_PIPE5r, register_type_global },
    { EGR_EFP_EVICTION_CONTROL_PIPE6r, register_type_global },
    { EGR_EFP_EVICTION_CONTROL_PIPE7r, register_type_global },
    { EGR_EFP_EVICTION_SEED_PIPE0r, register_type_global },
    { EGR_EFP_EVICTION_SEED_PIPE1r, register_type_global },
    { EGR_EFP_EVICTION_SEED_PIPE2r, register_type_global },
    { EGR_EFP_EVICTION_SEED_PIPE3r, register_type_global },
    { EGR_EFP_EVICTION_SEED_PIPE4r, register_type_global },
    { EGR_EFP_EVICTION_SEED_PIPE5r, register_type_global },
    { EGR_EFP_EVICTION_SEED_PIPE6r, register_type_global },
    { EGR_EFP_EVICTION_SEED_PIPE7r, register_type_global },
    { EGR_EFP_EVICTION_THRESHOLDr, register_type_global },
    { INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains diag cmdlist, reg_list, mem_list  for
 * "efp" feature for tomahawk3 chipset. */
techsupport_data_t techsupport_efp_tomahawk3_data = {
    techsupport_efp_diag_cmdlist,
    techsupport_efp_tomahawk3_reg_list,
    techsupport_efp_tomahawk3_memory_table_list,
    techsupport_efp_tomahawk3_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_efp_sw_dump_cmdlist,
    techsupport_efp_tomahawk3_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};
