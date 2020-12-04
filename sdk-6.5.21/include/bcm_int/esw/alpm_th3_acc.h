/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        alpm_th3_acc.h
 * Purpose:     Function declarations for ALPM Internal functions.
 */

#ifndef _BCM_INT_ALPM_TH3_ACC_H_
#define _BCM_INT_ALPM_TH3_ACC_H_

typedef struct {
#ifdef  LE_HOST
    uint32  DEST_TYPE:1,
            DESTINATION:15,
            PRI:4,
            RPE:1,
            DST_DISCARD:1,
            CLASS_ID:6,
            FLEX_CTR_POOL_NUMBER:4;
#else
    uint32  FLEX_CTR_POOL_NUMBER:4,
            CLASS_ID:6,
            DST_DISCARD:1,
            RPE:1,
            PRI:4,
            DESTINATION:15,
            DEST_TYPE:1;
#endif  /* LE_HOST */

#ifdef  LE_HOST
    uint32  FLEX_CTR_OFFSET_MODE:2,
            FLEX_CTR_BASE_COUNTER_IDX:12,
            __PAD0:18;
#else
    uint32  __PAD0:18,
            FLEX_CTR_BASE_COUNTER_IDX:12,
            FLEX_CTR_OFFSET_MODE:2;
#endif  /* LE_HOST */
} th3_fmt_ASSOC_DATA_FULL_t;

typedef struct {
#ifdef  LE_HOST
    uint32  DEST_TYPE:1,
            DESTINATION:15,
            __PAD0:16;
#else
    uint32  __PAD0:16,
            DESTINATION:15,
            DEST_TYPE:1;
#endif  /* LE_HOST */

} th3_fmt_ASSOC_DATA_REDUCED_t;

typedef struct {
#ifdef  LE_HOST
    uint32  SUB_DB_PRIORITY:2,
            __PAD0:30;
#else
    uint32  __PAD0:30,
            SUB_DB_PRIORITY:2;
#endif  /* LE_HOST */
} th3_fmt_FIXED_DATA_t;

typedef struct {
#ifdef  LE_HOST
    uint32  KEY_MODE:2,
            KEY_TYPE:1,
            IP_ADDR_0_28:29;
#else
    uint32  IP_ADDR_0_28:29,
            KEY_TYPE:1,
            KEY_MODE:2;
#endif  /* LE_HOST */

#ifdef  LE_HOST
    uint32  IP_ADDR_29_31:3,
            VRF_ID:12,
            RESERVED:1,
            __PAD0:16;
#else
    uint32  __PAD0:16,
            RESERVED:1,
            VRF_ID:12,
            IP_ADDR_29_31:3;
#endif  /* LE_HOST */
} th3_fmt_L3_DEFIP_TCAM_KEY_t;

typedef struct {
#ifdef  LE_HOST
    uint32  DEFAULT_MISS:1,
            DIRECT_ROUTE:1,
            BKT_PTR:10,
            KSHIFT:7,
            ROFS:3,
            FMT0:4,
            FMT1:4,
            FMT2_0_1:2;
#else
    uint32  FMT2_0_1:2,
            FMT1:4,
            FMT0:4,
            ROFS:3,
            KSHIFT:7,
            BKT_PTR:10,
            DIRECT_ROUTE:1,
            DEFAULT_MISS:1;
#endif  /* LE_HOST */

#ifdef  LE_HOST
    uint32  FMT2_2_3:2,
            FMT3:4,
            FMT4:4,
            __PAD0:22;
#else
    uint32  __PAD0:22,
            FMT4:4,
            FMT3:4,
            FMT2_2_3:2;
#endif  /* LE_HOST */
} th3_fmt_ALPM1_DATA_t;

typedef struct {
#ifdef  LE_HOST
    uint32  DEFAULT_MISS:1,
            BKT_PTR:13,
            KSHIFT:7,
            ROFS:3,
            FMT0:4,
            FMT1:4;
#else
    uint32  FMT1:4,
            FMT0:4,
            ROFS:3,
            KSHIFT:7,
            BKT_PTR:13,
            DEFAULT_MISS:1;
#endif  /* LE_HOST */

#ifdef  LE_HOST
    uint32  FMT2:4,
            FMT3:4,
            FMT4:4,
            FMT5:4,
            FMT6:4,
            FMT7:4,
            __PAD0:8;
#else
    uint32  __PAD0:8,
            FMT7:4,
            FMT6:4,
            FMT5:4,
            FMT4:4,
            FMT3:4,
            FMT2:4;
#endif  /* LE_HOST */
} th3_fmt_ALPM2_DATA_t;

#if 0
/* L3_DEFIP_ALPM_PIVOT_FMT type */
typedef struct {
#ifdef  LE_HOST
    uint32  PREFIX:8,
            ASSOC_DATA:16,
            ALPM2_DATA_0_7:8;
#else
    uint32  ALPM2_DATA_0_7:8,
            ASSOC_DATA:16,
            PREFIX:8;
#endif  /* LE_HOST */

    uint32 ALPM2_DATA_8_39;

#ifdef  LE_HOST
    uint32  ALPM2_DATA_40_55:16,
            __PAD0:16;
#else
    uint32  __PAD0:16,
            ALPM2_DATA_40_55:16;
#endif  /* LE_HOST */
} th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT1_t;

typedef struct {
#ifdef  LE_HOST
    uint32  PREFIX:24,
            ASSOC_DATA_0_7:8;
#else
    uint32  ASSOC_DATA_0_7:8,
            PREFIX:24;
#endif  /* LE_HOST */

#ifdef  LE_HOST
    uint32  ASSOC_DATA_8_15:8,
            ALPM2_DATA_0_23:24;
#else
    uint32  ALPM2_DATA_0_23:24,
            ASSOC_DATA_8_15:8;
#endif  /* LE_HOST */

    uint32  ALPM2_DATA_24_55;
} th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT2_t;

typedef struct {
    uint32  PREFIX_0_31;

#ifdef  LE_HOST
    uint32  PREFIX_32_47:16,
            ASSOC_DATA:16;
#else
    uint32  ASSOC_DATA:16,
            PREFIX_32_47:16;
#endif  /* LE_HOST */

    uint32  ALPM2_DATA_0_31;

#ifdef  LE_HOST
    uint32  ALPM2_DATA_32_55:24,
            __PAD0:8;
#else
    uint32  __PAD0:8,
            ALPM2_DATA_32_55:24;
#endif  /* LE_HOST */

} th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT3_t;

typedef struct {
    uint32  PREFIX_0_31;
    uint32  PREFIX_32_63;

#ifdef  LE_HOST
    uint32  PREFIX_64_87:24,
            ASSOC_DATA_0_7:8;
#else
    uint32  ASSOC_DATA_0_7:8,
            PREFIX_64_87:24;
#endif  /* LE_HOST */

#ifdef  LE_HOST
    uint32  ASSOC_DATA_8_15:8,
            ALPM2_DATA_0_23:24;
#else
    uint32  ALPM2_DATA_0_23:24,
            ASSOC_DATA_8_15:8;
#endif  /* LE_HOST */

    uint32  ALPM2_DATA_24_55;

} th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT4_t;

typedef struct {
    uint32  PREFIX_0_31;
    uint32  PREFIX_32_63;
    uint32  PREFIX_64_95;
    uint32  PREFIX_96_127;

#ifdef  LE_HOST
    uint32  PREFIX_128:1,
            RESERVED:7,
            ASSOC_DATA:16,
            ALPM2_DATA_0_7:8;
#else
    uint32  ALPM2_DATA_0_7:8,
            ASSOC_DATA:16,
            RESERVED:7,
            PREFIX_128:1;
#endif  /* LE_HOST */

    uint32  ALPM2_DATA_8_39;

#ifdef  LE_HOST
    uint32  ALPM2_DATA_40_55:16,
            __PAD0:16;
#else
    uint32  __PAD0:16,
            ALPM2_DATA_40_55:16;
#endif  /* LE_HOST */

} th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT5_t;

typedef struct {
#ifdef  LE_HOST
    uint32  PREFIX:18,
            ASSOC_DATA_0_13:14;
#else
    uint32  ASSOC_DATA_0_13:14,
            PREFIX:18;
#endif  /* LE_HOST */

    uint32  ASSOC_DATA_14_45;
    uint32  ALPM2_DATA_0_31;

#ifdef  LE_HOST
    uint32  ALPM2_DATA_32_55:24,
            __PAD0:8;
#else
    uint32  __PAD0:8,
            ALPM2_DATA_32_55:24;
#endif  /* LE_HOST */

} th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT6_t;

typedef struct {
    uint32  PREFIX_0_31;
#ifdef  LE_HOST
    uint32  PREFIX_32_57:26,
            ASSOC_DATA_0_5:6;
#else
    uint32  ASSOC_DATA_0_5:6,
            PREFIX_32_57:26;
#endif  /* LE_HOST */

    uint32  ASSOC_DATA_6_37;

#ifdef  LE_HOST
    uint32  ASSOC_DATA_38_45:8,
            ALPM2_DATA_0_23:24;
#else
    uint32  ALPM2_DATA_0_23:24,
            ASSOC_DATA_38_45:8;
#endif  /* LE_HOST */
    uint32  ALPM2_DATA_24_55;

} th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT7_t;

typedef struct {
    uint32  PREFIX_0_31;
    uint32  PREFIX_32_63;
    uint32  PREFIX_64_95;
    uint32  PREFIX_96_127;

#ifdef  LE_HOST
    uint32  PREFIX_128:1,
            RESERVED:1,
            ASSOC_DATA_0_29:30;
#else
    uint32  ASSOC_DATA_0_29:30,
            RESERVED:1,
            PREFIX_128:1;
#endif  /* LE_HOST */

#ifdef  LE_HOST
    uint32  ASSOC_DATA_30_45:16,
            ALPM2_DATA_0_15:16;
#else
    uint32  ALPM2_DATA_0_15:16,
            ASSOC_DATA_30_45:16;
#endif  /* LE_HOST */

    uint32  ALPM2_DATA_16_47;

#ifdef  LE_HOST
    uint32  ALPM2_DATA_48_55:8,
            __PAD0:24;
#else
    uint32  __PAD0:24,
            ALPM2_DATA_48_55:8;
#endif  /* LE_HOST */

} th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT8_t;

/* L3_DEFIP_ALPM_ROUTE_FMT type */
typedef struct {
#ifdef  LE_HOST
    uint32  PREFIX:16,
            ASSOC_DATA:16;
#else
    uint32  ASSOC_DATA:16,
            PREFIX:16;
#endif  /* LE_HOST */
} th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT1_t;

typedef struct {
#ifdef  LE_HOST
    uint32  PREFIX:24,
            ASSOC_DATA_0_7:8;
#else
    uint32  ASSOC_DATA_0_7:8,
            PREFIX:24;
#endif  /* LE_HOST */

#ifdef  LE_HOST
    uint32  ASSOC_DATA_8_15:8,
            __PAD0:24;
#else
    uint32  __PAD0:24,
            ASSOC_DATA_8_15:8;
#endif  /* LE_HOST */
} th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT2_t;

typedef struct {
    uint32  PREFIX;

#ifdef  LE_HOST
    uint32  ASSOC_DATA:16,
            __PAD0:16;
#else
    uint32  __PAD0:16,
            ASSOC_DATA:16;
#endif  /* LE_HOST */
} th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT3_t;

typedef struct {
    uint32  PREFIX_0_31;

#ifdef  LE_HOST
    uint32  PREFIX_32_39:8,
            ASSOC_DATA:16,
            __PAD0:8;
#else
    uint32  __PAD0:8,
            ASSOC_DATA:16,
            PREFIX_32_39:8;
#endif  /* LE_HOST */
} th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT4_t;

typedef struct {
    uint32  PREFIX_0_31;

#ifdef  LE_HOST
    uint32  PREFIX_32_47:16,
            ASSOC_DATA:16;
#else
    uint32  ASSOC_DATA:16,
            PREFIX_32_47:16;
#endif  /* LE_HOST */
} th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT5_t;

typedef struct {
    uint32  PREFIX_0_31;
    uint32  PREFIX_32_63;

#ifdef  LE_HOST
    uint32  ASSOC_DATA:16,
            __PAD0:16;
#else
    uint32  __PAD0:16,
            ASSOC_DATA:16;
#endif  /* LE_HOST */
} th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT6_t;

typedef struct {
    uint32  PREFIX_0_31;
    uint32  PREFIX_32_63;

#ifdef  LE_HOST
    uint32  PREFIX_64_79:16,
            ASSOC_DATA:16;
#else
    uint32  ASSOC_DATA:16,
            PREFIX_64_79:16;
#endif  /* LE_HOST */
} th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT7_t;

typedef struct {
    uint32  PREFIX_0_31;
    uint32  PREFIX_32_63;
    uint32  PREFIX_64_95;

#ifdef  LE_HOST
    uint32  PREFIX_96_103:8,
            ASSOC_DATA:16,
            __PAD0:8;
#else
    uint32  __PAD0:8,
            ASSOC_DATA:16,
            PREFIX_96_103:8;
#endif  /* LE_HOST */
} th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT8_t;

typedef struct {
    uint32  PREFIX_0_31;
    uint32  PREFIX_32_63;
    uint32  PREFIX_64_95;
    uint32  PREFIX_96_127;

#ifdef  LE_HOST
    uint32  PREFIX_128:1,
            RESERVED:7,
            ASSOC_DATA:16,
            __PAD0:8;
#else
    uint32  __PAD0:8,
            ASSOC_DATA:16,
            RESERVED:7,
            PREFIX_128:1;
#endif  /* LE_HOST */
} th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT9_t;

typedef struct {
    uint32  PREFIX_0_31;

#ifdef  LE_HOST
    uint32  PREFIX_32_33:2,
            ASSOC_DATA_0_29:30;
#else
    uint32  ASSOC_DATA_0_29:30,
            PREFIX_32_33:2;
#endif  /* LE_HOST */

#ifdef  LE_HOST
    uint32  ASSOC_DATA_30_45:16,
            __PAD0:16;
#else
    uint32  __PAD0:16,
            ASSOC_DATA_30_45:16;
#endif  /* LE_HOST */

} th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT10_t;

typedef struct {
    uint32  PREFIX_0_31;
    uint32  PREFIX_32_63;

#ifdef  LE_HOST
    uint32  PREFIX_64_73:10,
            ASSOC_DATA_0_21:22;
#else
    uint32  ASSOC_DATA_0_21:22,
            PREFIX_64_73:10;
#endif  /* LE_HOST */

#ifdef  LE_HOST
    uint32  ASSOC_DATA_22_45:24,
            __PAD0:8;
#else
    uint32  __PAD0:8,
            ASSOC_DATA_22_45:24;
#endif  /* LE_HOST */

} th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT11_t;

