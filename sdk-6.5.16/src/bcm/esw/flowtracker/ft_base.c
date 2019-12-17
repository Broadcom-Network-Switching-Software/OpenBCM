/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        ft_base.c
 * Purpose:     The purpose of this file is to set flow
 *              tracker base methods.
 * Requires:
 */

#include <bcm_int/esw/flowtracker/ft_group.h>
#include <bcm_int/esw/flowtracker/ft_export.h>
#include <bcm_int/esw/flowtracker/ft_user.h>

#if defined(BCM_FLOWTRACKER_SUPPORT)

#ifdef BCM_WARM_BOOT_SUPPORT

#include <bcm_int/esw/switch.h>


#define BCM_WB_VERSION_1_1                SOC_SCACHE_VERSION(1,1)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_1


/* Base FT state - groups, alus, etc..  */
#define BCM_WB_FT_PART_BASE    0
/* Export related FT state  */
#define BCM_WB_FT_PART_EXPORT  1

#endif

uint8 alu_type_str[5][50] =
    {{"AluLoadTypeLoad8"},
     {"AluLoadTypeLoad16"},
     {"AluLoadTypeAlu16"},
     {"AluLoadTypeAlu32"},
     {"AluLoadTypeNone"}};

int ft_initialized[BCM_MAX_NUM_UNITS] = {0};

/* Group Sw state. */
extern
bcmi_ft_group_sw_info_t **bcmi_ft_group_sw_state[BCM_MAX_NUM_UNITS];
/* Group bitmap. */
extern
bcmi_ft_group_bitmap_t bcmi_ft_group_bitmap[BCM_MAX_NUM_UNITS];
/*Alu state for wamboot. */
/* There are 4 different types of memories. */
extern
alu_load_index_info_t
    alu_load_index_info[BCM_MAX_NUM_UNITS][BCMI_FT_ALU_LOAD_MEMS];


/* Global chip debug state. */
bcmi_ft_chip_debug_t chip_debug_info[BCM_MAX_NUM_UNITS];

/* Database for TCP state transtition. */
bcmi_ft_state_transtion_t state_transition_data[] =
    /* tcp_flags[2], cur_state, next_state, new_flow, bidir, ts. */

     /* TCP. unidirectional, SYN_RCVD.  Inactive to listen */
    {{{_bcmFieldFtAluDataTcpFlagsSYN, -1}, 0, 1, 1, 0, 0},
     /* TCP. unidirectional, ACK_RCVD. connection established. */
     {{_bcmFieldFtAluDataTcpFlagsACK, -1}, 1, 2, 0, 0, 1},
     /* TCP. Bidirectional, SYN+ACK_RCVD. Connection Established. */
     {{_bcmFieldFtAluDataTcpFlagsSYN, _bcmFieldFtAluDataTcpFlagsACK}, 0, 2, -1, -1, 1},
     /* TCP. Bidirectional, SYN+ACK_RCVD. Connection established  */
     {{_bcmFieldFtAluDataTcpFlagsSYN, _bcmFieldFtAluDataTcpFlagsACK}, 1, 2, -1, -1, 1},
     /* Establilshed to CLOSE_WAIT. */
     {{_bcmFieldFtAluDataTcpFlagsFIN, -1}, 2, 3, -1, -1, 0},
     /* TCP. SYN_WAIT to CLOSED. */
     {{_bcmFieldFtAluDataTcpFlagsRST, -1}, 1, 0, -1, -1, 1},
     /* TCP. CLOSED_WAIT to CLOSED. */
     {{_bcmFieldFtAluDataTcpFlagsFIN, -1}, 3, 0, -1, -1, 1},
     /* UDP. Connection Established. */
     {{-1, -1}, 0, 1, 1, -1, 0},
     /* UDP. Connection Established. */
     {{-1, -1}, 1, 0, 1, -1, 0},
     /* Last entry to mark the end. */
     {{-1, -1}, -1, -1, -1, -1, -1}};


/*
 * Function:
 *     bcmi_ft_chip_debug_init
 * Purpose:
 *     Initialize Chip Debug state for flowtracker.
 * Parameters:
 *     unit - (IN) BCM device id
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_chip_debug_init(int unit)
{

    chip_debug_info[unit].num_debug_info = 0;
    chip_debug_info[unit].param_info = NULL;

    sal_memset(chip_debug_info[unit].alu32_mem_used, 0,
        BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32) * sizeof(int));

    sal_memset(chip_debug_info[unit].alu16_mem_used, 0,
        BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16) * sizeof(int));

    sal_memset(chip_debug_info[unit].alu32_ref_count, 0,
        BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32) * sizeof(int));

    sal_memset(chip_debug_info[unit].alu16_ref_count, 0,
        BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16) * sizeof(int));


    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_flow_transition_state_init
 * Purpose:
 *     Initialize State transition state for flowtracker.
 * Parameters:
 *     unit - (IN) BCM device id
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_flow_transition_state_init(int unit)
{

    uint16 alu_data = 0; /* TCP flags container. */
    uint16 alu_data_mask = 0x0;
    uint32 state_transfmt[1];
    uint32 state_trans_maskfmt[1];
