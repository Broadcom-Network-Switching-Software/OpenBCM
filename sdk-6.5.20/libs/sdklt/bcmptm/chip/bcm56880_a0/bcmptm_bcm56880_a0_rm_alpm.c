/*! \file bcmptm_bcm56880_a0_rm_alpm.c
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
#include <bcmdrd/chip/bcm56880_a0_defs.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmlrd/bcmlrd_enum.h>
#include <bcmptm/bcmptm_internal.h>

#include "../rm_alpm/rm_alpm_common.h"
#include "bcmptm_rm_alpm_common.h"
#include "bcmptm_bcm56880_a0_rm_alpm.h"
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_conf.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMALPM
#define BCM56880_ALPM_DEFAULT_MAX_VRF    1023
#define ALPM_VRF_MASK   32767
#define BCM56880_TCAM_DEPTH      512
#define ALPM_MAX_FP_OP  65535

#define PDD_APP_BIT     3
#define PDD_IPV_BIT     2
#define PDD_SRC_BIT     1
#define PDD_FULL_BIT    0

#define UNPAIR_T    L3_DEFIP_LEVEL1_FMT_t
#define PAIR_T      MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_t

#define DATA_VIEW_T    MEMDB_TCAM_IFTA80_MEM0_0_DATA_ONLYm_t
#define TCAM_VIEW_T    MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_t

#define SUB_DB_PRIORITY_OVERRIDE    0
#define SUB_DB_PRIORITY_VRF         1
#define SUB_DB_PRIORITY_GLOBAL      2

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
    L3_DEFIP_LEVEL1m_##_f##_SET(*de, (_v))
#define UNPAIR_FIELD_SET2(_f, _v)        \
    L3_DEFIP_LEVEL1m_##_f##_SET(*de, (uint32_t *)&(_v))
#define UNPAIR_URPF_FIELD_SET(_f, _v)    \
    L3_DEFIP_LEVEL1m_##_f##_SET(*se, (_v))
#define UNPAIR_URPF_FIELD_SET2(_f, _v)   \
    L3_DEFIP_LEVEL1m_##_f##_SET(*se, (uint32_t *)&(_v))

#define PAIR_FIELD_SET(_f, _v)         \
    L3_DEFIP_PAIR_LEVEL1m_##_f##_SET(*de, (_v))
#define PAIR_FIELD_SET2(_f, _v)        \
    L3_DEFIP_PAIR_LEVEL1m_##_f##_SET(*de, (uint32_t *)&(_v))
#define PAIR_URPF_FIELD_SET(_f, _v)    \
    L3_DEFIP_PAIR_LEVEL1m_##_f##_SET(*se, (_v))
#define PAIR_URPF_FIELD_SET2(_f, _v)   \
    L3_DEFIP_PAIR_LEVEL1m_##_f##_SET(*se, (uint32_t *)&(_v))

#define UNPAIR_FIELD_GET(_f, _v)       \
    (_v) = L3_DEFIP_LEVEL1m_##_f##_GET(*de)
#define UNPAIR_FIELD_GET2(_f, _v)      \
    L3_DEFIP_LEVEL1m_##_f##_GET(*de, (uint32_t *)&(_v))
#define UNPAIR_URPF_FIELD_GET(_f, _v)  \
    (_v) = L3_DEFIP_LEVEL1m_##_f##_GET(*se)
#define UNPAIR_URPF_FIELD_GET2(_f, _v) \
    L3_DEFIP_LEVEL1m_##_f##_GET(*se, (uint32_t *)&(_v))

#define PAIR_FIELD_GET(_f, _v)     \
    (_v) = L3_DEFIP_PAIR_LEVEL1m_##_f##_GET(*de)
#define PAIR_URPF_FIELD_GET(_f, _v) \
    (_v) = L3_DEFIP_PAIR_LEVEL1m_##_f##_GET(*se)
#define PAIR_FIELD_GET2(_f, _v)     \
    L3_DEFIP_PAIR_LEVEL1m_##_f##_GET(*de, (uint32_t *)&(_v))
#define PAIR_URPF_FIELD_GET2(_f, _v) \
    L3_DEFIP_PAIR_LEVEL1m_##_f##_GET(*se, (uint32_t *)&(_v))

#define HALF_WIDE   2
#define SINGLE_WIDE 0
#define DOUBLE_WIDE 1

typedef union ALPM_FLEX_POLICY_DATA_s {
    uint32_t v[2];
    uint32_t alpm_flex_policy_data[2];
    uint32_t _alpm_flex_policy_data;
} ALPM_FLEX_POLICY_DATA_t;

#define ALPM_FLEX_POLICY_DATA_SET(r, s, e, v) \
    bcmdrd_field32_set((r).alpm_flex_policy_data,s,e,v)

#define ALPM_FLEX_POLICY_DATA_GET(r, s, e) \
    bcmdrd_field32_get((r).alpm_flex_policy_data,s,e)

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

/*******************************************************************************
 * Typedefs
 */

typedef struct format_info_s {
    uint8_t prefix_len;
    uint8_t full;
    uint8_t num_entries;
} format_info_t;

typedef union bcm56880_assoc_data_s {
    ASSOC_DATA_FULL_t full;
    ASSOC_DATA_REDUCED_t red;
    uint32_t v[2];
} bcm56880_assoc_data_t;

/*******************************************************************************
 * Private variables
 */
