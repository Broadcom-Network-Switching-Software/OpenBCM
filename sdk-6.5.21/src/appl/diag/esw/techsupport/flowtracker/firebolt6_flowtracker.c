/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    firebolt6_flowtracker.c
 * Purpose:Maintains all the debug information for flowtracker
 *         feature for Firebolt6.
 */

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

#ifdef BCM_FLOWTRACKER_V3_SUPPORT
extern char * techsupport_flowtracker_diag_cmdlist[];
#ifndef BCM_SW_STATE_DUMP_DISABLE
extern char * techsupport_flowtracker_sw_dump_cmdlist[];
#endif

/* "flowtracker" feature's diag command list for specific device, currently NULL */
char * techsupport_flowtracker_firebolt6_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "flowtracker" feature's software dump command list for specific device, currently NULL */
char * techsupport_flowtracker_firebolt6_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/* Structure that maintains register list for "flowtracker" feature
 * for Firebolt6 device */
techsupport_reg_t techsupport_flowtracker_firebolt6_reg_list[] = {
    { BSC_DT_EXPORT_BANK_VALID_BITMAPr, register_type_global },
    { BSC_DT_RANDOM_SEED_CONFIGr, register_type_global },
    { BSC_DT_BANK_TO_INSTANCE_MAPPING_CONTROLr, register_type_global },
    { BSC_DT_ALU16_TO_INSTANCE_MAPPING_CONTROLr, register_type_global },
    { BSC_DT_ALU32_TO_INSTANCE_MAPPING_CONTROLr, register_type_global },
    { BSC_DT_LOAD8_TO_INSTANCE_MAPPING_CONTROLr, register_type_global },
    { BSC_DT_LOAD16_TO_INSTANCE_MAPPING_CONTROLr, register_type_global },
    { BSC_DT_TIMESTAMP_ENGINE_TO_INSTANCE_MAPPING_CONTROLr, register_type_global },
    { BSC_AG_DEBUG_REGISTERr, register_type_global },
    { BSC_AG_GROUP_FLUSH_CONFIG_64r, register_type_global },
    { BSC_AG_SCAN_PERIOD_SELECTr, register_type_global },
    { BSC_AG_SCAN_CONTROLr, register_type_global },
    { BSC_AG_PERIODIC_EXPORTr, register_type_global },
    { BSC_AG_LOSSLESS_PERIODIC_EXPORT_STATUSr, register_type_global },
    { BSC_EX_EXPORT_SET_LENGTHr, register_type_global },
    { BSC_EX_EXPORT_CONTROLr, register_type_global },
    { BSC_EX_SEQ_NUMr, register_type_global },
    { BSC_EX_BROADSCAN_RA_FT_ENABLEr, register_type_global },
    { IPFIX_TOD_SELECT_0r, register_type_global },
    { IPFIX_TOD_SELECT_1r, register_type_global },
    { IPFIX_TOD_SELECT_2r, register_type_global },
    { IPFIX_TOD_SELECT_3r, register_type_global },
    { IPFIX_TOD_SELECT_4r, register_type_global },
    { IPFIX_TOD_SELECT_5r, register_type_global },
    { IPFIX_TOD_SELECT_6r, register_type_global },
    { IPFIX_TOD_SELECT_7r, register_type_global },
    { BSC_EX_BROADSCAN_IPFIX_CREDIT_RESETr,register_type_global },
    { BSC_EX_GLOBAL_EVENT_VECTOR_CONTROL_ON_SERr,register_type_global },
    { BSC_EX_BROADSCAN_GLOBAL_SER_DROP_ENABLEr,register_type_global },
    { BSC_KT_SW_INSERT_CTRL_INFOr, register_type_global },
    { BSK_EFT_AFIFO_DROP_COUNTr, register_type_global },
    { BSK_EFT_AFIFO_MAX_LEVELr, register_type_global },
    { BSK_EFT_READ_IDLE_CONTROLr, register_type_global },
    { BSK_EFT_VALID_PKT_COUNTr, register_type_global },
    { BSK_HS_HW_RESET_CONTROLr, register_type_global },
    { BSK_MFT_AFIFO_DROP_COUNTr, register_type_global },
    { BSK_MFT_AFIFO_MAX_LEVELr, register_type_global },
    { BSK_MFT_READ_IDLE_CONTROLr, register_type_global },
    { BSK_MFT_VALID_PKT_COUNTr, register_type_global },
    { IDB_CA_BSK_HW_STATUSr, register_type_global },
    { BSC_DT_BANK_TO_AGG_INSTANCE_MAPPING_CONTROLr, register_type_global },
    { BSC_AG_AFT_PERIODIC_EXPORTr, register_type_global },
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains memory list for "flowtracker" feature
 * for Firebolt6 device */
static soc_mem_t techsupport_flowtracker_firebolt6_memory_table_list[] = {
    BSC_AGG_FIFO_STATSm,
    BSC_AG_AGE_TABLEm,
    BSC_AG_AIGID_CONFIG_TABLEm,
    BSC_DG_AEFT_GROUP_TABLEm,
    BSC_DG_AGG_GROUP_ALU32_PROFILE_0m,
    BSC_DG_AGG_GROUP_ALU32_PROFILE_1m,
    BSC_DG_AGG_GROUP_ALU32_PROFILE_2m,
    BSC_DG_AGG_GROUP_ALU32_PROFILE_3m,
    BSC_DG_AGG_GROUP_ALU32_PROFILE_4m,
    BSC_DG_AGG_GROUP_ALU32_PROFILE_5m,
    BSC_DG_AGG_GROUP_ALU32_PROFILE_6m,
    BSC_DG_AGG_GROUP_ALU32_PROFILE_7m,
    BSC_DG_AGG_GROUP_ALU32_PROFILE_8m,
    BSC_DG_AGG_GROUP_ALU32_PROFILE_9m,
    BSC_DG_AGG_GROUP_ALU32_PROFILE_10m,
    BSC_DG_AGG_GROUP_ALU32_PROFILE_11m,
    BSC_DG_AGG_GROUP_ALU32_PROFILE_12m,
    BSC_DG_AGG_GROUP_ALU32_PROFILE_13m,
    BSC_DG_AGG_GROUP_ALU32_PROFILE_14m,
    BSC_DG_AGG_GROUP_ALU32_PROFILE_15m,
    BSC_DG_AIFT_GROUP_TABLEm,
    BSC_DG_ALU16_0_EVENT_COUNTERm,
    BSC_DG_ALU16_1_EVENT_COUNTERm,
    BSC_DG_ALU16_2_EVENT_COUNTERm,
    BSC_DG_ALU16_3_EVENT_COUNTERm,
    BSC_DG_ALU16_4_EVENT_COUNTERm,
    BSC_DG_ALU16_5_EVENT_COUNTERm,
    BSC_DG_ALU16_6_EVENT_COUNTERm,
    BSC_DG_ALU16_7_EVENT_COUNTERm,
    BSC_DG_ALU32_0_EVENT_COUNTERm,
    BSC_DG_ALU32_10_EVENT_COUNTERm,
    BSC_DG_ALU32_11_EVENT_COUNTERm,
    BSC_DG_ALU32_1_EVENT_COUNTERm,
    BSC_DG_ALU32_2_EVENT_COUNTERm,
    BSC_DG_ALU32_3_EVENT_COUNTERm,
    BSC_DG_ALU32_4_EVENT_COUNTERm,
    BSC_DG_ALU32_5_EVENT_COUNTERm,
    BSC_DG_ALU32_6_EVENT_COUNTERm,
    BSC_DG_ALU32_7_EVENT_COUNTERm,
    BSC_DG_ALU32_8_EVENT_COUNTERm,
    BSC_DG_ALU32_9_EVENT_COUNTERm,
    BSC_DG_AMFT_GROUP_TABLEm,
    BSC_DG_COPY_TO_COLLECTOR_COUNTERm,
    BSC_DG_FLOW_METER_EXCEED_COUNTERm,
    BSC_DG_FLOW_OPAQUE_COUNTER_0m,
    BSC_DG_FLOW_OPAQUE_COUNTER_1m,
    BSC_DG_GROUP_ALU16_PROFILE_0m,
    BSC_DG_GROUP_ALU16_PROFILE_1m,
    BSC_DG_GROUP_ALU16_PROFILE_2m,
    BSC_DG_GROUP_ALU16_PROFILE_3m,
    BSC_DG_GROUP_ALU16_PROFILE_4m,
    BSC_DG_GROUP_ALU16_PROFILE_5m,
    BSC_DG_GROUP_ALU16_PROFILE_6m,
    BSC_DG_GROUP_ALU16_PROFILE_7m,
    BSC_DG_GROUP_ALU32_PROFILE_0m,
    BSC_DG_GROUP_ALU32_PROFILE_1m,
    BSC_DG_GROUP_ALU32_PROFILE_2m,
    BSC_DG_GROUP_ALU32_PROFILE_3m,
    BSC_DG_GROUP_ALU32_PROFILE_4m,
    BSC_DG_GROUP_ALU32_PROFILE_5m,
    BSC_DG_GROUP_ALU32_PROFILE_6m,
    BSC_DG_GROUP_ALU32_PROFILE_7m,
    BSC_DG_GROUP_ALU32_PROFILE_8m,
    BSC_DG_GROUP_ALU32_PROFILE_9m,
    BSC_DG_GROUP_ALU32_PROFILE_10m,
    BSC_DG_GROUP_ALU32_PROFILE_11m,
    BSC_DG_GROUP_COPY_TO_COLLECTOR_PROFILEm,
    BSC_DG_GROUP_LOAD16_PROFILEm,
    BSC_DG_GROUP_LOAD8_PROFILEm,
    BSC_DG_GROUP_METER_PROFILEm,
    BSC_DG_GROUP_TABLEm,
    BSC_DG_GROUP_TIMESTAMP_PROFILEm,
    BSC_DG_METER_EXCEED_COUNTERm,
    BSC_DG_OPAQUE_STATE_0_COUNTERm,
    BSC_DG_OPAQUE_STATE_1_COUNTERm,
    BSC_DT_AGG_ALU_MASK_PROFILEm,
    BSC_DT_AGG_DROP_MASK_1m,
    BSC_DT_AGG_DROP_MASK_2m,
    BSC_DT_ALU_MASK_PROFILEm,
    BSC_DT_DROP_MASK_1m,
    BSC_DT_DROP_MASK_2m,
    BSC_DT_EXPORT_PDE_PROFILE_0m,
    BSC_DT_EXPORT_PDE_PROFILE_1m,
    BSC_DT_EXPORT_PDE_PROFILE_2m,
    BSC_DT_FLEX_AFT_SESSION_DATA_BANK_0m,
    BSC_DT_FLEX_AFT_SESSION_DATA_BANK_1m,
    BSC_DT_FLEX_AFT_SESSION_DATA_BANK_2m,
    BSC_DT_FLEX_AFT_SESSION_DATA_BANK_3m,
    BSC_DT_FLEX_SESSION_DATA_DOUBLEm,
    BSC_DT_FLEX_SESSION_DATA_SINGLEm,
    BSC_DT_FLEX_STATE_TRANSITION_TABLEm,
    BSC_DT_KMAP_CONTROL_3m,
    BSC_DT_PDE_PROFILE_0m,
    BSC_DT_PDE_PROFILE_1m,
    BSC_DT_PDE_PROFILE_2m,
    BSC_EX_COLLECTOR_CONFIGm,
    BSC_EX_COLLECTOR_IPFIX_PACKET_BUILDm,
    BSC_EX_HDR_CONSTRUCT_CFGm,
    BSC_IPFIX_RA_COUNTER_1m,
    BSC_IPFIX_RA_STATS_0m,
    BSC_IPFIX_STATSm,
    BSC_KG_ACTIVE_COUNTERm,
    BSC_KG_AGE_OUT_COUNTERm,
    BSC_KG_AGE_OUT_PROFILEm,
    BSC_KG_EXCEED_COUNTERm,
    BSC_KG_FLOW_ACTIVE_COUNTERm,
    BSC_KG_FLOW_AGE_OUT_COUNTERm,
    BSC_KG_FLOW_EXCEED_COUNTERm,
    BSC_KG_FLOW_EXCEED_PROFILEm,
    BSC_KG_FLOW_LEARNED_COUNTERm,
    BSC_KG_FLOW_MISSED_COUNTERm,
    BSC_KG_GROUP_TABLEm,
    BSC_KG_LEARNED_COUNTERm,
    BSC_KG_LEARN_ELIGIBLE_COUNTERm,
    BSC_KG_LEARN_MISS_EXPORT_FIFO_FULL_COUNTERm,
    BSC_KG_MISSED_COUNTERm,
    BSC_KG_SESSION_TABLE_HIT_COUNTERm,
    BSC_RA_STATS_COUNTER_0m,
    BSC_TS_UTC_CONVERSIONm,
    BSK_AEFP_POLICYm,
    BSK_AEFP_TCAMm,
    BSK_ALU_DATA2_LTS_MUX_CTRL_PLUS_MASKm,
    BSK_ALU_DATA_LTS_MUX_CTRL_PLUS_MASKm,
    BSK_AMFP_POLICYm,
    BSK_AMFP_TCAMm,
    BSK_FTFP2_COMBO_TCAM_POLICYm,
    BSK_FTFP2_LTS_LOGICAL_TBL_SEL_SRAMm,
    BSK_FTFP2_LTS_LOGICAL_TBL_SEL_TCAMm,
    BSK_FTFP2_LTS_MASK_0m,
    BSK_FTFP2_LTS_MASK_1m,
    BSK_FTFP2_LTS_MUX_CTRL_0m,
    BSK_FTFP2_LTS_MUX_CTRL_1m,
    BSK_FTFP2_LTS_MUX_CTRL_2_PLUS_MASKm,
    BSK_FTFP2_POLICYm,
    BSK_FTFP2_TCAMm,
    BSK_FTFP_COMBO_TCAM_POLICYm,
    BSK_FTFP_LTS_LOGICAL_TBL_SEL_SRAMm,
    BSK_FTFP_LTS_LOGICAL_TBL_SEL_TCAMm,
    BSK_FTFP_LTS_MASK_0m,
    BSK_FTFP_LTS_MASK_1m,
    BSK_FTFP_LTS_MUX_CTRL_0m,
    BSK_FTFP_LTS_MUX_CTRL_1m,
    BSK_FTFP_LTS_MUX_CTRL_2_PLUS_MASKm,
    BSK_FTFP_POLICYm,
    BSK_FTFP_TCAMm,
    BSK_KMAP_CONTROL_0m,
    BSK_KMAP_CONTROL_1m,
    BSK_KMAP_CONTROL_2m,
    BSK_SESSION_DATA2_LTS_MASK_0m,
    BSK_SESSION_DATA2_LTS_MUX_CTRL_0m,
    BSK_SESSION_DATA2_LTS_MUX_CTRL_1_PLUS_MASKm,
    BSK_SESSION_DATA_LTS_MASK_0m,
    BSK_SESSION_DATA_LTS_MUX_CTRL_0m,
    BSK_SESSION_DATA_LTS_MUX_CTRL_1_PLUS_MASKm,
    BSK_SESSION_KEY_LTS_MASK_0m,
    BSK_SESSION_KEY_LTS_MASK_1m,
    BSK_SESSION_KEY_LTS_MUX_CTRL_0m,
    BSK_SESSION_KEY_LTS_MUX_CTRL_1m,
    BSK_SESSION_KEY_LTS_MUX_CTRL_2_PLUS_MASKm,
    BSD_POLICY_ACTION_PROFILE_0m,
    BSD_POLICY_ACTION_PROFILE_1m,
    BSD_POLICY_ACTION_PROFILE_2m,
    FT_KEY_SINGLEm,
    FT_KEY_DOUBLEm,
    FT_KEY_KEY_ATTRIBUTESm,
    FT_KEY_HASH_CONTROLm,
    INVALIDm /* Must be the last element in this structure */
};

/* flowtracker data structure */
techsupport_data_t techsupport_flowtracker_firebolt6_data = {
    techsupport_flowtracker_diag_cmdlist,
    techsupport_flowtracker_firebolt6_reg_list,
    techsupport_flowtracker_firebolt6_memory_table_list,
    techsupport_flowtracker_firebolt6_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_flowtracker_sw_dump_cmdlist,
    techsupport_flowtracker_firebolt6_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};
#endif