/*    int running_index = 0; */
    uint32 cur_state_mask = 0xFF;
    int offset, bidirectional, new_flow, cur_state, next_state, ts_enable, i;
    bsc_dt_flex_state_transition_table_entry_t table_entry;
    soc_mem_t mem;

    i = offset = bidirectional = new_flow = cur_state = next_state = 0;

    sal_memset(state_transfmt, 0, sizeof(uint32));
    sal_memset(state_trans_maskfmt, 0, sizeof(uint32));
    mem = BSC_DT_FLEX_STATE_TRANSITION_TABLEm;
    /*
     * bidirectional.
     *
     * INACTIVE(0) ->  SYN+direction => LEARN(1)
     * LEARN -> SYN_ACK + direction => ESTABLISHED(2).
     * INACTIVE -> SYN_ACK + direction => ESTABLISHED(2).
     * ESTABLISHED ->  <HIT> => ESTABLISHED.
     * ESTABLISHED ->  FIN => INACTIVE
     * ESTABLISHED ->  RST => INACTIVE
     * LEARN ->  FIN => INACTIVE
     * LEARN ->  RST => INACTIVE
     */
    while (state_transition_data[i].current_state != -1) {

        sal_memset(&table_entry, 0,
            sizeof(bsc_dt_flex_state_transition_table_entry_t));
        alu_data_mask = 0;
        offset = -1;

        if (state_transition_data[i].tcp_flags[0] != -1) {

            BCM_IF_ERROR_RETURN(_bcm_field_ft_alu_data_config_get(unit, -1,
                state_transition_data[i].tcp_flags[0], &offset, NULL));

            alu_data_mask = 0xFFFF;
            alu_data = (1 << offset);
        }

        if (state_transition_data[i].tcp_flags[1] != -1) {
            BCM_IF_ERROR_RETURN(_bcm_field_ft_alu_data_config_get(unit, -1,
                state_transition_data[i].tcp_flags[1], &offset, NULL));

            alu_data |= (1 << offset);
        }

        cur_state =  state_transition_data[i].current_state;
        next_state = state_transition_data[i].next_state;
        new_flow = state_transition_data[i].new_flow;
        bidirectional = state_transition_data[i].bidirectional;
        ts_enable = state_transition_data[i].ts_trigger;

        LOG_INFO(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            "cur_state : %d => next_state : %d\n"), cur_state, next_state));

        /* Set the common data. */
        if (new_flow != -1) {
            soc_format_field32_set(unit, BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
                state_transfmt, NEW_FLOW_LEARNEDf, new_flow);

            soc_format_field32_set(unit, BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
                state_trans_maskfmt, NEW_FLOW_LEARNEDf, 1);
        }

        /* Set bidirectional key and mask. */
        if (bidirectional != -1) {
            soc_format_field32_set(unit, BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
                state_transfmt, BIDIRECTIONAL_FLOWf, bidirectional);

            soc_format_field32_set(unit, BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
                state_trans_maskfmt, BIDIRECTIONAL_FLOWf, 1);
        }

        /* Till here mask and actual data is same. */
        sal_memcpy(state_trans_maskfmt, state_transfmt, sizeof(uint32));

        /* Set the data. */
        soc_format_field32_set(unit, BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
            state_transfmt, CURRENT_STATEf, cur_state);

        soc_format_field32_set(unit, BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
            state_transfmt, ALU_DATAf, alu_data);


        /* Set the mask. */
        soc_format_field32_set(unit, BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
            state_trans_maskfmt, CURRENT_STATEf, cur_state_mask);

        soc_format_field32_set(unit, BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
            state_trans_maskfmt, ALU_DATAf, alu_data_mask);

        soc_mem_field_set(unit, mem, (uint32 *)&table_entry, KEYf,
            state_transfmt);
        soc_mem_field_set(unit, mem, (uint32 *)&table_entry, MASKf,
            state_trans_maskfmt);

        soc_mem_field32_set(unit, mem, (uint32 *)&table_entry, NEXT_STATEf,
            next_state);

        if (ts_enable > 0) {
            if (next_state == 0) {
                /* Connection reset. */
                soc_mem_field32_set(unit, mem, (uint32 *)&table_entry,
                   OPAQUE_TS_1_TRIGGERf, 1);
            } else {
                /* Connection established. */
                soc_mem_field32_set(unit, mem, (uint32 *)&table_entry,
                   OPAQUE_TS_0_TRIGGERf, 1);
            }
        }

        /* set the valid bit for this entry. */
        soc_mem_field32_set(unit, mem, (uint32 *)&table_entry, VALIDf, 1);

        /* you may also want to set the export enable here. */
        BCM_IF_ERROR_RETURN(WRITE_BSC_DT_FLEX_STATE_TRANSITION_TABLEm
            (unit, MEM_BLOCK_ANY, i, &table_entry));

        i++;
    }

    return BCM_E_NONE;
}
#if 1
/*
 * Function:
 *     bcmi_ft_flow_direction_init
 * Purpose:
 *     Initialize Flow direction state for flowtracker.
 * Parameters:
 *     unit - (IN) BCM device id
 *
 * Returns:
 *     BCM_E_XXX
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

    soc_mem_field32_set(unit, mem, (uint32 *)&kmap0, KMAP_CONTROLf, 0x820);

    /* Write entry into memory. */
    BCM_IF_ERROR_RETURN(WRITE_BSK_KMAP_CONTROL_1m(unit,
        MEM_BLOCK_ANY, 0, &kmap1));


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
#endif

/*
 * Function:
 *     bcmi_ft_init
 * Purpose:
 *     Initialize Flowtracker state.
 * Parameters:
 *     unit - (IN) BCM device id
 *
 * Returns:
 *     None.
 */
