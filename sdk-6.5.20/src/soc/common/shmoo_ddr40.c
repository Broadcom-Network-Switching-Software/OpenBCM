/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DDR3 Memory support
 */

#include <shared/bsl.h>

#include <sal/core/boot.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <soc/memtune.h>
#include <soc/mcm/memregs.h>

#include <soc/drv.h>
#include <soc/cm.h>
#include <soc/mem.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/dpp_config_defs.h>
#endif

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/ARAD/arad_dram.h>
#include <soc/dpp/ARAD/arad_api_mgmt.h>
#endif

#ifdef BCM_HURRICANE2_SUPPORT
#include <soc/iproc.h>
#endif 

#ifdef BCM_DDR3_SUPPORT
#include <soc/shmoo_ddr40.h>
#include <soc/phy/ddr40.h>

#define DDR_TIMEOUT_10mS 1000
#define NUM_DATA 8
#define SET_UNOVR_STEP(v) ( 0x20000 | ( (v) & 0x3F ) )  /* OVR_FORCE = OVR_EN = 1, OVR_STEP = v */
#define SET_OVR_STEP(v) ( 0x30000 | ( (v) & 0x3F ) )    /* OVR_FORCE = OVR_EN = 1, OVR_STEP = v */

#define RD_EN_DQS_CORROLATION 2
#define MAX_SHMOO_INTERFACES 32

/***********************************************************************
* DDR Initialization Functions
 */

static uint32 shmoo_dram_config = 0x00000000;
static uint32 dram0_clamshell_enable = 0;
static uint32 dram1_clamshell_enable = 0;



/* 
    drc_regs_table used to  find the correct register id for each DR
    this tabe must be coordinate with the enum drc_reg_set_e
*/

uint32 drc_regs_table[DRC_REG_COUNT][8] = {
{ DRCA_BIST_CONFIGURATIONSr, DRCB_BIST_CONFIGURATIONSr, DRCC_BIST_CONFIGURATIONSr, DRCD_BIST_CONFIGURATIONSr, DRCE_BIST_CONFIGURATIONSr, DRCF_BIST_CONFIGURATIONSr, DRCG_BIST_CONFIGURATIONSr, DRCH_BIST_CONFIGURATIONSr},
{ DRCA_BIST_END_ADDRESSr, DRCB_BIST_END_ADDRESSr, DRCC_BIST_END_ADDRESSr, DRCD_BIST_END_ADDRESSr, DRCE_BIST_END_ADDRESSr, DRCF_BIST_END_ADDRESSr, DRCG_BIST_END_ADDRESSr, DRCH_BIST_END_ADDRESSr},
{ DRCA_BIST_ERROR_OCCURREDr, DRCB_BIST_ERROR_OCCURREDr, DRCC_BIST_ERROR_OCCURREDr, DRCD_BIST_ERROR_OCCURREDr, DRCE_BIST_ERROR_OCCURREDr, DRCF_BIST_ERROR_OCCURREDr, DRCG_BIST_ERROR_OCCURREDr, DRCH_BIST_ERROR_OCCURREDr},
{ DRCA_BIST_NUMBER_OF_ACTIONSr, DRCB_BIST_NUMBER_OF_ACTIONSr, DRCC_BIST_NUMBER_OF_ACTIONSr, DRCD_BIST_NUMBER_OF_ACTIONSr, DRCE_BIST_NUMBER_OF_ACTIONSr, DRCF_BIST_NUMBER_OF_ACTIONSr, DRCG_BIST_NUMBER_OF_ACTIONSr, DRCH_BIST_NUMBER_OF_ACTIONSr},
{ DRCA_BIST_PATTERN_WORD_0r, DRCB_BIST_PATTERN_WORD_0r, DRCC_BIST_PATTERN_WORD_0r, DRCD_BIST_PATTERN_WORD_0r, DRCE_BIST_PATTERN_WORD_0r, DRCF_BIST_PATTERN_WORD_0r, DRCG_BIST_PATTERN_WORD_0r, DRCH_BIST_PATTERN_WORD_0r},
{ DRCA_BIST_PATTERN_WORD_1r, DRCB_BIST_PATTERN_WORD_1r, DRCC_BIST_PATTERN_WORD_1r, DRCD_BIST_PATTERN_WORD_1r, DRCE_BIST_PATTERN_WORD_1r, DRCF_BIST_PATTERN_WORD_1r, DRCG_BIST_PATTERN_WORD_1r, DRCH_BIST_PATTERN_WORD_1r},
{ DRCA_BIST_PATTERN_WORD_2r, DRCB_BIST_PATTERN_WORD_2r, DRCC_BIST_PATTERN_WORD_2r, DRCD_BIST_PATTERN_WORD_2r, DRCE_BIST_PATTERN_WORD_2r, DRCF_BIST_PATTERN_WORD_2r, DRCG_BIST_PATTERN_WORD_2r, DRCH_BIST_PATTERN_WORD_2r},
{ DRCA_BIST_PATTERN_WORD_3r, DRCB_BIST_PATTERN_WORD_3r, DRCC_BIST_PATTERN_WORD_3r, DRCD_BIST_PATTERN_WORD_3r, DRCE_BIST_PATTERN_WORD_3r, DRCF_BIST_PATTERN_WORD_3r, DRCG_BIST_PATTERN_WORD_3r, DRCH_BIST_PATTERN_WORD_3r},
{ DRCA_BIST_PATTERN_WORD_4r, DRCB_BIST_PATTERN_WORD_4r, DRCC_BIST_PATTERN_WORD_4r, DRCD_BIST_PATTERN_WORD_4r, DRCE_BIST_PATTERN_WORD_4r, DRCF_BIST_PATTERN_WORD_4r, DRCG_BIST_PATTERN_WORD_4r, DRCH_BIST_PATTERN_WORD_4r},
{ DRCA_BIST_PATTERN_WORD_5r, DRCB_BIST_PATTERN_WORD_5r, DRCC_BIST_PATTERN_WORD_5r, DRCD_BIST_PATTERN_WORD_5r, DRCE_BIST_PATTERN_WORD_5r, DRCF_BIST_PATTERN_WORD_5r, DRCG_BIST_PATTERN_WORD_5r, DRCH_BIST_PATTERN_WORD_5r},
{ DRCA_BIST_PATTERN_WORD_6r, DRCB_BIST_PATTERN_WORD_6r, DRCC_BIST_PATTERN_WORD_6r, DRCD_BIST_PATTERN_WORD_6r, DRCE_BIST_PATTERN_WORD_6r, DRCF_BIST_PATTERN_WORD_6r, DRCG_BIST_PATTERN_WORD_6r, DRCH_BIST_PATTERN_WORD_6r},
{ DRCA_BIST_PATTERN_WORD_7r, DRCB_BIST_PATTERN_WORD_7r, DRCC_BIST_PATTERN_WORD_7r, DRCD_BIST_PATTERN_WORD_7r, DRCE_BIST_PATTERN_WORD_7r, DRCF_BIST_PATTERN_WORD_7r, DRCG_BIST_PATTERN_WORD_7r, DRCH_BIST_PATTERN_WORD_7r},
{ DRCA_BIST_START_ADDRESSr, DRCB_BIST_START_ADDRESSr, DRCC_BIST_START_ADDRESSr, DRCD_BIST_START_ADDRESSr, DRCE_BIST_START_ADDRESSr, DRCF_BIST_START_ADDRESSr, DRCG_BIST_START_ADDRESSr, DRCH_BIST_START_ADDRESSr},
{ DRCA_BIST_STATUSESr, DRCB_BIST_STATUSESr, DRCC_BIST_STATUSESr, DRCD_BIST_STATUSESr, DRCE_BIST_STATUSESr, DRCF_BIST_STATUSESr, DRCG_BIST_STATUSESr, DRCH_BIST_STATUSESr},
{ DRCA_DPI_STATUSr, DRCB_DPI_STATUSr, DRCC_DPI_STATUSr, DRCD_DPI_STATUSr, DRCE_DPI_STATUSr, DRCF_DPI_STATUSr, DRCG_DPI_STATUSr, DRCH_DPI_STATUSr},
{ DRCA_DRAM_INIT_FINISHEDr, DRCB_DRAM_INIT_FINISHEDr, DRCC_DRAM_INIT_FINISHEDr, DRCD_DRAM_INIT_FINISHEDr, DRCE_DRAM_INIT_FINISHEDr, DRCF_DRAM_INIT_FINISHEDr, DRCG_DRAM_INIT_FINISHEDr, DRCH_DRAM_INIT_FINISHEDr}
};




const KATANAfreq_grade_mem_set_set_t KATANAfgmss[FREQ_COUNT][GRADE_COUNT] =
{
    { /* FREQ_400 */
        /* GRADE_DEFAULT */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_080808 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_090909 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_101010 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_111111 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_121212 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_131313 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_141414 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1}
    },

    { /* FREQ_500 */
        /* GRADE_DEFAULT */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_080808 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_090909 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_101010 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_111111 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_121212 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_131313 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_141414 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1}
    },

    { /* FREQ_533 */
        /* GRADE_DEFAULT */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_080808 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_090909 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_101010 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_111111 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_121212 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_131313 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_141414 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1}
    },

    { /* FREQ_667 */
        /* GRADE_DEFAULT */
        {10,  5, 17,    32, 48,      5,  8, 15, 11,     17, 24,  0,  0,     2600,   14, 18, {100, 54, 37},  272,     9,  7, 10,     16,  667,   0x1b50, 0x0210},
        /* GRADE_080808 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_090909 */
        {10,  5, 17,    32, 48,      5,  8, 15, 11,     17, 24,  0,  0,     2600,   14, 18, {100, 54, 37},  272,     9,  7, 10,     16,  667,   0x1b50, 0x0210},
        /* GRADE_101010 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_111111 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_121212 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_131313 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_141414 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1}
    },

    { /* FREQ_800 */
        /* GRADE_DEFAULT */
        {13,  6, 20,    32, 48,      6, 10, 16, 15,     20, 26,  0,  0,     3120,   16, 21, {120, 64, 44},  272,    11,  8, 12,     21,  800,   0x1d70, 0x0218},
        /* GRADE_080808 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_090909 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_101010 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_111111 */
        {13,  6, 20,    32, 48,      6, 10, 16, 15,     20, 26,  0,  0,     3120,   16, 21, {120, 64, 44},  272,    11,  8, 12,     21,  800,   0x1d70, 0x0218},
        /* GRADE_121212 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_131313 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_141414 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1}
    },

    { /* FREQ_914 */
        /* GRADE_DEFAULT */
        {16,  7, 22,    32, 48,     10, 14, 16, 19,     21, 29,  0,  0,     3565,   16, 26, {138, 74, 51},  304,    13,  9, 14,     25,  914,   0x1f14, 0x0220},
        /* GRADE_080808 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_090909 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_101010 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_111111 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_121212 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_131313 */
        {16,  7, 22,    32, 48,     10, 14, 16, 19,     21, 29,  0,  0,     3565,   16, 26, {138, 74, 51},  304,    13,  9, 14,     25,  914,   0x1f14, 0x0220},
        /* GRADE_141414 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1}
    },

    { /* FREQ_933 */
        /* GRADE_DEFAULT */
        {16,  7, 22,    32, 48,     10, 14, 17, 19,     21, 29,  0,  0,     3565,   16, 26, {138, 74, 51},  304,    13,  9, 14,     25,  933,   0x1f14, 0x0220},
        /* GRADE_080808 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_090909 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_101010 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_111111 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_121212 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_131313 */
        {16,  7, 22,    32, 48,     10, 14, 17, 19,     21, 29,  0,  0,     3565,   16, 26, {138, 74, 51},  304,    13,  9, 14,     25,  933,   0x1f14, 0x0220},
        /* GRADE_141414 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1}
    },

    { /* FREQ_1066 */
        /* GRADE_DEFAULT */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_080808 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_090909 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_101010 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_111111 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_121212 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_131313 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_141414 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1}
    }
};

uint32 DRC_REG_READ(int unit, uint32 channel, drc_reg_set_t base, uint32 *rvp)
{
    soc_reg_t reg = drc_regs_table[base][channel];

    return soc_reg32_get(unit, reg, REG_PORT_ANY, 0, rvp); 
} 

uint32 DRC_REG_WRITE(int unit, uint32 channel,  drc_reg_set_t base, uint32 rv)
{
    soc_reg_t reg = drc_regs_table[base][channel];
    
    return soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rv);
}

void
soc_ddr40_set_shmoo_dram_config(uint32 unit, uint32 dram_config)
{
    shmoo_dram_config = dram_config;
}

uint32
_check_dram(int ci)
{
    return (shmoo_dram_config & (0x1 << ci));
}

uint32
_check_dram_either(int ci)
{
    uint32 result;

    result = _check_dram(ci);

    if(ci & 0x1) {
        result |= _check_dram(ci - 1);
    } else {
        result |= _check_dram(ci + 1);
    }

    return result;
}

uint32
_check_dram_both(int ci)
{
    uint32 result;

    if(_check_dram(ci)){
        result = 1;
    } else {
        result = 0;
    }

    if(ci & 0x1) {
        if(!_check_dram(ci - 1)) {
            result = 0;
        }
    } else {
        if(!_check_dram(ci + 1)) {
            result = 0;
        }
    }

    return result;
}

void
_soc_ddr40_katana_phy_init_mem_set_wrapper(uint32 unit, KATANAfreq_grade_mem_set_set_t *fgmssPtr, int freq_loc, int grade_loc)
{
    fgmssPtr->twl = KATANAfgmss[freq_loc][grade_loc].twl;
    fgmssPtr->twr = KATANAfgmss[freq_loc][grade_loc].twr;
    fgmssPtr->trc = KATANAfgmss[freq_loc][grade_loc].trc;
    fgmssPtr->rfifo_afull = KATANAfgmss[freq_loc][grade_loc].rfifo_afull;
    fgmssPtr->rfifo_full = KATANAfgmss[freq_loc][grade_loc].rfifo_full;
    fgmssPtr->trtw = KATANAfgmss[freq_loc][grade_loc].trtw;
    fgmssPtr->twtr = KATANAfgmss[freq_loc][grade_loc].twtr;
    fgmssPtr->tfaw = KATANAfgmss[freq_loc][grade_loc].tfaw;
    fgmssPtr->tread_enb = soc_property_get(unit,spn_DDR3_TREAD_ENB, KATANAfgmss[freq_loc][grade_loc].tread_enb);
    fgmssPtr->bank_unavail_rd = soc_property_get(unit,spn_DDR3_BANK_UNAVAIL_RD, KATANAfgmss[freq_loc][grade_loc].bank_unavail_rd);
    fgmssPtr->bank_unavail_wr = soc_property_get(unit,spn_DDR3_BANK_UNAVAIL_WR, KATANAfgmss[freq_loc][grade_loc].bank_unavail_wr);
    fgmssPtr->rr_read = soc_property_get(unit,spn_DDR3_ROUND_ROBIN_READ, KATANAfgmss[freq_loc][grade_loc].rr_read);
    fgmssPtr->rr_write = soc_property_get(unit,spn_DDR3_ROUND_ROBIN_WRITE, KATANAfgmss[freq_loc][grade_loc].rr_write);
    fgmssPtr->refrate = soc_property_get(unit,spn_DDR3_REFRESH_INTVL_OVERRIDE, KATANAfgmss[freq_loc][grade_loc].refrate);
    fgmssPtr->trp_read = soc_property_get(unit,spn_DDR3_TRP_READ, KATANAfgmss[freq_loc][grade_loc].trp_read);
    fgmssPtr->trp_write = soc_property_get(unit,spn_DDR3_TRP_WRITE, KATANAfgmss[freq_loc][grade_loc].trp_write);
    fgmssPtr->trfc[MEM_4G] = KATANAfgmss[freq_loc][grade_loc].trfc[MEM_4G];
    fgmssPtr->trfc[MEM_2G] = KATANAfgmss[freq_loc][grade_loc].trfc[MEM_2G];
    fgmssPtr->trfc[MEM_1G] = KATANAfgmss[freq_loc][grade_loc].trfc[MEM_1G];
    fgmssPtr->tzq = KATANAfgmss[freq_loc][grade_loc].tzq;
    fgmssPtr->cl = KATANAfgmss[freq_loc][grade_loc].cl;
    fgmssPtr->cwl = KATANAfgmss[freq_loc][grade_loc].cwl;
    fgmssPtr->wr = KATANAfgmss[freq_loc][grade_loc].wr;
    fgmssPtr->jedec = KATANAfgmss[freq_loc][grade_loc].jedec;
    fgmssPtr->mhz = KATANAfgmss[freq_loc][grade_loc].mhz;
    fgmssPtr->mr0 = KATANAfgmss[freq_loc][grade_loc].mr0;
    fgmssPtr->mr2 = KATANAfgmss[freq_loc][grade_loc].mr2;
}

#if 0 /* Uncomment this when required */
void
_soc_ddr40_arad_phy_init_mem_set_wrapper(uint32 unit, ARADfreq_grade_mem_set_set_t *fgmssPtr, int freq_loc, int grade_loc)
{
    fgmssPtr->Jedec = ARADfgmss[freq_loc][grade_loc].Jedec;
    fgmssPtr->Cl = ARADfgmss[freq_loc][grade_loc].Cl;
    fgmssPtr->Cwl = ARADfgmss[freq_loc][grade_loc].Cwl;
    fgmssPtr->Wr = ARADfgmss[freq_loc][grade_loc].Wr;
    fgmssPtr->DdrMhz = ARADfgmss[freq_loc][grade_loc].DdrMhz;
    fgmssPtr->ReadDataDelay = ARADfgmss[freq_loc][grade_loc].ReadDataDelay;
    fgmssPtr->ModeRegWr1 = ARADfgmss[freq_loc][grade_loc].ModeRegWr1;
    fgmssPtr->ModeRegWr2 = ARADfgmss[freq_loc][grade_loc].ModeRegWr2;
    fgmssPtr->ExtModeWr1 = ARADfgmss[freq_loc][grade_loc].ExtModeWr1;
    fgmssPtr->ExtModeWr2 = ARADfgmss[freq_loc][grade_loc].ExtModeWr2;
    fgmssPtr->ExtModeWr3 = ARADfgmss[freq_loc][grade_loc].ExtModeWr3;
    fgmssPtr->EMR2 = ARADfgmss[freq_loc][grade_loc].EMR2;
    fgmssPtr->EMR3 = ARADfgmss[freq_loc][grade_loc].EMR3;
    fgmssPtr->DDRtRST = ARADfgmss[freq_loc][grade_loc].DDRtRST;
    fgmssPtr->DDRtDLL = ARADfgmss[freq_loc][grade_loc].DDRtDLL;
    fgmssPtr->DDRtRC = ARADfgmss[freq_loc][grade_loc].DDRtRC;
    fgmssPtr->DDRtRRD = ARADfgmss[freq_loc][grade_loc].DDRtRRD;
    fgmssPtr->DDRtRFC = ARADfgmss[freq_loc][grade_loc].DDRtRFC;
    fgmssPtr->DDRtRCDR = ARADfgmss[freq_loc][grade_loc].DDRtRCDR;
    fgmssPtr->DDRtRCDW = ARADfgmss[freq_loc][grade_loc].DDRtRCDW;
    fgmssPtr->InitWaitPRD = ARADfgmss[freq_loc][grade_loc].InitWaitPRD;
    fgmssPtr->CntRASRDPRD = ARADfgmss[freq_loc][grade_loc].CntRASRDPRD;
    fgmssPtr->CntRASWRPRD = ARADfgmss[freq_loc][grade_loc].CntRASWRPRD;
    fgmssPtr->CntRDAPPRD = ARADfgmss[freq_loc][grade_loc].CntRDAPPRD;
    fgmssPtr->CntWRAPPRD = ARADfgmss[freq_loc][grade_loc].CntWRAPPRD;
    fgmssPtr->IndWrRdAddrMode = ARADfgmss[freq_loc][grade_loc].IndWrRdAddrMode;
    fgmssPtr->NumCols = ARADfgmss[freq_loc][grade_loc].NumCols;
    fgmssPtr->APBitPos = ARADfgmss[freq_loc][grade_loc].APBitPos;
    fgmssPtr->RefreshBurstSize = ARADfgmss[freq_loc][grade_loc].RefreshBurstSize;
    fgmssPtr->RefreshDelayPrd = ARADfgmss[freq_loc][grade_loc].RefreshDelayPrd;
    fgmssPtr->DDRtZQCS = ARADfgmss[freq_loc][grade_loc].DDRtZQCS;
    fgmssPtr->DDRtFAW = ARADfgmss[freq_loc][grade_loc].DDRtFAW;
    fgmssPtr->CntRdPrd = ARADfgmss[freq_loc][grade_loc].CntRdPrd;
    fgmssPtr->CntWrPrd = ARADfgmss[freq_loc][grade_loc].CntWrPrd;
    fgmssPtr->Enable8Banks = ARADfgmss[freq_loc][grade_loc].Enable8Banks;
    fgmssPtr->DDRResetPolarity = ARADfgmss[freq_loc][grade_loc].DDRResetPolarity;
    fgmssPtr->StaticOdtEn = ARADfgmss[freq_loc][grade_loc].StaticOdtEn;
    fgmssPtr->AddrTermHalf = ARADfgmss[freq_loc][grade_loc].AddrTermHalf;
    fgmssPtr->DRAMType = ARADfgmss[freq_loc][grade_loc].DRAMType;
    fgmssPtr->BurstSizeMode = ARADfgmss[freq_loc][grade_loc].BurstSizeMode;
    fgmssPtr->WrLatency = ARADfgmss[freq_loc][grade_loc].WrLatency;
    fgmssPtr->CntRDWRPRD = ARADfgmss[freq_loc][grade_loc].CntRDWRPRD;
    fgmssPtr->CntWRRDPRD = ARADfgmss[freq_loc][grade_loc].CntWRRDPRD;
    fgmssPtr->DDRtREFI = ARADfgmss[freq_loc][grade_loc].DDRtREFI;
    fgmssPtr->DynOdtLength = ARADfgmss[freq_loc][grade_loc].DynOdtLength;
    fgmssPtr->DynOdtStartDelay = ARADfgmss[freq_loc][grade_loc].DynOdtStartDelay;
    fgmssPtr->DDR3ZQCalibGenPrd = ARADfgmss[freq_loc][grade_loc].DDR3ZQCalibGenPrd;
    fgmssPtr->ReadDelay = ARADfgmss[freq_loc][grade_loc].ReadDelay;
    fgmssPtr->IntialCalibMprAddr = ARADfgmss[freq_loc][grade_loc].IntialCalibMprAddr;
}
#endif

const size_mem_type_set_t smts[MEM_COUNT] = {
            {0, 2}, /* MEM_4G, 14b */
            {1, 1}, /* MEM_2G, 13b */
            {2, 0} /* MEM_1G, 13b  */
};

const size_mem_type_set_t kt2_smts[MEM_COUNT] = {
            {0, 3}, /* MEM_4G, 15b address width (32K rows) */
            {1, 2}, /* MEM_2G, 14b address width (16K rows) */
            {2, 1} /* MEM_1G, 13b address width (8K rows) */
};
int
_soc_ddr40_mem_rowstoloc(uint32 rows)
{
    switch(rows) {
        case MEM_ROWS_32K:
            return MEM_4G;
        case MEM_ROWS_16K:
            return MEM_2G;
        default:
            return MEM_1G;
    }
}

int
_soc_ddr40_mem_gradetoloc(uint32 grade)
{
    switch(grade) {
        case MEM_GRADE_080808:
            return GRADE_080808;
        case MEM_GRADE_090909:
            return GRADE_090909;
        case MEM_GRADE_101010:
            return GRADE_101010;
        case MEM_GRADE_111111:
            return GRADE_111111;
        case MEM_GRADE_121212:
            return GRADE_121212;
        case MEM_GRADE_131313:
            return GRADE_131313;
        case MEM_GRADE_141414:
            return GRADE_141414;
        default:
            return GRADE_DEFAULT;
    }
}

const  phy_freq_div_set_t pfds[PHY_COUNT][FREQ_COUNT] = {
/*                 400      500       533       667       800      914       933      1066 */
/* PHY_ RSVP */ {{-1,-1}, {-1,-1},  {-1,-1},  {-1,-1},  {-1,-1}, {-1,-1},  {-1,-1},  {-1,-1}},
/* PHY_ NS   */ {{80,10}, {100,10}, {96,9},   {80,6},   {80,5},  {128,7},  {112,6},  {128,6}},
/* PHY_ ENG  */ {{80,10}, {100,10}, {96,9},   {80,6},   {80,5},  {128,7},  {168,9},  {192,9}},
/* PHY_ AND  */ {{80,10}, {100,10}, {96,9},   {80,6},   {128,8}, {128,7},  {112,6},  {128,6}},
/* PHY_ CE   */ {{-1,-1}, {-1,-1},  {-1,-1},  {-1,-1},  {-1,-1}, {-1,-1},  {-1,-1},  {-1,-1}}
};

int
_soc_ddr40_phy_freqtoloc(uint32 freq)
{
    switch(freq) {
        case DDR_FREQ_400:
            return FREQ_400;
        case DDR_FREQ_500:
            return FREQ_500;
        case DDR_FREQ_533:
            return FREQ_533;
        case DDR_FREQ_667:
            return FREQ_667;
        case DDR_FREQ_800:
            return FREQ_800;
        case DDR_FREQ_914:
            return FREQ_914;
        case DDR_FREQ_933:
            return FREQ_933;
        case DDR_FREQ_1066:
            return FREQ_1066;
        default:
            if(freq > DDR_FREQ_1066) {
                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META("Overclocking Frequency %d MHz \n"),
                          freq));
                return FREQ_1066;
            }
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("Unsupported Frequency %d MHz \n"), freq));
            return SOC_E_FAIL;
    }
}

#if 1 /* Uncomment this when required */
STATIC void
_shmoo_print_diagnostics(int unit, int ci, int wl) {

    int x, y;
    uint32 data;

    x = 0x004C;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tData = 0x%08X\t\tVDL CALIB STATUS\n"),
                      x, data));
        } else {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tERROR            \t\tVDL CALIB STATUS\n"),
                      x));
    }
    x = 0x0058;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tData = 0x%08X\t\tVDL RD EN CALIB STATUS\n"),
                      x, data));
        } else {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tERROR            \t\tVDL RD EN CALIB STATUS\n"),
                      x));
    }
    x = 0x0050;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tData = 0x%08X\t\tVDL DQ/DQS CALIB STATUS\n"),
                      x, data));
        } else {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tERROR            \t\tVDL DQ/DQS CALIB STATUS\n"),
                      x));
    }
    x = 0x0054;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tData = 0x%08X\t\tVDL WR DQ CALIB STATUS\n"),
                      x, data));
        } else {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tERROR            \t\tVDL WR DQ CALIB STATUS\n"),
                      x));
    }
    x = 0x0030;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE CTL\n"),
                      x, data));
        } else {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE CTL\n"),
                      x));
    }
    x = 0x0034;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BIT CTL\n"),
                      x, data));
        } else {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BIT CTL\n"),
                      x));
    }
    x = 0x003C;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tData = 0x%08X\t\tZQ PVT COMP CTL\n"),
                      x, data));
        } else {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tERROR            \t\tZQ PVT COMP CTL\n"),
                      x));
    }
    x = 0x006C;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tData = 0x%08X\t\tVREF DAC CONTROL\n"),
                      x, data));
        } else {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tERROR            \t\tVREF DAC CONTROL\n"),
                      x));
    }
    if(wl == 0) {
        y = 0;
    } else {
        y = 0x0200;
    }
    x = y + 0x0200;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE RD EN\n"),
                      x, data));
        } else {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE RD EN\n"),
                      x));
    }
    x = y + 0x0274;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE0 BIT RD EN\n"),
                      x, data));
        } else {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE0 BIT RD EN\n"),
                      x));
    }
    x = y + 0x0314;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE1 BIT RD EN\n"),
                      x, data));
        } else {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE1 BIT RD EN\n"),
                      x));
    }
    x = y + 0x0234;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE0 BIT0 R DQ\n"),
                      x, data));
        } else {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE0 BIT0 R DQ\n"),
                      x));
    }
    x = y + 0x02D4;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE1 BIT0 R DQ\n"),
                      x, data));
        } else {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE1 BIT0 R DQ\n"),
                      x));
    }
    x = y + 0x0208;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE0 R DQS\n"),
                      x, data));
        } else {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE0 R DQS\n"),
                      x));
    }
    x = y + 0x02A8;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE1 R DQS\n"),
                      x, data));
        } else {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE1 R DQS\n"),
                      x));
    }
    x = y + 0x0204;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE0 WR DQ\n"),
                      x, data));
        } else {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE0 WR DQ\n"),
                      x));
    }
    x = y + 0x0210;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE0 BIT WR DQ\n"),
                      x, data));
        } else {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE0 BIT WR DQ\n"),
                      x));
    }
    x = y + 0x02A4;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE1 WR DQ\n"),
                      x, data));
        } else {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE1 WR DQ\n"),
                      x));
    }
    x = y + 0x02B0;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE1 BIT WR DQ\n"),
                      x, data));
        } else {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE1 BIT WR DQ\n"),
                      x));
    }
}
#endif

int _soc_ddr40_pll_calc(uint32 freq,int *ndiv, int *mdiv) {

    uint32 m,n, diff, err, nval=0, mval=0;

    err = freq; /* Maximum error */

    for (n=120; n>=60; n--) {
        m = (n * 50 / freq); /* Round Down */
        diff = (50 * n / m) - freq;
        if (diff < err) {
            err = diff;
            nval = n;
            mval = m;
        }
        m++;                /* Round Up */
        diff = freq - (50 * n / m);
        if (diff < err) {
            err = diff;
            nval = n;
            mval = m;
        }
        if (err == 0) {
            break;
        }
    }

    if ((ndiv != NULL) && (mdiv != NULL)) {
        *ndiv = (int)nval;
        *mdiv = (int)mval;
    }
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META("Freq=%d n=%d m=%d err=%d\n"),
              freq, nval, mval, err));
    return err;
}

int
soc_ddr40_phy_pll_ctl(int unit, int ci, uint32 freq, uint32 phyType, int stat)
{
    uint32 rval;
    sal_usecs_t to_val;
    soc_timeout_t to;
    uint32 pllfreq;

    /* Per Speed & Grade Parameters */
    int ndiv, mdiv, freq_loc;

    freq_loc = _soc_ddr40_phy_freqtoloc(freq);

    if (freq_loc < 0) {
        return SOC_E_INTERNAL; /* Unsupported Frequency */
    }

    if (SAL_BOOT_QUICKTURN) {
            to_val = 10000000;  /* 10 Sec */
    } else {
            to_val = 50000;     /* 50 mS */
    }
    
    ndiv = pfds[phyType][freq_loc].ndiv;
    mdiv = pfds[phyType][freq_loc].mdiv;

    pllfreq = soc_property_get(unit, spn_DDR3_PLL_MHZ, freq);

    if (pllfreq != freq) {
        _soc_ddr40_pll_calc(pllfreq, &ndiv, &mdiv);
    }

    if (!stat) {
        switch(phyType) {
            case DDR_PHYTYPE_RSVP:
                break;
            case DDR_PHYTYPE_NS:
                if(SOC_IS_KATANA(unit)) {
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "A01. Reset DDR PLL\n")));
                    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit,&rval));
                    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                                                TOP_DDR3_PLL_RST_Lf, 0);
                    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                                                TOP_DDR3_PLL_POST_RST_Lf, 0);
                    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit,rval));

                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "A02. Set PLL div.(mdiv/ndiv:%d/%d)\n"),
                              mdiv,ndiv));
                    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,DDR3_PLL_CTRL_REGISTER_3r,
                                REG_PORT_ANY, NDIV_INTf, ndiv));
                    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,DDR3_PLL_CTRL_REGISTER_4r,
                                REG_PORT_ANY, CH0_MDIVf, mdiv));

                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "A03. Unreset PLL (mdiv:%d ndiv:%d)\n"),
                              mdiv,ndiv));
                    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit,&rval));
                    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                                                TOP_DDR3_PLL_RST_Lf, 1);
                    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit,rval));

                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "A04. Wait for PLL lock\n")));
                    to_val = 50000; /* 50 mS */
                    soc_timeout_init(&to, to_val, 0);
                    do {
                        SOC_IF_ERROR_RETURN(READ_DDR3_PLL_STATUSr(unit,&rval));
                        if (soc_reg_field_get(unit, DDR3_PLL_STATUSr, rval, DDR3_PLL_LOCKf)) {
                            break;
                        }
                        if (soc_timeout_check(&to)) {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "Timed out waiting for DDR3 PLL to Lock\n")));
                            return SOC_E_TIMEOUT;
                        }
                    } while (TRUE);

                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "A05. Unreset post PLL lock\n")));
                    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit,&rval));
                    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_DDR3_PLL_POST_RST_Lf, 1);
                    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit,rval));
                } else {
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "A01. Reset DDR PLL\n")));
                    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit,&rval));
                    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                                                TOP_DDR_PLL0_RST_Lf, 0);
                    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                                                TOP_DDR_PLL0_POST_RST_Lf, 0);
                    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit,rval));

                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "A02. Set PLL div.(mdiv/ndiv:%d/%d)\n"),
                              mdiv,ndiv));
                    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,TOP_DDR_PLL0_CTRL_REGISTER_3r,
                                REG_PORT_ANY, NDIV_INTf, ndiv));
                    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,TOP_DDR_PLL0_CTRL_REGISTER_4r,
                                REG_PORT_ANY, CH0_MDIVf, mdiv));

                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "A03. Unreset PLL (mdiv:%d ndiv:%d)\n"),
                              mdiv,ndiv));
                    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit,&rval));
                    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                                                TOP_DDR_PLL0_RST_Lf, 1);
                    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit,rval));

                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "A04. Wait for PLL lock\n")));
                    to_val = 50000; /* 50 mS */
                    soc_timeout_init(&to, to_val, 0);
                    do {
                        SOC_IF_ERROR_RETURN(READ_TOP_DDR_PLL0_STATUSr(unit,&rval));
                        if (soc_reg_field_get(unit, TOP_DDR_PLL0_STATUSr, rval, DDR_PLL0_LOCKf)) {
                            break;
                        }
                        if (soc_timeout_check(&to)) {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "Timed out waiting for DDR3 PLL to Lock\n")));
                            return SOC_E_TIMEOUT;
                        }
                    } while (TRUE);

                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "A05. Unreset post PLL lock\n")));
                    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit,&rval));
                    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_DDR_PLL0_POST_RST_Lf, 1);
                    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit,rval));
                }

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "A06. Wait for a while after lock\n")));
                sal_usleep(2000);
                break;
            case DDR_PHYTYPE_ENG:
            /*    SOC_DEBUG(SOC_DBG_DDR, ("A01. Reset DPRC\n"));
                SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, DRCALL, DRC_SPARE_REGISTER_3r, &rval));
                soc_reg_field_set(unit, DRCBROADCAST_SPARE_REGISTER_3r, &rval,
                                                        DPRC_ALIGN_PHY_RSTNf, 0);
                soc_reg_field_set(unit, DRCBROADCAST_SPARE_REGISTER_3r, &rval,
                                                   CLK_DIV_RSTNf, 0);
                SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, DRCALL, DRC_SPARE_REGISTER_3r, rval));

                SOC_DEBUG(SOC_DBG_DDR, ("A02. Bypass PLL mode\n"));
                SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, DRCALL, DRC_DPI_POWERr, &rval));
                soc_reg_field_set(unit, DRCBROADCAST_DPI_POWERr, &rval,
                                                        BYPASS_PLLf, 0);
                SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, DRCALL, DRC_DPI_POWERr, rval));

                SOC_DEBUG(SOC_DBG_DDR, ("A02. Set PLL div.(mdiv/ndiv:%d/%d)\n",mdiv,ndiv));
                SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,DDR3_PLL_CTRL_REGISTER_3r,
                          REG_PORT_ANY, NDIV_INTf, ndiv));
                SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,DDR3_PLL_CTRL_REGISTER_4r,
                          REG_PORT_ANY, CH0_MDIVf, mdiv));

                SOC_DEBUG(SOC_DBG_DDR, ("A03. Enable clock dividers\n"));
                SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, DRCALL, DRC_SPARE_REGISTER_3r, &rval));
                soc_reg_field_set(unit, DRCBROADCAST_SPARE_REGISTER_3r, &rval,
                                                   CLK_DIV_RSTNf, 1);
                SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, DRCALL, DRC_SPARE_REGISTER_3r, rval));
                
                SOC_DEBUG(SOC_DBG_DDR, ("A04. Wait for a while after clock divider enable\n"));
                sal_usleep(2000);

                SOC_DEBUG(SOC_DBG_DDR, ("A05. Unreset DPRC\n"));
                SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, DRCALL, DRC_SPARE_REGISTER_3r, &rval));
                soc_reg_field_set(unit, DRCBROADCAST_SPARE_REGISTER_3r, &rval,
                                                        DPRC_ALIGN_PHY_RSTNf, 1);
                SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, DRCALL, DRC_SPARE_REGISTER_3r, rval));

                SOC_DEBUG(SOC_DBG_DDR, ("A06. Wait for a while after clock divider enable\n"));
                sal_usleep(2000); */
                break;
            case DDR_PHYTYPE_CE:
                break;
            case DDR_PHYTYPE_HR2:
                break;
            default:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Error. Unsupported PHY type\n")));
                return SOC_E_FAIL;
        }
    } else {
    /* report only */
        switch(phyType) {
            case DDR_PHYTYPE_RSVP:
                break;
            case DDR_PHYTYPE_NS:
                SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit,&rval));
                LOG_CLI((BSL_META_U(unit,
                                    "TOP_SOFT_RESET_REG:%x\n"), rval));
                SOC_IF_ERROR_RETURN(READ_DDR3_PLL_CTRL_REGISTER_3r(unit,&rval));
                LOG_CLI((BSL_META_U(unit,
                                    "DDR3_PLL_CTRL_REGISTER_3r:%x\n"), rval));
                SOC_IF_ERROR_RETURN(READ_DDR3_PLL_CTRL_REGISTER_4r(unit,&rval));
                LOG_CLI((BSL_META_U(unit,
                                    "DDR3_PLL_CTRL_REGISTER_4r:%x\n"), rval));
                break;
            case DDR_PHYTYPE_ENG:
                break;
            case DDR_PHYTYPE_CE:
                break;
            case DDR_PHYTYPE_HR2:
                break;
            default:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Error. Unsupported PHY type\n")));
                return SOC_E_FAIL;
        }
    }

    return SOC_E_NONE;
}

/***********************************************************************
* Function to reset the PLL. Will reset the PLL.
 */
int
soc_ddr40_phy_pll_rst  (int unit, int ci, uint32 phyType, int cnt)
{
    int i = 0;
    int intCnt;
    uint32 rval;

    if(cnt == 0) {
        intCnt = 1;
    } else {
        intCnt = cnt;
    }

    for(i = 0; i < intCnt; i++) {
        switch(phyType) {
            case DDR_PHYTYPE_RSVP:
                break;
            case DDR_PHYTYPE_NS:
                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "A1. Reset DDR(ci=%d PLL(Iter=%d)\n"),
                          ci,i));
                SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit,&rval));
                soc_reg_field_set(unit,TOP_SOFT_RESET_REG_2r, &rval,
                                                        TOP_DDR3_PLL_RST_Lf, 0);
                soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                                                   TOP_DDR3_PLL_POST_RST_Lf, 0);
                SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit,rval));

                sal_usleep(2000);

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "A2. Unreset DDR(ci=%d PLL(Iter=%d)\n"),
                          ci,i));
                SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit,&rval));
                soc_reg_field_set(unit,TOP_SOFT_RESET_REG_2r,&rval,
                                                        TOP_DDR3_PLL_RST_Lf, 1);
                SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit,rval));
                break;
            case DDR_PHYTYPE_ENG:
                break;
            case DDR_PHYTYPE_CE:
                break;
            case DDR_PHYTYPE_HR2:
                break;
            default:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Error. Unsupported PHY type\n")));
                return SOC_E_FAIL;
        }
    }

    return SOC_E_NONE;
}

/***********************************************************************
* Functions to  set/report the PVT params
 */
int
_soc_ddr40_phy_PVT_ctl(int unit, int ci, uint32 phyType, int stat)
{
    /* WORKS ON ONLY ONE CI; DOES NOT ITERATE OVER ALL CIs. */
    /* ONLY EVEN CIs SHOULD BE PASSED */
    
    uint32 rval;
    sal_usecs_t to_val;
    soc_timeout_t to;
    
    /* uint32 mhz; */
    int ciC;
/*    uint32 data; */
    
    if (!stat) {
        switch(phyType) {
            case DDR_PHYTYPE_RSVP:
                break;
            case DDR_PHYTYPE_NS:
            case DDR_PHYTYPE_ENG:
            case DDR_PHYTYPE_HR2:
                if (SAL_BOOT_QUICKTURN) {
                    to_val = 10000000;  /* 10 Sec */
                } else {
                    to_val = 50000;     /* 50 mS */
                }
                
                ciC = ci & 0xFFFFFFFE;

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "E01. Reset Vref before Shmoo\n")));
                rval = 0x820;
                SOC_IF_ERROR_RETURN(MODIFY_DDR40_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, ciC, rval, 0xFFF));
                
                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "D04. Calibrate ZQ (ddr40_phy_calib_zq) before Shmoo\n")));
                
                SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,&rval));
                DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,ZQ_PVT_COMP_CTL,SAMPLE_EN,0);
                SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,rval));

                SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,&rval));
                DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,ZQ_PVT_COMP_CTL,SAMPLE_EN,1);
                SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,rval));

                soc_timeout_init(&to, to_val, 0);
                do {
                    SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,&rval));
                    if (DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, ZQ_PVT_COMP_CTL, SAMPLE_DONE)) {
                        break;
                    }
                    if (soc_timeout_check(&to)) {
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META_U(unit,
                                              "CI%d: Timed out waiting for ZQ Calibration\n"), ciC));
                        return SOC_E_TIMEOUT;
                    }
                } while (TRUE);

            /*    SOC_IF_ERROR_RETURN(READ_CI_PHY_CONTROLr(unit,0,&data));
                mhz = soc_reg_field_get(unit, CI_PHY_CONTROLr, data, DDR_MHZf);
                mhz = SOC_DDR3_CLOCK_MHZ(unit);
            */
            
                if (!SAL_BOOT_QUICKTURN) {
                    uint32 rd_en_byte_mode=0, rd_en_byte_vdl_steps=0, rd_en_bit_vdl_offset=0;

                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "D07. VDL Calibration before Shmoo\n")));
                    rval = 0;
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit,ciC,rval));

                    rval = 0;
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,VDL_CALIBRATE,CALIB_FAST,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,VDL_CALIBRATE,CALIB_ONCE,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,VDL_CALIBRATE,CALIB_AUTO,1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit,ciC,rval));
                    soc_timeout_init(&to, to_val, 0);
                    do {
                        SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit,ciC,&rval));
                        if (DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_IDLE)) {
                            break;
                        }
                        if (soc_timeout_check(&to)) {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "CI%d: Timed out waiting for VDL Calibration Idle\n"), ciC));
                            return SOC_E_TIMEOUT;
                        }
                    } while (TRUE);
                    SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit,ciC,&rval));
                    if (soc_property_get(unit, spn_DIAG_EMULATOR_PARTIAL_INIT, 0x0) == 0) {
                        if (0 == DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_LOCK)) {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "CI%d: VDL Calibration Did Not Lock reg=%x \n"), ciC, rval));
                            return SOC_E_FAIL;
                        }
                    }

                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "VDL calibration result: 0x%0x (cal_steps = %d)\n"),
                              rval, DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_TOTAL) >> 4));

                    /* clear VDL calib control */
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit,ciC,0));

                    SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_RD_EN_CALIB_STATUSr(unit,ciC,&rval));

                    rd_en_byte_mode = DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_BYTE_SEL);
                    rd_en_byte_vdl_steps = DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_TOTAL) >> 4;
                    rd_en_bit_vdl_offset = DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_BIT_OFFSET);
                    COMPILER_REFERENCE(rd_en_byte_mode);
                    COMPILER_REFERENCE(rd_en_byte_vdl_steps);
                    COMPILER_REFERENCE(rd_en_bit_vdl_offset);

                    soc_timeout_init(&to, to_val, 0);
                    do {
                        SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit,ciC,&rval));
                        if (DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_IDLE)) {
                            break;
                        }
                        if (soc_timeout_check(&to)) {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "CI%d: Timed out waiting for VDL Calibration Idle(1)\n"), ciC));
                            return SOC_E_TIMEOUT;
                        }
                    } while (TRUE);
                    SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit,ciC,&rval));
                    if (soc_property_get(unit, spn_DIAG_EMULATOR_PARTIAL_INIT, 0x0) == 0) {
                        if (0 == DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_LOCK)) {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "CI%d: VDL Calibration Did Not Lock(1)\n"), ciC));
                            return SOC_E_FAIL;
                        } 
                    }
                } else {
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "D07. VDL Calibration SKIPPED before Shmoo\n")));
                }/* Not QUICKTURN */
                break;
            case DDR_PHYTYPE_CE:
                break;
            default:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Error. Unsupported PHY type\n")));
                return SOC_E_FAIL;
        }
    } else {
    /* report only */
        switch(phyType) {
            case DDR_PHYTYPE_RSVP:
                break;
            case DDR_PHYTYPE_NS:
                break;
            case DDR_PHYTYPE_ENG:
                break;
            case DDR_PHYTYPE_CE:
                break;
            case DDR_PHYTYPE_HR2:
                break;
            default:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Error. Unsupported PHY type\n")));
                return SOC_E_FAIL;
        }
    }
    return SOC_E_NONE;
}

/***********************************************************************
* Functions to  Calibrate the PHY
 */
int
soc_ddr40_phy_calibrate(int unit, int ci, uint32 phyType, int stat)
{
    uint32 rval;
/*    uint64 rval64; */
    soc_timeout_t to;
    sal_usecs_t to_val;
    /* int mhz;  CI_PHY_CONTROL */
    int ciC, ciCS, ciCM;
/*    uint32 data; */
    uint32 mr0, mr1, mr2, mr3;

    KATANAfreq_grade_mem_set_set_t T0fgmss;
/*    ARADfreq_grade_mem_set_set_t ENGfgmss; */
/*    int grade_loc; */
    int freq_loc, grade_loc;
    
    freq_loc = _soc_ddr40_phy_freqtoloc(SOC_DDR3_CLOCK_MHZ(unit));
    grade_loc = _soc_ddr40_mem_gradetoloc(SOC_DDR3_MEM_GRADE(unit));

    if (freq_loc < 0) {
        return SOC_E_INTERNAL; /* Unsupported Frequency */
    }

    if (SAL_BOOT_QUICKTURN) {
        to_val = 10000000;  /* 10 Sec */
    } else {
        to_val = 50000;     /* 50 mS */
    }

    if (!stat) {
        switch(phyType) {
            case DDR_PHYTYPE_RSVP:
                break;
            case DDR_PHYTYPE_NS:
                _soc_ddr40_katana_phy_init_mem_set_wrapper(unit, &T0fgmss, freq_loc, grade_loc);
                if (T0fgmss.mhz == -1) {
                    return SOC_E_INTERNAL; /* Unsupported Frequency / Grade Combination */
                }

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C01. Check Power Up Reset_Bar\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    soc_timeout_init(&to, to_val, 0);
                    do {
                        SOC_IF_ERROR_RETURN(READ_CI_PHY_CONTROLr(unit,ciC,&rval));
                        if (soc_reg_field_get(unit, CI_PHY_CONTROLr, rval, PWRUP_RSBf )) {
                            break;
                        }
                        if (soc_timeout_check(&to)) {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "CI%d: Timed out waiting for DDR PHY to Power Up\n"), ciC));
                            return SOC_E_TIMEOUT;
                        }
                    } while (TRUE);
                }
                
                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C02. Config and Release PLL from reset\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    /* Divider */
                    SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_PLL_DIVIDERSr(unit,ciC,&rval));
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,PLL_DIVIDERS,NDIV,16);
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,PLL_DIVIDERS,POST_DIV,1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_PLL_DIVIDERSr(unit,ciC,rval));
                    /* Reset */
                    SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_PLL_CONFIGr(unit,ciC,&rval));
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,PLL_CONFIG,RESET,0);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_PLL_CONFIGr(unit,ciC,rval));

                    SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_PLL_CONFIGr(unit,ciC,&rval));
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,PLL_CONFIG,RESET,1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_PLL_CONFIGr(unit,ciC,rval));
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,PLL_CONFIG,RESET,0);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_PLL_CONFIGr(unit,ciC,rval));
                }
                
                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C03. Poll PLL Lock\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    soc_timeout_init(&to, to_val, 0);
                    do {
                        SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_PLL_STATUSr(unit,ciC,&rval));
                        if (DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, PLL_STATUS, LOCK)) {
                            break;
                        }
                        if (soc_timeout_check(&to)) {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "CI%d: Timed out waiting for DDR PHY PLL to Lock\n"), ciC));
                            return SOC_E_TIMEOUT;
                        }
                    } while (TRUE);
                }
                
                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C04. Calibrate ZQ (ddr40_phy_calib_zq)\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,&rval));
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,ZQ_PVT_COMP_CTL,SAMPLE_EN,0);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,rval));

                    SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,&rval));
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,ZQ_PVT_COMP_CTL,SAMPLE_EN,1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,rval));
                }
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    soc_timeout_init(&to, to_val, 0);
                    do {
                        SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,&rval));
                        if (DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, ZQ_PVT_COMP_CTL, SAMPLE_DONE)) {
                            break;
                        }
                        if (soc_timeout_check(&to)) {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "CI%d: Timed out waiting for ZQ Calibration\n"), ciC));
                            return SOC_E_TIMEOUT;
                        }
                    } while (TRUE);
                }
                
                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C05. DDR PHY VTT On (Virtual VTT setup) DISABLE all Virtual VTT\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    rval = 0x3f1ffff;  /* all bits except RESET and CKE */
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VIRTUAL_VTT_CONNECTIONSr(unit,ciC,rval));

                    rval = 0; /* AGUTMANN disable VTT override , previously 0x1ffff; */    
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VIRTUAL_VTT_OVERRIDEr(unit,ciC,rval));

                }

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C06. DDR40_PHY_DDR3_MISC\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    rval = 0;
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,DRIVE_PAD_CTL,VDDO_VOLTS,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,DRIVE_PAD_CTL,RT120B_G,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,DRIVE_PAD_CTL,RT60B,1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_DRIVE_PAD_CTLr(unit,ciC,rval));

                    rval = 0;
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,DRIVE_PAD_CTL,VDDO_VOLTS,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,DRIVE_PAD_CTL,RT120B_G,1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_0_DRIVE_PAD_CTLr(unit,ciC,rval));
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_1_DRIVE_PAD_CTLr(unit,ciC,rval));
                }
                
                /*    SOC_IF_ERROR_RETURN(READ_CI_PHY_CONTROLr(unit,0,&data));
                      mhz = soc_reg_field_get(unit, CI_PHY_CONTROLr, data, DDR_MHZf);
                      mhz = SOC_DDR3_CLOCK_MHZ(unit);
                */

                if (!SAL_BOOT_QUICKTURN) {
                    uint32 rd_en_byte_mode=0, rd_en_byte_vdl_steps=0, rd_en_bit_vdl_offset=0;

                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "C07. VDL Calibration\n")));
                    for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                        if(!_check_dram_either(ciC)) {
                            continue;
                        }
                        rval = 0;
                        SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit,ciC,rval));

                        rval = 0;
                        DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,VDL_CALIBRATE,CALIB_FAST,1);
                        DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,VDL_CALIBRATE,CALIB_ONCE,1);
                        DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,VDL_CALIBRATE,CALIB_AUTO,1);
                        SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit,ciC,rval));
                    }
                    for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                        if(!_check_dram_either(ciC)) {
                            continue;
                        }
                        soc_timeout_init(&to, to_val, 0);
                        do {
                            SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit,ciC,&rval));
                            if (DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_IDLE)) {
                                break;
                            }
                            if (soc_timeout_check(&to)) {
                                LOG_ERROR(BSL_LS_SOC_COMMON,
                                          (BSL_META_U(unit,
                                                      "CI%d: Timed out waiting for VDL Calibration Idle\n"), ciC));
                                return SOC_E_TIMEOUT;
                            }
                        } while (TRUE);
                        SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit,ciC,&rval));
                        if (0 == DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_LOCK)) {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "CI%d: VDL Calibration Did Not Lock reg=%x \n"), ciC, rval));
                            return SOC_E_FAIL;
                        }

                        LOG_INFO(BSL_LS_SOC_DDR,
                                 (BSL_META_U(unit,
                                             "VDL calibration result: 0x%0x (cal_steps = %d)\n"),
                                  rval, DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_TOTAL) >> 4));

                        /* clear VDL calib control */
                        SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit,ciC,0));

                    }
                    for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                        if(!_check_dram_either(ciC)) {
                            continue;
                        }

                        SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_RD_EN_CALIB_STATUSr(unit,ciC,&rval));

                        rd_en_byte_mode = DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_BYTE_SEL);
                        rd_en_byte_vdl_steps = DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_TOTAL) >> 4;
                        rd_en_bit_vdl_offset = DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_BIT_OFFSET);

                        COMPILER_REFERENCE(rd_en_byte_mode);
                        COMPILER_REFERENCE(rd_en_byte_vdl_steps);
                        COMPILER_REFERENCE(rd_en_bit_vdl_offset);

                        /* rval = 0;
                           SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit,ci,rval));
                           DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,VDL_CALIBRATE,CALIB_STEPS,1);
                           SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit,ci,rval));
                        */
                    }
                    for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                        if(!_check_dram_either(ciC)) {
                            continue;
                        }
                        soc_timeout_init(&to, to_val, 0);
                        do {
                            SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit,ciC,&rval));
                            if (DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_IDLE)) {
                                break;
                            }
                            if (soc_timeout_check(&to)) {
                                LOG_ERROR(BSL_LS_SOC_COMMON,
                                          (BSL_META_U(unit,
                                                      "CI%d: Timed out waiting for VDL Calibration Idle(1)\n"), ciC));
                                return SOC_E_TIMEOUT;
                            }
                        } while (TRUE);
                        SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit,ciC,&rval));
                        if (0 == DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_LOCK)) {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "CI%d: VDL Calibration Did Not Lock(1)\n"), ciC));
                            return SOC_E_FAIL;
                        }
                    }
                } else {
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "C07. VDL Calibration SKIPPED\n")));
                }/* Not QUICKTURN */

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C08. DDR40_PHY_DDR3_MISC : Start DDR40_PHY_RDLY_ODT....\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    rval = 0;
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,READ_DATA_DLY,RD_DATA_DLY,
                                    (SOC_DDR3_CLOCK_MHZ(unit) <= 400) ? 1 : (SOC_DDR3_CLOCK_MHZ(unit) <= 667) ? 2 : 3);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_0_READ_DATA_DLYr(unit,ciC,rval));
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_1_READ_DATA_DLYr(unit,ciC,rval));

                    rval = 0;
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,READ_CONTROL,DQ_ODT_ENABLE,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,READ_CONTROL,DQ_ODT_LE_ADJ,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,READ_CONTROL,DQ_ODT_TE_ADJ,1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_0_READ_CONTROLr(unit,ciC,rval));
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_1_READ_CONTROLr(unit,ciC,rval));
                    
                    if(SOC_DDR3_CLOCK_MHZ(unit) > 800) {
                        rval = 3;
                    } else {
                        rval = 1;
                    }
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_0_WR_PREAMBLE_MODEr(unit,ciC,rval));
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_1_WR_PREAMBLE_MODEr(unit,ciC,rval));
                }

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C09. Start ddr40_phy_autoidle_on (MEM_SYS_PARAM_PHY_AUTO_IDLE) ....\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    rval = 0;
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,IDLE_PAD_CONTROL,AUTO_DQ_IDDQ_MODE,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,IDLE_PAD_CONTROL,AUTO_DQ_RXENB_MODE,1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_0_IDLE_PAD_CONTROLr(unit,ciC,rval));
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_1_IDLE_PAD_CONTROLr(unit,ciC,rval));
                }

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C10. Set refresh count to 7.8us\n")));
                /* Set refresh count to 7.8us (by the time it starts, it already finishes MRS and ZQCal setting) */
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC++) {
                    if(!_check_dram(ciC)) {
                        continue;
                    }
                    SOC_IF_ERROR_RETURN(READ_CI_CONFIG4r(unit,ciC,&rval));
                    soc_reg_field_set(unit, CI_CONFIG4r, &rval, REFRESH_INTERVALf, T0fgmss.refrate);
                    SOC_IF_ERROR_RETURN(WRITE_CI_CONFIG4r(unit,ciC,rval));
                }
                sal_usleep(2000);

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C11. Set CKE (clock enable)\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC++) {
                    if(!_check_dram(ciC)) {
                        continue;
                    }
                    SOC_IF_ERROR_RETURN(READ_CI_PHY_CONTROLr(unit,ciC,&rval));
                    soc_reg_field_set(unit, CI_PHY_CONTROLr, &rval, AUTO_INITf, 0);
                    soc_reg_field_set(unit, CI_PHY_CONTROLr, &rval, CKEf, 0x3);
                    SOC_IF_ERROR_RETURN(WRITE_CI_PHY_CONTROLr(unit,ciC,rval));
                }
                sal_usleep(1000);

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C12. Wait for Phy Ready\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    soc_timeout_init(&to, to_val, 0);
                    do {
                        SOC_IF_ERROR_RETURN(READ_CI_PHY_CONTROLr(unit,ciC,&rval));
                        if (soc_reg_field_get(unit, CI_PHY_CONTROLr, rval, READYf)) {
                            break;
                        }
                        if (soc_timeout_check(&to)) {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "CI%d: Timed out waiting for PHY Ready\n"), ciC));
                            return SOC_E_TIMEOUT;
                        }
                    } while (TRUE);
                }

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C13. CI DDR Init : MR0, MR1, MR2, and MR3 command\n")));
                dram0_clamshell_enable = soc_property_get(unit, spn_DRAM0_CLAMSHELL_ENABLE, 0);
                dram1_clamshell_enable = soc_property_get(unit, spn_DRAM1_CLAMSHELL_ENABLE, 0);
                
                if(dram0_clamshell_enable || dram1_clamshell_enable) {
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "     Manual Init\n")));
                    
                    if(dram0_clamshell_enable) {
                        if(dram1_clamshell_enable) {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "Illegal clam shell configuration\n")));
                            return SOC_E_FAIL;
                        }
                        
                        ciCM = 0;    /* Only Even CIs Mirrored */
                        ciCS = 1;    /* Only Odd CIs Straight */
                    } else {
                        ciCM = 1;    /* Only Odd CIs Mirrored */
                        ciCS = 0;    /* Only Even CIs Straight */
                    }
                    
                    for (ciC = ciCM; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Every other CI */
                        if(!_check_dram(ciC)) {
                            continue;
                        }
                        
                        SOC_IF_ERROR_RETURN(READ_CI_DDR_MR0r(unit,ciC,&rval));
                        mr0 = soc_reg_field_get( unit, CI_DDR_MR0r, rval, Af );
                        mr0 = (mr0 & 0x00007E07) | ((mr0 & 0x0A8) << 1) | ((mr0 & 0x150) >> 1); 
                        soc_reg_field_set( unit, CI_DDR_MR0r, &mr0, Af, mr0 );
                        soc_reg_field_set( unit, CI_DDR_MR0r, &mr0, BAf, 0 );

                        SOC_IF_ERROR_RETURN(READ_CI_DDR_MR1r(unit,ciC,&rval));
                        mr2 = soc_reg_field_get( unit, CI_DDR_MR1r, rval, Af );
                        mr2 = (mr2 & 0x00007E07) | ((mr2 & 0x0A8) << 1) | ((mr2 & 0x150) >> 1);
                        soc_reg_field_set( unit, CI_DDR_MR2r, &mr2, Af, mr2 );
                        soc_reg_field_set( unit, CI_DDR_MR2r, &mr2, BAf, 2 );

                        SOC_IF_ERROR_RETURN(READ_CI_DDR_MR2r(unit,ciC,&rval));
                        mr1 = soc_reg_field_get( unit, CI_DDR_MR2r, rval, Af );
                        mr1 = (mr1 & 0x00007E07) | ((mr1 & 0x0A8) << 1) | ((mr1 & 0x150) >> 1);
                        soc_reg_field_set( unit, CI_DDR_MR1r, &mr1, Af, mr1 );
                        soc_reg_field_set( unit, CI_DDR_MR1r, &mr1, BAf, 1 );
                        
                        SOC_IF_ERROR_RETURN(READ_CI_DDR_MR3r(unit,ciC,&rval));
                        mr3 = soc_reg_field_get( unit, CI_DDR_MR3r, rval, Af );
                        mr3 = (mr3 & 0x00007E07) | ((mr3 & 0x0A8) << 1) | ((mr3 & 0x150) >> 1);
                        soc_reg_field_set( unit, CI_DDR_MR3r, &mr3, Af, mr3 );
                        soc_reg_field_set( unit, CI_DDR_MR3r, &mr3, BAf, 3 );
                        
                        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_MR0r(unit,ciC,mr0));
                        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_MR1r(unit,ciC,mr1));
                        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_MR2r(unit,ciC,mr2));
                        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_MR3r(unit,ciC,mr3));
                    }
                    
                    rval = 0;
                    soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DONEf, 1 );
                    soc_reg_field_set( unit, CI_MRS_CMDr, &rval, REQf, 1 );
                    soc_reg_field_set( unit, CI_MRS_CMDr, &rval, MRS_CMDf, 0x0 );
                    
                    /* MR2 */
                    soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DEVICE_SELECTf, 0x1 );
                    soc_reg_field_set( unit, CI_MRS_CMDr, &rval, MRS_SELECTf, 0x2 );
                    for (ciC = ciCS; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Every other CI */
                        if(!_check_dram(ciC)) {
                            continue;
                        }
                        SOC_IF_ERROR_RETURN( WRITE_CI_MRS_CMDr( unit, ciC, rval) );
                    }
                    
                    soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DEVICE_SELECTf, 0x2 );
                    soc_reg_field_set( unit, CI_MRS_CMDr, &rval, MRS_SELECTf, 0x1 );
                    for (ciC = ciCM; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Every other CI */
                        if(!_check_dram(ciC)) {
                            continue;
                        }
                        SOC_IF_ERROR_RETURN( WRITE_CI_MRS_CMDr( unit, ciC, rval) );
                    }
                    
                    /* MR3 */
                    soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DEVICE_SELECTf, 0x1 );
                    soc_reg_field_set( unit, CI_MRS_CMDr, &rval, MRS_SELECTf, 0x3 );
                    for (ciC = ciCS; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Every other CI */
                        if(!_check_dram(ciC)) {
                            continue;
                        }
                        SOC_IF_ERROR_RETURN( WRITE_CI_MRS_CMDr( unit, ciC, rval) );
                    }
                    
                    soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DEVICE_SELECTf, 0x2 );
                    for (ciC = ciCM; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Every other CI */
                        if(!_check_dram(ciC)) {
                            continue;
                        }
                        SOC_IF_ERROR_RETURN( WRITE_CI_MRS_CMDr( unit, ciC, rval) );
                    }
                    
                    /* MR1 */
                    soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DEVICE_SELECTf, 0x1 );
                    soc_reg_field_set( unit, CI_MRS_CMDr, &rval, MRS_SELECTf, 0x1 );
                    for (ciC = ciCS; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Every other CI */
                        if(!_check_dram(ciC)) {
                            continue;
                        }
                        SOC_IF_ERROR_RETURN( WRITE_CI_MRS_CMDr( unit, ciC, rval) );
                    }
                    
                    soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DEVICE_SELECTf, 0x2 );
                    soc_reg_field_set( unit, CI_MRS_CMDr, &rval, MRS_SELECTf, 0x2 );
                    for (ciC = ciCM; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Every other CI */
                        if(!_check_dram(ciC)) {
                            continue;
                        }
                        SOC_IF_ERROR_RETURN( WRITE_CI_MRS_CMDr( unit, ciC, rval) );
                    }
                    
                    /* MR0 */
                    soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DEVICE_SELECTf, 0x1 );
                    soc_reg_field_set( unit, CI_MRS_CMDr, &rval, MRS_SELECTf, 0x0 );
                    for (ciC = ciCS; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Every other CI */
                        if(!_check_dram(ciC)) {
                            continue;
                        }
                        SOC_IF_ERROR_RETURN( WRITE_CI_MRS_CMDr( unit, ciC, rval) );
                    }
                    
                    soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DEVICE_SELECTf, 0x2 );
                    for (ciC = ciCM; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Every other CI */
                        if(!_check_dram(ciC)) {
                            continue;
                        }
                        SOC_IF_ERROR_RETURN( WRITE_CI_MRS_CMDr( unit, ciC, rval) );
                    }
                    
                    /* ZQ Cal Long */
                    soc_reg_field_set( unit, CI_MRS_CMDr, &rval, MRS_CMDf, 0x3 );
                    soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DEVICE_SELECTf, 0x3 );
                    soc_reg_field_set( unit, CI_MRS_CMDr, &rval, MRS_SELECTf, 0x3 );
                    for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=1) { /* All CIs */
                        if(!_check_dram(ciC)) {
                            continue;
                        }
                        SOC_IF_ERROR_RETURN( WRITE_CI_MRS_CMDr( unit, ciC, rval) );
                    }
                
                } else {
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "     Auto Init\n")));
                
                    for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC++) {
                        if(!_check_dram(ciC)) {
                            continue;
                        }
                        SOC_IF_ERROR_RETURN(READ_CI_MRS_CMDr(unit,ciC,&rval));
                        soc_reg_field_set(unit, CI_MRS_CMDr, &rval, DEVICE_SELECTf, 3);
                        SOC_IF_ERROR_RETURN(WRITE_CI_MRS_CMDr(unit,ciC,rval));
                    }
                    for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC++) {
                        if(!_check_dram(ciC)) {
                            continue;
                        }
                        SOC_IF_ERROR_RETURN(READ_CI_DDR_AUTOINITr(unit,ciC,&rval));
                        soc_reg_field_set(unit, CI_DDR_AUTOINITr, &rval, DONEf, 1);
                        soc_reg_field_set(unit, CI_DDR_AUTOINITr, &rval, STARTf, 1);
                        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_AUTOINITr(unit,ciC,rval));
                    }
                    sal_usleep(1000);

                    for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC++) {
                        if(!_check_dram(ciC)) {
                            continue;
                        }
                        soc_timeout_init(&to, to_val, 0);
                        do {
                            SOC_IF_ERROR_RETURN(READ_CI_DDR_AUTOINITr(unit,ciC,&rval));
                            if (soc_reg_field_get(unit, CI_DDR_AUTOINITr, rval, DONEf)) {
                                break;
                            }
                            if (soc_timeout_check(&to)) {
                                LOG_ERROR(BSL_LS_SOC_COMMON,
                                          (BSL_META_U(unit,
                                                      "CI%d: Timed out DDR Init\n"), ciC));
                                return SOC_E_TIMEOUT;
                            }
                        } while (TRUE);
                    }
                }

                /* (19) Add some delay for TZQinit (512 DDR CLK) */
                sal_usleep(5120);

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C14. Clear Read/Write FIFO errors\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC++) {
                    if(!_check_dram(ciC)) {
                        continue;
                    }
                    rval = 0;
                    soc_reg_field_set(unit, CI_ERRORr, &rval, WFIFO_CTL_CORRECTED_ERRORf, 1);
                    soc_reg_field_set(unit, CI_ERRORr, &rval, WFIFO_CTL_UNCORRECTED_ERRORf, 1);
                    soc_reg_field_set(unit, CI_ERRORr, &rval, RFIFO_CTL_CORRECTED_ERRORf, 1);
                    soc_reg_field_set(unit, CI_ERRORr, &rval, RFIFO_CTL_UNCORRECTED_ERRORf, 1);
                    soc_reg_field_set(unit, CI_ERRORr, &rval, PHY_BL0_RD_FIFO_ERRORf, 1);
                    soc_reg_field_set(unit, CI_ERRORr, &rval, PHY_BL1_RD_FIFO_ERRORf, 1);
                    soc_reg_field_set(unit, CI_ERRORr, &rval, RFIFO_OVERFLOWf, 1);
                    soc_reg_field_set(unit, CI_ERRORr, &rval, WFIFO_OVERFLOWf, 1);
                    soc_reg_field_set(unit, CI_ERRORr, &rval, PHY_READY_EVENTf, 1);
                    SOC_IF_ERROR_RETURN(WRITE_CI_ERRORr(unit,ciC,rval));
                }

                if(SOC_IS_KATANA2(unit)) {
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "C15. Config and Release BIST from reset\n")));
                    for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC++) {
                        if(!_check_dram(ciC)) {
                            continue;
                        }
                        rval = 0;
                        soc_reg_field_set(unit, BISTGENERALCONFIGURATIONSr, &rval, NUMCOLSf, 2);
                        SOC_IF_ERROR_RETURN(WRITE_BISTGENERALCONFIGURATIONSr(unit,ciC,rval));

                        SOC_IF_ERROR_RETURN(READ_BISTCONFIGr(unit,ciC,&rval));
                        soc_reg_field_set(unit, BISTCONFIGr, &rval, BIST_RESETBf, 1);
                        SOC_IF_ERROR_RETURN(WRITE_BISTCONFIGr(unit,ciC,rval));
                    }
                }
                break;
            case DDR_PHYTYPE_ENG:
            /*    _soc_ddr40_arad_phy_init_mem_set_wrapper(unit, &ENGfgmss, freq_loc, grade_loc); */
                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C01. Check Power Up Reset_Bar\n")));
#ifdef BCM_PETRA_SUPPORT
                if(SOC_IS_ARAD(unit)) {
                    for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                        if(!_check_dram_either(ciC)) {
                            continue;
                        }
                        soc_timeout_init(&to, to_val, 0);
                        do {
                            SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, ciC>>1, DRC_DPI_STATUSr, &rval));
                            if (soc_reg_field_get(unit, DRCA_DPI_STATUSr, rval, PWRUP_RSBf)) {
                                break;
                            }
                            if (soc_timeout_check(&to)) {
                                LOG_ERROR(BSL_LS_SOC_COMMON,
                                          (BSL_META_U(unit,
                                                      "CI%d: Timed out waiting for DDR PHY to Power Up\n"), ciC));
                                return SOC_E_TIMEOUT;
                            }
                        } while (TRUE);
                    }
                }
#endif /* BCM_PETRA_SUPPORT */
                
                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C02. Config and Release PLL from reset\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    /* Divider */
                    SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_PLL_DIVIDERSr(unit,ciC,&rval));
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,PLL_DIVIDERS,NDIV,16);
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,PLL_DIVIDERS,POST_DIV,1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_PLL_DIVIDERSr(unit,ciC,rval));
                    /* Reset */
                    SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_PLL_CONFIGr(unit,ciC,&rval));
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,PLL_CONFIG,RESET,0);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_PLL_CONFIGr(unit,ciC,rval));

                    SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_PLL_CONFIGr(unit,ciC,&rval));
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,PLL_CONFIG,RESET,1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_PLL_CONFIGr(unit,ciC,rval));
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,PLL_CONFIG,RESET,0);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_PLL_CONFIGr(unit,ciC,rval));
                }
                
                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C03. Poll PLL Lock\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    soc_timeout_init(&to, to_val, 0);
                    do {
                        SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_PLL_STATUSr(unit,ciC,&rval));
                        if (DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, PLL_STATUS, LOCK)) {
                            break;
                        }
                        if (soc_timeout_check(&to)) {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "CI%d: Timed out waiting for DDR PHY PLL to Lock\n"), ciC));
                            return SOC_E_TIMEOUT;
                        }
                    } while (TRUE);
                }
                
                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C04. Calibrate ZQ (ddr40_phy_calib_zq)\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,&rval));
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,ZQ_PVT_COMP_CTL,SAMPLE_EN,0);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,rval));

                    SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,&rval));
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,ZQ_PVT_COMP_CTL,SAMPLE_EN,1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,rval));
                }
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    soc_timeout_init(&to, to_val, 0);
                    do {
                        SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,&rval));
                        if (DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, ZQ_PVT_COMP_CTL, SAMPLE_DONE)) {
                            break;
                        }
                        if (soc_timeout_check(&to)) {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "CI%d: Timed out waiting for ZQ Calibration\n"), ciC));
                            return SOC_E_TIMEOUT;
                        }
                    } while (TRUE);
                }
                
                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C05. DDR PHY VTT On (Virtual VTT setup) DISABLE all Virtual VTT\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    rval = 0x3f1ffff;  /* all bits except RESET and CKE */
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VIRTUAL_VTT_CONNECTIONSr(unit,ciC,rval));

                    rval = 0; /* AGUTMANN disable VTT override , previously 0x1ffff; */    
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VIRTUAL_VTT_OVERRIDEr(unit,ciC,rval));

                }

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C06. DDR40_PHY_DDR3_MISC\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    rval = 0;
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,DRIVE_PAD_CTL,VDDO_VOLTS,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,DRIVE_PAD_CTL,RT120B_G,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,DRIVE_PAD_CTL,RT60B,1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_DRIVE_PAD_CTLr(unit,ciC,rval));

                    rval = 0;
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,DRIVE_PAD_CTL,VDDO_VOLTS,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,DRIVE_PAD_CTL,RT120B_G,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,DRIVE_PAD_CTL,NO_DQS_RD,0);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_0_DRIVE_PAD_CTLr(unit,ciC,rval));
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_1_DRIVE_PAD_CTLr(unit,ciC,rval));
                }
                
            /*    SOC_IF_ERROR_RETURN(READ_CI_PHY_CONTROLr(unit,0,&data));
                mhz = soc_reg_field_get(unit, CI_PHY_CONTROLr, data, DDR_MHZf);
                mhz = SOC_DDR3_CLOCK_MHZ(unit);
            */
            
                if (!SAL_BOOT_QUICKTURN) {
                  uint32 rd_en_byte_mode=0, rd_en_byte_vdl_steps=0, rd_en_bit_vdl_offset=0;

                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "C07. VDL Calibration\n")));
                    for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                        if(!_check_dram_either(ciC)) {
                            continue;
                        }
                        rval = 0;
                        SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit,ciC,rval));

                        rval = 0;
                        DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,VDL_CALIBRATE,CALIB_FAST,1);
                        DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,VDL_CALIBRATE,CALIB_ONCE,1);
                        DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,VDL_CALIBRATE,CALIB_AUTO,1);
                        SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit,ciC,rval));
                    }
                    for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                        if(!_check_dram_either(ciC)) {
                            continue;
                        }
                        soc_timeout_init(&to, to_val, 0);
                        do {
                            SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit,ciC,&rval));
                            if (DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_IDLE)) {
                                break;
                            }
                            if (soc_timeout_check(&to)) {
                                LOG_ERROR(BSL_LS_SOC_COMMON,
                                          (BSL_META_U(unit,
                                                      "CI%d: Timed out waiting for VDL Calibration Idle\n"), ciC));
                                return SOC_E_TIMEOUT;
                            }
                        } while (TRUE);
                        SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit,ciC,&rval));
                        if (soc_property_get(unit, spn_DIAG_EMULATOR_PARTIAL_INIT, 0x0) == 0) {
                            if (0 == DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_LOCK)) {
                                LOG_ERROR(BSL_LS_SOC_COMMON,
                                          (BSL_META_U(unit,
                                                      "CI%d: VDL Calibration Did Not Lock reg=%x \n"), ciC, rval));
                                return SOC_E_FAIL;
                            } 
                        }

                        LOG_INFO(BSL_LS_SOC_DDR,
                                 (BSL_META_U(unit,
                                             "VDL calibration result: 0x%0x (cal_steps = %d)\n"),
                                  rval, DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_TOTAL) >> 4));

                        /* clear VDL calib control */
                        SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit,ciC,0));

                    }
                    for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                        if(!_check_dram_either(ciC)) {
                            continue;
                        }

                        SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_RD_EN_CALIB_STATUSr(unit,ciC,&rval));
                        rd_en_byte_mode = DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_BYTE_SEL);
                        rd_en_byte_vdl_steps = DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_TOTAL) >> 4;
                        rd_en_bit_vdl_offset = DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_BIT_OFFSET);
                        COMPILER_REFERENCE(rd_en_byte_mode);
                        COMPILER_REFERENCE(rd_en_byte_vdl_steps);
                        COMPILER_REFERENCE(rd_en_bit_vdl_offset);

                        /* rval = 0;
                           SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit,ci,rval));
                           DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,VDL_CALIBRATE,CALIB_STEPS,1);
                           SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit,ci,rval));
                        */
                    }
                    for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                        if(!_check_dram_either(ciC)) {
                            continue;
                        }
                        soc_timeout_init(&to, to_val, 0);
                        do {
                            SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit,ciC,&rval));
                            if (DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_IDLE)) {
                                break;
                            }
                            if (soc_timeout_check(&to)) {
                                LOG_ERROR(BSL_LS_SOC_COMMON,
                                          (BSL_META_U(unit,
                                                      "CI%d: Timed out waiting for VDL Calibration Idle(1)\n"), ciC));
                                return SOC_E_TIMEOUT;
                            }
                        } while (TRUE);
                        SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit,ciC,&rval));
                        if (soc_property_get(unit, spn_DIAG_EMULATOR_PARTIAL_INIT, 0x0) == 0) {
                            if (0 == DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_LOCK)) {
                                LOG_ERROR(BSL_LS_SOC_COMMON,
                                          (BSL_META_U(unit,
                                                      "CI%d: VDL Calibration Did Not Lock(1)\n"), ciC));
                                return SOC_E_FAIL;
                            } 
                        }
                    }
                } else {
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "C07. VDL Calibration SKIPPED\n")));
                }/* Not QUICKTURN */

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C08. DDR40_PHY_DDR3_MISC : Start DDR40_PHY_RDLY_ODT....\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    rval = 0;
                    #if 0 /* def BCM_PETRA_SUPPORT */
                    DDR40_SET_FIELD(rval, DDR40_PHY_WORD_LANE_0, READ_DATA_DLY, RD_DATA_DLY, arad_dram_read_data_dly_get(unit, SOC_DDR3_CLOCK_MHZ(unit)));
                    #else
                    DDR40_SET_FIELD(rval, DDR40_PHY_WORD_LANE_0, READ_DATA_DLY, RD_DATA_DLY, 1);
                    #endif
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_0_READ_DATA_DLYr(unit, ciC, rval));
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_1_READ_DATA_DLYr(unit, ciC, rval));

                    rval = 0;
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,READ_CONTROL,DQ_ODT_ENABLE,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,READ_CONTROL,DQ_ODT_LE_ADJ,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,READ_CONTROL,DQ_ODT_TE_ADJ,1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_0_READ_CONTROLr(unit,ciC,rval));
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_1_READ_CONTROLr(unit,ciC,rval));

                    rval = 3;
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_0_WR_PREAMBLE_MODEr(unit,ciC,rval));
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_1_WR_PREAMBLE_MODEr(unit,ciC,rval));
                }

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C09. Start ddr40_phy_autoidle_on (MEM_SYS_PARAM_PHY_AUTO_IDLE) ....\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    rval = 0;
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,IDLE_PAD_CONTROL,AUTO_DQ_IDDQ_MODE,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,IDLE_PAD_CONTROL,AUTO_DQ_RXENB_MODE,1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_0_IDLE_PAD_CONTROLr(unit,ciC,rval));
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_1_IDLE_PAD_CONTROLr(unit,ciC,rval));
                }

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C10. Wait for Phy Ready\n")));
#ifdef BCM_PETRA_SUPPORT
                if(SOC_IS_ARAD(unit)) {
                    for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                        if(!_check_dram_either(ciC)) {
                            continue;
                        }
                        soc_timeout_init(&to, to_val, 0);
                        do {
                            SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, ciC>>1, DRC_DPI_STATUSr, &rval));
                            if (soc_reg_field_get(unit, DRCA_DPI_STATUSr, rval, READYf)) {
                                break;
                            }
                            if (soc_timeout_check(&to)) {
                                LOG_ERROR(BSL_LS_SOC_COMMON,
                                          (BSL_META_U(unit,
                                                      "CI%d: Timed out waiting for PHY Ready\n"), ciC));
                                return SOC_E_TIMEOUT;
                            }
                        } while (TRUE);
                    }
                }
#endif /* BCM_PETRA_SUPPORT */
                break;
            case DDR_PHYTYPE_CE:
                break;
            case DDR_PHYTYPE_HR2:
                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C01. Check Power Up Reset_Bar\n")));
                /* Skipped for DDR_PHYTYPE_HR2 */
                                     
                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C02. Config and Release PLL from reset\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    /* Divider */
                    SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_PLL_DIVIDERSr(unit,ciC,&rval));
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,PLL_DIVIDERS,NDIV,16);
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,PLL_DIVIDERS,POST_DIV,1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_PLL_DIVIDERSr(unit,ciC,rval));
                    /* Reset */
                    SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_PLL_CONFIGr(unit,ciC,&rval));
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,PLL_CONFIG,RESET,0);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_PLL_CONFIGr(unit,ciC,rval));

                    SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_PLL_CONFIGr(unit,ciC,&rval));
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,PLL_CONFIG,RESET,1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_PLL_CONFIGr(unit,ciC,rval));
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,PLL_CONFIG,RESET,0);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_PLL_CONFIGr(unit,ciC,rval));
                }
                
                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C03. Poll PLL Lock\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    soc_timeout_init(&to, to_val, 0);
                    do {
                        SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_PLL_STATUSr(unit,ciC,&rval));
                        if (DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, PLL_STATUS, LOCK)) {
                            break;
                        }
                        if (soc_timeout_check(&to)) {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "CI%d: Timed out waiting for DDR PHY PLL to Lock\n"), ciC));
                            return SOC_E_TIMEOUT;
                        }
                    } while (TRUE);
                }
                
                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C04. Calibrate ZQ (ddr40_phy_calib_zq)\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,&rval));
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,ZQ_PVT_COMP_CTL,SAMPLE_EN,0);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,rval));

                    SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,&rval));
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,ZQ_PVT_COMP_CTL,SAMPLE_EN,1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,rval));
                }
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    soc_timeout_init(&to, to_val, 0);
                    do {
                        SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,&rval));
                        if (DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, ZQ_PVT_COMP_CTL, SAMPLE_DONE)) {
                            break;
                        }
                        if (soc_timeout_check(&to)) {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "CI%d: Timed out waiting for ZQ Calibration\n"), ciC));
                            return SOC_E_TIMEOUT;
                        }
                    } while (TRUE);
                }
                
                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C05. DDR PHY VTT On (Virtual VTT setup) DISABLE all Virtual VTT\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    rval = 0x3f1ffff;  /* all bits except RESET and CKE */
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VIRTUAL_VTT_CONNECTIONSr(unit,ciC,rval));

                    rval = 0; /* AGUTMANN disable VTT override , previously 0x1ffff; */    
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VIRTUAL_VTT_OVERRIDEr(unit,ciC,rval));

                }

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C06. DDR40_PHY_DDR3_MISC\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    rval = 0;
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,DRIVE_PAD_CTL,VDDO_VOLTS,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,DRIVE_PAD_CTL,RT120B_G,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,DRIVE_PAD_CTL,RT60B,1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_DRIVE_PAD_CTLr(unit,ciC,rval));

                    rval = 0;
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,DRIVE_PAD_CTL,VDDO_VOLTS,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,DRIVE_PAD_CTL,RT120B_G,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,DRIVE_PAD_CTL,NO_DQS_RD,0);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_0_DRIVE_PAD_CTLr(unit,ciC,rval));
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_1_DRIVE_PAD_CTLr(unit,ciC,rval));
                }
                
            /*    SOC_IF_ERROR_RETURN(READ_CI_PHY_CONTROLr(unit,0,&data));
                mhz = soc_reg_field_get(unit, CI_PHY_CONTROLr, data, DDR_MHZf);
                mhz = SOC_DDR3_CLOCK_MHZ(unit);
            */
            
                if (!SAL_BOOT_QUICKTURN) {
                  uint32 rd_en_byte_mode=0, rd_en_byte_vdl_steps=0, rd_en_bit_vdl_offset=0;

                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "C07. VDL Calibration\n")));
                    for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                        if(!_check_dram_either(ciC)) {
                            continue;
                        }
                        rval = 0;
                        SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit,ciC,rval));

                        rval = 0;
                        DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,VDL_CALIBRATE,CALIB_FAST,1);
                        DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,VDL_CALIBRATE,CALIB_ONCE,1);
                        DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,VDL_CALIBRATE,CALIB_AUTO,1);
                        SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit,ciC,rval));
                    }
                    for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                        if(!_check_dram_either(ciC)) {
                            continue;
                        }
                        soc_timeout_init(&to, to_val, 0);
                        do {
                            SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit,ciC,&rval));
                            if (DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_IDLE)) {
                                break;
                            }
                            if (soc_timeout_check(&to)) {
                                LOG_ERROR(BSL_LS_SOC_COMMON,
                                          (BSL_META_U(unit,
                                                      "CI%d: Timed out waiting for VDL Calibration Idle\n"), ciC));
                                return SOC_E_TIMEOUT;
                            }
                        } while (TRUE);
                        SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit,ciC,&rval));
                        if (soc_property_get(unit, spn_DIAG_EMULATOR_PARTIAL_INIT, 0x0) == 0) {
                            if (0 == DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_LOCK)) {
                                LOG_ERROR(BSL_LS_SOC_COMMON,
                                          (BSL_META_U(unit,
                                                      "CI%d: VDL Calibration Did Not Lock reg=%x \n"), ciC, rval));
                                return SOC_E_FAIL;
                            } 
                        }

                        LOG_INFO(BSL_LS_SOC_DDR,
                                 (BSL_META_U(unit,
                                             "VDL calibration result: 0x%0x (cal_steps = %d)\n"),
                                  rval, DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_TOTAL) >> 4));

                        /* clear VDL calib control */
                        SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit,ciC,0));

                    }
                    for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                        if(!_check_dram_either(ciC)) {
                            continue;
                        }

                        SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_RD_EN_CALIB_STATUSr(unit,ciC,&rval));
                        rd_en_byte_mode = DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_BYTE_SEL);
                        rd_en_byte_vdl_steps = DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_TOTAL) >> 4;
                        rd_en_bit_vdl_offset = DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_BIT_OFFSET);
                        COMPILER_REFERENCE(rd_en_byte_mode);
                        COMPILER_REFERENCE(rd_en_byte_vdl_steps);
                        COMPILER_REFERENCE(rd_en_bit_vdl_offset);

                        /* rval = 0;
                           SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit,ci,rval));
                           DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,VDL_CALIBRATE,CALIB_STEPS,1);
                           SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit,ci,rval));
                        */
                    }
                    for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                        if(!_check_dram_either(ciC)) {
                            continue;
                        }
                        soc_timeout_init(&to, to_val, 0);
                        do {
                            SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit,ciC,&rval));
                            if (DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_IDLE)) {
                                break;
                            }
                            if (soc_timeout_check(&to)) {
                                LOG_ERROR(BSL_LS_SOC_COMMON,
                                          (BSL_META_U(unit,
                                                      "CI%d: Timed out waiting for VDL Calibration Idle(1)\n"), ciC));
                                return SOC_E_TIMEOUT;
                            }
                        } while (TRUE);
                        SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit,ciC,&rval));
                        if (soc_property_get(unit, spn_DIAG_EMULATOR_PARTIAL_INIT, 0x0) == 0) {
                            if (0 == DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_LOCK)) {
                                LOG_ERROR(BSL_LS_SOC_COMMON,
                                          (BSL_META_U(unit,
                                                      "CI%d: VDL Calibration Did Not Lock(1)\n"), ciC));
                                return SOC_E_FAIL;
                            } 
                        }
                    }
                } else {
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "C07. VDL Calibration SKIPPED\n")));
                }/* Not QUICKTURN */

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C08. DDR40_PHY_DDR3_MISC : Start DDR40_PHY_RDLY_ODT....\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    rval = 0;
                    #if 0 /* def BCM_PETRA_SUPPORT */
                    DDR40_SET_FIELD(rval, DDR40_PHY_WORD_LANE_0, READ_DATA_DLY, RD_DATA_DLY, arad_dram_read_data_dly_get(unit, SOC_DDR3_CLOCK_MHZ(unit)));
                    #else
                    DDR40_SET_FIELD(rval, DDR40_PHY_WORD_LANE_0, READ_DATA_DLY, RD_DATA_DLY, 1);
                    #endif
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_0_READ_DATA_DLYr(unit, ciC, rval));
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_1_READ_DATA_DLYr(unit, ciC, rval));

                    rval = 0;
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,READ_CONTROL,DQ_ODT_ENABLE,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,READ_CONTROL,DQ_ODT_LE_ADJ,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,READ_CONTROL,DQ_ODT_TE_ADJ,1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_0_READ_CONTROLr(unit,ciC,rval));
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_1_READ_CONTROLr(unit,ciC,rval));

                    rval = 3;
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_0_WR_PREAMBLE_MODEr(unit,ciC,rval));
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_1_WR_PREAMBLE_MODEr(unit,ciC,rval));
                }

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C09. Start ddr40_phy_autoidle_on (MEM_SYS_PARAM_PHY_AUTO_IDLE) ....\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    rval = 0;
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,IDLE_PAD_CONTROL,AUTO_DQ_IDDQ_MODE,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,IDLE_PAD_CONTROL,AUTO_DQ_RXENB_MODE,1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_0_IDLE_PAD_CONTROLr(unit,ciC,rval));
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_1_IDLE_PAD_CONTROLr(unit,ciC,rval));
                }

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "C10. Wait for Phy Ready\n")));
                /* Skipped for DDR_PHYTYPE_HR2 */
                break;
            default:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Error. Unsupported PHY type\n")));
                return SOC_E_FAIL;
        }
    } else {
        /* report only */
        switch(phyType) {
            case DDR_PHYTYPE_RSVP:
                break;
            case DDR_PHYTYPE_NS:
                break;
            case DDR_PHYTYPE_ENG:
                break;
            case DDR_PHYTYPE_CE:
                break;
            case DDR_PHYTYPE_HR2:
                break;
            default:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Error. Unsupported PHY type\n")));
                return SOC_E_FAIL;
        }
    }
    return SOC_E_NONE;
}

/***********************************************************************
* Functions to reset the controller
 */
int
soc_ddr40_ctlr_reset   (int unit, int ci, uint32 ctlType, int stat)
{
    uint32 rval;

    if (!stat) {
        switch(ctlType) {
            case DDR_CTLR_TRSVP:
                break;
            case DDR_CTLR_T0:
                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "(1) Bring CI out of software reset\n")));
                SOC_IF_ERROR_RETURN(READ_CI_RESETr(unit,ci,&rval));
                soc_reg_field_set(unit, CI_RESETr, &rval, TREX2_DEBUG_ENABLEf, 0);
                soc_reg_field_set(unit, CI_RESETr, &rval, DDR_RESET_Nf, 0);
                soc_reg_field_set(unit, CI_RESETr, &rval, PHY_SW_INITf, 1);
                soc_reg_field_set(unit, CI_RESETr, &rval, SW_RESETf, 0);
                SOC_IF_ERROR_RETURN(WRITE_CI_RESETr(unit,ci,rval));
                sal_usleep(2000);

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "(6) Clear CI DDR Reset\n")));
                SOC_IF_ERROR_RETURN(READ_CI_RESETr(unit,ci,&rval));
                soc_reg_field_set(unit, CI_RESETr, &rval, TREX2_DEBUG_ENABLEf, 0);
                soc_reg_field_set(unit, CI_RESETr, &rval, DDR_RESET_Nf, 1);
                soc_reg_field_set(unit, CI_RESETr, &rval, PHY_SW_INITf, 0);
                soc_reg_field_set(unit, CI_RESETr, &rval, SW_RESETf, 0);
                SOC_IF_ERROR_RETURN(WRITE_CI_RESETr(unit,ci,rval));
                sal_usleep(2000);

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "(6) Set DDR_RST_N\n")));
                SOC_IF_ERROR_RETURN(READ_CI_PHY_CONTROLr(unit,ci,&rval));
                soc_reg_field_set(unit, CI_PHY_CONTROLr, &rval, DDR_MHZf, SOC_DDR3_CLOCK_MHZ(unit));
                soc_reg_field_set(unit, CI_PHY_CONTROLr, &rval, RST_Nf, 3);
                SOC_IF_ERROR_RETURN(WRITE_CI_PHY_CONTROLr(unit,ci,rval));
                sal_usleep(2000);
                break;
            case DDR_CTLR_T1:
                break;
            case DDR_CTLR_T2:
                break;
            case DDR_CTLR_T3:
                break;
            default:
                break;
        }
    } else {
        /* report only */
        switch(ctlType) {
            case DDR_CTLR_TRSVP:
                break;
            case DDR_CTLR_T0:
                break;
            case DDR_CTLR_T1:
                break;
            case DDR_CTLR_T2:
                break;
            case DDR_CTLR_T3:
                break;
            default:
                break;
        }
    }
    return SOC_E_NONE;
}

/***********************************************************************
* Functions to set/report the Controller Parameters
 */
int
soc_ddr40_ctlr_ctl(int unit, int ci, uint32 ctlType, int stat)
{
    uint32 rval;
    int row_loc, freq_loc, grade_loc;
    int ciC;

    KATANAfreq_grade_mem_set_set_t T0fgmss;
/*    ARADfreq_grade_mem_set_set_t T1fgmss; */
    
    row_loc = _soc_ddr40_mem_rowstoloc(SOC_DDR3_NUM_ROWS(unit));
    freq_loc = _soc_ddr40_phy_freqtoloc(SOC_DDR3_CLOCK_MHZ(unit));
    grade_loc = _soc_ddr40_mem_gradetoloc(SOC_DDR3_MEM_GRADE(unit)); 

    if (freq_loc < 0) {
        return SOC_E_INTERNAL; /* Unsupported Frequency */
    }

    if (!stat) {
        switch(ctlType) {
            case DDR_CTLR_TRSVP:
                break;
            case DDR_CTLR_T0:
                _soc_ddr40_katana_phy_init_mem_set_wrapper(unit, &T0fgmss, freq_loc, grade_loc);
                if (T0fgmss.mhz == -1) {
                    return SOC_E_INTERNAL; /* Unsupported Frequency / Grade Combination */
                }

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "B01. Bring CI out of software reset\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC++) {
                    if(!_check_dram(ciC)) {
                        continue;
                    }
                    SOC_IF_ERROR_RETURN(READ_CI_RESETr(unit,ciC,&rval));
                    soc_reg_field_set(unit, CI_RESETr, &rval, TREX2_DEBUG_ENABLEf, 0);
                    soc_reg_field_set(unit, CI_RESETr, &rval, DDR_RESET_Nf, 0);
                    soc_reg_field_set(unit, CI_RESETr, &rval, PHY_SW_INITf, 1);
                    soc_reg_field_set(unit, CI_RESETr, &rval, SW_RESETf, 0);
                    SOC_IF_ERROR_RETURN(WRITE_CI_RESETr(unit,ciC,rval));
                    sal_usleep(2000);
                }

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "B02. Set Strap and parameter per speed and grade\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC++) {
                    if(!_check_dram(ciC)) {
                        continue;
                    }
                    SOC_IF_ERROR_RETURN(READ_CI_CONFIG0r(unit,ciC,&rval));
                    soc_reg_field_set(unit, CI_CONFIG0r, &rval, TWLf, (ciC & 0x1)?(T0fgmss.twl-1):T0fgmss.twl);
                    soc_reg_field_set(unit, CI_CONFIG0r, &rval, TWRf, T0fgmss.twr);
                    soc_reg_field_set(unit, CI_CONFIG0r, &rval, TRCf, T0fgmss.trc);
                    SOC_IF_ERROR_RETURN(WRITE_CI_CONFIG0r(unit,ciC,rval));

                    SOC_IF_ERROR_RETURN(READ_CI_CONFIG1r(unit,ciC,&rval));
                    soc_reg_field_set(unit, CI_CONFIG1r, &rval, RFIFO_ALMOST_FULL_THRESHOLDf, T0fgmss.rfifo_afull);
                    soc_reg_field_set(unit, CI_CONFIG1r, &rval, RFIFO_FULL_THRESHOLDf, T0fgmss.rfifo_full);
                    SOC_IF_ERROR_RETURN(WRITE_CI_CONFIG1r(unit,ciC,rval));

                    SOC_IF_ERROR_RETURN(READ_CI_CONFIG2r(unit,ciC,&rval));
                    soc_reg_field_set(unit, CI_CONFIG2r, &rval, TRTWf, T0fgmss.trtw);
                    soc_reg_field_set(unit, CI_CONFIG2r, &rval, TWTRf, T0fgmss.twtr);
                    soc_reg_field_set(unit, CI_CONFIG2r, &rval, TFAWf, T0fgmss.tfaw);
                    soc_reg_field_set(unit, CI_CONFIG2r, &rval, TREAD_ENBf, (ciC & 0x1)?(T0fgmss.tread_enb-1):T0fgmss.tread_enb);
                    SOC_IF_ERROR_RETURN(WRITE_CI_CONFIG2r(unit,ciC,rval));

                    SOC_IF_ERROR_RETURN(READ_CI_CONFIG3r(unit,ciC,&rval));
                    soc_reg_field_set(unit, CI_CONFIG3r, &rval, BANK_UNAVAILABLE_WRf, T0fgmss.bank_unavail_wr);
                    soc_reg_field_set(unit, CI_CONFIG3r, &rval, BANK_UNAVAILABLE_RDf, T0fgmss.bank_unavail_rd);
                    soc_reg_field_set(unit, CI_CONFIG3r, &rval, RR_READf, T0fgmss.rr_read);
                    soc_reg_field_set(unit, CI_CONFIG3r, &rval, RR_WRITEf, T0fgmss.rr_write);
                    SOC_IF_ERROR_RETURN(WRITE_CI_CONFIG3r(unit,ciC,rval));

                    SOC_IF_ERROR_RETURN(READ_CI_CONFIG6r(unit,ciC,&rval));
                    soc_reg_field_set(unit, CI_CONFIG6r, &rval, TRFCf, T0fgmss.trfc[row_loc]);
                    soc_reg_field_set(unit, CI_CONFIG6r, &rval, TRP_READf, T0fgmss.trp_read);
                    soc_reg_field_set(unit, CI_CONFIG6r, &rval, TRP_WRITEf, T0fgmss.trp_write);
                    SOC_IF_ERROR_RETURN(WRITE_CI_CONFIG6r(unit,ciC,rval));

                    SOC_IF_ERROR_RETURN(READ_CI_CONFIG7r(unit,ciC,&rval));
                    soc_reg_field_set(unit, CI_CONFIG7r, &rval, TZQf, T0fgmss.tzq);
                    SOC_IF_ERROR_RETURN(WRITE_CI_CONFIG7r(unit,ciC,rval));

                    SOC_IF_ERROR_RETURN(READ_CI_PHY_STRAPS0r(unit,ciC,&rval));
                    soc_reg_field_set(unit, CI_PHY_STRAPS0r, &rval, CLf, T0fgmss.cl);
                    soc_reg_field_set(unit, CI_PHY_STRAPS0r, &rval, CWLf, T0fgmss.cwl);
                    soc_reg_field_set(unit, CI_PHY_STRAPS0r, &rval, WRf, T0fgmss.wr);
                    soc_reg_field_set(unit, CI_PHY_STRAPS0r, &rval, CHIP_SIZEf,
                            SOC_IS_KATANA2(unit) ? kt2_smts[row_loc].chip_siz : smts[row_loc].chip_siz);
                    soc_reg_field_set(unit, CI_PHY_STRAPS0r, &rval, AD_WIDTHf,
                            SOC_IS_KATANA2(unit) ? kt2_smts[row_loc].ad_width : smts[row_loc].ad_width);
                    SOC_IF_ERROR_RETURN(WRITE_CI_PHY_STRAPS0r(unit,ciC,rval));

                    SOC_IF_ERROR_RETURN(READ_CI_PHY_STRAPS1r(unit,ciC,&rval));
                    soc_reg_field_set(unit, CI_PHY_STRAPS1r, &rval, JEDECf, T0fgmss.jedec);
                    soc_reg_field_set(unit, CI_PHY_STRAPS1r, &rval, MHZf, T0fgmss.mhz);
                    SOC_IF_ERROR_RETURN(WRITE_CI_PHY_STRAPS1r(unit,ciC,rval));
                }

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "B03. Clear CI Phy reset\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC++) {
                    if(!_check_dram(ciC)) {
                        continue;
                    }
                    rval = 0;
                    SOC_IF_ERROR_RETURN(WRITE_CI_RESETr(unit,ciC,rval));
                }
                sal_usleep(2000);

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "B04. Set config MR0, MR1, and MR2 registers\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC++) {
                    if(!_check_dram(ciC)) {
                        continue;
                    }
                    SOC_IF_ERROR_RETURN(READ_CI_DDR_MR0r(unit,ciC,&rval)); 
                    soc_reg_field_set(unit, CI_DDR_MR0r, &rval, Af, T0fgmss.mr0);
                    SOC_IF_ERROR_RETURN(WRITE_CI_DDR_MR0r(unit,ciC,rval));

                    SOC_IF_ERROR_RETURN(READ_CI_DDR_MR1r(unit,ciC,&rval));
                    soc_reg_field_set(unit, CI_DDR_MR1r, &rval, Af, 0x10);
                    SOC_IF_ERROR_RETURN(WRITE_CI_DDR_MR1r(unit,ciC,rval));

                    SOC_IF_ERROR_RETURN(READ_CI_DDR_MR2r(unit,ciC,&rval)); 
                    soc_reg_field_set(unit, CI_DDR_MR2r, &rval, Af, T0fgmss.mr2); 
                    SOC_IF_ERROR_RETURN(WRITE_CI_DDR_MR2r(unit,ciC,rval));
                }

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "B05. Clear CI DDR Reset\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC++) {
                    if(!_check_dram(ciC)) {
                        continue;
                    }
                    SOC_IF_ERROR_RETURN(READ_CI_RESETr(unit,ciC,&rval));
                    soc_reg_field_set(unit, CI_RESETr, &rval, TREX2_DEBUG_ENABLEf, 0);
                    soc_reg_field_set(unit, CI_RESETr, &rval, DDR_RESET_Nf, 1);
                    soc_reg_field_set(unit, CI_RESETr, &rval, PHY_SW_INITf, 0);
                    soc_reg_field_set(unit, CI_RESETr, &rval, SW_RESETf, 0);
                    SOC_IF_ERROR_RETURN(WRITE_CI_RESETr(unit,ciC,rval));
                }
                sal_usleep(2000);

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "B06. Set DDR_RST_N\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC++) {
                    if(!_check_dram(ciC)) {
                        continue;
                    }
                    SOC_IF_ERROR_RETURN(READ_CI_PHY_CONTROLr(unit,ciC,&rval));
                    soc_reg_field_set(unit, CI_PHY_CONTROLr, &rval, DDR_MHZf, T0fgmss.mhz);
                    soc_reg_field_set(unit, CI_PHY_CONTROLr, &rval, RST_Nf, 3);
                    SOC_IF_ERROR_RETURN(WRITE_CI_PHY_CONTROLr(unit,ciC,rval));
                }
                sal_usleep(2000);

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "B06.1 Write 0x1 to PHY_STRAPS_CONTROL\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    rval = 0x1;
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_STRAP_CONTROLr(unit,ciC,rval));
                }
                sal_usleep(2000);

                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "B07. Make sure PHY AUTOINIT at strap is NOT set\n")));
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC++) {
                    if(!_check_dram(ciC)) {
                        continue;
                    }
                    SOC_IF_ERROR_RETURN(READ_CI_PHY_CONTROLr(unit,ciC,&rval));
                    soc_reg_field_set(unit, CI_PHY_CONTROLr, &rval, AUTO_INITf, 0);
                    soc_reg_field_set(unit, CI_PHY_CONTROLr, &rval, CKEf, 0);
                    SOC_IF_ERROR_RETURN(WRITE_CI_PHY_CONTROLr(unit,ciC,rval));
                }
                sal_usleep(2000);
                break;
            case DDR_CTLR_T1:
                break;
            case DDR_CTLR_T2:
                break;
            case DDR_CTLR_T3:
                break;
            default:
                break;
        }
    } else {
        /* report only */
        switch(ctlType) {
            case DDR_CTLR_TRSVP:
                break;
            case DDR_CTLR_T0:
                break;
            case DDR_CTLR_T1:
                break;
            case DDR_CTLR_T2:
                break;
            case DDR_CTLR_T3:
                break;
            default:
                break;
        }
    }
    return SOC_E_NONE;
}

/***********************************************************************
* Functions to set/report the Controller Parameters
 */
int
soc_ddr40_ctlr_zqcal_ctl(int unit, int ci, uint32 ctlType, int stat)
{
    if (!stat) {
        switch(ctlType) {
            case DDR_CTLR_TRSVP:
                break;
            case DDR_CTLR_T0:
                break;
            case DDR_CTLR_T1:
                break;
            case DDR_CTLR_T2:
                break;
            case DDR_CTLR_T3:
                break;
            default:
                break;
        }
    } else {
        /* report only */
        switch(ctlType) {
            case DDR_CTLR_TRSVP:
                break;
            case DDR_CTLR_T0:
                break;
            case DDR_CTLR_T1:
                break;
            case DDR_CTLR_T2:
                break;
            case DDR_CTLR_T3:
                break;
            default:
                break;
        }
    }
    return SOC_E_NONE;
}

/***********************************************************************
* Functions to Read / Write / Modify DDR Memory
 */
int _DDRWrRdComplete(int unit, int ci, uint32 uTimeout)
{
    uint32 i;
    uint32 data = 0;
    uint32 uAck = 0;

    /* wait for the ACK to indicate rd/wr op is finished */
    for (i = 0; i < uTimeout; i++) {
        SOC_IF_ERROR_RETURN(READ_CI_MEM_ACC_CTRLr(unit, ci, &data));
        uAck = soc_reg_field_get(unit, CI_MEM_ACC_CTRLr, data, MEM_ACC_ACKf);
        if (uAck) {
            return SOC_E_NONE;
        }
        sal_udelay(10);
    }

    /* timed out */
    return (SOC_E_TIMEOUT);
}

int soc_ddr40_read(int unit, int ci, uint32 addr, uint32 * pData0,
                   uint32 * pData1, uint32 * pData2, uint32 * pData3,
                   uint32 * pData4, uint32 * pData5, uint32 * pData6,
                   uint32 * pData7)
{

    uint32 uCmd;

    SOC_IF_ERROR_RETURN(WRITE_CI_MEM_ACC_DATA0r(unit, ci, 0));
    SOC_IF_ERROR_RETURN(WRITE_CI_MEM_ACC_DATA1r(unit, ci, 0));
    SOC_IF_ERROR_RETURN(WRITE_CI_MEM_ACC_DATA2r(unit, ci, 0));
    SOC_IF_ERROR_RETURN(WRITE_CI_MEM_ACC_DATA3r(unit, ci, 0));
    SOC_IF_ERROR_RETURN(WRITE_CI_MEM_ACC_DATA4r(unit, ci, 0));
    SOC_IF_ERROR_RETURN(WRITE_CI_MEM_ACC_DATA5r(unit, ci, 0));
    SOC_IF_ERROR_RETURN(WRITE_CI_MEM_ACC_DATA6r(unit, ci, 0));
    SOC_IF_ERROR_RETURN(WRITE_CI_MEM_ACC_DATA7r(unit, ci, 0));

    uCmd = 0;
    soc_reg_field_set(unit, CI_MEM_ACC_CTRLr, &uCmd, MEM_ACC_ACKf, 1);
    soc_reg_field_set(unit, CI_MEM_ACC_CTRLr, &uCmd, MEM_ACC_REQf, 1);
    soc_reg_field_set(unit, CI_MEM_ACC_CTRLr, &uCmd, MEM_ACC_RD_WR_Nf, 1);
    soc_reg_field_set(unit, CI_MEM_ACC_CTRLr, &uCmd, MEM_ACC_ADDRf, addr);
    SOC_IF_ERROR_RETURN(WRITE_CI_MEM_ACC_CTRLr(unit, ci, uCmd));

    if (SOC_E_NONE != _DDRWrRdComplete(unit, ci, DDR_TIMEOUT_10mS)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Error timeout reading from CI:%d addr:0x%x\n"),
                   ci, addr));
        return SOC_E_TIMEOUT;
    }

    SOC_IF_ERROR_RETURN(READ_CI_MEM_ACC_DATA0r(unit, ci, pData0));
    SOC_IF_ERROR_RETURN(READ_CI_MEM_ACC_DATA1r(unit, ci, pData1));
    SOC_IF_ERROR_RETURN(READ_CI_MEM_ACC_DATA2r(unit, ci, pData2));
    SOC_IF_ERROR_RETURN(READ_CI_MEM_ACC_DATA3r(unit, ci, pData3));
    SOC_IF_ERROR_RETURN(READ_CI_MEM_ACC_DATA4r(unit, ci, pData4));
    SOC_IF_ERROR_RETURN(READ_CI_MEM_ACC_DATA5r(unit, ci, pData5));
    SOC_IF_ERROR_RETURN(READ_CI_MEM_ACC_DATA6r(unit, ci, pData6));
    SOC_IF_ERROR_RETURN(READ_CI_MEM_ACC_DATA7r(unit, ci, pData7));
    return SOC_E_NONE;
}

int soc_ddr40_write(int unit, int ci, uint32 addr, uint32 uData0,
                    uint32 uData1, uint32 uData2, uint32 uData3,
                    uint32 uData4, uint32 uData5, uint32 uData6, uint32 uData7)
{

    uint32 uCmd;

    /* setup the data */
    SOC_IF_ERROR_RETURN(WRITE_CI_MEM_ACC_DATA0r(unit, ci, uData0));
    SOC_IF_ERROR_RETURN(WRITE_CI_MEM_ACC_DATA1r(unit, ci, uData1));
    SOC_IF_ERROR_RETURN(WRITE_CI_MEM_ACC_DATA2r(unit, ci, uData2));
    SOC_IF_ERROR_RETURN(WRITE_CI_MEM_ACC_DATA3r(unit, ci, uData3));
    SOC_IF_ERROR_RETURN(WRITE_CI_MEM_ACC_DATA4r(unit, ci, uData4));
    SOC_IF_ERROR_RETURN(WRITE_CI_MEM_ACC_DATA5r(unit, ci, uData5));
    SOC_IF_ERROR_RETURN(WRITE_CI_MEM_ACC_DATA6r(unit, ci, uData6));
    SOC_IF_ERROR_RETURN(WRITE_CI_MEM_ACC_DATA7r(unit, ci, uData7));

    uCmd = 0;
    soc_reg_field_set(unit, CI_MEM_ACC_CTRLr, &uCmd, MEM_ACC_ACKf, 1);
    soc_reg_field_set(unit, CI_MEM_ACC_CTRLr, &uCmd, MEM_ACC_REQf, 1);
    soc_reg_field_set(unit, CI_MEM_ACC_CTRLr, &uCmd, MEM_ACC_RD_WR_Nf, 0);
    soc_reg_field_set(unit, CI_MEM_ACC_CTRLr, &uCmd, MEM_ACC_ADDRf, addr);
    SOC_IF_ERROR_RETURN(WRITE_CI_MEM_ACC_CTRLr(unit, ci, uCmd));

    if (SOC_E_NONE != _DDRWrRdComplete(unit, ci, DDR_TIMEOUT_10mS)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Error timeout writing to CI:%d addr:0x%x\n"), ci,
                   addr));
        return SOC_E_TIMEOUT;
    }
    return SOC_E_NONE;
}

/***********************************************************************
* Functions to Read / Write / Modify DDR Phy Registers
 */
#ifdef BCM_PETRA_SUPPORT
int
soc_ddr40_arad_phy_reg_ci_read(int unit, int ci, uint32 reg_addr, uint32 * reg_data)
{
    arad_dram_rbus_read(unit, (ci>>1), reg_addr, reg_data);
    return SOC_E_NONE;
}

int
soc_ddr40_arad_phy_reg_ci_write(int unit, int ci, uint32 reg_addr, uint32 reg_data)
{
    arad_dram_rbus_write(unit, (ci>>1), reg_addr, reg_data);
    return SOC_E_NONE;
}

int
soc_ddr40_arad_phy_reg_ci_modify(int unit, uint32 ci, uint32 reg_addr, uint32 data, uint32 mask)
{
    arad_dram_rbus_modify(unit, (ci>>1), reg_addr, data, mask);
    return SOC_E_NONE;
}
#endif

int
soc_ddr40_katana_phy_reg_ci_read(int unit, int ci, uint32 reg_addr, uint32 * reg_data)
{
    uint32 rval;
    soc_timeout_t to;

    rval = 0;
    soc_reg_field_set(unit, CI_DDR_PHY_REG_CTRLr, &rval, PHY_REG_ACKf, 1);      /* write 1 to clear */
    soc_reg_field_set(unit, CI_DDR_PHY_REG_CTRLr, &rval, PHY_REG_ERR_ACKf, 1);  /* write 1 to clear */
    soc_reg_field_set(unit, CI_DDR_PHY_REG_CTRLr, &rval, PHY_REG_ADDRf,
                      reg_addr);
    soc_reg_field_set(unit, CI_DDR_PHY_REG_CTRLr, &rval, PHY_REG_RD_WR_Nf, 1);
    soc_reg_field_set(unit, CI_DDR_PHY_REG_CTRLr, &rval, PHY_REG_REQf, 1);
    SOC_IF_ERROR_RETURN(WRITE_CI_DDR_PHY_REG_CTRLr(unit, ci, rval));

    if (SAL_BOOT_SIMULATION) {
        soc_timeout_init(&to, 10000000, 0);     /* 10 Sec */
    } else {
        soc_timeout_init(&to, 50000, 0);        /* 50 mS */
    }
    do {
        SOC_IF_ERROR_RETURN(READ_CI_DDR_PHY_REG_CTRLr(unit, ci, &rval));
        if (soc_reg_field_get(unit, CI_DDR_PHY_REG_CTRLr, rval, PHY_REG_ACKf)) {
            break;
        }
        if (soc_reg_field_get
            (unit, CI_DDR_PHY_REG_CTRLr, rval, PHY_REG_ERR_ACKf)) {
            return SOC_E_FAIL;
        }
        if (soc_timeout_check(&to)) {
            return SOC_E_TIMEOUT;
        }
    } while (TRUE);
    SOC_IF_ERROR_RETURN(READ_CI_DDR_PHY_REG_DATAr(unit, ci, reg_data));
    return SOC_E_NONE;
}

/* DDR regs Indirect Write */
int
soc_ddr40_katana_phy_reg_ci_write(int unit, int ci, uint32 reg_addr, uint32 reg_data)
{
    uint32 rval;
    soc_timeout_t to;

    SOC_IF_ERROR_RETURN(WRITE_CI_DDR_PHY_REG_DATAr(unit, ci, reg_data));

    rval = 0;
    soc_reg_field_set(unit, CI_DDR_PHY_REG_CTRLr, &rval, PHY_REG_ACKf, 1);      /* write 1 to clear */
    soc_reg_field_set(unit, CI_DDR_PHY_REG_CTRLr, &rval, PHY_REG_ERR_ACKf, 1);  /* write 1 to clear */
    soc_reg_field_set(unit, CI_DDR_PHY_REG_CTRLr, &rval, PHY_REG_ADDRf,
                      reg_addr);
    soc_reg_field_set(unit, CI_DDR_PHY_REG_CTRLr, &rval, PHY_REG_RD_WR_Nf, 0);
    soc_reg_field_set(unit, CI_DDR_PHY_REG_CTRLr, &rval, PHY_REG_REQf, 1);
    SOC_IF_ERROR_RETURN(WRITE_CI_DDR_PHY_REG_CTRLr(unit, ci, rval));

    if (SAL_BOOT_SIMULATION) {
        soc_timeout_init(&to, 10000000, 0);     /* 10 Sec */
    } else {
        soc_timeout_init(&to, 50000, 0);        /* 50 mS */
    }
    do {
        SOC_IF_ERROR_RETURN(READ_CI_DDR_PHY_REG_CTRLr(unit, ci, &rval));
        if (soc_reg_field_get(unit, CI_DDR_PHY_REG_CTRLr, rval, PHY_REG_ACKf)) {
            break;
        }
        if (soc_reg_field_get
            (unit, CI_DDR_PHY_REG_CTRLr, rval, PHY_REG_ERR_ACKf)) {
            return SOC_E_FAIL;
        }
        if (soc_timeout_check(&to)) {
            return SOC_E_TIMEOUT;
        }
    } while (TRUE);
    return SOC_E_NONE;
}

int
soc_ddr40_katana_phy_reg_ci_modify(int unit, uint32 ci, uint32 reg_addr, uint32 data,
                            uint32 mask)
{
    int rv = SOC_E_NONE;
    uint32 tmp, otmp;

    data = data & mask;

    SOC_IF_ERROR_RETURN(soc_ddr40_katana_phy_reg_ci_read(unit, ci, reg_addr, &tmp));
    otmp = tmp;
    tmp &= ~(mask);
    tmp |= data;

    if (otmp != tmp) {
        SOC_IF_ERROR_RETURN
            (soc_ddr40_katana_phy_reg_ci_write(unit, ci, reg_addr, tmp));
    }

    return rv;
}

#ifdef BCM_HURRICANE2_SUPPORT
int
soc_ddr40_hurricane2_phy_reg_ci_read(
    int unit, int ci, uint32 reg_addr, uint32 *reg_data)
{
    uint32 base;
    uint32 rval;
    
    if (reg_data == NULL) {
        return SOC_E_PARAM;
    }

    /* Get DDR PHY register base address */
    base = soc_reg_addr(unit, DDR_PHY_CONTROL_REGS_REVISIONr, REG_PORT_ANY, 0);
    
    rval = soc_iproc_getreg(unit, base + reg_addr, reg_data);
    return rval;
}

int
soc_ddr40_hurricane2_phy_reg_ci_write(
    int unit, int ci, uint32 reg_addr, uint32 reg_data)
{
    int rv;
    uint32 base;
    uint32 dummy;
    
    /* Get DDR PHY register base address */
    base = soc_reg_addr(unit, DDR_PHY_CONTROL_REGS_REVISIONr, REG_PORT_ANY, 0);
    
    rv = soc_iproc_setreg(unit, base + reg_addr, reg_data);
    
    /* Need to read a PHY register after PHY write */
    if (rv == SOC_E_NONE) {
        (void)soc_iproc_getreg(unit, base, &dummy);
    }
    
    return rv;
}

int
soc_ddr40_hurricane2_phy_reg_ci_modify(
    int unit, uint32 ci, uint32 reg_addr, uint32 data, uint32 mask)
{
    int rv = SOC_E_NONE;
    uint32 tmp, otmp;

    data = data & mask;

    SOC_IF_ERROR_RETURN(
        soc_ddr40_hurricane2_phy_reg_ci_read(unit, ci, reg_addr, &tmp));
    otmp = tmp;
    tmp &= ~(mask);
    tmp |= data;

    if (otmp != tmp) {
        SOC_IF_ERROR_RETURN(
            soc_ddr40_hurricane2_phy_reg_ci_write(unit, ci, reg_addr, tmp));
    }

    return rv;
}
#endif /* BCM_HURRICANE2_SUPPORT */

int
soc_ddr40_phy_reg_ci_read(int unit, int ci, uint32 reg_addr, uint32 * reg_data)
{
    if(SOC_IS_KATANAX(unit)) {
        return soc_ddr40_katana_phy_reg_ci_read(unit, ci, reg_addr, reg_data);
#ifdef BCM_PETRA_SUPPORT
    } else if(SOC_IS_ARAD(unit)) {
        return soc_ddr40_arad_phy_reg_ci_read(unit, ci, reg_addr, reg_data);
#endif
#ifdef BCM_HURRICANE2_SUPPORT
    } else if(SOC_IS_HURRICANE2(unit)) {
        return soc_ddr40_hurricane2_phy_reg_ci_read(
                    unit, ci, reg_addr, reg_data);
#endif
    } else {
        return SOC_E_INTERNAL;     /* Unsupported configuration */
    }
}

int
soc_ddr40_phy_reg_ci_write(int unit, int ci, uint32 reg_addr, uint32 reg_data)
{
    if(SOC_IS_KATANAX(unit)) {
        return soc_ddr40_katana_phy_reg_ci_write(unit, ci, reg_addr, reg_data);
#ifdef BCM_PETRA_SUPPORT
    } else if(SOC_IS_ARAD(unit)) {
        return soc_ddr40_arad_phy_reg_ci_write(unit, ci, reg_addr, reg_data);
#endif
#ifdef BCM_HURRICANE2_SUPPORT
    } else if(SOC_IS_HURRICANE2(unit)) {
        return soc_ddr40_hurricane2_phy_reg_ci_write(
                    unit, ci, reg_addr, reg_data);
#endif
    } else {
        return SOC_E_INTERNAL;     /* Unsupported configuration */
    }
}

int
soc_ddr40_phy_reg_ci_modify(int unit, uint32 ci, uint32 reg_addr, uint32 data,
                            uint32 mask)
{
    if(SOC_IS_KATANAX(unit)) {
        return soc_ddr40_katana_phy_reg_ci_modify(unit, ci, reg_addr, data, mask);
#ifdef BCM_PETRA_SUPPORT
    } else if(SOC_IS_ARAD(unit)) {
        return soc_ddr40_arad_phy_reg_ci_modify(unit, ci, reg_addr, data, mask);
#endif
#ifdef BCM_HURRICANE2_SUPPORT
    } else if(SOC_IS_HURRICANE2(unit)) {
        return soc_ddr40_hurricane2_phy_reg_ci_modify(
                    unit, ci, reg_addr, data, mask);
#endif
    } else {
        return SOC_E_INTERNAL;     /* Unsupported configuration */
    }
}


/***********************************************************************
* Shmoo Functions
 */

uint32 _get_random(void)
{
    static uint32 m_w = 6483;    /* must not be zero */
    static uint32 m_z = 31245;    /* must not be zero */
    
    m_z = 36969 * (m_z & 65535) + (m_z >> 16);
    m_w = 18000 * (m_w & 65535) + (m_w >> 16);
    return (m_z << 16) + m_w;  /* 32-bit result */
}

/* Generate NUM_DATA uint32 data for memory accesses */
STATIC void _fill_in_data(uint32 * data)
{
    int32 i;

    for (i = 0; i < NUM_DATA; i++) {
        if (i & 0x1) {
            data[i] = 0xa5a5a500 | i;
        } else {
            data[i] = 0x5a5a5a00 | i;
        }
    }
    return;
}

STATIC void _test_mem_write(int unit, int ci, int addr, uint32 * uDataWR)
{
    if(SOC_IS_KATANAX(unit)) {
        int32 i;
        for (i = 0; i < NUM_DATA / 8; i++) {
            soc_ddr40_write(unit, ci, addr + i, uDataWR[i * 8],
                        uDataWR[(i * 8) + 1], uDataWR[(i * 8) + 2],
                        uDataWR[(i * 8) + 3], uDataWR[(i * 8) + 4],
                        uDataWR[(i * 8) + 5], uDataWR[(i * 8) + 6],
                        uDataWR[(i * 8) + 7]);
        }
    }
}

STATIC uint32 _test_rd_data_fifo_status(int unit, int ci, int wl, int loop)
{
    int i;
    uint32 addr;
    uint32 uDataRD[NUM_DATA];
    uint32 rval = 0;
    uint32 result_fail = 1;

    if(!SOC_IS_KATANAX(unit)) {
        return result_fail;
    }

    addr = (_get_random() & 0xFFFFFF);
    for (i = 0; i < NUM_DATA / 8; i++) {
        soc_ddr40_read(unit, (ci + wl), addr + i, &uDataRD[(i * 8)],
                       &uDataRD[(i * 8) + 1], &uDataRD[(i * 8) + 2],
                       &uDataRD[(i * 8) + 3], &uDataRD[(i * 8) + 4],
                       &uDataRD[(i * 8) + 5], &uDataRD[(i * 8) + 6],
                       &uDataRD[(i * 8) + 7]);
    }
    rval = 0;
    if (wl == 0) {
        READ_DDR40_PHY_WORD_LANE_0_READ_FIFO_STATUSr(unit, ci, &rval);
    } else {
        READ_DDR40_PHY_WORD_LANE_1_READ_FIFO_STATUSr(unit, ci, &rval);
    }

    result_fail = (rval>0) ? 0xffff : 0;

    return result_fail;
}

uint32
_test_func_self_test_1_katana(int unit, int ci, int wl, int loop, uint32 flag)
{
    uint32 i;
    int j, cinum;
    uint32 rval = 0;
    uint32 result_fail = 0;
    uint32 wdata[16];
    uint32 rdata[16];
    uint32 *rdPtr;
    int k, m;
    uint32 data = 0, fifo_status0, fifo_status1;
    
    uint32 startAddr;
    
    uint32 efold = 0;
    
    rdPtr = &rdata[0];

    /* Alternate data test */
    cinum = ci + wl;
    j = 0;
    while (j < loop) {
        rval = 0;
        SOC_IF_ERROR_RETURN(READ_CI_DDR_TESTr(unit, cinum, &rval));
        soc_reg_field_set(unit, CI_DDR_TESTr, &rval, MODEf, 0);
        soc_reg_field_set(unit, CI_DDR_TESTr, &rval, RAM_TESTf, 0);
        soc_reg_field_set(unit, CI_DDR_TESTr, &rval, RAM_DONEf, 1);     /* write 1 to clear */
        soc_reg_field_set(unit, CI_DDR_TESTr, &rval, RAM_TEST_FAILf, 1);        /* write 1 to clear */
        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_TESTr(unit, cinum, rval));
        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_BURSTr
                            (unit, cinum, 0x1ff0));
        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_STEPr(unit, cinum, 1));
        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_ITERr
                            (unit, cinum, 2));
        startAddr = _get_random() & 0xffffff;
        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_STARTr
                            (unit, cinum, startAddr));

        for(k = 0; k < 8; k++) {
            wdata[k] = _get_random();
        }
        
        for(k = 8; k < 16; k++) {
            wdata[k] = wdata[k-8];
        }
        
        SOC_IF_ERROR_RETURN
            (WRITE_CI_DDR_TEST_DATA0r(unit, cinum, wdata[0]));
        SOC_IF_ERROR_RETURN
            (WRITE_CI_DDR_TEST_DATA1r(unit, cinum, wdata[1]));
        SOC_IF_ERROR_RETURN
            (WRITE_CI_DDR_TEST_DATA2r(unit, cinum, wdata[2]));
        SOC_IF_ERROR_RETURN
            (WRITE_CI_DDR_TEST_DATA3r(unit, cinum, wdata[3]));
        SOC_IF_ERROR_RETURN
            (WRITE_CI_DDR_TEST_DATA4r(unit, cinum, wdata[4]));
        SOC_IF_ERROR_RETURN
            (WRITE_CI_DDR_TEST_DATA5r(unit, cinum, wdata[5]));
        SOC_IF_ERROR_RETURN
            (WRITE_CI_DDR_TEST_DATA6r(unit, cinum, wdata[6]));
        SOC_IF_ERROR_RETURN
            (WRITE_CI_DDR_TEST_DATA7r(unit, cinum, wdata[7]));

        SOC_IF_ERROR_RETURN
            (WRITE_CI_DDR_TEST_ALT_DATA0r(unit, cinum, wdata[8]));
        SOC_IF_ERROR_RETURN
            (WRITE_CI_DDR_TEST_ALT_DATA1r(unit, cinum, wdata[9]));
        SOC_IF_ERROR_RETURN
            (WRITE_CI_DDR_TEST_ALT_DATA2r(unit, cinum, wdata[10]));
        SOC_IF_ERROR_RETURN
            (WRITE_CI_DDR_TEST_ALT_DATA3r(unit, cinum, wdata[11]));
        SOC_IF_ERROR_RETURN
            (WRITE_CI_DDR_TEST_ALT_DATA4r(unit, cinum, wdata[12]));
        SOC_IF_ERROR_RETURN
            (WRITE_CI_DDR_TEST_ALT_DATA5r(unit, cinum, wdata[13]));
        SOC_IF_ERROR_RETURN
            (WRITE_CI_DDR_TEST_ALT_DATA6r(unit, cinum, wdata[14]));
        SOC_IF_ERROR_RETURN
            (WRITE_CI_DDR_TEST_ALT_DATA7r(unit, cinum, wdata[15]));

        rval = 0;
        SOC_IF_ERROR_RETURN(READ_CI_DDR_TESTr(unit, cinum, &rval));
        soc_reg_field_set(unit, CI_DDR_TESTr, &rval, RAM_TESTf, 1);
        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_TESTr(unit, cinum, rval));

        i = 0;
        do {
            sal_usleep(20);
            SOC_IF_ERROR_RETURN(READ_CI_DDR_TESTr(unit, cinum, &rval));
            if (soc_reg_field_get(unit, CI_DDR_TESTr, rval, RAM_DONEf)) {
                if (soc_reg_field_get
                    (unit, CI_DDR_TESTr, rval, RAM_TEST_FAILf)) {
                    result_fail = 1;
                    SOC_IF_ERROR_RETURN(READ_CI_DDR_TEST_FAILED_DATA0r(unit, cinum, rdPtr));
                    SOC_IF_ERROR_RETURN(READ_CI_DDR_TEST_FAILED_DATA1r(unit, cinum, rdPtr+1));
                    SOC_IF_ERROR_RETURN(READ_CI_DDR_TEST_FAILED_DATA2r(unit, cinum, rdPtr+2));
                    SOC_IF_ERROR_RETURN(READ_CI_DDR_TEST_FAILED_DATA3r(unit, cinum, rdPtr+3));
                    SOC_IF_ERROR_RETURN(READ_CI_DDR_TEST_FAILED_DATA4r(unit, cinum, rdPtr+4));
                    SOC_IF_ERROR_RETURN(READ_CI_DDR_TEST_FAILED_DATA5r(unit, cinum, rdPtr+5));
                    SOC_IF_ERROR_RETURN(READ_CI_DDR_TEST_FAILED_DATA6r(unit, cinum, rdPtr+6));
                    SOC_IF_ERROR_RETURN(READ_CI_DDR_TEST_FAILED_DATA7r(unit, cinum, rdPtr+7));
                    for(k = 0; k < 8; k++) {
                        for(m = 31; m >= 16; m--) {
                            efold |= (((wdata[k] ^ rdata[k]) >> m) & 0x00000001) << ((m%8)+8);
                        }
                        for(m = 15; m >= 0; m--) {
                            efold |= (((wdata[k] ^ rdata[k]) >> m) & 0x00000001) << (m%8);
                        }
                    }
                }
                break;
            }
            if (i > 10000) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Functional Self-Test timeout (1) \n")));
                result_fail = 0xFFFF;
                return SOC_E_FAIL;
            }
            i++;
        } while (TRUE);
        j++;
    }

    if (result_fail != 0)  {
        if((flag & 0x1) == 0) {
            if(efold & 0xFF00) {
                efold |= 0xFF00;
            }
            if(efold & 0x00FF) {
                efold |= 0x00FF;
            }
        }
        if((flag & 0x2) != 0) {
            if(wl == 0) {
                READ_DDR40_PHY_WORD_LANE_0_READ_FIFO_STATUSr(unit, ci, &data);
                fifo_status0 = DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, READ_FIFO_STATUS,
                                                STATUS0);
                fifo_status1 = DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, READ_FIFO_STATUS,
                                                STATUS1);
            } else {
                READ_DDR40_PHY_WORD_LANE_1_READ_FIFO_STATUSr(unit, ci, &data);
                fifo_status0 = DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, READ_FIFO_STATUS,
                                                STATUS0);
                fifo_status1 = DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, READ_FIFO_STATUS,
                                                STATUS1);
            }
            efold = 0x0000;
            if(fifo_status0 != 0) {
                efold |= 0x00FF;
            }
            if(fifo_status1 != 0) {
                efold |= 0xFF00;
            }
        /*    LOG_CLI((BSL_META_U(unit,
                                  "FIFO-0: %d\tFIFO-1: %d\n"), fifo_status0, fifo_status1)); */
        }
        if(result_fail != 0xFFFF) {
            result_fail = efold;
        }
    }

    return result_fail;
}

uint32
_test_func_self_test_1_katana2(int unit, int ci, int wl, int loop, uint32 flag)
{
    uint32 i;
    int j, cinum;
    uint32 rval = 0;
    uint32 result_fail = 0;
    uint32 wdata[8];
    uint32 rdata;
    int k;
    uint32 data = 0, fifo_status0, fifo_status1;
    
    uint32 startAddr;
    
    uint32 efold = 0;

    /* _configure_arad_mpr(unit, ci, wl, flag & 0x4); */
    cinum = ci + wl;
    
    j = 0;
    while (j < loop) {
        rval = 0;
        SOC_IF_ERROR_RETURN(READ_BISTCONFIGURATIONSr(unit, cinum, &rval));
        if(flag & 0x4) {
            soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, WRITEWEIGHTf, 0);
            soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, READWEIGHTf, 255);
            soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, PATTERNBITMODEf, 0);
            soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, PRBSMODEf, 0);
            soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, CONSADDR8BANKSf, 1);
            soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, ADDRESSSHIFTMODEf, 0);
            soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, DATASHIFTMODEf, 0);
            soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, DATAADDRMODEf, 0);
        } else {
            soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, WRITEWEIGHTf, 255);
            soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, READWEIGHTf, 255);
            soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, PATTERNBITMODEf, 0);
            soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, PRBSMODEf, 1);
            soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, CONSADDR8BANKSf, 1);
            soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, ADDRESSSHIFTMODEf, 0);
            soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, DATASHIFTMODEf, 0);
            soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, DATAADDRMODEf, 0);
        }

        SOC_IF_ERROR_RETURN(WRITE_BISTCONFIGURATIONSr(unit, cinum, rval));

        SOC_IF_ERROR_RETURN(READ_BISTNUMBEROFACTIONSr(unit, cinum, &rval));
        soc_reg_field_set(unit, BISTNUMBEROFACTIONSr, &rval, BISTNUMACTIONSf, 510);
        SOC_IF_ERROR_RETURN(WRITE_BISTNUMBEROFACTIONSr(unit, cinum, rval));

        /* startAddr = _get_random() & 0xffffff; */
        startAddr = 0x0;
        /* SOC_IF_ERROR_RETURN(READ_BISTSTARTADDRESSr(unit, cinum, &rval)); */
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTSTARTADDRESSr, cinum, 0, &rval));
        soc_reg_field_set(unit, BISTSTARTADDRESSr, &rval, BISTSTARTADDRESSf, startAddr);
        /* SOC_IF_ERROR_RETURN(WRITE_BISTSTARTADDRESSr(unit, cinum, rval)); */
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, BISTSTARTADDRESSr, cinum, 0, rval));

        /* SOC_IF_ERROR_RETURN(READ_BISTENDADDRESSr(unit, cinum, &rval)); */
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTENDADDRESSr, cinum, 0, &rval));
        soc_reg_field_set(unit, BISTENDADDRESSr, &rval, BISTENDADDRESSf, 0xFFFFFF);
        /* SOC_IF_ERROR_RETURN(WRITE_BISTENDADDRESSr(unit, cinum, rval)); */
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, BISTENDADDRESSr, cinum, 0, rval));

        if(flag & 0x4) {
            for(k = 0; k < 8; k+=2) {
                wdata[k] = 0x0;
                wdata[k+1] = 0xFFFFFFFF;
            }
        } else {
            for(k = 0; k < 8; k++) {
                wdata[k] = _get_random();
            }
        }

        SOC_IF_ERROR_RETURN
            (WRITE_BISTPATTERNWORD0r(unit, cinum, wdata[0]));
        SOC_IF_ERROR_RETURN
            (WRITE_BISTPATTERNWORD1r(unit, cinum, wdata[1]));
        SOC_IF_ERROR_RETURN
            (WRITE_BISTPATTERNWORD2r(unit, cinum, wdata[2]));
        SOC_IF_ERROR_RETURN
            (WRITE_BISTPATTERNWORD3r(unit, cinum, wdata[3]));
        SOC_IF_ERROR_RETURN
            (WRITE_BISTPATTERNWORD4r(unit, cinum, wdata[4]));
        SOC_IF_ERROR_RETURN
            (WRITE_BISTPATTERNWORD5r(unit, cinum, wdata[5]));
        SOC_IF_ERROR_RETURN
            (WRITE_BISTPATTERNWORD6r(unit, cinum, wdata[6]));
        SOC_IF_ERROR_RETURN
            (WRITE_BISTPATTERNWORD7r(unit, cinum, wdata[7]));

        SOC_IF_ERROR_RETURN(READ_BISTCONFIGURATIONSr(unit, cinum, &rval));
        soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, INDWRRDADDRMODEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_BISTCONFIGURATIONSr(unit, cinum, rval));

        SOC_IF_ERROR_RETURN(READ_BISTCONFIGURATIONSr(unit, cinum, &rval));
        soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, BISTENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_BISTCONFIGURATIONSr(unit, cinum, rval));

        i = 0;
        do {
            sal_usleep(20);
            SOC_IF_ERROR_RETURN(READ_BISTSTATUSESr(unit, cinum, &rval));
            if (soc_reg_field_get(unit, BISTSTATUSESr, rval, BISTFINISHEDf)) {
                SOC_IF_ERROR_RETURN(READ_BISTCONFIGURATIONSr(unit, cinum, &rval));
                soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, BISTENf, 0);
                SOC_IF_ERROR_RETURN(WRITE_BISTCONFIGURATIONSr(unit, cinum, rval));

                /* SOC_IF_ERROR_RETURN(READ_BISTERROROCCURREDr(unit, cinum, &rval)); */
                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTERROROCCURREDr, cinum, 0, &rval));
                rdata = soc_reg_field_get(unit, BISTERROROCCURREDr, rval, ERROCCURREDf);
/*                if(wl == 0) {
                    efold = rdata & 0xFFFF;
                } else {
                    efold = (rdata >> 16) & 0xFFFF;
                } */
                efold = (rdata & 0xFFFF) | ((rdata >> 16) & 0xFFFF);
                if(efold != 0) {
                    result_fail = 1;
                }
                break;
            }
            if (i > 10000) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Functional Self-Test timeout (1) \n")));
                result_fail = 0xFFFF;
                SOC_IF_ERROR_RETURN(READ_BISTCONFIGURATIONSr(unit, cinum, &rval));
                soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, BISTENf, 0);
                SOC_IF_ERROR_RETURN(WRITE_BISTCONFIGURATIONSr(unit, cinum, rval));
                return SOC_E_FAIL;
            }
            i++;
        } while (TRUE);
        j++;
    }

    if ((result_fail != 0) || (flag & 0x2))  {
        if((flag & 0x1) == 0) { 
            if(efold & 0xFF00) {
                efold |= 0xFF00;
            }
            if(efold & 0x00FF) {
                efold |= 0x00FF;
            }
        }
        if((flag & 0x2) != 0) {
            if(wl == 0) {
                READ_DDR40_PHY_WORD_LANE_0_READ_FIFO_STATUSr(unit, ci, &data);
                fifo_status0 = DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, READ_FIFO_STATUS,
                                                STATUS0);
                fifo_status1 = DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, READ_FIFO_STATUS,
                                                STATUS1);
            } else {
                READ_DDR40_PHY_WORD_LANE_1_READ_FIFO_STATUSr(unit, ci, &data);
                fifo_status0 = DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, READ_FIFO_STATUS,
                                                STATUS0);
                fifo_status1 = DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, READ_FIFO_STATUS,
                                                STATUS1);
            }
            efold = 0x0000;
            if(fifo_status0 != 0) {
                efold |= 0x00FF;
            }
            if(fifo_status1 != 0) {
                efold |= 0xFF00;
            }
            if(efold != 0x0) {
                result_fail = 2;
            }
        }
        if(result_fail != 0xFFFF) {
            result_fail = efold;
        }
    }

    return result_fail;
}

#ifdef BCM_PETRA_SUPPORT
uint32
_test_func_self_test_1_arad(int unit, int ci, int wl, int loop, uint32 flag)
{
    uint32 i;
    int j;
    uint32 rval = 0;
    uint32 result_fail = 0;
    uint32 wdata[8];
    uint32 rdata;
    int k;
    uint32 data, fifo_status0, fifo_status1;
    
    uint32 startAddr;
    
    uint32 efold = 0;

    /* _configure_arad_mpr(unit, ci, wl, flag & 0x4); */
    
    j = 0;
    while (j < loop) {
        rval = 0;
        SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, ci>>1, DRC_BIST_CONFIGURATIONSr, &rval));
        if(flag & 0x4) {
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, WRITE_WEIGHTf, 0);
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, READ_WEIGHTf, 255);
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, PATTERN_BIT_MODEf, 0);
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, PRBS_MODEf, 0);
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, CONS_ADDR_8_BANKSf, 1);
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, ADDRESS_SHIFT_MODEf, 0);
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, DATA_SHIFT_MODEf, 0);
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, DATA_ADDR_MODEf, 0);
        } else {
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, WRITE_WEIGHTf, 255);
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, READ_WEIGHTf, 255);
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, PATTERN_BIT_MODEf, 0);
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, PRBS_MODEf, 1);
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, CONS_ADDR_8_BANKSf, 1);
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, ADDRESS_SHIFT_MODEf, 0);
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, DATA_SHIFT_MODEf, 0);
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, DATA_ADDR_MODEf, 0);
        }

        SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, ci>>1, DRC_BIST_CONFIGURATIONSr, rval));

        SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, ci>>1, DRC_BIST_NUMBER_OF_ACTIONSr, &rval));
        soc_reg_field_set(unit, DRCA_BIST_NUMBER_OF_ACTIONSr, &rval, BIST_NUM_ACTIONSf, 510);
        SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, ci>>1, DRC_BIST_NUMBER_OF_ACTIONSr, rval));

        /* startAddr = _get_random() & 0xffffff; */
        startAddr = 0x0;
        SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, ci>>1, DRC_BIST_START_ADDRESSr, &rval));
        soc_reg_field_set(unit, DRCA_BIST_START_ADDRESSr, &rval, BIST_START_ADDRESSf, startAddr);
        SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, ci>>1, DRC_BIST_START_ADDRESSr, rval));

        SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, ci>>1, DRC_BIST_END_ADDRESSr, &rval));
        soc_reg_field_set(unit, DRCA_BIST_END_ADDRESSr, &rval, BIST_END_ADDRESSf, 0xFFFFFF);
        SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, ci>>1, DRC_BIST_END_ADDRESSr, rval));

        if(flag & 0x4) {
            for(k = 0; k < 8; k+=2) {
                wdata[k] = 0x0;
                wdata[k+1] = 0xFFFFFFFF;
            }
        } else {
            for(k = 0; k < 8; k++) {
                wdata[k] = _get_random();
            }
        }
        
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, ci>>1, DRC_BIST_PATTERN_WORD_0r, wdata[0]));
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, ci>>1, DRC_BIST_PATTERN_WORD_1r, wdata[1]));
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, ci>>1, DRC_BIST_PATTERN_WORD_2r, wdata[2]));
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, ci>>1, DRC_BIST_PATTERN_WORD_3r, wdata[3]));
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, ci>>1, DRC_BIST_PATTERN_WORD_4r, wdata[4]));
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, ci>>1, DRC_BIST_PATTERN_WORD_5r, wdata[5]));
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, ci>>1, DRC_BIST_PATTERN_WORD_6r, wdata[6]));
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, ci>>1, DRC_BIST_PATTERN_WORD_7r, wdata[7]));

        SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, ci>>1, DRC_BIST_CONFIGURATIONSr, &rval));
        soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, IND_WR_RD_ADDR_MODEf, 1);
        SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, ci>>1, DRC_BIST_CONFIGURATIONSr, rval));

        SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, ci>>1, DRC_BIST_CONFIGURATIONSr, &rval));
        soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, BIST_ENf, 1);
        SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, ci>>1, DRC_BIST_CONFIGURATIONSr, rval));

        i = 0;
        do {
            sal_usleep(20);
            SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, ci>>1, DRC_BIST_STATUSESr, &rval));
            if (soc_reg_field_get(unit, DRCA_BIST_STATUSESr, rval, BIST_FINISHEDf)) {
                SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, ci>>1, DRC_BIST_CONFIGURATIONSr, &rval));
                soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, BIST_ENf, 0);
                SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, ci>>1, DRC_BIST_CONFIGURATIONSr, rval));

                SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, ci>>1, DRC_BIST_ERROR_OCCURREDr, &rval));
                rdata = soc_reg_field_get(unit, DRCA_BIST_ERROR_OCCURREDr, rval, ERR_OCCURREDf);
                if(wl == 0) {
                    efold = rdata & 0xFFFF;
                } else {
                    efold = (rdata >> 16) & 0xFFFF;
                }
                if(efold != 0) {
                    result_fail = 1;
                }
                break;
            }
            if (i > 10000) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Functional Self-Test timeout (1) \n")));
                result_fail = 0xFFFF;
                SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, ci>>1, DRC_BIST_CONFIGURATIONSr, &rval));
                soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, BIST_ENf, 0);
                SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, ci>>1, DRC_BIST_CONFIGURATIONSr, rval));
                return SOC_E_FAIL;
            }
            i++;
        } while (TRUE);
        j++;
    }

    if ((result_fail != 0) || (flag & 0x2))  {
        if((flag & 0x1) == 0) { 
            if(efold & 0xFF00) {
                efold |= 0xFF00;
            }
            if(efold & 0x00FF) {
                efold |= 0x00FF;
            }
        }
        if((flag & 0x2) != 0) {
            if(wl == 0) {
                READ_DDR40_PHY_WORD_LANE_0_READ_FIFO_STATUSr(unit, ci, &data);
                fifo_status0 = DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, READ_FIFO_STATUS,
                                                STATUS0);
                fifo_status1 = DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, READ_FIFO_STATUS,
                                                STATUS1);
            } else {
                READ_DDR40_PHY_WORD_LANE_1_READ_FIFO_STATUSr(unit, ci, &data);
                fifo_status0 = DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, READ_FIFO_STATUS,
                                                STATUS0);
                fifo_status1 = DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, READ_FIFO_STATUS,
                                                STATUS1);
            }
            efold = 0x0000;
            if(fifo_status0 != 0) {
                efold |= 0x00FF;
            }
            if(fifo_status1 != 0) {
                efold |= 0xFF00;
            }
            if(efold != 0x0) {
                result_fail = 2;
            }
        /*    LOG_CLI((BSL_META_U(unit,
                                  "FIFO-0: %d\tFIFO-1: %d\n"), fifo_status0, fifo_status1)); */
        }
        if(result_fail != 0xFFFF) {
            result_fail = efold;
        }
    }

    return result_fail;
}
#endif

#ifdef BCM_HURRICANE2_SUPPORT
STATIC uint32
_test_func_self_test_1_hurricane2(int unit, int ci, int wl, int loop, uint32 flag)
{
    uint32 i;
    int j;
    uint32 rval = 0;
    uint32 result_fail = 0;
    uint32 wdata[8];
    uint32 rdata;
    int k;
    uint32 data, fifo_status0, fifo_status1;
    
    uint32 startAddr;
    
    uint32 efold = 0;

    j = 0;
    while (j < loop) {
        rval = 0;
        SOC_IF_ERROR_RETURN(READ_DDR_BISTCONFIGURATIONSr(unit, &rval));
        if(flag & 0x4) {
            soc_reg_field_set(
                unit, DDR_BISTCONFIGURATIONSr, &rval, WRITEWEIGHTf, 0);
            soc_reg_field_set(
                unit, DDR_BISTCONFIGURATIONSr, &rval, READWEIGHTf, 255);
            soc_reg_field_set(
                unit, DDR_BISTCONFIGURATIONSr, &rval, PATTERNBITMODEf, 0);
            soc_reg_field_set(
                unit, DDR_BISTCONFIGURATIONSr, &rval, PRBSMODEf, 0);
            soc_reg_field_set(
                unit, DDR_BISTCONFIGURATIONSr, &rval, CONSADDR8BANKSf, 1);
            soc_reg_field_set(
                unit, DDR_BISTCONFIGURATIONSr, &rval, ADDRESSSHIFTMODEf, 0);
            soc_reg_field_set(
                unit, DDR_BISTCONFIGURATIONSr, &rval, DATASHIFTMODEf, 0);
            soc_reg_field_set(
                unit, DDR_BISTCONFIGURATIONSr, &rval, DATAADDRMODEf, 0);
        } else {
            soc_reg_field_set(
                unit, DDR_BISTCONFIGURATIONSr, &rval, WRITEWEIGHTf, 255);
            soc_reg_field_set(
                unit, DDR_BISTCONFIGURATIONSr, &rval, READWEIGHTf, 255);
            soc_reg_field_set(
                unit, DDR_BISTCONFIGURATIONSr, &rval, PATTERNBITMODEf, 0);
            soc_reg_field_set(
                unit, DDR_BISTCONFIGURATIONSr, &rval, PRBSMODEf, 1);
            soc_reg_field_set(
                unit, DDR_BISTCONFIGURATIONSr, &rval, CONSADDR8BANKSf, 1);
            soc_reg_field_set(
                unit, DDR_BISTCONFIGURATIONSr, &rval, ADDRESSSHIFTMODEf, 0);
            soc_reg_field_set(
                unit, DDR_BISTCONFIGURATIONSr, &rval, DATASHIFTMODEf, 0);
            soc_reg_field_set(
                unit, DDR_BISTCONFIGURATIONSr, &rval, DATAADDRMODEf, 0);
        }
        SOC_IF_ERROR_RETURN(WRITE_DDR_BISTCONFIGURATIONSr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_DDR_BISTNUMBEROFACTIONSr(unit, &rval));
        soc_reg_field_set(
            unit, DDR_BISTNUMBEROFACTIONSr, &rval, BISTNUMACTIONSf, 510);
        SOC_IF_ERROR_RETURN(WRITE_DDR_BISTNUMBEROFACTIONSr(unit, rval));

        /* startAddr = _get_random() & 0xffffff; */
        startAddr = 0x0;
        SOC_IF_ERROR_RETURN(READ_DDR_BISTSTARTADDRESSr(unit, &rval));
        soc_reg_field_set(
            unit, DDR_BISTSTARTADDRESSr, &rval, BISTSTARTADDRESSf, startAddr);
        SOC_IF_ERROR_RETURN(WRITE_DDR_BISTSTARTADDRESSr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_DDR_BISTENDADDRESSr(unit, &rval));
        soc_reg_field_set(
            unit, DDR_BISTENDADDRESSr, &rval, BISTENDADDRESSf, 0xFFFFFF);
        SOC_IF_ERROR_RETURN(WRITE_DDR_BISTENDADDRESSr(unit, rval));

        if(flag & 0x4) {
            for(k = 0; k < 8; k+=2) {
                wdata[k] = 0x0;
                wdata[k+1] = 0xFFFFFFFF;
            }
        } else {
            for(k = 0; k < 8; k++) {
                wdata[k] = _get_random();
            }
        }
        
        SOC_IF_ERROR_RETURN(WRITE_DDR_BISTPATTERNWORD0r(unit, wdata[0]));
        SOC_IF_ERROR_RETURN(WRITE_DDR_BISTPATTERNWORD1r(unit, wdata[1]));
        SOC_IF_ERROR_RETURN(WRITE_DDR_BISTPATTERNWORD2r(unit, wdata[2]));
        SOC_IF_ERROR_RETURN(WRITE_DDR_BISTPATTERNWORD3r(unit, wdata[3]));
        SOC_IF_ERROR_RETURN(WRITE_DDR_BISTPATTERNWORD4r(unit, wdata[4]));
        SOC_IF_ERROR_RETURN(WRITE_DDR_BISTPATTERNWORD5r(unit, wdata[5]));
        SOC_IF_ERROR_RETURN(WRITE_DDR_BISTPATTERNWORD6r(unit, wdata[6]));
        SOC_IF_ERROR_RETURN(WRITE_DDR_BISTPATTERNWORD7r(unit, wdata[7]));

        SOC_IF_ERROR_RETURN(READ_DDR_BISTCONFIGURATIONSr(unit, &rval));
        soc_reg_field_set(
            unit, DDR_BISTCONFIGURATIONSr, &rval, INDWRRDADDRMODEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_DDR_BISTCONFIGURATIONSr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_DDR_BISTCONFIGURATIONSr(unit, &rval));
        soc_reg_field_set(unit, DDR_BISTCONFIGURATIONSr, &rval, BISTENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_DDR_BISTCONFIGURATIONSr(unit, rval));

        i = 0;
        do {
            sal_usleep(20);
            SOC_IF_ERROR_RETURN(READ_DDR_BISTSTATUSESr(unit, &rval));
            if (soc_reg_field_get(
                    unit, DDR_BISTSTATUSESr, rval, BISTFINISHEDf)) {
                SOC_IF_ERROR_RETURN(READ_DDR_BISTCONFIGURATIONSr(unit, &rval));
                soc_reg_field_set(
                    unit, DDR_BISTCONFIGURATIONSr, &rval, BISTENf, 0);
                SOC_IF_ERROR_RETURN(WRITE_DDR_BISTCONFIGURATIONSr(unit, rval));

                SOC_IF_ERROR_RETURN(READ_DDR_BISTERROROCCURREDr(unit, &rval));
                rdata = soc_reg_field_get(
                            unit, DDR_BISTERROROCCURREDr, rval, ERROCCURREDf);
                if(wl == 0) {
                    efold = rdata & 0xFFFF;
                } else {
                    efold = (rdata >> 16) & 0xFFFF;
                }
                if(efold != 0) {
                    result_fail = 1;
                }
                break;
            }
            if (i > 10000) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Functional Self-Test timeout (1) \n")));
                result_fail = 0xFFFF;
                SOC_IF_ERROR_RETURN(READ_DDR_BISTCONFIGURATIONSr(unit, &rval));
                soc_reg_field_set(
                    unit, DDR_BISTCONFIGURATIONSr, &rval, BISTENf, 0);
                SOC_IF_ERROR_RETURN(WRITE_DDR_BISTCONFIGURATIONSr(unit, rval));
                return SOC_E_FAIL;
            }
            i++;
        } while (TRUE);
        j++;
    }

    if ((result_fail != 0) || (flag & 0x2))  {
        if((flag & 0x1) == 0) { 
            if(efold & 0xFF00) {
                efold |= 0xFF00;
            }
            if(efold & 0x00FF) {
                efold |= 0x00FF;
            }
        }
        if((flag & 0x2) != 0) {
            if(wl == 0) {
                READ_DDR40_PHY_WORD_LANE_0_READ_FIFO_STATUSr(unit, ci, &data);
                fifo_status0 = DDR40_GET_FIELD(
                    data, DDR40_PHY_WORD_LANE_0, READ_FIFO_STATUS,STATUS0);
                fifo_status1 = DDR40_GET_FIELD(
                    data, DDR40_PHY_WORD_LANE_0, READ_FIFO_STATUS, STATUS1);
            } else {
                READ_DDR40_PHY_WORD_LANE_1_READ_FIFO_STATUSr(unit, ci, &data);
                fifo_status0 = DDR40_GET_FIELD(
                    data, DDR40_PHY_WORD_LANE_1, READ_FIFO_STATUS, STATUS0);
                fifo_status1 = DDR40_GET_FIELD(
                    data, DDR40_PHY_WORD_LANE_1, READ_FIFO_STATUS, STATUS1);
            }
            efold = 0x0000;
            if(fifo_status0 != 0) {
                efold |= 0x00FF;
            }
            if(fifo_status1 != 0) {
                efold |= 0xFF00;
            }
            if(efold != 0x0) {
                result_fail = 2;
            }
        /*    LOG_CLI((BSL_META_U(unit,
                                  "FIFO-0: %d\tFIFO-1: %d\n"), fifo_status0, fifo_status1)); */
        }
        if(result_fail != 0xFFFF) {
            result_fail = efold;
        }
    }

    return result_fail;
}
#endif /* BCM_HURRICANE2_SUPPORT */

/* Flag Settings:
 * flag[0]: 0 - Mask errors per byte
 *          1 - Do not mask errors
 * flag[1]: 0 - Compare data
 *          1 - Check FIFO status
 * flag[2]: 0 - Disable MPR
 *          1 - Enable MPR
 */

uint32
_test_func_self_test_1(int unit, int ci, int wl, int loop, uint32 flag)
{
    if(SOC_IS_KATANA(unit)) {
        return _test_func_self_test_1_katana(unit, ci, wl, loop, flag);
    } else if(SOC_IS_KATANA2(unit)) {
        return _test_func_self_test_1_katana2(unit, ci, wl, loop, flag);;
#ifdef BCM_PETRA_SUPPORT
    } else if(SOC_IS_ARAD(unit)) {
        return _test_func_self_test_1_arad(unit, ci, wl, loop, flag);
#endif
#ifdef BCM_HURRICANE2_SUPPORT
    } else if(SOC_IS_HURRICANE2(unit)) {
        return _test_func_self_test_1_hurricane2(unit, ci, wl, loop, flag);
#endif
    } else {
        return SOC_E_INTERNAL;     /* Unsupported configuration */
    }
}

uint32
_test_func_self_test_2_katana(int unit, int ci, int wl, int loop)
{
    int i, j, lane, cinum;
    uint32 rval = 0;
    uint32 result_fail = 0;
    uint32 max_num_lane;

    max_num_lane = 2;
  
    /* Alternate data test */
    cinum = ci + wl;
    for (lane = 0; lane < max_num_lane; lane++) {
        if(!_check_dram(ci+lane)) {
            continue;
        }
        j = 0;
        while (j < loop) {
            rval = 0;
            SOC_IF_ERROR_RETURN(READ_CI_DDR_TESTr(unit, (ci + lane), &rval));
            soc_reg_field_set(unit, CI_DDR_TESTr, &rval, MODEf, 0);
            soc_reg_field_set(unit, CI_DDR_TESTr, &rval, RAM_TESTf, 0);
            soc_reg_field_set(unit, CI_DDR_TESTr, &rval, RAM_DONEf, 1); /* write 1 to clear */
            soc_reg_field_set(unit, CI_DDR_TESTr, &rval, RAM_TEST_FAILf, 1);    /* write 1 to clear */
            SOC_IF_ERROR_RETURN(WRITE_CI_DDR_TESTr(unit, (ci + lane), rval));
            SOC_IF_ERROR_RETURN(READ_CI_DDR_TESTr(unit, (ci + lane), &rval));

            SOC_IF_ERROR_RETURN(WRITE_CI_DDR_BURSTr
                                (unit, cinum, 0x1FF0));
            SOC_IF_ERROR_RETURN(WRITE_CI_DDR_STEPr(unit, (ci + lane), 1));
            SOC_IF_ERROR_RETURN(WRITE_CI_DDR_ITERr
                                (unit, cinum, 2));

            SOC_IF_ERROR_RETURN(WRITE_CI_DDR_STARTr(unit, cinum, (_get_random() & 0xffffff)));

            SOC_IF_ERROR_RETURN
                (WRITE_CI_DDR_TEST_DATA0r(unit, cinum, _get_random()));
            SOC_IF_ERROR_RETURN
                (WRITE_CI_DDR_TEST_DATA1r(unit, cinum, _get_random()));
            SOC_IF_ERROR_RETURN
                (WRITE_CI_DDR_TEST_DATA2r(unit, cinum, _get_random()));
            SOC_IF_ERROR_RETURN
                (WRITE_CI_DDR_TEST_DATA3r(unit, cinum, _get_random()));
            SOC_IF_ERROR_RETURN
                (WRITE_CI_DDR_TEST_DATA4r(unit, cinum, _get_random()));
            SOC_IF_ERROR_RETURN
                (WRITE_CI_DDR_TEST_DATA5r(unit, cinum, _get_random()));
            SOC_IF_ERROR_RETURN
                (WRITE_CI_DDR_TEST_DATA6r(unit, cinum, _get_random()));
            SOC_IF_ERROR_RETURN
                (WRITE_CI_DDR_TEST_DATA7r(unit, cinum, _get_random()));

            SOC_IF_ERROR_RETURN
                (WRITE_CI_DDR_TEST_ALT_DATA0r(unit, cinum, _get_random()));
            SOC_IF_ERROR_RETURN
                (WRITE_CI_DDR_TEST_ALT_DATA1r(unit, cinum, _get_random()));
            SOC_IF_ERROR_RETURN
                (WRITE_CI_DDR_TEST_ALT_DATA2r(unit, cinum, _get_random()));
            SOC_IF_ERROR_RETURN
                (WRITE_CI_DDR_TEST_ALT_DATA3r(unit, cinum, _get_random()));
            SOC_IF_ERROR_RETURN
                (WRITE_CI_DDR_TEST_ALT_DATA4r(unit, cinum, _get_random()));
            SOC_IF_ERROR_RETURN
                (WRITE_CI_DDR_TEST_ALT_DATA5r(unit, cinum, _get_random()));
            SOC_IF_ERROR_RETURN
                (WRITE_CI_DDR_TEST_ALT_DATA6r(unit, cinum, _get_random()));
            SOC_IF_ERROR_RETURN
                (WRITE_CI_DDR_TEST_ALT_DATA7r(unit, cinum, _get_random()));

            rval = 0;
            soc_reg_field_set(unit, CI_DDR_TESTr, &rval, RAM_TESTf, 1);
            SOC_IF_ERROR_RETURN(WRITE_CI_DDR_TESTr(unit, (ci + lane), rval));

            i = 0;
            do {
                sal_usleep(20);
                SOC_IF_ERROR_RETURN(READ_CI_DDR_TESTr
                                    (unit, (ci + lane), &rval));
                if (soc_reg_field_get(unit, CI_DDR_TESTr, rval, RAM_DONEf)) {
                    if (soc_reg_field_get
                        (unit, CI_DDR_TESTr, rval, RAM_TEST_FAILf)) {
                        result_fail = result_fail + 1;
                    }
                    break;
                }
                if (i > 100000) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "Functional Self-Test timeout (2) \n")));
                    return SOC_E_FAIL;
                }
                i++;
            } while (TRUE);
            j++;
        }
    }

    if (result_fail > 0) {
        result_fail = 0xffff;
    }

    return result_fail;
}

uint32
_test_func_self_test_2_katana2(int unit, int ci, int wl, int loop)
{
    int i, j, cinum;
    uint32 rval = 0;
    uint32 result_fail = 0;
    uint32 result_both = 0;
    uint32 wdata[8];
    int k, dram;

    uint32 startAddr;
  
    /* Alternate data test */
    for(dram = 0; dram < 2; dram++) {
        cinum = ci + dram;
        
        if(!_check_dram(cinum)) {
            continue;
        }

        j = 0;
        while (j < loop) {
            rval = 0;
            SOC_IF_ERROR_RETURN(READ_BISTCONFIGURATIONSr(unit, cinum, &rval));
            soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, WRITEWEIGHTf, 255);
            soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, READWEIGHTf, 255);
            soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, PATTERNBITMODEf, 0);
            soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, PRBSMODEf, 1);
            soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, CONSADDR8BANKSf, 1);
            soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, ADDRESSSHIFTMODEf, 0);
            soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, DATASHIFTMODEf, 0);
            soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, DATAADDRMODEf, 0);
            SOC_IF_ERROR_RETURN(WRITE_BISTCONFIGURATIONSr(unit, cinum, rval));

            SOC_IF_ERROR_RETURN(READ_BISTNUMBEROFACTIONSr(unit, cinum, &rval));
            soc_reg_field_set(unit, BISTNUMBEROFACTIONSr, &rval, BISTNUMACTIONSf, 510);
            SOC_IF_ERROR_RETURN(WRITE_BISTNUMBEROFACTIONSr(unit, cinum, rval));

            /* startAddr = _get_random() & 0xffffff; */
            startAddr =0x0;
            /* SOC_IF_ERROR_RETURN(READ_BISTSTARTADDRESSr(unit, cinum, &rval)); */
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTSTARTADDRESSr, cinum, 0, &rval));
            soc_reg_field_set(unit, BISTSTARTADDRESSr, &rval, BISTSTARTADDRESSf, startAddr);
            /* SOC_IF_ERROR_RETURN(WRITE_BISTSTARTADDRESSr(unit, cinum, rval)); */
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, BISTSTARTADDRESSr, cinum, 0, rval));

            /* SOC_IF_ERROR_RETURN(READ_BISTENDADDRESSr(unit, cinum, &rval)); */
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTENDADDRESSr, cinum, 0, &rval));
            soc_reg_field_set(unit, BISTENDADDRESSr, &rval, BISTENDADDRESSf, 0xFFFFFF);
            /* SOC_IF_ERROR_RETURN(WRITE_BISTENDADDRESSr(unit, cinum, rval)); */
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, BISTENDADDRESSr, cinum, 0, rval));

            for(k = 0; k < 8; k++) {
                wdata[k] = _get_random();
            }

            SOC_IF_ERROR_RETURN
                (WRITE_BISTPATTERNWORD0r(unit, cinum, wdata[0]));
            SOC_IF_ERROR_RETURN
                (WRITE_BISTPATTERNWORD1r(unit, cinum, wdata[1]));
            SOC_IF_ERROR_RETURN
                (WRITE_BISTPATTERNWORD2r(unit, cinum, wdata[2]));
            SOC_IF_ERROR_RETURN
                (WRITE_BISTPATTERNWORD3r(unit, cinum, wdata[3]));
            SOC_IF_ERROR_RETURN
                (WRITE_BISTPATTERNWORD4r(unit, cinum, wdata[4]));
            SOC_IF_ERROR_RETURN
                (WRITE_BISTPATTERNWORD5r(unit, cinum, wdata[5]));
            SOC_IF_ERROR_RETURN
                (WRITE_BISTPATTERNWORD6r(unit, cinum, wdata[6]));
            SOC_IF_ERROR_RETURN
                (WRITE_BISTPATTERNWORD7r(unit, cinum, wdata[7]));

            SOC_IF_ERROR_RETURN(READ_BISTCONFIGURATIONSr(unit, cinum, &rval));
            soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, INDWRRDADDRMODEf, 1);
            SOC_IF_ERROR_RETURN(WRITE_BISTCONFIGURATIONSr(unit, cinum, rval));

            SOC_IF_ERROR_RETURN(READ_BISTCONFIGURATIONSr(unit, cinum, &rval));
            soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, BISTENf, 1);
            SOC_IF_ERROR_RETURN(WRITE_BISTCONFIGURATIONSr(unit, cinum, rval));

            i = 0;
            do {
                sal_usleep(20);
                SOC_IF_ERROR_RETURN(READ_BISTSTATUSESr(unit, cinum, &rval));
                if (soc_reg_field_get(unit, BISTSTATUSESr, rval, BISTFINISHEDf)) {
                    SOC_IF_ERROR_RETURN(READ_BISTCONFIGURATIONSr(unit, cinum, &rval));
                    soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, BISTENf, 0);
                    SOC_IF_ERROR_RETURN(WRITE_BISTCONFIGURATIONSr(unit, cinum, rval));

                    /* SOC_IF_ERROR_RETURN(READ_BISTERROROCCURREDr(unit, cinum, &rval)); */
                    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTERROROCCURREDr, cinum, 0, &rval));
                    result_fail = soc_reg_field_get(unit, BISTERROROCCURREDr, rval, ERROCCURREDf);
                    result_fail = (result_fail & 0xFFFF) | ((result_fail >> 16) & 0xFFFF);
                    break;
                }
                if (i > 10000) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "Functional Self-Test timeout (1) \n")));
                    result_fail = 0xFFFF;
                    SOC_IF_ERROR_RETURN(READ_BISTCONFIGURATIONSr(unit, cinum, &rval));
                    soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, BISTENf, 0);
                    SOC_IF_ERROR_RETURN(WRITE_BISTCONFIGURATIONSr(unit, cinum, rval));
                    return SOC_E_FAIL;
                }
                i++;
            } while (TRUE);
            j++;
        }

        if (result_fail > 0) {
            result_fail = 0xffff;
        }
        
        result_both |= result_fail;
    }

    return result_both;
}

#ifdef BCM_PETRA_SUPPORT
uint32
_test_func_self_test_2_arad(int unit, int ci, int wl, int loop)
{
    int i, j;
    uint32 rval = 0;
    uint32 result_fail = 0;
    uint32 wdata[8];
    int k;

    uint32 startAddr;
  
    /* Alternate data test */
    j = 0;
    while (j < loop) {
        rval = 0;
        SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, (ci>>1), DRC_BIST_CONFIGURATIONSr, &rval));
        soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, WRITE_WEIGHTf, 255);
        soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, READ_WEIGHTf, 255);
        soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, PATTERN_BIT_MODEf, 0);
        soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, PRBS_MODEf, 1);
        soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, CONS_ADDR_8_BANKSf, 1);
        soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, ADDRESS_SHIFT_MODEf, 0);
        soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, DATA_SHIFT_MODEf, 0);
        soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, DATA_ADDR_MODEf, 0);
        SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, ci>>1, DRC_BIST_CONFIGURATIONSr, rval));

        SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, ci>>1, DRC_BIST_NUMBER_OF_ACTIONSr, &rval));
        soc_reg_field_set(unit, DRCA_BIST_NUMBER_OF_ACTIONSr, &rval, BIST_NUM_ACTIONSf, 510);
        SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, ci>>1, DRC_BIST_NUMBER_OF_ACTIONSr, rval));

        /* startAddr = _get_random() & 0xffffff; */
        startAddr =0x0;
        SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, ci>>1, DRC_BIST_START_ADDRESSr, &rval));
        soc_reg_field_set(unit, DRCA_BIST_START_ADDRESSr, &rval, BIST_START_ADDRESSf, startAddr);
        SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, ci>>1, DRC_BIST_START_ADDRESSr, rval));

        SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, ci>>1, DRC_BIST_END_ADDRESSr, &rval));
        soc_reg_field_set(unit, DRCA_BIST_END_ADDRESSr, &rval, BIST_END_ADDRESSf, 0xFFFFFF);
        SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, ci>>1, DRC_BIST_END_ADDRESSr, rval));

        for(k = 0; k < 8; k++) {
            wdata[k] = _get_random();
        }
        
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, ci>>1, DRC_BIST_PATTERN_WORD_0r, wdata[0]));
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, ci>>1, DRC_BIST_PATTERN_WORD_1r, wdata[1]));
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, ci>>1, DRC_BIST_PATTERN_WORD_2r, wdata[2]));
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, ci>>1, DRC_BIST_PATTERN_WORD_3r, wdata[3]));
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, ci>>1, DRC_BIST_PATTERN_WORD_4r, wdata[4]));
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, ci>>1, DRC_BIST_PATTERN_WORD_5r, wdata[5]));
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, ci>>1, DRC_BIST_PATTERN_WORD_6r, wdata[6]));
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, ci>>1, DRC_BIST_PATTERN_WORD_7r, wdata[7]));

        SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, ci>>1, DRC_BIST_CONFIGURATIONSr, &rval));
        soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, IND_WR_RD_ADDR_MODEf, 1);
        SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, ci>>1, DRC_BIST_CONFIGURATIONSr, rval));

        SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, ci>>1, DRC_BIST_CONFIGURATIONSr, &rval));
        soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, BIST_ENf, 1);
        SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, ci>>1, DRC_BIST_CONFIGURATIONSr, rval));

        i = 0;
        do {
            sal_usleep(20);
            SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, ci>>1, DRC_BIST_STATUSESr, &rval));
            if (soc_reg_field_get(unit, DRCA_BIST_STATUSESr, rval, BIST_FINISHEDf)) {
                SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, ci>>1, DRC_BIST_CONFIGURATIONSr, &rval));
                soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, BIST_ENf, 0);
                SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, ci>>1, DRC_BIST_CONFIGURATIONSr, rval));

                SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, ci>>1, DRC_BIST_ERROR_OCCURREDr, &rval));
                result_fail = soc_reg_field_get(unit, DRCA_BIST_ERROR_OCCURREDr, rval, ERR_OCCURREDf);
                break;
            }
            if (i > 10000) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Functional Self-Test timeout (1) \n")));
                result_fail = 0xFFFF;
                SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, ci>>1, DRC_BIST_CONFIGURATIONSr, &rval));
                soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, BIST_ENf, 0);
                SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, ci>>1, DRC_BIST_CONFIGURATIONSr, rval));
                return SOC_E_FAIL;
            }
            i++;
        } while (TRUE);
        j++;
    }

    if (result_fail > 0) {
        result_fail = 0xffff;
    }

    return result_fail;
}
#endif

#ifdef BCM_HURRICANE2_SUPPORT
STATIC uint32
_test_func_self_test_2_hurricane2(int unit, int ci, int wl, int loop)
{
    int i, j;
    uint32 rval = 0;
    uint32 result_fail = 0;
    uint32 wdata[8];
    int k;

    uint32 startAddr;
  
    /* Alternate data test */
    j = 0;
    while (j < loop) {
        rval = 0;
        
        SOC_IF_ERROR_RETURN(READ_DDR_BISTCONFIGURATIONSr(unit, &rval));
        soc_reg_field_set(
            unit, DDR_BISTCONFIGURATIONSr, &rval, WRITEWEIGHTf, 255);
        soc_reg_field_set(
            unit, DDR_BISTCONFIGURATIONSr, &rval, READWEIGHTf, 255);
        soc_reg_field_set(
            unit, DDR_BISTCONFIGURATIONSr, &rval, PATTERNBITMODEf, 0);
        soc_reg_field_set(
            unit, DDR_BISTCONFIGURATIONSr, &rval, PRBSMODEf, 1);
        soc_reg_field_set(
            unit, DDR_BISTCONFIGURATIONSr, &rval, CONSADDR8BANKSf, 1);
        soc_reg_field_set(
            unit, DDR_BISTCONFIGURATIONSr, &rval, ADDRESSSHIFTMODEf, 0);
        soc_reg_field_set(
            unit, DDR_BISTCONFIGURATIONSr, &rval, DATASHIFTMODEf, 0);
        soc_reg_field_set(
            unit, DDR_BISTCONFIGURATIONSr, &rval, DATAADDRMODEf, 0);
        SOC_IF_ERROR_RETURN(WRITE_DDR_BISTCONFIGURATIONSr(unit, rval));
        
        SOC_IF_ERROR_RETURN(READ_DDR_BISTNUMBEROFACTIONSr(unit, &rval));
        soc_reg_field_set(
            unit, DDR_BISTNUMBEROFACTIONSr, &rval, BISTNUMACTIONSf, 510);
        SOC_IF_ERROR_RETURN(WRITE_DDR_BISTNUMBEROFACTIONSr(unit, rval));
        
        /* startAddr = _get_random() & 0xffffff; */
        startAddr = 0x0;
        SOC_IF_ERROR_RETURN(READ_DDR_BISTSTARTADDRESSr(unit, &rval));
        soc_reg_field_set(
            unit, DDR_BISTSTARTADDRESSr, &rval, BISTSTARTADDRESSf, startAddr);
        SOC_IF_ERROR_RETURN(WRITE_DDR_BISTSTARTADDRESSr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_DDR_BISTENDADDRESSr(unit, &rval));
        soc_reg_field_set(
            unit, DDR_BISTENDADDRESSr, &rval, BISTENDADDRESSf, 0xFFFFFF);
        SOC_IF_ERROR_RETURN(WRITE_DDR_BISTENDADDRESSr(unit, rval));
        

        for(k = 0; k < 8; k++) {
            wdata[k] = _get_random();
        }
        
        SOC_IF_ERROR_RETURN(WRITE_DDR_BISTPATTERNWORD0r(unit, wdata[0]));
        SOC_IF_ERROR_RETURN(WRITE_DDR_BISTPATTERNWORD1r(unit, wdata[1]));
        SOC_IF_ERROR_RETURN(WRITE_DDR_BISTPATTERNWORD2r(unit, wdata[2]));
        SOC_IF_ERROR_RETURN(WRITE_DDR_BISTPATTERNWORD3r(unit, wdata[3]));
        SOC_IF_ERROR_RETURN(WRITE_DDR_BISTPATTERNWORD4r(unit, wdata[4]));
        SOC_IF_ERROR_RETURN(WRITE_DDR_BISTPATTERNWORD5r(unit, wdata[5]));
        SOC_IF_ERROR_RETURN(WRITE_DDR_BISTPATTERNWORD6r(unit, wdata[6]));
        SOC_IF_ERROR_RETURN(WRITE_DDR_BISTPATTERNWORD7r(unit, wdata[7]));
        
        SOC_IF_ERROR_RETURN(READ_DDR_BISTCONFIGURATIONSr(unit, &rval));
        soc_reg_field_set(
            unit, DDR_BISTCONFIGURATIONSr, &rval, INDWRRDADDRMODEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_DDR_BISTCONFIGURATIONSr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_DDR_BISTCONFIGURATIONSr(unit, &rval));
        soc_reg_field_set(unit, DDR_BISTCONFIGURATIONSr, &rval, BISTENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_DDR_BISTCONFIGURATIONSr(unit, rval));
        
        i = 0;
        
        do {
            sal_usleep(20);
            SOC_IF_ERROR_RETURN(READ_DDR_BISTSTATUSESr(unit, &rval));
            if (soc_reg_field_get(
                    unit, DDR_BISTSTATUSESr, rval, BISTFINISHEDf)) {
                SOC_IF_ERROR_RETURN(READ_DDR_BISTCONFIGURATIONSr(unit, &rval));
                soc_reg_field_set(
                    unit, DDR_BISTCONFIGURATIONSr, &rval, BISTENf, 0);
                SOC_IF_ERROR_RETURN(WRITE_DDR_BISTCONFIGURATIONSr(unit, rval));

                SOC_IF_ERROR_RETURN(READ_DDR_BISTERROROCCURREDr(unit, &rval));
                result_fail = soc_reg_field_get(
                            unit, DDR_BISTERROROCCURREDr, rval, ERROCCURREDf);
                break;
            }
            if (i > 10000) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Functional Self-Test timeout (1) \n")));
                result_fail = 0xFFFF;
                SOC_IF_ERROR_RETURN(READ_DDR_BISTCONFIGURATIONSr(unit, &rval));
                soc_reg_field_set(
                    unit, DDR_BISTCONFIGURATIONSr, &rval, BISTENf, 0);
                SOC_IF_ERROR_RETURN(WRITE_DDR_BISTCONFIGURATIONSr(unit, rval));
                return SOC_E_FAIL;
            }
            i++;
        } while (TRUE);
        j++;
    }

    if (result_fail > 0) {
        result_fail = 0xffff;
    }

    return result_fail;
}
#endif /* BCM_HURRICANE2_SUPPORT */

uint32
_test_func_self_test_2(int unit, int ci, int wl, int loop)
{
    if(SOC_IS_KATANA(unit)) {
        return _test_func_self_test_2_katana(unit, ci, wl, loop);
    } else if(SOC_IS_KATANA2(unit)) {
        return _test_func_self_test_2_katana2(unit, ci, wl, loop);
#ifdef BCM_PETRA_SUPPORT
    } else if(SOC_IS_ARAD(unit)) {
        return _test_func_self_test_2_arad(unit, ci, wl, loop);
#endif
#ifdef BCM_HURRICANE2_SUPPORT
    } else if(SOC_IS_HURRICANE2(unit)) {
        return _test_func_self_test_2_hurricane2(unit, ci, wl, loop);
#endif
    } else {
        return SOC_E_INTERNAL;     /* Unsupported configuration */
    }
}

STATIC uint32 _test_mem_read(int unit, int ci, uint32 addr, uint32 * data_exp)
{
    int i, j;
    uint32 uDataRD[NUM_DATA];
    uint32 result = 0;

    if(!SOC_IS_KATANAX(unit)) {
        return result;
    }

    for (i = 0; i < NUM_DATA / 8; i++) {
        soc_ddr40_read(unit, ci, addr + i, &uDataRD[(i * 8)],
                       &uDataRD[(i * 8) + 1], &uDataRD[(i * 8) + 2],
                       &uDataRD[(i * 8) + 3], &uDataRD[(i * 8) + 4],
                       &uDataRD[(i * 8) + 5], &uDataRD[(i * 8) + 6],
                       &uDataRD[(i * 8) + 7]);

        if (data_exp != NULL) {
            for (j = 0; j < 8; j++) {
                if (data_exp[(i * 8) + j] != uDataRD[(i * 8) + j]) {
                    result |= data_exp[(i * 8) + j] ^ uDataRD[(i * 8) + j];
                }
            }
            result |= (result >> 16);
            result &= 0xFFFF;
        }
    }
    return result;
}

STATIC uint32 _test_mem_wr_rd(int unit, int ci, uint32 addr)
{
    uint32 uDataWR[NUM_DATA];
    uint32 result = 0;

    if(!SOC_IS_KATANAX(unit)) {
        return result;
    }

    _fill_in_data(uDataWR);
    _test_mem_write(unit, ci, addr, uDataWR);

    result = _test_mem_read(unit, ci, addr, uDataWR);

    return result;
}

/* Prepare for shmoo-calibration */
int _soc_ddr_shmoo_prepare_for_shmoo(int unit, int ci)
{
    int wl;
    uint32 data = 0;
    int timeout_cnt, calib_steps;
    int clock_period1000, clock_period;

    wl = 0;
    /* I want to display all STATUS value here and report */
    READ_DDR40_PHY_CONTROL_REGS_PLL_STATUSr(unit, ci, &data);
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) data      = 0x%x \n"), wl, data));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) PLL_STATUS : LOCK_LOST = 0x%x \n"), wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, PLL_STATUS,
              LOCK_LOST)));
    
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) PLL_STATUS : LOCK      = 0x%x \n"), wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, PLL_STATUS,
              LOCK)));
    READ_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit, ci, &data);
    
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) data      = 0x%x \n"), wl, data));
    
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) ZQ_PVT_COMP_CTL : PD_COMP          = 0x%x \n"), wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, ZQ_PVT_COMP_CTL,
              PD_COMP)));
    
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) ZQ_PVT_COMP_CTL : ND_COMP          = 0x%x \n"), wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, ZQ_PVT_COMP_CTL,
              ND_COMP)));
    if (wl == 0) {
        READ_DDR40_PHY_WORD_LANE_0_READ_CONTROLr(unit, ci, &data);
        
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "(WL=%d) data      = 0x%x \n"), wl, data));
        
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "(WL=%d) PHY_WORD_LANE_READ_CONTROL : DQ_ODT_ENABLE = 0x%x \n"),
                  wl, DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, READ_CONTROL,
                  DQ_ODT_ENABLE)));
        
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "(WL=%d) PHY_WORD_LANE_READ_CONTROL : DQ_ODT_LE_ADJ = 0x%x \n"),
                  wl, DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, READ_CONTROL,
                  DQ_ODT_LE_ADJ)));
        
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "(WL=%d) PHY_WORD_LANE_READ_CONTROL : DQ_ODT_TE_ADJ = 0x%x \n"),
                  wl, DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, READ_CONTROL,
                  DQ_ODT_TE_ADJ)));
    } else {                    /* if (wl==0) */
        READ_DDR40_PHY_WORD_LANE_1_READ_CONTROLr(unit, ci, &data);
        
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "(WL=%d) data      = 0x%x \n"), wl, data));
        
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "(WL=%d) PHY_WORD_LANE_READ_CONTROL : DQ_ODT_ENABLE = 0x%x \n"),
                  wl, DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, READ_CONTROL,
                  DQ_ODT_ENABLE)));
        
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "(WL=%d) PHY_WORD_LANE_READ_CONTROL : DQ_ODT_LE_ADJ = 0x%x \n"),
                  wl, DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, READ_CONTROL,
                  DQ_ODT_LE_ADJ)));
        
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "(WL=%d) PHY_WORD_LANE_READ_CONTROL : DQ_ODT_TE_ADJ = 0x%x \n"),
                  wl, DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, READ_CONTROL,
                  DQ_ODT_TE_ADJ)));
    }
    READ_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit, ci, &data);
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) data      = 0x%x \n"),
              wl, data));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIBRATE : CALIB_FAST       = 0x%x \n"),
              wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
              CALIB_FAST)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIBRATE : CALIB_ONCE       = 0x%x \n"),
              wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
              CALIB_ONCE)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIBRATE : CALIB_ALWAYS     = 0x%x \n"),
              wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
              CALIB_ALWAYS)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIBRATE : CALIB_TEST       = 0x%x \n"),
              wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
              CALIB_TEST)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIBRATE : CALIB_CLOCKS     = 0x%x \n"),
              wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
              CALIB_CLOCKS)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIBRATE : CALIB_BYTE       = 0x%x \n"),
              wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
              CALIB_BYTE)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIBRATE : CALIB_PHYBIST    = 0x%x \n"),
              wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
              CALIB_PHYBIST)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIBRATE : CALIB_FTM        = 0x%x \n"),
              wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
              CALIB_FTM)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIBRATE : CALIB_AUTO       = 0x%x \n"),
              wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
              CALIB_AUTO)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIBRATE : CALIB_STEPS      = 0x%x \n"),
              wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
              CALIB_STEPS)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIBRATE : CALIB_DQS_PAIR   = 0x%x \n"),
              wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
              CALIB_DQS_PAIR)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIBRATE : CALIB_DQS_CLOCKS = 0x%x \n"),
              wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
              CALIB_DQS_CLOCKS)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIBRATE : CALIB_BIT_OFFSET = 0x%x \n"),
              wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
              CALIB_BIT_OFFSET)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIBRATE : RD_EN_CAL        = 0x%x \n"),
              wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
              RD_EN_CAL)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIBRATE : BIT_CAL          = 0x%x \n"),
              wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
              BIT_CAL)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIBRATE : SET_MR_MPR       = 0x%x \n"),
              wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
              SET_MR_MPR)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIBRATE : DQ0_ONLY         = 0x%x \n"),
              wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
              DQ0_ONLY)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIBRATE : SET_WR_DQ        = 0x%x \n"),
              wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
              SET_WR_DQ)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIBRATE : BIT_REFRESH      = 0x%x \n"),
              wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
              BIT_REFRESH)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIBRATE : RD_DLY_CAL       = 0x%x \n"),
              wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
              RD_DLY_CAL)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIBRATE : EXIT_IN_SR       = 0x%x \n"),
              wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
              EXIT_IN_SR)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIBRATE : SKIP_RST         = 0x%x \n"),
              wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
              SKIP_RST)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIBRATE : AUTO_INIT        = 0x%x \n"),
              wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
              AUTO_INIT)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIBRATE : USE_STRAPS       = 0x%x \n"),
              wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
              USE_STRAPS)));
    READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit, ci, &data);
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) data      = 0x%x \n"),
              wl, data));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIB_STATUS : CALIB_LOCK                           = 0x%x \n"),
              wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS,
              CALIB_LOCK)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIB_STATUS : CALIB_IDLE                           = 0x%x \n"),
              wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS,
              CALIB_IDLE)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIB_STATUS : CALIB_BYTE_SEL                       = 0x%x \n"),
              wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS,
              CALIB_BYTE_SEL)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIB_STATUS : CALIB_BIT_OFFSET  set if byte mode   = 0x%x \n"),
              wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS,
              CALIB_BIT_OFFSET)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) NOTE: For single step calibration total result, please see below \n"),
              wl));
    READ_DDR40_PHY_CONTROL_REGS_VDL_DQ_CALIB_STATUSr(unit, ci, &data);
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) data      = 0x%x \n"),
              wl, data));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_DQ_CALIB_STATUS : DQ_CALIB_LOCK                     = 0x%x \n"),
              wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_DQ_CALIB_STATUS,
              DQ_CALIB_LOCK)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_DQ_CALIB_STATUS : DQS_CALIB_LOCK                    = 0x%x \n"),
              wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_DQ_CALIB_STATUS,
              DQS_CALIB_LOCK)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_DQ_CALIB_STATUS : DQS_CALIB_MODE    DQS(1=pair)     = 0x%x \n"),
              wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_DQ_CALIB_STATUS,
              DQS_CALIB_MODE)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_DQ_CALIB_STATUS : DQS_CALIB_CLOCKS  DQS(0=half bit) = 0x%x \n"),
              wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_DQ_CALIB_STATUS,
              DQS_CALIB_CLOCKS)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_DQ_CALIB_STATUS : DQ_CALIB_TOTAL    DQ (steps)      = 0x%x \n"),
              wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_DQ_CALIB_STATUS,
              DQ_CALIB_TOTAL) >> 4));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_DQ_CALIB_STATUS : DQS_CALIB_TOTAL   DQS (steps)     = 0x%x \n"),
              wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_DQ_CALIB_STATUS,
              DQS_CALIB_TOTAL) >> 4));
    READ_DDR40_PHY_CONTROL_REGS_VDL_WR_CHAN_CALIB_STATUSr(unit, ci, &data);
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) data      = 0x%x \n"),
              wl, data));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_WR_CHAN_CALIB_STATUS : WR_CHAN_CALIB_LOCK                                          = 0x%x \n"),
              wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
              VDL_WR_CHAN_CALIB_STATUS, WR_CHAN_CALIB_LOCK)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_WR_CHAN_CALIB_STATUS : WR_CHAN_CALIB_BYTE_SEL   (1=byte)                           = 0x%x \n"),
              wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
              VDL_WR_CHAN_CALIB_STATUS,
              WR_CHAN_CALIB_BYTE_SEL)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_WR_CHAN_CALIB_STATUS : WR_CHAN_CALIB_CLOCKS     (0=1/2bit)                         = 0x%x \n"),
              wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
              VDL_WR_CHAN_CALIB_STATUS, WR_CHAN_CALIB_CLOCKS)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_WR_CHAN_CALIB_STATUS : WR_CHAN_CALIB_TOTAL      (steps)                            = 0x%x \n"),
              wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
              VDL_WR_CHAN_CALIB_STATUS,
              WR_CHAN_CALIB_TOTAL) >> 4));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_WR_CHAN_CALIB_STATUS : WR_CHAN_CALIB_BIT_OFFSET (in byte mode, setting for bit vdl)= 0x%x \n"),
              wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
              VDL_WR_CHAN_CALIB_STATUS,
              WR_CHAN_CALIB_BIT_OFFSET)));
    READ_DDR40_PHY_CONTROL_REGS_VDL_RD_EN_CALIB_STATUSr(unit, ci, &data);
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) data      = 0x%x \n"),
              wl, data));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_RD_EN_CALIB_STATUS : RD_EN_CALIB_LOCK                                          = 0x%x \n"),
              wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
              VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_LOCK)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_RD_EN_CALIB_STATUS : RD_EN_CALIB_BYTE_SEL   (1=byte)                           = 0x%x \n"),
              wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
              VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_BYTE_SEL)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_RD_EN_CALIB_STATUS : RD_EN_CALIB_CLOCKS     (0=1/2bit)                         = 0x%x \n"),
              wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
              VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_CLOCKS)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_RD_EN_CALIB_STATUS : RD_EN_CALIB_TOTAL      (steps)                            = 0x%x \n"),
              wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
              VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_TOTAL) >> 4));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_RD_EN_CALIB_STATUS : RD_EN_CALIB_BIT_OFFSET (in byte mode, setting for bit vdl)= 0x%x \n"),
              wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
              VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_BIT_OFFSET)));

    /* Calculate VDL calibration steps */
    /* calib steps */
    WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit, ci, 0x0);
    WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit, ci, 0x200);       /* calib_auto = 1 */
    data = 0;
    timeout_cnt = 0;
    calib_steps = 1;
    do {
        sal_usleep(10);
        READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit, ci, &data);
        if (DDR40_GET_FIELD
            (data, DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_IDLE)) {
            calib_steps =
                DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS,
                                CALIB_TOTAL);
            if (DDR40_GET_FIELD
                (data, DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_LOCK))
                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "(WL=%d) VDL_CALIB_STATUS : NOT CALIB_LOCK \n"),
                          wl));
            break;              /* Check to and wait till we find idle state */
        }                       /* if ( DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_LOCK   ) ) */
        if (timeout_cnt > 20000) {
            LOG_CLI((BSL_META_U(unit,
                                "REWRITE Memory Register Mode failed\n")));
            break;
        }
        timeout_cnt += 1;
    } while (TRUE);
    READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit, ci, &data);
    clock_period1000 = 1000000000 / SOC_DDR3_CLOCK_MHZ(unit);
    clock_period = clock_period1000 / 1000;

    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "-----------------------------------------\n")));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "---  Single STEP Calibration          ---\n")));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "-----------------------------------------\n")));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIB_STATUS : CALIB_LOCK        = %0d        \n"), wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS,
              CALIB_LOCK)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIB_STATUS : CALIB_IDLE        = %0d        \n"), wl,
              DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS,
              CALIB_IDLE)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIB_STATUS : 360' CALIB_TOTAL  = %4d.000 (steps)\n"), wl,
              calib_steps));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIB_STATUS :  90' CALIB TOTAL  = %4d.%03d (steps)\n"), wl,
              (calib_steps >> 2), (((calib_steps * 1000) - (((calib_steps >> 2) << 2) * 1000)) >> 2)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIB_STATUS : 360'   steps time = %4d.%03d (ps)   \n"),
              wl, clock_period, clock_period1000 - (clock_period * 1000)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIB_STATUS :  90'   steps time = %4d.%03d (ps)   \n"),
              wl, (clock_period >> 2), ((clock_period1000 - (((clock_period >> 2) << 2) * 1000)) >> 2)));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "(WL=%d) VDL_CALIB_STATUS : Single step  time = %4d.%03d (ps)   \n"),
              wl, (clock_period / calib_steps), ((clock_period1000 - (((clock_period / calib_steps) * calib_steps) * 1000)) / calib_steps)));

    /* Do one dummy memory read to fix RTL bug on RD_FIFO write pointer coming out of reset bug */
    data = _test_mem_read(unit, (ci + wl), 0, NULL);

    return SOC_E_NONE;
} /* _soc_ddr_shmoo_prepare_for_shmoo */


/* RESET MEMORY and Rewrite MRS register */
int _soc_katana_mem_reset_and_init_after_shmoo_addr(int unit, int ci)
{
    int i, ciCS, ciCM, ciC, num_lane, timeout_cnt;
    uint32 data, rval;

    SOC_IF_ERROR_RETURN(READ_CI_PHY_CONTROLr(unit, ci, &data));
    soc_reg_field_set(unit, CI_PHY_CONTROLr, &data, CKEf, 0);
    soc_reg_field_set(unit, CI_PHY_CONTROLr, &data, RST_Nf, 0);
    SOC_IF_ERROR_RETURN(WRITE_CI_PHY_CONTROLr(unit, ci, data));
    sal_usleep(2000);
    SOC_IF_ERROR_RETURN(READ_CI_PHY_CONTROLr(unit, ci, &data));
    soc_reg_field_set(unit, CI_PHY_CONTROLr, &data, CKEf, 0);
    soc_reg_field_set(unit, CI_PHY_CONTROLr, &data, RST_Nf, 3);
    SOC_IF_ERROR_RETURN(WRITE_CI_PHY_CONTROLr(unit, ci, data));
    sal_usleep(2000);
    SOC_IF_ERROR_RETURN(READ_CI_PHY_CONTROLr(unit, ci, &data));
    soc_reg_field_set(unit, CI_PHY_CONTROLr, &data, CKEf, 3);
    soc_reg_field_set(unit, CI_PHY_CONTROLr, &data, RST_Nf, 3);
    SOC_IF_ERROR_RETURN(WRITE_CI_PHY_CONTROLr(unit, ci, data));
    sal_usleep(2000);
    
    if(dram0_clamshell_enable || dram1_clamshell_enable) {
        
        ciC = ci & 0xFFFFFFFE;
        
        if(dram0_clamshell_enable) {
            ciCM = ciC;         /* Only Even CIs Mirrored */
            ciCS = ciC + 1;     /* Only Odd CIs Straight */
        } else {
            ciCM = ciC + 1;     /* Only Odd CIs Mirrored */
            ciCS = ciC;         /* Only Even CIs Straight */
        }
    
        rval = 0;
        soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DONEf, 1 );
        soc_reg_field_set( unit, CI_MRS_CMDr, &rval, REQf, 1 );
        soc_reg_field_set( unit, CI_MRS_CMDr, &rval, MRS_CMDf, 0x0 );
    
        /* MR2 */
        if(_check_dram(ciCS)) {
            soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DEVICE_SELECTf, 0x1 );
            soc_reg_field_set( unit, CI_MRS_CMDr, &rval, MRS_SELECTf, 0x2 );
            SOC_IF_ERROR_RETURN( WRITE_CI_MRS_CMDr( unit, ciCS, rval) );
        }
        if(_check_dram(ciCM)) {
            soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DEVICE_SELECTf, 0x2 );
            soc_reg_field_set( unit, CI_MRS_CMDr, &rval, MRS_SELECTf, 0x1 );
            SOC_IF_ERROR_RETURN( WRITE_CI_MRS_CMDr( unit, ciCM, rval) );
        }
    
        /* MR3 */
        soc_reg_field_set( unit, CI_MRS_CMDr, &rval, MRS_SELECTf, 0x3 );
        if(_check_dram(ciCS)) {
            soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DEVICE_SELECTf, 0x1 );
            SOC_IF_ERROR_RETURN( WRITE_CI_MRS_CMDr( unit, ciCS, rval) );
        }
        if(_check_dram(ciCM)) {
            soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DEVICE_SELECTf, 0x2 );
            SOC_IF_ERROR_RETURN( WRITE_CI_MRS_CMDr( unit, ciCM, rval) );
        }
    
        /* MR1 */
        if(_check_dram(ciCS)) {
            soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DEVICE_SELECTf, 0x1 );
            soc_reg_field_set( unit, CI_MRS_CMDr, &rval, MRS_SELECTf, 0x1 );
            SOC_IF_ERROR_RETURN( WRITE_CI_MRS_CMDr( unit, ciCS, rval) );
        }
        if(_check_dram(ciCM)) {
            soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DEVICE_SELECTf, 0x2 );
            soc_reg_field_set( unit, CI_MRS_CMDr, &rval, MRS_SELECTf, 0x2 );
            SOC_IF_ERROR_RETURN( WRITE_CI_MRS_CMDr( unit, ciCM, rval) );
        }
    
        /* MR0 */
        soc_reg_field_set( unit, CI_MRS_CMDr, &rval, MRS_SELECTf, 0x0 );
        if(_check_dram(ciCS)) {
            soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DEVICE_SELECTf, 0x1 );
            SOC_IF_ERROR_RETURN( WRITE_CI_MRS_CMDr( unit, ciCS, rval) );
        }
        if(_check_dram(ciCM)) {
            soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DEVICE_SELECTf, 0x2 );
            SOC_IF_ERROR_RETURN( WRITE_CI_MRS_CMDr( unit, ciCM, rval) );
        }
    
        /* ZQ Cal Long */
        soc_reg_field_set( unit, CI_MRS_CMDr, &rval, MRS_CMDf, 0x3 );
        soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DEVICE_SELECTf, 0x3 );
        soc_reg_field_set( unit, CI_MRS_CMDr, &rval, MRS_SELECTf, 0x3 );
        if(_check_dram(ciCS)) {
            SOC_IF_ERROR_RETURN( WRITE_CI_MRS_CMDr( unit, ciCS, rval) );
        }
        if(_check_dram(ciCM)) {
            SOC_IF_ERROR_RETURN( WRITE_CI_MRS_CMDr( unit, ciCM, rval) );
        }

    } else {
    
        num_lane = 2;
        
        for (i = 0; i < num_lane; i++) {
            if(!_check_dram(ci+i)) {
                continue;
            }
            SOC_IF_ERROR_RETURN(READ_CI_DDR_AUTOINITr(unit, ci + i, &data));
            soc_reg_field_set(unit, CI_DDR_AUTOINITr, &data, DONEf, 1);
            soc_reg_field_set(unit, CI_DDR_AUTOINITr, &data, STARTf, 0);
            SOC_IF_ERROR_RETURN(WRITE_CI_DDR_AUTOINITr(unit, ci + i, data));
            soc_reg_field_set(unit, CI_DDR_AUTOINITr, &data, STARTf, 1);
            SOC_IF_ERROR_RETURN(WRITE_CI_DDR_AUTOINITr(unit, ci + i, data));
        }
        for (i = 0; i < num_lane; i++) {
            if(!_check_dram(ci+i)) {
                continue;
            }
            timeout_cnt = 0;
            do {
                sal_usleep(10);
                SOC_IF_ERROR_RETURN(READ_CI_DDR_AUTOINITr(unit, ci + i, &data));
                if (soc_reg_field_get(unit, CI_DDR_AUTOINITr, data, DONEf)) {
                    break;
                }
                if (timeout_cnt > 20000) {
                    LOG_CLI((BSL_META_U(unit,
                                        "REWRITE Memory Register Mode failed (DRAM %2d)\n"), ci+i));
                    break;
                }
                timeout_cnt += 1;
            } while (TRUE);
        }
    
    }

    sal_usleep(2000);
    
    return SOC_E_NONE;
}

#ifdef BCM_PETRA_SUPPORT
int _soc_arad_mem_reset_and_init_after_shmoo_addr(int unit, int ci)
{
    int timeout_cnt;
    uint32 rval;
#ifdef BCM_PETRA_SUPPORT
    uint8 is_valid[SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)] = {0};

    is_valid[ci>>1] = 1;

    arad_mgmt_dram_init_drc_soft_init(unit, is_valid, 1);
    sal_usleep(2000);
    arad_mgmt_dram_init_drc_soft_init(unit, is_valid, 0);
#endif

    timeout_cnt = 0;
    do {
        sal_usleep(10);
        SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, ci>>1, DRC_DRAM_INIT_FINISHEDr, &rval));
        if (soc_reg_field_get(unit, DRCA_DRAM_INIT_FINISHEDr, rval, DRAM_INIT_FINISHEDf)) {
            break;
        }
        if (timeout_cnt > 20000) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "CI%d: Timed out DRAM re-initialization\n"), ci));
            return SOC_E_TIMEOUT;
        }
        timeout_cnt += 1;
    } while (TRUE);

    return SOC_E_NONE;
}
#endif

int
_soc_mem_reset_and_init_after_shmoo_addr(int unit, int ci)
{
    if(SOC_IS_KATANAX(unit)) {
        return _soc_katana_mem_reset_and_init_after_shmoo_addr(unit, ci);
#ifdef BCM_PETRA_SUPPORT
    } else if(SOC_IS_ARAD(unit)) {
        return _soc_arad_mem_reset_and_init_after_shmoo_addr(unit, ci);
#endif
    } else {
        return SOC_E_INTERNAL;     /* Unsupported configuration */
    }
}

/* Initialize VDL 90' result from VDL Calibration */
int _shmoo_init_vdl_result(int unit, int ci, int wl, int self_test, uint32 * result)
{
    int byte, bit;
    uint32 data_expected[NUM_DATA];
    uint32 data, setting, setting_steps, setting_byte_sel;
    int32 steps_calib_total;
    int calib_steps;

    _fill_in_data(data_expected);
    if (self_test == 0) {
        _test_mem_write(unit, (ci + wl), 0, data_expected);
    }

    /* RD_DATA_DELAY */
    data = 1; /* 0 is undefined for RD_DATA_DELAY */
    if(wl == 0) {
        WRITE_DDR40_PHY_WORD_LANE_0_READ_DATA_DLYr(unit, ci, data);
            
    } else {
        WRITE_DDR40_PHY_WORD_LANE_1_READ_DATA_DLYr(unit, ci, data);
    }

    /* RD_EN */
    READ_DDR40_PHY_CONTROL_REGS_VDL_RD_EN_CALIB_STATUSr(unit, ci, &data);
    setting_byte_sel =
        DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_RD_EN_CALIB_STATUS,
                        RD_EN_CALIB_BYTE_SEL);
    setting_steps =
        DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_RD_EN_CALIB_STATUS,
                        RD_EN_CALIB_TOTAL) >> 4;
    if(setting_byte_sel) {
        setting = (setting_steps * 3) >> 1;
    } else {
        setting = setting_steps >> 1;
    }
    setting_byte_sel = 1;
    if (wl == 0) {
        setting += SHMOO_CI02_WL0_OFFSET_RD_EN;
    } else {
        setting += SHMOO_CI0_WL1_OFFSET_RD_EN;
    }
    if (setting & 0x80000000) {
        setting = 0;
    } else if (setting > 63) {
        setting = 63;
    }
    DDR40_SET_FIELD(setting, DDR40_PHY_CONTROL_REGS, VDL_OVRIDE_BYTE_CTL,
                    BYTE_SEL, setting_byte_sel);
    data = SET_OVR_STEP(setting);
    if (wl == 0) {
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci,
                                                                data);
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci,
                                                                data);
    } else {
        WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
        WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci,
                                                                data);
        WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci,
                                                                data);
    }

    /* RD_DQ */
    READ_DDR40_PHY_CONTROL_REGS_VDL_DQ_CALIB_STATUSr(unit, ci, &data);
    steps_calib_total =
        DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_DQ_CALIB_STATUS,
                        DQ_CALIB_TOTAL) >> 4;
    setting = steps_calib_total;
    if (wl == 0) {
        setting += SHMOO_CI02_WL0_OFFSET_RD_DQ;
    } else {
        setting += SHMOO_CI0_WL1_OFFSET_RD_DQ;
    }
    if (setting & 0x80000000) {
        setting = 0;
    } else if (setting > 63) {
        setting = 63;
    }
    data = SET_OVR_STEP(setting);
    for (byte = 0; byte < 2; byte++) {
        for (bit = 0; bit < 8; bit++) {
            WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Pr(wl, byte,
                                                                   bit, unit,
                                                                   ci, data);
            WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Nr(wl, byte,
                                                                   bit, unit,
                                                                   ci, data);
        }
    }

    /* RD_DQS */
    READ_DDR40_PHY_CONTROL_REGS_VDL_DQ_CALIB_STATUSr(unit, ci, &data);
    setting_byte_sel =
        DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_DQ_CALIB_STATUS,
                        DQS_CALIB_MODE);
    setting_steps =
        DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_DQ_CALIB_STATUS,
                        DQS_CALIB_TOTAL) >> 4;
    calib_steps =
                DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS,
                                CALIB_TOTAL);
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "calib_steps: %d\n"),
              calib_steps));
    setting = (setting_steps * (1 + setting_byte_sel));
    if (wl == 0) {
        setting += SHMOO_CI02_WL0_OFFSET_RD_DQS;
    } else {
        setting += SHMOO_CI0_WL1_OFFSET_RD_DQS;
    }
    if (setting & 0x80000000) {
        setting = 0;
    } else if (setting > 63) {
        setting = 63;
    }
    data = SET_OVR_STEP(setting);
    if (wl == 0) {
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, data);
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_Nr(unit, ci, data);
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, data);
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, data);
    } else {
        WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, data);
        WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_Nr(unit, ci, data);
        WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, data);
        WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, data);
    }

    /* WR_DQ  */
    /* bit mode */
    READ_DDR40_PHY_CONTROL_REGS_VDL_WR_CHAN_CALIB_STATUSr(unit, ci, &data);
    setting_byte_sel =
        DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_WR_CHAN_CALIB_STATUS,
                        WR_CHAN_CALIB_BYTE_SEL);
    setting_steps =
        DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_WR_CHAN_CALIB_STATUS,
                        WR_CHAN_CALIB_TOTAL) >> 4;
    setting = setting_steps;
    if (SOC_IS_ARAD(unit)) {
        /* Same offset applies to both WLs */
        setting += SOC_DDR3_OFFSET_WR_DQ_CI02_WL0(unit);
    } else {
        if (SOC_IS_KATANAX(unit) && (dram0_clamshell_enable || dram1_clamshell_enable)) {
            setting += SHMOO_CI02_WL0_OFFSET_WR_DQ;
        } else {
            if (wl == 0) {
                setting += SHMOO_CI02_WL0_OFFSET_WR_DQ;
            } else {
                setting += SHMOO_CI0_WL1_OFFSET_WR_DQ;
            }
        }
    }
    if (setting & 0x80000000) {
        setting = 0;
    } else if (setting > 63) {
        setting = 63;
    }
    DDR40_SET_FIELD(setting, DDR40_PHY_CONTROL_REGS, VDL_OVRIDE_BYTE_CTL,
                    BYTE_SEL, setting_byte_sel);
    data = SET_OVR_STEP(setting);
    for (byte = 0; byte < 2; byte++) {
        for (bit = 0; bit < 8; bit++) {
            WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_Wr(wl, byte, bit,
                                                                 unit, ci,
                                                                 data);
        }
    }
    /* byte */
    data = SET_OVR_STEP(0);
    if (wl == 0) {
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_Wr(unit, ci, data);
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_Wr(unit, ci, data);
    } else {
        WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_Wr(unit, ci, data);
        WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_Wr(unit, ci, data);
    }

    /* ADDR   */
    if ( wl == 0 ) {
        /* Hardware adjust ADDR 90' prior VDL
         * Because we cannot trust ADDR Calibration result as it reports 0 steps
         * We then will use WR_DQ 90' calibration result to adjust ADDR (by 90') to become 180' for the address
         * Excluded below and use WR_DQ instead
         * setting_byte_sel = DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_BYTE_SEL  ) ;
         * setting_steps = DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_TOTAL )>>4 ;
         * setting = (setting_steps*(1+setting_byte_sel));
         */ 
        READ_DDR40_PHY_CONTROL_REGS_VDL_WR_CHAN_CALIB_STATUSr(unit, ci, &data);
        setting_byte_sel =
            DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
                            VDL_WR_CHAN_CALIB_STATUS, WR_CHAN_CALIB_BYTE_SEL);
        setting_steps =
            DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
                            VDL_WR_CHAN_CALIB_STATUS,
                            WR_CHAN_CALIB_TOTAL) >> 4;
        setting = setting_steps;
        DDR40_SET_FIELD(setting, DDR40_PHY_CONTROL_REGS, VDL_OVRIDE_BYTE_CTL,
                        BYTE_SEL, setting_byte_sel);

        data = SET_OVR_STEP(40);
        WRITE_DDR40_PHY_CONTROL_REGS_VDL_OVRIDE_BIT_CTLr(unit, ci, data); 
    }

    /* Shmoo  */
    /* Do one dummy memory read to fix RTL bug on RD_FIFO write pointer coming out of reset bug */
    data = _test_mem_read(unit, (ci + wl), 0, NULL);
    
    /* Clear the FIFO error state */
    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
    return SOC_E_NONE;
}

#if 0 /* Uncomment this when required */
int
_shmoo_rd_en_rd_data_dly(int unit, int ci, int wl, int self_test, uint32 * result)
{
    int i, j;
    uint32 data;
    uint32 data_expected[NUM_DATA];
    int see_pass, see_fail, rd_en_pass, rd_en_fail, rd_en_size;
    uint32 dataB1p, dataB1n;

    int rd_data_dly_pass_sum = 0;
    int rd_data_dly_pass_count = 0;
    int rd_data_dly_pass_avg = 0;
    int rd_en_window_found = 0;

    _fill_in_data(data_expected);
    if (self_test == 0) {
        _test_mem_write(unit, (ci + wl), 0, data_expected);
    }

    /* Clear the FIFO error state */
    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 1);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 1);
    /* Clear any error status from previous run */
    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_STATUSr(unit, ci, 1);
    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_STATUSr(unit, ci, 0);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_STATUSr(unit, ci, 1);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_STATUSr(unit, ci, 0);

    /* RD_EN should be good by this point. We now find the best RD_DATA_DLY (for fast speed, we start from 3 and up) */
    rd_en_size = 0;

    if(wl == 0) {
        READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, &dataB1p);
        READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, &dataB1n);
        dataB1p =
            DDR40_GET_FIELD(dataB1p, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE1_R_P,
                            OVR_STEP);
        dataB1n =
            DDR40_GET_FIELD(dataB1n, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE1_R_N,
                            OVR_STEP);
    } else {
        READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, &dataB1p);
        READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, &dataB1n);
        dataB1p =
            DDR40_GET_FIELD(dataB1p, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE1_R_P,
                            OVR_STEP);
        dataB1n =
            DDR40_GET_FIELD(dataB1n, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE1_R_N,
                            OVR_STEP);
    }

    /* Find Best RD_DATA_DLY  */
    for (j = 1; j < 8; ++j) {
        /* For every single RD_DATA_DLY, we shmoo RD_EN to find the largest window */
        data = j;
        if (wl == 0) {
            WRITE_DDR40_PHY_WORD_LANE_0_READ_DATA_DLYr(unit, ci, data);
        } else {
            WRITE_DDR40_PHY_WORD_LANE_1_READ_DATA_DLYr(unit, ci, data);
        }
        see_pass = 0;
        see_fail = 0;
        rd_en_pass = 0;
        rd_en_fail = 0;
        rd_en_size = 0;
        rd_en_window_found = 0;
        for (i = 0; i < 64; ++i) {
            data = SET_OVR_STEP(i);

            /* RD_EN */
            if (wl == 0) {
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit,
                                                                        ci,
                                                                        data);
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit,
                                                                        ci,
                                                                        data);
            } else {
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit,
                                                                        ci,
                                                                        data);
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit,
                                                                        ci,
                                                                        data);
            }

            /* Clear the FIFO error state */
            WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
            WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
            sal_usleep(9);
            if (self_test==0) {
                result[i] = _test_mem_read(unit, (ci+wl), 0, data_expected);
            } else if (self_test==2) {
                result[i] = _test_rd_data_fifo_status(unit, ci, wl, 1); 
            } else {
                result[i] =
                    _test_func_self_test_1(unit, ci, wl, 1, 0);
            }

            if (result[i] == 0) {       /* PASS */
                if (see_pass == 0) {
                    rd_en_pass = i;
                    see_pass = 1;
                }
            } else {
                if (see_pass==1 && see_fail==0) {
                    rd_en_fail = i;
                    see_fail = 1;
                }
            }

            if(rd_en_window_found == 0) {
                if (see_pass==1 && see_fail==1) { 
                    rd_en_size = rd_en_fail - rd_en_pass;
                    if(rd_en_size < 3) {
                        see_pass = 0;
                        see_fail = 0;
                    } else {
                        rd_data_dly_pass_sum += j;
                        rd_data_dly_pass_count++;
                        rd_en_window_found = 1;
                    }
                } else if (see_pass==1 && i==63) { 
                    rd_en_size = i - rd_en_pass;
                    if(rd_en_size < 3) {
                        see_pass = 0;
                        see_fail = 0;
                    } else {
                        rd_data_dly_pass_sum += j;
                        rd_data_dly_pass_count++;
                        rd_en_window_found = 1;
                    }
                }
            }
        }
    }

    if(rd_data_dly_pass_count != 0) {
        rd_data_dly_pass_avg = rd_data_dly_pass_sum / rd_data_dly_pass_count;
    } else {
        rd_data_dly_pass_avg = 1;
    }

    /* set RD_EN and RD_DATA_DLY */
    /* For every single RD_DATA_DLY, we shmoo RD_EN to find the largest window */
    data = rd_data_dly_pass_avg;
    if (wl == 0) {
        WRITE_DDR40_PHY_WORD_LANE_0_READ_DATA_DLYr(unit, ci, data);
    } else {
        WRITE_DDR40_PHY_WORD_LANE_1_READ_DATA_DLYr(unit, ci, data);
    }

    /* Do one dummy memory read to fix RTL bug on RD_FIFO write pointer coming out of reset bug */
    data = _test_mem_read(unit, (ci + wl), 0, NULL);
    for (i = 0; i < 64; ++i) {
        data = SET_OVR_STEP(i);

        /* RD_EN */
        if (wl == 0) {
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci,
                                                                    data);
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci,
                                                                    data);
        } else {
            WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
            WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci,
                                                                    data);
            WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci,
                                                                    data);
        }

        /* Clear the FIFO error state */
        WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
        WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
        sal_usleep(9);
        if (self_test == 0) {
            result[i] = _test_mem_read(unit, (ci + wl), 0, data_expected);
        } else if (self_test==2) {
            result[i] = _test_rd_data_fifo_status(unit, ci, wl, 1);
        } else {
            result[i] =
                _test_func_self_test_1(unit, ci, wl, 1, 0);
        }
    }

    return SOC_E_NONE;
}
#endif

int
_shmoo_rd_data_dly(int unit, int ci, int wl, int self_test, uint32 * result)
{
    int i, j;
    uint32 data;
    uint32 data_expected[NUM_DATA];
    int see_pass, see_fail, rd_en_pass, rd_en_fail, rd_en_size;
    uint32 dataB1p, dataB1n;
    uint32 result0 = 0;

    int rd_data_dly_pass_sum = 0;
    int rd_data_dly_pass_count = 0;
    int rd_data_dly_pass_avg = 0;
    int rd_en_window_found = 0;

    _fill_in_data(data_expected);
    if (self_test == 0) {
        _test_mem_write(unit, (ci + wl), 0, data_expected);
    }

    /* Clear the FIFO error state */
    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 1);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 1);
    /* Clear any error status from previous run */
    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_STATUSr(unit, ci, 1);
    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_STATUSr(unit, ci, 0);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_STATUSr(unit, ci, 1);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_STATUSr(unit, ci, 0);

    /* RD_EN should be good by this point. We now find the best RD_DATA_DLY (for fast speed, we start from 3 and up) */
    rd_en_size = 0;

    if(wl == 0) {
        READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, &dataB1p);
        READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, &dataB1n);
        dataB1p =
            DDR40_GET_FIELD(dataB1p, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE1_R_P,
                            OVR_STEP);
        dataB1n =
            DDR40_GET_FIELD(dataB1n, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE1_R_N,
                            OVR_STEP);
    } else {
        READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, &dataB1p);
        READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, &dataB1n);
        dataB1p =
            DDR40_GET_FIELD(dataB1p, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE1_R_P,
                            OVR_STEP);
        dataB1n =
            DDR40_GET_FIELD(dataB1n, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE1_R_N,
                            OVR_STEP);
    }

    /* Find Best RD_DATA_DLY  */
    for (j = 1; j < 8; ++j) {
        /* For every single RD_DATA_DLY, we shmoo RD_EN to find the largest window */
        data = j;
        if (wl == 0) {
            WRITE_DDR40_PHY_WORD_LANE_0_READ_DATA_DLYr(unit, ci, data);
        } else {
            WRITE_DDR40_PHY_WORD_LANE_1_READ_DATA_DLYr(unit, ci, data);
        }
        see_pass = 0;
        see_fail = 0;
        rd_en_pass = 0;
        rd_en_fail = 0;
        rd_en_size = 0;
        rd_en_window_found = 0;
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "RD_DATA_DLY Iter: %d\t"),
                  j)); 
        for (i = 0; i < 64; ++i) {
            data = SET_OVR_STEP(i);

            /* RD_EN */
            if (wl == 0) {
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit,
                                                                        ci,
                                                                        data);
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit,
                                                                        ci,
                                                                        data);
            } else {
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit,
                                                                        ci,
                                                                        data);
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit,
                                                                        ci,
                                                                        data);
            }

            /* Clear the FIFO error state */
            WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
            WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
            sal_usleep(9);
            if (self_test==0) {
                result[i] = _test_mem_read(unit, (ci+wl), 0, data_expected);
            } else if (self_test==2) {
                result[i] = _test_rd_data_fifo_status(unit, ci, wl, 1); 
            } else {
                result[i] =
                    _test_func_self_test_1(unit, ci, wl, 1, 3);
            }

            if (result[i] == 0) {       /* PASS */
                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "+"))); 
                if (see_pass == 0) {
                    rd_en_pass = i;
                    see_pass = 1;
                }
            } else {
                LOG_INFO(BSL_LS_SOC_DDR,
                         (BSL_META_U(unit,
                                     "-"))); 
                if (see_pass==1 && see_fail==0) {
                    rd_en_fail = i;
                    see_fail = 1;
                }
            }

            if(rd_en_window_found == 0) {
                if (see_pass==1 && see_fail==1) { 
                    rd_en_size = rd_en_fail - rd_en_pass;
                    if(rd_en_size < 3) {
                        see_pass = 0;
                        see_fail = 0;
                    } else {
                        rd_data_dly_pass_sum += j;
                        rd_data_dly_pass_count++;
                        rd_en_window_found = 1;
                    }
                } else if (see_pass==1 && i==63) { 
                    rd_en_size = i - rd_en_pass;
                    if(rd_en_size < 3) {
                        see_pass = 0;
                        see_fail = 0;
                    } else {
                        rd_data_dly_pass_sum += j;
                        rd_data_dly_pass_count++;
                        rd_en_window_found = 1;
                    }
                }
            }
        }
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "\n"))); 
        if(rd_data_dly_pass_count != 0) {
            rd_data_dly_pass_avg = rd_data_dly_pass_sum + 1;
            break;
        }
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "RD_DATA_DLY Iter: %d\tCount: %d\tSum: %2d\n"),
                  j, rd_data_dly_pass_count, rd_data_dly_pass_sum)); 
    }
/*
    if(rd_data_dly_pass_count != 0) {
        rd_data_dly_pass_avg = rd_data_dly_pass_sum / rd_data_dly_pass_count;
    } else {
        rd_data_dly_pass_avg = 1;
    } */

    if(rd_data_dly_pass_count == 0) {
        rd_data_dly_pass_avg = 4;
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "Did not find valid RD_DATA_DELAY. Forcing RD_DATA_DELAY = %d\n"),
                  rd_data_dly_pass_avg));
    } else {
      /*  if(wl == 0) {
        rd_data_dly_pass_avg = 7;
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "Found valid RD_DATA_DELAY but forcing RD_DATA_DELAY = %d\n"),
                  rd_data_dly_pass_avg));
        } */
      /*  if(wl == 1) {
            rd_data_dly_pass_avg = result0;
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Found valid RD_DATA_DELAY but copying WL0 RD_DATA_DELAY = %d\n"),
                      rd_data_dly_pass_avg));
        } */
    }

    /* set RD_EN and RD_DATA_DLY */
    /* For every single RD_DATA_DLY, we shmoo RD_EN to find the largest window */
    data = rd_data_dly_pass_avg;
    if (wl == 0) {
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "Switching to RD_DATA_DELAY Step  : %2d (WL = 0)\n"),
                  data));
        /* result0 = data; */
        WRITE_DDR40_PHY_WORD_LANE_0_READ_DATA_DLYr(unit, ci, data);
    } else {
        READ_DDR40_PHY_WORD_LANE_0_READ_DATA_DLYr(unit, ci, &result0);
        result0 =
            DDR40_GET_FIELD(result0, DDR40_PHY_WORD_LANE_0, READ_DATA_DLY, RD_DATA_DLY);
        if(data > result0) {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Equalizing to RD_DATA_DELAY Step : %2d (Both WLs)\n"),
                      data));
            WRITE_DDR40_PHY_WORD_LANE_0_READ_DATA_DLYr(unit, ci, data);
            WRITE_DDR40_PHY_WORD_LANE_1_READ_DATA_DLYr(unit, ci, data);
        } else {
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Copying WL0 RD_DATA_DELAY Step   : %2d (WL = 1)\n"),
                      result0));
            WRITE_DDR40_PHY_WORD_LANE_1_READ_DATA_DLYr(unit, ci, result0);
        }
    }

    /* Clear the FIFO error state */
    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);

    return SOC_E_NONE;
}

STATIC int _shmoo_rd_dq(int unit, int ci, int wl)
{
    uint32 data = 0;
    uint32 init_rd_dqs_step;
    uint32 init_rd_en_step;
    uint32 new_rd_dqs_step;
    uint32 new_rd_en_step;
    uint32 new_rd_dq_step0 = 0xFFFFFFFF;
    uint32 new_rd_dq_step1 = 0xFFFFFFFF;
    uint32 result;
    int byte, bit;
    int i;
    int fail_count0 = 0;
    int pass_count0 = 0;
    int max_pass0 = 0;
    int first_fail0 = 63;
    int fail_count1 = 0;
    int pass_count1 = 0;
    int max_pass1 = 0;
    int first_fail1 = 63;
    uint32 new_rd_en_avg_step;
    
    if(wl == 0) {
        READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, &data);
        init_rd_dqs_step =
            DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE0_R_P,
                            OVR_STEP);
        READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, &data);
        init_rd_en_step =
            DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE0_BIT_RD_EN,
                            OVR_STEP);
        new_rd_dqs_step = 0;
        if(init_rd_en_step > (init_rd_dqs_step >> RD_EN_DQS_CORROLATION)) {
            new_rd_en_step = init_rd_en_step - (init_rd_dqs_step >> RD_EN_DQS_CORROLATION);
        } else {
            new_rd_en_step = 0;
        }
        new_rd_en_avg_step = new_rd_en_step;
        data = SET_OVR_STEP(new_rd_dqs_step);
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, data);
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_Nr(unit, ci, data);
        data = SET_OVR_STEP(new_rd_en_step);
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, data);
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "Initial RD_DQS Setting   (Byte 0): %2d\n"),
                  init_rd_dqs_step));
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "Initial RD_EN Setting    (Byte 0): %2d\n"),
                  init_rd_en_step));
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "New RD_DQS Setting       (Byte 0): %2d\n"),
                  new_rd_dqs_step));
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "New RD_EN Setting        (Byte 0): %2d\n"),
                  new_rd_en_step));
        READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, &data);
        init_rd_dqs_step =
            DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE1_R_P,
                            OVR_STEP);
        READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, &data);
        init_rd_en_step =
            DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE1_BIT_RD_EN,
                            OVR_STEP);
        new_rd_dqs_step = 0;
        if(init_rd_en_step > (init_rd_dqs_step >> RD_EN_DQS_CORROLATION)) {
            new_rd_en_step = init_rd_en_step - (init_rd_dqs_step >> RD_EN_DQS_CORROLATION);
        } else {
            new_rd_en_step = 0;
        }
        new_rd_en_avg_step = (new_rd_en_avg_step + new_rd_en_step) >> 1;
        data = SET_OVR_STEP(new_rd_dqs_step);
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, data);
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, data);
        data = SET_OVR_STEP(new_rd_en_step);
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, data);
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "Initial RD_DQS Setting   (Byte 1): %2d\n"),
                  init_rd_dqs_step));
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "Initial RD_EN Setting    (Byte 1): %2d\n"),
                  init_rd_en_step));
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "New RD_DQS Setting       (Byte 1): %2d\n"),
                  new_rd_dqs_step));
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "New RD_EN Setting        (Byte 1): %2d\n"),
                  new_rd_en_step));
        data = SET_OVR_STEP(new_rd_en_avg_step);
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
    } else {
        READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, &data);
        init_rd_dqs_step =
            DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE0_R_P,
                            OVR_STEP);
        READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, &data);
        init_rd_en_step =
            DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE0_BIT_RD_EN,
                            OVR_STEP);
        new_rd_dqs_step = 0;
        if(init_rd_en_step > (init_rd_dqs_step >> RD_EN_DQS_CORROLATION)) {
            new_rd_en_step = init_rd_en_step - (init_rd_dqs_step >> RD_EN_DQS_CORROLATION);
        } else {
            new_rd_en_step = 0;
        }
        new_rd_en_avg_step = new_rd_en_step;
        data = SET_OVR_STEP(new_rd_dqs_step);
        WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, data);
        WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_Nr(unit, ci, data);
        data = SET_OVR_STEP(new_rd_en_step);
        WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, data);
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "Initial RD_DQS Setting   (Byte 0): %2d\n"),
                  init_rd_dqs_step));
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "Initial RD_EN Setting    (Byte 0): %2d\n"),
                  init_rd_en_step));
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "New RD_DQS Setting       (Byte 0): %2d\n"),
                  new_rd_dqs_step));
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "New RD_EN Setting        (Byte 0): %2d\n"),
                  new_rd_en_step));
        READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, &data);
        init_rd_dqs_step =
            DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE1_R_P,
                            OVR_STEP);
        READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, &data);
        init_rd_en_step =
            DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE1_BIT_RD_EN,
                            OVR_STEP);
        new_rd_dqs_step = 0;
        if(init_rd_en_step > (init_rd_dqs_step >> RD_EN_DQS_CORROLATION)) {
            new_rd_en_step = init_rd_en_step - (init_rd_dqs_step >> RD_EN_DQS_CORROLATION);
        } else {
            new_rd_en_step = 0;
        }
        new_rd_en_avg_step = (new_rd_en_avg_step + new_rd_en_step) >> 1;
        data = SET_OVR_STEP(new_rd_dqs_step);
        WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, data);
        WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, data);
        data = SET_OVR_STEP(new_rd_en_step);
        WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, data);
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "Initial RD_DQS Setting   (Byte 1): %2d\n"),
                  init_rd_dqs_step));
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "Initial RD_EN Setting    (Byte 1): %2d\n"),
                  init_rd_en_step));
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "New RD_DQS Setting       (Byte 1): %2d\n"),
                  new_rd_dqs_step));
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "New RD_EN Setting        (Byte 1): %2d\n"),
                  new_rd_en_step));
        data = SET_OVR_STEP(new_rd_en_avg_step);
        WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
    }
    
    for (i = 0; i < 64; ++i) {
        data = SET_OVR_STEP(i);

        /* RD_DQ */
        for (byte = 0; byte < 2; byte++) {
            for (bit = 0; bit < 8; bit++) {
                WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Pr(wl,
                                                                       byte,
                                                                       bit,
                                                                       unit,
                                                                       ci,
                                                                       data);
                WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Nr(wl,
                                                                       byte,
                                                                       bit,
                                                                       unit,
                                                                       ci,
                                                                       data);
            }
        }

        /* Clear the FIFO error state */
        WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
        WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
        sal_usleep(9);
        result = _test_func_self_test_1(unit, ci, wl, 1, 0);

        if(new_rd_dq_step0 == 0xFFFFFFFF) {
            if((result & 0x00FF) != 0) {
                if(first_fail0 == 63) {
                    first_fail0 = i;
                }
                if(max_pass0 < pass_count0) {
                    max_pass0 = pass_count0;
                }
                pass_count0 = 0;
                fail_count0++;
            } else {
                fail_count0 = 0;
                pass_count0++;
            }
            if((max_pass0 >= 5) && (fail_count0 == 5)) {
                new_rd_dq_step0 = i;
            }
        }
        if(new_rd_dq_step1 == 0xFFFFFFFF) {
            if((result & 0xFF00) != 0) {
                if(first_fail1 == 63) {
                    first_fail1 = i;
                }
                if(max_pass1 < pass_count1) {
                    max_pass1 = pass_count1;
                }
                pass_count1 = 0;
                fail_count1++;
            } else {
                fail_count1 = 0;
                pass_count1++;
            }
            if((max_pass1 >= 5) && (fail_count1 == 5)) {
                new_rd_dq_step1 = i;
            }
        }
        if((new_rd_dq_step0 != 0xFFFFFFFF) && (new_rd_dq_step1 != 0xFFFFFFFF)) {
            break;
        }
    }
    if(i == 64) {
        if(new_rd_dq_step0 == 0xFFFFFFFF) {
            new_rd_dq_step0 = first_fail0;
        }
        if(new_rd_dq_step1 == 0xFFFFFFFF) {
            new_rd_dq_step1 = first_fail1;
        }
    }

    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "Switching to RD_DQ Step (Byte 0) : %2d\n"),
              new_rd_dq_step0));
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "Switching to RD_DQ Step (Byte 1) : %2d\n"),
              new_rd_dq_step1));

    data = SET_OVR_STEP(new_rd_dq_step0);
    for (bit = 0; bit < 8; bit++) {
        WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Pr(wl, 0,
                                                               bit, unit,
                                                               ci, data);
        WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Nr(wl, 0,
                                                               bit, unit,
                                                               ci, data);
    }
    data = SET_OVR_STEP(new_rd_dq_step1);
    for (bit = 0; bit < 8; bit++) {
        WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Pr(wl, 1,
                                                               bit, unit,
                                                               ci, data);
        WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Nr(wl, 1,
                                                               bit, unit,
                                                               ci, data);
    }

    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
    return SOC_E_NONE;

}

STATIC int _shmoo_rd_en_rd_dq(int unit, int ci, int wl, int self_test, vref_word_shmoo *vwsPtr)
{
    uint32 data;
    int byte, bit;
    int i, j;

    uint32 data_expected[NUM_DATA];

    _fill_in_data(data_expected);
    if (self_test == 0) {
        _test_mem_write(unit, (ci + wl), 0, data_expected);
    }
    
    for (i = 0; i < 64; ++i) {
        data = SET_OVR_STEP(i);

        /* RD_DQ */
        for (byte = 0; byte < 2; byte++) {
            for (bit = 0; bit < 8; bit++) {
                WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Pr(wl,
                                                                       byte,
                                                                       bit,
                                                                       unit,
                                                                       ci,
                                                                       data);
                WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Nr(wl,
                                                                       byte,
                                                                       bit,
                                                                       unit,
                                                                       ci,
                                                                       data);
            }
        }

        for (j = 0; j < 64; ++j) {
            data = SET_OVR_STEP(j);

            /* RD_EN */
            if (wl == 0) {
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci,
                                                                        data);
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci,
                                                                        data);
            } else {
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci,
                                                                        data);
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci,
                                                                        data);
            }

            /* Clear the FIFO error state */
            WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
            WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
            sal_usleep(9);
            if (self_test == 0) {
                (*vwsPtr)[i].raw_result[j] = _test_mem_read(unit, (ci + wl), 0, data_expected);
            } else if (self_test==2) {
                (*vwsPtr)[i].raw_result[j] = _test_rd_data_fifo_status(unit, ci, wl, 1);
            } else {
                    (*vwsPtr)[i].raw_result[j] =
                        _test_func_self_test_1(unit, ci, wl, 1, 0); /* MPR = 4 */
            }
        }
    }

    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
    return SOC_E_NONE;

}

#if 0 /* Uncomment this when required */
STATIC int _shmoo_rd_en(int unit, int ci, int wl, int self_test, uint32 * result)
{
    uint32 data;
    int i;

    uint32 data_expected[NUM_DATA];

    _fill_in_data(data_expected);
    if (self_test == 0) {
        _test_mem_write(unit, (ci + wl), 0, data_expected);
    }
    
    for (i = 0; i < 64; ++i) {
        data = SET_OVR_STEP(i);

        /* RD_EN */
        if (wl == 0) {
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci,
                                                                    data);
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci,
                                                                    data);
        } else {
            WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
            WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci,
                                                                    data);
            WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci,
                                                                    data);
        }

        /* Clear the FIFO error state */
        WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
        WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
        sal_usleep(9);
        if (self_test == 0) {
            result[i] = _test_mem_read(unit, (ci + wl), 0, data_expected);
        } else if (self_test==2) {
            result[i] = _test_rd_data_fifo_status(unit, ci, wl, 1);
        } else {
            if(SOC_IS_KATANAX(unit)) {
                result[i] =
                    _test_func_self_test_1(unit, ci, wl, 1, 3);
            } else {
                result[i] =
                    _test_func_self_test_1(unit, ci, wl, 1, 3); /* MPR = 4 */
            }
        }
    }

    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
    return SOC_E_NONE;

}
#endif

STATIC int _shmoo_rd_dqs(int unit, int ci, int wl, int self_test, vref_word_shmoo *vwsPtr)
{
    int32 i, vref;
    uint32 data;
    uint32 data_expected[NUM_DATA];
    
    uint32 init_rd_en_step0, init_rd_en_step1;
    uint32 new_rd_en_step, new_rd_en_byte_step;

    _fill_in_data(data_expected);
    if (self_test == 0) {
        _test_mem_write(unit, (ci + wl), 0, data_expected);
    }

    /* Do one dummy memory read to fix RTL bug on RD_FIFO write pointer coming out of reset bug */
    data = _test_mem_read(unit, (ci + wl), 0, NULL);
    if(wl == 0) {
        READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, &data);
        init_rd_en_step0 =
            DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE0_BIT_RD_EN,
                            OVR_STEP);
        READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, &data);
        init_rd_en_step1 =
            DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE1_BIT_RD_EN,
                            OVR_STEP);
    } else {
        READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, &data);
        init_rd_en_step0 =
            DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE0_BIT_RD_EN,
                            OVR_STEP);
        READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, &data);
        init_rd_en_step1 =
            DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE1_BIT_RD_EN,
                            OVR_STEP);
    }
    for(vref = 16; vref < 49; vref++) {
        data = (vref << 6) | vref;
        MODIFY_DDR40_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, ci, data, 0xFFF);
        for (i = 0; i < 64; ++i) {

            /* RD_DQS */
            if (wl == 0) {
                data = SET_OVR_STEP(i);
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, data);
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_Nr(unit, ci, data);
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, data);
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, data);
                new_rd_en_step = init_rd_en_step0 + (i >> RD_EN_DQS_CORROLATION);
                (*vwsPtr)[vref].uncapped_rd_en_step[0] = new_rd_en_step;
                if(new_rd_en_step > 63) {
                    new_rd_en_step = 63;
                }
                new_rd_en_byte_step = new_rd_en_step;
                data = SET_OVR_STEP(new_rd_en_step);
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, data);
                new_rd_en_step = init_rd_en_step1 + (i >> RD_EN_DQS_CORROLATION);
                (*vwsPtr)[vref].uncapped_rd_en_step[1] = new_rd_en_step;
                if(new_rd_en_step > 63) {
                    new_rd_en_step = 63;
                }
                new_rd_en_byte_step += new_rd_en_step;
                data = SET_OVR_STEP(new_rd_en_step);
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, data);
                new_rd_en_byte_step = new_rd_en_byte_step >> 1;
                data = SET_OVR_STEP(new_rd_en_byte_step);
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
            } else {
                data = SET_OVR_STEP(i);
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, data);
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_Nr(unit, ci, data);
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, data);
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, data);
                new_rd_en_step = init_rd_en_step0 + (i >> RD_EN_DQS_CORROLATION);
                (*vwsPtr)[vref].uncapped_rd_en_step[0] = new_rd_en_step;
                if(new_rd_en_step > 63) {
                    new_rd_en_step = 63;
                }
                new_rd_en_byte_step = new_rd_en_step;
                data = SET_OVR_STEP(new_rd_en_step);
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, data);
                new_rd_en_step = init_rd_en_step1 + (i >> RD_EN_DQS_CORROLATION);
                (*vwsPtr)[vref].uncapped_rd_en_step[1] = new_rd_en_step;
                if(new_rd_en_step > 63) {
                    new_rd_en_step = 63;
                }
                new_rd_en_byte_step += new_rd_en_step;
                data = SET_OVR_STEP(new_rd_en_step);
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, data);
                new_rd_en_byte_step = new_rd_en_byte_step >> 1;
                data = SET_OVR_STEP(new_rd_en_byte_step);
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
            }

            /* Clear the FIFO error state */
            WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
            WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
            sal_usleep(9);
            if (self_test == 0) {
                (*vwsPtr)[vref].raw_result[i] = _test_mem_read(unit, (ci + wl), 0, data_expected);
            } else if (self_test == 2) {
                (*vwsPtr)[vref].raw_result[i] = _test_rd_data_fifo_status(unit, ci, wl, 1);
            } else {
                (*vwsPtr)[vref].raw_result[i] =
                    _test_func_self_test_1(unit, ci, wl, 1, 0);
            }
        }
    }

    /* Clear the FIFO error state */
    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
    return SOC_E_NONE;

}
                       
/* Make a shmoo with the write dq vdl values */
STATIC int _shmoo_wr_dq(int unit, int ci, int wl, int self_test, uint32 * result)
{
    int32 i;
    int32 byte, bit;
    uint32 data;
    uint32 data_expected[NUM_DATA];

    _fill_in_data(data_expected);
    if (self_test == 0) {
        _test_mem_write(unit, (ci + wl), 0, data_expected);
    }

    /* Do one dummy memory read to fix RTL bug on RD_FIFO write pointer coming out of reset bug */
    data = _test_mem_read(unit, (ci + wl), 0, NULL);
    for (i = 0; i < 64; ++i) {
        data = SET_OVR_STEP(i);

        /* WR_DQ */
        for (byte = 0; byte < 2; byte++) {
            for (bit = 0; bit < 8; bit++) {
                WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_Wr(wl, byte,
                                                                     bit, unit,
                                                                     ci, data);
            }
        }

        /* Clear the FIFO error state */
        WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
        WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
        sal_usleep(9);
        if (self_test==0) {
            result[i] = _test_mem_read(unit, (ci+wl), 0, data_expected); 
        } else if (self_test==2) {
            result[i] = _test_rd_data_fifo_status(unit, ci, wl, 1); 
        } else {
            result[i] =
                _test_func_self_test_1(unit, ci, wl, 1, 1);
        }
    }

    /* Clear the FIFO error state */
    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
    return SOC_E_NONE;
}

/* Make a shmoo with the Address */
STATIC int _shmoo_addr(int unit, int ci, int wl, int self_test, uint32 * result)
{
    uint32 data;
    uint32 setting, setting_idx;
    uint32 see_pass, see_fail_after_pass;
    uint32 start_vdl_val, pass_start, pass_end;
    uint32 use_byte_sel;
    
    pass_start = 64;            /* 64 means "have not found pass yet" */
    pass_end = 64;              /* 64 means "have not found pass yet" */
    start_vdl_val = 0;

    for(use_byte_sel = 0; use_byte_sel <= 1; use_byte_sel++) {
        /* Find pass_end of window       */
        see_pass = 0;
        see_fail_after_pass = 0;

        /* Do one dummy memory read to fix RTL bug on RD_FIFO write pointer coming out of reset bug */
        data = _test_mem_read(unit, (ci + wl), 0, NULL);
        for (setting = start_vdl_val; setting < 64; setting++) {
            data = SET_OVR_STEP(setting);
            DDR40_SET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_OVRIDE_BIT_CTL,
                            BYTE_SEL, use_byte_sel);
            WRITE_DDR40_PHY_CONTROL_REGS_VDL_OVRIDE_BIT_CTLr(unit, ci, data);
            data = SET_OVR_STEP(setting);
            DDR40_SET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_OVRIDE_BYTE_CTL,
                            BYTE_SEL, use_byte_sel);
            WRITE_DDR40_PHY_CONTROL_REGS_VDL_OVRIDE_BYTE_CTLr(unit, ci, data);
            if (setting == 0) {
                _soc_mem_reset_and_init_after_shmoo_addr(unit, ci);
            }

            /* Clear the FIFO error state */
            WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
            WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
            READ_DDR40_PHY_CONTROL_REGS_VDL_OVRIDE_BIT_CTLr(unit, ci, &data);
            sal_usleep(9);
            setting_idx = (64 * use_byte_sel) + setting;
            if (self_test == 0) {
                result[setting_idx] = _test_mem_wr_rd(unit, (ci + wl), 0);
            } else {
                result[setting_idx] =
                    _test_func_self_test_2(unit, ci, wl, 1);
            }
            if (see_pass == 0 && see_fail_after_pass == 0) {        /* First time and so far see no pass */
                if (result[setting_idx] == 0) {     /* result_fail = 0 means PASSED */
                    see_pass = 1;
                    pass_start = setting;
                }
                /* result_fail = 0 means PASSED */
            } else {
                if (see_fail_after_pass == 0) {     /* seeing FAIL for the first time */
                    if (result[setting_idx] > 0) {  /* seeing FAIL for the first time */
                        see_fail_after_pass = 1;
                        pass_end = setting - 1;
                    } else {
                        pass_end = setting;
                    }
                }
            }
            if(result[setting_idx] > 0) {
                _soc_mem_reset_and_init_after_shmoo_addr(unit, ci);
            }
        }
        
        setting_idx = 128 + (2 * use_byte_sel);
        result[setting_idx] = pass_start;
        setting_idx = 128 + (2 * use_byte_sel) + 1;
        result[setting_idx] = pass_end;
    }
    
    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0); 
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0); 
    
    return SOC_E_NONE;
}

/* Make a shmoo with the write data mask vdl values */
STATIC int _shmoo_wr_dm(int unit, int ci, int wl, uint32 * result)
{
    LOG_CLI((BSL_META_U(unit,
                        "   ------------------------------------------------------\n")));
    LOG_CLI((BSL_META_U(unit,
                        "     THIS SHMOO WR_DM feature is NOT SUPPORTED ........  \n")));
    LOG_CLI((BSL_META_U(unit,
                        "   ------------------------------------------------------\n")));
    return SOC_E_NONE;
}

STATIC int _calib_all(int unit, int ci, uint32 *result, uint32 size, vref_word_shmoo *vwsPtr, int flag, uint32 *Xpos, int type)
{
    int i, bit, vref, byte, bit_loc;
    int passStart;
    int failStart;
    int passLength;
    int maxPassStart;
    int maxPassLength;
    int maxMidPoint;
    int calib_steps;
    uint32 data;
    int ninty_deg_steps, rd_dq_steps, Tfix_steps;
    int mPLover;
    int vrefCap;
    
    vrefCap = (type == SHMOO_RD_DQ) ? 49 : 1;
    vref = (type == SHMOO_RD_DQ) ? 16 : 0;
    
    for(; vref < vrefCap; vref++) {
        for(bit = 0; bit < 16; bit++) {
            passStart = -1;
            failStart = -1;
            passLength = -1;
            maxPassStart = -2;
            maxPassLength = -2;
            maxMidPoint = -2;
            for(i = 0; i < size; i++) {
                if((result[i] & (0x1 << bit)) == 0) {
                    (*vwsPtr)[vref].bs[bit].result[i] = '+';
                    if(passStart < 0) {
                        passStart = i;
                        passLength = 1;
                        failStart = -1;
                    } else {
                        passLength++;
                    }
                    if(i == size - 1) {
                        if(maxPassLength < passLength) {
                            maxPassStart = passStart;
                            maxPassLength = passLength;
                        }
                    }
                } else {
                    (*vwsPtr)[vref].bs[bit].result[i] = '-';
                    if(failStart < 0) {
                        failStart = i;
                        if(maxPassLength < passLength) {
                            maxPassStart = passStart;
                            maxPassLength = passLength;
                        }
                        passStart = -1;
                        passLength = -1;
                    }
                }
            }
            (*vwsPtr)[vref].bs[bit].result[size] = 0;
            /* SINGLE CALIBRATION */
            switch(type) {
                case SHMOO_ADDRC:
                    WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit, ci, 0x0);
                    WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit, ci, 0x200);
                    data = 0;
                    calib_steps = 0;
                    READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit, ci, &data);
                    calib_steps =
                        DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS,
                            CALIB_TOTAL);
                    READ_DDR40_PHY_CONTROL_REGS_VDL_DQ_CALIB_STATUSr(unit, ci, &data);
                    rd_dq_steps =
                        DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_DQ_CALIB_STATUS,
                            DQ_CALIB_TOTAL) >> 4;
                    ninty_deg_steps = calib_steps >> 2;
                    Tfix_steps = ninty_deg_steps - rd_dq_steps;
                    if((maxPassStart + maxPassLength) > 64) {
                        mPLover = maxPassStart + maxPassLength - 64;
                        maxPassLength = (3 * (mPLover - 1)) + (Tfix_steps << 1) - maxPassStart + 1;
                        maxMidPoint = (maxPassStart + maxPassStart + maxPassLength) >> 1;
                        if(maxMidPoint > 63) {
                            maxMidPoint = 63;
                        }
                    } else {
                        if((result[63] & (0x1 << bit)) == 0) {
                            i = 21 - ((Tfix_steps << 1) / (3 * rd_dq_steps));
                            /* X = 63/3 - 2Tfix / 3Tstep */
                            if(i < 0) {
                                i = 64;
                            } else {
                                i += 64;
                            }
                            for(; i < size; i++) {
                                if((result[i] & (0x1 << bit)) != 0) {
                                    maxPassLength += i - 64;
                                    break;
                                }
                            }
                            mPLover = maxPassStart + maxPassLength - 64;
                            maxPassLength = (3 * (mPLover - 1)) + (Tfix_steps << 1) - maxPassStart + 1;
                            maxMidPoint = (maxPassStart + maxPassStart + maxPassLength) >> 1;
                            if(maxMidPoint > 63) {
                                maxMidPoint = 63;
                            }
                        } else {
                            maxMidPoint = (maxPassStart + maxPassStart + maxPassLength) >> 1;
                        }
                    }
                    break;
                default:
                    maxMidPoint = (maxPassStart + maxPassStart + maxPassLength) >> 1;
                    break;
            }
            if(type != SHMOO_WR_DQ) {
                if(maxMidPoint != -2) {
                    (*vwsPtr)[vref].bs[bit].result[maxMidPoint] = 'X';
                }
                (*vwsPtr)[vref].step[bit] = maxMidPoint;
                Xpos[bit] = maxMidPoint;
            }
        }
        if(type == SHMOO_WR_DQ) {
            for(byte = 0; byte < 2; byte++) {
                passStart = -1;
                failStart = -1;
                passLength = -1;
                maxPassStart = -2;
                maxPassLength = -20;
                maxMidPoint = -2;
                for(i = 0; i < size; i++) {
                    if(((result[i] >> (byte << 3)) & 0xFF) == 0) {
                        if(passStart < 0) {
                            passStart = i;
                            passLength = 1;
                            failStart = -1;
                        } else {
                            passLength++;
                        }
                        if(i == size - 1) {
                            if(maxPassLength < (passLength - 7)) {
                                maxPassStart = passStart;
                                maxPassLength = passLength;
                            }
                        }
                    } else {
                        if(failStart < 0) {
                            failStart = i;
                            if(maxPassLength < (passLength - 7)) {
                                maxPassStart = passStart;
                                maxPassLength = passLength;
                            }
                            passStart = -1;
                            passLength = -1;
                        }
                    }
                }
                maxMidPoint = (maxPassStart + maxPassStart + maxPassLength) >> 1;
                for(bit = 0; bit < 8; bit++) {
                    bit_loc = (byte << 3) + bit;
                    if((maxMidPoint >= 0) && (maxMidPoint <= 63)) {
                        (*vwsPtr)[vref].bs[bit_loc].result[maxMidPoint] = 'X';
                    }
                    (*vwsPtr)[vref].step[bit_loc] = maxMidPoint;
                    Xpos[bit_loc] = maxMidPoint;
                }
            }
        }
    }

    return SOC_E_NONE;
}

STATIC int _calib_all2(int unit, int ci, int wl, uint32 size, vref_word_shmoo *vwsPtr, int flag, uint32 *Xpos, int type)
{
    int i, bit, vref, byte, bit_loc;
    int passStart;
    int failStart;
    int passLength;
    int maxPassStart;
    int maxPassLength;
    int maxMidPoint;
    uint32 data;
    int vrefAvg;
    int overallmPL0 = -3;
    int overallmPL1 = -3;
    int overallmPLiter0 = -3;
    int overallmPLiter1 = -3;
    int maxMidPoint0 = -4;
    int maxMidPoint1 = -4;
    
    for(vref = 16; vref < 49; vref++) {
        for(bit = 0; bit < 16; bit++) {
            passStart = -1;
            failStart = -1;
            passLength = -1;
            maxPassStart = -2;
            maxPassLength = -2;
            maxMidPoint = -2;
            for(i = 0; i < size; i++) {
                if(((*vwsPtr)[vref].raw_result[i] & (0x1 << bit)) == 0) {
                    (*vwsPtr)[vref].bs[bit].result[i] = '+';
                    if(passStart < 0) {
                        passStart = i;
                        passLength = 1;
                        failStart = -1;
                    } else {
                        passLength++;
                    }
                    if(i == size - 1) {
                        if(maxPassLength < passLength) {
                            maxPassStart = passStart;
                            maxPassLength = passLength;
                        }
                    }
                } else {
                    (*vwsPtr)[vref].bs[bit].result[i] = '-';
                    if(failStart < 0) {
                        failStart = i;
                        if(maxPassLength < passLength) {
                            maxPassStart = passStart;
                            maxPassLength = passLength;
                        }
                        passStart = -1;
                        passLength = -1;
                    }
                }
            }
            (*vwsPtr)[vref].bs[bit].result[size] = 0;
            /* SINGLE CALIBRATION */
            maxMidPoint = (maxPassStart + maxPassStart + maxPassLength) >> 1;
            (*vwsPtr)[vref].step[bit] = maxMidPoint;
            Xpos[bit] = maxMidPoint;
        }
        for(byte = 0; byte < 2; byte++) {
            passStart = -1;
            failStart = -1;
            passLength = -1;
            maxPassStart = -2;
            maxPassLength = -2;
            maxMidPoint = -2;
            for(i = 0; i < size; i++) {
                if((((*vwsPtr)[vref].raw_result[i] >> (byte << 3)) & 0xFF) == 0) {
                    (*vwsPtr)[vref].bytes[byte].result[i] = '+';
                    if(passStart < 0) {
                        passStart = i;
                        passLength = 1;
                        failStart = -1;
                    } else {
                        passLength++;
                    }
                    if(i == size - 1) {
                        if(maxPassLength < passLength) {
                            maxPassStart = passStart;
                            maxPassLength = passLength;
                        }
                    }
                } else {
                    (*vwsPtr)[vref].bytes[byte].result[i] = '-';
                    if(failStart < 0) {
                        failStart = i;
                        if(maxPassLength < passLength) {
                            maxPassStart = passStart;
                            maxPassLength = passLength;
                        }
                        passStart = -1;
                        passLength = -1;
                    }
                }
            }
            maxMidPoint = (maxPassStart + maxPassStart + maxPassLength) >> 1;
            for(bit = 0; bit < 8; bit++) {
                bit_loc = (byte << 3) + bit;
                if((maxMidPoint >= 0) && (maxMidPoint <= 63)) {
                    (*vwsPtr)[vref].bs[bit_loc].result[maxMidPoint] = 'X';
                }
                (*vwsPtr)[vref].step[bit_loc] = maxMidPoint;
            }
            (*vwsPtr)[vref].bytes[byte].result[maxMidPoint] = 'X';
            (*vwsPtr)[vref].bytes[byte].result[size] = 0;
            if(byte) {
                if(overallmPL1 < maxPassLength) {
                    overallmPL1 = maxPassLength;
                    overallmPLiter1 = vref;
                    maxMidPoint1 = maxMidPoint;
                }
            } else {
                if(overallmPL0 < maxPassLength) {
                    overallmPL0 = maxPassLength;
                    overallmPLiter0 = vref;
                    maxMidPoint0 = maxMidPoint;
                }
            }
        }
    }
    
    for(bit = 0; bit < 8; bit++) {
        Xpos[bit] = maxMidPoint0;
    }
    for(bit = 8; bit < 16; bit++) {
        Xpos[bit] = maxMidPoint1;
    }
    
    vrefAvg = (overallmPLiter0 + overallmPLiter1) >> 1;
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "Switching to Vref Step           : %2d\n"),
              vrefAvg));
    data = (vrefAvg << 6) | vrefAvg;
    MODIFY_DDR40_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, ci, data, 0xFFF);
    
    return SOC_E_NONE;
}

STATIC int _calib_all3(int unit, int ci, int wl, uint32 size, vref_word_shmoo *vwsPtr, int flag, uint32 *Xpos, int type)
{
    int i, bit, vref;
    int passStart;
    int failStart;
    int passLength;
    int maxPassStart;
    int maxPassLength;
    int maxMidPoint;
    uint32 data;
    int overallmPL0 = 0;
    int overallmPL1 = 0;
    int overallmPLiter0 = 0;
    int overallmPLiter1 = 0;
    int maxMidPoint0 = 0;
    int maxMidPoint1 = 0;
    int yMidPoint0 = 0;
    int yMidPoint1 = 0;
    
    for(vref = 0; vref < 64; vref++) {
        for(bit = 0; bit < 16; bit++) {
            passStart = -1;
            failStart = -1;
            passLength = -1;
            maxPassStart = -2;
            maxPassLength = -2;
            maxMidPoint = -2;
            for(i = 0; i < size; i++) {
                if(((*vwsPtr)[vref].raw_result[i] & (0x1 << bit)) == 0) {
                    (*vwsPtr)[vref].bs[bit].result[i] = '+';
                    if(passStart < 0) {
                        passStart = i;
                        passLength = 1;
                        failStart = -1;
                    } else {
                        passLength++;
                    }
                    if(i == size - 1) {
                        if(maxPassLength < passLength) {
                            maxPassStart = passStart;
                            maxPassLength = passLength;
                        }
                    }
                } else {
                    (*vwsPtr)[vref].bs[bit].result[i] = '-';
                    if(failStart < 0) {
                        failStart = i;
                        if(maxPassLength < passLength) {
                            maxPassStart = passStart;
                            maxPassLength = passLength;
                        }
                        passStart = -1;
                        passLength = -1;
                    }
                }
            }
            (*vwsPtr)[vref].bs[bit].result[size] = 0;
            /* SINGLE CALIBRATION */
            maxMidPoint = (maxPassStart + maxPassStart + maxPassLength) >> 1;
            if(maxMidPoint != -2) {
                (*vwsPtr)[vref].bs[bit].result[maxMidPoint] = 'X';
            }
            (*vwsPtr)[vref].step[bit] = maxMidPoint;
            if(bit >> 3) {
                if(maxPassLength >= 5) {
                    overallmPL1++;
                    overallmPLiter1 += vref;
                }
            } else {
                if(maxPassLength >= 5) {
                    overallmPL0++;
                    overallmPLiter0 += vref;
                }
            }
        }
    }

    if(overallmPL0 != 0) {
        yMidPoint0 = overallmPLiter0 / overallmPL0;
        for(bit = 0; bit < 8; bit++) {
            maxMidPoint0 += (*vwsPtr)[yMidPoint0].step[bit];
        }
        maxMidPoint0 = maxMidPoint0 >> 3;
    }
    if(overallmPL1 != 0) {
        yMidPoint1 = overallmPLiter1 / overallmPL1;
        for(bit = 8; bit < 16; bit++) {
            maxMidPoint1 += (*vwsPtr)[yMidPoint1].step[bit];
        }
        maxMidPoint1 = maxMidPoint1 >> 3;
    }
    
    for(bit = 0; bit < 8; bit++) {
        Xpos[bit] = maxMidPoint0;
    }
    for(bit = 8; bit < 16; bit++) {
        Xpos[bit] = maxMidPoint1;
    }

    /* RD_DQ */
    data = SET_OVR_STEP(yMidPoint0);
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "Switching to RD_DQ Step (Byte 0) : %2d\n"),
              yMidPoint0));
    for (bit = 0; bit < 8; bit++) {
        WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Pr(wl,
                                                               0,
                                                               bit,
                                                               unit,
                                                               ci,
                                                               data);
        WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Nr(wl,
                                                               0,
                                                               bit,
                                                               unit,
                                                               ci,
                                                               data);
    }
    data = SET_OVR_STEP(yMidPoint1);
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "Switching to RD_DQ Step (Byte 1) : %2d\n"),
              yMidPoint1));
    for (bit = 0; bit < 8; bit++) {
        WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Pr(wl,
                                                               1,
                                                               bit,
                                                               unit,
                                                               ci,
                                                               data);
        WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Nr(wl,
                                                               1,
                                                               bit,
                                                               unit,
                                                               ci,
                                                               data);
    }
    
    return SOC_E_NONE;
}

/* Get Init steps                                                                                                           */
int
_soc_ddr40_shmoo_get_init_step(int unit, int ci, soc_ddr_shmoo_param_t *sp)
{
    uint32 data = 0;
    uint32 calib_total;
    int byte, bit, en, n;

    switch (sp->type) {
    case SHMOO_RD_EN:
        READ_DDR40_PHY_CONTROL_REGS_VDL_RD_EN_CALIB_STATUSr(unit, ci, &data);
        calib_total =
            DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
                            VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_TOTAL) >> 4;
        if (sp->wl == 0) {
            READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, &data);    /* Byte 0 */
            sp->init_step[0] =
                (DDR40_GET_FIELD
                 (data, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE0_BIT_RD_EN,
                  OVR_EN)) ? DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0,
                                             VDL_OVRIDE_BYTE0_BIT_RD_EN,
                                             OVR_STEP) : calib_total;
            READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, &data);    /* Byte 1 */
            sp->init_step[1] =
                (DDR40_GET_FIELD
                 (data, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE1_BIT_RD_EN,
                  OVR_EN)) ? DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0,
                                             VDL_OVRIDE_BYTE1_BIT_RD_EN,
                                             OVR_STEP) : calib_total;
        } else {
            READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, &data);    /* Byte 0 */
            sp->init_step[0] =
                (DDR40_GET_FIELD
                 (data, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE0_BIT_RD_EN,
                  OVR_EN)) ? DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1,
                                             VDL_OVRIDE_BYTE0_BIT_RD_EN,
                                             OVR_STEP) : calib_total;
            READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, &data);    /* Byte 1 */
            sp->init_step[1] =
                (DDR40_GET_FIELD
                 (data, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE1_BIT_RD_EN,
                  OVR_EN)) ? DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1,
                                             VDL_OVRIDE_BYTE1_BIT_RD_EN,
                                             OVR_STEP) : calib_total;
        }
        if (sp->wl == 0) {
            READ_DDR40_PHY_WORD_LANE_0_READ_DATA_DLYr(unit, ci, &data);
            sp->init_step[2] = data;
        } else {
            READ_DDR40_PHY_WORD_LANE_1_READ_DATA_DLYr(unit, ci, &data);
            sp->init_step[2] = data;
        }
        break;
    case SHMOO_RD_DQ:
        READ_DDR40_PHY_CONTROL_REGS_VDL_DQ_CALIB_STATUSr(unit, ci, &data);
        calib_total =
            DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_DQ_CALIB_STATUS,
                            DQ_CALIB_TOTAL) >> 4;
        n = 0;
        for (byte = 0; byte < 2; byte++) {
            for (bit = 0; bit < 8; bit++) {
                READ_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Pr(sp->wl, byte,
                                                                      bit,
                                                                      unit, ci,
                                                                      &data);
                en = DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0,
                                     VDL_OVRIDE_BYTE0_BIT0_R_P, OVR_EN);
                sp->init_step[n] =
                    (en) ? GET_FIELD(data, DDR40_PHY_WORD_LANE_0,
                                     VDL_OVRIDE_BYTE0_BIT0_R_P,
                                     OVR_STEP) : calib_total;
                n++;
            }
        }
        break;
    case SHMOO_WR_DQ:
        READ_DDR40_PHY_CONTROL_REGS_VDL_WR_CHAN_CALIB_STATUSr(unit, ci, &data);
        calib_total =
            DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
                            VDL_WR_CHAN_CALIB_STATUS,
                            WR_CHAN_CALIB_TOTAL) >> 4;
        n = 0;
        for (byte = 0; byte < 2; byte++) {
            for (bit = 0; bit < 8; bit++) {
                READ_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_Wr(sp->wl, byte,
                                                                    bit, unit,
                                                                    ci, &data);
                en = DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0,
                                     VDL_OVRIDE_BYTE0_BIT0_W, OVR_EN);
                sp->init_step[n] =
                    (en) ? DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0,
                                           VDL_OVRIDE_BYTE0_BIT0_W,
                                           OVR_STEP) : calib_total;
                n++;
            }
        }
        break;
    case SHMOO_ADDRC:
        break;
    case SHMOO_WR_DM:
        READ_DDR40_PHY_CONTROL_REGS_VDL_WR_CHAN_CALIB_STATUSr(unit, ci, &data);
        calib_total =
            DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
                            VDL_WR_CHAN_CALIB_STATUS,
                            WR_CHAN_CALIB_TOTAL) >> 4;
        if (sp->wl == 0) {
            READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_DM_Wr(unit, ci, &data);
            sp->init_step[0] =
                (DDR40_GET_FIELD
                 (data, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE0_DM_W,
                  OVR_EN)) ? DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0,
                                             VDL_OVRIDE_BYTE0_DM_W,
                                             OVR_STEP) : calib_total;
            READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_DM_Wr(unit, ci, &data);
            sp->init_step[1] =
                (DDR40_GET_FIELD
                 (data, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE1_DM_W,
                  OVR_EN)) ? DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0,
                                             VDL_OVRIDE_BYTE1_DM_W,
                                             OVR_STEP) : calib_total;
        } else {
            READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_DM_Wr(unit, ci, &data);
            sp->init_step[0] =
                (DDR40_GET_FIELD
                 (data, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE0_DM_W,
                  OVR_EN)) ? DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1,
                                             VDL_OVRIDE_BYTE0_DM_W,
                                             OVR_STEP) : calib_total;
            READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_DM_Wr(unit, ci, &data);
            sp->init_step[1] =
                (DDR40_GET_FIELD
                 (data, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE1_DM_W,
                  OVR_EN)) ? DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1,
                                             VDL_OVRIDE_BYTE1_DM_W,
                                             OVR_STEP) : calib_total;
        }
        break;
    default:
        break;
    }
    return SOC_E_NONE;
}

int
_soc_ddr40_shmoo_do(int unit, int ci, soc_ddr_shmoo_param_t *sp)
{
    switch (sp->type) {
    case SHMOO_INIT_VDL_RESULT:
        return _shmoo_init_vdl_result(unit, ci, sp->wl, 1, sp->result);
    case SHMOO_RD_EN:
        _shmoo_rd_data_dly(unit, ci, sp->wl, 1, sp->result);
        return _shmoo_rd_en_rd_dq(unit, ci, sp->wl, 1, sp->vwsPtr);
    case SHMOO_RD_DQ:
        _shmoo_rd_dq(unit, ci, sp->wl);
        return _shmoo_rd_dqs(unit, ci, sp->wl, 1, sp->vwsPtr);
    case SHMOO_WR_DQ:
        return _shmoo_wr_dq(unit, ci, sp->wl, 1, sp->result);
    case SHMOO_ADDRC:
        return _shmoo_addr(unit, ci, sp->wl, 1, sp->result);
    case SHMOO_WR_DM:
        return _shmoo_wr_dm(unit, ci, sp->wl, sp->result);
    }
    return SOC_E_PARAM;
}

int
_soc_ddr40_shmoo_calib(int unit, int ci, soc_ddr_shmoo_param_t *sp)
{
    switch (sp->type) {
    case SHMOO_INIT_VDL_RESULT:
        return SOC_E_NONE;
    case SHMOO_RD_EN:
        return _calib_all3(unit, ci, sp->wl, 64, sp->vwsPtr, 0, sp->new_step, sp->type);
    case SHMOO_RD_DQ:
        return _calib_all2(unit, ci, sp->wl, 64, sp->vwsPtr, 0, sp->new_step, sp->type);
    case SHMOO_WR_DQ:
        return _calib_all(unit, ci, sp->result, 64, sp->vwsPtr, 0, sp->new_step, sp->type);
    case SHMOO_ADDRC:
        return _calib_all(unit, ci, sp->result, 128, sp->vwsPtr, 0, sp->new_step, sp->type);
    }
    return SOC_E_PARAM;
}

int _soc_ddr40_shmoo_plot(soc_ddr_shmoo_param_t *sp)
{
    int i;
    int vref, vref_count;
    char str[65];
    vref_word_shmoo *vwsPtr = sp->vwsPtr;

    str[64] = 0;

    LOG_CLI((BSL_META("\n")));

    switch (sp->type) {
    case SHMOO_INIT_VDL_RESULT:
        return SOC_E_NONE;
        break;
    case SHMOO_RD_EN:
        LOG_CLI((BSL_META("**** RD_EN  Shmoo (WL=%d)\n"), sp->wl));
        break;
    case SHMOO_RD_DQ:
        LOG_CLI((BSL_META("**** RD_DQS  Shmoo (WL=%d)\n"), sp->wl));
        break;
    case SHMOO_WR_DQ:
        LOG_CLI((BSL_META("**** WR_DQ  Shmoo (WL=%d)\n"), sp->wl));
        break;
    case SHMOO_ADDRC:
        LOG_CLI((BSL_META("**** ADDRC  Shmoo\n")));
        /*LOG_CLI((BSL_META("**** ADDRC  Shmoo (WL=%d)\n"), sp->wl));*/
        break;
    default:
        LOG_CLI((BSL_META("**** ?????  Shmoo (WL=%d)\n"), sp->wl));
        break;
    }
    LOG_CLI((BSL_META("\n")));
    
    if(sp->type == SHMOO_WR_DQ) {
        for (i = 0; i < 64; i++) {
            str[i] = '0' + (i/10);
        }
        LOG_CLI((BSL_META("    %s\n"), str));

        for ( i = 0; i < 64; i++ ) {
            str[i] = '0' + (i%10);
        }
        LOG_CLI((BSL_META("    %s\n"), str));

        vref_count = 16;
        
        for (vref = 0; vref < vref_count; vref++) {
            LOG_CLI((BSL_META(" %02d %s\n"), vref, (*vwsPtr)[0].bs[vref].result));
        }
        
        LOG_CLI((BSL_META("\n")));
        return SOC_E_NONE;
    }


    if (sp->type == SHMOO_ADDRC) {
        LOG_CLI((BSL_META("\n BYTE_SEL = 0  \n")));
    } else {
        LOG_CLI((BSL_META("\n BYTE = 0  \n")));
    }

    for (i = 0; i < 64; i++) {
        str[i] = '0' + (i/10);
    }
    LOG_CLI((BSL_META("    %s\n"), str));

    for ( i = 0; i < 64; i++ ) {
        str[i] = '0' + (i%10);
    }
    LOG_CLI((BSL_META("    %s\n"), str));

    vref_count = (sp->type == SHMOO_RD_DQ) ? 49 : 1;
    vref = (sp->type == SHMOO_RD_DQ) ? 16 : 0;
    
    vref_count = (sp->type == SHMOO_RD_EN) ? 64 : vref_count;

    for (; vref < vref_count; vref++) {
        if(sp->type == SHMOO_RD_DQ) {
            LOG_CLI((BSL_META(" %02d %s\n"), vref, (*vwsPtr)[vref].bytes[0].result));
        /*    for(i = 0; i < 8; i++) {
                LOG_CLI((BSL_META(" %02d-%02d %s\n"), vref, i, (*vwsPtr)[vref].bs[i].result));
            } */
        } else if(sp->type == SHMOO_RD_EN) {
            LOG_CLI((BSL_META(" %02d %s\n"), vref, (*vwsPtr)[vref].bs[0].result));
        } else if(sp->type == SHMOO_WR_DQ) {
            LOG_CLI((BSL_META(" %02d %s\n"), vref, (*vwsPtr)[vref].bs[0].result));
        } else if((sp->type == SHMOO_ADDRC)) {
            LOG_CLI((BSL_META(" %02d "), vref));
            for(i = 0; i < 64; i++) {
                LOG_CLI((BSL_META("%c"), (*vwsPtr)[vref].bs[0].result[i]));
            }
            LOG_CLI((BSL_META("\n")));
        } else {
            LOG_CLI((BSL_META("UNKNOWN SHMOO\n")));
            return -1;
        }
    }
    LOG_CLI((BSL_META("\n")));

    if (sp->type == SHMOO_ADDRC) {
        LOG_CLI((BSL_META("\n BYTE_SEL = 1  \n")));
    } else {
        LOG_CLI((BSL_META("\n BYTE = 1  \n")));
    }

    for (i = 0; i < 64; i++)
        str[i] = '0' + (i / 10);
    LOG_CLI((BSL_META("    %s\n"), str));

    for (i = 0; i < 64; i++)
        str[i] = '0' + (i % 10);
    LOG_CLI((BSL_META("    %s\n"), str));
    
    vref = (sp->type == SHMOO_RD_DQ) ? 16 : 0;
        
    for (; vref < vref_count; vref++) {
        if(sp->type == SHMOO_RD_DQ) {
            LOG_CLI((BSL_META(" %02d %s\n"), vref, (*vwsPtr)[vref].bytes[1].result));
        /*    for(i = 8; i < 16; i++) {
                LOG_CLI((BSL_META(" %02d-%02d %s\n"), vref, i, (*vwsPtr)[vref].bs[i].result));
            } */
        } else if(sp->type == SHMOO_RD_EN) {
            LOG_CLI((BSL_META(" %02d %s\n"), vref, (*vwsPtr)[vref].bs[8].result));
        } else if(sp->type == SHMOO_WR_DQ) {
            LOG_CLI((BSL_META(" %02d %s\n"), vref, (*vwsPtr)[vref].bs[8].result));
        } else if((sp->type == SHMOO_ADDRC)) {
            LOG_CLI((BSL_META(" %02d "), vref));
            for(i = 64; i < 128; i++) {
                LOG_CLI((BSL_META("%c"), (*vwsPtr)[vref].bs[0].result[i]));
            }
            LOG_CLI((BSL_META("\n")));
        } else {
            LOG_CLI((BSL_META("UNKNOWN SHMOO\n")));
            return -1;
        }
    }
    LOG_CLI((BSL_META("\n")));

    return SOC_E_NONE;
}

int
_soc_ddr40_shmoo_set_new_step(int unit, int ci, soc_ddr_shmoo_param_t *sp)
{
    uint32 data = 0;
    int byte, bit, n;
    int indx;
    uint32 init_rd_dqs_step;
    uint32 init_rd_en_step;
    uint32 new_rd_dqs_step;
    uint32 new_rd_en_step;
    uint32 vref_pick;
    uint32 new_rd_en_avg_step;
    vref_word_shmoo *vwsPtr = sp->vwsPtr;

    switch (sp->type) {
    case SHMOO_RD_EN:
        if (sp->wl == 0) {
            n = sp->new_step[0];
            data = SET_OVR_STEP(n);
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci,
                                                                    data);
            n = sp->new_step[8];
            data = SET_OVR_STEP(n);
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci,
                                                                    data);
            n = (sp->new_step[0] + sp->new_step[8]) >> 1;
            data = SET_OVR_STEP(n);
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
        } else {
            n = sp->new_step[0];
            data = SET_OVR_STEP(n);
            WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci,
                                                                    data);
            n = sp->new_step[8];
            data = SET_OVR_STEP(n);
            WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci,
                                                                    data);
            n = (sp->new_step[0] + sp->new_step[8]) >> 1;
            data = SET_OVR_STEP(n);
            WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
        }
        break;
    case SHMOO_RD_DQ:
        READ_DDR40_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, ci, &data);
        vref_pick =
                DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VREF_DAC_CONTROL,
                                DAC0);
        if (sp->wl == 0) {
            READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, &data);
            init_rd_dqs_step =
                DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE0_R_P,
                                OVR_STEP);
            init_rd_en_step = (*vwsPtr)[vref_pick].uncapped_rd_en_step[0];
            new_rd_dqs_step = sp->new_step[0];
            new_rd_en_step = init_rd_en_step - ((init_rd_dqs_step - new_rd_dqs_step) >> RD_EN_DQS_CORROLATION);
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Switching to RD_DQS Step (Byte 0): %2d\n"),
                      new_rd_dqs_step));
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Switching to RD_EN Step  (Byte 0): %2d\n"),
                      new_rd_en_step));
            data = SET_OVR_STEP(new_rd_dqs_step);
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, data);
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_Nr(unit, ci, data);
            data = SET_OVR_STEP(new_rd_en_step);
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, data);
            READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, &data);
            new_rd_en_avg_step = new_rd_en_step;
            init_rd_dqs_step =
                DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE1_R_P,
                                OVR_STEP);
            init_rd_en_step = (*vwsPtr)[vref_pick].uncapped_rd_en_step[1];
            new_rd_dqs_step = sp->new_step[8];
            new_rd_en_step = init_rd_en_step - ((init_rd_dqs_step - new_rd_dqs_step) >> RD_EN_DQS_CORROLATION);
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Switching to RD_DQS Step (Byte 1): %2d\n"),
                      new_rd_dqs_step));
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Switching to RD_EN Step  (Byte 1): %2d\n"),
                      new_rd_en_step));
            data = SET_OVR_STEP(new_rd_dqs_step);
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, data);
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, data);
            data = SET_OVR_STEP(new_rd_en_step);
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, data);
            new_rd_en_avg_step = (new_rd_en_avg_step + new_rd_en_step) >> 1;
            data = SET_OVR_STEP(new_rd_en_avg_step);
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
        } else {
            READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, &data);
            init_rd_dqs_step =
                DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE0_R_P,
                                OVR_STEP);
            init_rd_en_step = (*vwsPtr)[vref_pick].uncapped_rd_en_step[0];
            new_rd_dqs_step = sp->new_step[0];
            new_rd_en_step = init_rd_en_step - ((init_rd_dqs_step - new_rd_dqs_step) >> RD_EN_DQS_CORROLATION); 
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Switching to RD_DQS Step (Byte 0): %2d\n"),
                      new_rd_dqs_step));
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Switching to RD_EN Step  (Byte 0): %2d\n"),
                      new_rd_en_step));
            data = SET_OVR_STEP(new_rd_dqs_step);
            WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, data);
            WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_Nr(unit, ci, data);
            data = SET_OVR_STEP(new_rd_en_step);
            WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, data);
            READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, &data);
            new_rd_en_avg_step = new_rd_en_step;
            init_rd_dqs_step =
                DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE1_R_P,
                                OVR_STEP);
            init_rd_en_step = (*vwsPtr)[vref_pick].uncapped_rd_en_step[1];
            new_rd_dqs_step = sp->new_step[8];
            new_rd_en_step = init_rd_en_step - ((init_rd_dqs_step - new_rd_dqs_step) >> RD_EN_DQS_CORROLATION); 
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Switching to RD_DQS Step (Byte 1): %2d\n"),
                      new_rd_dqs_step));
            LOG_INFO(BSL_LS_SOC_DDR,
                     (BSL_META_U(unit,
                                 "Switching to RD_EN Step  (Byte 1): %2d\n"),
                      new_rd_en_step));
            data = SET_OVR_STEP(new_rd_dqs_step);
            WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, data);
            WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, data);
            data = SET_OVR_STEP(new_rd_en_step);
            WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, data);
            new_rd_en_avg_step = (new_rd_en_avg_step + new_rd_en_step) >> 1;
            data = SET_OVR_STEP(new_rd_en_avg_step);
            WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
        }
        break;
    case SHMOO_WR_DQ:
        n = 0;
        for (byte = 0; byte < 2; byte++) {
            for (bit = 0; bit < 8; bit++) {
                data = SET_OVR_STEP(sp->new_step[n]);
                indx = (sp->wl*4) + (byte*2) + (bit/4);
                COMPILER_REFERENCE(indx);
                WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_Wr(sp->wl, byte,
                                                                     bit, unit,
                                                                     ci, data);
                n++;
            }
        }
        break;
    case SHMOO_ADDRC:
        n = 0;
        if (sp->wl == 0) {
            data = SET_OVR_STEP(sp->new_step[n]);
            DDR40_SET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_OVRIDE_BIT_CTL,
                            BYTE_SEL, 0);
            WRITE_DDR40_PHY_CONTROL_REGS_VDL_OVRIDE_BIT_CTLr(unit, ci, data);
            DDR40_SET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_OVRIDE_BYTE_CTL,
                            BYTE_SEL, 0);
            WRITE_DDR40_PHY_CONTROL_REGS_VDL_OVRIDE_BYTE_CTLr(unit, ci, 0);
            sal_usleep(2000);
            _soc_mem_reset_and_init_after_shmoo_addr(unit, ci);
            n = _test_func_self_test_2(unit, ci, 0, 1);
        }
        break;
    default:
        break;
    }
    return SOC_E_NONE;
}

/***********************************************************************
* Functions to schmoo the DDR vdl/voltage etc.
 */
int
soc_ddr40_shmoo_ctl(int unit, int ci, uint32 phyType, uint32 ctlType, int stat, int isplot)
{
    soc_ddr_shmoo_param_t *sp;
    int ciC;
    uint32 error_mask0, error_mask1;
    uint32 rval;
    
    ciC = ci & 0xFFFFFFFE;

    if (!stat) {
        switch(ctlType) {
            case DDR_CTLR_TRSVP:
                break;
            case DDR_CTLR_T0:
                
                rval = 0;
                soc_reg_field_set(unit, CI_ERROR_MASKr, &rval, WFIFO_CTL_CORRECTED_ERROR_DISINTf, 1);
                soc_reg_field_set(unit, CI_ERROR_MASKr, &rval, WFIFO_CTL_UNCORRECTED_ERROR_DISINTf, 1);
                soc_reg_field_set(unit, CI_ERROR_MASKr, &rval, RFIFO_CTL_CORRECTED_ERROR_DISINTf, 1);
                soc_reg_field_set(unit, CI_ERROR_MASKr, &rval, RFIFO_CTL_UNCORRECTED_ERROR_DISINTf, 1);
                soc_reg_field_set(unit, CI_ERROR_MASKr, &rval, PHY_BL0_RD_FIFO_ERRORf, 1);
                soc_reg_field_set(unit, CI_ERROR_MASKr, &rval, PHY_BL1_RD_FIFO_ERRORf, 1);
                soc_reg_field_set(unit, CI_ERROR_MASKr, &rval, RFIFO_OVERFLOW_DISINTf, 1);
                soc_reg_field_set(unit, CI_ERROR_MASKr, &rval, WB_OVERFLOW_DISINTf, 1);
                soc_reg_field_set(unit, CI_ERROR_MASKr, &rval, PHY_READYf, 1);
                if(_check_dram(ciC)) {
                    SOC_IF_ERROR_RETURN(READ_CI_ERROR_MASKr(unit, ciC, &error_mask0));
                    SOC_IF_ERROR_RETURN(WRITE_CI_ERROR_MASKr(unit, ciC, rval));
                }
                if(_check_dram(ciC + 1)) {
                    SOC_IF_ERROR_RETURN(READ_CI_ERROR_MASKr(unit, ciC + 1, &error_mask1));
                    SOC_IF_ERROR_RETURN(WRITE_CI_ERROR_MASKr(unit, ciC + 1, rval));
                }
                
                if((ci & 0x1) == 0) {
                    if(_check_dram_either(ci)) {
                        SOC_IF_ERROR_RETURN(_soc_ddr40_phy_PVT_ctl(unit, ci, 1, stat));
                        SOC_IF_ERROR_RETURN(_soc_ddr_shmoo_prepare_for_shmoo(unit, ci));
                    }
                }
                sp = sal_alloc(sizeof(soc_ddr_shmoo_param_t), "Shmoo_Params");
                if (sp == NULL) {
                    return(SOC_E_MEMORY);
                }
                sal_memset(sp, 0, sizeof(soc_ddr_shmoo_param_t));
                sp->vwsPtr = sal_alloc(sizeof(vref_word_shmoo), "Shmoo_vws");
                if (sp->vwsPtr == NULL) {
                    sal_free(sp);
                    return(SOC_E_MEMORY);
                }
                for(sp->wl = 0; sp->wl < 2; sp->wl++) {
                    if(!_check_dram(ci+(sp->wl))) {
                        continue;
                    }
                    sp->type = SHMOO_INIT_VDL_RESULT;
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "BEFORE SHMOO: Type = %d\tCI = %d\tWL = %d\n"),
                              sp->type, ci, sp->wl));
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    _soc_ddr40_shmoo_get_init_step(unit, ci, sp);
                    _soc_ddr40_shmoo_do(unit, ci, sp);
                    _soc_ddr40_shmoo_calib(unit,ci, sp);
                    _soc_ddr40_shmoo_set_new_step(unit, ci, sp);
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "AFTER SHMOO:  Type = %d\tCI = %d\tWL = %d\n"),
                              sp->type, ci, sp->wl));
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    if (isplot || LOG_CHECK(BSL_LS_SOC_DDR | BSL_INFO)) {
                        _soc_ddr40_shmoo_plot(sp);
                    }
                }
                for(sp->wl = 0; sp->wl < 2; sp->wl++) {
                    if(!_check_dram(ci+(sp->wl))) {
                        continue;
                    }
                    sp->type = SHMOO_RD_EN;
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "BEFORE SHMOO: Type = %d\tCI = %d\tWL = %d\n"),
                              sp->type, ci, sp->wl));
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    _soc_ddr40_shmoo_get_init_step(unit, ci, sp);
                    _soc_ddr40_shmoo_do(unit, ci, sp);
                    _soc_ddr40_shmoo_calib(unit,ci, sp);
                    _soc_ddr40_shmoo_set_new_step(unit, ci, sp);
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "AFTER SHMOO:  Type = %d\tCI = %d\tWL = %d\n"),
                              sp->type, ci, sp->wl));
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    if (isplot || LOG_CHECK(BSL_LS_SOC_DDR | BSL_INFO)) {
                        _soc_ddr40_shmoo_plot(sp);
                    }
                    sp->type = SHMOO_RD_DQ;
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "BEFORE SHMOO: Type = %d\tCI = %d\tWL = %d\n"),
                              sp->type, ci, sp->wl));
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    _soc_ddr40_shmoo_get_init_step(unit, ci, sp);
                    _soc_ddr40_shmoo_do(unit, ci, sp);
                    _soc_ddr40_shmoo_calib(unit,ci, sp);
                    _soc_ddr40_shmoo_set_new_step(unit, ci, sp);
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "AFTER SHMOO:  Type = %d\tCI = %d\tWL = %d\n"),
                              sp->type, ci, sp->wl));
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    if (isplot || LOG_CHECK(BSL_LS_SOC_DDR | BSL_INFO)) {
                        _soc_ddr40_shmoo_plot(sp);
                    }
                    sp->type = SHMOO_WR_DQ;
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "BEFORE SHMOO: Type = %d\tCI = %d\tWL = %d\n"),
                              sp->type, ci, sp->wl));
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    _soc_ddr40_shmoo_get_init_step(unit, ci, sp);
                    _soc_ddr40_shmoo_do(unit, ci, sp);
                    _soc_ddr40_shmoo_calib(unit,ci, sp);
                    _soc_ddr40_shmoo_set_new_step(unit, ci, sp);
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "AFTER SHMOO:  Type = %d\tCI = %d\tWL = %d\n"),
                              sp->type, ci, sp->wl));
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    if (isplot || LOG_CHECK(BSL_LS_SOC_DDR | BSL_INFO)) {
                        _soc_ddr40_shmoo_plot(sp);
                    }
                    if((_check_dram_both(ci+(sp->wl)) && (sp->wl == 1)) || (!_check_dram_both(ci+(sp->wl)) && _check_dram_either(ci+(sp->wl)))) {
                        sp->wl = 0;
                        for(sp->type = SHMOO_ADDRC; sp->type < SHMOO_WR_DM; sp->type++) {
                            LOG_INFO(BSL_LS_SOC_DDR,
                                     (BSL_META_U(unit,
                                                 "BEFORE SHMOO: Type = %d\tCI = %d\tWL = Both\n"),
                                      sp->type, ci));
                            _shmoo_print_diagnostics(unit, ci, sp->wl);
                            _soc_ddr40_shmoo_get_init_step(unit, ci, sp);
                            _soc_ddr40_shmoo_do(unit, ci, sp);
                            _soc_ddr40_shmoo_calib(unit,ci,sp);
                            _soc_ddr40_shmoo_set_new_step(unit, ci, sp);
                            LOG_INFO(BSL_LS_SOC_DDR,
                                     (BSL_META_U(unit,
                                                 "AFTER SHMOO:  Type = %d\tCI = %d\tWL = Both\n"),
                                      sp->type, ci));
                            _shmoo_print_diagnostics(unit, ci, sp->wl);
                            if (isplot || LOG_CHECK(BSL_LS_SOC_DDR | BSL_INFO)) {
                                _soc_ddr40_shmoo_plot(sp);
                            }
                        }
                        sp->wl = 1;
                    }
                }
                if (sp->vwsPtr != NULL) {
                    sal_free(sp->vwsPtr);
                }
                if (sp != NULL) {
                    sal_free(sp);
                }
                
                rval = 0;
                soc_reg_field_set(unit, CI_ERRORr, &rval, WFIFO_CTL_CORRECTED_ERRORf, 1);
                soc_reg_field_set(unit, CI_ERRORr, &rval, WFIFO_CTL_UNCORRECTED_ERRORf, 1);
                soc_reg_field_set(unit, CI_ERRORr, &rval, RFIFO_CTL_CORRECTED_ERRORf, 1);
                soc_reg_field_set(unit, CI_ERRORr, &rval, RFIFO_CTL_UNCORRECTED_ERRORf, 1);
                soc_reg_field_set(unit, CI_ERRORr, &rval, PHY_BL0_RD_FIFO_ERRORf, 1);
                soc_reg_field_set(unit, CI_ERRORr, &rval, PHY_BL1_RD_FIFO_ERRORf, 1);
                soc_reg_field_set(unit, CI_ERRORr, &rval, RFIFO_OVERFLOWf, 1);
                soc_reg_field_set(unit, CI_ERRORr, &rval, WFIFO_OVERFLOWf, 1);
                soc_reg_field_set(unit, CI_ERRORr, &rval, PHY_READY_EVENTf, 1);
                if(_check_dram(ciC)) {
                    SOC_IF_ERROR_RETURN(WRITE_CI_ERROR_MASKr(unit, ciC, error_mask0));
                    SOC_IF_ERROR_RETURN(WRITE_CI_ERRORr(unit, ciC, rval));
                }
                if(_check_dram(ciC + 1)) {
                    SOC_IF_ERROR_RETURN(WRITE_CI_ERROR_MASKr(unit, ciC + 1, error_mask1));
                    SOC_IF_ERROR_RETURN(WRITE_CI_ERRORr(unit, ciC + 1, rval));
                }
                
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "DDR Tuning Complete\n")));
                break;
            case DDR_CTLR_T1:
                if((ci & 0x1) == 0) {
                    if(_check_dram_either(ci)) {
                        SOC_IF_ERROR_RETURN(_soc_ddr40_phy_PVT_ctl(unit, ci, 1, stat));
                        SOC_IF_ERROR_RETURN(_soc_ddr_shmoo_prepare_for_shmoo(unit, ci));
                    }
                }
                sp = sal_alloc(sizeof(soc_ddr_shmoo_param_t), "Shmoo_Params");
                if (sp == NULL) {
                    return(SOC_E_MEMORY);
                }
                sal_memset(sp, 0, sizeof(soc_ddr_shmoo_param_t));
                sp->vwsPtr = sal_alloc(sizeof(vref_word_shmoo), "Shmoo_vws");
                if (sp->vwsPtr == NULL) {
                    sal_free(sp);
                    return(SOC_E_MEMORY);
                }
                for(sp->wl = 0; sp->wl < 2; sp->wl++) {
                    if(!_check_dram(ci+(sp->wl))) {
                        continue;
                    }
                    sp->type = SHMOO_INIT_VDL_RESULT;
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "BEFORE SHMOO: Type = %d\tCI = %d\tWL = %d\n"),
                              sp->type, ci, sp->wl));
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    _soc_ddr40_shmoo_get_init_step(unit, ci, sp);
                    _soc_ddr40_shmoo_do(unit, ci, sp);
                    _soc_ddr40_shmoo_calib(unit,ci, sp);
                    _soc_ddr40_shmoo_set_new_step(unit, ci, sp);
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "AFTER SHMOO:  Type = %d\tCI = %d\tWL = %d\n"),
                              sp->type, ci, sp->wl));
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    if (isplot || LOG_CHECK(BSL_LS_SOC_DDR | BSL_INFO)) {
                        _soc_ddr40_shmoo_plot(sp);
                    }
                }
                for(sp->wl = 0; sp->wl < 2; sp->wl++) {
                    if(!_check_dram(ci+(sp->wl))) {
                        continue;
                    }
                    sp->type = SHMOO_RD_EN;
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "BEFORE SHMOO: Type = %d\tCI = %d\tWL = %d\n"),
                              sp->type, ci, sp->wl));
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    _soc_ddr40_shmoo_get_init_step(unit, ci, sp);
                    _soc_ddr40_shmoo_do(unit, ci, sp);
                    _soc_ddr40_shmoo_calib(unit,ci, sp);
                    _soc_ddr40_shmoo_set_new_step(unit, ci, sp);
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "AFTER SHMOO:  Type = %d\tCI = %d\tWL = %d\n"),
                              sp->type, ci, sp->wl));
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    if (isplot || LOG_CHECK(BSL_LS_SOC_DDR | BSL_INFO)) {
                        _soc_ddr40_shmoo_plot(sp);
                    }
                    sp->type = SHMOO_RD_DQ;
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "BEFORE SHMOO: Type = %d\tCI = %d\tWL = %d\n"),
                              sp->type, ci, sp->wl));
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    _soc_ddr40_shmoo_get_init_step(unit, ci, sp);
                    _soc_ddr40_shmoo_do(unit, ci, sp);
                    _soc_ddr40_shmoo_calib(unit,ci, sp);
                    _soc_ddr40_shmoo_set_new_step(unit, ci, sp);
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "AFTER SHMOO:  Type = %d\tCI = %d\tWL = %d\n"),
                              sp->type, ci, sp->wl));
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    if (isplot || LOG_CHECK(BSL_LS_SOC_DDR | BSL_INFO)) {
                        _soc_ddr40_shmoo_plot(sp);
                    }
                    sp->type = SHMOO_WR_DQ;
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "BEFORE SHMOO: Type = %d\tCI = %d\tWL = %d\n"),
                              sp->type, ci, sp->wl));
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    _soc_ddr40_shmoo_get_init_step(unit, ci, sp);
                    _soc_ddr40_shmoo_do(unit, ci, sp);
                    _soc_ddr40_shmoo_calib(unit,ci, sp);
                    _soc_ddr40_shmoo_set_new_step(unit, ci, sp);
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "AFTER SHMOO:  Type = %d\tCI = %d\tWL = %d\n"),
                              sp->type, ci, sp->wl));
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    if (isplot || LOG_CHECK(BSL_LS_SOC_DDR | BSL_INFO)) {
                        _soc_ddr40_shmoo_plot(sp);
                    }
                    if((_check_dram_both(ci+(sp->wl)) && (sp->wl == 1)) || (!_check_dram_both(ci+(sp->wl)) && _check_dram_either(ci+(sp->wl)))) {
                        sp->wl = 0;
                        for(sp->type = SHMOO_ADDRC; sp->type < SHMOO_WR_DM; sp->type++) {
                            LOG_INFO(BSL_LS_SOC_DDR,
                                     (BSL_META_U(unit,
                                                 "BEFORE SHMOO: Type = %d\tCI = %d\tWL = Both\n"),
                                      sp->type, ci));
                            _shmoo_print_diagnostics(unit, ci, sp->wl);
                            _soc_ddr40_shmoo_get_init_step(unit, ci, sp);
                            _soc_ddr40_shmoo_do(unit, ci, sp);
                            _soc_ddr40_shmoo_calib(unit,ci,sp);
                            _soc_ddr40_shmoo_set_new_step(unit, ci, sp);
                            LOG_INFO(BSL_LS_SOC_DDR,
                                     (BSL_META_U(unit,
                                                 "AFTER SHMOO:  Type = %d\tCI = %d\tWL = Both\n"),
                                      sp->type, ci));
                            _shmoo_print_diagnostics(unit, ci, sp->wl);
                            if (isplot || LOG_CHECK(BSL_LS_SOC_DDR | BSL_INFO)) {
                                _soc_ddr40_shmoo_plot(sp);
                            }
                        }
                        sp->wl = 1;
                    }
                }
                if (sp->vwsPtr != NULL) {
                    sal_free(sp->vwsPtr);
                }
                if (sp != NULL) {
                    sal_free(sp);
                }
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "DDR Tuning Complete\n")));
                break;
            case DDR_CTLR_T2:
                if((ci & 0x1) == 0) {
                    if(_check_dram_either(ci)) {
                        SOC_IF_ERROR_RETURN(_soc_ddr40_phy_PVT_ctl(unit, ci, 1, stat));
                        SOC_IF_ERROR_RETURN(_soc_ddr_shmoo_prepare_for_shmoo(unit, ci));
                    }
                }
                sp = sal_alloc(sizeof(soc_ddr_shmoo_param_t), "Shmoo_Params");
                if (sp == NULL) {
                    return(SOC_E_MEMORY);
                }
                sal_memset(sp, 0, sizeof(soc_ddr_shmoo_param_t));
                sp->vwsPtr = sal_alloc(sizeof(vref_word_shmoo), "Shmoo_vws");
                if (sp->vwsPtr == NULL) {
                    sal_free(sp);
                    return(SOC_E_MEMORY);
                }
                for(sp->wl = 0; sp->wl < 2; sp->wl++) {
                    if(!_check_dram(ci+(sp->wl))) {
                        continue;
                    }
                    sp->type = SHMOO_INIT_VDL_RESULT;
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "BEFORE SHMOO: Type = %d\tCI = %d\tWL = %d\n"),
                              sp->type, ci, sp->wl));
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    _soc_ddr40_shmoo_get_init_step(unit, ci, sp);
                    _soc_ddr40_shmoo_do(unit, ci, sp);
                    _soc_ddr40_shmoo_calib(unit,ci, sp);
                    _soc_ddr40_shmoo_set_new_step(unit, ci, sp);
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "AFTER SHMOO:  Type = %d\tCI = %d\tWL = %d\n"),
                              sp->type, ci, sp->wl));
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    if (isplot || LOG_CHECK(BSL_LS_SOC_DDR | BSL_INFO)) {
                        _soc_ddr40_shmoo_plot(sp);
                    }
                }
                for(sp->wl = 0; sp->wl < 2; sp->wl++) {
                    if(!_check_dram(ci+(sp->wl))) {
                        continue;
                    }
                    sp->type = SHMOO_RD_EN;
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "BEFORE SHMOO: Type = %d\tCI = %d\tWL = %d\n"),
                              sp->type, ci, sp->wl));
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    _soc_ddr40_shmoo_get_init_step(unit, ci, sp);
                    _soc_ddr40_shmoo_do(unit, ci, sp);
                    _soc_ddr40_shmoo_calib(unit,ci, sp);
                    _soc_ddr40_shmoo_set_new_step(unit, ci, sp);
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "AFTER SHMOO:  Type = %d\tCI = %d\tWL = %d\n"),
                              sp->type, ci, sp->wl));
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    if (isplot || LOG_CHECK(BSL_LS_SOC_DDR | BSL_INFO)) {
                        _soc_ddr40_shmoo_plot(sp);
                    }
                    sp->type = SHMOO_RD_DQ;
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "BEFORE SHMOO: Type = %d\tCI = %d\tWL = %d\n"),
                              sp->type, ci, sp->wl));
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    _soc_ddr40_shmoo_get_init_step(unit, ci, sp);
                    _soc_ddr40_shmoo_do(unit, ci, sp);
                    _soc_ddr40_shmoo_calib(unit,ci, sp);
                    _soc_ddr40_shmoo_set_new_step(unit, ci, sp);
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "AFTER SHMOO:  Type = %d\tCI = %d\tWL = %d\n"),
                              sp->type, ci, sp->wl));
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    if (isplot || LOG_CHECK(BSL_LS_SOC_DDR | BSL_INFO)) {
                        _soc_ddr40_shmoo_plot(sp);
                    }
                    sp->type = SHMOO_WR_DQ;
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "BEFORE SHMOO: Type = %d\tCI = %d\tWL = %d\n"),
                              sp->type, ci, sp->wl));
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    _soc_ddr40_shmoo_get_init_step(unit, ci, sp);
                    _soc_ddr40_shmoo_do(unit, ci, sp);
                    _soc_ddr40_shmoo_calib(unit,ci, sp);
                    _soc_ddr40_shmoo_set_new_step(unit, ci, sp);
                    LOG_INFO(BSL_LS_SOC_DDR,
                             (BSL_META_U(unit,
                                         "AFTER SHMOO:  Type = %d\tCI = %d\tWL = %d\n"),
                              sp->type, ci, sp->wl));
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    if (isplot || LOG_CHECK(BSL_LS_SOC_DDR | BSL_INFO)) {
                        _soc_ddr40_shmoo_plot(sp);
                    }
                    if((_check_dram_both(ci+(sp->wl)) && (sp->wl == 1)) || (!_check_dram_both(ci+(sp->wl)) && _check_dram_either(ci+(sp->wl)))) {
                        sp->wl = 0;
                        for(sp->type = SHMOO_ADDRC; sp->type < SHMOO_WR_DM; sp->type++) {
                            LOG_INFO(BSL_LS_SOC_DDR,
                                     (BSL_META_U(unit,
                                                 "BEFORE SHMOO: Type = %d\tCI = %d\tWL = Both\n"),
                                      sp->type, ci));
                            _shmoo_print_diagnostics(unit, ci, sp->wl);
                            _soc_ddr40_shmoo_get_init_step(unit, ci, sp);
                            _soc_ddr40_shmoo_do(unit, ci, sp);
                            _soc_ddr40_shmoo_calib(unit,ci,sp);
                            _soc_ddr40_shmoo_set_new_step(unit, ci, sp);
                            LOG_INFO(BSL_LS_SOC_DDR,
                                     (BSL_META_U(unit,
                                                 "AFTER SHMOO:  Type = %d\tCI = %d\tWL = Both\n"),
                                      sp->type, ci));
                            _shmoo_print_diagnostics(unit, ci, sp->wl);
                            if (isplot || LOG_CHECK(BSL_LS_SOC_DDR | BSL_INFO)) {
                                _soc_ddr40_shmoo_plot(sp);
                            }
                        }
                        sp->wl = 1;
                    }
                }
                if (sp->vwsPtr != NULL) {
                    sal_free(sp->vwsPtr);
                }
                if (sp != NULL) {
                    sal_free(sp);
                }
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "DDR Tuning Complete\n")));
                break;
            case DDR_CTLR_T3:
                break;
            default:
                break;
        }
    } else {
        /* report only */
        switch(ctlType) {
            case DDR_CTLR_TRSVP:
                break;
            case DDR_CTLR_T0:
                break;
            case DDR_CTLR_T1:
                break;
            case DDR_CTLR_T2:
                break;
            case DDR_CTLR_T3:
                break;
            default:
                break;
        }
    }
    return SOC_E_NONE;
}

#define OVR_COMPRESS(_v) (((_v) & 0x3f)  | \
                           (((_v) & 0x100) ? 0x40 : 0) | \
                           (((_v) & 0x10000) ? 0x80 : 0))


int soc_ddr40_shmoo_savecfg(int unit, int ci)
{
    char name_str[32], val_str[96];
    uint32 value[4], valarr[4] = { 0, 0, 0, 0};

    if (soc_mem_config_set == NULL) {
        return SOC_E_UNAVAIL;
    }

    /* SHMOO_RD_DATA_DLY: */
    READ_DDR40_PHY_WORD_LANE_0_READ_DATA_DLYr(unit, ci, (uint32 *)&valarr[0]); /* Only Bits 0..2 */
    READ_DDR40_PHY_WORD_LANE_1_READ_DATA_DLYr(unit, ci, (uint32 *)&valarr[1]); /* Only Bits 0..2 */
    value[0] = ((valarr[1] & 0x7) << 8) | (valarr[0] & 0x7);
    sal_sprintf(name_str, "%s_ci%d.%d", spn_DDR3_TUNE_RD_DATA_DLY, ci, unit);
    sal_sprintf(val_str, "0x%08x", value[0]);
    if (soc_mem_config_set(name_str, val_str) < 0) {
         return SOC_E_MEMORY;
    }

    /* SHMOO_RD_EN: */
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, (uint32 *)&valarr[2]);
    value[0] = OVR_COMPRESS(valarr[0]) | (OVR_COMPRESS(valarr[1]) << 8) | (OVR_COMPRESS(valarr[2]) << 16);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, (uint32 *)&valarr[2]);
    value[1] = OVR_COMPRESS(valarr[0]) | (OVR_COMPRESS(valarr[1]) << 8) | (OVR_COMPRESS(valarr[2]) << 16);
    sal_sprintf(name_str, "%s_ci%d.%d", spn_DDR3_TUNE_RD_EN, ci, unit);
    sal_sprintf(val_str, "0x%08x,0x%08x", value[0], value[1]);
    if (soc_mem_config_set(name_str, val_str) < 0) {
         return SOC_E_MEMORY;
    }

    /* SHMOO_RD_DQ : */
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_R_Pr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT1_R_Pr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT2_R_Pr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT3_R_Pr(unit, ci, (uint32 *)&valarr[3]);
    value[0] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT4_R_Pr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT5_R_Pr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT6_R_Pr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT7_R_Pr(unit, ci, (uint32 *)&valarr[3]);
    value[1] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_R_Pr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT1_R_Pr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT2_R_Pr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT3_R_Pr(unit, ci, (uint32 *)&valarr[3]);
    value[2] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT4_R_Pr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT5_R_Pr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT6_R_Pr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT7_R_Pr(unit, ci, (uint32 *)&valarr[3]);
    value[3] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    sal_sprintf(name_str, "%s_ci%d.%d", spn_DDR3_TUNE_RD_DQ_WL0_RP, ci, unit);
    sal_sprintf(val_str, "0x%08x,0x%08x,0x%08x,0x%08x", value[0], value[1], value[2], value[3]);
    if (soc_mem_config_set(name_str, val_str) < 0) {
         return SOC_E_MEMORY;
    }

    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT0_R_Pr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT1_R_Pr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT2_R_Pr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT3_R_Pr(unit, ci, (uint32 *)&valarr[3]);
    value[0] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT4_R_Pr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT5_R_Pr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT6_R_Pr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT7_R_Pr(unit, ci, (uint32 *)&valarr[3]);
    value[1] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT0_R_Pr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT1_R_Pr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT2_R_Pr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT3_R_Pr(unit, ci, (uint32 *)&valarr[3]);
    value[2] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT4_R_Pr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT5_R_Pr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT6_R_Pr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT7_R_Pr(unit, ci, (uint32 *)&valarr[3]);
    value[3] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    sal_sprintf(name_str, "%s_ci%d.%d", spn_DDR3_TUNE_RD_DQ_WL1_RP, ci, unit);
    sal_sprintf(val_str, "0x%08x,0x%08x,0x%08x,0x%08x", value[0], value[1], value[2], value[3]);
    if (soc_mem_config_set(name_str, val_str) < 0) {
         return SOC_E_MEMORY;
    }

    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_R_Nr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT1_R_Nr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT2_R_Nr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT3_R_Nr(unit, ci, (uint32 *)&valarr[3]);
    value[0] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT4_R_Nr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT5_R_Nr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT6_R_Nr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT7_R_Nr(unit, ci, (uint32 *)&valarr[3]);
    value[1] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_R_Nr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT1_R_Nr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT2_R_Nr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT3_R_Nr(unit, ci, (uint32 *)&valarr[3]);
    value[2] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT4_R_Nr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT5_R_Nr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT6_R_Nr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT7_R_Nr(unit, ci, (uint32 *)&valarr[3]);
    value[3] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    sal_sprintf(name_str, "%s_ci%d.%d", spn_DDR3_TUNE_RD_DQ_WL0_RN, ci, unit);
    sal_sprintf(val_str, "0x%08x,0x%08x,0x%08x,0x%08x", value[0], value[1], value[2], value[3]);
    if (soc_mem_config_set(name_str, val_str) < 0) {
         return SOC_E_MEMORY;
    }

    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT0_R_Nr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT1_R_Nr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT2_R_Nr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT3_R_Nr(unit, ci, (uint32 *)&valarr[3]);
    value[0] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT4_R_Nr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT5_R_Nr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT6_R_Nr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT7_R_Nr(unit, ci, (uint32 *)&valarr[3]);
    value[1] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT0_R_Nr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT1_R_Nr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT2_R_Nr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT3_R_Nr(unit, ci, (uint32 *)&valarr[3]);
    value[2] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT4_R_Nr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT5_R_Nr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT6_R_Nr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT7_R_Nr(unit, ci, (uint32 *)&valarr[3]);
    value[3] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    sal_sprintf(name_str, "%s_ci%d.%d", spn_DDR3_TUNE_RD_DQ_WL1_RN, ci, unit);
    sal_sprintf(val_str, "0x%08x,0x%08x,0x%08x,0x%08x", value[0], value[1], value[2], value[3]);
    if (soc_mem_config_set(name_str, val_str) < 0) {
         return SOC_E_MEMORY;
    }

    /* SHMOO_RD_DQS: */
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_Nr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, (uint32 *)&valarr[3]);
    value[0] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_Nr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, (uint32 *)&valarr[3]);
    value[1] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    sal_sprintf(name_str, "%s_ci%d.%d", spn_DDR3_TUNE_RD_DQS, ci, unit);
    sal_sprintf(val_str, "0x%08x,0x%08x", value[0], value[1]);
    if (soc_mem_config_set(name_str, val_str) < 0) {
         return SOC_E_MEMORY;
    }

    /* SHMOO_VREF: */
    READ_DDR40_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, ci, (uint32 *)&valarr[0]);
    sal_sprintf(name_str, "%s_ci%d.%d", spn_DDR3_TUNE_VREF, ci, unit);
    sal_sprintf(val_str, "0x%08x", valarr[0]);
    if (soc_mem_config_set(name_str, val_str) < 0) {
         return SOC_E_MEMORY;
    }

    /* SHMOO_WR_DQ : */
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_Wr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_Wr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_Wr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_Wr(unit, ci, (uint32 *)&valarr[3]);
    value[0] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    sal_sprintf(name_str, "%s_ci%d.%d", spn_DDR3_TUNE_WR_DQ, ci, unit);
    sal_sprintf(val_str, "0x%08x", value[0]);
    if (soc_mem_config_set(name_str, val_str) < 0) {
         return SOC_E_MEMORY;
    }

    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_Wr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT1_Wr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT2_Wr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT3_Wr(unit, ci, (uint32 *)&valarr[3]);
    value[0] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT4_Wr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT5_Wr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT6_Wr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT7_Wr(unit, ci, (uint32 *)&valarr[3]);
    value[1] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_Wr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT1_Wr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT2_Wr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT3_Wr(unit, ci, (uint32 *)&valarr[3]);
    value[2] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT4_Wr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT5_Wr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT6_Wr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT7_Wr(unit, ci, (uint32 *)&valarr[3]);
    value[3] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    sal_sprintf(name_str, "%s_ci%d.%d", spn_DDR3_TUNE_WR_DQ_WL0, ci, unit);
    sal_sprintf(val_str, "0x%08x,0x%08x,0x%08x,0x%08x", value[0], value[1], value[2], value[3]);
    if (soc_mem_config_set(name_str, val_str) < 0) {
         return SOC_E_MEMORY;
    }

    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT0_Wr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT1_Wr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT2_Wr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT3_Wr(unit, ci, (uint32 *)&valarr[3]);
    value[0] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT4_Wr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT5_Wr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT6_Wr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT7_Wr(unit, ci, (uint32 *)&valarr[3]);
    value[1] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT0_Wr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT1_Wr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT2_Wr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT3_Wr(unit, ci, (uint32 *)&valarr[3]);
    value[2] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT4_Wr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT5_Wr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT6_Wr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT7_Wr(unit, ci, (uint32 *)&valarr[3]);
    value[3] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    sal_sprintf(name_str, "%s_ci%d.%d", spn_DDR3_TUNE_WR_DQ_WL1, ci, unit);
    sal_sprintf(val_str, "0x%08x,0x%08x,0x%08x,0x%08x", value[0], value[1], value[2], value[3]);
    if (soc_mem_config_set(name_str, val_str) < 0) {
         return SOC_E_MEMORY;
    }

    /* SHMOO_ADDRC: */
    READ_DDR40_PHY_CONTROL_REGS_VDL_OVRIDE_BIT_CTLr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_CONTROL_REGS_VDL_OVRIDE_BYTE_CTLr(unit, ci, (uint32 *)&valarr[1]);
    value[0] = OVR_COMPRESS(valarr[0]) | (OVR_COMPRESS(valarr[1]) << 8);
    sal_sprintf(name_str, "%s_ci%d.%d", spn_DDR3_TUNE_ADDRC, ci, unit);
    sal_sprintf(val_str, "0x%08x", value[0]);
    if (soc_mem_config_set(name_str, val_str) < 0) {
         return SOC_E_MEMORY;
    }
    return SOC_E_NONE;
}

#define OVR_EXPAND(_v) (((_v) & 0x3f)  | (0x20000) | \
                           (((_v)&0x40) ? 0x100 : 0) | \
                           (((_v)&0x80) ? 0x10000 : 0))

int soc_ddr40_shmoo_restorecfg(int unit, int ci)
{
    int value[4], valarr[4];
    int ciC;
    uint32 error_mask0, error_mask1;
    uint32 rval;
    
    ciC = ci & 0xFFFFFFFE;
    
    if(SOC_IS_KATANAX(unit)) {
        rval = 0;
        soc_reg_field_set(unit, CI_ERROR_MASKr, &rval, WFIFO_CTL_CORRECTED_ERROR_DISINTf, 1);
        soc_reg_field_set(unit, CI_ERROR_MASKr, &rval, WFIFO_CTL_UNCORRECTED_ERROR_DISINTf, 1);
        soc_reg_field_set(unit, CI_ERROR_MASKr, &rval, RFIFO_CTL_CORRECTED_ERROR_DISINTf, 1);
        soc_reg_field_set(unit, CI_ERROR_MASKr, &rval, RFIFO_CTL_UNCORRECTED_ERROR_DISINTf, 1);
        soc_reg_field_set(unit, CI_ERROR_MASKr, &rval, PHY_BL0_RD_FIFO_ERRORf, 1);
        soc_reg_field_set(unit, CI_ERROR_MASKr, &rval, PHY_BL1_RD_FIFO_ERRORf, 1);
        soc_reg_field_set(unit, CI_ERROR_MASKr, &rval, RFIFO_OVERFLOW_DISINTf, 1);
        soc_reg_field_set(unit, CI_ERROR_MASKr, &rval, WB_OVERFLOW_DISINTf, 1);
        soc_reg_field_set(unit, CI_ERROR_MASKr, &rval, PHY_READYf, 1);
        if(_check_dram(ciC)) {
            SOC_IF_ERROR_RETURN(READ_CI_ERROR_MASKr(unit, ciC, &error_mask0));
            SOC_IF_ERROR_RETURN(WRITE_CI_ERROR_MASKr(unit, ciC, rval));
        }
        if(_check_dram(ciC + 1)) {
            SOC_IF_ERROR_RETURN(READ_CI_ERROR_MASKr(unit, ciC + 1, &error_mask1));
            SOC_IF_ERROR_RETURN(WRITE_CI_ERROR_MASKr(unit, ciC + 1, rval));
        }
    }

    /* SHMOO_RD_DATA_DLY: */
    if (soc_property_ci_get_csv(unit, ci, spn_DDR3_TUNE_RD_DATA_DLY,4,value) != 1) {
        return SOC_E_CONFIG;
    }
    valarr[0] = value[0] & 0x7;
    valarr[1] = (value[0] >> 8) & 0x7;
    WRITE_DDR40_PHY_WORD_LANE_0_READ_DATA_DLYr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_DATA_DLYr(unit, ci, valarr[1]);

    /* SHMOO_RD_EN: */
    if (soc_property_ci_get_csv(unit, ci, spn_DDR3_TUNE_RD_EN,4,value) != 2) {
        return SOC_E_CONFIG;
    }
    valarr[0] = OVR_EXPAND(value[0]);
    valarr[1] = OVR_EXPAND(value[0]>>8);
    valarr[2] = OVR_EXPAND(value[0]>>16);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, valarr[2]);
    valarr[0] = OVR_EXPAND(value[1]);
    valarr[1] = OVR_EXPAND(value[1]>>8);
    valarr[2] = OVR_EXPAND(value[1]>>16);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, valarr[2]);

    /* SHMOO_RD_DQ : */
    if (soc_property_ci_get_csv(unit, ci, spn_DDR3_TUNE_RD_DQ_WL0_RP,4,value) != 4) {
        return SOC_E_CONFIG;
    }
    valarr[0] = OVR_EXPAND(value[0]);
    valarr[1] = OVR_EXPAND(value[0]>>8);
    valarr[2] = OVR_EXPAND(value[0]>>16);
    valarr[3] = OVR_EXPAND(value[0]>>24);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_R_Pr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT1_R_Pr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT2_R_Pr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT3_R_Pr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[1]);
    valarr[1] = OVR_EXPAND(value[1]>>8);
    valarr[2] = OVR_EXPAND(value[1]>>16);
    valarr[3] = OVR_EXPAND(value[1]>>24);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT4_R_Pr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT5_R_Pr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT6_R_Pr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT7_R_Pr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[2]);
    valarr[1] = OVR_EXPAND(value[2]>>8);
    valarr[2] = OVR_EXPAND(value[2]>>16);
    valarr[3] = OVR_EXPAND(value[2]>>24);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_R_Pr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT1_R_Pr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT2_R_Pr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT3_R_Pr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[3]);
    valarr[1] = OVR_EXPAND(value[3]>>8);
    valarr[2] = OVR_EXPAND(value[3]>>16);
    valarr[3] = OVR_EXPAND(value[3]>>24);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT4_R_Pr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT5_R_Pr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT6_R_Pr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT7_R_Pr(unit, ci, valarr[3]);

    if (soc_property_ci_get_csv(unit, ci, spn_DDR3_TUNE_RD_DQ_WL1_RP,4,value) != 4) {
        return SOC_E_CONFIG;
    }
    valarr[0] = OVR_EXPAND(value[0]);
    valarr[1] = OVR_EXPAND(value[0]>>8);
    valarr[2] = OVR_EXPAND(value[0]>>16);
    valarr[3] = OVR_EXPAND(value[0]>>24);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT0_R_Pr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT1_R_Pr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT2_R_Pr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT3_R_Pr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[1]);
    valarr[1] = OVR_EXPAND(value[1]>>8);
    valarr[2] = OVR_EXPAND(value[1]>>16);
    valarr[3] = OVR_EXPAND(value[1]>>24);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT4_R_Pr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT5_R_Pr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT6_R_Pr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT7_R_Pr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[2]);
    valarr[1] = OVR_EXPAND(value[2]>>8);
    valarr[2] = OVR_EXPAND(value[2]>>16);
    valarr[3] = OVR_EXPAND(value[2]>>24);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT0_R_Pr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT1_R_Pr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT2_R_Pr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT3_R_Pr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[3]);
    valarr[1] = OVR_EXPAND(value[3]>>8);
    valarr[2] = OVR_EXPAND(value[3]>>16);
    valarr[3] = OVR_EXPAND(value[3]>>24);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT4_R_Pr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT5_R_Pr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT6_R_Pr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT7_R_Pr(unit, ci, valarr[3]);

    if (soc_property_ci_get_csv(unit, ci, spn_DDR3_TUNE_RD_DQ_WL0_RN,4,value) != 4) {
        return SOC_E_CONFIG;
    }
    valarr[0] = OVR_EXPAND(value[0]);
    valarr[1] = OVR_EXPAND(value[0]>>8);
    valarr[2] = OVR_EXPAND(value[0]>>16);
    valarr[3] = OVR_EXPAND(value[0]>>24);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_R_Nr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT1_R_Nr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT2_R_Nr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT3_R_Nr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[1]);
    valarr[1] = OVR_EXPAND(value[1]>>8);
    valarr[2] = OVR_EXPAND(value[1]>>16);
    valarr[3] = OVR_EXPAND(value[1]>>24);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT4_R_Nr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT5_R_Nr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT6_R_Nr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT7_R_Nr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[2]);
    valarr[1] = OVR_EXPAND(value[2]>>8);
    valarr[2] = OVR_EXPAND(value[2]>>16);
    valarr[3] = OVR_EXPAND(value[2]>>24);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_R_Nr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT1_R_Nr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT2_R_Nr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT3_R_Nr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[3]);
    valarr[1] = OVR_EXPAND(value[3]>>8);
    valarr[2] = OVR_EXPAND(value[3]>>16);
    valarr[3] = OVR_EXPAND(value[3]>>24);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT4_R_Nr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT5_R_Nr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT6_R_Nr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT7_R_Nr(unit, ci, valarr[3]);

    if (soc_property_ci_get_csv(unit, ci, spn_DDR3_TUNE_RD_DQ_WL1_RN,4,value) != 4) {
        return SOC_E_CONFIG;
    }
    valarr[0] = OVR_EXPAND(value[0]);
    valarr[1] = OVR_EXPAND(value[0]>>8);
    valarr[2] = OVR_EXPAND(value[0]>>16);
    valarr[3] = OVR_EXPAND(value[0]>>24);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT0_R_Nr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT1_R_Nr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT2_R_Nr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT3_R_Nr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[1]);
    valarr[1] = OVR_EXPAND(value[1]>>8);
    valarr[2] = OVR_EXPAND(value[1]>>16);
    valarr[3] = OVR_EXPAND(value[1]>>24);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT4_R_Nr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT5_R_Nr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT6_R_Nr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT7_R_Nr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[2]);
    valarr[1] = OVR_EXPAND(value[2]>>8);
    valarr[2] = OVR_EXPAND(value[2]>>16);
    valarr[3] = OVR_EXPAND(value[2]>>24);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT0_R_Nr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT1_R_Nr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT2_R_Nr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT3_R_Nr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[3]);
    valarr[1] = OVR_EXPAND(value[3]>>8);
    valarr[2] = OVR_EXPAND(value[3]>>16);
    valarr[3] = OVR_EXPAND(value[3]>>24);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT4_R_Nr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT5_R_Nr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT6_R_Nr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT7_R_Nr(unit, ci, valarr[3]);


    /* SHMOO_RD_DQS: */
    if (soc_property_ci_get_csv(unit, ci, spn_DDR3_TUNE_RD_DQS,4,value) != 2) {
        return SOC_E_CONFIG;
    }
    valarr[0] = OVR_EXPAND(value[0]);
    valarr[1] = OVR_EXPAND(value[0]>>8);
    valarr[2] = OVR_EXPAND(value[0]>>16);
    valarr[3] = OVR_EXPAND(value[0]>>24);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_Nr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[1]);
    valarr[1] = OVR_EXPAND(value[1]>>8);
    valarr[2] = OVR_EXPAND(value[1]>>16);
    valarr[3] = OVR_EXPAND(value[1]>>24);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_Nr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, valarr[3]);

    /* SHMOO_VREF: */
    if (soc_property_ci_get_csv(unit, ci, spn_DDR3_TUNE_VREF,2,value) != 1) {
        return SOC_E_CONFIG;
    }
    WRITE_DDR40_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, ci, value[0]);

    /* SHMOO_WR_DQ : */
    if (soc_property_ci_get_csv(unit, ci, spn_DDR3_TUNE_WR_DQ,2,value) != 1) {
        return SOC_E_CONFIG;
    }
    valarr[0] = OVR_EXPAND(value[0]);
    valarr[1] = OVR_EXPAND(value[0]>>8);
    valarr[2] = OVR_EXPAND(value[0]>>16);
    valarr[3] = OVR_EXPAND(value[0]>>24);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_Wr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_Wr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_Wr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_Wr(unit, ci, valarr[3]);

    if (soc_property_ci_get_csv(unit, ci, spn_DDR3_TUNE_WR_DQ_WL0,4,value) != 4) {
        return SOC_E_CONFIG;
    }
    valarr[0] = OVR_EXPAND(value[0]);
    valarr[1] = OVR_EXPAND(value[0]>>8);
    valarr[2] = OVR_EXPAND(value[0]>>16);
    valarr[3] = OVR_EXPAND(value[0]>>24);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_Wr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT1_Wr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT2_Wr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT3_Wr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[1]);
    valarr[1] = OVR_EXPAND(value[1]>>8);
    valarr[2] = OVR_EXPAND(value[1]>>16);
    valarr[3] = OVR_EXPAND(value[1]>>24);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT4_Wr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT5_Wr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT6_Wr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT7_Wr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[2]);
    valarr[1] = OVR_EXPAND(value[2]>>8);
    valarr[2] = OVR_EXPAND(value[2]>>16);
    valarr[3] = OVR_EXPAND(value[2]>>24);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_Wr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT1_Wr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT2_Wr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT3_Wr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[3]);
    valarr[1] = OVR_EXPAND(value[3]>>8);
    valarr[2] = OVR_EXPAND(value[3]>>16);
    valarr[3] = OVR_EXPAND(value[3]>>24);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT4_Wr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT5_Wr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT6_Wr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT7_Wr(unit, ci, valarr[3]);

    if (soc_property_ci_get_csv(unit, ci, spn_DDR3_TUNE_WR_DQ_WL1,4,value) != 4) {
        return SOC_E_CONFIG;
    }
    valarr[0] = OVR_EXPAND(value[0]);
    valarr[1] = OVR_EXPAND(value[0]>>8);
    valarr[2] = OVR_EXPAND(value[0]>>16);
    valarr[3] = OVR_EXPAND(value[0]>>24);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT0_Wr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT1_Wr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT2_Wr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT3_Wr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[1]);
    valarr[1] = OVR_EXPAND(value[1]>>8);
    valarr[2] = OVR_EXPAND(value[1]>>16);
    valarr[3] = OVR_EXPAND(value[1]>>24);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT4_Wr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT5_Wr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT6_Wr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT7_Wr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[2]);
    valarr[1] = OVR_EXPAND(value[2]>>8);
    valarr[2] = OVR_EXPAND(value[2]>>16);
    valarr[3] = OVR_EXPAND(value[2]>>24);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT0_Wr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT1_Wr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT2_Wr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT3_Wr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[3]);
    valarr[1] = OVR_EXPAND(value[3]>>8);
    valarr[2] = OVR_EXPAND(value[3]>>16);
    valarr[3] = OVR_EXPAND(value[3]>>24);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT4_Wr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT5_Wr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT6_Wr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT7_Wr(unit, ci, valarr[3]);

    /* SHMOO_ADDRC: */
    if (soc_property_ci_get_csv(unit, ci, spn_DDR3_TUNE_ADDRC,2,value) != 1) {
        return SOC_E_CONFIG;
    }
    valarr[0] = OVR_EXPAND(value[0]);
    valarr[1] = OVR_EXPAND(value[0]>>8);
    WRITE_DDR40_PHY_CONTROL_REGS_VDL_OVRIDE_BIT_CTLr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_CONTROL_REGS_VDL_OVRIDE_BYTE_CTLr(unit, ci, valarr[1]);

    /* Clear the FIFO error state */
    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
    
    if(SOC_IS_KATANAX(unit)) {
        rval = 0;
        soc_reg_field_set(unit, CI_ERRORr, &rval, WFIFO_CTL_CORRECTED_ERRORf, 1);
        soc_reg_field_set(unit, CI_ERRORr, &rval, WFIFO_CTL_UNCORRECTED_ERRORf, 1);
        soc_reg_field_set(unit, CI_ERRORr, &rval, RFIFO_CTL_CORRECTED_ERRORf, 1);
        soc_reg_field_set(unit, CI_ERRORr, &rval, RFIFO_CTL_UNCORRECTED_ERRORf, 1);
        soc_reg_field_set(unit, CI_ERRORr, &rval, PHY_BL0_RD_FIFO_ERRORf, 1);
        soc_reg_field_set(unit, CI_ERRORr, &rval, PHY_BL1_RD_FIFO_ERRORf, 1);
        soc_reg_field_set(unit, CI_ERRORr, &rval, RFIFO_OVERFLOWf, 1);
        soc_reg_field_set(unit, CI_ERRORr, &rval, WFIFO_OVERFLOWf, 1);
        soc_reg_field_set(unit, CI_ERRORr, &rval, PHY_READY_EVENTf, 1);
        if(_check_dram(ciC)) {
            SOC_IF_ERROR_RETURN(WRITE_CI_ERROR_MASKr(unit, ciC, error_mask0));
            SOC_IF_ERROR_RETURN(WRITE_CI_ERRORr(unit, ciC, rval));
        }
        if(_check_dram(ciC + 1)) {
            SOC_IF_ERROR_RETURN(WRITE_CI_ERROR_MASKr(unit, ciC + 1, error_mask1));
            SOC_IF_ERROR_RETURN(WRITE_CI_ERRORr(unit, ciC + 1, rval));
        }
    }

    return SOC_E_NONE;
}


#endif                          /* DDR3 Support */
