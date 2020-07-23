/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File:    alpm.c
 * Purpose: ALPM + TCAM device dependent implementation
 * Requires:
 */

/* Implementation notes:
 */

#if defined(ALPM_ENABLE)

#include <bcm_int/esw/alpm_internal.h>

#if defined(BCM_TOMAHAWK3_SUPPORT)

#define TH3_FMT_DECLARE(fmt_name)          \
    uint32 fmt_name##_fld = 0;             \
    th3_fmt_##fmt_name##_t fmt_name##_fmt;

#define TH3_FMT_FIELD_CHECK(u, fmt_name, fld_name, fld_width)       \
    do {                                                            \
        uint32 fld_value;                                           \
        assert(fld_width <= 32);                                    \
        sal_memset(&fmt_name##_fmt, 0, sizeof(fmt_name##_fmt));     \
        fld_value = ((sal_rand() & 0x3) << 30) | (sal_rand() << 15) | sal_rand(); \
        fld_value = (fld_width == 32) ? fld_value : (fld_value % (1 << fld_width)); \
        fmt_name##_fmt.fld_name = fld_value; \
        soc_format_field_get(u, fmt_name##fmt, (uint32 *)&fmt_name##_fmt, fld_name##f, &fmt_name##_fld);    \
        if (bsl_check(bslLayerBcm, bslSourceAlpm, bslSeverityVerbose, u)) { \
            cli_out("FMT CHECK: %s.%s set 0x%x get 0x%x\n", SOC_FORMAT_NAME(u, fmt_name##fmt), SOC_FIELD_NAME(u, fld_name##f), fld_value, fmt_name##_fld); \
        }   \
        if (fld_value != fmt_name##_fld) {            \
            assert(0);                                              \
        }                                                           \
    } while (0)

#define TH3_FMT_FIELD_SPLIT_CHECK(u, fmt_name, fld_name, fld_width, split_fld_name1, split_fld_width1, split_fld_name2, split_fld_width2)       \
    do {                                                            \
        uint32 fld_value;                                           \
        assert(fld_width == (split_fld_width1 + split_fld_width2)); \
        assert(fld_width <= 32);                                    \
        sal_memset(&fmt_name##_fmt, 0, sizeof(fmt_name##_fmt));     \
        fld_value = ((sal_rand() & 0x3) << 30) | (sal_rand() << 15) | sal_rand(); \
        fld_value = (fld_width == 32) ? fld_value : (fld_value % (1 << fld_width)); \
        fmt_name##_fmt.split_fld_name1 = fld_value & ((1 << split_fld_width1) - 1); \
        fmt_name##_fmt.split_fld_name2 = (fld_value >> split_fld_width1) & ((1 << split_fld_width2) - 1); \
        soc_format_field_get(u, fmt_name##fmt, (uint32 *)&fmt_name##_fmt, fld_name##f, &fmt_name##_fld);    \
        if (bsl_check(bslLayerBcm, bslSourceAlpm, bslSeverityVerbose, u)) { \
            cli_out("FMT SPLIT CHECK: %s.%s set 0x%x get 0x%x\n", SOC_FORMAT_NAME(u, fmt_name##fmt), SOC_FIELD_NAME(u, fld_name##f), fld_value, fmt_name##_fld); \
        } \
        if (fld_value != fmt_name##_fld) {                          \
            assert(0);                                              \
        }                                                           \
    } while (0)

static void th3_fmt_test(int u)
{
    sal_srand(sal_time());

    {
        TH3_FMT_DECLARE(ASSOC_DATA_FULL);
        TH3_FMT_FIELD_CHECK(u, ASSOC_DATA_FULL, DEST_TYPE, 1);
        TH3_FMT_FIELD_CHECK(u, ASSOC_DATA_FULL, DESTINATION, 15);
        TH3_FMT_FIELD_CHECK(u, ASSOC_DATA_FULL, PRI, 4);
        TH3_FMT_FIELD_CHECK(u, ASSOC_DATA_FULL, RPE, 1);
        TH3_FMT_FIELD_CHECK(u, ASSOC_DATA_FULL, DST_DISCARD, 1);
        TH3_FMT_FIELD_CHECK(u, ASSOC_DATA_FULL, CLASS_ID, 6);
        TH3_FMT_FIELD_CHECK(u, ASSOC_DATA_FULL, FLEX_CTR_POOL_NUMBER, 4);
        TH3_FMT_FIELD_CHECK(u, ASSOC_DATA_FULL, FLEX_CTR_OFFSET_MODE, 2);
        TH3_FMT_FIELD_CHECK(u, ASSOC_DATA_FULL, FLEX_CTR_BASE_COUNTER_IDX, 12);
    }
    {
        TH3_FMT_DECLARE(ASSOC_DATA_REDUCED);
        TH3_FMT_FIELD_CHECK(u, ASSOC_DATA_REDUCED, DEST_TYPE, 1);
        TH3_FMT_FIELD_CHECK(u, ASSOC_DATA_REDUCED, DESTINATION, 15);
    }
    {
        TH3_FMT_DECLARE(FIXED_DATA);
        TH3_FMT_FIELD_CHECK(u, FIXED_DATA, SUB_DB_PRIORITY, 2);
    }
    {
        TH3_FMT_DECLARE(L3_DEFIP_TCAM_KEY);
        TH3_FMT_FIELD_CHECK(u, L3_DEFIP_TCAM_KEY, KEY_MODE, 2);
        TH3_FMT_FIELD_CHECK(u, L3_DEFIP_TCAM_KEY, KEY_TYPE, 1);
        TH3_FMT_FIELD_SPLIT_CHECK(u, L3_DEFIP_TCAM_KEY, IP_ADDR, 32, IP_ADDR_0_28, 29, IP_ADDR_29_31, 3);
        TH3_FMT_FIELD_CHECK(u, L3_DEFIP_TCAM_KEY, VRF_ID, 12);
    }
    {
        TH3_FMT_DECLARE(ALPM1_DATA);
        TH3_FMT_FIELD_CHECK(u, ALPM1_DATA, DEFAULT_MISS, 1);
        TH3_FMT_FIELD_CHECK(u, ALPM1_DATA, DIRECT_ROUTE, 1);
        TH3_FMT_FIELD_CHECK(u, ALPM1_DATA, BKT_PTR, 10);
        TH3_FMT_FIELD_CHECK(u, ALPM1_DATA, KSHIFT, 7);
        TH3_FMT_FIELD_CHECK(u, ALPM1_DATA, ROFS, 3);
        TH3_FMT_FIELD_CHECK(u, ALPM1_DATA, FMT0, 4);
        TH3_FMT_FIELD_CHECK(u, ALPM1_DATA, FMT1, 4);
        TH3_FMT_FIELD_SPLIT_CHECK(u, ALPM1_DATA, FMT2, 4, FMT2_0_1, 2, FMT2_2_3, 2);
        TH3_FMT_FIELD_CHECK(u, ALPM1_DATA, FMT3, 4);
        TH3_FMT_FIELD_CHECK(u, ALPM1_DATA, FMT4, 4);
    }
    {
        TH3_FMT_DECLARE(ALPM2_DATA);
        TH3_FMT_FIELD_CHECK(u, ALPM2_DATA, DEFAULT_MISS, 1);
        TH3_FMT_FIELD_CHECK(u, ALPM2_DATA, BKT_PTR, 13);
        TH3_FMT_FIELD_CHECK(u, ALPM2_DATA, KSHIFT, 7);
        TH3_FMT_FIELD_CHECK(u, ALPM2_DATA, ROFS, 3);
        TH3_FMT_FIELD_CHECK(u, ALPM2_DATA, FMT0, 4);
        TH3_FMT_FIELD_CHECK(u, ALPM2_DATA, FMT1, 4);
        TH3_FMT_FIELD_CHECK(u, ALPM2_DATA, FMT2, 4);
        TH3_FMT_FIELD_CHECK(u, ALPM2_DATA, FMT3, 4);
        TH3_FMT_FIELD_CHECK(u, ALPM2_DATA, FMT4, 4);
        TH3_FMT_FIELD_CHECK(u, ALPM2_DATA, FMT5, 4);
        TH3_FMT_FIELD_CHECK(u, ALPM2_DATA, FMT6, 4);
        TH3_FMT_FIELD_CHECK(u, ALPM2_DATA, FMT7, 4);
    }
}

#define TH3_MEM_DECLARE(mem_name)          \
    uint32 mem_name##_fld = 0;             \
    th3_mem_##mem_name##_t mem_name##_mem;

#define TH3_MEM_FIELD_CHECK(u, mem_name, fld_name, fld_width)       \
    do {                                                            \
        uint32 fld_value;                                           \
        assert(fld_width <= 32);                                    \
        sal_memset(&mem_name##_mem, 0, sizeof(mem_name##_mem));     \
        fld_value = ((sal_rand() & 0x3) << 30) | (sal_rand() << 15) | sal_rand(); \
        fld_value = (fld_width == 32) ? fld_value : (fld_value % (1 << fld_width)); \
        mem_name##_mem.fld_name = fld_value; \
        soc_mem_field_get(u, mem_name##m, (uint32 *)&mem_name##_mem, fld_name##f, &mem_name##_fld);    \
        if (bsl_check(bslLayerBcm, bslSourceAlpm, bslSeverityVerbose, u)) { \
            cli_out("MEM CHECK: %s.%s set 0x%x get 0x%x\n", SOC_MEM_NAME(u, mem_name##m), SOC_FIELD_NAME(u, fld_name##f), fld_value, mem_name##_fld); \
        }   \
        if (fld_value != mem_name##_fld) {            \
            assert(0);                                              \
        }                                                           \
    } while (0)

#define TH3_MEM_FIELD_SPLIT_CHECK(u, mem_name, fld_name, fld_width, split_fld_name1, split_fld_width1, split_fld_name2, split_fld_width2)       \
    do {                                                            \
        uint32 fld_value;                                           \
        assert(fld_width == (split_fld_width1 + split_fld_width2)); \
        assert(fld_width <= 32);                                    \
        sal_memset(&mem_name##_mem, 0, sizeof(mem_name##_mem));     \
        fld_value = ((sal_rand() & 0x3) << 30) | (sal_rand() << 15) | sal_rand(); \
        fld_value = (fld_width == 32) ? fld_value : (fld_value % (1 << fld_width)); \
        mem_name##_mem.split_fld_name1 = fld_value & ((1 << split_fld_width1) - 1); \
        mem_name##_mem.split_fld_name2 = (fld_value >> split_fld_width1) & ((1 << split_fld_width2) - 1); \
        soc_mem_field_get(u, mem_name##m, (uint32 *)&mem_name##_mem, fld_name##f, &mem_name##_fld);    \
        if (bsl_check(bslLayerBcm, bslSourceAlpm, bslSeverityVerbose, u)) { \
            cli_out("MEM SPLIT CHECK: %s.%s set 0x%x get 0x%x\n", SOC_MEM_NAME(u, mem_name##m), SOC_FIELD_NAME(u, fld_name##f), fld_value, mem_name##_fld); \
        } \
        if (fld_value != mem_name##_fld) {                          \
            assert(0);                                              \
        }                                                           \
    } while (0)
#if 0
static void th3_mem_test(int u)
{
    sal_srand(sal_time());

    {
        TH3_MEM_DECLARE(ING_L3_NEXT_HOP);
        TH3_MEM_FIELD_CHECK(u, ING_L3_NEXT_HOP, VLAN_ID, 12);
        TH3_MEM_FIELD_CHECK(u, ING_L3_NEXT_HOP, PORT_NUM, 8);
        TH3_MEM_FIELD_CHECK(u, ING_L3_NEXT_HOP, COPY_TO_CPU, 1);
        TH3_MEM_FIELD_CHECK(u, ING_L3_NEXT_HOP, DROP, 1);
        TH3_MEM_FIELD_CHECK(u, ING_L3_NEXT_HOP, ENTRY_TYPE, 2);
        TH3_MEM_FIELD_CHECK(u, ING_L3_NEXT_HOP, T, 1);
    }
    {
        TH3_MEM_DECLARE(EGR_L3_NEXT_HOP);
        TH3_MEM_FIELD_CHECK(u, EGR_L3_NEXT_HOP, ENTRY_TYPE, 2);
        TH3_MEM_FIELD_CHECK(u, EGR_L3_NEXT_HOP, L3__CLASS_ID, 7);
        TH3_MEM_FIELD_CHECK(u, EGR_L3_NEXT_HOP, IFP_ACTIONS__L3_UC_DA_DISABLE, 1);
        TH3_MEM_FIELD_CHECK(u, EGR_L3_NEXT_HOP, IFP_ACTIONS__L3_UC_SA_DISABLE, 1);
        TH3_MEM_FIELD_CHECK(u, EGR_L3_NEXT_HOP, IFP_ACTIONS__L3_UC_TTL_DISABLE, 1);
        TH3_MEM_FIELD_CHECK(u, EGR_L3_NEXT_HOP, IFP_ACTIONS__L3_UC_VLAN_DISABLE, 1);

        TH3_MEM_FIELD_CHECK(u, EGR_L3_NEXT_HOP, L3__L3_UC_DA_DISABLE, 1);
        TH3_MEM_FIELD_CHECK(u, EGR_L3_NEXT_HOP, L3__L3_UC_SA_DISABLE, 1);
        TH3_MEM_FIELD_CHECK(u, EGR_L3_NEXT_HOP, L3__L3_UC_TTL_DISABLE, 1);
        TH3_MEM_FIELD_CHECK(u, EGR_L3_NEXT_HOP, L3__L3_UC_VLAN_DISABLE, 1);
        TH3_MEM_FIELD_CHECK(u, EGR_L3_NEXT_HOP, MPLS__MAC_DA_PROFILE_INDEX, 9);
        TH3_MEM_FIELD_SPLIT_CHECK(u, EGR_L3_NEXT_HOP, MPLS__MPLS_LABEL, 20, MPLS__MPLS_LABEL_0_1, 2, MPLS__MPLS_LABEL_2_19, 18);
    }
    {
        TH3_MEM_DECLARE(EGR_L3_NEXT_HOP_2);
        TH3_MEM_FIELD_CHECK(u, EGR_L3_NEXT_HOP_2, INTF_NUM, 13);
    }
}
#endif

/* TH3 default route usage result obtained from test DB:
 * grp route, {usage[]}, hlfbnk, acb_cnt, 128b, rsvd, full, {cnt_total[]} */
static alpm_default_usage_t th3_def_usage_combined[] = {
    /* Combined-128 */
    {bcmL3RouteGroupPrivateV4, 366256, { 35,  56,  96}, 0, 2, 1, 0, 1, {1024, 5120, 65536}},
    {bcmL3RouteGroupGlobalV4,  366256, { 35,  56,  96}, 0, 2, 1, 0, 1, {1024, 5120, 65536}},
    {bcmL3RouteGroupPrivateV6, 130863, {100,  40, 100}, 0, 2, 1, 0, 1, { 512, 5120, 65536}},
    {bcmL3RouteGroupGlobalV6,  130863, {100,  40, 100}, 0, 2, 1, 0, 1, { 512, 5120, 65536}},
    {bcmL3RouteGroupPrivateV4, 863721, { 42,  51,  96}, 0, 2, 1, 0, 0, {1024, 5120, 65536}},
    {bcmL3RouteGroupGlobalV4,  863721, { 42,  51,  96}, 0, 2, 1, 0, 0, {1024, 5120, 65536}},
    {bcmL3RouteGroupPrivateV6, 262084, {100,  30, 100}, 0, 2, 1, 0, 0, { 512, 5120, 65536}},
    {bcmL3RouteGroupGlobalV6,  262084, {100,  30, 100}, 0, 2, 1, 0, 0, { 512, 5120, 65536}},

    {bcmL3RouteGroupPrivateV4, 189598, { 18,  58, 100}, 0, 2, 1, 1, 1, {1024, 2560, 32768}},
    {bcmL3RouteGroupGlobalV4,  189598, { 18,  58, 100}, 0, 2, 1, 1, 1, {1024, 2560, 32768}},
    {bcmL3RouteGroupPrivateV6,  65535, { 50,  40, 100}, 0, 2, 1, 1, 1, { 512, 2560, 32768}},
    {bcmL3RouteGroupGlobalV6,   65535, { 50,  40, 100}, 0, 2, 1, 1, 1, { 512, 2560, 32768}},
    {bcmL3RouteGroupPrivateV4, 449861, { 22,  53, 100}, 0, 2, 1, 1, 0, {1024, 2560, 32768}},
    {bcmL3RouteGroupGlobalV4,  449861, { 22,  53, 100}, 0, 2, 1, 1, 0, {1024, 2560, 32768}},
    {bcmL3RouteGroupPrivateV6, 131055, { 50,  30, 100}, 0, 2, 1, 1, 0, { 512, 2560, 32768}},
    {bcmL3RouteGroupGlobalV6,  131055, { 50,  30, 100}, 0, 2, 1, 1, 0, { 512, 2560, 32768}},

    /* half-banks */
    {bcmL3RouteGroupPrivateV4, 174008, { 34,  53, 100}, 1, 2, 1, 0, 1, {1024, 5120, 32768}},
    {bcmL3RouteGroupGlobalV4,  174008, { 34,  53, 100}, 1, 2, 1, 0, 1, {1024, 5120, 32768}},
    {bcmL3RouteGroupPrivateV6,  65423, {100,  40, 100}, 1, 2, 1, 0, 1, { 512, 5120, 32768}},
    {bcmL3RouteGroupGlobalV6,   65423, {100,  40, 100}, 1, 2, 1, 0, 1, { 512, 5120, 32768}},
    {bcmL3RouteGroupPrivateV4, 367902, { 36,  43,  96}, 1, 2, 1, 0, 0, {1024, 5120, 32768}},
    {bcmL3RouteGroupGlobalV4,  367902, { 36,  43,  96}, 1, 2, 1, 0, 0, {1024, 5120, 32768}},
    {bcmL3RouteGroupPrivateV6, 130943, {100,  30, 100}, 1, 2, 1, 0, 0, { 512, 5120, 32768}},
    {bcmL3RouteGroupGlobalV6,  130943, {100,  30, 100}, 1, 2, 1, 0, 0, { 512, 5120, 32768}},

    {bcmL3RouteGroupPrivateV4,  87352, { 17,  54, 100}, 1, 2, 1, 1, 1, {1024, 2560, 16384}},
    {bcmL3RouteGroupGlobalV4,   87352, { 17,  54, 100}, 1, 2, 1, 1, 1, {1024, 2560, 16384}},
    {bcmL3RouteGroupPrivateV6,  32767, { 50,  40, 100}, 1, 2, 1, 1, 1, { 512, 2560, 16384}},
    {bcmL3RouteGroupGlobalV6,   32767, { 50,  40, 100}, 1, 2, 1, 1, 1, { 512, 2560, 16384}},
    {bcmL3RouteGroupPrivateV4, 196461, { 19,  46, 100}, 1, 2, 1, 1, 0, {1024, 2560, 16384}},
    {bcmL3RouteGroupGlobalV4,  196461, { 19,  46, 100}, 1, 2, 1, 1, 0, {1024, 2560, 16384}},
    {bcmL3RouteGroupPrivateV6,  65519, { 50,  30, 100}, 1, 2, 1, 1, 0, { 512, 2560, 16384}},
    {bcmL3RouteGroupGlobalV6,   65519, { 50,  30, 100}, 1, 2, 1, 1, 0, { 512, 2560, 16384}},

    /* levels=2 */
    {bcmL3RouteGroupPrivateV4,  27294, { 84, 100,   0}, 0, 1, 1, 0, 1, {1024, 5120, 0}},
    {bcmL3RouteGroupGlobalV4,   27294, { 84, 100,   0}, 0, 1, 1, 0, 1, {1024, 5120, 0}},
    {bcmL3RouteGroupPrivateV6,   4095, {100,  40,   0}, 0, 1, 1, 0, 1, { 512, 5120, 0}},
    {bcmL3RouteGroupGlobalV6,    4095, {100,  40,   0}, 0, 1, 1, 0, 1, { 512, 5120, 0}},
    {bcmL3RouteGroupPrivateV4,  59356, { 69, 100,   0}, 0, 1, 1, 0, 0, {1024, 5120, 0}},
    {bcmL3RouteGroupGlobalV4,   59356, { 69, 100,   0}, 0, 1, 1, 0, 0, {1024, 5120, 0}},
    {bcmL3RouteGroupPrivateV6,   4107, {100,  20,   0}, 0, 1, 1, 0, 0, { 512, 5120, 0}},
    {bcmL3RouteGroupGlobalV6,    4107, {100,  20,   0}, 0, 1, 1, 0, 0, { 512, 5120, 0}},

    {bcmL3RouteGroupPrivateV4,  13646, { 42, 100,   0}, 0, 1, 1, 1, 1, {1024, 2560, 0}},
    {bcmL3RouteGroupGlobalV4,   13646, { 42, 100,   0}, 0, 1, 1, 1, 1, {1024, 2560, 0}},
    {bcmL3RouteGroupPrivateV6,   4095, {100,  80,   0}, 0, 1, 1, 1, 1, { 512, 2560, 0}},
    {bcmL3RouteGroupGlobalV6,    4095, {100,  80,   0}, 0, 1, 1, 1, 1, { 512, 2560, 0}},
    {bcmL3RouteGroupPrivateV4,  29452, { 35, 100,   0}, 0, 1, 1, 1, 0, {1024, 2560, 0}},
    {bcmL3RouteGroupGlobalV4,   29452, { 35, 100,   0}, 0, 1, 1, 1, 0, {1024, 2560, 0}},
    {bcmL3RouteGroupPrivateV6,   4107, {100,  40,   0}, 0, 1, 1, 1, 0, { 512, 2560, 0}},
    {bcmL3RouteGroupGlobalV6,    4107, {100,  40,   0}, 0, 1, 1, 1, 0, { 512, 2560, 0}},

    /* combined-64 */
    {bcmL3RouteGroupPrivateV4, 366256, { 18,  56,  97}, 0, 2, 0, 0, 1, {2048, 5120, 65536}},
    {bcmL3RouteGroupGlobalV4,  366256, { 18,  56,  97}, 0, 2, 0, 0, 1, {2048, 5120, 65536}},
    {bcmL3RouteGroupPrivateV6, 262091, { 25,  40, 100}, 0, 2, 0, 0, 1, {2048, 5120, 65536}},
    {bcmL3RouteGroupGlobalV6,  262091, { 25,  40, 100}, 0, 2, 0, 0, 1, {2048, 5120, 65536}},
    {bcmL3RouteGroupPrivateV4, 863721, { 21,  51,  96}, 0, 2, 0, 0, 0, {2048, 5120, 65536}},
    {bcmL3RouteGroupGlobalV4,  863721, { 21,  51,  96}, 0, 2, 0, 0, 0, {2048, 5120, 65536}},
    {bcmL3RouteGroupPrivateV6, 524178, { 25,  30, 100}, 0, 2, 0, 0, 0, {2048, 5120, 65536}},
    {bcmL3RouteGroupGlobalV6,  524178, { 25,  30, 100}, 0, 2, 0, 0, 0, {2048, 5120, 65536}},

    /* half-banks */
    {bcmL3RouteGroupPrivateV4, 174008, { 17,  53, 100}, 1, 2, 0, 0, 1, {2048, 5120, 32768}},
    {bcmL3RouteGroupGlobalV4,  174008, { 17,  53, 100}, 1, 2, 0, 0, 1, {2048, 5120, 32768}},
    {bcmL3RouteGroupPrivateV6, 131051, { 25,  40, 100}, 1, 2, 0, 0, 1, {2048, 5120, 32768}},
    {bcmL3RouteGroupGlobalV6,  131051, { 25,  40, 100}, 1, 2, 0, 0, 1, {2048, 5120, 32768}},
    {bcmL3RouteGroupPrivateV4, 367902, { 18,  43,  96}, 1, 2, 0, 0, 0, {2048, 5120, 32768}},
    {bcmL3RouteGroupGlobalV4,  367902, { 18,  43,  96}, 1, 2, 0, 0, 0, {2048, 5120, 32768}},
    {bcmL3RouteGroupPrivateV6, 251663, { 48,  58,  96}, 1, 2, 0, 0, 0, {2048, 5120, 32768}},
    {bcmL3RouteGroupGlobalV6,  251663, { 48,  58,  96}, 1, 2, 0, 0, 0, {2048, 5120, 32768}},

    /* levels=2 */
    {bcmL3RouteGroupPrivateV4,  27294, { 42, 100,   0}, 0, 1, 0, 0, 1, {2048, 5120, 0}},
    {bcmL3RouteGroupGlobalV4,   27294, { 42, 100,   0}, 0, 1, 0, 0, 1, {2048, 5120, 0}},
    {bcmL3RouteGroupPrivateV6,  20463, { 63, 100,   0}, 0, 1, 0, 0, 1, {2048, 5120, 0}},
    {bcmL3RouteGroupGlobalV6,   20463, { 63, 100,   0}, 0, 1, 0, 0, 1, {2048, 5120, 0}},
    {bcmL3RouteGroupPrivateV4,  59356, { 35, 100,   0}, 0, 1, 0, 0, 0, {2048, 5120, 0}},
    {bcmL3RouteGroupGlobalV4,   59356, { 35, 100,   0}, 0, 1, 0, 0, 0, {2048, 5120, 0}},
    {bcmL3RouteGroupPrivateV6,  32771, {100,  80,   0}, 0, 1, 0, 0, 0, {2048, 5120, 0}},
    {bcmL3RouteGroupGlobalV6,   32771, {100,  80,   0}, 0, 1, 0, 0, 0, {2048, 5120, 0}},
};

static alpm_default_usage_t th3_def_usage_parallel[] = {
    /* parallel-128 */
    {bcmL3RouteGroupPrivateV4, 130225, {100,  66,  75}, 0, 2, 1, 0, 1, { 512, 3072, 32768}},
    {bcmL3RouteGroupGlobalV4,   78657, {100,  60,  45}, 0, 2, 1, 0, 1, { 512, 2048, 32768}},
    {bcmL3RouteGroupPrivateV6,  16311, {100,  17,  25}, 0, 2, 1, 0, 1, { 256, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,    8183, {100,  13,  12}, 0, 2, 1, 0, 1, { 256, 2048, 32768}},
    {bcmL3RouteGroupPrivateV4, 242417, {100,  67,  65}, 0, 2, 1, 0, 0, { 512, 3072, 32768}},
    {bcmL3RouteGroupGlobalV4,  255808, {100,  99,  62}, 0, 2, 1, 0, 0, { 512, 2048, 32768}},
    {bcmL3RouteGroupPrivateV6,  32703, {100,  17,  25}, 0, 2, 1, 0, 0, { 256, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,   32527, {100,  25,  25}, 0, 2, 1, 0, 0, { 256, 2048, 32768}},

    {bcmL3RouteGroupPrivateV4,  87352, { 67,  89, 100}, 0, 2, 1, 1, 1, { 512, 1536, 16384}},
    {bcmL3RouteGroupGlobalV4,   65201, { 83, 100,  75}, 0, 2, 1, 1, 1, { 512, 1024, 16384}},
    {bcmL3RouteGroupPrivateV6,  16311, {100,  33,  50}, 0, 2, 1, 1, 1, { 256, 1536, 16384}},
    {bcmL3RouteGroupGlobalV6,    8183, {100,  25,  25}, 0, 2, 1, 1, 1, { 256, 1024, 16384}},
    {bcmL3RouteGroupPrivateV4, 183264, { 75, 100,  97}, 0, 2, 1, 1, 0, { 512, 1536, 16384}},
    {bcmL3RouteGroupGlobalV4,  130449, { 51, 100,  62}, 0, 2, 1, 1, 0, { 512, 1024, 16384}},
    {bcmL3RouteGroupPrivateV6,  32703, {100,  33,  50}, 0, 2, 1, 1, 0, { 256, 1536, 16384}},
    {bcmL3RouteGroupGlobalV6,   32527, {100,  50,  50}, 0, 2, 1, 1, 0, { 256, 1024, 16384}},

    /* half-banks */
    {bcmL3RouteGroupPrivateV4, 138358, {100,  66,  75}, 1, 2, 1, 0, 1, { 512, 3072, 32768}},
    {bcmL3RouteGroupGlobalV4,    8133, {100,  99,   0}, 1, 2, 1, 0, 1, { 512, 2048,     0}},
    {bcmL3RouteGroupPrivateV6,  16824, {100,  17,  25}, 1, 2, 1, 0, 1, { 256, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,     513, {100,  13,   0}, 1, 2, 1, 0, 1, { 256, 2048,     0}},
    {bcmL3RouteGroupPrivateV4, 258688, {100,  67,  65}, 1, 2, 1, 0, 0, { 512, 3072, 32768}},
    {bcmL3RouteGroupGlobalV4,   16271, {100,  99,   0}, 1, 2, 1, 0, 0, { 512, 2048,     0}},
    {bcmL3RouteGroupPrivateV6,  33729, {100,  17,  25}, 1, 2, 1, 0, 0, { 256, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,    1026, {100,  13,   0}, 1, 2, 1, 0, 0, { 256, 2048,     0}},

    {bcmL3RouteGroupPrivateV4,  91444, { 67,  89, 100}, 1, 2, 1, 1, 1, { 512, 1536, 16384}},
    {bcmL3RouteGroupGlobalV4,    4092, { 50, 100,   0}, 1, 2, 1, 1, 1, { 512, 1024,     0}},
    {bcmL3RouteGroupPrivateV6,  16824, {100,  33,  50}, 1, 2, 1, 1, 1, { 256, 1536, 16384}},
    {bcmL3RouteGroupGlobalV6,     513, {100,  25,   0}, 1, 2, 1, 1, 1, { 256, 1024,     0}},
    {bcmL3RouteGroupPrivateV4, 191464, { 75, 100,  96}, 1, 2, 1, 1, 0, { 512, 1536, 16384}},
    {bcmL3RouteGroupGlobalV4,    8200, { 51, 100,   0}, 1, 2, 1, 1, 0, { 512, 1024,     0}},
    {bcmL3RouteGroupPrivateV6,  33729, {100,  33,  50}, 1, 2, 1, 1, 0, { 256, 1536, 16384}},
    {bcmL3RouteGroupGlobalV6,    1026, {100,  25,   0}, 1, 2, 1, 1, 0, { 256, 1024,     0}},

    /* levels=2 */
    {bcmL3RouteGroupPrivateV4,  16307, {100, 100,   0}, 0, 1, 1, 0, 1, { 512, 3072, 0}},
    {bcmL3RouteGroupGlobalV4,    8133, {100, 100,   0}, 0, 1, 1, 0, 1, { 512, 2048, 0}},
    {bcmL3RouteGroupPrivateV6,   1027, {100,  17,   0}, 0, 1, 1, 0, 1, { 256, 3072, 0}},
    {bcmL3RouteGroupGlobalV6,     513, {100,  13,   0}, 0, 1, 1, 0, 1, { 256, 2048, 0}},
    {bcmL3RouteGroupPrivateV4,  32510, {100, 100,   0}, 0, 1, 1, 0, 0, { 512, 3072, 0}},
    {bcmL3RouteGroupGlobalV4,   16271, {100, 100,   0}, 0, 1, 1, 0, 0, { 512, 2048, 0}},
    {bcmL3RouteGroupPrivateV6,   2045, {100,  17,   0}, 0, 1, 1, 0, 0, { 256, 3072, 0}},
    {bcmL3RouteGroupGlobalV6,    1026, {100,  13,   0}, 0, 1, 1, 0, 0, { 256, 2048, 0}},

    {bcmL3RouteGroupPrivateV4,   8194, { 51, 100,   0}, 0, 1, 1, 1, 1, { 512, 1536, 0}},
    {bcmL3RouteGroupGlobalV4,    4092, { 50, 100,   0}, 0, 1, 1, 1, 1, { 512, 1024, 0}},
    {bcmL3RouteGroupPrivateV6,   1027, {100,  33,   0}, 0, 1, 1, 1, 1, { 256, 1536, 0}},
    {bcmL3RouteGroupGlobalV6,     513, {100,  25,   0}, 0, 1, 1, 1, 1, { 256, 1024, 0}},
    {bcmL3RouteGroupPrivateV4,  16381, { 51, 100,   0}, 0, 1, 1, 1, 0, { 512, 1536, 0}},
    {bcmL3RouteGroupGlobalV4,    8200, { 51, 100,   0}, 0, 1, 1, 1, 0, { 512, 1024, 0}},
    {bcmL3RouteGroupPrivateV6,   2045, {100,  33,   0}, 0, 1, 1, 1, 0, { 256, 1536, 0}},
    {bcmL3RouteGroupGlobalV6,    1026, {100,  25,   0}, 0, 1, 1, 1, 0, { 256, 1024, 0}},

    /* parallel-64 */
    {bcmL3RouteGroupPrivateV4, 174072, { 67,  89, 100}, 0, 2, 0, 0, 1, {1024, 3072, 32768}},
    {bcmL3RouteGroupGlobalV4,  130737, { 82, 100,  75}, 0, 2, 0, 0, 1, {1024, 2048, 32768}},
    {bcmL3RouteGroupPrivateV6, 130991, {100,  67, 100}, 0, 2, 0, 0, 1, {1024, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,   65535, {100,  50,  50}, 0, 2, 0, 0, 1, {1024, 2048, 32768}},
    {bcmL3RouteGroupPrivateV4, 350444, { 73,  97,  96}, 0, 2, 0, 0, 0, {1024, 3072, 32768}},
    {bcmL3RouteGroupGlobalV4,  257856, { 50, 100,  62}, 0, 2, 0, 0, 0, {1024, 2048, 32768}},
    {bcmL3RouteGroupPrivateV6, 131167, {100,  67,  50}, 0, 2, 0, 0, 0, {1024, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  130831, {100, 100,  50}, 0, 2, 0, 0, 0, {1024, 2048, 32768}},

    /* half-banks */
    {bcmL3RouteGroupPrivateV4, 182261, { 67,  89, 100}, 1, 2, 0, 0, 1, {1024, 3072, 32768}},
    {bcmL3RouteGroupGlobalV4,    8189, { 50, 100,   0}, 1, 2, 0, 0, 1, {1024, 2048,     0}},
    {bcmL3RouteGroupPrivateV6, 135090, {100,  67, 100}, 1, 2, 0, 0, 1, {1024, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,    4099, {100,  50,   0}, 1, 2, 0, 0, 1, {1024, 2048,     0}},
    {bcmL3RouteGroupPrivateV4, 366836, { 73,  97,  96}, 1, 2, 0, 0, 0, {1024, 3072, 32768}},
    {bcmL3RouteGroupGlobalV4,   16392, { 50, 100,   0}, 1, 2, 0, 0, 0, {1024, 2048,     0}},
    {bcmL3RouteGroupPrivateV6, 135267, {100,  67,  50}, 1, 2, 0, 0, 0, {1024, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,    4100, {100,  50,   0}, 1, 2, 0, 0, 0, {1024, 2048,     0}},

    /* levels=2 */
    {bcmL3RouteGroupPrivateV4,  16386, { 50, 100,   0}, 0, 1, 0, 0, 1, {1024, 3072, 0}},
    {bcmL3RouteGroupGlobalV4,    8189, { 50, 100,   0}, 0, 1, 0, 0, 1, {1024, 2048, 0}},
    {bcmL3RouteGroupPrivateV6,   8189, {100,  67,   0}, 0, 1, 0, 0, 1, {1024, 3072, 0}},
    {bcmL3RouteGroupGlobalV6,    4099, {100,  50,   0}, 0, 1, 0, 0, 1, {1024, 2048, 0}},
    {bcmL3RouteGroupPrivateV4,  32765, { 50, 100,   0}, 0, 1, 0, 0, 0, {1024, 3072, 0}},
    {bcmL3RouteGroupGlobalV4,   16392, { 50, 100,   0}, 0, 1, 0, 0, 0, {1024, 2048, 0}},
    {bcmL3RouteGroupPrivateV6,   8201, {100,  33,   0}, 0, 1, 0, 0, 0, {1024, 3072, 0}},
    {bcmL3RouteGroupGlobalV6,    4100, {100,  50,   0}, 0, 1, 0, 0, 0, {1024, 2048, 0}},
};

static alpm_default_usage_t th3_def_usage_mixed[] = {
    /* alpm/tcam-128 */
    {bcmL3RouteGroupPrivateV4, 366256, { 71,  56,  97}, 0, 2, 1, 0, 1, { 512, 5120, 65536}},
    {bcmL3RouteGroupGlobalV4,    1023, {100,   0,   0}, 0, 2, 1, 0, 1, { 512,    0,     0}},
    {bcmL3RouteGroupPrivateV6,  65327, {100,  20,  50}, 0, 2, 1, 0, 1, { 256, 5120, 65536}},
    {bcmL3RouteGroupGlobalV6,     256, {100,   0,   0}, 0, 2, 1, 0, 1, { 256,    0,     0}},
    {bcmL3RouteGroupPrivateV4, 863721, { 83,  50,  96}, 0, 2, 1, 0, 0, { 512, 5120, 65536}},
    {bcmL3RouteGroupGlobalV4,    1023, {100,   0,   0}, 0, 2, 1, 0, 0, { 512,    0,     0}},
    {bcmL3RouteGroupPrivateV6, 131087, {100,  15,  50}, 0, 2, 1, 0, 0, { 256, 5120, 65536}},
    {bcmL3RouteGroupGlobalV6,     256, {100,   0,   0}, 0, 2, 1, 0, 0, { 256,    0,     0}},

    {bcmL3RouteGroupPrivateV4, 189598, { 37,  58, 100}, 0, 2, 1, 1, 1, { 512, 2560, 32768}},
    {bcmL3RouteGroupGlobalV4,    1023, {100,   0,   0}, 0, 2, 1, 1, 1, { 512,    0,     0}},
    {bcmL3RouteGroupPrivateV6,  65327, {100,  40, 100}, 0, 2, 1, 1, 1, { 256, 2560, 32768}},
    {bcmL3RouteGroupGlobalV6,     256, {100,   0,   0}, 0, 2, 1, 1, 1, { 256,    0,     0}},
    {bcmL3RouteGroupPrivateV4, 449861, { 43,  53, 100}, 0, 2, 1, 1, 0, { 512, 2560, 32768}},
    {bcmL3RouteGroupGlobalV4,    1023, {100,   0,   0}, 0, 2, 1, 1, 0, { 512,    0,     0}},
    {bcmL3RouteGroupPrivateV6, 131055, {100,  30, 100}, 0, 2, 1, 1, 0, { 256, 2560, 32768}},
    {bcmL3RouteGroupGlobalV6,     256, {100,   0,   0}, 0, 2, 1, 1, 0, { 256,    0,     0}},

    /* half-banks */
    {bcmL3RouteGroupPrivateV4, 174008, { 68,  53, 100}, 1, 2, 1, 0, 1, { 512, 5120, 32768}},
    {bcmL3RouteGroupGlobalV4,    1023, {100,   0,   0}, 1, 2, 1, 0, 1, { 512,    0,     0}},
    {bcmL3RouteGroupPrivateV6,  32655, {100,  20,  50}, 1, 2, 1, 0, 1, { 256, 5120, 32768}},
    {bcmL3RouteGroupGlobalV6,     256, {100,   0,   0}, 1, 2, 1, 0, 1, { 256,    0,     0}},
    {bcmL3RouteGroupPrivateV4, 367902, { 71,  43,  96}, 1, 2, 1, 0, 0, { 512, 5120, 32768}},
    {bcmL3RouteGroupGlobalV4,    1023, {100,   0,   0}, 1, 2, 1, 0, 0, { 512,    0,     0}},
    {bcmL3RouteGroupPrivateV6,  65407, {100,  15,  50}, 1, 2, 1, 0, 0, { 256, 5120, 32768}},
    {bcmL3RouteGroupGlobalV6,     256, {100,   0,   0}, 1, 2, 1, 0, 0, { 256,    0,     0}},

    {bcmL3RouteGroupPrivateV4,  87352, { 34,  54, 100}, 1, 2, 1, 1, 1, { 512, 2560, 16384}},
    {bcmL3RouteGroupGlobalV4,    1023, {100,   0,   0}, 1, 2, 1, 1, 1, { 512,    0,     0}},
    {bcmL3RouteGroupPrivateV6,  32655, {100,  40, 100}, 1, 2, 1, 1, 1, { 256, 2560, 16384}},
    {bcmL3RouteGroupGlobalV6,     256, {100,   0,   0}, 1, 2, 1, 1, 1, { 256,    0,     0}},
    {bcmL3RouteGroupPrivateV4, 196461, { 38,  46, 100}, 1, 2, 1, 1, 0, { 512, 2560, 16384}},
    {bcmL3RouteGroupGlobalV4,    1023, {100,   0,   0}, 1, 2, 1, 1, 0, { 512,    0,     0}},
    {bcmL3RouteGroupPrivateV6,  65407, {100,  30, 100}, 1, 2, 1, 1, 0, { 256, 2560, 16384}},
    {bcmL3RouteGroupGlobalV6,     256, {100,   0,   0}, 1, 2, 1, 1, 0, { 256,    0,     0}},

    /* levels=2 */
    {bcmL3RouteGroupPrivateV4,  16207, {100,  59,   0}, 0, 1, 1, 0, 1, { 512, 5120, 0}},
    {bcmL3RouteGroupGlobalV4,    1023, {100,   0,   0}, 0, 1, 1, 0, 1, { 512,    0, 0}},
    {bcmL3RouteGroupPrivateV6,   2047, {100,  20,   0}, 0, 1, 1, 0, 1, { 256, 5120, 0}},
    {bcmL3RouteGroupGlobalV6,     256, {100,   0,   0}, 0, 1, 1, 0, 1, { 256,    0, 0}},
    {bcmL3RouteGroupPrivateV4,  42812, {100,  72,   0}, 0, 1, 1, 0, 0, { 512, 5120, 0}},
    {bcmL3RouteGroupGlobalV4,    1023, {100,   0,   0}, 0, 1, 1, 0, 0, { 512,    0, 0}},
    {bcmL3RouteGroupPrivateV6,   2059, {100,  10,   0}, 0, 1, 1, 0, 0, { 256, 5120, 0}},
    {bcmL3RouteGroupGlobalV6,     256, {100,   0,   0}, 0, 1, 1, 0, 0, { 256,    0, 0}},

    {bcmL3RouteGroupPrivateV4,  13646, { 84, 100,   0}, 0, 1, 1, 1, 1, { 512, 2560, 0}},
    {bcmL3RouteGroupGlobalV4,    1023, {100,   0,   0}, 0, 1, 1, 1, 1, { 512,    0, 0}},
    {bcmL3RouteGroupPrivateV6,   2047, {100,  40,   0}, 0, 1, 1, 1, 1, { 256, 2560, 0}},
    {bcmL3RouteGroupGlobalV6,     256, {100,   0,   0}, 0, 1, 1, 1, 1, { 256,    0, 0}},
    {bcmL3RouteGroupPrivateV4,  29452, { 71, 100,   0}, 0, 1, 1, 1, 0, { 512, 2560, 0}},
    {bcmL3RouteGroupGlobalV4,    1023, {100,   0,   0}, 0, 1, 1, 1, 0, { 512,    0, 0}},
    {bcmL3RouteGroupPrivateV6,   2059, {100,  20,   0}, 0, 1, 1, 1, 0, { 256, 2560, 0}},
    {bcmL3RouteGroupGlobalV6,     256, {100,   0,   0}, 0, 1, 1, 1, 0, { 256,    0, 0}},

    /* alpm/tcam-64 */
    {bcmL3RouteGroupPrivateV4, 366256, { 35,  56,  97}, 0, 2, 0, 0, 1, {1024, 5120, 65536}},
    {bcmL3RouteGroupGlobalV4,    2047, {100,   0,   0}, 0, 2, 0, 0, 1, {1024,    0,     0}},
    {bcmL3RouteGroupPrivateV6, 262091, { 50,  40, 100}, 0, 2, 0, 0, 1, {1024, 5120, 65536}},
    {bcmL3RouteGroupGlobalV6,    1024, {100,   0,   0}, 0, 2, 0, 0, 1, {1024,    0,     0}},
    {bcmL3RouteGroupPrivateV4, 863721, { 42,  51,  96}, 0, 2, 0, 0, 0, {1024, 5120, 65536}},
    {bcmL3RouteGroupGlobalV4,    2047, {100,   0,   0}, 0, 2, 0, 0, 0, {1024,    0,     0}},
    {bcmL3RouteGroupPrivateV6, 524178, { 50,  30, 100}, 0, 2, 0, 0, 0, {1024, 5120, 65536}},
    {bcmL3RouteGroupGlobalV6,    1024, {100,   0,   0}, 0, 2, 0, 0, 0, {1024,    0,     0}},

    /* half-banks */
    {bcmL3RouteGroupPrivateV4, 174008, { 34,  53, 100}, 1, 2, 0, 0, 1, {1024, 5120, 32768}},
    {bcmL3RouteGroupGlobalV4,    2047, {100,   0,   0}, 1, 2, 0, 0, 1, {1024,    0,     0}},
    {bcmL3RouteGroupPrivateV6, 131051, { 50,  40, 100}, 1, 2, 0, 0, 1, {1024, 5120, 32768}},
    {bcmL3RouteGroupGlobalV6,    1024, {100,   0,   0}, 1, 2, 0, 0, 1, {1024,    0,     0}},
    {bcmL3RouteGroupPrivateV4, 367902, { 36,  43,  96}, 1, 2, 0, 0, 0, {1024, 5120, 32768}},
    {bcmL3RouteGroupGlobalV4,    2047, {100,   0,   0}, 1, 2, 0, 0, 0, {1024,    0,     0}},
    {bcmL3RouteGroupPrivateV6, 251663, { 96,  58,  96}, 1, 2, 0, 0, 0, {1024, 5120, 32768}},
    {bcmL3RouteGroupGlobalV6,    1024, {100,   0,   0}, 1, 2, 0, 0, 0, {1024,    0,     0}},

    /* levels=2 */
    {bcmL3RouteGroupPrivateV4,  27294, { 84, 100,   0}, 0, 1, 0, 0, 1, {1024, 5120, 0}},
    {bcmL3RouteGroupGlobalV4,    2047, {100,   0,   0}, 0, 1, 0, 0, 1, {1024,    0, 0}},
    {bcmL3RouteGroupPrivateV6,  16383, {100,  80,   0}, 0, 1, 0, 0, 1, {1024, 5120, 0}},
    {bcmL3RouteGroupGlobalV6,    1024, {100,   0,   0}, 0, 1, 0, 0, 1, {1024,    0, 0}},
    {bcmL3RouteGroupPrivateV4,  59356, { 69, 100,   0}, 0, 1, 0, 0, 0, {1024, 5120, 0}},
    {bcmL3RouteGroupGlobalV4,    2047, {100,   0,   0}, 0, 1, 0, 0, 0, {1024,    0, 0}},
    {bcmL3RouteGroupPrivateV6,  16387, {100,  40,   0}, 0, 1, 0, 0, 0, {1024, 5120, 0}},
    {bcmL3RouteGroupGlobalV6,    1024, {100,   0,   0}, 0, 1, 0, 0, 0, {1024,    0, 0}},
};

#define TBLSIZE(tbl) (sizeof(tbl)/sizeof(tbl[0]))
static alpm_default_usage_t *th3_def_usage[] = {
    th3_def_usage_combined,
    th3_def_usage_parallel,
    th3_def_usage_mixed
};
static int th3_def_usage_tblsize[] = {
    TBLSIZE(th3_def_usage_combined),
    TBLSIZE(th3_def_usage_parallel),
    TBLSIZE(th3_def_usage_mixed)
};

static _alpm_fmt_info_t th3_fmt_info[ALPM_FMT_CNT + 1] = {
    {
        ALPM_FMT_PVT,
        8,      /* 8 different formats(PIVOT FORMAT) */
        {       /* For fmt_ent_max */
            0,  /* FMT=0, means we cannot put entry in this format */
            6,  /* allow 6 entires in PIVOT FMT1 */
            5,
            4,
            3,
            2,
            4,
            3,
            2,  /* allow 2 entries in PIVOT FMT8 */
            0,
            0,
            0,
            0
        },
        {       /* For fmt_pfx_len */
            0,
            7,  /* PIVOT FMT1 */
            23,
            47,
            87,
            128,
            17,
            57,
            128,/* PIVOT FMT8 */
            0,
            0,
            0,
            0
        },
        {       /* fmt_bnk */
            INVALIDfmt,
            L3_DEFIP_ALPM_PIVOT_FMT1_FULLfmt,
            L3_DEFIP_ALPM_PIVOT_FMT2_FULLfmt,
            L3_DEFIP_ALPM_PIVOT_FMT3_FULLfmt,
            L3_DEFIP_ALPM_PIVOT_FMT4_FULLfmt,
            L3_DEFIP_ALPM_PIVOT_FMT5_FULLfmt,
            L3_DEFIP_ALPM_PIVOT_FMT6_FULLfmt,
            L3_DEFIP_ALPM_PIVOT_FMT7_FULLfmt,
            L3_DEFIP_ALPM_PIVOT_FMT8_FULLfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt
        },
        {       /* fmt_ent */
            INVALIDfmt,
            L3_DEFIP_ALPM_PIVOT_FMT1fmt,
            L3_DEFIP_ALPM_PIVOT_FMT2fmt,
            L3_DEFIP_ALPM_PIVOT_FMT3fmt,
            L3_DEFIP_ALPM_PIVOT_FMT4fmt,
            L3_DEFIP_ALPM_PIVOT_FMT5fmt,
            L3_DEFIP_ALPM_PIVOT_FMT6fmt,
            L3_DEFIP_ALPM_PIVOT_FMT7fmt,
            L3_DEFIP_ALPM_PIVOT_FMT8fmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt
        },
        {0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
    },  /* PIVOT fmt info */
    {
        ALPM_FMT_RTE,
        13,     /* 13 different formats(ROUTE FORMAT) */
        {       /* For fmt_ent_max */
            0,  /* FMT=0, means we cannot put entry in this format */
            20, /* FMT13 */
            15, /* allow 15 entires in ROUTE FMT1 */
            12,
            10,
            8,
            7,
            6,
            5,
            4,
            3,
            6,
            4,
            2   /* allow 2 entries in ROUTE FMT12 */
        },
        {       /* For fmt_pfx_len */
            0,
            7,  /* ROUTE FMT13 */
            15, /* ROUTE FMT1 */
            23,
            31,
            39,
            47,
            63,
            79,
            103,
            128,
            33,
            73,
            128  /* ROUTE FMT12 */
        },
        {       /* fmt_bnk */
            INVALIDfmt,
            INVALIDfmt, /* L3_DEFIP_ALPM_ROUTE_FMT13_FULLfmt, */
            L3_DEFIP_ALPM_ROUTE_FMT1_FULLfmt,
            L3_DEFIP_ALPM_ROUTE_FMT2_FULLfmt,
            L3_DEFIP_ALPM_ROUTE_FMT3_FULLfmt,
            L3_DEFIP_ALPM_ROUTE_FMT4_FULLfmt,
            L3_DEFIP_ALPM_ROUTE_FMT5_FULLfmt,
            L3_DEFIP_ALPM_ROUTE_FMT6_FULLfmt,
            L3_DEFIP_ALPM_ROUTE_FMT7_FULLfmt,
            L3_DEFIP_ALPM_ROUTE_FMT8_FULLfmt,
            L3_DEFIP_ALPM_ROUTE_FMT9_FULLfmt,
            L3_DEFIP_ALPM_ROUTE_FMT10_FULLfmt,
            L3_DEFIP_ALPM_ROUTE_FMT11_FULLfmt,
            L3_DEFIP_ALPM_ROUTE_FMT12_FULLfmt
        },
        {       /* fmt_ent */
            INVALIDfmt,
            L3_DEFIP_ALPM_ROUTE_FMT13fmt,
            L3_DEFIP_ALPM_ROUTE_FMT1fmt,
            L3_DEFIP_ALPM_ROUTE_FMT2fmt,
            L3_DEFIP_ALPM_ROUTE_FMT3fmt,
            L3_DEFIP_ALPM_ROUTE_FMT4fmt,
            L3_DEFIP_ALPM_ROUTE_FMT5fmt,
            L3_DEFIP_ALPM_ROUTE_FMT6fmt,
            L3_DEFIP_ALPM_ROUTE_FMT7fmt,
            L3_DEFIP_ALPM_ROUTE_FMT8fmt,
            L3_DEFIP_ALPM_ROUTE_FMT9fmt,
            L3_DEFIP_ALPM_ROUTE_FMT10fmt,
            L3_DEFIP_ALPM_ROUTE_FMT11fmt,
            L3_DEFIP_ALPM_ROUTE_FMT12fmt
        },
        {0,
         0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
    },  /* ROUTE fmt info */
    {
        ALPM_FMT_CNT,
        0,      /* 0 different formats */
        {       /* For fmt_ent_max */
            0,  /* FMT=0, means we cannot put entry in this format */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        {       /* For fmt_pfx_len */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        {       /* fmt_bnk */
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt
        },
        {       /* fmt_ent */
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt,
            INVALIDfmt
        },
        {0,
         0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
};

soc_field_t th3_alpm_ent_fld[] = {
    ENTRY_0f, ENTRY_1f, ENTRY_2f, ENTRY_3f,
    ENTRY_4f, ENTRY_5f, ENTRY_6f, ENTRY_7f,
    ENTRY_8f, ENTRY_9f, ENTRY_10f,ENTRY_11f,
    ENTRY_12f,ENTRY_13f,ENTRY_14f,ENTRY_15f,
    ENTRY_16f,ENTRY_17f,ENTRY_18f,ENTRY_19f,
};

soc_field_t th3_alpm_bnk_fld[] = {
    FMT0f, FMT1f, FMT2f, FMT3f,
    FMT4f, FMT5f, FMT6f, FMT7f
};

static soc_mem_t th3_alpm_hit_tbl_mem[] = {
    L3_DEFIP_ALPM_LEVEL2_HIT_ONLYm,
    L3_DEFIP_ALPM_LEVEL3_HIT_ONLYm
};

/* Should be the same order as _alpm_tbl_t */
static soc_mem_t th3_alpm_tbl[] = {
    INVALIDm,
    L3_DEFIP_LEVEL1m,
    L3_DEFIP_LEVEL1m,
    L3_DEFIP_PAIR_LEVEL1m,
    L3_DEFIP_PAIR_LEVEL1m,
    L3_DEFIP_ALPM_LEVEL2m,
    L3_DEFIP_ALPM_LEVEL2m,
    L3_DEFIP_ALPM_LEVEL2m,
    L3_DEFIP_ALPM_LEVEL2m,
    L3_DEFIP_ALPM_LEVEL3m,
    L3_DEFIP_ALPM_LEVEL3m,
    L3_DEFIP_ALPM_LEVEL3m,
    INVALIDm
};

static soc_mem_t *alpm_tbl;
/* Doesn't have to be unit-specific, 2: 0 Reduce, 1 Full */
static uint8 th3_alpm_def_fmt[ALPM_FMT_CNT][2];

/* Combined mode Level2 Table */
static _alpm_bkt_pool_conf_t th3_alpm_cmode_pool_l2 = {
    {NULL, NULL},  /* bnk_info, to be allocated */
    5120,   /* 5 Banks x 1K buckets */
    5,      /* 5 Banks per bucket */
    alpmTblBktCmodeL2,
    1,      /* Wrap around bucket */
    0,      /* Not Fixed FMT */
    NULL,
    &th3_fmt_info[ALPM_FMT_PVT],
};

/* Combined mode Level3 Table */
static _alpm_bkt_pool_conf_t th3_alpm_cmode_pool_l3 = {
    {NULL, NULL},  /* bnk_info, to be allocated */
    65536,  /* 8 Banks x 8K buckets */
    8,      /* 8 Banks per bucket */
    alpmTblBktCmodeL3,
    1,
    0,      /* Not Fixed FMT */
    NULL,
    &th3_fmt_info[ALPM_FMT_RTE],
};

/* Parallel mode Level2 Table pool for Global Routes */
static _alpm_bkt_pool_conf_t th3_alpm_pmode_pool_glb_l2 = {
    {NULL, NULL},  /* bnk_info, to be allocated */
    2048,   /* 2 Banks x 1K buckets */
    2,      /* 2 Banks per bucket */
    alpmTblBktPmodeGlbL2,
    1,
    0,      /* Not Fixed FMT */
    NULL,
    &th3_fmt_info[ALPM_FMT_PVT],
};

/* Parallel mode Level2 Table pool for Private Routes */
static _alpm_bkt_pool_conf_t th3_alpm_pmode_pool_prt_l2 = {
    {NULL, NULL},  /* bnk_info, to be allocated */
    3072,   /* 3 Banks x 1K buckets */
    3,      /* 3 Banks per bucket */
    alpmTblBktPmodePrtL2,
    1,
    0,      /* Not Fixed FMT */
    NULL,
    &th3_fmt_info[ALPM_FMT_PVT],
};

/* Parallel mode Level3 Table pool for Global Routes */
static _alpm_bkt_pool_conf_t th3_alpm_pmode_pool_glb_l3 = {
    {NULL, NULL},  /* bnk_info, to be allocated */
    32768,  /* 4 Banks x 8K buckets */
    4,      /* 4 Banks per bucket */
    alpmTblBktPmodeGlbL3,
    1,
    0,      /* Not Fixed FMT */
    NULL,
    &th3_fmt_info[ALPM_FMT_RTE],
};

/* Parallel mode Level3 Table pool for Private Routes */
static _alpm_bkt_pool_conf_t th3_alpm_pmode_pool_prt_l3 = {
    {NULL, NULL},  /* bnk_info, to be allocated */
    32768,  /* 4 Banks x 8K buckets */
    4,      /* 4 Banks per bucket */
    alpmTblBktPmodePrtL3,
    1,
    0,      /* Not Fixed FMT */
    NULL,
    &th3_fmt_info[ALPM_FMT_RTE],
};

static _alpm_bkt_pool_conf_t *th3_alpm_pool0_l2[SOC_MAX_NUM_DEVICES] = {NULL};
static _alpm_bkt_pool_conf_t *th3_alpm_pool1_l2[SOC_MAX_NUM_DEVICES] = {NULL};
static _alpm_bkt_pool_conf_t *th3_alpm_pool0_l3[SOC_MAX_NUM_DEVICES] = {NULL};
static _alpm_bkt_pool_conf_t *th3_alpm_pool1_l3[SOC_MAX_NUM_DEVICES] = {NULL};

static _alpm_cb_t th3_alpm_1cb_template = {
    0,          /* app */
    0,          /* Unit */
    {
        NULL,               /* pvt_ctl[V4] */
        NULL,               /* pvt_ctl[V6] */
    },
    {                   /* Pivot Tables of this block */
        alpmTblPvtCmodeL1,
        alpmTblPvtCmodeL1,
        alpmTblInvalid
    },
    {   /* _alpm_bnk_conf_t */
        {NULL, NULL}, /* bnk_pool pointer */
        10,     /* 1K buckets = 10 bits */
        0,      /* Single Wide */
        5,      /* 5 physical banks per bucket */
        3,      /* 3 bits */
    },
    0                   /* Block 0 */
};

static _alpm_cb_t th3_alpm_1cb_template_128b = {
    0,          /* app */
    0,          /* Unit */
    {
        NULL,               /* pvt_ctl[V4] */
        NULL,               /* pvt_ctl[V6] */
    },
    {                   /* Pivot Tables of this block */
        alpmTblPvtCmodeL1,
        alpmTblPvtCmodeL1P128,
        alpmTblPvtCmodeL1P128
    },
    {   /* _alpm_bnk_conf_t */
        {NULL, NULL}, /* bnk_pool pointer */
        10,     /* 1K buckets = 10 bits */
        0,      /* Single Wide */
        5,      /* 5 physical banks per bucket */
        3,      /* 3 bits */
    },
    0                   /* Block 0 */
};

static _alpm_cb_t th3_alpm_2cb_template_p0 = {
    0,          /* app */
    0,          /* Unit */
    {
        NULL,               /* pvt_ctl[V4] */
        NULL,               /* pvt_ctl[V6] */
    },
    {                   /* Pivot Tables of this block */
        alpmTblPvtCmodeL1,
        alpmTblPvtCmodeL1,
        alpmTblInvalid
    },
    {   /* _alpm_bnk_conf_t */
        {NULL, NULL}, /* bnk_pool pointer */
        10,     /* 1K buckets = 10 bits */
        0,      /* Single Wide */
        5,      /* 5 physical banks per bucket */
        3,      /* 3 bits */
    },
    0                   /* ALPM block 0 */
};

static _alpm_cb_t th3_alpm_2cb_template_p0_128b = {
    0,          /* app */
    0,          /* Unit */
    {
        NULL,               /* pvt_ctl[V4] */
        NULL,               /* pvt_ctl[V6] */
    },
    {                   /* Pivot Tables of this block */
        alpmTblPvtCmodeL1,
        alpmTblPvtCmodeL1P128,
        alpmTblPvtCmodeL1P128
    },
    {   /* _alpm_bnk_conf_t */
        {NULL, NULL}, /* bnk_pool pointer */
        10,     /* 1K buckets = 10 bits */
        0,      /* Single Wide */
        5,      /* 5 physical banks per bucket */
        3,      /* 3 bits */
    },
    0                   /* Block 0 */
};

static _alpm_cb_t th3_alpm_2cb_template_p1 = {
    0,          /* app */
    0,          /* Unit */
    {
        NULL,               /* pvt_ctl[V4] */
        NULL,               /* pvt_ctl[V6] */
    },
    {                   /* Pivot Tables of this block */
        alpmTblInvalid,
        alpmTblInvalid,
        alpmTblInvalid
    },
    {   /* _alpm_bnk_conf_t */
        {NULL, NULL}, /* bnk_pool pointer */
        13,     /* 8K buckets = 13 bits */
        0,      /* Single Wide */
        8,      /* 8 physical banks per bucket */
        3,      /* 3 bits */
    },
    1                   /* Block 1 */
};

/* local used functions */
int th3_alpm_bkt_hit_write(int u, int vrf_id, _alpm_cb_t *acb,
                           _alpm_tbl_t tbl, int ent_idx, int hit_val);
int th3_alpm_ent_hit_move(int u, int vrf_id, _alpm_cb_t *acb,
                          int src_ent_idx, int dst_ent_idx);
int th3_alpm_bnk_hit_move(int u, int vrf_id, _alpm_cb_t *acb,
                          int src_idx, int dst_idx);
int8 th3_alpm_bnk_fmt_get(int u, _alpm_cb_t *acb, _alpm_bkt_info_t *bkt_info,
                          int ent_idx);
int th3_alpm_table_dump_3level(int u, int app, soc_mem_t mem, int idx, int sub_idx,
                               int lvl_cnt, int flags);

/*#define MIS_FORWARDING_CHECK 1*/

#ifdef MIS_FORWARDING_CHECK
/* TH3 ALPM traffic hit and route mis-fowarding debug:
   This feature can be enables to trace all ALPM memory writes for
   a specific trace route sequence. It reports:
   1) Show each mem_write cli command (addr, fields).
   2) Show counter on all ports after each mem_write.
   3) For level 1 mem_write, it dumps ALPM table under this pivot.
   4) For level 1 & 2 mem_write, it does Hw invalid routing check. */

#define MIS_TR_SEQ  931834
extern int do_show_counters(int unit, soc_reg_t ctr_reg,
                            soc_pbmp_t pbmp, int flags);
int
th3_mem_write_check(int u, soc_mem_t mem, int phy_index, void *entry)
{
    static int tot_err = 1; /* 0 for invalid hw route check */
    int app = APP0;
    int rv = BCM_E_NONE;

    if (bcm_esw_alpm_trace_seq(u) == MIS_TR_SEQ) {
        pbmp_t pbmp;
        SOC_PBMP_CLEAR(pbmp);
        SOC_PBMP_OR(pbmp, PBMP_PORT_ALL(u));
        SOC_PBMP_OR(pbmp, PBMP_CMIC(u));

        if (mem == L3_DEFIP_ALPM_LEVEL2m || (mem == L3_DEFIP_ALPM_LEVEL3m)) {
            cli_out("w %s %d 1 ", SOC_MEM_NAME(u, mem), phy_index);
            soc_mem_entry_dump(u, mem, entry, BSL_LSS_CLI);
        } else {
            cli_out("mod %s %d 1 ", SOC_MEM_NAME(u, mem), phy_index);
            soc_mem_entry_dump_if_changed(u, mem, entry, "");
            th3_alpm_table_dump_3level(u, app, mem, phy_index, 0, 2, 0);
        }
        if (!tot_err && mem != L3_DEFIP_ALPM_LEVEL3m) {
            (void)alpm_pfx_hw_route_sanity(u, app, &tot_err);
        }
        /* show counter for traffic hit */
        do_show_counters(u, -1, pbmp, 9);
    }
    return rv;
}
#endif

typedef struct _alpm_custom_bkt_attr_s {
    int l2_phy_bnks;
    int l2_phy_bkts;
    int l2_glb_bnks;
    int l2_prt_bnks;
    int l3_phy_bnks;
    int l3_phy_bkts;
    int l3_glb_bnks;
    int l3_prt_bnks;
} _alpm_custom_bkt_attr_t;

static _alpm_custom_bkt_attr_t th3_custom_bkt_attr = {
    .l2_phy_bnks = 5,
    .l2_phy_bkts = 1024,
    .l2_glb_bnks = 2,
    .l2_prt_bnks = 3,
    .l3_phy_bnks = 8,
    .l3_phy_bkts = 8192,
    .l3_glb_bnks = 4,
    .l3_prt_bnks = 4,
};

static int
th3_cust_bkt_attr_load(int u, _alpm_custom_bkt_attr_t *cattr)
{
    int rv = BCM_E_NONE;
    cattr->l2_phy_bnks = soc_property_get(u, "l3_alpm2_th3_l2_phy_bnks", cattr->l2_phy_bnks);
    cattr->l2_phy_bkts = soc_property_get(u, "l3_alpm2_th3_l2_phy_bkts", cattr->l2_phy_bkts);
    cattr->l2_glb_bnks = cattr->l2_phy_bnks / 2;
    cattr->l2_prt_bnks = cattr->l2_phy_bnks - cattr->l2_glb_bnks;

    cattr->l3_phy_bnks = soc_property_get(u, "l3_alpm2_th3_l3_phy_bnks", cattr->l3_phy_bnks);
    cattr->l3_phy_bkts = soc_property_get(u, "l3_alpm2_th3_l3_phy_bkts", cattr->l3_phy_bkts);
    cattr->l3_glb_bnks = cattr->l3_phy_bnks / 2;
    cattr->l3_prt_bnks = cattr->l3_phy_bnks - cattr->l3_glb_bnks;

    /* Validate */
    if (cattr->l2_phy_bkts % 1024 != 0 || cattr->l2_phy_bkts <= 0) {
        cli_out("Invalid physical bucket count %d for Level 2\n", cattr->l2_phy_bkts);
        rv = BCM_E_CONFIG;
    }
    if (cattr->l3_phy_bkts % 1024 != 0 || cattr->l3_phy_bkts <= 0) {
        cli_out("Invalid physical bucket count %d for Level 3\n", cattr->l3_phy_bkts);
        rv = BCM_E_CONFIG;
    }
    if (cattr->l2_phy_bnks < 2 || cattr->l2_phy_bnks > 10) {
        cli_out("Invalid physical bank count %d for Level 2\n", cattr->l2_phy_bkts);
        rv = BCM_E_CONFIG;
    }
    if (cattr->l3_phy_bnks < 2 || cattr->l3_phy_bnks > 10) {
        cli_out("Invalid physical bank count %d for Level 3\n", cattr->l3_phy_bnks);
        rv = BCM_E_CONFIG;
    }

    return rv;
}

static int
th3_pool_conf_update(int u, _alpm_bkt_pool_conf_t *pconf, _alpm_custom_bkt_attr_t *cattr)
{
    int ori_bnk_per_bkt = BPC_BNK_PER_BKT(pconf);
    int ori_bkt_cnt     = BPC_BNK_CNT(pconf);

    switch (BPC_BKT_TBL(pconf)) {
        case alpmTblBktCmodeL2:
            BPC_BNK_PER_BKT(pconf) = cattr->l2_phy_bnks;
            break;
        case alpmTblBktPmodeGlbL2:
            BPC_BNK_PER_BKT(pconf) = cattr->l2_glb_bnks;
            break;
        case alpmTblBktPmodePrtL2:
            BPC_BNK_PER_BKT(pconf) = cattr->l2_prt_bnks;
            break;
        case alpmTblBktCmodeL3:
            BPC_BNK_PER_BKT(pconf) = cattr->l3_phy_bnks;
            break;
        case alpmTblBktPmodeGlbL3:
            BPC_BNK_PER_BKT(pconf) = cattr->l3_glb_bnks;
            break;
        case alpmTblBktPmodePrtL3:
            BPC_BNK_PER_BKT(pconf) = cattr->l3_prt_bnks;
            break;
        default:
            assert(0);
            break;
    }

    if (BPC_IS_LEVEL3_TBL(pconf)) {
        BPC_BNK_CNT(pconf) = BPC_BNK_PER_BKT(pconf) * cattr->l3_phy_bkts;
    } else {
        BPC_BNK_CNT(pconf) = BPC_BNK_PER_BKT(pconf) * cattr->l2_phy_bkts;
    }

    if (ori_bnk_per_bkt != BPC_BNK_PER_BKT(pconf)) {
        cli_out("CUSTOM CONFIG: %s %18s \t %d -> %d\n",
                BPC_BKT_TBL(pconf) < alpmTblBktCmodeL3 ? "Level 2" : "Level 3",
                "bnk_per_bkt", ori_bnk_per_bkt, BPC_BNK_PER_BKT(pconf));
    }

    if (ori_bkt_cnt != BPC_BNK_CNT(pconf)) {
        cli_out("CUSTOM CONFIG: %s %18s \t %d -> %d\n",
                BPC_BKT_TBL(pconf) < alpmTblBktCmodeL3 ? "Level 2" : "Level 3",
                "bkt_total", ori_bkt_cnt, BPC_BNK_CNT(pconf));
    }

    return BCM_E_NONE;
}

static int
th3_bnk_conf_update(int u, int acb_idx, _alpm_bnk_conf_t *bconf, _alpm_custom_bkt_attr_t *cattr)
{
    int i;
    int16 ori_bkt_bits = bconf->bkt_bits;
    int16 ori_phy_bnks = bconf->phy_bnks_per_bkt;
    int16 ori_bnk_bits = bconf->bnk_bits;

    int16 phy_bkts = 0;

    if (acb_idx == 0) {
        /* Level 2 */
        bconf->phy_bnks_per_bkt = (int16)cattr->l2_phy_bnks;
        phy_bkts = (int16)cattr->l2_phy_bkts;
    } else if (acb_idx == 1) {
        /* Level 3 */
        bconf->phy_bnks_per_bkt = (int16)cattr->l3_phy_bnks;
        phy_bkts = (int16)cattr->l3_phy_bkts;
    }

    for (i = 1; i < 16; i++) {
        if ((1 << i) >= phy_bkts) {
            break;
        }
    }
    bconf->bkt_bits = (int16)i;

    for (i = 1; i < 16; i++) {
        if ((1 << i) >= bconf->phy_bnks_per_bkt) {
            break;
        }
    }

    bconf->bnk_bits = (int16)i;

    if (ori_bkt_bits != bconf->bkt_bits) {
        cli_out("CUSTOM CONFIG: %s %18s \t %d -> %d\n",
                acb_idx == 0 ? "Level 2" : "Level 3", "bkt_bits", ori_bkt_bits,
                bconf->bkt_bits);
    }

    if (ori_phy_bnks != bconf->phy_bnks_per_bkt) {
        cli_out("CUSTOM CONFIG: %s %18s \t %d -> %d\n",
                acb_idx == 0 ? "Level 2" : "Level 3", "phy_bnks_per_bkt", ori_phy_bnks,
                bconf->phy_bnks_per_bkt);
    }

    if (ori_bnk_bits != bconf->bnk_bits) {
        cli_out("CUSTOM CONFIG: %s %18s \t %d -> %d\n",
                acb_idx == 0 ? "Level 2" : "Level 3", "bnk_bits", ori_bnk_bits,
                bconf->bnk_bits);
    }

    return BCM_E_NONE;
}

static int
th3_mem_phy_index_get(int u, _alpm_cb_t *acb, _alpm_tbl_t tbl, int index)
{
    int phy_index;
    /* Remapping index for actual memory in Parallel/Mixed mode */
    switch (tbl) {
        case alpmTblBktPmodeGlbL2:
        case alpmTblBktPmodeGlbL3:
            phy_index = index + ACB_BNK_PER_BKT(acb, 1) * ACB_BKT_CNT(acb);
            break;
        default:
            phy_index = index;
            break;
    }

    if (ACB_HLF_BNK_MD(acb)) {
        phy_index += (1 << ACB_BNK_BITS(acb)) * ACB_BKT_CNT(acb);
    }

    return phy_index;
}

int
th3_mem_read(int u, _alpm_cb_t *acb, _alpm_tbl_t tbl,
             int index, void *entry_data, int no_cache)
{
    int rv = BCM_E_NONE;
    soc_mem_t mem;

    mem = alpm_tbl[tbl];
    index = th3_mem_phy_index_get(u, acb, tbl, index);
    if (no_cache && !(ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_WRITE_CACHE_ONLY)) {
        rv = soc_mem_read_no_cache(u, 0, mem, 0, MEM_BLOCK_ANY,
                                   index, entry_data);
    } else {
        rv = soc_mem_read(u, mem, MEM_BLOCK_ANY, index, entry_data);
    }
    if (BCM_SUCCESS(rv)) {
        acb->acb_cnt.c_mem[tbl][0]++;
    }

    return rv;
}

int
th3_mem_write(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node,
              _alpm_tbl_t tbl, int index, void *entry_data,
              int update_data)
{
    int rv = BCM_E_NONE;
    int phy_index;
    soc_mem_t mem;
    void *entry = entry_data;

    mem = alpm_tbl[tbl];
    if (entry == NULL) {
        entry = soc_mem_entry_null(u, mem);
    }

    phy_index = th3_mem_phy_index_get(u, acb, tbl, index);
    if (ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_WRITE_CACHE_ONLY) {
        _soc_mem_alpm_write_cache(u, mem, MEM_BLOCK_ALL, phy_index, entry);
    } else {
        rv = soc_mem_write(u, mem, MEM_BLOCK_ALL, phy_index, entry);

#ifdef MIS_FORWARDING_CHECK
        (void)th3_mem_write_check(u, mem, phy_index, entry);
#endif
    }

    if (BCM_SUCCESS(rv)) {
        acb->acb_cnt.c_mem[tbl][1]++;
        if (pvt_node != NULL) {
            if (entry_data != NULL) {
                ACB_PVT_PTR(acb, pvt_node, index) = pvt_node;
            } else {
                ACB_PVT_PTR(acb, pvt_node, index) = NULL;
            }
        }
     }

    return rv;
}

int
th3_mem_bulk_write(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node,
                   _alpm_tbl_t tbl, int *index, uint32 **entry_data, int count)
{
    int i;
    int phy_index[20];  /* 20 = Local max count */
    int copyno[20];
    soc_mem_t mem;
    soc_mem_t bulk_mem[20];
    int rv = BCM_E_NONE;

    assert(count <= 20);

    mem = alpm_tbl[tbl];
    for (i = 0; i < count; i++) {
        phy_index[i] = th3_mem_phy_index_get(u, acb, tbl, index[i]);
        copyno[i] = MEM_BLOCK_ALL;
        bulk_mem[i] = mem;
    }

    if (ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_WRITE_CACHE_ONLY) {
        for (i = 0; i < count; i++) {
            _soc_mem_alpm_write_cache(u, mem, MEM_BLOCK_ALL, phy_index[i],
                                      entry_data[i]);
        }
    } else {
        if (count <= ALPMC(u)->_alpm_bulk_wr_threshold ||
            SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM) {
            for (i = 0; i < count; i++) {
                rv = soc_mem_write(u, mem, MEM_BLOCK_ALL, phy_index[i],
                                   entry_data[i]);

#ifdef MIS_FORWARDING_CHECK
                (void)th3_mem_write_check(u, mem, phy_index[i], entry_data[i]);
#endif
                ALPM_IER(rv);
            }
        } else {
            rv = soc_mem_bulk_write(u, bulk_mem, phy_index, copyno,
                                    entry_data, count);
        }
    }

    if (BCM_SUCCESS(rv)) {
        for (i = 0; i < count; i++) {
            acb->acb_cnt.c_mem[tbl][1]++;
            if (pvt_node != NULL) {
                ACB_PVT_PTR(acb, pvt_node, index[i]) = pvt_node;
            }
        }
    }

    return rv;
}

/* ALPM related functions */

static int
th3_alpm_cb_cnt(int u)
{
    int acb_cnt, alpm_enable, alpm_level;
    alpm_enable = soc_property_get(u, spn_L3_ALPM_ENABLE, 2);
    alpm_level  = soc_property_get(u, "l3_alpm2_level", 3);

    if (alpm_enable) {
        acb_cnt = alpm_level - 1;      /* 1 or 2 ALPM Control Blocks */
        if (acb_cnt < 1) {
            acb_cnt = ALPM_CB_CNT;
        }
    } else {
        acb_cnt = 0;                   /* 0 ALPM Control Block */
    }

    if (acb_cnt > ALPM_CB_CNT) {
        acb_cnt = ALPM_CB_CNT;
    }

    return acb_cnt;
}

void
th3_alpm_ctrl_deinit(int u)
{
    int i, j, pid, ipt, acb_cnt;
    _alpm_cb_t *acb;
    int app, app_cnt = alpm_app_cnt(u);

    acb_cnt = th3_alpm_cb_cnt(u);
    for (app = 0; app < app_cnt; app++) {
        for (i = 0; i < acb_cnt; i++) {
            acb = ACB(u, i, app);
            if (acb != NULL) {
                /* Free Bucket pool */
                for (pid = 0; pid < ALPM_BKT_PID_CNT; pid++) {
                    _alpm_bkt_pool_conf_t *bp_conf;

                    bp_conf = ACB_BKT_POOL(acb, pid);
                    if (bp_conf->pvt_ptr != NULL) {
                        alpm_util_free(bp_conf->pvt_ptr);
                        bp_conf->pvt_ptr = NULL;
                    }

                    for (ipt = ALPM_IPT_V4; ipt < ALPM_IPT_CNT; ipt++) {
                        if (ipt > ALPM_IPT_V4 &&
                            BPC_BNK_INFO(bp_conf, ipt) ==
                            BPC_BNK_INFO(bp_conf, ipt - 1)) {
                            continue;
                        }
                        if (BPC_BNK_INFO(bp_conf, ipt) != NULL) {
                            alpm_util_free(BPC_BNK_INFO(bp_conf, ipt));
                        }
                    }
                    for (ipt = ALPM_IPT_V4; ipt < ALPM_IPT_CNT; ipt++) {
                        BPC_BNK_INFO(bp_conf, ipt) = NULL;
                    }
                }

                for (j = 0; j < ALPM_IPT_CNT; j++) {
                    if (acb->pvt_ctl[j] != NULL) {
                        alpm_util_free(acb->pvt_ctl[j]);
                        acb->pvt_ctl[j] = NULL;
                    }
                }

                alpm_util_free(acb);
                ACB(u, i, app) = NULL;
            }
        }
    }

    alpm_util_free(th3_alpm_pool0_l2[u]);
    th3_alpm_pool0_l2[u] = NULL;
    alpm_util_free(th3_alpm_pool1_l2[u]);
    th3_alpm_pool1_l2[u] = NULL;
    alpm_util_free(th3_alpm_pool0_l3[u]);
    th3_alpm_pool0_l3[u] = NULL;
    alpm_util_free(th3_alpm_pool1_l3[u]);
    th3_alpm_pool1_l3[u] = NULL;

    return ;
}

/* Adjust config based on dynamic configuration */
/*
 *     Matrix of fmt_info usage:
 * ---------------------------------------------------------------
 * | Lvl_cnt | Banks | ALPM_mode | DB_type | Cur_lvl | fmt_type |
 * ---------------------------------------------------------------
 * | 2       | 5     | Combined  | DB0(P,G)| 2       | RTE      |
 * | 2       | 5     | Parallel  | DB0(P)  | 2       | RTE      |
 * | 2       | 5     | Parallel  | DB1(G)  | 2       | RTE      |
 * | 2       | 5     | Mixed     | DB0(G)  | 2       | RTE      |
 * | 2       | 5     | Mixed     | DB1(G)  | N/A     | N/A      |
 * ---------------------------------------------------------------
 * | 3       | 4/8   | Combined  | DB0(P,G)| 2       | PVT      |
 * | 3       | 4/8   | Combined  | DB0(P,G)| 3       | RTE      |
 * | 3       | 4     | Parallel  | DB0(P)  | 2       | PVT      |
 * | 3       | 4     | Parallel  | DB0(P)  | 3       | RTE      |
 * | 3       | 4     | Parallel  | DB1(G)  | 2       | RTE*     |
 * | 3       | 8     | Parallel  | DB0(P)  | 2       | PVT      |
 * | 3       | 8     | Parallel  | DB0(P)  | 3       | RTE      |
 * | 3       | 8     | Parallel  | DB1(G)  | 2       | PVT      |
 * | 3       | 8     | Parallel  | DB1(G)  | 3       | RTE      |
 * | 3       | 4/8   | Mixed     | DB0(P)  | 2       | PVT      |
 * | 3       | 4/8   | Mixed     | DB0(P)  | 3       | RTE      |
 * | 3       | 4/8   | Mixed     | DB1(G)  | N/A     | N/A      |
 * ---------------------------------------------------------------
 *
 * if (Cur_lvl == Lvl_cnt) {
 *     fmt_type = RTE;
 * } else {
 *     if (levels(DB) == (Lvl_cnt - 1)) {
 *         e.g.: (Banks==4 && Parallel && DB1)
 *         fmt_type = RTE;
 *     } else {
 *         fmt_type = PVT;
 *     }
 * }
 */
static int
th3_alpm_cfg_adjust(int u, int acb_cnt, _alpm_cb_t *acb)
{
    int i, half_banks = 0;
    int vrf_id = ALPM_VRF_ID_GLO(u);

    if (soc_property_get(u, "l3_alpm2_th3_use_fmt13", 0)) {
        if (th3_fmt_info[ALPM_FMT_RTE].fmt_bnk[1] == INVALIDfmt) {
            th3_fmt_info[ALPM_FMT_RTE].fmt_bnk[1] = L3_DEFIP_ALPM_ROUTE_FMT13_FULLfmt;
        }
    }

    if (soc_th3_get_alpm_banks(u) == 4) {
        half_banks = 1;
        ALPMC(u)->_alpm_half_banks = 1;
    }

    if (acb_cnt > 1 && acb->acb_idx > 0) {
        /* Half banks mode */
        if (half_banks) {
            int pid;
            /* Adjust _alpm_cb_t */
            ACB_PHY_BNK_PER_BKT(acb) >>= 1;
            ACB_BNK_BITS(acb) -= 1;

            /* Adjust _alpm_bkt_pool_conf_t */
            /* For Parallel mode, Global Low routes go to Level 2 only */
            if (ALPM_MODE_CHK(u, BCM_ALPM_MODE_PARALLEL)) {
                _alpm_bkt_pool_conf_t *bp_conf;
                bp_conf = ACB_BKT_VRF_POOL(acb, ALPM_VRF_ID_GLO(u));
                BPC_BNK_CNT(bp_conf) = 0;
                BPC_BNK_PER_BKT(bp_conf) = 1;
            } else {
                for (pid = 0; pid < ALPM_BKT_PID_CNT; pid++) {
                    _alpm_bkt_pool_conf_t *bp_conf;
                    bp_conf = ACB_BKT_POOL(acb, pid);

                    if (pid > 0 && bp_conf == ACB_BKT_POOL(acb, pid-1)) {
                        continue;
                    }
                    BPC_BNK_CNT(bp_conf) >>= 1;
                    BPC_BNK_PER_BKT(bp_conf) >>= 1;
                }
            }
        }
    }

    /* Adjust fmt_info */
    if (ACB_IDX(acb) == acb_cnt - 1) {
        for (i = 0; i < ALPM_BKT_PID_CNT; i++) {
            ACB_BKT_POOL(acb, i)->fmt_info = &th3_fmt_info[ALPM_FMT_RTE];
            if (acb_cnt < 2) {
                ALPMC(u)->_alpm_btm_cb_idx[i] = acb_cnt - 1;
            }
        }
        if (ALPM_GLO_VRF_IN_HLF_PMODE(u, vrf_id)) {
            ACB_VRF_POOL_FMT_INFO(acb, vrf_id) =
                &th3_fmt_info[ALPM_FMT_CNT];
        }
    } else {
        /* When reach here: more than 2 acbs, non-last acb */
        for (i = 0; i < ALPM_BKT_PID_CNT; i++) {
            ACB_POOL_FMT_INFO(acb, i) = &th3_fmt_info[ALPM_FMT_PVT];
            ALPMC(u)->_alpm_btm_cb_idx[i] = acb_cnt - 1;
        }
        if (ALPM_GLO_VRF_IN_HLF_PMODE(u, vrf_id)) {
            ACB_VRF_POOL_FMT_INFO(acb, vrf_id) =
                &th3_fmt_info[ALPM_FMT_RTE];
            ALPMC(u)->_alpm_btm_cb_idx[ALPM_VRF_ID_TO_BPC_PID(u, vrf_id, acb->app)] =
                acb_cnt - 2;
        }
    }

    return BCM_E_NONE;
}

static int
th3_alpm_mem_size(int u, soc_mem_t mem)
{
    return soc_mem_index_count(u, mem);
}

static void
th3_alpm_hw_mem_clear(int u, soc_mem_t mem)
{
    uint8 *mbuf;
    int size, offset = 0;

    ALPM_INFO(("ALPM clearing mem %s\n", SOC_MEM_NAME(u, mem)));

    size = SOC_MEM_TABLE_BYTES(u, mem);
    /* skip first 4 shared UFT banks for EM */
    if ((soc_th3_get_alpm_banks(u) == 4) &&
        (mem == L3_DEFIP_ALPM_LEVEL3m ||
         mem == L3_DEFIP_ALPM_LEVEL3_HIT_ONLYm)) {
        offset = (th3_alpm_mem_size(u, mem) >> 1);
        size >>= 1;
    }
    mbuf = soc_cm_salloc(u, size, "alpm_mem");
    if (mbuf != NULL) {
        sal_memset(mbuf, 0, size);
        (void)soc_mem_write_range(u, mem, MEM_BLOCK_ANY,
                                  offset, th3_alpm_mem_size(u, mem) - 1, mbuf);
        soc_cm_sfree(u, mbuf);
    } else {
        ALPM_ERR(("alpm_hw_mem_clear alloc DMA memory failed\n"));
    }

    return ;
}

static int
th3_alpm_hw_init(int u)
{

#define LVLS            3
#define MODES           3
#define PAIRS           5
#define CNTOF(arr) (sizeof(arr)/sizeof(arr[0]))

    int i, mde, lvl, prs, rv = BCM_E_NONE;
    soc_mem_t mem = LPM_IP_CONTROLm;
    lpm_ip_control_entry_t entry;

    /* Field definitions */
    soc_field_t db_sel_f[8] = {
        LEVEL1_DATABASE_SEL_BLOCK_0f,
        LEVEL1_DATABASE_SEL_BLOCK_1f,
        LEVEL1_DATABASE_SEL_BLOCK_2f,
        LEVEL1_DATABASE_SEL_BLOCK_3f,
        LEVEL2_DATABASE_SEL_BLOCK_0f,
        LEVEL2_DATABASE_SEL_BLOCK_1f,
        LEVEL3_DATABASE_SEL_BLOCK_0f,
        LEVEL3_DATABASE_SEL_BLOCK_1f
    };

    soc_field_t ki_sel_f[8] = {
        LEVEL1_KEY_INPUT_SEL_BLOCK_0f,
        LEVEL1_KEY_INPUT_SEL_BLOCK_1f,
        LEVEL1_KEY_INPUT_SEL_BLOCK_2f,
        LEVEL1_KEY_INPUT_SEL_BLOCK_3f,
        LEVEL2_KEY_INPUT_SEL_BLOCK_0f,
        LEVEL2_KEY_INPUT_SEL_BLOCK_1f,
        LEVEL3_KEY_INPUT_SEL_BLOCK_0f,
        LEVEL3_KEY_INPUT_SEL_BLOCK_1f
    };

    soc_field_t bnk_cf_f[4] = {
        LEVEL2_BANK_CONFIG_BLOCK_0f,
        LEVEL2_BANK_CONFIG_BLOCK_1f,
        LEVEL3_BANK_CONFIG_BLOCK_0f,
        LEVEL3_BANK_CONFIG_BLOCK_1f
    };

    soc_field_t db_mod_f[2] = {
        DB0_MODEf,
        DB1_MODEf
    };

    soc_field_t dt_sel_f[2] = {
        DATA0_SELf,
        DATA1_SELf
    };

    /* Value definitions */
    /* DB0: 0, Disable: 4 */
    uint32 db_sel_v[MODES][PAIRS][8] = {
        {   /* Combined */
            {0, 0, 0, 0, 0, 0, 0, 0},   /* 0 block  paired */
            {0, 0, 0, 0, 0, 0, 0, 0},   /* 1 block  paired */
            {0, 0, 0, 0, 0, 0, 0, 0},   /* 2 blocks paired */
            {0, 0, 0, 0, 0, 0, 0, 0},   /* 3 blocks paired */
            {0, 0, 0, 0, 0, 0, 0, 0}    /* 4 blocks paired */
        },
        {   /* Parallel */
            {0, 0, 1, 1, 0, 1, 0, 1},
            {0, 0, 1, 1, 0, 1, 0, 1},
            {0, 0, 1, 1, 0, 1, 0, 1},
            {0, 0, 1, 1, 0, 1, 0, 1},
            {0, 0, 1, 1, 0, 1, 0, 1}
        },
        {   /* Mixed */
            {0, 0, 1, 1, 0, 0, 0, 0},
            {0, 0, 1, 1, 0, 0, 0, 0},
            {0, 0, 1, 1, 0, 0, 0, 0},
            {0, 0, 1, 1, 0, 0, 0, 0},
            {0, 0, 1, 1, 0, 0, 0, 0}
        }
    };

    /* IPv6: K0(0), 128bits; K1(1), 64bits */
    uint32 ki_sel_v[MODES][PAIRS][8] = {
        {   /* Combined */
            {1, 1, 1, 1, 0, 0, 0, 0},   /* 0 block  paired */
            {0, 1, 1, 1, 0, 0, 0, 0},   /* 1 block  paired */
            {0, 0, 1, 1, 0, 0, 0, 0},   /* 2 blocks paired */
            {0, 0, 0, 1, 0, 0, 0, 0},   /* 3 blocks paired */
            {0, 0, 0, 0, 0, 0, 0, 0}    /* 4 blocks paired */
        },
        {   /* Parallel */
            {1, 1, 1, 1, 0, 0, 0, 0},   /* 0 block  paired */
            {0, 1, 0, 1, 0, 0, 0, 0},   /* 1 block  paired (invalid, use 2 blk paired setting) */
            {0, 1, 0, 1, 0, 0, 0, 0},   /* 2 block  paired */
            {0, 0, 0, 0, 0, 0, 0, 0},   /* 3 block  paired (invalid, use 4 blk paired setting) */
            {0, 0, 0, 0, 0, 0, 0, 0}    /* 4 block  paired */
        },
        {   /* Mixed */
            {1, 1, 1, 1, 0, 0, 0, 0},   /* 0 block  paired */
            {0, 1, 0, 1, 0, 0, 0, 0},   /* 1 block  paired (invalid, use 2 blk paired setting) */
            {0, 1, 0, 1, 0, 0, 0, 0},   /* 2 block  paired */
            {0, 0, 0, 0, 0, 0, 0, 0},   /* 3 block  paired (invalid, use 4 blk paired setting) */
            {0, 0, 0, 0, 0, 0, 0, 0}    /* 4 block  paired */
        }
    };

    /* Bank Config: bitmap */
    uint32 bnk_cf_v[MODES][LVLS][4] = {
        {   /* Combined */
            {0, 0, 0, 0},             /* 1Level */
            {0x7, 0x18, 0, 0},        /* 2Level */
            {0x7, 0x18, 0xf, 0xf0}    /* 3Level */
        },
        {   /* Parallel */
            {0, 0, 0, 0},
            {0x7, 0x18, 0, 0},
            {0x7, 0x18, 0xf, 0xf0}
        },
        {   /* Mixed */
            {0, 0, 0, 0},
            {0x7, 0x18, 0, 0},
            {0x7, 0x18, 0xf, 0xf0}
        }
    };

    /* DB_MODE: 1Level(1), 2Level(2), 3Level(3), Disable(0) */
    uint32 db_mod_v[MODES][LVLS][2] = {
        {   /* Combined */
            {1, 0},                     /* 1Level */
            {2, 0},                     /* 2Level */
            {3, 0}                      /* 3Level */
        },
        {   /* Parallel */
            {1, 1},
            {2, 2},
            {3, 3}
        },
        {   /* Mixed */
            {1, 1},
            {2, 1},
            {3, 1}
        }
    };

    /* DATA_SEL: DB0(0), DB1(1), Disable(4) */
    uint32 dt_sel_v[MODES][2] = {
        {0, 4},
        {0, 1},
        {0, 1}
    };

    if (!soc_mem_is_valid(u, LPM_IP_CONTROLm)) {
        return rv;
    }

    mde = ALPM_MODE(u);
    lvl = ACB_CNT(u);
    prs = ALPM_TCAM_PAIR_BLK_CNT(u);

    sal_memset(&entry, 0, sizeof(entry));

    /* Adjust DATABASE_SEL values based on Level count */
    for (i = 2; i > lvl; i--) {
        db_sel_v[mde][prs][2*i+2] = 4; /* 4: Disable */
        db_sel_v[mde][prs][2*i+3] = 4;
    }

    if (soc_th3_get_alpm_banks(u) == 4) {
        /* Set LEVEL3_DATABASE_SEL_BLOCK_0 to 4: Disable in half banks mode */
        db_sel_v[mde][prs][6] = 4;
        db_sel_v[mde][prs][7] = 0;
        /* Set LEVEL3_BANK_CONFIG_BLOCK_0 to 0: UFT banks 0-3 for EM */
        bnk_cf_v[mde][2][2] = 0;
        if (ALPM_MODE_CHK(u, BCM_ALPM_MODE_PARALLEL) && (lvl == 2)) {
            db_mod_v[mde][lvl][1] = 2; /* Public DB in Parallel mode goes to Level2 only */
        }
    }

    /* DATABASE_SEL fields */
    for (i = 0; i < CNTOF(db_sel_f); i++) {
        soc_mem_field32_set(u, mem, &entry, db_sel_f[i], db_sel_v[mde][prs][i]);
    }

    /* KEY_INPUT_SEL fields */
    for (i = 0; i < CNTOF(ki_sel_f); i++) {
        soc_mem_field32_set(u, mem, &entry, ki_sel_f[i], ki_sel_v[mde][prs][i]);
    }

    /* BANK_CONFIG fields */
    for (i = 0; i < CNTOF(bnk_cf_f); i++) {
        soc_mem_field32_set(u, mem, &entry, bnk_cf_f[i], bnk_cf_v[mde][lvl][i]);
    }

    /* DB_MODE fields */
    for (i = 0; i < CNTOF(db_mod_f); i++) {
        soc_mem_field32_set(u, mem, &entry, db_mod_f[i], db_mod_v[mde][lvl][i]);
    }

    /* DATA_SEL fields */
    for (i = 0; i < CNTOF(dt_sel_f); i++) {
        soc_mem_field32_set(u, mem, &entry, dt_sel_f[i], dt_sel_v[mde][i]);
    }

    soc_mem_write(u, mem, COPYNO_ALL, 0, &entry);

    return rv;
}

/* Initialize pivot and bkt bank mem and states.
   allocate: TRUE when th3_alpm_ctrl_init,
   FALSE when th3_alpm_ctrl_cleanup */
int
th3_alpm_ctrl_mem_init(int u, int app, int allocate)
{
    int rv = BCM_E_NONE;
    int i, j, pid, acb_cnt, vrf_id, vrf_id_cnt;
    uint8 def_mode, db_type;
    int alloc_sz;
    int scale_factor = 1;
    int sf = 0;

    soc_mem_t mem, allmems[20] = {INVALIDm};
    int m, mem_cnt = 0;


    if (SOC_MEM_IS_VALID(u, L3_DEFIP_ALPM_LEVEL2m)) {
        alpm_tbl = &th3_alpm_tbl[0];
    }

    /* Default route data mode = Full (1) */
    def_mode = soc_property_get(u, spn_L3_ALPM2_DEFAULT_ROUTE_DATA_MODE, 1);

    if (allocate) { /* for cleanup only scale_factor=1, sf=0 */
        /* Load scaling factor config
         * 8 means valid factors are 1,2,4,8...128
         */
        scale_factor = soc_property_get(u, "l3_alpm2_scaling_factor", 1);
        for (sf = 0; sf < 8; sf++) {
            if (scale_factor == (1 << sf)) {
                break;
            }
        }
        if (sf >= 8) {
            scale_factor = 1;
            sf = 0;
        }
    }

    vrf_id_cnt = ALPM_VRF_ID_CNT(u);
    acb_cnt = ACB_CNT(u);

    for (i = 0; i < acb_cnt; i++) {
        _alpm_cb_t *acb = ACB(u, i, app);

#define _CHK_IF_CLEAR_MEM(mem)                  \
    do {                                        \
        for (m = 0; m < mem_cnt; m++) {         \
            if (allmems[m] == (mem)) {          \
                break;                          \
            }                                   \
        }                                       \
        if (m == mem_cnt && mem != INVALIDm &&  \
            th3_alpm_mem_size(u, mem) > 0) {  \
            th3_alpm_hw_mem_clear(u, (mem));    \
            allmems[mem_cnt++] = (mem);         \
        }                                       \
    } while (0)

        for (pid = 0; pid < ALPM_BKT_PID_CNT; pid++) {
            _alpm_bkt_pool_conf_t *bp_conf = ACB_BKT_POOL(acb, pid);
            if (pid > 0 && ACB_BKT_POOL(acb, pid-1) == bp_conf) {
                continue;
            }

            /* Adjust bucket count based on scaling factor */
            BPC_BNK_CNT(bp_conf) /= scale_factor;
            alloc_sz = sizeof(_alpm_bnk_info_t);
            ALPM_REALLOC_EG(BPC_BNK_INFO(bp_conf, ALPM_IPT_V4),
                          alloc_sz, "bnk_bmp");
            if (ALPM_128B(u) && ALPM_BKT_RSVD(u)) {
                int rsvd_v6_bnks =
                    BPC_BNK_CNT(bp_conf) * ALPM_BKT_RSVD_CNT(u) /
                    ALPM_BKT_RSVD_TOTAL;
                ALPM_REALLOC_EG(
                    BPC_BNK_INFO(bp_conf, ALPM_IPT_V6), alloc_sz, "bnk_bmp");
                /* reserved v6 bnks in v4 bitmap */
                SHR_BITSET_RANGE(
                    BPC_BNK_BMP(bp_conf, ALPM_IPT_V4),
                    BPC_BNK_CNT(bp_conf) - rsvd_v6_bnks,
                    rsvd_v6_bnks);
                BPC_BNK_RSVD(bp_conf, ALPM_IPT_V4) = rsvd_v6_bnks;
                /* reserved v4 bnks in v6 bitmap */
                SHR_BITSET_RANGE(
                    BPC_BNK_BMP(bp_conf, ALPM_IPT_V6), 0,
                    BPC_BNK_CNT(bp_conf) - rsvd_v6_bnks);
                BPC_BNK_RSVD(bp_conf, ALPM_IPT_V6) =
                    BPC_BNK_CNT(bp_conf) - rsvd_v6_bnks;
            } else {
                BPC_BNK_INFO(bp_conf, ALPM_IPT_V6) =
                    BPC_BNK_INFO(bp_conf, ALPM_IPT_V4);
            }

            if (!SOC_WARM_BOOT(u)) {
                mem = alpm_tbl[BPC_BKT_TBL(bp_conf)];
                _CHK_IF_CLEAR_MEM(mem);
            }

            if (bp_conf->pvt_ptr == NULL) {
                alloc_sz = BPC_BNK_CNT(bp_conf);
                alloc_sz = sizeof(void *) * alloc_sz;
                ALPM_REALLOC_EG(bp_conf->pvt_ptr, alloc_sz, "pvt_ptr");
            }
        }

        /* Adjust bucket count based on scaling factor */
        ALPM_IDX_BNK_SHIFT(acb) -= sf;

        /* Per instance structure init */
        alloc_sz = vrf_id_cnt * sizeof(_alpm_pvt_ctrl_t);
        for (j = 0; j < ALPM_IPT_CNT; j++) {
            ALPM_REALLOC_EG(
                acb->pvt_ctl[j], alloc_sz, "pvt_ctl");
            /* Initialize all VRF db_type with default route data mode */
            for (vrf_id = 0; vrf_id < vrf_id_cnt; vrf_id++) {
                if (ALPM_VRF_ID_HAS_BKT(u, vrf_id)) {
                    db_type = def_mode;
                } else {
                    db_type = 1; /* DIRECT_ROUTE is always Full mode */
                }
                ACB_VRF_DB_TYPE_SET(u, acb, vrf_id, j, db_type);
            }
        }

        for (j = 0; j < ALPM_PKM_CNT; j++) {
            if (!SOC_WARM_BOOT(u)) {
                mem = alpm_tbl[acb->pvt_tbl[j]];
                _CHK_IF_CLEAR_MEM(mem);
            }
        }
    }

bad:
    return rv;
}

int
th3_alpm_ctrl_init(int u)
{
    int rv = BCM_E_NONE;
    int i, j, pid, acb_cnt;
    int alloc_sz;
    int app, app_cnt = alpm_app_cnt(u);

    /*
     *  b2(p1)  b1(p0)  bit0(128b)
     * ---------------------------
     *  0       0       0       -> 2-Level (64b)
     *  0       0       1       -> 2-Level (128b)
     *  0       1       0       -> 3-Level (p0-64b)
     *  0       1       1       -> 3-Level (p0-128b)
     *  1       0       0       -> 3-Level (p1-64b)
     *  1       0       1       -> 3-Level (p1-128b)
     */
#define ACB_TMPL_NUM      6
    _alpm_cb_t *acb_template[ACB_TMPL_NUM] = {
        &th3_alpm_1cb_template,
        &th3_alpm_1cb_template_128b,
        &th3_alpm_2cb_template_p0,
        &th3_alpm_2cb_template_p0_128b,
        &th3_alpm_2cb_template_p1,
        &th3_alpm_2cb_template_p1
    };

#define ACB_BP_CONF_NUM    8
    _alpm_bkt_pool_conf_t *acb_bp_conf[ACB_BP_CONF_NUM];

    _alpm_bkt_pool_conf_t *acb_bp_conf_template[ACB_BP_CONF_NUM] = {
        &th3_alpm_cmode_pool_l2,
        &th3_alpm_cmode_pool_l3,
        &th3_alpm_pmode_pool_prt_l2,
        &th3_alpm_pmode_pool_prt_l3,
        &th3_alpm_cmode_pool_l2,
        &th3_alpm_cmode_pool_l3,
        &th3_alpm_pmode_pool_glb_l2,
        &th3_alpm_pmode_pool_glb_l3
    };

    ALPM_IEG(th3_cust_bkt_attr_load(u, &th3_custom_bkt_attr));

    alloc_sz = sizeof(_alpm_bkt_pool_conf_t);
    ALPM_ALLOC_EG(th3_alpm_pool0_l2[u], alloc_sz, "th3_alpm_pool0_l2");
    ALPM_ALLOC_EG(th3_alpm_pool1_l2[u], alloc_sz, "th3_alpm_pool1_l2");
    ALPM_ALLOC_EG(th3_alpm_pool0_l3[u], alloc_sz, "th3_alpm_pool0_l3");
    ALPM_ALLOC_EG(th3_alpm_pool1_l3[u], alloc_sz, "th3_alpm_pool1_l3");

    acb_bp_conf[0] = th3_alpm_pool0_l2[u];
    acb_bp_conf[1] = th3_alpm_pool0_l3[u];
    acb_bp_conf[2] = th3_alpm_pool0_l2[u];
    acb_bp_conf[3] = th3_alpm_pool0_l3[u];
    acb_bp_conf[4] = th3_alpm_pool0_l2[u];
    acb_bp_conf[5] = th3_alpm_pool0_l3[u];
    acb_bp_conf[6] = th3_alpm_pool1_l2[u];
    acb_bp_conf[7] = th3_alpm_pool1_l3[u];

    /* assert ALPM_MEM_ENT_MAX for largest TCAM/ALPM memory entry */
    assert(ALPM_MEM_ENT_MAX * sizeof(uint32)
           >= sizeof(defip_pair_level1_entry_t));

    if (SOC_MEM_IS_VALID(u, L3_DEFIP_ALPM_LEVEL2m)) {
        alpm_tbl = &th3_alpm_tbl[0];
    }
    acb_cnt = th3_alpm_cb_cnt(u);
    assert((acb_cnt * ALPM_PKM_CNT) < 20);

    if (acb_cnt > 1 && (soc_th3_get_alpm_banks(u) == 0)) {
        ALPM_ERR(("There is no UFT bank available for 3-Level ALPM\n"));
        ALPM_IEG(BCM_E_CONFIG);
    }

    for (app = 0; app < app_cnt; app++) {
        for (i = 0; i < acb_cnt; i++) {
            int idx;
            _alpm_cb_t *acb = NULL;

            alloc_sz = sizeof(_alpm_cb_t);
            ALPM_ALLOC_EG(acb, alloc_sz, "alpm_cb");
            idx = (i + acb_cnt - 1) << 1 | (ALPM_128B(u) & 1);
            sal_memcpy(acb, acb_template[idx], sizeof(_alpm_cb_t));
            (void)th3_bnk_conf_update(u, ACB_IDX(acb), &ACB_BNK_CONF(acb), &th3_custom_bkt_attr);

            acb->unit = u;
            acb->app = app;

            /**
             *  -------
             *  |2|1|0|
             *  -------
             *  Bit 0: acb_idx: acb0, acb1
             *  Bit 1: Parallel: 1, Mixed/Combined: 0
             *  Bit 2: Private(Pid 0)/Global(Pid 1)
             *  ---------
 */
            for (pid = 0; pid < ALPM_BKT_PID_CNT; pid++) {
                idx = pid << 2 | (!!ALPM_MODE_CHK(u, BCM_ALPM_MODE_PARALLEL) << 1) | i;
                ACB_BKT_POOL(acb, pid) = acb_bp_conf[idx];
                sal_memcpy(acb_bp_conf[idx], acb_bp_conf_template[idx],
                           sizeof(_alpm_bkt_pool_conf_t));
                (void)th3_pool_conf_update(u, acb_bp_conf[idx], &th3_custom_bkt_attr);
            }

            /* Adjust ctrl bnk conf
             * BPC should NOT be adjusted because they point to
             * global storage
             */
            ALPM_IEG(th3_alpm_cfg_adjust(u, acb_cnt, acb));

            ACB(u, i, app) = acb;
        }
        ACB_CNT(u) = acb_cnt;

        /* Init pvt/bkt bank mem and ctrl (with allocate=TRUE) */
        ALPM_IEG(th3_alpm_ctrl_mem_init(u, app, TRUE));

        /* Obtain TCAM table skip (invalid or duplicated) per */
        for (i = 0; i < ALPM_PKM_CNT; i++) {

            if (alpm_tbl[ACB_TOP(u, app)->pvt_tbl[i]] == INVALIDm) {
                ALPM_TCAM_TBL_SKIP(u, i) = TRUE; /* invalid */
                 continue;
            } else {
                ALPM_TCAM_TBL_SKIP(u, i) = FALSE;
            }

            for (j = 0; j < i; j++) {
                if (alpm_tbl[ACB_TOP(u, app)->pvt_tbl[i]] ==
                    alpm_tbl[ACB_TOP(u, app)->pvt_tbl[j]]) {
                    ALPM_TCAM_TBL_SKIP(u, j) = TRUE; /* duplicated */
                    break;
                }
            }
        }
    }

    if (!SOC_WARM_BOOT(u)) {
        rv = th3_alpm_hw_init(u);
    }

    th3_fmt_test(u);
#if 0
    th3_mem_test(u);
#endif

    return rv;

bad:
    th3_alpm_ctrl_deinit(u);
    return rv;
}

/* TH3 ALPM ctrl cleanup (for faster delete_all) */
int
th3_alpm_ctrl_cleanup(int u)
{
    int rv = BCM_E_NONE;
    int app, app_cnt = alpm_app_cnt(u);
    /* Cleanup only bank mem and ctrl state,
       but keep statistics (with allocate=FALSE) */
    for (app = 0; app < app_cnt && rv == BCM_E_NONE; app++) {
        rv = th3_alpm_ctrl_mem_init(u, app, FALSE);
    }
    return rv;
}

/* L3_DEFIP_ALPM_ROUTE_BANK_FMT1fmt
 *   -> ENTRY0f: L3_DEFIP_ALPM_ROUTE_FMT1fmt
 *        -> PREFIXf
 *        -> ASSOC_DATAf: ASSOC_DATA_REDUCEDfmt
 *             -> DEST_TYPEf
 *             -> DESTINATIONf
 *
 *        -> ASSOC_DATAf: ASSOC_DATA_FULLfmt
 *             -> DEST_TYPEf
 *             -> DESTINATIONf
 *             -> PRIf
 *             -> RPEf
 *             -> DST_DISCARDf
 *             -> CLASS_IDf
 *             -> FLEX_CTR_POOL_NUMBERf
 *             -> FLEX_CTR_OFFSET_MODEf
 *             -> FLEX_CTR_BASE_COUNTER_IDXf
 */

int8
th3_alpm_bnk_fmt_get(int u, _alpm_cb_t *acb,
                     _alpm_bkt_info_t *bkt_info, int ent_idx)
{
    int bnk_idx;

    bnk_idx = ALPM_IDX_TO_BNK(acb, ent_idx);

    return bkt_info->bnk_fmt[bnk_idx];

}

static void
th3_alpm_ent_1bit_encoding(int u, uint32 *pfx, int fmt_len, int pfx_len)
{
    SHR_BITSET(pfx, fmt_len - pfx_len);
}

static void
th3_alpm_ent_1bit_decoding(int u, uint32 *pfx, int fmt_len, int *pfx_len)
{
    int bp = 0; /* Bit pos */
#if 0
    for (bp = 0; bp <= fmt_len; bp++) {
        if (SHR_BITGET(pfx, bp)) {
            break;
        }
    }
#else
    static int fbit[16] = {
       -1, /* 0b0000 */
        0, /* 0b0001 */
        1, /* 0b0010 */
        0, /* 0b0011 */
        2, /* 0b0100 */
        0, /* 0b0101 */
        1, /* 0b0110 */
        0, /* 0b0111 */
        3, /* 0b1000 */
        0, /* 0b1001 */
        1, /* 0b1010 */
        0, /* 0b1011 */
        2, /* 0b1100 */
        0, /* 0b1101 */
        1, /* 0b1110 */
        0, /* 0b1111 */
    };

    while (bp < fmt_len) {
        uint32 tmp;
        if (*pfx != 0) {
            tmp = *pfx;
            while (tmp > 0) {
                if (tmp & 0xf) {
                    bp += fbit[tmp & 0xf];
                    break;
                }
                bp += 4;
                tmp = tmp >> 4;
            }
            break;
        } else {
            pfx ++;
            bp += 32;
        }
    }
#endif
    *pfx_len = fmt_len - bp;
}

static void
th3_alpm_ent_assemble(int u, _alpm_cb_t *acb,
                      _alpm_pvt_node_t *pvt_node, int ent_idx,
                      _bcm_defip_cfg_t *lpm_cfg, uint32 *entry, uint32 write_flags)
{
    int b, vrf_id, ipt;
    int ofst, key_len;
    int dst_sbit, src_sbit; /* Source start bit and Dest start bit */
    int fmt_len, cpy_len, max_pfx_len[] = {32, 128};
    int8 fmt_tp;

    uint32 fmt[6] = {0};
    uint32 key[8] = {0};
    uint32 pfx[5] = {0};
    uint32 fent[ALPM_ENT_WORDS_MAX] = {0};
    soc_format_t ent_fmt, bnk_fmt;

    _alpm_bkt_info_t      *bkt_info = NULL;
    _alpm_bkt_pool_conf_t *bp_conf = NULL;

    th3_fmt_ASSOC_DATA_FULL_t       *fmt_adf = NULL;
    th3_fmt_ASSOC_DATA_REDUCED_t    *fmt_adr = NULL;
    th3_fmt_ALPM2_DATA_t            fmt_a2d;

    vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);
    ipt    = ALPM_LPM_IPT(u, lpm_cfg);

    fmt_tp  = th3_alpm_bnk_fmt_get(u, acb, &PVT_BKT_INFO(pvt_node), ent_idx);
    ent_fmt = ACB_FMT_ENT(acb, vrf_id, fmt_tp);
    bnk_fmt = ACB_FMT_BNK(acb, vrf_id, fmt_tp);
    ofst    = ALPM_IDX_TO_ENT(ent_idx);

    if (write_flags != ALPM_PVT_UPDATE_ALL) {
        soc_format_field_get(u, bnk_fmt, entry, th3_alpm_ent_fld[ofst], fent);
    }

    /* Assoc DATA update */
    if (write_flags & ALPM_PVT_UPDATE_ASSOC_DATA) {
        if (ACB_VRF_DB_TYPE(u, acb, vrf_id, ipt)) {
            fmt_adf = (th3_fmt_ASSOC_DATA_FULL_t *)fmt;
            fmt_adf->DEST_TYPE = lpm_cfg->defip_flags & BCM_L3_MULTIPATH ? 1 : 0;
            fmt_adf->DESTINATION = lpm_cfg->defip_ecmp_index;
            fmt_adf->PRI = lpm_cfg->defip_prio;
            fmt_adf->RPE = lpm_cfg->defip_flags & BCM_L3_RPE ? 1 : 0;
            fmt_adf->DST_DISCARD = lpm_cfg->defip_flags & BCM_L3_DST_DISCARD ? 1 : 0;
            fmt_adf->CLASS_ID = lpm_cfg->defip_lookup_class;
            fmt_adf->FLEX_CTR_POOL_NUMBER = lpm_cfg->defip_flex_ctr_pool;
            fmt_adf->FLEX_CTR_OFFSET_MODE = lpm_cfg->defip_flex_ctr_mode;
            fmt_adf->FLEX_CTR_BASE_COUNTER_IDX = lpm_cfg->defip_flex_ctr_base_id;
        } else {
            fmt_adr = (th3_fmt_ASSOC_DATA_REDUCED_t *)fmt;
            fmt_adr->DEST_TYPE = lpm_cfg->defip_flags & BCM_L3_MULTIPATH ? 1 : 0;
            fmt_adr->DESTINATION = lpm_cfg->defip_ecmp_index;
        }
        soc_format_field_set(u, ent_fmt, fent, ASSOC_DATAf, fmt);
    }

    if (write_flags & ALPM_PVT_UPDATE_PREFIX) {
        key_len = PVT_KEY_LEN(pvt_node);
        fmt_len = ACB_FMT_PFX_LEN(acb, vrf_id, fmt_tp);
        /*
         * 128B:
         *   ip6[0]            ----          ip6[15]
         *   ffff:ffff:0000:0000:0000:0000:0000:0000
         *   prefix[7] prefix[6] prefix[5] prefix[4] ... prefix[0]
         * 64B:
         *                       ip6[0] ---- ip6[7]
         *                       ffff:ffff:0000:0000
         *   prefix[7] prefix[6] prefix[5] prefix[4] ... prefix[0]
         * 32B:
         *                                 ip_addr
         *                                 c0a80101
         *   prefix[7] prefix[6] prefix[5] prefix[4] ... prefix[0]
         */
        alpm_util_cfg_to_key(u, ipt, lpm_cfg, &key[4]);
        if (max_pfx_len[ipt] < fmt_len) {
            src_sbit = 128;
            cpy_len = max_pfx_len[ipt] - key_len;
            dst_sbit = 1 + fmt_len - cpy_len;
        } else {
            src_sbit = 128 + max_pfx_len[ipt] - key_len - fmt_len;
            cpy_len = fmt_len;
            dst_sbit = 1;
        }
        SHR_BITCOPY_RANGE(pfx, dst_sbit, key, src_sbit, cpy_len);
        /* 1bit Length Encoding */
        th3_alpm_ent_1bit_encoding(u, pfx, fmt_len,
                                   lpm_cfg->defip_sub_len - key_len);
        soc_format_field_set(u, ent_fmt, fent, PREFIXf, pfx);
    }

    if ((write_flags & ALPM_PVT_UPDATE_ALPM_DATA) && ACB_HAS_PVT(acb, vrf_id)) {
        /* Pivot level ALPM data update */
        sal_memset(&fmt_a2d, 0, sizeof(fmt_a2d));
        bkt_info = (_alpm_bkt_info_t *)lpm_cfg->bkt_info;
        if (bkt_info != NULL) {
            int offset = 0;
            _alpm_tbl_t tbl;

            bp_conf = ACB_BKT_VRF_POOL(ACB_DWN(u, acb), PVT_BKT_VRF(pvt_node));
            tbl = BPC_BKT_TBL(bp_conf);
            switch (tbl) {
                case alpmTblBktPmodeGlbL2:
                case alpmTblBktPmodeGlbL3:
                    offset = ACB_BNK_PER_BKT(ACB_DWN(u, acb), 1);
                    break;
                default:
                    break;
            }

            if (ALPM_HLF_BNKS(u)) {
                offset = 4;
            }

            fmt_a2d.KSHIFT = lpm_cfg->bkt_kshift;
            fmt_a2d.ROFS = bkt_info->rofs + offset;
            fmt_a2d.BKT_PTR = bkt_info->bkt_idx;
            if (ALPM_TCAM_ZONED(u, acb->app) && lpm_cfg->default_miss == 1) {
                /* TH3TBD, Parallel & Mixed ALPM need to set DEFAULT_MISS to 1
                 * if assoc_data is NOT set, in case of missing default */
                fmt_a2d.DEFAULT_MISS = 1;
            }
            for (b = 0; b < BPC_BNK_PER_BKT(bp_conf); b++) {
                soc_field_t bnk_fld;
                uint8 bnk_fmt = bkt_info->bnk_fmt[b];
                if (bnk_fmt == 0) {
                    continue;
                }
                bnk_fld = th3_alpm_bnk_fld[b+offset];
                bnk_fmt -= 1; 
                switch (bnk_fld) {
                    case FMT0f:
                        fmt_a2d.FMT0 = bnk_fmt;
                        break;
                    case FMT1f:
                        fmt_a2d.FMT1 = bnk_fmt;
                        break;
                    case FMT2f:
                        fmt_a2d.FMT2 = bnk_fmt;
                        break;
                    case FMT3f:
                        fmt_a2d.FMT3 = bnk_fmt;
                        break;
                    case FMT4f:
                        fmt_a2d.FMT4 = bnk_fmt;
                        break;
                    case FMT5f:
                        fmt_a2d.FMT5 = bnk_fmt;
                        break;
                    case FMT6f:
                        fmt_a2d.FMT6 = bnk_fmt;
                        break;
                    case FMT7f:
                        fmt_a2d.FMT7 = bnk_fmt;
                        break;
                    default:
                        break;
                }
            }
            soc_format_field_set(u, ent_fmt, fent, ALPM2_DATAf, (uint32 *)&fmt_a2d);
        }
    }

    soc_format_field_set(u, bnk_fmt, entry, th3_alpm_ent_fld[ofst], fent);

    return ;

}

int
th3_alpm_ent_pfx_len_get(int u, _alpm_cb_t *acb,
                         _alpm_pvt_node_t *pvt_node, int ent_idx)
{
    int    rv;
    int    ent_id, pfx_len = -1;
    int8   fmt_tp;
    int16  fmt_len;
    uint32 bnkb[17];
    uint32 entb[17] = {0};
    uint32 fldb[17] = {0};
    soc_format_t bnk_fmt, ent_fmt;
    _alpm_bkt_pool_conf_t *bp_conf;

    bp_conf = ACB_BKT_VRF_POOL(acb, PVT_BKT_VRF(pvt_node));
    /* Get Bank data: bnkb */
    rv = th3_mem_read(u, acb, BPC_BKT_TBL(bp_conf),
                      ALPM_TAB_IDX_GET(ent_idx), bnkb, FALSE);
    if (BCM_SUCCESS(rv)) {
        fmt_tp = th3_alpm_bnk_fmt_get(u, acb, &PVT_BKT_INFO(pvt_node), ent_idx);
        bnk_fmt = BPC_FMT_BNK(bp_conf, fmt_tp);
        ent_id  = ALPM_IDX_TO_ENT(ent_idx);

        /* Get Entry data: entb */
        (void)soc_format_field_get(u, bnk_fmt, bnkb,
                                   th3_alpm_ent_fld[ent_id], entb);
        /* Get PREFIXf Field data: tmpf */
        ent_fmt = BPC_FMT_ENT(bp_conf, fmt_tp);
        (void)soc_format_field_get(u, ent_fmt, entb, PREFIXf, fldb);

        /* 1bit Length Decoding */
        fmt_len = BPC_FMT_PFX_LEN(bp_conf, fmt_tp);
        th3_alpm_ent_1bit_decoding(u, fldb, fmt_len, &pfx_len);
    }

    return pfx_len;
}

void
th3_alpm_ent_copy(int u, int vrf_id, _alpm_cb_t *acb, uint32 *sb_ent, uint32 *db_ent,
                  _alpm_bkt_info_t *sbinfo, _alpm_bkt_info_t *dbinfo,
                  uint32 src_pvt_len, uint32 dst_pvt_len,
                  uint32 src_idx, uint32 dst_idx,
                  int ipt, _alpm_bkt_node_t *src_pfx)
{
    int src_pfx_len, dst_pfx_len;

    int8 src_fid, dst_fid;
    int16 src_fmt_len, dst_fmt_len;
    uint32 src_eid, dst_eid;
    uint32 se_ent[17] = {0};
    uint32 de_ent[17] = {0};

    uint32 tmpf[17] = {0};
    uint32 dstf[17] = {0};

    soc_format_t sb_fmt, db_fmt;
    soc_format_t se_fmt, de_fmt;

    /* Retrieve src entry */
    src_fid = th3_alpm_bnk_fmt_get(u, acb, sbinfo, src_idx);
    src_eid = ALPM_IDX_TO_ENT(src_idx);
    sb_fmt  = ACB_FMT_BNK(acb, vrf_id, src_fid);
    se_fmt  = ACB_FMT_ENT(acb, vrf_id, src_fid);
    soc_format_field_get(u, sb_fmt, sb_ent, th3_alpm_ent_fld[src_eid], se_ent);

    dst_fid = th3_alpm_bnk_fmt_get(u, acb, dbinfo, dst_idx);
    dst_eid = ALPM_IDX_TO_ENT(dst_idx);
    db_fmt  = ACB_FMT_BNK(acb, vrf_id, dst_fid);
    de_fmt  = ACB_FMT_ENT(acb, vrf_id, dst_fid);

    /* Copy ASSOC_DATA and ALPM2_DATA field */
    if (sb_fmt != db_fmt) {
        sal_memset(tmpf, 0, sizeof(tmpf));
        soc_format_field_get(u, se_fmt, se_ent, ASSOC_DATAf, tmpf);
        soc_format_field_set(u, de_fmt, de_ent, ASSOC_DATAf, tmpf);
        if (ACB_HAS_PVT(acb, vrf_id)) {
            sal_memset(tmpf, 0, sizeof(tmpf));
            soc_format_field_get(u, se_fmt, se_ent, ALPM2_DATAf, tmpf);
            soc_format_field_set(u, de_fmt, de_ent, ALPM2_DATAf, tmpf);
        }
    } else {
        sal_memcpy(de_ent, se_ent, sizeof(de_ent));
    }

    /* Copy PREFIX field */
    sal_memset(tmpf, 0, sizeof(tmpf));
    soc_format_field_get(u, se_fmt, se_ent, PREFIXf, tmpf);
    src_fmt_len = ACB_FMT_PFX_LEN(acb, vrf_id, src_fid);
    dst_fmt_len = ACB_FMT_PFX_LEN(acb, vrf_id, dst_fid);

    /* 1bit Length Decoding */
    th3_alpm_ent_1bit_decoding(u, tmpf, src_fmt_len, &src_pfx_len);
    dst_pfx_len = src_pfx_len - (dst_pvt_len - src_pvt_len);
    if (dst_pfx_len > dst_fmt_len) {
        assert(0);
    } else {
        if (dst_pvt_len >= src_pvt_len) { /* from parent to child pvt */
            /* Copy only prefix part, src/dst offsets need jump
               after 1bit encoding and do 1bit encoding after copy */
            SHR_BITCOPY_RANGE(dstf, dst_fmt_len - dst_pfx_len + 1,
                              tmpf, src_fmt_len - src_pfx_len + 1, dst_pfx_len);
        } else { /* from child to parent pvt: dst_pfx_len > src_pfx_len */
            /* Need encode directly src_pfx into dst_pfx */
            int dst_sbit, src_sbit, cpy_len;
            int max_pfx_len[] = {32, 128};
            uint32 key[9] = {0}; /* need key[5 + 4] */

            alpm_trie_pfx_to_key(u, ipt, src_pfx->key, src_pfx->key_len, &key[4]);

            if (max_pfx_len[ipt] < dst_fmt_len) {
                src_sbit = 128;
                cpy_len = max_pfx_len[ipt] - dst_pvt_len;
                dst_sbit = 1 + dst_fmt_len - cpy_len;
            } else {
                src_sbit = 128 + max_pfx_len[ipt] - dst_pvt_len - dst_fmt_len;
                cpy_len = dst_fmt_len;
                dst_sbit = 1;
            }
            SHR_BITCOPY_RANGE(dstf, dst_sbit, key, src_sbit, cpy_len);
        }
    }

    /* 1bit Length Encoding */
    th3_alpm_ent_1bit_encoding(u, dstf, dst_fmt_len, dst_pfx_len);

    soc_format_field_set(u, de_fmt, de_ent, PREFIXf, dstf);

    /* Write entry back to bank */
    soc_format_field_set(u, db_fmt, db_ent, th3_alpm_ent_fld[dst_eid], de_ent);

    return ;
}

int
th3_alpm_bkt_bnk_write(int u, _alpm_cb_t *acb,
                       _alpm_pvt_node_t *pvt_node,
                       _bcm_defip_cfg_t *lpm_cfg, int ent_idx, uint32 write_flags)
{
    int rv = BCM_E_NONE, index, vrf_id;
    uint32 entry[ALPM_ENT_WORDS_MAX];
    _alpm_tbl_t tbl;
    uint32 vet_bmp;

    vrf_id = PVT_BKT_VRF(pvt_node);
    index  = ALPM_TAB_IDX_GET(ent_idx);
    tbl    = BPC_BKT_TBL(ACB_BKT_VRF_POOL(acb, vrf_id));

    vet_bmp = PVT_BKT_INFO(pvt_node).vet_bmp[ALPM_IDX_TO_BNK(acb, ent_idx)];

    if (vet_bmp & ~(1 << ALPM_IDX_TO_ENT(ent_idx))) {
        rv = th3_mem_read(u, acb, tbl, index, entry, FALSE);
    } else {
        /* If it was empty bank then there is no need to do read */
        sal_memset(entry, 0, sizeof(entry));
    }

    if (BCM_SUCCESS(rv)) {
        th3_alpm_ent_assemble(u, acb, pvt_node, ent_idx, lpm_cfg, entry, write_flags);
        rv = th3_mem_write(u, acb, pvt_node, tbl, index, entry, FALSE);

        /* write route HITBIT to cache & Hw */
        if (!ALPM_HIT_SKIP(u)) {
            if (BCM_SUCCESS(rv) && ACB_HAS_RTE(acb, vrf_id)) {
                th3_alpm_bkt_hit_write(u, PVT_BKT_VRF(pvt_node), acb, tbl, ent_idx,
                                 (lpm_cfg->defip_flags & BCM_L3_HIT ? 1 : 0));
            }
        }
    }

    return rv;
}

static int
th3_alpm_bkt_bnk_shrink(int u, _alpm_cb_t *acb,
                        _alpm_pvt_node_t *pvt_node, uint8 sbnk,
                        uint8 dbnk, _alpm_bkt_pfx_arr_t *pfx_arr)
{
    int i, j, k, rv = 0;
    int sepb, depb;
    int vrf_id;
    int8 src_fid, dst_fid;

    soc_format_t sb_fmt, db_fmt;

    uint32 tmp_idx, stab_idx, dtab_idx;
    _alpm_bkt_info_t *bkt_info = NULL;

    uint32 srcbnke[17];
    uint32 nullent[17];
    uint32 dstbnke[17];
    uint32 tempent[17];

    /* Bulk Write */
    int bulk_idx[2];
    uint32 *bulk_ent[2];
    int bulk_cnt;

    _alpm_bkt_pool_conf_t *bp_conf;

    vrf_id  = PVT_BKT_VRF(pvt_node);
    bp_conf = ACB_BKT_VRF_POOL(acb, vrf_id);

    bkt_info = &PVT_BKT_INFO(pvt_node);
    src_fid  = bkt_info->bnk_fmt[sbnk];
    dst_fid  = bkt_info->bnk_fmt[dbnk];
    sepb     = ACB_FMT_ENT_MAX(acb, vrf_id, src_fid);
    depb     = ACB_FMT_ENT_MAX(acb, vrf_id, dst_fid);

    if (bkt_info->vet_bmp[dbnk] == 0) {
        /* Copy from entire from_bnk to to_bnk and don't invalidate
           from_bnk until upper level pivot changed. Invalidate
           from_bnk is optional after pivot changed. */
        stab_idx = ALPM_TAB_IDX_GET_BI_BNK(acb, bkt_info, sbnk);
        dtab_idx = ALPM_TAB_IDX_GET_BI_BNK(acb, bkt_info, dbnk);

        rv = th3_mem_read(u, acb, BPC_BKT_TBL(bp_conf),
                          stab_idx, srcbnke, FALSE);
        ALPM_IER(rv);

        rv = th3_mem_write(u, acb, pvt_node, BPC_BKT_TBL(bp_conf),
                           dtab_idx, srcbnke, FALSE);

        ALPM_IER(rv);

        bkt_info->vet_bmp[dbnk] = bkt_info->vet_bmp[sbnk];
        bkt_info->vet_bmp[sbnk] = 0;
        bkt_info->bnk_fmt[dbnk] = src_fid;

        /* Update ent_idx of prefix */
        for (i = 0; i < pfx_arr->pfx_cnt; i++) {
            tmp_idx = pfx_arr->pfx[i]->ent_idx;
            if (ALPM_IDX_TO_BNK(acb, tmp_idx) == sbnk) {
                pfx_arr->pfx[i]->ent_idx =
                    ALPM_IDX_MAKE(acb, bkt_info, dbnk,
                    ALPM_IDX_TO_ENT(tmp_idx));
            }
        }

        ALPM_INFO(("**ACB(%d).BNK.MRG: RBBE from(%d %d %d *) to(%d %d %d *)\n",
                   ACB_IDX(acb),
                   PVT_ROFS(pvt_node),
                   PVT_BKT_IDX(pvt_node), sbnk,
                   PVT_ROFS(pvt_node),
                   PVT_BKT_IDX(pvt_node), dbnk));

        return rv;
    }

    /* Merge */
    stab_idx = ALPM_TAB_IDX_GET_BI_BNK(acb, bkt_info, sbnk);
    rv = th3_mem_read(u, acb, BPC_BKT_TBL(bp_conf),
                      stab_idx, srcbnke, FALSE);
    ALPM_IER(rv);
    dtab_idx = ALPM_TAB_IDX_GET_BI_BNK(acb, bkt_info, dbnk);
    rv = th3_mem_read(u, acb, BPC_BKT_TBL(bp_conf),
                      dtab_idx, dstbnke, FALSE);
    ALPM_IER(rv);

    sb_fmt = ACB_FMT_BNK(acb, vrf_id, src_fid);
    db_fmt = ACB_FMT_BNK(acb, vrf_id, dst_fid);
    for (i = 0; i < depb; i++) {
        /* to entry i */
        if (bkt_info->vet_bmp[dbnk] & (1 << i)) {
            continue;
        }
        for (j = 0; j < sepb; j++) {
            if (!(bkt_info->vet_bmp[sbnk] & (1 << j))) {
                continue;
            }
            /* from entry j */
            if (sb_fmt != db_fmt) {
                uint32 src_idx, dst_idx;
                src_idx = ALPM_IDX_MAKE(acb, bkt_info, sbnk, j);
                dst_idx = ALPM_IDX_MAKE(acb, bkt_info, dbnk, i);
                th3_alpm_ent_copy(u, vrf_id, acb, srcbnke, dstbnke,
                            &PVT_BKT_INFO(pvt_node),
                            &PVT_BKT_INFO(pvt_node),
                            PVT_KEY_LEN(pvt_node),
                            PVT_KEY_LEN(pvt_node),
                            src_idx, dst_idx,
                            PVT_BKT_IPT(pvt_node), NULL);
            } else {
                sal_memset(tempent, 0, sizeof(tempent));
                (void)soc_format_field_get(u, sb_fmt, srcbnke,
                                           th3_alpm_ent_fld[j], tempent);
                (void)soc_format_field_set(u, db_fmt, dstbnke,
                                           th3_alpm_ent_fld[i], tempent);
            }
            /* Invalidate entry j */
            sal_memset(nullent, 0, sizeof(nullent));
            (void)soc_format_field_set(u, sb_fmt, srcbnke,
                                       th3_alpm_ent_fld[j], nullent);

            bkt_info->vet_bmp[sbnk] &= ~(1 << j);
            bkt_info->vet_bmp[dbnk] |= (1 << i);

            ALPM_INFO((
                "**ACB(%d).BNK.MRG: RBBE from(%d %d %d %d) to(%d %d %d %d)\n",
                ACB_IDX(acb),
                PVT_ROFS(pvt_node), PVT_BKT_IDX(pvt_node), sbnk, j,
                PVT_ROFS(pvt_node), PVT_BKT_IDX(pvt_node), dbnk, i));

            /* Update ent_idx of prefix */
            for (k = 0; k < pfx_arr->pfx_cnt; k++) {
                tmp_idx = pfx_arr->pfx[k]->ent_idx;
                if (ALPM_IDX_TO_BNK(acb, tmp_idx) == sbnk &&
                    ALPM_IDX_TO_ENT(tmp_idx) == j) {
                    pfx_arr->pfx[k]->ent_idx =
                        ALPM_IDX_MAKE(acb, bkt_info, dbnk, i);
                }
            }
            break;
        }
    }

    bulk_idx[0] = dtab_idx;
    bulk_ent[0] = dstbnke;
    bulk_cnt = 1;
    if (bkt_info->vet_bmp[sbnk]) {
        bulk_idx[1] = stab_idx;
        bulk_ent[1] = srcbnke;
        bulk_cnt ++;
    }
    rv = th3_mem_bulk_write(u, acb, pvt_node, BPC_BKT_TBL(bp_conf),
                            bulk_idx, bulk_ent, bulk_cnt);

    return rv;
}

/* Prefix copy from opvt_node to npvt_node if opvt_node != npvt_node.
   Prefix merge (sort) from old bucket to new_bkt
   if opvt_node == npvt_node */
int
th3_alpm_bkt_pfx_copy(int u, _alpm_cb_t *acb,
                      _alpm_bkt_pfx_arr_t *pfx_arr,
                      _alpm_pvt_node_t *opvt_node,
                      _alpm_pvt_node_t *npvt_node,
                      _alpm_bkt_info_t *new_bkt)
{
    int i, rv = BCM_E_NONE;
    int vrf_id;
    uint32 src_idx, dst_idx = 0;
    uint8  def_fmt;
    uint16 sb_cnt[ALPM_BPB_MAX] = {0};
    uint16 db_cnt[ALPM_BPB_MAX] = {0};
    uint32 srcbnke[ALPM_BPB_MAX][17];
    uint32 dstbnke[ALPM_BPB_MAX][17];

    int     bulk_idx[ALPM_BPB_MAX];
    uint32  *bulk_ent[ALPM_BPB_MAX];
    int     bulk_ent_cnt = 0;

    _alpm_bkt_info_t *src_bkt, *dst_bkt;
    _alpm_bkt_pool_conf_t *bp_conf;

    vrf_id  = PVT_BKT_VRF(opvt_node);
    src_bkt = &PVT_BKT_INFO(opvt_node);
    if (npvt_node != opvt_node) {
        dst_bkt = &PVT_BKT_INFO(npvt_node);
    } else {
        dst_bkt = new_bkt;
    }
    bp_conf = ACB_BKT_VRF_POOL(acb, vrf_id);

    for (i = 0; i < pfx_arr->pfx_cnt; i++) {
        sb_cnt[ALPM_IDX_TO_BNK(acb, pfx_arr->pfx[i]->ent_idx)]++;
    }

    for (i = 0; i < ALPM_BPB_MAX; i++) {
        if (sb_cnt[i] == 0) {
            continue;
        }
        rv = th3_mem_read(u, acb, BPC_BKT_TBL(bp_conf),
                          ALPM_TAB_IDX_GET_BI_BNK(acb, src_bkt, i),
                          srcbnke[i], FALSE);
        ALPM_IER(rv);
    }

    def_fmt = alpm_util_bkt_def_fmt_get(u, acb, vrf_id, PVT_BKT_IPT(opvt_node));
    sal_memset(dstbnke, 0, sizeof(dstbnke));
    for (i = 0; i < pfx_arr->pfx_cnt; i++) {
        int dst_bnk, src_bnk, empty_bnk;
        src_idx = pfx_arr->pfx[i]->ent_idx;
        src_bnk = ALPM_IDX_TO_BNK(acb, src_idx);
        rv = alpm_bkt_ent_get_attempt(u, acb,
                ACB_BKT_VRF_POOL(acb, vrf_id),
                dst_bkt, def_fmt,
                pfx_arr->pfx[i]->key_len - PVT_KEY_LEN(npvt_node),
                &dst_idx, NULL, 0, &empty_bnk);
        /* We should not see a FULL here */
        ALPM_IER(rv);

        /* Need for merge */
        dst_bnk = ALPM_IDX_TO_BNK(acb, dst_idx);
        if (db_cnt[dst_bnk] == 0 && empty_bnk == FALSE) {
            rv = th3_mem_read(u,  acb, BPC_BKT_TBL(bp_conf),
                              ALPM_TAB_IDX_GET_BI_BNK(acb, dst_bkt, dst_bnk),
                              dstbnke[dst_bnk], FALSE);
            ALPM_IER(rv);
        }

        th3_alpm_ent_copy(u, vrf_id,  acb,
                          srcbnke[src_bnk],
                          dstbnke[dst_bnk],
                          src_bkt, dst_bkt,
                          PVT_KEY_LEN(opvt_node),
                          PVT_KEY_LEN(npvt_node),
                          src_idx, dst_idx,
                          PVT_BKT_IPT(opvt_node),
                          pfx_arr->pfx[i]);

        db_cnt[dst_bnk]++;
        pfx_arr->pfx[i]->ent_idx = dst_idx;
        if (!ALPM_HIT_SKIP(u)) {
            th3_alpm_ent_hit_move(u, vrf_id, acb, src_idx, dst_idx);
        }
    }

    for (i = 0; i < ALPM_BPB_MAX; i++) {
        if (db_cnt[i] == 0) {
            continue;
        }

        bulk_idx[bulk_ent_cnt] = ALPM_TAB_IDX_GET_BI_BNK(acb, dst_bkt, i);
        bulk_ent[bulk_ent_cnt] = dstbnke[i];
        bulk_ent_cnt ++;
    }

    rv = th3_mem_bulk_write(u, acb, npvt_node,
            BPC_BKT_TBL(bp_conf),
            bulk_idx, bulk_ent, bulk_ent_cnt);

    ALPM_IER(rv);

    /* TH3TBD: rollback@err */
    return rv;
}

int
th3_alpm_bkt_pfx_clean(int u, _alpm_cb_t *acb,
                        _alpm_pvt_node_t *pvt_node,
                        int count, uint32 *ent_idx)
{
    int i, rv = BCM_E_NONE;
    int vrf_id;
    uint32 src_idx, ent_id;
    /* Bulk write */
    int     bulk_idx[ALPM_BPB_MAX];
    uint32  *bulk_ent[ALPM_BPB_MAX];
    int     bulk_ent_cnt = 0;

    soc_format_t bnk_fmt;

    uint16 srcbnk[ALPM_BPB_MAX] = {0};
    uint32 srcbnke[ALPM_BPB_MAX][17];

    _alpm_bkt_pool_conf_t *bp_conf;

    vrf_id = PVT_BKT_VRF(pvt_node);
    bp_conf = ACB_BKT_VRF_POOL(acb, vrf_id);

    for (i = 0; i < count; i++) {
        srcbnk[ALPM_IDX_TO_BNK(acb, ent_idx[i])]++;
    }

    for (i = 0; i < ALPM_BPB_MAX; i++) {
        if (srcbnk[i] == 0) {
            continue;
        }
        rv = th3_mem_read(u, acb, BPC_BKT_TBL(bp_conf),
                ALPM_TAB_IDX_GET_BI_BNK(acb, &PVT_BKT_INFO(pvt_node), i),
                srcbnke[i], FALSE);
        ALPM_IER(rv);
    }

    /* Clear corresponding entry from bank */
    for (i = 0; i < count; i++) {
        uint32 *vet_bmp;
        uint32 tmpe[17] = {0};
        int8 fmt_tp;
        int bnk_idx;

        src_idx = ent_idx[i];
        bnk_idx = ALPM_IDX_TO_BNK(acb, src_idx);
        fmt_tp = th3_alpm_bnk_fmt_get(u, acb, &PVT_BKT_INFO(pvt_node), src_idx);
        bnk_fmt = BPC_FMT_BNK(bp_conf, fmt_tp);
        ent_id  = ALPM_IDX_TO_ENT(src_idx);
        vet_bmp = &(PVT_BKT_INFO(pvt_node).vet_bmp[bnk_idx]);
        *vet_bmp = (*vet_bmp) & ~(1 << ent_id);
        if (*vet_bmp) {
            (void)soc_format_field_set(u, bnk_fmt,
                        srcbnke[bnk_idx],
                        th3_alpm_ent_fld[ent_id], tmpe);
        } else {
            srcbnk[bnk_idx] = 0;
        }
    }

    for (i = 0; i < ALPM_BPB_MAX; i++) {
        if (srcbnk[i] == 0) {
            continue;
        }

        bulk_idx[bulk_ent_cnt] =
            ALPM_TAB_IDX_GET_BI_BNK(acb, &PVT_BKT_INFO(pvt_node), i);
        bulk_ent[bulk_ent_cnt] = srcbnke[i];
        bulk_ent_cnt ++;
    }

    rv = th3_mem_bulk_write(u, acb, pvt_node,
            BPC_BKT_TBL(bp_conf),
            bulk_idx, bulk_ent, bulk_ent_cnt);

    ALPM_IER(rv);

    if (!ALPM_HIT_SKIP(u)) {
        for (i = 0; i < count; i++) {
            _alpm_tbl_t tbl = alpmTblInvalid;
            src_idx = ent_idx[i];
            tbl = ACB_BKT_TBL(acb, vrf_id);
            th3_alpm_bkt_hit_write(u, vrf_id, acb, tbl, src_idx, 0);
        }
    }

    return rv;
}

/* Only do defrag inside a bucket */
int
th3_alpm_bkt_pfx_shrink(int u, _alpm_cb_t *acb,
                        _alpm_pvt_node_t *pvt_node,
                        _alpm_bkt_pfx_arr_t *pfx_arr,
                        int shrink_empty_only)
{
    int tb, fb, tbnk, fbnk;
    int rv = BCM_E_NONE;
    int start, close, step;
    int bnkpb = ACB_BNK_PER_BKT(acb, PVT_BKT_VRF(pvt_node));

    _alpm_bkt_info_t *bkt_info = NULL;

    bkt_info = &PVT_BKT_INFO(pvt_node);

#if 0 /* TH3TBD ??? */
    if (bkt_info->bkt_idx == 0) {
        return rv;
    }
#endif

    if (bkt_info->reverse == 0) {
        start = BI_ROFS(bkt_info);
        close = BI_ROFS(bkt_info) + bnkpb;
        step  = 1;
    } else {
        start = BI_ROFS(bkt_info) + bnkpb - 1;
        close = BI_ROFS(bkt_info) - 1;
        step  = -1;
    }

    for (tb = start; ABS(tb-close) > 0; tb += step) {
        tbnk = tb % bnkpb;
        /* Found a used tbnk */
        if (!BI_BNK_IS_USED(bkt_info, tbnk)) {
            continue;
        }
        for (fb = (close - step); ABS(fb-tb) > 0; fb -= step) {
            fbnk = fb % bnkpb;
            /* Bank fb empty */
            if (!BI_BNK_IS_USED(bkt_info, fbnk) ||
                bkt_info->vet_bmp[fbnk] == 0) {
                continue;
            }

            /* Bank tb empty, Bank fb occupied */
            if (bkt_info->vet_bmp[tbnk] == 0) {
                rv = th3_alpm_bkt_bnk_shrink(u, acb, pvt_node, fbnk, tbnk,
                                             pfx_arr);
                ALPM_IER(rv);
            } else if (bkt_info->bnk_fmt[tbnk] >= bkt_info->bnk_fmt[fbnk]) {
                /* Bank tb & fb are same type and both occupied */
                if (!shrink_empty_only) {
                    rv = th3_alpm_bkt_bnk_shrink(u, acb, pvt_node, fbnk,
                                                 tbnk, pfx_arr);
                    ALPM_IER(rv);
                }
            }
        }
    }

    return rv;
}

int
th3_alpm_bkt_bnk_copy(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node,
                      _alpm_bkt_info_t *src_bkt, _alpm_bkt_info_t *dst_bkt,
                      int src_idx, int dst_idx)
{
    int rv = BCM_E_NONE;
    uint32 ent[ALPM_ENT_WORDS_MAX] = {0};

    _alpm_tbl_t tbl = alpmTblInvalid;

    if (pvt_node != NULL) {
        tbl = BPC_BKT_TBL(ACB_BKT_VRF_POOL(acb, PVT_BKT_VRF(pvt_node)));
    }

    if (tbl != alpmTblInvalid) {
        ALPM_IER(
            th3_mem_read(u, acb, tbl, src_idx, ent, FALSE));
        ALPM_IER(
            th3_mem_write(u, acb, pvt_node, tbl, dst_idx, ent, FALSE));

        if (!ALPM_HIT_SKIP(u)) {
            th3_alpm_bnk_hit_move(u, PVT_BKT_VRF(pvt_node),
                                  acb, src_idx, dst_idx);
        }
    }

    return rv;
}

/* Not used by TH3
int
th3_alpm_bkt_bnk_clear(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node,
                        int tab_idx)
{
    int rv = BCM_E_NONE;
    _alpm_tbl_t tbl = alpmTblInvalid;

    if (pvt_node != NULL) {
        tbl = ACB_BKT_TBL(acb, PVT_BKT_VRF(pvt_node));
        ALPM_IER(th3_mem_write(u, acb, pvt_node, tbl, tab_idx, NULL));

        if (!ALPM_HIT_SKIP(u)) {
            th3_alpm_bnk_hit_clear(u, PVT_BKT_VRF(pvt_node), acb, tab_idx);
        }
    }

    return rv;
}
*/

/* TCAM related functions */
int
th3_tcam_table_sz(int u, int app, int pkm)
{
    int sz = 0;
    soc_mem_t mem;
    mem = alpm_tbl[ACB_TOP(u, app)->pvt_tbl[pkm]];

    if (mem != INVALIDm) {
        sz = th3_alpm_mem_size(u, mem);

        if (SOC_URPF_STATUS_GET(u)) {
            sz >>= 1;
        }
    }
    return sz;
}

/* TCAM Hw table cleanup (fast delete_all) */
int
th3_tcam_hw_cleanup(int u, int app)
{
    soc_mem_t mem;
    _alpm_cb_t *acb = ACB_TOP(u, app);
    int pkm, tbl_sz;
    int rv = BCM_E_NONE;

    for (pkm = 0; pkm < ALPM_PKM_CNT; pkm++) {

        if (ALPM_TCAM_TBL_SKIP(u, pkm)) {
            continue; /* skip duplicated or invalid TCAM table */
        }

        mem = alpm_tbl[acb->pvt_tbl[pkm]];
        tbl_sz = th3_tcam_table_sz(u, app, pkm);
        if (tbl_sz) {
            th3_alpm_hw_mem_clear(u, mem);
        }
    }

    return rv;
}

/*
 * x | y
 * --------------------
 * 0 | 0 : entry0_to_0
 * 0 | 1 : entry0_to_1
 * 1 | 0 : entry1_to_0
 * 1 | 1 : entry1_to_1
 * src and dst can be same
 */
int
th3_tcam_entry_x_to_y(int u, int app, int pkm, void *src, void *dst,
                      int copy_hit, int x, int y)
{
    int                 i;
    uint32              val[SOC_MAX_MEM_FIELD_WORDS] = {0};
    soc_mem_t           mem;
    _alpm_cb_t    *acb = ACB_TOP(u, app);

#define _FLD_CNT    6
    soc_field_t fld[2][_FLD_CNT] =
        {
            {
                VALID0f,
                KEY0f,
                MASK0f,
                ASSOC_DATA0f,
                ALPM1_DATA0f,
                FIXED_DATA0f
             },
            {
                VALID1f,
                KEY1f,
                MASK1f,
                ASSOC_DATA1f,
                ALPM1_DATA1f,
                FIXED_DATA1f
            }
        };
    soc_field_t hit[2] = {HIT0f, HIT1f};

    mem = alpm_tbl[acb->pvt_tbl[pkm]];
    for (i = 0; i < _FLD_CNT; i++) {
        soc_mem_field_get(u, mem, src, fld[x][i], val);
        soc_mem_field_set(u, mem, dst, fld[y][i], val);
    }

    if (copy_hit) {
        soc_mem_field_get(u, mem, src, hit[x], val);
        soc_mem_field_set(u, mem, dst, hit[y], val);
    }

    return BCM_E_NONE;
}

/*
 * field mapping:
 *
 * defip_flags
 *      BCM_L3_HIT          -> HIT0f
 *      BCM_L3_RPE          -> ASSOC_DATA0  -> RPE
 *      BCM_L3_MULTIPATH    -> ASSOC_DATA0  -> DEST_TYPE
 *      BCM_L3_DST_DISCARD  -> ASSOC_DATA0  -> DST_DISCARD
 *      BCM_L3_IP6          -> KEY0         -> KEY_MODE
 *                          -> KEY0         -> KEY_TYPE
 * defip_prio               -> ASSOC_DATA0  -> PRI
 * defip_ecmp_index         -> ASSOC_DATA0  -> DESTINATION
 * defip_lookup_class       -> ASSOC_DATA0  -> CLASS_ID
 * defip_vrf
 *      BCM_L3_VRF_GLOBAL   -> ALPM1_DATA   -> DIRECT_ROUTE
 *                          -> FIXED_DATA0  -> SUB_DB_PRIORITY
 *
 *      BCM_L3_VRF_OVERRIDE -> ALPM1_DATA   -> DIRECT_ROUTE
 *                          -> FIXED_DATA0  -> SUB_DB_PRIORITY
 *
 *      other               -> KEY0         -> VRF_ID
 *                          -> MASK0        -> VRF_ID
 *                          -> FIXED_DATA0  -> SUB_DB_PRIORITY
 *
 * defip_sub_len            -> MASK0        -> IP_ADDR
 * defip_ip_addr            -> KEY0         -> IP_ADDR
 * defip_ip6_addr           -> KEY0         -> IP_ADDR
 *
 * defip_flex_ctr_base_id   -> ASSOC_DATA0  -> FLEX_CTR_BASE_COUNTER_IDX
 * defip_flex_ctr_mode      -> ASSOC_DATA0  -> FLEX_CTR_OFFSET_MODE
 * defip_flex_ctr_pool      -> ASSOC_DATA0  -> FLEX_CTR_POOL_NUMBER
 */
int
th3_tcam_entry_from_cfg(int u, int pkm, _bcm_defip_cfg_t *lpm_cfg, void *e, int x, uint32 write_flags)
{
    int             i, b, cnt = 2;
    uint32          key[4] = {0};
    uint32          msk[4] = {0};
    uint32          val;
    int app = alpm_app_get(u, lpm_cfg->defip_inter_flags);

    soc_mem_t       mem;
    _alpm_cb_t      *acb = ACB_TOP(u, app);
    _alpm_pvt_node_t *pvt_node;
    _alpm_bkt_pool_conf_t *bp_conf;

    soc_field_t hit_fld[] = {HIT0f, HIT1f};
    soc_field_t asd_fld[] = {ASSOC_DATA0f, ASSOC_DATA1f};
    soc_field_t fix_fld[] = {FIXED_DATA0f, FIXED_DATA1f};
    soc_field_t bkt_fld[] = {ALPM1_DATA0f, ALPM1_DATA1f};
    soc_field_t vld_fld[] = {VALID0f, VALID1f};
    soc_field_t key_fld[] = {KEY0f, KEY1f, KEY0_UPRf, KEY1_UPRf};
    soc_field_t msk_fld[] = {MASK0f, MASK1f, MASK0_UPRf, MASK1_UPRf};

    th3_fmt_FIXED_DATA_t fmt_fd;
    th3_fmt_ASSOC_DATA_FULL_t fmt_asf;
    th3_fmt_L3_DEFIP_TCAM_KEY_t fmt_tk;
    th3_fmt_ALPM1_DATA_t fmt_a1d;

    mem = alpm_tbl[acb->pvt_tbl[pkm]];
    if (SOC_MEM_FIELD_VALID(u, mem, ASSOC_DATA0f)) {
        if (pkm == ALPM_PKM_32B) {
            hit_fld[!x] = hit_fld[!!x];
            asd_fld[!x] = asd_fld[!!x];
            fix_fld[!x] = fix_fld[!!x];
            bkt_fld[!x] = bkt_fld[!!x];
            vld_fld[!x] = vld_fld[!!x];
            key_fld[!x] = key_fld[!!x];
            msk_fld[!x] = msk_fld[!!x];
            cnt = 1;
        }
    } else {
        hit_fld[0] = LWR_HIT0f;
        hit_fld[1] = LWR_HIT1f;
        asd_fld[0] = LWR_ASSOC_DATA0f;
        asd_fld[1] = LWR_ASSOC_DATA1f;
        fix_fld[0] = LWR_FIXED_DATA0f;
        fix_fld[1] = LWR_FIXED_DATA1f;
        bkt_fld[0] = LWR_ALPM1_DATA0f;
        bkt_fld[1] = LWR_ALPM1_DATA1f;
        vld_fld[0] = LWR_VALID0f;
        vld_fld[1] = LWR_VALID1f;
        key_fld[0] = KEY0_LWRf;
        key_fld[1] = KEY1_LWRf;
        msk_fld[0] = MASK0_LWRf;
        msk_fld[1] = MASK1_LWRf;
    }

    for (i = 0; i < cnt; i++) {
        if (write_flags & ALPM_PVT_UPDATE_PREFIX) {
            soc_mem_field32_set(u, mem, e, vld_fld[i], 1);
            if (lpm_cfg->defip_flags & BCM_L3_HIT) {
                soc_mem_field32_set(u, mem, e, hit_fld[i], 1);
            }
        }

        /* ASSOC_DATAf format */
        if (write_flags & ALPM_PVT_UPDATE_ASSOC_DATA) {
            sal_memset(&fmt_asf, 0, sizeof(fmt_asf));
            fmt_asf.RPE = lpm_cfg->defip_flags & BCM_L3_RPE ? 1 : 0;
            fmt_asf.DEST_TYPE = lpm_cfg->defip_flags & BCM_L3_MULTIPATH ? 1 : 0;
            fmt_asf.DESTINATION = lpm_cfg->defip_ecmp_index;
            fmt_asf.DST_DISCARD = lpm_cfg->defip_flags & BCM_L3_DST_DISCARD ? 1 : 0;
            fmt_asf.PRI = lpm_cfg->defip_prio;
            fmt_asf.CLASS_ID = lpm_cfg->defip_lookup_class;
            fmt_asf.FLEX_CTR_BASE_COUNTER_IDX = lpm_cfg->defip_flex_ctr_base_id;
            fmt_asf.FLEX_CTR_OFFSET_MODE = lpm_cfg->defip_flex_ctr_mode;
            fmt_asf.FLEX_CTR_POOL_NUMBER = lpm_cfg->defip_flex_ctr_pool;
            soc_mem_field_set(u, mem, e, asd_fld[i], (uint32 *)&fmt_asf);
        }

        /* FIXED_DATAf format */
        if (write_flags & ALPM_PVT_UPDATE_PREFIX) {
            sal_memset(&fmt_fd, 0, sizeof(fmt_fd));
            switch (lpm_cfg->defip_vrf) {
                case BCM_L3_VRF_GLOBAL:
                    val = 2;
                    break;
                case BCM_L3_VRF_OVERRIDE:
                    val = 0;
                    break;
                default:
                    val = 1;
                    break;
            }
            fmt_fd.SUB_DB_PRIORITY = val;
            soc_mem_field_set(u, mem, e, fix_fld[i], (uint32 *)&fmt_fd);
        }

        /* ALPM1_DATAf format */
        if (write_flags & ALPM_PVT_UPDATE_ALPM_DATA) {
            sal_memset(&fmt_a1d, 0, sizeof(fmt_a1d));

            fmt_a1d.DIRECT_ROUTE = ALPM_LPM_DIRECT_RTE(u, lpm_cfg);
            if (ALPM_TCAM_ZONED(u, acb->app) && lpm_cfg->default_miss == 1) {
                /* TH3TBD, Parallel & Mixed ALPM need to set DEFAULT_MISS to 1
                 * if assoc_data is NOT set, in case of missing default */
                fmt_a1d.DEFAULT_MISS = 1;
            }
            pvt_node = (_alpm_pvt_node_t *)lpm_cfg->pvt_node;
            if (pvt_node != NULL) {
                int offset = 0;
                _alpm_tbl_t tbl;
                _alpm_bkt_info_t *bkt_info = &PVT_BKT_INFO(pvt_node);

                bp_conf = ACB_BKT_VRF_POOL(acb, PVT_BKT_VRF(pvt_node));
                tbl = BPC_BKT_TBL(bp_conf);
                switch (tbl) {
                    case alpmTblBktPmodeGlbL2:
                    case alpmTblBktPmodeGlbL3:
                        offset = ACB_BNK_PER_BKT(acb, 1);
                        break;
                    default:
                        break;
                }
                fmt_a1d.KSHIFT = PVT_KEY_LEN(pvt_node);
                fmt_a1d.ROFS = PVT_ROFS(pvt_node) + offset;
                fmt_a1d.BKT_PTR = PVT_BKT_IDX(pvt_node);
                for (b = 0; b < BPC_BNK_PER_BKT(bp_conf); b++) {
                    soc_field_t bnk_fld;
                    uint8 bnk_fmt;
                    if (!BI_BNK_IS_USED(bkt_info, b)) {
                        continue;
                    }
                    bnk_fld = th3_alpm_bnk_fld[b + offset];
                    bnk_fmt = bkt_info->bnk_fmt[b];
                    if (bnk_fmt > 0 && ACB_HAS_RTE(acb, ALPM_LPM_VRF_ID(u, lpm_cfg))) {
                        bnk_fmt --;
                    }
                    switch (bnk_fld) {
                        case FMT0f:
                            fmt_a1d.FMT0 = bnk_fmt;
                            break;
                        case FMT1f:
                            fmt_a1d.FMT1 = bnk_fmt;
                            break;
                        case FMT2f:
                            fmt_a1d.FMT2_0_1 = bnk_fmt & 0x3;
                            fmt_a1d.FMT2_2_3 = (bnk_fmt >> 2) & 0x3;
                            break;
                        case FMT3f:
                            fmt_a1d.FMT3 = bnk_fmt;
                            break;
                        case FMT4f:
                            fmt_a1d.FMT4 = bnk_fmt;
                            break;
                        default:
                            break;
                    }
                }
            }
            soc_mem_field_set(u, mem, e, bkt_fld[i], (uint32 *)&fmt_a1d);
        } /* if (write_flags & ALPM_PVT_UPDATE_ASSOC_DATA) */
    }

    if (hit_fld[0] == LWR_HIT0f) {
        soc_mem_field32_set(u, mem, e, UPR_VALIDf, 3);
    }

    if (write_flags & ALPM_PVT_UPDATE_PREFIX) {
        /* Program KEY & MASK
         *
         *           key[4] = {key[0], key[1], key[2], key[3]}
         * ipv4:     key[4] =
         *      {IP_ADDR0f, 0, 0, 0}
         * ipv6-64:  key[4] =
         *      {IP_ADDR0f, IP_ADDR1f, 0, 0}
         * ipv6-128: key[4] =
         *      {IP_ADDR0_LWRf, IP_ADDR1_LWRf, IP_ADDR0_UPRf, IP_ADDR1_UPRf}
         */
        alpm_util_cfg_to_key(u, ALPM_LPM_IPT(u, lpm_cfg), lpm_cfg, key);
        alpm_util_cfg_to_msk(u, ALPM_LPM_IPT(u, lpm_cfg), lpm_cfg, msk);
        if (hit_fld[0] == LWR_HIT0f) {
            cnt = 4;
        }

        switch (pkm) {
            case ALPM_PKM_32B:
                val = 0;
                break;
            case ALPM_PKM_64B:
                key[1] = key[3];
                key[0] = key[2];
                msk[1] = msk[3];
                msk[0] = msk[2];
                val = 1;
                break;
            case ALPM_PKM_128:
                val = 3;
                break;
            default:
                val = 0;
                break;
        }

        for (i = 0; i < cnt; i++) {
            /* KEY */
            sal_memset(&fmt_tk, 0, sizeof(fmt_tk));
            fmt_tk.KEY_MODE = val;
            fmt_tk.KEY_TYPE = (lpm_cfg->defip_flags & BCM_L3_IP6) ? 1 : 0;
            fmt_tk.IP_ADDR_0_28 = key[i] & 0x1fffffff;
            fmt_tk.IP_ADDR_29_31 = (key[i] >> 29) & 0x7;
            fmt_tk.VRF_ID = lpm_cfg->defip_vrf >= 0 ? lpm_cfg->defip_vrf : 0;
            soc_mem_field_set(u, mem, e, key_fld[i], (uint32 *)&fmt_tk);

            /* MASK */
            sal_memset(&fmt_tk, 0, sizeof(fmt_tk));
            fmt_tk.KEY_MODE = 0x3;
            fmt_tk.KEY_TYPE = 0x1;
            fmt_tk.IP_ADDR_0_28 = msk[i] & 0x1fffffff;
            fmt_tk.IP_ADDR_29_31 = (msk[i] >> 29) & 0x7;
            fmt_tk.VRF_ID = lpm_cfg->defip_vrf >= 0 ? 0xfff : 0;
            soc_mem_field_set(u, mem, e, msk_fld[i], (uint32 *)&fmt_tk);
        }
    }

    return BCM_E_NONE;
}

int
th3_tcam_entry_to_cfg(int u, int pkm, void *e, int x,
                      _bcm_defip_cfg_t *lpm_cfg)
{
    uint32              val;
    soc_mem_t           mem;
    th3_fmt_ASSOC_DATA_FULL_t fmte;
    int app = alpm_app_get(u, lpm_cfg->defip_inter_flags);
    _alpm_cb_t          *acb = ACB_TOP(u, app);

    soc_field_t hit_fld[] = {HIT0f, HIT1f};
    soc_field_t asd_fld[] = {ASSOC_DATA0f, ASSOC_DATA1f};
    soc_field_t fix_fld[] = {FIXED_DATA0f, FIXED_DATA1f};
    soc_field_t bkt_fld[] = {ALPM1_DATA0f, ALPM1_DATA1f};

    mem = alpm_tbl[acb->pvt_tbl[pkm]];
    if (SOC_MEM_FIELD_VALID(u, mem, ASSOC_DATA0f)) {
        if (pkm == ALPM_PKM_32B) {
            hit_fld[!x] = hit_fld[!!x];
            asd_fld[!x] = asd_fld[!!x];
            fix_fld[!x] = fix_fld[!!x];
            bkt_fld[!x] = bkt_fld[!!x];
        }
    } else {
        hit_fld[0] = LWR_HIT0f;
        asd_fld[0] = LWR_ASSOC_DATA0f;
        fix_fld[0] = LWR_FIXED_DATA0f;
        bkt_fld[0] = LWR_ALPM1_DATA0f;
    }

#define _LPM_DEFIP_FLAGS_SET(val, flag)     \
    do {                                    \
        if (val) {                          \
            lpm_cfg->defip_flags |= flag;   \
        } else {                            \
            lpm_cfg->defip_flags &= ~flag;  \
        }                                   \
    } while (0)

    val = soc_mem_field32_get(u, mem, e, hit_fld[0]);
    _LPM_DEFIP_FLAGS_SET(val, BCM_L3_HIT);

    /* ASSOC_DATAf format */
    soc_mem_field_get(u, mem, e, asd_fld[0], (uint32 *)&fmte);
    val = fmte.RPE;
    _LPM_DEFIP_FLAGS_SET(val, BCM_L3_RPE);

    val = fmte.DEST_TYPE;
    _LPM_DEFIP_FLAGS_SET(val, BCM_L3_MULTIPATH);
    if (val) {
        lpm_cfg->defip_ecmp = 1;
    } else {
        lpm_cfg->defip_ecmp = 0;
        lpm_cfg->defip_ecmp_count = 0;
    }

    lpm_cfg->defip_ecmp_index = fmte.DESTINATION;

    val = fmte.DST_DISCARD;
    _LPM_DEFIP_FLAGS_SET(val, BCM_L3_DST_DISCARD);

    lpm_cfg->defip_prio = fmte.PRI;
    lpm_cfg->defip_lookup_class = fmte.CLASS_ID;
    lpm_cfg->defip_flex_ctr_base_id = fmte.FLEX_CTR_BASE_COUNTER_IDX;
    lpm_cfg->defip_flex_ctr_mode = fmte.FLEX_CTR_OFFSET_MODE;
    lpm_cfg->defip_flex_ctr_pool = fmte.FLEX_CTR_POOL_NUMBER;

    /* No need to parse the rest field because there are already in the
     * lpm_cfg, part of key */

    return BCM_E_NONE;
}

int
th3_tcam_entry_valid(int u, int app, int pkm, void *e, int sub_idx)
{
    int vld_val;
    soc_mem_t mem;
    soc_field_t vld_fld = VALIDf;
    _alpm_cb_t *acb = ACB_TOP(u, app);

    mem = alpm_tbl[acb->pvt_tbl[pkm]];
    if (SOC_MEM_FIELD_VALID(u, mem, VALIDf)) {
        vld_fld = VALIDf;
    } else {
        vld_fld = LWR_VALIDf;
    }

    vld_val = soc_mem_field32_get(u, mem, e, vld_fld);

    return vld_val & (1 << sub_idx) ? 1 : 0;
}

int
th3_tcam_entry_valid_set(int u, int app, int pkm, void *e, int sub_idx, int val)
{
    soc_mem_t mem;
    soc_field_t vld_fld;
    _alpm_cb_t *acb = ACB_TOP(u, app);

    mem = alpm_tbl[acb->pvt_tbl[pkm]];
    switch (pkm) {
        case ALPM_PKM_32B:
            vld_fld = sub_idx ? VALID1f : VALID0f;
            soc_mem_field32_set(u, mem, e, vld_fld, val ? 1 : 0);
            break;
        case ALPM_PKM_64B:
            soc_mem_field32_set(u, mem, e, VALIDf, val ? 3 : 0);
            break;
        case ALPM_PKM_128:
            soc_mem_field32_set(u, mem, e, LWR_VALIDf, val ? 3 : 0);
            soc_mem_field32_set(u, mem, e, UPR_VALIDf, val ? 3 : 0);
            break;
        default:
            break;
    }

    return BCM_E_NONE;
}

static void
th3_tcam_key_fields_get(int u, soc_mem_t mem, void *e, soc_field_t fmt,
                        int fld_cnt, soc_field_t *flds, uint32 *vals)
{
    int         i;
    th3_fmt_L3_DEFIP_TCAM_KEY_t fmt_tk;

    for (i = 0; i < fld_cnt; i++) {
        soc_mem_field_get(u, mem, e, fmt, (uint32 *)&fmt_tk);
        switch(flds[i]) {
            case KEY_MODEf:
                vals[i] = fmt_tk.KEY_MODE;
                break;
            case KEY_TYPEf:
                vals[i] = fmt_tk.KEY_TYPE;
                break;
            case VRF_IDf:
                vals[i] = fmt_tk.VRF_ID;
                break;
            case IP_ADDRf:
                vals[i] = (fmt_tk.IP_ADDR_29_31 << 29) | fmt_tk.IP_ADDR_0_28;
                break;
            default:
                break;
        }
    }

    return ;
}

static int
th3_tcam_entry_to_mask_key(int u, int app, int pkm, void *e, int sub_idx,
                            uint32 *key, int is_mask)
{
    int         i, fld_cnt;
    soc_mem_t   mem;
    soc_field_t _msk_fld[] = {MASK0f, MASK1f};
    soc_field_t _key_fld[] = {KEY0f, KEY1f};
    soc_field_t _msk_fld_p[] = {MASK0_LWRf, MASK1_LWRf, MASK0_UPRf, MASK1_UPRf};
    soc_field_t _key_fld_p[] = {KEY0_LWRf, KEY1_LWRf, KEY0_UPRf, KEY1_UPRf};
    soc_field_t key_fld = IP_ADDRf;

    soc_field_t *_fld, *_fld_p, *fmt;
    _alpm_cb_t *acb = ACB_TOP(u, app);

    if (is_mask) {
        _fld = _msk_fld;
        _fld_p = _msk_fld_p;
    } else {
        _fld = _key_fld;
        _fld_p = _key_fld_p;
    }

    mem = alpm_tbl[acb->pvt_tbl[pkm]];
    if (SOC_MEM_FIELD_VALID(u, mem, ASSOC_DATA0f)) {
        if (pkm == ALPM_PKM_32B) {
            fld_cnt = 1;
            _fld[0] = _fld[sub_idx];
        } else {
            fld_cnt = 2;
        }
        fmt = _fld;
    } else {
        fld_cnt = 4;
        fmt = _fld_p;
    }

    sal_memset(key, 0, 4 * sizeof(uint32));
    for (i = 0; i < fld_cnt; i++) {
        th3_tcam_key_fields_get(u, mem, e, fmt[i], 1, &key_fld, &key[i]);
    }

    if (pkm == ALPM_PKM_64B) {
        key[3] = key[1];
        key[2] = key[0];
        key[1] = key[0] = 0;
    }

    return BCM_E_NONE;

}

/*
 *           key[4] = {key[0], key[1], key[2], key[3]}
 * ipv4:     key[4] =
 *               {IP_ADDR0f, 0, 0, 0}
 * ipv6-64:  key[4] =
 *               {0, 0, IP_ADDR0f, IP_ADDR1f, }
 * ipv6-128: key[4] =
 *               {IP_ADDR0_LWRf, IP_ADDR1_LWRf, IP_ADDR0_UPRf, IP_ADDR1_UPRf}
 */
int
th3_tcam_entry_to_key(int u, int app, int pkm, void *e, int sub_idx, uint32 *key)
{
    return th3_tcam_entry_to_mask_key(u, app, pkm, e, sub_idx, key, FALSE);
}

int
th3_tcam_entry_vrf_id_get(int u, int app, int pkm, void *e, int sub_idx, int *vrf_id)
{
    uint32      fld_val;
    soc_mem_t   mem;
    soc_field_t _glb_fld[]      = {FIXED_DATA0f, FIXED_DATA1f};
    soc_field_t _glb_fld_p[]    = {LWR_FIXED_DATA0f, LWR_FIXED_DATA1f};
    soc_field_t _vrf_fld[]      = {KEY0f, KEY1f};
    soc_field_t _vrf_fld_p[]    = {KEY0_LWRf, KEY1_LWRf};
    soc_field_t *glb_fmt, *vrf_fmt;
    soc_field_t key_fld = VRF_IDf;
    _alpm_cb_t *acb = ACB_TOP(u, app);

    th3_fmt_FIXED_DATA_t fmt_fd;

    mem = alpm_tbl[acb->pvt_tbl[pkm]];
    if (SOC_MEM_FIELD_VALID(u, mem, ASSOC_DATA0f)) {
        if (pkm == ALPM_PKM_32B) {
            _glb_fld[0] = _glb_fld[sub_idx];
            _vrf_fld[0] = _vrf_fld[sub_idx];
        }
        glb_fmt = _glb_fld;
        vrf_fmt = _vrf_fld;
    } else {
        glb_fmt = _glb_fld_p;
        vrf_fmt = _vrf_fld_p;
    }

    /* To distinguish Global/Private route */
    soc_mem_field_get(u, mem, e, glb_fmt[0], (uint32 *)&fmt_fd);
    fld_val = fmt_fd.SUB_DB_PRIORITY;
    if (fld_val == 0) {
        /* Global High */
        *vrf_id = ALPM_VRF_ID_GHI(u);
    } else if (fld_val == 2) {
        /* Global Low */
        *vrf_id = ALPM_VRF_ID_GLO(u);
    } else {
        /* Private routes */
        th3_tcam_key_fields_get(u, mem, e, vrf_fmt[0], 1, &key_fld,
                                (uint32 *)vrf_id);
    }


    return BCM_E_NONE;
}

int
th3_tcam_entry_key_type_get(int u, int app, int pkm, void *e, int sub_idx, int *key_type)
{
    soc_mem_t   mem;
    soc_field_t _vrf_fld[]      = {KEY0f, KEY1f};
    soc_field_t _vrf_fld_p[]    = {KEY0_LWRf, KEY1_LWRf};
    soc_field_t *key_type_fmt;
    soc_field_t key_fld = KEY_TYPEf;
    _alpm_cb_t *acb = ACB_TOP(u, app);

    mem = alpm_tbl[acb->pvt_tbl[pkm]];
    if (SOC_MEM_FIELD_VALID(u, mem, ASSOC_DATA0f)) {
        if (pkm == ALPM_PKM_32B) {
            _vrf_fld[0] = _vrf_fld[sub_idx];
        }
        key_type_fmt = _vrf_fld;
    } else {
        key_type_fmt = _vrf_fld_p;
    }
    th3_tcam_key_fields_get(u, mem, e, key_type_fmt[0], 1, &key_fld,
                            (uint32 *)key_type);

    return BCM_E_NONE;
}

int
th3_tcam_entry_pfx_len_get(int u, int app, int pkm, void *e, int sub_idx, int *pfx_len)
{
    int         i, rv = BCM_E_NONE;
    uint32      mask[4];

    /* Assuming each MASK field is correct,
     * Not my duty to validate MASK here */
    rv = th3_tcam_entry_to_mask_key(u, app, pkm, e, sub_idx, mask, TRUE);
    *pfx_len = 0;
    for (i = 0; i < 4; i++) {
        *pfx_len += _shr_popcount(mask[i]);
    }

    return rv;
}

int
th3_tcam_entry_mode_get_by_mem(int u, soc_mem_t mem, void *e, int *step_size,
                               int *pkm, int *ipv6, int *key_mode, int sub_idx)
{
    soc_field_t _key_fld[] = {KEY0f, KEY1f};
    soc_field_t key_fld = KEY0f, vld_fld = VALIDf;
    int valid, v6, km;

    th3_fmt_L3_DEFIP_TCAM_KEY_t fmt_tk;

    if (SOC_MEM_FIELD_VALID(u, mem, VALIDf)) {
        key_fld = _key_fld[sub_idx];
        vld_fld = VALIDf;
        if (step_size) {
            *step_size = 1; /* default step_size for un pair TCAM */
        }
    } else {
        key_fld = KEY0_LWRf;
        vld_fld = LWR_VALIDf;
        if (step_size) {
            *step_size = 2; /* default step_size for pair TCAM*/
        }
    }

    valid = soc_mem_field32_get(u, mem, e, vld_fld);
    valid &=  (1 << sub_idx);

    if (!valid) { /* invalid entry */
        return BCM_E_NOT_FOUND;
    }

    soc_mem_field_get(u, mem, e, key_fld, (uint32 *)&fmt_tk);
    km = fmt_tk.KEY_MODE;
    if (key_mode) {
        *key_mode = km;
    }
    v6 = fmt_tk.KEY_TYPE;
    if (ipv6) {
        *ipv6 = v6;
    }

    /* TH3TBD */
    if (pkm) {
        *pkm = (key_fld == KEY0_LWRf) ? ALPM_PKM_128 :
                                    v6 ? ALPM_PKM_64B : ALPM_PKM_32B;
    }

    /* Wrong key_mode or key_type */
    if ((key_fld != KEY0_LWRf && (km > 1)) ||
        (key_fld == KEY0_LWRf && (km != 3)) ||
        (key_fld == KEY0_LWRf && (v6 == 0))) {
        return BCM_E_INTERNAL;
    }

   /* step_size 2 only if full-wide in un pair TCAM */
   if (step_size) {
       if (key_fld != KEY0_LWRf && (km == 1)) {
           *step_size = 2;
       }
   }
   return BCM_E_NONE;
}

/*
 * Function:
 *      th3_tcam_entry_mode_get
 * Purpose:
 *      Get TCAM valid entry KEY mode and type.
 * Parameters:
 *      u          - (In)Device unit
 *      pk         - (In)Input TCAM packing mode table view
 *      e          - (In)TCAM lpm memory entry
 *      step_size  - (Out)Key mode step-size (un pair: 1 or 2, pair: 2)
 *      p km       - (Out)Detected packing mode
 *      ipv6       - (Out)Key Type (0:IPv4, 1:IPv6)
 *      key_mode   - (Out)Key Mode (0:Half, 1:Full, 3:Double)
 *      sub_idx    - (In)TCAM sub_idx (0, 1) for half-wide entries in unpair
 *                       TCAM. Pair TCAM has only one sub_idx=0.
 * Notes:
 *      The for-loop for sub_idx is using always as below:
 *        for (sub_idx = 0; sub_idx < ALPM_TCAM_SUBCNT; sub_idx += step_size)
 *      where
 *        ALPM_TCAM_SUBCNT is defined as 2, and
 *        step_size is variable, depending on the actual TCAM entry reading.
 *
 * Returns:
 *      BCM_E_XXX
 */
int
th3_tcam_entry_mode_get(int u, int app, int pk, void *e, int *step_size,
                        int *pkm, int *ipv6, int *key_mode, int sub_idx)
{
    _alpm_cb_t *acb = ACB_TOP(u, app);
    soc_mem_t mem = alpm_tbl[acb->pvt_tbl[pk]];
    return th3_tcam_entry_mode_get_by_mem(u, mem, e, step_size, pkm,
                                          ipv6, key_mode, sub_idx);
}

/* Get ASSOC_DATA from a TCAM entry */
int
th3_tcam_entry_adata_get(int u, int app, int pkm, void *e, int sub_idx,
                          _alpm_bkt_adata_t *adata)
{
    int rv = BCM_E_NONE;
    soc_field_t fldv;
    _alpm_cb_t *acb = ACB_TOP(u, app);
    soc_mem_t mem = alpm_tbl[acb->pvt_tbl[pkm]];
    th3_fmt_ASSOC_DATA_FULL_t fmt_adf;

    if (SOC_MEM_FIELD_VALID(u, mem, ASSOC_DATA0f)) {
        fldv = (sub_idx == 0 ? ASSOC_DATA0f : ASSOC_DATA1f);
    } else {
        fldv = (sub_idx == 0 ? LWR_ASSOC_DATA0f : LWR_ASSOC_DATA1f);
    }
    soc_mem_field_get(u, mem, e, fldv, (uint32 *)&fmt_adf);

    adata->defip_flags = fmt_adf.DEST_TYPE ? BCM_L3_MULTIPATH : 0;
    adata->defip_ecmp_index = fmt_adf.DESTINATION;
    adata->defip_flags |= fmt_adf.RPE ? BCM_L3_RPE : 0;
    adata->defip_flags |= fmt_adf.DST_DISCARD ? BCM_L3_DST_DISCARD : 0;
    adata->defip_prio = fmt_adf.PRI;
    adata->defip_lookup_class = fmt_adf.CLASS_ID;
    adata->defip_flex_ctr_base_id = fmt_adf.FLEX_CTR_BASE_COUNTER_IDX;
    adata->defip_flex_ctr_mode = fmt_adf.FLEX_CTR_OFFSET_MODE;
    adata->defip_flex_ctr_pool = fmt_adf.FLEX_CTR_POOL_NUMBER;
    adata->defip_acl_class_id = 0;
    adata->defip_acl_class_id_valid = 0;

    return rv;
}

/* Get ALPM_DATA entry from a TCAM entry */
int
th3_tcam_entry_bdata_get(int u, int app, int pkm, void *e, int sub_idx, void *fent)
{
    int rv = BCM_E_NONE;
    soc_field_t fldv;
    _alpm_cb_t *acb = ACB_TOP(u, app);
    soc_mem_t mem = alpm_tbl[acb->pvt_tbl[pkm]];

    if (SOC_MEM_FIELD_VALID(u, mem, ALPM1_DATA0f)) {
        fldv = (sub_idx == 0 ? ALPM1_DATA0f : ALPM1_DATA1f);
    } else {
        fldv = (sub_idx == 0 ? LWR_ALPM1_DATA0f : LWR_ALPM1_DATA1f);
    }
    soc_mem_field_get(u, mem, e, fldv, fent);
    return rv;
}

/***************** Bucket entry (Bank) starts from here *****************/
int
th3_alpm_ent_ent_get(int u, int vrf_id, _alpm_cb_t *acb, void *e, uint32 fmt,
                     int eid, void *ftmp)
{
    int rv = BCM_E_NONE;

    /* get bucket level bank format entry */
    soc_format_field_get(u, ACB_FMT_BNK(acb, vrf_id, fmt), e,
                         th3_alpm_ent_fld[eid], ftmp);
    return rv;
}

/* Get ALPM_Data bucket info (logic table based):
   bkt_ptr, rofs, kshift, def_miss and bnkfmt[] */
static void
th3_alpm_ent_binfo_get(int u, int vrf_id, _alpm_cb_t *acb, void *e,
                       _alpm_ent_info_t *info)
{
    int bnk, offset = 0;
    th3_fmt_ALPM1_DATA_t *fmt_a1d = NULL;
    th3_fmt_ALPM2_DATA_t *fmt_a2d = NULL;

    uint8  bnk_fmt[ALPM_BPB_MAX] = {0};
    uint16 bkt_idx;
    int    kshift;
    int    default_miss;
    uint8  rofs;

    if (ACB_HAS_TCAM(acb)) {
        fmt_a1d = e;
        bkt_idx = fmt_a1d->BKT_PTR;
        kshift  = fmt_a1d->KSHIFT;
        default_miss = fmt_a1d->DEFAULT_MISS;
        rofs    = fmt_a1d->ROFS;
        bnk_fmt[0] = fmt_a1d->FMT0;
        bnk_fmt[1] = fmt_a1d->FMT1;
        bnk_fmt[2] = (fmt_a1d->FMT2_2_3 << 2) | fmt_a1d->FMT2_0_1;
        bnk_fmt[3] = fmt_a1d->FMT3;
        bnk_fmt[4] = fmt_a1d->FMT4;
    } else {
        fmt_a2d = e;
        bkt_idx = fmt_a2d->BKT_PTR;
        kshift  = fmt_a2d->KSHIFT;
        default_miss = fmt_a2d->DEFAULT_MISS;
        rofs    = fmt_a2d->ROFS;
        bnk_fmt[0] = fmt_a2d->FMT0;
        bnk_fmt[1] = fmt_a2d->FMT1;
        bnk_fmt[2] = fmt_a2d->FMT2;
        bnk_fmt[3] = fmt_a2d->FMT3;
        bnk_fmt[4] = fmt_a2d->FMT4;
        bnk_fmt[5] = fmt_a2d->FMT5;
        bnk_fmt[6] = fmt_a2d->FMT6;
        bnk_fmt[7] = fmt_a2d->FMT7;
    }

    info->bkt_info.bkt_idx = bkt_idx;
    info->kshift = kshift;
    info->default_miss = default_miss;

    switch (ACB_BKT_TBL(acb, vrf_id)) {
        case alpmTblBktPmodeGlbL2:
        case alpmTblBktPmodeGlbL3:
            offset = ACB_BNK_PER_BKT(acb, 1);
            break;
        default:
            break;
    }

    if (ACB_HLF_BNK_MD(acb) && ALPM_HLF_BNKS(u)) {
        offset = 4;
    }

    info->bkt_info.rofs = rofs - offset;
    for (bnk = 0; bnk < ACB_BNK_PER_BKT(acb, vrf_id); bnk++) {
        soc_field_t bnk_fld;
        bnk_fld = th3_alpm_bnk_fld[bnk+offset];
        switch (bnk_fld) {
            case FMT0f:
                info->bkt_info.bnk_fmt[bnk] = bnk_fmt[0];
                break;
            case FMT1f:
                info->bkt_info.bnk_fmt[bnk] = bnk_fmt[1];
                break;
            case FMT2f:
                info->bkt_info.bnk_fmt[bnk] = bnk_fmt[2];
                break;
            case FMT3f:
                info->bkt_info.bnk_fmt[bnk] = bnk_fmt[3];
                break;
            case FMT4f:
                info->bkt_info.bnk_fmt[bnk] = bnk_fmt[4];
                break;
            case FMT5f:
                info->bkt_info.bnk_fmt[bnk] = bnk_fmt[5];
                break;
            case FMT6f:
                info->bkt_info.bnk_fmt[bnk] = bnk_fmt[6];
                break;
            case FMT7f:
                info->bkt_info.bnk_fmt[bnk] = bnk_fmt[7];
                break;
            default:
                break;
        }

        if (ACB_HAS_RTE(acb, vrf_id) && info->bkt_info.bnk_fmt[bnk] > 0) {
            info->bkt_info.bnk_fmt[bnk] ++;
        }
    }

    return;
}

/* Read and update ALPM prefix key using 1bit length decoding */
int
th3_alpm_bkt_pfx_get(int u, int vrf_id, _alpm_cb_t *acb, void *e, uint32 fmt, uint32 *valid,
                     uint32 *new_key, int *new_len)
{
    int rv = BCM_E_NONE;
    int fmt_len = ACB_FMT_PFX_LEN(acb, vrf_id, fmt);
    uint32 pfx[5] = {0};

    /* process Bucket Level PREFIX */
    soc_format_field_get(u, ACB_FMT_ENT(acb, vrf_id, fmt),
                         e, PREFIXf, pfx);
    *valid = pfx[0] | pfx[1] | pfx[2] | pfx[3] | pfx[4];

    /* pfx with all zero is invalid */
    if (*valid) {
        th3_alpm_ent_1bit_decoding(u, pfx, fmt_len, new_len);
        SHR_BITCOPY_RANGE(new_key, 0, pfx, fmt_len - (*new_len) + 1, *new_len);
    }

    return rv;
}

/* Read ALPM Assoc_Data */
static void
th3_alpm_bkt_adata_get(int u, int vrf_id, int ipt, _alpm_cb_t *acb,
                       void *e, int ent_fmt, _alpm_bkt_adata_t *adata)
{
    uint32 fval[ALPM_MEM_ENT_MAX];
    th3_fmt_ASSOC_DATA_REDUCED_t *fmt_adr = NULL;
    th3_fmt_ASSOC_DATA_FULL_t *fmt_adf = NULL;

    soc_format_field_get(u, ACB_FMT_ENT(acb, vrf_id, ent_fmt),
                         e, ASSOC_DATAf, fval);

    adata->defip_flags = ALPM_IS_IPV4(ipt) ? 0 : BCM_L3_IP6;
    if (ACB_FMT_FULL_TYPE(acb, vrf_id, ent_fmt)) {
        fmt_adf = (th3_fmt_ASSOC_DATA_FULL_t *)fval;
        adata->defip_flags |= fmt_adf->DEST_TYPE ? BCM_L3_MULTIPATH : 0;
        adata->defip_ecmp_index = fmt_adf->DESTINATION;
        adata->defip_flags |= fmt_adf->RPE ? BCM_L3_RPE : 0;
        adata->defip_flags |= fmt_adf->DST_DISCARD ? BCM_L3_DST_DISCARD : 0;
        adata->defip_prio = fmt_adf->PRI;
        adata->defip_lookup_class = fmt_adf->CLASS_ID;
        adata->defip_flex_ctr_base_id = fmt_adf->FLEX_CTR_BASE_COUNTER_IDX;
        adata->defip_flex_ctr_mode = fmt_adf->FLEX_CTR_OFFSET_MODE;
        adata->defip_flex_ctr_pool = fmt_adf->FLEX_CTR_POOL_NUMBER;
    } else {
        fmt_adr = (th3_fmt_ASSOC_DATA_REDUCED_t *)fval;
        adata->defip_flags |= fmt_adr->DEST_TYPE ? BCM_L3_MULTIPATH : 0;
        adata->defip_ecmp_index = fmt_adr->DESTINATION;
        adata->defip_prio = 0;
        adata->defip_lookup_class = 0;
        adata->defip_flex_ctr_base_id = 0;
        adata->defip_flex_ctr_mode = 0;
        adata->defip_flex_ctr_pool = 0;
    }
    adata->defip_acl_class_id = 0;
    adata->defip_acl_class_id_valid = 0;


    return ;
}

static void
th3_alpm_ent_binfo_raw_get(int u, int vrf_id, _alpm_cb_t *acb, void *ent,
                           int fmt, void *binfo_raw)
{
    soc_format_field_get(u, ACB_FMT_ENT(acb, vrf_id, fmt),
                         ent, ALPM2_DATAf, binfo_raw);
}

/* Used for retrieving alpm info from given entry,
 * Entry 'e' can be a TCAM entry or a entry entry, not bank entry */
int
th3_alpm_ent_selective_get(int u, _alpm_cb_t *acb, void *ent, _alpm_ent_info_t *info)
{
    int rv = BCM_E_NONE;
    int ent_fmt, vrf_id, ipt;
    uint32 mask = info->action_mask;

    ent_fmt = info->ent_fmt;
    vrf_id  = info->vrf_id;
    ipt     = info->ipt;

    if (mask & (ALPM_INFO_MASK_VALID | ALPM_INFO_MASK_KEYLEN)) {
        th3_alpm_bkt_pfx_get(u, vrf_id, acb, ent, ent_fmt, &info->ent_valid,
                             info->key, &info->key_len);
    }

    if (mask & ALPM_INFO_MASK_ASSOC_DATA) {
        th3_alpm_bkt_adata_get(u, vrf_id, ipt, acb, ent, ent_fmt, &info->adata);
    }

    if (mask & ALPM_INFO_MASK_ALPM_DATA) {
        /* Here ent is ALPM_DATA/ALPM2_DATA format */
        th3_alpm_ent_binfo_get(u, vrf_id, acb, ent, info);
    }

    if (mask & ALPM_INFO_MASK_ALPM_DATA_RAW) {
        th3_alpm_ent_binfo_raw_get(u, vrf_id, acb, ent, ent_fmt,
                                   info->alpm_data_raw);
    }

    if (mask & ALPM_INFO_MASK_FIXED_DATA) {
        /* For TCAM entry only */;
    }

    return rv;
}

static uint8
_th3_alpm_bkt_def_fmt_get(int u, _alpm_cb_t *acb, int vrf_id, int16 fmt_type, uint8 db_type)
{
    int fmt;

    if (th3_alpm_def_fmt[fmt_type][db_type] == 0) {
        for (fmt = 0; fmt < ALPM_FMT_TP_CNT; fmt++) {
            if (db_type != ACB_FMT_FULL_TYPE(acb, vrf_id, fmt)) {
                continue;
            }

            if (ACB_FMT_BNK(acb, vrf_id, fmt) != INVALIDfmt) {
                break;
            }
        }

        th3_alpm_def_fmt[fmt_type][db_type] = fmt;
    }

    return th3_alpm_def_fmt[fmt_type][db_type];
}

/* Return ALPM bucket bank default (minimum) format according to DB_Type */
uint8
th3_alpm_bkt_def_fmt_get(int u, _alpm_cb_t *acb, int vrf_id, int ipt)
{
    int16 fmt_type = ACB_FMT_TYPE(acb, vrf_id);
    uint8 db_type  = ACB_VRF_DB_TYPE(u, acb, vrf_id, ipt);

    return _th3_alpm_bkt_def_fmt_get(u, acb, vrf_id, fmt_type, db_type);
}

/* Return Reduced/Full format type for a given fmt */
int16
th3_alpm_bkt_fmt_type_get(int u, int vrf_id, _alpm_cb_t *acb, uint8 fmt)
{
    return ACB_FMT_FULL_TYPE(acb, vrf_id, fmt);
}

static int
th3_alpm_table_dump_3level_bkt(int u, int vrf_id, int ipt, _alpm_cb_t *acb,
                               uint32 *key, int key_len,
                               _alpm_bkt_info_t *bkt_info,
                               int lvl_cnt, int flags,
                               int *count)
{
    int i, bnk;
    int idx1, eid;
    int has_next = 1;
    uint32 fmt, valid;

    char ip_str[128] = {0};
    uint32 bkt_ent[ALPM_MEM_ENT_MAX];
    uint32 ftmp[ALPM_MEM_ENT_MAX];
    uint32 fent1[ALPM_MEM_ENT_MAX];

    uint32 hw_key[4];
    int hw_len;

    _alpm_bkt_adata_t adata;
    _alpm_bkt_info_t bkt_info1;
    _bcm_defip_cfg_t lpm_cfg;

    if (lvl_cnt < 1) {
        return BCM_E_NONE;
    }

    if (ACB_HAS_RTE(acb, vrf_id)) {
        has_next = 0;
    }

    for (bnk = 0; bnk < ACB_BNK_PER_BKT(acb, vrf_id); bnk++) {
        fmt = bkt_info->bnk_fmt[bnk];
        if ((fmt == 0) || (fmt > ACB_FMT_CNT(acb, vrf_id))) {
            continue;
        }

        /**** Bucket Level ****/
        idx1 = ALPM_TAB_IDX_GET_BKT_BNK(acb, BI_ROFS(bkt_info),
                                        BI_BKT_IDX(bkt_info), bnk);
        if (flags & ALPM_DUMP_TABLE_NOCACHE) {
            ALPM_IER(alpm_bkt_entry_read_no_cache(u,
                     ACB_BKT_TBL(acb, vrf_id), acb, bkt_ent, idx1));
        } else {
            ALPM_IER(alpm_bkt_entry_read(u,
                     ACB_BKT_TBL(acb, vrf_id), acb, bkt_ent, idx1));
        }
        for (eid = 0; eid < ACB_FMT_ENT_MAX(acb, vrf_id, fmt); eid++) {
            uint32 key2[4] = {0};
            int len2 = 0;
            int phy_idx;

            /* process bucket entry Prefix */
            alpm_util_ent_ent_get(u, vrf_id, acb, bkt_ent, fmt, eid, ftmp);
            alpm_util_bkt_pfx_get(u, vrf_id, acb, ftmp, fmt, key2, &len2, &valid);
            if (!valid) {
                continue;
            }
            alpm_util_bkt_adata_get(u, vrf_id, ipt, acb, ftmp, fmt, &adata, NULL);
            alpm_util_pfx_cat(u, ipt, key, key_len, key2, len2, hw_key, &hw_len);
            sal_memset(&lpm_cfg, 0, sizeof(lpm_cfg));
            alpm_util_key_to_cfg(u, ipt, hw_key, &lpm_cfg);
            if (ALPM_IS_IPV6(ipt)) {
                alpm_util_fmt_ip6addr(ip_str, lpm_cfg.defip_ip6_addr);
            } else {
                alpm_util_fmt_ipaddr(ip_str, lpm_cfg.defip_ip_addr);
            }
            for (i = 0; i <= acb->acb_idx; i++) {
                cli_out("\t");
            }
            phy_idx = th3_mem_phy_index_get(u, acb,
                            ACB_BKT_TBL(acb, vrf_id), idx1);
            cli_out("L%d-[%-5d] | f%-2db%de%-2d %-3d | %s/%d : %s %d %s\n",
                    acb->acb_idx + 2, phy_idx,
                    has_next ? fmt : fmt - 1, bnk, eid,
                    has_next ? 0 : *count, ip_str, hw_len,
                    (adata.defip_flags & BCM_L3_MULTIPATH) ? "ECMP" : "NH",
                    adata.defip_ecmp_index, has_next ? "->" : "" );
            if (!has_next) {
                *count += 1;
                continue;
            }
            alpm_util_ent_data_get(u, vrf_id, ipt, acb, ftmp, fmt, fent1);
            alpm_util_bkt_info_get(u, vrf_id, ipt, ipt, ACB_DWN(u, acb), fent1, 0,
                                   &bkt_info1, NULL, NULL);
            ALPM_IER(th3_alpm_table_dump_3level_bkt(u, vrf_id,
                     ipt, ACB_DWN(u, acb), hw_key, hw_len,
                     &bkt_info1, lvl_cnt - 1, flags, count));
        }
    }

    return BCM_E_NONE;
}

static int
th3_alpm_table_dump_3level_index(int u, int app, soc_mem_t mem,
                                 int idx, int sub_idx, int lvl_cnt, int flags)
{
    int rv;
    int ipt, pkm, ipv6, vrf_id;
    int use_alpm;
    int key_len;
    int count = 0;

    char ip_str[128] = {0};
    uint32 ent[ALPM_MEM_ENT_MAX] = {0};
    uint32 bdata_ent[ALPM_MEM_ENT_MAX];
    uint32 key[4];

    _alpm_bkt_adata_t adata0;
    _alpm_bkt_info_t bkt_info;
    _bcm_defip_cfg_t lpm_cfg;

    if (mem == L3_DEFIP_PAIR_LEVEL1m) {
        pkm = ALPM_PKM_128;
    } else {
        pkm = ALPM_PKM_32B;
    }

    if (flags & ALPM_DUMP_TABLE_NOCACHE) {
        ALPM_IER(tcam_entry_read_no_cache(u, app, pkm, ent, idx, idx));
    } else {
        ALPM_IER(tcam_entry_read(u, app, pkm, ent, idx, idx));
    }

    rv = th3_tcam_entry_mode_get_by_mem(u, mem, ent, NULL, &pkm,
                                        &ipv6, NULL, sub_idx);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (pkm == ALPM_PKM_64B && sub_idx == 1) {
        return BCM_E_NONE;
    }

    ipt = ipv6 ? ALPM_IPT_V6 : ALPM_IPT_V4;

    tcam_entry_vrf_id_get(u, app, pkm, ent, sub_idx, &vrf_id);
    /* Get Pre-pivot key IP_ADDR and prefix length (mask) */
    sal_memset(key, 0, sizeof(key));
    key_len = 0;
    tcam_entry_to_key(u, app, pkm, ent, sub_idx, key);
    tcam_entry_pfx_len_get(u, app, pkm, ent, sub_idx, &key_len);
    tcam_entry_adata_get(u, app, pkm, ent, sub_idx, &adata0);
    tcam_entry_bdata_get(u, app, pkm, ent, sub_idx, bdata_ent);

    use_alpm = !(adata0.defip_flags & BCM_L3_IPMC) &&
               ALPM_VRF_ID_HAS_BKT(u, vrf_id);
    sal_memset(&lpm_cfg, 0, sizeof(lpm_cfg));
    alpm_util_key_to_cfg(u, ipt, key, &lpm_cfg);
    if (ipv6) {
        alpm_util_fmt_ip6addr(ip_str, lpm_cfg.defip_ip6_addr);
    } else {
        alpm_util_fmt_ipaddr(ip_str, lpm_cfg.defip_ip_addr);
    }

    if (ALPM_VRF_ID_IS_GBL(u, vrf_id)) {
        cli_out("L1-[%4d.%d] | vrf-%-4s ", idx, sub_idx,
            vrf_id == ALPM_VRF_ID_GHI(u) ? "GHi" : "GLo");
    } else {
        cli_out("L1-[%4d.%d] | vrf-%-4d ", idx, sub_idx, vrf_id);
    }
    cli_out("| %s/%d : %s %d %s\n", ip_str, key_len,
            (adata0.defip_flags & BCM_L3_MULTIPATH) ? "ECMP" : "NH",
            adata0.defip_ecmp_index, use_alpm ? "->" : "" );
    if (!use_alpm) {
        rv = BCM_E_NONE;
        goto end;
    }

    /* process ALPM_DATA level recursively from top to bottom */
    alpm_util_bkt_info_get(u, vrf_id, ipt, pkm, ACB_TOP(u, app), bdata_ent,
                           sub_idx, &bkt_info, NULL, NULL);
    th3_alpm_table_dump_3level_bkt(u, vrf_id, ipt, ACB_TOP(u, app),
                                   key, key_len, &bkt_info,
                                   lvl_cnt - 1, flags, &count);
end:
    return rv;
}

int
th3_alpm_table_dump_3level(int u, int app, soc_mem_t mem,
                           int idx, int sub_idx, int lvl_cnt, int flags)
{
    int rv = BCM_E_NONE;
    int i, j, idx_min, idx_max, sub_idx_min, sub_idx_max;
    int pkm;

    if (idx < 0) {
        idx_min = 0;
        if (mem == L3_DEFIP_LEVEL1m) {
            pkm = ALPM_PKM_32B;
        } else {
            pkm = ALPM_PKM_128;
        }
        idx_max = tcam_table_size(u, app, pkm) - 1;
        sub_idx_min = 0;
        sub_idx_max = (pkm == ALPM_PKM_32B) ? 1 : 0;
    } else {
        idx_min = idx_max = idx;
        sub_idx_min = sub_idx_max = (sub_idx < 0) ? 0 : sub_idx;
    }

    /* Levels are going to be printed */
    if (lvl_cnt < 1 || lvl_cnt > 3) {
        lvl_cnt = 3;
    }

    for (i = idx_min; i <= idx_max; i++) {
        for (j = sub_idx_min; j <= sub_idx_max; j++) {
            rv = th3_alpm_table_dump_3level_index(u, app, mem, i, j, lvl_cnt, flags);
            if (BCM_FAILURE(rv)) {
                if (rv == BCM_E_NOT_FOUND) {
                    if (idx_min == idx_max) {
                        cli_out("Entry %d.%d is invalid\n", i, j);
                    }
                    continue;
                }
                break;
            }
        }
    }

    return rv;
}

/*
 * Function:
 *      th3_alpm_table_dump
 * Purpose:
 *      Dump TH3 ALPM Level2 and Level3 memory tables:
 *      "alpm table dump [raw|chg|all|nocache] <mem> <index> <format> [<ent>]"
 * Parameters:
 *      u       - Device unit #
 *      mem     - ALPM Level2 or Level3 memory table
 *      copyno  - mem table copy number
 *      index   - ALPM bucket level logic table index
 *      fmt     - Bank format (1-8 for Pivot formats, 1-12 for Route formats)
 *      ent_idx - entry index (0-14), -1 for all entries.
 *      flags   - Dump flags (raw, chg, all, nocache)
 * Returns:
 *      BCM_E_XXX
 */
int
th3_alpm_table_dump(int u, int app, int vrf_id, soc_mem_t mem, int copyno, int index,
                    int fmt, int ent_idx, int flags)
{
    int     rv;
    int     i, ent, ent_min, ent_max;
    int     f, f1, entry_dw;
    int     first_print_flag = 0;
    int     vertical = 0;
    uint32  entry[ALPM_MEM_ENT_MAX];
    uint32  fent[ALPM_MEM_ENT_MAX];
    uint32  fval[ALPM_MEM_ENT_MAX];
    uint32  fval1[ALPM_MEM_ENT_MAX];
    uint32  nfval[ALPM_MEM_ENT_MAX];
    char    ctmp[ALPM_MEM_ENT_MAX * 8 + 3];
    char    mem_prefix[256];
    _alpm_cb_t *acb = (mem == L3_DEFIP_ALPM_LEVEL2m ?
                       ACB_TOP(u, app) : ACB_VRF_BTM(u, vrf_id, app));
    soc_field_info_t *fieldp;
    soc_field_info_t *fieldp1;
    soc_format_info_t *fmtp;
    soc_format_info_t *fmtp1;
    soc_format_t tfmt;
    soc_format_t tfmt1;

    if (mem == L3_DEFIP_LEVEL1m || mem == L3_DEFIP_PAIR_LEVEL1m) {
        /* Use fmt to store Level count */
        return th3_alpm_table_dump_3level(u, app, mem, index, ent_idx, fmt, flags);
    }

    if ((fmt <= 0) || (fmt > ACB_FMT_CNT(acb, vrf_id))) {
        cli_out("invalid or disabled bank format %d\n", fmt);
        return BCM_E_INTERNAL;
    }
    tfmt = ACB_FMT_ENT(acb, vrf_id, fmt);

    if (ent_idx == -1) { /* all entries */
        ent_min = 0;
        ent_max = ACB_FMT_ENT_MAX(acb, vrf_id, fmt);
    } else if (ent_idx >= ACB_FMT_ENT_MAX(acb, vrf_id, fmt)) {
        cli_out("invalid entry idx %d\n", ent_idx);
        return BCM_E_INTERNAL;
    } else { /* one specific entry */
        ent_min = ent_idx;
        ent_max = ent_idx + 1;
    }

    sal_memset(nfval, 0, sizeof(nfval));

    if (flags & ALPM_DUMP_TABLE_NOCACHE) {
        rv = soc_mem_read_no_cache(u, 0, mem, 0, MEM_BLOCK_ANY, index, entry);
    } else {
        rv = soc_mem_read(u, mem, MEM_BLOCK_ANY, index, entry);
    }

    if (BCM_FAILURE(rv)) {
        cli_out("Read ERROR: table %s.%s[%d]: %s\n",
                SOC_MEM_UFNAME(u, mem),
                SOC_BLOCK_NAME(u, copyno), index, soc_errmsg(rv));
        return BCM_E_INTERNAL;
    }

    for (ent = ent_min; ent < ent_max; ent++) {
        soc_format_field_get(u, ACB_FMT_BNK(acb, vrf_id, fmt), entry,
                             th3_alpm_ent_fld[ent], fent);
        fmtp = &SOC_FORMAT_INFO(u, tfmt);

        sal_sprintf(mem_prefix, "%s.%s[%d.%d]: ", SOC_MEM_UFNAME(u, mem),
                    SOC_BLOCK_NAME(u, copyno), index, ent);

        if (flags & ALPM_DUMP_TABLE_RAW) {
            cli_out("%s", mem_prefix);
            entry_dw = BITS2WORDS(fmtp->bits);
            for (i = 0; i < entry_dw; i++) {
                cli_out("0x%08x ", fent[i]);
            }
            cli_out("\n");
        } else {
            /* for fields in entry format */
            for (f = fmtp->nFields - 1; f >= 0; f--) {
                fieldp = &fmtp->fields[f];
                sal_memset(fval, 0, ALPM_MEM_ENT_MAX * sizeof(uint32));
                soc_format_field_get(u, tfmt, fent, fieldp->field, fval);

                if (flags & ALPM_DUMP_TABLE_CHANGED) {
                    if (sal_memcmp(fval, nfval, ALPM_MEM_ENT_MAX *
                                   sizeof(uint32)) == 0) {
                        continue;
                    }
                }

                /* for fields in format-in-format (ALPM2_DATA or ASSOC_DATA) */
                tfmt1 = INVALIDfmt;
                if (fieldp->field == ALPM2_DATAf) {
                    tfmt1 = ALPM2_DATAfmt;
                } else if (fieldp->field == ASSOC_DATAf) {
                    tfmt1 = ACB_FMT_FULL_TYPE(acb, vrf_id, fmt) ?
                            ASSOC_DATA_FULLfmt : ASSOC_DATA_REDUCEDfmt;
                }
                if (tfmt1 != INVALIDfmt) {
                    fmtp1 = &SOC_FORMAT_INFO(u, tfmt1);
                    for (f1 = fmtp1->nFields - 1; f1 >= 0; f1--) {
                        fieldp1 = &fmtp1->fields[f1];
                        sal_memset(fval1, 0, ALPM_MEM_ENT_MAX * sizeof(uint32));
                        soc_format_field_get(u, tfmt1, fval, fieldp1->field, fval1);

                        if (flags & ALPM_DUMP_TABLE_CHANGED) {
                            if (sal_memcmp(fval1, nfval, ALPM_MEM_ENT_MAX *
                                           sizeof(uint32)) == 0) {
                                continue;
                            }
                        }
                        _shr_format_long_integer(ctmp, fval1, BITS2BYTES(fieldp1->len));

                        if (first_print_flag == 0) {
                            cli_out("%s%s", mem_prefix, vertical ? "" : "<");
                            first_print_flag=1;
                        }
                        if (vertical) {
                            cli_out("\n\t%30s: %s", SOC_FIELD_NAME(u, fieldp1->field), ctmp);
                        } else {
                            cli_out("%s=%s,", SOC_FIELD_NAME(u, fieldp1->field), ctmp);
                        }
                    } /* for f1 */
                }

                _shr_format_long_integer(ctmp, fval, BITS2BYTES(fieldp->len));

                if (first_print_flag == 0) {
                    cli_out("%s%s", mem_prefix, vertical ? "" : "<");
                    first_print_flag=1;
                }
                if (vertical) {
                    cli_out("\n\t%30s: %s", SOC_FIELD_NAME(u, fieldp->field), ctmp);
                } else {
                    cli_out("%s=%s%s", SOC_FIELD_NAME(u, fieldp->field),
                            ctmp, f > 0 ? "," : "");
                }
            } /* for f */

            if (first_print_flag == 1) {
                cli_out("%s\n", vertical ? "" : ">");
                first_print_flag = 0;
            }
        }
    } /* for ent */

    return BCM_E_NONE;
}

/*
 * Function:
 *      th3_alpm_cap_min
 * Purpose:
 *      Get the min capacity for ALPM routes from PKM
 * Parameters:
 *      u           - Device unit
 *      pkm         - Packing mode
 */
int
th3_alpm_cap_min(int u, int pkm, int app)
{
    int tcam_ent = 0;
    int min_bkt_cnt, min_bkt_ent;
    int i, fmt, vrf_id;
    _alpm_bkt_pool_conf_t *bp_conf;
    _alpm_cb_t *acb;

    /* Start from TCAM restriction */
    switch (pkm) {
    case ALPM_PKM_32B:
        tcam_ent = th3_alpm_mem_size(u, L3_DEFIP_LEVEL1m) * 2;
        break;
    case ALPM_PKM_64B:
        tcam_ent = th3_alpm_mem_size(u, L3_DEFIP_LEVEL1m);
        break;
    case ALPM_PKM_128:
        tcam_ent = th3_alpm_mem_size(u, L3_DEFIP_PAIR_LEVEL1m);
        break;
    default:
        return 0;
    }

    min_bkt_ent = (ALPM_TCAM_ZONED(u, app) ? tcam_ent / 2 : tcam_ent);

    /* Consider only for private DB */
    vrf_id = 1;
    for (i = 0; i < ACB_CNT(u); i++) {
        int avl_bnks, min_bnks;
        acb = ACB(u, i, app);
        bp_conf = ACB_BKT_POOL(acb, ALPM_VRF_ID_TO_BPC_PID(u, vrf_id, app));

        /* avl_bnks starts with all, then check ALPM_BKT_RSVD */
        avl_bnks = BPC_BNK_CNT(bp_conf);
        if (ALPM_128B(u) && ALPM_BKT_RSVD(u)) {
            int rsvd_v6_bnks =
                BPC_BNK_CNT(bp_conf) * ALPM_BKT_RSVD_CNT(u) /
                ALPM_BKT_RSVD_TOTAL;
            if (pkm == ALPM_PKM_128) {
                avl_bnks = rsvd_v6_bnks;
            } else {
                avl_bnks -= rsvd_v6_bnks;
            }
        }

        /* compare upper min_bkt_ent and bkt_cnt at current level */
        min_bkt_cnt = (ACB_BKT_CNT(acb) < min_bkt_ent ? ACB_BKT_CNT(acb) : min_bkt_ent);
        min_bnks = min_bkt_cnt * BPC_BNK_PER_BKT(bp_conf);

        /* update avl_bnks comapring to min_bnks */
        if (min_bnks < avl_bnks ) {
            avl_bnks = min_bnks;
        }

        if (ACB_HAS_RTE(acb, vrf_id)) {
            int v4fmt, v6fmt64, v6fmt128;
            v4fmt = _th3_alpm_bkt_def_fmt_get(u, acb, vrf_id, ALPM_FMT_RTE, 1); /* FMT10 (33b) */
            
            v6fmt64 = v4fmt + 1;
            v6fmt128 = v6fmt64 + 1;
            /* Route (full) format whose prefix length fits almost the full PKM length */
            fmt = (pkm == ALPM_PKM_32B ? v4fmt :
                   pkm == ALPM_PKM_64B ? v6fmt64 : v6fmt128);  /* FMT11 (73b), FMT12 (128b) */
        } else {
            /* Pivot (full) format whose prefix length
               fits almost the remaining pivot length */
            fmt = _th3_alpm_bkt_def_fmt_get(u, acb, vrf_id, ALPM_FMT_PVT, 1); /* FMT6 (17b) */
        }

        /* considering 33% on bucket split utlization */
        min_bkt_ent = avl_bnks * ACB_FMT_ENT_MAX(acb, vrf_id, fmt) / 3;
    }

    return min_bkt_ent;
}

/*
 * Function:
 *      th3_alpm_cap_max
 * Purpose:
 *      Get the max capacity for ALPM routes from PKM
 * Parameters:
 *      u           - Device unit
 *      pkm         - Packing mode
 */
int
th3_alpm_cap_max(int u, int pkm, int app)
{
    int tcam_ent = 0;
    int min_bkt_cnt, max_bkt_ent;
    int i, fmt, vrf_id, pid;
    _alpm_bkt_pool_conf_t *bp_conf;
    _alpm_cb_t *acb;

    /* Start from TCAM restriction */
    switch (pkm) {
    case ALPM_PKM_32B:
        tcam_ent = th3_alpm_mem_size(u, L3_DEFIP_LEVEL1m) * 2;
        break;
    case ALPM_PKM_64B:
        tcam_ent = th3_alpm_mem_size(u, L3_DEFIP_LEVEL1m);
        break;
    case ALPM_PKM_128:
        tcam_ent = th3_alpm_mem_size(u, L3_DEFIP_PAIR_LEVEL1m);
        break;
    default:
        return 0;
    }

    /* Include private/global ALPM routes (exclude TCAM direct routes) */
    /* TCAM mode private VRF has half TCAM entries */
    max_bkt_ent = (ALPM_MODE_CHK(u, BCM_ALPM_MODE_TCAM_ALPM) ? tcam_ent / 2 : tcam_ent);

    for (i = 0; i < ACB_CNT(u); i++) {
        int avl_bnks, min_bnks;
        acb = ACB(u, i, app);
        /* compare upper max_bkt_ent and bkt_cnt at current level */
        min_bkt_cnt = (ACB_BKT_CNT(acb) < max_bkt_ent ? ACB_BKT_CNT(acb) : max_bkt_ent);

        max_bkt_ent = 0;
        /* consider both private and global routes */
        for (pid = 0; pid < ALPM_BKT_PID_CNT; pid++) {
            bp_conf = ACB_BKT_POOL(acb, pid);
            vrf_id = ALPM_BPC_PID_TO_VRF_ID(u, pid);

            if (pid > 0 && bp_conf == ACB_BKT_POOL(acb, pid - 1)) {
                continue; /* Combined Mode uses PID=0 only */
            }

            /* avl_bnks starts with all, then check ALPM_BKT_RSVD */
            avl_bnks = BPC_BNK_CNT(bp_conf);
            if (ALPM_128B(u) && ALPM_BKT_RSVD(u)) {
                int rsvd_v6_bnks =
                    BPC_BNK_CNT(bp_conf) * ALPM_BKT_RSVD_CNT(u) /
                    ALPM_BKT_RSVD_TOTAL;
                if (pkm == ALPM_PKM_128) {
                    avl_bnks = rsvd_v6_bnks;
                } else {
                    avl_bnks -= rsvd_v6_bnks;
                }
            }

            min_bnks = min_bkt_cnt * BPC_BNK_PER_BKT(bp_conf);

            /* update avl_bnks comapring to min_bnks */
            if (min_bnks < avl_bnks ) {
                avl_bnks = min_bnks;
            }

            /* Route or Pivot (reduced) FMT1 with maximum entries per bank */
            fmt = _th3_alpm_bkt_def_fmt_get(u, acb, vrf_id, ACB_FMT_TYPE(acb, vrf_id), 0);
            max_bkt_ent += avl_bnks * ACB_FMT_ENT_MAX(acb, vrf_id, fmt);
        }
    }

    return max_bkt_ent;
}

/*
 * Function:
 *      th3_alpm_cap_get
 * Purpose:
 *      Get the min or max capacity for ALPM routes from legacy tables:
 * L3_DEFIP_ALPM_IPV4, L3_DEFIP_ALPM_IPV6_64, L3_DEFIP_ALPM_IPV6_128
 *
 * Parameters:
 *      u           - Device unit
 *      mem         - Legacy memory type
 *      max_cnt     - Maximum result returned.
 *      min_cnt     - Minimum result returned.
 */
int
th3_alpm_cap_get(int u, soc_mem_t mem, int *max_cnt, int *min_cnt, int app)
{
    int pkm;
    switch (mem) {
        case L3_DEFIP_ALPM_IPV4m:
        case L3_DEFIP_ALPM_IPV4_1m:
            pkm = ALPM_PKM_32B;
            break;
        case L3_DEFIP_ALPM_IPV6_64m:
        case L3_DEFIP_ALPM_IPV6_64_1m:
            pkm = ALPM_PKM_64B;
            break;
    case L3_DEFIP_ALPM_IPV6_128m:
            pkm = ALPM_PKM_128;
            break;
        default:
            return BCM_E_UNAVAIL;
    }

    if (min_cnt != NULL) {
        *min_cnt = th3_alpm_cap_min(u, pkm, app);
    }

    if (max_cnt != NULL) {
        *max_cnt = th3_alpm_cap_max(u, pkm, app);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      th3_alpm_bkt_hit_get
 * Purpose:
 *      Get ALPM bucket entry hitbit status from hitbit table cache
 *      sync with Hw. Route hitbit status is gathered from pivots
 *      whose BPM is the route.
 * Parameters:
 *      u       - Device unit #
 *      vrf_id  - VRF ID for logic hit table PID in Parallel/Mixed Mode
 *      acb     - ALPM control block where hit table located
 *      idx     - ALPM bucket level logic table index
 *      ent     - Bucket entry ID (0-15)
 * Returns:
 *      Bucket entry hitbit (0 or 1)
 */
int
th3_alpm_bkt_hit_get(int u, int vrf_id, _alpm_cb_t *acb,
                     _alpm_tbl_t tbl, int idx, int ent)
{
    int rv;
    uint32 *tbl_ent; /* bucket hit cache table entry pointer */
    int i = ACB_IDX(acb);
    int pid = ACB_BKT_VRF_PID(acb, vrf_id);
    soc_mem_t mem = ALPM_HTBL_MEM(u, i, pid);
    soc_field_t hit_fld[16] = {
        HIT_0f, HIT_1f, HIT_2f,  HIT_3f,  HIT_4f,  HIT_5f,  HIT_6f,  HIT_7f,
        HIT_8f, HIT_9f, HIT_10f, HIT_11f, HIT_12f, HIT_13f, HIT_14f, HIT_15f
    };

    tbl_ent = soc_mem_table_idx_to_pointer(u, mem, uint32 *,
                                           ALPM_HTBL_CACHE(u, i, pid), idx);
    rv = soc_mem_field32_get(u, mem, tbl_ent, hit_fld[ent]);

    return rv;
}

/*
 * Function:
 *      th3_alpm_bkt_hit_set
 * Purpose:
 *      Set ALPM bucket entry hitbit to cache table only
 * Parameters:
 *      u       - Device unit #.
 *      vrf_id  - VRF ID for logic hit table PID in Parallel/Mixed Mode
 *      acb     - ALPM control block where hit table located
 *      idx     - ALPM bucket level table index
 *      ent     - Bucket entry ID (0-15)
 *      hit_val - Bucket entry hit value (0/1)
 * Returns:
 *      BCM_E_XXX
 */
int
th3_alpm_bkt_hit_set(int u, int vrf_id, _alpm_cb_t *acb, _alpm_tbl_t tbl,
                     int idx, int ent, int hit_val)
{
    int rv = BCM_E_NONE;
    uint32 *tbl_ent; /* bucket hit cache table entry pointer */
    int i = ACB_IDX(acb);
    int pid = ACB_BKT_VRF_PID(acb, vrf_id);
    soc_mem_t mem = ALPM_HTBL_MEM(u, i, pid);
    soc_field_t hit_fld[16] = {
        HIT_0f, HIT_1f, HIT_2f,  HIT_3f,  HIT_4f,  HIT_5f,  HIT_6f,  HIT_7f,
        HIT_8f, HIT_9f, HIT_10f, HIT_11f, HIT_12f, HIT_13f, HIT_14f, HIT_15f
    };

    tbl_ent = soc_mem_table_idx_to_pointer(u, mem, uint32 *,
                                           ALPM_HTBL_CACHE(u, i, pid), idx);
    soc_mem_field32_set(u, mem, tbl_ent, hit_fld[ent], hit_val);

    return rv;
}

/*
 * Function:
 *      th3_alpm_bkt_hit_write
 * Purpose:
 *      Set ALPM bucket entry hitbit status to hitbit table cache
 *      and write to Hw (by setting Entry Move & Mask to trigger write to Hw
 *      in next Dist Hitbit thread clycle).
 * Parameters:
 *      u       - Device unit #
 *      vrf_id  - VRF ID for logic hit table PID in Parallel/Mixed Mode
 *      acb     - ALPM control block where hit table located
 *      ent_idx - ALPM bucket node ent_idx
 *      hit_val - HITBIT value requested
 * Returns:
 *      BCM_E_XXX
 */
int
th3_alpm_bkt_hit_write(int u, int vrf_id, _alpm_cb_t *acb,
                       _alpm_tbl_t tbl, int ent_idx, int hit_val)
{
    int rv = BCM_E_NONE;
    int idx, ent;
    uint32 *dst_ent; /* bucket hit cache table entry pointer */
    int i = ACB_IDX(acb);
    int pid = ACB_BKT_VRF_PID(acb, vrf_id);
    soc_mem_t mem = ALPM_HTBL_MEM(u, i, pid);
    soc_field_t hit_fld[16] = {
        HIT_0f, HIT_1f, HIT_2f,  HIT_3f,  HIT_4f,  HIT_5f,  HIT_6f,  HIT_7f,
        HIT_8f, HIT_9f, HIT_10f, HIT_11f, HIT_12f, HIT_13f, HIT_14f, HIT_15f
    };

    ALPM_HIT_LOCK(u);

    ent = ALPM_IDX_TO_ENT(ent_idx);
    idx = ALPM_TAB_IDX_GET(ent_idx);

    /* Write hit_val to dst cache hit bit */
    dst_ent = soc_mem_table_idx_to_pointer(u, mem, void *,
                                           ALPM_HTBL_CACHE(u, i, pid), idx);
    soc_mem_field32_set(u, mem, dst_ent, hit_fld[ent], hit_val);

    /* Write hit_val to dst move bit */
    dst_ent = soc_mem_table_idx_to_pointer(u, mem, void *,
                                           ALPM_HTBL_MOVE(u, i, pid), idx);
    soc_mem_field32_set(u, mem, dst_ent, hit_fld[ent], hit_val);

    /* Set dst move mask bit */
    dst_ent = soc_mem_table_idx_to_pointer(u, mem, void *,
                                      ALPM_HTBL_MOVE_MASK(u, i, pid), idx);
    soc_mem_field32_set(u, mem, dst_ent, hit_fld[ent], 1);

    /* declare finally hitbit entry_moved flag */
    ALPM_HTBL_ENT_MOVED(u, i, pid) = TRUE;

    ALPM_HIT_UNLOCK(u);

    return rv;
}

/* ALPM bucket hitbit entry move from bkt_node's src_ent_idx to dst_ent_idx.
   Copy hit bit cache, and set properly move, move_mask and ent_moved flag.
 */
int
th3_alpm_ent_hit_move(int u, int vrf_id, _alpm_cb_t *acb, int src_ent_idx, int dst_ent_idx)
{
    int rv = BCM_E_NONE;
    int i, hit_val, idx, ent;
    uint32 *src_ent, *dst_ent;
    int pid = ACB_BKT_VRF_PID(acb, vrf_id);
    soc_mem_t mem;
    soc_field_t hit_fld[16] = {
        HIT_0f, HIT_1f, HIT_2f,  HIT_3f,  HIT_4f,  HIT_5f,  HIT_6f,  HIT_7f,
        HIT_8f, HIT_9f, HIT_10f, HIT_11f, HIT_12f, HIT_13f, HIT_14f, HIT_15f
    };

    ALPM_HIT_LOCK(u);

    i = ACB_IDX(acb);
    mem = ALPM_HTBL_MEM(u, i, pid);

    /* 1) Src */
    ent = ALPM_IDX_TO_ENT(src_ent_idx);
    idx = ALPM_TAB_IDX_GET(src_ent_idx);

    /* Save src cache hit_val and clear it */
    src_ent = soc_mem_table_idx_to_pointer(u, mem, void *,
                                           ALPM_HTBL_CACHE(u, i, pid), idx);
    hit_val = soc_mem_field32_get(u, mem, src_ent, hit_fld[ent]);
    soc_mem_field32_set(u, mem, src_ent, hit_fld[ent], 0);

    /* Clear src move bit */
    src_ent = soc_mem_table_idx_to_pointer(u, mem, void *,
                                           ALPM_HTBL_MOVE(u, i, pid), idx);
    soc_mem_field32_set(u, mem, src_ent, hit_fld[ent], 0);

    /* Set src move mask bit */
    src_ent = soc_mem_table_idx_to_pointer(u, mem, void *,
                                           ALPM_HTBL_MOVE_MASK(u, i, pid), idx);
    soc_mem_field32_set(u, mem, src_ent, hit_fld[ent], 1);

    /* 2) Dst */
    ent = ALPM_IDX_TO_ENT(dst_ent_idx);
    idx = ALPM_TAB_IDX_GET(dst_ent_idx);

    /* Write hit_val to dst cache hit bit */
    dst_ent = soc_mem_table_idx_to_pointer(u, mem, void *,
                                           ALPM_HTBL_CACHE(u, i, pid), idx);
    soc_mem_field32_set(u, mem, dst_ent, hit_fld[ent], hit_val);

    /* Write hit_val to dst move bit */
    dst_ent = soc_mem_table_idx_to_pointer(u, mem, void *,
                                           ALPM_HTBL_MOVE(u, i, pid), idx);
    soc_mem_field32_set(u, mem, dst_ent, hit_fld[ent], hit_val);

    /* Set dst move mask bit */
    dst_ent = soc_mem_table_idx_to_pointer(u, mem, void *,
                                      ALPM_HTBL_MOVE_MASK(u, i, pid), idx);
    soc_mem_field32_set(u, mem, dst_ent, hit_fld[ent], 1);

    /* declare finally hitbit entry_moved flag */
    ALPM_HTBL_ENT_MOVED(u, i, pid) = TRUE;

    ALPM_HIT_UNLOCK(u);

    return rv;
}

/* ALPM bucket bank hitbit move from src_idx to dst_idx.
   Copy hit bit cache, and set properly move, move_mask and ent_moved flag.
 */
int
th3_alpm_bnk_hit_move(int u, int vrf_id, _alpm_cb_t *acb, int src_idx, int dst_idx)
{
    int rv = BCM_E_NONE;
    int i;
    uint32 src_cache;
    uint32 *cache_ent, *move_ent, *move_mask_ent;
    int pid = ACB_BKT_VRF_PID(acb, vrf_id);
    soc_mem_t mem;

    ALPM_HIT_LOCK(u);

    i = ACB_IDX(acb);
    mem = ALPM_HTBL_MEM(u, i, pid);

    /* 1) Src bank */
    cache_ent = soc_mem_table_idx_to_pointer(u, mem, void *,
                                         ALPM_HTBL_CACHE(u, i, pid), src_idx);
    move_ent = soc_mem_table_idx_to_pointer(u, mem, void *,
                                          ALPM_HTBL_MOVE(u, i, pid), src_idx);
    move_mask_ent = soc_mem_table_idx_to_pointer(u, mem, void *,
                                     ALPM_HTBL_MOVE_MASK(u, i, pid), src_idx);
    src_cache = *cache_ent; /* save src_cache value */
    *cache_ent = 0; /* clear src cache */
    *move_ent = 0; /* clear src move */
    *move_mask_ent = ALPM_MAX_HIT_ENT_MASK; /* Set src mask for all 16 bits */

    /* 2) Dst bank */
    cache_ent = soc_mem_table_idx_to_pointer(u, mem, void *,
                                         ALPM_HTBL_CACHE(u, i, pid), dst_idx);
    move_ent = soc_mem_table_idx_to_pointer(u, mem, void *,
                                          ALPM_HTBL_MOVE(u, i, pid), dst_idx);
    move_mask_ent = soc_mem_table_idx_to_pointer(u, mem, void *,
                                     ALPM_HTBL_MOVE_MASK(u, i, pid), dst_idx);
    *cache_ent = src_cache; /* copy saved src_cache value to dst cache */
    *move_ent = src_cache; /* copy same to dst move */
    *move_mask_ent = ALPM_MAX_HIT_ENT_MASK; /* Set dst mask for all 16 bits */

    /* declare finally hitbit entry_moved flag */
    ALPM_HTBL_ENT_MOVED(u, i, pid) = TRUE;

    ALPM_HIT_UNLOCK(u);
    return rv;
}

/* Clear ALPM bucket bank hitbit cache at idx.
   Set properly move, move_mask and ent_moved flag. */
#if 0
int
th3_alpm_bnk_hit_clear(int u, int vrf_id, _alpm_cb_t *acb, int idx)
{
    int rv = BCM_E_NONE;
    int i;
    uint32 *entry;
    int pid = ACB_BKT_VRF_PID(acb, vrf_id);
    soc_mem_t mem;

    ALPM_HIT_LOCK(u);

    i = ACB_IDX(acb);
    mem = ALPM_HTBL_MEM(u, i, pid);

    entry = soc_mem_table_idx_to_pointer(u, mem, void *,
                                         ALPM_HTBL_CACHE(u, i, pid), idx);
    *entry = 0; /* clear cache */

    entry = soc_mem_table_idx_to_pointer(u, mem, void *,
                                         ALPM_HTBL_MOVE(u, i, pid), idx);
    *entry = 0; /* clear move */

    entry = soc_mem_table_idx_to_pointer(u, mem, void *,
                                         ALPM_HTBL_MOVE_MASK(u, i, pid), idx);
    *entry = ALPM_MAX_HIT_ENT_MASK; /* set move mask */

    /* declare finally hitbit entry_moved flag */
    ALPM_HTBL_ENT_MOVED(u, i, pid) = TRUE;

    ALPM_HIT_UNLOCK(u);
    return rv;
}
#endif

/* Update hitbit for entry move on level i by
 * hit_cache = (hit_cache & ~hit_move_mask) | (hit_move & hit_move_mask)
 */
int
th3_alpm_hit_move_update(int u, int i, int pid)
{
    int rv = BCM_E_NONE;
    int idx, index_cnt;
    uint32 hit_cache, hit_move, hit_move_mask;
    uint32 *cache_ent, *move_ent, *move_mask_ent;
    soc_mem_t mem;

    if (!ALPM_HTBL_ENT_MOVED(u, i, pid)) {
        return rv;
    }

    mem = ALPM_HTBL_MEM(u, i, pid);
    index_cnt = ALPM_HTBL_IDX_CNT(u, i, pid);

    ALPM_HTBL_ENT_MOVED_CNT(u, i, pid) = 0;
    for (idx = 0; idx < index_cnt; idx++) {
        move_mask_ent = soc_mem_table_idx_to_pointer(u, mem, uint32 *,
                                    ALPM_HTBL_MOVE_MASK(u, i, pid), idx);
        hit_move_mask = *move_mask_ent;

        if (hit_move_mask) {
            cache_ent = soc_mem_table_idx_to_pointer(u, mem, uint32 * ,
                                        ALPM_HTBL_CACHE(u, i, pid), idx);
            move_ent = soc_mem_table_idx_to_pointer(u, mem, uint32 *,
                                        ALPM_HTBL_MOVE(u, i, pid), idx);
            hit_cache = *cache_ent;
            hit_move =  *move_ent;

            /* update cache entry */
            *cache_ent = (hit_cache & ~hit_move_mask) |
                            (hit_move & hit_move_mask);

            ALPM_HTBL_ENT_MOVED_CNT(u, i, pid)++;
        }
    }

    return rv;
}

/* Sync ALPM non-TCAM level (i:0/1 for level 2/3) hitbit cache with Hw */
int
th3_alpm_hit_cache_sync(int u, int app, int i, int pid)
{
    soc_mem_t mem;
    int idx_max, offset = 0;
    int rv = BCM_E_NONE;
    _alpm_cb_t *acb = (i == 0 ? ACB_TOP(u, app) : ACB_BTM(u, app));

    if (ALPM_HTBL_IDX_CNT(u, i, pid) == 0) {
        return rv;
    }

    mem = ALPM_HTBL_MEM(u, i, pid);
    idx_max = ALPM_HTBL_IDX_CNT(u, i, pid) - 1;

    if (pid == ALPM_BKT_GLB_PID) { /* adjust physical mem table offset */
        offset = ALPM_HTBL_IDX_CNT(u, i, ALPM_BKT_PRT_PID);
    }

    if (ACB_HLF_BNK_MD(acb) && ALPM_HLF_BNKS(u)) {
        offset = ALPM_HTBL_IDX_CNT(u, i, ALPM_BKT_PRT_PID);
    }

    rv = soc_mem_read_range(u, mem, MEM_BLOCK_ANY, offset, idx_max + offset,
                            ALPM_HTBL_CACHE(u, i, pid));

    return rv;
}

/* Write ALPM non-TCAM level (i:0/1 for level 2/3) hitbit cache to Hw */
int
th3_alpm_hit_hw_write(int u, int app, int i, int pid)
{
    soc_mem_t mem;
    int idx, idx_max, index, offset = 0;
    uint32 hit_move, hit_move_mask, entry_data;
    uint32 *cache_ent, *move_ent, *move_mask_ent;
    int rv = BCM_E_NONE;
    _alpm_cb_t *acb = (i == 0 ? ACB_TOP(u, app) : ACB_BTM(u, app));

    if (ALPM_HTBL_IDX_CNT(u, i, pid) == 0) {
        return rv;
    }

    mem = ALPM_HTBL_MEM(u, i, pid);
    idx_max = ALPM_HTBL_IDX_CNT(u, i, pid) - 1;

    if (pid == ALPM_BKT_GLB_PID) { /* adjust physical mem table offset */
        offset = ALPM_HTBL_IDX_CNT(u, i, ALPM_BKT_PRT_PID);
    }

    if (ACB_HLF_BNK_MD(acb) && ALPM_HLF_BNKS(u)) {
        offset = ALPM_HTBL_IDX_CNT(u, i, ALPM_BKT_PRT_PID);
    }

    /* use DMA write for whole HIT table */
    if (ALPM_HTBL_ENT_MOVED_CNT(u, i, pid) >= ALPM_HIT_DMA_THRESHOLD) {

        rv = soc_mem_write_range(u, mem, MEM_BLOCK_ALL, offset, idx_max + offset,
                                 ALPM_HTBL_CACHE(u, i, pid));
    } else { /* use HIT mem_read and mem_write per moved entry */

        for (idx = 0; idx <= idx_max; idx++) {

            move_mask_ent = soc_mem_table_idx_to_pointer(u, mem, uint32 *,
                                        ALPM_HTBL_MOVE_MASK(u, i, pid), idx);
            hit_move_mask = *move_mask_ent;

            if (hit_move_mask) {
                move_ent = soc_mem_table_idx_to_pointer(u, mem, uint32 *,
                                            ALPM_HTBL_MOVE(u, i, pid), idx);
                hit_move =  *move_ent;

                index = idx + offset; /* HIT entry physical index */
                /* read from Hw HIT entry data */
                rv = soc_mem_read_no_cache(u, 0, mem, 0, MEM_BLOCK_ANY,
                                           index, (void *)&entry_data);
                entry_data = (entry_data & ~hit_move_mask) |
                                (hit_move & hit_move_mask);
                /* write to cache & Hw updated HIT entry data */
                rv = soc_mem_write(u, mem, MEM_BLOCK_ALL, index, (void *)&entry_data);

                /* update again cache entry */
                cache_ent = soc_mem_table_idx_to_pointer(u, mem, uint32 * ,
                                            ALPM_HTBL_CACHE(u, i, pid), idx);
                *cache_ent = entry_data;
            }
        }
    }

    return rv;
}

/* Get hitbit status from a TCAM entry
   (it sync to Hw periodically) */
int
th3_tcam_ent_hit_get(int u, int app, int pkm, void *e, int sub_idx)
{
    int         rv = BCM_E_NONE;
    soc_field_t hit_fld[2] = {HIT0f, HIT1f};
    _alpm_cb_t  *acb = ACB_TOP(u, app);
    soc_mem_t mem = alpm_tbl[acb->pvt_tbl[pkm]];

    if (SOC_MEM_FIELD_VALID(u, mem, LWR_HIT0f)) {
        hit_fld[0] = LWR_HIT0f;
        hit_fld[1] = LWR_HIT1f;
    }
    rv = soc_mem_field32_get(u, mem, e, hit_fld[sub_idx]);

    return rv;
}

/* Get TCAM cached hitbit status from pvt_node's tcam_idx
   (it sync to Hw periodically) */
int
th3_tcam_cache_hit_get(int u, _alpm_pvt_node_t *pvt_node, int app, int pkm, int tcam_idx)
{
    int         rv = BCM_E_NONE;
    int         idx, sub_idx;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    soc_field_t hit_fld[2] = {HIT0f, HIT1f};

    _alpm_cb_t *acb = ACB_TOP(u, app);
    soc_mem_t mem = alpm_tbl[acb->pvt_tbl[pkm]];

    if (SOC_MEM_FIELD_VALID(u, mem, LWR_HIT0f)) {
        hit_fld[0] = LWR_HIT0f;
        hit_fld[1] = LWR_HIT1f;
    }

    if (pkm == ALPM_PKM_32B) {
        /* tcam_idx = idx << 1 | sub_idx */
        sub_idx = tcam_idx & 1;
        idx = tcam_idx >> 1;
    } else {
        sub_idx = 0;
        idx = tcam_idx;
    }

    ALPM_IER(th3_mem_read(u, acb, acb->pvt_tbl[pkm], idx, e, FALSE));
    rv = soc_mem_field32_get(u, mem, e, hit_fld[sub_idx]);
    return rv;
}

/* Write TCAM (hw & cache) hitbit status from pvt_node's tcam_idx */
int
th3_tcam_hit_write(int u, _alpm_pvt_node_t *pvt_node,
                   int app, int pkm, int tcam_idx, int hit_val)
{
    int         rv = BCM_E_NONE;
    int         idx, sub_idx;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    soc_field_t hit_fld[2] = {HIT0f, HIT1f};

    _alpm_cb_t *acb = ACB_TOP(u, app);
    soc_mem_t mem = alpm_tbl[acb->pvt_tbl[pkm]];

    if (SOC_MEM_FIELD_VALID(u, mem, LWR_HIT0f)) {
        hit_fld[0] = LWR_HIT0f;
        hit_fld[1] = LWR_HIT1f;
    }

    if (pkm == ALPM_PKM_32B) {
        /* tcam_idx = idx << 1 | sub_idx */
        sub_idx = tcam_idx & 1;
        idx = tcam_idx >> 1;
    } else {
        sub_idx = 0;
        idx = tcam_idx;
    }

    ALPM_IER(th3_mem_read(u, acb, acb->pvt_tbl[pkm], idx, e, FALSE));
    soc_mem_field32_set(u, mem, e, hit_fld[sub_idx], hit_val);
    rv = th3_mem_write(u, acb, NULL, acb->pvt_tbl[pkm], idx, e, FALSE);

    return rv;
}

/* Sync TCAM hitbit cache tables with Hw */
int
th3_tcam_hit_cache_sync(int u)
{
    soc_mem_t mem;
    _alpm_cb_t *acb;
    int pkm, tbl_sz;
    int rv = BCM_E_NONE;
    int app, app_cnt = alpm_app_cnt(u);

    for (app = 0; app < app_cnt; app++) {
        acb = ACB_TOP(u, app);
        for (pkm = 0; pkm < ALPM_PKM_CNT; pkm++) {

            if (ALPM_TCAM_TBL_SKIP(u, pkm)) {
                continue; /* skip duplicated or invalid TCAM table */
            }

            mem = alpm_tbl[acb->pvt_tbl[pkm]];
            tbl_sz = th3_tcam_table_sz(u, app, pkm);
            if (tbl_sz) {
                rv = soc_mem_cache_set(u, mem, MEM_BLOCK_ANY, TRUE);
            }
        }
    }

    return rv;
}

void
th3_alpm_hit_deinit(int u)
{
    int i, pid;
    _alpm_cb_t *acb;
    int app, app_cnt = alpm_app_cnt(u);

    if (ALPM_HIT_SKIP(u)) {
        return;
    }

    /* Dist hitbit tables deinit */
    if (ALPMDH(u) != NULL) {
        for (app = 0; app < app_cnt; app++) {
            for (i = 0; i < ACB_CNT(u); i++) {
                acb = ACB(u, i, app);
                for (pid = 0; pid < ACB_PID_CNT(acb); pid++) {
                    if (ALPM_HTBL(u, i, pid)) {
                        if (ALPM_HTBL_CACHE(u, i, pid)) {
                            soc_cm_sfree(u, ALPM_HTBL_CACHE(u, i, pid));
                            ALPM_HTBL_CACHE(u, i, pid) = NULL;
                        }
                        if (ALPM_HTBL_MOVE(u, i, pid)) {
                            soc_cm_sfree(u, ALPM_HTBL_MOVE(u, i, pid));
                            ALPM_HTBL_MOVE(u, i, pid) = NULL;
                        }
                        if (ALPM_HTBL_MOVE_MASK(u, i, pid)) {
                            soc_cm_sfree(u, ALPM_HTBL_MOVE_MASK(u, i, pid));
                            ALPM_HTBL_MOVE_MASK(u, i, pid) = NULL;
                        }
                        alpm_util_free(ALPM_HTBL(u, i, pid));
                        ALPM_HTBL(u, i, pid) = NULL;
                    }
                }
            }
        }
    }

    return;
}

/* Initialize TH3 ALPM distributed hitbit */
int
th3_alpm_hit_init(int u)
{
    int rv = BCM_E_NONE;
    int i, pid, index_cnt;
    int alloc_sz;
    soc_mem_t mem;
    _alpm_cb_t *acb;
    int app, app_cnt = alpm_app_cnt(u);

    if (ALPM_HIT_SKIP(u)) {
        return rv;
    }

    /* Initialize bucket Levels hitbit table ctrl */
    for (app = 0; app < app_cnt; app++) {
        for (i = 0; i < ACB_CNT(u); i++) {
            acb = ACB(u, i, app);
            mem = th3_alpm_hit_tbl_mem[i];
            /* Current implementation is assuming that HIT table entry
               is not more than 32 bits (actaully we have as maximum 16 HITBITs).
               Otherwise code has to adjust (for multiple words per entry) */
            assert(soc_mem_entry_words(u, mem) == 1);

            if (!SOC_WARM_BOOT(u)) {
                th3_alpm_hw_mem_clear(u, mem);
            }

            for (pid = 0; pid < ACB_PID_CNT(acb); pid++) {
                alloc_sz = sizeof(_alpm_hit_tbl_ctrl_t);
                ALPM_ALLOC_EG(ALPM_HTBL(u, i, pid), alloc_sz, "hit_tbl");

                /* logic table index counter (same for HIT table) */
                index_cnt = ACB_PID_BNK_CNT(acb, pid);
                alloc_sz = WORDS2BYTES(soc_mem_entry_words(u, mem)) * index_cnt;
                ALPM_HTBL_MEM(u, i, pid) = mem;
                ALPM_HTBL_IDX_CNT(u, i, pid) = index_cnt;
                ALPM_HTBL_SIZE(u, i, pid) = alloc_sz;
                ALPM_HTBL_ENT_MOVED(u, i, pid) = FALSE;
                ALPM_DMA_ALLOC_EG(u, ALPM_HTBL_CACHE(u, i, pid),
                                  alloc_sz, "hit_cache");
                ALPM_DMA_ALLOC_EG(u, ALPM_HTBL_MOVE(u, i, pid),
                                  alloc_sz, "hit_move");
                ALPM_DMA_ALLOC_EG(u, ALPM_HTBL_MOVE_MASK(u, i, pid),
                                  alloc_sz, "hit_move_mask");
            }
        }
    }

    return rv;

bad:
    th3_alpm_hit_deinit(u);
    return rv;
}

/* TH3 ALPM hitbit cleanup */
int
th3_alpm_hit_cleanup(int u)
{
    int rv = BCM_E_NONE;
    int i, pid;
    int alloc_sz;
    soc_mem_t mem;
    _alpm_cb_t *acb;
    int app, app_cnt = alpm_app_cnt(u);

    if (ALPM_HIT_SKIP(u)) {
        return rv;
    }

    for (app = 0; app < app_cnt; app++) {
        for (i = 0; i < ACB_CNT(u); i++) {
            acb = ACB(u, i, app);
            mem = th3_alpm_hit_tbl_mem[i];

            if (!SOC_WARM_BOOT(u)) {
                th3_alpm_hw_mem_clear(u, mem);
            }

            for (pid = 0; pid < ACB_PID_CNT(acb); pid++) {

                alloc_sz = ALPM_HTBL_SIZE(u, i, pid);
                sal_memset(ALPM_HTBL_CACHE(u, i, pid), 0, alloc_sz);
                sal_memset(ALPM_HTBL_MOVE(u, i, pid), 0, alloc_sz);
                sal_memset(ALPM_HTBL_MOVE_MASK(u, i, pid), 0, alloc_sz);

                ALPM_HTBL_ENT_MOVED(u, i, pid) = FALSE;
                ALPM_HTBL_ENT_MOVED_CNT(u, i, pid) = 0;
            }
        }
    }
    return rv;
}


/* Default projected route counter (obtained from Lab tested DB),
   providing current cnt_total[] for adjustment based on default value */
STATIC int
th3_alpm_def_usage_get(int u, int grp, int *def_route, int *def_lvl, int *def_usage, int *cur_cnt_total)
{
    alpm_default_usage_t *def;
    _alpm_cb_t *acb;
    int app = 0;
    int i, lvl, max_lvl, max_usage;
    uint8 mode = (uint8) ALPM_MODE(u);
    uint8 hlfbnk = 0;
    uint8 acb_cnt = (uint8) ACB_CNT(u);
    uint8 enable_128b = (uint8) ALPM_128B(u);
    uint8 rsvd = ALPM_BKT_RSVD(u) && ALPM_128B(u);
    uint8 full = (uint8) soc_property_get(u, spn_L3_ALPM2_DEFAULT_ROUTE_DATA_MODE, 1);
    int tblsize = th3_def_usage_tblsize[mode];

    acb = ACB_BTM(u, app);
    hlfbnk = (uint8) (ACB_HLF_BNK_MD(acb) && ALPM_HLF_BNKS(u));

    for (i = 0; i < tblsize; i++) {
        def = &th3_def_usage[mode][i];

        if (def->hlfbnk == hlfbnk && def->acb_cnt == acb_cnt &&
            def->enable_128b == enable_128b && def->rsvd == rsvd &&
            def->full == full && def->grp == grp) {
            break; /* found matched one */
        }
    }

    if (i < tblsize) { /* found matched one */
        int adjusted = 0;
        int delta = 0;
        int limit_usage = alpm_bnk_limit_usage(u, grp, BCM_L3_ALPM_LEVELS - 1);
        int cur_usage[BCM_L3_ALPM_LEVELS];

        /* initialize current usage with default usage */
        for (lvl = 0; lvl < BCM_L3_ALPM_LEVELS; lvl++) {
            cur_usage[lvl] = def->usage[lvl];
        }

        /* Adjust def usage if current L1 cnt_total is diff than default one */
        if (enable_128b && (cur_cnt_total[0] != def->cnt_total[0])) {
            for (lvl = 0; lvl < acb_cnt + 1; lvl++) {
                if (cur_cnt_total[lvl] == 0) {
                    break;
                }

                cur_usage[lvl] = def->usage[lvl] * def->cnt_total[lvl] / cur_cnt_total[lvl];
            }
            adjusted = 1;
        }

        /* search the worst (max) usage lvl */
        max_lvl = 0;
        max_usage = cur_usage[0];

        for (lvl = 1; lvl < acb_cnt + 1; lvl++) {
            if (cur_cnt_total[lvl] == 0) {
                break;
            }

            /* give 1% of advantage for L3 usage */
            delta = (lvl == BCM_L3_ALPM_LEVELS - 1 ? 1 : 0);
            if ((cur_usage[lvl] + delta) > max_usage) {
                max_lvl = lvl;
                max_usage = cur_usage[lvl];
            }
        }

        if ((max_lvl == BCM_L3_ALPM_LEVELS - 1) &&
            (limit_usage != max_usage)) {
            adjusted = 2; /* L3 max_usage adjustment to limit_usage needed */
        }

        if (max_usage <= 0) {
            return BCM_E_INTERNAL; /* test DB result wrong */
        }

        *def_lvl = max_lvl;
        if (adjusted) {
            limit_usage = alpm_bnk_limit_usage(u, grp, max_lvl);
            *def_usage = limit_usage;
            *def_route = def->route_cnt * limit_usage / max_usage;
        } else {
            *def_usage = max_usage;
            *def_route = def->route_cnt;
        }
    } else {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

STATIC int
th3_alpm_mem_data_save(int u, soc_mem_t mem, bcm_esw_alpm_data_cb write_cb, void *fp)
{
    char *mbuf;
    int size, offset = 0;
    int rv = 0;

    size = SOC_MEM_TABLE_BYTES(u, mem);
    /* skip first 4 shared UFT banks for EM */
    if ((soc_th3_get_alpm_banks(u) == 4) &&
        (mem == L3_DEFIP_ALPM_LEVEL3m ||
         mem == L3_DEFIP_ALPM_LEVEL3_HIT_ONLYm)) {
        offset = (th3_alpm_mem_size(u, mem) >> 1);
        size >>= 1;
    }
    mbuf = soc_cm_salloc(u, size, "alpm_mem");
    if (mbuf != NULL) {
        sal_memset(mbuf, 0, size);
        (void) soc_mem_read_range(u, mem, MEM_BLOCK_ANY,
                                  offset, th3_alpm_mem_size(u, mem) - 1, mbuf);
        rv = write_cb(u, fp, mbuf, size); /* write mbuf to fp */
        soc_cm_sfree(u, mbuf);
        if (rv != size) {
            cli_out("th3_alpm_mem_data_save %s read_cb failed (rv=%d, size=%d)\n",
                    SOC_MEM_NAME(u, mem), rv, size);
            return BCM_E_FAIL;
        }
    } else {
        cli_out("th3_alpm_mem_data_save %s alloc DMA memory failed\n", SOC_MEM_NAME(u, mem));
    }

    return BCM_E_NONE;
}

STATIC int
th3_alpm_mem_data_restore(int u, soc_mem_t mem, bcm_esw_alpm_data_cb read_cb, void *fp)
{
    char *mbuf;
    int size, offset = 0;
    int rv = 0;

    size = SOC_MEM_TABLE_BYTES(u, mem);
    /* skip first 4 shared UFT banks for EM */
    if ((soc_th3_get_alpm_banks(u) == 4) &&
        (mem == L3_DEFIP_ALPM_LEVEL3m ||
         mem == L3_DEFIP_ALPM_LEVEL3_HIT_ONLYm)) {
        offset = (th3_alpm_mem_size(u, mem) >> 1);
        size >>= 1;
    }
    mbuf = soc_cm_salloc(u, size, "alpm_mem");
    if (mbuf != NULL) {
        sal_memset(mbuf, 0, size);
        rv = read_cb(u, fp, mbuf, size); /* read mbuf from fp */
        if (rv != size) {
            cli_out("th3_alpm_mem_data_restore %s read_cb failed (rv=%d, size=%d)\n",
                    SOC_MEM_NAME(u, mem), rv, size);
            soc_cm_sfree(u, mbuf);
            return BCM_E_FAIL;
        }
        (void) soc_mem_write_range(u, mem, MEM_BLOCK_ANY,
                                  offset, th3_alpm_mem_size(u, mem) - 1, mbuf);
        soc_cm_sfree(u, mbuf);
    } else {
        cli_out("th3_alpm_mem_data_restore %s alloc DMA memory failed\n", SOC_MEM_NAME(u, mem));
    }

    return BCM_E_NONE;
}

/* op: save (0), restore (1) */
STATIC int
th3_alpm_data_op(int u, int app, int op, bcm_esw_alpm_data_cb cb, void *fp)
{
    int rv = BCM_E_NONE;
    int i, j, pid, acb_cnt;
    soc_mem_t mem, allmems[20] = {INVALIDm};
    int m, mem_cnt = 0;
    int (*mem_data_func[2])(int, soc_mem_t, bcm_esw_alpm_data_cb, void *) = {
        th3_alpm_mem_data_save,
        th3_alpm_mem_data_restore
    };

    if (SOC_MEM_IS_VALID(u, L3_DEFIP_ALPM_LEVEL2m)) {
        alpm_tbl = &th3_alpm_tbl[0];
    }

    acb_cnt = ACB_CNT(u);

    for (i = 0; i < acb_cnt; i++) {
        _alpm_cb_t *acb = ACB(u, i, app);

#define _CHK_IF_MEM_DATA_FUNC(mem)              \
    do {                                        \
        for (m = 0; m < mem_cnt; m++) {         \
            if (allmems[m] == (mem)) {          \
                break;                          \
            }                                   \
        }                                       \
        if (m == mem_cnt && mem != INVALIDm &&  \
            th3_alpm_mem_size(u, mem) > 0) {    \
            rv = mem_data_func[op](u, (mem), cb, fp); \
            if (BCM_FAILURE(rv)) { \
                goto bad; \
            } \
            allmems[mem_cnt++] = (mem);\
        }                                       \
    } while (0)

        for (pid = 0; pid < ALPM_BKT_PID_CNT; pid++) {
            _alpm_bkt_pool_conf_t *bp_conf = ACB_BKT_POOL(acb, pid);
            if (pid > 0 && ACB_BKT_POOL(acb, pid-1) == bp_conf) {
                continue;
            }
            mem = alpm_tbl[BPC_BKT_TBL(bp_conf)];
            _CHK_IF_MEM_DATA_FUNC(mem);
        }


        for (j = 0; j < ALPM_PKM_CNT; j++) {
            mem = alpm_tbl[acb->pvt_tbl[j]];
            _CHK_IF_MEM_DATA_FUNC(mem);
        }
    }

bad:
    return rv;
}

alpm_functions_t th3_alpm_driver = {
    th3_alpm_ctrl_init,
    th3_alpm_ctrl_deinit,
    th3_alpm_ctrl_cleanup,
    th3_alpm_bkt_bnk_copy,
    NULL, /* th3_alpm_bkt_bnk_clear, */
    th3_alpm_bkt_pfx_copy,
    th3_alpm_bkt_pfx_clean,
    th3_alpm_bkt_pfx_shrink,
    th3_alpm_bkt_bnk_write,
    th3_alpm_bkt_def_fmt_get,
    th3_alpm_bkt_fmt_type_get,

    th3_alpm_ent_ent_get,
    th3_alpm_ent_pfx_len_get,
    th3_alpm_ent_selective_get,

    /* TCAM */
    th3_tcam_table_sz,
    th3_tcam_hw_cleanup,
    th3_tcam_entry_x_to_y,
    th3_tcam_entry_from_cfg,
    th3_tcam_entry_to_cfg,
    th3_tcam_entry_valid,
    th3_tcam_entry_valid_set,
    th3_tcam_entry_vrf_id_get,
    th3_tcam_entry_key_type_get,
    th3_tcam_entry_pfx_len_get,
    th3_tcam_entry_to_key,

    th3_tcam_entry_mode_get,
    th3_tcam_entry_adata_get,
    th3_tcam_entry_bdata_get,

    /* TCAM/ALPM */
    th3_mem_read,
    th3_mem_write,

    /* HITBIT */
    th3_alpm_hit_init,
    th3_alpm_hit_deinit,
    th3_alpm_hit_cleanup,
    th3_alpm_bkt_hit_get,
    th3_alpm_bkt_hit_set,
    th3_alpm_bkt_hit_write,
    th3_alpm_hit_move_update,
    th3_alpm_hit_cache_sync,
    th3_alpm_hit_hw_write,
    th3_tcam_ent_hit_get,
    th3_tcam_cache_hit_get,
    th3_tcam_hit_cache_sync,
    th3_tcam_hit_write,

    /* dump table */
    th3_alpm_table_dump,
    th3_alpm_cap_get,
    th3_alpm_def_usage_get,

    /* data op: save/restore */
    th3_alpm_data_op,

    /* index mapping */
    th3_mem_phy_index_get,
};

#endif /* BCM_TOMAHAWK3_SUPPORT */
#else
typedef int bcm_esw_alpm2_alpm_drv_th3_not_empty; /* Make ISO compilers happy. */
#endif /* ALPM_ENABLE */