int
bcmi_ft_init(int unit)
{
    int result = BCM_E_NONE;
    ft_key_key_attributes_entry_t ft_key_attributes;

    if(!SOC_WARM_BOOT(unit)) {
        bcmi_ft_cleanup(unit);
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
 *     bcmi_ft_cleanup
 * Purpose:
 *     Cleanup Flowtracker state.
 * Parameters:
 *     unit - (IN) BCM device id
 *
 * Returns:
 *     None.
 */
int
bcmi_ft_cleanup(int unit)
{

    _bcm_field_ft_group_invalidate(unit);

    bcmi_ft_alu_load_cleanup(unit);

    bcmi_ft_alu_hash_cleanup(unit);

    bcmi_ft_flowchecker_cleanup(unit);

    bcmi_ft_user_cleanup(unit);

    bcmi_ft_export_cleanup(unit);

    bcmi_ft_group_state_clear(unit);

    ft_initialized[unit] = 0;
    return BCM_E_NONE;

}

#ifdef BCM_WARM_BOOT_SUPPORT

/*
 * Function:
 *      bcmi_ft_wb_alloc_size
 * Purpose:
 *      Allocate persistent info memory for flowtracker module
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
void
bcmi_ft_wb_alloc_size(int unit, int *size)
{
    int alloc_size = 0;
    int num_groups = 0, id = 0, i=0, j=0;
    soc_mem_t mem;
    bcmi_ft_flowchecker_list_t **head = NULL;
    int length = 0;
    int num_flowcheckers = 0;
    int total_indexes_per_mem = 0;

    mem = BSC_KG_GROUP_TABLEm;

    /* total number of groups. */
    num_groups = soc_mem_index_count(unit, mem);

    /* Group Valid bits. */
    alloc_size += SHR_BITALLOCSIZE(num_groups);


    LOG_INFO(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            " Total flowtracker wb size requested =  %d \n"), alloc_size));

    /* start getting the size of each element of group sw state. */
    for(; id<num_groups; id++) {

        if (bcmi_ft_group_is_invalid(unit, id)) {
            continue;
        }

        /* Allocate space for bcmi_ft_group_config_t. */
        alloc_size += sizeof(int);
        length = BCMI_FT_GROUP_TRACK_PARAM_NUM(unit, id);

        for (i=0; i<length; i++) {
            alloc_size += sizeof(uint32);
            alloc_size += sizeof(bcm_flowtracker_tracking_param_type_t);
            alloc_size += sizeof(bcm_flowtracker_tracking_param_mask_t);
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
        for (i=0; i<length; i++) {
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

            /* Alu managament data. */
            alloc_size += sizeof(int);

            alloc_size += sizeof(bcmi_ft_alu_load_type_t);
        }

        /* Group extraction key. */
        alloc_size += sizeof(int);
        /* Calculate length for the session data. */
        length = BCMI_FT_GROUP_EXT_INFO(unit, id).num_key_info;
        for (i=0; i<length; i++) {
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

            /* Alu managament data. */
            alloc_size += sizeof(int);

            alloc_size += sizeof(bcmi_ft_alu_load_type_t);
        }

        /* FT Key mode. */
        alloc_size += sizeof(bcmi_ft_group_key_data_mode_t);
        /* FT Data mode. */
        alloc_size += sizeof(bcmi_ft_group_key_data_mode_t);

        /* num_ftfp_entries. */
        alloc_size += sizeof(int);
        /* Direction on this group. */
        alloc_size += sizeof(int);

        /* flowtracker control on this group. */
        alloc_size += sizeof(int);

        /* New learn control on this group. */
        alloc_size += sizeof(int);


        /* bcmi_ft_group_ftfp_data_t. key, data and alu_data profile. */
        alloc_size += sizeof(uint32);
        alloc_size += sizeof(uint32);
        alloc_size += sizeof(uint32);

        /* Now storing PDD entry as it can be read from hardware. */
        alloc_size +=
        (BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32) *
        sizeof(uint32));
        alloc_size += (BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16)
        * sizeof(uint32));

        /* Groups Flags */
        alloc_size += sizeof(uint32);

        /* Groups Validated status. */
        alloc_size += sizeof(int);

        /* Save Groups timestamp triggers. */
        alloc_size += sizeof(uint32);


    } /* End of per group sw state */


    /* Allocate state for flowtracker checks. */
    /* for now number of flowcheckers is same as groups. */

    /* Now allocated which group is being used. */
    num_flowcheckers = num_groups;

    /* Allocate state for primary and secondary check refcounts. */
    alloc_size += ((num_flowcheckers) * sizeof(uint32));

    /* Allocate space to save indexes for flowcheckers. */
    alloc_size += SHR_BITALLOCSIZE(num_flowcheckers);

    for (id=0; id<num_flowcheckers; id++) {
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
        alloc_size += sizeof(bcm_flowtracker_export_element_type_t);
        /* Check-1 min_value. */
        alloc_size += sizeof(int);
        /* Check-1 max_value. */
        alloc_size += sizeof(int);
        /* Check-1 operation. */
        alloc_size += sizeof(bcm_flowtracker_check_operation_t);

        /* Check-2 flags. */
        alloc_size += sizeof(uint32);
        /* Check-2 element. */
        alloc_size += sizeof(bcm_flowtracker_export_element_type_t);
        /* Check-2 min_value. */
        alloc_size += sizeof(int);
        /* Check-2 max_value. */
        alloc_size += sizeof(int);
        /* Check-2 operation. */
        alloc_size += sizeof(bcm_flowtracker_check_operation_t);

        /* Action element. */
        alloc_size += sizeof(bcm_flowtracker_export_element_type_t);
        /* action. */
        alloc_size += sizeof(bcm_flowtracker_check_action_t);

        /* export threshold. */
        alloc_size += sizeof(int);
        /* operation. */
        alloc_size += sizeof(bcm_flowtracker_check_operation_t);

    }

    /* Allocate space for alu memory. */
    for (i=bcmiFtAluLoadTypeLoad8; i<bcmiFtAluLoadTypeNone; i++) {
        total_indexes_per_mem = 0;
        for (j=0; j<alu_load_index_info[unit][i].total_mem_count; j++) {


            alu_load_index_info[unit][i].alu_load_info[j].mem_size =
                soc_mem_index_count(
                    unit, alu_load_index_info[unit][i].alu_load_info[j].mem);

            total_indexes_per_mem +=
                alu_load_index_info[unit][i].alu_load_info[j].mem_size;

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
        alloc_size += BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16) *
                      sizeof(int);

        /* ALU32 ref count. */
        alloc_size +=
        BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32) * sizeof(int);

        /* ALU16 ref count. */
        alloc_size += BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu16) *
                      sizeof(int);

        /* Space for installed status of group. */
        alloc_size += sizeof(int);
    }
    LOG_INFO(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            " Total flowtracker wb size requested =  %d \n"), alloc_size));

    /* Return the calculated size. */
    *size = alloc_size;

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

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FLOWTRACKER, BCM_WB_FT_PART_BASE);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                                 size, &ft_scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, NULL);

    if (BCM_E_NOT_FOUND == rv) {
        /* Proceed with Level 1 Warm Boot */
        rv = BCM_E_NONE;
    }

    if (BCM_SUCCESS(rv)) {
        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FLOWTRACKER, BCM_WB_FT_PART_EXPORT);
        rv = bcmi_ft_export_warmboot_alloc(unit, scache_handle);
    }

    return rv;

}

