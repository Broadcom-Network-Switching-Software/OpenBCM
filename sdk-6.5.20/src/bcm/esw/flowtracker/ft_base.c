/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ft_base.c
 * Purpose:     The purpose of this file is to set flow
 *              tracker base methods.
 * Requires:
 */

#include <soc/esw/flowtracker.h>
#include <bcm_int/esw/flowtracker/ft_group.h>
#include <bcm_int/esw/flowtracker/ft_export.h>
#include <bcm_int/esw/flowtracker/ft_user.h>
#include <bcm_int/esw/flowtracker/ft_drop.h>
#include <bcm_int/esw/flowtracker/ft_class.h>
#include <bcm_int/esw/flowtracker/ft_udf.h>
#include <bcm_int/esw/switch.h>

#if defined(BCM_FLOWTRACKER_SUPPORT)

#ifdef BCM_WARM_BOOT_SUPPORT
/* For 6.5.20 Added sync/recv for agg drop reason group, ts_info per group,
   direction in tracking param/ flowchecks, aggregate periodic interval,
   Udf tracking params */
#define BCM_WB_VERSION_1_3                SOC_SCACHE_VERSION(1,3)
#define BCM_WB_VERSION_1_2                SOC_SCACHE_VERSION(1,2)
#define BCM_WB_VERSION_1_1                SOC_SCACHE_VERSION(1,1)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_3

/* Base FT state - groups, alus, etc..  */
#define BCM_WB_FT_PART_BASE    0
/* Export related FT state  */
#define BCM_WB_FT_PART_EXPORT  1
/* User entry related FT state  */
#define BCM_WB_FT_PART_USER    2

#endif /* BCM_WARM_BOOT_SUPPORT */

uint8 alu_type_str[6][50] =
    {{"AluLoadTypeLoad8"},
     {"AluLoadTypeLoad16"},
     {"AluLoadTypeAlu16"},
     {"AluLoadTypeAlu32"},
     {"AluLoadTypeAggAlu32"},
     {"AluLoadTypeNone"}};

int ft_initialized[BCM_MAX_NUM_UNITS] = {0};

/* Global chip debug state. */
bcmi_ft_chip_debug_t chip_debug_info[BCM_MAX_NUM_UNITS];

/* Database for TCP state transition */
bcmi_ft_state_transition_t state_transition_data[] = {
    /* current, tcp_flags[2], bidir, new_learn, next, ts_trig, event_flags */
    /* TCP. Closed/Listen to SYN_Wait on SYN (Bidirectional) */
    {0, {_bcmFieldFtAluDataTcpFlagsSYN, -1}, 1, -1, 1, 0x0,
        BCM_FLOWTRACKER_STATE_TRANSITION_TCP_CLOSED_SYN},
    /* TCP. Closed/Listen to SYN_Wait on SYN,ACK (Bidirectional) */
    {0, {_bcmFieldFtAluDataTcpFlagsSYN, _bcmFieldFtAluDataTcpFlagsACK},
        1, -1, 1, 0x0,
        BCM_FLOWTRACKER_STATE_TRANSITION_TCP_CLOSED_SYNACK},
    /* TCP. SYN_Wait to Established on ACK (bidirectional) */
    {1, {_bcmFieldFtAluDataTcpFlagsACK, -1}, 1, -1, 2, 0x1,
        BCM_FLOWTRACKER_STATE_TRANSITION_TCP_SYNWAIT_ACK},
    /* TCP. SYN_Wait to Closed/Listen on FIN (birectional) */
    {1, {_bcmFieldFtAluDataTcpFlagsFIN, -1}, 1, -1, 0, 0x0,
        BCM_FLOWTRACKER_STATE_TRANSITION_TCP_SYNWAIT_FIN},
    /* TCP. SYN_Wait to Closed/Listen on RST (bidirectional) */
    {1, {_bcmFieldFtAluDataTcpFlagsRST, -1}, 1, -1, 0, 0x0,
        BCM_FLOWTRACKER_STATE_TRANSITION_TCP_SYNWAIT_RST},
    /* TCP. Established to Closed/Listen on FIN (bidirectional */
    {2, {_bcmFieldFtAluDataTcpFlagsFIN, -1}, 1, -1, 0, 0x2,
        BCM_FLOWTRACKER_STATE_TRANSITION_TCP_ESTABLISHED_FIN},
    /* TCP. Established to Closed/Listen on RST (bidirectional) */
    {2, {_bcmFieldFtAluDataTcpFlagsRST, -1}, 1, -1, 0, 0x2,
        BCM_FLOWTRACKER_STATE_TRANSITION_TCP_ESTABLISHED_RST},
    /* UDP. Connection Established. */
    {0, {-1, -1}, -1, 1, 1, 0x1, 0},
    /* UDP. Connection Established. */
    {1, {-1, -1}, -1, 1, 1, 0x1, 0},
    /* Last entry to mark the end. */
    {-1, {-1, -1}, -1, -1, -1, -1}
};

/*
 * Function:
 *   bcmi_ft_chip_debug_init
 * Purpose:
 *   Initialize Chip Debug state for flowtracker.
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_chip_debug_init(int unit)
{
    BCMI_FT_CHIP_NUM_DEBUG_INFO(unit) = 0;

    if (BCMI_FT_CHIP_DEBUG_PARAM_INFO(unit) != NULL) {
        sal_free(BCMI_FT_CHIP_DEBUG_PARAM_INFO(unit));
        BCMI_FT_CHIP_DEBUG_PARAM_INFO(unit) = NULL;
    }

    sal_memset(BCMI_FT_CHIP_DEBUG_INFO(unit).alu32_mem_used, 0,
        BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32) * sizeof(int));

    sal_memset(BCMI_FT_CHIP_DEBUG_INFO(unit).alu16_mem_used, 0,
        BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16) * sizeof(int));

    sal_memset(BCMI_FT_CHIP_DEBUG_INFO(unit).alu32_ref_count, 0,
        BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32) * sizeof(int));

    sal_memset(BCMI_FT_CHIP_DEBUG_INFO(unit).alu16_ref_count, 0,
        BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16) * sizeof(int));

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_flow_transition_state_init
 * Purpose:
 *   Initialize State transition state for flowtracker.
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_flow_transition_state_init(int unit)
{
    soc_mem_t mem;
    uint32 data = 0;
    uint32 state_transfmt[1];
    uint16 alu_data_mask = 0x0;
    uint32 cur_state_mask = 0xFF;
    uint32 state_trans_maskfmt[1];
    int offset, i, j;
    bsc_dt_flex_state_transition_table_entry_t table_entry;

    int tcp_control_mask[] = {
        _bcmFieldFtAluDataTcpFlagsSYN,
        _bcmFieldFtAluDataTcpFlagsACK,
        _bcmFieldFtAluDataTcpFlagsRST,
        _bcmFieldFtAluDataTcpFlagsFIN
    };

    sal_memset(state_transfmt, 0, sizeof(uint32));
    sal_memset(state_trans_maskfmt, 0, sizeof(uint32));
    mem = BSC_DT_FLEX_STATE_TRANSITION_TABLEm;

    for (i = 0; i < COUNTOF(tcp_control_mask); i++) {
        BCM_IF_ERROR_RETURN(_bcm_field_ft_alu_data_config_get(unit,
                    -1, tcp_control_mask[i], &offset, NULL));
        alu_data_mask |= (1 << offset);
    }

    i = 0;
    while(state_transition_data[i].current_state != -1) {

        sal_memset(state_transfmt, 0, sizeof(uint32));
        sal_memset(state_trans_maskfmt, 0, sizeof(uint32));

        sal_memset(&table_entry, 0,
                sizeof(bsc_dt_flex_state_transition_table_entry_t));

        /* Set Current State */
        data = state_transition_data[i].current_state;
        soc_format_field32_set(unit, BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
                state_transfmt, CURRENT_STATEf, data);
        soc_format_field32_set(unit, BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
                state_trans_maskfmt, CURRENT_STATEf, cur_state_mask);

        data = 0;
        for (j = 0; j < COUNTOF(state_transition_data[i].tcp_flags); j++) {

            if (state_transition_data[i].tcp_flags[j] != -1) {

                BCM_IF_ERROR_RETURN(_bcm_field_ft_alu_data_config_get(unit, -1,
                            state_transition_data[i].tcp_flags[j], &offset, NULL));
                data |= (1 << offset);
            }
        }

        /* Set ALU Data */
        soc_format_field32_set(unit, BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
                state_transfmt, ALU_DATAf, data);
        soc_format_field32_set(unit, BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
                state_trans_maskfmt, ALU_DATAf, alu_data_mask);

        /* Set Bidirectional */
        if (state_transition_data[i].bidirectional != -1) {
            data = state_transition_data[i].bidirectional;
            soc_format_field32_set(unit, BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
                    state_transfmt, BIDIRECTIONAL_FLOWf, data);
            soc_format_field32_set(unit, BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
                    state_trans_maskfmt, BIDIRECTIONAL_FLOWf, 1);
        }

        /* Set New learn */
        if (state_transition_data[i].new_learn != -1) {
            data = state_transition_data[i].new_learn;
            soc_format_field32_set(unit, BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
                    state_transfmt, NEW_FLOW_LEARNEDf, data);
            soc_format_field32_set(unit, BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
                    state_trans_maskfmt, NEW_FLOW_LEARNEDf, 1);
        }

        /* Set Key/Mask */
        soc_mem_field_set(unit, mem, (uint32 *)&table_entry, KEYf,
                state_transfmt);
        soc_mem_field_set(unit, mem, (uint32 *)&table_entry, MASKf,
                state_trans_maskfmt);

        if (state_transition_data[i].next_state != -1) {
            data = state_transition_data[i].next_state;
            soc_mem_field32_set(unit, mem, (uint32 *)&table_entry, NEXT_STATEf,
                    data);
        }

        /* Flow Start Timestamp trigger */
        if (state_transition_data[i].ts_triggers & 0x1) {
            soc_mem_field32_set(unit, mem, (uint32 *)&table_entry,
                    OPAQUE_TS_0_TRIGGERf, 1);
        }

        /* Flow Start Counter trigger */
        if (state_transition_data[i].ts_triggers & 0x1) {
            soc_mem_field32_set(unit, mem, (uint32 *)&table_entry,
                    OPAQUE_CTR_0_TRIGGERf, 1);
        }

        /* Flow End Timestamp trigger */
        if (state_transition_data[i].ts_triggers & 0x2) {
            soc_mem_field32_set(unit, mem, (uint32 *)&table_entry,
                    OPAQUE_TS_1_TRIGGERf, 1);
        }

        /* Flow End Counter trigger */
        if (state_transition_data[i].ts_triggers & 0x2) {
            soc_mem_field32_set(unit, mem, (uint32 *)&table_entry,
                    OPAQUE_CTR_1_TRIGGERf, 1);
        }

        /* set the valid bit for this entry. */
        soc_mem_field32_set(unit, mem, (uint32 *)&table_entry, VALIDf, 1);

        /* you may also want to set the export enable here. */
        BCM_IF_ERROR_RETURN(WRITE_BSC_DT_FLEX_STATE_TRANSITION_TABLEm(unit,
                    MEM_BLOCK_ANY, i, &table_entry));

        i++;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_flow_direction_init
 * Purpose:
 *   Initialize Flow direction state for flowtracker.
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_flow_direction_init(int unit)
{
    int syn_offset, ack_offset;
    bsk_kmap_control_0_entry_t kmap0;
    bsk_kmap_control_1_entry_t kmap1;
    bsc_dt_kmap_control_3_entry_t kmap3;
    soc_mem_t mem;

    sal_memset(&kmap0, 0, sizeof(bsk_kmap_control_0_entry_t));
    sal_memset(&kmap1, 0, sizeof(bsk_kmap_control_1_entry_t));
    sal_memset(&kmap3, 0, sizeof(bsc_dt_kmap_control_3_entry_t));

    /*
     * Replicating Arch settings.
     * KMAP for Key Swapped
     * From BSK_KMAP_0_KEY_SWAPPED_IN_BUS format, extract and form 4bit value
     * bit0 = FTFP_POLICY_OPAQUE_0 = bidirectional Flow
     * bit1 = SRC_GT_DST_RESULT = src < dst
     * bit2 = FTFP_POLICY_OPAQUE_0 = bidirectional Flow (dummy)
     * bit3 = SRC_GT_DST_RESULT = src < dst (dummy)
     * Key_swapped = 1, for 1111
     */
    mem = BSK_KMAP_CONTROL_0m;
    soc_mem_field32_set(unit, mem, (uint32 *)&kmap0, KMAP_INPUT_SELECT_0f, 4);
    soc_mem_field32_set(unit, mem, (uint32 *)&kmap0, KMAP_INPUT_SELECT_1f, 7);
    soc_mem_field32_set(unit, mem, (uint32 *)&kmap0, KMAP_INPUT_SELECT_2f, 4);
    soc_mem_field32_set(unit, mem, (uint32 *)&kmap0, KMAP_INPUT_SELECT_3f, 7);

    soc_mem_field32_set(unit, mem, (uint32 *)&kmap0, KMAP_CONTROLf, 0x8000);

    /* Write entry into memory. */
    BCM_IF_ERROR_RETURN(WRITE_BSK_KMAP_CONTROL_0m(unit,
        MEM_BLOCK_ANY, 0, &kmap0));

    /*
     * KMAP for Key Direction
     * From BSK_KMAP_1_KEY_DIRECTION_IN_BUS format, extract and form 4bit value
     * b0 = bidirectional Flow
     * b1 = Key Swapped
     * b2 = SYN
     * b3 = SYN-ACK
     * Key_direction = 1, for 1011 & 0101
     */

    BCM_IF_ERROR_RETURN(_bcm_field_ft_alu_data_config_get(unit, -1,
        _bcmFieldFtAluDataTcpFlagsSYN, &syn_offset, NULL));

    BCM_IF_ERROR_RETURN(_bcm_field_ft_alu_data_config_get(unit, -1,
        _bcmFieldFtAluDataTcpFlagsACK, &ack_offset, NULL));


    mem = BSK_KMAP_CONTROL_1m;
    soc_mem_field32_set(unit, mem, (uint32 *)&kmap1, KMAP_INPUT_SELECT_0f, 4);
    soc_mem_field32_set(unit, mem, (uint32 *)&kmap1, KMAP_INPUT_SELECT_1f, 7);
    soc_mem_field32_set(unit, mem, (uint32 *)&kmap1, KMAP_INPUT_SELECT_2f,
        syn_offset);
    soc_mem_field32_set(unit, mem, (uint32 *)&kmap1, KMAP_INPUT_SELECT_3f,
        ack_offset);

    soc_mem_field32_set(unit, mem, (uint32 *)&kmap1, KMAP_CONTROLf, 0x820);

    /* Write entry into memory. */
    BCM_IF_ERROR_RETURN(WRITE_BSK_KMAP_CONTROL_1m(unit,
        MEM_BLOCK_ANY, 0, &kmap1));

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3) &&
            soc_feature(unit, soc_feature_flowtracker_ver_1_bidirectional)) {
        /*
         * KMAP for Key Direction for UDP flows
         * From BSK_KMAP_1_KEY_DIRECTION_IN_BUS format, extract and form 4bit value
         * b0 = bidirectional Flow
         * b1 = Key Swapped
         * b2 = FTFP Policy Opaque 1
         * b3 = FTFP Policy Opaque 1
         * Key_direction = 1, for 1101 & 0011
         */

        mem = BSK_KMAP_CONTROL_1m;
        soc_mem_field32_set(unit, mem, (uint32 *)&kmap1, KMAP_INPUT_SELECT_0f, 4);
        soc_mem_field32_set(unit, mem, (uint32 *)&kmap1, KMAP_INPUT_SELECT_1f, 7);
        soc_mem_field32_set(unit, mem, (uint32 *)&kmap1, KMAP_INPUT_SELECT_2f, 5);
        soc_mem_field32_set(unit, mem, (uint32 *)&kmap1, KMAP_INPUT_SELECT_3f, 5);

        soc_mem_field32_set(unit, mem, (uint32 *)&kmap1, KMAP_CONTROLf, 0x2008);

        /* Write entry into memory. */
        BCM_IF_ERROR_RETURN(WRITE_BSK_KMAP_CONTROL_1m(unit,
                    MEM_BLOCK_ANY, 1, &kmap1));
    }
#endif

    /*
     * KMAP for Flow Direction
     * From BSC_DT_KMAP_3_FLOW_DIRECTION_IN_BUS format,
     * extract and form 4bit value
     * b0 = bidirectional Flow
     * b1 = Key Direction
     * b2 = Key Swapped
     * b3 = Key Swapped (dummy)
     * Flow direction = 1, for 0011 & 1101
     */
    mem = BSC_DT_KMAP_CONTROL_3m;
    soc_mem_field32_set(unit, mem, (uint32 *)&kmap3, KMAP_INPUT_SELECT_0f, 4);
    soc_mem_field32_set(unit, mem, (uint32 *)&kmap3, KMAP_INPUT_SELECT_1f, 7);
    soc_mem_field32_set(unit, mem, (uint32 *)&kmap3, KMAP_INPUT_SELECT_2f, 6);
    soc_mem_field32_set(unit, mem, (uint32 *)&kmap3, KMAP_INPUT_SELECT_3f, 6);

    soc_mem_field32_set(unit, mem, (uint32 *)&kmap3, KMAP_CONTROLf, 0x2008);

    /* Write entry into memory. */
    BCM_IF_ERROR_RETURN(WRITE_BSC_DT_KMAP_CONTROL_3m(unit,
                                MEM_BLOCK_ANY, 0, &kmap3));

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_init
 * Purpose:
 *   Initialize Flowtracker state.
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   None.
 */
int
bcmi_ft_init(int unit)
{
    int result = BCM_E_NONE;
    ft_key_key_attributes_entry_t ft_key_attributes;

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    bsc_dg_ecc_corrupt_control_entry_t ft_ts_u16b_entry;
#endif


    if(!SOC_WARM_BOOT(unit)) {
        bcmi_ft_cleanup(unit);
    }

    /* Enable counters */
    result = _soc_flowtracker_counters_enable(unit);
    if (BCM_FAILURE(result)) {
        goto cleanup;
    }

    /* First initiate the group state. */
    result = bcmi_ft_group_state_init(unit);
    if (BCM_FAILURE(result)) {
        goto cleanup;
    }

    result = bcmi_ft_export_init(unit);
    if (BCM_FAILURE(result)) {
        goto cleanup;
    }

    result = bcmi_ft_group_profile_init(unit);
    if (BCM_FAILURE(result)) {
        goto cleanup;
    }

    result = bcmi_ft_alu_hash_init(unit);
    if (BCM_FAILURE(result)) {
        goto cleanup;
    }

    result = bcmi_ft_alu_load_init(unit);
    if (BCM_FAILURE(result)) {
        goto cleanup;
    }

    result = bcmi_ft_flowchecker_init(unit);
    if (BCM_FAILURE(result)) {
        goto cleanup;
    }

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        /* IPIPE supports DPR (HW feature which enables pipelines to process
         * an SOP cell and previous pkt EOP cell in same clock cycle), this
         * register should be left untouched while testing with HW, since HW
         * internally make sure Ipipe does not receive cell with both SOP=1
         * and EOP=1. Should be set to 0 only in BCMSIM otherwise first packet
         * of flow will not be considered for session data update.
         * By default this register is set to 1. */
        if (SOC_IS_HURRICANE4(unit) && (SAL_BOOT_XGSSIM || SAL_BOOT_BCMSIM)) {
            WRITE_BSC_DT_DPR_MODE_CONTROLr(unit, 0);
        }
        result = bcmi_ftv2_drop_reason_group_init(unit);
        if (BCM_FAILURE(result)) {
            goto cleanup;
        }
    }
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */

    result = bcmi_ft_flow_transition_state_init(unit);
    if (BCM_FAILURE(result)) {
        goto cleanup;
    }

    result = bcmi_ft_flow_direction_init(unit);
    if (BCM_FAILURE(result)) {
        goto cleanup;
    }

    result = bcmi_ft_chip_debug_init(unit);
    if (BCM_FAILURE(result)) {
        goto cleanup;
    }

    result = bcmi_ft_user_init(unit);
    if (BCM_FAILURE(result)) {
        goto cleanup;
    }

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        result = bcmi_ftv3_aggregate_class_init(unit);
        if (BCM_FAILURE(result)) {
            goto cleanup;
        }
    }
