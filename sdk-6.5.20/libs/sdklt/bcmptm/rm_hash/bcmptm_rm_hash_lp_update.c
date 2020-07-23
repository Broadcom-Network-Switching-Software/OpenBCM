/*! \file bcmptm_rm_hash_lp_update.c
 *
 * NGSDK
 *
 * Routines that implement hash lp table update.
 *
 * This file contains the functions that implements hash lp control word update.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_crc.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include "bcmptm_rm_hash.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMHASH

/* It is possible that each hash table has different base entry width. */
#define IFTA30_E2T_00_B0_SINGLE_WIDTH     120

/* It is possible that each hash table has different widest key width. */
#define IFTA30_E2T_00_B0_WIDEST_KEY       360

#define RM_HASH_MAX_BASE_ENTRY_NUM        16
#define RM_HASH_MAX_LOGICAL_ENTRY_NUM     4

/* Maximum entry size */
#define RM_HASH_MAX_PT_ENTRY_BITS         1024

#define MACRO0                            0x00
#define MACRO1                            0x01
#define MACRO2                            0x02
#define MACRO3                            0x03
/*******************************************************************************
 * Typedefs
 */
typedef struct rm_hash_ent_s {
    SHR_BITDCLNAME(e_words, RM_HASH_MAX_PT_ENTRY_BITS);
}rm_hash_ent_t;


/*
 *! \brief Entry content in a bucket to compute LP control word.
 *
 * This data structure describes entry content in a hash bucket.
 */
typedef struct rm_hash_lp_ctrl_info_s {
    /*! \brief Base entry buffer. */
    rm_hash_ent_t base_entry[RM_HASH_MAX_BASE_ENTRY_NUM];

    /*! \brief Pure logical entry buffer. */
    rm_hash_ent_t logical_entry[RM_HASH_MAX_LOGICAL_ENTRY_NUM];

    /*! \brief Logical entry with valid bit buffer. */
    rm_hash_ent_t valid_logical_entry[RM_HASH_MAX_LOGICAL_ENTRY_NUM];

    /*! \brief Key attribute table entry buffer. */
    rm_hash_ent_t key_attr;

    /*! \brief LP table entry buffer. */
    rm_hash_ent_t entry_lp;

    /*! \brief Flag indicating if the logical entry is valid */
    uint32_t logical_entry_valid[RM_HASH_MAX_LOGICAL_ENTRY_NUM];

    /*! \brief Key portion. */
    rm_hash_ent_t key[RM_HASH_MAX_LOGICAL_ENTRY_NUM];

    /*! \brief Hash control tab. */
    rm_hash_ent_t hash_control_tab;

    /*! \brief Base entry field. */
    rm_hash_ent_t base_entry_field;

    /*! \brief Null entry. */
    rm_hash_ent_t null_entry;
} rm_hash_lp_ctrl_info_t;

/*
 *! \brief Information used for computing LP serial control words.
 *
 * This data structure describes all the required information for LP CTRL word.
 */
typedef struct rm_hash_lp_serial_ctrl_word_s {
    uint32_t serial_tbp_0;

    uint32_t serial_tbp_0_polarity;

    uint32_t serial_macro_resolved_0;

    uint32_t serial_tbp_1;

    uint32_t serial_tbp_1_polarity;

    uint32_t serial_macro_resolved_1;

    uint32_t serial_tbp_2;

    uint32_t serial_tbp_2_polarity;

    uint32_t serial_macro_resolved_2;

    uint32_t serial_resolve_tbp_0;

    uint32_t serial_resolve_tbp_1;

    uint32_t serial_resolve_tbp_2;

    uint32_t serial_resolve_tbp_3;

    rm_hash_ent_t xor_0_1;

    rm_hash_ent_t xor_0_2;

    rm_hash_ent_t xor_0_3;

    rm_hash_ent_t xor_1_2;

    rm_hash_ent_t xor_1_3;

    rm_hash_ent_t xor_2_3;

    rm_hash_ent_t diff_0_1;

    rm_hash_ent_t diff_0_2;

    rm_hash_ent_t diff_0_3;

    rm_hash_ent_t diff_1_2;

    rm_hash_ent_t diff_1_3;

    rm_hash_ent_t diff_2_3;

    rm_hash_ent_t serial_resolve_bit_positions_0;

    rm_hash_ent_t serial_resolve_bit_positions_1;

    rm_hash_ent_t serial_resolve_bit_positions_2;

    rm_hash_ent_t serial_resolve_bit_positions_3;

    uint32_t serial_resolve_0_valid;

    uint32_t serial_resolve_1_valid;

    uint32_t serial_resolve_2_valid;

    uint32_t serial_resolve_3_valid;

    uint32_t hash_entry_0_bit_at_serial_resolve_tbp_0;

    uint32_t hash_entry_1_bit_at_serial_resolve_tbp_1;

    uint32_t hash_entry_2_bit_at_serial_resolve_tbp_2;

    uint32_t hash_entry_3_bit_at_serial_resolve_tbp_3;

    rm_hash_ent_t invalid_hash_entry_0;

    rm_hash_ent_t invalid_hash_entry_1;

    rm_hash_ent_t invalid_hash_entry_2;

    rm_hash_ent_t invalid_hash_entry_3;

    rm_hash_ent_t neg_invalid_hash_entry_0;

    rm_hash_ent_t neg_invalid_hash_entry_1;

    rm_hash_ent_t neg_invalid_hash_entry_2;

    rm_hash_ent_t neg_invalid_hash_entry_3;

    uint32_t serial_resolve_result;
} rm_hash_lp_serial_ctrl_word_t;

typedef struct rm_hash_lp_hierarchical_ctrl_word_s {

    uint32_t hierarchical_tbp_0;

    uint32_t hierarchical_tbp_0_polarity;

    uint32_t hierarchical_macro_resolved_0;

    uint32_t hierarchical_tbp_1;

    uint32_t hierarchical_tbp_1_polarity;

    uint32_t hierarchical_macro_resolved_1;

    uint32_t hierarchical_tbp_2;

    uint32_t hierarchical_tbp_2_polarity;

    uint32_t hierarchical_macro_resolved_2;

    uint32_t priority_encode_diff_0_1;

    uint32_t priority_encode_diff_0_2;

    uint32_t priority_encode_diff_0_3;

    uint32_t priority_encode_diff_1_2;

    uint32_t priority_encode_diff_1_3;

    uint32_t priority_encode_diff_2_3;

    rm_hash_ent_t xor_0_1;

    rm_hash_ent_t xor_0_2;

    rm_hash_ent_t xor_0_3;

    rm_hash_ent_t xor_1_2;

    rm_hash_ent_t xor_1_3;

    rm_hash_ent_t xor_2_3;

    rm_hash_ent_t tmp_xor_0_1;

    rm_hash_ent_t tmp_xor_2_3;

    rm_hash_ent_t tmp_and_0_1;

    rm_hash_ent_t tmp_or_0_1;

    rm_hash_ent_t and_0_1_2;

    rm_hash_ent_t hierarchical_resolve_bit_positions;

    rm_hash_ent_t hierarchical_resolve_bit_positions_final;

    rm_hash_ent_t invalid_hash_entry_0;

    rm_hash_ent_t invalid_hash_entry_1;

    rm_hash_ent_t invalid_hash_entry_2;

    rm_hash_ent_t invalid_hash_entry_3;

    rm_hash_ent_t xor_0_1_2_3;

    rm_hash_ent_t nand_0_1_2;

    rm_hash_ent_t or_0_1_2;

    rm_hash_ent_t nxor_0_1_2_3;

    rm_hash_ent_t nand_or_0_1_2;

    uint32_t hash_entry_0_bit_at_hierarchical_tbp_0;

    uint32_t hash_entry_1_bit_at_hierarchical_tbp_0;

    uint32_t hash_entry_2_bit_at_hierarchical_tbp_0;

    uint32_t hash_entry_3_bit_at_hierarchical_tbp_0;

    uint32_t hash_entry_bit_slice_at_hierarchical_tbp_0;

    uint32_t hierarchical_tbp_0_polarity_should_be_zero;
} rm_hash_lp_hierarchical_ctrl_word_t;

