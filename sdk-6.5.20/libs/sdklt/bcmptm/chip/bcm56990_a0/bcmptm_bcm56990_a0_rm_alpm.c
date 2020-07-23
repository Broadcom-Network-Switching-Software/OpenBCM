/*! \file bcmptm_bcm56990_a0_rm_alpm.c
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
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/chip/bcm56990_a0_defs.h>

#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_rm_alpm_internal.h>


#include "bcmptm_rm_alpm_common.h"
#include "bcmptm_bcm56990_a0_rm_alpm.h"


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMALPM
#define BCM56990_ALPM_MAX_VRF   8191
#define BCM56990_ALPM_VRF_MASK  8191
#define BCM56990_ALPM_MAX_COMP_PORT     0xFFFF
#define BCM56990_ALPM_COMP_PORT_MASK    0xFFFF

#define FMTX_KT_LPM_V4 0
#define FMTX_KT_LPM_V6 1
#define FMT0_KT_COMP_V4 0
#define FMT0_KT_COMP_V6 1
#define FMT1_KT_L3MC_V4 2
#define FMT1_KT_L3MC_V6 3

#define KSHIFT_PATCH1(u, arg, v) ((v) + bcm56990_kshift(u, arg))
#define KSHIFT_PATCH2(u, arg, v) ((v) -= bcm56990_kshift(u, arg))
#define UNPAIR0_T   L3_DEFIP_LEVEL1m_t
#define UNPAIR1_T   L3_DEFIP_LEVEL1_WIDEm_t
#define PAIR0_T     L3_DEFIP_PAIR_LEVEL1m_t
#define PAIR1_T     L3_DEFIP_PAIR_LEVEL1_WIDEm_t
#define L2_T        L3_DEFIP_ALPM_LEVEL2m_t
#define L3_T        L3_DEFIP_ALPM_LEVEL3m_t

#define SUB_DB_PRIORITY_OVERRIDE    0
#define SUB_DB_PRIORITY_VRF         1
#define SUB_DB_PRIORITY_GLOBAL      2

/* L3_DEFIP_TCAM_KEY_FMT0 */
#define FMT0_TCAM_KEY_FIELD_SET(_e, _f, _v)  L3_DEFIP_TCAM_KEY_FMT0_##_f##_SET(*_e, (_v))
#define FMT0_TCAM_KEY_FIELD_GET(_e, _f, _v)  (_v) = L3_DEFIP_TCAM_KEY_FMT0_##_f##_GET(*_e)
#define FMT0_TCAM_KEY_FIELD_SET2(_e, _f, _v) L3_DEFIP_TCAM_KEY_FMT0_##_f##_SET(*_e, (uint32_t *)&(_v)[0])
#define FMT0_TCAM_KEY_FIELD_GET2(_e, _f, _v) L3_DEFIP_TCAM_KEY_FMT0_##_f##_GET(*_e, (uint32_t *)&(_v)[0])

/* L3_DEFIP_TCAM_KEY_FMT1 */
#define FMT1_TCAM_KEY_FIELD_SET(_e, _f, _v)  L3_DEFIP_TCAM_KEY_FMT1_##_f##_SET(*_e, (_v))
#define FMT1_TCAM_KEY_FIELD_GET(_e, _f, _v)  (_v) = L3_DEFIP_TCAM_KEY_FMT1_##_f##_GET(*_e)
#define FMT1_TCAM_KEY_FIELD_SET2(_e, _f, _v) L3_DEFIP_TCAM_KEY_FMT1_##_f##_SET(*_e, (uint32_t *)&(_v)[0])
#define FMT1_TCAM_KEY_FIELD_GET2(_e, _f, _v) L3_DEFIP_TCAM_KEY_FMT1_##_f##_GET(*_e, (uint32_t *)&(_v)[0])


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


/* L3_DEFIP_LEVEL1m */
#define UNPAIR0_FIELD_SET(_f, _v)       L3_DEFIP_LEVEL1m_##_f##_SET(*de, (_v))
#define UNPAIR0_FIELD_GET(_f, _v)       (_v) = L3_DEFIP_LEVEL1m_##_f##_GET(*de)
#define UNPAIR0_FIELD_SET2(_f, _v)      L3_DEFIP_LEVEL1m_##_f##_SET(*de, (uint32_t *)&(_v))
#define UNPAIR0_FIELD_GET2(_f, _v)      L3_DEFIP_LEVEL1m_##_f##_GET(*de, (uint32_t *)&(_v))
#define UNPAIR0_SRC_FIELD_SET(_f, _v)   L3_DEFIP_LEVEL1m_##_f##_SET(*se, (_v))
#define UNPAIR0_SRC_FIELD_GET(_f, _v)   (_v) = L3_DEFIP_LEVEL1m_##_f##_GET(*se)
#define UNPAIR0_SRC_FIELD_SET2(_f, _v)  L3_DEFIP_LEVEL1m_##_f##_SET(*se, (uint32_t *)&(_v))
#define UNPAIR0_SRC_FIELD_GET2(_f, _v)  L3_DEFIP_LEVEL1m_##_f##_GET(*se, (uint32_t *)&(_v))

/* L3_DEFIP_PAIR_LEVEL1m  */
#define PAIR0_FIELD_SET(_f, _v)      L3_DEFIP_PAIR_LEVEL1m_##_f##_SET(*de, (_v))
#define PAIR0_FIELD_GET(_f, _v)      (_v) = L3_DEFIP_PAIR_LEVEL1m_##_f##_GET(*de)
#define PAIR0_FIELD_SET2(_f, _v)     L3_DEFIP_PAIR_LEVEL1m_##_f##_SET(*de, (uint32_t *)&(_v))
#define PAIR0_FIELD_GET2(_f, _v)     L3_DEFIP_PAIR_LEVEL1m_##_f##_GET(*de, (uint32_t *)&(_v))
#define PAIR0_SRC_FIELD_SET(_f, _v)  L3_DEFIP_PAIR_LEVEL1m_##_f##_SET(*se, (_v))
#define PAIR0_SRC_FIELD_GET(_f, _v)  (_v) = L3_DEFIP_PAIR_LEVEL1m_##_f##_GET(*se)
#define PAIR0_SRC_FIELD_SET2(_f, _v) L3_DEFIP_PAIR_LEVEL1m_##_f##_SET(*se, (uint32_t *)&(_v))
#define PAIR0_SRC_FIELD_GET2(_f, _v) L3_DEFIP_PAIR_LEVEL1m_##_f##_GET(*se, (uint32_t *)&(_v))

/* L3_DEFIP_LEVEL1_WIDEm field set */
#define UNPAIR1_FIELD_SET(_f, _v)       L3_DEFIP_LEVEL1_WIDEm_##_f##_SET(*de, (_v))
#define UNPAIR1_FIELD_GET(_f, _v)       (_v) = L3_DEFIP_LEVEL1_WIDEm_##_f##_GET(*de)
#define UNPAIR1_FIELD_SET2(_f, _v)      L3_DEFIP_LEVEL1_WIDEm_##_f##_SET(*de, (uint32_t *)&(_v))
#define UNPAIR1_FIELD_GET2(_f, _v)      L3_DEFIP_LEVEL1_WIDEm_##_f##_GET(*de, (uint32_t *)&(_v))
#define UNPAIR1_SRC_FIELD_SET(_f, _v)   L3_DEFIP_LEVEL1_WIDEm_##_f##_SET(*se, (_v))
#define UNPAIR1_SRC_FIELD_GET(_f, _v)   (_v) = L3_DEFIP_LEVEL1_WIDEm_##_f##_GET(*se)
#define UNPAIR1_SRC_FIELD_SET2(_f, _v)  L3_DEFIP_LEVEL1_WIDEm_##_f##_SET(*se, (uint32_t *)&(_v))
#define UNPAIR1_SRC_FIELD_GET2(_f, _v)  L3_DEFIP_LEVEL1_WIDEm_##_f##_GET(*se, (uint32_t *)&(_v))

/* L3_DEFIP_PAIR_LEVEL1_WIDEm field set */
#define PAIR1_FIELD_SET(_f, _v)       L3_DEFIP_PAIR_LEVEL1_WIDEm_##_f##_SET(*de, (_v))
#define PAIR1_FIELD_GET(_f, _v)       (_v) = L3_DEFIP_PAIR_LEVEL1_WIDEm_##_f##_GET(*de)
#define PAIR1_FIELD_SET2(_f, _v)      L3_DEFIP_PAIR_LEVEL1_WIDEm_##_f##_SET(*de, (uint32_t *)&(_v))
#define PAIR1_FIELD_GET2(_f, _v)      L3_DEFIP_PAIR_LEVEL1_WIDEm_##_f##_GET(*de, (uint32_t *)&(_v))
#define PAIR1_SRC_FIELD_SET(_f, _v)   L3_DEFIP_PAIR_LEVEL1_WIDEm_##_f##_SET(*se, (_v))
#define PAIR1_SRC_FIELD_GET(_f, _v)   (_v) = L3_DEFIP_PAIR_LEVEL1_WIDEm_##_f##_GET(*se)
#define PAIR1_SRC_FIELD_SET2(_f, _v)  L3_DEFIP_PAIR_LEVEL1_WIDEm_##_f##_SET(*se, (uint32_t *)&(_v))
#define PAIR1_SRC_FIELD_GET2(_f, _v)  L3_DEFIP_PAIR_LEVEL1_WIDEm_##_f##_GET(*se, (uint32_t *)&(_v))

#define HALF_WIDE   0
#define SINGLE_WIDE 1
#define DOUBLE_WIDE 3

/*******************************************************************************
 * Typedefs
 */

typedef struct format_info_s {
    uint16_t prefix_len;
    uint8_t full;
    uint8_t num_entries;
} format_info_t;

typedef union bcm56990_assoc_data_s {
    ASSOC_DATA_FULL_t full;
    ASSOC_DATA_REDUCED_t red;
    uint32_t v[2];
} bcm56990_assoc_data_t;

/*******************************************************************************
 * Private variables
 */
extern shr_lmm_hdl_t bcmptm_rm_alpm_arg_lmem[BCMDRD_CONFIG_MAX_UNITS][ALPMS];

/* Mapping logical formats to physical formats. */
static int bcm56990_l2p_route_fmts[] =
    {0, 13, 1, 2, 3, 4, 5, 6, 7, 8, 9, 14, 10, 11, 12, 15};

/* Mapping physical formats to logical formats. */
static int bcm56990_p2l_route_fmts[] =
    {0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 13, 14, 1, 11, 15};

static format_info_t bcm56990_route_base_entry_info[] = {
    {.prefix_len = 0,   .full = 0, .num_entries = 0},   /* 0 (L) - 0 (P) */
    {.prefix_len = 8,   .full = 0, .num_entries = 20},  /* 1 - 13 */
    {.prefix_len = 16,  .full = 0, .num_entries = 15},  /* 2 - 1 */
    {.prefix_len = 24,  .full = 0, .num_entries = 12},  /* 3 - 2 */
    {.prefix_len = 32,  .full = 0, .num_entries = 10},  /* 4 - 3 */
    {.prefix_len = 40,  .full = 0, .num_entries = 8},   /* 5 - 4 */
    {.prefix_len = 48,  .full = 0, .num_entries = 7},   /* 6 - 5 */
    {.prefix_len = 64,  .full = 0, .num_entries = 6},   /* 7 - 6 */
    {.prefix_len = 80,  .full = 0, .num_entries = 5},   /* 8 - 7 */
    {.prefix_len = 104, .full = 0, .num_entries = 4},   /* 9 - 8 */
    {.prefix_len = 144, .full = 0, .num_entries = 3},   /* 10 - 9 */
    {.prefix_len = 224, .full = 0, .num_entries = 2},   /* 11 - 14 */
    {.prefix_len = 31,  .full = 1, .num_entries = 6},   /* 12 - 10 */
    {.prefix_len = 71,  .full = 1, .num_entries = 4},   /* 13 - 11 */
    {.prefix_len = 191, .full = 1, .num_entries = 2},   /* 14 - 12 */
    {.prefix_len = 289, .full = 1, .num_entries = 1}    /* 15 - 15 */
};


/* Mapping logical formats to physical formats. */
static int bcm56990_l2p_pivot_fmts[] =
    {0, 1, 2, 3, 4, 5, 9, 6, 7, 8, 10};

/* Mapping physical formats to logical formats. */
static int bcm56990_p2l_pivot_fmts[] =
    {0, 1, 2, 3, 4, 5, 7, 8, 9, 6, 10};

static format_info_t bcm56990_pivot_base_entry_info[] = {
    {.prefix_len = 0,   .full = 0, .num_entries = 0}, /* 0 (L) - 0 (P) */
    {.prefix_len = 6,   .full = 0, .num_entries = 6}, /* 1 - 1 */
    {.prefix_len = 22,  .full = 0, .num_entries = 5}, /* 2 - 2 */
    {.prefix_len = 46,  .full = 0, .num_entries = 4}, /* 3 - 3 */
    {.prefix_len = 86,  .full = 0, .num_entries = 3}, /* 4 - 4 */
    {.prefix_len = 166, .full = 0, .num_entries = 2}, /* 5 - 5 */
    {.prefix_len = 289, .full = 0, .num_entries = 1}, /* 6 - 9 */
    {.prefix_len = 13,  .full = 1, .num_entries = 4}, /* 7 - 6 */
    {.prefix_len = 53,  .full = 1, .num_entries = 3}, /* 8 - 7 */
    {.prefix_len = 133, .full = 1, .num_entries = 2}, /* 9 - 8 */
    {.prefix_len = 289, .full = 1, .num_entries = 1}  /* 10 - 10 */

};

typedef uint32_t L3_DEFIP_TCAM_KEY_FMT0_KEY_t[2]; /* 45 bits */

static void
bcm56990_l1_lpm_vt_decode(int u, int sub_prio, int vrf,
                          int vrf_mask, alpm_arg_t *arg)
{
    if (sub_prio == SUB_DB_PRIORITY_OVERRIDE) {
        ALPM_ASSERT(vrf_mask == 0);
        arg->key.vt = VRF_OVERRIDE;
    } else if (sub_prio == SUB_DB_PRIORITY_GLOBAL) {
        ALPM_ASSERT(arg->lpm_large_vrf || vrf_mask == 0);
        arg->key.vt = VRF_GLOBAL;
    } else {
        ALPM_ASSERT(arg->lpm_large_vrf || vrf_mask == BCM56990_ALPM_VRF_MASK);
        ALPM_ASSERT(arg->lpm_large_vrf || vrf <= BCM56990_ALPM_MAX_VRF);
        arg->key.vt = VRF_PRIVATE;
        if (!arg->lpm_large_vrf) {
            arg->key.t.w_vrf = vrf;
        }
    }
}

static void
bcm56990_l1_comp_vt_decode(int u, bcmptm_cth_alpm_comp_key_type_t comp_key_type,
                           int vrf, int vrf_mask, alpm_arg_t *arg)
{
    switch (comp_key_type) {
        case COMP_KEY_L4_PORT:
            if (vrf_mask == BCM56990_ALPM_COMP_PORT_MASK) {
                arg->key.vt = VRF_PRIVATE;
                arg->key.t.w_vrf = vrf;
            } else {
                arg->key.vt = VRF_GLOBAL;
            }
            break;
        case COMP_KEY_VRF:
            if (vrf_mask == BCM56990_ALPM_VRF_MASK) {
                arg->key.vt = VRF_PRIVATE;
                arg->key.t.w_vrf = vrf;
            } else {
                arg->key.vt = VRF_GLOBAL;
            }
            break;
        case COMP_KEY_FULL:
            arg->key.vt = VRF_PRIVATE;
            break;
        default:
            break;
    }
}

static void
bcm56990_l1_key_fmt0_lpm_v4_encode(int u, int vrf, int key_mode,
                                   alpm_ip_ver_t v6, uint32_t ip_addr,
                                   L3_DEFIP_TCAM_KEY_FMT0_t *kdata)
{
    LPM_V4_KEY_t lpm_v4 = {{0}};
    L3_DEFIP_TCAM_KEY_FMT0_LPM_V4_KEY_t v4_key = {{0}};
    uint32_t *key = &v4_key.v[0];

    LPM_V4_KEY_VRFf_SET(lpm_v4, vrf);
    LPM_V4_KEY_IP_ADDRf_SET(lpm_v4, ip_addr);

    LPM_V4_KEY_L3_DEFIP_TCAM_KEY_FMT0_HALF_BASE_ENTRY_0f_GET(lpm_v4, key);

    FMT0_TCAM_KEY_FIELD_SET(kdata, KEY_MODEf, key_mode);
    FMT0_TCAM_KEY_FIELD_SET(kdata, KEY_TYPEf, v6);
    FMT0_TCAM_KEY_FIELD_SET2(kdata, KEYf, key);
}

static void
bcm56990_l1_key_fmt0_lpm_v4_encode_no_vrf(int u, int key_mode,
                                          alpm_ip_ver_t v6, uint32_t *udata,
                                          L3_DEFIP_TCAM_KEY_FMT0_t *kdata)
{

    LPM_V4_KEY_t *lpm_v4 = (LPM_V4_KEY_t *)udata;
    L3_DEFIP_TCAM_KEY_FMT0_LPM_V4_KEY_t v4_key = {{0}};
    uint32_t *key = &v4_key.v[0];

    LPM_V4_KEY_L3_DEFIP_TCAM_KEY_FMT0_HALF_BASE_ENTRY_0f_GET(*lpm_v4, key);

    FMT0_TCAM_KEY_FIELD_SET(kdata, KEY_MODEf, key_mode);
    FMT0_TCAM_KEY_FIELD_SET(kdata, KEY_TYPEf, v6);
    FMT0_TCAM_KEY_FIELD_SET2(kdata, KEYf, key);
}


static void
bcm56990_l1_key_fmt0_lpm_v6_double_encode(int u, int vrf, int key_mode,
                                          alpm_ip_ver_t v6, uint32_t *ip_addr,
                                          L3_DEFIP_TCAM_KEY_FMT0_t *kdata0,
                                          L3_DEFIP_TCAM_KEY_FMT0_t *kdata1,
                                          L3_DEFIP_TCAM_KEY_FMT0_t *kdata2,
                                          L3_DEFIP_TCAM_KEY_FMT0_t *kdata3
                                          )
{
    LPM_V6_KEY_t lpm_v6 = {{0}};
    L3_DEFIP_TCAM_KEY_FMT0_LPM_V6_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    LPM_V6_KEY_VRFf_SET(lpm_v6, vrf);
    LPM_V6_KEY_IP_ADDRf_SET(lpm_v6, ip_addr);

    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_0f_GET(lpm_v6, key);
    FMT0_TCAM_KEY_FIELD_SET(kdata0, KEY_MODEf, key_mode);
    FMT0_TCAM_KEY_FIELD_SET(kdata0, KEY_TYPEf, v6);
    FMT0_TCAM_KEY_FIELD_SET2(kdata0, KEYf, key);

    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_1f_GET(lpm_v6, key);
    FMT0_TCAM_KEY_FIELD_SET(kdata1, KEY_MODEf, key_mode);
    FMT0_TCAM_KEY_FIELD_SET(kdata1, KEY_TYPEf, v6);
    FMT0_TCAM_KEY_FIELD_SET2(kdata1, KEYf, key);

    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_2f_GET(lpm_v6, key);
    FMT0_TCAM_KEY_FIELD_SET(kdata2, KEY_MODEf, key_mode);
    FMT0_TCAM_KEY_FIELD_SET(kdata2, KEY_TYPEf, v6);
    FMT0_TCAM_KEY_FIELD_SET2(kdata2, KEYf, key);

    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_3f_GET(lpm_v6, key);
    FMT0_TCAM_KEY_FIELD_SET(kdata3, KEY_MODEf, key_mode);
    FMT0_TCAM_KEY_FIELD_SET(kdata3, KEY_TYPEf, v6);
    FMT0_TCAM_KEY_FIELD_SET2(kdata3, KEYf, key);
}


static void
bcm56990_l1_key_fmt0_lpm_v6_double_encode_no_vrf(int u, int key_mode,
                                                 alpm_ip_ver_t v6, uint32_t *udata,
                                                 L3_DEFIP_TCAM_KEY_FMT0_t *kdata0,
                                                 L3_DEFIP_TCAM_KEY_FMT0_t *kdata1,
                                                 L3_DEFIP_TCAM_KEY_FMT0_t *kdata2,
                                                 L3_DEFIP_TCAM_KEY_FMT0_t *kdata3
                                                )
{
    LPM_V6_KEY_t *lpm_v6 = (LPM_V6_KEY_t *)udata;
    L3_DEFIP_TCAM_KEY_FMT0_LPM_V6_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_0f_GET(*lpm_v6, key);
    FMT0_TCAM_KEY_FIELD_SET(kdata0, KEY_MODEf, key_mode);
    FMT0_TCAM_KEY_FIELD_SET(kdata0, KEY_TYPEf, v6);
    FMT0_TCAM_KEY_FIELD_SET2(kdata0, KEYf, key);

    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_1f_GET(*lpm_v6, key);
    FMT0_TCAM_KEY_FIELD_SET(kdata1, KEY_MODEf, key_mode);
    FMT0_TCAM_KEY_FIELD_SET(kdata1, KEY_TYPEf, v6);
    FMT0_TCAM_KEY_FIELD_SET2(kdata1, KEYf, key);

    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_2f_GET(*lpm_v6, key);
    FMT0_TCAM_KEY_FIELD_SET(kdata2, KEY_MODEf, key_mode);
    FMT0_TCAM_KEY_FIELD_SET(kdata2, KEY_TYPEf, v6);
    FMT0_TCAM_KEY_FIELD_SET2(kdata2, KEYf, key);

    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_3f_GET(*lpm_v6, key);
    FMT0_TCAM_KEY_FIELD_SET(kdata3, KEY_MODEf, key_mode);
    FMT0_TCAM_KEY_FIELD_SET(kdata3, KEY_TYPEf, v6);
    FMT0_TCAM_KEY_FIELD_SET2(kdata3, KEYf, key);
}



static void
bcm56990_l1_key_fmt0_lpm_v6_single_encode(int u, int vrf, int key_mode,
                                          alpm_ip_ver_t v6, uint32_t *ip_addr,
                                          L3_DEFIP_TCAM_KEY_FMT0_t *kdata0,
                                          L3_DEFIP_TCAM_KEY_FMT0_t *kdata1
                                          )
{
    LPM_V6_KEY_t lpm_v6 = {{0}};
    L3_DEFIP_TCAM_KEY_FMT0_LPM_V6_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    LPM_V6_KEY_VRFf_SET(lpm_v6, vrf);
    LPM_V6_KEY_IP_ADDRf_SET(lpm_v6, ip_addr);

    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_SINGLE_BASE_ENTRY_0f_GET(lpm_v6, key);
    FMT0_TCAM_KEY_FIELD_SET(kdata0, KEY_MODEf, key_mode);
    FMT0_TCAM_KEY_FIELD_SET(kdata0, KEY_TYPEf, v6);
    FMT0_TCAM_KEY_FIELD_SET2(kdata0, KEYf, key);

    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_SINGLE_BASE_ENTRY_1f_GET(lpm_v6, key);
    FMT0_TCAM_KEY_FIELD_SET(kdata1, KEY_MODEf, key_mode);
    FMT0_TCAM_KEY_FIELD_SET(kdata1, KEY_TYPEf, v6);
    FMT0_TCAM_KEY_FIELD_SET2(kdata1, KEYf, key);
}