#endif

    result = bcmi_ft_udf_tracking_param_info_init(unit);
    if (BCM_FAILURE(result)) {
        goto cleanup;
    }

    sal_memset(&ft_key_attributes, 0, sizeof(ft_key_key_attributes_entry_t));

    /* Key width for single entry in nibles. */
    soc_mem_field32_set(unit, FT_KEY_KEY_ATTRIBUTESm, &ft_key_attributes,
        KEY_WIDTHf, 46);

    result = soc_mem_write(unit, FT_KEY_KEY_ATTRIBUTESm, MEM_BLOCK_ALL, 0,
        &ft_key_attributes);

    if (BCM_FAILURE(result)) {
        goto cleanup;
    }

    /* Set for double entries also. */
    soc_mem_field32_set(unit, FT_KEY_KEY_ATTRIBUTESm, &ft_key_attributes,
        KEY_BASE_WIDTHf, 0x1);

    result = soc_mem_write(unit, FT_KEY_KEY_ATTRIBUTESm, MEM_BLOCK_ALL, 1,
        &ft_key_attributes);

    if (BCM_FAILURE(result)) {
        goto cleanup;
    }

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3) &&
            soc_feature(unit, soc_feature_flowtracker_ts_ntp64_support)) {

        sal_memset(&ft_ts_u16b_entry, 0, sizeof(ft_ts_u16b_entry));
        soc_mem_read(unit, BSC_DG_ECC_CORRUPT_CONTROLm, MEM_BLOCK_ALL,
                                                    0, &ft_ts_u16b_entry);
        soc_mem_field32_set(unit,
                        BSC_DG_ECC_CORRUPT_CONTROLm,
                        (void *) &ft_ts_u16b_entry,
                        ENABLE_TIMESTAMP_NTP_TOD_UB_LOADf, 1);

        result = soc_mem_write(unit, BSC_DG_ECC_CORRUPT_CONTROLm,
                                    MEM_BLOCK_ALL, 0, &ft_ts_u16b_entry);

        if (BCM_FAILURE(result)) {
            goto cleanup;
        }
    }
#endif /* BCM_FLOWTRACKER_V3_SUPPORT */

    if (soc_property_get(unit, spn_FLOWTRACKER_CHIP_DEBUG_ENABLE, 0)) {
        BCMI_FT_CHIP_DEBUG_ENABLE(unit) = 1;
    } else {
        BCMI_FT_CHIP_DEBUG_ENABLE(unit) = 0;
    }

    ft_initialized[unit] = 1;

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        /* Now that all the initial state is created, recover the data. */
        result = bcmi_ft_recover(unit);
    } else
    {
        result = bcmi_ft_warm_boot_alloc(unit);
    }

    if (BCM_FAILURE(result)) {
        goto cleanup;
    }

#endif /* BCM_WARM_BOOT_SUPPORT */

    return result;

cleanup:
    bcmi_ft_cleanup(unit);

    return result;
}

/*
 * Function:
 *   bcmi_ft_cleanup
 * Purpose:
 *   Cleanup Flowtracker state.
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   None.
 */
int
bcmi_ft_cleanup(int unit)
{
    _bcm_field_ft_group_invalidate(unit);

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        bcmi_ftv3_aggregate_class_cleanup(unit);
    }
#endif

    bcmi_ft_user_cleanup(unit);

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        bcmi_ftv2_drop_reason_group_cleanup(unit);
    }
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */

    bcmi_ft_flowchecker_cleanup(unit);

    bcmi_ft_alu_load_cleanup(unit);

    bcmi_ft_alu_hash_cleanup(unit);

    bcmi_ft_export_cleanup(unit);

    bcmi_ft_group_state_clear(unit);

    bcmi_ft_udf_tracking_param_info_init(unit);

    ft_initialized[unit] = 0;

    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
int
bcmi_ft_group_wb_alloc_size(int unit,
                            bcm_flowtracker_group_t id,
                            int *alloc_size_p)
{
    int alloc_size = 0;
    int i = 0;
    int length = 0;
    bcmi_ft_flowchecker_list_t **head = NULL;


    /* Allocate space for bcmi_ft_group_config_t. */
    alloc_size += sizeof(int);
    length = BCMI_FT_GROUP_TRACK_PARAM_NUM(unit, id);

    for (i=0; i < length; i++) {
        alloc_size += sizeof(uint32);
        alloc_size += sizeof(bcm_flowtracker_tracking_param_type_t);
        alloc_size += sizeof(bcm_flowtracker_tracking_param_mask_t);
        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_2) {
            alloc_size += sizeof(bcm_flowtracker_check_t);
            if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
                alloc_size += sizeof(bcm_flowtracker_timestamp_source_t);
            }
        }
        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_3) {
            alloc_size += sizeof(bcm_flowtracker_direction_t);
            alloc_size += sizeof(uint16);
        }
    }

    /* State transition type */
    if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_3) {
        alloc_size += sizeof(int);
    }

    /* allocate space for meter info. */
    alloc_size += sizeof(int);
    alloc_size += sizeof(int);

    /* Allocate space for flowchecker list. */

    /* Get the head of the list. */
    head = &(BCMI_FT_GROUP_FLOWCHECKER_HEAD(unit, id));

    alloc_size += sizeof(int);
    /* Get the length of list which is equal to number of checks. */
    length = bcmi_ft_flowchecker_list_length_get(unit, head);
    alloc_size += (length * sizeof(bcm_flowtracker_check_t));

    /* Template_id */
    alloc_size += sizeof(int);
    /* Collector Id. */
    alloc_size += sizeof(int);

    /* Group extraction data */
    alloc_size += sizeof(int);

    /* Calculate length for the session data. */
    length = BCMI_FT_GROUP_EXT_INFO(unit, id).num_data_info;
    for (i=0; i < length; i++) {

        /* Group ALU flags. */
        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_2) {
            alloc_size += sizeof(uint32);
        }

        /* Key 1 attributes and element type. */
        alloc_size += sizeof(int);

        alloc_size += sizeof(int);

        alloc_size += sizeof(int);

        alloc_size += sizeof(bcm_flowtracker_tracking_param_type_t);

        /* Key 2 attributes and element type. */
        alloc_size += sizeof(int);

        alloc_size += sizeof(int);

        alloc_size += sizeof(int);

        alloc_size += sizeof(bcm_flowtracker_tracking_param_type_t);

        /* action Key attributes and element type. */
        alloc_size += sizeof(int);

        alloc_size += sizeof(int);

        alloc_size += sizeof(int);

        alloc_size += sizeof(bcm_flowtracker_tracking_param_type_t);

        /* Flowchecker id. */
        alloc_size += sizeof(bcm_flowtracker_check_t);

        /* Alu FT Type. */
        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_2) {
            alloc_size += sizeof(bcmi_ft_type_t);
        }

        /* Alu managament data. */
        alloc_size += sizeof(int);

        alloc_size += sizeof(bcmi_ft_alu_load_type_t);

        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_3) {
            /* Flow direction */
            alloc_size += sizeof(bcm_flowtracker_direction_t);

            /* key1 param Custom Id */
            alloc_size += sizeof(uint16);

            /* Key2 param Custom Id */
            alloc_size += sizeof(uint16);

            /* Action_key param Custom Id */
            alloc_size += sizeof(uint16);
        }
    }

    /* Group extraction key. */
    alloc_size += sizeof(int);

    /* Calculate length for the session key. */
    length = BCMI_FT_GROUP_EXT_INFO(unit, id).num_key_info;
    for (i=0; i < length; i++) {

        /* Group ALU flags. */
        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_2) {
            alloc_size += sizeof(uint32);
        }

        /* Key 1 attributes and element type. */
        alloc_size += sizeof(int);

        alloc_size += sizeof(int);

        alloc_size += sizeof(int);

        alloc_size += sizeof(bcm_flowtracker_tracking_param_type_t);

        /* Key 2 attributes and element type. */
        alloc_size += sizeof(int);

        alloc_size += sizeof(int);

        alloc_size += sizeof(int);

        alloc_size += sizeof(bcm_flowtracker_tracking_param_type_t);

        /* action Key attributes and element type. */
        alloc_size += sizeof(int);

        alloc_size += sizeof(int);

        alloc_size += sizeof(int);

        alloc_size += sizeof(bcm_flowtracker_tracking_param_type_t);

        /* Flowchecker id. */
        alloc_size += sizeof(bcm_flowtracker_check_t);

        /* Alu FT Type. */
        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_2) {
            alloc_size += sizeof(bcmi_ft_type_t);
        }

        /* Alu managament data. */
        alloc_size += sizeof(int);

        alloc_size += sizeof(bcmi_ft_alu_load_type_t);

        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_3) {
            /* Direction not required */
            /* Key1 param Custom Id */
            alloc_size += sizeof(uint16);
        }
    }

    /* FT Key mode. */
    alloc_size += sizeof(bcmi_ft_group_key_data_mode_t);
    /* FT Data mode. */
    alloc_size += sizeof(bcmi_ft_group_key_data_mode_t);

    /* num_ftfp_entries. */
    alloc_size += sizeof(int);

    /* Direction control type on this group. */
    if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_2) {
        alloc_size += sizeof(bcm_flowtracker_group_control_type_t);
    }

    /* Direction on this group. */
    alloc_size += sizeof(bcm_flowtracker_direction_t);

    /* flowtracker control on this group. */
    alloc_size += sizeof(int);

    /* New learn control on this group. */
    alloc_size += sizeof(int);

    /* bcmi_ft_group_ftfp_data_t. key, data and alu_data profile. */
    alloc_size += sizeof(uint32);
    alloc_size += sizeof(uint32);
    alloc_size += sizeof(uint32);

    /* Now storing PDD entry as it can be read from hardware. */
    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        alloc_size +=
            (BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAggAlu32) *
             sizeof(uint32));
    } else {
        alloc_size +=
            (BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32) *
             sizeof(uint32));
    }
    alloc_size +=
        (BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16) *
         sizeof(uint32));

    /* Groups Flags */
    alloc_size += sizeof(uint32);

    /* Groups Validated status. */
    alloc_size += sizeof(int);

    /* Save Groups timestamp triggers. */
    alloc_size += sizeof(uint32);

    /* Save Groups trigger info. */
    if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_2) {
        for (i = 0; i < BCMI_FT_GROUP_MAX_TS_TYPE; i++) {
            /* Trigger Info flags. */
            alloc_size += sizeof(uint8);

            /* Trigger Info timestamp bank. */
            alloc_size += sizeof(uint8);

            /* Trigger Info timestamp ref count. */
            alloc_size += sizeof(uint16);

            /* Trigger Info timestamp FT type. */
            alloc_size += sizeof(bcmi_ft_type_t);
        }
    }

    *alloc_size_p += alloc_size;

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_wb_alloc_size
 * Purpose:
 *   Allocate persistent info memory for flowtracker module
 * Parameters:
 *   unit - StrataSwitch unit number.
 * Returns:
 *   BCM_E_XXX
 */
void
bcmi_ft_wb_alloc_size(int unit, int *size)
{
    int alloc_size = 0;
    int num_groups = 0, id = 0, i=0, j=0;
    soc_mem_t mem;
    int length = 0;
    int num_flowcheckers = 0;
    int total_indexes_per_mem = 0;
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    bcm_flowtracker_group_type_t group_type;
#endif

    mem = BSC_KG_GROUP_TABLEm;

    /* total number of groups. */
    num_groups = soc_mem_index_count(unit, mem);

    /* Now allocated which group is being used. */
    num_flowcheckers = num_groups;

    /* Group Valid bits. */
    alloc_size += SHR_BITALLOCSIZE(num_groups);
    LOG_INFO(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            " Total flowtracker wb size requested =  %d \n"), alloc_size));

    /* start getting the size of each element of group sw state. */
    for(i = 0; i < num_groups; i++) {

        BCM_FLOWTRACKER_GROUP_ID_SET(id, bcmFlowtrackerGroupTypeNone, i);

        if (bcmi_ft_group_is_invalid(unit, id)) {
            continue;
        }

        bcmi_ft_group_wb_alloc_size(unit, id, &alloc_size);
    }

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        for (group_type = bcmFlowtrackerGroupTypeAggregateIngress;
                group_type < bcmFlowtrackerGroupTypeCount;
                group_type++) {

            num_groups = BCMI_FT_GROUP_TYPE_MAX_COUNT(unit, group_type);

            for (i = 0; i < num_groups; i++) {

                BCM_FLOWTRACKER_GROUP_ID_SET(id, group_type, i);
                if (bcmi_ft_group_is_invalid(unit, id)) {
                    continue;
                }

                bcmi_ft_group_wb_alloc_size(unit, id, &alloc_size);
            }
        }
    }
#endif /* BCM_FLOWTRACKER_V3_SUPPORT */

    /* Allocate state for flowtracker checks. */
    /* for now number of flowcheckers is same as groups. */

    /* Allocate state for primary and secondary check refcounts. */
    alloc_size += ((num_flowcheckers) * sizeof(uint32));

    /* Allocate space to save indexes for flowcheckers. */
    alloc_size += SHR_BITALLOCSIZE(num_flowcheckers);

    for (id=0; id < num_flowcheckers; id++) {
        if (!(SHR_BITGET(BCMI_FT_FLOWCHECKER_BITMAP(unit), id))) {
            /* If the particular id is not set then do not save state. */
            continue;
        }
        /* Allocate space for bcmi_flowtracker_flowchecker_info_t */

        /* top level Flags */
        alloc_size += sizeof(uint32);

        /* Check-1 flags. */
        alloc_size += sizeof(uint32);
        /* Check-1 element. */
        alloc_size += sizeof(bcm_flowtracker_tracking_param_type_t);
        /* Check-1 min_value. */
        alloc_size += sizeof(uint32);
        /* Check-1 max_value. */
        alloc_size += sizeof(uint32);
        /* Check-1 operation. */
        alloc_size += sizeof(bcm_flowtracker_check_operation_t);
        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_2) {
            /* Check-1 mask_value. */
            alloc_size += sizeof(uint32);
            /* Check-1 shift_value. */
            alloc_size += sizeof(uint8);
            /* Check-1 state_transition_flags. */
            alloc_size += sizeof(uint32);
            /* Check-1 num_drop_reason_group_id. */
            alloc_size += sizeof(uint16);
            /* Check-1 drop reason group ids. */
            alloc_size += sizeof(bcm_flowtracker_drop_reason_group_t) *
                                 BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX;
        }
        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_3) {
            /* Check-1 Custom Id */
            alloc_size += sizeof(uint16);
        }

        /* Check-2 flags. */
        alloc_size += sizeof(uint32);
        /* Check-2 element. */
        alloc_size += sizeof(bcm_flowtracker_tracking_param_type_t);
        /* Check-2 min_value. */
        alloc_size += sizeof(uint32);
        /* Check-2 max_value. */
        alloc_size += sizeof(uint32);
        /* Check-2 operation. */
        alloc_size += sizeof(bcm_flowtracker_check_operation_t);
        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_2) {
            /* Check-2 primary checkid value. */
            alloc_size += sizeof(bcm_flowtracker_check_t);
            /* Check-2 mask_value. */
            alloc_size += sizeof(uint32);
            /* Check-2 shift_value. */
            alloc_size += sizeof(uint8);
            /* Check-2 state_transition_flags. */
            alloc_size += sizeof(uint32);
            /* Check-2 num_drop_reason_group_id. */
            alloc_size += sizeof(uint16);
            /* Check-2 drop reason group ids. */
            alloc_size += sizeof(bcm_flowtracker_drop_reason_group_t) *
                BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX;
        }
        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_3) {
            /* Check-2 Custom Id */
            alloc_size += sizeof(uint16);
        }

        /* Action element. */
        alloc_size += sizeof(bcm_flowtracker_tracking_param_type_t);
        /* action. */
        alloc_size += sizeof(bcm_flowtracker_check_action_t);
        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_2) {
            /* Action flags. */
            alloc_size += sizeof(uint32);
            /* Action mask_value. */
            alloc_size += sizeof(uint32);
            /* Action shift_value. */
            alloc_size += sizeof(uint8);
            /* Action weight. */
            alloc_size += sizeof(bcm_flowtracker_tracking_param_weight_t);
        }
        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_3) {
            /* Action direction */
            alloc_size += sizeof(bcm_flowtracker_direction_t);
            /* Action custom id */
            alloc_size += sizeof(uint16);
        }

        /* export threshold. */
        alloc_size += sizeof(uint32);
        /* operation. */
        alloc_size += sizeof(bcm_flowtracker_check_operation_t);

        /* check delay. */
        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_2) {
            /* delay source timestamp. */
            alloc_size += sizeof(bcm_flowtracker_timestamp_source_t);
            /* delay dest timestamp. */
            alloc_size += sizeof(bcm_flowtracker_timestamp_source_t);
            /* delay granularity. */
            alloc_size += sizeof(uint8);
            /* delay offset. */
            alloc_size += sizeof(int);
        }
    }

    /* Allocate space for alu memory. */
    for (i = bcmiFtAluLoadTypeLoad8; i < bcmiFtAluLoadTypeNone; i++) {

        total_indexes_per_mem = 0;
        for (j = 0; j < BCMI_FT_ALU_LOAD_MEM_COUNT(unit, i); j++) {

            BCMI_FT_ALU_LOAD_MEM_SIZE(unit, i, j) =
                    soc_mem_index_count(unit, BCMI_FT_ALU_LOAD_MEM(unit, i, j));

            total_indexes_per_mem += BCMI_FT_ALU_LOAD_MEM_SIZE(unit, i, j);
        }
        /* Index bitmap for each alu index. */
        alloc_size += SHR_BITALLOCSIZE(total_indexes_per_mem);
    }

    /* Chip debug Count data. */
    /* if feature is enabled. */
    alloc_size += sizeof(int);

    if (BCMI_FT_CHIP_DEBUG_ENABLE(unit)) {
        /* Number of chip debug objects. */
        alloc_size += sizeof(int);

        /* memory size of chip debug parameters. */
        length = BCMI_FT_CHIP_DEBUG_INFO(unit).num_debug_info;
        alloc_size += (length * sizeof(bcmi_ft_chip_debug_param_t));

        /* ALU32 used status. */
        alloc_size +=
         BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32) * sizeof(int);

        /* ALU16 used status. */
        alloc_size +=
         BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16) * sizeof(int);

        /* ALU32 ref count. */
        alloc_size +=
         BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32) * sizeof(int);

        /* ALU16 ref count. */
        alloc_size +=
         BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16) * sizeof(int);
    }

    /* Drop Reason Groups */
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_2) {
            /* Ing Egr drop reason group bitmap. */
            alloc_size += SHR_BITALLOCSIZE(BCMI_FT_DROP_REASON_ING_MMU_MAX_IDX(unit));
            /* Egr drop reason group bitmap. */
            alloc_size += SHR_BITALLOCSIZE(BCMI_FT_DROP_REASON_EGR_MAX_IDX(unit));
            /* Ing Egr drop reason group refcount. */
            alloc_size += (BCMI_FT_DROP_REASON_ING_MMU_MAX_IDX(unit) * sizeof(uint16));
            /* Egr drop reason group refcount. */
            alloc_size += (BCMI_FT_DROP_REASON_EGR_MAX_IDX(unit) * sizeof(uint16));
        }
    }
