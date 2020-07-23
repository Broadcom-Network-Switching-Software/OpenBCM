/*! \file bcmptm_bcm56990_a0_cth_alpm.c
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
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/chip/bcm56990_a0_defs.h>

#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_cth_alpm_internal.h>
#include "bcmptm_bcm56990_a0_cth_alpm.h"


/*******************************************************************************
 * Defines
 */
#undef FULL_LPM_IP_CONTROL
#define BSL_LOG_MODULE BSL_LS_BCMPTM_CTHALPM

#define CONTROL_FIELD_SET(_f, _v)   \
    LPM_IP_CONTROLm_##_f##_SET(ce, (_v))

#define IPDB0 0
#define IPDB1 1
#define IPDB2 2
#define IPDB3 3
#define IPDBX 4

#define K0  0
#define K1  1
#define K2  2
#define K3  3
#define K4  4
#define K5  5
#define K6  6
#define K7  7
#define K8  8
#define K9  9
#define K10 10

#define DISABLED    0

#define KEY_INPUT_LPM_DST_Q         0   /* IPv6-128 DST */
#define KEY_INPUT_LPM_DST_D         1   /* IPv6-77 DST */
#define KEY_INPUT_LPM_DST_S         2   /* IPv6-32 DST */
#define KEY_INPUT_LPM_SRC_Q         3   /* IPv6-128 SRC */
#define KEY_INPUT_LPM_SRC_D         4   /* IPv6-77 SRC */
#define KEY_INPUT_LPM_SRC_S         5   /* IPv6-32 SRC */
#define KEY_INPUT_FP_COMP_DST       6
#define KEY_INPUT_FP_COMP_SRC       7
#define KEY_INPUT_LPM_L3MC_Q        8
#define KEY_INPUT_LPM_L3MC_D        9
#define KEY_INPUT_LPM_L3MC_S        10

#define FOUR_RESULTS_MODE           2
#define THREE_RESULTS_MODE          1
#define TWO_RESULTS_MODE            0

#define L1_DB0 0
#define L1_DB1 1
#define L1_DB2 2
#define L1_DB3 3
#define L1_DBS 4

#define KEY_INPUT_MAP1(db)      \
    ((db) == IPDB2 ? K3 : K8)

#define KEY_INPUT_MAP2(db)      \
    (((db) == IPDB2 || (db) == IPDB3) ? K3 : K8)

#define KEY_INPUT_MAP3(db)      \
    ((db) == IPDB2 ? K6 :       \
     (db) == IPDB3 ? K7 : K8)

#define KEY_INPUT_MAP4(db)      \
    ((db) == IPDB2 ? K6 :       \
     (db) == IPDB3 ? K7 : K8)

#define KEY_INPUT_MAP5(db)      \
    ((db) == IPDB2 ? K6 :       \
     (db) == IPDB3 ? K7 :       \
     (db) == IPDB1 ? K3 : K8)

#define BCM56990_ALPM_L1_BLOCKS     8

/*******************************************************************************
 * Typedefs
 */
typedef struct bcm56990_lpm_ip_control_s {
    int level1_database_sel_block[8];
    int level2_database_sel_block[4];
    int level3_database_sel_block[4];

    int level1_key_input_sel_block[8];
    int level2_key_input_sel_block[4];
    int level3_key_input_sel_block[4];

    int level2_bank_config_block[4];
    int level3_bank_config_block[4];

    int db_mod[4];
    int data_sel[4];
    int result_mode;
} bcm56990_lpm_ip_control_t;

typedef struct bcm56990_lpm_lane_control_s {
    uint8_t comp_src_enable;
    uint8_t comp_dst_enable;
    uint8_t l3_src_enable;
    uint8_t l3_dst_enable;
} bcm56990_lpm_lane_control_t;



/*******************************************************************************
 * Private variables
 */