static void
bcm56990_l1_key_fmt0_lpm_v6_single_encode_no_vrf(int u, int key_mode,
                                                 alpm_ip_ver_t v6, uint32_t *udata,
                                                 L3_DEFIP_TCAM_KEY_FMT0_t *kdata0,
                                                 L3_DEFIP_TCAM_KEY_FMT0_t *kdata1
                                                 )
{
    LPM_V6_KEY_t *lpm_v6 = (LPM_V6_KEY_t *) udata;
    L3_DEFIP_TCAM_KEY_FMT0_LPM_V6_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_SINGLE_BASE_ENTRY_0f_GET(*lpm_v6, key);
    FMT0_TCAM_KEY_FIELD_SET(kdata0, KEY_MODEf, key_mode);
    FMT0_TCAM_KEY_FIELD_SET(kdata0, KEY_TYPEf, v6);
    FMT0_TCAM_KEY_FIELD_SET2(kdata0, KEYf, key);

    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_SINGLE_BASE_ENTRY_1f_GET(*lpm_v6, key);
    FMT0_TCAM_KEY_FIELD_SET(kdata1, KEY_MODEf, key_mode);
    FMT0_TCAM_KEY_FIELD_SET(kdata1, KEY_TYPEf, v6);
    FMT0_TCAM_KEY_FIELD_SET2(kdata1, KEYf, key);
}


/* For V4, V6-0 */
static void
bcm56990_l1_key_fmt0_lpm_v4_decode(int u, int *vrf, int *key_mode,
                                   int *kt, uint32_t *ip_addr,
                                   L3_DEFIP_TCAM_KEY_FMT0_t *kdata)
{
    LPM_V4_KEY_t lpm_v4 = {{0}};
    L3_DEFIP_TCAM_KEY_FMT0_LPM_V4_KEY_t v4_key = {{0}};
    uint32_t *key = &v4_key.v[0];

    if (key_mode) {
        FMT0_TCAM_KEY_FIELD_GET(kdata, KEY_MODEf, *key_mode);
    }
    if (kt) {
        FMT0_TCAM_KEY_FIELD_GET(kdata, KEY_TYPEf, *kt);
    }
    FMT0_TCAM_KEY_FIELD_GET2(kdata, KEYf, key);
    LPM_V4_KEY_L3_DEFIP_TCAM_KEY_FMT0_HALF_BASE_ENTRY_0f_SET(lpm_v4, key);

    if (ip_addr) {
        *ip_addr = LPM_V4_KEY_IP_ADDRf_GET(lpm_v4);
    }
    if (vrf) {
        *vrf = LPM_V4_KEY_VRFf_GET(lpm_v4);
    }
}

/* For V4, V6-0 */
static void
bcm56990_l1_key_fmt0_lpm_v4_decode_no_vrf(int u, int *key_mode,
                                          int *kt, uint32_t *udata,
                                          L3_DEFIP_TCAM_KEY_FMT0_t *kdata)
{
    LPM_V4_KEY_t *lpm_v4 = (LPM_V4_KEY_t *)udata;
    L3_DEFIP_TCAM_KEY_FMT0_LPM_V4_KEY_t v4_key = {{0}};
    uint32_t *key = &v4_key.v[0];

    if (key_mode) {
        FMT0_TCAM_KEY_FIELD_GET(kdata, KEY_MODEf, *key_mode);
    }
    if (kt) {
        FMT0_TCAM_KEY_FIELD_GET(kdata, KEY_TYPEf, *kt);
    }
    FMT0_TCAM_KEY_FIELD_GET2(kdata, KEYf, key);
    LPM_V4_KEY_L3_DEFIP_TCAM_KEY_FMT0_HALF_BASE_ENTRY_0f_SET(*lpm_v4, key);

}



static void
bcm56990_l1_key_fmt0_lpm_v6_double_decode(int u, int *vrf, int *key_mode,
                                          int *kt, uint32_t *ip_addr,
                                          L3_DEFIP_TCAM_KEY_FMT0_t *kdata0,
                                          L3_DEFIP_TCAM_KEY_FMT0_t *kdata1,
                                          L3_DEFIP_TCAM_KEY_FMT0_t *kdata2,
                                          L3_DEFIP_TCAM_KEY_FMT0_t *kdata3)
{
    LPM_V6_KEY_t lpm_v6 = {{0}};
    L3_DEFIP_TCAM_KEY_FMT0_LPM_V6_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    if (key_mode) {
        FMT0_TCAM_KEY_FIELD_GET(kdata0, KEY_MODEf, *key_mode);
    }
    if (kt) {
        FMT0_TCAM_KEY_FIELD_GET(kdata0, KEY_TYPEf, *kt);
    }
    FMT0_TCAM_KEY_FIELD_GET2(kdata0, KEYf, key);
    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_0f_SET(lpm_v6, key);
    FMT0_TCAM_KEY_FIELD_GET2(kdata1, KEYf, key);
    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_1f_SET(lpm_v6, key);
    FMT0_TCAM_KEY_FIELD_GET2(kdata2, KEYf, key);
    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_2f_SET(lpm_v6, key);
    FMT0_TCAM_KEY_FIELD_GET2(kdata3, KEYf, key);
    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_3f_SET(lpm_v6, key);

    if (ip_addr) {
        LPM_V6_KEY_IP_ADDRf_GET(lpm_v6, ip_addr);
    }
    if (vrf) {
        *vrf = LPM_V6_KEY_VRFf_GET(lpm_v6);
    }
}


static void
bcm56990_l1_key_fmt0_lpm_v6_double_decode_no_vrf(int u, int *key_mode,
                                                 int *kt, uint32_t *udata,
                                                 L3_DEFIP_TCAM_KEY_FMT0_t *kdata0,
                                                 L3_DEFIP_TCAM_KEY_FMT0_t *kdata1,
                                                 L3_DEFIP_TCAM_KEY_FMT0_t *kdata2,
                                                 L3_DEFIP_TCAM_KEY_FMT0_t *kdata3)
{
    LPM_V6_KEY_t *lpm_v6 = (LPM_V6_KEY_t *)udata;
    L3_DEFIP_TCAM_KEY_FMT0_LPM_V6_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    if (key_mode) {
        FMT0_TCAM_KEY_FIELD_GET(kdata0, KEY_MODEf, *key_mode);
    }
    if (kt) {
        FMT0_TCAM_KEY_FIELD_GET(kdata0, KEY_TYPEf, *kt);
    }
    FMT0_TCAM_KEY_FIELD_GET2(kdata0, KEYf, key);
    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_0f_SET(*lpm_v6, key);
    FMT0_TCAM_KEY_FIELD_GET2(kdata1, KEYf, key);
    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_1f_SET(*lpm_v6, key);
    FMT0_TCAM_KEY_FIELD_GET2(kdata2, KEYf, key);
    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_2f_SET(*lpm_v6, key);
    FMT0_TCAM_KEY_FIELD_GET2(kdata3, KEYf, key);
    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_3f_SET(*lpm_v6, key);

}


static void
bcm56990_l1_key_fmt0_lpm_v6_single_decode(int u, int *vrf, int *key_mode,
                                          int *kt, uint32_t *ip_addr,
                                          L3_DEFIP_TCAM_KEY_FMT0_t *kdata0,
                                          L3_DEFIP_TCAM_KEY_FMT0_t *kdata1)
{
    LPM_V6_KEY_t lpm_v6 = {{0}};
    L3_DEFIP_TCAM_KEY_FMT0_LPM_V6_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    if (key_mode) {
        FMT0_TCAM_KEY_FIELD_GET(kdata0, KEY_MODEf, *key_mode);
    }
    if (kt) {
        FMT0_TCAM_KEY_FIELD_GET(kdata0, KEY_TYPEf, *kt);
    }
    FMT0_TCAM_KEY_FIELD_GET2(kdata0, KEYf, key);
    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_SINGLE_BASE_ENTRY_0f_SET(lpm_v6, key);
    FMT0_TCAM_KEY_FIELD_GET2(kdata1, KEYf, key);
    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_SINGLE_BASE_ENTRY_1f_SET(lpm_v6, key);
    if (ip_addr) {
        LPM_V6_KEY_IP_ADDRf_GET(lpm_v6, ip_addr);
    }
    if (vrf) {
        *vrf = LPM_V6_KEY_VRFf_GET(lpm_v6);
    }
}


static void
bcm56990_l1_key_fmt0_lpm_v6_single_decode_no_vrf(int u, int *key_mode,
                                                 int *kt, uint32_t *udata,
                                                 L3_DEFIP_TCAM_KEY_FMT0_t *kdata0,
                                                 L3_DEFIP_TCAM_KEY_FMT0_t *kdata1)
{

    LPM_V6_KEY_t *lpm_v6 = (LPM_V6_KEY_t *)udata;
    L3_DEFIP_TCAM_KEY_FMT0_LPM_V6_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    if (key_mode) {
        FMT0_TCAM_KEY_FIELD_GET(kdata0, KEY_MODEf, *key_mode);
    }
    if (kt) {
        FMT0_TCAM_KEY_FIELD_GET(kdata0, KEY_TYPEf, *kt);
    }
    FMT0_TCAM_KEY_FIELD_GET2(kdata0, KEYf, key);
    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_SINGLE_BASE_ENTRY_0f_SET(*lpm_v6, key);
    FMT0_TCAM_KEY_FIELD_GET2(kdata1, KEYf, key);
    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_SINGLE_BASE_ENTRY_1f_SET(*lpm_v6, key);
}



static void
bcm56990_l1_key_fmt1_lpm_v4_encode(int u, int vrf, int key_mode,
                                   alpm_ip_ver_t v6, uint32_t ip_addr,
                                   L3_DEFIP_TCAM_KEY_FMT1_t *kdata)
{
    LPM_V4_KEY_t lpm_v4 = {{0}};
    L3_DEFIP_TCAM_KEY_FMT1_LPM_V4_KEY_t v4_key = {{0}};
    uint32_t *key = &v4_key.v[0];

    LPM_V4_KEY_VRFf_SET(lpm_v4, vrf);
    LPM_V4_KEY_IP_ADDRf_SET(lpm_v4, ip_addr);

    LPM_V4_KEY_L3_DEFIP_TCAM_KEY_FMT1_HALF_BASE_ENTRY_0f_GET(lpm_v4, key);

    FMT1_TCAM_KEY_FIELD_SET(kdata, KEY_MODEf, key_mode);
    FMT1_TCAM_KEY_FIELD_SET(kdata, KEY_TYPEf, v6);
    FMT1_TCAM_KEY_FIELD_SET2(kdata, KEYf, key);
}


static void
bcm56990_l1_key_fmt1_lpm_v4_encode_no_vrf(int u,  int key_mode,
                                          alpm_ip_ver_t v6, uint32_t *udata,
                                          L3_DEFIP_TCAM_KEY_FMT1_t *kdata)
{
    LPM_V4_KEY_t *lpm_v4 = (LPM_V4_KEY_t *)udata;
    L3_DEFIP_TCAM_KEY_FMT1_LPM_V4_KEY_t v4_key = {{0}};
    uint32_t *key = &v4_key.v[0];

    LPM_V4_KEY_L3_DEFIP_TCAM_KEY_FMT1_HALF_BASE_ENTRY_0f_GET(*lpm_v4, key);

    FMT1_TCAM_KEY_FIELD_SET(kdata, KEY_MODEf, key_mode);
    FMT1_TCAM_KEY_FIELD_SET(kdata, KEY_TYPEf, v6);
    FMT1_TCAM_KEY_FIELD_SET2(kdata, KEYf, key);
}


static void
bcm56990_l1_key_fmt1_lpm_v6_double_encode(int u, int vrf, int key_mode,
                                          alpm_ip_ver_t v6, uint32_t *ip_addr,
                                          L3_DEFIP_TCAM_KEY_FMT1_t *kdata0,
                                          L3_DEFIP_TCAM_KEY_FMT1_t *kdata1,
                                          L3_DEFIP_TCAM_KEY_FMT1_t *kdata2,
                                          L3_DEFIP_TCAM_KEY_FMT1_t *kdata3
                                          )
{
    LPM_V6_KEY_t lpm_v6 = {{0}};
    L3_DEFIP_TCAM_KEY_FMT1_LPM_V6_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    LPM_V6_KEY_VRFf_SET(lpm_v6, vrf);
    LPM_V6_KEY_IP_ADDRf_SET(lpm_v6, ip_addr);

    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_DOUBLE_BASE_ENTRY_0f_GET(lpm_v6, key);
    FMT1_TCAM_KEY_FIELD_SET(kdata0, KEY_MODEf, key_mode);
    FMT1_TCAM_KEY_FIELD_SET(kdata0, KEY_TYPEf, v6);
    FMT1_TCAM_KEY_FIELD_SET2(kdata0, KEYf, key);

    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_DOUBLE_BASE_ENTRY_1f_GET(lpm_v6, key);
    FMT1_TCAM_KEY_FIELD_SET(kdata1, KEY_MODEf, key_mode);
    FMT1_TCAM_KEY_FIELD_SET(kdata1, KEY_TYPEf, v6);
    FMT1_TCAM_KEY_FIELD_SET2(kdata1, KEYf, key);

    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_DOUBLE_BASE_ENTRY_2f_GET(lpm_v6, key);
    FMT1_TCAM_KEY_FIELD_SET(kdata2, KEY_MODEf, key_mode);
    FMT1_TCAM_KEY_FIELD_SET(kdata2, KEY_TYPEf, v6);
    FMT1_TCAM_KEY_FIELD_SET2(kdata2, KEYf, key);

    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_DOUBLE_BASE_ENTRY_3f_GET(lpm_v6, key);
    FMT1_TCAM_KEY_FIELD_SET(kdata3, KEY_MODEf, key_mode);
    FMT1_TCAM_KEY_FIELD_SET(kdata3, KEY_TYPEf, v6);
    FMT1_TCAM_KEY_FIELD_SET2(kdata3, KEYf, key);
}



static void
bcm56990_l1_key_fmt1_lpm_v6_double_encode_no_vrf(int u, int key_mode,
                                                 alpm_ip_ver_t v6, uint32_t *udata,
                                                 L3_DEFIP_TCAM_KEY_FMT1_t *kdata0,
                                                 L3_DEFIP_TCAM_KEY_FMT1_t *kdata1,
                                                 L3_DEFIP_TCAM_KEY_FMT1_t *kdata2,
                                                 L3_DEFIP_TCAM_KEY_FMT1_t *kdata3
                                                 )
{
    LPM_V6_KEY_t *lpm_v6 = (LPM_V6_KEY_t *)udata;
    L3_DEFIP_TCAM_KEY_FMT1_LPM_V6_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_DOUBLE_BASE_ENTRY_0f_GET(*lpm_v6, key);
    FMT1_TCAM_KEY_FIELD_SET(kdata0, KEY_MODEf, key_mode);
    FMT1_TCAM_KEY_FIELD_SET(kdata0, KEY_TYPEf, v6);
    FMT1_TCAM_KEY_FIELD_SET2(kdata0, KEYf, key);

    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_DOUBLE_BASE_ENTRY_1f_GET(*lpm_v6, key);
    FMT1_TCAM_KEY_FIELD_SET(kdata1, KEY_MODEf, key_mode);
    FMT1_TCAM_KEY_FIELD_SET(kdata1, KEY_TYPEf, v6);
    FMT1_TCAM_KEY_FIELD_SET2(kdata1, KEYf, key);

    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_DOUBLE_BASE_ENTRY_2f_GET(*lpm_v6, key);
    FMT1_TCAM_KEY_FIELD_SET(kdata2, KEY_MODEf, key_mode);
    FMT1_TCAM_KEY_FIELD_SET(kdata2, KEY_TYPEf, v6);
    FMT1_TCAM_KEY_FIELD_SET2(kdata2, KEYf, key);

    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_DOUBLE_BASE_ENTRY_3f_GET(*lpm_v6, key);
    FMT1_TCAM_KEY_FIELD_SET(kdata3, KEY_MODEf, key_mode);
    FMT1_TCAM_KEY_FIELD_SET(kdata3, KEY_TYPEf, v6);
    FMT1_TCAM_KEY_FIELD_SET2(kdata3, KEYf, key);
}


static void
bcm56990_l1_key_fmt1_lpm_v6_single_encode(int u, int vrf, int key_mode,
                                          alpm_ip_ver_t v6, uint32_t *ip_addr,
                                          L3_DEFIP_TCAM_KEY_FMT1_t *kdata0,
                                          L3_DEFIP_TCAM_KEY_FMT1_t *kdata1
                                          )
{
    LPM_V6_KEY_t lpm_v6 = {{0}};
    L3_DEFIP_TCAM_KEY_FMT1_LPM_V6_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    LPM_V6_KEY_VRFf_SET(lpm_v6, vrf);
    LPM_V6_KEY_IP_ADDRf_SET(lpm_v6, ip_addr);

    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_SINGLE_BASE_ENTRY_0f_GET(lpm_v6, key);
    FMT1_TCAM_KEY_FIELD_SET(kdata0, KEY_MODEf, key_mode);
    FMT1_TCAM_KEY_FIELD_SET(kdata0, KEY_TYPEf, v6);
    FMT1_TCAM_KEY_FIELD_SET2(kdata0, KEYf, key);

    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_SINGLE_BASE_ENTRY_1f_GET(lpm_v6, key);
    FMT1_TCAM_KEY_FIELD_SET(kdata1, KEY_MODEf, key_mode);
    FMT1_TCAM_KEY_FIELD_SET(kdata1, KEY_TYPEf, v6);
    FMT1_TCAM_KEY_FIELD_SET2(kdata1, KEYf, key);
}



static void
bcm56990_l1_key_fmt1_lpm_v6_single_encode_no_vrf(int u, int key_mode,
                                                 alpm_ip_ver_t v6, uint32_t *udata,
                                                 L3_DEFIP_TCAM_KEY_FMT1_t *kdata0,
                                                 L3_DEFIP_TCAM_KEY_FMT1_t *kdata1
                                                 )
{
    LPM_V6_KEY_t *lpm_v6 = (LPM_V6_KEY_t *)udata;
    L3_DEFIP_TCAM_KEY_FMT1_LPM_V6_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_SINGLE_BASE_ENTRY_0f_GET(*lpm_v6, key);
    FMT1_TCAM_KEY_FIELD_SET(kdata0, KEY_MODEf, key_mode);
    FMT1_TCAM_KEY_FIELD_SET(kdata0, KEY_TYPEf, v6);
    FMT1_TCAM_KEY_FIELD_SET2(kdata0, KEYf, key);

    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_SINGLE_BASE_ENTRY_1f_GET(*lpm_v6, key);
    FMT1_TCAM_KEY_FIELD_SET(kdata1, KEY_MODEf, key_mode);
    FMT1_TCAM_KEY_FIELD_SET(kdata1, KEY_TYPEf, v6);
    FMT1_TCAM_KEY_FIELD_SET2(kdata1, KEYf, key);
}


/* For V4, V6-0 */
static void
bcm56990_l1_key_fmt1_lpm_v4_decode(int u, int *vrf, int *key_mode,
                                   int *kt, uint32_t *ip_addr,
                                   L3_DEFIP_TCAM_KEY_FMT1_t *kdata)
{
    LPM_V4_KEY_t lpm_v4 = {{0}};
    L3_DEFIP_TCAM_KEY_FMT1_LPM_V4_KEY_t v4_key = {{0}};
    uint32_t *key = &v4_key.v[0];

    if (key_mode) {
        FMT1_TCAM_KEY_FIELD_GET(kdata, KEY_MODEf, *key_mode);
    }
    if (kt) {
        FMT1_TCAM_KEY_FIELD_GET(kdata, KEY_TYPEf, *kt);
    }
    FMT1_TCAM_KEY_FIELD_GET2(kdata, KEYf, key);
    LPM_V4_KEY_L3_DEFIP_TCAM_KEY_FMT1_HALF_BASE_ENTRY_0f_SET(lpm_v4, key);

    if (ip_addr) {
        *ip_addr = LPM_V4_KEY_IP_ADDRf_GET(lpm_v4);
    }
    if (vrf) {
        *vrf = LPM_V4_KEY_VRFf_GET(lpm_v4);
    }
}

/* For V4, V6-0 */
static void
bcm56990_l1_key_fmt1_lpm_v4_decode_no_vrf(int u, int *key_mode,
                                          int *kt, uint32_t *udata,
                                          L3_DEFIP_TCAM_KEY_FMT1_t *kdata)
{
    LPM_V4_KEY_t *lpm_v4 = (LPM_V4_KEY_t *)udata;
    L3_DEFIP_TCAM_KEY_FMT1_LPM_V4_KEY_t v4_key = {{0}};
    uint32_t *key = &v4_key.v[0];

    if (key_mode) {
        FMT1_TCAM_KEY_FIELD_GET(kdata, KEY_MODEf, *key_mode);
    }
    if (kt) {
        FMT1_TCAM_KEY_FIELD_GET(kdata, KEY_TYPEf, *kt);
    }
    FMT1_TCAM_KEY_FIELD_GET2(kdata, KEYf, key);
    LPM_V4_KEY_L3_DEFIP_TCAM_KEY_FMT1_HALF_BASE_ENTRY_0f_SET(*lpm_v4, key);

}



static void
bcm56990_l1_key_fmt1_lpm_v6_double_decode(int u, int *vrf, int *key_mode,
                                          int *kt, uint32_t *ip_addr,
                                          L3_DEFIP_TCAM_KEY_FMT1_t *kdata0,
                                          L3_DEFIP_TCAM_KEY_FMT1_t *kdata1,
                                          L3_DEFIP_TCAM_KEY_FMT1_t *kdata2,
                                          L3_DEFIP_TCAM_KEY_FMT1_t *kdata3)
{
    LPM_V6_KEY_t lpm_v6 = {{0}};
    L3_DEFIP_TCAM_KEY_FMT1_LPM_V6_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    if (key_mode) {
        FMT1_TCAM_KEY_FIELD_GET(kdata0, KEY_MODEf, *key_mode);
    }
    if (kt) {
        FMT1_TCAM_KEY_FIELD_GET(kdata0, KEY_TYPEf, *kt);
    }
    FMT1_TCAM_KEY_FIELD_GET2(kdata0, KEYf, key);
    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_DOUBLE_BASE_ENTRY_0f_SET(lpm_v6, key);
    FMT1_TCAM_KEY_FIELD_GET2(kdata1, KEYf, key);
    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_DOUBLE_BASE_ENTRY_1f_SET(lpm_v6, key);
    FMT1_TCAM_KEY_FIELD_GET2(kdata2, KEYf, key);
    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_DOUBLE_BASE_ENTRY_2f_SET(lpm_v6, key);
    FMT1_TCAM_KEY_FIELD_GET2(kdata3, KEYf, key);
    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_DOUBLE_BASE_ENTRY_3f_SET(lpm_v6, key);

    if (ip_addr) {
        LPM_V6_KEY_IP_ADDRf_GET(lpm_v6, ip_addr);
    }
    if (vrf) {
        *vrf = LPM_V6_KEY_VRFf_GET(lpm_v6);
    }
}


static void
bcm56990_l1_key_fmt1_lpm_v6_double_decode_no_vrf(int u, int *key_mode,
                                                 int *kt, uint32_t *udata,
                                                 L3_DEFIP_TCAM_KEY_FMT1_t *kdata0,
                                                 L3_DEFIP_TCAM_KEY_FMT1_t *kdata1,
                                                 L3_DEFIP_TCAM_KEY_FMT1_t *kdata2,
                                                 L3_DEFIP_TCAM_KEY_FMT1_t *kdata3)
{
    LPM_V6_KEY_t *lpm_v6 = (LPM_V6_KEY_t *)udata;
    L3_DEFIP_TCAM_KEY_FMT1_LPM_V6_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    if (key_mode) {
        FMT1_TCAM_KEY_FIELD_GET(kdata0, KEY_MODEf, *key_mode);
    }
    if (kt) {
        FMT1_TCAM_KEY_FIELD_GET(kdata0, KEY_TYPEf, *kt);
    }
    FMT1_TCAM_KEY_FIELD_GET2(kdata0, KEYf, key);
    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_DOUBLE_BASE_ENTRY_0f_SET(*lpm_v6, key);
    FMT1_TCAM_KEY_FIELD_GET2(kdata1, KEYf, key);
    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_DOUBLE_BASE_ENTRY_1f_SET(*lpm_v6, key);
    FMT1_TCAM_KEY_FIELD_GET2(kdata2, KEYf, key);
    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_DOUBLE_BASE_ENTRY_2f_SET(*lpm_v6, key);
    FMT1_TCAM_KEY_FIELD_GET2(kdata3, KEYf, key);
    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_DOUBLE_BASE_ENTRY_3f_SET(*lpm_v6, key);
}

