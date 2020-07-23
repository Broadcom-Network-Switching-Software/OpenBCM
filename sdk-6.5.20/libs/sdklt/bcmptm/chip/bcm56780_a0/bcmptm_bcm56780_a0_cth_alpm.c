/*! \file bcmptm_bcm56780_a0_cth_alpm.c
 *
 * Chip specific functions for CTH ALPM
 *
 * This file contains the chip specific functions for CTH ALPM
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
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <sal/sal_sleep.h>

#include <bcmdrd/bcmdrd_pt.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/chip/bcm56780_a0_defs.h>
#include <bcmbd/chip/bcm56780_a0_acc.h>
#include <bcmbd/chip/bcm56780_a0_dev.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_client.h>

#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_rm_alpm_internal.h>
#include <bcmptm/bcmptm_cth_alpm_internal.h>
#include <bcmptm/bcmptm_cth_uft_be_internal.h>
#include "bcmptm_bcm56780_a0_cth_alpm.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_CTHALPM

#define CONTROL_FIELD_SET(_f, _v)   \
    LPM_IP_CONTROLm_##_f##_SET(ce, (_v))

#define M_CONTROL_FIELD_SET(_f, _v)   \
    M_LPM_IP_CONTROLm_##_f##_SET(ce, (_v))

#define IPDB0 0
#define IPDB1 1
#define IPDB2 2
#define IPDB3 3
#define IPDBX 4
/* the following for LT_TYPE resolve DB only */
#define IPDB01 5
#define IPDB23 6

#define K0  0
#define K1  1
#define K2  2
#define K3  3
#define K4  4
#define K5  5
#define K6  6
#define K7  7

#define DISABLED    0

#define KEY_INPUT_LPM_DST_Q         0   /* IPv6-128 DST */
#define KEY_INPUT_LPM_DST_D         1   /* IPv6-64 DST */
#define KEY_INPUT_LPM_DST_S         2   /* IPv6-32 DST */
#define KEY_INPUT_LPM_SRC_Q         3   /* IPv6-128 SRC */
#define KEY_INPUT_LPM_SRC_D         4   /* IPv6-64 SRC */
#define KEY_INPUT_LPM_SRC_S         5   /* IPv6-32 SRC */
#define KEY_INPUT_FP_COMP_DST       6
#define KEY_INPUT_FP_COMP_SRC       7

#define FOUR_RESULTS_MODE           2
#define THREE_RESULTS_MODE          1
#define TWO_RESULTS_MODE            0

#define L1_PKSET_DSH        0   /* {DOUBLE, SINGLE, HALF} */
#define L1_PKSET_SSH        1   /* {SINGLE, SINGLE, HALF} */
#define L1_PKSET_SHH        2   /* {SINGLE, HALF, HALF} */
#define L1_PKSET_HHH        3   /* {HALF, HALF, HALF} */

#define LN_PKSET_16_49      0   /* {16b, 49b} */
#define LN_PKSET_72_104     1   /* {72b, 104b} */


#define L1_DB0 0
#define L1_DB1 1
#define L1_DB2 2
#define L1_DB3 3
#define L1_DBS 4

#define L1_TCAMS_0      0
#define L1_TCAMS_8      8
#define L1_TCAMS_12     12
#define L1_TCAMS_16     16


#define bcm56780_a0_l3_combined_block_dbs           bcm56780_a0_l3_block_dbs
#define bcm56780_a0_l3_parallel_block_dbs           bcm56780_a0_l3_block_dbs
#define bcm56780_a0_l3_combined_rpf_block_dbs       bcm56780_a0_l3_block_dbs
#define bcm56780_a0_l3_parallel_rpf_block_dbs       bcm56780_a0_l3_block_dbs
#define bcm56780_a0_l3_fp_combined_block_dbs        bcm56780_a0_l3_block_dbs
#define bcm56780_a0_l3_fp_no_lpm_block_dbs          bcm56780_a0_l3_block_dbs
#define bcm56780_a0_l3_fp_parallel_block_dbs        bcm56780_a0_l3_block_dbs
#define bcm56780_a0_l3_fp_combined_rpf_block_dbs    bcm56780_a0_l3_block_dbs

#define BCM56780_ALPM_L1_BLOCKS_MAX 8
#define BCM56780_ALPM_L1_BLOCKS(m)  (m ? 8 : 4)
#define BCM56780_ALPM_L1_KEY_MUXES  4
#define BCM56780_ALPM_L3_BLOCKS     4
#define BCM56780_ALPM_BLOCKS        4

#define LEVEL1  0
#define LEVEL3  2
/*******************************************************************************
 * Typedefs
 */
typedef struct bcm56780_lpm_ip_control_s {
    int lane0_key_type_l1_packing_set[4];
    int lane1_key_type_l1_packing_set[4];
    int lane2_key_type_l1_packing_set[4];
    int lane3_key_type_l1_packing_set[4];

    int lane0_key_type_ln_packing_set[4];
    int lane1_key_type_ln_packing_set[4];
    int lane2_key_type_ln_packing_set[4];
    int lane3_key_type_ln_packing_set[4];


    int level1_database_sel_block[8];
    int level2_database_sel_block[4];
    int level3_database_sel_block[4];

    int level1_key_input_sel_block[8];
    int level2_key_input_sel_block[4];
    int level3_key_input_sel_block[4];

    int level2_bank_config_block[4];
    int level3_bank_config_block[4];

    int destination[4];
    int destination_mask[4];
    int destination_type_match[4];
    int destination_type_non_match[4];

    int db_mod[4];
    int data_sel[4];
    int result_mode;
} bcm56780_lpm_ip_control_t;

typedef struct bcm56780_lpm_lane_control_s {
    uint8_t comp_src_enable;
    uint8_t comp_dst_enable;
    uint8_t l3_src_enable;
    uint8_t l3_dst_enable;
} bcm56780_lpm_lane_control_t;

typedef enum
{
    ALPM_LT_TYPE_LPM_NONE        = 0,
    ALPM_LT_TYPE_LPM_DST         = 1,
    ALPM_LT_TYPE_LPM_SRC         = 2,
    ALPM_LT_TYPE_FP_COMP_DST     = 3,
    ALPM_LT_TYPE_FP_COMP_SRC     = 4,
    ALPM_LT_TYPE_COUNT     = 5
} bcm56780_lt_type_t;

typedef struct bcm56780_lt_name_2_lt_type_s {
    const char *lt_name;
    bcm56780_lt_type_t type;
} bcm56780_lt_name_2_lt_type_t;

/*******************************************************************************
 * Private variables
 */
static uint8_t
bcm56780_a0_pdd_type_l3uc_dst[ALPMS] = {
    EM_TILE_ALPM_PDD_TYPE_L3UC_DST,
    EM_TILE_ALPM_PDD_TYPE_L3UC_DST_MTOP
};
static uint8_t
bcm56780_a0_pdd_type_l3uc_src[ALPMS] = {
    EM_TILE_ALPM_PDD_TYPE_L3UC_SRC,
    EM_TILE_ALPM_PDD_TYPE_L3UC_SRC_MTOP
};
static uint8_t
bcm56780_a0_pdd_type_comp_dst[ALPMS] = {
    EM_TILE_ALPM_PDD_TYPE_COMP_DST,
    EM_TILE_ALPM_PDD_TYPE_COMP_DST_MTOP
};
static uint8_t
bcm56780_a0_pdd_type_comp_src[ALPMS] = {
    EM_TILE_ALPM_PDD_TYPE_COMP_SRC,
    EM_TILE_ALPM_PDD_TYPE_COMP_SRC_MTOP
};

static uint8_t
bcm56780_combined_blocks[2][4] = {          /* DB0 */
    {IPDBX, IPDBX, IPDBX, IPDBX},           /*  0  */
    {IPDB0, IPDB0, IPDB0, IPDB0}            /*  1  */
};
static uint8_t
bcm56780_parallel_blocks[4][4] = {          /* DB0 DB1 */
    {IPDBX, IPDBX, IPDBX, IPDBX},           /*  0   0  */
    {IPDB1, IPDB1, IPDB1, IPDB1},           /*  0   1  */
    {IPDB0, IPDB0, IPDB0, IPDB0},           /*  1   0  */
    {IPDB0, IPDB0, IPDB1, IPDB1}            /*  1   1  */
};
static uint8_t
bcm56780_combined_rpf_blocks[4][4] = {      /* DB0 DB2 */
    {IPDBX, IPDBX, IPDBX, IPDBX},           /*  0   0  */
    {IPDB2, IPDB2, IPDB2, IPDB2},           /*  0   1  */
    {IPDB0, IPDB0, IPDB0, IPDB0},           /*  1   0  */
    {IPDB0, IPDB0, IPDB2, IPDB2}            /*  1   1  */
};
static uint8_t
bcm56780_parallel_rpf_blocks[16][4] = {     /* DB0 DB1 DB2 DB3 */
    {IPDBX, IPDBX, IPDBX, IPDBX},           /*  0   0   0   0  */
    {IPDB3, IPDB3, IPDB3, IPDB3},           /*  0   0   0   1  */
    {IPDB2, IPDB2, IPDB2, IPDB2},           /*  0   0   1   0  */
    {IPDB2, IPDB2, IPDB3, IPDB3},           /*  0   0   1   1  */
    {IPDB1, IPDB1, IPDB1, IPDB1},           /*  0   1   0   0  */
    {IPDB1, IPDB1, IPDB3, IPDB3},           /*  0   1   0   1  */
    {IPDB1, IPDB1, IPDB2, IPDB2},           /*  0   1   1   0  */
    {IPDB1, IPDB1, IPDB2, IPDB3},           /*  0   1   1   1  */
    {IPDB0, IPDB0, IPDB0, IPDB0},           /*  1   0   0   0  */
    {IPDB0, IPDB0, IPDB0, IPDB3},           /*  1   0   0   1  */
    {IPDB0, IPDB0, IPDB2, IPDB2},           /*  1   0   1   0  */
    {IPDB0, IPDB0, IPDB2, IPDB3},           /*  1   0   1   1  */
    {IPDB0, IPDB1, IPDB0, IPDB1},           /*  1   1   0   0  */
    {IPDB0, IPDB1, IPDB0, IPDB3},           /*  1   1   0   1  */
    {IPDB0, IPDB1, IPDB2, IPDB0},           /*  1   1   1   0  */
    {IPDB0, IPDB1, IPDB2, IPDB3}            /*  1   1   1   1  */
};
static uint8_t
bcm56780_fp_no_lpm_blocks[4][4] = {/* DB2 DB3 */
    {IPDBX, IPDBX, IPDBX, IPDBX},           /*  0   0  */
    {IPDB3, IPDB3, IPDB3, IPDB3},           /*  0   1  */
    {IPDB2, IPDB2, IPDB2, IPDB2},           /*  1   0  */
    {IPDB2, IPDB2, IPDB3, IPDB3},           /*  1   1  */
};

static uint8_t
bcm56780_fp_combined_blocks[8][4] = {       /* DB0 DB2 DB3 */
    {IPDBX, IPDBX, IPDBX, IPDBX},           /*  0   0   0  */
    {IPDB3, IPDB3, IPDB3, IPDB3},           /*  0   0   1  */
    {IPDB2, IPDB2, IPDB2, IPDB2},           /*  0   1   0  */
    {IPDB2, IPDB2, IPDB3, IPDB3},           /*  0   1   1  */
    {IPDB0, IPDB0, IPDB0, IPDB0},           /*  1   0   0  */
    {IPDB0, IPDB0, IPDB0, IPDB3},           /*  1   0   1  */
    {IPDB0, IPDB2, IPDB0, IPDB0},           /*  1   1   0  */
    {IPDB0, IPDB2, IPDB0, IPDB3}            /*  1   1   1  */
};
static uint8_t
bcm56780_fp_parallel_blocks[16][4] = {      /* DB0 DB1 DB2 DB3 */
    {IPDBX, IPDBX, IPDBX, IPDBX},           /*  0   0   0   0  */
    {IPDB3, IPDB3, IPDB3, IPDB3},           /*  0   0   0   1  */
    {IPDB2, IPDB2, IPDB2, IPDB2},           /*  0   0   1   0  */
    {IPDB2, IPDB2, IPDB3, IPDB3},           /*  0   0   1   1  */
    {IPDB1, IPDB1, IPDB1, IPDB1},           /*  0   1   0   0  */
    {IPDB1, IPDB1, IPDB3, IPDB3},           /*  0   1   0   1  */
    {IPDB1, IPDB1, IPDB2, IPDB2},           /*  0   1   1   0  */
    {IPDB1, IPDB2, IPDB1, IPDB3},           /*  0   1   1   1  */
    {IPDB0, IPDB0, IPDB0, IPDB0},           /*  1   0   0   0  */
    {IPDB0, IPDB0, IPDB0, IPDB3},           /*  1   0   0   1  */
    {IPDB0, IPDB0, IPDB2, IPDB2},           /*  1   0   1   0  */
    {IPDB0, IPDB2, IPDB0, IPDB3},           /*  1   0   1   1  */
    {IPDB0, IPDB1, IPDB0, IPDB1},           /*  1   1   0   0  */
    {IPDB0, IPDB0, IPDB1, IPDB3},           /*  1   1   0   1  */
    {IPDB0, IPDB2, IPDB1, IPDB0},           /*  1   1   1   0  */
    {IPDB0, IPDB2, IPDB1, IPDB3}            /*  1   1   1   1  */
};
static uint8_t
bcm56780_fp_combined_rpf_blocks[16][4] = {  /* DB0 DB1 DB2 DB3 */
    {IPDBX, IPDBX, IPDBX, IPDBX},           /*  0   0   0   0  */
    {IPDB3, IPDB3, IPDB3, IPDB3},           /*  0   0   0   1  */
    {IPDB2, IPDB2, IPDB2, IPDB2},           /*  0   0   1   0  */
    {IPDB2, IPDB2, IPDB3, IPDB3},           /*  0   0   1   1  */
    {IPDB1, IPDB1, IPDB1, IPDB1},           /*  0   1   0   0  */
    {IPDB1, IPDB1, IPDB3, IPDB3},           /*  0   1   0   1  */
    {IPDB1, IPDB1, IPDB2, IPDB2},           /*  0   1   1   0  */
    {IPDB1, IPDB2, IPDB1, IPDB3},           /*  0   1   1   1  */
    {IPDB0, IPDB0, IPDB0, IPDB0},           /*  1   0   0   0  */
    {IPDB0, IPDB0, IPDB0, IPDB3},           /*  1   0   0   1  */
    {IPDB0, IPDB0, IPDB2, IPDB2},           /*  1   0   1   0  */
    {IPDB0, IPDB2, IPDB0, IPDB3},           /*  1   0   1   1  */
    {IPDB0, IPDB1, IPDB0, IPDB1},           /*  1   1   0   0  */
    {IPDB0, IPDB0, IPDB1, IPDB3},           /*  1   1   0   1  */
    {IPDB0, IPDB2, IPDB1, IPDB0},           /*  1   1   1   0  */
    {IPDB0, IPDB2, IPDB1, IPDB3}            /*  1   1   1   1  */
};

/*
 * row format:
 * bit 0 P - ALPM_MODE_PARALLEL.
 * bit 1 R - ALPM_MODE_RPF
 * bit 2 F - ALPM_MODE_FP_COMP
 * bit 3 N - ALPM_MODE_NO_LPM
 *
 * column format: bcm56780_lt_type_t
 * TM0 - ALPM_LT_TYPE_LPM_NONE
 * TM1 - ALPM_LT_TYPE_LPM_DST
 * TM2 - ALPM_LT_TYPE_LPM_SRC
 * TM3 - ALPM_LT_TYPE_FP_COMP_DST
 * TM4 - ALPM_LT_TYPE_FP_COMP_SRC
 */
static uint8_t
bcm56780_l3_db_layout[16][5] = {
  /* TM0    TM1     TM2     TM3    TM4*/      /* N  F  R  P */
    {IPDBX, IPDB0,  IPDBX,  IPDBX, IPDBX},    /* 0  0  0  0 Combined */
    {IPDBX, IPDB01, IPDBX,  IPDBX, IPDBX},    /* 0  0  0  1 Parallel */
    {IPDBX, IPDB0,  IPDB2,  IPDBX, IPDBX},    /* 0  0  1  0 RPF + Combined */
    {IPDBX, IPDB01, IPDB23, IPDBX, IPDBX},    /* 0  0  1  1 RPF + Parallel */
    {IPDBX, IPDB0,  IPDBX,  IPDB2, IPDB3},    /* 0  1  0  0 FP + Combined */
    {IPDBX, IPDB01, IPDBX,  IPDB2, IPDB3},    /* 0  1  0  1 FP + Parallel */
    {IPDBX, IPDB0,  IPDB1,  IPDB2, IPDB3},    /* 0  1  1  0 FP + RPF + Combined */
    {IPDBX, IPDBX,  IPDBX,  IPDBX, IPDBX},    /* 0  1  1  1 not support */
    {IPDBX, IPDBX,  IPDBX,  IPDBX, IPDBX},    /* 1  0  0  0 not support */
    {IPDBX, IPDBX,  IPDBX,  IPDBX, IPDBX},    /* 1  0  0  1 not support */
    {IPDBX, IPDBX,  IPDBX,  IPDBX, IPDBX},    /* 1  0  1  0 not support */
    {IPDBX, IPDBX,  IPDBX,  IPDBX, IPDBX},    /* 1  0  1  1 not support */
    {IPDBX, IPDBX,  IPDBX,  IPDB2, IPDB3},    /* 1  1  0  0 FP (no LPM) */
    {IPDBX, IPDBX,  IPDBX,  IPDBX, IPDBX},    /* 1  1  0  1 not support */
    {IPDBX, IPDBX,  IPDBX,  IPDBX, IPDBX},    /* 1  1  1  0 not support */
    {IPDBX, IPDBX,  IPDBX,  IPDBX, IPDBX},    /* 1  1  1  1 not support */
};

static bcm56780_lt_name_2_lt_type_t
bcm56780_lt_names_2_lt_type[BCM56780_ALPM_BLOCKS] = {
    {"L3_IPV4_UC_ROUTE",        ALPM_LT_TYPE_LPM_DST},
    {"L3_SRC_IPV4_UC_ROUTE",    ALPM_LT_TYPE_LPM_SRC},
    {"L3_IPV4_COMP_DST",        ALPM_LT_TYPE_FP_COMP_DST},
    {"L3_IPV4_COMP_SRC",        ALPM_LT_TYPE_FP_COMP_SRC}
};
static bcm56780_lt_name_2_lt_type_t
bcm56780_lt_names_2_lt_type_mtop[BCM56780_ALPM_BLOCKS] = {
    {"L3_IPV4_UC_ROUTE_MTOP",        ALPM_LT_TYPE_LPM_DST},
    {"L3_SRC_IPV4_UC_ROUTE_MTOP",    ALPM_LT_TYPE_LPM_SRC},
    {"L3_IPV4_COMP_DST_MTOP",        ALPM_LT_TYPE_FP_COMP_DST},
    {"L3_IPV4_COMP_SRC_MTOP",       ALPM_LT_TYPE_FP_COMP_SRC}
};

/*******************************************************************************
 * Private Functions
 */
static int
bcm56780_a0_alpm_db_levels(bcmptm_cth_alpm_control_t *ctrl, int db)
{
    int levels = 0;
    switch (db) {
        case IPDB0: levels = ctrl->db0_levels; break;
        case IPDB1: levels = ctrl->db1_levels; break;
        case IPDB2: levels = ctrl->db2_levels; break;
        case IPDB3: levels = ctrl->db3_levels; break;
        default: assert(0); break;
    }
    return levels;
}