typedef struct {
    uint32  PREFIX_0_31;
    uint32  PREFIX_32_63;
    uint32  PREFIX_64_95;
    uint32  PREFIX_96_127;

#ifdef  LE_HOST
    uint32  PREFIX_128:1,
            RESERVED:1,
            ASSOC_DATA_0_29:30;
#else
    uint32  ASSOC_DATA_0_29:30,
            RESERVED:1,
            PREFIX_128:1;
#endif  /* LE_HOST */

#ifdef  LE_HOST
    uint32  ASSOC_DATA_30_45:16,
            __PAD0:16;
#else
    uint32  __PAD0:16,
            ASSOC_DATA_30_45:16;
#endif  /* LE_HOST */

} th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT12_t;


/* PIVOT FULL */
typedef struct {
    uint32  ENTRY_0_0_31;
    uint32  ENTRY_0_32_63;

#ifdef  LE_HOST
    uint32  ENTRY_0_64_79:16,
            ENTRY_1_0_15:16;
#else
    uint32  ENTRY_1_0_15:16,
            ENTRY_0_64_79:16;
#endif  /* LE_HOST */

    uint32  ENTRY_1_16_47;
    uint32  ENTRY_1_48_79;
    uint32  ENTRY_2_0_31;
    uint32  ENTRY_2_32_63;

#ifdef  LE_HOST
    uint32  ENTRY_2_64_79:16,
            ENTRY_3_0_15:16;
#else
    uint32  ENTRY_3_0_15:16,
            ENTRY_2_64_79:16;
#endif  /* LE_HOST */

    uint32  ENTRY_3_16_47;
    uint32  ENTRY_3_48_79;
    uint32  ENTRY_4_0_31;
    uint32  ENTRY_4_32_63;

#ifdef  LE_HOST
    uint32  ENTRY_4_64_79:16,
            ENTRY_5_0_15:16;
#else
    uint32  ENTRY_5_0_15:16,
            ENTRY_4_64_79:16;
#endif  /* LE_HOST */
    uint32  ENTRY_5_16_47;
    uint32  ENTRY_5_48_79;

} th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT1_FULL_t;

typedef struct {
    uint32  ENTRY_0_0_31;
    uint32  ENTRY_0_32_63;
    uint32  ENTRY_0_64_95;

    uint32  ENTRY_1_0_31;
    uint32  ENTRY_1_32_63;
    uint32  ENTRY_1_64_95;

    uint32  ENTRY_2_0_31;
    uint32  ENTRY_2_32_63;
    uint32  ENTRY_2_64_95;

    uint32  ENTRY_3_0_31;
    uint32  ENTRY_3_32_63;
    uint32  ENTRY_3_64_95;

    uint32  ENTRY_4_0_31;
    uint32  ENTRY_4_32_63;
    uint32  ENTRY_4_64_95;
} th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT2_FULL_t;

typedef struct {
    uint32  ENTRY_0_0_31;
    uint32  ENTRY_0_32_63;
    uint32  ENTRY_0_64_95;

#ifdef  LE_HOST
    uint32  ENTRY_0_96_119:24,
            ENTRY_1_0_7:8;
#else
    uint32  ENTRY_1_0_7:8,
            ENTRY_0_96_119:24;
#endif  /* LE_HOST */

    uint32  ENTRY_1_8_39;
    uint32  ENTRY_1_40_71;
    uint32  ENTRY_1_72_103;

#ifdef  LE_HOST
    uint32  ENTRY_1_104_119:16,
            ENTRY_2_0_15:16;
#else
    uint32  ENTRY_2_0_15:16,
            ENTRY_1_104_119:16;
#endif  /* LE_HOST */

    uint32  ENTRY_2_16_47;
    uint32  ENTRY_2_48_79;
    uint32  ENTRY_2_80_111;

#ifdef  LE_HOST
    uint32  ENTRY_2_112_119:8,
            ENTRY_3_0_23:24;
#else
    uint32  ENTRY_3_0_23:24,
            ENTRY_2_112_119:8;
#endif  /* LE_HOST */

    uint32  ENTRY_3_24_55;
    uint32  ENTRY_3_56_87;
    uint32  ENTRY_3_88_119;

} th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT3_FULL_t;

typedef struct {
    uint32  ENTRY_0_0_31;
    uint32  ENTRY_0_32_63;
    uint32  ENTRY_0_64_95;
    uint32  ENTRY_0_96_127;
    uint32  ENTRY_0_128_159;

    uint32  ENTRY_1_0_31;
    uint32  ENTRY_1_32_63;
    uint32  ENTRY_1_64_95;
    uint32  ENTRY_1_96_127;
    uint32  ENTRY_1_128_159;

    uint32  ENTRY_2_0_31;
    uint32  ENTRY_2_32_63;
    uint32  ENTRY_2_64_95;
    uint32  ENTRY_2_96_127;
    uint32  ENTRY_2_128_159;
} th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT4_FULL_t;

typedef struct {
    uint32  ENTRY_0_0_31;
    uint32  ENTRY_0_32_63;
    uint32  ENTRY_0_64_95;
    uint32  ENTRY_0_96_127;
    uint32  ENTRY_0_128_159;
    uint32  ENTRY_0_160_191;

#ifdef  LE_HOST
    uint32  ENTRY_0_192_207:16,
            ENTRY_1_0_15:16;
#else
    uint32  ENTRY_1_0_15:16,
            ENTRY_0_192_207:16;
#endif  /* LE_HOST */

    uint32  ENTRY_1_16_47;
    uint32  ENTRY_1_48_79;
    uint32  ENTRY_1_80_111;
    uint32  ENTRY_1_112_143;
    uint32  ENTRY_1_144_175;
    uint32  ENTRY_1_176_207;

    uint32  RESERVED_0_31;
    uint32  RESERVED_32_63;
} th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT5_FULL_t;

typedef th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT3_FULL_t th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT6_FULL_t;
typedef th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT4_FULL_t th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT7_FULL_t;

typedef struct {
    uint32  ENTRY_0_0_31;
    uint32  ENTRY_0_32_63;
    uint32  ENTRY_0_64_95;
    uint32  ENTRY_0_96_127;
    uint32  ENTRY_0_128_159;
    uint32  ENTRY_0_160_191;
    uint32  ENTRY_0_192_223;

#ifdef  LE_HOST
    uint32  ENTRY_0_224_231:8,
            ENTRY_1_0_23:24;
#else
    uint32  ENTRY_1_0_23:24,
            ENTRY_0_224_231:8;
#endif  /* LE_HOST */

    uint32  ENTRY_1_24_55;
    uint32  ENTRY_1_56_87;
    uint32  ENTRY_1_88_119;
    uint32  ENTRY_1_120_151;
    uint32  ENTRY_1_152_183;
    uint32  ENTRY_1_184_215;

#ifdef  LE_HOST
    uint32  ENTRY_1_216_231:16,
            RESERVED:16;
#else
    uint32  RESERVED:24,
            ENTRY_1_216_231:8;
#endif  /* LE_HOST */
} th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT8_FULL_t;

/* ROUTE FULL fmt */
typedef struct {
    uint32  ENTRY_0;
    uint32  ENTRY_1;
    uint32  ENTRY_2;
    uint32  ENTRY_3;
    uint32  ENTRY_4;
    uint32  ENTRY_5;
    uint32  ENTRY_6;
    uint32  ENTRY_7;
    uint32  ENTRY_8;
    uint32  ENTRY_9;
    uint32  ENTRY_10;
    uint32  ENTRY_11;
    uint32  ENTRY_12;
    uint32  ENTRY_13;
    uint32  ENTRY_14;
} th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT1_FULL_t;

typedef struct {
    uint32  ENTRY_0_0_31;
#ifdef  LE_HOST
    uint32  ENTRY_0_32_39:8,
            ENTRY_1_0_23:24;
#else
    uint32  ENTRY_1_0_23:24,
            ENTRY_0_32_39:8;
#endif  /* LE_HOST */

#ifdef  LE_HOST
    uint32  ENTRY_1_24_39:16,
            ENTRY_2_0_15:16;
#else
    uint32  ENTRY_2_0_15:16,
            ENTRY_1_24_39:16;
#endif  /* LE_HOST */

#ifdef  LE_HOST
    uint32  ENTRY_2_16_39:24,
            ENTRY_3_0_7:8;
#else
    uint32  ENTRY_3_0_7:8,
            ENTRY_2_16_39:24;
#endif  /* LE_HOST */

    uint32  ENTRY_3_8_39;

    uint32  ENTRY_4_0_31;
#ifdef  LE_HOST
    uint32  ENTRY_4_32_39:8,
            ENTRY_5_0_23:24;
#else
    uint32  ENTRY_5_0_23:24,
            ENTRY_4_32_39:8;
#endif  /* LE_HOST */

#ifdef  LE_HOST
    uint32  ENTRY_5_24_39:16,
            ENTRY_6_0_15:16;
#else
    uint32  ENTRY_6_0_15:16,
            ENTRY_5_24_39:16;
#endif  /* LE_HOST */

#ifdef  LE_HOST
    uint32  ENTRY_6_16_39:24,
            ENTRY_7_0_7:8;
#else
    uint32  ENTRY_7_0_7:8,
            ENTRY_6_16_39:24;
#endif  /* LE_HOST */

    uint32  ENTRY_7_8_39;

    uint32  ENTRY_8_0_31;
#ifdef  LE_HOST
    uint32  ENTRY_8_32_39:8,
            ENTRY_9_0_23:24;
#else
    uint32  ENTRY_9_0_23:24,
            ENTRY_8_32_39:8;
#endif  /* LE_HOST */

#ifdef  LE_HOST
    uint32  ENTRY_9_24_39:16,
            ENTRY_10_0_15:16;
#else
    uint32  ENTRY_10_0_15:16,
            ENTRY_9_24_39:16;
#endif  /* LE_HOST */

#ifdef  LE_HOST
    uint32  ENTRY_10_16_39:24,
            ENTRY_11_0_7:8;
#else
    uint32  ENTRY_11_0_7:8,
            ENTRY_10_16_39:24;
#endif  /* LE_HOST */
    uint32  ENTRY_11_8_39;
} th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT2_FULL_t;

typedef struct {
    uint32  ENTRY_0_0_31;
#ifdef  LE_HOST
    uint32  ENTRY_0_32_47:16,
            ENTRY_1_0_15:16;
#else
    uint32  ENTRY_1_0_15:16,
            ENTRY_0_32_47:16;
#endif  /* LE_HOST */
    uint32  ENTRY_1_16_47;

    uint32  ENTRY_2_0_31;
#ifdef  LE_HOST
    uint32  ENTRY_2_32_47:16,
            ENTRY_3_0_15:16;
#else
    uint32  ENTRY_3_0_15:16,
            ENTRY_2_32_47:16;
#endif  /* LE_HOST */
    uint32  ENTRY_3_16_47;

    uint32  ENTRY_4_0_31;
#ifdef  LE_HOST
    uint32  ENTRY_4_32_47:16,
            ENTRY_5_0_15:16;
#else
    uint32  ENTRY_5_0_15:16,
            ENTRY_4_32_47:16;
#endif  /* LE_HOST */
    uint32  ENTRY_5_16_47;

    uint32  ENTRY_6_0_31;
#ifdef  LE_HOST
    uint32  ENTRY_6_32_47:16,
            ENTRY_7_0_15:16;
#else
    uint32  ENTRY_7_0_15:16,
            ENTRY_6_32_47:16;
#endif  /* LE_HOST */
    uint32  ENTRY_7_16_47;

    uint32  ENTRY_8_0_31;
#ifdef  LE_HOST
    uint32  ENTRY_8_32_47:16,
            ENTRY_9_0_15:16;
#else
    uint32  ENTRY_9_0_15:16,
            ENTRY_8_32_47:16;
#endif  /* LE_HOST */
    uint32  ENTRY_9_16_47;
} th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT3_FULL_t;

typedef struct {
    uint32  ENTRY_0_0_31;
#ifdef  LE_HOST
    uint32  ENTRY_0_32_55:24,
            ENTRY_1_0_7:8;
#else
    uint32  ENTRY_1_0_7:8,
            ENTRY_0_32_55:24;
#endif  /* LE_HOST */
    uint32  ENTRY_1_8_39;
#ifdef  LE_HOST
    uint32  ENTRY_1_40_55:16,
            ENTRY_2_0_15:16;
#else
    uint32  ENTRY_2_0_15:16,
            ENTRY_1_40_55:16;
#endif  /* LE_HOST */
    uint32  ENTRY_2_16_47;
#ifdef  LE_HOST
    uint32  ENTRY_2_48_55:8,
            ENTRY_3_0_23:24;
#else
    uint32  ENTRY_3_0_23:24,
            ENTRY_2_48_55:8;
#endif  /* LE_HOST */
    uint32  ENTRY_3_24_55;

    uint32  ENTRY_4_0_31;
#ifdef  LE_HOST
    uint32  ENTRY_4_32_55:24,
            ENTRY_5_0_7:8;
#else
    uint32  ENTRY_5_0_7:8,
            ENTRY_4_32_55:24;
#endif  /* LE_HOST */
    uint32  ENTRY_5_8_39;
#ifdef  LE_HOST
    uint32  ENTRY_5_40_55:16,
            ENTRY_6_0_15:16;
#else
    uint32  ENTRY_6_0_15:16,
            ENTRY_5_40_55:16;
#endif  /* LE_HOST */
    uint32  ENTRY_6_16_47;
#ifdef  LE_HOST
    uint32  ENTRY_6_48_55:8,
            ENTRY_7_0_23:24;
#else
    uint32  ENTRY_7_0_23:24,
            ENTRY_6_48_55:8;
#endif  /* LE_HOST */
    uint32  ENTRY_7_24_55;

    uint32  RESERVED;
} th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT4_FULL_t;

typedef struct {
    uint32  ENTRY_0_0_31;
    uint32  ENTRY_0_32_63;

    uint32  ENTRY_1_0_31;
    uint32  ENTRY_1_32_63;

    uint32  ENTRY_2_0_31;
    uint32  ENTRY_2_32_63;

    uint32  ENTRY_3_0_31;
    uint32  ENTRY_3_32_63;

    uint32  ENTRY_4_0_31;
    uint32  ENTRY_4_32_63;

    uint32  ENTRY_5_0_31;
    uint32  ENTRY_5_32_63;

    uint32  ENTRY_6_0_31;
    uint32  ENTRY_6_32_63;

    uint32  RESERVED;
} th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT5_FULL_t;

typedef th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT1_FULL_t th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT6_FULL_t;
typedef th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT2_FULL_t th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT7_FULL_t;
typedef th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT3_FULL_t th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT8_FULL_t;

typedef struct {
    uint32  ENTRY_0_0_31;
    uint32  ENTRY_0_32_63;
    uint32  ENTRY_0_64_95;
    uint32  ENTRY_0_96_127;

#ifdef  LE_HOST
    uint32  ENTRY_0_128_151:24,
            ENTRY_1_0_7:8;
#else
    uint32  ENTRY_1_0_7:8,
            ENTRY_0_128_151:24;
#endif  /* LE_HOST */

    uint32  ENTRY_1_8_39;
    uint32  ENTRY_1_40_71;
    uint32  ENTRY_1_72_103;
    uint32  ENTRY_1_104_135;

#ifdef  LE_HOST
    uint32  ENTRY_1_136_151:16,
            ENTRY_2_0_15:16;
#else
    uint32  ENTRY_2_0_15:16,
            ENTRY_1_136_151:16;
#endif  /* LE_HOST */

    uint32  ENTRY_2_16_47;
    uint32  ENTRY_2_48_79;
    uint32  ENTRY_2_80_111;
    uint32  ENTRY_2_112_143;

#ifdef  LE_HOST
    uint32  ENTRY_2_144_151:8,
            RESERVED:24;
#else
    uint32  RESERVED:24,
            ENTRY_2_144_151:8;
#endif  /* LE_HOST */
} th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT9_FULL_t;

