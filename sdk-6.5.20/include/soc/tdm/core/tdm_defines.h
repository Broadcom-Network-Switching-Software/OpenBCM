/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM macro values for core scheduler
 */

#ifndef TDM_PREPROCESSOR_MACROS_H
#define TDM_PREPROCESSOR_MACROS_H

#define TDM_AUX_SIZE 128
#define PM_SORT_STACK_SIZE 8

/* Minimum freq in MHz at which all HG speeds are supported */
#define MIN_HG_FREQ 545

/* Max clk frquency in MHz */
#define MAX_FREQ 850

/* Granularity of TDM main calender in 100Mbps */
#define BW_QUANTA 25

/* Max size between speed vectors that can be the same, i.e. 40G vs 42G */
#define SPEED_DELIMITER 2000

/* Max number of internal opcodes */
#define NUM_OP_FLAGS 1

/* Min same port spacing (LLS scheduler req) */
#define LLS_MIN_SPACING 11
/* Min sister port spacing (VBS scheduler req) */
#define VBS_MIN_SPACING 4

/* Encap types */
#define PM_ENCAP__HIGIG2 999
#define PM_ENCAP__ETHRNT 998

/* Amount to relax the check of oversub token smoothness */
#define OS_RANGE_MARGIN 0
/* range for oversub smoothness as ratio with line rate slots */
#define OS_RANGE_BW_HI 1 /* OS>=200G */
#define OS_RANGE_BW_MD 2 /* OS<200G and OS>=100G */
/* #define OS_RANGE_BW_LO (USE SAME VALUE AS LR) */

/* TDM length based on frequency */
/* TH2 */
#define LEN_1700MHZ_HG 430
#define LEN_1700MHZ_EN 448
#define LEN_1625MHZ_HG 411
#define LEN_1625MHZ_EN 429
#define LEN_1525MHZ_HG 386
#define LEN_1525MHZ_EN 402
#define LEN_1425MHZ_HG 361
#define LEN_1425MHZ_EN 376
#define LEN_1325MHZ_HG 335
#define LEN_1325MHZ_EN 349
#define LEN_1275MHZ_HG 323
#define LEN_1275MHZ_EN 336
#define LEN_1225MHZ_HG 310
#define LEN_1225MHZ_EN 323
#define LEN_1133MHZ_HG 287
#define LEN_1133MHZ_EN 299
#define LEN_1125MHZ_HG 285
#define LEN_1125MHZ_EN 297
#define LEN_1050MHZ_HG 266
#define LEN_1050MHZ_EN 277
#define LEN_950MHZ_HG 240
#define LEN_950MHZ_EN 250
/* TD2, TD2+, TH */
#define LEN_850MHZ_HG 215
#define LEN_850MHZ_EN 224
#define LEN_765MHZ_HG 194
#define LEN_765MHZ_EN 202
#define LEN_672MHZ_HG 170
#define LEN_672MHZ_EN 177
#define LEN_645MHZ_HG 163
#define LEN_645MHZ_EN 170
#define LEN_545MHZ_HG 138
#define LEN_545MHZ_EN 143
#define LEN_760MHZ_HG 200
#define LEN_760MHZ_EN 200
#define LEN_608MHZ_HG 160
#define LEN_608MHZ_EN 160
#define LEN_517MHZ_HG 136
#define LEN_517MHZ_EN 136
#define LEN_415MHZ_HG 106
#define LEN_415MHZ_EN 106
/* Apache */
#define LEN_933MHZ_HG 233
#define LEN_933MHZ_EN 233
#define LEN_793MHZ_HG 200
#define LEN_793MHZ_EN 200
#define LEN_575MHZ_HG 145
#define LEN_575MHZ_EN 145
#define LEN_510MHZ_HG 129
#define LEN_510MHZ_EN 129
#define LEN_435MHZ_HG 110
#define LEN_435MHZ_EN 110
#define LEN_840MHZ_HG 212
#define LEN_840MHZ_EN 212

/* Length of TDM vector map - max length of TDM memory */
#define VEC_MAP_LEN 512
/* Width of TDM vector map - max number of vectors to reduce */
#define VEC_MAP_WID 64
/* Scan sees blank row or column */
#define BLANK 12345

/* Port tokens */
#define TOKEN_BASE 130
#define TOKEN_120G TOKEN_BASE+119
#define TOKEN_106G TOKEN_BASE+106
#define TOKEN_100G TOKEN_BASE+100
#define TOKEN_40G TOKEN_BASE+40
#define TOKEN_50G TOKEN_BASE+50
#define TOKEN_25G TOKEN_BASE+25
#define TOKEN_20G TOKEN_BASE+20
#define TOKEN_12G TOKEN_BASE+12
#define TOKEN_10G TOKEN_BASE+11
#define TOKEN_1G TOKEN_BASE+120

/* Max supported number of oversub speed groups */
#define OS_GROUP_NUM 8

/* 850MHz spacing quanta */
#define VECTOR_QUANTA_F 5
#define VECTOR_QUANTA_S 4
#define P_100G_HG_850MHZ 5
#define P_50G_HG_850MHZ 10
#define P_40G_HG_850MHZ 12
#define P_25G_HG_850MHZ 20
#define P_20G_HG_850MHZ 25
#define P_10G_HG_850MHZ 50

/* E2E CT Analysis Jitter Limits */
#define VECTOR_ISOLATION 0
#define JITTER_THRESH_LO 1 /* For 100G */
#define JITTER_THRESH_ML 2 /* For 50G, 40G */
#define JITTER_THRESH_MH 3 /* For 25G, 20G */
#define JITTER_THRESH_HI 5 /* For 10G */

/* Vector scheduling repititions */
#define MAIN_SCHEDULER_PASSES 5

/* Filter sensitivity */
#define DEFAULT_STEP_SIZE 1
#define DITHER_SUBPASS_STEP_SIZE 2
#define DITHER_THRESHOLD 7
#define DITHER_PASS 10

/* number of max speed classes: must be >= TDM_SPEED_IDX_NUM */
#define MAX_SPEED_TYPES 32

/* C Checker Constants */
#define PKT_SIZE 145
#define PKT_PREAMBLE 8
#define PKT_IPG 12

#define TIMEOUT 1000

#define ARR_FIXED_LEN 107

#define UP -1
#define DN 1
#define BOOL_TRUE 1
#define PASS 1
#define BOOL_FALSE 0
#define FAIL 0
#define UNDEF 254

/* VMAP */
#define TDM_VMAP_TOKEN_UNUSED 0
#define TDM_VMAP_PM_LANES 32

/* stack usage: 5K limit */
#define _TDM_DB_STACK 1

/* misc */
#define TDM_DIR_UP 0
#define TDM_DIR_DN 1

/* PM mode: sorted speed ratio pattern (decrement) */
#define TDM_VMAP_PM_MODE_0_0_0_0 0
#define TDM_VMAP_PM_MODE_1_0_0_0 1
#define TDM_VMAP_PM_MODE_1_1_0_0 2
#define TDM_VMAP_PM_MODE_1_1_1_0 3
#define TDM_VMAP_PM_MODE_1_1_1_1 4
#define TDM_VMAP_PM_MODE_2_1_0_0 5
#define TDM_VMAP_PM_MODE_2_1_1_0 6
#define TDM_VMAP_PM_MODE_4_1_1_0 7


#endif /* TDM_PREPROCESSOR_MACROS_H */