static int
bcm56780_a0_cth_alpm_l1_parse_check(int u, int m, int num_l1_banks,
                                    int *l1_key_input, int *l1_db,
                                    int db0_levels, int db1_levels,
                                    int db2_levels, int db3_levels,
                                    int *reason,
                                    int *alpm_mode_out, int *alpm_dbs_out)
{
    int i, alpm_mode = ALPM_MODE_FLEX_DEFAULT, alpm_src_mode = 0, alpm_dbs;
    int db_lpmdst = -1;
    int db_lpmsrc = -1;
    int db_fpsrc = -1;
    int db_fpdst = -1;
    int db_lpmdst_parallel = -1;
    int db_lpmsrc_parallel = -1;
    int num_lpmdst = 0;
    int num_lpmsrc = 0;
    int num_fpsrc = 0;
    int num_fpdst = 0;
    int num_lpmdst_parallel = 0;
    int num_lpmsrc_parallel = 0;
    int start_block = 0, l1_blocks;

    if (m == ALPM_0) {
        l1_blocks = num_l1_banks >> 2;
    } else {
        l1_blocks = num_l1_banks >> 1;
    }
    /* User should adhere to the Level-1 rules considering Level-1 bank bitmaps */
    for (i = start_block; i < l1_blocks; i++) {

        /*
        * Tcam blocks DB value must be strictly ordered.
        */
        if ((i + 1) < l1_blocks && l1_db[i] > l1_db[i + 1]) {
            LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META_U(u,
                        "Expect strict orders of DBs value.\n"
                        )));
            return SHR_E_PARAM;
        }

        /*
        * Tcam blocks with same DB group together, and within a group,
        * key input must be strictly ordered.
        */
        if ((i + 1) < l1_blocks && l1_db[i] == l1_db[i + 1]) {
            if (l1_key_input[i] > l1_key_input[i + 1]) {
                LOG_INFO(BSL_LOG_MODULE,
                        (BSL_META_U(u,
                            "Expect strict orders of key inputs.\n"
                            )));
                return SHR_E_PARAM;
            }
        }
    }

    for (i = start_block; i < l1_blocks; i++) {
        if (l1_key_input[i] == KEY_INPUT_LPM_DST_Q ||
            l1_key_input[i] == KEY_INPUT_LPM_DST_D ||
            l1_key_input[i] == KEY_INPUT_LPM_DST_S) {
            /*
             * When same kind of key_inputs assigned with 2 DBS,
             * it is assumed the first DB is private, and second DB
             * is public.
             */
            if (db_lpmdst == -1) {
                db_lpmdst = l1_db[i];
                num_lpmdst++;
            } else if (db_lpmdst == l1_db[i]) {
                num_lpmdst++;
            } else {
                if (db_lpmdst_parallel == -1) {
                    db_lpmdst_parallel = l1_db[i];
                    num_lpmdst_parallel++;
                } else if (db_lpmdst_parallel == l1_db[i]) {
                    num_lpmdst_parallel++;
                } else {
                    LOG_INFO(BSL_LOG_MODULE,
                              (BSL_META_U(u,
                                  "Expect LPM DST key inputs assigned with <=2 DBs.\n"
                               )));
                    return SHR_E_PARAM;
                }
            }
        }
        if (l1_key_input[i] == KEY_INPUT_LPM_SRC_Q ||
            l1_key_input[i] == KEY_INPUT_LPM_SRC_D ||
            l1_key_input[i] == KEY_INPUT_LPM_SRC_S ) {
            if (db_lpmsrc == -1) {
                db_lpmsrc = l1_db[i];
                num_lpmsrc++;
            } else if (db_lpmsrc == l1_db[i]) {
                num_lpmsrc++;
            } else {
                if (db_lpmsrc_parallel == -1) {
                    db_lpmsrc_parallel = l1_db[i];
                    num_lpmsrc_parallel++;
                } else if (db_lpmsrc_parallel == l1_db[i]) {
                    num_lpmsrc_parallel++;
                } else {
                    LOG_INFO(BSL_LOG_MODULE,
                              (BSL_META_U(u,
                                  "Expect LPM SRC key inputs assigned with <=2 DBs.\n"
                               )));
                    return SHR_E_PARAM;
                }
            }
        }

        if (l1_key_input[i] == KEY_INPUT_FP_COMP_SRC) {
            if (db_fpsrc == -1) {
                db_fpsrc = l1_db[i];
                num_fpsrc++;
            } else if (db_fpsrc == l1_db[i]) {
                num_fpsrc++;
            } else {
                LOG_INFO(BSL_LOG_MODULE,
                            (BSL_META_U(u,
                                "Expect FP compression SRC key inputs assigned with <=1 DBs.\n"
                            )));
                return SHR_E_PARAM;
            }
        }
        if (l1_key_input[i] == KEY_INPUT_FP_COMP_DST) {
            if (db_fpdst == -1) {
                db_fpdst = l1_db[i];
                num_fpdst++;
            } else if (db_fpdst == l1_db[i]) {
                num_fpdst++;
            } else {
                LOG_INFO(BSL_LOG_MODULE,
                            (BSL_META_U(u,
                                "Expect FP compression DST key inputs assigned with <=1 DBs.\n"
                            )));
                return SHR_E_PARAM;
            }
        }
    }

    if (num_fpsrc != num_fpdst) {
        LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META_U(u,
                        "Expect equal l1 blocks for COMP_SRC and COMP_DST.\n"
                    )));
        return SHR_E_PARAM;
    }

    alpm_dbs = 0;
    if (num_lpmdst) {
        alpm_mode &= ~ALPM_MODE_NO_LPM;
        alpm_dbs++;
    }

    if (num_lpmdst_parallel) {
        alpm_dbs++;
        alpm_mode |= ALPM_MODE_PARALLEL;
    }

    if (num_lpmsrc) {
        alpm_mode |= ALPM_MODE_RPF;
        alpm_dbs++;
    }
    if (num_fpdst) {
        alpm_mode |= ALPM_MODE_FP_COMP;
        alpm_dbs++;
    }
    if (num_fpsrc) {
        alpm_mode |= ALPM_MODE_FP_COMP;
        alpm_dbs++;
    }
    if (num_lpmsrc_parallel) {
        alpm_dbs++;
    }

    /*
     * There is no point to support two different ALPM modes
     * between LPM DST and LPM SRC.
     * When DST is Combined mode, SRC must be Combined mode if exists.
     * When DST is Parallel mode, SRC must be Parallel mode if exists.
     */
    if (num_lpmsrc) {
        alpm_src_mode = alpm_mode;
        if (num_lpmsrc_parallel) {
            alpm_src_mode |= ALPM_MODE_PARALLEL;
        } else {
            alpm_src_mode &= ~ALPM_MODE_PARALLEL;
        }
        if (alpm_mode != alpm_src_mode) {
            LOG_INFO(BSL_LOG_MODULE,
                        (BSL_META_U(u,
                                "Expect same mode between LPM SRC and LPM DST.\n"
                        )));
            return SHR_E_PARAM;
        }
    }

    if (alpm_dbs > 4) {
        LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META_U(u,
                        "Expect no more than 4 DBs.\n"
                    )));
        return SHR_E_PARAM;
    }


    if (alpm_mode_out) {
        *alpm_mode_out = alpm_mode;
    }
    if (alpm_dbs_out) {
        *alpm_dbs_out = alpm_dbs;
    }

    if (num_lpmdst) {
        if (db_lpmdst != L1_DB0) {
            LOG_INFO(BSL_LOG_MODULE,
                        (BSL_META_U(u,
                            "Expect LPM DST assigned with DB0.\n"
                        )));
            return SHR_E_PARAM;
        }
    }

    /* Check FP compression configurations */
    if (alpm_mode & ALPM_MODE_NO_LPM) {
    } else if (alpm_mode & ALPM_MODE_FP_COMP) {
        if (db_fpdst != L1_DB2) {
            LOG_INFO(BSL_LOG_MODULE,
                        (BSL_META_U(u,
                            "Expect FP compression DST assigned with DB2.\n"
                        )));
            return SHR_E_PARAM;
        }
        if (db_fpsrc != L1_DB3) {
            LOG_INFO(BSL_LOG_MODULE,
                        (BSL_META_U(u,
                            "Expect FP compression DST assigned with DB3.\n"
                        )));
            return SHR_E_PARAM;
        }

        if (db2_levels == 0) {
            LOG_INFO(BSL_LOG_MODULE,
                        (BSL_META_U(u,
                            "Expect DB2_LEVELS > 0.\n"
                        )));
            return SHR_E_PARAM;
        }
        if (db3_levels == 0) {
            LOG_INFO(BSL_LOG_MODULE,
                        (BSL_META_U(u,
                            "Expect DB3_LEVELS > 0.\n"
                        )));
            return SHR_E_PARAM;
        }
        if (alpm_mode & ALPM_MODE_RPF) {
            /* When RPF & FP_COMP enabled, it must be Combined mode */
            assert((alpm_mode & ALPM_MODE_PARALLEL) == 0);
            if (db_lpmsrc != L1_DB1) {
                LOG_INFO(BSL_LOG_MODULE,
                            (BSL_META_U(u,
                                "Expect LPM SRC assigned with DB1.\n"
                            )));
                return SHR_E_PARAM;
            }
            if (db1_levels == 0) {
                LOG_INFO(BSL_LOG_MODULE,
                            (BSL_META_U(u,
                                "Expect DB1_LEVELS > 0.\n"
                            )));
                return SHR_E_PARAM;
            }
        } else if (alpm_mode & ALPM_MODE_PARALLEL) {
            /* When PARALLEL & FP_COMP enabled, it must be non-RPF mode */
            assert((alpm_mode & ALPM_MODE_RPF) == 0);
            if (db_lpmdst_parallel != L1_DB1) {
                LOG_INFO(BSL_LOG_MODULE,
                            (BSL_META_U(u,
                                "Expect LPM DST public assigned with DB1.\n"
                            )));
                return SHR_E_PARAM;
            }
            if (db1_levels == 0) {
                LOG_INFO(BSL_LOG_MODULE,
                            (BSL_META_U(u,
                                "Expect DB1_LEVELS > 0.\n"
                            )));
                return SHR_E_PARAM;
            }
        } else {
            if (db1_levels > 0) {
                LOG_INFO(BSL_LOG_MODULE,
                            (BSL_META_U(u,
                                "Expect DB1_LEVELS = 0.\n"
                            )));
                return SHR_E_PARAM;
            }
        }
    } else {
        if (alpm_mode & ALPM_MODE_RPF) {
            if (alpm_mode & ALPM_MODE_PARALLEL) {
                assert(db_lpmdst_parallel != -1 &&
                       db_lpmsrc != -1 &&
                       db_lpmsrc_parallel != -1);
                if (db_lpmdst_parallel != L1_DB1) {
                    LOG_INFO(BSL_LOG_MODULE,
                                (BSL_META_U(u,
                                    "Expect LPM DST public assigned with DB1.\n"
                                )));
                    return SHR_E_PARAM;
                }
                if (db_lpmsrc != L1_DB2) {
                    LOG_INFO(BSL_LOG_MODULE,
                                (BSL_META_U(u,
                                    "Expect LPM SRC private assigned with DB2.\n"
                                )));
                    return SHR_E_PARAM;
                }
                if (db_lpmsrc_parallel != L1_DB3) {
                    LOG_INFO(BSL_LOG_MODULE,
                                (BSL_META_U(u,
                                    "Expect LPM SRC public assigned with DB3.\n"
                                )));
                    return SHR_E_PARAM;
                }
                if (db1_levels == 0) {
                    LOG_INFO(BSL_LOG_MODULE,
                                (BSL_META_U(u,
                                    "Expect DB1_LEVELS > 0.\n"
                                )));
                    return SHR_E_PARAM;
                }
                if (db2_levels == 0) {
                    LOG_INFO(BSL_LOG_MODULE,
                                (BSL_META_U(u,
                                    "Expect DB2_LEVELS > 0.\n"
                                )));
                    return SHR_E_PARAM;
                }
                if (db3_levels == 0) {
                    LOG_INFO(BSL_LOG_MODULE,
                                (BSL_META_U(u,
                                    "Expect DB3_LEVELS > 0.\n"
                                )));
                    return SHR_E_PARAM;
                }
            } else {
                assert(db_lpmdst_parallel == -1 &&
                       db_lpmsrc != -1 &&
                       db_lpmsrc_parallel == -1);
                if (db_lpmsrc != L1_DB2) {
                    LOG_INFO(BSL_LOG_MODULE,
                                (BSL_META_U(u,
                                    "Expect LPM SRC assigned with DB2.\n"
                                )));
                    return SHR_E_PARAM;
                }
                if (db1_levels > 0) {
                    LOG_INFO(BSL_LOG_MODULE,
                                (BSL_META_U(u,
                                    "Expect DB1_LEVELS = 0.\n"
                                )));
                    return SHR_E_PARAM;
                }
                if (db2_levels == 0) {
                    LOG_INFO(BSL_LOG_MODULE,
                                (BSL_META_U(u,
                                    "Expect DB2_LEVELS > 0.\n"
                                )));
                    return SHR_E_PARAM;
                }
                if (db3_levels > 0) {
                    LOG_INFO(BSL_LOG_MODULE,
                                (BSL_META_U(u,
                                    "Expect DB3_LEVELS = 0.\n"
                                )));
                    return SHR_E_PARAM;
                }
            }
        } else {
            if (alpm_mode & ALPM_MODE_PARALLEL) {
                assert(db_lpmdst_parallel != -1 &&
                       db_lpmsrc == -1 &&
                       db_lpmsrc_parallel == -1);
                if (db_lpmdst_parallel != L1_DB1) {
                    LOG_INFO(BSL_LOG_MODULE,
                                (BSL_META_U(u,
                                    "Expect LPM DST public assigned with DB1.\n"
                                )));
                    return SHR_E_PARAM;
                }
                if (db1_levels == 0) {
                    LOG_INFO(BSL_LOG_MODULE,
                                (BSL_META_U(u,
                                    "Expect DB1_LEVELS > 0.\n"
                                )));
                    return SHR_E_PARAM;
                }
                if (db2_levels > 0) {
                    LOG_INFO(BSL_LOG_MODULE,
                                (BSL_META_U(u,
                                    "Expect DB2_LEVELS = 0.\n"
                                )));
                    return SHR_E_PARAM;
                }
                if (db3_levels > 0) {
                    LOG_INFO(BSL_LOG_MODULE,
                                (BSL_META_U(u,
                                    "Expect DB3_LEVELS = 0.\n"
                                )));
                    return SHR_E_PARAM;
                }
            } else {
                assert(db_lpmdst_parallel == -1 &&
                       db_lpmsrc == -1 &&
                       db_lpmsrc_parallel == -1);
                if (db1_levels > 0) {
                    LOG_INFO(BSL_LOG_MODULE,
                                (BSL_META_U(u,
                                    "Expect DB1_LEVELS = 0.\n"
                                )));
                    return SHR_E_PARAM;
                }
                if (db2_levels > 0) {
                    LOG_INFO(BSL_LOG_MODULE,
                                (BSL_META_U(u,
                                    "Expect DB2_LEVELS = 0.\n"
                                )));
                    return SHR_E_PARAM;
                }
                if (db3_levels > 0) {
                    LOG_INFO(BSL_LOG_MODULE,
                                (BSL_META_U(u,
                                    "Expect DB3_LEVELS = 0.\n"
                                )));
                    return SHR_E_PARAM;
                }
            }
        }
    }
    return SHR_E_NONE;
}


static uint32_t
bcm56780_a0_l2_bank_bitmap(uint8_t *blocks, uint8_t db)
{
    /*
     * LEVEL2_BANK_CONFIG_BLOCK_0 is physically connected to mini-UFT banks 0-1 only.
     * LEVEL2_BANK_CONFIG_BLOCK_1 is physically connected to mini-UFT banks 2 only.
     * LEVEL2_BANK_CONFIG_BLOCK_2 is physically connected to mini-UFT banks 3-4 only.
     * LEVEL2_BANK_CONFIG_BLOCK_3 is physically connected to mini-UFT banks 5 only.
     */
    return ((blocks[0] == db) ? 0x3 : 0) |
           ((blocks[1] == db) ? 0x1 : 0) << 2 |
           ((blocks[2] == db) ? 0x3 : 0) << 3 |
           ((blocks[3] == db) ? 0x1 : 0) << 5;
}
static uint32_t
bcm56780_a0_l3_bank_bitmap(uint8_t *blocks, uint8_t db)
{
    /*
     * LEVEL3_BANK_CONFIG_BLOCK_0 is physically connected to UFT banks 0-1 only.
     * LEVEL3_BANK_CONFIG_BLOCK_1 is physically connected to UFT banks 2-3 only.
     * LEVEL3_BANK_CONFIG_BLOCK_2 is physically connected to UFT banks 4-5 only.
     * LEVEL3_BANK_CONFIG_BLOCK_3 is physically connected to UFT banks 6-7 only.
     */
    return ((blocks[0] == db) ? 0x3 : 0) |
           ((blocks[1] == db) ? 0x3 : 0) << 2 |
           ((blocks[2] == db) ? 0x3 : 0) << 4 |
           ((blocks[3] == db) ? 0x3 : 0) << 6;
}

static void
bcm56780_a0_l2_bitmap_check_and_set(uint8_t *blocks, uint8_t db,
                                    bcmptm_cth_alpm_control_t *ctrl_in,
                                    bcmptm_cth_alpm_control_t *ctrl_out,
                                    int *rv)
{
    uint32_t bank_bitmap;
    int num_banks;
    int levels;

    if (SHR_FAILURE(*rv)) {
        return;
    }

    bank_bitmap = bcm56780_a0_l2_bank_bitmap(blocks, db);
    if (bank_bitmap && ((ctrl_in->tot.l2_bank_bitmap & bank_bitmap) == 0)) {
        levels = bcm56780_a0_alpm_db_levels(ctrl_in, db);
        if (levels >= 2) {
            LOG_INFO(BSL_LOG_MODULE,
                        (BSL_META(
                            "Insufficient level-2 banks.\n"
                        )));
            *rv = SHR_E_PARAM;
            return;
        }
    }
    if (ctrl_out) {
        bank_bitmap = ctrl_in->tot.l2_bank_bitmap & bank_bitmap;
        num_banks = shr_util_popcount(bank_bitmap);
        switch (db) {
            case IPDB0:
            ctrl_out->db0.l2_bank_bitmap = bank_bitmap;
            ctrl_out->db0.num_l2_banks = num_banks;
            break;

            case IPDB1:
            ctrl_out->db1.l2_bank_bitmap = bank_bitmap;
            ctrl_out->db1.num_l2_banks = num_banks;
            break;

            case IPDB2:
            ctrl_out->db2.l2_bank_bitmap = bank_bitmap;
            ctrl_out->db2.num_l2_banks = num_banks;
            break;

            case IPDB3:
            ctrl_out->db3.l2_bank_bitmap = bank_bitmap;
            ctrl_out->db3.num_l2_banks = num_banks;
            break;

            default:
            *rv = SHR_E_PARAM;
        }
    }

    return;
}

static void
bcm56780_a0_l3_bitmap_check_and_set(uint8_t *blocks, uint8_t db,
                                    bcmptm_cth_alpm_control_t *ctrl_in,
                                    bcmptm_cth_alpm_control_t *ctrl_out,
                                    int *rv)
{

    uint32_t bank_bitmap;
    int num_banks;
    int levels;

    if (SHR_FAILURE(*rv)) {
        return;
    }

    bank_bitmap = bcm56780_a0_l3_bank_bitmap(blocks, db);
    if (bank_bitmap && ((ctrl_in->tot.l3_bank_bitmap & bank_bitmap) == 0)) {
        levels = bcm56780_a0_alpm_db_levels(ctrl_in, db);
        if (levels >= 3) {
            LOG_INFO(BSL_LOG_MODULE,
                        (BSL_META(
                            "Insufficient level-3 banks.\n"
                        )));
            *rv = SHR_E_PARAM;
            return;
        }
    }

    if (ctrl_out) {
        bank_bitmap = ctrl_in->tot.l3_bank_bitmap & bank_bitmap;
        num_banks = shr_util_popcount(bank_bitmap);
        switch (db) {
            case IPDB0:
            ctrl_out->db0.l3_bank_bitmap = bank_bitmap;
            ctrl_out->db0.num_l3_banks = num_banks;
            break;

            case IPDB1:
            ctrl_out->db1.l3_bank_bitmap = bank_bitmap;
            ctrl_out->db1.num_l3_banks = num_banks;
            break;

            case IPDB2:
            ctrl_out->db2.l3_bank_bitmap = bank_bitmap;
            ctrl_out->db2.num_l3_banks = num_banks;
            break;

            case IPDB3:
            ctrl_out->db3.l3_bank_bitmap = bank_bitmap;
            ctrl_out->db3.num_l3_banks = num_banks;
            break;

            default:
            *rv = SHR_E_PARAM;
        }
    }

    return;
}

static const char *
bcm56780_a0_alpm_key_mux_type_name(uint32_t key_mux_type)
{
    switch (key_mux_type) {
    case EM_TILE_ALPM_KEY_MUX_TYPE_NONE: return "n/a";
    case EM_TILE_ALPM_KEY_MUX_TYPE_L3UC_DST: return "L3UC_DST";
    case EM_TILE_ALPM_KEY_MUX_TYPE_L3UC_SRC: return "L3UC_SRC";
    case EM_TILE_ALPM_KEY_MUX_TYPE_COMP_DST: return "COMP_DST";
    case EM_TILE_ALPM_KEY_MUX_TYPE_COMP_SRC: return "COMP_SRC";
    case EM_TILE_ALPM_KEY_MUX_TYPE_L3UC_DST_MTOP: return "L3UC_DST_MTOP";
    case EM_TILE_ALPM_KEY_MUX_TYPE_L3UC_SRC_MTOP: return "L3UC_SRC_MTOP";
    case EM_TILE_ALPM_KEY_MUX_TYPE_COMP_DST_MTOP: return "COMP_DST_MTOP";
    case EM_TILE_ALPM_KEY_MUX_TYPE_COMP_SRC_MTOP: return "COMP_SRC_MTOP";
    default: return "?";
    }
}

static const char *
bcm56780_a0_alpm_pdd_type_name(uint32_t pdd_type)
{
    switch (pdd_type) {
    case EM_TILE_ALPM_PDD_TYPE_NONE: return "n/a";
    case EM_TILE_ALPM_PDD_TYPE_L3UC_DST: return "L3UC_DST";
    case EM_TILE_ALPM_PDD_TYPE_L3UC_SRC: return "L3UC_SRC";
    case EM_TILE_ALPM_PDD_TYPE_COMP_DST: return "COMP_DST";
    case EM_TILE_ALPM_PDD_TYPE_COMP_SRC: return "COMP_SRC";
    case EM_TILE_ALPM_PDD_TYPE_L3UC_DST_MTOP: return "L3UC_DST_MTOP";
    case EM_TILE_ALPM_PDD_TYPE_L3UC_SRC_MTOP: return "L3UC_SRC_MTOP";
    case EM_TILE_ALPM_PDD_TYPE_COMP_DST_MTOP: return "COMP_DST_MTOP";
    case EM_TILE_ALPM_PDD_TYPE_COMP_SRC_MTOP: return "COMP_SRC_MTOP";
    default: return "?";
    }
}

static const char *
bcm56780_a0_alpm_lt_type_name(bcm56780_lt_type_t lt_type)
{
    switch (lt_type) {
    case ALPM_LT_TYPE_LPM_NONE: return "n/a";
    case ALPM_LT_TYPE_LPM_DST: return "LPM_DST";
    case ALPM_LT_TYPE_LPM_SRC: return "LPM_SRC";
    case ALPM_LT_TYPE_FP_COMP_DST: return "FP_COMP_DST";
    case ALPM_LT_TYPE_FP_COMP_SRC: return "FP_COMP_SRC";
    default: return "?";
    }
}

static const char *
bcm56780_a0_alpm_key_input_name(int key_input)
{
    switch (key_input) {
    case KEY_INPUT_LPM_DST_Q: return "LPM_DST QUAD";
    case KEY_INPUT_LPM_DST_D: return "LPM_DST DOUBLE";
    case KEY_INPUT_LPM_DST_S: return "LPM_DST SINGLE";
    case KEY_INPUT_LPM_SRC_Q: return "LPM_SRC QUAD";
    case KEY_INPUT_LPM_SRC_D: return "LPM_SRC DOUBLE";
    case KEY_INPUT_LPM_SRC_S: return "LPM_SRC SINGLE";
    case KEY_INPUT_FP_COMP_DST: return "FP COMP DST";
    case KEY_INPUT_FP_COMP_SRC: return "FP COMP SRC";
    default: return "?";
    }
}


static int
bcm56780_a0_cth_alpm_block_key_muxes(int u, int m, uint32_t key_mux_count,
                                     uint32_t *key_mux)
{
    uint32_t i;
    uint32_t actual_count;
    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(
        bcmptm_uft_alpm_key_mux_type_get(u, key_mux_count, m,
                                         key_mux,
                                         &actual_count));

    if (actual_count != BCM56780_ALPM_L1_KEY_MUXES) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    for (i = 0; i < actual_count; i++) {
        if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
            cli_out("Key mux %d type %s\n", i,
                    bcm56780_a0_alpm_key_mux_type_name(key_mux[i]));
        }
    }
exit:
    SHR_FUNC_EXIT();
}


static int
bcm56780_a0_cth_alpm_block_pdd_types(int u, int m,
                                     uint32_t pdd_count,
                                     uint32_t *pdd_types)
{
    uint32_t i;
    uint32_t actual_count;
    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(
        bcmptm_uft_alpm_pdd_type_get(u, pdd_count, m,
                                     pdd_types,
                                     &actual_count));

    if (actual_count != BCM56780_ALPM_L3_BLOCKS) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    for (i = 0; i < actual_count; i++) {
        if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
            cli_out("Pdd %d type %s\n", i,
                    bcm56780_a0_alpm_pdd_type_name(pdd_types[i]));
        }
    }