/*
 * Function:
 *     bcmi_ft_sync
 * Purpose:
 *     Sync flowtracker state.
 * Parameters:
 *     unit - (IN) BCM device id
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_sync(int unit)
{

    int size = 0;
    int num_groups = 0, id = 0, i=0, j=0;
    soc_mem_t mem;
    bcmi_ft_flowchecker_list_t **head = NULL;
    soc_scache_handle_t scache_handle;
    uint8 *ft_scache_ptr;
    bcmi_ft_flowchecker_list_t *fc_temp = NULL;
    int length = 0;
    int num_flowcheckers = 0;
    bcmi_ft_group_alu_info_t *temp=NULL;
    bcm_flowtracker_tracking_param_info_t *t_info=NULL;
    int total_indexes_per_mem = 0;

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
    for(; id<num_groups; id++) {

        if (bcmi_ft_group_is_invalid(unit, id)) {
            continue;
        }

        /* Save tracking parametrs. */
        sal_memcpy(ft_scache_ptr, &(BCMI_FT_GROUP_TRACK_PARAM_NUM(unit, id)),
            sizeof(int));
        ft_scache_ptr += sizeof(int);
        length = BCMI_FT_GROUP_TRACK_PARAM_NUM(unit, id);
        t_info = BCMI_FT_GROUP_TRACK_PARAM(unit, id);

        for (i=0; i<length; i++) {
           sal_memcpy(ft_scache_ptr, &t_info->flags, sizeof(uint32));
           ft_scache_ptr += sizeof(uint32);

           sal_memcpy(ft_scache_ptr, &t_info->param,
               sizeof(bcm_flowtracker_tracking_param_type_t));
           ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_type_t);

           sal_memcpy(ft_scache_ptr, &t_info->mask,
               sizeof(bcm_flowtracker_tracking_param_type_t));
           ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_mask_t);
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
        for (i=0; i<length; i++) {

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
        for (i=0; i<length; i++) {

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

        /* Direction on this group. */
        sal_memcpy(ft_scache_ptr, &BCMI_FT_GROUP_FTFP_DATA(unit, id).direction,
            sizeof(int));
        ft_scache_ptr += sizeof(int);

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
        sal_memcpy(ft_scache_ptr, (BCMI_FT_GROUP(unit, id)->ALU32_MEM_USE),
            (BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32) *
            sizeof(uint32)));

        ft_scache_ptr +=
            (BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32) *
             sizeof(uint32));

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


        /* Create Template Head list here. */


    } /* Save state for each Group. */

    LOG_INFO(BSL_LS_BCM_FLOWTRACKER,
        (BSL_META_U(unit, "Group state Synced. \n")));

    /* Now save state for flowcheckers. */

    /* Save ref-count state. */

    sal_memcpy(ft_scache_ptr,
        bcmi_ft_flowchecker_state[unit].fc_idx_refcount,
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
        ft_scache_ptr += sizeof(int);

        sal_memcpy(ft_scache_ptr,
            &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check1.param,
            sizeof(bcm_flowtracker_export_element_type_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_export_element_type_t);

        sal_memcpy(ft_scache_ptr,
            &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check1.min_value,
            sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(ft_scache_ptr,
            &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check1.max_value,
            sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(ft_scache_ptr,
            &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check1.operation,
             sizeof(bcm_flowtracker_check_operation_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_check_operation_t);

        /* Check-2 Info */
        sal_memcpy(ft_scache_ptr,
            &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.flags,
            sizeof(uint32));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(ft_scache_ptr,
            &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.param,
            sizeof(bcm_flowtracker_export_element_type_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_export_element_type_t);

        sal_memcpy(ft_scache_ptr,
            &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.min_value,
            sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(ft_scache_ptr,
            &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.max_value,
            sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(ft_scache_ptr,
            &BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.operation,
             sizeof(bcm_flowtracker_check_operation_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_check_operation_t);

        /* Action info */
        sal_memcpy(ft_scache_ptr,
            &BCMI_FT_FLOWCHECKER_INFO(unit, id)->action_info.param,
            sizeof(bcm_flowtracker_export_element_type_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_export_element_type_t);

        sal_memcpy(ft_scache_ptr,
            &BCMI_FT_FLOWCHECKER_INFO(unit, id)->action_info.action,
            sizeof(bcm_flowtracker_check_action_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_check_action_t);

        /* Export Info */
        sal_memcpy(ft_scache_ptr,
            &(BCMI_FT_FLOWCHECKER_INFO(unit, id)->
                export_info.export_check_threshold),
            sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(ft_scache_ptr,
            &(BCMI_FT_FLOWCHECKER_INFO(unit, id)->export_info.operation),
            sizeof(bcm_flowtracker_check_operation_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_check_operation_t);

    }

    LOG_INFO(BSL_LS_BCM_FLOWTRACKER,
       (BSL_META_U(unit, "Flowchecker state synced. \n")));

    for (i=bcmiFtAluLoadTypeLoad8; i<bcmiFtAluLoadTypeNone; i++) {

        total_indexes_per_mem = 0;
        for (j=0; j<alu_load_index_info[unit][i].total_mem_count; j++) {


            alu_load_index_info[unit][i].alu_load_info[j].mem_size =
                soc_mem_index_count(
                    unit, alu_load_index_info[unit][i].alu_load_info[j].mem);

            total_indexes_per_mem +=
                alu_load_index_info[unit][i].alu_load_info[j].mem_size;

        }
        sal_memcpy(ft_scache_ptr, alu_load_index_info[unit][i].index_bitmap,
            SHR_BITALLOCSIZE(total_indexes_per_mem));
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

    /* Export database warmboot  */
    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FLOWTRACKER, BCM_WB_FT_PART_EXPORT);
    BCM_IF_ERROR_RETURN(bcmi_ft_export_warmboot_sync(unit, scache_handle));


    return BCM_E_NONE;
}


/*
 * Function:
 *     bcmi_ft_group_sw_reinstall
 * Purpose:
 *     Recover Group's Sw state.
 * Parameters:
 *     unit - (IN) BCM device id
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_sw_reinstall(int unit, bcm_flowtracker_group_t id)
{
    int rv =  BCM_E_NONE;
    int num_alus_loads = 0, i=0;
    bcmi_ft_group_alu_info_t *temp = NULL;
    int load16_idz[TOTAL_GROUP_LOAD16_DATA_NUM];
    int load8_idz[TOTAL_GROUP_LOAD8_DATA_NUM];
    int num_load_16=0, num_load_8=0;
    bcmi_ft_alu_load_type_t type = bcmiFtAluLoadTypeNone;
    int total_load8_mem = TOTAL_GROUP_LOAD8_DATA_NUM;
    int total_load16_mem = TOTAL_GROUP_LOAD16_DATA_NUM;

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

       CLEANUP_ON_ERROR(rv);

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
       CLEANUP_ON_ERROR(rv);
    }

    if (load8_idz[0] != -1) {
        /* Configure the Load8 sw state. */
        rv = bcmi_ft_group_alu_load_wb_recover(unit, id, NULL,
            NULL, load8_idz, bcmiFtAluLoadTypeLoad8);

       CLEANUP_ON_ERROR(rv);

    }
    /* Add the pdd/pde policy entry. */
    rv = bcmi_ft_pdd_policy_profile_recover(unit, id);
    CLEANUP_ON_ERROR(rv);

    /* Add the pdd/pde policy entry. */
    rv = bcmi_ft_group_profile_recover(unit, id);
    CLEANUP_ON_ERROR(rv);

    /* Create template information list. */
    rv = bcmi_ft_group_template_data_create(unit, id, load16_idz, load8_idz);
    CLEANUP_ON_ERROR(rv);

    /* Add the Session profiles entry */
    rv = _bcm_field_ft_session_profiles_recover(unit, id);
    CLEANUP_ON_ERROR(rv);

    return BCM_E_NONE;

cleanup :
    bcmi_ft_group_hw_uninstall(unit, id);
    return rv;
}

/*
 * Function:
 *     bcmi_ft_group_profile_recover
 * Purpose:
 *     Recover profile entries of Group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_profile_recover(
              int unit, bcm_flowtracker_group_t id)
{
    uint32 index = 0;

    bsc_kg_group_table_entry_t kg_group_entry;
    bsc_dg_group_table_entry_t dg_group_entry;

    sal_memset(&kg_group_entry, 0, sizeof(bsc_kg_group_table_entry_t));
    sal_memset(&dg_group_entry, 0, sizeof(bsc_dg_group_table_entry_t));

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
        MEM_BLOCK_ANY, id, &kg_group_entry));
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
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

/*
 * Function:
 *     bcmi_ft_recover
 * Purpose:
 *     Recover flowtracker state.
 * Parameters:
 *     unit - (IN) BCM device id
 *
 * Returns:
 *     BCM_E_XXX
 */

int
bcmi_ft_recover(int unit)
{

    int num_groups = 0, id = 0, i=0, j=0;
    soc_mem_t mem;
    bcmi_ft_flowchecker_list_t **head = NULL;
    bsc_kg_group_table_entry_t kg_group_entry;
    soc_scache_handle_t scache_handle;
    uint8 *ft_scache_ptr;
    int length = 0;
    bcm_flowtracker_check_t flowchecker_id;
    int stable_size = 0;
    uint16 recovered_ver = 0;
    int num_flowcheckers = 0;
    int pdd_index = 0;
    bcmi_ft_group_alu_info_t * temp=NULL;
    bcm_flowtracker_tracking_param_info_t *t_info=NULL;
    int total_indexes_per_mem = 0;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FLOWTRACKER, BCM_WB_FT_PART_BASE);
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    if (stable_size <= 0) { /* Limited and Extended recovery only */
        /* No warmboot recovery possible. */
        return BCM_E_NONE;
    }

    /* Get the scache start address where the information is stored. */
    SOC_IF_ERROR_RETURN(_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
        0, &ft_scache_ptr, BCM_WB_DEFAULT_VERSION, &recovered_ver));

    mem = BSC_KG_GROUP_TABLEm;
    /* total number of groups. */
    num_groups = soc_mem_index_count(unit, mem);
    num_flowcheckers = num_groups;

    /* start getting the size of each element of group sw state. */
    for(; id<num_groups; id++) {

        sal_memset(&kg_group_entry, 0, sizeof(bsc_kg_group_table_entry_t));

        /* First read the group entry. */
        BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
            MEM_BLOCK_ANY, id, &kg_group_entry));

        /* Do it only for groups which are valid. Get VALID bit from hardware.*/
        if (!(soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm, &kg_group_entry,
            GROUP_VALIDf))) {

            continue;
        }

        /* now write this index into the group. */
        pdd_index = soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm,
        ((uint32 *)&kg_group_entry), SESSION_DATA_TYPEf);

        /* Set the group state here. */
        SHR_BITSET(BCMI_FT_GROUP_BITMAP(unit), id);

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

        for (i=0; i<length; i++) {
           sal_memcpy(&t_info->flags, ft_scache_ptr, sizeof(uint32));
           ft_scache_ptr += sizeof(uint32);

           sal_memcpy(&t_info->param, ft_scache_ptr,
               sizeof(bcm_flowtracker_tracking_param_type_t));
           ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_type_t);

           sal_memcpy(&t_info->mask, ft_scache_ptr,
               sizeof(bcm_flowtracker_tracking_param_type_t));
           ft_scache_ptr += sizeof(bcm_flowtracker_tracking_param_mask_t);
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
        for (i=0; i<length; i++) {

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
        for (i=0; i<length; i++) {

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

        /* Direction on this group. */
        sal_memcpy(&BCMI_FT_GROUP_FTFP_DATA(unit, id).direction, ft_scache_ptr,
            sizeof(int));
        ft_scache_ptr += sizeof(int);

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


        /* Recover ALU32 usage. */
        sal_memcpy((BCMI_FT_GROUP(unit, id)->ALU32_MEM_USE), ft_scache_ptr,
            (BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32) *
            sizeof(uint32)));
        ft_scache_ptr +=
        (BCMI_FT_ALU_LOAD_MEM_COUNT(unit, bcmiFtAluLoadTypeAlu32) *
         sizeof(uint32));

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


        if (BCMI_FT_GROUP_IS_VALIDATED(unit, id)) {
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, BSD_POLICY_ACTION_PROFILEm,
                MEM_BLOCK_ANY, pdd_index,
                (&(BCMI_FT_GROUP(unit, id)->pdd_entry))));

        }

    } /* END of group state. */
    /*
     * Start flowcheckr state recovery.
     */

    sal_memcpy(bcmi_ft_flowchecker_state[unit].fc_idx_refcount,
        ft_scache_ptr,
        (num_flowcheckers * sizeof(uint32)));
    ft_scache_ptr += (num_flowcheckers * sizeof(uint32));

    sal_memcpy(BCMI_FT_FLOWCHECKER_BITMAP(unit), ft_scache_ptr,
        SHR_BITALLOCSIZE(num_flowcheckers));
    ft_scache_ptr += SHR_BITALLOCSIZE(num_flowcheckers);

    for (id=0; id<num_flowcheckers; id++) {
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
        ft_scache_ptr += sizeof(int);

        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check1.param,
            ft_scache_ptr,
            sizeof(bcm_flowtracker_export_element_type_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_export_element_type_t);

        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check1.min_value,
            ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check1.max_value,
            ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check1.operation,
            ft_scache_ptr, sizeof(bcm_flowtracker_check_operation_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_check_operation_t);

        /* Check-2 info */
        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.flags,
            ft_scache_ptr,
            sizeof(uint32));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.param,
            ft_scache_ptr,
            sizeof(bcm_flowtracker_export_element_type_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_export_element_type_t);

        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.min_value,
            ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.max_value,
            ft_scache_ptr, sizeof(int));
        ft_scache_ptr += sizeof(int);

        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->check2.operation,
            ft_scache_ptr, sizeof(bcm_flowtracker_check_operation_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_check_operation_t);

        /* Action info */
        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->action_info.param,
            ft_scache_ptr, sizeof(bcm_flowtracker_export_element_type_t));

        ft_scache_ptr += sizeof(bcm_flowtracker_export_element_type_t);


        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->action_info.action,
            ft_scache_ptr, sizeof(bcm_flowtracker_check_action_t));
       ft_scache_ptr += sizeof(bcm_flowtracker_check_action_t);

        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->
            export_info.export_check_threshold, ft_scache_ptr,
            sizeof(int));
        ft_scache_ptr += sizeof(int);

        /* Export info */
        sal_memcpy(&BCMI_FT_FLOWCHECKER_INFO(unit, id)->
            export_info.operation,
            ft_scache_ptr, sizeof(bcm_flowtracker_check_operation_t));
        ft_scache_ptr += sizeof(bcm_flowtracker_check_operation_t);

    } /* END : flowchecker information. */

    for (i=bcmiFtAluLoadTypeLoad8; i<bcmiFtAluLoadTypeNone; i++) {

        total_indexes_per_mem = 0;
        for (j=0; j<alu_load_index_info[unit][i].total_mem_count; j++) {


            alu_load_index_info[unit][i].alu_load_info[j].mem_size =
                soc_mem_index_count(
                    unit, alu_load_index_info[unit][i].alu_load_info[j].mem);

            total_indexes_per_mem +=
                alu_load_index_info[unit][i].alu_load_info[j].mem_size;

        }
        sal_memcpy(alu_load_index_info[unit][i].index_bitmap, ft_scache_ptr,
            SHR_BITALLOCSIZE(total_indexes_per_mem));
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


    /*
     * Now that we have recovered all the state from scache,
     * we can independtly set the other sw state information
     * from this.
     */
    /* start getting the size of each element of group sw state. */
    for(id=0; id<num_groups; id++) {

        if (bcmi_ft_group_is_invalid(unit, id)) {
            continue;
        }

        BCM_IF_ERROR_RETURN(bcmi_ft_group_sw_reinstall(unit, id));
    }

    /* Export database warmboot  */
    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FLOWTRACKER, BCM_WB_FT_PART_EXPORT);
    BCM_IF_ERROR_RETURN(bcmi_ft_export_warmboot_recover(unit, scache_handle));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_group_alu_load_wb_recover
 * Purpose:
 *     Warmboot recovery for alu/load sw state.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     info           - (IN) ALU information of group.
 *     load16_indexes  - (IN) Load 16 indexes in group
 *     load8_indexes  - (IN) Load 8 indexes in group
 *     alu_load_type - (IN) ALU/LOAD type.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_alu_load_wb_recover(
              int unit, bcm_flowtracker_group_t id,
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

    if ((alu_load_type == bcmiFtAluLoadTypeLoad16) ||
        (alu_load_type == bcmiFtAluLoadTypeLoad8)) {

        indexes = ((alu_load_type == bcmiFtAluLoadTypeLoad16) ?
                    load16_indexes : load8_indexes);

        temp = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);

        local = (&(temp[indexes[0]]));
        index = local->alu_load_index;

    } else {
        index = info->alu_load_index;
        temp = info;
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
/*            bcmi_ft_alu_load_mem_index_get
                (unit, index, bcmiFtAluLoadTypeAlu16, &new_index, &mem, &a_idx);*/
            rv = BCM_E_NONE;
        } else {
            return rv;
        }
    } else {


        /* Assign the bitmap to this index. */
        SHR_BITSET((BCMI_FT_ALU_LOAD_BITMAP(unit, alu_load_type)),
            index);

        /* Also increment the ref count on this index. */
        BCMI_FT_ALU_LOAD_REFCOUNT_INCR(unit, alu_load_type, index);
    }

    return rv;
}

