/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    tomahawk2_dgm.c
 * Purpose: Maintains all the debug information for dgm
 *          feature for tomahawk2.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_dgm_diag_cmdlist[];
extern char * techsupport_dgm_sw_dump_cmdlist[];

/* "dgm" feature's diag command list valid only for tomahawk2 */
char * techsupport_dgm_tomahawk2_diag_cmdlist[] = {
     NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "dgm" feature's software dump command list valid only for tomahawk2 */
char * techsupport_dgm_tomahawk2_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "dgm" feature for tomahawk2 chipset. */
static soc_mem_t techsupport_dgm_tomahawk2_memory_table_list[] = {
   DLB_ECMP_EEM_CONFIGURATIONm,
   DLB_ECMP_ETHERTYPE_ELIGIBILITY_MAPm,
   DLB_ECMP_FINAL_MEMBERS_QUALITY_MEASURE_PIPE0m,
   DLB_ECMP_FINAL_MEMBERS_QUALITY_MEASURE_PIPE1m,
   DLB_ECMP_FINAL_MEMBERS_QUALITY_MEASURE_PIPE2m,
   DLB_ECMP_FINAL_MEMBERS_QUALITY_MEASURE_PIPE3m,
   DLB_ECMP_FLOWSET_PIPE0m,
   DLB_ECMP_FLOWSET_PIPE1m,
   DLB_ECMP_FLOWSET_PIPE2m,
   DLB_ECMP_FLOWSET_PIPE3m,
   DLB_ECMP_FLOWSET_MEMBER_PIPE0m,
   DLB_ECMP_FLOWSET_MEMBER_PIPE1m,
   DLB_ECMP_FLOWSET_MEMBER_PIPE2m,
   DLB_ECMP_FLOWSET_MEMBER_PIPE3m,
   DLB_ECMP_FLOWSET_TIMESTAMP_PAGE_PIPE0m,
   DLB_ECMP_FLOWSET_TIMESTAMP_PAGE_PIPE1m,
   DLB_ECMP_FLOWSET_TIMESTAMP_PAGE_PIPE2m,
   DLB_ECMP_FLOWSET_TIMESTAMP_PAGE_PIPE3m,
   DLB_ECMP_GLB_QUANTIZE_THRESHOLDm,
   DLB_ECMP_GROUP_CONTROLm,
   DLB_ECMP_GROUP_MEMBERSHIPm,
   DLB_ECMP_GROUP_PORT_TO_MEMBERm,
   DLB_ECMP_GROUP_STATS_PIPE0m,
   DLB_ECMP_GROUP_STATS_PIPE1m,
   DLB_ECMP_GROUP_STATS_PIPE2m,
   DLB_ECMP_GROUP_STATS_PIPE3m,
   DLB_ECMP_LINK_CONTROLm,
   DLB_ECMP_OPTIMAL_CANDIDATE_PIPE0m,
   DLB_ECMP_OPTIMAL_CANDIDATE_PIPE1m,
   DLB_ECMP_OPTIMAL_CANDIDATE_PIPE2m,
   DLB_ECMP_OPTIMAL_CANDIDATE_PIPE3m,
   DLB_ECMP_PORT_AVG_QUALITY_MEASURE_PIPE0m,
   DLB_ECMP_PORT_AVG_QUALITY_MEASURE_PIPE1m,
   DLB_ECMP_PORT_AVG_QUALITY_MEASURE_PIPE2m,
   DLB_ECMP_PORT_AVG_QUALITY_MEASURE_PIPE3m,
   DLB_ECMP_PORT_INST_QUALITY_MEASURE_PIPE0m,
   DLB_ECMP_PORT_INST_QUALITY_MEASURE_PIPE1m,
   DLB_ECMP_PORT_INST_QUALITY_MEASURE_PIPE2m,
   DLB_ECMP_PORT_INST_QUALITY_MEASURE_PIPE3m,
   DLB_ECMP_PORT_QUALITY_MAPPINGm,
   DLB_ECMP_PORT_QUALITY_UPDATE_MEASURE_CONTROL_PIPE0m,
   DLB_ECMP_PORT_QUALITY_UPDATE_MEASURE_CONTROL_PIPE1m,
   DLB_ECMP_PORT_QUALITY_UPDATE_MEASURE_CONTROL_PIPE2m,
   DLB_ECMP_PORT_QUALITY_UPDATE_MEASURE_CONTROL_PIPE3m,
   DLB_ECMP_PORT_STATEm,
   DLB_ECMP_QUANTIZE_CONTROLm,
   DLB_ECMP_QUANTIZED_AVG_QUALITY_MEASURE_PIPE0m,
   DLB_ECMP_QUANTIZED_AVG_QUALITY_MEASURE_PIPE1m,
   DLB_ECMP_QUANTIZED_AVG_QUALITY_MEASURE_PIPE2m,
   DLB_ECMP_QUANTIZED_AVG_QUALITY_MEASURE_PIPE3m,
   INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "dgm" feature for tomahawk2 chipset. */
techsupport_reg_t techsupport_dgm_tomahawk2_reg_list[] = {
   {DLB_ECMP_CURRENT_TIMEr, register_type_global },
   {DLB_ECMP_PORT_XPE_A_B_SELECTORr, register_type_global },
   {DLB_ECMP_QUALITY_MEASURE_CONTROLr, register_type_global },
   {DLB_ECMP_RANDOM_SELECTION_CONTROLr, register_type_global },
   {DLB_ECMP_REFRESH_DISABLEr, register_type_global },
   {DLB_ECMP_REFRESH_INDEXr, register_type_global },
   {DLB_ECMP_SER_CONTROLr, register_type_global },
   {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "dgm" feature for tomahawk2 chipset. */
techsupport_data_t techsupport_dgm_tomahawk2_data = {
    techsupport_dgm_diag_cmdlist,
    techsupport_dgm_tomahawk2_reg_list,
    techsupport_dgm_tomahawk2_memory_table_list,
    techsupport_dgm_tomahawk2_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_dgm_sw_dump_cmdlist,
    techsupport_dgm_tomahawk2_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};