#endif

    /* Aggregate Class */
#ifdef BCM_FLOWTRACKER_V3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_2) {
            /* Mmu Flowtracker Class bitmap */
            alloc_size += SHR_BITALLOCSIZE(BCMI_FT_AGG_CLASS_MAX_IDX(unit));
            /* Egr Flowtracker Class bitmap */
            alloc_size += SHR_BITALLOCSIZE(BCMI_FT_AGG_CLASS_MAX_IDX(unit));
            /* Reference counts */
            alloc_size += (sizeof(int) * BCMI_FT_AGG_CLASS_MAX_IDX(unit));
        }
    }
#endif

    /* Aggregate Drop Reason Groups */
#ifdef BCM_FLOWTRACKER_V3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_3) {
            /* Ing Egr drop reason group bitmap. */
            alloc_size += SHR_BITALLOCSIZE(BCMI_FT_DROP_REASON_ING_MMU_MAX_IDX(unit));
            /* Egr drop reason group bitmap. */
            alloc_size += SHR_BITALLOCSIZE(BCMI_FT_DROP_REASON_EGR_MAX_IDX(unit));
            /* Ing Egr drop reason group refcount. */
            alloc_size += (BCMI_FT_DROP_REASON_ING_MMU_MAX_IDX(unit) * sizeof(uint16));
            /* Egr drop reason group refcount. */
            alloc_size += (BCMI_FT_DROP_REASON_EGR_MAX_IDX(unit) * sizeof(uint16));
        }
    }
#endif

    /* UDF tracking param */
    bcmi_ft_udf_tracking_param_info_alloc_size(unit, &alloc_size);

    /* Return the calculated size. */
    *size = alloc_size;

    LOG_INFO(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                    " Total flowtracker wb size requested =  %d \n"), alloc_size));

    return;
}

int
bcmi_ft_warm_boot_alloc(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *ft_scache_ptr;
    int size = 0;
    int rv = BCM_E_NONE;

    bcmi_ft_wb_alloc_size(unit, &size);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
            BCM_MODULE_FLOWTRACKER, BCM_WB_FT_PART_BASE);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
            size, &ft_scache_ptr,
            BCM_WB_DEFAULT_VERSION, NULL);

    if (BCM_E_NOT_FOUND == rv) {
        /* Proceed with Level 1 Warm Boot */
        rv = BCM_E_NONE;
    }

    if (BCM_SUCCESS(rv)) {
        SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                BCM_MODULE_FLOWTRACKER, BCM_WB_FT_PART_EXPORT);
        rv = bcmi_ft_export_warmboot_alloc(unit, scache_handle);
    }

    if (BCM_SUCCESS(rv)) {
        SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                BCM_MODULE_FLOWTRACKER, BCM_WB_FT_PART_USER);
        rv = bcmi_ft_user_warmboot_alloc(unit, scache_handle);
    }

    return rv;
}

int
bcmi_ft_group_state_sync(int unit,
        bcm_flowtracker_group_t id,
        uint8 **p_ft_scache_ptr)
{
    int i = 0, length = 0;
    uint8 *ft_scache_ptr = NULL;
    bcmi_ft_group_alu_info_t *temp = NULL;
    bcmi_ft_flowchecker_list_t **head = NULL;
    bcmi_ft_flowchecker_list_t *fc_temp = NULL;
    bcm_flowtracker_tracking_param_info_t *t_info = NULL;

    ft_scache_ptr = *p_ft_scache_ptr;

    /* Save tracking parametrs. */
    sal_memcpy(ft_scache_ptr, &(BCMI_FT_GROUP_TRACK_PARAM_NUM(unit, id)),
            sizeof(int));
    ft_scache_ptr += sizeof(int);
    length = BCMI_FT_GROUP_TRACK_PARAM_NUM(unit, id);
    t_info = BCMI_FT_GROUP_TRACK_PARAM(unit, id);

    for (i=0; i < length; i++) {
        sal_memcpy(ft_scache_ptr, &t_info->flags, sizeof(uint32));
        ft_scache_ptr += sizeof(uint32);

        sal_memcpy(ft_scache_ptr, &t_info->param,
                sizeof(bcm_flowtracker_tracking_param_type_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_type_t);

        sal_memcpy(ft_scache_ptr, &t_info->mask,
                sizeof(bcm_flowtracker_tracking_param_type_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_mask_t);

        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_2) {
            sal_memcpy(ft_scache_ptr, &t_info->check_id,
                    sizeof(bcm_flowtracker_check_t));
            ft_scache_ptr += sizeof(bcm_flowtracker_check_t);
            if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
                sal_memcpy(ft_scache_ptr, &t_info->src_ts,
                        sizeof(bcm_flowtracker_timestamp_source_t));
                ft_scache_ptr += sizeof(bcm_flowtracker_timestamp_source_t);
            }
        }
        t_info++;
    }

    /* Save meter related information. */
    sal_memcpy(ft_scache_ptr,
            &BCMI_FT_GROUP_METER_INFO(unit, id).sw_ckbits_sec,
            sizeof(uint32));
    ft_scache_ptr += sizeof(uint32);

    sal_memcpy(ft_scache_ptr,
            &BCMI_FT_GROUP_METER_INFO(unit, id).sw_ckbits_burst,
            sizeof(uint32));
    ft_scache_ptr += sizeof(uint32);

    /* Save information for flowchecker lists. */
    /* Get the head of the list. */
    head = &(BCMI_FT_GROUP_FLOWCHECKER_HEAD(unit, id));

    /* Get the length of list which is equal to number of checks. */
    length = bcmi_ft_flowchecker_list_length_get(unit, head);

    sal_memcpy(ft_scache_ptr, &length, sizeof(int));
    ft_scache_ptr += sizeof(int);

    /* Put head into local variable for traverse. */
    fc_temp = (*head);

    while(fc_temp) {
        sal_memcpy(ft_scache_ptr, &fc_temp->flowchecker_id,
                sizeof(bcm_flowtracker_check_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_check_t);
        fc_temp = fc_temp->next;
    }

    /* Save template id. */
    sal_memcpy(ft_scache_ptr, &BCMI_FT_GROUP(unit, id)->template_id,
            sizeof(uint32));
    ft_scache_ptr += sizeof(uint32);

    /* Save collector id. */
    sal_memcpy(ft_scache_ptr, &BCMI_FT_GROUP(unit, id)->collector_id,
            sizeof(uint32));
    ft_scache_ptr += sizeof(uint32);

    /* Save Groups extraction information.
     *
     * Session Data.
     */
    sal_memcpy(ft_scache_ptr,
            &BCMI_FT_GROUP_EXT_INFO(unit, id).num_data_info, sizeof(int));
    ft_scache_ptr += sizeof(int);

    /* Calculate length for the session data. */
    length = BCMI_FT_GROUP_EXT_INFO(unit, id).num_data_info;
    temp = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);
    for (i=0; i < length; i++) {

        /* Group ALU flags. */
        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_2) {
            sal_memcpy(ft_scache_ptr, &(temp->flags), sizeof(uint32));
            ft_scache_ptr += sizeof(uint32);
        }

        /* Key 1 attributes and element type. */
        sal_memcpy(ft_scache_ptr, &(temp->key1.location), sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(ft_scache_ptr, &(temp->key1.length), sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(ft_scache_ptr, &(temp->key1.is_alu), sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(ft_scache_ptr, &(temp->element_type1), sizeof(int));
        ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_type_t);

        /* Key 2 attributes and element type. */
        sal_memcpy(ft_scache_ptr, &(temp->key2.location), sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(ft_scache_ptr, &(temp->key2.length), sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(ft_scache_ptr, &(temp->key2.is_alu), sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(ft_scache_ptr, &(temp->element_type2), sizeof(int));
        ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_type_t);

        /* action Key attributes and element type. */
        sal_memcpy(ft_scache_ptr, &(temp->action_key.location),
                sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(ft_scache_ptr, &(temp->action_key.length), sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(ft_scache_ptr, &(temp->action_key.is_alu), sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(ft_scache_ptr, &(temp->action_element_type),
                sizeof(int));
        ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_type_t);

        /* Flowchecker id. */
        sal_memcpy(ft_scache_ptr, &(temp->flowchecker_id), sizeof(int));
        ft_scache_ptr += sizeof(bcm_flowtracker_check_t);

        /* Alu FT Type. */
        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_2) {
            sal_memcpy(ft_scache_ptr, &(temp->alu_ft_type), sizeof(bcmi_ft_type_t));
            ft_scache_ptr += sizeof(bcmi_ft_type_t);
        }

        /* Alu managament data. */
        sal_memcpy(ft_scache_ptr, &(temp->alu_load_index), sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(ft_scache_ptr, &(temp->alu_load_type), sizeof(int));
        ft_scache_ptr += sizeof(bcmi_ft_alu_load_type_t);
        temp++;
    }

    /*
     * Session key.
     */
    sal_memcpy(ft_scache_ptr,
            &BCMI_FT_GROUP_EXT_INFO(unit, id).num_key_info, sizeof(int));
    ft_scache_ptr += sizeof(int);

    /* Calculate length for the session key. */
    length = BCMI_FT_GROUP_EXT_INFO(unit, id).num_key_info;
    temp = BCMI_FT_GROUP_EXT_KEY_INFO(unit, id);
    for (i=0; i < length; i++) {

        /* Group ALU flags. */
        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_2) {
            sal_memcpy(ft_scache_ptr, &(temp->flags), sizeof(uint32));
            ft_scache_ptr += sizeof(uint32);
        }

        /* Key 1 attributes and element type. */
        sal_memcpy(ft_scache_ptr, &(temp->key1.location), sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(ft_scache_ptr, &(temp->key1.length), sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(ft_scache_ptr, &(temp->key1.is_alu), sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(ft_scache_ptr, &(temp->element_type1), sizeof(int));
        ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_type_t);

        /* Key 2 attributes and element type. */
        sal_memcpy(ft_scache_ptr, &(temp->key2.location), sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(ft_scache_ptr, &(temp->key2.length), sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(ft_scache_ptr, &(temp->key2.is_alu), sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(ft_scache_ptr, &(temp->element_type2), sizeof(int));
        ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_type_t);

        /* action Key attributes and element type. */
        sal_memcpy(ft_scache_ptr, &(temp->action_key.location),
                sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(ft_scache_ptr, &(temp->action_key.length), sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(ft_scache_ptr, &(temp->action_key.is_alu), sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(ft_scache_ptr, &(temp->action_element_type),
                sizeof(int));
        ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_type_t);

        /* Flowchecker id. */
        sal_memcpy(ft_scache_ptr, &(temp->flowchecker_id), sizeof(int));
        ft_scache_ptr += sizeof(bcm_flowtracker_check_t);

        /* Alu FT Type. */
        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_2) {
            sal_memcpy(ft_scache_ptr, &(temp->alu_ft_type), sizeof(bcmi_ft_type_t));
            ft_scache_ptr += sizeof(bcmi_ft_type_t);
        }

        /* Alu managament data. */
        sal_memcpy(ft_scache_ptr, &(temp->alu_load_index), sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(ft_scache_ptr, &(temp->alu_load_type), sizeof(int));
        ft_scache_ptr += sizeof(bcmi_ft_alu_load_type_t);
        temp++;
    }

    /* Session key mode. */
    sal_memcpy(ft_scache_ptr,
            &BCMI_FT_GROUP_EXT_HW_INFO(unit, id).ft_key_mode,
            sizeof(bcmi_ft_group_key_data_mode_t));
    ft_scache_ptr += sizeof(bcmi_ft_group_key_data_mode_t);

    /* Session data mode. */
    sal_memcpy(ft_scache_ptr,
            &BCMI_FT_GROUP_EXT_HW_INFO(unit, id).ft_data_mode,
            sizeof(bcmi_ft_group_key_data_mode_t));
    ft_scache_ptr += sizeof(bcmi_ft_group_key_data_mode_t);

    /* Save total ftfp entries on this group. */
    sal_memcpy(ft_scache_ptr, &BCMI_FT_GROUP(unit, id)->num_ftfp_entries,
            sizeof(uint32));
    ft_scache_ptr += sizeof(uint32);

    /* Direction control type on this group. */
    if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_2) {
        sal_memcpy(ft_scache_ptr,
                &BCMI_FT_GROUP_FTFP_DATA(unit, id).dir_ctrl_type,
                sizeof(bcm_flowtracker_group_control_type_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_group_control_type_t);
    }

    /* Direction on this group. */
    sal_memcpy(ft_scache_ptr, &BCMI_FT_GROUP_FTFP_DATA(unit, id).direction,
            sizeof(bcm_flowtracker_direction_t));
    ft_scache_ptr += sizeof(bcm_flowtracker_direction_t);

    /* flowtracker control on this group. */
    sal_memcpy(ft_scache_ptr, &BCMI_FT_GROUP_FTFP_DATA(unit, id).flowtrack,
            sizeof(int));
    ft_scache_ptr += sizeof(int);

    /* New learn control on this group. */
    sal_memcpy(ft_scache_ptr, &BCMI_FT_GROUP_FTFP_DATA(unit, id).new_learn,
            sizeof(int));
    ft_scache_ptr += sizeof(int);

    /* FTFP Data. Profile indexes. */
    sal_memcpy(ft_scache_ptr,
            &BCMI_FT_GROUP_FTFP_DATA(unit, id).profiles.session_key_profile_idx,
            sizeof(uint32));
    ft_scache_ptr += sizeof(uint32);

    sal_memcpy(ft_scache_ptr,
            &BCMI_FT_GROUP_FTFP_DATA(unit, id).profiles.session_data_profile_idx,
            sizeof(uint32));
    ft_scache_ptr += sizeof(uint32);

    sal_memcpy(ft_scache_ptr,
            &BCMI_FT_GROUP_FTFP_DATA(unit, id).profiles.alu_data_profile_idx,
            sizeof(uint32));
    ft_scache_ptr += sizeof(uint32);

    /* Save the PDD entry here. Read the template head. */

    /* Save Groups ALU allocation status. */
    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        sal_memcpy(ft_scache_ptr, (BCMI_FT_GROUP(unit, id)->ALU32_MEM_USE),
                (BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAggAlu32) *
                 sizeof(uint32)));
        ft_scache_ptr +=
            (BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAggAlu32) *
             sizeof(uint32));
    } else {
        sal_memcpy(ft_scache_ptr, (BCMI_FT_GROUP(unit, id)->ALU32_MEM_USE),
                (BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32) *
                 sizeof(uint32)));
        ft_scache_ptr +=
            (BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32) *
             sizeof(uint32));
    }

    sal_memcpy(ft_scache_ptr, (BCMI_FT_GROUP(unit, id)->ALU16_MEM_USE),
            (BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16) *
             sizeof(uint32)));

    ft_scache_ptr +=
        (BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16) *
         sizeof(uint32));

    /* Save Group Flags */
    sal_memcpy(ft_scache_ptr, &(BCMI_FT_GROUP(unit, id)->flags),
            sizeof(uint32));
    ft_scache_ptr += sizeof(uint32);

    /* Save Validate */
    sal_memcpy(ft_scache_ptr, &(BCMI_FT_GROUP(unit, id)->validated),
            (sizeof(int)));
    ft_scache_ptr += sizeof(int);

    /* Save Groups timestamp triggers. */
    sal_memcpy(ft_scache_ptr, &(BCMI_FT_GROUP_TS_TRIGGERS(unit, id)),
            (sizeof(uint32)));
    ft_scache_ptr += sizeof(uint32);

    /* Save Groups timestamp triggers info. */
    if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_2) {
        for (i = 0; i < 7; i++) {
            /* Trigger Info flags. */
            sal_memcpy(ft_scache_ptr,
                    &BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id, i), sizeof(uint8));
            ft_scache_ptr += sizeof(uint8);

            /* Trigger Info timestamp bank. */
            sal_memcpy(ft_scache_ptr,
                    &BCMI_FT_GROUP_TS_TRIGGER_BANK(unit, id, i), sizeof(uint8));
            ft_scache_ptr += sizeof(uint8);

            /* Trigger Info timestamp ref count. */
            sal_memcpy(ft_scache_ptr,
                    &BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT(unit, id, i), sizeof(uint16));
            ft_scache_ptr += sizeof(uint16);

            /* Trigger Info timestamp FT type. */
            sal_memcpy(ft_scache_ptr,
                    &BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id, i), sizeof(bcmi_ft_type_t));
            ft_scache_ptr += sizeof(bcmi_ft_type_t);
        }
    }

    *p_ft_scache_ptr = ft_scache_ptr;

    return BCM_E_NONE;
}

