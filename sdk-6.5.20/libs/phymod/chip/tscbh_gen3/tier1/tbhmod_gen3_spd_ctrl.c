/*------------------------------------------------------------------------------
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *  Broadcom Corporation
 *  Proprietary and Confidential information
 *  All rights reserved
 *  This source file is the property of Broadcom Corporation, and
 *  may not be copied or distributed in any isomorphic form without the
 *  prior written consent of Broadcom Corporation.
 *------------------------------------------------------------------------------
 *  Description: Functons for Speed Table, AM Table, and UM Table access.
 *----------------------------------------------------------------------------*/
#ifndef _TBHMOD_GEN3_SPD_CTRL_C_
#define _TBHMOD_GEN3_SPD_CTRL_C_

#include "tbhmod_gen3_spd_ctrl_defines.h"
#include "tbhmod_gen3_spd_ctrl.h"

/*
 * spd_ctrl_unpack_spd_id_tbl_entry() : Unpacks a char array into a spd_id_tbl_entry_t
 */
void tbhmod_gen3_spd_ctrl_unpack_spd_id_tbl_entry(uint32_t *packed_entry, tbhmod_gen3_spd_id_tbl_entry_t *entry)
{
    int word_size = 32;
    int word_idx;
    int bit_idx;
    int high_bits;
    int low_bits;
    int high_mask;
    int low_mask;

    /* AM_TABLE_INDEX */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_AM_TABLE_INDEX_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_AM_TABLE_INDEX_OFFSET % word_size;
    entry->am_table_index = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_AM_TABLE_INDEX_MASK;

    /* FEC_ARCH */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_FEC_ARCH_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_FEC_ARCH_OFFSET % word_size;
    entry->fec_arch = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_FEC_ARCH_MASK;

    /* SYMBOL_INTERLEAVE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_SYMBOL_INTERLEAVE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_SYMBOL_INTERLEAVE_OFFSET % word_size;
    entry->symbol_interleave = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_SYMBOL_INTERLEAVE_MASK;

    /* TC_XOR_CONTROL */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_TC_XOR_CONTROL_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_TC_XOR_CONTROL_OFFSET % word_size;
    entry->tc_xor_control = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_TC_XOR_CONTROL_MASK;

    /* RS_FEC_SYNC_HEADER_MODE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_RS_FEC_SYNC_HEADER_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_RS_FEC_SYNC_HEADER_MODE_OFFSET % word_size;
    entry->rs_fec_sync_header_mode = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_RS_FEC_SYNC_HEADER_MODE_MASK;

    /* RS_FEC_CWM_NIBBLE_MATCH_COUNT */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_RS_FEC_CWM_NIBBLE_MATCH_COUNT_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_RS_FEC_CWM_NIBBLE_MATCH_COUNT_OFFSET % word_size;
    entry->rs_fec_cwm_nibble_match_count = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_RS_FEC_CWM_NIBBLE_MATCH_COUNT_MASK;

    /* BASE_R_FEC_SYNC_HEADER_MODE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_BASE_R_FEC_SYNC_HEADER_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_BASE_R_FEC_SYNC_HEADER_MODE_OFFSET % word_size;
    entry->base_r_fec_sync_header_mode = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_BASE_R_FEC_SYNC_HEADER_MODE_MASK;

    /* RS_FEC_SYMBOL_ERROR_WINDOW_MODE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_RS_FEC_SYMBOL_ERROR_WINDOW_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_RS_FEC_SYMBOL_ERROR_WINDOW_MODE_OFFSET % word_size;
    entry->rs_fec_symbol_error_window_mode = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_RS_FEC_SYMBOL_ERROR_WINDOW_MODE_MASK;

    /* RS_FEC_SYMBOL_ERROR_COUNT_THRESHOLD */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_RS_FEC_SYMBOL_ERROR_COUNT_THRESHOLD_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_RS_FEC_SYMBOL_ERROR_COUNT_THRESHOLD_OFFSET % word_size;
    low_bits = word_size - bit_idx;
    low_mask = (1 << low_bits)  -1;
    high_bits = TBHMOD_GEN3_SPD_ID_TBL_RS_FEC_SYMBOL_ERROR_COUNT_THRESHOLD_WIDTH - low_bits;
    high_mask = (1 << high_bits) - 1;
    entry->rs_fec_symbol_error_count_threshold = (packed_entry[word_idx] >> bit_idx) & low_mask;
    entry->rs_fec_symbol_error_count_threshold |= (packed_entry[word_idx + 1] & high_mask) << low_bits;

    /* DESKEW_FORWARDING_THRESHOLD_2XN */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_DESKEW_FORWARDING_THRESHOLD_2XN_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_DESKEW_FORWARDING_THRESHOLD_2XN_OFFSET % word_size;
    entry->deskew_forwarding_threshold_2xn = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_DESKEW_FORWARDING_THRESHOLD_2XN_MASK;

    /* DESKEW_FORWARDING_THRESHOLD_1XN */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_DESKEW_FORWARDING_THRESHOLD_1XN_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_DESKEW_FORWARDING_THRESHOLD_1XN_OFFSET % word_size;
    entry->deskew_forwarding_threshold_1xn = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_DESKEW_FORWARDING_THRESHOLD_1XN_MASK;

    /* BIT_MUX_MODE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_BIT_MUX_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_BIT_MUX_MODE_OFFSET % word_size;
    entry->bit_mux_mode = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_BIT_MUX_MODE_MASK;

    /* NUM_LANES */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_NUM_LANES_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_NUM_LANES_OFFSET % word_size;
    entry->num_lanes = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_NUM_LANES_MASK;

    /* DESKEW_WINDOW_WITHOUT_RS_FEC */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_DESKEW_WINDOW_WITHOUT_RS_FEC_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_DESKEW_WINDOW_WITHOUT_RS_FEC_OFFSET % word_size;
    entry->deskew_window_without_rs_fec = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_DESKEW_WINDOW_WITHOUT_RS_FEC_MASK;

    /* DESKEW_WINDOW_WITH_RS_FEC */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_DESKEW_WINDOW_WITH_RS_FEC_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_DESKEW_WINDOW_WITH_RS_FEC_OFFSET % word_size;
    entry->deskew_window_with_rs_fec = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_DESKEW_WINDOW_WITH_RS_FEC_MASK;

    /* BER_FSM_DISABLE_WITH_RS_FEC */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_BER_FSM_DISABLE_WITH_RS_FEC_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_BER_FSM_DISABLE_WITH_RS_FEC_OFFSET % word_size;
    entry->ber_fsm_disable_with_rs_fec = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_BER_FSM_DISABLE_WITH_RS_FEC_MASK;

    /* BER_FSM_DISABLE_WITHOUT_RS_FEC */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_BER_FSM_DISABLE_WITHOUT_RS_FEC_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_BER_FSM_DISABLE_WITHOUT_RS_FEC_OFFSET % word_size;
    entry->ber_fsm_disable_without_rs_fec = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_BER_FSM_DISABLE_WITHOUT_RS_FEC_MASK;

    /* BER_WINDOW_MODE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_BER_WINDOW_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_BER_WINDOW_MODE_OFFSET % word_size;
    entry->ber_window_mode = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_BER_WINDOW_MODE_MASK;

    /* BER_TRIGGER_COUNT */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_BER_TRIGGER_COUNT_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_BER_TRIGGER_COUNT_OFFSET % word_size;
    entry->ber_trigger_count = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_BER_TRIGGER_COUNT_MASK;

    /* CREDIT_QUOTIENT */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_CREDIT_QUOTIENT_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_CREDIT_QUOTIENT_OFFSET % word_size;
    entry->credit_quotient = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_CREDIT_QUOTIENT_MASK;

    /* CREDIT_REMAINDER */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_CREDIT_REMAINDER_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_CREDIT_REMAINDER_OFFSET % word_size;
    entry->credit_remainder = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_CREDIT_REMAINDER_MASK;

    /* CREDIT_DIVISOR */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_CREDIT_DIVISOR_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_CREDIT_DIVISOR_OFFSET % word_size;
    low_bits = word_size - bit_idx;
    low_mask = (1 << low_bits)  -1;
    high_bits = TBHMOD_GEN3_SPD_ID_TBL_CREDIT_DIVISOR_WIDTH - low_bits;
    high_mask = (1 << high_bits) - 1;
    entry->credit_divisor = (packed_entry[word_idx] >> bit_idx) & low_mask;
    entry->credit_divisor |= (packed_entry[word_idx + 1] & high_mask) << low_bits;

    /* USE_CL49_BLOCK_SYNC */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_USE_CL49_BLOCK_SYNC_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_USE_CL49_BLOCK_SYNC_OFFSET % word_size;
    entry->use_cl49_block_sync = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_USE_CL49_BLOCK_SYNC_MASK;

    /* SCR_MODE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_SCR_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_SCR_MODE_OFFSET % word_size;
    entry->scr_mode = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_SCR_MODE_MASK;

    /* CODEC_MODE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_CODEC_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_CODEC_MODE_OFFSET % word_size;
    entry->codec_mode = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_CODEC_MODE_MASK;

    /* TRAINING_EN */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_TRAINING_EN_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_TRAINING_EN_OFFSET % word_size;
    entry->training_en = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_TRAINING_EN_MASK;

    /* AN_TIMER_SELECT */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_AN_TIMER_SELECT_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_AN_TIMER_SELECT_OFFSET % word_size;
    entry->an_timer_select = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_AN_TIMER_SELECT_MASK;

    /* PMD_PAM4_MODE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_PMD_PAM4_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_PMD_PAM4_MODE_OFFSET % word_size;
    entry->pmd_pam4_mode = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_PMD_PAM4_MODE_MASK;

    /* PMD_OSR_MODE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_PMD_OSR_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_PMD_OSR_MODE_OFFSET % word_size;
    entry->pmd_osr_mode = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_PMD_OSR_MODE_MASK;

    /* T_PMA_WATERMARK */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_T_PMA_WATERMARK_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_T_PMA_WATERMARK_OFFSET % word_size;
    entry->t_pma_watermark = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_T_PMA_WATERMARK_MASK;

    /* L_TPMA_WATERMARK */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_L_TPMA_WATERMARK_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_L_TPMA_WATERMARK_OFFSET % word_size;
    entry->l_tpma_watermark = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_L_TPMA_WATERMARK_MASK;

    /* LOOP_BLOCK_COUNT_FOR_TS */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_LOOP_BLOCK_COUNT_FOR_TS_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_LOOP_BLOCK_COUNT_FOR_TS_OFFSET % word_size;
    low_bits = word_size - bit_idx;
    low_mask = (1 << low_bits)  -1;
    high_bits = TBHMOD_GEN3_SPD_ID_TBL_LOOP_BLOCK_COUNT_FOR_TS_WIDTH - low_bits;
    high_mask = (1 << high_bits) - 1;
    entry->loop_block_count_for_ts = (packed_entry[word_idx] >> bit_idx) & low_mask;
    entry->loop_block_count_for_ts |= (packed_entry[word_idx + 1] & high_mask) << low_bits;

    /* LOOP_BIT_COUNT_FOR_TS */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_LOOP_BIT_COUNT_FOR_TS_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_LOOP_BIT_COUNT_FOR_TS_OFFSET % word_size;
    entry->loop_bit_count_for_ts = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_LOOP_BIT_COUNT_FOR_TS_MASK;

    /* AM_LOCK_FSM_MODE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_AM_LOCK_FSM_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_AM_LOCK_FSM_MODE_OFFSET % word_size;
    entry->am_lock_fsm_mode = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_AM_LOCK_FSM_MODE_MASK;

    /* T_PMA_START_MODE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_T_PMA_START_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_T_PMA_START_MODE_OFFSET % word_size;
    entry->t_pma_start_mode = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_T_PMA_START_MODE_MASK;

    /* HI_SER_ENABLE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_HI_SER_ENABLE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_HI_SER_ENABLE_OFFSET % word_size;
    entry->hi_ser_enable = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_HI_SER_ENABLE_MASK;

    /* T_PMA_INPUT_WIDTH_MODE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_T_PMA_INPUT_WIDTH_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_T_PMA_INPUT_WIDTH_MODE_OFFSET % word_size;
    entry->t_pma_input_width_mode = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_T_PMA_INPUT_WIDTH_MODE_MASK;

    /* T_PMA_OUTPUT_WIDTH_MODE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_T_PMA_OUTPUT_WIDTH_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_T_PMA_OUTPUT_WIDTH_MODE_OFFSET % word_size;
    entry->t_pma_output_width_mode = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_T_PMA_OUTPUT_WIDTH_MODE_MASK;

    /* BIT_MUX_MODE_MODIFIER */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_BIT_MUX_MODE_MODIFIER_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_BIT_MUX_MODE_MODIFIER_OFFSET % word_size;
    entry->bit_mux_mode_modifier = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_BIT_MUX_MODE_MODIFIER_MASK;

    /* NUM_LANES_MODIFIER */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_NUM_LANES_MODIFIER_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_NUM_LANES_MODIFIER_OFFSET % word_size;
    entry->num_lanes_modifier = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_NUM_LANES_MODIFIER_MASK;

    /* MAPPING_SEL */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_MAPPING_SEL_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_MAPPING_SEL_OFFSET % word_size;
    entry->mapping_sel = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_SPD_ID_TBL_MAPPING_SEL_MASK;

    return;
}