static void
bcm56990_l1_key_fmt1_lpm_v6_single_decode(int u, int *vrf, int *key_mode,
                                          int *kt, uint32_t *ip_addr,
                                          L3_DEFIP_TCAM_KEY_FMT1_t *kdata0,
                                          L3_DEFIP_TCAM_KEY_FMT1_t *kdata1)
{
    LPM_V6_KEY_t lpm_v6 = {{0}};
    L3_DEFIP_TCAM_KEY_FMT1_LPM_V6_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    if (key_mode) {
        FMT1_TCAM_KEY_FIELD_GET(kdata0, KEY_MODEf, *key_mode);
    }
    if (kt) {
        FMT1_TCAM_KEY_FIELD_GET(kdata0, KEY_TYPEf, *kt);
    }
    FMT1_TCAM_KEY_FIELD_GET2(kdata0, KEYf, key);
    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_SINGLE_BASE_ENTRY_0f_SET(lpm_v6, key);
    FMT1_TCAM_KEY_FIELD_GET2(kdata1, KEYf, key);
    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_SINGLE_BASE_ENTRY_1f_SET(lpm_v6, key);
    if (ip_addr) {
        LPM_V6_KEY_IP_ADDRf_GET(lpm_v6, ip_addr);
    }
    if (vrf) {
        *vrf = LPM_V6_KEY_VRFf_GET(lpm_v6);
    }
}



static void
bcm56990_l1_key_fmt1_lpm_v6_single_decode_no_vrf(int u, int *key_mode,
                                                 int *kt, uint32_t *udata,
                                                 L3_DEFIP_TCAM_KEY_FMT1_t *kdata0,
                                                 L3_DEFIP_TCAM_KEY_FMT1_t *kdata1)
{
    LPM_V6_KEY_t *lpm_v6 = (LPM_V6_KEY_t *)udata;
    L3_DEFIP_TCAM_KEY_FMT1_LPM_V6_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    if (key_mode) {
        FMT1_TCAM_KEY_FIELD_GET(kdata0, KEY_MODEf, *key_mode);
    }
    if (kt) {
        FMT1_TCAM_KEY_FIELD_GET(kdata0, KEY_TYPEf, *kt);
    }
    FMT1_TCAM_KEY_FIELD_GET2(kdata0, KEYf, key);
    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_SINGLE_BASE_ENTRY_0f_SET(*lpm_v6, key);
    FMT1_TCAM_KEY_FIELD_GET2(kdata1, KEYf, key);
    LPM_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_SINGLE_BASE_ENTRY_1f_SET(*lpm_v6, key);
}



static void
bcm56990_l1_key_fmt1_l3mc_v6_double_encode(int u, int key_mode,
                                           int kt, uint32_t *udata,
                                           L3_DEFIP_TCAM_KEY_FMT1_t *kdata0,
                                           L3_DEFIP_TCAM_KEY_FMT1_t *kdata1,
                                           L3_DEFIP_TCAM_KEY_FMT1_t *kdata2,
                                           L3_DEFIP_TCAM_KEY_FMT1_t *kdata3
                                           )
{
    L3MC_V6_KEY_t *l3mc = (L3MC_V6_KEY_t *)udata;
    L3_DEFIP_TCAM_KEY_FMT1_L3MC_V6_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    L3MC_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_DOUBLE_BASE_ENTRY_0f_GET(*l3mc, key);
    FMT1_TCAM_KEY_FIELD_SET(kdata0, KEY_MODEf, key_mode);
    FMT1_TCAM_KEY_FIELD_SET(kdata0, KEY_TYPEf, kt);
    FMT1_TCAM_KEY_FIELD_SET2(kdata0, KEYf, key);

    L3MC_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_DOUBLE_BASE_ENTRY_1f_GET(*l3mc, key);
    FMT1_TCAM_KEY_FIELD_SET(kdata1, KEY_MODEf, key_mode);
    FMT1_TCAM_KEY_FIELD_SET(kdata1, KEY_TYPEf, kt);
    FMT1_TCAM_KEY_FIELD_SET2(kdata1, KEYf, key);

    L3MC_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_DOUBLE_BASE_ENTRY_2f_GET(*l3mc, key);
    FMT1_TCAM_KEY_FIELD_SET(kdata2, KEY_MODEf, key_mode);
    FMT1_TCAM_KEY_FIELD_SET(kdata2, KEY_TYPEf, kt);
    FMT1_TCAM_KEY_FIELD_SET2(kdata2, KEYf, key);

    L3MC_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_DOUBLE_BASE_ENTRY_3f_GET(*l3mc, key);
    FMT1_TCAM_KEY_FIELD_SET(kdata3, KEY_MODEf, key_mode);
    FMT1_TCAM_KEY_FIELD_SET(kdata3, KEY_TYPEf, kt);
    FMT1_TCAM_KEY_FIELD_SET2(kdata3, KEYf, key);
}

static void
bcm56990_l1_key_fmt1_l3mc_v4_single_encode(int u, int key_mode,
                                           int kt, uint32_t *udata,
                                           L3_DEFIP_TCAM_KEY_FMT1_t *kdata0,
                                           L3_DEFIP_TCAM_KEY_FMT1_t *kdata1
                                           )
{
    L3MC_V4_KEY_t *l3mc = (L3MC_V4_KEY_t *)udata;
    L3_DEFIP_TCAM_KEY_FMT1_L3MC_V4_KEY_t v4_key = {{0}};
    uint32_t *key = &v4_key.v[0];

    L3MC_V4_KEY_L3_DEFIP_TCAM_KEY_FMT1_SINGLE_BASE_ENTRY_0f_GET(*l3mc, key);
    FMT1_TCAM_KEY_FIELD_SET(kdata0, KEY_MODEf, key_mode);
    FMT1_TCAM_KEY_FIELD_SET(kdata0, KEY_TYPEf, kt);
    FMT1_TCAM_KEY_FIELD_SET2(kdata0, KEYf, key);

    L3MC_V4_KEY_L3_DEFIP_TCAM_KEY_FMT1_SINGLE_BASE_ENTRY_1f_GET(*l3mc, key);
    FMT1_TCAM_KEY_FIELD_SET(kdata1, KEY_MODEf, key_mode);
    FMT1_TCAM_KEY_FIELD_SET(kdata1, KEY_TYPEf, kt);
    FMT1_TCAM_KEY_FIELD_SET2(kdata1, KEYf, key);
}

static void
bcm56990_l1_key_fmt1_l3mc_v6_double_decode(int u, int *key_mode,
                                           int *kt, uint32_t *udata,
                                           L3_DEFIP_TCAM_KEY_FMT1_t *kdata0,
                                           L3_DEFIP_TCAM_KEY_FMT1_t *kdata1,
                                           L3_DEFIP_TCAM_KEY_FMT1_t *kdata2,
                                           L3_DEFIP_TCAM_KEY_FMT1_t *kdata3)
{
    L3MC_V6_KEY_t *l3mc = (L3MC_V6_KEY_t *)udata;
    L3_DEFIP_TCAM_KEY_FMT1_L3MC_V6_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    if (key_mode) {
        FMT1_TCAM_KEY_FIELD_GET(kdata0, KEY_MODEf, *key_mode);
    }
    if (kt) {
        FMT1_TCAM_KEY_FIELD_GET(kdata0, KEY_TYPEf, *kt);
    }
    FMT1_TCAM_KEY_FIELD_GET2(kdata0, KEYf, key);
    L3MC_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_DOUBLE_BASE_ENTRY_0f_SET(*l3mc, key);
    FMT1_TCAM_KEY_FIELD_GET2(kdata1, KEYf, key);
    L3MC_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_DOUBLE_BASE_ENTRY_1f_SET(*l3mc, key);
    FMT1_TCAM_KEY_FIELD_GET2(kdata2, KEYf, key);
    L3MC_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_DOUBLE_BASE_ENTRY_2f_SET(*l3mc, key);
    FMT1_TCAM_KEY_FIELD_GET2(kdata3, KEYf, key);
    L3MC_V6_KEY_L3_DEFIP_TCAM_KEY_FMT1_DOUBLE_BASE_ENTRY_3f_SET(*l3mc, key);
}

static void
bcm56990_l1_key_fmt1_l3mc_v4_single_decode(int u, int *key_mode,
                                           int *kt, uint32_t *udata,
                                           L3_DEFIP_TCAM_KEY_FMT1_t *kdata0,
                                           L3_DEFIP_TCAM_KEY_FMT1_t *kdata1)
{
    L3MC_V4_KEY_t *l3mc = (L3MC_V4_KEY_t *)udata;
    L3_DEFIP_TCAM_KEY_FMT1_L3MC_V4_KEY_t v4_key = {{0}};
    uint32_t *key = &v4_key.v[0];

    if (key_mode) {
        FMT1_TCAM_KEY_FIELD_GET(kdata0, KEY_MODEf, *key_mode);
    }
    if (kt) {
        FMT1_TCAM_KEY_FIELD_GET(kdata0, KEY_TYPEf, *kt);
    }
    FMT1_TCAM_KEY_FIELD_GET2(kdata0, KEYf, key);
    L3MC_V4_KEY_L3_DEFIP_TCAM_KEY_FMT1_SINGLE_BASE_ENTRY_0f_SET(*l3mc, key);
    FMT1_TCAM_KEY_FIELD_GET2(kdata1, KEYf, key);
    L3MC_V4_KEY_L3_DEFIP_TCAM_KEY_FMT1_SINGLE_BASE_ENTRY_1f_SET(*l3mc, key);
}

static void
bcm56990_l1_key_fmt0_comp_v6_double_encode(int u,
                                           bcmptm_cth_alpm_comp_key_type_t comp_key_type,
                                           int vrf, int key_mode,
                                           int kt, uint32_t *ip_addr,
                                           L3_DEFIP_TCAM_KEY_FMT0_t *kdata0,
                                           L3_DEFIP_TCAM_KEY_FMT0_t *kdata1,
                                           L3_DEFIP_TCAM_KEY_FMT0_t *kdata2,
                                           L3_DEFIP_TCAM_KEY_FMT0_t *kdata3
                                           )
{
    COMP_V6_KEY_t comp = {{0}};
    L3_DEFIP_TCAM_KEY_FMT0_COMP_V6_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    switch (comp_key_type) {
        case COMP_KEY_L4_PORT:
            COMP_V6_KEY_IP_ADDRf_SET(comp, ip_addr);
            COMP_V6_KEY_L4_PORTf_SET(comp, vrf);
            break;
        case COMP_KEY_VRF:
            COMP_V6_KEY_IP_ADDRf_SET(comp, ip_addr);
            COMP_V6_KEY_VRFf_SET(comp, vrf);
            break;
        case COMP_KEY_FULL:
            sal_memcpy(&comp, (COMP_V6_KEY_t *)ip_addr, sizeof(comp));
            break;
        default:
            break;
    }

    COMP_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_0f_GET(comp, key);
    FMT0_TCAM_KEY_FIELD_SET(kdata0, KEY_MODEf, key_mode);
    FMT0_TCAM_KEY_FIELD_SET(kdata0, KEY_TYPEf, kt);
    FMT0_TCAM_KEY_FIELD_SET2(kdata0, KEYf, key);

    COMP_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_1f_GET(comp, key);
    FMT0_TCAM_KEY_FIELD_SET(kdata1, KEY_MODEf, key_mode);
    FMT0_TCAM_KEY_FIELD_SET(kdata1, KEY_TYPEf, kt);
    FMT0_TCAM_KEY_FIELD_SET2(kdata1, KEYf, key);

    COMP_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_2f_GET(comp, key);
    FMT0_TCAM_KEY_FIELD_SET(kdata2, KEY_MODEf, key_mode);
    FMT0_TCAM_KEY_FIELD_SET(kdata2, KEY_TYPEf, kt);
    FMT0_TCAM_KEY_FIELD_SET2(kdata2, KEYf, key);

    COMP_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_3f_GET(comp, key);
    FMT0_TCAM_KEY_FIELD_SET(kdata3, KEY_MODEf, key_mode);
    FMT0_TCAM_KEY_FIELD_SET(kdata3, KEY_TYPEf, kt);
    FMT0_TCAM_KEY_FIELD_SET2(kdata3, KEYf, key);
}


static void
bcm56990_l1_key_fmt0_comp_v4_double_encode(int u,
                                           bcmptm_cth_alpm_comp_key_type_t comp_key_type,
                                           int vrf, int key_mode,
                                           int kt, uint32_t *ip_addr,
                                           L3_DEFIP_TCAM_KEY_FMT0_t *kdata0,
                                           L3_DEFIP_TCAM_KEY_FMT0_t *kdata1,
                                           L3_DEFIP_TCAM_KEY_FMT0_t *kdata2,
                                           L3_DEFIP_TCAM_KEY_FMT0_t *kdata3
                                           )
{
    COMP_V4_KEY_t comp = {{0}};
    L3_DEFIP_TCAM_KEY_FMT0_COMP_V4_KEY_t v4_key = {{0}};
    uint32_t *key = &v4_key.v[0];

    switch (comp_key_type) {
        case COMP_KEY_L4_PORT:
            COMP_V4_KEY_IP_ADDRf_SET(comp, *ip_addr);
            COMP_V4_KEY_L4_PORTf_SET(comp, vrf);
            break;
        case COMP_KEY_VRF:
            COMP_V4_KEY_IP_ADDRf_SET(comp, *ip_addr);
            COMP_V4_KEY_VRFf_SET(comp, vrf);
            break;
        case COMP_KEY_FULL:
            sal_memcpy(&comp, (COMP_V4_KEY_t *)ip_addr, sizeof(comp));
            break;
        default:
            break;
    }

    COMP_V4_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_0f_GET(comp, key);
    FMT0_TCAM_KEY_FIELD_SET(kdata0, KEY_MODEf, key_mode);
    FMT0_TCAM_KEY_FIELD_SET(kdata0, KEY_TYPEf, kt);
    FMT0_TCAM_KEY_FIELD_SET2(kdata0, KEYf, key);

    COMP_V4_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_1f_GET(comp, key);
    FMT0_TCAM_KEY_FIELD_SET(kdata1, KEY_MODEf, key_mode);
    FMT0_TCAM_KEY_FIELD_SET(kdata1, KEY_TYPEf, kt);
    FMT0_TCAM_KEY_FIELD_SET2(kdata1, KEYf, key);

    COMP_V4_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_2f_GET(comp, key);
    FMT0_TCAM_KEY_FIELD_SET(kdata2, KEY_MODEf, key_mode);
    FMT0_TCAM_KEY_FIELD_SET(kdata2, KEY_TYPEf, kt);
    FMT0_TCAM_KEY_FIELD_SET2(kdata2, KEYf, key);

    COMP_V4_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_3f_GET(comp, key);
    FMT0_TCAM_KEY_FIELD_SET(kdata3, KEY_MODEf, key_mode);
    FMT0_TCAM_KEY_FIELD_SET(kdata3, KEY_TYPEf, kt);
    FMT0_TCAM_KEY_FIELD_SET2(kdata3, KEYf, key);
}

static void
bcm56990_l1_key_fmt0_comp_v6_double_decode(int u,
                                           bcmptm_cth_alpm_comp_key_type_t comp_key_type,
                                           int *vrf, int *key_mode,
                                           int *kt, uint32_t *ip_addr,
                                           L3_DEFIP_TCAM_KEY_FMT0_t *kdata0,
                                           L3_DEFIP_TCAM_KEY_FMT0_t *kdata1,
                                           L3_DEFIP_TCAM_KEY_FMT0_t *kdata2,
                                           L3_DEFIP_TCAM_KEY_FMT0_t *kdata3)
{
    COMP_V6_KEY_t comp = {{0}};
    L3_DEFIP_TCAM_KEY_FMT0_COMP_V6_KEY_t v6_key = {{0}};
    uint32_t *key = &v6_key.v[0];

    if (key_mode) {
        FMT0_TCAM_KEY_FIELD_GET(kdata0, KEY_MODEf, *key_mode);
    }
    if (kt) {
        FMT0_TCAM_KEY_FIELD_GET(kdata0, KEY_TYPEf, *kt);
    }
    FMT0_TCAM_KEY_FIELD_GET2(kdata0, KEYf, key);
    COMP_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_0f_SET(comp, key);
    FMT0_TCAM_KEY_FIELD_GET2(kdata1, KEYf, key);
    COMP_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_1f_SET(comp, key);
    FMT0_TCAM_KEY_FIELD_GET2(kdata2, KEYf, key);
    COMP_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_2f_SET(comp, key);
    FMT0_TCAM_KEY_FIELD_GET2(kdata3, KEYf, key);
    COMP_V6_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_3f_SET(comp, key);

    switch (comp_key_type) {
        case COMP_KEY_L4_PORT:
            if (ip_addr) {
                COMP_V6_KEY_IP_ADDRf_GET(comp, ip_addr);
            }

            if (vrf) {
                *vrf = COMP_V6_KEY_L4_PORTf_GET(comp);
            }
            break;
        case COMP_KEY_VRF:
            if (ip_addr) {
                COMP_V6_KEY_IP_ADDRf_GET(comp, ip_addr);
            }

            if (vrf) {
                *vrf = COMP_V6_KEY_VRFf_GET(comp);
            }
            break;
        case COMP_KEY_FULL:
            if (ip_addr) {
                sal_memcpy((COMP_V6_KEY_t *)ip_addr, &comp, sizeof(comp));
            }

            if (vrf) {
                *vrf = 0;
            }
            break;
        default:
            break;
    }
}

static void
bcm56990_l1_key_fmt0_comp_v4_double_decode(int u,
                                           bcmptm_cth_alpm_comp_key_type_t comp_key_type,
                                           int *vrf, int *key_mode,
                                           int *kt, uint32_t *ip_addr,
                                           L3_DEFIP_TCAM_KEY_FMT0_t *kdata0,
                                           L3_DEFIP_TCAM_KEY_FMT0_t *kdata1,
                                           L3_DEFIP_TCAM_KEY_FMT0_t *kdata2,
                                           L3_DEFIP_TCAM_KEY_FMT0_t *kdata3)
{
    COMP_V4_KEY_t comp = {{0}};
    L3_DEFIP_TCAM_KEY_FMT0_COMP_V4_KEY_t v4_key = {{0}};
    uint32_t *key = &v4_key.v[0];

    if (key_mode) {
        FMT0_TCAM_KEY_FIELD_GET(kdata0, KEY_MODEf, *key_mode);
    }
    if (kt) {
        FMT0_TCAM_KEY_FIELD_GET(kdata0, KEY_TYPEf, *kt);
    }
    FMT0_TCAM_KEY_FIELD_GET2(kdata0, KEYf, key);
    COMP_V4_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_0f_SET(comp, key);
    FMT0_TCAM_KEY_FIELD_GET2(kdata1, KEYf, key);
    COMP_V4_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_1f_SET(comp, key);
    FMT0_TCAM_KEY_FIELD_GET2(kdata2, KEYf, key);
    COMP_V4_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_2f_SET(comp, key);
    FMT0_TCAM_KEY_FIELD_GET2(kdata3, KEYf, key);
    COMP_V4_KEY_L3_DEFIP_TCAM_KEY_FMT0_DOUBLE_BASE_ENTRY_3f_SET(comp, key);

    switch (comp_key_type) {
        case COMP_KEY_L4_PORT:
            if (ip_addr) {
                *ip_addr = COMP_V4_KEY_IP_ADDRf_GET(comp);
            }

            if (vrf) {
                *vrf = COMP_V4_KEY_L4_PORTf_GET(comp);
            }
            break;
        case COMP_KEY_VRF:
            if (ip_addr) {
                *ip_addr = COMP_V4_KEY_IP_ADDRf_GET(comp);
            }

            if (vrf) {
                *vrf = COMP_V4_KEY_VRFf_GET(comp);
            }
            break;
        case COMP_KEY_FULL:
            if (ip_addr) {
                sal_memcpy((COMP_V4_KEY_t *)ip_addr, &comp, sizeof(comp));
            }

            if (vrf) {
                *vrf = 0;
            }
            break;
        default:
            break;
    }
}


static void
bcm56990_ln_assoc_data_format_encode(int u, bcm56990_assoc_data_t *buf,
                                     int full,
                                     assoc_data_t *assoc)
{
    ALPM_ASSERT(sizeof(*buf) <= sizeof(assoc->fmt_ad));
    sal_memcpy(buf, &assoc->fmt_ad, sizeof(*buf));
}

static inline void
bcm56990_ln_assoc_data_format_decode(int u, bcm56990_assoc_data_t *buf,
                                     int full,
                                     assoc_data_t *assoc,
                                     uint8_t app)
{
    ALPM_ASSERT(sizeof(*buf) <= sizeof(assoc->fmt_ad));
    sal_memcpy(&assoc->fmt_ad, buf, sizeof(*buf));

    if (full) {
        if (app == APP_LPM || app == APP_COMP1) {
            if ((buf->v[0] & 0xFFFF0000) == 0 && buf->v[1] == 0) {
                assoc->user_data_type = DATA_REDUCED;
            } else {
                assoc->user_data_type = DATA_FULL;
            }
        } else if (app == APP_L3MC) {
            assoc->user_data_type = DATA_FULL;
        }
    } else {
        assoc->user_data_type = DATA_REDUCED;
    }
}

static int
bcm56990_kshift(int u, alpm_arg_t *arg)
{
    int kshift = 0;

    if (arg->key.t.app == APP_LPM) {
        kshift = 13;
    } else if (arg->key.t.app == APP_L3MC) {
        kshift = 0;
    } else if (arg->key.t.app == APP_COMP1) {
        if (arg->key.t.ipv == IP_VER_4) {
            switch (arg->comp_key_type) {
                case COMP_KEY_L4_PORT:
                case COMP_KEY_VRF:
                    kshift = 101;
                    break;
                case COMP_KEY_FULL:
                default:
                    kshift = 0;
            }
        } else {
            switch (arg->comp_key_type) {
                case COMP_KEY_L4_PORT:
                case COMP_KEY_VRF:
                    kshift = 45;
                    break;
                case COMP_KEY_FULL:
                default:
                    kshift = 0;
            }
        }
    }

    return kshift;
}

static void
bcm56990_ln_alpm2_data_format_encode(int u, ALPM2_DATA_t *a2_format,
                                     alpm2_data_t *a2_data, alpm_arg_t *arg)
{
    ALPM2_DATA_CLR(*a2_format);
    FMT_ALPM2_DATA_FIELD_SET(BKT_PTRf, a2_data->phy_bkt);
    FMT_ALPM2_DATA_FIELD_SET(DEFAULT_MISSf, a2_data->default_miss);
    FMT_ALPM2_DATA_FIELD_SET(KSHIFTf, KSHIFT_PATCH1(u, arg, a2_data->kshift));
    FMT_ALPM2_DATA_FIELD_SET(ROFSf, a2_data->rofs);
    FMT_ALPM2_DATA_FIELD_SET(FMT0f, bcm56990_l2p_route_fmts[a2_data->fmt[0]]);
    FMT_ALPM2_DATA_FIELD_SET(FMT1f, bcm56990_l2p_route_fmts[a2_data->fmt[1]]);
    FMT_ALPM2_DATA_FIELD_SET(FMT2f, bcm56990_l2p_route_fmts[a2_data->fmt[2]]);
    FMT_ALPM2_DATA_FIELD_SET(FMT3f, bcm56990_l2p_route_fmts[a2_data->fmt[3]]);
    FMT_ALPM2_DATA_FIELD_SET(FMT4f, bcm56990_l2p_route_fmts[a2_data->fmt[4]]);
    FMT_ALPM2_DATA_FIELD_SET(FMT5f, bcm56990_l2p_route_fmts[a2_data->fmt[5]]);
    FMT_ALPM2_DATA_FIELD_SET(FMT6f, bcm56990_l2p_route_fmts[a2_data->fmt[6]]);
    FMT_ALPM2_DATA_FIELD_SET(FMT7f, bcm56990_l2p_route_fmts[a2_data->fmt[7]]);
}


