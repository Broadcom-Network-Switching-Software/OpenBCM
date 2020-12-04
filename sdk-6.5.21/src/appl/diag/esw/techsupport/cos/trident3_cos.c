/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    trident3_cos.c
 * Purpose: Maintains all the debug information for cos
 *          feature for tomahawk.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_cos_diag_cmdlist[];
extern char * techsupport_cos_sw_dump_cmdlist[];

/* "cos" feature's diag command list valid only for Trident3 */
char * techsupport_cos_trident3_diag_cmdlist[] = {
     NULL /* Must be the last element in this structure */
 };

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "cos" feature's software dump command list valid only for Trident3 */
char * techsupport_cos_trident3_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "cos" feature for Trident3 chipset. */
static soc_mem_t techsupport_cos_trident3_memory_table_list[] = {
    COS_MAP_SELm,
    CPU_COS_MAPm,
    MMU_THDM_DB_QUEUE_BST_XPE0_PIPE0m,
    MMU_THDM_DB_QUEUE_BST_XPE0_PIPE1m,
    MMU_THDU_BST_QUEUE_XPE0_PIPE0m,
    MMU_THDU_BST_QUEUE_XPE0_PIPE1m,
    MMU_THDU_Q_TO_QGRP_MAP_PIPE0m,
    MMU_THDU_Q_TO_QGRP_MAP_PIPE1m,
    MMU_WRED_AVG_QSIZEm,
    MMU_WRED_CONFIGm,
    MMU_WRED_DROP_CURVE_PROFILE_0m,
    MMU_WRED_DROP_CURVE_PROFILE_1m,
    MMU_WRED_DROP_CURVE_PROFILE_2m,
    MMU_WRED_DROP_CURVE_PROFILE_3m,
    MMU_WRED_DROP_CURVE_PROFILE_4m,
    MMU_WRED_DROP_CURVE_PROFILE_5m,
    MMU_WRED_DROP_CURVE_PROFILE_6m,
    MMU_WRED_DROP_CURVE_PROFILE_7m,
    MMU_WRED_DROP_CURVE_PROFILE_8m,
    MMU_WRED_PORT_SP_DROP_THDm,
    MMU_WRED_PORT_SP_DROP_THD_MARKm,
    MMU_WRED_PORT_SP_SHARED_COUNTm,
    MMU_WRED_UC_QUEUE_DROP_THD_0m,
    MMU_WRED_UC_QUEUE_DROP_THD_1m,
    MMU_WRED_UC_QUEUE_DROP_THD_MARKm,
    MMU_WRED_UC_QUEUE_TOTAL_COUNTm,
    Q_SCHED_L0_WEIGHT_MEM_PIPE0m,
    Q_SCHED_L0_WEIGHT_MEM_PIPE1m,
    THDI_PORT_PG_BST_XPE0_PIPE0m,
    THDI_PORT_PG_BST_XPE0_PIPE1m,
    THDI_PORT_SP_BST_XPE0_PIPE0m,
    THDI_PORT_SP_BST_XPE0_PIPE1m,
    INT_CN_TO_MMUIF_MAPPINGm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "cos" feature for Trident3 chipset. */
techsupport_reg_t techsupport_cos_trident3_reg_list[] = {
    {CFAPBSTSTATr, register_type_global },
    {MMU_GCFG_BST_HW_SNAPSHOT_ENr, register_type_global },
    {MMU_GCFG_BST_SNAPSHOT_ACTION_ENr, register_type_global },
    {MMU_GCFG_BST_TRACKING_ENABLEr, register_type_global },
    {MMU_GCFG_MISCCONFIGr, register_type_global },
    {MMU_SCFG_SC_CPU_INT_ENr, register_type_global },
    {MMU_XCFG_XPE_CPU_INT_ENr, register_type_global },
    {WRED_REFRESH_CONTROLr, register_type_global },
    {WRED_POOL_INST_CONG_LIMIT_3r, register_type_global },
    {WRED_POOL_INST_CONG_LIMIT_2r, register_type_global },
    {WRED_POOL_INST_CONG_LIMIT_1r, register_type_global },
    {WRED_POOL_INST_CONG_LIMIT_0r, register_type_global },
    {WRED_CONG_NOTIFICATION_RESOLUTION_TABLEr, register_type_global },
    {Q_SCHED_PORT_CONFIGr, register_type_global },
    {THDI_HDRM_POOL_CFGr, register_type_global },
    {THDI_PORT_PG_SPIDr, register_type_global },
    {THDI_PORT_PRI_GRP0r, register_type_global },
    {THD_MISC_CONTROLr, register_type_global },
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "cos" feature for Tomahawk chipset. */
techsupport_data_t techsupport_cos_trident3_data = {
    techsupport_cos_diag_cmdlist,
    techsupport_cos_trident3_reg_list,
    techsupport_cos_trident3_memory_table_list,
    techsupport_cos_trident3_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_cos_sw_dump_cmdlist,
    techsupport_cos_trident3_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};