/*******************************************************************************
 * Private variables
 */
static rm_hash_lp_ctrl_info_t        ctrl_info[BCMDRD_CONFIG_MAX_UNITS];

static rm_hash_lp_serial_ctrl_word_t serial_ctrl_word[BCMDRD_CONFIG_MAX_UNITS];

static rm_hash_lp_hierarchical_ctrl_word_t  hierarchical_ctrl_word[BCMDRD_CONFIG_MAX_UNITS];
/*******************************************************************************
 * Private Functions
 */
static void
rm_hash_serial_tbps_resolve(uint16_t widest_key_width,
                            rm_hash_lp_ctrl_info_t *lp_ctrl_info,
                            rm_hash_lp_serial_ctrl_word_t *lp_ctrl_word)
{
    int idx = 0;

    /* Clear lp control word */
    sal_memset(lp_ctrl_word, 0x00, sizeof(*lp_ctrl_word));

    /* Set invalid hash entry */
    if (!SHR_BITGET(lp_ctrl_info->valid_logical_entry[0].e_words, 0)) {
        SHR_BITSET(lp_ctrl_word->invalid_hash_entry_0.e_words, 1);
    } else {
        SHR_BITCOPY_RANGE(lp_ctrl_word->invalid_hash_entry_0.e_words, 0, lp_ctrl_info->valid_logical_entry[0].e_words, 0, 319);
    }

    if (!SHR_BITGET(lp_ctrl_info->valid_logical_entry[1].e_words, 0)) {
        SHR_BITSET(lp_ctrl_word->invalid_hash_entry_1.e_words, 2);
    } else {
        SHR_BITCOPY_RANGE(lp_ctrl_word->invalid_hash_entry_1.e_words, 0, lp_ctrl_info->valid_logical_entry[1].e_words, 0, 319);
    }

    if (!SHR_BITGET(lp_ctrl_info->valid_logical_entry[2].e_words, 0)) {
        SHR_BITSET(lp_ctrl_word->invalid_hash_entry_2.e_words, 3);
    } else {
        SHR_BITCOPY_RANGE(lp_ctrl_word->invalid_hash_entry_2.e_words, 0, lp_ctrl_info->valid_logical_entry[2].e_words, 0, 319);
    }

    if (!SHR_BITGET(lp_ctrl_info->valid_logical_entry[3].e_words, 0)) {
        SHR_BITSET(lp_ctrl_word->invalid_hash_entry_3.e_words, 4);
    } else {
        SHR_BITCOPY_RANGE(lp_ctrl_word->invalid_hash_entry_3.e_words, 0, lp_ctrl_info->valid_logical_entry[3].e_words, 0, 319);
    }

    /* Set XOR entry */
    SHR_BITXOR_RANGE(lp_ctrl_word->invalid_hash_entry_0.e_words,
                     lp_ctrl_word->invalid_hash_entry_1.e_words,
                     0,
                     319,
                     lp_ctrl_word->xor_0_1.e_words);

    SHR_BITXOR_RANGE(lp_ctrl_word->invalid_hash_entry_0.e_words,
                     lp_ctrl_word->invalid_hash_entry_2.e_words,
                     0,
                     319,
                     lp_ctrl_word->xor_0_2.e_words);

    SHR_BITXOR_RANGE(lp_ctrl_word->invalid_hash_entry_0.e_words,
                     lp_ctrl_word->invalid_hash_entry_3.e_words,
                     0,
                     319,
                     lp_ctrl_word->xor_0_3.e_words);

    SHR_BITXOR_RANGE(lp_ctrl_word->invalid_hash_entry_1.e_words,
                     lp_ctrl_word->invalid_hash_entry_2.e_words,
                     0,
                     319,
                     lp_ctrl_word->xor_1_2.e_words);

    SHR_BITXOR_RANGE(lp_ctrl_word->invalid_hash_entry_1.e_words,
                     lp_ctrl_word->invalid_hash_entry_3.e_words,
                     0,
                     319,
                     lp_ctrl_word->xor_1_3.e_words);

    SHR_BITXOR_RANGE(lp_ctrl_word->invalid_hash_entry_2.e_words,
                     lp_ctrl_word->invalid_hash_entry_3.e_words,
                     0,
                     319,
                     lp_ctrl_word->xor_2_3.e_words);

    /* Set negative entry */
    SHR_BITNEGATE_RANGE(lp_ctrl_word->invalid_hash_entry_0.e_words,
                        0,
                        319,
                        lp_ctrl_word->neg_invalid_hash_entry_0.e_words);

    SHR_BITNEGATE_RANGE(lp_ctrl_word->invalid_hash_entry_1.e_words,
                        0,
                        319,
                        lp_ctrl_word->neg_invalid_hash_entry_1.e_words);

    SHR_BITNEGATE_RANGE(lp_ctrl_word->invalid_hash_entry_2.e_words,
                        0,
                        319,
                        lp_ctrl_word->neg_invalid_hash_entry_2.e_words);

    SHR_BITNEGATE_RANGE(lp_ctrl_word->invalid_hash_entry_3.e_words,
                        0,
                        319,
                        lp_ctrl_word->neg_invalid_hash_entry_3.e_words);

    /* Set diff entry */
    SHR_BITAND_RANGE(lp_ctrl_word->invalid_hash_entry_0.e_words,
                     lp_ctrl_word->neg_invalid_hash_entry_1.e_words,
                     0,
                     319,
                     lp_ctrl_word->diff_0_1.e_words);

    SHR_BITAND_RANGE(lp_ctrl_word->invalid_hash_entry_0.e_words,
                     lp_ctrl_word->neg_invalid_hash_entry_2.e_words,
                     0,
                     319,
                     lp_ctrl_word->diff_0_2.e_words);

    SHR_BITAND_RANGE(lp_ctrl_word->invalid_hash_entry_0.e_words,
                     lp_ctrl_word->neg_invalid_hash_entry_3.e_words,
                     0,
                     319,
                     lp_ctrl_word->diff_0_3.e_words);

    SHR_BITAND_RANGE(lp_ctrl_word->invalid_hash_entry_1.e_words,
                     lp_ctrl_word->neg_invalid_hash_entry_2.e_words,
                     0,
                     319,
                     lp_ctrl_word->diff_1_2.e_words);

    SHR_BITAND_RANGE(lp_ctrl_word->invalid_hash_entry_1.e_words,
                     lp_ctrl_word->neg_invalid_hash_entry_3.e_words,
                     0,
                     319,
                     lp_ctrl_word->diff_1_3.e_words);

    SHR_BITAND_RANGE(lp_ctrl_word->invalid_hash_entry_2.e_words,
                     lp_ctrl_word->neg_invalid_hash_entry_3.e_words,
                     0,
                     319,
                     lp_ctrl_word->diff_2_3.e_words);

    /* Set serial_resolve_bit_positions */
    SHR_BITAND_RANGE(lp_ctrl_word->xor_0_1.e_words,
                     lp_ctrl_word->xor_0_2.e_words,
                     0,
                     319,
                     lp_ctrl_word->serial_resolve_bit_positions_0.e_words);

    SHR_BITAND_RANGE(lp_ctrl_word->xor_0_1.e_words,
                     lp_ctrl_word->xor_1_2.e_words,
                     0,
                     319,
                     lp_ctrl_word->serial_resolve_bit_positions_1.e_words);

    SHR_BITAND_RANGE(lp_ctrl_word->xor_0_2.e_words,
                     lp_ctrl_word->xor_1_2.e_words,
                     0,
                     319,
                     lp_ctrl_word->serial_resolve_bit_positions_2.e_words);

    SHR_BITAND_RANGE(lp_ctrl_word->xor_0_3.e_words,
                     lp_ctrl_word->xor_1_3.e_words,
                     0,
                     319,
                     lp_ctrl_word->serial_resolve_bit_positions_3.e_words);

    /* Set serial_resolve_x_valid */
    if (!SHR_BITNULL_RANGE(lp_ctrl_word->serial_resolve_bit_positions_0.e_words, 0, 319)) {
        lp_ctrl_word->serial_resolve_0_valid = 1;
    }
    if (!SHR_BITNULL_RANGE(lp_ctrl_word->serial_resolve_bit_positions_1.e_words, 0, 319)) {
        lp_ctrl_word->serial_resolve_1_valid = 1;
    }
    if (!SHR_BITNULL_RANGE(lp_ctrl_word->serial_resolve_bit_positions_2.e_words, 0, 319)) {
        lp_ctrl_word->serial_resolve_2_valid = 1;
    }
    if (!SHR_BITNULL_RANGE(lp_ctrl_word->serial_resolve_bit_positions_3.e_words, 0, 319)) {
        lp_ctrl_word->serial_resolve_3_valid = 1;
    }

    lp_ctrl_word->serial_resolve_tbp_0 = 0;
    lp_ctrl_word->serial_resolve_tbp_1 = 0;
    lp_ctrl_word->serial_resolve_tbp_2 = 0;
    lp_ctrl_word->serial_resolve_tbp_3 = 0;

    lp_ctrl_word->hash_entry_0_bit_at_serial_resolve_tbp_0 = 0;
    lp_ctrl_word->hash_entry_1_bit_at_serial_resolve_tbp_1 = 0;
    lp_ctrl_word->hash_entry_2_bit_at_serial_resolve_tbp_2 = 0;
    lp_ctrl_word->hash_entry_3_bit_at_serial_resolve_tbp_3 = 0;

    /* Set serial_resolve_tbp_0 */
    for (idx = widest_key_width -1; idx >= 0; idx--) {
        if (SHR_BITGET(lp_ctrl_word->serial_resolve_bit_positions_0.e_words, idx)) {
            lp_ctrl_word->serial_resolve_tbp_0 = idx;
        }
    }

    /* Set serial_resolve_tbp_1 */
    for (idx = widest_key_width -1; idx >= 0; idx--) {
        if (SHR_BITGET(lp_ctrl_word->serial_resolve_bit_positions_1.e_words, idx)) {
            lp_ctrl_word->serial_resolve_tbp_1 = idx;
        }
    }

    /* Set serial_resolve_tbp_2 */
    for (idx = widest_key_width -1; idx >= 0; idx--) {
        if (SHR_BITGET(lp_ctrl_word->serial_resolve_bit_positions_2.e_words, idx)) {
            lp_ctrl_word->serial_resolve_tbp_2 = idx;
        }
    }

    /* Set serial_resolve_tbp_3 */
    for (idx = widest_key_width -1; idx >= 0; idx--) {
        if (SHR_BITGET(lp_ctrl_word->serial_resolve_bit_positions_3.e_words, idx)) {
            lp_ctrl_word->serial_resolve_tbp_3 = idx;
        }
    }

    if (SHR_BITGET(lp_ctrl_word->invalid_hash_entry_0.e_words, lp_ctrl_word->serial_resolve_tbp_0)) {
        lp_ctrl_word->hash_entry_0_bit_at_serial_resolve_tbp_0 = 1;
    } else {
        lp_ctrl_word->hash_entry_0_bit_at_serial_resolve_tbp_0 = 0;
    }
    if (SHR_BITGET(lp_ctrl_word->invalid_hash_entry_1.e_words, lp_ctrl_word->serial_resolve_tbp_1)) {
        lp_ctrl_word->hash_entry_1_bit_at_serial_resolve_tbp_1 = 1;
    } else {
        lp_ctrl_word->hash_entry_1_bit_at_serial_resolve_tbp_1 = 0;
    }
    if (SHR_BITGET(lp_ctrl_word->invalid_hash_entry_2.e_words, lp_ctrl_word->serial_resolve_tbp_2)) {
        lp_ctrl_word->hash_entry_2_bit_at_serial_resolve_tbp_2 = 1;
    } else {
        lp_ctrl_word->hash_entry_2_bit_at_serial_resolve_tbp_2 = 0;
    }
    if (SHR_BITGET(lp_ctrl_word->invalid_hash_entry_3.e_words, lp_ctrl_word->serial_resolve_tbp_3)) {
        lp_ctrl_word->hash_entry_3_bit_at_serial_resolve_tbp_3 = 1;
    } else {
        lp_ctrl_word->hash_entry_3_bit_at_serial_resolve_tbp_3 = 0;
    }

    lp_ctrl_word->serial_resolve_result = (lp_ctrl_word->serial_resolve_0_valid << 3) |
                                          (lp_ctrl_word->serial_resolve_1_valid << 2) |
                                          (lp_ctrl_word->serial_resolve_2_valid << 1) |
                                          (lp_ctrl_word->serial_resolve_3_valid << 0);

    switch(lp_ctrl_word->serial_resolve_result) {
    case 0x07:
        lp_ctrl_word->serial_tbp_0 = lp_ctrl_word->serial_resolve_tbp_1;
        lp_ctrl_word->serial_tbp_0_polarity = lp_ctrl_word->hash_entry_1_bit_at_serial_resolve_tbp_1;
        lp_ctrl_word->serial_macro_resolved_0 = MACRO1;

        lp_ctrl_word->serial_tbp_1 = lp_ctrl_word->serial_resolve_tbp_2;
        lp_ctrl_word->serial_tbp_1_polarity = lp_ctrl_word->hash_entry_2_bit_at_serial_resolve_tbp_2;
        lp_ctrl_word->serial_macro_resolved_1 = MACRO2;

        lp_ctrl_word->serial_tbp_2 = lp_ctrl_word->serial_resolve_tbp_3;
        lp_ctrl_word->serial_tbp_2_polarity = lp_ctrl_word->hash_entry_3_bit_at_serial_resolve_tbp_3;
        lp_ctrl_word->serial_macro_resolved_2 = MACRO3;
        break;
    case 0x0B:
        lp_ctrl_word->serial_tbp_0 = lp_ctrl_word->serial_resolve_tbp_0;
        lp_ctrl_word->serial_tbp_0_polarity = lp_ctrl_word->hash_entry_0_bit_at_serial_resolve_tbp_0;
        lp_ctrl_word->serial_macro_resolved_0 = MACRO0;

        lp_ctrl_word->serial_tbp_1 = lp_ctrl_word->serial_resolve_tbp_2;
        lp_ctrl_word->serial_tbp_1_polarity = lp_ctrl_word->hash_entry_2_bit_at_serial_resolve_tbp_2;
        lp_ctrl_word->serial_macro_resolved_1 = MACRO2;

        lp_ctrl_word->serial_tbp_2 = lp_ctrl_word->serial_resolve_tbp_3;
        lp_ctrl_word->serial_tbp_2_polarity = lp_ctrl_word->hash_entry_3_bit_at_serial_resolve_tbp_3;
        lp_ctrl_word->serial_macro_resolved_2 = MACRO3;
        break;
    case 0x0D:
        lp_ctrl_word->serial_tbp_0 = lp_ctrl_word->serial_resolve_tbp_0;
        lp_ctrl_word->serial_tbp_0_polarity = lp_ctrl_word->hash_entry_0_bit_at_serial_resolve_tbp_0;
        lp_ctrl_word->serial_macro_resolved_0 = MACRO0;

        lp_ctrl_word->serial_tbp_1 = lp_ctrl_word->serial_resolve_tbp_1;
        lp_ctrl_word->serial_tbp_1_polarity = lp_ctrl_word->hash_entry_1_bit_at_serial_resolve_tbp_1;
        lp_ctrl_word->serial_macro_resolved_1 = MACRO1;

        lp_ctrl_word->serial_tbp_2 = lp_ctrl_word->serial_resolve_tbp_3;
        lp_ctrl_word->serial_tbp_2_polarity = lp_ctrl_word->hash_entry_3_bit_at_serial_resolve_tbp_3;
        lp_ctrl_word->serial_macro_resolved_2 = MACRO3;
        break;
    case 0x0E:
        lp_ctrl_word->serial_tbp_0 = lp_ctrl_word->serial_resolve_tbp_0;
        lp_ctrl_word->serial_tbp_0_polarity = lp_ctrl_word->hash_entry_0_bit_at_serial_resolve_tbp_0;
        lp_ctrl_word->serial_macro_resolved_0 = MACRO0;

        lp_ctrl_word->serial_tbp_1 = lp_ctrl_word->serial_resolve_tbp_1;
        lp_ctrl_word->serial_tbp_1_polarity = lp_ctrl_word->hash_entry_1_bit_at_serial_resolve_tbp_1;
        lp_ctrl_word->serial_macro_resolved_1 = MACRO1;

        lp_ctrl_word->serial_tbp_2 = lp_ctrl_word->serial_resolve_tbp_2;
        lp_ctrl_word->serial_tbp_2_polarity = lp_ctrl_word->hash_entry_2_bit_at_serial_resolve_tbp_2;
        lp_ctrl_word->serial_macro_resolved_2 = MACRO2;
        break;
    case 0x0F:
        lp_ctrl_word->serial_tbp_0 = lp_ctrl_word->serial_resolve_tbp_0;
        lp_ctrl_word->serial_tbp_0_polarity = lp_ctrl_word->hash_entry_0_bit_at_serial_resolve_tbp_0;
        lp_ctrl_word->serial_macro_resolved_0 = MACRO0;

        lp_ctrl_word->serial_tbp_1 = lp_ctrl_word->serial_resolve_tbp_1;
        lp_ctrl_word->serial_tbp_1_polarity = lp_ctrl_word->hash_entry_1_bit_at_serial_resolve_tbp_1;
        lp_ctrl_word->serial_macro_resolved_1 = MACRO1;

        lp_ctrl_word->serial_tbp_2 = lp_ctrl_word->serial_resolve_tbp_2;
        lp_ctrl_word->serial_tbp_2_polarity = lp_ctrl_word->hash_entry_2_bit_at_serial_resolve_tbp_2;
        lp_ctrl_word->serial_macro_resolved_2 = MACRO2;
        break;
    default:
        break;
    }
}