STATIC int
bcmi_ft_sync_ver_1_3(int unit, uint8 **p_ft_scache_ptr)
{
    int i = 0, j = 0, num_groups = 0;
    int num_flowcheckers = 0, length = 0;
    uint8 *ft_scache_ptr;
    soc_mem_t mem = INVALIDm;
    bcm_flowtracker_group_t id;
    bcmi_ft_group_alu_info_t *temp = NULL;
    bcm_flowtracker_tracking_param_info_t *t_info = NULL;

    ft_scache_ptr = *p_ft_scache_ptr;
    mem = BSC_KG_GROUP_TABLEm;
    num_groups = soc_mem_index_count(unit, mem);
    num_flowcheckers = num_groups;

    /* Sync chip aggregate drop reason group information. */
#ifdef BCM_FLOWTRACKER_V3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_3) {
            sal_memcpy(ft_scache_ptr,
                  BCMI_FT_DROP_REASON_AGG_ING_MMU_BITMAP(unit),
                  SHR_BITALLOCSIZE(BCMI_FT_DROP_REASON_ING_MMU_MAX_IDX(unit)));
            ft_scache_ptr +=
                   SHR_BITALLOCSIZE(BCMI_FT_DROP_REASON_ING_MMU_MAX_IDX(unit));

            sal_memcpy(ft_scache_ptr,
                  BCMI_FT_DROP_REASON_AGG_EGR_BITMAP(unit),
                  SHR_BITALLOCSIZE(BCMI_FT_DROP_REASON_EGR_MAX_IDX(unit)));
            ft_scache_ptr +=
                   SHR_BITALLOCSIZE(BCMI_FT_DROP_REASON_EGR_MAX_IDX(unit));

            sal_memcpy(ft_scache_ptr,
                  BCMI_FT_DROP_REASON_GROUP_AGG_ING_MMU_REFCOUNT_STATE(unit),
                 (BCMI_FT_DROP_REASON_ING_MMU_MAX_IDX(unit) * sizeof(uint16)));
            ft_scache_ptr +=
                 (BCMI_FT_DROP_REASON_ING_MMU_MAX_IDX(unit) * sizeof(uint16));

            sal_memcpy(ft_scache_ptr,
                  BCMI_FT_DROP_REASON_GROUP_AGG_EGR_REFCOUNT_STATE(unit),
                 (BCMI_FT_DROP_REASON_EGR_MAX_IDX(unit) * sizeof(uint16)));
            ft_scache_ptr +=
                 (BCMI_FT_DROP_REASON_EGR_MAX_IDX(unit) * sizeof(uint16));
        }

        LOG_INFO(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit, "Chip Aggregate Drop Reason Group State Synced. \n")));
    }
#endif

    /* Sync Change in ts_info per group */
    for(i = 0; i < num_groups; i++) {
        BCM_FLOWTRACKER_GROUP_ID_SET(id, bcmFlowtrackerGroupTypeNone, i);
        if (bcmi_ft_group_is_invalid(unit, id)) {
            continue;
        }

        for (j = 7; j < BCMI_FT_GROUP_MAX_TS_TYPE; j++) {
            /* Trigger Info flags. */
            sal_memcpy(ft_scache_ptr,
                    &BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id, j), sizeof(uint8));
            ft_scache_ptr += sizeof(uint8);

            /* Trigger Info timestamp bank. */
            sal_memcpy(ft_scache_ptr,
                    &BCMI_FT_GROUP_TS_TRIGGER_BANK(unit, id, j), sizeof(uint8));
            ft_scache_ptr += sizeof(uint8);

            /* Trigger Info timestamp ref count. */
            sal_memcpy(ft_scache_ptr,
                    &BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT(unit, id, j), sizeof(uint16));
            ft_scache_ptr += sizeof(uint16);

            /* Trigger Info timestamp FT type. */
            sal_memcpy(ft_scache_ptr,
                    &BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id, j), sizeof(bcmi_ft_type_t));
            ft_scache_ptr += sizeof(bcmi_ft_type_t);
        }

        length = BCMI_FT_GROUP_TRACK_PARAM_NUM(unit, id);
        t_info = BCMI_FT_GROUP_TRACK_PARAM(unit, id);
        for (j = 0; j < length; j++, t_info++) {
            /* Sync Direction in tracking param info */
            sal_memcpy(ft_scache_ptr, &t_info->direction, sizeof(bcm_flowtracker_direction_t));
            ft_scache_ptr += sizeof(bcm_flowtracker_direction_t);

            /* Sync Custom Id */
            sal_memcpy(ft_scache_ptr, &t_info->custom_id, sizeof(uint16));
            ft_scache_ptr += sizeof(uint16);
        }

        length = BCMI_FT_GROUP_EXT_INFO(unit, id).num_key_info;
        temp = BCMI_FT_GROUP_EXT_KEY_INFO(unit, id);
        for (j = 0; j < length; j++, temp++) {

            /* Sync Key1 param Custom Id */
            sal_memcpy(ft_scache_ptr, &(temp->custom_id1), sizeof(uint16));
            ft_scache_ptr += sizeof(uint16);
        }

        length = BCMI_FT_GROUP_EXT_INFO(unit, id).num_data_info;
        temp = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);
        for (j = 0; j < length; j++, temp++) {

            /* Sync direction in alu_info */
            sal_memcpy(ft_scache_ptr, &(temp->direction), sizeof(bcm_flowtracker_direction_t));
            ft_scache_ptr += sizeof(bcm_flowtracker_direction_t);

            /* Sync Key1 param Custom Id */
            sal_memcpy(ft_scache_ptr, &(temp->custom_id1), sizeof(uint16));
            ft_scache_ptr += sizeof(uint16);

            /* Sync Key2 param Custom Id */
            sal_memcpy(ft_scache_ptr, &(temp->custom_id2), sizeof(uint16));
            ft_scache_ptr += sizeof(uint16);

            /* Sync Action param Custom Id */
            sal_memcpy(ft_scache_ptr, &(temp->action_custom_id), sizeof(uint16));
            ft_scache_ptr += sizeof(uint16);
        }

        /* State transition type */
        sal_memcpy(ft_scache_ptr, &BCMI_FT_GROUP_CONFIG(unit, id).st_type,
            sizeof(int));
        ft_scache_ptr += sizeof(int);
    }

    /* Sync direction in Flowtracker check */
    for (i = 0; i < num_flowcheckers; i++) {
        if (!(SHR_BITGET(BCMI_FT_FLOWCHECKER_BITMAP(unit), i))) {
            /* If the particular id is not set then do not save state. */
            continue;
        }

        sal_memcpy(ft_scache_ptr,
                &BCMI_FT_FLOWCHECKER_INFO(unit, i)->check1.custom_id,
                sizeof(uint16));
        ft_scache_ptr += sizeof(uint16);

        sal_memcpy(ft_scache_ptr,
                &BCMI_FT_FLOWCHECKER_INFO(unit, i)->check2.custom_id,
                sizeof(uint16));
        ft_scache_ptr += sizeof(uint16);

        sal_memcpy(ft_scache_ptr,
                &BCMI_FT_FLOWCHECKER_INFO(unit, i)->action_info.direction,
                sizeof(bcm_flowtracker_direction_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_direction_t);

        sal_memcpy(ft_scache_ptr,
                &BCMI_FT_FLOWCHECKER_INFO(unit, i)->action_info.custom_id,
                sizeof(uint16));
        ft_scache_ptr += sizeof(uint16);
    }

    /* Aggregate Periodic Interval */
    sal_memcpy(ft_scache_ptr,
            &(BCMI_FT_GROUP_INFO(unit)->aft_periodic_interval),
            sizeof(uint32));
    ft_scache_ptr += sizeof(uint32);

    *p_ft_scache_ptr = ft_scache_ptr;
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_sync
 * Purpose:
 *   Sync flowtracker state.
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_sync(int unit)
{
    int size = 0;
    soc_mem_t mem;
    int length = 0;
    uint8 *ft_scache_ptr;
    int num_flowcheckers = 0;
    int total_indexes_per_mem = 0;
    soc_scache_handle_t scache_handle;
    int num_groups = 0, id = 0, i=0, j=0;
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    bcm_flowtracker_group_type_t group_type;
#endif

    /* Initialize the memory. */
    mem = BSC_KG_GROUP_TABLEm;

    /* total number of groups. */
    num_groups = soc_mem_index_count(unit, mem);
    num_flowcheckers = num_groups;

    /* Get the size to save warmboot state. */
    bcmi_ft_wb_alloc_size(unit, &size);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FLOWTRACKER, BCM_WB_FT_PART_BASE);
    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 size, &ft_scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, NULL));

    /* Do you want to keep some space on top for somethine. */

    /* start getting the size of each element of group sw state. */
    for(i = 0; i < num_groups; i++) {

        BCM_FLOWTRACKER_GROUP_ID_SET(id, bcmFlowtrackerGroupTypeNone, i);

        if (bcmi_ft_group_is_invalid(unit, id)) {
            continue;
        }

        bcmi_ft_group_state_sync(unit, id, &ft_scache_ptr);
    }

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        for (group_type = bcmFlowtrackerGroupTypeAggregateIngress;
                group_type < bcmFlowtrackerGroupTypeCount;
                group_type++) {

            num_groups = BCMI_FT_GROUP_TYPE_MAX_COUNT(unit, group_type);

            for (i = 0; i < num_groups; i++) {

                BCM_FLOWTRACKER_GROUP_ID_SET(id, group_type, i);
                if (bcmi_ft_group_is_invalid(unit, id)) {
                    continue;
                }

                bcmi_ft_group_state_sync(unit, id, &ft_scache_ptr);
            }
        }
    }
#endif /* BCM_FLOWTRACKER_V3_SUPPORT */

    LOG_INFO(BSL_LS_BCM_FLOWTRACKER,
        (BSL_META_U(unit, "Group state Synced. \n")));

    /* Now save state for flowcheckers. */

    /* Save ref-count state. */
    sal_memcpy(ft_scache_ptr,
               BCMI_FT_FLOWCHECKER_REFCOUNT_STATE(unit),
              (num_flowcheckers * sizeof(uint32)));
    ft_scache_ptr += (num_flowcheckers * sizeof(uint32));

    /* Allocate space to save indexes for flowcheckers. */
    sal_memcpy(ft_scache_ptr, BCMI_FT_FLOWCHECKER_BITMAP(unit),
                            SHR_BITALLOCSIZE(num_flowcheckers));
    ft_scache_ptr += SHR_BITALLOCSIZE(num_flowcheckers);

    for (id=0; id<num_flowcheckers; id++) {
        if (!(SHR_BITGET(BCMI_FT_FLOWCHECKER_BITMAP(unit), id))) {
            /* If the particular id is not set then do not save state. */
            continue;
        }

        sal_memcpy(ft_scache_ptr,
            &BCMI_FT_FLOWCHECKER_INFO(unit, id)->flags,
            sizeof(uint32));
        ft_scache_ptr += sizeof(uint32);

        /* Check-1 info */
        sal_memcpy(ft_scache_ptr,
            &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check1.flags,
            sizeof(uint32));
        ft_scache_ptr += sizeof(uint32);

        sal_memcpy(ft_scache_ptr,
            &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check1.param,
            sizeof(bcm_flowtracker_tracking_param_type_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_type_t);

        sal_memcpy(ft_scache_ptr,
            &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check1.min_value,
            sizeof(uint32));
        ft_scache_ptr += sizeof(uint32);

        sal_memcpy(ft_scache_ptr,
            &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check1.max_value,
            sizeof(uint32));
        ft_scache_ptr += sizeof(uint32);

        sal_memcpy(ft_scache_ptr,
            &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check1.operation,
             sizeof(bcm_flowtracker_check_operation_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_check_operation_t);

        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_2) {
            sal_memcpy(ft_scache_ptr,
                &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check1.mask_value,
                sizeof(uint32));
            ft_scache_ptr += sizeof(uint32);

            sal_memcpy(ft_scache_ptr,
                    &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check1.shift_value,
                    sizeof(uint8));
            ft_scache_ptr += sizeof(uint8);

            sal_memcpy(ft_scache_ptr,
                    &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check1.state_transition_flags,
                    sizeof(uint32));
            ft_scache_ptr += sizeof(uint32);

            sal_memcpy(ft_scache_ptr,
                    &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check1.num_drop_reason_group_id,
                    sizeof(uint16));
            ft_scache_ptr += sizeof(uint16);

            sal_memcpy(ft_scache_ptr,
                    &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check1.drop_reason_group_id[0],
                            (sizeof(bcm_flowtracker_drop_reason_group_t) *
                                            BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX));
            ft_scache_ptr += (sizeof(bcm_flowtracker_drop_reason_group_t) *
                                    BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX);
        }

        /* Check-2 Info */
        sal_memcpy(ft_scache_ptr,
            &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.flags,
            sizeof(uint32));
        ft_scache_ptr += sizeof(uint32);

        sal_memcpy(ft_scache_ptr,
            &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.param,
            sizeof(bcm_flowtracker_tracking_param_type_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_type_t);

        sal_memcpy(ft_scache_ptr,
            &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.min_value,
            sizeof(uint32));
        ft_scache_ptr += sizeof(uint32);

        sal_memcpy(ft_scache_ptr,
            &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.max_value,
            sizeof(uint32));
        ft_scache_ptr += sizeof(uint32);

        sal_memcpy(ft_scache_ptr,
            &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.operation,
             sizeof(bcm_flowtracker_check_operation_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_check_operation_t);

        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_2) {
            sal_memcpy(ft_scache_ptr,
                    &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.primary_check_id,
                    sizeof(bcm_flowtracker_check_t));
            ft_scache_ptr += sizeof(bcm_flowtracker_check_t);

            sal_memcpy(ft_scache_ptr,
                    &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.mask_value,
                    sizeof(uint32));
            ft_scache_ptr += sizeof(uint32);

            sal_memcpy(ft_scache_ptr,
                    &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.shift_value,
                    sizeof(uint8));
            ft_scache_ptr += sizeof(uint8);

            sal_memcpy(ft_scache_ptr,
                    &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.state_transition_flags,
                    sizeof(uint32));
            ft_scache_ptr += sizeof(uint32);

            sal_memcpy(ft_scache_ptr,
                    &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.num_drop_reason_group_id,
                    sizeof(uint16));
            ft_scache_ptr += sizeof(uint16);

            sal_memcpy(ft_scache_ptr,
                    &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.drop_reason_group_id[0],
                    (sizeof(bcm_flowtracker_drop_reason_group_t) *
                     BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX));
            ft_scache_ptr += (sizeof(bcm_flowtracker_drop_reason_group_t) *
                    BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX);
        }

        /* Action info */
        sal_memcpy(ft_scache_ptr,
            &BCMI_FT_FLOWCHECKER_INFO(unit, id)->action_info.param,
            sizeof(bcm_flowtracker_tracking_param_type_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_type_t);

        sal_memcpy(ft_scache_ptr,
            &BCMI_FT_FLOWCHECKER_INFO(unit, id)->action_info.action,
            sizeof(bcm_flowtracker_check_action_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_check_action_t);

        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_2) {
            sal_memcpy(ft_scache_ptr,
                    &BCMI_FT_FLOWCHECKER_INFO(unit, id)->action_info.flags,
                    sizeof(uint32));
            ft_scache_ptr += sizeof(uint32);

            sal_memcpy(ft_scache_ptr,
                    &BCMI_FT_FLOWCHECKER_INFO(unit, id)->action_info.mask_value,
                    sizeof(uint32));
            ft_scache_ptr += sizeof(uint32);

            sal_memcpy(ft_scache_ptr,
                    &BCMI_FT_FLOWCHECKER_INFO(unit, id)->action_info.shift_value,
                    sizeof(uint8));
            ft_scache_ptr += sizeof(uint8);

            sal_memcpy(ft_scache_ptr,
                    &BCMI_FT_FLOWCHECKER_INFO(unit, id)->action_info.weight,
                    sizeof(bcm_flowtracker_tracking_param_weight_t));
            ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_weight_t);
        }

        /* Export Info */
        sal_memcpy(ft_scache_ptr,
            &(BCMI_FT_FLOWCHECKER_INFO(unit, id)->
                export_info.export_check_threshold),
            sizeof(uint32));
        ft_scache_ptr += sizeof(uint32);

        sal_memcpy(ft_scache_ptr,
            &(BCMI_FT_FLOWCHECKER_INFO(unit, id)->export_info.operation),
            sizeof(bcm_flowtracker_check_operation_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_check_operation_t);

        /* Delay Info */
        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_2) {
            sal_memcpy(ft_scache_ptr,
                    &BCMI_FT_FLOWCHECKER_INFO(unit, id)->delay_info.src_ts,
                    sizeof(bcm_flowtracker_timestamp_source_t));
            ft_scache_ptr += sizeof(bcm_flowtracker_timestamp_source_t);

            sal_memcpy(ft_scache_ptr,
                    &BCMI_FT_FLOWCHECKER_INFO(unit, id)->delay_info.dst_ts,
                    sizeof(bcm_flowtracker_timestamp_source_t));
            ft_scache_ptr += sizeof(bcm_flowtracker_timestamp_source_t);

            sal_memcpy(ft_scache_ptr,
                    &BCMI_FT_FLOWCHECKER_INFO(unit, id)->delay_info.gran,
                    sizeof(uint8));
            ft_scache_ptr += sizeof(uint8);

            sal_memcpy(ft_scache_ptr,
                    &BCMI_FT_FLOWCHECKER_INFO(unit, id)->delay_info.offset,
                    sizeof(int));
            ft_scache_ptr += sizeof(int);
        }
    }

    LOG_INFO(BSL_LS_BCM_FLOWTRACKER,
       (BSL_META_U(unit, "Flowchecker state synced. \n")));

    for (i = bcmiFtAluLoadTypeLoad8; i < bcmiFtAluLoadTypeNone; i++) {

        total_indexes_per_mem = 0;
        for (j = 0; j < BCMI_FT_ALU_LOAD_MEM_COUNT(unit, i); j++) {

            BCMI_FT_ALU_LOAD_MEM_SIZE(unit, i, j) =
                soc_mem_index_count(unit, BCMI_FT_ALU_LOAD_MEM(unit, i, j));

            total_indexes_per_mem += BCMI_FT_ALU_LOAD_MEM_SIZE(unit, i, j);
        }
        sal_memcpy(ft_scache_ptr, BCMI_FT_ALU_LOAD_BITMAP_INFO(unit, i),
                                    SHR_BITALLOCSIZE(total_indexes_per_mem));
        ft_scache_ptr += SHR_BITALLOCSIZE(total_indexes_per_mem);
    }

    LOG_INFO(BSL_LS_BCM_FLOWTRACKER,
       (BSL_META_U(unit, "Alu-Load state synced. \n")));


    /* Sync chip debug information. */
    sal_memcpy(ft_scache_ptr, &BCMI_FT_CHIP_DEBUG_ENABLE(unit),
        sizeof(int));

    ft_scache_ptr += sizeof(int);

    if (BCMI_FT_CHIP_DEBUG_ENABLE(unit)) {
        /* Sync chip debug information. */
        sal_memcpy(ft_scache_ptr, &BCMI_FT_CHIP_DEBUG_INFO(unit).num_debug_info,
            sizeof(int));
        ft_scache_ptr += sizeof(int);

        length = BCMI_FT_CHIP_DEBUG_INFO(unit).num_debug_info;

        /* Sync chip debug information. */
        sal_memcpy(ft_scache_ptr, BCMI_FT_CHIP_DEBUG_PARAM_INFO(unit),
            length * sizeof(bcmi_ft_chip_debug_param_t));

        ft_scache_ptr += (length * sizeof(bcmi_ft_chip_debug_param_t));

        sal_memcpy(ft_scache_ptr, BCMI_FT_CHIP_DEBUG_INFO(unit).alu32_mem_used,
            BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32) *
            sizeof(int));
        ft_scache_ptr +=
        BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32) * sizeof(int);

        sal_memcpy(ft_scache_ptr, BCMI_FT_CHIP_DEBUG_INFO(unit).alu16_mem_used,
            BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16) *
            sizeof(int));
        ft_scache_ptr +=
            BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16) *
            sizeof(int);

        sal_memcpy(ft_scache_ptr, BCMI_FT_CHIP_DEBUG_INFO(unit).alu32_ref_count,
            BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32) *
            sizeof(int));
        ft_scache_ptr +=
            BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32) *
            sizeof(int);

        sal_memcpy(ft_scache_ptr, BCMI_FT_CHIP_DEBUG_INFO(unit).alu16_ref_count,
            BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16) *
            sizeof(int));
        ft_scache_ptr +=
            BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16) *
            sizeof(int);
    }

    LOG_INFO(BSL_LS_BCM_FLOWTRACKER,
        (BSL_META_U(unit, "Chip Debug State Synced. \n")));

    /* Sync chip drop reason group information. */
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_2) {
            sal_memcpy(ft_scache_ptr,
                  BCMI_FT_DROP_REASON_ING_MMU_BITMAP(unit),
                  SHR_BITALLOCSIZE(BCMI_FT_DROP_REASON_ING_MMU_MAX_IDX(unit)));
            ft_scache_ptr +=
                   SHR_BITALLOCSIZE(BCMI_FT_DROP_REASON_ING_MMU_MAX_IDX(unit));

            sal_memcpy(ft_scache_ptr,
                  BCMI_FT_DROP_REASON_EGR_BITMAP(unit),
                  SHR_BITALLOCSIZE(BCMI_FT_DROP_REASON_EGR_MAX_IDX(unit)));
            ft_scache_ptr +=
                   SHR_BITALLOCSIZE(BCMI_FT_DROP_REASON_EGR_MAX_IDX(unit));

            sal_memcpy(ft_scache_ptr,
                  BCMI_FT_DROP_REASON_GROUP_ING_MMU_REFCOUNT_STATE(unit),
                 (BCMI_FT_DROP_REASON_ING_MMU_MAX_IDX(unit) * sizeof(uint16)));
            ft_scache_ptr +=
                 (BCMI_FT_DROP_REASON_ING_MMU_MAX_IDX(unit) * sizeof(uint16));

            sal_memcpy(ft_scache_ptr,
                  BCMI_FT_DROP_REASON_GROUP_EGR_REFCOUNT_STATE(unit),
                 (BCMI_FT_DROP_REASON_EGR_MAX_IDX(unit) * sizeof(uint16)));
            ft_scache_ptr +=
                 (BCMI_FT_DROP_REASON_EGR_MAX_IDX(unit) * sizeof(uint16));
        }

        LOG_INFO(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit, "Chip Drop Reason Group State Synced. \n")));
    }