static uint8_t
bcm56990_combined_blocks[2][4] = {          /* DB0 */
    {IPDBX, IPDBX, IPDBX, IPDBX},           /*  0  */
    {IPDB0, IPDB0, IPDB0, IPDB0}            /*  1  */
};
static uint8_t
bcm56990_parallel_l2_blocks[4][4] = {       /* DB0 DB1 */
    {IPDBX, IPDBX, IPDBX, IPDBX},           /*  0   0  */
    {IPDB1, IPDB1, IPDB1, IPDB1},           /*  0   1  */
    {IPDB0, IPDB0, IPDB0, IPDB0},           /*  1   0  */
    {IPDB0, IPDB0, IPDB1, IPDB1}            /*  1   1  */
};
static uint8_t
bcm56990_parallel_l3_blocks[4][4] = {       /* DB0 DB1 */
    {IPDBX, IPDBX, IPDBX, IPDBX},           /*  0   0  */
    {IPDB1, IPDB1, IPDB1, IPDB1},           /*  0   1  */
    {IPDB0, IPDB0, IPDB0, IPDB0},           /*  1   0  */
    {IPDB1, IPDB0, IPDB1, IPDB0}            /*  1   1  */
};
static uint8_t
bcm56990_combined_rpf_l2_blocks[4][4] = {   /* DB0 DB2 */
    {IPDBX, IPDBX, IPDBX, IPDBX},           /*  0   0  */
    {IPDB2, IPDB2, IPDB2, IPDB2},           /*  0   1  */
    {IPDB0, IPDB0, IPDB0, IPDB0},           /*  1   0  */
    {IPDB0, IPDB0, IPDB2, IPDB2}            /*  1   1  */
};
static uint8_t
bcm56990_combined_rpf_l3_blocks[4][4] = {   /* DB0 DB2 */
    {IPDBX, IPDBX, IPDBX, IPDBX},           /*  0   0  */
    {IPDB2, IPDB2, IPDB2, IPDB2},           /*  0   1  */
    {IPDB0, IPDB0, IPDB0, IPDB0},           /*  1   0  */
    {IPDB2, IPDB0, IPDB2, IPDB0}            /*  1   1  */
};
static uint8_t
bcm56990_parallel_rpf_blocks[16][4] = {     /* DB0 DB1 DB2 DB3 */
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
bcm56990_fp_combined_blocks[8][4] = {       /* DB0 DB2 DB3 */
    {IPDBX, IPDBX, IPDBX, IPDBX},           /*  0   0   0  */
    {IPDB3, IPDB3, IPDB3, IPDB3},           /*  0   0   1  */
    {IPDB2, IPDB2, IPDB2, IPDB2},           /*  0   1   0  */
    {IPDB2, IPDB2, IPDB3, IPDB3},           /*  0   1   1  */
    {IPDB0, IPDB0, IPDB0, IPDB0},           /*  1   0   0  */
    {IPDB0, IPDB0, IPDB0, IPDB3},           /*  1   0   1  */
    {IPDB0, IPDB0, IPDB2, IPDB0},           /*  1   1   0  */
    {IPDB0, IPDB0, IPDB2, IPDB3}            /*  1   1   1  */
};
static uint8_t
bcm56990_fp_parallel_blocks[16][4] = {      /* DB0 DB1 DB2 DB3 */
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
bcm56990_fp_combined_rpf_blocks[16][4] = {  /* DB0 DB1 DB2 DB3 */
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

/*******************************************************************************
 * Private Functions
 */


static int
bcm56990_a0_cth_alpm_l1_parse_check(int u, int *l1_key_input, int *l1_db,
                                    int db0_levels,
                                    int db1_levels,
                                    int db2_levels,
                                    int db3_levels,
                                    int *alpm_mode_out, int *alpm_dbs_out)
{
    int i, alpm_mode = 0, alpm_src_mode = 0, alpm_dbs;
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

    switch (l1_key_input[0]) {
        case KEY_INPUT_LPM_L3MC_Q:
        case KEY_INPUT_LPM_L3MC_D:
        case KEY_INPUT_LPM_L3MC_S:
            break;
        default:
            LOG_INFO(BSL_LOG_MODULE,
                      (BSL_META_U(u,
                        "Expect L3MC key input of level 1 block 0 \n"
                        )));
            return SHR_E_PARAM;
    }

    for (i = 1; i < BCM56990_ALPM_L1_BLOCKS; i++) {
        if (l1_key_input[i] == KEY_INPUT_LPM_L3MC_Q ||
            l1_key_input[i] == KEY_INPUT_LPM_L3MC_D ||
            l1_key_input[i] == KEY_INPUT_LPM_L3MC_S) {
            LOG_INFO(BSL_LOG_MODULE,
                      (BSL_META_U(u,
                        "Expect no L3MC key input with level 1 block %d \n"
                        ), i));
            return SHR_E_PARAM;
        }

        /*
        * Tcam blocks with same DB group together, and within a group,
        * key input must be strictly ordered.
        */
        if ((i + 1) < BCM56990_ALPM_L1_BLOCKS && l1_db[i] == l1_db[i + 1]) {
            if (l1_key_input[i] > l1_key_input[i + 1]) {
                LOG_INFO(BSL_LOG_MODULE,
                        (BSL_META_U(u,
                            "Expect strict orders of key inputs.\n"
                            )));
                return SHR_E_PARAM;
            }
        }
    }

    for (i = 0; i < BCM56990_ALPM_L1_BLOCKS; i++) {
        if (l1_key_input[i] == KEY_INPUT_LPM_L3MC_Q ||
            l1_key_input[i] == KEY_INPUT_LPM_L3MC_D ||
            l1_key_input[i] == KEY_INPUT_LPM_L3MC_S ||
            l1_key_input[i] == KEY_INPUT_LPM_DST_Q ||
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
        alpm_dbs++;
    } else {
        LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META_U(u,
                        "Expect LPM DST key input.\n"
                    )));
        return SHR_E_PARAM;
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
        if (num_lpmdst == 0) {
            alpm_mode |= ALPM_MODE_NO_LPM;
        }
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

    assert(db_lpmdst != -1);
    if (db_lpmdst != L1_DB0) {
        LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META_U(u,
                        "Expect LPM DST assigned with DB0.\n"
                    )));
        return SHR_E_PARAM;
    }
    if (db0_levels == 0) {
        LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META_U(u,
                        "Expect DB0_LEVELS > 0.\n"
                    )));
        return SHR_E_PARAM;
    }

    /* Check FP compression configurations */
    if (alpm_mode & ALPM_MODE_FP_COMP) {
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
            assert(alpm_dbs == 4);
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
            assert(alpm_dbs == 4);
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
            assert(alpm_dbs == 3);
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
                assert(alpm_dbs == 4);
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
                assert(alpm_dbs == 2);
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
                assert(alpm_dbs == 2);
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
                assert(alpm_dbs == 1);
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
db_l2_bit_mask(uint8_t *blocks, uint8_t db)
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
db_l3_bit_mask(uint8_t *blocks, uint8_t db)
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
l2_bitmap_check_and_set(uint8_t *blocks, uint8_t db,
                        uint32_t in_bank_bitmap,
                        bcmptm_cth_alpm_control_t *ctrl_out,
                        int *rv)
{
    uint32_t bit_mask;

    if (SHR_FAILURE(*rv)) {
        return;
    }

    bit_mask = db_l2_bit_mask(blocks, db);
    if (bit_mask && ((in_bank_bitmap & bit_mask) == 0)) {
        *rv = SHR_E_PARAM;
        return;
    }
    if (ctrl_out) {
        switch (db) {
            case IPDB0:
            ctrl_out->db0.l2_bank_bitmap = in_bank_bitmap & bit_mask;
            ctrl_out->db0.num_l2_banks =
                shr_util_popcount(ctrl_out->db0.l2_bank_bitmap);
            break;

            case IPDB1:
            ctrl_out->db1.l2_bank_bitmap = in_bank_bitmap & bit_mask;
            ctrl_out->db1.num_l2_banks =
                shr_util_popcount(ctrl_out->db1.l2_bank_bitmap);
            break;

            case IPDB2:
            ctrl_out->db2.l2_bank_bitmap = in_bank_bitmap & bit_mask;
            ctrl_out->db2.num_l2_banks =
                shr_util_popcount(ctrl_out->db2.l2_bank_bitmap);
            break;

            case IPDB3:
            ctrl_out->db3.l2_bank_bitmap = in_bank_bitmap & bit_mask;
            ctrl_out->db3.num_l2_banks =
                shr_util_popcount(ctrl_out->db3.l2_bank_bitmap);
            break;

            default:
            *rv = SHR_E_PARAM;
        }
    }

    return;
}

static void
l3_bitmap_check_and_set(uint8_t *blocks, uint8_t db,
                        uint32_t in_bank_bitmap,
                        bcmptm_cth_alpm_control_t *ctrl_out,
                        int *rv)
{

    uint32_t bit_mask;

    if (SHR_FAILURE(*rv)) {
        return;
    }

    bit_mask = db_l3_bit_mask(blocks, db);
    if (bit_mask && ((in_bank_bitmap & bit_mask) == 0)) {
        *rv = SHR_E_PARAM;
        return;
    }

    if (ctrl_out) {
        switch (db) {
            case IPDB0:
            ctrl_out->db0.l3_bank_bitmap = in_bank_bitmap & bit_mask;
            ctrl_out->db0.num_l3_banks =
                shr_util_popcount(ctrl_out->db0.l3_bank_bitmap);
            break;

            case IPDB1:
            ctrl_out->db1.l3_bank_bitmap = in_bank_bitmap & bit_mask;
            ctrl_out->db1.num_l3_banks =
                shr_util_popcount(ctrl_out->db1.l3_bank_bitmap);
            break;

            case IPDB2:
            ctrl_out->db2.l3_bank_bitmap = in_bank_bitmap & bit_mask;
            ctrl_out->db2.num_l3_banks =
                shr_util_popcount(ctrl_out->db2.l3_bank_bitmap);
            break;

            case IPDB3:
            ctrl_out->db3.l3_bank_bitmap = in_bank_bitmap & bit_mask;
            ctrl_out->db3.num_l3_banks =
                shr_util_popcount(ctrl_out->db3.l3_bank_bitmap);
            break;

            default:
            *rv = SHR_E_PARAM;
        }
    }

    return;
}


static int
bcm56990_a0_cth_alpm_combined_check_and_set(int u,
                                   bcmptm_cth_alpm_control_t *ctrl,
                                   bcmptm_cth_alpm_control_t *ctrl_out)
{
    int rv = SHR_E_NONE;
    uint8_t sub;
    uint8_t *blocks;
    sub = (ctrl->db0_levels >= 2);
    blocks = &bcm56990_combined_blocks[sub][0];
    l2_bitmap_check_and_set(blocks, IPDB0, ctrl->tot.l2_bank_bitmap, ctrl_out, &rv);

    sub = (ctrl->db0_levels >= 3);
    blocks = &bcm56990_combined_blocks[sub][0];
    l3_bitmap_check_and_set(blocks, IPDB0, ctrl->tot.l3_bank_bitmap, ctrl_out, &rv);

    return rv;
}

static int
bcm56990_a0_cth_alpm_parallel_check_and_set(int u,
                                   bcmptm_cth_alpm_control_t *ctrl,
                                   bcmptm_cth_alpm_control_t *ctrl_out)
{
    int rv = SHR_E_NONE;
    uint8_t sub;
    uint8_t *blocks;

    sub = ((ctrl->db0_levels >= 2) << 1) | (ctrl->db1_levels >= 2);
    blocks = &bcm56990_parallel_l2_blocks[sub][0];
    l2_bitmap_check_and_set(blocks, IPDB0, ctrl->tot.l2_bank_bitmap, ctrl_out, &rv);
    l2_bitmap_check_and_set(blocks, IPDB1, ctrl->tot.l2_bank_bitmap, ctrl_out, &rv);


    sub = ((ctrl->db0_levels >= 3) << 1) | (ctrl->db1_levels >= 3);
    blocks = &bcm56990_parallel_l3_blocks[sub][0];
    l3_bitmap_check_and_set(blocks, IPDB0, ctrl->tot.l3_bank_bitmap, ctrl_out, &rv);
    l3_bitmap_check_and_set(blocks, IPDB1, ctrl->tot.l3_bank_bitmap, ctrl_out, &rv);

    return rv;
}

static int
bcm56990_a0_cth_alpm_combined_rpf_check_and_set(int u,
                                   bcmptm_cth_alpm_control_t *ctrl,
                                   bcmptm_cth_alpm_control_t *ctrl_out)
{
    int rv = SHR_E_NONE;
    uint8_t sub;
    uint8_t *blocks;

    sub = ((ctrl->db0_levels >= 2) << 1) | (ctrl->db2_levels >= 2);
    blocks = &bcm56990_combined_rpf_l2_blocks[sub][0];
    l2_bitmap_check_and_set(blocks, IPDB0, ctrl->tot.l2_bank_bitmap, ctrl_out, &rv);
    l2_bitmap_check_and_set(blocks, IPDB2, ctrl->tot.l2_bank_bitmap, ctrl_out, &rv);


    sub = ((ctrl->db0_levels >= 3) << 1) | (ctrl->db2_levels >= 3);
    blocks = &bcm56990_combined_rpf_l3_blocks[sub][0];
    l3_bitmap_check_and_set(blocks, IPDB0, ctrl->tot.l3_bank_bitmap, ctrl_out, &rv);
    l3_bitmap_check_and_set(blocks, IPDB2, ctrl->tot.l3_bank_bitmap, ctrl_out, &rv);
    return rv;
}

static int
bcm56990_a0_cth_alpm_parallel_rpf_check_and_set(int u,
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
    uint8_t sub;
    uint8_t *blocks;

    sub = ((ctrl->db0_levels >= 2) << 3) |
          ((ctrl->db1_levels >= 2) << 2) |
          ((ctrl->db2_levels >= 2) << 1) |
           (ctrl->db3_levels >= 2);
    blocks = &bcm56990_parallel_rpf_blocks[sub][0];
    l2_bitmap_check_and_set(blocks, IPDB0, ctrl->tot.l2_bank_bitmap, ctrl_out, &rv);
    l2_bitmap_check_and_set(blocks, IPDB1, ctrl->tot.l2_bank_bitmap, ctrl_out, &rv);
    l2_bitmap_check_and_set(blocks, IPDB2, ctrl->tot.l2_bank_bitmap, ctrl_out, &rv);
    l2_bitmap_check_and_set(blocks, IPDB3, ctrl->tot.l2_bank_bitmap, ctrl_out, &rv);


    sub = ((ctrl->db0_levels >= 3) << 3) |
          ((ctrl->db1_levels >= 3) << 2) |
          ((ctrl->db2_levels >= 3) << 1) |
           (ctrl->db3_levels >= 3);
    blocks = &bcm56990_parallel_rpf_blocks[sub][0];
    l3_bitmap_check_and_set(blocks, IPDB0, ctrl->tot.l3_bank_bitmap, ctrl_out, &rv);
    l3_bitmap_check_and_set(blocks, IPDB1, ctrl->tot.l3_bank_bitmap, ctrl_out, &rv);
    l3_bitmap_check_and_set(blocks, IPDB2, ctrl->tot.l3_bank_bitmap, ctrl_out, &rv);
    l3_bitmap_check_and_set(blocks, IPDB3, ctrl->tot.l3_bank_bitmap, ctrl_out, &rv);

    return rv;
}
static int
bcm56990_a0_cth_alpm_fp_combined_check_and_set(int u,
                                   bcmptm_cth_alpm_control_t *ctrl,
                                   bcmptm_cth_alpm_control_t *ctrl_out)
{
    /*
     * 1. DB0 - LPM_DST      - block 0/1
     * 2. DB2 - FP_COMP DST  - block 2
     * 3. DB3 - FP_COMP SRC  - block 3
     */
    int rv = SHR_E_NONE;
    uint8_t sub;
    uint8_t *blocks;

    sub = ((ctrl->db0_levels >= 2) << 2) |
          ((ctrl->db2_levels >= 2) << 1) |
           (ctrl->db3_levels >= 2);
    blocks = &bcm56990_fp_combined_blocks[sub][0];
    l2_bitmap_check_and_set(blocks, IPDB0, ctrl->tot.l2_bank_bitmap, ctrl_out, &rv);
    l2_bitmap_check_and_set(blocks, IPDB2, ctrl->tot.l2_bank_bitmap, ctrl_out, &rv);
    l2_bitmap_check_and_set(blocks, IPDB3, ctrl->tot.l2_bank_bitmap, ctrl_out, &rv);



    sub = ((ctrl->db0_levels >= 3) << 2) |
          ((ctrl->db2_levels >= 3) << 1) |
           (ctrl->db3_levels >= 3);
    blocks = &bcm56990_fp_combined_blocks[sub][0];
    l3_bitmap_check_and_set(blocks, IPDB0, ctrl->tot.l3_bank_bitmap, ctrl_out, &rv);
    l3_bitmap_check_and_set(blocks, IPDB2, ctrl->tot.l3_bank_bitmap, ctrl_out, &rv);
    l3_bitmap_check_and_set(blocks, IPDB3, ctrl->tot.l3_bank_bitmap, ctrl_out, &rv);
    return rv;
}

static int
bcm56990_a0_cth_alpm_fp_parallel_check_and_set(int u,
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
    uint8_t sub;
    uint8_t *blocks;

    sub = ((ctrl->db0_levels >= 2) << 3) |
          ((ctrl->db1_levels >= 2) << 2) |
          ((ctrl->db2_levels >= 2) << 1) |
           (ctrl->db3_levels >= 2);
    blocks = &bcm56990_fp_parallel_blocks[sub][0];
    l2_bitmap_check_and_set(blocks, IPDB0, ctrl->tot.l2_bank_bitmap, ctrl_out, &rv);
    l2_bitmap_check_and_set(blocks, IPDB1, ctrl->tot.l2_bank_bitmap, ctrl_out, &rv);
    l2_bitmap_check_and_set(blocks, IPDB2, ctrl->tot.l2_bank_bitmap, ctrl_out, &rv);
    l2_bitmap_check_and_set(blocks, IPDB3, ctrl->tot.l2_bank_bitmap, ctrl_out, &rv);

    sub = ((ctrl->db0_levels >= 3) << 3) |
          ((ctrl->db1_levels >= 3) << 2) |
          ((ctrl->db2_levels >= 3) << 1) |
           (ctrl->db3_levels >= 3);
    blocks = &bcm56990_fp_parallel_blocks[sub][0];
    l3_bitmap_check_and_set(blocks, IPDB0, ctrl->tot.l3_bank_bitmap, ctrl_out, &rv);
    l3_bitmap_check_and_set(blocks, IPDB1, ctrl->tot.l3_bank_bitmap, ctrl_out, &rv);
    l3_bitmap_check_and_set(blocks, IPDB2, ctrl->tot.l3_bank_bitmap, ctrl_out, &rv);
    l3_bitmap_check_and_set(blocks, IPDB3, ctrl->tot.l3_bank_bitmap, ctrl_out, &rv);

    return rv;
}

static int
bcm56990_a0_cth_alpm_fp_combined_rpf_check_and_set(int u,
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
    uint8_t sub;
    uint8_t *blocks;

    sub = ((ctrl->db0_levels >= 2) << 3) |
          ((ctrl->db1_levels >= 2) << 2) |
          ((ctrl->db2_levels >= 2) << 1) |
           (ctrl->db3_levels >= 2);
    blocks = &bcm56990_fp_combined_rpf_blocks[sub][0];
    l2_bitmap_check_and_set(blocks, IPDB0, ctrl->tot.l2_bank_bitmap, ctrl_out, &rv);
    l2_bitmap_check_and_set(blocks, IPDB1, ctrl->tot.l2_bank_bitmap, ctrl_out, &rv);
    l2_bitmap_check_and_set(blocks, IPDB2, ctrl->tot.l2_bank_bitmap, ctrl_out, &rv);
    l2_bitmap_check_and_set(blocks, IPDB3, ctrl->tot.l2_bank_bitmap, ctrl_out, &rv);


    sub = ((ctrl->db0_levels >= 3) << 3) |
          ((ctrl->db1_levels >= 3) << 2) |
          ((ctrl->db2_levels >= 3) << 1) |
           (ctrl->db3_levels >= 3);
    blocks = &bcm56990_fp_combined_rpf_blocks[sub][0];
    l3_bitmap_check_and_set(blocks, IPDB0, ctrl->tot.l3_bank_bitmap, ctrl_out, &rv);
    l3_bitmap_check_and_set(blocks, IPDB1, ctrl->tot.l3_bank_bitmap, ctrl_out, &rv);
    l3_bitmap_check_and_set(blocks, IPDB2, ctrl->tot.l3_bank_bitmap, ctrl_out, &rv);
    l3_bitmap_check_and_set(blocks, IPDB3, ctrl->tot.l3_bank_bitmap, ctrl_out, &rv);
    return rv;
}

static void
bcm56990_a0_cth_alpm_combined_init(int u,
                                   bcmptm_cth_alpm_control_t *ctrl,
                                   bcm56990_lpm_ip_control_t *ip_ctrl)
{

    uint8_t sub;
    uint8_t *blocks;

    sub = (ctrl->db0_levels >= 2);
    blocks = &bcm56990_combined_blocks[sub][0];
    ip_ctrl->level2_database_sel_block[0] = blocks[0];
    ip_ctrl->level2_database_sel_block[1] = blocks[1];
    ip_ctrl->level2_database_sel_block[2] = blocks[2];
    ip_ctrl->level2_database_sel_block[3] = blocks[3];

    ip_ctrl->level2_key_input_sel_block[0] = K8;
    ip_ctrl->level2_key_input_sel_block[1] = K8;
    ip_ctrl->level2_key_input_sel_block[2] = K8;
    ip_ctrl->level2_key_input_sel_block[3] = K8;

    sub = (ctrl->db0_levels >= 3);
    blocks = &bcm56990_combined_blocks[sub][0];
    ip_ctrl->level3_database_sel_block[0] = blocks[0];
    ip_ctrl->level3_database_sel_block[1] = blocks[1];
    ip_ctrl->level3_database_sel_block[2] = blocks[2];
    ip_ctrl->level3_database_sel_block[3] = blocks[3];

    ip_ctrl->level3_key_input_sel_block[0] = K8;
    ip_ctrl->level3_key_input_sel_block[1] = K8;
    ip_ctrl->level3_key_input_sel_block[2] = K8;
    ip_ctrl->level3_key_input_sel_block[3] = K8;

    /* Output DB sel configs */
    ip_ctrl->data_sel[0] = IPDB0;
    ip_ctrl->data_sel[1] = IPDBX;
    ip_ctrl->data_sel[2] = IPDBX;
    ip_ctrl->data_sel[3] = IPDBX;
    ip_ctrl->result_mode = FOUR_RESULTS_MODE;

    bcm56990_a0_cth_alpm_combined_check_and_set(u, ctrl, ctrl);

}

static void
bcm56990_a0_cth_alpm_parallel_init(int u,
                                   bcmptm_cth_alpm_control_t *ctrl,
                                   bcm56990_lpm_ip_control_t *ip_ctrl)
{
    uint8_t sub;
    uint8_t *blocks;

    sub = ((ctrl->db0_levels >= 2) << 1) | (ctrl->db1_levels >= 2);
    blocks = &bcm56990_parallel_l2_blocks[sub][0];
    ip_ctrl->level2_database_sel_block[0] = blocks[0];
    ip_ctrl->level2_database_sel_block[1] = blocks[1];
    ip_ctrl->level2_database_sel_block[2] = blocks[2];
    ip_ctrl->level2_database_sel_block[3] = blocks[3];

    ip_ctrl->level2_key_input_sel_block[0] = K8;
    ip_ctrl->level2_key_input_sel_block[1] = K8;
    ip_ctrl->level2_key_input_sel_block[2] = K8;
    ip_ctrl->level2_key_input_sel_block[3] = K8;

    sub = ((ctrl->db0_levels >= 3) << 1) | (ctrl->db1_levels >= 3);
    blocks = &bcm56990_parallel_l3_blocks[sub][0];
    ip_ctrl->level3_database_sel_block[0] = blocks[0];
    ip_ctrl->level3_database_sel_block[1] = blocks[1];
    ip_ctrl->level3_database_sel_block[2] = blocks[2];
    ip_ctrl->level3_database_sel_block[3] = blocks[3];

    ip_ctrl->level3_key_input_sel_block[0] = K8;
    ip_ctrl->level3_key_input_sel_block[1] = K8;
    ip_ctrl->level3_key_input_sel_block[2] = K8;
    ip_ctrl->level3_key_input_sel_block[3] = K8;

    /* Output DB sel configs */
    ip_ctrl->data_sel[0] = IPDB0;
    ip_ctrl->data_sel[1] = IPDB1;
    ip_ctrl->data_sel[2] = IPDBX;
    ip_ctrl->data_sel[3] = IPDBX;
    ip_ctrl->result_mode = THREE_RESULTS_MODE;
    bcm56990_a0_cth_alpm_parallel_check_and_set(u, ctrl, ctrl);
}

static void
bcm56990_a0_cth_alpm_combined_rpf_init(int u,
                                       bcmptm_cth_alpm_control_t *ctrl,
                                       bcm56990_lpm_ip_control_t *ip_ctrl)
{
    uint8_t sub;
    uint8_t *blocks;

    sub = ((ctrl->db0_levels >= 2) << 1) | (ctrl->db2_levels >= 2);
    blocks = &bcm56990_combined_rpf_l2_blocks[sub][0];
    ip_ctrl->level2_database_sel_block[0] = blocks[0];
    ip_ctrl->level2_database_sel_block[1] = blocks[1];
    ip_ctrl->level2_database_sel_block[2] = blocks[2];
    ip_ctrl->level2_database_sel_block[3] = blocks[3];

    ip_ctrl->level2_key_input_sel_block[0] = KEY_INPUT_MAP1(blocks[0]);
    ip_ctrl->level2_key_input_sel_block[1] = KEY_INPUT_MAP1(blocks[1]);
    ip_ctrl->level2_key_input_sel_block[2] = KEY_INPUT_MAP1(blocks[2]);
    ip_ctrl->level2_key_input_sel_block[3] = KEY_INPUT_MAP1(blocks[3]);


    sub = ((ctrl->db0_levels >= 3) << 1) | (ctrl->db2_levels >= 3);
    blocks = &bcm56990_combined_rpf_l3_blocks[sub][0];
    ip_ctrl->level3_database_sel_block[0] = blocks[0];
    ip_ctrl->level3_database_sel_block[1] = blocks[1];
    ip_ctrl->level3_database_sel_block[2] = blocks[2];
    ip_ctrl->level3_database_sel_block[3] = blocks[3];

    ip_ctrl->level3_key_input_sel_block[0] = KEY_INPUT_MAP1(blocks[0]);
    ip_ctrl->level3_key_input_sel_block[1] = KEY_INPUT_MAP1(blocks[1]);
    ip_ctrl->level3_key_input_sel_block[2] = KEY_INPUT_MAP1(blocks[2]);
    ip_ctrl->level3_key_input_sel_block[3] = KEY_INPUT_MAP1(blocks[3]);

    /* Output DB sel configs */
    ip_ctrl->data_sel[0] = IPDB0;
    ip_ctrl->data_sel[1] = IPDB2;
    ip_ctrl->data_sel[2] = IPDBX;
    ip_ctrl->data_sel[3] = IPDBX;
    ip_ctrl->result_mode = FOUR_RESULTS_MODE;
    bcm56990_a0_cth_alpm_combined_rpf_check_and_set(u, ctrl, ctrl);
}

static void
bcm56990_a0_cth_alpm_parallel_rpf_init(int u,
                                       bcmptm_cth_alpm_control_t *ctrl,
                                       bcm56990_lpm_ip_control_t *ip_ctrl)
{
    /*
     * In RPF & PARALLEL mode, we assume
     * 1. DB0 - LPM_DST Private - block 0
     * 2. DB1 - LPM_DST Public  - block 1
     * 3. DB2 - LPM_SRC Private - block 2
     * 4. DB3 - LPM_SRC Public  - block 3
     */

    uint8_t sub;
    uint8_t *blocks;


    sub = ((ctrl->db0_levels >= 2) << 3) |
          ((ctrl->db1_levels >= 2) << 2) |
          ((ctrl->db2_levels >= 2) << 1) |
           (ctrl->db3_levels >= 2);
    blocks = &bcm56990_parallel_rpf_blocks[sub][0];
    ip_ctrl->level2_database_sel_block[0] = blocks[0];
    ip_ctrl->level2_database_sel_block[1] = blocks[1];
    ip_ctrl->level2_database_sel_block[2] = blocks[2];
    ip_ctrl->level2_database_sel_block[3] = blocks[3];

    ip_ctrl->level2_key_input_sel_block[0] = KEY_INPUT_MAP2(blocks[0]);
    ip_ctrl->level2_key_input_sel_block[1] = KEY_INPUT_MAP2(blocks[1]);
    ip_ctrl->level2_key_input_sel_block[2] = KEY_INPUT_MAP2(blocks[2]);
    ip_ctrl->level2_key_input_sel_block[3] = KEY_INPUT_MAP2(blocks[3]);

    sub = ((ctrl->db0_levels >= 3) << 3) |
          ((ctrl->db1_levels >= 3) << 2) |
          ((ctrl->db2_levels >= 3) << 1) |
           (ctrl->db3_levels >= 3);
    blocks = &bcm56990_parallel_rpf_blocks[sub][0];
    ip_ctrl->level3_database_sel_block[0] = blocks[0];
    ip_ctrl->level3_database_sel_block[1] = blocks[1];
    ip_ctrl->level3_database_sel_block[2] = blocks[2];
    ip_ctrl->level3_database_sel_block[3] = blocks[3];

    ip_ctrl->level3_key_input_sel_block[0] = KEY_INPUT_MAP2(blocks[0]);
    ip_ctrl->level3_key_input_sel_block[1] = KEY_INPUT_MAP2(blocks[1]);
    ip_ctrl->level3_key_input_sel_block[2] = KEY_INPUT_MAP2(blocks[2]);
    ip_ctrl->level3_key_input_sel_block[3] = KEY_INPUT_MAP2(blocks[3]);

    /* Output DB sel configs */
    ip_ctrl->data_sel[0] = IPDB0;
    ip_ctrl->data_sel[1] = IPDB1;
    ip_ctrl->data_sel[2] = IPDB2;
    ip_ctrl->data_sel[3] = IPDB3;
    ip_ctrl->result_mode = TWO_RESULTS_MODE;
    bcm56990_a0_cth_alpm_parallel_rpf_check_and_set(u, ctrl, ctrl);
}


static void
bcm56990_a0_cth_alpm_fp_combined_init(int u,
                                      bcmptm_cth_alpm_control_t *ctrl,
                                      bcm56990_lpm_ip_control_t *ip_ctrl)
{
    /*
     * 1. DB0 - LPM_DST      - block 0/1
     * 2. DB2 - FP_COMP DST  - block 2
     * 3. DB3 - FP_COMP SRC  - block 3
     */

    uint8_t sub;
    uint8_t *blocks;

    sub = ((ctrl->db0_levels >= 2) << 2) |
          ((ctrl->db2_levels >= 2) << 1) |
           (ctrl->db3_levels >= 2);
    blocks = &bcm56990_fp_combined_blocks[sub][0];
    ip_ctrl->level2_database_sel_block[0] = blocks[0];
    ip_ctrl->level2_database_sel_block[1] = blocks[1];
    ip_ctrl->level2_database_sel_block[2] = blocks[2];
    ip_ctrl->level2_database_sel_block[3] = blocks[3];
    ip_ctrl->level2_key_input_sel_block[0] = KEY_INPUT_MAP3(blocks[0]);
    ip_ctrl->level2_key_input_sel_block[1] = KEY_INPUT_MAP3(blocks[1]);
    ip_ctrl->level2_key_input_sel_block[2] = KEY_INPUT_MAP3(blocks[2]);
    ip_ctrl->level2_key_input_sel_block[3] = KEY_INPUT_MAP3(blocks[3]);

    sub = ((ctrl->db0_levels >= 3) << 2) |
          ((ctrl->db2_levels >= 3) << 1) |
           (ctrl->db3_levels >= 3);
    blocks = &bcm56990_fp_combined_blocks[sub][0];
    ip_ctrl->level3_database_sel_block[0] = blocks[0];
    ip_ctrl->level3_database_sel_block[1] = blocks[1];
    ip_ctrl->level3_database_sel_block[2] = blocks[2];
    ip_ctrl->level3_database_sel_block[3] = blocks[3];
    ip_ctrl->level3_key_input_sel_block[0] = KEY_INPUT_MAP3(blocks[0]);
    ip_ctrl->level3_key_input_sel_block[1] = KEY_INPUT_MAP3(blocks[1]);
    ip_ctrl->level3_key_input_sel_block[2] = KEY_INPUT_MAP3(blocks[2]);
    ip_ctrl->level3_key_input_sel_block[3] = KEY_INPUT_MAP3(blocks[3]);


    /* Output DB sel configs */
    ip_ctrl->data_sel[0] = IPDB0;
    ip_ctrl->data_sel[1] = IPDBX;
    ip_ctrl->data_sel[2] = IPDB2;
    ip_ctrl->data_sel[3] = IPDB3;
    ip_ctrl->result_mode = FOUR_RESULTS_MODE;
    bcm56990_a0_cth_alpm_fp_combined_check_and_set(u, ctrl, ctrl);
}

static void
bcm56990_a0_cth_alpm_fp_parallel_init(int u,
                                      bcmptm_cth_alpm_control_t *ctrl,
                                      bcm56990_lpm_ip_control_t *ip_ctrl)
{
    /*
     * 1. DB0 - LPM_DST 0    - block 0
     * 3. DB1 - LPM_DST 1    - block 1
     * 3. DB2 - FP_COMP DST  - block 2
     * 4. DB3 - FP_COMP SRC  - block 3
     */

    uint8_t sub;
    uint8_t *blocks;

    sub = ((ctrl->db0_levels >= 2) << 3) |
          ((ctrl->db1_levels >= 2) << 2) |
          ((ctrl->db2_levels >= 2) << 1) |
           (ctrl->db3_levels >= 2);
    blocks = &bcm56990_fp_parallel_blocks[sub][0];
    ip_ctrl->level2_database_sel_block[0] = blocks[0];
    ip_ctrl->level2_database_sel_block[1] = blocks[1];
    ip_ctrl->level2_database_sel_block[2] = blocks[2];
    ip_ctrl->level2_database_sel_block[3] = blocks[3];
    ip_ctrl->level2_key_input_sel_block[0] = KEY_INPUT_MAP4(blocks[0]);
    ip_ctrl->level2_key_input_sel_block[1] = KEY_INPUT_MAP4(blocks[1]);
    ip_ctrl->level2_key_input_sel_block[2] = KEY_INPUT_MAP4(blocks[2]);
    ip_ctrl->level2_key_input_sel_block[3] = KEY_INPUT_MAP4(blocks[3]);

    sub = ((ctrl->db0_levels >= 3) << 3) |
          ((ctrl->db1_levels >= 3) << 2) |
          ((ctrl->db2_levels >= 3) << 1) |
           (ctrl->db3_levels >= 3);
    blocks = &bcm56990_fp_parallel_blocks[sub][0];
    ip_ctrl->level3_database_sel_block[0] = blocks[0];
    ip_ctrl->level3_database_sel_block[1] = blocks[1];
    ip_ctrl->level3_database_sel_block[2] = blocks[2];
    ip_ctrl->level3_database_sel_block[3] = blocks[3];
    ip_ctrl->level3_key_input_sel_block[0] = KEY_INPUT_MAP4(blocks[0]);
    ip_ctrl->level3_key_input_sel_block[1] = KEY_INPUT_MAP4(blocks[1]);
    ip_ctrl->level3_key_input_sel_block[2] = KEY_INPUT_MAP4(blocks[2]);
    ip_ctrl->level3_key_input_sel_block[3] = KEY_INPUT_MAP4(blocks[3]);

    /* Output DB sel configs */
    ip_ctrl->data_sel[0] = IPDB0;
    ip_ctrl->data_sel[1] = IPDB1;
    ip_ctrl->data_sel[2] = IPDB2;
    ip_ctrl->data_sel[3] = IPDB3;
    ip_ctrl->result_mode = THREE_RESULTS_MODE;
    bcm56990_a0_cth_alpm_fp_parallel_check_and_set(u, ctrl, ctrl);
}

static void
bcm56990_a0_cth_alpm_fp_combined_rpf_init(int u,
                                          bcmptm_cth_alpm_control_t *ctrl,
                                          bcm56990_lpm_ip_control_t *ip_ctrl)
{

    /*
     * 1. DB0 - LPM_DST      - block 0
     * 3. DB1 - LPM_SRC      - block 1
     * 3. DB2 - FP_COMP DST  - block 2
     * 4. DB3 - FP_COMP SRC  - block 3
     */

    uint8_t sub;
    uint8_t *blocks;

    sub = ((ctrl->db0_levels >= 2) << 3) |
          ((ctrl->db1_levels >= 2) << 2) |
          ((ctrl->db2_levels >= 2) << 1) |
           (ctrl->db3_levels >= 2);
    blocks = &bcm56990_fp_combined_rpf_blocks[sub][0];
    ip_ctrl->level2_database_sel_block[0] = blocks[0];
    ip_ctrl->level2_database_sel_block[1] = blocks[1];
    ip_ctrl->level2_database_sel_block[2] = blocks[2];
    ip_ctrl->level2_database_sel_block[3] = blocks[3];
    ip_ctrl->level2_key_input_sel_block[0] = KEY_INPUT_MAP5(blocks[0]);
    ip_ctrl->level2_key_input_sel_block[1] = KEY_INPUT_MAP5(blocks[1]);
    ip_ctrl->level2_key_input_sel_block[2] = KEY_INPUT_MAP5(blocks[2]);
    ip_ctrl->level2_key_input_sel_block[3] = KEY_INPUT_MAP5(blocks[3]);

    sub = ((ctrl->db0_levels >= 3) << 3) |
          ((ctrl->db1_levels >= 3) << 2) |
          ((ctrl->db2_levels >= 3) << 1) |
           (ctrl->db3_levels >= 3);
    blocks = &bcm56990_fp_combined_rpf_blocks[sub][0];
    ip_ctrl->level3_database_sel_block[0] = blocks[0];
    ip_ctrl->level3_database_sel_block[1] = blocks[1];
    ip_ctrl->level3_database_sel_block[2] = blocks[2];
    ip_ctrl->level3_database_sel_block[3] = blocks[3];
    ip_ctrl->level3_key_input_sel_block[0] = KEY_INPUT_MAP5(blocks[0]);
    ip_ctrl->level3_key_input_sel_block[1] = KEY_INPUT_MAP5(blocks[1]);
    ip_ctrl->level3_key_input_sel_block[2] = KEY_INPUT_MAP5(blocks[2]);
    ip_ctrl->level3_key_input_sel_block[3] = KEY_INPUT_MAP5(blocks[3]);

    /* Output DB sel configs */
    ip_ctrl->data_sel[0] = IPDB0;
    ip_ctrl->data_sel[1] = IPDB1;
    ip_ctrl->data_sel[2] = IPDB2;
    ip_ctrl->data_sel[3] = IPDB3;
    ip_ctrl->result_mode = FOUR_RESULTS_MODE;
    bcm56990_a0_cth_alpm_fp_combined_rpf_check_and_set(u, ctrl, ctrl);
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
bcm56990_a0_cth_alpm_ip_ctrl_init(int u,
                                  bcmptm_cth_alpm_control_t *ctrl,
                                  bcm56990_lpm_ip_control_t *ip_ctrl,
                                  bcm56990_lpm_lane_control_t *lane_ctrl)
{

    /*
     * All supported settings.
     *
     * - Lane 0 Combined
     *  +-----+---------+------------+
     *  | DB0 | LPM DST | LPM + L3MC |
     *  +-----+---------+------------+
     *
     * - Lane 0 Parallel
     *  +-----+---------+------------+
     *  | DB0 | LPM DST | LPM + L3MC |
     *  | DB1 | LPM DST | LPM        |
     *  +-----+---------+------------+
     *
     * - Lane 0/1 Combined + RPF
     *  +-----+---------+------------+
     *  | DB0 | LPM DST | LPM + L3MC |
     *  +-----+---------+------------+
     *  | DB2 | LPM SRC | LPM        |
     *  +-----+---------+------------+
     *
     * - Lane 0/1 Parallel + RPF
     *  +-----+---------+------------+
     *  | DB0 | LPM DST | LPM + L3MC |
     *  | DB1 | LPM DST | LPM        |
     *  +-----+---------+------------+
     *  | DB2 | LPM SRC | LPM + L3MC |
     *  | DB3 | LPM SRC | LPM        |
     *  +-----+---------+------------+
     *
     * - Lane 0/2/3 Combined + FP
     *  +-----+---------+------------+
     *  | DB0 | LPM DST | LPM + L3MC |
     *  +-----+---------+------------+
     *  | DB2 | FP DST  | FP         |
     *  +-----+---------+------------+
     *  | DB3 | FP SRC  | FP         |
     *  +-----+---------+------------+
     *
     * - Lane 0/2/3 Parallel + FP
     *  +-----+---------+------------+
     *  | DB0 | LPM DST | LPM + L3MC |
     *  | DB1 | LPM DST | LPM        |
     *  +-----+---------+------------+
     *  | DB2 | FP DST  | FP         |
     *  +-----+---------+------------+
     *  | DB3 | FP SRC  | FP         |
     *  +-----+---------+------------+
     *
     * - Lane 0/1/2/3 Combined + RPF + FP
     *  +-----+---------+------------+
     *  | DB0 | LPM DST | LPM + L3MC |
     *  +-----+---------+------------+
     *  | DB1 | LPM SRC | LPM        |
     *  +-----+---------+------------+
     *  | DB2 | FP DST  | FP         |
     *  +-----+---------+------------+
     *  | DB3 | FP SRC  | FP         |
     *  +-----+---------+------------+
 */
    bcm56990_a0_cth_alpm_l1_parse_check(u, &ctrl->l1_key_input[0],
                                        &ctrl->l1_db[0],
                                        ctrl->db0_levels,
                                        ctrl->db1_levels,
                                        ctrl->db2_levels,
                                        ctrl->db3_levels,
                                        &ctrl->alpm_mode,
                                        &ctrl->alpm_dbs);

    /* Level-1 key-input sel block */
    ip_ctrl->level1_key_input_sel_block[0] = ctrl->l1_phy_key_input[0];
    ip_ctrl->level1_key_input_sel_block[1] = ctrl->l1_phy_key_input[1];
    ip_ctrl->level1_key_input_sel_block[2] = ctrl->l1_phy_key_input[2];
    ip_ctrl->level1_key_input_sel_block[3] = ctrl->l1_phy_key_input[3];
    ip_ctrl->level1_key_input_sel_block[4] = ctrl->l1_phy_key_input[4];
    ip_ctrl->level1_key_input_sel_block[5] = ctrl->l1_phy_key_input[5];
    ip_ctrl->level1_key_input_sel_block[6] = ctrl->l1_phy_key_input[6];
    ip_ctrl->level1_key_input_sel_block[7] = ctrl->l1_phy_key_input[7];

    /* Level-1 database sel block */
    ip_ctrl->level1_database_sel_block[0] = ctrl->l1_phy_db[0];
    ip_ctrl->level1_database_sel_block[1] = ctrl->l1_phy_db[1];
    ip_ctrl->level1_database_sel_block[2] = ctrl->l1_phy_db[2];
    ip_ctrl->level1_database_sel_block[3] = ctrl->l1_phy_db[3];
    ip_ctrl->level1_database_sel_block[4] = ctrl->l1_phy_db[4];
    ip_ctrl->level1_database_sel_block[5] = ctrl->l1_phy_db[5];
    ip_ctrl->level1_database_sel_block[6] = ctrl->l1_phy_db[6];
    ip_ctrl->level1_database_sel_block[7] = ctrl->l1_phy_db[7];

    lane_ctrl->l3_dst_enable = 1;
    if (ctrl->alpm_mode & ALPM_MODE_FP_COMP) {
        lane_ctrl->comp_src_enable = 1;
        lane_ctrl->comp_dst_enable = 1;
        if (ctrl->alpm_mode & ALPM_MODE_RPF) {
            bcm56990_a0_cth_alpm_fp_combined_rpf_init(u, ctrl, ip_ctrl);
            lane_ctrl->l3_src_enable = 1;
        } else if (ctrl->alpm_mode & ALPM_MODE_PARALLEL) {
            bcm56990_a0_cth_alpm_fp_parallel_init(u, ctrl, ip_ctrl);
        } else {
            bcm56990_a0_cth_alpm_fp_combined_init(u, ctrl, ip_ctrl);
        }
    } else {
        if (ctrl->alpm_mode & ALPM_MODE_RPF) {
            if (ctrl->alpm_mode & ALPM_MODE_PARALLEL) {
                bcm56990_a0_cth_alpm_parallel_rpf_init(u, ctrl, ip_ctrl);
            } else {
                bcm56990_a0_cth_alpm_combined_rpf_init(u, ctrl, ip_ctrl);
            }
            lane_ctrl->l3_src_enable = 1;
        } else {
            if (ctrl->alpm_mode & ALPM_MODE_PARALLEL) {
                bcm56990_a0_cth_alpm_parallel_init(u, ctrl, ip_ctrl);
            } else {
                bcm56990_a0_cth_alpm_combined_init(u, ctrl, ip_ctrl);
            }
        }
    }

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
    ctrl->tot.l2_bucket_bits = 10;
    ctrl->db0.l2_bucket_bits = ctrl->db0_levels ? 10 : 0;
    ctrl->db1.l2_bucket_bits = ctrl->db1_levels ? 10 : 0;
    ctrl->db2.l2_bucket_bits = ctrl->db2_levels ? 10 : 0;
    ctrl->db3.l2_bucket_bits = ctrl->db3_levels ? 10 : 0;

    ctrl->tot.l3_bucket_bits = 13;
    ctrl->db0.l3_bucket_bits = ctrl->db0_levels ? 13 : 0;
    ctrl->db1.l3_bucket_bits = ctrl->db1_levels ? 13 : 0;
    ctrl->db2.l3_bucket_bits = ctrl->db2_levels ? 13 : 0;
    ctrl->db3.l3_bucket_bits = ctrl->db3_levels ? 13 : 0;
}

static int
bcm56990_a0_cth_alpm_control_encode(int u, int mtop, bcmptm_cth_alpm_control_t *ctrl,
                                    cth_alpm_control_pt_t *pts)
{
    LPM_IP_CONTROLm_t ce;
    LPM_LANE_CTRLr_t le;
    bcm56990_lpm_ip_control_t lpm_ctrl;
    bcm56990_lpm_lane_control_t lane_ctrl;

    SHR_FUNC_ENTER(u);
    SHR_NULL_CHECK(pts, SHR_E_PARAM);
    SHR_NULL_CHECK(ctrl, SHR_E_PARAM);

    pts->count = 2;

    /* If entry/sid pointer is null, simply return count. */
    if (!pts->entry || !pts->sid) {
        SHR_EXIT();
    }

    sal_memset(&ce, 0, sizeof(ce));
    sal_memset(&le, 0, sizeof(le));
    sal_memset(&lpm_ctrl, 0, sizeof(lpm_ctrl));
    sal_memset(&lane_ctrl, 0, sizeof(lane_ctrl));

    /* Level-1 Configurations */
    bcm56990_a0_cth_alpm_ip_ctrl_init(u, ctrl, &lpm_ctrl, &lane_ctrl);


    /* Level-1 Configurations */
    CONTROL_FIELD_SET(LEVEL1_DATABASE_SEL_BLOCK_0f,
                      lpm_ctrl.level1_database_sel_block[0]);
    CONTROL_FIELD_SET(LEVEL1_DATABASE_SEL_BLOCK_1f,
                      lpm_ctrl.level1_database_sel_block[1]);
    CONTROL_FIELD_SET(LEVEL1_DATABASE_SEL_BLOCK_2f,
                      lpm_ctrl.level1_database_sel_block[2]);
    CONTROL_FIELD_SET(LEVEL1_DATABASE_SEL_BLOCK_3f,
                      lpm_ctrl.level1_database_sel_block[3]);
    CONTROL_FIELD_SET(LEVEL1_DATABASE_SEL_BLOCK_4f,
                      lpm_ctrl.level1_database_sel_block[4]);
    CONTROL_FIELD_SET(LEVEL1_DATABASE_SEL_BLOCK_5f,
                      lpm_ctrl.level1_database_sel_block[5]);
    CONTROL_FIELD_SET(LEVEL1_DATABASE_SEL_BLOCK_6f,
                      lpm_ctrl.level1_database_sel_block[6]);
    CONTROL_FIELD_SET(LEVEL1_DATABASE_SEL_BLOCK_7f,
                      lpm_ctrl.level1_database_sel_block[7]);

    CONTROL_FIELD_SET(LEVEL1_KEY_INPUT_SEL_BLOCK_0f,
                      lpm_ctrl.level1_key_input_sel_block[0]);
    CONTROL_FIELD_SET(LEVEL1_KEY_INPUT_SEL_BLOCK_1f,
                      lpm_ctrl.level1_key_input_sel_block[1]);
    CONTROL_FIELD_SET(LEVEL1_KEY_INPUT_SEL_BLOCK_2f,
                      lpm_ctrl.level1_key_input_sel_block[2]);
    CONTROL_FIELD_SET(LEVEL1_KEY_INPUT_SEL_BLOCK_3f,
                      lpm_ctrl.level1_key_input_sel_block[3]);
    CONTROL_FIELD_SET(LEVEL1_KEY_INPUT_SEL_BLOCK_4f,
                      lpm_ctrl.level1_key_input_sel_block[4]);
    CONTROL_FIELD_SET(LEVEL1_KEY_INPUT_SEL_BLOCK_5f,
                      lpm_ctrl.level1_key_input_sel_block[5]);
    CONTROL_FIELD_SET(LEVEL1_KEY_INPUT_SEL_BLOCK_6f,
                      lpm_ctrl.level1_key_input_sel_block[6]);
    CONTROL_FIELD_SET(LEVEL1_KEY_INPUT_SEL_BLOCK_7f,
                      lpm_ctrl.level1_key_input_sel_block[7]);

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

    LPM_LANE_CTRLr_COMP_SRC_ENABLEf_SET(le, lane_ctrl.comp_src_enable);
    LPM_LANE_CTRLr_COMP_DST_ENABLEf_SET(le, lane_ctrl.comp_dst_enable);
    LPM_LANE_CTRLr_L3_SRC_ENABLEf_SET(le, lane_ctrl.l3_src_enable);
    LPM_LANE_CTRLr_L3_DST_ENABLEf_SET(le, lane_ctrl.l3_dst_enable);

    pts->sid[0] = LPM_IP_CONTROLm;
    pts->sid[1] = LPM_LANE_CTRLr;
    sal_memcpy(pts->entry[0], &ce, sizeof(ce));
    sal_memcpy(pts->entry[1], &le, sizeof(le));

exit:
    SHR_FUNC_EXIT();
}


static int
bcm56990_a0_cth_alpm_control_validate_stage1(int u,
                                             bcmptm_cth_alpm_control_t *ctrl)
{

    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(
        bcm56990_a0_cth_alpm_l1_parse_check(u, &ctrl->l1_key_input[0],
                                            &ctrl->l1_db[0],
                                            ctrl->db0_levels,
                                            ctrl->db1_levels,
                                            ctrl->db2_levels,
                                            ctrl->db3_levels,
                                            NULL,
                                            NULL));
exit:
    SHR_FUNC_EXIT();
}
static bool
bcm56990_a0_cth_alpm_control_validate_stage2(int u,
                                             bcmptm_cth_alpm_control_t *ctrl)
{
    bool ok;
    uint8_t i;
    uint32_t bitmap_base;
    int rv;

    /* Must be consecutive 1s of l2 banks */
    bitmap_base = (1 << ctrl->tot.num_l2_banks) - 1;
    ok = false;
    for (i = 0; i <= 6 - ctrl->tot.num_l2_banks; i++) {
        if (ctrl->tot.l2_bank_bitmap == bitmap_base << i) {
            ok = true;
            break;
        }
    }
    if (!ok) {
        return false;
    }

    /* Must be consecutive 1s of l3 banks */
    bitmap_base = (1 << ctrl->tot.num_l3_banks) - 1;
    ok = false;
    for (i = 0; i <= 8 - ctrl->tot.num_l3_banks; i++) {
        if (ctrl->tot.l3_bank_bitmap == bitmap_base << i) {
            ok = true;
            break;
        }
    }
    if (!ok) {
        return false;
    }

    bcm56990_a0_cth_alpm_l1_parse_check(u, &ctrl->l1_key_input[0],
                                        &ctrl->l1_db[0],
                                        ctrl->db0_levels,
                                        ctrl->db1_levels,
                                        ctrl->db2_levels,
                                        ctrl->db3_levels,
                                        &ctrl->alpm_mode,
                                        &ctrl->alpm_dbs);

    /* Level-2 database sel block */
    if (ctrl->alpm_mode & ALPM_MODE_FP_COMP) {
        if (ctrl->alpm_mode & ALPM_MODE_RPF) {
            rv = bcm56990_a0_cth_alpm_fp_combined_rpf_check_and_set(u, ctrl, NULL);
        } else if (ctrl->alpm_mode & ALPM_MODE_PARALLEL) {
            rv = bcm56990_a0_cth_alpm_fp_parallel_check_and_set(u, ctrl, NULL);
        } else {
            rv = bcm56990_a0_cth_alpm_fp_combined_check_and_set(u, ctrl, NULL);
        }
    } else {
        if (ctrl->alpm_mode & ALPM_MODE_RPF) {
            if (ctrl->alpm_mode & ALPM_MODE_PARALLEL) {
                rv = bcm56990_a0_cth_alpm_parallel_rpf_check_and_set(u, ctrl, NULL);
            } else {
                rv = bcm56990_a0_cth_alpm_combined_rpf_check_and_set(u, ctrl, NULL);
            }
        } else {
            if (ctrl->alpm_mode & ALPM_MODE_PARALLEL) {
                rv = bcm56990_a0_cth_alpm_parallel_check_and_set(u, ctrl, NULL);
            } else {
                rv = bcm56990_a0_cth_alpm_combined_check_and_set(u, ctrl, NULL);
            }
        }
    }
    if (SHR_FAILURE(rv)) {
        return false;
    }

    return ok;

}

static int
bcm56990_a0_cth_alpm_control_validate(int u, int mtop, uint8_t stage,
                                      bcmptm_cth_alpm_control_t *ctrl)
{
    SHR_FUNC_ENTER(u);
    switch (stage) {
    case 1:
        SHR_IF_ERR_EXIT
            (bcm56990_a0_cth_alpm_control_validate_stage1(u, ctrl));
        break;

    case 2:
    default:
        if (!bcm56990_a0_cth_alpm_control_validate_stage2(u, ctrl)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        break;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_cth_alpm_device_info_init(int u, int m, cth_alpm_device_info_t *dev)
{
    bcmptm_cth_alpm_control_t *ctrl;

    SHR_FUNC_ENTER(u);
    if (dev) {
        int i;
        ctrl = &dev->spec.default_control;
        dev->spec.tcam_blocks = 16;
        dev->spec.tcam_depth = 128;
        dev->feature_pkms = 11;
        dev->feature_urpf = 0; /* no urpf support for TH4 */
        dev->spec.l1_blocks = 8;
        dev->spec.l2_sid[0] = L3_DEFIP_ALPM_LEVEL2m;
        dev->spec.l3_sid[0] = L3_DEFIP_ALPM_LEVEL3m;
        for (i = 1; i < ALPM_LN_BANKS_MAX; i++) {
            dev->spec.l2_sid[i] = INVALIDm;
            dev->spec.l3_sid[i] = INVALIDm;
        }
        ctrl->alpm_mode = ALPM_MODE_COMBINED;
        ctrl->large_vrf = 0;
        ctrl->comp_key_type = COMP_KEY_L4_PORT;
        ctrl->db0_levels = 2;
        ctrl->db1_levels = 0;
        ctrl->db2_levels = 0;
        ctrl->db3_levels = 0;
        ctrl->l1_phy_key_input[0] = KEY_INPUT_LPM_L3MC_D;
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
bcm56990_a0_cth_alpm_driver_register(int u, bcmptm_cth_alpm_driver_t *drv)
{
    SHR_FUNC_ENTER(u);

    if (drv) {
        drv->control_encode = bcm56990_a0_cth_alpm_control_encode;
        drv->control_validate = bcm56990_a0_cth_alpm_control_validate;
        drv->device_info_init = bcm56990_a0_cth_alpm_device_info_init;
    } else {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}


