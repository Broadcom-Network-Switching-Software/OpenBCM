/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: tomahawk3_histogram.c
 *
 * Purpose:
 *    Maintains all the debug information for histogram feature for
 *    tomahawk3
 *
 * Note: This file is auto-generated. Manual edits
 *       to it will be lost when it is auto-regenerated.
 */

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_histogram_diag_cmdlist[];
extern char * techsupport_histogram_sw_dump_cmdlist[];

/* "histogram" feature's diag command list valid only for tomahawk3 */
char * techsupport_histogram_tomahawk3_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "histogram" feature's software dump command list valid only for tomahawk3 */
char * techsupport_histogram_tomahawk3_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "histogram" feature for tomahawk3 chipset. */
static soc_mem_t techsupport_histogram_tomahawk3_memory_table_list[] = {
    EGR_FLEX_CTR_COUNTER_TABLE_0_PIPE0m,
    EGR_FLEX_CTR_COUNTER_TABLE_0_PIPE1m,
    EGR_FLEX_CTR_COUNTER_TABLE_0_PIPE2m,
    EGR_FLEX_CTR_COUNTER_TABLE_0_PIPE3m,
    EGR_FLEX_CTR_COUNTER_TABLE_0_PIPE4m,
    EGR_FLEX_CTR_COUNTER_TABLE_0_PIPE5m,
    EGR_FLEX_CTR_COUNTER_TABLE_0_PIPE6m,
    EGR_FLEX_CTR_COUNTER_TABLE_0_PIPE7m,
    EGR_FLEX_CTR_COUNTER_TABLE_1_PIPE0m,
    EGR_FLEX_CTR_COUNTER_TABLE_1_PIPE1m,
    EGR_FLEX_CTR_COUNTER_TABLE_1_PIPE2m,
    EGR_FLEX_CTR_COUNTER_TABLE_1_PIPE3m,
    EGR_FLEX_CTR_COUNTER_TABLE_1_PIPE4m,
    EGR_FLEX_CTR_COUNTER_TABLE_1_PIPE5m,
    EGR_FLEX_CTR_COUNTER_TABLE_1_PIPE6m,
    EGR_FLEX_CTR_COUNTER_TABLE_1_PIPE7m,
    EGR_FLEX_CTR_COUNTER_TABLE_2_PIPE0m,
    EGR_FLEX_CTR_COUNTER_TABLE_2_PIPE1m,
    EGR_FLEX_CTR_COUNTER_TABLE_2_PIPE2m,
    EGR_FLEX_CTR_COUNTER_TABLE_2_PIPE3m,
    EGR_FLEX_CTR_COUNTER_TABLE_2_PIPE4m,
    EGR_FLEX_CTR_COUNTER_TABLE_2_PIPE5m,
    EGR_FLEX_CTR_COUNTER_TABLE_2_PIPE6m,
    EGR_FLEX_CTR_COUNTER_TABLE_2_PIPE7m,
    EGR_FLEX_CTR_COUNTER_TABLE_3_PIPE0m,
    EGR_FLEX_CTR_COUNTER_TABLE_3_PIPE1m,
    EGR_FLEX_CTR_COUNTER_TABLE_3_PIPE2m,
    EGR_FLEX_CTR_COUNTER_TABLE_3_PIPE3m,
    EGR_FLEX_CTR_COUNTER_TABLE_3_PIPE4m,
    EGR_FLEX_CTR_COUNTER_TABLE_3_PIPE5m,
    EGR_FLEX_CTR_COUNTER_TABLE_3_PIPE6m,
    EGR_FLEX_CTR_COUNTER_TABLE_3_PIPE7m,
    EGR_HISTO_LATENCY_LIMITm,
    EGR_HISTO_MON_0_Q_LATENCY_LIMIT_SELm,
    EGR_HISTO_MON_1_Q_LATENCY_LIMIT_SELm,
    EGR_HISTO_MON_2_Q_LATENCY_LIMIT_SELm,
    EGR_HISTO_MON_3_Q_LATENCY_LIMIT_SELm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "histogram" feature for tomahawk3 chipset. */
techsupport_reg_t techsupport_histogram_tomahawk3_reg_list[] = {
    { AUX_ARB_CONTROLr, register_type_global },
    { EGR_FLEX_CTR_COUNTER_UPDATE_CONTROL_0r, register_type_global },
    { EGR_FLEX_CTR_COUNTER_UPDATE_CONTROL_1r, register_type_global },
    { EGR_FLEX_CTR_COUNTER_UPDATE_CONTROL_2r, register_type_global },
    { EGR_FLEX_CTR_COUNTER_UPDATE_CONTROL_3r, register_type_global },
    { EGR_HISTO_MON_0_CONFIGr, register_type_global },
    { EGR_HISTO_MON_0_FLEX_BANK_SEL_0r, register_type_global },
    { EGR_HISTO_MON_0_FLEX_BANK_SEL_1r, register_type_global },
    { EGR_HISTO_MON_0_FLEX_COUNTER_WRAP_COUNTERr, register_type_global },
    { EGR_HISTO_MON_0_HISTOGRAM_SETr, register_type_global },
    { EGR_HISTO_MON_0_LAST_COUNTER_INDEXr, register_type_global },
    { EGR_HISTO_MON_1_CONFIGr, register_type_global },
    { EGR_HISTO_MON_1_FLEX_BANK_SEL_0r, register_type_global },
    { EGR_HISTO_MON_1_FLEX_BANK_SEL_1r, register_type_global },
    { EGR_HISTO_MON_1_FLEX_COUNTER_WRAP_COUNTERr, register_type_global },
    { EGR_HISTO_MON_1_HISTOGRAM_SETr, register_type_global },
    { EGR_HISTO_MON_1_LAST_COUNTER_INDEXr, register_type_global },
    { EGR_HISTO_MON_2_CONFIGr, register_type_global },
    { EGR_HISTO_MON_2_FLEX_BANK_SEL_0r, register_type_global },
    { EGR_HISTO_MON_2_FLEX_BANK_SEL_1r, register_type_global },
    { EGR_HISTO_MON_2_FLEX_COUNTER_WRAP_COUNTERr, register_type_global },
    { EGR_HISTO_MON_2_HISTOGRAM_SETr, register_type_global },
    { EGR_HISTO_MON_2_LAST_COUNTER_INDEXr, register_type_global },
    { EGR_HISTO_MON_3_CONFIGr, register_type_global },
    { EGR_HISTO_MON_3_FLEX_BANK_SEL_0r, register_type_global },
    { EGR_HISTO_MON_3_FLEX_BANK_SEL_1r, register_type_global },
    { EGR_HISTO_MON_3_FLEX_COUNTER_WRAP_COUNTERr, register_type_global },
    { EGR_HISTO_MON_3_HISTOGRAM_SETr, register_type_global },
    { EGR_HISTO_MON_3_LAST_COUNTER_INDEXr, register_type_global },
    { INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains diag cmdlist, reg_list, mem_list  for
 * "histogram" feature for tomahawk3 chipset. */
techsupport_data_t techsupport_histogram_tomahawk3_data = {
    techsupport_histogram_diag_cmdlist,
    techsupport_histogram_tomahawk3_reg_list,
    techsupport_histogram_tomahawk3_memory_table_list,
    techsupport_histogram_tomahawk3_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_histogram_sw_dump_cmdlist,
    techsupport_histogram_tomahawk3_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};