/*
 * Function:
 *     bcmi_ft_pdd_policy_profile_recover
 * Purpose:
 *     Recover PDD profile entry.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_pdd_policy_profile_recover(
              int unit, bcm_flowtracker_group_t id)
{


    bsc_kg_group_table_entry_t kg_group_entry;
    uint32 index = 0;

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_KG_GROUP_TABLEm,
        MEM_BLOCK_ANY, id, &kg_group_entry));


    /* now write this index into the group. */
    index = soc_mem_field32_get(unit, BSC_KG_GROUP_TABLEm,
        ((uint32 *)&kg_group_entry), SESSION_DATA_TYPEf);


    BCM_IF_ERROR_RETURN(soc_mem_read(unit, BSD_POLICY_ACTION_PROFILEm,
        MEM_BLOCK_ANY, index, (&(BCMI_FT_GROUP(unit, id)->pdd_entry))));

    /*
     * PDD entry is present in group table now. we do not maintain any state
     * for PDE profile entry and therefore we do not need to read it back.
     */

    return BCM_E_NONE;
}

#endif /* BCM_WARM_BOOT_SUPPORT */
#if 0
void
bcmi_ft_group_state_dump(
    int unit,
    bcm_flowtracker_group_t id)
{


    bcmi_ft_group_alu_info_t *temp = NULL;
    int j =0, num_alus_loads;

    if (bcmi_ft_group_is_invalid(unit, id)) {
        LOG_INFO(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            " Group id %d is not valid.\n"), id));

        return;
    }

    LOG_INFO(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
        "======================================================== \n")));

    LOG_INFO(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
        "Printing State for Group : %d \n"), id));
    LOG_INFO(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
        "======================================================== \n")));


    temp = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);
    num_alus_loads = (BCMI_FT_GROUP_EXT_INFO(unit, id)).num_data_info;


    for (j=0; j<num_alus_loads; j++) {
       LOG_INFO(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
           "location = %d, length = %d, is_alu = %d \n"),
           temp->key.location, temp->key.length, temp->key.is_alu));

        LOG_INFO(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            "Alu/load combined index = %d, type = %s\n"),
            temp->alu_load_index, alu_type_str[temp->alu_load_type]));

        temp++;

    }
}
#endif
void
bcmi_ft_state_dump(int unit)
{
    int num_groups = 0, id = 0;
    soc_mem_t mem;

    /* Group memory. */
    mem = BSC_KG_GROUP_TABLEm;

    /* total number of groups. */
    num_groups = soc_mem_index_count(unit, mem);

    LOG_INFO(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
        "==================Dumping Flowtracker State ================= \n")));

    if (BCMI_FT_CHIP_DEBUG_ENABLE(unit)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            "Chip Level Debugging is Enabled. \n")));
    } else {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            "Chip Level Debugging is Disabled. \n")));
    }

    for(; id<num_groups; id++) {

        if (bcmi_ft_group_is_invalid(unit, id)) {
            continue;
        }

        bcmi_ft_group_state_dump(unit, id);
    }

}