exit:
    SHR_FUNC_EXIT();
}


static int
bcm56780_a0_cth_alpm_block_lt_types_mtop(int u, int level,
                                         int block_count,
                                         bcm56780_lt_type_t *lt_types)
{
    bcmltd_sid_t ltid;
    bcmptm_pt_banks_info_t info;
    int i, j, k, sid_idx;
    int actual_block_count;

    static bcmdrd_sid_t l1_sids[] = {
        MEMDB_TCAM_M_CTL_MEM0_0m,
        MEMDB_TCAM_M_CTL_MEM0_1m,
        MEMDB_TCAM_M_CTL_MEM1_0m,
        MEMDB_TCAM_M_CTL_MEM1_1m,
        MEMDB_TCAM_M_CTL_MEM2_0m,
        MEMDB_TCAM_M_CTL_MEM2_1m,
        MEMDB_TCAM_M_CTL_MEM3_0m,
        MEMDB_TCAM_M_CTL_MEM3_1m,
        MEMDB_TCAM_M_CTL_MEM4_0m,
        MEMDB_TCAM_M_CTL_MEM4_1m,
        MEMDB_TCAM_M_CTL_MEM5_0m,
        MEMDB_TCAM_M_CTL_MEM5_1m,
        MEMDB_TCAM_M_CTL_MEM6_0m,
        MEMDB_TCAM_M_CTL_MEM6_1m,
        MEMDB_TCAM_M_CTL_MEM7_0m,
        MEMDB_TCAM_M_CTL_MEM7_1m
    };

    static bcmdrd_sid_t l3_sids[] = {
        M_DEFIP_ALPM_LEVEL3_B0m,
        M_DEFIP_ALPM_LEVEL3_B1m,
        M_DEFIP_ALPM_LEVEL3_B2m,
        M_DEFIP_ALPM_LEVEL3_B3m,
        M_DEFIP_ALPM_LEVEL3_B4m,
        M_DEFIP_ALPM_LEVEL3_B5m,
        M_DEFIP_ALPM_LEVEL3_B6m,
        M_DEFIP_ALPM_LEVEL3_B7m,
    };

    SHR_FUNC_ENTER(u);
    if (level == LEVEL1) {
        actual_block_count = BCM56780_ALPM_L1_BLOCKS(ALPM_1);
    } else if (level == LEVEL3) {
        actual_block_count = BCM56780_ALPM_L3_BLOCKS;
    } else {
        return SHR_E_PARAM;
    }
    if (block_count < actual_block_count) {
        return SHR_E_PARAM;
    }
    for (i = 0; i < actual_block_count; i++) {
        for (j = 0; j < BCM56780_ALPM_BLOCKS; j++) {
            ltid = bcmlrd_table_name_to_idx(u, bcm56780_lt_names_2_lt_type_mtop[j].lt_name);
            SHR_IF_ERR_EXIT(
                bcmptm_cth_uft_bank_info_get_from_lt(u, ltid, level + 1, &info));
            k = 0;
            if (level == LEVEL1) {
                for (k = 0; k < info.bank_cnt; k++) {
                    sid_idx = i * 2;
                    if (info.bank[k].bank_sid == l1_sids[sid_idx++] ||
                        info.bank[k].bank_sid == l1_sids[sid_idx++]) {
                        break;
                    }
                }
            } else if (level == LEVEL3) {
                for (k = 0; k < info.bank_cnt; k++) {
                    sid_idx = i * 2;
                    if (info.bank[k].bank_sid == l3_sids[sid_idx++] ||
                        info.bank[k].bank_sid == l3_sids[sid_idx++]) {
                        break;
                    }
                }
            }
            if (k < info.bank_cnt) {
                break;
            }
        }
        if (j < BCM56780_ALPM_BLOCKS) {
            lt_types[i] = bcm56780_lt_names_2_lt_type_mtop[j].type;
        } else {
            lt_types[i] = ALPM_LT_TYPE_LPM_NONE;
        }
        if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
            cli_out("MTOP Level %d block %d lt_type %s\n", level + 1, i,
                    bcm56780_a0_alpm_lt_type_name(lt_types[i]));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_alpm_block_lt_types(int u, int m, int level,
                                    int block_count,
                                    bcm56780_lt_type_t *lt_types)
{


    bcmltd_sid_t ltid;
    bcmptm_pt_banks_info_t info;
    int i, j, k, sid_idx;
    int actual_block_count;

    static bcmdrd_sid_t l1_sids[] = {
        MEMDB_TCAM_IFTA80_MEM0_0m,
        MEMDB_TCAM_IFTA80_MEM1_0m,
        MEMDB_TCAM_IFTA80_MEM0_1m,
        MEMDB_TCAM_IFTA80_MEM1_1m,
        MEMDB_TCAM_IFTA80_MEM2_0m,
        MEMDB_TCAM_IFTA80_MEM3_0m,
        MEMDB_TCAM_IFTA80_MEM2_1m,
        MEMDB_TCAM_IFTA80_MEM3_1m,
        MEMDB_TCAM_IFTA80_MEM4_0m,
        MEMDB_TCAM_IFTA80_MEM5_0m,
        MEMDB_TCAM_IFTA80_MEM4_1m,
        MEMDB_TCAM_IFTA80_MEM5_1m,
        MEMDB_TCAM_IFTA80_MEM6_0m,
        MEMDB_TCAM_IFTA80_MEM7_0m,
        MEMDB_TCAM_IFTA80_MEM6_1m,
        MEMDB_TCAM_IFTA80_MEM7_1m
    };

    static bcmdrd_sid_t l3_sids[] = {
        L3_DEFIP_ALPM_LEVEL3_B0m,
        L3_DEFIP_ALPM_LEVEL3_B1m,
        L3_DEFIP_ALPM_LEVEL3_B2m,
        L3_DEFIP_ALPM_LEVEL3_B3m,
        L3_DEFIP_ALPM_LEVEL3_B4m,
        L3_DEFIP_ALPM_LEVEL3_B5m,
        L3_DEFIP_ALPM_LEVEL3_B6m,
        L3_DEFIP_ALPM_LEVEL3_B7m,
    };

    SHR_FUNC_ENTER(u);

    if ((bcmdrd_dev_flags_get(u) & CHIP_FLAG_MTOP) && m) {
        SHR_IF_ERR_EXIT(
            bcm56780_a0_cth_alpm_block_lt_types_mtop(u, level,
                                                     block_count, lt_types));
        SHR_EXIT();
    }

    if (level == LEVEL1) {
        actual_block_count = BCM56780_ALPM_L1_BLOCKS(m);
    } else if (level == LEVEL3) {
        actual_block_count = BCM56780_ALPM_L3_BLOCKS;
    } else {
        return SHR_E_PARAM;
    }
    if (block_count < actual_block_count) {
        return SHR_E_PARAM;
    }
    for (i = 0; i < actual_block_count; i++) {
        for (j = 0; j < BCM56780_ALPM_BLOCKS; j++) {
            ltid = bcmlrd_table_name_to_idx(u, bcm56780_lt_names_2_lt_type[j].lt_name);
            SHR_IF_ERR_EXIT(
                bcmptm_cth_uft_bank_info_get_from_lt(u, ltid, level + 1, &info));
            k = 0;
            if (level == LEVEL1) {
                for (k = 0; k < info.bank_cnt; k++) {
                    sid_idx = i * 4;
                    if (info.bank[k].bank_sid == l1_sids[sid_idx++] ||
                        info.bank[k].bank_sid == l1_sids[sid_idx++] ||
                        info.bank[k].bank_sid == l1_sids[sid_idx++] ||
                        info.bank[k].bank_sid == l1_sids[sid_idx++]) {
                        break;
                    }
                }
            } else if (level == LEVEL3) {
                for (k = 0; k < info.bank_cnt; k++) {
                    sid_idx = i * 2;
                    if (info.bank[k].bank_sid == l3_sids[sid_idx++] ||
                        info.bank[k].bank_sid == l3_sids[sid_idx++]) {
                        break;
                    }
                }
            }
            if (k < info.bank_cnt) {
                break;
            }
        }
        if (j < BCM56780_ALPM_BLOCKS) {
            lt_types[i] = bcm56780_lt_names_2_lt_type[j].type;
        } else {
            lt_types[i] = ALPM_LT_TYPE_LPM_NONE;
        }
        if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
            cli_out("Level %d block %d lt_type %s\n", level + 1, i,
                    bcm56780_a0_alpm_lt_type_name(lt_types[i]));
        }
    }

exit:
    SHR_FUNC_EXIT();
}


static void
bcm56780_a0_alpm_db_from_lt_type(int u,
                                 int alpm_mode,
                                 bool is_uft_assign,
                                 bcm56780_lt_type_t lt_type,
                                 uint8_t *db,
                                 int *rv)
{
    if (SHR_FAILURE(*rv)) {
        return;
    }
    *db = bcm56780_l3_db_layout[alpm_mode][lt_type];
    if (*db == IPDBX) {
        if (lt_type == ALPM_LT_TYPE_LPM_NONE) {
            /* simply indicates user assigns no ALPM LT type.  */
        } else if (is_uft_assign) {
            /* indicates user assigns LT type that cannot be supported in current ALPM mode. */
            /* this is a resource wastage, but not necessarily an error. */
            LOG_INFO(BSL_LOG_MODULE,
                        (BSL_META(
                            "LT type %s couldn't be utilized with current ALPM setting.\n"
                        ), bcm56780_a0_alpm_lt_type_name(lt_type)));

         }
    }

    return;
}



static void
bcm56780_a0_l2_combined_block_dbs(int u, bcmptm_cth_alpm_control_t *ctrl, uint8_t *blocks)
{
    uint8_t sub;
    sub = (ctrl->db0_levels >= 2);
    sal_memcpy(blocks, &bcm56780_combined_blocks[sub][0], 4);
    return;
}

static void
bcm56780_a0_l2_parallel_block_dbs(int u, bcmptm_cth_alpm_control_t *ctrl, uint8_t *blocks)
{
    uint8_t sub;
    sub = ((ctrl->db0_levels >= 2) << 1) | (ctrl->db1_levels >= 2);
    sal_memcpy(blocks, &bcm56780_parallel_blocks[sub][0], 4);
    return;
}

static void
bcm56780_a0_l2_combined_rpf_block_dbs(int u, bcmptm_cth_alpm_control_t *ctrl, uint8_t *blocks)
{
    uint8_t sub;
    sub = ((ctrl->db0_levels >= 2) << 1) | (ctrl->db2_levels >= 2);
    sal_memcpy(blocks, &bcm56780_combined_rpf_blocks[sub][0], 4);
    return;

}
static void
bcm56780_a0_l2_parallel_rpf_block_dbs(int u, bcmptm_cth_alpm_control_t *ctrl, uint8_t *blocks)
{
    uint8_t sub;
    sub = ((ctrl->db0_levels >= 2) << 3) |
          ((ctrl->db1_levels >= 2) << 2) |
          ((ctrl->db2_levels >= 2) << 1) |
           (ctrl->db3_levels >= 2);
    sal_memcpy(blocks, &bcm56780_parallel_rpf_blocks[sub][0], 4);
    return;
}

static void
bcm56780_a0_l2_fp_no_lpm_block_dbs(int u, bcmptm_cth_alpm_control_t *ctrl, uint8_t *blocks)
{
    uint8_t sub;
    sub = ((ctrl->db2_levels >= 2) << 1) |
           (ctrl->db3_levels >= 2);
    sal_memcpy(blocks, &bcm56780_fp_no_lpm_blocks[sub][0], 4);
    return;
}

static void
bcm56780_a0_l2_fp_combined_block_dbs(int u, bcmptm_cth_alpm_control_t *ctrl, uint8_t *blocks)
{
    uint8_t sub;
    sub = ((ctrl->db0_levels >= 2) << 2) |
          ((ctrl->db2_levels >= 2) << 1) |
           (ctrl->db3_levels >= 2);
    sal_memcpy(blocks, &bcm56780_fp_combined_blocks[sub][0], 4);
    return;
}

static void
bcm56780_a0_l2_fp_parallel_block_dbs(int u, bcmptm_cth_alpm_control_t *ctrl, uint8_t *blocks)
{
    uint8_t sub;
    sub = ((ctrl->db0_levels >= 2) << 3) |
          ((ctrl->db1_levels >= 2) << 2) |
          ((ctrl->db2_levels >= 2) << 1) |
           (ctrl->db3_levels >= 2);
    sal_memcpy(blocks, &bcm56780_fp_parallel_blocks[sub][0], 4);
    return;
}

static void
bcm56780_a0_l2_fp_combined_rpf_block_dbs(int u, bcmptm_cth_alpm_control_t *ctrl, uint8_t *blocks)
{
    uint8_t sub;
    sub = ((ctrl->db0_levels >= 2) << 3) |
          ((ctrl->db1_levels >= 2) << 2) |
          ((ctrl->db2_levels >= 2) << 1) |
           (ctrl->db3_levels >= 2);
    sal_memcpy(blocks, &bcm56780_fp_combined_rpf_blocks[sub][0], 4);
    return;
}


static void
bcm56780_a0_l3_block_dbs(int u, int m, bcmptm_cth_alpm_control_t *ctrl, uint8_t *blocks, int *rv)
{
    uint32_t uft_assign_db_map =0;
    uint32_t alpm_request_db_map = 0;
#define ALPM_L3_TILES 4
    int i;
    bcm56780_lt_type_t l3_lt_types[ALPM_L3_TILES] = {ALPM_LT_TYPE_LPM_NONE};
    uint8_t  request_dbs[4];
    int db01_count = 0;
    int db23_count = 0;
    int mix_db_count;
    int am = ctrl->alpm_mode;

    if (SHR_FAILURE(*rv)) {
        return;
    }
    bcm56780_a0_cth_alpm_block_lt_types(u, m, LEVEL3, ALPM_L3_TILES, &l3_lt_types[0]);

    bcm56780_a0_alpm_db_from_lt_type(u, am, 1, l3_lt_types[0], &blocks[0], rv);
    bcm56780_a0_alpm_db_from_lt_type(u, am, 1, l3_lt_types[1], &blocks[1], rv);
    bcm56780_a0_alpm_db_from_lt_type(u, am, 1, l3_lt_types[2], &blocks[2], rv);
    bcm56780_a0_alpm_db_from_lt_type(u, am, 1, l3_lt_types[3], &blocks[3], rv);


    bcm56780_a0_alpm_db_from_lt_type(u, am, 0, ALPM_LT_TYPE_LPM_DST, &request_dbs[0], rv);
    bcm56780_a0_alpm_db_from_lt_type(u, am, 0, ALPM_LT_TYPE_LPM_SRC, &request_dbs[1], rv);
    bcm56780_a0_alpm_db_from_lt_type(u, am, 0, ALPM_LT_TYPE_FP_COMP_DST, &request_dbs[2], rv);
    bcm56780_a0_alpm_db_from_lt_type(u, am, 0, ALPM_LT_TYPE_FP_COMP_SRC, &request_dbs[3], rv);


    if (SHR_FAILURE(*rv)) {
        return;
    }

    for (i = 0; i < 4; i++) {
        /* Set UFT assign db map, excluding db that is less than 3 levels.  */
        if (blocks[i] == IPDB01) {
            if (bcm56780_a0_alpm_db_levels(ctrl, IPDB0) >=3 &&
                bcm56780_a0_alpm_db_levels(ctrl, IPDB1) >=3) {
                uft_assign_db_map |= 1 << IPDB0;
                uft_assign_db_map |= 1 << IPDB1;
                db01_count++;
            } else if (bcm56780_a0_alpm_db_levels(ctrl, IPDB0) >=3) {
                uft_assign_db_map |= 1 << IPDB0;
                blocks[i] = IPDB0;
            } else if (bcm56780_a0_alpm_db_levels(ctrl, IPDB1) >=3) {
                uft_assign_db_map |= 1 << IPDB1;
                blocks[i] = IPDB1;
            }

        } else if (blocks[i] == IPDB23) {
            if (bcm56780_a0_alpm_db_levels(ctrl, IPDB2) >=3 &&
                bcm56780_a0_alpm_db_levels(ctrl, IPDB3) >=3) {
                uft_assign_db_map |= 1 << IPDB2;
                uft_assign_db_map |= 1 << IPDB3;
                db23_count++;
            } else if (bcm56780_a0_alpm_db_levels(ctrl, IPDB2) >=3) {
                uft_assign_db_map |= 1 << IPDB2;
                blocks[i] = IPDB2;
            } else if (bcm56780_a0_alpm_db_levels(ctrl, IPDB3) >=3) {
                uft_assign_db_map |= 1 << IPDB3;
                blocks[i] = IPDB3;
            }
        } else if (blocks[i] != IPDBX) {
            if (bcm56780_a0_alpm_db_levels(ctrl, blocks[i]) >=3) {
                uft_assign_db_map |= 1 << blocks[i];
            }
        }

        /* Set ALPM request db map, excluding db that is less than 3 levels. */
        if (request_dbs[i] == IPDB01) {
            if (bcm56780_a0_alpm_db_levels(ctrl, IPDB0) >=3) {
                alpm_request_db_map |= 1 << IPDB0;
            }
            if (bcm56780_a0_alpm_db_levels(ctrl, IPDB1) >=3) {
                alpm_request_db_map |= 1 << IPDB1;
            }
        } else if (request_dbs[i] == IPDB23) {
            if (bcm56780_a0_alpm_db_levels(ctrl, IPDB2) >=3) {
                alpm_request_db_map |= 1 << IPDB2;
            }
            if (bcm56780_a0_alpm_db_levels(ctrl, IPDB3) >=3) {
                alpm_request_db_map |= 1 << IPDB3;
            }
        } else if (request_dbs[i] != IPDBX) {
            if (bcm56780_a0_alpm_db_levels(ctrl, request_dbs[i]) >=3) {
                alpm_request_db_map |= 1 << request_dbs[i];
            }
        }

    }

    if (alpm_request_db_map & ~uft_assign_db_map) {
        /* indicate under resource. this is an error. */
        LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META(
                        "Insufficient ALPM lt types.\n"
                    )));

        *rv = SHR_E_PARAM;
        return;
    }


    mix_db_count = 0;
    if (db01_count == 1) {
        /* cannot support parallel mode with one DB. This is an error. */
        LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META(
                        "Insufficient LPM DST LT types to support Parallel mode.\n"
                    )));
        *rv = SHR_E_PARAM;
        return;
    } else if (db01_count) {
        for (i = 0; i < 4; i++) {
            if (blocks[i] == IPDB01) {
                /* db01_count : comparand (db01_count + 1) / 2
                 * 0 : 0 n/a
                 * 1 : 1 !error
                 * 2 : 1 (DB0 DB1)
                 * 3 : 2 (DB0 DB0 DB1)
                 * 4 : 2 (DB0 DB0 DB1 DB1))
                 */
                if (mix_db_count >= (db01_count + 1) / 2) {
                   blocks[i] = IPDB1;
                } else {
                   blocks[i] = IPDB0;
                }
                mix_db_count++;
            }
        }
    }

    mix_db_count = 0;
    if (db23_count == 1) {
        /* cannot support parallel mode with one DB. This is an error. */
        LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META(
                        "Insufficient LPM SRC LT types to support Parallel mode.\n"
                    )));
        *rv = SHR_E_PARAM;
        return;
    } else if (db23_count) {
        for (i = 0; i < 4; i++) {
            if (blocks[i] == IPDB23) {
                /* db23_count : comparand (db23_count + 1) / 2
                 * 0 : 0 n/a
                 * 1 : 1 !error
                 * 2 : 1 (DB2 DB3)
                 * 3 : 2 (DB2 DB2 DB3)
                 * 4 : 2 (DB2 DB2 DB3 DB3))
                 */
                if (mix_db_count >= (db23_count + 1) / 2) {
                   blocks[i] = IPDB3;
                } else {
                   blocks[i] = IPDB2;
                }
                mix_db_count++;
            }
        }
    }

    return;
}


static int
bcm56780_a0_cth_alpm_combined_check_and_set(int u, int m,
                                            bcmptm_cth_alpm_control_t *ctrl,
                                            bcmptm_cth_alpm_control_t *ctrl_out)
{
    int rv = SHR_E_NONE;
    uint8_t blocks[4];
    bcm56780_a0_l2_combined_block_dbs(u, ctrl, &blocks[0]);
    bcm56780_a0_l2_bitmap_check_and_set(blocks, IPDB0, ctrl, ctrl_out, &rv);

    bcm56780_a0_l3_combined_block_dbs(u, m, ctrl, &blocks[0], &rv);
    bcm56780_a0_l3_bitmap_check_and_set(blocks, IPDB0, ctrl, ctrl_out, &rv);

    return rv;
}

static int
bcm56780_a0_cth_alpm_parallel_check_and_set(int u, int m,
                                            bcmptm_cth_alpm_control_t *ctrl,
                                            bcmptm_cth_alpm_control_t *ctrl_out)
{
    int rv = SHR_E_NONE;
    uint8_t blocks[4];

    bcm56780_a0_l2_parallel_block_dbs(u, ctrl, &blocks[0]);
    bcm56780_a0_l2_bitmap_check_and_set(blocks, IPDB0, ctrl, ctrl_out, &rv);
    bcm56780_a0_l2_bitmap_check_and_set(blocks, IPDB1, ctrl, ctrl_out, &rv);

    bcm56780_a0_l3_parallel_block_dbs(u, m, ctrl, &blocks[0], &rv);
    bcm56780_a0_l3_bitmap_check_and_set(blocks, IPDB0, ctrl, ctrl_out, &rv);
    bcm56780_a0_l3_bitmap_check_and_set(blocks, IPDB1, ctrl, ctrl_out, &rv);

    return rv;
}

static int
bcm56780_a0_cth_alpm_combined_rpf_check_and_set(int u, int m,
                                                bcmptm_cth_alpm_control_t *ctrl,
                                                bcmptm_cth_alpm_control_t *ctrl_out)
{
    int rv = SHR_E_NONE;
    uint8_t blocks[4];

    bcm56780_a0_l2_combined_rpf_block_dbs(u, ctrl, &blocks[0]);
    bcm56780_a0_l2_bitmap_check_and_set(blocks, IPDB0, ctrl, ctrl_out, &rv);
    bcm56780_a0_l2_bitmap_check_and_set(blocks, IPDB2, ctrl, ctrl_out, &rv);

    bcm56780_a0_l3_combined_rpf_block_dbs(u, m, ctrl, &blocks[0], &rv);
    bcm56780_a0_l3_bitmap_check_and_set(blocks, IPDB0, ctrl, ctrl_out, &rv);
    bcm56780_a0_l3_bitmap_check_and_set(blocks, IPDB2, ctrl, ctrl_out, &rv);
    return rv;
}

