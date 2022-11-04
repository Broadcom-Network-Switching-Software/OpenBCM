/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
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

bcmi_ft_state_transition_info_t state_trans_info[BCM_MAX_NUM_UNITS];

/* Structure to use for traversal in state transition table */
typedef struct bcmi_ft_state_transition_tbl_trv_data_s {
    int max_count;
    bcm_flowtracker_state_transition_key_t *key;
    bcm_flowtracker_state_transition_policy_t *policy;
    int actual_count;
    int current_idx;
    uint32 entry_words;
    int found;
    int match_index;
    int free_index;
    bsc_dt_flex_state_transition_table_entry_t match_hw_entry;
} bcmi_ft_state_transition_tbl_trv_data_t;

typedef int (*bcmi_ft_state_transition_tbl_callack_f)(int unit,
                soc_mem_t mem, int index, void *entry,
                bcmi_ft_state_transition_tbl_trv_data_t *trv_data,
                int *done);

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
 *  bcmi_ft_state_transition_key_validate
 * Purpose:
 *  Validate user input of state transition key.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  key           - (IN) State transition key
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
STATIC int
bcmi_ft_state_transition_key_validate(int unit,
    bcm_flowtracker_state_transition_key_t *key)
{
    uint32 max_size;
    soc_format_t fmt;
    soc_field_t field;

    fmt = BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt;

    field = GROUP_CTRL_IDf;
    max_size = ((1 << soc_format_field_length(unit, fmt, field)) - 1);
    if (key->control_id > max_size) {
        return BCM_E_PARAM;
    }

    if (key->bidirectional_flow != -1) {
        field = BIDIRECTIONAL_FLOWf;
        max_size = ((1 << soc_format_field_length(unit, fmt, field)) - 1);
        if (key->bidirectional_flow > max_size) {
            return BCM_E_PARAM;
        }
    }

    if (key->new_learn != -1) {
        field = NEW_FLOW_LEARNEDf;
        max_size = ((1 << soc_format_field_length(unit, fmt, field)) - 1);
        if (key->new_learn > max_size) {
            return BCM_E_PARAM;
        }
    }
    field = CURRENT_STATEf;
    max_size = ((1 << soc_format_field_length(unit, fmt, field)) - 1);
    if (key->current_state > max_size) {
        return BCM_E_PARAM;
    }

    field = ALU_DATAf;
    max_size = ((1 << soc_format_field_length(unit, fmt, field)) - 1);
    if ((key->tcp_control > max_size) || (key->tcp_control_mask > max_size)) {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcmi_ft_state_transition_policy_validate
 * Purpose:
 *  Validate user input of state transition policy.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  key           - (IN) State transition key
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
STATIC int
bcmi_ft_state_transition_policy_validate(int unit,
    bcm_flowtracker_state_transition_policy_t *policy)
{
    uint32 max_size;
    soc_mem_t mem;
    soc_field_t field;

    mem = BSC_DT_FLEX_STATE_TRANSITION_TABLEm;

    field = NEXT_STATEf;
    max_size = ((1 << soc_mem_field_length(unit, mem, field)) - 1);
    if (policy->next_state > max_size) {
        return BCM_E_PARAM;
    }

    field = EXPORT_ENf;
    max_size = ((1 << soc_mem_field_length(unit, mem, field)) - 1);
    if (policy->export_trigger > max_size) {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcmi_ft_state_transition_key_to_hw_entry
 * Purpose:
 *  Set state transition key struct to hw entry.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  key           - (IN) State transition key
 *  hw_entry      - (OUT) H/w entry
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
STATIC int
bcmi_ft_state_transition_key_to_hw_entry(int unit,
    bcm_flowtracker_state_transition_key_t *key,
    bsc_dt_flex_state_transition_table_entry_t *hw_entry)
{
    int extract_pos, tcp_flag_idx;
    soc_mem_t mem;
    uint32 data = 0;
    uint32 mask = 0;
    uint32 state_trans_keyfmt[1] = {0};
    uint32 state_trans_maskfmt[1] = {0};

    mem = BSC_DT_FLEX_STATE_TRANSITION_TABLEm;

    data = key->control_id;
    soc_format_field32_set(unit, BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
            state_trans_keyfmt, GROUP_CTRL_IDf, data);

    mask = 0x3;
    soc_format_field32_set(unit, BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
            state_trans_maskfmt, GROUP_CTRL_IDf, mask);

    if (key->bidirectional_flow != -1) {
        data = key->bidirectional_flow;
        soc_format_field32_set(unit, BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
                state_trans_keyfmt, BIDIRECTIONAL_FLOWf, data);

        mask = 0x1;
        soc_format_field32_set(unit, BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
                state_trans_maskfmt, BIDIRECTIONAL_FLOWf, mask);
    }

    if (key->new_learn != -1) {
        data = key->new_learn;
        soc_format_field32_set(unit, BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
                state_trans_keyfmt, NEW_FLOW_LEARNEDf, data);

        mask = 0x1;
        soc_format_field32_set(unit, BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
                state_trans_maskfmt, NEW_FLOW_LEARNEDf, mask);
    }

    data = key->current_state;
    soc_format_field32_set(unit, BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
            state_trans_keyfmt, CURRENT_STATEf, data);

    mask = 0xFF;
    soc_format_field32_set(unit, BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
            state_trans_maskfmt, CURRENT_STATEf, mask);

    data = 0;
    mask = 0;
    for (tcp_flag_idx = 0; tcp_flag_idx < 8; tcp_flag_idx++) {
        if (!(key->tcp_control & (1 << tcp_flag_idx))) {
            continue;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_field_ft_alu_data_extract_pos_get(unit, tcp_flag_idx,
                                                    &extract_pos));
        data |= (1 << extract_pos);
    }

    for (tcp_flag_idx = 0; tcp_flag_idx < 8; tcp_flag_idx++) {
        if (!(key->tcp_control_mask & (1 << tcp_flag_idx))) {
            continue;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_field_ft_alu_data_extract_pos_get(unit, tcp_flag_idx,
                                                    &extract_pos));
        mask |= (1 << extract_pos);
    }

    soc_format_field32_set(unit, BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
            state_trans_keyfmt, ALU_DATAf, data);

    soc_format_field32_set(unit, BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
                state_trans_maskfmt, ALU_DATAf, mask);

    soc_mem_field_set(unit, mem, (uint32 *) hw_entry,
            KEYf, state_trans_keyfmt);
    soc_mem_field_set(unit, mem, (uint32 *) hw_entry,
            MASKf, state_trans_maskfmt);

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcmi_ft_state_transition_hw_entry_to_key
 * Purpose:
 *  Set state transition key struct from hw entry.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  index         - (IN) Index
 *  hw_entry      - (IN) H/w entry
 *  key           - (OUT) State transition key
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
STATIC int
bcmi_ft_state_transition_hw_entry_to_key(int unit,
    int index,
    bsc_dt_flex_state_transition_table_entry_t *hw_entry,
    bcm_flowtracker_state_transition_key_t *key)
{
    int bit, tcp_flag_idx;
    soc_mem_t mem;
    uint32 data = 0;
    uint32 mask = 0;
    uint32 state_trans_keyfmt[1] = {0};
    uint32 state_trans_maskfmt[1] = {0};
    bcmi_ft_state_transition_entry_t *sw_ent = NULL;

    mem = BSC_DT_FLEX_STATE_TRANSITION_TABLEm;

    bcm_flowtracker_state_transition_key_t_init(key);

    soc_mem_field_get(unit, mem, (uint32 *) hw_entry,
            KEYf, state_trans_keyfmt);
    soc_mem_field_get(unit, mem, (uint32 *) hw_entry,
            MASKf, state_trans_maskfmt);

    if (BCMI_FT_STATE_TRANSITION_SW_ENTRY_ARR(unit) != NULL) {
        sw_ent = BCMI_FT_STATE_TRANSITION_SW_ENTRY(unit, index);
        if (sw_ent != NULL) {
            key->priority = sw_ent->priority;
        }
    }
    data = soc_format_field32_get(unit,
            BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
            state_trans_keyfmt, GROUP_CTRL_IDf);
    key->control_id = data;

    data = soc_format_field32_get(unit,
            BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
            state_trans_keyfmt, BIDIRECTIONAL_FLOWf);
    mask = soc_format_field32_get(unit,
            BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
            state_trans_maskfmt, BIDIRECTIONAL_FLOWf);
    if (mask != 0) {
        key->bidirectional_flow = data;
    } else {
        key->bidirectional_flow = -1;
    }

    data = soc_format_field32_get(unit,
            BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
            state_trans_keyfmt, NEW_FLOW_LEARNEDf);
    mask = soc_format_field32_get(unit,
            BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
            state_trans_maskfmt, NEW_FLOW_LEARNEDf);
    if (mask != 0) {
        key->new_learn = data;
    } else {
        key->new_learn = -1;
    }

    data = soc_format_field32_get(unit,
            BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
            state_trans_keyfmt, CURRENT_STATEf);
    key->current_state = data;

    data = soc_format_field32_get(unit,
            BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
            state_trans_keyfmt, ALU_DATAf);
    mask = soc_format_field32_get(unit,
            BSC_DT_FLEX_STATE_TRANSITION_IN_BUSfmt,
            state_trans_maskfmt, ALU_DATAf);

    for (bit = 0; bit < 8; bit++) {
        if (data & (1 << bit)) {
            BCM_IF_ERROR_RETURN
                (_bcm_field_ft_alu_data_offset_get(unit, bit,
                                                   &tcp_flag_idx));

            key->tcp_control |= (1 << tcp_flag_idx);
        }
    }

    for (bit = 0; bit < 8; bit++) {
        if (mask & (1 << bit)) {
            BCM_IF_ERROR_RETURN
                (_bcm_field_ft_alu_data_offset_get(unit, bit,
                                                   &tcp_flag_idx));

            key->tcp_control_mask |= (1 << tcp_flag_idx);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcmi_ft_state_transition_policy_to_hw_entry
 * Purpose:
 *  Set state transition policy struct to hw entry.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  policy        - (IN) State transition policy
 *  hw_entry      - (OUT) H/w entry
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
STATIC int
bcmi_ft_state_transition_policy_to_hw_entry(int unit,
    bcm_flowtracker_state_transition_policy_t *policy,
    bsc_dt_flex_state_transition_table_entry_t *hw_entry)
{
    soc_mem_t mem;
    uint32 data = 0;

    mem = BSC_DT_FLEX_STATE_TRANSITION_TABLEm;

    data = policy->next_state;
    soc_mem_field32_set(unit, mem, (void *) hw_entry, NEXT_STATEf, data);

    data = policy->export_trigger;
    soc_mem_field32_set(unit, mem, (void *) hw_entry, EXPORT_ENf, data);

    if (policy->ts_triggers &
            BCM_FLOWTRACKER_STATE_TRANSITION_TIMESTAMP_FLOW_START) {
        data = 0x1;
        soc_mem_field32_set(unit, mem, (void *) hw_entry,
                OPAQUE_TS_0_TRIGGERf, data);
        soc_mem_field32_set(unit, mem, (void *) hw_entry,
                OPAQUE_CTR_0_TRIGGERf, data);
    }

    if (policy->ts_triggers &
            BCM_FLOWTRACKER_STATE_TRANSITION_TIMESTAMP_FLOW_END) {
        data = 0x1;
        soc_mem_field32_set(unit, mem, (void *) hw_entry,
                OPAQUE_TS_1_TRIGGERf, data);
        soc_mem_field32_set(unit, mem, (void *) hw_entry,
                OPAQUE_CTR_1_TRIGGERf, data);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcmi_ft_state_transition_hw_entry_to_policy
 * Purpose:
 *  Set state transition policy struct from hw entry.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  hw_entry      - (IN) H/w entry
 *  policy        - (OUT) State transition policy
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
STATIC int
bcmi_ft_state_transition_hw_entry_to_policy(int unit,
    bsc_dt_flex_state_transition_table_entry_t *hw_entry,
    bcm_flowtracker_state_transition_policy_t *policy)
{
    soc_mem_t mem;
    uint32 data = 0;

    mem = BSC_DT_FLEX_STATE_TRANSITION_TABLEm;

    bcm_flowtracker_state_transition_policy_t_init(policy);

    data = soc_mem_field32_get(unit, mem, hw_entry, NEXT_STATEf);
    policy->next_state = data;

    data = soc_mem_field32_get(unit, mem, hw_entry, EXPORT_ENf);
    policy->export_trigger = data;

    data = soc_mem_field32_get(unit, mem, hw_entry, OPAQUE_TS_0_TRIGGERf);
    if (data != 0) {
        policy->ts_triggers |=
            BCM_FLOWTRACKER_STATE_TRANSITION_TIMESTAMP_FLOW_START;
    }

    data = soc_mem_field32_get(unit, mem, hw_entry, OPAQUE_TS_1_TRIGGERf);
    if (data != 0) {
        policy->ts_triggers |=
            BCM_FLOWTRACKER_STATE_TRANSITION_TIMESTAMP_FLOW_END;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcmi_ft_state_transition_entry_destroy
 * Purpose:
 *  Destroy sw state transition entry at given index.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  index         - (IN) Index
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
STATIC int
bcmi_ft_state_transition_entry_destroy(int unit,
        int index)
{
    bcmi_ft_state_transition_entry_t *sw_ent = NULL;

    sw_ent = BCMI_FT_STATE_TRANSITION_SW_ENTRY(unit, index);
    if (sw_ent == NULL) {
        return BCM_E_NONE;
    }

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        if (sw_ent->check_bmp != NULL) {
            sal_free(sw_ent->check_bmp);
            sw_ent->check_bmp = NULL;
        }
    }
#endif

    sal_free(sw_ent);
    BCMI_FT_STATE_TRANSITION_SW_ENTRY(unit, index) = NULL;

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcmi_ft_state_transition_entry_create
 * Purpose:
 *  Create sw state transition entry at given index.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  index         - (IN) Index
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
STATIC int
bcmi_ft_state_transition_entry_create(int unit,
        int index)
{
    int rv = BCM_E_NONE;
    bcmi_ft_state_transition_entry_t *sw_ent = NULL;
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    int num_checks;
#endif

    /* If index is invalid, no free index */
    if (index == -1) {
        return BCM_E_FULL;
    }

    BCMI_FT_ALLOC_IF_ERR_CLEANUP(
            BCMI_FT_STATE_TRANSITION_SW_ENTRY(unit, index),
            sizeof (bcmi_ft_state_transition_entry_t),
            "ft_state_trans_ent");

    sw_ent = BCMI_FT_STATE_TRANSITION_SW_ENTRY(unit, index);
    sw_ent->hw_index = index;

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        num_checks = BCMI_FT_FLOWCHECKER_MAX_ID(unit);
        BCMI_FT_ALLOC_IF_ERR_CLEANUP(
                sw_ent->check_bmp,
                SHR_BITALLOCSIZE(num_checks) * sizeof(uint32),
                "ft_state_trans_chk");

        sal_memset(sw_ent->agg_alu32_ref_cnt,
                0, TOTAL_GROUP_AGG_ALU32_MEM * sizeof (uint32));
    }
#endif

cleanup:
    if (rv != BCM_E_NONE) {
        (void) bcmi_ft_state_transition_entry_destroy(unit, index);
    }

    return rv;
}

/*
 * Function:
 *  bcmi_ft_state_transition_entry_install
 * Purpose:
 *  Install sw state transition entry to h/w at given index.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  set_valid     - (IN) If valid bit need to be updated
 *  sw_ent        - (IN) S/w state transition entry
 *  hw_entry      - (IN) H/w state transition entry
 *  hw_index      - (IN) H/w Index
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
STATIC int
bcmi_ft_state_transition_entry_install(int unit,
        int set_valid,
        bcmi_ft_state_transition_entry_t *sw_ent,
        bsc_dt_flex_state_transition_table_entry_t *hw_entry,
        int hw_index)
{
    soc_mem_t mem;

    mem = BSC_DT_FLEX_STATE_TRANSITION_TABLEm;
    if (set_valid) {
        soc_mem_field32_set(unit, mem, (void *) hw_entry, VALIDf, 1);
    }

    BCM_IF_ERROR_RETURN(WRITE_BSC_DT_FLEX_STATE_TRANSITION_TABLEm(unit,
                MEM_BLOCK_ANY, hw_index, hw_entry));

    if (set_valid) {
        sw_ent->flags |= BCMI_FT_STATE_TRANSITION_ENTRY_INSTALLED;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcmi_ft_state_transition_entry_remove
 * Purpose:
 *  Remove h/w state transition entry at given index. S/w is not updated.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  hw_index      - (IN) H/w Index
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
STATIC int
bcmi_ft_state_transition_entry_remove(int unit,
        int hw_index)
{
    bsc_dt_flex_state_transition_table_entry_t hw_entry;

    sal_memset(&hw_entry, 0,
            sizeof(bsc_dt_flex_state_transition_table_entry_t));
    BCM_IF_ERROR_RETURN(WRITE_BSC_DT_FLEX_STATE_TRANSITION_TABLEm(unit,
                MEM_BLOCK_ANY, hw_index, &hw_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *  ft_state_trans_entry_prio_cmp
 * Purpose:
 *  Compare priority.
 * Parameters:
 *  prio1         - (IN) First priority
 *  prio2         - (IN) Second priority
 * Returns:
 *  0 if same, -1 if first is smaller otherwise 1
 */
STATIC int
ft_state_trans_entry_prio_cmp(uint32 prio1, uint32 prio2)
{
    int diff;

    if (prio1 == prio2) {
        diff = 0;
    } else if (prio1 < prio2) {
        diff = -1;
    } else {
        diff = 1;
    }
    return diff;
}

/*
 * Function:
 *  ft_state_transition_entry_move_req
 * Purpose:
 *  Check if entry neeed to be moved.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  sw_ent        - (IN) S/w state transition entry
 *  priority      - (IN) New priority
 * Returns:
 *  1 if move is needed, otherwise 0
 */
STATIC int
ft_state_transition_entry_move_req(int unit,
        bcmi_ft_state_transition_entry_t *sw_ent,
        uint32 priority)
{
    int idx = 0;
    int max_entries = 0;
    int prio_diff = 0, flags = -1;
    bcmi_ft_state_transition_entry_t **entry_list = NULL;

    entry_list = BCMI_FT_STATE_TRANSITION_SW_ENTRY_ARR(unit);
    max_entries = BCMI_FT_STATE_TRANSITION_NUM_ENTRIES(unit);

    for (idx = 0; idx < max_entries; idx++) {
        if (entry_list[idx] == sw_ent) {
            flags = 1;
            continue;
        }
        if (entry_list[idx] == NULL) {
            continue;
        }
        prio_diff = ft_state_trans_entry_prio_cmp(entry_list[idx]->priority,
                priority);
        if (flags == -1) {
            if (prio_diff < 0) {
                return TRUE;
            }
        } else {
            if (prio_diff > 0) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

/*
 * Function:
 *  ft_state_transition_no_free_entries
 * Purpose:
 *  Check if free entry is present
 * Parameters:
 *  unit          - (IN) BCM device number
 *  entry_list    - (IN) S/w state transition entry list
 * Returns:
 *  1 if no free entry otherwise 0
 */
STATIC int
ft_state_transition_no_free_entries(int unit,
        bcmi_ft_state_transition_entry_t **entry_list)
{
    int idx;
    int max_entries;

    max_entries = BCMI_FT_STATE_TRANSITION_NUM_ENTRIES(unit);

    for (idx = 0; idx < max_entries; idx++) {
        if (entry_list[idx] == NULL) {
            return FALSE;
        }
    }
    return TRUE;
}

/*
 * Function:
 *  bcmi_ft_state_transition_entry_move
 * Purpose:
 *  Move state transition entry to new h/w index.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  sw_ent        - (IN) S/w state transition entry
 *  hw_entry      - (IN) H/w state transition entry
 *  new_hw_index  - (IN) New H/w Index
 *  flag          - (IN) Flag
 */
STATIC int
bcmi_ft_state_transition_entry_move(
    int unit,
    bcmi_ft_state_transition_entry_t *sw_ent,
    bsc_dt_flex_state_transition_table_entry_t *hw_entry,
    int new_hw_index,
    int flag)
{
    int rv = BCM_E_NONE;
    bcmi_ft_state_transition_entry_t **entry_list = NULL;
    bsc_dt_flex_state_transition_table_entry_t cur_hw_entry;

    if (sw_ent->hw_index == new_hw_index) {
        return BCM_E_NONE;
    }

    /* If hw_entry is NULL, get from h/w */
    if (hw_entry == NULL) {
        BCM_IF_ERROR_RETURN
            (READ_BSC_DT_FLEX_STATE_TRANSITION_TABLEm(unit, MEM_BLOCK_ANY,
                                                      sw_ent->hw_index,
                                                      (void *) &cur_hw_entry));
        hw_entry = &cur_hw_entry;
    }

    /* Install at new index */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_state_transition_entry_install(unit, 0, sw_ent,
                                                hw_entry, new_hw_index));

    /* Remove from old index */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_state_transition_entry_remove(unit, sw_ent->hw_index));

    entry_list = BCMI_FT_STATE_TRANSITION_SW_ENTRY_ARR(unit);

    if (flag == FALSE) {
        entry_list[sw_ent->hw_index] = NULL;
    }
    sw_ent->hw_index = new_hw_index;
    entry_list[new_hw_index] = sw_ent;

    return rv;
}

/*
 * Function:
 *  bcmi_ft_state_transition_entry_shift_down
 * Purpose:
 *  Move state transition entry down in the h/w table.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  entry_list    - (IN) S/w state transition entry list
 *  target_index  - (IN) target H/w Index
 *  next_null_index - (IN) Next free index
 */
STATIC int
bcmi_ft_state_transition_entry_shift_down(
    int unit,
    bcmi_ft_state_transition_entry_t **entry_list,
    int target_index,
    int next_null_index)
{
    int free_index = 0;
    bcmi_ft_state_transition_entry_t *sw_ent = NULL;

    free_index = next_null_index;
    while(free_index > target_index) {
        sw_ent = BCMI_FT_STATE_TRANSITION_SW_ENTRY(unit, free_index - 1);
        BCM_IF_ERROR_RETURN
            (bcmi_ft_state_transition_entry_move(unit, sw_ent,
                                                 NULL, free_index, 0));

        free_index--;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcmi_ft_state_transition_entry_shift_up
 * Purpose:
 *  Move state transition entry up in the h/w table.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  entry_list    - (IN) S/w state transition entry list
 *  target_index  - (IN) target H/w Index
 *  prev_null_index - (IN) Previous free index
 */
STATIC int
bcmi_ft_state_transition_entry_shift_up(
    int unit,
    bcmi_ft_state_transition_entry_t **entry_list,
    int target_index,
    int prev_null_index)
{
    int free_index = 0;
    bcmi_ft_state_transition_entry_t *sw_ent = NULL;

    free_index = prev_null_index;
    while (free_index < target_index) {

        sw_ent = BCMI_FT_STATE_TRANSITION_SW_ENTRY(unit, free_index + 1);
        BCM_IF_ERROR_RETURN
            (bcmi_ft_state_transition_entry_move(unit, sw_ent,
                                                 NULL, free_index, 0));

        free_index++;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcmi_ft_state_transition_entry_prio_set
 * Purpose:
 *  Set priority of state transition entry.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  index         - (IN) index
 *  hw_entry      - (IN) H/w state transition entry
 *  priority      - (IN) New priority
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
STATIC int
bcmi_ft_state_transition_entry_prio_set(int unit,
        int index,
        bsc_dt_flex_state_transition_table_entry_t *hw_entry,
        uint32 priority)
{
    int rv = BCM_E_NONE;
    int move_req;
    int max_entries = 0, dir = 0;
    int temp_idx = -1, target_idx = -1;
    int prev_null_idx = -1;
    int next_null_idx = -1;
    int first_null_idx = -1;
    int first_null_flag = 0;
    int pivot_found = FALSE;
    int only_move = FALSE;
    int flag_no_free_entries = FALSE;
    int decr_on_shift_up = TRUE;
    bcmi_ft_state_transition_entry_t *sw_ent = NULL;
    bcmi_ft_state_transition_entry_t **entry_list = NULL;

    sw_ent = BCMI_FT_STATE_TRANSITION_SW_ENTRY(unit, index);

    entry_list = BCMI_FT_STATE_TRANSITION_SW_ENTRY_ARR(unit);
    max_entries = BCMI_FT_STATE_TRANSITION_NUM_ENTRIES(unit);

    /* Check if movement is required */
    move_req = ft_state_transition_entry_move_req(unit, sw_ent, priority);
    if (move_req == FALSE) {
        BCM_IF_ERROR_RETURN
            (bcmi_ft_state_transition_entry_install(unit, 1, sw_ent,
                                                    hw_entry,
                                                    sw_ent->hw_index));
        sw_ent->priority = priority;
        return BCM_E_NONE;
    }

    if (ft_state_transition_no_free_entries(unit, entry_list) == TRUE) {
        if (!(sw_ent->flags & BCMI_FT_STATE_TRANSITION_ENTRY_INSTALLED)) {
            entry_list[sw_ent->hw_index] = NULL;
            flag_no_free_entries = TRUE;
        } else {
            return BCM_E_RESOURCE;
        }
    }

    /* Get nearest null index before given priority */
    for (target_idx = 0; target_idx < max_entries; target_idx++) {
        /* Skip Self */
        if (sw_ent == entry_list[target_idx]) {
            continue;
        }
        if (entry_list[target_idx] == NULL) {
            prev_null_idx = target_idx;
            if (first_null_flag == 0) {
                first_null_idx = target_idx;
                first_null_flag = 1;
            }
            continue;
        } else {
            first_null_flag = 0;
        }

        if (ft_state_trans_entry_prio_cmp(priority,
                    entry_list[target_idx]->priority) > 0) {
            pivot_found = TRUE;
            break;
        }
    }
    /* Get nearest null index after given priority */
    for (temp_idx = target_idx; temp_idx < max_entries; temp_idx++) {
        if (entry_list[temp_idx] == NULL) {
            next_null_idx = temp_idx;
            break;
        }
    }

    /* Put back the entry */
    if (flag_no_free_entries == TRUE) {
        entry_list[sw_ent->hw_index] = sw_ent;
    }

    /* Find correct target index */
    if (pivot_found == FALSE) {
        if ((prev_null_idx != -1) &&
                (prev_null_idx == (max_entries - 1)) &&
                (first_null_flag == 1)) {
            target_idx = first_null_idx;
            only_move = TRUE;
        } else {
            target_idx = (max_entries - 1);
            decr_on_shift_up = FALSE;
        }
    }
    if (only_move == FALSE) {
        /* Get smallest move direction */
        if (prev_null_idx != -1) {
            if (next_null_idx != -1) {
                if ((target_idx - prev_null_idx) <
                        (next_null_idx - target_idx)) {
                    dir = -1; /* Shift Up */
                } else {
                    dir = 1; /* Shift Down */
                }
            } else {
                dir = -1; /* Shift Up */
            }
        } else {
            if (next_null_idx != -1) {
                dir = 1; /* Shift Down */
            } else {
                LOG_ERROR(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                                "No free entries for movement\n")));
                return BCM_E_CONFIG; /* No free entry to move */
            }
        }

        if (dir == 1) {
            if (entry_list[target_idx] != NULL) {
                rv = bcmi_ft_state_transition_entry_shift_down(unit,
                        entry_list, target_idx, next_null_idx);
                BCM_IF_ERROR_RETURN(rv);
            }
        } else {
            if (decr_on_shift_up == TRUE) {
                target_idx--;
            }
            if (entry_list[target_idx] != NULL) {
                rv = bcmi_ft_state_transition_entry_shift_up(unit,
                        entry_list, target_idx, prev_null_idx);
            }
        }
        BCM_IF_ERROR_RETURN(rv);
    }

    if (sw_ent->flags & BCMI_FT_STATE_TRANSITION_ENTRY_INSTALLED) {
        BCM_IF_ERROR_RETURN
            (bcmi_ft_state_transition_entry_move(unit, sw_ent,
                                                 hw_entry, target_idx,
                                                 flag_no_free_entries));
    } else {
        BCM_IF_ERROR_RETURN
            (bcmi_ft_state_transition_entry_install(unit, 1, sw_ent,
                                                    hw_entry, target_idx));
        entry_list[sw_ent->hw_index] = NULL;
        sw_ent->hw_index = target_idx;
        entry_list[target_idx] = sw_ent;
    }

    sw_ent->priority = priority;

    return BCM_E_NONE;
}

/*
 * Function:
 *  ft_state_transition_key_cmp
 * Purpose:
 *  Compare state transition entry.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  sw_ent        - (IN) S/w state transition entry
 *  hw_entry      - (IN) H/w state transition entry
 *  hw_index      - (IN) H/w Index
 * Returns:
 *  0 if matched otherwise 1
 */
STATIC int
ft_state_transition_key_cmp(int unit,
        int index,
        bsc_dt_flex_state_transition_table_entry_t *hw_entry,
        bcm_flowtracker_state_transition_key_t *key)
{
    int rv;
    bcm_flowtracker_state_transition_key_t hw_key;

    rv = bcmi_ft_state_transition_hw_entry_to_key(unit, index,
            hw_entry, &hw_key);
    if (rv != BCM_E_NONE) {
        return -1;
    }

    if (key->tcp_control != hw_key.tcp_control) {
        return 1;
    }

    if (key->tcp_control_mask != hw_key.tcp_control_mask) {
        return 1;
    }

    if (key->control_id != hw_key.control_id) {
        return 1;
    }

    if (key->bidirectional_flow != hw_key.bidirectional_flow) {
        return 1;
    }

    if (key->new_learn != hw_key.new_learn) {
        return 1;
    }

    if (key->current_state != hw_key.current_state) {
        return 1;
    }

    return 0;
}

/*
 * Function:
 *  bcmi_ft_state_transition_key_match
 * Purpose:
 *  Match h/w entry with requested state transition key.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  mem           - (IN) H/w memory
 *  index         - (IN) H/w index which is matched
 *  entry         - (IN) H/w entry which is matched
 *  trv_data      - (IN/OUT) Traversal data
 *  done          - (OUT) Done
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
STATIC int
bcmi_ft_state_transition_key_match(int unit,
    soc_mem_t mem,
    int index,
    void *entry,
    bcmi_ft_state_transition_tbl_trv_data_t *trv_data,
    int *done)
{
    int match;
    bsc_dt_flex_state_transition_table_entry_t *hw_entry;

    hw_entry = (bsc_dt_flex_state_transition_table_entry_t *) entry;

    *done = FALSE;
    if (!soc_mem_field32_get(unit, mem, entry, VALIDf)) {
        if (trv_data->free_index == -1) {
            trv_data->free_index = index;
        }
        return BCM_E_NONE;
    }

    if (ft_state_transition_key_cmp(unit, index,
                hw_entry, trv_data->key) == 0) {
        match = TRUE;
    } else {
        match = FALSE;
    }

    if (match == TRUE) {
        trv_data->found = TRUE;
        trv_data->match_index = index;
        sal_memcpy((void *) &(trv_data->match_hw_entry),
                (void *) hw_entry,
                sizeof (bsc_dt_flex_state_transition_table_entry_t));
        *done = TRUE;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *  bcmi_ft_state_transition_entry_handle
 * Purpose:
 *  Update key_list / policy_list for current h/w entry.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  mem           - (IN) H/w memory
 *  index         - (IN) H/w index
 *  entry         - (IN) H/w entry
 *  trv_data      - (IN/OUT) Traversal data
 *  done          - (OUT) Done
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
STATIC int
bcmi_ft_state_transition_entry_handle(int unit,
    soc_mem_t mem,
    int index,
    void *entry,
    bcmi_ft_state_transition_tbl_trv_data_t *trv_data,
    int *done)
{
    int curr_idx = 0;
    int max_count = 0;
    bsc_dt_flex_state_transition_table_entry_t *hw_entry;

    if (!soc_mem_field32_get(unit, mem, entry, VALIDf)) {
        *done = FALSE;
        return BCM_E_NONE;
    }

    curr_idx = trv_data->current_idx;
    max_count = trv_data->max_count;
    hw_entry = (bsc_dt_flex_state_transition_table_entry_t *) entry;

    if (curr_idx < max_count) {
        BCM_IF_ERROR_RETURN
            (bcmi_ft_state_transition_hw_entry_to_key(unit,
                                         index, hw_entry,
                                         &(trv_data->key[curr_idx])));

        BCM_IF_ERROR_RETURN
            (bcmi_ft_state_transition_hw_entry_to_policy(unit, hw_entry,
                                         &(trv_data->policy[curr_idx])));

        curr_idx++;
    }

    trv_data->current_idx = curr_idx;
    trv_data->actual_count++;

    if ((max_count != 0) &&
            (trv_data->actual_count == max_count)) {
        *done = TRUE;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcmi_ft_state_transition_tbl_traverse
 * Purpose:
 *  State transition table traversal function to callback per entry.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  cb            - (IN) callback function
 *  trv_data      - (IN/OUT) Traversal data
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
STATIC int
bcmi_ft_state_transition_tbl_traverse(int unit,
    bcmi_ft_state_transition_tbl_callack_f cb,
    bcmi_ft_state_transition_tbl_trv_data_t *trv_data)
{
    int rv = BCM_E_NONE;
    int done = FALSE;
    int8 *buffer = NULL;
    void *entry = NULL;
    soc_mem_t mem;
    uint32 e_idx = 0;
    int mem_min = 0, mem_max = 0;
    uint32 chunk_size = 0, mem_entry_size = 0;

    mem = BSC_DT_FLEX_STATE_TRANSITION_TABLEm;
    mem_min = soc_mem_index_min(unit, mem);
    mem_max = soc_mem_index_max(unit, mem);

    chunk_size = (mem_max - mem_min + 1);
    trv_data->entry_words = SOC_MEM_WORDS(unit, mem);
    mem_entry_size = trv_data->entry_words * sizeof(uint32);

    buffer = soc_cm_salloc(unit, mem_entry_size * chunk_size, "mem buffer");
    if (buffer == NULL)  {
        return BCM_E_MEMORY;
    }

    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ALL,
            mem_min, mem_min + chunk_size - 1, buffer);
    if (BCM_FAILURE(rv)) {
        soc_cm_sfree(unit, buffer);
        return rv;
    }

    for (e_idx = 0; e_idx < chunk_size; e_idx += 1) {
        entry = soc_mem_table_idx_to_pointer(unit, mem, void *,
                buffer, e_idx);
        rv = (*cb)(unit, mem, e_idx, entry, trv_data, &done);
        if (BCM_FAILURE(rv) || (done == TRUE)) {
            break;
        }
    }

    if (buffer != NULL) {
        soc_cm_sfree(unit, buffer);
    }
    return rv;
}

/*
 * Function:
 *  bcmi_ft_state_transition_entry_find
 * Purpose:
 *  Find requested State transition key in h/w.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  key           - (IN) State transition key
 *  found         - (OUT) is found
 *  index         - (OUT) Found index or free index if not found
 *  match_hw_entry- (OUT) Matched h/w entry.
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
STATIC int
bcmi_ft_state_transition_entry_find(int unit,
    bcm_flowtracker_state_transition_key_t *key,
    int *found,
    int *index,
    bsc_dt_flex_state_transition_table_entry_t *match_hw_entry)
{
    int rv;
    soc_mem_t mem;
    int out_hw_index;
    uint32 mem_entry_size = 0;
    bcmi_ft_state_transition_tbl_trv_data_t trv_data;

    mem = BSC_DT_FLEX_STATE_TRANSITION_TABLEm;

    sal_memset(&trv_data, 0, sizeof (trv_data));
    trv_data.key = key;
    trv_data.free_index = -1;

    /* Traverse on mem and match key */
    rv = bcmi_ft_state_transition_tbl_traverse(unit,
            bcmi_ft_state_transition_key_match, &trv_data);
    BCM_IF_ERROR_RETURN(rv);

    if (trv_data.found == TRUE) {
        *found = TRUE;
        out_hw_index = trv_data.match_index;
    } else {
        *found = FALSE;
        out_hw_index = trv_data.free_index;
    }

    if (index != NULL) {
        *index = out_hw_index;
    }
    if (match_hw_entry != NULL) {
        mem_entry_size = SOC_MEM_WORDS(unit, mem) * sizeof(uint32);
        sal_memcpy((void *) match_hw_entry,
                (void *) &(trv_data.match_hw_entry), mem_entry_size);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_flow_transition_state_cleanup
 * Purpose:
 *   Cleanup State transition state for flowtracker.
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_flow_transition_state_cleanup(int unit)
{
    int idx, num_entries;
    bcmi_ft_state_transition_entry_t **entry_list = NULL;

    num_entries = BCMI_FT_STATE_TRANSITION_NUM_ENTRIES(unit);
    entry_list = BCMI_FT_STATE_TRANSITION_SW_ENTRY_ARR(unit);

    if (entry_list != NULL) {
        for (idx = 0; idx < num_entries; idx++) {
            if (entry_list[idx] != NULL) {
                (void) bcmi_ft_state_transition_entry_remove(unit,
                        entry_list[idx]->hw_index);
                /* enty_list[idx] is freed as well */
                (void) bcmi_ft_state_transition_entry_destroy(unit,
                        entry_list[idx]->hw_index);
            }
        }
        sal_free(entry_list);
        BCMI_FT_STATE_TRANSITION_SW_ENTRY_ARR(unit) = NULL;
    }
    sal_memset(BCMI_FT_STATE_TRANSITION_INFO(unit), 0,
            sizeof (bcmi_ft_state_transition_info_t));

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
    int rv = BCM_E_NONE;
    soc_mem_t mem = INVALIDm;
    int num_entries;
    bcmi_ft_state_transition_entry_t **entry_list = NULL;

    mem = BSC_DT_FLEX_STATE_TRANSITION_TABLEm;
    num_entries = soc_mem_index_count(unit, mem);

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3) ||
        soc_feature(unit, soc_feature_flex_flowtracker_state_transition)) {

        BCMI_FT_ALLOC_IF_ERR_CLEANUP(entry_list,
                num_entries * sizeof (bcmi_ft_state_transition_entry_t *),
                "ft_state_trans_ent_arr");

        BCMI_FT_STATE_TRANSITION_NUM_ENTRIES(unit) = num_entries;
        BCMI_FT_STATE_TRANSITION_SW_ENTRY_ARR(unit) = entry_list;
    }

cleanup:
    if (rv != BCM_E_NONE) {
        (void) bcmi_ft_flow_transition_state_cleanup(unit);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *  ft_state_transition_hw_def_entries_add
 * Purpose:
 *  Add default state transition entry to h/w onlyg.
 * Parameters:
 *  unit          - (IN) BCM device number
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
STATIC int
ft_state_transition_hw_def_entries_add(int unit)
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
                    -1, tcp_control_mask[i], &offset, NULL, NULL));
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

                BCM_IF_ERROR_RETURN(_bcm_field_ft_alu_data_config_get(unit,
                            -1, state_transition_data[i].tcp_flags[j],
                            &offset, NULL, NULL));
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
 *  ft_state_transition_hwsw_def_entries_add
 * Purpose:
 *  Add default state transition entry and update s/w mapping.
 * Parameters:
 *  unit          - (IN) BCM device number
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
STATIC int
ft_state_transition_hwsw_def_entries_add(int unit)
{
    int offset, idx, j;
    uint8 tcp_control = 0;
    uint8 tcp_control_mask = 0x0;
    bcmi_ft_state_transition_t *st_data = NULL;
    bcm_flowtracker_state_transition_key_t key;
    bcm_flowtracker_state_transition_policy_t policy;
    int tcp_flags[] = {
        _bcmFieldFtAluDataTcpFlagsSYN,
        _bcmFieldFtAluDataTcpFlagsACK,
        _bcmFieldFtAluDataTcpFlagsRST,
        _bcmFieldFtAluDataTcpFlagsFIN
    };


    for (idx = 0; idx < COUNTOF(tcp_flags); idx++) {
        BCM_IF_ERROR_RETURN(_bcm_field_ft_alu_data_config_get(unit,
                    -1, tcp_flags[idx], NULL, &offset, NULL));
        tcp_control_mask |= (1 << offset);
    }

    for (idx = 0; idx < COUNTOF(state_transition_data); idx++) {
        if (state_transition_data[idx].current_state == -1) {
            break;
        }

        st_data = &state_transition_data[idx];
        bcm_flowtracker_state_transition_key_t_init(&key);
        bcm_flowtracker_state_transition_policy_t_init(&policy);

        tcp_control = 0;
        for (j = 0; j < COUNTOF(st_data->tcp_flags); j++) {
            if (st_data->tcp_flags[j] != -1) {
                BCM_IF_ERROR_RETURN(_bcm_field_ft_alu_data_config_get(unit,
                            -1, st_data->tcp_flags[j], NULL, &offset, NULL));
                tcp_control |= (1 << offset);
            }
        }

        key.tcp_control = tcp_control;
        key.tcp_control_mask = tcp_control_mask;
        key.current_state = st_data->current_state;
        key.new_learn = st_data->new_learn;
        key.bidirectional_flow = st_data->bidirectional;

        policy.next_state = st_data->next_state;
        if (st_data->ts_triggers & 0x1) {
            policy.ts_triggers |=
                BCM_FLOWTRACKER_STATE_TRANSITION_TIMESTAMP_FLOW_START;
        }
        if (st_data->ts_triggers & 0x2) {
            policy.ts_triggers |=
                BCM_FLOWTRACKER_STATE_TRANSITION_TIMESTAMP_FLOW_END;
        }

        BCM_IF_ERROR_RETURN(
                bcmi_ft_state_transition_add(unit, &key, &policy));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcmi_ft_state_transition_def_entries_add
 * Purpose:
 *  Add defaults state transition entry.
 * Parameters:
 *  unit          - (IN) BCM device number
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int
bcmi_ft_state_transition_def_entries_add(int unit)
{
    int state_trans_mode;

    state_trans_mode = soc_property_get(unit,
            spn_FLOWTRACKER_FLEX_STATE_TRANSITION_MODE, 0);

    /* Default entries are not needed */
    if (state_trans_mode == 2) {
        return BCM_E_NONE;
    }

    /* Add default entries in H/w and update S/w structs. */
    if ((state_trans_mode == 1) ||
        (soc_feature(unit, soc_feature_flex_flowtracker_ver_3))) {
        return ft_state_transition_hwsw_def_entries_add(unit);
    }

    /* Add default entries in H/w only */
    return ft_state_transition_hw_def_entries_add(unit);
}

/*
 * Function:
 *  bcm_esw_flowtracker_state_transition_add
 * Purpose:
 *  The API adds a state transition entry with a given key/policy. If the
 *  given key is already present in the table, only policy is updated. Else,
 *  a new entry is added to the table. The function is applicable only if
 * config property flowtracker_flex_state_transition_mode is non-zero.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  key           - (IN) State transition key
 *  policy        - (IN) State transition policy
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int
bcmi_ft_state_transition_add(
    int unit,
    bcm_flowtracker_state_transition_key_t *key,
    bcm_flowtracker_state_transition_policy_t *policy)
{
    int index = -1;
    int found = FALSE;
    bsc_dt_flex_state_transition_table_entry_t hw_entry;

    /* Validate input key */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_state_transition_key_validate(unit, key));

    /* Validate input policy */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_state_transition_policy_validate(unit, policy));

    sal_memset(&hw_entry, 0, sizeof (hw_entry));
    /* Find key */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_state_transition_entry_find(unit, key, &found,
                                             &index, &hw_entry));

    /* If not found, insert at index */
    if (found == FALSE) {
        BCM_IF_ERROR_RETURN
            (bcmi_ft_state_transition_entry_create(unit, index));

        BCM_IF_ERROR_RETURN
            (bcmi_ft_state_transition_key_to_hw_entry(unit, key,
                                                      &hw_entry));
    }

    /* Setup policy */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_state_transition_policy_to_hw_entry(unit, policy,
                                                     &hw_entry));

    /* Now, move to correct position based on priority */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_state_transition_entry_prio_set(unit, index, &hw_entry,
                                                 key->priority));

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcm_esw_flowtracker_state_transition_delete
 * Purpose:
 *  The API deletes state transition entry from the hardware table. It is
 *  applicable only if config property flowtracker_flex_state_transition_mode
 *  is non-zero.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  key           - (IN) State transition key
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int
bcmi_ft_state_transition_delete(
    int unit,
    bcm_flowtracker_state_transition_key_t *key)
{
    int found = FALSE;
    int index = -1;
    bcmi_ft_state_transition_entry_t *sw_ent = NULL;

    /* Validate input key */
    BCM_IF_ERROR_RETURN(bcmi_ft_state_transition_key_validate(unit, key));

    /* Find key */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_state_transition_entry_find(unit, key, &found,
                                             &index, NULL));

    /* If not found, return error */
    if ((found == FALSE) || (index == -1)) {
        return BCM_E_NOT_FOUND;
    }

    sw_ent = BCMI_FT_STATE_TRANSITION_SW_ENTRY(unit, index);
    if (sw_ent == NULL) {
        return BCM_E_NONE;
    }

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        int i, bmp_words;
        bmp_words = SHR_BITALLOCSIZE(BCMI_FT_FLOWCHECKER_MAX_ID(unit));
        for (i = 0; i < bmp_words; i++) {
            if (sw_ent->check_bmp[i] != 0) {
                LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                                "State transition key is associated "
                                "with flowtracker check(s)\n\r")));
                return BCM_E_BUSY;
            }
        }
    }
#endif

    /* Remove entry */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_state_transition_entry_remove(unit, sw_ent->hw_index));

    /* Destroy entry */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_state_transition_entry_destroy(unit, sw_ent->hw_index));

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcm_esw_flowtracker_state_transition_get
 * Purpose:
 *  The API get policy configuration associated with given state transition
 *  key. If the state transition key is not present in the hardware, the
 *  function shall return BCM_E_NOT_FOUND. It is applicable only if config
 *  property flowtracker_flex_state_transition_mode is non-zero.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  key           - (IN) State transition key
 *  policy        - (OUT) State transition policy
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int
bcmi_ft_state_transition_get(
    int unit,
    bcm_flowtracker_state_transition_key_t *key,
    bcm_flowtracker_state_transition_policy_t *policy)
{
    int found = FALSE;
    bsc_dt_flex_state_transition_table_entry_t hw_entry;

    /* Validate input key */
    BCM_IF_ERROR_RETURN(bcmi_ft_state_transition_key_validate(unit, key));

    /* Find key */
    sal_memset(&hw_entry, 0, sizeof (hw_entry));
    BCM_IF_ERROR_RETURN
        (bcmi_ft_state_transition_entry_find(unit, key, &found,
                                             NULL, &hw_entry));
    if (found == FALSE) {
        return BCM_E_NOT_FOUND;
    }

    /* Setup policy */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_state_transition_hw_entry_to_policy(unit, &hw_entry,
                                                     policy));

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcm_esw_flowtracker_state_transition_get_all
 * Purpose:
 *  The API gets all state transitions configured in the system. If max_entries
 *  is 0 and key_list, policy_list is NULL, actual_entries returns number of
 *  configured entries. It is applicable only if config property
 *  flowtracker_flex_state_transition_mode is non-zero.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  max_entries   - (IN) Maximum number of state transition entries that can
 *                       be fit into key_list / policy_list. If passed as zero,
 *                       the actual_count param will return the actual number
 *                       of entries configured.",
 *  key_list      - (OUT) Pointer to the list of state transition key.
 *  policy_list   - (OUT) Pointer to the list of state transition policies.
 *  actual_entries- (OUT) The actual number of valid state transition entries.
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int
bcmi_ft_state_transition_get_all(
    int unit,
    int max_entries,
    bcm_flowtracker_state_transition_key_t *key_list,
    bcm_flowtracker_state_transition_policy_t *policy_list,
    int *actual_entries)
{
    int rv = BCM_E_NONE;
    bcmi_ft_state_transition_tbl_trv_data_t trv_data;

    /* validation of inputs */
    if ((max_entries != 0) &&
            ((key_list == NULL) || (policy_list == NULL))) {
        return BCM_E_PARAM;
    }

    if (actual_entries == NULL) {
        return BCM_E_NONE;
    }

    sal_memset(&trv_data, 0, sizeof (trv_data));
    trv_data.max_count = max_entries;
    trv_data.key = key_list;
    trv_data.policy = policy_list;

    rv = bcmi_ft_state_transition_tbl_traverse(unit,
            bcmi_ft_state_transition_entry_handle, &trv_data);
    BCM_IF_ERROR_RETURN(rv);

    *actual_entries = trv_data.actual_count;

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcmi_ft_state_transition_entry_decode
 * Purpose:
 *  Decode for current h/w entry for diag command.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  mem           - (IN) H/w memory
 *  index         - (IN) H/w index
 *  entry         - (IN) H/w entry
 *  trv_data      - (IN/OUT) Traversal data
 *  done          - (OUT) Done
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
STATIC int
bcmi_ft_state_transition_entry_decode(int unit,
    soc_mem_t mem,
    int index,
    void *entry,
    bcmi_ft_state_transition_tbl_trv_data_t *trv_data,
    int *done)
{
    bcm_flowtracker_state_transition_key_t key;
    bcm_flowtracker_state_transition_policy_t policy;
    bcmi_ft_state_transition_entry_t *sw_ent = NULL;
    bsc_dt_flex_state_transition_table_entry_t *hw_entry;

    if (!soc_mem_field32_get(unit, mem, entry, VALIDf)) {
        return BCM_E_NONE;
    }

    hw_entry = (bsc_dt_flex_state_transition_table_entry_t *) entry;

    bcm_flowtracker_state_transition_key_t_init(&key);
    BCM_IF_ERROR_RETURN(
            bcmi_ft_state_transition_hw_entry_to_key(unit,
                index, hw_entry, &key));

    bcm_flowtracker_state_transition_policy_t_init(&policy);
    BCM_IF_ERROR_RETURN(
            bcmi_ft_state_transition_hw_entry_to_policy(unit,
                hw_entry, &policy));

    if (BCMI_FT_STATE_TRANSITION_SW_ENTRY_ARR(unit) != NULL) {
        sw_ent = BCMI_FT_STATE_TRANSITION_SW_ENTRY(unit, index);
    }
    if (sw_ent != NULL) {
        LOG_CLI((BSL_META_U(unit, " [%2d] Hw_index=%d | Prio=%d\n\r"),
                    index, sw_ent->hw_index, sw_ent->priority));
        LOG_CLI((BSL_META_U(unit, " %4c Ctrl_id=%d | Tcp_control=0x%x(0x%x)"),
                    ' ', key.control_id, key.tcp_control,
                    key.tcp_control_mask));
        LOG_CLI((BSL_META_U(unit, " | BiDir=%d | NewLearn=%d\n\r"),
                    key.bidirectional_flow, key.new_learn));
    } else {
        LOG_CLI((BSL_META_U(unit, " [%2d] Ctrl_id=%d | Tcp_control=0x%x(0x%x)"),
                    index, key.control_id, key.tcp_control,
                    key.tcp_control_mask));
        LOG_CLI((BSL_META_U(unit, " | BiDir=%d | NewLearn=%d\n\r"),
                    key.bidirectional_flow, key.new_learn));
    }
    LOG_CLI((BSL_META_U(unit, " %4c CurrState=%d | NextState=%d |"),
                ' ', key.current_state, policy.next_state));
    LOG_CLI((BSL_META_U(unit, " ExportEn=%d | TS_Triggers=0x%x\n\r"),
                policy.export_trigger, policy.ts_triggers));

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        int i, typ, chk_in_line = 0;
        bcm_flowtracker_check_t check_id;
        bcmi_ft_alu_load_type_t alu_load_type;

        typ = BCM_FT_IDX_TYPE_CHECK;
        LOG_CLI((BSL_META_U(unit, " %4c CHECKs:"), ' '));
        for (i = 0; i < BCMI_FT_FLOWCHECKER_MAX_ID(unit); i++) {
            if (sw_ent == NULL) {
                break;
            }
            if (!SHR_BITGET(sw_ent->check_bmp, i)) {
                continue;
            }
            check_id = BCMI_FT_FLOWCHECKER_INDEX_SET(i, typ);
            LOG_CLI((BSL_META_U(unit, " 0x%08x"), check_id));
            chk_in_line++;
            if (chk_in_line == 8) {
                LOG_CLI((BSL_META_U(unit, "\n\r %4c   "), ' '));
                chk_in_line = 0;
            }
        }

        LOG_CLI((BSL_META_U(unit, "\n\r %4c AGG_ALU32 map:"), ' '));
        alu_load_type = bcmiFtAluLoadTypeAggAlu32;
        for (i = 0; i < BCMI_FT_ALU_LOAD_MEM_COUNT(unit, alu_load_type);
                i++) {
            if (sw_ent == NULL) {
                break;
            }
            if (sw_ent->agg_alu32_ref_cnt[i] != 0) {
                LOG_CLI((BSL_META_U(unit, " [%d->%d]"), i,
                            sw_ent->agg_alu32_ref_cnt[i]));
            }
        }
    }
#endif

    LOG_CLI((BSL_META_U(unit, "\n\r")));

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_state_transition_show
 * Purpose:
 *   Show state transition table configurations.
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   BCM_E_NONE - If successful
 *   BCM_E_xxx
 */
int
bcmi_ft_state_transition_show(int unit)
{
    bcmi_ft_state_transition_tbl_trv_data_t trv_data;

    sal_memset(&trv_data, 0, sizeof (trv_data));
    (void) bcmi_ft_state_transition_tbl_traverse(unit,
            bcmi_ft_state_transition_entry_decode, &trv_data);

    return BCM_E_NONE;
}

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)


/*
 * Function:
 *  bcmi_ftv3_state_transition_check_add
 * Purpose:
 *  Add flowtracker check with state transition key.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  key           - (IN) State transition key
 *  check_id      - (IN) Flowtracker Check Id
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int
bcmi_ftv3_state_transition_check_add(
    int unit,
    bcm_flowtracker_state_transition_key_t *key,
    bcm_flowtracker_check_t check_id)
{
    int rv;
    int st_idx = 0;
    uint32_t check_idx = 0;
    int found = FALSE;
    bcmi_flowtracker_flowchecker_info_t flowcheck_info;
    bcmi_ft_state_transition_entry_t *sw_ent = NULL;

    /* Validate input key */
    BCM_IF_ERROR_RETURN(bcmi_ft_state_transition_key_validate(unit, key));

    /* Find Index of Key */
    BCM_IF_ERROR_RETURN(bcmi_ft_state_transition_entry_find(unit,
                key, &found, &st_idx, NULL));
    if (found == FALSE) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "Flowtracker state transition key "
                        " is not configured\n\r")));
        return BCM_E_NOT_FOUND;
    }

    /* Validate Check */
    if (!BCMI_FT_IDX_IS_PRIMARY_CHECK(check_id)) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "Flowtracker check (0x%x) "
                        " is not primary check\n\r"), check_id));
        return BCM_E_PARAM;
    }

    /* Get flowtracker check info */
    sal_memset(&flowcheck_info, 0, sizeof (flowcheck_info));
    rv = bcmi_ft_flowchecker_get(unit, check_id, &flowcheck_info);
    BCM_IF_ERROR_RETURN(rv);

    /* State transition flags must be set */
    if (!(flowcheck_info.flags & BCM_FLOWTRACKER_CHECK_STATE_TRANSITION)) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "Flowtracker state transition is not enabled on"
                        " Flowtracker check (0x%x) \n\r"), check_id));
        return BCM_E_CONFIG;
    }

    sw_ent = BCMI_FT_STATE_TRANSITION_SW_ENTRY(unit, st_idx);
    check_idx = BCMI_FT_FLOWCHECKER_INDEX_GET(check_id);
    SHR_BITSET(sw_ent->check_bmp, check_idx);

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcmi_ftv3_state_transition_check_delete
 * Purpose:
 *  Delete flowtracker check from state transition key.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  key           - (IN) State transition key
 *  check_id      - (IN) Flowtracker Check Id
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int
bcmi_ftv3_state_transition_check_delete(
    int unit,
    bcm_flowtracker_state_transition_key_t *key,
    bcm_flowtracker_check_t check_id)
{
    int rv;
    int st_idx = 0;
    uint32_t check_idx = 0;
    int found = FALSE;
    bcmi_flowtracker_flowchecker_info_t flowcheck_info;
    bcmi_ft_state_transition_entry_t *sw_ent = NULL;

    /* Validate input key */
    BCM_IF_ERROR_RETURN(bcmi_ft_state_transition_key_validate(unit, key));

    /* Find Index of Key */
    BCM_IF_ERROR_RETURN(bcmi_ft_state_transition_entry_find(unit,
                key, &found, &st_idx, NULL));
    if (found == FALSE) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "Flowtracker state transition key "
                        " is not configured\n\r")));
        return BCM_E_NOT_FOUND;
    }

    /* Validate Check */
    if (!BCMI_FT_IDX_IS_PRIMARY_CHECK(check_id)) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "Flowtracker check (0x%x) "
                        " is not primary check\n\r"), check_id));
        return BCM_E_PARAM;
    }

    /* Get flowtracker check info */
    sal_memset(&flowcheck_info, 0, sizeof (flowcheck_info));
    rv = bcmi_ft_flowchecker_get(unit, check_id, &flowcheck_info);
    BCM_IF_ERROR_RETURN(rv);

    /* Check if flowtracker check is associated with validated group */
    rv = bcmi_ftv3_check_in_validated_group(unit, check_id);
    if (rv == BCM_E_NONE) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "Flowtracker check (0x%x) "
                        " is associated with validate group(s)\n\r"),
                    check_id));
        return BCM_E_BUSY;
    }

    sw_ent = BCMI_FT_STATE_TRANSITION_SW_ENTRY(unit, st_idx);
    check_idx = BCMI_FT_FLOWCHECKER_INDEX_GET(check_id);

    if (!SHR_BITGET(sw_ent->check_bmp, check_idx)) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "Flowtracker check (0x%x) is not associated"
                        " with Flowtracker state transition\n\r"), check_id));
        return BCM_E_NOT_FOUND;
    }

    SHR_BITCLR(sw_ent->check_bmp, check_idx);

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcmi_ftv3_state_transition_check_get_all
 * Purpose:
 *  Get all the flow checks associated with flow state transition key.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  key           - (IN) State transition key
 *  max_checks    - (IN) Maximum number of check information that user has
 *                       sent memory for.
 * check_id_list  - (OUT) List of checkids that will be filled and sent back.
 * actual_checks  - (OUT) Actual Number of check ids present in memory.
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int
bcmi_ftv3_state_transition_check_get_all(
    int unit,
    bcm_flowtracker_state_transition_key_t *key,
    int max_checks,
    bcm_flowtracker_check_t *check_id_list,
    int *actual_checks)
{
    int typ = 0;
    int curr_idx = 0;
    int found = FALSE;
    int st_idx = 0;
    uint32 i, num_avail_checks;
    bcmi_ft_state_transition_entry_t *sw_ent = NULL;

    if (key == NULL) {
        return BCM_E_PARAM;
    }

    if ((max_checks != 0) && (check_id_list == NULL)) {
        return BCM_E_PARAM;
    }

    if (actual_checks == NULL) {
        return BCM_E_PARAM;
    }

    /* Validate input key */
    BCM_IF_ERROR_RETURN(bcmi_ft_state_transition_key_validate(unit, key));

    /* Find Index of Key */
    BCM_IF_ERROR_RETURN(bcmi_ft_state_transition_entry_find(unit,
                key, &found, &st_idx, NULL));
    if (found == FALSE) {
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "Flowtracker state transition key "
                        " is not configured\n\r")));
        return BCM_E_NOT_FOUND;
    }

    sw_ent = BCMI_FT_STATE_TRANSITION_SW_ENTRY(unit, st_idx);
    num_avail_checks = BCMI_FT_FLOWCHECKER_MAX_ID(unit);
    typ = BCM_FT_IDX_TYPE_CHECK;
    for (i = 0; i < num_avail_checks; i++) {
        if (!SHR_BITGET(sw_ent->check_bmp, i)) {
            continue;
        }

        if (check_id_list != NULL) {
            check_id_list[curr_idx] = BCMI_FT_FLOWCHECKER_INDEX_SET(i, typ);
        }

        curr_idx++;
        if ((max_checks != 0) && (curr_idx == max_checks)) {
            break;
        }
    }

    *actual_checks = curr_idx;

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcmi_ftv3_check_state_transition_mapped
 * Purpose:
 *  Check if flowcheck is mapped to flow state transition key.
 * Parameters:
 *  unit          - (IN) BCM device number
 *  check_id      - (IN) Flowtracker check id
 *  is_mapped     - (OUT) Is check mapped to state transition key
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int
bcmi_ftv3_check_state_transition_mapped(int unit,
    bcm_flowtracker_check_t check_id,
    int *is_mapped)
{
    int idx;
    int check_idx;
    int num_entries;
    bcmi_ft_state_transition_entry_t *sw_ent = NULL;

    if (is_mapped == NULL) {
        return BCM_E_NONE;
    }

    *is_mapped = FALSE;
    check_idx = BCMI_FT_FLOWCHECKER_INDEX_GET(check_id);
    num_entries = BCMI_FT_STATE_TRANSITION_NUM_ENTRIES(unit);
    for (idx = 0; idx < num_entries; idx++) {
        sw_ent = BCMI_FT_STATE_TRANSITION_SW_ENTRY(unit, idx);
        if (sw_ent == NULL) {
            continue;
        }
        if (SHR_BITGET(sw_ent->check_bmp, check_idx)) {
            *is_mapped = TRUE;
            break;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *  bcmi_ftv3_flow_transition_group_map_check
 * Purpose:
 *  Check if group has any check mapped to flow state transition key
 * Parameters:
 *  unit          - (IN) BCM device number
 *  id            - (IN) Flowtracker group id
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int
bcmi_ftv3_flow_transition_group_map_check(
    int unit,
    bcm_flowtracker_group_t id)
{
    int found = FALSE;
    bcm_flowtracker_check_t check_id;
    bcmi_ft_flowchecker_list_t *check_list = NULL;
    bcmi_flowtracker_flowchecker_info_t fc_info;
    bcm_flowtracker_group_type_t group_type;

    /* State transition is supported only on AIFT groups */
    group_type = BCM_FLOWTRACKER_GROUP_TYPE_GET(id);
    if (group_type != bcmFlowtrackerGroupTypeAggregateIngress) {
        return BCM_E_NONE;
    }

    if (soc_feature(unit,
                soc_feature_flex_flowtracker_state_transition)) {

        check_list = BCMI_FT_GROUP_FLOWCHECKER_HEAD(unit, id);
        while (check_list != NULL) {
            check_id = check_list->flowchecker_id;
            BCM_IF_ERROR_RETURN(
                    bcmi_ft_flowchecker_get(unit, check_id, &fc_info));
            if (!(fc_info.check1.flags &
                        BCM_FLOWTRACKER_CHECK_STATE_TRANSITION)) {

                /* Move to next check in the list */
                check_list = check_list->next;
                continue;
            }

            /* Check must be linked with atleast 1 state transition key */
            BCM_IF_ERROR_RETURN(
                    bcmi_ftv3_check_state_transition_mapped(unit,
                        check_id, &found));
            if (found == FALSE) {
                LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                                "Flowtracker Check (0x%x) must be associated"
                                " with atleast one state transition"
                                " entry\n\r"), check_id));
                return BCM_E_CONFIG;
            }

            /* Move to next check in the list */
            check_list = check_list->next;
        }
    }

    return BCM_E_NONE;
}

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
    int idx;
    int flag_found;
    int update_policy = FALSE;
    uint32 alu_trigger;
    uint32 ref_cnt;
    bsc_dt_flex_state_transition_table_entry_t table_entry;
    soc_mem_t mem = BSC_DT_FLEX_STATE_TRANSITION_TABLEm;
    bcmi_ft_state_transition_entry_t *sw_ent = NULL;

    sal_memset(&table_entry, 0,
            sizeof(bsc_dt_flex_state_transition_table_entry_t));

    idx = 0;
    alu_trigger = 0;
    flag_found = FALSE;
    while (state_transition_data[idx].current_state != -1) {
        if (state_transition_data[idx].event_flags & flag) {

            sw_ent = BCMI_FT_STATE_TRANSITION_SW_ENTRY(unit, idx);
            update_policy = FALSE;
            ref_cnt = sw_ent->agg_alu32_ref_cnt[alu_bank];

            if (set) {
                sw_ent->agg_alu32_ref_cnt[alu_bank]++;
                if (ref_cnt == 0) {
                    update_policy = TRUE;
                }
            } else {
                if (ref_cnt != 0) {
                    sw_ent->agg_alu32_ref_cnt[alu_bank]--;
                    if (ref_cnt == 1) {
                        update_policy = TRUE;
                    }
                }
            }

            if (update_policy == TRUE) {
                BCM_IF_ERROR_RETURN(
                        soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx,
                            &table_entry));
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
                        (unit, MEM_BLOCK_ANY, idx, &table_entry));
            }

            flag_found = TRUE;
        }
        idx++;
    }

    if (flag_found == FALSE) {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/* Function:
 *   bcmi_ftv3_flex_state_transition_alu_trigger_set
 * Purpose:
 *   Get table based on Group Type.
 * Parameters:
 *   unit - (IN) BCM device id
 *   check_id - (IN) Flowtracker Check id
 *   alu_bank - (IN) ALU memory bank
 *   set - (IN) Set or Reset
 * Returns:
 *   BCM_E_NONE - If successful
 *   BCM_E_xxx
 */
int
bcmi_ftv3_flex_state_transition_alu_trigger_set(
    int unit,
    bcm_flowtracker_check_t check_id,
    int alu_bank,
    int set)
{
    int st_idx, check_idx;
    uint32 num_entries;
    int update_policy = FALSE;
    uint32 alu_trigger;
    uint32 ref_cnt;
    bsc_dt_flex_state_transition_table_entry_t table_entry;
    soc_mem_t mem = BSC_DT_FLEX_STATE_TRANSITION_TABLEm;
    bcmi_ft_state_transition_entry_t *sw_ent = NULL;

    sal_memset(&table_entry, 0,
            sizeof(bsc_dt_flex_state_transition_table_entry_t));

    check_idx = BCMI_FT_FLOWCHECKER_INDEX_GET(check_id);
    num_entries = BCMI_FT_STATE_TRANSITION_NUM_ENTRIES(unit);
    for (st_idx = 0; st_idx < num_entries; st_idx++) {
        sw_ent = BCMI_FT_STATE_TRANSITION_SW_ENTRY(unit, st_idx);
        if (sw_ent == NULL) {
            continue;
        }
        if (!SHR_BITGET(sw_ent->check_bmp, check_idx)) {
            continue;
        }

        ref_cnt = sw_ent->agg_alu32_ref_cnt[alu_bank];

        update_policy = FALSE;
        if (set) {
            sw_ent->agg_alu32_ref_cnt[alu_bank]++;
            if (ref_cnt == 0) {
                update_policy = TRUE;
            }
        } else {
            if (ref_cnt != 0) {
                sw_ent->agg_alu32_ref_cnt[alu_bank]--;
                if (ref_cnt == 1) {
                    update_policy = TRUE;
                }
            }
        }

        if (update_policy == TRUE) {
            BCM_IF_ERROR_RETURN(
                    soc_mem_read(unit, mem, MEM_BLOCK_ANY, st_idx,
                        &table_entry));
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
                    (unit, MEM_BLOCK_ANY, st_idx, &table_entry));
        }
    }

    return BCM_E_NONE;
}
#endif /* BCM_FLOWTRACKER_V3_SUPPORT */

#endif /* BCM_FLOWTRACKER_SUPPORT */
