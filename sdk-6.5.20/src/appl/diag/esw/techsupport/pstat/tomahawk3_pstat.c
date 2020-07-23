/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: tomahawk3_pstat.c
 *
 * Purpose:
 *    Maintains all the debug information for pstat feature for tomahawk3
 *
 * Note: This file is auto-generated. Manual edits
 *       to it will be lost when it is auto-regenerated.
 */

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_pstat_diag_cmdlist[];
extern char * techsupport_pstat_sw_dump_cmdlist[];

/* "pstat" feature's diag command list valid only for tomahawk3 */
char * techsupport_pstat_tomahawk3_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "pstat" feature's software dump command list valid only for tomahawk3 */
char * techsupport_pstat_tomahawk3_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "pstat" feature for tomahawk3 chipset. */
static soc_mem_t techsupport_pstat_tomahawk3_memory_table_list[] = {
    DLB_ECMP_FLOWSET_TIMESTAMP_PAGE_PIPE0m,
    DLB_ECMP_FLOWSET_TIMESTAMP_PAGE_PIPE1m,
    DLB_ECMP_FLOWSET_TIMESTAMP_PAGE_PIPE2m,
    DLB_ECMP_FLOWSET_TIMESTAMP_PAGE_PIPE3m,
    DLB_ECMP_FLOWSET_TIMESTAMP_PAGE_PIPE4m,
    DLB_ECMP_FLOWSET_TIMESTAMP_PAGE_PIPE5m,
    DLB_ECMP_FLOWSET_TIMESTAMP_PAGE_PIPE6m,
    DLB_ECMP_FLOWSET_TIMESTAMP_PAGE_PIPE7m,
    MMU_GLBCFG_TOD_TIMESTAMPm,
    MMU_THDI_PORTSP_BST_PIPE0m,
    MMU_THDI_PORTSP_BST_PIPE1m,
    MMU_THDI_PORTSP_BST_PIPE2m,
    MMU_THDI_PORTSP_BST_PIPE3m,
    MMU_THDI_PORTSP_BST_PIPE4m,
    MMU_THDI_PORTSP_BST_PIPE5m,
    MMU_THDI_PORTSP_BST_PIPE6m,
    MMU_THDI_PORTSP_BST_PIPE7m,
    MMU_THDI_PORT_BST_CONFIG_PIPE0m,
    MMU_THDI_PORT_BST_CONFIG_PIPE1m,
    MMU_THDI_PORT_BST_CONFIG_PIPE2m,
    MMU_THDI_PORT_BST_CONFIG_PIPE3m,
    MMU_THDI_PORT_BST_CONFIG_PIPE4m,
    MMU_THDI_PORT_BST_CONFIG_PIPE5m,
    MMU_THDI_PORT_BST_CONFIG_PIPE6m,
    MMU_THDI_PORT_BST_CONFIG_PIPE7m,
    MMU_THDI_PORT_PG_HDRM_BST_PIPE0m,
    MMU_THDI_PORT_PG_HDRM_BST_PIPE1m,
    MMU_THDI_PORT_PG_HDRM_BST_PIPE2m,
    MMU_THDI_PORT_PG_HDRM_BST_PIPE3m,
    MMU_THDI_PORT_PG_HDRM_BST_PIPE4m,
    MMU_THDI_PORT_PG_HDRM_BST_PIPE5m,
    MMU_THDI_PORT_PG_HDRM_BST_PIPE6m,
    MMU_THDI_PORT_PG_HDRM_BST_PIPE7m,
    MMU_THDI_PORT_PG_SHARED_BST_PIPE0m,
    MMU_THDI_PORT_PG_SHARED_BST_PIPE1m,
    MMU_THDI_PORT_PG_SHARED_BST_PIPE2m,
    MMU_THDI_PORT_PG_SHARED_BST_PIPE3m,
    MMU_THDI_PORT_PG_SHARED_BST_PIPE4m,
    MMU_THDI_PORT_PG_SHARED_BST_PIPE5m,
    MMU_THDI_PORT_PG_SHARED_BST_PIPE6m,
    MMU_THDI_PORT_PG_SHARED_BST_PIPE7m,
    MMU_THDO_BST_SHARED_PORTSP_MC_ITM0m,
    MMU_THDO_BST_SHARED_PORTSP_MC_ITM1m,
    MMU_THDO_BST_SHARED_PORTSP_MC_PKTSTAT_ITM0m,
    MMU_THDO_BST_SHARED_PORTSP_MC_PKTSTAT_ITM1m,
    MMU_THDO_BST_SHARED_PORTSP_MC_PKTSTATm,
    MMU_THDO_BST_SHARED_PORTSP_MCm,
    MMU_THDO_BST_SHARED_PORT_ITM0m,
    MMU_THDO_BST_SHARED_PORT_ITM1m,
    MMU_THDO_BST_SHARED_PORTm,
    MMU_THDO_BST_TOTAL_QUEUE_ITM0m,
    MMU_THDO_BST_TOTAL_QUEUE_ITM1m,
    MMU_THDO_BST_TOTAL_QUEUE_PKTSTAT_ITM0m,
    MMU_THDO_BST_TOTAL_QUEUE_PKTSTAT_ITM1m,
    MMU_THDO_BST_TOTAL_QUEUE_PKTSTATm,
    MMU_THDO_BST_TOTAL_QUEUEm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "pstat" feature for tomahawk3 chipset. */
techsupport_reg_t techsupport_pstat_tomahawk3_reg_list[] = {
    { MMU_CFAP_BSTCONFIGr, register_type_global },
    { MMU_CFAP_BSTSTATr, register_type_global },
    { MMU_CFAP_BSTTHRSr, register_type_global },
    { MMU_GLBCFG_BST_SNAPSHOT_ACTION_ENr, register_type_global },
    { MMU_GLBCFG_BST_TRACKING_ENABLEr, register_type_global },
    { MMU_GLBCFG_TIMESTAMPr, register_type_global },
    { MMU_GLBCFG_UTC_TIMESTAMPr, register_type_global },
    { MMU_THDI_BSTCONFIGr, register_type_global },
    { MMU_THDI_BST_HDRM_POOL_CNTr, register_type_global },
    { MMU_THDI_BST_HDRM_POOLr, register_type_global },
    { MMU_THDI_BST_PG_HDRM_PROFILEr, register_type_global },
    { MMU_THDI_BST_PG_SHARED_PROFILEr, register_type_global },
    { MMU_THDI_BST_PORTSP_SHARED_PROFILEr, register_type_global },
    { MMU_THDI_BST_SP_SHARED_CNTr, register_type_global },
    { MMU_THDI_BST_SP_SHAREDr, register_type_global },
    { MMU_THDI_BST_TRIGGER_STATUS_TYPEr, register_type_per_port },
    { MMU_THDO_BST_CONFIGr, register_type_global },
    { MMU_THDO_BST_CPU_INT_ENr, register_type_global },
    { MMU_THDO_BST_CPU_INT_SETr, register_type_global },
    { MMU_THDO_BST_CPU_INT_STATr, register_type_global },
    { MMU_THDO_BST_ENABLE_ECCP_MEMr, register_type_global },
    { MMU_THDO_BST_EN_COR_ERR_RPTr, register_type_global },
    { MMU_THDO_BST_STAT1r, register_type_global },
    { MMU_THDO_BST_STATr, register_type_global },
    { MMU_THDO_BST_TMBUSr, register_type_global },
    { MMU_THDO_MC_CPU_QUEUE_TOT_BST_THRESHOLDr, register_type_global },
    { MMU_THDO_MC_CQE_PRT_SP_BST_THRESHOLDr, register_type_global },
    { MMU_THDO_MC_CQE_SP_BST_THRESHOLDr, register_type_global },
    { MMU_THDO_MC_POOL_BST_COUNTr, register_type_global },
    { MMU_THDO_MC_QUEUE_TOT_BST_THRESHOLDr, register_type_global },
    { MMU_THDO_PRT_SP_SHR_BST_THRESHOLDr, register_type_global },
    { MMU_THDO_QUE_TOT_BST_THRESHOLDr, register_type_global },
    { MMU_THDO_SP_SHR_BST_THRESHOLDr, register_type_global },
    { MMU_THDO_UC_POOL_BST_COUNTr, register_type_global },
    { MMU_THDR_QE_BST_CONFIGr, register_type_global },
    { MMU_THDR_QE_BST_COUNT_PRIQr, register_type_global },
    { MMU_THDR_QE_BST_COUNT_SPr, register_type_global },
    { MMU_THDR_QE_BST_STATr, register_type_global },
    { MMU_THDR_QE_BST_THRESHOLD_PRIQr, register_type_global },
    { MMU_THDR_QE_BST_THRESHOLD_SPr, register_type_global },
    { NS_TIMESYNC_TS0_TIMESTAMP_LOWER_STATUSr, register_type_global },
    { NS_TIMESYNC_TS0_TIMESTAMP_UPPER_STATUSr, register_type_global },
    { NS_TIMESYNC_TS1_TIMESTAMP_LOWER_STATUSr, register_type_global },
    { NS_TIMESYNC_TS1_TIMESTAMP_UPPER_STATUSr, register_type_global },
    { THDI_BST_TRIGGER_STATUS_32r, register_type_per_port },
    { XLMAC_TIMESTAMP_ADJUSTr, register_type_per_port },
    { XLMAC_TIMESTAMP_BYTE_ADJUSTr, register_type_per_port },
    { XLMAC_TX_TIMESTAMP_FIFO_DATAr, register_type_per_port },
    { XLMAC_TX_TIMESTAMP_FIFO_STATUSr, register_type_per_port },
    { INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains diag cmdlist, reg_list, mem_list  for
 * "pstat" feature for tomahawk3 chipset. */
techsupport_data_t techsupport_pstat_tomahawk3_data = {
    techsupport_pstat_diag_cmdlist,
    techsupport_pstat_tomahawk3_reg_list,
    techsupport_pstat_tomahawk3_memory_table_list,
    techsupport_pstat_tomahawk3_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_pstat_sw_dump_cmdlist,
    techsupport_pstat_tomahawk3_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};
