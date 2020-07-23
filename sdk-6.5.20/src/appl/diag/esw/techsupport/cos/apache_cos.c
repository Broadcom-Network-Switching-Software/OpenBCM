/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    apache_cos.c
 * Purpose: Maintains all the debug information for cos
 *          feature for apache.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_cos_diag_cmdlist[];
extern char * techsupport_cos_sw_dump_cmdlist[];

/* "cos" feature's diag command list valid only for apache */
char * techsupport_cos_apache_diag_cmdlist[] = {
     "lls sw",
     "hsp",
     NULL /* Must be the last element in this structure */
 };

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "cos" feature's software dump command list valid only for apache */
char * techsupport_cos_apache_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "cos" feature for apache chipset. */
static soc_mem_t techsupport_cos_apache_memory_table_list[] = {
    CPU_TS_MAPm,
    ING_ETAG_PCP_MAPPINGm,
    ING_MPLS_EXP_MAPPINGm,
    ING_OUTER_DOT1P_MAPPING_TABLEm,
    DSCP_TABLEm,
    ING_PRI_CNG_MAPm,
    ING_UNTAGGED_PHBm,
    PORT_COS_MAPm,
    COS_MAP_SELm,
    CPU_COS_MAPm,
    ENDPOINT_COS_MAPm,
    ENDPOINT_QUEUE_MAPm,
    IFP_COS_MAPm,
    ING_ROUTED_INT_PRI_MAPPINGm,
    PHB2_COS_MAPm,
    SERVICE_COS_MAPm,
    SERVICE_PORT_MAPm,
    SERVICE_QUEUE_MAPm,
    VOQ_COS_MAPm,
    VOQ_MOD_MAPm,
    VOQ_PORT_MAPm,
    MMU_MTRI_BKPMETERINGBUCKET_MEM_0m,
    MMU_MTRI_BKPMETERINGCONFIG_MEM_0m,
    MMU_MTRO_EGRMETERINGBUCKET_MEM_0m,
    MMU_MTRO_EGRMETERINGCONFIG_MEM_0m,
    MMU_MTRO_BUCKET_L0_MEM_0m,
    MMU_MTRO_BUCKET_L1_MEM_0m,
    MMU_MTRO_BUCKET_L2_MEM_0m,
    MMU_MTRO_L0_MEM_0m,
    MMU_MTRO_L0_MEM_1m,
    MMU_MTRO_L1_MEM_0m,
    MMU_MTRO_L1_MEM_1m,
    MMU_MTRO_L2_MEM_0m,
    MMU_MTRO_L2_MEM_1m,
    LLS_PORT_HEADSm,
    LLS_PORT_SEQ_NUMm,
    LLS_PORT_TAILSm,
    LLS_PORT_WERR_MAX_SCm,
    LLS_L0_CHILD_STATE1m,
    LLS_L0_CHILD_WEIGHT_CFG_CNTm,
    LLS_L0_ERRORm,
    LLS_L0_HEADS_TAILS_1m,
    LLS_L0_HEADS_TAILS_2m,
    LLS_L0_MIN_NEXTm,
    LLS_L0_PARENTm,
    LLS_L0_PARENT_STATEm,
    LLS_L0_WERR_MAX_SCm,
    LLS_L0_WERR_NEXTm,
    LLS_L0_XOFFm,
    LLS_L1_CHILD_STATE1m,
    LLS_L1_CHILD_WEIGHT_CFG_CNTm,
    LLS_L1_ERRORm,
    LLS_L1_HEADS_TAILS_1m,
    LLS_L1_HEADS_TAILS_2m,
    LLS_L1_MIN_NEXTm,
    LLS_L1_PARENTm,
    LLS_L1_PARENT_STATEm,
    LLS_L1_WERR_MAX_SCm,
    LLS_L1_WERR_NEXTm,
    LLS_L1_XOFFm,
    LLS_L2_CHILD_STATE1m,
    LLS_L2_CHILD_WEIGHT_CFG_CNTm,
    LLS_L2_MC_CHILD_WEIGHT_CFG_CNTm,
    LLS_L2_ERRORm,
    LLS_L2_MIN_NEXTm,
    LLS_L2_PARENTm,
    LLS_L2_WERR_NEXTm,
    LLS_L2_XOFFm,
    MMU_WRED_CONFIG_X_PIPEm,
    MMU_WRED_DROP_CURVE_PROFILE_0_X_PIPEm,
    MMU_WRED_DROP_CURVE_PROFILE_1_X_PIPEm,
    MMU_WRED_DROP_CURVE_PROFILE_2_X_PIPEm,
    MMU_WRED_DROP_CURVE_PROFILE_3_X_PIPEm,
    MMU_WRED_DROP_CURVE_PROFILE_4_X_PIPEm,
    MMU_WRED_DROP_CURVE_PROFILE_5_X_PIPEm,
    MMU_WRED_PORT_SP_DROP_THD_X_PIPEm,
    MMU_WRED_PORT_SP_SHARED_COUNT_X_PIPEm,
    MMU_WRED_QGROUP_SHARED_COUNT_X_PIPEm,
    MMU_WRED_AVG_QSIZE_X_PIPEm,
    MMU_WRED_UC_QUEUE_TOTAL_COUNT_X_PIPEm,
    MMU_WRED_QGROUP_DROP_THD_X_PIPEm,
    MMU_WRED_UC_QUEUE_DROP_THD_DEQ_X_PIPE_0m,
    MMU_WRED_UC_QUEUE_DROP_THD_ENQ_X_PIPE_0m,
    MMU_CTR_MTRI_DROP_MEMm,
    EGR_DSCP_ECN_MAPm,
    EGR_DSCP_TABLEm,
    EGR_ETAG_PCP_MAPPINGm,
    EGR_MPLS_EXP_MAPPING_1m,
    EGR_MPLS_EXP_MAPPING_2m,
    EGR_PRI_CNG_MAPm,
    EGR_MPLS_EXP_PRI_MAPPINGm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "cos" feature for apache chipset. */
techsupport_reg_t techsupport_cos_apache_reg_list[] = {
    {BKPMETERINGDISCSTATUS0r, register_type_global},
    {BKPMETERINGDISCSTATUS1r, register_type_global},
    {BKPMETERINGWARNSTATUS0r, register_type_global},
    {BKPMETERINGWARNSTATUS1r, register_type_global},
    {CMIC_CMC0_CH0_COS_CTRL_RX_0r, register_type_global},
    {CMIC_CMC0_CH0_COS_CTRL_RX_1r, register_type_global},
    {CMIC_CMC0_CH1_COS_CTRL_RX_0r, register_type_global},
    {CMIC_CMC0_CH1_COS_CTRL_RX_1r, register_type_global},
    {CMIC_CMC0_CH2_COS_CTRL_RX_0r, register_type_global},
    {CMIC_CMC0_CH2_COS_CTRL_RX_1r, register_type_global},
    {CMIC_CMC0_CH3_COS_CTRL_RX_0r, register_type_global},
    {CMIC_CMC0_CH3_COS_CTRL_RX_1r, register_type_global},
    {CMIC_CMC0_PROGRAMMABLE_COS_MASK0r, register_type_global},
    {CMIC_CMC0_PROGRAMMABLE_COS_MASK1r, register_type_global},
    {CMIC_CMC1_CH0_COS_CTRL_RX_0r, register_type_global},
    {CMIC_CMC1_CH0_COS_CTRL_RX_1r, register_type_global},
    {CMIC_CMC1_CH1_COS_CTRL_RX_0r, register_type_global},
    {CMIC_CMC1_CH1_COS_CTRL_RX_1r, register_type_global},
    {CMIC_CMC1_CH2_COS_CTRL_RX_0r, register_type_global},
    {CMIC_CMC1_CH2_COS_CTRL_RX_1r, register_type_global},
    {CMIC_CMC1_CH3_COS_CTRL_RX_0r, register_type_global},
    {CMIC_CMC1_CH3_COS_CTRL_RX_1r, register_type_global},
    {CMIC_CMC1_PROGRAMMABLE_COS_MASK0r, register_type_global},
    {CMIC_CMC1_PROGRAMMABLE_COS_MASK1r, register_type_global},
    {CMIC_CMC2_CH0_COS_CTRL_RX_0r, register_type_global},
    {CMIC_CMC2_CH0_COS_CTRL_RX_1r, register_type_global},
    {CMIC_CMC2_CH1_COS_CTRL_RX_0r, register_type_global},
    {CMIC_CMC2_CH1_COS_CTRL_RX_1r, register_type_global},
    {CMIC_CMC2_CH2_COS_CTRL_RX_0r, register_type_global},
    {CMIC_CMC2_CH2_COS_CTRL_RX_1r, register_type_global},
    {CMIC_CMC2_CH3_COS_CTRL_RX_0r, register_type_global},
    {CMIC_CMC2_CH3_COS_CTRL_RX_1r, register_type_global},
    {CMIC_CMC2_PROGRAMMABLE_COS_MASK0r, register_type_global},
    {CMIC_CMC2_PROGRAMMABLE_COS_MASK1r, register_type_global},
    {CMIC_PKT_COS_0r, register_type_global},
    {CMIC_PKT_COS_1r, register_type_global},
    {CMIC_PKT_COS_QUEUES_HIr, register_type_global},
    {CMIC_PKT_COS_QUEUES_LOr, register_type_global},
    {COS_MODE_X_64_0r, register_type_global},
    {COS_MODE_X_64_1r, register_type_global},
    {LLS_CONFIG0r, register_type_global},
    {LLS_FC_CONFIGr, register_type_global},
    {LLS_SP_WERR_DYN_CHANGE_0Ar, register_type_global},
    {LLS_SP_WERR_DYN_CHANGE_0Br, register_type_global},
    {LLS_SP_WERR_DYN_CHANGE_0Cr, register_type_global},
    {LLS_SP_WERR_DYN_CHANGE_1Ar, register_type_global},
    {LLS_SP_WERR_DYN_CHANGE_1Br, register_type_global},
    {LLS_SP_WERR_DYN_CHANGE_1Cr, register_type_global},
    {LLS_SP_WERR_DYN_CHANGE_2Ar, register_type_global},
    {LLS_SP_WERR_DYN_CHANGE_2Br, register_type_global},
    {LLS_SP_WERR_DYN_CHANGE_2Cr, register_type_global},
    {LLS_SP_WERR_DYN_CHANGE_3Ar, register_type_global},
    {LLS_SP_WERR_DYN_CHANGE_3Br, register_type_global},
    {LLS_SP_WERR_DYN_CHANGE_3Cr, register_type_global},
    {HSP_SCHED_GLOBAL_CONFIGr, register_type_global},
    {HSP_SCHED_L0_NODE_CONFIGr, register_type_global},
    {HSP_SCHED_L0_NODE_CONNECTION_CONFIGr, register_type_global},
    {HSP_SCHED_L0_NODE_WEIGHTr, register_type_global},
    {HSP_SCHED_L1_NODE_CONFIGr, register_type_global},
    {HSP_SCHED_L1_NODE_WEIGHTr, register_type_global},
    {HSP_SCHED_L2_MC_QUEUE_CONFIGr, register_type_global},
    {HSP_SCHED_L2_MC_QUEUE_WEIGHTr, register_type_global},
    {HSP_SCHED_L2_UC_QUEUE_CONFIGr, register_type_global},
    {HSP_SCHED_L2_UC_QUEUE_WEIGHTr, register_type_global},
    {HSP_SCHED_PORT_CONFIGr, register_type_global},
    {ING_COS_MODE_64r, register_type_global},
    {ING_MIRROR_COS_CONTROLr, register_type_global},
    {MMU_TO_XPORT_BKPr, register_type_global},
    {MTP_COSr, register_type_global},
    {MTRI_IFGr, register_type_global},
    {MTRI_REFRESH_CONFIGr, register_type_global},
    {MTRO_REFRESH_CONFIGr, register_type_global},
    {MTRO_REFRESH_STATUSr, register_type_global},
    {PRIO2COS_PROFILEr, register_type_global},
    {PRIORITY_CONTROLr, register_type_global},
    {STORM_CONTROL_METER_CONFIGr, register_type_global},
    {STORM_CONTROL_METER_MAPPINGr, register_type_global},
    {XPORT_TO_MMU_BKPr, register_type_global},
    {WREDMEMDEBUG_AVG_QSIZE_X_PIPE_TMr, register_type_global},
    {WREDMEMDEBUG_CONFIG_X_PIPE_TMr, register_type_global},
    {WREDMEMDEBUG_DROP_CURVE_PROFILE_TMr, register_type_global},
    {WREDMEMDEBUG_PORT_SP_DROP_THD_X_PIPE_TMr, register_type_global},
    {WREDMEMDEBUG_PORT_SP_SHARED_COUNT_X_PIPE_TMr, register_type_global},
    {WREDMEMDEBUG_QGROUP_DROP_THD_X_PIPE_TMr, register_type_global},
    {WREDMEMDEBUG_QGROUP_SHARED_COUNT_X_PIPE_TMr, register_type_global},
    {WREDMEMDEBUG_UC_QUEUE_DROP_THD_DEQ_X_PIPE_0_TMr, register_type_global},
    {WREDMEMDEBUG_UC_QUEUE_DROP_THD_ENQ_X_PIPE_0_TMr, register_type_global},
    {WREDMEMDEBUG_UC_QUEUE_TOTAL_COUNT_X_PIPE_TMr, register_type_global},
    {WRED_EN_COR_ERR_RPT_X_PIPEr, register_type_global},
    {WRED_REFRESH_CONTROLr, register_type_global},
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "cos" feature for apache chipset. */
techsupport_data_t techsupport_cos_apache_data = {
    techsupport_cos_diag_cmdlist,
    techsupport_cos_apache_reg_list,
    techsupport_cos_apache_memory_table_list,
    techsupport_cos_apache_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_cos_sw_dump_cmdlist,
    techsupport_cos_apache_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};