typedef th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT1_FULL_t th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT10_FULL_t;
typedef th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT3_FULL_t th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT11_FULL_t;

typedef struct {
    uint32  ENTRY_0_0_31;
    uint32  ENTRY_0_32_63;
    uint32  ENTRY_0_64_95;
    uint32  ENTRY_0_96_127;
    uint32  ENTRY_0_128_159;

#ifdef  LE_HOST
    uint32  ENTRY_0_160_175:16,
            ENTRY_1_0_15:16;
#else
    uint32  ENTRY_1_0_15:16,
            ENTRY_0_160_175:16;
#endif  /* LE_HOST */

    uint32  ENTRY_1_16_47;
    uint32  ENTRY_1_48_79;
    uint32  ENTRY_1_80_111;
    uint32  ENTRY_1_112_143;
    uint32  ENTRY_1_144_175;

    uint32  RESERVED_0_31;
    uint32  RESERVED_32_63;
    uint32  RESERVED_64_95;
    uint32  RESERVED_96_127;
} th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT12_FULL_t;

static void
th3_format_field_get(int unit, soc_format_t format, const uint32 *entbuf,
                     soc_field_t field, uint32 *fldbuf)
{
    switch (format) {
    /* PIVOT format */
    case L3_DEFIP_ALPM_PIVOT_FMT1fmt: {
            th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT1_t *fmt1 = (void *)entbuf;
            if (field == PREFIXf) {
                fldbuf[0] = fmt1->PREFIX;
            } else if (field == ASSOC_DATAf) {
                fldbuf[0] = fmt1->ASSOC_DATA;
            } else if (field == ALPM2_DATAf) {
                fldbuf[0] = ((fmt1->ALPM2_DATA_8_39 & 0xffffff) << 8 ) | fmt1->ALPM2_DATA_0_7;
                fldbuf[1] = (fmt1->ALPM2_DATA_40_55 << 8) | ((fmt1->ALPM2_DATA_8_39 >> 24) & 0xff);
            }
        }
        break;
    case L3_DEFIP_ALPM_PIVOT_FMT2fmt: {
            th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT2_t *fmt2 = (void *)entbuf;
            if (field == PREFIXf) {
                fldbuf[0] = fmt2->PREFIX;
            } else if (field == ASSOC_DATAf) {
                fldbuf[0] = (fmt2->ASSOC_DATA_8_15 << 8) | fmt2->ASSOC_DATA_0_7;
            } else if (field == ALPM2_DATAf) {
                fldbuf[0] = ((fmt2->ALPM2_DATA_24_55 & 0xff) << 24) | fmt2->ALPM2_DATA_0_23;
                fldbuf[1] = (fmt2->ALPM2_DATA_24_55 >> 8) & 0xffffff;
            }
        }
        break;
    case L3_DEFIP_ALPM_PIVOT_FMT3fmt: {
            th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT3_t *fmt3 = (void *)entbuf;
            if (field == PREFIXf) {
                fldbuf[0] = fmt3->PREFIX_0_31;
                fldbuf[1] = fmt3->PREFIX_32_47;
            } else if (field == ASSOC_DATAf) {
                fldbuf[0] = fmt3->ASSOC_DATA;
            } else if (field == ALPM2_DATAf) {
                fldbuf[0] = fmt3->ALPM2_DATA_0_31;
                fldbuf[1] = fmt3->ALPM2_DATA_32_55;
            }
        }
        break;
    case L3_DEFIP_ALPM_PIVOT_FMT4fmt: {
            th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT4_t *fmt4 = (void *)entbuf;
            if (field == PREFIXf) {
                fldbuf[0] = fmt4->PREFIX_0_31;
                fldbuf[1] = fmt4->PREFIX_32_63;
                fldbuf[2] = fmt4->PREFIX_64_87;
            } else if (field == ASSOC_DATAf) {
                fldbuf[0] = (fmt4->ASSOC_DATA_8_15 << 8) | fmt4->ASSOC_DATA_0_7;
            } else if (field == ALPM2_DATAf) {
                fldbuf[0] = ((fmt4->ALPM2_DATA_24_55 & 0xff) << 24) | fmt4->ALPM2_DATA_0_23;
                fldbuf[1] = (fmt4->ALPM2_DATA_24_55 >> 8) & 0xffffff;
            }
        }
        break;
    case L3_DEFIP_ALPM_PIVOT_FMT5fmt: {
            th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT5_t *fmt5 = (void *)entbuf;
            if (field == PREFIXf) {
                fldbuf[0] = fmt5->PREFIX_0_31;
                fldbuf[1] = fmt5->PREFIX_32_63;
                fldbuf[2] = fmt5->PREFIX_64_95;
                fldbuf[3] = fmt5->PREFIX_96_127;
                fldbuf[4] = fmt5->PREFIX_128;
            } else if (field == ASSOC_DATAf) {
                fldbuf[0] = fmt5->ASSOC_DATA;
            } else if (field == ALPM2_DATAf) {
                fldbuf[0] = ((fmt5->ALPM2_DATA_8_39 & 0xffffff) << 8) | fmt5->ALPM2_DATA_0_7;
                fldbuf[1] = ((fmt5->ALPM2_DATA_40_55 & 0xffff) << 8) | ((fmt5->ALPM2_DATA_8_39 >> 24) & 0xff);
            }
        }
        break;
    case L3_DEFIP_ALPM_PIVOT_FMT6fmt: {
            th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT6_t *fmt6 = (void *)entbuf;
            if (field == PREFIXf) {
                fldbuf[0] = fmt6->PREFIX;
            } else if (field == ASSOC_DATAf) {
                fldbuf[0] = ((fmt6->ASSOC_DATA_14_45 & 0x3ffff) << 14) | fmt6->ASSOC_DATA_0_13;
                fldbuf[1] = (fmt6->ASSOC_DATA_14_45 >> 18) & 0x3fff;
            } else if (field == ALPM2_DATAf) {
                fldbuf[0] = fmt6->ALPM2_DATA_0_31;
                fldbuf[1] = fmt6->ALPM2_DATA_32_55;
            }
        }
        break;
    case L3_DEFIP_ALPM_PIVOT_FMT7fmt: {
            th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT7_t *fmt7 = (void *)entbuf;
            if (field == PREFIXf) {
                fldbuf[0] = fmt7->PREFIX_0_31;
                fldbuf[1] = fmt7->PREFIX_32_57;
            } else if (field == ASSOC_DATAf) {
                fldbuf[0] = ((fmt7->ASSOC_DATA_6_37 & 0x3ffffff) << 6) | fmt7->ASSOC_DATA_0_5;
                fldbuf[1] = (fmt7->ASSOC_DATA_6_37 >> 26) & 0x3f;
            } else if (field == ALPM2_DATAf) {
                fldbuf[0] = ((fmt7->ALPM2_DATA_24_55 & 0xff) << 24) | fmt7->ALPM2_DATA_0_23;
                fldbuf[1] = (fmt7->ALPM2_DATA_24_55 >> 8) & 0xffffff;
            }
        }
        break;
    case L3_DEFIP_ALPM_PIVOT_FMT8fmt: {
            th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT8_t *fmt8 = (void *)entbuf;
            if (field == PREFIXf) {
                fldbuf[0] = fmt8->PREFIX_0_31;
                fldbuf[1] = fmt8->PREFIX_32_63;
                fldbuf[2] = fmt8->PREFIX_64_95;
                fldbuf[3] = fmt8->PREFIX_96_127;
                fldbuf[4] = fmt8->PREFIX_128;
            } else if (field == ASSOC_DATAf) {
                fldbuf[0] = ((fmt8->ASSOC_DATA_30_45 & 0x3) << 30) | fmt8->ASSOC_DATA_0_29;
                fldbuf[1] = (fmt8->ASSOC_DATA_30_45 >> 2) & 0x3fff;
            } else if (field == ALPM2_DATAf) {
                fldbuf[0] = ((fmt8->ALPM2_DATA_16_47 & 0xffff) << 16) | fmt8->ALPM2_DATA_0_15;
                fldbuf[1] = ((fmt8->ALPM2_DATA_48_55 & 0xff) << 16) | ((fmt8->ALPM2_DATA_16_47 >> 16) & 0xffff);
            }
        }
        break;

    /* ROUTE format */
    case L3_DEFIP_ALPM_ROUTE_FMT1fmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT1_t *fmt1 = (void *)entbuf;
            if (field == PREFIXf) {
                fldbuf[0] = fmt1->PREFIX;
            } else if (field == ASSOC_DATAf) {
                fldbuf[0] = fmt1->ASSOC_DATA;
            }
        }
        break;
    case L3_DEFIP_ALPM_ROUTE_FMT2fmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT2_t *fmt2 = (void *)entbuf;
            if (field == PREFIXf) {
                fldbuf[0] = fmt2->PREFIX;
            } else if (field == ASSOC_DATAf) {
                fldbuf[0] = ((fmt2->ASSOC_DATA_8_15 & 0xff) << 8) | fmt2->ASSOC_DATA_0_7;
            }
        }
        break;
    case L3_DEFIP_ALPM_ROUTE_FMT3fmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT3_t *fmt3 = (void *)entbuf;
            if (field == PREFIXf) {
                fldbuf[0] = fmt3->PREFIX;
            } else if (field == ASSOC_DATAf) {
                fldbuf[0] = fmt3->ASSOC_DATA;
            }
        }
        break;
    case L3_DEFIP_ALPM_ROUTE_FMT4fmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT4_t *fmt4 = (void *)entbuf;
            if (field == PREFIXf) {
                fmt4->PREFIX_0_31 = fldbuf[0];
                fmt4->PREFIX_32_39 = fldbuf[1] & 0xff;
            } else if (field == ASSOC_DATAf) {
                fmt4->ASSOC_DATA = fldbuf[0];
            }
        }
        break;
    case L3_DEFIP_ALPM_ROUTE_FMT5fmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT5_t *fmt5 = (void *)entbuf;
            if (field == PREFIXf) {
                fldbuf[0] = fmt5->PREFIX_0_31;
                fldbuf[1] = fmt5->PREFIX_32_47;
            } else if (field == ASSOC_DATAf) {
                fldbuf[0] = fmt5->ASSOC_DATA;
            }
        }
        break;
    case L3_DEFIP_ALPM_ROUTE_FMT6fmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT6_t *fmt6 = (void *)entbuf;
            if (field == PREFIXf) {
                fldbuf[0] = fmt6->PREFIX_0_31;
                fldbuf[1] = fmt6->PREFIX_32_63;
            } else if (field == ASSOC_DATAf) {
                fldbuf[0] = fmt6->ASSOC_DATA;
            }
        }
        break;
    case L3_DEFIP_ALPM_ROUTE_FMT7fmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT7_t *fmt7 = (void *)entbuf;
            if (field == PREFIXf) {
                fldbuf[0] = fmt7->PREFIX_0_31;
                fldbuf[1] = fmt7->PREFIX_32_63;
                fldbuf[2] = fmt7->PREFIX_64_79;
            } else if (field == ASSOC_DATAf) {
                fldbuf[0] = fmt7->ASSOC_DATA;
            }
        }
        break;
    case L3_DEFIP_ALPM_ROUTE_FMT8fmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT8_t *fmt8 = (void *)entbuf;
            if (field == PREFIXf) {
                fldbuf[0] = fmt8->PREFIX_0_31;
                fldbuf[1] = fmt8->PREFIX_32_63;
                fldbuf[2] = fmt8->PREFIX_64_95;
                fldbuf[3] = fmt8->PREFIX_96_103;
            } else if (field == ASSOC_DATAf) {
                fldbuf[0] = fmt8->ASSOC_DATA;
            }
        }
        break;
    case L3_DEFIP_ALPM_ROUTE_FMT9fmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT9_t *fmt9 = (void *)entbuf;
            if (field == PREFIXf) {
                fldbuf[0] = fmt9->PREFIX_0_31;
                fldbuf[1] = fmt9->PREFIX_32_63;
                fldbuf[2] = fmt9->PREFIX_64_95;
                fldbuf[3] = fmt9->PREFIX_96_127;
                fldbuf[4] = fmt9->PREFIX_128;
            } else if (field == ASSOC_DATAf) {
                fldbuf[0] = fmt9->ASSOC_DATA;
            }
        }
        break;
    case L3_DEFIP_ALPM_ROUTE_FMT10fmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT10_t *fmt10 = (void *)entbuf;
            if (field == PREFIXf) {
                fldbuf[0] = fmt10->PREFIX_0_31;
                fldbuf[1] = fmt10->PREFIX_32_33;
            } else if (field == ASSOC_DATAf) {
                fldbuf[0] = ((fmt10->ASSOC_DATA_30_45 & 0x3) << 30) | fmt10->ASSOC_DATA_0_29;
                fldbuf[1] = (fmt10->ASSOC_DATA_30_45 >> 2) & 0x3fff;
            }
        }
        break;
    case L3_DEFIP_ALPM_ROUTE_FMT11fmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT11_t *fmt11 = (void *)entbuf;
            if (field == PREFIXf) {
                fldbuf[0] = fmt11->PREFIX_0_31;
                fldbuf[1] = fmt11->PREFIX_32_63;
                fldbuf[2] = fmt11->PREFIX_64_73;
            } else if (field == ASSOC_DATAf) {
                fldbuf[0] = ((fmt11->ASSOC_DATA_22_45 & 0x3ff) << 22) | fmt11->ASSOC_DATA_0_21;
                fldbuf[1] = (fmt11->ASSOC_DATA_22_45 >> 10) & 0x3fff;
            }
        }
        break;
    case L3_DEFIP_ALPM_ROUTE_FMT12fmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT12_t *fmt12 = (void *)entbuf;
            if (field == PREFIXf) {
                fldbuf[0] = fmt12->PREFIX_0_31;
                fldbuf[1] = fmt12->PREFIX_32_63;
                fldbuf[2] = fmt12->PREFIX_64_95;
                fldbuf[3] = fmt12->PREFIX_96_127;
                fldbuf[4] = fmt12->PREFIX_128;
            } else if (field == ASSOC_DATAf) {
                fldbuf[0] = ((fmt12->ASSOC_DATA_30_45 & 0x3) << 30) | fmt12->ASSOC_DATA_0_29;
                fldbuf[1] = (fmt12->ASSOC_DATA_30_45 >> 2) & 0x3fff;
            }
        }
        break;

    /* PIVOT FULL format */
    case L3_DEFIP_ALPM_PIVOT_FMT1_FULLfmt:
    case L3_DEFIP_ALPM_ROUTE_FMT6_FULLfmt:
    case L3_DEFIP_ALPM_ROUTE_FMT10_FULLfmt: {
            th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT1_FULL_t *fmt1pf = (void *)entbuf;
            if (field == ENTRY_0f) {
                fldbuf[0] = fmt1pf->ENTRY_0_0_31;
                fldbuf[1] = fmt1pf->ENTRY_0_32_63;
                fldbuf[2] = fmt1pf->ENTRY_0_64_79;
            } else if (field == ENTRY_1f) {
                fldbuf[0] = ((fmt1pf->ENTRY_1_16_47 & 0xffff) << 16) | fmt1pf->ENTRY_1_0_15;
                fldbuf[1] = ((fmt1pf->ENTRY_1_48_79 & 0xffff) << 16) | ((fmt1pf->ENTRY_1_16_47 >> 16) & 0xffff);
                fldbuf[2] = (fmt1pf->ENTRY_1_48_79 >> 16) & 0xffff;
            } else if (field == ENTRY_2f) {
                fldbuf[0] = fmt1pf->ENTRY_2_0_31;
                fldbuf[1] = fmt1pf->ENTRY_2_32_63;
                fldbuf[2] = fmt1pf->ENTRY_2_64_79;
            } else if (field == ENTRY_3f) {
                fldbuf[0] = ((fmt1pf->ENTRY_3_16_47 & 0xffff) << 16) | fmt1pf->ENTRY_3_0_15;
                fldbuf[1] = ((fmt1pf->ENTRY_3_48_79 & 0xffff) << 16) | ((fmt1pf->ENTRY_3_16_47 >> 16) & 0xffff);
                fldbuf[2] = (fmt1pf->ENTRY_3_48_79 >> 16) & 0xffff;
            } else if (field == ENTRY_4f) {
                fldbuf[0] = fmt1pf->ENTRY_4_0_31;
                fldbuf[1] = fmt1pf->ENTRY_4_32_63;
                fldbuf[2] = fmt1pf->ENTRY_4_64_79;
            } else if (field == ENTRY_5f) {
                fldbuf[0] = ((fmt1pf->ENTRY_5_16_47 & 0xffff) << 16) | fmt1pf->ENTRY_5_0_15;
                fldbuf[1] = ((fmt1pf->ENTRY_5_48_79 & 0xffff) << 16) | ((fmt1pf->ENTRY_5_16_47 >> 16) & 0xffff);
                fldbuf[2] = (fmt1pf->ENTRY_5_48_79 >> 16) & 0xffff;
            }
        }
        break;

    case L3_DEFIP_ALPM_PIVOT_FMT2_FULLfmt:
    case L3_DEFIP_ALPM_ROUTE_FMT7_FULLfmt: {
            th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT2_FULL_t *fmt2pf = (void *)entbuf;
            if (field == ENTRY_0f) {
                fldbuf[0] = fmt2pf->ENTRY_0_0_31;
                fldbuf[1] = fmt2pf->ENTRY_0_32_63;
                fldbuf[2] = fmt2pf->ENTRY_0_64_95;
            } else if (field == ENTRY_1f) {
                fldbuf[0] = fmt2pf->ENTRY_1_0_31;
                fldbuf[1] = fmt2pf->ENTRY_1_32_63;
                fldbuf[2] = fmt2pf->ENTRY_1_64_95;
            } else if (field == ENTRY_2f) {
                fldbuf[0] = fmt2pf->ENTRY_2_0_31;
                fldbuf[1] = fmt2pf->ENTRY_2_32_63;
                fldbuf[2] = fmt2pf->ENTRY_2_64_95;
            } else if (field == ENTRY_3f) {
                fldbuf[0] = fmt2pf->ENTRY_3_0_31;
                fldbuf[1] = fmt2pf->ENTRY_3_32_63;
                fldbuf[2] = fmt2pf->ENTRY_3_64_95;
            } else if (field == ENTRY_4f) {
                fldbuf[0] = fmt2pf->ENTRY_4_0_31;
                fldbuf[1] = fmt2pf->ENTRY_4_32_63;
                fldbuf[2] = fmt2pf->ENTRY_4_64_95;
            }
        }
        break;

    case L3_DEFIP_ALPM_PIVOT_FMT3_FULLfmt:
    case L3_DEFIP_ALPM_PIVOT_FMT6_FULLfmt:
    case L3_DEFIP_ALPM_ROUTE_FMT8_FULLfmt:
    case L3_DEFIP_ALPM_ROUTE_FMT11_FULLfmt: {
            th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT3_FULL_t *fmt3pf = (void *)entbuf;
            if (field == ENTRY_0f) {
                fldbuf[0] = fmt3pf->ENTRY_0_0_31;
                fldbuf[1] = fmt3pf->ENTRY_0_32_63;
                fldbuf[2] = fmt3pf->ENTRY_0_64_95;
                fldbuf[3] = fmt3pf->ENTRY_0_96_119;
            } else if (field == ENTRY_1f) {
                fldbuf[0] = ((fmt3pf->ENTRY_1_8_39 & 0xffffff) << 8) | fmt3pf->ENTRY_1_0_7;
                fldbuf[1] = ((fmt3pf->ENTRY_1_40_71 & 0xffffff) << 8) | ((fmt3pf->ENTRY_1_8_39 >> 24) & 0xff);
                fldbuf[2] = ((fmt3pf->ENTRY_1_72_103 & 0xffffff) << 8) | ((fmt3pf->ENTRY_1_40_71 >> 24) & 0xff);
                fldbuf[3] = (fmt3pf->ENTRY_1_104_119 << 8) | ((fmt3pf->ENTRY_1_72_103 >> 24) & 0xff);
            } else if (field == ENTRY_2f) {
                fldbuf[0] = ((fmt3pf->ENTRY_2_16_47 & 0xffff) << 16) | fmt3pf->ENTRY_2_0_15;
                fldbuf[1] = ((fmt3pf->ENTRY_2_48_79 & 0xffff) << 16) | ((fmt3pf->ENTRY_2_16_47 >> 16) & 0xffff);
                fldbuf[2] = ((fmt3pf->ENTRY_2_80_111 & 0xffff) << 16) | ((fmt3pf->ENTRY_2_48_79 >> 16) & 0xffff);
                fldbuf[3] = (fmt3pf->ENTRY_2_112_119 << 16) | ((fmt3pf->ENTRY_2_80_111 >> 16) & 0xffff);
            } else if (field == ENTRY_3f) {
                fldbuf[0] = ((fmt3pf->ENTRY_3_24_55 & 0xff) << 24) | fmt3pf->ENTRY_3_0_23;
                fldbuf[1] = ((fmt3pf->ENTRY_3_56_87 & 0xff) << 24) | ((fmt3pf->ENTRY_3_24_55 >> 8) & 0xffffff);
                fldbuf[2] = ((fmt3pf->ENTRY_3_88_119 & 0xff) << 24) | ((fmt3pf->ENTRY_3_56_87 >> 8) & 0xffffff);
                fldbuf[3] = (fmt3pf->ENTRY_3_88_119 >> 8) & 0xffffff;
            }
        }
        break;

    case L3_DEFIP_ALPM_PIVOT_FMT4_FULLfmt:
    case L3_DEFIP_ALPM_PIVOT_FMT7_FULLfmt: {
            th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT4_FULL_t *fmt4pf = (void *)entbuf;
            if (field == ENTRY_0f) {
                fldbuf[0] = fmt4pf->ENTRY_0_0_31;
                fldbuf[1] = fmt4pf->ENTRY_0_32_63;
                fldbuf[2] = fmt4pf->ENTRY_0_64_95;
                fldbuf[3] = fmt4pf->ENTRY_0_96_127;
                fldbuf[4] = fmt4pf->ENTRY_0_128_159;
            } else if (field == ENTRY_1f) {
                fldbuf[0] = fmt4pf->ENTRY_1_0_31;
                fldbuf[1] = fmt4pf->ENTRY_1_32_63;
                fldbuf[2] = fmt4pf->ENTRY_1_64_95;
                fldbuf[3] = fmt4pf->ENTRY_1_96_127;
                fldbuf[4] = fmt4pf->ENTRY_1_128_159;
            } else if (field == ENTRY_2f) {
                fldbuf[0] = fmt4pf->ENTRY_2_0_31;
                fldbuf[1] = fmt4pf->ENTRY_2_32_63;
                fldbuf[2] = fmt4pf->ENTRY_2_64_95;
                fldbuf[3] = fmt4pf->ENTRY_2_96_127;
                fldbuf[4] = fmt4pf->ENTRY_2_128_159;
            }
        }
        break;

    case L3_DEFIP_ALPM_PIVOT_FMT5_FULLfmt: {
            th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT5_FULL_t *fmt5pf = (void *)entbuf;
            if (field == ENTRY_0f) {
                fldbuf[0] = fmt5pf->ENTRY_0_0_31;
                fldbuf[1] = fmt5pf->ENTRY_0_32_63;
                fldbuf[2] = fmt5pf->ENTRY_0_64_95;
                fldbuf[3] = fmt5pf->ENTRY_0_96_127;
                fldbuf[4] = fmt5pf->ENTRY_0_128_159;
                fldbuf[5] = fmt5pf->ENTRY_0_160_191;
                fldbuf[6] = fmt5pf->ENTRY_0_192_207;
            } else if (field == ENTRY_1f) {
                fldbuf[0] = ((fmt5pf->ENTRY_1_16_47 & 0xffff) >> 16) | fmt5pf->ENTRY_1_0_15;
                fldbuf[1] = ((fmt5pf->ENTRY_1_48_79 & 0xffff) >> 16) | ((fmt5pf->ENTRY_1_16_47 >> 16) & 0xffff);
                fldbuf[2] = ((fmt5pf->ENTRY_1_80_111 & 0xffff) >> 16) | ((fmt5pf->ENTRY_1_48_79 >> 16) & 0xffff);
                fldbuf[3] = ((fmt5pf->ENTRY_1_112_143 & 0xffff) >> 16) | ((fmt5pf->ENTRY_1_80_111 >> 16) & 0xffff);
                fldbuf[4] = ((fmt5pf->ENTRY_1_144_175 & 0xffff) >> 16) | ((fmt5pf->ENTRY_1_112_143 >> 16) & 0xffff);
                fldbuf[5] = ((fmt5pf->ENTRY_1_176_207 & 0xffff) >> 16) | ((fmt5pf->ENTRY_1_144_175 >> 16) & 0xffff);
                fldbuf[6] = (fmt5pf->ENTRY_1_176_207 >> 16) & 0xffff;
            }
        }
        break;

    case L3_DEFIP_ALPM_PIVOT_FMT8_FULLfmt: {
            th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT8_FULL_t *fmt8pf = (void *)entbuf;
            if (field == ENTRY_0f) {
                fldbuf[0] = fmt8pf->ENTRY_0_0_31;
                fldbuf[1] = fmt8pf->ENTRY_0_32_63;
                fldbuf[2] = fmt8pf->ENTRY_0_64_95;
                fldbuf[3] = fmt8pf->ENTRY_0_96_127;
                fldbuf[4] = fmt8pf->ENTRY_0_128_159;
                fldbuf[5] = fmt8pf->ENTRY_0_160_191;
                fldbuf[6] = fmt8pf->ENTRY_0_192_223;
                fldbuf[7] = fmt8pf->ENTRY_0_224_231;
            } else if (field == ENTRY_1f) {
                fldbuf[0] = ((fmt8pf->ENTRY_1_24_55 & 0xff) << 24) | fmt8pf->ENTRY_1_0_23;
                fldbuf[1] = ((fmt8pf->ENTRY_1_56_87 & 0xff) << 24) | ((fmt8pf->ENTRY_1_24_55 >> 8) & 0xffffff);
                fldbuf[2] = ((fmt8pf->ENTRY_1_88_119 & 0xff) << 24) | ((fmt8pf->ENTRY_1_56_87 >> 8) & 0xffffff);
                fldbuf[3] = ((fmt8pf->ENTRY_1_120_151 & 0xff) << 24) | ((fmt8pf->ENTRY_1_88_119 >> 8) & 0xffffff);
                fldbuf[4] = ((fmt8pf->ENTRY_1_152_183 & 0xff) << 24) | ((fmt8pf->ENTRY_1_120_151 >> 8) & 0xffffff);
                fldbuf[5] = ((fmt8pf->ENTRY_1_184_215 & 0xff) << 24) | ((fmt8pf->ENTRY_1_152_183 >> 8) & 0xffffff);
                fldbuf[6] = ((fmt8pf->ENTRY_1_216_231 & 0xff) << 24) | ((fmt8pf->ENTRY_1_184_215 >> 8) & 0xffffff);
                fldbuf[7] = (fmt8pf->ENTRY_1_216_231 >> 8) & 0xff;
            }
        }
        break;

    /* ROUTE FULL fmt */
    case L3_DEFIP_ALPM_ROUTE_FMT1_FULLfmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT1_FULL_t *fmt1rf = (void *)entbuf;
            if (field == ENTRY_0f) {
                fldbuf[0] = fmt1rf->ENTRY_0;
            } else if (field == ENTRY_1f) {
                fldbuf[0] = fmt1rf->ENTRY_1;
            } else if (field == ENTRY_2f) {
                fldbuf[0] = fmt1rf->ENTRY_2;
            } else if (field == ENTRY_3f) {
                fldbuf[0] = fmt1rf->ENTRY_3;
            } else if (field == ENTRY_4f) {
                fldbuf[0] = fmt1rf->ENTRY_4;
            } else if (field == ENTRY_5f) {
                fldbuf[0] = fmt1rf->ENTRY_5;
            } else if (field == ENTRY_6f) {
                fldbuf[0] = fmt1rf->ENTRY_6;
            } else if (field == ENTRY_7f) {
                fldbuf[0] = fmt1rf->ENTRY_7;
            } else if (field == ENTRY_8f) {
                fldbuf[0] = fmt1rf->ENTRY_8;
            } else if (field == ENTRY_9f) {
                fldbuf[0] = fmt1rf->ENTRY_9;
            } else if (field == ENTRY_10f) {
                fldbuf[0] = fmt1rf->ENTRY_10;
            } else if (field == ENTRY_11f) {
                fldbuf[0] = fmt1rf->ENTRY_11;
            } else if (field == ENTRY_12f) {
                fldbuf[0] = fmt1rf->ENTRY_12;
            } else if (field == ENTRY_13f) {
                fldbuf[0] = fmt1rf->ENTRY_13;
            } else if (field == ENTRY_14f) {
                fldbuf[0] = fmt1rf->ENTRY_14;
            }
        }
        break;

    case L3_DEFIP_ALPM_ROUTE_FMT2_FULLfmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT2_FULL_t *fmt2rf = (void *)entbuf;
            if (field == ENTRY_0f) {
                fldbuf[0] = fmt2rf->ENTRY_0_0_31;
                fldbuf[1] = fmt2rf->ENTRY_0_32_39;
            } else if (field == ENTRY_1f) {
                fldbuf[0] = ((fmt2rf->ENTRY_1_24_39 & 0xff) << 24) | fmt2rf->ENTRY_1_0_23;
                fldbuf[1] = (fmt2rf->ENTRY_1_24_39 >> 8) & 0xff;
            } else if (field == ENTRY_2f) {
                fldbuf[0] = ((fmt2rf->ENTRY_2_16_39 & 0xffff) << 16) | fmt2rf->ENTRY_2_0_15;
                fldbuf[1] = (fmt2rf->ENTRY_2_16_39 >> 16) & 0xff;
            } else if (field == ENTRY_3f) {
                fldbuf[0] = ((fmt2rf->ENTRY_3_8_39 & 0xffffff) << 8) | fmt2rf->ENTRY_3_0_7;
                fldbuf[1] = (fmt2rf->ENTRY_3_8_39 >> 24) & 0xff;
            } else if (field == ENTRY_4f) {
                fldbuf[0] = fmt2rf->ENTRY_4_0_31;
                fldbuf[1] = fmt2rf->ENTRY_4_32_39;
            } else if (field == ENTRY_5f) {
                fldbuf[0] = ((fmt2rf->ENTRY_5_24_39 & 0xff) << 24) | fmt2rf->ENTRY_5_0_23;
                fldbuf[1] = (fmt2rf->ENTRY_5_24_39 >> 8) & 0xff;
            } else if (field == ENTRY_6f) {
                fldbuf[0] = ((fmt2rf->ENTRY_6_16_39 & 0xffff) << 16) | fmt2rf->ENTRY_6_0_15;
                fldbuf[1] = (fmt2rf->ENTRY_6_16_39 >> 16) & 0xff;
            } else if (field == ENTRY_7f) {
                fldbuf[0] = ((fmt2rf->ENTRY_7_8_39 & 0xffffff) << 8) | fmt2rf->ENTRY_7_0_7;
                fldbuf[1] = (fmt2rf->ENTRY_7_8_39 >> 24) & 0xff;
            } else if (field == ENTRY_8f) {
                fldbuf[0] = fmt2rf->ENTRY_8_0_31;
                fldbuf[1] = fmt2rf->ENTRY_8_32_39;
            } else if (field == ENTRY_9f) {
                fldbuf[0] = ((fmt2rf->ENTRY_9_24_39 & 0xff) << 24) | fmt2rf->ENTRY_9_0_23;
                fldbuf[1] = (fmt2rf->ENTRY_9_24_39 >> 8) & 0xff;
            } else if (field == ENTRY_10f) {
                fldbuf[0] = ((fmt2rf->ENTRY_10_16_39 & 0xffff) << 16) | fmt2rf->ENTRY_10_0_15;
                fldbuf[1] = (fmt2rf->ENTRY_10_16_39 >> 16) & 0xff;
            } else if (field == ENTRY_11f) {
                fldbuf[0] = ((fmt2rf->ENTRY_11_8_39 & 0xffffff) << 8) | fmt2rf->ENTRY_11_0_7;
                fldbuf[1] = (fmt2rf->ENTRY_11_8_39 >> 24) & 0xff;
            }
        }
        break;

    case L3_DEFIP_ALPM_ROUTE_FMT3_FULLfmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT3_FULL_t *fmt3rf = (void *)entbuf;
            if (field == ENTRY_0f) {
                fldbuf[0] = fmt3rf->ENTRY_0_0_31;
                fldbuf[1] = fmt3rf->ENTRY_0_32_47;
            } else if (field == ENTRY_1f) {
                fldbuf[0] = ((fmt3rf->ENTRY_1_16_47 & 0xffff) << 16) | ((fmt3rf->ENTRY_1_0_15 >> 16) & 0xffff);
                fldbuf[1] = (fmt3rf->ENTRY_1_16_47 >> 16) & 0xffff;
            } else if (field == ENTRY_2f) {
                fldbuf[0] = fmt3rf->ENTRY_2_0_31;
                fldbuf[1] = fmt3rf->ENTRY_2_32_47;
            } else if (field == ENTRY_3f) {
                fldbuf[0] = ((fmt3rf->ENTRY_3_16_47 & 0xffff) << 16) | ((fmt3rf->ENTRY_3_0_15 >> 16) & 0xffff);
                fldbuf[1] = (fmt3rf->ENTRY_3_16_47 >> 16) & 0xffff;
            } else if (field == ENTRY_4f) {
                fldbuf[0] = fmt3rf->ENTRY_4_0_31;
                fldbuf[1] = fmt3rf->ENTRY_4_32_47;
            } else if (field == ENTRY_5f) {
                fldbuf[0] = ((fmt3rf->ENTRY_5_16_47 & 0xffff) << 16) | ((fmt3rf->ENTRY_5_0_15 >> 16) & 0xffff);
                fldbuf[1] = (fmt3rf->ENTRY_5_16_47 >> 16) & 0xffff;
            } else if (field == ENTRY_6f) {
                fldbuf[0] = fmt3rf->ENTRY_6_0_31;
                fldbuf[1] = fmt3rf->ENTRY_6_32_47;
            } else if (field == ENTRY_7f) {
                fldbuf[0] = ((fmt3rf->ENTRY_7_16_47 & 0xffff) << 16) | ((fmt3rf->ENTRY_7_0_15 >> 16) & 0xffff);
                fldbuf[1] = (fmt3rf->ENTRY_7_16_47 >> 16) & 0xffff;
            } else if (field == ENTRY_8f) {
                fldbuf[0] = fmt3rf->ENTRY_8_0_31;
                fldbuf[1] = fmt3rf->ENTRY_8_32_47;
            } else if (field == ENTRY_9f) {
                fldbuf[0] = ((fmt3rf->ENTRY_9_16_47 & 0xffff) << 16) | ((fmt3rf->ENTRY_9_0_15 >> 16) & 0xffff);
                fldbuf[1] = (fmt3rf->ENTRY_9_16_47 >> 16) & 0xffff;
            }
        }
        break;

    case L3_DEFIP_ALPM_ROUTE_FMT4_FULLfmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT4_FULL_t *fmt4rf = (void *)entbuf;
            if (field == ENTRY_0f) {
                fldbuf[0] = fmt4rf->ENTRY_0_0_31;
                fldbuf[1] = fmt4rf->ENTRY_0_32_55;
            } else if (field == ENTRY_1f) {
                fldbuf[0] = ((fmt4rf->ENTRY_1_8_39 & 0xffffff) << 8) | fmt4rf->ENTRY_1_0_7;
                fldbuf[1] = (fmt4rf->ENTRY_1_40_55 << 8) | ((fmt4rf->ENTRY_1_8_39 >> 24) & 0xff);
            } else if (field == ENTRY_2f) {
                fldbuf[0] = ((fmt4rf->ENTRY_2_16_47 & 0xffff) << 16) | fmt4rf->ENTRY_2_0_15;
                fldbuf[1] = (fmt4rf->ENTRY_2_48_55 << 16) | ((fmt4rf->ENTRY_2_16_47 >> 16) & 0xffff);
            } else if (field == ENTRY_3f) {
                fldbuf[0] = ((fmt4rf->ENTRY_3_24_55 & 0xff) << 24) | fmt4rf->ENTRY_3_0_23;
                fldbuf[1] = (fmt4rf->ENTRY_3_24_55 >> 8) & 0xffffff;
            } else if (field == ENTRY_4f) {
                fldbuf[0] = fmt4rf->ENTRY_4_0_31;
                fldbuf[1] = fmt4rf->ENTRY_4_32_55;
            } else if (field == ENTRY_5f) {
                fldbuf[0] = ((fmt4rf->ENTRY_5_8_39 & 0xffffff) << 8) | fmt4rf->ENTRY_5_0_7;
                fldbuf[1] = (fmt4rf->ENTRY_5_40_55 << 8) | ((fmt4rf->ENTRY_5_8_39 >> 24) & 0xff);
            } else if (field == ENTRY_6f) {
                fldbuf[0] = ((fmt4rf->ENTRY_6_16_47 & 0xffff) << 16) | fmt4rf->ENTRY_6_0_15;
                fldbuf[1] = (fmt4rf->ENTRY_6_48_55 << 16) | ((fmt4rf->ENTRY_6_16_47 >> 16) & 0xffff);
            } else if (field == ENTRY_7f) {
                fldbuf[0] = ((fmt4rf->ENTRY_7_24_55 & 0xff) << 24) | fmt4rf->ENTRY_7_0_23;
                fldbuf[1] = (fmt4rf->ENTRY_7_24_55 >> 8) & 0xffffff;
            }
        }
        break;

    case L3_DEFIP_ALPM_ROUTE_FMT5_FULLfmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT5_FULL_t *fmt5rf = (void *)entbuf;
            if (field == ENTRY_0f) {
                fldbuf[0] = fmt5rf->ENTRY_0_0_31;
                fldbuf[1] = fmt5rf->ENTRY_0_32_63;
            } else if (field == ENTRY_1f) {
                fldbuf[0] = fmt5rf->ENTRY_1_0_31;
                fldbuf[1] = fmt5rf->ENTRY_1_32_63;
            } else if (field == ENTRY_2f) {
                fldbuf[0] = fmt5rf->ENTRY_2_0_31;
                fldbuf[1] = fmt5rf->ENTRY_2_32_63;
            } else if (field == ENTRY_3f) {
                fldbuf[0] = fmt5rf->ENTRY_3_0_31;
                fldbuf[1] = fmt5rf->ENTRY_3_32_63;
            } else if (field == ENTRY_4f) {
                fldbuf[0] = fmt5rf->ENTRY_4_0_31;
                fldbuf[1] = fmt5rf->ENTRY_4_32_63;
            } else if (field == ENTRY_5f) {
                fldbuf[0] = fmt5rf->ENTRY_5_0_31;
                fldbuf[1] = fmt5rf->ENTRY_5_32_63;
            } else if (field == ENTRY_6f) {
                fldbuf[0] = fmt5rf->ENTRY_6_0_31;
                fldbuf[1] = fmt5rf->ENTRY_6_32_63;
            }
        }
        break;

    case L3_DEFIP_ALPM_ROUTE_FMT9_FULLfmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT9_FULL_t *fmt9rf = (void *)entbuf;
            if (field == ENTRY_0f) {
                fldbuf[0] = fmt9rf->ENTRY_0_0_31;
                fldbuf[1] = fmt9rf->ENTRY_0_32_63;
                fldbuf[2] = fmt9rf->ENTRY_0_64_95;
                fldbuf[3] = fmt9rf->ENTRY_0_96_127;
                fldbuf[4] = fmt9rf->ENTRY_0_128_151;
            } else if (field == ENTRY_1f) {
                fldbuf[0] = ((fmt9rf->ENTRY_1_8_39 & 0xffffff) << 8) | fmt9rf->ENTRY_1_0_7;
                fldbuf[1] = ((fmt9rf->ENTRY_1_40_71 & 0xffffff) << 8) | ((fmt9rf->ENTRY_1_8_39 << 24) & 0xff);
                fldbuf[2] = ((fmt9rf->ENTRY_1_72_103 & 0xffffff) << 8) | ((fmt9rf->ENTRY_1_40_71 << 24) & 0xff);
                fldbuf[3] = ((fmt9rf->ENTRY_1_104_135 & 0xffffff) << 8) | ((fmt9rf->ENTRY_1_72_103 << 24) & 0xff);
                fldbuf[4] = (fmt9rf->ENTRY_1_136_151 << 8) | ((fmt9rf->ENTRY_1_104_135 << 24) & 0xff);
            } else if (field == ENTRY_2f) {
                fldbuf[0] = ((fmt9rf->ENTRY_2_16_47 & 0xffff) << 16) | fmt9rf->ENTRY_2_0_15;
                fldbuf[1] = ((fmt9rf->ENTRY_2_48_79 & 0xffff) << 16) | ((fmt9rf->ENTRY_2_16_47 << 16) & 0xffff);
                fldbuf[2] = ((fmt9rf->ENTRY_2_80_111 & 0xffff) << 16) | ((fmt9rf->ENTRY_2_48_79 << 16) & 0xffff);
                fldbuf[3] = ((fmt9rf->ENTRY_2_112_143 & 0xffff) << 16) | ((fmt9rf->ENTRY_2_80_111 << 16) & 0xffff);
                fldbuf[4] = (fmt9rf->ENTRY_2_144_151 << 16) | ((fmt9rf->ENTRY_2_112_143 << 16) & 0xffff);
            }
        }
        break;

    case L3_DEFIP_ALPM_ROUTE_FMT12_FULLfmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT12_FULL_t *fmt12rf = (void *)entbuf;
            if (field == ENTRY_0f) {
                fldbuf[0] = fmt12rf->ENTRY_0_0_31;
                fldbuf[1] = fmt12rf->ENTRY_0_32_63;
                fldbuf[2] = fmt12rf->ENTRY_0_64_95;
                fldbuf[3] = fmt12rf->ENTRY_0_96_127;
                fldbuf[4] = fmt12rf->ENTRY_0_128_159;
                fldbuf[5] = fmt12rf->ENTRY_0_160_175;
            } else if (field == ENTRY_1f) {
                fldbuf[0] = ((fmt12rf->ENTRY_1_16_47 & 0xffff) >> 16) | fmt12rf->ENTRY_1_0_15;
                fldbuf[1] = ((fmt12rf->ENTRY_1_48_79 & 0xffff) >> 16) | ((fmt12rf->ENTRY_1_16_47 >> 16) & 0xffff);
                fldbuf[2] = ((fmt12rf->ENTRY_1_80_111 & 0xffff) >> 16) | ((fmt12rf->ENTRY_1_48_79 >> 16) & 0xffff);
                fldbuf[3] = ((fmt12rf->ENTRY_1_112_143 & 0xffff) >> 16) | ((fmt12rf->ENTRY_1_80_111 >> 16) & 0xffff);
                fldbuf[4] = ((fmt12rf->ENTRY_1_144_175 & 0xffff) >> 16) | ((fmt12rf->ENTRY_1_112_143 >> 16) & 0xffff);
                fldbuf[5] = (fmt12rf->ENTRY_1_144_175 >> 16) & 0xffff;
            }
        }
        break;

    default:
        assert(0);
        break;
    }
}