#endif

#ifdef BCM_FLOWTRACKER_V3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {

        /* Sync Flowtracker Class information */
        group_type = bcmFlowtrackerGroupTypeAggregateMmu;
        sal_memcpy(ft_scache_ptr,
            BCMI_FT_AGG_CLASS_BITMAP(unit, group_type),
            SHR_BITALLOCSIZE(BCMI_FT_AGG_CLASS_MAX_IDX(unit)));
        ft_scache_ptr += SHR_BITALLOCSIZE(BCMI_FT_AGG_CLASS_MAX_IDX(unit));

        group_type = bcmFlowtrackerGroupTypeAggregateEgress;
        sal_memcpy(ft_scache_ptr,
            BCMI_FT_AGG_CLASS_BITMAP(unit, group_type),
            SHR_BITALLOCSIZE(BCMI_FT_AGG_CLASS_MAX_IDX(unit)));
        ft_scache_ptr += SHR_BITALLOCSIZE(BCMI_FT_AGG_CLASS_MAX_IDX(unit));

        sal_memcpy(ft_scache_ptr, BCMI_FT_AGG_CLASS_REF(unit),
            (sizeof(int) * BCMI_FT_AGG_CLASS_MAX_IDX(unit)));
        ft_scache_ptr += (sizeof(int) * BCMI_FT_AGG_CLASS_MAX_IDX(unit));

        LOG_INFO(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit, "Flowtracker Class info Synced. \n")));
    }
#endif /* BCM_FLOWTRACKER_V3_SUPPORT */

    /* Version 1_3 Sync */
    BCM_IF_ERROR_RETURN(bcmi_ft_sync_ver_1_3(unit, &ft_scache_ptr));
    BCM_IF_ERROR_RETURN(bcmi_ft_udf_tracking_param_info_sync(unit, &ft_scache_ptr));

    /* Export database warmboot  */
    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
            BCM_MODULE_FLOWTRACKER, BCM_WB_FT_PART_EXPORT);
    BCM_IF_ERROR_RETURN
        (bcmi_ft_export_warmboot_sync(unit, scache_handle));

    /* User entry state warmboot */
    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
            BCM_MODULE_FLOWTRACKER, BCM_WB_FT_PART_USER);
    BCM_IF_ERROR_RETURN(bcmi_ft_user_warmboot_sync(unit, scache_handle));

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_param_ts_triggers_recover
 * Purpose:
 *   Recover tracking params triggers for the group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) Flowtracker group Id
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_ft_group_param_ts_triggers_recover(int unit, bcm_flowtracker_group_t id)
{
    int i = 0;
    int num_elements = 0;
    int is_timestamp_param = 0;
    uint32 param_ts_triggers = 0;
    bcm_flowtracker_tracking_param_info_t *t_info = NULL;
#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
    int trig = 0;
    uint8 ts_engine_sel = 0;
#endif

    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        return BCM_E_NONE;
    }

    t_info = BCMI_FT_GROUP_TRACK_PARAM(unit, id);
    num_elements = BCMI_FT_GROUP_TRACK_PARAM_NUM(unit, id);

    /* set triggers based on the tracking params */
    for (i = 0; i < num_elements; i++, t_info++) {
        is_timestamp_param = TRUE;
        switch(t_info->param) {
            case bcmFlowtrackerTrackingParamTypeTimestampNewLearn:
                param_ts_triggers |= (1 << BCMI_FT_GROUP_TS_NEW_LEARN);
                break;
            case bcmFlowtrackerTrackingParamTypeTimestampFlowStart:
                param_ts_triggers |= (1 << BCMI_FT_GROUP_TS_FLOW_START);
                break;
            case bcmFlowtrackerTrackingParamTypeTimestampFlowEnd:
                param_ts_triggers |= (1 << BCMI_FT_GROUP_TS_FLOW_END);
                break;
            case bcmFlowtrackerTrackingParamTypeIPATDelay:
            case bcmFlowtrackerTrackingParamTypeIPDTDelay:
                break;
            case bcmFlowtrackerTrackingParamTypeTimestampIngress:
                param_ts_triggers |= (1 << BCMI_FT_GROUP_TS_INGRESS);
                break;
            case bcmFlowtrackerTrackingParamTypeTimestampEgress:
                param_ts_triggers |= (1 << BCMI_FT_GROUP_TS_EGRESS);
                break;
            default:
                is_timestamp_param = FALSE;
                break;
        }
        if (is_timestamp_param == FALSE) {
            continue;
        }
#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
        if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
            BCMI_FT_PARAM_TS_ENGINE_SEL_GET(unit, t_info->flags, ts_engine_sel);
            if (ts_engine_sel != BCMI_FT_TS_ENGINE_SELECT_ANY) {
                BCMI_FT_TS_ENGINE_SEL_TO_GROUP_TS_TRIGGER(ts_engine_sel, trig);
                param_ts_triggers |= (1 << trig);
            }
        }
#endif
    }

    /* Update ts triggers */
    BCMI_FT_GROUP_PARAM_TS_TRIGGERS(unit, id) = param_ts_triggers;

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_sw_reinstall
 * Purpose:
 *   Recover Group's Sw state.
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_sw_reinstall(int unit, bcm_flowtracker_group_t id)
{
    int rv =  BCM_E_NONE;
    int num_alus_loads = 0, i = 0;
    bcmi_ft_group_alu_info_t *temp = NULL;
    int load16_idz[TOTAL_GROUP_LOAD16_DATA_NUM];
    int load8_idz[TOTAL_GROUP_LOAD8_DATA_NUM];
    int num_load_16 = 0, num_load_8 = 0;
    bcmi_ft_alu_load_type_t type = bcmiFtAluLoadTypeNone;
    int total_load8_mem = TOTAL_GROUP_LOAD8_DATA_NUM;
    int total_load16_mem = TOTAL_GROUP_LOAD16_DATA_NUM;

    /* Update param timestamp triggers */
    rv = bcmi_ft_group_param_ts_triggers_recover(unit, id);
    BCM_IF_ERROR_RETURN(rv);

    /* If Group is not validated, return */
    if (!BCMI_FT_GROUP_IS_VALIDATED(unit, id)) {
        return BCM_E_NONE;
    }

    /* Read the group state for the ALU inforamtion. */
    if (!(BCMI_FT_GROUP_EXT_DATA_OR_TS_TRIGGERS_SET(unit, id))) {
        return BCM_E_INTERNAL;
    }

    for (; i<total_load16_mem; i++) {
        load16_idz[i] = -1;

        if (i<total_load8_mem) {
            load8_idz[i] = -1;
        }
    }

    /* assign the ALU memory chunk to local pointer. */
    temp = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);
    /* Get total number of alus in this chunk. */
    num_alus_loads = (BCMI_FT_GROUP_EXT_INFO(unit, id)).num_data_info;

    /* allocate the bitmap based on the type, length */
    for (i=0; i<num_alus_loads; i++) {

        /* Now get the type of the memory and take action. */
        type = temp->alu_load_type;

        if (type == bcmiFtAluLoadTypeLoad16) {
            load16_idz[num_load_16] = i;
            num_load_16++;
            if (num_load_16 > total_load16_mem) {
                rv = BCM_E_PARAM;
                goto cleanup;
            }
            temp++;
            /* We do not need to do anything here. */
            continue;
        } else if (type == bcmiFtAluLoadTypeLoad8 ) {
            load8_idz[num_load_8] = i;
            num_load_8++;
            if (num_load_8 > total_load8_mem) {
                rv = BCM_E_PARAM;
                goto cleanup;
            }
            temp++;
            /* We do not need to do anything here. */
            continue;
        }
        /* Configure the ALU memory. */
        rv = bcmi_ft_group_alu_load_wb_recover(unit, id, temp,
            NULL, NULL, type);

       BCMI_CLEANUP_IF_ERROR(rv);

        /* Set that index into the group table. */
        temp++;
    }

    /*
     * The upper loop has created the ALU indexes and wrote into them.
     * Now we need to set the load profiles based on the indices
     * that we created above.
     * If there is no load information then no need to configure load memories.
     */
    if (load16_idz[0] != -1) {
        /* Configure the Load16 sw state. */
        rv = bcmi_ft_group_alu_load_wb_recover(unit, id, NULL,
            load16_idz, NULL, bcmiFtAluLoadTypeLoad16);
       BCMI_CLEANUP_IF_ERROR(rv);
    }

    if (load8_idz[0] != -1) {
        /* Configure the Load8 sw state. */
        rv = bcmi_ft_group_alu_load_wb_recover(unit, id, NULL,
            NULL, load8_idz, bcmiFtAluLoadTypeLoad8);

       BCMI_CLEANUP_IF_ERROR(rv);

    }

#if defined (BCM_FLOWTRACKER_V2_SUPPORT)
    if (soc_feature(unit, soc_feature_flowtracker_coupled_data_engines)) {
        rv = bcmi_ftv2_data_engine_db_create(unit, id, load16_idz, load8_idz);
        BCMI_CLEANUP_IF_ERROR(rv);
    }
#endif

    /* Add the pdd/pde policy entry. */
    rv = bcmi_ft_group_pdd_pde_policy_recover(unit, id);
    BCMI_CLEANUP_IF_ERROR(rv);

    /* Add the pdd/pde policy entry. */
    rv = bcmi_ft_group_profile_recover(unit, id);
    BCMI_CLEANUP_IF_ERROR(rv);

    /* Create template information list. */
    rv = bcmi_ft_group_template_data_add(unit, id, load16_idz, load8_idz);
    BCMI_CLEANUP_IF_ERROR(rv);

    /* Add the Session profiles entry */
    rv = _bcm_field_ft_session_profiles_recover(unit, id);
    BCMI_CLEANUP_IF_ERROR(rv);

    return BCM_E_NONE;

cleanup :
    bcmi_ft_group_hw_uninstall(unit, id);
    return rv;
}

/*
 * Function:
 *   bcmi_ft_group_profile_recover
 * Purpose:
 *   Recover profile entries of Group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_profile_recover(int unit,
                              bcm_flowtracker_group_t id)
{
    uint32 index = 0;

    bsc_kg_group_table_entry_t kg_group_entry;
    bsc_dg_group_table_entry_t dg_group_entry;

    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        return BCM_E_NONE;
    }

    sal_memset(&kg_group_entry, 0, sizeof(bsc_kg_group_table_entry_t));
    sal_memset(&dg_group_entry, 0, sizeof(bsc_dg_group_table_entry_t));

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
                      MEM_BLOCK_ANY, id, &kg_group_entry));
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
                      MEM_BLOCK_ANY, id, &dg_group_entry));

    /* Recover Age profile. */
    /* Get associated index from the group. */
    index = soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm,
            ((uint32 *)&kg_group_entry), AGE_OUT_PROFILE_IDXf);

    BCM_IF_ERROR_RETURN(
            bcmi_ft_group_age_profile_refcount_set(unit, index));

    /* Recover Flow limit or exceed profile. */
    /* Get associated index from the group. */
    index = soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm,
            ((uint32 *)&kg_group_entry), FLOW_EXCEED_PROFILE_IDXf);

    BCM_IF_ERROR_RETURN(
            bcmi_ft_group_flow_limit_profile_refcount_set(unit, index));

    /* Recover Collector copy profile. */
    /* Get associated index from the group. */
    index = soc_mem_field32_get(unit, BSC_DG_GROUP_TABLEm,
            ((uint32 *)&dg_group_entry), COPY_TO_COLLECTOR_PROFILE_IDXf);

    BCM_IF_ERROR_RETURN(
            bcmi_ft_group_collector_copy_profile_refcount_set(unit, index));

    /* Recover Meter profile. */
    /* Get associated index from the group. */
    index = soc_mem_field32_get(unit, BSC_DG_GROUP_TABLEm,
            ((uint32 *)&dg_group_entry), GROUP_METER_PROFILE_IDXf);

    BCM_IF_ERROR_RETURN(
            bcmi_ft_group_meter_profile_refcount_set(unit, index));

    /* now write this index into the group. */
    index = soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm,
            ((uint32 *)&kg_group_entry), SESSION_DATA_TYPEf);

    BCM_IF_ERROR_RETURN(
            bcmi_ft_group_pdd_profile_refcount_set(unit, index));

    /* Recover Timestamp profiles. */
    index = soc_mem_field32_get(unit, BSC_DG_GROUP_TABLEm,
            ((uint32 *)&dg_group_entry), TIMESTAMP_PROFILE_IDXf);

    BCM_IF_ERROR_RETURN(
            bcmi_ft_group_timestamp_profile_refcount_set(unit, index));

    return BCM_E_NONE;
}