static void
bcm56990_ln_alpm2_data_format_decode(int u, ALPM2_DATA_t *a2_format,
                                     alpm2_data_t *a2_data, alpm_arg_t *arg)
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
    KSHIFT_PATCH2(u, arg, a2_data->kshift);
    for (i = 0; i < ALPM2_DATA_FORMATS; i++) {
        a2_data->fmt[i] = bcm56990_p2l_route_fmts[a2_data->fmt[i]];
    }
}


static void
bcm56990_ln_alpm1_data_format_encode(int u, void *buf, alpm1_data_t *a1_data,
                                     alpm_arg_t *arg)
{
    ALPM1_DATA_t *a1_format = buf;
    ALPM1_DATA_CLR(*a1_format);
    FMT_ALPM1_DATA_FIELD_SET(BKT_PTRf, a1_data->phy_bkt);
    FMT_ALPM1_DATA_FIELD_SET(DEFAULT_MISSf, a1_data->default_miss);
    FMT_ALPM1_DATA_FIELD_SET(DIRECT_ROUTEf, a1_data->direct_route);
    FMT_ALPM1_DATA_FIELD_SET(KSHIFTf, KSHIFT_PATCH1(u, arg, a1_data->kshift));
    FMT_ALPM1_DATA_FIELD_SET(ROFSf, a1_data->rofs);
    if (a1_data->is_route) {
        FMT_ALPM1_DATA_FIELD_SET(FMT0f, bcm56990_l2p_route_fmts[a1_data->fmt[0]]);
        FMT_ALPM1_DATA_FIELD_SET(FMT1f, bcm56990_l2p_route_fmts[a1_data->fmt[1]]);
        FMT_ALPM1_DATA_FIELD_SET(FMT2f, bcm56990_l2p_route_fmts[a1_data->fmt[2]]);
        FMT_ALPM1_DATA_FIELD_SET(FMT3f, bcm56990_l2p_route_fmts[a1_data->fmt[3]]);
        FMT_ALPM1_DATA_FIELD_SET(FMT4f, bcm56990_l2p_route_fmts[a1_data->fmt[4]]);
        FMT_ALPM1_DATA_FIELD_SET(FMT5f, bcm56990_l2p_route_fmts[a1_data->fmt[5]]);
    } else {
        FMT_ALPM1_DATA_FIELD_SET(FMT0f, bcm56990_l2p_pivot_fmts[a1_data->fmt[0]]);
        FMT_ALPM1_DATA_FIELD_SET(FMT1f, bcm56990_l2p_pivot_fmts[a1_data->fmt[1]]);
        FMT_ALPM1_DATA_FIELD_SET(FMT2f, bcm56990_l2p_pivot_fmts[a1_data->fmt[2]]);
        FMT_ALPM1_DATA_FIELD_SET(FMT3f, bcm56990_l2p_pivot_fmts[a1_data->fmt[3]]);
        FMT_ALPM1_DATA_FIELD_SET(FMT4f, bcm56990_l2p_pivot_fmts[a1_data->fmt[4]]);
        FMT_ALPM1_DATA_FIELD_SET(FMT5f, bcm56990_l2p_pivot_fmts[a1_data->fmt[5]]);
    }
}

static void
bcm56990_ln_alpm1_data_format_decode(int u, void *buf, alpm1_data_t *a1_data,
                                     alpm_arg_t *arg)
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
    KSHIFT_PATCH2(u, arg, a1_data->kshift);
    for (i = 0; i < ALPM1_DATA_FORMATS; i++) {
        if (a1_data->is_route) {
            a1_data->fmt[i] = bcm56990_p2l_route_fmts[a1_data->fmt[i]];
        } else {
            a1_data->fmt[i] = bcm56990_p2l_pivot_fmts[a1_data->fmt[i]];
        }
    }
}

static void
bcm56990_ln_pivot_base_entry_info(int u, uint8_t format_value,
                                  int *prefix_len,
                                  int *full,
                                  int *number)
{
    ALPM_ASSERT(format_value < COUNTOF(bcm56990_pivot_base_entry_info));
    *prefix_len = bcm56990_pivot_base_entry_info[format_value].prefix_len;
    *full = bcm56990_pivot_base_entry_info[format_value].full;
    *number = bcm56990_pivot_base_entry_info[format_value].num_entries;

    (*prefix_len)--; /* Exclude the extra bit */
}