static int
bcm56780_a0_cth_alpm_parallel_rpf_check_and_set(int u, int m,
                                                bcmptm_cth_alpm_control_t *ctrl,
                                                bcmptm_cth_alpm_control_t *ctrl_out)
{
    /*
     * In RPF & PARALLEL mode, we assume
     * 1. DB0 - LPM_DST Private - block 0
     * 2. DB1 - LPM_DST Public  - block 1
     * 3. DB2 - LPM_SRC Private - block 2
     * 4. DB3 - LPM_SRC Public  - block 3
     */
    int rv = SHR_E_NONE;
    uint8_t blocks[4];

    bcm56780_a0_l2_parallel_rpf_block_dbs(u, ctrl, &blocks[0]);
    bcm56780_a0_l2_bitmap_check_and_set(blocks, IPDB0, ctrl, ctrl_out, &rv);
    bcm56780_a0_l2_bitmap_check_and_set(blocks, IPDB1, ctrl, ctrl_out, &rv);
    bcm56780_a0_l2_bitmap_check_and_set(blocks, IPDB2, ctrl, ctrl_out, &rv);
    bcm56780_a0_l2_bitmap_check_and_set(blocks, IPDB3, ctrl, ctrl_out, &rv);

    bcm56780_a0_l3_parallel_rpf_block_dbs(u, m, ctrl, &blocks[0], &rv);
    bcm56780_a0_l3_bitmap_check_and_set(blocks, IPDB0, ctrl, ctrl_out, &rv);
    bcm56780_a0_l3_bitmap_check_and_set(blocks, IPDB1, ctrl, ctrl_out, &rv);
    bcm56780_a0_l3_bitmap_check_and_set(blocks, IPDB2, ctrl, ctrl_out, &rv);
    bcm56780_a0_l3_bitmap_check_and_set(blocks, IPDB3, ctrl, ctrl_out, &rv);

    return rv;
}
static int
bcm56780_a0_cth_alpm_fp_no_lpm_check_and_set(int u, int m,
                                             bcmptm_cth_alpm_control_t *ctrl,
                                             bcmptm_cth_alpm_control_t *ctrl_out)
{
    /*
     * 1. DB2 - FP_COMP DST  - block 2
     * 2. DB3 - FP_COMP SRC  - block 3
     */
    int rv = SHR_E_NONE;
    uint8_t blocks[4];

    bcm56780_a0_l2_fp_no_lpm_block_dbs(u, ctrl, &blocks[0]);
    bcm56780_a0_l2_bitmap_check_and_set(blocks, IPDB2, ctrl, ctrl_out, &rv);
    bcm56780_a0_l2_bitmap_check_and_set(blocks, IPDB3, ctrl, ctrl_out, &rv);


    bcm56780_a0_l3_fp_no_lpm_block_dbs(u, m, ctrl, &blocks[0], &rv);
    bcm56780_a0_l3_bitmap_check_and_set(blocks, IPDB2, ctrl, ctrl_out, &rv);
    bcm56780_a0_l3_bitmap_check_and_set(blocks, IPDB3, ctrl, ctrl_out, &rv);
    return rv;
}
static int
bcm56780_a0_cth_alpm_fp_combined_check_and_set(int u, int m,
                                               bcmptm_cth_alpm_control_t *ctrl,
                                               bcmptm_cth_alpm_control_t *ctrl_out)
{
    /*
     * 1. DB0 - LPM_DST      - block 0/1
     * 2. DB2 - FP_COMP DST  - block 2
     * 3. DB3 - FP_COMP SRC  - block 3
     */
    int rv = SHR_E_NONE;
    uint8_t blocks[4];

    bcm56780_a0_l2_fp_combined_block_dbs(u, ctrl, &blocks[0]);
    bcm56780_a0_l2_bitmap_check_and_set(blocks, IPDB0, ctrl, ctrl_out, &rv);
    bcm56780_a0_l2_bitmap_check_and_set(blocks, IPDB2, ctrl, ctrl_out, &rv);
    bcm56780_a0_l2_bitmap_check_and_set(blocks, IPDB3, ctrl, ctrl_out, &rv);


    bcm56780_a0_l3_fp_combined_block_dbs(u, m, ctrl, &blocks[0], &rv);
    bcm56780_a0_l3_bitmap_check_and_set(blocks, IPDB0, ctrl, ctrl_out, &rv);
    bcm56780_a0_l3_bitmap_check_and_set(blocks, IPDB2, ctrl, ctrl_out, &rv);
    bcm56780_a0_l3_bitmap_check_and_set(blocks, IPDB3, ctrl, ctrl_out, &rv);
    return rv;
}

static int
bcm56780_a0_cth_alpm_fp_parallel_check_and_set(int u, int m,
                                               bcmptm_cth_alpm_control_t *ctrl,
                                               bcmptm_cth_alpm_control_t *ctrl_out)
{
    /*
     * 1. DB0 - LPM_DST 0    - block 0
     * 3. DB1 - LPM_DST 1    - block 1
     * 3. DB2 - FP_COMP DST  - block 2
     * 4. DB3 - FP_COMP SRC  - block 3
     */
    int rv = SHR_E_NONE;
    uint8_t blocks[4];

    bcm56780_a0_l2_fp_parallel_block_dbs(u, ctrl, &blocks[0]);
    bcm56780_a0_l2_bitmap_check_and_set(blocks, IPDB0, ctrl, ctrl_out, &rv);
    bcm56780_a0_l2_bitmap_check_and_set(blocks, IPDB1, ctrl, ctrl_out, &rv);
    bcm56780_a0_l2_bitmap_check_and_set(blocks, IPDB2, ctrl, ctrl_out, &rv);
    bcm56780_a0_l2_bitmap_check_and_set(blocks, IPDB3, ctrl, ctrl_out, &rv);

    bcm56780_a0_l3_fp_parallel_block_dbs(u, m, ctrl, &blocks[0], &rv);
    bcm56780_a0_l3_bitmap_check_and_set(blocks, IPDB0, ctrl, ctrl_out, &rv);
    bcm56780_a0_l3_bitmap_check_and_set(blocks, IPDB1, ctrl, ctrl_out, &rv);
    bcm56780_a0_l3_bitmap_check_and_set(blocks, IPDB2, ctrl, ctrl_out, &rv);
    bcm56780_a0_l3_bitmap_check_and_set(blocks, IPDB3, ctrl, ctrl_out, &rv);

    return rv;
}

static int
bcm56780_a0_cth_alpm_fp_combined_rpf_check_and_set(int u, int m,
                                                   bcmptm_cth_alpm_control_t *ctrl,
                                                   bcmptm_cth_alpm_control_t *ctrl_out)
{

    /*
     * 1. DB0 - LPM_DST      - block 0
     * 3. DB1 - LPM_SRC      - block 1
     * 3. DB2 - FP_COMP DST  - block 2
     * 4. DB3 - FP_COMP SRC  - block 3
     */
    int rv = SHR_E_NONE;
    uint8_t blocks[4];

    bcm56780_a0_l2_fp_combined_rpf_block_dbs(u, ctrl, &blocks[0]);
    bcm56780_a0_l2_bitmap_check_and_set(blocks, IPDB0, ctrl, ctrl_out, &rv);
    bcm56780_a0_l2_bitmap_check_and_set(blocks, IPDB1, ctrl, ctrl_out, &rv);
    bcm56780_a0_l2_bitmap_check_and_set(blocks, IPDB2, ctrl, ctrl_out, &rv);
    bcm56780_a0_l2_bitmap_check_and_set(blocks, IPDB3, ctrl, ctrl_out, &rv);

    bcm56780_a0_l3_fp_combined_rpf_block_dbs(u, m, ctrl, &blocks[0], &rv);
    bcm56780_a0_l3_bitmap_check_and_set(blocks, IPDB0, ctrl, ctrl_out, &rv);
    bcm56780_a0_l3_bitmap_check_and_set(blocks, IPDB1, ctrl, ctrl_out, &rv);
    bcm56780_a0_l3_bitmap_check_and_set(blocks, IPDB2, ctrl, ctrl_out, &rv);
    bcm56780_a0_l3_bitmap_check_and_set(blocks, IPDB3, ctrl, ctrl_out, &rv);
    return rv;
}

static void
bcm56780_a0_cth_alpm_combined_init(int u, int m,
                                   bcmptm_cth_alpm_control_t *ctrl,
                                   bcm56780_lpm_ip_control_t *ip_ctrl,
                                   uint32_t *pdd_types,
                                   int *rv)
{
    uint8_t blocks[4];
    int i;

    bcm56780_a0_l2_combined_block_dbs(u, ctrl, &blocks[0]);
    ip_ctrl->level2_database_sel_block[0] = blocks[0];
    ip_ctrl->level2_database_sel_block[1] = blocks[1];
    ip_ctrl->level2_database_sel_block[2] = blocks[2];
    ip_ctrl->level2_database_sel_block[3] = blocks[3];

    bcm56780_a0_l3_combined_block_dbs(u, m, ctrl, &blocks[0], rv);
    ip_ctrl->level3_database_sel_block[0] = blocks[0];
    ip_ctrl->level3_database_sel_block[1] = blocks[1];
    ip_ctrl->level3_database_sel_block[2] = blocks[2];
    ip_ctrl->level3_database_sel_block[3] = blocks[3];

    ip_ctrl->destination[0] = ctrl->destination;
    ip_ctrl->destination_mask[0] = ctrl->destination_mask;
    ip_ctrl->destination_type_match[0] = ctrl->destination_type_match;
    ip_ctrl->destination_type_non_match[0] = ctrl->destination_type_non_match;
    bcm56780_a0_cth_alpm_combined_check_and_set(u, m, ctrl, ctrl);


    /* Output DB sel configs */
    ip_ctrl->result_mode = FOUR_RESULTS_MODE;
    ip_ctrl->data_sel[0] = IPDBX;
    ip_ctrl->data_sel[1] = IPDBX;
    ip_ctrl->data_sel[2] = IPDBX;
    ip_ctrl->data_sel[3] = IPDBX;

    for (i = 0; i < BCM56780_ALPM_L3_BLOCKS; i++) {
        if (pdd_types[i] == bcm56780_a0_pdd_type_l3uc_dst[m]) {
            ip_ctrl->data_sel[i] = IPDB0;
            break;
        }
    }

    if (i == BCM56780_ALPM_L3_BLOCKS) {
        LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META(
                        "No proper pdd type found.\n"
                    )));
        *rv = SHR_E_PARAM;
        return;
    }
}

static void
bcm56780_a0_cth_alpm_parallel_init(int u, int m,
                                   bcmptm_cth_alpm_control_t *ctrl,
                                   bcm56780_lpm_ip_control_t *ip_ctrl,
                                   uint32_t *pdd_types,
                                   int *rv)
{
    uint8_t blocks[4];

    bcm56780_a0_l2_parallel_block_dbs(u, ctrl, &blocks[0]);
    ip_ctrl->level2_database_sel_block[0] = blocks[0];
    ip_ctrl->level2_database_sel_block[1] = blocks[1];
    ip_ctrl->level2_database_sel_block[2] = blocks[2];
    ip_ctrl->level2_database_sel_block[3] = blocks[3];

    bcm56780_a0_l3_parallel_block_dbs(u, m, ctrl, &blocks[0], rv);
    ip_ctrl->level3_database_sel_block[0] = blocks[0];
    ip_ctrl->level3_database_sel_block[1] = blocks[1];
    ip_ctrl->level3_database_sel_block[2] = blocks[2];
    ip_ctrl->level3_database_sel_block[3] = blocks[3];

    ip_ctrl->destination[0] = ctrl->destination;
    ip_ctrl->destination_mask[0] = ctrl->destination_mask;
    ip_ctrl->destination_type_match[0] = ctrl->destination_type_match;
    ip_ctrl->destination_type_non_match[0] = ctrl->destination_type_non_match;

    ip_ctrl->destination[1] = ctrl->destination;
    ip_ctrl->destination_mask[1] = ctrl->destination_mask;
    ip_ctrl->destination_type_match[1] = ctrl->destination_type_match;
    ip_ctrl->destination_type_non_match[1] = ctrl->destination_type_non_match;

    bcm56780_a0_cth_alpm_parallel_check_and_set(u, m, ctrl, ctrl);

    /* Output DB sel configs */
    ip_ctrl->data_sel[0] = IPDBX;
    ip_ctrl->data_sel[1] = IPDBX;
    ip_ctrl->data_sel[2] = IPDBX;
    ip_ctrl->data_sel[3] = IPDBX;
    ip_ctrl->result_mode = TWO_RESULTS_MODE;

    if (pdd_types[0] == bcm56780_a0_pdd_type_l3uc_dst[m]) {
        ip_ctrl->data_sel[0] = IPDB0;
        ip_ctrl->data_sel[1] = IPDB1;
    } else if (pdd_types[2] == bcm56780_a0_pdd_type_l3uc_dst[m]) {
        ip_ctrl->data_sel[2] = IPDB0;
        ip_ctrl->data_sel[3] = IPDB1;
    } else {
        LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META(
                        "No proper pdd type found.\n"
                    )));
        *rv = SHR_E_PARAM;
        return;
    }
}

static void
bcm56780_a0_cth_alpm_combined_rpf_init(int u, int m,
                                       bcmptm_cth_alpm_control_t *ctrl,
                                       bcm56780_lpm_ip_control_t *ip_ctrl,
                                       uint32_t *pdd_types,
                                       int *rv)
{
    uint8_t blocks[4];
    int pdd_0 = 0, pdd_1 = 0;
    int i;

    bcm56780_a0_l2_combined_rpf_block_dbs(u, ctrl, &blocks[0]);
    ip_ctrl->level2_database_sel_block[0] = blocks[0];
    ip_ctrl->level2_database_sel_block[1] = blocks[1];
    ip_ctrl->level2_database_sel_block[2] = blocks[2];
    ip_ctrl->level2_database_sel_block[3] = blocks[3];

    bcm56780_a0_l3_combined_rpf_block_dbs(u, m, ctrl, &blocks[0], rv);
    ip_ctrl->level3_database_sel_block[0] = blocks[0];
    ip_ctrl->level3_database_sel_block[1] = blocks[1];
    ip_ctrl->level3_database_sel_block[2] = blocks[2];
    ip_ctrl->level3_database_sel_block[3] = blocks[3];

    ip_ctrl->destination[0] = ctrl->destination;
    ip_ctrl->destination_mask[0] = ctrl->destination_mask;
    ip_ctrl->destination_type_match[0] = ctrl->destination_type_match;
    ip_ctrl->destination_type_non_match[0] = ctrl->destination_type_non_match;

    ip_ctrl->destination[1] = ctrl->destination;
    ip_ctrl->destination_mask[1] = ctrl->destination_mask;
    ip_ctrl->destination_type_match[1] = ctrl->destination_type_match;
    ip_ctrl->destination_type_non_match[1] = ctrl->destination_type_non_match;

    bcm56780_a0_cth_alpm_combined_rpf_check_and_set(u, m, ctrl, ctrl);

    /* Output DB sel configs */
    ip_ctrl->data_sel[0] = IPDBX;
    ip_ctrl->data_sel[1] = IPDBX;
    ip_ctrl->data_sel[2] = IPDBX;
    ip_ctrl->data_sel[3] = IPDBX;
    ip_ctrl->result_mode = FOUR_RESULTS_MODE;

    for (i = 0; i < BCM56780_ALPM_L3_BLOCKS; i++) {
        if (pdd_types[i] == bcm56780_a0_pdd_type_l3uc_dst[m] && pdd_0 == 0) {
            ip_ctrl->data_sel[i] = IPDB0;
            pdd_0 = 1;
        }
        if (pdd_types[i] == bcm56780_a0_pdd_type_l3uc_src[m] && pdd_1 == 0) {
            ip_ctrl->data_sel[i] = IPDB2;
            pdd_1 = 1;
        }
    }


    if (pdd_0 == 0 || pdd_1 == 0) {
        LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META(
                        "No proper pdd type found.\n"
                    )));
        *rv = SHR_E_PARAM;
        return;
    }
}

static void
bcm56780_a0_cth_alpm_parallel_rpf_init(int u, int m,
                                       bcmptm_cth_alpm_control_t *ctrl,
                                       bcm56780_lpm_ip_control_t *ip_ctrl,
                                       uint32_t *pdd_types,
                                       int *rv)
{
    /*
     * In RPF & PARALLEL mode, we assume
     * 1. DB0 - LPM_DST Private - block 0
     * 2. DB1 - LPM_DST Public  - block 1
     * 3. DB2 - LPM_SRC Private - block 2
     * 4. DB3 - LPM_SRC Public  - block 3
     */

    uint8_t blocks[4];
    int ok = 0;

    bcm56780_a0_l2_parallel_rpf_block_dbs(u, ctrl, &blocks[0]);
    ip_ctrl->level2_database_sel_block[0] = blocks[0];
    ip_ctrl->level2_database_sel_block[1] = blocks[1];
    ip_ctrl->level2_database_sel_block[2] = blocks[2];
    ip_ctrl->level2_database_sel_block[3] = blocks[3];

    bcm56780_a0_l3_parallel_rpf_block_dbs(u, m, ctrl, &blocks[0], rv);
    ip_ctrl->level3_database_sel_block[0] = blocks[0];
    ip_ctrl->level3_database_sel_block[1] = blocks[1];
    ip_ctrl->level3_database_sel_block[2] = blocks[2];
    ip_ctrl->level3_database_sel_block[3] = blocks[3];

    ip_ctrl->destination[0] = ctrl->destination;
    ip_ctrl->destination_mask[0] = ctrl->destination_mask;
    ip_ctrl->destination_type_match[0] = ctrl->destination_type_match;
    ip_ctrl->destination_type_non_match[0] = ctrl->destination_type_non_match;

    ip_ctrl->destination[1] = ctrl->destination;
    ip_ctrl->destination_mask[1] = ctrl->destination_mask;
    ip_ctrl->destination_type_match[1] = ctrl->destination_type_match;
    ip_ctrl->destination_type_non_match[1] = ctrl->destination_type_non_match;

    bcm56780_a0_cth_alpm_parallel_rpf_check_and_set(u, m, ctrl, ctrl);

    /* Output DB sel configs */
    ip_ctrl->data_sel[0] = IPDBX;
    ip_ctrl->data_sel[1] = IPDBX;
    ip_ctrl->data_sel[2] = IPDBX;
    ip_ctrl->data_sel[3] = IPDBX;
    ip_ctrl->result_mode = TWO_RESULTS_MODE;

    if (pdd_types[0] == bcm56780_a0_pdd_type_l3uc_dst[m]) {
        ok = (pdd_types[1] == bcm56780_a0_pdd_type_l3uc_src[m]);
        ip_ctrl->data_sel[0] = IPDB0;
        ip_ctrl->data_sel[1] = IPDB1;
        ip_ctrl->data_sel[2] = IPDB2;
        ip_ctrl->data_sel[3] = IPDB3;
    } else if (pdd_types[0] == bcm56780_a0_pdd_type_l3uc_src[m]) {
        ok = (pdd_types[1] == bcm56780_a0_pdd_type_l3uc_dst[m]);
        ip_ctrl->data_sel[0] = IPDB2;
        ip_ctrl->data_sel[1] = IPDB3;
        ip_ctrl->data_sel[2] = IPDB0;
        ip_ctrl->data_sel[3] = IPDB1;
    }
    if (!ok) {
        LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META(
                        "No proper pdd type found.\n"
                    )));
        *rv = SHR_E_PARAM;
        return;
    }
}

static void
bcm56780_a0_cth_alpm_fp_no_lpm_init(int u, int m,
                                    bcmptm_cth_alpm_control_t *ctrl,
                                    bcm56780_lpm_ip_control_t *ip_ctrl,
                                    uint32_t *pdd_types,
                                    int *rv)
{
    /*
     * 1. DB2 - FP_COMP DST  - block 2
     * 2. DB3 - FP_COMP SRC  - block 3
     */

    uint8_t blocks[4];
    int pdd_0 = 0, pdd_1 = 0;
    int i;

    bcm56780_a0_l2_fp_no_lpm_block_dbs(u, ctrl, &blocks[0]);
    ip_ctrl->level2_database_sel_block[0] = blocks[0];
    ip_ctrl->level2_database_sel_block[1] = blocks[1];
    ip_ctrl->level2_database_sel_block[2] = blocks[2];
    ip_ctrl->level2_database_sel_block[3] = blocks[3];

    bcm56780_a0_l3_fp_no_lpm_block_dbs(u, m, ctrl, &blocks[0], rv);
    ip_ctrl->level3_database_sel_block[0] = blocks[0];
    ip_ctrl->level3_database_sel_block[1] = blocks[1];
    ip_ctrl->level3_database_sel_block[2] = blocks[2];
    ip_ctrl->level3_database_sel_block[3] = blocks[3];

    bcm56780_a0_cth_alpm_fp_combined_check_and_set(u, m, ctrl, ctrl);


    /* Output DB sel configs */
    ip_ctrl->data_sel[0] = IPDBX;
    ip_ctrl->data_sel[1] = IPDBX;
    ip_ctrl->data_sel[2] = IPDBX;
    ip_ctrl->data_sel[3] = IPDBX;
    ip_ctrl->result_mode = FOUR_RESULTS_MODE;

    for (i = 0; i < BCM56780_ALPM_L3_BLOCKS; i++) {
        if (pdd_types[i] == bcm56780_a0_pdd_type_comp_dst[m] && pdd_0 == 0) {
            ip_ctrl->data_sel[i] = IPDB2;
            pdd_0 = 1;
        }
        if (pdd_types[i] == bcm56780_a0_pdd_type_comp_src[m] && pdd_1 == 0) {
            ip_ctrl->data_sel[i] = IPDB3;
            pdd_1 = 1;
        }
    }

    if (pdd_0 == 0 || pdd_1 == 0) {
        LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META(
                        "No proper pdd type found.\n"
                    )));
        *rv = SHR_E_PARAM;
        return;
    }
}