int
bcmi_ft_group_state_recover(int unit,
        bcm_flowtracker_group_t id,
        uint16 recovered_ver,
        uint8 **p_ft_scache_ptr)
{
    int i = 0;
    bcmi_ft_flowchecker_list_t **head = NULL;
    uint8 *ft_scache_ptr = NULL;
    int length = 0;
    bcm_flowtracker_check_t flowchecker_id;
    bcmi_ft_group_alu_info_t * temp = NULL;
    bcm_flowtracker_tracking_param_info_t *t_info = NULL;
    bcm_flowtracker_group_type_t group_type;
    int num_alu32 = 0;

    ft_scache_ptr = *p_ft_scache_ptr;

    i = BCMI_FT_GROUP_INDEX_GET(id);
    group_type = BCMI_FT_GROUP_TYPE_GET(id);

    /* Set the group state here. */
    SHR_BITSET(BCMI_FT_GROUP_BITMAP(unit, group_type), i);

    /* Allocate the sw state for this group first. */
    BCMI_FT_GROUP(unit, id) = (bcmi_ft_group_sw_info_t *)
        sal_alloc(sizeof(bcmi_ft_group_sw_info_t),
                "flowtracker group sw state");

    if (BCMI_FT_GROUP(unit, id) == NULL) {
        return BCM_E_MEMORY;
    }

    /* memset everything to zero. */
    sal_memset(BCMI_FT_GROUP(unit, id), 0,
            sizeof(bcmi_ft_group_sw_info_t));

    /* Save tracking parametrs. */
    sal_memcpy(&(BCMI_FT_GROUP_TRACK_PARAM_NUM(unit, id)), ft_scache_ptr,
            sizeof(int));
    ft_scache_ptr += sizeof(int);

    BCMI_FT_GROUP_TRACK_PARAM(unit, id) =
        (bcm_flowtracker_tracking_param_info_t *)
        sal_alloc((sizeof(bcm_flowtracker_tracking_param_info_t) *
                    BCMI_FT_GROUP_TRACK_PARAM_NUM(unit, id)),
                "flowtracker group tracking parameters");

    if (BCMI_FT_GROUP_TRACK_PARAM(unit, id) == NULL) {
        return BCM_E_MEMORY;
    }

    length = BCMI_FT_GROUP_TRACK_PARAM_NUM(unit, id);
    t_info = BCMI_FT_GROUP_TRACK_PARAM(unit, id);

    for (i=0; i < length; i++) {
        sal_memcpy(&t_info->flags, ft_scache_ptr, sizeof(uint32));
        ft_scache_ptr += sizeof(uint32);

        sal_memcpy(&t_info->param, ft_scache_ptr,
                sizeof(bcm_flowtracker_tracking_param_type_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_type_t);

        sal_memcpy(&t_info->mask, ft_scache_ptr,
                sizeof(bcm_flowtracker_tracking_param_type_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_mask_t);

        if (recovered_ver >= BCM_WB_VERSION_1_2) {
            sal_memcpy(&t_info->check_id, ft_scache_ptr,
                    sizeof(bcm_flowtracker_check_t));
            ft_scache_ptr += sizeof(bcm_flowtracker_check_t);
            if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
                sal_memcpy(&t_info->src_ts, ft_scache_ptr,
                        sizeof(bcm_flowtracker_timestamp_source_t));
                ft_scache_ptr += sizeof(bcm_flowtracker_timestamp_source_t);
            }
        }
        t_info++;
    }

    /* Save meter related information. */
    sal_memcpy(&BCMI_FT_GROUP_METER_INFO(unit, id).sw_ckbits_sec,
            ft_scache_ptr, sizeof(uint32));
    ft_scache_ptr += sizeof(uint32);

    sal_memcpy(&BCMI_FT_GROUP_METER_INFO(unit, id).sw_ckbits_burst,
            ft_scache_ptr, sizeof(uint32));
    ft_scache_ptr += sizeof(uint32);


    /* Save information for flowchecker lists. */
    /* Get the head of the list. */
    head = &(BCMI_FT_GROUP_FLOWCHECKER_HEAD(unit, id));

    /* Get the length of list which is equal to number of checks. */
    sal_memcpy(&length, ft_scache_ptr, sizeof(int));
    ft_scache_ptr += sizeof(int);

    for (i=0; i<length; i++) {
        sal_memcpy(&flowchecker_id, ft_scache_ptr,
                sizeof(bcm_flowtracker_check_t));

        BCM_IF_ERROR_RETURN(
                bcmi_ft_flowchecker_list_add(unit, head, flowchecker_id));
        ft_scache_ptr += sizeof(bcm_flowtracker_check_t);
    }

    sal_memcpy(&BCMI_FT_GROUP(unit, id)->template_id, ft_scache_ptr,
            sizeof(uint32));
    ft_scache_ptr += sizeof(uint32);

    sal_memcpy(&BCMI_FT_GROUP(unit, id)->collector_id, ft_scache_ptr,
            sizeof(uint32));
    ft_scache_ptr += sizeof(uint32);

    /* Recover Groups extraction information.
     *
     * Session Data.
     */
    sal_memcpy(&BCMI_FT_GROUP_EXT_INFO(unit, id).num_data_info,
            ft_scache_ptr, sizeof(int));
    ft_scache_ptr += sizeof(int);

    length = BCMI_FT_GROUP_EXT_INFO(unit, id).num_data_info;
    if (length > 0) {
        /* allocate space needed to save this state in group. */
        BCMI_FT_GROUP_EXT_DATA_INFO(unit, id) =
            (bcmi_ft_group_alu_info_t *) sal_alloc(length *
                    sizeof(bcmi_ft_group_alu_info_t), "Group alu info.");

        if (BCMI_FT_GROUP_EXT_DATA_INFO(unit, id) == NULL) {
            return BCM_E_MEMORY;
        }
    }

    temp = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);
    for (i=0; i < length; i++) {

        /* Group ALU flags. */
        if (recovered_ver >= BCM_WB_VERSION_1_2) {
            sal_memcpy(&(temp->flags), ft_scache_ptr, sizeof(uint32));
            ft_scache_ptr += sizeof(uint32);
        }

        /* Key 1 attributes and element type. */
        sal_memcpy(&(temp->key1.location), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(&(temp->key1.length), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(&(temp->key1.is_alu), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(&(temp->element_type1), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_type_t);

        /* Key 2 attributes and element type. */
        sal_memcpy(&(temp->key2.location), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(&(temp->key2.length), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(&(temp->key2.is_alu), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(&(temp->element_type2), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_type_t);

        /* action Key attributes and element type. */
        sal_memcpy(&(temp->action_key.location), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(&(temp->action_key.length), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(&(temp->action_key.is_alu), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(&(temp->action_element_type), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_type_t);

        /* Flowchecker id. */
        sal_memcpy(&(temp->flowchecker_id), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(bcm_flowtracker_check_t);

        /* ALU FT type. */
        if (recovered_ver >= BCM_WB_VERSION_1_2) {
            sal_memcpy(&(temp->alu_ft_type), ft_scache_ptr,
                    sizeof(bcmi_ft_type_t));
            ft_scache_ptr += sizeof(bcmi_ft_type_t);
        }

        /* Alu managament data. */
        sal_memcpy(&(temp->alu_load_index), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(&(temp->alu_load_type), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(bcmi_ft_alu_load_type_t);
        temp++;
    }

    /*
     * Session key.
     */
    sal_memcpy(&BCMI_FT_GROUP_EXT_INFO(unit, id).num_key_info,
            ft_scache_ptr, sizeof(int));

    ft_scache_ptr += sizeof(int);

    /* Calculate length for the session key. */
    length = BCMI_FT_GROUP_EXT_INFO(unit, id).num_key_info;

    /* allocate space needed to save this state in group. */
    BCMI_FT_GROUP_EXT_KEY_INFO(unit, id) =
        (bcmi_ft_group_alu_info_t *) sal_alloc(length *
                sizeof(bcmi_ft_group_alu_info_t), "Group alu info.");

    if (BCMI_FT_GROUP_EXT_KEY_INFO(unit, id)== NULL) {
        return BCM_E_MEMORY;
    }

    temp = BCMI_FT_GROUP_EXT_KEY_INFO(unit, id);
    for (i=0; i < length; i++) {

        /* Group ALU flags. */
        if (recovered_ver >= BCM_WB_VERSION_1_2) {
            sal_memcpy(&(temp->flags), ft_scache_ptr, sizeof(uint32));
            ft_scache_ptr += sizeof(uint32);
        }

        /* Key 1 attributes and element type. */
        sal_memcpy(&(temp->key1.location), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(&(temp->key1.length), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(&(temp->key1.is_alu), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(&(temp->element_type1), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_type_t);

        /* Key 2 attributes and element type. */
        sal_memcpy(&(temp->key2.location), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(&(temp->key2.length), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(&(temp->key2.is_alu), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(&(temp->element_type2), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_type_t);

        /* action Key attributes and element type. */
        sal_memcpy(&(temp->action_key.location), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(&(temp->action_key.length), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(&(temp->action_key.is_alu), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(&(temp->action_element_type), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_type_t);

        /* Flowchecker id. */
        sal_memcpy(&(temp->flowchecker_id), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(bcm_flowtracker_check_t);

        /* ALU FT type. */
        if (recovered_ver >= BCM_WB_VERSION_1_2) {
            sal_memcpy(&(temp->alu_ft_type), ft_scache_ptr,
                    sizeof(bcmi_ft_type_t));
            ft_scache_ptr += sizeof(bcmi_ft_type_t);
        }

        /* Alu managament data. */
        sal_memcpy(&(temp->alu_load_index), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(&(temp->alu_load_type), ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(bcmi_ft_alu_load_type_t);
        temp++;
    }

    /* Session key mode. */
    sal_memcpy(&BCMI_FT_GROUP_EXT_HW_INFO(unit, id).ft_key_mode,
            ft_scache_ptr, sizeof(bcmi_ft_group_key_data_mode_t));
    BCMI_FT_GROUP_FTFP_DATA(unit, id).session_key_mode =
        BCMI_FT_GROUP_EXT_HW_INFO(unit, id).ft_key_mode;
    ft_scache_ptr += sizeof(bcmi_ft_group_key_data_mode_t);

    /* Session data mode. */
    sal_memcpy(&BCMI_FT_GROUP_EXT_HW_INFO(unit, id).ft_data_mode,
            ft_scache_ptr, sizeof(bcmi_ft_group_key_data_mode_t));
    ft_scache_ptr += sizeof(bcmi_ft_group_key_data_mode_t);

    /* ftfp entries on this group. */
    sal_memcpy(&BCMI_FT_GROUP(unit, id)->num_ftfp_entries, ft_scache_ptr,
            sizeof(int));
    ft_scache_ptr += sizeof(uint32);

    /* Direction control type on this group. */
    if (recovered_ver >= BCM_WB_VERSION_1_2) {
        sal_memcpy(&BCMI_FT_GROUP_FTFP_DATA(unit, id).dir_ctrl_type,
                ft_scache_ptr,
                sizeof(bcm_flowtracker_group_control_type_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_group_control_type_t);
    }

    /* Direction on this group. */
    sal_memcpy(&BCMI_FT_GROUP_FTFP_DATA(unit, id).direction, ft_scache_ptr,
            sizeof(bcm_flowtracker_direction_t));
    ft_scache_ptr += sizeof(bcm_flowtracker_direction_t);

    /* flowtracker control on this group. */
    sal_memcpy(&BCMI_FT_GROUP_FTFP_DATA(unit, id).flowtrack, ft_scache_ptr,
            sizeof(int));
    ft_scache_ptr += sizeof(int);

    /* New learn control on this group. */
    sal_memcpy(&BCMI_FT_GROUP_FTFP_DATA(unit, id).new_learn,
            ft_scache_ptr, sizeof(int));
    ft_scache_ptr += sizeof(int);

    /* FTFP data . profiles. */
    sal_memcpy(&BCMI_FT_GROUP_FTFP_DATA(unit, id).
            profiles.session_key_profile_idx, ft_scache_ptr,
            sizeof(uint32));
    ft_scache_ptr += sizeof(uint32);

    sal_memcpy(&BCMI_FT_GROUP_FTFP_DATA(unit, id).
            profiles.session_data_profile_idx, ft_scache_ptr,
            sizeof(uint32));
    ft_scache_ptr += sizeof(uint32);

    sal_memcpy(&BCMI_FT_GROUP_FTFP_DATA(unit, id).
            profiles.alu_data_profile_idx, ft_scache_ptr,
            sizeof(uint32));
    ft_scache_ptr += sizeof(uint32);

    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        num_alu32 = BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAggAlu32);
    } else {
        num_alu32 = BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32);
    }
    BCMI_FT_ALLOC(BCMI_FT_GROUP_ALU32_MEM_USE(unit, id),
            sizeof(uint32) * num_alu32, "ft group alu32 use");

    /* Recover ALU32 usage. */
    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        sal_memcpy((BCMI_FT_GROUP(unit, id)->ALU32_MEM_USE), ft_scache_ptr,
                (BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAggAlu32) *
                 sizeof(uint32)));
        ft_scache_ptr +=
            (BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAggAlu32) *
             sizeof(uint32));
    } else {
        sal_memcpy((BCMI_FT_GROUP(unit, id)->ALU32_MEM_USE), ft_scache_ptr,
                (BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32) *
                 sizeof(uint32)));
        ft_scache_ptr +=
            (BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32) *
             sizeof(uint32));
    }

    /* Recover ALU16 usage. */
    sal_memcpy((BCMI_FT_GROUP(unit, id)->ALU16_MEM_USE), ft_scache_ptr,
            (BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16) *
             sizeof(uint32)));
    ft_scache_ptr +=
        (BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16) *
         sizeof(uint32));

    /* Recover Group Flags */
    sal_memcpy(&(BCMI_FT_GROUP(unit, id)->flags), ft_scache_ptr,
            sizeof(uint32));
    ft_scache_ptr += sizeof(uint32);

    /* Recover Groups Validated status. */
    sal_memcpy(&(BCMI_FT_GROUP(unit, id)->validated), ft_scache_ptr,
            (sizeof(int)));
    ft_scache_ptr += sizeof(int);

    /* Recover Timestamp triggers. */
    sal_memcpy(&(BCMI_FT_GROUP_TS_TRIGGERS(unit, id)), ft_scache_ptr,
            (sizeof(uint32)));
    ft_scache_ptr += sizeof(uint32);

    /* Recover Timestamp triggers info. */
    if (recovered_ver >= BCM_WB_VERSION_1_2) {
        for (i = 0; i < 7; i++) {
            /* Trigger Info flags. */
            sal_memcpy(&BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id, i),
                    ft_scache_ptr, sizeof(uint8));
            ft_scache_ptr += sizeof(uint8);

            /* Trigger Info timestamp bank. */
            sal_memcpy(&BCMI_FT_GROUP_TS_TRIGGER_BANK(unit, id, i),
                    ft_scache_ptr, sizeof(uint8));
            ft_scache_ptr += sizeof(uint8);

            /* Trigger Info timestamp ref count. */
            sal_memcpy(&BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT(unit, id, i),
                    ft_scache_ptr, sizeof(uint16));
            ft_scache_ptr += sizeof(uint16);

            /* Trigger Info timestamp FT type. */
            sal_memcpy(&BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id, i),
                    ft_scache_ptr, sizeof(bcmi_ft_type_t));
            ft_scache_ptr += sizeof(bcmi_ft_type_t);
        }
    }

    *p_ft_scache_ptr = ft_scache_ptr;

    return BCM_E_NONE;
}

STATIC int
bcmi_ft_recover_ver_1_3(int unit, uint8 **p_ft_scache_ptr)
{
    int i = 0, j = 0;
    int num_groups = 0;
    uint8 *ft_scache_ptr;
    soc_mem_t mem = INVALIDm;
    bcm_flowtracker_group_t id;
    int num_flowcheckers = 0, length = 0;
    bcmi_ft_group_alu_info_t *temp = NULL;
    bcm_flowtracker_tracking_param_info_t *t_info = NULL;

    ft_scache_ptr = *p_ft_scache_ptr;
    mem = BSC_KG_GROUP_TABLEm;
    num_groups = soc_mem_index_count(unit, mem);
    num_flowcheckers = num_groups;

    /* Recover chip aggregate drop reason group information. */
#ifdef BCM_FLOWTRACKER_V3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        sal_memcpy(BCMI_FT_DROP_REASON_AGG_ING_MMU_BITMAP(unit), ft_scache_ptr,
                SHR_BITALLOCSIZE(BCMI_FT_DROP_REASON_ING_MMU_MAX_IDX(unit)));
        ft_scache_ptr +=
            SHR_BITALLOCSIZE(BCMI_FT_DROP_REASON_ING_MMU_MAX_IDX(unit));

        sal_memcpy(BCMI_FT_DROP_REASON_AGG_EGR_BITMAP(unit), ft_scache_ptr,
                SHR_BITALLOCSIZE(BCMI_FT_DROP_REASON_EGR_MAX_IDX(unit)));
        ft_scache_ptr +=
            SHR_BITALLOCSIZE(BCMI_FT_DROP_REASON_EGR_MAX_IDX(unit));

        sal_memcpy(BCMI_FT_DROP_REASON_GROUP_AGG_ING_MMU_REFCOUNT_STATE(unit),
                ft_scache_ptr,
                (BCMI_FT_DROP_REASON_ING_MMU_MAX_IDX(unit) *
                 sizeof(uint16)));
        ft_scache_ptr +=
            (BCMI_FT_DROP_REASON_ING_MMU_MAX_IDX(unit) * sizeof(uint16));

        sal_memcpy(BCMI_FT_DROP_REASON_GROUP_AGG_EGR_REFCOUNT_STATE(unit),
                ft_scache_ptr,
                (BCMI_FT_DROP_REASON_EGR_MAX_IDX(unit) *
                 sizeof(uint16)));
        ft_scache_ptr +=
            (BCMI_FT_DROP_REASON_EGR_MAX_IDX(unit) * sizeof(uint16));
    }
#endif

    /* Sync Change in ts_info per group */
    for(i = 0; i < num_groups; i++) {
        BCM_FLOWTRACKER_GROUP_ID_SET(id, bcmFlowtrackerGroupTypeNone, i);
        if (bcmi_ft_group_is_invalid(unit, id)) {
            continue;
        }

        for (j = 7; j < BCMI_FT_GROUP_MAX_TS_TYPE; j++) {
            /* Trigger Info flags. */
            sal_memcpy(&BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id, j),
                    ft_scache_ptr, sizeof(uint8));
            ft_scache_ptr += sizeof(uint8);

            /* Trigger Info timestamp bank. */
            sal_memcpy(&BCMI_FT_GROUP_TS_TRIGGER_BANK(unit, id, j),
                    ft_scache_ptr, sizeof(uint8));
            ft_scache_ptr += sizeof(uint8);

            /* Trigger Info timestamp ref count. */
            sal_memcpy(&BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT(unit, id, j),
                    ft_scache_ptr, sizeof(uint16));
            ft_scache_ptr += sizeof(uint16);

            /* Trigger Info timestamp FT type. */
            sal_memcpy(&BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id, j),
                    ft_scache_ptr, sizeof(bcmi_ft_type_t));
            ft_scache_ptr += sizeof(bcmi_ft_type_t);
        }

        /* Recover additional params in tracking param info */
        length = BCMI_FT_GROUP_TRACK_PARAM_NUM(unit, id);
        t_info = BCMI_FT_GROUP_TRACK_PARAM(unit, id);
        for (j = 0; j < length; j++, t_info++) {
            sal_memcpy(&t_info->direction, ft_scache_ptr, sizeof(bcm_flowtracker_direction_t));
            ft_scache_ptr += sizeof(bcm_flowtracker_direction_t);

            sal_memcpy(&t_info->custom_id, ft_scache_ptr, sizeof(uint16));
            ft_scache_ptr += sizeof(uint16);
        }

        /* Recover additional params in alu_info */
        length = BCMI_FT_GROUP_EXT_INFO(unit, id).num_key_info;
        temp = BCMI_FT_GROUP_EXT_KEY_INFO(unit, id);
        for (j = 0; j < length; j++, temp++) {

            /* Key1 param Custom Id */
            sal_memcpy(&(temp->custom_id1), ft_scache_ptr, sizeof(uint16));
            ft_scache_ptr += sizeof(uint16);
        }

        /* Recover additional params in alu_info */
        length = BCMI_FT_GROUP_EXT_INFO(unit, id).num_data_info;
        temp = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);
        for (j = 0; j < length; j++, temp++) {

            sal_memcpy(&(temp->direction), ft_scache_ptr, sizeof(bcm_flowtracker_direction_t));
            ft_scache_ptr += sizeof(bcm_flowtracker_direction_t);

            sal_memcpy(&(temp->custom_id1), ft_scache_ptr, sizeof(uint16));
            ft_scache_ptr += sizeof(uint16);

            sal_memcpy(&(temp->custom_id2), ft_scache_ptr, sizeof(uint16));
            ft_scache_ptr += sizeof(uint16);

            sal_memcpy(&(temp->action_custom_id), ft_scache_ptr, sizeof(uint16));
            ft_scache_ptr += sizeof(uint16);
        }

        sal_memcpy(&BCMI_FT_GROUP_CONFIG(unit, id).st_type,
                                ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(int);
    }

    /* Recover additional param in Flowtracker check */
    for (i = 0; i < num_flowcheckers; i++) {
        if (!(SHR_BITGET(BCMI_FT_FLOWCHECKER_BITMAP(unit), i))) {
            continue;
        }

        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, i)->check1.custom_id,
                ft_scache_ptr, sizeof(uint16));
        ft_scache_ptr += sizeof(uint16);
        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, i)->check2.custom_id,
                ft_scache_ptr, sizeof(uint16));
        ft_scache_ptr += sizeof(uint16);
        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, i)->action_info.direction,
                ft_scache_ptr, sizeof(bcm_flowtracker_direction_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_direction_t);
        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, i)->action_info.custom_id,
                ft_scache_ptr, sizeof(uint16));
        ft_scache_ptr += sizeof(uint16);
    }

    /* Aggregate Periodic Interval */
    sal_memcpy(&(BCMI_FT_GROUP_INFO(unit)->aft_periodic_interval),
            ft_scache_ptr, sizeof(uint32));
    ft_scache_ptr += sizeof(uint32);

    *p_ft_scache_ptr = ft_scache_ptr;
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_recover
 * Purpose:
 *   Recover flowtracker state.
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_recover(int unit)
{
    int rv = BCM_E_NONE;
    int num_groups = 0, id = 0, i=0, j=0;
    soc_mem_t mem;
    soc_mem_t mem1;
    bsc_kg_group_table_entry_t kg_group_entry;
    soc_scache_handle_t scache_handle;
    uint8 *ft_scache_ptr;
    int length = 0;
    int stable_size = 0;
    uint16 recovered_ver = 0;
    int num_flowcheckers = 0;
    int pdd_index = 0;
    int total_indexes_per_mem = 0;
    bcm_flowtracker_group_type_t group_type;
    bsd_policy_action_profile_entry_t pdd_entry;
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    bsc_dg_aift_group_table_entry_t agg_group_entry;
#endif

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FLOWTRACKER, BCM_WB_FT_PART_BASE);
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    if (stable_size <= 0) { /* Limited and Extended recovery only */
        /* No warmboot recovery possible. */
        return BCM_E_NONE;
    }

    group_type = bcmFlowtrackerGroupTypeNone;

    /* Get the scache start address where the information is stored. */
    SOC_IF_ERROR_RETURN(_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
        0, &ft_scache_ptr, BCM_WB_DEFAULT_VERSION, &recovered_ver));

    mem = BSC_KG_GROUP_TABLEm;
    /* total number of groups. */
    num_groups = soc_mem_index_count(unit, mem);
    num_flowcheckers = num_groups;

    /* start getting the size of each element of group sw state. */
    for(; id < num_groups; id++) {

        sal_memset(&kg_group_entry, 0, sizeof(bsc_kg_group_table_entry_t));

        /* First read the group entry. */
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
                          MEM_BLOCK_ANY, id, &kg_group_entry));

        /* Do it only for groups which are valid. Get VALID bit from hardware.*/
        if (!(soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm, &kg_group_entry,
                        GROUP_VALIDf))) {
            continue;
        }

        /* now write this index into the group. */
        pdd_index = soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm,
                ((uint32 *)&kg_group_entry), SESSION_DATA_TYPEf);

        bcmi_ft_group_state_recover(unit, id, recovered_ver, &ft_scache_ptr);

        /* Recover PDD entry. */
        if (BCMI_FT_GROUP_IS_VALIDATED(unit, id)) {

            bcmi_ft_alu_load_pdd_mem_get(unit, bcmiFtTypeIFT, &mem1);
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem1,
                MEM_BLOCK_ANY, pdd_index, &pdd_entry));

            SHR_BITOR_RANGE((uint32 * )&BCMI_FT_GROUP(unit, id)->pdd_entry,
                    (uint32 *) &pdd_entry, 0,
                    sizeof(bsd_policy_action_profile_entry_t) * 8,
                    (uint32 * )&BCMI_FT_GROUP(unit, id)->pdd_entry);

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
            bcmi_ft_alu_load_pdd_mem_get(unit, bcmiFtTypeMFT, &mem1);
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem1,
                        MEM_BLOCK_ANY, pdd_index, &pdd_entry));
            SHR_BITOR_RANGE((uint32 * )&BCMI_FT_GROUP(unit, id)->pdd_entry,
                    (uint32 *) &pdd_entry, 0,
                    sizeof(bsd_policy_action_profile_entry_t) * 8,
                    (uint32 * )&BCMI_FT_GROUP(unit, id)->pdd_entry);

            bcmi_ft_alu_load_pdd_mem_get(unit, bcmiFtTypeEFT, &mem1);
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem1,
                        MEM_BLOCK_ANY, pdd_index, &pdd_entry));
            SHR_BITOR_RANGE((uint32 * )&BCMI_FT_GROUP(unit, id)->pdd_entry,
                    (uint32 *) &pdd_entry, 0,
                    sizeof(bsd_policy_action_profile_entry_t) * 8,
                    (uint32 * )&BCMI_FT_GROUP(unit, id)->pdd_entry);