static void
rm_hash_hierarchical_tbps_resolve(uint16_t widest_key_width,
                                  rm_hash_lp_ctrl_info_t *lp_ctrl_info,
                                  rm_hash_lp_hierarchical_ctrl_word_t *lp_ctrl_word,
                                  uint32_t *is_hierarchical_resolve)
{
    int idx = 0;

    /* Clear lp control word */
    sal_memset(lp_ctrl_word, 0x00, sizeof(*lp_ctrl_word));

    *is_hierarchical_resolve = 0;

    /* Set invalid hash entry */
    if (!SHR_BITGET(lp_ctrl_info->valid_logical_entry[0].e_words, 0)) {
        SHR_BITSET(lp_ctrl_word->invalid_hash_entry_0.e_words, 1);
    } else {
        SHR_BITCOPY_RANGE(lp_ctrl_word->invalid_hash_entry_0.e_words, 0, lp_ctrl_info->valid_logical_entry[0].e_words, 0, 319);
    }

    if (!SHR_BITGET(lp_ctrl_info->valid_logical_entry[1].e_words, 0)) {
        SHR_BITSET(lp_ctrl_word->invalid_hash_entry_1.e_words, 2);
    } else {
        SHR_BITCOPY_RANGE(lp_ctrl_word->invalid_hash_entry_1.e_words, 0, lp_ctrl_info->valid_logical_entry[1].e_words, 0, 319);
    }

    if (!SHR_BITGET(lp_ctrl_info->valid_logical_entry[2].e_words, 0)) {
        SHR_BITSET(lp_ctrl_word->invalid_hash_entry_2.e_words, 3);
    } else {
        SHR_BITCOPY_RANGE(lp_ctrl_word->invalid_hash_entry_2.e_words, 0, lp_ctrl_info->valid_logical_entry[2].e_words, 0, 319);
    }

    if (!SHR_BITGET(lp_ctrl_info->valid_logical_entry[3].e_words, 0)) {
        SHR_BITSET(lp_ctrl_word->invalid_hash_entry_3.e_words, 4);
    } else {
        SHR_BITCOPY_RANGE(lp_ctrl_word->invalid_hash_entry_3.e_words, 0, lp_ctrl_info->valid_logical_entry[3].e_words, 0, 319);
    }

    /* Set XOR_0_1_2_3 */
    SHR_BITXOR_RANGE(lp_ctrl_word->invalid_hash_entry_0.e_words,
                     lp_ctrl_word->invalid_hash_entry_1.e_words,
                     0,
                     319,
                     lp_ctrl_word->tmp_xor_0_1.e_words);

    SHR_BITXOR_RANGE(lp_ctrl_word->invalid_hash_entry_2.e_words,
                     lp_ctrl_word->invalid_hash_entry_3.e_words,
                     0,
                     319,
                     lp_ctrl_word->tmp_xor_2_3.e_words);

    SHR_BITXOR_RANGE(lp_ctrl_word->tmp_xor_0_1.e_words,
                     lp_ctrl_word->tmp_xor_2_3.e_words,
                     0,
                     319,
                     lp_ctrl_word->xor_0_1_2_3.e_words);

    /* Set nand_0_1_2 */
    SHR_BITAND_RANGE(lp_ctrl_word->invalid_hash_entry_0.e_words,
                     lp_ctrl_word->invalid_hash_entry_1.e_words,
                     0,
                     319,
                     lp_ctrl_word->tmp_and_0_1.e_words);

    SHR_BITAND_RANGE(lp_ctrl_word->tmp_and_0_1.e_words,
                     lp_ctrl_word->invalid_hash_entry_2.e_words,
                     0,
                     319,
                     lp_ctrl_word->and_0_1_2.e_words);

    SHR_BITNEGATE_RANGE(lp_ctrl_word->and_0_1_2.e_words,
                        0,
                        319,
                        lp_ctrl_word->nand_0_1_2.e_words);

    /* Set or_0_1_2 */
    SHR_BITOR_RANGE(lp_ctrl_word->invalid_hash_entry_0.e_words,
                    lp_ctrl_word->invalid_hash_entry_1.e_words,
                    0,
                    319,
                    lp_ctrl_word->tmp_or_0_1.e_words);

    SHR_BITOR_RANGE(lp_ctrl_word->tmp_or_0_1.e_words,
                    lp_ctrl_word->invalid_hash_entry_2.e_words,
                    0,
                    319,
                    lp_ctrl_word->or_0_1_2.e_words);

    /* Set XOR entry */
    SHR_BITXOR_RANGE(lp_ctrl_word->invalid_hash_entry_0.e_words,
                     lp_ctrl_word->invalid_hash_entry_1.e_words,
                     0,
                     319,
                     lp_ctrl_word->xor_0_1.e_words);

    SHR_BITXOR_RANGE(lp_ctrl_word->invalid_hash_entry_0.e_words,
                     lp_ctrl_word->invalid_hash_entry_2.e_words,
                     0,
                     319,
                     lp_ctrl_word->xor_0_2.e_words);

    SHR_BITXOR_RANGE(lp_ctrl_word->invalid_hash_entry_0.e_words,
                     lp_ctrl_word->invalid_hash_entry_3.e_words,
                     0,
                     319,
                     lp_ctrl_word->xor_0_3.e_words);

    SHR_BITXOR_RANGE(lp_ctrl_word->invalid_hash_entry_1.e_words,
                     lp_ctrl_word->invalid_hash_entry_2.e_words,
                     0,
                     319,
                     lp_ctrl_word->xor_1_2.e_words);

    SHR_BITXOR_RANGE(lp_ctrl_word->invalid_hash_entry_1.e_words,
                     lp_ctrl_word->invalid_hash_entry_3.e_words,
                     0,
                     319,
                     lp_ctrl_word->xor_1_3.e_words);

    SHR_BITXOR_RANGE(lp_ctrl_word->invalid_hash_entry_2.e_words,
                     lp_ctrl_word->invalid_hash_entry_3.e_words,
                     0,
                     319,
                     lp_ctrl_word->xor_2_3.e_words);

    SHR_BITNEGATE_RANGE(lp_ctrl_word->xor_0_1_2_3.e_words,
                        0,
                        319,
                        lp_ctrl_word->nxor_0_1_2_3.e_words);

    SHR_BITAND_RANGE(lp_ctrl_word->nand_0_1_2.e_words,
                     lp_ctrl_word->or_0_1_2.e_words,
                     0,
                     319,
                     lp_ctrl_word->nand_or_0_1_2.e_words);

    SHR_BITAND_RANGE(lp_ctrl_word->nxor_0_1_2_3.e_words,
                     lp_ctrl_word->nand_or_0_1_2.e_words,
                     0,
                     319,
                     lp_ctrl_word->hierarchical_resolve_bit_positions.e_words);

    /*Below is to eliminate the cases "0000" or "1111"*/
    SHR_BITCOPY_RANGE(lp_ctrl_word->hierarchical_resolve_bit_positions_final.e_words,
                      0,
                      lp_ctrl_word->hierarchical_resolve_bit_positions.e_words,
                      0,
                      319);

    if (!SHR_BITNULL_RANGE(lp_ctrl_word->hierarchical_resolve_bit_positions_final.e_words, 0, 319)) {
        *is_hierarchical_resolve = 1;
    }

    lp_ctrl_word->priority_encode_diff_0_1  = 0;
    lp_ctrl_word->priority_encode_diff_0_2  = 0;
    lp_ctrl_word->priority_encode_diff_0_3  = 0;
    lp_ctrl_word->priority_encode_diff_1_2  = 0;
    lp_ctrl_word->priority_encode_diff_1_3  = 0;
    lp_ctrl_word->priority_encode_diff_2_3  = 0;
    lp_ctrl_word->hierarchical_tbp_0 = 0;

    for (idx = widest_key_width -1; idx >= 0; idx--) {
        if (SHR_BITGET(lp_ctrl_word->xor_0_1.e_words, idx)) {
            lp_ctrl_word->priority_encode_diff_0_1 = idx;
        }
    }
    for (idx = widest_key_width -1; idx >= 0; idx--) {
        if (SHR_BITGET(lp_ctrl_word->xor_0_2.e_words, idx)) {
            lp_ctrl_word->priority_encode_diff_0_2 = idx;
        }
    }
    for (idx = widest_key_width -1; idx >= 0; idx--) {
        if (SHR_BITGET(lp_ctrl_word->xor_0_3.e_words, idx)) {
            lp_ctrl_word->priority_encode_diff_0_3 = idx;
        }
    }
    for (idx = widest_key_width -1; idx >= 0; idx--) {
        if (SHR_BITGET(lp_ctrl_word->xor_1_2.e_words, idx)) {
            lp_ctrl_word->priority_encode_diff_1_2 = idx;
        }
    }
    for (idx = widest_key_width -1; idx >= 0; idx--) {
        if (SHR_BITGET(lp_ctrl_word->xor_1_3.e_words, idx)) {
            lp_ctrl_word->priority_encode_diff_1_3 = idx;
        }
    }
    for (idx = widest_key_width -1; idx >= 0; idx--) {
        if (SHR_BITGET(lp_ctrl_word->xor_2_3.e_words, idx)) {
            lp_ctrl_word->priority_encode_diff_2_3 = idx;
        }
    }
    for (idx = widest_key_width - 1; idx >= 0; idx--) {
        if (SHR_BITGET(lp_ctrl_word->hierarchical_resolve_bit_positions_final.e_words, idx)) {
            lp_ctrl_word->hierarchical_tbp_0 = idx;
        }
    }

    /* Get hash_entry_0_bit_at_hierarchical_tbp_0 */
    if (SHR_BITGET(lp_ctrl_word->invalid_hash_entry_0.e_words, lp_ctrl_word->hierarchical_tbp_0)) {
        lp_ctrl_word->hash_entry_0_bit_at_hierarchical_tbp_0 = 1;
    } else {
        lp_ctrl_word->hash_entry_0_bit_at_hierarchical_tbp_0 = 0;
    }

    if (SHR_BITGET(lp_ctrl_word->invalid_hash_entry_1.e_words, lp_ctrl_word->hierarchical_tbp_0)) {
        lp_ctrl_word->hash_entry_1_bit_at_hierarchical_tbp_0 = 1;
    } else {
        lp_ctrl_word->hash_entry_1_bit_at_hierarchical_tbp_0 = 0;
    }

    if (SHR_BITGET(lp_ctrl_word->invalid_hash_entry_2.e_words, lp_ctrl_word->hierarchical_tbp_0)) {
        lp_ctrl_word->hash_entry_2_bit_at_hierarchical_tbp_0 = 1;
    } else {
        lp_ctrl_word->hash_entry_2_bit_at_hierarchical_tbp_0 = 0;
    }

    if (SHR_BITGET(lp_ctrl_word->invalid_hash_entry_3.e_words, lp_ctrl_word->hierarchical_tbp_0)) {
        lp_ctrl_word->hash_entry_3_bit_at_hierarchical_tbp_0 = 1;
    } else {
        lp_ctrl_word->hash_entry_3_bit_at_hierarchical_tbp_0 = 0;
    }

    lp_ctrl_word->hash_entry_bit_slice_at_hierarchical_tbp_0 = (lp_ctrl_word->hash_entry_0_bit_at_hierarchical_tbp_0 << 3) |
                                                               (lp_ctrl_word->hash_entry_1_bit_at_hierarchical_tbp_0 << 2) |
                                                               (lp_ctrl_word->hash_entry_2_bit_at_hierarchical_tbp_0 << 1) |
                                                               (lp_ctrl_word->hash_entry_3_bit_at_hierarchical_tbp_0 << 0);

    lp_ctrl_word->hierarchical_tbp_0_polarity = 1;

    lp_ctrl_word->hierarchical_tbp_0_polarity_should_be_zero = ((0x01 & ((lp_ctrl_word->hash_entry_bit_slice_at_hierarchical_tbp_0 == 0x06) | (lp_ctrl_word->hash_entry_bit_slice_at_hierarchical_tbp_0 == 0x0A) | (lp_ctrl_word->hash_entry_bit_slice_at_hierarchical_tbp_0 == 0x0C))) |
                                                                (0x01 & ((lp_ctrl_word->hash_entry_bit_slice_at_hierarchical_tbp_0 == 0x03) | (lp_ctrl_word->hash_entry_bit_slice_at_hierarchical_tbp_0 == 0x05) | (lp_ctrl_word->hash_entry_bit_slice_at_hierarchical_tbp_0 == 0x06))) |
                                                                (0x01 & ((lp_ctrl_word->hash_entry_bit_slice_at_hierarchical_tbp_0 == 0x03) | (lp_ctrl_word->hash_entry_bit_slice_at_hierarchical_tbp_0 == 0x09) | (lp_ctrl_word->hash_entry_bit_slice_at_hierarchical_tbp_0 == 0x0A))) |
                                                                (0x01 & ((lp_ctrl_word->hash_entry_bit_slice_at_hierarchical_tbp_0 == 0x05) | (lp_ctrl_word->hash_entry_bit_slice_at_hierarchical_tbp_0 == 0x09) | (lp_ctrl_word->hash_entry_bit_slice_at_hierarchical_tbp_0 == 0x0C))));

    switch (lp_ctrl_word->hash_entry_bit_slice_at_hierarchical_tbp_0) {
    case 0x03:
        lp_ctrl_word->hierarchical_tbp_2 = lp_ctrl_word->priority_encode_diff_2_3;
        if (SHR_BITGET(lp_ctrl_word->invalid_hash_entry_2.e_words, lp_ctrl_word->hierarchical_tbp_2)) {
            lp_ctrl_word->hierarchical_tbp_2_polarity = 1;
        } else {
            lp_ctrl_word->hierarchical_tbp_2_polarity = 0;
        }
        lp_ctrl_word->hierarchical_macro_resolved_2 = MACRO3;
        lp_ctrl_word->hierarchical_tbp_1 = lp_ctrl_word->priority_encode_diff_0_1;

        if (SHR_BITGET(lp_ctrl_word->invalid_hash_entry_0.e_words, lp_ctrl_word->hierarchical_tbp_1)) {
            lp_ctrl_word->hierarchical_tbp_1_polarity = 1;
        } else {
            lp_ctrl_word->hierarchical_tbp_1_polarity = 0;
        }
        lp_ctrl_word->hierarchical_macro_resolved_0 = MACRO1;
        lp_ctrl_word->hierarchical_macro_resolved_1 = MACRO0;
        break;
    case 0x05:
        lp_ctrl_word->hierarchical_tbp_2 = lp_ctrl_word->priority_encode_diff_1_3;
        if (SHR_BITGET(lp_ctrl_word->invalid_hash_entry_1.e_words, lp_ctrl_word->hierarchical_tbp_2)) {
            lp_ctrl_word->hierarchical_tbp_2_polarity = 1;
        } else {
            lp_ctrl_word->hierarchical_tbp_2_polarity = 0;
        }
        lp_ctrl_word->hierarchical_macro_resolved_2 = MACRO3;
        lp_ctrl_word->hierarchical_tbp_1 = lp_ctrl_word->priority_encode_diff_0_2;
        if (SHR_BITGET(lp_ctrl_word->invalid_hash_entry_0.e_words, lp_ctrl_word->hierarchical_tbp_1)) {
            lp_ctrl_word->hierarchical_tbp_1_polarity = 1;
        } else {
            lp_ctrl_word->hierarchical_tbp_1_polarity = 0;
        }
        lp_ctrl_word->hierarchical_macro_resolved_0 = MACRO2;
        lp_ctrl_word->hierarchical_macro_resolved_1 = MACRO0;
        break;
    case 0x06:
        lp_ctrl_word->hierarchical_tbp_2 = lp_ctrl_word->priority_encode_diff_1_2;
        if (SHR_BITGET(lp_ctrl_word->invalid_hash_entry_1.e_words, lp_ctrl_word->hierarchical_tbp_2)) {
            lp_ctrl_word->hierarchical_tbp_2_polarity = 1;
        } else {
            lp_ctrl_word->hierarchical_tbp_2_polarity = 0;
        }
        lp_ctrl_word->hierarchical_macro_resolved_2 = MACRO2;
        lp_ctrl_word->hierarchical_tbp_1 = lp_ctrl_word->priority_encode_diff_0_3;
        if (SHR_BITGET(lp_ctrl_word->invalid_hash_entry_0.e_words, lp_ctrl_word->hierarchical_tbp_1)) {
            lp_ctrl_word->hierarchical_tbp_1_polarity = 1;
        } else {
            lp_ctrl_word->hierarchical_tbp_1_polarity = 0;
        }
        lp_ctrl_word->hierarchical_macro_resolved_0 = MACRO3;
        lp_ctrl_word->hierarchical_macro_resolved_1 = MACRO0;
        break;
    case 0x09:
        lp_ctrl_word->hierarchical_tbp_2 = lp_ctrl_word->priority_encode_diff_0_3;
        if (SHR_BITGET(lp_ctrl_word->invalid_hash_entry_0.e_words, lp_ctrl_word->hierarchical_tbp_2)) {
            lp_ctrl_word->hierarchical_tbp_2_polarity = 1;
        } else {
            lp_ctrl_word->hierarchical_tbp_2_polarity = 0;
        }
        lp_ctrl_word->hierarchical_macro_resolved_2 = MACRO3;
        lp_ctrl_word->hierarchical_tbp_1 = lp_ctrl_word->priority_encode_diff_1_2;
        if (SHR_BITGET(lp_ctrl_word->invalid_hash_entry_1.e_words, lp_ctrl_word->hierarchical_tbp_1)) {
            lp_ctrl_word->hierarchical_tbp_1_polarity = 1;
        } else {
            lp_ctrl_word->hierarchical_tbp_1_polarity = 0;
        }
        lp_ctrl_word->hierarchical_macro_resolved_0 = MACRO2;
        lp_ctrl_word->hierarchical_macro_resolved_1 = MACRO1;
        break;
    case 0x0A:
        lp_ctrl_word->hierarchical_tbp_2 = lp_ctrl_word->priority_encode_diff_0_2;
        if (SHR_BITGET(lp_ctrl_word->invalid_hash_entry_0.e_words, lp_ctrl_word->hierarchical_tbp_2)) {
            lp_ctrl_word->hierarchical_tbp_2_polarity = 1;
        } else {
            lp_ctrl_word->hierarchical_tbp_2_polarity = 0;
        }
        lp_ctrl_word->hierarchical_macro_resolved_2 = MACRO2;
        lp_ctrl_word->hierarchical_tbp_1 = lp_ctrl_word->priority_encode_diff_1_3;
        if (SHR_BITGET(lp_ctrl_word->invalid_hash_entry_1.e_words, lp_ctrl_word->hierarchical_tbp_1)) {
            lp_ctrl_word->hierarchical_tbp_1_polarity = 1;
        } else {
            lp_ctrl_word->hierarchical_tbp_1_polarity = 0;
        }
        lp_ctrl_word->hierarchical_macro_resolved_0 = MACRO3;
        lp_ctrl_word->hierarchical_macro_resolved_1 = MACRO1;
        break;
    case 0x0C:
        lp_ctrl_word->hierarchical_tbp_2 = lp_ctrl_word->priority_encode_diff_0_1;
        if (SHR_BITGET(lp_ctrl_word->invalid_hash_entry_0.e_words, lp_ctrl_word->hierarchical_tbp_2)) {
            lp_ctrl_word->hierarchical_tbp_2_polarity = 1;
        } else {
            lp_ctrl_word->hierarchical_tbp_2_polarity = 0;
        }
        lp_ctrl_word->hierarchical_macro_resolved_2 = MACRO1;
        lp_ctrl_word->hierarchical_tbp_1 = lp_ctrl_word->priority_encode_diff_2_3;
        if (SHR_BITGET(lp_ctrl_word->invalid_hash_entry_2.e_words, lp_ctrl_word->hierarchical_tbp_1)) {
            lp_ctrl_word->hierarchical_tbp_1_polarity = 1;
        } else {
            lp_ctrl_word->hierarchical_tbp_1_polarity = 0;
        }
        lp_ctrl_word->hierarchical_macro_resolved_0 = MACRO3;
        lp_ctrl_word->hierarchical_macro_resolved_1 = MACRO2;
        break;
    default:
        break;
    }
}

