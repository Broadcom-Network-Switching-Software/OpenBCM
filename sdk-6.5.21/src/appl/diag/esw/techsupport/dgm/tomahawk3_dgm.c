/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: tomahawk3_dgm.c
 *
 * Purpose:
 *    Maintains all the debug information for dgm feature for tomahawk3
 *
 * Note: This file is auto-generated. Manual edits
 *       to it will be lost when it is auto-regenerated.
 */

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_dgm_diag_cmdlist[];
extern char * techsupport_dgm_sw_dump_cmdlist[];

/* "dgm" feature's diag command list valid only for tomahawk3 */
char * techsupport_dgm_tomahawk3_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "dgm" feature's software dump command list valid only for tomahawk3 */
char * techsupport_dgm_tomahawk3_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "dgm" feature for tomahawk3 chipset. */
static soc_mem_t techsupport_dgm_tomahawk3_memory_table_list[] = {
    DLB_ECMP_EEM_CONFIGURATIONm,
    DLB_ECMP_ETHERTYPE_ELIGIBILITY_MAPm,
    DLB_ECMP_FINAL_MEMBERS_QUALITY_MEASUREm,
    DLB_ECMP_FLOWSET_MEMBER_PIPE0m,
    DLB_ECMP_FLOWSET_MEMBER_PIPE1m,
    DLB_ECMP_FLOWSET_MEMBER_PIPE2m,
    DLB_ECMP_FLOWSET_MEMBER_PIPE3m,
    DLB_ECMP_FLOWSET_MEMBER_PIPE4m,
    DLB_ECMP_FLOWSET_MEMBER_PIPE5m,
    DLB_ECMP_FLOWSET_MEMBER_PIPE6m,
    DLB_ECMP_FLOWSET_MEMBER_PIPE7m,
    DLB_ECMP_FLOWSET_PIPE0m,
    DLB_ECMP_FLOWSET_PIPE1m,
    DLB_ECMP_FLOWSET_PIPE2m,
    DLB_ECMP_FLOWSET_PIPE3m,
    DLB_ECMP_FLOWSET_PIPE4m,
    DLB_ECMP_FLOWSET_PIPE5m,
    DLB_ECMP_FLOWSET_PIPE6m,
    DLB_ECMP_FLOWSET_PIPE7m,
    DLB_ECMP_FLOWSET_TIMESTAMP_PAGE_PIPE0m,
    DLB_ECMP_FLOWSET_TIMESTAMP_PAGE_PIPE1m,
    DLB_ECMP_FLOWSET_TIMESTAMP_PAGE_PIPE2m,
    DLB_ECMP_FLOWSET_TIMESTAMP_PAGE_PIPE3m,
    DLB_ECMP_FLOWSET_TIMESTAMP_PAGE_PIPE4m,
    DLB_ECMP_FLOWSET_TIMESTAMP_PAGE_PIPE5m,
    DLB_ECMP_FLOWSET_TIMESTAMP_PAGE_PIPE6m,
    DLB_ECMP_FLOWSET_TIMESTAMP_PAGE_PIPE7m,
    DLB_ECMP_GLB_QUANTIZE_THRESHOLDm,
    DLB_ECMP_GROUP_ALT_NHI_MEMBERS_0_TO_12m,
    DLB_ECMP_GROUP_ALT_NHI_MEMBERS_13_TO_25m,
    DLB_ECMP_GROUP_ALT_NHI_MEMBERS_26_TO_38m,
    DLB_ECMP_GROUP_ALT_NHI_MEMBERS_39_TO_51m,
    DLB_ECMP_GROUP_ALT_NHI_MEMBERS_52_TO_63m,
    DLB_ECMP_GROUP_ALT_PORT_MEMBERS_0_TO_15m,
    DLB_ECMP_GROUP_ALT_PORT_MEMBERS_16_TO_31m,
    DLB_ECMP_GROUP_ALT_PORT_MEMBERS_32_TO_47m,
    DLB_ECMP_GROUP_ALT_PORT_MEMBERS_48_TO_63m,
    DLB_ECMP_GROUP_CONTROLm,
    DLB_ECMP_GROUP_MEMBERSHIPm,
    DLB_ECMP_GROUP_MONITOR_CONTROLm,
    DLB_ECMP_GROUP_NHI_MEMBERS_0_TO_12m,
    DLB_ECMP_GROUP_NHI_MEMBERS_13_TO_25m,
    DLB_ECMP_GROUP_NHI_MEMBERS_26_TO_38m,
    DLB_ECMP_GROUP_NHI_MEMBERS_39_TO_51m,
    DLB_ECMP_GROUP_NHI_MEMBERS_52_TO_63m,
    DLB_ECMP_GROUP_PORT_MEMBERS_0_TO_15m,
    DLB_ECMP_GROUP_PORT_MEMBERS_16_TO_31m,
    DLB_ECMP_GROUP_PORT_MEMBERS_32_TO_47m,
    DLB_ECMP_GROUP_PORT_MEMBERS_48_TO_63m,
    DLB_ECMP_GROUP_STATS_PIPE0m,
    DLB_ECMP_GROUP_STATS_PIPE1m,
    DLB_ECMP_GROUP_STATS_PIPE2m,
    DLB_ECMP_GROUP_STATS_PIPE3m,
    DLB_ECMP_GROUP_STATS_PIPE4m,
    DLB_ECMP_GROUP_STATS_PIPE5m,
    DLB_ECMP_GROUP_STATS_PIPE6m,
    DLB_ECMP_GROUP_STATS_PIPE7m,
    DLB_ECMP_LINK_CONTROLm,
    DLB_ECMP_OPTIMAL_CANDIDATE_PIPE0m,
    DLB_ECMP_OPTIMAL_CANDIDATE_PIPE1m,
    DLB_ECMP_OPTIMAL_CANDIDATE_PIPE2m,
    DLB_ECMP_OPTIMAL_CANDIDATE_PIPE3m,
    DLB_ECMP_OPTIMAL_CANDIDATE_PIPE4m,
    DLB_ECMP_OPTIMAL_CANDIDATE_PIPE5m,
    DLB_ECMP_OPTIMAL_CANDIDATE_PIPE6m,
    DLB_ECMP_OPTIMAL_CANDIDATE_PIPE7m,
    DLB_ECMP_PORT_AVG_QUALITY_MEASUREm,
    DLB_ECMP_PORT_INST_QUALITY_MEASUREm,
    DLB_ECMP_PORT_QUALITY_MAPPINGm,
    DLB_ECMP_PORT_QUALITY_UPDATE_MEASURE_CONTROLm,
    DLB_ECMP_PORT_STATEm,
    DLB_ECMP_QUANTIZED_AVG_QUALITY_MEASUREm,
    DLB_ECMP_QUANTIZE_CONTROLm,
    DLB_ECMP_SER_FIFOm,
    ECMP_GROUP_HIERARCHICALm,
    L3_ECMP_COUNTm,
    L3_ECMPm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "dgm" feature for tomahawk3 chipset. */
techsupport_reg_t techsupport_dgm_tomahawk3_reg_list[] = {
    { DLB_ECMP_CURRENT_TIME_PIPE0r, register_type_global },
    { DLB_ECMP_CURRENT_TIME_PIPE1r, register_type_global },
    { DLB_ECMP_CURRENT_TIME_PIPE2r, register_type_global },
    { DLB_ECMP_CURRENT_TIME_PIPE3r, register_type_global },
    { DLB_ECMP_CURRENT_TIME_PIPE4r, register_type_global },
    { DLB_ECMP_CURRENT_TIME_PIPE5r, register_type_global },
    { DLB_ECMP_CURRENT_TIME_PIPE6r, register_type_global },
    { DLB_ECMP_CURRENT_TIME_PIPE7r, register_type_global },
    { DLB_ECMP_HW_RESET_CONTROLr, register_type_global },
    { DLB_ECMP_INTR_ENABLEr, register_type_global },
    { DLB_ECMP_INTR_STATUSr, register_type_global },
    { DLB_ECMP_MONITOR_CONTROL_PIPE0r, register_type_global },
    { DLB_ECMP_MONITOR_CONTROL_PIPE1r, register_type_global },
    { DLB_ECMP_MONITOR_CONTROL_PIPE2r, register_type_global },
    { DLB_ECMP_MONITOR_CONTROL_PIPE3r, register_type_global },
    { DLB_ECMP_MONITOR_CONTROL_PIPE4r, register_type_global },
    { DLB_ECMP_MONITOR_CONTROL_PIPE5r, register_type_global },
    { DLB_ECMP_MONITOR_CONTROL_PIPE6r, register_type_global },
    { DLB_ECMP_MONITOR_CONTROL_PIPE7r, register_type_global },
    { DLB_ECMP_MONITOR_IFP_CONTROL_PIPE0r, register_type_global },
    { DLB_ECMP_MONITOR_IFP_CONTROL_PIPE1r, register_type_global },
    { DLB_ECMP_MONITOR_IFP_CONTROL_PIPE2r, register_type_global },
    { DLB_ECMP_MONITOR_IFP_CONTROL_PIPE3r, register_type_global },
    { DLB_ECMP_MONITOR_IFP_CONTROL_PIPE4r, register_type_global },
    { DLB_ECMP_MONITOR_IFP_CONTROL_PIPE5r, register_type_global },
    { DLB_ECMP_MONITOR_IFP_CONTROL_PIPE6r, register_type_global },
    { DLB_ECMP_MONITOR_IFP_CONTROL_PIPE7r, register_type_global },
    { DLB_ECMP_MONITOR_MIRROR_CONFIGr, register_type_global },
    { DLB_ECMP_QUALITY_MEASURE_CONTROLr, register_type_global },
    { DLB_ECMP_RAM_CONTROL_0_PIPE0r, register_type_global },
    { DLB_ECMP_RAM_CONTROL_0_PIPE1r, register_type_global },
    { DLB_ECMP_RAM_CONTROL_0_PIPE2r, register_type_global },
    { DLB_ECMP_RAM_CONTROL_0_PIPE3r, register_type_global },
    { DLB_ECMP_RAM_CONTROL_0_PIPE4r, register_type_global },
    { DLB_ECMP_RAM_CONTROL_0_PIPE5r, register_type_global },
    { DLB_ECMP_RAM_CONTROL_0_PIPE6r, register_type_global },
    { DLB_ECMP_RAM_CONTROL_0_PIPE7r, register_type_global },
    { DLB_ECMP_RAM_CONTROL_1r, register_type_global },
    { DLB_ECMP_RANDOM_SELECTION_CONTROL_PIPE0r, register_type_global },
    { DLB_ECMP_RANDOM_SELECTION_CONTROL_PIPE1r, register_type_global },
    { DLB_ECMP_RANDOM_SELECTION_CONTROL_PIPE2r, register_type_global },
    { DLB_ECMP_RANDOM_SELECTION_CONTROL_PIPE3r, register_type_global },
    { DLB_ECMP_RANDOM_SELECTION_CONTROL_PIPE4r, register_type_global },
    { DLB_ECMP_RANDOM_SELECTION_CONTROL_PIPE5r, register_type_global },
    { DLB_ECMP_RANDOM_SELECTION_CONTROL_PIPE6r, register_type_global },
    { DLB_ECMP_RANDOM_SELECTION_CONTROL_PIPE7r, register_type_global },
    { DLB_ECMP_REFRESH_DISABLEr, register_type_global },
    { DLB_ECMP_REFRESH_INDEX_PIPE0r, register_type_global },
    { DLB_ECMP_REFRESH_INDEX_PIPE1r, register_type_global },
    { DLB_ECMP_REFRESH_INDEX_PIPE2r, register_type_global },
    { DLB_ECMP_REFRESH_INDEX_PIPE3r, register_type_global },
    { DLB_ECMP_REFRESH_INDEX_PIPE4r, register_type_global },
    { DLB_ECMP_REFRESH_INDEX_PIPE5r, register_type_global },
    { DLB_ECMP_REFRESH_INDEX_PIPE6r, register_type_global },
    { DLB_ECMP_REFRESH_INDEX_PIPE7r, register_type_global },
    { DLB_ECMP_SER_CONTROL_2r, register_type_global },
    { DLB_ECMP_SER_CONTROLr, register_type_global },
    { DLB_ECMP_SER_FIFO_CTRLr, register_type_global },
    { DLB_ECMP_SER_FIFO_STATUSr, register_type_global },
    { DLB_ID_0_TO_63_ENABLEr, register_type_global },
    { DLB_ID_64_TO_127_ENABLEr, register_type_global },
    { DLB_IETR_CLK_CTRL_PIPE0r, register_type_global },
    { DLB_IETR_CLK_CTRL_PIPE1r, register_type_global },
    { DLB_IETR_CLK_CTRL_PIPE2r, register_type_global },
    { DLB_IETR_CLK_CTRL_PIPE3r, register_type_global },
    { DLB_IETR_CLK_CTRL_PIPE4r, register_type_global },
    { DLB_IETR_CLK_CTRL_PIPE5r, register_type_global },
    { DLB_IETR_CLK_CTRL_PIPE6r, register_type_global },
    { DLB_IETR_CLK_CTRL_PIPE7r, register_type_global },
    { ECMP_GROUP_DLB_ID_OFFSETr, register_type_global },
    { INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains diag cmdlist, reg_list, mem_list  for
 * "dgm" feature for tomahawk3 chipset. */
techsupport_data_t techsupport_dgm_tomahawk3_data = {
    techsupport_dgm_diag_cmdlist,
    techsupport_dgm_tomahawk3_reg_list,
    techsupport_dgm_tomahawk3_memory_table_list,
    techsupport_dgm_tomahawk3_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_dgm_sw_dump_cmdlist,
    techsupport_dgm_tomahawk3_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};