#endif
        }

    } /* END of group state. */

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        if (recovered_ver >= BCM_WB_VERSION_1_2) {

            for (group_type = bcmFlowtrackerGroupTypeAggregateIngress;
                    group_type < bcmFlowtrackerGroupTypeCount;
                    group_type++) {

                num_groups = BCMI_FT_GROUP_TYPE_MAX_COUNT(unit, group_type);
                bcmi_ft_group_type_mem_get(unit, 1, group_type, &mem);

                for (i = 0; i < num_groups; i++) {

                    sal_memset(&agg_group_entry, 0,
                            sizeof(bsc_dg_aift_group_table_entry_t));

                    /* First read the group entry. */
                    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem,
                                MEM_BLOCK_ANY, i, &agg_group_entry));

                    /* Get VALID bit from hardware.*/
                    if (!(soc_mem_field32_get(unit, mem, &agg_group_entry,
                                    AGG_GROUP_VALIDf))) {
                        continue;
                    }

                    BCM_FLOWTRACKER_GROUP_ID_SET(id, group_type, i);
                    bcmi_ft_group_state_recover(unit, id,
                            recovered_ver, &ft_scache_ptr);

                } /* END of group state. */
            }
        }
    }
#endif /* BCM_FLOWTRACKER_V3_SUPPORT */

    /*
     * Start flowcheckr state recovery.
     */
    sal_memcpy(BCMI_FT_FLOWCHECKER_REFCOUNT_STATE(unit),
               ft_scache_ptr,
              (num_flowcheckers * sizeof(uint32)));
    ft_scache_ptr += (num_flowcheckers * sizeof(uint32));

    sal_memcpy(BCMI_FT_FLOWCHECKER_BITMAP(unit), ft_scache_ptr,
        SHR_BITALLOCSIZE(num_flowcheckers));
    ft_scache_ptr += SHR_BITALLOCSIZE(num_flowcheckers);

    for (id = 0; id < num_flowcheckers; id++) {
        if (!(SHR_BITGET(BCMI_FT_FLOWCHECKER_BITMAP(unit), id))) {
            /* If the particular id is not set then do not save state. */
            continue;
        }

         /* Allocate memory for the new flowchecker. */
        BCMI_FT_FLOWCHECKER_INFO(unit, id) =
            (bcmi_flowtracker_flowchecker_info_t *)
            sal_alloc(sizeof(bcmi_flowtracker_flowchecker_info_t),
            "flowtracker info");

        if (BCMI_FT_FLOWCHECKER_INFO(unit, id) == NULL) {
            return BCM_E_MEMORY;
        }

        /* Rather than doing memcpy, we should actually set each element. */
        sal_memset(BCMI_FT_FLOWCHECKER_INFO(unit, id), 0,
            sizeof(bcmi_flowtracker_flowchecker_info_t));

        /* Software memory is created. Now start populating the state. */
        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->flags,
            ft_scache_ptr,
            sizeof(uint32));
        ft_scache_ptr += sizeof(uint32);

        /* Check-1 info */
        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check1.flags,
            ft_scache_ptr,
            sizeof(uint32));
        ft_scache_ptr += sizeof(uint32);

        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check1.param,
            ft_scache_ptr,
            sizeof(bcm_flowtracker_tracking_param_type_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_type_t);

        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check1.min_value,
            ft_scache_ptr, sizeof(uint32));
        ft_scache_ptr += sizeof(uint32);

        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check1.max_value,
            ft_scache_ptr, sizeof(uint32));
        ft_scache_ptr += sizeof(uint32);

        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check1.operation,
            ft_scache_ptr, sizeof(bcm_flowtracker_check_operation_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_check_operation_t);

        if (recovered_ver >= BCM_WB_VERSION_1_2) {
            sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check1.mask_value,
                    ft_scache_ptr, sizeof(uint32));
            ft_scache_ptr += sizeof(uint32);

            sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check1.shift_value,
                    ft_scache_ptr, sizeof(uint8));
            ft_scache_ptr += sizeof(uint8);

            sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check1.state_transition_flags,
                    ft_scache_ptr, sizeof(uint32));
            ft_scache_ptr += sizeof(uint32);

            sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check1.num_drop_reason_group_id,
                    ft_scache_ptr, sizeof(uint16));
            ft_scache_ptr += sizeof(uint16);

            sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check1.drop_reason_group_id[0],
                    ft_scache_ptr, (sizeof(bcm_flowtracker_drop_reason_group_t) *
                        BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX));
            ft_scache_ptr += (sizeof(bcm_flowtracker_drop_reason_group_t) *
                                BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX);
        }

        /* Check-2 info */
        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.flags,
            ft_scache_ptr,
            sizeof(uint32));
        ft_scache_ptr += sizeof(uint32);

        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.param,
            ft_scache_ptr,
            sizeof(bcm_flowtracker_tracking_param_type_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_type_t);

        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.min_value,
            ft_scache_ptr, sizeof(uint32));
        ft_scache_ptr += sizeof(uint32);

        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.max_value,
            ft_scache_ptr, sizeof(uint32));
        ft_scache_ptr += sizeof(uint32);

        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.operation,
            ft_scache_ptr, sizeof(bcm_flowtracker_check_operation_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_check_operation_t);

        if (recovered_ver >= BCM_WB_VERSION_1_2) {

            sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.primary_check_id,
                    ft_scache_ptr, sizeof(bcm_flowtracker_check_t));
            ft_scache_ptr += sizeof(bcm_flowtracker_check_t);

            sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.mask_value,
                    ft_scache_ptr, sizeof(uint32));
            ft_scache_ptr += sizeof(uint32);

            sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.shift_value,
                    ft_scache_ptr, sizeof(uint8));
            ft_scache_ptr += sizeof(uint8);

            sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.state_transition_flags,
                    ft_scache_ptr, sizeof(uint32));
            ft_scache_ptr += sizeof(uint32);

            sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.num_drop_reason_group_id,
                    ft_scache_ptr, sizeof(uint16));
            ft_scache_ptr += sizeof(uint16);

            sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.drop_reason_group_id[0],
                    ft_scache_ptr, (sizeof(bcm_flowtracker_drop_reason_group_t) *
                        BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX));
            ft_scache_ptr += (sizeof(bcm_flowtracker_drop_reason_group_t) *
                    BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX);
        }

        /* Action info */
        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->action_info.param,
            ft_scache_ptr, sizeof(bcm_flowtracker_tracking_param_type_t));

        ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_type_t);

        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->action_info.action,
                ft_scache_ptr, sizeof(bcm_flowtracker_check_action_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_check_action_t);

       if (recovered_ver >= BCM_WB_VERSION_1_2) {

           sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->action_info.flags,
                   ft_scache_ptr, sizeof(uint32));
           ft_scache_ptr += sizeof(uint32);

           sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->action_info.mask_value,
                   ft_scache_ptr, sizeof(uint32));
           ft_scache_ptr += sizeof(uint32);

           sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->action_info.shift_value,
                   ft_scache_ptr, sizeof(uint8));
           ft_scache_ptr += sizeof(uint8);

           sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->action_info.weight,
                   ft_scache_ptr, sizeof(bcm_flowtracker_tracking_param_weight_t));
           ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_weight_t);
       }

        /* Export info */
        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->
            export_info.export_check_threshold, ft_scache_ptr,
            sizeof(uint32));
        ft_scache_ptr += sizeof(uint32);

        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->
            export_info.operation,
            ft_scache_ptr, sizeof(bcm_flowtracker_check_operation_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_check_operation_t);

        /* Delay Info */
        if (recovered_ver >= BCM_WB_VERSION_1_2) {

            sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->delay_info.src_ts,
                    ft_scache_ptr, sizeof(bcm_flowtracker_timestamp_source_t));
            ft_scache_ptr += sizeof(bcm_flowtracker_timestamp_source_t);

            sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->delay_info.dst_ts,
                    ft_scache_ptr, sizeof(bcm_flowtracker_timestamp_source_t));
            ft_scache_ptr += sizeof(bcm_flowtracker_timestamp_source_t);

            sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->delay_info.gran,
                    ft_scache_ptr, sizeof(uint8));
            ft_scache_ptr += sizeof(uint8);

            sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->delay_info.offset,
                    ft_scache_ptr, sizeof(int));
            ft_scache_ptr += sizeof(int);
        }
    } /* END : flowchecker information. */

    for (i = bcmiFtAluLoadTypeLoad8; i < bcmiFtAluLoadTypeNone; i++) {

        total_indexes_per_mem = 0;
        for (j = 0; j < BCMI_FT_ALU_LOAD_MEM_COUNT(unit, i); j++) {

            BCMI_FT_ALU_LOAD_MEM_SIZE(unit, i, j) =
                soc_mem_index_count(unit, BCMI_FT_ALU_LOAD_MEM(unit, i, j));

            total_indexes_per_mem += BCMI_FT_ALU_LOAD_MEM_SIZE(unit, i , j);

        }
        sal_memcpy(BCMI_FT_ALU_LOAD_BITMAP_INFO(unit, i), ft_scache_ptr,
                                    SHR_BITALLOCSIZE(total_indexes_per_mem));
        ft_scache_ptr += SHR_BITALLOCSIZE(total_indexes_per_mem);
    }

    LOG_INFO(BSL_LS_BCM_FLOWTRACKER,
       (BSL_META_U(unit, "Alu-Load state synced. \n")));

    /* Sync chip debug information. */
    sal_memcpy(&BCMI_FT_CHIP_DEBUG_ENABLE(unit), ft_scache_ptr,
        sizeof(int));

    ft_scache_ptr += sizeof(int);

    if (BCMI_FT_CHIP_DEBUG_ENABLE(unit)) {
        /* Sync chip debug information. */
        sal_memcpy(&BCMI_FT_CHIP_DEBUG_INFO(unit).num_debug_info, ft_scache_ptr,
            sizeof(int));
        ft_scache_ptr += sizeof(int);

        length = BCMI_FT_CHIP_DEBUG_INFO(unit).num_debug_info;

        BCMI_FT_CHIP_DEBUG_PARAM_INFO(unit) =
            (bcmi_ft_chip_debug_param_t *)
            sal_alloc((sizeof(bcmi_ft_chip_debug_param_t) * length),
            "flowtracker chip debug information.");

        /* Sync chip debug information. */
        sal_memcpy(BCMI_FT_CHIP_DEBUG_PARAM_INFO(unit), ft_scache_ptr,
            length * sizeof(bcmi_ft_chip_debug_param_t));

        ft_scache_ptr += (length * sizeof(bcmi_ft_chip_debug_param_t));

        sal_memcpy(BCMI_FT_CHIP_DEBUG_INFO(unit).alu32_mem_used, ft_scache_ptr,
            BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32) *
            sizeof(int));
        ft_scache_ptr +=
            BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32) *
            sizeof(int);

        sal_memcpy(BCMI_FT_CHIP_DEBUG_INFO(unit).alu16_mem_used, ft_scache_ptr,
            BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16) *
            sizeof(int));
        ft_scache_ptr +=
            BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16) *
            sizeof(int);

        sal_memcpy(BCMI_FT_CHIP_DEBUG_INFO(unit).alu32_ref_count, ft_scache_ptr,
            BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32) *
            sizeof(int));
        ft_scache_ptr +=
            BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32) *
            sizeof(int);

        sal_memcpy(BCMI_FT_CHIP_DEBUG_INFO(unit).alu16_ref_count, ft_scache_ptr,
            BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16) *
            sizeof(int));
        ft_scache_ptr +=
            BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16) *
            sizeof(int);
    }

    /* Sync chip drop reason group information. */
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        if (recovered_ver >= BCM_WB_VERSION_1_2) {
            sal_memcpy(BCMI_FT_DROP_REASON_ING_MMU_BITMAP(unit), ft_scache_ptr,
                  SHR_BITALLOCSIZE(BCMI_FT_DROP_REASON_ING_MMU_MAX_IDX(unit)));
            ft_scache_ptr +=
                  SHR_BITALLOCSIZE(BCMI_FT_DROP_REASON_ING_MMU_MAX_IDX(unit));

            sal_memcpy(BCMI_FT_DROP_REASON_EGR_BITMAP(unit), ft_scache_ptr,
                  SHR_BITALLOCSIZE(BCMI_FT_DROP_REASON_EGR_MAX_IDX(unit)));
            ft_scache_ptr +=
                  SHR_BITALLOCSIZE(BCMI_FT_DROP_REASON_EGR_MAX_IDX(unit));

            sal_memcpy(BCMI_FT_DROP_REASON_GROUP_ING_MMU_REFCOUNT_STATE(unit),
                    ft_scache_ptr,
                    (BCMI_FT_DROP_REASON_ING_MMU_MAX_IDX(unit) *
                     sizeof(uint16)));
            ft_scache_ptr +=
                   (BCMI_FT_DROP_REASON_ING_MMU_MAX_IDX(unit) * sizeof(uint16));

            sal_memcpy(BCMI_FT_DROP_REASON_GROUP_EGR_REFCOUNT_STATE(unit),
                    ft_scache_ptr,
                    (BCMI_FT_DROP_REASON_EGR_MAX_IDX(unit) *
                     sizeof(uint16)));
            ft_scache_ptr +=
                    (BCMI_FT_DROP_REASON_EGR_MAX_IDX(unit) * sizeof(uint16));
        }
    }
#endif

#ifdef BCM_FLOWTRACKER_V3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        if (recovered_ver >= BCM_WB_VERSION_1_2) {

            /* Recover Flowtracker Class information */
            group_type = bcmFlowtrackerGroupTypeAggregateMmu;
            sal_memcpy(BCMI_FT_AGG_CLASS_BITMAP(unit, group_type), ft_scache_ptr,
                    SHR_BITALLOCSIZE(BCMI_FT_AGG_CLASS_MAX_IDX(unit)));
            ft_scache_ptr += SHR_BITALLOCSIZE(BCMI_FT_AGG_CLASS_MAX_IDX(unit));

            group_type = bcmFlowtrackerGroupTypeAggregateEgress;
            sal_memcpy(BCMI_FT_AGG_CLASS_BITMAP(unit, group_type), ft_scache_ptr,
                    SHR_BITALLOCSIZE(BCMI_FT_AGG_CLASS_MAX_IDX(unit)));
            ft_scache_ptr += SHR_BITALLOCSIZE(BCMI_FT_AGG_CLASS_MAX_IDX(unit));

            sal_memcpy(BCMI_FT_AGG_CLASS_REF(unit), ft_scache_ptr,
                    (sizeof(int) * BCMI_FT_AGG_CLASS_MAX_IDX(unit)));
            ft_scache_ptr += (sizeof(int) * BCMI_FT_AGG_CLASS_MAX_IDX(unit));

            LOG_INFO(BSL_LS_BCM_FLOWTRACKER,
                    (BSL_META_U(unit, "Flowtracker Class info Synced. \n")));
        }
    }
#endif /* BCM_FLOWTRACKER_V3_SUPPORT */

    if (recovered_ver >= BCM_WB_VERSION_1_3) {
        bcmi_ft_recover_ver_1_3(unit, &ft_scache_ptr);
        bcmi_ft_udf_tracking_param_info_recover(unit, &ft_scache_ptr);
    }

    /*
     * Now that we have recovered all the state from scache,
     * we can independtly set the other sw state information
     * from this.
     */
    /* start getting the size of each element of group sw state. */

    for (id = 0; id < num_flowcheckers; id++) {
        if (!(SHR_BITGET(BCMI_FT_FLOWCHECKER_BITMAP(unit), id))) {
            continue;
        }

        /* Update ft_type_bmp */
        rv = bcmi_ft_check_ft_type_bmp_set(unit,
                                  BCMI_FT_FLOWCHECKER_INFO(unit, id));
        BCM_IF_ERROR_RETURN(rv);
    }

    for (group_type = bcmFlowtrackerGroupTypeNone;
                group_type < bcmFlowtrackerGroupTypeCount;
                group_type++) {

        num_groups = BCMI_FT_GROUP_TYPE_MAX_COUNT(unit, group_type);
        for(i = 0; i < num_groups; i++) {

            BCM_FLOWTRACKER_GROUP_ID_SET(id, group_type, i);

            if (bcmi_ft_group_is_invalid(unit, id)) {
                continue;
            }

            BCM_IF_ERROR_RETURN
                (bcmi_ft_group_sw_reinstall(unit, id));
        }
    }

    /* Export database warmboot  */
    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
            BCM_MODULE_FLOWTRACKER, BCM_WB_FT_PART_EXPORT);
    BCM_IF_ERROR_RETURN
        (bcmi_ft_export_warmboot_recover(unit, scache_handle));

    /* User entry state warmboot */
    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
            BCM_MODULE_FLOWTRACKER, BCM_WB_FT_PART_USER);
    BCM_IF_ERROR_RETURN(
            bcmi_ft_user_warmboot_recover(unit, scache_handle));

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flowtracker_sw_agg_periodic_export)) {
        if (BCMI_FT_GROUP_INFO(unit)->aft_periodic_interval) {
            bcmi_ftv3_aft_export_sw_scan_update(unit,
                BCMI_FT_GROUP_INFO(unit)->aft_periodic_interval);
        }
    }