static void
th3_format_field_set(int u, soc_format_t format, uint32 *entbuf, soc_field_t field, uint32 *fldbuf)
{
    switch (format) {
    /* PIVOT format */
    case L3_DEFIP_ALPM_PIVOT_FMT1fmt: {
            th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT1_t *fmt1 = (void *)entbuf;
            if (field == PREFIXf) {
                fmt1->PREFIX = fldbuf[0];
            } else if (field == ASSOC_DATAf) {
                fmt1->ASSOC_DATA = fldbuf[0];
            } else if (field == ALPM2_DATAf) {
                fmt1->ALPM2_DATA_0_7 = fldbuf[0] & 0xff;
                fmt1->ALPM2_DATA_8_39 = ((fldbuf[1] & 0xff) << 24) | ((fldbuf[0] >> 8) & 0xffffff);
                fmt1->ALPM2_DATA_40_55 = (fldbuf[1] >> 8) & 0xffff;
            }
        }
        break;
    case L3_DEFIP_ALPM_PIVOT_FMT2fmt: {
            th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT2_t *fmt2 = (void *)entbuf;
            if (field == PREFIXf) {
                fmt2->PREFIX = fldbuf[0];
            } else if (field == ASSOC_DATAf) {
                fmt2->ASSOC_DATA_0_7 = fldbuf[0] & 0xff;
                fmt2->ASSOC_DATA_8_15 = (fldbuf[0] >> 8) & 0xff;
            } else if (field == ALPM2_DATAf) {
                fmt2->ALPM2_DATA_0_23 = fldbuf[0] & 0xffffff;
                fmt2->ALPM2_DATA_24_55 = ((fldbuf[1] & 0xffffff) << 8) | ((fldbuf[0] >> 24) & 0xff);
            }
        }
        break;
    case L3_DEFIP_ALPM_PIVOT_FMT3fmt: {
            th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT3_t *fmt3 = (void *)entbuf;
            if (field == PREFIXf) {
                fmt3->PREFIX_0_31 = fldbuf[0];
                fmt3->PREFIX_32_47 = fldbuf[1] & 0xffff;
            } else if (field == ASSOC_DATAf) {
                fmt3->ASSOC_DATA = fldbuf[0];
            } else if (field == ALPM2_DATAf) {
                fmt3->ALPM2_DATA_0_31 = fldbuf[0];
                fmt3->ALPM2_DATA_32_55 = fldbuf[1] & 0xffffff;
            }
        }
        break;
    case L3_DEFIP_ALPM_PIVOT_FMT4fmt: {
            th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT4_t *fmt4 = (void *)entbuf;
            if (field == PREFIXf) {
                fmt4->PREFIX_0_31 = fldbuf[0];
                fmt4->PREFIX_32_63 = fldbuf[1];
                fmt4->PREFIX_64_87 = fldbuf[2] & 0xffffff;
            } else if (field == ASSOC_DATAf) {
                fmt4->ASSOC_DATA_0_7 = fldbuf[0] & 0xff;
                fmt4->ASSOC_DATA_8_15 = (fldbuf[0] >> 8) & 0xff;
            } else if (field == ALPM2_DATAf) {
                fmt4->ALPM2_DATA_0_23 = fldbuf[0] & 0xffffff;
                fmt4->ALPM2_DATA_24_55 = ((fldbuf[1] & 0xffffff) << 8) | ((fldbuf[0] >> 24) & 0xff);
            }
        }
        break;
    case L3_DEFIP_ALPM_PIVOT_FMT5fmt: {
            th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT5_t *fmt5 = (void *)entbuf;
            if (field == PREFIXf) {
                fmt5->PREFIX_0_31 = fldbuf[0];
                fmt5->PREFIX_32_63 = fldbuf[1];
                fmt5->PREFIX_64_95 = fldbuf[2];
                fmt5->PREFIX_96_127 = fldbuf[3];
                fmt5->PREFIX_128 = fldbuf[4] & 0x1;
            } else if (field == ASSOC_DATAf) {
                fmt5->ASSOC_DATA = fldbuf[0];
            } else if (field == ALPM2_DATAf) {
                fmt5->ALPM2_DATA_0_7 = fldbuf[0] & 0xff;
                fmt5->ALPM2_DATA_8_39 = ((fldbuf[1] & 0xff) << 24) | ((fldbuf[0] >> 8) & 0xffffff);
                fmt5->ALPM2_DATA_40_55 = (fldbuf[1] >> 8) & 0xffff;
            }
        }
        break;
    case L3_DEFIP_ALPM_PIVOT_FMT6fmt: {
            th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT6_t *fmt6 = (void *)entbuf;
            if (field == PREFIXf) {
                fmt6->PREFIX = fldbuf[0];
            } else if (field == ASSOC_DATAf) {
                fmt6->ASSOC_DATA_0_13 = fldbuf[0] & 0x3fff;
                fmt6->ASSOC_DATA_14_45 = ((fldbuf[1] & 0x3fff) << 18) | ((fldbuf[0] >> 14) & 0x3ffff);
            } else if (field == ALPM2_DATAf) {
                fmt6->ALPM2_DATA_0_31 = fldbuf[0];
                fmt6->ALPM2_DATA_32_55 = fldbuf[1] & 0xffffff;
            }
        }
        break;
    case L3_DEFIP_ALPM_PIVOT_FMT7fmt: {
            th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT7_t *fmt7 = (void *)entbuf;
            if (field == PREFIXf) {
                fmt7->PREFIX_0_31 = fldbuf[0];
                fmt7->PREFIX_32_57 = fldbuf[1] & 0xffffff;
            } else if (field == ASSOC_DATAf) {
                fmt7->ASSOC_DATA_0_5 = fldbuf[0] & 0x3f;
                fmt7->ASSOC_DATA_6_37 = ((fldbuf[1] & 0x3f) << 26) | ((fldbuf[0] >> 6) & 0x3ffffff);
            } else if (field == ALPM2_DATAf) {
                fmt7->ALPM2_DATA_0_23 = fldbuf[0] & 0xffffff;
                fmt7->ALPM2_DATA_24_55 = ((fldbuf[1] & 0xffffff) << 8) | ((fldbuf[0] >> 24) & 0xff);
            }
        }
        break;
    case L3_DEFIP_ALPM_PIVOT_FMT8fmt: {
            th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT8_t *fmt8 = (void *)entbuf;
            if (field == PREFIXf) {
                fmt8->PREFIX_0_31 = fldbuf[0];
                fmt8->PREFIX_32_63 = fldbuf[1];
                fmt8->PREFIX_64_95 = fldbuf[2];
                fmt8->PREFIX_96_127 = fldbuf[3];
                fmt8->PREFIX_128 = fldbuf[4] & 0x1;
            } else if (field == ASSOC_DATAf) {
                fmt8->ASSOC_DATA_0_29 = fldbuf[0] & 0x3fffffff;
                fmt8->ASSOC_DATA_30_45 = ((fldbuf[1] & 0x3fff) << 2) | ((fldbuf[0] >> 30) & 0x3);
            } else if (field == ALPM2_DATAf) {
                fmt8->ALPM2_DATA_0_15 = fldbuf[0] & 0xffff;
                fmt8->ALPM2_DATA_16_47 = ((fldbuf[1] & 0xffff) << 16) | ((fldbuf[0] >> 16) & 0xffff);
                fmt8->ALPM2_DATA_48_55 = (fldbuf[1] >> 16) & 0xff;
            }
        }
        break;

    /* ROUTE format */
    case L3_DEFIP_ALPM_ROUTE_FMT1fmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT1_t *fmt1 = (void *)entbuf;
            if (field == PREFIXf) {
                fmt1->PREFIX = fldbuf[0] & 0xffff;
            } else if (field == ASSOC_DATAf) {
                fmt1->ASSOC_DATA = fldbuf[0] & 0xffff;
            }
        }
        break;
    case L3_DEFIP_ALPM_ROUTE_FMT2fmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT2_t *fmt2 = (void *)entbuf;
            if (field == PREFIXf) {
                fmt2->PREFIX = fldbuf[0] & 0xffffff;
            } else if (field == ASSOC_DATAf) {
                fmt2->ASSOC_DATA_0_7 = fldbuf[0] & 0xff;
                fmt2->ASSOC_DATA_8_15 = (fldbuf[0] >> 8) & 0xff;
            }
        }
        break;
    case L3_DEFIP_ALPM_ROUTE_FMT3fmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT3_t *fmt3 = (void *)entbuf;
            if (field == PREFIXf) {
                fmt3->PREFIX = fldbuf[0];
            } else if (field == ASSOC_DATAf) {
                fmt3->ASSOC_DATA = fldbuf[0] & 0xffff;
            }
        }
        break;
    case L3_DEFIP_ALPM_ROUTE_FMT4fmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT4_t *fmt4 = (void *)entbuf;
            if (field == PREFIXf) {
                fmt4->PREFIX_0_31 = fldbuf[0];
                fmt4->PREFIX_32_39 = fldbuf[1] & 0xff;
            } else if (field == ASSOC_DATAf) {
                fmt4->ASSOC_DATA = fldbuf[0] & 0xffff;
            }
        }
        break;
    case L3_DEFIP_ALPM_ROUTE_FMT5fmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT5_t *fmt5 = (void *)entbuf;
            if (field == PREFIXf) {
                fmt5->PREFIX_0_31 = fldbuf[0];
                fmt5->PREFIX_32_47 = fldbuf[1] & 0xffff;
            } else if (field == ASSOC_DATAf) {
                fmt5->ASSOC_DATA = fldbuf[0] & 0xffff;
            }
        }
        break;
    case L3_DEFIP_ALPM_ROUTE_FMT6fmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT6_t *fmt6 = (void *)entbuf;
            if (field == PREFIXf) {
                fmt6->PREFIX_0_31 = fldbuf[0];
                fmt6->PREFIX_32_63 = fldbuf[1];
            } else if (field == ASSOC_DATAf) {
                fmt6->ASSOC_DATA = fldbuf[0] & 0xffff;
            }
        }
        break;
    case L3_DEFIP_ALPM_ROUTE_FMT7fmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT7_t *fmt7 = (void *)entbuf;
            if (field == PREFIXf) {
                fmt7->PREFIX_0_31 = fldbuf[0];
                fmt7->PREFIX_32_63 = fldbuf[1];
                fmt7->PREFIX_64_79 = fldbuf[2] & 0xffff;
            } else if (field == ASSOC_DATAf) {
                fmt7->ASSOC_DATA = fldbuf[0] & 0xffff;
            }
        }
        break;
    case L3_DEFIP_ALPM_ROUTE_FMT8fmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT8_t *fmt8 = (void *)entbuf;
            if (field == PREFIXf) {
                fmt8->PREFIX_0_31 = fldbuf[0];
                fmt8->PREFIX_32_63 = fldbuf[1];
                fmt8->PREFIX_64_95 = fldbuf[2];
                fmt8->PREFIX_96_103 = fldbuf[3] & 0xff;
            } else if (field == ASSOC_DATAf) {
                fmt8->ASSOC_DATA = fldbuf[0] & 0xffff;
            }
        }
        break;
    case L3_DEFIP_ALPM_ROUTE_FMT9fmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT9_t *fmt9 = (void *)entbuf;
            if (field == PREFIXf) {
                fmt9->PREFIX_0_31 = fldbuf[0];
                fmt9->PREFIX_32_63 = fldbuf[1];
                fmt9->PREFIX_64_95 = fldbuf[2];
                fmt9->PREFIX_96_127 = fldbuf[3];
                fmt9->PREFIX_128 = fldbuf[4] & 0x1;
            } else if (field == ASSOC_DATAf) {
                fmt9->ASSOC_DATA = fldbuf[0] & 0xffff;
            }
        }
        break;
    case L3_DEFIP_ALPM_ROUTE_FMT10fmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT10_t *fmt10 = (void *)entbuf;
            if (field == PREFIXf) {
                fmt10->PREFIX_0_31 = fldbuf[0];
                fmt10->PREFIX_32_33 = fldbuf[1] & 0x3;
            } else if (field == ASSOC_DATAf) {
                fmt10->ASSOC_DATA_0_29 = fldbuf[0] & 0x3fffffff;
                fmt10->ASSOC_DATA_30_45 = ((fldbuf[1] & 0x3fff) << 2) | ((fldbuf[0] >> 30) & 0x3);
            }
        }
        break;
    case L3_DEFIP_ALPM_ROUTE_FMT11fmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT11_t *fmt11 = (void *)entbuf;
            if (field == PREFIXf) {
                fmt11->PREFIX_0_31 = fldbuf[0];
                fmt11->PREFIX_32_63 = fldbuf[1];
                fmt11->PREFIX_64_73 = fldbuf[2] & 0x3ff;
            } else if (field == ASSOC_DATAf) {
                fmt11->ASSOC_DATA_0_21 = fldbuf[0] & 0x3fffff;
                fmt11->ASSOC_DATA_22_45 = ((fldbuf[1] & 0x3fff) << 10) | ((fldbuf[0] >> 22) & 0x3ff);
            }
        }
        break;
    case L3_DEFIP_ALPM_ROUTE_FMT12fmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT12_t *fmt12 = (void *)entbuf;
            if (field == PREFIXf) {
                fmt12->PREFIX_0_31 = fldbuf[0];
                fmt12->PREFIX_32_63 = fldbuf[1];
                fmt12->PREFIX_64_95 = fldbuf[2];
                fmt12->PREFIX_96_127 = fldbuf[3];
                fmt12->PREFIX_128 = fldbuf[4] & 0x1;
            } else if (field == ASSOC_DATAf) {
                fmt12->ASSOC_DATA_0_29 = fldbuf[0] & 0x3fffffff;
                fmt12->ASSOC_DATA_30_45 = ((fldbuf[1] & 0x3fff) << 2) | ((fldbuf[0] >> 30) & 0x3);
            }
        }
        break;

    /* PIVOT FULL format */
    case L3_DEFIP_ALPM_PIVOT_FMT1_FULLfmt:
    case L3_DEFIP_ALPM_ROUTE_FMT6_FULLfmt:
    case L3_DEFIP_ALPM_ROUTE_FMT10_FULLfmt: {
            th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT1_FULL_t *fmt1pf = (void *)entbuf;
            if (field == ENTRY_0f) {
                fmt1pf->ENTRY_0_0_31 = fldbuf[0];
                fmt1pf->ENTRY_0_32_63 = fldbuf[1];
                fmt1pf->ENTRY_0_64_79 = fldbuf[2] & 0xffff;
            } else if (field == ENTRY_1f) {
                fmt1pf->ENTRY_1_0_15 = fldbuf[0] & 0xffff;
                fmt1pf->ENTRY_1_16_47 = ((fldbuf[1] & 0xffff) << 16) | ((fldbuf[0] >> 16) & 0xffff);
                fmt1pf->ENTRY_1_48_79 = ((fldbuf[2] & 0xffff) << 16) | ((fldbuf[1] >> 16) & 0xffff);
            } else if (field == ENTRY_2f) {
                fmt1pf->ENTRY_2_0_31 = fldbuf[0];
                fmt1pf->ENTRY_2_32_63 = fldbuf[1];
                fmt1pf->ENTRY_2_64_79 = fldbuf[2] & 0xffff;
            } else if (field == ENTRY_3f) {
                fmt1pf->ENTRY_3_0_15 = fldbuf[0] & 0xffff;
                fmt1pf->ENTRY_3_16_47 = ((fldbuf[1] & 0xffff) << 16) | ((fldbuf[0] >> 16) & 0xffff);
                fmt1pf->ENTRY_3_48_79 = ((fldbuf[2] & 0xffff) << 16) | ((fldbuf[1] >> 16) & 0xffff);
            } else if (field == ENTRY_4f) {
                fmt1pf->ENTRY_4_0_31 = fldbuf[0];
                fmt1pf->ENTRY_4_32_63 = fldbuf[1];
                fmt1pf->ENTRY_4_64_79 = fldbuf[2] & 0xffff;
            } else if (field == ENTRY_5f) {
                fmt1pf->ENTRY_5_0_15 = fldbuf[0] & 0xffff;
                fmt1pf->ENTRY_5_16_47 = ((fldbuf[1] & 0xffff) << 16) | ((fldbuf[0] >> 16) & 0xffff);
                fmt1pf->ENTRY_5_48_79 = ((fldbuf[2] & 0xffff) << 16) | ((fldbuf[1] >> 16) & 0xffff);
            }
        }
        break;

    case L3_DEFIP_ALPM_PIVOT_FMT2_FULLfmt:
    case L3_DEFIP_ALPM_ROUTE_FMT7_FULLfmt: {
            th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT2_FULL_t *fmt2pf = (void *)entbuf;
            if (field == ENTRY_0f) {
                fmt2pf->ENTRY_0_0_31 = fldbuf[0];
                fmt2pf->ENTRY_0_32_63 = fldbuf[1];
                fmt2pf->ENTRY_0_64_95 = fldbuf[2];
            } else if (field == ENTRY_1f) {
                fmt2pf->ENTRY_1_0_31 = fldbuf[0];
                fmt2pf->ENTRY_1_32_63 = fldbuf[1];
                fmt2pf->ENTRY_1_64_95 = fldbuf[2];
            } else if (field == ENTRY_2f) {
                fmt2pf->ENTRY_2_0_31 = fldbuf[0];
                fmt2pf->ENTRY_2_32_63 = fldbuf[1];
                fmt2pf->ENTRY_2_64_95 = fldbuf[2];
            } else if (field == ENTRY_3f) {
                fmt2pf->ENTRY_3_0_31 = fldbuf[0];
                fmt2pf->ENTRY_3_32_63 = fldbuf[1];
                fmt2pf->ENTRY_3_64_95 = fldbuf[2];
            } else if (field == ENTRY_4f) {
                fmt2pf->ENTRY_4_0_31 = fldbuf[0];
                fmt2pf->ENTRY_4_32_63 = fldbuf[1];
                fmt2pf->ENTRY_4_64_95 = fldbuf[2];
            }
        }
        break;

    case L3_DEFIP_ALPM_PIVOT_FMT3_FULLfmt:
    case L3_DEFIP_ALPM_PIVOT_FMT6_FULLfmt:
    case L3_DEFIP_ALPM_ROUTE_FMT8_FULLfmt:
    case L3_DEFIP_ALPM_ROUTE_FMT11_FULLfmt: {
            th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT3_FULL_t *fmt3pf = (void *)entbuf;
            if (field == ENTRY_0f) {
                fmt3pf->ENTRY_0_0_31 = fldbuf[0];
                fmt3pf->ENTRY_0_32_63 = fldbuf[1];
                fmt3pf->ENTRY_0_64_95 = fldbuf[2];
                fmt3pf->ENTRY_0_96_119 = fldbuf[3] & 0xffffff;
            } else if (field == ENTRY_1f) {
                fmt3pf->ENTRY_1_0_7 = fldbuf[0] & 0xff;
                fmt3pf->ENTRY_1_8_39 = ((fldbuf[1] & 0xff) << 24) | ((fldbuf[0] >> 8) & 0xffffff);
                fmt3pf->ENTRY_1_40_71 = ((fldbuf[2] & 0xff) << 24) | ((fldbuf[1] >> 8) & 0xffffff);
                fmt3pf->ENTRY_1_72_103 = ((fldbuf[3] & 0xff) << 24) | ((fldbuf[2] >> 8) & 0xffffff);
                fmt3pf->ENTRY_1_104_119 = ((fldbuf[3] >> 8) & 0xffff);
            } else if (field == ENTRY_2f) {
                fmt3pf->ENTRY_2_0_15 = fldbuf[0] & 0xffff;
                fmt3pf->ENTRY_2_16_47 = ((fldbuf[1] & 0xffff) << 16) | ((fldbuf[0] >> 16) & 0xffff);
                fmt3pf->ENTRY_2_48_79 = ((fldbuf[2] & 0xffff) << 16) | ((fldbuf[1] >> 16) & 0xffff);
                fmt3pf->ENTRY_2_80_111 = ((fldbuf[3] & 0xffff) << 16) | ((fldbuf[2] >> 16) & 0xffff);
                fmt3pf->ENTRY_2_112_119 = ((fldbuf[3] >> 16) & 0xff);
            } else if (field == ENTRY_3f) {
                fmt3pf->ENTRY_3_0_23 = fldbuf[0] & 0xffffff;
                fmt3pf->ENTRY_3_24_55 = ((fldbuf[1] & 0xffffff) << 8) | ((fldbuf[0] >> 24) & 0xff);
                fmt3pf->ENTRY_3_56_87 = ((fldbuf[2] & 0xffffff) << 8) | ((fldbuf[1] >> 24) & 0xff);
                fmt3pf->ENTRY_3_88_119 = ((fldbuf[3] & 0xffffff) << 8) | ((fldbuf[2] >> 24) & 0xff);
            }
        }
        break;

    case L3_DEFIP_ALPM_PIVOT_FMT4_FULLfmt:
    case L3_DEFIP_ALPM_PIVOT_FMT7_FULLfmt: {
            th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT4_FULL_t *fmt4pf = (void *)entbuf;
            if (field == ENTRY_0f) {
                fmt4pf->ENTRY_0_0_31 = fldbuf[0];
                fmt4pf->ENTRY_0_32_63 = fldbuf[1];
                fmt4pf->ENTRY_0_64_95 = fldbuf[2];
                fmt4pf->ENTRY_0_96_127 = fldbuf[3];
                fmt4pf->ENTRY_0_128_159 = fldbuf[4];
            } else if (field == ENTRY_1f) {
                fmt4pf->ENTRY_1_0_31 = fldbuf[0];
                fmt4pf->ENTRY_1_32_63 = fldbuf[1];
                fmt4pf->ENTRY_1_64_95 = fldbuf[2];
                fmt4pf->ENTRY_1_96_127 = fldbuf[3];
                fmt4pf->ENTRY_1_128_159 = fldbuf[4];
            } else if (field == ENTRY_2f) {
                fmt4pf->ENTRY_2_0_31 = fldbuf[0];
                fmt4pf->ENTRY_2_32_63 = fldbuf[1];
                fmt4pf->ENTRY_2_64_95 = fldbuf[2];
                fmt4pf->ENTRY_2_96_127 = fldbuf[3];
                fmt4pf->ENTRY_2_128_159 = fldbuf[4];
            }
        }
        break;

    case L3_DEFIP_ALPM_PIVOT_FMT5_FULLfmt: {
            th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT5_FULL_t *fmt5pf = (void *)entbuf;
            if (field == ENTRY_0f) {
                fmt5pf->ENTRY_0_0_31 = fldbuf[0];
                fmt5pf->ENTRY_0_32_63 = fldbuf[1];
                fmt5pf->ENTRY_0_64_95 = fldbuf[2];
                fmt5pf->ENTRY_0_96_127 = fldbuf[3];
                fmt5pf->ENTRY_0_128_159 = fldbuf[4];
                fmt5pf->ENTRY_0_160_191 = fldbuf[5];
                fmt5pf->ENTRY_0_192_207 = fldbuf[6] & 0xffff;
            } else if (field == ENTRY_1f) {
                fmt5pf->ENTRY_1_0_15 = fldbuf[0] & 0xffff;
                fmt5pf->ENTRY_1_16_47 = ((fldbuf[1] & 0xffff) << 16) | ((fldbuf[0] >> 16) & 0xffff);
                fmt5pf->ENTRY_1_48_79 = ((fldbuf[2] & 0xffff) << 16) | ((fldbuf[1] >> 16) & 0xffff);
                fmt5pf->ENTRY_1_80_111 = ((fldbuf[3] & 0xffff) << 16) | ((fldbuf[2] >> 16) & 0xffff);
                fmt5pf->ENTRY_1_112_143 = ((fldbuf[4] & 0xffff) << 16) | ((fldbuf[3] >> 16) & 0xffff);
                fmt5pf->ENTRY_1_144_175 = ((fldbuf[5] & 0xffff) << 16) | ((fldbuf[4] >> 16) & 0xffff);
                fmt5pf->ENTRY_1_176_207 = ((fldbuf[6] & 0xffff) << 16) | ((fldbuf[5] >> 16) & 0xffff);
            }
        }
        break;

    case L3_DEFIP_ALPM_PIVOT_FMT8_FULLfmt: {
            th3_fmt_L3_DEFIP_ALPM_PIVOT_FMT8_FULL_t *fmt8pf = (void *)entbuf;
            if (field == ENTRY_0f) {
                fmt8pf->ENTRY_0_0_31 = fldbuf[0];
                fmt8pf->ENTRY_0_32_63 = fldbuf[1];
                fmt8pf->ENTRY_0_64_95 = fldbuf[2];
                fmt8pf->ENTRY_0_96_127 = fldbuf[3];
                fmt8pf->ENTRY_0_128_159 = fldbuf[4];
                fmt8pf->ENTRY_0_160_191 = fldbuf[5];
                fmt8pf->ENTRY_0_192_223 = fldbuf[6];
                fmt8pf->ENTRY_0_224_231 = fldbuf[7] & 0xff;
            } else if (field == ENTRY_1f) {
                fmt8pf->ENTRY_1_0_23 = fldbuf[0] & 0xffffff;
                fmt8pf->ENTRY_1_24_55 = ((fldbuf[1] & 0xffffff) << 8) | ((fldbuf[0] >> 24) & 0xff);
                fmt8pf->ENTRY_1_56_87 = ((fldbuf[2] & 0xffffff) << 8) | ((fldbuf[1] >> 24) & 0xff);
                fmt8pf->ENTRY_1_88_119 = ((fldbuf[3] & 0xffffff) << 8) | ((fldbuf[2] >> 24) & 0xff);
                fmt8pf->ENTRY_1_120_151 = ((fldbuf[4] & 0xffffff) << 8) | ((fldbuf[3] >> 24) & 0xff);
                fmt8pf->ENTRY_1_152_183 = ((fldbuf[5] & 0xffffff) << 8) | ((fldbuf[4] >> 24) & 0xff);
                fmt8pf->ENTRY_1_184_215 = ((fldbuf[6] & 0xffffff) << 8) | ((fldbuf[5] >> 24) & 0xff);
                fmt8pf->ENTRY_1_216_231 = ((fldbuf[7] & 0xff) << 8) | ((fldbuf[6] >> 24) & 0xff);
            }
        }
        break;

    /* ROUTE FULL fmt */
    case L3_DEFIP_ALPM_ROUTE_FMT1_FULLfmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT1_FULL_t *fmt1rf = (void *)entbuf;
            if (field == ENTRY_0f) {
                fmt1rf->ENTRY_0 = fldbuf[0];
            } else if (field == ENTRY_1f) {
                fmt1rf->ENTRY_1 = fldbuf[0];
            } else if (field == ENTRY_2f) {
                fmt1rf->ENTRY_2 = fldbuf[0];
            } else if (field == ENTRY_3f) {
                fmt1rf->ENTRY_3 = fldbuf[0];
            } else if (field == ENTRY_4f) {
                fmt1rf->ENTRY_4 = fldbuf[0];
            } else if (field == ENTRY_5f) {
                fmt1rf->ENTRY_5 = fldbuf[0];
            } else if (field == ENTRY_6f) {
                fmt1rf->ENTRY_6 = fldbuf[0];
            } else if (field == ENTRY_7f) {
                fmt1rf->ENTRY_7 = fldbuf[0];
            } else if (field == ENTRY_8f) {
                fmt1rf->ENTRY_8 = fldbuf[0];
            } else if (field == ENTRY_9f) {
                fmt1rf->ENTRY_9 = fldbuf[0];
            } else if (field == ENTRY_10f) {
                fmt1rf->ENTRY_10 = fldbuf[0];
            } else if (field == ENTRY_11f) {
                fmt1rf->ENTRY_11 = fldbuf[0];
            } else if (field == ENTRY_12f) {
                fmt1rf->ENTRY_12 = fldbuf[0];
            } else if (field == ENTRY_13f) {
                fmt1rf->ENTRY_13 = fldbuf[0];
            } else if (field == ENTRY_14f) {
                fmt1rf->ENTRY_14 = fldbuf[0];
            }
        }
        break;

    case L3_DEFIP_ALPM_ROUTE_FMT2_FULLfmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT2_FULL_t *fmt2rf = (void *)entbuf;
            if (field == ENTRY_0f) {
                fmt2rf->ENTRY_0_0_31 = fldbuf[0];
                fmt2rf->ENTRY_0_32_39 = fldbuf[1] & 0xff;
            } else if (field == ENTRY_1f) {
                fmt2rf->ENTRY_1_0_23 = fldbuf[0] & 0xffffff;
                fmt2rf->ENTRY_1_24_39 = ((fldbuf[1] & 0xff) << 8) | ((fldbuf[0] >> 24) & 0xff);
            } else if (field == ENTRY_2f) {
                fmt2rf->ENTRY_2_0_15 = fldbuf[0] & 0xffff;
                fmt2rf->ENTRY_2_16_39 = ((fldbuf[1] & 0xff) << 16) | ((fldbuf[0] >> 16) & 0xffff);
            } else if (field == ENTRY_3f) {
                fmt2rf->ENTRY_3_0_7 = fldbuf[0] & 0xff;
                fmt2rf->ENTRY_3_8_39 = ((fldbuf[1] & 0xff) << 24) | ((fldbuf[0] >> 8) & 0xffffff);
            } else if (field == ENTRY_4f) {
                fmt2rf->ENTRY_4_0_31 = fldbuf[0];
                fmt2rf->ENTRY_4_32_39 = fldbuf[1] & 0xff;
            } else if (field == ENTRY_5f) {
                fmt2rf->ENTRY_5_0_23 = fldbuf[0] & 0xffffff;
                fmt2rf->ENTRY_5_24_39 = ((fldbuf[1] & 0xff) << 8) | ((fldbuf[0] >> 24) & 0xff);
            } else if (field == ENTRY_6f) {
                fmt2rf->ENTRY_6_0_15 = fldbuf[0] & 0xffff;
                fmt2rf->ENTRY_6_16_39 = ((fldbuf[1] & 0xff) << 16) | ((fldbuf[0] >> 16) & 0xffff);
            } else if (field == ENTRY_7f) {
                fmt2rf->ENTRY_7_0_7 = fldbuf[0] & 0xff;
                fmt2rf->ENTRY_7_8_39 = ((fldbuf[1] & 0xff) << 24) | ((fldbuf[0] >> 8) & 0xffffff);
            } else if (field == ENTRY_8f) {
                fmt2rf->ENTRY_8_0_31 = fldbuf[0];
                fmt2rf->ENTRY_8_32_39 = fldbuf[1] & 0xff;
            } else if (field == ENTRY_9f) {
                fmt2rf->ENTRY_9_0_23 = fldbuf[0] & 0xffffff;
                fmt2rf->ENTRY_9_24_39 = ((fldbuf[1] & 0xff) << 8) | ((fldbuf[0] >> 24) & 0xff);
            } else if (field == ENTRY_10f) {
                fmt2rf->ENTRY_10_0_15 = fldbuf[0] & 0xffff;
                fmt2rf->ENTRY_10_16_39 = ((fldbuf[1] & 0xff) << 16) | ((fldbuf[0] >> 16) & 0xffff);
            } else if (field == ENTRY_11f) {
                fmt2rf->ENTRY_11_0_7 = fldbuf[0] & 0xff;
                fmt2rf->ENTRY_11_8_39 = ((fldbuf[1] & 0xff) << 24) | ((fldbuf[0] >> 8) & 0xffffff);
            }
        }
        break;

    case L3_DEFIP_ALPM_ROUTE_FMT3_FULLfmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT3_FULL_t *fmt3rf = (void *)entbuf;
            if (field == ENTRY_0f) {
                fmt3rf->ENTRY_0_0_31 = fldbuf[0];
                fmt3rf->ENTRY_0_32_47 = fldbuf[1] & 0xff;
            } else if (field == ENTRY_1f) {
                fmt3rf->ENTRY_1_0_15 = fldbuf[0] & 0xffff;
                fmt3rf->ENTRY_1_16_47 = ((fldbuf[1] & 0xffff) << 16) | ((fldbuf[0] >> 16) & 0xffff);
            } else if (field == ENTRY_2f) {
                fmt3rf->ENTRY_2_0_31 = fldbuf[0];
                fmt3rf->ENTRY_2_32_47 = fldbuf[1] & 0xff;
            } else if (field == ENTRY_3f) {
                fmt3rf->ENTRY_3_0_15 = fldbuf[0] & 0xffff;
                fmt3rf->ENTRY_3_16_47 = ((fldbuf[1] & 0xffff) << 16) | ((fldbuf[0] >> 16) & 0xffff);
            } else if (field == ENTRY_4f) {
                fmt3rf->ENTRY_4_0_31 = fldbuf[0];
                fmt3rf->ENTRY_4_32_47 = fldbuf[1] & 0xff;
            } else if (field == ENTRY_5f) {
                fmt3rf->ENTRY_5_0_15 = fldbuf[0] & 0xffff;
                fmt3rf->ENTRY_5_16_47 = ((fldbuf[1] & 0xffff) << 16) | ((fldbuf[0] >> 16) & 0xffff);
            } else if (field == ENTRY_6f) {
                fmt3rf->ENTRY_6_0_31 = fldbuf[0];
                fmt3rf->ENTRY_6_32_47 = fldbuf[1] & 0xff;
            } else if (field == ENTRY_7f) {
                fmt3rf->ENTRY_7_0_15 = fldbuf[0] & 0xffff;
                fmt3rf->ENTRY_7_16_47 = ((fldbuf[1] & 0xffff) << 16) | ((fldbuf[0] >> 16) & 0xffff);
            } else if (field == ENTRY_8f) {
                fmt3rf->ENTRY_8_0_31 = fldbuf[0];
                fmt3rf->ENTRY_8_32_47 = fldbuf[1] & 0xff;
            } else if (field == ENTRY_9f) {
                fmt3rf->ENTRY_9_0_15 = fldbuf[0] & 0xffff;
                fmt3rf->ENTRY_9_16_47 = ((fldbuf[1] & 0xffff) << 16) | ((fldbuf[0] >> 16) & 0xffff);
            }
        }
        break;

    case L3_DEFIP_ALPM_ROUTE_FMT4_FULLfmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT4_FULL_t *fmt4rf = (void *)entbuf;
            if (field == ENTRY_0f) {
                fmt4rf->ENTRY_0_0_31 = fldbuf[0];
                fmt4rf->ENTRY_0_32_55 = fldbuf[1] & 0xffffff;
            } else if (field == ENTRY_1f) {
                fmt4rf->ENTRY_1_0_7 = fldbuf[0] & 0xff;
                fmt4rf->ENTRY_1_8_39 = ((fldbuf[1] & 0xff) << 24) | ((fldbuf[0] >> 8) & 0xffffff);
                fmt4rf->ENTRY_1_40_55 = ((fldbuf[1] >> 8) & 0xffff);
            } else if (field == ENTRY_2f) {
                fmt4rf->ENTRY_2_0_15 = fldbuf[0] & 0xffff;
                fmt4rf->ENTRY_2_16_47 = ((fldbuf[1] & 0xffff) << 16) | ((fldbuf[0] >> 16) & 0xffff);
                fmt4rf->ENTRY_2_48_55 = (fldbuf[1] >> 16) & 0xff;
            } else if (field == ENTRY_3f) {
                fmt4rf->ENTRY_3_0_23 = fldbuf[0] & 0xffffff;
                fmt4rf->ENTRY_3_24_55 = ((fldbuf[1] & 0xffffff) << 8) | ((fldbuf[0] >> 24) & 0xff);
            } else if (field == ENTRY_4f) {
                fmt4rf->ENTRY_4_0_31 = fldbuf[0];
                fmt4rf->ENTRY_4_32_55 = fldbuf[1] & 0xffffff;
            } else if (field == ENTRY_5f) {
                fmt4rf->ENTRY_5_0_7 = fldbuf[0] & 0xff;
                fmt4rf->ENTRY_5_8_39 = ((fldbuf[1] & 0xff) << 24) | ((fldbuf[0] >> 8) & 0xffffff);
                fmt4rf->ENTRY_5_40_55 = ((fldbuf[1] >> 8) & 0xffff);
            } else if (field == ENTRY_6f) {
                fmt4rf->ENTRY_6_0_15 = fldbuf[0] & 0xffff;
                fmt4rf->ENTRY_6_16_47 = ((fldbuf[1] & 0xffff) << 16) | ((fldbuf[0] >> 16) & 0xffff);
                fmt4rf->ENTRY_6_48_55 = (fldbuf[1] >> 16) & 0xff;
            } else if (field == ENTRY_7f) {
                fmt4rf->ENTRY_7_0_23 = fldbuf[0] & 0xffffff;
                fmt4rf->ENTRY_7_24_55 = ((fldbuf[1] & 0xffffff) << 8) | ((fldbuf[0] >> 24) & 0xff);
            }
        }
        break;

    case L3_DEFIP_ALPM_ROUTE_FMT5_FULLfmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT5_FULL_t *fmt5rf = (void *)entbuf;
            if (field == ENTRY_0f) {
                fmt5rf->ENTRY_0_0_31 = fldbuf[0];
                fmt5rf->ENTRY_0_32_63 = fldbuf[1];
            } else if (field == ENTRY_1f) {
                fmt5rf->ENTRY_1_0_31 = fldbuf[0];
                fmt5rf->ENTRY_1_32_63 = fldbuf[1];
            } else if (field == ENTRY_2f) {
                fmt5rf->ENTRY_2_0_31 = fldbuf[0];
                fmt5rf->ENTRY_2_32_63 = fldbuf[1];
            } else if (field == ENTRY_3f) {
                fmt5rf->ENTRY_3_0_31 = fldbuf[0];
                fmt5rf->ENTRY_3_32_63 = fldbuf[1];
            } else if (field == ENTRY_4f) {
                fmt5rf->ENTRY_4_0_31 = fldbuf[0];
                fmt5rf->ENTRY_4_32_63 = fldbuf[1];
            } else if (field == ENTRY_5f) {
                fmt5rf->ENTRY_5_0_31 = fldbuf[0];
                fmt5rf->ENTRY_5_32_63 = fldbuf[1];
            } else if (field == ENTRY_6f) {
                fmt5rf->ENTRY_6_0_31 = fldbuf[0];
                fmt5rf->ENTRY_6_32_63 = fldbuf[1];
            }
        }
        break;

    case L3_DEFIP_ALPM_ROUTE_FMT9_FULLfmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT9_FULL_t *fmt9rf = (void *)entbuf;
            if (field == ENTRY_0f) {
                fmt9rf->ENTRY_0_0_31 = fldbuf[0];
                fmt9rf->ENTRY_0_32_63 = fldbuf[1];
                fmt9rf->ENTRY_0_64_95 = fldbuf[2];
                fmt9rf->ENTRY_0_96_127 = fldbuf[3];
                fmt9rf->ENTRY_0_128_151 = fldbuf[4] & 0xffffff;
            } else if (field == ENTRY_1f) {
                fmt9rf->ENTRY_1_0_7 = fldbuf[0] & 0xff;
                fmt9rf->ENTRY_1_8_39 = ((fldbuf[1] & 0xff) << 24) | ((fldbuf[0] >> 8) & 0xffffff);
                fmt9rf->ENTRY_1_40_71 = ((fldbuf[2] & 0xff) << 24) | ((fldbuf[1] >> 8) & 0xffffff);
                fmt9rf->ENTRY_1_72_103 = ((fldbuf[3] & 0xff) << 24) | ((fldbuf[2] >> 8) & 0xffffff);
                fmt9rf->ENTRY_1_104_135 = ((fldbuf[4] & 0xff) << 24) | ((fldbuf[3] >> 8) & 0xffffff);
                fmt9rf->ENTRY_1_136_151 = (fldbuf[4] >> 8) & 0xffff;
            } else if (field == ENTRY_2f) {
                fmt9rf->ENTRY_2_0_15 = fldbuf[0] & 0xffff;
                fmt9rf->ENTRY_2_16_47 = ((fldbuf[1] & 0xffff) << 16) | ((fldbuf[0] >> 16) & 0xffff);
                fmt9rf->ENTRY_2_48_79 = ((fldbuf[2] & 0xffff) << 16) | ((fldbuf[1] >> 16) & 0xffff);
                fmt9rf->ENTRY_2_80_111 = ((fldbuf[3] & 0xffff) << 16) | ((fldbuf[2] >> 16) & 0xffff);
                fmt9rf->ENTRY_2_112_143 = ((fldbuf[4] & 0xffff) << 16) | ((fldbuf[3] >> 16) & 0xffff);
                fmt9rf->ENTRY_2_144_151 = (fldbuf[4] >> 16) & 0xff;
            }
        }
        break;

    case L3_DEFIP_ALPM_ROUTE_FMT12_FULLfmt: {
            th3_fmt_L3_DEFIP_ALPM_ROUTE_FMT12_FULL_t *fmt12rf = (void *)entbuf;
            if (field == ENTRY_0f) {
                fmt12rf->ENTRY_0_0_31 = fldbuf[0];
                fmt12rf->ENTRY_0_32_63 = fldbuf[1];
                fmt12rf->ENTRY_0_64_95 = fldbuf[2];
                fmt12rf->ENTRY_0_96_127 = fldbuf[3];
                fmt12rf->ENTRY_0_128_159 = fldbuf[4];
                fmt12rf->ENTRY_0_160_175 = fldbuf[5] & 0xffff;
            } else if (field == ENTRY_1f) {
                fmt12rf->ENTRY_1_0_15 = fldbuf[0] & 0xffff;
                fmt12rf->ENTRY_1_16_47 = ((fldbuf[1] & 0xffff) << 16) | ((fldbuf[0] >> 16) & 0xffff);
                fmt12rf->ENTRY_1_48_79 = ((fldbuf[2] & 0xffff) << 16) | ((fldbuf[1] >> 16) & 0xffff);
                fmt12rf->ENTRY_1_80_111 = ((fldbuf[3] & 0xffff) << 16) | ((fldbuf[2] >> 16) & 0xffff);
                fmt12rf->ENTRY_1_112_143 = ((fldbuf[4] & 0xffff) << 16) | ((fldbuf[3] >> 16) & 0xffff);
                fmt12rf->ENTRY_1_144_175 = ((fldbuf[5] & 0xffff) << 16) | ((fldbuf[4] >> 16) & 0xffff);
            }
        }
        break;

    default:
        assert(0);
        break;
    }
}
#endif