extern shr_lmm_hdl_t bcmptm_rm_alpm_arg_lmem[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
static uint64_t wide_mode[BCMDRD_CONFIG_MAX_UNITS] = {0};

static int bcm56880_alpm_max_vrf[BCMDRD_CONFIG_MAX_UNITS] = {0};

static uint8_t bcm56880_l2_bucket_bits[BCMDRD_CONFIG_MAX_UNITS] = {0};
static uint8_t bcm56880_l3_bucket_bits[BCMDRD_CONFIG_MAX_UNITS] = {0};
static uint32_t bcm56880_l2_bucket_mask[BCMDRD_CONFIG_MAX_UNITS] = {0};
static uint32_t bcm56880_l3_bucket_mask[BCMDRD_CONFIG_MAX_UNITS] = {0};

/* Mapping logical formats to physical formats. */
static int bcm56880_l2p_route_fmts[] =
    {0, 13, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

/* Mapping physical formats to logical formats. */
static int bcm56880_p2l_route_fmts[] =
    {0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 1};


static format_info_t bcm56880_route_base_entry_info[] = {
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
    {.prefix_len = 129, .full = 0, .num_entries = 3},   /* 10 - 9  */
    {.prefix_len = 34,  .full = 1, .num_entries = 6},   /* 11 - 10 */
    {.prefix_len = 74,  .full = 1, .num_entries = 4},   /* 12 - 11 */
    {.prefix_len = 129, .full = 1, .num_entries = 2}    /* 13 - 12 */
};

static format_info_t bcm56880_pivot_base_entry_info[] = {
    {.prefix_len = 0,   .full = 0, .num_entries = 0}, /* 0 */
    {.prefix_len = 7,   .full = 0, .num_entries = 6}, /* 1 */
    {.prefix_len = 23,  .full = 0, .num_entries = 5}, /* 2 */
    {.prefix_len = 47,  .full = 0, .num_entries = 4}, /* 3 */
    {.prefix_len = 87,  .full = 0, .num_entries = 3}, /* 4 */
    {.prefix_len = 129, .full = 0, .num_entries = 2}, /* 5 */
    {.prefix_len = 17,  .full = 1, .num_entries = 4}, /* 6 */
    {.prefix_len = 57,  .full = 1, .num_entries = 3}, /* 7 */
    {.prefix_len = 129, .full = 1, .num_entries = 2}  /* 8 */
};

static
bcmdrd_sid_t bcm56880_unpair_sid[UNPAIR_TCAM_COUNT][UNPAIR_VIEW_CNT] = {
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
bcmdrd_sid_t bcm56880_pair_sid[PAIR_TCAM_COUNT][PAIR_VIEW_CNT] = {
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

/*******************************************************************************
 * Private Functions
 */

static void
bcm56880_l1_tcam_key_format_encode(int u, int vrf, int key_mode,
                                   alpm_ip_ver_t v6, uint32_t ip_addr,
                                   L3_DEFIP_TCAM_KEY_t *kdata)
{
    FMT_TCAM_KEY_FIELD_SET(kdata, VRF_IDf, vrf);
    FMT_TCAM_KEY_FIELD_SET(kdata, KEY_MODEf, key_mode);
    FMT_TCAM_KEY_FIELD_SET(kdata, KEY_TYPEf, v6);
    FMT_TCAM_KEY_FIELD_SET(kdata, IP_ADDRf, ip_addr);
}

static void
bcm56880_l1_tcam_key_format_decode(int u, int *vrf, int *key_mode,
                                   alpm_ip_ver_t *v6, uint32_t *ip_addr,
                                   L3_DEFIP_TCAM_KEY_t *kdata)
{
    if (vrf) {
        FMT_TCAM_KEY_FIELD_GET(kdata, VRF_IDf, *vrf);
    }
    if (key_mode) {
        FMT_TCAM_KEY_FIELD_GET(kdata, KEY_MODEf, *key_mode);
    }
    if (v6) {
        FMT_TCAM_KEY_FIELD_GET(kdata, KEY_TYPEf, *v6);
    }
    if (ip_addr) {
        FMT_TCAM_KEY_FIELD_GET(kdata, IP_ADDRf, *ip_addr);
    }
}

static void
bcm56880_assoc_data_format_encode(int u, bcm56880_assoc_data_t *buf, uint8_t app,
                                  int full, assoc_data_t *assoc)
{
    const alpm_config_t *config;
    uint8_t i;
    ALPM_FLEX_POLICY_DATA_t bufp;
    const bcmptm_alpm_variant_info_t *info;
    const pdd_field_info_t *pdd_info;

    ALPM_ASSERT(sizeof(*buf) <= sizeof(assoc->fmt_ad));
    sal_memset(&bufp, 0, sizeof(bufp));

    if (full && (app == APP_LPM)) {
        info = bcmptm_rm_alpm_variant_get(u, ALPM_0);
        pdd_info = info->l3uc_pdd_info;

        config = bcmptm_rm_alpm_config(u, ALPM_0);
        /* Propagate correct destination_type */
        if (assoc->user_data_type == DATA_REDUCED) {
            if ((assoc->fmt_ad[0] & config->destination_mask) == config->destination) {
                assoc->fmt_ad[1] = config->destination_type_match;
            } else {
                assoc->fmt_ad[1] = config->destination_type_non_match;
            }

            for (i = 2; i < info->l3uc_pdd_info_count; i++) {
                assoc->fmt_ad[i] = 0; /* clean up the rest */
            }
        }

        /* using full format to encode */
        for (i = 0; i < info->l3uc_pdd_info_count; i++) {
            ALPM_FLEX_POLICY_DATA_SET(bufp,
                pdd_info->start_bit, pdd_info->end_bit, assoc->fmt_ad[i]);
            pdd_info++;
        }

        sal_memset(&buf->full, 0, sizeof(buf->full));
        sal_memcpy(&buf->full, &bufp, sizeof(bufp));
    } else {
        sal_memcpy(buf, &assoc->fmt_ad, sizeof(*buf));
    }
}

static inline void
bcm56880_assoc_data_format_decode(int u, bcm56880_assoc_data_t *buf, uint8_t app,
                                  int full, assoc_data_t *assoc)
{
    uint8_t i;
    ALPM_FLEX_POLICY_DATA_t bufp;
    const bcmptm_alpm_variant_info_t *info;
    const pdd_field_info_t *pdd_info;
    ALPM_ASSERT(sizeof(*buf) <= sizeof(assoc->fmt_ad));

    if (full && (app == APP_LPM)) {
        info = bcmptm_rm_alpm_variant_get(u, ALPM_0);
        pdd_info = info->l3uc_pdd_info;
        sal_memcpy(&bufp, &buf->full, sizeof(bufp));

        /* using full format to decode */
        for (i = 0; i < info->l3uc_pdd_info_count; i++) {
            assoc->fmt_ad[i] = ALPM_FLEX_POLICY_DATA_GET(bufp,
                pdd_info->start_bit, pdd_info->end_bit);
            pdd_info++;
        }
    } else {
        sal_memcpy(&assoc->fmt_ad, buf, sizeof(*buf));
    }
}

static inline void
bcm56880_fixed_data_format_decode(int u, void *buf, alpm_arg_t *arg)
{
    uint32_t data_type;
    FIXED_DATA_t *fd_format = buf;

    /* strength profile index fixed per-LT */
    FMT_FIXED_DATA_FIELD_GET(SBR_PROFILE_INDEXf, arg->profile_index);
    FMT_FIXED_DATA_FIELD_GET(DATA_TYPEf, data_type);
    if (arg->decode_lt_view) {
        ALPM_ASSERT(data_type == 0 || data_type == 1);
        arg->ad.user_data_type = ((data_type & 0x1) == wide_mode[u])? DATA_FULL:
                                 DATA_REDUCED;
    }
    /*
     * else {
     *     arg->data_type =
     *         (data_type & 0x1) == data_types[1 << PDD_FULL_BIT] ?
     *                              DATA_FULL : DATA_REDUCED;
     * }
     */
}

static void
bcm56880_sbr_profile_index_get(int u, alpm_arg_t *arg)
{
    int ipv6 = arg->key.t.ipv;
    uint8_t app = arg->key.t.app;
    alpm_vrf_type_t vt = arg->key.vt;
    uint8_t sbr;
    const alpm_config_t *config = bcmptm_rm_alpm_config(u, ALPM_0);

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
bcm56880_fixed_data_format_encode(int u, void *buf, uint32_t sub_db_prio, alpm_arg_t *arg)
{
    bool is_src;
    int index;
    const bcmptm_alpm_variant_info_t *info = bcmptm_rm_alpm_variant_get(u, ALPM_0);
    const uint8_t *data_types = info->pdd_data_types_array;
    FIXED_DATA_t *fd_format = buf;

    FIXED_DATA_CLR(*fd_format);
    FMT_FIXED_DATA_FIELD_SET(SUB_DB_PRIORITYf, sub_db_prio);
    /* Get arg->profile_index from arg->key */
    bcm56880_sbr_profile_index_get(u, arg);
    FMT_FIXED_DATA_FIELD_SET(SBR_PROFILE_INDEXf, arg->profile_index);

    if (arg->encode_lt_view) {
        FMT_FIXED_DATA_FIELD_SET(DATA_TYPEf,
            (arg->ad.user_data_type == DATA_FULL)? wide_mode[u]: !wide_mode[u]);
    } else {
        is_src = bcmptm_rm_alpm_db_is_src(u, ALPM_0, arg->db);

        /*
         * The PDD will always be programmed in pairs for LPM. For example IPV4
         * even/odd pair at PDD index 0 and 1, IPV6 even/odd pair at PDD index
         * 2, 3.
         * Even is for full policy data set (46b) while odd is for reduced
         * policy
         * data set (16b).
         */
        /*
         * Format for pdd_data_types_array subscript:
         * Bit 3: 0=FP_COMP, 1=LPM
         * Bit 2: 0=IPV4, 1= IPV6
         * Bit 1: 0=DST, 1=SRC
         * Bit 0: 0=REDUCED, 1=FULL (regardless of LT field definition)
         *
         * This format is assumed to hold true for any NPL variant,
         * if anything has to be changed in a particular NPL variant,
         * it should be values saved in the array changed accordingly,
         * not the subscript format.
         *
         * Below is the snapshot of data_type values defined in DNA_4_4_1.
         * Value - APP   V6   SRC   FULL
         * -----------------------------
         * 1     - 0     0    0     0
         * 0     - 0     0    0     1
         * 3     - 0     0    1     0
         * 2     - 0     0    1     1
         * 5     - 0     1    0     0
         * 4     - 0     1    0     1
         * 7     - 0     1    1     0
         * 6     - 0     1    1     1
         * 9     - 1     0    0     0
         * 8     - 1     0    0     1
         * 11    - 1     0    1     0
         * 10    - 1     0    1     1
         * 13    - 1     1    0     0
         * 12    - 1     1    0     1
         * 15    - 1     1    1     0
         * 14    - 1     1    1     1
         */
        /* Always set FULL for DATA_TYPE[0] for LPM. FP_COMP0 should be REDUCED */
        index = (arg->key.t.app == APP_COMP0) << PDD_APP_BIT |
                !!arg->key.t.ipv << PDD_IPV_BIT |
                is_src << PDD_SRC_BIT |
                (arg->key.t.app == APP_COMP0 ?
                 (arg->ad.user_data_type == DATA_FULL) : true) << PDD_FULL_BIT;

        FMT_FIXED_DATA_FIELD_SET(DATA_TYPEf, *(data_types+index));
    }
}

static void
bcm56880_ln_alpm2_data_format_encode(int u, ALPM2_DATA_t *a2_format,
                                     alpm2_data_t *a2_data)
{
    ALPM2_DATA_CLR(*a2_format);
    FMT_ALPM2_DATA_FIELD_SET(BKT_PTRf, a2_data->phy_bkt);
    FMT_ALPM2_DATA_FIELD_SET(DEFAULT_MISSf, a2_data->default_miss);
    FMT_ALPM2_DATA_FIELD_SET(KSHIFTf, a2_data->kshift);
    FMT_ALPM2_DATA_FIELD_SET(ROFSf, a2_data->rofs);
    FMT_ALPM2_DATA_FIELD_SET(FMT0f, bcm56880_l2p_route_fmts[a2_data->fmt[0]]);
    FMT_ALPM2_DATA_FIELD_SET(FMT1f, bcm56880_l2p_route_fmts[a2_data->fmt[1]]);
    FMT_ALPM2_DATA_FIELD_SET(FMT2f, bcm56880_l2p_route_fmts[a2_data->fmt[2]]);
    FMT_ALPM2_DATA_FIELD_SET(FMT3f, bcm56880_l2p_route_fmts[a2_data->fmt[3]]);
    FMT_ALPM2_DATA_FIELD_SET(FMT4f, bcm56880_l2p_route_fmts[a2_data->fmt[4]]);
    FMT_ALPM2_DATA_FIELD_SET(FMT5f, bcm56880_l2p_route_fmts[a2_data->fmt[5]]);
    FMT_ALPM2_DATA_FIELD_SET(FMT6f, bcm56880_l2p_route_fmts[a2_data->fmt[6]]);
    FMT_ALPM2_DATA_FIELD_SET(FMT7f, bcm56880_l2p_route_fmts[a2_data->fmt[7]]);

}


static void
bcm56880_ln_alpm2_data_format_decode(int u, ALPM2_DATA_t *a2_format,
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

    for (i = 0; i < ALPM2_DATA_FORMATS; i++) {
        a2_data->fmt[i] = bcm56880_p2l_route_fmts[a2_data->fmt[i]];
    }
}

static void
bcm56880_ln_alpm1_data_format_encode(int u, void *buf, alpm1_data_t *a1_data)
{
    ALPM1_DATA_t *a1_format = buf;

    ALPM1_DATA_CLR(*a1_format);
    FMT_ALPM1_DATA_FIELD_SET(BKT_PTRf, a1_data->phy_bkt);
    FMT_ALPM1_DATA_FIELD_SET(DEFAULT_MISSf, a1_data->default_miss);
    FMT_ALPM1_DATA_FIELD_SET(DIRECT_ROUTEf, a1_data->direct_route);
    FMT_ALPM1_DATA_FIELD_SET(KSHIFTf, a1_data->kshift);
    FMT_ALPM1_DATA_FIELD_SET(ROFSf, a1_data->rofs);
    if (a1_data->is_route) {
        FMT_ALPM1_DATA_FIELD_SET(FMT0f, bcm56880_l2p_route_fmts[a1_data->fmt[0]]);
        FMT_ALPM1_DATA_FIELD_SET(FMT1f, bcm56880_l2p_route_fmts[a1_data->fmt[1]]);
        FMT_ALPM1_DATA_FIELD_SET(FMT2f, bcm56880_l2p_route_fmts[a1_data->fmt[2]]);
        FMT_ALPM1_DATA_FIELD_SET(FMT3f, bcm56880_l2p_route_fmts[a1_data->fmt[3]]);
        FMT_ALPM1_DATA_FIELD_SET(FMT4f, bcm56880_l2p_route_fmts[a1_data->fmt[4]]);
        FMT_ALPM1_DATA_FIELD_SET(FMT5f, bcm56880_l2p_route_fmts[a1_data->fmt[5]]);
    } else {
        FMT_ALPM1_DATA_FIELD_SET(FMT0f, a1_data->fmt[0]);
        FMT_ALPM1_DATA_FIELD_SET(FMT1f, a1_data->fmt[1]);
        FMT_ALPM1_DATA_FIELD_SET(FMT2f, a1_data->fmt[2]);
        FMT_ALPM1_DATA_FIELD_SET(FMT3f, a1_data->fmt[3]);
        FMT_ALPM1_DATA_FIELD_SET(FMT4f, a1_data->fmt[4]);
        FMT_ALPM1_DATA_FIELD_SET(FMT5f, a1_data->fmt[5]);
    }
}

static void
bcm56880_ln_alpm1_data_format_decode(int u, void *buf, alpm1_data_t *a1_data)
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

    if (a1_data->is_route) {
        for (i = 0; i < ALPM1_DATA_FORMATS; i++) {
            a1_data->fmt[i] = bcm56880_p2l_route_fmts[a1_data->fmt[i]];
        }
    }
}

static void
bcm56880_ln_pivot_base_entry_info(int u, uint8_t format_value,
                                  int *prefix_len,
                                  int *full,
                                  int *number)
{
    ALPM_ASSERT(format_value < COUNTOF(bcm56880_pivot_base_entry_info));
    *prefix_len = bcm56880_pivot_base_entry_info[format_value].prefix_len;
    *full = bcm56880_pivot_base_entry_info[format_value].full;
    *number = bcm56880_pivot_base_entry_info[format_value].num_entries;

    (*prefix_len)--; /* Exclude the extra bit */
}

static void
bcm56880_ln_pivot_format_full_encode2(int u, uint8_t format_value,
                                      uint32_t *base_entry,
                                      int ent_ofs,
                                      void *buf)
{
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
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56880_ln_pivot_format_full_encode(int u, uint8_t format_value,
                                     ln_base_entry_t *base_entry,
                                     int entry_count,
                                     void *buf)
{
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
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56880_ln_pivot_format_full_decode2(int u, uint8_t format_value,
                                      uint32_t *base_entry,
                                      int ent_ofs,
                                      void *buf)
{
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
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56880_ln_pivot_format_full_decode(int u, uint8_t format_value,
                                     ln_base_entry_t *base_entry,
                                     int entry_count,
                                     void *buf)
{
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
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56880_ln_pivot_format_set_data(uint8_t format_value,
                                  uint32_t *ad_format,
                                  void *buf)
{
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
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56880_ln_pivot_format_set(uint8_t format_value,
                             uint32_t *ad_format,
                             uint32_t *a2_format,
                             alpm_format_ip_t prefix,
                             void *buf)
{
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
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56880_ln_pivot_format_encode(int u, uint8_t format_value,
                                uint8_t kshift,
                                alpm_arg_t *arg,
                                void *buf)
{
    int full = 0, number;
    int max_len = 0;
    bcm56880_assoc_data_t ad_format;
    bcm56880_ln_pivot_base_entry_info(u, format_value, &max_len, &full, &number);
    bcm56880_assoc_data_format_encode(u, &ad_format, arg->key.t.app, full, &arg->ad);

    if (arg->encode_data_only) {
        bcm56880_ln_pivot_format_set_data(format_value,
                                          &ad_format.v[0],
                                          buf);
    } else {
        alpm_format_ip_t prefix;
        ALPM2_DATA_t a2_format;
        bcm56880_ln_alpm2_data_format_encode(u, &a2_format, &arg->a2);
        bcmptm_rm_alpm_one_bit_encode(arg->key.t.max_bits,
                                      &arg->key.t.trie_ip,
                                      arg->key.t.len,
                                      max_len,
                                      kshift,
                                      &prefix[0]);
        bcm56880_ln_pivot_format_set(format_value,
                                     &ad_format.v[0],
                                     &a2_format.v[0],
                                     &prefix[0],
                                     buf);
    }
}

static void
bcm56880_ln_pivot_format_get_data(uint8_t format_value,
                                  uint32_t *ad_format,
                                  void *buf)
{
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
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56880_ln_pivot_format_get(uint8_t format_value,
                             uint32_t *ad_format,
                             uint32_t *a2_format,
                             alpm_format_ip_t prefix,
                             void *buf)
{
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
    default:
        ALPM_ASSERT(0);
        break;
    }
}


static void
bcm56880_ln_pivot_format_decode(int u, uint8_t format_value,
                                uint8_t kshift,
                                alpm_arg_t *arg,
                                void *buf)
{
    int full, number;
    int max_len;
    ALPM2_DATA_t a2_format;
    bcm56880_assoc_data_t ad_format = {{{0}}};
    alpm_format_ip_t prefix = {0};
    bcm56880_ln_pivot_base_entry_info(u, format_value, &max_len, &full,
                                      &number);
    if (arg->decode_data_only) {
        bcm56880_ln_pivot_format_get_data(format_value,
                                          &ad_format.v[0],
                                          buf);
    } else {
        bcm56880_ln_pivot_format_get(format_value,
                                     &ad_format.v[0],
                                     &a2_format.v[0],
                                     &prefix[0],
                                     buf);
        bcm56880_ln_alpm2_data_format_decode(u, &a2_format, &arg->a2);
        arg->valid = bcmptm_rm_alpm_one_bit_decode(arg->key.t.max_bits,
                                                   &arg->key.t.trie_ip,
                                                   &arg->key.t.len,
                                                   max_len,
                                                   kshift,
                                                   &prefix[0]);
    }
    bcm56880_assoc_data_format_decode(u, &ad_format, arg->key.t.app, full, &arg->ad);
}

static void
bcm56880_ln_route_base_entry_info(int u, uint8_t format_value,
                                  int *prefix_len,
                                  int *full,
                                  int *number)
{
    ALPM_ASSERT(format_value < COUNTOF(bcm56880_route_base_entry_info));
    *prefix_len = bcm56880_route_base_entry_info[format_value].prefix_len;
    *full = bcm56880_route_base_entry_info[format_value].full;
    *number = bcm56880_route_base_entry_info[format_value].num_entries;

    (*prefix_len)--; /* Exclude the extra bit */
}


static void
bcm56880_ln_route_format_full_encode2(int u, uint8_t format_value,
                                      uint32_t *base_entry,
                                      int ent_ofs,
                                      void *buf)
{
    switch (format_value) {
    case 2: {
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
    case 3: {
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
    case 4: {
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
    case 5: {
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
    case 6: {
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
    case 7: {
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
    case 8: {
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
    case 9: {
        L3_DEFIP_ALPM_ROUTE_FMT8_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(8, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET2(8, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_SET2(8, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_SET2(8, ENTRY_3f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 10: {
        L3_DEFIP_ALPM_ROUTE_FMT9_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(9, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET2(9, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_SET2(9, ENTRY_2f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 11: {
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
    case 12: {
        L3_DEFIP_ALPM_ROUTE_FMT11_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(11, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET2(11, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_SET2(11, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_SET2(11, ENTRY_3f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 13: {
        L3_DEFIP_ALPM_ROUTE_FMT12_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_SET2(12, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_SET2(12, ENTRY_1f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 1: {
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
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56880_ln_route_format_full_encode(int u, uint8_t format_value,
                                     ln_base_entry_t *base_entry,
                                     int entry_count,
                                     void *buf)
{
    switch (format_value) {
    case 2: {
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
    case 3: {
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
    case 4: {
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
    case 5: {
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
    case 6: {
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
    case 7: {
        L3_DEFIP_ALPM_ROUTE_FMT6_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(6, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(6, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(6, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_SET2(6, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_SET2(6, ENTRY_4f, base_entry[4]);
        ROUTE_FMT_FULL_FIELD_SET2(6, ENTRY_5f, base_entry[5]);
        ALPM_ASSERT(entry_count >= 5);
    } break;
    case 8: {
        L3_DEFIP_ALPM_ROUTE_FMT7_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(7, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(7, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(7, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_SET2(7, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_SET2(7, ENTRY_4f, base_entry[4]);
        ALPM_ASSERT(entry_count >= 4);
    } break;
    case 9: {
        L3_DEFIP_ALPM_ROUTE_FMT8_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(8, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(8, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(8, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_SET2(8, ENTRY_3f, base_entry[3]);
        ALPM_ASSERT(entry_count >= 3);
    } break;
    case 10: {
        L3_DEFIP_ALPM_ROUTE_FMT9_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(9, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(9, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(9, ENTRY_2f, base_entry[2]);
        ALPM_ASSERT(entry_count >= 2);
    } break;
    case 11: {
        L3_DEFIP_ALPM_ROUTE_FMT10_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(10, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(10, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(10, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_SET2(10, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_SET2(10, ENTRY_4f, base_entry[4]);
        ROUTE_FMT_FULL_FIELD_SET2(10, ENTRY_5f, base_entry[5]);
        ALPM_ASSERT(entry_count >= 5);
    } break;
    case 12: {
        L3_DEFIP_ALPM_ROUTE_FMT11_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(11, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(11, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(11, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_SET2(11, ENTRY_3f, base_entry[3]);
        ALPM_ASSERT(entry_count >= 3);
    } break;
    case 13: {
        L3_DEFIP_ALPM_ROUTE_FMT12_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(12, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(12, ENTRY_1f, base_entry[1]);
        ALPM_ASSERT(entry_count >= 1);
    } break;
    case 1: {
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
    default:
        ALPM_ASSERT(0);
        break;
    }
}


static void
bcm56880_ln_route_format_full_decode2(int u, uint8_t format_value,
                                      uint32_t *base_entry,
                                      int ent_ofs,
                                      void *buf)
{
    switch (format_value) {
    case 2: {
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
    case 3: {
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
    case 4: {
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
    case 5: {
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
    case 6: {
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
    case 7: {
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
    case 8: {
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
    case 9: {
        L3_DEFIP_ALPM_ROUTE_FMT8_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(8, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET2(8, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_GET2(8, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_GET2(8, ENTRY_3f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 10: {
        L3_DEFIP_ALPM_ROUTE_FMT9_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(9, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET2(9, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_GET2(9, ENTRY_2f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 11: {
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
    case 12: {
        L3_DEFIP_ALPM_ROUTE_FMT11_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(11, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET2(11, ENTRY_1f, base_entry); break;
        case 2: ROUTE_FMT_FULL_FIELD_GET2(11, ENTRY_2f, base_entry); break;
        case 3: ROUTE_FMT_FULL_FIELD_GET2(11, ENTRY_3f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 13: {
        L3_DEFIP_ALPM_ROUTE_FMT12_FULL_t *bucket = buf;
        switch (ent_ofs) {
        case 0: ROUTE_FMT_FULL_FIELD_GET2(12, ENTRY_0f, base_entry); break;
        case 1: ROUTE_FMT_FULL_FIELD_GET2(12, ENTRY_1f, base_entry); break;
        default: ALPM_ASSERT(0); break;
        }
    } break;
    case 1: {
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
    default:
        ALPM_ASSERT(0);
        break;
    }
}



static void
bcm56880_ln_route_format_full_decode(int u, uint8_t format_value,
                                     ln_base_entry_t *base_entry,
                                     int entry_count,
                                     void *buf)
{
    switch (format_value) {
    case 2: {
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
    case 3: {
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
    case 4: {
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
    case 5: {
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
    case 6: {
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
    case 7: {
        L3_DEFIP_ALPM_ROUTE_FMT6_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(6, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(6, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(6, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_GET2(6, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_GET2(6, ENTRY_4f, base_entry[4]);
        ROUTE_FMT_FULL_FIELD_GET2(6, ENTRY_5f, base_entry[5]);
        ALPM_ASSERT(entry_count >= 5);
    } break;
    case 8: {
        L3_DEFIP_ALPM_ROUTE_FMT7_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(7, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(7, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(7, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_GET2(7, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_GET2(7, ENTRY_4f, base_entry[4]);
        ALPM_ASSERT(entry_count >= 4);
    } break;
    case 9: {
        L3_DEFIP_ALPM_ROUTE_FMT8_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(8, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(8, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(8, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_GET2(8, ENTRY_3f, base_entry[3]);
        ALPM_ASSERT(entry_count >= 3);
    } break;
    case 10: {
        L3_DEFIP_ALPM_ROUTE_FMT9_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(9, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(9, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(9, ENTRY_2f, base_entry[2]);
        ALPM_ASSERT(entry_count >= 2);
    } break;
    case 11: {
        L3_DEFIP_ALPM_ROUTE_FMT10_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(10, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(10, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(10, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_GET2(10, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_GET2(10, ENTRY_4f, base_entry[4]);
        ROUTE_FMT_FULL_FIELD_GET2(10, ENTRY_5f, base_entry[5]);
        ALPM_ASSERT(entry_count >= 5);
    } break;
    case 12: {
        L3_DEFIP_ALPM_ROUTE_FMT11_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(11, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(11, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(11, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_GET2(11, ENTRY_3f, base_entry[3]);
        ALPM_ASSERT(entry_count >= 3);
    } break;
    case 13: {
        L3_DEFIP_ALPM_ROUTE_FMT12_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(12, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(12, ENTRY_1f, base_entry[1]);
        ALPM_ASSERT(entry_count >= 1);
    } break;
    case 1: {
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
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static int
bcm56880_ln_base_entry_info(int u, int m,
                            format_type_t format_type,
                            uint8_t format_value,
                            int *prefix_len,
                            int *full,
                            int *num_entry)
{
    int a, b, c;
    if (format_type == FORMAT_ROUTE) {
        bcm56880_ln_route_base_entry_info(u, format_value, &a, &b, &c);
    } else {
        bcm56880_ln_pivot_base_entry_info(u, format_value, &a, &b, &c);
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
bcm56880_ln_route_format_set_data(uint8_t format_value,
                                  uint32_t *ad_format,
                                  void *buf)
{
    switch (format_value) {
    case 2: {
        L3_DEFIP_ALPM_ROUTE_FMT1_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(1, ASSOC_DATAf, ad_format);
    } break;
    case 3: {
        L3_DEFIP_ALPM_ROUTE_FMT2_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(2, ASSOC_DATAf, ad_format);
    } break;
    case 4: {
        L3_DEFIP_ALPM_ROUTE_FMT3_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(3, ASSOC_DATAf, ad_format);
    } break;
    case 5: {
        L3_DEFIP_ALPM_ROUTE_FMT4_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(4, ASSOC_DATAf, ad_format);
    } break;
    case 6: {
        L3_DEFIP_ALPM_ROUTE_FMT5_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(5, ASSOC_DATAf, ad_format);
    } break;
    case 7: {
        L3_DEFIP_ALPM_ROUTE_FMT6_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(6, ASSOC_DATAf, ad_format);
    } break;
    case 8: {
        L3_DEFIP_ALPM_ROUTE_FMT7_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(7, ASSOC_DATAf, ad_format);
    } break;
    case 9: {
        L3_DEFIP_ALPM_ROUTE_FMT8_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(8, ASSOC_DATAf, ad_format);
    } break;
    case 10: {
        L3_DEFIP_ALPM_ROUTE_FMT9_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(9, ASSOC_DATAf, ad_format);
    } break;
    case 11: {
        L3_DEFIP_ALPM_ROUTE_FMT10_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(10, ASSOC_DATAf, ad_format);
    } break;
    case 12: {
        L3_DEFIP_ALPM_ROUTE_FMT11_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(11, ASSOC_DATAf, ad_format);
    } break;
    case 13: {
        L3_DEFIP_ALPM_ROUTE_FMT12_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(12, ASSOC_DATAf, ad_format);
    } break;
    case 1: {
        L3_DEFIP_ALPM_ROUTE_FMT13_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(13, ASSOC_DATAf, ad_format);
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
}
static void
bcm56880_ln_route_format_set(uint8_t format_value,
                             uint32_t *ad_format,
                             alpm_format_ip_t prefix,
                             void *buf)
{
    switch (format_value) {
    case 2: {
        L3_DEFIP_ALPM_ROUTE_FMT1_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(1, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET(1, ASSOC_DATAf, ad_format);
    } break;
    case 3: {
        L3_DEFIP_ALPM_ROUTE_FMT2_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(2, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET(2, ASSOC_DATAf, ad_format);
    } break;
    case 4: {
        L3_DEFIP_ALPM_ROUTE_FMT3_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(3, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET(3, ASSOC_DATAf, ad_format);
    } break;
    case 5: {
        L3_DEFIP_ALPM_ROUTE_FMT4_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(4, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET(4, ASSOC_DATAf, ad_format);
    } break;
    case 6: {
        L3_DEFIP_ALPM_ROUTE_FMT5_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(5, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET(5, ASSOC_DATAf, ad_format);
    } break;
    case 7: {
        L3_DEFIP_ALPM_ROUTE_FMT6_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(6, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET(6, ASSOC_DATAf, ad_format);
    } break;
    case 8: {
        L3_DEFIP_ALPM_ROUTE_FMT7_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(7, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET(7, ASSOC_DATAf, ad_format);
    } break;
    case 9: {
        L3_DEFIP_ALPM_ROUTE_FMT8_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(8, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET(8, ASSOC_DATAf, ad_format);
    } break;
    case 10: {
        L3_DEFIP_ALPM_ROUTE_FMT9_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(9, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET(9, ASSOC_DATAf, ad_format);
    } break;
    case 11: {
        L3_DEFIP_ALPM_ROUTE_FMT10_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(10, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET2(10, ASSOC_DATAf, ad_format);
    } break;
    case 12: {
        L3_DEFIP_ALPM_ROUTE_FMT11_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(11, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET2(11, ASSOC_DATAf, ad_format);
    } break;
    case 13: {
        L3_DEFIP_ALPM_ROUTE_FMT12_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET2(12, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET2(12, ASSOC_DATAf, ad_format);
    } break;
    case 1: {
        L3_DEFIP_ALPM_ROUTE_FMT13_t *format_buf = buf;
        ROUTE_FMT_FIELD_SET(13, PREFIXf, prefix);
        ROUTE_FMT_FIELD_SET(13, ASSOC_DATAf, ad_format);
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56880_ln_route_format_encode(int u, uint8_t format_value,
                                uint8_t kshift,
                                alpm_arg_t *arg,
                                void *buf)
{
    int full;
    int max_len;
    int number;
    bcm56880_assoc_data_t ad_format;
    bcm56880_ln_route_base_entry_info(u, format_value, &max_len, &full, &number);
    bcm56880_assoc_data_format_encode(u, &ad_format, arg->key.t.app, full, &(arg->ad));
    if (arg->encode_data_only) {
        bcm56880_ln_route_format_set_data(format_value,
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
        bcm56880_ln_route_format_set(format_value,
                                     &ad_format.v[0],
                                     &prefix[0],
                                     buf);
    }
}

static void
bcm56880_ln_route_format_get_data(uint8_t format_value,
                                  uint32_t *ad_format,
                                  void *buf)
{
    switch (format_value) {
    case 2: {
        L3_DEFIP_ALPM_ROUTE_FMT1_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(1, ASSOC_DATAf, ad_format);
    } break;
    case 3: {
        L3_DEFIP_ALPM_ROUTE_FMT2_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(2, ASSOC_DATAf, ad_format);
    } break;
    case 4: {
        L3_DEFIP_ALPM_ROUTE_FMT3_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(3, ASSOC_DATAf, ad_format);
    } break;
    case 5: {
        L3_DEFIP_ALPM_ROUTE_FMT4_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(4, ASSOC_DATAf, ad_format);
    } break;
    case 6: {
        L3_DEFIP_ALPM_ROUTE_FMT5_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(5, ASSOC_DATAf, ad_format);
    } break;
    case 7: {
        L3_DEFIP_ALPM_ROUTE_FMT6_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(6, ASSOC_DATAf, ad_format);
    } break;
    case 8: {
        L3_DEFIP_ALPM_ROUTE_FMT7_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(7, ASSOC_DATAf, ad_format);
    } break;
    case 9: {
        L3_DEFIP_ALPM_ROUTE_FMT8_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(8, ASSOC_DATAf, ad_format);
    } break;
    case 10: {
        L3_DEFIP_ALPM_ROUTE_FMT9_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(9, ASSOC_DATAf, ad_format);
    } break;
    case 11: {
        L3_DEFIP_ALPM_ROUTE_FMT10_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(10, ASSOC_DATAf, ad_format);
    } break;
    case 12: {
        L3_DEFIP_ALPM_ROUTE_FMT11_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(11, ASSOC_DATAf, ad_format);
    } break;
    case 13: {
        L3_DEFIP_ALPM_ROUTE_FMT12_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(12, ASSOC_DATAf, ad_format);
    } break;
    case 1: {
        L3_DEFIP_ALPM_ROUTE_FMT13_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(13, ASSOC_DATAf, ad_format);
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56880_ln_route_format_get(uint8_t format_value,
                             uint32_t *ad_format,
                             alpm_format_ip_t prefix,
                             void *buf)
{
    switch (format_value) {
    case 2: {
        L3_DEFIP_ALPM_ROUTE_FMT1_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(1, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET(1, ASSOC_DATAf, ad_format);
    } break;
    case 3: {
        L3_DEFIP_ALPM_ROUTE_FMT2_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(2, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET(2, ASSOC_DATAf, ad_format);
    } break;
    case 4: {
        L3_DEFIP_ALPM_ROUTE_FMT3_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(3, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET(3, ASSOC_DATAf, ad_format);
    } break;
    case 5: {
        L3_DEFIP_ALPM_ROUTE_FMT4_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(4, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET(4, ASSOC_DATAf, ad_format);
    } break;
    case 6: {
        L3_DEFIP_ALPM_ROUTE_FMT5_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(5, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET(5, ASSOC_DATAf, ad_format);
    } break;
    case 7: {
        L3_DEFIP_ALPM_ROUTE_FMT6_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(6, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET(6, ASSOC_DATAf, ad_format);
    } break;
    case 8: {
        L3_DEFIP_ALPM_ROUTE_FMT7_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(7, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET(7, ASSOC_DATAf, ad_format);
    } break;
    case 9: {
        L3_DEFIP_ALPM_ROUTE_FMT8_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(8, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET(8, ASSOC_DATAf, ad_format);
    } break;
    case 10: {
        L3_DEFIP_ALPM_ROUTE_FMT9_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(9, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET(9, ASSOC_DATAf, ad_format);
    } break;
    case 11: {
        L3_DEFIP_ALPM_ROUTE_FMT10_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(10, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET2(10, ASSOC_DATAf, ad_format);
    } break;
    case 12: {
        L3_DEFIP_ALPM_ROUTE_FMT11_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(11, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET2(11, ASSOC_DATAf, ad_format);
    } break;
    case 13: {
        L3_DEFIP_ALPM_ROUTE_FMT12_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET2(12, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET2(12, ASSOC_DATAf, ad_format);
    } break;
    case 1: {
        L3_DEFIP_ALPM_ROUTE_FMT13_t *format_buf = buf;
        ROUTE_FMT_FIELD_GET(13, PREFIXf, prefix);
        ROUTE_FMT_FIELD_GET(13, ASSOC_DATAf, ad_format);
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56880_ln_route_format_decode(int u, uint8_t format_value,
                                uint8_t kshift,
                                alpm_arg_t *arg,
                                void *buf)
{
    int full;
    int max_len;
    int number;
    bcm56880_assoc_data_t ad_format = {{{0}}};
    alpm_format_ip_t prefix = {0};
    bcm56880_ln_route_base_entry_info(u, format_value, &max_len, &full,
                                      &number);
    if (arg->decode_data_only) {
        bcm56880_ln_route_format_get_data(format_value,
                                          &ad_format.v[0],
                                          buf);
    } else {
        bcm56880_ln_route_format_get(format_value,
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
    bcm56880_assoc_data_format_decode(u, &ad_format, arg->key.t.app, full, &arg->ad);
}

static int
bcm56880_ln_raw_bucket_encode(int u, int m, format_type_t format_type,
                              uint8_t format_value,
                              ln_base_entry_t *base_entry,
                              int entry_count,
                              void *bucket)
{
    if (format_type == FORMAT_ROUTE) {
        bcm56880_ln_route_format_full_encode(u, format_value, base_entry,
                                             entry_count, bucket);
    } else {
        bcm56880_ln_pivot_format_full_encode(u, format_value, base_entry,
                                             entry_count, bucket);
    }
    return SHR_E_NONE;
}


static int
bcm56880_ln_raw_bucket_entry_set(int u, int m, format_type_t format_type,
                                 uint8_t format_value,
                                 uint32_t *base_entry,
                                 int ent_ofs,
                                 void *bucket)
{
    if (format_type == FORMAT_ROUTE) {
        bcm56880_ln_route_format_full_encode2(u, format_value, base_entry,
                                              ent_ofs, bucket);
    } else {
        bcm56880_ln_pivot_format_full_encode2(u, format_value, base_entry,
                                              ent_ofs, bucket);
    }
    return SHR_E_NONE;
}


static int
bcm56880_ln_raw_bucket_decode(int u, int m, format_type_t format_type,
                              uint8_t format_value,
                              ln_base_entry_t *base_entry,
                              int entry_count,
                              void *bucket)
{
    if (format_type == FORMAT_ROUTE) {
        bcm56880_ln_route_format_full_decode(u, format_value, base_entry,
                                             entry_count, bucket);
    } else {
        bcm56880_ln_pivot_format_full_decode(u, format_value, base_entry,
                                             entry_count, bucket);
    }
    return SHR_E_NONE;
}


static int
bcm56880_ln_raw_bucket_entry_get(int u, int m, format_type_t format_type,
                                 uint8_t format_value,
                                 uint32_t *base_entry,
                                 int ent_ofs,
                                 void *bucket)
{
    if (format_type == FORMAT_ROUTE) {
        bcm56880_ln_route_format_full_decode2(u, format_value, base_entry,
                                              ent_ofs, bucket);
    } else {
        bcm56880_ln_pivot_format_full_decode2(u, format_value, base_entry,
                                              ent_ofs, bucket);
    }
    return SHR_E_NONE;
}


static int
bcm56880_ln_base_entry_encode(int u, int m, format_type_t format_type,
                              uint8_t format_value,
                              uint8_t kshift,
                              alpm_arg_t *arg,
                              uint32_t *entry)
{
    if (format_type == FORMAT_ROUTE) {
        bcm56880_ln_route_format_encode(u, format_value, kshift, arg, entry);
    } else {
        bcm56880_ln_pivot_format_encode(u, format_value, kshift, arg, entry);
    }
    return SHR_E_NONE;
}

static int
bcm56880_ln_base_entry_decode(int u, int m, format_type_t format_type,
                              uint8_t format_value,
                              uint8_t kshift,
                              alpm_arg_t *arg,
                              uint32_t *entry)
{
    if (format_type == FORMAT_ROUTE) {
        bcm56880_ln_route_format_decode(u, format_value, kshift, arg, entry);
    } else {
        bcm56880_ln_pivot_format_decode(u, format_value, kshift, arg, entry);
    }
    return SHR_E_NONE;
}

static int
bcm56880_ln_base_entry_move(int u, int m, format_type_t format_type,
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
        bcm56880_ln_raw_bucket_entry_get(u, m, format_type,
                                         src_format_value,
                                         &src_base_entry[0],
                                         src_ent_ofs,
                                         src_bucket));

    if (src_format_value == dst_format_value &&
        src_kshift == dst_kshift) {
#ifdef ALPM_DEBUG
        if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
            arg = shr_lmm_alloc(bcmptm_rm_alpm_arg_lmem[u][ALPM_0]);
            if (arg == NULL) {
                SHR_ERR_EXIT(SHR_E_MEMORY);
            }
            sal_memset(arg, 0, sizeof(*arg));

            arg->key.t.max_bits = src_t->max_bits;
            arg->key.t.ipv = src_t->ipv;
            sal_memcpy(&arg->key.t.trie_ip, &src_t->trie_ip,
                       sizeof(src_t->trie_ip));
            SHR_IF_ERR_EXIT(
                bcm56880_ln_base_entry_decode(u, m, format_type,
                                              src_format_value,
                                              src_kshift,
                                              arg,
                                              &src_base_entry[0]));
            ALPM_ASSERT(arg->key.t.len >= src_kshift);
            SHR_IF_ERR_EXIT(
                bcm56880_ln_base_entry_encode(u, m, format_type,
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
        arg = shr_lmm_alloc(bcmptm_rm_alpm_arg_lmem[u][ALPM_0]);
        if (arg == NULL) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        arg->encode_data_only = 0;
        arg->decode_data_only = 0;
        arg->ad.user_data_type = src_dt;
/*        bcmptm_rm_alpm_arg_init(u, arg);*/

        arg->key.t.max_bits = src_t->max_bits;
        arg->key.t.ipv = src_t->ipv;
        sal_memcpy(&arg->key.t.trie_ip, &src_t->trie_ip, sizeof(src_t->trie_ip));
        SHR_IF_ERR_EXIT(
            bcm56880_ln_base_entry_decode(u, m, format_type,
                                          src_format_value,
                                          src_kshift,
                                          arg,
                                          &src_base_entry[0]));
        ALPM_ASSERT(arg->key.t.len >= src_kshift);
        ALPM_ASSERT(arg->key.t.len >= dst_kshift);
        SHR_IF_ERR_EXIT(
            bcm56880_ln_base_entry_encode(u, m, format_type,
                                          dst_format_value,
                                          dst_kshift,
                                          arg,
                                          &dst_base_entry[0]));
        dst_base = &dst_base_entry[0];
    }
    SHR_IF_ERR_EXIT(
        bcm56880_ln_raw_bucket_entry_set(u, m, format_type,
                                         dst_format_value,
                                         dst_base,
                                         dst_ent_ofs,
                                         dst_bucket));
    SHR_IF_ERR_EXIT(
        bcm56880_ln_raw_bucket_entry_set(u, m, format_type,
                                         src_format_value,
                                         &zero_base_entry[0],
                                         src_ent_ofs,
                                         src_bucket));
exit:
    if (arg) {
        shr_lmm_free(bcmptm_rm_alpm_arg_lmem[u][ALPM_0], arg);
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
bcm56880_l1_key_view_encode_n1(int u, alpm_arg_t *arg, TCAM_VIEW_T *de, int upkm)
{
    L3_DEFIP_TCAM_KEY_t kdata;
    L3_DEFIP_TCAM_KEY_t kmask;
    int vrf, vrf_mask;
    L3_DEFIP_TCAM_LEVEL1_t l1_key;
    L3_DEFIP_TCAM_LEVEL1_t l1_msk;
    int valid;

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
        vrf_mask = ALPM_VRF_MASK;
    }

    if (arg->key.kt == KEY_IPV4) {
        arg->key.ip_addr[0] &= arg->key.ip_mask[0];

        bcm56880_l1_tcam_key_format_encode(u, vrf, HALF_WIDE, IP_VER_4,
                                           arg->key.ip_addr[0], &kdata);
        bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                           arg->key.ip_mask[0], &kmask);
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_KEYf_GET(*de, (uint32_t *)&l1_key);
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_MASKf_GET(*de, (uint32_t *)&l1_msk);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_msk, (uint32_t *)&kmask);
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_KEYf_SET(*de, (uint32_t *)&l1_key);
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_MASKf_SET(*de, (uint32_t *)&l1_msk);
        valid = MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_VALIDf_GET(*de);
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_VALIDf_SET(*de, valid | 0x2);
    } else if (arg->key.kt == KEY_IPV6_SINGLE && (upkm == PKM_S || upkm == PKM_SS)) {
        /* Apply subnet kmask. */
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[0] == 0);
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[1] == 0);
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[2] == 0);
        arg->key.ip_addr[3] &= arg->key.ip_mask[3];

        bcm56880_l1_tcam_key_format_encode(u, vrf, HALF_WIDE, IP_VER_6,
                                           arg->key.ip_addr[3], &kdata);
        bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                           arg->key.ip_mask[3], &kmask);
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_KEYf_GET(*de, (uint32_t *)&l1_key);
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_MASKf_GET(*de, (uint32_t *)&l1_msk);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_msk, (uint32_t *)&kmask);
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_KEYf_SET(*de, (uint32_t *)&l1_key);
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_MASKf_SET(*de, (uint32_t *)&l1_msk);
        valid = MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_VALIDf_GET(*de);
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_VALIDf_SET(*de, valid | 0x2);
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
bcm56880_l1_key_view_encode(int u, alpm_arg_t *arg, TCAM_VIEW_T *de,
                            TCAM_VIEW_T *de_upr, int upkm)
{
    L3_DEFIP_TCAM_KEY_t kdata;
    L3_DEFIP_TCAM_KEY_t kmask;
    int vrf, vrf_mask;
    int valid;
    L3_DEFIP_TCAM_LEVEL1_t l1_key;
    L3_DEFIP_TCAM_LEVEL1_t l1_msk;

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
        vrf_mask = ALPM_VRF_MASK;
    }

    if (arg->key.kt == KEY_IPV4) {
        arg->key.ip_addr[0] &= arg->key.ip_mask[0];

        bcm56880_l1_tcam_key_format_encode(u, vrf, HALF_WIDE, IP_VER_4,
                                           arg->key.ip_addr[0], &kdata);
        bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 1,
                                           arg->key.ip_mask[0], &kmask);
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_KEYf_GET(*de, (uint32_t *)&l1_key);
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_MASKf_GET(*de, (uint32_t *)&l1_msk);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_msk, (uint32_t *)&kmask);
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_KEYf_SET(*de, (uint32_t *)&l1_key);
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_MASKf_SET(*de, (uint32_t *)&l1_msk);
        valid = MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_VALIDf_GET(*de);
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_VALIDf_SET(*de, valid | 0x1);
    } else if (arg->key.kt == KEY_IPV6_SINGLE && (upkm == PKM_S || upkm == PKM_SS)) {
        /* Apply subnet kmask. */
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[0] == 0);
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[1] == 0);
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[2] == 0);
        arg->key.ip_addr[3] &= arg->key.ip_mask[3];

        bcm56880_l1_tcam_key_format_encode(u, vrf, HALF_WIDE, IP_VER_6,
                                           arg->key.ip_addr[3], &kdata);
        bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                    arg->key.ip_mask[3], &kmask);
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_KEYf_GET(*de, (uint32_t *)&l1_key);
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_MASKf_GET(*de, (uint32_t *)&l1_msk);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_msk, (uint32_t *)&kmask);
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_KEYf_SET(*de, (uint32_t *)&l1_key);
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_MASKf_SET(*de, (uint32_t *)&l1_msk);
        valid = MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_VALIDf_GET(*de);
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_VALIDf_SET(*de, valid | 0x1);
    } else if (upkm == PKM_Q || upkm == PKM_SQ) {
        /* Apply subnet kmask. */
        arg->key.ip_addr[0] &= arg->key.ip_mask[0];
        arg->key.ip_addr[1] &= arg->key.ip_mask[1];
        arg->key.ip_addr[2] &= arg->key.ip_mask[2];
        arg->key.ip_addr[3] &= arg->key.ip_mask[3];

        bcm56880_l1_tcam_key_format_encode(u, vrf, DOUBLE_WIDE, IP_VER_6,
                                           arg->key.ip_addr[0], &kdata);
        bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                           arg->key.ip_mask[0],
                                           &kmask);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_msk, (uint32_t *)&kmask);

        bcm56880_l1_tcam_key_format_encode(u, vrf, DOUBLE_WIDE, IP_VER_6,
                                           arg->key.ip_addr[1], &kdata);
        bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                           arg->key.ip_mask[1],
                                           &kmask);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_msk, (uint32_t *)&kmask);


        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_KEYf_SET(*de, (uint32_t *)&l1_key);
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_MASKf_SET(*de, (uint32_t *)&l1_msk);
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_VALIDf_SET(*de, 3);

        bcm56880_l1_tcam_key_format_encode(u, vrf, DOUBLE_WIDE, IP_VER_6,
                                           arg->key.ip_addr[2], &kdata);
        bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                           arg->key.ip_mask[2],
                                           &kmask);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_msk, (uint32_t *)&kmask);

        bcm56880_l1_tcam_key_format_encode(u, vrf, DOUBLE_WIDE, IP_VER_6,
                                           arg->key.ip_addr[3], &kdata);
        bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                           arg->key.ip_mask[3],
                                           &kmask);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_msk, (uint32_t *)&kmask);

        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_KEYf_SET(*de_upr, (uint32_t *)&l1_key);
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_MASKf_SET(*de_upr, (uint32_t *)&l1_msk);
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_VALIDf_SET(*de_upr, 3);

    } else {
        /* IPv6 single wide */
        arg->key.ip_addr[3] &= arg->key.ip_mask[3];
        arg->key.ip_addr[2] &= arg->key.ip_mask[2];
        bcm56880_l1_tcam_key_format_encode(u, vrf, SINGLE_WIDE, IP_VER_6,
                                           arg->key.ip_addr[2], &kdata);
        bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                           arg->key.ip_mask[2], &kmask);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_msk, (uint32_t *)&kmask);

        bcm56880_l1_tcam_key_format_encode(u, vrf, SINGLE_WIDE, IP_VER_6,
                                           arg->key.ip_addr[3], &kdata);
        bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                           arg->key.ip_mask[3], &kmask);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_msk, (uint32_t *)&kmask);

        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_KEYf_SET(*de, (uint32_t *)&l1_key);
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_MASKf_SET(*de, (uint32_t *)&l1_msk);
        MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_VALIDf_SET(*de, 3);
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
bcm56880_l1_fp_key_view_narrow_encode(int u, alpm_arg_t *arg, TCAM_VIEW_T *de)
{
    L3_DEFIP_TCAM_KEY_t kdata;
    L3_DEFIP_TCAM_KEY_t kmask;
    uint32_t op, op_mask;
    int vrf, vrf_mask;
    L3_DEFIP_TCAM_LEVEL1_t l1_key;
    L3_DEFIP_TCAM_LEVEL1_t l1_msk;
    uint16_t op1;         /*!< FP_COMP Opaque1 (L4_PORT) */
    uint16_t op2;         /*!< FP_COMP Opaque2 */

    bool is_src;
    is_src = bcmptm_rm_alpm_db_is_src(u, ALPM_0, arg->db);

    L3_DEFIP_TCAM_KEY_CLR(kdata);
    L3_DEFIP_TCAM_KEY_CLR(kmask);

    vrf = VRF_DEFAULT;
    vrf_mask = 0;

    if (arg->key.t.w_vrf == (ALPM_MAX_FP_OP + 1)) { /* no L4_PORT */
        op1 = 0;
        op2 = 0;
        op_mask = 0;
    } else {
        if (is_src) {
            op1 = 0;
            op2 = arg->key.t.w_vrf; /* SRC L4_PORT coming from */
            op_mask = 0xffff0000;
        } else {
            op1 = arg->key.t.w_vrf; /* DST L4_PORT coming from */
            op2 = 0;
            op_mask = 0x0000ffff;
        }
    }

    op = op2;
    op <<= 16;
    op |= op1;

    /* IPv6 single wide */
    /* ip_addr ~ OP2[15:0], OP1[15:0] */
    bcm56880_l1_tcam_key_format_encode(u, vrf, SINGLE_WIDE, IP_VER_4,
                                       op, &kdata);
    bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                       op_mask, &kmask);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_key, (uint32_t *)&kdata);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_msk, (uint32_t *)&kmask);


    arg->key.ip_addr[0] &= arg->key.ip_mask[0];
    bcm56880_l1_tcam_key_format_encode(u, vrf, SINGLE_WIDE, IP_VER_4,
                                       arg->key.ip_addr[0], &kdata);
    bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                arg->key.ip_mask[0], &kmask);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_key, (uint32_t *)&kdata);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_msk, (uint32_t *)&kmask);

    MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_KEYf_SET(*de, (uint32_t *)&l1_key);
    MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_MASKf_SET(*de, (uint32_t *)&l1_msk);
    MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_VALIDf_SET(*de, 3);

}

/*!
 * \brief Encode key part into double-wide FP_COMP IPv6 entry.
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] de lower FP_COMP IPv6 entry
 * \param [out] de_upr upper FP_COMP IPv6 entry
 *
 * \return SHR_E_XXX.
 */
static void
bcm56880_l1_fp_key_view_wide_encode(int u, alpm_arg_t *arg, TCAM_VIEW_T *de,
                                    TCAM_VIEW_T *de_upr)
{
    L3_DEFIP_TCAM_KEY_t kdata;
    L3_DEFIP_TCAM_KEY_t kmask;
    uint32_t op, op_mask;
    int vrf, vrf_mask;
    L3_DEFIP_TCAM_LEVEL1_t l1_key;
    L3_DEFIP_TCAM_LEVEL1_t l1_msk;
    uint16_t op1;         /*!< FP_COMP Opaque1 (L4_PORT) */
    uint16_t op2;         /*!< FP_COMP Opaque2 */

    bool is_src;
    is_src = bcmptm_rm_alpm_db_is_src(u, ALPM_0, arg->db);

    L3_DEFIP_TCAM_KEY_CLR(kdata);
    L3_DEFIP_TCAM_KEY_CLR(kmask);

    vrf = VRF_DEFAULT;
    vrf_mask = 0;

    if (arg->key.t.w_vrf == (ALPM_MAX_FP_OP + 1)) { /* no L4_PORT */
        op1 = 0;
        op2 = 0;
        op_mask = 0;
    } else {
        if (is_src) {
            op1 = 0;
            op2 = arg->key.t.w_vrf; /* SRC L4_PORT coming from */
            op_mask = 0xffff0000;
        } else {
            op1 = arg->key.t.w_vrf; /* DST L4_PORT coming from */
            op2 = 0;
            op_mask = 0x0000ffff;
        }
    }

    op = op2;
    op <<= 16;
    op |= op1;

    /* Apply subnet kmask. */
    arg->key.ip_addr[0] &= arg->key.ip_mask[0];
    arg->key.ip_addr[1] &= arg->key.ip_mask[1];
    arg->key.ip_addr[2] &= arg->key.ip_mask[2];
    arg->key.ip_addr[3] &= arg->key.ip_mask[3];

    bcm56880_l1_tcam_key_format_encode(u, vrf, DOUBLE_WIDE, IP_VER_6,
                                       arg->key.ip_addr[0], &kdata);
    bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                       arg->key.ip_mask[0],
                                       &kmask);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_key, (uint32_t *)&kdata);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_msk, (uint32_t *)&kmask);

    /* vrf~ OP1[14:0] */
    vrf = op & 0x7fff;
    vrf_mask = op_mask & 0x7fff;
    bcm56880_l1_tcam_key_format_encode(u, vrf, DOUBLE_WIDE, IP_VER_6,
                                       arg->key.ip_addr[1], &kdata);
    bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                       arg->key.ip_mask[1],
                                       &kmask);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_key, (uint32_t *)&kdata);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_msk, (uint32_t *)&kmask);

    MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_KEYf_SET(*de, (uint32_t *)&l1_key);
    MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_MASKf_SET(*de, (uint32_t *)&l1_msk);
    MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_VALIDf_SET(*de, 3);

    /* vrf ~ OP2[13:0], OP1[15] */
    vrf = (op >> 15) & 0x7fff;
    vrf_mask = (op_mask >> 15) & 0x7fff;
    bcm56880_l1_tcam_key_format_encode(u, vrf, DOUBLE_WIDE, IP_VER_6,
                                       arg->key.ip_addr[2], &kdata);
    bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                       arg->key.ip_mask[2],
                                       &kmask);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_key, (uint32_t *)&kdata);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_msk, (uint32_t *)&kmask);

    /* vrf ~ OP2[15:14] */
    vrf = (op >> 30) & 0x0003;
    vrf_mask = (op_mask >> 30) & 0x0003;
    bcm56880_l1_tcam_key_format_encode(u, vrf, DOUBLE_WIDE, IP_VER_6,
                                       arg->key.ip_addr[3], &kdata);
    bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                       arg->key.ip_mask[3],
                                       &kmask);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_key, (uint32_t *)&kdata);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_msk, (uint32_t *)&kmask);

    MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_KEYf_SET(*de_upr, (uint32_t *)&l1_key);
    MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_MASKf_SET(*de_upr, (uint32_t *)&l1_msk);
    MEMDB_TCAM_IFTA80_MEM0_0_ONLYm_VALIDf_SET(*de_upr, 3);

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
bcm56880_l1_data_view_encode(int u, alpm_arg_t *arg, DATA_VIEW_T *de,
                             DATA_VIEW_T *se, int upkm)
{
    bcm56880_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    FIXED_DATA_t fd_format;
    uint32_t data_format_raw[ALPM_L1_ENT_MAX] = {0};
    L3_DEFIP_DATA_LEVEL1_t *data_format = (void *)data_format_raw;
    uint32_t sub_db_prio;

    if (arg->key.vt == VRF_OVERRIDE) {
        sub_db_prio = SUB_DB_PRIORITY_OVERRIDE;
    } else if (arg->key.vt == VRF_GLOBAL) {
        sub_db_prio = SUB_DB_PRIORITY_GLOBAL;
    } else {
        sub_db_prio = SUB_DB_PRIORITY_VRF;
    }

    bcm56880_ln_alpm1_data_format_encode(u, &a1_format, &arg->a1);
    bcm56880_assoc_data_format_encode(u, &ad_format, arg->key.t.app, DATA_FULL, &arg->ad);
    bcm56880_fixed_data_format_encode(u, &fd_format, sub_db_prio, arg);
    L3_DEFIP_DATA_LEVEL1_ALPM1_DATA0f_SET(*data_format, (uint32_t *)&a1_format);
    L3_DEFIP_DATA_LEVEL1_ASSOC_DATA0f_SET(*data_format, (uint32_t *)&ad_format);
    L3_DEFIP_DATA_LEVEL1_FIXED_DATA0f_SET(*data_format, fd_format.v[0]);

    MEMDB_TCAM_IFTA80_MEM0_0_DATA_ONLYm_DATAf_SET(*de, data_format_raw);

}

/*!
 * \brief Encode FP data part into single-wide entry.
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] de DIP entry
 *
 * \return SHR_E_XXX.
 */
static void
bcm56880_l1_fp_data_view_encode(int u, alpm_arg_t *arg, DATA_VIEW_T *de)
{
    bcm56880_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    FIXED_DATA_t fd_format;
    uint32_t data_format_raw[ALPM_L1_ENT_MAX] = {0};
    L3_DEFIP_DATA_LEVEL1_t *data_format = (void *)data_format_raw;
    uint32_t sub_db_prio;

    sub_db_prio = SUB_DB_PRIORITY_GLOBAL;

    /* Only REDUCED is supported in FP_COMP LT */
    arg->ad.user_data_type = DATA_REDUCED;

    bcm56880_ln_alpm1_data_format_encode(u, &a1_format, &arg->a1);
    bcm56880_assoc_data_format_encode(u, &ad_format, arg->key.t.app, DATA_FULL, &arg->ad);
    bcm56880_fixed_data_format_encode(u, &fd_format, sub_db_prio, arg);
    L3_DEFIP_DATA_LEVEL1_ALPM1_DATA0f_SET(*data_format, (uint32_t *)&a1_format);
    L3_DEFIP_DATA_LEVEL1_ASSOC_DATA0f_SET(*data_format, (uint32_t *)&ad_format);
    L3_DEFIP_DATA_LEVEL1_FIXED_DATA0f_SET(*data_format, fd_format.v[0]);

    MEMDB_TCAM_IFTA80_MEM0_0_DATA_ONLYm_DATAf_SET(*de, data_format_raw);

}

static void
bcm56880_l1_narrow_encode_n1(int u, alpm_arg_t *arg, UNPAIR_T *de,
                             UNPAIR_T *se, int upkm)
{
    L3_DEFIP_TCAM_KEY_t kdata;
    L3_DEFIP_TCAM_KEY_t kmask;
    int vrf, vrf_mask;
    uint32_t sub_db_prio;
    bcm56880_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    FIXED_DATA_t fd_format;
    uint32_t data_format_raw[ALPM_L1_ENT_MAX] = {0};
    L3_DEFIP_DATA_LEVEL1_t *data_format = (void *)data_format_raw;
    L3_DEFIP_TCAM_LEVEL1_t l1_key;
    L3_DEFIP_TCAM_LEVEL1_t l1_msk;
    int valid;

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
        vrf_mask = ALPM_VRF_MASK;
    }

    if (arg->key.kt == KEY_IPV4) {
        arg->key.ip_addr[0] &= arg->key.ip_mask[0];

        bcm56880_l1_tcam_key_format_encode(u, vrf, HALF_WIDE, IP_VER_4,
                                           arg->key.ip_addr[0], &kdata);
        bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                           arg->key.ip_mask[0], &kmask);
        L3_DEFIP_LEVEL1_FMT_KEYf_GET(*de, (uint32_t *)&l1_key);
        L3_DEFIP_LEVEL1_FMT_MASKf_GET(*de, (uint32_t *)&l1_msk);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_msk, (uint32_t *)&kmask);
        L3_DEFIP_LEVEL1_FMT_KEYf_SET(*de, (uint32_t *)&l1_key);
        L3_DEFIP_LEVEL1_FMT_MASKf_SET(*de, (uint32_t *)&l1_msk);
        valid = L3_DEFIP_LEVEL1_FMT_VALIDf_GET(*de);
        L3_DEFIP_LEVEL1_FMT_VALIDf_SET(*de, valid | 0x2);
    } else if (arg->key.kt == KEY_IPV6_SINGLE && (upkm == PKM_S || upkm == PKM_SS)) {
        /* Apply subnet kmask. */
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[0] == 0);
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[1] == 0);
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[2] == 0);
        arg->key.ip_addr[3] &= arg->key.ip_mask[3];

        bcm56880_l1_tcam_key_format_encode(u, vrf, HALF_WIDE, IP_VER_6,
                                           arg->key.ip_addr[3], &kdata);
        bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                           arg->key.ip_mask[3], &kmask);
        L3_DEFIP_LEVEL1_FMT_KEYf_GET(*de, (uint32_t *)&l1_key);
        L3_DEFIP_LEVEL1_FMT_MASKf_GET(*de, (uint32_t *)&l1_msk);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_msk, (uint32_t *)&kmask);
        L3_DEFIP_LEVEL1_FMT_KEYf_SET(*de, (uint32_t *)&l1_key);
        L3_DEFIP_LEVEL1_FMT_MASKf_SET(*de, (uint32_t *)&l1_msk);
        valid = L3_DEFIP_LEVEL1_FMT_VALIDf_GET(*de);
        L3_DEFIP_LEVEL1_FMT_VALIDf_SET(*de, valid | 0x2);
    }

    bcm56880_ln_alpm1_data_format_encode(u, &a1_format, &arg->a1);
    bcm56880_assoc_data_format_encode(u, &ad_format, arg->key.t.app, DATA_FULL, &arg->ad);
    bcm56880_fixed_data_format_encode(u, &fd_format, sub_db_prio, arg);
    L3_DEFIP_DATA_LEVEL1_ALPM1_DATA0f_SET(*data_format, (uint32_t *)&a1_format);
    L3_DEFIP_DATA_LEVEL1_ASSOC_DATA0f_SET(*data_format, (uint32_t *)&ad_format);
    L3_DEFIP_DATA_LEVEL1_FIXED_DATA0f_SET(*data_format, fd_format.v[0]);

    L3_DEFIP_LEVEL1_FMT_DATA_ONLY1f_SET(*de, data_format_raw);
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
bcm56880_l1_narrow_encode(int u, alpm_arg_t *arg, UNPAIR_T *de,
                          UNPAIR_T *se, int upkm)
{
    L3_DEFIP_TCAM_KEY_t kdata;
    L3_DEFIP_TCAM_KEY_t kmask;
    int vrf, vrf_mask;
    uint32_t sub_db_prio;
    bcm56880_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    FIXED_DATA_t fd_format;
    int valid;
    uint32_t data_format_raw[ALPM_L1_ENT_MAX] = {0};
    L3_DEFIP_DATA_LEVEL1_t *data_format = (void *)data_format_raw;
    L3_DEFIP_TCAM_LEVEL1_t l1_key;
    L3_DEFIP_TCAM_LEVEL1_t l1_msk;

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
        vrf_mask = ALPM_VRF_MASK;
    }

    if (arg->key.kt == KEY_IPV4) {
        arg->key.ip_addr[0] &= arg->key.ip_mask[0];

        bcm56880_l1_tcam_key_format_encode(u, vrf, HALF_WIDE, IP_VER_4,
                                           arg->key.ip_addr[0], &kdata);
        bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 1,
                                           arg->key.ip_mask[0], &kmask);
        L3_DEFIP_LEVEL1_FMT_KEYf_GET(*de, (uint32_t *)&l1_key);
        L3_DEFIP_LEVEL1_FMT_MASKf_GET(*de, (uint32_t *)&l1_msk);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_msk, (uint32_t *)&kmask);
        L3_DEFIP_LEVEL1_FMT_KEYf_SET(*de, (uint32_t *)&l1_key);
        L3_DEFIP_LEVEL1_FMT_MASKf_SET(*de, (uint32_t *)&l1_msk);
        valid = L3_DEFIP_LEVEL1_FMT_VALIDf_GET(*de);
        L3_DEFIP_LEVEL1_FMT_VALIDf_SET(*de, valid | 0x1);
    } else if (arg->key.kt == KEY_IPV6_SINGLE && (upkm == PKM_S || upkm == PKM_SS)) {
        /* Apply subnet kmask. */
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[0] == 0);
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[1] == 0);
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[2] == 0);
        arg->key.ip_addr[3] &= arg->key.ip_mask[3];

        bcm56880_l1_tcam_key_format_encode(u, vrf, HALF_WIDE, IP_VER_6,
                                           arg->key.ip_addr[3], &kdata);
        bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                    arg->key.ip_mask[3], &kmask);
        L3_DEFIP_LEVEL1_FMT_KEYf_GET(*de, (uint32_t *)&l1_key);
        L3_DEFIP_LEVEL1_FMT_MASKf_GET(*de, (uint32_t *)&l1_msk);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_msk, (uint32_t *)&kmask);
        L3_DEFIP_LEVEL1_FMT_KEYf_SET(*de, (uint32_t *)&l1_key);
        L3_DEFIP_LEVEL1_FMT_MASKf_SET(*de, (uint32_t *)&l1_msk);
        valid = L3_DEFIP_LEVEL1_FMT_VALIDf_GET(*de);
        L3_DEFIP_LEVEL1_FMT_VALIDf_SET(*de, valid | 0x1);
    } else {
        /* IPv6 single wide */
        arg->key.ip_addr[3] &= arg->key.ip_mask[3];
        arg->key.ip_addr[2] &= arg->key.ip_mask[2];
        bcm56880_l1_tcam_key_format_encode(u, vrf, SINGLE_WIDE, IP_VER_6,
                                           arg->key.ip_addr[2], &kdata);
        bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                    arg->key.ip_mask[2], &kmask);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_msk, (uint32_t *)&kmask);

        bcm56880_l1_tcam_key_format_encode(u, vrf, SINGLE_WIDE, IP_VER_6,
                                           arg->key.ip_addr[3], &kdata);
        bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                    arg->key.ip_mask[3], &kmask);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_msk, (uint32_t *)&kmask);

        L3_DEFIP_LEVEL1_FMT_KEYf_SET(*de, (uint32_t *)&l1_key);
        L3_DEFIP_LEVEL1_FMT_MASKf_SET(*de, (uint32_t *)&l1_msk);
        L3_DEFIP_LEVEL1_FMT_VALIDf_SET(*de, 3);
    }

    bcm56880_ln_alpm1_data_format_encode(u, &a1_format, &arg->a1);
    bcm56880_assoc_data_format_encode(u, &ad_format, arg->key.t.app, DATA_FULL, &arg->ad);
    bcm56880_fixed_data_format_encode(u, &fd_format, sub_db_prio, arg);
    L3_DEFIP_DATA_LEVEL1_ALPM1_DATA0f_SET(*data_format, (uint32_t *)&a1_format);
    L3_DEFIP_DATA_LEVEL1_ASSOC_DATA0f_SET(*data_format, (uint32_t *)&ad_format);
    L3_DEFIP_DATA_LEVEL1_FIXED_DATA0f_SET(*data_format, fd_format.v[0]);

    L3_DEFIP_LEVEL1_FMT_DATA_ONLY0f_SET(*de, data_format_raw);
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
bcm56880_l1_fp_narrow_encode(int u, alpm_arg_t *arg, UNPAIR_T *de)
{
    L3_DEFIP_TCAM_KEY_t kdata;
    L3_DEFIP_TCAM_KEY_t kmask;
    uint32_t op, op_mask;
    int vrf, vrf_mask;
    uint32_t sub_db_prio;
    bcm56880_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    FIXED_DATA_t fd_format;
    uint32_t data_format_raw[ALPM_L1_ENT_MAX] = {0};
    L3_DEFIP_DATA_LEVEL1_t *data_format = (void *)data_format_raw;
    L3_DEFIP_TCAM_LEVEL1_t l1_key;
    L3_DEFIP_TCAM_LEVEL1_t l1_msk;
    uint16_t op1;         /*!< FP_COMP Opaque1 (L4_PORT) */
    uint16_t op2;         /*!< FP_COMP Opaque2 */

    bool is_src;
    is_src = bcmptm_rm_alpm_db_is_src(u, ALPM_0, arg->db);

    L3_DEFIP_TCAM_KEY_CLR(kdata);
    L3_DEFIP_TCAM_KEY_CLR(kmask);

    sub_db_prio = SUB_DB_PRIORITY_GLOBAL;
    vrf = VRF_DEFAULT;
    vrf_mask = 0;

    if (arg->key.t.w_vrf == (ALPM_MAX_FP_OP + 1)) { /* no L4_PORT */
        op1 = 0;
        op2 = 0;
        op_mask = 0;
    } else {
        if (is_src) {
            op1 = 0;
            op2 = arg->key.t.w_vrf; /* SRC L4_PORT coming from */
            op_mask = 0xffff0000;
        } else {
            op1 = arg->key.t.w_vrf; /* DST L4_PORT coming from */
            op2 = 0;
            op_mask = 0x0000ffff;
        }
    }

    op = op2;
    op <<= 16;
    op |= op1;

    /* IPv6 single wide */
    /* ip_addr ~ OP2[15:0], OP1[15:0] */
    bcm56880_l1_tcam_key_format_encode(u, vrf, SINGLE_WIDE, IP_VER_4,
                                       op, &kdata);
    bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                       op_mask, &kmask);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_key, (uint32_t *)&kdata);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_msk, (uint32_t *)&kmask);


    arg->key.ip_addr[0] &= arg->key.ip_mask[0];
    bcm56880_l1_tcam_key_format_encode(u, vrf, SINGLE_WIDE, IP_VER_4,
                                       arg->key.ip_addr[0], &kdata);
    bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                arg->key.ip_mask[0], &kmask);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_key, (uint32_t *)&kdata);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_msk, (uint32_t *)&kmask);

    L3_DEFIP_LEVEL1_FMT_KEYf_SET(*de, (uint32_t *)&l1_key);
    L3_DEFIP_LEVEL1_FMT_MASKf_SET(*de, (uint32_t *)&l1_msk);
    L3_DEFIP_LEVEL1_FMT_VALIDf_SET(*de, 3);

    /* Only REDUCED is supported in FP_COMP LT */
    arg->ad.user_data_type = DATA_REDUCED;

    bcm56880_ln_alpm1_data_format_encode(u, &a1_format, &arg->a1);
    bcm56880_assoc_data_format_encode(u, &ad_format, arg->key.t.app, DATA_FULL, &arg->ad);
    bcm56880_fixed_data_format_encode(u, &fd_format, sub_db_prio, arg);
    L3_DEFIP_DATA_LEVEL1_ALPM1_DATA0f_SET(*data_format, (uint32_t *)&a1_format);
    L3_DEFIP_DATA_LEVEL1_ASSOC_DATA0f_SET(*data_format, (uint32_t *)&ad_format);
    L3_DEFIP_DATA_LEVEL1_FIXED_DATA0f_SET(*data_format, fd_format.v[0]);

    L3_DEFIP_LEVEL1_FMT_DATA_ONLY0f_SET(*de, data_format_raw);
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
bcm56880_l1_wide_encode(int u, alpm_arg_t *arg, PAIR_T *de)
{
    L3_DEFIP_TCAM_KEY_t kdata;
    L3_DEFIP_TCAM_KEY_t kmask;
    int vrf, vrf_mask;
    uint32_t sub_db_prio;
    bcm56880_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    FIXED_DATA_t fd_format;
    uint32_t data_format_raw[ALPM_L1_ENT_MAX] = {0};
    L3_DEFIP_DATA_LEVEL1_t *data_format = (void *)data_format_raw;
    L3_DEFIP_TCAM_LEVEL1_t l1_key0;
    L3_DEFIP_TCAM_LEVEL1_t l1_msk0;
    L3_DEFIP_TCAM_LEVEL1_t l1_key1;
    L3_DEFIP_TCAM_LEVEL1_t l1_msk1;

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
        vrf_mask = ALPM_VRF_MASK;
    }

    /* Apply subnet kmask. */
    arg->key.ip_addr[0] &= arg->key.ip_mask[0];
    arg->key.ip_addr[1] &= arg->key.ip_mask[1];
    arg->key.ip_addr[2] &= arg->key.ip_mask[2];
    arg->key.ip_addr[3] &= arg->key.ip_mask[3];

    bcm56880_l1_tcam_key_format_encode(u, vrf, DOUBLE_WIDE, IP_VER_6,
                                       arg->key.ip_addr[0], &kdata);
    bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                       arg->key.ip_mask[0],
                                       &kmask);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_key0, (uint32_t *)&kdata);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_msk0, (uint32_t *)&kmask);

    bcm56880_l1_tcam_key_format_encode(u, vrf, DOUBLE_WIDE, IP_VER_6,
                                       arg->key.ip_addr[1], &kdata);
    bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                       arg->key.ip_mask[1],
                                       &kmask);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_key0, (uint32_t *)&kdata);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_msk0, (uint32_t *)&kmask);

    bcm56880_l1_tcam_key_format_encode(u, vrf, DOUBLE_WIDE, IP_VER_6,
                                       arg->key.ip_addr[2], &kdata);
    bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                       arg->key.ip_mask[2],
                                       &kmask);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_key1, (uint32_t *)&kdata);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_msk1, (uint32_t *)&kmask);

    bcm56880_l1_tcam_key_format_encode(u, vrf, DOUBLE_WIDE, IP_VER_6,
                                       arg->key.ip_addr[3], &kdata);
    bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                       arg->key.ip_mask[3],
                                       &kmask);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_key1, (uint32_t *)&kdata);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_msk1, (uint32_t *)&kmask);

    MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_KEY0f_SET(*de, (uint32_t *)&l1_key0);
    MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_MASK0f_SET(*de, (uint32_t *)&l1_msk0);
    MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_KEY1f_SET(*de, (uint32_t *)&l1_key1);
    MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_MASK1f_SET(*de, (uint32_t *)&l1_msk1);

    MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_VALID0f_SET(*de, 3);
    MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_VALID1f_SET(*de, 3);

    bcm56880_ln_alpm1_data_format_encode(u, &a1_format, &arg->a1);
    bcm56880_assoc_data_format_encode(u, &ad_format, arg->key.t.app, DATA_FULL, &arg->ad);
    bcm56880_fixed_data_format_encode(u, &fd_format, sub_db_prio, arg);
    L3_DEFIP_DATA_LEVEL1_ALPM1_DATA0f_SET(*data_format, (uint32_t *)&a1_format);
    L3_DEFIP_DATA_LEVEL1_ASSOC_DATA0f_SET(*data_format, (uint32_t *)&ad_format);
    L3_DEFIP_DATA_LEVEL1_FIXED_DATA0f_SET(*data_format, fd_format.v[0]);

    MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_DATA_ONLYf_SET(*de, data_format_raw);
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
bcm56880_l1_fp_wide_encode(int u, alpm_arg_t *arg, PAIR_T *de)
{
    L3_DEFIP_TCAM_KEY_t kdata;
    L3_DEFIP_TCAM_KEY_t kmask;
    uint32_t op, op_mask;
    int vrf, vrf_mask;
    uint32_t sub_db_prio;
    bcm56880_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    FIXED_DATA_t fd_format;
    uint32_t data_format_raw[ALPM_L1_ENT_MAX] = {0};
    L3_DEFIP_DATA_LEVEL1_t *data_format = (void *)data_format_raw;
    L3_DEFIP_TCAM_LEVEL1_t l1_key0;
    L3_DEFIP_TCAM_LEVEL1_t l1_msk0;
    L3_DEFIP_TCAM_LEVEL1_t l1_key1;
    L3_DEFIP_TCAM_LEVEL1_t l1_msk1;
    uint16_t op1;         /*!< FP_COMP Opaque1 (L4_PORT) */
    uint16_t op2;         /*!< FP_COMP Opaque2 */

    bool is_src;
    is_src = bcmptm_rm_alpm_db_is_src(u, ALPM_0, arg->db);

    L3_DEFIP_TCAM_KEY_CLR(kdata);
    L3_DEFIP_TCAM_KEY_CLR(kmask);

    sub_db_prio = SUB_DB_PRIORITY_GLOBAL;
    vrf = VRF_DEFAULT;
    vrf_mask = 0;

    if (arg->key.t.w_vrf == (ALPM_MAX_FP_OP + 1)) { /* no L4_PORT */
        op1 = 0;
        op2 = 0;
        op_mask = 0;
    } else {
        if (is_src) {
            op1 = 0;
            op2 = arg->key.t.w_vrf; /* SRC L4_PORT coming from */
            op_mask = 0xffff0000;
        } else {
            op1 = arg->key.t.w_vrf; /* DST L4_PORT coming from */
            op2 = 0;
            op_mask = 0x0000ffff;
        }
    }

    op = op2;
    op <<= 16;
    op |= op1;

    /* Apply subnet kmask. */
    arg->key.ip_addr[0] &= arg->key.ip_mask[0];
    arg->key.ip_addr[1] &= arg->key.ip_mask[1];
    arg->key.ip_addr[2] &= arg->key.ip_mask[2];
    arg->key.ip_addr[3] &= arg->key.ip_mask[3];

    bcm56880_l1_tcam_key_format_encode(u, vrf, DOUBLE_WIDE, IP_VER_6,
                                       arg->key.ip_addr[0], &kdata);
    bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                       arg->key.ip_mask[0],
                                       &kmask);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_key0, (uint32_t *)&kdata);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_msk0, (uint32_t *)&kmask);

    /* vrf~ OP1[14:0] */
    vrf = op & 0x7fff;
    vrf_mask = op_mask & 0x7fff;
    bcm56880_l1_tcam_key_format_encode(u, vrf, DOUBLE_WIDE, IP_VER_6,
                                       arg->key.ip_addr[1], &kdata);
    bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                       arg->key.ip_mask[1],
                                       &kmask);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_key0, (uint32_t *)&kdata);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_msk0, (uint32_t *)&kmask);

    /* vrf ~ OP2[13:0], OP1[15] */
    vrf = (op >> 15) & 0x7fff;
    vrf_mask = (op_mask >> 15) & 0x7fff;
    bcm56880_l1_tcam_key_format_encode(u, vrf, DOUBLE_WIDE, IP_VER_6,
                                       arg->key.ip_addr[2], &kdata);
    bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                       arg->key.ip_mask[2],
                                       &kmask);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_key1, (uint32_t *)&kdata);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(l1_msk1, (uint32_t *)&kmask);

    /* vrf ~ OP2[15:14] */
    vrf = (op >> 30) & 0x0003;
    vrf_mask = (op_mask >> 30) & 0x0003;
    bcm56880_l1_tcam_key_format_encode(u, vrf, DOUBLE_WIDE, IP_VER_6,
                                       arg->key.ip_addr[3], &kdata);
    bcm56880_l1_tcam_key_format_encode(u, vrf_mask, 0x3, 0x1,
                                       arg->key.ip_mask[3],
                                       &kmask);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_key1, (uint32_t *)&kdata);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(l1_msk1, (uint32_t *)&kmask);

    MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_KEY0f_SET(*de, (uint32_t *)&l1_key0);
    MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_MASK0f_SET(*de, (uint32_t *)&l1_msk0);
    MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_KEY1f_SET(*de, (uint32_t *)&l1_key1);
    MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_MASK1f_SET(*de, (uint32_t *)&l1_msk1);

    MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_VALID0f_SET(*de, 3);
    MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_VALID1f_SET(*de, 3);

    /* Only REDUCED is supported in FP_COMP LT */
    arg->ad.user_data_type = DATA_REDUCED;

    bcm56880_ln_alpm1_data_format_encode(u, &a1_format, &arg->a1);
    bcm56880_assoc_data_format_encode(u, &ad_format, arg->key.t.app, DATA_FULL, &arg->ad);
    bcm56880_fixed_data_format_encode(u, &fd_format, sub_db_prio, arg);
    L3_DEFIP_DATA_LEVEL1_ALPM1_DATA0f_SET(*data_format, (uint32_t *)&a1_format);
    L3_DEFIP_DATA_LEVEL1_ASSOC_DATA0f_SET(*data_format, (uint32_t *)&ad_format);
    L3_DEFIP_DATA_LEVEL1_FIXED_DATA0f_SET(*data_format, fd_format.v[0]);

    MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_DATA_ONLYf_SET(*de, data_format_raw);
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
bcm56880_l1_encode(int u, int m, void *varg, void *de, void *se, uint8_t l1v)
{
    alpm_arg_t *arg = varg;

    if (arg->encode_key_view) {
        if (arg->key.t.app == APP_COMP0) {
            if (l1v == L1V_WIDE) {
                bcm56880_l1_fp_key_view_wide_encode(u, arg, de, se);
            } else {
                bcm56880_l1_fp_key_view_narrow_encode(u, arg, de);
            }
            return SHR_E_NONE;
        }

        if (l1v == L1V_WIDE) {
            bcm56880_l1_key_view_encode(u, arg, de, se, arg->pkm);
        } else if (l1v == L1V_NARROW_1) {
            bcm56880_l1_key_view_encode_n1(u, arg, de, arg->pkm);
        } else {
            bcm56880_l1_key_view_encode(u, arg, de, NULL, arg->pkm);
        }
    } else if (arg->encode_data_view) {
        if (arg->key.t.app == APP_COMP0) {
            bcm56880_l1_fp_data_view_encode(u, arg, de);
        } else {
            bcm56880_l1_data_view_encode(u, arg, de, se, arg->pkm);
        }
    } else {
        if (arg->key.t.app == APP_COMP0) {
            if (l1v == L1V_WIDE) {
                bcm56880_l1_fp_wide_encode(u, arg, de);
            } else {
                bcm56880_l1_fp_narrow_encode(u, arg, de);
            }
            return SHR_E_NONE;
        }

        if (l1v == L1V_WIDE) {
            bcm56880_l1_wide_encode(u, arg, de);
        } else if (l1v == L1V_NARROW_1) {
            bcm56880_l1_narrow_encode_n1(u, arg, de, se, arg->pkm);
        } else {
            bcm56880_l1_narrow_encode(u, arg, de, se, arg->pkm);
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
bcm56880_l1_narrow_decode_n1(int u, alpm_arg_t *arg, UNPAIR_T *de)
{
    L3_DEFIP_TCAM_KEY_t kdata;
    L3_DEFIP_TCAM_KEY_t kmask;
    L3_DEFIP_TCAM_LEVEL1_t l1_kdata;
    L3_DEFIP_TCAM_LEVEL1_t l1_kmask;
    int key_mode = 0;
    int vrf, vrf_mask;
    int sub_prio;
    uint32_t ip_addr, ip_mask;
    bcm56880_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    FIXED_DATA_t fd_format;
    uint32_t data_format_raw[ALPM_L1_ENT_MAX] = {0};
    L3_DEFIP_DATA_LEVEL1_t *data_format = (void *)data_format_raw;

    L3_DEFIP_TCAM_LEVEL1_CLR(l1_kdata);
    L3_DEFIP_TCAM_LEVEL1_CLR(l1_kmask);

    L3_DEFIP_LEVEL1_FMT_KEYf_GET(*de, (uint32_t *)&l1_kdata);
    L3_DEFIP_LEVEL1_FMT_MASKf_GET(*de, (uint32_t *)&l1_kmask);

    L3_DEFIP_TCAM_KEY_CLR(kdata);
    L3_DEFIP_TCAM_KEY_CLR(kmask);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_GET(l1_kdata, (uint32_t *)&kdata);
    L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_GET(l1_kmask, (uint32_t *)&kmask);
    bcm56880_l1_tcam_key_format_decode(u, &vrf_mask, NULL, NULL, &ip_mask, &kmask);
    bcm56880_l1_tcam_key_format_decode(u, &vrf, &key_mode, &arg->key.t.ipv, &ip_addr, &kdata);

    if (!arg->key.t.ipv) {
        arg->valid = (L3_DEFIP_LEVEL1_FMT_VALIDf_GET(*de) & 0x2) ? 1 : 0;
        arg->key.ip_addr[0] = ip_addr;
        arg->key.ip_mask[0] = ip_mask;
    } else if (key_mode == HALF_WIDE) {
        arg->valid = (L3_DEFIP_LEVEL1_FMT_VALIDf_GET(*de) & 0x2) ? 1 : 0;
        arg->key.ip_addr[3] = ip_addr;
        arg->key.ip_mask[3] = ip_mask;
    }

    L3_DEFIP_LEVEL1_FMT_DATA_ONLY1f_GET(*de, data_format_raw);
    fd_format.v[0] = L3_DEFIP_DATA_LEVEL1_FIXED_DATA0f_GET(*data_format);
    sub_prio = FIXED_DATA_SUB_DB_PRIORITYf_GET(fd_format);
    if (sub_prio == SUB_DB_PRIORITY_OVERRIDE) {
        arg->key.vt = VRF_OVERRIDE;
    } else if (sub_prio == SUB_DB_PRIORITY_GLOBAL) {
        arg->key.vt = VRF_GLOBAL;
    } else {
        /*
         * The assoc data index is different for single/half wide,
         * Therefore, the sub_prio can be different for single/half wide.
         * During recovery, in the single->half boundary, an incorrect assoc data
         * can be retrieved temporary, and that's fine.
         * We need to allow this case exists specifically in TD4-X11.
            ALPM_ASSERT(!arg->valid || vrf_mask == ALPM_VRF_MASK);
            ALPM_ASSERT(!arg->valid || vrf <= ALPM_MAX_VRF);
         */
        arg->key.vt = VRF_PRIVATE;
        arg->key.t.w_vrf = vrf;
    }

    L3_DEFIP_DATA_LEVEL1_ALPM1_DATA0f_GET(*data_format, (uint32_t *)&a1_format);
    L3_DEFIP_DATA_LEVEL1_ASSOC_DATA0f_GET(*data_format, (uint32_t *)&ad_format);

    bcm56880_ln_alpm1_data_format_decode(u, &a1_format, &arg->a1);

    bcm56880_fixed_data_format_decode(u, &fd_format, arg);
    bcm56880_assoc_data_format_decode(u, &ad_format, arg->key.t.app, DATA_FULL,
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
bcm56880_l1_narrow_decode(int u, alpm_arg_t *arg, UNPAIR_T *de)
{
    L3_DEFIP_TCAM_KEY_t kdata;
    L3_DEFIP_TCAM_KEY_t kmask;
    L3_DEFIP_TCAM_LEVEL1_t l1_kdata;
    L3_DEFIP_TCAM_LEVEL1_t l1_kmask;
    int key_mode = 0;
    int vrf, vrf_mask;
    int sub_prio, value;
    uint32_t ip_addr, ip_mask;
    bcm56880_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    FIXED_DATA_t fd_format;
    uint32_t data_format_raw[ALPM_L1_ENT_MAX] = {0};
    L3_DEFIP_DATA_LEVEL1_t *data_format = (void *)data_format_raw;

    L3_DEFIP_LEVEL1_FMT_DATA_ONLY0f_GET(*de, data_format_raw);
    fd_format.v[0] = L3_DEFIP_DATA_LEVEL1_FIXED_DATA0f_GET(*data_format);
    if (!arg->decode_data_only) {
        L3_DEFIP_TCAM_LEVEL1_CLR(l1_kdata);
        L3_DEFIP_TCAM_LEVEL1_CLR(l1_kmask);

        L3_DEFIP_LEVEL1_FMT_KEYf_GET(*de, (uint32_t *)&l1_kdata);
        L3_DEFIP_LEVEL1_FMT_MASKf_GET(*de, (uint32_t *)&l1_kmask);

        L3_DEFIP_TCAM_KEY_CLR(kdata);
        L3_DEFIP_TCAM_KEY_CLR(kmask);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_GET(l1_kdata, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_GET(l1_kmask, (uint32_t *)&kmask);
        bcm56880_l1_tcam_key_format_decode(u, &vrf_mask, NULL, NULL, &ip_mask, &kmask);
        bcm56880_l1_tcam_key_format_decode(u, &vrf, &key_mode, &arg->key.t.ipv, &ip_addr, &kdata);

        value = L3_DEFIP_LEVEL1_FMT_VALIDf_GET(*de);
        if (!arg->key.t.ipv) {
            arg->valid = value & 0x1;
            arg->key.ip_addr[0] = ip_addr;
            arg->key.ip_mask[0] = ip_mask;
        } else if (key_mode == HALF_WIDE) {
            /* IPv6 half wide */
            arg->valid = value & 0x1;
            arg->key.ip_addr[3] = ip_addr;
            arg->key.ip_mask[3] = ip_mask;
        } else {
            /* IPv6 single wide */
            arg->valid = value == 3;
            arg->key.ip_addr[2] = ip_addr;
            arg->key.ip_mask[2] = ip_mask;

            L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_GET(l1_kdata, (uint32_t *)&kdata);
            L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_GET(l1_kmask, (uint32_t *)&kmask);
            bcm56880_l1_tcam_key_format_decode(u, NULL, NULL, NULL, &ip_mask, &kmask);
            bcm56880_l1_tcam_key_format_decode(u, NULL, NULL, NULL, &ip_addr, &kdata);
            arg->key.ip_addr[3] = ip_addr;
            arg->key.ip_mask[3] = ip_mask;
        }

        sub_prio = FIXED_DATA_SUB_DB_PRIORITYf_GET(fd_format);
        if (sub_prio == SUB_DB_PRIORITY_OVERRIDE) {
            arg->key.vt = VRF_OVERRIDE;
        } else if (sub_prio == SUB_DB_PRIORITY_GLOBAL) {
            arg->key.vt = VRF_GLOBAL;
        } else {
        /*
         * The assoc data index is different for single/half wide,
         * Therefore, the sub_prio can be different for single/half wide.
         * During recovery, in the single->half boundary, an incorrect assoc data
         * can be retrieved temporary, and that's fine.
         * We need to allow this case exists specifically in TD4-X11.
            ALPM_ASSERT(!arg->valid || vrf_mask == ALPM_VRF_MASK);
            ALPM_ASSERT(!arg->valid || vrf <= ALPM_MAX_VRF);
         */
            arg->key.vt = VRF_PRIVATE;
            arg->key.t.w_vrf = vrf;
        }
    }

    L3_DEFIP_DATA_LEVEL1_ALPM1_DATA0f_GET(*data_format, (uint32_t *)&a1_format);
    L3_DEFIP_DATA_LEVEL1_ASSOC_DATA0f_GET(*data_format, (uint32_t *)&ad_format);

    bcm56880_ln_alpm1_data_format_decode(u, &a1_format, &arg->a1);
    bcm56880_fixed_data_format_decode(u, &fd_format, arg);
    bcm56880_assoc_data_format_decode(u, &ad_format, arg->key.t.app, DATA_FULL,
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
bcm56880_l1_fp_narrow_decode(int u, alpm_arg_t *arg, UNPAIR_T *de)
{
    L3_DEFIP_TCAM_KEY_t kdata;
    L3_DEFIP_TCAM_KEY_t kmask;
    L3_DEFIP_TCAM_LEVEL1_t l1_kdata;
    L3_DEFIP_TCAM_LEVEL1_t l1_kmask;
    uint16_t op1;         /*!< FP_COMP Opaque1 (L4_PORT) */
    uint16_t op2;         /*!< FP_COMP Opaque2 */

    uint32_t op, op_mask;
    int key_mode = 0;
    int vrf, vrf_mask;
    uint32_t ip_addr, ip_mask;
    bcm56880_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    FIXED_DATA_t fd_format;
    uint32_t data_format_raw[ALPM_L1_ENT_MAX] = {0};
    L3_DEFIP_DATA_LEVEL1_t *data_format = (void *)data_format_raw;

    L3_DEFIP_LEVEL1_FMT_DATA_ONLY0f_GET(*de, data_format_raw);
    fd_format.v[0] = L3_DEFIP_DATA_LEVEL1_FIXED_DATA0f_GET(*data_format);

    if (!arg->decode_data_only) {
        L3_DEFIP_TCAM_LEVEL1_CLR(l1_kdata);
        L3_DEFIP_TCAM_LEVEL1_CLR(l1_kmask);

        L3_DEFIP_LEVEL1_FMT_KEYf_GET(*de, (uint32_t *)&l1_kdata);
        L3_DEFIP_LEVEL1_FMT_MASKf_GET(*de, (uint32_t *)&l1_kmask);

        L3_DEFIP_TCAM_KEY_CLR(kdata);
        L3_DEFIP_TCAM_KEY_CLR(kmask);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_GET(l1_kdata, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_GET(l1_kmask, (uint32_t *)&kmask);
        bcm56880_l1_tcam_key_format_decode(u, &vrf_mask, NULL, NULL, &op_mask, &kmask);
        bcm56880_l1_tcam_key_format_decode(u, &vrf, &key_mode, &arg->key.t.ipv, &op, &kdata);
        /* ip_addr ~ OP2[15:0], OP1[15:0] */
        op1 = op & op_mask & 0xffff;
        op2 = (op & op_mask) >> 16;

        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_GET(l1_kdata, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_GET(l1_kmask, (uint32_t *)&kmask);
        bcm56880_l1_tcam_key_format_decode(u, &vrf_mask, NULL, NULL, &ip_mask, &kmask);
        bcm56880_l1_tcam_key_format_decode(u, &vrf, &key_mode, &arg->key.t.ipv, &ip_addr, &kdata);
        ALPM_ASSERT(arg->key.t.ipv == 0);
        arg->key.ip_addr[0] = ip_addr;
        arg->key.ip_mask[0] = ip_mask;
        arg->valid = L3_DEFIP_LEVEL1_FMT_VALIDf_GET(*de) == 3;

        ALPM_ASSERT(key_mode == 0);
        ALPM_ASSERT(vrf_mask == 0);
        arg->key.vt = VRF_DEFAULT;

        if (op_mask & 0xffff) {
            arg->key.t.w_vrf = op1; /* carry DST L4_PORT */
        } else if (op_mask & 0xffff0000) {
            arg->key.t.w_vrf = op2; /* carry SRC L4_PORT */
        } else { /* no L4_PORT */
            arg->key.t.w_vrf = ALPM_MAX_FP_OP + 1;
        }
    }

    L3_DEFIP_DATA_LEVEL1_ALPM1_DATA0f_GET(*data_format, (uint32_t *)&a1_format);
    L3_DEFIP_DATA_LEVEL1_ASSOC_DATA0f_GET(*data_format, (uint32_t *)&ad_format);

    bcm56880_ln_alpm1_data_format_decode(u, &a1_format, &arg->a1);
    bcm56880_fixed_data_format_decode(u, &fd_format, arg);

    /* Only REDUCED is supported in FP_COMP LT */
    arg->ad.user_data_type = DATA_REDUCED;
    bcm56880_assoc_data_format_decode(u, &ad_format, arg->key.t.app, DATA_FULL,
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
bcm56880_l1_wide_decode(int u, alpm_arg_t *arg, PAIR_T *de)
{
    L3_DEFIP_TCAM_KEY_t kdata;
    L3_DEFIP_TCAM_KEY_t kmask;
    L3_DEFIP_TCAM_LEVEL1_t l1_key;
    L3_DEFIP_TCAM_LEVEL1_t l1_msk;

    int key_mode = 0;
    int vrf, vrf_mask;
    int sub_prio;
    bcm56880_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    FIXED_DATA_t fd_format;
    uint32_t data_format_raw[ALPM_L1_ENT_MAX] = {0};
    L3_DEFIP_DATA_LEVEL1_t *data_format = (void *)data_format_raw;

    MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_DATA_ONLYf_GET(*de, data_format_raw);
    fd_format.v[0] = L3_DEFIP_DATA_LEVEL1_FIXED_DATA0f_GET(*data_format);
    if (!arg->decode_data_only) {
        L3_DEFIP_TCAM_LEVEL1_CLR(l1_key);
        L3_DEFIP_TCAM_LEVEL1_CLR(l1_msk);
        L3_DEFIP_TCAM_KEY_CLR(kdata);
        L3_DEFIP_TCAM_KEY_CLR(kmask);

        MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_KEY0f_GET(*de, (uint32_t *)&l1_key);
        MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_MASK0f_GET(*de, (uint32_t *)&l1_msk);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_GET(l1_key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_GET(l1_msk, (uint32_t *)&kmask);
        bcm56880_l1_tcam_key_format_decode(u, &vrf_mask, NULL, NULL, &arg->key.ip_mask[0], &kmask);
        bcm56880_l1_tcam_key_format_decode(u, &vrf, &key_mode, &arg->key.t.ipv, &arg->key.ip_addr[0], &kdata);

        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_GET(l1_key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_GET(l1_msk, (uint32_t *)&kmask);
        bcm56880_l1_tcam_key_format_decode(u, &vrf_mask, NULL, NULL, &arg->key.ip_mask[1], &kmask);
        bcm56880_l1_tcam_key_format_decode(u, &vrf, &key_mode, &arg->key.t.ipv, &arg->key.ip_addr[1], &kdata);

        MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_KEY1f_GET(*de, (uint32_t *)&l1_key);
        MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_MASK1f_GET(*de, (uint32_t *)&l1_msk);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_GET(l1_key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_GET(l1_msk, (uint32_t *)&kmask);
        bcm56880_l1_tcam_key_format_decode(u, &vrf_mask, NULL, NULL, &arg->key.ip_mask[2], &kmask);
        bcm56880_l1_tcam_key_format_decode(u, &vrf, &key_mode, &arg->key.t.ipv, &arg->key.ip_addr[2], &kdata);

        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_GET(l1_key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_GET(l1_msk, (uint32_t *)&kmask);
        bcm56880_l1_tcam_key_format_decode(u, &vrf_mask, NULL, NULL, &arg->key.ip_mask[3], &kmask);
        bcm56880_l1_tcam_key_format_decode(u, &vrf, &key_mode, &arg->key.t.ipv, &arg->key.ip_addr[3], &kdata);

        arg->valid = (MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_VALID0f_GET(*de) &
                      MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_VALID1f_GET(*de)) == 3;

        sub_prio = FIXED_DATA_SUB_DB_PRIORITYf_GET(fd_format);

        if (sub_prio == SUB_DB_PRIORITY_OVERRIDE) {
            arg->key.vt = VRF_OVERRIDE;
        } else if (sub_prio == SUB_DB_PRIORITY_GLOBAL) {
            arg->key.vt = VRF_GLOBAL;
        } else {
            arg->key.vt = VRF_PRIVATE;
            arg->key.t.w_vrf = vrf;
        }
    }

    /* Data */
    L3_DEFIP_DATA_LEVEL1_ALPM1_DATA0f_GET(*data_format, (uint32_t *)&a1_format);
    L3_DEFIP_DATA_LEVEL1_ASSOC_DATA0f_GET(*data_format, (uint32_t *)&ad_format);

    bcm56880_ln_alpm1_data_format_decode(u, &a1_format, &arg->a1);
    bcm56880_fixed_data_format_decode(u, &fd_format, arg);
    bcm56880_assoc_data_format_decode(u, &ad_format, arg->key.t.app, DATA_FULL,
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
bcm56880_l1_fp_wide_decode(int u, alpm_arg_t *arg, PAIR_T *de)
{
    L3_DEFIP_TCAM_KEY_t kdata;
    L3_DEFIP_TCAM_KEY_t kmask;
    L3_DEFIP_TCAM_LEVEL1_t l1_key;
    L3_DEFIP_TCAM_LEVEL1_t l1_msk;

    int op, op_mask, op1_mask, op2_mask;
    int key_mode = 0;
    int vrf, vrf_mask;
    bcm56880_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    FIXED_DATA_t fd_format;
    uint32_t data_format_raw[ALPM_L1_ENT_MAX] = {0};
    L3_DEFIP_DATA_LEVEL1_t *data_format = (void *)data_format_raw;
    uint16_t op1;         /*!< FP_COMP Opaque1 (L4_PORT) */
    uint16_t op2;         /*!< FP_COMP Opaque2 */

    MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_DATA_ONLYf_GET(*de, data_format_raw);
    fd_format.v[0] = L3_DEFIP_DATA_LEVEL1_FIXED_DATA0f_GET(*data_format);

    if (!arg->decode_data_only) {
        L3_DEFIP_TCAM_LEVEL1_CLR(l1_key);
        L3_DEFIP_TCAM_LEVEL1_CLR(l1_msk);
        L3_DEFIP_TCAM_KEY_CLR(kdata);
        L3_DEFIP_TCAM_KEY_CLR(kmask);

        MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_KEY0f_GET(*de, (uint32_t *)&l1_key);
        MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_MASK0f_GET(*de, (uint32_t *)&l1_msk);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_GET(l1_key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_GET(l1_msk, (uint32_t *)&kmask);
        bcm56880_l1_tcam_key_format_decode(u, &vrf_mask, NULL, NULL, &arg->key.ip_mask[0], &kmask);
        bcm56880_l1_tcam_key_format_decode(u, &vrf, &key_mode, &arg->key.t.ipv, &arg->key.ip_addr[0], &kdata);
        op1 = 0;
        op2 = 0;

        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_GET(l1_key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_GET(l1_msk, (uint32_t *)&kmask);
        bcm56880_l1_tcam_key_format_decode(u, &op_mask, NULL, NULL, &arg->key.ip_mask[1], &kmask);
        bcm56880_l1_tcam_key_format_decode(u, &op, &key_mode, &arg->key.t.ipv, &arg->key.ip_addr[1], &kdata);
        op1 = (uint16_t) (op & op_mask); /* vrf ~ OP1[14:0] */
        op1_mask = op_mask;

        MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_KEY1f_GET(*de, (uint32_t *)&l1_key);
        MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_MASK1f_GET(*de, (uint32_t *)&l1_msk);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_GET(l1_key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_GET(l1_msk, (uint32_t *)&kmask);
        bcm56880_l1_tcam_key_format_decode(u, &op_mask, NULL, NULL, &arg->key.ip_mask[2], &kmask);
        bcm56880_l1_tcam_key_format_decode(u, &op, &key_mode, &arg->key.t.ipv, &arg->key.ip_addr[2], &kdata);
        /* vrf ~ OP2[13:0], OP1[15] */
        op1 |= (uint16_t) ((op & op_mask & 0x01) << 15);
        op2 |= (uint16_t) ((op & op_mask) >> 1);
        op1_mask |= (op_mask & 0x01) << 15;
        op2_mask = op_mask >> 1;

        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_GET(l1_key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_GET(l1_msk, (uint32_t *)&kmask);
        bcm56880_l1_tcam_key_format_decode(u, &op_mask, NULL, NULL, &arg->key.ip_mask[3], &kmask);
        bcm56880_l1_tcam_key_format_decode(u, &op, &key_mode, &arg->key.t.ipv, &arg->key.ip_addr[3], &kdata);
        op2 |= (uint16_t) ((op & op_mask) << 14); /* vrf ~ OP2[15:14] */
        op2_mask |= op_mask << 14;

        ALPM_ASSERT(key_mode == 1);
        ALPM_ASSERT(vrf_mask == 0);
        ALPM_ASSERT(arg->key.t.ipv == IP_VER_6);
        arg->key.vt = VRF_DEFAULT;

        if (op1_mask) {
            arg->key.t.w_vrf = op1; /* carry DST L4_PORT */
        } else if (op2_mask) {
            arg->key.t.w_vrf = op2; /* carry SRC L4_PORT */
        } else { /* no L4_PORT */
            arg->key.t.w_vrf = ALPM_MAX_FP_OP + 1;
        }

        arg->valid = (MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_VALID0f_GET(*de) &
                      MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m_VALID1f_GET(*de)) == 3;
    }

    /* Data */
    L3_DEFIP_DATA_LEVEL1_ALPM1_DATA0f_GET(*data_format, (uint32_t *)&a1_format);
    L3_DEFIP_DATA_LEVEL1_ASSOC_DATA0f_GET(*data_format, (uint32_t *)&ad_format);

    bcm56880_ln_alpm1_data_format_decode(u, &a1_format, &arg->a1);
    bcm56880_fixed_data_format_decode(u, &fd_format, arg);
    /* Only REDUCED is supported in FP_COMP LT */
    arg->ad.user_data_type = DATA_REDUCED;
    bcm56880_assoc_data_format_decode(u, &ad_format, arg->key.t.app, DATA_FULL,
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
bcm56880_l1_decode(int u, int m, void *varg, void *de, void *se, uint8_t l1v)
{
    alpm_arg_t *arg = varg;

    if (FP_PKM(arg->pkm)) { /* for l1_recover during abort (no app available) */
        arg->key.t.app = APP_COMP0;
        if (l1v == L1V_WIDE) {
            bcm56880_l1_fp_wide_decode(u, arg, de);
        } else {
            bcm56880_l1_fp_narrow_decode(u, arg, de);
        }
        return SHR_E_NONE;
    }

    arg->key.t.app = APP_LPM;
    if (l1v == L1V_WIDE) {
        bcm56880_l1_wide_decode(u, arg, de);
    } else if (l1v == L1V_NARROW_1) {
        bcm56880_l1_narrow_decode_n1(u, arg, de);
    } else {
        bcm56880_l1_narrow_decode(u, arg, de);
    }
    return SHR_E_NONE;

}
static void
bcm56880_l1_wide_template_init(int u)
{
}

static void
bcm56880_l1_narrow_template_init(int u)
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
bcm56880_l1_half_entry_move(int u, int m, void *src, void *dst, int x, int y, int pkm)
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

    L3_DEFIP_TCAM_KEY_CLR(kdata);
    L3_DEFIP_TCAM_KEY_CLR(kmask);

    valid = L3_DEFIP_LEVEL1_FMT_VALIDf_GET(*se);
    L3_DEFIP_LEVEL1_FMT_KEYf_GET(*se, (uint32_t *)&key);
    L3_DEFIP_LEVEL1_FMT_MASKf_GET(*se, (uint32_t *)&mask);

    if (x) { /* source is 1 */
        v1 = !!(valid & 0x2); /* entry 1*/
        ALPM_ASSERT(v1);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_GET(key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_GET(mask, (uint32_t *)&kmask);
        L3_DEFIP_LEVEL1_FMT_DATA_ONLY1f_GET(*se, (uint32_t *)&data_format_raw);
    } else { /* source is 0 */
        v1 = valid & 0x1; /* entry 0 */
        ALPM_ASSERT(v1);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_GET(key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_GET(mask, (uint32_t *)&kmask);
        L3_DEFIP_LEVEL1_FMT_DATA_ONLY0f_GET(*se, (uint32_t *)&data_format_raw);
    }

    L3_DEFIP_LEVEL1_FMT_KEYf_GET(*de, (uint32_t *)&key);
    L3_DEFIP_LEVEL1_FMT_MASKf_GET(*de, (uint32_t *)&mask);

    valid = L3_DEFIP_LEVEL1_FMT_VALIDf_GET(*de);
    if (y) { /* dest is 1 */
        L3_DEFIP_LEVEL1_FMT_VALIDf_SET(*de, (valid & 0x1) | (v1 << 1)); /* entry 1 */
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_1f_SET(mask, (uint32_t *)&kmask);
        L3_DEFIP_LEVEL1_FMT_DATA_ONLY1f_SET(*de, (uint32_t *)&data_format_raw);
    } else { /* dest is 0 */
        L3_DEFIP_LEVEL1_FMT_VALIDf_SET(*de, (valid & 0x2) | v1); /* entry 0 */
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(key, (uint32_t *)&kdata);
        L3_DEFIP_TCAM_LEVEL1_ENTRY_0f_SET(mask, (uint32_t *)&kmask);
        L3_DEFIP_LEVEL1_FMT_DATA_ONLY0f_SET(*de, (uint32_t *)&data_format_raw);
    }

    L3_DEFIP_LEVEL1_FMT_KEYf_SET(*de, (uint32_t *)&key);
    L3_DEFIP_LEVEL1_FMT_MASKf_SET(*de, (uint32_t *)&mask);

    /* Clear source */
    valid = L3_DEFIP_LEVEL1_FMT_VALIDf_GET(*se);
    if (x) {
        L3_DEFIP_LEVEL1_FMT_VALIDf_SET(*se, valid & 0x1); /* clear entry 1 */
    } else {
        L3_DEFIP_LEVEL1_FMT_VALIDf_SET(*se, valid & 0x2); /* clear entry 0 */
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
bcm56880_l1_half_key_move(int u, int m, void *src, void *dst, int x, int y, int pkm)
{
    uint32_t v1;
    TCAM_VIEW_T *se = src;
    TCAM_VIEW_T *de = dst;
    int valid;
    L3_DEFIP_TCAM_KEY_t kdata;
    L3_DEFIP_TCAM_KEY_t kmask;
    L3_DEFIP_TCAM_LEVEL1_t key;
    L3_DEFIP_TCAM_LEVEL1_t mask;

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
bcm56880_l1_half_key_clear(int u, int m, void *de, int x, int pkm)
{
    TCAM_VIEW_T *e = de;
    int valid;

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
bcm56880_ln_sid_index_map(int u, int m, alpm_sid_index_map_t *map_info)
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
    static int l2_max = sizeof (l2_sids) / sizeof (bcmdrd_sid_t);
    static int l3_max = sizeof (l3_sids) / sizeof (bcmdrd_sid_t);

    if (map_info->wide == LEVEL2) {
        sid_idx = (map_info->index >> bcm56880_l2_bucket_bits[u]) & 0x7;
        if (sid_idx >= l2_max) {
            return SHR_E_INTERNAL;
        }
        map_info->sid = l2_sids[sid_idx];
        map_info->sid_index = map_info->index & bcm56880_l2_bucket_mask[u];
    } else if (map_info->wide == LEVEL3) {
        sid_idx = (map_info->index >> bcm56880_l3_bucket_bits[u]) & 0x7;
        if (sid_idx >= l3_max) {
            return SHR_E_INTERNAL;
        }
        map_info->sid = l3_sids[sid_idx];
        map_info->sid_index = map_info->index & bcm56880_l3_bucket_mask[u];
    }

    return SHR_E_NONE;
}



static int
bcm56880_l1_sid_index_map(int u, int m, alpm_sid_index_map_t *map_info)
{
    int depth, view;
    bcmdrd_sid_t *sid;
    int t_num;

    depth = BCM56880_TCAM_DEPTH;


    if (map_info->wide == WIDE_DOUBLE) {
        if (map_info->sid_type == SID_TYPE_TCAM) {
            view = PAIR_IDX_TCAM0_ONLY_VIEW;
        } else if (map_info->sid_type == SID_TYPE_TCAM2) {
            view = PAIR_IDX_TCAM1_ONLY_VIEW;
        } else if (map_info->sid_type == SID_TYPE_DATA) {
            view = PAIR_IDX_DATA_ONLY_VIEW;
        } else if (map_info->sid_type == SID_TYPE_COMBINED) {
            view = PAIR_IDX_COMBINED_VIEW;
        } else {
            return SHR_E_INTERNAL;
        }
        t_num = (map_info->index / depth);
        sid = bcm56880_pair_sid[UNPAIR_TO_PAIR_TCAM_NUM(t_num)];
    } else {
        if (map_info->sid_type == SID_TYPE_TCAM) {
            view = UNPAIR_IDX_TCAM_ONLY_VIEW;
        } else if (map_info->sid_type == SID_TYPE_DATA) {
            if (map_info->wide == WIDE_HALF) {
                depth *= 2;
            }
            view = UNPAIR_IDX_DATA_ONLY_VIEW;
        } else {
            return SHR_E_INTERNAL;
        }
        sid = bcm56880_unpair_sid[(map_info->index / depth)];
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
bcm56880_l1_sid_index_reverse_map(int u, int m, bcmdrd_sid_t hw_sid, alpm_sid_index_out_t *out_info)
{
    int i;
    int depth = BCM56880_TCAM_DEPTH;

    out_info->bank_depth = depth;

    for (i = 0; i < UNPAIR_TCAM_COUNT; i++) {
        if (hw_sid == bcm56880_unpair_sid[i][UNPAIR_IDX_TCAM_ONLY_VIEW]) {
            out_info->base_sid = bcm56880_unpair_sid[0][UNPAIR_IDX_TCAM_ONLY_VIEW];
            out_info->base_index = i * depth;
            return SHR_E_NONE;
        }
        if (hw_sid == bcm56880_unpair_sid[i][UNPAIR_IDX_DATA_ONLY_VIEW]) {
            out_info->base_sid = bcm56880_unpair_sid[0][UNPAIR_IDX_DATA_ONLY_VIEW];
            out_info->base_index = i * depth;
            return SHR_E_NONE;
        }
        if (hw_sid == bcm56880_unpair_sid[i][UNPAIR_IDX_COMBINED_VIEW]) {
            out_info->base_sid = bcm56880_unpair_sid[0][UNPAIR_IDX_COMBINED_VIEW];
            out_info->base_index = i * depth;
            return SHR_E_NONE;
        }
    }

    for (i = 0; i < PAIR_TCAM_COUNT; i++) {
        if (hw_sid == bcm56880_pair_sid[i][PAIR_IDX_COMBINED_VIEW]) {
            out_info->base_sid = bcm56880_pair_sid[0][PAIR_IDX_COMBINED_VIEW];
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
bcm56880_device_info_init(int u, int m, alpm_dev_info_t *dev)
{
    bcmlrd_sid_t ltid;
    bcmlrd_sid_t fid;
    int index_min, index_max;
    int rv = SHR_E_NONE;
    int bits;
    const bcmptm_alpm_variant_info_t *info = bcmptm_rm_alpm_variant_get(u, ALPM_0);

    SHR_FUNC_ENTER(u);

    if (dev) {
        if (info && info->lpm_ipv4) {
            bits = info->lpm_ipv4->misc_end_bit -
                   info->lpm_ipv4->misc_start_bit + 1;
            bcm56880_alpm_max_vrf[u] = (1 << bits) - 1;
        } else {
            bcm56880_alpm_max_vrf[u] = BCM56880_ALPM_DEFAULT_MAX_VRF;
        }
        dev->max_vrf = bcm56880_alpm_max_vrf[u];
        dev->max_fp_op = ALPM_MAX_FP_OP;
        dev->tcam_blocks = 8;
        dev->l1_blocks = 4;
        dev->l1_tcams_shared = 1;
        dev->tcam_depth = BCM56880_TCAM_DEPTH * 2;

        dev->pair_sid = MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m;
        dev->unpair_sid = MEMDB_TCAM_IFTA80_MEM0_0m;
        dev->tcam_only_sid = MEMDB_TCAM_IFTA80_MEM0_0_ONLYm;
        dev->data_only_sid = MEMDB_TCAM_IFTA80_MEM0_0_DATA_ONLYm;
        dev->l2_sid = L3_DEFIP_ALPM_LEVEL2_B0m;
        dev->l3_sid = L3_DEFIP_ALPM_LEVEL3_B0m;
        dev->feature_pkms = 8;
        dev->feature_cache_dt = 1;
        dev->feature_urpf = 1;
        dev->pivot_max_format_0 = 5;
        dev->route_max_format_0 = 10;
        dev->pivot_max_format_1 = 8;
        dev->route_max_format_1 = 13;
        dev->pivot_min_format_0 = 1;
        dev->route_min_format_0 = 1;
        dev->pivot_min_format_1 = 6;
        dev->route_min_format_1 = 11;
        dev->l2p_route_fmts = &bcm56880_l2p_route_fmts[0];
        dev->w_size = sizeof(PAIR_T);
        dev->n_size = sizeof(UNPAIR_T);
        dev->alpm_ver = m ? ALPM_VERSION_NONE : ALPM_VERSION_1;
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
        bcm56880_l1_wide_template_init(u);
        bcm56880_l1_narrow_template_init(u);

        index_min = bcmdrd_pt_index_min(u, L3_DEFIP_ALPM_LEVEL2_B0m);
        index_max = bcmdrd_pt_index_max(u, L3_DEFIP_ALPM_LEVEL2_B0m);
        bcm56880_l2_bucket_mask[u] = index_max - index_min;
        bcm56880_l2_bucket_bits[u] = shr_util_log2(index_max - index_min + 1);

        index_min = bcmdrd_pt_index_min(u, L3_DEFIP_ALPM_LEVEL3_B0m);
        index_max = bcmdrd_pt_index_max(u, L3_DEFIP_ALPM_LEVEL3_B0m);
        bcm56880_l3_bucket_mask[u] = index_max - index_min;
        bcm56880_l3_bucket_bits[u] = shr_util_log2(index_max - index_min + 1);

        /* Get the wide mode enum value as defined by NPL */
        rv = bcmlrd_table_field_name_to_idx(u, "L3_IPV4_UC_ROUTE", "DATA_TYPE",
                                            &ltid, &fid);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
        if (rv != SHR_E_NOT_FOUND) {
            bcmlrd_resolve_enum(u, ltid, fid, "WIDE_MODE", &wide_mode[u]);
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
bcm56880_a0_rm_alpm_driver_register(int u, bcmptm_rm_alpm_driver_t *drv)
{
    SHR_FUNC_ENTER(u);

    if (drv) {
        drv->l1_encode = bcm56880_l1_encode;
        drv->l1_decode = bcm56880_l1_decode;
        drv->l1_half_entry_move = bcm56880_l1_half_entry_move;
        drv->ln_base_entry_decode = bcm56880_ln_base_entry_decode;
        drv->ln_base_entry_encode = bcm56880_ln_base_entry_encode;
        drv->ln_raw_bucket_decode = bcm56880_ln_raw_bucket_decode;
        drv->ln_raw_bucket_encode = bcm56880_ln_raw_bucket_encode;
        drv->ln_raw_bucket_entry_get = bcm56880_ln_raw_bucket_entry_get;
        drv->ln_raw_bucket_entry_set = bcm56880_ln_raw_bucket_entry_set;
        drv->ln_base_entry_info = bcm56880_ln_base_entry_info;
        drv->ln_base_entry_move = bcm56880_ln_base_entry_move;
        drv->device_info_init   = bcm56880_device_info_init;
        drv->l1_sid_index_map = bcm56880_l1_sid_index_map;
        drv->ln_sid_index_map = bcm56880_ln_sid_index_map;
        drv->l1_half_key_clear = bcm56880_l1_half_key_clear;
        drv->l1_half_key_move = bcm56880_l1_half_key_move;
        drv->l1_sid_index_reverse_map = bcm56880_l1_sid_index_reverse_map;

    } else {

        SHR_ERR_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}


