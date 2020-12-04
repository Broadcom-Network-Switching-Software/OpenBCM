/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    tomahawk_efp.c
 * Purpose: Maintains all the debug information for efp
 *          feature for tomahawk.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_efp_diag_cmdlist[];
extern char * techsupport_efp_sw_dump_cmdlist[];

/* "efp" feature's diag command list valid only for tomahawk */
char * techsupport_efp_tomahawk_diag_cmdlist[] = {
     NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "efp" feature's software dump command list valid only for tomahawk */
char * techsupport_efp_tomahawk_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "efp" feature for tomahawk chipset. */
static soc_mem_t techsupport_efp_tomahawk_memory_table_list[] = {
    PORT_TABm,
    EFP_METER_TABLE_PIPE0m,
    EFP_METER_TABLE_PIPE1m,
    EFP_METER_TABLE_PIPE2m,
    EFP_METER_TABLE_PIPE3m,
    EFP_POLICY_TABLE_PIPE0m,
    EFP_POLICY_TABLE_PIPE1m,
    EFP_POLICY_TABLE_PIPE2m,
    EFP_POLICY_TABLE_PIPE3m,
    EFP_TCAM_PIPE0m,
    EFP_TCAM_PIPE1m,
    EFP_TCAM_PIPE2m,
    EFP_TCAM_PIPE3m,
    EFP_COUNTER_TABLE_PIPE0m,
    EFP_COUNTER_TABLE_PIPE1m,
    EFP_COUNTER_TABLE_PIPE2m,
    EFP_COUNTER_TABLE_PIPE3m,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "efp" feature for tomahawk chipset. */
techsupport_reg_t techsupport_efp_tomahawk_reg_list[] = {
   {EFP_CLASSID_SELECTOR_PIPE0r, register_type_global },
   {EFP_CLASSID_SELECTOR_PIPE1r, register_type_global },
   {EFP_CLASSID_SELECTOR_PIPE2r, register_type_global },
   {EFP_CLASSID_SELECTOR_PIPE3r, register_type_global },
   {EFP_KEY4_DVP_SELECTOR_PIPE0r, register_type_global },
   {EFP_KEY4_DVP_SELECTOR_PIPE1r, register_type_global },
   {EFP_KEY4_DVP_SELECTOR_PIPE2r, register_type_global },
   {EFP_KEY4_DVP_SELECTOR_PIPE3r, register_type_global },
   {EFP_KEY4_MDL_SELECTOR_PIPE0r, register_type_global },
   {EFP_KEY4_MDL_SELECTOR_PIPE1r, register_type_global },
   {EFP_KEY4_MDL_SELECTOR_PIPE2r, register_type_global },
   {EFP_KEY4_MDL_SELECTOR_PIPE3r, register_type_global },
   {EFP_KEY8_DVP_SELECTOR_PIPE0r, register_type_global },
   {EFP_KEY8_DVP_SELECTOR_PIPE1r, register_type_global },
   {EFP_KEY8_DVP_SELECTOR_PIPE2r, register_type_global },
   {EFP_KEY8_DVP_SELECTOR_PIPE3r, register_type_global },
   {EFP_METER_CONTROL_PIPE0r, register_type_global },
   {EFP_METER_CONTROL_PIPE1r, register_type_global },
   {EFP_METER_CONTROL_PIPE2r, register_type_global },
   {EFP_METER_CONTROL_PIPE3r, register_type_global },
   {EFP_SLICE_CONTROL_PIPE0r, register_type_global },
   {EFP_SLICE_CONTROL_PIPE1r, register_type_global },
   {EFP_SLICE_CONTROL_PIPE2r, register_type_global },
   {EFP_SLICE_CONTROL_PIPE3r, register_type_global },
   {EFP_SLICE_MAP_PIPE0r, register_type_global },
   {EFP_SLICE_MAP_PIPE1r, register_type_global },
   {EFP_SLICE_MAP_PIPE2r, register_type_global },
   {EFP_SLICE_MAP_PIPE3r, register_type_global },
   {EGR_EFP_CNTR_UPDATE_CONTROL_PIPE0r, register_type_global },
   {EGR_EFP_CNTR_UPDATE_CONTROL_PIPE1r, register_type_global },
   {EGR_EFP_CNTR_UPDATE_CONTROL_PIPE2r, register_type_global },
   {EGR_EFP_CNTR_UPDATE_CONTROL_PIPE3r, register_type_global },
   {EGR_EFP_EVICTION_CONTROL_PIPE0r, register_type_global },
   {EGR_EFP_EVICTION_CONTROL_PIPE1r, register_type_global },
   {EGR_EFP_EVICTION_CONTROL_PIPE2r, register_type_global },
   {EGR_EFP_EVICTION_CONTROL_PIPE3r, register_type_global },
   {EGR_EFP_EVICTION_SEED_PIPE0r, register_type_global },
   {EGR_EFP_EVICTION_SEED_PIPE1r, register_type_global },
   {EGR_EFP_EVICTION_SEED_PIPE2r, register_type_global },
   {EGR_EFP_EVICTION_SEED_PIPE3r, register_type_global },
   {EGR_EFP_EVICTION_THRESHOLD_PIPE0r, register_type_global },
   {EGR_EFP_EVICTION_THRESHOLD_PIPE1r, register_type_global },
   {EGR_EFP_EVICTION_THRESHOLD_PIPE2r, register_type_global },
   {EGR_EFP_EVICTION_THRESHOLD_PIPE3r, register_type_global },
   {EGR_CONFIGr, register_type_global },
   {EGR_CONFIG_1r, register_type_global },
   {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "efp" feature for tomahawk chipset. */
techsupport_data_t techsupport_efp_tomahawk_data = {
    techsupport_efp_diag_cmdlist,
    techsupport_efp_tomahawk_reg_list,
    techsupport_efp_tomahawk_memory_table_list,
    techsupport_efp_tomahawk_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_efp_sw_dump_cmdlist,
    techsupport_efp_tomahawk_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};