#endif /* BCM_FLOWTRACKER_V3_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_alu_load_wb_recover
 * Purpose:
 *   Warmboot recovery for alu/load sw state.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 *   info           - (IN) ALU information of group.
 *   load16_indexes  - (IN) Load 16 indexes in group
 *   load8_indexes  - (IN) Load 8 indexes in group
 *   alu_load_type - (IN) ALU/LOAD type.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_alu_load_wb_recover(int unit,
                                  bcm_flowtracker_group_t id,
                                  bcmi_ft_group_alu_info_t *info,
                                  int *load16_indexes,
                                  int *load8_indexes,
                                  bcmi_ft_alu_load_type_t alu_load_type)
{
    int rv = BCM_E_NONE;
    int *indexes = NULL;
    bcmi_ft_group_alu_info_t *temp = NULL;
    bcmi_ft_group_alu_info_t *local = NULL;
    int index = -1;
    int total_load8_mem = TOTAL_GROUP_LOAD8_DATA_NUM;
    int total_load16_mem = TOTAL_GROUP_LOAD16_DATA_NUM;
    int i = 0, total_loads_mem = 0;

    if ((alu_load_type == bcmiFtAluLoadTypeLoad16) ||
        (alu_load_type == bcmiFtAluLoadTypeLoad8)) {

        indexes = ((alu_load_type == bcmiFtAluLoadTypeLoad16) ?
                    load16_indexes : load8_indexes);
        total_loads_mem = (alu_load_type == bcmiFtAluLoadTypeLoad16) ?
                        total_load16_mem : total_load8_mem;

        temp = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);

        for (i = 0; i < total_loads_mem; i++) {
            if (indexes[i] == -1) {
                break;
            }
            local = (&(temp[indexes[i]]));

            /* Set installed flag */
            local->flags |= BCMI_FT_ALU_LOAD_HW_INSTALLED;

            /* Set alu_ft_type */
            local->alu_ft_type = BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit, \
                    alu_load_type, 0, i);
        }

        local = (&(temp[indexes[0]]));
        index = local->alu_load_index;

    } else {
        index = info->alu_load_index;
        temp = info;

        /* Set install flag */
        info->flags |= BCMI_FT_ALU_LOAD_HW_INSTALLED;

        /* Set alu_ft_type */
        bcmi_ft_alu_load_mem_index_get(unit,
                index, alu_load_type, NULL, NULL, &i);

        info->alu_ft_type = BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit, \
                alu_load_type, i, 0);
    }

    /*
     * Insert the hash entry.
     * Below coverity error says that indexes can be passed NULL.
     * it is passed NULL only for ALU memories and are used
     * only for LOAD memories. so no harm.
     */
    /* coverity[var_deref_model] */
    rv = bcmi_ft_alu_hash_insert(unit, id, alu_load_type, temp, indexes,
        &index);

    if (BCM_FAILURE(rv)) {
        if (rv == BCM_E_EXISTS) {
            /* Increment ref count and set up group table. */
            BCMI_FT_ALU_LOAD_REFCOUNT_INCR(unit, alu_load_type, index);
            rv = BCM_E_NONE;
        } else {
            return rv;
        }
    } else {

        /* Assign the bitmap to this index. */
        SHR_BITSET(
            BCMI_FT_ALU_LOAD_BITMAP_INFO(unit, alu_load_type), index);

        /* Also increment the ref count on this index. */
        BCMI_FT_ALU_LOAD_REFCOUNT_INCR(unit, alu_load_type, index);
    }

    return rv;
}

/*
 * Function:
 *   bcmi_ft_pdd_policy_profile_recover
 * Purpose:
 *   Recover PDD profile entry.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_pdd_policy_profile_recover(int unit,
                                   bcm_flowtracker_group_t id)
{
    soc_mem_t mem;
    uint32 index = 0;
    bsc_kg_group_table_entry_t kg_group_entry;

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
                      MEM_BLOCK_ANY, id, &kg_group_entry));

    /* now write this index into the group. */
    index = soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm,
        ((uint32 *)&kg_group_entry), SESSION_DATA_TYPEf);

    /* retreive relevant pdd mem for device */
    bcmi_ft_alu_load_pdd_mem_get(unit, 0, &mem);

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem,
        MEM_BLOCK_ANY, index, (&(BCMI_FT_GROUP(unit, id)->pdd_entry))));

    /*
     * PDD entry is present in group table now. we do not maintain any state
     * for PDE profile entry and therefore we do not need to read it back.
     */

    return BCM_E_NONE;
}

int
bcmi_ft_group_pdd_pde_policy_recover(int unit,
                                     bcm_flowtracker_group_t id)
{
    /* Skip for Aggregate Flow groups */
    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(id)) {
        return BCM_E_NONE;
    }

    return bcmi_ft_group_pdd_policy_profile_recover(unit, id);
}

#endif /* BCM_WARM_BOOT_SUPPORT */

void
bcmi_ft_state_dump(int unit, int brief)
{
    int idx = 0;
    int num_groups = 0;
    bcm_flowtracker_group_type_t group_type;
    bcm_flowtracker_group_t group_id;

    LOG_INFO(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                    "================== Dumping Flowtracker State ================= \n")));

    for (group_type = bcmFlowtrackerGroupTypeNone;
            group_type < bcmFlowtrackerGroupTypeCount;
            group_type++) {

        num_groups = BCMI_FT_GROUP_TYPE_MAX_COUNT(unit, group_type);
        for (idx = 0; idx < num_groups; idx++) {
            if (SHR_BITGET(BCMI_FT_GROUP_BITMAP(unit, group_type), idx)) {

                BCM_FLOWTRACKER_GROUP_ID_SET(group_id, group_type, idx);

                LOG_CLI((BSL_META_U(unit, "\n\rFlowtracker Group : %d"
                                " (0x%x)\n\r"), group_id, group_id));
                bcmi_ft_group_state_dump(unit, group_id, brief);
            }
        }
    }

    /* Dump hash db. */
    bcmi_ft_alu_load_hash_dump(unit);
}

/*
 * Function:
 *   bcmi_ft_chip_debug_info_set
 * Purpose:
 *   Set chip debug information.
 * Parameters:
 *   unit             - (IN)    BCM device number
 *   num_debug_info   - (IN) actunal number of parameters
 *   info             - (IN) memory to send parameters.
 * Returns:
 *   BCM_E_XXX   - BCM error code.
 */
int bcmi_ft_chip_debug_info_set(int unit,
                                int num_info,
                                bcm_flowtracker_chip_debug_info_t *info)
{
    int i = 0;

    if (info == NULL) {
        return BCM_E_PARAM;
    }

    if (!BCMI_FT_CHIP_DEBUG_ENABLE(unit)) {
        return BCM_E_UNAVAIL;
    }

    if (num_info == 0) {
        if (BCMI_FT_CHIP_DEBUG_PARAM_INFO(unit) != NULL) {
            sal_free(BCMI_FT_CHIP_DEBUG_PARAM_INFO(unit));
            BCMI_FT_CHIP_DEBUG_PARAM_INFO(unit) = NULL;
        }
        return BCM_E_NONE;
    }

    if (BCMI_FT_CHIP_DEBUG_PARAM_INFO(unit) != NULL) {
        /*
         * There is already existing state. Check if
         * user is free to add these tracking informations.
         */
        BCM_IF_ERROR_RETURN
            (bcmi_ft_chip_debug_data_sanity(unit, num_info, info));
    }

    BCMI_FT_CHIP_DEBUG_INFO(unit).num_debug_info = num_info;

    BCMI_FT_CHIP_DEBUG_PARAM_INFO(unit) =
        (bcmi_ft_chip_debug_param_t *)
        sal_alloc((sizeof(bcmi_ft_chip_debug_param_t) *
                   num_info),
        "flowtracker chip debug information.");

    if (BCMI_FT_CHIP_DEBUG_PARAM_INFO(unit) == NULL) {
        return BCM_E_MEMORY;
    }

    for (i = 0; i < num_info; i++) {
        BCMI_FT_CHIP_DEBUG_PARAM(unit, i).param = info[i].param;
        BCMI_FT_CHIP_DEBUG_PARAM(unit, i).count = info[i].count;
        BCMI_FT_CHIP_DEBUG_PARAM(unit, i).mem_id = -1;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_chip_debug_info_get
 * Purpose:
 *   Get chip debug information.
 * Parameters:
 *   unit             - (IN)    BCM device number
 *   max_size   - (INOUT) maximum number that user wants.
 *   info             - (OUT) memory to send parameters.
 *   num_debug_info   - (OUT) actunal number of parameters
 * Returns:
 *   BCM_E_XXX   - BCM error code.
 */
int bcmi_ft_chip_debug_info_get(int unit,
                                int max_size,
                                bcm_flowtracker_chip_debug_info_t *info,
                                int *num_debug_info)
{
    int i = 0;
    int result = BCM_E_UNAVAIL;

    if (!BCMI_FT_CHIP_DEBUG_ENABLE(unit)) {
        return BCM_E_UNAVAIL;
    }

    /* If the max_alu_info is non zero and list == NULL, return
     * BCM_E_PARAM.
     */
    if ((max_size != 0) && (info == NULL)) {
        return BCM_E_PARAM;
    }

    *num_debug_info = BCMI_FT_CHIP_DEBUG_INFO(unit).num_debug_info;

    /* If max_alu_info == 0,
     * return the number of export elements in list_size
     * variable.
     */
    if (max_size == 0) {
        /* Nothing to be done here. */
        return BCM_E_NONE;
    }

    /* Pick the number of elements to send back. */
    *num_debug_info = (*num_debug_info < max_size) ?
                              (*num_debug_info) : max_size;

    for (i = 0; i < *num_debug_info; i++) {

        info[i].param = BCMI_FT_CHIP_DEBUG_PARAM(unit, i).param;

        if (BCMI_FT_CHIP_DEBUG_PARAM(unit, i).mem_id == -1) {

            info[i].count = BCMI_FT_CHIP_DEBUG_PARAM(unit, i).count;

        } else {

            result = bcmi_ft_chip_debug_counter_get(unit,
                         BCMI_FT_CHIP_DEBUG_PARAM(unit, i).mem_id,
                         BCMI_FT_CHIP_DEBUG_PARAM(unit, i).alu_type,
                         (&(info[i].count)));

            if (BCM_FAILURE(result)) {
                return result;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_type_mem_get
 * Purpose:
 *   Get table based on Group Type.
 * Parameters:
 *   unit - (IN) BCM device id
 *   group_type - (IN) Flowtracker Group Type
 *   mem  - (OUT) table
 * Returns:
 *   None.
 */
int
bcmi_ft_group_type_mem_get(
    int unit,
    int key_or_data,
    bcm_flowtracker_group_type_t group_type,
    soc_mem_t *mem)
{
    *mem = INVALIDm;

    switch(group_type) {
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
        case bcmFlowtrackerGroupTypeAggregateIngress:
            if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
                *mem = BSC_DG_AIFT_GROUP_TABLEm;
            }
            break;

        case bcmFlowtrackerGroupTypeAggregateMmu:
            if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
                *mem = BSC_DG_AMFT_GROUP_TABLEm;
            }
            break;

        case bcmFlowtrackerGroupTypeAggregateEgress:
            if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
                *mem = BSC_DG_AEFT_GROUP_TABLEm;
            }
            break;
#endif
        case bcmFlowtrackerGroupTypeNone:
        default:
            if (key_or_data) {
                *mem = BSC_KG_GROUP_TABLEm;
            } else {
                *mem = BSC_DG_GROUP_TABLEm;
            }
            break;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcmi_ft_type_support_check
 * Purpose:
 *   Check the support for ft type
 * Parameters:
 *   unit              - (IN) BCM device number.
 *   ft_type           - (IN) FT Type
 *   support           - (OUT) Support
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_type_support_check(int unit,
    bcmi_ft_type_t ft_type,
    int *support)
{
    *support = FALSE;
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        if (ft_type == bcmiFtTypeIFT) {
            *support = TRUE;
        }
    }
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        if ((ft_type == bcmiFtTypeMFT) ||
                (ft_type == bcmiFtTypeEFT)) {
            *support = TRUE;
        }
    }
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        if ((ft_type == bcmiFtTypeAIFT) ||
                (ft_type == bcmiFtTypeAMFT) ||
                (ft_type == bcmiFtTypeAEFT)) {
            *support = TRUE;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcmi_ft_control_set
 * Purpose:
 *  Set global flowtracker controls.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  type          - (IN) Flowtracker control type
 *  arg           - (IN) Argument
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int
bcmi_ft_control_set(
    int unit,
    bcm_flowtracker_control_type_t type,
    int arg)
{
    int rv = BCM_E_NONE;
    bsk_kmap_control_2_entry_t kmap2;
    soc_mem_t mem;
    int syn_offset, ack_offset;

    switch(type) {
        case bcmFlowtrackerControlTcpBidirEstablishedDoNotLearn:
            /*
             * KMAP for Key Direction
             * From BSK_KMAP_1_KEY_DIRECTION_IN_BUS format, extract and form 4bit value
             * b0 = bidirectional Flow
             * b1 = bidirectional Flow
             * b2 = SYN
             * b3 = ACK
             * Learn disable = 1, for 0008
             */

            BCM_IF_ERROR_RETURN(_bcm_field_ft_alu_data_config_get(unit, -1,
                        _bcmFieldFtAluDataTcpFlagsSYN, &syn_offset, NULL));

            BCM_IF_ERROR_RETURN(_bcm_field_ft_alu_data_config_get(unit, -1,
                        _bcmFieldFtAluDataTcpFlagsACK, &ack_offset, NULL));


            mem = BSK_KMAP_CONTROL_2m;
            sal_memset(&kmap2, 0, sizeof(bsk_kmap_control_2_entry_t));
            if (arg != 0) {
                soc_mem_field32_set(unit, mem, (uint32 *)&kmap2, KMAP_INPUT_SELECT_0f, 4);
                soc_mem_field32_set(unit, mem, (uint32 *)&kmap2, KMAP_INPUT_SELECT_1f, 4);
                soc_mem_field32_set(unit, mem, (uint32 *)&kmap2, KMAP_INPUT_SELECT_2f,
                        syn_offset);
                soc_mem_field32_set(unit, mem, (uint32 *)&kmap2, KMAP_INPUT_SELECT_3f,
                        ack_offset);

                soc_mem_field32_set(unit, mem, (uint32 *)&kmap2, KMAP_CONTROLf, 0x8);
            }

            BCM_IF_ERROR_RETURN(WRITE_BSK_KMAP_CONTROL_2m(unit,
                        MEM_BLOCK_ANY, 0, &kmap2));
            break;

        default:
            rv = BCM_E_PARAM;
    }

    return rv;
}

/*
 * Function:
 *  bcmi_ft_control_get
 * Purpose:
 *  Get global flowtracker controls.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  type          - (IN) Flowtracker control type
 *  arg           - (OUT) Argument
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */

int
bcmi_ft_control_get(
    int unit,
    bcm_flowtracker_control_type_t type,
    int *arg)
{
    int rv = BCM_E_NONE;
    uint32 data = 0;
    bsk_kmap_control_2_entry_t kmap2;
    soc_mem_t mem;

    if (arg == NULL) {
        return BCM_E_PARAM;
    }

    switch(type) {
        case bcmFlowtrackerControlTcpBidirEstablishedDoNotLearn:

            mem = BSK_KMAP_CONTROL_2m;
            sal_memset(&kmap2, 0, sizeof(bsk_kmap_control_2_entry_t));
            BCM_IF_ERROR_RETURN(READ_BSK_KMAP_CONTROL_2m(unit,
                        MEM_BLOCK_ANY, 0, &kmap2));

            data = soc_mem_field32_get(unit, mem, &kmap2, KMAP_CONTROLf);
            if (data != 0) {
                *arg = 1;
            }
            break;

        default:
            rv = BCM_E_PARAM;
    }

    return rv;
}

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
/*
 * Function:
 *   bcmi_ftv3_flow_transition_alu_trigger_set
 * Purpose:
 *   Get table based on Group Type.
 * Parameters:
 *   unit - (IN) BCM device id
 *   flag - (IN) State transition flag
 *   alu_bank - (IN) ALU memory bank
 *   set - (IN) Set or Reset
 * Returns:
 *   BCM_E_NONE - If successful
 *   BCM_E_xxx
 */
int
bcmi_ftv3_flow_transition_alu_trigger_set(
    int unit,
    uint32 flag,
    int alu_bank,
    int set)
{
    int i, hw_idx;
    int flag_found;
    uint32 alu_trigger;
    bsc_dt_flex_state_transition_table_entry_t table_entry;
    soc_mem_t mem = BSC_DT_FLEX_STATE_TRANSITION_TABLEm;

    sal_memset(&table_entry, 0,
            sizeof(bsc_dt_flex_state_transition_table_entry_t));

    i = 0;
    alu_trigger = 0;
    flag_found = FALSE;
    while (state_transition_data[i].current_state != -1) {
        if (state_transition_data[i].event_flags & flag) {

            hw_idx = i;

            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, mem, MEM_BLOCK_ANY, hw_idx, &table_entry));
            alu_trigger = soc_mem_field32_get(unit, mem,
                    (uint32 *) &table_entry, AGG_ALU_CTR_TRIGGERf);
            if (set) {
                alu_trigger |= (1 << alu_bank);
            } else {
                alu_trigger &= ~(0 << alu_bank);
            }
            soc_mem_field32_set(unit, mem, (uint32 *)&table_entry,
                    AGG_ALU_CTR_TRIGGERf, alu_trigger);
            BCM_IF_ERROR_RETURN(WRITE_BSC_DT_FLEX_STATE_TRANSITION_TABLEm
                    (unit, MEM_BLOCK_ANY, hw_idx, &table_entry));

            flag_found = TRUE;
        }
        i++;
    }

    if (flag_found == FALSE) {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

#endif /* BCM_FLOWTRACKER_V3_SUPPORT */

#else /* BCM_FLOWTRACKER_SUPPORT*/
typedef int bcmi_ft_base_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_FLOWTRACKER_SUPPORT */
