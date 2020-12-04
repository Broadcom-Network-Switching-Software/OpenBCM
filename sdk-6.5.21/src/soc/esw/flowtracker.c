/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        flowtracker.c
 * Purpose:     soc functions for flowtracker module
 * Requires:
 */


#include <soc/defs.h>

#if defined(BCM_FLOWTRACKER_SUPPORT)

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/error.h>
#include <soc/esw/flowtracker.h>

#include <shared/bsl.h>

int _soc_flowtracker_mem_skip(int unit, soc_mem_t mem)
{
    if (!soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        switch (mem) {
            case BSC_AG_AGE_TABLEm:
            case BSC_DG_ALU16_0_EVENT_COUNTERm:
            case BSC_DG_ALU16_1_EVENT_COUNTERm:
            case BSC_DG_ALU16_2_EVENT_COUNTERm:
            case BSC_DG_ALU16_3_EVENT_COUNTERm:
            case BSC_DG_ALU16_4_EVENT_COUNTERm:
            case BSC_DG_ALU16_5_EVENT_COUNTERm:
            case BSC_DG_ALU16_6_EVENT_COUNTERm:
            case BSC_DG_ALU16_7_EVENT_COUNTERm:
            case BSC_DG_ALU32_0_EVENT_COUNTERm:
            case BSC_DG_ALU32_10_EVENT_COUNTERm:
            case BSC_DG_ALU32_11_EVENT_COUNTERm:
            case BSC_DG_ALU32_1_EVENT_COUNTERm:
            case BSC_DG_ALU32_2_EVENT_COUNTERm:
            case BSC_DG_ALU32_3_EVENT_COUNTERm:
            case BSC_DG_ALU32_4_EVENT_COUNTERm:
            case BSC_DG_ALU32_5_EVENT_COUNTERm:
            case BSC_DG_ALU32_6_EVENT_COUNTERm:
            case BSC_DG_ALU32_7_EVENT_COUNTERm:
            case BSC_DG_ALU32_8_EVENT_COUNTERm:
            case BSC_DG_ALU32_9_EVENT_COUNTERm:
            case BSC_DG_COPY_TO_COLLECTOR_COUNTERm:
            case BSC_DG_FLOW_METER_EXCEED_COUNTERm:
            case BSC_DG_FLOW_OPAQUE_COUNTER_0m:
            case BSC_DG_FLOW_OPAQUE_COUNTER_1m:
            case BSC_DG_GROUP_ALU16_PROFILE_0m:
            case BSC_DG_GROUP_ALU16_PROFILE_1m:
            case BSC_DG_GROUP_ALU16_PROFILE_2m:
            case BSC_DG_GROUP_ALU16_PROFILE_3m:
            case BSC_DG_GROUP_ALU16_PROFILE_4m:
            case BSC_DG_GROUP_ALU16_PROFILE_5m:
            case BSC_DG_GROUP_ALU16_PROFILE_6m:
            case BSC_DG_GROUP_ALU16_PROFILE_7m:
            case BSC_DG_GROUP_ALU32_PROFILE_0m:
            case BSC_DG_GROUP_ALU32_PROFILE_10m:
            case BSC_DG_GROUP_ALU32_PROFILE_11m:
            case BSC_DG_GROUP_ALU32_PROFILE_1m:
            case BSC_DG_GROUP_ALU32_PROFILE_2m:
            case BSC_DG_GROUP_ALU32_PROFILE_3m:
            case BSC_DG_GROUP_ALU32_PROFILE_4m:
            case BSC_DG_GROUP_ALU32_PROFILE_5m:
            case BSC_DG_GROUP_ALU32_PROFILE_6m:
            case BSC_DG_GROUP_ALU32_PROFILE_7m:
            case BSC_DG_GROUP_ALU32_PROFILE_8m:
            case BSC_DG_GROUP_ALU32_PROFILE_9m:
            case BSC_DG_GROUP_COPY_TO_COLLECTOR_PROFILEm:
            case BSC_DG_GROUP_LOAD16_PROFILEm:
            case BSC_DG_GROUP_LOAD8_PROFILEm:
            case BSC_DG_GROUP_METER_PROFILEm:
            case BSC_DG_GROUP_TABLEm:
            case BSC_DG_GROUP_TIMESTAMP_PROFILEm:
            case BSC_DG_METER_EXCEED_COUNTERm:
            case BSC_DG_OPAQUE_STATE_0_COUNTERm:
            case BSC_DG_OPAQUE_STATE_1_COUNTERm:
            case BSC_DT_ALU_MASK_PROFILEm:
            case BSC_DT_DROP_MASK_1m:
            case BSC_DT_DROP_MASK_2m:
            case BSC_DT_EXPORT_PDE_PROFILE_0m:
            case BSC_DT_EXPORT_PDE_PROFILE_1m:
            case BSC_DT_EXPORT_PDE_PROFILE_2m:
            case BSC_DT_FLEX_SESSION_DATA_DOUBLEm:
            case BSC_DT_FLEX_SESSION_DATA_SINGLEm:
            case BSC_DT_FLEX_STATE_TRANSITION_TABLEm:
            case BSC_DT_KMAP_CONTROL_3m:
            case BSC_DT_PDE_PROFILE_0m:
            case BSC_DT_PDE_PROFILE_1m:
            case BSC_DT_PDE_PROFILE_2m:
            case BSC_DT_SBUS_ACCESS_CONTROLm:
            case BSC_EX_COLLECTOR_CONFIGm:
            case BSC_EX_COLLECTOR_IPFIX_PACKET_BUILDm:
            case BSC_EX_EXPORT_FIFOm:
            case BSC_EX_HDR_CONSTRUCT_CFGm:
            case BSC_IPFIX_RA_COUNTER_1m:
            case BSC_KG_ACTIVE_COUNTERm:
            case BSC_KG_AGE_OUT_COUNTERm:
            case BSC_KG_AGE_OUT_PROFILEm:
            case BSC_KG_EXCEED_COUNTERm:
            case BSC_KG_FLOW_ACTIVE_COUNTERm:
            case BSC_KG_FLOW_AGE_OUT_COUNTERm:
            case BSC_KG_FLOW_EXCEED_COUNTERm:
            case BSC_KG_FLOW_EXCEED_PROFILEm:
            case BSC_KG_FLOW_LEARNED_COUNTERm:
            case BSC_KG_FLOW_MISSED_COUNTERm:
            case BSC_KG_GROUP_TABLEm:
            case BSC_KG_LEARNED_COUNTERm:
            case BSC_KG_LEARN_ELIGIBLE_COUNTERm:
            case BSC_KG_LEARN_MISS_EXPORT_FIFO_FULL_COUNTERm:
            case BSC_KG_MISSED_COUNTERm:
            case BSC_KG_SESSION_TABLE_HIT_COUNTERm:
            case BSC_KT_PURGE_BUFFERm:
            case BSC_RA_STATS_COUNTER_0m:
            case BSC_TS_UTC_CONVERSIONm:
            case BSD_FT_MASTER_1m:
            case BSD_FT_MASTER_2m:
            case BSD_FT_SLAVE_1m:
            case BSD_FT_SLAVE_2m:
            case BSD_POLICY_ACTION_PROFILE_0m:
            case BSD_POLICY_ACTION_PROFILE_1m:
            case BSD_POLICY_ACTION_PROFILE_2m:
            case BSK_ALU_DATA_LTS_MUX_CTRL_PLUS_MASK_PIPE0m:
            case BSK_ALU_DATA_LTS_MUX_CTRL_PLUS_MASKm:
            case BSK_FTFP_COMBO_TCAM_POLICYm:
            case BSK_FTFP_LTS_LOGICAL_TBL_SEL_SRAMm:
            case BSK_FTFP_LTS_LOGICAL_TBL_SEL_TCAMm:
            case BSK_FTFP_LTS_MASK_0_PIPE0m:
            case BSK_FTFP_LTS_MASK_0m:
            case BSK_FTFP_LTS_MASK_1_PIPE0m:
            case BSK_FTFP_LTS_MASK_1m:
            case BSK_FTFP_LTS_MUX_CTRL_0_PIPE0m:
            case BSK_FTFP_LTS_MUX_CTRL_0m:
            case BSK_FTFP_LTS_MUX_CTRL_1_PIPE0m:
            case BSK_FTFP_LTS_MUX_CTRL_1m:
            case BSK_FTFP_LTS_MUX_CTRL_2_PLUS_MASK_PIPE0m:
            case BSK_FTFP_LTS_MUX_CTRL_2_PLUS_MASKm:
            case BSK_FTFP_POLICYm:
            case BSK_FTFP_TCAMm:
            case BSK_KMAP_CONTROL_0m:
            case BSK_KMAP_CONTROL_1m:
            case BSK_KMAP_CONTROL_2m:
            case BSK_SESSION_DATA_LTS_MASK_0_PIPE0m:
            case BSK_SESSION_DATA_LTS_MASK_0m:
            case BSK_SESSION_DATA_LTS_MUX_CTRL_0_PIPE0m:
            case BSK_SESSION_DATA_LTS_MUX_CTRL_0m:
            case BSK_SESSION_DATA_LTS_MUX_CTRL_1_PLUS_MASK_PIPE0m:
            case BSK_SESSION_DATA_LTS_MUX_CTRL_1_PLUS_MASKm:
            case BSK_SESSION_KEY_LTS_MASK_0_PIPE0m:
            case BSK_SESSION_KEY_LTS_MASK_0m:
            case BSK_SESSION_KEY_LTS_MASK_1_PIPE0m:
            case BSK_SESSION_KEY_LTS_MASK_1m:
            case BSK_SESSION_KEY_LTS_MUX_CTRL_0_PIPE0m:
            case BSK_SESSION_KEY_LTS_MUX_CTRL_0m:
            case BSK_SESSION_KEY_LTS_MUX_CTRL_1_PIPE0m:
            case BSK_SESSION_KEY_LTS_MUX_CTRL_1m:
            case BSK_SESSION_KEY_LTS_MUX_CTRL_2_PLUS_MASK_PIPE0m:
            case BSK_SESSION_KEY_LTS_MUX_CTRL_2_PLUS_MASKm:
            case DO_NOT_FT_ENTRYm:
            case FT_KEY_ACTION_TABLE_Am:
            case FT_KEY_ACTION_TABLE_Bm:
            case FT_KEY_DOUBLE_PIPE0m:
            case FT_KEY_DOUBLEm:
            case FT_KEY_ECCm:
            case FT_KEY_REMAP_TABLE_Am:
            case FT_KEY_REMAP_TABLE_Bm:
            case FT_KEY_SINGLEm:
            case FT_KEY_HASH_CONTROLm:
            case KT_FCC_MASTERm:
                return TRUE;
            default:
                break;
        }
    }

    if (!soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        switch(mem) {
            case BSK_AEFP_POLICYm:
            case BSK_AEFP_TCAMm:
            case BSK_ALU_DATA2_LTS_MUX_CTRL_PLUS_MASKm:
            case BSK_ALU_DATA2_LTS_MUX_CTRL_PLUS_MASK_PIPE0m:
            case BSK_AMFP_POLICYm:
            case BSK_AMFP_TCAMm:
            case BSK_FTFP2_COMBO_TCAM_POLICYm:
            case BSK_FTFP2_LTS_LOGICAL_TBL_SEL_SRAMm:
            case BSK_FTFP2_LTS_LOGICAL_TBL_SEL_TCAMm:
            case BSK_FTFP2_LTS_MASK_0m:
            case BSK_FTFP2_LTS_MASK_1m:
            case BSK_FTFP2_LTS_MASK_0_PIPE0m:
            case BSK_FTFP2_LTS_MASK_1_PIPE0m:
            case BSK_FTFP2_LTS_MUX_CTRL_0m:
            case BSK_FTFP2_LTS_MUX_CTRL_1m:
            case BSK_FTFP2_LTS_MUX_CTRL_0_PIPE0m:
            case BSK_FTFP2_LTS_MUX_CTRL_1_PIPE0m:
            case BSK_FTFP2_LTS_MUX_CTRL_2_PLUS_MASKm:
            case BSK_FTFP2_LTS_MUX_CTRL_2_PLUS_MASK_PIPE0m:
            case BSK_FTFP2_POLICYm:
            case BSK_FTFP2_TCAMm:
            case BSK_HS_SER_FIFOm:
            case BSK_SESSION_DATA2_LTS_MASK_0m:
            case BSK_SESSION_DATA2_LTS_MASK_0_PIPE0m:
            case BSK_SESSION_DATA2_LTS_MUX_CTRL_0m:
            case BSK_SESSION_DATA2_LTS_MUX_CTRL_0_PIPE0m:
            case BSK_SESSION_DATA2_LTS_MUX_CTRL_1_PLUS_MASKm:
            case BSK_SESSION_DATA2_LTS_MUX_CTRL_1_PLUS_MASK_PIPE0m:
            case AMFT_CLASSID_PROFILEm:
            case AEFT_CLASSID_PROFILEm:
            case BSC_DG_AIFT_GROUP_TABLEm:
            case BSC_DG_AMFT_GROUP_TABLEm:
            case BSC_DG_AEFT_GROUP_TABLEm:
            case BSC_DG_AGG_GROUP_ALU32_PROFILE_0m:
            case BSC_DG_AGG_GROUP_ALU32_PROFILE_1m:
            case BSC_DG_AGG_GROUP_ALU32_PROFILE_2m:
            case BSC_DG_AGG_GROUP_ALU32_PROFILE_3m:
            case BSC_DG_AGG_GROUP_ALU32_PROFILE_4m:
            case BSC_DG_AGG_GROUP_ALU32_PROFILE_5m:
            case BSC_DG_AGG_GROUP_ALU32_PROFILE_6m:
            case BSC_DG_AGG_GROUP_ALU32_PROFILE_7m:
            case BSC_DG_AGG_GROUP_ALU32_PROFILE_8m:
            case BSC_DG_AGG_GROUP_ALU32_PROFILE_9m:
            case BSC_DG_AGG_GROUP_ALU32_PROFILE_10m:
            case BSC_DG_AGG_GROUP_ALU32_PROFILE_11m:
            case BSC_DG_AGG_GROUP_ALU32_PROFILE_12m:
            case BSC_DG_AGG_GROUP_ALU32_PROFILE_13m:
            case BSC_DG_AGG_GROUP_ALU32_PROFILE_14m:
            case BSC_DG_AGG_GROUP_ALU32_PROFILE_15m:
            case BSC_DT_FLEX_AFT_SESSION_DATA_BANK_0m:
            case BSC_DT_FLEX_AFT_SESSION_DATA_BANK_1m:
            case BSC_DT_FLEX_AFT_SESSION_DATA_BANK_2m:
            case BSC_DT_FLEX_AFT_SESSION_DATA_BANK_3m:
            case BSC_AG_AIGID_CONFIG_TABLEm:
            case BSC_AGG_ECC_CORRUPT_CONTROLm:
            case BSC_DG_ECC_CORRUPT_CONTROLm:
            case FT_KEY_KEY_ATTRIBUTESm:
            case BSC_DT_AGG_DROP_MASK_2m:
            case BSC_DT_AGG_DROP_MASK_1m:
            case BSC_DT_AGG_ALU_MASK_PROFILEm:

                return TRUE;
            default:
                break;
        }
    }
    return FALSE;
}