/*
 * spd_ctrl_pack_spd_id_tbl_entry() : Pack a spd_id_tbl_entry_t into an array of 32-bit blocks
 */
void tbhmod_gen3_spd_ctrl_pack_spd_id_tbl_entry(tbhmod_gen3_spd_id_tbl_entry_t *entry, uint32_t *packed_entry)
{
    int num_words = 6;
    int word_size = 32;
    int word_idx = 0;
    int bit_idx = 0;
    int high_bits;
    int low_bits;
    int high_mask;
    int low_mask;
    int i;

    /* Initialize the packed_entry array to 0's */
    for (i = 0; i < num_words; i++) {
        packed_entry[i] = 0;
    }

    /* AM_TABLE_INDEX */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_AM_TABLE_INDEX_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_AM_TABLE_INDEX_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->am_table_index & TBHMOD_GEN3_SPD_ID_TBL_AM_TABLE_INDEX_MASK) << bit_idx;

    /* FEC_ARCH */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_FEC_ARCH_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_FEC_ARCH_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->fec_arch & TBHMOD_GEN3_SPD_ID_TBL_FEC_ARCH_MASK) << bit_idx;

    /* SYMBOL_INTERLEAVE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_SYMBOL_INTERLEAVE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_SYMBOL_INTERLEAVE_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->symbol_interleave & TBHMOD_GEN3_SPD_ID_TBL_SYMBOL_INTERLEAVE_MASK) << bit_idx;

    /* TC_XOR_CONTROL */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_TC_XOR_CONTROL_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_TC_XOR_CONTROL_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->tc_xor_control & TBHMOD_GEN3_SPD_ID_TBL_TC_XOR_CONTROL_MASK) << bit_idx;

    /* RS_FEC_SYNC_HEADER_MODE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_RS_FEC_SYNC_HEADER_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_RS_FEC_SYNC_HEADER_MODE_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->rs_fec_sync_header_mode & TBHMOD_GEN3_SPD_ID_TBL_RS_FEC_SYNC_HEADER_MODE_MASK) << bit_idx;

    /* RS_FEC_CWM_NIBBLE_MATCH_COUNT */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_RS_FEC_CWM_NIBBLE_MATCH_COUNT_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_RS_FEC_CWM_NIBBLE_MATCH_COUNT_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->rs_fec_cwm_nibble_match_count & TBHMOD_GEN3_SPD_ID_TBL_RS_FEC_CWM_NIBBLE_MATCH_COUNT_MASK) << bit_idx;

    /* BASE_R_FEC_SYNC_HEADER_MODE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_BASE_R_FEC_SYNC_HEADER_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_BASE_R_FEC_SYNC_HEADER_MODE_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->base_r_fec_sync_header_mode & TBHMOD_GEN3_SPD_ID_TBL_BASE_R_FEC_SYNC_HEADER_MODE_MASK) << bit_idx;

    /* RS_FEC_SYMBOL_ERROR_WINDOW_MODE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_RS_FEC_SYMBOL_ERROR_WINDOW_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_RS_FEC_SYMBOL_ERROR_WINDOW_MODE_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->rs_fec_symbol_error_window_mode & TBHMOD_GEN3_SPD_ID_TBL_RS_FEC_SYMBOL_ERROR_WINDOW_MODE_MASK) << bit_idx;

    /* RS_FEC_SYMBOL_ERROR_COUNT_THRESHOLD */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_RS_FEC_SYMBOL_ERROR_COUNT_THRESHOLD_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_RS_FEC_SYMBOL_ERROR_COUNT_THRESHOLD_OFFSET % word_size;
    low_bits = word_size - bit_idx;
    low_mask = (1 << low_bits)  -1;
    high_bits = TBHMOD_GEN3_SPD_ID_TBL_RS_FEC_SYMBOL_ERROR_COUNT_THRESHOLD_WIDTH - low_bits;
    high_mask = (1 << high_bits) - 1;
    packed_entry[word_idx] |= (entry->rs_fec_symbol_error_count_threshold & low_mask) << bit_idx;
    packed_entry[word_idx + 1] |= (entry->rs_fec_symbol_error_count_threshold >> low_bits) & high_mask;

    /* DESKEW_FORWARDING_THRESHOLD_2XN */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_DESKEW_FORWARDING_THRESHOLD_2XN_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_DESKEW_FORWARDING_THRESHOLD_2XN_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->deskew_forwarding_threshold_2xn & TBHMOD_GEN3_SPD_ID_TBL_DESKEW_FORWARDING_THRESHOLD_2XN_MASK) << bit_idx;

    /* DESKEW_FORWARDING_THRESHOLD_1XN */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_DESKEW_FORWARDING_THRESHOLD_1XN_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_DESKEW_FORWARDING_THRESHOLD_1XN_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->deskew_forwarding_threshold_1xn & TBHMOD_GEN3_SPD_ID_TBL_DESKEW_FORWARDING_THRESHOLD_1XN_MASK) << bit_idx;

    /* BIT_MUX_MODE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_BIT_MUX_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_BIT_MUX_MODE_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->bit_mux_mode & TBHMOD_GEN3_SPD_ID_TBL_BIT_MUX_MODE_MASK) << bit_idx;

    /* NUM_LANES */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_NUM_LANES_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_NUM_LANES_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->num_lanes & TBHMOD_GEN3_SPD_ID_TBL_NUM_LANES_MASK) << bit_idx;

    /* DESKEW_WINDOW_WITHOUT_RS_FEC */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_DESKEW_WINDOW_WITHOUT_RS_FEC_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_DESKEW_WINDOW_WITHOUT_RS_FEC_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->deskew_window_without_rs_fec & TBHMOD_GEN3_SPD_ID_TBL_DESKEW_WINDOW_WITHOUT_RS_FEC_MASK) << bit_idx;

    /* DESKEW_WINDOW_WITH_RS_FEC */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_DESKEW_WINDOW_WITH_RS_FEC_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_DESKEW_WINDOW_WITH_RS_FEC_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->deskew_window_with_rs_fec & TBHMOD_GEN3_SPD_ID_TBL_DESKEW_WINDOW_WITH_RS_FEC_MASK) << bit_idx;

    /* BER_FSM_DISABLE_WITH_RS_FEC */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_BER_FSM_DISABLE_WITH_RS_FEC_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_BER_FSM_DISABLE_WITH_RS_FEC_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->ber_fsm_disable_with_rs_fec & TBHMOD_GEN3_SPD_ID_TBL_BER_FSM_DISABLE_WITH_RS_FEC_MASK) << bit_idx;

    /* BER_FSM_DISABLE_WITHOUT_RS_FEC */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_BER_FSM_DISABLE_WITHOUT_RS_FEC_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_BER_FSM_DISABLE_WITHOUT_RS_FEC_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->ber_fsm_disable_without_rs_fec & TBHMOD_GEN3_SPD_ID_TBL_BER_FSM_DISABLE_WITHOUT_RS_FEC_MASK) << bit_idx;

    /* BER_WINDOW_MODE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_BER_WINDOW_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_BER_WINDOW_MODE_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->ber_window_mode & TBHMOD_GEN3_SPD_ID_TBL_BER_WINDOW_MODE_MASK) << bit_idx;

    /* BER_TRIGGER_COUNT */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_BER_TRIGGER_COUNT_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_BER_TRIGGER_COUNT_OFFSET % word_size;
        packed_entry[word_idx] |= (entry->ber_trigger_count & TBHMOD_GEN3_SPD_ID_TBL_BER_TRIGGER_COUNT_MASK) << bit_idx;

    /* CREDIT_QUOTIENT */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_CREDIT_QUOTIENT_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_CREDIT_QUOTIENT_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->credit_quotient & TBHMOD_GEN3_SPD_ID_TBL_CREDIT_QUOTIENT_MASK) << bit_idx;

    /* CREDIT_REMAINDER */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_CREDIT_REMAINDER_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_CREDIT_REMAINDER_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->credit_remainder & TBHMOD_GEN3_SPD_ID_TBL_CREDIT_REMAINDER_MASK) << bit_idx;

    /* CREDIT_DIVISOR */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_CREDIT_DIVISOR_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_CREDIT_DIVISOR_OFFSET % word_size;
    low_bits = word_size - bit_idx;
    low_mask = (1 << low_bits)  -1;
    high_bits = TBHMOD_GEN3_SPD_ID_TBL_CREDIT_DIVISOR_WIDTH - low_bits;
    high_mask = (1 << high_bits) - 1;
    packed_entry[word_idx] |= (entry->credit_divisor & low_mask) << bit_idx;
    packed_entry[word_idx + 1] |= (entry->credit_divisor >> low_bits) & high_mask;

    /* USE_CL49_BLOCK_SYNC */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_USE_CL49_BLOCK_SYNC_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_USE_CL49_BLOCK_SYNC_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->use_cl49_block_sync & TBHMOD_GEN3_SPD_ID_TBL_USE_CL49_BLOCK_SYNC_MASK) << bit_idx;

    /* SCR_MODE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_SCR_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_SCR_MODE_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->scr_mode & TBHMOD_GEN3_SPD_ID_TBL_SCR_MODE_MASK) << bit_idx;

    /* CODEC_MODE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_CODEC_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_CODEC_MODE_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->codec_mode & TBHMOD_GEN3_SPD_ID_TBL_CODEC_MODE_MASK) << bit_idx;

    /* TRAINING_EN */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_TRAINING_EN_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_TRAINING_EN_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->training_en & TBHMOD_GEN3_SPD_ID_TBL_TRAINING_EN_MASK) << bit_idx;

    /* AN_TIMER_SELECT */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_AN_TIMER_SELECT_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_AN_TIMER_SELECT_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->an_timer_select & TBHMOD_GEN3_SPD_ID_TBL_AN_TIMER_SELECT_MASK) << bit_idx;

    /* PMD_PAM4_MODE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_PMD_PAM4_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_PMD_PAM4_MODE_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->pmd_pam4_mode & TBHMOD_GEN3_SPD_ID_TBL_PMD_PAM4_MODE_MASK) << bit_idx;

    /* PMD_OSR_MODE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_PMD_OSR_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_PMD_OSR_MODE_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->pmd_osr_mode & TBHMOD_GEN3_SPD_ID_TBL_PMD_OSR_MODE_MASK) << bit_idx;

    /* T_PMA_WATERMARK */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_T_PMA_WATERMARK_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_T_PMA_WATERMARK_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->t_pma_watermark & TBHMOD_GEN3_SPD_ID_TBL_T_PMA_WATERMARK_MASK) << bit_idx;

    /* L_TPMA_WATERMARK */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_L_TPMA_WATERMARK_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_L_TPMA_WATERMARK_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->l_tpma_watermark & TBHMOD_GEN3_SPD_ID_TBL_L_TPMA_WATERMARK_MASK) << bit_idx;

    /* LOOP_BLOCK_COUNT_FOR_TS */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_LOOP_BLOCK_COUNT_FOR_TS_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_LOOP_BLOCK_COUNT_FOR_TS_OFFSET % word_size;
    low_bits = word_size - bit_idx;
    low_mask = (1 << low_bits)  -1;
    high_bits = TBHMOD_GEN3_SPD_ID_TBL_LOOP_BLOCK_COUNT_FOR_TS_WIDTH - low_bits;
    high_mask = (1 << high_bits) - 1;
    packed_entry[word_idx] |= (entry->loop_block_count_for_ts & low_mask) << bit_idx;
    packed_entry[word_idx + 1] |= (entry->loop_block_count_for_ts >> low_bits) & high_mask;

    /* LOOP_BIT_COUNT_FOR_TS */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_LOOP_BIT_COUNT_FOR_TS_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_LOOP_BIT_COUNT_FOR_TS_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->loop_bit_count_for_ts & TBHMOD_GEN3_SPD_ID_TBL_LOOP_BIT_COUNT_FOR_TS_MASK) << bit_idx;

    /* AM_LOCK_FSM_MODE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_AM_LOCK_FSM_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_AM_LOCK_FSM_MODE_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->am_lock_fsm_mode & TBHMOD_GEN3_SPD_ID_TBL_AM_LOCK_FSM_MODE_MASK) << bit_idx;

    /* T_PMA_START_MODE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_T_PMA_START_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_T_PMA_START_MODE_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->t_pma_start_mode & TBHMOD_GEN3_SPD_ID_TBL_T_PMA_START_MODE_MASK) << bit_idx;

    /* HI_SER_ENABLE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_HI_SER_ENABLE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_HI_SER_ENABLE_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->hi_ser_enable & TBHMOD_GEN3_SPD_ID_TBL_HI_SER_ENABLE_MASK) << bit_idx;


    /* T_PMA_INPUT_WIDTH_MODE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_T_PMA_INPUT_WIDTH_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_T_PMA_INPUT_WIDTH_MODE_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->t_pma_input_width_mode & TBHMOD_GEN3_SPD_ID_TBL_T_PMA_INPUT_WIDTH_MODE_MASK) << bit_idx;

    /* T_PMA_OUTPUT_WIDTH_MODE */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_T_PMA_OUTPUT_WIDTH_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_T_PMA_OUTPUT_WIDTH_MODE_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->t_pma_output_width_mode & TBHMOD_GEN3_SPD_ID_TBL_T_PMA_OUTPUT_WIDTH_MODE_MASK) << bit_idx;

    /* BIT_MUX_MODE_MODIFIER */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_BIT_MUX_MODE_MODIFIER_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_BIT_MUX_MODE_MODIFIER_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->bit_mux_mode_modifier & TBHMOD_GEN3_SPD_ID_TBL_BIT_MUX_MODE_MODIFIER_MASK) << bit_idx;

    /* NUM_LANES_MODIFIER */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_NUM_LANES_MODIFIER_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_NUM_LANES_MODIFIER_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->num_lanes_modifier & TBHMOD_GEN3_SPD_ID_TBL_NUM_LANES_MODIFIER_MASK) << bit_idx;

    /* MAPPING_SEL */
    word_idx = TBHMOD_GEN3_SPD_ID_TBL_MAPPING_SEL_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_SPD_ID_TBL_MAPPING_SEL_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->mapping_sel & TBHMOD_GEN3_SPD_ID_TBL_MAPPING_SEL_MASK) << bit_idx;

    return;
}