/*
 * Function:
 *      bcmi_ft_chip_debug_info_set
 * Purpose:
 *      Set chip debug information.
 * Parameters:
 *      unit             - (IN)    BCM device number
 *      num_debug_info   - (IN) actunal number of parameters
 *      info             - (IN) memory to send parameters.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcmi_ft_chip_debug_info_set(
        int unit,
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
         * There is already existing state.
         * Check if user is free to add these tracking informations.
         */
        BCM_IF_ERROR_RETURN(
            bcmi_ft_chip_debug_data_sanity(unit, num_info, info));
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

    for (i = 0; i<num_info; i++) {
        BCMI_FT_CHIP_DEBUG_PARAM(unit, i).param = info[i].param;
        BCMI_FT_CHIP_DEBUG_PARAM(unit, i).count = info[i].count;
        BCMI_FT_CHIP_DEBUG_PARAM(unit, i).mem_id = -1;
    }

    return BCM_E_NONE;
}



/*
 * Function:
 *      bcmi_ft_chip_debug_info_get
 * Purpose:
 *      Get chip debug information.
 * Parameters:
 *      unit             - (IN)    BCM device number
 *      max_size   - (INOUT) maximum number that user wants.
 *      info             - (OUT) memory to send parameters.
 *      num_debug_info   - (OUT) actunal number of parameters
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcmi_ft_chip_debug_info_get(
        int unit,
        int max_size,
        bcm_flowtracker_chip_debug_info_t *info,
        int *num_debug_info)
{
    int result = BCM_E_UNAVAIL;
    int i = 0;

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

    for (i = 0; i<*num_debug_info; i++) {
        info[i].param = BCMI_FT_CHIP_DEBUG_PARAM(unit, i).param;
        if (BCMI_FT_CHIP_DEBUG_PARAM(unit, i).mem_id == -1) {
            info[i].count = BCMI_FT_CHIP_DEBUG_PARAM(unit, i).count;
        } else {
            result = bcmi_ft_chip_debug_counter_get(unit,
                         BCMI_FT_CHIP_DEBUG_PARAM(unit, i).mem_id,
                         BCMI_FT_CHIP_DEBUG_PARAM(unit, i).alu_type,
                         (&(info[i].count)));

            if (BCM_FAILURE(result))
                return result;
            }

    }

    return BCM_E_NONE;
}

#if 0
int
bcmi_ft_group_ts_state_recover(
        int unit,
        bcm_flowtracker_group_t id)
{

    bsc_dg_group_timestamp_profile_entry_t entry;
    bsc_dg_group_table_entry_t dg_group_entry;
    uint32 index=0;
    uint32 fmt[1];


    soc_field_t ts_data_fields[4] =
                    {DATA_0f, DATA_1f, DATA_2f, DATA_3f};


    /* Initialize the timestamp profile entry. */
    sal_memset(&entry, 0,
        sizeof(bsc_dg_group_timestamp_profile_entry_t));
    /* Initialize group entry. */
    sal_memset(&dg_group_entry, 0, sizeof(bsc_dg_group_table_entry_t));

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
            MEM_BLOCK_ANY, id, &dg_group_entry));

    /* now write this new index into the group. */
    index = soc_mem_field32_get(unit, BSC_DG_GROUP_TABLEm,
        ((uint32 *)&dg_group_entry), TIMESTAMP_PROFILE_IDXf, index);

    if (index == 0) {
        return BCM_E_NONE;
    }

    /* First read the group entry. */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, BSC_DG_GROUP_TIMESTAMP_PROFILEm,
            MEM_BLOCK_ANY, index, &entry));