static void
bcm56780_a0_cth_alpm_fp_combined_init(int u, int m,
                                      bcmptm_cth_alpm_control_t *ctrl,
                                      bcm56780_lpm_ip_control_t *ip_ctrl,
                                      uint32_t *pdd_types,
                                      int *rv)
{
    /*
     * 1. DB0 - LPM_DST      - block 0/1
     * 2. DB2 - FP_COMP DST  - block 2
     * 3. DB3 - FP_COMP SRC  - block 3
     */

    uint8_t blocks[4];
    int pdd_0 = 0, pdd_2 = 0, pdd_3 = 0;
    int i;

    bcm56780_a0_l2_fp_combined_block_dbs(u, ctrl, &blocks[0]);
    ip_ctrl->level2_database_sel_block[0] = blocks[0];
    ip_ctrl->level2_database_sel_block[1] = blocks[1];
    ip_ctrl->level2_database_sel_block[2] = blocks[2];
    ip_ctrl->level2_database_sel_block[3] = blocks[3];

    bcm56780_a0_l3_fp_combined_block_dbs(u, m, ctrl, &blocks[0], rv);
    ip_ctrl->level3_database_sel_block[0] = blocks[0];
    ip_ctrl->level3_database_sel_block[1] = blocks[1];
    ip_ctrl->level3_database_sel_block[2] = blocks[2];
    ip_ctrl->level3_database_sel_block[3] = blocks[3];

    ip_ctrl->destination[0] = ctrl->destination;
    ip_ctrl->destination_mask[0] = ctrl->destination_mask;
    ip_ctrl->destination_type_match[0] = ctrl->destination_type_match;
    ip_ctrl->destination_type_non_match[0] = ctrl->destination_type_non_match;

    bcm56780_a0_cth_alpm_fp_combined_check_and_set(u, m, ctrl, ctrl);

    /* Output DB sel configs */
    ip_ctrl->data_sel[0] = IPDBX;
    ip_ctrl->data_sel[1] = IPDBX;
    ip_ctrl->data_sel[2] = IPDBX;
    ip_ctrl->data_sel[3] = IPDBX;
    ip_ctrl->result_mode = FOUR_RESULTS_MODE;

    for (i = 0; i < BCM56780_ALPM_L3_BLOCKS; i++) {
        if (pdd_types[i] == bcm56780_a0_pdd_type_l3uc_dst[m] && pdd_0 == 0) {
            ip_ctrl->data_sel[i] = IPDB0;
            pdd_0 = 1;
        }
        if (pdd_types[i] == bcm56780_a0_pdd_type_comp_dst[m] && pdd_2 == 0) {
            ip_ctrl->data_sel[i] = IPDB2;
            pdd_2 = 1;
        }
        if (pdd_types[i] == bcm56780_a0_pdd_type_comp_src[m] && pdd_3 == 0) {
            ip_ctrl->data_sel[i] = IPDB3;
            pdd_3 = 1;
        }
    }

    if (pdd_0 == 0 || pdd_2 == 0 || pdd_3 == 0) {
        LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META(
                        "No proper pdd type found.\n"
                    )));
        *rv = SHR_E_PARAM;
        return;
    }

}

static void
bcm56780_a0_cth_alpm_fp_parallel_init(int u, int m,
                                      bcmptm_cth_alpm_control_t *ctrl,
                                      bcm56780_lpm_ip_control_t *ip_ctrl,
                                      uint32_t *pdd_types,
                                      int *rv)
{
    /*
     * 1. DB0 - LPM_DST 0    - block 0
     * 3. DB1 - LPM_DST 1    - block 1
     * 3. DB2 - FP_COMP DST  - block 2
     * 4. DB3 - FP_COMP SRC  - block 3
     */

    uint8_t blocks[4];
    int ok = 0;

    bcm56780_a0_l2_fp_parallel_block_dbs(u, ctrl, &blocks[0]);
    ip_ctrl->level2_database_sel_block[0] = blocks[0];
    ip_ctrl->level2_database_sel_block[1] = blocks[1];
    ip_ctrl->level2_database_sel_block[2] = blocks[2];
    ip_ctrl->level2_database_sel_block[3] = blocks[3];

    bcm56780_a0_l3_fp_parallel_block_dbs(u, m, ctrl, &blocks[0], rv);
    ip_ctrl->level3_database_sel_block[0] = blocks[0];
    ip_ctrl->level3_database_sel_block[1] = blocks[1];
    ip_ctrl->level3_database_sel_block[2] = blocks[2];
    ip_ctrl->level3_database_sel_block[3] = blocks[3];

    ip_ctrl->destination[0] = ctrl->destination;
    ip_ctrl->destination_mask[0] = ctrl->destination_mask;
    ip_ctrl->destination_type_match[0] = ctrl->destination_type_match;
    ip_ctrl->destination_type_non_match[0] = ctrl->destination_type_non_match;

    bcm56780_a0_cth_alpm_fp_parallel_check_and_set(u, m, ctrl, ctrl);

    /* Output DB sel configs */
    ip_ctrl->data_sel[0] = IPDB0;
    ip_ctrl->data_sel[1] = IPDB1;
    ip_ctrl->data_sel[2] = IPDB2;
    ip_ctrl->data_sel[3] = IPDB3;
    ip_ctrl->result_mode = THREE_RESULTS_MODE;
    ok = (pdd_types[0] == bcm56780_a0_pdd_type_l3uc_dst[m]) &&
         (pdd_types[2] == bcm56780_a0_pdd_type_comp_dst[m]) &&
         (pdd_types[3] == bcm56780_a0_pdd_type_comp_src[m]);
    if (!ok) {
        LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META(
                        "No proper pdd type found.\n"
                    )));
        *rv = SHR_E_PARAM;
        return;
    }
}

static void
bcm56780_a0_cth_alpm_fp_combined_rpf_init(int u, int m,
                                          bcmptm_cth_alpm_control_t *ctrl,
                                          bcm56780_lpm_ip_control_t *ip_ctrl,
                                          uint32_t *pdd_types,
                                          int *rv)
{

    /*
     * 1. DB0 - LPM_DST      - block 0
     * 3. DB1 - LPM_SRC      - block 1
     * 3. DB2 - FP_COMP DST  - block 2
     * 4. DB3 - FP_COMP SRC  - block 3
     */

    uint8_t blocks[4];
    int pdd_0 = 0, pdd_1 = 0, pdd_2 = 0, pdd_3 = 0;
    int i;

    bcm56780_a0_l2_fp_combined_rpf_block_dbs(u, ctrl, &blocks[0]);
    ip_ctrl->level2_database_sel_block[0] = blocks[0];
    ip_ctrl->level2_database_sel_block[1] = blocks[1];
    ip_ctrl->level2_database_sel_block[2] = blocks[2];
    ip_ctrl->level2_database_sel_block[3] = blocks[3];

    bcm56780_a0_l3_fp_combined_rpf_block_dbs(u, m, ctrl, &blocks[0], rv);
    ip_ctrl->level3_database_sel_block[0] = blocks[0];
    ip_ctrl->level3_database_sel_block[1] = blocks[1];
    ip_ctrl->level3_database_sel_block[2] = blocks[2];
    ip_ctrl->level3_database_sel_block[3] = blocks[3];

    ip_ctrl->destination[0] = ctrl->destination;
    ip_ctrl->destination_mask[0] = ctrl->destination_mask;
    ip_ctrl->destination_type_match[0] = ctrl->destination_type_match;
    ip_ctrl->destination_type_non_match[0] = ctrl->destination_type_non_match;

    ip_ctrl->destination[1] = ctrl->destination;
    ip_ctrl->destination_mask[1] = ctrl->destination_mask;
    ip_ctrl->destination_type_match[1] = ctrl->destination_type_match;
    ip_ctrl->destination_type_non_match[1] = ctrl->destination_type_non_match;

    bcm56780_a0_cth_alpm_fp_combined_rpf_check_and_set(u, m, ctrl, ctrl);

    /* Output DB sel configs */
    ip_ctrl->data_sel[0] = IPDBX;
    ip_ctrl->data_sel[1] = IPDBX;
    ip_ctrl->data_sel[2] = IPDBX;
    ip_ctrl->data_sel[3] = IPDBX;
    ip_ctrl->result_mode = FOUR_RESULTS_MODE;

    for (i = 0; i < BCM56780_ALPM_L3_BLOCKS; i++) {
        if (pdd_types[i] == bcm56780_a0_pdd_type_l3uc_dst[m] && pdd_0 == 0) {
            ip_ctrl->data_sel[i] = IPDB0;
            pdd_0 = 1;
        }
        if (pdd_types[i] == bcm56780_a0_pdd_type_l3uc_src[m] && pdd_1 == 0) {
            ip_ctrl->data_sel[i] = IPDB1;
            pdd_1 = 1;
        }
        if (pdd_types[i] == bcm56780_a0_pdd_type_comp_dst[m] && pdd_2 == 0) {
            ip_ctrl->data_sel[i] = IPDB2;
            pdd_2 = 1;
        }
        if (pdd_types[i] == bcm56780_a0_pdd_type_comp_src[m] && pdd_3 == 0) {
            ip_ctrl->data_sel[i] = IPDB3;
            pdd_3 = 1;
        }
    }

    if (pdd_0 == 0 || pdd_1 == 0 || pdd_2 == 0 || pdd_3 == 0) {
        LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META(
                        "No proper pdd type found.\n"
                    )));
        *rv = SHR_E_PARAM;
        return;
    }

}

static int
bcm56780_a0_cth_alpm_flex_ki_map(int u, int m, int l1_phy_key_input)
{

    uint32_t key_muxes[BCM56780_ALPM_L1_KEY_MUXES];
    int map_ki = 0;
    int find, i;

    switch (l1_phy_key_input) {
    case KEY_INPUT_LPM_DST_Q: map_ki= 0; break;
    case KEY_INPUT_LPM_DST_D: map_ki= 1; break;
    case KEY_INPUT_LPM_DST_S: map_ki= 2; break;
    case KEY_INPUT_LPM_SRC_Q: map_ki= 0; break;
    case KEY_INPUT_LPM_SRC_D: map_ki= 1; break;
    case KEY_INPUT_LPM_SRC_S: map_ki= 2; break;
    case KEY_INPUT_FP_COMP_DST: map_ki= 0; break;
    case KEY_INPUT_FP_COMP_SRC: map_ki= 0; break;
    default: break;
    }

    find = 0;
    bcm56780_a0_cth_alpm_block_key_muxes(u, m, BCM56780_ALPM_L1_KEY_MUXES, &key_muxes[0]);
    for (i = 0; i < BCM56780_ALPM_L1_KEY_MUXES; i++) {
        switch (key_muxes[i]) {
        case EM_TILE_ALPM_KEY_MUX_TYPE_L3UC_DST:
        case EM_TILE_ALPM_KEY_MUX_TYPE_L3UC_DST_MTOP:
            if (l1_phy_key_input == KEY_INPUT_LPM_DST_Q ||
                l1_phy_key_input == KEY_INPUT_LPM_DST_D ||
                l1_phy_key_input == KEY_INPUT_LPM_DST_S) {
                find = 1;
            }
            break;

        case EM_TILE_ALPM_KEY_MUX_TYPE_L3UC_SRC:
        case EM_TILE_ALPM_KEY_MUX_TYPE_L3UC_SRC_MTOP:
            if (l1_phy_key_input == KEY_INPUT_LPM_SRC_Q ||
                l1_phy_key_input == KEY_INPUT_LPM_SRC_D ||
                l1_phy_key_input == KEY_INPUT_LPM_SRC_S) {
                find = 1;
            }
            break;

        case EM_TILE_ALPM_KEY_MUX_TYPE_COMP_DST:
        case EM_TILE_ALPM_KEY_MUX_TYPE_COMP_DST_MTOP:
            if (l1_phy_key_input == KEY_INPUT_FP_COMP_DST) {
                find = 1;
            }
            break;

        case EM_TILE_ALPM_KEY_MUX_TYPE_COMP_SRC:
        case EM_TILE_ALPM_KEY_MUX_TYPE_COMP_SRC_MTOP:
            if (l1_phy_key_input == KEY_INPUT_FP_COMP_SRC) {
                find = 1;
            }
            break;
        default:
            break;
        }
        if (find) {
            break;
        }
    }
    if (find) {
        map_ki += (3 * i);
    } else {
        map_ki = 0; /* shouldn't be here. */
    }

    return map_ki;
}

/*!
 * \brief LPM IP control for parallel mode
 *
 * Two databases:
 *  - DB0 includes routes of VRF type.
 *  - DB1 includes routes of Global type.
 *
 * \param [in] u Device u.
 * \param [in] config Config input
 * \param [out] ctrl LPM ip control
 *
 * \return nothing.
 */
static void
bcm56780_a0_cth_alpm_ip_ctrl_init(int u, int m,
                                  bcmptm_cth_alpm_control_t *ctrl,
                                  bcm56780_lpm_ip_control_t *ip_ctrl,
                                  int *rv)
{
    const bcmptm_alpm_variant_info_t *info;
    int bank_size, bits;
    int i, j;
    int lpm_ipv4_key_type;
    int lpm_ipv6_key_type;
    int comp_ipv4_key_type;
    int comp_ipv6_key_type;
    uint32_t pdd_types[BCM56780_ALPM_L3_BLOCKS];
    /*
     * All supported settings.
     *
     * - Lane 0 Combined
     *  +-----+---------+
     *  | DB0 | LPM DST |
     *  +-----+---------+
     *
     * - Lane 0 Parallel
     *  +-----+---------+
     *  | DB0 | LPM DST |
     *  | DB1 | LPM DST |
     *  +-----+---------+
     *
     * - Lane 0/1 Combined + RPF
     *  +-----+---------+
     *  | DB0 | LPM DST |
     *  +-----+---------+
     *  | DB2 | LPM SRC |
     *  +-----+---------+
     *
     * - Lane 0/1 Parallel + RPF
     *  +-----+---------+
     *  | DB0 | LPM DST |
     *  | DB1 | LPM DST |
     *  +-----+---------+
     *  | DB2 | LPM SRC |
     *  | DB3 | LPM SRC |
     *  +-----+---------+
     *
     * - Lane 0/2/3 Combined + FP
     *  +-----+---------+
     *  | DB0 | LPM DST |
     *  +-----+---------+
     *  | DB2 | FP DST  |
     *  +-----+---------+
     *  | DB3 | FP SRC  |
     *  +-----+---------+
     *
     * - Lane 0/2/3 Parallel + FP
     *  +-----+---------+
     *  | DB0 | LPM DST |
     *  | DB1 | LPM DST |
     *  +-----+---------+
     *  | DB2 | FP DST  |
     *  +-----+---------+
     *  | DB3 | FP SRC  |
     *  +-----+---------+
     *
     * - Lane 0/1/2/3 Combined + RPF + FP
     *  +-----+---------+
     *  | DB0 | LPM DST |
     *  +-----+---------+
     *  | DB1 | LPM SRC |
     *  +-----+---------+
     *  | DB2 | FP DST  |
     *  +-----+---------+
     *  | DB3 | FP SRC  |
     *  +-----+---------+
 */
    bcm56780_a0_cth_alpm_l1_parse_check(u, m, ctrl->tot.num_l1_banks,
                                        &ctrl->l1_key_input[0],
                                        &ctrl->l1_db[0],
                                        ctrl->db0_levels,
                                        ctrl->db1_levels,
                                        ctrl->db2_levels,
                                        ctrl->db3_levels,
                                        &ctrl->reason,
                                        &ctrl->alpm_mode,
                                        &ctrl->alpm_dbs);

    /* Level-1 key-input sel block */
    ip_ctrl->level1_key_input_sel_block[0] =
        bcm56780_a0_cth_alpm_flex_ki_map(u, m, ctrl->l1_phy_key_input[0]);
    ip_ctrl->level1_key_input_sel_block[1] =
        bcm56780_a0_cth_alpm_flex_ki_map(u, m, ctrl->l1_phy_key_input[1]);
    ip_ctrl->level1_key_input_sel_block[2] =
        bcm56780_a0_cth_alpm_flex_ki_map(u, m, ctrl->l1_phy_key_input[2]);
    ip_ctrl->level1_key_input_sel_block[3] =
        bcm56780_a0_cth_alpm_flex_ki_map(u, m, ctrl->l1_phy_key_input[3]);

    if (m == ALPM_1) {
        ip_ctrl->level1_key_input_sel_block[4] =
            bcm56780_a0_cth_alpm_flex_ki_map(u, m, ctrl->l1_phy_key_input[4]);
        ip_ctrl->level1_key_input_sel_block[5] =
            bcm56780_a0_cth_alpm_flex_ki_map(u, m, ctrl->l1_phy_key_input[5]);
        ip_ctrl->level1_key_input_sel_block[6] =
            bcm56780_a0_cth_alpm_flex_ki_map(u, m, ctrl->l1_phy_key_input[6]);
        ip_ctrl->level1_key_input_sel_block[7] =
            bcm56780_a0_cth_alpm_flex_ki_map(u, m, ctrl->l1_phy_key_input[7]);
    }

    /* Level-1 database sel block */
    ip_ctrl->level1_database_sel_block[0] = ctrl->l1_phy_db[0];
    ip_ctrl->level1_database_sel_block[1] = ctrl->l1_phy_db[1];
    ip_ctrl->level1_database_sel_block[2] = ctrl->l1_phy_db[2];
    ip_ctrl->level1_database_sel_block[3] = ctrl->l1_phy_db[3];

    if (m == ALPM_1) {
        ip_ctrl->level1_database_sel_block[4] = ctrl->l1_phy_db[4];
        ip_ctrl->level1_database_sel_block[5] = ctrl->l1_phy_db[5];
        ip_ctrl->level1_database_sel_block[6] = ctrl->l1_phy_db[6];
        ip_ctrl->level1_database_sel_block[7] = ctrl->l1_phy_db[7];
    }

    *rv = bcm56780_a0_cth_alpm_block_pdd_types(u, m, BCM56780_ALPM_L3_BLOCKS, &pdd_types[0]);
    if (SHR_FAILURE(*rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META(
                        "Fetch PDD types fails.\n"
                    )));
        return;
    }

    if (ctrl->alpm_mode & ALPM_MODE_FP_COMP) {
        if (ctrl->alpm_mode & ALPM_MODE_NO_LPM) {
            bcm56780_a0_cth_alpm_fp_no_lpm_init(u, m, ctrl, ip_ctrl, &pdd_types[0], rv);
        } else if (ctrl->alpm_mode & ALPM_MODE_RPF) {
            bcm56780_a0_cth_alpm_fp_combined_rpf_init(u, m, ctrl, ip_ctrl, &pdd_types[0], rv);
        } else if (ctrl->alpm_mode & ALPM_MODE_PARALLEL) {
            bcm56780_a0_cth_alpm_fp_parallel_init(u, m, ctrl, ip_ctrl, &pdd_types[0], rv);
        } else {
            bcm56780_a0_cth_alpm_fp_combined_init(u, m, ctrl, ip_ctrl, &pdd_types[0], rv);
        }
    } else {
        if (ctrl->alpm_mode & ALPM_MODE_RPF) {
            if (ctrl->alpm_mode & ALPM_MODE_PARALLEL) {
                bcm56780_a0_cth_alpm_parallel_rpf_init(u, m, ctrl, ip_ctrl, &pdd_types[0], rv);
            } else {
                bcm56780_a0_cth_alpm_combined_rpf_init(u, m, ctrl, ip_ctrl, &pdd_types[0], rv);
            }
        } else {
            if (ctrl->alpm_mode & ALPM_MODE_PARALLEL) {
                bcm56780_a0_cth_alpm_parallel_init(u, m, ctrl, ip_ctrl, &pdd_types[0], rv);
            } else {
                bcm56780_a0_cth_alpm_combined_init(u, m, ctrl, ip_ctrl, &pdd_types[0], rv);
            }
        }
    }

    /* Level-2/3 key-input sel block */
    for (i = 0; i < COUNTOF(ip_ctrl->level2_database_sel_block); i++) {
        for (j = 0; j < BCM56780_ALPM_L1_BLOCKS(m); j++) {
            if (ip_ctrl->level2_database_sel_block[i] ==
                ip_ctrl->level1_database_sel_block[j]) {
                ip_ctrl->level2_key_input_sel_block[i] =
                    ip_ctrl->level1_key_input_sel_block[j] / 3 * 3;
                break;
            }
        }
        for (j = 0; j < BCM56780_ALPM_L1_BLOCKS(m); j++) {
            if (ip_ctrl->level3_database_sel_block[i] ==
                ip_ctrl->level1_database_sel_block[j]) {
                ip_ctrl->level3_key_input_sel_block[i] =
                    ip_ctrl->level1_key_input_sel_block[j] / 3 * 3;
                break;
            }
        }
    }

    assert(ctrl->tot.num_l1_banks || ctrl->alpm_dbs == 0);
    if (SHR_FAILURE(*rv) && ctrl->alpm_dbs) {
        /*
         * If no l1 banks assigned, the ip_ctrl init error for various modes
         * above does not count as a true error.
         */
        LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META(
                        "IP ctrl init for various modes fails.\n"
                    )));
        return;
    }
    *rv = SHR_E_NONE;


    /* Force to fixed values since it's hard connected in hardware */
    ip_ctrl->level2_bank_config_block[0] = ctrl->tot.l2_bank_bitmap & 0x3;
    ip_ctrl->level2_bank_config_block[1] = ctrl->tot.l2_bank_bitmap & 0x4;
    ip_ctrl->level2_bank_config_block[2] = ctrl->tot.l2_bank_bitmap & 0x18;
    ip_ctrl->level2_bank_config_block[3] = ctrl->tot.l2_bank_bitmap & 0x20;
    ip_ctrl->level3_bank_config_block[0] = ctrl->tot.l3_bank_bitmap & 0x3;
    ip_ctrl->level3_bank_config_block[1] = ctrl->tot.l3_bank_bitmap & 0xc;
    ip_ctrl->level3_bank_config_block[2] = ctrl->tot.l3_bank_bitmap & 0x30;
    ip_ctrl->level3_bank_config_block[3] = ctrl->tot.l3_bank_bitmap & 0xc0;

    /* DB_MODE configs */
    ip_ctrl->db_mod[0] = ctrl->db0_levels;
    ip_ctrl->db_mod[1] = ctrl->db1_levels;
    ip_ctrl->db_mod[2] = ctrl->db2_levels;;
    ip_ctrl->db_mod[3] = ctrl->db3_levels;;

    /* Fill bucket bits */
    if (m == ALPM_0) {
        bank_size = bcmdrd_pt_index_count(u, L3_DEFIP_ALPM_LEVEL2_B0m);
    } else {
        bank_size = bcmdrd_pt_index_count(u, M_DEFIP_ALPM_LEVEL2_B0m);
    }
    bits = shr_util_log2(bank_size);

    ctrl->tot.l2_bucket_bits = bits;
    ctrl->db0.l2_bucket_bits = ctrl->db0_levels ? bits : 0;
    ctrl->db1.l2_bucket_bits = ctrl->db1_levels ? bits : 0;
    ctrl->db2.l2_bucket_bits = ctrl->db2_levels ? bits : 0;
    ctrl->db3.l2_bucket_bits = ctrl->db3_levels ? bits : 0;

    if (m == ALPM_0) {
        bank_size = bcmdrd_pt_index_count(u, L3_DEFIP_ALPM_LEVEL3_B0m);
    } else {
        bank_size = bcmdrd_pt_index_count(u, M_DEFIP_ALPM_LEVEL3_B0m);
    }
    bits = shr_util_log2(bank_size);
    ctrl->tot.l3_bucket_bits = bits;
    ctrl->db0.l3_bucket_bits = ctrl->db0_levels ? bits : 0;
    ctrl->db1.l3_bucket_bits = ctrl->db1_levels ? bits : 0;
    ctrl->db2.l3_bucket_bits = ctrl->db2_levels ? bits : 0;
    ctrl->db3.l3_bucket_bits = ctrl->db3_levels ? bits : 0;

    info = bcmptm_rm_alpm_variant_get(u, m);
    if (info == NULL) {
        return;
    }
    if (m == ALPM_0) {
        lpm_ipv4_key_type = info->main_key_type->lpm_ipv4;
        lpm_ipv6_key_type = info->main_key_type->lpm_ipv6;
        comp_ipv4_key_type = info->main_key_type->comp_ipv4;
        comp_ipv6_key_type = info->main_key_type->comp_ipv6;
    } else {
        lpm_ipv4_key_type = info->mtop_key_type->lpm_ipv4;
        lpm_ipv6_key_type = info->mtop_key_type->lpm_ipv6;
        comp_ipv4_key_type = info->mtop_key_type->comp_ipv4;
        comp_ipv6_key_type = info->mtop_key_type->comp_ipv6;
    }
    ip_ctrl->lane0_key_type_l1_packing_set[lpm_ipv4_key_type] = L1_PKSET_HHH;
    ip_ctrl->lane1_key_type_l1_packing_set[lpm_ipv4_key_type] = L1_PKSET_HHH;
    ip_ctrl->lane2_key_type_l1_packing_set[lpm_ipv4_key_type] = L1_PKSET_HHH;
    ip_ctrl->lane3_key_type_l1_packing_set[lpm_ipv4_key_type] = L1_PKSET_HHH;

    ip_ctrl->lane0_key_type_l1_packing_set[lpm_ipv6_key_type] = L1_PKSET_DSH;
    ip_ctrl->lane1_key_type_l1_packing_set[lpm_ipv6_key_type] = L1_PKSET_DSH;
    ip_ctrl->lane2_key_type_l1_packing_set[lpm_ipv6_key_type] = L1_PKSET_DSH;
    ip_ctrl->lane3_key_type_l1_packing_set[lpm_ipv6_key_type] = L1_PKSET_DSH;

    ip_ctrl->lane0_key_type_l1_packing_set[comp_ipv4_key_type] = L1_PKSET_SSH;
    ip_ctrl->lane1_key_type_l1_packing_set[comp_ipv4_key_type] = L1_PKSET_SSH;
    ip_ctrl->lane2_key_type_l1_packing_set[comp_ipv4_key_type] = L1_PKSET_SSH;
    ip_ctrl->lane3_key_type_l1_packing_set[comp_ipv4_key_type] = L1_PKSET_SSH;

    ip_ctrl->lane0_key_type_l1_packing_set[comp_ipv6_key_type] = L1_PKSET_DSH;
    ip_ctrl->lane1_key_type_l1_packing_set[comp_ipv6_key_type] = L1_PKSET_DSH;
    ip_ctrl->lane2_key_type_l1_packing_set[comp_ipv6_key_type] = L1_PKSET_DSH;
    ip_ctrl->lane3_key_type_l1_packing_set[comp_ipv6_key_type] = L1_PKSET_DSH;

    ip_ctrl->lane0_key_type_ln_packing_set[lpm_ipv4_key_type] = LN_PKSET_16_49;
    ip_ctrl->lane1_key_type_ln_packing_set[lpm_ipv4_key_type] = LN_PKSET_16_49;
    ip_ctrl->lane2_key_type_ln_packing_set[lpm_ipv4_key_type] = LN_PKSET_16_49;
    ip_ctrl->lane3_key_type_ln_packing_set[lpm_ipv4_key_type] = LN_PKSET_16_49;

    ip_ctrl->lane0_key_type_ln_packing_set[lpm_ipv6_key_type] = LN_PKSET_16_49;
    ip_ctrl->lane1_key_type_ln_packing_set[lpm_ipv6_key_type] = LN_PKSET_16_49;
    ip_ctrl->lane2_key_type_ln_packing_set[lpm_ipv6_key_type] = LN_PKSET_16_49;
    ip_ctrl->lane3_key_type_ln_packing_set[lpm_ipv6_key_type] = LN_PKSET_16_49;

    ip_ctrl->lane0_key_type_ln_packing_set[comp_ipv4_key_type] = LN_PKSET_16_49;
    ip_ctrl->lane1_key_type_ln_packing_set[comp_ipv4_key_type] = LN_PKSET_16_49;
    ip_ctrl->lane2_key_type_ln_packing_set[comp_ipv4_key_type] = LN_PKSET_16_49;
    ip_ctrl->lane3_key_type_ln_packing_set[comp_ipv4_key_type] = LN_PKSET_16_49;

    ip_ctrl->lane0_key_type_ln_packing_set[comp_ipv6_key_type] = LN_PKSET_16_49;
    ip_ctrl->lane1_key_type_ln_packing_set[comp_ipv6_key_type] = LN_PKSET_16_49;
    ip_ctrl->lane2_key_type_ln_packing_set[comp_ipv6_key_type] = LN_PKSET_16_49;
    ip_ctrl->lane3_key_type_ln_packing_set[comp_ipv6_key_type] = LN_PKSET_16_49;

}