/*
 * spd_ctrl_unpack_am_tbl_entry() : Unpacks a char array into an am_tbl_entry_t
 */
void tbhmod_gen3_spd_ctrl_unpack_am_tbl_entry(uint32_t *packed_entry, tbhmod_gen3_am_tbl_entry_t *entry)
{
    int word_size = 32;
    int word_idx;
    int bit_idx;
    int high_bits;
    int low_bits;
    int high_mask;
    int low_mask;

    /* AM_COMPRESSION_MODE */
    word_idx = TBHMOD_GEN3_AM_TBL_AM_COMPRESSION_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_AM_TBL_AM_COMPRESSION_MODE_OFFSET % word_size;
    entry->am_compression_mode = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_AM_TBL_AM_COMPRESSION_MODE_MASK;

    /* AM_SIZE */
    word_idx = TBHMOD_GEN3_AM_TBL_AM_SIZE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_AM_TBL_AM_SIZE_OFFSET % word_size;
    entry->am_size = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_AM_TBL_AM_SIZE_MASK;

    /* PAD_MODE */
    word_idx = TBHMOD_GEN3_AM_TBL_PAD_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_AM_TBL_PAD_MODE_OFFSET % word_size;
    entry->pad_mode = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_AM_TBL_PAD_MODE_MASK;

    /* UM_TABLE_COUNT */
    word_idx = TBHMOD_GEN3_AM_TBL_UM_TABLE_COUNT_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_AM_TBL_UM_TABLE_COUNT_OFFSET % word_size;
    entry->um_table_count = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_AM_TBL_UM_TABLE_COUNT_MASK;

    /* UM_TABLE_INDEX */
    word_idx = TBHMOD_GEN3_AM_TBL_UM_TABLE_INDEX_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_AM_TBL_UM_TABLE_INDEX_OFFSET % word_size;
    entry->um_table_index = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_AM_TBL_UM_TABLE_INDEX_MASK;

    /* CM */
    word_idx = TBHMOD_GEN3_AM_TBL_CM_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_AM_TBL_CM_OFFSET % word_size;
    low_bits = word_size - bit_idx;
    low_mask = (1 << low_bits)  -1;
    high_bits = TBHMOD_GEN3_AM_TBL_CM_WIDTH - low_bits;
    high_mask = (1 << high_bits) - 1;
    entry->cm = (packed_entry[word_idx] >> bit_idx) & low_mask;
    entry->cm |= (packed_entry[word_idx + 1] & high_mask) << low_bits;

    /* USE_FIXED */
    word_idx = TBHMOD_GEN3_AM_TBL_USE_FIXED_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_AM_TBL_USE_FIXED_OFFSET % word_size;
    entry->use_fixed = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_AM_TBL_USE_FIXED_MASK;

    /* AM_SPACING_RS */
    word_idx = TBHMOD_GEN3_AM_TBL_AM_SPACING_RS_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_AM_TBL_AM_SPACING_RS_OFFSET % word_size;
    entry->am_spacing_rs = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_AM_TBL_AM_SPACING_RS_MASK;

    /* AM_SPACING_NO_RS */
    word_idx = TBHMOD_GEN3_AM_TBL_AM_SPACING_NO_RS_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_AM_TBL_AM_SPACING_NO_RS_OFFSET % word_size;
    low_bits = word_size - bit_idx;
    low_mask = (1 << low_bits)  -1;
    high_bits = TBHMOD_GEN3_AM_TBL_AM_SPACING_NO_RS_WIDTH - low_bits;
    high_mask = (1 << high_bits) - 1;
    entry->am_spacing_no_rs = (packed_entry[word_idx] >> bit_idx) & low_mask;
    entry->am_spacing_no_rs |= (packed_entry[word_idx + 1] & high_mask) << low_bits;

    /* AM_SPACING_CREDITS */
    word_idx = TBHMOD_GEN3_AM_TBL_AM_SPACING_CREDITS_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_AM_TBL_AM_SPACING_CREDITS_OFFSET % word_size;
    entry->am_spacing_credits = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_AM_TBL_AM_SPACING_CREDITS_MASK;

    /* CM_AM_SEL */
    word_idx = TBHMOD_GEN3_AM_TBL_CM_AM_SEL_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_AM_TBL_CM_AM_SEL_OFFSET % word_size;
    entry->cm_am_sel = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_AM_TBL_CM_AM_SEL_MASK;

    return;
}