#endif




#if 0
bcmi_ft_group_param_type_t
bcmi_ft_group_get_param_type(
        int unit,
        soc_field_t fid)
{

    bcmi_ft_group_param_type_t param;

    param = bcmiFtGroupParamTypeTracking;

    switch (fid) {
        case bcmFlowtrackerTrackingParamTypeTimestampNewLearn:
            param = bcmiFtGroupParamTypeTsNewLearn;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampFlowStart:
            param = bcmiFtGroupParamTypeTsFlowStart;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampFlowEnd:
            param = bcmiFtGroupParamTypeTsFlowEnd;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1:
            param = bcmiFtGroupParamTypeTsCheckEvent1;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2:
            param = bcmiFtGroupParamTypeTsCheckEvent2;
            break;
        default :
            param = bcmiFtGroupParamTypeTracking;
    }

    return param;
}


bcm_flowtracker_tracking_param_type_t
bcmi_ft_group_get_tracking_param_(
        int unit,
        bcmi_ft_group_param_type_t pram_type)
{

    bcmi_ft_group_param_type_t param;

    param = bcmFlowtrackerTrackingParamTypeNone;

    switch (pram_type) {
        case bcmiFtGroupParamTypeTsNewLearn:
            param = bcmFlowtrackerTrackingParamTypeTimestampNewLearn
            break;
        case FtGroupParamTypeTsFlowStart:
            param bcmFlowtrackerTrackingParamTypeTimestampFlowStart;
            break;
        case bcmiFtGroupParamTypeTsFlowEnd
            param = bcmFlowtrackerTrackingParamTypeTimestampFlowEnd:
            break;
        case bcmiFtGroupParamTypeTsCheckEvent1:
            param = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1:
            break;
        case bcmiFtGroupParamTypeTsCheckEvent2:
            param = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2:
            break;
        default :
            param = bcmFlowtrackerTrackingParamTypeNone;
    }

    return param;
}



bcmi_ft_group_param_type_t
bcmi_ft_group_get_ts_type(
        int unit,
        bcm_flowtracker_group_t id,
        soc_field_t fid)
{
    int i=0;

    for (; i<BCMI_FT_GROUP_MAX_TS_TYPE; i++) {
        if (fid == bcmi_ft_group_ts_info[i].field_id) {
            return bcmi_ft_group_get_param_type(unit,
                 bcmi_ft_group_ts_info[i].ts_param);
         }
    }

    return bcmiFtGroupParamTypeTracking;

}
#endif

#else /* BCM_FLOWTRCAKER_SUPPORT*/
int bcmi_ft_base_not_empty;
#endif /* BCM_FLOWTRCAKER_SUPPORT */