static int
bcm56780_a0_cth_alpm_control_mtop_encode(int u, int m,
                                         bcmptm_cth_alpm_control_t *ctrl,
                                         cth_alpm_control_pt_t *pts)
{
    M_LPM_IP_CONTROLm_t ce;
    M_E2T_00_SHARED_BANKS_CONTROLm_t sbc0;
    UFT_PDD_LANE_MUX_CONTROLr_t pdd_lane;
    MTOP_MUX_P0_CONTROLr_t mtop_control;
    bcm56780_lpm_ip_control_t lpm_ctrl;
    uint32_t pdd_types[BCM56780_ALPM_L3_BLOCKS] = {EM_TILE_ALPM_PDD_TYPE_NONE};
    uint32_t pdd_lane_bmp = 0;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(u);
    SHR_NULL_CHECK(pts, SHR_E_PARAM);
    SHR_NULL_CHECK(ctrl, SHR_E_PARAM);

    pts->count = 4;

    /* If entry/sid pointer is null, simply return count. */
    if (!pts->entry || !pts->sid) {
        SHR_EXIT();
    }

    sal_memcpy(&ce, pts->entry[0], sizeof(ce));
    sal_memcpy(&sbc0, pts->entry[1], sizeof(sbc0));
    sal_memcpy(&pdd_lane, pts->entry[2], sizeof(pdd_lane));
    sal_memcpy(&mtop_control, pts->entry[3], sizeof(mtop_control));
    sal_memset(&lpm_ctrl, 0, sizeof(lpm_ctrl));

    /* Level-1 Configurations */
    bcm56780_a0_cth_alpm_ip_ctrl_init(u, m, ctrl, &lpm_ctrl, &rv);
    SHR_IF_ERR_EXIT(rv); /* not expect to fail, as we did dry-run. */

    M_CONTROL_FIELD_SET(DRIVE_HT_LPM_HIT_INDEX_ENTRY_TYPEf, 1);

    /* Level-1 Packing Set */
    M_CONTROL_FIELD_SET(LANE_0_KEY_TYPE_0_L1_PACKING_SETf,
                      lpm_ctrl.lane0_key_type_l1_packing_set[0]);
    M_CONTROL_FIELD_SET(LANE_0_KEY_TYPE_1_L1_PACKING_SETf,
                      lpm_ctrl.lane0_key_type_l1_packing_set[1]);
    M_CONTROL_FIELD_SET(LANE_0_KEY_TYPE_2_L1_PACKING_SETf,
                      lpm_ctrl.lane0_key_type_l1_packing_set[2]);
    M_CONTROL_FIELD_SET(LANE_0_KEY_TYPE_3_L1_PACKING_SETf,
                      lpm_ctrl.lane0_key_type_l1_packing_set[3]);


    M_CONTROL_FIELD_SET(LANE_1_KEY_TYPE_0_L1_PACKING_SETf,
                      lpm_ctrl.lane1_key_type_l1_packing_set[0]);
    M_CONTROL_FIELD_SET(LANE_1_KEY_TYPE_1_L1_PACKING_SETf,
                      lpm_ctrl.lane1_key_type_l1_packing_set[1]);
    M_CONTROL_FIELD_SET(LANE_1_KEY_TYPE_2_L1_PACKING_SETf,
                      lpm_ctrl.lane1_key_type_l1_packing_set[2]);
    M_CONTROL_FIELD_SET(LANE_1_KEY_TYPE_3_L1_PACKING_SETf,
                      lpm_ctrl.lane1_key_type_l1_packing_set[3]);


    M_CONTROL_FIELD_SET(LANE_2_KEY_TYPE_0_L1_PACKING_SETf,
                      lpm_ctrl.lane2_key_type_l1_packing_set[0]);
    M_CONTROL_FIELD_SET(LANE_2_KEY_TYPE_1_L1_PACKING_SETf,
                      lpm_ctrl.lane2_key_type_l1_packing_set[1]);
    M_CONTROL_FIELD_SET(LANE_2_KEY_TYPE_2_L1_PACKING_SETf,
                      lpm_ctrl.lane2_key_type_l1_packing_set[2]);
    M_CONTROL_FIELD_SET(LANE_2_KEY_TYPE_3_L1_PACKING_SETf,
                      lpm_ctrl.lane2_key_type_l1_packing_set[3]);

    M_CONTROL_FIELD_SET(LANE_3_KEY_TYPE_0_L1_PACKING_SETf,
                      lpm_ctrl.lane3_key_type_l1_packing_set[0]);
    M_CONTROL_FIELD_SET(LANE_3_KEY_TYPE_1_L1_PACKING_SETf,
                      lpm_ctrl.lane3_key_type_l1_packing_set[1]);
    M_CONTROL_FIELD_SET(LANE_3_KEY_TYPE_2_L1_PACKING_SETf,
                      lpm_ctrl.lane3_key_type_l1_packing_set[2]);
    M_CONTROL_FIELD_SET(LANE_3_KEY_TYPE_3_L1_PACKING_SETf,
                      lpm_ctrl.lane3_key_type_l1_packing_set[3]);


    /* Level-23 Packing Set */
    M_CONTROL_FIELD_SET(LANE_0_KEY_TYPE_0_L2L3_PACKING_SETf,
                      lpm_ctrl.lane0_key_type_ln_packing_set[0]);
    M_CONTROL_FIELD_SET(LANE_0_KEY_TYPE_1_L2L3_PACKING_SETf,
                      lpm_ctrl.lane0_key_type_ln_packing_set[1]);
    M_CONTROL_FIELD_SET(LANE_0_KEY_TYPE_2_L2L3_PACKING_SETf,
                      lpm_ctrl.lane0_key_type_ln_packing_set[2]);
    M_CONTROL_FIELD_SET(LANE_0_KEY_TYPE_3_L2L3_PACKING_SETf,
                      lpm_ctrl.lane0_key_type_ln_packing_set[3]);


    M_CONTROL_FIELD_SET(LANE_1_KEY_TYPE_0_L2L3_PACKING_SETf,
                      lpm_ctrl.lane1_key_type_ln_packing_set[0]);
    M_CONTROL_FIELD_SET(LANE_1_KEY_TYPE_1_L2L3_PACKING_SETf,
                      lpm_ctrl.lane1_key_type_ln_packing_set[1]);
    M_CONTROL_FIELD_SET(LANE_1_KEY_TYPE_2_L2L3_PACKING_SETf,
                      lpm_ctrl.lane1_key_type_ln_packing_set[2]);
    M_CONTROL_FIELD_SET(LANE_1_KEY_TYPE_3_L2L3_PACKING_SETf,
                      lpm_ctrl.lane1_key_type_ln_packing_set[3]);


    M_CONTROL_FIELD_SET(LANE_2_KEY_TYPE_0_L2L3_PACKING_SETf,
                      lpm_ctrl.lane2_key_type_ln_packing_set[0]);
    M_CONTROL_FIELD_SET(LANE_2_KEY_TYPE_1_L2L3_PACKING_SETf,
                      lpm_ctrl.lane2_key_type_ln_packing_set[1]);
    M_CONTROL_FIELD_SET(LANE_2_KEY_TYPE_2_L2L3_PACKING_SETf,
                      lpm_ctrl.lane2_key_type_ln_packing_set[2]);
    M_CONTROL_FIELD_SET(LANE_2_KEY_TYPE_3_L2L3_PACKING_SETf,
                      lpm_ctrl.lane2_key_type_ln_packing_set[3]);

    M_CONTROL_FIELD_SET(LANE_3_KEY_TYPE_0_L2L3_PACKING_SETf,
                      lpm_ctrl.lane3_key_type_ln_packing_set[0]);
    M_CONTROL_FIELD_SET(LANE_3_KEY_TYPE_1_L2L3_PACKING_SETf,
                      lpm_ctrl.lane3_key_type_ln_packing_set[1]);
    M_CONTROL_FIELD_SET(LANE_3_KEY_TYPE_2_L2L3_PACKING_SETf,
                      lpm_ctrl.lane3_key_type_ln_packing_set[2]);
    M_CONTROL_FIELD_SET(LANE_3_KEY_TYPE_3_L2L3_PACKING_SETf,
                      lpm_ctrl.lane3_key_type_ln_packing_set[3]);


    /* Level-1 Configurations */
    M_CONTROL_FIELD_SET(LEVEL1_DATABASE_SEL_BLOCK_0f,
                      lpm_ctrl.level1_database_sel_block[0]);
    M_CONTROL_FIELD_SET(LEVEL1_DATABASE_SEL_BLOCK_1f,
                      lpm_ctrl.level1_database_sel_block[1]);
    M_CONTROL_FIELD_SET(LEVEL1_DATABASE_SEL_BLOCK_2f,
                      lpm_ctrl.level1_database_sel_block[2]);
    M_CONTROL_FIELD_SET(LEVEL1_DATABASE_SEL_BLOCK_3f,
                      lpm_ctrl.level1_database_sel_block[3]);
    M_CONTROL_FIELD_SET(LEVEL1_DATABASE_SEL_BLOCK_4f,
                      lpm_ctrl.level1_database_sel_block[4]);
    M_CONTROL_FIELD_SET(LEVEL1_DATABASE_SEL_BLOCK_5f,
                      lpm_ctrl.level1_database_sel_block[5]);
    M_CONTROL_FIELD_SET(LEVEL1_DATABASE_SEL_BLOCK_6f,
                      lpm_ctrl.level1_database_sel_block[6]);
    M_CONTROL_FIELD_SET(LEVEL1_DATABASE_SEL_BLOCK_7f,
                      lpm_ctrl.level1_database_sel_block[7]);


    M_CONTROL_FIELD_SET(LEVEL1_KEY_INPUT_SEL_BLOCK_0f,
                      lpm_ctrl.level1_key_input_sel_block[0]);
    M_CONTROL_FIELD_SET(LEVEL1_KEY_INPUT_SEL_BLOCK_1f,
                      lpm_ctrl.level1_key_input_sel_block[1]);
    M_CONTROL_FIELD_SET(LEVEL1_KEY_INPUT_SEL_BLOCK_2f,
                      lpm_ctrl.level1_key_input_sel_block[2]);
    M_CONTROL_FIELD_SET(LEVEL1_KEY_INPUT_SEL_BLOCK_3f,
                      lpm_ctrl.level1_key_input_sel_block[3]);
    M_CONTROL_FIELD_SET(LEVEL1_KEY_INPUT_SEL_BLOCK_4f,
                      lpm_ctrl.level1_key_input_sel_block[4]);
    M_CONTROL_FIELD_SET(LEVEL1_KEY_INPUT_SEL_BLOCK_5f,
                      lpm_ctrl.level1_key_input_sel_block[5]);
    M_CONTROL_FIELD_SET(LEVEL1_KEY_INPUT_SEL_BLOCK_6f,
                      lpm_ctrl.level1_key_input_sel_block[6]);
    M_CONTROL_FIELD_SET(LEVEL1_KEY_INPUT_SEL_BLOCK_7f,
                      lpm_ctrl.level1_key_input_sel_block[7]);

    /* Level-2 Configurations */
    M_CONTROL_FIELD_SET(LEVEL2_DATABASE_SEL_BLOCK_0f,
                      lpm_ctrl.level2_database_sel_block[0]);
    M_CONTROL_FIELD_SET(LEVEL2_DATABASE_SEL_BLOCK_1f,
                      lpm_ctrl.level2_database_sel_block[1]);
    M_CONTROL_FIELD_SET(LEVEL2_DATABASE_SEL_BLOCK_2f,
                      lpm_ctrl.level2_database_sel_block[2]);
    M_CONTROL_FIELD_SET(LEVEL2_DATABASE_SEL_BLOCK_3f,
                      lpm_ctrl.level2_database_sel_block[3]);

    M_CONTROL_FIELD_SET(LEVEL2_KEY_INPUT_SEL_BLOCK_0f,
                      lpm_ctrl.level2_key_input_sel_block[0]);
    M_CONTROL_FIELD_SET(LEVEL2_KEY_INPUT_SEL_BLOCK_1f,
                      lpm_ctrl.level2_key_input_sel_block[1]);
    M_CONTROL_FIELD_SET(LEVEL2_KEY_INPUT_SEL_BLOCK_2f,
                      lpm_ctrl.level2_key_input_sel_block[2]);
    M_CONTROL_FIELD_SET(LEVEL2_KEY_INPUT_SEL_BLOCK_3f,
                      lpm_ctrl.level2_key_input_sel_block[3]);

    M_CONTROL_FIELD_SET(LEVEL2_BANK_CONFIG_BLOCK_0f,
                      lpm_ctrl.level2_bank_config_block[0]);
    M_CONTROL_FIELD_SET(LEVEL2_BANK_CONFIG_BLOCK_1f,
                      lpm_ctrl.level2_bank_config_block[1]);
    M_CONTROL_FIELD_SET(LEVEL2_BANK_CONFIG_BLOCK_2f,
                      lpm_ctrl.level2_bank_config_block[2]);
    M_CONTROL_FIELD_SET(LEVEL2_BANK_CONFIG_BLOCK_3f,
                      lpm_ctrl.level2_bank_config_block[3]);

    /* Level-3 Configurations */
    M_CONTROL_FIELD_SET(LEVEL3_DATABASE_SEL_BLOCK_0f,
                      lpm_ctrl.level3_database_sel_block[0]);
    M_CONTROL_FIELD_SET(LEVEL3_DATABASE_SEL_BLOCK_1f,
                      lpm_ctrl.level3_database_sel_block[1]);
    M_CONTROL_FIELD_SET(LEVEL3_DATABASE_SEL_BLOCK_2f,
                      lpm_ctrl.level3_database_sel_block[2]);
    M_CONTROL_FIELD_SET(LEVEL3_DATABASE_SEL_BLOCK_3f,
                      lpm_ctrl.level3_database_sel_block[3]);

    M_CONTROL_FIELD_SET(LEVEL3_KEY_INPUT_SEL_BLOCK_0f,
                      lpm_ctrl.level3_key_input_sel_block[0]);
    M_CONTROL_FIELD_SET(LEVEL3_KEY_INPUT_SEL_BLOCK_1f,
                      lpm_ctrl.level3_key_input_sel_block[1]);
    M_CONTROL_FIELD_SET(LEVEL3_KEY_INPUT_SEL_BLOCK_2f,
                      lpm_ctrl.level3_key_input_sel_block[2]);
    M_CONTROL_FIELD_SET(LEVEL3_KEY_INPUT_SEL_BLOCK_3f,
                      lpm_ctrl.level3_key_input_sel_block[3]);

    M_CONTROL_FIELD_SET(LEVEL3_BANK_CONFIG_BLOCK_0f,
                      lpm_ctrl.level3_bank_config_block[0]);
    M_CONTROL_FIELD_SET(LEVEL3_BANK_CONFIG_BLOCK_1f,
                      lpm_ctrl.level3_bank_config_block[1]);
    M_CONTROL_FIELD_SET(LEVEL3_BANK_CONFIG_BLOCK_2f,
                      lpm_ctrl.level3_bank_config_block[2]);
    M_CONTROL_FIELD_SET(LEVEL3_BANK_CONFIG_BLOCK_3f,
                      lpm_ctrl.level3_bank_config_block[3]);

    /* DB mode configurations */
    M_CONTROL_FIELD_SET(DB0_MODEf, lpm_ctrl.db_mod[0]);
    M_CONTROL_FIELD_SET(DB1_MODEf, lpm_ctrl.db_mod[1]);
    M_CONTROL_FIELD_SET(DB2_MODEf, lpm_ctrl.db_mod[2]);
    M_CONTROL_FIELD_SET(DB3_MODEf, lpm_ctrl.db_mod[3]);

    /* Data sel configurations */
    M_CONTROL_FIELD_SET(DATA0_SELf, lpm_ctrl.data_sel[0]);
    M_CONTROL_FIELD_SET(DATA1_SELf, lpm_ctrl.data_sel[1]);
    M_CONTROL_FIELD_SET(DATA2_SELf, lpm_ctrl.data_sel[2]);
    M_CONTROL_FIELD_SET(DATA3_SELf, lpm_ctrl.data_sel[3]);

    M_CONTROL_FIELD_SET(LPM_IP_DATA_GEN_RESULT_MODEf, lpm_ctrl.result_mode);

    M_CONTROL_FIELD_SET(POLICY_TYPE_MASK_DATA_0f, lpm_ctrl.destination_mask[0]);
    M_CONTROL_FIELD_SET(POLICY_TYPE_VALUE_DATA_0f, lpm_ctrl.destination[0]);
    M_CONTROL_FIELD_SET(POLICY_TYPE_HIT_DATA_0f, lpm_ctrl.destination_type_match[0]);
    M_CONTROL_FIELD_SET(POLICY_TYPE_MISS_DATA_0f, lpm_ctrl.destination_type_non_match[0]);

    M_CONTROL_FIELD_SET(POLICY_TYPE_MASK_DATA_1f, lpm_ctrl.destination_mask[1]);
    M_CONTROL_FIELD_SET(POLICY_TYPE_VALUE_DATA_1f, lpm_ctrl.destination[1]);
    M_CONTROL_FIELD_SET(POLICY_TYPE_HIT_DATA_1f, lpm_ctrl.destination_type_match[1]);
    M_CONTROL_FIELD_SET(POLICY_TYPE_MISS_DATA_1f, lpm_ctrl.destination_type_non_match[1]);

    M_CONTROL_FIELD_SET(POLICY_TYPE_MASK_DATA_2f, lpm_ctrl.destination_mask[2]);
    M_CONTROL_FIELD_SET(POLICY_TYPE_VALUE_DATA_2f, lpm_ctrl.destination[2]);
    M_CONTROL_FIELD_SET(POLICY_TYPE_HIT_DATA_2f, lpm_ctrl.destination_type_match[2]);
    M_CONTROL_FIELD_SET(POLICY_TYPE_MISS_DATA_2f, lpm_ctrl.destination_type_non_match[2]);

    M_CONTROL_FIELD_SET(POLICY_TYPE_MASK_DATA_3f, lpm_ctrl.destination_mask[3]);
    M_CONTROL_FIELD_SET(POLICY_TYPE_VALUE_DATA_3f, lpm_ctrl.destination[3]);
    M_CONTROL_FIELD_SET(POLICY_TYPE_HIT_DATA_3f, lpm_ctrl.destination_type_match[3]);
    M_CONTROL_FIELD_SET(POLICY_TYPE_MISS_DATA_3f, lpm_ctrl.destination_type_non_match[3]);

    M_CONTROL_FIELD_SET(M_LANE_MODEf, 4);

    /* Level 3 shared banks */
    M_E2T_00_SHARED_BANKS_CONTROLm_BANK_BYPASS_LPf_SET(sbc0,
            ctrl->tot.l3_bank_bitmap & 0xff);
    bcm56780_a0_cth_alpm_block_pdd_types(u, m, BCM56780_ALPM_L3_BLOCKS, &pdd_types[0]);

    /*
     * UFT_PDD_LANE_MUX_CONTROL.UFT_LANE_SEL
     * bit0: e2t_00_0
     * bit1: e2t_00_1
     * bit2: e2t_01_0
     * bit3: e2t_01_1
     *
     * UFT_PDD_LANE_MUX_CONTROL.MTOP_LANE_PAIR_SEL
     * bit0: e2t_02
     * bit1: e2t_03
     */

    
    pdd_lane_bmp = 0;
    UFT_PDD_LANE_MUX_CONTROLr_MTOP_LANE_PAIR_SELf_SET(pdd_lane, pdd_lane_bmp);

    MTOP_MUX_P0_CONTROLr_TAP_POINT_SELf_SET(mtop_control, 1);

    pts->sid[0] = M_LPM_IP_CONTROLm;
    pts->sid[1] = M_E2T_00_SHARED_BANKS_CONTROLm;
    pts->sid[2] = UFT_PDD_LANE_MUX_CONTROLr;
    pts->sid[3] = MTOP_MUX_P0_CONTROLr;
    sal_memcpy(pts->entry[0], &ce, sizeof(ce));
    sal_memcpy(pts->entry[1], &sbc0, sizeof(sbc0));
    sal_memcpy(pts->entry[2], &pdd_lane, sizeof(pdd_lane));
    sal_memcpy(pts->entry[3], &mtop_control, sizeof(mtop_control));


exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_alpm_control_encode(int u, int m, bcmptm_cth_alpm_control_t *ctrl,
                                    cth_alpm_control_pt_t *pts)
{
    LPM_IP_CONTROLm_t ce;
    MEMDB_TCAM_IFTA80_ACC_MODESr_t ifta80_acc_mode;
    IFTA90_E2T_00_SHARED_BANKS_CONTROLm_t sbc0;
    IFTA90_E2T_01_SHARED_BANKS_CONTROLm_t sbc1;
    UFT_PDD_LANE_MUX_CONTROLr_t pdd_lane;
    bcm56780_lpm_ip_control_t lpm_ctrl;
    int acc_mode;
    uint32_t pdd_types[BCM56780_ALPM_L3_BLOCKS] = {EM_TILE_ALPM_PDD_TYPE_NONE};
    uint32_t pdd_lane_bmp = 0;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(u);
    if ((bcmdrd_dev_flags_get(u) & CHIP_FLAG_MTOP) && m) {
        SHR_IF_ERR_EXIT(
            bcm56780_a0_cth_alpm_control_mtop_encode(u, m, ctrl, pts));
        SHR_EXIT();
    }


    SHR_NULL_CHECK(pts, SHR_E_PARAM);
    SHR_NULL_CHECK(ctrl, SHR_E_PARAM);

    pts->count = 5;

    /* If entry/sid pointer is null, simply return count. */
    if (!pts->entry || !pts->sid) {
        SHR_EXIT();
    }

    sal_memcpy(&ce, pts->entry[0], sizeof(ce));
    sal_memcpy(&ifta80_acc_mode, pts->entry[1], sizeof(ifta80_acc_mode));
    sal_memcpy(&sbc0, pts->entry[2], sizeof(sbc0));
    sal_memcpy(&sbc1, pts->entry[3], sizeof(sbc1));
    sal_memcpy(&pdd_lane, pts->entry[4], sizeof(pdd_lane));
    sal_memset(&lpm_ctrl, 0, sizeof(lpm_ctrl));

    /* Level-1 Configurations */
    bcm56780_a0_cth_alpm_ip_ctrl_init(u, m, ctrl, &lpm_ctrl, &rv);
    SHR_IF_ERR_EXIT(rv); /* not expect to fail, as we did dry-run. */

    CONTROL_FIELD_SET(DRIVE_HT_LPM_HIT_INDEX_ENTRY_TYPEf, 1);

    /* Level-1 Packing Set */
    CONTROL_FIELD_SET(LANE_0_KEY_TYPE_0_L1_PACKING_SETf,
                      lpm_ctrl.lane0_key_type_l1_packing_set[0]);
    CONTROL_FIELD_SET(LANE_0_KEY_TYPE_1_L1_PACKING_SETf,
                      lpm_ctrl.lane0_key_type_l1_packing_set[1]);
    CONTROL_FIELD_SET(LANE_0_KEY_TYPE_2_L1_PACKING_SETf,
                      lpm_ctrl.lane0_key_type_l1_packing_set[2]);
    CONTROL_FIELD_SET(LANE_0_KEY_TYPE_3_L1_PACKING_SETf,
                      lpm_ctrl.lane0_key_type_l1_packing_set[3]);


    CONTROL_FIELD_SET(LANE_1_KEY_TYPE_0_L1_PACKING_SETf,
                      lpm_ctrl.lane1_key_type_l1_packing_set[0]);
    CONTROL_FIELD_SET(LANE_1_KEY_TYPE_1_L1_PACKING_SETf,
                      lpm_ctrl.lane1_key_type_l1_packing_set[1]);
    CONTROL_FIELD_SET(LANE_1_KEY_TYPE_2_L1_PACKING_SETf,
                      lpm_ctrl.lane1_key_type_l1_packing_set[2]);
    CONTROL_FIELD_SET(LANE_1_KEY_TYPE_3_L1_PACKING_SETf,
                      lpm_ctrl.lane1_key_type_l1_packing_set[3]);


    CONTROL_FIELD_SET(LANE_2_KEY_TYPE_0_L1_PACKING_SETf,
                      lpm_ctrl.lane2_key_type_l1_packing_set[0]);
    CONTROL_FIELD_SET(LANE_2_KEY_TYPE_1_L1_PACKING_SETf,
                      lpm_ctrl.lane2_key_type_l1_packing_set[1]);
    CONTROL_FIELD_SET(LANE_2_KEY_TYPE_2_L1_PACKING_SETf,
                      lpm_ctrl.lane2_key_type_l1_packing_set[2]);
    CONTROL_FIELD_SET(LANE_2_KEY_TYPE_3_L1_PACKING_SETf,
                      lpm_ctrl.lane2_key_type_l1_packing_set[3]);

    CONTROL_FIELD_SET(LANE_3_KEY_TYPE_0_L1_PACKING_SETf,
                      lpm_ctrl.lane3_key_type_l1_packing_set[0]);
    CONTROL_FIELD_SET(LANE_3_KEY_TYPE_1_L1_PACKING_SETf,
                      lpm_ctrl.lane3_key_type_l1_packing_set[1]);
    CONTROL_FIELD_SET(LANE_3_KEY_TYPE_2_L1_PACKING_SETf,
                      lpm_ctrl.lane3_key_type_l1_packing_set[2]);
    CONTROL_FIELD_SET(LANE_3_KEY_TYPE_3_L1_PACKING_SETf,
                      lpm_ctrl.lane3_key_type_l1_packing_set[3]);


    /* Level-23 Packing Set */
    CONTROL_FIELD_SET(LANE_0_KEY_TYPE_0_L2L3_PACKING_SETf,
                      lpm_ctrl.lane0_key_type_ln_packing_set[0]);
    CONTROL_FIELD_SET(LANE_0_KEY_TYPE_1_L2L3_PACKING_SETf,
                      lpm_ctrl.lane0_key_type_ln_packing_set[1]);
    CONTROL_FIELD_SET(LANE_0_KEY_TYPE_2_L2L3_PACKING_SETf,
                      lpm_ctrl.lane0_key_type_ln_packing_set[2]);
    CONTROL_FIELD_SET(LANE_0_KEY_TYPE_3_L2L3_PACKING_SETf,
                      lpm_ctrl.lane0_key_type_ln_packing_set[3]);


    CONTROL_FIELD_SET(LANE_1_KEY_TYPE_0_L2L3_PACKING_SETf,
                      lpm_ctrl.lane1_key_type_ln_packing_set[0]);
    CONTROL_FIELD_SET(LANE_1_KEY_TYPE_1_L2L3_PACKING_SETf,
                      lpm_ctrl.lane1_key_type_ln_packing_set[1]);
    CONTROL_FIELD_SET(LANE_1_KEY_TYPE_2_L2L3_PACKING_SETf,
                      lpm_ctrl.lane1_key_type_ln_packing_set[2]);
    CONTROL_FIELD_SET(LANE_1_KEY_TYPE_3_L2L3_PACKING_SETf,
                      lpm_ctrl.lane1_key_type_ln_packing_set[3]);


    CONTROL_FIELD_SET(LANE_2_KEY_TYPE_0_L2L3_PACKING_SETf,
                      lpm_ctrl.lane2_key_type_ln_packing_set[0]);
    CONTROL_FIELD_SET(LANE_2_KEY_TYPE_1_L2L3_PACKING_SETf,
                      lpm_ctrl.lane2_key_type_ln_packing_set[1]);
    CONTROL_FIELD_SET(LANE_2_KEY_TYPE_2_L2L3_PACKING_SETf,
                      lpm_ctrl.lane2_key_type_ln_packing_set[2]);
    CONTROL_FIELD_SET(LANE_2_KEY_TYPE_3_L2L3_PACKING_SETf,
                      lpm_ctrl.lane2_key_type_ln_packing_set[3]);

    CONTROL_FIELD_SET(LANE_3_KEY_TYPE_0_L2L3_PACKING_SETf,
                      lpm_ctrl.lane3_key_type_ln_packing_set[0]);
    CONTROL_FIELD_SET(LANE_3_KEY_TYPE_1_L2L3_PACKING_SETf,
                      lpm_ctrl.lane3_key_type_ln_packing_set[1]);
    CONTROL_FIELD_SET(LANE_3_KEY_TYPE_2_L2L3_PACKING_SETf,
                      lpm_ctrl.lane3_key_type_ln_packing_set[2]);
    CONTROL_FIELD_SET(LANE_3_KEY_TYPE_3_L2L3_PACKING_SETf,
                      lpm_ctrl.lane3_key_type_ln_packing_set[3]);


    /* Level-1 Configurations */
    CONTROL_FIELD_SET(LEVEL1_DATABASE_SEL_BLOCK_0f,
                      lpm_ctrl.level1_database_sel_block[0]);
    CONTROL_FIELD_SET(LEVEL1_DATABASE_SEL_BLOCK_1f,
                      lpm_ctrl.level1_database_sel_block[1]);
    CONTROL_FIELD_SET(LEVEL1_DATABASE_SEL_BLOCK_2f,
                      lpm_ctrl.level1_database_sel_block[2]);
    CONTROL_FIELD_SET(LEVEL1_DATABASE_SEL_BLOCK_3f,
                      lpm_ctrl.level1_database_sel_block[3]);

    CONTROL_FIELD_SET(LEVEL1_KEY_INPUT_SEL_BLOCK_0f,
                      lpm_ctrl.level1_key_input_sel_block[0]);
    CONTROL_FIELD_SET(LEVEL1_KEY_INPUT_SEL_BLOCK_1f,
                      lpm_ctrl.level1_key_input_sel_block[1]);
    CONTROL_FIELD_SET(LEVEL1_KEY_INPUT_SEL_BLOCK_2f,
                      lpm_ctrl.level1_key_input_sel_block[2]);
    CONTROL_FIELD_SET(LEVEL1_KEY_INPUT_SEL_BLOCK_3f,
                      lpm_ctrl.level1_key_input_sel_block[3]);

    /* Level-2 Configurations */
    CONTROL_FIELD_SET(LEVEL2_DATABASE_SEL_BLOCK_0f,
                      lpm_ctrl.level2_database_sel_block[0]);
    CONTROL_FIELD_SET(LEVEL2_DATABASE_SEL_BLOCK_1f,
                      lpm_ctrl.level2_database_sel_block[1]);
    CONTROL_FIELD_SET(LEVEL2_DATABASE_SEL_BLOCK_2f,
                      lpm_ctrl.level2_database_sel_block[2]);
    CONTROL_FIELD_SET(LEVEL2_DATABASE_SEL_BLOCK_3f,
                      lpm_ctrl.level2_database_sel_block[3]);

    CONTROL_FIELD_SET(LEVEL2_KEY_INPUT_SEL_BLOCK_0f,
                      lpm_ctrl.level2_key_input_sel_block[0]);
    CONTROL_FIELD_SET(LEVEL2_KEY_INPUT_SEL_BLOCK_1f,
                      lpm_ctrl.level2_key_input_sel_block[1]);
    CONTROL_FIELD_SET(LEVEL2_KEY_INPUT_SEL_BLOCK_2f,
                      lpm_ctrl.level2_key_input_sel_block[2]);
    CONTROL_FIELD_SET(LEVEL2_KEY_INPUT_SEL_BLOCK_3f,
                      lpm_ctrl.level2_key_input_sel_block[3]);

    CONTROL_FIELD_SET(LEVEL2_BANK_CONFIG_BLOCK_0f,
                      lpm_ctrl.level2_bank_config_block[0]);
    CONTROL_FIELD_SET(LEVEL2_BANK_CONFIG_BLOCK_1f,
                      lpm_ctrl.level2_bank_config_block[1]);
    CONTROL_FIELD_SET(LEVEL2_BANK_CONFIG_BLOCK_2f,
                      lpm_ctrl.level2_bank_config_block[2]);
    CONTROL_FIELD_SET(LEVEL2_BANK_CONFIG_BLOCK_3f,
                      lpm_ctrl.level2_bank_config_block[3]);

    /* Level-3 Configurations */
    CONTROL_FIELD_SET(LEVEL3_DATABASE_SEL_BLOCK_0f,
                      lpm_ctrl.level3_database_sel_block[0]);
    CONTROL_FIELD_SET(LEVEL3_DATABASE_SEL_BLOCK_1f,
                      lpm_ctrl.level3_database_sel_block[1]);
    CONTROL_FIELD_SET(LEVEL3_DATABASE_SEL_BLOCK_2f,
                      lpm_ctrl.level3_database_sel_block[2]);
    CONTROL_FIELD_SET(LEVEL3_DATABASE_SEL_BLOCK_3f,
                      lpm_ctrl.level3_database_sel_block[3]);

    CONTROL_FIELD_SET(LEVEL3_KEY_INPUT_SEL_BLOCK_0f,
                      lpm_ctrl.level3_key_input_sel_block[0]);
    CONTROL_FIELD_SET(LEVEL3_KEY_INPUT_SEL_BLOCK_1f,
                      lpm_ctrl.level3_key_input_sel_block[1]);
    CONTROL_FIELD_SET(LEVEL3_KEY_INPUT_SEL_BLOCK_2f,
                      lpm_ctrl.level3_key_input_sel_block[2]);
    CONTROL_FIELD_SET(LEVEL3_KEY_INPUT_SEL_BLOCK_3f,
                      lpm_ctrl.level3_key_input_sel_block[3]);

    CONTROL_FIELD_SET(LEVEL3_BANK_CONFIG_BLOCK_0f,
                      lpm_ctrl.level3_bank_config_block[0]);
    CONTROL_FIELD_SET(LEVEL3_BANK_CONFIG_BLOCK_1f,
                      lpm_ctrl.level3_bank_config_block[1]);
    CONTROL_FIELD_SET(LEVEL3_BANK_CONFIG_BLOCK_2f,
                      lpm_ctrl.level3_bank_config_block[2]);
    CONTROL_FIELD_SET(LEVEL3_BANK_CONFIG_BLOCK_3f,
                      lpm_ctrl.level3_bank_config_block[3]);

    /* DB mode configurations */
    CONTROL_FIELD_SET(DB0_MODEf, lpm_ctrl.db_mod[0]);
    CONTROL_FIELD_SET(DB1_MODEf, lpm_ctrl.db_mod[1]);
    CONTROL_FIELD_SET(DB2_MODEf, lpm_ctrl.db_mod[2]);
    CONTROL_FIELD_SET(DB3_MODEf, lpm_ctrl.db_mod[3]);

    /* Data sel configurations */
    CONTROL_FIELD_SET(DATA0_SELf, lpm_ctrl.data_sel[0]);
    CONTROL_FIELD_SET(DATA1_SELf, lpm_ctrl.data_sel[1]);
    CONTROL_FIELD_SET(DATA2_SELf, lpm_ctrl.data_sel[2]);
    CONTROL_FIELD_SET(DATA3_SELf, lpm_ctrl.data_sel[3]);

    CONTROL_FIELD_SET(LPM_IP_DATA_GEN_RESULT_MODEf, lpm_ctrl.result_mode);

    CONTROL_FIELD_SET(POLICY_TYPE_MASK_DATA_0f, lpm_ctrl.destination_mask[0]);
    CONTROL_FIELD_SET(POLICY_TYPE_VALUE_DATA_0f, lpm_ctrl.destination[0]);
    CONTROL_FIELD_SET(POLICY_TYPE_HIT_DATA_0f, lpm_ctrl.destination_type_match[0]);
    CONTROL_FIELD_SET(POLICY_TYPE_MISS_DATA_0f, lpm_ctrl.destination_type_non_match[0]);

    CONTROL_FIELD_SET(POLICY_TYPE_MASK_DATA_1f, lpm_ctrl.destination_mask[1]);
    CONTROL_FIELD_SET(POLICY_TYPE_VALUE_DATA_1f, lpm_ctrl.destination[1]);
    CONTROL_FIELD_SET(POLICY_TYPE_HIT_DATA_1f, lpm_ctrl.destination_type_match[1]);
    CONTROL_FIELD_SET(POLICY_TYPE_MISS_DATA_1f, lpm_ctrl.destination_type_non_match[1]);

    CONTROL_FIELD_SET(POLICY_TYPE_MASK_DATA_2f, lpm_ctrl.destination_mask[2]);
    CONTROL_FIELD_SET(POLICY_TYPE_VALUE_DATA_2f, lpm_ctrl.destination[2]);
    CONTROL_FIELD_SET(POLICY_TYPE_HIT_DATA_2f, lpm_ctrl.destination_type_match[2]);
    CONTROL_FIELD_SET(POLICY_TYPE_MISS_DATA_2f, lpm_ctrl.destination_type_non_match[2]);

    CONTROL_FIELD_SET(POLICY_TYPE_MASK_DATA_3f, lpm_ctrl.destination_mask[3]);
    CONTROL_FIELD_SET(POLICY_TYPE_VALUE_DATA_3f, lpm_ctrl.destination[3]);
    CONTROL_FIELD_SET(POLICY_TYPE_HIT_DATA_3f, lpm_ctrl.destination_type_match[3]);
    CONTROL_FIELD_SET(POLICY_TYPE_MISS_DATA_3f, lpm_ctrl.destination_type_non_match[3]);

    /* Level1 IFTA80 banks */
    acc_mode = MEMDB_TCAM_IFTA80_ACC_MODESr_DATAf_GET(ifta80_acc_mode);
    acc_mode &= 0xFFFF00;
    switch (ctrl->tot.l1_bank_bitmap) {
    case 0xFFFF: acc_mode |= 0xFF; break;
    case 0xFFF0: acc_mode |= 0xFC; break;
    case 0x0FFF: acc_mode |= 0x3F; break;
    case 0xFF00: acc_mode |= 0xF0; break;
    case 0x00FF: acc_mode |= 0x0F; break;
    default:     assert(ctrl->tot.l1_bank_bitmap == 0); break;
    }
    MEMDB_TCAM_IFTA80_ACC_MODESr_DATAf_SET(ifta80_acc_mode, acc_mode);

    /* Level 3 shared banks */
    IFTA90_E2T_00_SHARED_BANKS_CONTROLm_BANK_BYPASS_LPf_SET(sbc0,
            ctrl->tot.l3_bank_bitmap & 0xf);
    IFTA90_E2T_01_SHARED_BANKS_CONTROLm_BANK_BYPASS_LPf_SET(sbc1,
            (ctrl->tot.l3_bank_bitmap >> 4 ) & 0xf);
    bcm56780_a0_cth_alpm_block_pdd_types(u, m, BCM56780_ALPM_L3_BLOCKS, &pdd_types[0]);

    /*
     * UFT_PDD_LANE_MUX_CONTROL.UFT_LANE_SEL
     * bit0: e2t_00_0
     * bit1: e2t_00_1
     * bit2: e2t_01_0
     * bit3: e2t_01_1
     *
     * UFT_PDD_LANE_MUX_CONTROL.MTOP_LANE_PAIR_SEL
     * bit0: e2t_02
     * bit1: e2t_03
     */

    pdd_lane_bmp = (pdd_types[0] != EM_TILE_ALPM_PDD_TYPE_NONE) |
                   ((pdd_types[1] != EM_TILE_ALPM_PDD_TYPE_NONE) << 1) |
                   ((pdd_types[2] != EM_TILE_ALPM_PDD_TYPE_NONE) << 2) |
                   ((pdd_types[3] != EM_TILE_ALPM_PDD_TYPE_NONE) << 3);
    UFT_PDD_LANE_MUX_CONTROLr_UFT_LANE_SELf_SET(pdd_lane, pdd_lane_bmp);

    pts->sid[0] = LPM_IP_CONTROLm;
    pts->sid[1] = MEMDB_TCAM_IFTA80_ACC_MODESr;
    pts->sid[2] = IFTA90_E2T_00_SHARED_BANKS_CONTROLm;
    pts->sid[3] = IFTA90_E2T_01_SHARED_BANKS_CONTROLm;
    pts->sid[4] = UFT_PDD_LANE_MUX_CONTROLr;
    sal_memcpy(pts->entry[0], &ce, sizeof(ce));
    sal_memcpy(pts->entry[1], &ifta80_acc_mode, sizeof(ifta80_acc_mode));
    sal_memcpy(pts->entry[2], &sbc0, sizeof(sbc0));
    sal_memcpy(pts->entry[3], &sbc1, sizeof(sbc1));
    sal_memcpy(pts->entry[4], &pdd_lane, sizeof(pdd_lane));


exit:
    SHR_FUNC_EXIT();
}


static int
bcm56780_a0_l1_phy_key_input_check_against_lt_type(int u, int m,
                                                   bcmptm_cth_alpm_control_t *ctrl,
                                                   int phy_block,
                                                   bcm56780_lt_type_t *lt_types)
{
    int db, i, min_db = 0xFFFF;
    int key_input = ctrl->l1_phy_key_input[phy_block];
    switch (lt_types[phy_block]) {
    case ALPM_LT_TYPE_LPM_NONE:
        db = ctrl->l1_phy_db[phy_block];
        for (i = 0; i < BCM56780_ALPM_L1_BLOCKS(m); i++) {
            if (lt_types[i] != ALPM_LT_TYPE_LPM_NONE &&
                ctrl->l1_phy_db[i] < min_db ) {
                min_db = ctrl->l1_phy_db[i];
            }
        }
        if (min_db != 0xFFFF && db > min_db) {
            /*
             * The DB for disabled blocks must be <= the min DB for
             * valid blocks.
             * This is to ensure that after physical -> logical blocks
             * mapping, we don't have mixed disabled and valid blocks.
             * And therefore the valid L1 blocks are consecutive,
             * which is assumed in RMALPM L1 management.
             */
            return SHR_E_PARAM;
        }


        if (bcm56780_a0_alpm_db_levels(ctrl, db)) {
            for (i = 0; i < BCM56780_ALPM_L1_BLOCKS(m); i++) {
                if (i != phy_block && db == ctrl->l1_phy_db[i] &&
                    lt_types[i] != ALPM_LT_TYPE_LPM_NONE) {
                    break;
                }
            }
            if (i == BCM56780_ALPM_L1_BLOCKS(m)) {
                /*
                 * It is an error when following criterias meet.
                 * 1. num db levels > 0, and
                 * 2. No any other L1 block with a valid tile && share the same DB.
                 */
                return SHR_E_PARAM;
            }
        }
        break;
    case ALPM_LT_TYPE_LPM_DST:
        if (key_input != KEY_INPUT_LPM_DST_Q &&
            key_input != KEY_INPUT_LPM_DST_D &&
            key_input != KEY_INPUT_LPM_DST_S) {
            return SHR_E_PARAM;
        }
        break;
    case ALPM_LT_TYPE_LPM_SRC:
        if (key_input != KEY_INPUT_LPM_SRC_Q &&
            key_input != KEY_INPUT_LPM_SRC_D &&
            key_input != KEY_INPUT_LPM_SRC_S) {
            return SHR_E_PARAM;
        }
        break;
    case ALPM_LT_TYPE_FP_COMP_DST:
        if (key_input != KEY_INPUT_FP_COMP_DST) {
            return SHR_E_PARAM;
        }
        break;
    case ALPM_LT_TYPE_FP_COMP_SRC:
        if (key_input != KEY_INPUT_FP_COMP_SRC) {
            return SHR_E_PARAM;
        }
        break;
    default:
        break;
    }
    return SHR_E_NONE;
}

static int
bcm56780_a0_key_mux_check_against_pdd_type(int u, int m, bcm56780_lpm_ip_control_t *lpm_ctrl)
{
    int i;
    uint32_t key_muxes[BCM56780_ALPM_L1_KEY_MUXES];
    uint32_t pdd_types[BCM56780_ALPM_L3_BLOCKS];

    SHR_FUNC_ENTER(u);
    /*
     * PDD Types, RESULTS_MODE, DATA_SEL are already verified,
     * so only KEY MUX is verified in this routine.
     */

    bcm56780_a0_cth_alpm_block_key_muxes(u, m, BCM56780_ALPM_L1_KEY_MUXES, &key_muxes[0]);
    bcm56780_a0_cth_alpm_block_pdd_types(u, m, BCM56780_ALPM_L3_BLOCKS, &pdd_types[0]);


    for (i = 0; i < 4; i++) {
        if (lpm_ctrl->result_mode == THREE_RESULTS_MODE) {
            if (i == 1) {
                continue;
            }
        } else if (lpm_ctrl->result_mode == TWO_RESULTS_MODE) {
            if (i == 1 || i == 3) {
                continue;
            }
        }

        /* Examine block that is with valid DB. */
        if (lpm_ctrl->data_sel[i] != IPDBX) {
            if (pdd_types[i] != key_muxes[i]) { /* KEY MUX must match PDD. */
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_alpm_control_l1_lt_type_validate(int u, int m,
                                                 bcmptm_cth_alpm_control_t *ctrl,
                                                 bcm56780_lpm_ip_control_t *lpm_ctrl,
                                                 int *reason)
{
    int rv;
    int phy_block;
    bcm56780_lt_type_t l1_lt_types[BCM56780_ALPM_L1_BLOCKS_MAX] = {ALPM_LT_TYPE_LPM_NONE};

    SHR_FUNC_ENTER(u);
    SHR_IF_ERR_EXIT(
        bcm56780_a0_cth_alpm_block_lt_types(u, m, LEVEL1,
                                            BCM56780_ALPM_L1_BLOCKS(m),
                                            &l1_lt_types[0]));

    for (phy_block = 0; phy_block < BCM56780_ALPM_L1_BLOCKS(m); phy_block++) {
        rv = bcm56780_a0_l1_phy_key_input_check_against_lt_type(u, m, ctrl, phy_block,
                                                                &l1_lt_types[0]);
        if (SHR_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(u,
                                "L1 phy_block=%d key_input %s "
                                "check against lt type %s fails.\n"),
                                phy_block,
                                bcm56780_a0_alpm_key_input_name(ctrl->l1_phy_key_input[phy_block]),
                                bcm56780_a0_alpm_lt_type_name(l1_lt_types[phy_block])
                                ));
            SHR_ERR_EXIT(rv);
        }
    }

    SHR_IF_ERR_EXIT(
        bcm56780_a0_key_mux_check_against_pdd_type(u, m, lpm_ctrl));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_alpm_control_validate_stage1(int u, int m,
                                             bcmptm_cth_alpm_control_t *ctrl)
{
    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(
        bcm56780_a0_cth_alpm_l1_parse_check(u, m, ctrl->tot.num_l1_banks,
                                            &ctrl->l1_key_input[0],
                                            &ctrl->l1_db[0],
                                            ctrl->db0_levels,
                                            ctrl->db1_levels,
                                            ctrl->db2_levels,
                                            ctrl->db3_levels,
                                            &ctrl->reason,
                                            NULL,
                                            NULL));
exit:
    SHR_FUNC_EXIT();
}

static bool
bcm56780_a0_cth_alpm_control_validate_stage2(int u, int m,
                                             bcmptm_cth_alpm_control_t *ctrl)
{
    bool ok;
    int rv = SHR_E_NONE;
    bcm56780_lpm_ip_control_t lpm_ctrl = {{0}};

    /* Level-1 bank_bitmap check. */
    switch (ctrl->tot.num_l1_banks) {
        case L1_TCAMS_0:
            ok = (ctrl->tot.l1_bank_bitmap == 0) ? 1: 0;
            break;
        case L1_TCAMS_8:
            ok = ((ctrl->tot.l1_bank_bitmap == 0x00FF) ||
                  (ctrl->tot.l1_bank_bitmap == 0xFF00)) ? 1: 0;
            break;
        case L1_TCAMS_12:
            ok = ((ctrl->tot.l1_bank_bitmap == 0x0FFF) ||
                  (ctrl->tot.l1_bank_bitmap == 0xFFF0)) ? 1: 0;
            break;
        case L1_TCAMS_16:
            ok = (ctrl->tot.l1_bank_bitmap == 0xFFFF) ? 1: 0;
            break;
        default:
            ok = false;
            break;
    }
    if (!ok) {
        return false;
    }

    /* All l2 banks must be assigned if Level2 enabled */
    if ((ctrl->tot.l2_bank_bitmap) && (ctrl->tot.num_l2_banks != 6)) {
        return false;
    }

    /*
     * The l1 parse check is assured in stage1, so we will not
     * see error for same check in stage2 which is added specifically
     * for UFT banks changes.
     * Any check relates to UFT bank should stay out of l1_parse_check.
     *
     * bcm56780_a0_cth_alpm_l1_parse_check() is part of ip_ctrl_init() below,
     * therefore skip it.
     */


    /* Level-1 Configurations */
    bcm56780_a0_cth_alpm_ip_ctrl_init(u, m, ctrl, &lpm_ctrl, &rv);
    if (SHR_FAILURE(rv)) {
        return false;
    }


    rv = bcm56780_a0_cth_alpm_control_l1_lt_type_validate(u, m, ctrl, &lpm_ctrl,
                                                          &ctrl->reason);
    if (SHR_FAILURE(rv)) {
        return false;
    }

    /* Level-2 database sel block */
    if (ctrl->alpm_mode & ALPM_MODE_FP_COMP) {
        if (ctrl->alpm_mode & ALPM_MODE_NO_LPM) {
            rv = bcm56780_a0_cth_alpm_fp_no_lpm_check_and_set(u, m, ctrl, NULL);
        } else  if (ctrl->alpm_mode & ALPM_MODE_RPF) {
            rv = bcm56780_a0_cth_alpm_fp_combined_rpf_check_and_set(u, m, ctrl, NULL);
        } else if (ctrl->alpm_mode & ALPM_MODE_PARALLEL) {
            rv = bcm56780_a0_cth_alpm_fp_parallel_check_and_set(u, m, ctrl, NULL);
        } else {
            rv = bcm56780_a0_cth_alpm_fp_combined_check_and_set(u, m, ctrl, NULL);
        }
    } else {
        if (ctrl->alpm_mode & ALPM_MODE_RPF) {
            if (ctrl->alpm_mode & ALPM_MODE_PARALLEL) {
                rv = bcm56780_a0_cth_alpm_parallel_rpf_check_and_set(u, m, ctrl, NULL);
            } else {
                rv = bcm56780_a0_cth_alpm_combined_rpf_check_and_set(u, m, ctrl, NULL);
            }
        } else {
            if (ctrl->alpm_mode & ALPM_MODE_PARALLEL) {
                rv = bcm56780_a0_cth_alpm_parallel_check_and_set(u, m, ctrl, NULL);
            } else {
                rv = bcm56780_a0_cth_alpm_combined_check_and_set(u, m, ctrl, NULL);
            }
        }
    }
    if (SHR_FAILURE(rv)) {
        return false;
    }

    return ok;
}

static int
bcm56780_a0_cth_alpm_control_validate(int u, int m, uint8_t stage,
                                      bcmptm_cth_alpm_control_t *ctrl)
{
    bool ok = true;

    SHR_FUNC_ENTER(u);

    switch (stage) {
        case 1:
        SHR_IF_ERR_EXIT
            (bcm56780_a0_cth_alpm_control_validate_stage1(u, m, ctrl));
        break;

        case 2:
        default:
        ok = bcm56780_a0_cth_alpm_control_validate_stage2(u, m, ctrl);
        break;
    }
    if (!ok) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_alpm_device_info_init(int u, int m, cth_alpm_device_info_t *dev)
{
    uint32_t val;
    bcmptm_cth_alpm_control_t *ctrl;
    bcmltd_sid_t ltid;
    bcmltd_fid_t fid;

    SHR_FUNC_ENTER(u);
    if (dev) {
        int i;
        ctrl = &dev->spec.default_control;
        dev->feature_pkms = 8;
        dev->feature_urpf = 1;
        dev->l1_tcams_shared = 1;
        for (i = 0; i < ALPM_L1_BANKS_MAX; i++) {
            dev->spec.l1_sid[i] = INVALIDm;
        }
        for (i = 0; i < ALPM_LN_BANKS_MAX; i++) {
            dev->spec.l2_sid[i] = INVALIDm;
            dev->spec.l3_sid[i] = INVALIDm;
        }
        if (m == ALPM_0) {
            dev->spec.tcam_blocks = 8;
            dev->spec.tcam_depth = 1024;
            dev->spec.l1_blocks = 4;

            dev->spec.l1_sid[0] = MEMDB_TCAM_IFTA80_MEM0_0m;
            dev->spec.l1_sid[1] = MEMDB_TCAM_IFTA80_MEM1_0m;
            dev->spec.l1_sid[2] = MEMDB_TCAM_IFTA80_MEM0_1m;
            dev->spec.l1_sid[3] = MEMDB_TCAM_IFTA80_MEM1_1m;
            dev->spec.l1_sid[4] = MEMDB_TCAM_IFTA80_MEM2_0m;
            dev->spec.l1_sid[5] = MEMDB_TCAM_IFTA80_MEM3_0m;
            dev->spec.l1_sid[6] = MEMDB_TCAM_IFTA80_MEM2_1m;
            dev->spec.l1_sid[7] = MEMDB_TCAM_IFTA80_MEM3_1m;
            dev->spec.l1_sid[8] = MEMDB_TCAM_IFTA80_MEM4_0m;
            dev->spec.l1_sid[9] = MEMDB_TCAM_IFTA80_MEM5_0m;
            dev->spec.l1_sid[10] = MEMDB_TCAM_IFTA80_MEM4_1m;
            dev->spec.l1_sid[11] = MEMDB_TCAM_IFTA80_MEM5_1m;
            dev->spec.l1_sid[12] = MEMDB_TCAM_IFTA80_MEM6_0m;
            dev->spec.l1_sid[13] = MEMDB_TCAM_IFTA80_MEM7_0m;
            dev->spec.l1_sid[14] = MEMDB_TCAM_IFTA80_MEM6_1m;
            dev->spec.l1_sid[15] = MEMDB_TCAM_IFTA80_MEM7_1m;

            dev->spec.l2_sid[0] = L3_DEFIP_ALPM_LEVEL2_B0m;
            dev->spec.l2_sid[1] = L3_DEFIP_ALPM_LEVEL2_B1m;
            dev->spec.l2_sid[2] = L3_DEFIP_ALPM_LEVEL2_B2m;
            dev->spec.l2_sid[3] = L3_DEFIP_ALPM_LEVEL2_B3m;
            dev->spec.l2_sid[4] = L3_DEFIP_ALPM_LEVEL2_B4m;
            dev->spec.l2_sid[5] = L3_DEFIP_ALPM_LEVEL2_B5m;

            dev->spec.l3_sid[0] = L3_DEFIP_ALPM_LEVEL3_B0m;
            dev->spec.l3_sid[1] = L3_DEFIP_ALPM_LEVEL3_B1m;
            dev->spec.l3_sid[2] = L3_DEFIP_ALPM_LEVEL3_B2m;
            dev->spec.l3_sid[3] = L3_DEFIP_ALPM_LEVEL3_B3m;
            dev->spec.l3_sid[4] = L3_DEFIP_ALPM_LEVEL3_B4m;
            dev->spec.l3_sid[5] = L3_DEFIP_ALPM_LEVEL3_B5m;
            dev->spec.l3_sid[6] = L3_DEFIP_ALPM_LEVEL3_B6m;
            dev->spec.l3_sid[7] = L3_DEFIP_ALPM_LEVEL3_B7m;

            sal_memset(ctrl, 0, sizeof(*ctrl));
            ctrl->alpm_mode = ALPM_MODE_FLEX_DEFAULT;
            ctrl->db0_levels = 0;
            ctrl->db1_levels = 0;
            ctrl->db2_levels = 0;
            ctrl->db3_levels = 0;
            ctrl->l1_phy_key_input[0] = KEY_INPUT_LPM_DST_Q;
            ctrl->l1_phy_key_input[1] = KEY_INPUT_LPM_DST_D;
            ctrl->l1_phy_key_input[2] = KEY_INPUT_LPM_DST_D;
            ctrl->l1_phy_key_input[3] = KEY_INPUT_LPM_DST_S;
            ctrl->l1_phy_db[0] = L1_DB0;
            ctrl->l1_phy_db[1] = L1_DB0;
            ctrl->l1_phy_db[2] = L1_DB0;
            ctrl->l1_phy_db[3] = L1_DB0;
            ctrl->hit_support = 0;
            ctrl->destination = 0;
            ctrl->destination_mask = 0;
            ctrl->destination_type_non_match = 0;

            /* Get enum value as defined by NPL */
            SHR_IF_ERR_EXIT(
                bcmlrd_table_field_name_to_idx(u, "L3_ALPM_CONTROL",
                                               "DESTINATION_TYPE_MATCH",
                                               &ltid, &fid));
        } else {
            if (bcmdrd_pt_index_count(u, M_DEFIP_ALPM_LEVEL2_B0m) == 0) {
                SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
            }
            dev->spec.tcam_blocks = 16;
            dev->spec.tcam_depth = 512;
            dev->spec.l1_blocks = 8;

            dev->spec.l1_sid[0] = MEMDB_TCAM_M_CTL_MEM0_0m;
            dev->spec.l1_sid[1] = MEMDB_TCAM_M_CTL_MEM0_1m;
            dev->spec.l1_sid[2] = MEMDB_TCAM_M_CTL_MEM1_0m;
            dev->spec.l1_sid[3] = MEMDB_TCAM_M_CTL_MEM1_1m;
            dev->spec.l1_sid[4] = MEMDB_TCAM_M_CTL_MEM2_0m;
            dev->spec.l1_sid[5] = MEMDB_TCAM_M_CTL_MEM2_1m;
            dev->spec.l1_sid[6] = MEMDB_TCAM_M_CTL_MEM3_0m;
            dev->spec.l1_sid[7] = MEMDB_TCAM_M_CTL_MEM3_1m;
            dev->spec.l1_sid[8] = MEMDB_TCAM_M_CTL_MEM4_0m;
            dev->spec.l1_sid[9] = MEMDB_TCAM_M_CTL_MEM4_1m;
            dev->spec.l1_sid[10] = MEMDB_TCAM_M_CTL_MEM5_0m;
            dev->spec.l1_sid[11] = MEMDB_TCAM_M_CTL_MEM5_1m;
            dev->spec.l1_sid[12] = MEMDB_TCAM_M_CTL_MEM6_0m;
            dev->spec.l1_sid[13] = MEMDB_TCAM_M_CTL_MEM6_1m;
            dev->spec.l1_sid[14] = MEMDB_TCAM_M_CTL_MEM7_0m;
            dev->spec.l1_sid[15] = MEMDB_TCAM_M_CTL_MEM7_1m;

            dev->spec.l2_sid[0] = M_DEFIP_ALPM_LEVEL2_B0m;
            dev->spec.l2_sid[1] = M_DEFIP_ALPM_LEVEL2_B1m;
            dev->spec.l2_sid[2] = M_DEFIP_ALPM_LEVEL2_B2m;
            dev->spec.l2_sid[3] = M_DEFIP_ALPM_LEVEL2_B3m;
            dev->spec.l2_sid[4] = M_DEFIP_ALPM_LEVEL2_B4m;
            dev->spec.l2_sid[5] = M_DEFIP_ALPM_LEVEL2_B5m;

            dev->spec.l3_sid[0] = M_DEFIP_ALPM_LEVEL3_B0m;
            dev->spec.l3_sid[1] = M_DEFIP_ALPM_LEVEL3_B1m;
            dev->spec.l3_sid[2] = M_DEFIP_ALPM_LEVEL3_B2m;
            dev->spec.l3_sid[3] = M_DEFIP_ALPM_LEVEL3_B3m;
            dev->spec.l3_sid[4] = M_DEFIP_ALPM_LEVEL3_B4m;
            dev->spec.l3_sid[5] = M_DEFIP_ALPM_LEVEL3_B5m;
            dev->spec.l3_sid[6] = M_DEFIP_ALPM_LEVEL3_B6m;
            dev->spec.l3_sid[7] = M_DEFIP_ALPM_LEVEL3_B7m;

            sal_memset(ctrl, 0, sizeof(*ctrl));
            ctrl->alpm_mode = ALPM_MODE_FLEX_DEFAULT;
            ctrl->db0_levels = 0;
            ctrl->db1_levels = 0;
            ctrl->db2_levels = 0;
            ctrl->db3_levels = 0;
            ctrl->l1_phy_key_input[0] = KEY_INPUT_LPM_DST_Q;
            ctrl->l1_phy_key_input[1] = KEY_INPUT_LPM_DST_Q;
            ctrl->l1_phy_key_input[2] = KEY_INPUT_LPM_DST_D;
            ctrl->l1_phy_key_input[3] = KEY_INPUT_LPM_DST_D;
            ctrl->l1_phy_key_input[4] = KEY_INPUT_LPM_DST_S;
            ctrl->l1_phy_key_input[5] = KEY_INPUT_LPM_DST_S;
            ctrl->l1_phy_key_input[6] = KEY_INPUT_LPM_DST_S;
            ctrl->l1_phy_key_input[7] = KEY_INPUT_LPM_DST_S;
            ctrl->l1_phy_db[0] = L1_DB0;
            ctrl->l1_phy_db[1] = L1_DB0;
            ctrl->l1_phy_db[2] = L1_DB0;
            ctrl->l1_phy_db[3] = L1_DB0;
            ctrl->l1_phy_db[4] = L1_DB0;
            ctrl->l1_phy_db[5] = L1_DB0;
            ctrl->l1_phy_db[6] = L1_DB0;
            ctrl->l1_phy_db[7] = L1_DB0;
            ctrl->hit_support = 0;
            ctrl->destination = 0;
            ctrl->destination_mask = 0;
            ctrl->destination_type_non_match = 0;

            /* Get enum value as defined by NPL */
            SHR_IF_ERR_EXIT(
                bcmlrd_table_field_name_to_idx(u, "L3_ALPM_CONTROL_MTOP",
                                               "DESTINATION_TYPE_MATCH",
                                               &ltid, &fid));
        }

        SHR_IF_ERR_EXIT(
            bcmlrd_field_symbol_to_value(u,
                                         ltid,
                                         fid,
                                         "NHOP",
                                         &val));
        ctrl->destination_type_match = val;
        ctrl->ipv4_uc_sbr          = 0;
        ctrl->ipv4_uc_vrf_sbr      = 0;
        ctrl->ipv4_uc_override_sbr = 0;
        ctrl->ipv6_uc_sbr          = 0;
        ctrl->ipv6_uc_vrf_sbr      = 0;
        ctrl->ipv6_uc_override_sbr = 0;
        ctrl->ipv4_comp_sbr        = 0;
        ctrl->ipv6_comp_sbr        = 0;
        ctrl->reason = ALPM_ERROR_REASON_NONE;
    } else {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

int
bcm56780_a0_cth_alpm_driver_register(int u, bcmptm_cth_alpm_driver_t *drv)
{
    SHR_FUNC_ENTER(u);

    if (drv) {
        drv->control_encode = bcm56780_a0_cth_alpm_control_encode;
        drv->control_validate = bcm56780_a0_cth_alpm_control_validate;
        drv->device_info_init = bcm56780_a0_cth_alpm_device_info_init;
    } else {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}