/*
 * spd_ctrl_pack_am_tbl_entry() : Packet an am_tbl_entry_t into an array of 32-bit blocks
 */
void tbhmod_gen3_spd_ctrl_pack_am_tbl_entry(tbhmod_gen3_am_tbl_entry_t *entry, uint32_t *packed_entry)
{
    int num_words = 3;
    int word_size = 32;
    int word_idx = 0;
    int bit_idx = 0;
    int high_bits;
    int low_bits;
    int high_mask;
    int low_mask;
    int i;

    /* Initialize the packed_entry array to 0's */
    for (i = 0; i < num_words; i++) {
        packed_entry[i] = 0;
    }

    /* AM_COMPRESSION_MODE */
    word_idx = TBHMOD_GEN3_AM_TBL_AM_COMPRESSION_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_AM_TBL_AM_COMPRESSION_MODE_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->am_compression_mode & TBHMOD_GEN3_AM_TBL_AM_COMPRESSION_MODE_MASK) << bit_idx;

    /* AM_SIZE */
    word_idx = TBHMOD_GEN3_AM_TBL_AM_SIZE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_AM_TBL_AM_SIZE_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->am_size & TBHMOD_GEN3_AM_TBL_AM_SIZE_MASK) << bit_idx;

    /* PAD_MODE */
    word_idx = TBHMOD_GEN3_AM_TBL_PAD_MODE_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_AM_TBL_PAD_MODE_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->pad_mode & TBHMOD_GEN3_AM_TBL_PAD_MODE_MASK) << bit_idx;

    /* UM_TABLE_COUNT */
    word_idx = TBHMOD_GEN3_AM_TBL_UM_TABLE_COUNT_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_AM_TBL_UM_TABLE_COUNT_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->um_table_count & TBHMOD_GEN3_AM_TBL_UM_TABLE_COUNT_MASK) << bit_idx;

    /* UM_TABLE_INDEX */
    word_idx = TBHMOD_GEN3_AM_TBL_UM_TABLE_INDEX_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_AM_TBL_UM_TABLE_INDEX_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->um_table_index & TBHMOD_GEN3_AM_TBL_UM_TABLE_INDEX_MASK) << bit_idx;

    /* CM */
    word_idx = TBHMOD_GEN3_AM_TBL_CM_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_AM_TBL_CM_OFFSET % word_size;
    low_bits  = 32 - bit_idx;
    low_mask  = (1 << low_bits) - 1;
    high_bits = TBHMOD_GEN3_AM_TBL_CM_WIDTH - low_bits;
    high_mask  = (1 << high_bits) - 1;
    packed_entry[word_idx] |= (entry->cm & low_mask) << bit_idx;
    packed_entry[word_idx + 1] |= (entry->cm & high_mask) >> low_bits;

    /* USE_FIXED */
    word_idx = TBHMOD_GEN3_AM_TBL_USE_FIXED_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_AM_TBL_USE_FIXED_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->use_fixed & TBHMOD_GEN3_AM_TBL_USE_FIXED_MASK) << bit_idx;

    /* AM_SPACING_RS */
    word_idx = TBHMOD_GEN3_AM_TBL_AM_SPACING_RS_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_AM_TBL_AM_SPACING_RS_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->am_spacing_rs & TBHMOD_GEN3_AM_TBL_AM_SPACING_RS_MASK) << bit_idx;

    /* AM_SPACING_NO_RS */
    word_idx = TBHMOD_GEN3_AM_TBL_AM_SPACING_NO_RS_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_AM_TBL_AM_SPACING_NO_RS_OFFSET % word_size;
    low_bits  = 32 - bit_idx;
    low_mask  = (1 << low_bits) - 1;
    high_bits = TBHMOD_GEN3_AM_TBL_AM_SPACING_NO_RS_WIDTH - low_bits;
    high_mask  = (1 << high_bits) - 1;
    packed_entry[word_idx] |= (entry->am_spacing_no_rs & low_mask) << bit_idx;
    packed_entry[word_idx + 1] |= (entry->am_spacing_no_rs & high_mask) >> low_bits;

    /* AM_SPACING_CREDITS */
    word_idx = TBHMOD_GEN3_AM_TBL_AM_SPACING_CREDITS_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_AM_TBL_AM_SPACING_CREDITS_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->am_spacing_credits & TBHMOD_GEN3_AM_TBL_AM_SPACING_CREDITS_MASK) << bit_idx;

    /* CM_AM_SEL */
    word_idx = TBHMOD_GEN3_AM_TBL_CM_AM_SEL_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_AM_TBL_CM_AM_SEL_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->cm_am_sel & TBHMOD_GEN3_AM_TBL_CM_AM_SEL_MASK) << bit_idx;


    return;
}