int _soc_flowtracker_reg_skip(int unit, soc_reg_t reg)
{
    if (!soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        switch (reg) {
            case BSC_AG_DEBUG_REGISTERr:
            case BSC_AG_GROUP_FLUSH_CONFIG_64r:
            case BSC_AG_LOSSLESS_PERIODIC_EXPORT_STATUSr:
            case BSC_AG_PERIODIC_EXPORTr:
            case BSC_AG_SCAN_ADDR_RANGE_CONTROLr:
            case BSC_AG_SCAN_CONTROLr:
            case BSC_AG_SCAN_PERIOD_SELECTr:
            case BSC_AG_SER_CONTROLr:
            case BSC_DG_SER_CONTROLr:
            case BSC_DG_SER_CONTROL_1r:
            case BSC_DG_SER_CONTROL_2r:
            case BSC_DT_ALU16_TO_INSTANCE_MAPPING_CONTROLr:
            case BSC_DT_ALU32_TO_INSTANCE_MAPPING_CONTROLr:
            case BSC_DT_BANK_TO_INSTANCE_MAPPING_CONTROLr:
            case BSC_DT_DPR_MODE_CONTROLr:
            case BSC_DT_EXPORT_BANK_VALID_BITMAPr:
            case BSC_DT_LOAD16_TO_INSTANCE_MAPPING_CONTROLr:
            case BSC_DT_LOAD8_TO_INSTANCE_MAPPING_CONTROLr:
            case BSC_DT_RANDOM_SEED_CONFIGr:
            case BSC_DT_SER_CONTROLr:
            case BSC_DT_AGG_SER_CONTROLr:
            case BSC_DT_TIMESTAMP_ENGINE_TO_INSTANCE_MAPPING_CONTROLr:
            case BSC_EX_BROADSCAN_GLOBAL_SER_DROP_ENABLEr:
            case BSC_EX_BROADSCAN_IPFIX_CREDIT_RESETr:
            case BSC_EX_BROADSCAN_RA_FT_ENABLEr:
            case BSC_EX_EXPORT_CONTROLr:
            case BSC_EX_EXPORT_SET_LENGTHr:
            case BSC_EX_GLOBAL_EVENT_VECTOR_CONTROL_ON_SERr:
            case BSC_EX_SEQ_NUMr:
            case BSC_EX_SER_CONTROLr:
            case BSC_KG_SER_CONTROLr:
            case BSC_KT_KEY_SER_CONTROLr:
            case BSC_KT_SW_INSERT_CTRL_INFOr:
            case IPFIX_TOD_SELECT_0r:
            case IPFIX_TOD_SELECT_1r:
            case IPFIX_TOD_SELECT_2r:
            case IPFIX_TOD_SELECT_3r:
            case IPFIX_TOD_SELECT_4r:
            case IPFIX_TOD_SELECT_5r:
            case IPFIX_TOD_SELECT_6r:
            case IPFIX_TOD_SELECT_7r:
            case BSK_FTFP_LTS_A_KEY_GEN_CAM_BIST_CONFIG_64r:
            case BSK_FTFP_CAM_BIST_CONFIG_64r:
                return TRUE;
            default:
                break;
        }
    }

    if (!soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        switch(reg) {
            case BSC_DT_BANK_TO_AGG_INSTANCE_MAPPING_CONTROLr:
            case BSC_AG_AFT_PERIODIC_EXPORTr:
            case BSK_FTFP2_CAM_BIST_CONFIG_64r:
            case BSK_AMFP_CAM_BIST_CONFIG_64r:
            case BSK_AEFP_CAM_BIST_CONFIG_64r:

                return TRUE;
            default:
                break;
        }
    }

    return FALSE;
}

