/*! \file bcmptm_bcm56780_a0_rm_alpm.c
 *
 * Chip specific functions for PTRM ALPM
 *
 * This file contains the chip specific functions for PTRM ALPM
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
#include <shr/shr_lmem_mgr.h>
#include <shr/shr_util.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/chip/bcm56780_a0_defs.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmlrd/bcmlrd_enum.h>
#include <bcmptm/bcmptm_internal.h>

#include "../rm_alpm/rm_alpm_common.h"
#include "bcmptm_rm_alpm_common.h"
#include "bcmptm_bcm56780_a0_rm_alpm.h"
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_conf.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMALPM
#define BCM56780_TCAM_DEPTH      512

#define BCM56780_FIXED_COMP_DATA_TYPE  DATA_REDUCED

#define BCM56780_PDD_APP_BIT     3
#define BCM56780_PDD_IPV_BIT     2
#define BCM56780_PDD_SRC_BIT     1
#define BCM56780_PDD_FULL_BIT    0

#define UNPAIR_T    MEMDB_TCAM_IFTA80_MEM0_0m_t
#define PAIR_T      MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_t

#define DATA_VIEW_T    MEMDB_TCAM_IFTA80_MEM0_0_DATA_ONLYm_t
#define TCAM_VIEW_T    MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_t


#define M_UNPAIR_T    MEMDB_TCAM_M_CTL_MEM0_0m_t
#define M_PAIR_T      MEMDB_TCAM_M_CTL_MEM0_0_MEM0_1m_t

#define M_DATA_VIEW_T    MEMDB_TCAM_M_CTL_MEM0_0_DATA_ONLYm_t
#define M_TCAM_VIEW_T    MEMDB_TCAM_M_CTL_MEM0_0_ONLYm_t

#define SUB_DB_PRIORITY_OVERRIDE    0
#define SUB_DB_PRIORITY_VRF         1
#define SUB_DB_PRIORITY_GLOBAL      2

#define KSHIFT_PATCH1(u, m, arg, v) ((v) + bcm56780_kshift(u, m, arg))
#define KSHIFT_PATCH2(u, m, arg, v) ((v) -=bcm56780_kshift(u, m, arg))

#define FMT_TCAM_KEY_FIELD_SET(_e, _f, _v)  \
    L3_DEFIP_TCAM_KEY_##_f##_SET(*_e, (_v))
#define FMT_TCAM_KEY_FIELD_GET(_e, _f, _v)  \
    (_v) = L3_DEFIP_TCAM_KEY_##_f##_GET(*_e)

#define FMT_ASSOC_DATA_FIELD_SET(_e, _t, _f, _v)  \
    ASSOC_DATA_##_t##_##_f##_SET(*_e, (_v))
#define FMT_ASSOC_DATA_FIELD_GET(_e, _t, _f, _v)  \
    (_v) = ASSOC_DATA_##_t##_##_f##_GET(*_e)

#define FMT_ALPM1_DATA_FIELD_SET(_f, _v)  \
    ALPM1_DATA_##_f##_SET(*a1_format, (_v))
#define FMT_ALPM1_DATA_FIELD_GET(_f, _v)  \
    (_v) = ALPM1_DATA_##_f##_GET(*a1_format)

#define FMT_ALPM2_DATA_FIELD_SET(_f, _v)  \
    ALPM2_DATA_##_f##_SET(*a2_format, (_v))
#define FMT_ALPM2_DATA_FIELD_GET(_f, _v)  \
    (_v) = ALPM2_DATA_##_f##_GET(*a2_format)

#define FMT_FIXED_DATA_FIELD_SET(_f, _v)  \
    FIXED_DATA_##_f##_SET(*fd_format, (_v))
#define FMT_FIXED_DATA_FIELD_GET(_f, _v)  \
    (_v) = FIXED_DATA_##_f##_GET(*fd_format)

#define PIVOT_FMT_FULL_FIELD_SET(_n, _f, _v)  \
    L3_DEFIP_ALPM_PIVOT_FMT##_n##_FULL_##_f##_SET(*bucket, (_v)[0])
#define PIVOT_FMT_FULL_FIELD_GET(_n, _f, _v)  \
    (_v)[0] = L3_DEFIP_ALPM_PIVOT_FMT##_n##_FULL_##_f##_GET(*bucket)
#define ROUTE_FMT_FULL_FIELD_SET(_n, _f, _v)  \
    L3_DEFIP_ALPM_ROUTE_FMT##_n##_FULL_##_f##_SET(*bucket, (_v)[0])
#define ROUTE_FMT_FULL_FIELD_GET(_n, _f, _v)  \
    (_v)[0] = L3_DEFIP_ALPM_ROUTE_FMT##_n##_FULL_##_f##_GET(*bucket)
#define PIVOT_FMT_FULL_FIELD_SET2(_n, _f, _v)  \
    L3_DEFIP_ALPM_PIVOT_FMT##_n##_FULL_##_f##_SET(*bucket, &(_v)[0])
#define PIVOT_FMT_FULL_FIELD_GET2(_n, _f, _v)  \
    L3_DEFIP_ALPM_PIVOT_FMT##_n##_FULL_##_f##_GET(*bucket, &(_v)[0])
#define ROUTE_FMT_FULL_FIELD_SET2(_n, _f, _v)  \
    L3_DEFIP_ALPM_ROUTE_FMT##_n##_FULL_##_f##_SET(*bucket, &(_v)[0])
#define ROUTE_FMT_FULL_FIELD_GET2(_n, _f, _v)  \
    L3_DEFIP_ALPM_ROUTE_FMT##_n##_FULL_##_f##_GET(*bucket, &(_v)[0])

#define PIVOT_FMT_FIELD_SET(_n, _f, _v)    \
    L3_DEFIP_ALPM_PIVOT_FMT##_n##_##_f##_SET(*format_buf, (_v)[0])
#define PIVOT_FMT_FIELD_GET(_n, _f, _v)    \
    (_v)[0] = L3_DEFIP_ALPM_PIVOT_FMT##_n##_##_f##_GET(*format_buf)
#define ROUTE_FMT_FIELD_SET(_n, _f, _v)    \
    L3_DEFIP_ALPM_ROUTE_FMT##_n##_##_f##_SET(*format_buf, (_v)[0])
#define ROUTE_FMT_FIELD_GET(_n, _f, _v)    \
    (_v)[0] = L3_DEFIP_ALPM_ROUTE_FMT##_n##_##_f##_GET(*format_buf)

#define PIVOT_FMT_FIELD_SET2(_n, _f, _v)    \
    L3_DEFIP_ALPM_PIVOT_FMT##_n##_##_f##_SET(*format_buf, &(_v)[0])
#define PIVOT_FMT_FIELD_GET2(_n, _f, _v)    \
    L3_DEFIP_ALPM_PIVOT_FMT##_n##_##_f##_GET(*format_buf, &(_v)[0])
#define ROUTE_FMT_FIELD_SET2(_n, _f, _v)    \
    L3_DEFIP_ALPM_ROUTE_FMT##_n##_##_f##_SET(*format_buf, &(_v)[0])
#define ROUTE_FMT_FIELD_GET2(_n, _f, _v)    \
    L3_DEFIP_ALPM_ROUTE_FMT##_n##_##_f##_GET(*format_buf, &(_v)[0])

#define UNPAIR_FIELD_SET(_f, _v)         \
    MEMDB_TCAM_IFTA80_MEM0_0m_##_f##_SET(*de, (_v))
#define UNPAIR_FIELD_SET2(_f, _v)        \
    MEMDB_TCAM_IFTA80_MEM0_0m_##_f##_SET(*de, (uint32_t *)&(_v))

#define PAIR_FIELD_SET(_f, _v)         \
    MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_##_f##_SET(*de, (_v))
#define PAIR_FIELD_SET2(_f, _v)        \
    MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_##_f##_SET(*de, (uint32_t *)&(_v))

#define UNPAIR_FIELD_GET(_f, _v)       \
    (_v) = MEMDB_TCAM_IFTA80_MEM0_0m_##_f##_GET(*de)
#define UNPAIR_FIELD_GET2(_f, _v)      \
    MEMDB_TCAM_IFTA80_MEM0_0m_##_f##_GET(*de, (uint32_t *)&(_v))

#define PAIR_FIELD_GET(_f, _v)     \
    (_v) = MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_##_f##_GET(*de)
#define PAIR_FIELD_GET2(_f, _v)     \
    MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_##_f##_GET(*de, (uint32_t *)&(_v))



/* TCAM_ONLY */
#define KEY_VIEW_FIELD_SET(_f, _v)         \
    MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_##_f##_SET(*de, (_v))
#define KEY_VIEW_FIELD_SET2(_f, _v)        \
    MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_##_f##_SET(*de, (uint32_t *)&(_v))

#define KEY_VIEW_FIELD_GET(_f, _v)       \
    (_v) = MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_##_f##_GET(*de)
#define KEY_VIEW_FIELD_GET2(_f, _v)      \
    MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_##_f##_GET(*de, (uint32_t *)&(_v))


/* TCAM_ONLY UPPER */
#define UPR_KEY_VIEW_FIELD_SET(_f, _v)         \
    MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_##_f##_SET(*de_upr, (_v))
#define UPR_KEY_VIEW_FIELD_SET2(_f, _v)        \
    MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_##_f##_SET(*de_upr, (uint32_t *)&(_v))

#define UPR_KEY_VIEW_FIELD_GET(_f, _v)       \
    (_v) = MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_##_f##_GET(*de_upr)
#define UPR_KEY_VIEW_FIELD_GET2(_f, _v)      \
    MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_##_f##_GET(*de_upr, (uint32_t *)&(_v))


/* DATA_ONLY */
#define DATA_VIEW_FIELD_SET(_f, _v)         \
    MEMDB_TCAM_IFTA80_MEM0_0_DATA_ONLYm_##_f##_SET(*de, (_v))
#define DATA_VIEW_FIELD_SET2(_f, _v)        \
    MEMDB_TCAM_IFTA80_MEM0_0_DATA_ONLYm_##_f##_SET(*de, (uint32_t *)&(_v))

#define DATA_VIEW_FIELD_GET(_f, _v)       \
    (_v) = MEMDB_TCAM_IFTA80_MEM0_0_DATA_ONLYm_##_f##_GET(*de)
#define DATA_VIEW_FIELD_GET2(_f, _v)      \
    MEMDB_TCAM_IFTA80_MEM0_0_DATA_ONLYm_##_f##_GET(*de, (uint32_t *)&(_v))



#define M_UNPAIR_FIELD_SET(_f, _v)         \
    MEMDB_TCAM_M_CTL_MEM0_0m_##_f##_SET(*mde, (_v))
#define M_UNPAIR_FIELD_SET2(_f, _v)        \
    MEMDB_TCAM_M_CTL_MEM0_0m_##_f##_SET(*mde, (uint32_t *)&(_v))

#define M_PAIR_FIELD_SET(_f, _v)         \
    MEMDB_TCAM_M_CTL_MEM0_0_MEM0_1m_##_f##_SET(*mde, (_v))
#define M_PAIR_FIELD_SET2(_f, _v)        \
    MEMDB_TCAM_M_CTL_MEM0_0_MEM0_1m_##_f##_SET(*mde, (uint32_t *)&(_v))

#define M_UNPAIR_FIELD_GET(_f, _v)       \
    (_v) = MEMDB_TCAM_M_CTL_MEM0_0m_##_f##_GET(*mde)
#define M_UNPAIR_FIELD_GET2(_f, _v)      \
    MEMDB_TCAM_M_CTL_MEM0_0m_##_f##_GET(*mde, (uint32_t *)&(_v))

#define M_PAIR_FIELD_GET(_f, _v)     \
    (_v) = MEMDB_TCAM_M_CTL_MEM0_0_MEM0_1m_##_f##_GET(*mde)
#define M_PAIR_FIELD_GET2(_f, _v)     \
    MEMDB_TCAM_M_CTL_MEM0_0_MEM0_1m_##_f##_GET(*mde, (uint32_t *)&(_v))



/* TCAM_ONLY */
#define M_KEY_VIEW_FIELD_SET(_f, _v)         \
    MEMDB_TCAM_M_CTL_MEM0_0_ONLYm_##_f##_SET(*mde, (_v))
#define M_KEY_VIEW_FIELD_SET2(_f, _v)        \
    MEMDB_TCAM_M_CTL_MEM0_0_ONLYm_##_f##_SET(*mde, (uint32_t *)&(_v))

#define M_KEY_VIEW_FIELD_GET(_f, _v)       \
    (_v) = MEMDB_TCAM_M_CTL_MEM0_0_ONLYm_##_f##_GET(*mde)
#define M_KEY_VIEW_FIELD_GET2(_f, _v)      \
    MEMDB_TCAM_M_CTL_MEM0_0_ONLYm_##_f##_GET(*mde, (uint32_t *)&(_v))


/* TCAM_ONLY UPPER */
#define M_UPR_KEY_VIEW_FIELD_SET(_f, _v)         \
    MEMDB_TCAM_M_CTL_MEM0_0_ONLYm_##_f##_SET(*mde_upr, (_v))
#define M_UPR_KEY_VIEW_FIELD_SET2(_f, _v)        \
    MEMDB_TCAM_M_CTL_MEM0_0_ONLYm_##_f##_SET(*mde_upr, (uint32_t *)&(_v))

#define M_UPR_KEY_VIEW_FIELD_GET(_f, _v)       \
    (_v) = MEMDB_TCAM_M_CTL_MEM0_0_ONLYm_##_f##_GET(*mde_upr)
#define M_UPR_KEY_VIEW_FIELD_GET2(_f, _v)      \
    MEMDB_TCAM_M_CTL_MEM0_0_ONLYm_##_f##_GET(*mde_upr, (uint32_t *)&(_v))


/* DATA_ONLY */
#define M_DATA_VIEW_FIELD_SET(_f, _v)         \
    MEMDB_TCAM_M_CTL_MEM0_0_DATA_ONLYm_##_f##_SET(*mde, (_v))
#define M_DATA_VIEW_FIELD_SET2(_f, _v)        \
    MEMDB_TCAM_M_CTL_MEM0_0_DATA_ONLYm_##_f##_SET(*mde, (uint32_t *)&(_v))

#define M_DATA_VIEW_FIELD_GET(_f, _v)       \
    (_v) = MEMDB_TCAM_M_CTL_MEM0_0_DATA_ONLYm_##_f##_GET(*mde)
#define M_DATA_VIEW_FIELD_GET2(_f, _v)      \
    MEMDB_TCAM_M_CTL_MEM0_0_DATA_ONLYm_##_f##_GET(*mde, (uint32_t *)&(_v))



/* L3_DEFIP_TCAM_KEY_t */
#define FMT_TCAM_KEY_FIELD_SET(_e, _f, _v)  L3_DEFIP_TCAM_KEY_##_f##_SET(*_e, (_v))
#define FMT_TCAM_KEY_FIELD_GET(_e, _f, _v)  (_v) = L3_DEFIP_TCAM_KEY_##_f##_GET(*_e)
#define FMT_TCAM_KEY_FIELD_SET2(_e, _f, _v) L3_DEFIP_TCAM_KEY_##_f##_SET(*_e, (uint32_t *)&(_v)[0])
#define FMT_TCAM_KEY_FIELD_GET2(_e, _f, _v) L3_DEFIP_TCAM_KEY_##_f##_GET(*_e, (uint32_t *)&(_v)[0])

#define HALF_WIDE   2
#define SINGLE_WIDE 0
#define DOUBLE_WIDE 1



#define UNPAIR_TCAM_COUNT           16
#define UNPAIR_IDX_TCAM_ONLY_VIEW   0
#define UNPAIR_IDX_DATA_ONLY_VIEW   1
#define UNPAIR_IDX_COMBINED_VIEW    2
#define UNPAIR_VIEW_CNT             3

#define PAIR_TCAM_COUNT             8
#define PAIR_IDX_TCAM0_ONLY_VIEW    0
#define PAIR_IDX_TCAM1_ONLY_VIEW    1
#define PAIR_IDX_DATA_ONLY_VIEW     2
#define PAIR_IDX_COMBINED_VIEW      3
#define PAIR_VIEW_CNT               4

#define UNPAIR_TO_PAIR_TCAM_NUM(tcam_num) (((tcam_num >> 1) & ~0x1) | (tcam_num & 0x1))
#define UNPAIR_TO_PAIR_TCAM_NUM_MTOP(tcam_num) (tcam_num >> 1)

/*******************************************************************************
 * Typedefs
 */

typedef struct format_info_s {
    uint8_t prefix_len;
    uint8_t full;
    uint8_t num_entries;
} format_info_t;

typedef union bcm56780_assoc_data_s {
    ASSOC_DATA_100_t full_3;
    ASSOC_DATA_72_t full_2;
    ASSOC_DATA_49_t full;
    ASSOC_DATA_16_t red;
    uint32_t v[4];
} bcm56780_assoc_data_t;

/*******************************************************************************
 * Private variables
 */