static void
bcm56990_ln_pivot_format_full_encode2(int u, uint8_t format_value,
                                      uint32_t *base_entry,
                                      int ent_ofs,
                                      void *buf)
{
    format_value = bcm56990_l2p_pivot_fmts[format_value];
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
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56990_ln_pivot_format_full_encode(int u, uint8_t format_value,
                                     ln_base_entry_t *base_entry,
                                     int entry_count,
                                     void *buf)
{
    format_value = bcm56990_l2p_pivot_fmts[format_value];
    switch (format_value) {
    case 1: {
        L3_DEFIP_ALPM_PIVOT_FMT1_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_SET2(1, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_SET2(1, ENTRY_1f, base_entry[1]);
        PIVOT_FMT_FULL_FIELD_SET2(1, ENTRY_2f, base_entry[2]);
        PIVOT_FMT_FULL_FIELD_SET2(1, ENTRY_3f, base_entry[3]);
        PIVOT_FMT_FULL_FIELD_SET2(1, ENTRY_4f, base_entry[4]);
        PIVOT_FMT_FULL_FIELD_SET2(1, ENTRY_5f, base_entry[5]);
    } break;
    case 2: {
        L3_DEFIP_ALPM_PIVOT_FMT2_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_SET2(2, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_SET2(2, ENTRY_1f, base_entry[1]);
        PIVOT_FMT_FULL_FIELD_SET2(2, ENTRY_2f, base_entry[2]);
        PIVOT_FMT_FULL_FIELD_SET2(2, ENTRY_3f, base_entry[3]);
        PIVOT_FMT_FULL_FIELD_SET2(2, ENTRY_4f, base_entry[4]);
    } break;
    case 3: {
        L3_DEFIP_ALPM_PIVOT_FMT3_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_SET2(3, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_SET2(3, ENTRY_1f, base_entry[1]);
        PIVOT_FMT_FULL_FIELD_SET2(3, ENTRY_2f, base_entry[2]);
        PIVOT_FMT_FULL_FIELD_SET2(3, ENTRY_3f, base_entry[3]);
    } break;
    case 4: {
        L3_DEFIP_ALPM_PIVOT_FMT4_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_SET2(4, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_SET2(4, ENTRY_1f, base_entry[1]);
        PIVOT_FMT_FULL_FIELD_SET2(4, ENTRY_2f, base_entry[2]);
    } break;
    case 5: {
        L3_DEFIP_ALPM_PIVOT_FMT5_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_SET2(5, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_SET2(5, ENTRY_1f, base_entry[1]);
    } break;
    case 6: {
        L3_DEFIP_ALPM_PIVOT_FMT6_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_SET2(6, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_SET2(6, ENTRY_1f, base_entry[1]);
        PIVOT_FMT_FULL_FIELD_SET2(6, ENTRY_2f, base_entry[2]);
        PIVOT_FMT_FULL_FIELD_SET2(6, ENTRY_3f, base_entry[3]);
    } break;
    case 7: {
        L3_DEFIP_ALPM_PIVOT_FMT7_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_SET2(7, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_SET2(7, ENTRY_1f, base_entry[1]);
        PIVOT_FMT_FULL_FIELD_SET2(7, ENTRY_2f, base_entry[2]);
    } break;
    case 8: {
        L3_DEFIP_ALPM_PIVOT_FMT8_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_SET2(8, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_SET2(8, ENTRY_1f, base_entry[1]);
    } break;
    case 9: {
        L3_DEFIP_ALPM_PIVOT_FMT9_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_SET2(9, ENTRY_0f, base_entry[0]);
    } break;
    case 10: {
        L3_DEFIP_ALPM_PIVOT_FMT10_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_SET2(10, ENTRY_0f, base_entry[0]);
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56990_ln_pivot_format_full_decode2(int u, uint8_t format_value,
                                      uint32_t *base_entry,
                                      int ent_ofs,
                                      void *buf)
{
    format_value = bcm56990_l2p_pivot_fmts[format_value];
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
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56990_ln_pivot_format_full_decode(int u, uint8_t format_value,
                                     ln_base_entry_t *base_entry,
                                     int entry_count,
                                     void *buf)
{
    format_value = bcm56990_l2p_pivot_fmts[format_value];
    switch (format_value) {
    case 1: {
        L3_DEFIP_ALPM_PIVOT_FMT1_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_GET2(1, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_GET2(1, ENTRY_1f, base_entry[1]);
        PIVOT_FMT_FULL_FIELD_GET2(1, ENTRY_2f, base_entry[2]);
        PIVOT_FMT_FULL_FIELD_GET2(1, ENTRY_3f, base_entry[3]);
        PIVOT_FMT_FULL_FIELD_GET2(1, ENTRY_4f, base_entry[4]);
        PIVOT_FMT_FULL_FIELD_GET2(1, ENTRY_5f, base_entry[5]);
    } break;
    case 2: {
        L3_DEFIP_ALPM_PIVOT_FMT2_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_GET2(2, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_GET2(2, ENTRY_1f, base_entry[1]);
        PIVOT_FMT_FULL_FIELD_GET2(2, ENTRY_2f, base_entry[2]);
        PIVOT_FMT_FULL_FIELD_GET2(2, ENTRY_3f, base_entry[3]);
        PIVOT_FMT_FULL_FIELD_GET2(2, ENTRY_4f, base_entry[4]);
    } break;
    case 3: {
        L3_DEFIP_ALPM_PIVOT_FMT3_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_GET2(3, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_GET2(3, ENTRY_1f, base_entry[1]);
        PIVOT_FMT_FULL_FIELD_GET2(3, ENTRY_2f, base_entry[2]);
        PIVOT_FMT_FULL_FIELD_GET2(3, ENTRY_3f, base_entry[3]);
    } break;
    case 4: {
        L3_DEFIP_ALPM_PIVOT_FMT4_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_GET2(4, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_GET2(4, ENTRY_1f, base_entry[1]);
        PIVOT_FMT_FULL_FIELD_GET2(4, ENTRY_2f, base_entry[2]);
    } break;
    case 5: {
        L3_DEFIP_ALPM_PIVOT_FMT5_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_GET2(5, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_GET2(5, ENTRY_1f, base_entry[1]);
    } break;
    case 6: {
        L3_DEFIP_ALPM_PIVOT_FMT6_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_GET2(6, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_GET2(6, ENTRY_1f, base_entry[1]);
        PIVOT_FMT_FULL_FIELD_GET2(6, ENTRY_2f, base_entry[2]);
        PIVOT_FMT_FULL_FIELD_GET2(6, ENTRY_3f, base_entry[3]);
    } break;
    case 7: {
        L3_DEFIP_ALPM_PIVOT_FMT7_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_GET2(7, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_GET2(7, ENTRY_1f, base_entry[1]);
        PIVOT_FMT_FULL_FIELD_GET2(7, ENTRY_2f, base_entry[2]);
    } break;
    case 8: {
        L3_DEFIP_ALPM_PIVOT_FMT8_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_GET2(8, ENTRY_0f, base_entry[0]);
        PIVOT_FMT_FULL_FIELD_GET2(8, ENTRY_1f, base_entry[1]);
    } break;
    case 9: {
        L3_DEFIP_ALPM_PIVOT_FMT9_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_GET2(9, ENTRY_0f, base_entry[0]);
    } break;
    case 10: {
        L3_DEFIP_ALPM_PIVOT_FMT10_FULL_t *bucket = buf;
        PIVOT_FMT_FULL_FIELD_GET2(10, ENTRY_0f, base_entry[0]);
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56990_ln_pivot_format_set_data(uint8_t format_value,
                                  uint32_t *ad_format,
                                  void *buf)
{
    format_value = bcm56990_l2p_pivot_fmts[format_value];
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
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56990_ln_pivot_format_set(uint8_t format_value,
                             uint32_t *ad_format,
                             uint32_t *a2_format,
                             alpm_format_ip_t prefix,
                             void *buf)
{
    format_value = bcm56990_l2p_pivot_fmts[format_value];
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
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56990_ln_pivot_format_encode(int u, uint8_t format_value,
                                uint8_t kshift,
                                alpm_arg_t *arg,
                                void *buf)
{
    int full = 0, number;
    int max_len = 0;
    bcm56990_assoc_data_t ad_format;
    bcm56990_ln_pivot_base_entry_info(u, format_value, &max_len, &full,
                                      &number);
    bcm56990_ln_assoc_data_format_encode(u, &ad_format, full, &arg->ad);

    if (arg->encode_data_only) {
        bcm56990_ln_pivot_format_set_data(format_value,
                                          &ad_format.v[0],
                                          buf);
    } else {
        alpm_format_ip_t prefix;
        ALPM2_DATA_t a2_format;
        bcm56990_ln_alpm2_data_format_encode(u, &a2_format, &arg->a2, arg);
        bcmptm_rm_alpm_one_bit_encode(arg->key.t.max_bits,
                                      &arg->key.t.trie_ip,
                                      arg->key.t.len,
                                      max_len,
                                      kshift,
                                      &prefix[0]);
        bcm56990_ln_pivot_format_set(format_value,
                                     &ad_format.v[0],
                                     &a2_format.v[0],
                                     &prefix[0],
                                     buf);
    }
}

static void
bcm56990_ln_pivot_format_get_data(uint8_t format_value,
                                  uint32_t *ad_format,
                                  void *buf)
{
    format_value = bcm56990_l2p_pivot_fmts[format_value];
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
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56990_ln_pivot_format_get(uint8_t format_value,
                             uint32_t *ad_format,
                             uint32_t *a2_format,
                             alpm_format_ip_t prefix,
                             void *buf)
{
    format_value = bcm56990_l2p_pivot_fmts[format_value];
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
    default:
        ALPM_ASSERT(0);
        break;
    }
}


static void
bcm56990_ln_pivot_format_decode(int u, uint8_t format_value,
                                uint8_t kshift,
                                alpm_arg_t *arg,
                                void *buf)
{
    int full, number;
    int max_len;
    ALPM2_DATA_t a2_format;
    bcm56990_assoc_data_t ad_format = {{{0}}};
    alpm_format_ip_t prefix = {0};
    bcm56990_ln_pivot_base_entry_info(u, format_value, &max_len, &full,
                                      &number);
    if (arg->decode_data_only) {
        bcm56990_ln_pivot_format_get_data(format_value,
                                          &ad_format.v[0],
                                          buf);
    } else {
        bcm56990_ln_pivot_format_get(format_value,
                                     &ad_format.v[0],
                                     &a2_format.v[0],
                                     &prefix[0],
                                     buf);
        bcm56990_ln_alpm2_data_format_decode(u, &a2_format, &arg->a2, arg);
        arg->valid = bcmptm_rm_alpm_one_bit_decode(arg->key.t.max_bits,
                                                   &arg->key.t.trie_ip,
                                                   &arg->key.t.len,
                                                   max_len,
                                                   kshift,
                                                   &prefix[0]);
    }
    bcm56990_ln_assoc_data_format_decode(u, &ad_format, full, &arg->ad,
                                         arg->key.t.app);
}

static void
bcm56990_ln_route_base_entry_info(int u, uint8_t format_value,
                                  int *prefix_len,
                                  int *full,
                                  int *number)
{
    ALPM_ASSERT(format_value < COUNTOF(bcm56990_route_base_entry_info));
    *prefix_len = bcm56990_route_base_entry_info[format_value].prefix_len;
    *full = bcm56990_route_base_entry_info[format_value].full;
    *number = bcm56990_route_base_entry_info[format_value].num_entries;

    (*prefix_len)--; /* Exclude the extra bit */
}


static void
bcm56990_ln_route_format_full_encode2(int u, uint8_t format_value,
                                      uint32_t *base_entry,
                                      int ent_ofs,
                                      void *buf)
{
    format_value = bcm56990_l2p_route_fmts[format_value];
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
        default: ALPM_ASSERT(0); break;
        }
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56990_ln_route_format_full_encode(int u, uint8_t format_value,
                                     ln_base_entry_t *base_entry,
                                     int entry_count,
                                     void *buf)
{
    format_value = bcm56990_l2p_route_fmts[format_value];
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
    } break;
    case 6: {
        L3_DEFIP_ALPM_ROUTE_FMT6_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(6, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(6, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(6, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_SET2(6, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_SET2(6, ENTRY_4f, base_entry[4]);
        ROUTE_FMT_FULL_FIELD_SET2(6, ENTRY_5f, base_entry[5]);
    } break;
    case 7: {
        L3_DEFIP_ALPM_ROUTE_FMT7_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(7, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(7, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(7, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_SET2(7, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_SET2(7, ENTRY_4f, base_entry[4]);
    } break;
    case 8: {
        L3_DEFIP_ALPM_ROUTE_FMT8_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(8, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(8, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(8, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_SET2(8, ENTRY_3f, base_entry[3]);
    } break;
    case 9: {
        L3_DEFIP_ALPM_ROUTE_FMT9_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(9, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(9, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(9, ENTRY_2f, base_entry[2]);
    } break;
    case 10: {
        L3_DEFIP_ALPM_ROUTE_FMT10_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(10, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(10, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(10, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_SET2(10, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_SET2(10, ENTRY_4f, base_entry[4]);
        ROUTE_FMT_FULL_FIELD_SET2(10, ENTRY_5f, base_entry[5]);
    } break;
    case 11: {
        L3_DEFIP_ALPM_ROUTE_FMT11_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(11, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(11, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_SET2(11, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_SET2(11, ENTRY_3f, base_entry[3]);
    } break;
    case 12: {
        L3_DEFIP_ALPM_ROUTE_FMT12_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(12, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(12, ENTRY_1f, base_entry[1]);
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
    } break;
    case 14: {
        L3_DEFIP_ALPM_ROUTE_FMT14_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(14, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_SET2(14, ENTRY_1f, base_entry[1]);
    } break;
    case 15: {
        L3_DEFIP_ALPM_ROUTE_FMT15_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_SET2(15, ENTRY_0f, base_entry[0]);
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
}



static void
bcm56990_ln_route_format_full_decode2(int u, uint8_t format_value,
                                      uint32_t *base_entry,
                                      int ent_ofs,
                                      void *buf)
{
    format_value = bcm56990_l2p_route_fmts[format_value];
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
        default: ALPM_ASSERT(0); break;
        }
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
}


static void
bcm56990_ln_route_format_full_decode(int u, uint8_t format_value,
                                     ln_base_entry_t *base_entry,
                                     int entry_count,
                                     void *buf)
{
    format_value = bcm56990_l2p_route_fmts[format_value];
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
    } break;
    case 6: {
        L3_DEFIP_ALPM_ROUTE_FMT6_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(6, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(6, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(6, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_GET2(6, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_GET2(6, ENTRY_4f, base_entry[4]);
        ROUTE_FMT_FULL_FIELD_GET2(6, ENTRY_5f, base_entry[5]);
    } break;
    case 7: {
        L3_DEFIP_ALPM_ROUTE_FMT7_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(7, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(7, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(7, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_GET2(7, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_GET2(7, ENTRY_4f, base_entry[4]);
    } break;
    case 8: {
        L3_DEFIP_ALPM_ROUTE_FMT8_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(8, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(8, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(8, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_GET2(8, ENTRY_3f, base_entry[3]);
    } break;
    case 9: {
        L3_DEFIP_ALPM_ROUTE_FMT9_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(9, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(9, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(9, ENTRY_2f, base_entry[2]);
    } break;
    case 10: {
        L3_DEFIP_ALPM_ROUTE_FMT10_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(10, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(10, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(10, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_GET2(10, ENTRY_3f, base_entry[3]);
        ROUTE_FMT_FULL_FIELD_GET2(10, ENTRY_4f, base_entry[4]);
        ROUTE_FMT_FULL_FIELD_GET2(10, ENTRY_5f, base_entry[5]);
    } break;
    case 11: {
        L3_DEFIP_ALPM_ROUTE_FMT11_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(11, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(11, ENTRY_1f, base_entry[1]);
        ROUTE_FMT_FULL_FIELD_GET2(11, ENTRY_2f, base_entry[2]);
        ROUTE_FMT_FULL_FIELD_GET2(11, ENTRY_3f, base_entry[3]);
    } break;
    case 12: {
        L3_DEFIP_ALPM_ROUTE_FMT12_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(12, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(12, ENTRY_1f, base_entry[1]);
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
    } break;
    case 14: {
        L3_DEFIP_ALPM_ROUTE_FMT14_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(14, ENTRY_0f, base_entry[0]);
        ROUTE_FMT_FULL_FIELD_GET2(14, ENTRY_1f, base_entry[1]);
    } break;
    case 15: {
        L3_DEFIP_ALPM_ROUTE_FMT15_FULL_t *bucket = buf;
        ROUTE_FMT_FULL_FIELD_GET2(15, ENTRY_0f, base_entry[0]);
    } break;
    default:
        ALPM_ASSERT(0);
        break;
    }
}


static int
bcm56990_ln_base_entry_info(int u, int m,
                            format_type_t format_type,
                            uint8_t format_value,
                            int *prefix_len,
                            int *full,
                            int *num_entry)
{
    int a, b, c;
    if (format_type == FORMAT_ROUTE) {
        bcm56990_ln_route_base_entry_info(u, format_value, &a, &b, &c);
    } else {
        bcm56990_ln_pivot_base_entry_info(u, format_value, &a, &b, &c);
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
bcm56990_ln_route_format_set_data(uint8_t format_value,
                                  uint32_t *ad_format,
                                  void *buf)
{
    format_value = bcm56990_l2p_route_fmts[format_value];
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
    default:
        ALPM_ASSERT(0);
        break;
    }
}
static void
bcm56990_ln_route_format_set(uint8_t format_value,
                             uint32_t *ad_format,
                             alpm_format_ip_t prefix,
                             void *buf)
{
    format_value = bcm56990_l2p_route_fmts[format_value];
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
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56990_ln_route_format_encode(int u, uint8_t format_value,
                                uint8_t kshift,
                                alpm_arg_t *arg,
                                void *buf)
{
    int full;
    int max_len;
    int number;
    bcm56990_assoc_data_t ad_format;
    bcm56990_ln_route_base_entry_info(u, format_value, &max_len, &full,
                                      &number);
    bcm56990_ln_assoc_data_format_encode(u, &ad_format, full, &(arg->ad));
    if (arg->encode_data_only) {
        bcm56990_ln_route_format_set_data(format_value,
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
        bcm56990_ln_route_format_set(format_value,
                                     &ad_format.v[0],
                                     &prefix[0],
                                     buf);
    }
}

static void
bcm56990_ln_route_format_get_data(uint8_t format_value,
                                  uint32_t *ad_format,
                                  void *buf)
{
    format_value = bcm56990_l2p_route_fmts[format_value];
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
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56990_ln_route_format_get(uint8_t format_value,
                             uint32_t *ad_format,
                             alpm_format_ip_t prefix,
                             void *buf)
{
    format_value = bcm56990_l2p_route_fmts[format_value];
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
    default:
        ALPM_ASSERT(0);
        break;
    }
}

static void
bcm56990_ln_route_format_decode(int u, uint8_t format_value,
                                uint8_t kshift,
                                alpm_arg_t *arg,
                                void *buf)
{
    int full;
    int max_len;
    int number;
    bcm56990_assoc_data_t ad_format = {{{0}}};
    alpm_format_ip_t prefix = {0};
    bcm56990_ln_route_base_entry_info(u, format_value, &max_len, &full,
                                      &number);
    if (arg->decode_data_only) {
        bcm56990_ln_route_format_get_data(format_value,
                                          &ad_format.v[0],
                                          buf);
    } else {
        bcm56990_ln_route_format_get(format_value,
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
    bcm56990_ln_assoc_data_format_decode(u, &ad_format, full, &arg->ad,
                                         arg->key.t.app);
}

static int
bcm56990_ln_raw_bucket_encode(int u, int m, format_type_t format_type,
                              uint8_t format_value,
                              ln_base_entry_t *base_entry,
                              int entry_count,
                              void *bucket)
{
    if (format_type == FORMAT_ROUTE) {
        bcm56990_ln_route_format_full_encode(u, format_value, base_entry,
                                             entry_count, bucket);
    } else {
        bcm56990_ln_pivot_format_full_encode(u, format_value, base_entry,
                                             entry_count, bucket);
    }
    return SHR_E_NONE;
}


static int
bcm56990_ln_raw_bucket_entry_set(int u, int m, format_type_t format_type,
                                 uint8_t format_value,
                                 uint32_t *base_entry,
                                 int ent_ofs,
                                 void *bucket)
{
    if (format_type == FORMAT_ROUTE) {
        bcm56990_ln_route_format_full_encode2(u, format_value, base_entry,
                                              ent_ofs, bucket);
    } else {
        bcm56990_ln_pivot_format_full_encode2(u, format_value, base_entry,
                                              ent_ofs, bucket);
    }
    return SHR_E_NONE;
}


static int
bcm56990_ln_raw_bucket_decode(int u, int m, format_type_t format_type,
                              uint8_t format_value,
                              ln_base_entry_t *base_entry,
                              int entry_count,
                              void *bucket)
{
    if (format_type == FORMAT_ROUTE) {
        bcm56990_ln_route_format_full_decode(u, format_value, base_entry,
                                             entry_count, bucket);
    } else {
        bcm56990_ln_pivot_format_full_decode(u, format_value, base_entry,
                                             entry_count, bucket);
    }
    return SHR_E_NONE;
}


static int
bcm56990_ln_raw_bucket_entry_get(int u, int m, format_type_t format_type,
                                 uint8_t format_value,
                                 uint32_t *base_entry,
                                 int ent_ofs,
                                 void *bucket)
{
    if (format_type == FORMAT_ROUTE) {
        bcm56990_ln_route_format_full_decode2(u, format_value, base_entry,
                                              ent_ofs, bucket);
    } else {
        bcm56990_ln_pivot_format_full_decode2(u, format_value, base_entry,
                                              ent_ofs, bucket);
    }
    return SHR_E_NONE;
}


static int
bcm56990_ln_base_entry_encode(int u, int m, format_type_t format_type,
                              uint8_t format_value,
                              uint8_t kshift,
                              alpm_arg_t *arg,
                              uint32_t *entry)
{
    if (format_type == FORMAT_ROUTE) {
        bcm56990_ln_route_format_encode(u, format_value, kshift, arg, entry);
    } else {
        bcm56990_ln_pivot_format_encode(u, format_value, kshift, arg, entry);
    }
    return SHR_E_NONE;
}

static int
bcm56990_ln_base_entry_decode(int u, int m, format_type_t format_type,
                              uint8_t format_value,
                              uint8_t kshift,
                              alpm_arg_t *arg,
                              uint32_t *entry)
{
    if (format_type == FORMAT_ROUTE) {
        bcm56990_ln_route_format_decode(u, format_value, kshift, arg, entry);
    } else {
        bcm56990_ln_pivot_format_decode(u, format_value, kshift, arg, entry);
    }
    return SHR_E_NONE;
}

static int
bcm56990_ln_base_entry_move(int u, int m, format_type_t format_type,
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
        bcm56990_ln_raw_bucket_entry_get(u, m, format_type,
                                         src_format_value,
                                         &src_base_entry[0],
                                         src_ent_ofs,
                                         src_bucket));

    if (src_format_value == dst_format_value &&
        src_kshift == dst_kshift) {
#ifdef ALPM_DEBUG
        if (0) {
            arg = shr_lmm_alloc(bcmptm_rm_alpm_arg_lmem[u][ALPM_0]);
            if (arg == NULL) {
                SHR_ERR_EXIT(SHR_E_MEMORY);
            }
            sal_memset(arg, 0, sizeof(*arg));

            arg->key.t.max_bits = src_t->max_bits;
            sal_memcpy(&arg->key.t.trie_ip, &src_t->trie_ip,
                       sizeof(src_t->trie_ip));
            SHR_IF_ERR_EXIT(
                bcm56990_ln_base_entry_decode(u, m, format_type,
                                              src_format_value,
                                              src_kshift,
                                              arg,
                                              &src_base_entry[0]));
            ALPM_ASSERT(arg->key.t.len >= src_kshift);
            SHR_IF_ERR_EXIT(
                bcm56990_ln_base_entry_encode(u, m, format_type,
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
/*        bcmptm_rm_alpm_arg_init(u, arg);*/

        arg->key.t.max_bits = src_t->max_bits;
        sal_memcpy(&arg->key.t.trie_ip, &src_t->trie_ip, sizeof(src_t->trie_ip));
        SHR_IF_ERR_EXIT(
            bcm56990_ln_base_entry_decode(u, m, format_type,
                                          src_format_value,
                                          src_kshift,
                                          arg,
                                          &src_base_entry[0]));
        ALPM_ASSERT(arg->key.t.len >= src_kshift);
        ALPM_ASSERT(arg->key.t.len >= dst_kshift);
        SHR_IF_ERR_EXIT(
            bcm56990_ln_base_entry_encode(u, m, format_type,
                                          dst_format_value,
                                          dst_kshift,
                                          arg,
                                          &dst_base_entry[0]));
        dst_base = &dst_base_entry[0];
    }
    SHR_IF_ERR_EXIT(
        bcm56990_ln_raw_bucket_entry_set(u, m, format_type,
                                         dst_format_value,
                                         dst_base,
                                         dst_ent_ofs,
                                         dst_bucket));
    SHR_IF_ERR_EXIT(
        bcm56990_ln_raw_bucket_entry_set(u, m, format_type,
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
bcm56990_l1_fmt0_narrow_encode_data_n1(int u, alpm_arg_t *arg, UNPAIR0_T *de,
                                       UNPAIR0_T *se, int upkm)
{
    bcm56990_assoc_data_t ad_format;
    bcm56990_ln_assoc_data_format_encode(u, &ad_format, 1, &arg->ad);
    UNPAIR0_FIELD_SET2(ASSOC_DATA1f, ad_format);
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
bcm56990_l1_fmt0_narrow_lpm_encode_n1(int u, alpm_arg_t *arg, UNPAIR0_T *de,
                                  UNPAIR0_T *se, int upkm)
{
    L3_DEFIP_TCAM_KEY_FMT0_t kdata;
    L3_DEFIP_TCAM_KEY_FMT0_t kmask;
    uint32_t vrf, vrf_mask;
    bcm56990_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    uint8_t large_vrf;

    L3_DEFIP_TCAM_KEY_FMT0_CLR(kdata);
    L3_DEFIP_TCAM_KEY_FMT0_CLR(kmask);

    if (arg->key.vt == VRF_OVERRIDE) {
        UNPAIR0_FIELD_SET(FIXED_DATA1f, SUB_DB_PRIORITY_OVERRIDE);
        vrf = 0;
        vrf_mask = 0;
    } else if (arg->key.vt == VRF_GLOBAL) {
        UNPAIR0_FIELD_SET(FIXED_DATA1f, SUB_DB_PRIORITY_GLOBAL);
        vrf = 0;
        vrf_mask = 0;
    } else {
        UNPAIR0_FIELD_SET(FIXED_DATA1f, SUB_DB_PRIORITY_VRF);
        vrf = arg->key.t.w_vrf;
        vrf_mask = BCM56990_ALPM_VRF_MASK;
    }

    large_vrf = arg->lpm_large_vrf && arg->key.vt != VRF_OVERRIDE;
    if (arg->key.kt == KEY_IPV4) {
        arg->key.ip_addr[0] &= arg->key.ip_mask[0];

        if (large_vrf) {
            bcm56990_l1_key_fmt0_lpm_v4_encode_no_vrf(u, HALF_WIDE, IP_VER_4,
                                                      &arg->key.ip_addr[0], &kdata);
            bcm56990_l1_key_fmt0_lpm_v4_encode_no_vrf(u, 0x3, 0x1,
                                                      &arg->key.ip_mask[0], &kmask);
        } else {
            bcm56990_l1_key_fmt0_lpm_v4_encode(u, vrf, HALF_WIDE, IP_VER_4,
                                               arg->key.ip_addr[0], &kdata);
            bcm56990_l1_key_fmt0_lpm_v4_encode(u, vrf_mask, 0x3, 0x1,
                                               arg->key.ip_mask[0], &kmask);
        }
        UNPAIR0_FIELD_SET2(KEY1f, kdata);
        UNPAIR0_FIELD_SET2(MASK1f, kmask);
        UNPAIR0_FIELD_SET(VALID1f, arg->valid);
    } else if (arg->key.kt == KEY_IPV6_SINGLE &&
               (upkm == PKM_S || upkm == PKM_SS)) {
        /* Apply subnet kmask. */
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[0] == 0);
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[1] == 0);
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[2] == 0);
        arg->key.ip_addr[3] &= arg->key.ip_mask[3];

        if (large_vrf) {
            bcm56990_l1_key_fmt0_lpm_v4_encode_no_vrf(u, HALF_WIDE, IP_VER_6,
                                                      &arg->key.ip_addr[0], &kdata);
            bcm56990_l1_key_fmt0_lpm_v4_encode_no_vrf(u, 0x3, 0x1,
                                                      &arg->key.ip_mask[0], &kmask);
        } else {
            bcm56990_l1_key_fmt0_lpm_v4_encode(u, vrf, HALF_WIDE, IP_VER_6,
                                               arg->key.ip_addr[3], &kdata);
            bcm56990_l1_key_fmt0_lpm_v4_encode(u, vrf_mask, 0x3, 0x1,
                                               arg->key.ip_mask[3], &kmask);
        }
        UNPAIR0_FIELD_SET2(KEY1f, kdata);
        UNPAIR0_FIELD_SET2(MASK1f, kmask);
        UNPAIR0_FIELD_SET(VALID1f, arg->valid);
    }

    bcm56990_ln_alpm1_data_format_encode(u, &a1_format, &arg->a1, arg);
    bcm56990_ln_assoc_data_format_encode(u, &ad_format, 1, &arg->ad);
    UNPAIR0_FIELD_SET2(ALPM1_DATA1f, a1_format);
    UNPAIR0_FIELD_SET2(ASSOC_DATA1f, ad_format);
}

static void
bcm56990_l1_fmt0_narrow_encode_data(int u, alpm_arg_t *arg, UNPAIR0_T *de,
                                    UNPAIR0_T *se, int upkm)
{
    bcm56990_assoc_data_t ad_format;

    bcm56990_ln_assoc_data_format_encode(u, &ad_format, 1, &arg->ad);
    UNPAIR0_FIELD_SET2(ASSOC_DATA0f, ad_format);

    if (arg->key.kt != KEY_IPV4 &&
        !(arg->key.kt == KEY_IPV6_SINGLE &&
          (upkm == PKM_S || upkm == PKM_SS))) {
        UNPAIR0_FIELD_SET2(ASSOC_DATA1f, ad_format);
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
bcm56990_l1_fmt0_narrow_lpm_encode(int u, alpm_arg_t *arg, UNPAIR0_T *de,
                                   UNPAIR0_T *se, int upkm)
{
    L3_DEFIP_TCAM_KEY_FMT0_t kdata[2] = {{{0}}};
    L3_DEFIP_TCAM_KEY_FMT0_t kmask[2] = {{{0}}};
    uint32_t vrf, vrf_mask;
    bcm56990_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    uint8_t full = 0;
    uint8_t large_vrf;

    if (arg->key.vt == VRF_OVERRIDE) {
        UNPAIR0_FIELD_SET(FIXED_DATA0f, SUB_DB_PRIORITY_OVERRIDE);
        vrf = 0;
        vrf_mask = 0;
    } else if (arg->key.vt == VRF_GLOBAL) {
        UNPAIR0_FIELD_SET(FIXED_DATA0f, SUB_DB_PRIORITY_GLOBAL);
        vrf = 0;
        vrf_mask = 0;
    } else {
        UNPAIR0_FIELD_SET(FIXED_DATA0f, SUB_DB_PRIORITY_VRF);
        vrf = arg->key.t.w_vrf;
        vrf_mask = BCM56990_ALPM_VRF_MASK;
    }

    large_vrf = arg->lpm_large_vrf && arg->key.vt != VRF_OVERRIDE;
    if (arg->key.kt == KEY_IPV4) {
        arg->key.ip_addr[0] &= arg->key.ip_mask[0];

        if (large_vrf) {
            bcm56990_l1_key_fmt0_lpm_v4_encode_no_vrf(u, HALF_WIDE, IP_VER_4,
                                                      &arg->key.ip_addr[0], &kdata[0]);
            bcm56990_l1_key_fmt0_lpm_v4_encode_no_vrf(u, 0x3, 0x1,
                                                      &arg->key.ip_mask[0], &kmask[0]);
        } else {
            bcm56990_l1_key_fmt0_lpm_v4_encode(u, vrf, HALF_WIDE, IP_VER_4,
                                               arg->key.ip_addr[0], &kdata[0]);
            bcm56990_l1_key_fmt0_lpm_v4_encode(u, vrf_mask, 0x3, 0x1,
                                               arg->key.ip_mask[0], &kmask[0]);
        }
        UNPAIR0_FIELD_SET2(KEY0f, kdata[0]);
        UNPAIR0_FIELD_SET2(MASK0f, kmask[0]);
        UNPAIR0_FIELD_SET(VALID0f, arg->valid);
    } else if (arg->key.kt == KEY_IPV6_SINGLE &&
               (upkm == PKM_S || upkm == PKM_SS)) {
        /* Apply subnet kmask. */
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[0] == 0);
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[1] == 0);
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[2] == 0);
        arg->key.ip_addr[3] &= arg->key.ip_mask[3];

        if (large_vrf) {
            bcm56990_l1_key_fmt0_lpm_v4_encode_no_vrf(u, HALF_WIDE, IP_VER_6,
                                                      &arg->key.ip_addr[0], &kdata[0]);
            bcm56990_l1_key_fmt0_lpm_v4_encode_no_vrf(u, 0x3, 0x1,
                                                      &arg->key.ip_mask[0], &kmask[0]);
        } else {
            bcm56990_l1_key_fmt0_lpm_v4_encode(u, vrf, HALF_WIDE, IP_VER_6,
                                               arg->key.ip_addr[3], &kdata[0]);
            bcm56990_l1_key_fmt0_lpm_v4_encode(u, vrf_mask, 0x3, 0x1,
                                               arg->key.ip_mask[3], &kmask[0]);
        }
        UNPAIR0_FIELD_SET2(KEY0f, kdata[0]);
        UNPAIR0_FIELD_SET2(MASK0f, kmask[0]);
        UNPAIR0_FIELD_SET(VALID0f, arg->valid);
    } else {
        /* Apply subnet kmask. */
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[0] == 0);
        arg->key.ip_addr[1] &= arg->key.ip_mask[1];
        arg->key.ip_addr[2] &= arg->key.ip_mask[2];
        arg->key.ip_addr[3] &= arg->key.ip_mask[3];

        if (large_vrf) {
            bcm56990_l1_key_fmt0_lpm_v6_single_encode_no_vrf(u, SINGLE_WIDE,
                                                             IP_VER_6,
                                                             &arg->key.ip_addr[0],
                                                             &kdata[0],
                                                             &kdata[1]);
            bcm56990_l1_key_fmt0_lpm_v6_single_encode_no_vrf(u, 0x3, 0x1,
                                                             &arg->key.ip_mask[0],
                                                             &kmask[0],
                                                             &kmask[1]);
        } else {
            bcm56990_l1_key_fmt0_lpm_v6_single_encode(u, vrf, SINGLE_WIDE,
                                                      IP_VER_6,
                                                      &arg->key.ip_addr[0],
                                                      &kdata[0],
                                                      &kdata[1]);
            bcm56990_l1_key_fmt0_lpm_v6_single_encode(u, vrf_mask, 0x3, 0x1,
                                                      &arg->key.ip_mask[0],
                                                      &kmask[0],
                                                      &kmask[1]);
        }


        UNPAIR0_FIELD_SET2(KEY0f, kdata[0]);
        UNPAIR0_FIELD_SET2(KEY1f, kdata[1]);
        UNPAIR0_FIELD_SET2(MASK0f, kmask[0]);
        UNPAIR0_FIELD_SET2(MASK1f, kmask[1]);
        UNPAIR0_FIELD_SET(VALID0f, arg->valid);
        UNPAIR0_FIELD_SET(VALID1f, arg->valid);
        full = 1;
    }

    bcm56990_ln_alpm1_data_format_encode(u, &a1_format, &arg->a1, arg);
    bcm56990_ln_assoc_data_format_encode(u, &ad_format, 1, &arg->ad);
    UNPAIR0_FIELD_SET2(ALPM1_DATA0f, a1_format);
    UNPAIR0_FIELD_SET2(ASSOC_DATA0f, ad_format);
    UNPAIR0_FIELD_SET(PARITY0f, arg->hit);

    if (full) {
        UNPAIR0_FIELD_SET2(ALPM1_DATA1f, a1_format);
        UNPAIR0_FIELD_SET2(ASSOC_DATA1f, ad_format);
    }
}

static void
bcm56990_l1_fmt0_wide_encode_data(int u, alpm_arg_t *arg, PAIR0_T *de, PAIR0_T *se)
{
    bcm56990_assoc_data_t ad_format;
    bcm56990_ln_assoc_data_format_encode(u, &ad_format, 1, &arg->ad);
    PAIR0_FIELD_SET2(LWR_ASSOC_DATA0f, ad_format);
    PAIR0_FIELD_SET2(LWR_ASSOC_DATA1f, ad_format);
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
bcm56990_l1_fmt0_wide_lpm_encode(int u, alpm_arg_t *arg, PAIR0_T *de, PAIR0_T *se)
{
    L3_DEFIP_TCAM_KEY_FMT0_t kdata[4] = {{{0}}};
    L3_DEFIP_TCAM_KEY_FMT0_t kmask[4] = {{{0}}};
    int vrf, vrf_mask;
    bcm56990_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    uint8_t large_vrf;

    if (arg->key.vt == VRF_OVERRIDE) {
        PAIR0_FIELD_SET(LWR_FIXED_DATA1f, SUB_DB_PRIORITY_OVERRIDE);
        PAIR0_FIELD_SET(LWR_FIXED_DATA0f, SUB_DB_PRIORITY_OVERRIDE);
        vrf = 0;
        vrf_mask = 0;
    } else if (arg->key.vt == VRF_GLOBAL) {
        PAIR0_FIELD_SET(LWR_FIXED_DATA1f, SUB_DB_PRIORITY_GLOBAL);
        PAIR0_FIELD_SET(LWR_FIXED_DATA0f, SUB_DB_PRIORITY_GLOBAL);
        vrf = 0;
        vrf_mask = 0;
    } else {
        PAIR0_FIELD_SET(LWR_FIXED_DATA1f, SUB_DB_PRIORITY_VRF);
        PAIR0_FIELD_SET(LWR_FIXED_DATA0f, SUB_DB_PRIORITY_VRF);
        vrf = arg->key.t.w_vrf;
        vrf_mask = BCM56990_ALPM_VRF_MASK;
    }

    /* Apply subnet kmask. */
    arg->key.ip_addr[0] &= arg->key.ip_mask[0];
    arg->key.ip_addr[1] &= arg->key.ip_mask[1];
    arg->key.ip_addr[2] &= arg->key.ip_mask[2];
    arg->key.ip_addr[3] &= arg->key.ip_mask[3];

    large_vrf = arg->lpm_large_vrf && arg->key.vt != VRF_OVERRIDE;
    if (large_vrf) {
        bcm56990_l1_key_fmt0_lpm_v6_double_encode_no_vrf(u, DOUBLE_WIDE, IP_VER_6,
                                                         &arg->key.ip_addr[0],
                                                         &kdata[0],
                                                         &kdata[1],
                                                         &kdata[2],
                                                         &kdata[3]);
        bcm56990_l1_key_fmt0_lpm_v6_double_encode_no_vrf(u, 0x3, 0x1,
                                                         &arg->key.ip_mask[0],
                                                         &kmask[0],
                                                         &kmask[1],
                                                         &kmask[2],
                                                         &kmask[3]);
    } else {
        bcm56990_l1_key_fmt0_lpm_v6_double_encode(u, vrf, DOUBLE_WIDE, IP_VER_6,
                                                  &arg->key.ip_addr[0],
                                                  &kdata[0],
                                                  &kdata[1],
                                                  &kdata[2],
                                                  &kdata[3]);
        bcm56990_l1_key_fmt0_lpm_v6_double_encode(u, vrf_mask, 0x3, 0x1,
                                                  &arg->key.ip_mask[0],
                                                  &kmask[0],
                                                  &kmask[1],
                                                  &kmask[2],
                                                  &kmask[3]);
    }

    PAIR0_FIELD_SET2(LWR_KEY0f, kdata[0]);
    PAIR0_FIELD_SET2(LWR_KEY1f, kdata[1]);
    PAIR0_FIELD_SET2(UPR_KEY0f, kdata[2]);
    PAIR0_FIELD_SET2(UPR_KEY1f, kdata[3]);
    PAIR0_FIELD_SET2(LWR_MASK0f, kmask[0]);
    PAIR0_FIELD_SET2(LWR_MASK1f, kmask[1]);
    PAIR0_FIELD_SET2(UPR_MASK0f, kmask[2]);
    PAIR0_FIELD_SET2(UPR_MASK1f, kmask[3]);
    PAIR0_FIELD_SET(LWR_VALID0f, arg->valid);
    PAIR0_FIELD_SET(LWR_VALID1f, arg->valid);
    PAIR0_FIELD_SET(UPR_VALID0f, arg->valid);
    PAIR0_FIELD_SET(UPR_VALID1f, arg->valid);

    bcm56990_ln_alpm1_data_format_encode(u, &a1_format, &arg->a1, arg);
    bcm56990_ln_assoc_data_format_encode(u, &ad_format, 1, &arg->ad);

    PAIR0_FIELD_SET2(LWR_ALPM1_DATA0f, a1_format);
    PAIR0_FIELD_SET2(LWR_ALPM1_DATA1f, a1_format);
    PAIR0_FIELD_SET2(LWR_ASSOC_DATA0f, ad_format);
    PAIR0_FIELD_SET2(LWR_ASSOC_DATA1f, ad_format);
    PAIR0_FIELD_SET(LWR_PARITY0f, arg->hit);
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
bcm56990_l1_fmt1_narrow_lpm_encode_n1(int u, alpm_arg_t *arg, UNPAIR1_T *de,
                                  UNPAIR1_T *se, int upkm)
{
    L3_DEFIP_TCAM_KEY_FMT1_t kdata;
    L3_DEFIP_TCAM_KEY_FMT1_t kmask;
    uint32_t vrf, vrf_mask;
    bcm56990_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    uint8_t large_vrf;

    L3_DEFIP_TCAM_KEY_FMT1_CLR(kdata);
    L3_DEFIP_TCAM_KEY_FMT1_CLR(kmask);

    if (arg->key.vt == VRF_OVERRIDE) {
        UNPAIR1_FIELD_SET(FIXED_DATA1f, SUB_DB_PRIORITY_OVERRIDE);
        vrf = 0;
        vrf_mask = 0;
    } else if (arg->key.vt == VRF_GLOBAL) {
        UNPAIR1_FIELD_SET(FIXED_DATA1f, SUB_DB_PRIORITY_GLOBAL);
        vrf = 0;
        vrf_mask = 0;
    } else {
        UNPAIR1_FIELD_SET(FIXED_DATA1f, SUB_DB_PRIORITY_VRF);
        vrf = arg->key.t.w_vrf;
        vrf_mask = BCM56990_ALPM_VRF_MASK;
    }

    large_vrf = arg->lpm_large_vrf && arg->key.vt != VRF_OVERRIDE;
    if (arg->key.kt == KEY_IPV4) {
        arg->key.ip_addr[0] &= arg->key.ip_mask[0];

        if (large_vrf) {
            bcm56990_l1_key_fmt1_lpm_v4_encode_no_vrf(u, HALF_WIDE, IP_VER_4,
                                                      &arg->key.ip_addr[0], &kdata);
            bcm56990_l1_key_fmt1_lpm_v4_encode_no_vrf(u, 0x3, 0x3,
                                                      &arg->key.ip_mask[0], &kmask);

        } else {
            bcm56990_l1_key_fmt1_lpm_v4_encode(u, vrf, HALF_WIDE, IP_VER_4,
                                               arg->key.ip_addr[0], &kdata);
            bcm56990_l1_key_fmt1_lpm_v4_encode(u, vrf_mask, 0x3, 0x3,
                                               arg->key.ip_mask[0], &kmask);

        }

        UNPAIR1_FIELD_SET2(KEY1f, kdata);
        UNPAIR1_FIELD_SET2(MASK1f, kmask);
        UNPAIR1_FIELD_SET(VALID1f, arg->valid);
    } else if (arg->key.kt == KEY_IPV6_SINGLE &&
               (upkm == PKM_S || upkm == PKM_SS)) {
        /* Apply subnet kmask. */
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[0] == 0);
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[1] == 0);
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[2] == 0);
        arg->key.ip_addr[3] &= arg->key.ip_mask[3];

        if (large_vrf) {
            bcm56990_l1_key_fmt1_lpm_v4_encode_no_vrf(u, HALF_WIDE, IP_VER_6,
                                                      &arg->key.ip_addr[0], &kdata);
            bcm56990_l1_key_fmt1_lpm_v4_encode_no_vrf(u, 0x3, 0x3,
                                                      &arg->key.ip_mask[0], &kmask);
        } else {
            bcm56990_l1_key_fmt1_lpm_v4_encode(u, vrf, HALF_WIDE, IP_VER_6,
                                               arg->key.ip_addr[3], &kdata);
            bcm56990_l1_key_fmt1_lpm_v4_encode(u, vrf_mask, 0x3, 0x3,
                                               arg->key.ip_mask[3], &kmask);
        }
        UNPAIR1_FIELD_SET2(KEY1f, kdata);
        UNPAIR1_FIELD_SET2(MASK1f, kmask);
        UNPAIR1_FIELD_SET(VALID1f, arg->valid);
    }

    bcm56990_ln_alpm1_data_format_encode(u, &a1_format, &arg->a1, arg);
    bcm56990_ln_assoc_data_format_encode(u, &ad_format, 1, &arg->ad);
    UNPAIR1_FIELD_SET2(ALPM1_DATA1f, a1_format);
    UNPAIR1_FIELD_SET2(ASSOC_DATA1f, ad_format);
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
bcm56990_l1_fmt1_narrow_lpm_encode(int u, alpm_arg_t *arg, UNPAIR1_T *de,
                                   UNPAIR1_T *se, int upkm)
{
    L3_DEFIP_TCAM_KEY_FMT1_t kdata[2] = {{{0}}};
    L3_DEFIP_TCAM_KEY_FMT1_t kmask[2] = {{{0}}};
    uint32_t vrf, vrf_mask;
    bcm56990_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    uint8_t full = 0;
    uint8_t large_vrf;

    if (arg->key.vt == VRF_OVERRIDE) {
        UNPAIR1_FIELD_SET(FIXED_DATA0f, SUB_DB_PRIORITY_OVERRIDE);
        vrf = 0;
        vrf_mask = 0;
    } else if (arg->key.vt == VRF_GLOBAL) {
        UNPAIR1_FIELD_SET(FIXED_DATA0f, SUB_DB_PRIORITY_GLOBAL);
        vrf = 0;
        vrf_mask = 0;
    } else {
        UNPAIR1_FIELD_SET(FIXED_DATA0f, SUB_DB_PRIORITY_VRF);
        vrf = arg->key.t.w_vrf;
        vrf_mask = BCM56990_ALPM_VRF_MASK;
    }

    large_vrf = arg->lpm_large_vrf && arg->key.vt != VRF_OVERRIDE;
    if (arg->key.kt == KEY_IPV4) {
        arg->key.ip_addr[0] &= arg->key.ip_mask[0];

        if (large_vrf) {
            bcm56990_l1_key_fmt1_lpm_v4_encode_no_vrf(u, HALF_WIDE, IP_VER_4,
                                                      &arg->key.ip_addr[0], &kdata[0]);
            bcm56990_l1_key_fmt1_lpm_v4_encode_no_vrf(u, 0x3, 0x3,
                                                      &arg->key.ip_mask[0], &kmask[0]);

        } else {
            bcm56990_l1_key_fmt1_lpm_v4_encode(u, vrf, HALF_WIDE, IP_VER_4,
                                               arg->key.ip_addr[0], &kdata[0]);
            bcm56990_l1_key_fmt1_lpm_v4_encode(u, vrf_mask, 0x3, 0x3,
                                               arg->key.ip_mask[0], &kmask[0]);

        }

        UNPAIR1_FIELD_SET2(KEY0f, kdata[0]);
        UNPAIR1_FIELD_SET2(MASK0f, kmask[0]);
        UNPAIR1_FIELD_SET(VALID0f, arg->valid);
    } else if (arg->key.kt == KEY_IPV6_SINGLE &&
               (upkm == PKM_S || upkm == PKM_SS)) {
        /* Apply subnet kmask. */
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[0] == 0);
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[1] == 0);
        ALPM_ASSERT_ERRMSG(arg->key.ip_mask[2] == 0);
        arg->key.ip_addr[3] &= arg->key.ip_mask[3];

        if (large_vrf) {
            bcm56990_l1_key_fmt1_lpm_v4_encode_no_vrf(u, HALF_WIDE, IP_VER_6,
                                                      &arg->key.ip_addr[0], &kdata[0]);
            bcm56990_l1_key_fmt1_lpm_v4_encode_no_vrf(u, 0x3, 0x3,
                                                      &arg->key.ip_mask[0], &kmask[0]);

        } else {
            bcm56990_l1_key_fmt1_lpm_v4_encode(u, vrf, HALF_WIDE, IP_VER_6,
                                               arg->key.ip_addr[3], &kdata[0]);
            bcm56990_l1_key_fmt1_lpm_v4_encode(u, vrf_mask, 0x3, 0x3,
                                               arg->key.ip_mask[3], &kmask[0]);
        }
        UNPAIR1_FIELD_SET2(KEY0f, kdata[0]);
        UNPAIR1_FIELD_SET2(MASK0f, kmask[0]);
        UNPAIR1_FIELD_SET(VALID0f, arg->valid);
    } else {
        /* Apply subnet kmask. */
        arg->key.ip_addr[0] &= arg->key.ip_mask[0];
        arg->key.ip_addr[1] &= arg->key.ip_mask[1];
        arg->key.ip_addr[2] &= arg->key.ip_mask[2];
        arg->key.ip_addr[3] &= arg->key.ip_mask[3];

        if (large_vrf) {
            bcm56990_l1_key_fmt1_lpm_v6_single_encode_no_vrf(u, SINGLE_WIDE,
                                                             IP_VER_6,
                                                             &arg->key.ip_addr[0],
                                                             &kdata[0],
                                                             &kdata[1]);
            bcm56990_l1_key_fmt1_lpm_v6_single_encode_no_vrf(u, 0x3, 0x3,
                                                             &arg->key.ip_mask[0],
                                                             &kmask[0],
                                                             &kmask[1]);
        } else {
            bcm56990_l1_key_fmt1_lpm_v6_single_encode(u, vrf, SINGLE_WIDE,
                                                      IP_VER_6,
                                                      &arg->key.ip_addr[0],
                                                      &kdata[0],
                                                      &kdata[1]);
            bcm56990_l1_key_fmt1_lpm_v6_single_encode(u, vrf_mask, 0x3, 0x3,
                                                      &arg->key.ip_mask[0],
                                                      &kmask[0],
                                                      &kmask[1]);
        }

        UNPAIR1_FIELD_SET2(KEY0f, kdata[0]);
        UNPAIR1_FIELD_SET2(KEY1f, kdata[1]);
        UNPAIR1_FIELD_SET2(MASK0f, kmask[0]);
        UNPAIR1_FIELD_SET2(MASK1f, kmask[1]);
        UNPAIR1_FIELD_SET(VALID0f, arg->valid);
        UNPAIR1_FIELD_SET(VALID1f, arg->valid);
        full = 1;
    }

    bcm56990_ln_alpm1_data_format_encode(u, &a1_format, &arg->a1, arg);
    bcm56990_ln_assoc_data_format_encode(u, &ad_format, 1, &arg->ad);
    UNPAIR1_FIELD_SET2(ALPM1_DATA0f, a1_format);
    UNPAIR1_FIELD_SET2(ASSOC_DATA0f, ad_format);

    if (full) {
        UNPAIR1_FIELD_SET2(ALPM1_DATA1f, a1_format);
        UNPAIR1_FIELD_SET2(ASSOC_DATA1f, ad_format);
    }
}

static void
bcm56990_l1_fmt1_wide_encode_data(int u, alpm_arg_t *arg, PAIR1_T *de, PAIR1_T *se)
{
    bcm56990_assoc_data_t ad_format;
    bcm56990_ln_assoc_data_format_encode(u, &ad_format, 1, &arg->ad);
    PAIR1_FIELD_SET2(LWR_ASSOC_DATA0f, ad_format);
    PAIR1_FIELD_SET2(LWR_ASSOC_DATA1f, ad_format);
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
bcm56990_l1_fmt1_wide_lpm_encode(int u, alpm_arg_t *arg, PAIR1_T *de, PAIR1_T *se)
{
    L3_DEFIP_TCAM_KEY_FMT1_t kdata[4] = {{{0}}};
    L3_DEFIP_TCAM_KEY_FMT1_t kmask[4] = {{{0}}};
    int vrf, vrf_mask;
    bcm56990_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    uint8_t large_vrf;

    if (arg->key.vt == VRF_OVERRIDE) {
        PAIR1_FIELD_SET(LWR_FIXED_DATA1f, SUB_DB_PRIORITY_OVERRIDE);
        PAIR1_FIELD_SET(LWR_FIXED_DATA0f, SUB_DB_PRIORITY_OVERRIDE);
        vrf = 0;
        vrf_mask = 0;
    } else if (arg->key.vt == VRF_GLOBAL) {
        PAIR1_FIELD_SET(LWR_FIXED_DATA1f, SUB_DB_PRIORITY_GLOBAL);
        PAIR1_FIELD_SET(LWR_FIXED_DATA0f, SUB_DB_PRIORITY_GLOBAL);
        vrf = 0;
        vrf_mask = 0;
    } else {
        PAIR1_FIELD_SET(LWR_FIXED_DATA1f, SUB_DB_PRIORITY_VRF);
        PAIR1_FIELD_SET(LWR_FIXED_DATA0f, SUB_DB_PRIORITY_VRF);
        vrf = arg->key.t.w_vrf;
        vrf_mask = BCM56990_ALPM_VRF_MASK;
    }

    /* Apply subnet kmask. */
    arg->key.ip_addr[0] &= arg->key.ip_mask[0];
    arg->key.ip_addr[1] &= arg->key.ip_mask[1];
    arg->key.ip_addr[2] &= arg->key.ip_mask[2];
    arg->key.ip_addr[3] &= arg->key.ip_mask[3];


    large_vrf = arg->lpm_large_vrf && arg->key.vt != VRF_OVERRIDE;
    if (large_vrf) {
        bcm56990_l1_key_fmt1_lpm_v6_double_encode_no_vrf(u, DOUBLE_WIDE, IP_VER_6,
                                                         &arg->key.ip_addr[0],
                                                         &kdata[0],
                                                         &kdata[1],
                                                         &kdata[2],
                                                         &kdata[3]);
        bcm56990_l1_key_fmt1_lpm_v6_double_encode_no_vrf(u, 0x3, 0x3,
                                                         &arg->key.ip_mask[0],
                                                         &kmask[0],
                                                         &kmask[1],
                                                         &kmask[2],
                                                         &kmask[3]);

    } else {
        bcm56990_l1_key_fmt1_lpm_v6_double_encode(u, vrf, DOUBLE_WIDE, IP_VER_6,
                                                  &arg->key.ip_addr[0],
                                                  &kdata[0],
                                                  &kdata[1],
                                                  &kdata[2],
                                                  &kdata[3]);
        bcm56990_l1_key_fmt1_lpm_v6_double_encode(u, vrf_mask, 0x3, 0x3,
                                                  &arg->key.ip_mask[0],
                                                  &kmask[0],
                                                  &kmask[1],
                                                  &kmask[2],
                                                  &kmask[3]);
    }

    PAIR1_FIELD_SET2(LWR_KEY0f, kdata[0]);
    PAIR1_FIELD_SET2(LWR_KEY1f, kdata[1]);
    PAIR1_FIELD_SET2(UPR_KEY0f, kdata[2]);
    PAIR1_FIELD_SET2(UPR_KEY1f, kdata[3]);
    PAIR1_FIELD_SET2(LWR_MASK0f, kmask[0]);
    PAIR1_FIELD_SET2(LWR_MASK1f, kmask[1]);
    PAIR1_FIELD_SET2(UPR_MASK0f, kmask[2]);
    PAIR1_FIELD_SET2(UPR_MASK1f, kmask[3]);
    PAIR1_FIELD_SET(LWR_VALID0f, arg->valid);
    PAIR1_FIELD_SET(LWR_VALID1f, arg->valid);
    PAIR1_FIELD_SET(UPR_VALID0f, arg->valid);
    PAIR1_FIELD_SET(UPR_VALID1f, arg->valid);

    bcm56990_ln_alpm1_data_format_encode(u, &a1_format, &arg->a1, arg);
    bcm56990_ln_assoc_data_format_encode(u, &ad_format, 1, &arg->ad);

    PAIR1_FIELD_SET2(LWR_ALPM1_DATA0f, a1_format);
    PAIR1_FIELD_SET2(LWR_ALPM1_DATA1f, a1_format);
    PAIR1_FIELD_SET2(LWR_ASSOC_DATA0f, ad_format);
    PAIR1_FIELD_SET2(LWR_ASSOC_DATA1f, ad_format);
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
bcm56990_l1_fmt1_narrow_encode_data_n1(int u, alpm_arg_t *arg, UNPAIR1_T *de,
                                       UNPAIR1_T *se, int upkm)
{
    bcm56990_assoc_data_t ad_format;
    bcm56990_ln_assoc_data_format_encode(u, &ad_format, 1, &arg->ad);
    UNPAIR1_FIELD_SET2(ASSOC_DATA1f, ad_format);
}


static void
bcm56990_l1_fmt1_narrow_encode_data(int u, alpm_arg_t *arg, UNPAIR1_T *de,
                                    UNPAIR1_T *se, int upkm)
{
    bcm56990_assoc_data_t ad_format;

    bcm56990_ln_assoc_data_format_encode(u, &ad_format, 1, &arg->ad);
    UNPAIR1_FIELD_SET2(ASSOC_DATA0f, ad_format);

    if (arg->key.kt != KEY_IPV4 &&
        !(arg->key.kt == KEY_IPV6_SINGLE &&
        (upkm == PKM_S || upkm == PKM_SS))) {
        UNPAIR1_FIELD_SET2(ASSOC_DATA1f, ad_format);
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
bcm56990_l1_fmt1_narrow_l3mc_encode(int u, alpm_arg_t *arg, UNPAIR1_T *de,
                                   UNPAIR1_T *se, int upkm)
{
    L3_DEFIP_TCAM_KEY_FMT1_t kdata[2] = {{{0}}};
    L3_DEFIP_TCAM_KEY_FMT1_t kmask[2] = {{{0}}};
    bcm56990_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;

    UNPAIR1_FIELD_SET(FIXED_DATA0f, SUB_DB_PRIORITY_VRF);
    bcm56990_l1_key_fmt1_l3mc_v4_single_encode(u, SINGLE_WIDE,
                                              FMT1_KT_L3MC_V4,
                                              &arg->key.ip_addr[0],
                                              &kdata[0],
                                              &kdata[1]);
    bcm56990_l1_key_fmt1_l3mc_v4_single_encode(u, 0x3, 0x3,
                                              &arg->key.ip_mask[0],
                                              &kmask[0],
                                              &kmask[1]);

    UNPAIR1_FIELD_SET2(KEY0f, kdata[0]);
    UNPAIR1_FIELD_SET2(KEY1f, kdata[1]);
    UNPAIR1_FIELD_SET2(MASK0f, kmask[0]);
    UNPAIR1_FIELD_SET2(MASK1f, kmask[1]);
    UNPAIR1_FIELD_SET(VALID0f, arg->valid);
    UNPAIR1_FIELD_SET(VALID1f, arg->valid);

    bcm56990_ln_alpm1_data_format_encode(u, &a1_format, &arg->a1, arg);
    bcm56990_ln_assoc_data_format_encode(u, &ad_format, 1, &arg->ad);
    UNPAIR1_FIELD_SET2(ALPM1_DATA0f, a1_format);
    UNPAIR1_FIELD_SET2(ASSOC_DATA0f, ad_format);
    UNPAIR1_FIELD_SET(PARITY0f, arg->hit);

    UNPAIR1_FIELD_SET2(ALPM1_DATA1f, a1_format);
    UNPAIR1_FIELD_SET2(ASSOC_DATA1f, ad_format);
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
bcm56990_l1_fmt1_wide_l3mc_encode(int u, alpm_arg_t *arg, PAIR1_T *de, PAIR1_T *se)
{
    L3_DEFIP_TCAM_KEY_FMT1_t kdata[4] = {{{0}}};
    L3_DEFIP_TCAM_KEY_FMT1_t kmask[4] = {{{0}}};
    bcm56990_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;

    PAIR1_FIELD_SET(LWR_FIXED_DATA1f, SUB_DB_PRIORITY_VRF);
    PAIR1_FIELD_SET(LWR_FIXED_DATA0f, SUB_DB_PRIORITY_VRF);
    bcm56990_l1_key_fmt1_l3mc_v6_double_encode(u, DOUBLE_WIDE, FMT1_KT_L3MC_V6,
                                              &arg->key.ip_addr[0],
                                              &kdata[0],
                                              &kdata[1],
                                              &kdata[2],
                                              &kdata[3]);
    bcm56990_l1_key_fmt1_l3mc_v6_double_encode(u, 0x3, 0x3,
                                              &arg->key.ip_mask[0],
                                              &kmask[0],
                                              &kmask[1],
                                              &kmask[2],
                                              &kmask[3]);

    PAIR1_FIELD_SET2(LWR_KEY0f, kdata[0]);
    PAIR1_FIELD_SET2(LWR_KEY1f, kdata[1]);
    PAIR1_FIELD_SET2(UPR_KEY0f, kdata[2]);
    PAIR1_FIELD_SET2(UPR_KEY1f, kdata[3]);
    PAIR1_FIELD_SET2(LWR_MASK0f, kmask[0]);
    PAIR1_FIELD_SET2(LWR_MASK1f, kmask[1]);
    PAIR1_FIELD_SET2(UPR_MASK0f, kmask[2]);
    PAIR1_FIELD_SET2(UPR_MASK1f, kmask[3]);
    PAIR1_FIELD_SET(LWR_VALID0f, arg->valid);
    PAIR1_FIELD_SET(LWR_VALID1f, arg->valid);
    PAIR1_FIELD_SET(UPR_VALID0f, arg->valid);
    PAIR1_FIELD_SET(UPR_VALID1f, arg->valid);

    bcm56990_ln_alpm1_data_format_encode(u, &a1_format, &arg->a1, arg);
    bcm56990_ln_assoc_data_format_encode(u, &ad_format, 1, &arg->ad);

    PAIR1_FIELD_SET2(LWR_ALPM1_DATA0f, a1_format);
    PAIR1_FIELD_SET2(LWR_ALPM1_DATA1f, a1_format);
    PAIR1_FIELD_SET2(LWR_ASSOC_DATA0f, ad_format);
    PAIR1_FIELD_SET2(LWR_ASSOC_DATA1f, ad_format);
    PAIR1_FIELD_SET(LWR_PARITY0f, arg->hit);
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
bcm56990_l1_fmt0_wide_comp_encode(int u, alpm_arg_t *arg, PAIR0_T *de, PAIR0_T *se)
{
    L3_DEFIP_TCAM_KEY_FMT0_t kdata[4] = {{{0}}};
    L3_DEFIP_TCAM_KEY_FMT0_t kmask[4] = {{{0}}};
    bcm56990_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    uint8_t sub_db_prio;
    uint32_t vrf, vrf_mask;

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
        vrf_mask = BCM56990_ALPM_COMP_PORT_MASK;
    }

    PAIR0_FIELD_SET(LWR_FIXED_DATA1f, sub_db_prio);
    PAIR0_FIELD_SET(LWR_FIXED_DATA0f, sub_db_prio);

    if (arg->key.t.ipv == IP_VER_4) {
        bcm56990_l1_key_fmt0_comp_v4_double_encode(u, arg->comp_key_type,
                                                  vrf, DOUBLE_WIDE, IP_VER_4,
                                                  &arg->key.ip_addr[0],
                                                  &kdata[0],
                                                  &kdata[1],
                                                  &kdata[2],
                                                  &kdata[3]);
        bcm56990_l1_key_fmt0_comp_v4_double_encode(u, arg->comp_key_type,
                                                  vrf_mask, 0x3, 0x1,
                                                  &arg->key.ip_mask[0],
                                                  &kmask[0],
                                                  &kmask[1],
                                                  &kmask[2],
                                                  &kmask[3]);
    } else {
        bcm56990_l1_key_fmt0_comp_v6_double_encode(u, arg->comp_key_type,
                                                  vrf, DOUBLE_WIDE, IP_VER_6,
                                                  &arg->key.ip_addr[0],
                                                  &kdata[0],
                                                  &kdata[1],
                                                  &kdata[2],
                                                  &kdata[3]);
        bcm56990_l1_key_fmt0_comp_v6_double_encode(u, arg->comp_key_type,
                                                  vrf_mask, 0x3, 0x1,
                                                  &arg->key.ip_mask[0],
                                                  &kmask[0],
                                                  &kmask[1],
                                                  &kmask[2],
                                                  &kmask[3]);
    }

    PAIR0_FIELD_SET2(LWR_KEY0f, kdata[0]);
    PAIR0_FIELD_SET2(LWR_KEY1f, kdata[1]);
    PAIR0_FIELD_SET2(UPR_KEY0f, kdata[2]);
    PAIR0_FIELD_SET2(UPR_KEY1f, kdata[3]);
    PAIR0_FIELD_SET2(LWR_MASK0f, kmask[0]);
    PAIR0_FIELD_SET2(LWR_MASK1f, kmask[1]);
    PAIR0_FIELD_SET2(UPR_MASK0f, kmask[2]);
    PAIR0_FIELD_SET2(UPR_MASK1f, kmask[3]);
    PAIR0_FIELD_SET(LWR_VALID0f, arg->valid);
    PAIR0_FIELD_SET(LWR_VALID1f, arg->valid);
    PAIR0_FIELD_SET(UPR_VALID0f, arg->valid);
    PAIR0_FIELD_SET(UPR_VALID1f, arg->valid);

    bcm56990_ln_alpm1_data_format_encode(u, &a1_format, &arg->a1, arg);
    bcm56990_ln_assoc_data_format_encode(u, &ad_format, 1, &arg->ad);

    PAIR0_FIELD_SET2(LWR_ALPM1_DATA0f, a1_format);
    PAIR0_FIELD_SET2(LWR_ALPM1_DATA1f, a1_format);
    PAIR0_FIELD_SET2(LWR_ASSOC_DATA0f, ad_format);
    PAIR0_FIELD_SET2(LWR_ASSOC_DATA1f, ad_format);
    PAIR0_FIELD_SET(LWR_PARITY0f, arg->hit);
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
bcm56990_l1_encode(int u, int m, void *varg, void *de, void *se, uint8_t l1v)
{
    alpm_arg_t *arg = varg;

    if (WIDE_PKM(arg->pkm)) {
        if (arg->key.t.app == APP_L3MC) {
            if (l1v == L1V_WIDE) {
                if (arg->encode_data_only) {
                    bcm56990_l1_fmt1_wide_encode_data(u, arg, de, se);
                } else {
                    bcm56990_l1_fmt1_wide_l3mc_encode(u, arg, de, se);
                }
            } else {
                if (arg->encode_data_only) {
                    bcm56990_l1_fmt1_narrow_encode_data(u, arg, de, se, arg->pkm);
                } else {
                    bcm56990_l1_fmt1_narrow_l3mc_encode(u, arg, de, se, arg->pkm);
                }
            }
        } else {
            if (l1v == L1V_WIDE) {
                if (arg->encode_data_only) {
                    bcm56990_l1_fmt1_wide_encode_data(u, arg, de, se);
                } else {
                    bcm56990_l1_fmt1_wide_lpm_encode(u, arg, de, se);
                }
            } else if (l1v == L1V_NARROW_1) {
                if (arg->encode_data_only) {
                    bcm56990_l1_fmt1_narrow_encode_data_n1(u, arg, de, se, arg->pkm);
                } else {
                    bcm56990_l1_fmt1_narrow_lpm_encode_n1(u, arg, de, se, arg->pkm);
                }
            } else {
                if (arg->encode_data_only) {
                    bcm56990_l1_fmt1_narrow_encode_data(u, arg, de, se, arg->pkm);
                } else {
                    bcm56990_l1_fmt1_narrow_lpm_encode(u, arg, de, se, arg->pkm);
                }
            }
        }
    } else {
        if (arg->key.t.app == APP_COMP1) {
            if (arg->encode_data_only) {
                bcm56990_l1_fmt0_wide_encode_data(u, arg, de, se);
            } else {
                bcm56990_l1_fmt0_wide_comp_encode(u, arg, de, se);
            }
        } else {
            if (l1v == L1V_WIDE) {
                if (arg->encode_data_only) {
                    bcm56990_l1_fmt0_wide_encode_data(u, arg, de, se);
                } else {
                    bcm56990_l1_fmt0_wide_lpm_encode(u, arg, de, se);
                }
            } else if (l1v == L1V_NARROW_1) {
                if (arg->encode_data_only) {
                    bcm56990_l1_fmt0_narrow_encode_data_n1(u, arg, de, se, arg->pkm);
                } else {
                    bcm56990_l1_fmt0_narrow_lpm_encode_n1(u, arg, de, se, arg->pkm);
                }
            } else {
                if (arg->encode_data_only) {
                    bcm56990_l1_fmt0_narrow_encode_data(u, arg, de, se, arg->pkm);
                } else {
                    bcm56990_l1_fmt0_narrow_lpm_encode(u, arg, de, se, arg->pkm);
                }
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
bcm56990_l1_fmt0_narrow_decode_data_n1(int u, alpm_arg_t *arg, UNPAIR0_T *de)
{
    bcm56990_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;

    UNPAIR0_FIELD_GET2(ALPM1_DATA1f, a1_format);
    UNPAIR0_FIELD_GET2(ASSOC_DATA1f, ad_format);

    bcm56990_ln_alpm1_data_format_decode(u, &a1_format, &arg->a1, arg);
    bcm56990_ln_assoc_data_format_decode(u, &ad_format, 1, &arg->ad, arg->key.t.app);
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
bcm56990_l1_fmt0_narrow_decode_data(int u, alpm_arg_t *arg, UNPAIR0_T *de)
{
    bcm56990_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;

    UNPAIR0_FIELD_GET2(ALPM1_DATA0f, a1_format);
    UNPAIR0_FIELD_GET2(ASSOC_DATA0f, ad_format);

    /*
     * Mainly for LT decode.
     * If for entry get, it will be overwritten by following hit get.
     */
    UNPAIR0_FIELD_GET(PARITY0f, arg->hit);

    bcm56990_ln_alpm1_data_format_decode(u, &a1_format, &arg->a1, arg);
    bcm56990_ln_assoc_data_format_decode(u, &ad_format, 1, &arg->ad, arg->key.t.app);
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
bcm56990_l1_fmt0_wide_decode_data(int u, alpm_arg_t *arg, PAIR0_T *de)
{
    bcm56990_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;

    /* Data */
    PAIR0_FIELD_GET2(LWR_ALPM1_DATA0f, a1_format);
    PAIR0_FIELD_GET2(LWR_ASSOC_DATA0f, ad_format);

    PAIR0_FIELD_GET(LWR_PARITY0f, arg->hit);

    bcm56990_ln_alpm1_data_format_decode(u, &a1_format, &arg->a1, arg);
    bcm56990_ln_assoc_data_format_decode(u, &ad_format, 1, &arg->ad, arg->key.t.app);
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
bcm56990_l1_fmt1_narrow_decode_data_n1(int u, alpm_arg_t *arg, UNPAIR1_T *de)
{
    bcm56990_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;

    UNPAIR1_FIELD_GET2(ALPM1_DATA1f, a1_format);
    UNPAIR1_FIELD_GET2(ASSOC_DATA1f, ad_format);

    bcm56990_ln_alpm1_data_format_decode(u, &a1_format, &arg->a1, arg);
    bcm56990_ln_assoc_data_format_decode(u, &ad_format, 1, &arg->ad, arg->key.t.app);
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
bcm56990_l1_fmt1_narrow_decode_data(int u, alpm_arg_t *arg, UNPAIR1_T *de)
{
    bcm56990_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;

    UNPAIR1_FIELD_GET2(ALPM1_DATA0f, a1_format);
    UNPAIR1_FIELD_GET2(ASSOC_DATA0f, ad_format);

    /*
     * Mainly for LT decode.
     * If for entry get, it will be overwritten by following hit get.
     */
    UNPAIR1_FIELD_GET(PARITY0f, arg->hit);

    bcm56990_ln_alpm1_data_format_decode(u, &a1_format, &arg->a1, arg);
    bcm56990_ln_assoc_data_format_decode(u, &ad_format, 1, &arg->ad, arg->key.t.app);
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
bcm56990_l1_fmt1_wide_decode_data(int u, alpm_arg_t *arg, PAIR1_T *de)
{
    bcm56990_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;

    /* Data */
    PAIR1_FIELD_GET2(LWR_ALPM1_DATA0f, a1_format);
    PAIR1_FIELD_GET2(LWR_ASSOC_DATA0f, ad_format);

    PAIR1_FIELD_GET(LWR_PARITY0f, arg->hit);

    bcm56990_ln_alpm1_data_format_decode(u, &a1_format, &arg->a1, arg);
    bcm56990_ln_assoc_data_format_decode(u, &ad_format, 1, &arg->ad, arg->key.t.app);
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
bcm56990_l1_fmt0_narrow_lpm_decode_n1(int u, alpm_arg_t *arg, UNPAIR0_T *de)
{
    L3_DEFIP_TCAM_KEY_FMT0_t kdata;
    L3_DEFIP_TCAM_KEY_FMT0_t kmask;
    int key_mode;
    int vrf, vrf_mask;
    int sub_prio;
    uint32_t ip_addr, ip_mask;
    bcm56990_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    int kt;
    uint8_t large_vrf;

    L3_DEFIP_TCAM_KEY_FMT0_CLR(kdata);
    L3_DEFIP_TCAM_KEY_FMT0_CLR(kmask);

    UNPAIR0_FIELD_GET2(KEY1f, kdata);
    UNPAIR0_FIELD_GET2(MASK1f, kmask);
    UNPAIR0_FIELD_GET(FIXED_DATA1f, sub_prio);
    large_vrf = arg->lpm_large_vrf && sub_prio != SUB_DB_PRIORITY_OVERRIDE;
    if (large_vrf) {
        bcm56990_l1_key_fmt0_lpm_v4_decode_no_vrf(u, NULL, NULL,
                                                  &arg->key.ip_mask[0], &kmask);
        bcm56990_l1_key_fmt0_lpm_v4_decode_no_vrf(u, &key_mode, &kt,
                                                  &arg->key.ip_addr[0], &kdata);
    } else {
        bcm56990_l1_key_fmt0_lpm_v4_decode(u, &vrf_mask, NULL, NULL, &ip_mask,
                                           &kmask);
        bcm56990_l1_key_fmt0_lpm_v4_decode(u, &vrf, &key_mode, &kt,
                                           &ip_addr, &kdata);
    }
    arg->key.t.ipv = kt;
    if (!arg->key.t.ipv) {
        UNPAIR0_FIELD_GET(VALID1f, arg->valid);
        if (!large_vrf) {
            arg->key.ip_addr[0] = ip_addr;
            arg->key.ip_mask[0] = ip_mask;
        }
    } else if (key_mode == HALF_WIDE) {
        UNPAIR0_FIELD_GET(VALID1f, arg->valid);
        if (!large_vrf) {
            arg->key.ip_addr[3] = ip_addr;
            arg->key.ip_mask[3] = ip_mask;
        }
    }

    bcm56990_l1_lpm_vt_decode(u, sub_prio, vrf, vrf_mask, arg);

    UNPAIR0_FIELD_GET2(ALPM1_DATA1f, a1_format);
    UNPAIR0_FIELD_GET2(ASSOC_DATA1f, ad_format);

    bcm56990_ln_alpm1_data_format_decode(u, &a1_format, &arg->a1, arg);
    bcm56990_ln_assoc_data_format_decode(u, &ad_format, 1, &arg->ad, arg->key.t.app);
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
bcm56990_l1_fmt0_narrow_lpm_decode(int u, alpm_arg_t *arg, UNPAIR0_T *de)
{
    L3_DEFIP_TCAM_KEY_FMT0_t kdata[2] = {{{0}}};
    L3_DEFIP_TCAM_KEY_FMT0_t kmask[2] = {{{0}}};
    int valid[2];
    int key_mode;
    int vrf, vrf_mask;
    int sub_prio;
    uint32_t ip_addr, ip_mask;
    bcm56990_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    int kt;
    uint8_t large_vrf;

    UNPAIR0_FIELD_GET2(KEY0f, kdata[0]);
    UNPAIR0_FIELD_GET2(MASK0f, kmask[0]);
    UNPAIR0_FIELD_GET(FIXED_DATA0f, sub_prio);

    large_vrf = arg->lpm_large_vrf && sub_prio != SUB_DB_PRIORITY_OVERRIDE;
    if (large_vrf) {
        bcm56990_l1_key_fmt0_lpm_v4_decode_no_vrf(u, NULL, NULL,
                                                  &arg->key.ip_mask[0], &kmask[0]);
        bcm56990_l1_key_fmt0_lpm_v4_decode_no_vrf(u, &key_mode, &kt,
                                                  &arg->key.ip_addr[0], &kdata[0]);
    } else {
        bcm56990_l1_key_fmt0_lpm_v4_decode(u, &vrf_mask, NULL, NULL, &ip_mask,
                                           &kmask[0]);
        bcm56990_l1_key_fmt0_lpm_v4_decode(u, &vrf, &key_mode, &kt,
                                           &ip_addr, &kdata[0]);
    }
    arg->key.t.ipv = kt;
    if (!arg->key.t.ipv) {
        UNPAIR0_FIELD_GET(VALID0f, arg->valid);
        if (!large_vrf) {
            arg->key.ip_addr[0] = ip_addr;
            arg->key.ip_mask[0] = ip_mask;
        }
    } else if (key_mode == HALF_WIDE) {
        UNPAIR0_FIELD_GET(VALID0f, arg->valid);
        if (!large_vrf) {
            arg->key.ip_addr[3] = ip_addr;
            arg->key.ip_mask[3] = ip_mask;
        }
    } else {
        UNPAIR0_FIELD_GET(VALID0f, valid[0]);
        UNPAIR0_FIELD_GET(VALID1f, valid[1]);
        arg->valid = valid[0] && valid[1];

        UNPAIR0_FIELD_GET2(KEY1f, kdata[1]);
        UNPAIR0_FIELD_GET2(MASK1f, kmask[1]);

        if (large_vrf) {
            bcm56990_l1_key_fmt0_lpm_v6_single_decode_no_vrf(u, NULL, NULL,
                                                             &arg->key.ip_mask[0],
                                                             &kmask[0],
                                                             &kmask[1]);
            bcm56990_l1_key_fmt0_lpm_v6_single_decode_no_vrf(u, NULL, NULL,
                                                             &arg->key.ip_addr[0],
                                                             &kdata[0],
                                                             &kdata[1]);
        } else {
            bcm56990_l1_key_fmt0_lpm_v6_single_decode(u, &vrf_mask, NULL, NULL,
                                                      &arg->key.ip_mask[0],
                                                      &kmask[0],
                                                      &kmask[1]);
            bcm56990_l1_key_fmt0_lpm_v6_single_decode(u, &vrf, NULL, NULL,
                                                      &arg->key.ip_addr[0],
                                                      &kdata[0],
                                                      &kdata[1]);

        }
    }

    bcm56990_l1_lpm_vt_decode(u, sub_prio, vrf, vrf_mask, arg);


    UNPAIR0_FIELD_GET2(ALPM1_DATA0f, a1_format);
    UNPAIR0_FIELD_GET2(ASSOC_DATA0f, ad_format);

    /*
     * Mainly for LT decode.
     * If for entry get, it will be overwritten by following hit get.
     */
    UNPAIR0_FIELD_GET(PARITY0f, arg->hit);

    bcm56990_ln_alpm1_data_format_decode(u, &a1_format, &arg->a1, arg);
    bcm56990_ln_assoc_data_format_decode(u, &ad_format, 1, &arg->ad, arg->key.t.app);
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
bcm56990_l1_fmt0_wide_lpm_decode(int u, alpm_arg_t *arg, PAIR0_T *de)
{
    L3_DEFIP_TCAM_KEY_FMT0_t kdata[4] = {{{0}}};
    L3_DEFIP_TCAM_KEY_FMT0_t kmask[4] = {{{0}}};
    int valid[4] = {0};
    int vrf, vrf_mask;
    int sub_prio;
    bcm56990_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    int kt;
    uint8_t large_vrf;


    PAIR0_FIELD_GET(LWR_FIXED_DATA0f, sub_prio);

    PAIR0_FIELD_GET2(LWR_KEY0f, kdata[0]);
    PAIR0_FIELD_GET2(LWR_KEY1f, kdata[1]);
    PAIR0_FIELD_GET2(UPR_KEY0f, kdata[2]);
    PAIR0_FIELD_GET2(UPR_KEY1f, kdata[3]);
    PAIR0_FIELD_GET2(LWR_MASK0f, kmask[0]);
    PAIR0_FIELD_GET2(LWR_MASK1f, kmask[1]);
    PAIR0_FIELD_GET2(UPR_MASK0f, kmask[2]);
    PAIR0_FIELD_GET2(UPR_MASK1f, kmask[3]);
    large_vrf = arg->lpm_large_vrf && sub_prio != SUB_DB_PRIORITY_OVERRIDE;
    if (large_vrf) {
        bcm56990_l1_key_fmt0_lpm_v6_double_decode_no_vrf(u, NULL, &kt,
                                                         &arg->key.ip_addr[0],
                                                         &kdata[0],
                                                         &kdata[1],
                                                         &kdata[2],
                                                         &kdata[3]
                                                         );
        bcm56990_l1_key_fmt0_lpm_v6_double_decode_no_vrf(u, NULL, NULL,
                                                         &arg->key.ip_mask[0],
                                                         &kmask[0],
                                                         &kmask[1],
                                                         &kmask[2],
                                                         &kmask[3]
                                                         );

    } else {
        bcm56990_l1_key_fmt0_lpm_v6_double_decode(u, &vrf, NULL, &kt,
                                                  &arg->key.ip_addr[0],
                                                  &kdata[0],
                                                  &kdata[1],
                                                  &kdata[2],
                                                  &kdata[3]
                                                  );
        bcm56990_l1_key_fmt0_lpm_v6_double_decode(u, &vrf_mask, NULL, NULL,
                                                  &arg->key.ip_mask[0],
                                                  &kmask[0],
                                                  &kmask[1],
                                                  &kmask[2],
                                                  &kmask[3]
                                                  );
    }
    arg->key.t.ipv = kt;
    bcm56990_l1_lpm_vt_decode(u, sub_prio, vrf, vrf_mask, arg);

    PAIR0_FIELD_GET(LWR_VALID0f, valid[0]);
    PAIR0_FIELD_GET(LWR_VALID1f, valid[1]);
    PAIR0_FIELD_GET(UPR_VALID0f, valid[2]);
    PAIR0_FIELD_GET(UPR_VALID1f, valid[3]);
    arg->valid = valid[0] & valid[1] & valid[2] & valid[3];

    /* Data */
    PAIR0_FIELD_GET2(LWR_ALPM1_DATA0f, a1_format);
    PAIR0_FIELD_GET2(LWR_ASSOC_DATA0f, ad_format);

    PAIR0_FIELD_GET(LWR_PARITY0f, arg->hit);

    bcm56990_ln_alpm1_data_format_decode(u, &a1_format, &arg->a1, arg);
    bcm56990_ln_assoc_data_format_decode(u, &ad_format, 1, &arg->ad, arg->key.t.app);
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
bcm56990_l1_fmt1_narrow_l3mc_decode(int u, alpm_arg_t *arg, UNPAIR1_T *de)
{
    L3_DEFIP_TCAM_KEY_FMT1_t kdata[2] = {{{0}}};
    L3_DEFIP_TCAM_KEY_FMT1_t kmask[2] = {{{0}}};
    int valid[2];
    bcm56990_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;

    UNPAIR1_FIELD_GET2(KEY0f, kdata[0]);
    UNPAIR1_FIELD_GET2(MASK0f, kmask[0]);
    UNPAIR1_FIELD_GET2(KEY1f, kdata[1]);
    UNPAIR1_FIELD_GET2(MASK1f, kmask[1]);
    UNPAIR1_FIELD_GET(VALID0f, valid[0]);
    UNPAIR1_FIELD_GET(VALID1f, valid[1]);
    arg->valid = valid[0] && valid[1];

    bcm56990_l1_key_fmt1_l3mc_v4_single_decode(u, NULL, NULL,
                                              &arg->key.ip_mask[0],
                                              &kmask[0],
                                              &kmask[1]);
    bcm56990_l1_key_fmt1_l3mc_v4_single_decode(u, NULL, NULL,
                                              &arg->key.ip_addr[0],
                                              &kdata[0],
                                              &kdata[1]);

    arg->key.t.ipv = IP_VER_4;
    arg->key.vt = VRF_DEFAULT;

    UNPAIR1_FIELD_GET2(ALPM1_DATA0f, a1_format);
    UNPAIR1_FIELD_GET2(ASSOC_DATA0f, ad_format);
    UNPAIR1_FIELD_GET(PARITY0f, arg->hit);

    bcm56990_ln_alpm1_data_format_decode(u, &a1_format, &arg->a1, arg);
    bcm56990_ln_assoc_data_format_decode(u, &ad_format, 1, &arg->ad, arg->key.t.app);
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
bcm56990_l1_fmt1_wide_l3mc_decode(int u, alpm_arg_t *arg, PAIR1_T *de)
{
    L3_DEFIP_TCAM_KEY_FMT1_t kdata[4] = {{{0}}};
    L3_DEFIP_TCAM_KEY_FMT1_t kmask[4] = {{{0}}};
    int valid[4] = {0};
    bcm56990_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
/*    int hit[2] = {0}; */

    PAIR1_FIELD_GET2(LWR_KEY0f, kdata[0]);
    PAIR1_FIELD_GET2(LWR_KEY1f, kdata[1]);
    PAIR1_FIELD_GET2(UPR_KEY0f, kdata[2]);
    PAIR1_FIELD_GET2(UPR_KEY1f, kdata[3]);
    PAIR1_FIELD_GET2(LWR_MASK0f, kmask[0]);
    PAIR1_FIELD_GET2(LWR_MASK1f, kmask[1]);
    PAIR1_FIELD_GET2(UPR_MASK0f, kmask[2]);
    PAIR1_FIELD_GET2(UPR_MASK1f, kmask[3]);
    bcm56990_l1_key_fmt1_l3mc_v6_double_decode(u, NULL, NULL,
                                              &arg->key.ip_mask[0],
                                              &kmask[0],
                                              &kmask[1],
                                              &kmask[2],
                                              &kmask[3]
                                              );
    bcm56990_l1_key_fmt1_l3mc_v6_double_decode(u, NULL, NULL,
                                              &arg->key.ip_addr[0],
                                              &kdata[0],
                                              &kdata[1],
                                              &kdata[2],
                                              &kdata[3]
                                              );
    arg->key.t.ipv = IP_VER_6;
    arg->key.vt = VRF_DEFAULT;

    PAIR1_FIELD_GET(LWR_VALID0f, valid[0]);
    PAIR1_FIELD_GET(LWR_VALID1f, valid[1]);
    PAIR1_FIELD_GET(UPR_VALID0f, valid[2]);
    PAIR1_FIELD_GET(UPR_VALID1f, valid[3]);
    arg->valid = valid[0] & valid[1] & valid[2] & valid[3];

    /* Data */
    PAIR1_FIELD_GET2(LWR_ALPM1_DATA0f, a1_format);
    PAIR1_FIELD_GET2(LWR_ASSOC_DATA0f, ad_format);

    PAIR1_FIELD_GET(LWR_PARITY0f, arg->hit);

    bcm56990_ln_alpm1_data_format_decode(u, &a1_format, &arg->a1, arg);
    bcm56990_ln_assoc_data_format_decode(u, &ad_format, 1, &arg->ad, arg->key.t.app);
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
bcm56990_l1_fmt1_narrow_lpm_decode_n1(int u, alpm_arg_t *arg, UNPAIR1_T *de)
{
    L3_DEFIP_TCAM_KEY_FMT1_t kdata;
    L3_DEFIP_TCAM_KEY_FMT1_t kmask;
    int key_mode;
    int vrf, vrf_mask;
    int sub_prio;
    uint32_t ip_addr, ip_mask;
    bcm56990_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    int kt;
    uint8_t large_vrf;

    L3_DEFIP_TCAM_KEY_FMT1_CLR(kdata);
    L3_DEFIP_TCAM_KEY_FMT1_CLR(kmask);

    UNPAIR1_FIELD_GET2(KEY1f, kdata);
    UNPAIR1_FIELD_GET2(MASK1f, kmask);
    UNPAIR1_FIELD_GET(FIXED_DATA1f, sub_prio);
    large_vrf = arg->lpm_large_vrf && sub_prio != SUB_DB_PRIORITY_OVERRIDE;
    if (large_vrf) {
        bcm56990_l1_key_fmt1_lpm_v4_decode_no_vrf(u, NULL, NULL,
                                                  &arg->key.ip_mask[0], &kmask);
        bcm56990_l1_key_fmt1_lpm_v4_decode_no_vrf(u, &key_mode, &kt,
                                                  &arg->key.ip_addr[0], &kdata);

    } else {
        bcm56990_l1_key_fmt1_lpm_v4_decode(u, &vrf_mask, NULL, NULL, &ip_mask,
                                           &kmask);
        bcm56990_l1_key_fmt1_lpm_v4_decode(u, &vrf, &key_mode, &kt,
                                           &ip_addr, &kdata);
    }

    arg->key.t.ipv = kt;
    if (!arg->key.t.ipv) {
        UNPAIR1_FIELD_GET(VALID1f, arg->valid);
        if (!large_vrf) {
            arg->key.ip_addr[0] = ip_addr;
            arg->key.ip_mask[0] = ip_mask;
        }
    } else if (key_mode == HALF_WIDE) {
        UNPAIR1_FIELD_GET(VALID1f, arg->valid);
        if (!large_vrf) {
            arg->key.ip_addr[3] = ip_addr;
            arg->key.ip_mask[3] = ip_mask;
        }
    }

    bcm56990_l1_lpm_vt_decode(u, sub_prio, vrf, vrf_mask, arg);

    UNPAIR1_FIELD_GET2(ALPM1_DATA1f, a1_format);
    UNPAIR1_FIELD_GET2(ASSOC_DATA1f, ad_format);

    bcm56990_ln_alpm1_data_format_decode(u, &a1_format, &arg->a1, arg);
    bcm56990_ln_assoc_data_format_decode(u, &ad_format, 1, &arg->ad, arg->key.t.app);
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
bcm56990_l1_fmt1_narrow_lpm_decode(int u, alpm_arg_t *arg, UNPAIR1_T *de)
{
    L3_DEFIP_TCAM_KEY_FMT1_t kdata[2] = {{{0}}};
    L3_DEFIP_TCAM_KEY_FMT1_t kmask[2] = {{{0}}};
    int valid[2];
    int key_mode;
    int vrf, vrf_mask;
    int sub_prio;
    uint32_t ip_addr, ip_mask;
    bcm56990_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    int kt;
    uint8_t large_vrf;

    UNPAIR1_FIELD_GET2(KEY0f, kdata[0]);
    UNPAIR1_FIELD_GET(FIXED_DATA0f, sub_prio);
    large_vrf = arg->lpm_large_vrf && sub_prio != SUB_DB_PRIORITY_OVERRIDE;
    if (large_vrf) {
        bcm56990_l1_key_fmt1_lpm_v4_decode_no_vrf(u, &key_mode, &kt,
                                                  &arg->key.ip_addr[0], &kdata[0]);
    } else {
        bcm56990_l1_key_fmt1_lpm_v4_decode(u, &vrf, &key_mode, &kt,
                                           &ip_addr, &kdata[0]);
    }
    UNPAIR1_FIELD_GET2(MASK0f, kmask[0]);
    if (large_vrf) {
        bcm56990_l1_key_fmt1_lpm_v4_decode_no_vrf(u, NULL, NULL,
                                                  &arg->key.ip_mask[0], &kmask[0]);
    } else {
        bcm56990_l1_key_fmt1_lpm_v4_decode(u, &vrf_mask, NULL, NULL, &ip_mask,
                                           &kmask[0]);
    }
    if (kt == FMT1_KT_L3MC_V4 ||
        kt == FMT1_KT_L3MC_V6) {
        ALPM_ASSERT(kt == FMT1_KT_L3MC_V4);
        arg->key.t.app = APP_L3MC;
        bcm56990_l1_fmt1_narrow_l3mc_decode(u, arg, de);
        return;
    }

    arg->key.t.ipv = kt;

    if (!arg->key.t.ipv) {
        UNPAIR1_FIELD_GET(VALID0f, arg->valid);
        if (!large_vrf) {
            arg->key.ip_addr[0] = ip_addr;
            arg->key.ip_mask[0] = ip_mask;
        }
    } else if (key_mode == HALF_WIDE) {
        UNPAIR1_FIELD_GET(VALID0f, arg->valid);
        if (!large_vrf) {
            arg->key.ip_addr[3] = ip_addr;
            arg->key.ip_mask[3] = ip_mask;
        }
    } else {
        UNPAIR1_FIELD_GET(VALID0f, valid[0]);
        UNPAIR1_FIELD_GET(VALID1f, valid[1]);
        arg->valid = valid[0] && valid[1];

        UNPAIR1_FIELD_GET2(KEY1f, kdata[1]);
        UNPAIR1_FIELD_GET2(MASK1f, kmask[1]);
        if (large_vrf) {
            bcm56990_l1_key_fmt1_lpm_v6_single_decode_no_vrf(u, NULL, NULL,
                                                             &arg->key.ip_mask[0],
                                                             &kmask[0],
                                                             &kmask[1]);
            bcm56990_l1_key_fmt1_lpm_v6_single_decode_no_vrf(u, NULL, NULL,
                                                             &arg->key.ip_addr[0],
                                                             &kdata[0],
                                                             &kdata[1]);
        } else {
            bcm56990_l1_key_fmt1_lpm_v6_single_decode(u, &vrf_mask, NULL, NULL,
                                                      &arg->key.ip_mask[0],
                                                      &kmask[0],
                                                      &kmask[1]);
            bcm56990_l1_key_fmt1_lpm_v6_single_decode(u, &vrf, NULL, NULL,
                                                      &arg->key.ip_addr[0],
                                                      &kdata[0],
                                                      &kdata[1]);
        }
    }

    bcm56990_l1_lpm_vt_decode(u, sub_prio, vrf, vrf_mask, arg);

    UNPAIR1_FIELD_GET2(ALPM1_DATA0f, a1_format);
    UNPAIR1_FIELD_GET2(ASSOC_DATA0f, ad_format);

    bcm56990_ln_alpm1_data_format_decode(u, &a1_format, &arg->a1, arg);
    bcm56990_ln_assoc_data_format_decode(u, &ad_format, 1, &arg->ad, arg->key.t.app);
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
bcm56990_l1_fmt1_wide_lpm_decode(int u, alpm_arg_t *arg, PAIR1_T *de)
{
    L3_DEFIP_TCAM_KEY_FMT1_t kdata[4] = {{{0}}};
    L3_DEFIP_TCAM_KEY_FMT1_t kmask[4] = {{{0}}};
    int valid[4] = {0};
    int vrf, vrf_mask;
    int sub_prio;
    bcm56990_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    int kt;
/*    int hit[2] = {0}; */
    uint8_t large_vrf;


    PAIR1_FIELD_GET(LWR_FIXED_DATA0f, sub_prio);

    PAIR1_FIELD_GET2(LWR_KEY0f, kdata[0]);
    PAIR1_FIELD_GET2(LWR_KEY1f, kdata[1]);
    PAIR1_FIELD_GET2(UPR_KEY0f, kdata[2]);
    PAIR1_FIELD_GET2(UPR_KEY1f, kdata[3]);

    large_vrf = arg->lpm_large_vrf && sub_prio != SUB_DB_PRIORITY_OVERRIDE;
    if (large_vrf) {
        bcm56990_l1_key_fmt1_lpm_v6_double_decode_no_vrf(u, NULL, &kt,
                                                         &arg->key.ip_addr[0],
                                                         &kdata[0],
                                                         &kdata[1],
                                                         &kdata[2],
                                                         &kdata[3]
                                                         );
    } else {
        bcm56990_l1_key_fmt1_lpm_v6_double_decode(u, &vrf, NULL, &kt,
                                                  &arg->key.ip_addr[0],
                                                  &kdata[0],
                                                  &kdata[1],
                                                  &kdata[2],
                                                  &kdata[3]
                                                  );
    }

    if (kt == FMT1_KT_L3MC_V4 ||
        kt == FMT1_KT_L3MC_V6) {
        ALPM_ASSERT(kt == FMT1_KT_L3MC_V6);
        arg->key.t.app = APP_L3MC;
        bcm56990_l1_fmt1_wide_l3mc_decode(u, arg, de);
        return;
    }
    arg->key.t.ipv = kt;
    PAIR1_FIELD_GET2(LWR_MASK0f, kmask[0]);
    PAIR1_FIELD_GET2(LWR_MASK1f, kmask[1]);
    PAIR1_FIELD_GET2(UPR_MASK0f, kmask[2]);
    PAIR1_FIELD_GET2(UPR_MASK1f, kmask[3]);
    if (large_vrf) {
        bcm56990_l1_key_fmt1_lpm_v6_double_decode_no_vrf(u, NULL, NULL,
                                                         &arg->key.ip_mask[0],
                                                         &kmask[0],
                                                         &kmask[1],
                                                         &kmask[2],
                                                         &kmask[3]
                                                         );

    } else {
        bcm56990_l1_key_fmt1_lpm_v6_double_decode(u, &vrf_mask, NULL, NULL,
                                                  &arg->key.ip_mask[0],
                                                  &kmask[0],
                                                  &kmask[1],
                                                  &kmask[2],
                                                  &kmask[3]
                                                  );
    }

    bcm56990_l1_lpm_vt_decode(u, sub_prio, vrf, vrf_mask, arg);

    PAIR1_FIELD_GET(LWR_VALID0f, valid[0]);
    PAIR1_FIELD_GET(LWR_VALID1f, valid[1]);
    PAIR1_FIELD_GET(UPR_VALID0f, valid[2]);
    PAIR1_FIELD_GET(UPR_VALID1f, valid[3]);
    arg->valid = valid[0] & valid[1] & valid[2] & valid[3];

    /* Data */
    PAIR1_FIELD_GET2(LWR_ALPM1_DATA0f, a1_format);
    PAIR1_FIELD_GET2(LWR_ASSOC_DATA0f, ad_format);

    bcm56990_ln_alpm1_data_format_decode(u, &a1_format, &arg->a1, arg);
    bcm56990_ln_assoc_data_format_decode(u, &ad_format, 1, &arg->ad, arg->key.t.app);
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
bcm56990_l1_fmt0_wide_comp_decode(int u, alpm_arg_t *arg, PAIR0_T *de)
{
    L3_DEFIP_TCAM_KEY_FMT0_t kdata[4] = {{{0}}};
    L3_DEFIP_TCAM_KEY_FMT0_t kmask[4] = {{{0}}};
    int valid[4] = {0};
    bcm56990_assoc_data_t ad_format;
    ALPM1_DATA_t a1_format;
    int vrf, vrf_mask;

    PAIR0_FIELD_GET2(LWR_KEY0f, kdata[0]);
    PAIR0_FIELD_GET2(LWR_KEY1f, kdata[1]);
    PAIR0_FIELD_GET2(UPR_KEY0f, kdata[2]);
    PAIR0_FIELD_GET2(UPR_KEY1f, kdata[3]);
    PAIR0_FIELD_GET2(LWR_MASK0f, kmask[0]);
    PAIR0_FIELD_GET2(LWR_MASK1f, kmask[1]);
    PAIR0_FIELD_GET2(UPR_MASK0f, kmask[2]);
    PAIR0_FIELD_GET2(UPR_MASK1f, kmask[3]);

    FMT0_TCAM_KEY_FIELD_GET(&(kdata[0]), KEY_TYPEf, arg->key.t.ipv);

    if (arg->key.t.ipv == IP_VER_4) {
        bcm56990_l1_key_fmt0_comp_v4_double_decode(u, arg->comp_key_type,
                                                  &vrf_mask, NULL, NULL,
                                                  &arg->key.ip_mask[0],
                                                  &kmask[0],
                                                  &kmask[1],
                                                  &kmask[2],
                                                  &kmask[3]
                                                  );
        bcm56990_l1_key_fmt0_comp_v4_double_decode(u, arg->comp_key_type,
                                                  &vrf, NULL, NULL,
                                                  &arg->key.ip_addr[0],
                                                  &kdata[0],
                                                  &kdata[1],
                                                  &kdata[2],
                                                  &kdata[3]
                                                  );
    } else {
        bcm56990_l1_key_fmt0_comp_v6_double_decode(u, arg->comp_key_type,
                                                  &vrf_mask, NULL, NULL,
                                                  &arg->key.ip_mask[0],
                                                  &kmask[0],
                                                  &kmask[1],
                                                  &kmask[2],
                                                  &kmask[3]
                                                  );
        bcm56990_l1_key_fmt0_comp_v6_double_decode(u, arg->comp_key_type,
                                                  &vrf, NULL, NULL,
                                                  &arg->key.ip_addr[0],
                                                  &kdata[0],
                                                  &kdata[1],
                                                  &kdata[2],
                                                  &kdata[3]
                                                  );
    }

    PAIR0_FIELD_GET(LWR_VALID0f, valid[0]);
    PAIR0_FIELD_GET(LWR_VALID1f, valid[1]);
    PAIR0_FIELD_GET(UPR_VALID0f, valid[2]);
    PAIR0_FIELD_GET(UPR_VALID1f, valid[3]);
    arg->valid = valid[0] & valid[1] & valid[2] & valid[3];
    bcm56990_l1_comp_vt_decode(u, arg->comp_key_type, vrf, vrf_mask, arg);

    /* Data */
    PAIR0_FIELD_GET2(LWR_ALPM1_DATA0f, a1_format);
    PAIR0_FIELD_GET2(LWR_ASSOC_DATA0f, ad_format);

    PAIR0_FIELD_GET(LWR_PARITY0f, arg->hit);

    bcm56990_ln_alpm1_data_format_decode(u, &a1_format, &arg->a1, arg);
    bcm56990_ln_assoc_data_format_decode(u, &ad_format, 1, &arg->ad, arg->key.t.app);
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
bcm56990_l1_decode(int u, int m, void *varg, void *de, void *se, uint8_t l1v)
{
    alpm_arg_t *arg = varg;

    if (!arg->decode_data_only) {
        arg->key.t.app = APP_LPM;
    }
    if (WIDE_PKM(arg->pkm)) {
        if (l1v == L1V_WIDE) {
            if (arg->decode_data_only) {
                bcm56990_l1_fmt1_wide_decode_data(u, arg, de);
            } else {
                bcm56990_l1_fmt1_wide_lpm_decode(u, arg, de);
            }
        } else if (l1v == L1V_NARROW_1) {
            if (arg->decode_data_only) {
                bcm56990_l1_fmt1_narrow_decode_data_n1(u, arg, de);
            } else {
                bcm56990_l1_fmt1_narrow_lpm_decode_n1(u, arg, de);
            }
        } else {
            if (arg->decode_data_only) {
                bcm56990_l1_fmt1_narrow_decode_data(u, arg, de);
            } else {
                bcm56990_l1_fmt1_narrow_lpm_decode(u, arg, de);
            }
        }
    } else {
        if (FP_PKM(arg->pkm)) {
            if (arg->decode_data_only) {
                bcm56990_l1_fmt0_wide_decode_data(u, arg, de);
            } else {
                arg->key.t.app = APP_COMP1;
                bcm56990_l1_fmt0_wide_comp_decode(u, arg, de);
            }
        } else {
            if (l1v == L1V_WIDE) {
                if (arg->decode_data_only) {
                    bcm56990_l1_fmt0_wide_decode_data(u, arg, de);
                } else {
                    bcm56990_l1_fmt0_wide_lpm_decode(u, arg, de);
                }
            } else if (l1v == L1V_NARROW_1) {
                if (arg->decode_data_only) {
                    bcm56990_l1_fmt0_narrow_decode_data_n1(u, arg, de);
                } else {
                    bcm56990_l1_fmt0_narrow_lpm_decode_n1(u, arg, de);
                }
            } else {
                if (arg->decode_data_only) {
                    bcm56990_l1_fmt0_narrow_decode_data(u, arg, de);
                } else {
                    bcm56990_l1_fmt0_narrow_lpm_decode(u, arg, de);
                }
            }
        }
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
bcm56990_l1_fmt0_half_entry_move(int u, void *src, void *dst, int x, int y)
{

    uint32_t v1, v2;
    ALPM1_DATA_t a1_data;
    ASSOC_DATA_FULL_t asoc_data;
    L3_DEFIP_TCAM_KEY_FMT0_t key, mask;
    UNPAIR0_T *se = src;
    UNPAIR0_T *de = dst;

    if (x) {
        UNPAIR0_SRC_FIELD_GET(VALID1f, v1);
        UNPAIR0_SRC_FIELD_GET2(KEY1f, key);
        UNPAIR0_SRC_FIELD_GET2(MASK1f, mask);
        UNPAIR0_SRC_FIELD_GET2(ASSOC_DATA1f, asoc_data);
        UNPAIR0_SRC_FIELD_GET2(ALPM1_DATA1f, a1_data);
        UNPAIR0_SRC_FIELD_GET(FIXED_DATA1f, v2);
/*        UNPAIR0_SRC_FIELD_GET(HIT1f, v3); */
    } else {
        UNPAIR0_SRC_FIELD_GET(VALID0f, v1);
        UNPAIR0_SRC_FIELD_GET2(KEY0f, key);
        UNPAIR0_SRC_FIELD_GET2(MASK0f, mask);
        UNPAIR0_SRC_FIELD_GET2(ASSOC_DATA0f, asoc_data);
        UNPAIR0_SRC_FIELD_GET2(ALPM1_DATA0f, a1_data);
        UNPAIR0_SRC_FIELD_GET(FIXED_DATA0f, v2);
/*        UNPAIR0_SRC_FIELD_GET(HIT0f, v3); */
    }

    if (y) {
        UNPAIR0_FIELD_SET(VALID1f, v1);
        UNPAIR0_FIELD_SET2(KEY1f, key);
        UNPAIR0_FIELD_SET2(MASK1f, mask);
        UNPAIR0_FIELD_SET2(ASSOC_DATA1f, asoc_data);
        UNPAIR0_FIELD_SET2(ALPM1_DATA1f, a1_data);
        UNPAIR0_FIELD_SET(FIXED_DATA1f, v2);
/*        UNPAIR0_FIELD_SET(HIT1f, v3); */
    } else {
        UNPAIR0_FIELD_SET(VALID0f, v1);
        UNPAIR0_FIELD_SET2(KEY0f, key);
        UNPAIR0_FIELD_SET2(MASK0f, mask);
        UNPAIR0_FIELD_SET2(ASSOC_DATA0f, asoc_data);
        UNPAIR0_FIELD_SET2(ALPM1_DATA0f, a1_data);
        UNPAIR0_FIELD_SET(FIXED_DATA0f, v2);
/*        UNPAIR0_FIELD_SET(HIT0f, v3); */
    }

    /* Clear source */
    if (x) {
        UNPAIR0_SRC_FIELD_SET(VALID1f, 0);
    } else {
        UNPAIR0_SRC_FIELD_SET(VALID0f, 0);
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
bcm56990_l1_fmt1_half_entry_move(int u, void *src, void *dst, int x, int y)
{

    uint32_t v1, v2;
    ALPM1_DATA_t a1_data;
    ASSOC_DATA_FULL_t asoc_data;
    L3_DEFIP_TCAM_KEY_FMT1_t key, mask;
    UNPAIR1_T *se = src;
    UNPAIR1_T *de = dst;

    if (x) {
        UNPAIR1_SRC_FIELD_GET(VALID1f, v1);
        UNPAIR1_SRC_FIELD_GET2(KEY1f, key);
        UNPAIR1_SRC_FIELD_GET2(MASK1f, mask);
        UNPAIR1_SRC_FIELD_GET2(ASSOC_DATA1f, asoc_data);
        UNPAIR1_SRC_FIELD_GET2(ALPM1_DATA1f, a1_data);
        UNPAIR1_SRC_FIELD_GET(FIXED_DATA1f, v2);
/*        UNPAIR1_SRC_FIELD_GET(HIT1f, v3); */
    } else {
        UNPAIR1_SRC_FIELD_GET(VALID0f, v1);
        UNPAIR1_SRC_FIELD_GET2(KEY0f, key);
        UNPAIR1_SRC_FIELD_GET2(MASK0f, mask);
        UNPAIR1_SRC_FIELD_GET2(ASSOC_DATA0f, asoc_data);
        UNPAIR1_SRC_FIELD_GET2(ALPM1_DATA0f, a1_data);
        UNPAIR1_SRC_FIELD_GET(FIXED_DATA0f, v2);
/*        UNPAIR1_SRC_FIELD_GET(HIT0f, v3); */
    }

    if (y) {
        UNPAIR1_FIELD_SET(VALID1f, v1);
        UNPAIR1_FIELD_SET2(KEY1f, key);
        UNPAIR1_FIELD_SET2(MASK1f, mask);
        UNPAIR1_FIELD_SET2(ASSOC_DATA1f, asoc_data);
        UNPAIR1_FIELD_SET2(ALPM1_DATA1f, a1_data);
        UNPAIR1_FIELD_SET(FIXED_DATA1f, v2);
/*        UNPAIR1_FIELD_SET(HIT1f, v3); */
    } else {
        UNPAIR1_FIELD_SET(VALID0f, v1);
        UNPAIR1_FIELD_SET2(KEY0f, key);
        UNPAIR1_FIELD_SET2(MASK0f, mask);
        UNPAIR1_FIELD_SET2(ASSOC_DATA0f, asoc_data);
        UNPAIR1_FIELD_SET2(ALPM1_DATA0f, a1_data);
        UNPAIR1_FIELD_SET(FIXED_DATA0f, v2);
/*        UNPAIR1_FIELD_SET(HIT0f, v3); */
    }

    /* Clear source */
    if (x) {
        UNPAIR1_SRC_FIELD_SET(VALID1f, 0);
    } else {
        UNPAIR1_SRC_FIELD_SET(VALID0f, 0);
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
bcm56990_l1_half_entry_move(int u, int m, void *src, void *dst, int x, int y, int pkm)
{

    if (WIDE_PKM(pkm)) {
        return bcm56990_l1_fmt1_half_entry_move(u, src, dst, x, y);
    } else {
        return bcm56990_l1_fmt0_half_entry_move(u, src, dst, x, y);
    }

    return SHR_E_NONE;
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
bcm56990_device_info_init(int u, int m, alpm_dev_info_t *dev)
{
    SHR_FUNC_ENTER(u);

    if (dev) {
        dev->max_vrf = BCM56990_ALPM_MAX_VRF;
        dev->max_fp_op = BCM56990_ALPM_MAX_COMP_PORT;
        dev->tcam_blocks = 16;
        dev->tcam_depth = 128;
        dev->l1_blocks = 8;
        dev->pair_sid = L3_DEFIP_PAIR_LEVEL1m;
        dev->unpair_sid = L3_DEFIP_LEVEL1m;
        dev->wide_pair_sid = L3_DEFIP_PAIR_LEVEL1_WIDEm;
        dev->wide_unpair_sid = L3_DEFIP_LEVEL1_WIDEm;
        dev->tcam_only_sid = L3_DEFIP_TCAM_LEVEL1m;
        dev->data_only_sid = L3_DEFIP_DATA_LEVEL1m;
        dev->wide_tcam_only_sid = L3_DEFIP_TCAM_LEVEL1_WIDEm;
        dev->wide_data_only_sid = L3_DEFIP_DATA_LEVEL1_WIDEm;
        dev->l1_hit_sid = L3_DEFIP_LEVEL1_HIT_ONLYm;
        dev->l2_sid = L3_DEFIP_ALPM_LEVEL2m;
        dev->l2_hit_sid = L3_DEFIP_ALPM_LEVEL2_HIT_ONLYm;
        dev->l3_sid = L3_DEFIP_ALPM_LEVEL3m;
        dev->l3_hit_sid = L3_DEFIP_ALPM_LEVEL3_HIT_ONLYm;
        dev->feature_pkms = 11; /* 11 key inputs */
        dev->feature_urpf = 0; /* no urpf support for TH4 */
        dev->pivot_max_format_0 = 6;
        dev->route_max_format_0 = 11;
        dev->pivot_max_format_1 = 10;
        dev->route_max_format_1 = 15;
        dev->pivot_min_format_0 = 1;
        dev->route_min_format_0 = 1;
        dev->pivot_min_format_1 = 7;
        dev->route_min_format_1 = 12;
        dev->l2p_route_fmts = &bcm56990_l2p_route_fmts[0];
        dev->w_size = sizeof(PAIR0_T);
        dev->n_size = sizeof(UNPAIR0_T);
        dev->ww_size = sizeof(PAIR1_T);
        dev->wn_size = sizeof(UNPAIR1_T);
        dev->alpm_ver = m ? ALPM_VERSION_NONE : ALPM_VERSION_1;
        dev->hit_ver = ALPM_HIT_VERSION_1;
        dev->max_base_entries = 20;
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
bcm56990_a0_rm_alpm_driver_register(int u, bcmptm_rm_alpm_driver_t *drv)
{
    SHR_FUNC_ENTER(u);

    if (drv) {
        drv->l1_encode = bcm56990_l1_encode;
        drv->l1_decode = bcm56990_l1_decode;
        drv->l1_half_entry_move = bcm56990_l1_half_entry_move;
        drv->ln_base_entry_decode = bcm56990_ln_base_entry_decode;
        drv->ln_base_entry_encode = bcm56990_ln_base_entry_encode;
        drv->ln_raw_bucket_decode = bcm56990_ln_raw_bucket_decode;
        drv->ln_raw_bucket_encode = bcm56990_ln_raw_bucket_encode;
        drv->ln_raw_bucket_entry_get = bcm56990_ln_raw_bucket_entry_get;
        drv->ln_raw_bucket_entry_set = bcm56990_ln_raw_bucket_entry_set;
        drv->ln_base_entry_info = bcm56990_ln_base_entry_info;
        drv->ln_base_entry_move = bcm56990_ln_base_entry_move;
        drv->device_info_init   = bcm56990_device_info_init;
        drv->ln_hit_get = NULL;
        drv->ln_hit_set = NULL;
    } else {

        SHR_ERR_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}