static int
rm_hash_base_ent_addr_get(int unit,
                          rm_hash_bm_t bucket_mode,
                          uint32_t *bkt_base_adr)
{
    SHR_FUNC_ENTER(unit);

    switch(bucket_mode) {
    case RM_HASH_BM_0:
        *bkt_base_adr = *bkt_base_adr & (~0x03);
        break;
    case RM_HASH_BM_1:
        *bkt_base_adr = *bkt_base_adr & (~0x07);
        break;
    case RM_HASH_BM_2:
        *bkt_base_adr = *bkt_base_adr & (~0x0F);
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
rm_hash_lp_addr_get(int unit,
                    rm_hash_bm_t bucket_mode,
                    uint32_t remote_bank_depth,
                    uint32_t base_ent_adr,
                    uint32_t *lp_adr)
{
    SHR_FUNC_ENTER(unit);

    switch (bucket_mode) {
    case RM_HASH_BM_0:
        *lp_adr = base_ent_adr/4;
        break;
    case RM_HASH_BM_1:
        *lp_adr = remote_bank_depth + base_ent_adr / 8;
        break;
    case RM_HASH_BM_2:
        *lp_adr = remote_bank_depth + remote_bank_depth / 2 + base_ent_adr / 16;
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_hash_lp_ent_update(int unit,
                             bcmbd_pt_dyn_info_t *pt_dyn_info,
                             bcmdrd_sid_t single_sid,
                             bcmdrd_sid_t key_attr_sid,
                             bcmdrd_sid_t lp_sid,
                             rm_hash_bm_t bucket_mode,
                             uint8_t base_width,
                             uint32_t remote_bank_depth,
                             uint32_t mem_offset)
{
    rm_hash_lp_ctrl_info_t *lp_ctrl_info = NULL;
    rm_hash_lp_serial_ctrl_word_t *pserial_ctrl_word = NULL;
    rm_hash_lp_hierarchical_ctrl_word_t *phierarchical_ctrl_word = NULL;
    uint32_t base_entry_adr;
    uint32_t start_key = 0;
    uint32_t index = 0, key_length, is_hierarchical_resolve = 0;
    uint32_t base_valid = 0, key_type = 0;
    bcmbd_pt_dyn_info_t dyn_info = *pt_dyn_info;
    bool cache_valid;
    bcmltd_sid_t ltid;
    uint16_t dft_id;
    uint32_t rd_rsp_flags;
    uint32_t cmdproc_rsp_flags = 0;
    int i, j;

    SHR_FUNC_ENTER(unit);

    /* Get base entry address based on entry size. */
    base_entry_adr = mem_offset;
    if (base_width == 2) {
        base_entry_adr = mem_offset << 1;
    } else if (base_width == 4) {
        base_entry_adr = mem_offset << 2;
    }

    /* Align the base entry address based on bucket mode. */
    SHR_IF_ERR_EXIT
        (rm_hash_base_ent_addr_get(unit,
                                   bucket_mode,
                                   &base_entry_adr));
    lp_ctrl_info = &ctrl_info[unit];
    /* Clear base entry and logical entry */
    sal_memset(lp_ctrl_info, 0, sizeof(*lp_ctrl_info));

    /* Extracts 16 Base entries */
    for (i = 0; i < (4 << (bucket_mode - 1)); i++) {
        dyn_info.index = base_entry_adr + i;
        SHR_IF_ERR_EXIT
            (bcmptm_cmdproc_read(unit,
                                 BCMPTM_REQ_FLAGS_NO_FLAGS,
                                 0,
                                 single_sid,
                                 &dyn_info,
                                 NULL,
                                 BCMPTM_MAX_PT_ENTRY_WORDS,
                                 lp_ctrl_info->base_entry[i].e_words,
                                 &cache_valid,
                                 &ltid,
                                 &dft_id,
                                 &rd_rsp_flags));
    }

    /* Extracts the Logical Entries */
    for (i = 0; i < 4; i++) {
        switch (bucket_mode) {
        case RM_HASH_BM_0:
            base_valid = 0;
            bcmdrd_field_get(lp_ctrl_info->base_entry[i].e_words, 0, 2, &base_valid);
            if (base_valid == 0x01) {
                sal_memset(lp_ctrl_info->base_entry_field.e_words, 0, sizeof(rm_hash_ent_t));
                bcmdrd_field_get(lp_ctrl_info->base_entry[i].e_words, 3, 119, lp_ctrl_info->base_entry_field.e_words);
                bcmdrd_field_set(lp_ctrl_info->logical_entry[i].e_words, 0, 116, lp_ctrl_info->base_entry_field.e_words);
                start_key = 1;
                lp_ctrl_info->logical_entry_valid[i] = 1;
            }
            break;
        case RM_HASH_BM_1:
            base_valid = 0;
            bcmdrd_field_get(lp_ctrl_info->base_entry[2 * i].e_words, 0, 2, &base_valid);
            if (base_valid == 0x03) {
                sal_memset(lp_ctrl_info->base_entry_field.e_words, 0, sizeof(rm_hash_ent_t));
                bcmdrd_field_get(lp_ctrl_info->base_entry[2 * i].e_words, 3, 119, lp_ctrl_info->base_entry_field.e_words);
                bcmdrd_field_set(lp_ctrl_info->logical_entry[i].e_words, 0, 116, lp_ctrl_info->base_entry_field.e_words);
                start_key = 1;
                lp_ctrl_info->logical_entry_valid[i] = 1;
            }
            break;
        case RM_HASH_BM_2:
            base_valid = 0;
            bcmdrd_field_get(lp_ctrl_info->base_entry[4 * i].e_words, 0, 2, &base_valid);
            if (base_valid == 0x05) {
                sal_memset(lp_ctrl_info->base_entry_field.e_words, 0, sizeof(rm_hash_ent_t));
                bcmdrd_field_get(lp_ctrl_info->base_entry[4 * i].e_words, 3, 119, lp_ctrl_info->base_entry_field.e_words);
                bcmdrd_field_set(lp_ctrl_info->logical_entry[i].e_words, 0, 116, lp_ctrl_info->base_entry_field.e_words);
                start_key = 1;
                lp_ctrl_info->logical_entry_valid[i] = 1;
            }
            break;
        default:
            break;
        }
        /* For floating / Spanning entries. Concatenating the ENTRIES (excluding base valid) */
        for (j = 1; j < 4; j++) {
            if (bucket_mode == RM_HASH_BM_0) {
                base_valid = 0;
                bcmdrd_field_get(lp_ctrl_info->base_entry[i + j].e_words, 0, 2, &base_valid);
                if ((base_valid == 0x02) && (start_key == 1)) {
                    /* Copy KEY without BASEVALID */
                    sal_memset(lp_ctrl_info->base_entry_field.e_words, 0, sizeof(rm_hash_ent_t));
                    bcmdrd_field_get(lp_ctrl_info->base_entry[i + j].e_words, 3, 119, lp_ctrl_info->base_entry_field.e_words);
                    bcmdrd_field_set(lp_ctrl_info->logical_entry[i].e_words, j * 117, j * 117 + 116, lp_ctrl_info->base_entry_field.e_words);
                } else {
                    start_key = 0;
                    break;
                }
            } else if (bucket_mode == RM_HASH_BM_1) {
                base_valid = 0;
                bcmdrd_field_get(lp_ctrl_info->base_entry[2 * i + j].e_words, 0, 2, &base_valid);
                if ((base_valid == 0x03) && (j == 1) && (start_key == 1)) {
                    /*
                     * Illegal entry: Multiple Tab Entires with same bucket mode
                     * in Logical entry.
                     */
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                } else if ((base_valid == 0x03) && (j == 1) && (start_key != 1)) {
                    sal_memset(lp_ctrl_info->base_entry_field.e_words, 0, sizeof(rm_hash_ent_t));
                    bcmdrd_field_get(lp_ctrl_info->base_entry[2 * i + j].e_words, 3, 119, lp_ctrl_info->base_entry_field.e_words);
                    bcmdrd_field_set(lp_ctrl_info->logical_entry[i].e_words, 0, 116, lp_ctrl_info->base_entry_field.e_words);
                    start_key = 1;
                    lp_ctrl_info->logical_entry_valid[i] = 1;
                } else if ((base_valid == 0x04) && (start_key == 1)) {
                    sal_memset(lp_ctrl_info->base_entry_field.e_words, 0, sizeof(rm_hash_ent_t));
                    bcmdrd_field_get(lp_ctrl_info->base_entry[2 * i + j].e_words, 3, 119, lp_ctrl_info->base_entry_field.e_words);
                    bcmdrd_field_set(lp_ctrl_info->logical_entry[i].e_words, j * 117, j * 117 + 116, lp_ctrl_info->base_entry_field.e_words);
                } else if (start_key == 1) {
                    start_key = 0;
                    break;
                }
            } else if (bucket_mode == RM_HASH_BM_2) {
                base_valid = 0;
                bcmdrd_field_get(lp_ctrl_info->base_entry[4 * i + j].e_words, 0, 2, &base_valid);
                if ((base_valid == 0x05) && ((j == 1) || (j == 2) || (j == 3)) && (start_key == 1)) {
                    /*
                     * Illegal entry: Multiple Tab Entires with same bucket mode
                     * in Logical entry.
                     */
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                } else if ((base_valid == 0x05) && ((j == 1) || (j == 2) || (j == 3)) && (start_key != 1)) {
                    sal_memset(lp_ctrl_info->base_entry_field.e_words, 0, sizeof(rm_hash_ent_t));
                    bcmdrd_field_get(lp_ctrl_info->base_entry[4 * i + j].e_words, 3, 119, lp_ctrl_info->base_entry_field.e_words);
                    bcmdrd_field_set(lp_ctrl_info->logical_entry[i].e_words, 0, 116, lp_ctrl_info->base_entry_field.e_words);
                    start_key = 1;
                    lp_ctrl_info->logical_entry_valid[i] = 1;
                } else if ((base_valid == 0x06) && (start_key == 1)) {
                    sal_memset(lp_ctrl_info->base_entry_field.e_words, 0, sizeof(rm_hash_ent_t));
                    bcmdrd_field_get(lp_ctrl_info->base_entry[4 * i + j].e_words, 3, 119, lp_ctrl_info->base_entry_field.e_words);
                    bcmdrd_field_set(lp_ctrl_info->logical_entry[i].e_words, j * 117, j * 117 + 116, lp_ctrl_info->base_entry_field.e_words);
                } else if (start_key == 1) {
                    start_key = 0;
                    break;
                }
            } else {
                /* Illegal bucket mode */
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            if (j == 3) {
                start_key = 0;
            }
        }
    }

    /* Read key attribute table. */
    for (i = 0; i < 4; i++) {
        bcmdrd_field_get(lp_ctrl_info->logical_entry[i].e_words, 0, 3, &key_type);
        dyn_info.index = key_type;

        sal_memset(lp_ctrl_info->key_attr.e_words, 0, sizeof(rm_hash_ent_t));

        SHR_IF_ERR_EXIT
            (bcmptm_cmdproc_read(unit,
                                 BCMPTM_REQ_FLAGS_NO_FLAGS,
                                 0,
                                 key_attr_sid,
                                 &dyn_info,
                                 NULL,
                                 BCMPTM_MAX_PT_ENTRY_WORDS,
                                 lp_ctrl_info->key_attr.e_words,
                                 &cache_valid,
                                 &ltid,
                                 &dft_id,
                                 &rd_rsp_flags));

        if (lp_ctrl_info->logical_entry_valid[i] == 1) {
            uint32_t key_base_width = 0;
            uint32_t key_width = 0;

            bcmdrd_field_get(lp_ctrl_info->key_attr.e_words, 2, 3, &key_base_width);
            bcmdrd_field_get(lp_ctrl_info->key_attr.e_words, 4, 8, &key_width);

            key_length = key_base_width * (IFTA30_E2T_00_B0_SINGLE_WIDTH - 3) + (key_width + 1) * 4;

            bcmdrd_field_set(lp_ctrl_info->logical_entry[i].e_words, key_length, 1023, lp_ctrl_info->null_entry.e_words);
            bcmdrd_field_set(lp_ctrl_info->valid_logical_entry[i].e_words, 0, 0, &lp_ctrl_info->logical_entry_valid[i]);
            bcmdrd_field_set(lp_ctrl_info->valid_logical_entry[i].e_words, 1, key_length, lp_ctrl_info->logical_entry[i].e_words);
        }
    }

    /* Try serial tbps resolve. */
    pserial_ctrl_word = &serial_ctrl_word[unit];
    rm_hash_serial_tbps_resolve(IFTA30_E2T_00_B0_WIDEST_KEY,
                                lp_ctrl_info,
                                pserial_ctrl_word);

    /* Try hierarchical tbps resolve. */
    phierarchical_ctrl_word = &hierarchical_ctrl_word[unit];
    rm_hash_hierarchical_tbps_resolve(IFTA30_E2T_00_B0_WIDEST_KEY,
                                      lp_ctrl_info,
                                      phierarchical_ctrl_word,
                                      &is_hierarchical_resolve);

    if (is_hierarchical_resolve != 0) {
        /* CONTROL_WORD_MACRO_RESOLVED_0 */
        uint32_t tmp = 0;
        bcmdrd_field_set(lp_ctrl_info->entry_lp.e_words, 4, 5, &phierarchical_ctrl_word->hierarchical_macro_resolved_0);
        bcmdrd_field_set(lp_ctrl_info->entry_lp.e_words, 2, 3, &phierarchical_ctrl_word->hierarchical_macro_resolved_1);
        bcmdrd_field_set(lp_ctrl_info->entry_lp.e_words, 0, 1, &phierarchical_ctrl_word->hierarchical_macro_resolved_2);

        bcmdrd_field_set(lp_ctrl_info->entry_lp.e_words, 6, 14,  &phierarchical_ctrl_word->hierarchical_tbp_0);
        bcmdrd_field_set(lp_ctrl_info->entry_lp.e_words, 16, 24, &phierarchical_ctrl_word->hierarchical_tbp_1);
        bcmdrd_field_set(lp_ctrl_info->entry_lp.e_words, 26, 34, &phierarchical_ctrl_word->hierarchical_tbp_2);

        bcmdrd_field_set(lp_ctrl_info->entry_lp.e_words, 15, 15, &phierarchical_ctrl_word->hierarchical_tbp_0_polarity);
        bcmdrd_field_set(lp_ctrl_info->entry_lp.e_words, 25, 25, &phierarchical_ctrl_word->hierarchical_tbp_1_polarity);
        bcmdrd_field_set(lp_ctrl_info->entry_lp.e_words, 35, 35, &phierarchical_ctrl_word->hierarchical_tbp_2_polarity);
        bcmdrd_field_set(lp_ctrl_info->entry_lp.e_words, 36, 36, &tmp);
    } else {
        uint32_t tmp = 1;
        bcmdrd_field_set(lp_ctrl_info->entry_lp.e_words, 4, 5, &pserial_ctrl_word->serial_macro_resolved_0);
        bcmdrd_field_set(lp_ctrl_info->entry_lp.e_words, 2, 3, &pserial_ctrl_word->serial_macro_resolved_1);
        bcmdrd_field_set(lp_ctrl_info->entry_lp.e_words, 0, 1, &pserial_ctrl_word->serial_macro_resolved_2);

        bcmdrd_field_set(lp_ctrl_info->entry_lp.e_words, 6, 14,  &pserial_ctrl_word->serial_tbp_0);
        bcmdrd_field_set(lp_ctrl_info->entry_lp.e_words, 16, 24, &pserial_ctrl_word->serial_tbp_1);
        bcmdrd_field_set(lp_ctrl_info->entry_lp.e_words, 26, 34, &pserial_ctrl_word->serial_tbp_2);

        bcmdrd_field_set(lp_ctrl_info->entry_lp.e_words, 15, 15, &pserial_ctrl_word->serial_tbp_0_polarity);
        bcmdrd_field_set(lp_ctrl_info->entry_lp.e_words, 25, 25, &pserial_ctrl_word->serial_tbp_1_polarity);
        bcmdrd_field_set(lp_ctrl_info->entry_lp.e_words, 35, 35, &pserial_ctrl_word->serial_tbp_2_polarity);
        bcmdrd_field_set(lp_ctrl_info->entry_lp.e_words, 36, 36, &tmp);
    }
    SHR_IF_ERR_EXIT
        (rm_hash_lp_addr_get(unit,
                             bucket_mode,
                             remote_bank_depth,
                             base_entry_adr,
                             &index));
    dyn_info.index = index;
    SHR_IF_ERR_EXIT
        (bcmptm_cmdproc_write(unit,
                              0,
                              0,
                              lp_sid,
                              &dyn_info,
                              NULL,
                              lp_ctrl_info->entry_lp.e_words,
                              TRUE,
                              TRUE,
                              TRUE,
                              BCMPTM_RM_OP_NORMAL, NULL,
                              0, /* Using 0 for LP table */
                              0, /* Using 0 for LP table */
                              &cmdproc_rsp_flags));

exit:
    SHR_FUNC_EXIT();
}