/*
 * spd_ctrl_unpack_um_tbl_entry() : Unpacks a uint8_t array into an um_tbl_entry_t
 */
void tbhmod_gen3_spd_ctrl_unpack_um_tbl_entry(uint32_t *packed_entry, tbhmod_gen3_um_tbl_entry_t *entry)
{
    int word_size = 32;
    int word_idx;
    int bit_idx;
    int high_bits;
    int low_bits;
    int high_mask;
    int low_mask;

    /* CM_CONTROL */
    word_idx = TBHMOD_GEN3_UM_TBL_CM_CONTROL_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_UM_TBL_CM_CONTROL_OFFSET % word_size;
    entry->cm_control = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_UM_TBL_CM_CONTROL_MASK;

    /* UM_PAD */
    word_idx = TBHMOD_GEN3_UM_TBL_UM_PAD_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_UM_TBL_UM_PAD_OFFSET % word_size;
    entry->um_pad = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_UM_TBL_UM_PAD_MASK;

    /* CM_PAD */
    word_idx = TBHMOD_GEN3_UM_TBL_CM_PAD_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_UM_TBL_CM_PAD_OFFSET % word_size;
    entry->cm_pad = (packed_entry[word_idx] >> bit_idx) & TBHMOD_GEN3_UM_TBL_CM_PAD_MASK;

    /* UM */
    word_idx = TBHMOD_GEN3_UM_TBL_UM_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_UM_TBL_UM_OFFSET % word_size;
    low_bits = word_size - bit_idx;
    low_mask = (1 << low_bits)  -1;
    high_bits = TBHMOD_GEN3_UM_TBL_UM_WIDTH - low_bits;
    high_mask =  (1 << high_bits) - 1;
    entry->um = (packed_entry[word_idx] >> bit_idx) & low_mask;
    entry->um |= (packed_entry[word_idx + 1] & high_mask) << low_bits;

    return;
}

