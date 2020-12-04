/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    tomahawk2_pstat.c
 * Purpose: Maintains all the debug information for pstat
 *          feature for tomahawk2.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_pstat_diag_cmdlist[];
extern char * techsupport_pstat_sw_dump_cmdlist[];

/* "pstat" feature's diag command list valid only for tomahawk2 */
char * techsupport_pstat_tomahawk2_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "pstat" feature's software dump command list valid only for tomahawk2 */
char * techsupport_pstat_tomahawk2_sw_dump_cmdlist[] = {
    "dump sw pstats",
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "pstat" feature for tomahawk2 chipset. */
static soc_mem_t techsupport_pstat_tomahawk2_memory_table_list[] = {
   THDI_PKT_STAT_SP_SHARED_COUNT_XPE0m,
   THDI_PKT_STAT_SP_SHARED_COUNT_XPE1m,
   THDI_PKT_STAT_SP_SHARED_COUNT_XPE2m,
   THDI_PKT_STAT_SP_SHARED_COUNT_XPE3m,
   MMU_THDU_UCQ_STATS_TABLE_XPE0_PIPE0m,
   MMU_THDU_UCQ_STATS_TABLE_XPE0_PIPE1m,
   MMU_THDU_UCQ_STATS_TABLE_XPE1_PIPE2m,
   MMU_THDU_UCQ_STATS_TABLE_XPE1_PIPE3m,
   MMU_THDU_UCQ_STATS_TABLE_XPE2_PIPE0m,
   MMU_THDU_UCQ_STATS_TABLE_XPE2_PIPE1m,
   MMU_THDU_UCQ_STATS_TABLE_XPE3_PIPE2m,
   MMU_THDU_UCQ_STATS_TABLE_XPE3_PIPE3m,
   MMU_THDU_UCQ_STATS_XPE0_PIPE0m,
   MMU_THDU_UCQ_STATS_XPE0_PIPE1m,
   MMU_THDU_UCQ_STATS_XPE1_PIPE2m,
   MMU_THDU_UCQ_STATS_XPE1_PIPE3m,
   MMU_THDU_UCQ_STATS_XPE2_PIPE0m,
   MMU_THDU_UCQ_STATS_XPE2_PIPE1m,
   MMU_THDU_UCQ_STATS_XPE3_PIPE2m,
   MMU_THDU_UCQ_STATS_XPE3_PIPE3m,
   MMU_THDM_DB_POOL_MCUC_PKSTAT_XPE0m,
   MMU_THDM_DB_POOL_MCUC_PKSTAT_XPE1m,
   MMU_THDM_DB_POOL_MCUC_PKSTAT_XPE2m,
   MMU_THDM_DB_POOL_MCUC_PKSTAT_XPE3m,
   MMU_INTFO_TIMESTAMPm,
   MMU_INTFO_UTC_TIMESTAMPm,
   INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "pstat" feature for tomahawk2 chipset. */
techsupport_reg_t techsupport_pstat_tomahawk2_reg_list[] = {
   {MMU_GCFG_PKTSTAT_OOBSTATr, register_type_global },
   {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "pstat" feature for tomahawk2 chipset. */
techsupport_data_t techsupport_pstat_tomahawk2_data = {
    techsupport_pstat_diag_cmdlist,
    techsupport_pstat_tomahawk2_reg_list,
    techsupport_pstat_tomahawk2_memory_table_list,
    techsupport_pstat_tomahawk2_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_pstat_sw_dump_cmdlist,
    techsupport_pstat_tomahawk2_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};