typedef struct {
#ifdef  LE_HOST
    uint32  VLAN_ID:12,
            __PAD0:2,
            PORT_NUM:8,
            COPY_TO_CPU:1,
            DROP:1,
            ENTRY_TYPE:2,
            T:1,
            __PAD1:5;
#else
    uint32  __PAD1:5,
            T:1,
            ENTRY_TYPE:2,
            DROP:1,
            COPY_TO_CPU:1,
            PORT_NUM:8,
            __PAD0:2,
            VLAN_ID:12;
#endif  /* LE_HOST */

    uint32 __WORD_PAD0;
} th3_mem_ING_L3_NEXT_HOP_t;

typedef struct {
    /* Word0: bit 0 - bit 31 */
#ifdef  LE_HOST
    uint32  ENTRY_TYPE:2,
            __PAD0:8,
            L3__CLASS_ID:7,
            __PAD1:5,
            IFP_ACTIONS__L3_UC_DA_DISABLE:1,
            IFP_ACTIONS__L3_UC_SA_DISABLE:1,
            IFP_ACTIONS__L3_UC_TTL_DISABLE:1,
            IFP_ACTIONS__L3_UC_VLAN_DISABLE:1,
            __PAD2:6;
#else
    uint32  __PAD2:6,
            IFP_ACTIONS__L3_UC_VLAN_DISABLE:1,
            IFP_ACTIONS__L3_UC_TTL_DISABLE:1,
            IFP_ACTIONS__L3_UC_SA_DISABLE:1,
            IFP_ACTIONS__L3_UC_DA_DISABLE:1,
            __PAD1:5,
            L3__CLASS_ID:7,
            __PAD0:8,
            ENTRY_TYPE:2;
#endif  /* LE_HOST */

    /* Word1: bit 32 - bit 63 */
#ifdef  LE_HOST
    uint32  __PAD3:1,
            L3__L3_UC_DA_DISABLE:1,
            L3__L3_UC_SA_DISABLE:1,
            L3__L3_UC_TTL_DISABLE:1,
            L3__L3_UC_VLAN_DISABLE:1,
            __PAD4:11,
            MPLS__MAC_DA_PROFILE_INDEX:9,
            __PAD5:5,
            MPLS__MPLS_LABEL_0_1:2;
#else
    uint32  MPLS__MPLS_LABEL_0_1:2,
            __PAD5:5,
            MPLS__MAC_DA_PROFILE_INDEX:9,
            __PAD4:11,
            L3__L3_UC_VLAN_DISABLE:1,
            L3__L3_UC_TTL_DISABLE:1,
            L3__L3_UC_SA_DISABLE:1,
            L3__L3_UC_DA_DISABLE:1,
            __PAD3:1;
#endif  /* LE_HOST */

    /* Word2: bit 64 - bit 95 */
#ifdef  LE_HOST
    uint32  MPLS__MPLS_LABEL_2_19:18,
            __PAD6:14;
#else
    uint32  __PAD6:14,
            MPLS__MPLS_LABEL_2_19:18;
#endif  /* LE_HOST */

    /* Word3 */
    uint32 __WORD_PAD0;
} th3_mem_EGR_L3_NEXT_HOP_t;

typedef struct {
#ifdef  LE_HOST
    uint32  INTF_NUM:13,
            __PAD0:19;
#else
    uint32  __PAD0:19,
            INTF_NUM:13;
#endif  /* LE_HOST */
} th3_mem_EGR_L3_NEXT_HOP_2_t;

#endif /* _BCM_INT_ALPM_TH3_ACC_H_ */