extern shr_lmm_hdl_t bcmptm_rm_alpm_arg_lmem[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
static uint64_t bcm56780_narrow_mode[BCMDRD_CONFIG_MAX_UNITS] = {0};
static uint64_t bcm56780_wide_mode[BCMDRD_CONFIG_MAX_UNITS] = {0};
static uint64_t bcm56780_wide2_mode[BCMDRD_CONFIG_MAX_UNITS] = {0};
static uint64_t bcm56780_wide3_mode[BCMDRD_CONFIG_MAX_UNITS] = {0};

static int bcm56780_alpm_max_vrf[BCMDRD_CONFIG_MAX_UNITS] = {0};
static int bcm56780_alpm_max_comp_port[BCMDRD_CONFIG_MAX_UNITS] = {0};

static uint8_t bcm56780_l2_bucket_bits[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
static uint8_t bcm56780_l3_bucket_bits[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
static uint32_t bcm56780_l2_bucket_mask[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
static uint32_t bcm56780_l3_bucket_mask[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
static uint8_t bcm56780_lpm_v4_key_type[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
static uint8_t bcm56780_lpm_v6_key_type[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
static uint8_t bcm56780_comp_v4_key_type[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
static uint8_t bcm56780_comp_v6_key_type[BCMDRD_CONFIG_MAX_UNITS][ALPMS];

static uint8_t bcm56780_max_dt[BCMDRD_CONFIG_MAX_UNITS] = {DATA_REDUCED};

/* Mapping logical formats to physical formats. */
static int bcm56780_l2p_route_fmts[] =
    {0, 13, 1, 2, 3, 4, 5, 6, 7, 8, 9, 14, 10, 11, 12, 15, 16, 17, 18, 19, 20,
     21, 22, 23, 24};

/* Mapping physical formats to logical formats. */
static int bcm56780_p2l_route_fmts[] =
    {0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 13, 14, 1, 11, 15, 16, 17, 18, 19, 20,
     21, 22, 23, 24};

static format_info_t bcm56780_route_base_entry_info[] = {
    {.prefix_len = 0,   .full = 0, .num_entries = 0},   /* 0 (L) - 0 (P) */
    {.prefix_len = 8,   .full = 0, .num_entries = 20},  /* 1 - 13*/
    {.prefix_len = 16,  .full = 0, .num_entries = 15},  /* 2 - 1 */
    {.prefix_len = 24,  .full = 0, .num_entries = 12},  /* 3 - 2 */
    {.prefix_len = 32,  .full = 0, .num_entries = 10},  /* 4 - 3 */
    {.prefix_len = 40,  .full = 0, .num_entries = 8},   /* 5 - 4 */
    {.prefix_len = 48,  .full = 0, .num_entries = 7},   /* 6 - 5 */
    {.prefix_len = 64,  .full = 0, .num_entries = 6},   /* 7 - 6 */
    {.prefix_len = 80,  .full = 0, .num_entries = 5},   /* 8 - 7 */
    {.prefix_len = 104, .full = 0, .num_entries = 4},   /* 9 - 8 */
    {.prefix_len = 144, .full = 0, .num_entries = 3},   /* 10 - 9  */
    {.prefix_len = 185, .full = 0, .num_entries = 2},   /* 11 - 14 */
    {.prefix_len = 31,  .full = 1, .num_entries = 6},   /* 12 - 10 */
    {.prefix_len = 71,  .full = 1, .num_entries = 4},   /* 13 - 11 */
    {.prefix_len = 185, .full = 1, .num_entries = 2},   /* 14 - 12 */
    {.prefix_len = 111, .full = 1, .num_entries = 3},   /* 15 - 15 */
    {.prefix_len = 24,  .full = 2, .num_entries = 5},   /* 16 - 16 */
    {.prefix_len = 48,  .full = 2, .num_entries = 4},   /* 17 - 17 */
    {.prefix_len = 88,  .full = 2, .num_entries = 3},   /* 18 - 18 */
    {.prefix_len = 168, .full = 2, .num_entries = 2},   /* 19 - 19 */
    {.prefix_len = 185, .full = 2, .num_entries = 1},   /* 20 - 20 */
    {.prefix_len = 20,  .full = 3, .num_entries = 4},   /* 21 - 21 */
    {.prefix_len = 60,  .full = 3, .num_entries = 3},   /* 22 - 22 */
    {.prefix_len = 140, .full = 3, .num_entries = 2},   /* 23 - 23 */
    {.prefix_len = 185, .full = 3, .num_entries = 1}    /* 24 - 24 */
};


/* Mapping logical formats to physical formats. */
static int bcm56780_l2p_pivot_fmts[] =
    {0, 1, 2, 3, 4, 5, 9, 6, 7, 8, 10, 16, 17, 18, 19, 20, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/* Mapping physical formats to logical formats. */
static int bcm56780_p2l_pivot_fmts[] =
    {0, 1, 2, 3, 4, 5, 7, 8, 9, 6, 10, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 11, 12, 13, 14, 15};


static format_info_t bcm56780_pivot_base_entry_info[] = {
    {.prefix_len = 0,   .full = 0, .num_entries = 0}, /* 0 (L) - 0 (P) */
    {.prefix_len = 5,   .full = 0, .num_entries = 6}, /* 1 - 1 */
    {.prefix_len = 21,  .full = 0, .num_entries = 5}, /* 2 - 2 */
    {.prefix_len = 45,  .full = 0, .num_entries = 4}, /* 3 - 3 */
    {.prefix_len = 85,  .full = 0, .num_entries = 3}, /* 4 - 4 */
    {.prefix_len = 165, .full = 0, .num_entries = 2}, /* 5 - 5 */
    {.prefix_len = 185, .full = 0, .num_entries = 1}, /* 6 - 9 */
    {.prefix_len = 12,  .full = 1, .num_entries = 4}, /* 7 - 6 */
    {.prefix_len = 52,  .full = 1, .num_entries = 3}, /* 8 - 7 */
    {.prefix_len = 132, .full = 1, .num_entries = 2}, /* 9 - 8 */
    {.prefix_len = 185, .full = 1, .num_entries = 1}, /* 10 - 10 */
    {.prefix_len = 29,  .full = 2, .num_entries = 3}, /* 11 - 16 */
    {.prefix_len = 109, .full = 2, .num_entries = 2}, /* 12 - 17 */
    {.prefix_len = 185, .full = 2, .num_entries = 1}, /* 13 - 18 */
    {.prefix_len = 81,  .full = 3, .num_entries = 2}, /* 14 - 19 */
    {.prefix_len = 185, .full = 3, .num_entries = 1}  /* 15 - 20 */
    /* 16 - 11 */
    /* 17 - 12 */
    /* 18 - 13 */
    /* 19 - 14 */
    /* 20 - 15 */
};

static
bcmdrd_sid_t bcm56780_unpair_sid[UNPAIR_TCAM_COUNT][UNPAIR_VIEW_CNT] = {
    {
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm,
        MEMDB_TCAM_IFTA80_MEM0_0_DATA_ONLYm,
        MEMDB_TCAM_IFTA80_MEM0_0m,
    },
    {
        MEMDB_TCAM_IFTA80_MEM1_0_ONLYm,
        MEMDB_TCAM_IFTA80_MEM1_0_DATA_ONLYm,
        MEMDB_TCAM_IFTA80_MEM1_0m,
    },
    {
        MEMDB_TCAM_IFTA80_MEM0_1_ONLYm,
        MEMDB_TCAM_IFTA80_MEM0_1_DATA_ONLYm,
        MEMDB_TCAM_IFTA80_MEM0_1m,
    },
    {
        MEMDB_TCAM_IFTA80_MEM1_1_ONLYm,
        MEMDB_TCAM_IFTA80_MEM1_1_DATA_ONLYm,
        MEMDB_TCAM_IFTA80_MEM1_1m,
    },
    {
        MEMDB_TCAM_IFTA80_MEM2_0_ONLYm,
        MEMDB_TCAM_IFTA80_MEM2_0_DATA_ONLYm,
        MEMDB_TCAM_IFTA80_MEM2_0m,
    },
    {
        MEMDB_TCAM_IFTA80_MEM3_0_ONLYm,
        MEMDB_TCAM_IFTA80_MEM3_0_DATA_ONLYm,
        MEMDB_TCAM_IFTA80_MEM3_0m,
    },
    {
        MEMDB_TCAM_IFTA80_MEM2_1_ONLYm,
        MEMDB_TCAM_IFTA80_MEM2_1_DATA_ONLYm,
        MEMDB_TCAM_IFTA80_MEM2_1m,
    },
    {
        MEMDB_TCAM_IFTA80_MEM3_1_ONLYm,
        MEMDB_TCAM_IFTA80_MEM3_1_DATA_ONLYm,
        MEMDB_TCAM_IFTA80_MEM3_1m,
    },
    {
        MEMDB_TCAM_IFTA80_MEM4_0_ONLYm,
        MEMDB_TCAM_IFTA80_MEM4_0_DATA_ONLYm,
        MEMDB_TCAM_IFTA80_MEM4_0m,
    },
    {
        MEMDB_TCAM_IFTA80_MEM5_0_ONLYm,
        MEMDB_TCAM_IFTA80_MEM5_0_DATA_ONLYm,
        MEMDB_TCAM_IFTA80_MEM5_0m,
    },
    {
        MEMDB_TCAM_IFTA80_MEM4_1_ONLYm,
        MEMDB_TCAM_IFTA80_MEM4_1_DATA_ONLYm,
        MEMDB_TCAM_IFTA80_MEM4_1m,
    },
    {
        MEMDB_TCAM_IFTA80_MEM5_1_ONLYm,
        MEMDB_TCAM_IFTA80_MEM5_1_DATA_ONLYm,
        MEMDB_TCAM_IFTA80_MEM5_1m,
    },
    {
        MEMDB_TCAM_IFTA80_MEM6_0_ONLYm,
        MEMDB_TCAM_IFTA80_MEM6_0_DATA_ONLYm,
        MEMDB_TCAM_IFTA80_MEM6_0m,
    },
    {
        MEMDB_TCAM_IFTA80_MEM7_0_ONLYm,
        MEMDB_TCAM_IFTA80_MEM7_0_DATA_ONLYm,
        MEMDB_TCAM_IFTA80_MEM7_0m,
    },
    {
        MEMDB_TCAM_IFTA80_MEM6_1_ONLYm,
        MEMDB_TCAM_IFTA80_MEM6_1_DATA_ONLYm,
        MEMDB_TCAM_IFTA80_MEM6_1m,
    },
    {
        MEMDB_TCAM_IFTA80_MEM7_1_ONLYm,
        MEMDB_TCAM_IFTA80_MEM7_1_DATA_ONLYm,
        MEMDB_TCAM_IFTA80_MEM7_1m,
    },
};

static
bcmdrd_sid_t bcm56780_pair_sid[PAIR_TCAM_COUNT][PAIR_VIEW_CNT] = {
    {
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm,
        MEMDB_TCAM_IFTA80_MEM0_1_ONLYm,
        MEMDB_TCAM_IFTA80_MEM0_1_DATA_ONLYm,
        MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m,
    },
    {
        MEMDB_TCAM_IFTA80_MEM1_0_ONLYm,
        MEMDB_TCAM_IFTA80_MEM1_1_ONLYm,
        MEMDB_TCAM_IFTA80_MEM1_1_DATA_ONLYm,
        MEMDB_TCAM_IFTA80_MEM1_0_MEM1_1m,
    },
    {
        MEMDB_TCAM_IFTA80_MEM2_0_ONLYm,
        MEMDB_TCAM_IFTA80_MEM2_1_ONLYm,
        MEMDB_TCAM_IFTA80_MEM2_1_DATA_ONLYm,
        MEMDB_TCAM_IFTA80_MEM2_0_MEM2_1m,
    },
    {
        MEMDB_TCAM_IFTA80_MEM3_0_ONLYm,
        MEMDB_TCAM_IFTA80_MEM3_1_ONLYm,
        MEMDB_TCAM_IFTA80_MEM3_1_DATA_ONLYm,
        MEMDB_TCAM_IFTA80_MEM3_0_MEM3_1m,
    },
    {
        MEMDB_TCAM_IFTA80_MEM4_0_ONLYm,
        MEMDB_TCAM_IFTA80_MEM4_1_ONLYm,
        MEMDB_TCAM_IFTA80_MEM4_1_DATA_ONLYm,
        MEMDB_TCAM_IFTA80_MEM4_0_MEM4_1m,
    },
    {
        MEMDB_TCAM_IFTA80_MEM5_0_ONLYm,
        MEMDB_TCAM_IFTA80_MEM5_1_ONLYm,
        MEMDB_TCAM_IFTA80_MEM5_1_DATA_ONLYm,
        MEMDB_TCAM_IFTA80_MEM5_0_MEM5_1m,
    },
    {
        MEMDB_TCAM_IFTA80_MEM6_0_ONLYm,
        MEMDB_TCAM_IFTA80_MEM6_1_ONLYm,
        MEMDB_TCAM_IFTA80_MEM6_1_DATA_ONLYm,
        MEMDB_TCAM_IFTA80_MEM6_0_MEM6_1m,
    },
    {
        MEMDB_TCAM_IFTA80_MEM7_0_ONLYm,
        MEMDB_TCAM_IFTA80_MEM7_1_ONLYm,
        MEMDB_TCAM_IFTA80_MEM7_1_DATA_ONLYm,
        MEMDB_TCAM_IFTA80_MEM7_0_MEM7_1m,
    },
};


static
bcmdrd_sid_t bcm56780_unpair_sid_mtop[UNPAIR_TCAM_COUNT][UNPAIR_VIEW_CNT] = {
    {
        MEMDB_TCAM_M_CTL_MEM0_0_ONLYm,
        MEMDB_TCAM_M_CTL_MEM0_0_DATA_ONLYm,
        MEMDB_TCAM_M_CTL_MEM0_0m,
    },
    {
        MEMDB_TCAM_M_CTL_MEM0_1_ONLYm,
        MEMDB_TCAM_M_CTL_MEM0_1_DATA_ONLYm,
        MEMDB_TCAM_M_CTL_MEM0_1m,
    },
    {
        MEMDB_TCAM_M_CTL_MEM1_0_ONLYm,
        MEMDB_TCAM_M_CTL_MEM1_0_DATA_ONLYm,
        MEMDB_TCAM_M_CTL_MEM1_0m,
    },
    {
        MEMDB_TCAM_M_CTL_MEM1_1_ONLYm,
        MEMDB_TCAM_M_CTL_MEM1_1_DATA_ONLYm,
        MEMDB_TCAM_M_CTL_MEM1_1m,
    },
    {
        MEMDB_TCAM_M_CTL_MEM2_0_ONLYm,
        MEMDB_TCAM_M_CTL_MEM2_0_DATA_ONLYm,
        MEMDB_TCAM_M_CTL_MEM2_0m,
    },
    {
        MEMDB_TCAM_M_CTL_MEM2_1_ONLYm,
        MEMDB_TCAM_M_CTL_MEM2_1_DATA_ONLYm,
        MEMDB_TCAM_M_CTL_MEM2_1m,
    },
    {
        MEMDB_TCAM_M_CTL_MEM3_0_ONLYm,
        MEMDB_TCAM_M_CTL_MEM3_0_DATA_ONLYm,
        MEMDB_TCAM_M_CTL_MEM3_0m,
    },
    {
        MEMDB_TCAM_M_CTL_MEM3_1_ONLYm,
        MEMDB_TCAM_M_CTL_MEM3_1_DATA_ONLYm,
        MEMDB_TCAM_M_CTL_MEM3_1m,
    },
    {
        MEMDB_TCAM_M_CTL_MEM4_0_ONLYm,
        MEMDB_TCAM_M_CTL_MEM4_0_DATA_ONLYm,
        MEMDB_TCAM_M_CTL_MEM4_0m,
    },
    {
        MEMDB_TCAM_M_CTL_MEM4_1_ONLYm,
        MEMDB_TCAM_M_CTL_MEM4_1_DATA_ONLYm,
        MEMDB_TCAM_M_CTL_MEM4_1m,
    },
    {
        MEMDB_TCAM_M_CTL_MEM5_0_ONLYm,
        MEMDB_TCAM_M_CTL_MEM5_0_DATA_ONLYm,
        MEMDB_TCAM_M_CTL_MEM5_0m,
    },
    {
        MEMDB_TCAM_M_CTL_MEM5_1_ONLYm,
        MEMDB_TCAM_M_CTL_MEM5_1_DATA_ONLYm,
        MEMDB_TCAM_M_CTL_MEM5_1m,
    },
    {
        MEMDB_TCAM_M_CTL_MEM6_0_ONLYm,
        MEMDB_TCAM_M_CTL_MEM6_0_DATA_ONLYm,
        MEMDB_TCAM_M_CTL_MEM6_0m,
    },
    {
        MEMDB_TCAM_M_CTL_MEM6_1_ONLYm,
        MEMDB_TCAM_M_CTL_MEM6_1_DATA_ONLYm,
        MEMDB_TCAM_M_CTL_MEM6_1m,
    },
    {
        MEMDB_TCAM_M_CTL_MEM7_0_ONLYm,
        MEMDB_TCAM_M_CTL_MEM7_0_DATA_ONLYm,
        MEMDB_TCAM_M_CTL_MEM7_0m,
    },
    {
        MEMDB_TCAM_M_CTL_MEM7_1_ONLYm,
        MEMDB_TCAM_M_CTL_MEM7_1_DATA_ONLYm,
        MEMDB_TCAM_M_CTL_MEM7_1m,
    },
};

static
bcmdrd_sid_t bcm56780_pair_sid_mtop[PAIR_TCAM_COUNT][PAIR_VIEW_CNT] = {
    {
        MEMDB_TCAM_M_CTL_MEM0_0_ONLYm,
        MEMDB_TCAM_M_CTL_MEM0_1_ONLYm,
        MEMDB_TCAM_M_CTL_MEM0_1_DATA_ONLYm,
        MEMDB_TCAM_M_CTL_MEM0_0_MEM0_1m,
    },
    {
        MEMDB_TCAM_M_CTL_MEM1_0_ONLYm,
        MEMDB_TCAM_M_CTL_MEM1_1_ONLYm,
        MEMDB_TCAM_M_CTL_MEM1_1_DATA_ONLYm,
        MEMDB_TCAM_M_CTL_MEM1_0_MEM1_1m,
    },
    {
        MEMDB_TCAM_M_CTL_MEM2_0_ONLYm,
        MEMDB_TCAM_M_CTL_MEM2_1_ONLYm,
        MEMDB_TCAM_M_CTL_MEM2_1_DATA_ONLYm,
        MEMDB_TCAM_M_CTL_MEM2_0_MEM2_1m,
    },
    {
        MEMDB_TCAM_M_CTL_MEM3_0_ONLYm,
        MEMDB_TCAM_M_CTL_MEM3_1_ONLYm,
        MEMDB_TCAM_M_CTL_MEM3_1_DATA_ONLYm,
        MEMDB_TCAM_M_CTL_MEM3_0_MEM3_1m,
    },
    {
        MEMDB_TCAM_M_CTL_MEM4_0_ONLYm,
        MEMDB_TCAM_M_CTL_MEM4_1_ONLYm,
        MEMDB_TCAM_M_CTL_MEM4_1_DATA_ONLYm,
        MEMDB_TCAM_M_CTL_MEM4_0_MEM4_1m,
    },
    {
        MEMDB_TCAM_M_CTL_MEM5_0_ONLYm,
        MEMDB_TCAM_M_CTL_MEM5_1_ONLYm,
        MEMDB_TCAM_M_CTL_MEM5_1_DATA_ONLYm,
        MEMDB_TCAM_M_CTL_MEM5_0_MEM5_1m,
    },
    {
        MEMDB_TCAM_M_CTL_MEM6_0_ONLYm,
        MEMDB_TCAM_M_CTL_MEM6_1_ONLYm,
        MEMDB_TCAM_M_CTL_MEM6_1_DATA_ONLYm,
        MEMDB_TCAM_M_CTL_MEM6_0_MEM6_1m,
    },
    {
        MEMDB_TCAM_M_CTL_MEM7_0_ONLYm,
        MEMDB_TCAM_M_CTL_MEM7_1_ONLYm,
        MEMDB_TCAM_M_CTL_MEM7_1_DATA_ONLYm,
        MEMDB_TCAM_M_CTL_MEM7_0_MEM7_1m,
    },
};

/*******************************************************************************
 * Private Functions
 */
static int
bcm56780_npl_kt_to_ipv(int u, int m, int npl_kt)
{
    if (npl_kt == bcm56780_lpm_v4_key_type[u][m] ||
        npl_kt == bcm56780_comp_v4_key_type[u][m]) {
        return IP_VER_4;
    } else {
        return IP_VER_6;
    }
}

static void
bcm56780_l1_lpm_vt_decode(int u, int m, int sub_prio, int vrf,
                          int vrf_mask, alpm_arg_t *arg)
{
    if (sub_prio == SUB_DB_PRIORITY_OVERRIDE) {
        ALPM_ASSERT(vrf_mask == 0 || !arg->valid);
        arg->key.vt = VRF_OVERRIDE;
    } else if (sub_prio == SUB_DB_PRIORITY_GLOBAL) {
        ALPM_ASSERT(arg->lpm_large_vrf || vrf_mask == 0 || !arg->valid);
        arg->key.vt = VRF_GLOBAL;
    } else {
        ALPM_ASSERT(arg->lpm_large_vrf || vrf_mask == bcm56780_alpm_max_vrf[u] ||
                    !arg->valid);
        ALPM_ASSERT(arg->lpm_large_vrf || vrf <= bcm56780_alpm_max_vrf[u] ||
                    !arg->valid);
        arg->key.vt = VRF_PRIVATE;
        if (!arg->lpm_large_vrf) {
            arg->key.t.w_vrf = vrf;
        }
    }
}


static void
bcm56780_l1_comp_vt_decode(int u, int m, int sub_prio, int vrf,
                           int vrf_mask, alpm_arg_t *arg)
{
    if (arg->comp_large_port && sub_prio != SUB_DB_PRIORITY_OVERRIDE) {
        arg->key.vt = VRF_PRIVATE;
    } else {
        if (vrf_mask == bcm56780_alpm_max_comp_port[u]) {
            arg->key.vt = VRF_PRIVATE;
            arg->key.t.w_vrf = vrf;
        } else {
            arg->key.vt = VRF_GLOBAL;
        }
    }
}


static void
bcm56780_l1_key_lpm_v4_encode(int u, int m, int vrf, int key_mode,
                              int npl_kt, uint32_t ip_addr,
                              L3_DEFIP_TCAM_KEY_t *kdata)
{
    const bcmptm_alpm_variant_info_t *info;
    LPM_GENERIC_KEY_t lpm_v4 = {{0}};
    L3_DEFIP_TCAM_KEY_t v4_key = {{0}};
    uint32_t *key = &v4_key.v[0];


    info = bcmptm_rm_alpm_variant_get(u, m);
    bcmdrd_field_set((uint32_t *) &lpm_v4.v[0],
                     info->lpm_ipv4->ip_addr_start_bit,
                     info->lpm_ipv4->ip_addr_end_bit,
                     &ip_addr);
    bcmdrd_field32_set((uint32_t *) &lpm_v4.v[0],
                       info->lpm_ipv4->misc_start_bit,
                       info->lpm_ipv4->misc_end_bit,
                       vrf);

    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_HALF_BASE_ENTRY_0f_GET(lpm_v4, key);

    FMT_TCAM_KEY_FIELD_SET(kdata, KEY_MODEf, key_mode);
    FMT_TCAM_KEY_FIELD_SET(kdata, KEY_TYPEf, npl_kt);
    FMT_TCAM_KEY_FIELD_SET2(kdata, KEYf, key);
}

static void
bcm56780_l1_key_lpm_v4_encode_no_vrf(int u, int m, int key_mode,
                                     int npl_kt, uint32_t *udata,
                                     L3_DEFIP_TCAM_KEY_t *kdata)
{
    LPM_GENERIC_KEY_t *lpm_v4 = (LPM_GENERIC_KEY_t *)udata;
    L3_DEFIP_TCAM_KEY_t v4_key = {{0}};
    uint32_t *key = &v4_key.v[0];

    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_HALF_BASE_ENTRY_0f_GET(*lpm_v4, key);

    FMT_TCAM_KEY_FIELD_SET(kdata, KEY_MODEf, key_mode);
    FMT_TCAM_KEY_FIELD_SET(kdata, KEY_TYPEf, npl_kt);
    FMT_TCAM_KEY_FIELD_SET2(kdata, KEYf, key);
}


static void
bcm56780_l1_key_lpm_v6_double_encode(int u, int m, int vrf, int key_mode,
                                     int npl_kt, uint32_t *ip_addr,
                                     L3_DEFIP_TCAM_KEY_t *kdata0,
                                     L3_DEFIP_TCAM_KEY_t *kdata1,
                                     L3_DEFIP_TCAM_KEY_t *kdata2,
                                     L3_DEFIP_TCAM_KEY_t *kdata3
                                     )
{
    const bcmptm_alpm_variant_info_t *info;
    LPM_GENERIC_KEY_t lpm_v6 = {{0}};
    L3_DEFIP_TCAM_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];


    info = bcmptm_rm_alpm_variant_get(u, m);
    bcmdrd_field_set((uint32_t *) &lpm_v6.v[0],
                     info->lpm_ipv6->ip_addr_start_bit,
                     info->lpm_ipv6->ip_addr_end_bit,
                     ip_addr);
    bcmdrd_field32_set((uint32_t *) &lpm_v6.v[0],
                       info->lpm_ipv6->misc_start_bit,
                       info->lpm_ipv6->misc_end_bit,
                       vrf);

    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_0f_GET(lpm_v6, key);
    FMT_TCAM_KEY_FIELD_SET(kdata0, KEY_MODEf, key_mode);
    FMT_TCAM_KEY_FIELD_SET(kdata0, KEY_TYPEf, npl_kt);
    FMT_TCAM_KEY_FIELD_SET2(kdata0, KEYf, key);

    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_1f_GET(lpm_v6, key);
    FMT_TCAM_KEY_FIELD_SET(kdata1, KEY_MODEf, key_mode);
    FMT_TCAM_KEY_FIELD_SET(kdata1, KEY_TYPEf, npl_kt);
    FMT_TCAM_KEY_FIELD_SET2(kdata1, KEYf, key);

    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_2f_GET(lpm_v6, key);
    FMT_TCAM_KEY_FIELD_SET(kdata2, KEY_MODEf, key_mode);
    FMT_TCAM_KEY_FIELD_SET(kdata2, KEY_TYPEf, npl_kt);
    FMT_TCAM_KEY_FIELD_SET2(kdata2, KEYf, key);

    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_3f_GET(lpm_v6, key);
    FMT_TCAM_KEY_FIELD_SET(kdata3, KEY_MODEf, key_mode);
    FMT_TCAM_KEY_FIELD_SET(kdata3, KEY_TYPEf, npl_kt);
    FMT_TCAM_KEY_FIELD_SET2(kdata3, KEYf, key);
}


static void
bcm56780_l1_key_lpm_v6_double_encode_no_vrf(int u, int m, int key_mode,
                                            int npl_kt, uint32_t *udata,
                                            L3_DEFIP_TCAM_KEY_t *kdata0,
                                            L3_DEFIP_TCAM_KEY_t *kdata1,
                                            L3_DEFIP_TCAM_KEY_t *kdata2,
                                            L3_DEFIP_TCAM_KEY_t *kdata3
                                            )
{
    LPM_GENERIC_KEY_t *lpm_v6 = (LPM_GENERIC_KEY_t *)udata;
    L3_DEFIP_TCAM_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_0f_GET(*lpm_v6, key);
    FMT_TCAM_KEY_FIELD_SET(kdata0, KEY_MODEf, key_mode);
    FMT_TCAM_KEY_FIELD_SET(kdata0, KEY_TYPEf, npl_kt);
    FMT_TCAM_KEY_FIELD_SET2(kdata0, KEYf, key);

    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_1f_GET(*lpm_v6, key);
    FMT_TCAM_KEY_FIELD_SET(kdata1, KEY_MODEf, key_mode);
    FMT_TCAM_KEY_FIELD_SET(kdata1, KEY_TYPEf, npl_kt);
    FMT_TCAM_KEY_FIELD_SET2(kdata1, KEYf, key);

    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_2f_GET(*lpm_v6, key);
    FMT_TCAM_KEY_FIELD_SET(kdata2, KEY_MODEf, key_mode);
    FMT_TCAM_KEY_FIELD_SET(kdata2, KEY_TYPEf, npl_kt);
    FMT_TCAM_KEY_FIELD_SET2(kdata2, KEYf, key);

    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_3f_GET(*lpm_v6, key);
    FMT_TCAM_KEY_FIELD_SET(kdata3, KEY_MODEf, key_mode);
    FMT_TCAM_KEY_FIELD_SET(kdata3, KEY_TYPEf, npl_kt);
    FMT_TCAM_KEY_FIELD_SET2(kdata3, KEYf, key);
}



static void
bcm56780_l1_key_lpm_v6_single_encode(int u, int m, int vrf, int key_mode,
                                     int npl_kt, uint32_t *ip_addr,
                                     L3_DEFIP_TCAM_KEY_t *kdata0,
                                     L3_DEFIP_TCAM_KEY_t *kdata1
                                     )
{
    const bcmptm_alpm_variant_info_t *info;
    LPM_GENERIC_KEY_t lpm_v6 = {{0}};
    L3_DEFIP_TCAM_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    info = bcmptm_rm_alpm_variant_get(u, m);
    bcmdrd_field_set((uint32_t *) &lpm_v6.v[0],
                     info->lpm_ipv6->ip_addr_start_bit,
                     info->lpm_ipv6->ip_addr_end_bit,
                     ip_addr);
    bcmdrd_field32_set((uint32_t *) &lpm_v6.v[0],
                       info->lpm_ipv6->misc_start_bit,
                       info->lpm_ipv6->misc_end_bit,
                       vrf);

    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_SINGLE_BASE_ENTRY_0f_GET(lpm_v6, key);
    FMT_TCAM_KEY_FIELD_SET(kdata0, KEY_MODEf, key_mode);
    FMT_TCAM_KEY_FIELD_SET(kdata0, KEY_TYPEf, npl_kt);
    FMT_TCAM_KEY_FIELD_SET2(kdata0, KEYf, key);

    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_SINGLE_BASE_ENTRY_1f_GET(lpm_v6, key);
    FMT_TCAM_KEY_FIELD_SET(kdata1, KEY_MODEf, key_mode);
    FMT_TCAM_KEY_FIELD_SET(kdata1, KEY_TYPEf, npl_kt);
    FMT_TCAM_KEY_FIELD_SET2(kdata1, KEYf, key);
}


static void
bcm56780_l1_key_lpm_v6_single_encode_no_vrf(int u, int m, int key_mode,
                                            int npl_kt, uint32_t *udata,
                                            L3_DEFIP_TCAM_KEY_t *kdata0,
                                            L3_DEFIP_TCAM_KEY_t *kdata1
                                            )
{
    LPM_GENERIC_KEY_t *lpm_v6 = (LPM_GENERIC_KEY_t *) udata;
    L3_DEFIP_TCAM_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_SINGLE_BASE_ENTRY_0f_GET(*lpm_v6, key);
    FMT_TCAM_KEY_FIELD_SET(kdata0, KEY_MODEf, key_mode);
    FMT_TCAM_KEY_FIELD_SET(kdata0, KEY_TYPEf, npl_kt);
    FMT_TCAM_KEY_FIELD_SET2(kdata0, KEYf, key);

    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_SINGLE_BASE_ENTRY_1f_GET(*lpm_v6, key);
    FMT_TCAM_KEY_FIELD_SET(kdata1, KEY_MODEf, key_mode);
    FMT_TCAM_KEY_FIELD_SET(kdata1, KEY_TYPEf, npl_kt);
    FMT_TCAM_KEY_FIELD_SET2(kdata1, KEYf, key);
}



static void
bcm56780_l1_key_lpm_v6_half_encode(int u, int m, int vrf, int key_mode,
                                   int npl_kt, uint32_t *ip_addr,
                                   L3_DEFIP_TCAM_KEY_t *kdata)
{
    const bcmptm_alpm_variant_info_t *info;
    LPM_GENERIC_KEY_t lpm_v6 = {{0}};
    L3_DEFIP_TCAM_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];


    info = bcmptm_rm_alpm_variant_get(u, m);
    bcmdrd_field_set((uint32_t *) &lpm_v6.v[0],
                     info->lpm_ipv6->ip_addr_start_bit,
                     info->lpm_ipv6->ip_addr_end_bit,
                     ip_addr);
    bcmdrd_field32_set((uint32_t *) &lpm_v6.v[0],
                       info->lpm_ipv6->misc_start_bit,
                       info->lpm_ipv6->misc_end_bit,
                       vrf);

    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_HALF_BASE_ENTRY_0f_GET(lpm_v6, key);

    FMT_TCAM_KEY_FIELD_SET(kdata, KEY_MODEf, key_mode);
    FMT_TCAM_KEY_FIELD_SET(kdata, KEY_TYPEf, npl_kt);
    FMT_TCAM_KEY_FIELD_SET2(kdata, KEYf, key);
}

static void
bcm56780_l1_key_lpm_v6_half_encode_no_vrf(int u, int m, int key_mode,
                                          int npl_kt, uint32_t *udata,
                                          L3_DEFIP_TCAM_KEY_t *kdata)
{
    LPM_GENERIC_KEY_t *lpm_v6 = (LPM_GENERIC_KEY_t *)udata;
    L3_DEFIP_TCAM_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_HALF_BASE_ENTRY_0f_GET(*lpm_v6, key);

    FMT_TCAM_KEY_FIELD_SET(kdata, KEY_MODEf, key_mode);
    FMT_TCAM_KEY_FIELD_SET(kdata, KEY_TYPEf, npl_kt);
    FMT_TCAM_KEY_FIELD_SET2(kdata, KEYf, key);
}



static void
bcm56780_l1_key_comp_v4_single_encode(int u, int m, int port, int key_mode,
                                      int npl_kt, uint32_t *ip_addr,
                                      L3_DEFIP_TCAM_KEY_t *kdata0,
                                      L3_DEFIP_TCAM_KEY_t *kdata1
                                      )
{
    const bcmptm_alpm_variant_info_t *info;
    LPM_GENERIC_KEY_t comp_v4 = {{0}};
    L3_DEFIP_TCAM_KEY_t v4_key = {{0}};
    uint32_t *key = &v4_key.v[0];

    info = bcmptm_rm_alpm_variant_get(u, m);
    bcmdrd_field_set((uint32_t *) &comp_v4.v[0],
                     info->comp_ipv4->ip_addr_start_bit,
                     info->comp_ipv4->ip_addr_end_bit,
                     ip_addr);
    bcmdrd_field32_set((uint32_t *) &comp_v4.v[0],
                       info->comp_ipv4->misc_start_bit,
                       info->comp_ipv4->misc_end_bit,
                       port);

    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_SINGLE_BASE_ENTRY_0f_GET(comp_v4, key);
    FMT_TCAM_KEY_FIELD_SET(kdata0, KEY_MODEf, key_mode);
    FMT_TCAM_KEY_FIELD_SET(kdata0, KEY_TYPEf, npl_kt);
    FMT_TCAM_KEY_FIELD_SET2(kdata0, KEYf, key);

    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_SINGLE_BASE_ENTRY_1f_GET(comp_v4, key);
    FMT_TCAM_KEY_FIELD_SET(kdata1, KEY_MODEf, key_mode);
    FMT_TCAM_KEY_FIELD_SET(kdata1, KEY_TYPEf, npl_kt);
    FMT_TCAM_KEY_FIELD_SET2(kdata1, KEYf, key);
}


static void
bcm56780_l1_key_comp_v4_single_encode_no_port(int u, int m, int key_mode,
                                              int npl_kt, uint32_t *udata,
                                              L3_DEFIP_TCAM_KEY_t *kdata0,
                                              L3_DEFIP_TCAM_KEY_t *kdata1
                                              )
{
    LPM_GENERIC_KEY_t *comp_v4 = (LPM_GENERIC_KEY_t *) udata;
    L3_DEFIP_TCAM_KEY_t v4_key = {{0}};
    uint32_t *key = &v4_key.v[0];

    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_SINGLE_BASE_ENTRY_0f_GET(*comp_v4, key);
    FMT_TCAM_KEY_FIELD_SET(kdata0, KEY_MODEf, key_mode);
    FMT_TCAM_KEY_FIELD_SET(kdata0, KEY_TYPEf, npl_kt);
    FMT_TCAM_KEY_FIELD_SET2(kdata0, KEYf, key);

    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_SINGLE_BASE_ENTRY_1f_GET(*comp_v4, key);
    FMT_TCAM_KEY_FIELD_SET(kdata1, KEY_MODEf, key_mode);
    FMT_TCAM_KEY_FIELD_SET(kdata1, KEY_TYPEf, npl_kt);
    FMT_TCAM_KEY_FIELD_SET2(kdata1, KEYf, key);
}




static void
bcm56780_l1_key_comp_v6_double_encode(int u, int m, int port, int key_mode,
                                      int npl_kt, uint32_t *ip_addr,
                                      L3_DEFIP_TCAM_KEY_t *kdata0,
                                      L3_DEFIP_TCAM_KEY_t *kdata1,
                                      L3_DEFIP_TCAM_KEY_t *kdata2,
                                      L3_DEFIP_TCAM_KEY_t *kdata3
                                      )
{
    const bcmptm_alpm_variant_info_t *info;
    LPM_GENERIC_KEY_t comp_v6 = {{0}};
    L3_DEFIP_TCAM_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];


    info = bcmptm_rm_alpm_variant_get(u, m);
    bcmdrd_field_set((uint32_t *) &comp_v6.v[0],
                     info->comp_ipv6->ip_addr_start_bit,
                     info->comp_ipv6->ip_addr_end_bit,
                     ip_addr);
    bcmdrd_field32_set((uint32_t *) &comp_v6.v[0],
                       info->comp_ipv6->misc_start_bit,
                       info->comp_ipv6->misc_end_bit,
                       port);

    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_0f_GET(comp_v6, key);
    FMT_TCAM_KEY_FIELD_SET(kdata0, KEY_MODEf, key_mode);
    FMT_TCAM_KEY_FIELD_SET(kdata0, KEY_TYPEf, npl_kt);
    FMT_TCAM_KEY_FIELD_SET2(kdata0, KEYf, key);

    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_1f_GET(comp_v6, key);
    FMT_TCAM_KEY_FIELD_SET(kdata1, KEY_MODEf, key_mode);
    FMT_TCAM_KEY_FIELD_SET(kdata1, KEY_TYPEf, npl_kt);
    FMT_TCAM_KEY_FIELD_SET2(kdata1, KEYf, key);

    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_2f_GET(comp_v6, key);
    FMT_TCAM_KEY_FIELD_SET(kdata2, KEY_MODEf, key_mode);
    FMT_TCAM_KEY_FIELD_SET(kdata2, KEY_TYPEf, npl_kt);
    FMT_TCAM_KEY_FIELD_SET2(kdata2, KEYf, key);

    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_3f_GET(comp_v6, key);
    FMT_TCAM_KEY_FIELD_SET(kdata3, KEY_MODEf, key_mode);
    FMT_TCAM_KEY_FIELD_SET(kdata3, KEY_TYPEf, npl_kt);
    FMT_TCAM_KEY_FIELD_SET2(kdata3, KEYf, key);
}


static void
bcm56780_l1_key_comp_v6_double_encode_no_port(int u, int m, int key_mode,
                                              int npl_kt, uint32_t *udata,
                                              L3_DEFIP_TCAM_KEY_t *kdata0,
                                              L3_DEFIP_TCAM_KEY_t *kdata1,
                                              L3_DEFIP_TCAM_KEY_t *kdata2,
                                              L3_DEFIP_TCAM_KEY_t *kdata3
                                              )
{
    LPM_GENERIC_KEY_t *comp_v6 = (LPM_GENERIC_KEY_t *)udata;
    L3_DEFIP_TCAM_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_0f_GET(*comp_v6, key);
    FMT_TCAM_KEY_FIELD_SET(kdata0, KEY_MODEf, key_mode);
    FMT_TCAM_KEY_FIELD_SET(kdata0, KEY_TYPEf, npl_kt);
    FMT_TCAM_KEY_FIELD_SET2(kdata0, KEYf, key);

    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_1f_GET(*comp_v6, key);
    FMT_TCAM_KEY_FIELD_SET(kdata1, KEY_MODEf, key_mode);
    FMT_TCAM_KEY_FIELD_SET(kdata1, KEY_TYPEf, npl_kt);
    FMT_TCAM_KEY_FIELD_SET2(kdata1, KEYf, key);

    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_2f_GET(*comp_v6, key);
    FMT_TCAM_KEY_FIELD_SET(kdata2, KEY_MODEf, key_mode);
    FMT_TCAM_KEY_FIELD_SET(kdata2, KEY_TYPEf, npl_kt);
    FMT_TCAM_KEY_FIELD_SET2(kdata2, KEYf, key);

    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_3f_GET(*comp_v6, key);
    FMT_TCAM_KEY_FIELD_SET(kdata3, KEY_MODEf, key_mode);
    FMT_TCAM_KEY_FIELD_SET(kdata3, KEY_TYPEf, npl_kt);
    FMT_TCAM_KEY_FIELD_SET2(kdata3, KEYf, key);
}

static void
bcm56780_l1_key_lpm_v4_decode(int u, int m, int *vrf, int *key_mode,
                              int *kt, uint32_t *ip_addr,
                              L3_DEFIP_TCAM_KEY_t *kdata)
{
    const bcmptm_alpm_variant_info_t *info;
    LPM_GENERIC_KEY_t lpm_v4 = {{0}};
    L3_DEFIP_TCAM_KEY_t v4_key = {{0}};
    uint32_t *key = &v4_key.v[0];

    if (key_mode) {
        FMT_TCAM_KEY_FIELD_GET(kdata, KEY_MODEf, *key_mode);
    }
    if (kt) {
        FMT_TCAM_KEY_FIELD_GET(kdata, KEY_TYPEf, *kt);
    }
    FMT_TCAM_KEY_FIELD_GET2(kdata, KEYf, key);
    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_HALF_BASE_ENTRY_0f_SET(lpm_v4, key);

    info = bcmptm_rm_alpm_variant_get(u, m);
    if (ip_addr) {
        *ip_addr = bcmdrd_field32_get((uint32_t *) &lpm_v4.v[0],
                                      info->lpm_ipv4->ip_addr_start_bit,
                                      info->lpm_ipv4->ip_addr_end_bit);
    }
    if (vrf) {
        *vrf = bcmdrd_field32_get((uint32_t *) &lpm_v4.v[0],
                                  info->lpm_ipv4->misc_start_bit,
                                  info->lpm_ipv4->misc_end_bit);
    }
}

static void
bcm56780_l1_key_lpm_v4_decode_no_vrf(int u, int m, int *key_mode,
                                     int *kt, uint32_t *udata,
                                     L3_DEFIP_TCAM_KEY_t *kdata)
{
    LPM_GENERIC_KEY_t *lpm_v4 = (LPM_GENERIC_KEY_t *)udata;
    L3_DEFIP_TCAM_KEY_t v4_key = {{0}};
    uint32_t *key = &v4_key.v[0];

    if (key_mode) {
        FMT_TCAM_KEY_FIELD_GET(kdata, KEY_MODEf, *key_mode);
    }
    if (kt) {
        FMT_TCAM_KEY_FIELD_GET(kdata, KEY_TYPEf, *kt);
    }
    FMT_TCAM_KEY_FIELD_GET2(kdata, KEYf, key);
    if (lpm_v4) {
        LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_HALF_BASE_ENTRY_0f_SET(*lpm_v4, key);
    }

}



static void
bcm56780_l1_key_lpm_v6_double_decode(int u, int m, int *vrf, int *key_mode,
                                     int *kt, uint32_t *ip_addr,
                                     L3_DEFIP_TCAM_KEY_t *kdata0,
                                     L3_DEFIP_TCAM_KEY_t *kdata1,
                                     L3_DEFIP_TCAM_KEY_t *kdata2,
                                     L3_DEFIP_TCAM_KEY_t *kdata3)
{
    const bcmptm_alpm_variant_info_t *info;
    LPM_GENERIC_KEY_t lpm_v6 = {{0}};
    L3_DEFIP_TCAM_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    if (key_mode) {
        FMT_TCAM_KEY_FIELD_GET(kdata0, KEY_MODEf, *key_mode);
    }
    if (kt) {
        FMT_TCAM_KEY_FIELD_GET(kdata0, KEY_TYPEf, *kt);
    }
    FMT_TCAM_KEY_FIELD_GET2(kdata0, KEYf, key);
    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_0f_SET(lpm_v6, key);
    FMT_TCAM_KEY_FIELD_GET2(kdata1, KEYf, key);
    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_1f_SET(lpm_v6, key);
    FMT_TCAM_KEY_FIELD_GET2(kdata2, KEYf, key);
    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_2f_SET(lpm_v6, key);
    FMT_TCAM_KEY_FIELD_GET2(kdata3, KEYf, key);
    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_3f_SET(lpm_v6, key);

    info = bcmptm_rm_alpm_variant_get(u, m);
    if (ip_addr) {
        bcmdrd_field_get((uint32_t *) &lpm_v6.v[0],
                         info->lpm_ipv6->ip_addr_start_bit,
                         info->lpm_ipv6->ip_addr_end_bit,
                         ip_addr);

    }
    if (vrf) {
        *vrf = bcmdrd_field32_get((uint32_t *) &lpm_v6.v[0],
                                  info->lpm_ipv6->misc_start_bit,
                                  info->lpm_ipv6->misc_end_bit);
    }
}


static void
bcm56780_l1_key_lpm_v6_double_decode_no_vrf(int u, int m, int *key_mode,
                                            int *kt, uint32_t *udata,
                                            L3_DEFIP_TCAM_KEY_t *kdata0,
                                            L3_DEFIP_TCAM_KEY_t *kdata1,
                                            L3_DEFIP_TCAM_KEY_t *kdata2,
                                            L3_DEFIP_TCAM_KEY_t *kdata3)
{
    LPM_GENERIC_KEY_t *lpm_v6 = (LPM_GENERIC_KEY_t *)udata;
    L3_DEFIP_TCAM_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    if (key_mode) {
        FMT_TCAM_KEY_FIELD_GET(kdata0, KEY_MODEf, *key_mode);
    }
    if (kt) {
        FMT_TCAM_KEY_FIELD_GET(kdata0, KEY_TYPEf, *kt);
    }
    FMT_TCAM_KEY_FIELD_GET2(kdata0, KEYf, key);
    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_0f_SET(*lpm_v6, key);
    FMT_TCAM_KEY_FIELD_GET2(kdata1, KEYf, key);
    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_1f_SET(*lpm_v6, key);
    FMT_TCAM_KEY_FIELD_GET2(kdata2, KEYf, key);
    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_2f_SET(*lpm_v6, key);
    FMT_TCAM_KEY_FIELD_GET2(kdata3, KEYf, key);
    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_3f_SET(*lpm_v6, key);

}


static void
bcm56780_l1_key_lpm_v6_single_decode(int u, int m, int *vrf, int *key_mode,
                                     int *kt, uint32_t *ip_addr,
                                     L3_DEFIP_TCAM_KEY_t *kdata0,
                                     L3_DEFIP_TCAM_KEY_t *kdata1)
{
    const bcmptm_alpm_variant_info_t *info;
    LPM_GENERIC_KEY_t lpm_v6 = {{0}};
    L3_DEFIP_TCAM_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    if (key_mode) {
        FMT_TCAM_KEY_FIELD_GET(kdata0, KEY_MODEf, *key_mode);
    }
    if (kt) {
        FMT_TCAM_KEY_FIELD_GET(kdata0, KEY_TYPEf, *kt);
    }
    FMT_TCAM_KEY_FIELD_GET2(kdata0, KEYf, key);
    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_SINGLE_BASE_ENTRY_0f_SET(lpm_v6, key);
    FMT_TCAM_KEY_FIELD_GET2(kdata1, KEYf, key);
    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_SINGLE_BASE_ENTRY_1f_SET(lpm_v6, key);
    info = bcmptm_rm_alpm_variant_get(u, m);
    if (ip_addr) {
        bcmdrd_field_get((uint32_t *) &lpm_v6.v[0],
                         info->lpm_ipv6->ip_addr_start_bit,
                         info->lpm_ipv6->ip_addr_end_bit,
                         ip_addr);

    }
    if (vrf) {
        *vrf = bcmdrd_field32_get((uint32_t *) &lpm_v6.v[0],
                                  info->lpm_ipv6->misc_start_bit,
                                  info->lpm_ipv6->misc_end_bit);
    }
}


static void
bcm56780_l1_key_lpm_v6_single_decode_no_vrf(int u, int m, int *key_mode,
                                            int *kt, uint32_t *udata,
                                            L3_DEFIP_TCAM_KEY_t *kdata0,
                                            L3_DEFIP_TCAM_KEY_t *kdata1)
{

    LPM_GENERIC_KEY_t *lpm_v6 = (LPM_GENERIC_KEY_t *)udata;
    L3_DEFIP_TCAM_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    if (key_mode) {
        FMT_TCAM_KEY_FIELD_GET(kdata0, KEY_MODEf, *key_mode);
    }
    if (kt) {
        FMT_TCAM_KEY_FIELD_GET(kdata0, KEY_TYPEf, *kt);
    }
    FMT_TCAM_KEY_FIELD_GET2(kdata0, KEYf, key);
    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_SINGLE_BASE_ENTRY_0f_SET(*lpm_v6, key);
    FMT_TCAM_KEY_FIELD_GET2(kdata1, KEYf, key);
    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_SINGLE_BASE_ENTRY_1f_SET(*lpm_v6, key);
}


static void
bcm56780_l1_key_lpm_v6_half_decode(int u, int m, int *vrf, int *key_mode,
                                   int *kt, uint32_t *ip_addr,
                                   L3_DEFIP_TCAM_KEY_t *kdata)
{
    const bcmptm_alpm_variant_info_t *info;
    LPM_GENERIC_KEY_t lpm_v6 = {{0}};
    L3_DEFIP_TCAM_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    if (key_mode) {
        FMT_TCAM_KEY_FIELD_GET(kdata, KEY_MODEf, *key_mode);
    }
    if (kt) {
        FMT_TCAM_KEY_FIELD_GET(kdata, KEY_TYPEf, *kt);
    }
    FMT_TCAM_KEY_FIELD_GET2(kdata, KEYf, key);
    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_HALF_BASE_ENTRY_0f_SET(lpm_v6, key);

    info = bcmptm_rm_alpm_variant_get(u, m);
    if (ip_addr) {
        bcmdrd_field_get((uint32_t *) &lpm_v6.v[0],
                         info->lpm_ipv6->ip_addr_start_bit,
                         info->lpm_ipv6->ip_addr_end_bit,
                         ip_addr);
    }
    if (vrf) {
        *vrf = bcmdrd_field32_get((uint32_t *) &lpm_v6.v[0],
                                  info->lpm_ipv6->misc_start_bit,
                                  info->lpm_ipv6->misc_end_bit);
    }
}

static void
bcm56780_l1_key_lpm_v6_half_decode_no_vrf(int u, int m, int *key_mode,
                                          int *kt, uint32_t *udata,
                                          L3_DEFIP_TCAM_KEY_t *kdata)
{
    LPM_GENERIC_KEY_t *lpm_v6 = (LPM_GENERIC_KEY_t *)udata;
    L3_DEFIP_TCAM_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    if (key_mode) {
        FMT_TCAM_KEY_FIELD_GET(kdata, KEY_MODEf, *key_mode);
    }
    if (kt) {
        FMT_TCAM_KEY_FIELD_GET(kdata, KEY_TYPEf, *kt);
    }
    FMT_TCAM_KEY_FIELD_GET2(kdata, KEYf, key);
    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_HALF_BASE_ENTRY_0f_SET(*lpm_v6, key);

}


static void
bcm56780_l1_key_comp_v4_single_decode(int u, int m, int *port, int *key_mode,
                                      int *kt, uint32_t *ip_addr,
                                      L3_DEFIP_TCAM_KEY_t *kdata0,
                                      L3_DEFIP_TCAM_KEY_t *kdata1)
{
    const bcmptm_alpm_variant_info_t *info;
    LPM_GENERIC_KEY_t comp_v4 = {{0}};
    L3_DEFIP_TCAM_KEY_t v4_key = {{0}};
    uint32_t *key = &v4_key.v[0];

    if (key_mode) {
        FMT_TCAM_KEY_FIELD_GET(kdata0, KEY_MODEf, *key_mode);
    }
    if (kt) {
        FMT_TCAM_KEY_FIELD_GET(kdata0, KEY_TYPEf, *kt);
    }
    FMT_TCAM_KEY_FIELD_GET2(kdata0, KEYf, key);
    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_SINGLE_BASE_ENTRY_0f_SET(comp_v4, key);
    FMT_TCAM_KEY_FIELD_GET2(kdata1, KEYf, key);
    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_SINGLE_BASE_ENTRY_1f_SET(comp_v4, key);
    info = bcmptm_rm_alpm_variant_get(u, m);
    if (ip_addr) {
        bcmdrd_field_get((uint32_t *) &comp_v4.v[0],
                         info->comp_ipv4->ip_addr_start_bit,
                         info->comp_ipv4->ip_addr_end_bit,
                         ip_addr);

    }
    if (port) {
        *port = bcmdrd_field32_get((uint32_t *) &comp_v4.v[0],
                                   info->comp_ipv4->misc_start_bit,
                                   info->comp_ipv4->misc_end_bit);
    }
}


static void
bcm56780_l1_key_comp_v4_single_decode_no_port(int u, int m, int *key_mode,
                                              int *kt, uint32_t *udata,
                                              L3_DEFIP_TCAM_KEY_t *kdata0,
                                              L3_DEFIP_TCAM_KEY_t *kdata1)
{

    LPM_GENERIC_KEY_t *comp_v4 = (LPM_GENERIC_KEY_t *)udata;
    L3_DEFIP_TCAM_KEY_t v4_key = {{0}};
    uint32_t *key = &v4_key.v[0];

    if (key_mode) {
        FMT_TCAM_KEY_FIELD_GET(kdata0, KEY_MODEf, *key_mode);
    }
    if (kt) {
        FMT_TCAM_KEY_FIELD_GET(kdata0, KEY_TYPEf, *kt);
    }
    FMT_TCAM_KEY_FIELD_GET2(kdata0, KEYf, key);
    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_SINGLE_BASE_ENTRY_0f_SET(*comp_v4, key);
    FMT_TCAM_KEY_FIELD_GET2(kdata1, KEYf, key);
    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_SINGLE_BASE_ENTRY_1f_SET(*comp_v4, key);
}


static void
bcm56780_l1_key_comp_v6_double_decode(int u, int m, int *port, int *key_mode,
                                      int *kt, uint32_t *ip_addr,
                                      L3_DEFIP_TCAM_KEY_t *kdata0,
                                      L3_DEFIP_TCAM_KEY_t *kdata1,
                                      L3_DEFIP_TCAM_KEY_t *kdata2,
                                      L3_DEFIP_TCAM_KEY_t *kdata3)
{
    const bcmptm_alpm_variant_info_t *info;
    LPM_GENERIC_KEY_t comp_v6 = {{0}};
    L3_DEFIP_TCAM_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    if (key_mode) {
        FMT_TCAM_KEY_FIELD_GET(kdata0, KEY_MODEf, *key_mode);
    }
    if (kt) {
        FMT_TCAM_KEY_FIELD_GET(kdata0, KEY_TYPEf, *kt);
    }
    FMT_TCAM_KEY_FIELD_GET2(kdata0, KEYf, key);
    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_0f_SET(comp_v6, key);
    FMT_TCAM_KEY_FIELD_GET2(kdata1, KEYf, key);
    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_1f_SET(comp_v6, key);
    FMT_TCAM_KEY_FIELD_GET2(kdata2, KEYf, key);
    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_2f_SET(comp_v6, key);
    FMT_TCAM_KEY_FIELD_GET2(kdata3, KEYf, key);
    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_3f_SET(comp_v6, key);

    info = bcmptm_rm_alpm_variant_get(u, m);
    if (ip_addr) {
        bcmdrd_field_get((uint32_t *) &comp_v6.v[0],
                         info->comp_ipv6->ip_addr_start_bit,
                         info->comp_ipv6->ip_addr_end_bit,
                         ip_addr);

    }
    if (port) {
        *port = bcmdrd_field32_get((uint32_t *) &comp_v6.v[0],
                                   info->comp_ipv6->misc_start_bit,
                                   info->comp_ipv6->misc_end_bit);
    }
}


static void
bcm56780_l1_key_comp_v6_double_decode_no_port(int u, int m, int *key_mode,
                                              int *kt, uint32_t *udata,
                                              L3_DEFIP_TCAM_KEY_t *kdata0,
                                              L3_DEFIP_TCAM_KEY_t *kdata1,
                                              L3_DEFIP_TCAM_KEY_t *kdata2,
                                              L3_DEFIP_TCAM_KEY_t *kdata3)
{
    LPM_GENERIC_KEY_t *comp_v6 = (LPM_GENERIC_KEY_t *)udata;
    L3_DEFIP_TCAM_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    if (key_mode) {
        FMT_TCAM_KEY_FIELD_GET(kdata0, KEY_MODEf, *key_mode);
    }
    if (kt) {
        FMT_TCAM_KEY_FIELD_GET(kdata0, KEY_TYPEf, *kt);
    }
    FMT_TCAM_KEY_FIELD_GET2(kdata0, KEYf, key);
    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_0f_SET(*comp_v6, key);
    FMT_TCAM_KEY_FIELD_GET2(kdata1, KEYf, key);
    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_1f_SET(*comp_v6, key);
    FMT_TCAM_KEY_FIELD_GET2(kdata2, KEYf, key);
    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_2f_SET(*comp_v6, key);
    FMT_TCAM_KEY_FIELD_GET2(kdata3, KEYf, key);
    LPM_GENERIC_KEY_L3_DEFIP_TCAM_KEY_DOUBLE_BASE_ENTRY_3f_SET(*comp_v6, key);

}

static void
bcm56780_assoc_data_format_encode(int u, int m, bcm56780_assoc_data_t *buf,
                                  uint8_t app,
                                  int full, assoc_data_t *assoc)
{
    uint8_t i, fld_count;
    const alpm_config_t *config;
    const bcmptm_alpm_variant_info_t *info;
    const pdd_field_info_t *pdd_info;
    uint32_t dest, dest_type;

    ALPM_ASSERT(sizeof(*buf) <= sizeof(assoc->fmt_ad));

    if (full && app == APP_LPM) {
        config = bcmptm_rm_alpm_config(u, m);
        info = bcmptm_rm_alpm_variant_get(u, m);
        /* Propagate correct destination_type */
        if (assoc->user_data_type == DATA_REDUCED) {
        /* Assuming that pdd_info[0]->fld is destination. [1] is dest_type. */
            dest = assoc->fmt_ad[0];
            if ((dest & config->destination_mask) == config->destination) {
                dest_type = config->destination_type_match;
            } else {
                dest_type = config->destination_type_non_match;
            }
            assoc->fmt_ad[1] = dest_type;
            fld_count = 2;
        } else {
            fld_count = info->l3uc_pdd_info_count;
        }
        /* Use DATA_FULL format to encode */
        assert(bcm56780_max_dt[u] == DATA_FULL);
        sal_memset(&buf->full, 0, sizeof(buf->full));

        pdd_info = info->l3uc_pdd_info;
        for (i = 0; i < fld_count; i++) {
            bcmdrd_field_set(&buf->v[0],
                             pdd_info->start_bit,
                             pdd_info->end_bit,
                             &assoc->fmt_ad[i]);
            pdd_info++;
        }
    } else {
        /*
         * Reduced user type to reduced actual type goes here, assuming
         * reducued buffer has nothing but destination - fmt_ad[0].
         */
        sal_memcpy(buf, &assoc->fmt_ad, sizeof(*buf));
    }
}

static inline void
bcm56780_assoc_data_format_decode(int u, int m, bcm56780_assoc_data_t *buf,
                                  uint8_t app,
                                  int full,
                                  assoc_data_t *assoc)
{
    uint8_t i;
    const bcmptm_alpm_variant_info_t *info;
    const pdd_field_info_t *pdd_info;
    ALPM_ASSERT(sizeof(*buf) <= sizeof(assoc->fmt_ad));


    if (full && app == APP_LPM) {
        /* Use DATA_FULL format to decode */
        assert(bcm56780_max_dt[u] == DATA_FULL);

        info = bcmptm_rm_alpm_variant_get(u, m);
        pdd_info = info->l3uc_pdd_info;
        for (i = 0; i < info->l3uc_pdd_info_count; i++) {
            /* Assuming that pdd_info[0]->fld is destination. */
            assoc->fmt_ad[i] = bcmdrd_field32_get(&buf->v[0],
                                                  pdd_info->start_bit,
                                                  pdd_info->end_bit);
            pdd_info++;
        }

    } else {
        sal_memcpy(&assoc->fmt_ad, buf, sizeof(*buf));
    }
}

static inline void
bcm56780_fixed_data_format_decode(int u, int m, void *buf, uint8_t *sub_db_prio,
                                  alpm_arg_t *arg)
{
    uint32_t data_type;
    FIXED_DATA_t *fd_format = buf;
    int decode_dt;

    /* strength profile index fixed per-LT */
    FMT_FIXED_DATA_FIELD_GET(SBR_PROFILE_INDEXf, arg->profile_index);
    FMT_FIXED_DATA_FIELD_GET(DATA_TYPEf, data_type);
    if (sub_db_prio) {
        FMT_FIXED_DATA_FIELD_GET(SUB_DB_PRIORITYf, *sub_db_prio);
    }
    if (arg->decode_lt_view) {
        ALPM_ASSERT(data_type == 0 || data_type == 1 ||
                    data_type == 2 || data_type == 3);
        data_type &= 0x3;
        if (data_type == bcm56780_narrow_mode[u]) {
            decode_dt = DATA_REDUCED;
        } else if (data_type == bcm56780_wide_mode[u]) {
            decode_dt = DATA_FULL;
        } else if (data_type == bcm56780_wide2_mode[u]) {
            decode_dt = DATA_FULL_2;
        } else {
            decode_dt = DATA_FULL_3;
        }
        arg->ad.user_data_type = decode_dt;
    }
    /* else {
     *    data_type &= 0x3;
     *    if (data_type == 0) {
     *        decode_dt = DATA_REDUCED;
     *    } else if (data_type == data_types[0x1 << BCM56780_PDD_FULL_BIT]) {
     *        decode_dt = DATA_FULL;
     *    } else if (data_type == data_types[0x2 << BCM56780_PDD_FULL_BIT]) {
     *        decode_dt = DATA_FULL_2;
     *    } else {
     *        decode_dt = DATA_FULL_3;
     *    }
     * }
     */
}

static void
bcm56780_sbr_profile_index_get(int u, int m, alpm_arg_t *arg)
{
    int ipv6 = arg->key.t.ipv;
    uint8_t app = arg->key.t.app;
    alpm_vrf_type_t vt = arg->key.vt;
    uint8_t sbr;
    const alpm_config_t *config = bcmptm_rm_alpm_config(u, m);

    if (ipv6) {
        if (vt == VRF_OVERRIDE) {
            sbr = config->ipv6_uc_override_sbr;
        } else if (vt == VRF_PRIVATE) {
            sbr = config->ipv6_uc_vrf_sbr;
            if (app == APP_COMP0) {
                sbr = config->ipv6_comp_sbr;
            }
        } else {
            sbr = config->ipv6_uc_sbr;
            if (app == APP_COMP0) {
                sbr = config->ipv6_comp_sbr;
            }
        }
    } else {
        if (vt == VRF_OVERRIDE) {
            sbr = config->ipv4_uc_override_sbr;
        } else if (vt == VRF_PRIVATE) {
            sbr = config->ipv4_uc_vrf_sbr;
            if (app == APP_COMP0) {
                sbr = config->ipv4_comp_sbr;
            }
        } else {
            sbr = config->ipv4_uc_sbr;
            if (app == APP_COMP0) {
                sbr = config->ipv4_comp_sbr;
            }
        }
    }

    arg->profile_index = sbr;
}

static void
bcm56780_fixed_data_format_encode(int u, int m, void *buf, uint8_t sub_db_prio,
                                  alpm_arg_t *arg)
{
    bool is_src;
    int index;
    const bcmptm_alpm_variant_info_t *info = bcmptm_rm_alpm_variant_get(u, m);
    const uint8_t *data_types = info->pdd_data_types_array;
    FIXED_DATA_t *fd_format = buf;
    int encode_dt;

    FIXED_DATA_CLR(*fd_format);
    FMT_FIXED_DATA_FIELD_SET(SUB_DB_PRIORITYf, sub_db_prio);
    /* Get arg->profile_index from arg->key */
    bcm56780_sbr_profile_index_get(u, m, arg);
    FMT_FIXED_DATA_FIELD_SET(SBR_PROFILE_INDEXf, arg->profile_index);

    if (arg->encode_lt_view) {
        encode_dt = arg->ad.user_data_type;
        if (encode_dt == DATA_REDUCED) {
            FMT_FIXED_DATA_FIELD_SET(DATA_TYPEf, bcm56780_narrow_mode[u]);
        } else if (encode_dt == DATA_FULL) {
            FMT_FIXED_DATA_FIELD_SET(DATA_TYPEf, bcm56780_wide_mode[u]);
        } else if (encode_dt == DATA_FULL_2) {
            FMT_FIXED_DATA_FIELD_SET(DATA_TYPEf, bcm56780_wide2_mode[u]);
        } else {
            FMT_FIXED_DATA_FIELD_SET(DATA_TYPEf, bcm56780_wide3_mode[u]);
        }
    } else {
        is_src = bcmptm_rm_alpm_db_is_src(u, m, arg->db);

        index = (arg->key.t.app == APP_COMP0) << BCM56780_PDD_APP_BIT |
                !!arg->key.t.ipv << BCM56780_PDD_IPV_BIT |
                is_src << BCM56780_PDD_SRC_BIT;

        if (bcm56780_max_dt[u] > DATA_FULL) {
            index <<= 1;
        }
        /* Always set FULL for DATA_TYPE[0] for LPM. FP_COMP0 should be REDUCED */
        if (arg->key.t.app == APP_COMP0) {
            index |= 0;
        } else {
            index |= bcm56780_max_dt[u] << BCM56780_PDD_FULL_BIT;
        }

        FMT_FIXED_DATA_FIELD_SET(DATA_TYPEf, *(data_types + index));
    }
}

static int
bcm56780_kshift(int u, int m, alpm_arg_t *arg)
{
    int kshift = 0;
    const bcmptm_alpm_variant_info_t *info = bcmptm_rm_alpm_variant_get(u, m);

    if (arg->key.t.app == APP_LPM) {
        if (arg->key.t.ipv == IP_VER_4) {
            if (arg->lpm_large_vrf) {
                kshift = info->lpm_ipv4->end_bit;
            } else {
                kshift = info->lpm_ipv4->ip_addr_end_bit;
            }
        } else {
            if (arg->lpm_large_vrf) {
                kshift = info->lpm_ipv6->end_bit;
            } else {
                kshift = info->lpm_ipv6->ip_addr_end_bit;
            }
        }
    } else {
        if (arg->key.t.ipv == IP_VER_4) {
            if (arg->comp_large_port) {
                kshift = info->comp_ipv4->end_bit;
            } else {
                kshift = info->comp_ipv4->ip_addr_end_bit;
            }
        } else {
            if (arg->comp_large_port) {
                kshift = info->comp_ipv6->end_bit;
            } else {
                kshift = info->comp_ipv6->ip_addr_end_bit;
            }
        }
    }
    return (183 - kshift);
}

static void
bcm56780_ln_alpm2_data_format_encode(int u, int m, alpm_arg_t *arg,
                                     ALPM2_DATA_t *a2_format,
                                     alpm2_data_t *a2_data)
{
    ALPM2_DATA_CLR(*a2_format);
    FMT_ALPM2_DATA_FIELD_SET(BKT_PTRf, a2_data->phy_bkt);
    FMT_ALPM2_DATA_FIELD_SET(DEFAULT_MISSf, a2_data->default_miss);
    FMT_ALPM2_DATA_FIELD_SET(KSHIFTf, KSHIFT_PATCH1(u, m, arg, a2_data->kshift));
    FMT_ALPM2_DATA_FIELD_SET(ROFSf, a2_data->rofs);
    FMT_ALPM2_DATA_FIELD_SET(FMT0f, bcm56780_l2p_route_fmts[a2_data->fmt[0]]);
    FMT_ALPM2_DATA_FIELD_SET(FMT1f, bcm56780_l2p_route_fmts[a2_data->fmt[1]]);
    FMT_ALPM2_DATA_FIELD_SET(FMT2f, bcm56780_l2p_route_fmts[a2_data->fmt[2]]);
    FMT_ALPM2_DATA_FIELD_SET(FMT3f, bcm56780_l2p_route_fmts[a2_data->fmt[3]]);
    FMT_ALPM2_DATA_FIELD_SET(FMT4f, bcm56780_l2p_route_fmts[a2_data->fmt[4]]);
    FMT_ALPM2_DATA_FIELD_SET(FMT5f, bcm56780_l2p_route_fmts[a2_data->fmt[5]]);
    FMT_ALPM2_DATA_FIELD_SET(FMT6f, bcm56780_l2p_route_fmts[a2_data->fmt[6]]);
    FMT_ALPM2_DATA_FIELD_SET(FMT7f, bcm56780_l2p_route_fmts[a2_data->fmt[7]]);

}


static void
bcm56780_ln_alpm2_data_format_decode(int u, int m,
                                     alpm_arg_t *arg,
                                     ALPM2_DATA_t *a2_format,
                                     alpm2_data_t *a2_data)
{
    int i;

    FMT_ALPM2_DATA_FIELD_GET(BKT_PTRf, a2_data->phy_bkt);
    FMT_ALPM2_DATA_FIELD_GET(DEFAULT_MISSf, a2_data->default_miss);
    FMT_ALPM2_DATA_FIELD_GET(KSHIFTf, a2_data->kshift);
    FMT_ALPM2_DATA_FIELD_GET(ROFSf, a2_data->rofs);
    FMT_ALPM2_DATA_FIELD_GET(FMT0f, a2_data->fmt[0]);
    FMT_ALPM2_DATA_FIELD_GET(FMT1f, a2_data->fmt[1]);
    FMT_ALPM2_DATA_FIELD_GET(FMT2f, a2_data->fmt[2]);
    FMT_ALPM2_DATA_FIELD_GET(FMT3f, a2_data->fmt[3]);
    FMT_ALPM2_DATA_FIELD_GET(FMT4f, a2_data->fmt[4]);
    FMT_ALPM2_DATA_FIELD_GET(FMT5f, a2_data->fmt[5]);
    FMT_ALPM2_DATA_FIELD_GET(FMT6f, a2_data->fmt[6]);
    FMT_ALPM2_DATA_FIELD_GET(FMT7f, a2_data->fmt[7]);
    KSHIFT_PATCH2(u, m, arg, a2_data->kshift);
    for (i = 0; i < ALPM2_DATA_FORMATS; i++) {
        a2_data->fmt[i] = bcm56780_p2l_route_fmts[a2_data->fmt[i]];
    }
}

static void
bcm56780_ln_alpm1_data_format_encode(int u, int m,
                                     alpm_arg_t *arg,
                                     void *buf, alpm1_data_t *a1_data)
{
    ALPM1_DATA_t *a1_format = buf;

    ALPM1_DATA_CLR(*a1_format);
    FMT_ALPM1_DATA_FIELD_SET(BKT_PTRf, a1_data->phy_bkt);
    FMT_ALPM1_DATA_FIELD_SET(DEFAULT_MISSf, a1_data->default_miss);
    FMT_ALPM1_DATA_FIELD_SET(DIRECT_ROUTEf, a1_data->direct_route);
    FMT_ALPM1_DATA_FIELD_SET(KSHIFTf, KSHIFT_PATCH1(u, m, arg, a1_data->kshift));
    FMT_ALPM1_DATA_FIELD_SET(ROFSf, a1_data->rofs);
    if (a1_data->is_route) {
        FMT_ALPM1_DATA_FIELD_SET(FMT0f, bcm56780_l2p_route_fmts[a1_data->fmt[0]]);
        FMT_ALPM1_DATA_FIELD_SET(FMT1f, bcm56780_l2p_route_fmts[a1_data->fmt[1]]);
        FMT_ALPM1_DATA_FIELD_SET(FMT2f, bcm56780_l2p_route_fmts[a1_data->fmt[2]]);
        FMT_ALPM1_DATA_FIELD_SET(FMT3f, bcm56780_l2p_route_fmts[a1_data->fmt[3]]);
        FMT_ALPM1_DATA_FIELD_SET(FMT4f, bcm56780_l2p_route_fmts[a1_data->fmt[4]]);
        FMT_ALPM1_DATA_FIELD_SET(FMT5f, bcm56780_l2p_route_fmts[a1_data->fmt[5]]);
    } else {
        FMT_ALPM1_DATA_FIELD_SET(FMT0f, bcm56780_l2p_pivot_fmts[a1_data->fmt[0]]);
        FMT_ALPM1_DATA_FIELD_SET(FMT1f, bcm56780_l2p_pivot_fmts[a1_data->fmt[1]]);
        FMT_ALPM1_DATA_FIELD_SET(FMT2f, bcm56780_l2p_pivot_fmts[a1_data->fmt[2]]);
        FMT_ALPM1_DATA_FIELD_SET(FMT3f, bcm56780_l2p_pivot_fmts[a1_data->fmt[3]]);
        FMT_ALPM1_DATA_FIELD_SET(FMT4f, bcm56780_l2p_pivot_fmts[a1_data->fmt[4]]);
        FMT_ALPM1_DATA_FIELD_SET(FMT5f, bcm56780_l2p_pivot_fmts[a1_data->fmt[5]]);
    }
}

static void
bcm56780_ln_alpm1_data_format_decode(int u, int m,
                                     alpm_arg_t *arg,
                                     void *buf, alpm1_data_t *a1_data)
{
    int i;
    ALPM1_DATA_t *a1_format = buf;
    FMT_ALPM1_DATA_FIELD_GET(BKT_PTRf, a1_data->phy_bkt);
    FMT_ALPM1_DATA_FIELD_GET(DEFAULT_MISSf, a1_data->default_miss);
    FMT_ALPM1_DATA_FIELD_GET(DIRECT_ROUTEf, a1_data->direct_route);
    FMT_ALPM1_DATA_FIELD_GET(KSHIFTf, a1_data->kshift);
    FMT_ALPM1_DATA_FIELD_GET(ROFSf, a1_data->rofs);
    FMT_ALPM1_DATA_FIELD_GET(FMT0f, a1_data->fmt[0]);
    FMT_ALPM1_DATA_FIELD_GET(FMT1f, a1_data->fmt[1]);
    FMT_ALPM1_DATA_FIELD_GET(FMT2f, a1_data->fmt[2]);
    FMT_ALPM1_DATA_FIELD_GET(FMT3f, a1_data->fmt[3]);
    FMT_ALPM1_DATA_FIELD_GET(FMT4f, a1_data->fmt[4]);
    FMT_ALPM1_DATA_FIELD_GET(FMT5f, a1_data->fmt[5]);
    KSHIFT_PATCH2(u, m, arg, a1_data->kshift);

    for (i = 0; i < ALPM1_DATA_FORMATS; i++) {
        if (a1_data->is_route) {
            a1_data->fmt[i] = bcm56780_p2l_route_fmts[a1_data->fmt[i]];
        } else {
            a1_data->fmt[i] = bcm56780_p2l_pivot_fmts[a1_data->fmt[i]];
            assert(a1_data->fmt[i] != 0xFF);
        }
    }

}

static void
bcm56780_ln_pivot_base_entry_info(int u, int m, uint8_t format_value,
                                  int *prefix_len,
                                  int *full,
                                  int *number)
{
    ALPM_ASSERT(format_value < COUNTOF(bcm56780_pivot_base_entry_info));
    *prefix_len = bcm56780_pivot_base_entry_info[format_value].prefix_len;
    *full = bcm56780_pivot_base_entry_info[format_value].full;
    *number = bcm56780_pivot_base_entry_info[format_value].num_entries;

    (*prefix_len)--; /* Exclude the extra bit */
}

static void
bcm56780_ln_pivot_format_full_encode2(int u, int m, uint8_t format_value,
                                      uint32_t *base_entry,
                                      int ent_ofs,
                                      void *buf)
{
    format_value = bcm56780_l2p_pivot_fmts[format_value];
    switch (format_value) {
    case 1: {
        L3_DEFIP_ALPM_PIVOT_FMT1_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_SET2(1, ENTRY_0f, base_entry); break;
        case 1: PIVOT_FMT_FULL_FIELD_SET2(1, ENTRY_1f, base_entry); break;
        case 2: PIVOT_FMT_FULL_FIELD_SET2(1, ENTRY_2f, base_entry); break;
        case 3: PIVOT_FMT_FULL_FIELD_SET2(1, ENTRY_3f, base_entry); break;
        case 4: PIVOT_FMT_FULL_FIELD_SET2(1, ENTRY_4f, base_entry); break;
        case 5: PIVOT_FMT_FULL_FIELD_SET2(1, ENTRY_5f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 2: {
        L3_DEFIP_ALPM_PIVOT_FMT2_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_SET2(2, ENTRY_0f, base_entry); break;
        case 1: PIVOT_FMT_FULL_FIELD_SET2(2, ENTRY_1f, base_entry); break;
        case 2: PIVOT_FMT_FULL_FIELD_SET2(2, ENTRY_2f, base_entry); break;
        case 3: PIVOT_FMT_FULL_FIELD_SET2(2, ENTRY_3f, base_entry); break;
        case 4: PIVOT_FMT_FULL_FIELD_SET2(2, ENTRY_4f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 3: {
        L3_DEFIP_ALPM_PIVOT_FMT3_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_SET2(3, ENTRY_0f, base_entry); break;
        case 1: PIVOT_FMT_FULL_FIELD_SET2(3, ENTRY_1f, base_entry); break;
        case 2: PIVOT_FMT_FULL_FIELD_SET2(3, ENTRY_2f, base_entry); break;
        case 3: PIVOT_FMT_FULL_FIELD_SET2(3, ENTRY_3f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 4: {
        L3_DEFIP_ALPM_PIVOT_FMT4_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_SET2(4, ENTRY_0f, base_entry); break;
        case 1: PIVOT_FMT_FULL_FIELD_SET2(4, ENTRY_1f, base_entry); break;
        case 2: PIVOT_FMT_FULL_FIELD_SET2(4, ENTRY_2f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 5: {
        L3_DEFIP_ALPM_PIVOT_FMT5_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_SET2(5, ENTRY_0f, base_entry); break;
        case 1: PIVOT_FMT_FULL_FIELD_SET2(5, ENTRY_1f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 6: {
        L3_DEFIP_ALPM_PIVOT_FMT6_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_SET2(6, ENTRY_0f, base_entry); break;
        case 1: PIVOT_FMT_FULL_FIELD_SET2(6, ENTRY_1f, base_entry); break;
        case 2: PIVOT_FMT_FULL_FIELD_SET2(6, ENTRY_2f, base_entry); break;
        case 3: PIVOT_FMT_FULL_FIELD_SET2(6, ENTRY_3f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 7: {
        L3_DEFIP_ALPM_PIVOT_FMT7_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_SET2(7, ENTRY_0f, base_entry); break;
        case 1: PIVOT_FMT_FULL_FIELD_SET2(7, ENTRY_1f, base_entry); break;
        case 2: PIVOT_FMT_FULL_FIELD_SET2(7, ENTRY_2f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 8: {
        L3_DEFIP_ALPM_PIVOT_FMT8_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_SET2(8, ENTRY_0f, base_entry); break;
        case 1: PIVOT_FMT_FULL_FIELD_SET2(8, ENTRY_1f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 9: {
        L3_DEFIP_ALPM_PIVOT_FMT9_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_SET2(9, ENTRY_0f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 10: {
        L3_DEFIP_ALPM_PIVOT_FMT10_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_SET2(10, ENTRY_0f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 16: {
        L3_DEFIP_ALPM_PIVOT_FMT16_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_SET2(16, ENTRY_0f, base_entry); break;
        case 1: PIVOT_FMT_FULL_FIELD_SET2(16, ENTRY_1f, base_entry); break;
        case 2: PIVOT_FMT_FULL_FIELD_SET2(16, ENTRY_2f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 17: {
        L3_DEFIP_ALPM_PIVOT_FMT17_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_SET2(17, ENTRY_0f, base_entry); break;
        case 1: PIVOT_FMT_FULL_FIELD_SET2(17, ENTRY_1f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 18: {
        L3_DEFIP_ALPM_PIVOT_FMT18_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_SET2(18, ENTRY_0f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 19: {
        L3_DEFIP_ALPM_PIVOT_FMT19_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_SET2(19, ENTRY_0f, base_entry); break;
        case 1: PIVOT_FMT_FULL_FIELD_SET2(19, ENTRY_1f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 20: {
        L3_DEFIP_ALPM_PIVOT_FMT20_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_SET2(20, ENTRY_0f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
}


static void
bcm56780_ln_pivot_format_full_encode(int u, int m, uint8_t format_value,
                                     ln_base_entry_t *base_entry,
                                     int entry_count,
                                     void *buf)
{
    format_value = bcm56780_l2p_pivot_fmts[format_value];
    switch (format_value) {
    case 1: {
        L3_DEFIP_ALPM_PIVOT_FMT1_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_SET2(1, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_SET2(1, ENTRY_1f, base_entry[1]);
        PIVOT_FMT_FULL_FIELD_SET2(1, ENTRY_2f, base_entry[2]);
        PIVOT_FMT_FULL_FIELD_SET2(1, ENTRY_3f, base_entry[3]);
        PIVOT_FMT_FULL_FIELD_SET2(1, ENTRY_4f, base_entry[4]);
        PIVOT_FMT_FULL_FIELD_SET2(1, ENTRY_5f, base_entry[5]);
        ALPM_ASSERT(entry_count >= 5);
    } break;
    case 2: {
        L3_DEFIP_ALPM_PIVOT_FMT2_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_SET2(2, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_SET2(2, ENTRY_1f, base_entry[1]);
        PIVOT_FMT_FULL_FIELD_SET2(2, ENTRY_2f, base_entry[2]);
        PIVOT_FMT_FULL_FIELD_SET2(2, ENTRY_3f, base_entry[3]);
        PIVOT_FMT_FULL_FIELD_SET2(2, ENTRY_4f, base_entry[4]);
        ALPM_ASSERT(entry_count >= 4);
    } break;
    case 3: {
        L3_DEFIP_ALPM_PIVOT_FMT3_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_SET2(3, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_SET2(3, ENTRY_1f, base_entry[1]);
        PIVOT_FMT_FULL_FIELD_SET2(3, ENTRY_2f, base_entry[2]);
        PIVOT_FMT_FULL_FIELD_SET2(3, ENTRY_3f, base_entry[3]);
        ALPM_ASSERT(entry_count >= 3);
    } break;
    case 4: {
        L3_DEFIP_ALPM_PIVOT_FMT4_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_SET2(4, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_SET2(4, ENTRY_1f, base_entry[1]);
        PIVOT_FMT_FULL_FIELD_SET2(4, ENTRY_2f, base_entry[2]);
        ALPM_ASSERT(entry_count >= 2);
    } break;
    case 5: {
        L3_DEFIP_ALPM_PIVOT_FMT5_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_SET2(5, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_SET2(5, ENTRY_1f, base_entry[1]);
        ALPM_ASSERT(entry_count >= 1);
    } break;
    case 6: {
        L3_DEFIP_ALPM_PIVOT_FMT6_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_SET2(6, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_SET2(6, ENTRY_1f, base_entry[1]);
        PIVOT_FMT_FULL_FIELD_SET2(6, ENTRY_2f, base_entry[2]);
        PIVOT_FMT_FULL_FIELD_SET2(6, ENTRY_3f, base_entry[3]);
        ALPM_ASSERT(entry_count >= 3);
    } break;
    case 7: {
        L3_DEFIP_ALPM_PIVOT_FMT7_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_SET2(7, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_SET2(7, ENTRY_1f, base_entry[1]);
        PIVOT_FMT_FULL_FIELD_SET2(7, ENTRY_2f, base_entry[2]);
        ALPM_ASSERT(entry_count >= 2);
    } break;
    case 8: {
        L3_DEFIP_ALPM_PIVOT_FMT8_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_SET2(8, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_SET2(8, ENTRY_1f, base_entry[1]);
        ALPM_ASSERT(entry_count >= 1);
    } break;
    case 9: {
        L3_DEFIP_ALPM_PIVOT_FMT9_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_SET2(9, ENTRY_0f, base_entry[0]);
        ALPM_ASSERT(entry_count >= 0);
    } break;
    case 10: {
        L3_DEFIP_ALPM_PIVOT_FMT10_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_SET2(10, ENTRY_0f, base_entry[0]);
        ALPM_ASSERT(entry_count >= 0);
    } break;
    case 16: {
        L3_DEFIP_ALPM_PIVOT_FMT16_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_SET2(16, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_SET2(16, ENTRY_1f, base_entry[1]);
        PIVOT_FMT_FULL_FIELD_SET2(16, ENTRY_2f, base_entry[2]);
        ALPM_ASSERT(entry_count >= 2);
    } break;
    case 17: {
        L3_DEFIP_ALPM_PIVOT_FMT17_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_SET2(17, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_SET2(17, ENTRY_1f, base_entry[1]);
        ALPM_ASSERT(entry_count >= 1);
    } break;
    case 18: {
        L3_DEFIP_ALPM_PIVOT_FMT18_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_SET2(18, ENTRY_0f, base_entry[0]);
        ALPM_ASSERT(entry_count >= 0);
    } break;
    case 19: {
        L3_DEFIP_ALPM_PIVOT_FMT19_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_SET2(19, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_SET2(19, ENTRY_1f, base_entry[1]);
        ALPM_ASSERT(entry_count >= 1);
    } break;
    case 20: {
        L3_DEFIP_ALPM_PIVOT_FMT20_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_SET2(20, ENTRY_0f, base_entry[0]);
        ALPM_ASSERT(entry_count >= 0);
    } break;

    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56780_ln_pivot_format_full_decode2(int u, int m, uint8_t format_value,
                                      uint32_t *base_entry,
                                      int ent_ofs,
                                      void *buf)
{
    format_value = bcm56780_l2p_pivot_fmts[format_value];
    switch (format_value) {
    case 1: {
        L3_DEFIP_ALPM_PIVOT_FMT1_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_GET2(1, ENTRY_0f, base_entry); break;
        case 1: PIVOT_FMT_FULL_FIELD_GET2(1, ENTRY_1f, base_entry); break;
        case 2: PIVOT_FMT_FULL_FIELD_GET2(1, ENTRY_2f, base_entry); break;
        case 3: PIVOT_FMT_FULL_FIELD_GET2(1, ENTRY_3f, base_entry); break;
        case 4: PIVOT_FMT_FULL_FIELD_GET2(1, ENTRY_4f, base_entry); break;
        case 5: PIVOT_FMT_FULL_FIELD_GET2(1, ENTRY_5f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 2: {
        L3_DEFIP_ALPM_PIVOT_FMT2_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_GET2(2, ENTRY_0f, base_entry); break;
        case 1: PIVOT_FMT_FULL_FIELD_GET2(2, ENTRY_1f, base_entry); break;
        case 2: PIVOT_FMT_FULL_FIELD_GET2(2, ENTRY_2f, base_entry); break;
        case 3: PIVOT_FMT_FULL_FIELD_GET2(2, ENTRY_3f, base_entry); break;
        case 4: PIVOT_FMT_FULL_FIELD_GET2(2, ENTRY_4f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 3: {
        L3_DEFIP_ALPM_PIVOT_FMT3_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_GET2(3, ENTRY_0f, base_entry); break;
        case 1: PIVOT_FMT_FULL_FIELD_GET2(3, ENTRY_1f, base_entry); break;
        case 2: PIVOT_FMT_FULL_FIELD_GET2(3, ENTRY_2f, base_entry); break;
        case 3: PIVOT_FMT_FULL_FIELD_GET2(3, ENTRY_3f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 4: {
        L3_DEFIP_ALPM_PIVOT_FMT4_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_GET2(4, ENTRY_0f, base_entry); break;
        case 1: PIVOT_FMT_FULL_FIELD_GET2(4, ENTRY_1f, base_entry); break;
        case 2: PIVOT_FMT_FULL_FIELD_GET2(4, ENTRY_2f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 5: {
        L3_DEFIP_ALPM_PIVOT_FMT5_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_GET2(5, ENTRY_0f, base_entry); break;
        case 1: PIVOT_FMT_FULL_FIELD_GET2(5, ENTRY_1f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 6: {
        L3_DEFIP_ALPM_PIVOT_FMT6_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_GET2(6, ENTRY_0f, base_entry); break;
        case 1: PIVOT_FMT_FULL_FIELD_GET2(6, ENTRY_1f, base_entry); break;
        case 2: PIVOT_FMT_FULL_FIELD_GET2(6, ENTRY_2f, base_entry); break;
        case 3: PIVOT_FMT_FULL_FIELD_GET2(6, ENTRY_3f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 7: {
        L3_DEFIP_ALPM_PIVOT_FMT7_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_GET2(7, ENTRY_0f, base_entry); break;
        case 1: PIVOT_FMT_FULL_FIELD_GET2(7, ENTRY_1f, base_entry); break;
        case 2: PIVOT_FMT_FULL_FIELD_GET2(7, ENTRY_2f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 8: {
        L3_DEFIP_ALPM_PIVOT_FMT8_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_GET2(8, ENTRY_0f, base_entry); break;
        case 1: PIVOT_FMT_FULL_FIELD_GET2(8, ENTRY_1f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 9: {
        L3_DEFIP_ALPM_PIVOT_FMT9_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_GET2(9, ENTRY_0f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 10: {
        L3_DEFIP_ALPM_PIVOT_FMT10_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_GET2(10, ENTRY_0f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 16: {
        L3_DEFIP_ALPM_PIVOT_FMT16_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_GET2(16, ENTRY_0f, base_entry); break;
        case 1: PIVOT_FMT_FULL_FIELD_GET2(16, ENTRY_1f, base_entry); break;
        case 2: PIVOT_FMT_FULL_FIELD_GET2(16, ENTRY_2f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 17: {
        L3_DEFIP_ALPM_PIVOT_FMT17_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_GET2(17, ENTRY_0f, base_entry); break;
        case 1: PIVOT_FMT_FULL_FIELD_GET2(17, ENTRY_1f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 18: {
        L3_DEFIP_ALPM_PIVOT_FMT18_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_GET2(18, ENTRY_0f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 19: {
        L3_DEFIP_ALPM_PIVOT_FMT19_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_GET2(19, ENTRY_0f, base_entry); break;
        case 1: PIVOT_FMT_FULL_FIELD_GET2(19, ENTRY_1f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 20: {
        L3_DEFIP_ALPM_PIVOT_FMT20_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: PIVOT_FMT_FULL_FIELD_GET2(20, ENTRY_0f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56780_ln_pivot_format_full_decode(int u, int m, uint8_t format_value,
                                     ln_base_entry_t *base_entry,
                                     int entry_count,
                                     void *buf)
{
    format_value = bcm56780_l2p_pivot_fmts[format_value];
    switch (format_value) {
    case 1: {
        L3_DEFIP_ALPM_PIVOT_FMT1_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_GET2(1, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_GET2(1, ENTRY_1f, base_entry[1]);
        PIVOT_FMT_FULL_FIELD_GET2(1, ENTRY_2f, base_entry[2]);
        PIVOT_FMT_FULL_FIELD_GET2(1, ENTRY_3f, base_entry[3]);
        PIVOT_FMT_FULL_FIELD_GET2(1, ENTRY_4f, base_entry[4]);
        PIVOT_FMT_FULL_FIELD_GET2(1, ENTRY_5f, base_entry[5]);
        ALPM_ASSERT(entry_count >= 5);
    } break;
    case 2: {
        L3_DEFIP_ALPM_PIVOT_FMT2_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_GET2(2, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_GET2(2, ENTRY_1f, base_entry[1]);
        PIVOT_FMT_FULL_FIELD_GET2(2, ENTRY_2f, base_entry[2]);
        PIVOT_FMT_FULL_FIELD_GET2(2, ENTRY_3f, base_entry[3]);
        PIVOT_FMT_FULL_FIELD_GET2(2, ENTRY_4f, base_entry[4]);
        ALPM_ASSERT(entry_count >= 4);
    } break;
    case 3: {
        L3_DEFIP_ALPM_PIVOT_FMT3_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_GET2(3, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_GET2(3, ENTRY_1f, base_entry[1]);
        PIVOT_FMT_FULL_FIELD_GET2(3, ENTRY_2f, base_entry[2]);
        PIVOT_FMT_FULL_FIELD_GET2(3, ENTRY_3f, base_entry[3]);
        ALPM_ASSERT(entry_count >= 3);
    } break;
    case 4: {
        L3_DEFIP_ALPM_PIVOT_FMT4_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_GET2(4, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_GET2(4, ENTRY_1f, base_entry[1]);
        PIVOT_FMT_FULL_FIELD_GET2(4, ENTRY_2f, base_entry[2]);
        ALPM_ASSERT(entry_count >= 2);
    } break;
    case 5: {
        L3_DEFIP_ALPM_PIVOT_FMT5_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_GET2(5, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_GET2(5, ENTRY_1f, base_entry[1]);
        ALPM_ASSERT(entry_count >= 1);
    } break;
    case 6: {
        L3_DEFIP_ALPM_PIVOT_FMT6_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_GET2(6, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_GET2(6, ENTRY_1f, base_entry[1]);
        PIVOT_FMT_FULL_FIELD_GET2(6, ENTRY_2f, base_entry[2]);
        PIVOT_FMT_FULL_FIELD_GET2(6, ENTRY_3f, base_entry[3]);
        ALPM_ASSERT(entry_count >= 3);
    } break;
    case 7: {
        L3_DEFIP_ALPM_PIVOT_FMT7_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_GET2(7, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_GET2(7, ENTRY_1f, base_entry[1]);
        PIVOT_FMT_FULL_FIELD_GET2(7, ENTRY_2f, base_entry[2]);
        ALPM_ASSERT(entry_count >= 2);
    } break;
    case 8: {
        L3_DEFIP_ALPM_PIVOT_FMT8_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_GET2(8, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_GET2(8, ENTRY_1f, base_entry[1]);
        ALPM_ASSERT(entry_count >= 1);
    } break;
    case 9: {
        L3_DEFIP_ALPM_PIVOT_FMT9_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_GET2(9, ENTRY_0f, base_entry[0]);
        ALPM_ASSERT(entry_count >= 0);
    } break;
    case 10: {
        L3_DEFIP_ALPM_PIVOT_FMT10_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_GET2(10, ENTRY_0f, base_entry[0]);
        ALPM_ASSERT(entry_count >= 0);
    } break;
    case 16: {
        L3_DEFIP_ALPM_PIVOT_FMT16_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_GET2(16, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_GET2(16, ENTRY_1f, base_entry[1]);
        PIVOT_FMT_FULL_FIELD_GET2(16, ENTRY_2f, base_entry[2]);
        ALPM_ASSERT(entry_count >= 2);
    } break;
    case 17: {
        L3_DEFIP_ALPM_PIVOT_FMT17_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_GET2(17, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_GET2(17, ENTRY_1f, base_entry[1]);
        ALPM_ASSERT(entry_count >= 1);
    } break;
    case 18: {
        L3_DEFIP_ALPM_PIVOT_FMT18_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_GET2(18, ENTRY_0f, base_entry[0]);
        ALPM_ASSERT(entry_count >= 0);
    } break;
    case 19: {
        L3_DEFIP_ALPM_PIVOT_FMT19_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_GET2(19, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_GET2(19, ENTRY_1f, base_entry[1]);
        ALPM_ASSERT(entry_count >= 1);
    } break;
    case 20: {
        L3_DEFIP_ALPM_PIVOT_FMT20_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_GET2(20, ENTRY_0f, base_entry[0]);
        ALPM_ASSERT(entry_count >= 0);
    } break;

    default:
        ALPM_ASSERT(0);
        break;
    }
}


static void
bcm56780_ln_pivot_format_set_data(uint8_t format_value,
                                  uint32_t *ad_format,
                                  void *buf)
{
    format_value = bcm56780_l2p_pivot_fmts[format_value];
    switch (format_value) {
    case 1: {
        L3_DEFIP_ALPM_PIVOT_FMT1_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET(1, ASSOC_DATAf, ad_format);
    } break;
    case 2: {
        L3_DEFIP_ALPM_PIVOT_FMT2_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET(2, ASSOC_DATAf, ad_format);
    } break;
    case 3: {
        L3_DEFIP_ALPM_PIVOT_FMT3_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET(3, ASSOC_DATAf, ad_format);
    } break;
    case 4: {
        L3_DEFIP_ALPM_PIVOT_FMT4_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET(4, ASSOC_DATAf, ad_format);
    } break;
    case 5: {
        L3_DEFIP_ALPM_PIVOT_FMT5_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET(5, ASSOC_DATAf, ad_format);
    } break;
    case 6: {
        L3_DEFIP_ALPM_PIVOT_FMT6_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET2(6, ASSOC_DATAf, ad_format);
    } break;
    case 7: {
        L3_DEFIP_ALPM_PIVOT_FMT7_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET2(7, ASSOC_DATAf, ad_format);
    } break;
    case 8: {
        L3_DEFIP_ALPM_PIVOT_FMT8_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET2(8, ASSOC_DATAf, ad_format);
    } break;
    case 9: {
        L3_DEFIP_ALPM_PIVOT_FMT9_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET(9, ASSOC_DATAf, ad_format);
    } break;
    case 10: {
        L3_DEFIP_ALPM_PIVOT_FMT10_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET2(10, ASSOC_DATAf, ad_format);
    } break;
    case 16: {
        L3_DEFIP_ALPM_PIVOT_FMT16_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET2(16, ASSOC_DATAf, ad_format);
    } break;
    case 17: {
        L3_DEFIP_ALPM_PIVOT_FMT17_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET2(17, ASSOC_DATAf, ad_format);
    } break;
    case 18: {
        L3_DEFIP_ALPM_PIVOT_FMT18_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET2(18, ASSOC_DATAf, ad_format);
    } break;
    case 19: {
        L3_DEFIP_ALPM_PIVOT_FMT19_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET2(19, ASSOC_DATAf, ad_format);
    } break;
    case 20: {
        L3_DEFIP_ALPM_PIVOT_FMT20_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET2(20, ASSOC_DATAf, ad_format);
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56780_ln_pivot_format_set(uint8_t format_value,
                             uint32_t *ad_format,
                             uint32_t *a2_format,
                             alpm_format_ip_t prefix,
                             void *buf)
{
    format_value = bcm56780_l2p_pivot_fmts[format_value];
    switch (format_value) {
    case 1: {
        L3_DEFIP_ALPM_PIVOT_FMT1_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET(1, PREFIXf, prefix);
        PIVOT_FMT_FIELD_SET(1, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_SET2(1, ALPM2_DATAf, a2_format);
    } break;
    case 2: {
        L3_DEFIP_ALPM_PIVOT_FMT2_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET(2, PREFIXf, prefix);
        PIVOT_FMT_FIELD_SET(2, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_SET2(2, ALPM2_DATAf, a2_format);
    } break;
    case 3: {
        L3_DEFIP_ALPM_PIVOT_FMT3_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET2(3, PREFIXf, prefix);
        PIVOT_FMT_FIELD_SET(3, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_SET2(3, ALPM2_DATAf, a2_format);
    } break;
    case 4: {
        L3_DEFIP_ALPM_PIVOT_FMT4_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET2(4, PREFIXf, prefix);
        PIVOT_FMT_FIELD_SET(4, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_SET2(4, ALPM2_DATAf, a2_format);
    } break;
    case 5: {
        L3_DEFIP_ALPM_PIVOT_FMT5_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET2(5, PREFIXf, prefix);
        PIVOT_FMT_FIELD_SET(5, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_SET2(5, ALPM2_DATAf, a2_format);
    } break;
    case 6: {
        L3_DEFIP_ALPM_PIVOT_FMT6_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET(6, PREFIXf, prefix);
        PIVOT_FMT_FIELD_SET2(6, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_SET2(6, ALPM2_DATAf, a2_format);
    } break;
    case 7: {
        L3_DEFIP_ALPM_PIVOT_FMT7_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET2(7, PREFIXf, prefix);
        PIVOT_FMT_FIELD_SET2(7, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_SET2(7, ALPM2_DATAf, a2_format);
    } break;
    case 8: {
        L3_DEFIP_ALPM_PIVOT_FMT8_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET2(8, PREFIXf, prefix);
        PIVOT_FMT_FIELD_SET2(8, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_SET2(8, ALPM2_DATAf, a2_format);
    } break;
    case 9: {
        L3_DEFIP_ALPM_PIVOT_FMT9_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET2(9, PREFIXf, prefix);
        PIVOT_FMT_FIELD_SET(9, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_SET2(9, ALPM2_DATAf, a2_format);
    } break;
    case 10: {
        L3_DEFIP_ALPM_PIVOT_FMT10_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET2(10, PREFIXf, prefix);
        PIVOT_FMT_FIELD_SET2(10, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_SET2(10, ALPM2_DATAf, a2_format);
    } break;
    case 16: {
        L3_DEFIP_ALPM_PIVOT_FMT16_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET(16, PREFIXf, prefix);
        PIVOT_FMT_FIELD_SET2(16, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_SET2(16, ALPM2_DATAf, a2_format);
    } break;
    case 17: {
        L3_DEFIP_ALPM_PIVOT_FMT17_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET2(17, PREFIXf, prefix);
        PIVOT_FMT_FIELD_SET2(17, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_SET2(17, ALPM2_DATAf, a2_format);
    } break;
    case 18: {
        L3_DEFIP_ALPM_PIVOT_FMT18_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET2(18, PREFIXf, prefix);
        PIVOT_FMT_FIELD_SET2(18, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_SET2(18, ALPM2_DATAf, a2_format);
    } break;
    case 19: {
        L3_DEFIP_ALPM_PIVOT_FMT19_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET2(19, PREFIXf, prefix);
        PIVOT_FMT_FIELD_SET2(19, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_SET2(19, ALPM2_DATAf, a2_format);
    } break;
    case 20: {
        L3_DEFIP_ALPM_PIVOT_FMT20_t *format_buf = buf;
        PIVOT_FMT_FIELD_SET2(20, PREFIXf, prefix);
        PIVOT_FMT_FIELD_SET2(20, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_SET2(20, ALPM2_DATAf, a2_format);
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
}


static void
bcm56780_ln_pivot_format_encode(int u, int m, uint8_t format_value,
                                uint8_t kshift,
                                alpm_arg_t *arg,
                                void *buf)
{
    int full = 0, number;
    int max_len = 0;
    bcm56780_assoc_data_t ad_format;
    bcm56780_ln_pivot_base_entry_info(u, m, format_value, &max_len, &full,
                                      &number);
    bcm56780_assoc_data_format_encode(u, m, &ad_format, arg->key.t.app, full,
                                      &arg->ad);

    if (arg->encode_data_only) {
        bcm56780_ln_pivot_format_set_data(format_value,
                                          &ad_format.v[0],
                                          buf);
    } else {
        alpm_format_ip_t prefix;
        ALPM2_DATA_t a2_format;
        bcm56780_ln_alpm2_data_format_encode(u, m, arg, &a2_format, &arg->a2);
        bcmptm_rm_alpm_one_bit_encode(arg->key.t.max_bits,
                                      &arg->key.t.trie_ip,
                                      arg->key.t.len,
                                      max_len,
                                      kshift,
                                      &prefix[0]);
        bcm56780_ln_pivot_format_set(format_value,
                                     &ad_format.v[0],
                                     &a2_format.v[0],
                                     &prefix[0],
                                     buf);
    }
}

static void
bcm56780_ln_pivot_format_get_data(uint8_t format_value,
                                  uint32_t *ad_format,
                                  void *buf)
{
    format_value = bcm56780_l2p_pivot_fmts[format_value];
    switch (format_value) {
    case 1: {
        L3_DEFIP_ALPM_PIVOT_FMT1_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET(1, ASSOC_DATAf, ad_format);
    } break;
    case 2: {
        L3_DEFIP_ALPM_PIVOT_FMT2_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET(2, ASSOC_DATAf, ad_format);
    } break;
    case 3: {
        L3_DEFIP_ALPM_PIVOT_FMT3_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET(3, ASSOC_DATAf, ad_format);
    } break;
    case 4: {
        L3_DEFIP_ALPM_PIVOT_FMT4_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET(4, ASSOC_DATAf, ad_format);
    } break;
    case 5: {
        L3_DEFIP_ALPM_PIVOT_FMT5_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET(5, ASSOC_DATAf, ad_format);
    } break;
    case 6: {
        L3_DEFIP_ALPM_PIVOT_FMT6_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET2(6, ASSOC_DATAf, ad_format);
    } break;
    case 7: {
        L3_DEFIP_ALPM_PIVOT_FMT7_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET2(7, ASSOC_DATAf, ad_format);
    } break;
    case 8: {
        L3_DEFIP_ALPM_PIVOT_FMT8_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET2(8, ASSOC_DATAf, ad_format);
    } break;
    case 9: {
        L3_DEFIP_ALPM_PIVOT_FMT9_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET(9, ASSOC_DATAf, ad_format);
    } break;
    case 10: {
        L3_DEFIP_ALPM_PIVOT_FMT10_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET2(10, ASSOC_DATAf, ad_format);
    } break;
    case 16: {
        L3_DEFIP_ALPM_PIVOT_FMT16_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET2(16, ASSOC_DATAf, ad_format);
    } break;
    case 17: {
        L3_DEFIP_ALPM_PIVOT_FMT17_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET2(17, ASSOC_DATAf, ad_format);
    } break;
    case 18: {
        L3_DEFIP_ALPM_PIVOT_FMT18_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET2(18, ASSOC_DATAf, ad_format);
    } break;
    case 19: {
        L3_DEFIP_ALPM_PIVOT_FMT19_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET2(19, ASSOC_DATAf, ad_format);
    } break;
    case 20: {
        L3_DEFIP_ALPM_PIVOT_FMT20_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET2(20, ASSOC_DATAf, ad_format);
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56780_ln_pivot_format_get(uint8_t format_value,
                             uint32_t *ad_format,
                             uint32_t *a2_format,
                             alpm_format_ip_t prefix,
                             void *buf)
{
    format_value = bcm56780_l2p_pivot_fmts[format_value];
    switch (format_value) {
    case 1: {
        L3_DEFIP_ALPM_PIVOT_FMT1_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET(1, PREFIXf, prefix);
        PIVOT_FMT_FIELD_GET(1, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_GET2(1, ALPM2_DATAf, a2_format);
    } break;
    case 2: {
        L3_DEFIP_ALPM_PIVOT_FMT2_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET(2, PREFIXf, prefix);
        PIVOT_FMT_FIELD_GET(2, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_GET2(2, ALPM2_DATAf, a2_format);
    } break;
    case 3: {
        L3_DEFIP_ALPM_PIVOT_FMT3_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET2(3, PREFIXf, prefix);
        PIVOT_FMT_FIELD_GET(3, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_GET2(3, ALPM2_DATAf, a2_format);
    } break;
    case 4: {
        L3_DEFIP_ALPM_PIVOT_FMT4_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET2(4, PREFIXf, prefix);
        PIVOT_FMT_FIELD_GET(4, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_GET2(4, ALPM2_DATAf, a2_format);
    } break;
    case 5: {
        L3_DEFIP_ALPM_PIVOT_FMT5_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET2(5, PREFIXf, prefix);
        PIVOT_FMT_FIELD_GET(5, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_GET2(5, ALPM2_DATAf, a2_format);
    } break;
    case 6: {
        L3_DEFIP_ALPM_PIVOT_FMT6_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET(6, PREFIXf, prefix);
        PIVOT_FMT_FIELD_GET2(6, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_GET2(6, ALPM2_DATAf, a2_format);
    } break;
    case 7: {
        L3_DEFIP_ALPM_PIVOT_FMT7_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET2(7, PREFIXf, prefix);
        PIVOT_FMT_FIELD_GET2(7, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_GET2(7, ALPM2_DATAf, a2_format);
    } break;
    case 8: {
        L3_DEFIP_ALPM_PIVOT_FMT8_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET2(8, PREFIXf, prefix);
        PIVOT_FMT_FIELD_GET2(8, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_GET2(8, ALPM2_DATAf, a2_format);
    } break;
    case 9: {
        L3_DEFIP_ALPM_PIVOT_FMT9_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET2(9, PREFIXf, prefix);
        PIVOT_FMT_FIELD_GET(9, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_GET2(9, ALPM2_DATAf, a2_format);
    } break;
    case 10: {
        L3_DEFIP_ALPM_PIVOT_FMT10_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET2(10, PREFIXf, prefix);
        PIVOT_FMT_FIELD_GET2(10, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_GET2(10, ALPM2_DATAf, a2_format);
    } break;
    case 16: {
        L3_DEFIP_ALPM_PIVOT_FMT16_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET(16, PREFIXf, prefix);
        PIVOT_FMT_FIELD_GET2(16, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_GET2(16, ALPM2_DATAf, a2_format);
    } break;
    case 17: {
        L3_DEFIP_ALPM_PIVOT_FMT17_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET2(17, PREFIXf, prefix);
        PIVOT_FMT_FIELD_GET2(17, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_GET2(17, ALPM2_DATAf, a2_format);
    } break;
    case 18: {
        L3_DEFIP_ALPM_PIVOT_FMT18_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET2(18, PREFIXf, prefix);
        PIVOT_FMT_FIELD_GET2(18, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_GET2(18, ALPM2_DATAf, a2_format);
    } break;
    case 19: {
        L3_DEFIP_ALPM_PIVOT_FMT19_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET2(19, PREFIXf, prefix);
        PIVOT_FMT_FIELD_GET2(19, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_GET2(19, ALPM2_DATAf, a2_format);
    } break;
    case 20: {
        L3_DEFIP_ALPM_PIVOT_FMT20_t *format_buf = buf;
        PIVOT_FMT_FIELD_GET2(20, PREFIXf, prefix);
        PIVOT_FMT_FIELD_GET2(20, ASSOC_DATAf, ad_format);
        PIVOT_FMT_FIELD_GET2(20, ALPM2_DATAf, a2_format);
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
}


static void
bcm56780_ln_pivot_format_decode(int u, int m, uint8_t format_value,
                                uint8_t kshift,
                                alpm_arg_t *arg,
                                void *buf)
{
    int full, number;
    int max_len;
    ALPM2_DATA_t a2_format;
    bcm56780_assoc_data_t ad_format = {{{0}}};
    alpm_format_ip_t prefix = {0};
    bcm56780_ln_pivot_base_entry_info(u, m, format_value, &max_len, &full,
                                      &number);
    if (arg->decode_data_only) {
        bcm56780_ln_pivot_format_get_data(format_value,
                                          &ad_format.v[0],
                                          buf);
    } else {
        bcm56780_ln_pivot_format_get(format_value,
                                     &ad_format.v[0],
                                     &a2_format.v[0],
                                     &prefix[0],
                                     buf);
        bcm56780_ln_alpm2_data_format_decode(u, m, arg, &a2_format, &arg->a2);
        arg->valid = bcmptm_rm_alpm_one_bit_decode(arg->key.t.max_bits,
                                                   &arg->key.t.trie_ip,
                                                   &arg->key.t.len,
                                                   max_len,
                                                   kshift,
                                                   &prefix[0]);
    }
    bcm56780_assoc_data_format_decode(u, m, &ad_format, arg->key.t.app, full,
                                      &arg->ad);
}

static void
bcm56780_ln_route_base_entry_info(int u, int m, uint8_t format_value,
                                  int *prefix_len,
                                  int *full,
                                  int *number)
{
    ALPM_ASSERT(format_value < COUNTOF(bcm56780_route_base_entry_info));
    *prefix_len = bcm56780_route_base_entry_info[format_value].prefix_len;
    *full = bcm56780_route_base_entry_info[format_value].full;
    *number = bcm56780_route_base_entry_info[format_value].num_entries;

    (*prefix_len)--; /* Exclude the extra bit */
}


static void
bcm56780_ln_route_format_full_encode2(int u, int m, uint8_t format_value,
                                      uint32_t *base_entry,
                                      int ent_ofs,
                                      void *buf)
{
    format_value = bcm56780_l2p_route_fmts[format_value];
    switch (format_value) {
    case 1: {
        L3_DEFIP_ALPM_ROUTE_FMT1_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_3f, base_entry); break;
        case 4: ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_4f, base_entry); break;
        case 5: ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_5f, base_entry); break;
        case 6: ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_6f, base_entry); break;
        case 7: ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_7f, base_entry); break;
        case 8: ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_8f, base_entry); break;
        case 9: ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_9f, base_entry); break;
        case 10: ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_10f, base_entry); break;
        case 11: ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_11f, base_entry); break;
        case 12: ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_12f, base_entry); break;
        case 13: ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_13f, base_entry); break;
        case 14: ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_14f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 2: {
        L3_DEFIP_ALPM_ROUTE_FMT2_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_3f, base_entry); break;
        case 4: ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_4f, base_entry); break;
        case 5: ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_5f, base_entry); break;
        case 6: ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_6f, base_entry); break;
        case 7: ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_7f, base_entry); break;
        case 8: ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_8f, base_entry); break;
        case 9: ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_9f, base_entry); break;
        case 10: ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_10f, base_entry); break;
        case 11: ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_11f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 3: {
        L3_DEFIP_ALPM_ROUTE_FMT3_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(3, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET2(3, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_SET2(3, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_SET2(3, ENTRY_3f, base_entry); break;
        case 4: ROUTE_FMT_FULL_FIELD_SET2(3, ENTRY_4f, base_entry); break;
        case 5: ROUTE_FMT_FULL_FIELD_SET2(3, ENTRY_5f, base_entry); break;
        case 6: ROUTE_FMT_FULL_FIELD_SET2(3, ENTRY_6f, base_entry); break;
        case 7: ROUTE_FMT_FULL_FIELD_SET2(3, ENTRY_7f, base_entry); break;
        case 8: ROUTE_FMT_FULL_FIELD_SET2(3, ENTRY_8f, base_entry); break;
        case 9: ROUTE_FMT_FULL_FIELD_SET2(3, ENTRY_9f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 4: {
        L3_DEFIP_ALPM_ROUTE_FMT4_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(4, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET2(4, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_SET2(4, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_SET2(4, ENTRY_3f, base_entry); break;
        case 4: ROUTE_FMT_FULL_FIELD_SET2(4, ENTRY_4f, base_entry); break;
        case 5: ROUTE_FMT_FULL_FIELD_SET2(4, ENTRY_5f, base_entry); break;
        case 6: ROUTE_FMT_FULL_FIELD_SET2(4, ENTRY_6f, base_entry); break;
        case 7: ROUTE_FMT_FULL_FIELD_SET2(4, ENTRY_7f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 5: {
        L3_DEFIP_ALPM_ROUTE_FMT5_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(5, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET2(5, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_SET2(5, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_SET2(5, ENTRY_3f, base_entry); break;
        case 4: ROUTE_FMT_FULL_FIELD_SET2(5, ENTRY_4f, base_entry); break;
        case 5: ROUTE_FMT_FULL_FIELD_SET2(5, ENTRY_5f, base_entry); break;
        case 6: ROUTE_FMT_FULL_FIELD_SET2(5, ENTRY_6f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 6: {
        L3_DEFIP_ALPM_ROUTE_FMT6_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(6, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET2(6, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_SET2(6, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_SET2(6, ENTRY_3f, base_entry); break;
        case 4: ROUTE_FMT_FULL_FIELD_SET2(6, ENTRY_4f, base_entry); break;
        case 5: ROUTE_FMT_FULL_FIELD_SET2(6, ENTRY_5f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 7: {
        L3_DEFIP_ALPM_ROUTE_FMT7_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(7, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET2(7, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_SET2(7, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_SET2(7, ENTRY_3f, base_entry); break;
        case 4: ROUTE_FMT_FULL_FIELD_SET2(7, ENTRY_4f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 8: {
        L3_DEFIP_ALPM_ROUTE_FMT8_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(8, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET2(8, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_SET2(8, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_SET2(8, ENTRY_3f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 9: {
        L3_DEFIP_ALPM_ROUTE_FMT9_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(9, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET2(9, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_SET2(9, ENTRY_2f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 10: {
        L3_DEFIP_ALPM_ROUTE_FMT10_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(10, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET2(10, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_SET2(10, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_SET2(10, ENTRY_3f, base_entry); break;
        case 4: ROUTE_FMT_FULL_FIELD_SET2(10, ENTRY_4f, base_entry); break;
        case 5: ROUTE_FMT_FULL_FIELD_SET2(10, ENTRY_5f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 11: {
        L3_DEFIP_ALPM_ROUTE_FMT11_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(11, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET2(11, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_SET2(11, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_SET2(11, ENTRY_3f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 12: {
        L3_DEFIP_ALPM_ROUTE_FMT12_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(12, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET2(12, ENTRY_1f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 13: {
        L3_DEFIP_ALPM_ROUTE_FMT13_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_3f, base_entry); break;
        case 4: ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_4f, base_entry); break;
        case 5: ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_5f, base_entry); break;
        case 6: ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_6f, base_entry); break;
        case 7: ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_7f, base_entry); break;
        case 8: ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_8f, base_entry); break;
        case 9: ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_9f, base_entry); break;
        case 10: ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_10f, base_entry); break;
        case 11: ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_11f, base_entry); break;
        case 12: ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_12f, base_entry); break;
        case 13: ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_13f, base_entry); break;
        case 14: ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_14f, base_entry); break;
        case 15: ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_15f, base_entry); break;
        case 16: ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_16f, base_entry); break;
        case 17: ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_17f, base_entry); break;
        case 18: ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_18f, base_entry); break;
        case 19: ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_19f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 14: {
        L3_DEFIP_ALPM_ROUTE_FMT14_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(14, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET2(14, ENTRY_1f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 15: {
        L3_DEFIP_ALPM_ROUTE_FMT15_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(15, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET2(15, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_SET2(15, ENTRY_2f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 16: {
        L3_DEFIP_ALPM_ROUTE_FMT16_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(16, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET2(16, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_SET2(16, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_SET2(16, ENTRY_3f, base_entry); break;
        case 4: ROUTE_FMT_FULL_FIELD_SET2(16, ENTRY_4f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 17: {
        L3_DEFIP_ALPM_ROUTE_FMT17_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(17, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET2(17, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_SET2(17, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_SET2(17, ENTRY_3f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 18: {
        L3_DEFIP_ALPM_ROUTE_FMT18_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(18, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET2(18, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_SET2(18, ENTRY_2f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 19: {
        L3_DEFIP_ALPM_ROUTE_FMT19_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(19, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET2(19, ENTRY_1f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 20: {
        L3_DEFIP_ALPM_ROUTE_FMT20_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(20, ENTRY_0f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;

    case 21: {
        L3_DEFIP_ALPM_ROUTE_FMT21_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(21, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET2(21, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_SET2(21, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_SET2(21, ENTRY_3f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 22: {
        L3_DEFIP_ALPM_ROUTE_FMT22_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(22, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET2(22, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_SET2(22, ENTRY_2f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 23: {
        L3_DEFIP_ALPM_ROUTE_FMT23_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(23, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET2(23, ENTRY_1f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 24: {
        L3_DEFIP_ALPM_ROUTE_FMT24_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(24, ENTRY_0f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56780_ln_route_format_full_encode(int u, int m, uint8_t format_value,
                                     ln_base_entry_t *base_entry,
                                     int entry_count,
                                     void *buf)
{
    format_value = bcm56780_l2p_route_fmts[format_value];
    switch (format_value) {
    case 1: {
        L3_DEFIP_ALPM_ROUTE_FMT1_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_4f, base_entry[4]);
        ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_5f, base_entry[5]);
        ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_6f, base_entry[6]);
        ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_7f, base_entry[7]);
        ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_8f, base_entry[8]);
        ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_9f, base_entry[9]);
        ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_10f, base_entry[10]);
        ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_11f, base_entry[11]);
        ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_12f, base_entry[12]);
        ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_13f, base_entry[13]);
        ROUTE_FMT_FULL_FIELD_SET(1, ENTRY_14f, base_entry[14]);
        ALPM_ASSERT(entry_count >= 14);
    } break;
    case 2: {
        L3_DEFIP_ALPM_ROUTE_FMT2_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_4f, base_entry[4]);
        ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_5f, base_entry[5]);
        ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_6f, base_entry[6]);
        ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_7f, base_entry[7]);
        ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_8f, base_entry[8]);
        ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_9f, base_entry[9]);
        ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_10f, base_entry[10]);
        ROUTE_FMT_FULL_FIELD_SET2(2, ENTRY_11f, base_entry[11]);
        ALPM_ASSERT(entry_count >= 11);
    } break;
    case 3: {
        L3_DEFIP_ALPM_ROUTE_FMT3_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(3, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(3, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(3, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_SET2(3, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_SET2(3, ENTRY_4f, base_entry[4]);
        ROUTE_FMT_FULL_FIELD_SET2(3, ENTRY_5f, base_entry[5]);
        ROUTE_FMT_FULL_FIELD_SET2(3, ENTRY_6f, base_entry[6]);
        ROUTE_FMT_FULL_FIELD_SET2(3, ENTRY_7f, base_entry[7]);
        ROUTE_FMT_FULL_FIELD_SET2(3, ENTRY_8f, base_entry[8]);
        ROUTE_FMT_FULL_FIELD_SET2(3, ENTRY_9f, base_entry[9]);
        ALPM_ASSERT(entry_count >= 9);
    } break;
    case 4: {
        L3_DEFIP_ALPM_ROUTE_FMT4_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(4, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(4, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(4, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_SET2(4, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_SET2(4, ENTRY_4f, base_entry[4]);
        ROUTE_FMT_FULL_FIELD_SET2(4, ENTRY_5f, base_entry[5]);
        ROUTE_FMT_FULL_FIELD_SET2(4, ENTRY_6f, base_entry[6]);
        ROUTE_FMT_FULL_FIELD_SET2(4, ENTRY_7f, base_entry[7]);
        ALPM_ASSERT(entry_count >= 7);
    } break;
    case 5: {
        L3_DEFIP_ALPM_ROUTE_FMT5_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(5, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(5, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(5, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_SET2(5, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_SET2(5, ENTRY_4f, base_entry[4]);
        ROUTE_FMT_FULL_FIELD_SET2(5, ENTRY_5f, base_entry[5]);
        ROUTE_FMT_FULL_FIELD_SET2(5, ENTRY_6f, base_entry[6]);
        ALPM_ASSERT(entry_count >= 6);
    } break;
    case 6: {
        L3_DEFIP_ALPM_ROUTE_FMT6_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(6, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(6, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(6, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_SET2(6, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_SET2(6, ENTRY_4f, base_entry[4]);
        ROUTE_FMT_FULL_FIELD_SET2(6, ENTRY_5f, base_entry[5]);
        ALPM_ASSERT(entry_count >= 5);
    } break;
    case 7: {
        L3_DEFIP_ALPM_ROUTE_FMT7_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(7, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(7, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(7, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_SET2(7, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_SET2(7, ENTRY_4f, base_entry[4]);
        ALPM_ASSERT(entry_count >= 4);
    } break;
    case 8: {
        L3_DEFIP_ALPM_ROUTE_FMT8_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(8, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(8, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(8, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_SET2(8, ENTRY_3f, base_entry[3]);
        ALPM_ASSERT(entry_count >= 3);
    } break;
    case 9: {
        L3_DEFIP_ALPM_ROUTE_FMT9_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(9, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(9, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(9, ENTRY_2f, base_entry[2]);
        ALPM_ASSERT(entry_count >= 2);
    } break;
    case 10: {
        L3_DEFIP_ALPM_ROUTE_FMT10_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(10, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(10, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(10, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_SET2(10, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_SET2(10, ENTRY_4f, base_entry[4]);
        ROUTE_FMT_FULL_FIELD_SET2(10, ENTRY_5f, base_entry[5]);
        ALPM_ASSERT(entry_count >= 5);
    } break;
    case 11: {
        L3_DEFIP_ALPM_ROUTE_FMT11_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(11, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(11, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(11, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_SET2(11, ENTRY_3f, base_entry[3]);
        ALPM_ASSERT(entry_count >= 3);
    } break;
    case 12: {
        L3_DEFIP_ALPM_ROUTE_FMT12_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(12, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(12, ENTRY_1f, base_entry[1]);
        ALPM_ASSERT(entry_count >= 1);
    } break;
    case 13: {
        L3_DEFIP_ALPM_ROUTE_FMT13_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_4f, base_entry[4]);
        ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_5f, base_entry[5]);
        ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_6f, base_entry[6]);
        ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_7f, base_entry[7]);
        ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_8f, base_entry[8]);
        ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_9f, base_entry[9]);
        ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_10f, base_entry[10]);
        ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_11f, base_entry[11]);
        ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_12f, base_entry[12]);
        ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_13f, base_entry[13]);
        ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_14f, base_entry[14]);
        ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_15f, base_entry[15]);
        ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_16f, base_entry[16]);
        ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_17f, base_entry[17]);
        ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_18f, base_entry[18]);
        ROUTE_FMT_FULL_FIELD_SET(13, ENTRY_19f, base_entry[19]);
        ALPM_ASSERT(entry_count >= 19);
    } break;
    case 14: {
        L3_DEFIP_ALPM_ROUTE_FMT14_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(14, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(14, ENTRY_1f, base_entry[1]);
        ALPM_ASSERT(entry_count >= 1);
    } break;
    case 15: {
        L3_DEFIP_ALPM_ROUTE_FMT15_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(15, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(15, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(15, ENTRY_2f, base_entry[2]);
        ALPM_ASSERT(entry_count >= 2);
    } break;
    case 16: {
        L3_DEFIP_ALPM_ROUTE_FMT16_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(16, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(16, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(16, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_SET2(16, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_SET2(16, ENTRY_4f, base_entry[4]);
        ALPM_ASSERT(entry_count >= 4);
    } break;
    case 17: {
        L3_DEFIP_ALPM_ROUTE_FMT17_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(17, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(17, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(17, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_SET2(17, ENTRY_3f, base_entry[3]);
        ALPM_ASSERT(entry_count >= 3);
    } break;
    case 18: {
        L3_DEFIP_ALPM_ROUTE_FMT18_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(18, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(18, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(18, ENTRY_2f, base_entry[2]);
        ALPM_ASSERT(entry_count >= 2);
    } break;
    case 19: {
        L3_DEFIP_ALPM_ROUTE_FMT19_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(19, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(19, ENTRY_1f, base_entry[1]);
        ALPM_ASSERT(entry_count >= 1);
    } break;
    case 20: {
        L3_DEFIP_ALPM_ROUTE_FMT20_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(20, ENTRY_0f, base_entry[0]);
        ALPM_ASSERT(entry_count >= 0);
    } break;

    case 21: {
        L3_DEFIP_ALPM_ROUTE_FMT21_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(21, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(21, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(21, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_SET2(21, ENTRY_3f, base_entry[3]);
        ALPM_ASSERT(entry_count >= 3);
    } break;
    case 22: {
        L3_DEFIP_ALPM_ROUTE_FMT22_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(22, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(22, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(22, ENTRY_2f, base_entry[2]);
        ALPM_ASSERT(entry_count >= 2);
    } break;
    case 23: {
        L3_DEFIP_ALPM_ROUTE_FMT23_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(23, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(23, ENTRY_1f, base_entry[1]);
        ALPM_ASSERT(entry_count >= 1);
    } break;
    case 24: {
        L3_DEFIP_ALPM_ROUTE_FMT24_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(24, ENTRY_0f, base_entry[0]);
        ALPM_ASSERT(entry_count >= 0);
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
}


static void
bcm56780_ln_route_format_full_decode2(int u, int m, uint8_t format_value,
                                      uint32_t *base_entry,
                                      int ent_ofs,
                                      void *buf)
{
    format_value = bcm56780_l2p_route_fmts[format_value];
    switch (format_value) {
    case 1: {
        L3_DEFIP_ALPM_ROUTE_FMT1_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_3f, base_entry); break;
        case 4: ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_4f, base_entry); break;
        case 5: ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_5f, base_entry); break;
        case 6: ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_6f, base_entry); break;
        case 7: ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_7f, base_entry); break;
        case 8: ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_8f, base_entry); break;
        case 9: ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_9f, base_entry); break;
        case 10: ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_10f, base_entry); break;
        case 11: ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_11f, base_entry); break;
        case 12: ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_12f, base_entry); break;
        case 13: ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_13f, base_entry); break;
        case 14: ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_14f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 2: {
        L3_DEFIP_ALPM_ROUTE_FMT2_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_3f, base_entry); break;
        case 4: ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_4f, base_entry); break;
        case 5: ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_5f, base_entry); break;
        case 6: ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_6f, base_entry); break;
        case 7: ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_7f, base_entry); break;
        case 8: ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_8f, base_entry); break;
        case 9: ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_9f, base_entry); break;
        case 10: ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_10f, base_entry); break;
        case 11: ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_11f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 3: {
        L3_DEFIP_ALPM_ROUTE_FMT3_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(3, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET2(3, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_GET2(3, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_GET2(3, ENTRY_3f, base_entry); break;
        case 4: ROUTE_FMT_FULL_FIELD_GET2(3, ENTRY_4f, base_entry); break;
        case 5: ROUTE_FMT_FULL_FIELD_GET2(3, ENTRY_5f, base_entry); break;
        case 6: ROUTE_FMT_FULL_FIELD_GET2(3, ENTRY_6f, base_entry); break;
        case 7: ROUTE_FMT_FULL_FIELD_GET2(3, ENTRY_7f, base_entry); break;
        case 8: ROUTE_FMT_FULL_FIELD_GET2(3, ENTRY_8f, base_entry); break;
        case 9: ROUTE_FMT_FULL_FIELD_GET2(3, ENTRY_9f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 4: {
        L3_DEFIP_ALPM_ROUTE_FMT4_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(4, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET2(4, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_GET2(4, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_GET2(4, ENTRY_3f, base_entry); break;
        case 4: ROUTE_FMT_FULL_FIELD_GET2(4, ENTRY_4f, base_entry); break;
        case 5: ROUTE_FMT_FULL_FIELD_GET2(4, ENTRY_5f, base_entry); break;
        case 6: ROUTE_FMT_FULL_FIELD_GET2(4, ENTRY_6f, base_entry); break;
        case 7: ROUTE_FMT_FULL_FIELD_GET2(4, ENTRY_7f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 5: {
        L3_DEFIP_ALPM_ROUTE_FMT5_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(5, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET2(5, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_GET2(5, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_GET2(5, ENTRY_3f, base_entry); break;
        case 4: ROUTE_FMT_FULL_FIELD_GET2(5, ENTRY_4f, base_entry); break;
        case 5: ROUTE_FMT_FULL_FIELD_GET2(5, ENTRY_5f, base_entry); break;
        case 6: ROUTE_FMT_FULL_FIELD_GET2(5, ENTRY_6f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 6: {
        L3_DEFIP_ALPM_ROUTE_FMT6_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(6, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET2(6, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_GET2(6, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_GET2(6, ENTRY_3f, base_entry); break;
        case 4: ROUTE_FMT_FULL_FIELD_GET2(6, ENTRY_4f, base_entry); break;
        case 5: ROUTE_FMT_FULL_FIELD_GET2(6, ENTRY_5f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 7: {
        L3_DEFIP_ALPM_ROUTE_FMT7_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(7, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET2(7, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_GET2(7, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_GET2(7, ENTRY_3f, base_entry); break;
        case 4: ROUTE_FMT_FULL_FIELD_GET2(7, ENTRY_4f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 8: {
        L3_DEFIP_ALPM_ROUTE_FMT8_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(8, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET2(8, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_GET2(8, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_GET2(8, ENTRY_3f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 9: {
        L3_DEFIP_ALPM_ROUTE_FMT9_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(9, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET2(9, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_GET2(9, ENTRY_2f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 10: {
        L3_DEFIP_ALPM_ROUTE_FMT10_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(10, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET2(10, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_GET2(10, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_GET2(10, ENTRY_3f, base_entry); break;
        case 4: ROUTE_FMT_FULL_FIELD_GET2(10, ENTRY_4f, base_entry); break;
        case 5: ROUTE_FMT_FULL_FIELD_GET2(10, ENTRY_5f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 11: {
        L3_DEFIP_ALPM_ROUTE_FMT11_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(11, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET2(11, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_GET2(11, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_GET2(11, ENTRY_3f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 12: {
        L3_DEFIP_ALPM_ROUTE_FMT12_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(12, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET2(12, ENTRY_1f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 13: {
        L3_DEFIP_ALPM_ROUTE_FMT13_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_3f, base_entry); break;
        case 4: ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_4f, base_entry); break;
        case 5: ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_5f, base_entry); break;
        case 6: ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_6f, base_entry); break;
        case 7: ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_7f, base_entry); break;
        case 8: ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_8f, base_entry); break;
        case 9: ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_9f, base_entry); break;
        case 10: ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_10f, base_entry); break;
        case 11: ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_11f, base_entry); break;
        case 12: ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_12f, base_entry); break;
        case 13: ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_13f, base_entry); break;
        case 14: ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_14f, base_entry); break;
        case 15: ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_15f, base_entry); break;
        case 16: ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_16f, base_entry); break;
        case 17: ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_17f, base_entry); break;
        case 18: ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_18f, base_entry); break;
        case 19: ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_19f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 14: {
        L3_DEFIP_ALPM_ROUTE_FMT14_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(14, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET2(14, ENTRY_1f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 15: {
        L3_DEFIP_ALPM_ROUTE_FMT15_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(15, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET2(15, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_GET2(15, ENTRY_2f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 16: {
        L3_DEFIP_ALPM_ROUTE_FMT16_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(16, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET2(16, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_GET2(16, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_GET2(16, ENTRY_3f, base_entry); break;
        case 4: ROUTE_FMT_FULL_FIELD_GET2(16, ENTRY_4f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 17: {
        L3_DEFIP_ALPM_ROUTE_FMT17_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(17, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET2(17, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_GET2(17, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_GET2(17, ENTRY_3f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 18: {
        L3_DEFIP_ALPM_ROUTE_FMT18_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(18, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET2(18, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_GET2(18, ENTRY_2f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 19: {
        L3_DEFIP_ALPM_ROUTE_FMT19_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(19, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET2(19, ENTRY_1f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 20: {
        L3_DEFIP_ALPM_ROUTE_FMT20_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(20, ENTRY_0f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;

    case 21: {
        L3_DEFIP_ALPM_ROUTE_FMT21_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(21, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET2(21, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_GET2(21, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_GET2(21, ENTRY_3f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 22: {
        L3_DEFIP_ALPM_ROUTE_FMT22_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(22, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET2(22, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_GET2(22, ENTRY_2f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 23: {
        L3_DEFIP_ALPM_ROUTE_FMT23_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(23, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET2(23, ENTRY_1f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 24: {
        L3_DEFIP_ALPM_ROUTE_FMT24_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(24, ENTRY_0f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
}


static void
bcm56780_ln_route_format_full_decode(int u, int m, uint8_t format_value,
                                     ln_base_entry_t *base_entry,
                                     int entry_count,
                                     void *buf)
{
    format_value = bcm56780_l2p_route_fmts[format_value];
    switch (format_value) {
    case 1: {
        L3_DEFIP_ALPM_ROUTE_FMT1_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_4f, base_entry[4]);
        ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_5f, base_entry[5]);
        ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_6f, base_entry[6]);
        ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_7f, base_entry[7]);
        ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_8f, base_entry[8]);
        ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_9f, base_entry[9]);
        ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_10f, base_entry[10]);
        ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_11f, base_entry[11]);
        ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_12f, base_entry[12]);
        ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_13f, base_entry[13]);
        ROUTE_FMT_FULL_FIELD_GET(1, ENTRY_14f, base_entry[14]);
        ALPM_ASSERT(entry_count >= 14);
    } break;
    case 2: {
        L3_DEFIP_ALPM_ROUTE_FMT2_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_4f, base_entry[4]);
        ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_5f, base_entry[5]);
        ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_6f, base_entry[6]);
        ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_7f, base_entry[7]);
        ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_8f, base_entry[8]);
        ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_9f, base_entry[9]);
        ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_10f, base_entry[10]);
        ROUTE_FMT_FULL_FIELD_GET2(2, ENTRY_11f, base_entry[11]);
        ALPM_ASSERT(entry_count >= 11);
    } break;
    case 3: {
        L3_DEFIP_ALPM_ROUTE_FMT3_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(3, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(3, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(3, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_GET2(3, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_GET2(3, ENTRY_4f, base_entry[4]);
        ROUTE_FMT_FULL_FIELD_GET2(3, ENTRY_5f, base_entry[5]);
        ROUTE_FMT_FULL_FIELD_GET2(3, ENTRY_6f, base_entry[6]);
        ROUTE_FMT_FULL_FIELD_GET2(3, ENTRY_7f, base_entry[7]);
        ROUTE_FMT_FULL_FIELD_GET2(3, ENTRY_8f, base_entry[8]);
        ROUTE_FMT_FULL_FIELD_GET2(3, ENTRY_9f, base_entry[9]);
        ALPM_ASSERT(entry_count >= 9);
    } break;
    case 4: {
        L3_DEFIP_ALPM_ROUTE_FMT4_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(4, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(4, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(4, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_GET2(4, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_GET2(4, ENTRY_4f, base_entry[4]);
        ROUTE_FMT_FULL_FIELD_GET2(4, ENTRY_5f, base_entry[5]);
        ROUTE_FMT_FULL_FIELD_GET2(4, ENTRY_6f, base_entry[6]);
        ROUTE_FMT_FULL_FIELD_GET2(4, ENTRY_7f, base_entry[7]);
        ALPM_ASSERT(entry_count >= 7);
    } break;
    case 5: {
        L3_DEFIP_ALPM_ROUTE_FMT5_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(5, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(5, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(5, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_GET2(5, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_GET2(5, ENTRY_4f, base_entry[4]);
        ROUTE_FMT_FULL_FIELD_GET2(5, ENTRY_5f, base_entry[5]);
        ROUTE_FMT_FULL_FIELD_GET2(5, ENTRY_6f, base_entry[6]);
        ALPM_ASSERT(entry_count >= 6);
    } break;
    case 6: {
        L3_DEFIP_ALPM_ROUTE_FMT6_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(6, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(6, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(6, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_GET2(6, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_GET2(6, ENTRY_4f, base_entry[4]);
        ROUTE_FMT_FULL_FIELD_GET2(6, ENTRY_5f, base_entry[5]);
        ALPM_ASSERT(entry_count >= 5);
    } break;
    case 7: {
        L3_DEFIP_ALPM_ROUTE_FMT7_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(7, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(7, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(7, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_GET2(7, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_GET2(7, ENTRY_4f, base_entry[4]);
        ALPM_ASSERT(entry_count >= 4);
    } break;
    case 8: {
        L3_DEFIP_ALPM_ROUTE_FMT8_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(8, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(8, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(8, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_GET2(8, ENTRY_3f, base_entry[3]);
        ALPM_ASSERT(entry_count >= 3);
    } break;
    case 9: {
        L3_DEFIP_ALPM_ROUTE_FMT9_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(9, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(9, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(9, ENTRY_2f, base_entry[2]);
        ALPM_ASSERT(entry_count >= 2);
    } break;
    case 10: {
        L3_DEFIP_ALPM_ROUTE_FMT10_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(10, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(10, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(10, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_GET2(10, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_GET2(10, ENTRY_4f, base_entry[4]);
        ROUTE_FMT_FULL_FIELD_GET2(10, ENTRY_5f, base_entry[5]);
        ALPM_ASSERT(entry_count >= 5);
    } break;
    case 11: {
        L3_DEFIP_ALPM_ROUTE_FMT11_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(11, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(11, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(11, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_GET2(11, ENTRY_3f, base_entry[3]);
        ALPM_ASSERT(entry_count >= 3);
    } break;
    case 12: {
        L3_DEFIP_ALPM_ROUTE_FMT12_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(12, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(12, ENTRY_1f, base_entry[1]);
        ALPM_ASSERT(entry_count >= 1);
    } break;
    case 13: {
        L3_DEFIP_ALPM_ROUTE_FMT13_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_4f, base_entry[4]);
        ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_5f, base_entry[5]);
        ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_6f, base_entry[6]);
        ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_7f, base_entry[7]);
        ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_8f, base_entry[8]);
        ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_9f, base_entry[9]);
        ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_10f, base_entry[10]);
        ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_11f, base_entry[11]);
        ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_12f, base_entry[12]);
        ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_13f, base_entry[13]);
        ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_14f, base_entry[14]);
        ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_15f, base_entry[15]);
        ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_16f, base_entry[16]);
        ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_17f, base_entry[17]);
        ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_18f, base_entry[18]);
        ROUTE_FMT_FULL_FIELD_GET(13, ENTRY_19f, base_entry[19]);
        ALPM_ASSERT(entry_count >= 19);
    } break;
    case 14: {
        L3_DEFIP_ALPM_ROUTE_FMT14_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(14, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(14, ENTRY_1f, base_entry[1]);
        ALPM_ASSERT(entry_count >= 1);
    } break;
    case 15: {
        L3_DEFIP_ALPM_ROUTE_FMT15_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(15, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(15, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(15, ENTRY_2f, base_entry[2]);
        ALPM_ASSERT(entry_count >= 2);
    } break;
    case 16: {
        L3_DEFIP_ALPM_ROUTE_FMT16_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(16, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(16, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(16, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_GET2(16, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_GET2(16, ENTRY_4f, base_entry[4]);
        ALPM_ASSERT(entry_count >= 4);
    } break;
    case 17: {
        L3_DEFIP_ALPM_ROUTE_FMT17_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(17, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(17, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(17, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_GET2(17, ENTRY_3f, base_entry[3]);
        ALPM_ASSERT(entry_count >= 3);
    } break;
    case 18: {
        L3_DEFIP_ALPM_ROUTE_FMT18_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(18, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(18, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(18, ENTRY_2f, base_entry[2]);
        ALPM_ASSERT(entry_count >= 2);
    } break;
    case 19: {
        L3_DEFIP_ALPM_ROUTE_FMT19_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(19, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(19, ENTRY_1f, base_entry[1]);
        ALPM_ASSERT(entry_count >= 1);
    } break;
    case 20: {
        L3_DEFIP_ALPM_ROUTE_FMT20_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(20, ENTRY_0f, base_entry[0]);
        ALPM_ASSERT(entry_count >= 0);
    } break;

    case 21: {
        L3_DEFIP_ALPM_ROUTE_FMT21_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(21, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(21, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(21, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_GET2(21, ENTRY_3f, base_entry[3]);
        ALPM_ASSERT(entry_count >= 3);
    } break;
    case 22: {
        L3_DEFIP_ALPM_ROUTE_FMT22_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(22, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(22, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(22, ENTRY_2f, base_entry[2]);
        ALPM_ASSERT(entry_count >= 2);
    } break;
    case 23: {
        L3_DEFIP_ALPM_ROUTE_FMT23_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(23, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(23, ENTRY_1f, base_entry[1]);
        ALPM_ASSERT(entry_count >= 1);
    } break;
    case 24: {
        L3_DEFIP_ALPM_ROUTE_FMT24_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(24, ENTRY_0f, base_entry[0]);
        ALPM_ASSERT(entry_count >= 0);
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
}


static int
bcm56780_ln_base_entry_info(int u, int m,
                            format_type_t format_type,
                            uint8_t format_value,
                            int *prefix_len,
                            int *full,
                            int *num_entry)
{
    int a, b, c;
    if (format_type == FORMAT_ROUTE) {
        bcm56780_ln_route_base_entry_info(u, m, format_value, &a, &b, &c);
    } else {
        bcm56780_ln_pivot_base_entry_info(u, m, format_value, &a, &b, &c);
    }
    if (prefix_len) {
        *prefix_len = a;
    }
    if (full) {
        *full = b;
    }
    if (num_entry) {
        *num_entry = c;
    }
    return SHR_E_NONE;
}

static void
bcm56780_ln_route_format_set_data(uint8_t format_value,
                                  uint32_t *ad_format,
                                  void *buf)
{
    format_value = bcm56780_l2p_route_fmts[format_value];
    switch (format_value) {
    case 1: {
        L3_DEFIP_ALPM_ROUTE_FMT1_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(1, ASSOC_DATAf, ad_format);
    } break;
    case 2: {
        L3_DEFIP_ALPM_ROUTE_FMT2_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(2, ASSOC_DATAf, ad_format);
    } break;
    case 3: {
        L3_DEFIP_ALPM_ROUTE_FMT3_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(3, ASSOC_DATAf, ad_format);
    } break;
    case 4: {
        L3_DEFIP_ALPM_ROUTE_FMT4_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(4, ASSOC_DATAf, ad_format);
    } break;
    case 5: {
        L3_DEFIP_ALPM_ROUTE_FMT5_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(5, ASSOC_DATAf, ad_format);
    } break;
    case 6: {
        L3_DEFIP_ALPM_ROUTE_FMT6_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(6, ASSOC_DATAf, ad_format);
    } break;
    case 7: {
        L3_DEFIP_ALPM_ROUTE_FMT7_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(7, ASSOC_DATAf, ad_format);
    } break;
    case 8: {
        L3_DEFIP_ALPM_ROUTE_FMT8_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(8, ASSOC_DATAf, ad_format);
    } break;
    case 9: {
        L3_DEFIP_ALPM_ROUTE_FMT9_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(9, ASSOC_DATAf, ad_format);
    } break;
    case 10: {
        L3_DEFIP_ALPM_ROUTE_FMT10_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(10, ASSOC_DATAf, ad_format);
    } break;
    case 11: {
        L3_DEFIP_ALPM_ROUTE_FMT11_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(11, ASSOC_DATAf, ad_format);
    } break;
    case 12: {
        L3_DEFIP_ALPM_ROUTE_FMT12_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(12, ASSOC_DATAf, ad_format);
    } break;
    case 13: {
        L3_DEFIP_ALPM_ROUTE_FMT13_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(13, ASSOC_DATAf, ad_format);
    } break;
    case 14: {
        L3_DEFIP_ALPM_ROUTE_FMT14_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(14, ASSOC_DATAf, ad_format);
    } break;
    case 15: {
        L3_DEFIP_ALPM_ROUTE_FMT15_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(15, ASSOC_DATAf, ad_format);
    } break;
    case 16: {
        L3_DEFIP_ALPM_ROUTE_FMT16_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(16, ASSOC_DATAf, ad_format);
    } break;
    case 17: {
        L3_DEFIP_ALPM_ROUTE_FMT17_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(17, ASSOC_DATAf, ad_format);
    } break;
    case 18: {
        L3_DEFIP_ALPM_ROUTE_FMT18_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(18, ASSOC_DATAf, ad_format);
    } break;
    case 19: {
        L3_DEFIP_ALPM_ROUTE_FMT19_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(19, ASSOC_DATAf, ad_format);
    } break;
    case 20: {
        L3_DEFIP_ALPM_ROUTE_FMT20_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(20, ASSOC_DATAf, ad_format);
    } break;
    case 21: {
        L3_DEFIP_ALPM_ROUTE_FMT21_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(21, ASSOC_DATAf, ad_format);
    } break;
    case 22: {
        L3_DEFIP_ALPM_ROUTE_FMT22_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(22, ASSOC_DATAf, ad_format);
    } break;
    case 23: {
        L3_DEFIP_ALPM_ROUTE_FMT23_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(23, ASSOC_DATAf, ad_format);
    } break;
    case 24: {
        L3_DEFIP_ALPM_ROUTE_FMT24_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(24, ASSOC_DATAf, ad_format);
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56780_ln_route_format_set(uint8_t format_value,
                             uint32_t *ad_format,
                             alpm_format_ip_t prefix,
                             void *buf)
{
    format_value = bcm56780_l2p_route_fmts[format_value];
    switch (format_value) {
    case 1: {
        L3_DEFIP_ALPM_ROUTE_FMT1_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(1, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET(1, ASSOC_DATAf, ad_format);
    } break;
    case 2: {
        L3_DEFIP_ALPM_ROUTE_FMT2_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(2, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET(2, ASSOC_DATAf, ad_format);
    } break;
    case 3: {
        L3_DEFIP_ALPM_ROUTE_FMT3_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(3, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET(3, ASSOC_DATAf, ad_format);
    } break;
    case 4: {
        L3_DEFIP_ALPM_ROUTE_FMT4_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(4, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET(4, ASSOC_DATAf, ad_format);
    } break;
    case 5: {
        L3_DEFIP_ALPM_ROUTE_FMT5_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(5, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET(5, ASSOC_DATAf, ad_format);
    } break;
    case 6: {
        L3_DEFIP_ALPM_ROUTE_FMT6_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(6, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET(6, ASSOC_DATAf, ad_format);
    } break;
    case 7: {
        L3_DEFIP_ALPM_ROUTE_FMT7_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(7, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET(7, ASSOC_DATAf, ad_format);
    } break;
    case 8: {
        L3_DEFIP_ALPM_ROUTE_FMT8_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(8, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET(8, ASSOC_DATAf, ad_format);
    } break;
    case 9: {
        L3_DEFIP_ALPM_ROUTE_FMT9_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(9, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET(9, ASSOC_DATAf, ad_format);
    } break;
    case 10: {
        L3_DEFIP_ALPM_ROUTE_FMT10_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(10, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET2(10, ASSOC_DATAf, ad_format);
    } break;
    case 11: {
        L3_DEFIP_ALPM_ROUTE_FMT11_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(11, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET2(11, ASSOC_DATAf, ad_format);
    } break;
    case 12: {
        L3_DEFIP_ALPM_ROUTE_FMT12_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(12, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET2(12, ASSOC_DATAf, ad_format);
    } break;
    case 13: {
        L3_DEFIP_ALPM_ROUTE_FMT13_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(13, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET(13, ASSOC_DATAf, ad_format);
    } break;
    case 14: {
        L3_DEFIP_ALPM_ROUTE_FMT14_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(14, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET(14, ASSOC_DATAf, ad_format);
    } break;
    case 15: {
        L3_DEFIP_ALPM_ROUTE_FMT15_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(15, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET2(15, ASSOC_DATAf, ad_format);
    } break;
    case 16: {
        L3_DEFIP_ALPM_ROUTE_FMT16_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(16, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET2(16, ASSOC_DATAf, ad_format);
    } break;
    case 17: {
        L3_DEFIP_ALPM_ROUTE_FMT17_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(17, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET2(17, ASSOC_DATAf, ad_format);
    } break;
    case 18: {
        L3_DEFIP_ALPM_ROUTE_FMT18_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(18, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET2(18, ASSOC_DATAf, ad_format);
    } break;
    case 19: {
        L3_DEFIP_ALPM_ROUTE_FMT19_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(19, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET2(19, ASSOC_DATAf, ad_format);
    } break;
    case 20: {
        L3_DEFIP_ALPM_ROUTE_FMT20_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(20, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET2(20, ASSOC_DATAf, ad_format);
    } break;
    case 21: {
        L3_DEFIP_ALPM_ROUTE_FMT21_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(21, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET2(21, ASSOC_DATAf, ad_format);
    } break;
    case 22: {
        L3_DEFIP_ALPM_ROUTE_FMT22_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(22, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET2(22, ASSOC_DATAf, ad_format);
    } break;
    case 23: {
        L3_DEFIP_ALPM_ROUTE_FMT23_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(23, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET2(23, ASSOC_DATAf, ad_format);
    } break;
    case 24: {
        L3_DEFIP_ALPM_ROUTE_FMT24_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(24, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET2(24, ASSOC_DATAf, ad_format);
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
}


static void
bcm56780_ln_route_format_encode(int u, int m, uint8_t format_value,
                                uint8_t kshift,
                                alpm_arg_t *arg,
                                void *buf)
{
    int full;
    int max_len;
    int number;
    bcm56780_assoc_data_t ad_format;
    bcm56780_ln_route_base_entry_info(u, m, format_value, &max_len, &full,
                                      &number);
    bcm56780_assoc_data_format_encode(u, m, &ad_format, arg->key.t.app, full,
                                      &(arg->ad));
    if (arg->encode_data_only) {
        bcm56780_ln_route_format_set_data(format_value,
                                          &ad_format.v[0],
                                          buf);
    } else {
        alpm_format_ip_t prefix;
        bcmptm_rm_alpm_one_bit_encode(arg->key.t.max_bits,
                                      &arg->key.t.trie_ip,
                                      arg->key.t.len,
                                      max_len,
                                      kshift,
                                      &prefix[0]);
        bcm56780_ln_route_format_set(format_value,
                                     &ad_format.v[0],
                                     &prefix[0],
                                     buf);
    }
}

static void
bcm56780_ln_route_format_get_data(uint8_t format_value,
                                  uint32_t *ad_format,
                                  void *buf)
{
    format_value = bcm56780_l2p_route_fmts[format_value];
    switch (format_value) {
    case 1: {
        L3_DEFIP_ALPM_ROUTE_FMT1_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(1, ASSOC_DATAf, ad_format);
    } break;
    case 2: {
        L3_DEFIP_ALPM_ROUTE_FMT2_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(2, ASSOC_DATAf, ad_format);
    } break;
    case 3: {
        L3_DEFIP_ALPM_ROUTE_FMT3_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(3, ASSOC_DATAf, ad_format);
    } break;
    case 4: {
        L3_DEFIP_ALPM_ROUTE_FMT4_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(4, ASSOC_DATAf, ad_format);
    } break;
    case 5: {
        L3_DEFIP_ALPM_ROUTE_FMT5_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(5, ASSOC_DATAf, ad_format);
    } break;
    case 6: {
        L3_DEFIP_ALPM_ROUTE_FMT6_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(6, ASSOC_DATAf, ad_format);
    } break;
    case 7: {
        L3_DEFIP_ALPM_ROUTE_FMT7_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(7, ASSOC_DATAf, ad_format);
    } break;
    case 8: {
        L3_DEFIP_ALPM_ROUTE_FMT8_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(8, ASSOC_DATAf, ad_format);
    } break;
    case 9: {
        L3_DEFIP_ALPM_ROUTE_FMT9_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(9, ASSOC_DATAf, ad_format);
    } break;
    case 10: {
        L3_DEFIP_ALPM_ROUTE_FMT10_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(10, ASSOC_DATAf, ad_format);
    } break;
    case 11: {
        L3_DEFIP_ALPM_ROUTE_FMT11_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(11, ASSOC_DATAf, ad_format);
    } break;
    case 12: {
        L3_DEFIP_ALPM_ROUTE_FMT12_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(12, ASSOC_DATAf, ad_format);
    } break;
    case 13: {
        L3_DEFIP_ALPM_ROUTE_FMT13_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(13, ASSOC_DATAf, ad_format);
    } break;
    case 14: {
        L3_DEFIP_ALPM_ROUTE_FMT14_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(14, ASSOC_DATAf, ad_format);
    } break;
    case 15: {
        L3_DEFIP_ALPM_ROUTE_FMT15_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(15, ASSOC_DATAf, ad_format);
    } break;
    case 16: {
        L3_DEFIP_ALPM_ROUTE_FMT16_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(16, ASSOC_DATAf, ad_format);
    } break;
    case 17: {
        L3_DEFIP_ALPM_ROUTE_FMT17_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(17, ASSOC_DATAf, ad_format);
    } break;
    case 18: {
        L3_DEFIP_ALPM_ROUTE_FMT18_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(18, ASSOC_DATAf, ad_format);
    } break;
    case 19: {
        L3_DEFIP_ALPM_ROUTE_FMT19_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(19, ASSOC_DATAf, ad_format);
    } break;
    case 20: {
        L3_DEFIP_ALPM_ROUTE_FMT20_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(20, ASSOC_DATAf, ad_format);
    } break;
    case 21: {
        L3_DEFIP_ALPM_ROUTE_FMT21_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(21, ASSOC_DATAf, ad_format);
    } break;
    case 22: {
        L3_DEFIP_ALPM_ROUTE_FMT22_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(22, ASSOC_DATAf, ad_format);
    } break;
    case 23: {
        L3_DEFIP_ALPM_ROUTE_FMT23_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(23, ASSOC_DATAf, ad_format);
    } break;
    case 24: {
        L3_DEFIP_ALPM_ROUTE_FMT24_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(24, ASSOC_DATAf, ad_format);
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56780_ln_route_format_get(uint8_t format_value,
                             uint32_t *ad_format,
                             alpm_format_ip_t prefix,
                             void *buf)
{
    format_value = bcm56780_l2p_route_fmts[format_value];
    switch (format_value) {
    case 1: {
        L3_DEFIP_ALPM_ROUTE_FMT1_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(1, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET(1, ASSOC_DATAf, ad_format);
    } break;
    case 2: {
        L3_DEFIP_ALPM_ROUTE_FMT2_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(2, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET(2, ASSOC_DATAf, ad_format);
    } break;
    case 3: {
        L3_DEFIP_ALPM_ROUTE_FMT3_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(3, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET(3, ASSOC_DATAf, ad_format);
    } break;
    case 4: {
        L3_DEFIP_ALPM_ROUTE_FMT4_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(4, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET(4, ASSOC_DATAf, ad_format);
    } break;
    case 5: {
        L3_DEFIP_ALPM_ROUTE_FMT5_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(5, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET(5, ASSOC_DATAf, ad_format);
    } break;
    case 6: {
        L3_DEFIP_ALPM_ROUTE_FMT6_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(6, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET(6, ASSOC_DATAf, ad_format);
    } break;
    case 7: {
        L3_DEFIP_ALPM_ROUTE_FMT7_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(7, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET(7, ASSOC_DATAf, ad_format);
    } break;
    case 8: {
        L3_DEFIP_ALPM_ROUTE_FMT8_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(8, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET(8, ASSOC_DATAf, ad_format);
    } break;
    case 9: {
        L3_DEFIP_ALPM_ROUTE_FMT9_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(9, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET(9, ASSOC_DATAf, ad_format);
    } break;
    case 10: {
        L3_DEFIP_ALPM_ROUTE_FMT10_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(10, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET2(10, ASSOC_DATAf, ad_format);
    } break;
    case 11: {
        L3_DEFIP_ALPM_ROUTE_FMT11_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(11, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET2(11, ASSOC_DATAf, ad_format);
    } break;
    case 12: {
        L3_DEFIP_ALPM_ROUTE_FMT12_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(12, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET2(12, ASSOC_DATAf, ad_format);
    } break;
    case 13: {
        L3_DEFIP_ALPM_ROUTE_FMT13_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(13, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET(13, ASSOC_DATAf, ad_format);
    } break;
    case 14: {
        L3_DEFIP_ALPM_ROUTE_FMT14_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(14, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET(14, ASSOC_DATAf, ad_format);
    } break;
    case 15: {
        L3_DEFIP_ALPM_ROUTE_FMT15_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(15, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET2(15, ASSOC_DATAf, ad_format);
    } break;
    case 16: {
        L3_DEFIP_ALPM_ROUTE_FMT16_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(16, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET2(16, ASSOC_DATAf, ad_format);
    } break;
    case 17: {
        L3_DEFIP_ALPM_ROUTE_FMT17_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(17, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET2(17, ASSOC_DATAf, ad_format);
    } break;
    case 18: {
        L3_DEFIP_ALPM_ROUTE_FMT18_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(18, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET2(18, ASSOC_DATAf, ad_format);
    } break;
    case 19: {
        L3_DEFIP_ALPM_ROUTE_FMT19_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(19, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET2(19, ASSOC_DATAf, ad_format);
    } break;
    case 20: {
        L3_DEFIP_ALPM_ROUTE_FMT20_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(20, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET2(20, ASSOC_DATAf, ad_format);
    } break;
    case 21: {
        L3_DEFIP_ALPM_ROUTE_FMT21_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(21, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET2(21, ASSOC_DATAf, ad_format);
    } break;
    case 22: {
        L3_DEFIP_ALPM_ROUTE_FMT22_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(22, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET2(22, ASSOC_DATAf, ad_format);
    } break;
    case 23: {
        L3_DEFIP_ALPM_ROUTE_FMT23_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(23, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET2(23, ASSOC_DATAf, ad_format);
    } break;
    case 24: {
        L3_DEFIP_ALPM_ROUTE_FMT24_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(24, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET2(24, ASSOC_DATAf, ad_format);
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56780_ln_route_format_decode(int u, int m, uint8_t format_value,
                                uint8_t kshift,
                                alpm_arg_t *arg,
                                void *buf)
{
    int full;
    int max_len;
    int number;
    bcm56780_assoc_data_t ad_format = {{{0}}};
    alpm_format_ip_t prefix = {0};
    bcm56780_ln_route_base_entry_info(u, m, format_value, &max_len, &full,
                                      &number);
    if (arg->decode_data_only) {
        bcm56780_ln_route_format_get_data(format_value,
                                          &ad_format.v[0],
                                          buf);
    } else {
        bcm56780_ln_route_format_get(format_value,
                                     &ad_format.v[0],
                                     &prefix[0],
                                     buf);
        arg->valid = bcmptm_rm_alpm_one_bit_decode(arg->key.t.max_bits,
                                                   &arg->key.t.trie_ip,
                                                   &arg->key.t.len,
                                                   max_len,
                                                   kshift,
                                                   &prefix[0]);
    }
    bcm56780_assoc_data_format_decode(u, m, &ad_format, arg->key.t.app, full,
                                      &arg->ad);
}

static int
bcm56780_ln_raw_bucket_encode(int u, int m, format_type_t format_type,
                              uint8_t format_value,
                              ln_base_entry_t *base_entry,
                              int entry_count,
                              void *bucket)
{
    if (format_type == FORMAT_ROUTE) {
        bcm56780_ln_route_format_full_encode(u, m, format_value, base_entry,
                                             entry_count, bucket);
    } else {
        bcm56780_ln_pivot_format_full_encode(u, m, format_value, base_entry,
                                             entry_count, bucket);
    }
    return SHR_E_NONE;
}


static int
bcm56780_ln_raw_bucket_entry_set(int u, int m, format_type_t format_type,
                                 uint8_t format_value,
                                 uint32_t *base_entry,
                                 int ent_ofs,
                                 void *bucket)
{
    if (format_type == FORMAT_ROUTE) {
        bcm56780_ln_route_format_full_encode2(u, m, format_value, base_entry,
                                              ent_ofs, bucket);
    } else {
        bcm56780_ln_pivot_format_full_encode2(u, m, format_value, base_entry,
                                              ent_ofs, bucket);
    }
    return SHR_E_NONE;
}


static int
bcm56780_ln_raw_bucket_decode(int u, int m, format_type_t format_type,
                              uint8_t format_value,
                              ln_base_entry_t *base_entry,
                              int entry_count,
                              void *bucket)
{
    if (format_type == FORMAT_ROUTE) {
        bcm56780_ln_route_format_full_decode(u, m, format_value, base_entry,
                                             entry_count, bucket);
    } else {
        bcm56780_ln_pivot_format_full_decode(u, m, format_value, base_entry,
                                             entry_count, bucket);
    }
    return SHR_E_NONE;
}


static int
bcm56780_ln_raw_bucket_entry_get(int u, int m, format_type_t format_type,
                                 uint8_t format_value,
                                 uint32_t *base_entry,
                                 int ent_ofs,
                                 void *bucket)
{
    if (format_type == FORMAT_ROUTE) {
        bcm56780_ln_route_format_full_decode2(u, m, format_value, base_entry,
                                              ent_ofs, bucket);
    } else {
        bcm56780_ln_pivot_format_full_decode2(u, m, format_value, base_entry,
                                              ent_ofs, bucket);
    }
    return SHR_E_NONE;
}


static int
bcm56780_ln_base_entry_encode(int u, int m, format_type_t format_type,
                              uint8_t format_value,
                              uint8_t kshift,
                              alpm_arg_t *arg,
                              uint32_t *entry)
{
    if (format_type == FORMAT_ROUTE) {
        bcm56780_ln_route_format_encode(u, m, format_value, kshift, arg, entry);
    } else {
        bcm56780_ln_pivot_format_encode(u, m, format_value, kshift, arg, entry);
    }
    return SHR_E_NONE;
}

static int
bcm56780_ln_base_entry_decode(int u, int m, format_type_t format_type,
                              uint8_t format_value,
                              uint8_t kshift,
                              alpm_arg_t *arg,
                              uint32_t *entry)
{
    if (format_type == FORMAT_ROUTE) {
        bcm56780_ln_route_format_decode(u, m, format_value, kshift, arg, entry);
    } else {
        bcm56780_ln_pivot_format_decode(u, m, format_value, kshift, arg, entry);
    }
    return SHR_E_NONE;
}

static int
bcm56780_ln_base_entry_move(int u, int m, format_type_t format_type,
                            uint8_t src_format_value,
                            int src_ent_ofs,
                            key_tuple_t *src_t,
                            void *src_bucket,
                            alpm_data_type_t src_dt,

                            uint8_t dst_format_value,
                            int dst_ent_ofs,
                            key_tuple_t *dst_t,
                            void *dst_bucket)
{
    ln_base_entry_t src_base_entry;
    ln_base_entry_t dst_base_entry;
    static ln_base_entry_t zero_base_entry = {0};
    uint32_t *dst_base;
    int src_kshift, dst_kshift;
    alpm_arg_t *arg = NULL;
    SHR_FUNC_ENTER(u);

    ALPM_ASSERT(src_t && dst_t);
    src_kshift = src_t->len;
    dst_kshift = dst_t->len;

#ifdef ALPM_DEBUG
    if (LOG_CHECK_INFO(BSL_LOG_MODULE)) {
        sal_memset(src_base_entry, 0, sizeof(src_base_entry));
        sal_memset(dst_base_entry, 0, sizeof(dst_base_entry));
    }
#endif

    SHR_IF_ERR_EXIT(
        bcm56780_ln_raw_bucket_entry_get(u, m, format_type,
                                         src_format_value,
                                         &src_base_entry[0],
                                         src_ent_ofs,
                                         src_bucket));

    if (src_format_value == dst_format_value &&
        src_kshift == dst_kshift) {
#ifdef ALPM_DEBUG
        if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
            arg = shr_lmm_alloc(bcmptm_rm_alpm_arg_lmem[u][m]);
            if (arg == NULL) {
                SHR_ERR_EXIT(SHR_E_MEMORY);
            }
            sal_memset(arg, 0, sizeof(*arg));

            arg->key.t.max_bits = src_t->max_bits;
            arg->key.t.ipv = src_t->ipv;
            sal_memcpy(&arg->key.t.trie_ip, &src_t->trie_ip,
                       sizeof(src_t->trie_ip));
            SHR_IF_ERR_EXIT(
                bcm56780_ln_base_entry_decode(u, m, format_type,
                                              src_format_value,
                                              src_kshift,
                                              arg,
                                              &src_base_entry[0]));
            ALPM_ASSERT(arg->key.t.len >= src_kshift);
            SHR_IF_ERR_EXIT(
                bcm56780_ln_base_entry_encode(u, m, format_type,
                                              dst_format_value,
                                              dst_kshift,
                                              arg,
                                              &dst_base_entry[0]));
            ALPM_ASSERT(!sal_memcmp(&dst_base_entry[0], &src_base_entry[0],
                                    sizeof(ln_base_entry_t)));
            dst_base = &dst_base_entry[0];
        } else
#endif
        {
            dst_base = &src_base_entry[0];
        }
    } else {
        arg = shr_lmm_alloc(bcmptm_rm_alpm_arg_lmem[u][m]);
        if (arg == NULL) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        arg->encode_data_only = 0;
        arg->decode_data_only = 0;
        arg->ad.user_data_type = src_dt;
        /* bcmptm_rm_alpm_arg_init(u, arg);*/

        arg->key.t.max_bits = src_t->max_bits;
        arg->key.t.ipv = src_t->ipv;
        sal_memcpy(&arg->key.t.trie_ip, &src_t->trie_ip,
                   sizeof(src_t->trie_ip));
        SHR_IF_ERR_EXIT(
            bcm56780_ln_base_entry_decode(u, m, format_type,
                                          src_format_value,
                                          src_kshift,
                                          arg,
                                          &src_base_entry[0]));
        ALPM_ASSERT(arg->key.t.len >= src_kshift);
        ALPM_ASSERT(arg->key.t.len >= dst_kshift);
        SHR_IF_ERR_EXIT(
            bcm56780_ln_base_entry_encode(u, m, format_type,
                                          dst_format_value,
                                          dst_kshift,
                                          arg,
                                          &dst_base_entry[0]));
        dst_base = &dst_base_entry[0];
    }
    SHR_IF_ERR_EXIT(
        bcm56780_ln_raw_bucket_entry_set(u, m, format_type,
                                         dst_format_value,
                                         dst_base,
                                         dst_ent_ofs,
                                         dst_bucket));
    SHR_IF_ERR_EXIT(
        bcm56780_ln_raw_bucket_entry_set(u, m, format_type,
                                         src_format_value,
                                         &zero_base_entry[0],
                                         src_ent_ofs,
                                         src_bucket));
exit:
    if (arg) {
        shr_lmm_free(bcmptm_rm_alpm_arg_lmem[u][m], arg);
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Encode key part into single-wide entry.
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] de DIP entry
 * \param [out] se SIP entry
 *
 * \return SHR_E_XXX.
 */
static void
bcm56780_l1_key_view_lpm_encode_n1(int u, int m, alpm_arg_t *arg, void *e,
                                   int upkm)
{
    L3_DEFIP_TCAM_KEY_t kdata;
    L3_DEFIP_TCAM_KEY_t kmask;
    int vrf, vrf_mask;
    uint8_t large_vrf;
    TCAM_VIEW_T *de = e;
    M_TCAM_VIEW_T *mde = e;

    L3_DEFIP_TCAM_KEY_CLR(kdata);
    L3_DEFIP_TCAM_KEY_CLR(kmask);

    if (arg->key.vt == VRF_OVERRIDE) {
        vrf = 0;
        vrf_mask = 0;
    } else if (arg->key.vt == VRF_GLOBAL) {
        vrf = 0;
        vrf_mask = 0;
    } else {
        vrf = arg->key.t.w_vrf;
        vrf_mask = bcm56780_alpm_max_vrf[u];
    }

    large_vrf = arg->lpm_large_vrf && arg->key.vt != VRF_OVERRIDE;
    if (arg->key.kt == KEY_IPV4) {
        arg->key.ip_addr[0] &= arg->key.ip_mask[0];

        if (large_vrf) {
            bcm56780_l1_key_lpm_v4_encode_no_vrf(u, m, HALF_WIDE,
                                                 bcm56780_lpm_v4_key_type[u][m],
                                                 &arg->key.ip_addr[0], &kdata);
            bcm56780_l1_key_lpm_v4_encode_no_vrf(u, m, 0x3, 0x3,
                                                 &arg->key.ip_mask[0], &kmask);
        } else {
            bcm56780_l1_key_lpm_v4_encode(u, m, vrf, HALF_WIDE,
                                          bcm56780_lpm_v4_key_type[u][m],
                                          arg->key.ip_addr[0], &kdata);
            bcm56780_l1_key_lpm_v4_encode(u, m, vrf_mask, 0x3, 0x3,
                                          arg->key.ip_mask[0], &kmask);
        }
        if (m) {
            M_KEY_VIEW_FIELD_SET2(KEY_ENTRY_1f, kdata);
            M_KEY_VIEW_FIELD_SET2(MASK_ENTRY_1f, kmask);
            M_KEY_VIEW_FIELD_SET(VALID_1f, arg->valid);
        } else {
            KEY_VIEW_FIELD_SET2(KEY_ENTRY_1f, kdata);
            KEY_VIEW_FIELD_SET2(MASK_ENTRY_1f, kmask);
            KEY_VIEW_FIELD_SET(VALID_1f, arg->valid);
        }
    } else if (arg->key.kt == KEY_IPV6_SINGLE &&
               (upkm == PKM_S || upkm == PKM_SS)) {
        /* Apply subnet kmask. */
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[0] == 0);
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[1] == 0);
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[2] == 0);
        arg->key.ip_addr[3] &= arg->key.ip_mask[3];

        if (large_vrf) {
            bcm56780_l1_key_lpm_v6_half_encode_no_vrf(u, m, HALF_WIDE,
                                                      bcm56780_lpm_v6_key_type[u][m],
                                                      &arg->key.ip_addr[0],
                                                      &kdata);
            bcm56780_l1_key_lpm_v6_half_encode_no_vrf(u, m, 0x3, 0x3,
                                                      &arg->key.ip_mask[0],
                                                      &kmask);
        } else {
            bcm56780_l1_key_lpm_v6_half_encode(u, m, vrf, HALF_WIDE,
                                               bcm56780_lpm_v6_key_type[u][m],
                                               &arg->key.ip_addr[0], &kdata);
            bcm56780_l1_key_lpm_v6_half_encode(u, m, vrf_mask, 0x3, 0x3,
                                               &arg->key.ip_mask[0], &kmask);
        }
        if (m) {
            M_KEY_VIEW_FIELD_SET2(KEY_ENTRY_1f, kdata);
            M_KEY_VIEW_FIELD_SET2(MASK_ENTRY_1f, kmask);
            M_KEY_VIEW_FIELD_SET(VALID_1f, arg->valid);
        } else {
            KEY_VIEW_FIELD_SET2(KEY_ENTRY_1f, kdata);
            KEY_VIEW_FIELD_SET2(MASK_ENTRY_1f, kmask);
            KEY_VIEW_FIELD_SET(VALID_1f, arg->valid);
        }
    }
}


/*!
 * \brief Encode key part into single-wide entry.
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] de DIP entry
 * \param [out] se SIP entry
 *
 * \return SHR_E_XXX.
 */
static void
bcm56780_l1_key_view_lpm_encode(int u, int m, alpm_arg_t *arg, void *e,
                                void *e2, int upkm)
{
    L3_DEFIP_TCAM_KEY_t kdata[4] = {{{0}}};
    L3_DEFIP_TCAM_KEY_t kmask[4] = {{{0}}};
    int vrf, vrf_mask;
    uint8_t large_vrf;
    TCAM_VIEW_T *de = e, *de_upr = e2;
    M_TCAM_VIEW_T *mde = e, *mde_upr = e2;

    if (arg->key.vt == VRF_OVERRIDE) {
        vrf = 0;
        vrf_mask = 0;
    } else if (arg->key.vt == VRF_GLOBAL) {
        vrf = 0;
        vrf_mask = 0;
    } else {
        vrf = arg->key.t.w_vrf;
        vrf_mask = bcm56780_alpm_max_vrf[u];
    }

    large_vrf = arg->lpm_large_vrf && arg->key.vt != VRF_OVERRIDE;
    if (arg->key.kt == KEY_IPV4) {
        arg->key.ip_addr[0] &= arg->key.ip_mask[0];

        if (large_vrf) {
            bcm56780_l1_key_lpm_v4_encode_no_vrf(u, m, HALF_WIDE,
                                                 bcm56780_lpm_v4_key_type[u][m],
                                                 &arg->key.ip_addr[0],
                                                 &kdata[0]);
            bcm56780_l1_key_lpm_v4_encode_no_vrf(u, m, 0x3, 0x3,
                                                 &arg->key.ip_mask[0],
                                                 &kmask[0]);
        } else {
            bcm56780_l1_key_lpm_v4_encode(u, m, vrf, HALF_WIDE,
                                          bcm56780_lpm_v4_key_type[u][m],
                                          arg->key.ip_addr[0], &kdata[0]);
            bcm56780_l1_key_lpm_v4_encode(u, m, vrf_mask, 0x3, 0x3,
                                          arg->key.ip_mask[0], &kmask[0]);
        }
        if (m) {
            M_KEY_VIEW_FIELD_SET2(KEY_ENTRY_0f, kdata[0]);
            M_KEY_VIEW_FIELD_SET2(MASK_ENTRY_0f, kmask[0]);
            M_KEY_VIEW_FIELD_SET(VALID_0f, arg->valid);
        } else {
            KEY_VIEW_FIELD_SET2(KEY_ENTRY_0f, kdata[0]);
            KEY_VIEW_FIELD_SET2(MASK_ENTRY_0f, kmask[0]);
            KEY_VIEW_FIELD_SET(VALID_0f, arg->valid);
        }
    } else if (arg->key.kt == KEY_IPV6_SINGLE &&
               (upkm == PKM_S || upkm == PKM_SS)) {
        /* Apply subnet kmask. */
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[0] == 0);
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[1] == 0);
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[2] == 0);
        arg->key.ip_addr[3] &= arg->key.ip_mask[3];

        if (large_vrf) {
            bcm56780_l1_key_lpm_v6_half_encode_no_vrf(u, m, HALF_WIDE,
                                                      bcm56780_lpm_v6_key_type[u][m],
                                                      &arg->key.ip_addr[0],
                                                      &kdata[0]);
            bcm56780_l1_key_lpm_v6_half_encode_no_vrf(u, m, 0x3, 0x3,
                                                      &arg->key.ip_mask[0],
                                                      &kmask[0]);
        } else {
            bcm56780_l1_key_lpm_v6_half_encode(u, m, vrf, HALF_WIDE,
                                               bcm56780_lpm_v6_key_type[u][m],
                                               &arg->key.ip_addr[0], &kdata[0]);
            bcm56780_l1_key_lpm_v6_half_encode(u, m, vrf_mask, 0x3, 0x3,
                                               &arg->key.ip_mask[0], &kmask[0]);
        }
        if (m) {
            M_KEY_VIEW_FIELD_SET2(KEY_ENTRY_0f, kdata[0]);
            M_KEY_VIEW_FIELD_SET2(MASK_ENTRY_0f, kmask[0]);
            M_KEY_VIEW_FIELD_SET(VALID_0f, arg->valid);
        } else {
            KEY_VIEW_FIELD_SET2(KEY_ENTRY_0f, kdata[0]);
            KEY_VIEW_FIELD_SET2(MASK_ENTRY_0f, kmask[0]);
            KEY_VIEW_FIELD_SET(VALID_0f, arg->valid);
        }
    } else if (upkm == PKM_Q || upkm == PKM_SQ) {
        /* Apply subnet kmask. */
        arg->key.ip_addr[0] &= arg->key.ip_mask[0];
        arg->key.ip_addr[1] &= arg->key.ip_mask[1];
        arg->key.ip_addr[2] &= arg->key.ip_mask[2];
        arg->key.ip_addr[3] &= arg->key.ip_mask[3];

        large_vrf = arg->lpm_large_vrf && arg->key.vt != VRF_OVERRIDE;
        if (large_vrf) {
            bcm56780_l1_key_lpm_v6_double_encode_no_vrf(u, m, DOUBLE_WIDE,
                                                        bcm56780_lpm_v6_key_type[u][m],
                                                        &arg->key.ip_addr[0],
                                                        &kdata[0],
                                                        &kdata[1],
                                                        &kdata[2],
                                                        &kdata[3]);
            bcm56780_l1_key_lpm_v6_double_encode_no_vrf(u, m, 0x3, 0x3,
                                                        &arg->key.ip_mask[0],
                                                        &kmask[0],
                                                        &kmask[1],
                                                        &kmask[2],
                                                        &kmask[3]);
        } else {
            bcm56780_l1_key_lpm_v6_double_encode(u, m, vrf, DOUBLE_WIDE,
                                                 bcm56780_lpm_v6_key_type[u][m],
                                                 &arg->key.ip_addr[0],
                                                 &kdata[0],
                                                 &kdata[1],
                                                 &kdata[2],
                                                 &kdata[3]);
            bcm56780_l1_key_lpm_v6_double_encode(u, m, vrf_mask, 0x3, 0x3,
                                                 &arg->key.ip_mask[0],
                                                 &kmask[0],
                                                 &kmask[1],
                                                 &kmask[2],
                                                 &kmask[3]);
        }
        if (m) {
            M_KEY_VIEW_FIELD_SET2(KEY_ENTRY_0f, kdata[0]);
            M_KEY_VIEW_FIELD_SET2(KEY_ENTRY_1f, kdata[1]);
            M_KEY_VIEW_FIELD_SET2(MASK_ENTRY_0f, kmask[0]);
            M_KEY_VIEW_FIELD_SET2(MASK_ENTRY_1f, kmask[1]);
            M_KEY_VIEW_FIELD_SET(VALID_0f, arg->valid);
            M_KEY_VIEW_FIELD_SET(VALID_1f, arg->valid);

            M_UPR_KEY_VIEW_FIELD_SET2(KEY_ENTRY_0f, kdata[2]);
            M_UPR_KEY_VIEW_FIELD_SET2(KEY_ENTRY_1f, kdata[3]);
            M_UPR_KEY_VIEW_FIELD_SET2(MASK_ENTRY_0f, kmask[2]);
            M_UPR_KEY_VIEW_FIELD_SET2(MASK_ENTRY_1f, kmask[3]);
            M_UPR_KEY_VIEW_FIELD_SET(VALID_0f, arg->valid);
            M_UPR_KEY_VIEW_FIELD_SET(VALID_1f, arg->valid);
        } else {
            KEY_VIEW_FIELD_SET2(KEY_ENTRY_0f, kdata[0]);
            KEY_VIEW_FIELD_SET2(KEY_ENTRY_1f, kdata[1]);
            KEY_VIEW_FIELD_SET2(MASK_ENTRY_0f, kmask[0]);
            KEY_VIEW_FIELD_SET2(MASK_ENTRY_1f, kmask[1]);
            KEY_VIEW_FIELD_SET(VALID_0f, arg->valid);
            KEY_VIEW_FIELD_SET(VALID_1f, arg->valid);

            UPR_KEY_VIEW_FIELD_SET2(KEY_ENTRY_0f, kdata[2]);
            UPR_KEY_VIEW_FIELD_SET2(KEY_ENTRY_1f, kdata[3]);
            UPR_KEY_VIEW_FIELD_SET2(MASK_ENTRY_0f, kmask[2]);
            UPR_KEY_VIEW_FIELD_SET2(MASK_ENTRY_1f, kmask[3]);
            UPR_KEY_VIEW_FIELD_SET(VALID_0f, arg->valid);
            UPR_KEY_VIEW_FIELD_SET(VALID_1f, arg->valid);
        }
    } else {
        /* Apply subnet kmask. */
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[0] == 0);
        arg->key.ip_addr[1] &= arg->key.ip_mask[1];
        arg->key.ip_addr[2] &= arg->key.ip_mask[2];
        arg->key.ip_addr[3] &= arg->key.ip_mask[3];

        if (large_vrf) {
            bcm56780_l1_key_lpm_v6_single_encode_no_vrf(u, m, SINGLE_WIDE,
                                                        bcm56780_lpm_v6_key_type[u][m],
                                                        &arg->key.ip_addr[0],
                                                        &kdata[0],
                                                        &kdata[1]);
            bcm56780_l1_key_lpm_v6_single_encode_no_vrf(u, m, 0x3, 0x3,
                                                        &arg->key.ip_mask[0],
                                                        &kmask[0],
                                                        &kmask[1]);
        } else {
            bcm56780_l1_key_lpm_v6_single_encode(u, m, vrf, SINGLE_WIDE,
                                                 bcm56780_lpm_v6_key_type[u][m],
                                                 &arg->key.ip_addr[0],
                                                 &kdata[0],
                                                 &kdata[1]);
            bcm56780_l1_key_lpm_v6_single_encode(u, m, vrf_mask, 0x3, 0x3,
                                                 &arg->key.ip_mask[0],
                                                 &kmask[0],
                                                 &kmask[1]);
        }

        if (m) {
            M_KEY_VIEW_FIELD_SET2(KEY_ENTRY_0f, kdata[0]);
            M_KEY_VIEW_FIELD_SET2(KEY_ENTRY_1f, kdata[1]);
            M_KEY_VIEW_FIELD_SET2(MASK_ENTRY_0f, kmask[0]);
            M_KEY_VIEW_FIELD_SET2(MASK_ENTRY_1f, kmask[1]);
            M_KEY_VIEW_FIELD_SET(VALID_0f, arg->valid);
            M_KEY_VIEW_FIELD_SET(VALID_1f, arg->valid);
        } else {
            KEY_VIEW_FIELD_SET2(KEY_ENTRY_0f, kdata[0]);
            KEY_VIEW_FIELD_SET2(KEY_ENTRY_1f, kdata[1]);
            KEY_VIEW_FIELD_SET2(MASK_ENTRY_0f, kmask[0]);
            KEY_VIEW_FIELD_SET2(MASK_ENTRY_1f, kmask[1]);
            KEY_VIEW_FIELD_SET(VALID_0f, arg->valid);
            KEY_VIEW_FIELD_SET(VALID_1f, arg->valid);
        }
    }

}


/*!
 * \brief Encode key part into single-wide FP_COMP IPv4 entry.
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] de FP_COMP IPv4 entry
 *
 * \return SHR_E_XXX.
 */
static void
bcm56780_l1_key_view_comp_encode(int u, int m, alpm_arg_t *arg, void *e,
                                 void *e2)
{
    L3_DEFIP_TCAM_KEY_t kdata[4] = {{{0}}};
    L3_DEFIP_TCAM_KEY_t kmask[4] = {{{0}}};
    int port, port_mask;
    uint8_t large_port;
    TCAM_VIEW_T *de = e, *de_upr = e2;
    M_TCAM_VIEW_T *mde = e, *mde_upr = e2;

    if (arg->key.vt == VRF_OVERRIDE) {
        port = 0;
        port_mask = 0;
    } else if (arg->key.vt == VRF_GLOBAL) {
        port = 0;
        port_mask = 0;
    } else {
        port = arg->key.t.w_vrf;
        port_mask = bcm56780_alpm_max_comp_port[u];
    }

    large_port = arg->comp_large_port && arg->key.vt != VRF_OVERRIDE;
    if (arg->key.kt == KEY_COMP0_V4) {
        /* Apply subnet kmask. */
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[0] == 0);
        arg->key.ip_addr[1] &= arg->key.ip_mask[1];
        arg->key.ip_addr[2] &= arg->key.ip_mask[2];
        arg->key.ip_addr[3] &= arg->key.ip_mask[3];

        if (large_port) {
            bcm56780_l1_key_comp_v4_single_encode_no_port(u, m, SINGLE_WIDE,
                                                          bcm56780_comp_v4_key_type[u][m],
                                                          &arg->key.ip_addr[0],
                                                          &kdata[0],
                                                          &kdata[1]);
            bcm56780_l1_key_comp_v4_single_encode_no_port(u, m, 0x3, 0x3,
                                                          &arg->key.ip_mask[0],
                                                          &kmask[0],
                                                          &kmask[1]);
        } else {
            bcm56780_l1_key_comp_v4_single_encode(u, m, port, SINGLE_WIDE,
                                                  bcm56780_comp_v4_key_type[u][m],
                                                  &arg->key.ip_addr[0],
                                                  &kdata[0],
                                                  &kdata[1]);
            bcm56780_l1_key_comp_v4_single_encode(u, m, port_mask, 0x3, 0x3,
                                                  &arg->key.ip_mask[0],
                                                  &kmask[0],
                                                  &kmask[1]);
        }

        if (m) {
            M_KEY_VIEW_FIELD_SET2(KEY_ENTRY_0f, kdata[0]);
            M_KEY_VIEW_FIELD_SET2(KEY_ENTRY_1f, kdata[1]);
            M_KEY_VIEW_FIELD_SET2(MASK_ENTRY_0f, kmask[0]);
            M_KEY_VIEW_FIELD_SET2(MASK_ENTRY_1f, kmask[1]);
            M_KEY_VIEW_FIELD_SET(VALID_0f, arg->valid);
            M_KEY_VIEW_FIELD_SET(VALID_1f, arg->valid);
        } else {
            KEY_VIEW_FIELD_SET2(KEY_ENTRY_0f, kdata[0]);
            KEY_VIEW_FIELD_SET2(KEY_ENTRY_1f, kdata[1]);
            KEY_VIEW_FIELD_SET2(MASK_ENTRY_0f, kmask[0]);
            KEY_VIEW_FIELD_SET2(MASK_ENTRY_1f, kmask[1]);
            KEY_VIEW_FIELD_SET(VALID_0f, arg->valid);
            KEY_VIEW_FIELD_SET(VALID_1f, arg->valid);
        }
    } else {
        /* Apply subnet kmask. */
        arg->key.ip_addr[0] &= arg->key.ip_mask[0];
        arg->key.ip_addr[1] &= arg->key.ip_mask[1];
        arg->key.ip_addr[2] &= arg->key.ip_mask[2];
        arg->key.ip_addr[3] &= arg->key.ip_mask[3];

        large_port = arg->comp_large_port && arg->key.vt != VRF_OVERRIDE;
        if (large_port) {
            bcm56780_l1_key_comp_v6_double_encode_no_port(u, m, DOUBLE_WIDE,
                                                          bcm56780_comp_v6_key_type[u][m],
                                                          &arg->key.ip_addr[0],
                                                          &kdata[0],
                                                          &kdata[1],
                                                          &kdata[2],
                                                          &kdata[3]);
            bcm56780_l1_key_comp_v6_double_encode_no_port(u, m, 0x3, 0x3,
                                                          &arg->key.ip_mask[0],
                                                          &kmask[0],
                                                          &kmask[1],
                                                          &kmask[2],
                                                          &kmask[3]);
        } else {
            bcm56780_l1_key_comp_v6_double_encode(u, m, port, DOUBLE_WIDE,
                                                  bcm56780_comp_v6_key_type[u][m],
                                                  &arg->key.ip_addr[0],
                                                  &kdata[0],
                                                  &kdata[1],
                                                  &kdata[2],
                                                  &kdata[3]);
            bcm56780_l1_key_comp_v6_double_encode(u, m, port_mask, 0x3, 0x3,
                                                  &arg->key.ip_mask[0],
                                                  &kmask[0],
                                                  &kmask[1],
                                                  &kmask[2],
                                                  &kmask[3]);
        }

        if (m) {
            M_KEY_VIEW_FIELD_SET2(KEY_ENTRY_0f, kdata[0]);
            M_KEY_VIEW_FIELD_SET2(KEY_ENTRY_1f, kdata[1]);
            M_KEY_VIEW_FIELD_SET2(MASK_ENTRY_0f, kmask[0]);
            M_KEY_VIEW_FIELD_SET2(MASK_ENTRY_1f, kmask[1]);
            M_KEY_VIEW_FIELD_SET(VALID_0f, arg->valid);
            M_KEY_VIEW_FIELD_SET(VALID_1f, arg->valid);

            M_UPR_KEY_VIEW_FIELD_SET2(KEY_ENTRY_0f, kdata[2]);
            M_UPR_KEY_VIEW_FIELD_SET2(KEY_ENTRY_1f, kdata[3]);
            M_UPR_KEY_VIEW_FIELD_SET2(MASK_ENTRY_0f, kmask[2]);
            M_UPR_KEY_VIEW_FIELD_SET2(MASK_ENTRY_1f, kmask[3]);
            M_UPR_KEY_VIEW_FIELD_SET(VALID_0f, arg->valid);
            M_UPR_KEY_VIEW_FIELD_SET(VALID_1f, arg->valid);
        } else {
            KEY_VIEW_FIELD_SET2(KEY_ENTRY_0f, kdata[0]);
            KEY_VIEW_FIELD_SET2(KEY_ENTRY_1f, kdata[1]);
            KEY_VIEW_FIELD_SET2(MASK_ENTRY_0f, kmask[0]);
            KEY_VIEW_FIELD_SET2(MASK_ENTRY_1f, kmask[1]);
            KEY_VIEW_FIELD_SET(VALID_0f, arg->valid);
            KEY_VIEW_FIELD_SET(VALID_1f, arg->valid);

            UPR_KEY_VIEW_FIELD_SET2(KEY_ENTRY_0f, kdata[2]);
            UPR_KEY_VIEW_FIELD_SET2(KEY_ENTRY_1f, kdata[3]);
            UPR_KEY_VIEW_FIELD_SET2(MASK_ENTRY_0f, kmask[2]);
            UPR_KEY_VIEW_FIELD_SET2(MASK_ENTRY_1f, kmask[3]);
            UPR_KEY_VIEW_FIELD_SET(VALID_0f, arg->valid);
            UPR_KEY_VIEW_FIELD_SET(VALID_1f, arg->valid);
        }
    }
}




/*!
 * \brief Encode key part into single-wide entry.
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] de DIP entry
 * \param [out] se SIP entry
 *
 * \return SHR_E_XXX.
 */
static void
bcm56780_l1_data_view_lpm_encode(int u, int m, alpm_arg_t *arg, void *e,
                                 void *e2, int upkm)
{
    uint8_t sub_db_prio;
    bcm56780_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    FIXED_DATA_t fd_format;
    DATA_VIEW_T *de = e;
    M_DATA_VIEW_T *mde = e;

    if (arg->key.vt == VRF_OVERRIDE) {
        sub_db_prio = SUB_DB_PRIORITY_OVERRIDE;
    } else if (arg->key.vt == VRF_GLOBAL) {
        sub_db_prio = SUB_DB_PRIORITY_GLOBAL;
    } else {
        sub_db_prio = SUB_DB_PRIORITY_VRF;
    }

    bcm56780_ln_alpm1_data_format_encode(u, m, arg, &a1_format, &arg->a1);
    bcm56780_assoc_data_format_encode(u, m, &ad_format, arg->key.t.app,
                                      bcm56780_max_dt[u],
                                      &arg->ad);
    bcm56780_fixed_data_format_encode(u, m, &fd_format, sub_db_prio, arg);
    if (m) {
        M_DATA_VIEW_FIELD_SET2(DATA_ALPM1_DATA0f, a1_format);
        M_DATA_VIEW_FIELD_SET2(DATA_ASSOC_DATA0f, ad_format);
        M_DATA_VIEW_FIELD_SET(DATA_FIXED_DATA0f, fd_format.v[0]);
    } else {
        DATA_VIEW_FIELD_SET2(DATA_ALPM1_DATA0f, a1_format);
        DATA_VIEW_FIELD_SET2(DATA_ASSOC_DATA0f, ad_format);
        DATA_VIEW_FIELD_SET(DATA_FIXED_DATA0f, fd_format.v[0]);
    }
}


/*!
 * \brief Encode key part into single-wide entry.
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] de DIP entry
 * \param [out] se SIP entry
 *
 * \return SHR_E_XXX.
 */
static void
bcm56780_l1_narrow_encode_data_n1(int u, int m, alpm_arg_t *arg, void *e,
                                  void *e2)
{
    bcm56780_assoc_data_t ad_format;
    UNPAIR_T *de = e;
    M_UNPAIR_T *mde = e;

    bcm56780_assoc_data_format_encode(u, m, &ad_format, arg->key.t.app,
                                      bcm56780_max_dt[u],
                                      &arg->ad);
    if (m) {
        M_UNPAIR_FIELD_SET2(DATA1_ASSOC_DATA0f, ad_format);
    } else {
        UNPAIR_FIELD_SET2(DATA1_ASSOC_DATA0f, ad_format);
    }
}

static void
bcm56780_l1_narrow_encode_data(int u, int m, alpm_arg_t *arg, void *e,
                               void *e2)
{
    bcm56780_assoc_data_t ad_format;
    UNPAIR_T *de = e;
    M_UNPAIR_T *mde = e;

    bcm56780_assoc_data_format_encode(u, m, &ad_format, arg->key.t.app,
                                      bcm56780_max_dt[u],
                                      &arg->ad);
    if (m) {
        M_UNPAIR_FIELD_SET2(DATA0_ASSOC_DATA0f, ad_format);
    } else {
        UNPAIR_FIELD_SET2(DATA0_ASSOC_DATA0f, ad_format);
    }
}
static void
bcm56780_l1_wide_encode_data(int u, int m, alpm_arg_t *arg, void *e, void *e2)
{
    bcm56780_assoc_data_t ad_format;
    PAIR_T *de = e;
    M_PAIR_T *mde = e;

    bcm56780_assoc_data_format_encode(u, m, &ad_format, arg->key.t.app,
                                      bcm56780_max_dt[u],
                                      &arg->ad);
    if (m) {
        M_PAIR_FIELD_SET2(DATA_ASSOC_DATA0f, ad_format);
    } else {
        PAIR_FIELD_SET2(DATA_ASSOC_DATA0f, ad_format);
    }
}

static void
bcm56780_l1_narrow_lpm_encode_n1(int u, int m, alpm_arg_t *arg, void *e,
                                 void *e2, int upkm)
{
    L3_DEFIP_TCAM_KEY_t kdata;
    L3_DEFIP_TCAM_KEY_t kmask;
    int vrf, vrf_mask;
    uint8_t sub_db_prio;
    bcm56780_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    FIXED_DATA_t fd_format;
    uint8_t large_vrf;
    UNPAIR_T *de = e;
    M_UNPAIR_T *mde = e;

    L3_DEFIP_TCAM_KEY_CLR(kdata);
    L3_DEFIP_TCAM_KEY_CLR(kmask);

    if (arg->key.vt == VRF_OVERRIDE) {
        sub_db_prio = SUB_DB_PRIORITY_OVERRIDE;
        vrf = 0;
        vrf_mask = 0;
    } else if (arg->key.vt == VRF_GLOBAL) {
        sub_db_prio = SUB_DB_PRIORITY_GLOBAL;
        vrf = 0;
        vrf_mask = 0;
    } else {
        sub_db_prio = SUB_DB_PRIORITY_VRF;
        vrf = arg->key.t.w_vrf;
        vrf_mask = bcm56780_alpm_max_vrf[u];
    }

    large_vrf = arg->lpm_large_vrf && arg->key.vt != VRF_OVERRIDE;
    if (arg->key.kt == KEY_IPV4) {
        arg->key.ip_addr[0] &= arg->key.ip_mask[0];

        if (large_vrf) {
            bcm56780_l1_key_lpm_v4_encode_no_vrf(u, m, HALF_WIDE,
                                                 bcm56780_lpm_v4_key_type[u][m],
                                                 &arg->key.ip_addr[0], &kdata);
            bcm56780_l1_key_lpm_v4_encode_no_vrf(u, m, 0x3, 0x3,
                                                 &arg->key.ip_mask[0], &kmask);
        } else {
            bcm56780_l1_key_lpm_v4_encode(u, m, vrf, HALF_WIDE,
                                          bcm56780_lpm_v4_key_type[u][m],
                                          arg->key.ip_addr[0], &kdata);
            bcm56780_l1_key_lpm_v4_encode(u, m, vrf_mask, 0x3, 0x3,
                                          arg->key.ip_mask[0], &kmask);
        }
        if (m) {
            M_UNPAIR_FIELD_SET2(KEY_ENTRY_1f, kdata);
            M_UNPAIR_FIELD_SET2(MASK_ENTRY_1f, kmask);
            M_UNPAIR_FIELD_SET(VALID_1f, arg->valid);
        } else {
            UNPAIR_FIELD_SET2(KEY_ENTRY_1f, kdata);
            UNPAIR_FIELD_SET2(MASK_ENTRY_1f, kmask);
            UNPAIR_FIELD_SET(VALID_1f, arg->valid);
        }
    } else if (arg->key.kt == KEY_IPV6_SINGLE &&
               (upkm == PKM_S || upkm == PKM_SS)) {
        /* Apply subnet kmask. */
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[0] == 0);
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[1] == 0);
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[2] == 0);
        arg->key.ip_addr[3] &= arg->key.ip_mask[3];

        if (large_vrf) {
            bcm56780_l1_key_lpm_v6_half_encode_no_vrf(u, m, HALF_WIDE,
                                                      bcm56780_lpm_v6_key_type[u][m],
                                                      &arg->key.ip_addr[0],
                                                      &kdata);
            bcm56780_l1_key_lpm_v6_half_encode_no_vrf(u, m, 0x3, 0x3,
                                                      &arg->key.ip_mask[0],
                                                      &kmask);
        } else {
            bcm56780_l1_key_lpm_v6_half_encode(u, m, vrf, HALF_WIDE,
                                               bcm56780_lpm_v6_key_type[u][m],
                                               &arg->key.ip_addr[0], &kdata);
            bcm56780_l1_key_lpm_v6_half_encode(u, m, vrf_mask, 0x3, 0x3,
                                               &arg->key.ip_mask[0], &kmask);
        }
        if (m) {
            M_UNPAIR_FIELD_SET2(KEY_ENTRY_1f, kdata);
            M_UNPAIR_FIELD_SET2(MASK_ENTRY_1f, kmask);
            M_UNPAIR_FIELD_SET(VALID_1f, arg->valid);
        } else {
            UNPAIR_FIELD_SET2(KEY_ENTRY_1f, kdata);
            UNPAIR_FIELD_SET2(MASK_ENTRY_1f, kmask);
            UNPAIR_FIELD_SET(VALID_1f, arg->valid);
        }
    }

    bcm56780_ln_alpm1_data_format_encode(u, m, arg, &a1_format, &arg->a1);
    bcm56780_assoc_data_format_encode(u, m, &ad_format, arg->key.t.app,
                                      bcm56780_max_dt[u],
                                      &arg->ad);
    bcm56780_fixed_data_format_encode(u, m, &fd_format, sub_db_prio, arg);
    if (m) {
        M_UNPAIR_FIELD_SET2(DATA1_ALPM1_DATA0f, a1_format);
        M_UNPAIR_FIELD_SET2(DATA1_ASSOC_DATA0f, ad_format);
        M_UNPAIR_FIELD_SET(DATA1_FIXED_DATA0f, fd_format.v[0]);
    } else {
        UNPAIR_FIELD_SET2(DATA1_ALPM1_DATA0f, a1_format);
        UNPAIR_FIELD_SET2(DATA1_ASSOC_DATA0f, ad_format);
        UNPAIR_FIELD_SET(DATA1_FIXED_DATA0f, fd_format.v[0]);
    }
}

/*!
 * \brief Encode key part into single-wide entry.
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] de DIP entry
 * \param [out] se SIP entry
 *
 * \return SHR_E_XXX.
 */
static void
bcm56780_l1_narrow_lpm_encode(int u, int m, alpm_arg_t *arg, void *e,
                              void *e2, int upkm)
{
    L3_DEFIP_TCAM_KEY_t kdata[2] = {{{0}}};
    L3_DEFIP_TCAM_KEY_t kmask[2] = {{{0}}};
    int vrf, vrf_mask;
    uint8_t sub_db_prio;
    bcm56780_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    FIXED_DATA_t fd_format;
    uint8_t large_vrf;
    UNPAIR_T *de = e;
    M_UNPAIR_T *mde = e;

    if (arg->key.vt == VRF_OVERRIDE) {
        sub_db_prio = SUB_DB_PRIORITY_OVERRIDE;
        vrf = 0;
        vrf_mask = 0;
    } else if (arg->key.vt == VRF_GLOBAL) {
        sub_db_prio = SUB_DB_PRIORITY_GLOBAL;
        vrf = 0;
        vrf_mask = 0;
    } else {
        sub_db_prio = SUB_DB_PRIORITY_VRF;
        vrf = arg->key.t.w_vrf;
        vrf_mask = bcm56780_alpm_max_vrf[u];
    }

    large_vrf = arg->lpm_large_vrf && arg->key.vt != VRF_OVERRIDE;
    if (arg->key.kt == KEY_IPV4) {
        arg->key.ip_addr[0] &= arg->key.ip_mask[0];

        if (large_vrf) {
            bcm56780_l1_key_lpm_v4_encode_no_vrf(u, m, HALF_WIDE,
                                                 bcm56780_lpm_v4_key_type[u][m],
                                                 &arg->key.ip_addr[0],
                                                 &kdata[0]);
            bcm56780_l1_key_lpm_v4_encode_no_vrf(u, m, 0x3, 0x3,
                                                 &arg->key.ip_mask[0],
                                                 &kmask[0]);
        } else {
            bcm56780_l1_key_lpm_v4_encode(u, m, vrf, HALF_WIDE,
                                          bcm56780_lpm_v4_key_type[u][m],
                                          arg->key.ip_addr[0], &kdata[0]);
            bcm56780_l1_key_lpm_v4_encode(u, m, vrf_mask, 0x3, 0x3,
                                          arg->key.ip_mask[0], &kmask[0]);
        }
        if (m) {
            M_UNPAIR_FIELD_SET2(KEY_ENTRY_0f, kdata[0]);
            M_UNPAIR_FIELD_SET2(MASK_ENTRY_0f, kmask[0]);
            M_UNPAIR_FIELD_SET(VALID_0f, arg->valid);
        } else {
            UNPAIR_FIELD_SET2(KEY_ENTRY_0f, kdata[0]);
            UNPAIR_FIELD_SET2(MASK_ENTRY_0f, kmask[0]);
            UNPAIR_FIELD_SET(VALID_0f, arg->valid);
        }
    } else if (arg->key.kt == KEY_IPV6_SINGLE &&
               (upkm == PKM_S || upkm == PKM_SS)) {
        /* Apply subnet kmask. */
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[0] == 0);
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[1] == 0);
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[2] == 0);
        arg->key.ip_addr[3] &= arg->key.ip_mask[3];

        if (large_vrf) {
            bcm56780_l1_key_lpm_v6_half_encode_no_vrf(u, m, HALF_WIDE,
                                                      bcm56780_lpm_v6_key_type[u][m],
                                                      &arg->key.ip_addr[0],
                                                      &kdata[0]);
            bcm56780_l1_key_lpm_v6_half_encode_no_vrf(u, m, 0x3, 0x3,
                                                      &arg->key.ip_mask[0],
                                                      &kmask[0]);
        } else {
            bcm56780_l1_key_lpm_v6_half_encode(u, m, vrf, HALF_WIDE,
                                               bcm56780_lpm_v6_key_type[u][m],
                                               &arg->key.ip_addr[0], &kdata[0]);
            bcm56780_l1_key_lpm_v6_half_encode(u, m, vrf_mask, 0x3, 0x3,
                                               &arg->key.ip_mask[0], &kmask[0]);
        }
        if (m) {
            M_UNPAIR_FIELD_SET2(KEY_ENTRY_0f, kdata[0]);
            M_UNPAIR_FIELD_SET2(MASK_ENTRY_0f, kmask[0]);
            M_UNPAIR_FIELD_SET(VALID_0f, arg->valid);
        } else {
            UNPAIR_FIELD_SET2(KEY_ENTRY_0f, kdata[0]);
            UNPAIR_FIELD_SET2(MASK_ENTRY_0f, kmask[0]);
            UNPAIR_FIELD_SET(VALID_0f, arg->valid);
        }
    } else {
        /* Apply subnet kmask. */
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[0] == 0);
        arg->key.ip_addr[1] &= arg->key.ip_mask[1];
        arg->key.ip_addr[2] &= arg->key.ip_mask[2];
        arg->key.ip_addr[3] &= arg->key.ip_mask[3];

        if (large_vrf) {
            bcm56780_l1_key_lpm_v6_single_encode_no_vrf(u, m, SINGLE_WIDE,
                                                        bcm56780_lpm_v6_key_type[u][m],
                                                        &arg->key.ip_addr[0],
                                                        &kdata[0],
                                                        &kdata[1]);
            bcm56780_l1_key_lpm_v6_single_encode_no_vrf(u, m, 0x3, 0x3,
                                                        &arg->key.ip_mask[0],
                                                        &kmask[0],
                                                        &kmask[1]);
        } else {
            bcm56780_l1_key_lpm_v6_single_encode(u, m, vrf, SINGLE_WIDE,
                                                 bcm56780_lpm_v6_key_type[u][m],
                                                 &arg->key.ip_addr[0],
                                                 &kdata[0],
                                                 &kdata[1]);
            bcm56780_l1_key_lpm_v6_single_encode(u, m, vrf_mask, 0x3, 0x3,
                                                 &arg->key.ip_mask[0],
                                                 &kmask[0],
                                                 &kmask[1]);
        }

        if (m) {
            M_UNPAIR_FIELD_SET2(KEY_ENTRY_0f, kdata[0]);
            M_UNPAIR_FIELD_SET2(KEY_ENTRY_1f, kdata[1]);
            M_UNPAIR_FIELD_SET2(MASK_ENTRY_0f, kmask[0]);
            M_UNPAIR_FIELD_SET2(MASK_ENTRY_1f, kmask[1]);
            M_UNPAIR_FIELD_SET(VALID_0f, arg->valid);
            M_UNPAIR_FIELD_SET(VALID_1f, arg->valid);
        } else {
            UNPAIR_FIELD_SET2(KEY_ENTRY_0f, kdata[0]);
            UNPAIR_FIELD_SET2(KEY_ENTRY_1f, kdata[1]);
            UNPAIR_FIELD_SET2(MASK_ENTRY_0f, kmask[0]);
            UNPAIR_FIELD_SET2(MASK_ENTRY_1f, kmask[1]);
            UNPAIR_FIELD_SET(VALID_0f, arg->valid);
            UNPAIR_FIELD_SET(VALID_1f, arg->valid);
        }
    }

    bcm56780_ln_alpm1_data_format_encode(u, m, arg, &a1_format, &arg->a1);
    bcm56780_assoc_data_format_encode(u, m, &ad_format, arg->key.t.app,
                                      bcm56780_max_dt[u],
                                      &arg->ad);
    bcm56780_fixed_data_format_encode(u, m, &fd_format, sub_db_prio, arg);
    if (m) {
        M_UNPAIR_FIELD_SET2(DATA0_ALPM1_DATA0f, a1_format);
        M_UNPAIR_FIELD_SET2(DATA0_ASSOC_DATA0f, ad_format);
        M_UNPAIR_FIELD_SET(DATA0_FIXED_DATA0f, fd_format.v[0]);
    } else {
        UNPAIR_FIELD_SET2(DATA0_ALPM1_DATA0f, a1_format);
        UNPAIR_FIELD_SET2(DATA0_ASSOC_DATA0f, ad_format);
        UNPAIR_FIELD_SET(DATA0_FIXED_DATA0f, fd_format.v[0]);
    }
}


/*!
 * \brief Encode key part into single-wide FP_COMP IPv4 entry.
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] de FP_COMP IPv4 entry
 *
 * \return SHR_E_XXX.
 */
static void
bcm56780_l1_narrow_comp_encode(int u, int m, alpm_arg_t *arg, void *e)
{
    L3_DEFIP_TCAM_KEY_t kdata[2] = {{{0}}};
    L3_DEFIP_TCAM_KEY_t kmask[2] = {{{0}}};
    int port, port_mask;
    uint8_t sub_db_prio;
    bcm56780_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    FIXED_DATA_t fd_format;
    uint8_t large_port;
    UNPAIR_T *de = e;
    M_UNPAIR_T *mde = e;

    if (arg->key.vt == VRF_OVERRIDE) {
        sub_db_prio = SUB_DB_PRIORITY_OVERRIDE;
        port = 0;
        port_mask = 0;
    } else if (arg->key.vt == VRF_GLOBAL) {
        sub_db_prio = SUB_DB_PRIORITY_GLOBAL;
        port = 0;
        port_mask = 0;
    } else {
        sub_db_prio = SUB_DB_PRIORITY_VRF;
        port = arg->key.t.w_vrf;
        port_mask = bcm56780_alpm_max_comp_port[u];
    }

    large_port = arg->comp_large_port && arg->key.vt != VRF_OVERRIDE;

    /* Apply subnet kmask. */
    ALPM_ASSERT_ERRMSG(arg->key.ip_mask[0] == 0);
    arg->key.ip_addr[1] &= arg->key.ip_mask[1];
    arg->key.ip_addr[2] &= arg->key.ip_mask[2];
    arg->key.ip_addr[3] &= arg->key.ip_mask[3];

    if (large_port) {
        bcm56780_l1_key_comp_v4_single_encode_no_port(u, m, SINGLE_WIDE,
                                                      bcm56780_comp_v4_key_type[u][m],
                                                      &arg->key.ip_addr[0],
                                                      &kdata[0],
                                                      &kdata[1]);
        bcm56780_l1_key_comp_v4_single_encode_no_port(u, m, 0x3, 0x3,
                                                      &arg->key.ip_mask[0],
                                                      &kmask[0],
                                                      &kmask[1]);
    } else {
        bcm56780_l1_key_comp_v4_single_encode(u, m, port, SINGLE_WIDE,
                                              bcm56780_comp_v4_key_type[u][m],
                                              &arg->key.ip_addr[0],
                                              &kdata[0],
                                              &kdata[1]);
        bcm56780_l1_key_comp_v4_single_encode(u, m, port_mask, 0x3, 0x3,
                                              &arg->key.ip_mask[0],
                                              &kmask[0],
                                              &kmask[1]);
    }

    if (m) {
        M_UNPAIR_FIELD_SET2(KEY_ENTRY_0f, kdata[0]);
        M_UNPAIR_FIELD_SET2(KEY_ENTRY_1f, kdata[1]);
        M_UNPAIR_FIELD_SET2(MASK_ENTRY_0f, kmask[0]);
        M_UNPAIR_FIELD_SET2(MASK_ENTRY_1f, kmask[1]);
        M_UNPAIR_FIELD_SET(VALID_0f, arg->valid);
        M_UNPAIR_FIELD_SET(VALID_1f, arg->valid);
    } else {
        UNPAIR_FIELD_SET2(KEY_ENTRY_0f, kdata[0]);
        UNPAIR_FIELD_SET2(KEY_ENTRY_1f, kdata[1]);
        UNPAIR_FIELD_SET2(MASK_ENTRY_0f, kmask[0]);
        UNPAIR_FIELD_SET2(MASK_ENTRY_1f, kmask[1]);
        UNPAIR_FIELD_SET(VALID_0f, arg->valid);
        UNPAIR_FIELD_SET(VALID_1f, arg->valid);
    }

    bcm56780_ln_alpm1_data_format_encode(u, m, arg, &a1_format, &arg->a1);
    bcm56780_assoc_data_format_encode(u, m, &ad_format, arg->key.t.app,
                                      bcm56780_max_dt[u],
                                      &arg->ad);
    bcm56780_fixed_data_format_encode(u, m, &fd_format, sub_db_prio, arg);
    if (m) {
        M_UNPAIR_FIELD_SET2(DATA0_ALPM1_DATA0f, a1_format);
        M_UNPAIR_FIELD_SET2(DATA0_ASSOC_DATA0f, ad_format);
        M_UNPAIR_FIELD_SET(DATA0_FIXED_DATA0f, fd_format.v[0]);
    } else {
        UNPAIR_FIELD_SET2(DATA0_ALPM1_DATA0f, a1_format);
        UNPAIR_FIELD_SET2(DATA0_ASSOC_DATA0f, ad_format);
        UNPAIR_FIELD_SET(DATA0_FIXED_DATA0f, fd_format.v[0]);
    }
}

/*!
 * \brief Encode key part into double-wide entry.
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] de DIP entry
 * \param [out] se SIP entry
 *
 * \return SHR_E_XXX.
 */
static void
bcm56780_l1_wide_lpm_encode(int u, int m, alpm_arg_t *arg, void *e)
{
    L3_DEFIP_TCAM_KEY_t kdata[4] = {{{0}}};
    L3_DEFIP_TCAM_KEY_t kmask[4] = {{{0}}};
    int vrf, vrf_mask;
    uint8_t sub_db_prio;
    bcm56780_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    FIXED_DATA_t fd_format;
    uint8_t large_vrf;
    PAIR_T *de = e;
    M_PAIR_T *mde = e;

    if (arg->key.vt == VRF_OVERRIDE) {
        sub_db_prio = SUB_DB_PRIORITY_OVERRIDE;
        vrf = 0;
        vrf_mask = 0;
    } else if (arg->key.vt == VRF_GLOBAL) {
        sub_db_prio = SUB_DB_PRIORITY_GLOBAL;
        vrf = 0;
        vrf_mask = 0;
    } else {
        sub_db_prio = SUB_DB_PRIORITY_VRF;
        vrf = arg->key.t.w_vrf;
        vrf_mask = bcm56780_alpm_max_vrf[u];
    }

    /* Apply subnet kmask. */
    arg->key.ip_addr[0] &= arg->key.ip_mask[0];
    arg->key.ip_addr[1] &= arg->key.ip_mask[1];
    arg->key.ip_addr[2] &= arg->key.ip_mask[2];
    arg->key.ip_addr[3] &= arg->key.ip_mask[3];

    large_vrf = arg->lpm_large_vrf && arg->key.vt != VRF_OVERRIDE;
    if (large_vrf) {
        bcm56780_l1_key_lpm_v6_double_encode_no_vrf(u, m, DOUBLE_WIDE,
                                                    bcm56780_lpm_v6_key_type[u][m],
                                                    &arg->key.ip_addr[0],
                                                    &kdata[0],
                                                    &kdata[1],
                                                    &kdata[2],
                                                    &kdata[3]);
        bcm56780_l1_key_lpm_v6_double_encode_no_vrf(u, m, 0x3, 0x3,
                                                    &arg->key.ip_mask[0],
                                                    &kmask[0],
                                                    &kmask[1],
                                                    &kmask[2],
                                                    &kmask[3]);
    } else {
        bcm56780_l1_key_lpm_v6_double_encode(u, m, vrf, DOUBLE_WIDE,
                                             bcm56780_lpm_v6_key_type[u][m],
                                             &arg->key.ip_addr[0],
                                             &kdata[0],
                                             &kdata[1],
                                             &kdata[2],
                                             &kdata[3]);
        bcm56780_l1_key_lpm_v6_double_encode(u, m, vrf_mask, 0x3, 0x3,
                                             &arg->key.ip_mask[0],
                                             &kmask[0],
                                             &kmask[1],
                                             &kmask[2],
                                             &kmask[3]);
    }

    if (m) {
        M_PAIR_FIELD_SET2(KEY0_ENTRY_0f, kdata[0]);
        M_PAIR_FIELD_SET2(KEY0_ENTRY_1f, kdata[1]);
        M_PAIR_FIELD_SET2(KEY1_ENTRY_0f, kdata[2]);
        M_PAIR_FIELD_SET2(KEY1_ENTRY_1f, kdata[3]);
        M_PAIR_FIELD_SET2(MASK0_ENTRY_0f, kmask[0]);
        M_PAIR_FIELD_SET2(MASK0_ENTRY_1f, kmask[1]);
        M_PAIR_FIELD_SET2(MASK1_ENTRY_0f, kmask[2]);
        M_PAIR_FIELD_SET2(MASK1_ENTRY_1f, kmask[3]);
        M_PAIR_FIELD_SET(VALID0_0f, arg->valid);
        M_PAIR_FIELD_SET(VALID0_1f, arg->valid);
        M_PAIR_FIELD_SET(VALID1_0f, arg->valid);
        M_PAIR_FIELD_SET(VALID1_1f, arg->valid);
    } else {
        PAIR_FIELD_SET2(KEY0_ENTRY_0f, kdata[0]);
        PAIR_FIELD_SET2(KEY0_ENTRY_1f, kdata[1]);
        PAIR_FIELD_SET2(KEY1_ENTRY_0f, kdata[2]);
        PAIR_FIELD_SET2(KEY1_ENTRY_1f, kdata[3]);
        PAIR_FIELD_SET2(MASK0_ENTRY_0f, kmask[0]);
        PAIR_FIELD_SET2(MASK0_ENTRY_1f, kmask[1]);
        PAIR_FIELD_SET2(MASK1_ENTRY_0f, kmask[2]);
        PAIR_FIELD_SET2(MASK1_ENTRY_1f, kmask[3]);
        PAIR_FIELD_SET(VALID0_0f, arg->valid);
        PAIR_FIELD_SET(VALID0_1f, arg->valid);
        PAIR_FIELD_SET(VALID1_0f, arg->valid);
        PAIR_FIELD_SET(VALID1_1f, arg->valid);
    }
    bcm56780_ln_alpm1_data_format_encode(u, m, arg, &a1_format, &arg->a1);
    bcm56780_assoc_data_format_encode(u, m, &ad_format, arg->key.t.app,
                                      bcm56780_max_dt[u],
                                      &arg->ad);
    bcm56780_fixed_data_format_encode(u, m, &fd_format, sub_db_prio, arg);

    if (m) {
        M_PAIR_FIELD_SET2(DATA_ALPM1_DATA0f, a1_format);
        M_PAIR_FIELD_SET2(DATA_ASSOC_DATA0f, ad_format);
        M_PAIR_FIELD_SET(DATA_FIXED_DATA0f, fd_format.v[0]);
    } else {
        PAIR_FIELD_SET2(DATA_ALPM1_DATA0f, a1_format);
        PAIR_FIELD_SET2(DATA_ASSOC_DATA0f, ad_format);
        PAIR_FIELD_SET(DATA_FIXED_DATA0f, fd_format.v[0]);
    }
}

/*!
 * \brief Encode key part into double-wide FP_COMP IPv6 entry.
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] de FP_COMP IPv6 entry
 *
 * \return SHR_E_XXX.
 */
static void
bcm56780_l1_wide_comp_encode(int u, int m, alpm_arg_t *arg, void *e)
{
    L3_DEFIP_TCAM_KEY_t kdata[4] = {{{0}}};
    L3_DEFIP_TCAM_KEY_t kmask[4] = {{{0}}};
    int port, port_mask;
    uint8_t sub_db_prio;
    bcm56780_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    FIXED_DATA_t fd_format;
    uint8_t large_port;
    PAIR_T *de = e;
    M_PAIR_T *mde = e;

    if (arg->key.vt == VRF_OVERRIDE) {
        sub_db_prio = SUB_DB_PRIORITY_OVERRIDE;
        port = 0;
        port_mask = 0;
    } else if (arg->key.vt == VRF_GLOBAL) {
        sub_db_prio = SUB_DB_PRIORITY_GLOBAL;
        port = 0;
        port_mask = 0;
    } else {
        sub_db_prio = SUB_DB_PRIORITY_VRF;
        port = arg->key.t.w_vrf;
        port_mask = bcm56780_alpm_max_comp_port[u];
    }

    /* Apply subnet kmask. */
    arg->key.ip_addr[0] &= arg->key.ip_mask[0];
    arg->key.ip_addr[1] &= arg->key.ip_mask[1];
    arg->key.ip_addr[2] &= arg->key.ip_mask[2];
    arg->key.ip_addr[3] &= arg->key.ip_mask[3];

    large_port = arg->comp_large_port && arg->key.vt != VRF_OVERRIDE;
    if (large_port) {
        bcm56780_l1_key_comp_v6_double_encode_no_port(u, m, DOUBLE_WIDE,
                                                      bcm56780_comp_v6_key_type[u][m],
                                                      &arg->key.ip_addr[0],
                                                      &kdata[0],
                                                      &kdata[1],
                                                      &kdata[2],
                                                      &kdata[3]);
        bcm56780_l1_key_comp_v6_double_encode_no_port(u, m, 0x3, 0x3,
                                                      &arg->key.ip_mask[0],
                                                      &kmask[0],
                                                      &kmask[1],
                                                      &kmask[2],
                                                      &kmask[3]);
    } else {
        bcm56780_l1_key_comp_v6_double_encode(u, m, port, DOUBLE_WIDE,
                                              bcm56780_comp_v6_key_type[u][m],
                                              &arg->key.ip_addr[0],
                                              &kdata[0],
                                              &kdata[1],
                                              &kdata[2],
                                              &kdata[3]);
        bcm56780_l1_key_comp_v6_double_encode(u, m, port_mask, 0x3, 0x3,
                                              &arg->key.ip_mask[0],
                                              &kmask[0],
                                              &kmask[1],
                                              &kmask[2],
                                              &kmask[3]);
    }

    if (m) {
        M_PAIR_FIELD_SET2(KEY0_ENTRY_0f, kdata[0]);
        M_PAIR_FIELD_SET2(KEY0_ENTRY_1f, kdata[1]);
        M_PAIR_FIELD_SET2(KEY1_ENTRY_0f, kdata[2]);
        M_PAIR_FIELD_SET2(KEY1_ENTRY_1f, kdata[3]);
        M_PAIR_FIELD_SET2(MASK0_ENTRY_0f, kmask[0]);
        M_PAIR_FIELD_SET2(MASK0_ENTRY_1f, kmask[1]);
        M_PAIR_FIELD_SET2(MASK1_ENTRY_0f, kmask[2]);
        M_PAIR_FIELD_SET2(MASK1_ENTRY_1f, kmask[3]);
        M_PAIR_FIELD_SET(VALID0_0f, arg->valid);
        M_PAIR_FIELD_SET(VALID0_1f, arg->valid);
        M_PAIR_FIELD_SET(VALID1_0f, arg->valid);
        M_PAIR_FIELD_SET(VALID1_1f, arg->valid);
    } else {
        PAIR_FIELD_SET2(KEY0_ENTRY_0f, kdata[0]);
        PAIR_FIELD_SET2(KEY0_ENTRY_1f, kdata[1]);
        PAIR_FIELD_SET2(KEY1_ENTRY_0f, kdata[2]);
        PAIR_FIELD_SET2(KEY1_ENTRY_1f, kdata[3]);
        PAIR_FIELD_SET2(MASK0_ENTRY_0f, kmask[0]);
        PAIR_FIELD_SET2(MASK0_ENTRY_1f, kmask[1]);
        PAIR_FIELD_SET2(MASK1_ENTRY_0f, kmask[2]);
        PAIR_FIELD_SET2(MASK1_ENTRY_1f, kmask[3]);
        PAIR_FIELD_SET(VALID0_0f, arg->valid);
        PAIR_FIELD_SET(VALID0_1f, arg->valid);
        PAIR_FIELD_SET(VALID1_0f, arg->valid);
        PAIR_FIELD_SET(VALID1_1f, arg->valid);
    }

    bcm56780_ln_alpm1_data_format_encode(u, m, arg, &a1_format, &arg->a1);
    bcm56780_assoc_data_format_encode(u, m, &ad_format, arg->key.t.app,
                                      bcm56780_max_dt[u],
                                      &arg->ad);
    bcm56780_fixed_data_format_encode(u, m, &fd_format, sub_db_prio, arg);

    if (m) {
        M_PAIR_FIELD_SET2(DATA_ALPM1_DATA0f, a1_format);
        M_PAIR_FIELD_SET2(DATA_ASSOC_DATA0f, ad_format);
        M_PAIR_FIELD_SET(DATA_FIXED_DATA0f, fd_format.v[0]);
    } else {
        PAIR_FIELD_SET2(DATA_ALPM1_DATA0f, a1_format);
        PAIR_FIELD_SET2(DATA_ASSOC_DATA0f, ad_format);
        PAIR_FIELD_SET(DATA_FIXED_DATA0f, fd_format.v[0]);
    }
}


/*!
 * \brief Encode Level-1 entry (both key and data parts).
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] de DIP entry
 * \param [out] se SIP entry
 *
 * \return SHR_E_NONE.
 */
static int
bcm56780_l1_encode(int u, int m, void *varg, void *de, void *se, uint8_t l1v)
{
    alpm_arg_t *arg = varg;

    if (arg->encode_key_view) {
        if (arg->key.t.app == APP_COMP0) {
            bcm56780_l1_key_view_comp_encode(u, m, arg, de, se);
            return SHR_E_NONE;
        }

        if (l1v == L1V_WIDE) {
            bcm56780_l1_key_view_lpm_encode(u, m, arg, de, se, arg->pkm);
        } else if (l1v == L1V_NARROW_1) {
            bcm56780_l1_key_view_lpm_encode_n1(u, m, arg, de, arg->pkm);
        } else {
            bcm56780_l1_key_view_lpm_encode(u, m, arg, de, NULL, arg->pkm);
        }
    } else if (arg->encode_data_view) {
        if (arg->key.t.app == APP_COMP0) {
            arg->ad.user_data_type = BCM56780_FIXED_COMP_DATA_TYPE;
        }
        bcm56780_l1_data_view_lpm_encode(u, m, arg, de, se, arg->pkm);
    } else {
        if (arg->key.t.app == APP_COMP0) {
            if (l1v == L1V_WIDE) {
                if (arg->encode_data_only) {
                    bcm56780_l1_wide_encode_data(u, m, arg, de, se);
                } else {
                    bcm56780_l1_wide_comp_encode(u, m, arg, de);
                }
            } else {
                if (arg->encode_data_only) {
                    bcm56780_l1_narrow_encode_data(u, m, arg, de, se);
                } else {
                    bcm56780_l1_narrow_comp_encode(u, m, arg, de);
                }
            }
            return SHR_E_NONE;
        }

        if (l1v == L1V_WIDE) {
            if (arg->encode_data_only) {
                bcm56780_l1_wide_encode_data(u, m, arg, de, se);
            } else {
                bcm56780_l1_wide_lpm_encode(u, m, arg, de);
            }
        } else if (l1v == L1V_NARROW_1) {
            if (arg->encode_data_only) {
                bcm56780_l1_narrow_encode_data_n1(u, m, arg, de, se);
            } else {
                bcm56780_l1_narrow_lpm_encode_n1(u, m, arg, de, se, arg->pkm);
            }
        } else {
            if (arg->encode_data_only) {
                bcm56780_l1_narrow_encode_data(u, m, arg, de, se);
            } else {
                bcm56780_l1_narrow_lpm_encode(u, m, arg, de, se, arg->pkm);
            }
        }
    }
    return SHR_E_NONE;
}

/*!
 * \brief Parse key part from single-wide entry.
 *
 * \param [in] u Device u.
 * \param [out] arg ALPM args
 * \param [in] de DIP entry
 *
 * \return SHR_E_XXX.
 */
static void
bcm56780_l1_narrow_decode_data_n1(int u, int m, alpm_arg_t *arg, void *e)
{
    bcm56780_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    FIXED_DATA_t fd_format;
    UNPAIR_T *de = e;
    M_UNPAIR_T *mde = e;

    if (m) {
        M_UNPAIR_FIELD_GET2(DATA1_ALPM1_DATA0f, a1_format);
        M_UNPAIR_FIELD_GET2(DATA1_ASSOC_DATA0f, ad_format);
        M_UNPAIR_FIELD_GET(DATA1_FIXED_DATA0f, fd_format.v[0]);
    } else {
        UNPAIR_FIELD_GET2(DATA1_ALPM1_DATA0f, a1_format);
        UNPAIR_FIELD_GET2(DATA1_ASSOC_DATA0f, ad_format);
        UNPAIR_FIELD_GET(DATA1_FIXED_DATA0f, fd_format.v[0]);
    }

    bcm56780_ln_alpm1_data_format_decode(u, m, arg, &a1_format, &arg->a1);
    bcm56780_fixed_data_format_decode(u, m, &fd_format, NULL, arg);
    bcm56780_assoc_data_format_decode(u, m, &ad_format, arg->key.t.app,
                                      bcm56780_max_dt[u],
                                      &arg->ad);
}


/*!
 * \brief Parse key part from single-wide entry.
 *
 * \param [in] u Device u.
 * \param [out] arg ALPM args
 * \param [in] de DIP entry
 *
 * \return SHR_E_XXX.
 */
static void
bcm56780_l1_narrow_decode_data(int u, int m, alpm_arg_t *arg, void *e)
{
    bcm56780_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    FIXED_DATA_t fd_format;
    UNPAIR_T *de = e;
    M_UNPAIR_T *mde = e;

    if (m) {
        M_UNPAIR_FIELD_GET2(DATA0_ALPM1_DATA0f, a1_format);
        M_UNPAIR_FIELD_GET2(DATA0_ASSOC_DATA0f, ad_format);
        M_UNPAIR_FIELD_GET(DATA0_FIXED_DATA0f, fd_format.v[0]);
    } else {
        UNPAIR_FIELD_GET2(DATA0_ALPM1_DATA0f, a1_format);
        UNPAIR_FIELD_GET2(DATA0_ASSOC_DATA0f, ad_format);
        UNPAIR_FIELD_GET(DATA0_FIXED_DATA0f, fd_format.v[0]);
    }

    bcm56780_ln_alpm1_data_format_decode(u, m, arg, &a1_format, &arg->a1);
    bcm56780_fixed_data_format_decode(u, m, &fd_format, NULL, arg);
    bcm56780_assoc_data_format_decode(u, m, &ad_format, arg->key.t.app,
                                      bcm56780_max_dt[u],
                                      &arg->ad);
}


/*!
 * \brief Parse key part from double-wide entry.
 *
 * \param [in] u Device u.
 * \param [out] arg ALPM args
 * \param [in] de DIP entry
 * \param [in] se SIP entry
 *
 * \return SHR_E_XXX.
 */
static void
bcm56780_l1_wide_decode_data(int u, int m, alpm_arg_t *arg, void *e)
{
    bcm56780_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    FIXED_DATA_t fd_format;
    PAIR_T *de = e;
    M_PAIR_T *mde = e;

    /* Data */
    if (m) {
        M_PAIR_FIELD_GET2(DATA_ALPM1_DATA0f, a1_format);
        M_PAIR_FIELD_GET2(DATA_ASSOC_DATA0f, ad_format);
        M_PAIR_FIELD_GET(DATA_FIXED_DATA0f, fd_format.v[0]);
    } else {
        PAIR_FIELD_GET2(DATA_ALPM1_DATA0f, a1_format);
        PAIR_FIELD_GET2(DATA_ASSOC_DATA0f, ad_format);
        PAIR_FIELD_GET(DATA_FIXED_DATA0f, fd_format.v[0]);
    }

    bcm56780_ln_alpm1_data_format_decode(u, m, arg, &a1_format, &arg->a1);
    bcm56780_fixed_data_format_decode(u, m, &fd_format, NULL, arg);
    bcm56780_assoc_data_format_decode(u, m, &ad_format, arg->key.t.app,
                                      bcm56780_max_dt[u],
                                      &arg->ad);
}


/*!
 * \brief Parse key part from single-wide entry.
 *
 * \param [in] u Device u.
 * \param [out] arg ALPM args
 * \param [in] de DIP entry
 *
 * \return SHR_E_XXX.
 */
static void
bcm56780_l1_narrow_lpm_decode_n1(int u, int m, alpm_arg_t *arg, void *e)
{
    L3_DEFIP_TCAM_KEY_t kdata;
    L3_DEFIP_TCAM_KEY_t kmask;
    int key_mode = 0;
    int vrf, vrf_mask;
    uint8_t sub_prio;
    uint32_t ip_addr, ip_mask;
    bcm56780_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    FIXED_DATA_t fd_format;
    int kt;
    uint8_t large_vrf;
    UNPAIR_T *de = e;
    M_UNPAIR_T *mde = e;

    L3_DEFIP_TCAM_KEY_CLR(kdata);
    L3_DEFIP_TCAM_KEY_CLR(kmask);

    if (m) {
        M_UNPAIR_FIELD_GET2(KEY_ENTRY_1f, kdata);
        M_UNPAIR_FIELD_GET2(MASK_ENTRY_1f, kmask);
        M_UNPAIR_FIELD_GET(DATA1_FIXED_DATA0f, fd_format.v[0]);
    } else {
        UNPAIR_FIELD_GET2(KEY_ENTRY_1f, kdata);
        UNPAIR_FIELD_GET2(MASK_ENTRY_1f, kmask);
        UNPAIR_FIELD_GET(DATA1_FIXED_DATA0f, fd_format.v[0]);
    }
    bcm56780_fixed_data_format_decode(u, m, &fd_format, &sub_prio, arg);

    large_vrf = arg->lpm_large_vrf && sub_prio != SUB_DB_PRIORITY_OVERRIDE;
    if (large_vrf) {
        bcm56780_l1_key_lpm_v4_decode_no_vrf(u, m, &key_mode, &kt, NULL, &kdata);
    } else {
        bcm56780_l1_key_lpm_v4_decode(u, m, NULL, &key_mode, &kt, NULL, &kdata);
    }
    arg->key.t.ipv = bcm56780_npl_kt_to_ipv(u, m, kt);
    if (!arg->key.t.ipv) {
        if (large_vrf) {
            bcm56780_l1_key_lpm_v4_decode_no_vrf(u, m, NULL, NULL,
                                                 &arg->key.ip_mask[0], &kmask);
            bcm56780_l1_key_lpm_v4_decode_no_vrf(u, m, &key_mode, &kt,
                                                 &arg->key.ip_addr[0], &kdata);
        } else {
            bcm56780_l1_key_lpm_v4_decode(u, m, &vrf_mask, NULL, NULL, &ip_mask,
                                          &kmask);
            bcm56780_l1_key_lpm_v4_decode(u, m, &vrf, &key_mode, &kt,
                                          &ip_addr, &kdata);
            arg->key.ip_addr[0] = ip_addr;
            arg->key.ip_mask[0] = ip_mask;
        }
    } else if (key_mode == HALF_WIDE) {
        if (large_vrf) {
            bcm56780_l1_key_lpm_v6_half_decode_no_vrf(u, m, NULL, NULL,
                                                      &arg->key.ip_mask[0],
                                                      &kmask);
            bcm56780_l1_key_lpm_v6_half_decode_no_vrf(u, m, &key_mode, &kt,
                                                      &arg->key.ip_addr[0],
                                                      &kdata);
        } else {
            bcm56780_l1_key_lpm_v6_half_decode(u, m, &vrf_mask, NULL, NULL,
                                               &arg->key.ip_mask[0], &kmask);
            bcm56780_l1_key_lpm_v6_half_decode(u, m, &vrf, &key_mode, &kt,
                                               &arg->key.ip_addr[0], &kdata);
        }
    }
    if (m) {
        M_UNPAIR_FIELD_GET(VALID_1f, arg->valid);
    } else {
        UNPAIR_FIELD_GET(VALID_1f, arg->valid);
    }
    bcm56780_l1_lpm_vt_decode(u, m, sub_prio, vrf, vrf_mask, arg);

    if (m) {
        M_UNPAIR_FIELD_GET2(DATA1_ALPM1_DATA0f, a1_format);
        M_UNPAIR_FIELD_GET2(DATA1_ASSOC_DATA0f, ad_format);
    } else {
        UNPAIR_FIELD_GET2(DATA1_ALPM1_DATA0f, a1_format);
        UNPAIR_FIELD_GET2(DATA1_ASSOC_DATA0f, ad_format);
    }
    bcm56780_ln_alpm1_data_format_decode(u, m, arg, &a1_format, &arg->a1);
    bcm56780_assoc_data_format_decode(u, m, &ad_format, arg->key.t.app,
                                      bcm56780_max_dt[u],
                                      &arg->ad);
}


/*!
 * \brief Parse key part from single-wide entry.
 *
 * \param [in] u Device u.
 * \param [out] arg ALPM args
 * \param [in] de DIP entry
 *
 * \return SHR_E_XXX.
 */
static void
bcm56780_l1_narrow_lpm_decode(int u, int m, alpm_arg_t *arg, void *e)
{
    L3_DEFIP_TCAM_KEY_t kdata[2] = {{{0}}};
    L3_DEFIP_TCAM_KEY_t kmask[2] = {{{0}}};
    int key_mode = 0;
    int vrf, vrf_mask;
    uint8_t sub_prio;
    uint32_t ip_addr, ip_mask;
    bcm56780_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    FIXED_DATA_t fd_format;
    int kt;
    uint8_t large_vrf;
    int valid[2] = {0};
    UNPAIR_T *de = e;
    M_UNPAIR_T *mde = e;

    if (m) {
        M_UNPAIR_FIELD_GET2(KEY_ENTRY_0f, kdata);
        M_UNPAIR_FIELD_GET2(MASK_ENTRY_0f, kmask);
        M_UNPAIR_FIELD_GET(DATA0_FIXED_DATA0f, fd_format.v[0]);
    } else {
        UNPAIR_FIELD_GET2(KEY_ENTRY_0f, kdata);
        UNPAIR_FIELD_GET2(MASK_ENTRY_0f, kmask);
        UNPAIR_FIELD_GET(DATA0_FIXED_DATA0f, fd_format.v[0]);
    }
    bcm56780_fixed_data_format_decode(u, m, &fd_format, &sub_prio, arg);

    large_vrf = arg->lpm_large_vrf && sub_prio != SUB_DB_PRIORITY_OVERRIDE;

    if (large_vrf) {
        bcm56780_l1_key_lpm_v4_decode_no_vrf(u, m, &key_mode, &kt, NULL,
                                             &kdata[0]);
    } else {
        bcm56780_l1_key_lpm_v4_decode(u, m, NULL, &key_mode, &kt, NULL, &kdata[0]);
    }
    arg->key.t.ipv = bcm56780_npl_kt_to_ipv(u, m, kt);
    if (!arg->key.t.ipv) {
        if (large_vrf) {
            bcm56780_l1_key_lpm_v4_decode_no_vrf(u, m, NULL, NULL,
                                                 &arg->key.ip_mask[0],
                                                 &kmask[0]);
            bcm56780_l1_key_lpm_v4_decode_no_vrf(u, m, &key_mode, &kt,
                                                 &arg->key.ip_addr[0],
                                                 &kdata[0]);
        } else {
            bcm56780_l1_key_lpm_v4_decode(u, m, &vrf_mask, NULL, NULL, &ip_mask,
                                          &kmask[0]);
            bcm56780_l1_key_lpm_v4_decode(u, m, &vrf, &key_mode, &kt,
                                          &ip_addr, &kdata[0]);
            arg->key.ip_addr[0] = ip_addr;
            arg->key.ip_mask[0] = ip_mask;
        }

        if (m) {
            M_UNPAIR_FIELD_GET(VALID_0f, arg->valid);
        } else {
            UNPAIR_FIELD_GET(VALID_0f, arg->valid);
        }
    } else if (key_mode == HALF_WIDE) {
        if (large_vrf) {
            bcm56780_l1_key_lpm_v6_half_decode_no_vrf(u, m, NULL, NULL,
                                                      &arg->key.ip_mask[0],
                                                      &kmask[0]);
            bcm56780_l1_key_lpm_v6_half_decode_no_vrf(u, m, &key_mode, &kt,
                                                      &arg->key.ip_addr[0],
                                                      &kdata[0]);
        } else {
            bcm56780_l1_key_lpm_v6_half_decode(u, m, &vrf_mask, NULL, NULL,
                                               &arg->key.ip_mask[0], &kmask[0]);
            bcm56780_l1_key_lpm_v6_half_decode(u, m, &vrf, &key_mode, &kt,
                                               &arg->key.ip_addr[0], &kdata[0]);
        }

        if (m) {
            M_UNPAIR_FIELD_GET(VALID_0f, arg->valid);
        } else {
            UNPAIR_FIELD_GET(VALID_0f, arg->valid);
        }
    } else {
        if (m) {
            M_UNPAIR_FIELD_GET(VALID_0f, valid[0]);
            M_UNPAIR_FIELD_GET(VALID_1f, valid[1]);
            arg->valid = valid[0] && valid[1];

            M_UNPAIR_FIELD_GET2(KEY_ENTRY_1f, kdata[1]);
            M_UNPAIR_FIELD_GET2(MASK_ENTRY_1f, kmask[1]);
        } else {
            UNPAIR_FIELD_GET(VALID_0f, valid[0]);
            UNPAIR_FIELD_GET(VALID_1f, valid[1]);
            arg->valid = valid[0] && valid[1];

            UNPAIR_FIELD_GET2(KEY_ENTRY_1f, kdata[1]);
            UNPAIR_FIELD_GET2(MASK_ENTRY_1f, kmask[1]);
        }
        if (large_vrf) {
            bcm56780_l1_key_lpm_v6_single_decode_no_vrf(u, m, NULL, NULL,
                                                        &arg->key.ip_mask[0],
                                                        &kmask[0],
                                                        &kmask[1]);
            bcm56780_l1_key_lpm_v6_single_decode_no_vrf(u, m, NULL, NULL,
                                                        &arg->key.ip_addr[0],
                                                        &kdata[0],
                                                        &kdata[1]);
        } else {
            bcm56780_l1_key_lpm_v6_single_decode(u, m, &vrf_mask, NULL, NULL,
                                                 &arg->key.ip_mask[0],
                                                 &kmask[0],
                                                 &kmask[1]);
            bcm56780_l1_key_lpm_v6_single_decode(u, m, &vrf, NULL, NULL,
                                                 &arg->key.ip_addr[0],
                                                 &kdata[0],
                                                 &kdata[1]);

        }
    }

    bcm56780_l1_lpm_vt_decode(u, m, sub_prio, vrf, vrf_mask, arg);

    if (m) {
        M_UNPAIR_FIELD_GET2(DATA0_ALPM1_DATA0f, a1_format);
        M_UNPAIR_FIELD_GET2(DATA0_ASSOC_DATA0f, ad_format);
    } else {
        UNPAIR_FIELD_GET2(DATA0_ALPM1_DATA0f, a1_format);
        UNPAIR_FIELD_GET2(DATA0_ASSOC_DATA0f, ad_format);
    }
    bcm56780_ln_alpm1_data_format_decode(u, m, arg, &a1_format, &arg->a1);
    bcm56780_assoc_data_format_decode(u, m, &ad_format, arg->key.t.app,
                                      bcm56780_max_dt[u],
                                      &arg->ad);
}


/*!
 * \brief Parse key part from single-wide FP_COMP IPv4 entry.
 *
 * \param [in] u Device u.
 * \param [out] arg ALPM args
 * \param [in] de FP_COMP IPv4 entry
 *
 * \return SHR_E_XXX.
 */
static void
bcm56780_l1_narrow_comp_decode(int u, int m, alpm_arg_t *arg, void *e)
{
    L3_DEFIP_TCAM_KEY_t kdata[2] = {{{0}}};
    L3_DEFIP_TCAM_KEY_t kmask[2] = {{{0}}};
    int port, port_mask;
    uint8_t sub_prio;
    bcm56780_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    FIXED_DATA_t fd_format;
    int kt;
    uint8_t large_port;
    int valid[2] = {0};
    UNPAIR_T *de = e;
    M_UNPAIR_T *mde = e;

    if (m) {
        M_UNPAIR_FIELD_GET(DATA0_FIXED_DATA0f, fd_format.v[0]);
    } else {
        UNPAIR_FIELD_GET(DATA0_FIXED_DATA0f, fd_format.v[0]);
    }
    bcm56780_fixed_data_format_decode(u, m, &fd_format, &sub_prio, arg);

    large_port = arg->comp_large_port && sub_prio != SUB_DB_PRIORITY_OVERRIDE;


    if (m) {
        M_UNPAIR_FIELD_GET2(KEY_ENTRY_0f, kdata[0]);
        M_UNPAIR_FIELD_GET2(MASK_ENTRY_0f, kmask[0]);
        M_UNPAIR_FIELD_GET2(KEY_ENTRY_1f, kdata[1]);
        M_UNPAIR_FIELD_GET2(MASK_ENTRY_1f, kmask[1]);
    } else {
        UNPAIR_FIELD_GET2(KEY_ENTRY_0f, kdata[0]);
        UNPAIR_FIELD_GET2(MASK_ENTRY_0f, kmask[0]);
        UNPAIR_FIELD_GET2(KEY_ENTRY_1f, kdata[1]);
        UNPAIR_FIELD_GET2(MASK_ENTRY_1f, kmask[1]);
    }

    if (large_port) {
        bcm56780_l1_key_comp_v4_single_decode_no_port(u, m, NULL, NULL,
                                                      &arg->key.ip_mask[0],
                                                      &kmask[0],
                                                      &kmask[1]);
        bcm56780_l1_key_comp_v4_single_decode_no_port(u, m, NULL, &kt,
                                                      &arg->key.ip_addr[0],
                                                      &kdata[0],
                                                      &kdata[1]);
    } else {
        bcm56780_l1_key_comp_v4_single_decode(u, m, &port_mask, NULL, NULL,
                                              &arg->key.ip_mask[0],
                                              &kmask[0],
                                              &kmask[1]);
        bcm56780_l1_key_comp_v4_single_decode(u, m, &port, NULL, &kt,
                                              &arg->key.ip_addr[0],
                                              &kdata[0],
                                              &kdata[1]);

    }
    arg->key.t.ipv = bcm56780_npl_kt_to_ipv(u, m, kt);
    if (m) {
        M_UNPAIR_FIELD_GET(VALID_0f, valid[0]);
        M_UNPAIR_FIELD_GET(VALID_1f, valid[1]);
    } else {
        UNPAIR_FIELD_GET(VALID_0f, valid[0]);
        UNPAIR_FIELD_GET(VALID_1f, valid[1]);
    }
    arg->valid = valid[0] && valid[1];
    bcm56780_l1_comp_vt_decode(u, m, sub_prio, port, port_mask, arg);

    if (m) {
        M_UNPAIR_FIELD_GET2(DATA0_ALPM1_DATA0f, a1_format);
        M_UNPAIR_FIELD_GET2(DATA0_ASSOC_DATA0f, ad_format);
    } else {
        UNPAIR_FIELD_GET2(DATA0_ALPM1_DATA0f, a1_format);
        UNPAIR_FIELD_GET2(DATA0_ASSOC_DATA0f, ad_format);
    }

    bcm56780_ln_alpm1_data_format_decode(u, m, arg, &a1_format, &arg->a1);
    bcm56780_fixed_data_format_decode(u, m, &fd_format, NULL, arg);
    arg->ad.user_data_type = BCM56780_FIXED_COMP_DATA_TYPE;
    bcm56780_assoc_data_format_decode(u, m, &ad_format, arg->key.t.app,
                                      bcm56780_max_dt[u],
                                      &arg->ad);
}


/*!
 * \brief Parse key part from double-wide entry.
 *
 * \param [in] u Device u.
 * \param [out] arg ALPM args
 * \param [in] de DIP entry
 * \param [in] se SIP entry
 *
 * \return SHR_E_XXX.
 */
static void
bcm56780_l1_wide_lpm_decode(int u, int m, alpm_arg_t *arg, void *e)
{
    L3_DEFIP_TCAM_KEY_t kdata[4] = {{{0}}};
    L3_DEFIP_TCAM_KEY_t kmask[4] = {{{0}}};
    int valid[4] = {0};
    int vrf, vrf_mask;
    uint8_t sub_prio;
    bcm56780_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    FIXED_DATA_t fd_format;
    int kt;
    uint8_t large_vrf;
    PAIR_T *de = e;
    M_PAIR_T *mde = e;

    if (m) {
        M_PAIR_FIELD_GET2(KEY0_ENTRY_0f, kdata[0]);
        M_PAIR_FIELD_GET2(KEY0_ENTRY_1f, kdata[1]);
        M_PAIR_FIELD_GET2(KEY1_ENTRY_0f, kdata[2]);
        M_PAIR_FIELD_GET2(KEY1_ENTRY_1f, kdata[3]);
        M_PAIR_FIELD_GET2(MASK0_ENTRY_0f, kmask[0]);
        M_PAIR_FIELD_GET2(MASK0_ENTRY_1f, kmask[1]);
        M_PAIR_FIELD_GET2(MASK1_ENTRY_0f, kmask[2]);
        M_PAIR_FIELD_GET2(MASK1_ENTRY_1f, kmask[3]);
        M_PAIR_FIELD_GET(DATA_FIXED_DATA0f, fd_format.v[0]);
    } else {
        PAIR_FIELD_GET2(KEY0_ENTRY_0f, kdata[0]);
        PAIR_FIELD_GET2(KEY0_ENTRY_1f, kdata[1]);
        PAIR_FIELD_GET2(KEY1_ENTRY_0f, kdata[2]);
        PAIR_FIELD_GET2(KEY1_ENTRY_1f, kdata[3]);
        PAIR_FIELD_GET2(MASK0_ENTRY_0f, kmask[0]);
        PAIR_FIELD_GET2(MASK0_ENTRY_1f, kmask[1]);
        PAIR_FIELD_GET2(MASK1_ENTRY_0f, kmask[2]);
        PAIR_FIELD_GET2(MASK1_ENTRY_1f, kmask[3]);
        PAIR_FIELD_GET(DATA_FIXED_DATA0f, fd_format.v[0]);
    }
    bcm56780_fixed_data_format_decode(u, m, &fd_format, &sub_prio, arg);

    large_vrf = arg->lpm_large_vrf && sub_prio != SUB_DB_PRIORITY_OVERRIDE;
    if (large_vrf) {
        bcm56780_l1_key_lpm_v6_double_decode_no_vrf(u, m, NULL, &kt,
                                                    &arg->key.ip_addr[0],
                                                    &kdata[0],
                                                    &kdata[1],
                                                    &kdata[2],
                                                    &kdata[3]
                                                    );
        bcm56780_l1_key_lpm_v6_double_decode_no_vrf(u, m, NULL, NULL,
                                                    &arg->key.ip_mask[0],
                                                    &kmask[0],
                                                    &kmask[1],
                                                    &kmask[2],
                                                    &kmask[3]
                                                    );

    } else {
        bcm56780_l1_key_lpm_v6_double_decode(u, m, &vrf, NULL, &kt,
                                             &arg->key.ip_addr[0],
                                             &kdata[0],
                                             &kdata[1],
                                             &kdata[2],
                                             &kdata[3]
                                             );
        bcm56780_l1_key_lpm_v6_double_decode(u, m, &vrf_mask, NULL, NULL,
                                             &arg->key.ip_mask[0],
                                             &kmask[0],
                                             &kmask[1],
                                             &kmask[2],
                                             &kmask[3]
                                             );
    }
    arg->key.t.ipv = bcm56780_npl_kt_to_ipv(u, m, kt);
    if (m) {
        M_PAIR_FIELD_GET(VALID0_0f, valid[0]);
        M_PAIR_FIELD_GET(VALID0_1f, valid[1]);
        M_PAIR_FIELD_GET(VALID1_0f, valid[2]);
        M_PAIR_FIELD_GET(VALID1_1f, valid[3]);
    } else {
        PAIR_FIELD_GET(VALID0_0f, valid[0]);
        PAIR_FIELD_GET(VALID0_1f, valid[1]);
        PAIR_FIELD_GET(VALID1_0f, valid[2]);
        PAIR_FIELD_GET(VALID1_1f, valid[3]);
    }
    arg->valid = valid[0] & valid[1] & valid[2] & valid[3];
    bcm56780_l1_lpm_vt_decode(u, m, sub_prio, vrf, vrf_mask, arg);


    /* Data */
    if (m) {
        M_PAIR_FIELD_GET2(DATA_ALPM1_DATA0f, a1_format);
        M_PAIR_FIELD_GET2(DATA_ASSOC_DATA0f, ad_format);
    } else {
        PAIR_FIELD_GET2(DATA_ALPM1_DATA0f, a1_format);
        PAIR_FIELD_GET2(DATA_ASSOC_DATA0f, ad_format);
    }

    bcm56780_ln_alpm1_data_format_decode(u, m, arg, &a1_format, &arg->a1);
    bcm56780_assoc_data_format_decode(u, m, &ad_format, arg->key.t.app,
                                      bcm56780_max_dt[u],
                                      &arg->ad);
}


/*!
 * \brief Parse key part from double-wide FP_COMP IPv6 entry.
 *
 * \param [in] u Device u.
 * \param [out] arg ALPM args
 * \param [in] de FP_COMP IPv6 entry
 *
 * \return SHR_E_XXX.
 */
static void
bcm56780_l1_wide_comp_decode(int u, int m, alpm_arg_t *arg, void *e)
{
    L3_DEFIP_TCAM_KEY_t kdata[4] = {{{0}}};
    L3_DEFIP_TCAM_KEY_t kmask[4] = {{{0}}};
    int valid[4] = {0};
    int port, port_mask;
    uint8_t sub_prio;
    bcm56780_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    FIXED_DATA_t fd_format;
    int kt;
    uint8_t large_port;
    PAIR_T *de = e;
    M_PAIR_T *mde = e;

    if (m) {
        M_PAIR_FIELD_GET2(KEY0_ENTRY_0f, kdata[0]);
        M_PAIR_FIELD_GET2(KEY0_ENTRY_1f, kdata[1]);
        M_PAIR_FIELD_GET2(KEY1_ENTRY_0f, kdata[2]);
        M_PAIR_FIELD_GET2(KEY1_ENTRY_1f, kdata[3]);
        M_PAIR_FIELD_GET2(MASK0_ENTRY_0f, kmask[0]);
        M_PAIR_FIELD_GET2(MASK0_ENTRY_1f, kmask[1]);
        M_PAIR_FIELD_GET2(MASK1_ENTRY_0f, kmask[2]);
        M_PAIR_FIELD_GET2(MASK1_ENTRY_1f, kmask[3]);
        M_PAIR_FIELD_GET(DATA_FIXED_DATA0f, fd_format.v[0]);
    } else {
        PAIR_FIELD_GET2(KEY0_ENTRY_0f, kdata[0]);
        PAIR_FIELD_GET2(KEY0_ENTRY_1f, kdata[1]);
        PAIR_FIELD_GET2(KEY1_ENTRY_0f, kdata[2]);
        PAIR_FIELD_GET2(KEY1_ENTRY_1f, kdata[3]);
        PAIR_FIELD_GET2(MASK0_ENTRY_0f, kmask[0]);
        PAIR_FIELD_GET2(MASK0_ENTRY_1f, kmask[1]);
        PAIR_FIELD_GET2(MASK1_ENTRY_0f, kmask[2]);
        PAIR_FIELD_GET2(MASK1_ENTRY_1f, kmask[3]);
        PAIR_FIELD_GET(DATA_FIXED_DATA0f, fd_format.v[0]);
    }
    bcm56780_fixed_data_format_decode(u, m, &fd_format, &sub_prio, arg);

    large_port = arg->comp_large_port && sub_prio != SUB_DB_PRIORITY_OVERRIDE;
    if (large_port) {
        bcm56780_l1_key_comp_v6_double_decode_no_port(u, m, NULL, &kt,
                                                      &arg->key.ip_addr[0],
                                                      &kdata[0],
                                                      &kdata[1],
                                                      &kdata[2],
                                                      &kdata[3]
                                                      );
        bcm56780_l1_key_comp_v6_double_decode_no_port(u, m, NULL, NULL,
                                                      &arg->key.ip_mask[0],
                                                      &kmask[0],
                                                      &kmask[1],
                                                      &kmask[2],
                                                      &kmask[3]
                                                      );

    } else {
        bcm56780_l1_key_comp_v6_double_decode(u, m, &port, NULL, &kt,
                                              &arg->key.ip_addr[0],
                                              &kdata[0],
                                              &kdata[1],
                                              &kdata[2],
                                              &kdata[3]
                                              );
        bcm56780_l1_key_comp_v6_double_decode(u, m, &port_mask, NULL, NULL,
                                              &arg->key.ip_mask[0],
                                              &kmask[0],
                                              &kmask[1],
                                              &kmask[2],
                                              &kmask[3]
                                              );
    }
    arg->key.t.ipv = bcm56780_npl_kt_to_ipv(u, m, kt);
    if (m) {
        M_PAIR_FIELD_GET(VALID0_0f, valid[0]);
        M_PAIR_FIELD_GET(VALID0_1f, valid[1]);
        M_PAIR_FIELD_GET(VALID1_0f, valid[2]);
        M_PAIR_FIELD_GET(VALID1_1f, valid[3]);
    } else {
        PAIR_FIELD_GET(VALID0_0f, valid[0]);
        PAIR_FIELD_GET(VALID0_1f, valid[1]);
        PAIR_FIELD_GET(VALID1_0f, valid[2]);
        PAIR_FIELD_GET(VALID1_1f, valid[3]);
    }
    arg->valid = valid[0] & valid[1] & valid[2] & valid[3];
    bcm56780_l1_comp_vt_decode(u, m, sub_prio, port, port_mask, arg);


    /* Data */
    if (m) {
        M_PAIR_FIELD_GET2(DATA_ALPM1_DATA0f, a1_format);
        M_PAIR_FIELD_GET2(DATA_ASSOC_DATA0f, ad_format);
    } else {
        PAIR_FIELD_GET2(DATA_ALPM1_DATA0f, a1_format);
        PAIR_FIELD_GET2(DATA_ASSOC_DATA0f, ad_format);
    }

    bcm56780_ln_alpm1_data_format_decode(u, m, arg, &a1_format, &arg->a1);
    bcm56780_fixed_data_format_decode(u, m, &fd_format, NULL, arg);
    arg->ad.user_data_type = BCM56780_FIXED_COMP_DATA_TYPE;
    bcm56780_assoc_data_format_decode(u, m, &ad_format, arg->key.t.app,
                                      bcm56780_max_dt[u],
                                      &arg->ad);
}


/*!
 * \brief Decode Level-1 entry (data part).
 *
 * \param [in] u Device u.
 * \param [out] arg ALPM args
 * \param [in] de DIP entry
 * \param [in] se SIP entry
 *
 * \return SHR_E_NONE.
 */
static int
bcm56780_l1_decode(int u, int m, void *varg, void *de, void *se, uint8_t l1v)
{
    alpm_arg_t *arg = varg;

    if (FP_PKM(arg->pkm)) {
        arg->key.t.app = APP_COMP0;
        if (l1v == L1V_WIDE) {
            if (arg->decode_data_only) {
                bcm56780_l1_wide_decode_data(u, m, arg, de);
            } else {
                bcm56780_l1_wide_comp_decode(u, m, arg, de);
            }
        } else {
            if (arg->decode_data_only) {
                bcm56780_l1_narrow_decode_data(u, m, arg, de);
            } else {
                bcm56780_l1_narrow_comp_decode(u, m, arg, de);
            }
        }
        return SHR_E_NONE;
    }

    arg->key.t.app = APP_LPM;
    if (l1v == L1V_WIDE) {
        if (arg->decode_data_only) {
            bcm56780_l1_wide_decode_data(u, m, arg, de);
        } else {
            bcm56780_l1_wide_lpm_decode(u, m, arg, de);
        }
    } else if (l1v == L1V_NARROW_1) {
        if (arg->decode_data_only) {
            bcm56780_l1_narrow_decode_data_n1(u, m, arg, de);
        } else {
            bcm56780_l1_narrow_lpm_decode_n1(u, m, arg, de);
        }
    } else {
        if (arg->decode_data_only) {
            bcm56780_l1_narrow_decode_data(u, m, arg, de);
        } else {
            bcm56780_l1_narrow_lpm_decode(u, m, arg, de);
        }
    }
    return SHR_E_NONE;

}


static void
bcm56780_l1_wide_template_init(int u)
{
}

static void
bcm56780_l1_narrow_template_init(int u)
{
}

/*!
 * \brief Move half part of one entry to half part of another.
 *
 * Source entry and destiation entry can be same. The source half entry
 * will be cleared when done.
 *
 * \param [in] u Device u.
 * \param [out] arg ALPM args
 * \param [in] de DIP entry
 * \param [in] se SIP entry
 *
 * \return SHR_E_XXX.
 */
static int
bcm56780_l1_half_entry_move_mtop(int u, int m, void *src, void *dst, int x, int y,
                                 int pkm)
{
    uint32_t v1;
    M_UNPAIR_T *se = src;
    M_UNPAIR_T *de = dst;
    int valid;
    L3_DEFIP_TCAM_KEY_t kdata;
    L3_DEFIP_TCAM_KEY_t kmask;
    L3_DEFIP_TCAM_LEVEL1_t key;
    L3_DEFIP_TCAM_LEVEL1_t mask;
    MEMDB_TCAM_M_CTL_MEM0_0_DATA_ONLYm_t data_format_raw;

    L3_DEFIP_TCAM_KEY_CLR(kdata);
    L3_DEFIP_TCAM_KEY_CLR(kmask);

    valid = MEMDB_TCAM_M_CTL_MEM0_0m_VALIDf_GET(*se);
    MEMDB_TCAM_M_CTL_MEM0_0m_KEYf_GET(*se, (uint32_t *)&key);
    MEMDB_TCAM_M_CTL_MEM0_0m_MASKf_GET(*se, (uint32_t *)&mask);

    if (x) { /* source is 1 */
        v1 = !!(valid & 0x2); /* entry 1*/
        ALPM_ASSERT(v1);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_GET(key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_GET(mask, (uint32_t *)&kmask);
        MEMDB_TCAM_M_CTL_MEM0_0m_DATA_ONLY_1f_GET(*se, (uint32_t *)&data_format_raw);
    } else { /* source is 0 */
        v1 = valid & 0x1; /* entry 0 */
        ALPM_ASSERT(v1);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_GET(key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_GET(mask, (uint32_t *)&kmask);
        MEMDB_TCAM_M_CTL_MEM0_0m_DATA_ONLY_0f_GET(*se, (uint32_t *)&data_format_raw);
    }

    MEMDB_TCAM_M_CTL_MEM0_0m_KEYf_GET(*de, (uint32_t *)&key);
    MEMDB_TCAM_M_CTL_MEM0_0m_MASKf_GET(*de, (uint32_t *)&mask);

    valid = MEMDB_TCAM_M_CTL_MEM0_0m_VALIDf_GET(*de);
    if (y) { /* dest is 1 */
        MEMDB_TCAM_M_CTL_MEM0_0m_VALIDf_SET(*de, (valid & 0x1) | (v1 << 1)); /* entry 1 */
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(mask, (uint32_t *)&kmask);
        MEMDB_TCAM_M_CTL_MEM0_0m_DATA_ONLY_1f_SET(*de, (uint32_t *)&data_format_raw);
    } else { /* dest is 0 */
        MEMDB_TCAM_M_CTL_MEM0_0m_VALIDf_SET(*de, (valid & 0x2) | v1); /* entry 0 */
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(mask, (uint32_t *)&kmask);
        MEMDB_TCAM_M_CTL_MEM0_0m_DATA_ONLY_0f_SET(*de, (uint32_t *)&data_format_raw);
    }

    MEMDB_TCAM_M_CTL_MEM0_0m_KEYf_SET(*de, (uint32_t *)&key);
    MEMDB_TCAM_M_CTL_MEM0_0m_MASKf_SET(*de, (uint32_t *)&mask);

    /* Clear source */
    valid = MEMDB_TCAM_M_CTL_MEM0_0m_VALIDf_GET(*se);
    if (x) {
        MEMDB_TCAM_M_CTL_MEM0_0m_VALIDf_SET(*se, valid & 0x1); /* clear entry 1 */
    } else {
        MEMDB_TCAM_M_CTL_MEM0_0m_VALIDf_SET(*se, valid & 0x2); /* clear entry 0 */
    }

    return SHR_E_NONE;
}

/*!
 * \brief Move half part of one entry to half part of another.
 *
 * Source entry and destiation entry can be same. The source half entry
 * will be cleared when done.
 *
 * \param [in] u Device u.
 * \param [out] arg ALPM args
 * \param [in] de DIP entry
 * \param [in] se SIP entry
 *
 * \return SHR_E_XXX.
 */
static int
bcm56780_l1_half_entry_move(int u, int m, void *src, void *dst, int x, int y,
                            int pkm)
{
    uint32_t v1;
    UNPAIR_T *se = src;
    UNPAIR_T *de = dst;
    int valid;
    L3_DEFIP_TCAM_KEY_t kdata;
    L3_DEFIP_TCAM_KEY_t kmask;
    L3_DEFIP_TCAM_LEVEL1_t key;
    L3_DEFIP_TCAM_LEVEL1_t mask;
    MEMDB_TCAM_IFTA80_MEM0_0_DATA_ONLYm_t data_format_raw;


    if (m) {
        return bcm56780_l1_half_entry_move_mtop(u, m, src, dst, x, y, pkm);
    }


    L3_DEFIP_TCAM_KEY_CLR(kdata);
    L3_DEFIP_TCAM_KEY_CLR(kmask);

    valid = MEMDB_TCAM_IFTA80_MEM0_0m_VALIDf_GET(*se);
    MEMDB_TCAM_IFTA80_MEM0_0m_KEYf_GET(*se, (uint32_t *)&key);
    MEMDB_TCAM_IFTA80_MEM0_0m_MASKf_GET(*se, (uint32_t *)&mask);

    if (x) { /* source is 1 */
        v1 = !!(valid & 0x2); /* entry 1*/
        ALPM_ASSERT(v1);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_GET(key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_GET(mask, (uint32_t *)&kmask);
        MEMDB_TCAM_IFTA80_MEM0_0m_DATA_ONLY_1f_GET(*se, (uint32_t *)&data_format_raw);
    } else { /* source is 0 */
        v1 = valid & 0x1; /* entry 0 */
        ALPM_ASSERT(v1);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_GET(key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_GET(mask, (uint32_t *)&kmask);
        MEMDB_TCAM_IFTA80_MEM0_0m_DATA_ONLY_0f_GET(*se, (uint32_t *)&data_format_raw);
    }

    MEMDB_TCAM_IFTA80_MEM0_0m_KEYf_GET(*de, (uint32_t *)&key);
    MEMDB_TCAM_IFTA80_MEM0_0m_MASKf_GET(*de, (uint32_t *)&mask);

    valid = MEMDB_TCAM_IFTA80_MEM0_0m_VALIDf_GET(*de);
    if (y) { /* dest is 1 */
        MEMDB_TCAM_IFTA80_MEM0_0m_VALIDf_SET(*de, (valid & 0x1) | (v1 << 1)); /* entry 1 */
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(mask, (uint32_t *)&kmask);
        MEMDB_TCAM_IFTA80_MEM0_0m_DATA_ONLY_1f_SET(*de, (uint32_t *)&data_format_raw);
    } else { /* dest is 0 */
        MEMDB_TCAM_IFTA80_MEM0_0m_VALIDf_SET(*de, (valid & 0x2) | v1); /* entry 0 */
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(mask, (uint32_t *)&kmask);
        MEMDB_TCAM_IFTA80_MEM0_0m_DATA_ONLY_0f_SET(*de, (uint32_t *)&data_format_raw);
    }

    MEMDB_TCAM_IFTA80_MEM0_0m_KEYf_SET(*de, (uint32_t *)&key);
    MEMDB_TCAM_IFTA80_MEM0_0m_MASKf_SET(*de, (uint32_t *)&mask);

    /* Clear source */
    valid = MEMDB_TCAM_IFTA80_MEM0_0m_VALIDf_GET(*se);
    if (x) {
        MEMDB_TCAM_IFTA80_MEM0_0m_VALIDf_SET(*se, valid & 0x1); /* clear entry 1 */
    } else {
        MEMDB_TCAM_IFTA80_MEM0_0m_VALIDf_SET(*se, valid & 0x2); /* clear entry 0 */
    }

    return SHR_E_NONE;
}



/*!
 * \brief Move half part of one entry to half part of another.
 *
 * Source entry and destiation entry can be same. The source half entry
 * will be cleared when done.
 *
 * \param [in] u Device u.
 * \param [out] arg ALPM args
 * \param [in] de DIP entry
 * \param [in] se SIP entry
 *
 * \return SHR_E_XXX.
 */
static int
bcm56780_l1_half_key_move_mtop(int u, int m, void *src, void *dst, int x, int y,
                               int pkm)
{
    uint32_t v1;
    M_TCAM_VIEW_T *se = src;
    M_TCAM_VIEW_T *de = dst;
    int valid;
    L3_DEFIP_TCAM_KEY_t kdata;
    L3_DEFIP_TCAM_KEY_t kmask;
    L3_DEFIP_TCAM_LEVEL1_t key;
    L3_DEFIP_TCAM_LEVEL1_t mask;

    L3_DEFIP_TCAM_KEY_CLR(kdata);
    L3_DEFIP_TCAM_KEY_CLR(kmask);

    valid = MEMDB_TCAM_M_CTL_MEM0_0_ONLYm_VALIDf_GET(*se);
    MEMDB_TCAM_M_CTL_MEM0_0_ONLYm_KEYf_GET(*se, (uint32_t *)&key);
    MEMDB_TCAM_M_CTL_MEM0_0_ONLYm_MASKf_GET(*se, (uint32_t *)&mask);

    if (x) { /* source is 1 */
        v1 = !!(valid & 0x2); /* entry 1*/
        ALPM_ASSERT(v1);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_GET(key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_GET(mask, (uint32_t *)&kmask);
    } else { /* source is 0 */
        v1 = valid & 0x1; /* entry 0 */
        ALPM_ASSERT(v1);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_GET(key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_GET(mask, (uint32_t *)&kmask);
    }

    MEMDB_TCAM_M_CTL_MEM0_0_ONLYm_KEYf_GET(*de, (uint32_t *)&key);
    MEMDB_TCAM_M_CTL_MEM0_0_ONLYm_MASKf_GET(*de, (uint32_t *)&mask);

    valid = MEMDB_TCAM_M_CTL_MEM0_0_ONLYm_VALIDf_GET(*de);
    if (y) { /* dest is 1 */
        MEMDB_TCAM_M_CTL_MEM0_0_ONLYm_VALIDf_SET(*de, (valid & 0x1) | (v1 << 1)); /* entry 1 */
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(mask, (uint32_t *)&kmask);
    } else { /* dest is 0 */
        MEMDB_TCAM_M_CTL_MEM0_0_ONLYm_VALIDf_SET(*de, (valid & 0x2) | v1); /* entry 0 */
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(mask, (uint32_t *)&kmask);
    }

    MEMDB_TCAM_M_CTL_MEM0_0_ONLYm_KEYf_SET(*de, (uint32_t *)&key);
    MEMDB_TCAM_M_CTL_MEM0_0_ONLYm_MASKf_SET(*de, (uint32_t *)&mask);

    /* Clear source */
    valid = MEMDB_TCAM_M_CTL_MEM0_0_ONLYm_VALIDf_GET(*se);
    if (x) {
        MEMDB_TCAM_M_CTL_MEM0_0_ONLYm_VALIDf_SET(*se, valid & 0x1); /* clear entry 1 */
    } else {
        MEMDB_TCAM_M_CTL_MEM0_0_ONLYm_VALIDf_SET(*se, valid & 0x2); /* clear entry 0 */
    }

    return SHR_E_NONE;
}




/*!
 * \brief Move half part of one entry to half part of another.
 *
 * Source entry and destiation entry can be same. The source half entry
 * will be cleared when done.
 *
 * \param [in] u Device u.
 * \param [out] arg ALPM args
 * \param [in] de DIP entry
 * \param [in] se SIP entry
 *
 * \return SHR_E_XXX.
 */
static int
bcm56780_l1_half_key_move(int u, int m, void *src, void *dst, int x, int y,
                          int pkm)
{
    uint32_t v1;
    TCAM_VIEW_T *se = src;
    TCAM_VIEW_T *de = dst;
    int valid;
    L3_DEFIP_TCAM_KEY_t kdata;
    L3_DEFIP_TCAM_KEY_t kmask;
    L3_DEFIP_TCAM_LEVEL1_t key;
    L3_DEFIP_TCAM_LEVEL1_t mask;

    if (m) {
        return bcm56780_l1_half_key_move_mtop(u, m, src, dst, x, y, pkm);
    }

    L3_DEFIP_TCAM_KEY_CLR(kdata);
    L3_DEFIP_TCAM_KEY_CLR(kmask);

    valid = MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_VALIDf_GET(*se);
    MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_KEYf_GET(*se, (uint32_t *)&key);
    MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_MASKf_GET(*se, (uint32_t *)&mask);

    if (x) { /* source is 1 */
        v1 = !!(valid & 0x2); /* entry 1*/
        ALPM_ASSERT(v1);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_GET(key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_GET(mask, (uint32_t *)&kmask);
    } else { /* source is 0 */
        v1 = valid & 0x1; /* entry 0 */
        ALPM_ASSERT(v1);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_GET(key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_GET(mask, (uint32_t *)&kmask);
    }

    MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_KEYf_GET(*de, (uint32_t *)&key);
    MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_MASKf_GET(*de, (uint32_t *)&mask);

    valid = MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_VALIDf_GET(*de);
    if (y) { /* dest is 1 */
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_VALIDf_SET(*de, (valid & 0x1) | (v1 << 1)); /* entry 1 */
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(mask, (uint32_t *)&kmask);
    } else { /* dest is 0 */
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_VALIDf_SET(*de, (valid & 0x2) | v1); /* entry 0 */
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(mask, (uint32_t *)&kmask);
    }

    MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_KEYf_SET(*de, (uint32_t *)&key);
    MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_MASKf_SET(*de, (uint32_t *)&mask);

    /* Clear source */
    valid = MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_VALIDf_GET(*se);
    if (x) {
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_VALIDf_SET(*se, valid & 0x1); /* clear entry 1 */
    } else {
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_VALIDf_SET(*se, valid & 0x2); /* clear entry 0 */
    }

    return SHR_E_NONE;
}



/*!
 * \brief Move half part of one entry to half part of another.
 *
 * Source entry and destiation entry can be same. The source half entry
 * will be cleared when done.
 *
 * \param [in] u Device u.
 * \param [out] arg ALPM args
 * \param [in] de DIP entry
 * \param [in] se SIP entry
 *
 * \return SHR_E_XXX.
 */
static int
bcm56780_l1_half_key_clear_mtop(int u, int m, void *de, int x, int pkm)
{
    M_TCAM_VIEW_T *e = de;
    int valid;

    /* Clear source */
    valid = MEMDB_TCAM_M_CTL_MEM0_0_ONLYm_VALIDf_GET(*e);
    if (x) {
        MEMDB_TCAM_M_CTL_MEM0_0_ONLYm_VALIDf_SET(*e, valid & 0x1); /* clear entry 1 */
    } else {
        MEMDB_TCAM_M_CTL_MEM0_0_ONLYm_VALIDf_SET(*e, valid & 0x2); /* clear entry 0 */
    }

    return SHR_E_NONE;
}



/*!
 * \brief Move half part of one entry to half part of another.
 *
 * Source entry and destiation entry can be same. The source half entry
 * will be cleared when done.
 *
 * \param [in] u Device u.
 * \param [out] arg ALPM args
 * \param [in] de DIP entry
 * \param [in] se SIP entry
 *
 * \return SHR_E_XXX.
 */
static int
bcm56780_l1_half_key_clear(int u, int m, void *de, int x, int pkm)
{
    TCAM_VIEW_T *e = de;
    int valid;

    if (m) {
        return bcm56780_l1_half_key_clear_mtop(u, m, de, x, pkm);
    }

    /* Clear source */
    valid = MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_VALIDf_GET(*e);
    if (x) {
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_VALIDf_SET(*e, valid & 0x1); /* clear entry 1 */
    } else {
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_VALIDf_SET(*e, valid & 0x2); /* clear entry 0 */
    }

    return SHR_E_NONE;
}

static int
bcm56780_ln_sid_index_map(int u, int m, alpm_sid_index_map_t *map_info)
{
    int sid_idx;
    static bcmdrd_sid_t l2_sids[] = {
        L3_DEFIP_ALPM_LEVEL2_B0m,
        L3_DEFIP_ALPM_LEVEL2_B1m,
        L3_DEFIP_ALPM_LEVEL2_B2m,
        L3_DEFIP_ALPM_LEVEL2_B3m,
        L3_DEFIP_ALPM_LEVEL2_B4m,
        L3_DEFIP_ALPM_LEVEL2_B5m
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
    static bcmdrd_sid_t m_l2_sids[] = {
        M_DEFIP_ALPM_LEVEL2_B0m,
        M_DEFIP_ALPM_LEVEL2_B1m,
        M_DEFIP_ALPM_LEVEL2_B2m,
        M_DEFIP_ALPM_LEVEL2_B3m,
        M_DEFIP_ALPM_LEVEL2_B4m,
        M_DEFIP_ALPM_LEVEL2_B5m
    };
    static bcmdrd_sid_t m_l3_sids[] = {
        M_DEFIP_ALPM_LEVEL3_B0m,
        M_DEFIP_ALPM_LEVEL3_B1m,
        M_DEFIP_ALPM_LEVEL3_B2m,
        M_DEFIP_ALPM_LEVEL3_B3m,
        M_DEFIP_ALPM_LEVEL3_B4m,
        M_DEFIP_ALPM_LEVEL3_B5m,
        M_DEFIP_ALPM_LEVEL3_B6m,
        M_DEFIP_ALPM_LEVEL3_B7m,
    };
    int l2_max;
    int l3_max;

    if (m == ALPM_0) {
        l2_max = sizeof (l2_sids) / sizeof (bcmdrd_sid_t);
        l3_max = sizeof (l3_sids) / sizeof (bcmdrd_sid_t);
    } else {
        l2_max = sizeof (m_l2_sids) / sizeof (bcmdrd_sid_t);
        l3_max = sizeof (m_l3_sids) / sizeof (bcmdrd_sid_t);
    }

    if (map_info->wide == LEVEL2) {
        sid_idx = (map_info->index >> bcm56780_l2_bucket_bits[u][m]) & 0x7;
        if (sid_idx >= l2_max) {
            return SHR_E_INTERNAL;
        }
        if (m == ALPM_0) {
            map_info->sid = l2_sids[sid_idx];
        } else {
            map_info->sid = m_l2_sids[sid_idx];
        }
        map_info->sid_index = map_info->index & bcm56780_l2_bucket_mask[u][m];
    } else if (map_info->wide == LEVEL3) {
        sid_idx = (map_info->index >> bcm56780_l3_bucket_bits[u][m]) & 0x7;
        if (sid_idx >= l3_max) {
            return SHR_E_INTERNAL;
        }
        if (m == ALPM_0) {
            map_info->sid = l3_sids[sid_idx];
        } else {
            map_info->sid = m_l3_sids[sid_idx];
        }
        map_info->sid_index = map_info->index & bcm56780_l3_bucket_mask[u][m];
    }

    return SHR_E_NONE;
}



static int
bcm56780_l1_sid_index_map(int u, int m, alpm_sid_index_map_t *map_info)
{
    int depth, view;
    bcmdrd_sid_t *sid;
    int t_num;

    depth = BCM56780_TCAM_DEPTH;


    if (map_info->wide == WIDE_DOUBLE) {
        if (map_info->sid_type == SID_TYPE_TCAM) {
            view = PAIR_IDX_TCAM0_ONLY_VIEW;
        } else if (map_info->sid_type == SID_TYPE_TCAM2) {
            view = PAIR_IDX_TCAM1_ONLY_VIEW;
        } else if (map_info->sid_type == SID_TYPE_DATA) {
            view = PAIR_IDX_DATA_ONLY_VIEW;
            depth *= 2;
        } else if (map_info->sid_type == SID_TYPE_COMBINED) {
            view = PAIR_IDX_COMBINED_VIEW;
        } else {
            return SHR_E_INTERNAL;
        }
        t_num = (map_info->index / depth);
        if (m) {
            sid = bcm56780_pair_sid_mtop[UNPAIR_TO_PAIR_TCAM_NUM_MTOP(t_num)];
        } else {
            sid = bcm56780_pair_sid[UNPAIR_TO_PAIR_TCAM_NUM(t_num)];
        }
    } else {
        if (map_info->sid_type == SID_TYPE_TCAM) {
            view = UNPAIR_IDX_TCAM_ONLY_VIEW;
        } else if (map_info->sid_type == SID_TYPE_DATA) {
            depth *= 2;
            view = UNPAIR_IDX_DATA_ONLY_VIEW;
        } else if (map_info->sid_type == SID_TYPE_COMBINED) {
            view = UNPAIR_IDX_COMBINED_VIEW;
        } else {
            return SHR_E_INTERNAL;
        }
        t_num = (map_info->index / depth);
        if (m) {
            sid = bcm56780_unpair_sid_mtop[t_num];
        } else {
            sid = bcm56780_unpair_sid[t_num];
        }
    }

    map_info->sid = sid[view];
    map_info->sid_index = map_info->index % depth;

    return SHR_E_NONE;
}

/*!
 * \brief Map from hw sid to base sid & index, plus other view sids.
 *
 * \param [in]  u Device u.
 * \param [in]  hw_sid Input multidepth hw sid
 * \param [out] out_info Map output info
 *
 * \return SHR_E_XXX.
 */
static int
bcm56780_l1_sid_index_reverse_map_mtop(int u, int m, bcmdrd_sid_t hw_sid,
                                       alpm_sid_index_out_t *out_info)
{
    int i;
    int depth = BCM56780_TCAM_DEPTH;

    out_info->bank_depth = depth;

    for (i = 0; i < UNPAIR_TCAM_COUNT; i++) {
        if (hw_sid == bcm56780_unpair_sid_mtop[i][UNPAIR_IDX_TCAM_ONLY_VIEW]) {
            out_info->base_sid =
                bcm56780_unpair_sid_mtop[0][UNPAIR_IDX_TCAM_ONLY_VIEW];
            out_info->base_index = i * depth;
            return SHR_E_NONE;
        }
        if (hw_sid == bcm56780_unpair_sid_mtop[i][UNPAIR_IDX_DATA_ONLY_VIEW]) {
            out_info->base_sid =
                bcm56780_unpair_sid_mtop[0][UNPAIR_IDX_DATA_ONLY_VIEW];
            out_info->base_index = i * depth;
            return SHR_E_NONE;
        }
        if (hw_sid == bcm56780_unpair_sid_mtop[i][UNPAIR_IDX_COMBINED_VIEW]) {
            out_info->base_sid =
                bcm56780_unpair_sid_mtop[0][UNPAIR_IDX_COMBINED_VIEW];
            out_info->base_index = i * depth;
            return SHR_E_NONE;
        }
    }

    for (i = 0; i < PAIR_TCAM_COUNT; i++) {
        if (hw_sid == bcm56780_pair_sid_mtop[i][PAIR_IDX_COMBINED_VIEW]) {
            out_info->base_sid =
                bcm56780_pair_sid_mtop[0][PAIR_IDX_COMBINED_VIEW];
            /* rest don't matter for SER correction */
            return SHR_E_NONE;
        }
    }

    out_info->base_sid = INVALID_SID;
    return SHR_E_NOT_FOUND;
}

/*!
 * \brief Map from hw sid to base sid & index, plus other view sids.
 *
 * \param [in]  u Device u.
 * \param [in]  hw_sid Input multidepth hw sid
 * \param [out] out_info Map output info
 *
 * \return SHR_E_XXX.
 */
static int
bcm56780_l1_sid_index_reverse_map(int u, int m, bcmdrd_sid_t hw_sid,
                                  alpm_sid_index_out_t *out_info)
{
    int i;
    int depth = BCM56780_TCAM_DEPTH;

    if (m) {
        return bcm56780_l1_sid_index_reverse_map_mtop(u, m, hw_sid, out_info);
    }

    out_info->bank_depth = depth;

    for (i = 0; i < UNPAIR_TCAM_COUNT; i++) {
        if (hw_sid == bcm56780_unpair_sid[i][UNPAIR_IDX_TCAM_ONLY_VIEW]) {
            out_info->base_sid =
                bcm56780_unpair_sid[0][UNPAIR_IDX_TCAM_ONLY_VIEW];
            out_info->base_index = i * depth;
            return SHR_E_NONE;
        }
        if (hw_sid == bcm56780_unpair_sid[i][UNPAIR_IDX_DATA_ONLY_VIEW]) {
            out_info->base_sid =
                bcm56780_unpair_sid[0][UNPAIR_IDX_DATA_ONLY_VIEW];
            out_info->base_index = i * depth;
            return SHR_E_NONE;
        }
        if (hw_sid == bcm56780_unpair_sid[i][UNPAIR_IDX_COMBINED_VIEW]) {
            out_info->base_sid =
                bcm56780_unpair_sid[0][UNPAIR_IDX_COMBINED_VIEW];
            out_info->base_index = i * depth;
            return SHR_E_NONE;
        }
    }

    for (i = 0; i < PAIR_TCAM_COUNT; i++) {
        if (hw_sid == bcm56780_pair_sid[i][PAIR_IDX_COMBINED_VIEW]) {
            out_info->base_sid = bcm56780_pair_sid[0][PAIR_IDX_COMBINED_VIEW];
            /* rest don't matter for SER correction */
            return SHR_E_NONE;
        }
    }

    out_info->base_sid = INVALID_SID;
    return SHR_E_NOT_FOUND;
}

/*!
 * \brief Initialize device specific info
 *
 * \param [in] u Device u.
 * \param [out] dev Device specific info.
 *
 * \return SHR_E_XXX.
 */
static int
bcm56780_device_info_init(int u, int m, alpm_dev_info_t *dev)
{
    bcmlrd_sid_t ltid;
    bcmlrd_sid_t fid;
    int index_count;
    int rv = SHR_E_NONE;
    int bits;
    const bcmptm_alpm_variant_info_t *info = bcmptm_rm_alpm_variant_get(u, m);

    SHR_FUNC_ENTER(u);

    if (dev) {
        if (info) {
            bits = info->lpm_ipv4->misc_end_bit -
                   info->lpm_ipv4->misc_start_bit + 1;
            bcm56780_alpm_max_vrf[u] = (1 << bits) - 1;
            bits = info->comp_ipv4->misc_end_bit -
                   info->comp_ipv4->misc_start_bit + 1;
            bcm56780_alpm_max_comp_port[u] = (1 << bits) - 1;
        }
        dev->max_vrf = bcm56780_alpm_max_vrf[u];
        dev->max_fp_op = bcm56780_alpm_max_comp_port[u];

        if (m == ALPM_0) {
            dev->tcam_blocks = 8;
            dev->l1_blocks = 4;
            dev->l1_tcams_shared = 1;
            dev->tcam_depth = BCM56780_TCAM_DEPTH * 2;

            dev->pair_sid = MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m;
            dev->unpair_sid = MEMDB_TCAM_IFTA80_MEM0_0m;
            dev->tcam_only_sid = MEMDB_TCAM_IFTA80_MEM0_0_ONLYm;
            dev->data_only_sid = MEMDB_TCAM_IFTA80_MEM0_0_DATA_ONLYm;
            dev->l2_sid = L3_DEFIP_ALPM_LEVEL2_B0m;
            dev->l3_sid = L3_DEFIP_ALPM_LEVEL3_B0m;
        } else {
            dev->tcam_blocks = 16;
            dev->l1_blocks = 8;
            dev->l1_tcams_shared = 1;
            dev->tcam_depth = BCM56780_TCAM_DEPTH;

            dev->pair_sid = MEMDB_TCAM_M_CTL_MEM0_0_MEM0_1m;
            dev->unpair_sid = MEMDB_TCAM_M_CTL_MEM0_0m;
            dev->tcam_only_sid = MEMDB_TCAM_M_CTL_MEM0_0_ONLYm;
            dev->data_only_sid = MEMDB_TCAM_M_CTL_MEM0_0_DATA_ONLYm;
            dev->l2_sid = M_DEFIP_ALPM_LEVEL2_B0m;
            dev->l3_sid = M_DEFIP_ALPM_LEVEL3_B0m;
        }

        dev->feature_pkms = 8;
        dev->feature_cache_dt = 1;
        dev->feature_urpf = 1;

        dev->pivot_min_format_0 = 1;
        dev->pivot_max_format_0 = 6;
        dev->pivot_min_format_1 = 7;
        dev->pivot_max_format_1 = 10;
        dev->pivot_min_format_2 = 11;
        dev->pivot_max_format_2 = 13;
        dev->pivot_min_format_3 = 14;
        dev->pivot_max_format_3 = 15;

        dev->route_min_format_0 = 1;
        dev->route_max_format_0 = 11;
        dev->route_min_format_1 = 12;
        dev->route_max_format_1 = 15;
        dev->route_min_format_2 = 16;
        dev->route_max_format_2 = 20;
        dev->route_min_format_3 = 21;
        dev->route_max_format_3 = 24;

        dev->l2p_route_fmts = &bcm56780_l2p_route_fmts[0];
        dev->w_size = sizeof(PAIR_T);
        dev->n_size = sizeof(UNPAIR_T);
        assert(sizeof(M_PAIR_T) == sizeof(PAIR_T));
        assert(sizeof(M_UNPAIR_T) == sizeof(UNPAIR_T));
        dev->alpm_ver = ALPM_VERSION_1;
        dev->l1_data_only_x2 = 1;
        dev->max_base_entries = 20;
        dev->key_data_separate[0] = 1; /* For single half */
        dev->key_data_separate[1] = 1; /* For double */
        dev->single_tcam_start_bit = 0;
        dev->single_tcam_end_bit = 209;

        dev->single_data_0_start_bit = 210;
        dev->single_data_0_end_bit = 378;
        dev->single_data_1_start_bit = 379;
        dev->single_data_1_end_bit = 547;

        dev->double_data_start_bit = 420;
        dev->double_data_end_bit = 588;

        dev->double_tcam_0_start_bit = 0;
        dev->double_tcam_0_end_bit = 209;
        dev->double_tcam_1_start_bit = 210;
        dev->double_tcam_1_end_bit = 419;
        dev->l1_ipv6_single_len = 22;
        dev->l1_ipv6_double_len = 68;
        dev->l1_ipv6_quad_len = 128;
        dev->feature_l1_ipv6_len_nonstandard = 1;
        dev->feature_has_4_data_types = 1;
        dev->feature_large_data_type[DB0] = 0;
        dev->feature_large_data_type[DB1] = 0;
        dev->feature_large_data_type[DB2] = 0;
        dev->feature_large_data_type[DB3] = 0;
        bcm56780_max_dt[u] = DATA_FULL;
        bcm56780_wide2_mode[u] = 0xf; /* invalid for now */
        bcm56780_wide3_mode[u] = 0xf; /* invalid for now */

        bcm56780_l1_wide_template_init(u);
        bcm56780_l1_narrow_template_init(u);

        if (m == ALPM_0) {
            index_count = bcmdrd_pt_index_count(u, L3_DEFIP_ALPM_LEVEL2_B0m);
            bcm56780_l2_bucket_mask[u][m] = index_count ? (index_count - 1) : 0;
            bcm56780_l2_bucket_bits[u][m] = shr_util_log2(index_count);

            index_count = bcmdrd_pt_index_count(u, L3_DEFIP_ALPM_LEVEL3_B0m);
            bcm56780_l3_bucket_mask[u][m] = index_count ? (index_count - 1) : 0;;
            bcm56780_l3_bucket_bits[u][m] = shr_util_log2(index_count);

            if (info) {
                bcm56780_lpm_v4_key_type[u][m] =
                    info->main_key_type->lpm_ipv4; /* 2 */
                bcm56780_lpm_v6_key_type[u][m] =
                    info->main_key_type->lpm_ipv6; /* 3 */
                bcm56780_comp_v4_key_type[u][m] =
                    info->main_key_type->comp_ipv4; /* 1 */
                bcm56780_comp_v6_key_type[u][m] =
                    info->main_key_type->comp_ipv6; /* 0 */
            }
        } else {
            index_count = bcmdrd_pt_index_count(u, M_DEFIP_ALPM_LEVEL2_B0m);
            if (index_count == 0) {
                SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
            }
            bcm56780_l2_bucket_mask[u][m] = index_count - 1;
            bcm56780_l2_bucket_bits[u][m] = shr_util_log2(index_count);

            index_count = bcmdrd_pt_index_count(u, M_DEFIP_ALPM_LEVEL3_B0m);
            if (index_count == 0) {
                SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
            }
            bcm56780_l3_bucket_mask[u][m] = index_count - 1;
            bcm56780_l3_bucket_bits[u][m] = shr_util_log2(index_count);

            if (info) {
                bcm56780_lpm_v4_key_type[u][m] = info->mtop_key_type->lpm_ipv4; /* 3 */
                bcm56780_lpm_v6_key_type[u][m] = info->mtop_key_type->lpm_ipv6; /* 0 */
                bcm56780_comp_v4_key_type[u][m] = info->mtop_key_type->comp_ipv4; /* 2 */
                bcm56780_comp_v6_key_type[u][m] = info->mtop_key_type->comp_ipv6; /* 1 */
            }

        }


        /* Get the wide mode enum value as defined by NPL */
        rv = bcmlrd_table_field_name_to_idx(u, "L3_IPV4_UC_ROUTE", "DATA_TYPE",
                                            &ltid, &fid);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
        if (rv != SHR_E_NOT_FOUND) {
            bcmlrd_resolve_enum(u, ltid, fid, "WIDE_MODE",
                                &bcm56780_wide_mode[u]);
            bcmlrd_resolve_enum(u, ltid, fid, "NARROW_MODE",
                                &bcm56780_narrow_mode[u]);
        }
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
bcm56780_a0_rm_alpm_driver_register(int u, bcmptm_rm_alpm_driver_t *drv)
{
    SHR_FUNC_ENTER(u);

    if (drv) {
        drv->l1_encode = bcm56780_l1_encode;
        drv->l1_decode = bcm56780_l1_decode;
        drv->l1_half_entry_move = bcm56780_l1_half_entry_move;
        drv->ln_base_entry_decode = bcm56780_ln_base_entry_decode;
        drv->ln_base_entry_encode = bcm56780_ln_base_entry_encode;
        drv->ln_raw_bucket_decode = bcm56780_ln_raw_bucket_decode;
        drv->ln_raw_bucket_encode = bcm56780_ln_raw_bucket_encode;
        drv->ln_raw_bucket_entry_get = bcm56780_ln_raw_bucket_entry_get;
        drv->ln_raw_bucket_entry_set = bcm56780_ln_raw_bucket_entry_set;
        drv->ln_base_entry_info = bcm56780_ln_base_entry_info;
        drv->ln_base_entry_move = bcm56780_ln_base_entry_move;
        drv->device_info_init   = bcm56780_device_info_init;
        drv->l1_sid_index_map = bcm56780_l1_sid_index_map;
        drv->ln_sid_index_map = bcm56780_ln_sid_index_map;
        drv->l1_half_key_clear = bcm56780_l1_half_key_clear;
        drv->l1_half_key_move = bcm56780_l1_half_key_move;
        drv->l1_sid_index_reverse_map = bcm56780_l1_sid_index_reverse_map;

    } else {

        SHR_ERR_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}