/*
 * Function:
 *     _soc_flowtracker_counters_enable
 * Purpose:
 *      Enable Flowtracker related counters if feature supported
 * Parameters:
 *      unit    - (IN) Unit number.
 * Returns:
 *       NONE.
 */
int _soc_flowtracker_counters_enable(int unit)
{
    soc_control_t *soc = NULL;
    soc_counter_non_dma_t *non_dma = NULL;

    if (!soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        return SOC_E_NONE;
    }

    soc = SOC_CONTROL(unit);
    if (soc == NULL) {
        return SOC_E_INIT;
    }

    /* GROUP_FLOW_EXCEED_COUNT */
    non_dma = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_NUM_EXCEEDED_CNT -
        SOC_COUNTER_NON_DMA_START];
    non_dma->flags |= _SOC_COUNTER_NON_DMA_VALID;


    /* GROUP_FLOW_LEARNED_COUNT */
    non_dma = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_LEARNT_CNT -
        SOC_COUNTER_NON_DMA_START];
    non_dma->flags |= _SOC_COUNTER_NON_DMA_VALID;

    /* GROUP_AGE_OUT_COUNT */
    non_dma = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_AGE_OUT_CNT -
        SOC_COUNTER_NON_DMA_START];
    non_dma->flags |= _SOC_COUNTER_NON_DMA_VALID;

    /* GROUP_FLOW_MISSED_COUNT */
    non_dma = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_MISSED_CNT -
        SOC_COUNTER_NON_DMA_START];
    non_dma->flags |= _SOC_COUNTER_NON_DMA_VALID;

    /* GROUP_METER_EXCEED_COUNT */
    non_dma = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_METER_EXCEEDED_CNT -
        SOC_COUNTER_NON_DMA_START];
    non_dma->flags |= _SOC_COUNTER_NON_DMA_VALID;

    /* GROUP_FLOW_START_COUNT */
    non_dma = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_START_CNT -
        SOC_COUNTER_NON_DMA_START];
    non_dma->flags |= _SOC_COUNTER_NON_DMA_VALID;

    /* GROUP_FLOW_END_COUNT */
    non_dma = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_END_CNT -
        SOC_COUNTER_NON_DMA_START];
    non_dma->flags |= _SOC_COUNTER_NON_DMA_VALID;

    return SOC_E_NONE;
}


#endif /* BCM_FLOWTRACKER_SUPPORT */