/*
 * spd_ctrl_pack_um_tbl_entry() : Packet an am_tbl_entry_t into an array of 32-bit blocks
 */
void tbhmod_gen3_spd_ctrl_pack_um_tbl_entry(tbhmod_gen3_um_tbl_entry_t *entry, uint32_t *packed_entry)
{
    int num_words = 2;
    int word_size = 32;
    int word_idx = 0;
    int bit_idx = 0;
    int high_bits;
    int low_bits;
    int high_mask;
    int low_mask;
    int i;

    /* Initialize the packed_entry array to 0's */
    for (i = 0; i < num_words; i++) {
        packed_entry[i] = 0;
    }

    /* CM_CONTROL */
    word_idx = TBHMOD_GEN3_UM_TBL_CM_CONTROL_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_UM_TBL_CM_CONTROL_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->cm_control & TBHMOD_GEN3_UM_TBL_CM_CONTROL_MASK) << bit_idx;

    /* UM_PAD */
    word_idx = TBHMOD_GEN3_UM_TBL_UM_PAD_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_UM_TBL_UM_PAD_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->um_pad & TBHMOD_GEN3_UM_TBL_UM_PAD_MASK) << bit_idx;

    /* CM_PAD */
    word_idx = TBHMOD_GEN3_UM_TBL_CM_PAD_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_UM_TBL_CM_PAD_OFFSET % word_size;
    packed_entry[word_idx] |= (entry->cm_pad & TBHMOD_GEN3_UM_TBL_CM_PAD_MASK) << bit_idx;

    /* UM */
    word_idx = TBHMOD_GEN3_UM_TBL_UM_OFFSET / word_size;
    bit_idx  = TBHMOD_GEN3_UM_TBL_UM_OFFSET % word_size;
    low_bits  = word_size - bit_idx;
    low_mask  = (1 << low_bits) - 1;
    high_bits = TBHMOD_GEN3_UM_TBL_UM_WIDTH - low_bits;
    high_mask  = (1 << high_bits) - 1;
    packed_entry[word_idx] |= (entry->um & low_mask) << bit_idx;
    packed_entry[word_idx + 1] |= (entry->um >> low_bits) & high_mask;

    return;
}

#endif  /* _TBHMOD_GEN3_SPD_CTRL_C_ */
